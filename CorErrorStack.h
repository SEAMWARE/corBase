#ifndef CORBASE_CORERRORSTACK_H_
#define CORBASE_CORERRORSTACK_H_

// 
// FILE            CorErrorStack.h
// 
// AUTHOR          Ken Zangelin
// 
// Copyright 2020 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//



// -----------------------------------------------------------------------------
//
// CorErrorItem -
//
typedef struct CorErrorItem
{
  const char*  file;
  int          line;
  const char*  function;
  int          code;
  const char*  title;
  const char*  where;
  const char*  detail;
} CorErrorItem;



// -----------------------------------------------------------------------------
//
// CorErrorStack -
//
typedef struct CorErrorStack
{
  CorErrorItem*  errorV;
  int          errorSize;
  int          ix;                // Current index in the Error Stack (errorV)
  int          errorCounter;      // Accumulated number of errors (number of error stacks produced)
} CorErrorStack;



// -----------------------------------------------------------------------------
//
// corErrorPush -
//
#define corErrorPush(errorStackP, code, title, where, detail)                                      \
do                                                                                               \
{                                                                                                \
  corErrorPushFunction(errorStackP, __FILE__, __LINE__, __FUNCTION__, code, title, where, detail); \
} while (0)
  


// -----------------------------------------------------------------------------
//
// corErrorInit -
//
extern void corErrorInit(CorErrorStack* esP, int maxErrorItems);



// -----------------------------------------------------------------------------
//
// corErrorPushFunction -
//
extern void corErrorPushFunction
(
  CorErrorStack*  esP,
  const char*   file,
  int           line,
  const char*   function,
  int           code,
  const char*   title,
  const char*   where,
  const char*   detail
);



// -----------------------------------------------------------------------------
//
// corErrorFlushToScreen -
//
extern void corErrorFlushToScreen(CorErrorStack* esP);



// -----------------------------------------------------------------------------
//
// corErrorFree -
//
extern void corErrorFree(CorErrorStack* esP);

#endif  // CORBASE_CORERRORSTACK_H_
