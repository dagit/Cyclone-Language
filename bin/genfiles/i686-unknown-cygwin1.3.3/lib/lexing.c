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
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern struct _tagged_arr
Cyc_Core_new_string( unsigned int); extern unsigned char Cyc_Core_Invalid_argument[
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
ntCsl_to_ntsl( unsigned char**); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern int Cyc_Std_file_string_read( struct Cyc_Std___sFILE*
fd, struct _tagged_arr dest, int dest_offset, int max_count); static const int
Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{
int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
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
int tag; float* f1; } ; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_function( int(* read_fun)(
struct _tagged_arr, int, void*), void*); extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file(
struct Cyc_Std___sFILE*); extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_string(
struct _tagged_arr); extern struct _tagged_arr Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*);
extern unsigned char Cyc_Lexing_lexeme_char( struct Cyc_Lexing_lexbuf*, int);
extern int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lexeme_end(
struct Cyc_Lexing_lexbuf*); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; extern struct _tagged_arr Cyc_Std_zstrncpy( struct
_tagged_arr, struct _tagged_arr, unsigned int); extern struct _tagged_arr Cyc_Std_strdup(
struct _tagged_arr src); unsigned char Cyc_Lexing_Error[ 10u]="\000\000\000\000Error";
struct Cyc_Lexing_lexbuf; struct Cyc_Lexing_function_lexbuf_state; struct Cyc_Lexing_lex_tables;
static unsigned char Cyc_Lexing_aux_buffer_v[ 1u]={'\000'}; static struct
_tagged_arr Cyc_Lexing_aux_buffer={( void*)(( unsigned char*) Cyc_Lexing_aux_buffer_v),(
void*)(( unsigned char*) Cyc_Lexing_aux_buffer_v),( void*)(( unsigned char*) Cyc_Lexing_aux_buffer_v
+  1u)}; void Cyc_Lexing_lex_refill( struct Cyc_Lexing_lexbuf* lexbuf){ if(
_get_arr_size( Cyc_Lexing_aux_buffer, sizeof( unsigned char)) ==  1){ Cyc_Lexing_aux_buffer=
Cyc_Core_new_string( 4096);}{ int read=((( struct Cyc_Lexing_function_lexbuf_state*)
lexbuf->refill_state)->read_fun)( Cyc_Lexing_aux_buffer,( int) _get_arr_size(
Cyc_Lexing_aux_buffer, sizeof( unsigned char)),( void*)(( struct Cyc_Lexing_function_lexbuf_state*)
lexbuf->refill_state)->read_fun_state); int n= read >  0? read:(( lexbuf->lex_eof_reached=
1, 0)); if( lexbuf->lex_start_pos <  n){ int oldlen= lexbuf->lex_buffer_len; int
newlen= oldlen *  2; struct _tagged_arr newbuf= Cyc_Core_new_string((
unsigned int)( newlen +  1)); Cyc_Std_zstrncpy( _tagged_arr_plus( newbuf,
sizeof( unsigned char), oldlen),( struct _tagged_arr) lexbuf->lex_buffer,(
unsigned int) oldlen); lexbuf->lex_buffer= newbuf; lexbuf->lex_buffer_len=
newlen; lexbuf->lex_abs_pos= lexbuf->lex_abs_pos -  oldlen; lexbuf->lex_curr_pos=
lexbuf->lex_curr_pos +  oldlen; lexbuf->lex_start_pos= lexbuf->lex_start_pos + 
oldlen; lexbuf->lex_last_pos= lexbuf->lex_last_pos +  oldlen;} Cyc_Std_zstrncpy(
lexbuf->lex_buffer,( struct _tagged_arr) _tagged_arr_plus( lexbuf->lex_buffer,
sizeof( unsigned char), n),( unsigned int)( lexbuf->lex_buffer_len -  n)); Cyc_Std_zstrncpy(
_tagged_arr_plus( _tagged_arr_plus( lexbuf->lex_buffer, sizeof( unsigned char),
lexbuf->lex_buffer_len), sizeof( unsigned char), - n),( struct _tagged_arr) Cyc_Lexing_aux_buffer,(
unsigned int) n); lexbuf->lex_abs_pos= lexbuf->lex_abs_pos +  n; lexbuf->lex_curr_pos=
lexbuf->lex_curr_pos -  n; lexbuf->lex_start_pos= lexbuf->lex_start_pos -  n;
lexbuf->lex_last_pos= lexbuf->lex_last_pos -  n;}} struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_function(
int(* read_fun)( struct _tagged_arr, int, void*), void* read_fun_state){ return({
struct Cyc_Lexing_lexbuf* _temp0=( struct Cyc_Lexing_lexbuf*) _cycalloc( sizeof(
struct Cyc_Lexing_lexbuf)); _temp0->refill_buff= Cyc_Lexing_lex_refill; _temp0->refill_state=(
void*)({ struct Cyc_Lexing_function_lexbuf_state* _temp1=( struct Cyc_Lexing_function_lexbuf_state*)
_cycalloc( sizeof( struct Cyc_Lexing_function_lexbuf_state)); _temp1->read_fun=
read_fun; _temp1->read_fun_state=( void*) read_fun_state; _temp1;}); _temp0->lex_buffer=
Cyc_Core_new_string( 8192); _temp0->lex_buffer_len= 8192; _temp0->lex_abs_pos= -
8192; _temp0->lex_start_pos= 8192; _temp0->lex_curr_pos= 8192; _temp0->lex_last_pos=
8192; _temp0->lex_last_action= 0; _temp0->lex_eof_reached= 0; _temp0;});} int
Cyc_Lexing_read_from_file( struct _tagged_arr aux, int n, struct Cyc_Std___sFILE*
f){ return Cyc_Std_file_string_read( f, aux, 0, n);} struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file( struct Cyc_Std___sFILE* f){ return(( struct Cyc_Lexing_lexbuf*(*)(
int(* read_fun)( struct _tagged_arr, int, struct Cyc_Std___sFILE*), struct Cyc_Std___sFILE*
read_fun_state)) Cyc_Lexing_from_function)( Cyc_Lexing_read_from_file, f);}
static void Cyc_Lexing_set_eof( struct Cyc_Lexing_lexbuf* lbuf){ lbuf->lex_eof_reached=
1;} struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_string( struct _tagged_arr s){
return({ struct Cyc_Lexing_lexbuf* _temp2=( struct Cyc_Lexing_lexbuf*) _cycalloc(
sizeof( struct Cyc_Lexing_lexbuf)); _temp2->refill_buff= Cyc_Lexing_set_eof;
_temp2->refill_state=( void*) 0; _temp2->lex_buffer= Cyc_Std_strdup(( struct
_tagged_arr) s); _temp2->lex_buffer_len=( int) _get_arr_size( s, sizeof(
unsigned char)); _temp2->lex_abs_pos= 0; _temp2->lex_start_pos= 0; _temp2->lex_curr_pos=
0; _temp2->lex_last_pos= 0; _temp2->lex_last_action= 0; _temp2->lex_eof_reached=
1; _temp2;});} struct _tagged_arr Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*
lbuf){ int len= lbuf->lex_curr_pos -  lbuf->lex_start_pos; struct _tagged_arr s=
Cyc_Core_new_string(( unsigned int)( len +  1)); Cyc_Std_zstrncpy( s,( struct
_tagged_arr) _tagged_arr_plus( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_start_pos),(
unsigned int) len);*(( unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), len))='\000'; return s;} unsigned char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf* lbuf, int i){ return*(( unsigned char*)
_check_unknown_subscript( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_start_pos
+  i));} int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf* lbuf){ return
lbuf->lex_abs_pos +  lbuf->lex_start_pos;} int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*
lbuf){ return lbuf->lex_abs_pos +  lbuf->lex_curr_pos;}
