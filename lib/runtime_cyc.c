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

// This is the C "runtime library" to be used with the output of the
// Cyclone to C translator

#include <stdio.h>
#include <string.h>
#include <setjmp.h>

// The C include file precore_c.h is produced (semi) automatically
// from the Cyclone include file core.h.  Note, it now includes
// the contents of cyc_include.h

/* RUNTIME_CYC defined to prevent including parts of precore_c.h 
   that might cause problems, particularly relating to region profiling */
#define RUNTIME_CYC
#include "precore_c.h"

/// UTILITY ROUTINES (move into own file, or delete ...)

/* struct _dyneither_ptr Cstring_to_string(Cstring s) { */
/*   struct _dyneither_ptr str; */
/*   if (s == NULL) { */
/*     str.base = str.curr = str.last_plus_one = NULL; */
/*   } */
/*   else { */
/*     int sz = strlen(s)+1; */
/*     str.curr = (char *)_cycalloc_atomic(sz); */
/*     if (str.curr == NULL)  */
/*       _throw_badalloc(); */
/*     str.base = str.curr; */
/*     str.last_plus_one = str.curr + sz; */

/*     // Copy the string in case the C code frees it or mangles it */
/*     str.curr[--sz] = '\0'; */
/*     while(--sz>=0) */
/*       str.curr[sz]=s[sz]; */
/*   } */
/*   return str; */
/* } */

// no longer copying the C string (see above if this is bogus)
static struct _dyneither_ptr Cstring_to_string(Cstring s) {
  struct _dyneither_ptr str;
  if (s == NULL) {
    str.base = str.curr = str.last_plus_one = NULL;
  }
  else {
    unsigned int sz = strlen(s)+1;
    str.curr = str.base = (unsigned char *)s;
    str.last_plus_one = str.curr + sz;
  }
  return str;
}

// argc is redundant
struct _tagged_argv { 
  struct _dyneither_ptr *base;
  struct _dyneither_ptr *curr;
  struct _dyneither_ptr *last_plus_one;
};

// some debugging routines
// ONLY WORKS on x86 with frame pointer -- dumps out a list of 
// return addresses which can then be "looked up" using the output
// of nm.  I suggest doing nm -a -A -n -l cyclone.exe > nmout.txt.
// You'll have to adjust the output addresses depending upon where
// the text segment gets placed.  This could be useful for debugging
// certain exceptions because you can then get a lot of context. 
// I plan to use it for doing some statistics collection.
void stack_trace() {
  unsigned int x = 0;
  unsigned int *ra = (&x)+2;
  unsigned int *fp = (&x)+1;
  unsigned int *old_fp = fp;

  fprintf(stderr,"stack trace: ");
  while (*ra != 0) {
    fprintf(stderr,"0x%x ",*ra);
    old_fp = fp;
    fp = (unsigned int *)*fp;
    ra = fp+1;
  }
  fprintf(stderr,"\n");
}  

// These are defined in cstubs.c
extern struct Cyc___cycFILE {
  FILE *file;
} *Cyc_stdin, *Cyc_stdout, *Cyc_stderr;

extern int Cyc_main(int argc, struct _dyneither_ptr argv);
extern int _set_top_handler(); // defined in runtime_exception.c
extern int _init_regions();    // defined in runtime_memory.c
extern int _fini_regions();    // defined in runtime_memory.c

int main(int argc, char **argv) {
  // initialize region system
  _init_regions();
  // install outermost exception handler
  if (_set_top_handler() != 0) return 1;
  // set standard file descriptors
  Cyc_stdin->file = stdin;
  Cyc_stdout->file = stdout;
  Cyc_stderr->file = stderr;
  // convert command-line args to Cyclone strings -- we add an extra
  // NULL to the end of the argv so that people can step through argv
  // until they hit NULL.  
  {struct _tagged_argv args;
  struct _dyneither_ptr args_p;
  int i, result;
  args.curr = 
    (struct _dyneither_ptr *)GC_malloc((argc+1)*sizeof(struct _dyneither_ptr));
  args.base = args.curr;
  args.last_plus_one = args.curr + argc + 1;
  for(i = 0; i < argc; ++i)
    args.curr[i] = Cstring_to_string(argv[i]);
  // plug in final NULL
  args.curr[argc].base = 0;
  args.curr[argc].curr = 0;
  args.curr[argc].last_plus_one = 0;
  args_p.curr = (unsigned char *)args.curr;
  args_p.base = args_p.curr;
  args_p.last_plus_one = (unsigned char *)args.last_plus_one;
  result = Cyc_main(argc, args_p);
  _fini_regions();
  return result;
  }
}
