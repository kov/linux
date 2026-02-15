/* SPDX-License-Identifier: GPL-2.0 */
/*
 * BPF JIT compatibility stubs for x86_64 UML
 *
 * Included via -include before bpf_jit_comp.c to stub out hardware-specific
 * features that don't exist in UML. The JIT's core code generation is pure
 * x86_64 instruction emission which works fine under UML.
 *
 * This header is included very early (before linux/types.h), so we can
 * only use primitive C types here, not bool/true/false.
 */
#ifndef __UM_X86_BPF_JIT_COMPAT_H
#define __UM_X86_BPF_JIT_COMPAT_H

/*
 * UML does not have hardware segment registers.
 * __KERNEL_DS is used by x86_clear_cpu_buffers() for the VERW instruction.
 * Irrelevant for UML.
 */
#define __KERNEL_DS 0

/*
 * x86_nops and ASM_NOP_MAX are used for NOP padding in text patching.
 * Provide the standard single-byte NOP for UML.
 */
#define ASM_NOP_MAX 1
static const unsigned char _um_nop1[] = { 0x90 };
static const unsigned char * const x86_nops[] = {
	(const unsigned char *)0,
	_um_nop1,
};

/*
 * CPU feature detection stubs.
 * UML runs as a userspace process; conservatively assume no special
 * CPU features (no retpoline, no BMI2, no IBT, etc.).
 * This produces slightly less optimized but always-correct JIT code.
 */
#define cpu_feature_enabled(x)	0
#define boot_cpu_has(x)		0

/* CPU feature constants referenced by the JIT */
#define X86_FEATURE_INDIRECT_THUNK_ITS	0
#define X86_FEATURE_RETPOLINE_LFENCE	0
#define X86_FEATURE_RETPOLINE		0
#define X86_FEATURE_CALL_DEPTH		0
#define X86_FEATURE_CLEAR_BHB_LOOP	0
#define X86_FEATURE_CLEAR_BHB_HW	0
#define X86_FEATURE_HYPERVISOR		0
#define X86_FEATURE_BMI2		0

/*
 * VSYSCALL_ADDR is used for BPF_PROBE_MEM bounds checking.
 * Use the same value as native x86_64.
 */
#define VSYSCALL_ADDR (-10UL << 20)

/*
 * Override the UML pt_regs with the native x86_64 layout for the JIT.
 *
 * The BPF JIT uses offsetof(struct pt_regs, ...) to encode register offsets
 * into generated machine code. These offsets must match the native x86_64
 * register save area since the JIT generates native instructions.
 *
 * Block UML's ptrace headers and provide the native x86_64 pt_regs layout.
 * This only affects the bpf_jit_comp.o compilation unit.
 */
#define __UM_X86_PTRACE_H
#define __UM_PTRACE_GENERIC_H

struct pt_regs {
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long bp;
	unsigned long bx;
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long ax;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	unsigned long orig_ax;
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
};

/*
 * Stub for user_mode() - pulled in transitively by sched/signal.h.
 * Not used by the JIT itself. With native pt_regs, check CS RPL bits.
 */
static inline int user_mode(struct pt_regs *regs)
{
	return !!(regs->cs & 3);
}

/*
 * Stub for instruction_pointer and other ptrace accessors that
 * headers may reference.
 */
#define instruction_pointer(regs) ((regs)->ip)
#define user_stack_pointer(regs) ((regs)->sp)

/* profile_pc is used by some profiling headers */
static inline unsigned long profile_pc(struct pt_regs *regs)
{
	return regs->ip;
}

/* Provide EMPTY_REGS stub */
#define EMPTY_REGS { }

/* PT_REGS accessors used by various headers */
#define PT_REGS_IP(r) ((r)->ip)
#define PT_REGS_SP(r) ((r)->sp)

#endif /* __UM_X86_BPF_JIT_COMPAT_H */
