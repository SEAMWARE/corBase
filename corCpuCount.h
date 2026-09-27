#ifndef CORBASE_CORCPUCOUNT_H_
#define CORBASE_CORCPUCOUNT_H_

//
// FILE            corCpuCount.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//



// -----------------------------------------------------------------------------
//
// corCpuCount - how many CPUs this process may actually run on
//
// Affinity mask (taskset, --cpuset-cpus) intersected with the cgroup CFS quota
// (--cpus). Never less than 1. Not the machine's CPU count - the process's.
//
extern int corCpuCount(void);

#endif  // CORBASE_CORCPUCOUNT_H_
