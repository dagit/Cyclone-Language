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
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE*
Cyc_Std_stderr; extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern
unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int Cyc_Std_sscanf( struct _tagged_arr src,
struct _tagged_arr fmt, struct _tagged_arr); struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_mismatch[ 18u];
extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned int Cyc_Std_strlen(
struct _tagged_arr s); extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern int Cyc_Std_strncmp( struct _tagged_arr s1, struct
_tagged_arr s2, unsigned int len); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern unsigned char Cyc_Arg_Bad[ 8u];
struct Cyc_Arg_Bad_struct{ unsigned char* tag; struct _tagged_arr f1; } ; extern
unsigned char Cyc_Arg_Error[ 10u]; static const int Cyc_Arg_Unit_spec= 0; struct
Cyc_Arg_Unit_spec_struct{ int tag; void(* f1)(); } ; static const int Cyc_Arg_Flag_spec=
1; struct Cyc_Arg_Flag_spec_struct{ int tag; void(* f1)( struct _tagged_arr); }
; static const int Cyc_Arg_FlagString_spec= 2; struct Cyc_Arg_FlagString_spec_struct{
int tag; void(* f1)( struct _tagged_arr, struct _tagged_arr); } ; static const
int Cyc_Arg_Set_spec= 3; struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ;
static const int Cyc_Arg_Clear_spec= 4; struct Cyc_Arg_Clear_spec_struct{ int
tag; int* f1; } ; static const int Cyc_Arg_String_spec= 5; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; static const int Cyc_Arg_Int_spec=
6; struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; static const
int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{ int tag; void(* f1)(
struct _tagged_arr); } ; extern void Cyc_Arg_usage( struct Cyc_List_List*,
struct _tagged_arr); extern int Cyc_Arg_current; extern void Cyc_Arg_parse(
struct Cyc_List_List* specs, void(* anonfun)( struct _tagged_arr), struct
_tagged_arr errmsg, struct _tagged_arr args); struct Cyc_Buffer_t; extern struct
Cyc_Buffer_t* Cyc_Buffer_create( unsigned int n); extern struct _tagged_arr Cyc_Buffer_contents(
struct Cyc_Buffer_t*); extern void Cyc_Buffer_add_substring( struct Cyc_Buffer_t*,
struct _tagged_arr, int offset, int len); extern void Cyc_Buffer_add_string(
struct Cyc_Buffer_t*, struct _tagged_arr); extern int isalnum( int); extern int
isalpha( int); extern int isascii( int); extern int iscntrl( int); extern int
isdigit( int); extern int isgraph( int); extern int islower( int); extern int
isprint( int); extern int ispunct( int); extern int isspace( int); extern int
isupper( int); extern int isxdigit( int); extern int toascii( int); extern int
tolower( int); extern int toupper( int); extern int _tolower( int); extern int
_toupper( int); unsigned char Cyc_Arg_Bad[ 8u]="\000\000\000\000Bad";
unsigned char Cyc_Arg_Error[ 10u]="\000\000\000\000Error"; static const int Cyc_Arg_Prefix=
0; struct Cyc_Arg_Prefix_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Arg_Exact= 1; struct Cyc_Arg_Exact_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Arg_Unknown= 0; struct Cyc_Arg_Unknown_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Arg_Missing= 1; struct
Cyc_Arg_Missing_struct{ int tag; struct _tagged_arr f1; } ; static const int Cyc_Arg_Message=
2; struct Cyc_Arg_Message_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Arg_Wrong= 3; struct Cyc_Arg_Wrong_struct{ int tag; struct
_tagged_arr f1; struct _tagged_arr f2; struct _tagged_arr f3; } ; struct _tuple0{
struct _tagged_arr f1; int f2; struct _tagged_arr f3; void* f4; struct
_tagged_arr f5; } ; static void* Cyc_Arg_lookup( struct Cyc_List_List* l, struct
_tagged_arr x){ while( l !=  0) { struct _tagged_arr _temp0=(*(( struct _tuple0*)
l->hd)).f1; unsigned int _temp1= Cyc_Std_strlen( _temp0); if( _temp1 >  0?(*((
struct _tuple0*) l->hd)).f2: 0){ if( Cyc_Std_strncmp( x,(*(( struct _tuple0*) l->hd)).f1,
_temp1) ==  0){ return(*(( struct _tuple0*) l->hd)).f4;}} else{ if( Cyc_Std_strcmp(
x,(*(( struct _tuple0*) l->hd)).f1) ==  0){ return(*(( struct _tuple0*) l->hd)).f4;}}
l= l->tl;}( int) _throw(( void*) Cyc_Core_Not_found);} static struct _tagged_arr
Cyc_Arg_Justify_break_line( struct Cyc_Buffer_t* b, int howmuch, struct
_tagged_arr s){ if( s.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
return( struct _tagged_arr) _tag_arr( 0u, 0u, 0u);} if( howmuch <  0){ howmuch=
0;}{ unsigned int _temp2= Cyc_Std_strlen( s); if( howmuch >  _temp2){ Cyc_Buffer_add_string(
b, s); return( struct _tagged_arr) _tag_arr( 0u, 0u, 0u);}{ int i; for( i=
howmuch -  1; i >=  0? ! isspace(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i))): 0; i --){;} if( i < 
0){ for( i= howmuch? howmuch -  1: 0;( i <  _temp2?( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)): 0)? ! isspace(( int)*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))):
0; i ++){;}} Cyc_Buffer_add_substring( b, s, 0, i);{ struct _tagged_arr
whatsleft=( struct _tagged_arr) _tag_arr( 0u, 0u, 0u); for( 0;( i <  _temp2?(
int)*(( const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
i)): 0)? isspace(( int)*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i))): 0; i ++){;} if( i <  _temp2?( int)*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)): 0){
whatsleft= _tagged_arr_plus( s, sizeof( unsigned char), i);} return whatsleft;}}}}
void Cyc_Arg_Justify_justify_b( struct Cyc_Buffer_t* b, int indent, int margin,
struct _tagged_arr item, struct _tagged_arr desc){ if( item.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Buffer_add_string( b, item);} if(
desc.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ return;} if(
indent <  0){ indent= 0;} if( margin <  0){ margin= 0;}{ struct _tagged_arr
indentstr=({ unsigned int _temp8=( unsigned int)( indent +  1); unsigned char*
_temp9=( unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char),
_temp8)); struct _tagged_arr _temp11= _tag_arr( _temp9, sizeof( unsigned char),(
unsigned int)( indent +  1));{ unsigned int _temp10= _temp8; unsigned int i;
for( i= 0; i <  _temp10; i ++){ _temp9[ i]= i ==  0?'\n':' ';}}; _temp11;});
unsigned int _temp3= Cyc_Std_strlen( item); struct _tagged_arr itemsep; if( Cyc_Std_strlen(
desc) >  0){ if( _temp3 +  1 >  indent){ itemsep= indentstr;} else{ itemsep=({
unsigned int _temp4= indent -  _temp3; unsigned char* _temp5=( unsigned char*)
_cycalloc_atomic( _check_times( sizeof( unsigned char), _temp4)); struct
_tagged_arr _temp7= _tag_arr( _temp5, sizeof( unsigned char), indent -  _temp3);{
unsigned int _temp6= _temp4; unsigned int i; for( i= 0; i <  _temp6; i ++){
_temp5[ i]=' ';}}; _temp7;});}} else{ return;} Cyc_Buffer_add_string( b, itemsep);
while( desc.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr) { desc=
Cyc_Arg_Justify_break_line( b, margin -  indent, desc); if( desc.curr != ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Buffer_add_string( b,
indentstr);} else{ Cyc_Buffer_add_string( b, _tag_arr("\n", sizeof(
unsigned char), 2u));}} return;}} void Cyc_Arg_usage( struct Cyc_List_List*
speclist, struct _tagged_arr errmsg){({ struct Cyc_Std_String_pa_struct _temp13;
_temp13.tag= Cyc_Std_String_pa; _temp13.f1=( struct _tagged_arr) errmsg;{ void*
_temp12[ 1u]={& _temp13}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s\n",
sizeof( unsigned char), 4u), _tag_arr( _temp12, sizeof( void*), 1u));}});{
struct Cyc_Buffer_t* _temp14= Cyc_Buffer_create( 1024); while( speclist !=  0) {
Cyc_Arg_Justify_justify_b( _temp14, 12, 72,( struct _tagged_arr) Cyc_Std_strconcat((*((
struct _tuple0*) speclist->hd)).f1,(*(( struct _tuple0*) speclist->hd)).f3),(*((
struct _tuple0*) speclist->hd)).f5); speclist= speclist->tl;}({ struct Cyc_Std_String_pa_struct
_temp16; _temp16.tag= Cyc_Std_String_pa; _temp16.f1=( struct _tagged_arr) Cyc_Buffer_contents(
_temp14);{ void* _temp15[ 1u]={& _temp16}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp15, sizeof( void*), 1u));}});}}
int Cyc_Arg_current= 0; static struct _tagged_arr Cyc_Arg_args={( void*) 0u,(
void*) 0u,( void*)( 0u +  0u)}; static void Cyc_Arg_stop( int prog_pos, void* e,
struct Cyc_List_List* speclist, struct _tagged_arr errmsg){ struct _tagged_arr
progname= prog_pos <  _get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr))?*((
struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args, sizeof( struct
_tagged_arr), prog_pos)): _tag_arr("(?)", sizeof( unsigned char), 4u);{ void*
_temp17= e; struct _tagged_arr _temp27; struct _tagged_arr _temp29; struct
_tagged_arr _temp31; struct _tagged_arr _temp33; struct _tagged_arr _temp35;
struct _tagged_arr _temp37; _LL19: if(*(( int*) _temp17) ==  Cyc_Arg_Unknown){
_LL28: _temp27=(( struct Cyc_Arg_Unknown_struct*) _temp17)->f1; goto _LL20;}
else{ goto _LL21;} _LL21: if(*(( int*) _temp17) ==  Cyc_Arg_Missing){ _LL30:
_temp29=(( struct Cyc_Arg_Missing_struct*) _temp17)->f1; goto _LL22;} else{ goto
_LL23;} _LL23: if(*(( int*) _temp17) ==  Cyc_Arg_Wrong){ _LL36: _temp35=((
struct Cyc_Arg_Wrong_struct*) _temp17)->f1; goto _LL34; _LL34: _temp33=(( struct
Cyc_Arg_Wrong_struct*) _temp17)->f2; goto _LL32; _LL32: _temp31=(( struct Cyc_Arg_Wrong_struct*)
_temp17)->f3; goto _LL24;} else{ goto _LL25;} _LL25: if(*(( int*) _temp17) == 
Cyc_Arg_Message){ _LL38: _temp37=(( struct Cyc_Arg_Message_struct*) _temp17)->f1;
goto _LL26;} else{ goto _LL18;} _LL20: if( Cyc_Std_strcmp( _temp27, _tag_arr("-help",
sizeof( unsigned char), 6u)) !=  0){({ struct Cyc_Std_String_pa_struct _temp41;
_temp41.tag= Cyc_Std_String_pa; _temp41.f1=( struct _tagged_arr) _temp27;{
struct Cyc_Std_String_pa_struct _temp40; _temp40.tag= Cyc_Std_String_pa; _temp40.f1=(
struct _tagged_arr) progname;{ void* _temp39[ 2u]={& _temp40,& _temp41}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: unknown option `%s'.\n", sizeof( unsigned char),
26u), _tag_arr( _temp39, sizeof( void*), 2u));}}});} goto _LL18; _LL22:({ struct
Cyc_Std_String_pa_struct _temp44; _temp44.tag= Cyc_Std_String_pa; _temp44.f1=(
struct _tagged_arr) _temp29;{ struct Cyc_Std_String_pa_struct _temp43; _temp43.tag=
Cyc_Std_String_pa; _temp43.f1=( struct _tagged_arr) progname;{ void* _temp42[ 2u]={&
_temp43,& _temp44}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s: option `%s' needs an argument.\n",
sizeof( unsigned char), 36u), _tag_arr( _temp42, sizeof( void*), 2u));}}}); goto
_LL18; _LL24:({ struct Cyc_Std_String_pa_struct _temp49; _temp49.tag= Cyc_Std_String_pa;
_temp49.f1=( struct _tagged_arr) _temp31;{ struct Cyc_Std_String_pa_struct
_temp48; _temp48.tag= Cyc_Std_String_pa; _temp48.f1=( struct _tagged_arr)
_temp35;{ struct Cyc_Std_String_pa_struct _temp47; _temp47.tag= Cyc_Std_String_pa;
_temp47.f1=( struct _tagged_arr) _temp33;{ struct Cyc_Std_String_pa_struct
_temp46; _temp46.tag= Cyc_Std_String_pa; _temp46.f1=( struct _tagged_arr)
progname;{ void* _temp45[ 4u]={& _temp46,& _temp47,& _temp48,& _temp49}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: wrong argument `%s'; option `%s' expects %s.\n",
sizeof( unsigned char), 50u), _tag_arr( _temp45, sizeof( void*), 4u));}}}}});
goto _LL18; _LL26:({ struct Cyc_Std_String_pa_struct _temp52; _temp52.tag= Cyc_Std_String_pa;
_temp52.f1=( struct _tagged_arr) _temp37;{ struct Cyc_Std_String_pa_struct
_temp51; _temp51.tag= Cyc_Std_String_pa; _temp51.f1=( struct _tagged_arr)
progname;{ void* _temp50[ 2u]={& _temp51,& _temp52}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: %s.\n", sizeof( unsigned char), 9u), _tag_arr( _temp50, sizeof(
void*), 2u));}}}); goto _LL18; _LL18:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=(
int) _get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr));} void Cyc_Arg_parse(
struct Cyc_List_List* speclist, void(* anonfun)( struct _tagged_arr), struct
_tagged_arr errmsg, struct _tagged_arr orig_args){ Cyc_Arg_args= orig_args;{ int
initpos= Cyc_Arg_current; int l=( int) _get_arr_size( Cyc_Arg_args, sizeof(
struct _tagged_arr)); ++ Cyc_Arg_current; while( Cyc_Arg_current <  l) { struct
_tagged_arr s=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current)); if( _get_arr_size( s, sizeof(
unsigned char)) >=  1?*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), 0)) == '-': 0){ void* action;{ struct _handler_cons
_temp53; _push_handler(& _temp53);{ int _temp55= 0; if( setjmp( _temp53.handler)){
_temp55= 1;} if( ! _temp55){ action= Cyc_Arg_lookup( speclist, s);; _pop_handler();}
else{ void* _temp54=( void*) _exn_thrown; void* _temp57= _temp54; _LL59: if(
_temp57 ==  Cyc_Core_Not_found){ goto _LL60;} else{ goto _LL61;} _LL61: goto
_LL62; _LL60: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Unknown_struct*
_temp63=( struct Cyc_Arg_Unknown_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unknown_struct));
_temp63[ 0]=({ struct Cyc_Arg_Unknown_struct _temp64; _temp64.tag= Cyc_Arg_Unknown;
_temp64.f1= s; _temp64;}); _temp63;}), speclist, errmsg); return; _LL62:( void)
_throw( _temp57); _LL58:;}}}{ struct _handler_cons _temp65; _push_handler(&
_temp65);{ int _temp67= 0; if( setjmp( _temp65.handler)){ _temp67= 1;} if( !
_temp67){{ void* _temp68= action; void(* _temp86)(); void(* _temp88)( struct
_tagged_arr); int* _temp90; int* _temp92; void(* _temp94)( struct _tagged_arr,
struct _tagged_arr); void(* _temp96)( struct _tagged_arr); void(* _temp98)( int);
void(* _temp100)( struct _tagged_arr); _LL70: if(*(( int*) _temp68) ==  Cyc_Arg_Unit_spec){
_LL87: _temp86=(( struct Cyc_Arg_Unit_spec_struct*) _temp68)->f1; goto _LL71;}
else{ goto _LL72;} _LL72: if(*(( int*) _temp68) ==  Cyc_Arg_Flag_spec){ _LL89:
_temp88=(( struct Cyc_Arg_Flag_spec_struct*) _temp68)->f1; goto _LL73;} else{
goto _LL74;} _LL74: if(*(( int*) _temp68) ==  Cyc_Arg_Set_spec){ _LL91: _temp90=((
struct Cyc_Arg_Set_spec_struct*) _temp68)->f1; goto _LL75;} else{ goto _LL76;}
_LL76: if(*(( int*) _temp68) ==  Cyc_Arg_Clear_spec){ _LL93: _temp92=(( struct
Cyc_Arg_Clear_spec_struct*) _temp68)->f1; goto _LL77;} else{ goto _LL78;} _LL78:
if(*(( int*) _temp68) ==  Cyc_Arg_FlagString_spec){ _LL95: _temp94=(( struct Cyc_Arg_FlagString_spec_struct*)
_temp68)->f1; goto _LL79;} else{ goto _LL80;} _LL80: if(*(( int*) _temp68) == 
Cyc_Arg_String_spec){ _LL97: _temp96=(( struct Cyc_Arg_String_spec_struct*)
_temp68)->f1; goto _LL81;} else{ goto _LL82;} _LL82: if(*(( int*) _temp68) == 
Cyc_Arg_Int_spec){ _LL99: _temp98=(( struct Cyc_Arg_Int_spec_struct*) _temp68)->f1;
goto _LL83;} else{ goto _LL84;} _LL84: if(*(( int*) _temp68) ==  Cyc_Arg_Rest_spec){
_LL101: _temp100=(( struct Cyc_Arg_Rest_spec_struct*) _temp68)->f1; goto _LL85;}
else{ goto _LL69;} _LL71: _temp86(); goto _LL69; _LL73: _temp88( s); goto _LL69;
_LL75:* _temp90= 1; goto _LL69; _LL77:* _temp92= 0; goto _LL69; _LL79: if( Cyc_Arg_current
+  1 <  l){ _temp94( s,*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current +  1))); ++ Cyc_Arg_current;} else{
Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct* _temp102=(
struct Cyc_Arg_Missing_struct*) _cycalloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp102[ 0]=({ struct Cyc_Arg_Missing_struct _temp103; _temp103.tag= Cyc_Arg_Missing;
_temp103.f1= s; _temp103;}); _temp102;}), speclist, errmsg);} goto _LL69; _LL81:
if( Cyc_Arg_current +  1 <  l){ _temp96(*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+  1))); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct*
_temp104=( struct Cyc_Arg_Missing_struct*) _cycalloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp104[ 0]=({ struct Cyc_Arg_Missing_struct _temp105; _temp105.tag= Cyc_Arg_Missing;
_temp105.f1= s; _temp105;}); _temp104;}), speclist, errmsg);} goto _LL69; _LL83: {
struct _tagged_arr arg=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current +  1)); int n= 0; if(({ struct Cyc_Std_IntPtr_sa_struct
_temp107; _temp107.tag= Cyc_Std_IntPtr_sa; _temp107.f1=& n;{ void* _temp106[ 1u]={&
_temp107}; Cyc_Std_sscanf( arg, _tag_arr("%d", sizeof( unsigned char), 3u),
_tag_arr( _temp106, sizeof( void*), 1u));}}) !=  1){ Cyc_Arg_stop( initpos,(
void*)({ struct Cyc_Arg_Wrong_struct* _temp108=( struct Cyc_Arg_Wrong_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Wrong_struct)); _temp108[ 0]=({ struct Cyc_Arg_Wrong_struct
_temp109; _temp109.tag= Cyc_Arg_Wrong; _temp109.f1= s; _temp109.f2= arg;
_temp109.f3= _tag_arr("an integer", sizeof( unsigned char), 11u); _temp109;});
_temp108;}), speclist, errmsg); _npop_handler( 0u); return;} _temp98( n); ++ Cyc_Arg_current;
goto _LL69;} _LL85: while( Cyc_Arg_current <  l -  1) { _temp100(*(( struct
_tagged_arr*) _check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr),
Cyc_Arg_current +  1))); ++ Cyc_Arg_current;} goto _LL69; _LL69:;}; _pop_handler();}
else{ void* _temp66=( void*) _exn_thrown; void* _temp111= _temp66; struct
_tagged_arr _temp117; _LL113: if(*(( void**) _temp111) ==  Cyc_Arg_Bad){ _LL118:
_temp117=(( struct Cyc_Arg_Bad_struct*) _temp111)->f1; goto _LL114;} else{ goto
_LL115;} _LL115: goto _LL116; _LL114: Cyc_Arg_stop( initpos,( void*)({ struct
Cyc_Arg_Message_struct* _temp119=( struct Cyc_Arg_Message_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Message_struct)); _temp119[ 0]=({ struct Cyc_Arg_Message_struct
_temp120; _temp120.tag= Cyc_Arg_Message; _temp120.f1= _temp117; _temp120;});
_temp119;}), speclist, errmsg); goto _LL112; _LL116:( void) _throw( _temp111);
_LL112:;}}} ++ Cyc_Arg_current;} else{{ struct _handler_cons _temp121;
_push_handler(& _temp121);{ int _temp123= 0; if( setjmp( _temp121.handler)){
_temp123= 1;} if( ! _temp123){ anonfun( s);; _pop_handler();} else{ void*
_temp122=( void*) _exn_thrown; void* _temp125= _temp122; struct _tagged_arr
_temp131; _LL127: if(*(( void**) _temp125) ==  Cyc_Arg_Bad){ _LL132: _temp131=((
struct Cyc_Arg_Bad_struct*) _temp125)->f1; goto _LL128;} else{ goto _LL129;}
_LL129: goto _LL130; _LL128: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Message_struct*
_temp133=( struct Cyc_Arg_Message_struct*) _cycalloc( sizeof( struct Cyc_Arg_Message_struct));
_temp133[ 0]=({ struct Cyc_Arg_Message_struct _temp134; _temp134.tag= Cyc_Arg_Message;
_temp134.f1= _temp131; _temp134;}); _temp133;}), speclist, errmsg); goto _LL126;
_LL130:( void) _throw( _temp125); _LL126:;}}} ++ Cyc_Arg_current;}}}}
