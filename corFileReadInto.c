//
// FILE            corFileReadInto.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#include <fcntl.h>                                    // open, O_RDONLY
#include <unistd.h>                                   // read, close

#include "corBase/corFileReadInto.h"                  // Own interface



// -----------------------------------------------------------------------------
//
// corFileReadInto -
//
int corFileReadInto(const char* path, char* buf, int bufSize)
{
  if ((buf == NULL) || (bufSize < 1))
    return -1;

  int fd = open(path, O_RDONLY);
  if (fd < 0)
    return -1;

  int total = 0;

  while (total < bufSize - 1)
  {
    int n = read(fd, &buf[total], bufSize - 1 - total);

    if (n < 0)
    {
      close(fd);
      return -1;
    }

    if (n == 0)
      break;

    total += n;
  }

  close(fd);
  buf[total] = 0;

  return total;
}
