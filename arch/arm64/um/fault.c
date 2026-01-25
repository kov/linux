// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/fault.c
 */

#include <arch.h>
#include <sysdep/ptrace.h>

/* From asm-um/uaccess.h and linux/module.h */
struct exception_table_entry
{
	unsigned long insn;
	unsigned long fixup;
};

const struct exception_table_entry *search_exception_tables(unsigned long add);

/*
 * Exception table fixup handling
 * Compare to arch/arm64/mm/extable.c
 */
int arch_fixup(unsigned long address, struct uml_pt_regs *regs)
{
	const struct exception_table_entry *fixup;

	fixup = search_exception_tables(address);
	if (fixup) {
		UPT_PC(regs) = fixup->fixup;
		return 1;
	}
	return 0;
}
