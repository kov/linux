/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_USER_H
#define __UM_ARM64_USER_H

#include <linux/types.h>

/*
 * User structures for ARM64 UML
 * These define the layout of user-visible register state
 */

/*
 * FP/SIMD state for user mode
 * ARM64 has 32 x 128-bit SIMD registers
 */
struct user_fpsimd_state {
	__uint128_t vregs[32];  /* 32 x 128-bit SIMD registers */
	__u32 fpsr;              /* Floating-point status register */
	__u32 fpcr;              /* Floating-point control register */
};

/*
 * User structure - for ptrace compatibility
 * This is a minimal stub for UML
 */
struct user {
	unsigned long regs[34];  /* x0-x30, sp, pc, pstate */
	unsigned long fp_regs[256];  /* FP/SIMD state */
	unsigned long u_debugreg[16];  /* Debug registers (HW breakpoints/watchpoints) */
};

#endif /* __UM_ARM64_USER_H */
