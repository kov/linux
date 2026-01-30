// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Library functions for ARM64 UML
 */

#include <linux/export.h>
#include <linux/string.h>
#include <asm/checksum.h>
#include <asm/delay.h>

/*
 * Delay functions
 * UML runs as userspace process, so delays are approximate
 */
void __const_udelay(unsigned long xloops)
{
	/* For UML: skip delay to avoid hanging in panic handler */
	/* TODO: implement proper delay using os_usleep_file() */
	(void)xloops;
}
EXPORT_SYMBOL(__const_udelay);

void __udelay(unsigned long usecs)
{
	__const_udelay(usecs * 0x10C7UL); /* 2**32 / 1000000 (rounded up) */
}
EXPORT_SYMBOL(__udelay);

void __ndelay(unsigned long nsecs)
{
	__const_udelay(nsecs * 0x5UL); /* Approximate */
}
EXPORT_SYMBOL(__ndelay);

void __delay(unsigned long loops)
{
	volatile unsigned long i;

	for (i = 0; i < loops; i++)
		cpu_relax();
}
EXPORT_SYMBOL(__delay);

/*
 * Checksum functions
 * csum_partial, ip_compute_csum, csum_tcpudp_nofold are provided by lib/checksum.c
 * Only provide csum_ipv6_magic which is not in the generic library
 */

#include <linux/in6.h>
#include <net/checksum.h>

/*
 * IPv6 checksum
 */
__sum16 csum_ipv6_magic(const struct in6_addr *saddr,
			const struct in6_addr *daddr,
			__u32 len, __u8 proto, __wsum csum)
{
	int carry;
	__u32 ulen;
	__u32 uproto;
	__u32 sum = (__force __u32)csum;

	sum += (__force __u32)saddr->s6_addr32[0];
	carry = (sum < (__force __u32)saddr->s6_addr32[0]);
	sum += carry;

	sum += (__force __u32)saddr->s6_addr32[1];
	carry = (sum < (__force __u32)saddr->s6_addr32[1]);
	sum += carry;

	sum += (__force __u32)saddr->s6_addr32[2];
	carry = (sum < (__force __u32)saddr->s6_addr32[2]);
	sum += carry;

	sum += (__force __u32)saddr->s6_addr32[3];
	carry = (sum < (__force __u32)saddr->s6_addr32[3]);
	sum += carry;

	sum += (__force __u32)daddr->s6_addr32[0];
	carry = (sum < (__force __u32)daddr->s6_addr32[0]);
	sum += carry;

	sum += (__force __u32)daddr->s6_addr32[1];
	carry = (sum < (__force __u32)daddr->s6_addr32[1]);
	sum += carry;

	sum += (__force __u32)daddr->s6_addr32[2];
	carry = (sum < (__force __u32)daddr->s6_addr32[2]);
	sum += carry;

	sum += (__force __u32)daddr->s6_addr32[3];
	carry = (sum < (__force __u32)daddr->s6_addr32[3]);
	sum += carry;

	ulen = (__force __u32)htonl((__u32)len);
	sum += ulen;
	carry = (sum < ulen);
	sum += carry;

	uproto = (__force __u32)htonl(proto);
	sum += uproto;
	carry = (sum < uproto);
	sum += carry;

	return csum_fold((__force __wsum)sum);
}
EXPORT_SYMBOL(csum_ipv6_magic);
