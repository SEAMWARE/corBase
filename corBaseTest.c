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
#include <stdint.h>                          // uint64_t
#include <stdlib.h>                          // exit
#include <signal.h>                          // SIGSEGV
#include <time.h>                            // clock_gettime
#include <unistd.h>                          // fork, _exit
#include <sys/wait.h>                        // waitpid
#include <sys/epoll.h>                       // epoll_create1, epoll_wait
#include <poll.h>                            // POLLIN

#include "corBase/corBaseInit.h"             // corBaseInit
#include "corBase/corCo.h"                   // corCoCreate, corCoResume, corCoYield, ...
#include "corBase/corCoLoop.h"               // corCoLoopInit, corCoLoopWait, ...
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
// Coroutines
//
static int coSteps;

static void coCounter(void* arg)
{
  int* nP = (int*) arg;

  for (int i = 0; i < *nP; i++)
  {
    coSteps += 1;
    corCoYield();
  }
}

static double coFloat(double x)                // floating point both sides of a switch
{
  double r = x * 1.5;
  corCoYield();
  return r + x / 3.0;
}

static void coFloatFn(void* arg)
{
  double* dP = (double*) arg;
  *dP = coFloat(*dP);
}

static CorCo* coInnerP;
static int    coOrder[8];
static int    coOrderN;

static void coInner(void* arg)
{
  (void) arg;
  coOrder[coOrderN++] = 2;
  corCoYield();                                // back to the OUTER coroutine, not to main
  coOrder[coOrderN++] = 4;
}

static void coOuter(void* arg)
{
  (void) arg;
  coOrder[coOrderN++] = 1;
  coInnerP = corCoCreate(coInner, NULL);
  corCoResume(coInnerP);
  coOrder[coOrderN++] = 3;
  corCoResume(coInnerP);                       // inner finishes
  coOrder[coOrderN++] = 5;
}

__attribute__((noinline)) static int coDeep(int n)   // ~1 KiB a frame, kept: pad is read after the call
{
  volatile char pad[1000];
  pad[0] = (char) n;
  if (n == 0)
    return 0;
  int r = coDeep(n - 1);
  return r + 1 + (pad[0] - (char) n);
}

static void coDeepFn(void* arg)
{
  *(int*) arg = coDeep(*(int*) arg);
}

static void coRunaway(void* arg)
{
  (void) arg;
  coDeep(1000000);                             // far past the stack: the guard page
}

static void coSelf(void* arg)
{
  *(CorCo**) arg = corCoCurrent();
}

static void coNothing(void* arg)
{
  (void) arg;
}

static void coPingPong(void* arg)
{
  long n = *(long*) arg;
  for (long i = 0; i < n; i++)
    corCoYield();
}

static double nowSeconds(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
}

static void corCoTest(void)
{
  //
  // Yield and resume: the resumer gets control back at each yield; the end is reported once
  //
  int    n       = 3;
  int    results = 0;
  CorCo* coP     = corCoCreate(coCounter, &n);

  coSteps = 0;
  while (corCoResume(coP) == false)
    results = results * 10 + coSteps;
  check((results == 123) && (coSteps == 3), "corCo: yield/resume steps through the coroutine, the end reported");

  //
  // The resumer's own registers - callee-saved, in a loop the compiler keeps there - survive
  //
  uint64_t a = 0x1111, b = 0x2222, c = 0x3333, d = 0x4444, e = 0x5555;
  n   = 1000;
  coP = corCoCreate(coCounter, &n);
  for (int i = 0; corCoResume(coP) == false; i++)
  {
    a = a * 3 + i; b ^= a; c += b; d = d * 7 + c; e -= d;
  }
  uint64_t a2 = 0x1111, b2 = 0x2222, c2 = 0x3333, d2 = 0x4444, e2 = 0x5555;
  for (int i = 0; i < 1000; i++)
  {
    a2 = a2 * 3 + i; b2 ^= a2; c2 += b2; d2 = d2 * 7 + c2; e2 -= d2;
  }
  check((a == a2) && (b == b2) && (c == c2) && (d == d2) && (e == e2), "corCo: the resumer's registers survive 1000 switches");

  //
  // Floating point on both sides
  //
  double x = 4.0;
  double y = 2.5;
  coP = corCoCreate(coFloatFn, &x);
  corCoResume(coP);
  y = y * 3.0;                                 // the resumer uses the FP registers meanwhile
  corCoResume(coP);
  check((x == 4.0 * 1.5 + 4.0 / 3.0) && (y == 7.5), "corCo: floating point both sides of a switch");

  //
  // Nested: a coroutine resumes another, whose yield goes back to it
  //
  coOrderN = 0;
  coP = corCoCreate(coOuter, NULL);
  bool done = corCoResume(coP);
  check(done && (coOrderN == 5) && (coOrder[0] == 1) && (coOrder[1] == 2) && (coOrder[2] == 3) && (coOrder[3] == 4) && (coOrder[4] == 5),
        "corCo: a coroutine resumes another; the inner yield returns to the outer");

  //
  // corCoCurrent: the coroutine inside, NULL outside
  //
  CorCo* seen = NULL;
  coP = corCoCreate(coSelf, &seen);
  CorCo* created = coP;
  corCoResume(coP);
  check((seen == created) && (corCoCurrent() == NULL), "corCo: corCoCurrent - the coroutine inside, NULL outside");

  //
  // Deep use of the stack: 200 frames of ~1 KiB, in a 256 KiB stack
  //
  int depth = 200;
  coP = corCoCreate(coDeepFn, &depth);
  corCoResume(coP);
  check(depth == 200, "corCo: 200 KiB of stack used");

  //
  // An overflow hits the guard page: SIGSEGV, in a child, not a silent write into the next stack
  //
  pid_t pid = fork();
  if (pid == 0)
  {
    CorCo* runawayP = corCoCreate(coRunaway, NULL);
    corCoResume(runawayP);
    _exit(0);                                  // not reached
  }
  int status = 0;
  waitpid(pid, &status, 0);
  check(WIFSIGNALED(status) && (WTERMSIG(status) == SIGSEGV), "corCo: a stack overflow is a SIGSEGV on the guard page");

  //
  // The pool: 100 000 coroutines created and finished - the same few stacks, no mmap each
  //
  double t0 = nowSeconds();
  for (int i = 0; i < 100000; i++)
  {
    coP = corCoCreate(coNothing, NULL);
    corCoResume(coP);
  }
  double t1 = nowSeconds();
  printf("      corCo: create + run + finish: %.0f ns\n", (t1 - t0) * 1e9 / 100000);
  check(true, "corCo: 100 000 coroutines through the pool");

  //
  // A switch: yield + resume, 10 million times
  //
  long rounds = 10000000;
  coP = corCoCreate(coPingPong, &rounds);
  t0 = nowSeconds();
  while (corCoResume(coP) == false)
    ;
  t1 = nowSeconds();
  printf("      corCo: yield + resume: %.1f ns\n", (t1 - t0) * 1e9 / rounds);
  check(true, "corCo: 10 million yield/resume round trips");
}



// -----------------------------------------------------------------------------
//
// corCoLoop - coroutines waiting on an epoll loop
//
static int  loopPipe[2];
static int  loopGot[4];

static void coReader(void* arg)
{
  (void) arg;
  short revents = 0;
  int   r       = corCoLoopWait(loopPipe[0], POLLIN, 2000, &revents);
  char  c       = 0;

  if (r == 1)
    (void) !read(loopPipe[0], &c, 1);
  loopGot[0] = r;
  loopGot[1] = c;
}

static void coSleeper(void* arg)
{
  (void) arg;
  double t0 = nowSeconds();
  loopGot[2] = corCoLoopWait(-1, 0, 50, NULL);           // a timer: 0 when the time is up
  loopGot[3] = (int) ((nowSeconds() - t0) * 1000);
}

static void corCoLoopTest(void)
{
  int efd = epoll_create1(0);

  (void) !pipe(loopPipe);
  corCoLoopInit(efd);

  CorCo* readerP  = corCoCreate(coReader, NULL);
  CorCo* sleeperP = corCoCreate(coSleeper, NULL);

  corCoLoopResume(readerP);                              // both start, both yield at their wait
  corCoLoopResume(sleeperP);

  (void) !write(loopPipe[1], "x", 1);                    // the reader's socket becomes ready

  struct epoll_event evV[8];
  double             t0 = nowSeconds();

  while ((nowSeconds() - t0) < 1.0)
  {
    int n = epoll_wait(efd, evV, 8, corCoLoopTimeoutMs());
    for (int i = 0; i < n; i++)
      corCoLoopEvent(evV[i].data.ptr, evV[i].events);
    corCoLoopExpire();
    if ((loopGot[0] != 0) && (loopGot[3] != 0))
      break;
  }

  check((loopGot[0] == 1) && (loopGot[1] == 'x'), "corCoLoop: a coroutine waits for a socket, the loop resumes it when it is ready");
  check((loopGot[2] == 0) && (loopGot[3] >= 45) && (loopGot[3] < 500), "corCoLoop: a timer wait (fd -1) - resumed when its time is up");
  check(corCoLoopTimeoutMs() == -1, "corCoLoop: nothing left waiting");
}



// -----------------------------------------------------------------------------
//
// corCoBlocking - a blocking call on a thread of its own; the loop goes on meanwhile
//
static double blockT0;
static int    blockDoneMs  = 0;                         // when the blocking call's coroutine resumed
static int    blockTimerMs = 0;                         // when the other coroutine's 50 ms timer fired
static int    blockRan     = 0;

static void sleep200(void* arg)
{
  usleep(200 * 1000);
  *(int*) arg += 1;
}

static void coBlocker(void* arg)
{
  (void) arg;
  corCoBlocking(sleep200, &blockRan);
  blockDoneMs = (int) ((nowSeconds() - blockT0) * 1000);
}

static void coTimer(void* arg)
{
  (void) arg;
  corCoLoopWait(-1, 0, 50, NULL);
  blockTimerMs = (int) ((nowSeconds() - blockT0) * 1000);
}

static void corCoBlockingTest(void)
{
  int efd = epoll_create1(0);

  corCoLoopInit(efd);
  blockT0 = nowSeconds();

  corCoLoopResume(corCoCreate(coBlocker, NULL));
  corCoLoopResume(corCoCreate(coTimer, NULL));

  struct epoll_event evV[8];

  while (((nowSeconds() - blockT0) < 2.0) && (blockDoneMs == 0))
  {
    int n = epoll_wait(efd, evV, 8, corCoLoopTimeoutMs());
    for (int i = 0; i < n; i++)
      corCoLoopEvent(evV[i].data.ptr, evV[i].events);
    corCoLoopExpire();
  }

  check((blockRan == 1) && (blockDoneMs >= 195) && (blockDoneMs < 1000), "corCoBlocking: the call ran, and the coroutine resumed when it was done");
  check((blockTimerMs >= 45) && (blockTimerMs < 150), "corCoBlocking: the loop went on meanwhile - another coroutine's 50 ms timer fired on time");

  corCoBlocking(sleep200, &blockRan);                    // not in a coroutine: right here
  check(blockRan == 2, "corCoBlocking: outside a coroutine, a plain call");
  close(efd);
}



// -----------------------------------------------------------------------------
//
// corCoLoopPark / Wake - a coroutine woken by another; one that nobody wakes times out
//
static void* parkHandle   = NULL;
static int   parkResult   = -2;
static int   parkMs       = 0;
static int   parkTimeout  = -2;
static int   parkTimeoutMs = 0;
static double parkT0;

static void coParker(void* arg)
{
  (void) arg;
  parkResult = corCoLoopPark(&parkHandle, -1);
  parkMs     = (int) ((nowSeconds() - parkT0) * 1000);
}

static void coWaker(void* arg)
{
  (void) arg;
  corCoLoopWait(-1, 0, 30, NULL);
  corCoLoopWake(parkHandle);
  corCoLoopWake(parkHandle);                             // twice is once
}

static void coLonely(void* arg)
{
  (void) arg;
  void* h = NULL;
  parkTimeout   = corCoLoopPark(&h, 50);
  parkTimeoutMs = (int) ((nowSeconds() - parkT0) * 1000);
}

static void corCoParkTest(void)
{
  int efd = epoll_create1(0);

  corCoLoopInit(efd);
  parkT0 = nowSeconds();

  corCoLoopResume(corCoCreate(coParker, NULL));
  corCoLoopResume(corCoCreate(coWaker,  NULL));
  corCoLoopResume(corCoCreate(coLonely, NULL));

  struct epoll_event evV[8];

  while (((nowSeconds() - parkT0) < 2.0) && ((parkResult == -2) || (parkTimeout == -2)))
  {
    int n = epoll_wait(efd, evV, 8, corCoLoopTimeoutMs());
    for (int i = 0; i < n; i++)
      corCoLoopEvent(evV[i].data.ptr, evV[i].events);
    corCoLoopExpire();
  }

  check((parkResult == 1) && (parkMs >= 28) && (parkMs < 200), "corCoLoopPark: woken by another coroutine (after its 30 ms timer)");
  check((parkTimeout == 0) && (parkTimeoutMs >= 45) && (parkTimeoutMs < 300), "corCoLoopPark: nobody wakes it - its 50 ms run out");
  check(corCoLoopPark(&parkHandle, 10) == -1, "corCoLoopPark: not in a coroutine - -1");
  check(corCoLoopTimeoutMs() == -1, "corCoLoopPark: nothing left waiting");
  close(efd);
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
#ifdef COR_T_ON
  check(ownerType == 'T' && ownerAux == 301 && strcmp(ownerMsg, "trace line") == 0, "COR_LIB_T reaches the owner, with its trace level");
#else
  check(ownerType == 0, "COR_LIB_T compiled out (not a debug build) - nothing reaches the owner");
#endif

  COR_LIB_W("warning %d", 7);
  check(ownerType == 'W' && ownerAux == -1 && strcmp(ownerMsg, "warning 7") == 0, "COR_LIB_W reaches the owner");

  check(logReturns(21) == 42 && ownerType == 'E' && strcmp(ownerMsg, "returning 42") == 0, "COR_LIB_RE logs an error and returns");

  char  in[] = "name,type,value";
  char* outV[4];
  int   items = corStringSplit(in, ',', outV, COR_VEC_SIZE(outV));
  check(items == 3 && strcmp(outV[2], "value") == 0, "corStringSplit");

  check(strcmp(COR_FT(true), "true") == 0 && strcmp(COR_FT(false), "false") == 0, "COR_FT");

  corCoTest();
  corCoLoopTest();
  corCoBlockingTest();
  corCoParkTest();

  return (failures == 0)? 0 : 1;
}
