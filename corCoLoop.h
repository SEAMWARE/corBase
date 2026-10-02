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

#endif  // CORBASE_CORCOLOOP_H_
