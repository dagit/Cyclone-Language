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
struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern int Cyc_Std_printf( struct _tagged_arr fmt, struct
_tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern int isalnum( int); extern int isalpha( int); extern int
isascii( int); extern int iscntrl( int); extern int isdigit( int); extern int
isgraph( int); extern int islower( int); extern int isprint( int); extern int
ispunct( int); extern int isspace( int); extern int isupper( int); extern int
isxdigit( int); extern int toascii( int); extern int tolower( int); extern int
toupper( int); extern int _tolower( int); extern int _toupper( int); extern void
Cyc_Marshal_print_type( void* rep, void** val); extern void* unsafe_cast( void*
x); static int Cyc_Marshal_int_cmp( unsigned int a, unsigned int b){ if( a ==  b){
return 0;} else{ if( a <  b){ return 1;} else{ return - 1;}}} static struct Cyc_Set_Set*
Cyc_Marshal_empty_addr_set(){ return(( struct Cyc_Set_Set*(*)( int(* cmp)(
unsigned int, unsigned int))) Cyc_Set_empty)( Cyc_Marshal_int_cmp);} static void*
Cyc_Marshal_get_unionbranch( unsigned int tag, struct _tagged_arr l){{ int i= 0;
for( 0; i <  _get_arr_size( l, sizeof( struct _tuple0*)); i ++){ void* _temp2;
unsigned int _temp4; struct _tuple0 _temp0=*(*(( struct _tuple0**)
_check_unknown_subscript( l, sizeof( struct _tuple0*), i))); _LL5: _temp4=
_temp0.f1; goto _LL3; _LL3: _temp2= _temp0.f2; goto _LL1; _LL1: if( _temp4 == 
tag){ return _temp2;}}}( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp6=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp6[ 0]=({ struct Cyc_Core_Failure_struct _temp7; _temp7.tag= Cyc_Core_Failure;
_temp7.f1= _tag_arr("Could not find tag in TUnion", sizeof( unsigned char), 29u);
_temp7;}); _temp6;}));} static void* Cyc_Marshal_get_xtunionbranch( struct
_tagged_arr tag, struct _tagged_arr l){{ int i= 0; for( 0; i <  _get_arr_size( l,
sizeof( struct _tuple1*)); i ++){ void* _temp10; struct _tagged_arr _temp12;
struct _tuple1 _temp8=*(*(( struct _tuple1**) _check_unknown_subscript( l,
sizeof( struct _tuple1*), i))); _LL13: _temp12= _temp8.f1; goto _LL11; _LL11:
_temp10= _temp8.f2; goto _LL9; _LL9: if( Cyc_Std_strcmp( _temp12, tag) ==  0){
return _temp10;}}}( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp14=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp14[ 0]=({ struct Cyc_Core_Failure_struct _temp15; _temp15.tag= Cyc_Core_Failure;
_temp15.f1= _tag_arr("Could not find tag in XTUnion", sizeof( unsigned char), 30u);
_temp15;}); _temp14;}));} static unsigned int Cyc_Marshal_size_type( void* rep){
void* _temp16= rep; unsigned int _temp36; unsigned int _temp38; struct
_tagged_arr _temp40; _LL18: if(( unsigned int) _temp16 >  2u?*(( int*) _temp16)
==  Cyc_Typerep_Int: 0){ _LL37: _temp36=(( struct Cyc_Typerep_Int_struct*)
_temp16)->f1; goto _LL19;} else{ goto _LL20;} _LL20: if( _temp16 == ( void*) Cyc_Typerep_Float){
goto _LL21;} else{ goto _LL22;} _LL22: if( _temp16 == ( void*) Cyc_Typerep_Double){
goto _LL23;} else{ goto _LL24;} _LL24: if(( unsigned int) _temp16 >  2u?*(( int*)
_temp16) ==  Cyc_Typerep_ThinPtr: 0){ goto _LL25;} else{ goto _LL26;} _LL26: if((
unsigned int) _temp16 >  2u?*(( int*) _temp16) ==  Cyc_Typerep_FatPtr: 0){ goto
_LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp16 >  2u?*(( int*)
_temp16) ==  Cyc_Typerep_Tuple: 0){ _LL39: _temp38=(( struct Cyc_Typerep_Tuple_struct*)
_temp16)->f1; goto _LL29;} else{ goto _LL30;} _LL30: if(( unsigned int) _temp16
>  2u?*(( int*) _temp16) ==  Cyc_Typerep_TUnion: 0){ goto _LL31;} else{ goto
_LL32;} _LL32: if(( unsigned int) _temp16 >  2u?*(( int*) _temp16) ==  Cyc_Typerep_Union:
0){ _LL41: _temp40=(( struct Cyc_Typerep_Union_struct*) _temp16)->f1; goto _LL33;}
else{ goto _LL34;} _LL34: goto _LL35; _LL19: return _temp36 >>  3; _LL21: return
sizeof( float); _LL23: return sizeof( double); _LL25: return sizeof( void*);
_LL27: return sizeof( struct _tagged_arr); _LL29: return _temp38; _LL31: return
sizeof( void*); _LL33: { int max= 0;{ int i= 0; for( 0; i <  _get_arr_size(
_temp40, sizeof( void**)); i ++){ unsigned int _temp42= Cyc_Marshal_size_type(*(*((
void***) _check_unknown_subscript( _temp40, sizeof( void**), i)))); max=( int)(
max <  _temp42? _temp42:( unsigned int) max);}} return( unsigned int) max;}
_LL35:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp43=( struct
Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp43[ 0]=({ struct Cyc_Core_Failure_struct _temp44; _temp44.tag= Cyc_Core_Failure;
_temp44.f1= _tag_arr("Unhandled case in size_type", sizeof( unsigned char), 28u);
_temp44;}); _temp43;})); _LL17:;} struct _tuple2{ int f1; struct Cyc_Set_Set* f2;
} ; struct _tuple2* Cyc_Marshal_member_insert( struct Cyc_Set_Set* set,
unsigned int val){ return({ struct _tuple2* _temp45=( struct _tuple2*) _cycalloc(
sizeof( struct _tuple2)); _temp45->f1=(( int(*)( struct Cyc_Set_Set* s,
unsigned int elt)) Cyc_Set_member)( set, val); _temp45->f2=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, unsigned int elt)) Cyc_Set_insert)( set, val); _temp45;});}
struct _tuple3{ unsigned int f1; unsigned int f2; unsigned int f3; } ; struct
Cyc_Set_Set* Cyc_Marshal_print_type_base( void* rep, struct Cyc_Set_Set* env,
void** val){{ void* _temp46= rep; unsigned int _temp68; void* _temp70;
unsigned int _temp72; void* _temp74; struct _tagged_arr _temp76; struct
_tagged_arr _temp78; struct _tagged_arr _temp80; struct _tagged_arr _temp82;
_LL48: if(( unsigned int) _temp46 >  2u?*(( int*) _temp46) ==  Cyc_Typerep_Int:
0){ _LL69: _temp68=(( struct Cyc_Typerep_Int_struct*) _temp46)->f1; goto _LL49;}
else{ goto _LL50;} _LL50: if( _temp46 == ( void*) Cyc_Typerep_Float){ goto _LL51;}
else{ goto _LL52;} _LL52: if( _temp46 == ( void*) Cyc_Typerep_Double){ goto
_LL53;} else{ goto _LL54;} _LL54: if(( unsigned int) _temp46 >  2u?*(( int*)
_temp46) ==  Cyc_Typerep_ThinPtr: 0){ _LL73: _temp72=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp46)->f1; goto _LL71; _LL71: _temp70=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp46)->f2; goto _LL55;} else{ goto _LL56;} _LL56: if(( unsigned int) _temp46
>  2u?*(( int*) _temp46) ==  Cyc_Typerep_FatPtr: 0){ _LL75: _temp74=( void*)((
struct Cyc_Typerep_FatPtr_struct*) _temp46)->f1; goto _LL57;} else{ goto _LL58;}
_LL58: if(( unsigned int) _temp46 >  2u?*(( int*) _temp46) ==  Cyc_Typerep_Tuple:
0){ _LL77: _temp76=(( struct Cyc_Typerep_Tuple_struct*) _temp46)->f2; goto _LL59;}
else{ goto _LL60;} _LL60: if(( unsigned int) _temp46 >  2u?*(( int*) _temp46) == 
Cyc_Typerep_TUnion: 0){ _LL79: _temp78=(( struct Cyc_Typerep_TUnion_struct*)
_temp46)->f1; goto _LL61;} else{ goto _LL62;} _LL62: if(( unsigned int) _temp46
>  2u?*(( int*) _temp46) ==  Cyc_Typerep_XTUnion: 0){ _LL81: _temp80=(( struct
Cyc_Typerep_XTUnion_struct*) _temp46)->f1; goto _LL63;} else{ goto _LL64;} _LL64:
if(( unsigned int) _temp46 >  2u?*(( int*) _temp46) ==  Cyc_Typerep_Union: 0){
_LL83: _temp82=(( struct Cyc_Typerep_Union_struct*) _temp46)->f1; goto _LL65;}
else{ goto _LL66;} _LL66: goto _LL67; _LL49: switch( _temp68){ case 8: _LL84: {
unsigned char* x=(( unsigned char*(*)( void** x)) unsafe_cast)( val); if(
isprint(( int)* x)){({ struct Cyc_Std_Int_pa_struct _temp87; _temp87.tag= Cyc_Std_Int_pa;
_temp87.f1=( int)(( unsigned int)(( int)* x));{ void* _temp86[ 1u]={& _temp87};
Cyc_Std_printf( _tag_arr("'%c'", sizeof( unsigned char), 5u), _tag_arr( _temp86,
sizeof( void*), 1u));}});} else{({ struct Cyc_Std_Int_pa_struct _temp89; _temp89.tag=
Cyc_Std_Int_pa; _temp89.f1=( int)(( unsigned int)(( int)* x));{ void* _temp88[ 1u]={&
_temp89}; Cyc_Std_printf( _tag_arr("'\\%d'", sizeof( unsigned char), 6u),
_tag_arr( _temp88, sizeof( void*), 1u));}});} break;} case 16: _LL85: { short* x=((
short*(*)( void** x)) unsafe_cast)( val);({ struct Cyc_Std_Int_pa_struct _temp92;
_temp92.tag= Cyc_Std_Int_pa; _temp92.f1=( int)(( unsigned int)(( int)* x));{
void* _temp91[ 1u]={& _temp92}; Cyc_Std_printf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp91, sizeof( void*), 1u));}}); break;} case
32: _LL90: { int* x=(( int*(*)( void** x)) unsafe_cast)( val);({ struct Cyc_Std_Int_pa_struct
_temp95; _temp95.tag= Cyc_Std_Int_pa; _temp95.f1=( int)(( unsigned int)* x);{
void* _temp94[ 1u]={& _temp95}; Cyc_Std_printf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp94, sizeof( void*), 1u));}}); break;}
default: _LL93:({ struct Cyc_Std_Int_pa_struct _temp98; _temp98.tag= Cyc_Std_Int_pa;
_temp98.f1=( int) _temp68;{ void* _temp97[ 1u]={& _temp98}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("print_type_base: illegal int size %d bits\n", sizeof( unsigned char),
43u), _tag_arr( _temp97, sizeof( void*), 1u));}}); exit( 1);} goto _LL47; _LL51: {
float* x=(( float*(*)( void** x)) unsafe_cast)( val);({ struct Cyc_Std_Double_pa_struct
_temp100; _temp100.tag= Cyc_Std_Double_pa; _temp100.f1=( double)* x;{ void*
_temp99[ 1u]={& _temp100}; Cyc_Std_printf( _tag_arr("%f", sizeof( unsigned char),
3u), _tag_arr( _temp99, sizeof( void*), 1u));}}); goto _LL47;} _LL53: { double*
x=(( double*(*)( void** x)) unsafe_cast)( val);({ struct Cyc_Std_Double_pa_struct
_temp102; _temp102.tag= Cyc_Std_Double_pa; _temp102.f1=* x;{ void* _temp101[ 1u]={&
_temp102}; Cyc_Std_printf( _tag_arr("%f", sizeof( unsigned char), 3u), _tag_arr(
_temp101, sizeof( void*), 1u));}}); goto _LL47;} _LL55: { int* x=(( int*(*)(
void** x)) unsafe_cast)( val); int y=*(( int*) _check_null( x)); struct _tuple2
_temp105; struct Cyc_Set_Set* _temp106; int _temp108; struct _tuple2* _temp103=
Cyc_Marshal_member_insert( env,( unsigned int) y); _temp105=* _temp103; _LL109:
_temp108= _temp105.f1; goto _LL107; _LL107: _temp106= _temp105.f2; goto _LL104;
_LL104: env= _temp106; if( ! _temp108){({ struct Cyc_Std_Int_pa_struct _temp111;
_temp111.tag= Cyc_Std_Int_pa; _temp111.f1=( unsigned int) y;{ void* _temp110[ 1u]={&
_temp111}; Cyc_Std_printf( _tag_arr("{(@%x) ", sizeof( unsigned char), 8u),
_tag_arr( _temp110, sizeof( void*), 1u));}}); if( y !=  0){ int typ_szb=( int)
Cyc_Marshal_size_type( _temp70); int i= 0; for( 0; i <  _temp72; i ++){ env= Cyc_Marshal_print_type_base(
_temp70, env,( void**)(( void**(*)( int x)) unsafe_cast)( y)); if( i !=  _temp72
-  1){({ void* _temp112[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp112, sizeof( void*), 0u));});} y += typ_szb;}}
else{({ void* _temp113[ 0u]={}; Cyc_Std_printf( _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp113, sizeof( void*), 0u));});}({ void*
_temp114[ 0u]={}; Cyc_Std_printf( _tag_arr(" }", sizeof( unsigned char), 3u),
_tag_arr( _temp114, sizeof( void*), 0u));});} else{({ struct Cyc_Std_Int_pa_struct
_temp116; _temp116.tag= Cyc_Std_Int_pa; _temp116.f1=( unsigned int) y;{ void*
_temp115[ 1u]={& _temp116}; Cyc_Std_printf( _tag_arr("(#%x)", sizeof(
unsigned char), 6u), _tag_arr( _temp115, sizeof( void*), 1u));}});} goto _LL47;}
_LL57: { struct _tuple3* x=(( struct _tuple3*(*)( void** x)) unsafe_cast)( val);
unsigned int typ_szb= Cyc_Marshal_size_type( _temp74); unsigned int base=(* x).f1;
unsigned int curr=(* x).f2; unsigned int last_plus_one=(* x).f3; struct _tuple2
_temp119; struct Cyc_Set_Set* _temp120; int _temp122; struct _tuple2* _temp117=
Cyc_Marshal_member_insert( env, base); _temp119=* _temp117; _LL123: _temp122=
_temp119.f1; goto _LL121; _LL121: _temp120= _temp119.f2; goto _LL118; _LL118:
env= _temp120; if( ! _temp122){({ struct Cyc_Std_Int_pa_struct _temp125;
_temp125.tag= Cyc_Std_Int_pa; _temp125.f1= base;{ void* _temp124[ 1u]={&
_temp125}; Cyc_Std_printf( _tag_arr("[(@%x) ", sizeof( unsigned char), 8u),
_tag_arr( _temp124, sizeof( void*), 1u));}});{ unsigned int i= base; for( 0; i < 
last_plus_one; i += typ_szb){ env= Cyc_Marshal_print_type_base( _temp74, env,(
void**)(( void**(*)( unsigned int x)) unsafe_cast)( i)); if( i +  typ_szb < 
last_plus_one){({ void* _temp126[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp126, sizeof( void*), 0u));});}}}({ void*
_temp127[ 0u]={}; Cyc_Std_printf( _tag_arr(" ]", sizeof( unsigned char), 3u),
_tag_arr( _temp127, sizeof( void*), 0u));});} else{({ struct Cyc_Std_Int_pa_struct
_temp129; _temp129.tag= Cyc_Std_Int_pa; _temp129.f1= base;{ void* _temp128[ 1u]={&
_temp129}; Cyc_Std_printf( _tag_arr("(#%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp128, sizeof( void*), 1u));}});} goto _LL47;} _LL59:({ void*
_temp130[ 0u]={}; Cyc_Std_printf( _tag_arr("$( ", sizeof( unsigned char), 4u),
_tag_arr( _temp130, sizeof( void*), 0u));});{ int x=(( int(*)( void** x))
unsafe_cast)( val);{ int i= 0; for( 0; i <  _get_arr_size( _temp76, sizeof(
struct _tuple0*)); i ++){ void* _temp133; unsigned int _temp135; struct _tuple0
_temp131=*(*(( struct _tuple0**) _check_unknown_subscript( _temp76, sizeof(
struct _tuple0*), i))); _LL136: _temp135= _temp131.f1; goto _LL134; _LL134:
_temp133= _temp131.f2; goto _LL132; _LL132: { int x_ofs=( int)( x +  _temp135);
env= Cyc_Marshal_print_type_base( _temp133, env,( void**)(( void**(*)( int x))
unsafe_cast)( x_ofs)); if( i !=  _get_arr_size( _temp76, sizeof( struct _tuple0*))
-  1){({ void* _temp137[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp137, sizeof( void*), 0u));});}}}}({ void*
_temp138[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof( unsigned char), 3u),
_tag_arr( _temp138, sizeof( void*), 0u));}); goto _LL47;} _LL61:({ void*
_temp139[ 0u]={}; Cyc_Std_printf( _tag_arr("&[( ", sizeof( unsigned char), 5u),
_tag_arr( _temp139, sizeof( void*), 0u));});{ unsigned int* tagp=(( unsigned int*(*)(
void** x)) unsafe_cast)( val); if(* tagp <  1024){({ struct Cyc_Std_Int_pa_struct
_temp141; _temp141.tag= Cyc_Std_Int_pa; _temp141.f1=( int)* tagp;{ void*
_temp140[ 1u]={& _temp141}; Cyc_Std_printf( _tag_arr("%d", sizeof( unsigned char),
3u), _tag_arr( _temp140, sizeof( void*), 1u));}});} else{ struct _tuple2
_temp144; struct Cyc_Set_Set* _temp145; int _temp147; struct _tuple2* _temp142=
Cyc_Marshal_member_insert( env,* tagp); _temp144=* _temp142; _LL148: _temp147=
_temp144.f1; goto _LL146; _LL146: _temp145= _temp144.f2; goto _LL143; _LL143:
env= _temp145; if( ! _temp147){({ struct Cyc_Std_Int_pa_struct _temp150;
_temp150.tag= Cyc_Std_Int_pa; _temp150.f1=* tagp;{ void* _temp149[ 1u]={&
_temp150}; Cyc_Std_printf( _tag_arr("(@%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp149, sizeof( void*), 1u));}});{ unsigned int* x=(( unsigned int*(*)(
unsigned int x)) unsafe_cast)(* tagp); unsigned int tag=* x; void* typ= Cyc_Marshal_get_unionbranch(
tag, _temp78); env=(( struct Cyc_Set_Set*(*)( void* rep, struct Cyc_Set_Set* env,
unsigned int* val)) Cyc_Marshal_print_type_base)( typ, env, x);}} else{({ struct
Cyc_Std_Int_pa_struct _temp152; _temp152.tag= Cyc_Std_Int_pa; _temp152.f1=* tagp;{
void* _temp151[ 1u]={& _temp152}; Cyc_Std_printf( _tag_arr("(#%x)", sizeof(
unsigned char), 6u), _tag_arr( _temp151, sizeof( void*), 1u));}});}}({ void*
_temp153[ 0u]={}; Cyc_Std_printf( _tag_arr(" )]", sizeof( unsigned char), 4u),
_tag_arr( _temp153, sizeof( void*), 0u));}); goto _LL47;} _LL63: { unsigned int*
xtunionp=(( unsigned int*(*)( void** x)) unsafe_cast)( val); struct _tuple2
_temp156; struct Cyc_Set_Set* _temp157; int _temp159; struct _tuple2* _temp154=
Cyc_Marshal_member_insert( env,* xtunionp); _temp156=* _temp154; _LL160:
_temp159= _temp156.f1; goto _LL158; _LL158: _temp157= _temp156.f2; goto _LL155;
_LL155: env= _temp157; if( ! _temp159){({ struct Cyc_Std_Int_pa_struct _temp162;
_temp162.tag= Cyc_Std_Int_pa; _temp162.f1=* xtunionp;{ void* _temp161[ 1u]={&
_temp162}; Cyc_Std_printf( _tag_arr("(@%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp161, sizeof( void*), 1u));}});{ unsigned int* xtstructp=((
unsigned int*(*)( unsigned int x)) unsafe_cast)(* xtunionp); if(* xtstructp != 
0){ unsigned char* xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(*
xtstructp +  4); struct _tagged_arr xtname=( struct _tagged_arr)
Cstring_to_string( xtnamec);({ struct Cyc_Std_String_pa_struct _temp164;
_temp164.tag= Cyc_Std_String_pa; _temp164.f1=( struct _tagged_arr) xtname;{ void*
_temp163[ 1u]={& _temp164}; Cyc_Std_printf( _tag_arr("&%s[( ", sizeof(
unsigned char), 7u), _tag_arr( _temp163, sizeof( void*), 1u));}});{ void* typ=
Cyc_Marshal_get_xtunionbranch( xtname, _temp80); env=(( struct Cyc_Set_Set*(*)(
void* rep, struct Cyc_Set_Set* env, unsigned int* val)) Cyc_Marshal_print_type_base)(
typ, env,( unsigned int*) _check_null( xtstructp));}} else{ unsigned char*
xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(* xtunionp +  4);
struct _tagged_arr xtname=( struct _tagged_arr) Cstring_to_string( xtnamec);({
struct Cyc_Std_String_pa_struct _temp166; _temp166.tag= Cyc_Std_String_pa;
_temp166.f1=( struct _tagged_arr) xtname;{ void* _temp165[ 1u]={& _temp166}; Cyc_Std_printf(
_tag_arr("&%s[( ", sizeof( unsigned char), 7u), _tag_arr( _temp165, sizeof( void*),
1u));}});}}} else{({ struct Cyc_Std_Int_pa_struct _temp168; _temp168.tag= Cyc_Std_Int_pa;
_temp168.f1=* xtunionp;{ void* _temp167[ 1u]={& _temp168}; Cyc_Std_printf(
_tag_arr("(#%x)", sizeof( unsigned char), 6u), _tag_arr( _temp167, sizeof( void*),
1u));}});}({ void* _temp169[ 0u]={}; Cyc_Std_printf( _tag_arr(" )]", sizeof(
unsigned char), 4u), _tag_arr( _temp169, sizeof( void*), 0u));}); goto _LL47;}
_LL65:({ void* _temp170[ 0u]={}; Cyc_Std_printf( _tag_arr("[[", sizeof(
unsigned char), 3u), _tag_arr( _temp170, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp82, sizeof( void**)); i ++){ Cyc_Marshal_print_type_base(*(*((
void***) _check_unknown_subscript( _temp82, sizeof( void**), i))), env, val);
if( i <  _get_arr_size( _temp82, sizeof( void**)) -  1){({ void* _temp171[ 0u]={};
Cyc_Std_printf( _tag_arr(" | ", sizeof( unsigned char), 4u), _tag_arr( _temp171,
sizeof( void*), 0u));});}}}({ void* _temp172[ 0u]={}; Cyc_Std_printf( _tag_arr("]]",
sizeof( unsigned char), 3u), _tag_arr( _temp172, sizeof( void*), 0u));}); goto
_LL47; _LL67:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp173=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp173[ 0]=({ struct Cyc_Core_Failure_struct _temp174; _temp174.tag= Cyc_Core_Failure;
_temp174.f1= _tag_arr("Unhandled case in print_type_base", sizeof( unsigned char),
34u); _temp174;}); _temp173;})); _LL47:;} return env;} void Cyc_Marshal_print_type(
void* ts, void** val){ struct _handler_cons _temp175; _push_handler(& _temp175);{
int _temp177= 0; if( setjmp( _temp175.handler)){ _temp177= 1;} if( ! _temp177){
Cyc_Marshal_print_type_base( ts, Cyc_Marshal_empty_addr_set(), val);;
_pop_handler();} else{ void* _temp176=( void*) _exn_thrown; void* _temp179=
_temp176; struct _tagged_arr _temp185; _LL181: if(*(( void**) _temp179) ==  Cyc_Core_Failure){
_LL186: _temp185=(( struct Cyc_Core_Failure_struct*) _temp179)->f1; goto _LL182;}
else{ goto _LL183;} _LL183: goto _LL184; _LL182:({ struct Cyc_Std_String_pa_struct
_temp188; _temp188.tag= Cyc_Std_String_pa; _temp188.f1=( struct _tagged_arr)
_temp185;{ void* _temp187[ 1u]={& _temp188}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n", sizeof( unsigned char), 19u), _tag_arr( _temp187,
sizeof( void*), 1u));}});( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp189=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp189[ 0]=({ struct Cyc_Core_Failure_struct _temp190; _temp190.tag= Cyc_Core_Failure;
_temp190.f1= _temp185; _temp190;}); _temp189;})); _LL184:( void) _throw(
_temp179); _LL180:;}}}
