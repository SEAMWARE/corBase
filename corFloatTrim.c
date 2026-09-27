//
// FILE            corFloatTrim.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdio.h>                           // snprintf
#include <stdlib.h>                          // strtod
#include <math.h>                            // trunc, fabs

#include <stdbool.h>                         // bool
#include "corBase/corFloatTrim.h"            // Own interface



// -----------------------------------------------------------------------------
//
// shortestRoundTrip - the shortest %g rendering that reads back as exactly 'd'
//
// Used where the fixed-point renderings below cannot hold the value: |d| >= 1e21
// (%.0f would print 22+ digits - and the output buffer is 31 chars, so from 1e30
// on the number was silently CUT, and read back 10x, 100x, ... too small), and
// non-zero values too small for 9 decimals (they printed as 0).
// Exponent notation is valid JSON.
//
static void shortestRoundTrip(char* floatString, double d)
{
  for (int precision = 15; precision < 17; precision++)
  {
    snprintf(floatString, 31, "%.*g", precision, d);

    if (strtod(floatString, NULL) == d)
      return;
  }

  snprintf(floatString, 31, "%.17g", d);  // Always round-trips
}



// -----------------------------------------------------------------------------
//
// corFloatTrim -
//
void corFloatTrim(char* floatString, double d)
{
  //
  // 1. Is there a decimal part?
  //
  double  intPart = trunc(d);
  double  dPart   = fabs(d - intPart);

  if (fabs(intPart) >= 1e21)
  {
    shortestRoundTrip(floatString, d);
    return;
  }

  if (dPart < 0.000000005)
  {
    if ((intPart == 0) && (d != 0))  // Non-zero, but too small for 9 decimals
      shortestRoundTrip(floatString, d);
    else
      snprintf(floatString, 31, "%.0f", intPart);

    return;
  }

  int lastCharIx = snprintf(floatString, 31, "%.9f", d) - 1;

  //
  // 3. Remove trailing zeroes
  //
  while (floatString[lastCharIx] == '0')
  {
    floatString[lastCharIx] = 0;
    --lastCharIx;
  }

  //
  // 4. Remove last char if it's a '.'
  //
  if (floatString[lastCharIx] == '.')
    floatString[lastCharIx] = 0;
}
