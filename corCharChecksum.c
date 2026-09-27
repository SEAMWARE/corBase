//
// FILE            corCharChecksum.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include "corBase/corCharChecksum.h"         // Own interface



// -----------------------------------------------------------------------------
//
// corCharChecksum - simple checksum of 'char' of a sized buffer
//
char corCharChecksum(void* buf, int bufLen)
{
  char* bufP = (char*) buf;
  char  cs   = 0;
  int   ix;

  for (ix = 0; ix < bufLen; ix++)
    cs += bufP[ix];

  return cs;
}
