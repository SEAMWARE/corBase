//
// FILE            corUriPasswordMask.h
//
// AUTHOR          Ken Zangelin
//
// Copyright 2026 Seamware
//
// SPDX-License-Identifier: Apache-2.0
//
#ifndef CORBASE_CORURIPASSWORDMASK_H_
#define CORBASE_CORURIPASSWORDMASK_H_



// -----------------------------------------------------------------------------
//
// corUriPasswordMask - a URI with the password of its user information as "***", for a log line or an error
//
// "mongodb://user:s3cret@host:27017/?authSource=admin" -> "mongodb://user:***@host:27017/?authSource=admin".
// Without a scheme the user information is at the start: "u:p@proxy:3128" -> "u:***@proxy:3128". A URI
// without a password (no "user:...@") is copied as it is. Cut to bufSize - 1 bytes;
// NULL is "". Returns buf.
//
extern char* corUriPasswordMask(const char* uri, char* buf, int bufSize);

#endif  // CORBASE_CORURIPASSWORDMASK_H_
