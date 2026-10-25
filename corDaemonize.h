#ifndef CORBASE_CORDAEMONIZE_H_
#define CORBASE_CORDAEMONIZE_H_

//
// FILE            corDaemonize.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//



// -----------------------------------------------------------------------------
//
// corDaemonize - detach the process from its terminal: the classic daemonize
//
// fork - the parent exits (0), the child goes on; setsid - a session of its own, no controlling
// terminal; chdir("/") - no directory kept busy; stdin, stdout and stderr on /dev/null.
//
// Returns 0 in the detached process, -1 if a step failed (errno says why): a failed fork returns in
// the original process, a later failure in the child. The parent does not return.
//
// What the caller has to know:
// - call it before the first thread is created: a fork copies only the calling thread;
// - a POSIX record lock (fcntl F_SETLK) is not inherited by the child - take such locks after it;
// - a relative path is relative to / afterwards;
// - pending stdio output is flushed first, so it is written once, by the parent;
// - the process id changes: getpid() after the call is the daemon's.
//
extern int corDaemonize(void);

#endif  // CORBASE_CORDAEMONIZE_H_
