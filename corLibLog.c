//
// FILE            corLibLog.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdio.h>                           // fprintf, vfprintf
#include <stdarg.h>                          // va_list
#include <string.h>                          // strrchr

#include "corBase/corLibLog.h"               // Own interface



// -----------------------------------------------------------------------------
//
// corLibLogFunction -
//
CorLibLogFunction corLibLogFunction = NULL;



// -----------------------------------------------------------------------------
//
// corLibLogFallback -
//
void corLibLogFallback
(
  const char*  fileName,
  int          lineNo,
  const char*  functionName,
  char         type,
  int          aux,
  const char*  format,
  ...
)
{
  (void) aux;

  if ((type != 'E') && (type != 'W') && (type != 'X'))
    return;

  const char* file = strrchr(fileName, '/');
  file = (file == NULL)? fileName : &file[1];

  va_list ap;

  fprintf(stderr, "%c: %s[%d]: %s: ", type, file, lineNo, functionName);
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
  fprintf(stderr, "\n");
}
