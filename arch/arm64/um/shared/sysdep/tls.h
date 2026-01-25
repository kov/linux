/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __SYSDEP_ARM64_TLS_H
#define __SYSDEP_ARM64_TLS_H

/*
 * TLS support for ARM64 UML
 *
 * ARM64 uses TPIDR_EL0 register for TLS, managed via ptrace
 */

/* TLS is not used in user-mode helper code */
#define do_set_thread_area(p, v) (-ENOSYS)
#define do_get_thread_area(p, v) (-ENOSYS)

#endif /* __SYSDEP_ARM64_TLS_H */
