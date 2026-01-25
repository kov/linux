/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_MEMORY_H
#define __UM_ARM64_MEMORY_H

/*
 * This header prevents inclusion of native ARM64 memory.h
 * UML uses its own memory management via arch/um/include/asm/page.h
 *
 * DO NOT include native arch/arm64/include/asm/memory.h
 */

#endif /* __UM_ARM64_MEMORY_H */
