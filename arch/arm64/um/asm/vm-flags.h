/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/asm/vm-flags.h
 */

#ifndef __VM_FLAGS_ARM64_H
#define __VM_FLAGS_ARM64_H

/*
 * VM flags for ARM64
 * ARM64 is always 64-bit, so stack grows down and is executable
 */
#define VM_STACK_DEFAULT_FLAGS (VM_GROWSDOWN | VM_DATA_FLAGS_EXEC)

#endif /* __VM_FLAGS_ARM64_H */
