/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_UACCESS_H
#define __UM_ARM64_UACCESS_H

/*
 * Prevent inclusion of native ARM64 uaccess.h
 * UML uses its own uaccess implementation
 */

/* Stub for backtrack fault tracking - not needed for UML */
#define ___backtrack_faulted(x) do { } while (0)

/* Use generic UML uaccess */
#include <asm-generic/uaccess.h>

#endif /* __UM_ARM64_UACCESS_H */
