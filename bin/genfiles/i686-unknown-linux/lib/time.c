// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in
// the file lib/runtime_cyc.c

#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#include <setjmp.h>

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
// should be size_t, but int is fine.
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

//// Tagged arrays
struct _tagged_arr { 
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};
struct _tagged_string {  // delete after bootstrapping
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// The runtime maintains a stack that contains either _handler_cons
// structs or _RegionHandle structs.  The tag is 0 for a handler_cons
// and 1 for a region handle.  
struct _RuntimeStack {
  int tag; // 0 for an exception handler, 1 for a region handle
  struct _RuntimeStack *next;
};

//// Regions
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned int total_bytes;
  unsigned int free_bytes;
#endif
  struct _RegionPage *next;
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

extern struct _RegionHandle _new_region(const char *);
//  extern struct _RegionHandle _new_region();
extern void * _region_malloc(struct _RegionHandle *, unsigned int);
extern void   _free_region(struct _RegionHandle *);

//// Exceptions 
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();

#ifndef _throw
extern int _throw_null();
extern int _throw_arraybounds();
extern int _throw_badalloc();
extern int _throw(void * e);
#endif

extern struct _xtunion_struct *_exn_thrown;

//// Built-in Exceptions
extern struct _xtunion_struct ADD_PREFIX(Null_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Null_Exception);
extern struct _xtunion_struct ADD_PREFIX(Array_bounds_struct);
extern struct _xtunion_struct * ADD_PREFIX(Array_bounds);
extern struct _xtunion_struct ADD_PREFIX(Match_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Match_Exception);
extern struct _xtunion_struct ADD_PREFIX(Bad_alloc_struct);
extern struct _xtunion_struct * ADD_PREFIX(Bad_alloc);

//// Built-in Run-time Checks and company
static inline 
void * _check_null(void * ptr) {
  if(!ptr)
    _throw_null();
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
  if(!ptr || index >= bound)
    _throw_null();
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
  if(index >= bound)
    _throw_arraybounds();
  return index;
}
static inline 
char * _check_unknown_subscript(struct _tagged_arr arr,
				unsigned elt_sz, unsigned index) {
  // caller casts first argument and result
  // multiplication looks inefficient, but C compiler has to insert it otherwise
  // by inlining, it should be able to avoid actual multiplication
  unsigned char * ans = arr.curr + elt_sz * index;
  // might be faster not to distinguish these two cases. definitely would be
  // smaller.
  if(!arr.base) 
    _throw_null();
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
  return ans;
}
static inline 
struct _tagged_arr _tag_arr(const void * curr, 
			    unsigned elt_sz, unsigned num_elts) {
  // beware the gcc bug, can happen with *temp = _tag_arr(...) in weird places!
  struct _tagged_arr ans;
  ans.base = (void *)curr;
  ans.curr = (void *)curr;
  ans.last_plus_one = ((char *)curr) + elt_sz * num_elts;
  return ans;
}
static inline
struct _tagged_arr * _init_tag_arr(struct _tagged_arr * arr_ptr, void * arr, 
				   unsigned elt_sz, unsigned num_elts) {
  // we use this to (hopefully) avoid the gcc bug
  arr_ptr->base = arr_ptr->curr = arr;
  arr_ptr->last_plus_one = ((char *)arr) + elt_sz * num_elts;
  return arr_ptr;
}

static inline
char * _untag_arr(struct _tagged_arr arr, unsigned elt_sz, unsigned num_elts) {
  // Note: if arr is "null" base and curr should both be null, so this
  //       is correct (caller checks for null if untagging to @ type)
  // base may not be null if you use t ? pointer subtraction to get 0 -- oh well
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.last_plus_one - arr.curr) / elt_sz;
}
static inline
struct _tagged_arr _tagged_arr_plus(struct _tagged_arr arr, unsigned elt_sz,
				    int change) {
  struct _tagged_arr ans = arr;
  ans.curr += ((int)elt_sz)*change;
  return ans;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus(struct _tagged_arr * arr_ptr,
					    unsigned elt_sz, int change) {
  arr_ptr->curr += ((int)elt_sz)*change;
  return *arr_ptr;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus_post(struct _tagged_arr * arr_ptr,
						 unsigned elt_sz, int change) {
  struct _tagged_arr ans = *arr_ptr;
  arr_ptr->curr += ((int)elt_sz)*change;
  return ans;
}
				  
//// Allocation
extern void * GC_malloc(int);
extern void * GC_malloc_atomic(int);

static inline void * _cycalloc(int n) {
  void * ans = GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned int x, unsigned int y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned int word_ans = (unsigned int)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						char *file,int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#endif

#endif
 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std_tm{
int tm_sec; int tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int
tm_wday; int tm_yday; int tm_isdst; } ; extern void tzset(); extern int time(
int* t); extern struct _tagged_arr Cyc_Std_asctime( const struct Cyc_Std_tm*
timeptr); extern struct _tagged_arr Cyc_Std_ctime( const int* timep); extern
struct Cyc_Std_tm* gmtime( const int* timep); extern struct Cyc_Std_tm*
localtime( const int* timep); extern unsigned int Cyc_Std_strftime( struct
_tagged_arr s, unsigned int maxsize, struct _tagged_arr fmt, const struct Cyc_Std_tm*
t); extern struct _tagged_arr Cyc_Std_asctime_r( const struct Cyc_Std_tm*,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_ctime_r( const int*,
struct _tagged_arr); extern int timezone; extern int daylight; struct Cyc_Std_timeval{
int tv_sec; int tv_usec; } ; struct Cyc_Std_timezone{ int tz_minuteswest; int
tz_dsttime; } ; extern int gettimeofday( struct Cyc_Std_timeval* __p, struct Cyc_Std_timezone*
__z); struct Cyc_Std_itimerval{ struct Cyc_Std_timeval it_interval; struct Cyc_Std_timeval
it_value; } ; extern int getitimer( int, struct Cyc_Std_itimerval*); extern int
setitimer( int, const struct Cyc_Std_itimerval*, struct Cyc_Std_itimerval*);
typedef struct { unsigned int __val[ 64u]; } Cyc_Std___sigset_t; typedef Cyc_Std___sigset_t
Cyc_Std_sigset_t; struct Cyc_Std_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_Std_timeval; struct Cyc_Std___fd_set{ int __fds_bits[ 32u]; } ; extern int
select( int, struct Cyc_Std___fd_set*, struct Cyc_Std___fd_set*, struct Cyc_Std___fd_set*,
struct Cyc_Std_timeval*); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; extern unsigned int Cyc_Std_strlen( struct _tagged_arr s);
extern struct _tagged_arr Cyc_Std_strcpy( struct _tagged_arr dest, struct
_tagged_arr src); extern unsigned char* asctime( const struct Cyc_Std_tm*
timeptr); extern unsigned char* ctime( const int* timep); extern unsigned int
strftime( unsigned char* s, unsigned int maxsize, unsigned char* fmt, const
struct Cyc_Std_tm* t); extern unsigned char* asctime_r( const struct Cyc_Std_tm*,
unsigned char*); extern unsigned char* ctime_r( const int*, unsigned char*);
struct _tagged_arr Cyc_Std_asctime( const struct Cyc_Std_tm* timeptr){ return
wrap_Cstring_as_string( asctime( timeptr), - 1);} struct _tagged_arr Cyc_Std_ctime(
const int* timep){ return wrap_Cstring_as_string( ctime( timep), - 1);}
unsigned int Cyc_Std_strftime( struct _tagged_arr s, unsigned int maxsize,
struct _tagged_arr fmt, const struct Cyc_Std_tm* t){ unsigned int m=
_get_arr_size( s, sizeof( unsigned char)) <  maxsize? _get_arr_size( s, sizeof(
unsigned char)): maxsize; return strftime( underlying_Cstring( s), m,
underlying_Cstring( fmt), t);} struct _tagged_arr Cyc_Std_asctime_r( const
struct Cyc_Std_tm* t, struct _tagged_arr s){ struct _tagged_arr _temp0=
wrap_Cstring_as_string( asctime( t), - 1); if( Cyc_Std_strlen(( struct
_tagged_arr) _temp0) +  1 >  _get_arr_size( s, sizeof( unsigned char))){( int)
_throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp1=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp1[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp2; _temp2.tag= Cyc_Core_Invalid_argument;
_temp2.f1= _tag_arr("Time::asctime_r: string too small", sizeof( unsigned char),
34u); _temp2;}); _temp1;}));} else{ Cyc_Std_strcpy( s,( struct _tagged_arr)
_temp0); return s;}} struct _tagged_arr Cyc_Std_ctime_r( const int* t, struct
_tagged_arr s){ struct _tagged_arr _temp3= wrap_Cstring_as_string( ctime( t), -
1); if( Cyc_Std_strlen(( struct _tagged_arr) _temp3) +  1 >  _get_arr_size( s,
sizeof( unsigned char))){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp4=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp4[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp5; _temp5.tag= Cyc_Core_Invalid_argument;
_temp5.f1= _tag_arr("Time::ctime_r: string too small", sizeof( unsigned char),
32u); _temp5;}); _temp4;}));} else{ Cyc_Std_strcpy( s,( struct _tagged_arr)
_temp3); return s;}}
