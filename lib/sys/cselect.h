/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

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

#ifndef _SYS_SELECT_H_
#define _SYS_SELECT_H_

namespace Std {
#ifndef ___time_t_def_
#define ___time_t_def_
typedef int __time_t;
#endif
#ifndef ___suseconds_t_def_
#define ___suseconds_t_def_
typedef int __suseconds_t;
#endif
#ifndef ___sigset_t_def_
#define ___sigset_t_def_
typedef struct {unsigned int __val[1024 / (8 * 2)];} __sigset_t;
#endif
#ifndef _sigset_t_def_
#define _sigset_t_def_
typedef __sigset_t sigset_t;
#endif
#ifndef _timespec_def_
#define _timespec_def_
struct timespec {
  __time_t tv_sec;
  int tv_nsec;
};
#endif
#ifndef _timeval_def_
#define _timeval_def_
struct timeval {
  __time_t tv_sec;
  __suseconds_t tv_usec;
};
#endif
#ifndef _suseconds_t_def_
#define _suseconds_t_def_
typedef __suseconds_t suseconds_t;
#endif
#ifndef ___fd_mask_def_
#define ___fd_mask_def_
typedef int __fd_mask;
#endif
#ifdef __linux__
#ifndef _fd_set_def_
#define _fd_set_def_
typedef struct __fd_set {__fd_mask __fds_bits[1024 / (8 * 4)];} fd_set;
#endif
#endif
#ifndef FD_SETSIZE
#define FD_SETSIZE __FD_SETSIZE
#endif
#ifndef __FD_SETSIZE
#define __FD_SETSIZE 1024
#endif

  extern "C" int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);

// FIX: lifted from bits/select.h -- non-asm versions of fdset macros -- incorporate these changes into buildlib!!
#ifdef __linux__
#define __NFDBITS       (8 * sizeof (__fd_mask))
#define __FDELT(d)      ((d) / __NFDBITS)
#define __FDMASK(d)     ((__fd_mask) 1 << ((d) % __NFDBITS))
# define __FDS_BITS(set) ((set)->__fds_bits)
/* We don't use `memset' because this would require a prototype and
   the array isn't too big.  */
# define __FD_ZERO(set)  \
  do {                                                                        \
    unsigned int __i;                                                         \
    fd_set *__arr = (set);                                                    \
    for (__i = 0; __i < sizeof (fd_set) / sizeof (__fd_mask); ++__i)          \
      __FDS_BITS (__arr)[__i] = 0;                                            \
  } while (0)
# define __FD_SET(d, set)    (__FDS_BITS (set)[__FDELT (d)] |= __FDMASK (d))
# define __FD_CLR(d, set)    (__FDS_BITS (set)[__FDELT (d)] &= ~__FDMASK (d))
# define __FD_ISSET(d, set)  (__FDS_BITS (set)[__FDELT (d)] & __FDMASK (d))

/* Access macros for `fd_set'.  */
#define FD_SET(fd, fdsetp)      __FD_SET (fd, fdsetp)
#define FD_CLR(fd, fdsetp)      __FD_CLR (fd, fdsetp)
#define FD_ISSET(fd, fdsetp)    __FD_ISSET (fd, fdsetp)
#define FD_ZERO(fdsetp)         __FD_ZERO (fdsetp)
#endif

}
#endif
