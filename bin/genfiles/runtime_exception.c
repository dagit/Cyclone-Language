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

#include "runtime_internal.h"

// FIX: makes alignment and pointer-size assumptions
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
#ifdef HAVE_THREADS
static tlocal_key_t _exn_thrown_key;
static tlocal_key_t _exn_filename_key;
static tlocal_key_t _exn_lineno_key;
#else
static const char *_exn_filename = "?";
static int _exn_lineno = 0; // MWH: should be unsigned? Must match core.h
static struct _xtunion_struct *_exn_thrown = NULL;
#endif

void _init_exceptions() {
#ifdef HAVE_THREADS
  int status;
  //  fprintf(stderr, "Initing keys ... \n");
  if((status = create_tlocal_key(&_exn_thrown_key, NULL)))
    errquit("Thread local storage key creation failed (%d)", status);
  if((status = create_tlocal_key(&_exn_filename_key, NULL)))
    errquit("Thread local storage key creation failed (%d)", status);
  if((status = create_tlocal_key(&_exn_lineno_key, NULL)))
    errquit("Thread local storage key creation failed (%d)", status);
#endif
}

// create a new handler, put it on the stack, and return it so its
// jmp_buf can be filled in by the caller
void _push_handler(struct _handler_cons * new_handler) {
  //errprintf("pushing handler %x\n",(unsigned int)new_handler);  
  new_handler->s.tag = 0;
  new_handler->s.cleanup = NULL;
  _push_frame((struct _RuntimeStack *)new_handler);
}

// set _current_handler to its n+1'th tail
// Invariant: result is non-null
void _npop_handler(int n) { _npop_frame(n); }

void _pop_handler() {
/*   if (_top_frame() == NULL || _top_frame()->tag != 0) { */
/*     errquit("internal error: _pop_handler"); */
/*   } */
  _npop_handler(0);
}

// FIX: would be better to support a real closure here
static int (*uncaught_fun)() = NULL;
struct _handler_cons top_handler;
static int in_uncaught_fun = 0; // avoid infinite exception chain

struct _xtunion_struct* Cyc_Core_get_exn_thrown() {
#ifdef HAVE_THREADS
  return (struct _xtunion_struct*)get_tlocal(_exn_thrown_key);
#else
  return _exn_thrown;
#endif
}

const char *Cyc_Core_get_exn_filename() {
#ifdef HAVE_THREADS
  return (const char*)get_tlocal(_exn_filename_key);
#else
  return _exn_filename;
#endif
}
int Cyc_Core_get_exn_lineno() {
#ifdef HAVE_THREADS
  return (int)get_tlocal(_exn_lineno_key);
#else
  return _exn_lineno;
#endif
}
const char *Cyc_Core_get_exn_name(struct _xtunion_struct *x) {
  return x->tag;
}

void Cyc_Core_set_uncaught_exn_fun(int f()) {
  uncaught_fun = f;
}

#define SET_HANDLER(_handler)						\
  int status = 0;							\
  if (setjmp(_handler->handler)) status = 1;				\
  if (status) {								\
    char *exn_name;							\
    struct _xtunion_struct *exn_thrown = Cyc_Core_get_exn_thrown();     \
    const char *exn_filename = Cyc_Core_get_exn_filename();             \
    int exn_lineno = Cyc_Core_get_exn_lineno();                         \
    exn_name = exn_thrown->tag;						\
    errprintf("Uncaught exception %s thrown from around %s:%d\n",exn_name,\
	      exn_filename,exn_lineno);					  \
    return exn_name;							  \
  }									  \
  _push_handler(_handler);						  \
  return NULL;


// Used by C code to call Cyclone code
char *_set_catchall_handler(struct _handler_cons *handler) {
  SET_HANDLER(handler)
}

// Called by main to set the topmost exception handler
char *_set_top_handler() {
  SET_HANDLER((&top_handler))
}

void* _throw_fn(void* e, const char *filename, unsigned lineno) {
  // FIX: use struct _xtunion_struct *  ??
  struct _handler_cons *my_handler;
  // pop handlers until exception handler found
  my_handler = (struct _handler_cons *)_pop_frame_until(0);
  _npop_frame(0);
#ifdef HAVE_THREADS
  put_tlocal(_exn_thrown_key, e);
  put_tlocal(_exn_filename_key, filename);
  put_tlocal(_exn_lineno_key, (void *)lineno);
#else
  _exn_thrown = e; 
  _exn_filename = filename;
  _exn_lineno = lineno; 
#endif
  if (my_handler->handler == top_handler.handler && !in_uncaught_fun) {
    in_uncaught_fun = 1;
    if(uncaught_fun)
      uncaught_fun();
  }
  longjmp(my_handler->handler,1);
  return NULL; // unreached
}

/* re-throw an exception, but keep the filename and lineno info */
void* _rethrow(void *e) {
  _throw_fn(e, Cyc_Core_get_exn_filename(), Cyc_Core_get_exn_lineno());
  return NULL; // unreached
}
void Cyc_Core_rethrow(void *e) {
  _rethrow(e);
}

#ifdef throw
#undef throw
#endif
void* throw(void *e) { return _throw_fn(e,"?",0); }

void* _throw_null_fn(const char *filename, unsigned lineno) {
  return _throw_fn(Cyc_Null_Exception_val,filename,lineno);
}
void* _throw_arraybounds_fn(const char *filename, unsigned lineno) {
  return _throw_fn(Cyc_Array_bounds_val,filename,lineno);
}
void* _throw_badalloc_fn(const char *filename, unsigned lineno) {
  return _throw_fn(Cyc_Bad_alloc_val,filename,lineno);
}
void* _throw_match_fn(const char *filename, unsigned lineno) {
  return _throw_fn(Cyc_Match_Exception_val,filename,lineno);
}
