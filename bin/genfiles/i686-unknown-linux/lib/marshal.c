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
unsigned int __size, unsigned int __n, struct Cyc_Std___sFILE* __s); extern int
Cyc_Std_getw( struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_putw( int
__w, struct Cyc_Std___sFILE* __stream); extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
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
return(( struct Cyc_Set_Set*(*)( int(* cmp)( unsigned int, unsigned int))) Cyc_Set_empty)(
Cyc_Marshal_uint_cmp);} struct _tuple4{ int f1; struct Cyc_Set_Set* f2; } ;
struct _tuple4* Cyc_Marshal_member_insert( struct Cyc_Set_Set* set, unsigned int
val){ return({ struct _tuple4* _temp0=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp0->f1=(( int(*)( struct Cyc_Set_Set* s, unsigned int elt))
Cyc_Set_member)( set, val); _temp0->f2=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, unsigned int elt)) Cyc_Set_insert)( set, val); _temp0;});} struct _tuple2 Cyc_Marshal_empty_addr_index(){
return({ struct _tuple2 _temp1; _temp1.f1=(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
unsigned int, unsigned int))) Cyc_Dict_empty)( Cyc_Marshal_uint_cmp); _temp1.f2=
0; _temp1;});} struct _tuple5{ int f1; struct _tuple2 f2; } ; struct _tuple5*
Cyc_Marshal_lookup_insert( struct _tuple2 index, unsigned int val){ int _temp4;
struct Cyc_Dict_Dict* _temp6; struct _tuple2 _temp2= index; _LL7: _temp6= _temp2.f1;
goto _LL5; _LL5: _temp4= _temp2.f2; goto _LL3; _LL3: { struct Cyc_Core_Opt*
_temp8=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, unsigned int k)) Cyc_Dict_lookup_opt)(
_temp6, val); if( _temp8 !=  0){ return({ struct _tuple5* _temp9=( struct
_tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp9->f1=( int) _temp8->v;
_temp9->f2= index; _temp9;});} else{ return({ struct _tuple5* _temp10=( struct
_tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp10->f1= - 1; _temp10->f2=({
struct _tuple2 _temp11; _temp11.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, unsigned int k, int v)) Cyc_Dict_insert)( _temp6, val, _temp4); _temp11.f2=
_temp4 +  1; _temp11;}); _temp10;});}}} struct _tuple3 Cyc_Marshal_empty_addr_table(){
return({ struct _tuple3 _temp12; _temp12.f1= _tag_arr( 0u, 0u, 0u); _temp12.f2=
0; _temp12;});} struct _tuple3 Cyc_Marshal_table_insert( struct _tuple3 tbl,
unsigned int i){ int _temp15; struct _tagged_arr _temp17; struct _tuple3 _temp13=
tbl; _LL18: _temp17= _temp13.f1; goto _LL16; _LL16: _temp15= _temp13.f2; goto
_LL14; _LL14: if( _temp17.curr == ( _tag_arr( 0u, 0u, 0u)).curr){ _temp17=
_tag_arr(({ unsigned int _temp19=( unsigned int) 1; unsigned int* _temp20=(
unsigned int*) _cycalloc_atomic( _check_times( sizeof( unsigned int), _temp19));{
unsigned int _temp21= _temp19; unsigned int j; for( j= 0; j <  _temp21; j ++){
_temp20[ j]= i;}}; _temp20;}), sizeof( unsigned int),( unsigned int) 1); return({
struct _tuple3 _temp22; _temp22.f1= _temp17; _temp22.f2= 1; _temp22;});} else{
if( _get_arr_size( _temp17, sizeof( unsigned int)) ==  _temp15){ _temp17=({
unsigned int _temp23=( unsigned int)( 2 *  _temp15); unsigned int* _temp24=(
unsigned int*) _cycalloc_atomic( _check_times( sizeof( unsigned int), _temp23));
struct _tagged_arr _temp26= _tag_arr( _temp24, sizeof( unsigned int),(
unsigned int)( 2 *  _temp15));{ unsigned int _temp25= _temp23; unsigned int j;
for( j= 0; j <  _temp25; j ++){ _temp24[ j]= j <  _temp15?*(( unsigned int*)
_check_unknown_subscript( _temp17, sizeof( unsigned int),( int) j)): 0;}};
_temp26;});*(( unsigned int*) _check_unknown_subscript( _temp17, sizeof(
unsigned int), _temp15))= i; return({ struct _tuple3 _temp27; _temp27.f1=
_temp17; _temp27.f2= _temp15 +  1; _temp27;});} else{*(( unsigned int*)
_check_unknown_subscript( _temp17, sizeof( unsigned int), _temp15))= i; return({
struct _tuple3 _temp28; _temp28.f1= _temp17; _temp28.f2= _temp15 +  1; _temp28;});}}}
unsigned int Cyc_Marshal_table_lookup( struct _tuple3 tbl, int i){ return*((
unsigned int*) _check_unknown_subscript( tbl.f1, sizeof( unsigned int), i));}
struct _tuple6{ unsigned int f1; unsigned int f2; unsigned int f3; } ; struct
Cyc_Set_Set* Cyc_Marshal_print_type_base( void* rep, struct Cyc_Set_Set* env,
void* val){{ void* _temp29= rep; unsigned int _temp51; void* _temp53;
unsigned int _temp55; void* _temp57; struct _tagged_arr _temp59; struct
_tagged_arr _temp61; struct _tagged_arr _temp63; struct _tagged_arr _temp65;
_LL31: if(( unsigned int) _temp29 >  2u?*(( int*) _temp29) ==  Cyc_Typerep_Int:
0){ _LL52: _temp51=(( struct Cyc_Typerep_Int_struct*) _temp29)->f1; goto _LL32;}
else{ goto _LL33;} _LL33: if( _temp29 == ( void*) Cyc_Typerep_Float){ goto _LL34;}
else{ goto _LL35;} _LL35: if( _temp29 == ( void*) Cyc_Typerep_Double){ goto
_LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp29 >  2u?*(( int*)
_temp29) ==  Cyc_Typerep_ThinPtr: 0){ _LL56: _temp55=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp29)->f1; goto _LL54; _LL54: _temp53=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp29)->f2; goto _LL38;} else{ goto _LL39;} _LL39: if(( unsigned int) _temp29
>  2u?*(( int*) _temp29) ==  Cyc_Typerep_FatPtr: 0){ _LL58: _temp57=( void*)((
struct Cyc_Typerep_FatPtr_struct*) _temp29)->f1; goto _LL40;} else{ goto _LL41;}
_LL41: if(( unsigned int) _temp29 >  2u?*(( int*) _temp29) ==  Cyc_Typerep_Tuple:
0){ _LL60: _temp59=(( struct Cyc_Typerep_Tuple_struct*) _temp29)->f2; goto _LL42;}
else{ goto _LL43;} _LL43: if(( unsigned int) _temp29 >  2u?*(( int*) _temp29) == 
Cyc_Typerep_TUnion: 0){ _LL62: _temp61=(( struct Cyc_Typerep_TUnion_struct*)
_temp29)->f1; goto _LL44;} else{ goto _LL45;} _LL45: if(( unsigned int) _temp29
>  2u?*(( int*) _temp29) ==  Cyc_Typerep_XTUnion: 0){ _LL64: _temp63=(( struct
Cyc_Typerep_XTUnion_struct*) _temp29)->f1; goto _LL46;} else{ goto _LL47;} _LL47:
if(( unsigned int) _temp29 >  2u?*(( int*) _temp29) ==  Cyc_Typerep_Union: 0){
_LL66: _temp65=(( struct Cyc_Typerep_Union_struct*) _temp29)->f1; goto _LL48;}
else{ goto _LL49;} _LL49: goto _LL50; _LL32: switch( _temp51){ case 8: _LL67: {
unsigned char* x=(( unsigned char*(*)( void* x)) unsafe_cast)( val); if( isprint((
int)* x)){({ struct Cyc_Std_Int_pa_struct _temp70; _temp70.tag= Cyc_Std_Int_pa;
_temp70.f1=( int)(( unsigned int)(( int)* x));{ void* _temp69[ 1u]={& _temp70};
Cyc_Std_printf( _tag_arr("'%c'", sizeof( unsigned char), 5u), _tag_arr( _temp69,
sizeof( void*), 1u));}});} else{({ struct Cyc_Std_Int_pa_struct _temp72; _temp72.tag=
Cyc_Std_Int_pa; _temp72.f1=( int)(( unsigned int)(( int)* x));{ void* _temp71[ 1u]={&
_temp72}; Cyc_Std_printf( _tag_arr("'\\%d'", sizeof( unsigned char), 6u),
_tag_arr( _temp71, sizeof( void*), 1u));}});} break;} case 16: _LL68: { short* x=((
short*(*)( void* x)) unsafe_cast)( val);({ struct Cyc_Std_Int_pa_struct _temp75;
_temp75.tag= Cyc_Std_Int_pa; _temp75.f1=( int)(( unsigned int)(( int)* x));{
void* _temp74[ 1u]={& _temp75}; Cyc_Std_printf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp74, sizeof( void*), 1u));}}); break;} case
32: _LL73: { int* x=(( int*(*)( void* x)) unsafe_cast)( val);({ struct Cyc_Std_Int_pa_struct
_temp78; _temp78.tag= Cyc_Std_Int_pa; _temp78.f1=( int)(( unsigned int)* x);{
void* _temp77[ 1u]={& _temp78}; Cyc_Std_printf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp77, sizeof( void*), 1u));}}); break;}
default: _LL76:({ struct Cyc_Std_Int_pa_struct _temp81; _temp81.tag= Cyc_Std_Int_pa;
_temp81.f1=( int) _temp51;{ void* _temp80[ 1u]={& _temp81}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("print_type_base: illegal int size %d bits\n", sizeof( unsigned char),
43u), _tag_arr( _temp80, sizeof( void*), 1u));}}); exit( 1);} goto _LL30; _LL34: {
float* x=(( float*(*)( void* x)) unsafe_cast)( val);({ struct Cyc_Std_Double_pa_struct
_temp83; _temp83.tag= Cyc_Std_Double_pa; _temp83.f1=( double)* x;{ void* _temp82[
1u]={& _temp83}; Cyc_Std_printf( _tag_arr("%f", sizeof( unsigned char), 3u),
_tag_arr( _temp82, sizeof( void*), 1u));}}); goto _LL30;} _LL36: { double* x=((
double*(*)( void* x)) unsafe_cast)( val);({ struct Cyc_Std_Double_pa_struct
_temp85; _temp85.tag= Cyc_Std_Double_pa; _temp85.f1=* x;{ void* _temp84[ 1u]={&
_temp85}; Cyc_Std_printf( _tag_arr("%f", sizeof( unsigned char), 3u), _tag_arr(
_temp84, sizeof( void*), 1u));}}); goto _LL30;} _LL38: { int* x=(( int*(*)( void*
x)) unsafe_cast)( val); int y=*(( int*) _check_null( x)); struct _tuple4 _temp88;
struct Cyc_Set_Set* _temp89; int _temp91; struct _tuple4* _temp86= Cyc_Marshal_member_insert(
env,( unsigned int) y); _temp88=* _temp86; _LL92: _temp91= _temp88.f1; goto
_LL90; _LL90: _temp89= _temp88.f2; goto _LL87; _LL87: env= _temp89; if( !
_temp91){({ struct Cyc_Std_Int_pa_struct _temp94; _temp94.tag= Cyc_Std_Int_pa;
_temp94.f1=( unsigned int) y;{ void* _temp93[ 1u]={& _temp94}; Cyc_Std_printf(
_tag_arr("{(@%x) ", sizeof( unsigned char), 8u), _tag_arr( _temp93, sizeof( void*),
1u));}}); if( y !=  0){ int typ_szb=( int) Cyc_Typerep_size_type( _temp53); int
i= 0; for( 0; i <  _temp55; i ++){ env= Cyc_Marshal_print_type_base( _temp53,
env,( void*)(( void*(*)( int x)) unsafe_cast)( y)); if( i !=  _temp55 -  1){({
void* _temp95[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof( unsigned char), 3u),
_tag_arr( _temp95, sizeof( void*), 0u));});} y += typ_szb;}} else{({ void*
_temp96[ 0u]={}; Cyc_Std_printf( _tag_arr("NULL", sizeof( unsigned char), 5u),
_tag_arr( _temp96, sizeof( void*), 0u));});}({ void* _temp97[ 0u]={}; Cyc_Std_printf(
_tag_arr(" }", sizeof( unsigned char), 3u), _tag_arr( _temp97, sizeof( void*), 0u));});}
else{({ struct Cyc_Std_Int_pa_struct _temp99; _temp99.tag= Cyc_Std_Int_pa;
_temp99.f1=( unsigned int) y;{ void* _temp98[ 1u]={& _temp99}; Cyc_Std_printf(
_tag_arr("(#%x)", sizeof( unsigned char), 6u), _tag_arr( _temp98, sizeof( void*),
1u));}});} goto _LL30;} _LL40: { struct _tuple6* x=(( struct _tuple6*(*)( void*
x)) unsafe_cast)( val); unsigned int typ_szb= Cyc_Typerep_size_type( _temp57);
unsigned int base=(* x).f1; unsigned int curr=(* x).f2; unsigned int
last_plus_one=(* x).f3; struct _tuple4 _temp102; struct Cyc_Set_Set* _temp103;
int _temp105; struct _tuple4* _temp100= Cyc_Marshal_member_insert( env, base);
_temp102=* _temp100; _LL106: _temp105= _temp102.f1; goto _LL104; _LL104:
_temp103= _temp102.f2; goto _LL101; _LL101: env= _temp103; if( ! _temp105){({
struct Cyc_Std_Int_pa_struct _temp108; _temp108.tag= Cyc_Std_Int_pa; _temp108.f1=
base;{ void* _temp107[ 1u]={& _temp108}; Cyc_Std_printf( _tag_arr("[(@%x) ",
sizeof( unsigned char), 8u), _tag_arr( _temp107, sizeof( void*), 1u));}});{
unsigned int i= base; for( 0; i <  last_plus_one; i += typ_szb){ env= Cyc_Marshal_print_type_base(
_temp57, env,( void*)(( void*(*)( unsigned int x)) unsafe_cast)( i)); if( i + 
typ_szb <  last_plus_one){({ void* _temp109[ 0u]={}; Cyc_Std_printf( _tag_arr(", ",
sizeof( unsigned char), 3u), _tag_arr( _temp109, sizeof( void*), 0u));});}}}({
void* _temp110[ 0u]={}; Cyc_Std_printf( _tag_arr(" ]", sizeof( unsigned char), 3u),
_tag_arr( _temp110, sizeof( void*), 0u));});} else{({ struct Cyc_Std_Int_pa_struct
_temp112; _temp112.tag= Cyc_Std_Int_pa; _temp112.f1= base;{ void* _temp111[ 1u]={&
_temp112}; Cyc_Std_printf( _tag_arr("(#%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp111, sizeof( void*), 1u));}});} goto _LL30;} _LL42:({ void*
_temp113[ 0u]={}; Cyc_Std_printf( _tag_arr("$( ", sizeof( unsigned char), 4u),
_tag_arr( _temp113, sizeof( void*), 0u));});{ int x=(( int(*)( void* x))
unsafe_cast)( val);{ int i= 0; for( 0; i <  _get_arr_size( _temp59, sizeof(
struct _tuple0*)); i ++){ void* _temp116; unsigned int _temp118; struct _tuple0
_temp114=*(*(( struct _tuple0**) _check_unknown_subscript( _temp59, sizeof(
struct _tuple0*), i))); _LL119: _temp118= _temp114.f1; goto _LL117; _LL117:
_temp116= _temp114.f2; goto _LL115; _LL115: { int x_ofs=( int)( x +  _temp118);
env= Cyc_Marshal_print_type_base( _temp116, env,( void*)(( void*(*)( int x))
unsafe_cast)( x_ofs)); if( i !=  _get_arr_size( _temp59, sizeof( struct _tuple0*))
-  1){({ void* _temp120[ 0u]={}; Cyc_Std_printf( _tag_arr(", ", sizeof(
unsigned char), 3u), _tag_arr( _temp120, sizeof( void*), 0u));});}}}}({ void*
_temp121[ 0u]={}; Cyc_Std_printf( _tag_arr(" )", sizeof( unsigned char), 3u),
_tag_arr( _temp121, sizeof( void*), 0u));}); goto _LL30;} _LL44:({ void*
_temp122[ 0u]={}; Cyc_Std_printf( _tag_arr("&[( ", sizeof( unsigned char), 5u),
_tag_arr( _temp122, sizeof( void*), 0u));});{ unsigned int* tagp=(( unsigned int*(*)(
void* x)) unsafe_cast)( val); if(* tagp <  1024){({ struct Cyc_Std_Int_pa_struct
_temp124; _temp124.tag= Cyc_Std_Int_pa; _temp124.f1=( int)* tagp;{ void*
_temp123[ 1u]={& _temp124}; Cyc_Std_printf( _tag_arr("%d", sizeof( unsigned char),
3u), _tag_arr( _temp123, sizeof( void*), 1u));}});} else{ struct _tuple4
_temp127; struct Cyc_Set_Set* _temp128; int _temp130; struct _tuple4* _temp125=
Cyc_Marshal_member_insert( env,* tagp); _temp127=* _temp125; _LL131: _temp130=
_temp127.f1; goto _LL129; _LL129: _temp128= _temp127.f2; goto _LL126; _LL126:
env= _temp128; if( ! _temp130){({ struct Cyc_Std_Int_pa_struct _temp133;
_temp133.tag= Cyc_Std_Int_pa; _temp133.f1=* tagp;{ void* _temp132[ 1u]={&
_temp133}; Cyc_Std_printf( _tag_arr("(@%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp132, sizeof( void*), 1u));}});{ unsigned int* x=(( unsigned int*(*)(
unsigned int x)) unsafe_cast)(* tagp); unsigned int tag=* x; void* typ= Cyc_Typerep_get_unionbranch(
tag, _temp61); env=(( struct Cyc_Set_Set*(*)( void* rep, struct Cyc_Set_Set* env,
unsigned int* val)) Cyc_Marshal_print_type_base)( typ, env, x);}} else{({ struct
Cyc_Std_Int_pa_struct _temp135; _temp135.tag= Cyc_Std_Int_pa; _temp135.f1=* tagp;{
void* _temp134[ 1u]={& _temp135}; Cyc_Std_printf( _tag_arr("(#%x)", sizeof(
unsigned char), 6u), _tag_arr( _temp134, sizeof( void*), 1u));}});}}({ void*
_temp136[ 0u]={}; Cyc_Std_printf( _tag_arr(" )]", sizeof( unsigned char), 4u),
_tag_arr( _temp136, sizeof( void*), 0u));}); goto _LL30;} _LL46: { unsigned int*
xtunionp=(( unsigned int*(*)( void* x)) unsafe_cast)( val); struct _tuple4
_temp139; struct Cyc_Set_Set* _temp140; int _temp142; struct _tuple4* _temp137=
Cyc_Marshal_member_insert( env,* xtunionp); _temp139=* _temp137; _LL143:
_temp142= _temp139.f1; goto _LL141; _LL141: _temp140= _temp139.f2; goto _LL138;
_LL138: env= _temp140; if( ! _temp142){({ struct Cyc_Std_Int_pa_struct _temp145;
_temp145.tag= Cyc_Std_Int_pa; _temp145.f1=* xtunionp;{ void* _temp144[ 1u]={&
_temp145}; Cyc_Std_printf( _tag_arr("(@%x)", sizeof( unsigned char), 6u),
_tag_arr( _temp144, sizeof( void*), 1u));}});{ unsigned int* xtstructp=((
unsigned int*(*)( unsigned int x)) unsafe_cast)(* xtunionp); if(* xtstructp != 
0){ unsigned char* xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(*
xtstructp +  4); struct _tagged_arr xtname=( struct _tagged_arr)
Cstring_to_string( xtnamec);({ struct Cyc_Std_String_pa_struct _temp147;
_temp147.tag= Cyc_Std_String_pa; _temp147.f1=( struct _tagged_arr) xtname;{ void*
_temp146[ 1u]={& _temp147}; Cyc_Std_printf( _tag_arr("&%s[( ", sizeof(
unsigned char), 7u), _tag_arr( _temp146, sizeof( void*), 1u));}});{ void* typ=
Cyc_Typerep_get_xtunionbranch( xtname, _temp63); env=(( struct Cyc_Set_Set*(*)(
void* rep, struct Cyc_Set_Set* env, unsigned int* val)) Cyc_Marshal_print_type_base)(
typ, env,( unsigned int*) _check_null( xtstructp));}} else{ unsigned char*
xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(* xtunionp +  4);
struct _tagged_arr xtname=( struct _tagged_arr) Cstring_to_string( xtnamec);({
struct Cyc_Std_String_pa_struct _temp149; _temp149.tag= Cyc_Std_String_pa;
_temp149.f1=( struct _tagged_arr) xtname;{ void* _temp148[ 1u]={& _temp149}; Cyc_Std_printf(
_tag_arr("&%s[( ", sizeof( unsigned char), 7u), _tag_arr( _temp148, sizeof( void*),
1u));}});}}} else{({ struct Cyc_Std_Int_pa_struct _temp151; _temp151.tag= Cyc_Std_Int_pa;
_temp151.f1=* xtunionp;{ void* _temp150[ 1u]={& _temp151}; Cyc_Std_printf(
_tag_arr("(#%x)", sizeof( unsigned char), 6u), _tag_arr( _temp150, sizeof( void*),
1u));}});}({ void* _temp152[ 0u]={}; Cyc_Std_printf( _tag_arr(" )]", sizeof(
unsigned char), 4u), _tag_arr( _temp152, sizeof( void*), 0u));}); goto _LL30;}
_LL48:({ void* _temp153[ 0u]={}; Cyc_Std_printf( _tag_arr("[[", sizeof(
unsigned char), 3u), _tag_arr( _temp153, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp65, sizeof( void**)); i ++){ Cyc_Marshal_print_type_base(*(*((
void***) _check_unknown_subscript( _temp65, sizeof( void**), i))), env, val);
if( i <  _get_arr_size( _temp65, sizeof( void**)) -  1){({ void* _temp154[ 0u]={};
Cyc_Std_printf( _tag_arr(" | ", sizeof( unsigned char), 4u), _tag_arr( _temp154,
sizeof( void*), 0u));});}}}({ void* _temp155[ 0u]={}; Cyc_Std_printf( _tag_arr("]]",
sizeof( unsigned char), 3u), _tag_arr( _temp155, sizeof( void*), 0u));}); goto
_LL30; _LL50:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp156=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp156[ 0]=({ struct Cyc_Core_Failure_struct _temp157; _temp157.tag= Cyc_Core_Failure;
_temp157.f1= _tag_arr("Unhandled case in print_type_base", sizeof( unsigned char),
34u); _temp157;}); _temp156;})); _LL30:;} return env;} void Cyc_Marshal_print_type(
void* ts, void* val){ struct _handler_cons _temp158; _push_handler(& _temp158);{
int _temp160= 0; if( setjmp( _temp158.handler)){ _temp160= 1;} if( ! _temp160){
Cyc_Marshal_print_type_base( ts, Cyc_Marshal_empty_addr_set(), val);;
_pop_handler();} else{ void* _temp159=( void*) _exn_thrown; void* _temp162=
_temp159; struct _tagged_arr _temp168; _LL164: if(*(( void**) _temp162) ==  Cyc_Core_Failure){
_LL169: _temp168=(( struct Cyc_Core_Failure_struct*) _temp162)->f1; goto _LL165;}
else{ goto _LL166;} _LL166: goto _LL167; _LL165:({ struct Cyc_Std_String_pa_struct
_temp171; _temp171.tag= Cyc_Std_String_pa; _temp171.f1=( struct _tagged_arr)
_temp168;{ void* _temp170[ 1u]={& _temp171}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n", sizeof( unsigned char), 19u), _tag_arr( _temp170,
sizeof( void*), 1u));}});( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp172=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp172[ 0]=({ struct Cyc_Core_Failure_struct _temp173; _temp173.tag= Cyc_Core_Failure;
_temp173.f1= _temp168; _temp173;}); _temp172;})); _LL167:( void) _throw(
_temp162); _LL163:;}}} struct _tuple2 Cyc_Marshal_write_type_base( void* rep,
struct _tuple2 env, struct Cyc_Std___sFILE* fp, void* val){{ void* _temp174= rep;
unsigned int _temp196; void* _temp198; unsigned int _temp200; void* _temp202;
struct _tagged_arr _temp204; struct _tagged_arr _temp206; struct _tagged_arr
_temp208; struct _tagged_arr _temp210; _LL176: if(( unsigned int) _temp174 >  2u?*((
int*) _temp174) ==  Cyc_Typerep_Int: 0){ _LL197: _temp196=(( struct Cyc_Typerep_Int_struct*)
_temp174)->f1; goto _LL177;} else{ goto _LL178;} _LL178: if( _temp174 == ( void*)
Cyc_Typerep_Float){ goto _LL179;} else{ goto _LL180;} _LL180: if( _temp174 == (
void*) Cyc_Typerep_Double){ goto _LL181;} else{ goto _LL182;} _LL182: if((
unsigned int) _temp174 >  2u?*(( int*) _temp174) ==  Cyc_Typerep_ThinPtr: 0){
_LL201: _temp200=(( struct Cyc_Typerep_ThinPtr_struct*) _temp174)->f1; goto
_LL199; _LL199: _temp198=( void*)(( struct Cyc_Typerep_ThinPtr_struct*) _temp174)->f2;
goto _LL183;} else{ goto _LL184;} _LL184: if(( unsigned int) _temp174 >  2u?*((
int*) _temp174) ==  Cyc_Typerep_FatPtr: 0){ _LL203: _temp202=( void*)(( struct
Cyc_Typerep_FatPtr_struct*) _temp174)->f1; goto _LL185;} else{ goto _LL186;}
_LL186: if(( unsigned int) _temp174 >  2u?*(( int*) _temp174) ==  Cyc_Typerep_Tuple:
0){ _LL205: _temp204=(( struct Cyc_Typerep_Tuple_struct*) _temp174)->f2; goto
_LL187;} else{ goto _LL188;} _LL188: if(( unsigned int) _temp174 >  2u?*(( int*)
_temp174) ==  Cyc_Typerep_TUnion: 0){ _LL207: _temp206=(( struct Cyc_Typerep_TUnion_struct*)
_temp174)->f1; goto _LL189;} else{ goto _LL190;} _LL190: if(( unsigned int)
_temp174 >  2u?*(( int*) _temp174) ==  Cyc_Typerep_XTUnion: 0){ _LL209: _temp208=((
struct Cyc_Typerep_XTUnion_struct*) _temp174)->f1; goto _LL191;} else{ goto
_LL192;} _LL192: if(( unsigned int) _temp174 >  2u?*(( int*) _temp174) ==  Cyc_Typerep_Union:
0){ _LL211: _temp210=(( struct Cyc_Typerep_Union_struct*) _temp174)->f1; goto
_LL193;} else{ goto _LL194;} _LL194: goto _LL195; _LL177: switch( _temp196){
case 8: _LL212: { unsigned char* x=(( unsigned char*(*)( void* x)) unsafe_cast)(
val); Cyc_Std_fputc(( int)* x, fp); break;} case 16: _LL213: { short* x=(( short*(*)(
void* x)) unsafe_cast)( val); Cyc_Std_putw(( int)* x, fp); break;} case 32:
_LL214: { int* x=(( int*(*)( void* x)) unsafe_cast)( val); Cyc_Std_putw(* x, fp);
break;} default: _LL215:({ struct Cyc_Std_Int_pa_struct _temp218; _temp218.tag=
Cyc_Std_Int_pa; _temp218.f1=( int) _temp196;{ void* _temp217[ 1u]={& _temp218};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("write_type_base: illegal int size %d bits\n",
sizeof( unsigned char), 43u), _tag_arr( _temp217, sizeof( void*), 1u));}}); exit(
1);} goto _LL175; _LL179: { float* x=(( float*(*)( void* x)) unsafe_cast)( val);
Cyc_Std_fwrite(( struct _tagged_arr)(( struct _tagged_arr) _tag_arr( x, sizeof(
float), 1u)), sizeof( float), 1, fp); goto _LL175;} _LL181: { double* x=((
double*(*)( void* x)) unsafe_cast)( val); Cyc_Std_fwrite(( struct _tagged_arr)((
struct _tagged_arr) _tag_arr( x, sizeof( double), 1u)), sizeof( double), 1, fp);
goto _LL175;} _LL183: { int* x=(( int*(*)( void* x)) unsafe_cast)( val); int y=*((
int*) _check_null( x)); struct _tuple5 _temp221; struct _tuple2 _temp222; int
_temp224; struct _tuple5* _temp219= Cyc_Marshal_lookup_insert( env,(
unsigned int) y); _temp221=* _temp219; _LL225: _temp224= _temp221.f1; goto
_LL223; _LL223: _temp222= _temp221.f2; goto _LL220; _LL220: env= _temp222; Cyc_Std_putw(
_temp224, fp); Cyc_Std_putw( y !=  0? 1: 0, fp); if( _temp224 ==  - 1? y !=  0:
0){ int typ_szb=( int) Cyc_Typerep_size_type( _temp198); int i= 0; for( 0; i < 
_temp200; i ++){ env= Cyc_Marshal_write_type_base( _temp198, env, fp,( void*)((
void*(*)( int x)) unsafe_cast)( y)); y += typ_szb;}} goto _LL175;} _LL185: {
struct _tuple6* x=(( struct _tuple6*(*)( void* x)) unsafe_cast)( val);
unsigned int typ_szb= Cyc_Typerep_size_type( _temp202); unsigned int base=(* x).f1;
unsigned int curr=(* x).f2; unsigned int last_plus_one=(* x).f3; unsigned int
pos=( curr -  base) /  typ_szb; unsigned int sz=( last_plus_one -  base) / 
typ_szb; struct _tuple5 _temp228; struct _tuple2 _temp229; int _temp231; struct
_tuple5* _temp226= Cyc_Marshal_lookup_insert( env, base); _temp228=* _temp226;
_LL232: _temp231= _temp228.f1; goto _LL230; _LL230: _temp229= _temp228.f2; goto
_LL227; _LL227: env= _temp229; Cyc_Std_putw( _temp231, fp); Cyc_Std_putw( base
!=  0? 1: 0, fp); Cyc_Std_putw(( int) pos, fp); Cyc_Std_putw(( int) sz, fp); if(
_temp231 ==  - 1? base !=  0: 0){ unsigned int i= base; for( 0; i < 
last_plus_one; i += typ_szb){ env= Cyc_Marshal_write_type_base( _temp202, env,
fp,( void*)(( void*(*)( unsigned int x)) unsafe_cast)( i));}} goto _LL175;}
_LL187: { int x=(( int(*)( void* x)) unsafe_cast)( val);{ int i= 0; for( 0; i < 
_get_arr_size( _temp204, sizeof( struct _tuple0*)); i ++){ void* _temp235;
unsigned int _temp237; struct _tuple0 _temp233=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp204, sizeof( struct _tuple0*), i))); _LL238:
_temp237= _temp233.f1; goto _LL236; _LL236: _temp235= _temp233.f2; goto _LL234;
_LL234: { int x_ofs=( int)( x +  _temp237); env= Cyc_Marshal_write_type_base(
_temp235, env, fp,( void*)(( void*(*)( int x)) unsafe_cast)( x_ofs));}}} goto
_LL175;} _LL189: { unsigned int* tagp=(( unsigned int*(*)( void* x)) unsafe_cast)(
val); if(* tagp <  1024){ Cyc_Std_putw(( int)(* tagp -  1025), fp);} else{
struct _tuple5 _temp241; struct _tuple2 _temp242; int _temp244; struct _tuple5*
_temp239= Cyc_Marshal_lookup_insert( env,* tagp); _temp241=* _temp239; _LL245:
_temp244= _temp241.f1; goto _LL243; _LL243: _temp242= _temp241.f2; goto _LL240;
_LL240: env= _temp242; Cyc_Std_putw( _temp244, fp); if( _temp244 ==  - 1){
unsigned int* x=(( unsigned int*(*)( unsigned int x)) unsafe_cast)(* tagp);
unsigned int tag=* x; Cyc_Std_putw(( int) tag, fp);{ void* typ= Cyc_Typerep_get_unionbranch(
tag, _temp206); env=(( struct _tuple2(*)( void* rep, struct _tuple2 env, struct
Cyc_Std___sFILE* fp, unsigned int* val)) Cyc_Marshal_write_type_base)( typ, env,
fp, x);}}} goto _LL175;} _LL191: { unsigned int* xtunionp=(( unsigned int*(*)(
void* x)) unsafe_cast)( val); struct _tuple5 _temp248; struct _tuple2 _temp249;
int _temp251; struct _tuple5* _temp246= Cyc_Marshal_lookup_insert( env,*
xtunionp); _temp248=* _temp246; _LL252: _temp251= _temp248.f1; goto _LL250;
_LL250: _temp249= _temp248.f2; goto _LL247; _LL247: env= _temp249; Cyc_Std_putw(
_temp251, fp); if( _temp251 ==  - 1){ unsigned int* xtstructp=(( unsigned int*(*)(
unsigned int x)) unsafe_cast)(* xtunionp); if(* xtstructp !=  0){ Cyc_Std_fputc(
1, fp);{ unsigned char* xtnamec=(( unsigned char*(*)( unsigned int x))
unsafe_cast)(* xtstructp +  4); struct _tagged_arr xtname=( struct _tagged_arr)
Cstring_to_string( xtnamec);({ struct Cyc_Std_String_pa_struct _temp254;
_temp254.tag= Cyc_Std_String_pa; _temp254.f1=( struct _tagged_arr) xtname;{ void*
_temp253[ 1u]={& _temp254}; Cyc_Std_fprintf( fp, _tag_arr("%s", sizeof(
unsigned char), 3u), _tag_arr( _temp253, sizeof( void*), 1u));}}); Cyc_Std_fputc(
0, fp);{ void* typ= Cyc_Typerep_get_xtunionbranch( xtname, _temp208); env=((
struct _tuple2(*)( void* rep, struct _tuple2 env, struct Cyc_Std___sFILE* fp,
unsigned int* val)) Cyc_Marshal_write_type_base)( typ, env, fp,( unsigned int*)
_check_null( xtstructp));}}} else{ Cyc_Std_fputc( 0, fp);{ unsigned char*
xtnamec=(( unsigned char*(*)( unsigned int x)) unsafe_cast)(* xtunionp +  4);
struct _tagged_arr xtname=( struct _tagged_arr) Cstring_to_string( xtnamec);({
struct Cyc_Std_String_pa_struct _temp256; _temp256.tag= Cyc_Std_String_pa;
_temp256.f1=( struct _tagged_arr) xtname;{ void* _temp255[ 1u]={& _temp256}; Cyc_Std_fprintf(
fp, _tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp255, sizeof( void*),
1u));}}); Cyc_Std_fputc( 0, fp);}}} goto _LL175;} _LL193: { int sz=( int) Cyc_Typerep_size_type(
rep); env= Cyc_Marshal_write_type_base(( void*)({ struct Cyc_Typerep_Tuple_struct*
_temp257=( struct Cyc_Typerep_Tuple_struct*) _cycalloc( sizeof( struct Cyc_Typerep_Tuple_struct));
_temp257[ 0]=({ struct Cyc_Typerep_Tuple_struct _temp258; _temp258.tag= Cyc_Typerep_Tuple;
_temp258.f1=( unsigned int) sz; _temp258.f2=({ unsigned int _temp259=(
unsigned int) sz; struct _tuple0** _temp260=( struct _tuple0**) _cycalloc(
_check_times( sizeof( struct _tuple0*), _temp259)); struct _tagged_arr _temp265=
_tag_arr( _temp260, sizeof( struct _tuple0*),( unsigned int) sz);{ unsigned int
_temp261= _temp259; unsigned int i; for( i= 0; i <  _temp261; i ++){ _temp260[ i]=({
struct _tuple0* _temp262=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp262->f1= i; _temp262->f2=( void*)({ struct Cyc_Typerep_Int_struct* _temp263=(
struct Cyc_Typerep_Int_struct*) _cycalloc_atomic( sizeof( struct Cyc_Typerep_Int_struct));
_temp263[ 0]=({ struct Cyc_Typerep_Int_struct _temp264; _temp264.tag= Cyc_Typerep_Int;
_temp264.f1= 8; _temp264;}); _temp263;}); _temp262;});}}; _temp265;}); _temp258;});
_temp257;}), env, fp, val); goto _LL175;} _LL195:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp266=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp266[ 0]=({ struct Cyc_Core_Failure_struct
_temp267; _temp267.tag= Cyc_Core_Failure; _temp267.f1= _tag_arr("Unhandled case in write_type_base",
sizeof( unsigned char), 34u); _temp267;}); _temp266;})); _LL175:;} return env;}
void Cyc_Marshal_write_type( void* ts, struct Cyc_Std___sFILE* fp, void* val){
struct _handler_cons _temp268; _push_handler(& _temp268);{ int _temp270= 0; if(
setjmp( _temp268.handler)){ _temp270= 1;} if( ! _temp270){ Cyc_Marshal_write_type_base(
ts, Cyc_Marshal_empty_addr_index(), fp, val);; _pop_handler();} else{ void*
_temp269=( void*) _exn_thrown; void* _temp272= _temp269; struct _tagged_arr
_temp278; _LL274: if(*(( void**) _temp272) ==  Cyc_Core_Failure){ _LL279:
_temp278=(( struct Cyc_Core_Failure_struct*) _temp272)->f1; goto _LL275;} else{
goto _LL276;} _LL276: goto _LL277; _LL275:({ struct Cyc_Std_String_pa_struct
_temp281; _temp281.tag= Cyc_Std_String_pa; _temp281.f1=( struct _tagged_arr)
_temp278;{ void* _temp280[ 1u]={& _temp281}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n", sizeof( unsigned char), 19u), _tag_arr( _temp280,
sizeof( void*), 1u));}});( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp282=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp282[ 0]=({ struct Cyc_Core_Failure_struct _temp283; _temp283.tag= Cyc_Core_Failure;
_temp283.f1= _temp278; _temp283;}); _temp282;})); _LL277:( void) _throw(
_temp272); _LL273:;}}} void Cyc_Marshal_fscanntstring( struct Cyc_Std___sFILE*
fp, struct _tagged_arr s){ int c; int i= 0; while(( c= Cyc_Std_fgetc( fp))? c != 
- 1: 0) {*(( unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
i))=( unsigned char) c; i ++;}*(( unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i))='\000';} struct _tuple3 Cyc_Marshal_read_type_base(
void* rep, struct _tuple3 env, struct Cyc_Std___sFILE* fp, void* result){{ void*
_temp284= rep; unsigned int _temp306; void* _temp308; unsigned int _temp310;
void* _temp312; struct _tagged_arr _temp314; struct _tagged_arr _temp316; struct
_tagged_arr _temp318; struct _tagged_arr _temp320; _LL286: if(( unsigned int)
_temp284 >  2u?*(( int*) _temp284) ==  Cyc_Typerep_Int: 0){ _LL307: _temp306=((
struct Cyc_Typerep_Int_struct*) _temp284)->f1; goto _LL287;} else{ goto _LL288;}
_LL288: if( _temp284 == ( void*) Cyc_Typerep_Float){ goto _LL289;} else{ goto
_LL290;} _LL290: if( _temp284 == ( void*) Cyc_Typerep_Double){ goto _LL291;}
else{ goto _LL292;} _LL292: if(( unsigned int) _temp284 >  2u?*(( int*) _temp284)
==  Cyc_Typerep_ThinPtr: 0){ _LL311: _temp310=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp284)->f1; goto _LL309; _LL309: _temp308=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp284)->f2; goto _LL293;} else{ goto _LL294;} _LL294: if(( unsigned int)
_temp284 >  2u?*(( int*) _temp284) ==  Cyc_Typerep_FatPtr: 0){ _LL313: _temp312=(
void*)(( struct Cyc_Typerep_FatPtr_struct*) _temp284)->f1; goto _LL295;} else{
goto _LL296;} _LL296: if(( unsigned int) _temp284 >  2u?*(( int*) _temp284) == 
Cyc_Typerep_Tuple: 0){ _LL315: _temp314=(( struct Cyc_Typerep_Tuple_struct*)
_temp284)->f2; goto _LL297;} else{ goto _LL298;} _LL298: if(( unsigned int)
_temp284 >  2u?*(( int*) _temp284) ==  Cyc_Typerep_TUnion: 0){ _LL317: _temp316=((
struct Cyc_Typerep_TUnion_struct*) _temp284)->f1; goto _LL299;} else{ goto
_LL300;} _LL300: if(( unsigned int) _temp284 >  2u?*(( int*) _temp284) ==  Cyc_Typerep_XTUnion:
0){ _LL319: _temp318=(( struct Cyc_Typerep_XTUnion_struct*) _temp284)->f1; goto
_LL301;} else{ goto _LL302;} _LL302: if(( unsigned int) _temp284 >  2u?*(( int*)
_temp284) ==  Cyc_Typerep_Union: 0){ _LL321: _temp320=(( struct Cyc_Typerep_Union_struct*)
_temp284)->f1; goto _LL303;} else{ goto _LL304;} _LL304: goto _LL305; _LL287:
switch( _temp306){ case 8: _LL322: { unsigned char* x=(( unsigned char*(*)( void*
x)) unsafe_cast)( result);* x=( unsigned char) Cyc_Std_fgetc( fp); break;} case
16: _LL323: { short* x=(( short*(*)( void* x)) unsafe_cast)( result);* x=( short)
Cyc_Std_getw( fp); break;} case 32: _LL324: { int* x=(( int*(*)( void* x))
unsafe_cast)( result);* x= Cyc_Std_getw( fp); break;} default: _LL325:( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp327=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp327[ 0]=({ struct Cyc_Core_Failure_struct
_temp328; _temp328.tag= Cyc_Core_Failure; _temp328.f1= _tag_arr("read_type_base: illegal int size bits",
sizeof( unsigned char), 38u); _temp328;}); _temp327;}));} goto _LL285; _LL289: {
struct _tagged_arr x= _tag_arr(({ unsigned int _temp329= sizeof( float);
unsigned char* _temp330=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp329));{ unsigned int _temp331= _temp329;
unsigned int i; for( i= 0; i <  _temp331; i ++){ _temp330[ i]='\000';}};
_temp330;}), sizeof( unsigned char), sizeof( float)); Cyc_Std_fread( x, sizeof(
float), 1, fp);{ float* xx=(( float*(*)( unsigned char* x)) unsafe_cast)((
unsigned char*) _check_null( _untag_arr( x, sizeof( unsigned char), 1u))); float*
y=(( float*(*)( void* x)) unsafe_cast)( result);* y=* xx; goto _LL285;}} _LL291: {
struct _tagged_arr x= _tag_arr(({ unsigned int _temp332= sizeof( double);
unsigned char* _temp333=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp332));{ unsigned int _temp334= _temp332;
unsigned int i; for( i= 0; i <  _temp334; i ++){ _temp333[ i]='\000';}};
_temp333;}), sizeof( unsigned char), sizeof( double)); Cyc_Std_fread( x, sizeof(
double), 1, fp);{ double* xx=(( double*(*)( unsigned char* x)) unsafe_cast)((
unsigned char*) _check_null( _untag_arr( x, sizeof( unsigned char), 1u)));
double* y=(( double*(*)( void* x)) unsafe_cast)( result);* y=* xx; goto _LL285;}}
_LL293: { int mem= Cyc_Std_getw( fp); int nonnull= Cyc_Std_getw( fp); if( mem == 
- 1){ int typ_szb=( int) Cyc_Typerep_size_type( _temp308); struct _tagged_arr
buf= nonnull?({ unsigned int _temp335= _temp310 *  typ_szb; unsigned char*
_temp336=( unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char),
_temp335)); struct _tagged_arr _temp338= _tag_arr( _temp336, sizeof(
unsigned char), _temp310 *  typ_szb);{ unsigned int _temp337= _temp335;
unsigned int i; for( i= 0; i <  _temp337; i ++){ _temp336[ i]='\000';}};
_temp338;}): _tag_arr( 0u, 0u, 0u); unsigned int bufptr= nonnull?( unsigned int)((
unsigned char*) _check_null( _untag_arr( buf, sizeof( unsigned char), 1u))): 0;
unsigned int* x=(( unsigned int*(*)( void* x)) unsafe_cast)( result);* x= bufptr;
env= Cyc_Marshal_table_insert( env, bufptr); if( nonnull){ int y=( int) bufptr;
int i= 0; for( 0; i <  _temp310; i ++){ env= Cyc_Marshal_read_type_base(
_temp308, env, fp,( void*)(( void*(*)( int x)) unsafe_cast)( y)); y += typ_szb;}}}
else{ unsigned int* x=(( unsigned int*(*)( void* x)) unsafe_cast)( result);* x=
Cyc_Marshal_table_lookup( env, mem);} goto _LL285;} _LL295: { int mem= Cyc_Std_getw(
fp); int nonnull= Cyc_Std_getw( fp); int pos= Cyc_Std_getw( fp); int sz= Cyc_Std_getw(
fp); int typ_szb=( int) Cyc_Typerep_size_type( _temp312); if( mem ==  - 1){
struct _tagged_arr buf= nonnull?({ unsigned int _temp339=( unsigned int)( sz * 
typ_szb); unsigned char* _temp340=( unsigned char*) _cycalloc_atomic(
_check_times( sizeof( unsigned char), _temp339)); struct _tagged_arr _temp342=
_tag_arr( _temp340, sizeof( unsigned char),( unsigned int)( sz *  typ_szb));{
unsigned int _temp341= _temp339; unsigned int i; for( i= 0; i <  _temp341; i ++){
_temp340[ i]='\000';}}; _temp342;}): _tag_arr( 0u, 0u, 0u); unsigned int bufptr=
nonnull?( unsigned int)(( unsigned char*) _check_null( _untag_arr( buf, sizeof(
unsigned char), 1u))): 0; struct _tuple6* x=(( struct _tuple6*(*)( void* x))
unsafe_cast)( result);(* x).f1= bufptr;(* x).f2= bufptr +  pos *  typ_szb;(* x).f3=
bufptr +  sz *  typ_szb; env= Cyc_Marshal_table_insert( env, bufptr); if(
nonnull){ int y=( int) bufptr; int i= 0; for( 0; i <  sz; i ++){ env= Cyc_Marshal_read_type_base(
_temp312, env, fp,( void*)(( void*(*)( int x)) unsafe_cast)( y)); y += typ_szb;}}}
else{ struct _tuple6* x=(( struct _tuple6*(*)( void* x)) unsafe_cast)( result);(*
x).f1= Cyc_Marshal_table_lookup( env, mem);(* x).f2=(* x).f1 +  typ_szb *  pos;(*
x).f3=(* x).f1 +  typ_szb *  sz;} goto _LL285;} _LL297: { unsigned int x=((
unsigned int(*)( void* x)) unsafe_cast)( result);{ int i= 0; for( 0; i < 
_get_arr_size( _temp314, sizeof( struct _tuple0*)); i ++){ void* _temp345;
unsigned int _temp347; struct _tuple0 _temp343=*(*(( struct _tuple0**)
_check_unknown_subscript( _temp314, sizeof( struct _tuple0*), i))); _LL348:
_temp347= _temp343.f1; goto _LL346; _LL346: _temp345= _temp343.f2; goto _LL344;
_LL344: { int x_ofs=( int)( x +  _temp347); env= Cyc_Marshal_read_type_base(
_temp345, env, fp,( void*)(( void*(*)( int x)) unsafe_cast)( x_ofs));}}} goto
_LL285;} _LL299: { int tagmem= Cyc_Std_getw( fp); if( tagmem <  - 1){ int tag=
tagmem +  1025; unsigned int* x=(( unsigned int*(*)( void* x)) unsafe_cast)(
result);* x=( unsigned int) tag;} else{ int mem= tagmem; if( mem ==  - 1){ int
tag= Cyc_Std_getw( fp); void* typ= Cyc_Typerep_get_unionbranch(( unsigned int)
tag, _temp316); struct _tagged_arr buf=({ unsigned int _temp349= Cyc_Typerep_size_type(
typ); unsigned char* _temp350=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp349)); struct _tagged_arr _temp352= _tag_arr(
_temp350, sizeof( unsigned char), Cyc_Typerep_size_type( typ));{ unsigned int
_temp351= _temp349; unsigned int i; for( i= 0; i <  _temp351; i ++){ _temp350[ i]='\000';}};
_temp352;}); unsigned int bufptr=( unsigned int)(( unsigned char*) _check_null(
_untag_arr( buf, sizeof( unsigned char), 1u))); env= Cyc_Marshal_table_insert(
env, bufptr);{ unsigned int* x=(( unsigned int*(*)( void* x)) unsafe_cast)(
result);* x= bufptr; env= Cyc_Marshal_read_type_base( typ, env, fp,( void*)((
void*(*)( unsigned int x)) unsafe_cast)( bufptr));}} else{ unsigned int* x=((
unsigned int*(*)( void* x)) unsafe_cast)( result);* x= Cyc_Marshal_table_lookup(
env, mem);}} goto _LL285;} _LL301: { int mem= Cyc_Std_getw( fp); if( mem ==  - 1){
unsigned char nonnull=( unsigned char) Cyc_Std_fgetc( fp); struct _tagged_arr s=
_tag_arr(({ unsigned int _temp357=( unsigned int) 100; unsigned char* _temp358=(
unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char), _temp357));{
unsigned int _temp359= _temp357; unsigned int i; for( i= 0; i <  _temp359; i ++){
_temp358[ i]='\000';}}; _temp358;}), sizeof( unsigned char),( unsigned int) 100);
struct _tagged_arr xtname= _tagged_arr_plus( s, sizeof( unsigned char), 4); Cyc_Marshal_fscanntstring(
fp, xtname); if(( int) nonnull){ void* typ= Cyc_Typerep_get_xtunionbranch((
struct _tagged_arr) xtname, _temp318); struct _tagged_arr buf=({ unsigned int
_temp353= Cyc_Typerep_size_type( typ); unsigned char* _temp354=( unsigned char*)
_cycalloc_atomic( _check_times( sizeof( unsigned char), _temp353)); struct
_tagged_arr _temp356= _tag_arr( _temp354, sizeof( unsigned char), Cyc_Typerep_size_type(
typ));{ unsigned int _temp355= _temp353; unsigned int i; for( i= 0; i < 
_temp355; i ++){ _temp354[ i]='\000';}}; _temp356;}); unsigned int bufptr=(
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
_LL285;} _LL303: { int sz=( int) Cyc_Typerep_size_type( rep); env= Cyc_Marshal_read_type_base((
void*)({ struct Cyc_Typerep_Tuple_struct* _temp360=( struct Cyc_Typerep_Tuple_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Tuple_struct)); _temp360[ 0]=({ struct Cyc_Typerep_Tuple_struct
_temp361; _temp361.tag= Cyc_Typerep_Tuple; _temp361.f1=( unsigned int) sz;
_temp361.f2=({ unsigned int _temp362=( unsigned int) sz; struct _tuple0**
_temp363=( struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*),
_temp362)); struct _tagged_arr _temp368= _tag_arr( _temp363, sizeof( struct
_tuple0*),( unsigned int) sz);{ unsigned int _temp364= _temp362; unsigned int i;
for( i= 0; i <  _temp364; i ++){ _temp363[ i]=({ struct _tuple0* _temp365=(
struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp365->f1= i; _temp365->f2=(
void*)({ struct Cyc_Typerep_Int_struct* _temp366=( struct Cyc_Typerep_Int_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Typerep_Int_struct)); _temp366[ 0]=({
struct Cyc_Typerep_Int_struct _temp367; _temp367.tag= Cyc_Typerep_Int; _temp367.f1=
8; _temp367;}); _temp366;}); _temp365;});}}; _temp368;}); _temp361;}); _temp360;}),
env, fp, result); goto _LL285;} _LL305:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp369=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp369[ 0]=({ struct Cyc_Core_Failure_struct _temp370; _temp370.tag= Cyc_Core_Failure;
_temp370.f1= _tag_arr("Unhandled case in read_type_base", sizeof( unsigned char),
33u); _temp370;}); _temp369;})); _LL285:;} return env;} void* Cyc_Marshal_read_type(
void* ts, struct Cyc_Std___sFILE* fp){ struct _handler_cons _temp371;
_push_handler(& _temp371);{ int _temp373= 0; if( setjmp( _temp371.handler)){
_temp373= 1;} if( ! _temp373){{ struct _tagged_arr buf=({ unsigned int _temp376=
Cyc_Typerep_size_type( ts); unsigned char* _temp377=( unsigned char*)
_cycalloc_atomic( _check_times( sizeof( unsigned char), _temp376)); struct
_tagged_arr _temp379= _tag_arr( _temp377, sizeof( unsigned char), Cyc_Typerep_size_type(
ts));{ unsigned int _temp378= _temp376; unsigned int i; for( i= 0; i <  _temp378;
i ++){ _temp377[ i]='\000';}}; _temp379;}); void* result=(( void*(*)(
unsigned char* x)) unsafe_cast)(( unsigned char*) _check_null( _untag_arr( buf,
sizeof( unsigned char), 1u))); Cyc_Marshal_read_type_base( ts,({ struct _tuple3
_temp374; _temp374.f1= _tag_arr( 0u, 0u, 0u); _temp374.f2= 0; _temp374;}), fp,
result);{ void* _temp375= result; _npop_handler( 0u); return _temp375;}};
_pop_handler();} else{ void* _temp372=( void*) _exn_thrown; void* _temp381=
_temp372; struct _tagged_arr _temp387; _LL383: if(*(( void**) _temp381) ==  Cyc_Core_Failure){
_LL388: _temp387=(( struct Cyc_Core_Failure_struct*) _temp381)->f1; goto _LL384;}
else{ goto _LL385;} _LL385: goto _LL386; _LL384:({ struct Cyc_Std_String_pa_struct
_temp390; _temp390.tag= Cyc_Std_String_pa; _temp390.f1=( struct _tagged_arr)
_temp387;{ void* _temp389[ 1u]={& _temp390}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Core::Failure(%s)\n", sizeof( unsigned char), 19u), _tag_arr( _temp389,
sizeof( void*), 1u));}});( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp391=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp391[ 0]=({ struct Cyc_Core_Failure_struct _temp392; _temp392.tag= Cyc_Core_Failure;
_temp392.f1= _temp387; _temp392;}); _temp391;})); _LL386:( void) _throw(
_temp381); _LL382:;}}}
