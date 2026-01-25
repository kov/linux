/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/shared/sysdep/faultinfo_64.h
 */

#ifndef __FAULTINFO_ARM64_H
#define __FAULTINFO_ARM64_H

/*
 * ARM64 fault information structure
 *
 * This contains architecture-specific fault information extracted from
 * signal context (mcontext_t). On ARM64, we get:
 *   - FAR_EL1 (Fault Address Register) - the faulting virtual address
 *   - ESR_EL1 (Exception Syndrome Register) - detailed exception info
 *
 * ESR_EL1 encoding:
 *   [31:26] EC  - Exception Class (e.g., data abort, instruction abort)
 *   [25]    IL  - Instruction Length (0=16-bit, 1=32-bit)
 *   [24:0]  ISS - Instruction Specific Syndrome
 *
 * For data/instruction aborts, ISS contains:
 *   [10]    FnV - FAR not Valid (if 1, FAR is not valid)
 *   [9]     EA  - External Abort
 *   [8]     CM  - Cache Maintenance
 *   [7]     S1PTW - Stage 1 translation table walk
 *   [6]     WnR - Write not Read (1=write, 0=read)
 *   [5:0]   DFSC/IFSC - Data/Instruction Fault Status Code
 */

struct faultinfo {
	int error_code;      /* ESR_EL1 ISS field with additional flags */
	unsigned long cr2;   /* FAR_EL1 - faulting address */
	int trap_no;         /* Exception class from ESR_EL1 */
};

/*
 * ESR_EL1 Exception Class values (bits [31:26])
 */
#define ESR_ELx_EC_SHIFT        26
#define ESR_ELx_EC_MASK         0x3F

/* Common exception classes */
#define ESR_ELx_EC_UNKNOWN      0x00  /* Unknown reason */
#define ESR_ELx_EC_WFx          0x01  /* WFI or WFE instruction */
#define ESR_ELx_EC_CP15_32      0x03  /* CP15 MCR/MRC */
#define ESR_ELx_EC_CP15_64      0x04  /* CP15 MCRR/MRRC */
#define ESR_ELx_EC_CP14_MR      0x05  /* CP14 MCR/MRC */
#define ESR_ELx_EC_CP14_LS      0x06  /* CP14 LDC/STC */
#define ESR_ELx_EC_FP_ASIMD     0x07  /* ASIMD/FP access */
#define ESR_ELx_EC_CP10_ID      0x08  /* CP10 MRC (VMRS) */
#define ESR_ELx_EC_PAC          0x09  /* PAC failure */
#define ESR_ELx_EC_CP14_64      0x0C  /* CP14 MCRR/MRRC */
#define ESR_ELx_EC_ILL          0x0E  /* Illegal Execution */
#define ESR_ELx_EC_SVC32        0x11  /* SVC from AArch32 */
#define ESR_ELx_EC_SVC64        0x15  /* SVC from AArch64 */
#define ESR_ELx_EC_SYS64        0x18  /* MRS/MSR (system register) */
#define ESR_ELx_EC_SVE          0x19  /* SVE access */
#define ESR_ELx_EC_IABT_LOW     0x20  /* Instruction abort from EL0 */
#define ESR_ELx_EC_IABT_CUR     0x21  /* Instruction abort from EL1 */
#define ESR_ELx_EC_PC_ALIGN     0x22  /* PC alignment fault */
#define ESR_ELx_EC_DABT_LOW     0x24  /* Data abort from EL0 */
#define ESR_ELx_EC_DABT_CUR     0x25  /* Data abort from EL1 */
#define ESR_ELx_EC_SP_ALIGN     0x26  /* SP alignment fault */
#define ESR_ELx_EC_FP_EXC32     0x28  /* FP exception from AArch32 */
#define ESR_ELx_EC_FP_EXC64     0x2C  /* FP exception from AArch64 */
#define ESR_ELx_EC_SERROR       0x2F  /* SError interrupt */
#define ESR_ELx_EC_BREAKPT_LOW  0x30  /* Breakpoint from EL0 */
#define ESR_ELx_EC_BREAKPT_CUR  0x31  /* Breakpoint from EL1 */
#define ESR_ELx_EC_SOFTSTP_LOW  0x32  /* Software step from EL0 */
#define ESR_ELx_EC_SOFTSTP_CUR  0x33  /* Software step from EL1 */
#define ESR_ELx_EC_WATCHPT_LOW  0x34  /* Watchpoint from EL0 */
#define ESR_ELx_EC_WATCHPT_CUR  0x35  /* Watchpoint from EL1 */
#define ESR_ELx_EC_BKPT32       0x38  /* BKPT from AArch32 */
#define ESR_ELx_EC_BRK64        0x3C  /* BRK from AArch64 */

/*
 * ISS field bits for data/instruction aborts
 */
#define ESR_ELx_WNR             (1UL << 6)   /* Write not Read */
#define ESR_ELx_S1PTW           (1UL << 7)   /* Stage 1 translation walk */
#define ESR_ELx_CM              (1UL << 8)   /* Cache maintenance */
#define ESR_ELx_EA              (1UL << 9)   /* External abort */
#define ESR_ELx_FnV             (1UL << 10)  /* FAR not Valid */

/*
 * Data/Instruction Fault Status Code (DFSC/IFSC) - bits [5:0]
 */
#define ESR_ELx_FSC_MASK        0x3F
#define ESR_ELx_FSC_FAULT       0x04  /* Translation fault (page not present) */
#define ESR_ELx_FSC_ACCESS      0x08  /* Access flag fault */
#define ESR_ELx_FSC_PERM        0x0C  /* Permission fault */

/*
 * Macro to extract fault information from mcontext_t
 *
 * ARM64 mcontext has:
 *   - fault_address: FAR_EL1
 *   - __reserved: Contains ESR_EL1 and other state
 *
 * Note: The exact location of ESR in __reserved may vary by libc.
 * We assume it's in the standard sigcontext structure.
 */
#define GET_FAULTINFO_FROM_MC(fi, mc) \
do { \
	unsigned long esr; \
	(fi).cr2 = (mc)->fault_address;  /* FAR_EL1 */ \
	/* ESR is typically stored after the main regs in sigcontext */ \
	/* For now, we'll get it from the reserved area */ \
	esr = *((unsigned long *)&(mc)->__reserved[0]); \
	(fi).error_code = esr & 0x1FFFFFF;  /* ISS + IL + EC lower bits */ \
	(fi).trap_no = (esr >> ESR_ELx_EC_SHIFT) & ESR_ELx_EC_MASK; \
} while (0)

/*
 * Determine if this is a write fault
 * Check the WnR (Write not Read) bit in error_code
 */
#define FAULT_WRITE(fi) ((fi).error_code & ESR_ELx_WNR)

/*
 * Get the faulting address
 */
#define FAULT_ADDRESS(fi) ((fi).cr2)

/*
 * Check if this is a fixable page fault
 * True for translation faults (page not present) from user mode
 */
#define SEGV_IS_FIXABLE(fi) \
	(((fi)->trap_no == ESR_ELx_EC_DABT_LOW || \
	  (fi)->trap_no == ESR_ELx_EC_IABT_LOW) && \
	 !((fi)->error_code & ESR_ELx_FnV))

/*
 * ARM64 has full fault info via ESR_EL1/FAR_EL1
 */
#define PTRACE_FULL_FAULTINFO 1

/*
 * Fault backtracking for kernel nofault access
 * ARM64 version using ARM64 assembly
 */
#define ___backtrack_faulted(_faulted)					\
	asm volatile (							\
		"adr %1, __get_kernel_nofault_faulted_%=\n"		\
		"mov %0, #0\n"						\
		"b _end_%=\n"						\
		"__get_kernel_nofault_faulted_%=:\n"			\
		"mov %0, #1\n"						\
		"_end_%=:"						\
		: "=r" (_faulted),					\
		  "=r" (current->thread.segv_continue) ::		\
	)

#endif /* __FAULTINFO_ARM64_H */
