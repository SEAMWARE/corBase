//
// FILE            corStringReplace.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2020 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <string.h>                      // strncmp, strlen, strcpy



// -----------------------------------------------------------------------------
//
// corStringReplace -
//
char* corStringReplace(const char* in, const char* replace, const char* with, char* out, int outLen)
{
  int inIx       = 0;
  int outIx      = 0;
  int replaceLen = strlen(replace);
  int withLen    = strlen(with);

  while (in[inIx] != 0)
  {
    if (strncmp(&in[inIx], replace, replaceLen) == 0)
    {
      inIx += replaceLen;
      if (withLen != 0)
      {
        if (outIx + withLen >= outLen)
          break;  // Not enough room - stop here
        memcpy(&out[outIx], with, withLen);
        outIx += withLen;
      }
    }
    else
    {
      if (outIx + 1 >= outLen)
        break;  // Not enough room - stop here
      out[outIx++] = in[inIx++];
    }
  }

  out[outIx] = 0;

  return out;
}
