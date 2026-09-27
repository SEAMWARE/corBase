#ifndef CORBASE_CORFILESUFFIXEXTRACT_H_
#define CORBASE_CORFILESUFFIXEXTRACT_H_

// 
// FILE            corFileSuffixExtract.h
// 
// AUTHOR          Ken Zangelin
// 
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdbool.h>                    // bool



// -----------------------------------------------------------------------------
//
// corFileSuffixExtract -
//
extern char* corFileSuffixExtract(char* fileName, int len, bool destructive);

#endif  // CORBASE_CORFILESUFFIXEXTRACT_H_
