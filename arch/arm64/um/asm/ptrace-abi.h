/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_PTRACE_ABI_H
#define __UM_ARM64_PTRACE_ABI_H

/*
 * ARM64 ptrace ABI definitions for UML
 * This header provides ptrace request constants
 */

/* Standard ARM64 ptrace requests */
#define PTRACE_GETFPREGS	14
#define PTRACE_SETFPREGS	15
#define PTRACE_GETREGSET	0x4204
#define PTRACE_SETREGSET	0x4205

/* ARM64 register sets for GETREGSET/SETREGSET */
#define REGSET_GPR	0
#define REGSET_FP	1

#endif /* __UM_ARM64_PTRACE_ABI_H */
