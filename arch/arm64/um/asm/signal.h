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
 * ARM64 doesn't use SA_RESTORER like x86 does
 * Define it as 0 for compatibility with UML common code
 */
#ifndef SA_RESTORER
#define SA_RESTORER 0
#endif

/* Use generic signal definitions */
#include <asm-generic/signal.h>

#endif /* __UM_ARM64_SIGNAL_H */
