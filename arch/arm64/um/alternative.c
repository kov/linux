// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 */

#include <asm/alternative.h>

/*
 * The alternative_has_cap_*() checks emit .altinstructions entries
 * that reference this callback. UML never applies alternatives, so it
 * must only exist to satisfy the link.
 */
void alt_cb_patch_nops(struct alt_instr *alt, __le32 *origptr,
		       __le32 *updptr, int nr_inst)
{
}
