// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/syscalls_64.c
 */

#include <linux/syscalls.h>
#include <linux/mm.h>
#include <asm/unistd.h>
#include <os.h>

/*
 * ARM64-specific syscall wrappers
 *
 * Most syscalls are generic, but some need arch-specific handling
 */

/*
 * sys_mmap - memory mapping syscall
 *
 * ARM64 mmap uses offset in bytes (not pages like some architectures)
 * This is already the standard Linux behavior, so no conversion needed
 */
SYSCALL_DEFINE6(mmap, unsigned long, addr, unsigned long, len,
		unsigned long, prot, unsigned long, flags,
		unsigned long, fd, off_t, offset)
{
	/* kov added */
	if (offset_in_page(offset) != 0)
		return -EINVAL;
	/* kov added */

	/*
	 * ARM64 uses standard mmap with byte offset
	 * No special handling needed like on some architectures
	 */
	return ksys_mmap_pgoff(addr, len, prot, flags, fd,
			       offset >> PAGE_SHIFT);
}

/*
 * sys_arm64_personality - ARM64 personality syscall
 *
 * Handles personality setting for ARM64
 */
SYSCALL_DEFINE1(arm64_personality, unsigned int, personality)
{
	return ksys_personality(personality);
}

/*
 * arch_switch_to - architecture-specific task switch hook
 * @to: task being switched to
 *
 * Called during context switch to perform any architecture-specific setup.
 * For ARM64 UML, no special handling is needed - TLS and register state
 * are managed through ptrace.
 */
void arch_switch_to(struct task_struct *to)
{
	/*
	 * Nothing needs to be done on ARM64 UML.
	 * The TPIDR_EL0 register is saved in the ptrace register set.
	 */
}
