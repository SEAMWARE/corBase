#ifndef CORBASE_CORSIZETEXT_H_
#define CORBASE_CORSIZETEXT_H_

//
// FILE            corSizeText.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#include <stdint.h>                                   // uint64_t



// -----------------------------------------------------------------------------
//
// corSizeText - a byte count as text, in the largest unit it has at least one of, up to MiB: "4096 MiB",
// "64 KiB", "512 B"
//
// Whole units, rounded down - 1.5 MiB is "1 MiB" - so the MiB text is what `bytes >> 20` printed before;
// below 1 MiB that printed "0 MiB" for a size that is not zero (a 64 KiB budget). No GiB: rounded down,
// 1.5 GiB would read 1 GiB, a third of it gone. The text goes into buf (32 bytes hold any uint64_t),
// which is returned.
//
extern char* corSizeText(uint64_t bytes, char* buf, int bufSize);

#endif  // CORBASE_CORSIZETEXT_H_
