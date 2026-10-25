//
// FILE            corHeaderIsCredential.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORHEADERISCREDENTIAL_H_
#define CORBASE_CORHEADERISCREDENTIAL_H_

#include <stdbool.h>                           // bool



// -----------------------------------------------------------------------------
//
// corHeaderIsCredential - an HTTP header whose value is a credential, its name in any case: Authorization,
// Proxy-Authorization, Cookie, Set-Cookie, X-Auth-Token, Fiware-Token
//
// For whatever shows a header - a trace, a crash report - to show "***" instead of its value. No library
// call, no allocation: a signal handler may call it.
//
extern bool corHeaderIsCredential(const char* name);

#endif  // CORBASE_CORHEADERISCREDENTIAL_H_
