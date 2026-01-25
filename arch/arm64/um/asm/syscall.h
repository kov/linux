/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_SYSCALL_H
#define __UM_ARM64_SYSCALL_H

/*
 * ARM64 syscall interface for UML
 * Prevents inclusion of native ARM64 syscall.h which expects different pt_regs
 */

#include <linux/err.h>
#include <linux/sched.h>
#include <linux/audit.h>
#include <asm/ptrace.h>

/* ARM64 audit architecture constant */
#ifndef AUDIT_ARCH_AARCH64
#define AUDIT_ARCH_AARCH64	(EM_AARCH64|__AUDIT_ARCH_64BIT|__AUDIT_ARCH_LE)
#endif

/* Syscall function pointer type */
typedef long (*sys_call_ptr_t)(const struct pt_regs *);

/* Syscall table declaration */
extern const sys_call_ptr_t sys_call_table[];

/*
 * UML uses UPT_* macros for register access, not direct pt_regs fields
 */
static inline int syscall_get_nr(struct task_struct *task,
				 struct pt_regs *regs)
{
	return UPT_SYSCALL_NR(&regs->regs);
}

static inline void syscall_set_return_value(struct task_struct *task,
					    struct pt_regs *regs,
					    int error, long val)
{
	UPT_SET_SYSCALL_RETURN(&regs->regs,
			       error ? error : val);
}

static inline void syscall_get_arguments(struct task_struct *task,
					 struct pt_regs *regs,
					 unsigned long *args)
{
	args[0] = UPT_SYSCALL_ARG1(&regs->regs);
	args[1] = UPT_SYSCALL_ARG2(&regs->regs);
	args[2] = UPT_SYSCALL_ARG3(&regs->regs);
	args[3] = UPT_SYSCALL_ARG4(&regs->regs);
	args[4] = UPT_SYSCALL_ARG5(&regs->regs);
	args[5] = UPT_SYSCALL_ARG6(&regs->regs);
}

static inline int syscall_get_arch(struct task_struct *task)
{
	return AUDIT_ARCH_AARCH64;
}

static inline long syscall_get_error(struct task_struct *task,
				     struct pt_regs *regs)
{
	unsigned long error = UPT_SYSCALL_RET(&regs->regs);
	return IS_ERR_VALUE(error) ? error : 0;
}

static inline long syscall_get_return_value(struct task_struct *task,
					    struct pt_regs *regs)
{
	return UPT_SYSCALL_RET(&regs->regs);
}

static inline void syscall_set_arguments(struct task_struct *task,
					 struct pt_regs *regs,
					 const unsigned long *args)
{
	UPT_SYSCALL_ARG1(&regs->regs) = args[0];
	UPT_SYSCALL_ARG2(&regs->regs) = args[1];
	UPT_SYSCALL_ARG3(&regs->regs) = args[2];
	UPT_SYSCALL_ARG4(&regs->regs) = args[3];
	UPT_SYSCALL_ARG5(&regs->regs) = args[4];
	UPT_SYSCALL_ARG6(&regs->regs) = args[5];
}

static inline void syscall_set_nr(struct task_struct *task,
				  struct pt_regs *regs,
				  int syscall)
{
	UPT_SYSCALL_NR(&regs->regs) = syscall;
}

static inline void syscall_rollback(struct task_struct *task,
				    struct pt_regs *regs)
{
	/* Restore syscall number from x8 - for ARM64, we don't have orig_syscall */
	/* No action needed as syscall number is still in x8 */
}

#endif /* __UM_ARM64_SYSCALL_H */
