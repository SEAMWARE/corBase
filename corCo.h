//
// FILE            corCo.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORCO_H_
#define CORBASE_CORCO_H_

//
// corCo - coroutines: a function that runs on a stack of its own, and can stop half way (corCoYield)
// and be continued later (corCoResume), on the same thread. See coraine doc/coroutines.md.
//
// The switch is our own, a few instructions per architecture (corCo.c - x86-64 and aarch64):
// the callee-saved registers and the stack pointer, nothing else - no signal mask, no system call.
//
// A coroutine belongs to the thread that created it: it is resumed on that thread only. Stacks come
// from a pool of the thread's - a coroutine that has finished gives its stack back, and the next one
// takes it, so in the steady state a coroutine costs no mmap.
//
// Each stack: CORCO_STACK_SIZE of address space, mmap()ed MAP_NORESERVE - only the pages a coroutine
// touches cost memory - with a guard page below it, so an overflow is a SIGSEGV on the guard, not a
// silent write into the next stack.
//
#include <stdbool.h>                    // bool
#include <stddef.h>                     // size_t



// -----------------------------------------------------------------------------
//
// CorCo - a coroutine (opaque)
//
typedef struct CorCo CorCo;

typedef void (*CorCoFunction)(void* arg);



// -----------------------------------------------------------------------------
//
// corCoCreate - a coroutine that will run fn(arg) when first resumed; NULL when out of memory
//
// It does not run yet. Every coroutine created must be resumed until it finishes: its stack goes back
// to the pool then (or freed with corCoDestroy, if it never will finish).
//
extern CorCo* corCoCreate(CorCoFunction fn, void* arg);



// -----------------------------------------------------------------------------
//
// corCoResume - run the coroutine until it yields or its function returns; true when it has finished
//
// Called from the thread's own stack or from another coroutine (which is then the one corCoYield
// returns to). A finished coroutine is gone: its stack is back in the pool and coP may not be used
// again.
//
extern bool corCoResume(CorCo* coP);



// -----------------------------------------------------------------------------
//
// corCoYield - inside a coroutine: back to whoever resumed it; returns when it is resumed again
//
// Outside a coroutine it does nothing. Never with a lock held: another coroutine of this thread that
// wants the lock would wait for ever - the holder cannot run.
//
extern void corCoYield(void);



// -----------------------------------------------------------------------------
//
// corCoCurrent - the coroutine running on this thread, NULL on the thread's own stack
//
extern CorCo* corCoCurrent(void);



// -----------------------------------------------------------------------------
//
// corCoDestroy - a coroutine that will never be resumed again: its stack back to the pool
//
// Not the running one. Whatever its stack still held is simply dropped - nothing on it is unwound.
//
extern void corCoDestroy(CorCo* coP);



// -----------------------------------------------------------------------------
//
// corCoUserData - one pointer for the caller's own use (the scheduler's request, ...)
//
extern void  corCoUserDataSet(CorCo* coP, void* dataP);
extern void* corCoUserData(CorCo* coP);



// -----------------------------------------------------------------------------
//
// corCoStackSize - the address space of a coroutine's stack (fixed: 256 KiB)
//
extern size_t corCoStackSize(void);

#endif  // CORBASE_CORCO_H_
