/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_UM_TOPOLOGY_H
#define _ASM_UM_TOPOLOGY_H

/*
 * UML NUMA topology: single node, all CPUs on node 0.
 *
 * The generic asm/topology.h only provides these definitions when
 * CONFIG_NUMA is disabled. UML supports CONFIG_NUMA for syscall
 * compatibility (mbind, set_mempolicy, etc.) but always presents
 * a single NUMA node, so we provide trivial definitions here.
 */

#ifdef CONFIG_NUMA

#define cpu_to_node(cpu)	((void)(cpu), 0)
#define cpumask_of_node(node)	((void)(node), cpu_online_mask)
#define pcibus_to_node(bus)	((void)(bus), -1)

#define set_numa_node(node)
#define set_cpu_numa_node(cpu, node)
#define cpu_to_mem(cpu)		((void)(cpu), 0)

#endif /* CONFIG_NUMA */

#include <asm-generic/topology.h>

#endif /* _ASM_UM_TOPOLOGY_H */
