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
 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern
struct Cyc_List_List* Cyc_List_rlist( struct _RegionHandle*, struct _tagged_arr);
extern int Cyc_List_length( struct Cyc_List_List* x); extern void* Cyc_List_hd(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_copy( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_rcopy( struct _RegionHandle*, struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_map2( void*(* f)( void*, void*),
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_rmap2( struct _RegionHandle*, void*(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y); extern void Cyc_List_app( void*(* f)( void*),
struct Cyc_List_List* x); extern void Cyc_List_app_c( void*(* f)( void*, void*),
void*, struct Cyc_List_List* x); extern void Cyc_List_app2( void*(* f)( void*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y); extern void Cyc_List_app2_c(
void*(* f)( void*, void*, void*), void* env, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x);
extern void Cyc_List_iter_c( void(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern void Cyc_List_iter2( void(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y); extern void Cyc_List_iter2_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_List_List* x, struct Cyc_List_List* y);
extern void* Cyc_List_fold_left( void*(* f)( void*, void*), void* accum, struct
Cyc_List_List* x); extern void* Cyc_List_fold_left_c( void*(* f)( void*, void*,
void*), void*, void* accum, struct Cyc_List_List* x); extern void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum); extern void*
Cyc_List_fold_right_c( void*(* f)( void*, void*, void*), void*, struct Cyc_List_List*
x, void* accum); extern struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rrevappend(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rrev( struct _RegionHandle*, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_flatten( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rflatten(
struct _RegionHandle*, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_merge_sort( int(* cmp)( void*, void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_rmerge_sort( struct _RegionHandle*, int(* cmp)(
void*, void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rimp_merge_sort(
int(* cmp)( void*, void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_merge( int(* cmp)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rmerge( struct _RegionHandle*, int(*
cmp)( void*, void*), struct Cyc_List_List* a, struct Cyc_List_List* b); extern
struct Cyc_List_List* Cyc_List_imp_merge( int(* cmp)( void*, void*), struct Cyc_List_List*
a, struct Cyc_List_List* b); extern unsigned char Cyc_List_Nth[ 8u]; extern void*
Cyc_List_nth( struct Cyc_List_List* x, int n); extern struct Cyc_List_List* Cyc_List_nth_tail(
struct Cyc_List_List* x, int i); extern int Cyc_List_forall( int(* pred)( void*),
struct Cyc_List_List* x); extern int Cyc_List_forall_c( int(* pred)( void*, void*),
void* env, struct Cyc_List_List* x); extern int Cyc_List_exists( int(* pred)(
void*), struct Cyc_List_List* x); extern int Cyc_List_exists_c( int(* pred)(
void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct
Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct _RegionHandle* r2,
struct Cyc_List_List* x, struct Cyc_List_List* y); struct _tuple0{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; extern struct _tuple0 Cyc_List_split( struct
Cyc_List_List* x); struct _tuple1{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; extern struct _tuple1 Cyc_List_split3( struct
Cyc_List_List* x); extern struct _tuple0 Cyc_List_rsplit( struct _RegionHandle*
r1, struct _RegionHandle* r2, struct Cyc_List_List* x); extern struct _tuple1
Cyc_List_rsplit3( struct _RegionHandle* r3, struct _RegionHandle* r4, struct
_RegionHandle* r5, struct Cyc_List_List* x); extern int Cyc_List_memq( struct
Cyc_List_List* l, void* x); extern int Cyc_List_mem( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x); extern void* Cyc_List_assoc( struct Cyc_List_List*
l, void* k); extern void* Cyc_List_assoc_cmp( int(* cmp)( void*, void*), struct
Cyc_List_List* l, void* x); extern int Cyc_List_mem_assoc( struct Cyc_List_List*
l, void* x); extern struct Cyc_List_List* Cyc_List_delete( struct Cyc_List_List*
l, void* x); extern struct Cyc_Core_Opt* Cyc_List_check_unique( int(* cmp)( void*,
void*), struct Cyc_List_List* x); extern struct _tagged_arr Cyc_List_to_array(
struct Cyc_List_List* x); extern struct _tagged_arr Cyc_List_rto_array( struct
_RegionHandle* r, struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_from_array(
struct _tagged_arr arr); extern struct Cyc_List_List* Cyc_List_rfrom_array(
struct _RegionHandle* r2, struct _tagged_arr arr); extern struct Cyc_List_List*
Cyc_List_tabulate( int n, void*(* f)( int)); extern struct Cyc_List_List* Cyc_List_tabulate_c(
int n, void*(* f)( void*, int), void* env); extern struct Cyc_List_List* Cyc_List_rtabulate(
struct _RegionHandle* r, int n, void*(* f)( int)); extern struct Cyc_List_List*
Cyc_List_rtabulate_c( struct _RegionHandle* r, int n, void*(* f)( void*, int),
void* env); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); extern int Cyc_List_list_prefix( int(* cmp)( void*,
void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); extern struct Cyc_List_List*
Cyc_List_filter( int(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rfilter( struct _RegionHandle* r, int(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rfilter_c(
struct _RegionHandle* r, int(* f)( void*, void*), void* env, struct Cyc_List_List*
x); int Cyc_List_length( struct Cyc_List_List* x){ int i= 0; while( x !=  0) {
++ i; x= x->tl;} return i;} void* Cyc_List_hd( struct Cyc_List_List* x){ if( x
==  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp0=( struct
Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp0[ 0]=({ struct Cyc_Core_Failure_struct _temp1; _temp1.tag= Cyc_Core_Failure;
_temp1.f1= _tag_arr("hd", sizeof( unsigned char), 3u); _temp1;}); _temp0;}));}
return( void*) x->hd;} struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List*
x){ if( x ==  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp2=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp2[ 0]=({ struct Cyc_Core_Failure_struct _temp3; _temp3.tag= Cyc_Core_Failure;
_temp3.f1= _tag_arr("tl", sizeof( unsigned char), 3u); _temp3;}); _temp2;}));}
return x->tl;} struct Cyc_List_List* Cyc_List_rlist( struct _RegionHandle* r,
struct _tagged_arr argv){ struct Cyc_List_List* result= 0;{ int i=( int)(
_get_arr_size( argv, sizeof( void*)) -  1); for( 0; i >=  0; i --){ result=({
struct Cyc_List_List* _temp4=( struct Cyc_List_List*) _region_malloc( r, sizeof(
struct Cyc_List_List)); _temp4->hd=( void*)*(( void**) _check_unknown_subscript(
argv, sizeof( void*), i)); _temp4->tl= result; _temp4;});}} return result;}
struct Cyc_List_List* Cyc_List_list( struct _tagged_arr argv){ struct Cyc_List_List*
result= 0;{ int i=( int)( _get_arr_size( argv, sizeof( void*)) -  1); for( 0; i
>=  0; i --){ result=({ struct Cyc_List_List* _temp5=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp5->hd=( void*)*(( void**)
_check_unknown_subscript( argv, sizeof( void*), i)); _temp5->tl= result; _temp5;});}}
return result;} struct Cyc_List_List* Cyc_List_rcopy( struct _RegionHandle* r2,
struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x ==  0){ return 0;} result=({ struct Cyc_List_List* _temp6=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp6->hd=(
void*)(( void*) x->hd); _temp6->tl= 0; _temp6;}); prev= result; for( x= x->tl; x
!=  0; x= x->tl){(( struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List*
_temp7=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp7->hd=( void*)(( void*) x->hd); _temp7->tl= 0; _temp7;}); prev=(( struct
Cyc_List_List*) _check_null( prev))->tl;} return result;} struct Cyc_List_List*
Cyc_List_copy( struct Cyc_List_List* x){ return Cyc_List_rcopy( Cyc_Core_heap_region,
x);} struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle* r2, void*(* f)(
void*), struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x ==  0){ return 0;} result=({ struct Cyc_List_List* _temp8=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp8->hd=(
void*) f(( void*) x->hd); _temp8->tl= 0; _temp8;}); prev= result; for( x= x->tl;
x !=  0; x= x->tl){ struct Cyc_List_List* temp=({ struct Cyc_List_List* _temp9=(
struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp9->hd=( void*) f(( void*) x->hd); _temp9->tl= 0; _temp9;});(( struct Cyc_List_List*)
_check_null( prev))->tl= temp; prev= temp;} return result;} struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x){ return Cyc_List_rmap(
Cyc_Core_heap_region, f, x);} struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x ==  0){ return 0;}
result=({ struct Cyc_List_List* _temp10=( struct Cyc_List_List*) _region_malloc(
r2, sizeof( struct Cyc_List_List)); _temp10->hd=( void*) f( env,( void*) x->hd);
_temp10->tl= 0; _temp10;}); prev= result; for( x= x->tl; x !=  0; x= x->tl){((
struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List* _temp11=(
struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp11->hd=( void*) f( env,( void*) x->hd); _temp11->tl= 0; _temp11;}); prev=((
struct Cyc_List_List*) _check_null( prev))->tl;} return result;} struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
return Cyc_List_rmap_c( Cyc_Core_heap_region, f, env, x);} unsigned char Cyc_List_List_mismatch[
18u]="\000\000\000\000List_mismatch"; struct Cyc_List_List* Cyc_List_rmap2(
struct _RegionHandle* r3, void*(* f)( void*, void*), struct Cyc_List_List* x,
struct Cyc_List_List* y){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x ==  0? y ==  0: 0){ return 0;} if( x ==  0? 1: y ==  0){( int)
_throw(( void*) Cyc_List_List_mismatch);} result=({ struct Cyc_List_List*
_temp12=( struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp12->hd=( void*) f(( void*) x->hd,( void*) y->hd); _temp12->tl= 0; _temp12;});
prev= result; x= x->tl; y= y->tl; while( x !=  0? y !=  0: 0) {(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp13=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp13->hd=( void*) f((
void*) x->hd,( void*) y->hd); _temp13->tl= 0; _temp13;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl; x= x->tl; y= y->tl;} if( x !=  0? 1: y !=  0){( int)
_throw(( void*) Cyc_List_List_mismatch);} return result;} struct Cyc_List_List*
Cyc_List_map2( void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y){ return Cyc_List_rmap2( Cyc_Core_heap_region, f, x, y);} void Cyc_List_app(
void*(* f)( void*), struct Cyc_List_List* x){ while( x !=  0) { f(( void*) x->hd);
x= x->tl;}} void Cyc_List_app_c( void*(* f)( void*, void*), void* env, struct
Cyc_List_List* x){ while( x !=  0) { f( env,( void*) x->hd); x= x->tl;}} void
Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x){ while( x !=  0) { f((
void*) x->hd); x= x->tl;}} void Cyc_List_iter_c( void(* f)( void*, void*), void*
env, struct Cyc_List_List* x){ while( x !=  0) { f( env,( void*) x->hd); x= x->tl;}}
void Cyc_List_app2( void*(* f)( void*, void*), struct Cyc_List_List* x, struct
Cyc_List_List* y){ while( x !=  0? y !=  0: 0) { f(( void*) x->hd,( void*) y->hd);
x= x->tl; y= y->tl;} if( x !=  0? 1: y !=  0){( int) _throw(( void*) Cyc_List_List_mismatch);}}
void Cyc_List_app2_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_List_List*
x, struct Cyc_List_List* y){ while( x !=  0? y !=  0: 0) { f( env,( void*) x->hd,(
void*) y->hd); x= x->tl; y= y->tl;} if( x !=  0? 1: y !=  0){( int) _throw((
void*) Cyc_List_List_mismatch);}} void Cyc_List_iter2( void(* f)( void*, void*),
struct Cyc_List_List* x, struct Cyc_List_List* y){ while( x !=  0? y !=  0: 0) {
f(( void*) x->hd,( void*) y->hd); x= x->tl; y= y->tl;} if( x !=  0? 1: y !=  0){(
int) _throw(( void*) Cyc_List_List_mismatch);}} void Cyc_List_iter2_c( void(* f)(
void*, void*, void*), void* env, struct Cyc_List_List* x, struct Cyc_List_List*
y){ while( x !=  0? y !=  0: 0) { f( env,( void*) x->hd,( void*) y->hd); x= x->tl;
y= y->tl;} if( x !=  0? 1: y !=  0){( int) _throw(( void*) Cyc_List_List_mismatch);}}
void* Cyc_List_fold_left( void*(* f)( void*, void*), void* accum, struct Cyc_List_List*
x){ while( x !=  0) { accum= f( accum,( void*) x->hd); x= x->tl;} return accum;}
void* Cyc_List_fold_left_c( void*(* f)( void*, void*, void*), void* env, void*
accum, struct Cyc_List_List* x){ while( x !=  0) { accum= f( env, accum,( void*)
x->hd); x= x->tl;} return accum;} void* Cyc_List_fold_right( void*(* f)( void*,
void*), struct Cyc_List_List* x, void* accum){ if( x ==  0){ return accum;}
else{ return f(( void*) x->hd, Cyc_List_fold_right( f, x->tl, accum));}} void*
Cyc_List_fold_right_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_List_List*
x, void* accum){ if( x ==  0){ return accum;} else{ return f( env,( void*) x->hd,
Cyc_List_fold_right_c( f, env, x->tl, accum));}} struct Cyc_List_List* Cyc_List_rrevappend(
struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y){
while( x !=  0) { y=({ struct Cyc_List_List* _temp14=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp14->hd=( void*)(( void*)
x->hd); _temp14->tl= y; _temp14;}); x= x->tl;} return y;} struct Cyc_List_List*
Cyc_List_revappend( struct Cyc_List_List* x, struct Cyc_List_List* y){ return
Cyc_List_rrevappend( Cyc_Core_heap_region, x, y);} struct Cyc_List_List* Cyc_List_rrev(
struct _RegionHandle* r2, struct Cyc_List_List* x){ if( x ==  0){ return 0;}
return Cyc_List_rrevappend( r2, x, 0);} struct Cyc_List_List* Cyc_List_rev(
struct Cyc_List_List* x){ return Cyc_List_rrev( Cyc_Core_heap_region, x);}
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x){ if( x ==  0){
return x;} else{ struct Cyc_List_List* first= x; struct Cyc_List_List* second= x->tl;
x->tl= 0; while( second !=  0) { struct Cyc_List_List* temp= second->tl; second->tl=
first; first= second; second= temp;} return first;}} struct Cyc_List_List* Cyc_List_rappend(
struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x ==  0){ return y;}
if( y ==  0){ return Cyc_List_rcopy( r2, x);} result=({ struct Cyc_List_List*
_temp15=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp15->hd=( void*)(( void*) x->hd); _temp15->tl= 0; _temp15;}); prev= result;
for( x= x->tl; x !=  0; x= x->tl){(( struct Cyc_List_List*) _check_null( prev))->tl=({
struct Cyc_List_List* _temp16=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp16->hd=( void*)(( void*) x->hd); _temp16->tl=
0; _temp16;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;}(( struct
Cyc_List_List*) _check_null( prev))->tl= y; return result;} struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y){ return Cyc_List_rappend(
Cyc_Core_heap_region, x, y);} struct Cyc_List_List* Cyc_List_imp_append( struct
Cyc_List_List* x, struct Cyc_List_List* y){ struct Cyc_List_List* z; if( x ==  0){
return y;} if( y ==  0){ return x;} for( z= x; z->tl !=  0; z= z->tl){;} z->tl=
y; return x;} struct Cyc_List_List* Cyc_List_rflatten( struct _RegionHandle* r3,
struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _RegionHandle*, struct Cyc_List_List*, struct Cyc_List_List*), struct
_RegionHandle* env, struct Cyc_List_List* x, struct Cyc_List_List* accum)) Cyc_List_fold_right_c)(
Cyc_List_rappend, r3, x, 0);} struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List*
x){ return Cyc_List_rflatten( Cyc_Core_heap_region, x);} struct Cyc_List_List*
Cyc_List_imp_merge( int(* less_eq)( void*, void*), struct Cyc_List_List* a,
struct Cyc_List_List* b){ struct Cyc_List_List* c; struct Cyc_List_List* d; if(
a ==  0){ return b;} if( b ==  0){ return a;} if( less_eq(( void*) a->hd,( void*)
b->hd) <=  0){ c= a; a= a->tl;} else{ c= b; b= b->tl;} d= c; while( a !=  0? b
!=  0: 0) { if( less_eq(( void*) a->hd,( void*) b->hd) <=  0){ c->tl= a; c= a; a=
a->tl;} else{ c->tl= b; c= b; b= b->tl;}} if( a ==  0){ c->tl= b;} else{ c->tl=
a;} return d;} struct Cyc_List_List* Cyc_List_rimp_merge_sort( int(* less_eq)(
void*, void*), struct Cyc_List_List* x){ if( x ==  0? 1: x->tl ==  0){ return x;}{
struct Cyc_List_List* a= x; struct Cyc_List_List* aptr= a; struct Cyc_List_List*
b= x->tl; struct Cyc_List_List* bptr= b; x=(( struct Cyc_List_List*) _check_null(
b))->tl; while( x !=  0) { aptr->tl= x; aptr= x; x= x->tl; if( x !=  0){((
struct Cyc_List_List*) _check_null( bptr))->tl= x; bptr= x; x= x->tl;}} aptr->tl=
0;(( struct Cyc_List_List*) _check_null( bptr))->tl= 0; return Cyc_List_imp_merge(
less_eq, Cyc_List_rimp_merge_sort( less_eq, a), Cyc_List_rimp_merge_sort(
less_eq, b));}} struct Cyc_List_List* Cyc_List_rmerge_sort( struct _RegionHandle*
r2, int(* less_eq)( void*, void*), struct Cyc_List_List* x){ return Cyc_List_rimp_merge_sort(
less_eq, Cyc_List_rcopy( r2, x));} struct Cyc_List_List* Cyc_List_rmerge( struct
_RegionHandle* r3, int(* less_eq)( void*, void*), struct Cyc_List_List* a,
struct Cyc_List_List* b){ struct Cyc_List_List* c; struct Cyc_List_List* d; if(
a ==  0){ return Cyc_List_rcopy( r3, b);} if( b ==  0){ return Cyc_List_rcopy(
r3, a);} if( less_eq(( void*) a->hd,( void*) b->hd) <=  0){ c=({ struct Cyc_List_List*
_temp17=( struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp17->hd=( void*)(( void*) a->hd); _temp17->tl= 0; _temp17;}); a= a->tl;}
else{ c=({ struct Cyc_List_List* _temp18=( struct Cyc_List_List*) _region_malloc(
r3, sizeof( struct Cyc_List_List)); _temp18->hd=( void*)(( void*) b->hd);
_temp18->tl= 0; _temp18;}); b= b->tl;} d= c; while( a !=  0? b !=  0: 0) { if(
less_eq(( void*) a->hd,( void*) b->hd) <=  0){(( struct Cyc_List_List*)
_check_null( c))->tl=({ struct Cyc_List_List* _temp19=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp19->hd=( void*)(( void*)
a->hd); _temp19->tl= 0; _temp19;}); c= c->tl; a= a->tl;} else{(( struct Cyc_List_List*)
_check_null( c))->tl=({ struct Cyc_List_List* _temp20=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp20->hd=( void*)(( void*)
b->hd); _temp20->tl= 0; _temp20;}); c= c->tl; b= b->tl;}} if( a ==  0){(( struct
Cyc_List_List*) _check_null( c))->tl= Cyc_List_rcopy( r3, b);} else{(( struct
Cyc_List_List*) _check_null( c))->tl= Cyc_List_rcopy( r3, a);} return d;} struct
Cyc_List_List* Cyc_List_merge_sort( int(* less_eq)( void*, void*), struct Cyc_List_List*
x){ return Cyc_List_rmerge_sort( Cyc_Core_heap_region, less_eq, x);} struct Cyc_List_List*
Cyc_List_merge( int(* less_eq)( void*, void*), struct Cyc_List_List* a, struct
Cyc_List_List* b){ return Cyc_List_rmerge( Cyc_Core_heap_region, less_eq, a, b);}
unsigned char Cyc_List_Nth[ 8u]="\000\000\000\000Nth"; void* Cyc_List_nth(
struct Cyc_List_List* x, int i){ while( i >  0? x !=  0: 0) { -- i; x= x->tl;}
if( i <  0? 1: x ==  0){( int) _throw(( void*) Cyc_List_Nth);} return( void*) x->hd;}
struct Cyc_List_List* Cyc_List_nth_tail( struct Cyc_List_List* x, int i){ if( i
<  0){( int) _throw(( void*) Cyc_List_Nth);} while( i !=  0) { if( x ==  0){(
int) _throw(( void*) Cyc_List_Nth);} x= x->tl; -- i;} return x;} int Cyc_List_forall(
int(* pred)( void*), struct Cyc_List_List* x){ while( x !=  0? pred(( void*) x->hd):
0) { x= x->tl;} return x ==  0;} int Cyc_List_forall_c( int(* pred)( void*, void*),
void* env, struct Cyc_List_List* x){ while( x !=  0? pred( env,( void*) x->hd):
0) { x= x->tl;} return x ==  0;} int Cyc_List_exists( int(* pred)( void*),
struct Cyc_List_List* x){ while( x !=  0? ! pred(( void*) x->hd): 0) { x= x->tl;}
return x !=  0;} int Cyc_List_exists_c( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x){ while( x !=  0? ! pred( env,( void*) x->hd): 0) { x= x->tl;}
return x !=  0;} struct _tuple2{ void* f1; void* f2; } ; struct Cyc_List_List*
Cyc_List_rzip( struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List*
x, struct Cyc_List_List* y){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x ==  0? y ==  0: 0){ return 0;} if( x ==  0? 1: y ==  0){( int)
_throw(( void*) Cyc_List_List_mismatch);} result=({ struct Cyc_List_List*
_temp21=( struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp21->hd=( void*)({ struct _tuple2* _temp22=( struct _tuple2*) _region_malloc(
r4, sizeof( struct _tuple2)); _temp22->f1=( void*) x->hd; _temp22->f2=( void*) y->hd;
_temp22;}); _temp21->tl= 0; _temp21;}); prev= result; x= x->tl; y= y->tl; while(
x !=  0? y !=  0: 0) {(( struct Cyc_List_List*) _check_null( prev))->tl=({
struct Cyc_List_List* _temp23=( struct Cyc_List_List*) _region_malloc( r3,
sizeof( struct Cyc_List_List)); _temp23->hd=( void*)({ struct _tuple2* _temp24=(
struct _tuple2*) _region_malloc( r4, sizeof( struct _tuple2)); _temp24->f1=(
void*) x->hd; _temp24->f2=( void*) y->hd; _temp24;}); _temp23->tl= 0; _temp23;});
prev=(( struct Cyc_List_List*) _check_null( prev))->tl; x= x->tl; y= y->tl;} if(
x !=  0? 1: y !=  0){( int) _throw(( void*) Cyc_List_List_mismatch);} return
result;} struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List*
y){ return Cyc_List_rzip( Cyc_Core_heap_region, Cyc_Core_heap_region, x, y);}
struct _tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x){ struct Cyc_List_List* result1; struct Cyc_List_List*
prev1; struct Cyc_List_List* result2; struct Cyc_List_List* prev2; if( x ==  0){
return({ struct _tuple0 _temp25; _temp25.f1= 0; _temp25.f2= 0; _temp25;});}
prev1=( result1=({ struct Cyc_List_List* _temp26=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp26->hd=( void*)(((
struct _tuple2*) x->hd)[ _check_known_subscript_notnull( 1u, 0)]).f1; _temp26->tl=
0; _temp26;})); prev2=( result2=({ struct Cyc_List_List* _temp27=( struct Cyc_List_List*)
_region_malloc( r4, sizeof( struct Cyc_List_List)); _temp27->hd=( void*)(((
struct _tuple2*) x->hd)[ _check_known_subscript_notnull( 1u, 0)]).f2; _temp27->tl=
0; _temp27;})); for( x= x->tl; x !=  0; x= x->tl){(( struct Cyc_List_List*)
_check_null( prev1))->tl=({ struct Cyc_List_List* _temp28=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp28->hd=( void*)(((
struct _tuple2*) x->hd)[ _check_known_subscript_notnull( 1u, 0)]).f1; _temp28->tl=
0; _temp28;});(( struct Cyc_List_List*) _check_null( prev2))->tl=({ struct Cyc_List_List*
_temp29=( struct Cyc_List_List*) _region_malloc( r4, sizeof( struct Cyc_List_List));
_temp29->hd=( void*)((( struct _tuple2*) x->hd)[ _check_known_subscript_notnull(
1u, 0)]).f2; _temp29->tl= 0; _temp29;}); prev1=(( struct Cyc_List_List*)
_check_null( prev1))->tl; prev2=(( struct Cyc_List_List*) _check_null( prev2))->tl;}
return({ struct _tuple0 _temp30; _temp30.f1= result1; _temp30.f2= result2;
_temp30;});} struct _tuple0 Cyc_List_split( struct Cyc_List_List* x){ return Cyc_List_rsplit(
Cyc_Core_heap_region, Cyc_Core_heap_region, x);} struct _tuple3{ void* f1; void*
f2; void* f3; } ; struct _tuple1 Cyc_List_rsplit3( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct _RegionHandle* r5, struct Cyc_List_List* x){
struct Cyc_List_List* result1; struct Cyc_List_List* prev1; struct Cyc_List_List*
result2; struct Cyc_List_List* prev2; struct Cyc_List_List* result3; struct Cyc_List_List*
prev3; if( x ==  0){ return({ struct _tuple1 _temp31; _temp31.f1= 0; _temp31.f2=
0; _temp31.f3= 0; _temp31;});} prev1=( result1=({ struct Cyc_List_List* _temp32=(
struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp32->hd=( void*)((( struct _tuple3*) x->hd)[ _check_known_subscript_notnull(
1u, 0)]).f1; _temp32->tl= 0; _temp32;})); prev2=( result2=({ struct Cyc_List_List*
_temp33=( struct Cyc_List_List*) _region_malloc( r4, sizeof( struct Cyc_List_List));
_temp33->hd=( void*)((( struct _tuple3*) x->hd)[ _check_known_subscript_notnull(
1u, 0)]).f2; _temp33->tl= 0; _temp33;})); prev3=( result3=({ struct Cyc_List_List*
_temp34=( struct Cyc_List_List*) _region_malloc( r5, sizeof( struct Cyc_List_List));
_temp34->hd=( void*)((( struct _tuple3*) x->hd)[ _check_known_subscript_notnull(
1u, 0)]).f3; _temp34->tl= 0; _temp34;})); for( x= x->tl; x !=  0; x= x->tl){((
struct Cyc_List_List*) _check_null( prev1))->tl=({ struct Cyc_List_List* _temp35=(
struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp35->hd=( void*)((( struct _tuple3*) x->hd)[ _check_known_subscript_notnull(
1u, 0)]).f1; _temp35->tl= 0; _temp35;});(( struct Cyc_List_List*) _check_null(
prev2))->tl=({ struct Cyc_List_List* _temp36=( struct Cyc_List_List*)
_region_malloc( r4, sizeof( struct Cyc_List_List)); _temp36->hd=( void*)(((
struct _tuple3*) x->hd)[ _check_known_subscript_notnull( 1u, 0)]).f2; _temp36->tl=
0; _temp36;});(( struct Cyc_List_List*) _check_null( prev3))->tl=({ struct Cyc_List_List*
_temp37=( struct Cyc_List_List*) _region_malloc( r5, sizeof( struct Cyc_List_List));
_temp37->hd=( void*)((( struct _tuple3*) x->hd)[ _check_known_subscript_notnull(
1u, 0)]).f3; _temp37->tl= 0; _temp37;}); prev1=(( struct Cyc_List_List*)
_check_null( prev1))->tl; prev2=(( struct Cyc_List_List*) _check_null( prev2))->tl;
prev3=(( struct Cyc_List_List*) _check_null( prev3))->tl;} return({ struct
_tuple1 _temp38; _temp38.f1= result1; _temp38.f2= result2; _temp38.f3= result3;
_temp38;});} struct _tuple1 Cyc_List_split3( struct Cyc_List_List* x){ return
Cyc_List_rsplit3( Cyc_Core_heap_region, Cyc_Core_heap_region, Cyc_Core_heap_region,
x);} int Cyc_List_memq( struct Cyc_List_List* l, void* x){ while( l !=  0) { if((
void*) l->hd ==  x){ return 1;} l= l->tl;} return 0;} int Cyc_List_mem( int(*
cmp)( void*, void*), struct Cyc_List_List* l, void* x){ while( l !=  0) { if(
cmp(( void*) l->hd, x) ==  0){ return 1;} l= l->tl;} return 0;} void* Cyc_List_assoc(
struct Cyc_List_List* l, void* x){ while( l !=  0) { if(((( struct _tuple2*) l->hd)[
_check_known_subscript_notnull( 1u, 0)]).f1 ==  x){ return((( struct _tuple2*) l->hd)[
_check_known_subscript_notnull( 1u, 0)]).f2;} l= l->tl;}( int) _throw(( void*)
Cyc_Core_Not_found);} void* Cyc_List_assoc_cmp( int(* cmp)( void*, void*),
struct Cyc_List_List* l, void* x){ while( l !=  0) { if( cmp(((( struct _tuple2*)
l->hd)[ _check_known_subscript_notnull( 1u, 0)]).f1, x) ==  0){ return((( struct
_tuple2*) l->hd)[ _check_known_subscript_notnull( 1u, 0)]).f2;} l= l->tl;}( int)
_throw(( void*) Cyc_Core_Not_found);} struct Cyc_List_List* Cyc_List_delete(
struct Cyc_List_List* l, void* x){ struct Cyc_List_List* _temp39= l; struct Cyc_List_List*
_temp40= l; while( _temp40 !=  0) { if(( void*) _temp40->hd ==  x){ if( _temp39
==  l){ return(( struct Cyc_List_List*) _check_null( l))->tl;} else{(( struct
Cyc_List_List*) _check_null( _temp39))->tl= _temp40->tl; return l;}} _temp39=
_temp40; _temp40= _temp40->tl;}( int) _throw(( void*) Cyc_Core_Not_found);} int
Cyc_List_mem_assoc( struct Cyc_List_List* l, void* x){ while( l !=  0) { if((((
struct _tuple2*) l->hd)[ _check_known_subscript_notnull( 1u, 0)]).f1 ==  x){
return 1;} l= l->tl;} return 0;} struct Cyc_Core_Opt* Cyc_List_check_unique( int(*
cmp)( void*, void*), struct Cyc_List_List* x){ while( x !=  0) { if( x->tl !=  0){
if( cmp(( void*) x->hd,( void*)(( struct Cyc_List_List*) _check_null( x->tl))->hd)
==  0){ return({ struct Cyc_Core_Opt* _temp41=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp41->v=( void*)(( void*) x->hd); _temp41;});}}
x= x->tl;} return 0;} struct _tagged_arr Cyc_List_rto_array( struct
_RegionHandle* r2, struct Cyc_List_List* x){ int s; struct _tagged_arr arr; s=
Cyc_List_length( x); arr=({ unsigned int _temp42=( unsigned int) s; void**
_temp43=( void**) _region_malloc( r2, _check_times( sizeof( void*), _temp42));
struct _tagged_arr _temp45= _tag_arr( _temp43, sizeof( void*),( unsigned int) s);{
unsigned int _temp44= _temp42; unsigned int i; for( i= 0; i <  _temp44; i ++){
_temp43[ i]=( void*)(( struct Cyc_List_List*) _check_null( x))->hd;}}; _temp45;});{
int i= 0; for( 0; i <  s;( ++ i, x= x->tl)){*(( void**) _check_unknown_subscript(
arr, sizeof( void*), i))=( void*)(( struct Cyc_List_List*) _check_null( x))->hd;}}
return arr;} struct _tagged_arr Cyc_List_to_array( struct Cyc_List_List* x){
return Cyc_List_rto_array( Cyc_Core_heap_region, x);} struct Cyc_List_List* Cyc_List_rfrom_array(
struct _RegionHandle* r2, struct _tagged_arr arr){ struct Cyc_List_List* ans= 0;{
int i=( int)( _get_arr_size( arr, sizeof( void*)) -  1); for( 0; i >=  0; -- i){
ans=({ struct Cyc_List_List* _temp46=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp46->hd=( void*)*(( void**)
_check_unknown_subscript( arr, sizeof( void*), i)); _temp46->tl= ans; _temp46;});}}
return ans;} struct Cyc_List_List* Cyc_List_from_array( struct _tagged_arr arr){
return Cyc_List_rfrom_array( Cyc_Core_heap_region, arr);} struct Cyc_List_List*
Cyc_List_rtabulate( struct _RegionHandle* r, int n, void*(* f)( int)){ struct
Cyc_List_List* res= 0;{ int i= 0; for( 0; i <  n; ++ i){ res=({ struct Cyc_List_List*
_temp47=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp47->hd=( void*) f( i); _temp47->tl= res; _temp47;});}} return Cyc_List_imp_rev(
res);} struct Cyc_List_List* Cyc_List_tabulate( int n, void*(* f)( int)){ return
Cyc_List_rtabulate( Cyc_Core_heap_region, n, f);} struct Cyc_List_List* Cyc_List_rtabulate_c(
struct _RegionHandle* r, int n, void*(* f)( void*, int), void* env){ struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i <  n; ++ i){ res=({ struct Cyc_List_List* _temp48=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp48->hd=( void*) f( env, i); _temp48->tl= res; _temp48;});}} return Cyc_List_imp_rev(
res);} struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*, int),
void* env){ return Cyc_List_rtabulate_c( Cyc_Core_heap_region, n, f, env);} int
Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct
Cyc_List_List* l2){ for( 0; l1 !=  0? l2 !=  0: 0;( l1= l1->tl, l2= l2->tl)){
if(( unsigned int) l1 == ( unsigned int) l2){ return 0;}{ int _temp49= cmp((
void*) l1->hd,( void*) l2->hd); if( _temp49 !=  0){ return _temp49;}}} if( l1 != 
0){ return 1;} if( l2 !=  0){ return - 1;} return 0;} int Cyc_List_list_prefix(
int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2){
for( 0; l1 !=  0? l2 !=  0: 0;( l1= l1->tl, l2= l2->tl)){ if(( unsigned int) l1
== ( unsigned int) l2){ return 1;}{ int _temp50= cmp(( void*) l1->hd,( void*) l2->hd);
if( _temp50 !=  0){ return 0;}}} return l1 ==  0;} struct Cyc_List_List* Cyc_List_rfilter_c(
struct _RegionHandle* r2, int(* f)( void*, void*), void* env, struct Cyc_List_List*
l){ if( l ==  0){ return 0;}{ struct Cyc_List_List* result=({ struct Cyc_List_List*
_temp52=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp52->hd=( void*)(( void*) l->hd); _temp52->tl= 0; _temp52;}); struct Cyc_List_List*
end= result; for( 0; l !=  0; l= l->tl){ if( f( env,( void*) l->hd)){(( struct
Cyc_List_List*) _check_null( end))->tl=({ struct Cyc_List_List* _temp51=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp51->hd=(
void*)(( void*) l->hd); _temp51->tl= 0; _temp51;}); end=(( struct Cyc_List_List*)
_check_null( end))->tl;}} return result->tl;}} struct Cyc_List_List* Cyc_List_filter_c(
int(* f)( void*, void*), void* env, struct Cyc_List_List* l){ return Cyc_List_rfilter_c(
Cyc_Core_heap_region, f, env, l);} struct Cyc_List_List* Cyc_List_rfilter(
struct _RegionHandle* r2, int(* f)( void*), struct Cyc_List_List* l){ if( l == 
0){ return 0;}{ struct Cyc_List_List* result=({ struct Cyc_List_List* _temp54=(
struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp54->hd=( void*)(( void*) l->hd); _temp54->tl= 0; _temp54;}); struct Cyc_List_List*
end= result; for( 0; l !=  0; l= l->tl){ if( f(( void*) l->hd)){(( struct Cyc_List_List*)
_check_null( end))->tl=({ struct Cyc_List_List* _temp53=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp53->hd=( void*)(( void*)
l->hd); _temp53->tl= 0; _temp53;}); end=(( struct Cyc_List_List*) _check_null(
end))->tl;}} return result->tl;}} struct Cyc_List_List* Cyc_List_filter( int(* f)(
void*), struct Cyc_List_List* l){ return Cyc_List_rfilter( Cyc_Core_heap_region,
f, l);}
