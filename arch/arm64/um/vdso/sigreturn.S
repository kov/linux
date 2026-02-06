/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Sigreturn trampoline for returning from a signal when the SA_RESTORER
 * flag is not set.
 *
 * Based on arch/arm64/kernel/vdso/sigreturn.S
 */

#include <linux/linkage.h>
#include <asm/unistd.h>

	.text

/*
 * A NOP before the sigreturn entry point is required for some unwinders
 * that unconditionally subtract one from the return address.
 */
	nop

SYM_CODE_START(__kernel_rt_sigreturn)
	mov	x8, #__NR_rt_sigreturn
	svc	#0
SYM_CODE_END(__kernel_rt_sigreturn)
