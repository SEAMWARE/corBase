//
// FILE            corStringArrayJoin.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2020 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <string.h>                                // strlen, strcpy
#include <stddef.h>                                // NULL

#include "corBase/corStringArrayJoin.h"            // Own interface



// -----------------------------------------------------------------------------
//
// corStringArrayJoin - join a string vector into a single string
//
void corStringArrayJoin(char* output, int outputLen, char** stringV, int vecItems, const char* separator)
{
  int outputIx      = 0;
  int separatorLen  = (separator != NULL)? strlen(separator) : 0;

  output[0] = 0;  // Initialize in case vecItems is 0

  for (int ix = 0; ix < vecItems; ix++)
  {
    int itemLen = strlen(stringV[ix]);

    if (outputIx + itemLen >= outputLen)
      break;  // Not enough room

    memcpy(&output[outputIx], stringV[ix], itemLen);
    outputIx += itemLen;

    // Add 'separator', except for the last round
    if ((ix != vecItems - 1) && (separator != NULL))
    {
      if (outputIx + separatorLen >= outputLen)
        break;  // Not enough room for separator

      memcpy(&output[outputIx], separator, separatorLen);
      outputIx += separatorLen;
    }
  }

  output[outputIx] = 0;
}
