/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * word-at-a-time wrapper for ARM64 UML
 */

#ifndef __UM_ARM64_WORD_AT_A_TIME_H
#define __UM_ARM64_WORD_AT_A_TIME_H

/*
 * Use the generic word-at-a-time implementation for UML
 * The native ARM64 version uses architecture-specific assembly
 * and MTE (Memory Tagging Extension) features that don't apply to UML
 */
#include <asm-generic/word-at-a-time.h>

#endif /* __UM_ARM64_WORD_AT_A_TIME_H */
