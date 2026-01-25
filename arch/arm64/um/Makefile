# SPDX-License-Identifier: GPL-2.0
#
# Copyright (C) 2026 - AArch64 support for User Mode Linux
# Based on arch/x86/um/Makefile
#

#
# ARM64 is always 64-bit
#
BITS := 64

#
# Undefine ARM64 architecture-specific string functions
# UML uses generic implementations from lib/string.c instead of ARM64 assembly
#
KBUILD_CFLAGS += -U__HAVE_ARCH_STRRCHR -U__HAVE_ARCH_STRCHR \
	-U__HAVE_ARCH_STRCMP -U__HAVE_ARCH_STRNCMP \
	-U__HAVE_ARCH_STRLEN -U__HAVE_ARCH_STRNLEN \
	-U__HAVE_ARCH_MEMCMP -U__HAVE_ARCH_MEMCHR

#
# Core object files for ARM64 UML
#
obj-y = bugs_64.o delay.o fault.o \
	ptrace.o ptrace_64.o setjmp_64.o signal.o \
	stub_segv.o \
	sys_call_table_64.o syscall.o syscalls_64.o \
	sysrq_64.o tls_64.o vdso/ os-Linux/

# Use generic checksum library (must be obj-y not lib-y to ensure it's linked)
obj-y += ../../../lib/checksum.o

# Disable hardening for stub code (runs in userspace context)
UNPROFILE_OBJS := stub_segv.o
CFLAGS_stub_segv.o := $(CFLAGS_NO_HARDENING)

#
# Don't link ARM64 assembly library functions - they use native ARM64
# headers with assembly directives that conflict with UML. UML uses
# standard C implementations instead.
#
# subarch-y = ../lib/clear_page.o ../lib/copy_page.o \
# 	../lib/memchr.o ../lib/memcmp.o ../lib/strcmp.o ../lib/strlen.o \
# 	../lib/strncmp.o

#
# Module support - disabled for now, needs ARM64-specific relocation support
#
# subarch-$(CONFIG_MODULES) += ../kernel/module.o

#
# User-space object files (compiled with user CFLAGS)
#
USER_OBJS := bugs_64.o fault.o

#
# Generate user_constants.h from user-offsets.c
# This provides HOST_* register offset constants
#
$(obj)/user-offsets.s: c_flags = -Wp,-MD,$(depfile) $(USER_CFLAGS) \
	-Iarch/arm64/include/generated
targets += user-offsets.s

include/generated/user_constants.h: $(obj)/user-offsets.s FORCE
	$(call filechk,offsets,__USER_CONSTANT_H__)

#
# Include common UML build rules
#
include $(srctree)/arch/um/scripts/Makefile.rules
