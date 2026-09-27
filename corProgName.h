//
// FILE            corProgName.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORPROGNAME_H_
#define CORBASE_CORPROGNAME_H_



// -----------------------------------------------------------------------------
//
// corProgName - return program name, without any directories in the string
//
// NOTE
//   This function only returns a pointerto where the 'clean' program name starts.
//   The function DOES NOT allocate any memory for it.
//   Allocation would be responsibility of the caller, if needed.
//   Why?
//   Well, the normal usage of this function is to send in argV[0], right in the beginning
//   of the 'main' function. This string leves on the stack as long as the program is alive,
//   so, no allocation is needed, at least not in the intended use of this function.
//   If used in other circumstances, what yhis function returns might be a good candidate
//   for strdup ...
//
extern char* corProgName(char* argv0);

#endif  // CORBASE_CORPROGNAME_H_
