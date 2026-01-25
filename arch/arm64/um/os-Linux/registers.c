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
#include <sysdep/ptrace_user.h>
#include <registers.h>

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
		 * unmodified regs buffer (caller will have zeroed it).
		 */
		if (errno == EINVAL)
			return 0;
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
	 * If SVE is not available, fall back to NT_ARM_VFP (FPSIMD)
	 * This is the standard 32x128-bit SIMD register set
	 */
	if (ret == -EINVAL || ret == -ENODEV) {
		ptrace_regset = NT_ARM_VFP;
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
	 * for standard FPSIMD (32 x 128-bit registers = 512 bytes)
	 */
	if (ret == -EINVAL) {
		host_fp_size = 512;  /* 32 x 128-bit FPSIMD registers */
		ptrace_regset = NT_ARM_VFP;
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
int ptrace_getregs(int pid, unsigned long *regs)
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
int ptrace_setregs(int pid, unsigned long *regs)
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
