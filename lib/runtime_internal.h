/* This file is part of the Cyclone Library.
   Copyright (C) 2004 Dan Grossman, AT&T

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
#ifndef RUNTIME_INTERNAL_H
#define RUNTIME_INTERNAL_H

// only need things here that are not in cyc_include.h, which end up
// included because of the following:

// The C include file precore_c.h is produced (semi) automatically
// from the Cyclone include file core.h.  Note, it now includes
// the contents of cyc_include.h

#if defined(GEEKOS)

#include <geekos/setjmp.h>
#include <geekos/screen.h>  // for error printing
#include <geekos/kthread.h> // for Exit
#include <geekos/string.h> // for memset

// #include <limits.h> // for magic numbers
#define INT_MAX 0x7fffffff
#define bzero(_addr,_szb) memset(_addr,'\0',_szb)

// error printing functions
#define errprintf Print
#define errquit(arg...) { Print(arg); Exit(1); }

#else

#include <setjmp.h> // precore_c.h uses jmp_buf without defining it
#include <stdio.h>  // for error printing
#include <limits.h> // for magic numbers
#ifdef _HAVE_PTHREAD_
#include <pthread.h>
#endif

// error printing functions
#define errprintf(arg...) fprintf(stderr,##arg)
#define errquit(arg...) { fprintf(stderr,##arg); exit(1); }

#endif

#define MAX_ALLOC_SIZE INT_MAX

/* RUNTIME_CYC defined to prevent including parts of precore_c.h 
   that might cause problems, particularly relating to region profiling */
#define RUNTIME_CYC
#include "precore_c.h"

// pushes a frame on the stack
void _push_frame(struct _RuntimeStack *frame);

// pop N+1 frames from the stack (error if stack_size < N+1)
void _npop_frame(unsigned int n);

// returns top frame on the stack (NULL if stack is empty)
struct _RuntimeStack * _top_frame();

// pops off frames until a frame with the given tag is reached.  This
// frame is returned, or else NULL if none found.
struct _RuntimeStack * _pop_frame_until(int tag);

#endif
