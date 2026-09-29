//
// FILE            corBaseInit.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORBASEINIT_H_
#define CORBASE_CORBASEINIT_H_

#include "corBase/corLibLog.h"               // CorLibLogFunction, corBaseTraceLevelsSet



// -----------------------------------------------------------------------------
//
// corBaseInit - give the libraries the executable's log function
//
// Every library of the stack logs through it (COR_LIB_E, ...). Call it once the
// executable's log is up; until then a library line takes corLibLogFallback.
//
extern void corBaseInit(CorLibLogFunction logFunction);

#endif  // CORBASE_CORBASEINIT_H_
