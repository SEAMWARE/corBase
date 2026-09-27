//
// FILE            kTime.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORTIME_H_
#define CORBASE_CORTIME_H_

#include <time.h>                            // struct timespec



// -----------------------------------------------------------------------------
//
// corTimeGet - get current time 
//
extern int corTimeGet(struct timespec* tP);



// -----------------------------------------------------------------------------
//
// corTimeDiff - calculate the difference between two timespecs
//
extern void corTimeDiff(struct timespec* startP, struct timespec* endP, struct timespec* diffP, float* fP);



// -----------------------------------------------------------------------------
//
// corTimeAccumulate - accumulate timespecs
//
extern void corTimeAccumulate(struct timespec* accumulatedP, struct timespec* partP, float* fP);

#endif  // CORBASE_CORTIME_H_
