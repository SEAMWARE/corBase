//
// FILE            corStringInArray.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORSTRINGINARRAY_H_
#define CORBASE_CORSTRINGINARRAY_H_

#include <stdbool.h>                            // bool


// -----------------------------------------------------------------------------
//
// corStringInArray - true if the NULL-terminated array `v` contains `s`
//
// Sister to corStringArrayLookup, but for size-less NULL-terminated string
// arrays (the common shape produced by parsers that build an array and
// terminate it with a NULL slot rather than tracking a count). Returns
// false on any NULL input.
//
extern bool corStringInArray(const char* s, char** v);

#endif  // CORBASE_CORSTRINGINARRAY_H_
