//
// FILE            corSizeText.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#include <stdint.h>                                   // uint64_t
#include <stdio.h>                                    // snprintf

#include "corBase/corSizeText.h"                      // Own interface



// -----------------------------------------------------------------------------
//
// corSizeText -
//
char* corSizeText(uint64_t bytes, char* buf, int bufSize)
{
  if (bytes >= (1ULL << 20))
    snprintf(buf, bufSize, "%llu MiB", (unsigned long long) (bytes >> 20));
  else if (bytes >= (1ULL << 10))
    snprintf(buf, bufSize, "%llu KiB", (unsigned long long) (bytes >> 10));
  else
    snprintf(buf, bufSize, "%llu B", (unsigned long long) bytes);

  return buf;
}
