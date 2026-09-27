//
// FILE            kMacros.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORMACROS_H_
#define CORBASE_CORMACROS_H_

#include <stdbool.h>                    // bool


// -----------------------------------------------------------------------------
//
// COR_VEC_SIZE - the size of an array, in items
//
#define COR_VEC_SIZE(a)  (sizeof(a) / sizeof(a[0]))



// -----------------------------------------------------------------------------
//
// COR_MAX - max value
//
#define COR_MAX(a, b)  ((a) > (b)? (a) : (b))



// -----------------------------------------------------------------------------
//
// COR_MIN - min value
//
#define COR_MIN(a, b)  ((a) < (b)? (a) : (b))



// -----------------------------------------------------------------------------
//
// COR_FT - a bool as "true"/"false"
//
#define COR_FT(b)  ((b)? "true" : "false")



// -----------------------------------------------------------------------------
//
// COR_SET - a bool as "set"/"unset"
//
#define COR_SET(b)  ((b)? "set" : "unset")

#endif  // CORBASE_CORMACROS_H_
