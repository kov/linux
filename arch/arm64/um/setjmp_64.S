/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ARM64 setjmp/longjmp implementation for User Mode Linux
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/setjmp_64.S
 */

/*
 * The jmp_buf structure layout (see archsetjmp_64.h):
 *   Offset 0:   x19
 *   Offset 8:   x20
 *   Offset 16:  x21
 *   Offset 24:  x22
 *   Offset 32:  x23
 *   Offset 40:  x24
 *   Offset 48:  x25
 *   Offset 56:  x26
 *   Offset 64:  x27
 *   Offset 72:  x28
 *   Offset 80:  x29 (FP)
 *   Offset 88:  x30 (LR - return address)
 *   Offset 96:  sp
 *
 * Total size: 104 bytes (13 registers * 8 bytes)
 */

	.text
	.align 4
	.globl kernel_setjmp
	.type kernel_setjmp, @function

/*
 * int kernel_setjmp(jmp_buf env)
 *
 * Save callee-saved registers to jmp_buf.
 * Arguments:
 *   x0 = pointer to jmp_buf
 * Returns:
 *   0 on direct invocation
 */
kernel_setjmp:
	/* Save callee-saved registers x19-x28 */
	stp	x19, x20, [x0, #0]
	stp	x21, x22, [x0, #16]
	stp	x23, x24, [x0, #32]
	stp	x25, x26, [x0, #48]
	stp	x27, x28, [x0, #64]

	/* Save frame pointer (x29) and link register (x30) */
	stp	x29, x30, [x0, #80]

	/* Save stack pointer */
	mov	x1, sp
	str	x1, [x0, #96]

	/* Return 0 */
	mov	x0, #0
	ret

	.size kernel_setjmp, .-kernel_setjmp


	.text
	.align 4
	.globl kernel_longjmp
	.type kernel_longjmp, @function

/*
 * void kernel_longjmp(jmp_buf env, int val)
 *
 * Restore callee-saved registers from jmp_buf and jump.
 * Arguments:
 *   x0 = pointer to jmp_buf
 *   x1 = return value (will be returned by setjmp)
 * Returns:
 *   Does not return - jumps to saved context
 */
kernel_longjmp:
	/* Restore callee-saved registers x19-x28 */
	ldp	x19, x20, [x0, #0]
	ldp	x21, x22, [x0, #16]
	ldp	x23, x24, [x0, #32]
	ldp	x25, x26, [x0, #48]
	ldp	x27, x28, [x0, #64]

	/* Restore frame pointer (x29) and link register (x30) */
	ldp	x29, x30, [x0, #80]

	/* Restore stack pointer */
	ldr	x2, [x0, #96]
	mov	sp, x2

	/* Set return value (from x1 parameter) */
	mov	x0, x1

	/* Jump to saved return address (in x30/LR) */
	ret

	.size kernel_longjmp, .-kernel_longjmp
