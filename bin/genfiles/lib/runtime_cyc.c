// This is the C "runtime library" to be used with the output of the
// Cyclone to C translator

#include <stdio.h>
#include <stdarg.h>
#include "cyc_include.h"
#include <errno.h>
extern void exit(int);

//////////////////////////////////////////////////////////
// First, definitions for things declared in cyc_include.h
//////////////////////////////////////////////////////////

// FIX: makes alignment and pointer-size assumptions
char _Null_Exception_tag[] = "\0\0\0\0Null_Exception";
struct _xtunion_struct _Null_Exception_struct = { _Null_Exception_tag };
struct _xtunion_struct * Null_Exception = &_Null_Exception_struct;
char _Match_Exception_tag[] = "\0\0\0\0Match_Exception";
struct _xtunion_struct _Match_Exception_struct = { _Match_Exception_tag };
struct _xtunion_struct * Match_Exception = &_Match_Exception_struct;

#ifdef CYC_REGION_PROFILE
static FILE *alloc_log = NULL;
#endif;

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

void throw(void* e) { // FIX: use struct _xtunion_struct *  ??
  struct _handler_cons *my_handler;
  while (_current_handler->tag != 0)
    _pop_region();
  my_handler = (struct _handler_cons *)_current_handler;
  _pop_handler();
  _exn_thrown = e;
  longjmp(my_handler->handler,1);
}

void _throw(void* e) { // FIX: use struct _xtunion_struct *  ??
  throw(e);
}


// The C include file precore_c.h is produced (semi) automatically
// from the Cyclone include file precore.h.
#include "precore_c.h"

struct _tagged_string Cstring_to_string(Cstring s) {
  struct _tagged_string str;
  int sz=(s?strlen(s):0);
  str.base = (char *)GC_malloc_atomic(sz);
  str.curr = str.base;
  str.last_plus_one = str.base + sz;

  // Copy the string in case the C code frees it or mangles it
  while(--sz>=0)
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

FILE *Cyc_Stdio_stdin; 
FILE *Cyc_Stdio_stdout;
FILE *Cyc_Stdio_stderr;

///////////////////////////////////////////////
// Regions

#ifdef CYC_REGION_PROFILE
static int rgn_total_bytes = 0;
static int heap_total_bytes = 0;
static int heap_total_atomic_bytes = 0;
#endif CYC_REGION_PROFILE


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
  struct _tagged_string *curr;
  struct _tagged_string *base;
  struct _tagged_string *last_plus_one;
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
  struct _handler_cons top_handler;
  int status = 0;
#ifdef CYC_REGION_PROFILE
  alloc_log = fopen("alloc_profile.txt","w");
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
  Cyc_Stdio_stdin  = stdin;
  Cyc_Stdio_stdout = stdout;
  Cyc_Stdio_stderr = stderr;
  // convert command-line args to Cyclone strings
  {struct _tagged_argv args;
  int i, result;
  args.base = 
    (struct _tagged_string *)GC_malloc(argc*sizeof(struct _tagged_string));
  args.curr = args.base;
  args.last_plus_one = args.base + argc;
  for(i = 0; i < argc; ++i)
    args.curr[i] = Cstring_to_string(argv[i]);
  result = Cyc_main(argc, args);
#ifdef CYC_REGION_PROFILE
  fprintf(stderr,"rgn_total_bytes: %d\n",rgn_total_bytes);
  fprintf(stderr,"heap_total_bytes: %d\n",heap_total_bytes);
  fprintf(stderr,"heap_total_atomic_bytes: %d\n",heap_total_atomic_bytes);
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
    throw(Null_Exception);
  }
  p->next = r->curr;
#ifdef CYC_REGION_PROFILE
  p->total_bytes = sizeof(struct _RegionPage) + next_size;
  p->free_bytes = next_size;
#endif CYC_REGION_PROFILE
  r->curr = p;
  r->offset = ((char *)p) + sizeof(struct _RegionPage);
  r->last_plus_one = r->offset + next_size;
}

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
    throw(Null_Exception);
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
    fprintf(alloc_log,":%d\t%d\n",lineno,s);
  }
  return _region_malloc(r,s);
}

void * GC_profile_malloc(int n, char *file, int lineno) {
  heap_total_bytes += n;
  if (alloc_log != NULL) {
    fputs(file,alloc_log);
    fprintf(alloc_log,":%d\t%d\n",lineno,n);
  }
  return GC_malloc(n);
}

void * GC_profile_malloc_atomic(int n, char *file, int lineno) {
  heap_total_bytes += n;
  heap_total_atomic_bytes +=n;
  if (alloc_log != NULL) {
    fputs(file,alloc_log);
    fprintf(alloc_log,":%d\t%d\n",lineno,n);
  }
  return GC_malloc_atomic(n);
}

#endif
