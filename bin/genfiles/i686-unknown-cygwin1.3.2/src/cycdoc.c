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
14u]; extern int system( unsigned char*); struct Cyc_Std__Div{ int quot; int rem;
} ; struct Cyc_Std__Ldiv{ int quot; int rem; } ; extern int abs( int __x);
extern int atexit( void(* __func)()); extern struct Cyc_Std__Div div( int
__numer, int __denom); extern struct Cyc_Std__Ldiv ldiv( int __numer, int
__denom); extern int random(); extern void srandom( unsigned int __seed); extern
int rand(); extern void srand( unsigned int __seed); extern int rand_r(
unsigned int* __seed); extern int grantpt( int __fd); extern int unlockpt( int
__fd); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; extern
unsigned char Cyc_Arg_Bad[ 8u]; struct Cyc_Arg_Bad_struct{ unsigned char* tag;
struct _tagged_arr f1; } ; extern unsigned char Cyc_Arg_Error[ 10u]; static
const int Cyc_Arg_Unit_spec= 0; struct Cyc_Arg_Unit_spec_struct{ int tag; void(*
f1)(); } ; static const int Cyc_Arg_Flag_spec= 1; struct Cyc_Arg_Flag_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; static const int Cyc_Arg_FlagString_spec=
2; struct Cyc_Arg_FlagString_spec_struct{ int tag; void(* f1)( struct
_tagged_arr, struct _tagged_arr); } ; static const int Cyc_Arg_Set_spec= 3;
struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec=
4; struct Cyc_Arg_Clear_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_String_spec=
5; struct Cyc_Arg_String_spec_struct{ int tag; void(* f1)( struct _tagged_arr);
} ; static const int Cyc_Arg_Int_spec= 6; struct Cyc_Arg_Int_spec_struct{ int
tag; void(* f1)( int); } ; static const int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; extern void Cyc_Arg_parse( struct
Cyc_List_List* specs, void(* anonfun)( struct _tagged_arr), struct _tagged_arr
errmsg, struct _tagged_arr args); struct _tuple1{ void* f1; struct _tagged_arr*
f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref;
struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_SwitchC_clause; struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl;
struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield;
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract=
1; static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
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
0; struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars;
struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args; int
c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple1* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
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
const int Cyc_Absyn_DoubleType= 2; static const int Cyc_Absyn_ArrayType= 6;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual f2;
struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 8;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_StructType= 9; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff= 16;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 18; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e= 14; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static const
int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e= 16;
struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Gentyp_e=
19; struct Cyc_Absyn_Gentyp_e_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_Deref_e= 20; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_StructMember_e= 21; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 22; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
23; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 24; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
25; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 26; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
27; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
28; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 29; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 30;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 32; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
33; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 34; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 35; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 36; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
f4; } ; static const int Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_SwitchC_s= 10; struct Cyc_Absyn_SwitchC_s_struct{ int tag; struct
Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Fallthru_s=
11; struct Cyc_Absyn_Fallthru_s_struct{ int tag; struct Cyc_List_List* f1;
struct Cyc_Absyn_Switch_clause** f2; } ; static const int Cyc_Absyn_Decl_s= 12;
struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Cut_s= 13; struct Cyc_Absyn_Cut_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Splice_s= 14;
struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static
const int Cyc_Absyn_Label_s= 15; struct Cyc_Absyn_Label_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
16; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct
_tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 18; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List*
non_local_preds; int try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p=
0; static const int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Null_p= 1; static
const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1;
int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
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
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_UnknownId_p= 10;
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct
Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_SwitchC_clause{
struct Cyc_Absyn_Exp* cnst_exp; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment*
loc; } ; static const int Cyc_Absyn_Unresolved_b= 0; static const int Cyc_Absyn_Global_b=
0; struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Funname_b= 1; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Param_b= 2;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Local_b= 3; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Pat_b= 4; struct
Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct Cyc_Absyn_Vardecl{
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE* f);
struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
extern int Cyc_yyparse(); extern unsigned char Cyc_AbstractDeclarator_tok[ 27u];
struct Cyc_AbstractDeclarator_tok_struct{ unsigned char* tag; struct Cyc_Abstractdeclarator*
f1; } ; extern unsigned char Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[
18u]; struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
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
Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls;
struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict*
typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List* availables; }
; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0; static const
int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2; static const
int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{
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
Cyc_Dump_to_VC= 0; void Cyc_Dump_dumptyp( void*); void Cyc_Dump_dumpntyp( void*
t); void Cyc_Dump_dumpexp( struct Cyc_Absyn_Exp*); void Cyc_Dump_dumpexp_prec(
int, struct Cyc_Absyn_Exp*); void Cyc_Dump_dumpdecl( struct Cyc_Absyn_Decl*,
struct _tagged_arr); void Cyc_Dump_dumptms( struct Cyc_List_List*, void(* f)(
void*), void*); void Cyc_Dump_dumptqtd( struct Cyc_Absyn_Tqual, void*, void(* f)(
void*), void*); void Cyc_Dump_dumpstructfields( struct Cyc_List_List* fields);
void Cyc_Dump_ignore( void* x){ return;} void Cyc_Dump_dump_semi(){ Cyc_Dump_dump_char((
int)';');} void Cyc_Dump_dump_sep( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_arr sep){ if( l ==  0){ return;} for( 0;(( struct Cyc_List_List*)
_check_null( l))->tl !=  0; l=(( struct Cyc_List_List*) _check_null( l))->tl){ f((
void*)(( struct Cyc_List_List*) _check_null( l))->hd); Cyc_Dump_dump_symbol( sep);}
f(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);} void Cyc_Dump_dump_sep_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_arr
sep){ if( l ==  0){ return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl
!=  0; l=(( struct Cyc_List_List*) _check_null( l))->tl){ f( env,( void*)((
struct Cyc_List_List*) _check_null( l))->hd); Cyc_Dump_dump_symbol( sep);} f(
env,( void*)(( struct Cyc_List_List*) _check_null( l))->hd);} void Cyc_Dump_group(
void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep){ Cyc_Dump_dump_symbol( start); Cyc_Dump_dump_sep(
f, l, sep); Cyc_Dump_dump_symbol( end);} void Cyc_Dump_group_c( void(* f)( void*,
void*), void* env, struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep){ Cyc_Dump_dump_symbol( start); Cyc_Dump_dump_sep_c(
f, env, l, sep); Cyc_Dump_dump_symbol( end);} void Cyc_Dump_egroup( void(* f)(
void*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep){ if( l !=  0){ Cyc_Dump_group( f, l, start, end,
sep);}} void Cyc_Dump_dumpqvar( struct _tuple1* v){ struct Cyc_List_List*
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
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp32))->hd); for(
_temp32=(( struct Cyc_List_List*) _check_null( _temp32))->tl; _temp32 !=  0;
_temp32=(( struct Cyc_List_List*) _check_null( _temp32))->tl){ if( Cyc_Dump_qvar_to_Cids){
Cyc_Dump_dump_char(( int)'_');} else{ Cyc_Dump_dump_symbol( _tag_arr("::",
sizeof( unsigned char), 3u));} Cyc_Dump_dump_alpha(*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( _temp32))->hd));} if( Cyc_Dump_qvar_to_Cids){
Cyc_Dump_dump_symbol( _tag_arr("_", sizeof( unsigned char), 2u));} else{ Cyc_Dump_dump_symbol(
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
tv->name);{ void* _temp70=( void*)( Cyc_Absyn_compress_conref( tv->kind))->v;
void* _temp78; void* _temp80; _LL72: if(( unsigned int) _temp70 >  1u?*(( int*)
_temp70) ==  Cyc_Absyn_Eq_constr: 0){ _LL79: _temp78=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp70)->f1; if( _temp78 == ( void*) Cyc_Absyn_BoxKind){ goto _LL73;} else{
goto _LL74;}} else{ goto _LL74;} _LL74: if(( unsigned int) _temp70 >  1u?*(( int*)
_temp70) ==  Cyc_Absyn_Eq_constr: 0){ _LL81: _temp80=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp70)->f1; goto _LL75;} else{ goto _LL76;} _LL76: goto _LL77; _LL73: goto
_LL71; _LL75: Cyc_Dump_dump_symbol( _tag_arr("::", sizeof( unsigned char), 3u));
Cyc_Dump_dumpkind( _temp80); goto _LL71; _LL77: Cyc_Dump_dump_symbol( _tag_arr("::?",
sizeof( unsigned char), 4u)); goto _LL71; _LL71:;}} void Cyc_Dump_dumptvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Dump_egroup)( Cyc_Dump_dumptvar, tvs, _tag_arr("<",
sizeof( unsigned char), 2u), _tag_arr(">", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u)); Cyc_Dump_dump_alpha( _tag_arr("", sizeof(
unsigned char), 1u));} void Cyc_Dump_dumpkindedtvars( struct Cyc_List_List* tvs){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_egroup)(
Cyc_Dump_dumpkindedtvar, tvs, _tag_arr("<", sizeof( unsigned char), 2u),
_tag_arr(">", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); Cyc_Dump_dump_alpha( _tag_arr("", sizeof( unsigned char), 1u));} struct
_tuple11{ struct Cyc_Absyn_Tqual f1; void* f2; } ; void Cyc_Dump_dumparg( struct
_tuple11* pr){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int), int))
Cyc_Dump_dumptqtd)((* pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Dump_ignore, 0);}
void Cyc_Dump_dumpargs( struct Cyc_List_List* ts){(( void(*)( void(* f)( struct
_tuple11*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dumparg, ts,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(", ", sizeof( unsigned char), 3u));} int Cyc_Dump_next_is_pointer(
struct Cyc_List_List* tms){ if( tms ==  0){ return 0;}{ void* _temp82=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; _LL84: if(( unsigned int) _temp82
>  1u?*(( int*) _temp82) ==  Cyc_Absyn_Pointer_mod: 0){ goto _LL85;} else{ goto
_LL86;} _LL86: goto _LL87; _LL85: return 1; _LL87: return 0; _LL83:;}} static
void Cyc_Dump_dumprgn( void* t){ void* _temp88= Cyc_Tcutil_compress( t); _LL90:
if( _temp88 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL91;} else{ goto _LL92;}
_LL92: goto _LL93; _LL91: Cyc_Dump_dump_alpha( _tag_arr("`H", sizeof(
unsigned char), 3u)); goto _LL89; _LL93: Cyc_Dump_dumpntyp( t); goto _LL89;
_LL89:;} struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; }
; static struct _tuple12 Cyc_Dump_effects_split( void* t){ struct Cyc_List_List*
rgions= 0; struct Cyc_List_List* effects= 0;{ void* _temp94= Cyc_Tcutil_compress(
t); void* _temp102; struct Cyc_List_List* _temp104; _LL96: if(( unsigned int)
_temp94 >  4u?*(( int*) _temp94) ==  Cyc_Absyn_AccessEff: 0){ _LL103: _temp102=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp94)->f1; goto _LL97;} else{
goto _LL98;} _LL98: if(( unsigned int) _temp94 >  4u?*(( int*) _temp94) ==  Cyc_Absyn_JoinEff:
0){ _LL105: _temp104=(( struct Cyc_Absyn_JoinEff_struct*) _temp94)->f1; goto
_LL99;} else{ goto _LL100;} _LL100: goto _LL101; _LL97: rgions=({ struct Cyc_List_List*
_temp106=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp106->hd=( void*) _temp102; _temp106->tl= rgions; _temp106;}); goto _LL95;
_LL99: for( 0; _temp104 !=  0; _temp104=(( struct Cyc_List_List*) _check_null(
_temp104))->tl){ struct Cyc_List_List* _temp109; struct Cyc_List_List* _temp111;
struct _tuple12 _temp107= Cyc_Dump_effects_split(( void*)(( struct Cyc_List_List*)
_check_null( _temp104))->hd); _LL112: _temp111= _temp107.f1; goto _LL110; _LL110:
_temp109= _temp107.f2; goto _LL108; _LL108: rgions= Cyc_List_imp_append(
_temp111, rgions); effects= Cyc_List_imp_append( _temp109, effects);} goto _LL95;
_LL101: effects=({ struct Cyc_List_List* _temp113=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp113->hd=( void*) t; _temp113->tl=
effects; _temp113;}); goto _LL95; _LL95:;} return({ struct _tuple12 _temp114;
_temp114.f1= rgions; _temp114.f2= effects; _temp114;});} static void Cyc_Dump_dumpeff(
void* t){ struct Cyc_List_List* _temp117; struct Cyc_List_List* _temp119; struct
_tuple12 _temp115= Cyc_Dump_effects_split( t); _LL120: _temp119= _temp115.f1;
goto _LL118; _LL118: _temp117= _temp115.f2; goto _LL116; _LL116: _temp119= Cyc_List_imp_rev(
_temp119); _temp117= Cyc_List_imp_rev( _temp117); for( 0; _temp117 !=  0;
_temp117=(( struct Cyc_List_List*) _check_null( _temp117))->tl){ Cyc_Dump_dumpntyp((
void*)(( struct Cyc_List_List*) _check_null( _temp117))->hd); Cyc_Dump_dump_char((
int)'+');} Cyc_Dump_dump_lb(); for( 0; _temp119 !=  0; _temp119=(( struct Cyc_List_List*)
_check_null( _temp119))->tl){ Cyc_Dump_dumprgn(( void*)(( struct Cyc_List_List*)
_check_null( _temp119))->hd); if((( struct Cyc_List_List*) _check_null( _temp119))->tl
!=  0){ Cyc_Dump_dump_char(( int)',');}} Cyc_Dump_dump_rb();} void Cyc_Dump_dumpntyp(
void* t){ void* _temp121= t; struct Cyc_Absyn_Tvar* _temp193; int _temp195;
struct Cyc_Core_Opt* _temp197; struct Cyc_Core_Opt* _temp199; int _temp201;
struct Cyc_Core_Opt* _temp203; struct Cyc_Core_Opt _temp205; void* _temp206;
struct Cyc_Core_Opt* _temp208; struct Cyc_Absyn_TunionInfo _temp210; void*
_temp212; struct Cyc_List_List* _temp214; void* _temp216; struct Cyc_Absyn_TunionFieldInfo
_temp218; struct Cyc_List_List* _temp220; void* _temp222; struct _tuple1*
_temp224; void* _temp226; void* _temp228; void* _temp230; void* _temp232; void*
_temp234; void* _temp236; void* _temp238; void* _temp240; void* _temp242; void*
_temp244; void* _temp246; void* _temp248; void* _temp251; void* _temp253; void*
_temp255; void* _temp257; void* _temp260; void* _temp262; void* _temp264; void*
_temp266; struct Cyc_List_List* _temp268; struct Cyc_List_List* _temp270; struct
_tuple1* _temp272; struct Cyc_List_List* _temp274; struct _tuple1* _temp276;
struct Cyc_List_List* _temp278; struct _tuple1* _temp280; struct Cyc_List_List*
_temp282; struct _tuple1* _temp284; struct Cyc_List_List* _temp286; struct Cyc_List_List*
_temp288; struct Cyc_Core_Opt* _temp290; struct Cyc_List_List* _temp292; struct
_tuple1* _temp294; void* _temp296; _LL123: if(( unsigned int) _temp121 >  4u?*((
int*) _temp121) ==  Cyc_Absyn_ArrayType: 0){ goto _LL124;} else{ goto _LL125;}
_LL125: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_FnType:
0){ goto _LL126;} else{ goto _LL127;} _LL127: if(( unsigned int) _temp121 >  4u?*((
int*) _temp121) ==  Cyc_Absyn_PointerType: 0){ goto _LL128;} else{ goto _LL129;}
_LL129: if( _temp121 == ( void*) Cyc_Absyn_VoidType){ goto _LL130;} else{ goto
_LL131;} _LL131: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_VarType:
0){ _LL194: _temp193=(( struct Cyc_Absyn_VarType_struct*) _temp121)->f1; goto
_LL132;} else{ goto _LL133;} _LL133: if(( unsigned int) _temp121 >  4u?*(( int*)
_temp121) ==  Cyc_Absyn_Evar: 0){ _LL200: _temp199=(( struct Cyc_Absyn_Evar_struct*)
_temp121)->f1; goto _LL198; _LL198: _temp197=(( struct Cyc_Absyn_Evar_struct*)
_temp121)->f2; if( _temp197 ==  0){ goto _LL196;} else{ goto _LL135;} _LL196:
_temp195=(( struct Cyc_Absyn_Evar_struct*) _temp121)->f3; goto _LL134;} else{
goto _LL135;} _LL135: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121) == 
Cyc_Absyn_Evar: 0){ _LL209: _temp208=(( struct Cyc_Absyn_Evar_struct*) _temp121)->f1;
goto _LL204; _LL204: _temp203=(( struct Cyc_Absyn_Evar_struct*) _temp121)->f2;
if( _temp203 ==  0){ goto _LL137;} else{ _temp205=* _temp203; _LL207: _temp206=(
void*) _temp205.v; goto _LL202;} _LL202: _temp201=(( struct Cyc_Absyn_Evar_struct*)
_temp121)->f3; goto _LL136;} else{ goto _LL137;} _LL137: if(( unsigned int)
_temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_TunionType: 0){ _LL211:
_temp210=(( struct Cyc_Absyn_TunionType_struct*) _temp121)->f1; _LL217: _temp216=(
void*) _temp210.tunion_info; goto _LL215; _LL215: _temp214= _temp210.targs; goto
_LL213; _LL213: _temp212=( void*) _temp210.rgn; goto _LL138;} else{ goto _LL139;}
_LL139: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_TunionFieldType:
0){ _LL219: _temp218=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp121)->f1;
_LL223: _temp222=( void*) _temp218.field_info; goto _LL221; _LL221: _temp220=
_temp218.targs; goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int)
_temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_EnumType: 0){ _LL225: _temp224=((
struct Cyc_Absyn_EnumType_struct*) _temp121)->f1; goto _LL142;} else{ goto
_LL143;} _LL143: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_IntType:
0){ _LL229: _temp228=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp121)->f1;
if( _temp228 == ( void*) Cyc_Absyn_Signed){ goto _LL227;} else{ goto _LL145;}
_LL227: _temp226=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp121)->f2; if(
_temp226 == ( void*) Cyc_Absyn_B4){ goto _LL144;} else{ goto _LL145;}} else{
goto _LL145;} _LL145: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121) == 
Cyc_Absyn_IntType: 0){ _LL233: _temp232=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f1; if( _temp232 == ( void*) Cyc_Absyn_Signed){ goto _LL231;} else{
goto _LL147;} _LL231: _temp230=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f2; if( _temp230 == ( void*) Cyc_Absyn_B1){ goto _LL146;} else{ goto
_LL147;}} else{ goto _LL147;} _LL147: if(( unsigned int) _temp121 >  4u?*(( int*)
_temp121) ==  Cyc_Absyn_IntType: 0){ _LL237: _temp236=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f1; if( _temp236 == ( void*) Cyc_Absyn_Unsigned){ goto _LL235;} else{
goto _LL149;} _LL235: _temp234=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f2; if( _temp234 == ( void*) Cyc_Absyn_B1){ goto _LL148;} else{ goto
_LL149;}} else{ goto _LL149;} _LL149: if(( unsigned int) _temp121 >  4u?*(( int*)
_temp121) ==  Cyc_Absyn_IntType: 0){ _LL241: _temp240=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f1; if( _temp240 == ( void*) Cyc_Absyn_Signed){ goto _LL239;} else{
goto _LL151;} _LL239: _temp238=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f2; if( _temp238 == ( void*) Cyc_Absyn_B2){ goto _LL150;} else{ goto
_LL151;}} else{ goto _LL151;} _LL151: if(( unsigned int) _temp121 >  4u?*(( int*)
_temp121) ==  Cyc_Absyn_IntType: 0){ _LL245: _temp244=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f1; if( _temp244 == ( void*) Cyc_Absyn_Unsigned){ goto _LL243;} else{
goto _LL153;} _LL243: _temp242=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f2; if( _temp242 == ( void*) Cyc_Absyn_B2){ goto _LL152;} else{ goto
_LL153;}} else{ goto _LL153;} _LL153: if(( unsigned int) _temp121 >  4u?*(( int*)
_temp121) ==  Cyc_Absyn_IntType: 0){ _LL249: _temp248=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f1; if( _temp248 == ( void*) Cyc_Absyn_Unsigned){ goto _LL247;} else{
goto _LL155;} _LL247: _temp246=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f2; if( _temp246 == ( void*) Cyc_Absyn_B4){ goto _LL154;} else{ goto
_LL155;}} else{ goto _LL155;} _LL155: if(( unsigned int) _temp121 >  4u?*(( int*)
_temp121) ==  Cyc_Absyn_IntType: 0){ _LL254: _temp253=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f1; if( _temp253 == ( void*) Cyc_Absyn_Signed){ goto _LL252;} else{
goto _LL157;} _LL252: _temp251=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f2; if( _temp251 == ( void*) Cyc_Absyn_B8){ goto _LL250;} else{ goto
_LL157;}} else{ goto _LL157;} _LL250: if( Cyc_Dump_to_VC){ goto _LL156;} else{
goto _LL157;} _LL157: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121) == 
Cyc_Absyn_IntType: 0){ _LL258: _temp257=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f1; if( _temp257 == ( void*) Cyc_Absyn_Signed){ goto _LL256;} else{
goto _LL159;} _LL256: _temp255=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f2; if( _temp255 == ( void*) Cyc_Absyn_B8){ goto _LL158;} else{ goto
_LL159;}} else{ goto _LL159;} _LL159: if(( unsigned int) _temp121 >  4u?*(( int*)
_temp121) ==  Cyc_Absyn_IntType: 0){ _LL263: _temp262=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f1; if( _temp262 == ( void*) Cyc_Absyn_Unsigned){ goto _LL261;} else{
goto _LL161;} _LL261: _temp260=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f2; if( _temp260 == ( void*) Cyc_Absyn_B8){ goto _LL259;} else{ goto
_LL161;}} else{ goto _LL161;} _LL259: if( Cyc_Dump_to_VC){ goto _LL160;} else{
goto _LL161;} _LL161: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121) == 
Cyc_Absyn_IntType: 0){ _LL267: _temp266=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f1; if( _temp266 == ( void*) Cyc_Absyn_Unsigned){ goto _LL265;} else{
goto _LL163;} _LL265: _temp264=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp121)->f2; if( _temp264 == ( void*) Cyc_Absyn_B8){ goto _LL162;} else{ goto
_LL163;}} else{ goto _LL163;} _LL163: if( _temp121 == ( void*) Cyc_Absyn_FloatType){
goto _LL164;} else{ goto _LL165;} _LL165: if( _temp121 == ( void*) Cyc_Absyn_DoubleType){
goto _LL166;} else{ goto _LL167;} _LL167: if(( unsigned int) _temp121 >  4u?*((
int*) _temp121) ==  Cyc_Absyn_TupleType: 0){ _LL269: _temp268=(( struct Cyc_Absyn_TupleType_struct*)
_temp121)->f1; goto _LL168;} else{ goto _LL169;} _LL169: if(( unsigned int)
_temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_StructType: 0){ _LL273:
_temp272=(( struct Cyc_Absyn_StructType_struct*) _temp121)->f1; if( _temp272 == 
0){ goto _LL271;} else{ goto _LL171;} _LL271: _temp270=(( struct Cyc_Absyn_StructType_struct*)
_temp121)->f2; goto _LL170;} else{ goto _LL171;} _LL171: if(( unsigned int)
_temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_StructType: 0){ _LL277:
_temp276=(( struct Cyc_Absyn_StructType_struct*) _temp121)->f1; goto _LL275;
_LL275: _temp274=(( struct Cyc_Absyn_StructType_struct*) _temp121)->f2; goto
_LL172;} else{ goto _LL173;} _LL173: if(( unsigned int) _temp121 >  4u?*(( int*)
_temp121) ==  Cyc_Absyn_UnionType: 0){ _LL281: _temp280=(( struct Cyc_Absyn_UnionType_struct*)
_temp121)->f1; if( _temp280 ==  0){ goto _LL279;} else{ goto _LL175;} _LL279:
_temp278=(( struct Cyc_Absyn_UnionType_struct*) _temp121)->f2; goto _LL174;}
else{ goto _LL175;} _LL175: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121)
==  Cyc_Absyn_UnionType: 0){ _LL285: _temp284=(( struct Cyc_Absyn_UnionType_struct*)
_temp121)->f1; goto _LL283; _LL283: _temp282=(( struct Cyc_Absyn_UnionType_struct*)
_temp121)->f2; goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int)
_temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_AnonStructType: 0){ _LL287:
_temp286=(( struct Cyc_Absyn_AnonStructType_struct*) _temp121)->f1; goto _LL178;}
else{ goto _LL179;} _LL179: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121)
==  Cyc_Absyn_AnonUnionType: 0){ _LL289: _temp288=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp121)->f1; goto _LL180;} else{ goto _LL181;} _LL181: if(( unsigned int)
_temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_TypedefType: 0){ _LL295:
_temp294=(( struct Cyc_Absyn_TypedefType_struct*) _temp121)->f1; goto _LL293;
_LL293: _temp292=(( struct Cyc_Absyn_TypedefType_struct*) _temp121)->f2; goto
_LL291; _LL291: _temp290=(( struct Cyc_Absyn_TypedefType_struct*) _temp121)->f3;
goto _LL182;} else{ goto _LL183;} _LL183: if(( unsigned int) _temp121 >  4u?*((
int*) _temp121) ==  Cyc_Absyn_RgnHandleType: 0){ _LL297: _temp296=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp121)->f1; goto _LL184;} else{ goto
_LL185;} _LL185: if( _temp121 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL186;}
else{ goto _LL187;} _LL187: if(( unsigned int) _temp121 >  4u?*(( int*) _temp121)
==  Cyc_Absyn_AccessEff: 0){ goto _LL188;} else{ goto _LL189;} _LL189: if((
unsigned int) _temp121 >  4u?*(( int*) _temp121) ==  Cyc_Absyn_RgnsEff: 0){ goto
_LL190;} else{ goto _LL191;} _LL191: if(( unsigned int) _temp121 >  4u?*(( int*)
_temp121) ==  Cyc_Absyn_JoinEff: 0){ goto _LL192;} else{ goto _LL122;} _LL124:
return; _LL126: return; _LL128: return; _LL130: Cyc_Dump_dump_alpha( _tag_arr("void",
sizeof( unsigned char), 5u)); return; _LL132: Cyc_Dump_dump_alphastr( _temp193->name);
return; _LL134: Cyc_Dump_dump_char(( int)'%'); if( _temp199 ==  0){ Cyc_Dump_dump_char((
int)'?');} else{ Cyc_Dump_dumpkind(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp199))->v);} Cyc_Dump_dump_symbol(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp299; _temp299.tag= Cyc_Std_Int_pa; _temp299.f1=( int)(( unsigned int)
_temp195);{ void* _temp298[ 1u]={& _temp299}; Cyc_Std_aprintf( _tag_arr("(%d)",
sizeof( unsigned char), 5u), _tag_arr( _temp298, sizeof( void*), 1u));}}));
return; _LL136: Cyc_Dump_dumpntyp( _temp206); return; _LL138:{ void* _temp300=
_temp216; struct Cyc_Absyn_UnknownTunionInfo _temp306; int _temp308; struct
_tuple1* _temp310; struct Cyc_Absyn_Tuniondecl* _temp312; struct Cyc_Absyn_Tuniondecl
_temp314; int _temp315; struct _tuple1* _temp317; _LL302: if(*(( int*) _temp300)
==  Cyc_Absyn_UnknownTunion){ _LL307: _temp306=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp300)->f1; _LL311: _temp310= _temp306.name; goto _LL309; _LL309: _temp308=
_temp306.is_xtunion; goto _LL303;} else{ goto _LL304;} _LL304: if(*(( int*)
_temp300) ==  Cyc_Absyn_KnownTunion){ _LL313: _temp312=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp300)->f1; _temp314=* _temp312; _LL318: _temp317= _temp314.name; goto _LL316;
_LL316: _temp315= _temp314.is_xtunion; goto _LL305;} else{ goto _LL301;} _LL303:
_temp317= _temp310; _temp315= _temp308; goto _LL305; _LL305: if( _temp315){ Cyc_Dump_dump_alpha(
_tag_arr("xtunion", sizeof( unsigned char), 8u));} else{ Cyc_Dump_dump_alpha(
_tag_arr("tunion", sizeof( unsigned char), 7u));}{ void* _temp319= Cyc_Tcutil_compress(
_temp212); _LL321: if( _temp319 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL322;}
else{ goto _LL323;} _LL323: goto _LL324; _LL322: goto _LL320; _LL324: Cyc_Dump_dumptyp(
_temp212); goto _LL320; _LL320:;} Cyc_Dump_dumpqvar( _temp317); Cyc_Dump_dumptps(
_temp214); goto _LL301; _LL301:;} goto _LL122; _LL140:{ void* _temp325= _temp222;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp331; int _temp333; struct _tuple1*
_temp335; struct _tuple1* _temp337; struct Cyc_Absyn_Tunionfield* _temp339;
struct Cyc_Absyn_Tunionfield _temp341; struct _tuple1* _temp342; struct Cyc_Absyn_Tuniondecl*
_temp344; struct Cyc_Absyn_Tuniondecl _temp346; int _temp347; struct _tuple1*
_temp349; _LL327: if(*(( int*) _temp325) ==  Cyc_Absyn_UnknownTunionfield){
_LL332: _temp331=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp325)->f1;
_LL338: _temp337= _temp331.tunion_name; goto _LL336; _LL336: _temp335= _temp331.field_name;
goto _LL334; _LL334: _temp333= _temp331.is_xtunion; goto _LL328;} else{ goto
_LL329;} _LL329: if(*(( int*) _temp325) ==  Cyc_Absyn_KnownTunionfield){ _LL345:
_temp344=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp325)->f1; _temp346=*
_temp344; _LL350: _temp349= _temp346.name; goto _LL348; _LL348: _temp347=
_temp346.is_xtunion; goto _LL340; _LL340: _temp339=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp325)->f2; _temp341=* _temp339; _LL343: _temp342= _temp341.name; goto _LL330;}
else{ goto _LL326;} _LL328: _temp349= _temp337; _temp347= _temp333; _temp342=
_temp335; goto _LL330; _LL330: if( _temp347){ Cyc_Dump_dump_alpha( _tag_arr("xtunion",
sizeof( unsigned char), 8u));} else{ Cyc_Dump_dump_alpha( _tag_arr("tunion",
sizeof( unsigned char), 7u));} Cyc_Dump_dumpqvar( _temp349); Cyc_Dump_dump_char((
int)'.'); Cyc_Dump_dumpqvar( _temp342); Cyc_Dump_dumptps( _temp220); goto _LL326;
_LL326:;} goto _LL122; _LL142: Cyc_Dump_dump_alpha( _tag_arr("enum", sizeof(
unsigned char), 5u)); Cyc_Dump_dumpqvar( _temp224); return; _LL144: Cyc_Dump_dump_alpha(
_tag_arr("int", sizeof( unsigned char), 4u)); return; _LL146: Cyc_Dump_dump_alpha(
_tag_arr("signed char", sizeof( unsigned char), 12u)); return; _LL148: Cyc_Dump_dump_alpha(
_tag_arr("unsigned char", sizeof( unsigned char), 14u)); return; _LL150: Cyc_Dump_dump_alpha(
_tag_arr("short", sizeof( unsigned char), 6u)); return; _LL152: Cyc_Dump_dump_alpha(
_tag_arr("unsigned short", sizeof( unsigned char), 15u)); return; _LL154: Cyc_Dump_dump_alpha(
_tag_arr("unsigned int", sizeof( unsigned char), 13u)); return; _LL156: Cyc_Dump_dump_alpha(
_tag_arr("__int64", sizeof( unsigned char), 8u)); return; _LL158: Cyc_Dump_dump_alpha(
_tag_arr("long long", sizeof( unsigned char), 10u)); return; _LL160: Cyc_Dump_dump_alpha(
_tag_arr("unsigned __int64", sizeof( unsigned char), 17u)); return; _LL162: Cyc_Dump_dump_alpha(
_tag_arr("unsigned long long", sizeof( unsigned char), 19u)); return; _LL164:
Cyc_Dump_dump_alpha( _tag_arr("float", sizeof( unsigned char), 6u)); return;
_LL166: Cyc_Dump_dump_alpha( _tag_arr("double", sizeof( unsigned char), 7u));
return; _LL168: Cyc_Dump_dump_symbol( _tag_arr("\\$", sizeof( unsigned char), 3u));
Cyc_Dump_dumpargs( _temp268); return; _LL170: Cyc_Dump_dump_alpha( _tag_arr("struct",
sizeof( unsigned char), 7u)); Cyc_Dump_dumptps( _temp270); return; _LL172: Cyc_Dump_dump_alpha(
_tag_arr("struct", sizeof( unsigned char), 7u)); Cyc_Dump_dumpqvar(( struct
_tuple1*) _check_null( _temp276)); Cyc_Dump_dumptps( _temp274); return; _LL174:
Cyc_Dump_dump_alpha( _tag_arr("union", sizeof( unsigned char), 6u)); Cyc_Dump_dumptps(
_temp278); return; _LL176: Cyc_Dump_dump_alpha( _tag_arr("union", sizeof(
unsigned char), 6u)); Cyc_Dump_dumpqvar(( struct _tuple1*) _check_null( _temp284));
Cyc_Dump_dumptps( _temp282); return; _LL178: Cyc_Dump_dump_alpha( _tag_arr("struct",
sizeof( unsigned char), 7u)); Cyc_Dump_dump_lb(); Cyc_Dump_dumpstructfields(
_temp286); Cyc_Dump_dump_rb(); return; _LL180: Cyc_Dump_dump_alpha( _tag_arr("union",
sizeof( unsigned char), 6u)); Cyc_Dump_dump_lb(); Cyc_Dump_dumpstructfields(
_temp288); Cyc_Dump_dump_rb(); return; _LL182:( Cyc_Dump_dumpqvar( _temp294),
Cyc_Dump_dumptps( _temp292)); return; _LL184: Cyc_Dump_dumprgn( _temp296);
return; _LL186: return; _LL188: return; _LL190: return; _LL192: return; _LL122:;}
void Cyc_Dump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo !=  0){ Cyc_Dump_dump_alphastr((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v);}} void Cyc_Dump_dumpfunarg(
struct _tuple2* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct
Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Dump_dumptqtd)((* t).f2,(* t).f3, Cyc_Dump_dumpvaropt,(*
t).f1);} void Cyc_Dump_dump_rgncmp( struct _tuple0* cmp){ struct _tuple0
_temp353; void* _temp354; void* _temp356; struct _tuple0* _temp351= cmp;
_temp353=* _temp351; _LL357: _temp356= _temp353.f1; goto _LL355; _LL355:
_temp354= _temp353.f2; goto _LL352; _LL352: Cyc_Dump_dumptyp( _temp356); Cyc_Dump_dump_char((
int)'<'); Cyc_Dump_dumptyp( _temp354);} void Cyc_Dump_dump_rgnpo( struct Cyc_List_List*
rgn_po){(( void(*)( void(* f)( struct _tuple0*), struct Cyc_List_List* l, struct
_tagged_arr sep)) Cyc_Dump_dump_sep)( Cyc_Dump_dump_rgncmp, rgn_po, _tag_arr(",",
sizeof( unsigned char), 2u));} void Cyc_Dump_dumpfunargs( struct Cyc_List_List*
args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_Core_Opt*
effopt, struct Cyc_List_List* rgn_po){ Cyc_Dump_dump_char(( int)'('); for( 0;
args !=  0; args=(( struct Cyc_List_List*) _check_null( args))->tl){ Cyc_Dump_dumpfunarg((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( args))->hd); if((((
struct Cyc_List_List*) _check_null( args))->tl !=  0? 1: c_varargs)? 1:
cyc_varargs !=  0){ Cyc_Dump_dump_symbol( _tag_arr(", ", sizeof( unsigned char),
3u));}} if( c_varargs){ Cyc_Dump_dump_symbol( _tag_arr("...", sizeof(
unsigned char), 4u));} else{ if( cyc_varargs !=  0){ struct _tuple2* _temp358=({
struct _tuple2* _temp359=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp359->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp359->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp359->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp359;}); Cyc_Dump_dump_symbol( _tag_arr("...", sizeof( unsigned char), 4u));
if((( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->inject){ Cyc_Dump_dump_alpha(
_tag_arr(" inject", sizeof( unsigned char), 8u));} Cyc_Dump_dumpfunarg( _temp358);}}
if( effopt !=  0){ Cyc_Dump_dump_semi(); Cyc_Dump_dumpeff(( void*)(( struct Cyc_Core_Opt*)
_check_null( effopt))->v);} if( rgn_po !=  0){ Cyc_Dump_dump_char(( int)':');
Cyc_Dump_dump_rgnpo( rgn_po);} Cyc_Dump_dump_char(( int)')');} void Cyc_Dump_dumptyp(
void* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Dump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp360; _temp360.q_const= 0; _temp360.q_volatile= 0;
_temp360.q_restrict= 0; _temp360;}), t,( void(*)( int x)) Cyc_Dump_ignore, 0);}
void Cyc_Dump_dumpdesignator( void* d){ void* _temp361= d; struct Cyc_Absyn_Exp*
_temp367; struct _tagged_arr* _temp369; _LL363: if(*(( int*) _temp361) ==  Cyc_Absyn_ArrayElement){
_LL368: _temp367=(( struct Cyc_Absyn_ArrayElement_struct*) _temp361)->f1; goto
_LL364;} else{ goto _LL365;} _LL365: if(*(( int*) _temp361) ==  Cyc_Absyn_FieldName){
_LL370: _temp369=(( struct Cyc_Absyn_FieldName_struct*) _temp361)->f1; goto
_LL366;} else{ goto _LL362;} _LL364: Cyc_Dump_dump_symbol( _tag_arr(".[",
sizeof( unsigned char), 3u)); Cyc_Dump_dumpexp( _temp367); Cyc_Dump_dump_char((
int)']'); goto _LL362; _LL366: Cyc_Dump_dump_char(( int)'.'); Cyc_Dump_dump_alpha(*
_temp369); goto _LL362; _LL362:;} struct _tuple13{ struct Cyc_List_List* f1;
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
inprec >=  myprec){ Cyc_Dump_dump_char(( int)'(');}{ void* _temp371=( void*) e->r;
void* _temp467; unsigned char _temp469; void* _temp471; void* _temp473; short
_temp475; void* _temp477; void* _temp479; int _temp481; void* _temp483; void*
_temp485; int _temp487; void* _temp489; void* _temp491; long long _temp493; void*
_temp495; void* _temp497; struct _tagged_arr _temp499; void* _temp501; void*
_temp503; struct _tagged_arr _temp505; struct _tuple1* _temp507; struct _tuple1*
_temp509; struct Cyc_List_List* _temp511; void* _temp513; struct Cyc_Absyn_Exp*
_temp515; struct Cyc_Core_Opt* _temp517; struct Cyc_Absyn_Exp* _temp519; void*
_temp521; struct Cyc_Absyn_Exp* _temp523; void* _temp525; struct Cyc_Absyn_Exp*
_temp527; void* _temp529; struct Cyc_Absyn_Exp* _temp531; void* _temp533; struct
Cyc_Absyn_Exp* _temp535; struct Cyc_Absyn_Exp* _temp537; struct Cyc_Absyn_Exp*
_temp539; struct Cyc_Absyn_Exp* _temp541; struct Cyc_Absyn_Exp* _temp543; struct
Cyc_Absyn_Exp* _temp545; struct Cyc_List_List* _temp547; struct Cyc_Absyn_Exp*
_temp549; struct Cyc_List_List* _temp551; struct Cyc_Absyn_Exp* _temp553; struct
Cyc_Absyn_Exp* _temp555; struct Cyc_Absyn_Exp* _temp557; struct Cyc_Absyn_Exp*
_temp559; struct Cyc_Absyn_Exp* _temp561; void* _temp563; struct Cyc_Absyn_Exp*
_temp565; struct Cyc_Absyn_Exp* _temp567; struct Cyc_Absyn_Exp* _temp569; void*
_temp571; struct Cyc_Absyn_Exp* _temp573; struct _tagged_arr* _temp575; void*
_temp577; void* _temp579; struct Cyc_Absyn_Exp* _temp581; struct _tagged_arr*
_temp583; struct Cyc_Absyn_Exp* _temp585; struct _tagged_arr* _temp587; struct
Cyc_Absyn_Exp* _temp589; struct Cyc_Absyn_Exp* _temp591; struct Cyc_Absyn_Exp*
_temp593; struct Cyc_List_List* _temp595; struct Cyc_List_List* _temp597; struct
_tuple2* _temp599; struct Cyc_List_List* _temp601; struct Cyc_Absyn_Exp*
_temp603; struct Cyc_Absyn_Exp* _temp605; struct Cyc_Absyn_Vardecl* _temp607;
struct Cyc_List_List* _temp609; struct _tuple1* _temp611; struct Cyc_List_List*
_temp613; struct Cyc_Absyn_Tunionfield* _temp615; struct Cyc_List_List* _temp617;
struct _tuple1* _temp619; void* _temp621; struct Cyc_Absyn_Exp* _temp623; struct
Cyc_List_List* _temp625; struct Cyc_Core_Opt* _temp627; struct Cyc_Absyn_Stmt*
_temp629; struct Cyc_Absyn_Fndecl* _temp631; struct Cyc_Absyn_Exp* _temp633;
_LL373: if(*(( int*) _temp371) ==  Cyc_Absyn_Const_e){ _LL468: _temp467=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp371)->f1; if(( unsigned int) _temp467 > 
1u?*(( int*) _temp467) ==  Cyc_Absyn_Char_c: 0){ _LL472: _temp471=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp467)->f1; goto _LL470; _LL470: _temp469=((
struct Cyc_Absyn_Char_c_struct*) _temp467)->f2; goto _LL374;} else{ goto _LL375;}}
else{ goto _LL375;} _LL375: if(*(( int*) _temp371) ==  Cyc_Absyn_Const_e){
_LL474: _temp473=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp371)->f1; if((
unsigned int) _temp473 >  1u?*(( int*) _temp473) ==  Cyc_Absyn_Short_c: 0){
_LL478: _temp477=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp473)->f1;
goto _LL476; _LL476: _temp475=(( struct Cyc_Absyn_Short_c_struct*) _temp473)->f2;
goto _LL376;} else{ goto _LL377;}} else{ goto _LL377;} _LL377: if(*(( int*)
_temp371) ==  Cyc_Absyn_Const_e){ _LL480: _temp479=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp371)->f1; if(( unsigned int) _temp479 >  1u?*(( int*) _temp479) ==  Cyc_Absyn_Int_c:
0){ _LL484: _temp483=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp479)->f1;
if( _temp483 == ( void*) Cyc_Absyn_Signed){ goto _LL482;} else{ goto _LL379;}
_LL482: _temp481=(( struct Cyc_Absyn_Int_c_struct*) _temp479)->f2; goto _LL378;}
else{ goto _LL379;}} else{ goto _LL379;} _LL379: if(*(( int*) _temp371) ==  Cyc_Absyn_Const_e){
_LL486: _temp485=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp371)->f1; if((
unsigned int) _temp485 >  1u?*(( int*) _temp485) ==  Cyc_Absyn_Int_c: 0){ _LL490:
_temp489=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp485)->f1; if( _temp489
== ( void*) Cyc_Absyn_Unsigned){ goto _LL488;} else{ goto _LL381;} _LL488:
_temp487=(( struct Cyc_Absyn_Int_c_struct*) _temp485)->f2; goto _LL380;} else{
goto _LL381;}} else{ goto _LL381;} _LL381: if(*(( int*) _temp371) ==  Cyc_Absyn_Const_e){
_LL492: _temp491=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp371)->f1; if((
unsigned int) _temp491 >  1u?*(( int*) _temp491) ==  Cyc_Absyn_LongLong_c: 0){
_LL496: _temp495=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp491)->f1;
goto _LL494; _LL494: _temp493=(( struct Cyc_Absyn_LongLong_c_struct*) _temp491)->f2;
goto _LL382;} else{ goto _LL383;}} else{ goto _LL383;} _LL383: if(*(( int*)
_temp371) ==  Cyc_Absyn_Const_e){ _LL498: _temp497=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp371)->f1; if(( unsigned int) _temp497 >  1u?*(( int*) _temp497) ==  Cyc_Absyn_Float_c:
0){ _LL500: _temp499=(( struct Cyc_Absyn_Float_c_struct*) _temp497)->f1; goto
_LL384;} else{ goto _LL385;}} else{ goto _LL385;} _LL385: if(*(( int*) _temp371)
==  Cyc_Absyn_Const_e){ _LL502: _temp501=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp371)->f1; if( _temp501 == ( void*) Cyc_Absyn_Null_c){ goto _LL386;} else{
goto _LL387;}} else{ goto _LL387;} _LL387: if(*(( int*) _temp371) ==  Cyc_Absyn_Const_e){
_LL504: _temp503=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp371)->f1; if((
unsigned int) _temp503 >  1u?*(( int*) _temp503) ==  Cyc_Absyn_String_c: 0){
_LL506: _temp505=(( struct Cyc_Absyn_String_c_struct*) _temp503)->f1; goto
_LL388;} else{ goto _LL389;}} else{ goto _LL389;} _LL389: if(*(( int*) _temp371)
==  Cyc_Absyn_UnknownId_e){ _LL508: _temp507=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp371)->f1; goto _LL390;} else{ goto _LL391;} _LL391: if(*(( int*) _temp371)
==  Cyc_Absyn_Var_e){ _LL510: _temp509=(( struct Cyc_Absyn_Var_e_struct*)
_temp371)->f1; goto _LL392;} else{ goto _LL393;} _LL393: if(*(( int*) _temp371)
==  Cyc_Absyn_Primop_e){ _LL514: _temp513=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp371)->f1; goto _LL512; _LL512: _temp511=(( struct Cyc_Absyn_Primop_e_struct*)
_temp371)->f2; goto _LL394;} else{ goto _LL395;} _LL395: if(*(( int*) _temp371)
==  Cyc_Absyn_AssignOp_e){ _LL520: _temp519=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp371)->f1; goto _LL518; _LL518: _temp517=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp371)->f2; goto _LL516; _LL516: _temp515=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp371)->f3; goto _LL396;} else{ goto _LL397;} _LL397: if(*(( int*) _temp371)
==  Cyc_Absyn_Increment_e){ _LL524: _temp523=(( struct Cyc_Absyn_Increment_e_struct*)
_temp371)->f1; goto _LL522; _LL522: _temp521=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp371)->f2; if( _temp521 == ( void*) Cyc_Absyn_PreInc){ goto _LL398;} else{
goto _LL399;}} else{ goto _LL399;} _LL399: if(*(( int*) _temp371) ==  Cyc_Absyn_Increment_e){
_LL528: _temp527=(( struct Cyc_Absyn_Increment_e_struct*) _temp371)->f1; goto
_LL526; _LL526: _temp525=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp371)->f2; if( _temp525 == ( void*) Cyc_Absyn_PreDec){ goto _LL400;} else{
goto _LL401;}} else{ goto _LL401;} _LL401: if(*(( int*) _temp371) ==  Cyc_Absyn_Increment_e){
_LL532: _temp531=(( struct Cyc_Absyn_Increment_e_struct*) _temp371)->f1; goto
_LL530; _LL530: _temp529=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp371)->f2; if( _temp529 == ( void*) Cyc_Absyn_PostInc){ goto _LL402;} else{
goto _LL403;}} else{ goto _LL403;} _LL403: if(*(( int*) _temp371) ==  Cyc_Absyn_Increment_e){
_LL536: _temp535=(( struct Cyc_Absyn_Increment_e_struct*) _temp371)->f1; goto
_LL534; _LL534: _temp533=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp371)->f2; if( _temp533 == ( void*) Cyc_Absyn_PostDec){ goto _LL404;} else{
goto _LL405;}} else{ goto _LL405;} _LL405: if(*(( int*) _temp371) ==  Cyc_Absyn_Conditional_e){
_LL542: _temp541=(( struct Cyc_Absyn_Conditional_e_struct*) _temp371)->f1; goto
_LL540; _LL540: _temp539=(( struct Cyc_Absyn_Conditional_e_struct*) _temp371)->f2;
goto _LL538; _LL538: _temp537=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp371)->f3; goto _LL406;} else{ goto _LL407;} _LL407: if(*(( int*) _temp371)
==  Cyc_Absyn_SeqExp_e){ _LL546: _temp545=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp371)->f1; goto _LL544; _LL544: _temp543=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp371)->f2; goto _LL408;} else{ goto _LL409;} _LL409: if(*(( int*) _temp371)
==  Cyc_Absyn_UnknownCall_e){ _LL550: _temp549=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp371)->f1; goto _LL548; _LL548: _temp547=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp371)->f2; goto _LL410;} else{ goto _LL411;} _LL411: if(*(( int*) _temp371)
==  Cyc_Absyn_FnCall_e){ _LL554: _temp553=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp371)->f1; goto _LL552; _LL552: _temp551=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp371)->f2; goto _LL412;} else{ goto _LL413;} _LL413: if(*(( int*) _temp371)
==  Cyc_Absyn_Throw_e){ _LL556: _temp555=(( struct Cyc_Absyn_Throw_e_struct*)
_temp371)->f1; goto _LL414;} else{ goto _LL415;} _LL415: if(*(( int*) _temp371)
==  Cyc_Absyn_NoInstantiate_e){ _LL558: _temp557=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp371)->f1; goto _LL416;} else{ goto _LL417;} _LL417: if(*(( int*) _temp371)
==  Cyc_Absyn_Instantiate_e){ _LL560: _temp559=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp371)->f1; goto _LL418;} else{ goto _LL419;} _LL419: if(*(( int*) _temp371)
==  Cyc_Absyn_Cast_e){ _LL564: _temp563=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp371)->f1; goto _LL562; _LL562: _temp561=(( struct Cyc_Absyn_Cast_e_struct*)
_temp371)->f2; goto _LL420;} else{ goto _LL421;} _LL421: if(*(( int*) _temp371)
==  Cyc_Absyn_Address_e){ _LL566: _temp565=(( struct Cyc_Absyn_Address_e_struct*)
_temp371)->f1; goto _LL422;} else{ goto _LL423;} _LL423: if(*(( int*) _temp371)
==  Cyc_Absyn_New_e){ _LL570: _temp569=(( struct Cyc_Absyn_New_e_struct*)
_temp371)->f1; goto _LL568; _LL568: _temp567=(( struct Cyc_Absyn_New_e_struct*)
_temp371)->f2; goto _LL424;} else{ goto _LL425;} _LL425: if(*(( int*) _temp371)
==  Cyc_Absyn_Sizeoftyp_e){ _LL572: _temp571=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp371)->f1; goto _LL426;} else{ goto _LL427;} _LL427: if(*(( int*) _temp371)
==  Cyc_Absyn_Sizeofexp_e){ _LL574: _temp573=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp371)->f1; goto _LL428;} else{ goto _LL429;} _LL429: if(*(( int*) _temp371)
==  Cyc_Absyn_Offsetof_e){ _LL578: _temp577=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp371)->f1; goto _LL576; _LL576: _temp575=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp371)->f2; goto _LL430;} else{ goto _LL431;} _LL431: if(*(( int*) _temp371)
==  Cyc_Absyn_Gentyp_e){ _LL580: _temp579=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*)
_temp371)->f1; goto _LL432;} else{ goto _LL433;} _LL433: if(*(( int*) _temp371)
==  Cyc_Absyn_Deref_e){ _LL582: _temp581=(( struct Cyc_Absyn_Deref_e_struct*)
_temp371)->f1; goto _LL434;} else{ goto _LL435;} _LL435: if(*(( int*) _temp371)
==  Cyc_Absyn_StructMember_e){ _LL586: _temp585=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp371)->f1; goto _LL584; _LL584: _temp583=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp371)->f2; goto _LL436;} else{ goto _LL437;} _LL437: if(*(( int*) _temp371)
==  Cyc_Absyn_StructArrow_e){ _LL590: _temp589=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp371)->f1; goto _LL588; _LL588: _temp587=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp371)->f2; goto _LL438;} else{ goto _LL439;} _LL439: if(*(( int*) _temp371)
==  Cyc_Absyn_Subscript_e){ _LL594: _temp593=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp371)->f1; goto _LL592; _LL592: _temp591=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp371)->f2; goto _LL440;} else{ goto _LL441;} _LL441: if(*(( int*) _temp371)
==  Cyc_Absyn_Tuple_e){ _LL596: _temp595=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp371)->f1; goto _LL442;} else{ goto _LL443;} _LL443: if(*(( int*) _temp371)
==  Cyc_Absyn_CompoundLit_e){ _LL600: _temp599=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp371)->f1; goto _LL598; _LL598: _temp597=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp371)->f2; goto _LL444;} else{ goto _LL445;} _LL445: if(*(( int*) _temp371)
==  Cyc_Absyn_Array_e){ _LL602: _temp601=(( struct Cyc_Absyn_Array_e_struct*)
_temp371)->f1; goto _LL446;} else{ goto _LL447;} _LL447: if(*(( int*) _temp371)
==  Cyc_Absyn_Comprehension_e){ _LL608: _temp607=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp371)->f1; goto _LL606; _LL606: _temp605=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp371)->f2; goto _LL604; _LL604: _temp603=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp371)->f3; goto _LL448;} else{ goto _LL449;} _LL449: if(*(( int*) _temp371)
==  Cyc_Absyn_Struct_e){ _LL612: _temp611=(( struct Cyc_Absyn_Struct_e_struct*)
_temp371)->f1; goto _LL610; _LL610: _temp609=(( struct Cyc_Absyn_Struct_e_struct*)
_temp371)->f3; goto _LL450;} else{ goto _LL451;} _LL451: if(*(( int*) _temp371)
==  Cyc_Absyn_AnonStruct_e){ _LL614: _temp613=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp371)->f2; goto _LL452;} else{ goto _LL453;} _LL453: if(*(( int*) _temp371)
==  Cyc_Absyn_Tunion_e){ _LL618: _temp617=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp371)->f3; goto _LL616; _LL616: _temp615=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp371)->f5; goto _LL454;} else{ goto _LL455;} _LL455: if(*(( int*) _temp371)
==  Cyc_Absyn_Enum_e){ _LL620: _temp619=(( struct Cyc_Absyn_Enum_e_struct*)
_temp371)->f1; goto _LL456;} else{ goto _LL457;} _LL457: if(*(( int*) _temp371)
==  Cyc_Absyn_Malloc_e){ _LL624: _temp623=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp371)->f1; goto _LL622; _LL622: _temp621=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp371)->f2; goto _LL458;} else{ goto _LL459;} _LL459: if(*(( int*) _temp371)
==  Cyc_Absyn_UnresolvedMem_e){ _LL628: _temp627=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp371)->f1; goto _LL626; _LL626: _temp625=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp371)->f2; goto _LL460;} else{ goto _LL461;} _LL461: if(*(( int*) _temp371)
==  Cyc_Absyn_StmtExp_e){ _LL630: _temp629=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp371)->f1; goto _LL462;} else{ goto _LL463;} _LL463: if(*(( int*) _temp371)
==  Cyc_Absyn_Codegen_e){ _LL632: _temp631=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp371)->f1; goto _LL464;} else{ goto _LL465;} _LL465: if(*(( int*) _temp371)
==  Cyc_Absyn_Fill_e){ _LL634: _temp633=(( struct Cyc_Absyn_Fill_e_struct*)
_temp371)->f1; goto _LL466;} else{ goto _LL372;} _LL374: Cyc_Dump_dump_char((
int)'\''); Cyc_Dump_dump_alpha( Cyc_Absynpp_char_escape( _temp469)); Cyc_Dump_dump_char((
int)'\''); goto _LL372; _LL376: Cyc_Dump_dump_alpha(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp636; _temp636.tag= Cyc_Std_Int_pa; _temp636.f1=(
int)(( unsigned int)(( int) _temp475));{ void* _temp635[ 1u]={& _temp636}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp635, sizeof( void*),
1u));}})); goto _LL372; _LL378: Cyc_Dump_dump_alpha(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp638; _temp638.tag= Cyc_Std_Int_pa; _temp638.f1=(
int)(( unsigned int) _temp481);{ void* _temp637[ 1u]={& _temp638}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp637, sizeof( void*),
1u));}})); goto _LL372; _LL380: Cyc_Dump_dump_alpha(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp640; _temp640.tag= Cyc_Std_Int_pa; _temp640.f1=(
int)(( unsigned int) _temp487);{ void* _temp639[ 1u]={& _temp640}; Cyc_Std_aprintf(
_tag_arr("%du", sizeof( unsigned char), 4u), _tag_arr( _temp639, sizeof( void*),
1u));}})); goto _LL372; _LL382: Cyc_Dump_dump_alpha( _tag_arr("<<FIX LONG LONG CONSTANT>>",
sizeof( unsigned char), 27u)); goto _LL372; _LL384: Cyc_Dump_dump_alpha(
_temp499); goto _LL372; _LL386: Cyc_Dump_dump_alpha( _tag_arr("NULL", sizeof(
unsigned char), 5u)); goto _LL372; _LL388: Cyc_Dump_dump_char(( int)'"'); Cyc_Dump_dump_alpha(
Cyc_Absynpp_string_escape( _temp505)); Cyc_Dump_dump_char(( int)'"'); goto
_LL372; _LL390: _temp509= _temp507; goto _LL392; _LL392: Cyc_Dump_dumpqvar(
_temp509); goto _LL372; _LL394: { struct _tagged_arr _temp641= Cyc_Absynpp_prim2str(
_temp513); switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp511)){ case 1: _LL642: if( _temp513 == ( void*) Cyc_Absyn_Size){ Cyc_Dump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp511))->hd);
Cyc_Dump_dump_char(( int)'.'); Cyc_Dump_dump_alpha( _tag_arr("size", sizeof(
unsigned char), 5u));} else{ Cyc_Dump_dump_symbol( _temp641); Cyc_Dump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp511))->hd);}
break; case 2: _LL643: Cyc_Dump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp511))->hd); Cyc_Dump_dump_symbol(
_temp641); Cyc_Dump_dump_char(( int)' '); Cyc_Dump_dumpexp_prec( myprec,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp511))->tl))->hd); break; default: _LL644:( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp646=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp646[ 0]=({ struct Cyc_Core_Failure_struct
_temp647; _temp647.tag= Cyc_Core_Failure; _temp647.f1= _tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof( unsigned char), 47u); _temp647;}); _temp646;}));} goto _LL372;} _LL396:
Cyc_Dump_dumpexp_prec( myprec, _temp519); if( _temp517 !=  0){ Cyc_Dump_dump_symbol(
Cyc_Absynpp_prim2str(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp517))->v));}
Cyc_Dump_dump_symbol( _tag_arr("=", sizeof( unsigned char), 2u)); Cyc_Dump_dumpexp_prec(
myprec, _temp515); goto _LL372; _LL398: Cyc_Dump_dump_symbol( _tag_arr("++",
sizeof( unsigned char), 3u)); Cyc_Dump_dumpexp_prec( myprec, _temp523); goto
_LL372; _LL400: Cyc_Dump_dump_symbol( _tag_arr("--", sizeof( unsigned char), 3u));
Cyc_Dump_dumpexp_prec( myprec, _temp527); goto _LL372; _LL402: Cyc_Dump_dumpexp_prec(
myprec, _temp531); Cyc_Dump_dump_symbol( _tag_arr("++", sizeof( unsigned char),
3u)); goto _LL372; _LL404: Cyc_Dump_dumpexp_prec( myprec, _temp535); Cyc_Dump_dump_symbol(
_tag_arr("--", sizeof( unsigned char), 3u)); goto _LL372; _LL406: Cyc_Dump_dumpexp_prec(
myprec, _temp541); Cyc_Dump_dump_char(( int)'?'); Cyc_Dump_dumpexp_prec( 0,
_temp539); Cyc_Dump_dump_char(( int)':'); Cyc_Dump_dumpexp_prec( myprec,
_temp537); goto _LL372; _LL408: Cyc_Dump_dump_char(( int)'('); Cyc_Dump_dumpexp_prec(
myprec, _temp545); Cyc_Dump_dump_char(( int)','); Cyc_Dump_dumpexp_prec( myprec,
_temp543); Cyc_Dump_dump_char(( int)')'); goto _LL372; _LL410: _temp553=
_temp549; _temp551= _temp547; goto _LL412; _LL412: Cyc_Dump_dumpexp_prec( myprec,
_temp553); Cyc_Dump_dump_char(( int)'('); Cyc_Dump_dumpexps_prec( 20, _temp551);
Cyc_Dump_dump_char(( int)')'); goto _LL372; _LL414: Cyc_Dump_dump_alpha(
_tag_arr("throw", sizeof( unsigned char), 6u)); Cyc_Dump_dumpexp_prec( myprec,
_temp555); goto _LL372; _LL416: _temp559= _temp557; goto _LL418; _LL418: Cyc_Dump_dumpexp_prec(
inprec, _temp559); goto _LL372; _LL420: Cyc_Dump_dump_char(( int)'('); Cyc_Dump_dumptyp(
_temp563); Cyc_Dump_dump_char(( int)')'); Cyc_Dump_dumpexp_prec( myprec,
_temp561); goto _LL372; _LL422: Cyc_Dump_dump_char(( int)'&'); Cyc_Dump_dumpexp_prec(
myprec, _temp565); goto _LL372; _LL424: Cyc_Dump_dump_alpha( _tag_arr("new",
sizeof( unsigned char), 4u)); Cyc_Dump_dumpexp_prec( myprec, _temp567); goto
_LL372; _LL426: Cyc_Dump_dump_alpha( _tag_arr("sizeof", sizeof( unsigned char),
7u)); Cyc_Dump_dump_char(( int)'('); Cyc_Dump_dumptyp( _temp571); Cyc_Dump_dump_char((
int)')'); goto _LL372; _LL428: Cyc_Dump_dump_alpha( _tag_arr("sizeof", sizeof(
unsigned char), 7u)); Cyc_Dump_dump_char(( int)'('); Cyc_Dump_dumpexp_prec(
myprec, _temp573); Cyc_Dump_dump_char(( int)')'); goto _LL372; _LL430: Cyc_Dump_dump_alpha(
_tag_arr("offsetof", sizeof( unsigned char), 9u)); Cyc_Dump_dump_char(( int)'(');
Cyc_Dump_dumptyp( _temp577); Cyc_Dump_dump_char(( int)','); Cyc_Dump_dump_alpha(*
_temp575); Cyc_Dump_dump_char(( int)')'); goto _LL372; _LL432: Cyc_Dump_dump_alpha(
_tag_arr("__gen", sizeof( unsigned char), 6u)); Cyc_Dump_dump_char(( int)'(');
Cyc_Dump_dumptyp( _temp579); Cyc_Dump_dump_char(( int)')'); goto _LL372; _LL434:
Cyc_Dump_dump_char(( int)'*'); Cyc_Dump_dumpexp_prec( myprec, _temp581); goto
_LL372; _LL436: Cyc_Dump_dumpexp_prec( myprec, _temp585); Cyc_Dump_dump_char((
int)'.'); Cyc_Dump_dump_alpha(* _temp583); goto _LL372; _LL438: Cyc_Dump_dumpexp_prec(
myprec, _temp589); Cyc_Dump_dump_symbol( _tag_arr("->", sizeof( unsigned char),
3u)); Cyc_Dump_dump_alpha(* _temp587); goto _LL372; _LL440: Cyc_Dump_dumpexp_prec(
myprec, _temp593); Cyc_Dump_dump_char(( int)'['); Cyc_Dump_dumpexp( _temp591);
Cyc_Dump_dump_char(( int)']'); goto _LL372; _LL442: Cyc_Dump_dump_symbol(
_tag_arr("\\$(", sizeof( unsigned char), 4u)); Cyc_Dump_dumpexps_prec( 20,
_temp595); Cyc_Dump_dump_char(( int)')'); goto _LL372; _LL444: Cyc_Dump_dump_char((
int)'('); Cyc_Dump_dumptyp((* _temp599).f3); Cyc_Dump_dump_char(( int)')');((
void(*)( void(* f)( struct _tuple13*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)(
Cyc_Dump_dumpde, _temp597, _tag_arr("{\\lb}", sizeof( unsigned char), 6u),
_tag_arr("{\\rb}", sizeof( unsigned char), 6u), _tag_arr(",", sizeof(
unsigned char), 2u)); goto _LL372; _LL446:(( void(*)( void(* f)( struct _tuple13*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dumpde, _temp601, _tag_arr("{\\lb}",
sizeof( unsigned char), 6u), _tag_arr("{\\rb}", sizeof( unsigned char), 6u),
_tag_arr(",", sizeof( unsigned char), 2u)); goto _LL372; _LL448: Cyc_Dump_dump_alpha(
_tag_arr("new", sizeof( unsigned char), 4u)); Cyc_Dump_dump_char(( int)'{'); Cyc_Dump_dump_alpha(
_tag_arr("for", sizeof( unsigned char), 4u)); Cyc_Dump_dump_alphastr((* _temp607->name).f2);
Cyc_Dump_dump_char(( int)'<'); Cyc_Dump_dumpexp( _temp605); Cyc_Dump_dump_char((
int)':'); Cyc_Dump_dumpexp( _temp603); Cyc_Dump_dump_char(( int)'}'); goto
_LL372; _LL450: Cyc_Dump_dumpqvar( _temp611);(( void(*)( void(* f)( struct
_tuple13*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dumpde,
_temp609, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL372;
_LL452:(( void(*)( void(* f)( struct _tuple13*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)(
Cyc_Dump_dumpde, _temp613, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto
_LL372; _LL454: Cyc_Dump_dumpqvar( _temp615->name); if( _temp617 !=  0){(( void(*)(
void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct _tagged_arr
start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dumpexp,
_temp617, _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u));} goto _LL372;
_LL456: Cyc_Dump_dumpqvar( _temp619); goto _LL372; _LL458: if( _temp623 !=  0){
Cyc_Dump_dump_alpha( _tag_arr("rmalloc", sizeof( unsigned char), 8u)); Cyc_Dump_dump_char((
int)'('); Cyc_Dump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp623)); Cyc_Dump_dump_symbol(
_tag_arr(", ", sizeof( unsigned char), 3u));} else{ Cyc_Dump_dump_alpha(
_tag_arr("malloc", sizeof( unsigned char), 7u)); Cyc_Dump_dump_char(( int)'(');}
Cyc_Dump_dump_alpha( _tag_arr("sizeof", sizeof( unsigned char), 7u)); Cyc_Dump_dump_char((
int)'('); Cyc_Dump_dumptyp( _temp621); Cyc_Dump_dump_symbol( _tag_arr("))",
sizeof( unsigned char), 3u)); goto _LL372; _LL460:(( void(*)( void(* f)( struct
_tuple13*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dumpde,
_temp625, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL372;
_LL462: goto _LL372; _LL464: goto _LL372; _LL466: goto _LL372; _LL372:;} if(
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
struct Cyc_List_List* fields){ for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ Cyc_Dump_dump_symbol( _tag_arr("~~", sizeof(
unsigned char), 3u));{ struct Cyc_Absyn_Structfield _temp650; struct Cyc_List_List*
_temp651; struct Cyc_Absyn_Exp* _temp653; void* _temp655; struct Cyc_Absyn_Tqual
_temp657; struct _tagged_arr* _temp659; struct Cyc_Absyn_Structfield* _temp648=(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fields))->hd;
_temp650=* _temp648; _LL660: _temp659= _temp650.name; goto _LL658; _LL658:
_temp657= _temp650.tq; goto _LL656; _LL656: _temp655=( void*) _temp650.type;
goto _LL654; _LL654: _temp653= _temp650.width; goto _LL652; _LL652: _temp651=
_temp650.attributes; goto _LL649; _LL649:(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct _tagged_arr*), struct _tagged_arr*)) Cyc_Dump_dumptqtd)(
_temp657, _temp655, Cyc_Dump_dump_alphastr, _temp659); if( _temp653 !=  0){ Cyc_Dump_dump_char((
int)':'); Cyc_Dump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp653));}
Cyc_Dump_dump_symbol( _tag_arr(";\\\\\n", sizeof( unsigned char), 5u));}}} void
Cyc_Dump_dumptypedefname( struct Cyc_Absyn_Typedefdecl* td){ Cyc_Dump_dumpqvar(
td->name); Cyc_Dump_dumptvars( td->tvs);} void Cyc_Dump_dump_boldtypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Dump_dump_boldqvar( td->name); Cyc_Dump_dumptvars(
td->tvs);} static void Cyc_Dump_dumpids( struct Cyc_List_List* vds){ for( 0; vds
!=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_Dump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->name);
if((( struct Cyc_List_List*) _check_null( vds))->tl !=  0){ Cyc_Dump_dump_char((
int)',');}}} static void Cyc_Dump_dump_upperbound( struct Cyc_Absyn_Exp* e){
unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i !=  1){ Cyc_Dump_dump_char((
int)'{'); Cyc_Dump_dumpexp( e); Cyc_Dump_dump_char(( int)'}');}} void Cyc_Dump_dumptms(
struct Cyc_List_List* tms, void(* f)( void*), void* a){ if( tms ==  0){ f( a);
return;}{ void* _temp661=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Tqual _temp683; void* _temp685; void* _temp687; struct Cyc_Absyn_Exp*
_temp689; struct Cyc_Absyn_Tqual _temp691; void* _temp693; void* _temp695;
struct Cyc_Absyn_Exp* _temp697; struct Cyc_Absyn_Tqual _temp699; void* _temp701;
void* _temp703; struct Cyc_Absyn_Tqual _temp705; void* _temp707; struct Cyc_Absyn_Tvar*
_temp709; void* _temp711; struct Cyc_Absyn_Exp* _temp713; struct Cyc_Absyn_Tqual
_temp715; void* _temp717; struct Cyc_Absyn_Tvar* _temp719; void* _temp721;
struct Cyc_Absyn_Exp* _temp723; struct Cyc_Absyn_Tqual _temp725; void* _temp727;
struct Cyc_Absyn_Tvar* _temp729; void* _temp731; struct Cyc_Absyn_Tqual _temp733;
void* _temp735; struct Cyc_Absyn_Exp* _temp737; struct Cyc_Absyn_Tqual _temp739;
void* _temp741; struct Cyc_Absyn_Exp* _temp743; struct Cyc_Absyn_Tqual _temp745;
void* _temp747; _LL663: if(( unsigned int) _temp661 >  1u?*(( int*) _temp661) == 
Cyc_Absyn_Pointer_mod: 0){ _LL688: _temp687=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp661)->f1; if(( unsigned int) _temp687 >  1u?*(( int*) _temp687) ==  Cyc_Absyn_Nullable_ps:
0){ _LL690: _temp689=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp687)->f1;
goto _LL686;} else{ goto _LL665;} _LL686: _temp685=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp661)->f2; if( _temp685 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL684;} else{
goto _LL665;} _LL684: _temp683=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f3;
goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int) _temp661 >  1u?*((
int*) _temp661) ==  Cyc_Absyn_Pointer_mod: 0){ _LL696: _temp695=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f1; if(( unsigned int) _temp695
>  1u?*(( int*) _temp695) ==  Cyc_Absyn_NonNullable_ps: 0){ _LL698: _temp697=((
struct Cyc_Absyn_NonNullable_ps_struct*) _temp695)->f1; goto _LL694;} else{ goto
_LL667;} _LL694: _temp693=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp661)->f2; if( _temp693 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL692;} else{
goto _LL667;} _LL692: _temp691=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f3;
goto _LL666;} else{ goto _LL667;} _LL667: if(( unsigned int) _temp661 >  1u?*((
int*) _temp661) ==  Cyc_Absyn_Pointer_mod: 0){ _LL704: _temp703=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f1; if( _temp703 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL702;} else{ goto _LL669;} _LL702: _temp701=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f2; if( _temp701 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL700;} else{ goto _LL669;} _LL700: _temp699=((
struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f3; goto _LL668;} else{ goto
_LL669;} _LL669: if(( unsigned int) _temp661 >  1u?*(( int*) _temp661) ==  Cyc_Absyn_Pointer_mod:
0){ _LL712: _temp711=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f1;
if(( unsigned int) _temp711 >  1u?*(( int*) _temp711) ==  Cyc_Absyn_Nullable_ps:
0){ _LL714: _temp713=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp711)->f1;
goto _LL708;} else{ goto _LL671;} _LL708: _temp707=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp661)->f2; if(( unsigned int) _temp707 >  4u?*(( int*) _temp707) ==  Cyc_Absyn_VarType:
0){ _LL710: _temp709=(( struct Cyc_Absyn_VarType_struct*) _temp707)->f1; goto
_LL706;} else{ goto _LL671;} _LL706: _temp705=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp661)->f3; goto _LL670;} else{ goto _LL671;} _LL671: if(( unsigned int)
_temp661 >  1u?*(( int*) _temp661) ==  Cyc_Absyn_Pointer_mod: 0){ _LL722:
_temp721=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f1; if((
unsigned int) _temp721 >  1u?*(( int*) _temp721) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL724: _temp723=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp721)->f1; goto
_LL718;} else{ goto _LL673;} _LL718: _temp717=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp661)->f2; if(( unsigned int) _temp717 >  4u?*(( int*) _temp717) ==  Cyc_Absyn_VarType:
0){ _LL720: _temp719=(( struct Cyc_Absyn_VarType_struct*) _temp717)->f1; goto
_LL716;} else{ goto _LL673;} _LL716: _temp715=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp661)->f3; goto _LL672;} else{ goto _LL673;} _LL673: if(( unsigned int)
_temp661 >  1u?*(( int*) _temp661) ==  Cyc_Absyn_Pointer_mod: 0){ _LL732:
_temp731=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f1; if(
_temp731 == ( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL728;} else{ goto _LL675;}
_LL728: _temp727=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f2;
if(( unsigned int) _temp727 >  4u?*(( int*) _temp727) ==  Cyc_Absyn_VarType: 0){
_LL730: _temp729=(( struct Cyc_Absyn_VarType_struct*) _temp727)->f1; goto _LL726;}
else{ goto _LL675;} _LL726: _temp725=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp661)->f3; goto _LL674;} else{ goto _LL675;} _LL675: if(( unsigned int)
_temp661 >  1u?*(( int*) _temp661) ==  Cyc_Absyn_Pointer_mod: 0){ _LL736:
_temp735=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f1; if((
unsigned int) _temp735 >  1u?*(( int*) _temp735) ==  Cyc_Absyn_Nullable_ps: 0){
_LL738: _temp737=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp735)->f1; goto
_LL734;} else{ goto _LL677;} _LL734: _temp733=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp661)->f3; goto _LL676;} else{ goto _LL677;} _LL677: if(( unsigned int)
_temp661 >  1u?*(( int*) _temp661) ==  Cyc_Absyn_Pointer_mod: 0){ _LL742:
_temp741=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f1; if((
unsigned int) _temp741 >  1u?*(( int*) _temp741) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL744: _temp743=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp741)->f1; goto
_LL740;} else{ goto _LL679;} _LL740: _temp739=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp661)->f3; goto _LL678;} else{ goto _LL679;} _LL679: if(( unsigned int)
_temp661 >  1u?*(( int*) _temp661) ==  Cyc_Absyn_Pointer_mod: 0){ _LL748:
_temp747=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f1; if(
_temp747 == ( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL746;} else{ goto _LL681;}
_LL746: _temp745=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp661)->f3; goto
_LL680;} else{ goto _LL681;} _LL681: goto _LL682; _LL664: Cyc_Dump_dump_pointer(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_Dump_dump_upperbound( _temp689);
Cyc_Dump_dump_alpha( _tag_arr("`H", sizeof( unsigned char), 3u)); Cyc_Dump_dumptms(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL666: Cyc_Dump_dump_pointer(
_tag_arr("@", sizeof( unsigned char), 2u)); Cyc_Dump_dump_upperbound( _temp697);
Cyc_Dump_dump_alpha( _tag_arr("`H", sizeof( unsigned char), 3u)); Cyc_Dump_dumptms(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL668: Cyc_Dump_dump_pointer(
_tag_arr("{?}", sizeof( unsigned char), 4u)); Cyc_Dump_dump_alpha( _tag_arr("`H",
sizeof( unsigned char), 3u)); Cyc_Dump_dumptms((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL670: Cyc_Dump_dump_pointer( _tag_arr("*",
sizeof( unsigned char), 2u)); Cyc_Dump_dump_upperbound( _temp713); Cyc_Dump_dump_alphastr(
_temp709->name); Cyc_Dump_dumptms((( struct Cyc_List_List*) _check_null( tms))->tl,
f, a); return; _LL672: Cyc_Dump_dump_pointer( _tag_arr("@", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_upperbound( _temp723); Cyc_Dump_dump_alphastr(
_temp719->name); Cyc_Dump_dumptms((( struct Cyc_List_List*) _check_null( tms))->tl,
f, a); return; _LL674: Cyc_Dump_dump_pointer( _tag_arr("{?}", sizeof(
unsigned char), 4u)); Cyc_Dump_dump_alphastr( _temp729->name); Cyc_Dump_dumptms(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL676: Cyc_Dump_dump_pointer(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_Dump_dump_upperbound( _temp737);
Cyc_Dump_dumptms((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL678: Cyc_Dump_dump_pointer( _tag_arr("@", sizeof( unsigned char), 2u));
Cyc_Dump_dump_upperbound( _temp743); Cyc_Dump_dumptms((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL680: Cyc_Dump_dump_pointer( _tag_arr("{?}",
sizeof( unsigned char), 4u)); Cyc_Dump_dumptms((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL682: { int next_is_pointer= 0; if(((
struct Cyc_List_List*) _check_null( tms))->tl !=  0){ void* _temp749=( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd;
_LL751: if(( unsigned int) _temp749 >  1u?*(( int*) _temp749) ==  Cyc_Absyn_Pointer_mod:
0){ goto _LL752;} else{ goto _LL753;} _LL753: goto _LL754; _LL752:
next_is_pointer= 1; goto _LL750; _LL754: goto _LL750; _LL750:;} if(
next_is_pointer){ Cyc_Dump_dump_char(( int)'(');} Cyc_Dump_dumptms((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); if( next_is_pointer){ Cyc_Dump_dump_char(( int)')');}{
void* _temp755=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct
Cyc_Absyn_Exp* _temp771; void* _temp773; struct Cyc_List_List* _temp775; struct
Cyc_Core_Opt* _temp777; struct Cyc_Absyn_VarargInfo* _temp779; int _temp781;
struct Cyc_List_List* _temp783; void* _temp785; struct Cyc_Position_Segment*
_temp787; struct Cyc_List_List* _temp789; int _temp791; struct Cyc_Position_Segment*
_temp793; struct Cyc_List_List* _temp795; void* _temp797; void* _temp799; _LL757:
if( _temp755 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL758;} else{ goto _LL759;}
_LL759: if(( unsigned int) _temp755 >  1u?*(( int*) _temp755) ==  Cyc_Absyn_ConstArray_mod:
0){ _LL772: _temp771=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp755)->f1;
goto _LL760;} else{ goto _LL761;} _LL761: if(( unsigned int) _temp755 >  1u?*((
int*) _temp755) ==  Cyc_Absyn_Function_mod: 0){ _LL774: _temp773=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp755)->f1; if(*(( int*) _temp773) == 
Cyc_Absyn_WithTypes){ _LL784: _temp783=(( struct Cyc_Absyn_WithTypes_struct*)
_temp773)->f1; goto _LL782; _LL782: _temp781=(( struct Cyc_Absyn_WithTypes_struct*)
_temp773)->f2; goto _LL780; _LL780: _temp779=(( struct Cyc_Absyn_WithTypes_struct*)
_temp773)->f3; goto _LL778; _LL778: _temp777=(( struct Cyc_Absyn_WithTypes_struct*)
_temp773)->f4; goto _LL776; _LL776: _temp775=(( struct Cyc_Absyn_WithTypes_struct*)
_temp773)->f5; goto _LL762;} else{ goto _LL763;}} else{ goto _LL763;} _LL763:
if(( unsigned int) _temp755 >  1u?*(( int*) _temp755) ==  Cyc_Absyn_Function_mod:
0){ _LL786: _temp785=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp755)->f1;
if(*(( int*) _temp785) ==  Cyc_Absyn_NoTypes){ _LL790: _temp789=(( struct Cyc_Absyn_NoTypes_struct*)
_temp785)->f1; goto _LL788; _LL788: _temp787=(( struct Cyc_Absyn_NoTypes_struct*)
_temp785)->f2; goto _LL764;} else{ goto _LL765;}} else{ goto _LL765;} _LL765:
if(( unsigned int) _temp755 >  1u?*(( int*) _temp755) ==  Cyc_Absyn_TypeParams_mod:
0){ _LL796: _temp795=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp755)->f1;
goto _LL794; _LL794: _temp793=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp755)->f2; goto _LL792; _LL792: _temp791=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp755)->f3; goto _LL766;} else{ goto _LL767;} _LL767: if(( unsigned int)
_temp755 >  1u?*(( int*) _temp755) ==  Cyc_Absyn_Attributes_mod: 0){ goto _LL768;}
else{ goto _LL769;} _LL769: if(( unsigned int) _temp755 >  1u?*(( int*) _temp755)
==  Cyc_Absyn_Pointer_mod: 0){ _LL800: _temp799=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp755)->f1; goto _LL798; _LL798: _temp797=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp755)->f2; goto _LL770;} else{ goto _LL756;} _LL758: Cyc_Dump_dump_symbol(
_tag_arr("[]", sizeof( unsigned char), 3u)); goto _LL756; _LL760: Cyc_Dump_dump_char((
int)'['); Cyc_Dump_dumpexp( _temp771); Cyc_Dump_dump_char(( int)']'); goto
_LL756; _LL762: Cyc_Dump_dumpfunargs( _temp783, _temp781, _temp779, _temp777,
_temp775); goto _LL756; _LL764:(( void(*)( void(* f)( struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Dump_group)( Cyc_Dump_dump_alphastr, _temp789,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(", ", sizeof( unsigned char), 3u)); goto _LL756; _LL766: if(
_temp791){ Cyc_Dump_dumpkindedtvars( _temp795);} else{ Cyc_Dump_dumptvars(
_temp795);} goto _LL756; _LL768: goto _LL756; _LL770:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp801=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp801[ 0]=({ struct
Cyc_Core_Impossible_struct _temp802; _temp802.tag= Cyc_Core_Impossible; _temp802.f1=
_tag_arr("dumptms", sizeof( unsigned char), 8u); _temp802;}); _temp801;}));
_LL756:;} return;} _LL662:;}} void Cyc_Dump_dumptqtd( struct Cyc_Absyn_Tqual tq,
void* t, void(* f)( void*), void* a){ struct Cyc_List_List* _temp805; void*
_temp807; struct Cyc_Absyn_Tqual _temp809; struct _tuple9 _temp803= Cyc_Absynpp_to_tms(
tq, t); _LL810: _temp809= _temp803.f1; goto _LL808; _LL808: _temp807= _temp803.f2;
goto _LL806; _LL806: _temp805= _temp803.f3; goto _LL804; _LL804: Cyc_Dump_dumptq(
_temp809); Cyc_Dump_dumpntyp( _temp807); Cyc_Dump_dumptms( Cyc_List_imp_rev(
_temp805), f, a);} void Cyc_Dump_pr_comment( struct Cyc_Std___sFILE* outf,
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
_tagged_arr comment){ void* _temp811=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp837; struct Cyc_Absyn_Uniondecl* _temp839; struct Cyc_Absyn_Vardecl*
_temp841; struct Cyc_Absyn_Vardecl _temp843; void* _temp844; struct Cyc_Absyn_Tqual
_temp846; struct _tuple1* _temp848; struct Cyc_Absyn_Tuniondecl* _temp850;
struct Cyc_Absyn_Tuniondecl _temp852; int _temp853; struct Cyc_Core_Opt*
_temp855; struct Cyc_List_List* _temp857; struct _tuple1* _temp859; void*
_temp861; struct Cyc_Absyn_Enumdecl* _temp863; struct Cyc_Absyn_Enumdecl
_temp865; struct Cyc_Core_Opt* _temp866; struct _tuple1* _temp868; void*
_temp870; struct Cyc_Absyn_Typedefdecl* _temp872; _LL813: if(*(( int*) _temp811)
==  Cyc_Absyn_Struct_d){ _LL838: _temp837=(( struct Cyc_Absyn_Struct_d_struct*)
_temp811)->f1; goto _LL814;} else{ goto _LL815;} _LL815: if(*(( int*) _temp811)
==  Cyc_Absyn_Union_d){ _LL840: _temp839=(( struct Cyc_Absyn_Union_d_struct*)
_temp811)->f1; goto _LL816;} else{ goto _LL817;} _LL817: if(*(( int*) _temp811)
==  Cyc_Absyn_Var_d){ _LL842: _temp841=(( struct Cyc_Absyn_Var_d_struct*)
_temp811)->f1; _temp843=* _temp841; _LL849: _temp848= _temp843.name; goto _LL847;
_LL847: _temp846= _temp843.tq; goto _LL845; _LL845: _temp844=( void*) _temp843.type;
goto _LL818;} else{ goto _LL819;} _LL819: if(*(( int*) _temp811) ==  Cyc_Absyn_Tunion_d){
_LL851: _temp850=(( struct Cyc_Absyn_Tunion_d_struct*) _temp811)->f1; _temp852=*
_temp850; _LL862: _temp861=( void*) _temp852.sc; goto _LL860; _LL860: _temp859=
_temp852.name; goto _LL858; _LL858: _temp857= _temp852.tvs; goto _LL856; _LL856:
_temp855= _temp852.fields; goto _LL854; _LL854: _temp853= _temp852.is_xtunion;
goto _LL820;} else{ goto _LL821;} _LL821: if(*(( int*) _temp811) ==  Cyc_Absyn_Enum_d){
_LL864: _temp863=(( struct Cyc_Absyn_Enum_d_struct*) _temp811)->f1; _temp865=*
_temp863; _LL871: _temp870=( void*) _temp865.sc; goto _LL869; _LL869: _temp868=
_temp865.name; goto _LL867; _LL867: _temp866= _temp865.fields; goto _LL822;}
else{ goto _LL823;} _LL823: if(*(( int*) _temp811) ==  Cyc_Absyn_Typedef_d){
_LL873: _temp872=(( struct Cyc_Absyn_Typedef_d_struct*) _temp811)->f1; goto
_LL824;} else{ goto _LL825;} _LL825: if(*(( int*) _temp811) ==  Cyc_Absyn_Fn_d){
goto _LL826;} else{ goto _LL827;} _LL827: if(*(( int*) _temp811) ==  Cyc_Absyn_Let_d){
goto _LL828;} else{ goto _LL829;} _LL829: if(*(( int*) _temp811) ==  Cyc_Absyn_Letv_d){
goto _LL830;} else{ goto _LL831;} _LL831: if(*(( int*) _temp811) ==  Cyc_Absyn_Namespace_d){
goto _LL832;} else{ goto _LL833;} _LL833: if(*(( int*) _temp811) ==  Cyc_Absyn_Using_d){
goto _LL834;} else{ goto _LL835;} _LL835: if(*(( int*) _temp811) ==  Cyc_Absyn_ExternC_d){
goto _LL836;} else{ goto _LL812;} _LL814: Cyc_Dump_dump_begin(); Cyc_Dump_dump_alpha(
_tag_arr("struct", sizeof( unsigned char), 7u)); if( _temp837->name !=  0){ Cyc_Dump_dump_boldqvar((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp837->name))->v);} Cyc_Dump_dumptvars(
_temp837->tvs); if( _temp837->fields ==  0){ Cyc_Dump_dump_symbol( _tag_arr(";\n",
sizeof( unsigned char), 3u));} else{ Cyc_Dump_dump_symbol( _tag_arr(" ", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_lb(); Cyc_Dump_dump_symbol( _tag_arr("\\\\\n",
sizeof( unsigned char), 4u)); Cyc_Dump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp837->fields))->v); Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char), 3u));} Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment( comment); Cyc_Dump_dump_symbol( _tag_arr("\n", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_end(); goto _LL812; _LL816: Cyc_Dump_dump_begin();
Cyc_Dump_dump_alpha( _tag_arr("union", sizeof( unsigned char), 6u)); if(
_temp839->name !=  0){ Cyc_Dump_dump_boldqvar(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp839->name))->v);} Cyc_Dump_dumptvars( _temp839->tvs); if(
_temp839->fields ==  0){ Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof(
unsigned char), 3u));} else{ Cyc_Dump_dump_symbol( _tag_arr(" ", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_lb(); Cyc_Dump_dump_symbol( _tag_arr("\\\\\n",
sizeof( unsigned char), 4u)); Cyc_Dump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp839->fields))->v); Cyc_Dump_dump_rb();
Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char), 3u));} Cyc_Dump_dump_middle();
Cyc_Dump_dump_comment( comment); Cyc_Dump_dump_symbol( _tag_arr("\n", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_end(); goto _LL812; _LL818: Cyc_Dump_dump_begin();((
void(*)( struct Cyc_Absyn_Tqual tq, void* t, void(* f)( struct _tuple1*), struct
_tuple1* a)) Cyc_Dump_dumptqtd)( _temp846, _temp844, Cyc_Dump_dump_boldqvar,
_temp848); Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char), 3u));
Cyc_Dump_dump_middle(); Cyc_Dump_dump_comment( comment); Cyc_Dump_dump_symbol(
_tag_arr("\n", sizeof( unsigned char), 2u)); Cyc_Dump_dump_end(); goto _LL812;
_LL820: Cyc_Dump_dump_begin(); if( _temp853){ Cyc_Dump_dump_alpha( _tag_arr("xtunion",
sizeof( unsigned char), 8u));} else{ Cyc_Dump_dump_alpha( _tag_arr("tunion",
sizeof( unsigned char), 7u));} Cyc_Dump_dump_boldqvar( _temp859); Cyc_Dump_dumptvars(
_temp857); if( _temp855 ==  0){ Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof(
unsigned char), 3u));} else{ Cyc_Dump_dump_symbol( _tag_arr(" ", sizeof(
unsigned char), 2u)); Cyc_Dump_dump_lb(); Cyc_Dump_dump_symbol( _tag_arr("\\\\\n~~",
sizeof( unsigned char), 6u)); Cyc_Dump_dumptunionfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp855))->v); Cyc_Dump_dump_symbol(
_tag_arr("\\\\\n", sizeof( unsigned char), 4u)); Cyc_Dump_dump_rb(); Cyc_Dump_dump_symbol(
_tag_arr(";\n", sizeof( unsigned char), 3u));} Cyc_Dump_dump_middle(); Cyc_Dump_dump_comment(
comment); Cyc_Dump_dump_symbol( _tag_arr("\n", sizeof( unsigned char), 2u)); Cyc_Dump_dump_end();
goto _LL812; _LL822: Cyc_Dump_dump_begin(); Cyc_Dump_dump_alpha( _tag_arr("enum",
sizeof( unsigned char), 5u)); Cyc_Dump_dump_boldqvar( _temp868); if( _temp866 == 
0){ Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char), 3u));} else{
Cyc_Dump_dump_symbol( _tag_arr(" ", sizeof( unsigned char), 2u)); Cyc_Dump_dump_lb();
Cyc_Dump_dump_symbol( _tag_arr("\\\\\n~~", sizeof( unsigned char), 6u)); Cyc_Dump_dumpenumfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp866))->v); Cyc_Dump_dump_symbol(
_tag_arr("\\\\\n", sizeof( unsigned char), 4u)); Cyc_Dump_dump_rb(); Cyc_Dump_dump_symbol(
_tag_arr(";\n", sizeof( unsigned char), 3u));} Cyc_Dump_dump_middle(); Cyc_Dump_dump_comment(
comment); Cyc_Dump_dump_symbol( _tag_arr("\n", sizeof( unsigned char), 2u)); Cyc_Dump_dump_end();
goto _LL812; _LL824: Cyc_Dump_dump_begin(); Cyc_Dump_dump_alpha( _tag_arr("typedef",
sizeof( unsigned char), 8u));(( void(*)( struct Cyc_Absyn_Tqual tq, void* t,
void(* f)( struct Cyc_Absyn_Typedefdecl*), struct Cyc_Absyn_Typedefdecl* a)) Cyc_Dump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp874; _temp874.q_const= 0; _temp874.q_volatile= 0;
_temp874.q_restrict= 0; _temp874;}),( void*) _temp872->defn, Cyc_Dump_dump_boldtypedefname,
_temp872); Cyc_Dump_dump_symbol( _tag_arr(";\n", sizeof( unsigned char), 3u));
Cyc_Dump_dump_middle(); Cyc_Dump_dump_comment( comment); Cyc_Dump_dump_symbol(
_tag_arr("\n", sizeof( unsigned char), 2u)); Cyc_Dump_dump_end(); goto _LL812;
_LL826: goto _LL812; _LL828: goto _LL812; _LL830: goto _LL812; _LL832: goto
_LL812; _LL834: goto _LL812; _LL836: goto _LL812; _LL812:;} static int Cyc_is_other_special(
unsigned char c){ switch( c){ case '\\': _LL875: goto _LL876; case '"': _LL876:
goto _LL877; case ';': _LL877: goto _LL878; case '&': _LL878: goto _LL879; case
'(': _LL879: goto _LL880; case ')': _LL880: goto _LL881; case '|': _LL881: goto
_LL882; case '^': _LL882: goto _LL883; case '<': _LL883: goto _LL884; case '>':
_LL884: goto _LL885; case ' ': _LL885: goto _LL886; case '\n': _LL886: goto
_LL887; case '\t': _LL887: return 1; default: _LL888: return 0;}} static struct
_tagged_arr Cyc_sh_escape_string( struct _tagged_arr s){ unsigned int _temp890=
Cyc_Std_strlen( s); int _temp891= 0; int _temp892= 0;{ int i= 0; for( 0; i < 
_temp890; i ++){ unsigned char _temp893=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( _temp893 == '\''){
_temp891 ++;} else{ if( Cyc_is_other_special( _temp893)){ _temp892 ++;}}}} if(
_temp891 ==  0? _temp892 ==  0: 0){ return s;} if( _temp891 ==  0){ return(
struct _tagged_arr) Cyc_Std_strconcat_l(({ struct _tagged_arr* _temp894[ 3u];
_temp894[ 2u]= _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)),"'", sizeof( unsigned char), 2u); _temp894[ 1u]=({ struct
_tagged_arr* _temp895=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp895[ 0]=( struct _tagged_arr) s; _temp895;}); _temp894[ 0u]=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"'",
sizeof( unsigned char), 2u);(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp894, sizeof( struct _tagged_arr*), 3u));}));}{
unsigned int _temp896=( _temp890 +  _temp891) +  _temp892; struct _tagged_arr
_temp897=({ unsigned int _temp901= _temp896 +  1; unsigned char* _temp902=(
unsigned char*) _cycalloc_atomic( _check_times( sizeof( unsigned char), _temp901));
struct _tagged_arr _temp904= _tag_arr( _temp902, sizeof( unsigned char),
_temp896 +  1);{ unsigned int _temp903= _temp901; unsigned int i; for( i= 0; i < 
_temp903; i ++){ _temp902[ i]='\000';}}; _temp904;}); int _temp898= 0; int
_temp899= 0; for( 0; _temp898 <  _temp890; _temp898 ++){ unsigned char _temp900=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
_temp898)); if( _temp900 == '\''? 1: Cyc_is_other_special( _temp900)){*((
unsigned char*) _check_unknown_subscript( _temp897, sizeof( unsigned char),
_temp899 ++))='\\';}*(( unsigned char*) _check_unknown_subscript( _temp897,
sizeof( unsigned char), _temp899 ++))= _temp900;} return( struct _tagged_arr)
_temp897;}} static struct _tagged_arr* Cyc_sh_escape_stringptr( struct
_tagged_arr* sp){ return({ struct _tagged_arr* _temp905=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp905[ 0]= Cyc_sh_escape_string(* sp);
_temp905;});} static struct Cyc_Lineno_Pos* Cyc_new_pos(){ return({ struct Cyc_Lineno_Pos*
_temp906=( struct Cyc_Lineno_Pos*) _cycalloc( sizeof( struct Cyc_Lineno_Pos));
_temp906->logical_file= _tag_arr("", sizeof( unsigned char), 1u); _temp906->line=
Cyc_Core_new_string( 0); _temp906->line_no= 0; _temp906->col= 0; _temp906;});}
struct _tuple14{ int f1; struct Cyc_Lineno_Pos* f2; } ; static struct _tuple14*
Cyc_start2pos( int x){ return({ struct _tuple14* _temp907=( struct _tuple14*)
_cycalloc( sizeof( struct _tuple14)); _temp907->f1= x; _temp907->f2= Cyc_new_pos();
_temp907;});} static int Cyc_decl2start( struct Cyc_Absyn_Decl* d){ return((
struct Cyc_Position_Segment*) _check_null( d->loc))->start;} struct _tuple15{
struct Cyc_Lineno_Pos* f1; void* f2; } ; static struct Cyc_List_List* Cyc_this_file(
struct _tagged_arr file, struct Cyc_List_List* x){ struct Cyc_List_List*
_temp908= 0; for( 0; x !=  0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
if( Cyc_Std_strcmp(((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
x))->hd)).f1)->logical_file, file) ==  0){ _temp908=({ struct Cyc_List_List*
_temp909=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp909->hd=( void*)(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
x))->hd); _temp909->tl= _temp908; _temp909;});}} _temp908=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp908); return _temp908;} static
int Cyc_lineno( struct Cyc_Lineno_Pos* p){ return p->line_no;} static struct Cyc_List_List*
Cyc_flatten_decls( struct Cyc_List_List* decls){ struct Cyc_List_List* _temp910=
0; while( decls !=  0) { void* _temp911=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( decls))->hd)->r; struct Cyc_List_List*
_temp937; struct Cyc_List_List* _temp939; struct Cyc_List_List* _temp941; _LL913:
if(*(( int*) _temp911) ==  Cyc_Absyn_Namespace_d){ _LL938: _temp937=(( struct
Cyc_Absyn_Namespace_d_struct*) _temp911)->f2; goto _LL914;} else{ goto _LL915;}
_LL915: if(*(( int*) _temp911) ==  Cyc_Absyn_Using_d){ _LL940: _temp939=((
struct Cyc_Absyn_Using_d_struct*) _temp911)->f2; goto _LL916;} else{ goto _LL917;}
_LL917: if(*(( int*) _temp911) ==  Cyc_Absyn_ExternC_d){ _LL942: _temp941=((
struct Cyc_Absyn_ExternC_d_struct*) _temp911)->f1; goto _LL918;} else{ goto
_LL919;} _LL919: if(*(( int*) _temp911) ==  Cyc_Absyn_Struct_d){ goto _LL920;}
else{ goto _LL921;} _LL921: if(*(( int*) _temp911) ==  Cyc_Absyn_Union_d){ goto
_LL922;} else{ goto _LL923;} _LL923: if(*(( int*) _temp911) ==  Cyc_Absyn_Var_d){
goto _LL924;} else{ goto _LL925;} _LL925: if(*(( int*) _temp911) ==  Cyc_Absyn_Tunion_d){
goto _LL926;} else{ goto _LL927;} _LL927: if(*(( int*) _temp911) ==  Cyc_Absyn_Enum_d){
goto _LL928;} else{ goto _LL929;} _LL929: if(*(( int*) _temp911) ==  Cyc_Absyn_Typedef_d){
goto _LL930;} else{ goto _LL931;} _LL931: if(*(( int*) _temp911) ==  Cyc_Absyn_Fn_d){
goto _LL932;} else{ goto _LL933;} _LL933: if(*(( int*) _temp911) ==  Cyc_Absyn_Let_d){
goto _LL934;} else{ goto _LL935;} _LL935: if(*(( int*) _temp911) ==  Cyc_Absyn_Letv_d){
goto _LL936;} else{ goto _LL912;} _LL914: _temp939= _temp937; goto _LL916;
_LL916: _temp941= _temp939; goto _LL918; _LL918: decls=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp941,((
struct Cyc_List_List*) _check_null( decls))->tl); goto _LL912; _LL920: goto
_LL922; _LL922: goto _LL924; _LL924: goto _LL926; _LL926: goto _LL928; _LL928:
goto _LL930; _LL930: goto _LL932; _LL932: goto _LL934; _LL934: goto _LL936;
_LL936: _temp910=({ struct Cyc_List_List* _temp943=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp943->hd=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( decls))->hd); _temp943->tl= _temp910;
_temp943;}); decls=(( struct Cyc_List_List*) _check_null( decls))->tl; goto
_LL912; _LL912:;} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp910);} static struct Cyc_List_List* Cyc_cycdoc_files= 0;
static void Cyc_add_other( struct _tagged_arr s){ Cyc_cycdoc_files=({ struct Cyc_List_List*
_temp944=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp944->hd=( void*)({ struct _tagged_arr* _temp945=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp945[ 0]= s; _temp945;}); _temp944->tl=
Cyc_cycdoc_files; _temp944;});} static struct Cyc_List_List* Cyc_cycargs= 0;
static void Cyc_add_cycarg( struct _tagged_arr s){ Cyc_cycargs=({ struct Cyc_List_List*
_temp946=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp946->hd=( void*)({ struct _tagged_arr* _temp947=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp947[ 0]= s; _temp947;}); _temp946->tl=
Cyc_cycargs; _temp946;});} static unsigned char _temp948[ 8u]="cyclone"; static
struct _tagged_arr Cyc_cyclone_file={ _temp948, _temp948, _temp948 +  8u};
static void Cyc_set_cyclone_file( struct _tagged_arr s){ Cyc_cyclone_file= s;}
struct _tuple16{ int f1; struct Cyc_Absyn_Decl* f2; } ; struct _tuple17{ struct
Cyc_Lineno_Pos* f1; struct Cyc_Absyn_Decl* f2; } ; static void Cyc_process_file(
struct _tagged_arr filename){ struct _tagged_arr _temp949= Cyc_Filename_chop_extension(
filename); struct _tagged_arr _temp950= Cyc_Std_strconcat(( struct _tagged_arr)
_temp949, _tag_arr(".cyp", sizeof( unsigned char), 5u)); struct _tagged_arr
_temp951= Cyc_Std_str_sepstr(({ struct Cyc_List_List* _temp999=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp999->hd=( void*) _init_tag_arr((
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"", sizeof(
unsigned char), 1u); _temp999->tl=(( struct Cyc_List_List*(*)( struct
_tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( Cyc_cycargs)); _temp999;}), _tag_arr(" ", sizeof( unsigned char),
2u)); struct _tagged_arr _temp952=({ struct Cyc_Std_String_pa_struct _temp998;
_temp998.tag= Cyc_Std_String_pa; _temp998.f1=( struct _tagged_arr) Cyc_sh_escape_string(
filename);{ struct Cyc_Std_String_pa_struct _temp997; _temp997.tag= Cyc_Std_String_pa;
_temp997.f1=( struct _tagged_arr) Cyc_sh_escape_string(( struct _tagged_arr)
_temp950);{ struct Cyc_Std_String_pa_struct _temp996; _temp996.tag= Cyc_Std_String_pa;
_temp996.f1=( struct _tagged_arr) _temp951;{ struct Cyc_Std_String_pa_struct
_temp995; _temp995.tag= Cyc_Std_String_pa; _temp995.f1=( struct _tagged_arr) Cyc_cyclone_file;{
void* _temp994[ 4u]={& _temp995,& _temp996,& _temp997,& _temp998}; Cyc_Std_aprintf(
_tag_arr("%s %s -E -o %s -x cyc %s", sizeof( unsigned char), 25u), _tag_arr(
_temp994, sizeof( void*), 4u));}}}}}); if( system( string_to_Cstring(( struct
_tagged_arr) _temp952)) !=  0){({ void* _temp953[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("\nError: preprocessing\n", sizeof( unsigned char), 23u), _tag_arr(
_temp953, sizeof( void*), 0u));}); return;} Cyc_Position_reset_position(( struct
_tagged_arr) _temp950);{ struct Cyc_Std___sFILE* in_file=( struct Cyc_Std___sFILE*)
_check_null( Cyc_Std_fopen(( struct _tagged_arr) _temp950, _tag_arr("r", sizeof(
unsigned char), 2u))); Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp954= Cyc_Parse_parse_file(
in_file); Cyc_Lex_lex_init(); Cyc_Std_file_close(( struct Cyc_Std___sFILE*)
in_file); _temp954= Cyc_flatten_decls( _temp954);{ struct Cyc_List_List*
_temp955=(( struct Cyc_List_List*(*)( struct _tuple14*(* f)( int), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_start2pos,(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_decl2start,
_temp954)); Cyc_Lineno_poss_of_abss(( struct _tagged_arr) _temp950, _temp955);
Cyc_Std_remove(( struct _tagged_arr) _temp950);{ struct Cyc_List_List* _temp956=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(((
struct Cyc_List_List*(*)( struct Cyc_Lineno_Pos*(* f)( struct _tuple14*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Lineno_Pos*(*)( struct _tuple14*))
Cyc_Core_snd, _temp955), _temp954); _temp956=(( struct Cyc_List_List*(*)( struct
_tagged_arr file, struct Cyc_List_List* x)) Cyc_this_file)( filename, _temp956);{
struct Cyc_List_List* _temp957=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)((( struct Cyc_List_List*(*)( int(* f)(
struct Cyc_Lineno_Pos*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_lineno,((
struct Cyc_List_List*(*)( struct Cyc_Lineno_Pos*(* f)( struct _tuple17*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Lineno_Pos*(*)( struct _tuple17*))
Cyc_Core_fst, _temp956)),(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)(
struct _tuple17*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Decl*(*)(
struct _tuple17*)) Cyc_Core_snd, _temp956)); struct Cyc_Std___sFILE* _temp958=(
struct Cyc_Std___sFILE*) _check_null( Cyc_Std_fopen( filename, _tag_arr("r",
sizeof( unsigned char), 2u))); struct Cyc_Lexing_lexbuf* _temp959= Cyc_Lexing_from_file(
_temp958); struct Cyc_List_List* _temp960= 0; struct _tuple10* comment; while((
comment=(( struct _tuple10*(*)( struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_token)(
_temp959)) !=  0) { _temp960=({ struct Cyc_List_List* _temp961=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp961->hd=( void*)(( struct
_tuple10*) _check_null( comment)); _temp961->tl= _temp960; _temp961;});} Cyc_Std_fclose(
_temp958); _temp960=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp960);{ struct Cyc_List_List* _temp962=(( struct Cyc_List_List*(*)( struct
_tuple14*(* f)( int), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_start2pos,((
struct Cyc_List_List*(*)( int(* f)( struct _tuple10*), struct Cyc_List_List* x))
Cyc_List_map)(( int(*)( struct _tuple10*)) Cyc_Core_fst, _temp960)); Cyc_Lineno_poss_of_abss(
filename, _temp962);{ struct Cyc_List_List* _temp963=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)((( struct Cyc_List_List*(*)(
struct Cyc_Lineno_Pos*(* f)( struct _tuple14*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)( struct _tuple14*)) Cyc_Core_snd, _temp962),(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple10*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple10*)) Cyc_Core_snd, _temp960)); _temp963= Cyc_this_file(
filename, _temp963);{ struct Cyc_List_List* _temp964=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)((( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Lineno_Pos*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_lineno,((
struct Cyc_List_List*(*)( struct Cyc_Lineno_Pos*(* f)( struct _tuple15*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Lineno_Pos*(*)( struct _tuple15*))
Cyc_Core_fst, _temp963)),(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple15*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple15*))
Cyc_Core_snd, _temp963)); while( _temp957 !=  0? _temp964 !=  0: 0) { struct
_tuple16 _temp967; struct Cyc_Absyn_Decl* _temp968; int _temp970; struct
_tuple16* _temp965=( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp957))->hd; _temp967=* _temp965; _LL971: _temp970= _temp967.f1; goto _LL969;
_LL969: _temp968= _temp967.f2; goto _LL966; _LL966: { struct _tuple10 _temp974;
void* _temp975; int _temp977; struct _tuple10* _temp972=( struct _tuple10*)((
struct Cyc_List_List*) _check_null( _temp964))->hd; _temp974=* _temp972; _LL978:
_temp977= _temp974.f1; goto _LL976; _LL976: _temp975= _temp974.f2; goto _LL973;
_LL973: { void* _temp979= _temp975; struct _tagged_arr _temp985; struct
_tagged_arr _temp987; _LL981: if(*(( int*) _temp979) ==  Cyc_Standalone){ _LL986:
_temp985=(( struct Cyc_Standalone_struct*) _temp979)->f1; goto _LL982;} else{
goto _LL983;} _LL983: if(*(( int*) _temp979) ==  Cyc_MatchDecl){ _LL988:
_temp987=(( struct Cyc_MatchDecl_struct*) _temp979)->f1; goto _LL984;} else{
goto _LL980;} _LL982: Cyc_Dump_dump_comment(( struct _tagged_arr) _temp985); Cyc_Dump_dump_symbol(
_tag_arr("\n", sizeof( unsigned char), 2u)); _temp964=(( struct Cyc_List_List*)
_check_null( _temp964))->tl; goto _LL980; _LL984: if( _temp977 <  _temp970){
_temp964=(( struct Cyc_List_List*) _check_null( _temp964))->tl; continue;} if(((
struct Cyc_List_List*) _check_null( _temp957))->tl !=  0){ struct _tuple16
_temp991; int _temp992; struct _tuple16* _temp989=( struct _tuple16*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( _temp957))->tl))->hd;
_temp991=* _temp989; _LL993: _temp992= _temp991.f1; goto _LL990; _LL990: if(
_temp992 <  _temp977){ _temp957=(( struct Cyc_List_List*) _check_null( _temp957))->tl;
continue;}} Cyc_Dump_dumpdecl( _temp968,( struct _tagged_arr) _temp987); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stdout); _temp957=(( struct Cyc_List_List*)
_check_null( _temp957))->tl; _temp964=(( struct Cyc_List_List*) _check_null(
_temp964))->tl; goto _LL980; _LL980:;}}}}}}}}}}}} struct _tuple18{ struct
_tagged_arr f1; int f2; struct _tagged_arr f3; void* f4; struct _tagged_arr f5;
} ; int Cyc_main( int argc, struct _tagged_arr argv){ struct Cyc_List_List*
options=({ struct _tuple18* _temp1001[ 4u]; _temp1001[ 3u]=({ struct _tuple18*
_temp1011=( struct _tuple18*) _cycalloc( sizeof( struct _tuple18)); _temp1011->f1=
_tag_arr("-B", sizeof( unsigned char), 3u); _temp1011->f2= 1; _temp1011->f3=
_tag_arr("<file>", sizeof( unsigned char), 7u); _temp1011->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp1012=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp1012[ 0]=({ struct
Cyc_Arg_Flag_spec_struct _temp1013; _temp1013.tag= Cyc_Arg_Flag_spec; _temp1013.f1=
Cyc_add_cycarg; _temp1013;}); _temp1012;}); _temp1011->f5= _tag_arr("Add to the list of directories to search for compiler files",
sizeof( unsigned char), 60u); _temp1011;}); _temp1001[ 2u]=({ struct _tuple18*
_temp1008=( struct _tuple18*) _cycalloc( sizeof( struct _tuple18)); _temp1008->f1=
_tag_arr("-I", sizeof( unsigned char), 3u); _temp1008->f2= 1; _temp1008->f3=
_tag_arr("<dir>", sizeof( unsigned char), 6u); _temp1008->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp1009=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp1009[ 0]=({ struct
Cyc_Arg_Flag_spec_struct _temp1010; _temp1010.tag= Cyc_Arg_Flag_spec; _temp1010.f1=
Cyc_add_cycarg; _temp1010;}); _temp1009;}); _temp1008->f5= _tag_arr("Add to the list of directories to search for include files",
sizeof( unsigned char), 59u); _temp1008;}); _temp1001[ 1u]=({ struct _tuple18*
_temp1005=( struct _tuple18*) _cycalloc( sizeof( struct _tuple18)); _temp1005->f1=
_tag_arr("-D", sizeof( unsigned char), 3u); _temp1005->f2= 1; _temp1005->f3=
_tag_arr("<name>[=<value>]", sizeof( unsigned char), 17u); _temp1005->f4=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp1006=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp1006[ 0]=({ struct
Cyc_Arg_Flag_spec_struct _temp1007; _temp1007.tag= Cyc_Arg_Flag_spec; _temp1007.f1=
Cyc_add_cycarg; _temp1007;}); _temp1006;}); _temp1005->f5= _tag_arr("Pass definition to preprocessor",
sizeof( unsigned char), 32u); _temp1005;}); _temp1001[ 0u]=({ struct _tuple18*
_temp1002=( struct _tuple18*) _cycalloc( sizeof( struct _tuple18)); _temp1002->f1=
_tag_arr("-cyclone", sizeof( unsigned char), 9u); _temp1002->f2= 0; _temp1002->f3=
_tag_arr(" <file>", sizeof( unsigned char), 8u); _temp1002->f4=( void*)({ struct
Cyc_Arg_String_spec_struct* _temp1003=( struct Cyc_Arg_String_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp1003[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp1004; _temp1004.tag= Cyc_Arg_String_spec;
_temp1004.f1= Cyc_set_cyclone_file; _temp1004;}); _temp1003;}); _temp1002->f5=
_tag_arr("Use <file> as the cyclone compiler", sizeof( unsigned char), 35u);
_temp1002;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1001, sizeof( struct _tuple18*), 4u));}); Cyc_Arg_parse( options,
Cyc_add_other, _tag_arr("Options:", sizeof( unsigned char), 9u), argv);{ struct
Cyc_List_List* _temp1000=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( Cyc_cycdoc_files); for( 0; _temp1000 !=  0; _temp1000=(( struct
Cyc_List_List*) _check_null( _temp1000))->tl){ Cyc_process_file(*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp1000))->hd));}} return
0;}
