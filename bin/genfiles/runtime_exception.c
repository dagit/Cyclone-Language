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

/* This part of the runtime system implements exceptions. */

#include <stdio.h>
#include <setjmp.h>

// The C include file precore_c.h is produced (semi) automatically
// from the Cyclone include file core.h.  Note, it now includes
// the contents of cyc_include.h

/* RUNTIME_CYC defined to prevent including parts of precore_c.h 
   that might cause problems, particularly relating to region profiling */
#define RUNTIME_CYC
#include "precore_c.h"

extern void longjmp(jmp_buf,int); // NB precore_c.h defines jmp_buf
extern void exit(int);

// FIX: makes alignment and pointer-size assumptions
// FIX: what about -nocyc???
char Cyc_Null_Exception[] = "Cyc_Null_Exception";
struct _xtunion_struct Cyc_Null_Exception_struct = { Cyc_Null_Exception };
struct _xtunion_struct * Cyc_Null_Exception_val = &Cyc_Null_Exception_struct;
char Cyc_Array_bounds[] = "Cyc_Array_bounds";
struct _xtunion_struct Cyc_Array_bounds_struct = { Cyc_Array_bounds };
struct _xtunion_struct * Cyc_Array_bounds_val = &Cyc_Array_bounds_struct;
char Cyc_Match_Exception[] = "Cyc_Match_Exception";
struct _xtunion_struct Cyc_Match_Exception_struct = { Cyc_Match_Exception };
struct _xtunion_struct * Cyc_Match_Exception_val = &Cyc_Match_Exception_struct;
char Cyc_Bad_alloc[] = "Cyc_Bad_alloc";
struct _xtunion_struct Cyc_Bad_alloc_struct = { Cyc_Bad_alloc };
struct _xtunion_struct * Cyc_Bad_alloc_val = &Cyc_Bad_alloc_struct;

// The exception that was thrown
struct _xtunion_struct *_exn_thrown = NULL;

// create a new handler, put it on the stack, and return it so its
// jmp_buf can be filled in by the caller
void _push_handler(struct _handler_cons * new_handler) {
  //fprintf(stderr,"pushing handler %x\n",(unsigned int)new_handler);  
  new_handler->s.tag = 0;
  new_handler->s.cleanup = NULL;
  _push_frame((struct _RuntimeStack *)new_handler);
}

// set _current_handler to its n+1'th tail
// Invariant: result is non-null
void _npop_handler(int n) { _npop_frame(n); }

void _pop_handler() {
  if (_top_frame() == NULL || _top_frame()->tag != 0) {
    fprintf(stderr,"internal error: _pop_handler");
    exit(1);
  }
  _npop_handler(0);
}

extern int Cyc_Execinfo_bt(void);
static struct _handler_cons top_handler;
static int in_backtrace = 0; // avoid infinite exception chain
static const char *_exn_filename = "?";
static unsigned _exn_lineno = 0;

const char *Cyc_Core_get_exn_filename() {
  return _exn_filename;
}
int Cyc_Core_get_exn_lineno() {
  return _exn_lineno;
}
const char *Cyc_Core_get_exn_name(struct _xtunion_struct *x) {
  return x->tag;
}

// Called by main to set the topmost exception handler
int _set_top_handler() {
  int status = 0;
  if (setjmp(top_handler.handler)) status = 1;
  _push_handler(&top_handler);
  if (status) {
    char *exn_name;
    exn_name = _exn_thrown->tag;
    fprintf(stderr,"Uncaught exception %s thrown from around %s:%d\n",exn_name,
            _exn_filename,_exn_lineno);
    return 1;
  }
  return 0;
}

int _throw_fn(void* e, const char *filename, unsigned lineno) {
  // FIX: use struct _xtunion_struct *  ??
  struct _handler_cons *my_handler;
  // pop handlers until exception handler found
  my_handler = (struct _handler_cons *)_pop_frame_until(0);
  _npop_frame(0);
  _exn_thrown = e;
  _exn_filename = filename;
  _exn_lineno = lineno;
#ifdef __linux__
  /* bt only works in linux, and gives a circular dependence in os x,
     so we need to compile this conditionally */
  if (my_handler->handler == top_handler.handler && !in_backtrace) {
    in_backtrace = 1;
    Cyc_Execinfo_bt();
  }
#endif
  longjmp(my_handler->handler,1);
}

/* re-throw an exception, but keep the filename and lineno info */
int _rethrow(void *e) {
  _throw_fn(e, _exn_filename, _exn_lineno);
}
void Cyc_Core_rethrow(void *e) {
  _rethrow(e);
}

#ifdef throw
#undef throw
#endif
int throw(void *e) { _throw_fn(e,"?",0); }

int _throw_null_fn(const char *filename, unsigned lineno) {
  _throw_fn(Cyc_Null_Exception_val,filename,lineno);
}
int _throw_arraybounds_fn(const char *filename, unsigned lineno) {
  _throw_fn(Cyc_Array_bounds_val,filename,lineno);
}
int _throw_badalloc_fn(const char *filename, unsigned lineno) {
  _throw_fn(Cyc_Bad_alloc_val,filename,lineno);
}
int _throw_match_fn(const char *filename, unsigned lineno) {
  _throw_fn(Cyc_Match_Exception_val,filename,lineno);
}
