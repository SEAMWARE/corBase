//
// FILE            corCharCount.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include "corBase/corCharCount.h"            // Own interface



// -----------------------------------------------------------------------------
//
// corCharCount - count number of occurences of 'needle' in 'haystack'
//
// NOTE
//   haystack is not checked for NULL, if it is NULL, this function crashes your program,
//   This is how the C std library works and it is good, for performance issues.
//
int corCharCount(char* haystack, char needle)
{
  int hits = 0;

  while (*haystack != 0)
  {
    if (*haystack == needle)
      ++hits;

    ++haystack;
  }

  return hits;
}
