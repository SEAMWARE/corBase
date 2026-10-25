//
// FILE            corHeaderIsCredential.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdbool.h>                           // bool
#include <stddef.h>                            // NULL

#include "corBase/corHeaderIsCredential.h"     // Own interface



// -----------------------------------------------------------------------------
//
// corHeaderIsCredential -
//
// Compared a byte at a time, lower-cased by hand (| 0x20 - every name in the list is letters and '-',
// which | 0x20 leaves as it is): strcasecmp is locale-aware and not async-signal-safe.
//
bool corHeaderIsCredential(const char* name)
{
  static const char* credentialV[] = { "authorization", "proxy-authorization", "cookie", "set-cookie", "x-auth-token", "fiware-token", NULL };

  if (name == NULL)
    return false;

  for (int i = 0; credentialV[i] != NULL; i++)
  {
    const char* a = name;
    const char* b = credentialV[i];

    while ((*a != 0) && (*b != 0) && ((*a | 0x20) == *b))
    {
      ++a;
      ++b;
    }

    if ((*a == 0) && (*b == 0))
      return true;
  }

  return false;
}
