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
#include <string.h> // for memcpy
#include <stdarg.h>
#include <signal.h>
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

//////////////////////////////////////////////////////////
// First, definitions for things declared in cyc_include.h
//////////////////////////////////////////////////////////

/* Dynamic regions:  When ref_count > 0, the region is opened.  When
 * handle is NULL, the region has been freed. */
struct _DynRegionHandle {
  int ref_count;
  struct _RegionHandle *handle;
  /* link to other dynamic regions occuring within the same region --
   * used so we can free sub-regions when an outer region is freed. */
  struct _DynRegionHandle *next;
};

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
char Cyc_Core_Free_Region[] = "Cyc_Core_Free_Region";
struct _xtunion_struct Cyc_Core_Free_Region_struct = { Cyc_Core_Free_Region };
struct _xtunion_struct * Cyc_Core_Free_Region_val = &Cyc_Core_Free_Region_struct;
char Cyc_Core_Open_Region[] = "Cyc_Core_Open_Region";
struct _xtunion_struct Cyc_Core_Open_Region_struct = { Cyc_Core_Open_Region };
struct _xtunion_struct * Cyc_Core_Open_Region_val = &Cyc_Core_Open_Region_struct;

#ifdef CYC_REGION_PROFILE
static FILE *alloc_log = NULL;
extern unsigned int GC_gc_no;
extern size_t GC_get_heap_size();
extern size_t GC_get_free_bytes();
extern size_t GC_get_total_bytes();
#endif

// FIX: I'm putting GC_calloc and GC_calloc_atomic in here as just
// calls to GC_malloc and GC_malloc_atomic respectively.  This will
// *not* work for the nogc option.
void *GC_calloc(unsigned int n, unsigned int t) {
  return (void *)GC_malloc(n*t);
}

void *GC_calloc_atomic(unsigned int n, unsigned int t) {
  unsigned int p = n*t;
  // the collector does not zero things when you call malloc atomic...
  void *res = GC_malloc_atomic(p);
  if (res == NULL) {
    fprintf(stderr,"GC_calloc_atomic failure");
    _throw_badalloc();
  }
  bzero(res,p);
  return res;
}

static struct _RuntimeStack *_current_handler = NULL;

// Need one per thread
struct _xtunion_struct *_exn_thrown = NULL;

// create a new handler, put it on the stack, and return it so its
// jmp_buf can be filled in by the caller
void _push_handler(struct _handler_cons * new_handler) {
  //fprintf(stderr,"pushing handler %x\n",(unsigned int)new_handler);  
  new_handler->s.tag = 0;
  new_handler->s.next = _current_handler;
  _current_handler  = (struct _RuntimeStack *)new_handler;
}

void _push_region(struct _RegionHandle * r) {
  //fprintf(stderr,"pushing region %x\n",(unsigned int)r);  
  r->s.tag = 1;
  r->s.next = _current_handler;
  _current_handler  = (struct _RuntimeStack *)r;
}

void _push_dynregion(struct _DynRegionFrame *d) {
  //fprintf(stderr,"pushing dynregion %x\n",(unsigned int)d);  
  d->s.tag = 2;
  d->s.next = _current_handler;
  _current_handler = (struct _RuntimeStack *)d;
}

// set _current_handler to it's n+1'th tail
// Invariant: result is non-null
void _npop_handler(int n) {
  if (n<0) {
    fprintf(stderr,
            "internal error: _npop_handler called with negative argument\n");
    exit(1);
  }
  for(; n>=0; n--) {
    if(_current_handler == NULL) {
      fprintf(stderr,"internal error: empty handler stack\n");
      exit(1);
    } 
    if (_current_handler->tag == 1) {
      //fprintf(stderr,"popping region %x\n",(unsigned int)_current_handler);
#ifdef CYC_REGION_PROFILE
      _profile_free_region((struct _RegionHandle *)_current_handler,
			   "bogus",0);
#else
      _free_region((struct _RegionHandle *)_current_handler);
#endif
      //} else {
      //fprintf(stderr,"popping handler %x\n",(unsigned int)_current_handler);
    } else if (_current_handler->tag == 2) {
      // fprintf(stderr,"popping dynregion %x\n",(unsigned int)_current_handler);
      struct _DynRegionFrame *f = (struct _DynRegionFrame *)_current_handler;
      f->x->ref_count--;
      if (f->x->ref_count < 0) {
        fprintf(stderr,"internal error: _npop_handler made dynamic region reference count negative");
        exit(1);
      }
    }
    _current_handler = _current_handler->next;
  }
}

void _pop_handler() {
  if (_current_handler == NULL || _current_handler->tag != 0) {
    fprintf(stderr,"internal error: _pop_handler");
    exit(1);
  }
  _npop_handler(0);
}
void _pop_region() {
  if (_current_handler == NULL || _current_handler->tag != 1) {
    fprintf(stderr,"internal error: _pop_region");
    exit(1);
  }
  _npop_handler(0);
}
void _pop_dynregion() {
  if (_current_handler == NULL || _current_handler->tag != 2) {
    fprintf(stderr,"internal error: _pop_dynregion");
    exit(1);
  }
  _npop_handler(0);
}

extern int Cyc_Execinfo_bt(void);
static struct _handler_cons top_handler;
static int in_backtrace = 0; // avoid infinite exception chain
void throw(void* e) { // FIX: use struct _xtunion_struct *  ??
  struct _handler_cons *my_handler;
  while (_current_handler->tag != 0) {
    if (_current_handler->tag == 1)
      _pop_region();
    else if (_current_handler->tag == 2) 
      _pop_dynregion();
  }
  my_handler = (struct _handler_cons *)_current_handler;
  _pop_handler();
  _exn_thrown = e;
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

int _throw(void* e) { // FIX: use struct _xtunion_struct *  ??
  throw(e);
}

int _throw_null() {
  throw(Cyc_Null_Exception_val);
}
int _throw_arraybounds() {
  throw(Cyc_Array_bounds_val);
}
int _throw_badalloc() {
  throw(Cyc_Bad_alloc_val);
}
int _throw_match() {
  throw(Cyc_Match_Exception_val);
}

struct _dyneither_ptr wrap_Cstring_as_string(Cstring s, size_t len) {
  struct _dyneither_ptr str;
  if (s == NULL) {
    str.base = str.curr = str.last_plus_one = NULL;
  } else {
    int slen = strlen(s)+1;
    if (len == -1)
      len = slen;
    else if (len > slen)
      _throw_arraybounds(); /* FIX: pick better exception */
    str.base = str.curr = s;
    str.last_plus_one = s + len;
  }
  return str;
}

// trusted---the length field is not verified to be correct
struct _dyneither_ptr wrap_Cbuffer_as_buffer(Cstring s, size_t len) {
  struct _dyneither_ptr str;
  if (s == NULL) {
    str.base = str.curr = str.last_plus_one = NULL;
  } else {
    str.base = str.curr = s;
    str.last_plus_one = s + len;
  }
  return str;
}

struct _dyneither_ptr Cstring_to_string(Cstring s) {
  struct _dyneither_ptr str;
  if (s == NULL) {
    str.base = str.curr = str.last_plus_one = NULL;
  }
  else {
    int sz = strlen(s)+1;
    str.curr = (char *)_cycalloc_atomic(sz);
    if (str.curr == NULL) 
      _throw_badalloc();
    str.base = str.curr;
    str.last_plus_one = str.curr + sz;

    // Copy the string in case the C code frees it or mangles it
    str.curr[--sz] = '\0';
    while(--sz>=0)
      str.curr[sz]=s[sz];
  }
  return str;
}

Cstring string_to_Cstring(struct _dyneither_ptr s) {
  int i;
  char *contents = s.curr;
  size_t sz = s.last_plus_one - s.curr;
  char *str;

  if (s.curr == NULL) return NULL;

  if (s.curr >= s.last_plus_one || s.curr < s.base)
    throw(Cyc_Null_Exception_val); // FIX: this should be a bounds error
  // check that there's a '\0' somewhere in the string -- if not,
  // throw an exception.
  for (str = s.last_plus_one-1; str >= contents; str--) {
    if (*str == '\0') return contents;
  }
  throw(Cyc_Null_Exception_val);
  /*
  // slow path -- have to copy the string to ensure it's null terminated
  str = (char *)GC_malloc_atomic(sz+1);
  if (str == NULL) 
    _throw_badalloc();

  for(i=0; i < sz; i++) str[i]=contents[i];
  str[sz]='\0';
  return str;
  */
}

// Copy a null-terminated list of Cstrings to a tagged,
// null-terminated list of strings.  (The return type is misleading.)
// can put back in after bootstrapping
/*
struct _tagged_arr ntCsl_to_ntsl(Cstring *ntCsl) {
  int i, numstrs = 0;
  struct _tagged_arr result;
  Cstring *ptr;

  for (ptr = ntCsl; *ptr;  ptr++,numstrs++); // not safe!
  result.base = (char *)_cycalloc_atomic(numstrs+1);
  if (result.base == NULL) 
    _throw_badalloc();

  result.curr = result.base;
  result.last_plus_one = (char*)(((string_t*)result.base) + numstrs+1);
  for (i = 0; i <= numstrs; i++)
    ((string_t*)result.base)[i] = Cstring_to_string(ntCsl[i]);
  return result;
}
*/
// Convert a "@pointer to a null-terminated list of pointers" to a
// "?pointer to a null-terminated list of pointers".  We don't list
// this function in core.h because "pointers to what" might change.
/*
struct _tagged_arr pntlp_toCyc(void **in) {
  struct _tagged_arr result;
  result.curr = result.base = result.last_plus_one = (char*)in;
  while (*(result.last_plus_one++));
  return result;
}
*/
// Returns the size of an array from the current pointer back to
// its starting point.  If the curr pointer = start pointer, or 
// the curr pointer is out of bounds, then this is 0.
unsigned int arr_prevsize(struct _dyneither_ptr arr,size_t elt_sz) {
  unsigned char *_get_arr_size_curr=arr.curr;
  unsigned char *_get_arr_size_base=arr.base;
  return 
    (_get_arr_size_curr < _get_arr_size_base ||
     _get_arr_size_curr >= arr.last_plus_one) ? 0 :
    ((_get_arr_size_curr - _get_arr_size_base) / (elt_sz));
}
/*
// FIX:  this isn't really needed since you can cast char[?] to char[]
Cstring underlying_Cstring(struct _tagged_arr s) {
  char *str=s.curr;

  if (s.curr == NULL) return NULL;

  // FIX:  should throw exception?  
  if (s.curr < s.base || s.curr >= s.last_plus_one)
    throw(Cyc_Null_Exception);
  return str;
}
*/
///////////////////////////////////////////////
// Regions

#ifdef CYC_REGION_PROFILE
static int rgn_total_bytes = 0;
static int heap_total_bytes = 0;
static int heap_total_atomic_bytes = 0;
static int unique_total_bytes = 0;
static int unique_freed_bytes = 0;
static int refcnt_total_bytes = 0;
static int refcnt_freed_bytes = 0;
#endif


// exported in core.h
struct _RegionHandle *Cyc_Core_heap_region = NULL;
struct _RegionHandle *Cyc_Core_unique_region = (struct _RegionHandle *)1;
struct _RegionHandle *Cyc_Core_refcnt_region = (struct _RegionHandle *)2;

/////// UNIQUE REGION //////////

// for freeing unique pointers; might want to make this "free"
// eventually (but currently this is set to a no-op in libc.cys).
// Note that this is not recursive; it assumes that programmer
// has freed nested pointers (otherwise will be grabbed by the GC).
void Cyc_Core_ufree(struct _dyneither_ptr ptr) {
  if (ptr.base == NULL) return; // eventually make not-null type
  else {
#ifdef CYC_REGION_PROFILE
    unique_freed_bytes += GC_size(ptr.base);
#endif
    GC_free(ptr.base);
    ptr.base = ptr.curr = ptr.last_plus_one = NULL; // not really necessary...
  }
}

/////// REFERENCE-COUNTED REGION //////////

static int *get_refcnt(struct _dyneither_ptr ptr) {
  if (ptr.base == NULL) return NULL;
  else {
    int *res = ptr.base - sizeof(int);
/*     fprintf(stderr,"getting count, refptr=%x, cnt=%x\n",ptr.base,res); */
    return res;
  }
}

struct _count_ret{
  int cnt;
  struct _dyneither_ptr f2;
};

struct _count_ret Cyc_Core_refptr_count(struct _dyneither_ptr ptr) {
  struct _count_ret res = { 0, ptr };
  int *cnt = get_refcnt(ptr);
  if (cnt != NULL) { res.cnt = *cnt; }
  return res;
}

struct _alias_ret{
  struct _dyneither_ptr f1;
  struct _dyneither_ptr f2;
};

struct _alias_ret Cyc_Core_alias_refptr(struct _dyneither_ptr ptr) {
  struct _alias_ret res = { ptr, ptr };
  int *cnt = get_refcnt(ptr);
  if (cnt != NULL) *cnt = *cnt + 1;
/*   fprintf(stderr,"refptr=%x, cnt=%x, updated *cnt=%d\n",ptr.base,cnt, */
/* 	  cnt != NULL ? *cnt : 0); */
  return res;
}

void Cyc_Core_drop_refptr(struct _dyneither_ptr ptr) {
  int *cnt = get_refcnt(ptr);
  if (cnt != NULL) {
/*     fprintf(stderr,"refptr=%x, cnt=%x, *cnt=%d\n",ptr.base,cnt,*cnt); */
    *cnt = *cnt - 1;
    if (*cnt == 0) { // no more references
/*       fprintf(stderr,"freeing refptr=%x\n",ptr.base); */
#ifdef CYC_REGION_PROFILE
      refcnt_freed_bytes += GC_size(ptr.base - sizeof(int));
#endif
      GC_free(ptr.base - sizeof(int));
      ptr.base = ptr.curr = ptr.last_plus_one = NULL; // not necessary...
    }
  }
}

/////////////////////////////////////////////////////////////////////

// defined below so profiling macros work
struct _RegionHandle _new_region(const char *);
//  struct _RegionHandle _new_region();
static void grow_region(struct _RegionHandle *r, unsigned int s);

// minimum page size for a region
#define CYC_MIN_REGION_PAGE_SIZE 480

// controls the default page size for a region
static size_t default_region_page_size = CYC_MIN_REGION_PAGE_SIZE;

// set the default region page size -- returns false and has no effect 
// if s is not at least CYC_MIN_REGION_PAGE_SIZE
bool Cyc_set_default_region_page_size(size_t s) {
  if (s < CYC_MIN_REGION_PAGE_SIZE) 
    return 0;
  default_region_page_size = s;
  return 1;
}

#define MIN_ALIGNMENT (sizeof(double))

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

int main(int argc, char **argv) {
  // install outermost exception handler
  int status = 0;
#ifdef CYC_REGION_PROFILE
  alloc_log = fopen("amon.out","w");
#endif
  if (setjmp(top_handler.handler)) status = 1;
  _push_handler(&top_handler);
  if (status) {
    char *exn_name;
    exn_name = _exn_thrown->tag;
    fprintf(stderr,"Uncaught exception %s\n",exn_name);
    return 1;
  }
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
#ifdef CYC_REGION_PROFILE
  fprintf(stderr,"rgn_total_bytes: %d\n",rgn_total_bytes);
  fprintf(stderr,"heap_total_bytes: %d\n",heap_total_bytes);
  fprintf(stderr,"heap_total_atomic_bytes: %d\n",heap_total_atomic_bytes);
  fprintf(stderr,"unique_total_bytes: %d\n",unique_total_bytes);
  fprintf(stderr,"unique_free_bytes: %d\n",unique_freed_bytes);
  fprintf(stderr,"refcnt_total_bytes: %d\n",refcnt_total_bytes);
  fprintf(stderr,"refcnt_free_bytes: %d\n",refcnt_freed_bytes);
  fprintf(stderr,"number of collections: %d\n",GC_gc_no);
  if (alloc_log != NULL)
    fclose(alloc_log);
#endif
  return result;
  }
}

static void grow_region(struct _RegionHandle *r, unsigned int s) {
  struct _RegionPage *p;
  unsigned int prev_size, next_size;

  prev_size = r->last_plus_one - ((char *)r->curr + sizeof(struct _RegionPage));
  next_size = prev_size * 2;

  if (next_size < s) 
    next_size = s + default_region_page_size;

  // Note, we call calloc here to ensure we get zero-filled pages
  p = GC_calloc(sizeof(struct _RegionPage) + next_size,1);
  if (!p) {
    fprintf(stderr,"grow_region failure");
    _throw_badalloc;
  }
  p->next = r->curr;
#ifdef CYC_REGION_PROFILE
  p->total_bytes = sizeof(struct _RegionPage) + next_size;
  p->free_bytes = next_size;
#endif
  r->curr = p;
  r->offset = ((char *)p) + sizeof(struct _RegionPage);
  r->last_plus_one = r->offset + next_size;
}

// better not get called with the heap or unique regions ...
static void _get_first_region_page(struct _RegionHandle *r, unsigned int s) {
  struct _RegionPage *p;
  unsigned int page_size = 
    default_region_page_size < s ? s : default_region_page_size;
  p = (struct _RegionPage *)GC_calloc(sizeof(struct _RegionPage) + 
                                      page_size,1);
#ifdef CYC_REGION_PROFILE
  p->total_bytes = sizeof(struct _RegionPage) + page_size;
  p->free_bytes = page_size;
#endif
  if (p == NULL) 
    _throw_badalloc();
  p->next = NULL;
  r->curr = p;
  r->offset = ((char *)p) + sizeof(struct _RegionPage);
  r->last_plus_one = r->offset + page_size;
}

// allocate s bytes within region r
void * _region_malloc(struct _RegionHandle *r, unsigned int s) {
  char *result;
  if (r == Cyc_Core_unique_region || r == Cyc_Core_heap_region) {
    result = GC_malloc(s);
    if(!result)
      _throw_badalloc;
#ifdef CYC_REGION_PROFILE
    if (r == Cyc_Core_heap_region)
      heap_total_bytes += s;
    else {
      unsigned int actual_size;
      actual_size = GC_size(result);
      unique_total_bytes += actual_size;
    }
#endif
    return (void *)result;
  }
  else if (r == Cyc_Core_refcnt_region) {
    // need to add a word for the reference count.  We use a word to
    // keep the resulting memory word-aligned.  Then bump the pointer.
    result = GC_malloc(s+sizeof(int));
    if(!result)
      _throw_badalloc;
    *(int *)result = 1;
    result += sizeof(int);
/*     fprintf(stderr,"alloc refptr=%x\n",result); */
#ifdef CYC_REGION_PROFILE
    {
      unsigned int actual_size;
      actual_size = GC_size(result);
      refcnt_total_bytes += actual_size;
    }
#endif
    return (void *)result;
  }
  else {
    // round s up to the nearest MIN_ALIGNMENT value
    s =  (s + MIN_ALIGNMENT - 1) & (~(MIN_ALIGNMENT - 1));
    // if no page yet, then fetch one
    if (r->curr == 0) {
      _get_first_region_page(r,s);
      result = r->offset;
    } else {
      result = r->offset;
      // else check for space on the current page
      if (s > (r->last_plus_one - result)) {
        grow_region(r,s);
        result = r->offset;
      }
    }
    r->offset = result + s;
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
    return (void *)result;
  }
}

// allocate s bytes within region r
void * _region_calloc(struct _RegionHandle *r, unsigned int n, unsigned int t) 
{
  unsigned int s = n*t;
  char *result;
  if (r == Cyc_Core_heap_region || r == Cyc_Core_unique_region) {
    // allocate in the heap
    result = GC_calloc(n,t);
    if(!result)
      _throw_badalloc;
#ifdef CYC_REGION_PROFILE
    if (r == Cyc_Core_heap_region)
      heap_total_bytes += s;
    else {
      unsigned int actual_size;
      actual_size = GC_size(result);
      unique_total_bytes += actual_size;
    }
#endif
    return result;
  } else if (r == Cyc_Core_refcnt_region) {
    // allocate in the heap + 1 word for the refcount
    result = GC_calloc(n*t+sizeof(int),1);
    if(!result)
      _throw_badalloc;
    *(int *)result = 1;
    result += sizeof(int);
#ifdef CYC_REGION_PROFILE
    {
      unsigned int actual_size;
      actual_size = GC_size(result);
      refcnt_total_bytes += actual_size;
    }
#endif
    return result;
  }
  else {
    // round s up to the nearest MIN_ALIGNMENT value
    s =  (s + MIN_ALIGNMENT - 1) & (~(MIN_ALIGNMENT - 1));
    // if no page yet, then fetch one
    if (r->curr == 0) {
      _get_first_region_page(r,s);
      result = r->offset;
    } else {
      result = r->offset;
      // else check for space on the current page
      if (s > (r->last_plus_one - result)) {
        grow_region(r,s);
        result = r->offset;
      }
    }
    r->offset = result + s;
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
    return (void *)result;
  }
}

/* Open a dynamic region -- throws an exception when the handle
 * is NULL or the region is already opened.
 */
struct _RegionHandle *_open_dynregion(struct _DynRegionFrame *f, 
                                      struct _DynRegionHandle *h) {
  struct _RegionHandle *res = h->handle;
  if (res == NULL)
    throw(Cyc_Core_Open_Region_val);
  h->ref_count++;
  f->x = h;
  _push_dynregion(f);
  return res;
}

/* Frees a dynamic region -- throws an exception when the region
 * has already been freed or is open.
 */
void Cyc_Core_free_dynregion(struct _DynRegionHandle* x) {
  if (x->ref_count != 0 || x->handle == NULL) 
    throw(Cyc_Core_Free_Region_val);
  _free_region(x->handle);
  x->handle = NULL;
}

/* Same as above, but returns 0 when it fails, 1 when it succeeds */
int Cyc_Core_try_free_dynregion(struct _DynRegionHandle *x) {
  if (x->ref_count != 0 || x->handle == NULL) 
    return 0;
  _free_region(x->handle);
  x->handle = NULL;
  return 1;
}

/* Allocate a new dynamic region in region r */
struct Core_NewRegion Cyc_Core__rnew_dynregion(struct _RegionHandle *r,
					       char *file, int lineno) {
  struct Core_NewRegion resrgn;
  struct _DynRegionHandle *res = 
    _region_malloc(r, sizeof(struct _DynRegionHandle));
  struct _RegionHandle *d = 
    _region_malloc(r, sizeof(struct _RegionHandle));
  res->handle = d;
  res->ref_count = 0;
  /* link this dynregion into r's list of sub-regions so we can 
   * free the dynregion when r is freed -- note that the Heap and
   * Unique regions end up generating free-floating sub-regions
   * that are reclaimed by the collector... */
  if (r != Cyc_Core_heap_region &&
      r != Cyc_Core_unique_region &&
      r != Cyc_Core_refcnt_region) {
    res->next = r->sub_regions;
    r->sub_regions = res;
  }
#ifdef CYC_REGION_PROFILE
  *d = _profile_new_region("dyn",file,lineno);
#else
  *d = _new_region(NULL);
#endif
  resrgn.dynregion = res;
  return resrgn;
}

// allocate a new page and return a region handle for a new region.
struct _RegionHandle _new_region(const char *rgn_name) {
  struct _RegionHandle r;

  /* we're now lazy about allocating a region page */
  //struct _RegionPage *p;

  // we use calloc to make sure the memory is zero'd
  //p = (struct _RegionPage *)GC_calloc(sizeof(struct _RegionPage) + 
  //                                  default_region_page_size,1);
  //if (p == NULL) 
  //  _throw_badalloc();
  //p->next = NULL;
#ifdef CYC_REGION_PROFILE
  //p->total_bytes = sizeof(struct _RegionPage) + default_region_page_size;
  //p->free_bytes = default_region_page_size;
  r.name = rgn_name;
#endif
  r.s.tag = 1;
  r.s.next = NULL;
  r.curr = 0;
  r.offset = 0;
  r.last_plus_one = 0;
  r.sub_regions = NULL;
  //r.offset = ((char *)p) + sizeof(struct _RegionPage);
  //r.last_plus_one = r.offset + default_region_page_size;
  return r;
}

// free all the resources associated with a region (except the handle)
//   (assumes r is not heap or unique region)
void _free_region(struct _RegionHandle *r) {
  struct _DynRegionHandle *d;
  struct _RegionPage *p;

  /* free sub regions */
  d = r->sub_regions;
  while (d != NULL) {
    Cyc_Core_try_free_dynregion(d);
    d = d->next;
  }
  /* free pages */
  p = r->curr;
  while (p != NULL) {
    struct _RegionPage *n = p->next;
    GC_free(p);
    p = n;
  }
  r->curr = 0;
  r->offset = 0;
  r->last_plus_one = 0;
}

// reset the region by freeing its pages and then reallocating a fresh page.
//   (assumes r is not heap or unique region)
void _reset_region(struct _RegionHandle *r) {
  _free_region(r);
#ifdef CYC_REGION_PROFILE  
  *r = _new_region(r->name);
#else
  *r = _new_region(NULL);
#endif
}


#ifdef CYC_REGION_PROFILE

static int region_get_heap_size(struct _RegionHandle *r) {
  if (r != Cyc_Core_heap_region &&
      r != Cyc_Core_unique_region &&
      r != Cyc_Core_refcnt_region) {
    struct _RegionPage *p = r->curr;
    int sz = 0;
    while (p != NULL) {
      sz += p->total_bytes;
      p = p->next;
    }  
    return sz;
  }
  else
    return GC_get_heap_size();
}

/* Two choices: print the "effective" free bytes, which are just
   the ones in the current page, or print the non-allocated bytes,
   which are the free bytes in all the pages.  Doing the former. */
static int region_get_free_bytes(struct _RegionHandle *r) {
  if (r != Cyc_Core_heap_region &&
      r != Cyc_Core_unique_region &&
      r != Cyc_Core_refcnt_region) {
    struct _RegionPage *p = r->curr;
    if (p != NULL)
      return p->free_bytes;
    else 
      return 0;
  }
  else
    return GC_get_free_bytes();
}

static int region_get_total_bytes(struct _RegionHandle *r) {
  if (r != Cyc_Core_heap_region &&
      r != Cyc_Core_unique_region &&
      r != Cyc_Core_refcnt_region) {
    struct _RegionPage *p = r->curr;
    int sz = 0;
    while (p != NULL) {
      sz = sz + (p->total_bytes - p->free_bytes);
      p = p->next;
    }  
    return sz;
  }
  else {
    unsigned int unique_avail_bytes =
      unique_total_bytes - unique_freed_bytes;
    unsigned int refcnt_avail_bytes =
      refcnt_total_bytes - refcnt_freed_bytes;
    if (r == Cyc_Core_unique_region) return unique_avail_bytes;
    else if (r == Cyc_Core_refcnt_region) return refcnt_avail_bytes;
    else return GC_get_total_bytes() - unique_avail_bytes - refcnt_avail_bytes;
  }
}

struct _RegionHandle _profile_new_region(const char *rgn_name, 
					 char *file, int lineno) {
  int len;
  char *buf;
  static int cnt = 0;

  len = strlen(rgn_name)+10;
  buf = GC_malloc_atomic(len);
  if(!buf)
    _throw_badalloc;
  else
    snprintf(buf,len,"%d-%s\n",cnt++,rgn_name);

  if (alloc_log != NULL) {
    fputs(file,alloc_log);
    fprintf(alloc_log,":%d\t%s\tcreate\n",lineno,buf);
  }

  return _new_region(buf);
}

void _profile_free_region(struct _RegionHandle *r, char *file, int lineno) {
  if (alloc_log != NULL) {
    fputs(file,alloc_log);
    fprintf(alloc_log,":%d\t%s\tfree\n",lineno,
	    (r == Cyc_Core_heap_region ? "heap" :
	     (r == Cyc_Core_unique_region ? "unique" :
	      (r == Cyc_Core_refcnt_region ? "refcnt" : r->name))));
  }
  _free_region(r);
}

void * _profile_region_malloc(struct _RegionHandle *r, unsigned int s,
                              char *file, int lineno) {
  if (alloc_log != NULL) {
    fputs(file,alloc_log);
    fprintf(alloc_log,":%d\t%s\t%d\t%d\t%d\t%d\n",lineno,
	    (r == Cyc_Core_heap_region ? "heap" :
	     (r == Cyc_Core_unique_region ? "unique" :
	      (r == Cyc_Core_refcnt_region ? "refcnt" : r->name))), s,
	    region_get_heap_size(r), 
	    region_get_free_bytes(r),
	    region_get_total_bytes(r));
  }
  return _region_malloc(r,s);
}

void * _profile_GC_malloc(int n, char *file, int lineno) {
  void * result;
  heap_total_bytes += n;
  if (alloc_log != NULL) {
    fputs(file,alloc_log);
    fprintf(alloc_log,":%d\theap\t%d\t%d\t%d\t%d\n",lineno,n,
	    GC_get_heap_size(),GC_get_free_bytes(),GC_get_total_bytes());
  }
  result =  GC_malloc(n);
  if(!result)
    _throw_badalloc();
  return result;
}

void * _profile_GC_malloc_atomic(int n, char *file, int lineno) {
  void * result;
  heap_total_bytes += n;
  heap_total_atomic_bytes +=n;
  if (alloc_log != NULL) {
    fputs(file,alloc_log);
    fprintf(alloc_log,":%d\theap\t%d\t%d\t%d\t%d\n",lineno,n,
	    GC_get_heap_size(),GC_get_free_bytes(),GC_get_total_bytes());
  }
  result =  GC_malloc_atomic(n);
  if(!result)
    _throw_badalloc();
  return result;
}

#endif

/******* for turning off gc warnings about blacklisted blocks *******/
/* These type/macro defns are taken from gc/include/gc.h and must
   be kept in sync. */
#   define GC_PROTO(args) args
typedef unsigned long GC_word;
typedef void (*GC_warn_proc) GC_PROTO((char *msg, GC_word arg));

extern void GC_default_warn_proc(char *msg, GC_word arg);
extern GC_warn_proc GC_set_warn_proc(GC_warn_proc p);

static void GC_noblacklist_warn_proc(char *msg, GC_word arg) {
  if (!msg) return;
  if (!strncmp(msg,"Needed to allocate blacklisted block",
               strlen("Needed to allocate blacklisted block")))
    return;
  GC_default_warn_proc(msg,arg);
}

/* Use these in Cyclone programs */
void GC_blacklist_warn_set() {
  GC_set_warn_proc(GC_default_warn_proc);
}
void GC_blacklist_warn_clear() {
  GC_set_warn_proc(GC_noblacklist_warn_proc);
}
