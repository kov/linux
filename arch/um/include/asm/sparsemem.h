/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_UM_SPARSEMEM_H
#define __ASM_UM_SPARSEMEM_H

/*
 * UML does not use sparsemem. Provide an empty header to prevent
 * the arm64 version (which pulls in pgtable-prot.h and causes type
 * conflicts) from being included when CONFIG_NUMA is enabled.
 */

#endif
