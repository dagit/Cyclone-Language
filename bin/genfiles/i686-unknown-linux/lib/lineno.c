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
extern struct _tagged_arr Cyc_Core_new_string( int); extern int Cyc_Core_intcmp(
int, int); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u];
extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct Cyc_Std___sFILE*
Cyc_Std_file_open( struct _tagged_arr fname, struct _tagged_arr mode); extern
void Cyc_Std_file_close( struct Cyc_Std___sFILE*); static const int Cyc_Std_String_pa=
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
int tag; float* f1; } ; extern int Cyc_Std_sscanf( struct _tagged_arr src,
struct _tagged_arr fmt, struct _tagged_arr); extern unsigned char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_arr lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{
int(* read_fun)( struct _tagged_arr, int, void*); void* read_fun_state; } ;
struct Cyc_Lexing_lex_tables{ struct _tagged_arr lex_base; struct _tagged_arr
lex_backtrk; struct _tagged_arr lex_default; struct _tagged_arr lex_trans;
struct _tagged_arr lex_check; } ; extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file(
struct Cyc_Std___sFILE*); extern struct _tagged_arr Cyc_Lexing_lexeme( struct
Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_merge_sort(
int(* cmp)( void*, void*), struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[
8u]; extern unsigned int Cyc_Std_strlen( struct _tagged_arr s); extern struct
_tagged_arr Cyc_Std_strdup( struct _tagged_arr src); extern struct _tagged_arr
Cyc_Std_substring( struct _tagged_arr, int ofs, unsigned int n); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs( struct _tagged_arr, int);
extern void Cyc_Lineno_poss_of_abss( struct _tagged_arr filename, struct Cyc_List_List*
places); static const int Cyc_Lineno_NEWLINE= 0; static const int Cyc_Lineno_LINEDEF=
1; static const int Cyc_Lineno_END= 2; const int Cyc_Lineno_lex_base[ 8u]=(
const int[ 8u]){ 0, 1, - 2, 2, 5, - 3, - 1, 6}; const int Cyc_Lineno_lex_backtrk[
8u]=( const int[ 8u]){ - 1, - 1, - 1, 1, - 1, - 1, - 1, 0}; const int Cyc_Lineno_lex_default[
8u]=( const int[ 8u]){ 1, 1, 0, - 1, 4, 0, 0, - 1}; const int Cyc_Lineno_lex_trans[
263u]=( const int[ 263u]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 3, 3, 6, 6, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
5, 0}; const int Cyc_Lineno_lex_check[ 263u]=( const int[ 263u]){ - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 1, 3, 0, 1, 4, 7, - 1, 4, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 1, - 1, - 1, - 1, 4, - 1}; int
Cyc_Lineno_lex_engine( int start_state, struct Cyc_Lexing_lexbuf* lbuf){ int
state; int base; int backtrk; int c; state= start_state; if( state >=  0){ lbuf->lex_last_pos=(
lbuf->lex_start_pos= lbuf->lex_curr_pos); lbuf->lex_last_action= - 1;} else{
state=( - state) -  1;} while( 1) { base= Cyc_Lineno_lex_base[
_check_known_subscript_notnull( 8u, state)]; if( base <  0){ return( - base) - 
1;} backtrk= Cyc_Lineno_lex_backtrk[ _check_known_subscript_notnull( 8u, state)];
if( backtrk >=  0){ lbuf->lex_last_pos= lbuf->lex_curr_pos; lbuf->lex_last_action=
backtrk;} if( lbuf->lex_curr_pos >=  lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){
return( - state) -  1;} else{ c= 256;}} else{ c=( int)*(( unsigned char*)
_check_unknown_subscript( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_curr_pos
++)); if( c ==  - 1){ c= 256;}} if( Cyc_Lineno_lex_check[
_check_known_subscript_notnull( 263u, base +  c)] ==  state){ state= Cyc_Lineno_lex_trans[
_check_known_subscript_notnull( 263u, base +  c)];} else{ state= Cyc_Lineno_lex_default[
_check_known_subscript_notnull( 8u, state)];} if( state <  0){ lbuf->lex_curr_pos=
lbuf->lex_last_pos; if( lbuf->lex_last_action ==  - 1){( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp0=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp0[ 0]=({ struct Cyc_Lexing_Error_struct
_temp1; _temp1.tag= Cyc_Lexing_Error; _temp1.f1= _tag_arr("empty token", sizeof(
unsigned char), 12u); _temp1;}); _temp0;}));} else{ return lbuf->lex_last_action;}}
else{ if( c ==  256){ lbuf->lex_eof_reached= 0;}}}} void* Cyc_Lineno_token_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_Lineno_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL2: return( void*) Cyc_Lineno_LINEDEF;
case 1: _LL3: return( void*) Cyc_Lineno_NEWLINE; case 2: _LL4: return( void*)
Cyc_Lineno_END; default: _LL5:( lexbuf->refill_buff)( lexbuf); return Cyc_Lineno_token_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp7=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp7[ 0]=({ struct Cyc_Lexing_Error_struct _temp8; _temp8.tag= Cyc_Lexing_Error;
_temp8.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp8;}); _temp7;}));} void* Cyc_Lineno_token( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_Lineno_token_rec( lexbuf, 0);} struct Cyc_Lineno_Pos; struct _tuple0{
struct _tagged_arr f1; int f2; } ; static struct Cyc_Core_Opt* Cyc_Lineno_parse_linedef(
struct _tagged_arr line){ struct _handler_cons _temp9; _push_handler(& _temp9);{
int _temp11= 0; if( setjmp( _temp9.handler)){ _temp11= 1;} if( ! _temp11){{ int
i= 0; while( i <  _get_arr_size( line, sizeof( unsigned char))?*(( unsigned char*)
_check_unknown_subscript( line, sizeof( unsigned char), i)) < '0'? 1:*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), i)) > '9':
0) { ++ i;}{ int j= i; while(( j <  _get_arr_size( line, sizeof( unsigned char))?*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), j)) >= '0':
0)?*(( unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), j))
<= '9': 0) { ++ j;} if( i ==  _get_arr_size( line, sizeof( unsigned char))){
struct Cyc_Core_Opt* _temp12= 0; _npop_handler( 0u); return _temp12;}{ int
number= 0; if(({ struct Cyc_Std_IntPtr_sa_struct _temp14; _temp14.tag= Cyc_Std_IntPtr_sa;
_temp14.f1=& number;{ void* _temp13[ 1u]={& _temp14}; Cyc_Std_sscanf(( struct
_tagged_arr) Cyc_Std_substring(( struct _tagged_arr) line, i,( unsigned int)( j
-  i)), _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp13, sizeof(
void*), 1u));}}) !=  1){ struct Cyc_Core_Opt* _temp15= 0; _npop_handler( 0u);
return _temp15;} while( j <  _get_arr_size( line, sizeof( unsigned char))?*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), j)) != '"':
0) { ++ j;}{ int k= ++ j; while( k <  _get_arr_size( line, sizeof( unsigned char))?*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), k)) != '"':
0) { ++ k;} if( j ==  _get_arr_size( line, sizeof( unsigned char))? 1: k == 
_get_arr_size( line, sizeof( unsigned char))){ struct Cyc_Core_Opt* _temp16= 0;
_npop_handler( 0u); return _temp16;}{ struct _tagged_arr fname= Cyc_Std_substring((
struct _tagged_arr) line, j,( unsigned int)( k -  j)); struct Cyc_Core_Opt*
_temp19=({ struct Cyc_Core_Opt* _temp17=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp17->v=( void*)({ struct _tuple0* _temp18=(
struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp18->f1= fname;
_temp18->f2= number; _temp18;}); _temp17;}); _npop_handler( 0u); return _temp19;}}}}};
_pop_handler();} else{ void* _temp10=( void*) _exn_thrown; void* _temp21=
_temp10; _LL23: goto _LL24; _LL25: goto _LL26; _LL24: return 0; _LL26:( void)
_throw( _temp21); _LL22:;}}} struct _tuple1{ int f1; struct Cyc_Lineno_Pos* f2;
} ; int Cyc_Lineno_place_cmp( struct _tuple1* place1, struct _tuple1* place2){
return Cyc_Core_intcmp((* place1).f1,(* place2).f1);} void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename, struct Cyc_List_List* places){ places=(( struct Cyc_List_List*(*)(
int(* cmp)( struct _tuple1*, struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_merge_sort)(
Cyc_Lineno_place_cmp, places);{ struct Cyc_Std___sFILE* f= Cyc_Std_file_open(
filename, _tag_arr("r", sizeof( unsigned char), 2u));{ struct _handler_cons
_temp27; _push_handler(& _temp27);{ int _temp29= 0; if( setjmp( _temp27.handler)){
_temp29= 1;} if( ! _temp29){{ struct Cyc_Lexing_lexbuf* lbuf= Cyc_Lexing_from_file(
f); struct _tagged_arr _temp30= filename; int _temp31= 1; struct _tagged_arr
this_line; int eol; void* next; while( places !=  0) { while( 1) { next=(( void*(*)(
struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_Lineno_token)( lbuf); eol=(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)( lbuf); this_line=(( struct
_tagged_arr(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lbuf); if( next
== ( void*) Cyc_Lineno_END? 1: eol > (*(( struct _tuple1*) places->hd)).f1){
break;} if( next == ( void*) Cyc_Lineno_NEWLINE){ ++ _temp31;} else{ struct Cyc_Core_Opt*
fno= Cyc_Lineno_parse_linedef( this_line); if( fno ==  0){ ++ _temp31;} else{
_temp30=( struct _tagged_arr)(*(( struct _tuple0*) fno->v)).f1; _temp31=(*((
struct _tuple0*) fno->v)).f2;}}} while( places !=  0? next == ( void*) Cyc_Lineno_END?
1: eol > (*(( struct _tuple1*) places->hd)).f1: 0) { struct Cyc_Lineno_Pos*
_temp32=(*(( struct _tuple1*) places->hd)).f2; _temp32->logical_file=( struct
_tagged_arr) Cyc_Std_strdup( _temp30); _temp32->line= this_line; _temp32->line_no=
_temp31; _temp32->col=( int)( Cyc_Std_strlen(( struct _tagged_arr) this_line) - (
eol - (*(( struct _tuple1*) places->hd)).f1)); if( _temp32->col <  0){ _temp32->col=
0;} places= places->tl;} ++ _temp31;}}; _pop_handler();} else{ void* _temp28=(
void*) _exn_thrown; void* _temp34= _temp28; _LL36: goto _LL37; _LL38: goto _LL39;
_LL37: Cyc_Std_file_close( f);( int) _throw( _temp34); _LL39:( void) _throw(
_temp34); _LL35:;}}} Cyc_Std_file_close( f); return;}} struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_of_abs( struct _tagged_arr filename, int abs){ struct Cyc_Lineno_Pos*
ans=({ struct Cyc_Lineno_Pos* _temp42=( struct Cyc_Lineno_Pos*) _cycalloc(
sizeof( struct Cyc_Lineno_Pos)); _temp42->logical_file= _tag_arr("", sizeof(
unsigned char), 1u); _temp42->line= Cyc_Core_new_string( 0); _temp42->line_no= 0;
_temp42->col= 0; _temp42;}); Cyc_Lineno_poss_of_abss( filename,({ struct Cyc_List_List*
_temp40=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp40->hd=( void*)({ struct _tuple1* _temp41=( struct _tuple1*) _cycalloc(
sizeof( struct _tuple1)); _temp41->f1= abs; _temp41->f2= ans; _temp41;});
_temp40->tl= 0; _temp40;})); return ans;}
