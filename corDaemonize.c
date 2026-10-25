//
// FILE            corDaemonize.c
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#include <stdio.h>                                    // fflush
#include <unistd.h>                                   // fork, setsid, chdir, dup2, close, _exit
#include <fcntl.h>                                    // open, O_RDWR

#include "corBase/corDaemonize.h"                     // Own interface



// -----------------------------------------------------------------------------
//
// corDaemonize - detach the process from its terminal: fork, setsid, chdir /, 0/1/2 on /dev/null
//
int corDaemonize(void)
{
  fflush(NULL);                                       // else the child inherits the buffers and writes them again

  pid_t pid = fork();

  if (pid < 0)
    return -1;

  if (pid > 0)
    _exit(0);                                         // the parent - _exit: the atexit handlers are the child's

  if (setsid() < 0)
    return -1;

  if (chdir("/") != 0)
    return -1;

  int fd = open("/dev/null", O_RDWR);

  if (fd < 0)
    return -1;

  int rc = ((dup2(fd, 0) < 0) || (dup2(fd, 1) < 0) || (dup2(fd, 2) < 0)) ? -1 : 0;

  if (fd > 2)
    close(fd);

  return rc;
}
