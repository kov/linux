// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2000 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 */

#include <linux/stddef.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ptrace.h>
#include <linux/sched/mm.h>
#include <linux/sched/task.h>
#include <linux/sched/task_stack.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/current.h>
#include <asm/processor.h>
#include <linux/uaccess.h>
#include <as-layout.h>
#include <mem_user.h>
#include <registers.h>
#include <skas.h>
#include <os.h>

void flush_thread(void)
{
	arch_flush_thread(&current->thread.arch);

	get_safe_registers(current_pt_regs()->regs.gp,
			   current_pt_regs()->regs.fp);
}

void start_thread(struct pt_regs *regs, unsigned long eip, unsigned long esp)
{
	PT_REGS_IP(regs) = eip;
	PT_REGS_SP(regs) = esp;
#ifdef __aarch64__
	/* ARM64: Clear frame pointer and link register for new userspace process
	 * These contain kernel addresses from exec_regs that are meaningless in userspace
	 */
	PT_REGS_REG(regs, 29) = 0;  /* X29 (FP) */
	PT_REGS_REG(regs, 30) = 0;  /* X30 (LR) */
#endif
	clear_thread_flag(TIF_SINGLESTEP);
}
EXPORT_SYMBOL(start_thread);
