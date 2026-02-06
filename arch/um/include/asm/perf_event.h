/* SPDX-License-Identifier: GPL-2.0 */
/*
 * UML perf_event.h - Override subarch perf_event.h
 *
 * UML's pt_regs structure differs from the native arch, so we cannot use
 * the native perf_arch_fetch_caller_regs macro. Leave it undefined so that
 * the generic no-op in linux/perf_event.h is used instead.
 */
#ifndef __UM_PERF_EVENT_H
#define __UM_PERF_EVENT_H

/* No arch-specific perf definitions for UML */

#endif /* __UM_PERF_EVENT_H */
