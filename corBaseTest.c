//
// FILE            corBaseTest.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdio.h>                           // printf
#include <stdarg.h>                          // va_list
#include <string.h>                          // strcmp

#include "corBase/corBaseInit.h"             // corBaseInit
#include "corBase/corLibLog.h"               // COR_LIB_*
#include "corBase/corMacros.h"               // COR_FT, COR_VEC_SIZE
#include "corBase/corStringSplit.h"          // corStringSplit
#include "corBase/version.h"                 // CORBASE_VERSION



// -----------------------------------------------------------------------------
//
// ownerLog - what an executable passes to corBaseInit (corLogOut's signature)
//
static char ownerType;
static int  ownerAux;
static char ownerMsg[256];

static void ownerLog(const char* fileName, int lineNo, const char* functionName, char type, int aux, const char* format, ...)
{
  va_list ap;

  (void) fileName;
  (void) lineNo;
  (void) functionName;

  ownerType = type;
  ownerAux  = aux;

  va_start(ap, format);
  vsnprintf(ownerMsg, sizeof(ownerMsg), format, ap);
  va_end(ap);
}



// -----------------------------------------------------------------------------
//
// check -
//
static int failures = 0;

static void check(bool ok, const char* what)
{
  printf("%s: %s\n", ok? "ok  " : "FAIL", what);
  if (!ok)
    ++failures;
}



// -----------------------------------------------------------------------------
//
// logReturns - a function that returns through COR_LIB_RE
//
static int logReturns(int n)
{
  COR_LIB_RE(n * 2, "returning %d", n * 2);
}



// -----------------------------------------------------------------------------
//
// main -
//
int main(void)
{
  printf("corBase %s\n", CORBASE_VERSION);

  // Before corBaseInit: the fallback - an error reaches stderr, a trace is dropped
  COR_LIB_E("fallback error line - expected on stderr");
  COR_LIB_T(300, "fallback trace line - must NOT be seen");
  check(ownerType == 0, "no owner before corBaseInit");

  corBaseInit(ownerLog);

  COR_LIB_T(301, "trace %s", "line");
  check(ownerType == 'T' && ownerAux == 301 && strcmp(ownerMsg, "trace line") == 0, "COR_LIB_T reaches the owner, with its trace level");

  COR_LIB_W("warning %d", 7);
  check(ownerType == 'W' && ownerAux == -1 && strcmp(ownerMsg, "warning 7") == 0, "COR_LIB_W reaches the owner");

  check(logReturns(21) == 42 && ownerType == 'E' && strcmp(ownerMsg, "returning 42") == 0, "COR_LIB_RE logs an error and returns");

  char  in[] = "name,type,value";
  char* outV[4];
  int   items = corStringSplit(in, ',', outV, COR_VEC_SIZE(outV));
  check(items == 3 && strcmp(outV[2], "value") == 0, "corStringSplit");

  check(strcmp(COR_FT(true), "true") == 0 && strcmp(COR_FT(false), "false") == 0, "COR_FT");

  return (failures == 0)? 0 : 1;
}
