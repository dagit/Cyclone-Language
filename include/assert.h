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

/* Originally derived from the GNU C Library, released under LGPL:
   Copyright (C) 1991,1992,1994-1999,2000,2001 Free Software Foundation, Inc. */
#ifndef _ASSERT_H_
#define _ASSERT_H_

#undef assert
#ifdef NDEBUG
#define assert(test) (0)
#else
#include <core.h>
extern `a
__assert_fail (string_t assertion, string_t file, unsigned int line) 
  __attribute__((noreturn));

#ifndef __FILE2__
#ifdef __BASE_FILE__
#define __FILE2__ __BASE_FILE__
#else
#define __FILE2__ __FILE__
#endif
#endif

#define __STRING(x) #x
#define assert(expr) ((expr) ? 0 : \
  (__assert_fail(__STRING(expr), __FILE2__, __LINE__)))

#endif
#endif
