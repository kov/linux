// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/sysrq_64.c
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/sched/debug.h>
#include <linux/utsname.h>
#include <asm/current.h>
#include <asm/ptrace.h>

void show_regs(struct pt_regs *regs)
{
	printk("\n");
	print_modules();
	printk(KERN_INFO "Pid: %d, comm: %.20s %s %s\n", task_pid_nr(current),
		current->comm, print_tainted(), init_utsname()->release);
	printk(KERN_INFO "PC: %pS\n", (void *)PT_REGS_IP(regs));
	printk(KERN_INFO "LR: %016lx  SP: %016lx\n",
	       PT_REGS_REG(regs, 30), PT_REGS_SP(regs));
	printk(KERN_INFO "PSTATE: %08lx\n", PT_REGS_PSTATE(regs));
	printk(KERN_INFO "x0 : %016lx x1 : %016lx x2 : %016lx\n",
	       PT_REGS_REG(regs, 0), PT_REGS_REG(regs, 1), PT_REGS_REG(regs, 2));
	printk(KERN_INFO "x3 : %016lx x4 : %016lx x5 : %016lx\n",
	       PT_REGS_REG(regs, 3), PT_REGS_REG(regs, 4), PT_REGS_REG(regs, 5));
	printk(KERN_INFO "x6 : %016lx x7 : %016lx x8 : %016lx\n",
	       PT_REGS_REG(regs, 6), PT_REGS_REG(regs, 7), PT_REGS_REG(regs, 8));
	printk(KERN_INFO "x9 : %016lx x10: %016lx x11: %016lx\n",
	       PT_REGS_REG(regs, 9), PT_REGS_REG(regs, 10), PT_REGS_REG(regs, 11));
	printk(KERN_INFO "x12: %016lx x13: %016lx x14: %016lx\n",
	       PT_REGS_REG(regs, 12), PT_REGS_REG(regs, 13), PT_REGS_REG(regs, 14));
	printk(KERN_INFO "x15: %016lx x16: %016lx x17: %016lx\n",
	       PT_REGS_REG(regs, 15), PT_REGS_REG(regs, 16), PT_REGS_REG(regs, 17));
	printk(KERN_INFO "x18: %016lx x19: %016lx x20: %016lx\n",
	       PT_REGS_REG(regs, 18), PT_REGS_REG(regs, 19), PT_REGS_REG(regs, 20));
	printk(KERN_INFO "x21: %016lx x22: %016lx x23: %016lx\n",
	       PT_REGS_REG(regs, 21), PT_REGS_REG(regs, 22), PT_REGS_REG(regs, 23));
	printk(KERN_INFO "x24: %016lx x25: %016lx x26: %016lx\n",
	       PT_REGS_REG(regs, 24), PT_REGS_REG(regs, 25), PT_REGS_REG(regs, 26));
	printk(KERN_INFO "x27: %016lx x28: %016lx x29: %016lx\n",
	       PT_REGS_REG(regs, 27), PT_REGS_REG(regs, 28), PT_REGS_REG(regs, 29));
}
