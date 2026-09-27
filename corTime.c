//
// FILE            kTime.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <time.h>                            // struct timespec

#include "corBase/corTime.h"                 // Own interface



// -----------------------------------------------------------------------------
//
// corTimeGet - get current time 
//
int corTimeGet(struct timespec* tP)
{
  return clock_gettime(CLOCK_REALTIME, tP);
}



// -----------------------------------------------------------------------------
//
// corTimeDiff - calculate the difference between two timespecs
//
void corTimeDiff(struct timespec* startP, struct timespec* endP, struct timespec* diffP, float* fP)
{
  diffP->tv_sec  = endP->tv_sec  - startP->tv_sec;
  diffP->tv_nsec = endP->tv_nsec - startP->tv_nsec;

  if (diffP->tv_nsec < 0)
  {
    diffP->tv_sec  -= 1;
    diffP->tv_nsec += 1000000000;
  }
  
  if (fP != NULL)
    *fP = diffP->tv_sec + ((float) diffP->tv_nsec) / 1000000000;
}



// -----------------------------------------------------------------------------
//
// corTimeAccumulate - accumulate timespecs
//
void corTimeAccumulate(struct timespec* accumulatedP, struct timespec* partP, float* fP)
{
  accumulatedP->tv_sec  += partP->tv_sec;
  accumulatedP->tv_nsec += partP->tv_nsec;

  while (accumulatedP->tv_nsec >= 1000000000)
  {
    accumulatedP->tv_sec  += 1;
    accumulatedP->tv_nsec -= 1000000000;
  }

  if (fP != NULL)
    *fP = accumulatedP->tv_sec + ((float) accumulatedP->tv_nsec) / 1000000000;
}
