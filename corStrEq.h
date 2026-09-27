#ifndef CORBASE_CORSRTEQ_H_
#define CORBASE_CORSRTEQ_H_

// 
// FILE            corStrEq.h
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
// corStrEq -
//
static inline bool corStrEq(const char* s1, const char* s2)
{
  while ((*s1 != 0) && (*s2 != 0))
  {
    if (*s1 != *s2)
      return false;
    ++s1;
    ++s2;
  }

  if (*s1 != *s2)
    return false;

  return true;
}

#endif  // CORBASE_CORSRTEQ_H_
