// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * System call table for ARM64
 * Based on arch/x86/um/sys_call_table_64.c
 */

#include <linux/linkage.h>
#include <linux/sys.h>
#include <linux/cache.h>
#include <asm/syscall.h>

extern asmlinkage long __arm64_sys_ni_syscall(const struct pt_regs *regs);

#define __SYSCALL_NORETURN __SYSCALL

#define __SYSCALL_WITH_COMPAT(nr, native, compat) __SYSCALL(nr, native)

#undef __SYSCALL
#define __SYSCALL(nr, sym) \
	extern asmlinkage long __arm64_##sym(const struct pt_regs *regs);
#include <asm/syscall_table_64.h>
#undef __SYSCALL

#undef __SYSCALL_WITH_COMPAT
#define __SYSCALL_WITH_COMPAT(nr, native, compat) __SYSCALL(nr, native)
#define __SYSCALL(nr, sym) [nr] = __arm64_##sym,

const sys_call_ptr_t sys_call_table[__NR_syscalls] ____cacheline_aligned = {
	[0 ... __NR_syscalls - 1] = __arm64_sys_ni_syscall,
#include <asm/syscall_table_64.h>
};

int syscall_table_size = sizeof(sys_call_table);
