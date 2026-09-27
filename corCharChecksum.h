//
// FILE            corCharChecksum.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORCHARCHECKSUM_H_
#define CORBASE_CORCHARCHECKSUM_H_



// -----------------------------------------------------------------------------
//
// corCharChecksum - simple checksum of 'char' of a sized buffer
//
extern char corCharChecksum(void* buf, int bufLen);

#endif  // CORBASE_CORCHARCHECKSUM_H_
