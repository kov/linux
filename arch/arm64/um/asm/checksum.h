/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Use generic checksum implementations for UML
 */

#ifndef __UM_ARM64_CHECKSUM_H
#define __UM_ARM64_CHECKSUM_H

#include <linux/types.h>
#include <linux/in6.h>

/* We provide our own IPv6 checksum implementation */
#define _HAVE_ARCH_IPV6_CSUM 1

/* Declare arch-specific checksum functions */
__sum16 csum_ipv6_magic(const struct in6_addr *saddr,
			const struct in6_addr *daddr,
			__u32 len, __u8 proto, __wsum csum);

#include <asm-generic/checksum.h>

#endif /* __UM_ARM64_CHECKSUM_H */
