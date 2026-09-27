//
// FILE            corFileRead.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2019 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//
#include <stdio.h>                                 // snprintf
#include <stdlib.h>                                // free
#include <sys/types.h>                             // needed for stat
#include <sys/stat.h>                              // struct stat
#include <unistd.h>                                // stat(), open()
#include <fcntl.h>                                 // O_RDONLY
#include <errno.h>                                 // errno
#include <string.h>                                // strerror

#include "corBase/corLibLog.h"                     // log macros
#include "corBase/corFileRead.h"                   // Own interface



// -----------------------------------------------------------------------------
//
// corFileRead -
//
int corFileRead(char* base, char* relPath, char** bufP, int* bufLenP)
{
  char         path[1024];
  struct stat  statBuf;

  snprintf(path, sizeof(path), "%s/%s", base, relPath);

  if (stat(path, &statBuf) == -1)
    COR_LIB_RE(1, "stat(%s): %s", path, strerror(errno));

  int fd = open(path, O_RDONLY);
  if (fd == -1)
    COR_LIB_RE(1, "open(%s): %s", path, strerror(errno));

  char* fileBuf = (char*) malloc(statBuf.st_size + 1);
  if (fileBuf == NULL)
  {
    close(fd);
    COR_LIB_RE(2, "malloc(%ld bytes) for %s", (long) statBuf.st_size + 1, path);
  }

  int nb = read(fd, fileBuf, statBuf.st_size);
  if (nb != statBuf.st_size)
  {
    free(fileBuf);
    close(fd);

    if (nb == -1)
      COR_LIB_RE(3, "read(%s): %s", path, strerror(errno));
    else
      COR_LIB_RE(4, "%s: read %d bytes, expected %ld", path, nb, (long) statBuf.st_size);
  }

  close(fd);

  // Zero terminate the buffer
  fileBuf[statBuf.st_size] = 0;

  *bufP    = fileBuf;
  *bufLenP = statBuf.st_size;

  return 0;
}
