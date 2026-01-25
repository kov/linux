/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_POINTER_AUTH_H
#define __UM_ARM64_POINTER_AUTH_H

/*
 * Pointer authentication stubs for ARM64 UML
 *
 * UML runs in user mode and doesn't have access to pointer authentication
 * features (PACIA, PACIB, etc.). Provide stub implementations.
 */

struct task_struct;

/*
 * Feature detection - always return false for UML
 */
static inline bool system_supports_address_auth(void)
{
	return false;
}

static inline bool system_supports_generic_auth(void)
{
	return false;
}

/*
 * Instruction barrier - use compiler barrier for UML
 */
#define isb() __asm__ __volatile__("" : : : "memory")

/*
 * Pointer authentication operations - no-ops for UML
 */
#define ptrauth_thread_init_user(tsk)		do { } while (0)
#define ptrauth_thread_switch_user(tsk)		do { } while (0)
#define ptrauth_thread_init_kernel(tsk)		do { } while (0)
#define ptrauth_thread_switch_kernel(tsk)	do { } while (0)
#define ptrauth_keys_install_user(keys)		do { } while (0)
#define ptrauth_keys_init_user(keys)		do { } while (0)
#define ptrauth_enable()			do { } while (0)

#endif /* __UM_ARM64_POINTER_AUTH_H */
