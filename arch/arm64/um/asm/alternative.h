/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#ifndef __UM_ARM64_ALTERNATIVE_H
#define __UM_ARM64_ALTERNATIVE_H

/*
 * Use the native ALTERNATIVE macros. The .altinstructions section is
 * emitted but never applied (UML does not patch code at runtime), so
 * the original instruction sequence is always executed and capability
 * checks always take the "capability absent" path.
 */
#include <asm/alternative-macros.h>

#ifndef __ASSEMBLY__

#include <linux/types.h>

/*
 * ARM64 alternative instructions (stub for UML)
 * In real ARM64, this allows runtime code patching based on CPU features
 * For UML, we don't need this functionality
 */

struct alt_instr {
	__s32 orig_offset;	/* offset to original instruction */
	__s32 alt_offset;	/* offset to replacement instruction */
	__u16 cpufeature;	/* cpufeature bit set for replacement */
	__u8  orig_len;		/* size of original instruction(s) */
	__u8  alt_len;		/* size of new instruction(s), <= orig_len */
};

void alt_cb_patch_nops(struct alt_instr *alt, __le32 *origptr,
		       __le32 *updptr, int nr_inst);

#endif /* __ASSEMBLY__ */

#endif /* __UM_ARM64_ALTERNATIVE_H */
