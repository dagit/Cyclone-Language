/* This file is part of the Cyclone Library.
   Copyright (C) 2001 AT&T

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

#include <stdlib.h>

#ifdef CYC_REGION_PROFILE
#undef GC_malloc
#undef GC_malloc_atomic
#undef GC_free
#endif

void GC_free(void *x) {
  free(x);
}

void *GC_malloc(int x) {
  // FIX:  I'm calling calloc to ensure the memory is zero'd.  This
  // is because I had to define GC_calloc in runtime_cyc.c
  return (void*)calloc(sizeof(char),x);
}

void *GC_malloc_atomic(int x) {
  // FIX:  I'm calling calloc to ensure the memory is zero'd.  This
  // is because I had to define GC_calloc in runtime_cyc.c
  return (void*)calloc(sizeof(char),x);
}

unsigned int GC_gc_no = 0;

size_t GC_get_heap_size() {
  return 0;
}

size_t GC_get_free_bytes() {
  return 0;
}

size_t GC_get_total_bytes() {
  return 0;
}

void *GC_realloc(void *x, size_t n) {
  return (void *)realloc(x,n);
}

/* These type/macro defns are taken from gc/include/gc.h and must
   be kept in sync. */
#   define GC_PROTO(args) args
typedef unsigned long GC_word;
typedef void (*GC_warn_proc) GC_PROTO((char *msg, GC_word arg));

void GC_default_warn_proc(char *msg, GC_word arg) {
  return;
}
GC_warn_proc GC_set_warn_proc(GC_warn_proc p) {
  return GC_default_warn_proc;
}
