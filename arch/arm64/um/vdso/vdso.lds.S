/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Linker script for 64-bit vDSO.
 * We #include the file to define the layout details.
 * Here we only choose the prelinked virtual address.
 *
 * This file defines the version script giving the user-exported symbols in
 * the DSO.  We can define local symbols here called VDSO* to make their
 * values visible using the asm-arm64/vdso.h macros from the kernel proper.
 */

#define VDSO_PRELINK 0
#include "vdso-layout.lds.S"

/*
 * This controls what userland symbols we export from the vDSO.
 */
/*
 * Make the sigreturn code visible to the kernel by aliasing its symbol
 * to a VDSO_* name (used by gen_vdso_offsets.sh).
 */
VDSO_sigtramp		= __kernel_rt_sigreturn;

VERSION {
	LINUX_2.6 {
	global:
		clock_gettime;
		__vdso_clock_gettime;
		gettimeofday;
		__vdso_gettimeofday;
		__kernel_rt_sigreturn;
	local: *;
	};
}
