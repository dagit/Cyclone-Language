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

#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <sys/ctime.h> // for timeval

namespace Std {

#define RLIMIT_CPU 0
#define RLIMIT_FSIZE 1
#define RLIMIT_DATA 2
#define RLIMIT_STACK 3
#define RLIMIT_CORE 4
#define RLIMIT_RSS 5
#define RLIMIT_NPROC 6
#define RLIMIT_NOFILE 7
#define RLIMIT_MEMLOCK 8
#ifndef __USE_FILE_OFFSET64
# define RLIM_INFINITY ((unsigned long int)(~0UL))
#else
# define RLIM_INFINITY 0xffffffffffffffffuLL
#endif

  typedef unsigned long rlim_t;

  struct rlimit {
    rlim_t rlim_cur;
    rlim_t rlim_max;
  };

  struct rusage {
    struct timeval ru_utime;	/* user time used */
    struct timeval ru_stime;	/* system time used */
    long ru_maxrss;
    long ru_ixrss;               /* XXX: 0 */
    long ru_idrss;               /* XXX: sum of rm_asrss */
    long ru_isrss;               /* XXX: 0 */
    long ru_minflt;              /* any page faults not requiring I/O */
    long ru_majflt;              /* any page faults requiring I/O */
    long ru_nswap;               /* swaps */
    long ru_inblock;             /* block input operations */
    long ru_oublock;             /* block output operations */
    long ru_msgsnd;              /* messages sent */
    long ru_msgrcv;              /* messages received */
    long ru_nsignals;            /* signals received */
    long ru_nvcsw;               /* voluntary context switches */
    long ru_nivcsw;              /* involuntary " */
  #define ru_last         ru_nivcsw
  };

  extern "C" int getrlimit(int resource, struct rlimit @ rlim);
  extern "C" int setrlimit(int resource, const struct rlimit @ rlim);

  extern "C" int getrusage (int who, struct rusage @ rusage);

}

#endif
