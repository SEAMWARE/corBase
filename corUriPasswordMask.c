//
// FILE            corUriPasswordMask.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdio.h>                             // snprintf
#include <string.h>                            // strstr, strcspn, memchr

#include "corBase/corUriPasswordMask.h"        // Own interface



// -----------------------------------------------------------------------------
//
// corUriPasswordMask -
//
// The user information is what precedes the LAST '@' before the query or the fragment ('?', '#'), and its
// password what follows its first ':'. The last '@', not the first: a password written with an '@' in it
// (it should be %40, many are not) is masked whole. The authority is not taken to end at the first '/',
// for the same reason - a password with a '/' in it. The price: "http://host:8080/a@b", an '@' in a
// path behind a port, comes out as "http://host:***@b" - garbled, never a password shown.
//
char* corUriPasswordMask(const char* uri, char* buf, int bufSize)
{
  if (bufSize <= 0)
    return buf;

  if (uri == NULL)
  {
    buf[0] = 0;
    return buf;
  }

  const char* start = strstr(uri, "://");
  const char* at    = NULL;
  const char* colon = NULL;

  start = (start != NULL) ? start + 3 : uri;            // "user:password@host:3128" - a proxy's, without its scheme

  const char* end = start + strcspn(start, "?#");

  for (const char* p = start; p < end; p++)
  {
    if (*p == '@')
      at = p;
  }

  if (at != NULL)
    colon = memchr(start, ':', at - start);

  if (colon == NULL)
    snprintf(buf, bufSize, "%s", uri);
  else
    snprintf(buf, bufSize, "%.*s***%s", (int) (colon + 1 - uri), uri, at);

  return buf;
}
