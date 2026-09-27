//
// FILE            corCpuCount.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#define _GNU_SOURCE                                   // CPU_COUNT, sched_getaffinity

#include <sched.h>                                    // sched_getaffinity, cpu_set_t, CPU_COUNT
#include <stdio.h>                                    // fopen, fscanf, fclose
#include <unistd.h>                                   // sysconf

#include "corBase/corCpuCount.h"          // Own interface



// -----------------------------------------------------------------------------
//
// cgroupQuota - CPUs this process may use per period, or 0 if there is no quota
//
// The case sched_getaffinity cannot see. `docker --cpuset-cpus 0-1` restricts
// the AFFINITY MASK and is caught below; `docker --cpus 2` sets a CFS QUOTA and
// leaves the mask showing every CPU on the host. Both are ordinary ways to
// limit a container, and the second is the more common one - so a broker that
// sized itself off affinity alone would start a loop per host core inside a
// two-core container.
//
// cgroup v2 puts "<quota> <period>" in cpu.max, with quota "max" for no limit.
// v1 splits it across two files and uses -1 for no limit. Rounded UP: half a
// core of quota is still a core the broker may run on.
//
static int cgroupQuota(void)
{
  FILE* fP;
  long  quota  = -1;
  long  period = 0;

  if ((fP = fopen("/sys/fs/cgroup/cpu.max", "r")) != NULL)   // v2
  {
    char q[64];

    if (fscanf(fP, "%63s %ld", q, &period) == 2)
    {
      if (q[0] != 'm')                                       // "max" == no limit
        sscanf(q, "%ld", &quota);
    }

    fclose(fP);
  }
  else if ((fP = fopen("/sys/fs/cgroup/cpu/cpu.cfs_quota_us", "r")) != NULL)   // v1
  {
    if (fscanf(fP, "%ld", &quota) != 1)
      quota = -1;

    fclose(fP);

    if ((fP = fopen("/sys/fs/cgroup/cpu/cpu.cfs_period_us", "r")) != NULL)
    {
      if (fscanf(fP, "%ld", &period) != 1)
        period = 0;

      fclose(fP);
    }
  }

  if ((quota <= 0) || (period <= 0))
    return 0;

  return (int) ((quota + period - 1) / period);              // round up
}



// -----------------------------------------------------------------------------
//
// corCpuCount - how many CPUs this process may actually run on
//
// NOT sysconf(_SC_NPROCESSORS_ONLN), which answers "how many CPUs does this
// machine have" - a different question, and the wrong one anywhere the process
// is confined. A container limited to two cores on a 64-core host would size
// itself for 64.
//
// sched_getaffinity is also what `taskset` sets, so a pinned broker - which is
// how every measurement here runs it - sizes itself to the pin without being
// told.
//
// Never returns less than 1: a broker that concluded it had no CPUs would
// configure itself into doing nothing.
//
int corCpuCount(void)
{
  int       cpus = 0;
  cpu_set_t set;

  if (sched_getaffinity(0, sizeof(set), &set) == 0)
    cpus = CPU_COUNT(&set);

  if (cpus <= 0)
  {
    long online = sysconf(_SC_NPROCESSORS_ONLN);              // last resort

    cpus = (online > 0) ? (int) online : 1;
  }

  int quota = cgroupQuota();

  if ((quota > 0) && (quota < cpus))
    cpus = quota;

  return (cpus > 0) ? cpus : 1;
}
