/* This file is part of the Cyclone Library.
   Copyright (C) 2000-2001 Greg Morrisett

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

#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <sys/ctypes.h> // get size_t, off_t

namespace Std {

  // In C, MAP_FAILED is (void*)-1, but I have no idea why.  The user
  // can never request a map at (void*)0, since that is defined to
  // mean "I don't care where you map it".  And mmap() will never
  // decide to map anything there.  So it probably should have been
  // NULL all along.

#define MAP_FAILED NULL

  // Of course, there are safety problems with mmap'ing: don't try to
  // access the memory in a way that violates the protection you
  // requested.
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define PROT_NONE 0

  // MAP_FIXED is unsupported, so we don't define it.
#define MAP_SHARED 1
#define MAP_PRIVATE 2

  // The traditional first arg to mmap should be 0 anyway, so don't
  // even bother to pass it to us.
  extern char ?mmap(size_t length, int prot, int flags, int fd, off_t offset);

  // Is it unsafe to ask the user for the size?  I'd like to take it
  // from the char? type, but that could have been cast around...
  extern int munmap(char ?start, size_t length);
}

#endif
