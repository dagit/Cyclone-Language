// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
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

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// Need one of these per thread (we don't have threads)
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
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
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
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
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
#ifndef NO_CYC_NULL_CHECKS
  if(!arr.base) 
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
#endif
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
#ifndef NO_CYC_BOUNDS_CHECKS
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
#endif
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.curr<arr.base || arr.curr>=arr.last_plus_one) ? 0 : ((arr.last_plus_one - arr.curr) / elt_sz);
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
extern void * GC_calloc(unsigned int,unsigned int);
extern void * GC_calloc_atomic(unsigned int,unsigned int);

static inline void * _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc_atomic(n,s);
  if (!ans)
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
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
#endif
#endif
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern struct _tagged_arr
Cyc_Core_new_string( unsigned int); extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_Buffer_t; extern struct Cyc_Buffer_t*
Cyc_Buffer_create( unsigned int n); extern struct _tagged_arr Cyc_Buffer_contents(
struct Cyc_Buffer_t*); extern unsigned int Cyc_Buffer_length( struct Cyc_Buffer_t*);
extern void Cyc_Buffer_clear( struct Cyc_Buffer_t*); extern void Cyc_Buffer_reset(
struct Cyc_Buffer_t*); extern void Cyc_Buffer_add_char( struct Cyc_Buffer_t*,
unsigned char); extern void Cyc_Buffer_add_substring( struct Cyc_Buffer_t*,
struct _tagged_arr, int offset, int len); extern void Cyc_Buffer_add_string(
struct Cyc_Buffer_t*, struct _tagged_arr); extern void Cyc_Buffer_add_buffer(
struct Cyc_Buffer_t* buf_dest, struct Cyc_Buffer_t* buf_source); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned int Cyc_Std_strlen(
struct _tagged_arr s); extern struct _tagged_arr Cyc_Std_strncpy( struct
_tagged_arr, struct _tagged_arr, unsigned int); extern struct _tagged_arr Cyc_Std_zstrncpy(
struct _tagged_arr, struct _tagged_arr, unsigned int); extern struct _tagged_arr
Cyc_Std_substring( struct _tagged_arr, int ofs, unsigned int n); struct Cyc_Buffer_t{
struct _tagged_arr buffer; unsigned int position; unsigned int length; struct
_tagged_arr initial_buffer; } ; struct Cyc_Buffer_t* Cyc_Buffer_create(
unsigned int n){ if( n <  1){ n= 1;}{ struct _tagged_arr s= Cyc_Core_new_string(
n); return({ struct Cyc_Buffer_t* _temp0=( struct Cyc_Buffer_t*) _cycalloc(
sizeof( struct Cyc_Buffer_t)); _temp0->buffer= s; _temp0->position= 0; _temp0->length=
n; _temp0->initial_buffer= s; _temp0;});}} struct _tagged_arr Cyc_Buffer_contents(
struct Cyc_Buffer_t* b){ return Cyc_Std_substring(( struct _tagged_arr) b->buffer,
0, b->position);} unsigned int Cyc_Buffer_length( struct Cyc_Buffer_t* b){
return( unsigned int) b->position;} void Cyc_Buffer_clear( struct Cyc_Buffer_t*
b){ b->position= 0; return;} void Cyc_Buffer_reset( struct Cyc_Buffer_t* b){ b->position=
0; b->buffer= b->initial_buffer; b->length= _get_arr_size( b->buffer, sizeof(
unsigned char)); return;} static void Cyc_Buffer_resize( struct Cyc_Buffer_t* b,
unsigned int more){ unsigned int len= b->length; unsigned int new_len= len;
struct _tagged_arr new_buffer; while( b->position +  more >  new_len) { new_len=
2 *  new_len;} new_buffer= Cyc_Core_new_string( new_len); Cyc_Std_strncpy(
new_buffer,( struct _tagged_arr) b->buffer, b->position); b->buffer= new_buffer;
b->length= new_len; return;} void Cyc_Buffer_add_char( struct Cyc_Buffer_t* b,
unsigned char c){ int pos=( int) b->position; if( pos >=  b->length){ Cyc_Buffer_resize(
b, 1);}*(( unsigned char*) _check_unknown_subscript( b->buffer, sizeof(
unsigned char), pos))= c; b->position=( unsigned int)( pos +  1); return;} void
Cyc_Buffer_add_substring( struct Cyc_Buffer_t* b, struct _tagged_arr s, int
offset, int len){ if(( offset <  0? 1: len <  0)? 1: offset +  len > 
_get_arr_size( s, sizeof( unsigned char))){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp1=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp1[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp2; _temp2.tag= Cyc_Core_Invalid_argument;
_temp2.f1= _tag_arr("Buffer::add_substring", sizeof( unsigned char), 22u);
_temp2;}); _temp1;}));}{ int new_position=( int)( b->position +  len); if(
new_position >  b->length){ Cyc_Buffer_resize( b,( unsigned int) len);} Cyc_Std_zstrncpy(
_tagged_arr_plus( b->buffer, sizeof( unsigned char),( int) b->position),
_tagged_arr_plus( s, sizeof( unsigned char), offset),( unsigned int) len); b->position=(
unsigned int) new_position; return;}} void Cyc_Buffer_add_string( struct Cyc_Buffer_t*
b, struct _tagged_arr s){ int len=( int) Cyc_Std_strlen( s); int new_position=(
int)( b->position +  len); if( new_position >  b->length){ Cyc_Buffer_resize( b,(
unsigned int) len);} Cyc_Std_strncpy( _tagged_arr_plus( b->buffer, sizeof(
unsigned char),( int) b->position), s,( unsigned int) len); b->position=(
unsigned int) new_position; return;} void Cyc_Buffer_add_buffer( struct Cyc_Buffer_t*
b, struct Cyc_Buffer_t* bs){ Cyc_Buffer_add_substring( b,( struct _tagged_arr)
bs->buffer, 0,( int) bs->position); return;}
