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
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern unsigned char Cyc_Core_Invalid_argument[
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
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
static const int Cyc_Fn_Fun= 0; struct Cyc_Fn_Fun_struct{ int tag; void*(* f1)(
void*, void*); void* f2; } ; extern void* Cyc_Fn_make_fn( void*(* f)( void*,
void*), void* x); extern void* Cyc_Fn_fp2fn( void*(* f)( void*)); extern void*
Cyc_Fn_apply( void* f, void* x); extern void* Cyc_Fn_compose( void* g, void* f);
extern void* Cyc_Fn_curry( void* f); extern void* Cyc_Fn_uncurry( void* f);
extern struct Cyc_List_List* Cyc_Fn_map_fn( void* f, struct Cyc_List_List* x);
void* Cyc_Fn_make_fn( void*(* f)( void*, void*), void* x){ return( void*)({
struct Cyc_Fn_Fun_struct* _temp0=( struct Cyc_Fn_Fun_struct*) _cycalloc( sizeof(
struct Cyc_Fn_Fun_struct)); _temp0[ 0]=({ struct Cyc_Fn_Fun_struct _temp1;
_temp1.tag= Cyc_Fn_Fun; _temp1.f1= f; _temp1.f2=( void*) x; _temp1;}); _temp0;});}
static void* Cyc_Fn_fp_apply( void*(* f)( void*), void* x){ return f( x);} void*
Cyc_Fn_fp2fn( void*(* f)( void*)){ return(( void*(*)( void*(* f)( void*(*)( void*),
void*), void*(* x)( void*))) Cyc_Fn_make_fn)( Cyc_Fn_fp_apply, f);} void* Cyc_Fn_apply(
void* f, void* x){ void* _temp4; void*(* _temp6)( void*, void*); void* _temp2= f;
if(*(( int*) _temp2) ==  Cyc_Fn_Fun){ _LL7: _temp6=(( struct Cyc_Fn_Fun_struct*)
_temp2)->f1; goto _LL5; _LL5: _temp4=( void*)(( struct Cyc_Fn_Fun_struct*)
_temp2)->f2; goto _LL3;} else{ goto _LL3;} _LL3: return _temp6( _temp4, x);}
struct _tuple0{ void* f1; void* f2; } ; static void* Cyc_Fn_fn_compose( struct
_tuple0* f_and_g, void* arg){ struct _tuple0 _temp10; void* _temp11; void*
_temp13; struct _tuple0* _temp8= f_and_g; _temp10=* _temp8; _LL14: _temp13=
_temp10.f1; goto _LL12; _LL12: _temp11= _temp10.f2; goto _LL9; _LL9: return Cyc_Fn_apply(
_temp13, Cyc_Fn_apply( _temp11, arg));} void* Cyc_Fn_compose( void* g, void* f){
return(( void*(*)( void*(* f)( struct _tuple0*, void*), struct _tuple0* x)) Cyc_Fn_make_fn)(
Cyc_Fn_fn_compose,({ struct _tuple0* _temp15=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp15->f1= f; _temp15->f2= g; _temp15;}));} static
void* Cyc_Fn_inner( struct _tuple0* env, void* second){ return(( void*(*)( void*
f, struct _tuple0* x)) Cyc_Fn_apply)((* env).f1,({ struct _tuple0* _temp16=(
struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp16->f1=(* env).f2;
_temp16->f2= second; _temp16;}));} static void* Cyc_Fn_outer( void* f, void*
first){ return(( void*(*)( void*(* f)( struct _tuple0*, void*), struct _tuple0*
x)) Cyc_Fn_make_fn)( Cyc_Fn_inner,({ struct _tuple0* _temp17=( struct _tuple0*)
_cycalloc( sizeof( struct _tuple0)); _temp17->f1= f; _temp17->f2= first; _temp17;}));}
void* Cyc_Fn_curry( void* f){ return Cyc_Fn_make_fn( Cyc_Fn_outer, f);} static
void* Cyc_Fn_lambda( void* f, struct _tuple0* arg){ return Cyc_Fn_apply( Cyc_Fn_apply(
f,(* arg).f1),(* arg).f2);} void* Cyc_Fn_uncurry( void* f){ return(( void*(*)(
void*(* f)( void*, struct _tuple0*), void* x)) Cyc_Fn_make_fn)( Cyc_Fn_lambda, f);}
struct Cyc_List_List* Cyc_Fn_map_fn( void* f, struct Cyc_List_List* x){ struct
Cyc_List_List* res= 0; for( 0; x !=  0; x= x->tl){ res=({ struct Cyc_List_List*
_temp18=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp18->hd=( void*) Cyc_Fn_apply( f,( void*) x->hd); _temp18->tl= res; _temp18;});}
res= Cyc_List_imp_rev( res); return res;}
