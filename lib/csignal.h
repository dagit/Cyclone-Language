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

#define _EXTRACT_SIGNALCONSTS
#include _PLATFORM_INCLUDES_
#undef _EXTRACT_SIGNALCONSTS

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