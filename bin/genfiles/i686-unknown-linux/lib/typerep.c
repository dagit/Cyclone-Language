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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); static const int Cyc_Typerep_Var=
0; struct Cyc_Typerep_Var_struct{ int tag; struct _tagged_arr* f1; } ; static
const int Cyc_Typerep_Int= 1; struct Cyc_Typerep_Int_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Typerep_Float= 0; static const int Cyc_Typerep_Double=
1; static const int Cyc_Typerep_ThinPtr= 2; struct Cyc_Typerep_ThinPtr_struct{
int tag; unsigned int f1; void* f2; } ; static const int Cyc_Typerep_FatPtr= 3;
struct Cyc_Typerep_FatPtr_struct{ int tag; void* f1; } ; static const int Cyc_Typerep_Tuple=
4; struct _tuple0{ unsigned int f1; void* f2; } ; struct Cyc_Typerep_Tuple_struct{
int tag; unsigned int f1; struct _tagged_arr f2; } ; static const int Cyc_Typerep_TUnion=
5; struct Cyc_Typerep_TUnion_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Typerep_XTUnion= 6; struct _tuple1{ struct _tagged_arr f1; void*
f2; } ; struct Cyc_Typerep_XTUnion_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Typerep_Union= 7; struct Cyc_Typerep_Union_struct{ int tag;
struct _tagged_arr f1; } ; static const int Cyc_Typerep_Forall= 8; struct Cyc_Typerep_Forall_struct{
int tag; struct _tagged_arr f1; void** f2; } ; static const int Cyc_Typerep_App=
9; struct Cyc_Typerep_App_struct{ int tag; void* f1; struct _tagged_arr f2; } ;
extern void Cyc_Typerep_print_typestruct( void* rep); extern void Cyc_Typerep_xmlize_typestruct(
void* rep); extern void* Cyc_Typerep_normalize( void* ts); extern void* Cyc_Typerep_get_unionbranch(
unsigned int tag, struct _tagged_arr l); extern void* Cyc_Typerep_get_xtunionbranch(
struct _tagged_arr tag, struct _tagged_arr l); unsigned int Cyc_Typerep_size_type(
void* rep); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE; extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_printf( struct _tagged_arr fmt, struct _tagged_arr);
static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{ int
tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[
8u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* cmp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct
Cyc_Dict_Dict* d, void* k, void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict*
d, void* k); extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); void* Cyc_Typerep_get_unionbranch( unsigned int tag, struct
_tagged_arr l){{ int i= 0; for( 0; i <  _get_arr_size( l, sizeof( struct _tuple0*));
i ++){ void* _temp2; unsigned int _temp4; struct _tuple0 _temp0=*(*(( struct
_tuple0**) _check_unknown_subscript( l, sizeof( struct _tuple0*), i))); _LL5:
_temp4= _temp0.f1; goto _LL3; _LL3: _temp2= _temp0.f2; goto _LL1; _LL1: if(
_temp4 ==  tag){ return _temp2;}}}( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp6=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp6[ 0]=({ struct Cyc_Core_Failure_struct _temp7; _temp7.tag= Cyc_Core_Failure;
_temp7.f1= _tag_arr("Could not find tag in TUnion", sizeof( unsigned char), 29u);
_temp7;}); _temp6;}));} void* Cyc_Typerep_get_xtunionbranch( struct _tagged_arr
tag, struct _tagged_arr l){{ int i= 0; for( 0; i <  _get_arr_size( l, sizeof(
struct _tuple1*)); i ++){ void* _temp10; struct _tagged_arr _temp12; struct
_tuple1 _temp8=*(*(( struct _tuple1**) _check_unknown_subscript( l, sizeof(
struct _tuple1*), i))); _LL13: _temp12= _temp8.f1; goto _LL11; _LL11: _temp10=
_temp8.f2; goto _LL9; _LL9: if( Cyc_Std_strcmp( _temp12, tag) ==  0){ return
_temp10;}}}( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp14=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp14[ 0]=({ struct Cyc_Core_Failure_struct _temp15; _temp15.tag= Cyc_Core_Failure;
_temp15.f1= _tag_arr("Could not find tag in XTUnion", sizeof( unsigned char), 30u);
_temp15;}); _temp14;}));} unsigned int Cyc_Typerep_size_type( void* rep){ void*
_temp16= rep; unsigned int _temp42; unsigned int _temp44; struct _tagged_arr
_temp46; void** _temp48; void* _temp50; _LL18: if(( unsigned int) _temp16 >  2u?*((
int*) _temp16) ==  Cyc_Typerep_Int: 0){ _LL43: _temp42=(( struct Cyc_Typerep_Int_struct*)
_temp16)->f1; goto _LL19;} else{ goto _LL20;} _LL20: if( _temp16 == ( void*) Cyc_Typerep_Float){
goto _LL21;} else{ goto _LL22;} _LL22: if( _temp16 == ( void*) Cyc_Typerep_Double){
goto _LL23;} else{ goto _LL24;} _LL24: if(( unsigned int) _temp16 >  2u?*(( int*)
_temp16) ==  Cyc_Typerep_ThinPtr: 0){ goto _LL25;} else{ goto _LL26;} _LL26: if((
unsigned int) _temp16 >  2u?*(( int*) _temp16) ==  Cyc_Typerep_FatPtr: 0){ goto
_LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp16 >  2u?*(( int*)
_temp16) ==  Cyc_Typerep_Tuple: 0){ _LL45: _temp44=(( struct Cyc_Typerep_Tuple_struct*)
_temp16)->f1; goto _LL29;} else{ goto _LL30;} _LL30: if(( unsigned int) _temp16
>  2u?*(( int*) _temp16) ==  Cyc_Typerep_TUnion: 0){ goto _LL31;} else{ goto
_LL32;} _LL32: if(( unsigned int) _temp16 >  2u?*(( int*) _temp16) ==  Cyc_Typerep_XTUnion:
0){ goto _LL33;} else{ goto _LL34;} _LL34: if(( unsigned int) _temp16 >  2u?*((
int*) _temp16) ==  Cyc_Typerep_Union: 0){ _LL47: _temp46=(( struct Cyc_Typerep_Union_struct*)
_temp16)->f1; goto _LL35;} else{ goto _LL36;} _LL36: if(( unsigned int) _temp16
>  2u?*(( int*) _temp16) ==  Cyc_Typerep_Var: 0){ goto _LL37;} else{ goto _LL38;}
_LL38: if(( unsigned int) _temp16 >  2u?*(( int*) _temp16) ==  Cyc_Typerep_Forall:
0){ _LL49: _temp48=(( struct Cyc_Typerep_Forall_struct*) _temp16)->f2; goto
_LL39;} else{ goto _LL40;} _LL40: if(( unsigned int) _temp16 >  2u?*(( int*)
_temp16) ==  Cyc_Typerep_App: 0){ _LL51: _temp50=( void*)(( struct Cyc_Typerep_App_struct*)
_temp16)->f1; goto _LL41;} else{ goto _LL17;} _LL19: return _temp42 >>  3; _LL21:
return sizeof( float); _LL23: return sizeof( double); _LL25: return sizeof( void*);
_LL27: return sizeof( struct _tagged_arr); _LL29: return _temp44; _LL31: return
sizeof( void*); _LL33: return sizeof( void*); _LL35: { int max= 0;{ int i= 0;
for( 0; i <  _get_arr_size( _temp46, sizeof( void**)); i ++){ unsigned int
_temp52= Cyc_Typerep_size_type(*(*(( void***) _check_unknown_subscript( _temp46,
sizeof( void**), i)))); max=( int)( max <  _temp52? _temp52:( unsigned int) max);}}
return( unsigned int) max;} _LL37: return sizeof( void*); _LL39: return Cyc_Typerep_size_type(*
_temp48); _LL41: return Cyc_Typerep_size_type( _temp50); _LL17:;} void Cyc_Typerep_print_typestruct(
void* rep){ void* _temp53= rep; struct _tagged_arr* _temp79; unsigned int
_temp81; void* _temp83; unsigned int _temp85; void* _temp87; struct _tagged_arr
_temp89; unsigned int _temp91; struct _tagged_arr _temp93; struct _tagged_arr
_temp95; struct _tagged_arr _temp97; void** _temp99; struct _tagged_arr _temp101;
struct _tagged_arr _temp103; void* _temp105; _LL55: if(( unsigned int) _temp53 > 
2u?*(( int*) _temp53) ==  Cyc_Typerep_Var: 0){ _LL80: _temp79=(( struct Cyc_Typerep_Var_struct*)
_temp53)->f1; goto _LL56;} else{ goto _LL57;} _LL57: if(( unsigned int) _temp53
>  2u?*(( int*) _temp53) ==  Cyc_Typerep_Int: 0){ _LL82: _temp81=(( struct Cyc_Typerep_Int_struct*)
_temp53)->f1; goto _LL58;} else{ goto _LL59;} _LL59: if( _temp53 == ( void*) Cyc_Typerep_Float){
goto _LL60;} else{ goto _LL61;} _LL61: if( _temp53 == ( void*) Cyc_Typerep_Double){
goto _LL62;} else{ goto _LL63;} _LL63: if(( unsigned int) _temp53 >  2u?*(( int*)
_temp53) ==  Cyc_Typerep_ThinPtr: 0){ _LL86: _temp85=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp53)->f1; goto _LL84; _LL84: _temp83=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp53)->f2; goto _LL64;} else{ goto _LL65;} _LL65: if(( unsigned int) _temp53
>  2u?*(( int*) _temp53) ==  Cyc_Typerep_FatPtr: 0){ _LL88: _temp87=( void*)((
struct Cyc_Typerep_FatPtr_struct*) _temp53)->f1; goto _LL66;} else{ goto _LL67;}
_LL67: if(( unsigned int) _temp53 >  2u?*(( int*) _temp53) ==  Cyc_Typerep_Tuple:
0){ _LL92: _temp91=(( struct Cyc_Typerep_Tuple_struct*) _temp53)->f1; goto _LL90;
_LL90: _temp89=(( struct Cyc_Typerep_Tuple_struct*) _temp53)->f2; goto _LL68;}
else{ goto _LL69;} _LL69: if(( unsigned int) _temp53 >  2u?*(( int*) _temp53) == 
Cyc_Typerep_TUnion: 0){ _LL94: _temp93=(( struct Cyc_Typerep_TUnion_struct*)
_temp53)->f1; goto _LL70;} else{ goto _LL71;} _LL71: if(( unsigned int) _temp53
>  2u?*(( int*) _temp53) ==  Cyc_Typerep_XTUnion: 0){ _LL96: _temp95=(( struct
Cyc_Typerep_XTUnion_struct*) _temp53)->f1; goto _LL72;} else{ goto _LL73;} _LL73:
if(( unsigned int) _temp53 >  2u?*(( int*) _temp53) ==  Cyc_Typerep_Union: 0){
_LL98: _temp97=(( struct Cyc_Typerep_Union_struct*) _temp53)->f1; goto _LL74;}
else{ goto _LL75;} _LL75: if(( unsigned int) _temp53 >  2u?*(( int*) _temp53) == 
Cyc_Typerep_Forall: 0){ _LL102: _temp101=(( struct Cyc_Typerep_Forall_struct*)
_temp53)->f1; goto _LL100; _LL100: _temp99=(( struct Cyc_Typerep_Forall_struct*)
_temp53)->f2; goto _LL76;} else{ goto _LL77;} _LL77: if(( unsigned int) _temp53
>  2u?*(( int*) _temp53) ==  Cyc_Typerep_App: 0){ _LL106: _temp105=( void*)((
struct Cyc_Typerep_App_struct*) _temp53)->f1; goto _LL104; _LL104: _temp103=((
struct Cyc_Typerep_App_struct*) _temp53)->f2; goto _LL78;} else{ goto _LL54;}
_LL56:({ struct Cyc_Std_String_pa_struct _temp108; _temp108.tag= Cyc_Std_String_pa;
_temp108.f1=( struct _tagged_arr)* _temp79;{ void* _temp107[ 1u]={& _temp108};
Cyc_Std_printf( _tag_arr("Var(%s)", sizeof( unsigned char), 8u), _tag_arr(
_temp107, sizeof( void*), 1u));}}); goto _LL54; _LL58:({ struct Cyc_Std_Int_pa_struct
_temp110; _temp110.tag= Cyc_Std_Int_pa; _temp110.f1=( int) _temp81;{ void*
_temp109[ 1u]={& _temp110}; Cyc_Std_printf( _tag_arr("Int(%d)", sizeof(
unsigned char), 8u), _tag_arr( _temp109, sizeof( void*), 1u));}}); goto _LL54;
_LL60:({ void* _temp111[ 0u]={}; Cyc_Std_printf( _tag_arr("Float", sizeof(
unsigned char), 6u), _tag_arr( _temp111, sizeof( void*), 0u));}); goto _LL54;
_LL62:({ void* _temp112[ 0u]={}; Cyc_Std_printf( _tag_arr("Double", sizeof(
unsigned char), 7u), _tag_arr( _temp112, sizeof( void*), 0u));}); goto _LL54;
_LL64:({ struct Cyc_Std_Int_pa_struct _temp114; _temp114.tag= Cyc_Std_Int_pa;
_temp114.f1=( int) _temp85;{ void* _temp113[ 1u]={& _temp114}; Cyc_Std_printf(
_tag_arr("ThinPtr(%d,", sizeof( unsigned char), 12u), _tag_arr( _temp113,
sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct( _temp83);({ void*
_temp115[ 0u]={}; Cyc_Std_printf( _tag_arr(")", sizeof( unsigned char), 2u),
_tag_arr( _temp115, sizeof( void*), 0u));}); goto _LL54; _LL66:({ void* _temp116[
0u]={}; Cyc_Std_printf( _tag_arr("FatPtr(", sizeof( unsigned char), 8u),
_tag_arr( _temp116, sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct(
_temp87);({ void* _temp117[ 0u]={}; Cyc_Std_printf( _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr( _temp117, sizeof( void*), 0u));}); goto _LL54;
_LL68:({ struct Cyc_Std_Int_pa_struct _temp119; _temp119.tag= Cyc_Std_Int_pa;
_temp119.f1=( int) _temp91;{ void* _temp118[ 1u]={& _temp119}; Cyc_Std_printf(
_tag_arr("Tuple(%d,[", sizeof( unsigned char), 11u), _tag_arr( _temp118, sizeof(
void*), 1u));}});{ int i= 0; for( 0; i <  _get_arr_size( _temp89, sizeof( struct
_tuple0*)); i ++){ void* _temp122; unsigned int _temp124; struct _tuple0
_temp120=*(*(( struct _tuple0**) _check_unknown_subscript( _temp89, sizeof(
struct _tuple0*), i))); _LL125: _temp124= _temp120.f1; goto _LL123; _LL123:
_temp122= _temp120.f2; goto _LL121; _LL121:({ struct Cyc_Std_Int_pa_struct
_temp127; _temp127.tag= Cyc_Std_Int_pa; _temp127.f1=( int) _temp124;{ void*
_temp126[ 1u]={& _temp127}; Cyc_Std_printf( _tag_arr("$(%d,", sizeof(
unsigned char), 6u), _tag_arr( _temp126, sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct(
_temp122);({ struct Cyc_Std_Int_pa_struct _temp129; _temp129.tag= Cyc_Std_Int_pa;
_temp129.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size( _temp89, sizeof(
struct _tuple0*)) -  1?',':' ')));{ void* _temp128[ 1u]={& _temp129}; Cyc_Std_printf(
_tag_arr(")%c", sizeof( unsigned char), 4u), _tag_arr( _temp128, sizeof( void*),
1u));}});}}({ void* _temp130[ 0u]={}; Cyc_Std_printf( _tag_arr(" ]", sizeof(
unsigned char), 3u), _tag_arr( _temp130, sizeof( void*), 0u));}); goto _LL54;
_LL70:({ void* _temp131[ 0u]={}; Cyc_Std_printf( _tag_arr("TUnion(", sizeof(
unsigned char), 8u), _tag_arr( _temp131, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp93, sizeof( struct _tuple0*)); i ++){ void*
_temp134; unsigned int _temp136; struct _tuple0 _temp132=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp93, sizeof( struct _tuple0*), i))); _LL137:
_temp136= _temp132.f1; goto _LL135; _LL135: _temp134= _temp132.f2; goto _LL133;
_LL133:({ struct Cyc_Std_Int_pa_struct _temp139; _temp139.tag= Cyc_Std_Int_pa;
_temp139.f1=( int) _temp136;{ void* _temp138[ 1u]={& _temp139}; Cyc_Std_printf(
_tag_arr("$(%d,", sizeof( unsigned char), 6u), _tag_arr( _temp138, sizeof( void*),
1u));}}); Cyc_Typerep_print_typestruct( _temp134);({ struct Cyc_Std_Int_pa_struct
_temp141; _temp141.tag= Cyc_Std_Int_pa; _temp141.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp93, sizeof( struct _tuple0*)) -  1?',':' ')));{ void*
_temp140[ 1u]={& _temp141}; Cyc_Std_printf( _tag_arr(")%c", sizeof(
unsigned char), 4u), _tag_arr( _temp140, sizeof( void*), 1u));}});}}({ void*
_temp142[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof( unsigned char), 3u),
_tag_arr( _temp142, sizeof( void*), 0u));}); goto _LL54; _LL72:({ void* _temp143[
0u]={}; Cyc_Std_printf( _tag_arr("XTUnion(", sizeof( unsigned char), 9u),
_tag_arr( _temp143, sizeof( void*), 0u));});{ int i= 0; for( 0; i < 
_get_arr_size( _temp95, sizeof( struct _tuple1*)); i ++){ void* _temp146; struct
_tagged_arr _temp148; struct _tuple1 _temp144=*(*(( struct _tuple1**)
_check_unknown_subscript( _temp95, sizeof( struct _tuple1*), i))); _LL149:
_temp148= _temp144.f1; goto _LL147; _LL147: _temp146= _temp144.f2; goto _LL145;
_LL145:({ struct Cyc_Std_String_pa_struct _temp151; _temp151.tag= Cyc_Std_String_pa;
_temp151.f1=( struct _tagged_arr) _tagged_arr_plus( _temp148, sizeof(
unsigned char), 4);{ void* _temp150[ 1u]={& _temp151}; Cyc_Std_printf( _tag_arr("$(%s,",
sizeof( unsigned char), 6u), _tag_arr( _temp150, sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct(
_temp146);({ struct Cyc_Std_Int_pa_struct _temp153; _temp153.tag= Cyc_Std_Int_pa;
_temp153.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size( _temp95, sizeof(
struct _tuple1*)) -  1?',':' ')));{ void* _temp152[ 1u]={& _temp153}; Cyc_Std_printf(
_tag_arr(")%c", sizeof( unsigned char), 4u), _tag_arr( _temp152, sizeof( void*),
1u));}});}}({ void* _temp154[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof(
unsigned char), 3u), _tag_arr( _temp154, sizeof( void*), 0u));}); goto _LL54;
_LL74:({ void* _temp155[ 0u]={}; Cyc_Std_printf( _tag_arr("Union(", sizeof(
unsigned char), 7u), _tag_arr( _temp155, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp97, sizeof( void**)); i ++){ Cyc_Typerep_print_typestruct(*(*((
void***) _check_unknown_subscript( _temp97, sizeof( void**), i))));({ struct Cyc_Std_Int_pa_struct
_temp157; _temp157.tag= Cyc_Std_Int_pa; _temp157.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp97, sizeof( void**)) -  1?',':' ')));{ void* _temp156[
1u]={& _temp157}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char), 3u),
_tag_arr( _temp156, sizeof( void*), 1u));}});}}({ void* _temp158[ 0u]={}; Cyc_Std_printf(
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr( _temp158, sizeof( void*), 0u));});
goto _LL54; _LL76:({ void* _temp159[ 0u]={}; Cyc_Std_printf( _tag_arr("Forall([",
sizeof( unsigned char), 9u), _tag_arr( _temp159, sizeof( void*), 0u));});{ int i=
0; for( 0; i <  _get_arr_size( _temp101, sizeof( struct _tagged_arr*)); i ++){({
void* _temp160[ 0u]={}; Cyc_Std_printf(*(*(( struct _tagged_arr**)
_check_unknown_subscript( _temp101, sizeof( struct _tagged_arr*), i))), _tag_arr(
_temp160, sizeof( void*), 0u));});({ struct Cyc_Std_Int_pa_struct _temp162;
_temp162.tag= Cyc_Std_Int_pa; _temp162.f1=( int)(( unsigned int)(( int)( i != 
_get_arr_size( _temp101, sizeof( struct _tagged_arr*)) -  1?',':' ')));{ void*
_temp161[ 1u]={& _temp162}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char),
3u), _tag_arr( _temp161, sizeof( void*), 1u));}});}}({ void* _temp163[ 0u]={};
Cyc_Std_printf( _tag_arr("],", sizeof( unsigned char), 3u), _tag_arr( _temp163,
sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct(* _temp99);({ void*
_temp164[ 0u]={}; Cyc_Std_printf( _tag_arr(")", sizeof( unsigned char), 2u),
_tag_arr( _temp164, sizeof( void*), 0u));}); goto _LL54; _LL78:({ void* _temp165[
0u]={}; Cyc_Std_printf( _tag_arr("App(", sizeof( unsigned char), 5u), _tag_arr(
_temp165, sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct( _temp105);({
void* _temp166[ 0u]={}; Cyc_Std_printf( _tag_arr(",[", sizeof( unsigned char), 3u),
_tag_arr( _temp166, sizeof( void*), 0u));});{ int i= 0; for( 0; i < 
_get_arr_size( _temp103, sizeof( void*)); i ++){ Cyc_Typerep_print_typestruct(*((
void**) _check_unknown_subscript( _temp103, sizeof( void*), i)));({ struct Cyc_Std_Int_pa_struct
_temp168; _temp168.tag= Cyc_Std_Int_pa; _temp168.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp103, sizeof( void*)) -  1?',':' ')));{ void* _temp167[
1u]={& _temp168}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char), 3u),
_tag_arr( _temp167, sizeof( void*), 1u));}});}}({ void* _temp169[ 0u]={}; Cyc_Std_printf(
_tag_arr("])", sizeof( unsigned char), 3u), _tag_arr( _temp169, sizeof( void*),
0u));}); goto _LL54; _LL54:;} void Cyc_Typerep_xmlize_typestruct( void* rep){
void* _temp170= rep; struct _tagged_arr* _temp196; unsigned int _temp198; void*
_temp200; unsigned int _temp202; void* _temp204; struct _tagged_arr _temp206;
unsigned int _temp208; struct _tagged_arr _temp210; struct _tagged_arr _temp212;
struct _tagged_arr _temp214; void** _temp216; struct _tagged_arr _temp218;
struct _tagged_arr _temp220; void* _temp222; _LL172: if(( unsigned int) _temp170
>  2u?*(( int*) _temp170) ==  Cyc_Typerep_Var: 0){ _LL197: _temp196=(( struct
Cyc_Typerep_Var_struct*) _temp170)->f1; goto _LL173;} else{ goto _LL174;} _LL174:
if(( unsigned int) _temp170 >  2u?*(( int*) _temp170) ==  Cyc_Typerep_Int: 0){
_LL199: _temp198=(( struct Cyc_Typerep_Int_struct*) _temp170)->f1; goto _LL175;}
else{ goto _LL176;} _LL176: if( _temp170 == ( void*) Cyc_Typerep_Float){ goto
_LL177;} else{ goto _LL178;} _LL178: if( _temp170 == ( void*) Cyc_Typerep_Double){
goto _LL179;} else{ goto _LL180;} _LL180: if(( unsigned int) _temp170 >  2u?*((
int*) _temp170) ==  Cyc_Typerep_ThinPtr: 0){ _LL203: _temp202=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp170)->f1; goto _LL201; _LL201: _temp200=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp170)->f2; goto _LL181;} else{ goto _LL182;} _LL182: if(( unsigned int)
_temp170 >  2u?*(( int*) _temp170) ==  Cyc_Typerep_FatPtr: 0){ _LL205: _temp204=(
void*)(( struct Cyc_Typerep_FatPtr_struct*) _temp170)->f1; goto _LL183;} else{
goto _LL184;} _LL184: if(( unsigned int) _temp170 >  2u?*(( int*) _temp170) == 
Cyc_Typerep_Tuple: 0){ _LL209: _temp208=(( struct Cyc_Typerep_Tuple_struct*)
_temp170)->f1; goto _LL207; _LL207: _temp206=(( struct Cyc_Typerep_Tuple_struct*)
_temp170)->f2; goto _LL185;} else{ goto _LL186;} _LL186: if(( unsigned int)
_temp170 >  2u?*(( int*) _temp170) ==  Cyc_Typerep_TUnion: 0){ _LL211: _temp210=((
struct Cyc_Typerep_TUnion_struct*) _temp170)->f1; goto _LL187;} else{ goto
_LL188;} _LL188: if(( unsigned int) _temp170 >  2u?*(( int*) _temp170) ==  Cyc_Typerep_XTUnion:
0){ _LL213: _temp212=(( struct Cyc_Typerep_XTUnion_struct*) _temp170)->f1; goto
_LL189;} else{ goto _LL190;} _LL190: if(( unsigned int) _temp170 >  2u?*(( int*)
_temp170) ==  Cyc_Typerep_Union: 0){ _LL215: _temp214=(( struct Cyc_Typerep_Union_struct*)
_temp170)->f1; goto _LL191;} else{ goto _LL192;} _LL192: if(( unsigned int)
_temp170 >  2u?*(( int*) _temp170) ==  Cyc_Typerep_Forall: 0){ _LL219: _temp218=((
struct Cyc_Typerep_Forall_struct*) _temp170)->f1; goto _LL217; _LL217: _temp216=((
struct Cyc_Typerep_Forall_struct*) _temp170)->f2; goto _LL193;} else{ goto
_LL194;} _LL194: if(( unsigned int) _temp170 >  2u?*(( int*) _temp170) ==  Cyc_Typerep_App:
0){ _LL223: _temp222=( void*)(( struct Cyc_Typerep_App_struct*) _temp170)->f1;
goto _LL221; _LL221: _temp220=(( struct Cyc_Typerep_App_struct*) _temp170)->f2;
goto _LL195;} else{ goto _LL171;} _LL173:({ struct Cyc_Std_String_pa_struct
_temp225; _temp225.tag= Cyc_Std_String_pa; _temp225.f1=( struct _tagged_arr)*
_temp196;{ void* _temp224[ 1u]={& _temp225}; Cyc_Std_printf( _tag_arr("<Var name=\"%s\"/>",
sizeof( unsigned char), 17u), _tag_arr( _temp224, sizeof( void*), 1u));}}); goto
_LL171; _LL175:({ struct Cyc_Std_Int_pa_struct _temp227; _temp227.tag= Cyc_Std_Int_pa;
_temp227.f1=( int) _temp198;{ void* _temp226[ 1u]={& _temp227}; Cyc_Std_printf(
_tag_arr("<Int sz=\"%d\"/>", sizeof( unsigned char), 15u), _tag_arr( _temp226,
sizeof( void*), 1u));}}); goto _LL171; _LL177:({ void* _temp228[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Float/>", sizeof( unsigned char), 9u), _tag_arr( _temp228, sizeof(
void*), 0u));}); goto _LL171; _LL179:({ void* _temp229[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Double/>", sizeof( unsigned char), 10u), _tag_arr( _temp229, sizeof(
void*), 0u));}); goto _LL171; _LL181:({ struct Cyc_Std_Int_pa_struct _temp231;
_temp231.tag= Cyc_Std_Int_pa; _temp231.f1=( int) _temp202;{ void* _temp230[ 1u]={&
_temp231}; Cyc_Std_printf( _tag_arr("<ThinPtr sz=\"%d\">", sizeof( unsigned char),
18u), _tag_arr( _temp230, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp200);({ void* _temp232[ 0u]={}; Cyc_Std_printf( _tag_arr("</ThinPtr",
sizeof( unsigned char), 10u), _tag_arr( _temp232, sizeof( void*), 0u));}); goto
_LL171; _LL183:({ void* _temp233[ 0u]={}; Cyc_Std_printf( _tag_arr("<FatPtr>",
sizeof( unsigned char), 9u), _tag_arr( _temp233, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct(
_temp204);({ void* _temp234[ 0u]={}; Cyc_Std_printf( _tag_arr("</FatPtr>",
sizeof( unsigned char), 10u), _tag_arr( _temp234, sizeof( void*), 0u));}); goto
_LL171; _LL185:({ struct Cyc_Std_Int_pa_struct _temp236; _temp236.tag= Cyc_Std_Int_pa;
_temp236.f1=( int) _temp208;{ void* _temp235[ 1u]={& _temp236}; Cyc_Std_printf(
_tag_arr("<Tuple sz=\"%d\">", sizeof( unsigned char), 16u), _tag_arr( _temp235,
sizeof( void*), 1u));}});{ int i= 0; for( 0; i <  _get_arr_size( _temp206,
sizeof( struct _tuple0*)); i ++){ void* _temp239; unsigned int _temp241; struct
_tuple0 _temp237=*(*(( struct _tuple0**) _check_unknown_subscript( _temp206,
sizeof( struct _tuple0*), i))); _LL242: _temp241= _temp237.f1; goto _LL240;
_LL240: _temp239= _temp237.f2; goto _LL238; _LL238:({ struct Cyc_Std_Int_pa_struct
_temp244; _temp244.tag= Cyc_Std_Int_pa; _temp244.f1=( int) _temp241;{ void*
_temp243[ 1u]={& _temp244}; Cyc_Std_printf( _tag_arr("<Component ofs=\"%d\">",
sizeof( unsigned char), 21u), _tag_arr( _temp243, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp239);({ void* _temp245[ 0u]={}; Cyc_Std_printf( _tag_arr("</Component>",
sizeof( unsigned char), 13u), _tag_arr( _temp245, sizeof( void*), 0u));});}}({
void* _temp246[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tuple>", sizeof(
unsigned char), 9u), _tag_arr( _temp246, sizeof( void*), 0u));}); goto _LL171;
_LL187:({ void* _temp247[ 0u]={}; Cyc_Std_printf( _tag_arr("<TUnion>", sizeof(
unsigned char), 9u), _tag_arr( _temp247, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp210, sizeof( struct _tuple0*)); i ++){ void*
_temp250; unsigned int _temp252; struct _tuple0 _temp248=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp210, sizeof( struct _tuple0*), i))); _LL253:
_temp252= _temp248.f1; goto _LL251; _LL251: _temp250= _temp248.f2; goto _LL249;
_LL249:({ struct Cyc_Std_Int_pa_struct _temp255; _temp255.tag= Cyc_Std_Int_pa;
_temp255.f1=( int) _temp252;{ void* _temp254[ 1u]={& _temp255}; Cyc_Std_printf(
_tag_arr("<Tag tag=\"%d\">", sizeof( unsigned char), 15u), _tag_arr( _temp254,
sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct( _temp250);({ void*
_temp256[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tag>", sizeof( unsigned char), 7u),
_tag_arr( _temp256, sizeof( void*), 0u));});}}({ void* _temp257[ 0u]={}; Cyc_Std_printf(
_tag_arr("</Tunion>", sizeof( unsigned char), 10u), _tag_arr( _temp257, sizeof(
void*), 0u));}); goto _LL171; _LL189:({ void* _temp258[ 0u]={}; Cyc_Std_printf(
_tag_arr("<XTUnion>", sizeof( unsigned char), 10u), _tag_arr( _temp258, sizeof(
void*), 0u));});{ int i= 0; for( 0; i <  _get_arr_size( _temp212, sizeof( struct
_tuple1*)); i ++){ void* _temp261; struct _tagged_arr _temp263; struct _tuple1
_temp259=*(*(( struct _tuple1**) _check_unknown_subscript( _temp212, sizeof(
struct _tuple1*), i))); _LL264: _temp263= _temp259.f1; goto _LL262; _LL262:
_temp261= _temp259.f2; goto _LL260; _LL260:({ struct Cyc_Std_String_pa_struct
_temp266; _temp266.tag= Cyc_Std_String_pa; _temp266.f1=( struct _tagged_arr)
_tagged_arr_plus( _temp263, sizeof( unsigned char), 4);{ void* _temp265[ 1u]={&
_temp266}; Cyc_Std_printf( _tag_arr("<Tag tag=\"%s\">", sizeof( unsigned char),
15u), _tag_arr( _temp265, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp261);({ void* _temp267[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tag>", sizeof(
unsigned char), 7u), _tag_arr( _temp267, sizeof( void*), 0u));});}}({ void*
_temp268[ 0u]={}; Cyc_Std_printf( _tag_arr("</XTunion>", sizeof( unsigned char),
11u), _tag_arr( _temp268, sizeof( void*), 0u));}); goto _LL171; _LL191:({ void*
_temp269[ 0u]={}; Cyc_Std_printf( _tag_arr("<Union>", sizeof( unsigned char), 8u),
_tag_arr( _temp269, sizeof( void*), 0u));});{ int i= 0; for( 0; i < 
_get_arr_size( _temp214, sizeof( void**)); i ++){({ void* _temp270[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Case>", sizeof( unsigned char), 7u), _tag_arr( _temp270, sizeof( void*),
0u));}); Cyc_Typerep_xmlize_typestruct(*(*(( void***) _check_unknown_subscript(
_temp214, sizeof( void**), i))));({ void* _temp271[ 0u]={}; Cyc_Std_printf(
_tag_arr("</Case>", sizeof( unsigned char), 8u), _tag_arr( _temp271, sizeof(
void*), 0u));});}}({ void* _temp272[ 0u]={}; Cyc_Std_printf( _tag_arr("</Union>",
sizeof( unsigned char), 9u), _tag_arr( _temp272, sizeof( void*), 0u));}); goto
_LL171; _LL193:({ void* _temp273[ 0u]={}; Cyc_Std_printf( _tag_arr("<Forall vars=\"",
sizeof( unsigned char), 15u), _tag_arr( _temp273, sizeof( void*), 0u));});{ int
i= 0; for( 0; i <  _get_arr_size( _temp218, sizeof( struct _tagged_arr*)); i ++){
if( i !=  0){({ void* _temp274[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp274, sizeof( void*), 0u));});}({ void*
_temp275[ 0u]={}; Cyc_Std_printf(*(*(( struct _tagged_arr**)
_check_unknown_subscript( _temp218, sizeof( struct _tagged_arr*), i))), _tag_arr(
_temp275, sizeof( void*), 0u));});}}({ void* _temp276[ 0u]={}; Cyc_Std_printf(
_tag_arr("\">", sizeof( unsigned char), 3u), _tag_arr( _temp276, sizeof( void*),
0u));}); Cyc_Typerep_xmlize_typestruct(* _temp216);({ void* _temp277[ 0u]={};
Cyc_Std_printf( _tag_arr("</Forall>", sizeof( unsigned char), 10u), _tag_arr(
_temp277, sizeof( void*), 0u));}); goto _LL171; _LL195:({ void* _temp278[ 0u]={};
Cyc_Std_printf( _tag_arr("<App>", sizeof( unsigned char), 6u), _tag_arr(
_temp278, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct( _temp222);{
int i= 0; for( 0; i <  _get_arr_size( _temp220, sizeof( void*)); i ++){ if( i != 
0){({ void* _temp279[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp279, sizeof( void*), 0u));});} Cyc_Typerep_xmlize_typestruct(*((
void**) _check_unknown_subscript( _temp220, sizeof( void*), i)));}}({ void*
_temp280[ 0u]={}; Cyc_Std_printf( _tag_arr("</App>", sizeof( unsigned char), 7u),
_tag_arr( _temp280, sizeof( void*), 0u));}); goto _LL171; _LL171:;} static void*
Cyc_Typerep_normalize_env( struct Cyc_Dict_Dict* env, void* ts){ void* _temp281=
ts; struct _tagged_arr* _temp307; void* _temp309; unsigned int _temp311; void*
_temp313; struct _tagged_arr _temp315; unsigned int _temp317; struct _tagged_arr
_temp319; struct _tagged_arr _temp321; struct _tagged_arr _temp323; void**
_temp325; struct _tagged_arr _temp327; struct _tagged_arr _temp329; void*
_temp331; _LL283: if(( unsigned int) _temp281 >  2u?*(( int*) _temp281) ==  Cyc_Typerep_Var:
0){ _LL308: _temp307=(( struct Cyc_Typerep_Var_struct*) _temp281)->f1; goto
_LL284;} else{ goto _LL285;} _LL285: if(( unsigned int) _temp281 >  2u?*(( int*)
_temp281) ==  Cyc_Typerep_Int: 0){ goto _LL286;} else{ goto _LL287;} _LL287: if(
_temp281 == ( void*) Cyc_Typerep_Float){ goto _LL288;} else{ goto _LL289;}
_LL289: if( _temp281 == ( void*) Cyc_Typerep_Double){ goto _LL290;} else{ goto
_LL291;} _LL291: if(( unsigned int) _temp281 >  2u?*(( int*) _temp281) ==  Cyc_Typerep_ThinPtr:
0){ _LL312: _temp311=(( struct Cyc_Typerep_ThinPtr_struct*) _temp281)->f1; goto
_LL310; _LL310: _temp309=( void*)(( struct Cyc_Typerep_ThinPtr_struct*) _temp281)->f2;
goto _LL292;} else{ goto _LL293;} _LL293: if(( unsigned int) _temp281 >  2u?*((
int*) _temp281) ==  Cyc_Typerep_FatPtr: 0){ _LL314: _temp313=( void*)(( struct
Cyc_Typerep_FatPtr_struct*) _temp281)->f1; goto _LL294;} else{ goto _LL295;}
_LL295: if(( unsigned int) _temp281 >  2u?*(( int*) _temp281) ==  Cyc_Typerep_Tuple:
0){ _LL318: _temp317=(( struct Cyc_Typerep_Tuple_struct*) _temp281)->f1; goto
_LL316; _LL316: _temp315=(( struct Cyc_Typerep_Tuple_struct*) _temp281)->f2;
goto _LL296;} else{ goto _LL297;} _LL297: if(( unsigned int) _temp281 >  2u?*((
int*) _temp281) ==  Cyc_Typerep_TUnion: 0){ _LL320: _temp319=(( struct Cyc_Typerep_TUnion_struct*)
_temp281)->f1; goto _LL298;} else{ goto _LL299;} _LL299: if(( unsigned int)
_temp281 >  2u?*(( int*) _temp281) ==  Cyc_Typerep_XTUnion: 0){ _LL322: _temp321=((
struct Cyc_Typerep_XTUnion_struct*) _temp281)->f1; goto _LL300;} else{ goto
_LL301;} _LL301: if(( unsigned int) _temp281 >  2u?*(( int*) _temp281) ==  Cyc_Typerep_Union:
0){ _LL324: _temp323=(( struct Cyc_Typerep_Union_struct*) _temp281)->f1; goto
_LL302;} else{ goto _LL303;} _LL303: if(( unsigned int) _temp281 >  2u?*(( int*)
_temp281) ==  Cyc_Typerep_Forall: 0){ _LL328: _temp327=(( struct Cyc_Typerep_Forall_struct*)
_temp281)->f1; goto _LL326; _LL326: _temp325=(( struct Cyc_Typerep_Forall_struct*)
_temp281)->f2; goto _LL304;} else{ goto _LL305;} _LL305: if(( unsigned int)
_temp281 >  2u?*(( int*) _temp281) ==  Cyc_Typerep_App: 0){ _LL332: _temp331=(
void*)(( struct Cyc_Typerep_App_struct*) _temp281)->f1; goto _LL330; _LL330:
_temp329=(( struct Cyc_Typerep_App_struct*) _temp281)->f2; goto _LL306;} else{
goto _LL282;} _LL284: return(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( env,( struct _tagged_arr*) _temp307); _LL286:
goto _LL288; _LL288: goto _LL290; _LL290: return ts; _LL292: return( void*)({
struct Cyc_Typerep_ThinPtr_struct* _temp333=( struct Cyc_Typerep_ThinPtr_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_ThinPtr_struct)); _temp333[ 0]=({ struct
Cyc_Typerep_ThinPtr_struct _temp334; _temp334.tag= Cyc_Typerep_ThinPtr; _temp334.f1=
_temp311; _temp334.f2=( void*) Cyc_Typerep_normalize_env( env, _temp309);
_temp334;}); _temp333;}); _LL294: return( void*)({ struct Cyc_Typerep_FatPtr_struct*
_temp335=( struct Cyc_Typerep_FatPtr_struct*) _cycalloc( sizeof( struct Cyc_Typerep_FatPtr_struct));
_temp335[ 0]=({ struct Cyc_Typerep_FatPtr_struct _temp336; _temp336.tag= Cyc_Typerep_FatPtr;
_temp336.f1=( void*) Cyc_Typerep_normalize_env( env, _temp313); _temp336;});
_temp335;}); _LL296: { struct _tagged_arr _temp337=({ unsigned int _temp353=
_get_arr_size( _temp315, sizeof( struct _tuple0*)); struct _tuple0** _temp354=(
struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*), _temp353));
struct _tagged_arr _temp356= _tag_arr( _temp354, sizeof( struct _tuple0*),
_get_arr_size( _temp315, sizeof( struct _tuple0*)));{ unsigned int _temp355=
_temp353; unsigned int i; for( i= 0; i <  _temp355; i ++){ _temp354[ i]= 0;}};
_temp356;});{ int i= 0; for( 0; i <  _get_arr_size( _temp315, sizeof( struct
_tuple0*)); i ++){ struct _tuple0 _temp340; void* _temp341; unsigned int
_temp343; struct _tuple0* _temp338=*(( struct _tuple0**)
_check_unknown_subscript( _temp315, sizeof( struct _tuple0*), i)); _temp340=*
_temp338; _LL344: _temp343= _temp340.f1; goto _LL342; _LL342: _temp341= _temp340.f2;
goto _LL339; _LL339:*(( struct _tuple0**) _check_unknown_subscript( _temp337,
sizeof( struct _tuple0*), i))=({ struct _tuple0* _temp345=( struct _tuple0*)
_cycalloc( sizeof( struct _tuple0)); _temp345->f1= _temp343; _temp345->f2= Cyc_Typerep_normalize_env(
env, _temp341); _temp345;});}}{ struct _tagged_arr _temp346=({ unsigned int
_temp349= _get_arr_size( _temp315, sizeof( struct _tuple0*)); struct _tuple0**
_temp350=( struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*),
_temp349)); struct _tagged_arr _temp352= _tag_arr( _temp350, sizeof( struct
_tuple0*), _get_arr_size( _temp315, sizeof( struct _tuple0*)));{ unsigned int
_temp351= _temp349; unsigned int i; for( i= 0; i <  _temp351; i ++){ _temp350[ i]=(
struct _tuple0*) _check_null(*(( struct _tuple0**) _check_unknown_subscript(
_temp337, sizeof( struct _tuple0*),( int) i)));}}; _temp352;}); return( void*)({
struct Cyc_Typerep_Tuple_struct* _temp347=( struct Cyc_Typerep_Tuple_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Tuple_struct)); _temp347[ 0]=({ struct Cyc_Typerep_Tuple_struct
_temp348; _temp348.tag= Cyc_Typerep_Tuple; _temp348.f1= _temp317; _temp348.f2=(
struct _tagged_arr) _temp346; _temp348;}); _temp347;});}} _LL298: { struct
_tagged_arr _temp357=({ unsigned int _temp373= _get_arr_size( _temp319, sizeof(
struct _tuple0*)); struct _tuple0** _temp374=( struct _tuple0**) _cycalloc(
_check_times( sizeof( struct _tuple0*), _temp373)); struct _tagged_arr _temp376=
_tag_arr( _temp374, sizeof( struct _tuple0*), _get_arr_size( _temp319, sizeof(
struct _tuple0*)));{ unsigned int _temp375= _temp373; unsigned int i; for( i= 0;
i <  _temp375; i ++){ _temp374[ i]= 0;}}; _temp376;});{ int i= 0; for( 0; i < 
_get_arr_size( _temp319, sizeof( struct _tuple0*)); i ++){ struct _tuple0
_temp360; void* _temp361; unsigned int _temp363; struct _tuple0* _temp358=*((
struct _tuple0**) _check_unknown_subscript( _temp319, sizeof( struct _tuple0*),
i)); _temp360=* _temp358; _LL364: _temp363= _temp360.f1; goto _LL362; _LL362:
_temp361= _temp360.f2; goto _LL359; _LL359:*(( struct _tuple0**)
_check_unknown_subscript( _temp357, sizeof( struct _tuple0*), i))=({ struct
_tuple0* _temp365=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp365->f1= _temp363; _temp365->f2= Cyc_Typerep_normalize_env( env, _temp361);
_temp365;});}}{ struct _tagged_arr _temp366=({ unsigned int _temp369=
_get_arr_size( _temp319, sizeof( struct _tuple0*)); struct _tuple0** _temp370=(
struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*), _temp369));
struct _tagged_arr _temp372= _tag_arr( _temp370, sizeof( struct _tuple0*),
_get_arr_size( _temp319, sizeof( struct _tuple0*)));{ unsigned int _temp371=
_temp369; unsigned int i; for( i= 0; i <  _temp371; i ++){ _temp370[ i]=( struct
_tuple0*) _check_null(*(( struct _tuple0**) _check_unknown_subscript( _temp357,
sizeof( struct _tuple0*),( int) i)));}}; _temp372;}); return( void*)({ struct
Cyc_Typerep_TUnion_struct* _temp367=( struct Cyc_Typerep_TUnion_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_TUnion_struct)); _temp367[ 0]=({ struct
Cyc_Typerep_TUnion_struct _temp368; _temp368.tag= Cyc_Typerep_TUnion; _temp368.f1=(
struct _tagged_arr) _temp366; _temp368;}); _temp367;});}} _LL300: { struct
_tagged_arr _temp377=({ unsigned int _temp393= _get_arr_size( _temp321, sizeof(
struct _tuple1*)); struct _tuple1** _temp394=( struct _tuple1**) _cycalloc(
_check_times( sizeof( struct _tuple1*), _temp393)); struct _tagged_arr _temp396=
_tag_arr( _temp394, sizeof( struct _tuple1*), _get_arr_size( _temp321, sizeof(
struct _tuple1*)));{ unsigned int _temp395= _temp393; unsigned int i; for( i= 0;
i <  _temp395; i ++){ _temp394[ i]= 0;}}; _temp396;});{ int i= 0; for( 0; i < 
_get_arr_size( _temp321, sizeof( struct _tuple1*)); i ++){ struct _tuple1
_temp380; void* _temp381; struct _tagged_arr _temp383; struct _tuple1* _temp378=*((
struct _tuple1**) _check_unknown_subscript( _temp321, sizeof( struct _tuple1*),
i)); _temp380=* _temp378; _LL384: _temp383= _temp380.f1; goto _LL382; _LL382:
_temp381= _temp380.f2; goto _LL379; _LL379:*(( struct _tuple1**)
_check_unknown_subscript( _temp377, sizeof( struct _tuple1*), i))=({ struct
_tuple1* _temp385=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp385->f1= _temp383; _temp385->f2= Cyc_Typerep_normalize_env( env, _temp381);
_temp385;});}}{ struct _tagged_arr _temp386=({ unsigned int _temp389=
_get_arr_size( _temp321, sizeof( struct _tuple1*)); struct _tuple1** _temp390=(
struct _tuple1**) _cycalloc( _check_times( sizeof( struct _tuple1*), _temp389));
struct _tagged_arr _temp392= _tag_arr( _temp390, sizeof( struct _tuple1*),
_get_arr_size( _temp321, sizeof( struct _tuple1*)));{ unsigned int _temp391=
_temp389; unsigned int i; for( i= 0; i <  _temp391; i ++){ _temp390[ i]=( struct
_tuple1*) _check_null(*(( struct _tuple1**) _check_unknown_subscript( _temp377,
sizeof( struct _tuple1*),( int) i)));}}; _temp392;}); return( void*)({ struct
Cyc_Typerep_XTUnion_struct* _temp387=( struct Cyc_Typerep_XTUnion_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_XTUnion_struct)); _temp387[ 0]=({ struct
Cyc_Typerep_XTUnion_struct _temp388; _temp388.tag= Cyc_Typerep_XTUnion; _temp388.f1=(
struct _tagged_arr) _temp386; _temp388;}); _temp387;});}} _LL302: { struct
_tagged_arr _temp397=({ unsigned int _temp406= _get_arr_size( _temp323, sizeof(
void**)); void*** _temp407=( void***) _cycalloc( _check_times( sizeof( void**),
_temp406)); struct _tagged_arr _temp409= _tag_arr( _temp407, sizeof( void**),
_get_arr_size( _temp323, sizeof( void**)));{ unsigned int _temp408= _temp406;
unsigned int i; for( i= 0; i <  _temp408; i ++){ _temp407[ i]= 0;}}; _temp409;});{
int i= 0; for( 0; i <  _get_arr_size( _temp323, sizeof( void**)); i ++){*(( void***)
_check_unknown_subscript( _temp397, sizeof( void**), i))=({ void** _temp398=(
void**) _cycalloc( sizeof( void*)); _temp398[ 0]= Cyc_Typerep_normalize_env( env,*(*((
void***) _check_unknown_subscript( _temp323, sizeof( void**), i)))); _temp398;});}}{
struct _tagged_arr _temp399=({ unsigned int _temp402= _get_arr_size( _temp323,
sizeof( void**)); void*** _temp403=( void***) _cycalloc( _check_times( sizeof(
void**), _temp402)); struct _tagged_arr _temp405= _tag_arr( _temp403, sizeof(
void**), _get_arr_size( _temp323, sizeof( void**)));{ unsigned int _temp404=
_temp402; unsigned int i; for( i= 0; i <  _temp404; i ++){ _temp403[ i]=( void**)
_check_null(*(( void***) _check_unknown_subscript( _temp397, sizeof( void**),(
int) i)));}}; _temp405;}); return( void*)({ struct Cyc_Typerep_Union_struct*
_temp400=( struct Cyc_Typerep_Union_struct*) _cycalloc( sizeof( struct Cyc_Typerep_Union_struct));
_temp400[ 0]=({ struct Cyc_Typerep_Union_struct _temp401; _temp401.tag= Cyc_Typerep_Union;
_temp401.f1=( struct _tagged_arr) _temp399; _temp401;}); _temp400;});}} _LL304:
return( void*)({ struct Cyc_Typerep_Forall_struct* _temp410=( struct Cyc_Typerep_Forall_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Forall_struct)); _temp410[ 0]=({ struct
Cyc_Typerep_Forall_struct _temp411; _temp411.tag= Cyc_Typerep_Forall; _temp411.f1=
_temp327; _temp411.f2=({ void** _temp412=( void**) _cycalloc( sizeof( void*));
_temp412[ 0]= Cyc_Typerep_normalize_env( env,* _temp325); _temp412;}); _temp411;});
_temp410;}); _LL306: { void* _temp413= Cyc_Typerep_normalize_env( env, _temp331);
void** _temp419; struct _tagged_arr _temp421; _LL415: if(( unsigned int)
_temp413 >  2u?*(( int*) _temp413) ==  Cyc_Typerep_Forall: 0){ _LL422: _temp421=((
struct Cyc_Typerep_Forall_struct*) _temp413)->f1; goto _LL420; _LL420: _temp419=((
struct Cyc_Typerep_Forall_struct*) _temp413)->f2; goto _LL416;} else{ goto
_LL417;} _LL417: goto _LL418; _LL416: if( _get_arr_size( _temp329, sizeof( void*))
!=  _get_arr_size( _temp421, sizeof( struct _tagged_arr*))){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp423=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp423[ 0]=({ struct Cyc_Core_Failure_struct
_temp424; _temp424.tag= Cyc_Core_Failure; _temp424.f1= _tag_arr("app appiled wrong number of arguments",
sizeof( unsigned char), 38u); _temp424;}); _temp423;}));}{ int i= 0; for( 0; i < 
_get_arr_size( _temp329, sizeof( void*)); i ++){ env=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( env,*((
struct _tagged_arr**) _check_unknown_subscript( _temp421, sizeof( struct
_tagged_arr*), i)),*(( void**) _check_unknown_subscript( _temp329, sizeof( void*),
i)));}} return Cyc_Typerep_normalize_env( env,* _temp419); _LL418:( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp425=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp425[ 0]=({ struct Cyc_Core_Failure_struct
_temp426; _temp426.tag= Cyc_Core_Failure; _temp426.f1= _tag_arr("app misapplied",
sizeof( unsigned char), 15u); _temp426;}); _temp425;})); _LL414:;} _LL282:;} int
Cyc_Typerep_name_order( struct _tagged_arr* s1, struct _tagged_arr* s2){ return
Cyc_Std_strcmp(* s1,* s2);} void* Cyc_Typerep_normalize( void* ts){ return Cyc_Typerep_normalize_env(((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Typerep_name_order), ts);}
