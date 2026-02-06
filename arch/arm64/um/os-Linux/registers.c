// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 - AArch64 support for User Mode Linux
 * Based on arch/x86/um/os-Linux/registers.c
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <linux/elf.h>
#include <longjmp.h>
#include <stdbool.h>
#include <signal.h>
#include <asm/ptrace.h>
#include <stub-data.h>
#include <sysdep/stub.h>
#include <sysdep/ptrace_user.h>
#include <ptrace_user.h>
#include <registers.h>
#include <os.h>
#include <skas.h>

/*
 * ARM64 FP register handling
 *
 * ARM64 has two FP modes:
 * - FPSIMD: 32 x 128-bit registers (fixed, ~512 bytes)
 * - SVE: Variable length vectors (128-2048 bits per register)
 *
 * We detect which is available and use NT_ARM_SVE if present,
 * otherwise fall back to NT_ARM_VFP (FPSIMD).
 */

static unsigned long ptrace_regset;
unsigned long host_fp_size;

/*
 * Get FP/SIMD registers from host process
 */
int get_fp_registers(int pid, unsigned long *regs)
{
	struct iovec iov = {
		.iov_base = regs,
		.iov_len = host_fp_size,
	};

	if (ptrace(PTRACE_GETREGSET, pid, ptrace_regset, &iov) < 0) {
		/*
		 * EINVAL means the process hasn't used FP registers yet,
		 * so the FP context hasn't been initialized by the kernel.
		 * This is not a fatal error - just return success with
		 * zeroed regs buffer.
		 */
		if (errno == EINVAL) {
			memset(regs, 0, host_fp_size);
			return 0;
		}
		return -errno;
	}
	return 0;
}

/*
 * Set FP/SIMD registers in host process
 */
int put_fp_registers(int pid, unsigned long *regs)
{
	struct iovec iov = {
		.iov_base = regs,
		.iov_len = host_fp_size,
	};

	if (ptrace(PTRACE_SETREGSET, pid, ptrace_regset, &iov) < 0) {
		/*
		 * EINVAL means the process hasn't used FP registers yet,
		 * so the FP context hasn't been initialized by the kernel.
		 * This is not a fatal error - just skip FP restore.
		 */
		if (errno == EINVAL)
			return 0;
		return -errno;
	}
	return 0;
}

/*
 * Initialize and detect FP register size
 * Called once at startup to determine what FP features the host has
 */
int arch_init_registers(int pid)
{
	struct iovec iov = {
		/* Allocate plenty of space for SVE (can be large) */
		.iov_len = 2 * 1024 * 1024,
	};
	int ret;

	iov.iov_base = mmap(NULL, iov.iov_len, PROT_WRITE | PROT_READ,
			    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (iov.iov_base == MAP_FAILED) {
		return -ENOMEM;
	}

	/*
	 * Try NT_ARM_SVE first (Scalable Vector Extension)
	 * This is the modern way and includes FPSIMD state
	 */
	ptrace_regset = NT_ARM_SVE;
	ret = ptrace(PTRACE_GETREGSET, pid, ptrace_regset, &iov);
	if (ret) {
		ret = -errno;
	}

	/*
	 * If SVE is not available, fall back to NT_PRFPREG (FPSIMD)
	 * This is the standard 32x128-bit SIMD register set
	 */
	if (ret == -EINVAL || ret == -ENODEV) {
		ptrace_regset = NT_PRFPREG;
		iov.iov_len = 2 * 1024 * 1024;
		ret = ptrace(PTRACE_GETREGSET, pid, ptrace_regset, &iov);
		if (ret) {
			ret = -errno;
		}
	}

	munmap(iov.iov_base, 2 * 1024 * 1024);

	/*
	 * If both SVE and VFP failed with EINVAL, it likely means the
	 * child process hasn't used FP registers yet. Use a default size
	 * for standard FPSIMD (32 x 128-bit FPSIMD registers + FPSR/FPCR = 528 bytes)
	 */
	if (ret == -EINVAL) {
		host_fp_size = 528;
		ptrace_regset = NT_PRFPREG;
		ret = 0;  /* Not a fatal error */
	} else {
		/*
		 * Save the actual size returned by ptrace
		 * This tells us how much space we need for FP register save/restore
		 */
		host_fp_size = iov.iov_len;
	}

	return ret;
}

/*
 * Get all general-purpose registers using ptrace
 */
int ptrace_getregs(long pid, unsigned long *regs)
{
	struct iovec iov = {
		.iov_base = regs,
		.iov_len = MAX_REG_NR * sizeof(unsigned long),
	};

	if (ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov) < 0)
		return -errno;
	return 0;
}

/*
 * Set all general-purpose registers using ptrace
 */
int ptrace_setregs(long pid, unsigned long *regs)
{
	struct iovec iov = {
		.iov_base = regs,
		.iov_len = MAX_REG_NR * sizeof(unsigned long),
	};

	if (ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &iov) < 0)
		return -errno;
	return 0;
}

/*
 * Get register values from jmp_buf (for thread context switching)
 * ARM64 jmp_buf contains: x19-x29, x30 (LR), sp
 */
unsigned long get_thread_reg(int reg, jmp_buf *buf)
{
	switch (reg) {
	case HOST_IP:
		/* Link register (x30) serves as return address */
		return buf[0]->__x30;
	case HOST_SP:
		/* Stack pointer */
		return buf[0]->__sp;
	case HOST_FP:
		/* Frame pointer (x29) */
		return buf[0]->__x29;
	default:
		printk(UM_KERN_ERR "get_thread_reg - unknown register %d\n", reg);
		return 0;
	}
}

/*
 * ARM64 doesn't support PTRACE_PEEKUSER/POKEUSER
 * Provide wrappers using PTRACE_GETREGSET/SETREGSET instead
 */

long ptrace_peek_user(int pid, long offset)
{
	int reg_index = offset / sizeof(long);

	if (reg_index >= MAX_REG_NR) {
		return -EINVAL;
	}

	/* Special handling for syscall number register (x8) */
	if (reg_index == HOST_X8) {
		int syscall_nr;
		struct iovec iov = {
			.iov_base = &syscall_nr,
			.iov_len = sizeof(syscall_nr),
		};

		if (ptrace(PTRACE_GETREGSET, pid, NT_ARM_SYSTEM_CALL, &iov) < 0) {
			return -errno;
		}

		return syscall_nr;
	}

	/* Regular register access */
	unsigned long regs[MAX_REG_NR];
	struct iovec iov = {
		.iov_base = regs,
		.iov_len = MAX_REG_NR * sizeof(unsigned long),
	};

	if (ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov) < 0) {
		return -errno;
	}

	return regs[reg_index];
}

long ptrace_poke_user(int pid, long offset, long value)
{
	int reg_index = offset / sizeof(long);

	if (reg_index >= MAX_REG_NR) {
		return -EINVAL;
	}

	/* Special handling for syscall number register (x8) */
	if (reg_index == HOST_X8) {
		int syscall_nr = (int)value;
		struct iovec iov = {
			.iov_base = &syscall_nr,
			.iov_len = sizeof(syscall_nr),
		};

		if (ptrace(PTRACE_SETREGSET, pid, NT_ARM_SYSTEM_CALL, &iov) < 0) {
			return -errno;
		}

		return 0;
	}

	/* Regular register access */
	unsigned long regs[MAX_REG_NR];
	struct iovec iov = {
		.iov_base = regs,
		.iov_len = MAX_REG_NR * sizeof(unsigned long),
	};

	/* Read current register state */
	if (ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov) < 0) {
		return -errno;
	}

	/* Modify the specified register */
	regs[reg_index] = value;

	/* Write back the modified register state */
	if (ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &iov) < 0) {
		return -errno;
	}

	return 0;
}

#ifndef NT_ARM_TLS
#define NT_ARM_TLS 0x403
#endif

int ptrace_set_tls(int pid, unsigned long tls)
{
	struct iovec iov;
	iov.iov_base = &tls;
	iov.iov_len = sizeof(tls);
	return ptrace(PTRACE_SETREGSET, pid, NT_ARM_TLS, &iov);
}


int os_dump_regs(int pid)
{
	unsigned long regs[MAX_REG_NR];
	int i;
	struct iovec iov = { .iov_base = regs, .iov_len = sizeof(regs) };

	if (ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov) < 0)
		return -errno;

	printk(UM_KERN_ERR "Stub registers -\n");
	for (i = 0; i < ARRAY_SIZE(regs); i++) {
		/* ARM64 dump might be too verbose if we dump all 34 regs,
		 * maybe just dump interesting ones?
		 * For consistency with x86, we dump loop.
		 */
		printk(UM_KERN_ERR "\t%d\t: %lx\n", i, regs[i]);
	}

	return 0;
}

static bool arm64_sig_is_store(struct mm_id *mm_id, unsigned long pc,
			       unsigned int *insn_out)
{
	unsigned long aligned;
	unsigned long word;
	unsigned int insn;

	if (!mm_id || pc == 0)
		return false;

	aligned = pc & ~0x7UL;
	errno = 0;
	word = ptrace(PTRACE_PEEKDATA, mm_id->pid, (void *)aligned, 0);
	if (errno)
		return false;

	insn = (pc & 0x4) ? (unsigned int)(word >> 32) : (unsigned int)word;
	if (insn_out)
		*insn_out = insn;

	/*
	 * Heuristic decode for common A64 load/store encodings.
	 *
	 * Load/Store Exclusive & Compare-And-Swap (0x08 class):
	 *   bits[29:24] = 001000
	 *   CAS/CASP (bit[23]=1, bit[21]=1): always read-modify-write
	 *   STXR/STLXR/STLR etc (bit[22]=0): store
	 *   LDXR/LDAXR/LDAR etc (bit[22]=1): load
	 *
	 * For unsigned immediate (0x39) and unscaled/pre/post/register
	 * offset (0x38) classes, the decode depends on the V bit (bit 26):
	 *
	 * When V=0 (integer), bits [23:22] encode opc[1:0]:
	 *   opc=00 → store (STR/STRB/STRH)
	 *   opc=01 → unsigned load (LDR/LDRB/LDRH)
	 *   opc=10 → signed load (LDRSW/LDRSB/LDRSH)
	 *   opc=11 → signed load (32-bit) or PRFM
	 * Only opc==00 is a store.
	 * Exception: Atomic memory operations (V=0, bit[21]=1,
	 *   bits[11:10]=00) like SWP, LDADD, LDCLR etc. are all
	 *   read-modify-write and treated as stores.
	 *
	 * When V=1 (SIMD/FP), the opc field encodes size, not direction:
	 *   opc=00 → STR (8/16/32/64-bit)
	 *   opc=01 → LDR (8/16/32/64-bit)
	 *   opc=10 → STR Q (128-bit)
	 *   opc=11 → LDR Q (128-bit)
	 * Bit 22 alone is the load/store indicator.
	 *
	 * For load/store pairs (0x29 class), bit 22 is the L-bit
	 * directly (L=0 store, L=1 load) for both integer and SIMD.
	 *
	 * DC ZVA (cache line zero) is a write-like operation.
	 */

	/* Load/Store Exclusive & CAS: bits[29:24] = 001000 */
	if ((insn & 0x3F000000) == 0x08000000) {
		/* CAS/CASP (bit[23]=1, bit[21]=1): read-modify-write */
		if ((insn & (1U << 23)) && (insn & (1U << 21)))
			return true;
		/* Others: bit[22] is L-bit (L=0 store, L=1 load) */
		return !(insn & (1U << 22));
	}

	if (((insn & 0x3b000000) == 0x39000000) ||
	    ((insn & 0x3b000000) == 0x38000000)) {
		/* Atomic memory ops: V=0, bit[21]=1, bits[11:10]=00 */
		if (!(insn & (1U << 26)) && (insn & (1U << 21)) &&
		    !(insn & (3U << 10)))
			return true;
		if (insn & (1U << 26))	/* V=1: SIMD/FP */
			return !(insn & (1U << 22));
		else			/* V=0: integer */
			return (insn & (3U << 22)) == 0;
	}

	if ((insn & 0x3b000000) == 0x29000000)
		return !(insn & (1U << 22));

	if ((insn & 0xffffffe0) == 0xd50b7420)
		return true;

	return false;
}

void os_get_faultinfo(int pid, struct faultinfo *fi, void *si,
		      struct uml_pt_regs *regs)
{
	unsigned long addr;
	unsigned long pc;
	siginfo_t *siginfo = si;

	memset(fi, 0, sizeof(*fi));
	if (!siginfo)
		return;

	addr = (unsigned long)siginfo->si_addr;
	pc = regs ? regs->gp[32] : 0;

	fi->cr2 = addr;
	fi->error_code = 0;
	/*
	 * Heuristic: if the faulting address is the current PC, treat
	 * it as an instruction abort, otherwise a data abort.
	 */
	if (pc && addr == pc)
		fi->trap_no = ESR_ELx_EC_IABT_LOW;
	else
		fi->trap_no = ESR_ELx_EC_DABT_LOW;

	if (fi->trap_no == ESR_ELx_EC_DABT_LOW &&
	    arm64_sig_is_store(current_mm_id(), pc, NULL))
		fi->error_code |= ESR_ELx_WNR;
}

/* ARM64 specific Globals for PAC/TLS */
static bool arm64_pac_valid;
static struct user_pac_address_keys arm64_pac_keys;

int os_set_thread_area(void *tls, int pid);

void os_arch_process_handshake(int pid, struct uml_pt_regs *regs,
			       struct stub_data *proc_data)
{
	/* Set TLS via ptrace */
	unsigned long tls_val;
	/* Hack removed: correct sync implemented below */

	tls_val = regs->tpidr_el0;

	if (proc_data->arch_data.tpidr_el0 != tls_val) {
		proc_data->arch_data.tpidr_el0 = tls_val;
		proc_data->arch_data.sync |= STUB_SYNC_TPIDR_EL0;
	}

	os_set_thread_area(&tls_val, pid);

	/* Set PAC Keys */
	if (arm64_pac_valid) {
		struct iovec iov;
		iov.iov_base = &arm64_pac_keys;
		iov.iov_len = sizeof(arm64_pac_keys);
		ptrace(PTRACE_SETREGSET, pid, NT_ARM_PAC_MASK, &iov);
	}
}

int os_get_thread_area(void *tls, int pid);

void os_arch_post_wait_handshake(int pid, struct uml_pt_regs *regs)
{
	unsigned long tls;
	if (!os_get_thread_area(&tls, pid))
		regs->tpidr_el0 = tls;
}
