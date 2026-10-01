#ifndef CORBASE_CORMEMORYLIMIT_H_
#define CORBASE_CORMEMORYLIMIT_H_

//
// FILE            corMemoryLimit.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#include <stdint.h>                                   // uint64_t



// -----------------------------------------------------------------------------
//
// corMemoryLimit - the memory limit this process lives under, in bytes; 0 if there is none
//
// The counterpart of corCpuCount: what the container allows, not what the machine has.
//
// cgroup v2: the process's own cgroup is named in /proc/self/cgroup ("0::/kubepods/.../<container>"),
// and the limit that gets it killed may sit on it OR on any cgroup above it - a Kubernetes pod's limit
// is its parent's. So the walk goes from the process's cgroup up to the root, and the smallest
// memory.max on the way is the one ("max" = no limit at that level). Inside a container with its own
// cgroup namespace the path is "/", and the walk is the single file /sys/fs/cgroup/memory.max.
//
// cgroup v1: memory.limit_in_bytes at the mount root ("unlimited" is a huge page-rounded number).
//
extern uint64_t corMemoryLimit(void);

#endif  // CORBASE_CORMEMORYLIMIT_H_
