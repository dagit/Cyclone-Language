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
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_app(
void*(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter( void(*
f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_merge_sort( int(* cmp)( void*, void*), struct Cyc_List_List* x); extern
unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* cmp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_rempty(
struct _RegionHandle* r, int(* cmp)( void*, void*)); extern struct Cyc_Set_Set*
Cyc_Set_singleton( int(* cmp)( void*, void*), void* x); extern struct Cyc_Set_Set*
Cyc_Set_from_list( int(* cmp)( void*, void*), struct Cyc_List_List* l); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
struct Cyc_Set_Set* Cyc_Set_rinsert( struct _RegionHandle* r, struct Cyc_Set_Set*
s, void* elt); extern struct Cyc_Set_Set* Cyc_Set_union_two( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2); extern struct Cyc_Set_Set* Cyc_Set_intersect(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2); extern struct Cyc_Set_Set* Cyc_Set_diff(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2); extern struct Cyc_Set_Set* Cyc_Set_delete(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set*
s); extern int Cyc_Set_is_empty( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_subset( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2); extern int Cyc_Set_setcmp( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2); extern int Cyc_Set_equals( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2); extern struct Cyc_List_List* Cyc_Set_elements( struct
Cyc_Set_Set* s); extern void* Cyc_Set_fold( void*(* f)( void*, void*), struct
Cyc_Set_Set* s, void* accum); extern void* Cyc_Set_fold_c( void*(* f)( void*,
void*, void*), void* env, struct Cyc_Set_Set* s, void* accum); extern void Cyc_Set_app(
void*(* f)( void*), struct Cyc_Set_Set* s); extern void Cyc_Set_iter( void(* f)(
void*), struct Cyc_Set_Set* s); extern void Cyc_Set_iter_c( void(* f)( void*,
void*), void* env, struct Cyc_Set_Set* s); extern unsigned char Cyc_Set_Absent[
11u]; extern void* Cyc_Set_choose( struct Cyc_Set_Set* s); struct Cyc_Set_Set{
int(* cmp)( void*, void*); int cardinality; struct Cyc_List_List* nodes; } ;
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)){ return({ struct
Cyc_Set_Set* _temp0=( struct Cyc_Set_Set*) _cycalloc( sizeof( struct Cyc_Set_Set));
_temp0->cmp= comp; _temp0->cardinality= 0; _temp0->nodes= 0; _temp0;});} struct
Cyc_Set_Set* Cyc_Set_rempty( struct _RegionHandle* rgn, int(* comp)( void*, void*)){
return({ struct Cyc_Set_Set* _temp1=( struct Cyc_Set_Set*) _region_malloc( rgn,
sizeof( struct Cyc_Set_Set)); _temp1->cmp= comp; _temp1->cardinality= 0; _temp1->nodes=
0; _temp1;});} struct Cyc_Set_Set* Cyc_Set_singleton( int(* comp)( void*, void*),
void* x){ return({ struct Cyc_Set_Set* _temp2=( struct Cyc_Set_Set*) _cycalloc(
sizeof( struct Cyc_Set_Set)); _temp2->cmp= comp; _temp2->cardinality= 1; _temp2->nodes=({
struct Cyc_List_List* _temp3=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp3->hd=( void*) x; _temp3->tl= 0; _temp3;}); _temp2;});}
int Cyc_Set_cardinality( struct Cyc_Set_Set* s){ return s->cardinality;} int Cyc_Set_is_empty(
struct Cyc_Set_Set* s){ return s->cardinality ==  0;} static int Cyc_Set_member_b(
int(* cmp)( void*, void*), struct Cyc_List_List* n, void* elt){ while( n !=  0) {
int i= cmp( elt,( void*) n->hd); if( i ==  0){ return 1;} else{ if( i <  0){
return 0;} else{ n= n->tl;}}} return 0;} int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt){ return Cyc_Set_member_b( s->cmp, s->nodes, elt);} static struct
Cyc_List_List* Cyc_Set_insert_b( struct _RegionHandle* rgn, int(* cmp)( void*,
void*), struct Cyc_List_List* n, void* elt){ if( n ==  0){ return({ struct Cyc_List_List*
_temp4=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp4->hd=( void*) elt; _temp4->tl= 0; _temp4;});} else{ int i= cmp( elt,( void*)
n->hd); if( i <  0){ return({ struct Cyc_List_List* _temp5=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp5->hd=( void*) elt;
_temp5->tl= n; _temp5;});} else{ struct Cyc_List_List* result=({ struct Cyc_List_List*
_temp8=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp8->hd=( void*)(( void*) n->hd); _temp8->tl= 0; _temp8;}); struct Cyc_List_List*
prev= result; n= n->tl; while( n !=  0?( i= cmp(( void*) n->hd, elt)) <  0: 0) {((
struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List* _temp6=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp6->hd=( void*)(( void*) n->hd); _temp6->tl= 0; _temp6;}); prev=(( struct
Cyc_List_List*) _check_null( prev))->tl; n= n->tl;}(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp7=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp7->hd=( void*) elt;
_temp7->tl= n; _temp7;}); return result;}}} struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt){ if( Cyc_Set_member( s, elt)){ return s;}
else{ return({ struct Cyc_Set_Set* _temp9=( struct Cyc_Set_Set*) _cycalloc(
sizeof( struct Cyc_Set_Set)); _temp9->cmp= s->cmp; _temp9->cardinality= s->cardinality
+  1; _temp9->nodes= Cyc_Set_insert_b( Cyc_Core_heap_region, s->cmp, s->nodes,
elt); _temp9;});}} struct Cyc_Set_Set* Cyc_Set_rinsert( struct _RegionHandle*
rgn, struct Cyc_Set_Set* s, void* elt){ if( Cyc_Set_member( s, elt)){ return s;}
else{ return({ struct Cyc_Set_Set* _temp10=( struct Cyc_Set_Set*) _region_malloc(
rgn, sizeof( struct Cyc_Set_Set)); _temp10->cmp= s->cmp; _temp10->cardinality= s->cardinality
+  1; _temp10->nodes= Cyc_Set_insert_b( rgn, s->cmp, s->nodes, elt); _temp10;});}}
struct Cyc_Set_Set* Cyc_Set_union_two( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2){ if( s1 ==  s2){ return s1;} if( s1->cardinality ==  0){ return s2;} if( s2->cardinality
==  0){ return s1;}{ struct Cyc_List_List* nodes= 0; int cardinality= 0; int(*
comp)( void*, void*)= s1->cmp; struct Cyc_List_List* x1= s1->nodes; struct Cyc_List_List*
x2= s2->nodes; struct Cyc_List_List* curr= 0; while( x1 !=  0? x2 !=  0: 0) {
int i= comp(( void*) x1->hd,( void*) x2->hd); if( i ==  0){ x2= x2->tl;} else{
if( i <  0){ if( curr ==  0){ nodes=({ struct Cyc_List_List* _temp11=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp11->hd=( void*)((
void*) x1->hd); _temp11->tl= 0; _temp11;}); curr= nodes;} else{(( struct Cyc_List_List*)
_check_null( curr))->tl=({ struct Cyc_List_List* _temp12=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp12->hd=( void*)(( void*) x1->hd);
_temp12->tl= 0; _temp12;}); curr= curr->tl;} x1= x1->tl; ++ cardinality;} else{
if( curr ==  0){ nodes=({ struct Cyc_List_List* _temp13=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp13->hd=( void*)(( void*) x2->hd);
_temp13->tl= 0; _temp13;}); curr= nodes;} else{(( struct Cyc_List_List*)
_check_null( curr))->tl=({ struct Cyc_List_List* _temp14=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp14->hd=( void*)(( void*) x2->hd);
_temp14->tl= 0; _temp14;}); curr= curr->tl;} x2= x2->tl; ++ cardinality;}}} if(
x1 !=  0){ if( curr ==  0){ nodes= x1;} else{(( struct Cyc_List_List*)
_check_null( curr))->tl= x1;} cardinality += Cyc_List_length( x1);} else{ if( x2
!=  0){ if( curr ==  0){ nodes= x2;} else{(( struct Cyc_List_List*) _check_null(
curr))->tl= x2;} cardinality += Cyc_List_length( x2);}} return({ struct Cyc_Set_Set*
_temp15=( struct Cyc_Set_Set*) _cycalloc( sizeof( struct Cyc_Set_Set)); _temp15->cmp=
comp; _temp15->cardinality= cardinality; _temp15->nodes= nodes; _temp15;});}}
static struct Cyc_List_List* Cyc_Set_delete_b( int(* cmp)( void*, void*), struct
Cyc_List_List* n, void* elt){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( n))->hd, elt) ==  0){ return n->tl;}{ struct Cyc_List_List* result=({
struct Cyc_List_List* _temp17=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp17->hd=( void*)(( void*) n->hd); _temp17->tl= 0; _temp17;});
struct Cyc_List_List* prev= result; n= n->tl; while( n !=  0? cmp(( void*) n->hd,
elt) !=  0: 0) {(( struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List*
_temp16=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp16->hd=( void*)(( void*) n->hd); _temp16->tl= 0; _temp16;}); prev=(( struct
Cyc_List_List*) _check_null( prev))->tl; n= n->tl;}(( struct Cyc_List_List*)
_check_null( prev))->tl=(( struct Cyc_List_List*) _check_null( n))->tl; return
result;}} struct Cyc_Set_Set* Cyc_Set_delete( struct Cyc_Set_Set* s, void* elt){
if( Cyc_Set_member( s, elt)){ return({ struct Cyc_Set_Set* _temp18=( struct Cyc_Set_Set*)
_cycalloc( sizeof( struct Cyc_Set_Set)); _temp18->cmp= s->cmp; _temp18->cardinality=
s->cardinality -  1; _temp18->nodes= Cyc_Set_delete_b( s->cmp, s->nodes, elt);
_temp18;});} else{ return s;}} struct Cyc_List_List* Cyc_Set_elements( struct
Cyc_Set_Set* s){ return s->nodes;} void* Cyc_Set_fold( void*(* f)( void*, void*),
struct Cyc_Set_Set* s, void* accum){ struct Cyc_List_List* n= s->nodes; while( n
!=  0) { accum= f(( void*) n->hd, accum); n= n->tl;} return accum;} void* Cyc_Set_fold_c(
void*(* f)( void*, void*, void*), void* env, struct Cyc_Set_Set* s, void* accum){
struct Cyc_List_List* n= s->nodes; while( n !=  0) { accum= f( env,( void*) n->hd,
accum); n= n->tl;} return accum;} void Cyc_Set_app( void*(* f)( void*), struct
Cyc_Set_Set* s){ Cyc_List_app( f, s->nodes);} void Cyc_Set_iter( void(* f)( void*),
struct Cyc_Set_Set* s){ Cyc_List_iter( f, s->nodes);} void Cyc_Set_iter_c( void(*
f)( void*, void*), void* env, struct Cyc_Set_Set* s){ Cyc_List_iter_c( f, env, s->nodes);}
struct Cyc_Set_Set* Cyc_Set_intersect( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2){ if( s1 ==  s2){ return s1;}{ int(* comp)( void*, void*)= s1->cmp; struct
Cyc_List_List* x1= s1->nodes; struct Cyc_List_List* x2= s2->nodes; struct Cyc_List_List*
result= 0; struct Cyc_List_List* prev= 0; int card= 0; if( x1 ==  0){ return s1;}
if( x2 ==  0){ return s2;} while( x1 !=  0? x2 !=  0: 0) { int i= comp(( void*)
x1->hd,( void*) x2->hd); if( i ==  0){ if( result ==  0){ result=({ struct Cyc_List_List*
_temp19=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp19->hd=( void*)(( void*) x1->hd); _temp19->tl= 0; _temp19;}); prev= result;}
else{(( struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List*
_temp20=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp20->hd=( void*)(( void*) x1->hd); _temp20->tl= 0; _temp20;}); prev= prev->tl;}
++ card; x1= x1->tl; x2= x2->tl;} else{ if( i <  0){ x1= x1->tl;} else{ x2= x2->tl;}}}
return({ struct Cyc_Set_Set* _temp21=( struct Cyc_Set_Set*) _cycalloc( sizeof(
struct Cyc_Set_Set)); _temp21->cmp= comp; _temp21->cardinality= card; _temp21->nodes=
result; _temp21;});}} struct Cyc_Set_Set* Cyc_Set_from_list( int(* comp)( void*,
void*), struct Cyc_List_List* x){ struct Cyc_List_List* z= Cyc_List_merge_sort(
comp, x);{ struct Cyc_List_List* y= z; for( 0; y !=  0; y= y->tl){ if( y->tl != 
0? comp(( void*) y->hd,( void*)(( struct Cyc_List_List*) _check_null( y->tl))->hd)
==  0: 0){ y->tl=(( struct Cyc_List_List*) _check_null( y->tl))->tl;}}} return({
struct Cyc_Set_Set* _temp22=( struct Cyc_Set_Set*) _cycalloc( sizeof( struct Cyc_Set_Set));
_temp22->cmp= comp; _temp22->cardinality= Cyc_List_length( z); _temp22->nodes= z;
_temp22;});} int Cyc_Set_subset( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){
int(* comp)( void*, void*)= s1->cmp; struct Cyc_List_List* x1= s1->nodes; struct
Cyc_List_List* x2= s2->nodes; while( 1) { if( x1 ==  0){ return 1;} if( x2 ==  0){
return 0;}{ int i= comp(( void*) x1->hd,( void*) x2->hd); if( i ==  0){ x1= x1->tl;
x2= x2->tl;} else{ if( i >  0){ x2= x2->tl;} else{ return 0;}}}} return 1;}
struct Cyc_Set_Set* Cyc_Set_diff( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){
int(* comp)( void*, void*)= s1->cmp; struct Cyc_List_List* x1= s1->nodes; struct
Cyc_List_List* x2= s2->nodes; int card= s1->cardinality; if( x2 ==  0){ return
s1;} while( x2 !=  0) { void* elt=( void*) x2->hd; if( Cyc_Set_member_b( comp,
x1, elt)){ -- card; x1= Cyc_Set_delete_b( comp, x1, elt);} x2= x2->tl;} return({
struct Cyc_Set_Set* _temp23=( struct Cyc_Set_Set*) _cycalloc( sizeof( struct Cyc_Set_Set));
_temp23->cmp= comp; _temp23->cardinality= card; _temp23->nodes= x1; _temp23;});}
int Cyc_Set_setcmp( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){ if( s1->cardinality
!=  s2->cardinality){ return s1->cardinality -  s2->cardinality;}{ struct Cyc_List_List*
x1= s1->nodes; struct Cyc_List_List* x2= s2->nodes; int(* cmp)( void*, void*)=
s1->cmp; while( x1 !=  0) { int diff= cmp(( void*) x1->hd,( void*)(( struct Cyc_List_List*)
_check_null( x2))->hd); if( diff !=  0){ return diff;} x1= x1->tl; x2= x2->tl;}
return 0;}} int Cyc_Set_equals( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){
return Cyc_Set_setcmp( s1, s2) ==  0;} unsigned char Cyc_Set_Absent[ 11u]="\000\000\000\000Absent";
void* Cyc_Set_choose( struct Cyc_Set_Set* s){ if( s->nodes ==  0){( int) _throw((
void*) Cyc_Set_Absent);} return( void*)(( struct Cyc_List_List*) _check_null( s->nodes))->hd;}
