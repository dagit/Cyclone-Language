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
_tagged_arr s2); void* Cyc_Typerep_tuple_tl( void* typ){ void* _temp0= typ;
struct _tagged_arr _temp6; unsigned int _temp8; _LL2: if(( unsigned int) _temp0
>  2u?*(( int*) _temp0) ==  Cyc_Typerep_Tuple: 0){ _LL9: _temp8=(( struct Cyc_Typerep_Tuple_struct*)
_temp0)->f1; goto _LL7; _LL7: _temp6=(( struct Cyc_Typerep_Tuple_struct*) _temp0)->f2;
goto _LL3;} else{ goto _LL4;} _LL4: goto _LL5; _LL3: { int sz1=( int) Cyc_Typerep_size_type((*(*((
struct _tuple0**) _check_unknown_subscript( _temp6, sizeof( struct _tuple0*), 0)))).f2);
struct _tagged_arr _temp10=({ unsigned int _temp13= _get_arr_size( _temp6,
sizeof( struct _tuple0*)) -  1; struct _tuple0** _temp14=( struct _tuple0**)
_cycalloc( _check_times( sizeof( struct _tuple0*), _temp13)); struct _tagged_arr
_temp17= _tag_arr( _temp14, sizeof( struct _tuple0*), _get_arr_size( _temp6,
sizeof( struct _tuple0*)) -  1);{ unsigned int _temp15= _temp13; unsigned int i;
for( i= 0; i <  _temp15; i ++){ _temp14[ i]=({ struct _tuple0* _temp16=( struct
_tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp16->f1=(*(*(( struct _tuple0**)
_check_unknown_subscript( _temp6, sizeof( struct _tuple0*),( int)( i +  1))))).f1
-  sz1; _temp16->f2=(*(*(( struct _tuple0**) _check_unknown_subscript( _temp6,
sizeof( struct _tuple0*),( int)( i +  1))))).f2; _temp16;});}}; _temp17;});
return( void*)({ struct Cyc_Typerep_Tuple_struct* _temp11=( struct Cyc_Typerep_Tuple_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_Tuple_struct)); _temp11[ 0]=({ struct Cyc_Typerep_Tuple_struct
_temp12; _temp12.tag= Cyc_Typerep_Tuple; _temp12.f1= _temp8 -  sz1; _temp12.f2=
_temp10; _temp12;}); _temp11;});} _LL5: return typ; _LL1:;} void* Cyc_Typerep_get_unionbranch(
unsigned int tag, struct _tagged_arr l){{ int i= 0; for( 0; i <  _get_arr_size(
l, sizeof( struct _tuple0*)); i ++){ void* _temp20; unsigned int _temp22; struct
_tuple0 _temp18=*(( struct _tuple0**) l.curr)[ i]; _LL23: _temp22= _temp18.f1;
goto _LL21; _LL21: _temp20= _temp18.f2; goto _LL19; _LL19: if( _temp22 ==  tag){
return Cyc_Typerep_tuple_tl( _temp20);}}}( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp24=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp24[ 0]=({ struct Cyc_Core_Failure_struct _temp25; _temp25.tag= Cyc_Core_Failure;
_temp25.f1= _tag_arr("Could not find tag in TUnion", sizeof( unsigned char), 29u);
_temp25;}); _temp24;}));} void* Cyc_Typerep_get_xtunionbranch( struct
_tagged_arr tag, struct _tagged_arr l){{ int i= 0; for( 0; i <  _get_arr_size( l,
sizeof( struct _tuple1*)); i ++){ void* _temp28; struct _tagged_arr _temp30;
struct _tuple1 _temp26=*(( struct _tuple1**) l.curr)[ i]; _LL31: _temp30=
_temp26.f1; goto _LL29; _LL29: _temp28= _temp26.f2; goto _LL27; _LL27: if( Cyc_Std_strcmp(
_temp30, tag) ==  0){ return Cyc_Typerep_tuple_tl( _temp28);}}}( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp32=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp32[ 0]=({ struct Cyc_Core_Failure_struct
_temp33; _temp33.tag= Cyc_Core_Failure; _temp33.f1= _tag_arr("Could not find tag in XTUnion",
sizeof( unsigned char), 30u); _temp33;}); _temp32;}));} unsigned int Cyc_Typerep_size_type(
void* rep){ void* _temp34= rep; unsigned int _temp60; unsigned int _temp62;
struct _tagged_arr _temp64; void** _temp66; void* _temp68; _LL36: if((
unsigned int) _temp34 >  2u?*(( int*) _temp34) ==  Cyc_Typerep_Int: 0){ _LL61:
_temp60=(( struct Cyc_Typerep_Int_struct*) _temp34)->f1; goto _LL37;} else{ goto
_LL38;} _LL38: if( _temp34 == ( void*) Cyc_Typerep_Float){ goto _LL39;} else{
goto _LL40;} _LL40: if( _temp34 == ( void*) Cyc_Typerep_Double){ goto _LL41;}
else{ goto _LL42;} _LL42: if(( unsigned int) _temp34 >  2u?*(( int*) _temp34) == 
Cyc_Typerep_ThinPtr: 0){ goto _LL43;} else{ goto _LL44;} _LL44: if((
unsigned int) _temp34 >  2u?*(( int*) _temp34) ==  Cyc_Typerep_FatPtr: 0){ goto
_LL45;} else{ goto _LL46;} _LL46: if(( unsigned int) _temp34 >  2u?*(( int*)
_temp34) ==  Cyc_Typerep_Tuple: 0){ _LL63: _temp62=(( struct Cyc_Typerep_Tuple_struct*)
_temp34)->f1; goto _LL47;} else{ goto _LL48;} _LL48: if(( unsigned int) _temp34
>  2u?*(( int*) _temp34) ==  Cyc_Typerep_TUnion: 0){ goto _LL49;} else{ goto
_LL50;} _LL50: if(( unsigned int) _temp34 >  2u?*(( int*) _temp34) ==  Cyc_Typerep_XTUnion:
0){ goto _LL51;} else{ goto _LL52;} _LL52: if(( unsigned int) _temp34 >  2u?*((
int*) _temp34) ==  Cyc_Typerep_Union: 0){ _LL65: _temp64=(( struct Cyc_Typerep_Union_struct*)
_temp34)->f1; goto _LL53;} else{ goto _LL54;} _LL54: if(( unsigned int) _temp34
>  2u?*(( int*) _temp34) ==  Cyc_Typerep_Var: 0){ goto _LL55;} else{ goto _LL56;}
_LL56: if(( unsigned int) _temp34 >  2u?*(( int*) _temp34) ==  Cyc_Typerep_Forall:
0){ _LL67: _temp66=(( struct Cyc_Typerep_Forall_struct*) _temp34)->f2; goto
_LL57;} else{ goto _LL58;} _LL58: if(( unsigned int) _temp34 >  2u?*(( int*)
_temp34) ==  Cyc_Typerep_App: 0){ _LL69: _temp68=( void*)(( struct Cyc_Typerep_App_struct*)
_temp34)->f1; goto _LL59;} else{ goto _LL35;} _LL37: return _temp60 >>  3; _LL39:
return sizeof( float); _LL41: return sizeof( double); _LL43: return sizeof( void*);
_LL45: return sizeof( struct _tagged_arr); _LL47: return _temp62; _LL49: return
sizeof( void*); _LL51: return sizeof( void*); _LL53: { int max= 0;{ int i= 0;
for( 0; i <  _get_arr_size( _temp64, sizeof( void**)); i ++){ unsigned int
_temp70= Cyc_Typerep_size_type(*(( void***) _temp64.curr)[ i]); max=( int)( max
<  _temp70? _temp70:( unsigned int) max);}} return( unsigned int) max;} _LL55:
return sizeof( void*); _LL57: return Cyc_Typerep_size_type(* _temp66); _LL59:
return Cyc_Typerep_size_type( _temp68); _LL35:;} void Cyc_Typerep_print_typestruct(
void* rep){ void* _temp71= rep; struct _tagged_arr* _temp97; unsigned int
_temp99; void* _temp101; unsigned int _temp103; void* _temp105; struct
_tagged_arr _temp107; unsigned int _temp109; struct _tagged_arr _temp111; struct
_tagged_arr _temp113; struct _tagged_arr _temp115; void** _temp117; struct
_tagged_arr _temp119; struct _tagged_arr _temp121; void* _temp123; _LL73: if((
unsigned int) _temp71 >  2u?*(( int*) _temp71) ==  Cyc_Typerep_Var: 0){ _LL98:
_temp97=(( struct Cyc_Typerep_Var_struct*) _temp71)->f1; goto _LL74;} else{ goto
_LL75;} _LL75: if(( unsigned int) _temp71 >  2u?*(( int*) _temp71) ==  Cyc_Typerep_Int:
0){ _LL100: _temp99=(( struct Cyc_Typerep_Int_struct*) _temp71)->f1; goto _LL76;}
else{ goto _LL77;} _LL77: if( _temp71 == ( void*) Cyc_Typerep_Float){ goto _LL78;}
else{ goto _LL79;} _LL79: if( _temp71 == ( void*) Cyc_Typerep_Double){ goto
_LL80;} else{ goto _LL81;} _LL81: if(( unsigned int) _temp71 >  2u?*(( int*)
_temp71) ==  Cyc_Typerep_ThinPtr: 0){ _LL104: _temp103=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp71)->f1; goto _LL102; _LL102: _temp101=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp71)->f2; goto _LL82;} else{ goto _LL83;} _LL83: if(( unsigned int) _temp71
>  2u?*(( int*) _temp71) ==  Cyc_Typerep_FatPtr: 0){ _LL106: _temp105=( void*)((
struct Cyc_Typerep_FatPtr_struct*) _temp71)->f1; goto _LL84;} else{ goto _LL85;}
_LL85: if(( unsigned int) _temp71 >  2u?*(( int*) _temp71) ==  Cyc_Typerep_Tuple:
0){ _LL110: _temp109=(( struct Cyc_Typerep_Tuple_struct*) _temp71)->f1; goto
_LL108; _LL108: _temp107=(( struct Cyc_Typerep_Tuple_struct*) _temp71)->f2; goto
_LL86;} else{ goto _LL87;} _LL87: if(( unsigned int) _temp71 >  2u?*(( int*)
_temp71) ==  Cyc_Typerep_TUnion: 0){ _LL112: _temp111=(( struct Cyc_Typerep_TUnion_struct*)
_temp71)->f1; goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp71
>  2u?*(( int*) _temp71) ==  Cyc_Typerep_XTUnion: 0){ _LL114: _temp113=(( struct
Cyc_Typerep_XTUnion_struct*) _temp71)->f1; goto _LL90;} else{ goto _LL91;} _LL91:
if(( unsigned int) _temp71 >  2u?*(( int*) _temp71) ==  Cyc_Typerep_Union: 0){
_LL116: _temp115=(( struct Cyc_Typerep_Union_struct*) _temp71)->f1; goto _LL92;}
else{ goto _LL93;} _LL93: if(( unsigned int) _temp71 >  2u?*(( int*) _temp71) == 
Cyc_Typerep_Forall: 0){ _LL120: _temp119=(( struct Cyc_Typerep_Forall_struct*)
_temp71)->f1; goto _LL118; _LL118: _temp117=(( struct Cyc_Typerep_Forall_struct*)
_temp71)->f2; goto _LL94;} else{ goto _LL95;} _LL95: if(( unsigned int) _temp71
>  2u?*(( int*) _temp71) ==  Cyc_Typerep_App: 0){ _LL124: _temp123=( void*)((
struct Cyc_Typerep_App_struct*) _temp71)->f1; goto _LL122; _LL122: _temp121=((
struct Cyc_Typerep_App_struct*) _temp71)->f2; goto _LL96;} else{ goto _LL72;}
_LL74:({ struct Cyc_Std_String_pa_struct _temp126; _temp126.tag= Cyc_Std_String_pa;
_temp126.f1=( struct _tagged_arr)* _temp97;{ void* _temp125[ 1u]={& _temp126};
Cyc_Std_printf( _tag_arr("Var(%s)", sizeof( unsigned char), 8u), _tag_arr(
_temp125, sizeof( void*), 1u));}}); goto _LL72; _LL76:({ struct Cyc_Std_Int_pa_struct
_temp128; _temp128.tag= Cyc_Std_Int_pa; _temp128.f1=( int) _temp99;{ void*
_temp127[ 1u]={& _temp128}; Cyc_Std_printf( _tag_arr("Int(%d)", sizeof(
unsigned char), 8u), _tag_arr( _temp127, sizeof( void*), 1u));}}); goto _LL72;
_LL78:({ void* _temp129[ 0u]={}; Cyc_Std_printf( _tag_arr("Float", sizeof(
unsigned char), 6u), _tag_arr( _temp129, sizeof( void*), 0u));}); goto _LL72;
_LL80:({ void* _temp130[ 0u]={}; Cyc_Std_printf( _tag_arr("Double", sizeof(
unsigned char), 7u), _tag_arr( _temp130, sizeof( void*), 0u));}); goto _LL72;
_LL82:({ struct Cyc_Std_Int_pa_struct _temp132; _temp132.tag= Cyc_Std_Int_pa;
_temp132.f1=( int) _temp103;{ void* _temp131[ 1u]={& _temp132}; Cyc_Std_printf(
_tag_arr("ThinPtr(%d,", sizeof( unsigned char), 12u), _tag_arr( _temp131,
sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct( _temp101);({ void*
_temp133[ 0u]={}; Cyc_Std_printf( _tag_arr(")", sizeof( unsigned char), 2u),
_tag_arr( _temp133, sizeof( void*), 0u));}); goto _LL72; _LL84:({ void* _temp134[
0u]={}; Cyc_Std_printf( _tag_arr("FatPtr(", sizeof( unsigned char), 8u),
_tag_arr( _temp134, sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct(
_temp105);({ void* _temp135[ 0u]={}; Cyc_Std_printf( _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr( _temp135, sizeof( void*), 0u));}); goto _LL72;
_LL86:({ struct Cyc_Std_Int_pa_struct _temp137; _temp137.tag= Cyc_Std_Int_pa;
_temp137.f1=( int) _temp109;{ void* _temp136[ 1u]={& _temp137}; Cyc_Std_printf(
_tag_arr("Tuple(%d,[", sizeof( unsigned char), 11u), _tag_arr( _temp136, sizeof(
void*), 1u));}});{ int i= 0; for( 0; i <  _get_arr_size( _temp107, sizeof(
struct _tuple0*)); i ++){ void* _temp140; unsigned int _temp142; struct _tuple0
_temp138=*(( struct _tuple0**) _temp107.curr)[ i]; _LL143: _temp142= _temp138.f1;
goto _LL141; _LL141: _temp140= _temp138.f2; goto _LL139; _LL139:({ struct Cyc_Std_Int_pa_struct
_temp145; _temp145.tag= Cyc_Std_Int_pa; _temp145.f1=( int) _temp142;{ void*
_temp144[ 1u]={& _temp145}; Cyc_Std_printf( _tag_arr("$(%d,", sizeof(
unsigned char), 6u), _tag_arr( _temp144, sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct(
_temp140);({ struct Cyc_Std_Int_pa_struct _temp147; _temp147.tag= Cyc_Std_Int_pa;
_temp147.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size( _temp107,
sizeof( struct _tuple0*)) -  1?',':' ')));{ void* _temp146[ 1u]={& _temp147};
Cyc_Std_printf( _tag_arr(")%c", sizeof( unsigned char), 4u), _tag_arr( _temp146,
sizeof( void*), 1u));}});}}({ void* _temp148[ 0u]={}; Cyc_Std_printf( _tag_arr(" ]",
sizeof( unsigned char), 3u), _tag_arr( _temp148, sizeof( void*), 0u));}); goto
_LL72; _LL88:({ void* _temp149[ 0u]={}; Cyc_Std_printf( _tag_arr("TUnion(",
sizeof( unsigned char), 8u), _tag_arr( _temp149, sizeof( void*), 0u));});{ int i=
0; for( 0; i <  _get_arr_size( _temp111, sizeof( struct _tuple0*)); i ++){ void*
_temp152; unsigned int _temp154; struct _tuple0 _temp150=*(( struct _tuple0**)
_temp111.curr)[ i]; _LL155: _temp154= _temp150.f1; goto _LL153; _LL153: _temp152=
_temp150.f2; goto _LL151; _LL151:({ struct Cyc_Std_Int_pa_struct _temp157;
_temp157.tag= Cyc_Std_Int_pa; _temp157.f1=( int) _temp154;{ void* _temp156[ 1u]={&
_temp157}; Cyc_Std_printf( _tag_arr("$(%d,", sizeof( unsigned char), 6u),
_tag_arr( _temp156, sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct(
_temp152);({ struct Cyc_Std_Int_pa_struct _temp159; _temp159.tag= Cyc_Std_Int_pa;
_temp159.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size( _temp111,
sizeof( struct _tuple0*)) -  1?',':' ')));{ void* _temp158[ 1u]={& _temp159};
Cyc_Std_printf( _tag_arr(")%c", sizeof( unsigned char), 4u), _tag_arr( _temp158,
sizeof( void*), 1u));}});}}({ void* _temp160[ 0u]={}; Cyc_Std_printf( _tag_arr(" )",
sizeof( unsigned char), 3u), _tag_arr( _temp160, sizeof( void*), 0u));}); goto
_LL72; _LL90:({ void* _temp161[ 0u]={}; Cyc_Std_printf( _tag_arr("XTUnion(",
sizeof( unsigned char), 9u), _tag_arr( _temp161, sizeof( void*), 0u));});{ int i=
0; for( 0; i <  _get_arr_size( _temp113, sizeof( struct _tuple1*)); i ++){ void*
_temp164; struct _tagged_arr _temp166; struct _tuple1 _temp162=*(( struct
_tuple1**) _temp113.curr)[ i]; _LL167: _temp166= _temp162.f1; goto _LL165;
_LL165: _temp164= _temp162.f2; goto _LL163; _LL163:({ struct Cyc_Std_String_pa_struct
_temp169; _temp169.tag= Cyc_Std_String_pa; _temp169.f1=( struct _tagged_arr)
_tagged_arr_plus( _temp166, sizeof( unsigned char), 4);{ void* _temp168[ 1u]={&
_temp169}; Cyc_Std_printf( _tag_arr("$(%s,", sizeof( unsigned char), 6u),
_tag_arr( _temp168, sizeof( void*), 1u));}}); Cyc_Typerep_print_typestruct(
_temp164);({ struct Cyc_Std_Int_pa_struct _temp171; _temp171.tag= Cyc_Std_Int_pa;
_temp171.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size( _temp113,
sizeof( struct _tuple1*)) -  1?',':' ')));{ void* _temp170[ 1u]={& _temp171};
Cyc_Std_printf( _tag_arr(")%c", sizeof( unsigned char), 4u), _tag_arr( _temp170,
sizeof( void*), 1u));}});}}({ void* _temp172[ 0u]={}; Cyc_Std_printf( _tag_arr(" )",
sizeof( unsigned char), 3u), _tag_arr( _temp172, sizeof( void*), 0u));}); goto
_LL72; _LL92:({ void* _temp173[ 0u]={}; Cyc_Std_printf( _tag_arr("Union(",
sizeof( unsigned char), 7u), _tag_arr( _temp173, sizeof( void*), 0u));});{ int i=
0; for( 0; i <  _get_arr_size( _temp115, sizeof( void**)); i ++){ Cyc_Typerep_print_typestruct(*((
void***) _temp115.curr)[ i]);({ struct Cyc_Std_Int_pa_struct _temp175; _temp175.tag=
Cyc_Std_Int_pa; _temp175.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size(
_temp115, sizeof( void**)) -  1?',':' ')));{ void* _temp174[ 1u]={& _temp175};
Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char), 3u), _tag_arr( _temp174,
sizeof( void*), 1u));}});}}({ void* _temp176[ 0u]={}; Cyc_Std_printf( _tag_arr(")",
sizeof( unsigned char), 2u), _tag_arr( _temp176, sizeof( void*), 0u));}); goto
_LL72; _LL94:({ void* _temp177[ 0u]={}; Cyc_Std_printf( _tag_arr("Forall([",
sizeof( unsigned char), 9u), _tag_arr( _temp177, sizeof( void*), 0u));});{ int i=
0; for( 0; i <  _get_arr_size( _temp119, sizeof( struct _tagged_arr*)); i ++){({
void* _temp178[ 0u]={}; Cyc_Std_printf(*(( struct _tagged_arr**) _temp119.curr)[
i], _tag_arr( _temp178, sizeof( void*), 0u));});({ struct Cyc_Std_Int_pa_struct
_temp180; _temp180.tag= Cyc_Std_Int_pa; _temp180.f1=( int)(( unsigned int)(( int)(
i !=  _get_arr_size( _temp119, sizeof( struct _tagged_arr*)) -  1?',':' ')));{
void* _temp179[ 1u]={& _temp180}; Cyc_Std_printf( _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp179, sizeof( void*), 1u));}});}}({ void*
_temp181[ 0u]={}; Cyc_Std_printf( _tag_arr("],", sizeof( unsigned char), 3u),
_tag_arr( _temp181, sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct(*
_temp117);({ void* _temp182[ 0u]={}; Cyc_Std_printf( _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr( _temp182, sizeof( void*), 0u));}); goto _LL72;
_LL96:({ void* _temp183[ 0u]={}; Cyc_Std_printf( _tag_arr("App(", sizeof(
unsigned char), 5u), _tag_arr( _temp183, sizeof( void*), 0u));}); Cyc_Typerep_print_typestruct(
_temp123);({ void* _temp184[ 0u]={}; Cyc_Std_printf( _tag_arr(",[", sizeof(
unsigned char), 3u), _tag_arr( _temp184, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp121, sizeof( void*)); i ++){ Cyc_Typerep_print_typestruct(((
void**) _temp121.curr)[ i]);({ struct Cyc_Std_Int_pa_struct _temp186; _temp186.tag=
Cyc_Std_Int_pa; _temp186.f1=( int)(( unsigned int)(( int)( i !=  _get_arr_size(
_temp121, sizeof( void*)) -  1?',':' ')));{ void* _temp185[ 1u]={& _temp186};
Cyc_Std_printf( _tag_arr("%c", sizeof( unsigned char), 3u), _tag_arr( _temp185,
sizeof( void*), 1u));}});}}({ void* _temp187[ 0u]={}; Cyc_Std_printf( _tag_arr("])",
sizeof( unsigned char), 3u), _tag_arr( _temp187, sizeof( void*), 0u));}); goto
_LL72; _LL72:;} void Cyc_Typerep_xmlize_typestruct( void* rep){ void* _temp188=
rep; struct _tagged_arr* _temp214; unsigned int _temp216; void* _temp218;
unsigned int _temp220; void* _temp222; struct _tagged_arr _temp224; unsigned int
_temp226; struct _tagged_arr _temp228; struct _tagged_arr _temp230; struct
_tagged_arr _temp232; void** _temp234; struct _tagged_arr _temp236; struct
_tagged_arr _temp238; void* _temp240; _LL190: if(( unsigned int) _temp188 >  2u?*((
int*) _temp188) ==  Cyc_Typerep_Var: 0){ _LL215: _temp214=(( struct Cyc_Typerep_Var_struct*)
_temp188)->f1; goto _LL191;} else{ goto _LL192;} _LL192: if(( unsigned int)
_temp188 >  2u?*(( int*) _temp188) ==  Cyc_Typerep_Int: 0){ _LL217: _temp216=((
struct Cyc_Typerep_Int_struct*) _temp188)->f1; goto _LL193;} else{ goto _LL194;}
_LL194: if( _temp188 == ( void*) Cyc_Typerep_Float){ goto _LL195;} else{ goto
_LL196;} _LL196: if( _temp188 == ( void*) Cyc_Typerep_Double){ goto _LL197;}
else{ goto _LL198;} _LL198: if(( unsigned int) _temp188 >  2u?*(( int*) _temp188)
==  Cyc_Typerep_ThinPtr: 0){ _LL221: _temp220=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp188)->f1; goto _LL219; _LL219: _temp218=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp188)->f2; goto _LL199;} else{ goto _LL200;} _LL200: if(( unsigned int)
_temp188 >  2u?*(( int*) _temp188) ==  Cyc_Typerep_FatPtr: 0){ _LL223: _temp222=(
void*)(( struct Cyc_Typerep_FatPtr_struct*) _temp188)->f1; goto _LL201;} else{
goto _LL202;} _LL202: if(( unsigned int) _temp188 >  2u?*(( int*) _temp188) == 
Cyc_Typerep_Tuple: 0){ _LL227: _temp226=(( struct Cyc_Typerep_Tuple_struct*)
_temp188)->f1; goto _LL225; _LL225: _temp224=(( struct Cyc_Typerep_Tuple_struct*)
_temp188)->f2; goto _LL203;} else{ goto _LL204;} _LL204: if(( unsigned int)
_temp188 >  2u?*(( int*) _temp188) ==  Cyc_Typerep_TUnion: 0){ _LL229: _temp228=((
struct Cyc_Typerep_TUnion_struct*) _temp188)->f1; goto _LL205;} else{ goto
_LL206;} _LL206: if(( unsigned int) _temp188 >  2u?*(( int*) _temp188) ==  Cyc_Typerep_XTUnion:
0){ _LL231: _temp230=(( struct Cyc_Typerep_XTUnion_struct*) _temp188)->f1; goto
_LL207;} else{ goto _LL208;} _LL208: if(( unsigned int) _temp188 >  2u?*(( int*)
_temp188) ==  Cyc_Typerep_Union: 0){ _LL233: _temp232=(( struct Cyc_Typerep_Union_struct*)
_temp188)->f1; goto _LL209;} else{ goto _LL210;} _LL210: if(( unsigned int)
_temp188 >  2u?*(( int*) _temp188) ==  Cyc_Typerep_Forall: 0){ _LL237: _temp236=((
struct Cyc_Typerep_Forall_struct*) _temp188)->f1; goto _LL235; _LL235: _temp234=((
struct Cyc_Typerep_Forall_struct*) _temp188)->f2; goto _LL211;} else{ goto
_LL212;} _LL212: if(( unsigned int) _temp188 >  2u?*(( int*) _temp188) ==  Cyc_Typerep_App:
0){ _LL241: _temp240=( void*)(( struct Cyc_Typerep_App_struct*) _temp188)->f1;
goto _LL239; _LL239: _temp238=(( struct Cyc_Typerep_App_struct*) _temp188)->f2;
goto _LL213;} else{ goto _LL189;} _LL191:({ struct Cyc_Std_String_pa_struct
_temp243; _temp243.tag= Cyc_Std_String_pa; _temp243.f1=( struct _tagged_arr)*
_temp214;{ void* _temp242[ 1u]={& _temp243}; Cyc_Std_printf( _tag_arr("<Var name=\"%s\"/>",
sizeof( unsigned char), 17u), _tag_arr( _temp242, sizeof( void*), 1u));}}); goto
_LL189; _LL193:({ struct Cyc_Std_Int_pa_struct _temp245; _temp245.tag= Cyc_Std_Int_pa;
_temp245.f1=( int) _temp216;{ void* _temp244[ 1u]={& _temp245}; Cyc_Std_printf(
_tag_arr("<Int sz=\"%d\"/>", sizeof( unsigned char), 15u), _tag_arr( _temp244,
sizeof( void*), 1u));}}); goto _LL189; _LL195:({ void* _temp246[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Float/>", sizeof( unsigned char), 9u), _tag_arr( _temp246, sizeof(
void*), 0u));}); goto _LL189; _LL197:({ void* _temp247[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Double/>", sizeof( unsigned char), 10u), _tag_arr( _temp247, sizeof(
void*), 0u));}); goto _LL189; _LL199:({ struct Cyc_Std_Int_pa_struct _temp249;
_temp249.tag= Cyc_Std_Int_pa; _temp249.f1=( int) _temp220;{ void* _temp248[ 1u]={&
_temp249}; Cyc_Std_printf( _tag_arr("<ThinPtr sz=\"%d\">", sizeof( unsigned char),
18u), _tag_arr( _temp248, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp218);({ void* _temp250[ 0u]={}; Cyc_Std_printf( _tag_arr("</ThinPtr",
sizeof( unsigned char), 10u), _tag_arr( _temp250, sizeof( void*), 0u));}); goto
_LL189; _LL201:({ void* _temp251[ 0u]={}; Cyc_Std_printf( _tag_arr("<FatPtr>",
sizeof( unsigned char), 9u), _tag_arr( _temp251, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct(
_temp222);({ void* _temp252[ 0u]={}; Cyc_Std_printf( _tag_arr("</FatPtr>",
sizeof( unsigned char), 10u), _tag_arr( _temp252, sizeof( void*), 0u));}); goto
_LL189; _LL203:({ struct Cyc_Std_Int_pa_struct _temp254; _temp254.tag= Cyc_Std_Int_pa;
_temp254.f1=( int) _temp226;{ void* _temp253[ 1u]={& _temp254}; Cyc_Std_printf(
_tag_arr("<Tuple sz=\"%d\">", sizeof( unsigned char), 16u), _tag_arr( _temp253,
sizeof( void*), 1u));}});{ int i= 0; for( 0; i <  _get_arr_size( _temp224,
sizeof( struct _tuple0*)); i ++){ void* _temp257; unsigned int _temp259; struct
_tuple0 _temp255=*(( struct _tuple0**) _temp224.curr)[ i]; _LL260: _temp259=
_temp255.f1; goto _LL258; _LL258: _temp257= _temp255.f2; goto _LL256; _LL256:({
struct Cyc_Std_Int_pa_struct _temp262; _temp262.tag= Cyc_Std_Int_pa; _temp262.f1=(
int) _temp259;{ void* _temp261[ 1u]={& _temp262}; Cyc_Std_printf( _tag_arr("<Component ofs=\"%d\">",
sizeof( unsigned char), 21u), _tag_arr( _temp261, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp257);({ void* _temp263[ 0u]={}; Cyc_Std_printf( _tag_arr("</Component>",
sizeof( unsigned char), 13u), _tag_arr( _temp263, sizeof( void*), 0u));});}}({
void* _temp264[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tuple>", sizeof(
unsigned char), 9u), _tag_arr( _temp264, sizeof( void*), 0u));}); goto _LL189;
_LL205:({ void* _temp265[ 0u]={}; Cyc_Std_printf( _tag_arr("<TUnion>", sizeof(
unsigned char), 9u), _tag_arr( _temp265, sizeof( void*), 0u));});{ int i= 0;
for( 0; i <  _get_arr_size( _temp228, sizeof( struct _tuple0*)); i ++){ void*
_temp268; unsigned int _temp270; struct _tuple0 _temp266=*(( struct _tuple0**)
_temp228.curr)[ i]; _LL271: _temp270= _temp266.f1; goto _LL269; _LL269: _temp268=
_temp266.f2; goto _LL267; _LL267:({ struct Cyc_Std_Int_pa_struct _temp273;
_temp273.tag= Cyc_Std_Int_pa; _temp273.f1=( int) _temp270;{ void* _temp272[ 1u]={&
_temp273}; Cyc_Std_printf( _tag_arr("<Tag tag=\"%d\">", sizeof( unsigned char),
15u), _tag_arr( _temp272, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp268);({ void* _temp274[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tag>", sizeof(
unsigned char), 7u), _tag_arr( _temp274, sizeof( void*), 0u));});}}({ void*
_temp275[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tunion>", sizeof( unsigned char),
10u), _tag_arr( _temp275, sizeof( void*), 0u));}); goto _LL189; _LL207:({ void*
_temp276[ 0u]={}; Cyc_Std_printf( _tag_arr("<XTUnion>", sizeof( unsigned char),
10u), _tag_arr( _temp276, sizeof( void*), 0u));});{ int i= 0; for( 0; i < 
_get_arr_size( _temp230, sizeof( struct _tuple1*)); i ++){ void* _temp279;
struct _tagged_arr _temp281; struct _tuple1 _temp277=*(( struct _tuple1**)
_temp230.curr)[ i]; _LL282: _temp281= _temp277.f1; goto _LL280; _LL280: _temp279=
_temp277.f2; goto _LL278; _LL278:({ struct Cyc_Std_String_pa_struct _temp284;
_temp284.tag= Cyc_Std_String_pa; _temp284.f1=( struct _tagged_arr)
_tagged_arr_plus( _temp281, sizeof( unsigned char), 4);{ void* _temp283[ 1u]={&
_temp284}; Cyc_Std_printf( _tag_arr("<Tag tag=\"%s\">", sizeof( unsigned char),
15u), _tag_arr( _temp283, sizeof( void*), 1u));}}); Cyc_Typerep_xmlize_typestruct(
_temp279);({ void* _temp285[ 0u]={}; Cyc_Std_printf( _tag_arr("</Tag>", sizeof(
unsigned char), 7u), _tag_arr( _temp285, sizeof( void*), 0u));});}}({ void*
_temp286[ 0u]={}; Cyc_Std_printf( _tag_arr("</XTunion>", sizeof( unsigned char),
11u), _tag_arr( _temp286, sizeof( void*), 0u));}); goto _LL189; _LL209:({ void*
_temp287[ 0u]={}; Cyc_Std_printf( _tag_arr("<Union>", sizeof( unsigned char), 8u),
_tag_arr( _temp287, sizeof( void*), 0u));});{ int i= 0; for( 0; i < 
_get_arr_size( _temp232, sizeof( void**)); i ++){({ void* _temp288[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Case>", sizeof( unsigned char), 7u), _tag_arr( _temp288, sizeof( void*),
0u));}); Cyc_Typerep_xmlize_typestruct(*(( void***) _temp232.curr)[ i]);({ void*
_temp289[ 0u]={}; Cyc_Std_printf( _tag_arr("</Case>", sizeof( unsigned char), 8u),
_tag_arr( _temp289, sizeof( void*), 0u));});}}({ void* _temp290[ 0u]={}; Cyc_Std_printf(
_tag_arr("</Union>", sizeof( unsigned char), 9u), _tag_arr( _temp290, sizeof(
void*), 0u));}); goto _LL189; _LL211:({ void* _temp291[ 0u]={}; Cyc_Std_printf(
_tag_arr("<Forall vars=\"", sizeof( unsigned char), 15u), _tag_arr( _temp291,
sizeof( void*), 0u));});{ int i= 0; for( 0; i <  _get_arr_size( _temp236,
sizeof( struct _tagged_arr*)); i ++){ if( i !=  0){({ void* _temp292[ 0u]={};
Cyc_Std_printf( _tag_arr(", ", sizeof( unsigned char), 3u), _tag_arr( _temp292,
sizeof( void*), 0u));});}({ void* _temp293[ 0u]={}; Cyc_Std_printf(*(( struct
_tagged_arr**) _temp236.curr)[ i], _tag_arr( _temp293, sizeof( void*), 0u));});}}({
void* _temp294[ 0u]={}; Cyc_Std_printf( _tag_arr("\">", sizeof( unsigned char),
3u), _tag_arr( _temp294, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct(*
_temp234);({ void* _temp295[ 0u]={}; Cyc_Std_printf( _tag_arr("</Forall>",
sizeof( unsigned char), 10u), _tag_arr( _temp295, sizeof( void*), 0u));}); goto
_LL189; _LL213:({ void* _temp296[ 0u]={}; Cyc_Std_printf( _tag_arr("<App>",
sizeof( unsigned char), 6u), _tag_arr( _temp296, sizeof( void*), 0u));}); Cyc_Typerep_xmlize_typestruct(
_temp240);{ int i= 0; for( 0; i <  _get_arr_size( _temp238, sizeof( void*)); i
++){ if( i !=  0){({ void* _temp297[ 0u]={}; Cyc_Std_printf( _tag_arr(", ",
sizeof( unsigned char), 3u), _tag_arr( _temp297, sizeof( void*), 0u));});} Cyc_Typerep_xmlize_typestruct(((
void**) _temp238.curr)[ i]);}}({ void* _temp298[ 0u]={}; Cyc_Std_printf(
_tag_arr("</App>", sizeof( unsigned char), 7u), _tag_arr( _temp298, sizeof( void*),
0u));}); goto _LL189; _LL189:;} static void* Cyc_Typerep_normalize_env( struct
Cyc_Dict_Dict* env, void* ts){ void* _temp299= ts; struct _tagged_arr* _temp325;
void* _temp327; unsigned int _temp329; void* _temp331; struct _tagged_arr
_temp333; unsigned int _temp335; struct _tagged_arr _temp337; struct _tagged_arr
_temp339; struct _tagged_arr _temp341; void** _temp343; struct _tagged_arr
_temp345; struct _tagged_arr _temp347; void* _temp349; _LL301: if(( unsigned int)
_temp299 >  2u?*(( int*) _temp299) ==  Cyc_Typerep_Var: 0){ _LL326: _temp325=((
struct Cyc_Typerep_Var_struct*) _temp299)->f1; goto _LL302;} else{ goto _LL303;}
_LL303: if(( unsigned int) _temp299 >  2u?*(( int*) _temp299) ==  Cyc_Typerep_Int:
0){ goto _LL304;} else{ goto _LL305;} _LL305: if( _temp299 == ( void*) Cyc_Typerep_Float){
goto _LL306;} else{ goto _LL307;} _LL307: if( _temp299 == ( void*) Cyc_Typerep_Double){
goto _LL308;} else{ goto _LL309;} _LL309: if(( unsigned int) _temp299 >  2u?*((
int*) _temp299) ==  Cyc_Typerep_ThinPtr: 0){ _LL330: _temp329=(( struct Cyc_Typerep_ThinPtr_struct*)
_temp299)->f1; goto _LL328; _LL328: _temp327=( void*)(( struct Cyc_Typerep_ThinPtr_struct*)
_temp299)->f2; goto _LL310;} else{ goto _LL311;} _LL311: if(( unsigned int)
_temp299 >  2u?*(( int*) _temp299) ==  Cyc_Typerep_FatPtr: 0){ _LL332: _temp331=(
void*)(( struct Cyc_Typerep_FatPtr_struct*) _temp299)->f1; goto _LL312;} else{
goto _LL313;} _LL313: if(( unsigned int) _temp299 >  2u?*(( int*) _temp299) == 
Cyc_Typerep_Tuple: 0){ _LL336: _temp335=(( struct Cyc_Typerep_Tuple_struct*)
_temp299)->f1; goto _LL334; _LL334: _temp333=(( struct Cyc_Typerep_Tuple_struct*)
_temp299)->f2; goto _LL314;} else{ goto _LL315;} _LL315: if(( unsigned int)
_temp299 >  2u?*(( int*) _temp299) ==  Cyc_Typerep_TUnion: 0){ _LL338: _temp337=((
struct Cyc_Typerep_TUnion_struct*) _temp299)->f1; goto _LL316;} else{ goto
_LL317;} _LL317: if(( unsigned int) _temp299 >  2u?*(( int*) _temp299) ==  Cyc_Typerep_XTUnion:
0){ _LL340: _temp339=(( struct Cyc_Typerep_XTUnion_struct*) _temp299)->f1; goto
_LL318;} else{ goto _LL319;} _LL319: if(( unsigned int) _temp299 >  2u?*(( int*)
_temp299) ==  Cyc_Typerep_Union: 0){ _LL342: _temp341=(( struct Cyc_Typerep_Union_struct*)
_temp299)->f1; goto _LL320;} else{ goto _LL321;} _LL321: if(( unsigned int)
_temp299 >  2u?*(( int*) _temp299) ==  Cyc_Typerep_Forall: 0){ _LL346: _temp345=((
struct Cyc_Typerep_Forall_struct*) _temp299)->f1; goto _LL344; _LL344: _temp343=((
struct Cyc_Typerep_Forall_struct*) _temp299)->f2; goto _LL322;} else{ goto
_LL323;} _LL323: if(( unsigned int) _temp299 >  2u?*(( int*) _temp299) ==  Cyc_Typerep_App:
0){ _LL350: _temp349=( void*)(( struct Cyc_Typerep_App_struct*) _temp299)->f1;
goto _LL348; _LL348: _temp347=(( struct Cyc_Typerep_App_struct*) _temp299)->f2;
goto _LL324;} else{ goto _LL300;} _LL302: return(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( env,( struct _tagged_arr*) _temp325);
_LL304: goto _LL306; _LL306: goto _LL308; _LL308: return ts; _LL310: return(
void*)({ struct Cyc_Typerep_ThinPtr_struct* _temp351=( struct Cyc_Typerep_ThinPtr_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_ThinPtr_struct)); _temp351[ 0]=({ struct
Cyc_Typerep_ThinPtr_struct _temp352; _temp352.tag= Cyc_Typerep_ThinPtr; _temp352.f1=
_temp329; _temp352.f2=( void*) Cyc_Typerep_normalize_env( env, _temp327);
_temp352;}); _temp351;}); _LL312: return( void*)({ struct Cyc_Typerep_FatPtr_struct*
_temp353=( struct Cyc_Typerep_FatPtr_struct*) _cycalloc( sizeof( struct Cyc_Typerep_FatPtr_struct));
_temp353[ 0]=({ struct Cyc_Typerep_FatPtr_struct _temp354; _temp354.tag= Cyc_Typerep_FatPtr;
_temp354.f1=( void*) Cyc_Typerep_normalize_env( env, _temp331); _temp354;});
_temp353;}); _LL314: { struct _tagged_arr _temp355=({ unsigned int _temp371=
_get_arr_size( _temp333, sizeof( struct _tuple0*)); struct _tuple0** _temp372=(
struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*), _temp371));
struct _tagged_arr _temp374= _tag_arr( _temp372, sizeof( struct _tuple0*),
_get_arr_size( _temp333, sizeof( struct _tuple0*)));{ unsigned int _temp373=
_temp371; unsigned int i; for( i= 0; i <  _temp373; i ++){ _temp372[ i]= 0;}};
_temp374;});{ int i= 0; for( 0; i <  _get_arr_size( _temp333, sizeof( struct
_tuple0*)); i ++){ struct _tuple0 _temp358; void* _temp359; unsigned int
_temp361; struct _tuple0* _temp356=(( struct _tuple0**) _temp333.curr)[ i];
_temp358=* _temp356; _LL362: _temp361= _temp358.f1; goto _LL360; _LL360:
_temp359= _temp358.f2; goto _LL357; _LL357:(( struct _tuple0**) _temp355.curr)[
i]=({ struct _tuple0* _temp363=( struct _tuple0*) _cycalloc( sizeof( struct
_tuple0)); _temp363->f1= _temp361; _temp363->f2= Cyc_Typerep_normalize_env( env,
_temp359); _temp363;});}}{ struct _tagged_arr _temp364=({ unsigned int _temp367=
_get_arr_size( _temp333, sizeof( struct _tuple0*)); struct _tuple0** _temp368=(
struct _tuple0**) _cycalloc( _check_times( sizeof( struct _tuple0*), _temp367));
struct _tagged_arr _temp370= _tag_arr( _temp368, sizeof( struct _tuple0*),
_get_arr_size( _temp333, sizeof( struct _tuple0*)));{ unsigned int _temp369=
_temp367; unsigned int i; for( i= 0; i <  _temp369; i ++){ _temp368[ i]=( struct
_tuple0*) _check_null(*(( struct _tuple0**) _check_unknown_subscript( _temp355,
sizeof( struct _tuple0*),( int) i)));}}; _temp370;}); return( void*)({ struct
Cyc_Typerep_Tuple_struct* _temp365=( struct Cyc_Typerep_Tuple_struct*) _cycalloc(
sizeof( struct Cyc_Typerep_Tuple_struct)); _temp365[ 0]=({ struct Cyc_Typerep_Tuple_struct
_temp366; _temp366.tag= Cyc_Typerep_Tuple; _temp366.f1= _temp335; _temp366.f2=(
struct _tagged_arr) _temp364; _temp366;}); _temp365;});}} _LL316: { struct
_tagged_arr _temp375=({ unsigned int _temp391= _get_arr_size( _temp337, sizeof(
struct _tuple0*)); struct _tuple0** _temp392=( struct _tuple0**) _cycalloc(
_check_times( sizeof( struct _tuple0*), _temp391)); struct _tagged_arr _temp394=
_tag_arr( _temp392, sizeof( struct _tuple0*), _get_arr_size( _temp337, sizeof(
struct _tuple0*)));{ unsigned int _temp393= _temp391; unsigned int i; for( i= 0;
i <  _temp393; i ++){ _temp392[ i]= 0;}}; _temp394;});{ int i= 0; for( 0; i < 
_get_arr_size( _temp337, sizeof( struct _tuple0*)); i ++){ struct _tuple0
_temp378; void* _temp379; unsigned int _temp381; struct _tuple0* _temp376=((
struct _tuple0**) _temp337.curr)[ i]; _temp378=* _temp376; _LL382: _temp381=
_temp378.f1; goto _LL380; _LL380: _temp379= _temp378.f2; goto _LL377; _LL377:((
struct _tuple0**) _temp375.curr)[ i]=({ struct _tuple0* _temp383=( struct
_tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp383->f1= _temp381; _temp383->f2=
Cyc_Typerep_normalize_env( env, _temp379); _temp383;});}}{ struct _tagged_arr
_temp384=({ unsigned int _temp387= _get_arr_size( _temp337, sizeof( struct
_tuple0*)); struct _tuple0** _temp388=( struct _tuple0**) _cycalloc(
_check_times( sizeof( struct _tuple0*), _temp387)); struct _tagged_arr _temp390=
_tag_arr( _temp388, sizeof( struct _tuple0*), _get_arr_size( _temp337, sizeof(
struct _tuple0*)));{ unsigned int _temp389= _temp387; unsigned int i; for( i= 0;
i <  _temp389; i ++){ _temp388[ i]=( struct _tuple0*) _check_null(*(( struct
_tuple0**) _check_unknown_subscript( _temp375, sizeof( struct _tuple0*),( int) i)));}};
_temp390;}); return( void*)({ struct Cyc_Typerep_TUnion_struct* _temp385=(
struct Cyc_Typerep_TUnion_struct*) _cycalloc( sizeof( struct Cyc_Typerep_TUnion_struct));
_temp385[ 0]=({ struct Cyc_Typerep_TUnion_struct _temp386; _temp386.tag= Cyc_Typerep_TUnion;
_temp386.f1=( struct _tagged_arr) _temp384; _temp386;}); _temp385;});}} _LL318: {
struct _tagged_arr _temp395=({ unsigned int _temp411= _get_arr_size( _temp339,
sizeof( struct _tuple1*)); struct _tuple1** _temp412=( struct _tuple1**)
_cycalloc( _check_times( sizeof( struct _tuple1*), _temp411)); struct
_tagged_arr _temp414= _tag_arr( _temp412, sizeof( struct _tuple1*),
_get_arr_size( _temp339, sizeof( struct _tuple1*)));{ unsigned int _temp413=
_temp411; unsigned int i; for( i= 0; i <  _temp413; i ++){ _temp412[ i]= 0;}};
_temp414;});{ int i= 0; for( 0; i <  _get_arr_size( _temp339, sizeof( struct
_tuple1*)); i ++){ struct _tuple1 _temp398; void* _temp399; struct _tagged_arr
_temp401; struct _tuple1* _temp396=(( struct _tuple1**) _temp339.curr)[ i];
_temp398=* _temp396; _LL402: _temp401= _temp398.f1; goto _LL400; _LL400:
_temp399= _temp398.f2; goto _LL397; _LL397:(( struct _tuple1**) _temp395.curr)[
i]=({ struct _tuple1* _temp403=( struct _tuple1*) _cycalloc( sizeof( struct
_tuple1)); _temp403->f1= _temp401; _temp403->f2= Cyc_Typerep_normalize_env( env,
_temp399); _temp403;});}}{ struct _tagged_arr _temp404=({ unsigned int _temp407=
_get_arr_size( _temp339, sizeof( struct _tuple1*)); struct _tuple1** _temp408=(
struct _tuple1**) _cycalloc( _check_times( sizeof( struct _tuple1*), _temp407));
struct _tagged_arr _temp410= _tag_arr( _temp408, sizeof( struct _tuple1*),
_get_arr_size( _temp339, sizeof( struct _tuple1*)));{ unsigned int _temp409=
_temp407; unsigned int i; for( i= 0; i <  _temp409; i ++){ _temp408[ i]=( struct
_tuple1*) _check_null(*(( struct _tuple1**) _check_unknown_subscript( _temp395,
sizeof( struct _tuple1*),( int) i)));}}; _temp410;}); return( void*)({ struct
Cyc_Typerep_XTUnion_struct* _temp405=( struct Cyc_Typerep_XTUnion_struct*)
_cycalloc( sizeof( struct Cyc_Typerep_XTUnion_struct)); _temp405[ 0]=({ struct
Cyc_Typerep_XTUnion_struct _temp406; _temp406.tag= Cyc_Typerep_XTUnion; _temp406.f1=(
struct _tagged_arr) _temp404; _temp406;}); _temp405;});}} _LL320: { struct
_tagged_arr _temp415=({ unsigned int _temp424= _get_arr_size( _temp341, sizeof(
void**)); void*** _temp425=( void***) _cycalloc( _check_times( sizeof( void**),
_temp424)); struct _tagged_arr _temp427= _tag_arr( _temp425, sizeof( void**),
_get_arr_size( _temp341, sizeof( void**)));{ unsigned int _temp426= _temp424;
unsigned int i; for( i= 0; i <  _temp426; i ++){ _temp425[ i]= 0;}}; _temp427;});{
int i= 0; for( 0; i <  _get_arr_size( _temp341, sizeof( void**)); i ++){(( void***)
_temp415.curr)[ i]=({ void** _temp416=( void**) _cycalloc( sizeof( void*));
_temp416[ 0]= Cyc_Typerep_normalize_env( env,*(( void***) _temp341.curr)[ i]);
_temp416;});}}{ struct _tagged_arr _temp417=({ unsigned int _temp420=
_get_arr_size( _temp341, sizeof( void**)); void*** _temp421=( void***) _cycalloc(
_check_times( sizeof( void**), _temp420)); struct _tagged_arr _temp423= _tag_arr(
_temp421, sizeof( void**), _get_arr_size( _temp341, sizeof( void**)));{
unsigned int _temp422= _temp420; unsigned int i; for( i= 0; i <  _temp422; i ++){
_temp421[ i]=( void**) _check_null(*(( void***) _check_unknown_subscript(
_temp415, sizeof( void**),( int) i)));}}; _temp423;}); return( void*)({ struct
Cyc_Typerep_Union_struct* _temp418=( struct Cyc_Typerep_Union_struct*) _cycalloc(
sizeof( struct Cyc_Typerep_Union_struct)); _temp418[ 0]=({ struct Cyc_Typerep_Union_struct
_temp419; _temp419.tag= Cyc_Typerep_Union; _temp419.f1=( struct _tagged_arr)
_temp417; _temp419;}); _temp418;});}} _LL322: return( void*)({ struct Cyc_Typerep_Forall_struct*
_temp428=( struct Cyc_Typerep_Forall_struct*) _cycalloc( sizeof( struct Cyc_Typerep_Forall_struct));
_temp428[ 0]=({ struct Cyc_Typerep_Forall_struct _temp429; _temp429.tag= Cyc_Typerep_Forall;
_temp429.f1= _temp345; _temp429.f2=({ void** _temp430=( void**) _cycalloc(
sizeof( void*)); _temp430[ 0]= Cyc_Typerep_normalize_env( env,* _temp343);
_temp430;}); _temp429;}); _temp428;}); _LL324: { void* _temp431= Cyc_Typerep_normalize_env(
env, _temp349); void** _temp437; struct _tagged_arr _temp439; _LL433: if((
unsigned int) _temp431 >  2u?*(( int*) _temp431) ==  Cyc_Typerep_Forall: 0){
_LL440: _temp439=(( struct Cyc_Typerep_Forall_struct*) _temp431)->f1; goto
_LL438; _LL438: _temp437=(( struct Cyc_Typerep_Forall_struct*) _temp431)->f2;
goto _LL434;} else{ goto _LL435;} _LL435: goto _LL436; _LL434: if( _get_arr_size(
_temp347, sizeof( void*)) !=  _get_arr_size( _temp439, sizeof( struct
_tagged_arr*))){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp441=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp441[ 0]=({ struct Cyc_Core_Failure_struct _temp442; _temp442.tag= Cyc_Core_Failure;
_temp442.f1= _tag_arr("app appiled wrong number of arguments", sizeof(
unsigned char), 38u); _temp442;}); _temp441;}));}{ int i= 0; for( 0; i < 
_get_arr_size( _temp347, sizeof( void*)); i ++){ env=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( env,((
struct _tagged_arr**) _temp439.curr)[ i],(( void**) _temp347.curr)[ i]);}}
return Cyc_Typerep_normalize_env( env,* _temp437); _LL436:( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp443=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp443[ 0]=({ struct Cyc_Core_Failure_struct
_temp444; _temp444.tag= Cyc_Core_Failure; _temp444.f1= _tag_arr("app misapplied",
sizeof( unsigned char), 15u); _temp444;}); _temp443;})); _LL432:;} _LL300:;} int
Cyc_Typerep_name_order( struct _tagged_arr* s1, struct _tagged_arr* s2){ return
Cyc_Std_strcmp(* s1,* s2);} void* Cyc_Typerep_normalize( void* ts){ return Cyc_Typerep_normalize_env(((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Typerep_name_order), ts);}
