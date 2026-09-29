//
// FILE            corLibLog.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORLIBLOG_H_
#define CORBASE_CORLIBLOG_H_

#include <stdlib.h>                          // exit



// -----------------------------------------------------------------------------
//
// CorLibLogFunction - how a library reaches the log of the executable it runs in
//
// A library cannot know where its executable logs - a file, which one, in what
// format - so it does not log at all: it hands every line to the executable's
// log function, set by corBaseInit(). The signature is corLog's corLogOut, so an
// executable that logs with corLog passes corLogOut itself.
//
// type:  'E' error, 'W' warning, 'I' info, 'V' verbose, 'T' trace, 'X' exit
// aux:   the trace level for 'T', the exit code for 'X', -1 otherwise
//
typedef void (*CorLibLogFunction)
(
  const char*  fileName,
  int          lineNo,
  const char*  functionName,
  char         type,
  int          aux,
  const char*  format,
  ...
) __attribute__((format(printf, 6, 7)));



// -----------------------------------------------------------------------------
//
// corLibLogFunction - the executable's log function, NULL until corBaseInit()
//
extern CorLibLogFunction corLibLogFunction;



// -----------------------------------------------------------------------------
//
// corLibLogFallback - what a library line becomes before corBaseInit()
//
// Errors, warnings and exits go to stderr - option parsing, for one, runs before
// any log file exists, and its errors must still be seen. Everything else is
// dropped: with no owner there is nobody to have turned it on.
//
extern void corLibLogFallback
(
  const char*  fileName,
  int          lineNo,
  const char*  functionName,
  char         type,
  int          aux,
  const char*  format,
  ...
) __attribute__((format(printf, 6, 7)));



// -----------------------------------------------------------------------------
//
// corLibTraceLevels - the executable's trace-level bitmask, NULL unless corBaseTraceLevelsSet()
//
// So that a trace which is off is decided INLINE: without it every COR_LIB_T is a call through
// corLibLogFunction - a variadic one, arguments evaluated - only for the owner to answer "not
// this level". In per-node code that is a measurable share of all the work done. NULL keeps the
// old way: the log function decides.
//
extern const unsigned int* corLibTraceLevels;
extern unsigned int        corLibTraceLevelWords;

extern void corBaseTraceLevelsSet(const unsigned int* levelV, unsigned int words);

static inline int corLibTraceOn(unsigned int level)
{
  if (corLibTraceLevels == NULL)
    return 1;

  unsigned int index = level / 32;

  if (index >= corLibTraceLevelWords)
    return 0;

  return (corLibTraceLevels[index] & (1U << (level % 32))) != 0;
}



// -----------------------------------------------------------------------------
//
// COR_LIB_LOG - collect file, line and function, and hand the line to the owner
//
#define COR_LIB_LOG(type, aux, ...)                                                          \
do                                                                                           \
{                                                                                            \
  if (corLibLogFunction != NULL)                                                             \
    corLibLogFunction(__FILE__, __LINE__, __FUNCTION__, type, aux, __VA_ARGS__);             \
  else                                                                                       \
    corLibLogFallback(__FILE__, __LINE__, __FUNCTION__, type, aux, __VA_ARGS__);             \
} while (0)



// -----------------------------------------------------------------------------
//
// COR_LIB_E, ... - the library's log macros (corLog's COR_E, ... for a library)
//
#define COR_LIB_E(...)           COR_LIB_LOG('E', -1,     __VA_ARGS__)
#define COR_LIB_W(...)           COR_LIB_LOG('W', -1,     __VA_ARGS__)
#define COR_LIB_I(...)           COR_LIB_LOG('I', -1,     __VA_ARGS__)
#define COR_LIB_V(...)           COR_LIB_LOG('V', -1,     __VA_ARGS__)
//
// COR_LIB_T exists in a build that defines COR_T_ON (a debug build) and nowhere else - as corLog's
// COR_T. Without it the trace is compiled away, its arguments still type-checked and used.
//
#ifdef COR_T_ON
#define COR_LIB_T(tLevel, ...)   do { if (corLibTraceOn(tLevel)) COR_LIB_LOG('T', tLevel, __VA_ARGS__); } while (0)
#else
#define COR_LIB_T(tLevel, ...)   do { if (0) COR_LIB_LOG('T', tLevel, __VA_ARGS__); } while (0)
#endif
#define COR_LIB_X(eCode, ...)    do { COR_LIB_LOG('X', eCode, __VA_ARGS__); exit(eCode); } while (0)
#define COR_LIB_RE(retVal, ...)  do { COR_LIB_LOG('E', -1,    __VA_ARGS__); return retVal; } while (0)
#define COR_LIB_RVE(...)         do { COR_LIB_LOG('E', -1,    __VA_ARGS__); return;        } while (0)

#endif  // CORBASE_CORLIBLOG_H_
