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

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

//FIX: need to generate this automatically

#include <cstddef.h> // get size_t

namespace Std {
#if defined (__CYGWIN__) || defined (__CYGWIN32__)
/* cygwin */
typedef unsigned long   	clock_t;
typedef unsigned long   	clockid_t;
typedef	short			dev_t;
typedef	unsigned short		gid_t;
typedef	unsigned long		ino_t;
typedef int 			mode_t;
typedef unsigned short  	nlink_t;
typedef	long			off_t;
typedef int 			pid_t;
typedef long  			ssize_t;
typedef long  			time_t;
typedef unsigned long   	timer_t;
typedef	unsigned short		uid_t;
#else
/* linux */
typedef long int 		blkcnt_t;
typedef int                     clockid_t;
typedef unsigned long long int  dev_t;
typedef unsigned long 		fsblkcnt_t;
typedef unsigned long 		fsfilcnt_t;
typedef unsigned int 		gid_t;
typedef unsigned int 		id_t;
typedef unsigned long 		ino_t;
typedef unsigned int 		mode_t;
typedef unsigned int 		nlink_t;
typedef long int 		off_t;
typedef int 			pid_t;
typedef int 			ssize_t;
typedef long int 		time_t;
typedef int   			timer_t;
typedef unsigned int 		uid_t;
#endif
}
#endif
