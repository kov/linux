/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * CPU feature detection for ARM64 UML
 */

#ifndef _ASM_UM_ARM64_CPUFEATURE_H
#define _ASM_UM_ARM64_CPUFEATURE_H

#include <linux/types.h>

/*
 * ARM64 CPU feature bits
 * We define a minimal set for UML compatibility
 */
#define ARM64_NCAPINTS		2
#define ARM64_NBUGINTS		1

/* Always-on feature for compatibility with x86 code */
#define X86_FEATURE_ALWAYS	0

/* ARM64 feature flags (simplified for UML) */
#define ARM64_FEATURE_NEON	0
#define ARM64_FEATURE_SVE	1
#define ARM64_FEATURE_LSE	2
#define ARM64_FEATURE_PTR_AUTH	3

static inline int cpu_has(unsigned int feature)
{
	/* For UML, we inherit host CPU features */
	/* In a real implementation, check /proc/cpuinfo or use ptrace */
	return 1;  /* Assume features are available */
}

static inline int boot_cpu_has(unsigned int feature)
{
	return cpu_has(feature);
}

/*
 * Alternative instruction support (stub for ARM64 UML)
 */
#define alternative(oldinstr, newinstr, feature) \
	asm volatile (oldinstr)

#define alternative_2(oldinstr, newinstr1, feature1, newinstr2, feature2) \
	asm volatile (oldinstr)

/*
 * CPU feature testing
 */
#define cpu_feature_enabled(feature) cpu_has(feature)

/*
 * Static key support (simplified for UML)
 */
struct static_key;

static inline bool static_key_enabled(struct static_key *key)
{
	return false;
}

#endif /* _ASM_UM_ARM64_CPUFEATURE_H */
