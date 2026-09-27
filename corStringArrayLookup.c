//
// FILE            corStringArrayLookup.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2020 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <string.h>                                // strlen, strcpy
#include <stddef.h>                                // NULL

#include "corBase/corStringArrayLookup.h"          // Own interface



// -----------------------------------------------------------------------------
//
// corStringArrayLookup - lookup a string in a string vector
//
int corStringArrayLookup(char** stringV, int vecItems, const char* needle)
{
  int ix;
  for (ix = 0; ix < vecItems; ix++)
  {
    if (strcmp(stringV[ix], needle) == 0)
      return ix;
  }

  return -1;
}
