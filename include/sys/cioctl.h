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

#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

namespace Std {

//  extern tunion IoctlArg<`r::R> {
//    /* The remaining cases are for the argument following the type-varying
//       argument; they could be eliminated if we had per-arg injection. */
//    /* NOTE: ORDER MATTERS! on these inject things */
//    IO_nonblock(int @`r); // FIONBIO
//  };
//  typedef tunion `r IoctlArg<`r> IO<`r>;
//  extern "C" int ioctl(int fd, int cmd, ... inject IO);

#ifdef __CYGWIN__
#define FIONBIO 0x8004667e /* To be compatible with termiost version */
#define FIONREAD 0x4004667f
#elif defined(__linux__)
#define FIONREAD 0x541b
#endif

extern "C" int ioctl(int fd, int cmd, int @arg);

}

#endif
