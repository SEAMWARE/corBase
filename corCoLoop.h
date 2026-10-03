//
// FILE            corCoLoop.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORCOLOOP_H_
#define CORBASE_CORCOLOOP_H_

//
// corCoLoop - coroutines on an epoll event loop (coraine doc/coroutines.md § 3)
//
// An event loop that runs coroutines (corCo) gives them a way to wait without stopping the loop: a
// coroutine that needs a socket ready hands it to the loop's epoll and yields (corCoLoopWait); the loop
// resumes it when the socket is ready or its time is up. Per thread - a loop is one thread.
//
// The loop's side, three calls in its own epoll loop:
//
//   corCoLoopInit(epollFd)                        once, on the loop's thread
//   epoll_wait(epollFd, ..., corCoLoopTimeoutMs()) the loop's own timeout clamped to it
//   corCoLoopEvent(ptr, events)                   for every event: true if it was a waiting coroutine's
//   corCoLoopExpire()                             after the events: the coroutines whose time is up
//
// A waiting coroutine's epoll entry carries a pointer with its lowest bit set - the loop's own entries
// (connections, listeners) are pointers to aligned structs, lowest bit clear.
//
#include <stdbool.h>                    // bool
#include <stdint.h>                     // uint32_t



// -----------------------------------------------------------------------------
//
// corCoLoopInit - this thread runs an epoll loop with coroutines on it
//
extern void corCoLoopInit(int epollFd);



// -----------------------------------------------------------------------------
//
// corCoLoopWait - inside a coroutine of this thread's loop: until fd is ready for events (POLLIN, POLLOUT)
//
// Returns as poll() on one fd does: 1 ready (*reventsP gets what for), 0 the time ran out (timeoutMs,
// < 0: no limit), -1 an error. fd < 0: a plain timer. timeoutMs 0, or no loop on this thread, or not in
// a coroutine: poll(). Never with a lock held - the coroutine yields.
//
extern int corCoLoopWait(int fd, short events, int timeoutMs, short* reventsP);



// -----------------------------------------------------------------------------
//
// corCoLoopTimeoutMs - how long the loop may sleep: until the nearest deadline, -1 if there is none
//
extern int corCoLoopTimeoutMs(void);



// -----------------------------------------------------------------------------
//
// corCoLoopEvent - an event from the loop's epoll_wait: a waiting coroutine's (resumed - true), or not
//
extern bool corCoLoopEvent(void* ptr, uint32_t events);



// -----------------------------------------------------------------------------
//
// corCoLoopExpire - resume every coroutine whose time is up
//
extern void corCoLoopExpire(void);



// -----------------------------------------------------------------------------
//
// corCoLoopResume - resume a coroutine from the loop (also for the loop's own first resume of one)
//
// Calls the after-resume hook (corCoLoopResumeHookSet) when the coroutine yields or ends.
//
extern void corCoLoopResume(void* coP);



// -----------------------------------------------------------------------------
//
// corCoLoopResumeHookSet - called on the loop each time a coroutine gives control back to it
//
// For an application that binds per-request state to the thread (corRest's corRestP): unbind it there.
//
extern void corCoLoopResumeHookSet(void (*hook)(void));



// -----------------------------------------------------------------------------
//
// corCoBlocking - fn(arg), which blocks in a library that knows nothing of the loop, without blocking it
//
// Inside a coroutine of this thread's loop: fn runs on a thread of its own while the coroutine waits
// on an eventfd and the loop serves everything else; returns when fn has. Anywhere else (or with no
// thread to be had): fn(arg), here. fn must not touch the caller's thread-bound state - it runs on
// another thread - and whatever the caller had bound to its thread may have been rebound meanwhile
// (corRest's corRestP: save it before, restore it after).
//
extern void corCoBlocking(void (*fn)(void*), void* arg);



// -----------------------------------------------------------------------------
//
// corCoLoopPark / corCoLoopWake - a coroutine waits until another (of the same loop) wakes it
//
// corCoLoopPark: inside a coroutine of this thread's loop only; *handleP is set to what corCoLoopWake
// takes, valid until Park returns (set it NULL then). Returns 1 woken, 0 the time ran out (timeoutMs;
// < 0: no limit), -1 not in a coroutine of a loop.
// corCoLoopWake: on the loop's thread - a coroutine of it, or the loop. The parked coroutine resumes
// on the loop's next round, not inside the call. Waking twice is waking once.
//
extern int  corCoLoopPark(void** handleP, int timeoutMs);
extern void corCoLoopWake(void* handle);

#endif  // CORBASE_CORCOLOOP_H_
