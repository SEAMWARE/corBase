//
// FILE            itoh.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdio.h>
#include <stdlib.h>

int main(int argC, char* argV[])
{
  int ix;
  
  for (ix = 1; ix < argC; ix++)
    printf("0x%llX ", strtoull(argV[ix], NULL, 10));
  printf("\n");

  return 0;
}
