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
18u]; extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Set_Set; extern struct
Cyc_Set_Set* Cyc_Set_empty( int(* cmp)( void*, void*)); extern struct Cyc_Set_Set*
Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[ 11u];
static const int Cyc_Typerep_Var= 0; struct Cyc_Typerep_Var_struct{ int tag;
struct _tagged_arr* f1; } ; static const int Cyc_Typerep_Int= 1; struct Cyc_Typerep_Int_struct{
int tag; unsigned int f1; } ; static const int Cyc_Typerep_Float= 0; static
const int Cyc_Typerep_Double= 1; static const int Cyc_Typerep_ThinPtr= 2; struct
Cyc_Typerep_ThinPtr_struct{ int tag; unsigned int f1; void* f2; } ; static const
int Cyc_Typerep_FatPtr= 3; struct Cyc_Typerep_FatPtr_struct{ int tag; void* f1;
} ; static const int Cyc_Typerep_Tuple= 4; struct _tuple0{ unsigned int f1; void*
f2; } ; struct Cyc_Typerep_Tuple_struct{ int tag; unsigned int f1; struct
_tagged_arr f2; } ; static const int Cyc_Typerep_TUnion= 5; struct Cyc_Typerep_TUnion_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Typerep_XTUnion= 6;
struct _tuple1{ struct _tagged_arr f1; void* f2; } ; struct Cyc_Typerep_XTUnion_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Typerep_Union= 7;
struct Cyc_Typerep_Union_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Typerep_Forall= 8; struct Cyc_Typerep_Forall_struct{ int tag;
struct _tagged_arr f1; void** f2; } ; static const int Cyc_Typerep_App= 9;
struct Cyc_Typerep_App_struct{ int tag; void* f1; struct _tagged_arr f2; } ;
extern void Cyc_Typerep_print_typestruct( void* rep); extern void Cyc_Typerep_xmlize_typestruct(
void* rep); extern void* Cyc_Typerep_normalize( void* ts); extern struct Cyc_Set_Set*
Cyc_Typerep_empty_addr_set(); struct _tuple2{ int f1; struct Cyc_Set_Set* f2; }
; extern struct _tuple2* Cyc_Typerep_member_insert( struct Cyc_Set_Set*,
unsigned int); extern void* Cyc_Typerep_get_unionbranch( unsigned int tag,
struct _tagged_arr l); extern void* Cyc_Typerep_get_xtunionbranch( struct
_tagged_arr tag, struct _tagged_arr l); unsigned int Cyc_Typerep_size_type( void*
rep); struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u];
extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_printf( struct _tagged_arr fmt,
struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* cmp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* k, void* v); extern void* Cyc_Dict_lookup( struct
Cyc_Dict_Dict* d, void* k); extern int Cyc_Std_strcmp( struct _tagged_arr s1,
struct _tagged_arr s2); static int Cyc_Typerep_int_cmp( unsigned int a,
unsigned int b){ if( a ==  b){ return 0;} else{ if( a <  b){ return 1;} else{
return - 1;}}} struct Cyc_Set_Set* Cyc_Typerep_empty_addr_set(){ return(( struct
Cyc_Set_Set*(*)( int(* cmp)( unsigned int, unsigned int))) Cyc_Set_empty)( Cyc_Typerep_int_cmp);}
void* Cyc_Typerep_get_unionbranch( unsigned int tag, struct _tagged_arr l){{ int
i= 0; for( 0; i <  _get_arr_size( l, sizeof( struct _tuple0*)); i ++){ void*
_temp2; unsigned int _temp4; struct _tuple0 _temp0=*(*(( struct _tuple0**)
_check_unknown_subscript( l, sizeof( struct _tuple0*), i))); _LL5: _temp4=
_temp0.f1; goto _LL3; _LL3: _temp2= _temp0.f2; goto _LL1; _LL1: if( _temp4 == 
tag){ return _temp2;}}}( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
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
return( unsigned int) max;} _LL37: return sizeof( void*); _LL39: return sizeof(
_temp48); _LL41: return sizeof( _temp50); _LL17:;} struct _tuple2* Cyc_Typerep_member_insert(
struct Cyc_Set_Set* set, unsigned int val){ return({ struct _tuple2* _temp53=(
struct _tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp53->f1=(( int(*)(
struct Cyc_Set_Set* s, unsigned int elt)) Cyc_Set_member)( set, val); _temp53->f2=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, unsigned int elt)) Cyc_Set_insert)(
set, val); _temp53;});} void Cyc_Typerep_print_typestruct( void* rep){ void*
_temp54= rep; struct _tagged_arr* _temp80; unsigned int _temp82; void* _temp84;
unsigned int _temp86; void* _temp88; struct _tagged_arr _temp90; unsigned int
_temp92; struct _tagged_arr _temp94; struct _tagged_arr _temp96; struct
_tagged_arr _temp98; void** _temp100; struct _tagged_arr _temp102; struct
_tagged_arr _temp104; void* _temp106; _LL56: if(( unsigned int) _temp54 >  2u?*((
int*) _temp54) ==  Cyc_Typerep_Var: 0){ _LL81: _temp80=(( struct Cyc_Typerep_Var_struct*)
_temp54)->f1; goto _LL57;} else{ goto _LL58;} _LL58: if(( unsigned int) _temp54
>  2u?*(( int*) _temp54) ==  Cyc_Typerep_Int: 0){ _LL83: _temp82=(( struct Cyc_Typerep_Int_struct*)
_temp54)->f1; goto _LL59;} else{ goto _LL60;} _LL60: if( _temp54 == ( void*) Cyc_Typerep_Float){
goto _LL61;} else{ goto _LL62;} _LL62: if( _temp54 == ( void*) Cyc_Typerep_Double){
goto _LL63;} else{ goto _LL64;} _LL64: if(( unsigned int) _temp54 >  2u?*(( int*)
_temp54) ==  Cyc_Typerep_ThinPtr: 0){ _LL87: _temp86=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp54)->f1; goto _LL85; _LL85: _temp84=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp54)->f2; goto _LL65;} else{ goto _LL66;} _LL66: if(( unsigned int) _temp54
>  2u?*(( int*) _temp54) ==  Cyc_Typerep_FatPtr: 0){ _LL89: _temp88=( void*)((
struct Cyc_Typerep_FatPtr_struct*) _temp54)->f1; goto _LL67;} else{ goto _LL68;}
_LL68: if(( unsigned int) _temp54 >  2u?*(( int*) _temp54) ==  Cyc_Typerep_Tuple:
0){ _LL93: _temp92=(( struct Cyc_Typerep_Tuple_struct*) _temp54)->f1; goto _LL91;
_LL91: _temp90=(( struct Cyc_Typerep_Tuple_struct*) _temp54)->f2; goto _LL69;}
else{ goto _LL70;} _LL70: if(( unsigned int) _temp54 >  2u?*(( int*) _temp54) == 
Cyc_Typerep_TUnion: 0){ _LL95: _temp94=(( struct Cyc_Typerep_TUnion_struct*)
_temp54)->f1; goto _LL71;} else{ goto _LL72;} _LL72: if(( unsigned int) _temp54
>  2u?*(( int*) _temp54) ==  Cyc_Typerep_XTUnion: 0){ _LL97: _temp96=(( struct
Cyc_Typerep_XTUnion_struct*) _temp54)->f1; goto _LL73;} else{ goto _LL74;} _LL74:
if(( unsigned int) _temp54 >  2u?*(( int*) _temp54) ==  Cyc_Typerep_Union: 0){
_LL99: _temp98=(( struct Cyc_Typerep_Union_struct*) _temp54)->f1; goto _LL75;}
else{ goto _LL76;} _LL76: if(( unsigned int) _temp54 >  2u?*(( int*) _temp54) == 
Cyc_Typerep_Forall: 0){ _LL103: _temp102=(( struct Cyc_Typerep_Forall_struct*)
_temp54)->f1; goto _LL101; _LL101: _temp100=(( struct Cyc_Typerep_Forall_struct*)
_temp54)->f2; goto _LL77;} else{ goto _LL78;} _LL78: if(( unsigned int) _temp54
>  2u?*(( int*) _temp54) ==  Cyc_Typerep_App: 0){ _LL107: _temp106=( void*)((
struct Cyc_Typerep_App_struct*) _temp54)->f1; goto _LL105; _LL105: _temp104=((
struct Cyc_Typerep_App_struct*) _temp54)->f2; goto _LL79;} else{ goto _LL55;}
_LL57:({ struct Cyc_Std_String_pa_struct _temp109; _temp109.tag= Cyc_Std_String_pa;
_temp109.f1=( struct _tagged_arr)* _temp80;{ void* _temp108[ 1u]={& _temp109};
Cyc_Std_printf( _tag_arr("Var(%s)", sizeof( unsigned char), 8u), _tag_arr(
_temp108, sizeof( void*), 1u));}}); goto _LL55; _LL59:({ struct Cyc_Std_Int_pa_struct
_temp111; _temp111.tag= Cyc_Std_Int_pa; _temp111.f1=( int) _temp82;{ void*
_temp110[ 1u]={& _temp111}; Cyc_Std_printf( _tag_arr("Int(%d)", sizeof(
unsigned char), 8u), _tag_arr( _temp110, sizeof( void*), 1u));}}); goto _LL55;
_LL61:({ void* _temp112[ 0u]={}; Cyc_Std_printf( _tag_arr("Float", sizeof(
unsigned char), 6u), _tag_arr( _temp112, sizeof( void*), 0u));}); goto _LL55;
_LL63:({ void* _temp113[ 0u]={}; Cyc_Std_printf( _tag_arr("Double", sizeof(
unsigned char), 7u), _tag_arr( _temp113, sizeof( void*), 0u));}); goto _LL55;
_LL65:({ struct Cyc_Std_Int_pa_struct _temp115; _temp115.tag= Cyc_Std_Int_pa;
_temp115.f1=( int) _temp86;{ void* _temp114[ 1u]={& _temp115}; Cyc_Std_printf(
_tag_arr("ThinPtr(%d,", sizeof( unsigned char), 12u), _tag_arr( _temp114,
sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct( _temp84);({ void*
_temp116[ 0u]={}; Cyc_Std_printf( _tag_arr(")", sizeof( unsigned char), 2u),
_tag_arr( _temp116, sizeof( void*), 0u));}); goto _LL55; _LL67:({ void* _temp117[
0u]={}; Cyc_Std_printf( _tag_arr("FatPtr(", sizeof( unsigned char), 8u),
_tag_arr( _temp117, sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct(
_temp88);({ void* _temp118[ 0u]={}; Cyc_Std_printf( _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr( _temp118, sizeof( void*), 0u));}); goto _LL55;
_LL69:({ struct Cyc_Std_Int_pa_struct _temp120; _temp120.tag= Cyc_Std_Int_pa;
_temp120.f1=( int) _temp92;{ void* _temp119[ 1u]={& _temp120}; Cyc_Std_printf(
_tag_arr("Tuple(%d,[", sizeof( unsigned char), 11u), _tag_arr( _temp119, sizeof(
void*), 1u));}});{ int i= 0; for( 0; i <  _get_arr_size( _temp90, sizeof( struct
_tuple0*)); i ++){ void* _temp123; unsigned int _temp125; struct _tuple0
_temp121=*(*(( struct _tuple0**) _check_unknown_subscript( _temp90, sizeof(
struct _tuple0*), i))); _LL126: _temp125= _temp121.f1; goto _LL124; _LL124:
_temp123= _temp121.f2; goto _LL122; _LL122:({ struct Cyc_Std_Int_pa_struct
_temp128; _temp128.tag= Cyc_Std_Int_pa; _temp128.f1=( int) _temp125;{ void*
_temp127[ 1u]={& _temp128}; Cyc_Std_printf( _tag_arr("$(%d,", sizeof(
unsigned char), 6u), _tag_arr( _temp127, sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct(
_temp123);({ struct Cyc_Std_Int_pa_struct _temp130; _temp130.tag= Cyc_Std_Int_pa;
_temp130.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size( _temp90, sizeof(
struct _tuple0*)) -  1?',':' ')));{ void* _temp129[ 1u]={& _temp130}; Cyc_Std_printf(
_tag_arr(")%c", sizeof( unsigned char), 4u), _tag_arr( _temp129, sizeof( void*),
1u));}});}}({ void* _temp131[ 0u]={}; Cyc_Std_printf( _tag_arr(" ]", sizeof(
unsigned char), 3u), _tag_arr( _temp131, sizeof( void*), 0u));}); goto _LL55;
_LL71:({ void* _temp132[ 0u]={}; Cyc_Std_printf( _tag_arr("TUnion(", sizeof(
unsigned char), 8u), _tag_arr( _temp132, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp94, sizeof( struct _tuple0*)); i ++){ void*
_temp135; unsigned int _temp137; struct _tuple0 _temp133=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp94, sizeof( struct _tuple0*), i))); _LL138:
_temp137= _temp133.f1; goto _LL136; _LL136: _temp135= _temp133.f2; goto _LL134;
_LL134:({ struct Cyc_Std_Int_pa_struct _temp140; _temp140.tag= Cyc_Std_Int_pa;
_temp140.f1=( int) _temp137;{ void* _temp139[ 1u]={& _temp140}; Cyc_Std_printf(
_tag_arr("$(%d,", sizeof( unsigned char), 6u), _tag_arr( _temp139, sizeof( void*),
1u));}}); Cyc_Typerep_print_typestruct( _temp135);({ struct Cyc_Std_Int_pa_struct
_temp142; _temp142.tag= Cyc_Std_Int_pa; _temp142.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp94, sizeof( struct _tuple0*)) -  1?',':' ')));{ void*
_temp141[ 1u]={& _temp142}; Cyc_Std_printf( _tag_arr(")%c", sizeof(
unsigned char), 4u), _tag_arr( _temp141, sizeof( void*), 1u));}});}}({ void*
_temp143[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof( unsigned char), 3u),
_tag_arr( _temp143, sizeof( void*), 0u));}); goto _LL55; _LL73:({ void* _temp144[
0u]={}; Cyc_Std_printf( _tag_arr("XTUnion(", sizeof( unsigned char), 9u),
_tag_arr( _temp144, sizeof( void*), 0u));});{ int i= 0; for( 0; i < 
_get_arr_size( _temp96, sizeof( struct _tuple1*)); i ++){ void* _temp147; struct
_tagged_arr _temp149; struct _tuple1 _temp145=*(*(( struct _tuple1**)
_check_unknown_subscript( _temp96, sizeof( struct _tuple1*), i))); _LL150:
_temp149= _temp145.f1; goto _LL148; _LL148: _temp147= _temp145.f2; goto _LL146;
_LL146:({ struct Cyc_Std_String_pa_struct _temp152; _temp152.tag= Cyc_Std_String_pa;
_temp152.f1=( struct _tagged_arr) _tagged_arr_plus( _temp149, sizeof(
unsigned char), 4);{ void* _temp151[ 1u]={& _temp152}; Cyc_Std_printf( _tag_arr("$(%s,",
sizeof( unsigned char), 6u), _tag_arr( _temp151, sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct(
_temp147);({ struct Cyc_Std_Int_pa_struct _temp154; _temp154.tag= Cyc_Std_Int_pa;
_temp154.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size( _temp96, sizeof(
struct _tuple1*)) -  1?',':' ')));{ void* _temp153[ 1u]={& _temp154}; Cyc_Std_printf(
_tag_arr(")%c", sizeof( unsigned char), 4u), _tag_arr( _temp153, sizeof( void*),
1u));}});}}({ void* _temp155[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof(
unsigned char), 3u), _tag_arr( _temp155, sizeof( void*), 0u));}); goto _LL55;
_LL75:({ void* _temp156[ 0u]={}; Cyc_Std_printf( _tag_arr("Union(", sizeof(
unsigned char), 7u), _tag_arr( _temp156, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp98, sizeof( void**)); i ++){ Cyc_Typerep_print_typestruct(*(*((
void***) _check_unknown_subscript( _temp98, sizeof( void**), i))));({ struct Cyc_Std_Int_pa_struct
_temp158; _temp158.tag= Cyc_Std_Int_pa; _temp158.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp98, sizeof( void**)) -  1?',':' ')));{ void* _temp157[
1u]={& _temp158}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char), 3u),
_tag_arr( _temp157, sizeof( void*), 1u));}});}}({ void* _temp159[ 0u]={}; Cyc_Std_printf(
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr( _temp159, sizeof( void*), 0u));});
goto _LL55; _LL77:({ void* _temp160[ 0u]={}; Cyc_Std_printf( _tag_arr("Forall([",
sizeof( unsigned char), 9u), _tag_arr( _temp160, sizeof( void*), 0u));});{ int i=
0; for( 0; i <  _get_arr_size( _temp102, sizeof( struct _tagged_arr*)); i ++){({
void* _temp161[ 0u]={}; Cyc_Std_printf(*(*(( struct _tagged_arr**)
_check_unknown_subscript( _temp102, sizeof( struct _tagged_arr*), i))), _tag_arr(
_temp161, sizeof( void*), 0u));});({ struct Cyc_Std_Int_pa_struct _temp163;
_temp163.tag= Cyc_Std_Int_pa; _temp163.f1=( int)(( unsigned int)(( int)( i != 
_get_arr_size( _temp102, sizeof( struct _tagged_arr*)) -  1?',':' ')));{ void*
_temp162[ 1u]={& _temp163}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char),
3u), _tag_arr( _temp162, sizeof( void*), 1u));}});}}({ void* _temp164[ 0u]={};
Cyc_Std_printf( _tag_arr("],", sizeof( unsigned char), 3u), _tag_arr( _temp164,
sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct(* _temp100);({ void*
_temp165[ 0u]={}; Cyc_Std_printf( _tag_arr(")", sizeof( unsigned char), 2u),
_tag_arr( _temp165, sizeof( void*), 0u));}); goto _LL55; _LL79:({ void* _temp166[
0u]={}; Cyc_Std_printf( _tag_arr("App(", sizeof( unsigned char), 5u), _tag_arr(
_temp166, sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct( _temp106);({
void* _temp167[ 0u]={}; Cyc_Std_printf( _tag_arr(",[", sizeof( unsigned char), 3u),
_tag_arr( _temp167, sizeof( void*), 0u));});{ int i= 0; for( 0; i < 
_get_arr_size( _temp104, sizeof( void*)); i ++){ Cyc_Typerep_print_typestruct(*((
void**) _check_unknown_subscript( _temp104, sizeof( void*), i)));({ struct Cyc_Std_Int_pa_struct
_temp169; _temp169.tag= Cyc_Std_Int_pa; _temp169.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp104, sizeof( void*)) -  1?',':' ')));{ void* _temp168[
1u]={& _temp169}; Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char), 3u),
_tag_arr( _temp168, sizeof( void*), 1u));}});}}({ void* _temp170[ 0u]={}; Cyc_Std_printf(
_tag_arr("])", sizeof( unsigned char), 3u), _tag_arr( _temp170, sizeof( void*),
0u));}); goto _LL55; _LL55:;} void Cyc_Typerep_xmlize_typestruct( void* rep){
void* _temp171= rep; struct _tagged_arr* _temp197; unsigned int _temp199; void*
_temp201; unsigned int _temp203; void* _temp205; struct _tagged_arr _temp207;
unsigned int _temp209; struct _tagged_arr _temp211; struct _tagged_arr _temp213;
struct _tagged_arr _temp215; void** _temp217; struct _tagged_arr _temp219;
struct _tagged_arr _temp221; void* _temp223; _LL173: if(( unsigned int) _temp171
>  2u?*(( int*) _temp171) ==  Cyc_Typerep_Var: 0){ _LL198: _temp197=(( struct
Cyc_Typerep_Var_struct*) _temp171)->f1; goto _LL174;} else{ goto _LL175;} _LL175:
if(( unsigned int) _temp171 >  2u?*(( int*) _temp171) ==  Cyc_Typerep_Int: 0){
_LL200: _temp199=(( struct Cyc_Typerep_Int_struct*) _temp171)->f1; goto _LL176;}
else{ goto _LL177;} _LL177: if( _temp171 == ( void*) Cyc_Typerep_Float){ goto
_LL178;} else{ goto _LL179;} _LL179: if( _temp171 == ( void*) Cyc_Typerep_Double){
goto _LL180;} else{ goto _LL181;} _LL181: if(( unsigned int) _temp171 >  2u?*((
int*) _temp171) ==  Cyc_Typerep_ThinPtr: 0){ _LL204: _temp203=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp171)->f1; goto _LL202; _LL202: _temp201=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp171)->f2; goto _LL182;} else{ goto _LL183;} _LL183: if(( unsigned int)
_temp171 >  2u?*(( int*) _temp171) ==  Cyc_Typerep_FatPtr: 0){ _LL206: _temp205=(
void*)(( struct Cyc_Typerep_FatPtr_struct*) _temp171)->f1; goto _LL184;} else{
goto _LL185;} _LL185: if(( unsigned int) _temp171 >  2u?*(( int*) _temp171) == 
Cyc_Typerep_Tuple: 0){ _LL210: _temp209=(( struct Cyc_Typerep_Tuple_struct*)
_temp171)->f1; goto _LL208; _LL208: _temp207=(( struct Cyc_Typerep_Tuple_struct*)
_temp171)->f2; goto _LL186;} else{ goto _LL187;} _LL187: if(( unsigned int)
_temp171 >  2u?*(( int*) _temp171) ==  Cyc_Typerep_TUnion: 0){ _LL212: _temp211=((
struct Cyc_Typerep_TUnion_struct*) _temp171)->f1; goto _LL188;} else{ goto
_LL189;} _LL189: if(( unsigned int) _temp171 >  2u?*(( int*) _temp171) ==  Cyc_Typerep_XTUnion:
0){ _LL214: _temp213=(( struct Cyc_Typerep_XTUnion_struct*) _temp171)->f1; goto
_LL190;} else{ goto _LL191;} _LL191: if(( unsigned int) _temp171 >  2u?*(( int*)
_temp171) ==  Cyc_Typerep_Union: 0){ _LL216: _temp215=(( struct Cyc_Typerep_Union_struct*)
_temp171)->f1; goto _LL192;} else{ goto _LL193;} _LL193: if(( unsigned int)
_temp171 >  2u?*(( int*) _temp171) ==  Cyc_Typerep_Forall: 0){ _LL220: _temp219=((
struct Cyc_Typerep_Forall_struct*) _temp171)->f1; goto _LL218; _LL218: _temp217=((
struct Cyc_Typerep_Forall_struct*) _temp171)->f2; goto _LL194;} else{ goto
_LL195;} _LL195: if(( unsigned int) _temp171 >  2u?*(( int*) _temp171) ==  Cyc_Typerep_App:
0){ _LL224: _temp223=( void*)(( struct Cyc_Typerep_App_struct*) _temp171)->f1;
goto _LL222; _LL222: _temp221=(( struct Cyc_Typerep_App_struct*) _temp171)->f2;
goto _LL196;} else{ goto _LL172;} _LL174:({ struct Cyc_Std_String_pa_struct
_temp226; _temp226.tag= Cyc_Std_String_pa; _temp226.f1=( struct _tagged_arr)*
_temp197;{ void* _temp225[ 1u]={& _temp226}; Cyc_Std_printf( _tag_arr("<Var name=\"%s\"/>",
sizeof( unsigned char), 17u), _tag_arr( _temp225, sizeof( void*), 1u));}}); goto
_LL172; _LL176:({ struct Cyc_Std_Int_pa_struct _temp228; _temp228.tag= Cyc_Std_Int_pa;
_temp228.f1=( int) _temp199;{ void* _temp227[ 1u]={& _temp228}; Cyc_Std_printf(
_tag_arr("<Int sz=\"%d\"/>", sizeof( unsigned char), 15u), _tag_arr( _temp227,
sizeof( void*), 1u));}}); goto _LL172; _LL178:({ void* _temp229[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Float/>", sizeof( unsigned char), 9u), _tag_arr( _temp229, sizeof(
void*), 0u));}); goto _LL172; _LL180:({ void* _temp230[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Double/>", sizeof( unsigned char), 10u), _tag_arr( _temp230, sizeof(
void*), 0u));}); goto _LL172; _LL182:({ struct Cyc_Std_Int_pa_struct _temp232;
_temp232.tag= Cyc_Std_Int_pa; _temp232.f1=( int) _temp203;{ void* _temp231[ 1u]={&
_temp232}; Cyc_Std_printf( _tag_arr("<ThinPtr sz=\"%d\">", sizeof( unsigned char),
18u), _tag_arr( _temp231, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp201);({ void* _temp233[ 0u]={}; Cyc_Std_printf( _tag_arr("</ThinPtr",
sizeof( unsigned char), 10u), _tag_arr( _temp233, sizeof( void*), 0u));}); goto
_LL172; _LL184:({ void* _temp234[ 0u]={}; Cyc_Std_printf( _tag_arr("<FatPtr>",
sizeof( unsigned char), 9u), _tag_arr( _temp234, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct(
_temp205);({ void* _temp235[ 0u]={}; Cyc_Std_printf( _tag_arr("</FatPtr>",
sizeof( unsigned char), 10u), _tag_arr( _temp235, sizeof( void*), 0u));}); goto
_LL172; _LL186:({ struct Cyc_Std_Int_pa_struct _temp237; _temp237.tag= Cyc_Std_Int_pa;
_temp237.f1=( int) _temp209;{ void* _temp236[ 1u]={& _temp237}; Cyc_Std_printf(
_tag_arr("<Tuple sz=\"%d\">", sizeof( unsigned char), 16u), _tag_arr( _temp236,
sizeof( void*), 1u));}});{ int i= 0; for( 0; i <  _get_arr_size( _temp207,
sizeof( struct _tuple0*)); i ++){ void* _temp240; unsigned int _temp242; struct
_tuple0 _temp238=*(*(( struct _tuple0**) _check_unknown_subscript( _temp207,
sizeof( struct _tuple0*), i))); _LL243: _temp242= _temp238.f1; goto _LL241;
_LL241: _temp240= _temp238.f2; goto _LL239; _LL239:({ struct Cyc_Std_Int_pa_struct
_temp245; _temp245.tag= Cyc_Std_Int_pa; _temp245.f1=( int) _temp242;{ void*
_temp244[ 1u]={& _temp245}; Cyc_Std_printf( _tag_arr("<Component ofs=\"%d\">",
sizeof( unsigned char), 21u), _tag_arr( _temp244, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp240);({ void* _temp246[ 0u]={}; Cyc_Std_printf( _tag_arr("</Component>",
sizeof( unsigned char), 13u), _tag_arr( _temp246, sizeof( void*), 0u));});}}({
void* _temp247[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tuple>", sizeof(
unsigned char), 9u), _tag_arr( _temp247, sizeof( void*), 0u));}); goto _LL172;
_LL188:({ void* _temp248[ 0u]={}; Cyc_Std_printf( _tag_arr("<TUnion>", sizeof(
unsigned char), 9u), _tag_arr( _temp248, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp211, sizeof( struct _tuple0*)); i ++){ void*
_temp251; unsigned int _temp253; struct _tuple0 _temp249=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp211, sizeof( struct _tuple0*), i))); _LL254:
_temp253= _temp249.f1; goto _LL252; _LL252: _temp251= _temp249.f2; goto _LL250;
_LL250:({ struct Cyc_Std_Int_pa_struct _temp256; _temp256.tag= Cyc_Std_Int_pa;
_temp256.f1=( int) _temp253;{ void* _temp255[ 1u]={& _temp256}; Cyc_Std_printf(
_tag_arr("<Tag tag=\"%d\">", sizeof( unsigned char), 15u), _tag_arr( _temp255,
sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct( _temp251);({ void*
_temp257[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tag>", sizeof( unsigned char), 7u),
_tag_arr( _temp257, sizeof( void*), 0u));});}}({ void* _temp258[ 0u]={}; Cyc_Std_printf(
_tag_arr("</Tunion>", sizeof( unsigned char), 10u), _tag_arr( _temp258, sizeof(
void*), 0u));}); goto _LL172; _LL190:({ void* _temp259[ 0u]={}; Cyc_Std_printf(
_tag_arr("<XTUnion>", sizeof( unsigned char), 10u), _tag_arr( _temp259, sizeof(
void*), 0u));});{ int i= 0; for( 0; i <  _get_arr_size( _temp213, sizeof( struct
_tuple1*)); i ++){ void* _temp262; struct _tagged_arr _temp264; struct _tuple1
_temp260=*(*(( struct _tuple1**) _check_unknown_subscript( _temp213, sizeof(
struct _tuple1*), i))); _LL265: _temp264= _temp260.f1; goto _LL263; _LL263:
_temp262= _temp260.f2; goto _LL261; _LL261:({ struct Cyc_Std_String_pa_struct
_temp267; _temp267.tag= Cyc_Std_String_pa; _temp267.f1=( struct _tagged_arr)
_tagged_arr_plus( _temp264, sizeof( unsigned char), 4);{ void* _temp266[ 1u]={&
_temp267}; Cyc_Std_printf( _tag_arr("<Tag tag=\"%s\">", sizeof( unsigned char),
15u), _tag_arr( _temp266, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp262);({ void* _temp268[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tag>", sizeof(
unsigned char), 7u), _tag_arr( _temp268, sizeof( void*), 0u));});}}({ void*
_temp269[ 0u]={}; Cyc_Std_printf( _tag_arr("</XTunion>", sizeof( unsigned char),
11u), _tag_arr( _temp269, sizeof( void*), 0u));}); goto _LL172; _LL192:({ void*
_temp270[ 0u]={}; Cyc_Std_printf( _tag_arr("<Union>", sizeof( unsigned char), 8u),
_tag_arr( _temp270, sizeof( void*), 0u));});{ int i= 0; for( 0; i < 
_get_arr_size( _temp215, sizeof( void**)); i ++){({ void* _temp271[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Case>", sizeof( unsigned char), 7u), _tag_arr( _temp271, sizeof( void*),
0u));}); Cyc_Typerep_xmlize_typestruct(*(*(( void***) _check_unknown_subscript(
_temp215, sizeof( void**), i))));({ void* _temp272[ 0u]={}; Cyc_Std_printf(
_tag_arr("</Case>", sizeof( unsigned char), 8u), _tag_arr( _temp272, sizeof(
void*), 0u));});}}({ void* _temp273[ 0u]={}; Cyc_Std_printf( _tag_arr("</Union>",
sizeof( unsigned char), 9u), _tag_arr( _temp273, sizeof( void*), 0u));}); goto
_LL172; _LL194:({ void* _temp274[ 0u]={}; Cyc_Std_printf( _tag_arr("<Forall vars=\"",
sizeof( unsigned char), 15u), _tag_arr( _temp274, sizeof( void*), 0u));});{ int
i= 0; for( 0; i <  _get_arr_size( _temp219, sizeof( struct _tagged_arr*)); i ++){
if( i !=  0){({ void* _temp275[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp275, sizeof( void*), 0u));});}({ void*
_temp276[ 0u]={}; Cyc_Std_printf(*(*(( struct _tagged_arr**)
_check_unknown_subscript( _temp219, sizeof( struct _tagged_arr*), i))), _tag_arr(
_temp276, sizeof( void*), 0u));});}}({ void* _temp277[ 0u]={}; Cyc_Std_printf(
_tag_arr("\">", sizeof( unsigned char), 3u), _tag_arr( _temp277, sizeof( void*),
0u));}); Cyc_Typerep_xmlize_typestruct(* _temp217);({ void* _temp278[ 0u]={};
Cyc_Std_printf( _tag_arr("</Forall>", sizeof( unsigned char), 10u), _tag_arr(
_temp278, sizeof( void*), 0u));}); goto _LL172; _LL196:({ void* _temp279[ 0u]={};
Cyc_Std_printf( _tag_arr("<App>", sizeof( unsigned char), 6u), _tag_arr(
_temp279, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct( _temp223);{
int i= 0; for( 0; i <  _get_arr_size( _temp221, sizeof( void*)); i ++){ if( i != 
0){({ void* _temp280[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp280, sizeof( void*), 0u));});} Cyc_Typerep_xmlize_typestruct(*((
void**) _check_unknown_subscript( _temp221, sizeof( void*), i)));}}({ void*
_temp281[ 0u]={}; Cyc_Std_printf( _tag_arr("</App>", sizeof( unsigned char), 7u),
_tag_arr( _temp281, sizeof( void*), 0u));}); goto _LL172; _LL172:;} static void*
Cyc_Typerep_normalize_env( struct Cyc_Dict_Dict* env, void* ts){ void* _temp282=
ts; struct _tagged_arr* _temp308; void* _temp310; unsigned int _temp312; void*
_temp314; struct _tagged_arr _temp316; unsigned int _temp318; struct _tagged_arr
_temp320; struct _tagged_arr _temp322; struct _tagged_arr _temp324; void**
_temp326; struct _tagged_arr _temp328; struct _tagged_arr _temp330; void*
_temp332; _LL284: if(( unsigned int) _temp282 >  2u?*(( int*) _temp282) ==  Cyc_Typerep_Var:
0){ _LL309: _temp308=(( struct Cyc_Typerep_Var_struct*) _temp282)->f1; goto
_LL285;} else{ goto _LL286;} _LL286: if(( unsigned int) _temp282 >  2u?*(( int*)
_temp282) ==  Cyc_Typerep_Int: 0){ goto _LL287;} else{ goto _LL288;} _LL288: if(
_temp282 == ( void*) Cyc_Typerep_Float){ goto _LL289;} else{ goto _LL290;}
_LL290: if( _temp282 == ( void*) Cyc_Typerep_Double){ goto _LL291;} else{ goto
_LL292;} _LL292: if(( unsigned int) _temp282 >  2u?*(( int*) _temp282) ==  Cyc_Typerep_ThinPtr:
0){ _LL313: _temp312=(( struct Cyc_Typerep_ThinPtr_struct*) _temp282)->f1; goto
_LL311; _LL311: _temp310=( void*)(( struct Cyc_Typerep_ThinPtr_struct*) _temp282)->f2;
goto _LL293;} else{ goto _LL294;} _LL294: if(( unsigned int) _temp282 >  2u?*((
int*) _temp282) ==  Cyc_Typerep_FatPtr: 0){ _LL315: _temp314=( void*)(( struct
Cyc_Typerep_FatPtr_struct*) _temp282)->f1; goto _LL295;} else{ goto _LL296;}
_LL296: if(( unsigned int) _temp282 >  2u?*(( int*) _temp282) ==  Cyc_Typerep_Tuple:
0){ _LL319: _temp318=(( struct Cyc_Typerep_Tuple_struct*) _temp282)->f1; goto
_LL317; _LL317: _temp316=(( struct Cyc_Typerep_Tuple_struct*) _temp282)->f2;
goto _LL297;} else{ goto _LL298;} _LL298: if(( unsigned int) _temp282 >  2u?*((
int*) _temp282) ==  Cyc_Typerep_TUnion: 0){ _LL321: _temp320=(( struct Cyc_Typerep_TUnion_struct*)
_temp282)->f1; goto _LL299;} else{ goto _LL300;} _LL300: if(( unsigned int)
_temp282 >  2u?*(( int*) _temp282) ==  Cyc_Typerep_XTUnion: 0){ _LL323: _temp322=((
struct Cyc_Typerep_XTUnion_struct*) _temp282)->f1; goto _LL301;} else{ goto
_LL302;} _LL302: if(( unsigned int) _temp282 >  2u?*(( int*) _temp282) ==  Cyc_Typerep_Union:
0){ _LL325: _temp324=(( struct Cyc_Typerep_Union_struct*) _temp282)->f1; goto
_LL303;} else{ goto _LL304;} _LL304: if(( unsigned int) _temp282 >  2u?*(( int*)
_temp282) ==  Cyc_Typerep_Forall: 0){ _LL329: _temp328=(( struct Cyc_Typerep_Forall_struct*)
_temp282)->f1; goto _LL327; _LL327: _temp326=(( struct Cyc_Typerep_Forall_struct*)
_temp282)->f2; goto _LL305;} else{ goto _LL306;} _LL306: if(( unsigned int)
_temp282 >  2u?*(( int*) _temp282) ==  Cyc_Typerep_App: 0){ _LL333: _temp332=(
void*)(( struct Cyc_Typerep_App_struct*) _temp282)->f1; goto _LL331; _LL331:
_temp330=(( struct Cyc_Typerep_App_struct*) _temp282)->f2; goto _LL307;} else{
goto _LL283;} _LL285: return(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( env,( struct _tagged_arr*) _temp308); _LL287:
goto _LL289; _LL289: goto _LL291; _LL291: return ts; _LL293: return( void*)({
struct Cyc_Typerep_ThinPtr_struct* _temp334=( struct Cyc_Typerep_ThinPtr_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_ThinPtr_struct)); _temp334[ 0]=({ struct
Cyc_Typerep_ThinPtr_struct _temp335; _temp335.tag= Cyc_Typerep_ThinPtr; _temp335.f1=
_temp312; _temp335.f2=( void*) Cyc_Typerep_normalize_env( env, _temp310);
_temp335;}); _temp334;}); _LL295: return( void*)({ struct Cyc_Typerep_FatPtr_struct*
_temp336=( struct Cyc_Typerep_FatPtr_struct*) _cycalloc( sizeof( struct Cyc_Typerep_FatPtr_struct));
_temp336[ 0]=({ struct Cyc_Typerep_FatPtr_struct _temp337; _temp337.tag= Cyc_Typerep_FatPtr;
_temp337.f1=( void*) Cyc_Typerep_normalize_env( env, _temp314); _temp337;});
_temp336;}); _LL297: { struct _tagged_arr _temp338=({ unsigned int _temp354=
_get_arr_size( _temp316, sizeof( struct _tuple0*)); struct _tuple0** _temp355=(
struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*), _temp354));
struct _tagged_arr _temp357= _tag_arr( _temp355, sizeof( struct _tuple0*),
_get_arr_size( _temp316, sizeof( struct _tuple0*)));{ unsigned int _temp356=
_temp354; unsigned int i; for( i= 0; i <  _temp356; i ++){ _temp355[ i]= 0;}};
_temp357;});{ int i= 0; for( 0; i <  _get_arr_size( _temp316, sizeof( struct
_tuple0*)); i ++){ struct _tuple0 _temp341; void* _temp342; unsigned int
_temp344; struct _tuple0* _temp339=*(( struct _tuple0**)
_check_unknown_subscript( _temp316, sizeof( struct _tuple0*), i)); _temp341=*
_temp339; _LL345: _temp344= _temp341.f1; goto _LL343; _LL343: _temp342= _temp341.f2;
goto _LL340; _LL340:*(( struct _tuple0**) _check_unknown_subscript( _temp338,
sizeof( struct _tuple0*), i))=({ struct _tuple0* _temp346=( struct _tuple0*)
_cycalloc( sizeof( struct _tuple0)); _temp346->f1= _temp344; _temp346->f2= Cyc_Typerep_normalize_env(
env, _temp342); _temp346;});}}{ struct _tagged_arr _temp347=({ unsigned int
_temp350= _get_arr_size( _temp316, sizeof( struct _tuple0*)); struct _tuple0**
_temp351=( struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*),
_temp350)); struct _tagged_arr _temp353= _tag_arr( _temp351, sizeof( struct
_tuple0*), _get_arr_size( _temp316, sizeof( struct _tuple0*)));{ unsigned int
_temp352= _temp350; unsigned int i; for( i= 0; i <  _temp352; i ++){ _temp351[ i]=(
struct _tuple0*) _check_null(*(( struct _tuple0**) _check_unknown_subscript(
_temp338, sizeof( struct _tuple0*),( int) i)));}}; _temp353;}); return( void*)({
struct Cyc_Typerep_Tuple_struct* _temp348=( struct Cyc_Typerep_Tuple_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Tuple_struct)); _temp348[ 0]=({ struct Cyc_Typerep_Tuple_struct
_temp349; _temp349.tag= Cyc_Typerep_Tuple; _temp349.f1= _temp318; _temp349.f2=(
struct _tagged_arr) _temp347; _temp349;}); _temp348;});}} _LL299: { struct
_tagged_arr _temp358=({ unsigned int _temp374= _get_arr_size( _temp320, sizeof(
struct _tuple0*)); struct _tuple0** _temp375=( struct _tuple0**) _cycalloc(
_check_times( sizeof( struct _tuple0*), _temp374)); struct _tagged_arr _temp377=
_tag_arr( _temp375, sizeof( struct _tuple0*), _get_arr_size( _temp320, sizeof(
struct _tuple0*)));{ unsigned int _temp376= _temp374; unsigned int i; for( i= 0;
i <  _temp376; i ++){ _temp375[ i]= 0;}}; _temp377;});{ int i= 0; for( 0; i < 
_get_arr_size( _temp320, sizeof( struct _tuple0*)); i ++){ struct _tuple0
_temp361; void* _temp362; unsigned int _temp364; struct _tuple0* _temp359=*((
struct _tuple0**) _check_unknown_subscript( _temp320, sizeof( struct _tuple0*),
i)); _temp361=* _temp359; _LL365: _temp364= _temp361.f1; goto _LL363; _LL363:
_temp362= _temp361.f2; goto _LL360; _LL360:*(( struct _tuple0**)
_check_unknown_subscript( _temp358, sizeof( struct _tuple0*), i))=({ struct
_tuple0* _temp366=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp366->f1= _temp364; _temp366->f2= Cyc_Typerep_normalize_env( env, _temp362);
_temp366;});}}{ struct _tagged_arr _temp367=({ unsigned int _temp370=
_get_arr_size( _temp320, sizeof( struct _tuple0*)); struct _tuple0** _temp371=(
struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*), _temp370));
struct _tagged_arr _temp373= _tag_arr( _temp371, sizeof( struct _tuple0*),
_get_arr_size( _temp320, sizeof( struct _tuple0*)));{ unsigned int _temp372=
_temp370; unsigned int i; for( i= 0; i <  _temp372; i ++){ _temp371[ i]=( struct
_tuple0*) _check_null(*(( struct _tuple0**) _check_unknown_subscript( _temp358,
sizeof( struct _tuple0*),( int) i)));}}; _temp373;}); return( void*)({ struct
Cyc_Typerep_TUnion_struct* _temp368=( struct Cyc_Typerep_TUnion_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_TUnion_struct)); _temp368[ 0]=({ struct
Cyc_Typerep_TUnion_struct _temp369; _temp369.tag= Cyc_Typerep_TUnion; _temp369.f1=(
struct _tagged_arr) _temp367; _temp369;}); _temp368;});}} _LL301: { struct
_tagged_arr _temp378=({ unsigned int _temp394= _get_arr_size( _temp322, sizeof(
struct _tuple1*)); struct _tuple1** _temp395=( struct _tuple1**) _cycalloc(
_check_times( sizeof( struct _tuple1*), _temp394)); struct _tagged_arr _temp397=
_tag_arr( _temp395, sizeof( struct _tuple1*), _get_arr_size( _temp322, sizeof(
struct _tuple1*)));{ unsigned int _temp396= _temp394; unsigned int i; for( i= 0;
i <  _temp396; i ++){ _temp395[ i]= 0;}}; _temp397;});{ int i= 0; for( 0; i < 
_get_arr_size( _temp322, sizeof( struct _tuple1*)); i ++){ struct _tuple1
_temp381; void* _temp382; struct _tagged_arr _temp384; struct _tuple1* _temp379=*((
struct _tuple1**) _check_unknown_subscript( _temp322, sizeof( struct _tuple1*),
i)); _temp381=* _temp379; _LL385: _temp384= _temp381.f1; goto _LL383; _LL383:
_temp382= _temp381.f2; goto _LL380; _LL380:*(( struct _tuple1**)
_check_unknown_subscript( _temp378, sizeof( struct _tuple1*), i))=({ struct
_tuple1* _temp386=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp386->f1= _temp384; _temp386->f2= Cyc_Typerep_normalize_env( env, _temp382);
_temp386;});}}{ struct _tagged_arr _temp387=({ unsigned int _temp390=
_get_arr_size( _temp322, sizeof( struct _tuple1*)); struct _tuple1** _temp391=(
struct _tuple1**) _cycalloc( _check_times( sizeof( struct _tuple1*), _temp390));
struct _tagged_arr _temp393= _tag_arr( _temp391, sizeof( struct _tuple1*),
_get_arr_size( _temp322, sizeof( struct _tuple1*)));{ unsigned int _temp392=
_temp390; unsigned int i; for( i= 0; i <  _temp392; i ++){ _temp391[ i]=( struct
_tuple1*) _check_null(*(( struct _tuple1**) _check_unknown_subscript( _temp378,
sizeof( struct _tuple1*),( int) i)));}}; _temp393;}); return( void*)({ struct
Cyc_Typerep_XTUnion_struct* _temp388=( struct Cyc_Typerep_XTUnion_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_XTUnion_struct)); _temp388[ 0]=({ struct
Cyc_Typerep_XTUnion_struct _temp389; _temp389.tag= Cyc_Typerep_XTUnion; _temp389.f1=(
struct _tagged_arr) _temp387; _temp389;}); _temp388;});}} _LL303: { struct
_tagged_arr _temp398=({ unsigned int _temp407= _get_arr_size( _temp324, sizeof(
void**)); void*** _temp408=( void***) _cycalloc( _check_times( sizeof( void**),
_temp407)); struct _tagged_arr _temp410= _tag_arr( _temp408, sizeof( void**),
_get_arr_size( _temp324, sizeof( void**)));{ unsigned int _temp409= _temp407;
unsigned int i; for( i= 0; i <  _temp409; i ++){ _temp408[ i]= 0;}}; _temp410;});{
int i= 0; for( 0; i <  _get_arr_size( _temp324, sizeof( void**)); i ++){*(( void***)
_check_unknown_subscript( _temp398, sizeof( void**), i))=({ void** _temp399=(
void**) _cycalloc( sizeof( void*)); _temp399[ 0]= Cyc_Typerep_normalize_env( env,*(*((
void***) _check_unknown_subscript( _temp324, sizeof( void**), i)))); _temp399;});}}{
struct _tagged_arr _temp400=({ unsigned int _temp403= _get_arr_size( _temp324,
sizeof( void**)); void*** _temp404=( void***) _cycalloc( _check_times( sizeof(
void**), _temp403)); struct _tagged_arr _temp406= _tag_arr( _temp404, sizeof(
void**), _get_arr_size( _temp324, sizeof( void**)));{ unsigned int _temp405=
_temp403; unsigned int i; for( i= 0; i <  _temp405; i ++){ _temp404[ i]=( void**)
_check_null(*(( void***) _check_unknown_subscript( _temp398, sizeof( void**),(
int) i)));}}; _temp406;}); return( void*)({ struct Cyc_Typerep_Union_struct*
_temp401=( struct Cyc_Typerep_Union_struct*) _cycalloc( sizeof( struct Cyc_Typerep_Union_struct));
_temp401[ 0]=({ struct Cyc_Typerep_Union_struct _temp402; _temp402.tag= Cyc_Typerep_Union;
_temp402.f1=( struct _tagged_arr) _temp400; _temp402;}); _temp401;});}} _LL305:
return( void*)({ struct Cyc_Typerep_Forall_struct* _temp411=( struct Cyc_Typerep_Forall_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Forall_struct)); _temp411[ 0]=({ struct
Cyc_Typerep_Forall_struct _temp412; _temp412.tag= Cyc_Typerep_Forall; _temp412.f1=
_temp328; _temp412.f2=({ void** _temp413=( void**) _cycalloc( sizeof( void*));
_temp413[ 0]= Cyc_Typerep_normalize_env( env,* _temp326); _temp413;}); _temp412;});
_temp411;}); _LL307: { void* _temp414= Cyc_Typerep_normalize_env( env, _temp332);
void** _temp420; struct _tagged_arr _temp422; _LL416: if(( unsigned int)
_temp414 >  2u?*(( int*) _temp414) ==  Cyc_Typerep_Forall: 0){ _LL423: _temp422=((
struct Cyc_Typerep_Forall_struct*) _temp414)->f1; goto _LL421; _LL421: _temp420=((
struct Cyc_Typerep_Forall_struct*) _temp414)->f2; goto _LL417;} else{ goto
_LL418;} _LL418: goto _LL419; _LL417: if( _get_arr_size( _temp330, sizeof( void*))
!=  _get_arr_size( _temp422, sizeof( struct _tagged_arr*))){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp424=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp424[ 0]=({ struct Cyc_Core_Failure_struct
_temp425; _temp425.tag= Cyc_Core_Failure; _temp425.f1= _tag_arr("app appiled wrong number of arguments",
sizeof( unsigned char), 38u); _temp425;}); _temp424;}));}{ int i= 0; for( 0; i < 
_get_arr_size( _temp330, sizeof( void*)); i ++){ env=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( env,*((
struct _tagged_arr**) _check_unknown_subscript( _temp422, sizeof( struct
_tagged_arr*), i)),*(( void**) _check_unknown_subscript( _temp330, sizeof( void*),
i)));}} return Cyc_Typerep_normalize_env( env,* _temp420); _LL419:( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp426=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp426[ 0]=({ struct Cyc_Core_Failure_struct
_temp427; _temp427.tag= Cyc_Core_Failure; _temp427.f1= _tag_arr("app misapplied",
sizeof( unsigned char), 15u); _temp427;}); _temp426;})); _LL415:;} _LL283:;} int
Cyc_Typerep_name_order( struct _tagged_arr* s1, struct _tagged_arr* s2){ return
Cyc_Std_strcmp(* s1,* s2);} void* Cyc_Typerep_normalize( void* ts){ return Cyc_Typerep_normalize_env(((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Typerep_name_order), ts);}
