// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/tls_64.c
 */

#include <linux/sched.h>
#include <asm/ptrace-abi.h>
#include <registers.h>

/*
 * ARM64 TLS (Thread-Local Storage) management
 *
 * ARM64 uses the TPIDR_EL0 system register for TLS.
 * Unlike x86_32 (which uses GDT entries), ARM64's TLS is saved/restored
 * as part of the regular register state via ptrace, so no special handling
 * is needed beyond storing the value in the thread structure.
 */

void clear_flushed_tls(struct task_struct *task)
{
	/* Nothing to do - TLS is handled via ptrace register state */
}

int arch_set_tls(struct task_struct *t, unsigned long tls)
{
	/*
	 * Store TLS value in arch_thread structure
	 * It will be saved/restored automatically via ptrace
	 */
	t->thread.arch.tpidr_el0 = tls;
	return 0;
}
