//
// FILE            corTimeIso.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#include <stdbool.h>                                  // bool
#include <stdint.h>                                   // int64_t

#include "corBase/corTimeIso.h"                       // Own interface



// -----------------------------------------------------------------------------
//
// digits - n digits of v, zero-padded, at p
//
static char* digits(char* p, int64_t v, int n)
{
  for (int i = n - 1; i >= 0; i--)
  {
    p[i] = '0' + (char) (v % 10);
    v   /= 10;
  }

  return p + n;
}



// -----------------------------------------------------------------------------
//
// corTimeIso -
//
// Days -> civil date: Howard Hinnant's civil_from_days, valid for the whole proleptic Gregorian range.
//
int corTimeIso(int64_t nsec, int fracDigits, bool trim, char* buf)
{
  int64_t sec  = nsec / 1000000000;
  int64_t frac = nsec % 1000000000;

  if (frac < 0)                                      // floor, for times before 1970
  {
    frac += 1000000000;
    sec  -= 1;
  }

  int64_t days = sec / 86400;
  int64_t sod  = sec % 86400;

  if (sod < 0)
  {
    sod  += 86400;
    days -= 1;
  }

  int64_t z   = days + 719468;
  int64_t era = ((z >= 0) ? z : z - 146096) / 146097;
  int64_t doe = z - era * 146097;
  int64_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  int64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  int64_t mp  = (5 * doy + 2) / 153;
  int64_t d   = doy - (153 * mp + 2) / 5 + 1;
  int64_t m   = (mp < 10) ? mp + 3 : mp - 9;
  int64_t y   = yoe + era * 400 + ((m <= 2) ? 1 : 0);

  char* p = buf;

  p = digits(p, y, 4);   *p++ = '-';
  p = digits(p, m, 2);   *p++ = '-';
  p = digits(p, d, 2);   *p++ = 'T';
  p = digits(p, sod / 3600, 2);         *p++ = ':';
  p = digits(p, (sod / 60) % 60, 2);    *p++ = ':';
  p = digits(p, sod % 60, 2);

  if (fracDigits > 9)
    fracDigits = 9;

  if (fracDigits > 0)
  {
    int64_t f = frac;

    for (int i = fracDigits; i < 9; i++)
      f /= 10;

    if (f != 0)
    {
      int n = fracDigits;

      if (trim == true)
      {
        while ((n > 0) && ((f % 10) == 0))
        {
          f /= 10;
          --n;
        }
      }

      *p++ = '.';
      p    = digits(p, f, n);
    }
  }

  *p++ = 'Z';
  *p   = 0;

  return (int) (p - buf);
}
