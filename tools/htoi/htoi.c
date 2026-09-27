//
// FILE            htoi.c
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
// htoi -
//
void htoi(char* in, int bytes)
{
  //
  // Strip off '0x/0X', if present
  //
  if ((in[0] == '0') && ((in[1] == 'x') || in[1] == 'X'))
    in = &in[2];

  if (bytes == 8)
  {
    unsigned long long ll;

    ll = strtoul(in, NULL, 16);

    printf("%llu\n", ll);
  }
  else if (bytes == 4)
  {
    unsigned int i;

    i = strtoul(in, NULL, 16);

    printf("%u\n", i);
  }
  else if (bytes == 2)
  {
    unsigned short s;

    s = strtoul(in, NULL, 16);

    printf("%d\n", s);
  }
  else if (bytes == 1)
  {
    unsigned char c;

    c = strtoul(in, NULL, 16);

    printf("%d\n", c);
  }
}


// -----------------------------------------------------------------------------
//
// usage -
//
void usage(void)
{
  printf("Usage: htoa -u (to see this text)\n");
  printf("       htoa <--bytes|-b (1|2|4|8)> <hex1> <hex2> <hex3> ... <hexN>\n");
}



// -----------------------------------------------------------------------------
//
// main - 
//
int main(int argC, char* argV[])
{
  int ix    = 1;
  int bytes = 8;

  if ((argC == 1) || ((argC == 2) && (strcmp(argV[1], "-u") == 0)))
  {
    usage();
    exit(1);
  }
  
  if ((argC >= 4) && ((strcmp(argV[1], "--bytes") == 0) || (strcmp(argV[1], "-b") == 0)))
  {
    bytes = atoi(argV[2]);

    if ((bytes != 1) && (bytes != 2) && (bytes != 4) && (bytes != 8))
    {
      printf("htoa: invalid number of bytes. Allowed: 1|2|4|8\n");
      usage();
      exit(2);
    }

    ix = 3;
  }

  for (; ix < argC; ix++)
    htoi(argV[ix], bytes);

  return 0;
}
