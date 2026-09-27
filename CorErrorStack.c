//
// FILE            CorErrorStack.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2020 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <strings.h>                               // bzero
#include <stdlib.h>                                // calloc, realloc
#include <stdio.h>                                 // printf

#include "corBase/CorErrorStack.h"                 // Own interface



// -----------------------------------------------------------------------------
//
// corErrorInit -
//
void corErrorInit(CorErrorStack* esP, int maxErrorItems)
{
  bzero(esP, sizeof(CorErrorStack));
  esP->errorV    = (CorErrorItem*) calloc(maxErrorItems, sizeof(CorErrorItem));
  esP->errorSize = maxErrorItems;
}



// -----------------------------------------------------------------------------
//
// corErrorPushFunction -
//
void corErrorPushFunction
(
  CorErrorStack*  esP,
  const char*   file,
  int           line,
  const char*   function,
  int           code,
  const char*   title,
  const char*   where,
  const char*   detail
)
{
  if (esP->ix >= esP->errorSize)
  {
    // No room - let's add another 5
    int          newSize = esP->errorSize + 5;
    CorErrorItem*  newV    = realloc(esP->errorV, newSize * sizeof(CorErrorItem));

    if (newV == NULL)
      return;  // Can't push error - out of memory

    esP->errorV    = newV;
    esP->errorSize = newSize;
  }

  CorErrorItem* errorP = &esP->errorV[esP->ix];

  errorP->file      = file;
  errorP->line      = line;
  errorP->function  = function;
  errorP->code      = code;
  errorP->title     = title;
  errorP->where     = where;
  errorP->detail    = detail;

  esP->ix++;
}



// -----------------------------------------------------------------------------
//
// corErrorFlushToScreen -
//
void corErrorFlushToScreen(CorErrorStack* esP)
{
  if (esP->ix == 0)  // False Alarm?
    return;

  esP->errorCounter += 1;  // As it starts from 0, it is inceremented before reporting

  int ix;
  for (ix = 0; ix < esP->ix; ix++)
  {
    CorErrorItem* errorP = &esP->errorV[ix];

    printf("Error %d: %04d: %s[%d]:%s: %s|%s|: %s\n",
           esP->errorCounter,
           errorP->code,
           errorP->file,
           errorP->line,
           errorP->function,
           errorP->title,
           errorP->where,
           errorP->detail);
  }

  esP->ix = 0;
}



// -----------------------------------------------------------------------------
//
// corErrorFree -
//
void corErrorFree(CorErrorStack* esP)
{
  if (esP->errorV != NULL)
    free(esP->errorV);
  esP->errorV = NULL;
}
