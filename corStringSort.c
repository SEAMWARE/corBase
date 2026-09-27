//
// FILE            corStringSort.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stddef.h>                                // NULL

#include "corBase/corStringSort.h"                 // Own interface



// -----------------------------------------------------------------------------
//
// corStringSort - sort a string, char by char
//
// Resulting string (the value of 's' is overwritten by this function) is sorted
// in increasing order
//
void corStringSort(char* s)
{
  char* current = s;
  char* tmp;
  char* min;

  while (*current != 0)
  {
    tmp = current;
    min = current;

    while (*tmp != 0)
    {
      if (*tmp < *min)
        min = tmp;
      ++tmp;
    }

    if (min != current)
    {
      char tmpChar = *current;

      *current = *min;
      *min     = tmpChar;
    }
    ++current;
  }
}
