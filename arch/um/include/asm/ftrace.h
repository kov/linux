/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __UM_FTRACE_H
#define __UM_FTRACE_H

#ifdef CONFIG_UML_ARM64
/*
 * ARM64 UML syscalls use __arm64_sys_ prefix in the syscall table,
 * while SYSCALL_METADATA generates names with sys_ prefix.
 * Skip the "__arm64_" (8 chars) prefix when matching.
 */
#define ARCH_HAS_SYSCALL_MATCH_SYM_NAME

static inline bool arch_syscall_match_sym_name(const char *sym,
					       const char *name)
{
	return !strcmp(sym + 8, name);
}
#endif /* CONFIG_UML_ARM64 */

#endif /* __UM_FTRACE_H */
