/* This file is part of the Cyclone Library.
   Copyright (C) 2003 Greg Morrisett, Dan Grossman, AT&T

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

#ifndef _GC_IFC_H_
#define _GC_IFC_H_

#include <stddef.h>  /* for size_t */

/*** \subsection{\texttt{<gc_ifc.h>}} */
/*** The file \texttt{<gc_ifc.h>} is a limited interface to the
     garbage collector used to manage the heap.  These will be
     nonsensical when the program is linked with option -nogc.

*/

/* These definitions are made outside of any namespace, as they are
   implemented in C code; either the GC library or nogc.c.  If you add
   a definition here, be sure to add one to nogc.c as well! */

extern "C" unsigned int GC_gc_no;
/** The number of times the garbage collector has been run since the
    program began.  */
extern "C" size_t GC_get_heap_size(void);
/** Return the number of bytes in the heap.  Excludes collector
    private data structures.  Includes empty blocks and fragmentation
    loss.  Includes some pages that were allocated but never
    written. */
extern "C" size_t GC_get_free_bytes(void);
/** Return a lower bound on the number of free bytes in the heap. */
extern "C" size_t GC_get_total_bytes(void);
/** Return the total number of bytes allocated in this process. Never
    decreases. */
extern "C" void GC_set_max_heap_size(unsigned int sz);
/** Prevents the heap from growing beyond [sz], specified in bytes.
    If the footprint becomes larger than this, the allocation routines
    will throw the exception [Bad_alloc]. */
extern "C" int GC_dont_expand;
/** Dont expand heap unless explicitly requested or forced to. */
extern "C" int GC_use_entire_heap;
/** Causes the nonincremental collector to use the entire heap before
    collecting.  This sometimes results in more large block
    fragmentation, since very larg blocks will tend to get broken up
    during each GC cycle.  It is likely to result in a larger working
    set, but lower collection frequencies, and hence fewer
    instructions executed in the collector. */

#endif
