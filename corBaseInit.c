//
// FILE            corBaseInit.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include "corBase/corLibLog.h"               // corLibLogFunction
#include "corBase/corBaseInit.h"             // Own interface



// -----------------------------------------------------------------------------
//
// corBaseInit -
//
void corBaseInit(CorLibLogFunction logFunction)
{
  corLibLogFunction = logFunction;
}



// -----------------------------------------------------------------------------
//
// corBaseTraceLevelsSet - let COR_LIB_T test the executable's trace levels inline
//
// levelV is the executable's own bitmask (bit N of word N/32 = trace level N on), read in
// place, so a level set or reset at runtime is seen at once.
//
void corBaseTraceLevelsSet(const unsigned int* levelV, unsigned int words)
{
  corLibTraceLevelWords = words;
  corLibTraceLevels     = levelV;
}
