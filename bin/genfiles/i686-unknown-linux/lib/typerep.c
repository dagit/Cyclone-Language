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
const int Cyc_Typerep_Union= 6; struct Cyc_Typerep_Union_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Typerep_Forall= 7; struct Cyc_Typerep_Forall_struct{
int tag; struct _tagged_arr f1; void** f2; } ; static const int Cyc_Typerep_App=
8; struct Cyc_Typerep_App_struct{ int tag; void* f1; struct _tagged_arr f2; } ;
extern void Cyc_Typerep_print_typestruct( void* rep); extern void Cyc_Typerep_xmlize_typestruct(
void* rep); extern void* Cyc_Typerep_normalize( void* ts); struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
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
_tagged_arr s2); void Cyc_Typerep_print_typestruct( void* rep){ void* _temp0=
rep; struct _tagged_arr* _temp24; unsigned int _temp26; void* _temp28;
unsigned int _temp30; void* _temp32; struct _tagged_arr _temp34; unsigned int
_temp36; struct _tagged_arr _temp38; struct _tagged_arr _temp40; void** _temp42;
struct _tagged_arr _temp44; struct _tagged_arr _temp46; void* _temp48; _LL2: if((
unsigned int) _temp0 >  2u?*(( int*) _temp0) ==  Cyc_Typerep_Var: 0){ _LL25:
_temp24=(( struct Cyc_Typerep_Var_struct*) _temp0)->f1; goto _LL3;} else{ goto
_LL4;} _LL4: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0) ==  Cyc_Typerep_Int:
0){ _LL27: _temp26=(( struct Cyc_Typerep_Int_struct*) _temp0)->f1; goto _LL5;}
else{ goto _LL6;} _LL6: if( _temp0 == ( void*) Cyc_Typerep_Float){ goto _LL7;}
else{ goto _LL8;} _LL8: if( _temp0 == ( void*) Cyc_Typerep_Double){ goto _LL9;}
else{ goto _LL10;} _LL10: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0) == 
Cyc_Typerep_ThinPtr: 0){ _LL31: _temp30=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp0)->f1; goto _LL29; _LL29: _temp28=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp0)->f2; goto _LL11;} else{ goto _LL12;} _LL12: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_FatPtr: 0){ _LL33: _temp32=( void*)((
struct Cyc_Typerep_FatPtr_struct*) _temp0)->f1; goto _LL13;} else{ goto _LL14;}
_LL14: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0) ==  Cyc_Typerep_Tuple:
0){ _LL37: _temp36=(( struct Cyc_Typerep_Tuple_struct*) _temp0)->f1; goto _LL35;
_LL35: _temp34=(( struct Cyc_Typerep_Tuple_struct*) _temp0)->f2; goto _LL15;}
else{ goto _LL16;} _LL16: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0) == 
Cyc_Typerep_TUnion: 0){ _LL39: _temp38=(( struct Cyc_Typerep_TUnion_struct*)
_temp0)->f1; goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_Union: 0){ _LL41: _temp40=(( struct Cyc_Typerep_Union_struct*)
_temp0)->f1; goto _LL19;} else{ goto _LL20;} _LL20: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_Forall: 0){ _LL45: _temp44=(( struct Cyc_Typerep_Forall_struct*)
_temp0)->f1; goto _LL43; _LL43: _temp42=(( struct Cyc_Typerep_Forall_struct*)
_temp0)->f2; goto _LL21;} else{ goto _LL22;} _LL22: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_App: 0){ _LL49: _temp48=( void*)(( struct
Cyc_Typerep_App_struct*) _temp0)->f1; goto _LL47; _LL47: _temp46=(( struct Cyc_Typerep_App_struct*)
_temp0)->f2; goto _LL23;} else{ goto _LL1;} _LL3:({ struct Cyc_Std_String_pa_struct
_temp51; _temp51.tag= Cyc_Std_String_pa; _temp51.f1=( struct _tagged_arr)*
_temp24;{ void* _temp50[ 1u]={& _temp51}; Cyc_Std_printf( _tag_arr("Var(%s)",
sizeof( unsigned char), 8u), _tag_arr( _temp50, sizeof( void*), 1u));}}); goto
_LL1; _LL5:({ struct Cyc_Std_Int_pa_struct _temp53; _temp53.tag= Cyc_Std_Int_pa;
_temp53.f1=( int) _temp26;{ void* _temp52[ 1u]={& _temp53}; Cyc_Std_printf(
_tag_arr("Int(%d)", sizeof( unsigned char), 8u), _tag_arr( _temp52, sizeof( void*),
1u));}}); goto _LL1; _LL7:({ void* _temp54[ 0u]={}; Cyc_Std_printf( _tag_arr("Float",
sizeof( unsigned char), 6u), _tag_arr( _temp54, sizeof( void*), 0u));}); goto
_LL1; _LL9:({ void* _temp55[ 0u]={}; Cyc_Std_printf( _tag_arr("Double", sizeof(
unsigned char), 7u), _tag_arr( _temp55, sizeof( void*), 0u));}); goto _LL1;
_LL11:({ struct Cyc_Std_Int_pa_struct _temp57; _temp57.tag= Cyc_Std_Int_pa;
_temp57.f1=( int) _temp30;{ void* _temp56[ 1u]={& _temp57}; Cyc_Std_printf(
_tag_arr("ThinPtr(%d,", sizeof( unsigned char), 12u), _tag_arr( _temp56, sizeof(
void*), 1u));}}); Cyc_Typerep_print_typestruct( _temp28);({ void* _temp58[ 0u]={};
Cyc_Std_printf( _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr( _temp58,
sizeof( void*), 0u));}); goto _LL1; _LL13:({ void* _temp59[ 0u]={}; Cyc_Std_printf(
_tag_arr("FatPtr(", sizeof( unsigned char), 8u), _tag_arr( _temp59, sizeof( void*),
0u));}); Cyc_Typerep_print_typestruct( _temp32);({ void* _temp60[ 0u]={}; Cyc_Std_printf(
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr( _temp60, sizeof( void*), 0u));});
goto _LL1; _LL15:({ struct Cyc_Std_Int_pa_struct _temp62; _temp62.tag= Cyc_Std_Int_pa;
_temp62.f1=( int) _temp36;{ void* _temp61[ 1u]={& _temp62}; Cyc_Std_printf(
_tag_arr("Tuple(%d,[", sizeof( unsigned char), 11u), _tag_arr( _temp61, sizeof(
void*), 1u));}});{ int i= 0; for( 0; i <  _get_arr_size( _temp34, sizeof( struct
_tuple0*)); i ++){ void* _temp65; unsigned int _temp67; struct _tuple0 _temp63=*(*((
struct _tuple0**) _check_unknown_subscript( _temp34, sizeof( struct _tuple0*), i)));
_LL68: _temp67= _temp63.f1; goto _LL66; _LL66: _temp65= _temp63.f2; goto _LL64;
_LL64:({ struct Cyc_Std_Int_pa_struct _temp70; _temp70.tag= Cyc_Std_Int_pa;
_temp70.f1=( int) _temp67;{ void* _temp69[ 1u]={& _temp70}; Cyc_Std_printf(
_tag_arr("$(%d,", sizeof( unsigned char), 6u), _tag_arr( _temp69, sizeof( void*),
1u));}}); Cyc_Typerep_print_typestruct( _temp65);({ struct Cyc_Std_Int_pa_struct
_temp72; _temp72.tag= Cyc_Std_Int_pa; _temp72.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp34, sizeof( struct _tuple0*)) -  1?',':' ')));{ void*
_temp71[ 1u]={& _temp72}; Cyc_Std_printf( _tag_arr(")%c", sizeof( unsigned char),
4u), _tag_arr( _temp71, sizeof( void*), 1u));}});}}({ void* _temp73[ 0u]={}; Cyc_Std_printf(
_tag_arr(" ]", sizeof( unsigned char), 3u), _tag_arr( _temp73, sizeof( void*), 0u));});
goto _LL1; _LL17:({ void* _temp74[ 0u]={}; Cyc_Std_printf( _tag_arr("TUnion(",
sizeof( unsigned char), 8u), _tag_arr( _temp74, sizeof( void*), 0u));});{ int i=
0; for( 0; i <  _get_arr_size( _temp38, sizeof( struct _tuple0*)); i ++){ void*
_temp77; unsigned int _temp79; struct _tuple0 _temp75=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp38, sizeof( struct _tuple0*), i))); _LL80:
_temp79= _temp75.f1; goto _LL78; _LL78: _temp77= _temp75.f2; goto _LL76; _LL76:({
struct Cyc_Std_Int_pa_struct _temp82; _temp82.tag= Cyc_Std_Int_pa; _temp82.f1=(
int) _temp79;{ void* _temp81[ 1u]={& _temp82}; Cyc_Std_printf( _tag_arr("$(%d,",
sizeof( unsigned char), 6u), _tag_arr( _temp81, sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct(
_temp77);({ struct Cyc_Std_Int_pa_struct _temp84; _temp84.tag= Cyc_Std_Int_pa;
_temp84.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size( _temp38, sizeof(
struct _tuple0*)) -  1?',':' ')));{ void* _temp83[ 1u]={& _temp84}; Cyc_Std_printf(
_tag_arr(")%c", sizeof( unsigned char), 4u), _tag_arr( _temp83, sizeof( void*),
1u));}});}}({ void* _temp85[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof(
unsigned char), 3u), _tag_arr( _temp85, sizeof( void*), 0u));}); goto _LL1;
_LL19:({ void* _temp86[ 0u]={}; Cyc_Std_printf( _tag_arr("Union(", sizeof(
unsigned char), 7u), _tag_arr( _temp86, sizeof( void*), 0u));});{ int i= 0; for(
0; i <  _get_arr_size( _temp40, sizeof( void**)); i ++){ Cyc_Typerep_print_typestruct(*(*((
void***) _check_unknown_subscript( _temp40, sizeof( void**), i))));({ struct Cyc_Std_Int_pa_struct
_temp88; _temp88.tag= Cyc_Std_Int_pa; _temp88.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp40, sizeof( void**)) -  1?',':' ')));{ void* _temp87[
1u]={& _temp88}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char), 3u),
_tag_arr( _temp87, sizeof( void*), 1u));}});}}({ void* _temp89[ 0u]={}; Cyc_Std_printf(
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr( _temp89, sizeof( void*), 0u));});
goto _LL1; _LL21:({ void* _temp90[ 0u]={}; Cyc_Std_printf( _tag_arr("Forall([",
sizeof( unsigned char), 9u), _tag_arr( _temp90, sizeof( void*), 0u));});{ int i=
0; for( 0; i <  _get_arr_size( _temp44, sizeof( struct _tagged_arr*)); i ++){({
void* _temp91[ 0u]={}; Cyc_Std_printf(*(*(( struct _tagged_arr**)
_check_unknown_subscript( _temp44, sizeof( struct _tagged_arr*), i))), _tag_arr(
_temp91, sizeof( void*), 0u));});({ struct Cyc_Std_Int_pa_struct _temp93;
_temp93.tag= Cyc_Std_Int_pa; _temp93.f1=( int)(( unsigned int)(( int)( i != 
_get_arr_size( _temp44, sizeof( struct _tagged_arr*)) -  1?',':' ')));{ void*
_temp92[ 1u]={& _temp93}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char),
3u), _tag_arr( _temp92, sizeof( void*), 1u));}});}}({ void* _temp94[ 0u]={}; Cyc_Std_printf(
_tag_arr("],", sizeof( unsigned char), 3u), _tag_arr( _temp94, sizeof( void*), 0u));});
Cyc_Typerep_print_typestruct(* _temp42);({ void* _temp95[ 0u]={}; Cyc_Std_printf(
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr( _temp95, sizeof( void*), 0u));});
goto _LL1; _LL23:({ void* _temp96[ 0u]={}; Cyc_Std_printf( _tag_arr("App(",
sizeof( unsigned char), 5u), _tag_arr( _temp96, sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct(
_temp48);({ void* _temp97[ 0u]={}; Cyc_Std_printf( _tag_arr(",[", sizeof(
unsigned char), 3u), _tag_arr( _temp97, sizeof( void*), 0u));});{ int i= 0; for(
0; i <  _get_arr_size( _temp46, sizeof( void*)); i ++){ Cyc_Typerep_print_typestruct(*((
void**) _check_unknown_subscript( _temp46, sizeof( void*), i)));({ struct Cyc_Std_Int_pa_struct
_temp99; _temp99.tag= Cyc_Std_Int_pa; _temp99.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp46, sizeof( void*)) -  1?',':' ')));{ void* _temp98[ 1u]={&
_temp99}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char), 3u), _tag_arr(
_temp98, sizeof( void*), 1u));}});}}({ void* _temp100[ 0u]={}; Cyc_Std_printf(
_tag_arr("])", sizeof( unsigned char), 3u), _tag_arr( _temp100, sizeof( void*),
0u));}); goto _LL1; _LL1:;} void Cyc_Typerep_xmlize_typestruct( void* rep){ void*
_temp101= rep; struct _tagged_arr* _temp125; unsigned int _temp127; void*
_temp129; unsigned int _temp131; void* _temp133; struct _tagged_arr _temp135;
unsigned int _temp137; struct _tagged_arr _temp139; struct _tagged_arr _temp141;
void** _temp143; struct _tagged_arr _temp145; struct _tagged_arr _temp147; void*
_temp149; _LL103: if(( unsigned int) _temp101 >  2u?*(( int*) _temp101) ==  Cyc_Typerep_Var:
0){ _LL126: _temp125=(( struct Cyc_Typerep_Var_struct*) _temp101)->f1; goto
_LL104;} else{ goto _LL105;} _LL105: if(( unsigned int) _temp101 >  2u?*(( int*)
_temp101) ==  Cyc_Typerep_Int: 0){ _LL128: _temp127=(( struct Cyc_Typerep_Int_struct*)
_temp101)->f1; goto _LL106;} else{ goto _LL107;} _LL107: if( _temp101 == ( void*)
Cyc_Typerep_Float){ goto _LL108;} else{ goto _LL109;} _LL109: if( _temp101 == (
void*) Cyc_Typerep_Double){ goto _LL110;} else{ goto _LL111;} _LL111: if((
unsigned int) _temp101 >  2u?*(( int*) _temp101) ==  Cyc_Typerep_ThinPtr: 0){
_LL132: _temp131=(( struct Cyc_Typerep_ThinPtr_struct*) _temp101)->f1; goto
_LL130; _LL130: _temp129=( void*)(( struct Cyc_Typerep_ThinPtr_struct*) _temp101)->f2;
goto _LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp101 >  2u?*((
int*) _temp101) ==  Cyc_Typerep_FatPtr: 0){ _LL134: _temp133=( void*)(( struct
Cyc_Typerep_FatPtr_struct*) _temp101)->f1; goto _LL114;} else{ goto _LL115;}
_LL115: if(( unsigned int) _temp101 >  2u?*(( int*) _temp101) ==  Cyc_Typerep_Tuple:
0){ _LL138: _temp137=(( struct Cyc_Typerep_Tuple_struct*) _temp101)->f1; goto
_LL136; _LL136: _temp135=(( struct Cyc_Typerep_Tuple_struct*) _temp101)->f2;
goto _LL116;} else{ goto _LL117;} _LL117: if(( unsigned int) _temp101 >  2u?*((
int*) _temp101) ==  Cyc_Typerep_TUnion: 0){ _LL140: _temp139=(( struct Cyc_Typerep_TUnion_struct*)
_temp101)->f1; goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int)
_temp101 >  2u?*(( int*) _temp101) ==  Cyc_Typerep_Union: 0){ _LL142: _temp141=((
struct Cyc_Typerep_Union_struct*) _temp101)->f1; goto _LL120;} else{ goto _LL121;}
_LL121: if(( unsigned int) _temp101 >  2u?*(( int*) _temp101) ==  Cyc_Typerep_Forall:
0){ _LL146: _temp145=(( struct Cyc_Typerep_Forall_struct*) _temp101)->f1; goto
_LL144; _LL144: _temp143=(( struct Cyc_Typerep_Forall_struct*) _temp101)->f2;
goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int) _temp101 >  2u?*((
int*) _temp101) ==  Cyc_Typerep_App: 0){ _LL150: _temp149=( void*)(( struct Cyc_Typerep_App_struct*)
_temp101)->f1; goto _LL148; _LL148: _temp147=(( struct Cyc_Typerep_App_struct*)
_temp101)->f2; goto _LL124;} else{ goto _LL102;} _LL104:({ struct Cyc_Std_String_pa_struct
_temp152; _temp152.tag= Cyc_Std_String_pa; _temp152.f1=( struct _tagged_arr)*
_temp125;{ void* _temp151[ 1u]={& _temp152}; Cyc_Std_printf( _tag_arr("<Var name=\"%s\"/>",
sizeof( unsigned char), 17u), _tag_arr( _temp151, sizeof( void*), 1u));}}); goto
_LL102; _LL106:({ struct Cyc_Std_Int_pa_struct _temp154; _temp154.tag= Cyc_Std_Int_pa;
_temp154.f1=( int) _temp127;{ void* _temp153[ 1u]={& _temp154}; Cyc_Std_printf(
_tag_arr("<Int sz=\"%d\"/>", sizeof( unsigned char), 15u), _tag_arr( _temp153,
sizeof( void*), 1u));}}); goto _LL102; _LL108:({ void* _temp155[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Float/>", sizeof( unsigned char), 9u), _tag_arr( _temp155, sizeof(
void*), 0u));}); goto _LL102; _LL110:({ void* _temp156[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Double/>", sizeof( unsigned char), 10u), _tag_arr( _temp156, sizeof(
void*), 0u));}); goto _LL102; _LL112:({ struct Cyc_Std_Int_pa_struct _temp158;
_temp158.tag= Cyc_Std_Int_pa; _temp158.f1=( int) _temp131;{ void* _temp157[ 1u]={&
_temp158}; Cyc_Std_printf( _tag_arr("<ThinPtr sz=\"%d\">", sizeof( unsigned char),
18u), _tag_arr( _temp157, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp129);({ void* _temp159[ 0u]={}; Cyc_Std_printf( _tag_arr("</ThinPtr",
sizeof( unsigned char), 10u), _tag_arr( _temp159, sizeof( void*), 0u));}); goto
_LL102; _LL114:({ void* _temp160[ 0u]={}; Cyc_Std_printf( _tag_arr("<FatPtr>",
sizeof( unsigned char), 9u), _tag_arr( _temp160, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct(
_temp133);({ void* _temp161[ 0u]={}; Cyc_Std_printf( _tag_arr("</FatPtr>",
sizeof( unsigned char), 10u), _tag_arr( _temp161, sizeof( void*), 0u));}); goto
_LL102; _LL116:({ struct Cyc_Std_Int_pa_struct _temp163; _temp163.tag= Cyc_Std_Int_pa;
_temp163.f1=( int) _temp137;{ void* _temp162[ 1u]={& _temp163}; Cyc_Std_printf(
_tag_arr("<Tuple sz=\"%d\">", sizeof( unsigned char), 16u), _tag_arr( _temp162,
sizeof( void*), 1u));}});{ int i= 0; for( 0; i <  _get_arr_size( _temp135,
sizeof( struct _tuple0*)); i ++){ void* _temp166; unsigned int _temp168; struct
_tuple0 _temp164=*(*(( struct _tuple0**) _check_unknown_subscript( _temp135,
sizeof( struct _tuple0*), i))); _LL169: _temp168= _temp164.f1; goto _LL167;
_LL167: _temp166= _temp164.f2; goto _LL165; _LL165:({ struct Cyc_Std_Int_pa_struct
_temp171; _temp171.tag= Cyc_Std_Int_pa; _temp171.f1=( int) _temp168;{ void*
_temp170[ 1u]={& _temp171}; Cyc_Std_printf( _tag_arr("<Component ofs=\"%d\">",
sizeof( unsigned char), 21u), _tag_arr( _temp170, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp166);({ void* _temp172[ 0u]={}; Cyc_Std_printf( _tag_arr("</Component>",
sizeof( unsigned char), 13u), _tag_arr( _temp172, sizeof( void*), 0u));});}}({
void* _temp173[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tuple>", sizeof(
unsigned char), 9u), _tag_arr( _temp173, sizeof( void*), 0u));}); goto _LL102;
_LL118:({ void* _temp174[ 0u]={}; Cyc_Std_printf( _tag_arr("<TUnion>", sizeof(
unsigned char), 9u), _tag_arr( _temp174, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp139, sizeof( struct _tuple0*)); i ++){ void*
_temp177; unsigned int _temp179; struct _tuple0 _temp175=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp139, sizeof( struct _tuple0*), i))); _LL180:
_temp179= _temp175.f1; goto _LL178; _LL178: _temp177= _temp175.f2; goto _LL176;
_LL176:({ struct Cyc_Std_Int_pa_struct _temp182; _temp182.tag= Cyc_Std_Int_pa;
_temp182.f1=( int) _temp179;{ void* _temp181[ 1u]={& _temp182}; Cyc_Std_printf(
_tag_arr("<Tag tag=\"%d\">", sizeof( unsigned char), 15u), _tag_arr( _temp181,
sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct( _temp177);({ void*
_temp183[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tag>", sizeof( unsigned char), 7u),
_tag_arr( _temp183, sizeof( void*), 0u));});}}({ void* _temp184[ 0u]={}; Cyc_Std_printf(
_tag_arr("</Tunion>", sizeof( unsigned char), 10u), _tag_arr( _temp184, sizeof(
void*), 0u));}); goto _LL102; _LL120:({ void* _temp185[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Union>", sizeof( unsigned char), 8u), _tag_arr( _temp185, sizeof(
void*), 0u));});{ int i= 0; for( 0; i <  _get_arr_size( _temp141, sizeof( void**));
i ++){({ void* _temp186[ 0u]={}; Cyc_Std_printf( _tag_arr("<Case>", sizeof(
unsigned char), 7u), _tag_arr( _temp186, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct(*(*((
void***) _check_unknown_subscript( _temp141, sizeof( void**), i))));({ void*
_temp187[ 0u]={}; Cyc_Std_printf( _tag_arr("</Case>", sizeof( unsigned char), 8u),
_tag_arr( _temp187, sizeof( void*), 0u));});}}({ void* _temp188[ 0u]={}; Cyc_Std_printf(
_tag_arr("</Union>", sizeof( unsigned char), 9u), _tag_arr( _temp188, sizeof(
void*), 0u));}); goto _LL102; _LL122:({ void* _temp189[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Forall vars=\"", sizeof( unsigned char), 15u), _tag_arr( _temp189,
sizeof( void*), 0u));});{ int i= 0; for( 0; i <  _get_arr_size( _temp145,
sizeof( struct _tagged_arr*)); i ++){ if( i !=  0){({ void* _temp190[ 0u]={};
Cyc_Std_printf( _tag_arr(", ", sizeof( unsigned char), 3u), _tag_arr( _temp190,
sizeof( void*), 0u));});}({ void* _temp191[ 0u]={}; Cyc_Std_printf(*(*(( struct
_tagged_arr**) _check_unknown_subscript( _temp145, sizeof( struct _tagged_arr*),
i))), _tag_arr( _temp191, sizeof( void*), 0u));});}}({ void* _temp192[ 0u]={};
Cyc_Std_printf( _tag_arr("\">", sizeof( unsigned char), 3u), _tag_arr( _temp192,
sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct(* _temp143);({ void*
_temp193[ 0u]={}; Cyc_Std_printf( _tag_arr("</Forall>", sizeof( unsigned char),
10u), _tag_arr( _temp193, sizeof( void*), 0u));}); goto _LL102; _LL124:({ void*
_temp194[ 0u]={}; Cyc_Std_printf( _tag_arr("<App>", sizeof( unsigned char), 6u),
_tag_arr( _temp194, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct(
_temp149);{ int i= 0; for( 0; i <  _get_arr_size( _temp147, sizeof( void*)); i
++){ if( i !=  0){({ void* _temp195[ 0u]={}; Cyc_Std_printf( _tag_arr(", ",
sizeof( unsigned char), 3u), _tag_arr( _temp195, sizeof( void*), 0u));});} Cyc_Typerep_xmlize_typestruct(*((
void**) _check_unknown_subscript( _temp147, sizeof( void*), i)));}}({ void*
_temp196[ 0u]={}; Cyc_Std_printf( _tag_arr("</App>", sizeof( unsigned char), 7u),
_tag_arr( _temp196, sizeof( void*), 0u));}); goto _LL102; _LL102:;} static void*
Cyc_Typerep_normalize_env( struct Cyc_Dict_Dict* env, void* ts){ void* _temp197=
ts; struct _tagged_arr* _temp221; void* _temp223; unsigned int _temp225; void*
_temp227; struct _tagged_arr _temp229; unsigned int _temp231; struct _tagged_arr
_temp233; struct _tagged_arr _temp235; void** _temp237; struct _tagged_arr
_temp239; struct _tagged_arr _temp241; void* _temp243; _LL199: if(( unsigned int)
_temp197 >  2u?*(( int*) _temp197) ==  Cyc_Typerep_Var: 0){ _LL222: _temp221=((
struct Cyc_Typerep_Var_struct*) _temp197)->f1; goto _LL200;} else{ goto _LL201;}
_LL201: if(( unsigned int) _temp197 >  2u?*(( int*) _temp197) ==  Cyc_Typerep_Int:
0){ goto _LL202;} else{ goto _LL203;} _LL203: if( _temp197 == ( void*) Cyc_Typerep_Float){
goto _LL204;} else{ goto _LL205;} _LL205: if( _temp197 == ( void*) Cyc_Typerep_Double){
goto _LL206;} else{ goto _LL207;} _LL207: if(( unsigned int) _temp197 >  2u?*((
int*) _temp197) ==  Cyc_Typerep_ThinPtr: 0){ _LL226: _temp225=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp197)->f1; goto _LL224; _LL224: _temp223=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp197)->f2; goto _LL208;} else{ goto _LL209;} _LL209: if(( unsigned int)
_temp197 >  2u?*(( int*) _temp197) ==  Cyc_Typerep_FatPtr: 0){ _LL228: _temp227=(
void*)(( struct Cyc_Typerep_FatPtr_struct*) _temp197)->f1; goto _LL210;} else{
goto _LL211;} _LL211: if(( unsigned int) _temp197 >  2u?*(( int*) _temp197) == 
Cyc_Typerep_Tuple: 0){ _LL232: _temp231=(( struct Cyc_Typerep_Tuple_struct*)
_temp197)->f1; goto _LL230; _LL230: _temp229=(( struct Cyc_Typerep_Tuple_struct*)
_temp197)->f2; goto _LL212;} else{ goto _LL213;} _LL213: if(( unsigned int)
_temp197 >  2u?*(( int*) _temp197) ==  Cyc_Typerep_TUnion: 0){ _LL234: _temp233=((
struct Cyc_Typerep_TUnion_struct*) _temp197)->f1; goto _LL214;} else{ goto
_LL215;} _LL215: if(( unsigned int) _temp197 >  2u?*(( int*) _temp197) ==  Cyc_Typerep_Union:
0){ _LL236: _temp235=(( struct Cyc_Typerep_Union_struct*) _temp197)->f1; goto
_LL216;} else{ goto _LL217;} _LL217: if(( unsigned int) _temp197 >  2u?*(( int*)
_temp197) ==  Cyc_Typerep_Forall: 0){ _LL240: _temp239=(( struct Cyc_Typerep_Forall_struct*)
_temp197)->f1; goto _LL238; _LL238: _temp237=(( struct Cyc_Typerep_Forall_struct*)
_temp197)->f2; goto _LL218;} else{ goto _LL219;} _LL219: if(( unsigned int)
_temp197 >  2u?*(( int*) _temp197) ==  Cyc_Typerep_App: 0){ _LL244: _temp243=(
void*)(( struct Cyc_Typerep_App_struct*) _temp197)->f1; goto _LL242; _LL242:
_temp241=(( struct Cyc_Typerep_App_struct*) _temp197)->f2; goto _LL220;} else{
goto _LL198;} _LL200: return(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( env,( struct _tagged_arr*) _temp221); _LL202:
goto _LL204; _LL204: goto _LL206; _LL206: return ts; _LL208: return( void*)({
struct Cyc_Typerep_ThinPtr_struct* _temp245=( struct Cyc_Typerep_ThinPtr_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_ThinPtr_struct)); _temp245[ 0]=({ struct
Cyc_Typerep_ThinPtr_struct _temp246; _temp246.tag= Cyc_Typerep_ThinPtr; _temp246.f1=
_temp225; _temp246.f2=( void*) Cyc_Typerep_normalize_env( env, _temp223);
_temp246;}); _temp245;}); _LL210: return( void*)({ struct Cyc_Typerep_FatPtr_struct*
_temp247=( struct Cyc_Typerep_FatPtr_struct*) _cycalloc( sizeof( struct Cyc_Typerep_FatPtr_struct));
_temp247[ 0]=({ struct Cyc_Typerep_FatPtr_struct _temp248; _temp248.tag= Cyc_Typerep_FatPtr;
_temp248.f1=( void*) Cyc_Typerep_normalize_env( env, _temp227); _temp248;});
_temp247;}); _LL212: { struct _tagged_arr _temp249=({ unsigned int _temp265=
_get_arr_size( _temp229, sizeof( struct _tuple0*)); struct _tuple0** _temp266=(
struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*), _temp265));
struct _tagged_arr _temp268= _tag_arr( _temp266, sizeof( struct _tuple0*),
_get_arr_size( _temp229, sizeof( struct _tuple0*)));{ unsigned int _temp267=
_temp265; unsigned int i; for( i= 0; i <  _temp267; i ++){ _temp266[ i]= 0;}};
_temp268;});{ int i= 0; for( 0; i <  _get_arr_size( _temp229, sizeof( struct
_tuple0*)); i ++){ struct _tuple0 _temp252; void* _temp253; unsigned int
_temp255; struct _tuple0* _temp250=*(( struct _tuple0**)
_check_unknown_subscript( _temp229, sizeof( struct _tuple0*), i)); _temp252=*
_temp250; _LL256: _temp255= _temp252.f1; goto _LL254; _LL254: _temp253= _temp252.f2;
goto _LL251; _LL251:*(( struct _tuple0**) _check_unknown_subscript( _temp249,
sizeof( struct _tuple0*), i))=({ struct _tuple0* _temp257=( struct _tuple0*)
_cycalloc( sizeof( struct _tuple0)); _temp257->f1= _temp255; _temp257->f2= Cyc_Typerep_normalize_env(
env, _temp253); _temp257;});}}{ struct _tagged_arr _temp258=({ unsigned int
_temp261= _get_arr_size( _temp229, sizeof( struct _tuple0*)); struct _tuple0**
_temp262=( struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*),
_temp261)); struct _tagged_arr _temp264= _tag_arr( _temp262, sizeof( struct
_tuple0*), _get_arr_size( _temp229, sizeof( struct _tuple0*)));{ unsigned int
_temp263= _temp261; unsigned int i; for( i= 0; i <  _temp263; i ++){ _temp262[ i]=(
struct _tuple0*) _check_null(*(( struct _tuple0**) _check_unknown_subscript(
_temp249, sizeof( struct _tuple0*),( int) i)));}}; _temp264;}); return( void*)({
struct Cyc_Typerep_Tuple_struct* _temp259=( struct Cyc_Typerep_Tuple_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Tuple_struct)); _temp259[ 0]=({ struct Cyc_Typerep_Tuple_struct
_temp260; _temp260.tag= Cyc_Typerep_Tuple; _temp260.f1= _temp231; _temp260.f2=(
struct _tagged_arr) _temp258; _temp260;}); _temp259;});}} _LL214: { struct
_tagged_arr _temp269=({ unsigned int _temp285= _get_arr_size( _temp233, sizeof(
struct _tuple0*)); struct _tuple0** _temp286=( struct _tuple0**) _cycalloc(
_check_times( sizeof( struct _tuple0*), _temp285)); struct _tagged_arr _temp288=
_tag_arr( _temp286, sizeof( struct _tuple0*), _get_arr_size( _temp233, sizeof(
struct _tuple0*)));{ unsigned int _temp287= _temp285; unsigned int i; for( i= 0;
i <  _temp287; i ++){ _temp286[ i]= 0;}}; _temp288;});{ int i= 0; for( 0; i < 
_get_arr_size( _temp233, sizeof( struct _tuple0*)); i ++){ struct _tuple0
_temp272; void* _temp273; unsigned int _temp275; struct _tuple0* _temp270=*((
struct _tuple0**) _check_unknown_subscript( _temp233, sizeof( struct _tuple0*),
i)); _temp272=* _temp270; _LL276: _temp275= _temp272.f1; goto _LL274; _LL274:
_temp273= _temp272.f2; goto _LL271; _LL271:*(( struct _tuple0**)
_check_unknown_subscript( _temp269, sizeof( struct _tuple0*), i))=({ struct
_tuple0* _temp277=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp277->f1= _temp275; _temp277->f2= Cyc_Typerep_normalize_env( env, _temp273);
_temp277;});}}{ struct _tagged_arr _temp278=({ unsigned int _temp281=
_get_arr_size( _temp233, sizeof( struct _tuple0*)); struct _tuple0** _temp282=(
struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*), _temp281));
struct _tagged_arr _temp284= _tag_arr( _temp282, sizeof( struct _tuple0*),
_get_arr_size( _temp233, sizeof( struct _tuple0*)));{ unsigned int _temp283=
_temp281; unsigned int i; for( i= 0; i <  _temp283; i ++){ _temp282[ i]=( struct
_tuple0*) _check_null(*(( struct _tuple0**) _check_unknown_subscript( _temp269,
sizeof( struct _tuple0*),( int) i)));}}; _temp284;}); return( void*)({ struct
Cyc_Typerep_TUnion_struct* _temp279=( struct Cyc_Typerep_TUnion_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_TUnion_struct)); _temp279[ 0]=({ struct
Cyc_Typerep_TUnion_struct _temp280; _temp280.tag= Cyc_Typerep_TUnion; _temp280.f1=(
struct _tagged_arr) _temp278; _temp280;}); _temp279;});}} _LL216: { struct
_tagged_arr _temp289=({ unsigned int _temp298= _get_arr_size( _temp235, sizeof(
void**)); void*** _temp299=( void***) _cycalloc( _check_times( sizeof( void**),
_temp298)); struct _tagged_arr _temp301= _tag_arr( _temp299, sizeof( void**),
_get_arr_size( _temp235, sizeof( void**)));{ unsigned int _temp300= _temp298;
unsigned int i; for( i= 0; i <  _temp300; i ++){ _temp299[ i]= 0;}}; _temp301;});{
int i= 0; for( 0; i <  _get_arr_size( _temp235, sizeof( void**)); i ++){*(( void***)
_check_unknown_subscript( _temp289, sizeof( void**), i))=({ void** _temp290=(
void**) _cycalloc( sizeof( void*)); _temp290[ 0]= Cyc_Typerep_normalize_env( env,*(*((
void***) _check_unknown_subscript( _temp235, sizeof( void**), i)))); _temp290;});}}{
struct _tagged_arr _temp291=({ unsigned int _temp294= _get_arr_size( _temp235,
sizeof( void**)); void*** _temp295=( void***) _cycalloc( _check_times( sizeof(
void**), _temp294)); struct _tagged_arr _temp297= _tag_arr( _temp295, sizeof(
void**), _get_arr_size( _temp235, sizeof( void**)));{ unsigned int _temp296=
_temp294; unsigned int i; for( i= 0; i <  _temp296; i ++){ _temp295[ i]=( void**)
_check_null(*(( void***) _check_unknown_subscript( _temp289, sizeof( void**),(
int) i)));}}; _temp297;}); return( void*)({ struct Cyc_Typerep_Union_struct*
_temp292=( struct Cyc_Typerep_Union_struct*) _cycalloc( sizeof( struct Cyc_Typerep_Union_struct));
_temp292[ 0]=({ struct Cyc_Typerep_Union_struct _temp293; _temp293.tag= Cyc_Typerep_Union;
_temp293.f1=( struct _tagged_arr) _temp291; _temp293;}); _temp292;});}} _LL218:
return( void*)({ struct Cyc_Typerep_Forall_struct* _temp302=( struct Cyc_Typerep_Forall_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Forall_struct)); _temp302[ 0]=({ struct
Cyc_Typerep_Forall_struct _temp303; _temp303.tag= Cyc_Typerep_Forall; _temp303.f1=
_temp239; _temp303.f2=({ void** _temp304=( void**) _cycalloc( sizeof( void*));
_temp304[ 0]= Cyc_Typerep_normalize_env( env,* _temp237); _temp304;}); _temp303;});
_temp302;}); _LL220: { void* _temp305= Cyc_Typerep_normalize_env( env, _temp243);
void** _temp311; struct _tagged_arr _temp313; _LL307: if(( unsigned int)
_temp305 >  2u?*(( int*) _temp305) ==  Cyc_Typerep_Forall: 0){ _LL314: _temp313=((
struct Cyc_Typerep_Forall_struct*) _temp305)->f1; goto _LL312; _LL312: _temp311=((
struct Cyc_Typerep_Forall_struct*) _temp305)->f2; goto _LL308;} else{ goto
_LL309;} _LL309: goto _LL310; _LL308: if( _get_arr_size( _temp241, sizeof( void*))
!=  _get_arr_size( _temp313, sizeof( struct _tagged_arr*))){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp315=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp315[ 0]=({ struct Cyc_Core_Failure_struct
_temp316; _temp316.tag= Cyc_Core_Failure; _temp316.f1= _tag_arr("app appiled wrong number of arguments",
sizeof( unsigned char), 38u); _temp316;}); _temp315;}));}{ int i= 0; for( 0; i < 
_get_arr_size( _temp241, sizeof( void*)); i ++){ env=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( env,*((
struct _tagged_arr**) _check_unknown_subscript( _temp313, sizeof( struct
_tagged_arr*), i)),*(( void**) _check_unknown_subscript( _temp241, sizeof( void*),
i)));}} return Cyc_Typerep_normalize_env( env,* _temp311); _LL310:( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp317=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp317[ 0]=({ struct Cyc_Core_Failure_struct
_temp318; _temp318.tag= Cyc_Core_Failure; _temp318.f1= _tag_arr("app misapplied",
sizeof( unsigned char), 15u); _temp318;}); _temp317;})); _LL306:;} _LL198:;} int
Cyc_Typerep_name_order( struct _tagged_arr* s1, struct _tagged_arr* s2){ return
Cyc_Std_strcmp(* s1,* s2);} void* Cyc_Typerep_normalize( void* ts){ return Cyc_Typerep_normalize_env(((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Typerep_name_order), ts);}
