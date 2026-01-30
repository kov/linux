// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/os-Linux/tls.c
 */

#include <errno.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <linux/elf.h>
#include <stdio.h>
#include <sysdep/ptrace.h>

/*
 * ARM64 TLS operations via ptrace
 *
 * ARM64 TLS is accessed via the TPIDR_EL0 register using:
 *   ptrace(PTRACE_SETREGSET, pid, NT_ARM_TLS, &iov)
 *
 * This is much simpler than x86:
 *   - No GDT manipulation (x86_32)
 *   - No FS_BASE/GS_BASE MSRs (x86_64)
 *   - Just a single register via ptrace
 */

/*
 * Set TLS pointer in host process
 * Uses ptrace to set TPIDR_EL0 register
 */
int os_set_thread_area(void *tls, int pid)
{
	struct iovec iov;
	unsigned long tls_vals[2];
	int ret;

	iov.iov_base = tls_vals;
	iov.iov_len = sizeof(tls_vals);

	/*
	 * Use ptrace SETREGSET with NT_ARM_TLS to set TPIDR_EL0
	 * This is the standard way to set TLS on ARM64
	 */
	ret = ptrace(PTRACE_GETREGSET, pid, NT_ARM_TLS, &iov);
	if (ret < 0)
		return -errno;

	tls_vals[0] = *(unsigned long *)tls;
	iov.iov_base = tls_vals;
	iov.iov_len = sizeof(tls_vals);
	if (ptrace(PTRACE_SETREGSET, pid, NT_ARM_TLS, &iov) < 0)
		return -errno;

	return 0;
}

/*
 * Get TLS pointer from host process
 * Uses ptrace to read TPIDR_EL0 register
 */
int os_get_thread_area(void *tls, int pid)
{
	struct iovec iov;
	unsigned long tls_vals[2];

	iov.iov_base = tls_vals;
	iov.iov_len = sizeof(unsigned long); // Try 8 bytes
	tls_vals[0] = 0xdeadbeef;
	tls_vals[1] = 0xcafebabe;

	/*
	 * Use ptrace GETREGSET with NT_ARM_TLS to read TPIDR_EL0
	 */
	if (ptrace(PTRACE_GETREGSET, pid, NT_ARM_TLS, &iov) < 0)
		return -errno;

	*(unsigned long *)tls = tls_vals[0];
	*(unsigned long *)tls = tls_vals[0];
	return 0;
}

/*
 * Check if host supports TLS
 * ARM64 always supports TPIDR_EL0, so this always succeeds
 */
void check_host_supports_tls(int *supports_tls, int *tls_min)
{
	/*
	 * ARM64 always supports TLS via TPIDR_EL0
	 * No version checking needed like on x86
	 */
	*supports_tls = 1;
	*tls_min = 0;  /* Not applicable for ARM64 */
}
