/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_EXTABLE_H
#define __UM_ARM64_EXTABLE_H

/*
 * Exception table for ARM64 UML
 * Used for handling faults in kernel code (like copy_from_user)
 */

/* Include alternative.h for struct alt_instr used by asm-offsets.c */
#include <asm/alternative.h>

#include <asm-generic/extable.h>

#endif /* __UM_ARM64_EXTABLE_H */
