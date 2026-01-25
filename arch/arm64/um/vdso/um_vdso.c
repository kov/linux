// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/vdso/um_vdso.c
 *
 * This vDSO turns all calls into a syscall so that UML can trap them.
 */

/* Disable profiling for userspace code */
#define DISABLE_BRANCH_PROFILING

#include <vdso/gettime.h>
#include <linux/time.h>
#include <asm/unistd.h>

/*
 * ARM64 syscall calling convention:
 * - Syscall number in x8
 * - Arguments in x0-x5
 * - Return value in x0
 * - Use "svc #0" instruction to invoke syscall
 */

int __vdso_clock_gettime(clockid_t clock, struct __kernel_timespec *ts)
{
	register long ret asm("x0");
	register long nr asm("x8") = __NR_clock_gettime;
	register clockid_t _clock asm("x0") = clock;
	register struct __kernel_timespec *_ts asm("x1") = ts;

	asm volatile(
		"svc #0"
		: "=r" (ret)
		: "r" (nr), "r" (_clock), "r" (_ts)
		: "memory");

	return ret;
}
int clock_gettime(clockid_t, struct __kernel_timespec *)
	__attribute__((weak, alias("__vdso_clock_gettime")));

int __vdso_gettimeofday(struct __kernel_old_timeval *tv, struct timezone *tz)
{
	register long ret asm("x0");
	register long nr asm("x8") = __NR_gettimeofday;
	register struct __kernel_old_timeval *_tv asm("x0") = tv;
	register struct timezone *_tz asm("x1") = tz;

	asm volatile(
		"svc #0"
		: "=r" (ret)
		: "r" (nr), "r" (_tv), "r" (_tz)
		: "memory");

	return ret;
}
int gettimeofday(struct __kernel_old_timeval *, struct timezone *)
	__attribute__((weak, alias("__vdso_gettimeofday")));
