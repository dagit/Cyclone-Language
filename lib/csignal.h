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

#ifndef _SIGNAL_H
#define _SIGNAL_H

#define SIG_DFL _SIG_DFL
#define SIG_IGN _SIG_IGN
#define SIG_ERR _SIG_ERR

#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGILL 4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGFPE 8
#define SIGSEGV 11
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGPROF 27
#define SIGWINCH 28
#define SIGCLD SIGCHLD
#define SIGPOLL SIGIO

  // The other signals vary between Cygwin and Linux, so we list them
  // here -- alphabetically so we can tell we define the same symbols
  // for both.  If we don't list a signal at all, it's because it's
  // not available on both platforms.

#ifdef __CYGWIN__
#define SIGBUS 10
#define SIGCHLD 20
#define SIGCONT 19
#define SIGIO 23
#define SIGSTOP 17
#define SIGSYS 12
#define SIGTSTP 18
#define SIGURG 16
#define SIGUSR1 30
#define SIGUSR2 31
#else
#define SIGBUS 7
#define SIGCHLD 17
#define SIGCONT 18
#define SIGIO 29
#define SIGSTOP 19
#define SIGSYS 31
#define SIGTSTP 20
#define SIGURG 23
#define SIGUSR1 10
#define SIGUSR2 12
#endif

#include <sys/ctypes.h> // for pid_t

namespace std {
  typedef int sig_atomic_t;

  typedef void (*__sighandler_t) (int;{});

  __sighandler_t signal(int sig, __sighandler_t func);
  extern "C" int raise(int sig);

  extern void _SIG_DFL(int;{});
  extern void _SIG_IGN(int;{});
  extern void _SIG_ERR(int;{});

  extern "C" int kill(pid_t, int);
}

#endif
