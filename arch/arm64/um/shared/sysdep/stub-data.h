/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __SYSDEP_ARM64_STUB_DATA_H
#define __SYSDEP_ARM64_STUB_DATA_H

/*
 * Stub data structures for ARM64 UML
 * Used for seccomp stub communication
 */

#define STUB_SYNC_TPIDR_EL0 (1 << 0)

struct stub_data_arch {
	int sync;  /* Sync flags for TLS updates */
	unsigned long tpidr_el0;  /* Thread pointer (TLS) */
};

#endif /* __SYSDEP_ARM64_STUB_DATA_H */
