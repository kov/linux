/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/shared/sysdep/ptrace_64.h
 */

#ifndef __SYSDEP_ARM64_PTRACE_H
#define __SYSDEP_ARM64_PTRACE_H

#include <sysdep/faultinfo.h>

/*
 * Register indices for ARM64 user_pt_regs structure.
 * These are used to index into the gp[] array in struct uml_pt_regs.
 *
 * ARM64 register layout:
 *   x0-x30: General purpose registers (31 regs)
 *   sp:     Stack pointer
 *   pc:     Program counter
 *   pstate: Processor state (flags, mode, etc.)
 */

/* General purpose registers x0-x30 */
#define HOST_X0         0
#define HOST_X1         1
#define HOST_X2         2
#define HOST_X3         3
#define HOST_X4         4
#define HOST_X5         5
#define HOST_X6         6
#define HOST_X7         7
#define HOST_X8         8
#define HOST_X9         9
#define HOST_X10        10
#define HOST_X11        11
#define HOST_X12        12
#define HOST_X13        13
#define HOST_X14        14
#define HOST_X15        15
#define HOST_X16        16
#define HOST_X17        17
#define HOST_X18        18
#define HOST_X19        19
#define HOST_X20        20
#define HOST_X21        21
#define HOST_X22        22
#define HOST_X23        23
#define HOST_X24        24
#define HOST_X25        25
#define HOST_X26        26
#define HOST_X27        27
#define HOST_X28        28
#define HOST_X29        29
#define HOST_X30        30

/* Special registers */
#define HOST_SP         31
#define HOST_PC         32
#define HOST_PSTATE     33

/* Aliases for commonly used registers */
#define HOST_IP         HOST_PC   /* Instruction pointer (x86 compatibility) */
#define HOST_LR         HOST_X30  /* Link register */
#define HOST_FP         HOST_X29  /* Frame pointer */

/* Maximum register number */
#define MAX_REG_NR      34

/*
 * Register accessor macros - direct access to gp[] array
 */
#define REGS_X0(r)      ((r)[HOST_X0])
#define REGS_X1(r)      ((r)[HOST_X1])
#define REGS_X2(r)      ((r)[HOST_X2])
#define REGS_X3(r)      ((r)[HOST_X3])
#define REGS_X4(r)      ((r)[HOST_X4])
#define REGS_X5(r)      ((r)[HOST_X5])
#define REGS_X6(r)      ((r)[HOST_X6])
#define REGS_X7(r)      ((r)[HOST_X7])
#define REGS_X8(r)      ((r)[HOST_X8])
#define REGS_X9(r)      ((r)[HOST_X9])
#define REGS_X10(r)     ((r)[HOST_X10])
#define REGS_X11(r)     ((r)[HOST_X11])
#define REGS_X12(r)     ((r)[HOST_X12])
#define REGS_X13(r)     ((r)[HOST_X13])
#define REGS_X14(r)     ((r)[HOST_X14])
#define REGS_X15(r)     ((r)[HOST_X15])
#define REGS_X16(r)     ((r)[HOST_X16])
#define REGS_X17(r)     ((r)[HOST_X17])
#define REGS_X18(r)     ((r)[HOST_X18])
#define REGS_X19(r)     ((r)[HOST_X19])
#define REGS_X20(r)     ((r)[HOST_X20])
#define REGS_X21(r)     ((r)[HOST_X21])
#define REGS_X22(r)     ((r)[HOST_X22])
#define REGS_X23(r)     ((r)[HOST_X23])
#define REGS_X24(r)     ((r)[HOST_X24])
#define REGS_X25(r)     ((r)[HOST_X25])
#define REGS_X26(r)     ((r)[HOST_X26])
#define REGS_X27(r)     ((r)[HOST_X27])
#define REGS_X28(r)     ((r)[HOST_X28])
#define REGS_X29(r)     ((r)[HOST_X29])
#define REGS_X30(r)     ((r)[HOST_X30])

#define REGS_SP(r)      ((r)[HOST_SP])
#define REGS_PC(r)      ((r)[HOST_PC])
#define REGS_PSTATE(r)  ((r)[HOST_PSTATE])

#define REGS_LR(r)      REGS_X30(r)
#define REGS_FP(r)      REGS_X29(r)

/*
 * UPT_* macros - access registers through struct uml_pt_regs pointer
 * These are used throughout UML kernel code
 */
#define UPT_X0(r)       REGS_X0((r)->gp)
#define UPT_X1(r)       REGS_X1((r)->gp)
#define UPT_X2(r)       REGS_X2((r)->gp)
#define UPT_X3(r)       REGS_X3((r)->gp)
#define UPT_X4(r)       REGS_X4((r)->gp)
#define UPT_X5(r)       REGS_X5((r)->gp)
#define UPT_X6(r)       REGS_X6((r)->gp)
#define UPT_X7(r)       REGS_X7((r)->gp)
#define UPT_X8(r)       REGS_X8((r)->gp)
#define UPT_X9(r)       REGS_X9((r)->gp)
#define UPT_X10(r)      REGS_X10((r)->gp)
#define UPT_X11(r)      REGS_X11((r)->gp)
#define UPT_X12(r)      REGS_X12((r)->gp)
#define UPT_X13(r)      REGS_X13((r)->gp)
#define UPT_X14(r)      REGS_X14((r)->gp)
#define UPT_X15(r)      REGS_X15((r)->gp)
#define UPT_X16(r)      REGS_X16((r)->gp)
#define UPT_X17(r)      REGS_X17((r)->gp)
#define UPT_X18(r)      REGS_X18((r)->gp)
#define UPT_X19(r)      REGS_X19((r)->gp)
#define UPT_X20(r)      REGS_X20((r)->gp)
#define UPT_X21(r)      REGS_X21((r)->gp)
#define UPT_X22(r)      REGS_X22((r)->gp)
#define UPT_X23(r)      REGS_X23((r)->gp)
#define UPT_X24(r)      REGS_X24((r)->gp)
#define UPT_X25(r)      REGS_X25((r)->gp)
#define UPT_X26(r)      REGS_X26((r)->gp)
#define UPT_X27(r)      REGS_X27((r)->gp)
#define UPT_X28(r)      REGS_X28((r)->gp)
#define UPT_X29(r)      REGS_X29((r)->gp)
#define UPT_X30(r)      REGS_X30((r)->gp)

#define UPT_SP(r)       REGS_SP((r)->gp)
#define UPT_PC(r)       REGS_PC((r)->gp)
#define UPT_PSTATE(r)   REGS_PSTATE((r)->gp)

#define UPT_LR(r)       UPT_X30(r)
#define UPT_FP(r)       UPT_X29(r)

/*
 * Syscall interface macros
 * ARM64 syscall convention:
 *   - Syscall number is tracked in regs->syscall to avoid clobbering x8
 *   - Arguments in x0-x5 (up to 6 args)
 *   - Return value in x0
 */
#define UPT_SYSCALL_NR(r)       ((r)->syscall)
#define UPT_SYSCALL_ARG1(r)     UPT_X0(r)
#define UPT_SYSCALL_ARG2(r)     UPT_X1(r)
#define UPT_SYSCALL_ARG3(r)     UPT_X2(r)
#define UPT_SYSCALL_ARG4(r)     UPT_X3(r)
#define UPT_SYSCALL_ARG5(r)     UPT_X4(r)
#define UPT_SYSCALL_ARG6(r)     UPT_X5(r)
#define UPT_SYSCALL_RET(r)      UPT_X0(r)

/*
 * Instruction pointer and stack pointer accessors
 * These are used by common UML code
 */
#define UPT_IP(r)       UPT_PC(r)
#define UPT_SP(r)       REGS_SP((r)->gp)

/*
 * Set syscall return value
 */
#define UPT_SET_SYSCALL_RETURN(r, res) \
	(UPT_X0(r) = (res))

/*
 * Restart syscall - decrement PC by 4 to re-execute SVC instruction
 */
#define UPT_RESTART_SYSCALL(r) \
	(UPT_PC(r) -= 4)

/*
 * PSTATE bits we care about
 */
#define PSR_MODE_EL0t   0x00000000
#define PSR_MODE_EL1t   0x00000004
#define PSR_MODE_EL1h   0x00000005
#define PSR_MODE_MASK   0x0000000f

#define PSR_F_BIT       0x00000040  /* FIQ mask */
#define PSR_I_BIT       0x00000080  /* IRQ mask */
#define PSR_A_BIT       0x00000100  /* Async abort mask */
#define PSR_D_BIT       0x00000200  /* Debug mask */

#define PSR_SSBS_BIT    0x00001000  /* Speculative Store Bypass Safe */
#define PSR_PAN_BIT     0x00400000  /* Privileged Access Never */
#define PSR_UAO_BIT     0x00800000  /* User Access Override */
#define PSR_DIT_BIT     0x01000000  /* Data Independent Timing */
#define PSR_TCO_BIT     0x02000000  /* Tag Check Override */

#define PSR_V_BIT       0x10000000  /* Overflow condition flag */
#define PSR_C_BIT       0x20000000  /* Carry condition flag */
#define PSR_Z_BIT       0x40000000  /* Zero condition flag */
#define PSR_N_BIT       0x80000000  /* Negative condition flag */

#define PSR_SS_BIT      0x00200000  /* Single-step */

/*
 * User-modifiable PSTATE bits
 * Only allow modification of condition flags (N, Z, C, V)
 * Kernel controls mode, interrupt masks, and security features
 */
#define PSR_USER_MASK   (PSR_N_BIT | PSR_Z_BIT | PSR_C_BIT | PSR_V_BIT)

/*
 * Host FP register size (detected at runtime)
 */
extern unsigned long host_fp_size;

/*
 * UML pt_regs structure
 * Contains all guest register state
 */
struct uml_pt_regs {
	unsigned long gp[MAX_REG_NR];  /* General purpose registers */
	struct faultinfo faultinfo;     /* Fault information */
	long syscall;                   /* Current syscall number */
	int is_user;                    /* Running in user mode? */
	unsigned long tpidr_el0;        /* ARM64 TLS Register */
	unsigned long _pad; /* Align fp to 16 bytes for SIMD access */

	/* Dynamically sized FP registers (FPSIMD/SVE state) */
	unsigned long fp[];
};

#define EMPTY_UML_PT_REGS { }

/*
 * Additional UML pt_regs accessors
 */
#define UPT_FAULTINFO(r)        (&(r)->faultinfo)
#define UPT_IS_USER(r)          ((r)->is_user)

/*
 * Note: UPT_SYSCALL_NR is already defined above as UPT_X8
 * but we also store it in the syscall field for tracking
 */

/*
 * Architecture initialization
 */
extern int arch_init_registers(int pid);

#ifdef __KERNEL__
struct pt_regs;
extern int arch_do_signal_or_restart(struct pt_regs *regs, int has_signal);
extern long __arm64_sys_ni_syscall(const struct pt_regs *regs);
#endif

/* TLS handling */
extern int os_set_thread_area(void *tls, int pid);
extern int os_get_thread_area(void *tls, int pid);
extern void check_host_supports_tls(int *supports_tls, int *tls_min);

#endif /* __SYSDEP_ARM64_PTRACE_H */
