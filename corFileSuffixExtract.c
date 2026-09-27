//
// FILE            corFileSuffixExtract.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2020 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <string.h>                           // strlen, strdup

#include <stdbool.h>                          // bool



// -----------------------------------------------------------------------------
//
// corFileSuffixExtract -
//
char* corFileSuffixExtract(char* fileName, int len, bool destructive)
{
  char* endP;

  if (len == -1)
    len = strlen(fileName);
  endP = &fileName[len - 1];

  while (*endP != '.')
  {
    --endP;
    if (endP <= fileName)
      return NULL;
  }

  if (destructive == false)   // Not destructive - can't NULL out the dor at *endP . must allocate
    return strdup(&endP[1]);

  *endP = 0;
  return &endP[1];
}
