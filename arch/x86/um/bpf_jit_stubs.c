// SPDX-License-Identifier: GPL-2.0-only
/*
 * BPF JIT stubs for x86_64 UML
 *
 * UML lacks the x86 alternatives/text-patching infrastructure.
 * Provide minimal implementations for symbols the JIT references.
 */
#include <linux/types.h>
#include <linux/string.h>
#include <asm/cfi.h>

/* CFI is not supported under UML - always off */
enum cfi_mode cfi_mode = CFI_OFF;

/*
 * text_poke_set - memset into executable memory.
 * In UML, JIT memory is regular process memory, so just use memset.
 */
void *text_poke_set(void *addr, int c, size_t len)
{
	return memset(addr, c, len);
}

/*
 * smp_text_poke_single - atomically patch a single instruction.
 * In UML (single-process), just memcpy the new instruction in.
 */
void smp_text_poke_single(void *addr, const void *opcode, size_t len,
			   const void *emulate)
{
	memcpy(addr, opcode, len);
}
