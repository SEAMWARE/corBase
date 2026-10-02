//
// FILE            corCoLoop.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
//
// SPDX-License-Identifier: Apache-2.0
//
#include <errno.h>                      // errno, ENOENT
#include <poll.h>                       // poll, POLLIN, POLLOUT
#include <stdbool.h>                    // bool
#include <stdint.h>                     // uintptr_t
#include <string.h>                     // memset
#include <time.h>                       // clock_gettime
#include <pthread.h>                    // pthread_create
#include <unistd.h>                     // write, close
#include <sys/eventfd.h>                // eventfd
#include <sys/epoll.h>                  // epoll_ctl

#include "corBase/corCo.h"              // corCoCurrent, corCoYield, corCoResume
#include "corBase/corCoLoop.h"          // Own interface



// -----------------------------------------------------------------------------
//
// CoWait - a coroutine waiting for a socket, or for time (fd < 0); it lives on the coroutine's stack
//
typedef struct CoWait
{
  CorCo*          coP;
  int             fd;
  short           revents;
  bool            ready;
  long long       deadline;        // CLOCK_MONOTONIC ms; 0: no limit
  struct CoWait*  next;            // the loop's list of waits with a deadline
  struct CoWait*  prev;
} CoWait;

#define CO_TAG ((uintptr_t) 1)

static __thread int      loopFd   = -1;       // this thread's loop
static __thread CoWait*  timed    = NULL;     // the waits with a deadline
static void            (*resumeHook)(void) = NULL;



// -----------------------------------------------------------------------------
//
// nowMs -
//
static long long nowMs(void)
{
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long) ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}



// -----------------------------------------------------------------------------
//
// timedUnlink -
//
static void timedUnlink(CoWait* wP)
{
  if (wP->prev != NULL)       wP->prev->next = wP->next;
  else if (timed == wP)       timed          = wP->next;
  if (wP->next != NULL)       wP->next->prev = wP->prev;
  wP->next     = NULL;
  wP->prev     = NULL;
  wP->deadline = 0;
}



// -----------------------------------------------------------------------------
//
// corCoLoopInit -
//
void corCoLoopInit(int epollFd)
{
  loopFd = epollFd;
}



// -----------------------------------------------------------------------------
//
// corCoLoopResumeHookSet -
//
void corCoLoopResumeHookSet(void (*hook)(void))
{
  resumeHook = hook;
}



// -----------------------------------------------------------------------------
//
// corCoLoopResume -
//
void corCoLoopResume(void* coP)
{
  corCoResume((CorCo*) coP);

  if (resumeHook != NULL)
    resumeHook();
}



// -----------------------------------------------------------------------------
//
// corCoLoopWait -
//
// One system call per wait: a socket stays in the loop's set, disarmed (EPOLLONESHOT) between waits -
// MOD re-arms it, ADD only the first time; a closed socket leaves the set by itself. A wait that times
// out is still armed, its entry pointing at this CoWait on a stack about to move on: disarmed then.
//
int corCoLoopWait(int fd, short events, int timeoutMs, short* reventsP)
{
  if ((loopFd < 0) || (timeoutMs == 0) || (corCoCurrent() == NULL))
  {
    struct pollfd p = { fd, events, 0 };
    int           r;

    while (((r = poll(&p, 1, timeoutMs)) < 0) && (errno == EINTR))
      ;

    if ((r > 0) && (reventsP != NULL))
      *reventsP = p.revents;
    return r;
  }

  CoWait w;

  memset(&w, 0, sizeof(w));
  w.coP = corCoCurrent();
  w.fd  = fd;

  if (fd >= 0)
  {
    struct epoll_event ev;

    ev.events   = EPOLLONESHOT | ((events & POLLIN) ? EPOLLIN : 0) | ((events & POLLOUT) ? EPOLLOUT : 0);
    ev.data.ptr = (void*) (((uintptr_t) &w) | CO_TAG);

    if ((epoll_ctl(loopFd, EPOLL_CTL_MOD, fd, &ev) != 0) && ((errno != ENOENT) || (epoll_ctl(loopFd, EPOLL_CTL_ADD, fd, &ev) != 0)))
      return -1;
  }

  if (timeoutMs > 0)
  {
    w.deadline = nowMs() + timeoutMs;
    w.next     = timed;
    if (timed != NULL)
      timed->prev = &w;
    timed = &w;
  }

  corCoYield();

  if ((fd >= 0) && (w.ready == false))
  {
    struct epoll_event ev;

    memset(&ev, 0, sizeof(ev));
    epoll_ctl(loopFd, EPOLL_CTL_MOD, fd, &ev);
  }

  if (w.deadline != 0)
    timedUnlink(&w);

  if (w.ready == false)
    return 0;

  if (reventsP != NULL)
    *reventsP = w.revents;
  return 1;
}



// -----------------------------------------------------------------------------
//
// corCoLoopTimeoutMs -
//
int corCoLoopTimeoutMs(void)
{
  if (timed == NULL)
    return -1;

  long long now  = nowMs();
  long long next = timed->deadline;

  for (CoWait* wP = timed->next; wP != NULL; wP = wP->next)
  {
    if (wP->deadline < next)
      next = wP->deadline;
  }

  return (next <= now) ? 0 : (int) (next - now);
}



// -----------------------------------------------------------------------------
//
// corCoLoopEvent -
//
bool corCoLoopEvent(void* ptr, uint32_t events)
{
  if ((((uintptr_t) ptr) & CO_TAG) == 0)
    return false;

  CoWait* wP = (CoWait*) (((uintptr_t) ptr) & ~CO_TAG);

  if (wP->deadline != 0)
    timedUnlink(wP);

  wP->ready   = true;
  wP->revents = ((events & EPOLLIN)  ? POLLIN  : 0) | ((events & EPOLLOUT) ? POLLOUT : 0) |
                ((events & EPOLLERR) ? POLLERR : 0) | ((events & EPOLLHUP) ? POLLHUP : 0);

  corCoLoopResume(wP->coP);
  return true;
}



// -----------------------------------------------------------------------------
//
// corCoLoopExpire - one at a time: a resumed coroutine changes the list
//
void corCoLoopExpire(void)
{
  while (true)
  {
    long long now = nowMs();
    CoWait*   wP  = timed;

    while ((wP != NULL) && (wP->deadline > now))
      wP = wP->next;

    if (wP == NULL)
      return;

    timedUnlink(wP);
    wP->ready = false;
    corCoLoopResume(wP->coP);
  }
}



// -----------------------------------------------------------------------------
//
// Blocking - one corCoBlocking call; on the coroutine's stack, which waits for the thread to finish
//
typedef struct Blocking
{
  void  (*fn)(void*);
  void*   arg;
  int     efd;
} Blocking;

static void* blockingThread(void* p)
{
  Blocking* bP  = (Blocking*) p;
  uint64_t  one = 1;

  bP->fn(bP->arg);
  (void) !write(bP->efd, &one, sizeof(one));      // the last touch of *bP: the coroutine may free it now
  return NULL;
}



// -----------------------------------------------------------------------------
//
// corCoBlocking -
//
void corCoBlocking(void (*fn)(void*), void* arg)
{
  if ((loopFd < 0) || (corCoCurrent() == NULL))
  {
    fn(arg);
    return;
  }

  Blocking b = { fn, arg, eventfd(0, EFD_CLOEXEC) };

  if (b.efd < 0)
  {
    fn(arg);
    return;
  }

  pthread_t      tid;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  int r = pthread_create(&tid, &attr, blockingThread, &b);
  pthread_attr_destroy(&attr);

  if (r != 0)
  {
    close(b.efd);
    fn(arg);
    return;
  }

  //
  // No limit: fn keeps its own. And b must outlive the thread's write - so a wait the loop could not
  // take (-1) still waits, the plain way
  //
  short revents;
  int   w;

  while ((w = corCoLoopWait(b.efd, POLLIN, -1, &revents)) == 0)
    ;

  if (w < 0)
  {
    struct pollfd p = { b.efd, POLLIN, 0 };

    while ((poll(&p, 1, -1) < 0) && (errno == EINTR))
      ;
  }

  close(b.efd);
}
