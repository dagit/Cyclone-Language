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
extern void * _region_calloc(struct _RegionHandle *, unsigned int t,
                             unsigned int n);
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
 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_Invalid_argument[
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
ntCsl_to_ntsl( unsigned char**); extern struct _tagged_arr Cyc_Filename_concat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_Filename_chop_extension(
struct _tagged_arr); extern struct _tagged_arr Cyc_Filename_dirname( struct
_tagged_arr); extern struct _tagged_arr Cyc_Filename_basename( struct
_tagged_arr); extern int Cyc_Filename_check_suffix( struct _tagged_arr, struct
_tagged_arr); extern struct _tagged_arr Cyc_Filename_gnuify( struct _tagged_arr);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
extern struct _tagged_arr Cyc_Std_strconcat( struct _tagged_arr, struct
_tagged_arr); extern struct _tagged_arr Cyc_Std_substring( struct _tagged_arr,
int ofs, unsigned int n); struct _tagged_arr Cyc_Filename_concat( struct
_tagged_arr s1, struct _tagged_arr s2){ return Cyc_Std_strconcat( s1,( struct
_tagged_arr) Cyc_Std_strconcat( _tag_arr("/", sizeof( unsigned char), 2u), s2));}
struct _tagged_arr Cyc_Filename_chop_extension( struct _tagged_arr filename){
int i=( int)( _get_arr_size( filename, sizeof( unsigned char)) -  1); while( i
>=  0?*(( const unsigned char*) _check_unknown_subscript( filename, sizeof(
unsigned char), i)) != '.': 0) { -- i;} if( i <  0){( int) _throw(( void*)({
struct Cyc_Core_Invalid_argument_struct* _temp0=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp0[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp1; _temp1.tag= Cyc_Core_Invalid_argument;
_temp1.f1= _tag_arr("chop_extension", sizeof( unsigned char), 15u); _temp1;});
_temp0;}));} return Cyc_Std_substring( filename, 0,( unsigned int) i);} struct
_tagged_arr Cyc_Filename_dirname( struct _tagged_arr filename){ int i=( int)(
_get_arr_size( filename, sizeof( unsigned char)) -  1); while( i >=  0?*(( const
unsigned char*) _check_unknown_subscript( filename, sizeof( unsigned char), i))
!= '/': 0) { -- i;} if( i <  0){ return Cyc_Core_new_string( 0);} return Cyc_Std_substring(
filename, 0,( unsigned int) i);} struct _tagged_arr Cyc_Filename_basename(
struct _tagged_arr filename){ int i=( int)( _get_arr_size( filename, sizeof(
unsigned char)) -  1); while( i >=  0?*(( const unsigned char*)
_check_unknown_subscript( filename, sizeof( unsigned char), i)) != '/': 0) { --
i;} return Cyc_Std_substring( filename, i +  1, _get_arr_size( filename, sizeof(
unsigned char)) - ( i +  1));} int Cyc_Filename_check_suffix( struct _tagged_arr
filename, struct _tagged_arr suffix){ int i=( int)( _get_arr_size( filename,
sizeof( unsigned char)) -  1); int j=( int)( _get_arr_size( suffix, sizeof(
unsigned char)) -  1); while( i >=  0? j >=  0: 0) { if(*(( const unsigned char*)
_check_unknown_subscript( filename, sizeof( unsigned char), i --)) != *(( const
unsigned char*) _check_unknown_subscript( suffix, sizeof( unsigned char), j --))){
return 0;}} if( j >=  0){ return 0;} else{ return 1;}} struct _tagged_arr Cyc_Filename_gnuify(
struct _tagged_arr filename){ int has_drive_name= _get_arr_size( filename,
sizeof( unsigned char)) >  1?*(( const unsigned char*) _check_unknown_subscript(
filename, sizeof( unsigned char), 1)) == ':': 0; int i; int j; struct
_tagged_arr ans; int ans_sz; if( has_drive_name){ ans_sz=( int)( _get_arr_size(
filename, sizeof( unsigned char)) +  1); ans=({ unsigned int _temp2=(
unsigned int) ans_sz; unsigned char* _temp3=( unsigned char*) _cycalloc_atomic(
_check_times( sizeof( unsigned char), _temp2)); struct _tagged_arr _temp5=
_tag_arr( _temp3, sizeof( unsigned char),( unsigned int) ans_sz);{ unsigned int
_temp4= _temp2; unsigned int k; for( k= 0; k <  _temp4; k ++){ _temp3[ k]='\000';}};
_temp5;});*(( unsigned char*) _check_unknown_subscript( ans, sizeof(
unsigned char), 0))=(*(( unsigned char*) _check_unknown_subscript( ans, sizeof(
unsigned char), 1))='/');*(( unsigned char*) _check_unknown_subscript( ans,
sizeof( unsigned char), 2))=*(( const unsigned char*) _check_unknown_subscript(
filename, sizeof( unsigned char), 0)); i= 3; j= 2;} else{ ans_sz=( int)
_get_arr_size( filename, sizeof( unsigned char)); ans=({ unsigned int _temp6=(
unsigned int) ans_sz; unsigned char* _temp7=( unsigned char*) _cycalloc_atomic(
_check_times( sizeof( unsigned char), _temp6)); struct _tagged_arr _temp9=
_tag_arr( _temp7, sizeof( unsigned char),( unsigned int) ans_sz);{ unsigned int
_temp8= _temp6; unsigned int k; for( k= 0; k <  _temp8; k ++){ _temp7[ k]='\000';}};
_temp9;}); i= 0; j= 0;} while( i <  ans_sz) { unsigned char c=*(( const
unsigned char*) _check_unknown_subscript( filename, sizeof( unsigned char), j ++));*((
unsigned char*) _check_unknown_subscript( ans, sizeof( unsigned char), i ++))= c
== '\\'?'/': c;} return ans;}
