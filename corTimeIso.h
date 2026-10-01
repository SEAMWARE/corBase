#ifndef CORBASE_CORTIMEISO_H_
#define CORBASE_CORTIMEISO_H_

//
// FILE            corTimeIso.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#include <stdbool.h>                                  // bool
#include <stdint.h>                                   // int64_t



// -----------------------------------------------------------------------------
//
// corTimeIso - epoch nanoseconds as ISO 8601 UTC: YYYY-MM-DDTHH:MM:SS[.f...]Z
//
// fracDigits  how many fraction digits to keep, 0-9 (the rest truncated, not rounded)
// trim        drop trailing zeros of the fraction
//
// A fraction that is zero (at fracDigits) is left out altogether, '.' included.
// buf must hold 31 bytes. Returns the length.
//
// No gmtime_r, strftime or printf: the date comes straight from the day count. A broker renders
// createdAt/modifiedAt for every entity it answers with, and the libc route was 4% of a GET.
//
extern int corTimeIso(int64_t nsec, int fracDigits, bool trim, char* buf);

#endif  // CORBASE_CORTIMEISO_H_
