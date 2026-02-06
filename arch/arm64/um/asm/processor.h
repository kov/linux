/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_PROCESSOR_H
#define __UM_ARM64_PROCESSOR_H

/* Number of capability/bug words for cpuinfo_um */
#define NCAPINTS	2
#define NBUGINTS	1

#ifndef __ASSEMBLY__

/* Include fault info structure */
#include "../shared/sysdep/faultinfo_64.h"

/*
 * Architecture-specific thread state for ARM64 UML
 */
struct arch_thread {
	/*
	 * Hardware debug registers (breakpoints and watchpoints)
	 * ARM64 has up to 16 breakpoint/watchpoint registers
	 */
	unsigned long debugregs[16];

	/*
	 * Single-step state
	 */
	int singlestep_syscall;

	/*
	 * Fault information for current thread
	 */
	struct faultinfo faultinfo;
};

/*
 * Initialize arch_thread structure
 */
#define INIT_ARCH_THREAD { \
	.debugregs = { [0 ... 15] = 0 }, \
	.singlestep_syscall = 0, \
	.faultinfo = { 0, 0, 0 } \
}

/*
 * Architecture-specific thread operations
 * Moved to end of file to access thread_struct checks
 */

/*
 * Get current stack pointer and frame pointer
 * Used for stack unwinding and debugging
 */
#define current_sp() ({ \
	unsigned long sp; \
	__asm__("mov %0, sp" : "=r" (sp)); \
	(void *)sp; \
})

#define current_bp() ({ \
	unsigned long bp; \
	__asm__("mov %0, x29" : "=r" (bp)); \
	bp; \
})

/*
 * Kernel stack registers for task
 */
#define KSTK_EIP(tsk) KSTK_REG(tsk, HOST_PC)
#define KSTK_ESP(tsk) KSTK_REG(tsk, HOST_SP)
#define KSTK_EBP(tsk) KSTK_REG(tsk, HOST_FP)  /* Frame pointer for ARM64 */

/* Stack slots per line for stack dump */
#define STACKSLOTS_PER_LINE 4

/*
 * Stack growth detection
 */
#define ARCH_IS_STACKGROW(address) \
	(address + 65536 + 32 * sizeof(unsigned long) >= UPT_SP(&current->thread.regs.regs))

/*
 * Get pt_regs for task
 */
#define task_pt_regs(t) (&(t)->thread.regs)

/* Include generic UML processor definitions */
#include <asm/processor-generic.h>

/*
 * Architecture-specific thread operations
 * These must be defined after thread_struct is known (from processor-generic.h)
 */
#define arch_flush_thread(t) \
	do { \
		extern unsigned long exec_regs[]; \
		exec_regs[33] = 0; \
		container_of(t, struct thread_struct, arch)->regs.regs.tpidr_el0 = 0; \
	} while (0)

#define arch_copy_thread(from, to) \
	do { \
		container_of(to, struct thread_struct, arch)->regs.regs.tpidr_el0 = \
			container_of(from, struct thread_struct, arch)->regs.regs.tpidr_el0; \
	} while (0)

#endif /* __ASSEMBLY__ */

#endif /* __UM_ARM64_PROCESSOR_H */
