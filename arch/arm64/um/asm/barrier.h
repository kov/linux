/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_BARRIER_H
#define __UM_ARM64_BARRIER_H

#include <asm-generic/rwonce.h>

/*
 * Memory barriers for ARM64 UML
 * Since UML runs as a userspace process, we use the host's barriers
 */

#define mb()	__asm__ __volatile__ ("dmb sy" : : : "memory")
#define rmb()	__asm__ __volatile__ ("dmb ld" : : : "memory")
#define wmb()	__asm__ __volatile__ ("dmb st" : : : "memory")

#define dma_mb()	mb()
#define dma_rmb()	rmb()
#define dma_wmb()	wmb()

/*
 * SMP barriers
 */
#define smp_mb()	mb()
#define smp_rmb()	rmb()
#define smp_wmb()	wmb()

/*
 * Barriers for atomic operations
 */
#define smp_mb__before_atomic()	smp_mb()
#define smp_mb__after_atomic()	smp_mb()

#define smp_store_release(p, v)						\
do {									\
	compiletime_assert_atomic_type(*p);				\
	barrier();							\
	WRITE_ONCE(*p, v);						\
} while (0)

#define smp_load_acquire(p)						\
({									\
	typeof(*p) ___p1 = READ_ONCE(*p);				\
	compiletime_assert_atomic_type(*p);				\
	barrier();							\
	___p1;								\
})

/*
 * CPU relax - yield hint for the processor
 * On ARM64, use the YIELD instruction
 */
#define cpu_relax()	__asm__ __volatile__ ("yield" : : : "memory")

#define cpu_relax_lowlatency() cpu_relax()

/*
 * Control dependency barrier
 * Used after control-dependent loads to ensure ordering
 */
#define smp_acquire__after_ctrl_dep()	smp_rmb()

/*
 * Conditional barrier
 */
#define smp_cond_load_relaxed(ptr, cond_expr)				\
({									\
	typeof(ptr) __PTR = (ptr);					\
	typeof(*ptr) VAL;						\
	for (;;) {							\
		VAL = READ_ONCE(*__PTR);				\
		if (cond_expr)						\
			break;						\
		cpu_relax();						\
	}								\
	VAL;								\
})

#define smp_cond_load_acquire(ptr, cond_expr)				\
({									\
	typeof(*ptr) _val;						\
	_val = smp_cond_load_relaxed(ptr, cond_expr);			\
	smp_acquire__after_ctrl_dep();					\
	_val;								\
})

/*
 * Store with memory barrier
 */
#define smp_store_mb(var, value) \
do { \
	WRITE_ONCE(var, value); \
	smp_mb(); \
} while (0)

#endif /* __UM_ARM64_BARRIER_H */
