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
extern void* Cyc_Typerep_get_unionbranch( unsigned int tag, struct _tagged_arr l);
extern void* Cyc_Typerep_get_xtunionbranch( struct _tagged_arr tag, struct
_tagged_arr l); unsigned int Cyc_Typerep_size_type( void* rep); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern
int Cyc_Std_fgetc( struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_fputc(
int __c, struct Cyc_Std___sFILE* __stream); extern unsigned int Cyc_Std_fread(
struct _tagged_arr __ptr, unsigned int __size, unsigned int __n, struct Cyc_Std___sFILE*
__stream); extern unsigned int Cyc_Std_fwrite( struct _tagged_arr __ptr,
unsigned int __size, unsigned int __n, struct Cyc_Std___sFILE* __s); extern
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
int tag; float* f1; } ; extern int isalnum( int); extern int isalpha( int);
extern int isascii( int); extern int iscntrl( int); extern int isdigit( int);
extern int isgraph( int); extern int islower( int); extern int isprint( int);
extern int ispunct( int); extern int isspace( int); extern int isupper( int);
extern int isxdigit( int); extern int toascii( int); extern int tolower( int);
extern int toupper( int); extern int _tolower( int); extern int _toupper( int);
extern void Cyc_Marshal_print_type( void* rep, void* val); extern void Cyc_Marshal_write_type(
void* rep, struct Cyc_Std___sFILE* fp, void* val); extern void* Cyc_Marshal_read_type(
void* rep, struct Cyc_Std___sFILE* fp); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k, void* v);
extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void*
k); extern void* unsafe_cast( void* x); struct _tuple2{ struct Cyc_Dict_Dict* f1;
int f2; } ; struct _tuple3{ struct _tagged_arr f1; int f2; } ; static int Cyc_Marshal_uint_cmp(
unsigned int a, unsigned int b){ if( a ==  b){ return 0;} else{ if( a <  b){
return 1;} else{ return - 1;}}} struct Cyc_Set_Set* Cyc_Marshal_empty_addr_set(){
return(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, unsigned int elt)) Cyc_Set_insert)(((
struct Cyc_Set_Set*(*)( int(* cmp)( unsigned int, unsigned int))) Cyc_Set_empty)(
Cyc_Marshal_uint_cmp), 0);} struct _tuple4{ int f1; struct Cyc_Set_Set* f2; } ;
struct _tuple4* Cyc_Marshal_member_insert( struct Cyc_Set_Set* set, unsigned int
val){ return({ struct _tuple4* _temp0=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp0->f1=(( int(*)( struct Cyc_Set_Set* s, unsigned int elt))
Cyc_Set_member)( set, val); _temp0->f2=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, unsigned int elt)) Cyc_Set_insert)( set, val); _temp0;});} inline int Cyc_Marshal_index_member(
struct _tuple2 idx, unsigned int m){ int _temp3; struct _tuple2 _temp1= idx;
_LL4: _temp3= _temp1.f2; goto _LL2; _LL2: return m <  _temp3;} struct _tuple2
Cyc_Marshal_empty_addr_index(){ return({ struct _tuple2 _temp5; _temp5.f1=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, unsigned int k, int v)) Cyc_Dict_insert)(((
struct Cyc_Dict_Dict*(*)( int(* cmp)( unsigned int, unsigned int))) Cyc_Dict_empty)(
Cyc_Marshal_uint_cmp), 0, 0); _temp5.f2= 1; _temp5;});} int Cyc_Marshal_index_lookup(
struct _tuple2 index, unsigned int val){ int _temp8; struct Cyc_Dict_Dict*
_temp10; struct _tuple2 _temp6= index; _LL11: _temp10= _temp6.f1; goto _LL9;
_LL9: _temp8= _temp6.f2; goto _LL7; _LL7: { struct Cyc_Core_Opt* _temp12=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, unsigned int k)) Cyc_Dict_lookup_opt)(
_temp10, val); if( _temp12 !=  0){ return( int) _temp12->v;} else{ return _temp8;}}}
struct _tuple2 Cyc_Marshal_index_insert( struct _tuple2 index, unsigned int val){
int _temp15; struct Cyc_Dict_Dict* _temp17; struct _tuple2 _temp13= index; _LL18:
_temp17= _temp13.f1; goto _LL16; _LL16: _temp15= _temp13.f2; goto _LL14; _LL14:
return({ struct _tuple2 _temp19; _temp19.f1=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, unsigned int k, int v)) Cyc_Dict_insert)( _temp17, val,
_temp15); _temp19.f2= _temp15 +  1; _temp19;});} struct _tuple3 Cyc_Marshal_empty_addr_table(){
return({ struct _tuple3 _temp20; _temp20.f1= _tag_arr(({ unsigned int _temp21=(
unsigned int) 1; unsigned int* _temp22=( unsigned int*) _cycalloc_atomic(
_check_times( sizeof( unsigned int), _temp21));{ unsigned int _temp23= _temp21;
unsigned int i; for( i= 0; i <  _temp23; i ++){ _temp22[ i]= 0;}}; _temp22;}),
sizeof( unsigned int),( unsigned int) 1); _temp20.f2= 1; _temp20;});} inline int
Cyc_Marshal_table_member( struct _tuple3 table, unsigned int m){ int _temp26;
struct _tuple3 _temp24= table; _LL27: _temp26= _temp24.f2; goto _LL25; _LL25:
return m <  _temp26;} struct _tuple3 Cyc_Marshal_table_insert( struct _tuple3
tbl, unsigned int i){ int _temp30; struct _tagged_arr _temp32; struct _tuple3
_temp28= tbl; _LL33: _temp32= _temp28.f1; goto _LL31; _LL31: _temp30= _temp28.f2;
goto _LL29; _LL29: if( _get_arr_size( _temp32, sizeof( unsigned int)) == 
_temp30){ _temp32=({ unsigned int _temp34=( unsigned int)( 2 *  _temp30);
unsigned int* _temp35=( unsigned int*) _cycalloc_atomic( _check_times( sizeof(
unsigned int), _temp34)); struct _tagged_arr _temp37= _tag_arr( _temp35, sizeof(
unsigned int),( unsigned int)( 2 *  _temp30));{ unsigned int _temp36= _temp34;
unsigned int j; for( j= 0; j <  _temp36; j ++){ _temp35[ j]= j <  _temp30?*((
unsigned int*) _check_unknown_subscript( _temp32, sizeof( unsigned int),( int) j)):
0;}}; _temp37;});}*(( unsigned int*) _check_unknown_subscript( _temp32, sizeof(
unsigned int), _temp30))= i; return({ struct _tuple3 _temp38; _temp38.f1=
_temp32; _temp38.f2= _temp30 +  1; _temp38;});} unsigned int Cyc_Marshal_table_lookup(
struct _tuple3 tbl, int i){ return*(( unsigned int*) _check_unknown_subscript(
tbl.f1, sizeof( unsigned int), i));} struct _tuple5{ unsigned int f1;
unsigned int f2; unsigned int f3; } ; struct Cyc_Set_Set* Cyc_Marshal_print_type_base(
void* rep, struct Cyc_Set_Set* env, void* val){{ void* _temp39= rep;
unsigned int _temp61; void* _temp63; unsigned int _temp65; void* _temp67; struct
_tagged_arr _temp69; struct _tagged_arr _temp71; struct _tagged_arr _temp73;
struct _tagged_arr _temp75; _LL41: if(( unsigned int) _temp39 >  2u?*(( int*)
_temp39) ==  Cyc_Typerep_Int: 0){ _LL62: _temp61=(( struct Cyc_Typerep_Int_struct*)
_temp39)->f1; goto _LL42;} else{ goto _LL43;} _LL43: if( _temp39 == ( void*) Cyc_Typerep_Float){
goto _LL44;} else{ goto _LL45;} _LL45: if( _temp39 == ( void*) Cyc_Typerep_Double){
goto _LL46;} else{ goto _LL47;} _LL47: if(( unsigned int) _temp39 >  2u?*(( int*)
_temp39) ==  Cyc_Typerep_ThinPtr: 0){ _LL66: _temp65=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp39)->f1; goto _LL64; _LL64: _temp63=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp39)->f2; goto _LL48;} else{ goto _LL49;} _LL49: if(( unsigned int) _temp39
>  2u?*(( int*) _temp39) ==  Cyc_Typerep_FatPtr: 0){ _LL68: _temp67=( void*)((
struct Cyc_Typerep_FatPtr_struct*) _temp39)->f1; goto _LL50;} else{ goto _LL51;}
_LL51: if(( unsigned int) _temp39 >  2u?*(( int*) _temp39) ==  Cyc_Typerep_Tuple:
0){ _LL70: _temp69=(( struct Cyc_Typerep_Tuple_struct*) _temp39)->f2; goto _LL52;}
else{ goto _LL53;} _LL53: if(( unsigned int) _temp39 >  2u?*(( int*) _temp39) == 
Cyc_Typerep_TUnion: 0){ _LL72: _temp71=(( struct Cyc_Typerep_TUnion_struct*)
_temp39)->f1; goto _LL54;} else{ goto _LL55;} _LL55: if(( unsigned int) _temp39
>  2u?*(( int*) _temp39) ==  Cyc_Typerep_XTUnion: 0){ _LL74: _temp73=(( struct
Cyc_Typerep_XTUnion_struct*) _temp39)->f1; goto _LL56;} else{ goto _LL57;} _LL57:
if(( unsigned int) _temp39 >  2u?*(( int*) _temp39) ==  Cyc_Typerep_Union: 0){
_LL76: _temp75=(( struct Cyc_Typerep_Union_struct*) _temp39)->f1; goto _LL58;}
else{ goto _LL59;} _LL59: goto _LL60; _LL42: switch( _temp61){ case 8: _LL77: {
unsigned char* x=(( unsigned char*(*)( void* x)) unsafe_cast)( val); if( isprint((
int)* x)){({ struct Cyc_Std_Int_pa_struct _temp80; _temp80.tag= Cyc_Std_Int_pa;
_temp80.f1=( int)(( unsigned int)(( int)* x));{ void* _temp79[ 1u]={& _temp80};
Cyc_Std_printf( _tag_arr("'%c'", sizeof( unsigned char), 5u), _tag_arr( _temp79,
sizeof( void*), 1u));}});} else{({ struct Cyc_Std_Int_pa_struct _temp82; _temp82.tag=
Cyc_Std_Int_pa; _temp82.f1=( int)(( unsigned int)(( int)* x));{ void* _temp81[ 1u]={&
_temp82}; Cyc_Std_printf( _tag_arr("'\\%d'", sizeof( unsigned char), 6u),
_tag_arr( _temp81, sizeof( void*), 1u));}});} break;} case 16: _LL78: { short* x=((
short*(*)( void* x)) unsafe_cast)( val);({ struct Cyc_Std_Int_pa_struct _temp85;
_temp85.tag= Cyc_Std_Int_pa; _temp85.f1=( int)(( unsigned int)(( int)* x));{
void* _temp84[ 1u]={& _temp85}; Cyc_Std_printf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp84, sizeof( void*), 1u));}}); break;} case
32: _LL83: { int* x=(( int*(*)( void* x)) unsafe_cast)( val);({ struct Cyc_Std_Int_pa_struct
_temp88; _temp88.tag= Cyc_Std_Int_pa; _temp88.f1=( int)(( unsigned int)* x);{
void* _temp87[ 1u]={& _temp88}; Cyc_Std_printf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp87, sizeof( void*), 1u));}}); break;}
default: _LL86:({ struct Cyc_Std_Int_pa_struct _temp91; _temp91.tag= Cyc_Std_Int_pa;
_temp91.f1=( int) _temp61;{ void* _temp90[ 1u]={& _temp91}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("print_type_base: illegal int size %d bits\n", sizeof( unsigned char),
43u), _tag_arr( _temp90, sizeof( void*), 1u));}}); exit( 1);} goto _LL40; _LL44: {
float* x=(( float*(*)( void* x)) unsafe_cast)( val);({ struct Cyc_Std_Double_pa_struct
_temp93; _temp93.tag= Cyc_Std_Double_pa; _temp93.f1=( double)* x;{ void* _temp92[
1u]={& _temp93}; Cyc_Std_printf( _tag_arr("%f", sizeof( unsigned char), 3u),
_tag_arr( _temp92, sizeof( void*), 1u));}}); goto _LL40;} _LL46: { double* x=((
double*(*)( void* x)) unsafe_cast)( val);({ struct Cyc_Std_Double_pa_struct
_temp95; _temp95.tag= Cyc_Std_Double_pa; _temp95.f1=* x;{ void* _temp94[ 1u]={&
_temp95}; Cyc_Std_printf( _tag_arr("%f", sizeof( unsigned char), 3u), _tag_arr(
_temp94, sizeof( void*), 1u));}}); goto _LL40;} _LL48: { int* x=(( int*(*)( void*
x)) unsafe_cast)( val); int y=*(( int*) _check_null( x)); struct _tuple4 _temp98;
struct Cyc_Set_Set* _temp99; int _temp101; struct _tuple4* _temp96= Cyc_Marshal_member_insert(
env,( unsigned int) y); _temp98=* _temp96; _LL102: _temp101= _temp98.f1; goto
_LL100; _LL100: _temp99= _temp98.f2; goto _LL97; _LL97: env= _temp99; if( !
_temp101){({ struct Cyc_Std_Int_pa_struct _temp104; _temp104.tag= Cyc_Std_Int_pa;
_temp104.f1=( unsigned int) y;{ void* _temp103[ 1u]={& _temp104}; Cyc_Std_printf(
_tag_arr("{(@%x) ", sizeof( unsigned char), 8u), _tag_arr( _temp103, sizeof(
void*), 1u));}}); if( y !=  0){ int typ_szb=( int) Cyc_Typerep_size_type(
_temp63); int i= 0; for( 0; i <  _temp65; i ++){ env= Cyc_Marshal_print_type_base(
_temp63, env,( void*)(( void*(*)( int x)) unsafe_cast)( y)); if( i !=  _temp65 - 
1){({ void* _temp105[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp105, sizeof( void*), 0u));});} y += typ_szb;}}
else{({ void* _temp106[ 0u]={}; Cyc_Std_printf( _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp106, sizeof( void*), 0u));});}({ void*
_temp107[ 0u]={}; Cyc_Std_printf( _tag_arr(" }", sizeof( unsigned char), 3u),
_tag_arr( _temp107, sizeof( void*), 0u));});} else{({ struct Cyc_Std_Int_pa_struct
_temp109; _temp109.tag= Cyc_Std_Int_pa; _temp109.f1=( unsigned int) y;{ void*
_temp108[ 1u]={& _temp109}; Cyc_Std_printf( _tag_arr("(#%x)", sizeof(
unsigned char), 6u), _tag_arr( _temp108, sizeof( void*), 1u));}});} goto _LL40;}
_LL50: { struct _tuple5* x=(( struct _tuple5*(*)( void* x)) unsafe_cast)( val);
unsigned int typ_szb= Cyc_Typerep_size_type( _temp67); unsigned int base=(* x).f1;
unsigned int curr=(* x).f2; unsigned int last_plus_one=(* x).f3; struct _tuple4
_temp112; struct Cyc_Set_Set* _temp113; int _temp115; struct _tuple4* _temp110=
Cyc_Marshal_member_insert( env, base); _temp112=* _temp110; _LL116: _temp115=
_temp112.f1; goto _LL114; _LL114: _temp113= _temp112.f2; goto _LL111; _LL111:
env= _temp113; if( ! _temp115){({ struct Cyc_Std_Int_pa_struct _temp118;
_temp118.tag= Cyc_Std_Int_pa; _temp118.f1= base;{ void* _temp117[ 1u]={&
_temp118}; Cyc_Std_printf( _tag_arr("[(@%x) ", sizeof( unsigned char), 8u),
_tag_arr( _temp117, sizeof( void*), 1u));}});{ unsigned int i= base; for( 0; i < 
last_plus_one; i += typ_szb){ env= Cyc_Marshal_print_type_base( _temp67, env,(
void*)(( void*(*)( unsigned int x)) unsafe_cast)( i)); if( i +  typ_szb < 
last_plus_one){({ void* _temp119[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp119, sizeof( void*), 0u));});}}}({ void*
_temp120[ 0u]={}; Cyc_Std_printf( _tag_arr(" ]", sizeof( unsigned char), 3u),
_tag_arr( _temp120, sizeof( void*), 0u));});} else{({ struct Cyc_Std_Int_pa_struct
_temp122; _temp122.tag= Cyc_Std_Int_pa; _temp122.f1= base;{ void* _temp121[ 1u]={&
_temp122}; Cyc_Std_printf( _tag_arr("(#%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp121, sizeof( void*), 1u));}});} goto _LL40;} _LL52:({ void*
_temp123[ 0u]={}; Cyc_Std_printf( _tag_arr("$( ", sizeof( unsigned char), 4u),
_tag_arr( _temp123, sizeof( void*), 0u));});{ int x=(( int(*)( void* x))
unsafe_cast)( val);{ int i= 0; for( 0; i <  _get_arr_size( _temp69, sizeof(
struct _tuple0*)); i ++){ void* _temp126; unsigned int _temp128; struct _tuple0
_temp124=*(*(( struct _tuple0**) _check_unknown_subscript( _temp69, sizeof(
struct _tuple0*), i))); _LL129: _temp128= _temp124.f1; goto _LL127; _LL127:
_temp126= _temp124.f2; goto _LL125; _LL125: { int x_ofs=( int)( x +  _temp128);
env= Cyc_Marshal_print_type_base( _temp126, env,( void*)(( void*(*)( int x))
unsafe_cast)( x_ofs)); if( i !=  _get_arr_size( _temp69, sizeof( struct _tuple0*))
-  1){({ void* _temp130[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp130, sizeof( void*), 0u));});}}}}({ void*
_temp131[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof( unsigned char), 3u),
_tag_arr( _temp131, sizeof( void*), 0u));}); goto _LL40;} _LL54:({ void*
_temp132[ 0u]={}; Cyc_Std_printf( _tag_arr("&[( ", sizeof( unsigned char), 5u),
_tag_arr( _temp132, sizeof( void*), 0u));});{ unsigned int* tagp=(( unsigned int*(*)(
void* x)) unsafe_cast)( val); if(* tagp <  1024){({ struct Cyc_Std_Int_pa_struct
_temp134; _temp134.tag= Cyc_Std_Int_pa; _temp134.f1=( int)* tagp;{ void*
_temp133[ 1u]={& _temp134}; Cyc_Std_printf( _tag_arr("%d", sizeof( unsigned char),
3u), _tag_arr( _temp133, sizeof( void*), 1u));}});} else{ struct _tuple4
_temp137; struct Cyc_Set_Set* _temp138; int _temp140; struct _tuple4* _temp135=
Cyc_Marshal_member_insert( env,* tagp); _temp137=* _temp135; _LL141: _temp140=
_temp137.f1; goto _LL139; _LL139: _temp138= _temp137.f2; goto _LL136; _LL136:
env= _temp138; if( ! _temp140){({ struct Cyc_Std_Int_pa_struct _temp143;
_temp143.tag= Cyc_Std_Int_pa; _temp143.f1=* tagp;{ void* _temp142[ 1u]={&
_temp143}; Cyc_Std_printf( _tag_arr("(@%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp142, sizeof( void*), 1u));}});{ unsigned int* x=(( unsigned int*(*)(
unsigned int x)) unsafe_cast)(* tagp); unsigned int tag=* x; void* typ= Cyc_Typerep_get_unionbranch(
tag, _temp71); env=(( struct Cyc_Set_Set*(*)( void* rep, struct Cyc_Set_Set* env,
unsigned int* val)) Cyc_Marshal_print_type_base)( typ, env, x);}} else{({ struct
Cyc_Std_Int_pa_struct _temp145; _temp145.tag= Cyc_Std_Int_pa; _temp145.f1=* tagp;{
void* _temp144[ 1u]={& _temp145}; Cyc_Std_printf( _tag_arr("(#%x)", sizeof(
unsigned char), 6u), _tag_arr( _temp144, sizeof( void*), 1u));}});}}({ void*
_temp146[ 0u]={}; Cyc_Std_printf( _tag_arr(" )]", sizeof( unsigned char), 4u),
_tag_arr( _temp146, sizeof( void*), 0u));}); goto _LL40;} _LL56: { unsigned int*
xtunionp=(( unsigned int*(*)( void* x)) unsafe_cast)( val); struct _tuple4
_temp149; struct Cyc_Set_Set* _temp150; int _temp152; struct _tuple4* _temp147=
Cyc_Marshal_member_insert( env,* xtunionp); _temp149=* _temp147; _LL153:
_temp152= _temp149.f1; goto _LL151; _LL151: _temp150= _temp149.f2; goto _LL148;
_LL148: env= _temp150; if( ! _temp152){({ struct Cyc_Std_Int_pa_struct _temp155;
_temp155.tag= Cyc_Std_Int_pa; _temp155.f1=* xtunionp;{ void* _temp154[ 1u]={&
_temp155}; Cyc_Std_printf( _tag_arr("(@%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp154, sizeof( void*), 1u));}});{ unsigned int* xtstructp=((
unsigned int*(*)( unsigned int x)) unsafe_cast)(* xtunionp); if(* xtstructp != 
0){ unsigned char* xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(*
xtstructp +  4); struct _tagged_arr xtname=( struct _tagged_arr)
Cstring_to_string( xtnamec);({ struct Cyc_Std_String_pa_struct _temp157;
_temp157.tag= Cyc_Std_String_pa; _temp157.f1=( struct _tagged_arr) xtname;{ void*
_temp156[ 1u]={& _temp157}; Cyc_Std_printf( _tag_arr("&%s[( ", sizeof(
unsigned char), 7u), _tag_arr( _temp156, sizeof( void*), 1u));}});{ void* typ=
Cyc_Typerep_get_xtunionbranch( xtname, _temp73); env=(( struct Cyc_Set_Set*(*)(
void* rep, struct Cyc_Set_Set* env, unsigned int* val)) Cyc_Marshal_print_type_base)(
typ, env,( unsigned int*) _check_null( xtstructp));}} else{ unsigned char*
xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(* xtunionp +  4);
struct _tagged_arr xtname=( struct _tagged_arr) Cstring_to_string( xtnamec);({
struct Cyc_Std_String_pa_struct _temp159; _temp159.tag= Cyc_Std_String_pa;
_temp159.f1=( struct _tagged_arr) xtname;{ void* _temp158[ 1u]={& _temp159}; Cyc_Std_printf(
_tag_arr("&%s[( ", sizeof( unsigned char), 7u), _tag_arr( _temp158, sizeof( void*),
1u));}});}}} else{({ struct Cyc_Std_Int_pa_struct _temp161; _temp161.tag= Cyc_Std_Int_pa;
_temp161.f1=* xtunionp;{ void* _temp160[ 1u]={& _temp161}; Cyc_Std_printf(
_tag_arr("(#%x)", sizeof( unsigned char), 6u), _tag_arr( _temp160, sizeof( void*),
1u));}});}({ void* _temp162[ 0u]={}; Cyc_Std_printf( _tag_arr(" )]", sizeof(
unsigned char), 4u), _tag_arr( _temp162, sizeof( void*), 0u));}); goto _LL40;}
_LL58:({ void* _temp163[ 0u]={}; Cyc_Std_printf( _tag_arr("[[", sizeof(
unsigned char), 3u), _tag_arr( _temp163, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp75, sizeof( void**)); i ++){ Cyc_Marshal_print_type_base(*(*((
void***) _check_unknown_subscript( _temp75, sizeof( void**), i))), env, val);
if( i <  _get_arr_size( _temp75, sizeof( void**)) -  1){({ void* _temp164[ 0u]={};
Cyc_Std_printf( _tag_arr(" | ", sizeof( unsigned char), 4u), _tag_arr( _temp164,
sizeof( void*), 0u));});}}}({ void* _temp165[ 0u]={}; Cyc_Std_printf( _tag_arr("]]",
sizeof( unsigned char), 3u), _tag_arr( _temp165, sizeof( void*), 0u));}); goto
_LL40; _LL60:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp166=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp166[ 0]=({ struct Cyc_Core_Failure_struct _temp167; _temp167.tag= Cyc_Core_Failure;
_temp167.f1= _tag_arr("Unhandled case in print_type_base", sizeof( unsigned char),
34u); _temp167;}); _temp166;})); _LL40:;} return env;} extern int*
__errno_location(); void Cyc_Marshal_print_type( void* ts, void* val){ struct
_handler_cons _temp168; _push_handler(& _temp168);{ int _temp170= 0; if( setjmp(
_temp168.handler)){ _temp170= 1;} if( ! _temp170){ Cyc_Marshal_print_type_base(
ts, Cyc_Marshal_empty_addr_set(), val);; _pop_handler();} else{ void* _temp169=(
void*) _exn_thrown; void* _temp172= _temp169; struct _tagged_arr _temp178;
_LL174: if(*(( void**) _temp172) ==  Cyc_Core_Failure){ _LL179: _temp178=((
struct Cyc_Core_Failure_struct*) _temp172)->f1; goto _LL175;} else{ goto _LL176;}
_LL176: goto _LL177; _LL175:({ struct Cyc_Std_String_pa_struct _temp181;
_temp181.tag= Cyc_Std_String_pa; _temp181.f1=( struct _tagged_arr) _temp178;{
void* _temp180[ 1u]={& _temp181}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Core::Failure(%s)\n",
sizeof( unsigned char), 19u), _tag_arr( _temp180, sizeof( void*), 1u));}});( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp182=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp182[ 0]=({ struct Cyc_Core_Failure_struct
_temp183; _temp183.tag= Cyc_Core_Failure; _temp183.f1= _temp178; _temp183;});
_temp182;})); _LL177:( void) _throw( _temp172); _LL173:;}}} void Cyc_Marshal_cycputchar(
unsigned char x, struct Cyc_Std___sFILE* fp){ if( Cyc_Std_fwrite(( struct
_tagged_arr)(( struct _tagged_arr) _tag_arr(& x, sizeof( unsigned char), 1u)),
sizeof( unsigned char), 1, fp) <  1){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp184=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp184[ 0]=({ struct Cyc_Core_Failure_struct _temp185; _temp185.tag= Cyc_Core_Failure;
_temp185.f1= _tag_arr("Write failure", sizeof( unsigned char), 14u); _temp185;});
_temp184;}));}} void Cyc_Marshal_cycputshort( short x, struct Cyc_Std___sFILE*
fp){ if( Cyc_Std_fwrite(( struct _tagged_arr)(( struct _tagged_arr) _tag_arr(& x,
sizeof( short), 1u)), sizeof( short), 1, fp) <  1){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp186=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp186[ 0]=({ struct Cyc_Core_Failure_struct
_temp187; _temp187.tag= Cyc_Core_Failure; _temp187.f1= _tag_arr("Write failure",
sizeof( unsigned char), 14u); _temp187;}); _temp186;}));}} void Cyc_Marshal_cycputint(
int x, struct Cyc_Std___sFILE* fp){ if( Cyc_Std_fwrite(( struct _tagged_arr)((
struct _tagged_arr) _tag_arr(& x, sizeof( int), 1u)), sizeof( int), 1, fp) <  1){(
int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp188=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp188[ 0]=({ struct Cyc_Core_Failure_struct
_temp189; _temp189.tag= Cyc_Core_Failure; _temp189.f1= _tag_arr("Write failure",
sizeof( unsigned char), 14u); _temp189;}); _temp188;}));}} void Cyc_Marshal_cycputfloat(
float x, struct Cyc_Std___sFILE* fp){ if( Cyc_Std_fwrite(( struct _tagged_arr)((
struct _tagged_arr) _tag_arr(& x, sizeof( float), 1u)), sizeof( float), 1, fp) < 
1){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp190=( struct
Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp190[ 0]=({ struct Cyc_Core_Failure_struct _temp191; _temp191.tag= Cyc_Core_Failure;
_temp191.f1= _tag_arr("Write failure", sizeof( unsigned char), 14u); _temp191;});
_temp190;}));}} void Cyc_Marshal_cycputdouble( double x, struct Cyc_Std___sFILE*
fp){ if( Cyc_Std_fwrite(( struct _tagged_arr)(( struct _tagged_arr) _tag_arr(& x,
sizeof( double), 1u)), sizeof( double), 1, fp) <  1){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp192=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp192[ 0]=({ struct Cyc_Core_Failure_struct
_temp193; _temp193.tag= Cyc_Core_Failure; _temp193.f1= _tag_arr("Write failure",
sizeof( unsigned char), 14u); _temp193;}); _temp192;}));}} void Cyc_Marshal_cycputvarint(
unsigned int i, struct Cyc_Std___sFILE* fp){ int width= 0; unsigned int mask=
-256; while(( int) mask) { if(( int)( i &  mask)){ width ++;} mask <<= 8;} Cyc_Std_fputc((
int)( width <<  6 |  i &  63), fp); i >>= 6;{ int j= 0; for( 0; j <  width; j ++){
Cyc_Std_fputc(( int)( i &  255), fp); i >>= 8;}}} unsigned char Cyc_Marshal_cycgetchar(
struct Cyc_Std___sFILE* fp){ struct _RegionHandle _temp194= _new_region("r");
struct _RegionHandle* r=& _temp194; _push_region( r);{ struct _tagged_arr x=
_tag_arr(({ unsigned int _temp198= sizeof( unsigned char); unsigned char*
_temp199=( unsigned char*) _region_malloc( r, _check_times( sizeof(
unsigned char), _temp198));{ unsigned int _temp200= _temp198; unsigned int i;
for( i= 0; i <  _temp200; i ++){ _temp199[ i]='\000';}}; _temp199;}), sizeof(
unsigned char), sizeof( unsigned char)); if( Cyc_Std_fread( x, sizeof(
unsigned char), 1, fp) <  1){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp195=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp195[ 0]=({ struct Cyc_Core_Failure_struct _temp196; _temp196.tag= Cyc_Core_Failure;
_temp196.f1= _tag_arr("Read failure", sizeof( unsigned char), 13u); _temp196;});
_temp195;}));}{ unsigned char* xx=(( unsigned char*(*)( unsigned char* x))
unsafe_cast)(( unsigned char*) _check_null( _untag_arr( x, sizeof( unsigned char),
1u))); unsigned char _temp197=* xx; _npop_handler( 0u); return _temp197;}};
_pop_region( r);} short Cyc_Marshal_cycgetshort( struct Cyc_Std___sFILE* fp){
struct _RegionHandle _temp201= _new_region("r"); struct _RegionHandle* r=&
_temp201; _push_region( r);{ struct _tagged_arr x= _tag_arr(({ unsigned int
_temp205= sizeof( short); unsigned char* _temp206=( unsigned char*)
_region_malloc( r, _check_times( sizeof( unsigned char), _temp205));{
unsigned int _temp207= _temp205; unsigned int i; for( i= 0; i <  _temp207; i ++){
_temp206[ i]='\000';}}; _temp206;}), sizeof( unsigned char), sizeof( short));
if( Cyc_Std_fread( x, sizeof( short), 1, fp) <  1){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp202=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp202[ 0]=({ struct Cyc_Core_Failure_struct
_temp203; _temp203.tag= Cyc_Core_Failure; _temp203.f1= _tag_arr("Read failure",
sizeof( unsigned char), 13u); _temp203;}); _temp202;}));}{ short* xx=(( short*(*)(
unsigned char* x)) unsafe_cast)(( unsigned char*) _check_null( _untag_arr( x,
sizeof( unsigned char), 1u))); short _temp204=* xx; _npop_handler( 0u); return
_temp204;}}; _pop_region( r);} int Cyc_Marshal_cycgetint( struct Cyc_Std___sFILE*
fp){ struct _RegionHandle _temp208= _new_region("r"); struct _RegionHandle* r=&
_temp208; _push_region( r);{ struct _tagged_arr x= _tag_arr(({ unsigned int
_temp212= sizeof( int); unsigned char* _temp213=( unsigned char*) _region_malloc(
r, _check_times( sizeof( unsigned char), _temp212));{ unsigned int _temp214=
_temp212; unsigned int i; for( i= 0; i <  _temp214; i ++){ _temp213[ i]='\000';}};
_temp213;}), sizeof( unsigned char), sizeof( int)); if( Cyc_Std_fread( x,
sizeof( int), 1, fp) <  1){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp209=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp209[ 0]=({ struct Cyc_Core_Failure_struct _temp210; _temp210.tag= Cyc_Core_Failure;
_temp210.f1= _tag_arr("Read failure", sizeof( unsigned char), 13u); _temp210;});
_temp209;}));}{ int* xx=(( int*(*)( unsigned char* x)) unsafe_cast)((
unsigned char*) _check_null( _untag_arr( x, sizeof( unsigned char), 1u))); int
_temp211=* xx; _npop_handler( 0u); return _temp211;}}; _pop_region( r);} float
Cyc_Marshal_cycgetfloat( struct Cyc_Std___sFILE* fp){ struct _RegionHandle
_temp215= _new_region("r"); struct _RegionHandle* r=& _temp215; _push_region( r);{
struct _tagged_arr x= _tag_arr(({ unsigned int _temp219= sizeof( float);
unsigned char* _temp220=( unsigned char*) _region_malloc( r, _check_times(
sizeof( unsigned char), _temp219));{ unsigned int _temp221= _temp219;
unsigned int i; for( i= 0; i <  _temp221; i ++){ _temp220[ i]='\000';}};
_temp220;}), sizeof( unsigned char), sizeof( float)); if( Cyc_Std_fread( x,
sizeof( float), 1, fp) <  1){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp216=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp216[ 0]=({ struct Cyc_Core_Failure_struct _temp217; _temp217.tag= Cyc_Core_Failure;
_temp217.f1= _tag_arr("Read failure", sizeof( unsigned char), 13u); _temp217;});
_temp216;}));}{ float* xx=(( float*(*)( unsigned char* x)) unsafe_cast)((
unsigned char*) _check_null( _untag_arr( x, sizeof( unsigned char), 1u))); float
_temp218=* xx; _npop_handler( 0u); return _temp218;}}; _pop_region( r);} double
Cyc_Marshal_cycgetdouble( struct Cyc_Std___sFILE* fp){ struct _RegionHandle
_temp222= _new_region("r"); struct _RegionHandle* r=& _temp222; _push_region( r);{
struct _tagged_arr x= _tag_arr(({ unsigned int _temp226= sizeof( double);
unsigned char* _temp227=( unsigned char*) _region_malloc( r, _check_times(
sizeof( unsigned char), _temp226));{ unsigned int _temp228= _temp226;
unsigned int i; for( i= 0; i <  _temp228; i ++){ _temp227[ i]='\000';}};
_temp227;}), sizeof( unsigned char), sizeof( double)); if( Cyc_Std_fread( x,
sizeof( double), 1, fp) <  1){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp223=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp223[ 0]=({ struct Cyc_Core_Failure_struct _temp224; _temp224.tag= Cyc_Core_Failure;
_temp224.f1= _tag_arr("Read failure", sizeof( unsigned char), 13u); _temp224;});
_temp223;}));}{ double* xx=(( double*(*)( unsigned char* x)) unsafe_cast)((
unsigned char*) _check_null( _untag_arr( x, sizeof( unsigned char), 1u)));
double _temp225=* xx; _npop_handler( 0u); return _temp225;}}; _pop_region( r);}
unsigned int Cyc_Marshal_cycgetvarint( struct Cyc_Std___sFILE* fp){ int i= 0;
int j=( int) Cyc_Marshal_cycgetchar( fp); int width=( j &  192) >>  6; int pos=
6; i += j &  63;{ int k= 0; for( 0; k <  width; k ++){ j=( int) Cyc_Marshal_cycgetchar(
fp); i |= j <<  pos; pos += 8;}} return( unsigned int) i;} struct _tuple2 Cyc_Marshal_write_type_base(
void* rep, struct _tuple2 env, struct Cyc_Std___sFILE* fp, void* val){{ void*
_temp229= rep; unsigned int _temp251; void* _temp253; unsigned int _temp255;
void* _temp257; struct _tagged_arr _temp259; struct _tagged_arr _temp261; struct
_tagged_arr _temp263; struct _tagged_arr _temp265; _LL231: if(( unsigned int)
_temp229 >  2u?*(( int*) _temp229) ==  Cyc_Typerep_Int: 0){ _LL252: _temp251=((
struct Cyc_Typerep_Int_struct*) _temp229)->f1; goto _LL232;} else{ goto _LL233;}
_LL233: if( _temp229 == ( void*) Cyc_Typerep_Float){ goto _LL234;} else{ goto
_LL235;} _LL235: if( _temp229 == ( void*) Cyc_Typerep_Double){ goto _LL236;}
else{ goto _LL237;} _LL237: if(( unsigned int) _temp229 >  2u?*(( int*) _temp229)
==  Cyc_Typerep_ThinPtr: 0){ _LL256: _temp255=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp229)->f1; goto _LL254; _LL254: _temp253=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp229)->f2; goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int)
_temp229 >  2u?*(( int*) _temp229) ==  Cyc_Typerep_FatPtr: 0){ _LL258: _temp257=(
void*)(( struct Cyc_Typerep_FatPtr_struct*) _temp229)->f1; goto _LL240;} else{
goto _LL241;} _LL241: if(( unsigned int) _temp229 >  2u?*(( int*) _temp229) == 
Cyc_Typerep_Tuple: 0){ _LL260: _temp259=(( struct Cyc_Typerep_Tuple_struct*)
_temp229)->f2; goto _LL242;} else{ goto _LL243;} _LL243: if(( unsigned int)
_temp229 >  2u?*(( int*) _temp229) ==  Cyc_Typerep_TUnion: 0){ _LL262: _temp261=((
struct Cyc_Typerep_TUnion_struct*) _temp229)->f1; goto _LL244;} else{ goto
_LL245;} _LL245: if(( unsigned int) _temp229 >  2u?*(( int*) _temp229) ==  Cyc_Typerep_XTUnion:
0){ _LL264: _temp263=(( struct Cyc_Typerep_XTUnion_struct*) _temp229)->f1; goto
_LL246;} else{ goto _LL247;} _LL247: if(( unsigned int) _temp229 >  2u?*(( int*)
_temp229) ==  Cyc_Typerep_Union: 0){ _LL266: _temp265=(( struct Cyc_Typerep_Union_struct*)
_temp229)->f1; goto _LL248;} else{ goto _LL249;} _LL249: goto _LL250; _LL232:
switch( _temp251){ case 8: _LL267: { unsigned char* x=(( unsigned char*(*)( void*
x)) unsafe_cast)( val); Cyc_Marshal_cycputchar(* x, fp); break;} case 16: _LL268: {
short* x=(( short*(*)( void* x)) unsafe_cast)( val); Cyc_Marshal_cycputshort(* x,
fp); break;} case 32: _LL269: { int* x=(( int*(*)( void* x)) unsafe_cast)( val);
Cyc_Marshal_cycputint(* x, fp); break;} default: _LL270:({ struct Cyc_Std_Int_pa_struct
_temp273; _temp273.tag= Cyc_Std_Int_pa; _temp273.f1=( int) _temp251;{ void*
_temp272[ 1u]={& _temp273}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("write_type_base: illegal int size %d bits\n",
sizeof( unsigned char), 43u), _tag_arr( _temp272, sizeof( void*), 1u));}}); exit(
1);} goto _LL230; _LL234: { float* x=(( float*(*)( void* x)) unsafe_cast)( val);
Cyc_Marshal_cycputfloat(* x, fp); goto _LL230;} _LL236: { double* x=(( double*(*)(
void* x)) unsafe_cast)( val); Cyc_Marshal_cycputdouble(* x, fp); goto _LL230;}
_LL238: { int* x=(( int*(*)( void* x)) unsafe_cast)( val); int y=*(( int*)
_check_null( x)); int _temp274= Cyc_Marshal_index_lookup( env,( unsigned int) y);
Cyc_Marshal_cycputvarint(( unsigned int) _temp274, fp); if( ! Cyc_Marshal_index_member(
env,( unsigned int) _temp274)){ env= Cyc_Marshal_index_insert( env,(
unsigned int) y);{ int typ_szb=( int) Cyc_Typerep_size_type( _temp253); int i= 0;
for( 0; i <  _temp255; i ++){ env= Cyc_Marshal_write_type_base( _temp253, env,
fp,( void*)(( void*(*)( int x)) unsafe_cast)( y)); y += typ_szb;}}} goto _LL230;}
_LL240: { struct _tuple5* x=(( struct _tuple5*(*)( void* x)) unsafe_cast)( val);
unsigned int typ_szb= Cyc_Typerep_size_type( _temp257); unsigned int base=(* x).f1;
unsigned int curr=(* x).f2; unsigned int last_plus_one=(* x).f3; unsigned int
pos=( curr -  base) /  typ_szb; unsigned int sz=( last_plus_one -  base) / 
typ_szb; int _temp275= Cyc_Marshal_index_lookup( env, base); Cyc_Marshal_cycputvarint((
unsigned int) _temp275, fp); Cyc_Marshal_cycputvarint( pos, fp); Cyc_Marshal_cycputvarint(
sz, fp); if( ! Cyc_Marshal_index_member( env,( unsigned int) _temp275)){ env=
Cyc_Marshal_index_insert( env, base);{ unsigned int i= base; for( 0; i < 
last_plus_one; i += typ_szb){ env= Cyc_Marshal_write_type_base( _temp257, env,
fp,( void*)(( void*(*)( unsigned int x)) unsafe_cast)( i));}}} goto _LL230;}
_LL242: { int x=(( int(*)( void* x)) unsafe_cast)( val);{ int i= 0; for( 0; i < 
_get_arr_size( _temp259, sizeof( struct _tuple0*)); i ++){ void* _temp278;
unsigned int _temp280; struct _tuple0 _temp276=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp259, sizeof( struct _tuple0*), i))); _LL281:
_temp280= _temp276.f1; goto _LL279; _LL279: _temp278= _temp276.f2; goto _LL277;
_LL277: { int x_ofs=( int)( x +  _temp280); env= Cyc_Marshal_write_type_base(
_temp278, env, fp,( void*)(( void*(*)( int x)) unsafe_cast)( x_ofs));}}} goto
_LL230;} _LL244: { unsigned int* tagp=(( unsigned int*(*)( void* x)) unsafe_cast)(
val); if(* tagp <  1024){ Cyc_Marshal_cycputvarint(* tagp, fp);} else{ int
_temp282= Cyc_Marshal_index_lookup( env,* tagp); Cyc_Marshal_cycputvarint((
unsigned int)( _temp282 +  1024), fp); if( ! Cyc_Marshal_index_member( env,(
unsigned int) _temp282)){ env= Cyc_Marshal_index_insert( env,* tagp);{
unsigned int* x=(( unsigned int*(*)( unsigned int x)) unsafe_cast)(* tagp);
unsigned int tag=* x; Cyc_Marshal_cycputvarint( tag, fp);{ void* typ= Cyc_Typerep_get_unionbranch(
tag, _temp261); env=(( struct _tuple2(*)( void* rep, struct _tuple2 env, struct
Cyc_Std___sFILE* fp, unsigned int* val)) Cyc_Marshal_write_type_base)( typ, env,
fp, x);}}}} goto _LL230;} _LL246: { unsigned int* xtunionp=(( unsigned int*(*)(
void* x)) unsafe_cast)( val); int _temp283= Cyc_Marshal_index_lookup( env,*
xtunionp); Cyc_Marshal_cycputvarint(( unsigned int) _temp283, fp); if( ! Cyc_Marshal_index_member(
env,( unsigned int) _temp283)){ env= Cyc_Marshal_index_insert( env,* xtunionp);{
unsigned int* xtstructp=(( unsigned int*(*)( unsigned int x)) unsafe_cast)(*
xtunionp); if(* xtstructp !=  0){ Cyc_Marshal_cycputchar('\001', fp);{
unsigned char* xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(*
xtstructp +  4); struct _tagged_arr xtname=( struct _tagged_arr)
Cstring_to_string( xtnamec);({ struct Cyc_Std_String_pa_struct _temp285;
_temp285.tag= Cyc_Std_String_pa; _temp285.f1=( struct _tagged_arr) xtname;{ void*
_temp284[ 1u]={& _temp285}; Cyc_Std_fprintf( fp, _tag_arr("%s", sizeof(
unsigned char), 3u), _tag_arr( _temp284, sizeof( void*), 1u));}}); Cyc_Marshal_cycputchar('\000',
fp);{ void* typ= Cyc_Typerep_get_xtunionbranch( xtname, _temp263); env=(( struct
_tuple2(*)( void* rep, struct _tuple2 env, struct Cyc_Std___sFILE* fp,
unsigned int* val)) Cyc_Marshal_write_type_base)( typ, env, fp,( unsigned int*)
_check_null( xtstructp));}}} else{ Cyc_Marshal_cycputchar('\000', fp);{
unsigned char* xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(*
xtunionp +  4); struct _tagged_arr xtname=( struct _tagged_arr)
Cstring_to_string( xtnamec);({ struct Cyc_Std_String_pa_struct _temp287;
_temp287.tag= Cyc_Std_String_pa; _temp287.f1=( struct _tagged_arr) xtname;{ void*
_temp286[ 1u]={& _temp287}; Cyc_Std_fprintf( fp, _tag_arr("%s", sizeof(
unsigned char), 3u), _tag_arr( _temp286, sizeof( void*), 1u));}}); Cyc_Marshal_cycputchar('\000',
fp);}}}} goto _LL230;} _LL248: { int sz=( int) Cyc_Typerep_size_type( rep); env=
Cyc_Marshal_write_type_base(( void*)({ struct Cyc_Typerep_Tuple_struct* _temp288=(
struct Cyc_Typerep_Tuple_struct*) _cycalloc( sizeof( struct Cyc_Typerep_Tuple_struct));
_temp288[ 0]=({ struct Cyc_Typerep_Tuple_struct _temp289; _temp289.tag= Cyc_Typerep_Tuple;
_temp289.f1=( unsigned int) sz; _temp289.f2=({ unsigned int _temp290=(
unsigned int) sz; struct _tuple0** _temp291=( struct _tuple0**) _cycalloc(
_check_times( sizeof( struct _tuple0*), _temp290)); struct _tagged_arr _temp296=
_tag_arr( _temp291, sizeof( struct _tuple0*),( unsigned int) sz);{ unsigned int
_temp292= _temp290; unsigned int i; for( i= 0; i <  _temp292; i ++){ _temp291[ i]=({
struct _tuple0* _temp293=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp293->f1= i; _temp293->f2=( void*)({ struct Cyc_Typerep_Int_struct* _temp294=(
struct Cyc_Typerep_Int_struct*) _cycalloc_atomic( sizeof( struct Cyc_Typerep_Int_struct));
_temp294[ 0]=({ struct Cyc_Typerep_Int_struct _temp295; _temp295.tag= Cyc_Typerep_Int;
_temp295.f1= 8; _temp295;}); _temp294;}); _temp293;});}}; _temp296;}); _temp289;});
_temp288;}), env, fp, val); goto _LL230;} _LL250:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp297=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp297[ 0]=({ struct Cyc_Core_Failure_struct
_temp298; _temp298.tag= Cyc_Core_Failure; _temp298.f1= _tag_arr("Unhandled case in write_type_base",
sizeof( unsigned char), 34u); _temp298;}); _temp297;})); _LL230:;} return env;}
void Cyc_Marshal_write_type( void* ts, struct Cyc_Std___sFILE* fp, void* val){
struct _handler_cons _temp299; _push_handler(& _temp299);{ int _temp301= 0; if(
setjmp( _temp299.handler)){ _temp301= 1;} if( ! _temp301){ Cyc_Marshal_write_type_base(
ts, Cyc_Marshal_empty_addr_index(), fp, val);; _pop_handler();} else{ void*
_temp300=( void*) _exn_thrown; void* _temp303= _temp300; struct _tagged_arr
_temp309; _LL305: if(*(( void**) _temp303) ==  Cyc_Core_Failure){ _LL310:
_temp309=(( struct Cyc_Core_Failure_struct*) _temp303)->f1; goto _LL306;} else{
goto _LL307;} _LL307: goto _LL308; _LL306:({ struct Cyc_Std_String_pa_struct
_temp312; _temp312.tag= Cyc_Std_String_pa; _temp312.f1=( struct _tagged_arr)
_temp309;{ void* _temp311[ 1u]={& _temp312}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n", sizeof( unsigned char), 19u), _tag_arr( _temp311,
sizeof( void*), 1u));}});( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp313=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp313[ 0]=({ struct Cyc_Core_Failure_struct _temp314; _temp314.tag= Cyc_Core_Failure;
_temp314.f1= _temp309; _temp314;}); _temp313;})); _LL308:( void) _throw(
_temp303); _LL304:;}}} void Cyc_Marshal_fscanntstring( struct Cyc_Std___sFILE*
fp, struct _tagged_arr s){ int c; int i= 0; while( c= Cyc_Std_fgetc( fp)) { if(
c ==  - 1){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp315=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp315[ 0]=({ struct Cyc_Core_Failure_struct _temp316; _temp316.tag= Cyc_Core_Failure;
_temp316.f1= _tag_arr("Read error", sizeof( unsigned char), 11u); _temp316;});
_temp315;}));}*(( unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i))=( unsigned char) c; i ++;}*(( unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i))='\000';} struct _tuple3
Cyc_Marshal_read_type_base( void* rep, struct _tuple3 env, struct Cyc_Std___sFILE*
fp, void* result){{ void* _temp317= rep; unsigned int _temp339; void* _temp341;
unsigned int _temp343; void* _temp345; struct _tagged_arr _temp347; struct
_tagged_arr _temp349; struct _tagged_arr _temp351; struct _tagged_arr _temp353;
_LL319: if(( unsigned int) _temp317 >  2u?*(( int*) _temp317) ==  Cyc_Typerep_Int:
0){ _LL340: _temp339=(( struct Cyc_Typerep_Int_struct*) _temp317)->f1; goto
_LL320;} else{ goto _LL321;} _LL321: if( _temp317 == ( void*) Cyc_Typerep_Float){
goto _LL322;} else{ goto _LL323;} _LL323: if( _temp317 == ( void*) Cyc_Typerep_Double){
goto _LL324;} else{ goto _LL325;} _LL325: if(( unsigned int) _temp317 >  2u?*((
int*) _temp317) ==  Cyc_Typerep_ThinPtr: 0){ _LL344: _temp343=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp317)->f1; goto _LL342; _LL342: _temp341=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp317)->f2; goto _LL326;} else{ goto _LL327;} _LL327: if(( unsigned int)
_temp317 >  2u?*(( int*) _temp317) ==  Cyc_Typerep_FatPtr: 0){ _LL346: _temp345=(
void*)(( struct Cyc_Typerep_FatPtr_struct*) _temp317)->f1; goto _LL328;} else{
goto _LL329;} _LL329: if(( unsigned int) _temp317 >  2u?*(( int*) _temp317) == 
Cyc_Typerep_Tuple: 0){ _LL348: _temp347=(( struct Cyc_Typerep_Tuple_struct*)
_temp317)->f2; goto _LL330;} else{ goto _LL331;} _LL331: if(( unsigned int)
_temp317 >  2u?*(( int*) _temp317) ==  Cyc_Typerep_TUnion: 0){ _LL350: _temp349=((
struct Cyc_Typerep_TUnion_struct*) _temp317)->f1; goto _LL332;} else{ goto
_LL333;} _LL333: if(( unsigned int) _temp317 >  2u?*(( int*) _temp317) ==  Cyc_Typerep_XTUnion:
0){ _LL352: _temp351=(( struct Cyc_Typerep_XTUnion_struct*) _temp317)->f1; goto
_LL334;} else{ goto _LL335;} _LL335: if(( unsigned int) _temp317 >  2u?*(( int*)
_temp317) ==  Cyc_Typerep_Union: 0){ _LL354: _temp353=(( struct Cyc_Typerep_Union_struct*)
_temp317)->f1; goto _LL336;} else{ goto _LL337;} _LL337: goto _LL338; _LL320:
switch( _temp339){ case 8: _LL355: { unsigned char* x=(( unsigned char*(*)( void*
x)) unsafe_cast)( result);* x= Cyc_Marshal_cycgetchar( fp); break;} case 16:
_LL356: { short* x=(( short*(*)( void* x)) unsafe_cast)( result);* x= Cyc_Marshal_cycgetshort(
fp); break;} case 32: _LL357: { int* x=(( int*(*)( void* x)) unsafe_cast)(
result);* x= Cyc_Marshal_cycgetint( fp); break;} default: _LL358:( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp360=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp360[ 0]=({ struct Cyc_Core_Failure_struct
_temp361; _temp361.tag= Cyc_Core_Failure; _temp361.f1= _tag_arr("read_type_base: illegal int size bits",
sizeof( unsigned char), 38u); _temp361;}); _temp360;}));} goto _LL318; _LL322: {
float* y=(( float*(*)( void* x)) unsafe_cast)( result);* y= Cyc_Marshal_cycgetfloat(
fp); goto _LL318;} _LL324: { double* y=(( double*(*)( void* x)) unsafe_cast)(
result);* y= Cyc_Marshal_cycgetdouble( fp); goto _LL318;} _LL326: { int mem=(
int) Cyc_Marshal_cycgetvarint( fp); if( ! Cyc_Marshal_table_member( env,(
unsigned int) mem)){ int typ_szb=( int) Cyc_Typerep_size_type( _temp341); struct
_tagged_arr buf=({ unsigned int _temp362= _temp343 *  typ_szb; unsigned char*
_temp363=( unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char),
_temp362)); struct _tagged_arr _temp365= _tag_arr( _temp363, sizeof(
unsigned char), _temp343 *  typ_szb);{ unsigned int _temp364= _temp362;
unsigned int i; for( i= 0; i <  _temp364; i ++){ _temp363[ i]='\000';}};
_temp365;}); unsigned int bufptr=( unsigned int)(( unsigned char*) _check_null(
_untag_arr( buf, sizeof( unsigned char), 1u))); unsigned int* x=(( unsigned int*(*)(
void* x)) unsafe_cast)( result);* x= bufptr; env= Cyc_Marshal_table_insert( env,
bufptr);{ int y=( int) bufptr; int i= 0; for( 0; i <  _temp343; i ++){ env= Cyc_Marshal_read_type_base(
_temp341, env, fp,( void*)(( void*(*)( int x)) unsafe_cast)( y)); y += typ_szb;}}}
else{ unsigned int* x=(( unsigned int*(*)( void* x)) unsafe_cast)( result);* x=
Cyc_Marshal_table_lookup( env, mem);} goto _LL318;} _LL328: { int mem=( int) Cyc_Marshal_cycgetvarint(
fp); int pos=( int) Cyc_Marshal_cycgetvarint( fp); int sz=( int) Cyc_Marshal_cycgetvarint(
fp); int typ_szb=( int) Cyc_Typerep_size_type( _temp345); if( ! Cyc_Marshal_table_member(
env,( unsigned int) mem)){ struct _tagged_arr buf=({ unsigned int _temp366=(
unsigned int)( sz *  typ_szb); unsigned char* _temp367=( unsigned char*)
_cycalloc_atomic( _check_times( sizeof( unsigned char), _temp366)); struct
_tagged_arr _temp369= _tag_arr( _temp367, sizeof( unsigned char),( unsigned int)(
sz *  typ_szb));{ unsigned int _temp368= _temp366; unsigned int i; for( i= 0; i
<  _temp368; i ++){ _temp367[ i]='\000';}}; _temp369;}); unsigned int bufptr=(
unsigned int)(( unsigned char*) _check_null( _untag_arr( buf, sizeof(
unsigned char), 1u))); struct _tuple5* x=(( struct _tuple5*(*)( void* x))
unsafe_cast)( result);(* x).f1= bufptr;(* x).f2= bufptr +  pos *  typ_szb;(* x).f3=
bufptr +  sz *  typ_szb; env= Cyc_Marshal_table_insert( env, bufptr);{ int y=(
int) bufptr; int i= 0; for( 0; i <  sz; i ++){ env= Cyc_Marshal_read_type_base(
_temp345, env, fp,( void*)(( void*(*)( int x)) unsafe_cast)( y)); y += typ_szb;}}}
else{ struct _tuple5* x=(( struct _tuple5*(*)( void* x)) unsafe_cast)( result);(*
x).f1= Cyc_Marshal_table_lookup( env, mem);(* x).f2=(* x).f1 +  typ_szb *  pos;(*
x).f3=(* x).f1 +  typ_szb *  sz;} goto _LL318;} _LL330: { unsigned int x=((
unsigned int(*)( void* x)) unsafe_cast)( result);{ int i= 0; for( 0; i < 
_get_arr_size( _temp347, sizeof( struct _tuple0*)); i ++){ void* _temp372;
unsigned int _temp374; struct _tuple0 _temp370=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp347, sizeof( struct _tuple0*), i))); _LL375:
_temp374= _temp370.f1; goto _LL373; _LL373: _temp372= _temp370.f2; goto _LL371;
_LL371: { int x_ofs=( int)( x +  _temp374); env= Cyc_Marshal_read_type_base(
_temp372, env, fp,( void*)(( void*(*)( int x)) unsafe_cast)( x_ofs));}}} goto
_LL318;} _LL332: { int tagmem=( int) Cyc_Marshal_cycgetvarint( fp); if( tagmem < 
1024){ int tag= tagmem; unsigned int* x=(( unsigned int*(*)( void* x))
unsafe_cast)( result);* x=( unsigned int) tag;} else{ int mem= tagmem -  1024;
if( ! Cyc_Marshal_table_member( env,( unsigned int) mem)){ int tag=( int) Cyc_Marshal_cycgetvarint(
fp); void* typ= Cyc_Typerep_get_unionbranch(( unsigned int) tag, _temp349);
struct _tagged_arr buf=({ unsigned int _temp376= Cyc_Typerep_size_type( typ);
unsigned char* _temp377=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp376)); struct _tagged_arr _temp379= _tag_arr(
_temp377, sizeof( unsigned char), Cyc_Typerep_size_type( typ));{ unsigned int
_temp378= _temp376; unsigned int i; for( i= 0; i <  _temp378; i ++){ _temp377[ i]='\000';}};
_temp379;}); unsigned int bufptr=( unsigned int)(( unsigned char*) _check_null(
_untag_arr( buf, sizeof( unsigned char), 1u))); env= Cyc_Marshal_table_insert(
env, bufptr);{ unsigned int* x=(( unsigned int*(*)( void* x)) unsafe_cast)(
result);* x= bufptr; env= Cyc_Marshal_read_type_base( typ, env, fp,( void*)((
void*(*)( unsigned int x)) unsafe_cast)( bufptr));}} else{ unsigned int* x=((
unsigned int*(*)( void* x)) unsafe_cast)( result);* x= Cyc_Marshal_table_lookup(
env, mem);}} goto _LL318;} _LL334: { int mem=( int) Cyc_Marshal_cycgetvarint( fp);
if( ! Cyc_Marshal_table_member( env,( unsigned int) mem)){ unsigned char nonnull=(
unsigned char) Cyc_Marshal_cycgetchar( fp); struct _tagged_arr s= _tag_arr(({
unsigned int _temp384=( unsigned int) 100; unsigned char* _temp385=(
unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char), _temp384));{
unsigned int _temp386= _temp384; unsigned int i; for( i= 0; i <  _temp386; i ++){
_temp385[ i]='\000';}}; _temp385;}), sizeof( unsigned char),( unsigned int) 100);
struct _tagged_arr xtname= _tagged_arr_plus( s, sizeof( unsigned char), 4); Cyc_Marshal_fscanntstring(
fp, xtname); if(( int) nonnull){ void* typ= Cyc_Typerep_get_xtunionbranch((
struct _tagged_arr) xtname, _temp351); struct _tagged_arr buf=({ unsigned int
_temp380= Cyc_Typerep_size_type( typ); unsigned char* _temp381=( unsigned char*)
_cycalloc_atomic( _check_times( sizeof( unsigned char), _temp380)); struct
_tagged_arr _temp383= _tag_arr( _temp381, sizeof( unsigned char), Cyc_Typerep_size_type(
typ));{ unsigned int _temp382= _temp380; unsigned int i; for( i= 0; i < 
_temp382; i ++){ _temp381[ i]='\000';}}; _temp383;}); unsigned int bufptr=(
unsigned int)(( unsigned char*) _check_null( _untag_arr( buf, sizeof(
unsigned char), 1u))); env= Cyc_Marshal_table_insert( env, bufptr);{
unsigned int* x=(( unsigned int*(*)( void* x)) unsafe_cast)( result);* x= bufptr;
env= Cyc_Marshal_read_type_base( typ, env, fp,( void*)(( void*(*)( unsigned int
x)) unsafe_cast)( bufptr));{ unsigned int* tagptr=(( unsigned int*(*)(
unsigned int x)) unsafe_cast)( bufptr);* tagptr=( unsigned int)(( unsigned char*)
_check_null( _untag_arr( s, sizeof( unsigned char), 1u)));}}} else{ unsigned int*
x=(( unsigned int*(*)( void* x)) unsafe_cast)( result);* x=( unsigned int)((
unsigned char*) _check_null( _untag_arr( s, sizeof( unsigned char), 1u))); env=
Cyc_Marshal_table_insert( env,* x);}} else{ unsigned int* x=(( unsigned int*(*)(
void* x)) unsafe_cast)( result);* x= Cyc_Marshal_table_lookup( env, mem);} goto
_LL318;} _LL336: { int sz=( int) Cyc_Typerep_size_type( rep); env= Cyc_Marshal_read_type_base((
void*)({ struct Cyc_Typerep_Tuple_struct* _temp387=( struct Cyc_Typerep_Tuple_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Tuple_struct)); _temp387[ 0]=({ struct Cyc_Typerep_Tuple_struct
_temp388; _temp388.tag= Cyc_Typerep_Tuple; _temp388.f1=( unsigned int) sz;
_temp388.f2=({ unsigned int _temp389=( unsigned int) sz; struct _tuple0**
_temp390=( struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*),
_temp389)); struct _tagged_arr _temp395= _tag_arr( _temp390, sizeof( struct
_tuple0*),( unsigned int) sz);{ unsigned int _temp391= _temp389; unsigned int i;
for( i= 0; i <  _temp391; i ++){ _temp390[ i]=({ struct _tuple0* _temp392=(
struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp392->f1= i; _temp392->f2=(
void*)({ struct Cyc_Typerep_Int_struct* _temp393=( struct Cyc_Typerep_Int_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Typerep_Int_struct)); _temp393[ 0]=({
struct Cyc_Typerep_Int_struct _temp394; _temp394.tag= Cyc_Typerep_Int; _temp394.f1=
8; _temp394;}); _temp393;}); _temp392;});}}; _temp395;}); _temp388;}); _temp387;}),
env, fp, result); goto _LL318;} _LL338:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp396=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp396[ 0]=({ struct Cyc_Core_Failure_struct _temp397; _temp397.tag= Cyc_Core_Failure;
_temp397.f1= _tag_arr("Unhandled case in read_type_base", sizeof( unsigned char),
33u); _temp397;}); _temp396;})); _LL318:;} return env;} void* Cyc_Marshal_read_type(
void* ts, struct Cyc_Std___sFILE* fp){ struct _handler_cons _temp398;
_push_handler(& _temp398);{ int _temp400= 0; if( setjmp( _temp398.handler)){
_temp400= 1;} if( ! _temp400){{ struct _tagged_arr buf=({ unsigned int _temp402=
Cyc_Typerep_size_type( ts); unsigned char* _temp403=( unsigned char*)
_cycalloc_atomic( _check_times( sizeof( unsigned char), _temp402)); struct
_tagged_arr _temp405= _tag_arr( _temp403, sizeof( unsigned char), Cyc_Typerep_size_type(
ts));{ unsigned int _temp404= _temp402; unsigned int i; for( i= 0; i <  _temp404;
i ++){ _temp403[ i]='\000';}}; _temp405;}); void* result=(( void*(*)(
unsigned char* x)) unsafe_cast)(( unsigned char*) _check_null( _untag_arr( buf,
sizeof( unsigned char), 1u))); Cyc_Marshal_read_type_base( ts, Cyc_Marshal_empty_addr_table(),
fp, result);{ void* _temp401= result; _npop_handler( 0u); return _temp401;}};
_pop_handler();} else{ void* _temp399=( void*) _exn_thrown; void* _temp407=
_temp399; struct _tagged_arr _temp413; _LL409: if(*(( void**) _temp407) ==  Cyc_Core_Failure){
_LL414: _temp413=(( struct Cyc_Core_Failure_struct*) _temp407)->f1; goto _LL410;}
else{ goto _LL411;} _LL411: goto _LL412; _LL410:({ struct Cyc_Std_String_pa_struct
_temp416; _temp416.tag= Cyc_Std_String_pa; _temp416.f1=( struct _tagged_arr)
_temp413;{ void* _temp415[ 1u]={& _temp416}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n", sizeof( unsigned char), 19u), _tag_arr( _temp415,
sizeof( void*), 1u));}});( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp417=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp417[ 0]=({ struct Cyc_Core_Failure_struct _temp418; _temp418.tag= Cyc_Core_Failure;
_temp418.f1= _temp413; _temp418;}); _temp417;})); _LL412:( void) _throw(
_temp407); _LL408:;}}}
