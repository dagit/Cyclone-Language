/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

/* Some constants taken from the GNU C Library:
   Copyright (C) 1991-1999, 2000, 2001 Free Software Foundation, Inc.
*/

#ifndef _FCNTL_H
#define _FCNTL_H

#include <sys/ctypes.h>
#include <core.h> // for string_t

#define O_RDONLY       00
#define O_WRONLY       01
#define O_RDWR         02
#define O_CREAT      0100
#define O_EXCL       0200
#define O_NOCTTY     0400
#define O_TRUNC     01000
#define O_APPEND    02000
#define O_NONBLOCK  04000
#define O_SYNC     010000

#define F_DUPFD 0
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4

namespace Std {

  struct flock {
    short l_type;
    off_t l_start;
    short l_whence;
    off_t l_len;
    pid_t l_pid;
  };

  tunion FcntlArg<`r::R> {
    Flock(struct flock @`r);
    Long(long);
  };
  typedef tunion `r FcntlArg<`r> fcntlarg_t<`r>;

  extern int fcntl(int fd, int cmd, ... inject fcntlarg_t);
  extern int open(string_t,int,... mode_t);
  extern int creat(string_t,mode_t);
}

#endif
