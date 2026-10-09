//
// FILE            corCo.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
//
// SPDX-License-Identifier: Apache-2.0
//
// corCo - see corCo.h. The switch itself is the assembly block below - one per architecture.
//
#include <pthread.h>                    // pthread_key_create, pthread_once, pthread_setspecific
#include <stdbool.h>                    // bool
#include <stdint.h>                     // uintptr_t
#include <stdlib.h>                     // malloc, free
#include <string.h>                     // memset
#include <unistd.h>                     // sysconf
#include <sys/mman.h>                   // mmap, mprotect, munmap

#include "corBase/corCo.h"              // Own interface



// -----------------------------------------------------------------------------
//
// corCoSwitch - save the running context, continue another
//
//   void corCoSwitch(void** fromSpP, void* toSp);
//
// Pushes what the ABI says a callee must preserve onto the running stack, stores the stack pointer in
// *fromSpP, loads toSp, pops the same set from there and returns - into wherever the other context
// last called corCoSwitch, or, for a coroutine that never ran, into the entry its first frame names
// (frameBuild lays that frame out exactly as this pushes).
//
// Nothing else: caller-saved registers are the caller's business (corCoSwitch is a call like any
// other), and there is no signal mask to swap, so no system call.
//
// File-level assembly, one block per architecture - not asm() inside a function: there the compiler
// would not know the stack pointer changed under it, and could keep values across the switch in
// registers or in a frame that is no longer the one running.
//
extern void corCoSwitch(void** fromSpP, void* toSp);

#if defined(__x86_64__)
//
// System V AMD64: rbx, rbp, r12-r15 callee-saved, plus the SSE control/status word (mxcsr) and the
// x87 control word. The frame, from the saved stack pointer up:
//
//   +0   mxcsr (4) + x87 control word (2) + pad (2)
//   +8   r15   +16 r14   +24 r13   +32 r12   +40 rbx   +48 rbp
//   +56  return address
//
__asm__(
  "  .text\n"
  "  .globl  corCoSwitch\n"
  "  .type   corCoSwitch, @function\n"
  "corCoSwitch:\n"
  "  pushq   %rbp\n"
  "  pushq   %rbx\n"
  "  pushq   %r12\n"
  "  pushq   %r13\n"
  "  pushq   %r14\n"
  "  pushq   %r15\n"
  "  subq    $8, %rsp\n"
  "  stmxcsr (%rsp)\n"
  "  fnstcw  4(%rsp)\n"
  "  movq    %rsp, (%rdi)\n"
  "  movq    %rsi, %rsp\n"
  "  ldmxcsr (%rsp)\n"
  "  fldcw   4(%rsp)\n"
  "  addq    $8, %rsp\n"
  "  popq    %r15\n"
  "  popq    %r14\n"
  "  popq    %r13\n"
  "  popq    %r12\n"
  "  popq    %rbx\n"
  "  popq    %rbp\n"
  "  ret\n"
  "  .size   corCoSwitch, .-corCoSwitch\n"
);
#elif defined(__aarch64__)
//
// AAPCS64: x19-x28, x29 (frame pointer), x30 (link register) and the low halves of v8-v15 (d8-d15)
// callee-saved. The frame, from the saved stack pointer up (160 bytes, 16-aligned):
//
//   +0  x19 x20   +16 x21 x22   +32 x23 x24   +48 x25 x26   +64 x27 x28   +80 x29 x30
//   +96 d8 d9     +112 d10 d11  +128 d12 d13  +144 d14 d15
//
// `ret` goes to x30: for a coroutine that never ran, its entry.
//
__asm__(
  "  .text\n"
  "  .globl  corCoSwitch\n"
  "  .type   corCoSwitch, %function\n"
  "corCoSwitch:\n"
  "  sub     sp, sp, #160\n"
  "  stp     x19, x20, [sp, #0]\n"
  "  stp     x21, x22, [sp, #16]\n"
  "  stp     x23, x24, [sp, #32]\n"
  "  stp     x25, x26, [sp, #48]\n"
  "  stp     x27, x28, [sp, #64]\n"
  "  stp     x29, x30, [sp, #80]\n"
  "  stp     d8,  d9,  [sp, #96]\n"
  "  stp     d10, d11, [sp, #112]\n"
  "  stp     d12, d13, [sp, #128]\n"
  "  stp     d14, d15, [sp, #144]\n"
  "  mov     x2, sp\n"
  "  str     x2, [x0]\n"
  "  mov     sp, x1\n"
  "  ldp     x19, x20, [sp, #0]\n"
  "  ldp     x21, x22, [sp, #16]\n"
  "  ldp     x23, x24, [sp, #32]\n"
  "  ldp     x25, x26, [sp, #48]\n"
  "  ldp     x27, x28, [sp, #64]\n"
  "  ldp     x29, x30, [sp, #80]\n"
  "  ldp     d8,  d9,  [sp, #96]\n"
  "  ldp     d10, d11, [sp, #112]\n"
  "  ldp     d12, d13, [sp, #128]\n"
  "  ldp     d14, d15, [sp, #144]\n"
  "  add     sp, sp, #160\n"
  "  ret\n"
  "  .size   corCoSwitch, .-corCoSwitch\n"
);
#else
#error "corCo: no context switch for this architecture (x86-64 and aarch64 only)"
#endif



// -----------------------------------------------------------------------------
//
// The stack: CORCO_STACK_SIZE of address space, the lowest page of it the guard
//
#define CORCO_STACK_SIZE   (256 * 1024)
#define CORCO_POOL_MAX     256                          // stacks kept per thread beyond that are unmapped



// -----------------------------------------------------------------------------
//
// CorCo -
//
struct CorCo
{
  void*          sp;                                    // the saved stack pointer, while not running
  void*          resumerSp;                             // where corCoYield / the end go back to
  CorCo*         resumer;                               // the coroutine that resumed this one, NULL: the thread
  CorCoFunction  fn;
  void*          arg;
  void*          userData;
  bool           finished;
  char*          stackP;                                // the mapping - guard page included
  CorCo*         next;                                  // in the pool
};



// -----------------------------------------------------------------------------
//
// Per thread: the running coroutine, and the pool of finished ones (stacks ready for the next)
//
static __thread CorCo*  current   = NULL;
static __thread CorCo*  pool      = NULL;
static __thread int     poolSize  = 0;



// -----------------------------------------------------------------------------
//
// coEntry - where a coroutine starts: its function, then back to the resumer, for good
//
// Reached by corCoSwitch's `ret` into the first frame corCoCreate built - so it is never called and
// never returns: the coroutine is `current` already, set by corCoResume.
//
static void coEntry(void)
{
  CorCo* coP = current;

  coP->fn(coP->arg);

  coP->finished = true;
  corCoSwitch(&coP->sp, coP->resumerSp);

  __builtin_unreachable();
}



// -----------------------------------------------------------------------------
//
// frameBuild - the first frame on a fresh stack, as corCoSwitch would have left it: zeroed
// callee-saved registers, the default floating-point control words, and coEntry to return to
//
static void* frameBuild(char* top)
{
  uintptr_t t = ((uintptr_t) top) & ~((uintptr_t) 15);         // 16-aligned

#if defined(__x86_64__)
  //
  // corCoSwitch pops 8 bytes of control words, six registers, then `ret`s. coEntry must then see the
  // stack as a call leaves it: (rsp + 8) % 16 == 0 - so the return address sits at a 16-aligned
  // address, with 8 bytes of room above it.
  //
  uint64_t* sp = (uint64_t*) (t - 16);

  *sp-- = (uint64_t) (uintptr_t) coEntry;                         // return address
  for (int i = 0; i < 6; i++)
    *sp-- = 0;                                                    // rbp rbx r12 r13 r14 r15
  *sp = 0x037F00001F80ULL;                                        // mxcsr 0x1F80 | x87 control word 0x037F << 32

  return sp;
#elif defined(__aarch64__)
  //
  // corCoSwitch loads 160 bytes - x19..x30, d8..d15 - and `ret`s to x30. coEntry then starts at a
  // 16-aligned sp: the frame's top.
  //
  uint64_t* sp = (uint64_t*) (t - 160);

  memset(sp, 0, 160);
  sp[11] = (uint64_t) (uintptr_t) coEntry;                        // x30, at +88

  return sp;
#endif
}



// -----------------------------------------------------------------------------
//
// stackMap - a fresh stack: address space reserved, not committed, the lowest page made a guard
//
static char* stackMap(void)
{
  char* p = mmap(NULL, CORCO_STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE | MAP_STACK, -1, 0);

  if (p == MAP_FAILED)
    return NULL;

  if (mprotect(p, (size_t) sysconf(_SC_PAGESIZE), PROT_NONE) != 0)
  {
    munmap(p, CORCO_STACK_SIZE);
    return NULL;
  }

  return p;
}



// -----------------------------------------------------------------------------
//
// poolRelease - unmap and free the ending thread's pool (a pthread key destructor)
//
// The pool is thread-local: a thread that ends (an HTTP loop at shutdown) leaves it unreachable - up to
// CORCO_POOL_MAX coroutines, each with its 256 KB stack mapped
//
static pthread_key_t  poolKey;
static pthread_once_t poolKeyOnce = PTHREAD_ONCE_INIT;

static void poolRelease(void* unused)
{
  (void) unused;

  while (pool != NULL)
  {
    CorCo* coP = pool;

    pool = coP->next;
    munmap(coP->stackP, CORCO_STACK_SIZE);
    free(coP);
  }

  poolSize = 0;
}

static void poolKeyCreate(void)
{
  pthread_key_create(&poolKey, poolRelease);
}



// -----------------------------------------------------------------------------
//
// coRelease - a coroutine done with: to the pool, or unmapped when the pool is full
//
static void coRelease(CorCo* coP)
{
  if (poolSize < CORCO_POOL_MAX)
  {
    if (pool == NULL)                                // the thread's first: its pool goes when the thread does
    {
      pthread_once(&poolKeyOnce, poolKeyCreate);
      pthread_setspecific(poolKey, &pool);
    }

    coP->next = pool;
    pool      = coP;
    poolSize += 1;
    return;
  }

  munmap(coP->stackP, CORCO_STACK_SIZE);
  free(coP);
}



// -----------------------------------------------------------------------------
//
// corCoCreate -
//
CorCo* corCoCreate(CorCoFunction fn, void* arg)
{
  CorCo* coP = pool;

  if (coP != NULL)
  {
    pool      = coP->next;
    poolSize -= 1;
  }
  else
  {
    if ((coP = malloc(sizeof(CorCo))) == NULL)
      return NULL;

    if ((coP->stackP = stackMap()) == NULL)
    {
      free(coP);
      return NULL;
    }
  }

  coP->sp        = frameBuild(coP->stackP + CORCO_STACK_SIZE);
  coP->resumerSp = NULL;
  coP->resumer   = NULL;
  coP->fn        = fn;
  coP->arg       = arg;
  coP->userData  = NULL;
  coP->finished  = false;
  coP->next      = NULL;

  return coP;
}



// -----------------------------------------------------------------------------
//
// corCoResume -
//
bool corCoResume(CorCo* coP)
{
  coP->resumer = current;
  current      = coP;

  corCoSwitch(&coP->resumerSp, coP->sp);                          // back here when it yields or ends

  current = coP->resumer;

  if (coP->finished == false)
    return false;

  coRelease(coP);
  return true;
}



// -----------------------------------------------------------------------------
//
// corCoYield -
//
void corCoYield(void)
{
  CorCo* coP = current;

  if (coP == NULL)
    return;

  corCoSwitch(&coP->sp, coP->resumerSp);                          // back here when resumed
}



// -----------------------------------------------------------------------------
//
// corCoCurrent -
//
CorCo* corCoCurrent(void)
{
  return current;
}



// -----------------------------------------------------------------------------
//
// corCoDestroy -
//
void corCoDestroy(CorCo* coP)
{
  if ((coP == NULL) || (coP == current))
    return;

  coRelease(coP);
}



// -----------------------------------------------------------------------------
//
// corCoUserDataSet / corCoUserData -
//
void corCoUserDataSet(CorCo* coP, void* dataP)
{
  coP->userData = dataP;
}

void* corCoUserData(CorCo* coP)
{
  return coP->userData;
}



// -----------------------------------------------------------------------------
//
// corCoStackSize -
//
size_t corCoStackSize(void)
{
  return CORCO_STACK_SIZE;
}
