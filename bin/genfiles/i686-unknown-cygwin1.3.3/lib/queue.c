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
  struct _RegionPage *next;
#ifdef CYC_REGION_PROFILE
  unsigned int total_bytes;
  unsigned int free_bytes;
#endif
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
};

extern struct _RegionHandle _new_region();
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
#  if !defined(RUNTIME_CYC)
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
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Queue_Queue; extern int
Cyc_Queue_is_empty( struct Cyc_Queue_Queue*); extern struct Cyc_Queue_Queue* Cyc_Queue_create();
extern void Cyc_Queue_add( struct Cyc_Queue_Queue*, void* x); extern void Cyc_Queue_radd(
struct _RegionHandle*, struct Cyc_Queue_Queue*, void* x); extern unsigned char
Cyc_Queue_Empty[ 10u]; extern void* Cyc_Queue_take( struct Cyc_Queue_Queue*);
extern void* Cyc_Queue_peek( struct Cyc_Queue_Queue*); extern void Cyc_Queue_clear(
struct Cyc_Queue_Queue*); extern void Cyc_Queue_remove( struct Cyc_Queue_Queue*,
void*); extern int Cyc_Queue_length( struct Cyc_Queue_Queue*); extern void Cyc_Queue_iter(
void(* f)( void*), struct Cyc_Queue_Queue*); extern void Cyc_Queue_app( void*(*
f)( void*), struct Cyc_Queue_Queue*); struct Cyc_Queue_Queue{ struct Cyc_List_List*
front; struct Cyc_List_List* rear; unsigned int len; } ; int Cyc_Queue_is_empty(
struct Cyc_Queue_Queue* q){ return q->front ==  0;} unsigned char Cyc_Queue_Empty[
10u]="\000\000\000\000Empty"; struct Cyc_Queue_Queue* Cyc_Queue_create(){ return({
struct Cyc_Queue_Queue* _temp0=( struct Cyc_Queue_Queue*) _cycalloc( sizeof(
struct Cyc_Queue_Queue)); _temp0->front= 0; _temp0->rear= 0; _temp0->len= 0;
_temp0;});} void Cyc_Queue_radd( struct _RegionHandle* r, struct Cyc_Queue_Queue*
q, void* x){ struct Cyc_List_List* cell=({ struct Cyc_List_List* _temp1=( struct
Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List)); _temp1->hd=(
void*) x; _temp1->tl= 0; _temp1;}); if( q->front ==  0){ q->front= cell; q->rear=
cell;} else{(( struct Cyc_List_List*) _check_null( q->rear))->tl= cell; q->rear=
cell;} q->len ++;} void Cyc_Queue_add( struct Cyc_Queue_Queue* q, void* x){ Cyc_Queue_radd(
Cyc_Core_heap_region, q, x);} void* Cyc_Queue_take( struct Cyc_Queue_Queue* q){
if( q->front ==  0){( int) _throw(( void*) Cyc_Queue_Empty);} else{ void* _temp2=(
void*)(( struct Cyc_List_List*) _check_null( q->front))->hd; q->front=(( struct
Cyc_List_List*) _check_null( q->front))->tl; if( q->front ==  0){ q->rear= 0;} q->len
--; return _temp2;}} void* Cyc_Queue_peek( struct Cyc_Queue_Queue* q){ if( q->front
==  0){( int) _throw(( void*) Cyc_Queue_Empty);} else{ return( void*)(( struct
Cyc_List_List*) _check_null( q->front))->hd;}} void Cyc_Queue_clear( struct Cyc_Queue_Queue*
q){ q->front= 0; q->rear= 0; q->len= 0;} void Cyc_Queue_remove( struct Cyc_Queue_Queue*
q, void* v){ struct Cyc_List_List* x; struct Cyc_List_List* y; for(( x= q->front,
y= 0); x !=  0;( y= x, x= x->tl)){ if(( void*) x->hd ==  v){ if( q->front ==  x){
q->front= x->tl;} else{(( struct Cyc_List_List*) _check_null( y))->tl= x->tl;}
if( q->rear ==  x){ q->rear= y;} break;}}} int Cyc_Queue_length( struct Cyc_Queue_Queue*
q){ return( int) q->len;} void Cyc_Queue_iter( void(* f)( void*), struct Cyc_Queue_Queue*
q){ struct Cyc_List_List* x= q->front; for( 0; x !=  0; x= x->tl){ f(( void*) x->hd);}}
void Cyc_Queue_app( void*(* f)( void*), struct Cyc_Queue_Queue* q){ struct Cyc_List_List*
x= q->front; for( 0; x !=  0; x= x->tl){ f(( void*) x->hd);}}
