// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * ARM64-specific syscall handling
 */

#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/seccomp.h>
#include <linux/dcache.h>
#include <linux/file.h>
#include <linux/poll.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <kern_util.h>
#include <os.h>
#include <skas.h>
#include <sysdep/ptrace.h>
#include <sysdep/ptrace_user.h>
#include <linux/time-internal.h>
#include <asm/syscall.h>
#include <asm/unistd.h>
#include <asm/delay.h>

/* map lookups live in skas/process.c */

asmlinkage long __arm64_sys_ni_syscall(const struct pt_regs *regs)
{
	return -ENOSYS;
}


/*
 * ARM64-specific syscall handler
 * ARM64 syscalls take struct pt_regs * wrappers.
 */
void handle_syscall(struct uml_pt_regs *r)
{
	struct pt_regs *regs = container_of(r, struct pt_regs, regs);
	long syscall;

	syscall = UPT_SYSCALL_NR(r);

	if (syscall_trace_enter(regs))
		goto out;

	/* Re-read in case tracer changed it */
	syscall = UPT_SYSCALL_NR(r);

	if (syscall >= 0 && syscall < __NR_syscalls) {
		unsigned long ret;

		ret = (*sys_call_table[syscall])(regs);

		PT_REGS_SET_SYSCALL_RETURN(regs, ret);
	} else {
		PT_REGS_SET_SYSCALL_RETURN(regs, -ENOSYS);
	}

out:
	syscall_trace_leave(regs);

	/*
	 * This should do all the signal delivery and notification
	 * and stuff like that
	 */
	do_signal(regs);
}
