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
extern struct _tagged_arr Cyc_Core_new_string( int); struct _tuple0{ void* f1;
void* f2; } ; extern void* Cyc_Core_fst( struct _tuple0*); extern void* Cyc_Core_snd(
struct _tuple0*); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct
Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct _tagged_arr f1; } ;
extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE*
Cyc_Std_stdout; extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern int Cyc_Std_remove(
struct _tagged_arr); extern int Cyc_Std_fclose( struct Cyc_Std___sFILE*); extern
int Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern struct Cyc_Std___sFILE* Cyc_Std_fopen(
struct _tagged_arr __filename, struct _tagged_arr __modes); extern int Cyc_Std_fputc(
int __c, struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_fputs( struct
_tagged_arr __s, struct Cyc_Std___sFILE* __stream); extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern void Cyc_Std_file_close(
struct Cyc_Std___sFILE*); static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern int Cyc_Std_printf( struct _tagged_arr fmt, struct
_tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt,
struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Std___sFILE*);
extern int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list(
struct _tagged_arr); extern int Cyc_List_length( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern unsigned int Cyc_Std_strlen( struct _tagged_arr s);
extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern
struct _tagged_arr Cyc_Std_strconcat( struct _tagged_arr, struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_strconcat_l( struct Cyc_List_List*); extern
struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List*, struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_substring( struct _tagged_arr, int ofs,
unsigned int n); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename, struct Cyc_List_List* places); extern struct
_tagged_arr Cyc_Filename_chop_extension( struct _tagged_arr); extern
unsigned char Cyc_Position_Exit[ 9u]; extern void Cyc_Position_reset_position(
struct _tagged_arr); struct Cyc_Position_Segment; static const int Cyc_Position_Lex=
0; static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
struct Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; extern unsigned char Cyc_Array_Array_mismatch[ 19u]; struct Cyc_Std__Div{
int quot; int rem; } ; struct Cyc_Std__Ldiv{ int quot; int rem; } ; extern int
abs( int __x); extern int atexit( void(* __func)()); extern struct Cyc_Std__Div
div( int __numer, int __denom); extern struct Cyc_Std__Ldiv ldiv( int __numer,
int __denom); extern int random(); extern void srandom( unsigned int __seed);
extern int rand(); extern void srand( unsigned int __seed); extern int rand_r(
unsigned int* __seed); extern int grantpt( int __fd); extern int unlockpt( int
__fd); extern int Cyc_Std_system( struct _tagged_arr); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; extern unsigned char Cyc_Arg_Bad[ 8u];
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
struct _tagged_arr); } ; extern void Cyc_Arg_parse( struct Cyc_List_List* specs,
void(* anonfun)( struct _tagged_arr), struct _tagged_arr errmsg, struct
_tagged_arr args); static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n=
0; struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; struct _tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref;
static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1;
static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0;
static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const
int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; static const int Cyc_Absyn_Eq_kb= 0; struct Cyc_Absyn_Eq_kb_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_Unknown_kb= 1; struct Cyc_Absyn_Unknown_kb_struct{
int tag; struct Cyc_Core_Opt* f1; } ; static const int Cyc_Absyn_Less_kb= 2;
struct Cyc_Absyn_Less_kb_struct{ int tag; struct Cyc_Core_Opt* f1; void* f2; } ;
struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; void* kind; } ;
static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars;
struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args; int
c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple1* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl** f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1* tunion_name; struct _tuple1* field_name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{
int tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
1; struct Cyc_Absyn_KnownTunionfield_struct{ int tag; struct Cyc_Absyn_Tuniondecl*
f1; struct Cyc_Absyn_Tunionfield* f2; } ; struct Cyc_Absyn_TunionFieldInfo{ void*
field_info; struct Cyc_List_List* targs; } ; static const int Cyc_Absyn_VoidType=
0; static const int Cyc_Absyn_Evar= 0; struct Cyc_Absyn_Evar_struct{ int tag;
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; int f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_VarType= 1; struct Cyc_Absyn_VarType_struct{
int tag; struct Cyc_Absyn_Tvar* f1; } ; static const int Cyc_Absyn_TunionType= 2;
struct Cyc_Absyn_TunionType_struct{ int tag; struct Cyc_Absyn_TunionInfo f1; } ;
static const int Cyc_Absyn_TunionFieldType= 3; struct Cyc_Absyn_TunionFieldType_struct{
int tag; struct Cyc_Absyn_TunionFieldInfo f1; } ; static const int Cyc_Absyn_PointerType=
4; struct Cyc_Absyn_PointerType_struct{ int tag; struct Cyc_Absyn_PtrInfo f1; }
; static const int Cyc_Absyn_IntType= 5; struct Cyc_Absyn_IntType_struct{ int
tag; void* f1; void* f2; } ; static const int Cyc_Absyn_FloatType= 1; static
const int Cyc_Absyn_DoubleType= 6; struct Cyc_Absyn_DoubleType_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_ArrayType= 7; struct Cyc_Absyn_ArrayType_struct{
int tag; void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ;
static const int Cyc_Absyn_FnType= 8; struct Cyc_Absyn_FnType_struct{ int tag;
struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 9; struct
Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static const
int Cyc_Absyn_StructType= 10; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 11; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_EnumType= 12; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_AnonStructType= 13; struct Cyc_Absyn_AnonStructType_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType= 14;
struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_AnonEnumType= 15; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
16; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 17; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 2; static const int Cyc_Absyn_AccessEff= 18;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
19; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 20; struct Cyc_Absyn_RgnsEff_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; struct Cyc_List_List* f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Nullable_ps= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_TaggedArray_ps=
0; static const int Cyc_Absyn_Printf_ft= 0; static const int Cyc_Absyn_Scanf_ft=
1; static const int Cyc_Absyn_Regparm_att= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att= 0; static const int
Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att= 2; static const
int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att= 4; static
const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Absyn_Nocommon_att= 6; static const int Cyc_Absyn_Shared_att=
7; static const int Cyc_Absyn_Unused_att= 8; static const int Cyc_Absyn_Weak_att=
9; static const int Cyc_Absyn_Dllimport_att= 10; static const int Cyc_Absyn_Dllexport_att=
11; static const int Cyc_Absyn_No_instrument_function_att= 12; static const int
Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att= 14;
static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int Cyc_Absyn_Format_att=
3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1; int f2; int f3; } ;
static const int Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; static const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ; static
const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void* f1;
unsigned char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c= 2; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c= 3; struct
Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; static const
int Cyc_Absyn_Float_c= 4; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Null_c= 0; static
const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static const
int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const int Cyc_Absyn_Mod=
4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq= 6; static
const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static const int
Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int Cyc_Absyn_Not=
11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand= 13;
static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
static const int Cyc_Absyn_Bitlshift= 16; static const int Cyc_Absyn_Bitlrshift=
17; static const int Cyc_Absyn_Bitarshift= 18; static const int Cyc_Absyn_Size=
19; static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
static const int Cyc_Absyn_PreDec= 2; static const int Cyc_Absyn_PostDec= 3;
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_StructField=
0; struct Cyc_Absyn_StructField_struct{ int tag; struct _tagged_arr* f1; } ;
static const int Cyc_Absyn_TupleIndex= 1; struct Cyc_Absyn_TupleIndex_struct{
int tag; unsigned int f1; } ; static const int Cyc_Absyn_Const_e= 0; struct Cyc_Absyn_Const_e_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Var_e= 1; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple1* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_Primop_e= 3; struct Cyc_Absyn_Primop_e_struct{ int tag; void*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_AssignOp_e= 4;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Increment_e=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_Conditional_e= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_SeqExp_e= 7; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_FnCall_e=
9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static const int Cyc_Absyn_Throw_e=
10; struct Cyc_Absyn_Throw_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_NoInstantiate_e= 11; struct Cyc_Absyn_NoInstantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Instantiate_e=
12; struct Cyc_Absyn_Instantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e=
14; struct Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e=
16; struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_Sizeofexp_e= 17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_Absyn_Gentyp_e= 19; struct
Cyc_Absyn_Gentyp_e_struct{ int tag; struct Cyc_List_List* f1; void* f2; } ;
static const int Cyc_Absyn_Deref_e= 20; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 21;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_arr* f2; } ; static const int Cyc_Absyn_StructArrow_e= 22; struct
Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e= 23; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 24; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 25; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
26; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 27; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 28; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 29;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 30; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_AnonEnum_e= 32; struct Cyc_Absyn_AnonEnum_e_struct{
int tag; struct _tuple1* f1; void* f2; struct Cyc_Absyn_Enumfield* f3; } ;
static const int Cyc_Absyn_Malloc_e= 33; struct Cyc_Absyn_Malloc_e_struct{ int
tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
34; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 35; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 36; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 37; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; static const
int Cyc_Absyn_Skip_s= 0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1;
struct Cyc_Absyn_Seq_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_IfThenElse_s=
3; struct Cyc_Absyn_IfThenElse_s_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3; } ; static const int Cyc_Absyn_While_s=
4; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Break_s= 5; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Continue_s= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Goto_s= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_For_s=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt* f4; } ; static const int
Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_SwitchC_s= 10;
struct Cyc_Absyn_SwitchC_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Fallthru_s= 11; struct Cyc_Absyn_Fallthru_s_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ;
static const int Cyc_Absyn_Decl_s= 12; struct Cyc_Absyn_Decl_s_struct{ int tag;
struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s=
13; struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Splice_s= 14; struct Cyc_Absyn_Splice_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Label_s= 15;
struct Cyc_Absyn_Label_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Do_s= 16; struct Cyc_Absyn_Do_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s=
17; struct Cyc_Absyn_TryCatch_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Region_s= 18; struct
Cyc_Absyn_Region_s_struct{ int tag; struct Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment*
loc; struct Cyc_List_List* non_local_preds; int try_depth; void* annot; } ;
static const int Cyc_Absyn_Wild_p= 0; static const int Cyc_Absyn_Var_p= 0;
struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Null_p= 1; static const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
int tag; unsigned char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Pointer_p= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; static const int Cyc_Absyn_Reference_p= 6; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Struct_p= 7; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Tunion_p= 8; struct
Cyc_Absyn_Tunion_p_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Enum_p=
9; struct Cyc_Absyn_Enum_p_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1;
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_AnonEnum_p= 10;
struct Cyc_Absyn_AnonEnum_p_struct{ int tag; void* f1; struct Cyc_Absyn_Enumfield*
f2; } ; static const int Cyc_Absyn_UnknownId_p= 11; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_UnknownCall_p= 12;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p= 13;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt*
topt; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc; } ;
struct Cyc_Absyn_SwitchC_clause{ struct Cyc_Absyn_Exp* cnst_exp; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Unresolved_b=
0; static const int Cyc_Absyn_Global_b= 0; struct Cyc_Absyn_Global_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Funname_b= 1;
struct Cyc_Absyn_Funname_b_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
static const int Cyc_Absyn_Param_b= 2; struct Cyc_Absyn_Param_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Local_b= 3; struct
Cyc_Absyn_Local_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Pat_b= 4; struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple1* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
tvs; void* defn; } ; static const int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Fn_d= 1;
struct Cyc_Absyn_Fn_d_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ; static
const int Cyc_Absyn_Let_d= 2; struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat*
f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4;
int f5; } ; static const int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Struct_d= 4;
struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl* f1; } ;
static const int Cyc_Absyn_Union_d= 5; struct Cyc_Absyn_Union_d_struct{ int tag;
struct Cyc_Absyn_Uniondecl* f1; } ; static const int Cyc_Absyn_Tunion_d= 6;
struct Cyc_Absyn_Tunion_d_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; } ;
static const int Cyc_Absyn_Enum_d= 7; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d= 8;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; static const int Cyc_Absyn_Namespace_d= 9; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_arr* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple1*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern void* Cyc_Absyn_compress_kb( void*); extern struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Std___sFILE* f); struct Cyc_Declaration_spec;
struct Cyc_Declarator; struct Cyc_Abstractdeclarator; extern int Cyc_yyparse();
extern unsigned char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
unsigned char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern unsigned char
Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{ unsigned char*
tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Bool_tok[ 13u]; struct Cyc_Bool_tok_struct{ unsigned char* tag;
int f1; } ; extern unsigned char Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{
unsigned char* tag; unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[
17u]; struct Cyc_DeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_DeclSpec_tok[ 17u]; struct Cyc_DeclSpec_tok_struct{
unsigned char* tag; struct Cyc_Declaration_spec* f1; } ; extern unsigned char
Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{ unsigned char* tag;
struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[ 23u];
struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple4{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple4* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct _tuple5{ struct Cyc_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple5* f1; } ; extern unsigned char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple6{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple6* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple7{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple7* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple2* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple1* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple8{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple8* f1; } ; extern unsigned char Cyc_Rgnorder_tok[
17u]; struct Cyc_Rgnorder_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Short_tok[ 14u];
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; extern
unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{ unsigned char*
tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[ 18u];
struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; }
; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[
28u]; struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchCClauseList_tok[
26u]; struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[ 25u]; struct Cyc_SwitchClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionFieldList_tok[
24u]; struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionField_tok[ 20u]; struct Cyc_TunionField_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; extern unsigned char
Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ unsigned char* tag;
struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; extern int Cyc_yyparse();
extern unsigned char Cyc_YY1[ 8u]; struct Cyc_YY1_struct{ unsigned char* tag;
struct _tuple7* f1; } ; extern unsigned char Cyc_YYINITIALSVAL[ 18u]; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr* Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct
_tagged_arr Cyc_Absynpp_char_escape( unsigned char); extern struct _tagged_arr
Cyc_Absynpp_string_escape( struct _tagged_arr); extern struct _tagged_arr Cyc_Absynpp_prim2str(
void* p); struct _tuple9{ struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List*
f3; } ; extern struct _tuple9 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq,
void* t); extern void Cyc_Lex_lex_init(); struct Cyc_Position_Segment{ int start;
int end; } ; static const int Cyc_MatchDecl= 0; struct Cyc_MatchDecl_struct{ int
tag; struct _tagged_arr f1; } ; static const int Cyc_Standalone= 1; struct Cyc_Standalone_struct{
int tag; struct _tagged_arr f1; } ; struct _tuple10{ int f1; void* f2; } ;
extern struct _tuple10* Cyc_token( struct Cyc_Lexing_lexbuf* lexbuf); const int
Cyc_lex_base[ 15u]=( const int[ 15u]){ 0, - 4, 0, - 3, 1, 2, 3, 0, 4, 6, 7, - 1,
8, 9, - 2}; const int Cyc_lex_backtrk[ 15u]=( const int[ 15u]){ - 1, - 1, 3, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; const int Cyc_lex_default[
15u]=( const int[ 15u]){ 1, 0, - 1, 0, - 1, - 1, 6, - 1, 8, 8, 8, 0, 6, 6, 0};
const int Cyc_lex_trans[ 266u]=( const int[ 266u]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 6, 0, 0,
0, 0, 0, 0, 0, 4, 5, 7, 12, 9, 2, 10, 10, 13, 13, 0, 11, - 1, 14, - 1, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 3, 0, 0, - 1, - 1, 0, - 1, - 1, - 1, - 1}; const int Cyc_lex_check[
266u]=( const int[ 266u]){ - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 7, - 1, 5, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 2, 4, 5,
6, 8, 0, 9, 10, 12, 13, - 1, 9, 10, 12, 13, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0,
- 1, - 1, 6, 8, - 1, 9, 10, 12, 13}; int Cyc_lex_engine( int start_state, struct
Cyc_Lexing_lexbuf* lbuf){ int state; int base; int backtrk; int c; state=
start_state; if( state >=  0){ lbuf->lex_last_pos=( lbuf->lex_start_pos= lbuf->lex_curr_pos);
lbuf->lex_last_action= - 1;} else{ state=( - state) -  1;} while( 1) { base= Cyc_lex_base[
_check_known_subscript_notnull( 15u, state)]; if( base <  0){ return( - base) - 
1;} backtrk= Cyc_lex_backtrk[ _check_known_subscript_notnull( 15u, state)]; if(
backtrk >=  0){ lbuf->lex_last_pos= lbuf->lex_curr_pos; lbuf->lex_last_action=
backtrk;} if( lbuf->lex_curr_pos >=  lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){
return( - state) -  1;} else{ c= 256;}} else{ c=( int)*(( unsigned char*)
_check_unknown_subscript( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_curr_pos
++)); if( c ==  - 1){ c= 256;}} if( Cyc_lex_check[
_check_known_subscript_notnull( 266u, base +  c)] ==  state){ state= Cyc_lex_trans[
_check_known_subscript_notnull( 266u, base +  c)];} else{ state= Cyc_lex_default[
_check_known_subscript_notnull( 15u, state)];} if( state <  0){ lbuf->lex_curr_pos=
lbuf->lex_last_pos; if( lbuf->lex_last_action ==  - 1){( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp0=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp0[ 0]=({ struct Cyc_Lexing_Error_struct
_temp1; _temp1.tag= Cyc_Lexing_Error; _temp1.f1= _tag_arr("empty token", sizeof(
unsigned char), 12u); _temp1;}); _temp0;}));} else{ return lbuf->lex_last_action;}}
else{ if( c ==  256){ lbuf->lex_eof_reached= 0;}}}} struct _tuple10* Cyc_token_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL2: { int _temp4= lexbuf->lex_start_pos
+  5; int _temp5=( lexbuf->lex_curr_pos -  lexbuf->lex_start_pos) -  7; return({
struct _tuple10* _temp6=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10));
_temp6->f1= Cyc_Lexing_lexeme_start( lexbuf); _temp6->f2=( void*)({ struct Cyc_Standalone_struct*
_temp7=( struct Cyc_Standalone_struct*) _cycalloc( sizeof( struct Cyc_Standalone_struct));
_temp7[ 0]=({ struct Cyc_Standalone_struct _temp8; _temp8.tag= Cyc_Standalone;
_temp8.f1= Cyc_Std_substring(( struct _tagged_arr) lexbuf->lex_buffer, _temp4,(
unsigned int) _temp5); _temp8;}); _temp7;}); _temp6;});} case 1: _LL3: { int
_temp10= lexbuf->lex_start_pos +  4; int _temp11=( lexbuf->lex_curr_pos - 
lexbuf->lex_start_pos) -  6; return({ struct _tuple10* _temp12=( struct _tuple10*)
_cycalloc( sizeof( struct _tuple10)); _temp12->f1= Cyc_Lexing_lexeme_start(
lexbuf); _temp12->f2=( void*)({ struct Cyc_MatchDecl_struct* _temp13=( struct
Cyc_MatchDecl_struct*) _cycalloc( sizeof( struct Cyc_MatchDecl_struct)); _temp13[
0]=({ struct Cyc_MatchDecl_struct _temp14; _temp14.tag= Cyc_MatchDecl; _temp14.f1=
Cyc_Std_substring(( struct _tagged_arr) lexbuf->lex_buffer, _temp10,(
unsigned int) _temp11); _temp14;}); _temp13;}); _temp12;});} case 2: _LL9:
return 0; case 3: _LL15: return Cyc_token( lexbuf); default: _LL16:( lexbuf->refill_buff)(
lexbuf); return Cyc_token_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp18=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp18[ 0]=({ struct Cyc_Lexing_Error_struct
_temp19; _temp19.tag= Cyc_Lexing_Error; _temp19.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp19;}); _temp18;}));} struct _tuple10* Cyc_token(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_token_rec( lexbuf, 0);} struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes= 3;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Tcenv_AnonEnumRes= 4; struct
Cyc_Tcenv_AnonEnumRes_struct{ int tag; void* f1; struct Cyc_Absyn_Enumfield* f2;
} ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict*
structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict* tuniondecls;
struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict*
ordinaries; struct Cyc_List_List* availables; } ; struct Cyc_Tcenv_Fenv; static
const int Cyc_Tcenv_NotLoop_j= 0; static const int Cyc_Tcenv_CaseEnd_j= 1;
static const int Cyc_Tcenv_FnEnd_j= 2; static const int Cyc_Tcenv_Stmt_j= 0;
struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static
const int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct
Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void* f2; } ; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); static const int Cyc_Dump_ALPHA= 0; static const int Cyc_Dump_SYMBOL= 1;
static const int Cyc_Dump_POINTER= 2; void* Cyc_Dump_last_dumped=( void*) 1u;
void Cyc_Dump_dump_alpha( struct _tagged_arr s){ if( Cyc_Dump_last_dumped == (
void*) Cyc_Dump_ALPHA){({ struct Cyc_Std_String_pa_struct _temp21; _temp21.tag=
Cyc_Std_String_pa; _temp21.f1=( struct _tagged_arr) s;{ void* _temp20[ 1u]={&
_temp21}; Cyc_Std_printf( _tag_arr(" %s", sizeof( unsigned char), 4u), _tag_arr(
_temp20, sizeof( void*), 1u));}});} else{({ struct Cyc_Std_String_pa_struct
_temp23; _temp23.tag= Cyc_Std_String_pa; _temp23.f1=( struct _tagged_arr) s;{
void* _temp22[ 1u]={& _temp23}; Cyc_Std_printf( _tag_arr("%s", sizeof(
unsigned char), 3u), _tag_arr( _temp22, sizeof( void*), 1u));}}); Cyc_Dump_last_dumped=(
void*) Cyc_Dump_ALPHA;}} void Cyc_Dump_dump_alphastr( struct _tagged_arr* s){
Cyc_Dump_dump_alpha(* s);} void Cyc_Dump_dump_symbol( struct _tagged_arr s){({
struct Cyc_Std_String_pa_struct _temp25; _temp25.tag= Cyc_Std_String_pa; _temp25.f1=(
struct _tagged_arr) s;{ void* _temp24[ 1u]={& _temp25}; Cyc_Std_printf( _tag_arr("%s",
sizeof( unsigned char), 3u), _tag_arr( _temp24, sizeof( void*), 1u));}}); Cyc_Dump_last_dumped=(
void*) Cyc_Dump_SYMBOL;} void Cyc_Dump_dump_char( int c){({ struct Cyc_Std_Int_pa_struct
_temp27; _temp27.tag= Cyc_Std_Int_pa; _temp27.f1=( int)(( unsigned int) c);{
void* _temp26[ 1u]={& _temp27}; Cyc_Std_printf( _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp26, sizeof( void*), 1u));}}); Cyc_Dump_last_dumped=(
void*) Cyc_Dump_SYMBOL;} void Cyc_Dump_dump_pointer( struct _tagged_arr s){ if(
Cyc_Dump_last_dumped == ( void*) Cyc_Dump_ALPHA){({ struct Cyc_Std_String_pa_struct
_temp29; _temp29.tag= Cyc_Std_String_pa; _temp29.f1=( struct _tagged_arr) s;{
void* _temp28[ 1u]={& _temp29}; Cyc_Std_printf( _tag_arr(" %s", sizeof(
unsigned char), 4u), _tag_arr( _temp28, sizeof( void*), 1u));}});} else{({
struct Cyc_Std_String_pa_struct _temp31; _temp31.tag= Cyc_Std_String_pa; _temp31.f1=(
struct _tagged_arr) s;{ void* _temp30[ 1u]={& _temp31}; Cyc_Std_printf( _tag_arr("%s",
sizeof( unsigned char), 3u), _tag_arr( _temp30, sizeof( void*), 1u));}});} Cyc_Dump_last_dumped=(
void*) Cyc_Dump_POINTER;} void Cyc_Dump_dump_boldalpha( struct _tagged_arr x){
Cyc_Dump_dump_alpha( _tag_arr("", sizeof( unsigned char), 1u)); Cyc_Dump_dump_symbol(
_tag_arr("\\textbf{", sizeof( unsigned char), 9u)); Cyc_Dump_dump_alpha( x); Cyc_Dump_dump_symbol(
_tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Dump_dump_alpha( _tag_arr("",
sizeof( unsigned char), 1u));} void Cyc_Dump_dump_lb(){ Cyc_Dump_dump_symbol(
_tag_arr("{\\lb}", sizeof( unsigned char), 6u));} void Cyc_Dump_dump_rb(){ Cyc_Dump_dump_symbol(
_tag_arr("{\\rb}", sizeof( unsigned char), 6u));} void Cyc_Dump_dump_begin(){
Cyc_Dump_dump_symbol( _tag_arr("%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}\n",
sizeof( unsigned char), 348u));} void Cyc_Dump_dump_middle(){ Cyc_Dump_dump_symbol(
_tag_arr("\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}\n",
sizeof( unsigned char), 148u));} void Cyc_Dump_dump_end(){ Cyc_Dump_dump_symbol(
_tag_arr("%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}\n",
sizeof( unsigned char), 114u));} void Cyc_Dump_dumpqvar( struct _tuple1* v);
void Cyc_Dump_dump_boldqvar( struct _tuple1* v){ Cyc_Dump_dump_alpha( _tag_arr("",
sizeof( unsigned char), 1u)); Cyc_Dump_dump_symbol( _tag_arr("\\textbf{",
sizeof( unsigned char), 9u)); Cyc_Dump_dumpqvar( v); Cyc_Dump_dump_symbol(
_tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Dump_dump_alpha( _tag_arr("",
sizeof( unsigned char), 1u));} static int Cyc_Dump_expand_typedefs= 0; static
int Cyc_Dump_qvar_to_Cids= 0; static int Cyc_Dump_add_cyc_prefix= 0; static int
Cyc_Dump_to_VC= 0; void Cyc_Dump_dumpenumfields( struct Cyc_List_List* fields);
void Cyc_Dump_dumptyp( void*); void Cyc_Dump_dumpntyp( void* t); void Cyc_Dump_dumpexp(
struct Cyc_Absyn_Exp*); void Cyc_Dump_dumpexp_prec( int, struct Cyc_Absyn_Exp*);
void Cyc_Dump_dumpdecl( struct Cyc_Absyn_Decl*, struct _tagged_arr); void Cyc_Dump_dumptms(
struct Cyc_List_List*, void(* f)( void*), void*); void Cyc_Dump_dumptqtd( struct
Cyc_Absyn_Tqual, void*, void(* f)( void*), void*); void Cyc_Dump_dumpstructfields(
struct Cyc_List_List* fields); void Cyc_Dump_ignore( void* x){ return;} void Cyc_Dump_dump_semi(){
Cyc_Dump_dump_char(( int)';');} void Cyc_Dump_dump_sep( void(* f)( void*),
struct Cyc_List_List* l, struct _tagged_arr sep){ if( l ==  0){ return;} for( 0;
l->tl !=  0; l= l->tl){ f(( void*) l->hd); Cyc_Dump_dump_symbol( sep);} f(( void*)
l->hd);} void Cyc_Dump_dump_sep_c( void(* f)( void*, void*), void* env, struct
Cyc_List_List* l, struct _tagged_arr sep){ if( l ==  0){ return;} for( 0; l->tl
!=  0; l= l->tl){ f( env,( void*) l->hd); Cyc_Dump_dump_symbol( sep);} f( env,(
void*) l->hd);} void Cyc_Dump_group( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep){ Cyc_Dump_dump_symbol(
start); Cyc_Dump_dump_sep( f, l, sep); Cyc_Dump_dump_symbol( end);} void Cyc_Dump_group_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_arr
start, struct _tagged_arr end, struct _tagged_arr sep){ Cyc_Dump_dump_symbol(
start); Cyc_Dump_dump_sep_c( f, env, l, sep); Cyc_Dump_dump_symbol( end);} void
Cyc_Dump_egroup( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr
start, struct _tagged_arr end, struct _tagged_arr sep){ if( l !=  0){ Cyc_Dump_group(
f, l, start, end, sep);}} void Cyc_Dump_dumpqvar( struct _tuple1* v){ struct Cyc_List_List*
_temp32= 0;{ void* _temp33=(* v).f1; struct Cyc_List_List* _temp41; struct Cyc_List_List*
_temp43; _LL35: if( _temp33 == ( void*) Cyc_Absyn_Loc_n){ goto _LL36;} else{
goto _LL37;} _LL37: if(( unsigned int) _temp33 >  1u?*(( int*) _temp33) ==  Cyc_Absyn_Rel_n:
0){ _LL42: _temp41=(( struct Cyc_Absyn_Rel_n_struct*) _temp33)->f1; goto _LL38;}
else{ goto _LL39;} _LL39: if(( unsigned int) _temp33 >  1u?*(( int*) _temp33) == 
Cyc_Absyn_Abs_n: 0){ _LL44: _temp43=(( struct Cyc_Absyn_Abs_n_struct*) _temp33)->f1;
goto _LL40;} else{ goto _LL34;} _LL36: _temp41= 0; goto _LL38; _LL38: _temp32=
_temp41; goto _LL34; _LL40: _temp32=( Cyc_Dump_qvar_to_Cids? Cyc_Dump_add_cyc_prefix:
0)?({ struct Cyc_List_List* _temp45=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp45->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp45->tl=
_temp43; _temp45;}): _temp43; goto _LL34; _LL34:;} if( _temp32 !=  0){ Cyc_Dump_dump_alphastr((
struct _tagged_arr*) _temp32->hd); for( _temp32= _temp32->tl; _temp32 !=  0;
_temp32= _temp32->tl){ if( Cyc_Dump_qvar_to_Cids){ Cyc_Dump_dump_char(( int)'_');}
else{ Cyc_Dump_dump_symbol( _tag_arr("::", sizeof( unsigned char), 3u));} Cyc_Dump_dump_alpha(*((
struct _tagged_arr*) _temp32->hd));} if( Cyc_Dump_qvar_to_Cids){ Cyc_Dump_dump_symbol(
_tag_arr("_", sizeof( unsigned char), 2u));} else{ Cyc_Dump_dump_symbol(
_tag_arr("::", sizeof( unsigned char), 3u));} Cyc_Dump_dump_alpha(*(* v).f2);}
else{ Cyc_Dump_dump_alpha(*(* v).f2);}} void Cyc_Dump_dumptq( struct Cyc_Absyn_Tqual
tq){ if( tq.q_restrict){ Cyc_Dump_dump_alpha( _tag_arr("restrict", sizeof(
unsigned char), 9u));} if( tq.q_volatile){ Cyc_Dump_dump_alpha( _tag_arr("volatile",
sizeof( unsigned char), 9u));} if( tq.q_const){ Cyc_Dump_dump_alpha( _tag_arr("const",
sizeof( unsigned char), 6u));}} void Cyc_Dump_dumpscope( void* sc){ void*
_temp46= sc; _LL48: if( _temp46 == ( void*) Cyc_Absyn_Static){ goto _LL49;}
else{ goto _LL50;} _LL50: if( _temp46 == ( void*) Cyc_Absyn_Public){ goto _LL51;}
else{ goto _LL52;} _LL52: if( _temp46 == ( void*) Cyc_Absyn_Extern){ goto _LL53;}
else{ goto _LL54;} _LL54: if( _temp46 == ( void*) Cyc_Absyn_ExternC){ goto _LL55;}
else{ goto _LL56;} _LL56: if( _temp46 == ( void*) Cyc_Absyn_Abstract){ goto
_LL57;} else{ goto _LL47;} _LL49: Cyc_Dump_dump_alpha( _tag_arr("static",
sizeof( unsigned char), 7u)); return; _LL51: return; _LL53: Cyc_Dump_dump_alpha(
_tag_arr("extern", sizeof( unsigned char), 7u)); return; _LL55: Cyc_Dump_dump_alpha(
_tag_arr("extern \"C\"", sizeof( unsigned char), 11u)); return; _LL57: Cyc_Dump_dump_alpha(
_tag_arr("abstract", sizeof( unsigned char), 9u)); return; _LL47:;} void Cyc_Dump_dumpkind(
void* k){ void* _temp58= k; _LL60: if( _temp58 == ( void*) Cyc_Absyn_AnyKind){
goto _LL61;} else{ goto _LL62;} _LL62: if( _temp58 == ( void*) Cyc_Absyn_MemKind){
goto _LL63;} else{ goto _LL64;} _LL64: if( _temp58 == ( void*) Cyc_Absyn_BoxKind){
goto _LL65;} else{ goto _LL66;} _LL66: if( _temp58 == ( void*) Cyc_Absyn_RgnKind){
goto _LL67;} else{ goto _LL68;} _LL68: if( _temp58 == ( void*) Cyc_Absyn_EffKind){
goto _LL69;} else{ goto _LL59;} _LL61: Cyc_Dump_dump_alpha( _tag_arr("A",
sizeof( unsigned char), 2u)); return; _LL63: Cyc_Dump_dump_alpha( _tag_arr("M",
sizeof( unsigned char), 2u)); return; _LL65: Cyc_Dump_dump_alpha( _tag_arr("B",
sizeof( unsigned char), 2u)); return; _LL67: Cyc_Dump_dump_alpha( _tag_arr("R",
sizeof( unsigned char), 2u)); return; _LL69: Cyc_Dump_dump_alpha( _tag_arr("E",
sizeof( unsigned char), 2u)); return; _LL59:;} void Cyc_Dump_dumptps( struct Cyc_List_List*
ts){ Cyc_Dump_egroup( Cyc_Dump_dumptyp, ts, _tag_arr("<", sizeof( unsigned char),
2u), _tag_arr(">", sizeof( unsigned char), 2u), _tag_arr(", ", sizeof(
unsigned char), 3u)); Cyc_Dump_dump_alpha( _tag_arr("", sizeof( unsigned char),
1u));} void Cyc_Dump_dumptvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Dump_dump_alphastr(
tv->name);} void Cyc_Dump_dumpkindedtvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Dump_dump_alphastr(
tv->name);{ void* _temp70= Cyc_Absyn_compress_kb(( void*) tv->kind); void*
_temp80; void* _temp82; void* _temp84; _LL72: if(*(( int*) _temp70) ==  Cyc_Absyn_Eq_kb){
_LL81: _temp80=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp70)->f1; if(
_temp80 == ( void*) Cyc_Absyn_BoxKind){ goto _LL73;} else{ goto _LL74;}} else{
goto _LL74;} _LL74: if(*(( int*) _temp70) ==  Cyc_Absyn_Less_kb){ _LL83: _temp82=(
void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp70)->f2; goto _LL75;} else{ goto
_LL76;} _LL76: if(*(( int*) _temp70) ==  Cyc_Absyn_Eq_kb){ _LL85: _temp84=( void*)((
struct Cyc_Absyn_Eq_kb_struct*) _temp70)->f1; goto _LL77;} else{ goto _LL78;}
_LL78: goto _LL79; _LL73: goto _LL71; _LL75: _temp84= _temp82; goto _LL77; _LL77:
Cyc_Dump_dump_symbol( _tag_arr("::", sizeof( unsigned char), 3u)); Cyc_Dump_dumpkind(
_temp84); goto _LL71; _LL79: Cyc_Dump_dump_symbol( _tag_arr("::?", sizeof(
unsigned char), 4u)); goto _LL71; _LL71:;}} void Cyc_Dump_dumptvars( struct Cyc_List_List*
tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_egroup)(
Cyc_Dump_dumptvar, tvs, _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); Cyc_Dump_dump_alpha(
_tag_arr("", sizeof( unsigned char), 1u));} void Cyc_Dump_dumpkindedtvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Dump_egroup)( Cyc_Dump_dumpkindedtvar, tvs,
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u)); Cyc_Dump_dump_alpha( _tag_arr("",
sizeof( unsigned char), 1u));} struct _tuple11{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; void Cyc_Dump_dumparg( struct _tuple11* pr){(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( int), int)) Cyc_Dump_dumptqtd)((* pr).f1,(* pr).f2,( void(*)(
int x)) Cyc_Dump_ignore, 0);} void Cyc_Dump_dumpargs( struct Cyc_List_List* ts){((
void(*)( void(* f)( struct _tuple11*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)(
Cyc_Dump_dumparg, ts, _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")",
sizeof( unsigned char), 2u), _tag_arr(", ", sizeof( unsigned char), 3u));} int
Cyc_Dump_next_is_pointer( struct Cyc_List_List* tms){ if( tms ==  0){ return 0;}{
void* _temp86=( void*) tms->hd; _LL88: if(( unsigned int) _temp86 >  1u?*(( int*)
_temp86) ==  Cyc_Absyn_Pointer_mod: 0){ goto _LL89;} else{ goto _LL90;} _LL90:
goto _LL91; _LL89: return 1; _LL91: return 0; _LL87:;}} static void Cyc_Dump_dumprgn(
void* t){ void* _temp92= Cyc_Tcutil_compress( t); _LL94: if( _temp92 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL95;} else{ goto _LL96;} _LL96: goto _LL97; _LL95:
Cyc_Dump_dump_alpha( _tag_arr("`H", sizeof( unsigned char), 3u)); goto _LL93;
_LL97: Cyc_Dump_dumpntyp( t); goto _LL93; _LL93:;} struct _tuple12{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; static struct _tuple12 Cyc_Dump_effects_split(
void* t){ struct Cyc_List_List* rgions= 0; struct Cyc_List_List* effects= 0;{
void* _temp98= Cyc_Tcutil_compress( t); void* _temp106; struct Cyc_List_List*
_temp108; _LL100: if(( unsigned int) _temp98 >  3u?*(( int*) _temp98) ==  Cyc_Absyn_AccessEff:
0){ _LL107: _temp106=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp98)->f1;
goto _LL101;} else{ goto _LL102;} _LL102: if(( unsigned int) _temp98 >  3u?*((
int*) _temp98) ==  Cyc_Absyn_JoinEff: 0){ _LL109: _temp108=(( struct Cyc_Absyn_JoinEff_struct*)
_temp98)->f1; goto _LL103;} else{ goto _LL104;} _LL104: goto _LL105; _LL101:
rgions=({ struct Cyc_List_List* _temp110=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp110->hd=( void*) _temp106; _temp110->tl=
rgions; _temp110;}); goto _LL99; _LL103: for( 0; _temp108 !=  0; _temp108=
_temp108->tl){ struct Cyc_List_List* _temp113; struct Cyc_List_List* _temp115;
struct _tuple12 _temp111= Cyc_Dump_effects_split(( void*) _temp108->hd); _LL116:
_temp115= _temp111.f1; goto _LL114; _LL114: _temp113= _temp111.f2; goto _LL112;
_LL112: rgions= Cyc_List_imp_append( _temp115, rgions); effects= Cyc_List_imp_append(
_temp113, effects);} goto _LL99; _LL105: effects=({ struct Cyc_List_List*
_temp117=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp117->hd=( void*) t; _temp117->tl= effects; _temp117;}); goto _LL99; _LL99:;}
return({ struct _tuple12 _temp118; _temp118.f1= rgions; _temp118.f2= effects;
_temp118;});} static void Cyc_Dump_dumpeff( void* t){ struct Cyc_List_List*
_temp121; struct Cyc_List_List* _temp123; struct _tuple12 _temp119= Cyc_Dump_effects_split(
t); _LL124: _temp123= _temp119.f1; goto _LL122; _LL122: _temp121= _temp119.f2;
goto _LL120; _LL120: _temp123= Cyc_List_imp_rev( _temp123); _temp121= Cyc_List_imp_rev(
_temp121); for( 0; _temp121 !=  0; _temp121= _temp121->tl){ Cyc_Dump_dumpntyp((
void*) _temp121->hd); Cyc_Dump_dump_char(( int)'+');} Cyc_Dump_dump_lb(); for( 0;
_temp123 !=  0; _temp123= _temp123->tl){ Cyc_Dump_dumprgn(( void*) _temp123->hd);
if( _temp123->tl !=  0){ Cyc_Dump_dump_char(( int)',');}} Cyc_Dump_dump_rb();}
void Cyc_Dump_dumpntyp( void* t){ void* _temp125= t; struct Cyc_Absyn_Tvar*
_temp199; int _temp201; struct Cyc_Core_Opt* _temp203; struct Cyc_Core_Opt*
_temp205; int _temp207; struct Cyc_Core_Opt* _temp209; struct Cyc_Core_Opt
_temp211; void* _temp212; struct Cyc_Core_Opt* _temp214; struct Cyc_Absyn_TunionInfo
_temp216; void* _temp218; struct Cyc_List_List* _temp220; void* _temp222; struct
Cyc_Absyn_TunionFieldInfo _temp224; struct Cyc_List_List* _temp226; void*
_temp228; struct _tuple1* _temp230; struct Cyc_List_List* _temp232; void*
_temp234; void* _temp236; void* _temp238; void* _temp240; void* _temp242; void*
_temp244; void* _temp246; void* _temp248; void* _temp250; void* _temp252; void*
_temp254; void* _temp256; void* _temp259; void* _temp261; void* _temp263; void*
_temp265; void* _temp268; void* _temp270; void* _temp272; void* _temp274; int
_temp276; struct Cyc_List_List* _temp278; struct Cyc_List_List* _temp280; struct
_tuple1* _temp282; struct Cyc_List_List* _temp284; struct _tuple1* _temp286;
struct Cyc_List_List* _temp288; struct _tuple1* _temp290; struct Cyc_List_List*
_temp292; struct _tuple1* _temp294; struct Cyc_List_List* _temp296; struct Cyc_List_List*
_temp298; struct Cyc_Core_Opt* _temp300; struct Cyc_List_List* _temp302; struct
_tuple1* _temp304; void* _temp306; _LL127: if(( unsigned int) _temp125 >  3u?*((
int*) _temp125) ==  Cyc_Absyn_ArrayType: 0){ goto _LL128;} else{ goto _LL129;}
_LL129: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_FnType:
0){ goto _LL130;} else{ goto _LL131;} _LL131: if(( unsigned int) _temp125 >  3u?*((
int*) _temp125) ==  Cyc_Absyn_PointerType: 0){ goto _LL132;} else{ goto _LL133;}
_LL133: if( _temp125 == ( void*) Cyc_Absyn_VoidType){ goto _LL134;} else{ goto
_LL135;} _LL135: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_VarType:
0){ _LL200: _temp199=(( struct Cyc_Absyn_VarType_struct*) _temp125)->f1; goto
_LL136;} else{ goto _LL137;} _LL137: if(( unsigned int) _temp125 >  3u?*(( int*)
_temp125) ==  Cyc_Absyn_Evar: 0){ _LL206: _temp205=(( struct Cyc_Absyn_Evar_struct*)
_temp125)->f1; goto _LL204; _LL204: _temp203=(( struct Cyc_Absyn_Evar_struct*)
_temp125)->f2; if( _temp203 ==  0){ goto _LL202;} else{ goto _LL139;} _LL202:
_temp201=(( struct Cyc_Absyn_Evar_struct*) _temp125)->f3; goto _LL138;} else{
goto _LL139;} _LL139: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125) == 
Cyc_Absyn_Evar: 0){ _LL215: _temp214=(( struct Cyc_Absyn_Evar_struct*) _temp125)->f1;
goto _LL210; _LL210: _temp209=(( struct Cyc_Absyn_Evar_struct*) _temp125)->f2;
if( _temp209 ==  0){ goto _LL141;} else{ _temp211=* _temp209; _LL213: _temp212=(
void*) _temp211.v; goto _LL208;} _LL208: _temp207=(( struct Cyc_Absyn_Evar_struct*)
_temp125)->f3; goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int)
_temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_TunionType: 0){ _LL217:
_temp216=(( struct Cyc_Absyn_TunionType_struct*) _temp125)->f1; _LL223: _temp222=(
void*) _temp216.tunion_info; goto _LL221; _LL221: _temp220= _temp216.targs; goto
_LL219; _LL219: _temp218=( void*) _temp216.rgn; goto _LL142;} else{ goto _LL143;}
_LL143: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_TunionFieldType:
0){ _LL225: _temp224=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp125)->f1;
_LL229: _temp228=( void*) _temp224.field_info; goto _LL227; _LL227: _temp226=
_temp224.targs; goto _LL144;} else{ goto _LL145;} _LL145: if(( unsigned int)
_temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_EnumType: 0){ _LL231: _temp230=((
struct Cyc_Absyn_EnumType_struct*) _temp125)->f1; goto _LL146;} else{ goto
_LL147;} _LL147: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_AnonEnumType:
0){ _LL233: _temp232=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp125)->f1;
goto _LL148;} else{ goto _LL149;} _LL149: if(( unsigned int) _temp125 >  3u?*((
int*) _temp125) ==  Cyc_Absyn_IntType: 0){ _LL237: _temp236=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp125)->f1; if( _temp236 == ( void*) Cyc_Absyn_Signed){
goto _LL235;} else{ goto _LL151;} _LL235: _temp234=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp234 == ( void*) Cyc_Absyn_B4){ goto _LL150;} else{ goto
_LL151;}} else{ goto _LL151;} _LL151: if(( unsigned int) _temp125 >  3u?*(( int*)
_temp125) ==  Cyc_Absyn_IntType: 0){ _LL241: _temp240=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp240 == ( void*) Cyc_Absyn_Signed){ goto _LL239;} else{
goto _LL153;} _LL239: _temp238=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp238 == ( void*) Cyc_Absyn_B1){ goto _LL152;} else{ goto
_LL153;}} else{ goto _LL153;} _LL153: if(( unsigned int) _temp125 >  3u?*(( int*)
_temp125) ==  Cyc_Absyn_IntType: 0){ _LL245: _temp244=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp244 == ( void*) Cyc_Absyn_Unsigned){ goto _LL243;} else{
goto _LL155;} _LL243: _temp242=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp242 == ( void*) Cyc_Absyn_B1){ goto _LL154;} else{ goto
_LL155;}} else{ goto _LL155;} _LL155: if(( unsigned int) _temp125 >  3u?*(( int*)
_temp125) ==  Cyc_Absyn_IntType: 0){ _LL249: _temp248=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp248 == ( void*) Cyc_Absyn_Signed){ goto _LL247;} else{
goto _LL157;} _LL247: _temp246=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp246 == ( void*) Cyc_Absyn_B2){ goto _LL156;} else{ goto
_LL157;}} else{ goto _LL157;} _LL157: if(( unsigned int) _temp125 >  3u?*(( int*)
_temp125) ==  Cyc_Absyn_IntType: 0){ _LL253: _temp252=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp252 == ( void*) Cyc_Absyn_Unsigned){ goto _LL251;} else{
goto _LL159;} _LL251: _temp250=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp250 == ( void*) Cyc_Absyn_B2){ goto _LL158;} else{ goto
_LL159;}} else{ goto _LL159;} _LL159: if(( unsigned int) _temp125 >  3u?*(( int*)
_temp125) ==  Cyc_Absyn_IntType: 0){ _LL257: _temp256=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp256 == ( void*) Cyc_Absyn_Unsigned){ goto _LL255;} else{
goto _LL161;} _LL255: _temp254=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp254 == ( void*) Cyc_Absyn_B4){ goto _LL160;} else{ goto
_LL161;}} else{ goto _LL161;} _LL161: if(( unsigned int) _temp125 >  3u?*(( int*)
_temp125) ==  Cyc_Absyn_IntType: 0){ _LL262: _temp261=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp261 == ( void*) Cyc_Absyn_Signed){ goto _LL260;} else{
goto _LL163;} _LL260: _temp259=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp259 == ( void*) Cyc_Absyn_B8){ goto _LL258;} else{ goto
_LL163;}} else{ goto _LL163;} _LL258: if( Cyc_Dump_to_VC){ goto _LL162;} else{
goto _LL163;} _LL163: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125) == 
Cyc_Absyn_IntType: 0){ _LL266: _temp265=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp265 == ( void*) Cyc_Absyn_Signed){ goto _LL264;} else{
goto _LL165;} _LL264: _temp263=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp263 == ( void*) Cyc_Absyn_B8){ goto _LL164;} else{ goto
_LL165;}} else{ goto _LL165;} _LL165: if(( unsigned int) _temp125 >  3u?*(( int*)
_temp125) ==  Cyc_Absyn_IntType: 0){ _LL271: _temp270=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp270 == ( void*) Cyc_Absyn_Unsigned){ goto _LL269;} else{
goto _LL167;} _LL269: _temp268=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp268 == ( void*) Cyc_Absyn_B8){ goto _LL267;} else{ goto
_LL167;}} else{ goto _LL167;} _LL267: if( Cyc_Dump_to_VC){ goto _LL166;} else{
goto _LL167;} _LL167: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125) == 
Cyc_Absyn_IntType: 0){ _LL275: _temp274=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp274 == ( void*) Cyc_Absyn_Unsigned){ goto _LL273;} else{
goto _LL169;} _LL273: _temp272=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp272 == ( void*) Cyc_Absyn_B8){ goto _LL168;} else{ goto
_LL169;}} else{ goto _LL169;} _LL169: if( _temp125 == ( void*) Cyc_Absyn_FloatType){
goto _LL170;} else{ goto _LL171;} _LL171: if(( unsigned int) _temp125 >  3u?*((
int*) _temp125) ==  Cyc_Absyn_DoubleType: 0){ _LL277: _temp276=(( struct Cyc_Absyn_DoubleType_struct*)
_temp125)->f1; goto _LL172;} else{ goto _LL173;} _LL173: if(( unsigned int)
_temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_TupleType: 0){ _LL279: _temp278=((
struct Cyc_Absyn_TupleType_struct*) _temp125)->f1; goto _LL174;} else{ goto
_LL175;} _LL175: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_StructType:
0){ _LL283: _temp282=(( struct Cyc_Absyn_StructType_struct*) _temp125)->f1; if(
_temp282 ==  0){ goto _LL281;} else{ goto _LL177;} _LL281: _temp280=(( struct
Cyc_Absyn_StructType_struct*) _temp125)->f2; goto _LL176;} else{ goto _LL177;}
_LL177: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_StructType:
0){ _LL287: _temp286=(( struct Cyc_Absyn_StructType_struct*) _temp125)->f1; goto
_LL285; _LL285: _temp284=(( struct Cyc_Absyn_StructType_struct*) _temp125)->f2;
goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int) _temp125 >  3u?*((
int*) _temp125) ==  Cyc_Absyn_UnionType: 0){ _LL291: _temp290=(( struct Cyc_Absyn_UnionType_struct*)
_temp125)->f1; if( _temp290 ==  0){ goto _LL289;} else{ goto _LL181;} _LL289:
_temp288=(( struct Cyc_Absyn_UnionType_struct*) _temp125)->f2; goto _LL180;}
else{ goto _LL181;} _LL181: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125)
==  Cyc_Absyn_UnionType: 0){ _LL295: _temp294=(( struct Cyc_Absyn_UnionType_struct*)
_temp125)->f1; goto _LL293; _LL293: _temp292=(( struct Cyc_Absyn_UnionType_struct*)
_temp125)->f2; goto _LL182;} else{ goto _LL183;} _LL183: if(( unsigned int)
_temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_AnonStructType: 0){ _LL297:
_temp296=(( struct Cyc_Absyn_AnonStructType_struct*) _temp125)->f1; goto _LL184;}
else{ goto _LL185;} _LL185: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125)
==  Cyc_Absyn_AnonUnionType: 0){ _LL299: _temp298=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp125)->f1; goto _LL186;} else{ goto _LL187;} _LL187: if(( unsigned int)
_temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_TypedefType: 0){ _LL305:
_temp304=(( struct Cyc_Absyn_TypedefType_struct*) _temp125)->f1; goto _LL303;
_LL303: _temp302=(( struct Cyc_Absyn_TypedefType_struct*) _temp125)->f2; goto
_LL301; _LL301: _temp300=(( struct Cyc_Absyn_TypedefType_struct*) _temp125)->f3;
goto _LL188;} else{ goto _LL189;} _LL189: if(( unsigned int) _temp125 >  3u?*((
int*) _temp125) ==  Cyc_Absyn_RgnHandleType: 0){ _LL307: _temp306=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp125)->f1; goto _LL190;} else{ goto
_LL191;} _LL191: if( _temp125 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL192;}
else{ goto _LL193;} _LL193: if(( unsigned int) _temp125 >  3u?*(( int*) _temp125)
==  Cyc_Absyn_AccessEff: 0){ goto _LL194;} else{ goto _LL195;} _LL195: if((
unsigned int) _temp125 >  3u?*(( int*) _temp125) ==  Cyc_Absyn_RgnsEff: 0){ goto
_LL196;} else{ goto _LL197;} _LL197: if(( unsigned int) _temp125 >  3u?*(( int*)
_temp125) ==  Cyc_Absyn_JoinEff: 0){ goto _LL198;} else{ goto _LL126;} _LL128:
return; _LL130: return; _LL132: return; _LL134: Cyc_Dump_dump_alpha( _tag_arr("void",
sizeof( unsigned char), 5u)); return; _LL136: Cyc_Dump_dump_alphastr( _temp199->name);
return; _LL138: Cyc_Dump_dump_char(( int)'%'); if( _temp205 ==  0){ Cyc_Dump_dump_char((
int)'?');} else{ Cyc_Dump_dumpkind(( void*) _temp205->v);} Cyc_Dump_dump_symbol((
struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp309; _temp309.tag= Cyc_Std_Int_pa;
_temp309.f1=( int)(( unsigned int) _temp201);{ void* _temp308[ 1u]={& _temp309};
Cyc_Std_aprintf( _tag_arr("(%d)", sizeof( unsigned char), 5u), _tag_arr(
_temp308, sizeof( void*), 1u));}})); return; _LL140: Cyc_Dump_dumpntyp( _temp212);
return; _LL142:{ void* _temp310= _temp222; struct Cyc_Absyn_UnknownTunionInfo
_temp316; int _temp318; struct _tuple1* _temp320; struct Cyc_Absyn_Tuniondecl**
_temp322; struct Cyc_Absyn_Tuniondecl* _temp324; struct Cyc_Absyn_Tuniondecl
_temp325; int _temp326; struct _tuple1* _temp328; _LL312: if(*(( int*) _temp310)
==  Cyc_Absyn_UnknownTunion){ _LL317: _temp316=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp310)->f1; _LL321: _temp320= _temp316.name; goto _LL319; _LL319: _temp318=
_temp316.is_xtunion; goto _LL313;} else{ goto _LL314;} _LL314: if(*(( int*)
_temp310) ==  Cyc_Absyn_KnownTunion){ _LL323: _temp322=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp310)->f1; _temp324=* _temp322; _temp325=* _temp324; _LL329: _temp328=
_temp325.name; goto _LL327; _LL327: _temp326= _temp325.is_xtunion; goto _LL315;}
else{ goto _LL311;} _LL313: _temp328= _temp320; _temp326= _temp318; goto _LL315;
_LL315: if( _temp326){ Cyc_Dump_dump_alpha( _tag_arr("xtunion", sizeof(
unsigned char), 8u));} else{ Cyc_Dump_dump_alpha( _tag_arr("tunion", sizeof(
unsigned char), 7u));}{ void* _temp330= Cyc_Tcutil_compress( _temp218); _LL332:
if( _temp330 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL333;} else{ goto _LL334;}
_LL334: goto _LL335; _LL333: goto _LL331; _LL335: Cyc_Dump_dumptyp( _temp218);
goto _LL331; _LL331:;} Cyc_Dump_dumpqvar( _temp328); Cyc_Dump_dumptps( _temp220);
goto _LL311; _LL311:;} goto _LL126; _LL144:{ void* _temp336= _temp228; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp342; int _temp344; struct _tuple1*
_temp346; struct _tuple1* _temp348; struct Cyc_Absyn_Tunionfield* _temp350;
struct Cyc_Absyn_Tunionfield _temp352; struct _tuple1* _temp353; struct Cyc_Absyn_Tuniondecl*
_temp355; struct Cyc_Absyn_Tuniondecl _temp357; int _temp358; struct _tuple1*
_temp360; _LL338: if(*(( int*) _temp336) ==  Cyc_Absyn_UnknownTunionfield){
_LL343: _temp342=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp336)->f1;
_LL349: _temp348= _temp342.tunion_name; goto _LL347; _LL347: _temp346= _temp342.field_name;
goto _LL345; _LL345: _temp344= _temp342.is_xtunion; goto _LL339;} else{ goto
_LL340;} _LL340: if(*(( int*) _temp336) ==  Cyc_Absyn_KnownTunionfield){ _LL356:
_temp355=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp336)->f1; _temp357=*
_temp355; _LL361: _temp360= _temp357.name; goto _LL359; _LL359: _temp358=
_temp357.is_xtunion; goto _LL351; _LL351: _temp350=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp336)->f2; _temp352=* _temp350; _LL354: _temp353= _temp352.name; goto _LL341;}
else{ goto _LL337;} _LL339: _temp360= _temp348; _temp358= _temp344; _temp353=
_temp346; goto _LL341; _LL341: if( _temp358){ Cyc_Dump_dump_alpha( _tag_arr("xtunion",
sizeof( unsigned char), 8u));} else{ Cyc_Dump_dump_alpha( _tag_arr("tunion",
sizeof( unsigned char), 7u));} Cyc_Dump_dumpqvar( _temp360); Cyc_Dump_dump_char((
int)'.'); Cyc_Dump_dumpqvar( _temp353); Cyc_Dump_dumptps( _temp226); goto _LL337;
_LL337:;} goto _LL126; _LL146: Cyc_Dump_dump_alpha( _tag_arr("enum", sizeof(
unsigned char), 5u)); Cyc_Dump_dumpqvar( _temp230); return; _LL148: Cyc_Dump_dump_alpha(
_tag_arr("enum", sizeof( unsigned char), 5u)); Cyc_Dump_dump_symbol( _tag_arr(" ",
sizeof( unsigned char), 2u)); Cyc_Dump_dump_lb(); Cyc_Dump_dump_symbol( _tag_arr("\\\\\n~~",
sizeof( unsigned char), 6u)); Cyc_Dump_dumpenumfields( _temp232); Cyc_Dump_dump_symbol(
_tag_arr("\\\\\n", sizeof( unsigned char), 4u)); Cyc_Dump_dump_rb(); return;
_LL150: Cyc_Dump_dump_alpha( _tag_arr("int", sizeof( unsigned char), 4u));
return; _LL152: Cyc_Dump_dump_alpha( _tag_arr("signed char", sizeof(
unsigned char), 12u)); return; _LL154: Cyc_Dump_dump_alpha( _tag_arr("unsigned char",
sizeof( unsigned char), 14u)); return; _LL156: Cyc_Dump_dump_alpha( _tag_arr("short",
sizeof( unsigned char), 6u)); return; _LL158: Cyc_Dump_dump_alpha( _tag_arr("unsigned short",
sizeof( unsigned char), 15u)); return; _LL160: Cyc_Dump_dump_alpha( _tag_arr("unsigned int",
sizeof( unsigned char), 13u)); return; _LL162: Cyc_Dump_dump_alpha( _tag_arr("__int64",
sizeof( unsigned char), 8u)); return; _LL164: Cyc_Dump_dump_alpha( _tag_arr("long long",
sizeof( unsigned char), 10u)); return; _LL166: Cyc_Dump_dump_alpha( _tag_arr("unsigned __int64",
sizeof( unsigned char), 17u)); return; _LL168: Cyc_Dump_dump_alpha( _tag_arr("unsigned long long",
sizeof( unsigned char), 19u)); return; _LL170: Cyc_Dump_dump_alpha( _tag_arr("float",
sizeof( unsigned char), 6u)); return; _LL172: if( _temp276){ Cyc_Dump_dump_alpha(
_tag_arr("long double", sizeof( unsigned char), 12u));} else{ Cyc_Dump_dump_alpha(
_tag_arr("double", sizeof( unsigned char), 7u));} return; _LL174: Cyc_Dump_dump_symbol(
_tag_arr("\\$", sizeof( unsigned char), 3u)); Cyc_Dump_dumpargs( _temp278);
return; _LL176: Cyc_Dump_dump_alpha( _tag_arr("struct", sizeof( unsigned char),
7u)); Cyc_Dump_dumptps( _temp280); return; _LL178: Cyc_Dump_dump_alpha( _tag_arr("struct",
sizeof( unsigned char), 7u)); Cyc_Dump_dumpqvar(( struct _tuple1*) _check_null(
_temp286)); Cyc_Dump_dumptps( _temp284); return; _LL180: Cyc_Dump_dump_alpha(
_tag_arr("union", sizeof( unsigned char), 6u)); Cyc_Dump_dumptps( _temp288);
return; _LL182: Cyc_Dump_dump_alpha( _tag_arr("union", sizeof( unsigned char), 6u));
Cyc_Dump_dumpqvar(( struct _tuple1*) _check_null( _temp294)); Cyc_Dump_dumptps(
_temp292); return; _LL184: Cyc_Dump_dump_alpha( _tag_arr("struct", sizeof(
unsigned char), 7u)); Cyc_Dump_dump_lb(); Cyc_Dump_dumpstructfields( _temp296);
Cyc_Dump_dump_rb(); return; _LL186: Cyc_Dump_dump_alpha( _tag_arr("union",
sizeof( unsigned char), 6u)); Cyc_Dump_dump_lb(); Cyc_Dump_dumpstructfields(
_temp298); Cyc_Dump_dump_rb(); return; _LL188:( Cyc_Dump_dumpqvar( _temp304),
Cyc_Dump_dumptps( _temp302)); return; _LL190: Cyc_Dump_dumprgn( _temp306);
return; _LL192: return; _LL194: return; _LL196: return; _LL198: return; _LL126:;}
void Cyc_Dump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo !=  0){ Cyc_Dump_dump_alphastr((
struct _tagged_arr*) vo->v);}} void Cyc_Dump_dumpfunarg( struct _tuple2* t){((
void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Core_Opt*), struct
Cyc_Core_Opt*)) Cyc_Dump_dumptqtd)((* t).f2,(* t).f3, Cyc_Dump_dumpvaropt,(* t).f1);}
void Cyc_Dump_dump_rgncmp( struct _tuple0* cmp){ struct _tuple0 _temp364; void*
_temp365; void* _temp367; struct _tuple0* _temp362= cmp; _temp364=* _temp362;
_LL368: _temp367= _temp364.f1; goto _LL366; _LL366: _temp365= _temp364.f2; goto
_LL363; _LL363: Cyc_Dump_dumptyp( _temp367); Cyc_Dump_dump_char(( int)'<'); Cyc_Dump_dumptyp(
_temp365);} void Cyc_Dump_dump_rgnpo( struct Cyc_List_List* rgn_po){(( void(*)(
void(* f)( struct _tuple0*), struct Cyc_List_List* l, struct _tagged_arr sep))
Cyc_Dump_dump_sep)( Cyc_Dump_dump_rgncmp, rgn_po, _tag_arr(",", sizeof(
unsigned char), 2u));} void Cyc_Dump_dumpfunargs( struct Cyc_List_List* args,
int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_Core_Opt*
effopt, struct Cyc_List_List* rgn_po){ Cyc_Dump_dump_char(( int)'('); for( 0;
args !=  0; args= args->tl){ Cyc_Dump_dumpfunarg(( struct _tuple2*) args->hd);
if(( args->tl !=  0? 1: c_varargs)? 1: cyc_varargs !=  0){ Cyc_Dump_dump_symbol(
_tag_arr(", ", sizeof( unsigned char), 3u));}} if( c_varargs){ Cyc_Dump_dump_symbol(
_tag_arr("...", sizeof( unsigned char), 4u));} else{ if( cyc_varargs !=  0){
struct _tuple2* _temp369=({ struct _tuple2* _temp370=( struct _tuple2*)
_cycalloc( sizeof( struct _tuple2)); _temp370->f1= cyc_varargs->name; _temp370->f2=
cyc_varargs->tq; _temp370->f3=( void*) cyc_varargs->type; _temp370;}); Cyc_Dump_dump_symbol(
_tag_arr("...", sizeof( unsigned char), 4u)); if( cyc_varargs->inject){ Cyc_Dump_dump_alpha(
_tag_arr(" inject", sizeof( unsigned char), 8u));} Cyc_Dump_dumpfunarg( _temp369);}}
if( effopt !=  0){ Cyc_Dump_dump_semi(); Cyc_Dump_dumpeff(( void*) effopt->v);}
if( rgn_po !=  0){ Cyc_Dump_dump_char(( int)':'); Cyc_Dump_dump_rgnpo( rgn_po);}
Cyc_Dump_dump_char(( int)')');} void Cyc_Dump_dumptyp( void* t){(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Dump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp371; _temp371.q_const= 0; _temp371.q_volatile= 0;
_temp371.q_restrict= 0; _temp371;}), t,( void(*)( int x)) Cyc_Dump_ignore, 0);}
void Cyc_Dump_dumpdesignator( void* d){ void* _temp372= d; struct Cyc_Absyn_Exp*
_temp378; struct _tagged_arr* _temp380; _LL374: if(*(( int*) _temp372) ==  Cyc_Absyn_ArrayElement){
_LL379: _temp378=(( struct Cyc_Absyn_ArrayElement_struct*) _temp372)->f1; goto
_LL375;} else{ goto _LL376;} _LL376: if(*(( int*) _temp372) ==  Cyc_Absyn_FieldName){
_LL381: _temp380=(( struct Cyc_Absyn_FieldName_struct*) _temp372)->f1; goto
_LL377;} else{ goto _LL373;} _LL375: Cyc_Dump_dump_symbol( _tag_arr(".[",
sizeof( unsigned char), 3u)); Cyc_Dump_dumpexp( _temp378); Cyc_Dump_dump_char((
int)']'); goto _LL373; _LL377: Cyc_Dump_dump_char(( int)'.'); Cyc_Dump_dump_alpha(*
_temp380); goto _LL373; _LL373:;} struct _tuple13{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; void Cyc_Dump_dumpde( struct _tuple13* de){ Cyc_Dump_egroup(
Cyc_Dump_dumpdesignator,(* de).f1, _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof( unsigned char),
2u)); Cyc_Dump_dumpexp((* de).f2);} void Cyc_Dump_dumpexps_prec( int inprec,
struct Cyc_List_List* es){(( void(*)( void(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Dump_group_c)( Cyc_Dump_dumpexp_prec, inprec,
es, _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char),
1u), _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Dump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >=  myprec){ Cyc_Dump_dump_char(( int)'(');}{ void* _temp382=( void*) e->r;
void* _temp482; unsigned char _temp484; void* _temp486; void* _temp488; short
_temp490; void* _temp492; void* _temp494; int _temp496; void* _temp498; void*
_temp500; int _temp502; void* _temp504; void* _temp506; long long _temp508; void*
_temp510; void* _temp512; struct _tagged_arr _temp514; void* _temp516; void*
_temp518; struct _tagged_arr _temp520; struct _tuple1* _temp522; struct _tuple1*
_temp524; struct Cyc_List_List* _temp526; void* _temp528; struct Cyc_Absyn_Exp*
_temp530; struct Cyc_Core_Opt* _temp532; struct Cyc_Absyn_Exp* _temp534; void*
_temp536; struct Cyc_Absyn_Exp* _temp538; void* _temp540; struct Cyc_Absyn_Exp*
_temp542; void* _temp544; struct Cyc_Absyn_Exp* _temp546; void* _temp548; struct
Cyc_Absyn_Exp* _temp550; struct Cyc_Absyn_Exp* _temp552; struct Cyc_Absyn_Exp*
_temp554; struct Cyc_Absyn_Exp* _temp556; struct Cyc_Absyn_Exp* _temp558; struct
Cyc_Absyn_Exp* _temp560; struct Cyc_List_List* _temp562; struct Cyc_Absyn_Exp*
_temp564; struct Cyc_List_List* _temp566; struct Cyc_Absyn_Exp* _temp568; struct
Cyc_Absyn_Exp* _temp570; struct Cyc_Absyn_Exp* _temp572; struct Cyc_Absyn_Exp*
_temp574; struct Cyc_Absyn_Exp* _temp576; void* _temp578; struct Cyc_Absyn_Exp*
_temp580; struct Cyc_Absyn_Exp* _temp582; struct Cyc_Absyn_Exp* _temp584; void*
_temp586; struct Cyc_Absyn_Exp* _temp588; void* _temp590; struct _tagged_arr*
_temp592; void* _temp594; void* _temp596; unsigned int _temp598; void* _temp600;
void* _temp602; struct Cyc_List_List* _temp604; struct Cyc_Absyn_Exp* _temp606;
struct _tagged_arr* _temp608; struct Cyc_Absyn_Exp* _temp610; struct _tagged_arr*
_temp612; struct Cyc_Absyn_Exp* _temp614; struct Cyc_Absyn_Exp* _temp616; struct
Cyc_Absyn_Exp* _temp618; struct Cyc_List_List* _temp620; struct Cyc_List_List*
_temp622; struct _tuple2* _temp624; struct Cyc_List_List* _temp626; struct Cyc_Absyn_Exp*
_temp628; struct Cyc_Absyn_Exp* _temp630; struct Cyc_Absyn_Vardecl* _temp632;
struct Cyc_List_List* _temp634; struct _tuple1* _temp636; struct Cyc_List_List*
_temp638; struct Cyc_Absyn_Tunionfield* _temp640; struct Cyc_List_List* _temp642;
struct _tuple1* _temp644; struct _tuple1* _temp646; void* _temp648; struct Cyc_Absyn_Exp*
_temp650; struct Cyc_List_List* _temp652; struct Cyc_Core_Opt* _temp654; struct
Cyc_Absyn_Stmt* _temp656; struct Cyc_Absyn_Fndecl* _temp658; struct Cyc_Absyn_Exp*
_temp660; _LL384: if(*(( int*) _temp382) ==  Cyc_Absyn_Const_e){ _LL483:
_temp482=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp382)->f1; if((
unsigned int) _temp482 >  1u?*(( int*) _temp482) ==  Cyc_Absyn_Char_c: 0){
_LL487: _temp486=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp482)->f1; goto
_LL485; _LL485: _temp484=(( struct Cyc_Absyn_Char_c_struct*) _temp482)->f2; goto
_LL385;} else{ goto _LL386;}} else{ goto _LL386;} _LL386: if(*(( int*) _temp382)
==  Cyc_Absyn_Const_e){ _LL489: _temp488=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp382)->f1; if(( unsigned int) _temp488 >  1u?*(( int*) _temp488) ==  Cyc_Absyn_Short_c:
0){ _LL493: _temp492=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp488)->f1;
goto _LL491; _LL491: _temp490=(( struct Cyc_Absyn_Short_c_struct*) _temp488)->f2;
goto _LL387;} else{ goto _LL388;}} else{ goto _LL388;} _LL388: if(*(( int*)
_temp382) ==  Cyc_Absyn_Const_e){ _LL495: _temp494=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp382)->f1; if(( unsigned int) _temp494 >  1u?*(( int*) _temp494) ==  Cyc_Absyn_Int_c:
0){ _LL499: _temp498=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp494)->f1;
if( _temp498 == ( void*) Cyc_Absyn_Signed){ goto _LL497;} else{ goto _LL390;}
_LL497: _temp496=(( struct Cyc_Absyn_Int_c_struct*) _temp494)->f2; goto _LL389;}
else{ goto _LL390;}} else{ goto _LL390;} _LL390: if(*(( int*) _temp382) ==  Cyc_Absyn_Const_e){
_LL501: _temp500=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp382)->f1; if((
unsigned int) _temp500 >  1u?*(( int*) _temp500) ==  Cyc_Absyn_Int_c: 0){ _LL505:
_temp504=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp500)->f1; if( _temp504
== ( void*) Cyc_Absyn_Unsigned){ goto _LL503;} else{ goto _LL392;} _LL503:
_temp502=(( struct Cyc_Absyn_Int_c_struct*) _temp500)->f2; goto _LL391;} else{
goto _LL392;}} else{ goto _LL392;} _LL392: if(*(( int*) _temp382) ==  Cyc_Absyn_Const_e){
_LL507: _temp506=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp382)->f1; if((
unsigned int) _temp506 >  1u?*(( int*) _temp506) ==  Cyc_Absyn_LongLong_c: 0){
_LL511: _temp510=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp506)->f1;
goto _LL509; _LL509: _temp508=(( struct Cyc_Absyn_LongLong_c_struct*) _temp506)->f2;
goto _LL393;} else{ goto _LL394;}} else{ goto _LL394;} _LL394: if(*(( int*)
_temp382) ==  Cyc_Absyn_Const_e){ _LL513: _temp512=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp382)->f1; if(( unsigned int) _temp512 >  1u?*(( int*) _temp512) ==  Cyc_Absyn_Float_c:
0){ _LL515: _temp514=(( struct Cyc_Absyn_Float_c_struct*) _temp512)->f1; goto
_LL395;} else{ goto _LL396;}} else{ goto _LL396;} _LL396: if(*(( int*) _temp382)
==  Cyc_Absyn_Const_e){ _LL517: _temp516=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp382)->f1; if( _temp516 == ( void*) Cyc_Absyn_Null_c){ goto _LL397;} else{
goto _LL398;}} else{ goto _LL398;} _LL398: if(*(( int*) _temp382) ==  Cyc_Absyn_Const_e){
_LL519: _temp518=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp382)->f1; if((
unsigned int) _temp518 >  1u?*(( int*) _temp518) ==  Cyc_Absyn_String_c: 0){
_LL521: _temp520=(( struct Cyc_Absyn_String_c_struct*) _temp518)->f1; goto
_LL399;} else{ goto _LL400;}} else{ goto _LL400;} _LL400: if(*(( int*) _temp382)
==  Cyc_Absyn_UnknownId_e){ _LL523: _temp522=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp382)->f1; goto _LL401;} else{ goto _LL402;} _LL402: if(*(( int*) _temp382)
==  Cyc_Absyn_Var_e){ _LL525: _temp524=(( struct Cyc_Absyn_Var_e_struct*)
_temp382)->f1; goto _LL403;} else{ goto _LL404;} _LL404: if(*(( int*) _temp382)
==  Cyc_Absyn_Primop_e){ _LL529: _temp528=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp382)->f1; goto _LL527; _LL527: _temp526=(( struct Cyc_Absyn_Primop_e_struct*)
_temp382)->f2; goto _LL405;} else{ goto _LL406;} _LL406: if(*(( int*) _temp382)
==  Cyc_Absyn_AssignOp_e){ _LL535: _temp534=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp382)->f1; goto _LL533; _LL533: _temp532=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp382)->f2; goto _LL531; _LL531: _temp530=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp382)->f3; goto _LL407;} else{ goto _LL408;} _LL408: if(*(( int*) _temp382)
==  Cyc_Absyn_Increment_e){ _LL539: _temp538=(( struct Cyc_Absyn_Increment_e_struct*)
_temp382)->f1; goto _LL537; _LL537: _temp536=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp382)->f2; if( _temp536 == ( void*) Cyc_Absyn_PreInc){ goto _LL409;} else{
goto _LL410;}} else{ goto _LL410;} _LL410: if(*(( int*) _temp382) ==  Cyc_Absyn_Increment_e){
_LL543: _temp542=(( struct Cyc_Absyn_Increment_e_struct*) _temp382)->f1; goto
_LL541; _LL541: _temp540=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp382)->f2; if( _temp540 == ( void*) Cyc_Absyn_PreDec){ goto _LL411;} else{
goto _LL412;}} else{ goto _LL412;} _LL412: if(*(( int*) _temp382) ==  Cyc_Absyn_Increment_e){
_LL547: _temp546=(( struct Cyc_Absyn_Increment_e_struct*) _temp382)->f1; goto
_LL545; _LL545: _temp544=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp382)->f2; if( _temp544 == ( void*) Cyc_Absyn_PostInc){ goto _LL413;} else{
goto _LL414;}} else{ goto _LL414;} _LL414: if(*(( int*) _temp382) ==  Cyc_Absyn_Increment_e){
_LL551: _temp550=(( struct Cyc_Absyn_Increment_e_struct*) _temp382)->f1; goto
_LL549; _LL549: _temp548=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp382)->f2; if( _temp548 == ( void*) Cyc_Absyn_PostDec){ goto _LL415;} else{
goto _LL416;}} else{ goto _LL416;} _LL416: if(*(( int*) _temp382) ==  Cyc_Absyn_Conditional_e){
_LL557: _temp556=(( struct Cyc_Absyn_Conditional_e_struct*) _temp382)->f1; goto
_LL555; _LL555: _temp554=(( struct Cyc_Absyn_Conditional_e_struct*) _temp382)->f2;
goto _LL553; _LL553: _temp552=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp382)->f3; goto _LL417;} else{ goto _LL418;} _LL418: if(*(( int*) _temp382)
==  Cyc_Absyn_SeqExp_e){ _LL561: _temp560=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp382)->f1; goto _LL559; _LL559: _temp558=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp382)->f2; goto _LL419;} else{ goto _LL420;} _LL420: if(*(( int*) _temp382)
==  Cyc_Absyn_UnknownCall_e){ _LL565: _temp564=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp382)->f1; goto _LL563; _LL563: _temp562=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp382)->f2; goto _LL421;} else{ goto _LL422;} _LL422: if(*(( int*) _temp382)
==  Cyc_Absyn_FnCall_e){ _LL569: _temp568=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp382)->f1; goto _LL567; _LL567: _temp566=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp382)->f2; goto _LL423;} else{ goto _LL424;} _LL424: if(*(( int*) _temp382)
==  Cyc_Absyn_Throw_e){ _LL571: _temp570=(( struct Cyc_Absyn_Throw_e_struct*)
_temp382)->f1; goto _LL425;} else{ goto _LL426;} _LL426: if(*(( int*) _temp382)
==  Cyc_Absyn_NoInstantiate_e){ _LL573: _temp572=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp382)->f1; goto _LL427;} else{ goto _LL428;} _LL428: if(*(( int*) _temp382)
==  Cyc_Absyn_Instantiate_e){ _LL575: _temp574=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp382)->f1; goto _LL429;} else{ goto _LL430;} _LL430: if(*(( int*) _temp382)
==  Cyc_Absyn_Cast_e){ _LL579: _temp578=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp382)->f1; goto _LL577; _LL577: _temp576=(( struct Cyc_Absyn_Cast_e_struct*)
_temp382)->f2; goto _LL431;} else{ goto _LL432;} _LL432: if(*(( int*) _temp382)
==  Cyc_Absyn_Address_e){ _LL581: _temp580=(( struct Cyc_Absyn_Address_e_struct*)
_temp382)->f1; goto _LL433;} else{ goto _LL434;} _LL434: if(*(( int*) _temp382)
==  Cyc_Absyn_New_e){ _LL585: _temp584=(( struct Cyc_Absyn_New_e_struct*)
_temp382)->f1; goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_New_e_struct*)
_temp382)->f2; goto _LL435;} else{ goto _LL436;} _LL436: if(*(( int*) _temp382)
==  Cyc_Absyn_Sizeoftyp_e){ _LL587: _temp586=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp382)->f1; goto _LL437;} else{ goto _LL438;} _LL438: if(*(( int*) _temp382)
==  Cyc_Absyn_Sizeofexp_e){ _LL589: _temp588=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp382)->f1; goto _LL439;} else{ goto _LL440;} _LL440: if(*(( int*) _temp382)
==  Cyc_Absyn_Offsetof_e){ _LL595: _temp594=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp382)->f1; goto _LL591; _LL591: _temp590=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp382)->f2; if(*(( int*) _temp590) ==  Cyc_Absyn_StructField){ _LL593:
_temp592=(( struct Cyc_Absyn_StructField_struct*) _temp590)->f1; goto _LL441;}
else{ goto _LL442;}} else{ goto _LL442;} _LL442: if(*(( int*) _temp382) ==  Cyc_Absyn_Offsetof_e){
_LL601: _temp600=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp382)->f1;
goto _LL597; _LL597: _temp596=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp382)->f2; if(*(( int*) _temp596) ==  Cyc_Absyn_TupleIndex){ _LL599:
_temp598=(( struct Cyc_Absyn_TupleIndex_struct*) _temp596)->f1; goto _LL443;}
else{ goto _LL444;}} else{ goto _LL444;} _LL444: if(*(( int*) _temp382) ==  Cyc_Absyn_Gentyp_e){
_LL605: _temp604=(( struct Cyc_Absyn_Gentyp_e_struct*) _temp382)->f1; goto
_LL603; _LL603: _temp602=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp382)->f2;
goto _LL445;} else{ goto _LL446;} _LL446: if(*(( int*) _temp382) ==  Cyc_Absyn_Deref_e){
_LL607: _temp606=(( struct Cyc_Absyn_Deref_e_struct*) _temp382)->f1; goto _LL447;}
else{ goto _LL448;} _LL448: if(*(( int*) _temp382) ==  Cyc_Absyn_StructMember_e){
_LL611: _temp610=(( struct Cyc_Absyn_StructMember_e_struct*) _temp382)->f1; goto
_LL609; _LL609: _temp608=(( struct Cyc_Absyn_StructMember_e_struct*) _temp382)->f2;
goto _LL449;} else{ goto _LL450;} _LL450: if(*(( int*) _temp382) ==  Cyc_Absyn_StructArrow_e){
_LL615: _temp614=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp382)->f1; goto
_LL613; _LL613: _temp612=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp382)->f2;
goto _LL451;} else{ goto _LL452;} _LL452: if(*(( int*) _temp382) ==  Cyc_Absyn_Subscript_e){
_LL619: _temp618=(( struct Cyc_Absyn_Subscript_e_struct*) _temp382)->f1; goto
_LL617; _LL617: _temp616=(( struct Cyc_Absyn_Subscript_e_struct*) _temp382)->f2;
goto _LL453;} else{ goto _LL454;} _LL454: if(*(( int*) _temp382) ==  Cyc_Absyn_Tuple_e){
_LL621: _temp620=(( struct Cyc_Absyn_Tuple_e_struct*) _temp382)->f1; goto _LL455;}
else{ goto _LL456;} _LL456: if(*(( int*) _temp382) ==  Cyc_Absyn_CompoundLit_e){
_LL625: _temp624=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp382)->f1; goto
_LL623; _LL623: _temp622=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp382)->f2;
goto _LL457;} else{ goto _LL458;} _LL458: if(*(( int*) _temp382) ==  Cyc_Absyn_Array_e){
_LL627: _temp626=(( struct Cyc_Absyn_Array_e_struct*) _temp382)->f1; goto _LL459;}
else{ goto _LL460;} _LL460: if(*(( int*) _temp382) ==  Cyc_Absyn_Comprehension_e){
_LL633: _temp632=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp382)->f1;
goto _LL631; _LL631: _temp630=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp382)->f2; goto _LL629; _LL629: _temp628=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp382)->f3; goto _LL461;} else{ goto _LL462;} _LL462: if(*(( int*) _temp382)
==  Cyc_Absyn_Struct_e){ _LL637: _temp636=(( struct Cyc_Absyn_Struct_e_struct*)
_temp382)->f1; goto _LL635; _LL635: _temp634=(( struct Cyc_Absyn_Struct_e_struct*)
_temp382)->f3; goto _LL463;} else{ goto _LL464;} _LL464: if(*(( int*) _temp382)
==  Cyc_Absyn_AnonStruct_e){ _LL639: _temp638=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp382)->f2; goto _LL465;} else{ goto _LL466;} _LL466: if(*(( int*) _temp382)
==  Cyc_Absyn_Tunion_e){ _LL643: _temp642=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp382)->f3; goto _LL641; _LL641: _temp640=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp382)->f5; goto _LL467;} else{ goto _LL468;} _LL468: if(*(( int*) _temp382)
==  Cyc_Absyn_Enum_e){ _LL645: _temp644=(( struct Cyc_Absyn_Enum_e_struct*)
_temp382)->f1; goto _LL469;} else{ goto _LL470;} _LL470: if(*(( int*) _temp382)
==  Cyc_Absyn_AnonEnum_e){ _LL647: _temp646=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp382)->f1; goto _LL471;} else{ goto _LL472;} _LL472: if(*(( int*) _temp382)
==  Cyc_Absyn_Malloc_e){ _LL651: _temp650=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp382)->f1; goto _LL649; _LL649: _temp648=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp382)->f2; goto _LL473;} else{ goto _LL474;} _LL474: if(*(( int*) _temp382)
==  Cyc_Absyn_UnresolvedMem_e){ _LL655: _temp654=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp382)->f1; goto _LL653; _LL653: _temp652=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp382)->f2; goto _LL475;} else{ goto _LL476;} _LL476: if(*(( int*) _temp382)
==  Cyc_Absyn_StmtExp_e){ _LL657: _temp656=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp382)->f1; goto _LL477;} else{ goto _LL478;} _LL478: if(*(( int*) _temp382)
==  Cyc_Absyn_Codegen_e){ _LL659: _temp658=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp382)->f1; goto _LL479;} else{ goto _LL480;} _LL480: if(*(( int*) _temp382)
==  Cyc_Absyn_Fill_e){ _LL661: _temp660=(( struct Cyc_Absyn_Fill_e_struct*)
_temp382)->f1; goto _LL481;} else{ goto _LL383;} _LL385: Cyc_Dump_dump_char((
int)'\''); Cyc_Dump_dump_alpha( Cyc_Absynpp_char_escape( _temp484)); Cyc_Dump_dump_char((
int)'\''); goto _LL383; _LL387: Cyc_Dump_dump_alpha(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp663; _temp663.tag= Cyc_Std_Int_pa; _temp663.f1=(
int)(( unsigned int)(( int) _temp490));{ void* _temp662[ 1u]={& _temp663}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp662, sizeof( void*),
1u));}})); goto _LL383; _LL389: Cyc_Dump_dump_alpha(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp665; _temp665.tag= Cyc_Std_Int_pa; _temp665.f1=(
int)(( unsigned int) _temp496);{ void* _temp664[ 1u]={& _temp665}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp664, sizeof( void*),
1u));}})); goto _LL383; _LL391: Cyc_Dump_dump_alpha(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp667; _temp667.tag= Cyc_Std_Int_pa; _temp667.f1=(
int)(( unsigned int) _temp502);{ void* _temp666[ 1u]={& _temp667}; Cyc_Std_aprintf(
_tag_arr("%du", sizeof( unsigned char), 4u), _tag_arr( _temp666, sizeof( void*),
1u));}})); goto _LL383; _LL393: Cyc_Dump_dump_alpha( _tag_arr("<<FIX LONG LONG CONSTANT>>",
sizeof( unsigned char), 27u)); goto _LL383; _LL395: Cyc_Dump_dump_alpha(
_temp514); goto _LL383; _LL397: Cyc_Dump_dump_alpha( _tag_arr("NULL", sizeof(
unsigned char), 5u)); goto _LL383; _LL399: Cyc_Dump_dump_char(( int)'"'); Cyc_Dump_dump_alpha(
Cyc_Absynpp_string_escape( _temp520)); Cyc_Dump_dump_char(( int)'"'); goto
_LL383; _LL401: _temp524= _temp522; goto _LL403; _LL403: Cyc_Dump_dumpqvar(
_temp524); goto _LL383; _LL405: { struct _tagged_arr _temp668= Cyc_Absynpp_prim2str(
_temp528); switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp526)){ case 1: _LL669: if( _temp528 == ( void*) Cyc_Absyn_Size){ Cyc_Dump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp526))->hd);
Cyc_Dump_dump_char(( int)'.'); Cyc_Dump_dump_alpha( _tag_arr("size", sizeof(
unsigned char), 5u));} else{ Cyc_Dump_dump_symbol( _temp668); Cyc_Dump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp526))->hd);}
break; case 2: _LL670: Cyc_Dump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp526))->hd); Cyc_Dump_dump_symbol(
_temp668); Cyc_Dump_dump_char(( int)' '); Cyc_Dump_dumpexp_prec( myprec,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp526->tl))->hd);
break; default: _LL671:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp673=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp673[ 0]=({ struct Cyc_Core_Failure_struct _temp674; _temp674.tag= Cyc_Core_Failure;
_temp674.f1= _tag_arr("Absyndump -- Bad number of arguments to primop", sizeof(
unsigned char), 47u); _temp674;}); _temp673;}));} goto _LL383;} _LL407: Cyc_Dump_dumpexp_prec(
myprec, _temp534); if( _temp532 !=  0){ Cyc_Dump_dump_symbol( Cyc_Absynpp_prim2str((
void*) _temp532->v));} Cyc_Dump_dump_symbol( _tag_arr("=", sizeof( unsigned char),
2u)); Cyc_Dump_dumpexp_prec( myprec, _temp530); goto _LL383; _LL409: Cyc_Dump_dump_symbol(
_tag_arr("++", sizeof( unsigned char), 3u)); Cyc_Dump_dumpexp_prec( myprec,
_temp538); goto _LL383; _LL411: Cyc_Dump_dump_symbol( _tag_arr("--", sizeof(
unsigned char), 3u)); Cyc_Dump_dumpexp_prec( myprec, _temp542); goto _LL383;
_LL413: Cyc_Dump_dumpexp_prec( myprec, _temp546); Cyc_Dump_dump_symbol( _tag_arr("++",
sizeof( unsigned char), 3u)); goto _LL383; _LL415: Cyc_Dump_dumpexp_prec( myprec,
_temp550); Cyc_Dump_dump_symbol( _tag_arr("--", sizeof( unsigned char), 3u));
goto _LL383; _LL417: Cyc_Dump_dumpexp_prec( myprec, _temp556); Cyc_Dump_dump_char((
int)'?'); Cyc_Dump_dumpexp_prec( 0, _temp554); Cyc_Dump_dump_char(( int)':');
Cyc_Dump_dumpexp_prec( myprec, _temp552); goto _LL383; _LL419: Cyc_Dump_dump_char((
int)'('); Cyc_Dump_dumpexp_prec( myprec, _temp560); Cyc_Dump_dump_char(( int)',');
Cyc_Dump_dumpexp_prec( myprec, _temp558); Cyc_Dump_dump_char(( int)')'); goto
_LL383; _LL421: _temp568= _temp564; _temp566= _temp562; goto _LL423; _LL423: Cyc_Dump_dumpexp_prec(
myprec, _temp568); Cyc_Dump_dump_char(( int)'('); Cyc_Dump_dumpexps_prec( 20,
_temp566); Cyc_Dump_dump_char(( int)')'); goto _LL383; _LL425: Cyc_Dump_dump_alpha(
_tag_arr("throw", sizeof( unsigned char), 6u)); Cyc_Dump_dumpexp_prec( myprec,
_temp570); goto _LL383; _LL427: _temp574= _temp572; goto _LL429; _LL429: Cyc_Dump_dumpexp_prec(
inprec, _temp574); goto _LL383; _LL431: Cyc_Dump_dump_char(( int)'('); Cyc_Dump_dumptyp(
_temp578); Cyc_Dump_dump_char(( int)')'); Cyc_Dump_dumpexp_prec( myprec,
_temp576); goto _LL383; _LL433: Cyc_Dump_dump_char(( int)'&'); Cyc_Dump_dumpexp_prec(
myprec, _temp580); goto _LL383; _LL435: Cyc_Dump_dump_alpha( _tag_arr("new",
sizeof( unsigned char), 4u)); Cyc_Dump_dumpexp_prec( myprec, _temp582); goto
_LL383; _LL437: Cyc_Dump_dump_alpha( _tag_arr("sizeof", sizeof( unsigned char),
7u)); Cyc_Dump_dump_char(( int)'('); Cyc_Dump_dumptyp( _temp586); Cyc_Dump_dump_char((
int)')'); goto _LL383; _LL439: Cyc_Dump_dump_alpha( _tag_arr("sizeof", sizeof(
unsigned char), 7u)); Cyc_Dump_dump_char(( int)'('); Cyc_Dump_dumpexp_prec(
myprec, _temp588); Cyc_Dump_dump_char(( int)')'); goto _LL383; _LL441: Cyc_Dump_dump_alpha(
_tag_arr("offsetof", sizeof( unsigned char), 9u)); Cyc_Dump_dump_char(( int)'(');
Cyc_Dump_dumptyp( _temp594); Cyc_Dump_dump_char(( int)','); Cyc_Dump_dump_alpha(*
_temp592); Cyc_Dump_dump_char(( int)')'); goto _LL383; _LL443: Cyc_Dump_dump_alpha(
_tag_arr("offsetof(", sizeof( unsigned char), 10u)); Cyc_Dump_dumptyp( _temp600);
Cyc_Dump_dump_char(( int)','); Cyc_Dump_dump_alpha(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp676; _temp676.tag= Cyc_Std_Int_pa; _temp676.f1=(
int) _temp598;{ void* _temp675[ 1u]={& _temp676}; Cyc_Std_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp675, sizeof( void*), 1u));}})); Cyc_Dump_dump_char((
int)')'); goto _LL383; _LL445: Cyc_Dump_dump_alpha( _tag_arr("__gen", sizeof(
unsigned char), 6u)); Cyc_Dump_dumptvars( _temp604); Cyc_Dump_dump_char(( int)'(');
Cyc_Dump_dumptyp( _temp602); Cyc_Dump_dump_char(( int)')'); goto _LL383; _LL447:
Cyc_Dump_dump_char(( int)'*'); Cyc_Dump_dumpexp_prec( myprec, _temp606); goto
_LL383; _LL449: Cyc_Dump_dumpexp_prec( myprec, _temp610); Cyc_Dump_dump_char((
int)'.'); Cyc_Dump_dump_alpha(* _temp608); goto _LL383; _LL451: Cyc_Dump_dumpexp_prec(
myprec, _temp614); Cyc_Dump_dump_symbol( _tag_arr("->", sizeof( unsigned char),
3u)); Cyc_Dump_dump_alpha(* _temp612); goto _LL383; _LL453: Cyc_Dump_dumpexp_prec(
myprec, _temp618); Cyc_Dump_dump_char(( int)'['); Cyc_Dump_dumpexp( _temp616);
Cyc_Dump_dump_char(( int)']'); goto _LL383; _LL455: Cyc_Dump_dump_symbol(
_tag_arr("\\$(", sizeof( unsigned char), 4u)); Cyc_Dump_dumpexps_prec( 20,
_temp620); Cyc_Dump_dump_char(( int)')'); goto _LL383; _LL457: Cyc_Dump_dump_char((
int)'('); Cyc_Dump_dumptyp((* _temp624).f3); Cyc_Dump_dump_char(( int)')');((
void(*)( void(* f)( struct _tuple13*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)(
Cyc_Dump_dumpde, _temp622, _tag_arr("{\\lb}", sizeof( unsigned char), 6u),
_tag_arr("{\\rb}", sizeof( unsigned char), 6u), _tag_arr(",", sizeof(
unsigned char), 2u)); goto _LL383; _LL459:(( void(*)( void(* f)( struct _tuple13*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dumpde, _temp626, _tag_arr("{\\lb}",
sizeof( unsigned char), 6u), _tag_arr("{\\rb}", sizeof( unsigned char), 6u),
_tag_arr(",", sizeof( unsigned char), 2u)); goto _LL383; _LL461: Cyc_Dump_dump_alpha(
_tag_arr("new", sizeof( unsigned char), 4u)); Cyc_Dump_dump_char(( int)'{'); Cyc_Dump_dump_alpha(
_tag_arr("for", sizeof( unsigned char), 4u)); Cyc_Dump_dump_alphastr((* _temp632->name).f2);
Cyc_Dump_dump_char(( int)'<'); Cyc_Dump_dumpexp( _temp630); Cyc_Dump_dump_char((
int)':'); Cyc_Dump_dumpexp( _temp628); Cyc_Dump_dump_char(( int)'}'); goto
_LL383; _LL463: Cyc_Dump_dumpqvar( _temp636);(( void(*)( void(* f)( struct
_tuple13*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dumpde,
_temp634, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL383;
_LL465:(( void(*)( void(* f)( struct _tuple13*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)(
Cyc_Dump_dumpde, _temp638, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto
_LL383; _LL467: Cyc_Dump_dumpqvar( _temp640->name); if( _temp642 !=  0){(( void(*)(
void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct _tagged_arr
start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dumpexp,
_temp642, _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u));} goto _LL383;
_LL469: Cyc_Dump_dumpqvar( _temp644); goto _LL383; _LL471: Cyc_Dump_dumpqvar(
_temp646); goto _LL383; _LL473: if( _temp650 !=  0){ Cyc_Dump_dump_alpha(
_tag_arr("rmalloc", sizeof( unsigned char), 8u)); Cyc_Dump_dump_char(( int)'(');
Cyc_Dump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp650)); Cyc_Dump_dump_symbol(
_tag_arr(", ", sizeof( unsigned char), 3u));} else{ Cyc_Dump_dump_alpha(
_tag_arr("malloc", sizeof( unsigned char), 7u)); Cyc_Dump_dump_char(( int)'(');}
Cyc_Dump_dump_alpha( _tag_arr("sizeof", sizeof( unsigned char), 7u)); Cyc_Dump_dump_char((
int)'('); Cyc_Dump_dumptyp( _temp648); Cyc_Dump_dump_symbol( _tag_arr("))",
sizeof( unsigned char), 3u)); goto _LL383; _LL475:(( void(*)( void(* f)( struct
_tuple13*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dumpde,
_temp652, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL383;
_LL477: goto _LL383; _LL479: goto _LL383; _LL481: goto _LL383; _LL383:;} if(
inprec >=  myprec){ Cyc_Dump_dump_char(( int)')');}} void Cyc_Dump_dumpexp(
struct Cyc_Absyn_Exp* e){ Cyc_Dump_dumpexp_prec( 0, e);} void Cyc_Dump_dumptunionfield(
struct Cyc_Absyn_Tunionfield* ef){ Cyc_Dump_dumpqvar( ef->name); if( ef->typs != 
0){ Cyc_Dump_dumpargs( ef->typs);}} void Cyc_Dump_dumptunionfields( struct Cyc_List_List*
fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List*
l, struct _tagged_arr sep)) Cyc_Dump_dump_sep)( Cyc_Dump_dumptunionfield, fields,
_tag_arr(",\\\\\n~~", sizeof( unsigned char), 7u));} void Cyc_Dump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Dump_dumpqvar( ef->name); if( ef->tag !=  0){
Cyc_Dump_dump_symbol( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_Dump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( ef->tag));}} void Cyc_Dump_dumpenumfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List* l, struct _tagged_arr sep)) Cyc_Dump_dump_sep)( Cyc_Dump_dumpenumfield,
fields, _tag_arr(",\\\\\n~~", sizeof( unsigned char), 7u));} void Cyc_Dump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields !=  0; fields= fields->tl){ Cyc_Dump_dump_symbol(
_tag_arr("~~", sizeof( unsigned char), 3u));{ struct Cyc_Absyn_Structfield
_temp679; struct Cyc_List_List* _temp680; struct Cyc_Absyn_Exp* _temp682; void*
_temp684; struct Cyc_Absyn_Tqual _temp686; struct _tagged_arr* _temp688; struct
Cyc_Absyn_Structfield* _temp677=( struct Cyc_Absyn_Structfield*) fields->hd;
_temp679=* _temp677; _LL689: _temp688= _temp679.name; goto _LL687; _LL687:
_temp686= _temp679.tq; goto _LL685; _LL685: _temp684=( void*) _temp679.type;
goto _LL683; _LL683: _temp682= _temp679.width; goto _LL681; _LL681: _temp680=
_temp679.attributes; goto _LL678; _LL678:(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct _tagged_arr*), struct _tagged_arr*)) Cyc_Dump_dumptqtd)(
_temp686, _temp684, Cyc_Dump_dump_alphastr, _temp688); if( _temp682 !=  0){ Cyc_Dump_dump_char((
int)':'); Cyc_Dump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp682));}
Cyc_Dump_dump_symbol( _tag_arr(";\\\\\n", sizeof( unsigned char), 5u));}}} void
Cyc_Dump_dumptypedefname( struct Cyc_Absyn_Typedefdecl* td){ Cyc_Dump_dumpqvar(
td->name); Cyc_Dump_dumptvars( td->tvs);} void Cyc_Dump_dump_boldtypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Dump_dump_boldqvar( td->name); Cyc_Dump_dumptvars(
td->tvs);} static void Cyc_Dump_dumpids( struct Cyc_List_List* vds){ for( 0; vds
!=  0; vds= vds->tl){ Cyc_Dump_dumpqvar((( struct Cyc_Absyn_Vardecl*) vds->hd)->name);
if( vds->tl !=  0){ Cyc_Dump_dump_char(( int)',');}}} static void Cyc_Dump_dump_upperbound(
struct Cyc_Absyn_Exp* e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if(
i !=  1){ Cyc_Dump_dump_char(( int)'{'); Cyc_Dump_dumpexp( e); Cyc_Dump_dump_char((
int)'}');}} void Cyc_Dump_dumptms( struct Cyc_List_List* tms, void(* f)( void*),
void* a){ if( tms ==  0){ f( a); return;}{ void* _temp690=( void*) tms->hd;
struct Cyc_Absyn_Tqual _temp712; void* _temp714; void* _temp716; struct Cyc_Absyn_Exp*
_temp718; struct Cyc_Absyn_Tqual _temp720; void* _temp722; void* _temp724;
struct Cyc_Absyn_Exp* _temp726; struct Cyc_Absyn_Tqual _temp728; void* _temp730;
void* _temp732; struct Cyc_Absyn_Tqual _temp734; void* _temp736; struct Cyc_Absyn_Tvar*
_temp738; void* _temp740; struct Cyc_Absyn_Exp* _temp742; struct Cyc_Absyn_Tqual
_temp744; void* _temp746; struct Cyc_Absyn_Tvar* _temp748; void* _temp750;
struct Cyc_Absyn_Exp* _temp752; struct Cyc_Absyn_Tqual _temp754; void* _temp756;
struct Cyc_Absyn_Tvar* _temp758; void* _temp760; struct Cyc_Absyn_Tqual _temp762;
void* _temp764; struct Cyc_Absyn_Exp* _temp766; struct Cyc_Absyn_Tqual _temp768;
void* _temp770; struct Cyc_Absyn_Exp* _temp772; struct Cyc_Absyn_Tqual _temp774;
void* _temp776; _LL692: if(( unsigned int) _temp690 >  1u?*(( int*) _temp690) == 
Cyc_Absyn_Pointer_mod: 0){ _LL717: _temp716=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp690)->f1; if(( unsigned int) _temp716 >  1u?*(( int*) _temp716) ==  Cyc_Absyn_Nullable_ps:
0){ _LL719: _temp718=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp716)->f1;
goto _LL715;} else{ goto _LL694;} _LL715: _temp714=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp690)->f2; if( _temp714 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL713;} else{
goto _LL694;} _LL713: _temp712=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f3;
goto _LL693;} else{ goto _LL694;} _LL694: if(( unsigned int) _temp690 >  1u?*((
int*) _temp690) ==  Cyc_Absyn_Pointer_mod: 0){ _LL725: _temp724=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f1; if(( unsigned int) _temp724
>  1u?*(( int*) _temp724) ==  Cyc_Absyn_NonNullable_ps: 0){ _LL727: _temp726=((
struct Cyc_Absyn_NonNullable_ps_struct*) _temp724)->f1; goto _LL723;} else{ goto
_LL696;} _LL723: _temp722=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp690)->f2; if( _temp722 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL721;} else{
goto _LL696;} _LL721: _temp720=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f3;
goto _LL695;} else{ goto _LL696;} _LL696: if(( unsigned int) _temp690 >  1u?*((
int*) _temp690) ==  Cyc_Absyn_Pointer_mod: 0){ _LL733: _temp732=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f1; if( _temp732 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL731;} else{ goto _LL698;} _LL731: _temp730=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f2; if( _temp730 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL729;} else{ goto _LL698;} _LL729: _temp728=((
struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f3; goto _LL697;} else{ goto
_LL698;} _LL698: if(( unsigned int) _temp690 >  1u?*(( int*) _temp690) ==  Cyc_Absyn_Pointer_mod:
0){ _LL741: _temp740=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f1;
if(( unsigned int) _temp740 >  1u?*(( int*) _temp740) ==  Cyc_Absyn_Nullable_ps:
0){ _LL743: _temp742=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp740)->f1;
goto _LL737;} else{ goto _LL700;} _LL737: _temp736=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp690)->f2; if(( unsigned int) _temp736 >  3u?*(( int*) _temp736) ==  Cyc_Absyn_VarType:
0){ _LL739: _temp738=(( struct Cyc_Absyn_VarType_struct*) _temp736)->f1; goto
_LL735;} else{ goto _LL700;} _LL735: _temp734=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp690)->f3; goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int)
_temp690 >  1u?*(( int*) _temp690) ==  Cyc_Absyn_Pointer_mod: 0){ _LL751:
_temp750=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f1; if((
unsigned int) _temp750 >  1u?*(( int*) _temp750) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL753: _temp752=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp750)->f1; goto
_LL747;} else{ goto _LL702;} _LL747: _temp746=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp690)->f2; if(( unsigned int) _temp746 >  3u?*(( int*) _temp746) ==  Cyc_Absyn_VarType:
0){ _LL749: _temp748=(( struct Cyc_Absyn_VarType_struct*) _temp746)->f1; goto
_LL745;} else{ goto _LL702;} _LL745: _temp744=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp690)->f3; goto _LL701;} else{ goto _LL702;} _LL702: if(( unsigned int)
_temp690 >  1u?*(( int*) _temp690) ==  Cyc_Absyn_Pointer_mod: 0){ _LL761:
_temp760=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f1; if(
_temp760 == ( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL757;} else{ goto _LL704;}
_LL757: _temp756=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f2;
if(( unsigned int) _temp756 >  3u?*(( int*) _temp756) ==  Cyc_Absyn_VarType: 0){
_LL759: _temp758=(( struct Cyc_Absyn_VarType_struct*) _temp756)->f1; goto _LL755;}
else{ goto _LL704;} _LL755: _temp754=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp690)->f3; goto _LL703;} else{ goto _LL704;} _LL704: if(( unsigned int)
_temp690 >  1u?*(( int*) _temp690) ==  Cyc_Absyn_Pointer_mod: 0){ _LL765:
_temp764=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f1; if((
unsigned int) _temp764 >  1u?*(( int*) _temp764) ==  Cyc_Absyn_Nullable_ps: 0){
_LL767: _temp766=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp764)->f1; goto
_LL763;} else{ goto _LL706;} _LL763: _temp762=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp690)->f3; goto _LL705;} else{ goto _LL706;} _LL706: if(( unsigned int)
_temp690 >  1u?*(( int*) _temp690) ==  Cyc_Absyn_Pointer_mod: 0){ _LL771:
_temp770=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f1; if((
unsigned int) _temp770 >  1u?*(( int*) _temp770) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL773: _temp772=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp770)->f1; goto
_LL769;} else{ goto _LL708;} _LL769: _temp768=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp690)->f3; goto _LL707;} else{ goto _LL708;} _LL708: if(( unsigned int)
_temp690 >  1u?*(( int*) _temp690) ==  Cyc_Absyn_Pointer_mod: 0){ _LL777:
_temp776=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f1; if(
_temp776 == ( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL775;} else{ goto _LL710;}
_LL775: _temp774=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp690)->f3; goto
_LL709;} else{ goto _LL710;} _LL710: goto _LL711; _LL693: Cyc_Dump_dump_pointer(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_Dump_dump_upperbound( _temp718);
Cyc_Dump_dump_alpha( _tag_arr("`H", sizeof( unsigned char), 3u)); Cyc_Dump_dumptms(
tms->tl, f, a); return; _LL695: Cyc_Dump_dump_pointer( _tag_arr("@", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_upperbound( _temp726); Cyc_Dump_dump_alpha(
_tag_arr("`H", sizeof( unsigned char), 3u)); Cyc_Dump_dumptms( tms->tl, f, a);
return; _LL697: Cyc_Dump_dump_pointer( _tag_arr("{?}", sizeof( unsigned char), 4u));
Cyc_Dump_dump_alpha( _tag_arr("`H", sizeof( unsigned char), 3u)); Cyc_Dump_dumptms(
tms->tl, f, a); return; _LL699: Cyc_Dump_dump_pointer( _tag_arr("*", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_upperbound( _temp742); Cyc_Dump_dump_alphastr(
_temp738->name); Cyc_Dump_dumptms( tms->tl, f, a); return; _LL701: Cyc_Dump_dump_pointer(
_tag_arr("@", sizeof( unsigned char), 2u)); Cyc_Dump_dump_upperbound( _temp752);
Cyc_Dump_dump_alphastr( _temp748->name); Cyc_Dump_dumptms( tms->tl, f, a);
return; _LL703: Cyc_Dump_dump_pointer( _tag_arr("{?}", sizeof( unsigned char), 4u));
Cyc_Dump_dump_alphastr( _temp758->name); Cyc_Dump_dumptms( tms->tl, f, a);
return; _LL705: Cyc_Dump_dump_pointer( _tag_arr("*", sizeof( unsigned char), 2u));
Cyc_Dump_dump_upperbound( _temp766); Cyc_Dump_dumptms( tms->tl, f, a); return;
_LL707: Cyc_Dump_dump_pointer( _tag_arr("@", sizeof( unsigned char), 2u)); Cyc_Dump_dump_upperbound(
_temp772); Cyc_Dump_dumptms( tms->tl, f, a); return; _LL709: Cyc_Dump_dump_pointer(
_tag_arr("{?}", sizeof( unsigned char), 4u)); Cyc_Dump_dumptms( tms->tl, f, a);
return; _LL711: { int next_is_pointer= 0; if( tms->tl !=  0){ void* _temp778=(
void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd; _LL780: if((
unsigned int) _temp778 >  1u?*(( int*) _temp778) ==  Cyc_Absyn_Pointer_mod: 0){
goto _LL781;} else{ goto _LL782;} _LL782: goto _LL783; _LL781: next_is_pointer=
1; goto _LL779; _LL783: goto _LL779; _LL779:;} if( next_is_pointer){ Cyc_Dump_dump_char((
int)'(');} Cyc_Dump_dumptms( tms->tl, f, a); if( next_is_pointer){ Cyc_Dump_dump_char((
int)')');}{ void* _temp784=( void*) tms->hd; struct Cyc_Absyn_Exp* _temp800;
void* _temp802; struct Cyc_List_List* _temp804; struct Cyc_Core_Opt* _temp806;
struct Cyc_Absyn_VarargInfo* _temp808; int _temp810; struct Cyc_List_List*
_temp812; void* _temp814; struct Cyc_Position_Segment* _temp816; struct Cyc_List_List*
_temp818; int _temp820; struct Cyc_Position_Segment* _temp822; struct Cyc_List_List*
_temp824; void* _temp826; void* _temp828; _LL786: if( _temp784 == ( void*) Cyc_Absyn_Carray_mod){
goto _LL787;} else{ goto _LL788;} _LL788: if(( unsigned int) _temp784 >  1u?*((
int*) _temp784) ==  Cyc_Absyn_ConstArray_mod: 0){ _LL801: _temp800=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp784)->f1; goto _LL789;} else{ goto _LL790;} _LL790: if(( unsigned int)
_temp784 >  1u?*(( int*) _temp784) ==  Cyc_Absyn_Function_mod: 0){ _LL803:
_temp802=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp784)->f1; if(*((
int*) _temp802) ==  Cyc_Absyn_WithTypes){ _LL813: _temp812=(( struct Cyc_Absyn_WithTypes_struct*)
_temp802)->f1; goto _LL811; _LL811: _temp810=(( struct Cyc_Absyn_WithTypes_struct*)
_temp802)->f2; goto _LL809; _LL809: _temp808=(( struct Cyc_Absyn_WithTypes_struct*)
_temp802)->f3; goto _LL807; _LL807: _temp806=(( struct Cyc_Absyn_WithTypes_struct*)
_temp802)->f4; goto _LL805; _LL805: _temp804=(( struct Cyc_Absyn_WithTypes_struct*)
_temp802)->f5; goto _LL791;} else{ goto _LL792;}} else{ goto _LL792;} _LL792:
if(( unsigned int) _temp784 >  1u?*(( int*) _temp784) ==  Cyc_Absyn_Function_mod:
0){ _LL815: _temp814=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp784)->f1;
if(*(( int*) _temp814) ==  Cyc_Absyn_NoTypes){ _LL819: _temp818=(( struct Cyc_Absyn_NoTypes_struct*)
_temp814)->f1; goto _LL817; _LL817: _temp816=(( struct Cyc_Absyn_NoTypes_struct*)
_temp814)->f2; goto _LL793;} else{ goto _LL794;}} else{ goto _LL794;} _LL794:
if(( unsigned int) _temp784 >  1u?*(( int*) _temp784) ==  Cyc_Absyn_TypeParams_mod:
0){ _LL825: _temp824=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp784)->f1;
goto _LL823; _LL823: _temp822=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp784)->f2; goto _LL821; _LL821: _temp820=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp784)->f3; goto _LL795;} else{ goto _LL796;} _LL796: if(( unsigned int)
_temp784 >  1u?*(( int*) _temp784) ==  Cyc_Absyn_Attributes_mod: 0){ goto _LL797;}
else{ goto _LL798;} _LL798: if(( unsigned int) _temp784 >  1u?*(( int*) _temp784)
==  Cyc_Absyn_Pointer_mod: 0){ _LL829: _temp828=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp784)->f1; goto _LL827; _LL827: _temp826=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp784)->f2; goto _LL799;} else{ goto _LL785;} _LL787: Cyc_Dump_dump_symbol(
_tag_arr("[]", sizeof( unsigned char), 3u)); goto _LL785; _LL789: Cyc_Dump_dump_char((
int)'['); Cyc_Dump_dumpexp( _temp800); Cyc_Dump_dump_char(( int)']'); goto
_LL785; _LL791: Cyc_Dump_dumpfunargs( _temp812, _temp810, _temp808, _temp806,
_temp804); goto _LL785; _LL793:(( void(*)( void(* f)( struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dump_alphastr, _temp818,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(", ", sizeof( unsigned char), 3u)); goto _LL785; _LL795: if(
_temp820){ Cyc_Dump_dumpkindedtvars( _temp824);} else{ Cyc_Dump_dumptvars(
_temp824);} goto _LL785; _LL797: goto _LL785; _LL799:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp830=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp830[ 0]=({ struct
Cyc_Core_Impossible_struct _temp831; _temp831.tag= Cyc_Core_Impossible; _temp831.f1=
_tag_arr("dumptms", sizeof( unsigned char), 8u); _temp831;}); _temp830;}));
_LL785:;} return;} _LL691:;}} void Cyc_Dump_dumptqtd( struct Cyc_Absyn_Tqual tq,
void* t, void(* f)( void*), void* a){ struct Cyc_List_List* _temp834; void*
_temp836; struct Cyc_Absyn_Tqual _temp838; struct _tuple9 _temp832= Cyc_Absynpp_to_tms(
tq, t); _LL839: _temp838= _temp832.f1; goto _LL837; _LL837: _temp836= _temp832.f2;
goto _LL835; _LL835: _temp834= _temp832.f3; goto _LL833; _LL833: Cyc_Dump_dumptq(
_temp838); Cyc_Dump_dumpntyp( _temp836); Cyc_Dump_dumptms( Cyc_List_imp_rev(
_temp834), f, a);} void Cyc_Dump_pr_comment( struct Cyc_Std___sFILE* outf,
struct _tagged_arr s){ int depth= 0; int len=( int) Cyc_Std_strlen( s); int i= 0;
for( 0; i <  len; i ++){ unsigned char c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c != '['){ Cyc_Std_fputc((
int) c, outf); continue;} Cyc_Std_fputs( _tag_arr("\\texttt{", sizeof(
unsigned char), 9u), outf); i ++; depth ++; for( 0; i <  len; i ++){
unsigned char c=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( c == ']'){ depth --; if( depth ==  0){ Cyc_Std_fputc((
int)'}', outf); break;}} else{ if( c == '['){ depth ++;}} Cyc_Std_fputc(( int) c,
outf);}}} void Cyc_Dump_dump_comment( struct _tagged_arr s){ Cyc_Dump_pr_comment(
Cyc_Std_stdout, s);} void Cyc_Dump_dumpdecl( struct Cyc_Absyn_Decl* d, struct
_tagged_arr comment){ void* _temp840=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp866; struct Cyc_Absyn_Uniondecl* _temp868; struct Cyc_Absyn_Vardecl*
_temp870; struct Cyc_Absyn_Vardecl _temp872; void* _temp873; struct Cyc_Absyn_Tqual
_temp875; struct _tuple1* _temp877; struct Cyc_Absyn_Tuniondecl* _temp879;
struct Cyc_Absyn_Tuniondecl _temp881; int _temp882; struct Cyc_Core_Opt*
_temp884; struct Cyc_List_List* _temp886; struct _tuple1* _temp888; void*
_temp890; struct Cyc_Absyn_Enumdecl* _temp892; struct Cyc_Absyn_Enumdecl
_temp894; struct Cyc_Core_Opt* _temp895; struct _tuple1* _temp897; void*
_temp899; struct Cyc_Absyn_Typedefdecl* _temp901; _LL842: if(*(( int*) _temp840)
==  Cyc_Absyn_Struct_d){ _LL867: _temp866=(( struct Cyc_Absyn_Struct_d_struct*)
_temp840)->f1; goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp840)
==  Cyc_Absyn_Union_d){ _LL869: _temp868=(( struct Cyc_Absyn_Union_d_struct*)
_temp840)->f1; goto _LL845;} else{ goto _LL846;} _LL846: if(*(( int*) _temp840)
==  Cyc_Absyn_Var_d){ _LL871: _temp870=(( struct Cyc_Absyn_Var_d_struct*)
_temp840)->f1; _temp872=* _temp870; _LL878: _temp877= _temp872.name; goto _LL876;
_LL876: _temp875= _temp872.tq; goto _LL874; _LL874: _temp873=( void*) _temp872.type;
goto _LL847;} else{ goto _LL848;} _LL848: if(*(( int*) _temp840) ==  Cyc_Absyn_Tunion_d){
_LL880: _temp879=(( struct Cyc_Absyn_Tunion_d_struct*) _temp840)->f1; _temp881=*
_temp879; _LL891: _temp890=( void*) _temp881.sc; goto _LL889; _LL889: _temp888=
_temp881.name; goto _LL887; _LL887: _temp886= _temp881.tvs; goto _LL885; _LL885:
_temp884= _temp881.fields; goto _LL883; _LL883: _temp882= _temp881.is_xtunion;
goto _LL849;} else{ goto _LL850;} _LL850: if(*(( int*) _temp840) ==  Cyc_Absyn_Enum_d){
_LL893: _temp892=(( struct Cyc_Absyn_Enum_d_struct*) _temp840)->f1; _temp894=*
_temp892; _LL900: _temp899=( void*) _temp894.sc; goto _LL898; _LL898: _temp897=
_temp894.name; goto _LL896; _LL896: _temp895= _temp894.fields; goto _LL851;}
else{ goto _LL852;} _LL852: if(*(( int*) _temp840) ==  Cyc_Absyn_Typedef_d){
_LL902: _temp901=(( struct Cyc_Absyn_Typedef_d_struct*) _temp840)->f1; goto
_LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp840) ==  Cyc_Absyn_Fn_d){
goto _LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp840) ==  Cyc_Absyn_Let_d){
goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp840) ==  Cyc_Absyn_Letv_d){
goto _LL859;} else{ goto _LL860;} _LL860: if(*(( int*) _temp840) ==  Cyc_Absyn_Namespace_d){
goto _LL861;} else{ goto _LL862;} _LL862: if(*(( int*) _temp840) ==  Cyc_Absyn_Using_d){
goto _LL863;} else{ goto _LL864;} _LL864: if(*(( int*) _temp840) ==  Cyc_Absyn_ExternC_d){
goto _LL865;} else{ goto _LL841;} _LL843: Cyc_Dump_dump_begin(); Cyc_Dump_dump_alpha(
_tag_arr("struct", sizeof( unsigned char), 7u)); if( _temp866->name !=  0){ Cyc_Dump_dump_boldqvar((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp866->name))->v);} Cyc_Dump_dumptvars(
_temp866->tvs); if( _temp866->fields ==  0){ Cyc_Dump_dump_symbol( _tag_arr(";\n",
sizeof( unsigned char), 3u));} else{ Cyc_Dump_dump_symbol( _tag_arr(" ", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_lb(); Cyc_Dump_dump_symbol( _tag_arr("\\\\\n",
sizeof( unsigned char), 4u)); Cyc_Dump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp866->fields))->v); Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char), 3u));} Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment( comment); Cyc_Dump_dump_symbol( _tag_arr("\n", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_end(); goto _LL841; _LL845: Cyc_Dump_dump_begin();
Cyc_Dump_dump_alpha( _tag_arr("union", sizeof( unsigned char), 6u)); if(
_temp868->name !=  0){ Cyc_Dump_dump_boldqvar(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp868->name))->v);} Cyc_Dump_dumptvars( _temp868->tvs); if(
_temp868->fields ==  0){ Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof(
unsigned char), 3u));} else{ Cyc_Dump_dump_symbol( _tag_arr(" ", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_lb(); Cyc_Dump_dump_symbol( _tag_arr("\\\\\n",
sizeof( unsigned char), 4u)); Cyc_Dump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp868->fields))->v); Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char), 3u));} Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment( comment); Cyc_Dump_dump_symbol( _tag_arr("\n", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_end(); goto _LL841; _LL847: Cyc_Dump_dump_begin();((
void(*)( struct Cyc_Absyn_Tqual tq, void* t, void(* f)( struct _tuple1*), struct
_tuple1* a)) Cyc_Dump_dumptqtd)( _temp875, _temp873, Cyc_Dump_dump_boldqvar,
_temp877); Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char), 3u));
Cyc_Dump_dump_middle(); Cyc_Dump_dump_comment( comment); Cyc_Dump_dump_symbol(
_tag_arr("\n", sizeof( unsigned char), 2u)); Cyc_Dump_dump_end(); goto _LL841;
_LL849: Cyc_Dump_dump_begin(); if( _temp882){ Cyc_Dump_dump_alpha( _tag_arr("xtunion",
sizeof( unsigned char), 8u));} else{ Cyc_Dump_dump_alpha( _tag_arr("tunion",
sizeof( unsigned char), 7u));} Cyc_Dump_dump_boldqvar( _temp888); Cyc_Dump_dumptvars(
_temp886); if( _temp884 ==  0){ Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof(
unsigned char), 3u));} else{ Cyc_Dump_dump_symbol( _tag_arr(" ", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_lb(); Cyc_Dump_dump_symbol( _tag_arr("\\\\\n~~",
sizeof( unsigned char), 6u)); Cyc_Dump_dumptunionfields(( struct Cyc_List_List*)
_temp884->v); Cyc_Dump_dump_symbol( _tag_arr("\\\\\n", sizeof( unsigned char), 4u));
Cyc_Dump_dump_rb(); Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char),
3u));} Cyc_Dump_dump_middle(); Cyc_Dump_dump_comment( comment); Cyc_Dump_dump_symbol(
_tag_arr("\n", sizeof( unsigned char), 2u)); Cyc_Dump_dump_end(); goto _LL841;
_LL851: Cyc_Dump_dump_begin(); Cyc_Dump_dump_alpha( _tag_arr("enum", sizeof(
unsigned char), 5u)); Cyc_Dump_dump_boldqvar( _temp897); if( _temp895 ==  0){
Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char), 3u));} else{ Cyc_Dump_dump_symbol(
_tag_arr(" ", sizeof( unsigned char), 2u)); Cyc_Dump_dump_lb(); Cyc_Dump_dump_symbol(
_tag_arr("\\\\\n~~", sizeof( unsigned char), 6u)); Cyc_Dump_dumpenumfields((
struct Cyc_List_List*) _temp895->v); Cyc_Dump_dump_symbol( _tag_arr("\\\\\n",
sizeof( unsigned char), 4u)); Cyc_Dump_dump_rb(); Cyc_Dump_dump_symbol( _tag_arr(";\n",
sizeof( unsigned char), 3u));} Cyc_Dump_dump_middle(); Cyc_Dump_dump_comment(
comment); Cyc_Dump_dump_symbol( _tag_arr("\n", sizeof( unsigned char), 2u)); Cyc_Dump_dump_end();
goto _LL841; _LL853: Cyc_Dump_dump_begin(); Cyc_Dump_dump_alpha( _tag_arr("typedef",
sizeof( unsigned char), 8u));(( void(*)( struct Cyc_Absyn_Tqual tq, void* t,
void(* f)( struct Cyc_Absyn_Typedefdecl*), struct Cyc_Absyn_Typedefdecl* a)) Cyc_Dump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp903; _temp903.q_const= 0; _temp903.q_volatile= 0;
_temp903.q_restrict= 0; _temp903;}),( void*) _temp901->defn, Cyc_Dump_dump_boldtypedefname,
_temp901); Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char), 3u));
Cyc_Dump_dump_middle(); Cyc_Dump_dump_comment( comment); Cyc_Dump_dump_symbol(
_tag_arr("\n", sizeof( unsigned char), 2u)); Cyc_Dump_dump_end(); goto _LL841;
_LL855: goto _LL841; _LL857: goto _LL841; _LL859: goto _LL841; _LL861: goto
_LL841; _LL863: goto _LL841; _LL865: goto _LL841; _LL841:;} static int Cyc_is_other_special(
unsigned char c){ switch( c){ case '\\': _LL904: goto _LL905; case '"': _LL905:
goto _LL906; case ';': _LL906: goto _LL907; case '&': _LL907: goto _LL908; case
'(': _LL908: goto _LL909; case ')': _LL909: goto _LL910; case '|': _LL910: goto
_LL911; case '^': _LL911: goto _LL912; case '<': _LL912: goto _LL913; case '>':
_LL913: goto _LL914; case ' ': _LL914: goto _LL915; case '\n': _LL915: goto
_LL916; case '\t': _LL916: return 1; default: _LL917: return 0;}} static struct
_tagged_arr Cyc_sh_escape_string( struct _tagged_arr s){ unsigned int _temp919=
Cyc_Std_strlen( s); int _temp920= 0; int _temp921= 0;{ int i= 0; for( 0; i < 
_temp919; i ++){ unsigned char _temp922=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( _temp922 == '\''){
_temp920 ++;} else{ if( Cyc_is_other_special( _temp922)){ _temp921 ++;}}}} if(
_temp920 ==  0? _temp921 ==  0: 0){ return s;} if( _temp920 ==  0){ return(
struct _tagged_arr) Cyc_Std_strconcat_l(({ struct _tagged_arr* _temp923[ 3u];
_temp923[ 2u]= _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)),"'", sizeof( unsigned char), 2u); _temp923[ 1u]=({ struct
_tagged_arr* _temp924=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp924[ 0]=( struct _tagged_arr) s; _temp924;}); _temp923[ 0u]=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"'",
sizeof( unsigned char), 2u);(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp923, sizeof( struct _tagged_arr*), 3u));}));}{
unsigned int _temp925=( _temp919 +  _temp920) +  _temp921; struct _tagged_arr
_temp926=({ unsigned int _temp930= _temp925 +  1; unsigned char* _temp931=(
unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char), _temp930));
struct _tagged_arr _temp933= _tag_arr( _temp931, sizeof( unsigned char),
_temp925 +  1);{ unsigned int _temp932= _temp930; unsigned int i; for( i= 0; i < 
_temp932; i ++){ _temp931[ i]='\000';}}; _temp933;}); int _temp927= 0; int
_temp928= 0; for( 0; _temp927 <  _temp919; _temp927 ++){ unsigned char _temp929=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
_temp927)); if( _temp929 == '\''? 1: Cyc_is_other_special( _temp929)){*((
unsigned char*) _check_unknown_subscript( _temp926, sizeof( unsigned char),
_temp928 ++))='\\';}*(( unsigned char*) _check_unknown_subscript( _temp926,
sizeof( unsigned char), _temp928 ++))= _temp929;} return( struct _tagged_arr)
_temp926;}} static struct _tagged_arr* Cyc_sh_escape_stringptr( struct
_tagged_arr* sp){ return({ struct _tagged_arr* _temp934=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp934[ 0]= Cyc_sh_escape_string(* sp);
_temp934;});} static struct Cyc_Lineno_Pos* Cyc_new_pos(){ return({ struct Cyc_Lineno_Pos*
_temp935=( struct Cyc_Lineno_Pos*) _cycalloc( sizeof( struct Cyc_Lineno_Pos));
_temp935->logical_file= _tag_arr("", sizeof( unsigned char), 1u); _temp935->line=
Cyc_Core_new_string( 0); _temp935->line_no= 0; _temp935->col= 0; _temp935;});}
struct _tuple14{ int f1; struct Cyc_Lineno_Pos* f2; } ; static struct _tuple14*
Cyc_start2pos( int x){ return({ struct _tuple14* _temp936=( struct _tuple14*)
_cycalloc( sizeof( struct _tuple14)); _temp936->f1= x; _temp936->f2= Cyc_new_pos();
_temp936;});} static int Cyc_decl2start( struct Cyc_Absyn_Decl* d){ return((
struct Cyc_Position_Segment*) _check_null( d->loc))->start;} struct _tuple15{
struct Cyc_Lineno_Pos* f1; void* f2; } ; static struct Cyc_List_List* Cyc_this_file(
struct _tagged_arr file, struct Cyc_List_List* x){ struct Cyc_List_List*
_temp937= 0; for( 0; x !=  0; x= x->tl){ if( Cyc_Std_strcmp(((*(( struct
_tuple15*) x->hd)).f1)->logical_file, file) ==  0){ _temp937=({ struct Cyc_List_List*
_temp938=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp938->hd=( void*)(( struct _tuple15*) x->hd); _temp938->tl= _temp937;
_temp938;});}} _temp937=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp937); return _temp937;} static int Cyc_lineno( struct
Cyc_Lineno_Pos* p){ return p->line_no;} static struct Cyc_List_List* Cyc_flatten_decls(
struct Cyc_List_List* decls){ struct Cyc_List_List* _temp939= 0; while( decls != 
0) { void* _temp940=( void*)(( struct Cyc_Absyn_Decl*) decls->hd)->r; struct Cyc_List_List*
_temp966; struct Cyc_List_List* _temp968; struct Cyc_List_List* _temp970; _LL942:
if(*(( int*) _temp940) ==  Cyc_Absyn_Namespace_d){ _LL967: _temp966=(( struct
Cyc_Absyn_Namespace_d_struct*) _temp940)->f2; goto _LL943;} else{ goto _LL944;}
_LL944: if(*(( int*) _temp940) ==  Cyc_Absyn_Using_d){ _LL969: _temp968=((
struct Cyc_Absyn_Using_d_struct*) _temp940)->f2; goto _LL945;} else{ goto _LL946;}
_LL946: if(*(( int*) _temp940) ==  Cyc_Absyn_ExternC_d){ _LL971: _temp970=((
struct Cyc_Absyn_ExternC_d_struct*) _temp940)->f1; goto _LL947;} else{ goto
_LL948;} _LL948: if(*(( int*) _temp940) ==  Cyc_Absyn_Struct_d){ goto _LL949;}
else{ goto _LL950;} _LL950: if(*(( int*) _temp940) ==  Cyc_Absyn_Union_d){ goto
_LL951;} else{ goto _LL952;} _LL952: if(*(( int*) _temp940) ==  Cyc_Absyn_Var_d){
goto _LL953;} else{ goto _LL954;} _LL954: if(*(( int*) _temp940) ==  Cyc_Absyn_Tunion_d){
goto _LL955;} else{ goto _LL956;} _LL956: if(*(( int*) _temp940) ==  Cyc_Absyn_Enum_d){
goto _LL957;} else{ goto _LL958;} _LL958: if(*(( int*) _temp940) ==  Cyc_Absyn_Typedef_d){
goto _LL959;} else{ goto _LL960;} _LL960: if(*(( int*) _temp940) ==  Cyc_Absyn_Fn_d){
goto _LL961;} else{ goto _LL962;} _LL962: if(*(( int*) _temp940) ==  Cyc_Absyn_Let_d){
goto _LL963;} else{ goto _LL964;} _LL964: if(*(( int*) _temp940) ==  Cyc_Absyn_Letv_d){
goto _LL965;} else{ goto _LL941;} _LL943: _temp968= _temp966; goto _LL945;
_LL945: _temp970= _temp968; goto _LL947; _LL947: decls=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp970,
decls->tl); goto _LL941; _LL949: goto _LL951; _LL951: goto _LL953; _LL953: goto
_LL955; _LL955: goto _LL957; _LL957: goto _LL959; _LL959: goto _LL961; _LL961:
goto _LL963; _LL963: goto _LL965; _LL965: _temp939=({ struct Cyc_List_List*
_temp972=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp972->hd=( void*)(( struct Cyc_Absyn_Decl*) decls->hd); _temp972->tl=
_temp939; _temp972;}); decls= decls->tl; goto _LL941; _LL941:;} return(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp939);}
static struct Cyc_List_List* Cyc_cycdoc_files= 0; static void Cyc_add_other(
struct _tagged_arr s){ Cyc_cycdoc_files=({ struct Cyc_List_List* _temp973=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp973->hd=(
void*)({ struct _tagged_arr* _temp974=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp974[ 0]= s; _temp974;}); _temp973->tl= Cyc_cycdoc_files;
_temp973;});} static struct Cyc_List_List* Cyc_cycargs= 0; static void Cyc_add_cycarg(
struct _tagged_arr s){ Cyc_cycargs=({ struct Cyc_List_List* _temp975=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp975->hd=( void*)({
struct _tagged_arr* _temp976=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp976[ 0]= s; _temp976;}); _temp975->tl= Cyc_cycargs; _temp975;});}
static unsigned char _temp977[ 8u]="cyclone"; static struct _tagged_arr Cyc_cyclone_file={
_temp977, _temp977, _temp977 +  8u}; static void Cyc_set_cyclone_file( struct
_tagged_arr s){ Cyc_cyclone_file= s;} struct _tuple16{ int f1; struct Cyc_Absyn_Decl*
f2; } ; struct _tuple17{ struct Cyc_Lineno_Pos* f1; struct Cyc_Absyn_Decl* f2; }
; static void Cyc_process_file( struct _tagged_arr filename){ struct _tagged_arr
_temp978= Cyc_Filename_chop_extension( filename); struct _tagged_arr _temp979=
Cyc_Std_strconcat(( struct _tagged_arr) _temp978, _tag_arr(".cyp", sizeof(
unsigned char), 5u)); struct _tagged_arr _temp980= Cyc_Std_str_sepstr(({ struct
Cyc_List_List* _temp1028=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1028->hd=( void*) _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)),"", sizeof( unsigned char), 1u); _temp1028->tl=(( struct
Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_sh_escape_stringptr,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( Cyc_cycargs)); _temp1028;}), _tag_arr(" ",
sizeof( unsigned char), 2u)); struct _tagged_arr _temp981=({ struct Cyc_Std_String_pa_struct
_temp1027; _temp1027.tag= Cyc_Std_String_pa; _temp1027.f1=( struct _tagged_arr)
Cyc_sh_escape_string( filename);{ struct Cyc_Std_String_pa_struct _temp1026;
_temp1026.tag= Cyc_Std_String_pa; _temp1026.f1=( struct _tagged_arr) Cyc_sh_escape_string((
struct _tagged_arr) _temp979);{ struct Cyc_Std_String_pa_struct _temp1025;
_temp1025.tag= Cyc_Std_String_pa; _temp1025.f1=( struct _tagged_arr) _temp980;{
struct Cyc_Std_String_pa_struct _temp1024; _temp1024.tag= Cyc_Std_String_pa;
_temp1024.f1=( struct _tagged_arr) Cyc_cyclone_file;{ void* _temp1023[ 4u]={&
_temp1024,& _temp1025,& _temp1026,& _temp1027}; Cyc_Std_aprintf( _tag_arr("%s %s -E -o %s -x cyc %s",
sizeof( unsigned char), 25u), _tag_arr( _temp1023, sizeof( void*), 4u));}}}}});
if( Cyc_Std_system( _tag_arr( string_to_Cstring(( struct _tagged_arr) _temp981),
sizeof( unsigned char), 0u)) !=  0){({ void* _temp982[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("\nError: preprocessing\n", sizeof( unsigned char), 23u),
_tag_arr( _temp982, sizeof( void*), 0u));}); return;} Cyc_Position_reset_position((
struct _tagged_arr) _temp979);{ struct Cyc_Std___sFILE* in_file=( struct Cyc_Std___sFILE*)
_check_null( Cyc_Std_fopen(( struct _tagged_arr) _temp979, _tag_arr("r", sizeof(
unsigned char), 2u))); Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp983= Cyc_Parse_parse_file(
in_file); Cyc_Lex_lex_init(); Cyc_Std_file_close(( struct Cyc_Std___sFILE*)
in_file); _temp983= Cyc_flatten_decls( _temp983);{ struct Cyc_List_List*
_temp984=(( struct Cyc_List_List*(*)( struct _tuple14*(* f)( int), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_start2pos,(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_decl2start,
_temp983)); Cyc_Lineno_poss_of_abss(( struct _tagged_arr) _temp979, _temp984);
Cyc_Std_remove(( struct _tagged_arr) _temp979);{ struct Cyc_List_List* _temp985=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(((
struct Cyc_List_List*(*)( struct Cyc_Lineno_Pos*(* f)( struct _tuple14*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Lineno_Pos*(*)( struct _tuple14*))
Cyc_Core_snd, _temp984), _temp983); _temp985=(( struct Cyc_List_List*(*)( struct
_tagged_arr file, struct Cyc_List_List* x)) Cyc_this_file)( filename, _temp985);{
struct Cyc_List_List* _temp986=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)((( struct Cyc_List_List*(*)( int(* f)(
struct Cyc_Lineno_Pos*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_lineno,((
struct Cyc_List_List*(*)( struct Cyc_Lineno_Pos*(* f)( struct _tuple17*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Lineno_Pos*(*)( struct _tuple17*))
Cyc_Core_fst, _temp985)),(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)(
struct _tuple17*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Decl*(*)(
struct _tuple17*)) Cyc_Core_snd, _temp985)); struct Cyc_Std___sFILE* _temp987=(
struct Cyc_Std___sFILE*) _check_null( Cyc_Std_fopen( filename, _tag_arr("r",
sizeof( unsigned char), 2u))); struct Cyc_Lexing_lexbuf* _temp988= Cyc_Lexing_from_file(
_temp987); struct Cyc_List_List* _temp989= 0; struct _tuple10* comment; while((
comment=(( struct _tuple10*(*)( struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_token)(
_temp988)) !=  0) { _temp989=({ struct Cyc_List_List* _temp990=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp990->hd=( void*)(( struct
_tuple10*) _check_null( comment)); _temp990->tl= _temp989; _temp990;});} Cyc_Std_fclose(
_temp987); _temp989=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp989);{ struct Cyc_List_List* _temp991=(( struct Cyc_List_List*(*)( struct
_tuple14*(* f)( int), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_start2pos,((
struct Cyc_List_List*(*)( int(* f)( struct _tuple10*), struct Cyc_List_List* x))
Cyc_List_map)(( int(*)( struct _tuple10*)) Cyc_Core_fst, _temp989)); Cyc_Lineno_poss_of_abss(
filename, _temp991);{ struct Cyc_List_List* _temp992=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)((( struct Cyc_List_List*(*)(
struct Cyc_Lineno_Pos*(* f)( struct _tuple14*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)( struct _tuple14*)) Cyc_Core_snd, _temp991),(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple10*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple10*)) Cyc_Core_snd, _temp989)); _temp992= Cyc_this_file(
filename, _temp992);{ struct Cyc_List_List* _temp993=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)((( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Lineno_Pos*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_lineno,((
struct Cyc_List_List*(*)( struct Cyc_Lineno_Pos*(* f)( struct _tuple15*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Lineno_Pos*(*)( struct _tuple15*))
Cyc_Core_fst, _temp992)),(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple15*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple15*))
Cyc_Core_snd, _temp992)); while( _temp986 !=  0? _temp993 !=  0: 0) { struct
_tuple16 _temp996; struct Cyc_Absyn_Decl* _temp997; int _temp999; struct
_tuple16* _temp994=( struct _tuple16*) _temp986->hd; _temp996=* _temp994;
_LL1000: _temp999= _temp996.f1; goto _LL998; _LL998: _temp997= _temp996.f2; goto
_LL995; _LL995: { struct _tuple10 _temp1003; void* _temp1004; int _temp1006;
struct _tuple10* _temp1001=( struct _tuple10*) _temp993->hd; _temp1003=*
_temp1001; _LL1007: _temp1006= _temp1003.f1; goto _LL1005; _LL1005: _temp1004=
_temp1003.f2; goto _LL1002; _LL1002: { void* _temp1008= _temp1004; struct
_tagged_arr _temp1014; struct _tagged_arr _temp1016; _LL1010: if(*(( int*)
_temp1008) ==  Cyc_Standalone){ _LL1015: _temp1014=(( struct Cyc_Standalone_struct*)
_temp1008)->f1; goto _LL1011;} else{ goto _LL1012;} _LL1012: if(*(( int*)
_temp1008) ==  Cyc_MatchDecl){ _LL1017: _temp1016=(( struct Cyc_MatchDecl_struct*)
_temp1008)->f1; goto _LL1013;} else{ goto _LL1009;} _LL1011: Cyc_Dump_dump_comment((
struct _tagged_arr) _temp1014); Cyc_Dump_dump_symbol( _tag_arr("\n", sizeof(
unsigned char), 2u)); _temp993= _temp993->tl; goto _LL1009; _LL1013: if(
_temp1006 <  _temp999){ _temp993= _temp993->tl; continue;} if( _temp986->tl != 
0){ struct _tuple16 _temp1020; int _temp1021; struct _tuple16* _temp1018=(
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp986->tl))->hd;
_temp1020=* _temp1018; _LL1022: _temp1021= _temp1020.f1; goto _LL1019; _LL1019:
if( _temp1021 <  _temp1006){ _temp986= _temp986->tl; continue;}} Cyc_Dump_dumpdecl(
_temp997,( struct _tagged_arr) _temp1016); Cyc_Std_fflush(( struct Cyc_Std___sFILE*)
Cyc_Std_stdout); _temp986= _temp986->tl; _temp993= _temp993->tl; goto _LL1009;
_LL1009:;}}}}}}}}}}}} struct _tuple18{ struct _tagged_arr f1; int f2; struct
_tagged_arr f3; void* f4; struct _tagged_arr f5; } ; int Cyc_main( int argc,
struct _tagged_arr argv){ struct Cyc_List_List* options=({ struct _tuple18*
_temp1030[ 4u]; _temp1030[ 3u]=({ struct _tuple18* _temp1040=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp1040->f1= _tag_arr("-B", sizeof(
unsigned char), 3u); _temp1040->f2= 1; _temp1040->f3= _tag_arr("<file>", sizeof(
unsigned char), 7u); _temp1040->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp1041=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp1041[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp1042; _temp1042.tag= Cyc_Arg_Flag_spec;
_temp1042.f1= Cyc_add_cycarg; _temp1042;}); _temp1041;}); _temp1040->f5=
_tag_arr("Add to the list of directories to search for compiler files", sizeof(
unsigned char), 60u); _temp1040;}); _temp1030[ 2u]=({ struct _tuple18* _temp1037=(
struct _tuple18*) _cycalloc( sizeof( struct _tuple18)); _temp1037->f1= _tag_arr("-I",
sizeof( unsigned char), 3u); _temp1037->f2= 1; _temp1037->f3= _tag_arr("<dir>",
sizeof( unsigned char), 6u); _temp1037->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp1038=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp1038[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp1039; _temp1039.tag= Cyc_Arg_Flag_spec;
_temp1039.f1= Cyc_add_cycarg; _temp1039;}); _temp1038;}); _temp1037->f5=
_tag_arr("Add to the list of directories to search for include files", sizeof(
unsigned char), 59u); _temp1037;}); _temp1030[ 1u]=({ struct _tuple18* _temp1034=(
struct _tuple18*) _cycalloc( sizeof( struct _tuple18)); _temp1034->f1= _tag_arr("-D",
sizeof( unsigned char), 3u); _temp1034->f2= 1; _temp1034->f3= _tag_arr("<name>[=<value>]",
sizeof( unsigned char), 17u); _temp1034->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp1035=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp1035[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp1036; _temp1036.tag= Cyc_Arg_Flag_spec;
_temp1036.f1= Cyc_add_cycarg; _temp1036;}); _temp1035;}); _temp1034->f5=
_tag_arr("Pass definition to preprocessor", sizeof( unsigned char), 32u);
_temp1034;}); _temp1030[ 0u]=({ struct _tuple18* _temp1031=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp1031->f1= _tag_arr("-cyclone",
sizeof( unsigned char), 9u); _temp1031->f2= 0; _temp1031->f3= _tag_arr(" <file>",
sizeof( unsigned char), 8u); _temp1031->f4=( void*)({ struct Cyc_Arg_String_spec_struct*
_temp1032=( struct Cyc_Arg_String_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp1032[ 0]=({ struct Cyc_Arg_String_spec_struct _temp1033; _temp1033.tag= Cyc_Arg_String_spec;
_temp1033.f1= Cyc_set_cyclone_file; _temp1033;}); _temp1032;}); _temp1031->f5=
_tag_arr("Use <file> as the cyclone compiler", sizeof( unsigned char), 35u);
_temp1031;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1030, sizeof( struct _tuple18*), 4u));}); Cyc_Arg_parse( options,
Cyc_add_other, _tag_arr("Options:", sizeof( unsigned char), 9u), argv);{ struct
Cyc_List_List* _temp1029=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( Cyc_cycdoc_files); for( 0; _temp1029 !=  0; _temp1029= _temp1029->tl){
Cyc_process_file(*(( struct _tagged_arr*) _temp1029->hd));}} return 0;}
