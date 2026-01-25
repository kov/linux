/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_ELF_H
#define __UM_ARM64_ELF_H

#include <linux/types.h>

/*
 * ELF definitions for ARM64 UML
 * This prevents native ARM64 elf.h from being included
 */

typedef unsigned long elf_greg_t;

#define ELF_NGREG 34  /* x0-x30, sp, pc, pstate */

typedef elf_greg_t elf_gregset_t[ELF_NGREG];

/* Floating point registers */
typedef struct {
	__uint128_t vregs[32];
	__u32 fpsr;
	__u32 fpcr;
} elf_fpregset_t;

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_CLASS	ELFCLASS64
#define ELF_DATA	ELFDATA2LSB
#define ELF_ARCH	EM_AARCH64

#define ELF_PLATFORM	(elf_aux_platform)

/*
 * Hardware capabilities - defined in os-Linux/elf_aux.c
 */
#define ELF_HWCAP	(elf_aux_hwcap)
extern unsigned long elf_aux_hwcap;
extern char *elf_aux_platform;

/*
 * This is used to ensure we don't load something for the wrong architecture.
 */
#define elf_check_arch(x) ((x)->e_machine == EM_AARCH64)

/*
 * Initialize registers for new ELF process
 * UML pt_regs has gp[] array, not direct register fields
 */
#define ELF_PLAT_INIT(_r, load_addr) \
	do { \
		(_r)->regs.gp[0] = 0; \
	} while (0)

#define USE_ELF_CORE_DUMP
#define ELF_EXEC_PAGESIZE	4096

/*
 * This is the location that an ET_DYN program is loaded if exec'ed.
 * Typical use of this is to invoke "./ld.so someprog" to test out a
 * new version of the loader. We need to make sure that it is out of
 * the way of the program that it will "exec", and that there is
 * sufficient room for the brk.
 */
#define ELF_ET_DYN_BASE		0x400000UL

/* ARM64 relocation types */
#define R_AARCH64_NONE		0
#define R_AARCH64_ABS64		257
#define R_AARCH64_ABS32		258
#define R_AARCH64_RELATIVE	1027

#endif /* __UM_ARM64_ELF_H */
