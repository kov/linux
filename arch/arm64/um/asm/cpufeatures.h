/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef _ASM_UM_ARM64_CPUFEATURES_H
#define _ASM_UM_ARM64_CPUFEATURES_H

/*
 * ARM64 CPU features (simplified for UML)
 * This provides compatibility with UML common code
 */

/* Number of capability/bug integers */
#define NCAPINTS	2
#define NBUGINTS	1

/* Always-available feature for optimization */
#define X86_FEATURE_ALWAYS	0

#endif /* _ASM_UM_ARM64_CPUFEATURES_H */
