//
// FILE            corProgName.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <string.h>                          // strlen

#include "corBase/corProgName.h"             // Own interface



// -----------------------------------------------------------------------------
//
// corProgName - return program name, without any directories in the string
//
char* corProgName(char* argv0)
{
  //
  // 00. Validity checks
  //
  if (argv0 == NULL)
    return "NO PROGNAME";

  if (argv0[0] == 0)
    return "EMPTY PROGNAME";

  //
  // 01. Position cP at the last char of the string argv0
  //
  char* cP = &argv0[strlen(argv0) - 1];


  //
  // 02. Search 'backwards' until '/' is found (or not)
  //
  while (cP >= argv0)
  {
    if (*cP == '/')
      return &cP[1];

    --cP;
  }


  //
  // 03. No '/' found - just return the string that was input to the function
  //
  return argv0;
}
