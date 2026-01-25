/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * String function overrides for UML
 *
 * UML cannot use ARM64 native assembly implementations because:
 * 1. They use ARM64 assembly instructions not available in userspace
 * 2. They use native ARM64 headers that conflict with UML
 *
 * Instead, we use the generic C implementations from lib/string.c
 * These are renamed via -Dstrrchr=kernel_strrchr in arch/um/Makefile
 */

#ifndef __UM_ARM64_STRING_H
#define __UM_ARM64_STRING_H

/*
 * Undefine ARM64 architecture-specific string functions
 * This allows the generic implementations from lib/string.c to be used
 */
#undef __HAVE_ARCH_STRRCHR
#undef __HAVE_ARCH_STRCHR
#undef __HAVE_ARCH_STRCMP
#undef __HAVE_ARCH_STRNCMP
#undef __HAVE_ARCH_STRLEN
#undef __HAVE_ARCH_STRNLEN
#undef __HAVE_ARCH_MEMCMP
#undef __HAVE_ARCH_MEMCHR

/*
 * We do keep the optimized memcpy/memmove/memset from ARM64
 * as they are provided as C wrappers that work in UML
 */
#define __HAVE_ARCH_MEMCPY
extern void *memcpy(void *, const void *, __kernel_size_t);
extern void *__memcpy(void *, const void *, __kernel_size_t);

#define __HAVE_ARCH_MEMMOVE
extern void *memmove(void *, const void *, __kernel_size_t);
extern void *__memmove(void *, const void *, __kernel_size_t);

#define __HAVE_ARCH_MEMSET
extern void *memset(void *, int, __kernel_size_t);
extern void *__memset(void *, int, __kernel_size_t);

#endif /* __UM_ARM64_STRING_H */
