/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/shared/sysdep/ptrace_user.h
 */

#ifndef __SYSDEP_ARM64_PTRACE_USER_H
#define __SYSDEP_ARM64_PTRACE_USER_H

#include <generated/user_constants.h>

/*
 * Calculate offset in bytes for a register
 */
#define PT_OFFSET(r) ((r) * sizeof(long))

/*
 * ARM64 syscall register conventions
 * Syscall number in x8, return in x0
 */
#define PT_SYSCALL_NR(regs) ((regs)[HOST_X8])
#define PT_SYSCALL_NR_OFFSET PT_OFFSET(HOST_X8)

#define PT_SYSCALL_RET_OFFSET PT_OFFSET(HOST_X0)

/*
 * Register indices for IP and SP
 */
#define REGS_IP_INDEX HOST_PC
#define REGS_SP_INDEX HOST_SP

/*
 * ARM64 ptrace constants
 * Ensure PTRACE_SYSEMU_SINGLESTEP is defined (for compatibility)
 */
#ifndef PTRACE_SYSEMU_SINGLESTEP
#define PTRACE_SYSEMU_SINGLESTEP 32
#endif

/*
 * ARM64 doesn't support PTRACE_PEEKUSER/POKEUSER
 * Use wrapper functions instead
 */
long ptrace_peek_user(int pid, long offset);
long ptrace_poke_user(int pid, long offset, long value);

#define os_ptrace_peek_user(pid, addr) ptrace_peek_user(pid, addr)
#define os_ptrace_poke_user(pid, addr, data) ptrace_poke_user(pid, addr, data)

#define PTRACE_PEEKUSER_SUPPORTED 0

#endif /* __SYSDEP_ARM64_PTRACE_USER_H */
