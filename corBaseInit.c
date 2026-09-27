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
