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

// This is a Cyclone include file, but the actual definitions are
// made in C code.  So, this file is used to automatically produce
// a C include file, for use with the C definitions.

#ifndef _PRECORE_H_
#define _PRECORE_H_

#ifndef _CYC_GENERATE_PRECORE_C_
#include <sys/types.h>
#endif 

typedef char *{0} Cstring;
typedef const char ? string_t;
typedef char ? mstring_t;

typedef string_t @stringptr_t;
typedef mstring_t @mstringptr_t;

#ifndef bool
typedef int bool;
#endif
#ifndef false 
#define false (0)
#endif
#ifndef true
#define true (1)
#endif
extern "C" void exit(int) __attribute__((noreturn)) ;
extern "C" `a abort() __attribute__((noreturn));

#endif /* _PRECORE_H_ */
