#ifndef CORBASE_CORSTRINGARRAYJOIN_H_
#define CORBASE_CORSTRINGARRAYJOIN_H_

// 
// FILE            corStringArrayJoin.h
// 
// AUTHOR          Ken Zangelin
// 
// Copyright 2020 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//



// -----------------------------------------------------------------------------
//
// corStringArrayJoin - join a string vector into a single string
//
extern void corStringArrayJoin(char* output, int outputLen, char** stringV, int vecItems, const char* separator);

#endif  // CORBASE_CORSTRINGARRAYJOIN_H_
