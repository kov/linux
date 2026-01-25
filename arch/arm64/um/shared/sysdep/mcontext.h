/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/shared/sysdep/mcontext.h
 */

#ifndef __SYSDEP_ARM64_MCONTEXT_H
#define __SYSDEP_ARM64_MCONTEXT_H

#include <stub-data.h>

extern void get_regs_from_mc(struct uml_pt_regs *, mcontext_t *);
extern void get_mc_from_regs(struct uml_pt_regs *regs, mcontext_t *mc,
			     int single_stepping);

extern int get_stub_state(struct uml_pt_regs *regs, struct stub_data *data,
			  unsigned long *fp_size_out);
extern int set_stub_state(struct uml_pt_regs *regs, struct stub_data *data,
			  int single_stepping);

extern void mc_set_rip(void *mc, void *target);

/* GET_FAULTINFO_FROM_MC is defined in faultinfo_64.h */

#endif
