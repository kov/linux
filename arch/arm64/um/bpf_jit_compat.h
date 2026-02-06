/* SPDX-License-Identifier: GPL-2.0 */
/*
 * BPF JIT compatibility stubs for ARM64 UML
 *
 * Included via -include before bpf_jit_comp.c to stub out hardware-specific
 * features that don't exist in UML. The JIT's core code generation is pure
 * aarch64 instruction emission which works fine under UML.
 *
 * This header is included very early (before linux/types.h), so we can
 * only use primitive C types here, not bool/true/false.
 */
#ifndef __UM_BPF_JIT_COMPAT_H
#define __UM_BPF_JIT_COMPAT_H

/*
 * Block headers that reference hardware features UML doesn't have.
 * These are pulled in transitively but not used by the JIT itself.
 */
#define __LINUX_ARM_SMCCC_H	/* block arm-smccc.h */
#define __ASM_RSI_H_		/* block rsi.h */
#define __ASM_RSI_CMDS_H	/* block rsi_cmds.h */

/* Provide the minimal smccc types/functions that cpufeature.h needs */
enum arm_smccc_conduit {
	SMCCC_CONDUIT_NONE,
	SMCCC_CONDUIT_SMC,
	SMCCC_CONDUIT_HVC,
};

#define ARM_SMCCC_ARCH_WORKAROUND_3 0

/* --- CPU capability detection stubs --- */

#define ARM64_HAS_VIRT_HOST_EXTN	0
#define ARM64_HAS_SB			0
#define ARM64_HAS_LSE_ATOMICS		0

#define cpus_have_cap(x)		0
#define alternative_has_cap_likely(x)	0

/* --- Spectre BHB mitigation stubs --- */

#define SPECTRE_VULNERABLE 0
#define SCOPE_SYSTEM 0

#define get_spectre_bhb_loop_value()	0
#define arm64_get_spectre_v2_state()	SPECTRE_VULNERABLE
#define supports_clearbhb(scope)	0
#define is_spectre_bhb_fw_mitigated()	0
#define __nospectre_bhb			1
#define arm_smccc_1_1_get_conduit()	SMCCC_CONDUIT_NONE

/* --- Realm Management Extension stubs --- */
#define is_realm_world()		0

/*
 * Override ex_handler_bpf to use UML's pt_regs layout.
 * UML's pt_regs wraps uml_pt_regs which wraps user_pt_regs.
 * The native arm64 pt_regs has .regs[] and .pc directly.
 * We need to access them via regs->regs.gp[] and set IP.
 */
#define UM_BPF_JIT_COMPAT_EXTABLE

/*
 * BPF JIT emits MRS SP_EL0 to get 'current' — on UML we load from cpu_tasks[].
 * Forward-declare it here so bpf_jit_comp.c can reference it.
 */
struct task_struct;
extern struct task_struct *cpu_tasks[];

#endif /* __UM_BPF_JIT_COMPAT_H */
