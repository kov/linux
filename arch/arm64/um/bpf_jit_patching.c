// SPDX-License-Identifier: GPL-2.0-only
/*
 * BPF JIT text patching for ARM64 UML
 *
 * On UML, JIT code lives in regular host memory (allocated via execmem_alloc
 * which uses vmalloc with PROT_EXEC). No fixmap or special text patching
 * is needed — we can just write directly.
 */
#include <linux/bug.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <asm/insn.h>
#include <asm/cacheflush.h>

int aarch64_insn_read(void *addr, u32 *insnp)
{
	*insnp = le32_to_cpu(*(__le32 *)addr);
	return 0;
}

void *aarch64_insn_copy(void *dst, void *src, size_t len)
{
	if ((uintptr_t)dst & 0x3)
		return NULL;

	memcpy(dst, src, len);
	flush_icache_range((uintptr_t)dst, (uintptr_t)dst + len);
	return dst;
}

void *aarch64_insn_set(void *dst, u32 insn, size_t len)
{
	if ((uintptr_t)dst & 0x3)
		return NULL;

	memset32(dst, insn, len / 4);
	flush_icache_range((uintptr_t)dst, (uintptr_t)dst + len);
	return dst;
}

int aarch64_insn_patch_text_nosync(void *addr, u32 insn)
{
	u32 *tp = addr;

	if ((uintptr_t)tp & 0x3)
		return -EINVAL;

	*tp = cpu_to_le32(insn);
	flush_icache_range((uintptr_t)tp,
			   (uintptr_t)tp + AARCH64_INSN_SIZE);
	return 0;
}
