// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/ptrace_64.c
 */

#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/regset.h>
#include <linux/uaccess.h>
#include <linux/elf.h>
#include <asm/ptrace.h>
#include <asm/user.h>
#include <registers.h>

/*
 * User-modifiable PSTATE bits
 * Only allow modification of condition flags (N, Z, C, V)
 * Kernel controls mode, interrupt masks, and security features
 */
#define PSTATE_USER_MASK  (PSR_N_BIT | PSR_Z_BIT | PSR_C_BIT | PSR_V_BIT)

/*
 * Mapping from ptrace register offsets to HOST_* indices
 * ARM64 uses sequential layout: x0-x30, sp, pc, pstate
 */
static const int reg_offsets[MAX_REG_NR] = {
	[0]  = HOST_X0,
	[1]  = HOST_X1,
	[2]  = HOST_X2,
	[3]  = HOST_X3,
	[4]  = HOST_X4,
	[5]  = HOST_X5,
	[6]  = HOST_X6,
	[7]  = HOST_X7,
	[8]  = HOST_X8,
	[9]  = HOST_X9,
	[10] = HOST_X10,
	[11] = HOST_X11,
	[12] = HOST_X12,
	[13] = HOST_X13,
	[14] = HOST_X14,
	[15] = HOST_X15,
	[16] = HOST_X16,
	[17] = HOST_X17,
	[18] = HOST_X18,
	[19] = HOST_X19,
	[20] = HOST_X20,
	[21] = HOST_X21,
	[22] = HOST_X22,
	[23] = HOST_X23,
	[24] = HOST_X24,
	[25] = HOST_X25,
	[26] = HOST_X26,
	[27] = HOST_X27,
	[28] = HOST_X28,
	[29] = HOST_X29,
	[30] = HOST_X30,
	[31] = HOST_SP,
	[32] = HOST_PC,
	[33] = HOST_PSTATE,
};

/*
 * Set a single register value
 */
int putreg(struct task_struct *child, int regno, unsigned long value)
{
	if (regno < 0 || regno >= MAX_REG_NR)
		return -EIO;

	switch (regno) {
	case 0 ... 30:  /* x0-x30 (general purpose registers) */
	case 31:        /* sp */
	case 32:        /* pc */
		/* Allow any value for GP registers, SP, and PC */
		break;

	case 33:        /* pstate */
		/*
		 * Only allow modification of user-accessible PSTATE bits
		 * Preserve kernel-controlled bits and apply user mask
		 */
		value &= PSTATE_USER_MASK;
		child->thread.regs.regs.gp[HOST_PSTATE] &= ~PSTATE_USER_MASK;
		child->thread.regs.regs.gp[HOST_PSTATE] |= value;
		return 0;

	default:
		return -EIO;
	}

	child->thread.regs.regs.gp[reg_offsets[regno]] = value;
	return 0;
}

/*
 * Get a single register value
 */
unsigned long getreg(struct task_struct *child, int regno)
{
	if (regno < 0 || regno >= MAX_REG_NR)
		return 0;

	return child->thread.regs.regs.gp[reg_offsets[regno]];
}

/*
 * Write a register value via ptrace POKEUSER interface
 * addr is in bytes, must be 8-byte aligned
 */
int poke_user(struct task_struct *child, long addr, long data)
{
	/* Check alignment and bounds */
	if ((addr & 7) || addr < 0)
		return -EIO;

	/* Convert byte offset to register number */
	addr = addr >> 3;

	if (addr < MAX_REG_NR) {
		return putreg(child, addr, data);
	} else if (addr >= offsetof(struct user, u_debugreg[0]) >> 3 &&
		   addr <= offsetof(struct user, u_debugreg[15]) >> 3) {
		/*
		 * ARM64 has hardware debug registers (breakpoints/watchpoints)
		 * For now, just store them without activating
		 */
		int reg = addr - (offsetof(struct user, u_debugreg[0]) >> 3);
		if (reg >= 16)
			return -EIO;
		child->thread.arch.debugregs[reg] = data;
		return 0;
	}

	return -EIO;
}

/*
 * Read a register value via ptrace PEEKUSER interface
 * addr is in bytes, must be 8-byte aligned
 */
int peek_user(struct task_struct *child, long addr, long data)
{
	unsigned long tmp = 0;

	/* Check alignment and bounds */
	if ((addr & 7) || addr < 0)
		return -EIO;

	/* Convert byte offset to register number */
	addr = addr >> 3;

	if (addr < MAX_REG_NR) {
		tmp = getreg(child, addr);
	} else if (addr >= offsetof(struct user, u_debugreg[0]) >> 3 &&
		   addr <= offsetof(struct user, u_debugreg[15]) >> 3) {
		/*
		 * Return debug register values
		 */
		int reg = addr - (offsetof(struct user, u_debugreg[0]) >> 3);
		if (reg >= 16)
			return -EIO;
		tmp = child->thread.arch.debugregs[reg];
	}

	return put_user(tmp, (unsigned long __user *) data);
}

/*
 * Architecture-specific ptrace operations
 */
long subarch_ptrace(struct task_struct *child, long request,
		    unsigned long addr, unsigned long data)
{
	int ret = -EIO;
	void __user *datap = (void __user *) data;

	switch (request) {
	case PTRACE_GETFPREGS:  /* Get FP/SIMD registers */
		/*
		 * Copy FPSIMD register set to userspace
		 * This uses the regset framework for proper handling
		 */
		return copy_regset_to_user(child, task_user_regset_view(child),
					   REGSET_FP, 0,
					   sizeof(struct user_fpsimd_state),
					   datap);

	case PTRACE_SETFPREGS:  /* Set FP/SIMD registers */
		/*
		 * Copy FPSIMD registers from userspace
		 */
		return copy_regset_from_user(child, task_user_regset_view(child),
					     REGSET_FP, 0,
					     sizeof(struct user_fpsimd_state),
					     datap);

	case PTRACE_GETREGSET:
	case PTRACE_SETREGSET:
		/*
		 * Generic regset operations are handled by the core ptrace code
		 * in kernel/ptrace.c using task_user_regset_view()
		 * Return -EINVAL to let the core handle it
		 */
		return -EINVAL;

	default:
		ret = -EIO;
		break;
	}

	return ret;
}

/*
 * Check if single-stepping is enabled
 */
static inline int is_single_stepping(struct task_struct *child)
{
	unsigned long pstate = child->thread.regs.regs.gp[HOST_PSTATE];
	return !!(pstate & PSR_SS_BIT);
}

/*
 * ARM64-specific single-step setup
 * Called by common UML code via SUBARCH_SET_SINGLESTEPPING macro
 */
#define SUBARCH_SET_SINGLESTEPPING(child, on) \
	do { \
		if (on) \
			(child)->thread.regs.regs.gp[HOST_PSTATE] |= PSR_SS_BIT; \
		else \
			(child)->thread.regs.regs.gp[HOST_PSTATE] &= ~PSR_SS_BIT; \
		(child)->thread.arch.singlestep_syscall = 0; \
	} while (0)
