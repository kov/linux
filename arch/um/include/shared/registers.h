/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2004 PathScale, Inc
 */

#ifndef __REGISTERS_H
#define __REGISTERS_H

#include <sysdep/ptrace.h>
#include <sysdep/archsetjmp.h>

extern int init_pid_registers(int pid);
extern void get_safe_registers(unsigned long *regs, unsigned long *fp_regs);
extern int get_fp_registers(int pid, unsigned long *regs);
extern int put_fp_registers(int pid, unsigned long *regs);
extern int os_dump_regs(int pid);
struct faultinfo;
struct uml_pt_regs;
struct stub_data;
extern void os_get_faultinfo(int pid, struct faultinfo *fi, void *si,
			     struct uml_pt_regs *regs);
extern void os_arch_process_handshake(int pid, struct uml_pt_regs *regs,
				      struct stub_data *proc_data);
extern void os_arch_post_wait_handshake(int pid, struct uml_pt_regs *regs);
extern int ptrace_set_tls(int pid, unsigned long tls);

#endif
