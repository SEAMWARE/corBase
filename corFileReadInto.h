#ifndef CORBASE_CORFILEREADINTO_H_
#define CORBASE_CORFILEREADINTO_H_

//
// FILE            corFileReadInto.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//



// -----------------------------------------------------------------------------
//
// corFileReadInto - read a small file into the caller's buffer, NUL-terminated
//
// For the files of /proc and /sys above all: they report a size of 0 (or 4096) whatever they hold,
// so corFileRead, which sizes its buffer with stat(), gets nothing out of them. This reads until EOF
// or until the buffer is full (bufSize - 1 bytes, the rest dropped), allocates nothing and logs
// nothing - a missing file is an answer here, not an error (a cgroup level with no limit file).
//
// Returns the number of bytes read, or -1 if the file could not be opened or read.
//
extern int corFileReadInto(const char* path, char* buf, int bufSize);

#endif  // CORBASE_CORFILEREADINTO_H_
