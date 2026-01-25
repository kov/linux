// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/stub_segv.c
 */

#include <sysdep/stub.h>
#include <sysdep/faultinfo.h>
#include <sysdep/mcontext.h>
#include <sys/ucontext.h>

void __attribute__ ((__section__ (".__syscall_stub")))
stub_segv_handler(int sig, siginfo_t *info, void *p)
{
	struct faultinfo *f = get_stub_data();
	ucontext_t *uc = p;

	/*
	 * Prefer ESR/FAR from mcontext so WNR is available for page faults.
	 * Fall back to siginfo when the mcontext data is missing.
	 */
	if (uc)
		GET_FAULTINFO_FROM_MC((*f), &uc->uc_mcontext);

	if (info) {
		if (!f->cr2)
			f->cr2 = (unsigned long)info->si_addr;
		if (!f->error_code)
			f->error_code = info->si_code;
		if (!f->trap_no)
			f->trap_no = sig;
	}

	/* ARM64 DEBUG: Write sentinel to verify handler ran */
	*((unsigned long *)f + 3) = 0xDEADBEEFCAFEBABEUL;

	trap_myself();
}
