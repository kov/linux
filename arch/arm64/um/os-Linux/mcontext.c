// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/os-Linux/mcontext.c
 */

#include <linux/errno.h>
#include <linux/string.h>
#include <sys/ucontext.h>
#include <asm/ptrace.h>
#include <asm/sigcontext.h>
#include <sysdep/ptrace.h>
#include <sysdep/faultinfo.h>
#include <stub-data.h>
#include <sysdep/mcontext.h>


/*
 * Copy registers from host signal context to UML pt_regs
 *
 * ARM64 mcontext_t contains:
 *   - fault_address: FAR_EL1 (faulting address)
 *   - regs[31]: x0-x30
 *   - sp: stack pointer
 *   - pc: program counter
 *   - pstate: processor state
 *   - __reserved: FP/SIMD state and other extensions
 */
void get_regs_from_mc(struct uml_pt_regs *regs, mcontext_t *mc)
{
	int i;

	/* Copy general purpose registers x0-x30 */
	for (i = 0; i < 31; i++)
		regs->gp[i] = mc->regs[i];

	/* Copy special registers */
	regs->gp[HOST_SP] = mc->sp;
	regs->gp[HOST_PC] = mc->pc;
	regs->gp[HOST_PSTATE] = mc->pstate;
}

/*
 * Set instruction pointer in signal context
 * Used for signal handler setup
 */
void mc_set_rip(void *_mc, void *target)
{
	mcontext_t *mc = _mc;
	mc->pc = (unsigned long)target;
}

/*
 * Copy registers from UML pt_regs to host signal context
 *
 * single_stepping: if true, set single-step bit in PSTATE
 */
void get_mc_from_regs(struct uml_pt_regs *regs, mcontext_t *mc,
		      int single_stepping)
{
	int i;

	/* Copy general purpose registers x0-x30 */
	for (i = 0; i < 31; i++)
		mc->regs[i] = regs->gp[i];

	/* Copy special registers */
	mc->sp = regs->gp[HOST_SP];
	mc->pc = regs->gp[HOST_PC];
	mc->pstate = regs->gp[HOST_PSTATE];

	/* Set or clear single-step bit */
	if (single_stepping)
		mc->pstate |= PSR_SS_BIT;
	else
		mc->pstate &= ~PSR_SS_BIT;
}

/*
 * Get FP/SIMD state from signal context __reserved area
 *
 * ARM64 signal frame layout:
 *   - sigcontext with basic registers
 *   - __reserved area contains:
 *     - fpsimd_context (FPSIMD registers)
 *     - sve_context (if SVE is active)
 *     - other extension contexts
 *
 * Each context has a header:
 *   u32 magic;  // identifies the context type
 *   u32 size;   // size including header
 *
 * fpsimd_context is defined in asm/sigcontext.h
 */
#define SVE_MAGIC	0x53564501

static void *get_fpstate(struct stub_data *data,
			 mcontext_t *mcontext,
			 int *fp_size)
{
	struct fpsimd_context *fpsimd;
	unsigned long reserved_offset;

	/*
	 * Calculate offset of __reserved from start of sigstack
	 * mcontext->__reserved comes after the main register state
	 */
	reserved_offset = ((unsigned long)mcontext->__reserved -
			   (unsigned long)mcontext) +
			  ((unsigned long)mcontext -
			   (unsigned long)&data->sigstack[0]);

	if (reserved_offset + sizeof(struct fpsimd_context) >
	    sizeof(data->sigstack))
		return NULL;

	fpsimd = (void *)&data->sigstack[reserved_offset];

	/* Verify FPSIMD magic */
	if (fpsimd->head.magic != FPSIMD_MAGIC)
		return NULL;

	*fp_size = fpsimd->head.size;

	return fpsimd;
}

/*
 * Get register state from stub (seccomp mode)
 * Reads both GP registers and FP state from signal stack
 */
int get_stub_state(struct uml_pt_regs *regs, struct stub_data *data,
		   unsigned long *fp_size_out)
{
	mcontext_t *mcontext;
	struct fpsimd_context *fpstate_stub;
	int fp_size;

	/* mctx_offset is verified by wait_stub_done_seccomp */
	mcontext = (void *)&data->sigstack[data->mctx_offset];

	/* Copy GP registers from signal context */
	get_regs_from_mc(regs, mcontext);

	/* Get FP/SIMD state */
	fpstate_stub = get_fpstate(data, mcontext, &fp_size);
	if (!fpstate_stub)
		return -EINVAL;

	if (fp_size_out)
		*fp_size_out = fp_size;

	if (fp_size > host_fp_size)
		return -ENOSPC;

	/* Copy FP state to UML pt_regs */
	memcpy(&regs->fp, fpstate_stub, fp_size);

	/*
	 * Note: We do not need to read TLS (TPIDR_EL0) from signal context
	 * as it is managed separately via ptrace NT_ARM_TLS
	 */

	return 0;
}

/*
 * Set register state in stub (seccomp mode)
 * Writes both GP registers and FP state to signal stack
 */
int set_stub_state(struct uml_pt_regs *regs, struct stub_data *data,
		   int single_stepping)
{
	mcontext_t *mcontext;
	struct fpsimd_context *fpstate_stub;
	int fp_size;

	/* mctx_offset is verified by wait_stub_done_seccomp */
	mcontext = (void *)&data->sigstack[data->mctx_offset];

	/* Validate mcontext pointer is within sigstack */
	if ((unsigned long)mcontext < (unsigned long)data->sigstack ||
	    (unsigned long)mcontext >
			(unsigned long)data->sigstack +
			sizeof(data->sigstack) - sizeof(*mcontext))
		return -EINVAL;

	/* Copy GP registers to signal context */
	get_mc_from_regs(regs, mcontext, single_stepping);

	/* Get FP/SIMD state pointer */
	fpstate_stub = get_fpstate(data, mcontext, &fp_size);
	if (!fpstate_stub)
		return -EINVAL;

	/* Copy FP state from UML pt_regs */
	memcpy(fpstate_stub, &regs->fp, fp_size);

	/*
	 * TLS (TPIDR_EL0) updates are handled via ptrace in arch_switch_to()
	 * We don't need to sync it here like x86 does with FS_BASE/GS_BASE
	 */

	return 0;
}
