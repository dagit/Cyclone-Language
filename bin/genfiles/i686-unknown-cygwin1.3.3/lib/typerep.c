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
const int Cyc_Typerep_XTUnion= 6; struct _tuple1{ struct _tagged_arr f1; void*
f2; } ; struct Cyc_Typerep_XTUnion_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Typerep_Union= 7; struct Cyc_Typerep_Union_struct{ int tag;
struct _tagged_arr f1; } ; static const int Cyc_Typerep_Forall= 8; struct Cyc_Typerep_Forall_struct{
int tag; struct _tagged_arr f1; void** f2; } ; static const int Cyc_Typerep_App=
9; struct Cyc_Typerep_App_struct{ int tag; void* f1; struct _tagged_arr f2; } ;
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
rep; struct _tagged_arr* _temp26; unsigned int _temp28; void* _temp30;
unsigned int _temp32; void* _temp34; struct _tagged_arr _temp36; unsigned int
_temp38; struct _tagged_arr _temp40; struct _tagged_arr _temp42; struct
_tagged_arr _temp44; void** _temp46; struct _tagged_arr _temp48; struct
_tagged_arr _temp50; void* _temp52; _LL2: if(( unsigned int) _temp0 >  2u?*((
int*) _temp0) ==  Cyc_Typerep_Var: 0){ _LL27: _temp26=(( struct Cyc_Typerep_Var_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 >  2u?*((
int*) _temp0) ==  Cyc_Typerep_Int: 0){ _LL29: _temp28=(( struct Cyc_Typerep_Int_struct*)
_temp0)->f1; goto _LL5;} else{ goto _LL6;} _LL6: if( _temp0 == ( void*) Cyc_Typerep_Float){
goto _LL7;} else{ goto _LL8;} _LL8: if( _temp0 == ( void*) Cyc_Typerep_Double){
goto _LL9;} else{ goto _LL10;} _LL10: if(( unsigned int) _temp0 >  2u?*(( int*)
_temp0) ==  Cyc_Typerep_ThinPtr: 0){ _LL33: _temp32=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp0)->f1; goto _LL31; _LL31: _temp30=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp0)->f2; goto _LL11;} else{ goto _LL12;} _LL12: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_FatPtr: 0){ _LL35: _temp34=( void*)((
struct Cyc_Typerep_FatPtr_struct*) _temp0)->f1; goto _LL13;} else{ goto _LL14;}
_LL14: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0) ==  Cyc_Typerep_Tuple:
0){ _LL39: _temp38=(( struct Cyc_Typerep_Tuple_struct*) _temp0)->f1; goto _LL37;
_LL37: _temp36=(( struct Cyc_Typerep_Tuple_struct*) _temp0)->f2; goto _LL15;}
else{ goto _LL16;} _LL16: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0) == 
Cyc_Typerep_TUnion: 0){ _LL41: _temp40=(( struct Cyc_Typerep_TUnion_struct*)
_temp0)->f1; goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_XTUnion: 0){ _LL43: _temp42=(( struct Cyc_Typerep_XTUnion_struct*)
_temp0)->f1; goto _LL19;} else{ goto _LL20;} _LL20: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_Union: 0){ _LL45: _temp44=(( struct Cyc_Typerep_Union_struct*)
_temp0)->f1; goto _LL21;} else{ goto _LL22;} _LL22: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_Forall: 0){ _LL49: _temp48=(( struct Cyc_Typerep_Forall_struct*)
_temp0)->f1; goto _LL47; _LL47: _temp46=(( struct Cyc_Typerep_Forall_struct*)
_temp0)->f2; goto _LL23;} else{ goto _LL24;} _LL24: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_App: 0){ _LL53: _temp52=( void*)(( struct
Cyc_Typerep_App_struct*) _temp0)->f1; goto _LL51; _LL51: _temp50=(( struct Cyc_Typerep_App_struct*)
_temp0)->f2; goto _LL25;} else{ goto _LL1;} _LL3:({ struct Cyc_Std_String_pa_struct
_temp55; _temp55.tag= Cyc_Std_String_pa; _temp55.f1=( struct _tagged_arr)*
_temp26;{ void* _temp54[ 1u]={& _temp55}; Cyc_Std_printf( _tag_arr("Var(%s)",
sizeof( unsigned char), 8u), _tag_arr( _temp54, sizeof( void*), 1u));}}); goto
_LL1; _LL5:({ struct Cyc_Std_Int_pa_struct _temp57; _temp57.tag= Cyc_Std_Int_pa;
_temp57.f1=( int) _temp28;{ void* _temp56[ 1u]={& _temp57}; Cyc_Std_printf(
_tag_arr("Int(%d)", sizeof( unsigned char), 8u), _tag_arr( _temp56, sizeof( void*),
1u));}}); goto _LL1; _LL7:({ void* _temp58[ 0u]={}; Cyc_Std_printf( _tag_arr("Float",
sizeof( unsigned char), 6u), _tag_arr( _temp58, sizeof( void*), 0u));}); goto
_LL1; _LL9:({ void* _temp59[ 0u]={}; Cyc_Std_printf( _tag_arr("Double", sizeof(
unsigned char), 7u), _tag_arr( _temp59, sizeof( void*), 0u));}); goto _LL1;
_LL11:({ struct Cyc_Std_Int_pa_struct _temp61; _temp61.tag= Cyc_Std_Int_pa;
_temp61.f1=( int) _temp32;{ void* _temp60[ 1u]={& _temp61}; Cyc_Std_printf(
_tag_arr("ThinPtr(%d,", sizeof( unsigned char), 12u), _tag_arr( _temp60, sizeof(
void*), 1u));}}); Cyc_Typerep_print_typestruct( _temp30);({ void* _temp62[ 0u]={};
Cyc_Std_printf( _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr( _temp62,
sizeof( void*), 0u));}); goto _LL1; _LL13:({ void* _temp63[ 0u]={}; Cyc_Std_printf(
_tag_arr("FatPtr(", sizeof( unsigned char), 8u), _tag_arr( _temp63, sizeof( void*),
0u));}); Cyc_Typerep_print_typestruct( _temp34);({ void* _temp64[ 0u]={}; Cyc_Std_printf(
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr( _temp64, sizeof( void*), 0u));});
goto _LL1; _LL15:({ struct Cyc_Std_Int_pa_struct _temp66; _temp66.tag= Cyc_Std_Int_pa;
_temp66.f1=( int) _temp38;{ void* _temp65[ 1u]={& _temp66}; Cyc_Std_printf(
_tag_arr("Tuple(%d,[", sizeof( unsigned char), 11u), _tag_arr( _temp65, sizeof(
void*), 1u));}});{ int i= 0; for( 0; i <  _get_arr_size( _temp36, sizeof( struct
_tuple0*)); i ++){ void* _temp69; unsigned int _temp71; struct _tuple0 _temp67=*(*((
struct _tuple0**) _check_unknown_subscript( _temp36, sizeof( struct _tuple0*), i)));
_LL72: _temp71= _temp67.f1; goto _LL70; _LL70: _temp69= _temp67.f2; goto _LL68;
_LL68:({ struct Cyc_Std_Int_pa_struct _temp74; _temp74.tag= Cyc_Std_Int_pa;
_temp74.f1=( int) _temp71;{ void* _temp73[ 1u]={& _temp74}; Cyc_Std_printf(
_tag_arr("$(%d,", sizeof( unsigned char), 6u), _tag_arr( _temp73, sizeof( void*),
1u));}}); Cyc_Typerep_print_typestruct( _temp69);({ struct Cyc_Std_Int_pa_struct
_temp76; _temp76.tag= Cyc_Std_Int_pa; _temp76.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp36, sizeof( struct _tuple0*)) -  1?',':' ')));{ void*
_temp75[ 1u]={& _temp76}; Cyc_Std_printf( _tag_arr(")%c", sizeof( unsigned char),
4u), _tag_arr( _temp75, sizeof( void*), 1u));}});}}({ void* _temp77[ 0u]={}; Cyc_Std_printf(
_tag_arr(" ]", sizeof( unsigned char), 3u), _tag_arr( _temp77, sizeof( void*), 0u));});
goto _LL1; _LL17:({ void* _temp78[ 0u]={}; Cyc_Std_printf( _tag_arr("TUnion(",
sizeof( unsigned char), 8u), _tag_arr( _temp78, sizeof( void*), 0u));});{ int i=
0; for( 0; i <  _get_arr_size( _temp40, sizeof( struct _tuple0*)); i ++){ void*
_temp81; unsigned int _temp83; struct _tuple0 _temp79=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp40, sizeof( struct _tuple0*), i))); _LL84:
_temp83= _temp79.f1; goto _LL82; _LL82: _temp81= _temp79.f2; goto _LL80; _LL80:({
struct Cyc_Std_Int_pa_struct _temp86; _temp86.tag= Cyc_Std_Int_pa; _temp86.f1=(
int) _temp83;{ void* _temp85[ 1u]={& _temp86}; Cyc_Std_printf( _tag_arr("$(%d,",
sizeof( unsigned char), 6u), _tag_arr( _temp85, sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct(
_temp81);({ struct Cyc_Std_Int_pa_struct _temp88; _temp88.tag= Cyc_Std_Int_pa;
_temp88.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size( _temp40, sizeof(
struct _tuple0*)) -  1?',':' ')));{ void* _temp87[ 1u]={& _temp88}; Cyc_Std_printf(
_tag_arr(")%c", sizeof( unsigned char), 4u), _tag_arr( _temp87, sizeof( void*),
1u));}});}}({ void* _temp89[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof(
unsigned char), 3u), _tag_arr( _temp89, sizeof( void*), 0u));}); goto _LL1;
_LL19:({ void* _temp90[ 0u]={}; Cyc_Std_printf( _tag_arr("XTUnion(", sizeof(
unsigned char), 9u), _tag_arr( _temp90, sizeof( void*), 0u));});{ int i= 0; for(
0; i <  _get_arr_size( _temp42, sizeof( struct _tuple1*)); i ++){ void* _temp93;
struct _tagged_arr _temp95; struct _tuple1 _temp91=*(*(( struct _tuple1**)
_check_unknown_subscript( _temp42, sizeof( struct _tuple1*), i))); _LL96:
_temp95= _temp91.f1; goto _LL94; _LL94: _temp93= _temp91.f2; goto _LL92; _LL92:({
struct Cyc_Std_String_pa_struct _temp98; _temp98.tag= Cyc_Std_String_pa; _temp98.f1=(
struct _tagged_arr) _tagged_arr_plus( _temp95, sizeof( unsigned char), 4);{ void*
_temp97[ 1u]={& _temp98}; Cyc_Std_printf( _tag_arr("$(%s,", sizeof(
unsigned char), 6u), _tag_arr( _temp97, sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct(
_temp93);({ struct Cyc_Std_Int_pa_struct _temp100; _temp100.tag= Cyc_Std_Int_pa;
_temp100.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size( _temp42, sizeof(
struct _tuple1*)) -  1?',':' ')));{ void* _temp99[ 1u]={& _temp100}; Cyc_Std_printf(
_tag_arr(")%c", sizeof( unsigned char), 4u), _tag_arr( _temp99, sizeof( void*),
1u));}});}}({ void* _temp101[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof(
unsigned char), 3u), _tag_arr( _temp101, sizeof( void*), 0u));}); goto _LL1;
_LL21:({ void* _temp102[ 0u]={}; Cyc_Std_printf( _tag_arr("Union(", sizeof(
unsigned char), 7u), _tag_arr( _temp102, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp44, sizeof( void**)); i ++){ Cyc_Typerep_print_typestruct(*(*((
void***) _check_unknown_subscript( _temp44, sizeof( void**), i))));({ struct Cyc_Std_Int_pa_struct
_temp104; _temp104.tag= Cyc_Std_Int_pa; _temp104.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp44, sizeof( void**)) -  1?',':' ')));{ void* _temp103[
1u]={& _temp104}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char), 3u),
_tag_arr( _temp103, sizeof( void*), 1u));}});}}({ void* _temp105[ 0u]={}; Cyc_Std_printf(
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr( _temp105, sizeof( void*), 0u));});
goto _LL1; _LL23:({ void* _temp106[ 0u]={}; Cyc_Std_printf( _tag_arr("Forall([",
sizeof( unsigned char), 9u), _tag_arr( _temp106, sizeof( void*), 0u));});{ int i=
0; for( 0; i <  _get_arr_size( _temp48, sizeof( struct _tagged_arr*)); i ++){({
void* _temp107[ 0u]={}; Cyc_Std_printf(*(*(( struct _tagged_arr**)
_check_unknown_subscript( _temp48, sizeof( struct _tagged_arr*), i))), _tag_arr(
_temp107, sizeof( void*), 0u));});({ struct Cyc_Std_Int_pa_struct _temp109;
_temp109.tag= Cyc_Std_Int_pa; _temp109.f1=( int)(( unsigned int)(( int)( i != 
_get_arr_size( _temp48, sizeof( struct _tagged_arr*)) -  1?',':' ')));{ void*
_temp108[ 1u]={& _temp109}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char),
3u), _tag_arr( _temp108, sizeof( void*), 1u));}});}}({ void* _temp110[ 0u]={};
Cyc_Std_printf( _tag_arr("],", sizeof( unsigned char), 3u), _tag_arr( _temp110,
sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct(* _temp46);({ void*
_temp111[ 0u]={}; Cyc_Std_printf( _tag_arr(")", sizeof( unsigned char), 2u),
_tag_arr( _temp111, sizeof( void*), 0u));}); goto _LL1; _LL25:({ void* _temp112[
0u]={}; Cyc_Std_printf( _tag_arr("App(", sizeof( unsigned char), 5u), _tag_arr(
_temp112, sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct( _temp52);({
void* _temp113[ 0u]={}; Cyc_Std_printf( _tag_arr(",[", sizeof( unsigned char), 3u),
_tag_arr( _temp113, sizeof( void*), 0u));});{ int i= 0; for( 0; i < 
_get_arr_size( _temp50, sizeof( void*)); i ++){ Cyc_Typerep_print_typestruct(*((
void**) _check_unknown_subscript( _temp50, sizeof( void*), i)));({ struct Cyc_Std_Int_pa_struct
_temp115; _temp115.tag= Cyc_Std_Int_pa; _temp115.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp50, sizeof( void*)) -  1?',':' ')));{ void* _temp114[
1u]={& _temp115}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char), 3u),
_tag_arr( _temp114, sizeof( void*), 1u));}});}}({ void* _temp116[ 0u]={}; Cyc_Std_printf(
_tag_arr("])", sizeof( unsigned char), 3u), _tag_arr( _temp116, sizeof( void*),
0u));}); goto _LL1; _LL1:;} void Cyc_Typerep_xmlize_typestruct( void* rep){ void*
_temp117= rep; struct _tagged_arr* _temp143; unsigned int _temp145; void*
_temp147; unsigned int _temp149; void* _temp151; struct _tagged_arr _temp153;
unsigned int _temp155; struct _tagged_arr _temp157; struct _tagged_arr _temp159;
struct _tagged_arr _temp161; void** _temp163; struct _tagged_arr _temp165;
struct _tagged_arr _temp167; void* _temp169; _LL119: if(( unsigned int) _temp117
>  2u?*(( int*) _temp117) ==  Cyc_Typerep_Var: 0){ _LL144: _temp143=(( struct
Cyc_Typerep_Var_struct*) _temp117)->f1; goto _LL120;} else{ goto _LL121;} _LL121:
if(( unsigned int) _temp117 >  2u?*(( int*) _temp117) ==  Cyc_Typerep_Int: 0){
_LL146: _temp145=(( struct Cyc_Typerep_Int_struct*) _temp117)->f1; goto _LL122;}
else{ goto _LL123;} _LL123: if( _temp117 == ( void*) Cyc_Typerep_Float){ goto
_LL124;} else{ goto _LL125;} _LL125: if( _temp117 == ( void*) Cyc_Typerep_Double){
goto _LL126;} else{ goto _LL127;} _LL127: if(( unsigned int) _temp117 >  2u?*((
int*) _temp117) ==  Cyc_Typerep_ThinPtr: 0){ _LL150: _temp149=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp117)->f1; goto _LL148; _LL148: _temp147=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp117)->f2; goto _LL128;} else{ goto _LL129;} _LL129: if(( unsigned int)
_temp117 >  2u?*(( int*) _temp117) ==  Cyc_Typerep_FatPtr: 0){ _LL152: _temp151=(
void*)(( struct Cyc_Typerep_FatPtr_struct*) _temp117)->f1; goto _LL130;} else{
goto _LL131;} _LL131: if(( unsigned int) _temp117 >  2u?*(( int*) _temp117) == 
Cyc_Typerep_Tuple: 0){ _LL156: _temp155=(( struct Cyc_Typerep_Tuple_struct*)
_temp117)->f1; goto _LL154; _LL154: _temp153=(( struct Cyc_Typerep_Tuple_struct*)
_temp117)->f2; goto _LL132;} else{ goto _LL133;} _LL133: if(( unsigned int)
_temp117 >  2u?*(( int*) _temp117) ==  Cyc_Typerep_TUnion: 0){ _LL158: _temp157=((
struct Cyc_Typerep_TUnion_struct*) _temp117)->f1; goto _LL134;} else{ goto
_LL135;} _LL135: if(( unsigned int) _temp117 >  2u?*(( int*) _temp117) ==  Cyc_Typerep_XTUnion:
0){ _LL160: _temp159=(( struct Cyc_Typerep_XTUnion_struct*) _temp117)->f1; goto
_LL136;} else{ goto _LL137;} _LL137: if(( unsigned int) _temp117 >  2u?*(( int*)
_temp117) ==  Cyc_Typerep_Union: 0){ _LL162: _temp161=(( struct Cyc_Typerep_Union_struct*)
_temp117)->f1; goto _LL138;} else{ goto _LL139;} _LL139: if(( unsigned int)
_temp117 >  2u?*(( int*) _temp117) ==  Cyc_Typerep_Forall: 0){ _LL166: _temp165=((
struct Cyc_Typerep_Forall_struct*) _temp117)->f1; goto _LL164; _LL164: _temp163=((
struct Cyc_Typerep_Forall_struct*) _temp117)->f2; goto _LL140;} else{ goto
_LL141;} _LL141: if(( unsigned int) _temp117 >  2u?*(( int*) _temp117) ==  Cyc_Typerep_App:
0){ _LL170: _temp169=( void*)(( struct Cyc_Typerep_App_struct*) _temp117)->f1;
goto _LL168; _LL168: _temp167=(( struct Cyc_Typerep_App_struct*) _temp117)->f2;
goto _LL142;} else{ goto _LL118;} _LL120:({ struct Cyc_Std_String_pa_struct
_temp172; _temp172.tag= Cyc_Std_String_pa; _temp172.f1=( struct _tagged_arr)*
_temp143;{ void* _temp171[ 1u]={& _temp172}; Cyc_Std_printf( _tag_arr("<Var name=\"%s\"/>",
sizeof( unsigned char), 17u), _tag_arr( _temp171, sizeof( void*), 1u));}}); goto
_LL118; _LL122:({ struct Cyc_Std_Int_pa_struct _temp174; _temp174.tag= Cyc_Std_Int_pa;
_temp174.f1=( int) _temp145;{ void* _temp173[ 1u]={& _temp174}; Cyc_Std_printf(
_tag_arr("<Int sz=\"%d\"/>", sizeof( unsigned char), 15u), _tag_arr( _temp173,
sizeof( void*), 1u));}}); goto _LL118; _LL124:({ void* _temp175[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Float/>", sizeof( unsigned char), 9u), _tag_arr( _temp175, sizeof(
void*), 0u));}); goto _LL118; _LL126:({ void* _temp176[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Double/>", sizeof( unsigned char), 10u), _tag_arr( _temp176, sizeof(
void*), 0u));}); goto _LL118; _LL128:({ struct Cyc_Std_Int_pa_struct _temp178;
_temp178.tag= Cyc_Std_Int_pa; _temp178.f1=( int) _temp149;{ void* _temp177[ 1u]={&
_temp178}; Cyc_Std_printf( _tag_arr("<ThinPtr sz=\"%d\">", sizeof( unsigned char),
18u), _tag_arr( _temp177, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp147);({ void* _temp179[ 0u]={}; Cyc_Std_printf( _tag_arr("</ThinPtr",
sizeof( unsigned char), 10u), _tag_arr( _temp179, sizeof( void*), 0u));}); goto
_LL118; _LL130:({ void* _temp180[ 0u]={}; Cyc_Std_printf( _tag_arr("<FatPtr>",
sizeof( unsigned char), 9u), _tag_arr( _temp180, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct(
_temp151);({ void* _temp181[ 0u]={}; Cyc_Std_printf( _tag_arr("</FatPtr>",
sizeof( unsigned char), 10u), _tag_arr( _temp181, sizeof( void*), 0u));}); goto
_LL118; _LL132:({ struct Cyc_Std_Int_pa_struct _temp183; _temp183.tag= Cyc_Std_Int_pa;
_temp183.f1=( int) _temp155;{ void* _temp182[ 1u]={& _temp183}; Cyc_Std_printf(
_tag_arr("<Tuple sz=\"%d\">", sizeof( unsigned char), 16u), _tag_arr( _temp182,
sizeof( void*), 1u));}});{ int i= 0; for( 0; i <  _get_arr_size( _temp153,
sizeof( struct _tuple0*)); i ++){ void* _temp186; unsigned int _temp188; struct
_tuple0 _temp184=*(*(( struct _tuple0**) _check_unknown_subscript( _temp153,
sizeof( struct _tuple0*), i))); _LL189: _temp188= _temp184.f1; goto _LL187;
_LL187: _temp186= _temp184.f2; goto _LL185; _LL185:({ struct Cyc_Std_Int_pa_struct
_temp191; _temp191.tag= Cyc_Std_Int_pa; _temp191.f1=( int) _temp188;{ void*
_temp190[ 1u]={& _temp191}; Cyc_Std_printf( _tag_arr("<Component ofs=\"%d\">",
sizeof( unsigned char), 21u), _tag_arr( _temp190, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp186);({ void* _temp192[ 0u]={}; Cyc_Std_printf( _tag_arr("</Component>",
sizeof( unsigned char), 13u), _tag_arr( _temp192, sizeof( void*), 0u));});}}({
void* _temp193[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tuple>", sizeof(
unsigned char), 9u), _tag_arr( _temp193, sizeof( void*), 0u));}); goto _LL118;
_LL134:({ void* _temp194[ 0u]={}; Cyc_Std_printf( _tag_arr("<TUnion>", sizeof(
unsigned char), 9u), _tag_arr( _temp194, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp157, sizeof( struct _tuple0*)); i ++){ void*
_temp197; unsigned int _temp199; struct _tuple0 _temp195=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp157, sizeof( struct _tuple0*), i))); _LL200:
_temp199= _temp195.f1; goto _LL198; _LL198: _temp197= _temp195.f2; goto _LL196;
_LL196:({ struct Cyc_Std_Int_pa_struct _temp202; _temp202.tag= Cyc_Std_Int_pa;
_temp202.f1=( int) _temp199;{ void* _temp201[ 1u]={& _temp202}; Cyc_Std_printf(
_tag_arr("<Tag tag=\"%d\">", sizeof( unsigned char), 15u), _tag_arr( _temp201,
sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct( _temp197);({ void*
_temp203[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tag>", sizeof( unsigned char), 7u),
_tag_arr( _temp203, sizeof( void*), 0u));});}}({ void* _temp204[ 0u]={}; Cyc_Std_printf(
_tag_arr("</Tunion>", sizeof( unsigned char), 10u), _tag_arr( _temp204, sizeof(
void*), 0u));}); goto _LL118; _LL136:({ void* _temp205[ 0u]={}; Cyc_Std_printf(
_tag_arr("<XTUnion>", sizeof( unsigned char), 10u), _tag_arr( _temp205, sizeof(
void*), 0u));});{ int i= 0; for( 0; i <  _get_arr_size( _temp159, sizeof( struct
_tuple1*)); i ++){ void* _temp208; struct _tagged_arr _temp210; struct _tuple1
_temp206=*(*(( struct _tuple1**) _check_unknown_subscript( _temp159, sizeof(
struct _tuple1*), i))); _LL211: _temp210= _temp206.f1; goto _LL209; _LL209:
_temp208= _temp206.f2; goto _LL207; _LL207:({ struct Cyc_Std_String_pa_struct
_temp213; _temp213.tag= Cyc_Std_String_pa; _temp213.f1=( struct _tagged_arr)
_tagged_arr_plus( _temp210, sizeof( unsigned char), 4);{ void* _temp212[ 1u]={&
_temp213}; Cyc_Std_printf( _tag_arr("<Tag tag=\"%s\">", sizeof( unsigned char),
15u), _tag_arr( _temp212, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp208);({ void* _temp214[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tag>", sizeof(
unsigned char), 7u), _tag_arr( _temp214, sizeof( void*), 0u));});}}({ void*
_temp215[ 0u]={}; Cyc_Std_printf( _tag_arr("</XTunion>", sizeof( unsigned char),
11u), _tag_arr( _temp215, sizeof( void*), 0u));}); goto _LL118; _LL138:({ void*
_temp216[ 0u]={}; Cyc_Std_printf( _tag_arr("<Union>", sizeof( unsigned char), 8u),
_tag_arr( _temp216, sizeof( void*), 0u));});{ int i= 0; for( 0; i < 
_get_arr_size( _temp161, sizeof( void**)); i ++){({ void* _temp217[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Case>", sizeof( unsigned char), 7u), _tag_arr( _temp217, sizeof( void*),
0u));}); Cyc_Typerep_xmlize_typestruct(*(*(( void***) _check_unknown_subscript(
_temp161, sizeof( void**), i))));({ void* _temp218[ 0u]={}; Cyc_Std_printf(
_tag_arr("</Case>", sizeof( unsigned char), 8u), _tag_arr( _temp218, sizeof(
void*), 0u));});}}({ void* _temp219[ 0u]={}; Cyc_Std_printf( _tag_arr("</Union>",
sizeof( unsigned char), 9u), _tag_arr( _temp219, sizeof( void*), 0u));}); goto
_LL118; _LL140:({ void* _temp220[ 0u]={}; Cyc_Std_printf( _tag_arr("<Forall vars=\"",
sizeof( unsigned char), 15u), _tag_arr( _temp220, sizeof( void*), 0u));});{ int
i= 0; for( 0; i <  _get_arr_size( _temp165, sizeof( struct _tagged_arr*)); i ++){
if( i !=  0){({ void* _temp221[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp221, sizeof( void*), 0u));});}({ void*
_temp222[ 0u]={}; Cyc_Std_printf(*(*(( struct _tagged_arr**)
_check_unknown_subscript( _temp165, sizeof( struct _tagged_arr*), i))), _tag_arr(
_temp222, sizeof( void*), 0u));});}}({ void* _temp223[ 0u]={}; Cyc_Std_printf(
_tag_arr("\">", sizeof( unsigned char), 3u), _tag_arr( _temp223, sizeof( void*),
0u));}); Cyc_Typerep_xmlize_typestruct(* _temp163);({ void* _temp224[ 0u]={};
Cyc_Std_printf( _tag_arr("</Forall>", sizeof( unsigned char), 10u), _tag_arr(
_temp224, sizeof( void*), 0u));}); goto _LL118; _LL142:({ void* _temp225[ 0u]={};
Cyc_Std_printf( _tag_arr("<App>", sizeof( unsigned char), 6u), _tag_arr(
_temp225, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct( _temp169);{
int i= 0; for( 0; i <  _get_arr_size( _temp167, sizeof( void*)); i ++){ if( i != 
0){({ void* _temp226[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp226, sizeof( void*), 0u));});} Cyc_Typerep_xmlize_typestruct(*((
void**) _check_unknown_subscript( _temp167, sizeof( void*), i)));}}({ void*
_temp227[ 0u]={}; Cyc_Std_printf( _tag_arr("</App>", sizeof( unsigned char), 7u),
_tag_arr( _temp227, sizeof( void*), 0u));}); goto _LL118; _LL118:;} static void*
Cyc_Typerep_normalize_env( struct Cyc_Dict_Dict* env, void* ts){ void* _temp228=
ts; struct _tagged_arr* _temp254; void* _temp256; unsigned int _temp258; void*
_temp260; struct _tagged_arr _temp262; unsigned int _temp264; struct _tagged_arr
_temp266; struct _tagged_arr _temp268; struct _tagged_arr _temp270; void**
_temp272; struct _tagged_arr _temp274; struct _tagged_arr _temp276; void*
_temp278; _LL230: if(( unsigned int) _temp228 >  2u?*(( int*) _temp228) ==  Cyc_Typerep_Var:
0){ _LL255: _temp254=(( struct Cyc_Typerep_Var_struct*) _temp228)->f1; goto
_LL231;} else{ goto _LL232;} _LL232: if(( unsigned int) _temp228 >  2u?*(( int*)
_temp228) ==  Cyc_Typerep_Int: 0){ goto _LL233;} else{ goto _LL234;} _LL234: if(
_temp228 == ( void*) Cyc_Typerep_Float){ goto _LL235;} else{ goto _LL236;}
_LL236: if( _temp228 == ( void*) Cyc_Typerep_Double){ goto _LL237;} else{ goto
_LL238;} _LL238: if(( unsigned int) _temp228 >  2u?*(( int*) _temp228) ==  Cyc_Typerep_ThinPtr:
0){ _LL259: _temp258=(( struct Cyc_Typerep_ThinPtr_struct*) _temp228)->f1; goto
_LL257; _LL257: _temp256=( void*)(( struct Cyc_Typerep_ThinPtr_struct*) _temp228)->f2;
goto _LL239;} else{ goto _LL240;} _LL240: if(( unsigned int) _temp228 >  2u?*((
int*) _temp228) ==  Cyc_Typerep_FatPtr: 0){ _LL261: _temp260=( void*)(( struct
Cyc_Typerep_FatPtr_struct*) _temp228)->f1; goto _LL241;} else{ goto _LL242;}
_LL242: if(( unsigned int) _temp228 >  2u?*(( int*) _temp228) ==  Cyc_Typerep_Tuple:
0){ _LL265: _temp264=(( struct Cyc_Typerep_Tuple_struct*) _temp228)->f1; goto
_LL263; _LL263: _temp262=(( struct Cyc_Typerep_Tuple_struct*) _temp228)->f2;
goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int) _temp228 >  2u?*((
int*) _temp228) ==  Cyc_Typerep_TUnion: 0){ _LL267: _temp266=(( struct Cyc_Typerep_TUnion_struct*)
_temp228)->f1; goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int)
_temp228 >  2u?*(( int*) _temp228) ==  Cyc_Typerep_XTUnion: 0){ _LL269: _temp268=((
struct Cyc_Typerep_XTUnion_struct*) _temp228)->f1; goto _LL247;} else{ goto
_LL248;} _LL248: if(( unsigned int) _temp228 >  2u?*(( int*) _temp228) ==  Cyc_Typerep_Union:
0){ _LL271: _temp270=(( struct Cyc_Typerep_Union_struct*) _temp228)->f1; goto
_LL249;} else{ goto _LL250;} _LL250: if(( unsigned int) _temp228 >  2u?*(( int*)
_temp228) ==  Cyc_Typerep_Forall: 0){ _LL275: _temp274=(( struct Cyc_Typerep_Forall_struct*)
_temp228)->f1; goto _LL273; _LL273: _temp272=(( struct Cyc_Typerep_Forall_struct*)
_temp228)->f2; goto _LL251;} else{ goto _LL252;} _LL252: if(( unsigned int)
_temp228 >  2u?*(( int*) _temp228) ==  Cyc_Typerep_App: 0){ _LL279: _temp278=(
void*)(( struct Cyc_Typerep_App_struct*) _temp228)->f1; goto _LL277; _LL277:
_temp276=(( struct Cyc_Typerep_App_struct*) _temp228)->f2; goto _LL253;} else{
goto _LL229;} _LL231: return(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( env,( struct _tagged_arr*) _temp254); _LL233:
goto _LL235; _LL235: goto _LL237; _LL237: return ts; _LL239: return( void*)({
struct Cyc_Typerep_ThinPtr_struct* _temp280=( struct Cyc_Typerep_ThinPtr_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_ThinPtr_struct)); _temp280[ 0]=({ struct
Cyc_Typerep_ThinPtr_struct _temp281; _temp281.tag= Cyc_Typerep_ThinPtr; _temp281.f1=
_temp258; _temp281.f2=( void*) Cyc_Typerep_normalize_env( env, _temp256);
_temp281;}); _temp280;}); _LL241: return( void*)({ struct Cyc_Typerep_FatPtr_struct*
_temp282=( struct Cyc_Typerep_FatPtr_struct*) _cycalloc( sizeof( struct Cyc_Typerep_FatPtr_struct));
_temp282[ 0]=({ struct Cyc_Typerep_FatPtr_struct _temp283; _temp283.tag= Cyc_Typerep_FatPtr;
_temp283.f1=( void*) Cyc_Typerep_normalize_env( env, _temp260); _temp283;});
_temp282;}); _LL243: { struct _tagged_arr _temp284=({ unsigned int _temp300=
_get_arr_size( _temp262, sizeof( struct _tuple0*)); struct _tuple0** _temp301=(
struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*), _temp300));
struct _tagged_arr _temp303= _tag_arr( _temp301, sizeof( struct _tuple0*),
_get_arr_size( _temp262, sizeof( struct _tuple0*)));{ unsigned int _temp302=
_temp300; unsigned int i; for( i= 0; i <  _temp302; i ++){ _temp301[ i]= 0;}};
_temp303;});{ int i= 0; for( 0; i <  _get_arr_size( _temp262, sizeof( struct
_tuple0*)); i ++){ struct _tuple0 _temp287; void* _temp288; unsigned int
_temp290; struct _tuple0* _temp285=*(( struct _tuple0**)
_check_unknown_subscript( _temp262, sizeof( struct _tuple0*), i)); _temp287=*
_temp285; _LL291: _temp290= _temp287.f1; goto _LL289; _LL289: _temp288= _temp287.f2;
goto _LL286; _LL286:*(( struct _tuple0**) _check_unknown_subscript( _temp284,
sizeof( struct _tuple0*), i))=({ struct _tuple0* _temp292=( struct _tuple0*)
_cycalloc( sizeof( struct _tuple0)); _temp292->f1= _temp290; _temp292->f2= Cyc_Typerep_normalize_env(
env, _temp288); _temp292;});}}{ struct _tagged_arr _temp293=({ unsigned int
_temp296= _get_arr_size( _temp262, sizeof( struct _tuple0*)); struct _tuple0**
_temp297=( struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*),
_temp296)); struct _tagged_arr _temp299= _tag_arr( _temp297, sizeof( struct
_tuple0*), _get_arr_size( _temp262, sizeof( struct _tuple0*)));{ unsigned int
_temp298= _temp296; unsigned int i; for( i= 0; i <  _temp298; i ++){ _temp297[ i]=(
struct _tuple0*) _check_null(*(( struct _tuple0**) _check_unknown_subscript(
_temp284, sizeof( struct _tuple0*),( int) i)));}}; _temp299;}); return( void*)({
struct Cyc_Typerep_Tuple_struct* _temp294=( struct Cyc_Typerep_Tuple_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Tuple_struct)); _temp294[ 0]=({ struct Cyc_Typerep_Tuple_struct
_temp295; _temp295.tag= Cyc_Typerep_Tuple; _temp295.f1= _temp264; _temp295.f2=(
struct _tagged_arr) _temp293; _temp295;}); _temp294;});}} _LL245: { struct
_tagged_arr _temp304=({ unsigned int _temp320= _get_arr_size( _temp266, sizeof(
struct _tuple0*)); struct _tuple0** _temp321=( struct _tuple0**) _cycalloc(
_check_times( sizeof( struct _tuple0*), _temp320)); struct _tagged_arr _temp323=
_tag_arr( _temp321, sizeof( struct _tuple0*), _get_arr_size( _temp266, sizeof(
struct _tuple0*)));{ unsigned int _temp322= _temp320; unsigned int i; for( i= 0;
i <  _temp322; i ++){ _temp321[ i]= 0;}}; _temp323;});{ int i= 0; for( 0; i < 
_get_arr_size( _temp266, sizeof( struct _tuple0*)); i ++){ struct _tuple0
_temp307; void* _temp308; unsigned int _temp310; struct _tuple0* _temp305=*((
struct _tuple0**) _check_unknown_subscript( _temp266, sizeof( struct _tuple0*),
i)); _temp307=* _temp305; _LL311: _temp310= _temp307.f1; goto _LL309; _LL309:
_temp308= _temp307.f2; goto _LL306; _LL306:*(( struct _tuple0**)
_check_unknown_subscript( _temp304, sizeof( struct _tuple0*), i))=({ struct
_tuple0* _temp312=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp312->f1= _temp310; _temp312->f2= Cyc_Typerep_normalize_env( env, _temp308);
_temp312;});}}{ struct _tagged_arr _temp313=({ unsigned int _temp316=
_get_arr_size( _temp266, sizeof( struct _tuple0*)); struct _tuple0** _temp317=(
struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*), _temp316));
struct _tagged_arr _temp319= _tag_arr( _temp317, sizeof( struct _tuple0*),
_get_arr_size( _temp266, sizeof( struct _tuple0*)));{ unsigned int _temp318=
_temp316; unsigned int i; for( i= 0; i <  _temp318; i ++){ _temp317[ i]=( struct
_tuple0*) _check_null(*(( struct _tuple0**) _check_unknown_subscript( _temp304,
sizeof( struct _tuple0*),( int) i)));}}; _temp319;}); return( void*)({ struct
Cyc_Typerep_TUnion_struct* _temp314=( struct Cyc_Typerep_TUnion_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_TUnion_struct)); _temp314[ 0]=({ struct
Cyc_Typerep_TUnion_struct _temp315; _temp315.tag= Cyc_Typerep_TUnion; _temp315.f1=(
struct _tagged_arr) _temp313; _temp315;}); _temp314;});}} _LL247: { struct
_tagged_arr _temp324=({ unsigned int _temp340= _get_arr_size( _temp268, sizeof(
struct _tuple1*)); struct _tuple1** _temp341=( struct _tuple1**) _cycalloc(
_check_times( sizeof( struct _tuple1*), _temp340)); struct _tagged_arr _temp343=
_tag_arr( _temp341, sizeof( struct _tuple1*), _get_arr_size( _temp268, sizeof(
struct _tuple1*)));{ unsigned int _temp342= _temp340; unsigned int i; for( i= 0;
i <  _temp342; i ++){ _temp341[ i]= 0;}}; _temp343;});{ int i= 0; for( 0; i < 
_get_arr_size( _temp268, sizeof( struct _tuple1*)); i ++){ struct _tuple1
_temp327; void* _temp328; struct _tagged_arr _temp330; struct _tuple1* _temp325=*((
struct _tuple1**) _check_unknown_subscript( _temp268, sizeof( struct _tuple1*),
i)); _temp327=* _temp325; _LL331: _temp330= _temp327.f1; goto _LL329; _LL329:
_temp328= _temp327.f2; goto _LL326; _LL326:*(( struct _tuple1**)
_check_unknown_subscript( _temp324, sizeof( struct _tuple1*), i))=({ struct
_tuple1* _temp332=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp332->f1= _temp330; _temp332->f2= Cyc_Typerep_normalize_env( env, _temp328);
_temp332;});}}{ struct _tagged_arr _temp333=({ unsigned int _temp336=
_get_arr_size( _temp268, sizeof( struct _tuple1*)); struct _tuple1** _temp337=(
struct _tuple1**) _cycalloc( _check_times( sizeof( struct _tuple1*), _temp336));
struct _tagged_arr _temp339= _tag_arr( _temp337, sizeof( struct _tuple1*),
_get_arr_size( _temp268, sizeof( struct _tuple1*)));{ unsigned int _temp338=
_temp336; unsigned int i; for( i= 0; i <  _temp338; i ++){ _temp337[ i]=( struct
_tuple1*) _check_null(*(( struct _tuple1**) _check_unknown_subscript( _temp324,
sizeof( struct _tuple1*),( int) i)));}}; _temp339;}); return( void*)({ struct
Cyc_Typerep_XTUnion_struct* _temp334=( struct Cyc_Typerep_XTUnion_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_XTUnion_struct)); _temp334[ 0]=({ struct
Cyc_Typerep_XTUnion_struct _temp335; _temp335.tag= Cyc_Typerep_XTUnion; _temp335.f1=(
struct _tagged_arr) _temp333; _temp335;}); _temp334;});}} _LL249: { struct
_tagged_arr _temp344=({ unsigned int _temp353= _get_arr_size( _temp270, sizeof(
void**)); void*** _temp354=( void***) _cycalloc( _check_times( sizeof( void**),
_temp353)); struct _tagged_arr _temp356= _tag_arr( _temp354, sizeof( void**),
_get_arr_size( _temp270, sizeof( void**)));{ unsigned int _temp355= _temp353;
unsigned int i; for( i= 0; i <  _temp355; i ++){ _temp354[ i]= 0;}}; _temp356;});{
int i= 0; for( 0; i <  _get_arr_size( _temp270, sizeof( void**)); i ++){*(( void***)
_check_unknown_subscript( _temp344, sizeof( void**), i))=({ void** _temp345=(
void**) _cycalloc( sizeof( void*)); _temp345[ 0]= Cyc_Typerep_normalize_env( env,*(*((
void***) _check_unknown_subscript( _temp270, sizeof( void**), i)))); _temp345;});}}{
struct _tagged_arr _temp346=({ unsigned int _temp349= _get_arr_size( _temp270,
sizeof( void**)); void*** _temp350=( void***) _cycalloc( _check_times( sizeof(
void**), _temp349)); struct _tagged_arr _temp352= _tag_arr( _temp350, sizeof(
void**), _get_arr_size( _temp270, sizeof( void**)));{ unsigned int _temp351=
_temp349; unsigned int i; for( i= 0; i <  _temp351; i ++){ _temp350[ i]=( void**)
_check_null(*(( void***) _check_unknown_subscript( _temp344, sizeof( void**),(
int) i)));}}; _temp352;}); return( void*)({ struct Cyc_Typerep_Union_struct*
_temp347=( struct Cyc_Typerep_Union_struct*) _cycalloc( sizeof( struct Cyc_Typerep_Union_struct));
_temp347[ 0]=({ struct Cyc_Typerep_Union_struct _temp348; _temp348.tag= Cyc_Typerep_Union;
_temp348.f1=( struct _tagged_arr) _temp346; _temp348;}); _temp347;});}} _LL251:
return( void*)({ struct Cyc_Typerep_Forall_struct* _temp357=( struct Cyc_Typerep_Forall_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Forall_struct)); _temp357[ 0]=({ struct
Cyc_Typerep_Forall_struct _temp358; _temp358.tag= Cyc_Typerep_Forall; _temp358.f1=
_temp274; _temp358.f2=({ void** _temp359=( void**) _cycalloc( sizeof( void*));
_temp359[ 0]= Cyc_Typerep_normalize_env( env,* _temp272); _temp359;}); _temp358;});
_temp357;}); _LL253: { void* _temp360= Cyc_Typerep_normalize_env( env, _temp278);
void** _temp366; struct _tagged_arr _temp368; _LL362: if(( unsigned int)
_temp360 >  2u?*(( int*) _temp360) ==  Cyc_Typerep_Forall: 0){ _LL369: _temp368=((
struct Cyc_Typerep_Forall_struct*) _temp360)->f1; goto _LL367; _LL367: _temp366=((
struct Cyc_Typerep_Forall_struct*) _temp360)->f2; goto _LL363;} else{ goto
_LL364;} _LL364: goto _LL365; _LL363: if( _get_arr_size( _temp276, sizeof( void*))
!=  _get_arr_size( _temp368, sizeof( struct _tagged_arr*))){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp370=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp370[ 0]=({ struct Cyc_Core_Failure_struct
_temp371; _temp371.tag= Cyc_Core_Failure; _temp371.f1= _tag_arr("app appiled wrong number of arguments",
sizeof( unsigned char), 38u); _temp371;}); _temp370;}));}{ int i= 0; for( 0; i < 
_get_arr_size( _temp276, sizeof( void*)); i ++){ env=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( env,*((
struct _tagged_arr**) _check_unknown_subscript( _temp368, sizeof( struct
_tagged_arr*), i)),*(( void**) _check_unknown_subscript( _temp276, sizeof( void*),
i)));}} return Cyc_Typerep_normalize_env( env,* _temp366); _LL365:( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp372=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp372[ 0]=({ struct Cyc_Core_Failure_struct
_temp373; _temp373.tag= Cyc_Core_Failure; _temp373.f1= _tag_arr("app misapplied",
sizeof( unsigned char), 15u); _temp373;}); _temp372;})); _LL361:;} _LL229:;} int
Cyc_Typerep_name_order( struct _tagged_arr* s1, struct _tagged_arr* s2){ return
Cyc_Std_strcmp(* s1,* s2);} void* Cyc_Typerep_normalize( void* ts){ return Cyc_Typerep_normalize_env(((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Typerep_name_order), ts);}
