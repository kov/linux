// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/signal.c and arch/arm64/kernel/signal.c
 */

#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/regset.h>
#include <asm/unistd.h>
#include <asm/user.h>
#include <asm/ucontext.h>
#include <asm/sigframe.h>
#include <generated/vdso-offsets.h>
#include <frame_kern.h>
#include <registers.h>
#include <skas.h>

extern unsigned long um_vdso_addr;

/* Prototypes for functions defined in this file to satisfy -Wmissing-prototypes */
int setup_signal_stack(unsigned long stack_top, struct ksignal *ksig,
		       struct pt_regs *regs, sigset_t *set);
int setup_signal_stack_si(unsigned long stack_top, struct ksignal *ksig,
			  struct pt_regs *regs, sigset_t *set);
long sys_sigreturn(void);

/*
 * ARM64 signal frame structures
 *
 * ARM64 only uses RT signal frames (no legacy frames like x86)
 * Layout:
 *   struct rt_sigframe {
 *       struct siginfo info;
 *       struct ucontext uc;
 *   };
 *
 * The ucontext contains:
 *   - uc_mcontext: register state (x0-x30, sp, pc, pstate)
 *   - uc_sigmask: signal mask
 *   - __reserved: FP/SIMD state
 */

/*
 * Note: struct fpsimd_context is defined in native ARM64 headers
 * (arch/arm64/include/uapi/asm/sigcontext.h), we use that definition
 */

/*
 * Copy sigcontext from user to kernel pt_regs
 * Used when returning from a signal handler
 */
static int copy_sc_from_user(struct pt_regs *regs,
			     struct sigcontext __user *from)
{
	struct fpsimd_context __user *fpsimd_user;
	struct _aarch64_ctx head;
	int err, i;

	/* Always make any pending restarted system calls return -EINTR */
	current->restart_block.fn = do_no_restart_syscall;

	/* Copy general purpose registers x0-x30 */
	for (i = 0; i < 31; i++) {
		err = __get_user(regs->regs.gp[i], &from->regs[i]);
		if (err)
			return err;
	}

	/* Copy special registers */
	err = __get_user(regs->regs.gp[HOST_SP], &from->sp);
	err |= __get_user(regs->regs.gp[HOST_PC], &from->pc);
	err |= __get_user(regs->regs.gp[HOST_PSTATE], &from->pstate);
	if (err)
		return err;

	/*
	 * Copy FP/SIMD state from __reserved area
	 * We look for FPSIMD_MAGIC. If not found, we don't restore FP.
	 * This is a simplified parser - it expects FPSIMD at the start.
	 */
	fpsimd_user = (void __user *)&from->__reserved;
	err = copy_from_user(&head, &fpsimd_user->head, sizeof(head));
	if (err)
		return 1;

	if (head.magic == FPSIMD_MAGIC &&
	    head.size == sizeof(struct fpsimd_context)) {
		struct user_fpsimd_state *kfp =
			(struct user_fpsimd_state *)regs->regs.fp;

		err = __get_user(kfp->fpsr, &fpsimd_user->fpsr);
		err |= __get_user(kfp->fpcr, &fpsimd_user->fpcr);
		err |= copy_from_user(kfp->vregs, fpsimd_user->vregs,
				      sizeof(kfp->vregs));

		if (err)
			return 1;
	}

	return 0;
}

#ifndef ESR_MAGIC
#define ESR_MAGIC 0x45535201
#endif

/*
 * Copy kernel pt_regs to user sigcontext
 * Used when delivering a signal
 */
static int copy_sc_to_user(struct sigcontext __user *to, struct pt_regs *regs,
			   unsigned long mask, int sig)
{
	struct fpsimd_context __user *fpsimd_user;
	struct _aarch64_ctx terminator = { 0, 0 };
	struct faultinfo *fi = &current->thread.arch.faultinfo;
	struct user_fpsimd_state *kfp =
		(struct user_fpsimd_state *)regs->regs.fp;
	int err, i;

	/*
	 * We don't copy the whole sigcontext to stack because it is >4KB.
	 * Instead, we write fields individually and clear the reserved area.
	 */

	/* Copy general purpose registers x0-x30 */
	for (i = 0; i < 31; i++) {
		err = __put_user(regs->regs.gp[i], &to->regs[i]);
		if (err)
			return err;
	}

	/* Copy special registers */
	err = __put_user(regs->regs.gp[HOST_SP], &to->sp);
	err |= __put_user(regs->regs.gp[HOST_PC], &to->pc);
	err |= __put_user(regs->regs.gp[HOST_PSTATE], &to->pstate);

	/* Store fault information */
	err |= __put_user(fi->cr2, &to->fault_address);  /* FAR_EL1 */

	if (err)
		return 1;

	/*
	 * Copy FP/SIMD state to __reserved area
	 * We must create a valid fpsimd_context with magic and size
	 */
	fpsimd_user = (void __user *)&to->__reserved;

	/* Write Magic and Size */
	err = __put_user(FPSIMD_MAGIC, &fpsimd_user->head.magic);
	err |= __put_user(sizeof(struct fpsimd_context),
			  &fpsimd_user->head.size);

	/* Copy Registers: Map user_fpsimd_state to fpsimd_context layout */
	err |= __put_user(kfp->fpsr, &fpsimd_user->fpsr);
	err |= __put_user(kfp->fpcr, &fpsimd_user->fpcr);
	err |= copy_to_user(fpsimd_user->vregs, kfp->vregs, sizeof(kfp->vregs));

	/*
	 * Append ESR context for synchronous signals
	 * This is critical for glibc to handle SIGILL probes correctly
	 */
	if (sig == SIGILL || sig == SIGSEGV || sig == SIGBUS ||
	    sig == SIGTRAP) {
		struct esr_context esr_ctx;
		void __user *next = (void __user *)(fpsimd_user + 1);

		esr_ctx.head.magic = ESR_MAGIC;
		esr_ctx.head.size = sizeof(struct esr_context);
		esr_ctx.esr = ((unsigned long)fi->trap_no << 26) |
			      fi->error_code;

		err |= copy_to_user(next, &esr_ctx, sizeof(esr_ctx));
		next += sizeof(struct esr_context);

		/* Write Terminator */
		err |= copy_to_user(next, &terminator, sizeof(terminator));
	} else {
		/* Write Terminator immediately after FPSIMD */
		err |= copy_to_user((void __user *)(fpsimd_user + 1),
				    &terminator, sizeof(terminator));
	}

	if (err)
		return 1;

	return 0;
}

/*
 * Set up a signal frame
 *
 * ARM64 signal frames are always RT frames, stack layout:
 *   [higher addresses]
 *   ... (previous stack content)
 *   struct rt_sigframe
 *   [lower addresses - SP points here]
 *
 * Stack must be 16-byte aligned
 */
int setup_signal_stack_si(unsigned long stack_top, struct ksignal *ksig,
			  struct pt_regs *regs, sigset_t *set)
{
	struct rt_sigframe __user *frame;
	unsigned long sp;
	int err = 0;

	/* Allocate space for signal frame on stack */
	sp = stack_top - sizeof(struct rt_sigframe);

	/* Ensure 16-byte alignment */
	sp &= ~15UL;

	frame = (struct rt_sigframe __user *)sp;

	/* Check frame is accessible */
	if (!access_ok(frame, sizeof(*frame)))
		return 1;

	/* Setup siginfo */
	if (ksig->ka.sa.sa_flags & SA_SIGINFO) {
		err |= copy_siginfo_to_user(&frame->info, &ksig->info);
		if (err)
			return err;
	}

	/* Setup ucontext */
	err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(NULL, &frame->uc.uc_link);
	err |= __save_altstack(&frame->uc.uc_stack, PT_REGS_SP(regs));

	/* Copy register state to signal frame */
	err |= copy_sc_to_user(&frame->uc.uc_mcontext, regs, set->sig[0],
			       ksig->sig);

	/* Set signal mask */
	err |= __copy_to_user(&frame->uc.uc_sigmask, set, sizeof(*set));

	if (err)
		return err;

	/*
	 * Set up registers for signal handler entry:
	 *   x0 = signal number
	 *   x1 = pointer to siginfo (if SA_SIGINFO)
	 *   x2 = pointer to ucontext
	 *   pc = signal handler address
	 *   sp = frame pointer
	 *   x30 (LR) = return trampoline address (restorer)
	 */
	regs->regs.gp[HOST_X0] = ksig->sig;
	PT_REGS_SP(regs) = (unsigned long)frame;
	PT_REGS_IP(regs) = (unsigned long)ksig->ka.sa.sa_handler;

	if (ksig->ka.sa.sa_flags & SA_SIGINFO) {
		regs->regs.gp[HOST_X1] = (unsigned long)&frame->info;
		regs->regs.gp[HOST_X2] = (unsigned long)&frame->uc;
	} else {
		regs->regs.gp[HOST_X1] = 0;
		regs->regs.gp[HOST_X2] = 0;
	}

	/*
	 * Set up return trampoline (restorer).
	 *
	 * If the caller specified SA_RESTORER, use that. Otherwise use
	 * the sigreturn trampoline in the VDSO (the same approach as
	 * native arm64).  An on-stack trampoline does not work because
	 * modern stacks are non-executable.
	 */
	if (ksig->ka.sa.sa_flags & SA_RESTORER)
		regs->regs.gp[HOST_LR] = (unsigned long)ksig->ka.sa.sa_restorer;
	else
		regs->regs.gp[HOST_LR] = um_vdso_addr + vdso_offset_sigtramp;

	return 0;
}

/*
 * Main entry point for signal delivery
 */
int setup_signal_stack(unsigned long stack_top, struct ksignal *ksig,
		       struct pt_regs *regs, sigset_t *set)
{
	return setup_signal_stack_si(stack_top, ksig, regs, set);
}

/*
 * sys_rt_sigreturn - return from signal handler
 *
 * Called when signal handler returns via restorer
 * Restores register state from signal frame
 */
SYSCALL_DEFINE0(rt_sigreturn)
{
	struct pt_regs *regs = current_pt_regs();
	struct rt_sigframe __user *frame;
	sigset_t set;

	/* Signal frame is at current stack pointer */
	frame = (struct rt_sigframe __user *)PT_REGS_SP(regs);

	if (!access_ok(frame, sizeof(*frame)))
		goto badframe;

	/* Restore signal mask */
	if (__copy_from_user(&set, &frame->uc.uc_sigmask, sizeof(set)))
		goto badframe;

	set_current_blocked(&set);

	/* Restore register state from signal frame */
	if (copy_sc_from_user(regs, &frame->uc.uc_mcontext))
		goto badframe;

	/* Restore alternate signal stack */
	if (restore_altstack(&frame->uc.uc_stack))
		goto badframe;

	return regs->regs.gp[HOST_X0];

badframe:
	force_sig(SIGSEGV);
	return 0;
}

/*
 * ARM64 uses only RT signals (no legacy signal handling)
 */
asmlinkage long __arm64_sys_rt_sigreturn(const struct pt_regs *regs);

long sys_sigreturn(void)
{
	return __arm64_sys_rt_sigreturn(current_pt_regs());
}

/*
 * Determine if we're in a syscall
 * Used to decide if we need to restart syscalls
 */
int arch_do_signal_or_restart(struct pt_regs *regs, int has_signal)
{
	/* ARM64-specific signal restart logic if needed */
	return 0;
}
