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

#ifdef CYC_REGION_PROFILE
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
#define _cycalloc(n) _profile_cycalloc(n,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_cycalloc_atomic(n,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FUNCTION__,__LINE__)
#endif

#endif
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern struct _tagged_arr
Cyc_Core_new_string( int); extern unsigned char Cyc_Core_Invalid_argument[ 21u];
struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Rope_Rope_node; extern struct Cyc_Rope_Rope_node* Cyc_Rope_from_string(
struct _tagged_arr); extern struct _tagged_arr Cyc_Rope_to_string( struct Cyc_Rope_Rope_node*);
extern struct Cyc_Rope_Rope_node* Cyc_Rope_concat( struct Cyc_Rope_Rope_node*,
struct Cyc_Rope_Rope_node*); extern struct Cyc_Rope_Rope_node* Cyc_Rope_concata(
struct _tagged_arr); extern struct Cyc_Rope_Rope_node* Cyc_Rope_concatl( struct
Cyc_List_List*); extern unsigned int Cyc_Rope_length( struct Cyc_Rope_Rope_node*);
extern int Cyc_Rope_cmp( struct Cyc_Rope_Rope_node*, struct Cyc_Rope_Rope_node*);
extern unsigned int Cyc_Std_strlen( struct _tagged_arr s); extern int Cyc_Std_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern struct _tagged_arr Cyc_Std_strncpy(
struct _tagged_arr, struct _tagged_arr, unsigned int); static const int Cyc_Rope_String_rope=
0; struct Cyc_Rope_String_rope_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Rope_Array_rope= 1; struct Cyc_Rope_Array_rope_struct{ int
tag; struct _tagged_arr f1; } ; struct Cyc_Rope_Rope_node{ void* v; } ; struct
Cyc_Rope_Rope_node* Cyc_Rope_from_string( struct _tagged_arr s){ return({ struct
Cyc_Rope_Rope_node* _temp0=( struct Cyc_Rope_Rope_node*) _cycalloc( sizeof(
struct Cyc_Rope_Rope_node)); _temp0->v=( void*)(( void*)({ struct Cyc_Rope_String_rope_struct*
_temp1=( struct Cyc_Rope_String_rope_struct*) _cycalloc( sizeof( struct Cyc_Rope_String_rope_struct));
_temp1[ 0]=({ struct Cyc_Rope_String_rope_struct _temp2; _temp2.tag= Cyc_Rope_String_rope;
_temp2.f1= s; _temp2;}); _temp1;})); _temp0;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concat(
struct Cyc_Rope_Rope_node* r1, struct Cyc_Rope_Rope_node* r2){ return({ struct
Cyc_Rope_Rope_node* _temp3=( struct Cyc_Rope_Rope_node*) _cycalloc( sizeof(
struct Cyc_Rope_Rope_node)); _temp3->v=( void*)(( void*)({ struct Cyc_Rope_Array_rope_struct*
_temp4=( struct Cyc_Rope_Array_rope_struct*) _cycalloc( sizeof( struct Cyc_Rope_Array_rope_struct));
_temp4[ 0]=({ struct Cyc_Rope_Array_rope_struct _temp5; _temp5.tag= Cyc_Rope_Array_rope;
_temp5.f1= _tag_arr(({ struct Cyc_Rope_Rope_node** _temp6=( struct Cyc_Rope_Rope_node**)
_cycalloc( sizeof( struct Cyc_Rope_Rope_node*) *  2); _temp6[ 0]= r1; _temp6[ 1]=
r2; _temp6;}), sizeof( struct Cyc_Rope_Rope_node*), 2u); _temp5;}); _temp4;}));
_temp3;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concata( struct _tagged_arr rs){
return({ struct Cyc_Rope_Rope_node* _temp7=( struct Cyc_Rope_Rope_node*)
_cycalloc( sizeof( struct Cyc_Rope_Rope_node)); _temp7->v=( void*)(( void*)({
struct Cyc_Rope_Array_rope_struct* _temp8=( struct Cyc_Rope_Array_rope_struct*)
_cycalloc( sizeof( struct Cyc_Rope_Array_rope_struct)); _temp8[ 0]=({ struct Cyc_Rope_Array_rope_struct
_temp9; _temp9.tag= Cyc_Rope_Array_rope; _temp9.f1= rs; _temp9;}); _temp8;}));
_temp7;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concatl( struct Cyc_List_List* l){
return({ struct Cyc_Rope_Rope_node* _temp10=( struct Cyc_Rope_Rope_node*)
_cycalloc( sizeof( struct Cyc_Rope_Rope_node)); _temp10->v=( void*)(( void*)({
struct Cyc_Rope_Array_rope_struct* _temp11=( struct Cyc_Rope_Array_rope_struct*)
_cycalloc( sizeof( struct Cyc_Rope_Array_rope_struct)); _temp11[ 0]=({ struct
Cyc_Rope_Array_rope_struct _temp12; _temp12.tag= Cyc_Rope_Array_rope; _temp12.f1=({
unsigned int _temp13=( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
l); struct Cyc_Rope_Rope_node** _temp14=( struct Cyc_Rope_Rope_node**) _cycalloc(
_check_times( sizeof( struct Cyc_Rope_Rope_node*), _temp13)); struct _tagged_arr
_temp16= _tag_arr( _temp14, sizeof( struct Cyc_Rope_Rope_node*),( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( l));{ unsigned int _temp15=
_temp13; unsigned int i; for( i= 0; i <  _temp15; i ++){ _temp14[ i]=({ struct
Cyc_Rope_Rope_node* r=( struct Cyc_Rope_Rope_node*)(( struct Cyc_List_List*)
_check_null( l))->hd; l= l->tl; r;});}}; _temp16;}); _temp12;}); _temp11;}));
_temp10;});} unsigned int Cyc_Rope_length( struct Cyc_Rope_Rope_node* r){ void*
_temp17=( void*) r->v; struct _tagged_arr _temp23; struct _tagged_arr _temp25;
_LL19: if(*(( int*) _temp17) ==  Cyc_Rope_String_rope){ _LL24: _temp23=(( struct
Cyc_Rope_String_rope_struct*) _temp17)->f1; goto _LL20;} else{ goto _LL21;}
_LL21: if(*(( int*) _temp17) ==  Cyc_Rope_Array_rope){ _LL26: _temp25=(( struct
Cyc_Rope_Array_rope_struct*) _temp17)->f1; goto _LL22;} else{ goto _LL18;} _LL20:
return( unsigned int) Cyc_Std_strlen( _temp23); _LL22: { unsigned int total= 0;
unsigned int sz= _get_arr_size( _temp25, sizeof( struct Cyc_Rope_Rope_node*));{
unsigned int i= 0; for( 0; i <  sz; i ++){ total += Cyc_Rope_length(*(( struct
Cyc_Rope_Rope_node**) _check_unknown_subscript( _temp25, sizeof( struct Cyc_Rope_Rope_node*),(
int) i)));}} return total;} _LL18:;} static unsigned int Cyc_Rope_flatten_it(
struct _tagged_arr s, unsigned int i, struct Cyc_Rope_Rope_node* r){ void*
_temp27=( void*) r->v; struct _tagged_arr _temp33; struct _tagged_arr _temp35;
_LL29: if(*(( int*) _temp27) ==  Cyc_Rope_String_rope){ _LL34: _temp33=(( struct
Cyc_Rope_String_rope_struct*) _temp27)->f1; goto _LL30;} else{ goto _LL31;}
_LL31: if(*(( int*) _temp27) ==  Cyc_Rope_Array_rope){ _LL36: _temp35=(( struct
Cyc_Rope_Array_rope_struct*) _temp27)->f1; goto _LL32;} else{ goto _LL28;} _LL30: {
unsigned int _temp37= Cyc_Std_strlen( _temp33); Cyc_Std_strncpy(
_tagged_arr_plus( s, sizeof( unsigned char),( int) i), _temp33, _temp37); return
i +  _temp37;} _LL32: { unsigned int _temp38= _get_arr_size( _temp35, sizeof(
struct Cyc_Rope_Rope_node*));{ int j= 0; for( 0; j <  _temp38; j ++){ i= Cyc_Rope_flatten_it(
s, i,*(( struct Cyc_Rope_Rope_node**) _check_unknown_subscript( _temp35, sizeof(
struct Cyc_Rope_Rope_node*), j)));}} return i;} _LL28:;} struct _tagged_arr Cyc_Rope_to_string(
struct Cyc_Rope_Rope_node* r){ struct _tagged_arr s= Cyc_Core_new_string(( int)
Cyc_Rope_length( r)); Cyc_Rope_flatten_it( s, 0, r);( void*)( r->v=( void*)((
void*)({ struct Cyc_Rope_String_rope_struct* _temp39=( struct Cyc_Rope_String_rope_struct*)
_cycalloc( sizeof( struct Cyc_Rope_String_rope_struct)); _temp39[ 0]=({ struct
Cyc_Rope_String_rope_struct _temp40; _temp40.tag= Cyc_Rope_String_rope; _temp40.f1=(
struct _tagged_arr) s; _temp40;}); _temp39;}))); return s;} int Cyc_Rope_cmp(
struct Cyc_Rope_Rope_node* r1, struct Cyc_Rope_Rope_node* r2){ return Cyc_Std_strcmp((
struct _tagged_arr) Cyc_Rope_to_string( r1),( struct _tagged_arr) Cyc_Rope_to_string(
r2));}
