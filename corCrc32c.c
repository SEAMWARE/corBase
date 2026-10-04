//
// FILE            corCrc32c.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
// CRC-32C: what corDB's log records carry (coraine doc/cordb-persistence.md § 4) - a torn or rotten
// record is found, never applied.
//
#include <stdbool.h>                    // bool
#include <stdint.h>                     // uint32_t, uint64_t
#include <string.h>                     // memcpy

#if defined(__x86_64__)
#include <nmmintrin.h>                  // _mm_crc32_u8, _mm_crc32_u64
#elif defined(__aarch64__)
#include <arm_acle.h>                   // __crc32cb, __crc32cd
#include <sys/auxv.h>                   // getauxval
#include <asm/hwcap.h>                  // HWCAP_CRC32
#endif

#include "corBase/corCrc32c.h"          // Own interface



// -----------------------------------------------------------------------------
//
// table - the software fallback, built on first use (reflected polynomial 0x82F63B78)
//
static uint32_t table[256];
static bool     tableReady = false;

static uint32_t crcSoftware(uint32_t crc, const unsigned char* p, size_t len)
{
  if (tableReady == false)                       // idempotent: two threads build the same table
  {
    for (uint32_t i = 0; i < 256; i++)
    {
      uint32_t c = i;

      for (int k = 0; k < 8; k++)
        c = (c & 1) ? (c >> 1) ^ 0x82F63B78 : (c >> 1);
      table[i] = c;
    }
    tableReady = true;
  }

  while (len-- > 0)
    crc = table[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

  return crc;
}



#if defined(__x86_64__)
// -----------------------------------------------------------------------------
//
// crcHardware - SSE4.2, eight bytes an instruction
//
__attribute__((target("sse4.2")))
static uint32_t crcHardware(uint32_t crc, const unsigned char* p, size_t len)
{
  uint64_t c = crc;

  for (; len >= 8; p += 8, len -= 8)
  {
    uint64_t v;

    memcpy(&v, p, 8);
    c = _mm_crc32_u64(c, v);
  }

  uint32_t c32 = (uint32_t) c;

  while (len-- > 0)
    c32 = _mm_crc32_u8(c32, *p++);

  return c32;
}

static bool hardware(void)
{
  return __builtin_cpu_supports("sse4.2");
}

#elif defined(__aarch64__)
// -----------------------------------------------------------------------------
//
// crcHardware - the ARMv8 CRC extension, eight bytes an instruction
//
__attribute__((target("+crc")))
static uint32_t crcHardware(uint32_t crc, const unsigned char* p, size_t len)
{
  for (; len >= 8; p += 8, len -= 8)
  {
    uint64_t v;

    memcpy(&v, p, 8);
    crc = __crc32cd(crc, v);
  }

  while (len-- > 0)
    crc = __crc32cb(crc, *p++);

  return crc;
}

static bool hardware(void)
{
  return (getauxval(AT_HWCAP) & HWCAP_CRC32) != 0;
}

#else
static uint32_t crcHardware(uint32_t crc, const unsigned char* p, size_t len) { return crcSoftware(crc, p, len); }
static bool     hardware(void) { return false; }
#endif



// -----------------------------------------------------------------------------
//
// corCrc32c -
//
uint32_t corCrc32c(uint32_t crc, const void* buf, size_t len)
{
  static int hw = -1;                            // -1: not asked yet

  if (hw == -1)
    hw = (hardware() == true) ? 1 : 0;

  crc = ~crc;
  crc = (hw == 1) ? crcHardware(crc, (const unsigned char*) buf, len) : crcSoftware(crc, (const unsigned char*) buf, len);

  return ~crc;
}
