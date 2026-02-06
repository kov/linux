/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_SIGNAL_H
#define __UM_ARM64_SIGNAL_H

/*
 * This header prevents inclusion of native ARM64 signal.h
 * which would conflict with UML's signal handling
 */

/*
 * SA_RESTORER is used by glibc on arm64 to provide a signal return
 * trampoline via the VDSO. Must match the native arm64 value so that
 * glibc's sa_restorer is used instead of an on-stack trampoline.
 */
#ifndef SA_RESTORER
#define SA_RESTORER 0x04000000
#endif

/* Use generic signal definitions */
#include <asm-generic/signal.h>

#endif /* __UM_ARM64_SIGNAL_H */
