//
// FILE            corStringInArray.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdbool.h>                            // bool
#include <stddef.h>                             // NULL
#include <string.h>                             // strcmp

#include "corBase/corStringInArray.h"           // Own interface



// -----------------------------------------------------------------------------
//
// corStringInArray -
//
bool corStringInArray(const char* s, char** v)
{
  if (s == NULL || v == NULL)
    return false;

  for (int i = 0; v[i] != NULL; i++)
  {
    if (strcmp(s, v[i]) == 0)
      return true;
  }

  return false;
}
