/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_SEGMENT_ARM64_H
#define __UM_SEGMENT_ARM64_H

/*
 * ARM64 doesn't use segments like x86
 * TLS is handled via TPIDR_EL0 register instead of GDT entries
 * This header exists for compatibility with common UML code
 */

/* For compatibility with x86-specific UML code */
#define GDT_ENTRY_TLS_ENTRIES 0  /* ARM64 doesn't use GDT */

#endif /* __UM_SEGMENT_ARM64_H */
