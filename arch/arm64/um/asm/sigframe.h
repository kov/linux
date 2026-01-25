/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_SIGFRAME_H
#define __UM_ARM64_SIGFRAME_H

#include <linux/signal.h>

/*
 * ARM64 signal frame structures for UML
 * These are simplified versions for UML usage
 */

/* ARM64 only uses RT signal frames */
struct rt_sigframe {
	struct siginfo info;
	struct ucontext uc;
};

#define sigframe rt_sigframe

#endif /* __UM_ARM64_SIGFRAME_H */
