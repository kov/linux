// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/user-offsets.c
 */

#include <stdio.h>
#include <stddef.h>
#include <signal.h>
#include <poll.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <linux/ptrace.h>
#include <asm/types.h>
#include <linux/kbuild.h>

/*
 * Generate offset constants for ARM64 registers
 *
 * These HOST_* constants are used to index into the gp[] array
 * in struct uml_pt_regs
 */

#define DEFINE_LONGS(sym, val)	\
	COMMENT(#val " / sizeof(unsigned long)");	\
	DEFINE(sym, val / sizeof(unsigned long))

/* Silence -Wmissing-prototypes warning */
void foo(void);

void foo(void)
{
	/*
	 * ARM64 register offsets
	 * These map to indices in user_pt_regs structure
	 * DEFINE_LONGS converts byte offsets to array indices
	 */

	/* General purpose registers x0-x30 */
	DEFINE_LONGS(HOST_X0, offsetof(struct user_pt_regs, regs[0]));
	DEFINE_LONGS(HOST_X1, offsetof(struct user_pt_regs, regs[1]));
	DEFINE_LONGS(HOST_X2, offsetof(struct user_pt_regs, regs[2]));
	DEFINE_LONGS(HOST_X3, offsetof(struct user_pt_regs, regs[3]));
	DEFINE_LONGS(HOST_X4, offsetof(struct user_pt_regs, regs[4]));
	DEFINE_LONGS(HOST_X5, offsetof(struct user_pt_regs, regs[5]));
	DEFINE_LONGS(HOST_X6, offsetof(struct user_pt_regs, regs[6]));
	DEFINE_LONGS(HOST_X7, offsetof(struct user_pt_regs, regs[7]));
	DEFINE_LONGS(HOST_X8, offsetof(struct user_pt_regs, regs[8]));
	DEFINE_LONGS(HOST_X9, offsetof(struct user_pt_regs, regs[9]));
	DEFINE_LONGS(HOST_X10, offsetof(struct user_pt_regs, regs[10]));
	DEFINE_LONGS(HOST_X11, offsetof(struct user_pt_regs, regs[11]));
	DEFINE_LONGS(HOST_X12, offsetof(struct user_pt_regs, regs[12]));
	DEFINE_LONGS(HOST_X13, offsetof(struct user_pt_regs, regs[13]));
	DEFINE_LONGS(HOST_X14, offsetof(struct user_pt_regs, regs[14]));
	DEFINE_LONGS(HOST_X15, offsetof(struct user_pt_regs, regs[15]));
	DEFINE_LONGS(HOST_X16, offsetof(struct user_pt_regs, regs[16]));
	DEFINE_LONGS(HOST_X17, offsetof(struct user_pt_regs, regs[17]));
	DEFINE_LONGS(HOST_X18, offsetof(struct user_pt_regs, regs[18]));
	DEFINE_LONGS(HOST_X19, offsetof(struct user_pt_regs, regs[19]));
	DEFINE_LONGS(HOST_X20, offsetof(struct user_pt_regs, regs[20]));
	DEFINE_LONGS(HOST_X21, offsetof(struct user_pt_regs, regs[21]));
	DEFINE_LONGS(HOST_X22, offsetof(struct user_pt_regs, regs[22]));
	DEFINE_LONGS(HOST_X23, offsetof(struct user_pt_regs, regs[23]));
	DEFINE_LONGS(HOST_X24, offsetof(struct user_pt_regs, regs[24]));
	DEFINE_LONGS(HOST_X25, offsetof(struct user_pt_regs, regs[25]));
	DEFINE_LONGS(HOST_X26, offsetof(struct user_pt_regs, regs[26]));
	DEFINE_LONGS(HOST_X27, offsetof(struct user_pt_regs, regs[27]));
	DEFINE_LONGS(HOST_X28, offsetof(struct user_pt_regs, regs[28]));
	DEFINE_LONGS(HOST_X29, offsetof(struct user_pt_regs, regs[29]));
	DEFINE_LONGS(HOST_X30, offsetof(struct user_pt_regs, regs[30]));

	/* Special registers */
	DEFINE_LONGS(HOST_SP, offsetof(struct user_pt_regs, sp));
	DEFINE_LONGS(HOST_PC, offsetof(struct user_pt_regs, pc));
	DEFINE_LONGS(HOST_PSTATE, offsetof(struct user_pt_regs, pstate));

	/*
	 * Note: HOST_LR, HOST_FP, HOST_IP aliases are defined in ptrace_64.h
	 * not here, to match x86_64 pattern
	 */

	/*
	 * Frame size and other constants
	 */
	DEFINE(UM_FRAME_SIZE, sizeof(struct user_pt_regs));

	/* Poll constants */
	DEFINE(UM_POLLIN, POLLIN);
	DEFINE(UM_POLLPRI, POLLPRI);
	DEFINE(UM_POLLOUT, POLLOUT);

	/* Memory protection flags */
	DEFINE(UM_PROT_READ, PROT_READ);
	DEFINE(UM_PROT_WRITE, PROT_WRITE);
	DEFINE(UM_PROT_EXEC, PROT_EXEC);
}
