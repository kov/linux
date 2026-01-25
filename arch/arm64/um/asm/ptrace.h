/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_PTRACE_H
#define __UM_ARM64_PTRACE_H

/*
 * This header prevents inclusion of native ARM64 ptrace.h
 * UML uses its own ptrace interface via arch/um/include/asm/ptrace-generic.h
 *
 * DO NOT include native arch/arm64/include/asm/ptrace.h
 */

#include <linux/errno.h>
#include <asm/ptrace-abi.h>

/* PSR (PSTATE) mode definitions */
#define PSR_MODE_EL0t   0x00000000
#define PSR_MODE_EL1t   0x00000004
#define PSR_MODE_EL1h   0x00000005
#define PSR_MODE_MASK   0x0000000f
#define PSR_MODE32_BIT  0x00000010

/* Include UML generic ptrace instead */
#include <asm/ptrace-generic.h>

/*
 * user_mode - check if running in user mode
 * @regs: pt_regs pointer
 *
 * Returns 1 if in user mode, 0 otherwise
 */
#define user_mode(r) UPT_IS_USER(&(r)->regs)

/*
 * PT_REGS register access macros
 */
#define PT_REGS_X0(r) UPT_X0(&(r)->regs)

/*
 * PT_REGS_SET_SYSCALL_RETURN - set syscall return value
 * @regs: pt_regs pointer
 * @val: return value to set
 */
#define PT_REGS_SET_SYSCALL_RETURN(r, res) (PT_REGS_X0(r) = (res))

/*
 * PT_REGS_SYSCALL_RET - get syscall return value
 */
#define PT_REGS_SYSCALL_RET(regs) \
	UPT_SYSCALL_RET(&(regs)->regs)

/*
 * PT_REGS_ORIG_SYSCALL - get/set original syscall number
 * Use the UML syscall shadow to avoid clobbering x8.
 * Returns lvalue so it can be assigned to.
 */
#define PT_REGS_ORIG_SYSCALL(r) ((r)->regs.syscall)

/*
 * PT_REGS_BP - get frame pointer (x29)
 * Used by stack unwinding code
 */
#define PT_REGS_BP(r) UPT_FP(&(r)->regs)

/*
 * PT_REGS_REG - get specific register by number (0-30)
 * Used for generic register access in debugging code
 */
#define PT_REGS_REG(r, n) ((r)->regs.gp[n])

/*
 * PT_REGS_PSTATE - get processor state register
 */
#define PT_REGS_PSTATE(r) ((r)->regs.gp[HOST_PSTATE])

/*
 * Ptrace constants not in ARM64 ABI
 * ARM64 doesn't have SYSEMU or thread area ptrace requests
 */
#define PTRACE_SYSEMU		  31
#define PTRACE_SYSEMU_SINGLESTEP  32
#define PTRACE_OLDSETOPTIONS	  21
#define PTRACE_GET_THREAD_AREA	  25
#define PTRACE_SET_THREAD_AREA	  26
#define PTRACE_ARCH_PRCTL	  30

#ifndef __ASSEMBLY__

/*
 * Task switching hook
 */
struct task_struct;
extern void arch_switch_to(struct task_struct *to);

/*
 * Thread area get/set (stubs for ARM64)
 * ARM64 doesn't use thread areas like x86, but we provide stubs for compatibility
 */
static inline int ptrace_get_thread_area(struct task_struct *child,
					 int idx, void __user *addr)
{
	return -ENOSYS;  /* Not implemented on ARM64 */
}

static inline int ptrace_set_thread_area(struct task_struct *child,
					 int idx, void __user *addr)
{
	return -ENOSYS;  /* Not implemented on ARM64 */
}

/*
 * Get user stack pointer
 */
#define user_stack_pointer(regs) PT_REGS_SP(regs)

/*
 * Regset support - declarations for ptrace_getregset/ptrace_setregset
 */
struct user_regset;
struct user_regset_view;
struct task_struct;
struct iovec;

extern int ptrace_getregset(struct task_struct *target,
			    unsigned int type, struct iovec *kiov);
extern int ptrace_setregset(struct task_struct *target,
			    unsigned int type, const struct iovec *kiov);

#endif /* __ASSEMBLY__ */

#endif /* __UM_ARM64_PTRACE_H */
