/* SPDX-License-Identifier: GPL-2.0 */
/*
 * syscall_wrapper.h - UML arm64 syscall wrappers to syscall definitions
 */

#ifndef __ASM_SYSCALL_WRAPPER_H
#define __ASM_SYSCALL_WRAPPER_H

#include <asm/ptrace.h>
#include <sysdep/ptrace.h>

#define SC_ARM64_REGS_TO_ARGS(x, ...)					\
	__MAP(x, __SC_ARGS						\
	      ,,UPT_SYSCALL_ARG1(&regs->regs)				\
	      ,,UPT_SYSCALL_ARG2(&regs->regs)				\
	      ,,UPT_SYSCALL_ARG3(&regs->regs)				\
	      ,,UPT_SYSCALL_ARG4(&regs->regs)				\
	      ,,UPT_SYSCALL_ARG5(&regs->regs)				\
	      ,,UPT_SYSCALL_ARG6(&regs->regs))

#ifdef CONFIG_COMPAT

#define COMPAT_SYSCALL_DEFINEx(x, name, ...)						\
	asmlinkage long __arm64_compat_sys##name(const struct pt_regs *regs);		\
	ALLOW_ERROR_INJECTION(__arm64_compat_sys##name, ERRNO);				\
	static long __se_compat_sys##name(__MAP(x,__SC_LONG,__VA_ARGS__));		\
	static inline long __do_compat_sys##name(__MAP(x,__SC_DECL,__VA_ARGS__));	\
	asmlinkage long __arm64_compat_sys##name(const struct pt_regs *regs)		\
	{										\
		return __se_compat_sys##name(SC_ARM64_REGS_TO_ARGS(x,__VA_ARGS__));	\
	}										\
	static long __se_compat_sys##name(__MAP(x,__SC_LONG,__VA_ARGS__))		\
	{										\
		return __do_compat_sys##name(__MAP(x,__SC_DELOUSE,__VA_ARGS__));	\
	}										\
	static inline long __do_compat_sys##name(__MAP(x,__SC_DECL,__VA_ARGS__))

#define COMPAT_SYSCALL_DEFINE0(sname)							\
	asmlinkage long __arm64_compat_sys_##sname(const struct pt_regs *__unused);	\
	ALLOW_ERROR_INJECTION(__arm64_compat_sys_##sname, ERRNO);			\
	asmlinkage long __arm64_compat_sys_##sname(const struct pt_regs *__unused)

#define COND_SYSCALL_COMPAT(name) 							\
	asmlinkage long __arm64_compat_sys_##name(const struct pt_regs *regs);		\
	asmlinkage long __weak __arm64_compat_sys_##name(const struct pt_regs *regs)	\
	{										\
		return sys_ni_syscall();						\
	}

#endif /* CONFIG_COMPAT */

#define __SYSCALL_DEFINEx(x, name, ...)						\
	asmlinkage long __arm64_sys##name(const struct pt_regs *regs);		\
	ALLOW_ERROR_INJECTION(__arm64_sys##name, ERRNO);			\
	static long __se_sys##name(__MAP(x,__SC_LONG,__VA_ARGS__));		\
	static inline long __do_sys##name(__MAP(x,__SC_DECL,__VA_ARGS__));	\
	asmlinkage long __arm64_sys##name(const struct pt_regs *regs)		\
	{									\
		return __se_sys##name(SC_ARM64_REGS_TO_ARGS(x,__VA_ARGS__));	\
	}									\
	static long __se_sys##name(__MAP(x,__SC_LONG,__VA_ARGS__))		\
	{									\
		long ret = __do_sys##name(__MAP(x,__SC_CAST,__VA_ARGS__));	\
		__MAP(x,__SC_TEST,__VA_ARGS__);					\
		__PROTECT(x, ret,__MAP(x,__SC_ARGS,__VA_ARGS__));		\
		return ret;							\
	}									\
	static inline long __do_sys##name(__MAP(x,__SC_DECL,__VA_ARGS__))

#define SYSCALL_DEFINE0(sname)							\
	SYSCALL_METADATA(_##sname, 0);						\
	asmlinkage long __arm64_sys_##sname(const struct pt_regs *__unused);	\
	ALLOW_ERROR_INJECTION(__arm64_sys_##sname, ERRNO);			\
	asmlinkage long __arm64_sys_##sname(const struct pt_regs *__unused)

#define COND_SYSCALL(name)							\
	asmlinkage long __arm64_sys_##name(const struct pt_regs *regs);		\
	asmlinkage long __weak __arm64_sys_##name(const struct pt_regs *regs)	\
	{									\
		return sys_ni_syscall();					\
	}

asmlinkage long __arm64_sys_ni_syscall(const struct pt_regs *__unused);

#endif /* __ASM_SYSCALL_WRAPPER_H */
