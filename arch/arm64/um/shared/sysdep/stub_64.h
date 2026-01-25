/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/shared/sysdep/stub_64.h
 */

#ifndef __SYSDEP_STUB_H
#define __SYSDEP_STUB_H

#include <asm/unistd.h>
#include <sysdep/ptrace_user.h>

/*
 * ARM64 uses __NR_mmap for memory mapping
 */
#define STUB_MMAP_NR __NR_mmap

/*
 * mmap offset handling - ARM64 is 64-bit so offset is used directly
 */
#define MMAP_OFFSET(o) (o)

/*
 * ARM64 syscall stub functions
 *
 * These are used in seccomp mode to make syscalls from the stub
 * without going through the normal syscall path.
 *
 * ARM64 syscall convention:
 *   - Syscall instruction: SVC #0
 *   - Syscall number in x8
 *   - Arguments in x0-x5
 *   - Return value in x0
 *   - Syscall clobbers: x0-x18, x30 (lr)
 */

/*
 * Make a raw syscall with no arguments
 */
static __always_inline long stub_syscall0(long syscall)
{
	register long x8 __asm__("x8") = syscall;
	register long x0 __asm__("x0");

	__asm__ __volatile__ (
		"svc #0"
		: "=r" (x0)
		: "r" (x8)
		: "memory", "cc"
	);

	return x0;
}

/*
 * Make a raw syscall with 1 argument
 */
static __always_inline long stub_syscall1(long syscall, long arg1)
{
	register long x8 __asm__("x8") = syscall;
	register long x0 __asm__("x0") = arg1;

	__asm__ __volatile__ (
		"svc #0"
		: "+r" (x0)
		: "r" (x8)
		: "memory", "cc"
	);

	return x0;
}

/*
 * Make a raw syscall with 2 arguments
 */
static __always_inline long stub_syscall2(long syscall, long arg1, long arg2)
{
	register long x8 __asm__("x8") = syscall;
	register long x0 __asm__("x0") = arg1;
	register long x1 __asm__("x1") = arg2;

	__asm__ __volatile__ (
		"svc #0"
		: "+r" (x0)
		: "r" (x8), "r" (x1)
		: "memory", "cc"
	);

	return x0;
}

/*
 * Make a raw syscall with 3 arguments
 */
static __always_inline long stub_syscall3(long syscall, long arg1, long arg2,
					  long arg3)
{
	register long x8 __asm__("x8") = syscall;
	register long x0 __asm__("x0") = arg1;
	register long x1 __asm__("x1") = arg2;
	register long x2 __asm__("x2") = arg3;

	__asm__ __volatile__ (
		"svc #0"
		: "+r" (x0)
		: "r" (x8), "r" (x1), "r" (x2)
		: "memory", "cc"
	);

	return x0;
}

/*
 * Make a raw syscall with 4 arguments
 */
static __always_inline long stub_syscall4(long syscall, long arg1, long arg2,
					  long arg3, long arg4)
{
	register long x8 __asm__("x8") = syscall;
	register long x0 __asm__("x0") = arg1;
	register long x1 __asm__("x1") = arg2;
	register long x2 __asm__("x2") = arg3;
	register long x3 __asm__("x3") = arg4;

	__asm__ __volatile__ (
		"svc #0"
		: "+r" (x0)
		: "r" (x8), "r" (x1), "r" (x2), "r" (x3)
		: "memory", "cc"
	);

	return x0;
}

/*
 * Make a raw syscall with 5 arguments
 */
static __always_inline long stub_syscall5(long syscall, long arg1, long arg2,
					  long arg3, long arg4, long arg5)
{
	register long x8 __asm__("x8") = syscall;
	register long x0 __asm__("x0") = arg1;
	register long x1 __asm__("x1") = arg2;
	register long x2 __asm__("x2") = arg3;
	register long x3 __asm__("x3") = arg4;
	register long x4 __asm__("x4") = arg5;

	__asm__ __volatile__ (
		"svc #0"
		: "+r" (x0)
		: "r" (x8), "r" (x1), "r" (x2), "r" (x3), "r" (x4)
		: "memory", "cc"
	);

	return x0;
}

/*
 * Make a raw syscall with 6 arguments
 */
static __always_inline long stub_syscall6(long syscall, long arg1, long arg2,
					  long arg3, long arg4, long arg5,
					  long arg6)
{
	register long x8 __asm__("x8") = syscall;
	register long x0 __asm__("x0") = arg1;
	register long x1 __asm__("x1") = arg2;
	register long x2 __asm__("x2") = arg3;
	register long x3 __asm__("x3") = arg4;
	register long x4 __asm__("x4") = arg5;
	register long x5 __asm__("x5") = arg6;

	__asm__ __volatile__ (
		"svc #0"
		: "+r" (x0)
		: "r" (x8), "r" (x1), "r" (x2), "r" (x3), "r" (x4), "r" (x5)
		: "memory", "cc"
	);

	return x0;
}

/*
 * Trigger a breakpoint for debugging/trapping
 * ARM64 uses BRK instruction
 */
static __always_inline void trap_myself(void)
{
	__asm__ __volatile__ ("brk #0" ::: "memory");
}

/*
 * Get pointer to stub data page
 * The stub data is stored in the page following the stub code
 */
static __always_inline void *get_stub_data(void)
{
	unsigned long ret;

	__asm__ __volatile__ (
		"adr %0, .\n"           /* Get current PC */
		"and %0, %0, %1\n"      /* Round down to page boundary */
		"add %0, %0, %2\n"      /* Add one page to get data page */
		"sub %0, %0, #8\n"      /* ARM64: adjust for stub_data base skew */
		: "=r" (ret)
		: "r" (~(UM_KERN_PAGE_SIZE - 1)),
		  "r" (UM_KERN_PAGE_SIZE)
		: "memory"
	);

	return (void *)ret;
}

/*
 * Restore TLS register after seccomp trap
 * ARM64 uses TPIDR_EL0 for TLS
 */
static __always_inline void
stub_seccomp_restore_state(struct stub_data_arch *arch)
{
	/*
	 * TLS restore would require ptrace, which we can't do from stub.
	 * The kernel-side code handles TLS restoration via ptrace.
	 * Just mark as synced.
	 */
	arch->sync = 0;
}

/*
 * Get current instruction pointer (PC)
 * Used for debugging and error reporting
 */
static __always_inline unsigned long stub_get_pc(void)
{
	unsigned long pc;

	__asm__ __volatile__ (
		"adr %0, ."
		: "=r" (pc)
	);

	return pc;
}

/*
 * Clone stub trampoline
 * Used when creating new processes in seccomp mode
 */
static __always_inline void remap_stack_and_trap(void)
{
	trap_myself();
}

/*
 * Start stub by adjusting stack and calling init function
 * ARM64 version: subtract STUB_SIZE from stack pointer and call function
 */
#define stub_start(fn)							\
	__asm__ __volatile__ (						\
		"sub sp, sp, %0\n"					\
		"mov x0, %1\n"						\
		"blr x0\n"						\
		:: "i" (STUB_SIZE),					\
		   "r" (&fn)						\
		: "x0", "memory"					\
	)

#endif /* __SYSDEP_STUB_H */
