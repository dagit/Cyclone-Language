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
//#include "cyc_include.h"
#include <errno.h>
extern void exit(int);

int *__errno(void) { return &errno; }

//////////////////////////////////////////////////////////
// First, definitions for things declared in cyc_include.h
//////////////////////////////////////////////////////////

// FIX: makes alignment and pointer-size assumptions
char Cyc_Null_Exception_tag[] = "\0\0\0\0Cyc_Null_Exception";
struct _xtunion_struct Cyc_Null_Exception_struct = { Cyc_Null_Exception_tag };
struct _xtunion_struct * Cyc_Null_Exception = &Cyc_Null_Exception_struct;
char Cyc_Match_Exception_tag[] = "\0\0\0\0Cyc_Match_Exception";
struct _xtunion_struct Cyc_Match_Exception_struct = { Cyc_Match_Exception_tag };
struct _xtunion_struct * Cyc_Match_Exception = &Cyc_Match_Exception_struct;

#ifdef CYC_REGION_PROFILE
static FILE *alloc_log = NULL;
extern unsigned int GC_gc_no;
extern size_t GC_get_heap_size();
extern size_t GC_get_free_bytes();
extern size_t GC_get_total_bytes();
#endif

// Need one of these per thread (we don't have threads)
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
      _free_region((struct _RegionHandle *)_current_handler);
      //} else {
      //fprintf(stderr,"popping handler %x\n",(unsigned int)_current_handler);
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

#ifndef __linux__
int backtrace(int *array, int size) { return 0; }
#endif

extern int Cyc_Execinfo_bt(void);
static struct _handler_cons top_handler;
void throw(void* e) { // FIX: use struct _xtunion_struct *  ??
  struct _handler_cons *my_handler;
  while (_current_handler->tag != 0)
    _pop_region();
  my_handler = (struct _handler_cons *)_current_handler;
  _pop_handler();
  _exn_thrown = e;
  if (my_handler->handler == top_handler.handler)
    Cyc_Execinfo_bt();
  longjmp(my_handler->handler,1);
}

int _throw(void* e) { // FIX: use struct _xtunion_struct *  ??
  throw(e);
}

int _throw_null() {
  throw(Cyc_Null_Exception);
}


// The C include file precore_c.h is produced (semi) automatically
// from the Cyclone include file precore.h.
#include "precore_c.h"

struct _tagged_arr wrap_Cstring_as_string(Cstring s, size_t len) {
  struct _tagged_arr str;
  if (s == NULL) {
    str.base = str.curr = str.last_plus_one = NULL;
  } else {
    str.base = str.curr = s;
    str.last_plus_one = str.base + len;
  }
  return str;
}

struct _tagged_arr Cstring_to_string(Cstring s) {
  struct _tagged_arr str;
  if (s == NULL) {
    str.base = str.curr = str.last_plus_one = NULL;
  }
  else {
    int sz = strlen(s)+1;
    str.base = (char *)GC_malloc_atomic(sz);
    if (str.base == NULL) {
      fprintf(stderr, "internal error: out of memory in Cstring_to_string\n");
      exit(1);
    }
    str.curr = str.base;
    str.last_plus_one = str.base + sz;

    // Copy the string in case the C code frees it or mangles it
    str.curr[--sz] = '\0';
    while(--sz>=0)
      str.curr[sz]=s[sz];
  }
  return str;
}

Cstring string_to_Cstring(struct _tagged_arr s) {
  int i;
  char *contents = s.curr;
  size_t sz = s.last_plus_one - s.curr;
  char *str;

  if (s.curr == NULL) return NULL;

  if (s.curr < s.base || s.curr >= s.last_plus_one)
    throw(Cyc_Null_Exception); // FIX: this should be a bounds error, not null exn
  str = (char *)GC_malloc_atomic(sz+1);
  if (str == NULL) {
    fprintf(stderr,"internal error: out of memory in string_to_Cstring\n");
    exit(1);
  }

  for(i=0; i < sz; i++) str[i]=contents[i];
  str[sz]='\0';
  return str;
}

// Copy a null-terminated list of Cstrings to a tagged,
// null-terminated list of strings.  (The return type is misleading.)
// can put back in after bootstrapping

struct _tagged_arr ntCsl_to_ntsl(Cstring *ntCsl) {
  int i, numstrs = 0;
  struct _tagged_arr result;
  Cstring *ptr;

  for (ptr = ntCsl; *ptr; numstrs++); // not safe!
  result.base = (char *)GC_malloc_atomic(numstrs+1);
  if (result.base == NULL) {
    fprintf(stderr, "internal error: out of memory in ntCsl_to_ntsl\n");
    exit(1);
  }
  result.curr = result.base;
  result.last_plus_one = (char*)(((string_t*)result.base) + numstrs+1);
  for (i = 0; i <= numstrs; i++)
    ((string_t*)result.base)[i] = Cstring_to_string(ntCsl[i]);
  return result;
}

// Convert a "@pointer to a null-terminated list of pointers" to a
// "?pointer to a null-terminated list of pointers".  We don't list
// this function in core.h because "pointers to what" might change.
struct _tagged_arr pntlp_toCyc(void **in) {
  struct _tagged_arr result;
  result.curr = result.base = result.last_plus_one = (char*)in;
  while (*(result.last_plus_one++));
  return result;
}

// FIX:  this isn't really needed since you can cast char[?] to char[]
Cstring underlying_Cstring(struct _tagged_arr s) {
  char *str=s.curr;

  if (s.curr == NULL) return NULL;

  // FIX:  should throw exception?  
  if (s.curr < s.base || s.curr >= s.last_plus_one)
    throw(Cyc_Null_Exception);
  return str;
}

struct Cyc_Stdio___sFILE {
  FILE *file; // Mirror any changes in stdio.cyc
} Cyc_Stdio_stdin_v, Cyc_Stdio_stdout_v, Cyc_Stdio_stderr_v,
  *Cyc_Stdio_stdin = &Cyc_Stdio_stdin_v,
  *Cyc_Stdio_stdout = &Cyc_Stdio_stdout_v,
  *Cyc_Stdio_stderr = &Cyc_Stdio_stderr_v;

FILE *_sfile_to_file(struct Cyc_Stdio___sFILE *sf) {
  if(!sf) {
    fprintf(stderr,"Attempt to access null file descriptor.\n");
    exit(255);
  }
  if(!sf->file)
    throw(Cyc_Null_Exception); // FIX:  should be more descriptive?
  return sf->file;
}
FILE *sfile_to_file(struct Cyc_Stdio___sFILE *sf) {
  if(!sf) {
    fprintf(stderr,"Attempt to access null file descriptor.\n");
    exit(255);
  }
  if(!sf->file)
    throw(Cyc_Null_Exception); // FIX:  should be more descriptive?
  return sf->file;
}

int Cyc_Stdio_file_string_read(struct Cyc_Stdio___sFILE *sf, 
                               struct _tagged_arr dest,
                               int dest_offset, int max_count) {
  unsigned char *new_curr = dest.curr + dest_offset;
  size_t sz = dest.last_plus_one - new_curr;
  FILE *fd = _sfile_to_file(sf);
  if (new_curr < dest.base || new_curr >= dest.last_plus_one)
    throw(Cyc_Null_Exception);
  if(dest_offset + max_count > sz) {
    fprintf(stderr,"Attempt to read off end of string.\n");
    exit(255);
  }
  return fread(new_curr, 1, max_count, fd);
}
int Cyc_Stdio_file_string_write(struct Cyc_Stdio___sFILE *sf, 
                                struct _tagged_arr src,
                                int src_offset, int max_count) {
  size_t sz = src.last_plus_one - src.curr;
  unsigned char *new_curr = src.curr + src_offset;
  FILE *fd = _sfile_to_file(sf);
  if (new_curr < src.base || new_curr >= src.last_plus_one)
    throw(Cyc_Null_Exception);
  if(src_offset + max_count > sz) {
    fprintf(stderr,"Attempt to write off end of string.\n");
    exit(255);
  } 
  return fwrite(new_curr, 1, max_count, fd);
}

///////////////////////////////////////////////
// Sockets

// This really doesn't belong here; we should change the makefiles so
// C code can live with the libraries and be linked only as necessary.

// This function takes a const xtunion Socket::sockaddr and returns a
// pointer to the body of the union.  We'd return the size of the body
// if we could, but that information isn't available at runtime.

#include <sys/socket.h>
struct sa_xtunion {
  unsigned char *tag;
  struct sockaddr sa;
};
struct sockaddr *sockaddr_to_Csockaddr(struct sa_xtunion *xtunionaddr) {
  return &xtunionaddr->sa;
}
extern unsigned char *Cyc_Inet_sockaddr_in;

// We handle Unix domain sockets properly here: the sa field is an
// argument, since it could be bigger (or smaller) than 16
struct sa_xtunion *Csockaddr_to_sockaddr(struct sockaddr *addr, int len) {
  // I hope structure alignment doesn't cause any problem here
  struct sa_xtunion *result = GC_malloc_atomic(sizeof(unsigned char*)+len);
  if (result == NULL) {
    fprintf(stderr, "internal error: out of memory in Csockaddr_to_sockaddr\n");
    exit(1);
  }
  memcpy(&result->sa, addr, len);
  switch (addr->sa_family) {
  case AF_INET: result->tag = Cyc_Inet_sockaddr_in; break;
  default:
    fprintf(stderr, "internal error: Csockaddr_to_sockaddr with unsupported socket type\n");
    exit(1);
  }
  return result;
}

///////////////////////////////////////////////
// File locking

// Goes with fcntl.h

int fcntl_with_arg(int fd, int cmd, long arg) {
  return fcntl(fd, cmd, arg);
}
// We call lock a void* so we don't have to #include anything else
int fcntl_with_lock(int fd, int cmd, void *lock) {
  return fcntl(fd, cmd, lock);
}

///////////////////////////////////////////////
// Signals (goes with signal.cyc)

extern void Cyc_Signal__SIG_DFL(int);
extern void Cyc_Signal__SIG_IGN(int);
extern void Cyc_Signal__SIG_ERR(int);
typedef void (*signal_t)(int);
extern signal_t signal(int, signal_t);

signal_t signal_func(int sig, signal_t p) {
  if (p == Cyc_Signal__SIG_DFL) p = ((void(*)(int)) 0);
  else if (p == Cyc_Signal__SIG_IGN) p = ((void(*)(int)) 1);
  else if (p == Cyc_Signal__SIG_ERR) p = ((void(*)(int))-1);

  p = signal(sig, p);

  if (p == (void(*)(int)) 0) p = Cyc_Signal__SIG_DFL;
  else if (p == (void(*)(int)) 1) p = Cyc_Signal__SIG_IGN;
  else if (p == (void(*)(int))-1) p = Cyc_Signal__SIG_ERR;
  return p;
}

///////////////////////////////////////////////
// Regions

#ifdef CYC_REGION_PROFILE
static int rgn_total_bytes = 0;
static int heap_total_bytes = 0;
static int heap_total_atomic_bytes = 0;
#endif


// exported in core.h
struct _RegionHandle *Cyc_Core_heap_region = NULL;

// defined below so profiling macros work
struct _RegionHandle _new_region();
static void grow_region(struct _RegionHandle *r, unsigned int s);

// minimum page size for a region
#define CYC_MIN_REGION_PAGE_SIZE 128

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

extern void _free_region(struct _RegionHandle *r) {
  struct _RegionPage *p = r->curr;
  while (p != NULL) {
    struct _RegionPage *n = p->next;
    GC_free(p);
    p = n;
  }
  r->curr = 0;
  r->offset = 0;
  r->last_plus_one = 0;
}

// argc is redundant
struct _tagged_argv { 
  struct _tagged_arr *curr;
  struct _tagged_arr *base;
  struct _tagged_arr *last_plus_one;
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

extern int Cyc_main(int argc, struct _tagged_argv argv);

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
    if(_exn_thrown->tag == 0)
      exn_name = (((char *)_exn_thrown)+4);
    else
      exn_name = _exn_thrown->tag + 4;
    fprintf(stderr,"Uncaught exception %s\n",exn_name);
    return 1;
  }
  // set standard file descriptors
  Cyc_Stdio_stdin->file = stdin;
  Cyc_Stdio_stdout->file = stdout;
  Cyc_Stdio_stderr->file = stderr;
  // convert command-line args to Cyclone strings
  {struct _tagged_argv args;
  int i, result;
  args.base = 
    (struct _tagged_arr *)GC_malloc(argc*sizeof(struct _tagged_arr));
  args.curr = args.base;
  args.last_plus_one = args.base + argc;
  for(i = 0; i < argc; ++i)
    args.curr[i] = Cstring_to_string(argv[i]);
  result = Cyc_main(argc, args);
#ifdef CYC_REGION_PROFILE
  fprintf(stderr,"rgn_total_bytes: %d\n",rgn_total_bytes);
  fprintf(stderr,"heap_total_bytes: %d\n",heap_total_bytes);
  fprintf(stderr,"heap_total_atomic_bytes: %d\n",heap_total_atomic_bytes);
  fprintf(stderr,"number of collections: %d\n",GC_gc_no);
  if (alloc_log != NULL)
    fclose(alloc_log);
#endif
  return result;
  }
}

#ifdef CYC_REGION_PROFILE
#undef _region_malloc
#undef GC_malloc
#undef GC_malloc_atomic
#endif

static void grow_region(struct _RegionHandle *r, unsigned int s) {
  struct _RegionPage *p;
  unsigned int prev_size, next_size;

  prev_size = r->last_plus_one - ((char *)r->curr + sizeof(struct _RegionPage));
  next_size = prev_size * 2;

  if (next_size < s) 
    next_size = s + default_region_page_size;

  p = GC_malloc(sizeof(struct _RegionPage) + next_size);
  if (p == NULL) {
    fprintf(stderr,"grow_region failure");
    throw(Cyc_Null_Exception);
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

// allocate s bytes within region r
void * _region_malloc(struct _RegionHandle *r, unsigned int s) {
  void *result;
  // allocate in the heap
  if (r == NULL) {
#ifdef CYC_REGION_PROFILE
    heap_total_bytes += s;
#endif
    return GC_malloc(s);
  }
  // round s up to the nearest MIN_ALIGNMENT value
  s =  (s + MIN_ALIGNMENT - 1) & (~(MIN_ALIGNMENT - 1));
  if (s > (r->last_plus_one - r->offset))
    grow_region(r,s);
  result = (void *)r->offset;
  r->offset = r->offset + s;
#ifdef CYC_REGION_PROFILE
  r->curr->free_bytes = r->curr->free_bytes - s;
  rgn_total_bytes += s;
#endif
  return result;
}


// allocate a new page and return a region handle for a new region.
struct _RegionHandle _new_region() {
  struct _RegionHandle r;
  struct _RegionPage *p;

  p = (struct _RegionPage *)GC_malloc(sizeof(struct _RegionPage) + 
                                      default_region_page_size);
  if (p == NULL) {
    fprintf(stderr,"_new_region failure");
    throw(Cyc_Null_Exception);
  }
  p->next = NULL;
#ifdef CYC_REGION_PROFILE
  p->total_bytes = sizeof(struct _RegionPage) + default_region_page_size;
  p->free_bytes = default_region_page_size;
#endif
  r.s.tag = 1;
  r.s.next = NULL;
  r.curr = p;
  r.offset = ((char *)p) + sizeof(struct _RegionPage);
  r.last_plus_one = r.offset + default_region_page_size;
  return r;
}



#ifdef CYC_REGION_PROFILE

void * _profile_region_malloc(struct _RegionHandle *r, unsigned int s,
                              char *file, int lineno) {
  if (alloc_log != NULL) {
    fputs(file,alloc_log);
    fprintf(alloc_log,":%d\t%d\t%d\t%d\t%d\n",lineno,s,GC_get_heap_size(),
            GC_get_free_bytes(),GC_get_total_bytes());
  }
  return _region_malloc(r,s);
}

void * _profile_GC_malloc(int n, char *file, int lineno) {
  heap_total_bytes += n;
  if (alloc_log != NULL) {
    fputs(file,alloc_log);
    fprintf(alloc_log,":%d\t%d\t%d\t%d\t%d\n",lineno,n,GC_get_heap_size(),
            GC_get_free_bytes(),GC_get_total_bytes());
  }
  return GC_malloc(n);
}

void * _profile_GC_malloc_atomic(int n, char *file, int lineno) {
  heap_total_bytes += n;
  heap_total_atomic_bytes +=n;
  if (alloc_log != NULL) {
    fputs(file,alloc_log);
    fprintf(alloc_log,":%d\t%d\t%d\t%d\t%d\n",lineno,n,GC_get_heap_size(),
            GC_get_free_bytes(),GC_get_total_bytes());
  }
  return GC_malloc_atomic(n);
}

#endif