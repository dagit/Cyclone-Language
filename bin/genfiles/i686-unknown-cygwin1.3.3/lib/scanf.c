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
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int isalnum( int); extern int isalpha(
int); extern int isascii( int); extern int iscntrl( int); extern int isdigit(
int); extern int isgraph( int); extern int islower( int); extern int isprint(
int); extern int ispunct( int); extern int isspace( int); extern int isupper(
int); extern int isxdigit( int); extern int toascii( int); extern int tolower(
int); extern int toupper( int); extern int _tolower( int); extern int _toupper(
int); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
extern unsigned char Cyc_Array_Array_mismatch[ 19u]; struct Cyc_Std__Div{ int
quot; int rem; } ; struct Cyc_Std__Ldiv{ int quot; int rem; } ; extern int abs(
int __x); extern int atexit( void(* __func)()); extern struct Cyc_Std__Div div(
int __numer, int __denom); extern struct Cyc_Std__Ldiv ldiv( int __numer, int
__denom); extern int random(); extern void srandom( unsigned int __seed); extern
int rand(); extern void srand( unsigned int __seed); extern int rand_r(
unsigned int* __seed); extern int grantpt( int __fd); extern int unlockpt( int
__fd); extern double Cyc_Std_atof( struct _tagged_arr); extern int Cyc_Std_strtol(
struct _tagged_arr n, struct _tagged_arr* end, int base); extern unsigned int
Cyc_Std_strtoul( struct _tagged_arr n, struct _tagged_arr* end, int base);
struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE*
Cyc_Std_stdin; extern int Cyc_Std_fgetc( struct Cyc_Std___sFILE* __stream);
extern int Cyc_Std_getc( struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_ungetc(
int __c, struct Cyc_Std___sFILE* __stream); extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_ShortPtr_sa= 0; struct
Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int Cyc_Std_scanf( struct _tagged_arr fmt, struct
_tagged_arr); extern int Cyc_Std_fscanf( struct Cyc_Std___sFILE* stream, struct
_tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_sscanf( struct
_tagged_arr src, struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_vfscanf(
struct Cyc_Std___sFILE* stream, struct _tagged_arr fmt, struct _tagged_arr);
extern int Cyc_Std_vsscanf( struct _tagged_arr src, struct _tagged_arr fmt,
struct _tagged_arr); static struct _tagged_arr Cyc_Std___sccl( struct
_tagged_arr tab, struct _tagged_arr fmt); static short* Cyc_Std_va_arg_short_ptr(
void* a){ void* _temp0= a; short* _temp8; unsigned short* _temp10; _LL2: if(*((
int*) _temp0) ==  Cyc_Std_ShortPtr_sa){ _LL9: _temp8=(( struct Cyc_Std_ShortPtr_sa_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(*(( int*) _temp0) ==  Cyc_Std_UShortPtr_sa){
_LL11: _temp10=(( struct Cyc_Std_UShortPtr_sa_struct*) _temp0)->f1; goto _LL5;}
else{ goto _LL6;} _LL6: goto _LL7; _LL3: return _temp8; _LL5: return( short*)
_temp10; _LL7:( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp12=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp12[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp13; _temp13.tag= Cyc_Core_Invalid_argument; _temp13.f1= _tag_arr("scan expects short pointer",
sizeof( unsigned char), 27u); _temp13;}); _temp12;})); _LL1:;} static int* Cyc_Std_va_arg_int_ptr(
void* a){ void* _temp14= a; int* _temp22; unsigned int* _temp24; _LL16: if(*((
int*) _temp14) ==  Cyc_Std_IntPtr_sa){ _LL23: _temp22=(( struct Cyc_Std_IntPtr_sa_struct*)
_temp14)->f1; goto _LL17;} else{ goto _LL18;} _LL18: if(*(( int*) _temp14) == 
Cyc_Std_UIntPtr_sa){ _LL25: _temp24=(( struct Cyc_Std_UIntPtr_sa_struct*)
_temp14)->f1; goto _LL19;} else{ goto _LL20;} _LL20: goto _LL21; _LL17: return
_temp22; _LL19: return( int*) _temp24; _LL21:( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp26=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp26[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp27; _temp27.tag= Cyc_Core_Invalid_argument; _temp27.f1= _tag_arr("scan expects int pointer",
sizeof( unsigned char), 25u); _temp27;}); _temp26;})); _LL15:;} static struct
_tagged_arr Cyc_Std_va_arg_string_ptr( void* a){ void* _temp28= a; struct
_tagged_arr _temp34; _LL30: if(*(( int*) _temp28) ==  Cyc_Std_StringPtr_sa){
_LL35: _temp34=(( struct Cyc_Std_StringPtr_sa_struct*) _temp28)->f1; goto _LL31;}
else{ goto _LL32;} _LL32: goto _LL33; _LL31: return _temp34; _LL33:( int) _throw((
void*)({ struct Cyc_Core_Invalid_argument_struct* _temp36=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp36[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp37; _temp37.tag= Cyc_Core_Invalid_argument;
_temp37.f1= _tag_arr("scan expects char pointer", sizeof( unsigned char), 26u);
_temp37;}); _temp36;})); _LL29:;} static double* Cyc_Std_va_arg_double_ptr( void*
a){ void* _temp38= a; double* _temp44; _LL40: if(*(( int*) _temp38) ==  Cyc_Std_DoublePtr_sa){
_LL45: _temp44=(( struct Cyc_Std_DoublePtr_sa_struct*) _temp38)->f1; goto _LL41;}
else{ goto _LL42;} _LL42: goto _LL43; _LL41: return _temp44; _LL43:( int) _throw((
void*)({ struct Cyc_Core_Invalid_argument_struct* _temp46=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp46[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp47; _temp47.tag= Cyc_Core_Invalid_argument;
_temp47.f1= _tag_arr("scan expects double pointer", sizeof( unsigned char), 28u);
_temp47;}); _temp46;})); _LL39:;} static float* Cyc_Std_va_arg_float_ptr( void*
a){ void* _temp48= a; float* _temp54; _LL50: if(*(( int*) _temp48) ==  Cyc_Std_FloatPtr_sa){
_LL55: _temp54=(( struct Cyc_Std_FloatPtr_sa_struct*) _temp48)->f1; goto _LL51;}
else{ goto _LL52;} _LL52: goto _LL53; _LL51: return _temp54; _LL53:( int) _throw((
void*)({ struct Cyc_Core_Invalid_argument_struct* _temp56=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp56[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp57; _temp57.tag= Cyc_Core_Invalid_argument;
_temp57.f1= _tag_arr("scan expects float pointer", sizeof( unsigned char), 27u);
_temp57;}); _temp56;})); _LL49:;} int Cyc_Std__IO_vfscanf( int(* _IO_getc)( void*),
int(* _IO_ungetc)( int, void*), int(* _IO_peekc)( void*), void* fp, struct
_tagged_arr fmt0, struct _tagged_arr ap, int* errp){ struct _tagged_arr fmt=
fmt0; int c; long long width; struct _tagged_arr p= _tag_arr( 0u, 0u, 0u); int n;
int flags= 0; struct _tagged_arr p0= _tag_arr( 0u, 0u, 0u); int nassigned; int
nread; int base= 0; int use_strtoul= 0; unsigned char ccltab[ 256u];
unsigned char buf[ 350u]; int seen_eof= 0; static short basefix[ 17u]={ 10, 1, 2,
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; nassigned= 0; nread= 0; for( 0;
1; 0){ c=( int)*(( const unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u)); if( c ==  0){ goto done;} if( isspace( c)){ for( 0; 1; 0){
c= _IO_getc( fp); if( c ==  - 1){ seen_eof ++; break;} if( ! isspace( c)){
_IO_ungetc( c, fp); break;} nread ++;} continue;} if( c != '%'){ goto literal;}
width=( long long) 0; flags= 0; again: c=( int)*(( const unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& fmt, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u)); switch( c){ case '%': _LL58:
literal: n= _IO_getc( fp); if( n ==  - 1){ goto eof_failure;} if( n !=  c){
_IO_ungetc( n, fp); goto match_failure;} nread ++; continue; case '*': _LL59:
if( flags){ goto control_failure;} flags= 8; goto again; case 'l': _LL60: if(
flags &  ~( 8 |  64)){ goto control_failure;} flags |= 1; goto again; case 'L':
_LL61: if( flags &  ~( 8 |  64)){ goto control_failure;} flags |= 2; goto again;
case 'h': _LL62: if( flags &  ~( 8 |  64)){ goto control_failure;} flags |= 4;
goto again; case '0': _LL63: goto _LL64; case '1': _LL64: goto _LL65; case '2':
_LL65: goto _LL66; case '3': _LL66: goto _LL67; case '4': _LL67: goto _LL68;
case '5': _LL68: goto _LL69; case '6': _LL69: goto _LL70; case '7': _LL70: goto
_LL71; case '8': _LL71: goto _LL72; case '9': _LL72: if( flags &  ~( 8 |  64)){
goto control_failure;} flags |= 64; width=( width *  10 +  c) - '0'; goto again;
case 'D': _LL73: flags |= 1; goto _LL74; case 'd': _LL74: c= 3; use_strtoul= 0;
base= 10; break; case 'i': _LL75: c= 3; use_strtoul= 0; base= 0; break; case 'O':
_LL76: flags |= 1; goto _LL77; case 'o': _LL77: c= 3; use_strtoul= 1; base= 8;
break; case 'u': _LL78: c= 3; use_strtoul= 1; base= 10; break; case 'X': _LL79:
goto _LL80; case 'x': _LL80: flags |= 256; c= 3; use_strtoul= 1; base= 16;
break; case 'E': _LL81: goto _LL82; case 'F': _LL82: goto _LL83; case 'e': _LL83:
goto _LL84; case 'f': _LL84: goto _LL85; case 'g': _LL85: c= 4; break; case 's':
_LL86: c= 2; break; case '[': _LL87: fmt= Cyc_Std___sccl( _tag_arr( ccltab,
sizeof( unsigned char), 256u), fmt); flags |= 32; c= 1; break; case 'c': _LL88:
flags |= 32; c= 0; break; case 'p': _LL89: flags |= 16 |  256; c= 3; use_strtoul=
1; base= 16; break; case 'n': _LL90: if( flags &  8){ continue;} if( flags &  4){*
Cyc_Std_va_arg_short_ptr(*(( void**) _check_unknown_subscript( ap, sizeof( void*),
0u)))=( short) nread;} else{ if( flags &  1){* Cyc_Std_va_arg_int_ptr(*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 0u)))=( int) nread;} else{* Cyc_Std_va_arg_int_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)))=( int) nread;}}
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); continue; case '\000':
_LL91: nassigned= - 1; goto done; default: _LL92: if( isupper( c)){ flags |= 1;}
c= 3; use_strtoul= 0; base= 10; break;} if( _IO_peekc( fp) ==  - 1){ goto
eof_failure;} if(( flags &  32) ==  0){ n= _IO_peekc( fp); while( isspace( n)) {
n= _IO_getc( fp); nread ++; n= _IO_peekc( fp); if( n ==  - 1){ goto eof_failure;}}}
switch( c){ case 0: _LL94: if( width ==  0){ width=( long long) 1;} if( flags & 
8){ long long sum=( long long) 0; for( 0; 1; 0){ n= _IO_getc( fp); if( n ==  - 1?
width !=  0: 0){ goto eof_failure;} else{ if( n ==  - 1){ seen_eof ++; break;}}
sum ++; width --;} nread += sum;} else{ long long sum=( long long) 0; struct
_tagged_arr _temp96= Cyc_Std_va_arg_string_ptr(*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 0u)));
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); for( 0; 1; 0){ n=
_IO_getc( fp); if( n ==  - 1? width !=  0: 0){ goto eof_failure;} else{ if( n == 
- 1){ seen_eof ++; break;}} if( _get_arr_size( _temp96, sizeof( unsigned char))
==  0){ goto eof_failure;}*(( unsigned char*) _check_unknown_subscript( _temp96,
sizeof( unsigned char), 0u))=( unsigned char) n; sum ++; width --;
_tagged_arr_inplace_plus_post(& _temp96, sizeof( unsigned char), 1);} nread +=
sum; if( _get_arr_size( _temp96, sizeof( unsigned char)) !=  0){*((
unsigned char*) _check_unknown_subscript( _temp96, sizeof( unsigned char), 0u))='\000';}
nassigned ++;} break; case 1: _LL95: if( width ==  0){ width=( long long) ~ 0;}
if( flags &  8){ n= 0;{ int c= _IO_peekc( fp); while(( int) ccltab[
_check_known_subscript_notnull( 256u,( int)(( unsigned char) c))]) { n ++;
_IO_getc( fp); if( -- width ==  0){ break;} if(( c= _IO_peekc( fp)) ==  - 1){
if( n ==  0){ goto eof_failure;} seen_eof ++; break;}} if( n ==  0){ goto
match_failure;}}} else{ struct _tagged_arr _temp98= Cyc_Std_va_arg_string_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)));
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1);{ struct _tagged_arr
_temp99= _temp98; int c= _IO_peekc( fp); while(( int) ccltab[
_check_known_subscript_notnull( 256u,( int)(( unsigned char) c))]) { if(
_get_arr_size( _temp99, sizeof( unsigned char)) ==  0){ goto eof_failure;}*((
unsigned char*) _check_unknown_subscript( _temp99, sizeof( unsigned char), 0u))=(
unsigned char) c; _tagged_arr_inplace_plus_post(& _temp99, sizeof( unsigned char),
1); _IO_getc( fp); if( -- width ==  0){ break;} if(( c= _IO_peekc( fp)) ==  - 1){
if( _temp99.curr ==  p0.curr){ goto eof_failure;} seen_eof ++; break;}} n=(
_temp99.curr -  _temp98.curr) /  sizeof( unsigned char); if( n ==  0){ goto
match_failure;} if( _get_arr_size( _temp99, sizeof( unsigned char)) ==  0){ goto
eof_failure;}*(( unsigned char*) _check_unknown_subscript( _temp99, sizeof(
unsigned char), 0u))='\000'; nassigned ++;}} nread += n; break; case 2: _LL97:
if( width ==  0){ width=( long long) ~ 0;} if( flags &  8){ n= 0;{ int c=
_IO_peekc( fp); while( ! isspace(( int)(( unsigned char) c))) { n ++; _IO_getc(
fp); if( -- width ==  0){ break;} if(( c= _IO_peekc( fp)) ==  - 1){ seen_eof ++;
break;}} nread += n;}} else{ struct _tagged_arr _temp101= Cyc_Std_va_arg_string_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)));
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1);{ struct _tagged_arr
_temp102= _temp101; int c= _IO_peekc( fp); while( ! isspace(( int)((
unsigned char) c))) { c= _IO_getc( fp); if( _get_arr_size( _temp102, sizeof(
unsigned char)) ==  0){ goto eof_failure;}*(( unsigned char*)
_check_unknown_subscript( _temp102, sizeof( unsigned char), 0u))=( unsigned char)
c; _tagged_arr_inplace_plus_post(& _temp102, sizeof( unsigned char), 1); if( --
width ==  0){ break;} if(( c= _IO_peekc( fp)) ==  - 1){ seen_eof ++; break;}}
if( _get_arr_size( _temp102, sizeof( unsigned char)) ==  0){ goto eof_failure;}*((
unsigned char*) _check_unknown_subscript( _temp102, sizeof( unsigned char), 0u))='\000';
nread +=( _temp102.curr -  _temp101.curr) /  sizeof( unsigned char); nassigned
++;}} continue; case 3: _LL100: if( width ==  0? 1: width >  sizeof( buf) -  1){
width=( long long)( sizeof( buf) -  1);} flags |=( 64 |  128) |  512; for( p=
_tag_arr( buf, sizeof( unsigned char), 350u); width !=  0; width --){ c=( int)((
unsigned char) _IO_peekc( fp)); switch( c){ case '0': _LL104: if( base ==  0){
base= 8; flags |= 256;} if( flags &  512){ flags &= ~(( 64 |  512) |  128);}
else{ flags &= ~(( 64 |  256) |  128);} goto ok; case '1': _LL105: goto _LL106;
case '2': _LL106: goto _LL107; case '3': _LL107: goto _LL108; case '4': _LL108:
goto _LL109; case '5': _LL109: goto _LL110; case '6': _LL110: goto _LL111; case
'7': _LL111: base=( int) basefix[ _check_known_subscript_notnull( 17u, base)];
flags &= ~(( 64 |  256) |  128); goto ok; case '8': _LL112: goto _LL113; case '9':
_LL113: base=( int) basefix[ _check_known_subscript_notnull( 17u, base)]; if(
base <=  8){ break;} flags &= ~(( 64 |  256) |  128); goto ok; case 'A': _LL114:
goto _LL115; case 'B': _LL115: goto _LL116; case 'C': _LL116: goto _LL117; case
'D': _LL117: goto _LL118; case 'E': _LL118: goto _LL119; case 'F': _LL119: goto
_LL120; case 'a': _LL120: goto _LL121; case 'b': _LL121: goto _LL122; case 'c':
_LL122: goto _LL123; case 'd': _LL123: goto _LL124; case 'e': _LL124: goto
_LL125; case 'f': _LL125: if( base <=  10){ break;} flags &= ~(( 64 |  256) | 
128); goto ok; case '+': _LL126: goto _LL127; case '-': _LL127: if( flags &  64){
flags &= ~ 64; goto ok;} break; case 'x': _LL128: goto _LL129; case 'X': _LL129:
if( flags &  256? p.curr == ( _tagged_arr_plus( _tag_arr( buf, sizeof(
unsigned char), 350u), sizeof( unsigned char), 1)).curr: 0){ base= 16; flags &=
~ 256; goto ok;} break; default: _LL130: break;} break; ok:*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& p, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))=( unsigned char) c; _IO_getc(
fp); if( _IO_peekc( fp) ==  - 1){ seen_eof ++; break;}} if( flags &  128){ if( p.curr
> ( _tag_arr( buf, sizeof( unsigned char), 350u)).curr){
_tagged_arr_inplace_plus(& p, sizeof( unsigned char), -1); _IO_ungetc(( int)*((
unsigned char*) _check_unknown_subscript( p, sizeof( unsigned char), 0u)), fp);}
goto match_failure;} c=( int)*(( unsigned char*) _check_unknown_subscript( p,
sizeof( unsigned char), -1)); if( c == 'x'? 1: c == 'X'){
_tagged_arr_inplace_plus(& p, sizeof( unsigned char), -1); _IO_ungetc( c, fp);}
if(( flags &  8) ==  0){ unsigned int res;*(( unsigned char*)
_check_unknown_subscript( p, sizeof( unsigned char), 0u))='\000'; if(
use_strtoul){ res= Cyc_Std_strtoul( _tag_arr( buf, sizeof( unsigned char), 350u),
0, base);} else{ res=( unsigned int) Cyc_Std_strtol( _tag_arr( buf, sizeof(
unsigned char), 350u), 0, base);} if( flags &  16){* Cyc_Std_va_arg_int_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)))=( int) res;} else{
if( flags &  4){* Cyc_Std_va_arg_short_ptr(*(( void**) _check_unknown_subscript(
ap, sizeof( void*), 0u)))=( short) res;} else{ if( flags &  1){* Cyc_Std_va_arg_int_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)))=( int) res;} else{*
Cyc_Std_va_arg_int_ptr(*(( void**) _check_unknown_subscript( ap, sizeof( void*),
0u)))=( int) res;}}} _tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1);
nassigned ++;} nread +=( p.curr - ( _tag_arr( buf, sizeof( unsigned char), 350u)).curr)
/  sizeof( unsigned char); break; case 4: _LL103: if( width ==  0? 1: width > 
sizeof( buf) -  1){ width=( long long)( sizeof( buf) -  1);} flags |=(( 64 | 
128) |  256) |  512; for( p= _tag_arr( buf, sizeof( unsigned char), 350u); width
!=  0; width --){ c= _IO_peekc( fp); switch( c){ case '0': _LL133: goto _LL134;
case '1': _LL134: goto _LL135; case '2': _LL135: goto _LL136; case '3': _LL136:
goto _LL137; case '4': _LL137: goto _LL138; case '5': _LL138: goto _LL139; case
'6': _LL139: goto _LL140; case '7': _LL140: goto _LL141; case '8': _LL141: goto
_LL142; case '9': _LL142: flags &= ~( 64 |  128); goto fok; case '+': _LL143:
goto _LL144; case '-': _LL144: if( flags &  64){ flags &= ~ 64; goto fok;}
break; case '.': _LL145: if( flags &  256){ flags &= ~( 64 |  256); goto fok;}
break; case 'e': _LL146: goto _LL147; case 'E': _LL147: if(( flags & ( 128 | 
512)) ==  512){ flags=( flags &  ~( 512 |  256) |  64) |  128; goto fok;} break;
default: _LL148: break;} break; fok:*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& p, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))=( unsigned char) c; _IO_getc( fp); if( _IO_peekc( fp) ==  -
1){ seen_eof ++; break;}} if( flags &  128){ if( flags &  512){ while( p.curr > (
_tag_arr( buf, sizeof( unsigned char), 350u)).curr) { _tagged_arr_inplace_plus(&
p, sizeof( unsigned char), -1); _IO_ungetc(( int)*(( unsigned char*)
_check_unknown_subscript( p, sizeof( unsigned char), 0u)), fp);} goto
match_failure;} _tagged_arr_inplace_plus(& p, sizeof( unsigned char), -1); c=(
int)*(( unsigned char*) _check_unknown_subscript( p, sizeof( unsigned char), 0u));
if( c != 'e'? c != 'E': 0){ _IO_ungetc( c, fp); _tagged_arr_inplace_plus(& p,
sizeof( unsigned char), -1); c=( int)*(( unsigned char*)
_check_unknown_subscript( p, sizeof( unsigned char), 0u));} _IO_ungetc( c, fp);}
if(( flags &  8) ==  0){ double res;*(( unsigned char*) _check_unknown_subscript(
p, sizeof( unsigned char), 0u))='\000'; res= Cyc_Std_atof( _tag_arr( buf,
sizeof( unsigned char), 350u)); if( flags &  1){* Cyc_Std_va_arg_double_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)))= res;} else{* Cyc_Std_va_arg_float_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)))=( float) res;}
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); nassigned ++;} nread +=(
p.curr - ( _tag_arr( buf, sizeof( unsigned char), 350u)).curr) /  sizeof(
unsigned char); break; default: _LL132:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp151=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp151[ 0]=({ struct Cyc_Core_Impossible_struct _temp152; _temp152.tag= Cyc_Core_Impossible;
_temp152.f1= _tag_arr("scanf3", sizeof( unsigned char), 7u); _temp152;});
_temp151;}));}} eof_failure: seen_eof ++; input_failure: if( nassigned ==  0){
nassigned= - 1;} control_failure: match_failure: if(( unsigned int) errp){* errp
|= 2;} done: if(( unsigned int) errp? seen_eof: 0){* errp |= 1;} return
nassigned;} static struct _tagged_arr Cyc_Std___sccl( struct _tagged_arr tab,
struct _tagged_arr fmt){ int c; int n; int v; c=( int)*(( const unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& fmt, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u)); if( c == '^'){ v= 1; c=( int)*((
const unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(&
fmt, sizeof( unsigned char), 1), sizeof( unsigned char), 0u));} else{ v= 0;}
for( n= 0; n <  256; n ++){*(( unsigned char*) _check_unknown_subscript( tab,
sizeof( unsigned char), n))=( unsigned char) v;} if( c ==  0){ return
_tagged_arr_plus( fmt, sizeof( unsigned char), - 1);} v= 1 -  v; for( 0; 1; 0){*((
unsigned char*) _check_unknown_subscript( tab, sizeof( unsigned char), c))=(
unsigned char) v; doswitch: n=( int)*(( const unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& fmt, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u)); switch( n){ case 0: _LL153:
return _tagged_arr_plus( fmt, sizeof( unsigned char), - 1); case '-': _LL154: n=(
int)*(( const unsigned char*) _check_unknown_subscript( fmt, sizeof(
unsigned char), 0u)); if( n == ']'? 1: n <  c){ c=( int)'-'; break;}
_tagged_arr_inplace_plus_post(& fmt, sizeof( unsigned char), 1); do {*((
unsigned char*) _check_unknown_subscript( tab, sizeof( unsigned char), ++ c))=(
unsigned char) v;} while ( c <  n); goto doswitch; break; case ']': _LL155:
return fmt; default: _LL156: c= n; break;}}} static int Cyc_Std_string_getc(
struct _tagged_arr* sptr){ unsigned char c; struct _tagged_arr s=* sptr; if(( s.curr
== (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1: _get_arr_size( s,
sizeof( unsigned char)) ==  0)? 1:( c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0))) == '\000'){ return - 1;}*
sptr= _tagged_arr_plus( s, sizeof( unsigned char), 1); return( int) c;} static
int Cyc_Std_string_ungetc( int ignore, struct _tagged_arr* sptr){* sptr=
_tagged_arr_plus(* sptr, sizeof( unsigned char), - 1); return 0;} static int Cyc_Std_string_peekc(
struct _tagged_arr* sptr){ unsigned char c; struct _tagged_arr s=* sptr; if(( s.curr
== (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1: _get_arr_size( s,
sizeof( unsigned char)) ==  0)? 1:( c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0))) == '\000'){ return - 1;}
return( int) c;} int Cyc_Std_vsscanf( struct _tagged_arr src, struct _tagged_arr
fmt, struct _tagged_arr ap){ int err= 0; int(* sgc)( struct _tagged_arr*)= Cyc_Std_string_getc;
return(( int(*)( int(* _IO_getc)( struct _tagged_arr*), int(* _IO_ungetc)( int,
struct _tagged_arr*), int(* _IO_peekc)( struct _tagged_arr*), struct _tagged_arr*
fp, struct _tagged_arr fmt0, struct _tagged_arr ap, int* errp)) Cyc_Std__IO_vfscanf)(
Cyc_Std_string_getc, Cyc_Std_string_ungetc, Cyc_Std_string_peekc,& src, fmt, ap,(
int*)& err);} int Cyc_Std_sscanf( struct _tagged_arr src, struct _tagged_arr fmt,
struct _tagged_arr ap){ return Cyc_Std_vsscanf( src, fmt, ap);} int Cyc_Std_peekc(
struct Cyc_Std___sFILE* stream){ int c= Cyc_Std_fgetc( stream); Cyc_Std_ungetc(
c, stream); return c;} int Cyc_Std_vfscanf( struct Cyc_Std___sFILE* stream,
struct _tagged_arr fmt, struct _tagged_arr ap){ int err= 0; return(( int(*)( int(*
_IO_getc)( struct Cyc_Std___sFILE*), int(* _IO_ungetc)( int, struct Cyc_Std___sFILE*),
int(* _IO_peekc)( struct Cyc_Std___sFILE*), struct Cyc_Std___sFILE* fp, struct
_tagged_arr fmt0, struct _tagged_arr ap, int* errp)) Cyc_Std__IO_vfscanf)( Cyc_Std_getc,
Cyc_Std_ungetc, Cyc_Std_peekc, stream, fmt, ap,( int*)& err);} int Cyc_Std_fscanf(
struct Cyc_Std___sFILE* stream, struct _tagged_arr fmt, struct _tagged_arr ap){
return Cyc_Std_vfscanf( stream, fmt, ap);} int Cyc_Std_scanf( struct _tagged_arr
fmt, struct _tagged_arr ap){ return Cyc_Std_vfscanf( Cyc_Std_stdin, fmt, ap);}
