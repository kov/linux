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
#define ELF_PLAT_INIT(_r, load_addr)             \
	do {                                     \
		int i;                           \
		for (i = 0; i < MAX_REG_NR; i++) \
			(_r)->regs.gp[i] = 0;    \
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

/*
 * vDSO support - map the vDSO page into each new process and pass its
 * address via AT_SYSINFO_EHDR so that glibc/ld.so can locate it.
 */
#define ARCH_HAS_SETUP_ADDITIONAL_PAGES 1
struct linux_binprm;
extern int arch_setup_additional_pages(struct linux_binprm *bprm,
	int uses_interp);

extern unsigned long um_vdso_addr;
#ifndef AT_SYSINFO_EHDR
#define AT_SYSINFO_EHDR 33
#endif
#define ARCH_DLINFO	NEW_AUX_ENT(AT_SYSINFO_EHDR, um_vdso_addr)

#endif /* __UM_ARM64_ELF_H */
