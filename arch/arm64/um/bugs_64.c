// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/bugs_64.c
 */

#include <arch.h>
#include <sysdep/ptrace.h>

/*
 * CPU feature detection and bug checks for ARM64
 */
void arch_check_bugs(void)
{
	/*
	 * Check for ARM64 CPU features
	 * Could check /proc/cpuinfo for:
	 *   - NEON/ASIMD support (should always be present on ARM64)
	 *   - SVE support (optional)
	 *   - Crypto extensions
	 *   - Pointer authentication
	 *   - Memory tagging (MTE)
	 *
	 * For now, we assume basic ARM64 compliance
	 */
}

/*
 * Examine signals for architecture-specific issues
 */
void arch_examine_signal(int sig, struct uml_pt_regs *regs)
{
	/*
	 * Could check for:
	 *   - Illegal instructions
	 *   - Alignment faults
	 *   - FP/SIMD exceptions
	 *
	 * For now, no special handling needed
	 */
}
