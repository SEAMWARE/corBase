//
// FILE            corMemoryLimit.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#include <stdbool.h>                                  // true
#include <stdint.h>                                   // uint64_t
#include <stdio.h>                                    // snprintf
#include <stdlib.h>                                   // strtoull
#include <string.h>                                   // strncmp, strchr, strrchr, strlen

#include "corBase/corFileReadInto.h"                  // corFileReadInto
#include "corBase/corMemoryLimit.h"                   // Own interface



// -----------------------------------------------------------------------------
//
// corMemoryLimit -
//
uint64_t corMemoryLimit(void)
{
  char     buf[512];
  char     path[1024];
  uint64_t limit = 0;

  if ((corFileReadInto("/proc/self/cgroup", buf, sizeof(buf)) > 0) && (strncmp(buf, "0::", 3) == 0))
  {
    char* nl = strchr(buf, '\n');
    if (nl != NULL)
      *nl = 0;

    char* cg = &buf[3];                               // "/user.slice/..." or "/"

    while (true)
    {
      char val[64];

      snprintf(path, sizeof(path), "/sys/fs/cgroup%s%smemory.max", cg, (cg[strlen(cg) - 1] == '/') ? "" : "/");

      if ((corFileReadInto(path, val, sizeof(val)) > 0) && (strncmp(val, "max", 3) != 0))
      {
        uint64_t v = strtoull(val, NULL, 10);
        if ((v > 0) && ((limit == 0) || (v < limit)))
          limit = v;
      }

      char* slash = strrchr(cg, '/');
      if ((slash == NULL) || (slash == cg))
      {
        if (cg[1] == 0)
          break;                                      // "/" was the last one
        cg[1] = 0;                                    // up to the root
        continue;
      }
      *slash = 0;
    }

    return limit;
  }

  if (corFileReadInto("/sys/fs/cgroup/memory/memory.limit_in_bytes", buf, sizeof(buf)) > 0)
  {
    uint64_t v = strtoull(buf, NULL, 10);
    return (v >= (1ULL << 60)) ? 0 : v;
  }

  return 0;
}
