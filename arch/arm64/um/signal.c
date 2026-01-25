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
#include <asm/ucontext.h>
#include <asm/sigframe.h>
#include <frame_kern.h>
#include <registers.h>
#include <skas.h>

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
	struct sigcontext sc;
	struct fpsimd_context __user *fpsimd_user;
	int err, i;

	/* Always make any pending restarted system calls return -EINTR */
	current->restart_block.fn = do_no_restart_syscall;

	err = copy_from_user(&sc, from, sizeof(sc));
	if (err)
		return err;

	/* Copy general purpose registers x0-x30 */
	for (i = 0; i < 31; i++)
		regs->regs.gp[i] = sc.regs[i];

	/* Copy special registers */
	regs->regs.gp[HOST_SP] = sc.sp;
	regs->regs.gp[HOST_PC] = sc.pc;
	regs->regs.gp[HOST_PSTATE] = sc.pstate;

	/* Copy FP/SIMD state from __reserved area */
	fpsimd_user = (void __user *)&from->__reserved;
	err = copy_from_user(regs->regs.fp, fpsimd_user, host_fp_size);
	if (err)
		return 1;

	return 0;
}

/*
 * Copy kernel pt_regs to user sigcontext
 * Used when delivering a signal
 */
static int copy_sc_to_user(struct sigcontext __user *to,
			   struct pt_regs *regs,
			   unsigned long mask)
{
	struct sigcontext sc;
	struct fpsimd_context __user *fpsimd_user;
	struct faultinfo *fi = &current->thread.arch.faultinfo;
	int err, i;

	memset(&sc, 0, sizeof(struct sigcontext));

	/* Copy general purpose registers x0-x30 */
	for (i = 0; i < 31; i++)
		sc.regs[i] = regs->regs.gp[i];

	/* Copy special registers */
	sc.sp = regs->regs.gp[HOST_SP];
	sc.pc = regs->regs.gp[HOST_PC];
	sc.pstate = regs->regs.gp[HOST_PSTATE];

	/* Store fault information */
	sc.fault_address = fi->cr2;  /* FAR_EL1 */

	/* Copy sigcontext to user */
	err = copy_to_user(to, &sc, sizeof(sc));
	if (err)
		return 1;

	/* Copy FP/SIMD state to __reserved area */
	fpsimd_user = (void __user *)&to->__reserved;
	err = copy_to_user(fpsimd_user, regs->regs.fp, host_fp_size);
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
	unsigned long restorer;
	int err = 0;
	u32 __user *retcode;

#define ARM64_RT_SIGRETURN_MOV_X8 \
	(0xd2800000 | ((__NR_rt_sigreturn & 0xffff) << 5) | HOST_X8)
#define ARM64_RT_SIGRETURN_SVC 0xd4000001

	/* Allocate space for signal frame on stack */
	sp = stack_top - sizeof(struct rt_sigframe);

	/* Ensure 16-byte alignment */
	sp &= ~15UL;

	frame = (struct rt_sigframe __user *)sp;
	restorer = sp + sizeof(struct rt_sigframe);

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
	err |= copy_sc_to_user(&frame->uc.uc_mcontext, regs, set->sig[0]);

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
	 * Set up return trampoline (restorer)
	 * On ARM64, the restorer calls rt_sigreturn syscall
	 */
	if (ksig->ka.sa.sa_flags & SA_RESTORER)
		regs->regs.gp[HOST_LR] = (unsigned long)ksig->ka.sa.sa_restorer;
	else {
		retcode = (u32 __user *)restorer;
		err |= __put_user(ARM64_RT_SIGRETURN_MOV_X8, &retcode[0]);
		err |= __put_user(ARM64_RT_SIGRETURN_SVC, &retcode[1]);
		if (err)
			return err;
		regs->regs.gp[HOST_LR] = restorer;
	}

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
int arch_do_signal_or_restart(struct pt_regs *regs, bool has_signal)
{
	/* ARM64-specific signal restart logic if needed */
	return 0;
}
