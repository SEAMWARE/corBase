//
// FILE            htoa.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdio.h>             // printf
#include <stdlib.h>            // exit, strtoul
#include <string.h>            // strcmp



// -----------------------------------------------------------------------------
//
// htoa -
//
void htoa(char* in)
{
  // printf("Incoming string: '%s'\n", in);

  //
  // Strip off '0x/0X', if present
  //
  if ((in[0] == '0') && ((in[1] == 'x') || in[1] == 'X'))
    in = &in[2];
  
  // printf("Starting: '%s'\n", in);
  
  while (*in != 0)
  {
    char h[3];
    int  value;
    int  len;

    h[0] = *in;
    h[1] = 0;
    h[2] = 0;

    ++in;
    len = strlen(in);
    if ((len != 0) && ((len % 2) == 1))  // uneven - take another char
    {
      h[1] = *in;
      ++in;
    }
    value = strtoul(h, NULL, 16);
    printf("%c ", value);
  }
  printf("\n");
}



// -----------------------------------------------------------------------------
//
// main - 
//
int main(int argC, char* argV[])
{
  int ix;

  if ((argC == 1) || ((argC == 2) && (strcmp(argV[1], "-u") == 0)))
  {
    printf("Usage: htoa -u (to see this text)\n");
    printf("       htoa <hex1> <hex2> <hex3> ... <hexN>\n");
    exit(1);
  }
  
  for (ix = 1; ix < argC; ix++)
    htoa(argV[ix]);

  return 0;
}
