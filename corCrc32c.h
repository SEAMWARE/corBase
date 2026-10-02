#ifndef CORBASE_CORCRC32C_H_
#define CORBASE_CORCRC32C_H_

//
// FILE            corCrc32c.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#include <stddef.h>                     // size_t
#include <stdint.h>                     // uint32_t



// -----------------------------------------------------------------------------
//
// corCrc32c - CRC-32C (Castagnoli) of len bytes, continuing from crc (0 to start)
//
// The CPU's own instruction where it has one (SSE4.2 on x86-64, the CRC extension on ARMv8), a
// table where it has not - the same value either way. corCrc32c(0, "123456789", 9) == 0xE3069283.
//
extern uint32_t corCrc32c(uint32_t crc, const void* buf, size_t len);

#endif  // CORBASE_CORCRC32C_H_
