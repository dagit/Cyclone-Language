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
static inline void * _cyccalloc(unsigned int s, unsigned int n) {
  void * ans = (void *)GC_calloc(s,n);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int s, unsigned int n) {
  void * ans = (void *)GC_calloc_atomic(s,n);
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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned char Cyc_Array_Array_mismatch[
19u]; struct Cyc_Std__Div{ int quot; int rem; } ; struct Cyc_Std__Ldiv{ int quot;
int rem; } ; extern int abs( int __x); extern int atexit( void(* __func)());
extern struct Cyc_Std__Div div( int __numer, int __denom); extern struct Cyc_Std__Ldiv
ldiv( int __numer, int __denom); extern int random(); extern void srandom(
unsigned int __seed); extern int rand(); extern void srand( unsigned int __seed);
extern int rand_r( unsigned int* __seed); extern int grantpt( int __fd); extern
int unlockpt( int __fd); extern double Cyc_Std_atof( struct _tagged_arr); extern
int Cyc_Std_atoi( struct _tagged_arr); extern int Cyc_Std_atol( struct
_tagged_arr); extern struct _tagged_arr Cyc_Std_getenv( struct _tagged_arr);
extern double Cyc_Std_strtod( struct _tagged_arr n, struct _tagged_arr* end);
extern int Cyc_Std_strtol( struct _tagged_arr n, struct _tagged_arr* end, int
base); extern unsigned int Cyc_Std_strtoul( struct _tagged_arr n, struct
_tagged_arr* end, int base); extern unsigned int Cyc_Std_mstrtoul( struct
_tagged_arr n, struct _tagged_arr* endptr, int base); extern void Cyc_Std_qsort(
struct _tagged_arr tab, unsigned int nmemb, unsigned int szmemb, int(* compar)(
const void*, const void*)); extern int Cyc_Std_system( struct _tagged_arr);
extern void Cyc_Std_free( struct _tagged_arr); extern double atof( unsigned char*);
extern int atoi( unsigned char*); extern int atol( unsigned char*); extern
unsigned char* getenv( unsigned char*); extern int putenv( unsigned char*);
extern double strtod( unsigned char*, unsigned char**); extern int strtol(
unsigned char*, unsigned char**, int); extern unsigned int strtoul(
unsigned char*, unsigned char**, int); extern void qsort( void* base,
unsigned int nmemb, unsigned int size, int(* compar)( const void*, const void*));
extern int system( unsigned char*); double Cyc_Std_atof( struct _tagged_arr
_nptr){ return atof( string_to_Cstring( _nptr));} int Cyc_Std_atoi( struct
_tagged_arr _nptr){ return atoi( string_to_Cstring( _nptr));} int Cyc_Std_atol(
struct _tagged_arr _nptr){ return atol( string_to_Cstring( _nptr));} struct
_tagged_arr Cyc_Std_getenv( struct _tagged_arr name){ return Cstring_to_string(
getenv( string_to_Cstring( name)));} int Cyc_Std_putenv( struct _tagged_arr s){
return putenv( string_to_Cstring( s));} static void Cyc_Std_check_valid_cstring(
struct _tagged_arr s){ if( s.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){(
int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp0=( struct
Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp0[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp1; _temp1.tag= Cyc_Core_Invalid_argument;
_temp1.f1= _tag_arr("strtox NULL pointer", sizeof( unsigned char), 20u); _temp1;});
_temp0;}));}{ int found_zero= 0;{ int i=( int)( _get_arr_size( s, sizeof(
unsigned char)) -  1); for( 0; i >=  0; i --){ if(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) == '\000'){ found_zero=
1; break;}}} if( ! found_zero){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp2=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp2[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp3; _temp3.tag= Cyc_Core_Invalid_argument;
_temp3.f1= _tag_arr("strtox: not a C string", sizeof( unsigned char), 23u);
_temp3;}); _temp2;}));}}} double Cyc_Std_strtod( struct _tagged_arr nptr, struct
_tagged_arr* endptr){ Cyc_Std_check_valid_cstring( nptr);{ unsigned char* c=
underlying_Cstring( nptr); unsigned char* e= endptr ==  0? 0: c; double d=
strtod( c,( unsigned char**)& e); if( endptr !=  0){ int n=( int)(( unsigned int)
e - ( unsigned int) c);* endptr= _tagged_arr_plus( nptr, sizeof( unsigned char),
n);} return d;}} int Cyc_Std_strtol( struct _tagged_arr n, struct _tagged_arr*
endptr, int base){ Cyc_Std_check_valid_cstring( n);{ unsigned char* c=
underlying_Cstring( n); unsigned char* e= endptr ==  0? 0: c; int r= strtol( c,(
unsigned char**)& e, base); if( endptr !=  0){ int m=( int)(( unsigned int) e - (
unsigned int) c);* endptr= _tagged_arr_plus( n, sizeof( unsigned char), m);}
return r;}} unsigned int Cyc_Std_strtoul( struct _tagged_arr n, struct
_tagged_arr* endptr, int base){ Cyc_Std_check_valid_cstring( n);{ unsigned char*
c= underlying_Cstring( n); unsigned char* e= endptr ==  0? 0: c; unsigned int r=
strtoul( c,( unsigned char**)& e, base); if( endptr !=  0){ int m=( int)((
unsigned int) e - ( unsigned int) c);* endptr= _tagged_arr_plus( n, sizeof(
unsigned char), m);} return r;}} unsigned int Cyc_Std_mstrtoul( struct
_tagged_arr n, struct _tagged_arr* endptr, int base){ Cyc_Std_check_valid_cstring((
struct _tagged_arr) n);{ unsigned char* c= underlying_Cstring(( struct
_tagged_arr) n); unsigned char* e= endptr ==  0? 0: c; unsigned int r= strtoul(
c,( unsigned char**)& e, base); if( endptr !=  0){ int m=( int)(( unsigned int)
e - ( unsigned int) c);* endptr= _tagged_arr_plus( n, sizeof( unsigned char), m);}
return r;}} void Cyc_Std_qsort( struct _tagged_arr tab, unsigned int nmemb,
unsigned int szmemb, int(* compar)( const void*, const void*)){ if( tab.curr == (
_tag_arr( 0u, 0u, 0u)).curr? 1: _get_arr_size( tab, sizeof( void)) <  nmemb){(
int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp4=( struct
Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp4[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp5; _temp5.tag= Cyc_Core_Invalid_argument;
_temp5.f1= _tag_arr("Std::qsort", sizeof( unsigned char), 11u); _temp5;});
_temp4;}));} qsort(( void*) _check_null( _untag_arr( tab, sizeof( void), 1u)),
nmemb,( unsigned int) szmemb,( int(*)( const void*, const void*)) compar);} int
Cyc_Std_system( struct _tagged_arr cmd){ return system( string_to_Cstring( cmd));}
void Cyc_Std_free( struct _tagged_arr ptr){;}
