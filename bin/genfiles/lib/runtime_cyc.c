// This is the C "runtime library" to be used with the output of the
// Cyclone to C translator

// FIX: for now we use malloc instead of GC_malloc

#include <stdio.h>
#include <stdarg.h>
#include "cyc_include.h"
#include <errno.h>

//////////////////////////////////////////////////////////
// First, definitions for things declared in cyc_include.h
//////////////////////////////////////////////////////////

char _Null_Exception_tag[15] = "Null_Exception";
struct _xtunion_struct _Null_Exception_struct = { _Null_Exception_tag };
exn Null_Exception = &_Null_Exception_struct;
char _Match_Exception_tag[16] = "Match_Exception";
struct _xtunion_struct _Match_Exception_struct = { _Match_Exception_tag };
exn Match_Exception = &_Match_Exception_struct;

struct _tagged_string xprintf(char *fmt, ...) {
  char my_buff[1];
  va_list argp;
  int len1;
  int len2;
  struct _tagged_string result;

  va_start(argp,fmt);
  len1 = vsnprintf(my_buff,1,fmt,argp); // how much space do we need
  va_end(argp);

  // Presumably the Cyclone typechecker rules this out, but check anyway
  if (len1 < 0) {
    fprintf(stderr,"internal error: encoding error in xprintf\n");
    exit(1);
  }

  // Careful: we need space for a trailing zero (???)
  result.base     = (char *)GC_malloc_atomic(len1+1);
  result.base[len1] = '\0';
  result.curr     = result.base;
  result.last_plus_one = result.base + (len1+1);

  va_start(argp,fmt);
  len2 = vsnprintf(result.curr,len1+1,fmt,argp);
  va_end(argp);

  if (len1 != len2) {
    fprintf(stderr, "internal error: encoding error in xprintf\n");
    exit(1);
  }
  return result;
}

// Exceptions

// Need one of these per thread (we don't have threads)
static struct _handler_cons *_current_handler = NULL;

// create a new handler, put it on the stack, and return it so its
// jmp_buf can be filled in by the caller
void _push_handler(struct _handler_cons * new_handler) {
  new_handler->tail = _current_handler;
  _current_handler  = new_handler;
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
    _current_handler = _current_handler->tail;
  }
}

void _pop_handler() {
  _npop_handler(0);
}
void throw(void* e) {
  struct _handler_cons * my_handler = _current_handler;
  _npop_handler(0);
  longjmp(my_handler->handler,(int)e);
}
void _throw(void* e) {
  throw(e);
}


////////////////////////////////////////////////////////////////
// The rest of the code is stuff declared in core.h or otherwise
// used in core.c
// It's taken from talc/runtime/stdlib.c
////////////////////////////////////////////////////////////////

// The C include file precore_c.h is produced (semi) automatically
// from the Cyclone include file precore.h.
#include "precore_c.h"

struct _tagged_string Cstring_to_string(Cstring s) {
  struct _tagged_string str;
  int sz=(s?strlen(s):0);
  str.base = (char *)GC_malloc_atomic(sz);
  str.curr = str.base;
  str.last_plus_one = str.base + sz;

  while(--sz>=0)
    // Copy the string in case the C code frees it or mangles it
    str.curr[sz]=s[sz];

  return str;
}

Cstring string_to_Cstring(string s) {
  int i;
  char *contents = s.curr;
  size_t sz = s.last_plus_one - s.curr;
  char *str;

  if (s.curr < s.base || s.curr >= s.last_plus_one)
    throw(Null_Exception);
  str = (char *)GC_malloc_atomic(sz+1);
  if (str == NULL) {
    fprintf(stderr,"internal error: out of memory in string_to_Cstring\n");
    exit(1);
  }

  for(i=0; i < sz; i++) str[i]=contents[i];
  str[sz]='\0';
  return str;
}

// FIX:  this isn't really needed since you can cast char[?] to char[]
Cstring underlying_Cstring(string s) {
  char *str=s.curr;
  // FIX:  should throw exception?  
  if (s.curr < s.base || s.curr >= s.last_plus_one)
    throw(Null_Exception);
  return str;
}

extern int system(Cstring);

static void check_fd(FILE *fd) {
  if(!fd) {
    fprintf(stderr,"Attempt to access null file descriptor.\n");
    exit(255);
  }
}
int Cyc_Stdio_file_string_read(FILE *fd, string dest, int dest_offset, 
			       int max_count) {
  char *new_curr = dest.curr + dest_offset;
  size_t sz = dest.last_plus_one - new_curr;
  if (new_curr < dest.base || new_curr >= dest.last_plus_one)
    throw(Null_Exception);
  check_fd(fd);
  if(dest_offset + max_count > sz) {
    fprintf(stderr,"Attempt to read off end of string.\n");
    exit(255);
  }
  return fread(new_curr, 1, max_count, fd);
}
int Cyc_Stdio_file_string_write(FILE *fd, string src, int src_offset, 
				int max_count) {
  size_t sz = src.last_plus_one - src.curr;
  char *new_curr = src.curr + src_offset;
  if (new_curr < src.base || new_curr >= src.last_plus_one)
    throw(Null_Exception);
  check_fd(fd);
  if(src_offset + max_count > sz) {
    fprintf(stderr,"Attempt to write off end of string.\n");
    exit(255);
  } 
  return fwrite(new_curr, 1, max_count, fd);
}
// FIX: Make more C-like.
int f_seek(FILE *fd, int offset) {
  check_fd(fd);
  if(fseek(fd, (long int) offset, SEEK_SET)) {
    fprintf(stderr, "file seek failed, offset %d.\n", offset);
    exit(255);
  }
  return 0;
}
// extern int fflush(FILE *); // supplied by stdio
// extern int fgetc(FILE *);  // supplied by stdio


/////////////////////////////////////
// THIS SECTION ONLY USED IN CORE.CYC

// extern FILE    *fopen            (Cstring,Cstring);  // supplied by stdio
/*
int f_close(FILE *fd) {
  if(!fd) {
    fprintf(stderr,"Attempt to close null file descriptor.\n");
    exit(255);
  }
  return fclose(fd);
}
*/
#include <stdio.h>

FILE *Cyc_Stdio_stdin; 
FILE *Cyc_Stdio_stdout;
FILE *Cyc_Stdio_stderr;

///////////////////////////////////////////////
// Regions

// exported in core.h
struct _RegionHandle *Cyc_Core_heap_region = NULL;

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

// allocate a new page and return a region handle for a new region.
struct _RegionHandle _new_region() {
  struct _RegionHandle r;
  struct _RegionPage *p = 
    (struct _RegionPage *)GC_malloc(sizeof(struct _RegionPage) + 
                                    default_region_page_size);
  if (p == NULL) {
    fprintf(stderr,"_new_region failure");
    throw(Null_Exception);
  }
  p->next = NULL;
  r.curr = p;
  r.offset = ((char *)p) + sizeof(struct _RegionPage);
  r.last_plus_one = r.offset + default_region_page_size;
  return r;
}

static void grow_region(struct _RegionHandle *r, unsigned int s) {
  struct _RegionPage *p;
  unsigned int prev_size, next_size;

  prev_size = r->last_plus_one - 
    ((char *)r->curr + sizeof(struct _RegionPage));
  next_size = prev_size * 2;

  if (next_size < s) 
    next_size = s + default_region_page_size;

  p = GC_malloc(sizeof(struct _RegionPage) + next_size);
  if (p == NULL) {
    fprintf(stderr,"grow_region failure");
    throw(Null_Exception);
  }
  p->next = r->curr;
  r->curr = p;
  r->offset = ((char *)p) + sizeof(struct _RegionPage);
  r->last_plus_one = r->offset + next_size;
}

#define MIN_ALIGNMENT (sizeof(double))

// allocate s bytes within region r
void * _region_malloc(struct _RegionHandle *r, unsigned int s) {
  void *result;
  // allocate in the heap
  if (r == NULL)
    return GC_malloc(s);
  // round s up to the nearest MIN_ALIGNMENT value
  s =  (s + MIN_ALIGNMENT - 1) & (~(MIN_ALIGNMENT - 1));
  if (s > (r->last_plus_one - r->offset))
    grow_region(r,s);
  result = (void *)r->offset;
  r->offset = r->offset + s;
  return result;
}

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
  struct _tagged_string *curr;
  struct _tagged_string *base;
  struct _tagged_string *last_plus_one;
};
extern int Cyc_main(int argc, struct _tagged_argv argv); 

int main(int argc, char **argv) {
  // install outermost exception handler
  struct _handler_cons top_handler;
  int status = setjmp(top_handler.handler);
  _push_handler(&top_handler);
  if(status) {
    fprintf(stderr,"Uncaught exception %s\n",(((exn)status)->tag)+4);
    return 1;
  }
  // set standard file descriptors
  Cyc_Stdio_stdin  = stdin;
  Cyc_Stdio_stdout = stdout;
  Cyc_Stdio_stderr = stderr;
  // convert command-line args to Cyclone strings
  {struct _tagged_argv args;
  int i;
  args.base = 
    (struct _tagged_string *)GC_malloc(argc*sizeof(struct _tagged_string));
  args.curr = args.base;
  args.last_plus_one = args.base + argc;
  for(i = 0; i < argc; ++i)
    args.curr[i] = Cstring_to_string(argv[i]);
  return Cyc_main(argc, args);
  }
}
