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
18u]; extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Set_Set; extern
unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Typerep_Var= 0; struct
Cyc_Typerep_Var_struct{ int tag; struct _tagged_arr* f1; } ; static const int
Cyc_Typerep_Int= 1; struct Cyc_Typerep_Int_struct{ int tag; unsigned int f1; } ;
static const int Cyc_Typerep_Float= 0; static const int Cyc_Typerep_Double= 1;
static const int Cyc_Typerep_ThinPtr= 2; struct Cyc_Typerep_ThinPtr_struct{ int
tag; unsigned int f1; void* f2; } ; static const int Cyc_Typerep_FatPtr= 3;
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
extern struct Cyc_Set_Set* Cyc_Typerep_empty_addr_set(); struct _tuple2{ int f1;
struct Cyc_Set_Set* f2; } ; extern struct _tuple2* Cyc_Typerep_member_insert(
struct Cyc_Set_Set*, unsigned int); extern void* Cyc_Typerep_get_unionbranch(
unsigned int tag, struct _tagged_arr l); extern void* Cyc_Typerep_get_xtunionbranch(
struct _tagged_arr tag, struct _tagged_arr l); unsigned int Cyc_Typerep_size_type(
void* rep); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE*
Cyc_Std_stderr; extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern
unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_printf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int isalnum( int); extern int isalpha( int);
extern int isascii( int); extern int iscntrl( int); extern int isdigit( int);
extern int isgraph( int); extern int islower( int); extern int isprint( int);
extern int ispunct( int); extern int isspace( int); extern int isupper( int);
extern int isxdigit( int); extern int toascii( int); extern int tolower( int);
extern int toupper( int); extern int _tolower( int); extern int _toupper( int);
extern void Cyc_Marshal_print_type( void* rep, void** val); extern void*
unsafe_cast( void* x); struct _tuple3{ unsigned int f1; unsigned int f2;
unsigned int f3; } ; struct Cyc_Set_Set* Cyc_Marshal_print_type_base( void* rep,
struct Cyc_Set_Set* env, void** val){{ void* _temp0= rep; unsigned int _temp22;
void* _temp24; unsigned int _temp26; void* _temp28; struct _tagged_arr _temp30;
struct _tagged_arr _temp32; struct _tagged_arr _temp34; struct _tagged_arr
_temp36; _LL2: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0) ==  Cyc_Typerep_Int:
0){ _LL23: _temp22=(( struct Cyc_Typerep_Int_struct*) _temp0)->f1; goto _LL3;}
else{ goto _LL4;} _LL4: if( _temp0 == ( void*) Cyc_Typerep_Float){ goto _LL5;}
else{ goto _LL6;} _LL6: if( _temp0 == ( void*) Cyc_Typerep_Double){ goto _LL7;}
else{ goto _LL8;} _LL8: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0) == 
Cyc_Typerep_ThinPtr: 0){ _LL27: _temp26=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp0)->f1; goto _LL25; _LL25: _temp24=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp0)->f2; goto _LL9;} else{ goto _LL10;} _LL10: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_FatPtr: 0){ _LL29: _temp28=( void*)((
struct Cyc_Typerep_FatPtr_struct*) _temp0)->f1; goto _LL11;} else{ goto _LL12;}
_LL12: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0) ==  Cyc_Typerep_Tuple:
0){ _LL31: _temp30=(( struct Cyc_Typerep_Tuple_struct*) _temp0)->f2; goto _LL13;}
else{ goto _LL14;} _LL14: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0) == 
Cyc_Typerep_TUnion: 0){ _LL33: _temp32=(( struct Cyc_Typerep_TUnion_struct*)
_temp0)->f1; goto _LL15;} else{ goto _LL16;} _LL16: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_XTUnion: 0){ _LL35: _temp34=(( struct Cyc_Typerep_XTUnion_struct*)
_temp0)->f1; goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp0 > 
2u?*(( int*) _temp0) ==  Cyc_Typerep_Union: 0){ _LL37: _temp36=(( struct Cyc_Typerep_Union_struct*)
_temp0)->f1; goto _LL19;} else{ goto _LL20;} _LL20: goto _LL21; _LL3: switch(
_temp22){ case 8: _LL38: { unsigned char* x=(( unsigned char*(*)( void** x))
unsafe_cast)( val); if( isprint(( int)* x)){({ struct Cyc_Std_Int_pa_struct
_temp41; _temp41.tag= Cyc_Std_Int_pa; _temp41.f1=( int)(( unsigned int)(( int)*
x));{ void* _temp40[ 1u]={& _temp41}; Cyc_Std_printf( _tag_arr("'%c'", sizeof(
unsigned char), 5u), _tag_arr( _temp40, sizeof( void*), 1u));}});} else{({
struct Cyc_Std_Int_pa_struct _temp43; _temp43.tag= Cyc_Std_Int_pa; _temp43.f1=(
int)(( unsigned int)(( int)* x));{ void* _temp42[ 1u]={& _temp43}; Cyc_Std_printf(
_tag_arr("'\\%d'", sizeof( unsigned char), 6u), _tag_arr( _temp42, sizeof( void*),
1u));}});} break;} case 16: _LL39: { short* x=(( short*(*)( void** x))
unsafe_cast)( val);({ struct Cyc_Std_Int_pa_struct _temp46; _temp46.tag= Cyc_Std_Int_pa;
_temp46.f1=( int)(( unsigned int)(( int)* x));{ void* _temp45[ 1u]={& _temp46};
Cyc_Std_printf( _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp45,
sizeof( void*), 1u));}}); break;} case 32: _LL44: { int* x=(( int*(*)( void** x))
unsafe_cast)( val);({ struct Cyc_Std_Int_pa_struct _temp49; _temp49.tag= Cyc_Std_Int_pa;
_temp49.f1=( int)(( unsigned int)* x);{ void* _temp48[ 1u]={& _temp49}; Cyc_Std_printf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp48, sizeof( void*), 1u));}});
break;} default: _LL47:({ struct Cyc_Std_Int_pa_struct _temp52; _temp52.tag= Cyc_Std_Int_pa;
_temp52.f1=( int) _temp22;{ void* _temp51[ 1u]={& _temp52}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("print_type_base: illegal int size %d bits\n", sizeof( unsigned char),
43u), _tag_arr( _temp51, sizeof( void*), 1u));}}); exit( 1);} goto _LL1; _LL5: {
float* x=(( float*(*)( void** x)) unsafe_cast)( val);({ struct Cyc_Std_Double_pa_struct
_temp54; _temp54.tag= Cyc_Std_Double_pa; _temp54.f1=( double)* x;{ void* _temp53[
1u]={& _temp54}; Cyc_Std_printf( _tag_arr("%f", sizeof( unsigned char), 3u),
_tag_arr( _temp53, sizeof( void*), 1u));}}); goto _LL1;} _LL7: { double* x=((
double*(*)( void** x)) unsafe_cast)( val);({ struct Cyc_Std_Double_pa_struct
_temp56; _temp56.tag= Cyc_Std_Double_pa; _temp56.f1=* x;{ void* _temp55[ 1u]={&
_temp56}; Cyc_Std_printf( _tag_arr("%f", sizeof( unsigned char), 3u), _tag_arr(
_temp55, sizeof( void*), 1u));}}); goto _LL1;} _LL9: { int* x=(( int*(*)( void**
x)) unsafe_cast)( val); int y=*(( int*) _check_null( x)); struct _tuple2 _temp59;
struct Cyc_Set_Set* _temp60; int _temp62; struct _tuple2* _temp57= Cyc_Typerep_member_insert(
env,( unsigned int) y); _temp59=* _temp57; _LL63: _temp62= _temp59.f1; goto
_LL61; _LL61: _temp60= _temp59.f2; goto _LL58; _LL58: env= _temp60; if( !
_temp62){({ struct Cyc_Std_Int_pa_struct _temp65; _temp65.tag= Cyc_Std_Int_pa;
_temp65.f1=( unsigned int) y;{ void* _temp64[ 1u]={& _temp65}; Cyc_Std_printf(
_tag_arr("{(@%x) ", sizeof( unsigned char), 8u), _tag_arr( _temp64, sizeof( void*),
1u));}}); if( y !=  0){ int typ_szb=( int) Cyc_Typerep_size_type( _temp24); int
i= 0; for( 0; i <  _temp26; i ++){ env= Cyc_Marshal_print_type_base( _temp24,
env,( void**)(( void**(*)( int x)) unsafe_cast)( y)); if( i !=  _temp26 -  1){({
void* _temp66[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof( unsigned char), 3u),
_tag_arr( _temp66, sizeof( void*), 0u));});} y += typ_szb;}} else{({ void*
_temp67[ 0u]={}; Cyc_Std_printf( _tag_arr("NULL", sizeof( unsigned char), 5u),
_tag_arr( _temp67, sizeof( void*), 0u));});}({ void* _temp68[ 0u]={}; Cyc_Std_printf(
_tag_arr(" }", sizeof( unsigned char), 3u), _tag_arr( _temp68, sizeof( void*), 0u));});}
else{({ struct Cyc_Std_Int_pa_struct _temp70; _temp70.tag= Cyc_Std_Int_pa;
_temp70.f1=( unsigned int) y;{ void* _temp69[ 1u]={& _temp70}; Cyc_Std_printf(
_tag_arr("(#%x)", sizeof( unsigned char), 6u), _tag_arr( _temp69, sizeof( void*),
1u));}});} goto _LL1;} _LL11: { struct _tuple3* x=(( struct _tuple3*(*)( void**
x)) unsafe_cast)( val); unsigned int typ_szb= Cyc_Typerep_size_type( _temp28);
unsigned int base=(* x).f1; unsigned int curr=(* x).f2; unsigned int
last_plus_one=(* x).f3; struct _tuple2 _temp73; struct Cyc_Set_Set* _temp74; int
_temp76; struct _tuple2* _temp71= Cyc_Typerep_member_insert( env, base); _temp73=*
_temp71; _LL77: _temp76= _temp73.f1; goto _LL75; _LL75: _temp74= _temp73.f2;
goto _LL72; _LL72: env= _temp74; if( ! _temp76){({ struct Cyc_Std_Int_pa_struct
_temp79; _temp79.tag= Cyc_Std_Int_pa; _temp79.f1= base;{ void* _temp78[ 1u]={&
_temp79}; Cyc_Std_printf( _tag_arr("[(@%x) ", sizeof( unsigned char), 8u),
_tag_arr( _temp78, sizeof( void*), 1u));}});{ unsigned int i= base; for( 0; i < 
last_plus_one; i += typ_szb){ env= Cyc_Marshal_print_type_base( _temp28, env,(
void**)(( void**(*)( unsigned int x)) unsafe_cast)( i)); if( i +  typ_szb < 
last_plus_one){({ void* _temp80[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp80, sizeof( void*), 0u));});}}}({ void*
_temp81[ 0u]={}; Cyc_Std_printf( _tag_arr(" ]", sizeof( unsigned char), 3u),
_tag_arr( _temp81, sizeof( void*), 0u));});} else{({ struct Cyc_Std_Int_pa_struct
_temp83; _temp83.tag= Cyc_Std_Int_pa; _temp83.f1= base;{ void* _temp82[ 1u]={&
_temp83}; Cyc_Std_printf( _tag_arr("(#%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp82, sizeof( void*), 1u));}});} goto _LL1;} _LL13:({ void* _temp84[
0u]={}; Cyc_Std_printf( _tag_arr("$( ", sizeof( unsigned char), 4u), _tag_arr(
_temp84, sizeof( void*), 0u));});{ int x=(( int(*)( void** x)) unsafe_cast)( val);{
int i= 0; for( 0; i <  _get_arr_size( _temp30, sizeof( struct _tuple0*)); i ++){
void* _temp87; unsigned int _temp89; struct _tuple0 _temp85=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp30, sizeof( struct _tuple0*), i))); _LL90:
_temp89= _temp85.f1; goto _LL88; _LL88: _temp87= _temp85.f2; goto _LL86; _LL86: {
int x_ofs=( int)( x +  _temp89); env= Cyc_Marshal_print_type_base( _temp87, env,(
void**)(( void**(*)( int x)) unsafe_cast)( x_ofs)); if( i !=  _get_arr_size(
_temp30, sizeof( struct _tuple0*)) -  1){({ void* _temp91[ 0u]={}; Cyc_Std_printf(
_tag_arr(", ", sizeof( unsigned char), 3u), _tag_arr( _temp91, sizeof( void*), 0u));});}}}}({
void* _temp92[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof( unsigned char), 3u),
_tag_arr( _temp92, sizeof( void*), 0u));}); goto _LL1;} _LL15:({ void* _temp93[
0u]={}; Cyc_Std_printf( _tag_arr("&[( ", sizeof( unsigned char), 5u), _tag_arr(
_temp93, sizeof( void*), 0u));});{ unsigned int* tagp=(( unsigned int*(*)( void**
x)) unsafe_cast)( val); if(* tagp <  1024){({ struct Cyc_Std_Int_pa_struct
_temp95; _temp95.tag= Cyc_Std_Int_pa; _temp95.f1=( int)* tagp;{ void* _temp94[ 1u]={&
_temp95}; Cyc_Std_printf( _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr(
_temp94, sizeof( void*), 1u));}});} else{ struct _tuple2 _temp98; struct Cyc_Set_Set*
_temp99; int _temp101; struct _tuple2* _temp96= Cyc_Typerep_member_insert( env,*
tagp); _temp98=* _temp96; _LL102: _temp101= _temp98.f1; goto _LL100; _LL100:
_temp99= _temp98.f2; goto _LL97; _LL97: env= _temp99; if( ! _temp101){({ struct
Cyc_Std_Int_pa_struct _temp104; _temp104.tag= Cyc_Std_Int_pa; _temp104.f1=* tagp;{
void* _temp103[ 1u]={& _temp104}; Cyc_Std_printf( _tag_arr("(@%x)", sizeof(
unsigned char), 6u), _tag_arr( _temp103, sizeof( void*), 1u));}});{ unsigned int*
x=(( unsigned int*(*)( unsigned int x)) unsafe_cast)(* tagp); unsigned int tag=*
x; void* typ= Cyc_Typerep_get_unionbranch( tag, _temp32); env=(( struct Cyc_Set_Set*(*)(
void* rep, struct Cyc_Set_Set* env, unsigned int* val)) Cyc_Marshal_print_type_base)(
typ, env, x);}} else{({ struct Cyc_Std_Int_pa_struct _temp106; _temp106.tag= Cyc_Std_Int_pa;
_temp106.f1=* tagp;{ void* _temp105[ 1u]={& _temp106}; Cyc_Std_printf( _tag_arr("(#%x)",
sizeof( unsigned char), 6u), _tag_arr( _temp105, sizeof( void*), 1u));}});}}({
void* _temp107[ 0u]={}; Cyc_Std_printf( _tag_arr(" )]", sizeof( unsigned char),
4u), _tag_arr( _temp107, sizeof( void*), 0u));}); goto _LL1;} _LL17: {
unsigned int* xtunionp=(( unsigned int*(*)( void** x)) unsafe_cast)( val);
struct _tuple2 _temp110; struct Cyc_Set_Set* _temp111; int _temp113; struct
_tuple2* _temp108= Cyc_Typerep_member_insert( env,* xtunionp); _temp110=*
_temp108; _LL114: _temp113= _temp110.f1; goto _LL112; _LL112: _temp111= _temp110.f2;
goto _LL109; _LL109: env= _temp111; if( ! _temp113){({ struct Cyc_Std_Int_pa_struct
_temp116; _temp116.tag= Cyc_Std_Int_pa; _temp116.f1=* xtunionp;{ void* _temp115[
1u]={& _temp116}; Cyc_Std_printf( _tag_arr("(@%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp115, sizeof( void*), 1u));}});{ unsigned int* xtstructp=((
unsigned int*(*)( unsigned int x)) unsafe_cast)(* xtunionp); if(* xtstructp != 
0){ unsigned char* xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(*
xtstructp +  4); struct _tagged_arr xtname=( struct _tagged_arr)
Cstring_to_string( xtnamec);({ struct Cyc_Std_String_pa_struct _temp118;
_temp118.tag= Cyc_Std_String_pa; _temp118.f1=( struct _tagged_arr) xtname;{ void*
_temp117[ 1u]={& _temp118}; Cyc_Std_printf( _tag_arr("&%s[( ", sizeof(
unsigned char), 7u), _tag_arr( _temp117, sizeof( void*), 1u));}});{ void* typ=
Cyc_Typerep_get_xtunionbranch( xtname, _temp34); env=(( struct Cyc_Set_Set*(*)(
void* rep, struct Cyc_Set_Set* env, unsigned int* val)) Cyc_Marshal_print_type_base)(
typ, env,( unsigned int*) _check_null( xtstructp));}} else{ unsigned char*
xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(* xtunionp +  4);
struct _tagged_arr xtname=( struct _tagged_arr) Cstring_to_string( xtnamec);({
struct Cyc_Std_String_pa_struct _temp120; _temp120.tag= Cyc_Std_String_pa;
_temp120.f1=( struct _tagged_arr) xtname;{ void* _temp119[ 1u]={& _temp120}; Cyc_Std_printf(
_tag_arr("&%s[( ", sizeof( unsigned char), 7u), _tag_arr( _temp119, sizeof( void*),
1u));}});}}} else{({ struct Cyc_Std_Int_pa_struct _temp122; _temp122.tag= Cyc_Std_Int_pa;
_temp122.f1=* xtunionp;{ void* _temp121[ 1u]={& _temp122}; Cyc_Std_printf(
_tag_arr("(#%x)", sizeof( unsigned char), 6u), _tag_arr( _temp121, sizeof( void*),
1u));}});}({ void* _temp123[ 0u]={}; Cyc_Std_printf( _tag_arr(" )]", sizeof(
unsigned char), 4u), _tag_arr( _temp123, sizeof( void*), 0u));}); goto _LL1;}
_LL19:({ void* _temp124[ 0u]={}; Cyc_Std_printf( _tag_arr("[[", sizeof(
unsigned char), 3u), _tag_arr( _temp124, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp36, sizeof( void**)); i ++){ Cyc_Marshal_print_type_base(*(*((
void***) _check_unknown_subscript( _temp36, sizeof( void**), i))), env, val);
if( i <  _get_arr_size( _temp36, sizeof( void**)) -  1){({ void* _temp125[ 0u]={};
Cyc_Std_printf( _tag_arr(" | ", sizeof( unsigned char), 4u), _tag_arr( _temp125,
sizeof( void*), 0u));});}}}({ void* _temp126[ 0u]={}; Cyc_Std_printf( _tag_arr("]]",
sizeof( unsigned char), 3u), _tag_arr( _temp126, sizeof( void*), 0u));}); goto
_LL1; _LL21:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp127=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp127[ 0]=({ struct Cyc_Core_Failure_struct _temp128; _temp128.tag= Cyc_Core_Failure;
_temp128.f1= _tag_arr("Unhandled case in print_type_base", sizeof( unsigned char),
34u); _temp128;}); _temp127;})); _LL1:;} return env;} void Cyc_Marshal_print_type(
void* ts, void** val){ struct _handler_cons _temp129; _push_handler(& _temp129);{
int _temp131= 0; if( setjmp( _temp129.handler)){ _temp131= 1;} if( ! _temp131){
Cyc_Marshal_print_type_base( ts, Cyc_Typerep_empty_addr_set(), val);;
_pop_handler();} else{ void* _temp130=( void*) _exn_thrown; void* _temp133=
_temp130; struct _tagged_arr _temp139; _LL135: if(*(( void**) _temp133) ==  Cyc_Core_Failure){
_LL140: _temp139=(( struct Cyc_Core_Failure_struct*) _temp133)->f1; goto _LL136;}
else{ goto _LL137;} _LL137: goto _LL138; _LL136:({ struct Cyc_Std_String_pa_struct
_temp142; _temp142.tag= Cyc_Std_String_pa; _temp142.f1=( struct _tagged_arr)
_temp139;{ void* _temp141[ 1u]={& _temp142}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n", sizeof( unsigned char), 19u), _tag_arr( _temp141,
sizeof( void*), 1u));}});( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp143=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp143[ 0]=({ struct Cyc_Core_Failure_struct _temp144; _temp144.tag= Cyc_Core_Failure;
_temp144.f1= _temp139; _temp144;}); _temp143;})); _LL138:( void) _throw(
_temp133); _LL134:;}}}
