//
// FILE            atoh.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdlib.h>            // exit, strtoul
#include <string.h>            // strcmp
#include <stdio.h>             // printf
#include <ctype.h>             // isprint



// -----------------------------------------------------------------------------
//
// main - 
//
int main(int argC, char* argV[])
{
  if ((argC == 1) || ((argC == 2) && (strcmp(argV[1], "-u") == 0)))
  {
    printf("Usage: atoh -u (to see this text)\n");
    printf("       atoh -a (to see a list of all printable chars)\n");
    printf("       atoh <char1> <char2> <char3> ... <charN>\n");
    exit(1);
  }
  
  if ((argC == 2) && (strcmp(argV[1], "-a") == 0))
  {
    int ix;

    for (ix = 1; ix <= 0xFF; ix++)
    {
      if (isprint(ix))
        printf("%c: 0x%x\n", ix, ix);
    }
  }
  else
  {
    int ix;

    for (ix = 1; ix < argC; ix++)
    {
      int len = strlen(argV[ix]);
      int jx;

      for (jx = 0; jx < len; ++jx)
        printf("%c: 0x%x\n", argV[ix][jx], argV[ix][jx]);
    }
  }

  return 0;
}
