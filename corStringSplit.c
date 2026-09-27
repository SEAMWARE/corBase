//
// FILE            corStringSplit.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stddef.h>                                // NULL

#include "corBase/corStringSplit.h"                // Own interface



// -----------------------------------------------------------------------------
//
// corStringSplit - split a string into a vector
//
// NOTE
//   The initial string (char* s) is destroyed, as each occurrence of the DELIMITER (char d)
//   is replaced by a ZERO and an item in the string vector (char** sVec) points
//   inside the initial string (char* s).
//
//   This is very efficient as no allocation is needed, however, it has two (major) drawbacks:
//     1. The initial string must be writable.
//     2. The initial string must be copied before calling corStringSplit if it needs to be maintained intact.
//
int corStringSplit(char* s, char d, char** sVec, int sVecLen)
{
  int sIx = 0;

  // No/Empty string?
  if ((s == NULL)  || (*s == 0))
    return 0;

  sVec[sIx++] = s;

  while (*s != 0)
  {
    if (*s == d)
    {
      *s = 0;
      ++s;

      if (sIx < sVecLen)
        sVec[sIx] = s;
      else
        return sIx;  // FIXME: Silently stop or return an error?

      ++sIx;
    }

    ++s;
  }

  return sIx;
}
