/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_SECCOMP_H
#define __UM_ARM64_SECCOMP_H

#include <linux/unistd.h>

/*
 * SECCOMP_ARCH_NATIVE for ARM64
 * This identifies the architecture for seccomp filters
 */
#define SECCOMP_ARCH_NATIVE		AUDIT_ARCH_AARCH64
#define SECCOMP_ARCH_NATIVE_NR		NR_syscalls
#define SECCOMP_ARCH_NATIVE_NAME	"aarch64"

/* Include generic seccomp syscall definitions */
#include <asm-generic/seccomp.h>

#endif /* __UM_ARM64_SECCOMP_H */
