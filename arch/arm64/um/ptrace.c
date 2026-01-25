// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Ptrace regset support
 * Based on arch/x86/um/ptrace.c
 */

#include <linux/regset.h>
#include <linux/elf.h>
#include <linux/uaccess.h>
#include <linux/ptrace.h>
#include <asm/ptrace.h>

enum uml_regset {
	UML_REGSET_GENERAL,
	UML_REGSET_FP,
};

/*
 * Callbacks for general purpose registers
 */
static int genregs_get(struct task_struct *target,
		       const struct user_regset *regset,
		       struct membuf to)
{
	struct uml_pt_regs *regs = &target->thread.regs.regs;

	/* Copy all 34 registers (x0-x30, sp, pc, pstate) */
	return membuf_write(&to, regs->gp, ELF_NGREG * sizeof(unsigned long));
}

static int genregs_set(struct task_struct *target,
		       const struct user_regset *regset,
		       unsigned int pos, unsigned int count,
		       const void *kbuf, const void __user *ubuf)
{
	struct uml_pt_regs *regs = &target->thread.regs.regs;
	int ret;

	/* Allow setting all registers */
	ret = user_regset_copyin(&pos, &count, &kbuf, &ubuf,
				 regs->gp, 0, ELF_NGREG * sizeof(unsigned long));

	return ret;
}

/*
 * Callbacks for FP registers
 */
extern unsigned long host_fp_size;

static int generic_fpregs_active(struct task_struct *target,
				 const struct user_regset *regset)
{
	return host_fp_size > 0 ? regset->n : 0;
}

static int generic_fpregs_get(struct task_struct *target,
			      const struct user_regset *regset,
			      struct membuf to)
{
	struct uml_pt_regs *regs = &target->thread.regs.regs;

	if (!host_fp_size)
		return -ENODEV;

	return membuf_write(&to, regs->fp, host_fp_size);
}

static int generic_fpregs_set(struct task_struct *target,
			      const struct user_regset *regset,
			      unsigned int pos, unsigned int count,
			      const void *kbuf, const void __user *ubuf)
{
	struct uml_pt_regs *regs = &target->thread.regs.regs;
	int ret;

	if (!host_fp_size)
		return -ENODEV;

	ret = user_regset_copyin(&pos, &count, &kbuf, &ubuf,
				 regs->fp, 0, host_fp_size);

	return ret;
}

/*
 * Register set definitions
 */
static struct user_regset uml_regsets[] __ro_after_init = {
	[UML_REGSET_GENERAL] = {
		USER_REGSET_NOTE_TYPE(PRSTATUS),
		.n		= ELF_NGREG,
		.size		= sizeof(unsigned long),
		.align		= sizeof(unsigned long),
		.regset_get	= genregs_get,
		.set		= genregs_set
	},
	[UML_REGSET_FP] = {
		USER_REGSET_NOTE_TYPE(PRFPREG),
		.size		= sizeof(unsigned long),
		.align		= sizeof(unsigned long),
		.active		= generic_fpregs_active,
		.regset_get	= generic_fpregs_get,
		.set		= generic_fpregs_set,
	},
};

static const struct user_regset_view user_uml_view = {
	.name = "aarch64",
	.e_machine = EM_AARCH64,
	.regsets = uml_regsets,
	.n = ARRAY_SIZE(uml_regsets)
};

const struct user_regset_view *task_user_regset_view(struct task_struct *tsk)
{
	return &user_uml_view;
}
/* Exported by kernel/ptrace.c */

/*
 * Initialize FP regset size
 */
static int __init init_regset_fp_info(void)
{
	if (host_fp_size > 0)
		uml_regsets[UML_REGSET_FP].n = host_fp_size / sizeof(unsigned long);

	return 0;
}
arch_initcall(init_regset_fp_info);
