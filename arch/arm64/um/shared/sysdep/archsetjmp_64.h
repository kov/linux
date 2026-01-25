/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/shared/sysdep/archsetjmp_64.h
 */

#ifndef _KLIBC_ARCHSETJMP_H
#define _KLIBC_ARCHSETJMP_H

/*
 * ARM64 jmp_buf structure for kernel context switching
 *
 * ARM64 calling convention - callee-saved registers:
 *   x19-x28: General purpose callee-saved (10 registers)
 *   x29:     Frame pointer (FP)
 *   x30:     Link register (LR)
 *   sp:      Stack pointer
 *
 * Total: 13 registers to save for context switch
 *
 * This is more than x86_64 which only saves 8 registers
 * (rbx, rsp, rbp, r12-r15, rip)
 */

struct __jmp_buf {
	unsigned long __x19;
	unsigned long __x20;
	unsigned long __x21;
	unsigned long __x22;
	unsigned long __x23;
	unsigned long __x24;
	unsigned long __x25;
	unsigned long __x26;
	unsigned long __x27;
	unsigned long __x28;
	unsigned long __x29;  /* Frame pointer */
	unsigned long __x30;  /* Link register (return address) */
	unsigned long __sp;   /* Stack pointer */
};

typedef struct __jmp_buf jmp_buf[1];

/*
 * Convenience macros for accessing key fields
 */
#define JB_IP __x30   /* Link register serves as instruction pointer */
#define JB_SP __sp    /* Stack pointer */
#define JB_FP __x29   /* Frame pointer */

/* Get register value from jmp_buf - used by KSTK_REG macro */
unsigned long get_thread_reg(int reg, jmp_buf *buf);

#endif /* _KLIBC_ARCHSETJMP_H */
