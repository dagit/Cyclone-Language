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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
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
extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Std___sFILE*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* x); extern void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum); extern struct
Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y); struct _tuple0{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; extern struct _tuple0 Cyc_List_split( struct
Cyc_List_List* x); extern unsigned int Cyc_Std_strlen( struct _tagged_arr s);
extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern
int Cyc_Std_zstrcmp( struct _tagged_arr, struct _tagged_arr); extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_Std_strcat(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_Std_strcpy(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr, int ofs, unsigned int n); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; extern
struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int); extern
struct Cyc_Position_Segment* Cyc_Position_segment_join( struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*); extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt;
struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_SwitchC_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Static= 0;
static const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2;
static const int Cyc_Absyn_Extern= 3; static const int Cyc_Absyn_ExternC= 4;
struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1; }
; static const int Cyc_Absyn_B1= 0; static const int Cyc_Absyn_B2= 1; static
const int Cyc_Absyn_B4= 2; static const int Cyc_Absyn_B8= 3; static const int
Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind= 1; static const int
Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3; static const int
Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0; static const int Cyc_Absyn_Unsigned=
1; struct Cyc_Absyn_Conref{ void* v; } ; static const int Cyc_Absyn_Eq_constr= 0;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr=
1; struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1;
} ; static const int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_arr* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; int inject; } ; struct Cyc_Absyn_FnInfo{
struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct
Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs;
struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
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
15u]; extern void* Cyc_Absyn_rel_ns_null; extern int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual( struct
Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt*
k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*);
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t; extern struct
_tuple1* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one;
extern void* Cyc_Absyn_starb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq,
void* b); extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq, void* b); extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp* rgn_handle,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_arr f, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_arr s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp( void* t, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeofexp_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_offsetof_exp(
void*, struct _tagged_arr*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_gentyp_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_arr*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct
Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment*
s); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_letv_decl( struct Cyc_List_List*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts,
struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl( void* s, struct Cyc_Core_Opt*
n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl(
void* s, struct _tuple1* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs,
int is_xtunion, struct Cyc_Position_Segment* loc); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs,
struct Cyc_List_List* rgn_po, struct Cyc_List_List*); extern struct _tagged_arr
Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att( void* a);
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_copy_type(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Lex_register_typedef(
struct _tuple1* s); extern void Cyc_Lex_enter_namespace( struct _tagged_arr*);
extern void Cyc_Lex_leave_namespace(); extern void Cyc_Lex_enter_using( struct
_tuple1*); extern void Cyc_Lex_leave_using(); enum  Cyc_Parse_Struct_or_union{
Cyc_Parse_Struct_su  =  0u, Cyc_Parse_Union_su  =  1u}; static const int Cyc_Parse_Signed_spec=
0; struct Cyc_Parse_Signed_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Unsigned_spec= 1; struct Cyc_Parse_Unsigned_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Short_spec=
2; struct Cyc_Parse_Short_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Long_spec= 3; struct Cyc_Parse_Long_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Type_spec=
4; struct Cyc_Parse_Type_spec_struct{ int tag; void* f1; struct Cyc_Position_Segment*
f2; } ; static const int Cyc_Parse_Decl_spec= 5; struct Cyc_Parse_Decl_spec_struct{
int tag; struct Cyc_Absyn_Decl* f1; } ; enum  Cyc_Parse_Storage_class{ Cyc_Parse_Typedef_sc
 =  0u, Cyc_Parse_Extern_sc  =  1u, Cyc_Parse_ExternC_sc  =  2u, Cyc_Parse_Static_sc
 =  3u, Cyc_Parse_Auto_sc  =  4u, Cyc_Parse_Register_sc  =  5u, Cyc_Parse_Abstract_sc
 =  6u}; struct Cyc_Parse_Declaration_spec{ struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual
tq; struct Cyc_List_List* type_specs; int is_inline; struct Cyc_List_List*
attributes; } ; struct Cyc_Parse_Declarator{ struct _tuple1* id; struct Cyc_List_List*
tms; } ; struct Cyc_Parse_Abstractdeclarator{ struct Cyc_List_List* tms; } ;
struct _tuple4{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment*
loc, struct _tuple2* t); struct _tuple5{ void* f1; struct Cyc_Core_Opt* f2; } ;
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc); struct _tuple6{ struct Cyc_Absyn_Tqual f1;
void* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ; static struct
_tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual, void*, struct Cyc_List_List*,
struct Cyc_List_List*); struct _tuple7{ struct _tuple1* f1; struct Cyc_Absyn_Tqual
f2; void* f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; } ; static
struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t); struct Cyc_Core_Opt* Cyc_Parse_lbuf= 0; static struct
Cyc_List_List* Cyc_Parse_parse_result= 0; struct Cyc_Absyn_Aligned_att_struct
Cyc_Parse_att_aligned={ 1u, - 1}; struct _tuple8{ struct _tagged_arr f1; void*
f2; } ; static unsigned char _temp1[ 8u]="stdcall"; static unsigned char _temp2[
6u]="cdecl"; static unsigned char _temp3[ 9u]="fastcall"; static unsigned char
_temp4[ 9u]="noreturn"; static unsigned char _temp5[ 6u]="const"; static
unsigned char _temp6[ 8u]="aligned"; static unsigned char _temp7[ 7u]="packed";
static unsigned char _temp8[ 7u]="shared"; static unsigned char _temp9[ 7u]="unused";
static unsigned char _temp10[ 5u]="weak"; static unsigned char _temp11[ 10u]="dllimport";
static unsigned char _temp12[ 10u]="dllexport"; static unsigned char _temp13[ 23u]="no_instrument_function";
static unsigned char _temp14[ 12u]="constructor"; static unsigned char _temp15[
11u]="destructor"; static unsigned char _temp16[ 22u]="no_check_memory_usage";
struct _tuple8 Cyc_Parse_att_map[ 16u]={{{ _temp1, _temp1, _temp1 +  8u},( void*)
0u},{{ _temp2, _temp2, _temp2 +  6u},( void*) 1u},{{ _temp3, _temp3, _temp3 +  9u},(
void*) 2u},{{ _temp4, _temp4, _temp4 +  9u},( void*) 3u},{{ _temp5, _temp5,
_temp5 +  6u},( void*) 4u},{{ _temp6, _temp6, _temp6 +  8u},( void*)& Cyc_Parse_att_aligned},{{
_temp7, _temp7, _temp7 +  7u},( void*) 5u},{{ _temp8, _temp8, _temp8 +  7u},(
void*) 7u},{{ _temp9, _temp9, _temp9 +  7u},( void*) 8u},{{ _temp10, _temp10,
_temp10 +  5u},( void*) 9u},{{ _temp11, _temp11, _temp11 +  10u},( void*) 10u},{{
_temp12, _temp12, _temp12 +  10u},( void*) 11u},{{ _temp13, _temp13, _temp13 + 
23u},( void*) 12u},{{ _temp14, _temp14, _temp14 +  12u},( void*) 13u},{{ _temp15,
_temp15, _temp15 +  11u},( void*) 14u},{{ _temp16, _temp16, _temp16 +  22u},(
void*) 15u}}; static void Cyc_Parse_err( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){ Cyc_Position_post_error( Cyc_Position_mk_err_parse( sg, msg));} static void*
Cyc_Parse_abort( struct _tagged_arr msg, struct Cyc_Position_Segment* sg){ Cyc_Parse_err(
msg, sg);( int) _throw(( void*) Cyc_Position_Exit);} static void Cyc_Parse_warn(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){({ struct Cyc_Std_String_pa_struct
_temp19; _temp19.tag= Cyc_Std_String_pa; _temp19.f1=( struct _tagged_arr) msg;{
struct Cyc_Std_String_pa_struct _temp18; _temp18.tag= Cyc_Std_String_pa; _temp18.f1=(
struct _tagged_arr) Cyc_Position_string_of_segment( sg);{ void* _temp17[ 2u]={&
_temp18,& _temp19}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s: Warning: %s\n",
sizeof( unsigned char), 17u), _tag_arr( _temp17, sizeof( void*), 2u));}}});
return;} static void* Cyc_Parse_unimp( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){ return Cyc_Parse_abort(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp21; _temp21.tag= Cyc_Std_String_pa; _temp21.f1=( struct _tagged_arr) msg;{
void* _temp20[ 1u]={& _temp21}; Cyc_Std_aprintf( _tag_arr("%s unimplemented",
sizeof( unsigned char), 17u), _tag_arr( _temp20, sizeof( void*), 1u));}}), sg);}
static void Cyc_Parse_unimp2( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg){({ struct Cyc_Std_String_pa_struct _temp24; _temp24.tag= Cyc_Std_String_pa;
_temp24.f1=( struct _tagged_arr) msg;{ struct Cyc_Std_String_pa_struct _temp23;
_temp23.tag= Cyc_Std_String_pa; _temp23.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
sg);{ void* _temp22[ 2u]={& _temp23,& _temp24}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: Warning: Cyclone does not yet support %s\n", sizeof( unsigned char),
46u), _tag_arr( _temp22, sizeof( void*), 2u));}}}); return;} struct _tuple9{
struct _tuple7* f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Structfield*
Cyc_Parse_make_struct_field( struct Cyc_Position_Segment* loc, struct _tuple9*
field_info){ struct _tuple9 _temp27; struct Cyc_Absyn_Exp* _temp28; struct
_tuple7* _temp30; struct _tuple7 _temp32; struct Cyc_List_List* _temp33; struct
Cyc_List_List* _temp35; void* _temp37; struct Cyc_Absyn_Tqual _temp39; struct
_tuple1* _temp41; struct _tuple9* _temp25= field_info; _temp27=* _temp25; _LL31:
_temp30= _temp27.f1; _temp32=* _temp30; _LL42: _temp41= _temp32.f1; goto _LL40;
_LL40: _temp39= _temp32.f2; goto _LL38; _LL38: _temp37= _temp32.f3; goto _LL36;
_LL36: _temp35= _temp32.f4; goto _LL34; _LL34: _temp33= _temp32.f5; goto _LL29;
_LL29: _temp28= _temp27.f2; goto _LL26; _LL26: if( _temp35 !=  0){ Cyc_Parse_err(
_tag_arr("bad type params in struct field", sizeof( unsigned char), 32u), loc);}
if( Cyc_Absyn_is_qvar_qualified( _temp41)){ Cyc_Parse_err( _tag_arr("struct field cannot be qualified with a namespace",
sizeof( unsigned char), 50u), loc);} return({ struct Cyc_Absyn_Structfield*
_temp43=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp43->name=(* _temp41).f2; _temp43->tq= _temp39; _temp43->type=( void*)
_temp37; _temp43->width= _temp28; _temp43->attributes= _temp33; _temp43;});}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp46; struct Cyc_List_List* _temp47; void* _temp49; struct Cyc_Absyn_Tqual
_temp51; struct Cyc_Core_Opt* _temp53; struct _tuple10* _temp44= field; _temp46=*
_temp44; _LL54: _temp53= _temp46.f1; goto _LL52; _LL52: _temp51= _temp46.f2;
goto _LL50; _LL50: _temp49= _temp46.f3; goto _LL48; _LL48: _temp47= _temp46.f4;
goto _LL45; _LL45: { struct Cyc_Core_Opt* _temp55= 0; if( _temp53 !=  0){ if(
Cyc_Absyn_is_qvar_qualified(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp53))->v)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), loc);} _temp55=({ struct Cyc_Core_Opt* _temp56=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp56->v=(
void*)(*(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp53))->v)).f2;
_temp56;});} if( _temp47 !=  0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("parameter should have no type parameters",
sizeof( unsigned char), 41u), loc);} return({ struct _tuple2* _temp57=( struct
_tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp57->f1= _temp55; _temp57->f2=
_temp51; _temp57->f3= _temp49; _temp57;});}} static void* Cyc_Parse_type_spec(
void* t, struct Cyc_Position_Segment* loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp58=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp58[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp59; _temp59.tag= Cyc_Parse_Type_spec;
_temp59.f1=( void*) t; _temp59.f2= loc; _temp59;}); _temp58;});} static void*
Cyc_Parse_array2ptr( void* t){ void* _temp60= t; struct Cyc_Absyn_Exp* _temp66;
struct Cyc_Absyn_Tqual _temp68; void* _temp70; _LL62: if(( unsigned int) _temp60
>  4u?*(( int*) _temp60) ==  Cyc_Absyn_ArrayType: 0){ _LL71: _temp70=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp60)->f1; goto _LL69; _LL69: _temp68=((
struct Cyc_Absyn_ArrayType_struct*) _temp60)->f2; goto _LL67; _LL67: _temp66=((
struct Cyc_Absyn_ArrayType_struct*) _temp60)->f3; goto _LL63;} else{ goto _LL64;}
_LL64: goto _LL65; _LL63: return Cyc_Absyn_starb_typ( _temp70,( void*) Cyc_Absyn_HeapRgn,
_temp68, _temp66 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp72=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp72[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp73; _temp73.tag= Cyc_Absyn_Upper_b;
_temp73.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp66); _temp73;}); _temp72;}));
_LL65: return t; _LL61:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment*
loc, struct _tuple2* t){ return({ struct _tuple11* _temp74=( struct _tuple11*)
_cycalloc( sizeof( struct _tuple11)); _temp74->f1=(* t).f2; _temp74->f2=(* t).f3;
_temp74;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_arr decl_kind;{ void* _temp75=( void*)
x->r; struct Cyc_Absyn_Vardecl* _temp101; _LL77: if(*(( int*) _temp75) ==  Cyc_Absyn_Var_d){
_LL102: _temp101=(( struct Cyc_Absyn_Var_d_struct*) _temp75)->f1; goto _LL78;}
else{ goto _LL79;} _LL79: if(*(( int*) _temp75) ==  Cyc_Absyn_Let_d){ goto _LL80;}
else{ goto _LL81;} _LL81: if(*(( int*) _temp75) ==  Cyc_Absyn_Letv_d){ goto
_LL82;} else{ goto _LL83;} _LL83: if(*(( int*) _temp75) ==  Cyc_Absyn_Fn_d){
goto _LL84;} else{ goto _LL85;} _LL85: if(*(( int*) _temp75) ==  Cyc_Absyn_Struct_d){
goto _LL86;} else{ goto _LL87;} _LL87: if(*(( int*) _temp75) ==  Cyc_Absyn_Union_d){
goto _LL88;} else{ goto _LL89;} _LL89: if(*(( int*) _temp75) ==  Cyc_Absyn_Tunion_d){
goto _LL90;} else{ goto _LL91;} _LL91: if(*(( int*) _temp75) ==  Cyc_Absyn_Typedef_d){
goto _LL92;} else{ goto _LL93;} _LL93: if(*(( int*) _temp75) ==  Cyc_Absyn_Enum_d){
goto _LL94;} else{ goto _LL95;} _LL95: if(*(( int*) _temp75) ==  Cyc_Absyn_Namespace_d){
goto _LL96;} else{ goto _LL97;} _LL97: if(*(( int*) _temp75) ==  Cyc_Absyn_Using_d){
goto _LL98;} else{ goto _LL99;} _LL99: if(*(( int*) _temp75) ==  Cyc_Absyn_ExternC_d){
goto _LL100;} else{ goto _LL76;} _LL78: if( _temp101->initializer !=  0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);} if( Cyc_Absyn_is_qvar_qualified( _temp101->name)){
Cyc_Parse_err( _tag_arr("namespaces not allowed on parameter declarations",
sizeof( unsigned char), 49u), x->loc);}{ int found= 0; for( 0; params !=  0;
params=(( struct Cyc_List_List*) _check_null( params))->tl){ if( Cyc_Std_zstrptrcmp((*
_temp101->name).f2,( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
params))->hd) ==  0){ found= 1; break;}} if( ! found){(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp104; _temp104.tag= Cyc_Std_String_pa;
_temp104.f1=( struct _tagged_arr)*(* _temp101->name).f2;{ void* _temp103[ 1u]={&
_temp104}; Cyc_Std_aprintf( _tag_arr("%s is not listed as a parameter", sizeof(
unsigned char), 32u), _tag_arr( _temp103, sizeof( void*), 1u));}}), x->loc);}
return;} _LL80: decl_kind= _tag_arr("let declaration", sizeof( unsigned char),
16u); goto _LL76; _LL82: decl_kind= _tag_arr("let declaration", sizeof(
unsigned char), 16u); goto _LL76; _LL84: decl_kind= _tag_arr("function declaration",
sizeof( unsigned char), 21u); goto _LL76; _LL86: decl_kind= _tag_arr("struct declaration",
sizeof( unsigned char), 19u); goto _LL76; _LL88: decl_kind= _tag_arr("union declaration",
sizeof( unsigned char), 18u); goto _LL76; _LL90: decl_kind= _tag_arr("tunion declaration",
sizeof( unsigned char), 19u); goto _LL76; _LL92: decl_kind= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL76; _LL94: decl_kind= _tag_arr("enum declaration",
sizeof( unsigned char), 17u); goto _LL76; _LL96: decl_kind= _tag_arr("namespace declaration",
sizeof( unsigned char), 22u); goto _LL76; _LL98: decl_kind= _tag_arr("using declaration",
sizeof( unsigned char), 18u); goto _LL76; _LL100: decl_kind= _tag_arr("extern C declaration",
sizeof( unsigned char), 21u); goto _LL76; _LL76:;}(( int(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp106; _temp106.tag= Cyc_Std_String_pa;
_temp106.f1=( struct _tagged_arr) decl_kind;{ void* _temp105[ 1u]={& _temp106};
Cyc_Std_aprintf( _tag_arr("%s appears in parameter type", sizeof( unsigned char),
29u), _tag_arr( _temp105, sizeof( void*), 1u));}}), x->loc); return;} struct
_tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_arr* x){ struct _tuple12 _temp109; struct Cyc_Position_Segment* _temp110;
struct Cyc_List_List* _temp112; struct _tuple12* _temp107= env; _temp109=*
_temp107; _LL113: _temp112= _temp109.f1; goto _LL111; _LL111: _temp110= _temp109.f2;
goto _LL108; _LL108: if( _temp112 ==  0){ return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp115; _temp115.tag= Cyc_Std_String_pa;
_temp115.f1=( struct _tagged_arr)* x;{ void* _temp114[ 1u]={& _temp115}; Cyc_Std_aprintf(
_tag_arr("missing type for parameter %s", sizeof( unsigned char), 30u), _tag_arr(
_temp114, sizeof( void*), 1u));}}), _temp110);}{ void* _temp116=( void*)((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp112))->hd)->r;
struct Cyc_Absyn_Vardecl* _temp122; _LL118: if(*(( int*) _temp116) ==  Cyc_Absyn_Var_d){
_LL123: _temp122=(( struct Cyc_Absyn_Var_d_struct*) _temp116)->f1; goto _LL119;}
else{ goto _LL120;} _LL120: goto _LL121; _LL119: if( Cyc_Absyn_is_qvar_qualified(
_temp122->name)){ Cyc_Parse_err( _tag_arr("namespace not allowed on parameter",
sizeof( unsigned char), 35u), _temp110);} if( Cyc_Std_zstrptrcmp((* _temp122->name).f2,
x) ==  0){ return({ struct _tuple2* _temp124=( struct _tuple2*) _cycalloc(
sizeof( struct _tuple2)); _temp124->f1=({ struct Cyc_Core_Opt* _temp125=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp125->v=( void*)(*
_temp122->name).f2; _temp125;}); _temp124->f2= _temp122->tq; _temp124->f3=( void*)
_temp122->type; _temp124;});} else{ return Cyc_Parse_get_param_type(({ struct
_tuple12* _temp126=( struct _tuple12*) _cycalloc( sizeof( struct _tuple12));
_temp126->f1=(( struct Cyc_List_List*) _check_null( _temp112))->tl; _temp126->f2=
_temp110; _temp126;}), x);} _LL121: return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration",
sizeof( unsigned char), 25u),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp112))->hd)->loc); _LL117:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp127= tm; _LL129: if(( unsigned int) _temp127 >  1u?*(( int*)
_temp127) ==  Cyc_Absyn_TypeParams_mod: 0){ goto _LL130;} else{ goto _LL131;}
_LL131: goto _LL132; _LL130: return 1; _LL132: return 0; _LL128:;} static void*
Cyc_Parse_id2type( struct _tagged_arr s, struct Cyc_Absyn_Conref* k){ if( Cyc_Std_zstrcmp(
s, _tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){ return( void*) Cyc_Absyn_HeapRgn;}
else{ return( void*)({ struct Cyc_Absyn_VarType_struct* _temp133=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp133[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp134; _temp134.tag= Cyc_Absyn_VarType; _temp134.f1=({ struct Cyc_Absyn_Tvar*
_temp135=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp135->name=({ struct _tagged_arr* _temp136=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp136[ 0]= s; _temp136;}); _temp135->identity=
0; _temp135->kind= k; _temp135;}); _temp134;}); _temp133;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp137=
t; struct Cyc_Absyn_Tvar* _temp143; _LL139: if(( unsigned int) _temp137 >  4u?*((
int*) _temp137) ==  Cyc_Absyn_VarType: 0){ _LL144: _temp143=(( struct Cyc_Absyn_VarType_struct*)
_temp137)->f1; goto _LL140;} else{ goto _LL141;} _LL141: goto _LL142; _LL140:
return _temp143; _LL142: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL138:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp145=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp145[ 0]=({ struct Cyc_Absyn_VarType_struct _temp146; _temp146.tag= Cyc_Absyn_VarType;
_temp146.f1= pr; _temp146;}); _temp145;});} static int Cyc_Parse_set_vartyp_kind(
void* t, void* k){ void* _temp147= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp153; struct Cyc_Absyn_Tvar _temp155; struct Cyc_Absyn_Conref* _temp156;
_LL149: if(( unsigned int) _temp147 >  4u?*(( int*) _temp147) ==  Cyc_Absyn_VarType:
0){ _LL154: _temp153=(( struct Cyc_Absyn_VarType_struct*) _temp147)->f1;
_temp155=* _temp153; _LL157: _temp156= _temp155.kind; goto _LL150;} else{ goto
_LL151;} _LL151: goto _LL152; _LL150: _temp156= Cyc_Absyn_compress_conref(
_temp156);{ void* _temp158=( void*) _temp156->v; void* _temp166; _LL160: if(
_temp158 == ( void*) Cyc_Absyn_No_constr){ goto _LL161;} else{ goto _LL162;}
_LL162: if(( unsigned int) _temp158 >  1u?*(( int*) _temp158) ==  Cyc_Absyn_Eq_constr:
0){ _LL167: _temp166=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp158)->f1;
goto _LL163;} else{ goto _LL164;} _LL164: goto _LL165; _LL161:( void*)( _temp156->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp168=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp168[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp169; _temp169.tag= Cyc_Absyn_Eq_constr; _temp169.f1=(
void*) k; _temp169;}); _temp168;}))); return 1; _LL163: return k ==  _temp166;
_LL165:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp170=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp170[ 0]=({ struct Cyc_Core_Impossible_struct _temp171; _temp171.tag= Cyc_Core_Impossible;
_temp171.f1= _tag_arr("forward after compress_conref", sizeof( unsigned char),
30u); _temp171;}); _temp170;})); _LL159:;} _LL152: return 0; _LL148:;} static
struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List* tms,
struct Cyc_List_List* tds, struct Cyc_Position_Segment* loc){ if( tds ==  0){
return tms;} if( tms ==  0){ return 0;}{ void* _temp172=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; void* _temp178; _LL174: if(( unsigned int) _temp172 >  1u?*((
int*) _temp172) ==  Cyc_Absyn_Function_mod: 0){ _LL179: _temp178=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp172)->f1; goto _LL175;} else{ goto
_LL176;} _LL176: goto _LL177; _LL175: if((( struct Cyc_List_List*) _check_null(
tms))->tl ==  0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
==  0: 0)){ void* _temp180= _temp178; struct Cyc_List_List* _temp186; _LL182:
if(*(( int*) _temp180) ==  Cyc_Absyn_WithTypes){ goto _LL183;} else{ goto _LL184;}
_LL184: if(*(( int*) _temp180) ==  Cyc_Absyn_NoTypes){ _LL187: _temp186=((
struct Cyc_Absyn_NoTypes_struct*) _temp180)->f1; goto _LL185;} else{ goto _LL181;}
_LL183: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL185:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp186,
tds);{ struct _tuple12* _temp188=({ struct _tuple12* _temp194=( struct _tuple12*)
_cycalloc( sizeof( struct _tuple12)); _temp194->f1= tds; _temp194->f2= loc;
_temp194;}); return({ struct Cyc_List_List* _temp189=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp189->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp190=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp190[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp191; _temp191.tag= Cyc_Absyn_Function_mod;
_temp191.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp192=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp192[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp193; _temp193.tag= Cyc_Absyn_WithTypes;
_temp193.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp188, _temp186); _temp193.f2= 0; _temp193.f3= 0;
_temp193.f4= 0; _temp193.f5= 0; _temp193;}); _temp192;})); _temp191;}); _temp190;}));
_temp189->tl= 0; _temp189;});} _LL181:;} else{ return({ struct Cyc_List_List*
_temp195=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp195->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp195->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp195;});} _LL177: return({ struct Cyc_List_List*
_temp196=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp196->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp196->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp196;}); _LL173:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds !=  0){ d=({ struct Cyc_Parse_Declarator* _temp197=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp197->id= d->id; _temp197->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp197;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso !=  0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc !=  0){ switch(( enum  Cyc_Parse_Storage_class)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v){ case Cyc_Parse_Extern_sc:
_LL198: sc=( void*) Cyc_Absyn_Extern; break; case Cyc_Parse_ExternC_sc: _LL199:
sc=( void*) Cyc_Absyn_ExternC; break; case Cyc_Parse_Static_sc: _LL200: sc=(
void*) Cyc_Absyn_Static; break; default: _LL201: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); break;}}}{ struct Cyc_Core_Opt* _temp205;
void* _temp207; struct _tuple5 _temp203= Cyc_Parse_collapse_type_specifiers( tss,
loc); _LL208: _temp207= _temp203.f1; goto _LL206; _LL206: _temp205= _temp203.f2;
goto _LL204; _LL204: { struct Cyc_List_List* _temp211; struct Cyc_List_List*
_temp213; void* _temp215; struct Cyc_Absyn_Tqual _temp217; struct _tuple6
_temp209= Cyc_Parse_apply_tms( tq, _temp207, atts, d->tms); _LL218: _temp217=
_temp209.f1; goto _LL216; _LL216: _temp215= _temp209.f2; goto _LL214; _LL214:
_temp213= _temp209.f3; goto _LL212; _LL212: _temp211= _temp209.f4; goto _LL210;
_LL210: if( _temp205 !=  0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp213 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp219= _temp215; struct Cyc_Absyn_FnInfo _temp225; struct Cyc_List_List*
_temp227; struct Cyc_List_List* _temp229; struct Cyc_Absyn_VarargInfo* _temp231;
int _temp233; struct Cyc_List_List* _temp235; void* _temp237; struct Cyc_Core_Opt*
_temp239; struct Cyc_List_List* _temp241; _LL221: if(( unsigned int) _temp219 > 
4u?*(( int*) _temp219) ==  Cyc_Absyn_FnType: 0){ _LL226: _temp225=(( struct Cyc_Absyn_FnType_struct*)
_temp219)->f1; _LL242: _temp241= _temp225.tvars; goto _LL240; _LL240: _temp239=
_temp225.effect; goto _LL238; _LL238: _temp237=( void*) _temp225.ret_typ; goto
_LL236; _LL236: _temp235= _temp225.args; goto _LL234; _LL234: _temp233= _temp225.c_varargs;
goto _LL232; _LL232: _temp231= _temp225.cyc_varargs; goto _LL230; _LL230:
_temp229= _temp225.rgn_po; goto _LL228; _LL228: _temp227= _temp225.attributes;
goto _LL222;} else{ goto _LL223;} _LL223: goto _LL224; _LL222: { struct Cyc_List_List*
_temp243=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp235); return({ struct Cyc_Absyn_Fndecl*
_temp244=( struct Cyc_Absyn_Fndecl*) _cycalloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp244->sc=( void*) sc; _temp244->name= d->id; _temp244->tvs= _temp241;
_temp244->is_inline= is_inline; _temp244->effect= _temp239; _temp244->ret_type=(
void*) _temp237; _temp244->args= _temp243; _temp244->c_varargs= _temp233;
_temp244->cyc_varargs= _temp231; _temp244->rgn_po= _temp229; _temp244->body=
body; _temp244->cached_typ= 0; _temp244->param_vardecls= 0; _temp244->attributes=
Cyc_List_append( _temp227, _temp211); _temp244;});} _LL224: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL220:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 ==  0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp245=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp245->f1= _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"?", sizeof( unsigned char),
2u); _temp245->f2=(* t).f2; _temp245->f3=(* t).f3; _temp245;});} else{ return({
struct _tuple4* _temp246=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp246->f1=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp246->f2=(* t).f2; _temp246->f3=(* t).f3; _temp246;});}} static
unsigned char _temp247[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp247, _temp247, _temp247 +  52u};
static unsigned char _temp248[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp248, _temp248, _temp248 +  63u};
static unsigned char _temp249[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp249, _temp249, _temp249 +  50u};
static unsigned char _temp250[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp250, _temp250, _temp250 +  60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts !=  0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp251=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp265; void* _temp267; struct Cyc_Position_Segment*
_temp269; struct Cyc_Position_Segment* _temp271; struct Cyc_Position_Segment*
_temp273; struct Cyc_Position_Segment* _temp275; struct Cyc_Absyn_Decl* _temp277;
_LL253: if(*(( int*) _temp251) ==  Cyc_Parse_Type_spec){ _LL268: _temp267=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp251)->f1; goto _LL266; _LL266: _temp265=((
struct Cyc_Parse_Type_spec_struct*) _temp251)->f2; goto _LL254;} else{ goto
_LL255;} _LL255: if(*(( int*) _temp251) ==  Cyc_Parse_Signed_spec){ _LL270:
_temp269=(( struct Cyc_Parse_Signed_spec_struct*) _temp251)->f1; goto _LL256;}
else{ goto _LL257;} _LL257: if(*(( int*) _temp251) ==  Cyc_Parse_Unsigned_spec){
_LL272: _temp271=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp251)->f1; goto
_LL258;} else{ goto _LL259;} _LL259: if(*(( int*) _temp251) ==  Cyc_Parse_Short_spec){
_LL274: _temp273=(( struct Cyc_Parse_Short_spec_struct*) _temp251)->f1; goto
_LL260;} else{ goto _LL261;} _LL261: if(*(( int*) _temp251) ==  Cyc_Parse_Long_spec){
_LL276: _temp275=(( struct Cyc_Parse_Long_spec_struct*) _temp251)->f1; goto
_LL262;} else{ goto _LL263;} _LL263: if(*(( int*) _temp251) ==  Cyc_Parse_Decl_spec){
_LL278: _temp277=(( struct Cyc_Parse_Decl_spec_struct*) _temp251)->f1; goto
_LL264;} else{ goto _LL252;} _LL254: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp265);} last_loc= _temp265; seen_type= 1; t= _temp267; goto _LL252; _LL256:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp269);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp269);} last_loc= _temp269; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL252; _LL258: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp271);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp271);} last_loc= _temp271; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL252; _LL260: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp273);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp273);} last_loc= _temp273; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL252; _LL262: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp275);} if( seen_size){ void* _temp279= sz; _LL281: if( _temp279 == ( void*)
Cyc_Absyn_B4){ goto _LL282;} else{ goto _LL283;} _LL283: goto _LL284; _LL282: sz=(
void*) Cyc_Absyn_B8; goto _LL280; _LL284: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp275); goto _LL280; _LL280:;} last_loc=
_temp275; seen_size= 1; goto _LL252; _LL264: last_loc= _temp277->loc; if(
declopt ==  0? ! seen_type: 0){ seen_type= 1;{ void* _temp285=( void*) _temp277->r;
struct Cyc_Absyn_Structdecl* _temp297; struct Cyc_Absyn_Tuniondecl* _temp299;
struct Cyc_Absyn_Uniondecl* _temp301; struct Cyc_Absyn_Enumdecl* _temp303;
_LL287: if(*(( int*) _temp285) ==  Cyc_Absyn_Struct_d){ _LL298: _temp297=((
struct Cyc_Absyn_Struct_d_struct*) _temp285)->f1; goto _LL288;} else{ goto
_LL289;} _LL289: if(*(( int*) _temp285) ==  Cyc_Absyn_Tunion_d){ _LL300:
_temp299=(( struct Cyc_Absyn_Tunion_d_struct*) _temp285)->f1; goto _LL290;}
else{ goto _LL291;} _LL291: if(*(( int*) _temp285) ==  Cyc_Absyn_Union_d){
_LL302: _temp301=(( struct Cyc_Absyn_Union_d_struct*) _temp285)->f1; goto _LL292;}
else{ goto _LL293;} _LL293: if(*(( int*) _temp285) ==  Cyc_Absyn_Enum_d){ _LL304:
_temp303=(( struct Cyc_Absyn_Enum_d_struct*) _temp285)->f1; goto _LL294;} else{
goto _LL295;} _LL295: goto _LL296; _LL288: { struct Cyc_List_List* _temp305=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp297->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp306=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp306[ 0]=({ struct Cyc_Absyn_StructType_struct _temp307; _temp307.tag= Cyc_Absyn_StructType;
_temp307.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp297->name))->v); _temp307.f2= _temp305; _temp307.f3= 0;
_temp307;}); _temp306;}); if( _temp297->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp308=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp308->v=( void*) _temp277; _temp308;});} goto _LL286;} _LL290: { struct Cyc_List_List*
_temp309=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp299->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp310=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp310[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp311; _temp311.tag= Cyc_Absyn_TunionType;
_temp311.f1=({ struct Cyc_Absyn_TunionInfo _temp312; _temp312.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp313=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp313[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp314; _temp314.tag= Cyc_Absyn_KnownTunion;
_temp314.f1= _temp299; _temp314;}); _temp313;})); _temp312.targs= _temp309;
_temp312.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp312;}); _temp311;});
_temp310;}); if( _temp299->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp315=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp315->v=( void*) _temp277; _temp315;});} goto _LL286;} _LL292: { struct Cyc_List_List*
_temp316=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp301->tvs); t=(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp317=( struct Cyc_Absyn_UnionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp317[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp318; _temp318.tag= Cyc_Absyn_UnionType; _temp318.f1=(
struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp301->name))->v); _temp318.f2= _temp316; _temp318.f3= 0; _temp318;});
_temp317;}); if( _temp301->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp319=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp319->v=( void*) _temp277; _temp319;});} goto _LL286;} _LL294: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp320=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp320[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp321; _temp321.tag= Cyc_Absyn_EnumType; _temp321.f1=
_temp303->name; _temp321.f2= 0; _temp321;}); _temp320;}); declopt=({ struct Cyc_Core_Opt*
_temp322=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp322->v=( void*) _temp277; _temp322;}); goto _LL286; _LL296:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp277->loc); goto _LL286; _LL286:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp277->loc);} goto _LL252; _LL252:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp323=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp323[ 0]=({ struct Cyc_Absyn_IntType_struct _temp324; _temp324.tag= Cyc_Absyn_IntType;
_temp324.f1=( void*) sgn; _temp324.f2=( void*) sz; _temp324;}); _temp323;});}
else{ if( seen_sign){ void* _temp325= t; void* _temp331; _LL327: if((
unsigned int) _temp325 >  4u?*(( int*) _temp325) ==  Cyc_Absyn_IntType: 0){
_LL332: _temp331=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp325)->f2;
goto _LL328;} else{ goto _LL329;} _LL329: goto _LL330; _LL328: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp333=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp333[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp334; _temp334.tag= Cyc_Absyn_IntType; _temp334.f1=( void*) sgn; _temp334.f2=(
void*) _temp331; _temp334;}); _temp333;}); goto _LL326; _LL330: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL326; _LL326:;} if( seen_size){ void* _temp335= t; void*
_temp341; _LL337: if(( unsigned int) _temp335 >  4u?*(( int*) _temp335) ==  Cyc_Absyn_IntType:
0){ _LL342: _temp341=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp335)->f1;
goto _LL338;} else{ goto _LL339;} _LL339: goto _LL340; _LL338: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp343=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp343[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp344; _temp344.tag= Cyc_Absyn_IntType; _temp344.f1=( void*) _temp341;
_temp344.f2=( void*) sz; _temp344;}); _temp343;}); goto _LL336; _LL340: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL336; _LL336:;}} return({ struct _tuple5 _temp345; _temp345.f1=
t; _temp345.f2= declopt; _temp345;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds ==  0){ return 0;}{ struct Cyc_Parse_Declarator* _temp346=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp347= _temp346->id; struct Cyc_List_List* _temp350; struct
Cyc_List_List* _temp352; void* _temp354; struct Cyc_Absyn_Tqual _temp356; struct
_tuple6 _temp348= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp346->tms);
_LL357: _temp356= _temp348.f1; goto _LL355; _LL355: _temp354= _temp348.f2; goto
_LL353; _LL353: _temp352= _temp348.f3; goto _LL351; _LL351: _temp350= _temp348.f4;
goto _LL349; _LL349: return({ struct Cyc_List_List* _temp358=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp358->hd=( void*)({ struct
_tuple7* _temp359=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7));
_temp359->f1= _temp347; _temp359->f2= _temp356; _temp359->f3= _temp354; _temp359->f4=
_temp352; _temp359->f5= _temp350; _temp359;}); _temp358->tl= Cyc_Parse_apply_tmss(
_temp356, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp358;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms ==  0){ return({ struct _tuple6 _temp360; _temp360.f1= tq;
_temp360.f2= t; _temp360.f3= 0; _temp360.f4= atts; _temp360;});}{ void* _temp361=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp*
_temp375; void* _temp377; struct Cyc_Position_Segment* _temp379; struct Cyc_List_List*
_temp381; struct Cyc_Absyn_Tqual _temp383; void* _temp385; void* _temp387;
struct Cyc_List_List* _temp389; struct Cyc_Position_Segment* _temp391; _LL363:
if( _temp361 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL364;} else{ goto _LL365;}
_LL365: if(( unsigned int) _temp361 >  1u?*(( int*) _temp361) ==  Cyc_Absyn_ConstArray_mod:
0){ _LL376: _temp375=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp361)->f1;
goto _LL366;} else{ goto _LL367;} _LL367: if(( unsigned int) _temp361 >  1u?*((
int*) _temp361) ==  Cyc_Absyn_Function_mod: 0){ _LL378: _temp377=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp361)->f1; goto _LL368;} else{ goto
_LL369;} _LL369: if(( unsigned int) _temp361 >  1u?*(( int*) _temp361) ==  Cyc_Absyn_TypeParams_mod:
0){ _LL382: _temp381=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp361)->f1;
goto _LL380; _LL380: _temp379=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp361)->f2; goto _LL370;} else{ goto _LL371;} _LL371: if(( unsigned int)
_temp361 >  1u?*(( int*) _temp361) ==  Cyc_Absyn_Pointer_mod: 0){ _LL388:
_temp387=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp361)->f1; goto
_LL386; _LL386: _temp385=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp361)->f2; goto _LL384; _LL384: _temp383=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp361)->f3; goto _LL372;} else{ goto _LL373;} _LL373: if(( unsigned int)
_temp361 >  1u?*(( int*) _temp361) ==  Cyc_Absyn_Attributes_mod: 0){ _LL392:
_temp391=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp361)->f1; goto _LL390;
_LL390: _temp389=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp361)->f2; goto
_LL374;} else{ goto _LL362;} _LL364: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp393=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp393[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp394; _temp394.tag= Cyc_Absyn_ArrayType; _temp394.f1=(
void*) t; _temp394.f2= tq; _temp394.f3= 0; _temp394;}); _temp393;}), atts,((
struct Cyc_List_List*) _check_null( tms))->tl); _LL366: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp395=(
struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp395[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp396; _temp396.tag= Cyc_Absyn_ArrayType;
_temp396.f1=( void*) t; _temp396.f2= tq; _temp396.f3=( struct Cyc_Absyn_Exp*)
_temp375; _temp396;}); _temp395;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL368: { void* _temp397= _temp377; struct Cyc_List_List* _temp403;
struct Cyc_Core_Opt* _temp405; struct Cyc_Absyn_VarargInfo* _temp407; int
_temp409; struct Cyc_List_List* _temp411; struct Cyc_Position_Segment* _temp413;
_LL399: if(*(( int*) _temp397) ==  Cyc_Absyn_WithTypes){ _LL412: _temp411=((
struct Cyc_Absyn_WithTypes_struct*) _temp397)->f1; goto _LL410; _LL410: _temp409=((
struct Cyc_Absyn_WithTypes_struct*) _temp397)->f2; goto _LL408; _LL408: _temp407=((
struct Cyc_Absyn_WithTypes_struct*) _temp397)->f3; goto _LL406; _LL406: _temp405=((
struct Cyc_Absyn_WithTypes_struct*) _temp397)->f4; goto _LL404; _LL404: _temp403=((
struct Cyc_Absyn_WithTypes_struct*) _temp397)->f5; goto _LL400;} else{ goto
_LL401;} _LL401: if(*(( int*) _temp397) ==  Cyc_Absyn_NoTypes){ _LL414: _temp413=((
struct Cyc_Absyn_NoTypes_struct*) _temp397)->f2; goto _LL402;} else{ goto _LL398;}
_LL400: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!=  0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp415=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp415->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp415->tl= fn_atts; _temp415;});} else{ new_atts=({ struct Cyc_List_List*
_temp416=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp416->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp416->tl= new_atts; _temp416;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl !=  0){ void* _temp417=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_List_List*
_temp423; _LL419: if(( unsigned int) _temp417 >  1u?*(( int*) _temp417) ==  Cyc_Absyn_TypeParams_mod:
0){ _LL424: _temp423=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp417)->f1;
goto _LL420;} else{ goto _LL421;} _LL421: goto _LL422; _LL420: typvars= _temp423;
tms=(( struct Cyc_List_List*) _check_null( tms))->tl; goto _LL418; _LL422: goto
_LL418; _LL418:;} if((((( ! _temp409? _temp407 ==  0: 0)? _temp411 !=  0: 0)?((
struct Cyc_List_List*) _check_null( _temp411))->tl ==  0: 0)?(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp411))->hd)).f1 ==  0: 0)?(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( _temp411))->hd)).f3 == ( void*)
Cyc_Absyn_VoidType: 0){ _temp411= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)(
void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp411); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp405, t, _temp411, _temp409, _temp407, _temp403, fn_atts), new_atts,((
struct Cyc_List_List*) _check_null( tms))->tl);} _LL402:( int) _throw( Cyc_Parse_abort(
_tag_arr("function declaration without parameter types", sizeof( unsigned char),
45u), _temp413)); _LL398:;} _LL370: if((( struct Cyc_List_List*) _check_null(
tms))->tl ==  0){ return({ struct _tuple6 _temp425; _temp425.f1= tq; _temp425.f2=
t; _temp425.f3= _temp381; _temp425.f4= atts; _temp425;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp379)); _LL372: { void* _temp426= _temp387;
struct Cyc_Absyn_Exp* _temp434; struct Cyc_Absyn_Exp* _temp436; _LL428: if((
unsigned int) _temp426 >  1u?*(( int*) _temp426) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL435: _temp434=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp426)->f1; goto
_LL429;} else{ goto _LL430;} _LL430: if(( unsigned int) _temp426 >  1u?*(( int*)
_temp426) ==  Cyc_Absyn_Nullable_ps: 0){ _LL437: _temp436=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp426)->f1; goto _LL431;} else{ goto _LL432;} _LL432: if( _temp426 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL433;} else{ goto _LL427;} _LL429: return Cyc_Parse_apply_tms(
_temp383, Cyc_Absyn_atb_typ( t, _temp385, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp438=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp438[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp439; _temp439.tag= Cyc_Absyn_Upper_b;
_temp439.f1= _temp434; _temp439;}); _temp438;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL431: return Cyc_Parse_apply_tms( _temp383, Cyc_Absyn_starb_typ(
t, _temp385, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp440=( struct
Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp440[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp441; _temp441.tag= Cyc_Absyn_Upper_b;
_temp441.f1= _temp436; _temp441;}); _temp440;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL433: return Cyc_Parse_apply_tms( _temp383, Cyc_Absyn_tagged_typ(
t, _temp385, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL427:;}
_LL374: return Cyc_Parse_apply_tms( tq, t, Cyc_List_append( atts, _temp389),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL362:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp444; void* _temp446; struct _tuple5 _temp442= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL447: _temp446= _temp442.f1; goto _LL445; _LL445: _temp444=
_temp442.f2; goto _LL443; _LL443: if( _temp444 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp446;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp448=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp448[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp449; _temp449.tag= Cyc_Absyn_Decl_s;
_temp449.f1= d; _temp449.f2= s; _temp449;}); _temp448;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err( _tag_arr("inline is only allowed on function definitions",
sizeof( unsigned char), 47u), loc);} if( tss ==  0){ Cyc_Parse_err( _tag_arr("missing type specifiers in declaration",
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc !=  0){ switch(( enum 
Cyc_Parse_Storage_class)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v){ case
Cyc_Parse_Typedef_sc: _LL450: istypedef= 1; break; case Cyc_Parse_Extern_sc:
_LL451: s=( void*) Cyc_Absyn_Extern; break; case Cyc_Parse_ExternC_sc: _LL452: s=(
void*) Cyc_Absyn_ExternC; break; case Cyc_Parse_Static_sc: _LL453: s=( void*)
Cyc_Absyn_Static; break; case Cyc_Parse_Auto_sc: _LL454: s=( void*) Cyc_Absyn_Public;
break; case Cyc_Parse_Register_sc: _LL455: s=( void*) Cyc_Absyn_Public; break;
case Cyc_Parse_Abstract_sc: _LL456: s=( void*) Cyc_Absyn_Abstract; break;}}{
struct Cyc_List_List* _temp460; struct Cyc_List_List* _temp462; struct _tuple0
_temp458=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( ids);
_LL463: _temp462= _temp458.f1; goto _LL461; _LL461: _temp460= _temp458.f2; goto
_LL459; _LL459: { int exps_empty= 1;{ struct Cyc_List_List* es= _temp460; for( 0;
es !=  0; es=(( struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd !=  0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp464= Cyc_Parse_collapse_type_specifiers( tss, loc); if(
_temp462 ==  0){ struct Cyc_Core_Opt* _temp467; void* _temp469; struct _tuple5
_temp465= _temp464; _LL470: _temp469= _temp465.f1; goto _LL468; _LL468: _temp467=
_temp465.f2; goto _LL466; _LL466: if( _temp467 !=  0){ struct Cyc_Absyn_Decl* d=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null( _temp467))->v;{
void* _temp471=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp483; struct Cyc_Absyn_Structdecl*
_temp485; struct Cyc_Absyn_Uniondecl* _temp487; struct Cyc_Absyn_Tuniondecl*
_temp489; _LL473: if(*(( int*) _temp471) ==  Cyc_Absyn_Enum_d){ _LL484: _temp483=((
struct Cyc_Absyn_Enum_d_struct*) _temp471)->f1; goto _LL474;} else{ goto _LL475;}
_LL475: if(*(( int*) _temp471) ==  Cyc_Absyn_Struct_d){ _LL486: _temp485=((
struct Cyc_Absyn_Struct_d_struct*) _temp471)->f1; goto _LL476;} else{ goto
_LL477;} _LL477: if(*(( int*) _temp471) ==  Cyc_Absyn_Union_d){ _LL488: _temp487=((
struct Cyc_Absyn_Union_d_struct*) _temp471)->f1; goto _LL478;} else{ goto _LL479;}
_LL479: if(*(( int*) _temp471) ==  Cyc_Absyn_Tunion_d){ _LL490: _temp489=((
struct Cyc_Absyn_Tunion_d_struct*) _temp471)->f1; goto _LL480;} else{ goto
_LL481;} _LL481: goto _LL482; _LL474:( void*)( _temp483->sc=( void*) s); if(
atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} goto _LL472; _LL476:( void*)( _temp485->sc=( void*)
s); _temp485->attributes= atts; goto _LL472; _LL478:( void*)( _temp487->sc=(
void*) s); _temp487->attributes= atts; goto _LL472; _LL480:( void*)( _temp489->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL472; _LL482: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL472:;}
return({ struct Cyc_List_List* _temp491=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp491->hd=( void*) d; _temp491->tl= 0;
_temp491;});} else{ void* _temp492= _temp469; struct Cyc_List_List* _temp506;
struct _tuple1* _temp508; struct Cyc_Absyn_TunionInfo _temp510; void* _temp512;
struct Cyc_Absyn_Tuniondecl* _temp514; struct Cyc_Absyn_TunionInfo _temp516;
struct Cyc_List_List* _temp518; void* _temp520; struct Cyc_Absyn_UnknownTunionInfo
_temp522; int _temp524; struct _tuple1* _temp526; struct Cyc_List_List* _temp528;
struct _tuple1* _temp530; struct _tuple1* _temp532; _LL494: if(( unsigned int)
_temp492 >  4u?*(( int*) _temp492) ==  Cyc_Absyn_StructType: 0){ _LL509:
_temp508=(( struct Cyc_Absyn_StructType_struct*) _temp492)->f1; goto _LL507;
_LL507: _temp506=(( struct Cyc_Absyn_StructType_struct*) _temp492)->f2; goto
_LL495;} else{ goto _LL496;} _LL496: if(( unsigned int) _temp492 >  4u?*(( int*)
_temp492) ==  Cyc_Absyn_TunionType: 0){ _LL511: _temp510=(( struct Cyc_Absyn_TunionType_struct*)
_temp492)->f1; _LL513: _temp512=( void*) _temp510.tunion_info; if(*(( int*)
_temp512) ==  Cyc_Absyn_KnownTunion){ _LL515: _temp514=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp512)->f1; goto _LL497;} else{ goto _LL498;}} else{ goto _LL498;} _LL498:
if(( unsigned int) _temp492 >  4u?*(( int*) _temp492) ==  Cyc_Absyn_TunionType:
0){ _LL517: _temp516=(( struct Cyc_Absyn_TunionType_struct*) _temp492)->f1;
_LL521: _temp520=( void*) _temp516.tunion_info; if(*(( int*) _temp520) ==  Cyc_Absyn_UnknownTunion){
_LL523: _temp522=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp520)->f1;
_LL527: _temp526= _temp522.name; goto _LL525; _LL525: _temp524= _temp522.is_xtunion;
goto _LL519;} else{ goto _LL500;} _LL519: _temp518= _temp516.targs; goto _LL499;}
else{ goto _LL500;} _LL500: if(( unsigned int) _temp492 >  4u?*(( int*) _temp492)
==  Cyc_Absyn_UnionType: 0){ _LL531: _temp530=(( struct Cyc_Absyn_UnionType_struct*)
_temp492)->f1; goto _LL529; _LL529: _temp528=(( struct Cyc_Absyn_UnionType_struct*)
_temp492)->f2; goto _LL501;} else{ goto _LL502;} _LL502: if(( unsigned int)
_temp492 >  4u?*(( int*) _temp492) ==  Cyc_Absyn_EnumType: 0){ _LL533: _temp532=((
struct Cyc_Absyn_EnumType_struct*) _temp492)->f1; goto _LL503;} else{ goto
_LL504;} _LL504: goto _LL505; _LL495: { struct Cyc_List_List* _temp534=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp506); struct Cyc_Absyn_Structdecl* _temp535=({
struct Cyc_Absyn_Structdecl* _temp539=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp539->sc=( void*) s; _temp539->name=({
struct Cyc_Core_Opt* _temp540=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp540->v=( void*)(( struct _tuple1*) _check_null( _temp508));
_temp540;}); _temp539->tvs= _temp534; _temp539->fields= 0; _temp539->attributes=
0; _temp539;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp536=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp536->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp537=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp537[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp538; _temp538.tag= Cyc_Absyn_Struct_d;
_temp538.f1= _temp535; _temp538;}); _temp537;}), loc); _temp536->tl= 0; _temp536;});}
_LL497: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp541=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp541->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp542=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp542[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp543; _temp543.tag= Cyc_Absyn_Tunion_d;
_temp543.f1= _temp514; _temp543;}); _temp542;}), loc); _temp541->tl= 0; _temp541;});
_LL499: { struct Cyc_List_List* _temp544=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp518);
struct Cyc_Absyn_Decl* _temp545= Cyc_Absyn_tunion_decl( s, _temp526, _temp544, 0,
_temp524, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp546=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp546->hd=(
void*) _temp545; _temp546->tl= 0; _temp546;});} _LL501: { struct Cyc_List_List*
_temp547=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp528); struct Cyc_Absyn_Uniondecl* _temp548=({
struct Cyc_Absyn_Uniondecl* _temp553=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp553->sc=( void*) s; _temp553->name=({
struct Cyc_Core_Opt* _temp554=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp554->v=( void*)(( struct _tuple1*) _check_null( _temp530));
_temp554;}); _temp553->tvs= _temp547; _temp553->fields= 0; _temp553->attributes=
0; _temp553;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp549=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp549->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp550=( struct Cyc_Absyn_Decl*) _cycalloc(
sizeof( struct Cyc_Absyn_Decl)); _temp550->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp551=( struct Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp551[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp552; _temp552.tag= Cyc_Absyn_Union_d;
_temp552.f1= _temp548; _temp552;}); _temp551;})); _temp550->loc= loc; _temp550;});
_temp549->tl= 0; _temp549;});} _LL503: { struct Cyc_Absyn_Enumdecl* _temp555=({
struct Cyc_Absyn_Enumdecl* _temp560=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp560->sc=( void*) s; _temp560->name=
_temp532; _temp560->fields= 0; _temp560;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp556=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp556->hd=( void*)({ struct Cyc_Absyn_Decl* _temp557=(
struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp557->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp558=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp558[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp559; _temp559.tag= Cyc_Absyn_Enum_d; _temp559.f1= _temp555; _temp559;});
_temp558;})); _temp557->loc= loc; _temp557;}); _temp556->tl= 0; _temp556;});}
_LL505: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL493:;}} else{ void* t= _temp464.f1; struct Cyc_List_List*
_temp561= Cyc_Parse_apply_tmss( tq, t, _temp462, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp561); if( _temp464.f2 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp464.f2))->v;{ void* _temp562=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp574; struct Cyc_Absyn_Tuniondecl* _temp576;
struct Cyc_Absyn_Uniondecl* _temp578; struct Cyc_Absyn_Enumdecl* _temp580;
_LL564: if(*(( int*) _temp562) ==  Cyc_Absyn_Struct_d){ _LL575: _temp574=((
struct Cyc_Absyn_Struct_d_struct*) _temp562)->f1; goto _LL565;} else{ goto
_LL566;} _LL566: if(*(( int*) _temp562) ==  Cyc_Absyn_Tunion_d){ _LL577:
_temp576=(( struct Cyc_Absyn_Tunion_d_struct*) _temp562)->f1; goto _LL567;}
else{ goto _LL568;} _LL568: if(*(( int*) _temp562) ==  Cyc_Absyn_Union_d){
_LL579: _temp578=(( struct Cyc_Absyn_Union_d_struct*) _temp562)->f1; goto _LL569;}
else{ goto _LL570;} _LL570: if(*(( int*) _temp562) ==  Cyc_Absyn_Enum_d){ _LL581:
_temp580=(( struct Cyc_Absyn_Enum_d_struct*) _temp562)->f1; goto _LL571;} else{
goto _LL572;} _LL572: goto _LL573; _LL565:( void*)( _temp574->sc=( void*) s);
_temp574->attributes= atts; atts= 0; goto _LL563; _LL567:( void*)( _temp576->sc=(
void*) s); goto _LL563; _LL569:( void*)( _temp578->sc=( void*) s); goto _LL563;
_LL571:( void*)( _temp580->sc=( void*) s); goto _LL563; _LL573: Cyc_Parse_err(
_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL563; _LL563:;} decls=({ struct Cyc_List_List*
_temp582=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp582->hd=( void*) d; _temp582->tl= decls; _temp582;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp584; _temp584.tag=
Cyc_Std_String_pa; _temp584.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp583[ 1u]={&
_temp584}; Cyc_Std_aprintf( _tag_arr("bad attribute %s in typedef", sizeof(
unsigned char), 28u), _tag_arr( _temp583, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp464.f2 !=  0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp585= _temp561; for( 0; _temp585 !=  0;( _temp585=(( struct
Cyc_List_List*) _check_null( _temp585))->tl, _temp460=(( struct Cyc_List_List*)
_check_null( _temp460))->tl)){ struct _tuple7 _temp588; struct Cyc_List_List*
_temp589; struct Cyc_List_List* _temp591; void* _temp593; struct Cyc_Absyn_Tqual
_temp595; struct _tuple1* _temp597; struct _tuple7* _temp586=( struct _tuple7*)((
struct Cyc_List_List*) _check_null( _temp585))->hd; _temp588=* _temp586; _LL598:
_temp597= _temp588.f1; goto _LL596; _LL596: _temp595= _temp588.f2; goto _LL594;
_LL594: _temp593= _temp588.f3; goto _LL592; _LL592: _temp591= _temp588.f4; goto
_LL590; _LL590: _temp589= _temp588.f5; goto _LL587; _LL587: if( _temp591 !=  0){
Cyc_Parse_warn( _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
loc);} if( _temp460 ==  0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected NULL in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp599=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp460))->hd; struct Cyc_Absyn_Vardecl*
_temp600= Cyc_Absyn_new_vardecl( _temp597, _temp593, _temp599); _temp600->tq=
_temp595;( void*)( _temp600->sc=( void*) s); _temp600->attributes= _temp589;{
struct Cyc_Absyn_Decl* _temp601=({ struct Cyc_Absyn_Decl* _temp603=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp603->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp604=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp604[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp605; _temp605.tag= Cyc_Absyn_Var_d; _temp605.f1= _temp600; _temp605;});
_temp604;})); _temp603->loc= loc; _temp603;}); decls=({ struct Cyc_List_List*
_temp602=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp602->hd=( void*) _temp601; _temp602->tl= decls; _temp602;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_Std_strlen( s) !=  1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp607; _temp607.tag= Cyc_Std_String_pa;
_temp607.f1=( struct _tagged_arr) s;{ void* _temp606[ 1u]={& _temp607}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp606,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL608: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL609: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL610: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL611: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL612: return(
void*) Cyc_Absyn_EffKind; default: _LL613: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp616; _temp616.tag= Cyc_Std_String_pa;
_temp616.f1=( struct _tagged_arr) s;{ void* _temp615[ 1u]={& _temp616}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp615,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts ==  0){ return
tms;} else{ return({ struct Cyc_List_List* _temp617=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp617->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp618=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp618[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp619; _temp619.tag= Cyc_Absyn_Attributes_mod;
_temp619.f1= loc; _temp619.f2= atts; _temp619;}); _temp618;})); _temp617->tl=
tms; _temp617;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp622;
struct Cyc_List_List* _temp623; struct Cyc_List_List* _temp625; void* _temp627;
struct Cyc_Absyn_Tqual _temp629; struct _tuple1* _temp631; struct _tuple7*
_temp620= t; _temp622=* _temp620; _LL632: _temp631= _temp622.f1; goto _LL630;
_LL630: _temp629= _temp622.f2; goto _LL628; _LL628: _temp627= _temp622.f3; goto
_LL626; _LL626: _temp625= _temp622.f4; goto _LL624; _LL624: _temp623= _temp622.f5;
goto _LL621; _LL621: Cyc_Lex_register_typedef( _temp631); if( _temp623 !=  0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp634;
_temp634.tag= Cyc_Std_String_pa; _temp634.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp623))->hd);{ void* _temp633[ 1u]={&
_temp634}; Cyc_Std_aprintf( _tag_arr("bad attribute %s within typedef", sizeof(
unsigned char), 32u), _tag_arr( _temp633, sizeof( void*), 1u));}}), loc);}
return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp635=(
struct Cyc_Absyn_Typedef_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp635[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp636; _temp636.tag= Cyc_Absyn_Typedef_d;
_temp636.f1=({ struct Cyc_Absyn_Typedefdecl* _temp637=( struct Cyc_Absyn_Typedefdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp637->name= _temp631;
_temp637->tvs= _temp625; _temp637->defn=( void*) _temp627; _temp637;}); _temp636;});
_temp635;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{ unsigned char* tag; struct Cyc_Parse_Abstractdeclarator*
f1; } ; unsigned char Cyc_AttributeList_tok[ 22u]="\000\000\000\000AttributeList_tok";
struct Cyc_AttributeList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Attribute_tok[ 18u]="\000\000\000\000Attribute_tok";
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char
Cyc_Bool_tok[ 13u]="\000\000\000\000Bool_tok"; struct Cyc_Bool_tok_struct{
unsigned char* tag; int f1; } ; unsigned char Cyc_Char_tok[ 13u]="\000\000\000\000Char_tok";
struct Cyc_Char_tok_struct{ unsigned char* tag; unsigned char f1; } ;
unsigned char Cyc_DeclList_tok[ 17u]="\000\000\000\000DeclList_tok"; struct Cyc_DeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_DeclSpec_tok[
17u]="\000\000\000\000DeclSpec_tok"; struct Cyc_DeclSpec_tok_struct{
unsigned char* tag; struct Cyc_Parse_Declaration_spec* f1; } ; unsigned char Cyc_Declarator_tok[
19u]="\000\000\000\000Declarator_tok"; struct Cyc_Declarator_tok_struct{
unsigned char* tag; struct Cyc_Parse_Declarator* f1; } ; unsigned char Cyc_DesignatorList_tok[
23u]="\000\000\000\000DesignatorList_tok"; struct Cyc_DesignatorList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_Designator_tok[
19u]="\000\000\000\000Designator_tok"; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_EnumfieldList_tok[ 22u]="\000\000\000\000EnumfieldList_tok";
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Enumfield_tok[ 18u]="\000\000\000\000Enumfield_tok";
struct Cyc_Enumfield_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Enumfield*
f1; } ; unsigned char Cyc_ExpList_tok[ 16u]="\000\000\000\000ExpList_tok";
struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1; } ;
unsigned char Cyc_Exp_tok[ 12u]="\000\000\000\000Exp_tok"; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; unsigned char Cyc_FieldPatternList_tok[
25u]="\000\000\000\000FieldPatternList_tok"; struct Cyc_FieldPatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_FieldPattern_tok[
21u]="\000\000\000\000FieldPattern_tok"; struct _tuple13{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple13* f1; } ; unsigned char Cyc_FnDecl_tok[ 15u]="\000\000\000\000FnDecl_tok";
struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl* f1; }
; unsigned char Cyc_IdList_tok[ 15u]="\000\000\000\000IdList_tok"; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_InitDeclList_tok[
21u]="\000\000\000\000InitDeclList_tok"; struct Cyc_InitDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_InitDecl_tok[
17u]="\000\000\000\000InitDecl_tok"; struct _tuple14{ struct Cyc_Parse_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple14* f1; } ; unsigned char Cyc_InitializerList_tok[ 24u]="\000\000\000\000InitializerList_tok";
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Int_tok[ 12u]="\000\000\000\000Int_tok"; struct
_tuple15{ void* f1; int f2; } ; struct Cyc_Int_tok_struct{ unsigned char* tag;
struct _tuple15* f1; } ; unsigned char Cyc_Kind_tok[ 13u]="\000\000\000\000Kind_tok";
struct Cyc_Kind_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char Cyc_ParamDeclListBool_tok[
26u]="\000\000\000\000ParamDeclListBool_tok"; struct _tuple16{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple16* f1; } ; unsigned char Cyc_ParamDeclList_tok[ 22u]="\000\000\000\000ParamDeclList_tok";
struct Cyc_ParamDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_ParamDecl_tok[ 18u]="\000\000\000\000ParamDecl_tok";
struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple2* f1; } ;
unsigned char Cyc_PatternList_tok[ 20u]="\000\000\000\000PatternList_tok";
struct Cyc_PatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1;
} ; unsigned char Cyc_Pattern_tok[ 16u]="\000\000\000\000Pattern_tok"; struct
Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat* f1; } ;
unsigned char Cyc_Pointer_Sort_tok[ 21u]="\000\000\000\000Pointer_Sort_tok";
struct Cyc_Pointer_Sort_tok_struct{ unsigned char* tag; void* f1; } ;
unsigned char Cyc_Primop_tok[ 15u]="\000\000\000\000Primop_tok"; struct Cyc_Primop_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_Primopopt_tok[ 18u]="\000\000\000\000Primopopt_tok";
struct Cyc_Primopopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; }
; unsigned char Cyc_QualId_tok[ 15u]="\000\000\000\000QualId_tok"; struct Cyc_QualId_tok_struct{
unsigned char* tag; struct _tuple1* f1; } ; unsigned char Cyc_QualSpecList_tok[
21u]="\000\000\000\000QualSpecList_tok"; struct _tuple17{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple17* f1; } ; unsigned char Cyc_Rgnorder_tok[ 17u]="\000\000\000\000Rgnorder_tok";
struct Cyc_Rgnorder_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1; }
; unsigned char Cyc_Scope_tok[ 14u]="\000\000\000\000Scope_tok"; struct Cyc_Scope_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_Short_tok[ 14u]="\000\000\000\000Short_tok";
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; unsigned char Cyc_Stmt_tok[
13u]="\000\000\000\000Stmt_tok"; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; unsigned char Cyc_StorageClass_tok[ 21u]="\000\000\000\000StorageClass_tok";
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; enum  Cyc_Parse_Storage_class
f1; } ; unsigned char Cyc_String_tok[ 15u]="\000\000\000\000String_tok"; struct
Cyc_String_tok_struct{ unsigned char* tag; struct _tagged_arr f1; } ;
unsigned char Cyc_Stringopt_tok[ 18u]="\000\000\000\000Stringopt_tok"; struct
Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; } ;
unsigned char Cyc_StructFieldDeclListList_tok[ 32u]="\000\000\000\000StructFieldDeclListList_tok";
struct Cyc_StructFieldDeclListList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_StructFieldDeclList_tok[ 28u]="\000\000\000\000StructFieldDeclList_tok";
struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_StructOrUnion_tok[ 22u]="\000\000\000\000StructOrUnion_tok";
struct Cyc_StructOrUnion_tok_struct{ unsigned char* tag; enum  Cyc_Parse_Struct_or_union
f1; } ; unsigned char Cyc_SwitchCClauseList_tok[ 26u]="\000\000\000\000SwitchCClauseList_tok";
struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_SwitchClauseList_tok[ 25u]="\000\000\000\000SwitchClauseList_tok";
struct Cyc_SwitchClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_TunionFieldList_tok[ 24u]="\000\000\000\000TunionFieldList_tok";
struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_TunionField_tok[ 20u]="\000\000\000\000TunionField_tok";
struct Cyc_TunionField_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tunionfield*
f1; } ; unsigned char Cyc_TypeList_tok[ 17u]="\000\000\000\000TypeList_tok";
struct Cyc_TypeList_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1; }
; unsigned char Cyc_TypeModifierList_tok[ 25u]="\000\000\000\000TypeModifierList_tok";
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_TypeOpt_tok[ 16u]="\000\000\000\000TypeOpt_tok";
struct Cyc_TypeOpt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; } ;
unsigned char Cyc_TypeQual_tok[ 17u]="\000\000\000\000TypeQual_tok"; struct Cyc_TypeQual_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tqual f1; } ; unsigned char Cyc_TypeSpecifier_tok[
22u]="\000\000\000\000TypeSpecifier_tok"; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_Type_tok[ 13u]="\000\000\000\000Type_tok";
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char Cyc_YY1[
8u]="\000\000\000\000YY1"; struct Cyc_YY1_struct{ unsigned char* tag; struct
_tuple16* f1; } ; unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL";
unsigned char Cyc_Okay_tok[ 13u]="\000\000\000\000Okay_tok"; static
unsigned char _temp639[ 15u]="$(sign_t,int)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp639, _temp639, _temp639 +  15u}};
struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ struct _tuple15* yyzzz;{ void*
_temp640= yy1; struct _tuple15* _temp646; _LL642: if(*(( void**) _temp640) == 
Cyc_Int_tok){ _LL647: _temp646=(( struct Cyc_Int_tok_struct*) _temp640)->f1;
goto _LL643;} else{ goto _LL644;} _LL644: goto _LL645; _LL643: yyzzz= _temp646;
goto _LL641; _LL645:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL641:;}
return yyzzz;} static unsigned char _temp649[ 9u]="string_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp649, _temp649, _temp649 +  9u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ struct _tagged_arr yyzzz;{
void* _temp650= yy1; struct _tagged_arr _temp656; _LL652: if(*(( void**)
_temp650) ==  Cyc_String_tok){ _LL657: _temp656=(( struct Cyc_String_tok_struct*)
_temp650)->f1; goto _LL653;} else{ goto _LL654;} _LL654: goto _LL655; _LL653:
yyzzz= _temp656; goto _LL651; _LL655:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL651:;} return yyzzz;} static unsigned char _temp659[ 5u]="char"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{ _temp659,
_temp659, _temp659 +  5u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
unsigned char yyzzz;{ void* _temp660= yy1; unsigned char _temp666; _LL662: if(*((
void**) _temp660) ==  Cyc_Char_tok){ _LL667: _temp666=(( struct Cyc_Char_tok_struct*)
_temp660)->f1; goto _LL663;} else{ goto _LL664;} _LL664: goto _LL665; _LL663:
yyzzz= _temp666; goto _LL661; _LL665:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL661:;} return yyzzz;} static unsigned char _temp669[ 20u]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{
_temp669, _temp669, _temp669 +  20u}}; void* Cyc_yyget_Pointer_Sort_tok( void*
yy1){ void* yyzzz;{ void* _temp670= yy1; void* _temp676; _LL672: if(*(( void**)
_temp670) ==  Cyc_Pointer_Sort_tok){ _LL677: _temp676=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp670)->f1; goto _LL673;} else{ goto _LL674;} _LL674: goto _LL675; _LL673:
yyzzz= _temp676; goto _LL671; _LL675:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL671:;} return yyzzz;} static unsigned char _temp679[ 6u]="exp_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp679,
_temp679, _temp679 +  6u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp680= yy1; struct Cyc_Absyn_Exp*
_temp686; _LL682: if(*(( void**) _temp680) ==  Cyc_Exp_tok){ _LL687: _temp686=((
struct Cyc_Exp_tok_struct*) _temp680)->f1; goto _LL683;} else{ goto _LL684;}
_LL684: goto _LL685; _LL683: yyzzz= _temp686; goto _LL681; _LL685:( int) _throw((
void*)& Cyc_yyfail_Exp_tok); _LL681:;} return yyzzz;} static unsigned char
_temp689[ 14u]="list_t<exp_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{ _temp689, _temp689, _temp689 +  14u}}; struct Cyc_List_List*
Cyc_yyget_ExpList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp690=
yy1; struct Cyc_List_List* _temp696; _LL692: if(*(( void**) _temp690) ==  Cyc_ExpList_tok){
_LL697: _temp696=(( struct Cyc_ExpList_tok_struct*) _temp690)->f1; goto _LL693;}
else{ goto _LL694;} _LL694: goto _LL695; _LL693: yyzzz= _temp696; goto _LL691;
_LL695:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL691:;} return yyzzz;}
static unsigned char _temp699[ 39u]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{
_temp699, _temp699, _temp699 +  39u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp700= yy1; struct Cyc_List_List*
_temp706; _LL702: if(*(( void**) _temp700) ==  Cyc_InitializerList_tok){ _LL707:
_temp706=(( struct Cyc_InitializerList_tok_struct*) _temp700)->f1; goto _LL703;}
else{ goto _LL704;} _LL704: goto _LL705; _LL703: yyzzz= _temp706; goto _LL701;
_LL705:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL701:;} return
yyzzz;} static unsigned char _temp709[ 9u]="primop_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp709, _temp709, _temp709 +  9u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* yyzzz;{ void* _temp710= yy1; void*
_temp716; _LL712: if(*(( void**) _temp710) ==  Cyc_Primop_tok){ _LL717: _temp716=(
void*)(( struct Cyc_Primop_tok_struct*) _temp710)->f1; goto _LL713;} else{ goto
_LL714;} _LL714: goto _LL715; _LL713: yyzzz= _temp716; goto _LL711; _LL715:( int)
_throw(( void*)& Cyc_yyfail_Primop_tok); _LL711:;} return yyzzz;} static
unsigned char _temp719[ 16u]="opt_t<primop_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp719, _temp719, _temp719 +  16u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ struct Cyc_Core_Opt*
yyzzz;{ void* _temp720= yy1; struct Cyc_Core_Opt* _temp726; _LL722: if(*(( void**)
_temp720) ==  Cyc_Primopopt_tok){ _LL727: _temp726=(( struct Cyc_Primopopt_tok_struct*)
_temp720)->f1; goto _LL723;} else{ goto _LL724;} _LL724: goto _LL725; _LL723:
yyzzz= _temp726; goto _LL721; _LL725:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL721:;} return yyzzz;} static unsigned char _temp729[ 7u]="qvar_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{
_temp729, _temp729, _temp729 +  7u}}; struct _tuple1* Cyc_yyget_QualId_tok( void*
yy1){ struct _tuple1* yyzzz;{ void* _temp730= yy1; struct _tuple1* _temp736;
_LL732: if(*(( void**) _temp730) ==  Cyc_QualId_tok){ _LL737: _temp736=(( struct
Cyc_QualId_tok_struct*) _temp730)->f1; goto _LL733;} else{ goto _LL734;} _LL734:
goto _LL735; _LL733: yyzzz= _temp736; goto _LL731; _LL735:( int) _throw(( void*)&
Cyc_yyfail_QualId_tok); _LL731:;} return yyzzz;} static unsigned char _temp739[
7u]="stmt_t"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{
_temp739, _temp739, _temp739 +  7u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok(
void* yy1){ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp740= yy1; struct Cyc_Absyn_Stmt*
_temp746; _LL742: if(*(( void**) _temp740) ==  Cyc_Stmt_tok){ _LL747: _temp746=((
struct Cyc_Stmt_tok_struct*) _temp740)->f1; goto _LL743;} else{ goto _LL744;}
_LL744: goto _LL745; _LL743: yyzzz= _temp746; goto _LL741; _LL745:( int) _throw((
void*)& Cyc_yyfail_Stmt_tok); _LL741:;} return yyzzz;} static unsigned char
_temp749[ 24u]="list_t<switch_clause_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp749, _temp749,
_temp749 +  24u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ struct Cyc_List_List* yyzzz;{ void* _temp750= yy1; struct Cyc_List_List*
_temp756; _LL752: if(*(( void**) _temp750) ==  Cyc_SwitchClauseList_tok){ _LL757:
_temp756=(( struct Cyc_SwitchClauseList_tok_struct*) _temp750)->f1; goto _LL753;}
else{ goto _LL754;} _LL754: goto _LL755; _LL753: yyzzz= _temp756; goto _LL751;
_LL755:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL751:;}
return yyzzz;} static unsigned char _temp759[ 25u]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp759, _temp759, _temp759 +  25u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp760= yy1; struct Cyc_List_List*
_temp766; _LL762: if(*(( void**) _temp760) ==  Cyc_SwitchCClauseList_tok){
_LL767: _temp766=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp760)->f1;
goto _LL763;} else{ goto _LL764;} _LL764: goto _LL765; _LL763: yyzzz= _temp766;
goto _LL761; _LL765:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL761:;} return yyzzz;} static unsigned char _temp769[ 6u]="pat_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{
_temp769, _temp769, _temp769 +  6u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
void* yy1){ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp770= yy1; struct Cyc_Absyn_Pat*
_temp776; _LL772: if(*(( void**) _temp770) ==  Cyc_Pattern_tok){ _LL777:
_temp776=(( struct Cyc_Pattern_tok_struct*) _temp770)->f1; goto _LL773;} else{
goto _LL774;} _LL774: goto _LL775; _LL773: yyzzz= _temp776; goto _LL771; _LL775:(
int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL771:;} return yyzzz;} static
unsigned char _temp779[ 14u]="list_t<pat_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp779, _temp779, _temp779 + 
14u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp780= yy1; struct Cyc_List_List* _temp786; _LL782: if(*(( void**)
_temp780) ==  Cyc_PatternList_tok){ _LL787: _temp786=(( struct Cyc_PatternList_tok_struct*)
_temp780)->f1; goto _LL783;} else{ goto _LL784;} _LL784: goto _LL785; _LL783:
yyzzz= _temp786; goto _LL781; _LL785:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL781:;} return yyzzz;} static unsigned char _temp789[ 31u]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp789, _temp789, _temp789 +  31u}}; struct _tuple13* Cyc_yyget_FieldPattern_tok(
void* yy1){ struct _tuple13* yyzzz;{ void* _temp790= yy1; struct _tuple13*
_temp796; _LL792: if(*(( void**) _temp790) ==  Cyc_FieldPattern_tok){ _LL797:
_temp796=(( struct Cyc_FieldPattern_tok_struct*) _temp790)->f1; goto _LL793;}
else{ goto _LL794;} _LL794: goto _LL795; _LL793: yyzzz= _temp796; goto _LL791;
_LL795:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL791:;} return
yyzzz;} static unsigned char _temp799[ 39u]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp799, _temp799, _temp799 +  39u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp800= yy1; struct Cyc_List_List*
_temp806; _LL802: if(*(( void**) _temp800) ==  Cyc_FieldPatternList_tok){ _LL807:
_temp806=(( struct Cyc_FieldPatternList_tok_struct*) _temp800)->f1; goto _LL803;}
else{ goto _LL804;} _LL804: goto _LL805; _LL803: yyzzz= _temp806; goto _LL801;
_LL805:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL801:;}
return yyzzz;} static unsigned char _temp809[ 9u]="fndecl_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp809, _temp809, _temp809 +  9u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ struct Cyc_Absyn_Fndecl*
yyzzz;{ void* _temp810= yy1; struct Cyc_Absyn_Fndecl* _temp816; _LL812: if(*((
void**) _temp810) ==  Cyc_FnDecl_tok){ _LL817: _temp816=(( struct Cyc_FnDecl_tok_struct*)
_temp810)->f1; goto _LL813;} else{ goto _LL814;} _LL814: goto _LL815; _LL813:
yyzzz= _temp816; goto _LL811; _LL815:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL811:;} return yyzzz;} static unsigned char _temp819[ 15u]="list_t<decl_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp819, _temp819, _temp819 +  15u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp820= yy1; struct Cyc_List_List*
_temp826; _LL822: if(*(( void**) _temp820) ==  Cyc_DeclList_tok){ _LL827:
_temp826=(( struct Cyc_DeclList_tok_struct*) _temp820)->f1; goto _LL823;} else{
goto _LL824;} _LL824: goto _LL825; _LL823: yyzzz= _temp826; goto _LL821; _LL825:(
int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL821:;} return yyzzz;} static
unsigned char _temp829[ 12u]="decl_spec_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp829, _temp829, _temp829 +  12u}};
struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){ struct
Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp830= yy1; struct Cyc_Parse_Declaration_spec*
_temp836; _LL832: if(*(( void**) _temp830) ==  Cyc_DeclSpec_tok){ _LL837:
_temp836=(( struct Cyc_DeclSpec_tok_struct*) _temp830)->f1; goto _LL833;} else{
goto _LL834;} _LL834: goto _LL835; _LL833: yyzzz= _temp836; goto _LL831; _LL835:(
int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL831:;} return yyzzz;} static
unsigned char _temp839[ 27u]="$(declarator_t,exp_opt_t)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp839, _temp839, _temp839 +  27u}};
struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){ struct _tuple14* yyzzz;{
void* _temp840= yy1; struct _tuple14* _temp846; _LL842: if(*(( void**) _temp840)
==  Cyc_InitDecl_tok){ _LL847: _temp846=(( struct Cyc_InitDecl_tok_struct*)
_temp840)->f1; goto _LL843;} else{ goto _LL844;} _LL844: goto _LL845; _LL843:
yyzzz= _temp846; goto _LL841; _LL845:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL841:;} return yyzzz;} static unsigned char _temp849[ 35u]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp849, _temp849, _temp849 +  35u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp850= yy1; struct Cyc_List_List*
_temp856; _LL852: if(*(( void**) _temp850) ==  Cyc_InitDeclList_tok){ _LL857:
_temp856=(( struct Cyc_InitDeclList_tok_struct*) _temp850)->f1; goto _LL853;}
else{ goto _LL854;} _LL854: goto _LL855; _LL853: yyzzz= _temp856; goto _LL851;
_LL855:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL851:;} return
yyzzz;} static unsigned char _temp859[ 16u]="storage_class_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp859, _temp859, _temp859 + 
16u}}; enum  Cyc_Parse_Storage_class Cyc_yyget_StorageClass_tok( void* yy1){
enum  Cyc_Parse_Storage_class yyzzz;{ void* _temp860= yy1; enum  Cyc_Parse_Storage_class
_temp866; _LL862: if(*(( void**) _temp860) ==  Cyc_StorageClass_tok){ _LL867:
_temp866=(( struct Cyc_StorageClass_tok_struct*) _temp860)->f1; goto _LL863;}
else{ goto _LL864;} _LL864: goto _LL865; _LL863: yyzzz= _temp866; goto _LL861;
_LL865:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL861:;} return
yyzzz;} static unsigned char _temp869[ 17u]="type_specifier_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp869, _temp869, _temp869 +  17u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* yyzzz;{ void* _temp870= yy1; void* _temp876; _LL872: if(*(( void**)
_temp870) ==  Cyc_TypeSpecifier_tok){ _LL877: _temp876=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp870)->f1; goto _LL873;} else{ goto _LL874;} _LL874: goto _LL875; _LL873:
yyzzz= _temp876; goto _LL871; _LL875:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL871:;} return yyzzz;} static unsigned char _temp879[ 18u]="struct_or_union_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp879, _temp879, _temp879 +  18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ enum  Cyc_Parse_Struct_or_union yyzzz;{ void* _temp880= yy1; enum 
Cyc_Parse_Struct_or_union _temp886; _LL882: if(*(( void**) _temp880) ==  Cyc_StructOrUnion_tok){
_LL887: _temp886=(( struct Cyc_StructOrUnion_tok_struct*) _temp880)->f1; goto
_LL883;} else{ goto _LL884;} _LL884: goto _LL885; _LL883: yyzzz= _temp886; goto
_LL881; _LL885:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok); _LL881:;}
return yyzzz;} static unsigned char _temp889[ 8u]="tqual_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp889, _temp889, _temp889 +  8u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp890= yy1; struct Cyc_Absyn_Tqual _temp896; _LL892: if(*((
void**) _temp890) ==  Cyc_TypeQual_tok){ _LL897: _temp896=(( struct Cyc_TypeQual_tok_struct*)
_temp890)->f1; goto _LL893;} else{ goto _LL894;} _LL894: goto _LL895; _LL893:
yyzzz= _temp896; goto _LL891; _LL895:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL891:;} return yyzzz;} static unsigned char _temp899[ 22u]="list_t<structfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp899, _temp899, _temp899 +  22u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp900= yy1; struct Cyc_List_List*
_temp906; _LL902: if(*(( void**) _temp900) ==  Cyc_StructFieldDeclList_tok){
_LL907: _temp906=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp900)->f1;
goto _LL903;} else{ goto _LL904;} _LL904: goto _LL905; _LL903: yyzzz= _temp906;
goto _LL901; _LL905:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL901:;} return yyzzz;} static unsigned char _temp909[ 30u]="list_t<list_t<structfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp909, _temp909, _temp909 +  30u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{
void* _temp910= yy1; struct Cyc_List_List* _temp916; _LL912: if(*(( void**)
_temp910) ==  Cyc_StructFieldDeclListList_tok){ _LL917: _temp916=(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp910)->f1; goto _LL913;} else{ goto _LL914;} _LL914: goto _LL915; _LL913:
yyzzz= _temp916; goto _LL911; _LL915:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL911:;} return yyzzz;} static unsigned char _temp919[ 24u]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{
_temp919, _temp919, _temp919 +  24u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp920= yy1; struct Cyc_List_List*
_temp926; _LL922: if(*(( void**) _temp920) ==  Cyc_TypeModifierList_tok){ _LL927:
_temp926=(( struct Cyc_TypeModifierList_tok_struct*) _temp920)->f1; goto _LL923;}
else{ goto _LL924;} _LL924: goto _LL925; _LL923: yyzzz= _temp926; goto _LL921;
_LL925:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL921:;}
return yyzzz;} static unsigned char _temp929[ 13u]="declarator_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp929,
_temp929, _temp929 +  13u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp930= yy1; struct Cyc_Parse_Declarator*
_temp936; _LL932: if(*(( void**) _temp930) ==  Cyc_Declarator_tok){ _LL937:
_temp936=(( struct Cyc_Declarator_tok_struct*) _temp930)->f1; goto _LL933;}
else{ goto _LL934;} _LL934: goto _LL935; _LL933: yyzzz= _temp936; goto _LL931;
_LL935:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL931:;} return
yyzzz;} static unsigned char _temp939[ 21u]="abstractdeclarator_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{
_temp939, _temp939, _temp939 +  21u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp940= yy1;
struct Cyc_Parse_Abstractdeclarator* _temp946; _LL942: if(*(( void**) _temp940)
==  Cyc_AbstractDeclarator_tok){ _LL947: _temp946=(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp940)->f1; goto _LL943;} else{ goto _LL944;} _LL944: goto _LL945; _LL943:
yyzzz= _temp946; goto _LL941; _LL945:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL941:;} return yyzzz;} static unsigned char _temp949[ 5u]="bool"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp949,
_temp949, _temp949 +  5u}}; int Cyc_yyget_Bool_tok( void* yy1){ int yyzzz;{ void*
_temp950= yy1; int _temp956; _LL952: if(*(( void**) _temp950) ==  Cyc_Bool_tok){
_LL957: _temp956=(( struct Cyc_Bool_tok_struct*) _temp950)->f1; goto _LL953;}
else{ goto _LL954;} _LL954: goto _LL955; _LL953: yyzzz= _temp956; goto _LL951;
_LL955:( int) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL951:;} return yyzzz;}
static unsigned char _temp959[ 8u]="scope_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp959, _temp959, _temp959 +  8u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* yyzzz;{ void* _temp960= yy1; void*
_temp966; _LL962: if(*(( void**) _temp960) ==  Cyc_Scope_tok){ _LL967: _temp966=(
void*)(( struct Cyc_Scope_tok_struct*) _temp960)->f1; goto _LL963;} else{ goto
_LL964;} _LL964: goto _LL965; _LL963: yyzzz= _temp966; goto _LL961; _LL965:( int)
_throw(( void*)& Cyc_yyfail_Scope_tok); _LL961:;} return yyzzz;} static
unsigned char _temp969[ 14u]="tunionfield_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{ _temp969, _temp969, _temp969 + 
14u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok( void* yy1){
struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp970= yy1; struct Cyc_Absyn_Tunionfield*
_temp976; _LL972: if(*(( void**) _temp970) ==  Cyc_TunionField_tok){ _LL977:
_temp976=(( struct Cyc_TunionField_tok_struct*) _temp970)->f1; goto _LL973;}
else{ goto _LL974;} _LL974: goto _LL975; _LL973: yyzzz= _temp976; goto _LL971;
_LL975:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok); _LL971:;} return
yyzzz;} static unsigned char _temp979[ 22u]="list_t<tunionfield_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp979, _temp979, _temp979 +  22u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp980= yy1; struct Cyc_List_List*
_temp986; _LL982: if(*(( void**) _temp980) ==  Cyc_TunionFieldList_tok){ _LL987:
_temp986=(( struct Cyc_TunionFieldList_tok_struct*) _temp980)->f1; goto _LL983;}
else{ goto _LL984;} _LL984: goto _LL985; _LL983: yyzzz= _temp986; goto _LL981;
_LL985:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok); _LL981:;} return
yyzzz;} static unsigned char _temp989[ 50u]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp989, _temp989, _temp989 +  50u}}; struct _tuple17* Cyc_yyget_QualSpecList_tok(
void* yy1){ struct _tuple17* yyzzz;{ void* _temp990= yy1; struct _tuple17*
_temp996; _LL992: if(*(( void**) _temp990) ==  Cyc_QualSpecList_tok){ _LL997:
_temp996=(( struct Cyc_QualSpecList_tok_struct*) _temp990)->f1; goto _LL993;}
else{ goto _LL994;} _LL994: goto _LL995; _LL993: yyzzz= _temp996; goto _LL991;
_LL995:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL991:;} return
yyzzz;} static unsigned char _temp999[ 14u]="list_t<var_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp999, _temp999, _temp999 +  14u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp1000= yy1; struct Cyc_List_List* _temp1006; _LL1002: if(*((
void**) _temp1000) ==  Cyc_IdList_tok){ _LL1007: _temp1006=(( struct Cyc_IdList_tok_struct*)
_temp1000)->f1; goto _LL1003;} else{ goto _LL1004;} _LL1004: goto _LL1005;
_LL1003: yyzzz= _temp1006; goto _LL1001; _LL1005:( int) _throw(( void*)& Cyc_yyfail_IdList_tok);
_LL1001:;} return yyzzz;} static unsigned char _temp1009[ 32u]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{
_temp1009, _temp1009, _temp1009 +  32u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ struct _tuple2* yyzzz;{ void* _temp1010= yy1; struct _tuple2*
_temp1016; _LL1012: if(*(( void**) _temp1010) ==  Cyc_ParamDecl_tok){ _LL1017:
_temp1016=(( struct Cyc_ParamDecl_tok_struct*) _temp1010)->f1; goto _LL1013;}
else{ goto _LL1014;} _LL1014: goto _LL1015; _LL1013: yyzzz= _temp1016; goto
_LL1011; _LL1015:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1011:;}
return yyzzz;} static unsigned char _temp1019[ 40u]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{
_temp1019, _temp1019, _temp1019 +  40u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1020= yy1; struct Cyc_List_List*
_temp1026; _LL1022: if(*(( void**) _temp1020) ==  Cyc_ParamDeclList_tok){
_LL1027: _temp1026=(( struct Cyc_ParamDeclList_tok_struct*) _temp1020)->f1; goto
_LL1023;} else{ goto _LL1024;} _LL1024: goto _LL1025; _LL1023: yyzzz= _temp1026;
goto _LL1021; _LL1025:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1021:;} return yyzzz;} static unsigned char _temp1029[ 107u]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={ Cyc_Core_Failure,{
_temp1029, _temp1029, _temp1029 +  107u}}; struct _tuple16* Cyc_yyget_YY1( void*
yy1){ struct _tuple16* yyzzz;{ void* _temp1030= yy1; struct _tuple16* _temp1036;
_LL1032: if(*(( void**) _temp1030) ==  Cyc_YY1){ _LL1037: _temp1036=(( struct
Cyc_YY1_struct*) _temp1030)->f1; goto _LL1033;} else{ goto _LL1034;} _LL1034:
goto _LL1035; _LL1033: yyzzz= _temp1036; goto _LL1031; _LL1035:( int) _throw((
void*)& Cyc_yyfail_YY1); _LL1031:;} return yyzzz;} static unsigned char
_temp1039[ 15u]="list_t<type_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1039, _temp1039, _temp1039 +  15u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void*
_temp1040= yy1; struct Cyc_List_List* _temp1046; _LL1042: if(*(( void**)
_temp1040) ==  Cyc_TypeList_tok){ _LL1047: _temp1046=(( struct Cyc_TypeList_tok_struct*)
_temp1040)->f1; goto _LL1043;} else{ goto _LL1044;} _LL1044: goto _LL1045;
_LL1043: yyzzz= _temp1046; goto _LL1041; _LL1045:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok);
_LL1041:;} return yyzzz;} static unsigned char _temp1049[ 21u]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{
_temp1049, _temp1049, _temp1049 +  21u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1050= yy1; struct Cyc_List_List*
_temp1056; _LL1052: if(*(( void**) _temp1050) ==  Cyc_DesignatorList_tok){
_LL1057: _temp1056=(( struct Cyc_DesignatorList_tok_struct*) _temp1050)->f1;
goto _LL1053;} else{ goto _LL1054;} _LL1054: goto _LL1055; _LL1053: yyzzz=
_temp1056; goto _LL1051; _LL1055:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1051:;} return yyzzz;} static unsigned char _temp1059[ 13u]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{
_temp1059, _temp1059, _temp1059 +  13u}}; void* Cyc_yyget_Designator_tok( void*
yy1){ void* yyzzz;{ void* _temp1060= yy1; void* _temp1066; _LL1062: if(*(( void**)
_temp1060) ==  Cyc_Designator_tok){ _LL1067: _temp1066=( void*)(( struct Cyc_Designator_tok_struct*)
_temp1060)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: goto _LL1065;
_LL1063: yyzzz= _temp1066; goto _LL1061; _LL1065:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL1061:;} return yyzzz;} static unsigned char _temp1069[ 7u]="kind_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{
_temp1069, _temp1069, _temp1069 +  7u}}; void* Cyc_yyget_Kind_tok( void* yy1){
void* yyzzz;{ void* _temp1070= yy1; void* _temp1076; _LL1072: if(*(( void**)
_temp1070) ==  Cyc_Kind_tok){ _LL1077: _temp1076=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1070)->f1; goto _LL1073;} else{ goto _LL1074;} _LL1074: goto _LL1075;
_LL1073: yyzzz= _temp1076; goto _LL1071; _LL1075:( int) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1071:;} return yyzzz;} static unsigned char _temp1079[ 7u]="type_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1079, _temp1079, _temp1079 +  7u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* yyzzz;{ void* _temp1080= yy1; void* _temp1086; _LL1082: if(*(( void**)
_temp1080) ==  Cyc_Type_tok){ _LL1087: _temp1086=( void*)(( struct Cyc_Type_tok_struct*)
_temp1080)->f1; goto _LL1083;} else{ goto _LL1084;} _LL1084: goto _LL1085;
_LL1083: yyzzz= _temp1086; goto _LL1081; _LL1085:( int) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1081:;} return yyzzz;} static unsigned char _temp1089[ 20u]="list_t<attribute_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1089, _temp1089, _temp1089 +  20u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1090= yy1; struct Cyc_List_List*
_temp1096; _LL1092: if(*(( void**) _temp1090) ==  Cyc_AttributeList_tok){
_LL1097: _temp1096=(( struct Cyc_AttributeList_tok_struct*) _temp1090)->f1; goto
_LL1093;} else{ goto _LL1094;} _LL1094: goto _LL1095; _LL1093: yyzzz= _temp1096;
goto _LL1091; _LL1095:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1091:;} return yyzzz;} static unsigned char _temp1099[ 12u]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{
_temp1099, _temp1099, _temp1099 +  12u}}; void* Cyc_yyget_Attribute_tok( void*
yy1){ void* yyzzz;{ void* _temp1100= yy1; void* _temp1106; _LL1102: if(*(( void**)
_temp1100) ==  Cyc_Attribute_tok){ _LL1107: _temp1106=( void*)(( struct Cyc_Attribute_tok_struct*)
_temp1100)->f1; goto _LL1103;} else{ goto _LL1104;} _LL1104: goto _LL1105;
_LL1103: yyzzz= _temp1106; goto _LL1101; _LL1105:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1101:;} return yyzzz;} static unsigned char _temp1109[ 12u]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{
_temp1109, _temp1109, _temp1109 +  12u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
void* yy1){ struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp1110= yy1; struct Cyc_Absyn_Enumfield*
_temp1116; _LL1112: if(*(( void**) _temp1110) ==  Cyc_Enumfield_tok){ _LL1117:
_temp1116=(( struct Cyc_Enumfield_tok_struct*) _temp1110)->f1; goto _LL1113;}
else{ goto _LL1114;} _LL1114: goto _LL1115; _LL1113: yyzzz= _temp1116; goto
_LL1111; _LL1115:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1111:;}
return yyzzz;} static unsigned char _temp1119[ 20u]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1119, _temp1119, _temp1119 +  20u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1120= yy1; struct Cyc_List_List*
_temp1126; _LL1122: if(*(( void**) _temp1120) ==  Cyc_EnumfieldList_tok){
_LL1127: _temp1126=(( struct Cyc_EnumfieldList_tok_struct*) _temp1120)->f1; goto
_LL1123;} else{ goto _LL1124;} _LL1124: goto _LL1125; _LL1123: yyzzz= _temp1126;
goto _LL1121; _LL1125:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1121:;} return yyzzz;} static unsigned char _temp1129[ 14u]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{
_temp1129, _temp1129, _temp1129 +  14u}}; struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok(
void* yy1){ struct Cyc_Core_Opt* yyzzz;{ void* _temp1130= yy1; struct Cyc_Core_Opt*
_temp1136; _LL1132: if(*(( void**) _temp1130) ==  Cyc_TypeOpt_tok){ _LL1137:
_temp1136=(( struct Cyc_TypeOpt_tok_struct*) _temp1130)->f1; goto _LL1133;}
else{ goto _LL1134;} _LL1134: goto _LL1135; _LL1133: yyzzz= _temp1136; goto
_LL1131; _LL1135:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1131:;}
return yyzzz;} static unsigned char _temp1139[ 26u]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{
_temp1139, _temp1139, _temp1139 +  26u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1140= yy1; struct Cyc_List_List*
_temp1146; _LL1142: if(*(( void**) _temp1140) ==  Cyc_Rgnorder_tok){ _LL1147:
_temp1146=(( struct Cyc_Rgnorder_tok_struct*) _temp1140)->f1; goto _LL1143;}
else{ goto _LL1144;} _LL1144: goto _LL1145; _LL1143: yyzzz= _temp1146; goto
_LL1141; _LL1145:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1141:;}
return yyzzz;} struct Cyc_Yyltype{ int timestamp; int first_line; int
first_column; int last_line; int last_column; } ; struct Cyc_Yyltype Cyc_yynewloc(){
return({ struct Cyc_Yyltype _temp1148; _temp1148.timestamp= 0; _temp1148.first_line=
0; _temp1148.first_column= 0; _temp1148.last_line= 0; _temp1148.last_column= 0;
_temp1148;});} struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[
349u]={ 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 120, 2, 2, 102, 118, 114, 2, 98, 99, 109, 112, 100,
113, 106, 117, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 105, 94, 103, 97, 104, 111, 110, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 107,
2, 108, 116, 101, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 95, 115, 96, 119, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93}; static
unsigned char _temp1149[ 2u]="$"; static unsigned char _temp1150[ 6u]="error";
static unsigned char _temp1151[ 12u]="$undefined."; static unsigned char
_temp1152[ 5u]="AUTO"; static unsigned char _temp1153[ 9u]="REGISTER"; static
unsigned char _temp1154[ 7u]="STATIC"; static unsigned char _temp1155[ 7u]="EXTERN";
static unsigned char _temp1156[ 8u]="TYPEDEF"; static unsigned char _temp1157[ 5u]="VOID";
static unsigned char _temp1158[ 5u]="CHAR"; static unsigned char _temp1159[ 6u]="SHORT";
static unsigned char _temp1160[ 4u]="INT"; static unsigned char _temp1161[ 5u]="LONG";
static unsigned char _temp1162[ 6u]="FLOAT"; static unsigned char _temp1163[ 7u]="DOUBLE";
static unsigned char _temp1164[ 7u]="SIGNED"; static unsigned char _temp1165[ 9u]="UNSIGNED";
static unsigned char _temp1166[ 6u]="CONST"; static unsigned char _temp1167[ 9u]="VOLATILE";
static unsigned char _temp1168[ 9u]="RESTRICT"; static unsigned char _temp1169[
7u]="STRUCT"; static unsigned char _temp1170[ 6u]="UNION"; static unsigned char
_temp1171[ 5u]="CASE"; static unsigned char _temp1172[ 8u]="DEFAULT"; static
unsigned char _temp1173[ 7u]="INLINE"; static unsigned char _temp1174[ 7u]="SIZEOF";
static unsigned char _temp1175[ 9u]="OFFSETOF"; static unsigned char _temp1176[
3u]="IF"; static unsigned char _temp1177[ 5u]="ELSE"; static unsigned char
_temp1178[ 7u]="SWITCH"; static unsigned char _temp1179[ 6u]="WHILE"; static
unsigned char _temp1180[ 3u]="DO"; static unsigned char _temp1181[ 4u]="FOR";
static unsigned char _temp1182[ 5u]="GOTO"; static unsigned char _temp1183[ 9u]="CONTINUE";
static unsigned char _temp1184[ 6u]="BREAK"; static unsigned char _temp1185[ 7u]="RETURN";
static unsigned char _temp1186[ 5u]="ENUM"; static unsigned char _temp1187[ 8u]="NULL_kw";
static unsigned char _temp1188[ 4u]="LET"; static unsigned char _temp1189[ 6u]="THROW";
static unsigned char _temp1190[ 4u]="TRY"; static unsigned char _temp1191[ 6u]="CATCH";
static unsigned char _temp1192[ 4u]="NEW"; static unsigned char _temp1193[ 9u]="ABSTRACT";
static unsigned char _temp1194[ 9u]="FALLTHRU"; static unsigned char _temp1195[
6u]="USING"; static unsigned char _temp1196[ 10u]="NAMESPACE"; static
unsigned char _temp1197[ 7u]="TUNION"; static unsigned char _temp1198[ 8u]="XTUNION";
static unsigned char _temp1199[ 5u]="FILL"; static unsigned char _temp1200[ 8u]="CODEGEN";
static unsigned char _temp1201[ 4u]="CUT"; static unsigned char _temp1202[ 7u]="SPLICE";
static unsigned char _temp1203[ 7u]="MALLOC"; static unsigned char _temp1204[ 9u]="REGION_T";
static unsigned char _temp1205[ 7u]="REGION"; static unsigned char _temp1206[ 5u]="RNEW";
static unsigned char _temp1207[ 8u]="RMALLOC"; static unsigned char _temp1208[ 8u]="REGIONS";
static unsigned char _temp1209[ 4u]="GEN"; static unsigned char _temp1210[ 7u]="PTR_OP";
static unsigned char _temp1211[ 7u]="INC_OP"; static unsigned char _temp1212[ 7u]="DEC_OP";
static unsigned char _temp1213[ 8u]="LEFT_OP"; static unsigned char _temp1214[ 9u]="RIGHT_OP";
static unsigned char _temp1215[ 6u]="LE_OP"; static unsigned char _temp1216[ 6u]="GE_OP";
static unsigned char _temp1217[ 6u]="EQ_OP"; static unsigned char _temp1218[ 6u]="NE_OP";
static unsigned char _temp1219[ 7u]="AND_OP"; static unsigned char _temp1220[ 6u]="OR_OP";
static unsigned char _temp1221[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1222[ 11u]="DIV_ASSIGN"; static unsigned char _temp1223[ 11u]="MOD_ASSIGN";
static unsigned char _temp1224[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1225[ 11u]="SUB_ASSIGN"; static unsigned char _temp1226[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1227[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1228[ 11u]="AND_ASSIGN"; static unsigned char _temp1229[ 11u]="XOR_ASSIGN";
static unsigned char _temp1230[ 10u]="OR_ASSIGN"; static unsigned char _temp1231[
9u]="ELLIPSIS"; static unsigned char _temp1232[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1233[ 12u]="COLON_COLON"; static unsigned char _temp1234[ 11u]="IDENTIFIER";
static unsigned char _temp1235[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1236[ 7u]="STRING"; static unsigned char _temp1237[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1238[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1239[ 9u]="TYPE_VAR"; static unsigned char _temp1240[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1241[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1242[ 10u]="ATTRIBUTE"; static unsigned char _temp1243[ 4u]="';'"; static
unsigned char _temp1244[ 4u]="'{'"; static unsigned char _temp1245[ 4u]="'}'";
static unsigned char _temp1246[ 4u]="'='"; static unsigned char _temp1247[ 4u]="'('";
static unsigned char _temp1248[ 4u]="')'"; static unsigned char _temp1249[ 4u]="','";
static unsigned char _temp1250[ 4u]="'_'"; static unsigned char _temp1251[ 4u]="'$'";
static unsigned char _temp1252[ 4u]="'<'"; static unsigned char _temp1253[ 4u]="'>'";
static unsigned char _temp1254[ 4u]="':'"; static unsigned char _temp1255[ 4u]="'.'";
static unsigned char _temp1256[ 4u]="'['"; static unsigned char _temp1257[ 4u]="']'";
static unsigned char _temp1258[ 4u]="'*'"; static unsigned char _temp1259[ 4u]="'@'";
static unsigned char _temp1260[ 4u]="'?'"; static unsigned char _temp1261[ 4u]="'+'";
static unsigned char _temp1262[ 4u]="'-'"; static unsigned char _temp1263[ 4u]="'&'";
static unsigned char _temp1264[ 4u]="'|'"; static unsigned char _temp1265[ 4u]="'^'";
static unsigned char _temp1266[ 4u]="'/'"; static unsigned char _temp1267[ 4u]="'%'";
static unsigned char _temp1268[ 4u]="'~'"; static unsigned char _temp1269[ 4u]="'!'";
static unsigned char _temp1270[ 5u]="prog"; static unsigned char _temp1271[ 17u]="translation_unit";
static unsigned char _temp1272[ 21u]="external_declaration"; static
unsigned char _temp1273[ 20u]="function_definition"; static unsigned char
_temp1274[ 21u]="function_definition2"; static unsigned char _temp1275[ 13u]="using_action";
static unsigned char _temp1276[ 15u]="unusing_action"; static unsigned char
_temp1277[ 17u]="namespace_action"; static unsigned char _temp1278[ 19u]="unnamespace_action";
static unsigned char _temp1279[ 12u]="declaration"; static unsigned char
_temp1280[ 17u]="declaration_list"; static unsigned char _temp1281[ 23u]="declaration_specifiers";
static unsigned char _temp1282[ 24u]="storage_class_specifier"; static
unsigned char _temp1283[ 15u]="attributes_opt"; static unsigned char _temp1284[
11u]="attributes"; static unsigned char _temp1285[ 15u]="attribute_list"; static
unsigned char _temp1286[ 10u]="attribute"; static unsigned char _temp1287[ 15u]="type_specifier";
static unsigned char _temp1288[ 5u]="kind"; static unsigned char _temp1289[ 15u]="type_qualifier";
static unsigned char _temp1290[ 15u]="enum_specifier"; static unsigned char
_temp1291[ 11u]="enum_field"; static unsigned char _temp1292[ 22u]="enum_declaration_list";
static unsigned char _temp1293[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1294[ 16u]="type_params_opt"; static unsigned char _temp1295[
16u]="struct_or_union"; static unsigned char _temp1296[ 24u]="struct_declaration_list";
static unsigned char _temp1297[ 25u]="struct_declaration_list0"; static
unsigned char _temp1298[ 21u]="init_declarator_list"; static unsigned char
_temp1299[ 22u]="init_declarator_list0"; static unsigned char _temp1300[ 16u]="init_declarator";
static unsigned char _temp1301[ 19u]="struct_declaration"; static unsigned char
_temp1302[ 25u]="specifier_qualifier_list"; static unsigned char _temp1303[ 23u]="struct_declarator_list";
static unsigned char _temp1304[ 24u]="struct_declarator_list0"; static
unsigned char _temp1305[ 18u]="struct_declarator"; static unsigned char
_temp1306[ 17u]="tunion_specifier"; static unsigned char _temp1307[ 18u]="tunion_or_xtunion";
static unsigned char _temp1308[ 17u]="tunionfield_list"; static unsigned char
_temp1309[ 18u]="tunionfield_scope"; static unsigned char _temp1310[ 12u]="tunionfield";
static unsigned char _temp1311[ 11u]="declarator"; static unsigned char
_temp1312[ 18u]="direct_declarator"; static unsigned char _temp1313[ 8u]="pointer";
static unsigned char _temp1314[ 13u]="pointer_char"; static unsigned char
_temp1315[ 8u]="rgn_opt"; static unsigned char _temp1316[ 4u]="rgn"; static
unsigned char _temp1317[ 20u]="type_qualifier_list"; static unsigned char
_temp1318[ 20u]="parameter_type_list"; static unsigned char _temp1319[ 9u]="type_var";
static unsigned char _temp1320[ 16u]="optional_effect"; static unsigned char
_temp1321[ 19u]="optional_rgn_order"; static unsigned char _temp1322[ 10u]="rgn_order";
static unsigned char _temp1323[ 16u]="optional_inject"; static unsigned char
_temp1324[ 11u]="effect_set"; static unsigned char _temp1325[ 14u]="atomic_effect";
static unsigned char _temp1326[ 11u]="region_set"; static unsigned char
_temp1327[ 15u]="parameter_list"; static unsigned char _temp1328[ 22u]="parameter_declaration";
static unsigned char _temp1329[ 16u]="identifier_list"; static unsigned char
_temp1330[ 17u]="identifier_list0"; static unsigned char _temp1331[ 12u]="initializer";
static unsigned char _temp1332[ 18u]="array_initializer"; static unsigned char
_temp1333[ 17u]="initializer_list"; static unsigned char _temp1334[ 12u]="designation";
static unsigned char _temp1335[ 16u]="designator_list"; static unsigned char
_temp1336[ 11u]="designator"; static unsigned char _temp1337[ 10u]="type_name";
static unsigned char _temp1338[ 14u]="any_type_name"; static unsigned char
_temp1339[ 15u]="type_name_list"; static unsigned char _temp1340[ 20u]="abstract_declarator";
static unsigned char _temp1341[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1342[ 10u]="statement"; static unsigned char _temp1343[ 18u]="labeled_statement";
static unsigned char _temp1344[ 21u]="expression_statement"; static
unsigned char _temp1345[ 19u]="compound_statement"; static unsigned char
_temp1346[ 16u]="block_item_list"; static unsigned char _temp1347[ 20u]="selection_statement";
static unsigned char _temp1348[ 15u]="switch_clauses"; static unsigned char
_temp1349[ 16u]="switchC_clauses"; static unsigned char _temp1350[ 20u]="iteration_statement";
static unsigned char _temp1351[ 15u]="jump_statement"; static unsigned char
_temp1352[ 8u]="pattern"; static unsigned char _temp1353[ 19u]="tuple_pattern_list";
static unsigned char _temp1354[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1355[ 14u]="field_pattern"; static unsigned char _temp1356[ 19u]="field_pattern_list";
static unsigned char _temp1357[ 20u]="field_pattern_list0"; static unsigned char
_temp1358[ 11u]="expression"; static unsigned char _temp1359[ 22u]="assignment_expression";
static unsigned char _temp1360[ 20u]="assignment_operator"; static unsigned char
_temp1361[ 23u]="conditional_expression"; static unsigned char _temp1362[ 20u]="constant_expression";
static unsigned char _temp1363[ 22u]="logical_or_expression"; static
unsigned char _temp1364[ 23u]="logical_and_expression"; static unsigned char
_temp1365[ 24u]="inclusive_or_expression"; static unsigned char _temp1366[ 24u]="exclusive_or_expression";
static unsigned char _temp1367[ 15u]="and_expression"; static unsigned char
_temp1368[ 20u]="equality_expression"; static unsigned char _temp1369[ 22u]="relational_expression";
static unsigned char _temp1370[ 17u]="shift_expression"; static unsigned char
_temp1371[ 20u]="additive_expression"; static unsigned char _temp1372[ 26u]="multiplicative_expression";
static unsigned char _temp1373[ 16u]="cast_expression"; static unsigned char
_temp1374[ 17u]="unary_expression"; static unsigned char _temp1375[ 15u]="unary_operator";
static unsigned char _temp1376[ 19u]="postfix_expression"; static unsigned char
_temp1377[ 19u]="primary_expression"; static unsigned char _temp1378[ 25u]="argument_expression_list";
static unsigned char _temp1379[ 26u]="argument_expression_list0"; static
unsigned char _temp1380[ 9u]="constant"; static unsigned char _temp1381[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 233u]={{ _temp1149, _temp1149, _temp1149
+  2u},{ _temp1150, _temp1150, _temp1150 +  6u},{ _temp1151, _temp1151,
_temp1151 +  12u},{ _temp1152, _temp1152, _temp1152 +  5u},{ _temp1153,
_temp1153, _temp1153 +  9u},{ _temp1154, _temp1154, _temp1154 +  7u},{ _temp1155,
_temp1155, _temp1155 +  7u},{ _temp1156, _temp1156, _temp1156 +  8u},{ _temp1157,
_temp1157, _temp1157 +  5u},{ _temp1158, _temp1158, _temp1158 +  5u},{ _temp1159,
_temp1159, _temp1159 +  6u},{ _temp1160, _temp1160, _temp1160 +  4u},{ _temp1161,
_temp1161, _temp1161 +  5u},{ _temp1162, _temp1162, _temp1162 +  6u},{ _temp1163,
_temp1163, _temp1163 +  7u},{ _temp1164, _temp1164, _temp1164 +  7u},{ _temp1165,
_temp1165, _temp1165 +  9u},{ _temp1166, _temp1166, _temp1166 +  6u},{ _temp1167,
_temp1167, _temp1167 +  9u},{ _temp1168, _temp1168, _temp1168 +  9u},{ _temp1169,
_temp1169, _temp1169 +  7u},{ _temp1170, _temp1170, _temp1170 +  6u},{ _temp1171,
_temp1171, _temp1171 +  5u},{ _temp1172, _temp1172, _temp1172 +  8u},{ _temp1173,
_temp1173, _temp1173 +  7u},{ _temp1174, _temp1174, _temp1174 +  7u},{ _temp1175,
_temp1175, _temp1175 +  9u},{ _temp1176, _temp1176, _temp1176 +  3u},{ _temp1177,
_temp1177, _temp1177 +  5u},{ _temp1178, _temp1178, _temp1178 +  7u},{ _temp1179,
_temp1179, _temp1179 +  6u},{ _temp1180, _temp1180, _temp1180 +  3u},{ _temp1181,
_temp1181, _temp1181 +  4u},{ _temp1182, _temp1182, _temp1182 +  5u},{ _temp1183,
_temp1183, _temp1183 +  9u},{ _temp1184, _temp1184, _temp1184 +  6u},{ _temp1185,
_temp1185, _temp1185 +  7u},{ _temp1186, _temp1186, _temp1186 +  5u},{ _temp1187,
_temp1187, _temp1187 +  8u},{ _temp1188, _temp1188, _temp1188 +  4u},{ _temp1189,
_temp1189, _temp1189 +  6u},{ _temp1190, _temp1190, _temp1190 +  4u},{ _temp1191,
_temp1191, _temp1191 +  6u},{ _temp1192, _temp1192, _temp1192 +  4u},{ _temp1193,
_temp1193, _temp1193 +  9u},{ _temp1194, _temp1194, _temp1194 +  9u},{ _temp1195,
_temp1195, _temp1195 +  6u},{ _temp1196, _temp1196, _temp1196 +  10u},{
_temp1197, _temp1197, _temp1197 +  7u},{ _temp1198, _temp1198, _temp1198 +  8u},{
_temp1199, _temp1199, _temp1199 +  5u},{ _temp1200, _temp1200, _temp1200 +  8u},{
_temp1201, _temp1201, _temp1201 +  4u},{ _temp1202, _temp1202, _temp1202 +  7u},{
_temp1203, _temp1203, _temp1203 +  7u},{ _temp1204, _temp1204, _temp1204 +  9u},{
_temp1205, _temp1205, _temp1205 +  7u},{ _temp1206, _temp1206, _temp1206 +  5u},{
_temp1207, _temp1207, _temp1207 +  8u},{ _temp1208, _temp1208, _temp1208 +  8u},{
_temp1209, _temp1209, _temp1209 +  4u},{ _temp1210, _temp1210, _temp1210 +  7u},{
_temp1211, _temp1211, _temp1211 +  7u},{ _temp1212, _temp1212, _temp1212 +  7u},{
_temp1213, _temp1213, _temp1213 +  8u},{ _temp1214, _temp1214, _temp1214 +  9u},{
_temp1215, _temp1215, _temp1215 +  6u},{ _temp1216, _temp1216, _temp1216 +  6u},{
_temp1217, _temp1217, _temp1217 +  6u},{ _temp1218, _temp1218, _temp1218 +  6u},{
_temp1219, _temp1219, _temp1219 +  7u},{ _temp1220, _temp1220, _temp1220 +  6u},{
_temp1221, _temp1221, _temp1221 +  11u},{ _temp1222, _temp1222, _temp1222 +  11u},{
_temp1223, _temp1223, _temp1223 +  11u},{ _temp1224, _temp1224, _temp1224 +  11u},{
_temp1225, _temp1225, _temp1225 +  11u},{ _temp1226, _temp1226, _temp1226 +  12u},{
_temp1227, _temp1227, _temp1227 +  13u},{ _temp1228, _temp1228, _temp1228 +  11u},{
_temp1229, _temp1229, _temp1229 +  11u},{ _temp1230, _temp1230, _temp1230 +  10u},{
_temp1231, _temp1231, _temp1231 +  9u},{ _temp1232, _temp1232, _temp1232 +  11u},{
_temp1233, _temp1233, _temp1233 +  12u},{ _temp1234, _temp1234, _temp1234 +  11u},{
_temp1235, _temp1235, _temp1235 +  17u},{ _temp1236, _temp1236, _temp1236 +  7u},{
_temp1237, _temp1237, _temp1237 +  19u},{ _temp1238, _temp1238, _temp1238 +  18u},{
_temp1239, _temp1239, _temp1239 +  9u},{ _temp1240, _temp1240, _temp1240 +  16u},{
_temp1241, _temp1241, _temp1241 +  18u},{ _temp1242, _temp1242, _temp1242 +  10u},{
_temp1243, _temp1243, _temp1243 +  4u},{ _temp1244, _temp1244, _temp1244 +  4u},{
_temp1245, _temp1245, _temp1245 +  4u},{ _temp1246, _temp1246, _temp1246 +  4u},{
_temp1247, _temp1247, _temp1247 +  4u},{ _temp1248, _temp1248, _temp1248 +  4u},{
_temp1249, _temp1249, _temp1249 +  4u},{ _temp1250, _temp1250, _temp1250 +  4u},{
_temp1251, _temp1251, _temp1251 +  4u},{ _temp1252, _temp1252, _temp1252 +  4u},{
_temp1253, _temp1253, _temp1253 +  4u},{ _temp1254, _temp1254, _temp1254 +  4u},{
_temp1255, _temp1255, _temp1255 +  4u},{ _temp1256, _temp1256, _temp1256 +  4u},{
_temp1257, _temp1257, _temp1257 +  4u},{ _temp1258, _temp1258, _temp1258 +  4u},{
_temp1259, _temp1259, _temp1259 +  4u},{ _temp1260, _temp1260, _temp1260 +  4u},{
_temp1261, _temp1261, _temp1261 +  4u},{ _temp1262, _temp1262, _temp1262 +  4u},{
_temp1263, _temp1263, _temp1263 +  4u},{ _temp1264, _temp1264, _temp1264 +  4u},{
_temp1265, _temp1265, _temp1265 +  4u},{ _temp1266, _temp1266, _temp1266 +  4u},{
_temp1267, _temp1267, _temp1267 +  4u},{ _temp1268, _temp1268, _temp1268 +  4u},{
_temp1269, _temp1269, _temp1269 +  4u},{ _temp1270, _temp1270, _temp1270 +  5u},{
_temp1271, _temp1271, _temp1271 +  17u},{ _temp1272, _temp1272, _temp1272 +  21u},{
_temp1273, _temp1273, _temp1273 +  20u},{ _temp1274, _temp1274, _temp1274 +  21u},{
_temp1275, _temp1275, _temp1275 +  13u},{ _temp1276, _temp1276, _temp1276 +  15u},{
_temp1277, _temp1277, _temp1277 +  17u},{ _temp1278, _temp1278, _temp1278 +  19u},{
_temp1279, _temp1279, _temp1279 +  12u},{ _temp1280, _temp1280, _temp1280 +  17u},{
_temp1281, _temp1281, _temp1281 +  23u},{ _temp1282, _temp1282, _temp1282 +  24u},{
_temp1283, _temp1283, _temp1283 +  15u},{ _temp1284, _temp1284, _temp1284 +  11u},{
_temp1285, _temp1285, _temp1285 +  15u},{ _temp1286, _temp1286, _temp1286 +  10u},{
_temp1287, _temp1287, _temp1287 +  15u},{ _temp1288, _temp1288, _temp1288 +  5u},{
_temp1289, _temp1289, _temp1289 +  15u},{ _temp1290, _temp1290, _temp1290 +  15u},{
_temp1291, _temp1291, _temp1291 +  11u},{ _temp1292, _temp1292, _temp1292 +  22u},{
_temp1293, _temp1293, _temp1293 +  26u},{ _temp1294, _temp1294, _temp1294 +  16u},{
_temp1295, _temp1295, _temp1295 +  16u},{ _temp1296, _temp1296, _temp1296 +  24u},{
_temp1297, _temp1297, _temp1297 +  25u},{ _temp1298, _temp1298, _temp1298 +  21u},{
_temp1299, _temp1299, _temp1299 +  22u},{ _temp1300, _temp1300, _temp1300 +  16u},{
_temp1301, _temp1301, _temp1301 +  19u},{ _temp1302, _temp1302, _temp1302 +  25u},{
_temp1303, _temp1303, _temp1303 +  23u},{ _temp1304, _temp1304, _temp1304 +  24u},{
_temp1305, _temp1305, _temp1305 +  18u},{ _temp1306, _temp1306, _temp1306 +  17u},{
_temp1307, _temp1307, _temp1307 +  18u},{ _temp1308, _temp1308, _temp1308 +  17u},{
_temp1309, _temp1309, _temp1309 +  18u},{ _temp1310, _temp1310, _temp1310 +  12u},{
_temp1311, _temp1311, _temp1311 +  11u},{ _temp1312, _temp1312, _temp1312 +  18u},{
_temp1313, _temp1313, _temp1313 +  8u},{ _temp1314, _temp1314, _temp1314 +  13u},{
_temp1315, _temp1315, _temp1315 +  8u},{ _temp1316, _temp1316, _temp1316 +  4u},{
_temp1317, _temp1317, _temp1317 +  20u},{ _temp1318, _temp1318, _temp1318 +  20u},{
_temp1319, _temp1319, _temp1319 +  9u},{ _temp1320, _temp1320, _temp1320 +  16u},{
_temp1321, _temp1321, _temp1321 +  19u},{ _temp1322, _temp1322, _temp1322 +  10u},{
_temp1323, _temp1323, _temp1323 +  16u},{ _temp1324, _temp1324, _temp1324 +  11u},{
_temp1325, _temp1325, _temp1325 +  14u},{ _temp1326, _temp1326, _temp1326 +  11u},{
_temp1327, _temp1327, _temp1327 +  15u},{ _temp1328, _temp1328, _temp1328 +  22u},{
_temp1329, _temp1329, _temp1329 +  16u},{ _temp1330, _temp1330, _temp1330 +  17u},{
_temp1331, _temp1331, _temp1331 +  12u},{ _temp1332, _temp1332, _temp1332 +  18u},{
_temp1333, _temp1333, _temp1333 +  17u},{ _temp1334, _temp1334, _temp1334 +  12u},{
_temp1335, _temp1335, _temp1335 +  16u},{ _temp1336, _temp1336, _temp1336 +  11u},{
_temp1337, _temp1337, _temp1337 +  10u},{ _temp1338, _temp1338, _temp1338 +  14u},{
_temp1339, _temp1339, _temp1339 +  15u},{ _temp1340, _temp1340, _temp1340 +  20u},{
_temp1341, _temp1341, _temp1341 +  27u},{ _temp1342, _temp1342, _temp1342 +  10u},{
_temp1343, _temp1343, _temp1343 +  18u},{ _temp1344, _temp1344, _temp1344 +  21u},{
_temp1345, _temp1345, _temp1345 +  19u},{ _temp1346, _temp1346, _temp1346 +  16u},{
_temp1347, _temp1347, _temp1347 +  20u},{ _temp1348, _temp1348, _temp1348 +  15u},{
_temp1349, _temp1349, _temp1349 +  16u},{ _temp1350, _temp1350, _temp1350 +  20u},{
_temp1351, _temp1351, _temp1351 +  15u},{ _temp1352, _temp1352, _temp1352 +  8u},{
_temp1353, _temp1353, _temp1353 +  19u},{ _temp1354, _temp1354, _temp1354 +  20u},{
_temp1355, _temp1355, _temp1355 +  14u},{ _temp1356, _temp1356, _temp1356 +  19u},{
_temp1357, _temp1357, _temp1357 +  20u},{ _temp1358, _temp1358, _temp1358 +  11u},{
_temp1359, _temp1359, _temp1359 +  22u},{ _temp1360, _temp1360, _temp1360 +  20u},{
_temp1361, _temp1361, _temp1361 +  23u},{ _temp1362, _temp1362, _temp1362 +  20u},{
_temp1363, _temp1363, _temp1363 +  22u},{ _temp1364, _temp1364, _temp1364 +  23u},{
_temp1365, _temp1365, _temp1365 +  24u},{ _temp1366, _temp1366, _temp1366 +  24u},{
_temp1367, _temp1367, _temp1367 +  15u},{ _temp1368, _temp1368, _temp1368 +  20u},{
_temp1369, _temp1369, _temp1369 +  22u},{ _temp1370, _temp1370, _temp1370 +  17u},{
_temp1371, _temp1371, _temp1371 +  20u},{ _temp1372, _temp1372, _temp1372 +  26u},{
_temp1373, _temp1373, _temp1373 +  16u},{ _temp1374, _temp1374, _temp1374 +  17u},{
_temp1375, _temp1375, _temp1375 +  15u},{ _temp1376, _temp1376, _temp1376 +  19u},{
_temp1377, _temp1377, _temp1377 +  19u},{ _temp1378, _temp1378, _temp1378 +  25u},{
_temp1379, _temp1379, _temp1379 +  26u},{ _temp1380, _temp1380, _temp1380 +  9u},{
_temp1381, _temp1381, _temp1381 +  20u}}; static short Cyc_yyr1[ 404u]={ 0, 121,
122, 122, 122, 122, 122, 122, 122, 123, 123, 124, 124, 124, 124, 125, 125, 126,
127, 128, 129, 130, 130, 130, 130, 131, 131, 132, 132, 132, 132, 132, 132, 132,
132, 133, 133, 133, 133, 133, 133, 133, 134, 134, 135, 136, 136, 137, 137, 137,
137, 137, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
138, 138, 138, 139, 139, 140, 140, 140, 141, 141, 142, 142, 143, 143, 144, 144,
144, 144, 144, 145, 145, 145, 146, 146, 147, 148, 148, 149, 150, 150, 151, 151,
152, 153, 153, 153, 153, 154, 155, 155, 156, 156, 156, 157, 157, 157, 157, 158,
158, 159, 159, 159, 159, 160, 160, 160, 161, 161, 162, 162, 163, 163, 163, 163,
163, 163, 163, 163, 163, 163, 163, 163, 164, 164, 164, 164, 165, 165, 165, 165,
165, 166, 166, 167, 167, 168, 168, 169, 169, 169, 169, 170, 170, 171, 171, 172,
172, 173, 173, 174, 174, 175, 175, 176, 176, 176, 176, 177, 177, 178, 178, 179,
179, 179, 180, 181, 181, 182, 182, 183, 183, 183, 183, 184, 184, 184, 184, 185,
186, 186, 187, 187, 188, 188, 189, 189, 189, 189, 189, 190, 190, 191, 191, 191,
192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 193, 193, 193, 193,
193, 193, 193, 193, 193, 193, 194, 195, 195, 196, 196, 197, 197, 197, 197, 197,
197, 198, 198, 198, 198, 198, 199, 199, 199, 199, 199, 199, 200, 200, 200, 200,
201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 202, 202,
202, 202, 202, 202, 202, 202, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203,
203, 203, 203, 203, 204, 204, 205, 205, 206, 206, 207, 208, 208, 209, 209, 210,
210, 211, 211, 211, 211, 211, 211, 211, 211, 211, 211, 211, 212, 212, 212, 212,
212, 212, 212, 213, 214, 214, 215, 215, 216, 216, 217, 217, 218, 218, 219, 219,
219, 220, 220, 220, 220, 220, 221, 221, 221, 222, 222, 222, 223, 223, 223, 223,
224, 224, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 226,
226, 226, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227,
228, 228, 228, 228, 228, 228, 228, 228, 228, 229, 230, 230, 231, 231, 231, 231,
232, 232}; static short Cyc_yyr2[ 404u]={ 0, 1, 2, 3, 5, 3, 5, 6, 0, 1, 1, 2, 3,
3, 4, 3, 4, 2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 2,
1, 1, 0, 1, 6, 1, 3, 1, 1, 4, 4, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
4, 4, 1, 1, 1, 1, 1, 5, 2, 1, 3, 1, 3, 4, 6, 6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1,
1, 3, 1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3, 6, 4, 3, 5, 1, 1, 1, 2, 3, 3, 0, 1,
1, 2, 6, 1, 2, 1, 3, 3, 4, 4, 5, 4, 4, 4, 2, 2, 1, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0,
1, 1, 1, 1, 2, 3, 5, 5, 7, 1, 3, 0, 2, 0, 2, 3, 5, 0, 1, 1, 3, 2, 3, 4, 1, 1, 3,
1, 3, 2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2,
3, 4, 3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6,
3, 2, 2, 3, 1, 2, 2, 3, 1, 2, 1, 2, 1, 2, 5, 7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3,
4, 5, 5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1,
2, 1, 1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1,
3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4,
2, 6, 4, 7, 9, 1, 1, 1, 1, 4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2,
5, 4, 4, 5, 1, 1, 3, 1, 1, 1, 1, 1, 1}; static short Cyc_yydefact[ 798u]={ 0,
137, 35, 36, 37, 38, 40, 52, 54, 55, 56, 57, 58, 59, 60, 61, 71, 72, 73, 88, 89,
42, 0, 0, 41, 0, 0, 113, 114, 0, 402, 157, 403, 85, 0, 53, 0, 142, 143, 146, 1,
0, 9, 0, 0, 10, 0, 42, 42, 42, 62, 63, 0, 64, 0, 0, 124, 0, 147, 65, 126, 39, 0,
33, 43, 75, 286, 402, 282, 285, 284, 0, 280, 0, 0, 0, 0, 0, 180, 0, 287, 17, 19,
0, 0, 0, 66, 0, 0, 0, 0, 2, 0, 0, 0, 0, 21, 0, 93, 94, 96, 27, 29, 31, 85, 0, 85,
150, 0, 149, 85, 38, 0, 25, 0, 0, 11, 159, 0, 0, 135, 125, 42, 148, 136, 0, 0,
34, 0, 0, 294, 293, 283, 292, 24, 0, 0, 0, 0, 0, 42, 42, 198, 200, 0, 69, 70,
158, 205, 0, 127, 0, 0, 175, 0, 0, 401, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 398, 388,
399, 400, 0, 0, 0, 0, 371, 0, 369, 370, 0, 305, 318, 326, 328, 330, 332, 334,
336, 339, 344, 347, 350, 354, 0, 356, 372, 387, 386, 0, 3, 0, 5, 0, 22, 0, 0, 0,
12, 28, 30, 32, 84, 0, 90, 91, 0, 83, 85, 0, 111, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 402, 233, 235, 0, 241, 237, 0, 239, 222, 223, 224, 0, 225, 226,
227, 0, 303, 26, 13, 96, 165, 181, 0, 0, 161, 159, 0, 0, 128, 0, 138, 0, 0, 78,
0, 76, 281, 296, 0, 295, 182, 0, 0, 294, 0, 201, 173, 0, 99, 101, 159, 0, 207,
199, 208, 68, 0, 87, 0, 86, 0, 177, 0, 179, 67, 0, 0, 364, 0, 320, 354, 0, 321,
322, 0, 0, 0, 0, 0, 0, 0, 357, 358, 0, 0, 0, 0, 360, 361, 359, 144, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 308, 309, 310, 311, 312, 313, 314,
315, 316, 317, 307, 0, 362, 0, 380, 381, 0, 0, 0, 390, 0, 0, 145, 18, 0, 20, 0,
95, 97, 184, 183, 14, 0, 80, 92, 0, 0, 103, 104, 106, 0, 110, 85, 119, 0, 0, 0,
0, 0, 0, 0, 273, 274, 275, 0, 0, 277, 0, 230, 231, 0, 0, 0, 0, 242, 238, 96, 240,
236, 234, 0, 166, 0, 0, 0, 172, 160, 167, 130, 0, 0, 0, 161, 132, 134, 133, 129,
151, 140, 139, 0, 48, 47, 0, 45, 0, 74, 0, 289, 0, 23, 290, 0, 0, 0, 0, 194, 298,
301, 0, 300, 0, 0, 0, 202, 100, 102, 0, 161, 0, 211, 0, 209, 159, 0, 0, 221, 204,
206, 176, 0, 0, 0, 185, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 389, 396, 0,
395, 327, 0, 329, 331, 333, 335, 337, 338, 342, 343, 340, 341, 345, 346, 348,
349, 351, 352, 353, 306, 378, 379, 374, 0, 376, 377, 0, 0, 0, 4, 6, 0, 325, 107,
98, 0, 0, 0, 112, 121, 120, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 272, 276, 0, 0, 0,
229, 0, 232, 0, 15, 304, 159, 0, 169, 0, 0, 0, 162, 131, 165, 153, 152, 141, 7,
0, 0, 0, 79, 77, 297, 197, 0, 299, 193, 195, 291, 0, 288, 203, 174, 216, 0, 210,
213, 0, 161, 0, 212, 0, 363, 0, 0, 186, 0, 190, 384, 385, 0, 0, 0, 0, 366, 0, 0,
0, 355, 392, 0, 0, 375, 373, 393, 0, 0, 82, 105, 108, 81, 109, 122, 119, 119, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 278, 0, 0, 16, 161, 0, 170, 168, 0, 161, 0, 0, 0,
0, 44, 46, 196, 302, 215, 218, 0, 220, 219, 214, 0, 0, 187, 191, 0, 0, 323, 324,
0, 394, 0, 397, 319, 391, 0, 118, 117, 243, 0, 248, 258, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 279, 0, 155, 171, 163, 154, 159, 0, 49, 50, 217, 365, 0, 192, 0, 0,
382, 0, 0, 0, 254, 0, 0, 260, 0, 0, 0, 268, 0, 0, 0, 0, 0, 0, 0, 0, 247, 228, 0,
161, 0, 0, 367, 0, 383, 0, 244, 0, 0, 0, 245, 259, 261, 262, 0, 270, 269, 0, 264,
0, 0, 0, 0, 248, 249, 164, 156, 0, 0, 0, 123, 0, 0, 246, 263, 271, 265, 266, 0,
0, 248, 250, 0, 188, 368, 254, 255, 267, 248, 251, 51, 254, 256, 248, 252, 257,
253, 0, 0, 0}; static short Cyc_yydefgoto[ 112u]={ 795, 40, 41, 42, 239, 43, 369,
44, 371, 45, 206, 46, 47, 63, 64, 438, 439, 48, 147, 49, 50, 268, 269, 51, 86,
52, 212, 213, 97, 98, 99, 214, 142, 381, 382, 383, 53, 54, 537, 538, 539, 55, 56,
57, 58, 122, 108, 434, 462, 59, 463, 425, 565, 417, 421, 422, 282, 260, 153, 77,
78, 479, 374, 480, 481, 450, 451, 143, 148, 149, 464, 289, 242, 243, 244, 245,
246, 247, 697, 747, 248, 249, 272, 273, 274, 453, 454, 455, 250, 251, 356, 180,
529, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195,
494, 495, 196, 197}; static short Cyc_yypact[ 798u]={ 1810, - -32768, - -32768,
- -32768, - -32768, - 9, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, 14, 193, 2124, - -32768, 193, 8, - -32768, - -32768, 28, -
-32768, 73, - -32768, 39, 159, - -32768, 75, 124, 138, - -32768, - -32768, 1698,
- -32768, 179, 200, - -32768, 476, 14, 14, 14, - -32768, - -32768, 86, - -32768,
122, 2355, 168, 24, 0, - -32768, 193, 148, 100, 2411, - -32768, 170, - -32768,
189, - -32768, - -32768, - -32768, 2173, - -32768, 156, 218, 250, 2173, 244, 267,
286, 182, - -32768, - -32768, 4269, 98, 4269, - -32768, 292, 1407, 3893, 3893, -
-32768, 1698, 1921, 1698, 1921, - -32768, 309, 306, - -32768, 2296, 2411, 2411,
2411, 39, 1407, 39, - -32768, 193, - -32768, - 64, 330, 1233, - -32768, 2355,
476, - -32768, 1639, 4269, 2821, - -32768, 168, 14, - -32768, - -32768, 1921,
337, - -32768, 193, 338, 2173, - -32768, - -32768, - -32768, - -32768, 334, 3893,
35, 348, 234, 14, 14, 436, - -32768, - 4, - -32768, - -32768, - -32768, 341, 20,
- -32768, 507, 368, - -32768, 4046, 357, - -32768, 3893, 3936, 367, 376, 380,
398, 400, 402, 4113, 4113, - -32768, - -32768, - -32768, - -32768, 1464, 408,
4156, 4156, - -32768, 4156, - -32768, - -32768, 418, - -32768, - 45, 446, 434,
407, 428, 417, 44, 423, 378, 130, - -32768, 967, 4156, 247, - -32768, - -32768,
4, 457, - -32768, 468, - -32768, 470, - -32768, 159, 2888, 2355, - -32768, -
-32768, - -32768, - -32768, 486, 494, 1407, - -32768, 530, 502, 39, 193, 505, -
-32768, 474, 41, 510, 2442, 512, 528, 538, 540, 2955, 2442, 147, 2442, 2442, - 5,
533, - -32768, - -32768, 547, 1351, 1351, 476, 1351, - -32768, - -32768, -
-32768, 550, - -32768, - -32768, - -32768, 237, - -32768, - -32768, - -32768,
553, 567, - -32768, - 6, 552, 548, 246, 555, 52, - -32768, 554, 299, 562, 6, 560,
565, 568, - -32768, - -32768, 564, 569, - -32768, 261, 733, 2173, 4269, - -32768,
570, 571, 1407, 1407, 2187, 3022, - 1, - -32768, 183, - -32768, - 6, - -32768,
4269, - -32768, 2083, - -32768, 707, - -32768, - -32768, 1407, 1464, - -32768,
1407, - -32768, - -32768, 2509, - -32768, 593, 3893, 2032, 641, 3893, 3893, 1407,
1464, - -32768, - -32768, 1351, 572, 393, 3893, - -32768, - -32768, - -32768, -
-32768, 4156, 3893, 4156, 4156, 4156, 4156, 4156, 4156, 4156, 4156, 4156, 4156,
4156, 4156, 4156, 4156, 4156, 4156, 4156, - -32768, - -32768, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 3893, -
-32768, 207, - -32768, - -32768, 3089, 212, 3893, - -32768, 2743, 574, - -32768,
- -32768, 1698, - -32768, 1698, - -32768, - -32768, - -32768, - -32768, - -32768,
1407, - -32768, - -32768, 3893, 578, 573, - -32768, 575, 1407, - -32768, 39, 497,
3893, 581, 3893, 3893, 651, 845, 589, - -32768, - -32768, - -32768, 284, 646, -
-32768, 3156, - -32768, - -32768, 2442, 600, 2442, 1577, - -32768, - -32768,
2296, - -32768, - -32768, - -32768, 3893, - -32768, 1407, 596, 251, - -32768, -
-32768, 585, - -32768, 609, 602, 1972, 548, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, 299, 1698, - -32768, 604, 605, 603, 193, - -32768,
3893, - -32768, 2173, - -32768, - -32768, 622, 3893, 2173, 109, - -32768, -
-32768, - -32768, 613, 611, 614, - 17, 624, - -32768, - -32768, - -32768, 618,
548, 621, - -32768, 615, 183, 1861, 4269, 3223, - -32768, - -32768, 341, -
-32768, 623, 612, 640, - -32768, - -32768, 215, 2888, 410, 630, 159, 632, 412,
633, 635, 637, 648, 4003, - -32768, - -32768, 647, 650, 446, 103, 434, 407, 428,
417, 44, 44, 423, 423, 423, 423, 378, 378, 130, 130, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, 652, - -32768, - -32768, 45, 223,
4269, - -32768, - -32768, 656, - -32768, - -32768, - -32768, 530, 3893, 657, -
-32768, - -32768, - -32768, 658, 193, 300, 422, 3893, 427, 433, 649, 3290, 3357,
304, - -32768, - -32768, 660, 664, 661, - -32768, 662, - -32768, 2355, - -32768,
- -32768, 667, 4269, - -32768, 671, - 6, 665, - -32768, - -32768, 137, - -32768,
- -32768, - -32768, - -32768, 395, 670, 6, - -32768, - -32768, - -32768, -
-32768, 666, - -32768, - -32768, - -32768, - -32768, 1000, - -32768, - -32768, -
-32768, - -32768, 673, - -32768, - -32768, 674, 548, 58, - -32768, 669, 675, 694,
677, - -32768, 2587, - -32768, - -32768, - -32768, 2355, 1407, 3936, 757, -
-32768, 675, 685, 2743, - -32768, - -32768, 3893, 3893, - -32768, - -32768, -
-32768, 2743, 325, - -32768, - -32768, - -32768, - -32768, - -32768, 195, 63,
497, 2442, 440, 690, 2442, 3893, 3424, 305, 3491, 320, 3558, 529, - -32768, 692,
702, - -32768, 548, 17, - -32768, - -32768, 698, 548, 1407, 691, 695, 697, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, 700, - -32768, -
-32768, - -32768, 703, 3893, - -32768, - -32768, 2888, 705, - -32768, 593, 699,
- -32768, 340, - -32768, - -32768, - -32768, 710, - -32768, - -32768, 762, 714,
529, - -32768, 459, 2442, 469, 3625, 2442, 477, 3692, 3759, 321, 2173, 688, 715,
- -32768, 2442, - -32768, - -32768, 713, - -32768, 667, 729, - -32768, - -32768,
- -32768, - -32768, 222, - -32768, 717, 1407, - -32768, 2665, 1407, 2442, 566,
721, 726, - -32768, 2442, 2442, 504, - -32768, 2442, 2442, 520, 2442, 523, 3826,
- 34, 1351, - -32768, - -32768, 609, 548, 723, 3893, - -32768, 727, - -32768,
525, - -32768, 3893, 722, 734, - -32768, - -32768, - -32768, - -32768, 2442, -
-32768, - -32768, 2442, - -32768, 2442, 2442, 527, 3893, 964, - -32768, - -32768,
- -32768, 747, 351, 737, - -32768, 327, 1351, - -32768, - -32768, - -32768, -
-32768, - -32768, 2442, 333, 529, - -32768, 738, - -32768, - -32768, 1115, -
-32768, - -32768, 964, - -32768, - -32768, 566, - -32768, 529, - -32768, -
-32768, - -32768, 838, 841, - -32768}; static short Cyc_yypgoto[ 112u]={ -
-32768, 102, - -32768, 534, - -32768, - -32768, - -32768, - -32768, - -32768, -
51, - 38, - 55, - -32768, 535, - 49, 269, - -32768, 87, - -32768, 49, - -32768,
- -32768, 405, - -32768, - 52, - -32768, - 209, - -32768, - -32768, - -32768,
663, 655, 112, - -32768, - -32768, 342, - -32768, - -32768, 2, - -32768, -
-32768, 31, - 37, - 121, - -32768, - -32768, 814, - -32768, - 79, - 27, - 111, -
344, 139, 307, 313, 586, - 382, - 87, - 276, 763, - -32768, - 190, - 155, - 353,
- 272, - -32768, 429, - 77, - 61, - 100, - 15, - 118, - 31, - -32768, - -32768,
- 26, - 226, - -32768, - 665, - 367, - -32768, - -32768, - 21, 608, - -32768,
297, - -32768, - -32768, 50, 163, - -32768, - 146, - 369, - 149, 561, 563, 577,
580, 559, 310, 126, 298, 308, - 133, 583, - -32768, - -32768, - -32768, - 327, -
-32768, - -32768, 10}; static short Cyc_yytable[ 4372u]={ 115, 152, 79, 307, 113,
449, 259, 120, 127, 308, 60, 304, 523, 409, 410, 373, 412, 114, 262, 719, 121,
287, 144, 436, 474, 1, 326, 109, 137, 116, 297, 109, 65, 80, 519, 81, 760, 562,
258, 85, 322, 323, 218, 324, 60, 115, 208, 209, 210, 113, 129, 60, 211, 418, 216,
133, 60, 241, 219, 115, 357, 240, 106, 252, 110, 87, 327, 60, 535, 536, 124, 761,
120, 576, 207, 552, 587, 100, 61, 579, 405, 80, 586, 568, 31, 292, 80, 364, 253,
419, 31, 437, 490, 82, 319, 291, 779, 285, 406, 365, 290, 107, 60, 60, 60, 60,
286, 62, 291, 30, 334, 335, 281, 787, 366, 32, 701, 429, 217, 589, 293, 792, 34,
662, 294, 60, 794, 288, 390, 291, 277, 83, 141, 278, 141, 60, 298, 141, 270, 391,
80, 559, 85, 91, 433, 415, 254, 336, 337, 427, 474, 115, 293, 618, 141, 252, 430,
84, 293, - 116, 1, 60, 663, 624, 287, 386, 141, 141, 527, 467, 140, 30, 140, 88,
297, 140, 533, 32, 104, 467, 376, 105, 296, 145, 241, 241, 276, 241, 240, 240,
146, 240, 140, 393, 199, 200, 201, 202, 126, 400, 151, 403, 404, 415, 140, 140,
581, 30, 616, 512, 513, 514, 31, 32, 60, 447, 448, 215, 457, 89, 141, 320, 416,
107, 475, 60, 476, 266, 387, 151, 420, 257, 473, 90, 528, 254, - 159, 488, 489,
342, 471, 401, - 159, 125, 30, 402, 384, 343, 344, 661, 32, 60, 179, 198, 130,
484, 452, 34, 140, 675, 121, 62, 141, 241, 420, 128, 117, 240, 37, 38, 39, 118,
411, 92, 93, 119, 62, - 85, 30, 399, - 85, 468, 264, - 181, 32, 85, 469, 80, 80,
- 181, 470, 602, 516, - 85, 94, 95, 528, 520, 85, 517, 140, 700, 528, 131, 521,
60, 703, 60, 358, 359, 360, 600, 449, 570, 432, 601, 16, 17, 18, 619, 60, 152,
415, 620, 31, 215, 87, 739, 141, 669, 280, 414, 141, 141, 141, 534, 132, 415,
134, 115, 257, 31, 141, 546, 141, 361, 426, 561, 669, 141, 141, 320, 141, 362,
363, 445, 115, 593, 613, 482, 113, 415, 486, 141, 141, 320, 140, 135, 375, 594,
140, 140, 140, 556, 553, 704, 555, 497, 549, 60, 140, 60, 140, 136, 415, 557,
528, 140, 140, 592, 140, 150, 281, 764, 628, 460, 461, 151, 639, 689, 629, 140,
140, 203, 415, 415, 204, 151, 37, 38, 39, 668, 151, 522, 692, 731, 790, 220, 471,
275, 415, 415, 793, 577, 621, 293, 141, 415, 580, 678, 668, 281, 783, 415, 141,
267, 714, 271, 786, 540, 715, 542, 543, 669, 547, 60, 279, 781, 645, 466, 270,
415, 671, 291, 80, 303, 650, 141, 672, 80, 504, 505, 506, 507, 140, 309, 141,
299, 300, 375, 677, 525, 140, 526, 310, 141, 487, 1, 311, 711, 652, 653, 654,
569, 493, 332, 333, 338, 339, 215, 340, 341, 492, 415, 60, 140, 312, 215, 313,
646, 314, 115, 535, 536, 140, 252, 321, 762, 1, 603, 415, 607, 415, 140, 325,
605, 328, 141, 141, 515, 151, 630, 415, 330, 493, 668, 632, 415, 375, 151, 644,
1, 633, 415, 285, 778, 420, 571, 151, 683, 415, 60, 331, 286, 784, 37, 38, 39,
627, 329, 115, 695, 696, 367, 113, 140, 140, 789, 720, 415, 791, 30, 384, 452,
368, 493, 370, 32, 722, 415, 96, 493, 389, 141, 34, 679, 726, 415, 558, 207, 151,
377, 101, 102, 103, 37, 38, 39, 745, 746, 378, 631, 30, 737, 80, 636, 638, 385,
32, 682, 388, - 178, 685, 752, 415, 295, - 178, - 178, 392, 141, 394, 140, - 178,
395, 286, 30, 37, 38, 39, 755, 415, 32, 757, 415, 768, 300, 776, 415, 34, 743,
680, 681, 396, 596, 397, 380, 508, 509, 407, 37, 38, 39, 502, 503, 375, 408, 413,
140, 510, 511, 205, 423, 416, 424, 428, 141, 721, 265, 435, 725, 440, 441, 431,
443, 326, 442, 485, 459, 735, 444, 458, 491, 530, 531, 732, 283, 284, 524, 241,
541, 532, 544, 240, 548, 686, 688, 744, 691, 550, 694, 554, 750, 751, 140, 560,
753, 754, 563, 756, 564, 141, 566, 572, 574, 573, 80, 241, 578, 1, 583, 240, 584,
598, 585, 31, 241, 710, 588, 670, 240, 590, 772, 597, 591, 773, 599, 774, 775,
241, 604, 606, 241, 240, 608, 609, 240, 610, 302, 140, 724, 305, 305, 728, 730,
611, 785, 614, 634, 316, 317, 615, 617, 622, 625, 626, 640, 305, 305, 641, 305,
642, 257, 141, 151, 375, 141, 643, 647, 649, 655, 612, 66, 659, 660, 657, 375,
305, 664, 676, 665, 666, 759, 673, 375, 674, 684, 698, 699, 702, 766, 717, 705,
30, 733, 706, 769, 707, 713, 32, 708, 140, - 207, 709, 140, 712, 295, - 207, -
207, 716, 718, 777, 734, - 207, 736, 286, 738, 740, 748, 30, 68, 749, 69, 70,
765, 32, 741, 767, 770, 151, 446, 771, 71, 375, 780, 72, 73, 782, 788, 796, 447,
448, 797, 74, 656, 483, 575, 75, 76, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 372, 379, 21, 154, 155, 123, 623, 651, 763, 648, 472,
375, 582, 261, 658, 22, 156, 23, 157, 456, 496, 158, 24, 501, 498, 0, 27, 28,
159, 160, 0, 0, 161, 29, 0, 162, 163, 0, 164, 499, 165, 166, 305, 500, 305, 305,
305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 0, 0,
30, 167, 168, 169, 170, 31, 32, 33, 0, 545, 0, 0, 0, 171, 0, 0, 35, 238, 0, 0, 0,
0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 305, 177, 178, 0, 2, 3, 4, 111, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 695, 696, 21, 154, 155, 221, 0,
222, 223, 224, 225, 226, 227, 228, 229, 22, 156, 23, 157, 230, 0, 158, 24, 231,
0, 0, 27, 28, 159, 160, 232, 233, 161, 29, 234, 162, 163, 0, 164, 305, 165, 166,
0, 0, 0, 305, 0, 0, 0, 0, 0, 0, 66, 345, 346, 347, 348, 349, 350, 351, 352, 353,
354, 235, 167, 168, 169, 170, 31, 32, 33, 0, 236, 112, 0, 0, 171, 0, 355, 35,
238, 0, 0, 0, 0, 0, 0, 173, 305, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 30, 68,
0, 69, 70, 0, 32, 0, 0, 0, 0, 0, 0, 71, 0, 0, 72, 73, 0, 0, 0, 447, 448, 0, 74,
0, 0, 0, 75, 76, 305, 0, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 745, 746, 21, 154, 155, 221, 0, 222, 223, 224, 225, 226, 227,
228, 229, 22, 156, 23, 157, 230, 0, 158, 24, 231, 0, 0, 27, 28, 159, 160, 232,
233, 161, 29, 234, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 305, 0, 0, 0, 0, 0, 0, 0, 0, 305, 235, 167, 168, 169, 170, 31, 32, 33, 0, 236,
112, 0, 0, 171, 0, 0, 35, 238, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0,
0, 0, 177, 178, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 21, 154, 155, 221, 0, 222, 223, 224, 225, 226, 227, 228, 229, 22, 156,
23, 157, 230, 0, 158, 24, 231, 0, 0, 27, 28, 159, 160, 232, 233, 161, 29, 234,
162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 235, 167, 168, 169, 170, 31, 32, 33, 0, 236, 112, 237, 0, 171, 0, 0,
35, 238, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 2, 3,
4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 154,
155, 221, 0, 222, 223, 224, 225, 226, 227, 228, 229, 22, 156, 23, 157, 230, 0,
158, 24, 231, 0, 0, 27, 28, 159, 160, 232, 233, 161, 29, 234, 162, 163, 0, 164,
0, 165, 166, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0,
0, 0, 235, 167, 168, 169, 170, 31, 32, 33, 22, 236, 112, 0, 0, 171, 0, 0, 35,
238, 0, 27, 28, 0, 0, 0, 173, 0, 29, 174, 175, 176, 0, 0, 0, 0, 177, 178, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0,
0, 31, 0, 33, 0, 22, 156, 0, 157, 0, 0, 158, 35, 36, 0, 0, 27, 28, 159, 160, 0,
0, 161, 29, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 31, 32, 33, 0, 0, 318, 0, 0,
171, 0, 0, 35, 238, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177,
178, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 154, 155, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 156, 0, 157, 0, 0, 158, 0, 0, 0, 0, 27, 28, 159,
160, 0, 0, 161, 29, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 30, 167, 168, 169, 170, 31, 32,
33, 0, 0, 0, 0, 0, 171, 22, 0, 35, 238, 0, 0, 0, 0, 0, 0, 173, 27, 28, 174, 175,
176, 0, 0, 29, 0, 177, 178, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 255, 21, 0, 256, 0, 0, 0, 0, 31, 0, 33, 0, 257, 0,
22, 0, 23, 0, 0, 35, 36, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0,
0, 0, 31, 32, 33, 0, 0, 0, - 8, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38,
39, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0,
25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33,
0, 0, 0, 0, 0, 34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4,
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 255, 0, 21, 0, 0, 0,
0, 0, 31, 0, 33, 0, 257, 0, 0, 22, 0, 23, 0, 35, 36, 0, 24, 0, 25, 26, 27, 28, 0,
0, 0, 0, 0, 29, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, - 8, 0,
34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4, 111, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 567, 0, 21, 0, 0, 0, 0, 0, 31, 0,
33, 0, 0, 0, 0, 22, 0, 0, 0, 35, 36, 0, 24, 0, 0, 0, 27, 28, 0, 0, 1, 0, 0, 29,
0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28, 35, 36,
0, 0, 0, 29, 0, 0, 37, 38, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 66, 0, 0, 255, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 257, 0, 0, 0, 295, - 159,
0, 35, 36, 0, 0, - 159, 0, 286, 0, 37, 38, 39, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 67, 68, 66, 69, 70, 0, 32, 0, 0, 0, 0, 0, 0, 71, 0, 22, 72,
73, 0, 0, 0, 0, 0, 0, 74, 0, 27, 28, 75, 76, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 30, 68, 0, 69, 70, 0, 32, 0, 0, 0, 0, 255, 0, 71, 0, 0, 72,
73, 0, 31, 0, 33, 0, 257, 74, 0, 0, 285, 75, 76, 35, 36, 0, 0, 0, 0, 286, 0, 37,
38, 39, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 0, 0,
27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0,
112, 22, 205, 23, 0, 0, 35, 36, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0,
2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21,
0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 22, 0, 112, 0, 0, 0, 0, 24, 35, 36, 0, 27,
28, 0, 0, 0, 0, 0, 29, 154, 155, 221, 0, 222, 223, 224, 225, 226, 227, 228, 229,
0, 156, 0, 157, 230, 0, 158, 0, 231, 0, 0, 0, 0, 159, 160, 232, 233, 161, 0, 234,
162, 163, 31, 164, 33, 165, 166, 0, 0, 0, 0, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 235, 167, 168, 169, 170, 0, 32, 154, 155, 236, 112, 0, 0, 171,
477, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160,
177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 0, 0, 0, 306,
478, 0, 171, 0, 0, 0, 172, 154, 155, 0, 447, 448, 0, 173, 0, 0, 174, 175, 176, 0,
156, 0, 157, 177, 178, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0, 162,
163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 0, 0, 0, 306, 667, 0, 171, 0, 0, 0, 172,
154, 155, 0, 447, 448, 0, 173, 0, 0, 174, 175, 176, 0, 156, 0, 157, 177, 178,
158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169,
170, 0, 32, 0, 0, 0, 306, 742, 0, 171, 0, 0, 0, 172, 154, 155, 0, 447, 448, 0,
173, 0, 0, 174, 175, 176, 0, 156, 0, 157, 177, 178, 158, 0, 0, 0, 0, 0, 0, 159,
160, 0, 0, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 0, 0, 0, 306,
0, 0, 171, 0, 0, 0, 172, 154, 155, 0, 447, 448, 0, 173, 0, 0, 174, 175, 176, 0,
156, 0, 157, 177, 178, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0, 162,
163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 0, 0, 0, 172,
0, 0, 156, 0, 157, 263, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178,
161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 306, 0, 0,
171, 0, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159,
160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155,
398, 0, 0, 0, 171, 0, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175,
176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0,
32, 154, 155, 0, 0, 0, 0, 171, 0, 0, 0, 172, 0, 0, 156, 0, 157, 465, 173, 158, 0,
174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168,
169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 518, 0, 0, 172, 0, 0, 156, 0, 157, 0,
173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0,
164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 551, 0, 0, 172, 0, 0,
156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0,
162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 0, 0, 0,
172, 0, 0, 156, 0, 157, 595, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177,
178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 635, 0, 0,
0, 171, 0, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0,
159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154,
155, 637, 0, 0, 0, 171, 0, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174,
175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169,
170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 687, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173,
158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0,
165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167,
168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 690, 0, 0, 172, 0, 0, 156, 0,
157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162,
163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 693, 0, 0, 0, 171, 0, 0, 0,
172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178,
161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171,
723, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160,
177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0,
0, 0, 171, 727, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0,
0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32,
154, 155, 0, 0, 0, 0, 171, 729, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0,
174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168,
169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 758, 0, 0, 172, 0, 0, 156, 0, 157, 0,
173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0,
164, 0, 165, 166, 0, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0,
0, 0, 30, 167, 168, 169, 170, 0, 32, 0, 159, 160, 0, 0, 161, 171, 0, 0, 163, 172,
164, 0, 165, 166, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 0, 0, 0,
0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 306, 0, 0, 171, 0, 0, 0,
172, 0, 0, 156, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161,
0, 0, 0, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 156, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 0, 159, 160, 612, 0, 161,
171, 0, 0, 163, 172, 164, 0, 165, 166, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0,
0, 177, 178, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0,
0, 0, 301, 0, 0, 0, 172, 0, 0, 156, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 159,
160, 177, 178, 161, 0, 0, 0, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 154, 155, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 0,
159, 160, 0, 0, 161, 315, 0, 0, 163, 172, 164, 0, 165, 166, 0, 0, 173, 0, 0, 174,
175, 176, 0, 0, 0, 0, 177, 178, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0,
32, 0, 0, 0, 0, 0, 0, 171, 0, 0, 0, 172, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175,
176, 0, 0, 0, 0, 177, 178, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 138, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0, 139, 0, 0, 0,
0, 0, 35, 36}; static short Cyc_yycheck[ 4372u]={ 55, 88, 23, 158, 55, 277, 117,
56, 63, 158, 0, 157, 365, 239, 240, 205, 242, 55, 118, 684, 57, 142, 83, 17, 300,
1, 71, 54, 80, 55, 151, 58, 22, 23, 361, 25, 70, 419, 117, 103, 173, 174, 106,
176, 34, 100, 101, 102, 103, 100, 71, 41, 104, 59, 106, 76, 46, 112, 110, 114,
193, 112, 52, 114, 54, 34, 111, 57, 5, 6, 60, 105, 121, 442, 100, 402, 458, 46,
87, 448, 85, 71, 99, 427, 90, 65, 76, 83, 114, 95, 90, 85, 318, 85, 171, 112,
761, 98, 103, 95, 104, 101, 92, 93, 94, 95, 107, 93, 112, 85, 66, 67, 139, 778,
110, 91, 99, 65, 108, 463, 100, 786, 98, 65, 104, 115, 791, 142, 87, 112, 95,
103, 83, 98, 85, 125, 151, 88, 128, 98, 130, 417, 103, 41, 265, 100, 115, 103,
104, 260, 426, 206, 100, 108, 105, 206, 104, 84, 100, 96, 1, 151, 104, 532, 285,
217, 117, 118, 377, 287, 83, 85, 85, 98, 295, 88, 385, 91, 92, 297, 206, 95, 151,
85, 239, 240, 136, 242, 239, 240, 92, 242, 105, 224, 92, 93, 94, 95, 98, 230, 88,
232, 233, 100, 117, 118, 97, 85, 105, 342, 343, 344, 90, 91, 204, 106, 107, 105,
279, 95, 171, 171, 85, 101, 301, 215, 303, 125, 218, 117, 257, 94, 293, 95, 380,
204, 99, 314, 315, 109, 289, 94, 105, 95, 85, 98, 215, 117, 118, 593, 91, 241,
89, 90, 98, 310, 277, 98, 171, 612, 297, 93, 213, 318, 291, 95, 98, 318, 109,
110, 111, 103, 241, 94, 95, 107, 93, 95, 85, 229, 98, 98, 119, 94, 91, 103, 103,
277, 278, 100, 107, 481, 85, 98, 94, 95, 442, 85, 103, 92, 213, 645, 448, 85, 92,
295, 650, 297, 61, 62, 63, 96, 584, 434, 265, 100, 17, 18, 19, 96, 310, 408, 100,
100, 90, 213, 295, 105, 279, 601, 96, 94, 283, 284, 285, 387, 86, 100, 94, 394,
94, 90, 293, 394, 295, 98, 100, 96, 620, 300, 301, 301, 303, 106, 107, 94, 411,
468, 491, 309, 411, 100, 312, 314, 315, 315, 279, 100, 205, 469, 283, 284, 285,
411, 405, 651, 407, 327, 94, 369, 293, 371, 295, 97, 100, 411, 532, 300, 301,
468, 303, 99, 419, 737, 94, 283, 284, 285, 94, 94, 100, 314, 315, 94, 100, 100,
100, 295, 109, 110, 111, 601, 300, 363, 94, 94, 783, 87, 467, 85, 100, 100, 789,
444, 524, 100, 377, 100, 449, 104, 620, 458, 105, 100, 385, 98, 96, 99, 105, 389,
100, 391, 392, 715, 394, 435, 98, 96, 559, 286, 440, 100, 607, 112, 444, 98, 567,
408, 607, 449, 334, 335, 336, 337, 377, 98, 417, 99, 100, 306, 616, 369, 385,
371, 98, 426, 313, 1, 98, 669, 85, 86, 87, 434, 321, 68, 69, 64, 65, 377, 112,
113, 99, 100, 484, 408, 98, 385, 98, 560, 98, 556, 5, 6, 417, 556, 98, 733, 1,
99, 100, 99, 100, 426, 96, 484, 70, 468, 469, 356, 408, 99, 100, 116, 361, 715,
99, 100, 365, 417, 556, 1, 99, 100, 98, 761, 563, 435, 426, 99, 100, 531, 114,
107, 770, 109, 110, 111, 538, 115, 605, 22, 23, 96, 605, 468, 469, 783, 99, 100,
786, 85, 531, 584, 96, 402, 96, 91, 99, 100, 94, 408, 98, 524, 98, 627, 99, 100,
415, 605, 468, 95, 47, 48, 49, 109, 110, 111, 22, 23, 96, 541, 85, 704, 584, 545,
546, 95, 91, 630, 95, 94, 633, 99, 100, 98, 99, 100, 98, 560, 98, 524, 105, 85,
107, 85, 109, 110, 111, 99, 100, 91, 99, 100, 99, 100, 99, 100, 98, 716, 628,
629, 94, 470, 94, 105, 338, 339, 105, 109, 110, 111, 332, 333, 481, 98, 96, 560,
340, 341, 97, 99, 85, 105, 99, 606, 687, 122, 96, 690, 100, 96, 108, 99, 71, 97,
25, 96, 699, 100, 100, 99, 94, 100, 695, 140, 141, 103, 733, 98, 105, 30, 733,
94, 634, 635, 717, 637, 42, 639, 90, 722, 723, 606, 98, 726, 727, 112, 729, 90,
651, 99, 98, 100, 99, 695, 761, 85, 1, 96, 761, 100, 100, 99, 90, 770, 666, 99,
606, 770, 99, 752, 99, 108, 755, 85, 757, 758, 783, 99, 98, 786, 783, 100, 99,
786, 99, 154, 651, 689, 157, 158, 692, 693, 96, 776, 99, 98, 165, 166, 100, 99,
96, 96, 96, 95, 173, 174, 94, 176, 99, 94, 713, 651, 601, 716, 104, 96, 103, 99,
95, 38, 99, 99, 108, 612, 193, 108, 615, 85, 103, 731, 25, 620, 99, 95, 94, 85,
90, 739, 28, 100, 85, 105, 99, 745, 99, 98, 91, 99, 713, 94, 99, 716, 99, 98, 99,
100, 98, 95, 760, 96, 105, 100, 107, 86, 99, 96, 85, 86, 94, 88, 89, 100, 91,
713, 99, 105, 716, 96, 96, 98, 669, 86, 101, 102, 99, 99, 0, 106, 107, 0, 109,
574, 310, 440, 113, 114, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, 204, 213, 24, 25, 26, 58, 531, 567, 736, 563, 291, 715, 450, 117,
584, 37, 38, 39, 40, 278, 326, 43, 44, 331, 328, - 1, 48, 49, 50, 51, - 1, - 1,
54, 55, - 1, 57, 58, - 1, 60, 329, 62, 63, 326, 330, 328, 329, 330, 331, 332,
333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, - 1, - 1, 85, 86, 87,
88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, -
1, - 1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1, 380, 119, 120, -
1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43,
44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60, 442, 62,
63, - 1, - 1, - 1, 448, - 1, - 1, - 1, - 1, - 1, - 1, 38, 72, 73, 74, 75, 76, 77,
78, 79, 80, 81, 85, 86, 87, 88, 89, 90, 91, 92, - 1, 94, 95, - 1, - 1, 98, - 1,
97, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, 109, 491, - 1, 112, 113, 114, - 1, -
1, - 1, - 1, 119, 120, 85, 86, - 1, 88, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, -
1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, 106, 107, - 1, 109, - 1, - 1, - 1, 113,
114, 532, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, -
1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 607,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 616, 85, 86, 87, 88, 89, 90, 91, 92, - 1,
94, 95, - 1, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, 109, - 1,
- 1, 112, 113, 114, - 1, - 1, - 1, - 1, 119, 120, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30,
31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49,
50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 85, 86, 87, 88, 89, 90, 91, 92, - 1, 94, 95, 96, - 1, 98, - 1, - 1, 101, 102,
- 1, - 1, - 1, - 1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1, - 1,
119, 120, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
- 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60,
- 1, 62, 63, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, 90, 91, 92, 37, 94, 95, - 1, - 1, 98, -
1, - 1, 101, 102, - 1, 48, 49, - 1, - 1, - 1, 109, - 1, 55, 112, 113, 114, - 1,
- 1, - 1, - 1, 119, 120, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, 90, - 1, 92, - 1, 37, 38, - 1,
40, - 1, - 1, 43, 101, 102, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57,
58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, 90, 91, 92,
- 1, - 1, 95, - 1, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1,
109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1, - 1, 119, 120, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48,
49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1,
- 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 85, 86,
87, 88, 89, 90, 91, 92, - 1, - 1, - 1, - 1, - 1, 98, 37, - 1, 101, 102, - 1, - 1,
- 1, - 1, - 1, - 1, 109, 48, 49, 112, 113, 114, - 1, - 1, 55, - 1, 119, 120, 0,
1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1,
82, 24, - 1, 85, - 1, - 1, - 1, - 1, 90, - 1, 92, - 1, 94, - 1, 37, - 1, 39, - 1,
- 1, 101, 102, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, - 1, - 1, 90,
91, 92, - 1, - 1, - 1, 96, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1,
- 1, 109, 110, 111, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1,
- 1, - 1, - 1, 55, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1,
37, - 1, 90, 91, 92, - 1, - 1, - 1, - 1, - 1, 98, 48, 49, 101, 102, - 1, - 1, -
1, 55, - 1, - 1, 109, 110, 111, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, 82, - 1, 24, - 1, - 1, - 1, - 1, - 1, 90, - 1, 92, -
1, 94, - 1, - 1, 37, - 1, 39, - 1, 101, 102, - 1, 44, - 1, 46, 47, 48, 49, - 1,
- 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1,
- 1, 37, - 1, 90, 91, 92, - 1, - 1, - 1, 96, - 1, 98, 48, 49, 101, 102, - 1, - 1,
- 1, 55, - 1, - 1, 109, 110, 111, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, 82, - 1, 24, - 1, - 1, - 1, - 1, - 1, 90, - 1,
92, - 1, - 1, - 1, - 1, 37, - 1, - 1, - 1, 101, 102, - 1, 44, - 1, - 1, - 1, 48,
49, - 1, - 1, 1, - 1, - 1, 55, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
85, - 1, - 1, 37, - 1, 90, 91, 92, - 1, - 1, - 1, - 1, - 1, 98, 48, 49, 101, 102,
- 1, - 1, - 1, 55, - 1, - 1, 109, 110, 111, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, 82, - 1, - 1,
85, - 1, - 1, - 1, - 1, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, 99, - 1, 101,
102, - 1, - 1, 105, - 1, 107, - 1, 109, 110, 111, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, 85, 86, 38, 88, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, -
1, 98, - 1, 37, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, 109, - 1, 48, 49, 113,
114, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 85, 86, - 1, 88, 89, - 1, 91, - 1, - 1, - 1, - 1, 82, - 1, 98,
- 1, - 1, 101, 102, - 1, 90, - 1, 92, - 1, 94, 109, - 1, - 1, 98, 113, 114, 101,
102, - 1, - 1, - 1, - 1, 107, - 1, 109, 110, 111, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, - 1,
- 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, 3, 4, 5,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, -
1, - 1, - 1, - 1, - 1, 90, - 1, 92, - 1, - 1, 95, 37, 97, 39, - 1, - 1, 101, 102,
44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, 3, 4, 5,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 90, - 1, 92, 37, - 1, 95, - 1, - 1, - 1, -
1, 44, 101, 102, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, 25, 26, 27, - 1, 29,
30, 31, 32, 33, 34, 35, 36, - 1, 38, - 1, 40, 41, - 1, 43, - 1, 45, - 1, - 1, -
1, - 1, 50, 51, 52, 53, 54, - 1, 56, 57, 58, 90, 60, 92, 62, 63, - 1, - 1, - 1,
- 1, - 1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, 94, 95, - 1, - 1, 98, 32, - 1, -
1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50,
51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 85, 86, 87, 88, 89, - 1, 91, - 1, - 1, - 1, 95, 96, - 1, 98, - 1, - 1, - 1,
102, 25, 26, - 1, 106, 107, - 1, 109, - 1, - 1, 112, 113, 114, - 1, 38, - 1, 40,
119, 120, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57,
58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, -
1, - 1, - 1, 95, 96, - 1, 98, - 1, - 1, - 1, 102, 25, 26, - 1, 106, 107, - 1,
109, - 1, - 1, 112, 113, 114, - 1, 38, - 1, 40, 119, 120, 43, - 1, - 1, - 1, - 1,
- 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, - 1, - 1, - 1, 95, 96, - 1, 98, - 1,
- 1, - 1, 102, 25, 26, - 1, 106, 107, - 1, 109, - 1, - 1, 112, 113, 114, - 1, 38,
- 1, 40, 119, 120, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1,
- 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89,
- 1, 91, - 1, - 1, - 1, 95, - 1, - 1, 98, - 1, - 1, - 1, 102, 25, 26, - 1, 106,
107, - 1, 109, - 1, - 1, 112, 113, 114, - 1, 38, - 1, 40, 119, 120, 43, - 1, - 1,
- 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1,
98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, 108, 109, 43, - 1, 112, 113, 114,
- 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, 95, - 1, - 1, 98, - 1,
- 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1,
50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, 94, - 1, - 1, - 1, 98, - 1, - 1, - 1,
102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51,
119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85,
86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1,
- 1, 38, - 1, 40, 108, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120,
54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87,
88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, 38,
- 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1,
- 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89,
- 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, 38, - 1,
40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1,
57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1,
91, 25, 26, - 1, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, 40,
108, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57,
58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91,
25, 26, 94, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1,
109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, -
1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26,
94, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43,
- 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, -
1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1,
- 1, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112,
113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1,
98, 99, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114,
- 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, 94, - 1, - 1, - 1, 98, - 1,
- 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1,
50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, 99, - 1, - 1,
102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51,
119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85,
86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, 99, - 1, - 1, 102, - 1,
- 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120,
54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87,
88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, 38,
- 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1,
- 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89,
- 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, 38, - 1,
40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1,
57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91,
- 1, 50, 51, - 1, - 1, 54, 98, - 1, - 1, 58, 102, 60, - 1, 62, 63, - 1, - 1, 109,
- 1, - 1, 112, 113, 114, - 1, - 1, - 1, - 1, 119, 120, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, 95, - 1, - 1, 98, - 1, - 1,
- 1, 102, - 1, - 1, 38, - 1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1,
50, 51, 119, 120, 54, - 1, - 1, - 1, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, -
1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 38, - 1, - 1,
- 1, 85, 86, 87, 88, 89, - 1, 91, - 1, 50, 51, 95, - 1, 54, 98, - 1, - 1, 58,
102, 60, - 1, 62, 63, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1, - 1,
119, 120, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26,
- 1, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, - 1, - 1, 109, -
1, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, - 1, 58, - 1,
60, - 1, 62, 63, - 1, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, - 1, 50, 51,
- 1, - 1, 54, 98, - 1, - 1, 58, 102, 60, - 1, 62, 63, - 1, - 1, 109, - 1, - 1,
112, 113, 114, - 1, - 1, - 1, - 1, 119, 120, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
85, 86, 87, 88, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, 98, - 1, - 1, - 1,
102, - 1, - 1, - 1, - 1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1,
- 1, 119, 120, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1,
- 1, 59, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 90,
- 1, 92, - 1, - 1, 95, - 1, - 1, - 1, - 1, - 1, 101, 102}; unsigned char Cyc_Yyimpossible[
17u]="\000\000\000\000Yyimpossible"; unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex(); static int
Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static int
Cyc_yynerrs= 0; struct _tuple18{ void* f1; void* f2; } ; struct _tuple19{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int yystate;
int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short* yyss=(
short*)({ unsigned int _temp3176= 10000u; short* _temp3177=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3176));{ unsigned int
_temp3178= _temp3176; unsigned int i; for( i= 0; i <  _temp3178; i ++){
_temp3177[ i]=( short) 0;}}; _temp3177;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3173= 10000u; void** _temp3174=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3173));{ unsigned int _temp3175= _temp3173;
unsigned int i; for( i= 0; i <  _temp3175; i ++){ _temp3174[ i]= Cyc_yylval;}};
_temp3174;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3170= 10000u; struct Cyc_Yyltype* _temp3171=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3170));{
unsigned int _temp3172= _temp3170; unsigned int i; for( i= 0; i <  _temp3172; i
++){ _temp3171[ i]= Cyc_yynewloc();}}; _temp3171;}); int yystacksize= 10000;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:
yyss[ _check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate;
if( yyssp_offset >=  yystacksize -  1){ if( yystacksize >=  10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 798u, yystate)]; if( yyn ==  - 32768){ goto
yydefault;} if( Cyc_yychar ==  - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar >  0? Cyc_yychar <= 
348: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 349u, Cyc_yychar)]:
233;} yyn += yychar1; if(( yyn <  0? 1: yyn >  4371)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4372u, yyn)] !=  yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4372u, yyn)]; if( yyn <  0){
if( yyn ==  - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
==  0){ goto yyerrlab;}} if( yyn ==  797){ return 0;} if( Cyc_yychar !=  0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus !=  0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 798u, yystate)]; if(
yyn ==  0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 404u, yyn)]; if( yylen >  0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset +  1) -  yylen)];} switch(
yyn){ case 1: _LL1382: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1383:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1385=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1385[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1386; _temp1386.tag= Cyc_DeclList_tok; _temp1386.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1386;});
_temp1385;}); break; case 3: _LL1384: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1388=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1388[ 0]=({ struct Cyc_DeclList_tok_struct _temp1389; _temp1389.tag= Cyc_DeclList_tok;
_temp1389.f1=({ struct Cyc_List_List* _temp1390=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1390->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1391=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1391->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1392=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1392[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1393; _temp1393.tag= Cyc_Absyn_Using_d;
_temp1393.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1393.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1393;}); _temp1392;}));
_temp1391->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1391;});
_temp1390->tl= 0; _temp1390;}); _temp1389;}); _temp1388;}); Cyc_Lex_leave_using();
break; case 4: _LL1387: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1395=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1395[ 0]=({ struct Cyc_DeclList_tok_struct _temp1396; _temp1396.tag= Cyc_DeclList_tok;
_temp1396.f1=({ struct Cyc_List_List* _temp1397=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1397->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1398=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1398->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1399=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1399[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1400; _temp1400.tag= Cyc_Absyn_Using_d;
_temp1400.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp1400.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1400;});
_temp1399;})); _temp1398->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1398;}); _temp1397->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1397;}); _temp1396;});
_temp1395;}); break; case 5: _LL1394: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1402=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1402[ 0]=({ struct Cyc_DeclList_tok_struct _temp1403; _temp1403.tag= Cyc_DeclList_tok;
_temp1403.f1=({ struct Cyc_List_List* _temp1404=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1404->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1405=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1405->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1406=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1406[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1407; _temp1407.tag=
Cyc_Absyn_Namespace_d; _temp1407.f1=({ struct _tagged_arr* _temp1408=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1408[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1408;});
_temp1407.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1407;}); _temp1406;})); _temp1405->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1405;}); _temp1404->tl= 0; _temp1404;}); _temp1403;}); _temp1402;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1401: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1410=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1410[ 0]=({ struct Cyc_DeclList_tok_struct _temp1411; _temp1411.tag= Cyc_DeclList_tok;
_temp1411.f1=({ struct Cyc_List_List* _temp1412=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1412->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1413=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1413->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1414=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1414[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1415; _temp1415.tag=
Cyc_Absyn_Namespace_d; _temp1415.f1=({ struct _tagged_arr* _temp1416=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1416[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1416;});
_temp1415.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1415;}); _temp1414;})); _temp1413->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1413;}); _temp1412->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1412;}); _temp1411;});
_temp1410;}); break; case 7: _LL1409: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1418=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1418[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1419; _temp1419.tag= Cyc_DeclList_tok; _temp1419.f1=({ struct Cyc_List_List*
_temp1420=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1420->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1421=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1421->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1422=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1422[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1423; _temp1423.tag= Cyc_Absyn_ExternC_d;
_temp1423.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1423;}); _temp1422;})); _temp1421->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1421;}); _temp1420->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1420;}); _temp1419;});
_temp1418;}); break; case 8: _LL1417: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1425=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1425[ 0]=({ struct Cyc_DeclList_tok_struct _temp1426; _temp1426.tag= Cyc_DeclList_tok;
_temp1426.f1= 0; _temp1426;}); _temp1425;}); break; case 9: _LL1424: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1428=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1428[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1429; _temp1429.tag= Cyc_DeclList_tok; _temp1429.f1=({ struct Cyc_List_List*
_temp1430=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1430->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1431=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1431[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1432; _temp1432.tag= Cyc_Absyn_Fn_d;
_temp1432.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1432;}); _temp1431;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1430->tl=
0; _temp1430;}); _temp1429;}); _temp1428;}); break; case 10: _LL1427: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1433: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1435=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1435[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1436; _temp1436.tag= Cyc_FnDecl_tok; _temp1436.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1436;}); _temp1435;}); break; case 12: _LL1434: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1438=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1438[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1439; _temp1439.tag= Cyc_FnDecl_tok;
_temp1439.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1440=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1440->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1440;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1439;}); _temp1438;}); break; case 13: _LL1437: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1442=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1442[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1443; _temp1443.tag= Cyc_FnDecl_tok;
_temp1443.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1443;}); _temp1442;}); break; case 14: _LL1441: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1445=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1445[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1446; _temp1446.tag= Cyc_FnDecl_tok;
_temp1446.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1447=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1447->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1447;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1446;}); _temp1445;}); break; case 15: _LL1444: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1449=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1449[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1450; _temp1450.tag= Cyc_FnDecl_tok;
_temp1450.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1451=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1451->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1451;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1450;}); _temp1449;}); break; case 16: _LL1448: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1453=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1453[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1454; _temp1454.tag= Cyc_FnDecl_tok;
_temp1454.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1455=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1455->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1455;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1454;}); _temp1453;}); break; case 17: _LL1452: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1456:
Cyc_Lex_leave_using(); break; case 19: _LL1457: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1459=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1459[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1459;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1458: Cyc_Lex_leave_namespace(); break; case 21: _LL1460: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1462=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1462[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1463; _temp1463.tag= Cyc_DeclList_tok; _temp1463.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1463;}); _temp1462;}); break; case 22: _LL1461: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1465=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1465[ 0]=({ struct Cyc_DeclList_tok_struct _temp1466; _temp1466.tag= Cyc_DeclList_tok;
_temp1466.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1466;}); _temp1465;}); break; case 23: _LL1464: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1468=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1468[ 0]=({ struct Cyc_DeclList_tok_struct _temp1469; _temp1469.tag= Cyc_DeclList_tok;
_temp1469.f1=({ struct Cyc_List_List* _temp1470=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1470->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1470->tl=
0; _temp1470;}); _temp1469;}); _temp1468;}); break; case 24: _LL1467: { struct
Cyc_List_List* _temp1472= 0;{ struct Cyc_List_List* _temp1473= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0;
_temp1473 !=  0; _temp1473=(( struct Cyc_List_List*) _check_null( _temp1473))->tl){
struct _tagged_arr* _temp1474=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1473))->hd; struct _tuple1* _temp1475=({ struct _tuple1*
_temp1478=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1478->f1=
Cyc_Absyn_rel_ns_null; _temp1478->f2= _temp1474; _temp1478;}); struct Cyc_Absyn_Vardecl*
_temp1476= Cyc_Absyn_new_vardecl( _temp1475, Cyc_Absyn_wildtyp( 0), 0);
_temp1472=({ struct Cyc_List_List* _temp1477=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1477->hd=( void*) _temp1476; _temp1477->tl=
_temp1472; _temp1477;});}} _temp1472=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1472); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1479=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1479[ 0]=({ struct Cyc_DeclList_tok_struct _temp1480; _temp1480.tag= Cyc_DeclList_tok;
_temp1480.f1=({ struct Cyc_List_List* _temp1481=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1481->hd=( void*) Cyc_Absyn_letv_decl(
_temp1472, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1481->tl= 0; _temp1481;}); _temp1480;});
_temp1479;}); break;} case 25: _LL1471: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1482:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1484=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1484[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1485; _temp1485.tag= Cyc_DeclList_tok; _temp1485.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1485;});
_temp1484;}); break; case 27: _LL1483: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1487=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1487[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1488; _temp1488.tag= Cyc_DeclSpec_tok;
_temp1488.f1=({ struct Cyc_Parse_Declaration_spec* _temp1489=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1489->sc=({ struct
Cyc_Core_Opt* _temp1490=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp1490->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1490;});
_temp1489->tq= Cyc_Absyn_empty_tqual(); _temp1489->type_specs= 0; _temp1489->is_inline=
0; _temp1489->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1489;}); _temp1488;});
_temp1487;}); break; case 28: _LL1486: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc !=  0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1492=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1492[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1493; _temp1493.tag= Cyc_DeclSpec_tok; _temp1493.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1494=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1494->sc=({ struct Cyc_Core_Opt* _temp1495=( struct Cyc_Core_Opt*)
_cycalloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1495->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1495;});
_temp1494->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1494->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1494->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1494->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1494;}); _temp1493;}); _temp1492;}); break; case 29: _LL1491: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1497=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1497[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1498; _temp1498.tag= Cyc_DeclSpec_tok; _temp1498.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1499=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1499->sc= 0; _temp1499->tq= Cyc_Absyn_empty_tqual(); _temp1499->type_specs=({
struct Cyc_List_List* _temp1500=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1500->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1500->tl= 0;
_temp1500;}); _temp1499->is_inline= 0; _temp1499->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1499;});
_temp1498;}); _temp1497;}); break; case 30: _LL1496: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1502=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1502[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1503; _temp1503.tag= Cyc_DeclSpec_tok;
_temp1503.f1=({ struct Cyc_Parse_Declaration_spec* _temp1504=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1504->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1504->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1504->type_specs=({ struct Cyc_List_List* _temp1505=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1505->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1505->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1505;});
_temp1504->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1504->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1504;}); _temp1503;}); _temp1502;}); break; case 31: _LL1501: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1507=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1507[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1508; _temp1508.tag= Cyc_DeclSpec_tok; _temp1508.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1509=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1509->sc= 0; _temp1509->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1509->type_specs=
0; _temp1509->is_inline= 0; _temp1509->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1509;});
_temp1508;}); _temp1507;}); break; case 32: _LL1506: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1511=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1511[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1512; _temp1512.tag= Cyc_DeclSpec_tok;
_temp1512.f1=({ struct Cyc_Parse_Declaration_spec* _temp1513=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1513->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1513->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1513->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1513->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1513->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1513;}); _temp1512;}); _temp1511;}); break; case 33: _LL1510: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1515=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1515[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1516; _temp1516.tag= Cyc_DeclSpec_tok; _temp1516.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1517=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1517->sc= 0; _temp1517->tq= Cyc_Absyn_empty_tqual(); _temp1517->type_specs=
0; _temp1517->is_inline= 1; _temp1517->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1517;});
_temp1516;}); _temp1515;}); break; case 34: _LL1514: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1519=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1519[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1520; _temp1520.tag= Cyc_DeclSpec_tok;
_temp1520.f1=({ struct Cyc_Parse_Declaration_spec* _temp1521=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1521->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1521->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1521->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1521->is_inline=
1; _temp1521->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1521;}); _temp1520;}); _temp1519;}); break; case 35: _LL1518: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1523=( struct Cyc_StorageClass_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1523[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1524; _temp1524.tag= Cyc_StorageClass_tok;
_temp1524.f1= Cyc_Parse_Auto_sc; _temp1524;}); _temp1523;}); break; case 36:
_LL1522: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1526=( struct
Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1526[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1527; _temp1527.tag=
Cyc_StorageClass_tok; _temp1527.f1= Cyc_Parse_Register_sc; _temp1527;});
_temp1526;}); break; case 37: _LL1525: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1529=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1529[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1530; _temp1530.tag= Cyc_StorageClass_tok; _temp1530.f1= Cyc_Parse_Static_sc;
_temp1530;}); _temp1529;}); break; case 38: _LL1528: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1532=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1532[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1533; _temp1533.tag= Cyc_StorageClass_tok; _temp1533.f1= Cyc_Parse_Extern_sc;
_temp1533;}); _temp1532;}); break; case 39: _LL1531: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1535=( struct Cyc_StorageClass_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1535[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1536; _temp1536.tag= Cyc_StorageClass_tok;
_temp1536.f1= Cyc_Parse_ExternC_sc; _temp1536;}); _temp1535;}); break; case 40:
_LL1534: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1538=( struct
Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1538[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1539; _temp1539.tag=
Cyc_StorageClass_tok; _temp1539.f1= Cyc_Parse_Typedef_sc; _temp1539;});
_temp1538;}); break; case 41: _LL1537: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1541=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1541[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1542; _temp1542.tag= Cyc_StorageClass_tok; _temp1542.f1= Cyc_Parse_Abstract_sc;
_temp1542;}); _temp1541;}); break; case 42: _LL1540: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1544=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1544[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1545; _temp1545.tag=
Cyc_AttributeList_tok; _temp1545.f1= 0; _temp1545;}); _temp1544;}); break; case
43: _LL1543: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1546: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1548=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1548[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1549; _temp1549.tag=
Cyc_AttributeList_tok; _temp1549.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1549;});
_temp1548;}); break; case 45: _LL1547: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1551=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1551[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1552; _temp1552.tag=
Cyc_AttributeList_tok; _temp1552.f1=({ struct Cyc_List_List* _temp1553=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1553->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1553->tl= 0; _temp1553;}); _temp1552;}); _temp1551;});
break; case 46: _LL1550: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1555=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1555[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1556; _temp1556.tag=
Cyc_AttributeList_tok; _temp1556.f1=({ struct Cyc_List_List* _temp1557=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1557->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1557->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1557;}); _temp1556;});
_temp1555;}); break; case 47: _LL1554: { struct _tagged_arr _temp1559= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1559, sizeof( unsigned char)) >  4?*(( const unsigned char*)
_check_unknown_subscript( _temp1559, sizeof( unsigned char), 0)) == '_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1559, sizeof( unsigned char),
1)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1559,
sizeof( unsigned char),( int)( _get_arr_size( _temp1559, sizeof( unsigned char))
-  2))) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1559,
sizeof( unsigned char),( int)( _get_arr_size( _temp1559, sizeof( unsigned char))
-  3))) == '_': 0){ _temp1559=( struct _tagged_arr) Cyc_Std_substring( _temp1559,
2, _get_arr_size( _temp1559, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0;
i <  16u; ++ i){ if( Cyc_Std_strcmp( _temp1559,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1560=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1560[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1561; _temp1561.tag= Cyc_Attribute_tok; _temp1561.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1561;}); _temp1560;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1562=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1562[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1563; _temp1563.tag= Cyc_Attribute_tok; _temp1563.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1563;}); _temp1562;});} break;}} case
48: _LL1558: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1565=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1565[ 0]=({ struct Cyc_Attribute_tok_struct _temp1566; _temp1566.tag= Cyc_Attribute_tok;
_temp1566.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1566;}); _temp1565;});
break; case 49: _LL1564: { struct _tagged_arr _temp1568= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1571; int _temp1572; struct _tuple15* _temp1569= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1571=*
_temp1569; _LL1573: _temp1572= _temp1571.f2; goto _LL1570; _LL1570: { void* a;
if( Cyc_Std_zstrcmp( _temp1568, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1568, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1572 <=  0? 1: _temp1572 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 1 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1574=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1574[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1575; _temp1575.tag= Cyc_Absyn_Regparm_att; _temp1575.f1= _temp1572;
_temp1575;}); _temp1574;});} else{ if( Cyc_Std_zstrcmp( _temp1568, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1568, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1572 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1572;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1576=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1576[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1577; _temp1577.tag= Cyc_Absyn_Aligned_att; _temp1577.f1= _temp1572;
_temp1577;}); _temp1576;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1578=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1578[ 0]=({ struct Cyc_Attribute_tok_struct _temp1579; _temp1579.tag= Cyc_Attribute_tok;
_temp1579.f1=( void*) a; _temp1579;}); _temp1578;}); break;}} case 50: _LL1567: {
struct _tagged_arr _temp1581= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1582= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1581, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1581, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1583=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1583[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1584; _temp1584.tag=
Cyc_Absyn_Section_att; _temp1584.f1= _temp1582; _temp1584;}); _temp1583;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1585=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1585[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1586; _temp1586.tag= Cyc_Attribute_tok; _temp1586.f1=(
void*) a; _temp1586;}); _temp1585;}); break;} case 51: _LL1580: { struct
_tagged_arr _temp1588= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1589= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1592; struct _tuple15 _temp1590=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1593:
_temp1592= _temp1590.f2; goto _LL1591; _LL1591: { int _temp1596; struct _tuple15
_temp1594=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1597: _temp1596= _temp1594.f2; goto _LL1595; _LL1595: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1588, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1588, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1589, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1598=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1598[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1599; _temp1599.tag=
Cyc_Absyn_Format_att; _temp1599.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1599.f2= _temp1592; _temp1599.f3= _temp1596; _temp1599;}); _temp1598;});}
else{ if( Cyc_Std_zstrcmp( _temp1589, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1600=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1600[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1601; _temp1601.tag=
Cyc_Absyn_Format_att; _temp1601.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1601.f2= _temp1592; _temp1601.f3= _temp1596; _temp1601;}); _temp1600;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1602=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1602[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1603; _temp1603.tag= Cyc_Attribute_tok; _temp1603.f1=(
void*) a; _temp1603;}); _temp1602;}); break;}}} case 52: _LL1587: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1605=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1605[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1606; _temp1606.tag= Cyc_TypeSpecifier_tok;
_temp1606.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1606;});
_temp1605;}); break; case 53: _LL1604: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1608=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1608[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1609; _temp1609.tag=
Cyc_TypeSpecifier_tok; _temp1609.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1609;}); _temp1608;}); break; case 54: _LL1607:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1611=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1611[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1612; _temp1612.tag= Cyc_TypeSpecifier_tok;
_temp1612.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1612;});
_temp1611;}); break; case 55: _LL1610: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1614=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1614[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1615; _temp1615.tag=
Cyc_TypeSpecifier_tok; _temp1615.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1616=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1616[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1617; _temp1617.tag=
Cyc_Parse_Short_spec; _temp1617.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1617;});
_temp1616;})); _temp1615;}); _temp1614;}); break; case 56: _LL1613: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1619=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1619[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1620; _temp1620.tag= Cyc_TypeSpecifier_tok;
_temp1620.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1620;});
_temp1619;}); break; case 57: _LL1618: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1622=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1622[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1623; _temp1623.tag=
Cyc_TypeSpecifier_tok; _temp1623.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1624=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1624[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1625; _temp1625.tag= Cyc_Parse_Long_spec;
_temp1625.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1625;}); _temp1624;})); _temp1623;}); _temp1622;});
break; case 58: _LL1621: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1627=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1627[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1628; _temp1628.tag=
Cyc_TypeSpecifier_tok; _temp1628.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1628;}); _temp1627;}); break; case 59: _LL1626:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1630=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1630[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1631; _temp1631.tag= Cyc_TypeSpecifier_tok;
_temp1631.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1631;});
_temp1630;}); break; case 60: _LL1629: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1633=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1633[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1634; _temp1634.tag=
Cyc_TypeSpecifier_tok; _temp1634.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1635=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1635[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1636; _temp1636.tag=
Cyc_Parse_Signed_spec; _temp1636.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1636;});
_temp1635;})); _temp1634;}); _temp1633;}); break; case 61: _LL1632: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1638=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1638[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1639; _temp1639.tag= Cyc_TypeSpecifier_tok;
_temp1639.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1640=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1640[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1641; _temp1641.tag= Cyc_Parse_Unsigned_spec; _temp1641.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1641;});
_temp1640;})); _temp1639;}); _temp1638;}); break; case 62: _LL1637: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1642:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1643: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1644: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1646=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1646[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1647; _temp1647.tag=
Cyc_TypeSpecifier_tok; _temp1647.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1647;});
_temp1646;}); break; case 66: _LL1645: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1649=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1649[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1650; _temp1650.tag=
Cyc_TypeSpecifier_tok; _temp1650.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1651=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1651[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1652; _temp1652.tag= Cyc_Absyn_TypedefType;
_temp1652.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1652.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1652.f3= 0;
_temp1652;}); _temp1651;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1650;});
_temp1649;}); break; case 67: _LL1648: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1654=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1654[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1655; _temp1655.tag=
Cyc_TypeSpecifier_tok; _temp1655.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1656=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1656[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1657; _temp1657.tag= Cyc_Absyn_TupleType;
_temp1657.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1657;});
_temp1656;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1655;}); _temp1654;}); break; case 68:
_LL1653: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1659=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1659[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1660; _temp1660.tag=
Cyc_TypeSpecifier_tok; _temp1660.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1661=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1661[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1662; _temp1662.tag= Cyc_Absyn_RgnHandleType;
_temp1662.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1662;}); _temp1661;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1660;}); _temp1659;}); break; case 69: _LL1658: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1664=( struct Cyc_Kind_tok_struct*) _cycalloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1664[ 0]=({ struct Cyc_Kind_tok_struct _temp1665; _temp1665.tag= Cyc_Kind_tok;
_temp1665.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1665;});
_temp1664;}); break; case 70: _LL1663: { struct _tagged_arr* _temp1669; void*
_temp1671; struct _tuple1 _temp1667=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1672: _temp1671=
_temp1667.f1; goto _LL1670; _LL1670: _temp1669= _temp1667.f2; goto _LL1668;
_LL1668: if( _temp1671 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1673=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1673[ 0]=({ struct Cyc_Kind_tok_struct
_temp1674; _temp1674.tag= Cyc_Kind_tok; _temp1674.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1669, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1674;}); _temp1673;}); break;} case 71:
_LL1666: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1676=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1676[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1677; _temp1677.tag= Cyc_TypeQual_tok;
_temp1677.f1=({ struct Cyc_Absyn_Tqual _temp1678; _temp1678.q_const= 1;
_temp1678.q_volatile= 0; _temp1678.q_restrict= 0; _temp1678;}); _temp1677;});
_temp1676;}); break; case 72: _LL1675: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1680=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1680[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1681; _temp1681.tag= Cyc_TypeQual_tok; _temp1681.f1=({ struct Cyc_Absyn_Tqual
_temp1682; _temp1682.q_const= 0; _temp1682.q_volatile= 1; _temp1682.q_restrict=
0; _temp1682;}); _temp1681;}); _temp1680;}); break; case 73: _LL1679: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1684=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1684[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1685; _temp1685.tag= Cyc_TypeQual_tok;
_temp1685.f1=({ struct Cyc_Absyn_Tqual _temp1686; _temp1686.q_const= 0;
_temp1686.q_volatile= 0; _temp1686.q_restrict= 1; _temp1686;}); _temp1685;});
_temp1684;}); break; case 74: _LL1683: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1688=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1688[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1689; _temp1689.tag=
Cyc_TypeSpecifier_tok; _temp1689.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1690=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1690[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1691; _temp1691.tag= Cyc_Parse_Decl_spec;
_temp1691.f1=({ struct Cyc_Absyn_Decl* _temp1692=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1692->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1693=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1693[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1694; _temp1694.tag= Cyc_Absyn_Enum_d; _temp1694.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1695=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1695->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1695->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1695->fields=({
struct Cyc_Core_Opt* _temp1696=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1696->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1696;});
_temp1695;}); _temp1694;}); _temp1693;})); _temp1692->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1692;}); _temp1691;}); _temp1690;})); _temp1689;}); _temp1688;}); break;
case 75: _LL1687: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1698=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1698[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1699; _temp1699.tag=
Cyc_TypeSpecifier_tok; _temp1699.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1700=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1700[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1701; _temp1701.tag= Cyc_Absyn_EnumType;
_temp1701.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1701.f2= 0; _temp1701;}); _temp1700;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1699;}); _temp1698;}); break; case 76: _LL1697: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1703=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1703[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1704; _temp1704.tag= Cyc_Enumfield_tok;
_temp1704.f1=({ struct Cyc_Absyn_Enumfield* _temp1705=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1705->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1705->tag= 0;
_temp1705->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1705;});
_temp1704;}); _temp1703;}); break; case 77: _LL1702: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1707=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1707[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1708; _temp1708.tag= Cyc_Enumfield_tok;
_temp1708.f1=({ struct Cyc_Absyn_Enumfield* _temp1709=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1709->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1709->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1709->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1709;});
_temp1708;}); _temp1707;}); break; case 78: _LL1706: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1711=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1711[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1712; _temp1712.tag=
Cyc_EnumfieldList_tok; _temp1712.f1=({ struct Cyc_List_List* _temp1713=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1713->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1713->tl= 0; _temp1713;}); _temp1712;}); _temp1711;});
break; case 79: _LL1710: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1715=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1715[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1716; _temp1716.tag=
Cyc_EnumfieldList_tok; _temp1716.f1=({ struct Cyc_List_List* _temp1717=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1717->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1717->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1717;}); _temp1716;});
_temp1715;}); break; case 80: _LL1714: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1719: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1721=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1721[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1722; _temp1722.tag=
Cyc_Absyn_AnonStructType; _temp1722.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1722;});
_temp1721;}); break; case Cyc_Parse_Union_su: _LL1720: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1724=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1724[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1725; _temp1725.tag= Cyc_Absyn_AnonUnionType; _temp1725.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1725;});
_temp1724;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1726=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1726[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1727; _temp1727.tag=
Cyc_TypeSpecifier_tok; _temp1727.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1728=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1728[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1729; _temp1729.tag= Cyc_Parse_Type_spec;
_temp1729.f1=( void*) t; _temp1729.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1729;});
_temp1728;})); _temp1727;}); _temp1726;}); break;} case 81: _LL1718: { struct
Cyc_List_List* _temp1731=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1732: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1734=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1734->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1734;}), _temp1731,({ struct Cyc_Core_Opt*
_temp1735=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1735->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1735;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1733: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1737=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1737->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1737;}),
_temp1731,({ struct Cyc_Core_Opt* _temp1738=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1738->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1738;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1739=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1739[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1740; _temp1740.tag=
Cyc_TypeSpecifier_tok; _temp1740.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1741=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1741[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1742; _temp1742.tag= Cyc_Parse_Decl_spec;
_temp1742.f1= d; _temp1742;}); _temp1741;})); _temp1740;}); _temp1739;}); break;}
case 82: _LL1730: { struct Cyc_List_List* _temp1744=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1745: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1747=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1747->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1747;}), _temp1744,({ struct Cyc_Core_Opt*
_temp1748=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1748->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1748;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1746: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1750=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1750->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1750;}),
_temp1744,({ struct Cyc_Core_Opt* _temp1751=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1751->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1751;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1752=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1752[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1753; _temp1753.tag=
Cyc_TypeSpecifier_tok; _temp1753.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1754=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1754[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1755; _temp1755.tag= Cyc_Parse_Decl_spec;
_temp1755.f1= d; _temp1755;}); _temp1754;})); _temp1753;}); _temp1752;}); break;}
case 83: _LL1743: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1757: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1759=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1759[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1760; _temp1760.tag=
Cyc_TypeSpecifier_tok; _temp1760.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1761=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1761[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1762; _temp1762.tag= Cyc_Absyn_StructType;
_temp1762.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1762.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1762.f3= 0;
_temp1762;}); _temp1761;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1760;});
_temp1759;}); break; case Cyc_Parse_Union_su: _LL1758: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1764=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1764[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1765; _temp1765.tag= Cyc_TypeSpecifier_tok;
_temp1765.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1766=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1766[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1767; _temp1767.tag= Cyc_Absyn_UnionType;
_temp1767.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1767.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1767.f3= 0;
_temp1767;}); _temp1766;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1765;});
_temp1764;}); break;} break; case 84: _LL1756: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1769: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1771=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1771[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1772; _temp1772.tag=
Cyc_TypeSpecifier_tok; _temp1772.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1773=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1773[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1774; _temp1774.tag= Cyc_Absyn_StructType;
_temp1774.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1774.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1774.f3= 0;
_temp1774;}); _temp1773;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1772;});
_temp1771;}); break; case Cyc_Parse_Union_su: _LL1770: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1776=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1776[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1777; _temp1777.tag= Cyc_TypeSpecifier_tok;
_temp1777.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1778=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1778[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1779; _temp1779.tag= Cyc_Absyn_UnionType;
_temp1779.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1779.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1779.f3= 0;
_temp1779;}); _temp1778;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1777;});
_temp1776;}); break;} break; case 85: _LL1768: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1781=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1781[ 0]=({ struct Cyc_TypeList_tok_struct _temp1782; _temp1782.tag= Cyc_TypeList_tok;
_temp1782.f1= 0; _temp1782;}); _temp1781;}); break; case 86: _LL1780: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1784=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1784[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1785; _temp1785.tag= Cyc_TypeList_tok; _temp1785.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1785;});
_temp1784;}); break; case 87: _LL1783:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1787=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1787[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1788; _temp1788.tag= Cyc_TypeList_tok; _temp1788.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1788;});
_temp1787;}); break; case 88: _LL1786: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1790=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1790[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1791; _temp1791.tag= Cyc_StructOrUnion_tok; _temp1791.f1= Cyc_Parse_Struct_su;
_temp1791;}); _temp1790;}); break; case 89: _LL1789: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1793=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1793[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1794; _temp1794.tag= Cyc_StructOrUnion_tok; _temp1794.f1= Cyc_Parse_Union_su;
_temp1794;}); _temp1793;}); break; case 90: _LL1792: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1796=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1796[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1797; _temp1797.tag= Cyc_StructFieldDeclList_tok; _temp1797.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1797;});
_temp1796;}); break; case 91: _LL1795: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1799=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1799[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1800; _temp1800.tag= Cyc_StructFieldDeclListList_tok; _temp1800.f1=({
struct Cyc_List_List* _temp1801=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1801->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1801->tl= 0;
_temp1801;}); _temp1800;}); _temp1799;}); break; case 92: _LL1798: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1803=( struct Cyc_StructFieldDeclListList_tok_struct*)
_cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1803[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1804; _temp1804.tag= Cyc_StructFieldDeclListList_tok;
_temp1804.f1=({ struct Cyc_List_List* _temp1805=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1805->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1805->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1805;}); _temp1804;}); _temp1803;}); break;
case 93: _LL1802: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1807=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1807[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1808; _temp1808.tag=
Cyc_InitDeclList_tok; _temp1808.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1808;});
_temp1807;}); break; case 94: _LL1806: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1810=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1810[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1811; _temp1811.tag=
Cyc_InitDeclList_tok; _temp1811.f1=({ struct Cyc_List_List* _temp1812=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1812->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1812->tl= 0; _temp1812;}); _temp1811;}); _temp1810;});
break; case 95: _LL1809: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1814=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1814[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1815; _temp1815.tag=
Cyc_InitDeclList_tok; _temp1815.f1=({ struct Cyc_List_List* _temp1816=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1816->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1816->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1816;});
_temp1815;}); _temp1814;}); break; case 96: _LL1813: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1818=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1818[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1819; _temp1819.tag= Cyc_InitDecl_tok;
_temp1819.f1=({ struct _tuple14* _temp1820=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1820->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1820->f2= 0;
_temp1820;}); _temp1819;}); _temp1818;}); break; case 97: _LL1817: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1822=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1822[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1823; _temp1823.tag= Cyc_InitDecl_tok; _temp1823.f1=({ struct _tuple14*
_temp1824=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1824->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1824->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1824;});
_temp1823;}); _temp1822;}); break; case 98: _LL1821: { struct _tuple17 _temp1828;
struct Cyc_List_List* _temp1829; struct Cyc_List_List* _temp1831; struct Cyc_Absyn_Tqual
_temp1833; struct _tuple17* _temp1826= Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1828=*
_temp1826; _LL1834: _temp1833= _temp1828.f1; goto _LL1832; _LL1832: _temp1831=
_temp1828.f2; goto _LL1830; _LL1830: _temp1829= _temp1828.f3; goto _LL1827;
_LL1827: { struct Cyc_List_List* _temp1837; struct Cyc_List_List* _temp1839;
struct _tuple0 _temp1835=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1840: _temp1839= _temp1835.f1; goto _LL1838; _LL1838:
_temp1837= _temp1835.f2; goto _LL1836; _LL1836: { void* _temp1841= Cyc_Parse_speclist2typ(
_temp1831, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1842=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1833, _temp1841, _temp1839, _temp1829), _temp1837);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1843=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1843[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1844; _temp1844.tag=
Cyc_StructFieldDeclList_tok; _temp1844.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1842); _temp1844;}); _temp1843;}); break;}}}
case 99: _LL1825: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1846=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1846[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1847; _temp1847.tag=
Cyc_QualSpecList_tok; _temp1847.f1=({ struct _tuple17* _temp1848=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1848->f1= Cyc_Absyn_empty_tqual();
_temp1848->f2=({ struct Cyc_List_List* _temp1849=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1849->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1849->tl=
0; _temp1849;}); _temp1848->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1848;}); _temp1847;});
_temp1846;}); break; case 100: _LL1845: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1851=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1851[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1852; _temp1852.tag=
Cyc_QualSpecList_tok; _temp1852.f1=({ struct _tuple17* _temp1853=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1853->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1853->f2=({
struct Cyc_List_List* _temp1854=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1854->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1854->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1854;}); _temp1853->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1853;});
_temp1852;}); _temp1851;}); break; case 101: _LL1850: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1856=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1856[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1857; _temp1857.tag= Cyc_QualSpecList_tok;
_temp1857.f1=({ struct _tuple17* _temp1858=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1858->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1858->f2= 0;
_temp1858->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1858;}); _temp1857;}); _temp1856;}); break; case
102: _LL1855: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1860=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1860[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1861; _temp1861.tag=
Cyc_QualSpecList_tok; _temp1861.f1=({ struct _tuple17* _temp1862=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1862->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1862->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1862->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1862;});
_temp1861;}); _temp1860;}); break; case 103: _LL1859: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1864=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1864[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1865; _temp1865.tag= Cyc_InitDeclList_tok;
_temp1865.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1865;}); _temp1864;}); break; case 104: _LL1863: yyval=(
void*)({ struct Cyc_InitDeclList_tok_struct* _temp1867=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1867[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1868; _temp1868.tag= Cyc_InitDeclList_tok;
_temp1868.f1=({ struct Cyc_List_List* _temp1869=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1869->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1869->tl= 0;
_temp1869;}); _temp1868;}); _temp1867;}); break; case 105: _LL1866: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp1871=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1871[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1872; _temp1872.tag= Cyc_InitDeclList_tok;
_temp1872.f1=({ struct Cyc_List_List* _temp1873=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1873->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1873->tl=
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1873;}); _temp1872;}); _temp1871;}); break; case 106:
_LL1870: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp1875=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1875[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1876; _temp1876.tag= Cyc_InitDecl_tok; _temp1876.f1=({ struct _tuple14*
_temp1877=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1877->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1877->f2= 0; _temp1877;}); _temp1876;}); _temp1875;});
break; case 107: _LL1874: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1879=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1879[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1880; _temp1880.tag= Cyc_InitDecl_tok;
_temp1880.f1=({ struct _tuple14* _temp1881=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1881->f1=({ struct Cyc_Parse_Declarator*
_temp1882=( struct Cyc_Parse_Declarator*) _cycalloc( sizeof( struct Cyc_Parse_Declarator));
_temp1882->id=({ struct _tuple1* _temp1883=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp1883->f1= Cyc_Absyn_rel_ns_null; _temp1883->f2=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1883;}); _temp1882->tms= 0; _temp1882;});
_temp1881->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1881;}); _temp1880;});
_temp1879;}); break; case 108: _LL1878: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1885=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1885[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1886; _temp1886.tag= Cyc_InitDecl_tok;
_temp1886.f1=({ struct _tuple14* _temp1887=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1887->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1887->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1887;}); _temp1886;}); _temp1885;}); break; case
109: _LL1884: { struct Cyc_List_List* _temp1889=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1890=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1890[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1891; _temp1891.tag=
Cyc_TypeSpecifier_tok; _temp1891.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1892=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1892[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1893; _temp1893.tag= Cyc_Parse_Decl_spec;
_temp1893.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1889,({
struct Cyc_Core_Opt* _temp1894=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1894->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1894;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1893;}); _temp1892;})); _temp1891;}); _temp1890;}); break;} case 110:
_LL1888: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1896=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1896[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1897; _temp1897.tag=
Cyc_TypeSpecifier_tok; _temp1897.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1898=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1898[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1899; _temp1899.tag= Cyc_Absyn_TunionType;
_temp1899.f1=({ struct Cyc_Absyn_TunionInfo _temp1900; _temp1900.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1901=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1901[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1902; _temp1902.tag= Cyc_Absyn_UnknownTunion;
_temp1902.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1903; _temp1903.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1903.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1903;});
_temp1902;}); _temp1901;})); _temp1900.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1900.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1900;}); _temp1899;}); _temp1898;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1897;}); _temp1896;}); break; case 111: _LL1895: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1905=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1905[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1906; _temp1906.tag= Cyc_TypeSpecifier_tok;
_temp1906.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1907=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1907[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1908; _temp1908.tag=
Cyc_Absyn_TunionType; _temp1908.f1=({ struct Cyc_Absyn_TunionInfo _temp1909;
_temp1909.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1910=( struct Cyc_Absyn_UnknownTunion_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1910[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1911; _temp1911.tag= Cyc_Absyn_UnknownTunion; _temp1911.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1912; _temp1912.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1912.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1912;}); _temp1911;}); _temp1910;})); _temp1909.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1909.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp1909;}); _temp1908;}); _temp1907;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1906;}); _temp1905;}); break; case 112:
_LL1904: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1914=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1914[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1915; _temp1915.tag=
Cyc_TypeSpecifier_tok; _temp1915.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1916=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1916[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1917; _temp1917.tag= Cyc_Absyn_TunionFieldType;
_temp1917.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1918; _temp1918.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp1919=( struct
Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
_temp1919[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct _temp1920; _temp1920.tag=
Cyc_Absyn_UnknownTunionfield; _temp1920.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp1921; _temp1921.tunion_name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1921.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1921.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1921;});
_temp1920;}); _temp1919;})); _temp1918.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1918;}); _temp1917;});
_temp1916;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1915;}); _temp1914;}); break; case 113:
_LL1913: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1923=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1923[ 0]=({ struct
Cyc_Bool_tok_struct _temp1924; _temp1924.tag= Cyc_Bool_tok; _temp1924.f1= 0;
_temp1924;}); _temp1923;}); break; case 114: _LL1922: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1926=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1926[ 0]=({ struct Cyc_Bool_tok_struct
_temp1927; _temp1927.tag= Cyc_Bool_tok; _temp1927.f1= 1; _temp1927;}); _temp1926;});
break; case 115: _LL1925: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1929=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1929[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1930; _temp1930.tag= Cyc_TunionFieldList_tok; _temp1930.f1=({ struct Cyc_List_List*
_temp1931=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1931->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1931->tl= 0;
_temp1931;}); _temp1930;}); _temp1929;}); break; case 116: _LL1928: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp1933=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1933[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1934; _temp1934.tag= Cyc_TunionFieldList_tok;
_temp1934.f1=({ struct Cyc_List_List* _temp1935=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1935->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1935->tl=
0; _temp1935;}); _temp1934;}); _temp1933;}); break; case 117: _LL1932: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1937=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1937[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1938; _temp1938.tag= Cyc_TunionFieldList_tok;
_temp1938.f1=({ struct Cyc_List_List* _temp1939=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1939->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1939->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1939;}); _temp1938;}); _temp1937;}); break; case 118:
_LL1936: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1941=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp1941[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp1942; _temp1942.tag=
Cyc_TunionFieldList_tok; _temp1942.f1=({ struct Cyc_List_List* _temp1943=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1943->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1943->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1943;}); _temp1942;});
_temp1941;}); break; case 119: _LL1940: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1945=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1945[ 0]=({ struct Cyc_Scope_tok_struct _temp1946; _temp1946.tag= Cyc_Scope_tok;
_temp1946.f1=( void*)(( void*) Cyc_Absyn_Public); _temp1946;}); _temp1945;});
break; case 120: _LL1944: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1948=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1948[ 0]=({ struct Cyc_Scope_tok_struct _temp1949; _temp1949.tag= Cyc_Scope_tok;
_temp1949.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp1949;}); _temp1948;});
break; case 121: _LL1947: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1951=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1951[ 0]=({ struct Cyc_Scope_tok_struct _temp1952; _temp1952.tag= Cyc_Scope_tok;
_temp1952.f1=( void*)(( void*) Cyc_Absyn_Static); _temp1952;}); _temp1951;});
break; case 122: _LL1950: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp1954=( struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp1954[ 0]=({ struct Cyc_TunionField_tok_struct _temp1955; _temp1955.tag= Cyc_TunionField_tok;
_temp1955.f1=({ struct Cyc_Absyn_Tunionfield* _temp1956=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1956->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1956->tvs= 0;
_temp1956->typs= 0; _temp1956->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1956->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1956;}); _temp1955;}); _temp1954;}); break; case 123:
_LL1953: { struct Cyc_List_List* _temp1958=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp1959=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp1960=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp1960[ 0]=({ struct
Cyc_TunionField_tok_struct _temp1961; _temp1961.tag= Cyc_TunionField_tok;
_temp1961.f1=({ struct Cyc_Absyn_Tunionfield* _temp1962=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1962->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1962->tvs=
_temp1959; _temp1962->typs= _temp1958; _temp1962->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1962->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp1962;}); _temp1961;}); _temp1960;}); break;}
case 124: _LL1957: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 125: _LL1963: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1965=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1965[ 0]=({ struct Cyc_Declarator_tok_struct _temp1966; _temp1966.tag= Cyc_Declarator_tok;
_temp1966.f1=({ struct Cyc_Parse_Declarator* _temp1967=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1967->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp1967->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp1967;});
_temp1966;}); _temp1965;}); break; case 126: _LL1964: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp1969=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp1969[ 0]=({ struct
Cyc_Declarator_tok_struct _temp1970; _temp1970.tag= Cyc_Declarator_tok;
_temp1970.f1=({ struct Cyc_Parse_Declarator* _temp1971=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1971->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1971->tms= 0;
_temp1971;}); _temp1970;}); _temp1969;}); break; case 127: _LL1968: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 128:
_LL1972: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp1974=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1974[ 0]=({ struct Cyc_Declarator_tok_struct _temp1975; _temp1975.tag= Cyc_Declarator_tok;
_temp1975.f1=({ struct Cyc_Parse_Declarator* _temp1976=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1976->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp1976->tms=({ struct Cyc_List_List* _temp1977=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1977->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp1977->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp1977;}); _temp1976;}); _temp1975;});
_temp1974;}); break; case 129: _LL1973: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1979=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1979[ 0]=({ struct Cyc_Declarator_tok_struct _temp1980; _temp1980.tag= Cyc_Declarator_tok;
_temp1980.f1=({ struct Cyc_Parse_Declarator* _temp1981=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1981->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp1981->tms=({ struct Cyc_List_List* _temp1982=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1982->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp1983=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1983[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1984; _temp1984.tag= Cyc_Absyn_ConstArray_mod;
_temp1984.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1984;}); _temp1983;})); _temp1982->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp1982;}); _temp1981;}); _temp1980;}); _temp1979;}); break; case 130: _LL1978: {
struct _tuple16 _temp1988; struct Cyc_List_List* _temp1989; struct Cyc_Core_Opt*
_temp1991; struct Cyc_Absyn_VarargInfo* _temp1993; int _temp1995; struct Cyc_List_List*
_temp1997; struct _tuple16* _temp1986= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1988=*
_temp1986; _LL1998: _temp1997= _temp1988.f1; goto _LL1996; _LL1996: _temp1995=
_temp1988.f2; goto _LL1994; _LL1994: _temp1993= _temp1988.f3; goto _LL1992;
_LL1992: _temp1991= _temp1988.f4; goto _LL1990; _LL1990: _temp1989= _temp1988.f5;
goto _LL1987; _LL1987: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1999=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1999[ 0]=({ struct Cyc_Declarator_tok_struct _temp2000; _temp2000.tag= Cyc_Declarator_tok;
_temp2000.f1=({ struct Cyc_Parse_Declarator* _temp2001=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2001->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2001->tms=({ struct Cyc_List_List* _temp2002=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2002->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2003=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2003[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2004; _temp2004.tag= Cyc_Absyn_Function_mod;
_temp2004.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2005=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2005[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2006; _temp2006.tag= Cyc_Absyn_WithTypes;
_temp2006.f1= _temp1997; _temp2006.f2= _temp1995; _temp2006.f3= _temp1993;
_temp2006.f4= _temp1991; _temp2006.f5= _temp1989; _temp2006;}); _temp2005;}));
_temp2004;}); _temp2003;})); _temp2002->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2002;});
_temp2001;}); _temp2000;}); _temp1999;}); break;} case 131: _LL1985: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2008=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2008[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2009; _temp2009.tag= Cyc_Declarator_tok;
_temp2009.f1=({ struct Cyc_Parse_Declarator* _temp2010=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2010->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2010->tms=({ struct Cyc_List_List* _temp2011=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2011->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2012=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2012[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2013; _temp2013.tag= Cyc_Absyn_Function_mod;
_temp2013.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2014=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2014[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2015; _temp2015.tag= Cyc_Absyn_WithTypes;
_temp2015.f1= 0; _temp2015.f2= 0; _temp2015.f3= 0; _temp2015.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2015.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2015;}); _temp2014;})); _temp2013;}); _temp2012;}));
_temp2011->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2011;}); _temp2010;}); _temp2009;});
_temp2008;}); break; case 132: _LL2007: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2017=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2017[ 0]=({ struct Cyc_Declarator_tok_struct _temp2018; _temp2018.tag= Cyc_Declarator_tok;
_temp2018.f1=({ struct Cyc_Parse_Declarator* _temp2019=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2019->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2019->tms=({ struct Cyc_List_List* _temp2020=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2020->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2021=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2021[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2022; _temp2022.tag= Cyc_Absyn_Function_mod;
_temp2022.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2023=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2023[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2024; _temp2024.tag= Cyc_Absyn_NoTypes;
_temp2024.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2024.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2024;});
_temp2023;})); _temp2022;}); _temp2021;})); _temp2020->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2020;}); _temp2019;}); _temp2018;}); _temp2017;}); break; case 133: _LL2016: {
struct Cyc_List_List* _temp2026=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2027=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2027[ 0]=({ struct Cyc_Declarator_tok_struct _temp2028; _temp2028.tag= Cyc_Declarator_tok;
_temp2028.f1=({ struct Cyc_Parse_Declarator* _temp2029=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2029->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2029->tms=({ struct Cyc_List_List* _temp2030=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2030->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2031=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2031[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2032; _temp2032.tag= Cyc_Absyn_TypeParams_mod;
_temp2032.f1= _temp2026; _temp2032.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2032.f3=
0; _temp2032;}); _temp2031;})); _temp2030->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2030;});
_temp2029;}); _temp2028;}); _temp2027;}); break;} case 134: _LL2025:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2034=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2035=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2035[ 0]=({ struct Cyc_Declarator_tok_struct _temp2036; _temp2036.tag= Cyc_Declarator_tok;
_temp2036.f1=({ struct Cyc_Parse_Declarator* _temp2037=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2037->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2037->tms=({ struct Cyc_List_List* _temp2038=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2038->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2039=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2039[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2040; _temp2040.tag= Cyc_Absyn_TypeParams_mod;
_temp2040.f1= _temp2034; _temp2040.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2040.f3=
0; _temp2040;}); _temp2039;})); _temp2038->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2038;});
_temp2037;}); _temp2036;}); _temp2035;}); break;} case 135: _LL2033: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2042=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2042[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2043; _temp2043.tag= Cyc_Declarator_tok;
_temp2043.f1=({ struct Cyc_Parse_Declarator* _temp2044=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2044->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2044->tms=({ struct Cyc_List_List* _temp2045=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2045->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2046=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2046[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2047; _temp2047.tag= Cyc_Absyn_Attributes_mod;
_temp2047.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2047.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2047;}); _temp2046;}));
_temp2045->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]))->tms; _temp2045;}); _temp2044;}); _temp2043;});
_temp2042;}); break; case 136: _LL2041: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2049=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2049[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2050; _temp2050.tag= Cyc_Declarator_tok;
_temp2050.f1=({ struct Cyc_Parse_Declarator* _temp2051=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2051->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2051->tms= 0;
_temp2051;}); _temp2050;}); _temp2049;}); break; case 137: _LL2048: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2053=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2053[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2054; _temp2054.tag= Cyc_Declarator_tok;
_temp2054.f1=({ struct Cyc_Parse_Declarator* _temp2055=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2055->id= Cyc_Absyn_exn_name;
_temp2055->tms= 0; _temp2055;}); _temp2054;}); _temp2053;}); break; case 138:
_LL2052: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2057=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2057[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2058; _temp2058.tag=
Cyc_TypeModifierList_tok; _temp2058.f1=({ struct Cyc_List_List* _temp2059=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2059->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2060=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2060[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2061; _temp2061.tag= Cyc_Absyn_Pointer_mod;
_temp2061.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2061.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2061.f3= Cyc_Absyn_empty_tqual(); _temp2061;});
_temp2060;})); _temp2059->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2059;});
_temp2058;}); _temp2057;}); break; case 139: _LL2056: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2063=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2063[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2064; _temp2064.tag= Cyc_TypeModifierList_tok;
_temp2064.f1=({ struct Cyc_List_List* _temp2065=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2065->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2066=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2066[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2067; _temp2067.tag= Cyc_Absyn_Pointer_mod;
_temp2067.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2067.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2067.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2067;}); _temp2066;}));
_temp2065->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0);
_temp2065;}); _temp2064;}); _temp2063;}); break; case 140: _LL2062: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2069=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2069[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2070; _temp2070.tag= Cyc_TypeModifierList_tok;
_temp2070.f1=({ struct Cyc_List_List* _temp2071=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2071->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2072=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2072[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2073; _temp2073.tag= Cyc_Absyn_Pointer_mod;
_temp2073.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2073.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2073.f3= Cyc_Absyn_empty_tqual(); _temp2073;});
_temp2072;})); _temp2071->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2071;});
_temp2070;}); _temp2069;}); break; case 141: _LL2068: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2075=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2075[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2076; _temp2076.tag= Cyc_TypeModifierList_tok;
_temp2076.f1=({ struct Cyc_List_List* _temp2077=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2077->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2078=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2078[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2079; _temp2079.tag= Cyc_Absyn_Pointer_mod;
_temp2079.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2079.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2079.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2079;});
_temp2078;})); _temp2077->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2077;});
_temp2076;}); _temp2075;}); break; case 142: _LL2074: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2081=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2081[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2082; _temp2082.tag= Cyc_Pointer_Sort_tok;
_temp2082.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2083=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2083[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2084; _temp2084.tag=
Cyc_Absyn_Nullable_ps; _temp2084.f1= Cyc_Absyn_exp_unsigned_one; _temp2084;});
_temp2083;})); _temp2082;}); _temp2081;}); break; case 143: _LL2080: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2086=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2086[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2087; _temp2087.tag= Cyc_Pointer_Sort_tok;
_temp2087.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2088=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2088[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2089; _temp2089.tag= Cyc_Absyn_NonNullable_ps; _temp2089.f1= Cyc_Absyn_exp_unsigned_one;
_temp2089;}); _temp2088;})); _temp2087;}); _temp2086;}); break; case 144:
_LL2085: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2091=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2091[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2092; _temp2092.tag=
Cyc_Pointer_Sort_tok; _temp2092.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2093=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2093[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2094; _temp2094.tag=
Cyc_Absyn_Nullable_ps; _temp2094.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2094;});
_temp2093;})); _temp2092;}); _temp2091;}); break; case 145: _LL2090: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2096=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2096[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2097; _temp2097.tag= Cyc_Pointer_Sort_tok;
_temp2097.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2098=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2098[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2099; _temp2099.tag= Cyc_Absyn_NonNullable_ps; _temp2099.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2099;});
_temp2098;})); _temp2097;}); _temp2096;}); break; case 146: _LL2095: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2101=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2101[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2102; _temp2102.tag= Cyc_Pointer_Sort_tok;
_temp2102.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2102;});
_temp2101;}); break; case 147: _LL2100: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2104=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2104[ 0]=({ struct Cyc_Type_tok_struct _temp2105; _temp2105.tag= Cyc_Type_tok;
_temp2105.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2106=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2106->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2106;}), 0); _temp2105;}); _temp2104;});
break; case 148: _LL2103: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 149: _LL2107: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
150: _LL2108: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2110=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2110[
0]=({ struct Cyc_Type_tok_struct _temp2111; _temp2111.tag= Cyc_Type_tok;
_temp2111.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2112=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2112->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2112;}), 0); _temp2111;}); _temp2110;});
break; case 151: _LL2109: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 152: _LL2113: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2115=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2115[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2116; _temp2116.tag= Cyc_TypeQual_tok; _temp2116.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2116;});
_temp2115;}); break; case 153: _LL2114: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2118=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2118[ 0]=({ struct Cyc_YY1_struct _temp2119; _temp2119.tag= Cyc_YY1;
_temp2119.f1=({ struct _tuple16* _temp2120=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2120->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2120->f2= 0;
_temp2120->f3= 0; _temp2120->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2120->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2120;});
_temp2119;}); _temp2118;}); break; case 154: _LL2117: yyval=( void*)({ struct
Cyc_YY1_struct* _temp2122=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct
Cyc_YY1_struct)); _temp2122[ 0]=({ struct Cyc_YY1_struct _temp2123; _temp2123.tag=
Cyc_YY1; _temp2123.f1=({ struct _tuple16* _temp2124=( struct _tuple16*)
_cycalloc( sizeof( struct _tuple16)); _temp2124->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2124->f2= 1;
_temp2124->f3= 0; _temp2124->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2124->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2124;});
_temp2123;}); _temp2122;}); break; case 155: _LL2121: { struct _tuple2 _temp2128;
void* _temp2129; struct Cyc_Absyn_Tqual _temp2131; struct Cyc_Core_Opt*
_temp2133; struct _tuple2* _temp2126= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2128=*
_temp2126; _LL2134: _temp2133= _temp2128.f1; goto _LL2132; _LL2132: _temp2131=
_temp2128.f2; goto _LL2130; _LL2130: _temp2129= _temp2128.f3; goto _LL2127;
_LL2127: { struct Cyc_Absyn_VarargInfo* _temp2135=({ struct Cyc_Absyn_VarargInfo*
_temp2139=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2139->name= _temp2133; _temp2139->tq= _temp2131; _temp2139->type=( void*)
_temp2129; _temp2139->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2139;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2136=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2136[ 0]=({ struct Cyc_YY1_struct
_temp2137; _temp2137.tag= Cyc_YY1; _temp2137.f1=({ struct _tuple16* _temp2138=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2138->f1= 0;
_temp2138->f2= 0; _temp2138->f3= _temp2135; _temp2138->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2138->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2138;}); _temp2137;}); _temp2136;}); break;}} case 156:
_LL2125: { struct _tuple2 _temp2143; void* _temp2144; struct Cyc_Absyn_Tqual
_temp2146; struct Cyc_Core_Opt* _temp2148; struct _tuple2* _temp2141= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2143=*
_temp2141; _LL2149: _temp2148= _temp2143.f1; goto _LL2147; _LL2147: _temp2146=
_temp2143.f2; goto _LL2145; _LL2145: _temp2144= _temp2143.f3; goto _LL2142;
_LL2142: { struct Cyc_Absyn_VarargInfo* _temp2150=({ struct Cyc_Absyn_VarargInfo*
_temp2154=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2154->name= _temp2148; _temp2154->tq= _temp2146; _temp2154->type=( void*)
_temp2144; _temp2154->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2154;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2151=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2151[ 0]=({ struct Cyc_YY1_struct
_temp2152; _temp2152.tag= Cyc_YY1; _temp2152.f1=({ struct _tuple16* _temp2153=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2153->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2153->f2=
0; _temp2153->f3= _temp2150; _temp2153->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2153->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2153;});
_temp2152;}); _temp2151;}); break;}} case 157: _LL2140: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2156=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2156[ 0]=({ struct Cyc_Type_tok_struct
_temp2157; _temp2157.tag= Cyc_Type_tok; _temp2157.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()); _temp2157;}); _temp2156;}); break; case 158: _LL2155:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2159=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2159[ 0]=({ struct Cyc_Type_tok_struct
_temp2160; _temp2160.tag= Cyc_Type_tok; _temp2160.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp2160;});
_temp2159;}); break; case 159: _LL2158: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2162=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2162[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2163; _temp2163.tag= Cyc_TypeOpt_tok;
_temp2163.f1= 0; _temp2163;}); _temp2162;}); break; case 160: _LL2161: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2165=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2165[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2166; _temp2166.tag= Cyc_TypeOpt_tok; _temp2166.f1=({ struct Cyc_Core_Opt*
_temp2167=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2167->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2168=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2168[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2169; _temp2169.tag= Cyc_Absyn_JoinEff;
_temp2169.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2169;}); _temp2168;})); _temp2167;}); _temp2166;});
_temp2165;}); break; case 161: _LL2164: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2171=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2171[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2172; _temp2172.tag= Cyc_Rgnorder_tok;
_temp2172.f1= 0; _temp2172;}); _temp2171;}); break; case 162: _LL2170: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL2173: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2175=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2175[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2176; _temp2176.tag= Cyc_Rgnorder_tok; _temp2176.f1=({ struct Cyc_List_List*
_temp2177=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2177->hd=( void*)({ struct _tuple18* _temp2178=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2178->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2178->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2178;}); _temp2177->tl= 0; _temp2177;});
_temp2176;}); _temp2175;}); break; case 164: _LL2174: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2180=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2180[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2181; _temp2181.tag= Cyc_Rgnorder_tok; _temp2181.f1=({ struct Cyc_List_List*
_temp2182=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2182->hd=( void*)({ struct _tuple18* _temp2183=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2183->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2183->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2183;}); _temp2182->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2182;});
_temp2181;}); _temp2180;}); break; case 165: _LL2179: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2185=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2185[ 0]=({ struct Cyc_Bool_tok_struct
_temp2186; _temp2186.tag= Cyc_Bool_tok; _temp2186.f1= 0; _temp2186;}); _temp2185;});
break; case 166: _LL2184: { struct _tagged_arr _temp2188= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Std_zstrcmp(
_temp2188, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2189=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2189[ 0]=({ struct Cyc_Bool_tok_struct _temp2190; _temp2190.tag= Cyc_Bool_tok;
_temp2190.f1= 1; _temp2190;}); _temp2189;}); break;} case 167: _LL2187: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168:
_LL2191: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2193=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2193[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2194; _temp2194.tag= Cyc_TypeList_tok; _temp2194.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2194;});
_temp2193;}); break; case 169: _LL2192: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2196=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2196[ 0]=({ struct Cyc_TypeList_tok_struct _temp2197; _temp2197.tag= Cyc_TypeList_tok;
_temp2197.f1= 0; _temp2197;}); _temp2196;}); break; case 170: _LL2195: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case
171: _LL2198: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2200=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2200[ 0]=({ struct Cyc_TypeList_tok_struct _temp2201; _temp2201.tag= Cyc_TypeList_tok;
_temp2201.f1=({ struct Cyc_List_List* _temp2202=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2202->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp2203=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp2203[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp2204; _temp2204.tag= Cyc_Absyn_RgnsEff; _temp2204.f1=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2204;}); _temp2203;})); _temp2202->tl= 0; _temp2202;});
_temp2201;}); _temp2200;}); break; case 172: _LL2199: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2206=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2206[ 0]=({ struct Cyc_TypeList_tok_struct _temp2207; _temp2207.tag= Cyc_TypeList_tok;
_temp2207.f1=({ struct Cyc_List_List* _temp2208=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2208->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2208->tl= 0;
_temp2208;}); _temp2207;}); _temp2206;}); break; case 173: _LL2205: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2210=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2210[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2211; _temp2211.tag= Cyc_TypeList_tok; _temp2211.f1=({ struct Cyc_List_List*
_temp2212=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2212->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2213=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2213[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2214; _temp2214.tag= Cyc_Absyn_AccessEff;
_temp2214.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2214;}); _temp2213;})); _temp2212->tl= 0;
_temp2212;}); _temp2211;}); _temp2210;}); break; case 174: _LL2209: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),( void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2216=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2216[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2217; _temp2217.tag= Cyc_TypeList_tok; _temp2217.f1=({ struct Cyc_List_List*
_temp2218=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2218->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2219=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2219[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2220; _temp2220.tag= Cyc_Absyn_AccessEff;
_temp2220.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2220;}); _temp2219;})); _temp2218->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2218;});
_temp2217;}); _temp2216;}); break; case 175: _LL2215: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2222=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2222[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2223; _temp2223.tag= Cyc_ParamDeclList_tok;
_temp2223.f1=({ struct Cyc_List_List* _temp2224=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2224->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2224->tl= 0;
_temp2224;}); _temp2223;}); _temp2222;}); break; case 176: _LL2221: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2226=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2226[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2227; _temp2227.tag= Cyc_ParamDeclList_tok;
_temp2227.f1=({ struct Cyc_List_List* _temp2228=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2228->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2228->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2228;}); _temp2227;}); _temp2226;}); break; case 177:
_LL2225: { struct _tuple17 _temp2232; struct Cyc_List_List* _temp2233; struct
Cyc_List_List* _temp2235; struct Cyc_Absyn_Tqual _temp2237; struct _tuple17*
_temp2230= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2232=* _temp2230; _LL2238: _temp2237=
_temp2232.f1; goto _LL2236; _LL2236: _temp2235= _temp2232.f2; goto _LL2234;
_LL2234: _temp2233= _temp2232.f3; goto _LL2231; _LL2231: { struct Cyc_Parse_Declarator
_temp2241; struct Cyc_List_List* _temp2242; struct _tuple1* _temp2244; struct
Cyc_Parse_Declarator* _temp2239= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2241=* _temp2239;
_LL2245: _temp2244= _temp2241.id; goto _LL2243; _LL2243: _temp2242= _temp2241.tms;
goto _LL2240; _LL2240: { void* _temp2246= Cyc_Parse_speclist2typ( _temp2235, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2249; struct Cyc_List_List* _temp2251; void*
_temp2253; struct Cyc_Absyn_Tqual _temp2255; struct _tuple6 _temp2247= Cyc_Parse_apply_tms(
_temp2237, _temp2246, _temp2233, _temp2242); _LL2256: _temp2255= _temp2247.f1;
goto _LL2254; _LL2254: _temp2253= _temp2247.f2; goto _LL2252; _LL2252: _temp2251=
_temp2247.f3; goto _LL2250; _LL2250: _temp2249= _temp2247.f4; goto _LL2248;
_LL2248: if( _temp2251 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2244)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2257=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2261=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2261->v=( void*)(* _temp2244).f2; _temp2261;}); if( _temp2249 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2258=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2258[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2259; _temp2259.tag= Cyc_ParamDecl_tok;
_temp2259.f1=({ struct _tuple2* _temp2260=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2260->f1= _temp2257; _temp2260->f2= _temp2255; _temp2260->f3=
_temp2253; _temp2260;}); _temp2259;}); _temp2258;}); break;}}}} case 178:
_LL2229: { struct _tuple17 _temp2265; struct Cyc_List_List* _temp2266; struct
Cyc_List_List* _temp2268; struct Cyc_Absyn_Tqual _temp2270; struct _tuple17*
_temp2263= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2265=* _temp2263; _LL2271: _temp2270= _temp2265.f1;
goto _LL2269; _LL2269: _temp2268= _temp2265.f2; goto _LL2267; _LL2267: _temp2266=
_temp2265.f3; goto _LL2264; _LL2264: { void* _temp2272= Cyc_Parse_speclist2typ(
_temp2268, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2266 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2273=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2273[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2274; _temp2274.tag= Cyc_ParamDecl_tok; _temp2274.f1=({
struct _tuple2* _temp2275=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2275->f1= 0; _temp2275->f2= _temp2270; _temp2275->f3= _temp2272; _temp2275;});
_temp2274;}); _temp2273;}); break;}} case 179: _LL2262: { struct _tuple17
_temp2279; struct Cyc_List_List* _temp2280; struct Cyc_List_List* _temp2282;
struct Cyc_Absyn_Tqual _temp2284; struct _tuple17* _temp2277= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2279=*
_temp2277; _LL2285: _temp2284= _temp2279.f1; goto _LL2283; _LL2283: _temp2282=
_temp2279.f2; goto _LL2281; _LL2281: _temp2280= _temp2279.f3; goto _LL2278;
_LL2278: { void* _temp2286= Cyc_Parse_speclist2typ( _temp2282, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2287=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2290; struct Cyc_List_List* _temp2292; void* _temp2294; struct Cyc_Absyn_Tqual
_temp2296; struct _tuple6 _temp2288= Cyc_Parse_apply_tms( _temp2284, _temp2286,
_temp2280, _temp2287); _LL2297: _temp2296= _temp2288.f1; goto _LL2295; _LL2295:
_temp2294= _temp2288.f2; goto _LL2293; _LL2293: _temp2292= _temp2288.f3; goto
_LL2291; _LL2291: _temp2290= _temp2288.f4; goto _LL2289; _LL2289: if( _temp2292
!=  0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2290 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2298=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2298[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2299; _temp2299.tag= Cyc_ParamDecl_tok; _temp2299.f1=({
struct _tuple2* _temp2300=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2300->f1= 0; _temp2300->f2= _temp2296; _temp2300->f3= _temp2294; _temp2300;});
_temp2299;}); _temp2298;}); break;}} case 180: _LL2276: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2302=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2302[ 0]=({ struct Cyc_IdList_tok_struct
_temp2303; _temp2303.tag= Cyc_IdList_tok; _temp2303.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2303;});
_temp2302;}); break; case 181: _LL2301: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2305=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2305[ 0]=({ struct Cyc_IdList_tok_struct _temp2306; _temp2306.tag= Cyc_IdList_tok;
_temp2306.f1=({ struct Cyc_List_List* _temp2307=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2307->hd=( void*)({ struct
_tagged_arr* _temp2308=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2308[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2308;}); _temp2307->tl=
0; _temp2307;}); _temp2306;}); _temp2305;}); break; case 182: _LL2304: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2310=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2310[ 0]=({ struct Cyc_IdList_tok_struct
_temp2311; _temp2311.tag= Cyc_IdList_tok; _temp2311.f1=({ struct Cyc_List_List*
_temp2312=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2312->hd=( void*)({ struct _tagged_arr* _temp2313=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2313[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2313;});
_temp2312->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2312;}); _temp2311;}); _temp2310;}); break; case 183:
_LL2309: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 184: _LL2314: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 185: _LL2315: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2317=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2317[ 0]=({ struct Cyc_Exp_tok_struct _temp2318; _temp2318.tag= Cyc_Exp_tok;
_temp2318.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2319=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2319[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2320; _temp2320.tag= Cyc_Absyn_UnresolvedMem_e; _temp2320.f1= 0; _temp2320.f2=
0; _temp2320;}); _temp2319;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2318;});
_temp2317;}); break; case 186: _LL2316: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2322=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2322[ 0]=({ struct Cyc_Exp_tok_struct _temp2323; _temp2323.tag= Cyc_Exp_tok;
_temp2323.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2324=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2324[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2325; _temp2325.tag= Cyc_Absyn_UnresolvedMem_e; _temp2325.f1= 0; _temp2325.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2325;});
_temp2324;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2323;}); _temp2322;}); break; case 187:
_LL2321: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2327=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2327[ 0]=({ struct Cyc_Exp_tok_struct
_temp2328; _temp2328.tag= Cyc_Exp_tok; _temp2328.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2329=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2329[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2330; _temp2330.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2330.f1= 0; _temp2330.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2330;});
_temp2329;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2328;}); _temp2327;}); break; case 188:
_LL2326: { struct Cyc_Absyn_Vardecl* _temp2332= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2338=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2338->f1=( void*) Cyc_Absyn_Loc_n; _temp2338->f2=({ struct _tagged_arr*
_temp2339=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp2339[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); _temp2339;}); _temp2338;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp2332->tq=({ struct Cyc_Absyn_Tqual _temp2333; _temp2333.q_const= 1;
_temp2333.q_volatile= 0; _temp2333.q_restrict= 1; _temp2333;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2334=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2334[ 0]=({ struct Cyc_Exp_tok_struct
_temp2335; _temp2335.tag= Cyc_Exp_tok; _temp2335.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2336=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2336[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2337; _temp2337.tag= Cyc_Absyn_Comprehension_e;
_temp2337.f1= _temp2332; _temp2337.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2337.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2337;});
_temp2336;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2335;}); _temp2334;}); break;} case
189: _LL2331: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2341=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2341[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2342; _temp2342.tag=
Cyc_InitializerList_tok; _temp2342.f1=({ struct Cyc_List_List* _temp2343=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2343->hd=(
void*)({ struct _tuple19* _temp2344=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2344->f1= 0; _temp2344->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2344;}); _temp2343->tl=
0; _temp2343;}); _temp2342;}); _temp2341;}); break; case 190: _LL2340: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2346=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2346[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2347; _temp2347.tag= Cyc_InitializerList_tok;
_temp2347.f1=({ struct Cyc_List_List* _temp2348=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2348->hd=( void*)({ struct
_tuple19* _temp2349=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2349->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2349->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2349;});
_temp2348->tl= 0; _temp2348;}); _temp2347;}); _temp2346;}); break; case 191:
_LL2345: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2351=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2351[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2352; _temp2352.tag=
Cyc_InitializerList_tok; _temp2352.f1=({ struct Cyc_List_List* _temp2353=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2353->hd=(
void*)({ struct _tuple19* _temp2354=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2354->f1= 0; _temp2354->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2354;}); _temp2353->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2353;}); _temp2352;}); _temp2351;}); break; case 192:
_LL2350: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2356=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2356[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2357; _temp2357.tag=
Cyc_InitializerList_tok; _temp2357.f1=({ struct Cyc_List_List* _temp2358=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2358->hd=(
void*)({ struct _tuple19* _temp2359=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2359->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2359->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2359;});
_temp2358->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2358;});
_temp2357;}); _temp2356;}); break; case 193: _LL2355: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2361=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2361[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2362; _temp2362.tag= Cyc_DesignatorList_tok;
_temp2362.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2362;});
_temp2361;}); break; case 194: _LL2360: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2364=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2364[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2365; _temp2365.tag=
Cyc_DesignatorList_tok; _temp2365.f1=({ struct Cyc_List_List* _temp2366=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2366->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2366->tl= 0; _temp2366;}); _temp2365;}); _temp2364;});
break; case 195: _LL2363: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2368=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2368[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2369; _temp2369.tag=
Cyc_DesignatorList_tok; _temp2369.f1=({ struct Cyc_List_List* _temp2370=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2370->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2370->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2370;});
_temp2369;}); _temp2368;}); break; case 196: _LL2367: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2372=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2372[ 0]=({ struct
Cyc_Designator_tok_struct _temp2373; _temp2373.tag= Cyc_Designator_tok;
_temp2373.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2374=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2374[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2375; _temp2375.tag=
Cyc_Absyn_ArrayElement; _temp2375.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2375;});
_temp2374;})); _temp2373;}); _temp2372;}); break; case 197: _LL2371: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2377=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2377[ 0]=({ struct
Cyc_Designator_tok_struct _temp2378; _temp2378.tag= Cyc_Designator_tok;
_temp2378.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2379=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2379[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2380; _temp2380.tag= Cyc_Absyn_FieldName;
_temp2380.f1=({ struct _tagged_arr* _temp2381=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2381[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2381;}); _temp2380;});
_temp2379;})); _temp2378;}); _temp2377;}); break; case 198: _LL2376: { void*
_temp2383= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2384=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2384 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2385=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2386=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2386[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2387; _temp2387.tag= Cyc_ParamDecl_tok; _temp2387.f1=({
struct _tuple2* _temp2388=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2388->f1= 0; _temp2388->f2= _temp2385; _temp2388->f3= _temp2383; _temp2388;});
_temp2387;}); _temp2386;}); break;}} case 199: _LL2382: { void* _temp2390= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp2391=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2392=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2393=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2394= Cyc_Parse_apply_tms( _temp2392, _temp2390, _temp2391,
_temp2393); if( _temp2394.f3 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2394.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2395=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2395[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2396; _temp2396.tag= Cyc_ParamDecl_tok; _temp2396.f1=({
struct _tuple2* _temp2397=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2397->f1= 0; _temp2397->f2= _temp2394.f1; _temp2397->f3= _temp2394.f2;
_temp2397;}); _temp2396;}); _temp2395;}); break;} case 200: _LL2389: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2399=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2399[ 0]=({ struct Cyc_Type_tok_struct
_temp2400; _temp2400.tag= Cyc_Type_tok; _temp2400.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2400;});
_temp2399;}); break; case 201: _LL2398: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2402=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2402[ 0]=({ struct Cyc_Type_tok_struct _temp2403; _temp2403.tag= Cyc_Type_tok;
_temp2403.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2404=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2404[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2405; _temp2405.tag= Cyc_Absyn_JoinEff;
_temp2405.f1= 0; _temp2405;}); _temp2404;})); _temp2403;}); _temp2402;}); break;
case 202: _LL2401: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2407=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2407[ 0]=({ struct Cyc_Type_tok_struct _temp2408; _temp2408.tag= Cyc_Type_tok;
_temp2408.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2409=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2409[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2410; _temp2410.tag= Cyc_Absyn_JoinEff;
_temp2410.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2410;}); _temp2409;})); _temp2408;});
_temp2407;}); break; case 203: _LL2406: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2412=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2412[ 0]=({ struct Cyc_Type_tok_struct _temp2413; _temp2413.tag= Cyc_Type_tok;
_temp2413.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2414=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2414[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2415; _temp2415.tag= Cyc_Absyn_RgnsEff;
_temp2415.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2415;}); _temp2414;})); _temp2413;});
_temp2412;}); break; case 204: _LL2411: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2417=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2417[ 0]=({ struct Cyc_Type_tok_struct _temp2418; _temp2418.tag= Cyc_Type_tok;
_temp2418.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2419=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2419[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2420; _temp2420.tag= Cyc_Absyn_JoinEff;
_temp2420.f1=({ struct Cyc_List_List* _temp2421=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2421->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2421->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2421;}); _temp2420;}); _temp2419;})); _temp2418;});
_temp2417;}); break; case 205: _LL2416: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2423=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2423[ 0]=({ struct Cyc_TypeList_tok_struct _temp2424; _temp2424.tag= Cyc_TypeList_tok;
_temp2424.f1=({ struct Cyc_List_List* _temp2425=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2425->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2425->tl= 0;
_temp2425;}); _temp2424;}); _temp2423;}); break; case 206: _LL2422: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2427=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2427[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2428; _temp2428.tag= Cyc_TypeList_tok; _temp2428.f1=({ struct Cyc_List_List*
_temp2429=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2429->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2429->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2429;});
_temp2428;}); _temp2427;}); break; case 207: _LL2426: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2431=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2431[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2432; _temp2432.tag= Cyc_AbstractDeclarator_tok;
_temp2432.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2433=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2433->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2433;});
_temp2432;}); _temp2431;}); break; case 208: _LL2430: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 209: _LL2434:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2436=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2436[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2437; _temp2437.tag=
Cyc_AbstractDeclarator_tok; _temp2437.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2438=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2438->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2438;});
_temp2437;}); _temp2436;}); break; case 210: _LL2435: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 211:
_LL2439: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2441=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2441[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2442; _temp2442.tag=
Cyc_AbstractDeclarator_tok; _temp2442.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2443=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2443->tms=({ struct Cyc_List_List* _temp2444=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2444->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2444->tl= 0; _temp2444;}); _temp2443;}); _temp2442;}); _temp2441;}); break;
case 212: _LL2440: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2446=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2446[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2447; _temp2447.tag= Cyc_AbstractDeclarator_tok; _temp2447.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2448=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2448->tms=({ struct Cyc_List_List* _temp2449=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2449->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2449->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2449;});
_temp2448;}); _temp2447;}); _temp2446;}); break; case 213: _LL2445: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2451=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2451[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2452; _temp2452.tag= Cyc_AbstractDeclarator_tok;
_temp2452.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2453=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2453->tms=({
struct Cyc_List_List* _temp2454=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2454->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2455=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2455[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2456; _temp2456.tag= Cyc_Absyn_ConstArray_mod; _temp2456.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2456;});
_temp2455;})); _temp2454->tl= 0; _temp2454;}); _temp2453;}); _temp2452;});
_temp2451;}); break; case 214: _LL2450: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2458=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2458[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2459; _temp2459.tag= Cyc_AbstractDeclarator_tok; _temp2459.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2460=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2460->tms=({ struct Cyc_List_List* _temp2461=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2461->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2462=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2462[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2463; _temp2463.tag= Cyc_Absyn_ConstArray_mod;
_temp2463.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2463;}); _temp2462;})); _temp2461->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2461;}); _temp2460;}); _temp2459;}); _temp2458;}); break; case 215: _LL2457:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2465=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2465[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2466; _temp2466.tag=
Cyc_AbstractDeclarator_tok; _temp2466.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2467=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2467->tms=({ struct Cyc_List_List* _temp2468=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2468->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2469=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2469[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2470; _temp2470.tag= Cyc_Absyn_Function_mod;
_temp2470.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2471=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2471[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2472; _temp2472.tag= Cyc_Absyn_WithTypes;
_temp2472.f1= 0; _temp2472.f2= 0; _temp2472.f3= 0; _temp2472.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2472.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2472;}); _temp2471;})); _temp2470;}); _temp2469;}));
_temp2468->tl= 0; _temp2468;}); _temp2467;}); _temp2466;}); _temp2465;}); break;
case 216: _LL2464: { struct _tuple16 _temp2476; struct Cyc_List_List* _temp2477;
struct Cyc_Core_Opt* _temp2479; struct Cyc_Absyn_VarargInfo* _temp2481; int
_temp2483; struct Cyc_List_List* _temp2485; struct _tuple16* _temp2474= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2476=*
_temp2474; _LL2486: _temp2485= _temp2476.f1; goto _LL2484; _LL2484: _temp2483=
_temp2476.f2; goto _LL2482; _LL2482: _temp2481= _temp2476.f3; goto _LL2480;
_LL2480: _temp2479= _temp2476.f4; goto _LL2478; _LL2478: _temp2477= _temp2476.f5;
goto _LL2475; _LL2475: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2487=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2487[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2488; _temp2488.tag= Cyc_AbstractDeclarator_tok; _temp2488.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2489=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2489->tms=({ struct Cyc_List_List* _temp2490=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2490->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2491=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2491[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2492; _temp2492.tag= Cyc_Absyn_Function_mod;
_temp2492.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2493=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2493[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2494; _temp2494.tag= Cyc_Absyn_WithTypes;
_temp2494.f1= _temp2485; _temp2494.f2= _temp2483; _temp2494.f3= _temp2481;
_temp2494.f4= _temp2479; _temp2494.f5= _temp2477; _temp2494;}); _temp2493;}));
_temp2492;}); _temp2491;})); _temp2490->tl= 0; _temp2490;}); _temp2489;});
_temp2488;}); _temp2487;}); break;} case 217: _LL2473: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2496=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2496[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2497; _temp2497.tag= Cyc_AbstractDeclarator_tok;
_temp2497.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2498=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2498->tms=({
struct Cyc_List_List* _temp2499=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2499->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2500=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2500[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2501; _temp2501.tag=
Cyc_Absyn_Function_mod; _temp2501.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2502=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2502[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2503; _temp2503.tag= Cyc_Absyn_WithTypes;
_temp2503.f1= 0; _temp2503.f2= 0; _temp2503.f3= 0; _temp2503.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2503.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2503;}); _temp2502;})); _temp2501;}); _temp2500;}));
_temp2499->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2499;});
_temp2498;}); _temp2497;}); _temp2496;}); break; case 218: _LL2495: { struct
_tuple16 _temp2507; struct Cyc_List_List* _temp2508; struct Cyc_Core_Opt*
_temp2510; struct Cyc_Absyn_VarargInfo* _temp2512; int _temp2514; struct Cyc_List_List*
_temp2516; struct _tuple16* _temp2505= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2507=*
_temp2505; _LL2517: _temp2516= _temp2507.f1; goto _LL2515; _LL2515: _temp2514=
_temp2507.f2; goto _LL2513; _LL2513: _temp2512= _temp2507.f3; goto _LL2511;
_LL2511: _temp2510= _temp2507.f4; goto _LL2509; _LL2509: _temp2508= _temp2507.f5;
goto _LL2506; _LL2506: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2518=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2518[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2519; _temp2519.tag= Cyc_AbstractDeclarator_tok; _temp2519.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2520=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2520->tms=({ struct Cyc_List_List* _temp2521=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2521->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2522=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2522[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2523; _temp2523.tag= Cyc_Absyn_Function_mod;
_temp2523.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2524=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2524[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2525; _temp2525.tag= Cyc_Absyn_WithTypes;
_temp2525.f1= _temp2516; _temp2525.f2= _temp2514; _temp2525.f3= _temp2512;
_temp2525.f4= _temp2510; _temp2525.f5= _temp2508; _temp2525;}); _temp2524;}));
_temp2523;}); _temp2522;})); _temp2521->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2521;}); _temp2520;}); _temp2519;}); _temp2518;}); break;} case 219:
_LL2504: { struct Cyc_List_List* _temp2527=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2528=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2528[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2529; _temp2529.tag= Cyc_AbstractDeclarator_tok; _temp2529.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2530=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2530->tms=({ struct Cyc_List_List* _temp2531=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2531->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2532=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2532[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2533; _temp2533.tag= Cyc_Absyn_TypeParams_mod;
_temp2533.f1= _temp2527; _temp2533.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2533.f3=
0; _temp2533;}); _temp2532;})); _temp2531->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2531;}); _temp2530;}); _temp2529;}); _temp2528;}); break;} case 220:
_LL2526:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2535=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2536=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2536[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2537; _temp2537.tag= Cyc_AbstractDeclarator_tok; _temp2537.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2538=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2538->tms=({ struct Cyc_List_List* _temp2539=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2539->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2540=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2540[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2541; _temp2541.tag= Cyc_Absyn_TypeParams_mod;
_temp2541.f1= _temp2535; _temp2541.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2541.f3=
0; _temp2541;}); _temp2540;})); _temp2539->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2539;}); _temp2538;}); _temp2537;}); _temp2536;}); break;} case 221:
_LL2534: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2543=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2543[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2544; _temp2544.tag=
Cyc_AbstractDeclarator_tok; _temp2544.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2545=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2545->tms=({ struct Cyc_List_List* _temp2546=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2546->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2547=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2547[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2548; _temp2548.tag= Cyc_Absyn_Attributes_mod;
_temp2548.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2548.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2548;}); _temp2547;}));
_temp2546->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms; _temp2546;});
_temp2545;}); _temp2544;}); _temp2543;}); break; case 222: _LL2542: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 223: _LL2549:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
224: _LL2550: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 225: _LL2551: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 226: _LL2552: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 227: _LL2553:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
228: _LL2554: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2566=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2566->name=({ struct
_tagged_arr* _temp2567=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2567[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2567;});
_temp2566->identity= 0; _temp2566->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2566;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2564=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2564[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2565; _temp2565.tag= Cyc_Absyn_VarType;
_temp2565.f1= tv; _temp2565;}); _temp2564;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2556=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2556[ 0]=({ struct Cyc_Stmt_tok_struct _temp2557; _temp2557.tag= Cyc_Stmt_tok;
_temp2557.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2558=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2558[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2559; _temp2559.tag= Cyc_Absyn_Region_s;
_temp2559.f1= tv; _temp2559.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2560=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2560->f1=(
void*) Cyc_Absyn_Loc_n; _temp2560->f2=({ struct _tagged_arr* _temp2561=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2561[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2561;});
_temp2560;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2562=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2562[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2563; _temp2563.tag=
Cyc_Absyn_RgnHandleType; _temp2563.f1=( void*) t; _temp2563;}); _temp2562;}), 0);
_temp2559.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2559;}); _temp2558;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2557;});
_temp2556;}); break;} case 229: _LL2555: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2579=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2579->name=({ struct
_tagged_arr* _temp2580=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2580[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2582; _temp2582.tag= Cyc_Std_String_pa; _temp2582.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2581[ 1u]={& _temp2582}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2581, sizeof( void*), 1u));}});
_temp2580;}); _temp2579->identity= 0; _temp2579->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2579;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2577=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2577[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2578; _temp2578.tag= Cyc_Absyn_VarType;
_temp2578.f1= tv; _temp2578;}); _temp2577;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2569=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2569[ 0]=({ struct Cyc_Stmt_tok_struct _temp2570; _temp2570.tag= Cyc_Stmt_tok;
_temp2570.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2571=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2571[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2572; _temp2572.tag= Cyc_Absyn_Region_s;
_temp2572.f1= tv; _temp2572.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2573=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2573->f1=(
void*) Cyc_Absyn_Loc_n; _temp2573->f2=({ struct _tagged_arr* _temp2574=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2574[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2574;});
_temp2573;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2575=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2575[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2576; _temp2576.tag=
Cyc_Absyn_RgnHandleType; _temp2576.f1=( void*) t; _temp2576;}); _temp2575;}), 0);
_temp2572.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2572;}); _temp2571;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2570;});
_temp2569;}); break;} case 230: _LL2568: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2584=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2584[ 0]=({ struct Cyc_Stmt_tok_struct _temp2585; _temp2585.tag= Cyc_Stmt_tok;
_temp2585.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2586=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2586[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2587; _temp2587.tag= Cyc_Absyn_Cut_s;
_temp2587.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2587;}); _temp2586;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2585;});
_temp2584;}); break; case 231: _LL2583: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2589=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2589[ 0]=({ struct Cyc_Stmt_tok_struct _temp2590; _temp2590.tag= Cyc_Stmt_tok;
_temp2590.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2591=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2591[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2592; _temp2592.tag= Cyc_Absyn_Splice_s;
_temp2592.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2592;}); _temp2591;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2590;});
_temp2589;}); break; case 232: _LL2588: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2594=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2594[ 0]=({ struct Cyc_Stmt_tok_struct _temp2595; _temp2595.tag= Cyc_Stmt_tok;
_temp2595.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2596=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2596[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2597; _temp2597.tag= Cyc_Absyn_Label_s;
_temp2597.f1=({ struct _tagged_arr* _temp2598=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2598[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2598;});
_temp2597.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2597;}); _temp2596;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2595;});
_temp2594;}); break; case 233: _LL2593: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2600=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2600[ 0]=({ struct Cyc_Stmt_tok_struct _temp2601; _temp2601.tag= Cyc_Stmt_tok;
_temp2601.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2601;});
_temp2600;}); break; case 234: _LL2599: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2603=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2603[ 0]=({ struct Cyc_Stmt_tok_struct _temp2604; _temp2604.tag= Cyc_Stmt_tok;
_temp2604.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2604;}); _temp2603;}); break; case 235: _LL2602: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2606=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2606[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2607; _temp2607.tag= Cyc_Stmt_tok; _temp2607.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2607;}); _temp2606;}); break; case 236: _LL2605: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 237:
_LL2608: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2610=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2610[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2611; _temp2611.tag= Cyc_Stmt_tok; _temp2611.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2611;}); _temp2610;}); break;
case 238: _LL2609: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2613=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2613[ 0]=({ struct Cyc_Stmt_tok_struct _temp2614; _temp2614.tag= Cyc_Stmt_tok;
_temp2614.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2614;});
_temp2613;}); break; case 239: _LL2612: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 240: _LL2615:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2617=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2617[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2618; _temp2618.tag= Cyc_Stmt_tok; _temp2618.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2618;}); _temp2617;}); break; case 241: _LL2616: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2620=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2620[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2621; _temp2621.tag= Cyc_Stmt_tok; _temp2621.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2622=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2622[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2623; _temp2623.tag= Cyc_Absyn_Fn_d;
_temp2623.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2623;}); _temp2622;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2621;}); _temp2620;}); break; case 242: _LL2619: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2625=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2625[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2626; _temp2626.tag= Cyc_Stmt_tok; _temp2626.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2627=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2627[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2628; _temp2628.tag= Cyc_Absyn_Fn_d;
_temp2628.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2628;}); _temp2627;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2626;}); _temp2625;}); break; case 243: _LL2624: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2630=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2630[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2631; _temp2631.tag= Cyc_Stmt_tok; _temp2631.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2631;});
_temp2630;}); break; case 244: _LL2629: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2633=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2633[ 0]=({ struct Cyc_Stmt_tok_struct _temp2634; _temp2634.tag= Cyc_Stmt_tok;
_temp2634.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2634;}); _temp2633;}); break; case 245: _LL2632: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2636=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2636[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2637; _temp2637.tag= Cyc_Stmt_tok; _temp2637.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2637;}); _temp2636;}); break; case 246: _LL2635: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2639=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2639[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2640; _temp2640.tag= Cyc_Stmt_tok; _temp2640.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2641=( struct Cyc_Absyn_SwitchC_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2641[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2642; _temp2642.tag= Cyc_Absyn_SwitchC_s;
_temp2642.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2642.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2642;});
_temp2641;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2640;}); _temp2639;}); break; case 247:
_LL2638: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2644=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2644[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2645; _temp2645.tag= Cyc_Stmt_tok; _temp2645.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2645;});
_temp2644;}); break; case 248: _LL2643: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2647=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2647[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2648; _temp2648.tag= Cyc_SwitchClauseList_tok; _temp2648.f1= 0; _temp2648;});
_temp2647;}); break; case 249: _LL2646: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2650=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2650[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2651; _temp2651.tag= Cyc_SwitchClauseList_tok; _temp2651.f1=({ struct Cyc_List_List*
_temp2652=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2652->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2653=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2653->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2653->pat_vars= 0; _temp2653->where_clause= 0; _temp2653->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2653->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2653;}); _temp2652->tl= 0; _temp2652;});
_temp2651;}); _temp2650;}); break; case 250: _LL2649: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2655=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2655[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2656; _temp2656.tag= Cyc_SwitchClauseList_tok;
_temp2656.f1=({ struct Cyc_List_List* _temp2657=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2657->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2658=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2658->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2658->pat_vars= 0; _temp2658->where_clause= 0;
_temp2658->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2658->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2658;});
_temp2657->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2657;}); _temp2656;});
_temp2655;}); break; case 251: _LL2654: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2660=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2660[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2661; _temp2661.tag= Cyc_SwitchClauseList_tok; _temp2661.f1=({ struct Cyc_List_List*
_temp2662=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2662->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2663=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2663->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2663->pat_vars=
0; _temp2663->where_clause= 0; _temp2663->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2663->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2663;}); _temp2662->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2662;});
_temp2661;}); _temp2660;}); break; case 252: _LL2659: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2665=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2665[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2666; _temp2666.tag= Cyc_SwitchClauseList_tok;
_temp2666.f1=({ struct Cyc_List_List* _temp2667=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2667->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2668=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2668->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp2668->pat_vars= 0; _temp2668->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2668->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2668->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2668;});
_temp2667->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2667;}); _temp2666;});
_temp2665;}); break; case 253: _LL2664: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2670=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2670[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2671; _temp2671.tag= Cyc_SwitchClauseList_tok; _temp2671.f1=({ struct Cyc_List_List*
_temp2672=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2672->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2673=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2673->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2673->pat_vars=
0; _temp2673->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2673->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2673->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2673;});
_temp2672->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2672;}); _temp2671;});
_temp2670;}); break; case 254: _LL2669: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2675=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2675[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2676; _temp2676.tag= Cyc_SwitchCClauseList_tok; _temp2676.f1= 0; _temp2676;});
_temp2675;}); break; case 255: _LL2674: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2678=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2678[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2679; _temp2679.tag= Cyc_SwitchCClauseList_tok; _temp2679.f1=({ struct Cyc_List_List*
_temp2680=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2680->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2681=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2681->cnst_exp= 0;
_temp2681->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2681->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2681;});
_temp2680->tl= 0; _temp2680;}); _temp2679;}); _temp2678;}); break; case 256:
_LL2677: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2683=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2683[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2684; _temp2684.tag=
Cyc_SwitchCClauseList_tok; _temp2684.f1=({ struct Cyc_List_List* _temp2685=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2685->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2686=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2686->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2686->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2686->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2686;});
_temp2685->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2685;}); _temp2684;});
_temp2683;}); break; case 257: _LL2682: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2688=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2688[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2689; _temp2689.tag= Cyc_SwitchCClauseList_tok; _temp2689.f1=({ struct Cyc_List_List*
_temp2690=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2690->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2691=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2691->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2691->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2691->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2691;});
_temp2690->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2690;}); _temp2689;});
_temp2688;}); break; case 258: _LL2687: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2693=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2693[ 0]=({ struct Cyc_Stmt_tok_struct _temp2694; _temp2694.tag= Cyc_Stmt_tok;
_temp2694.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2694;}); _temp2693;}); break; case 259: _LL2692: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2696=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2696[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2697; _temp2697.tag= Cyc_Stmt_tok; _temp2697.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2697;}); _temp2696;}); break; case 260: _LL2695: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2699=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2699[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2700; _temp2700.tag= Cyc_Stmt_tok; _temp2700.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2700;}); _temp2699;}); break; case 261: _LL2698: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2702=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2702[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2703; _temp2703.tag= Cyc_Stmt_tok; _temp2703.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2703;}); _temp2702;}); break; case 262: _LL2701: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2705=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2705[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2706; _temp2706.tag= Cyc_Stmt_tok; _temp2706.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2706;}); _temp2705;}); break; case 263: _LL2704: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2708=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2708[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2709; _temp2709.tag= Cyc_Stmt_tok; _temp2709.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2709;}); _temp2708;}); break; case 264: _LL2707: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2711=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2711[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2712; _temp2712.tag= Cyc_Stmt_tok; _temp2712.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2712;}); _temp2711;}); break; case 265: _LL2710: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2714=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2714[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2715; _temp2715.tag= Cyc_Stmt_tok; _temp2715.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2715;});
_temp2714;}); break; case 266: _LL2713: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2717=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2717[ 0]=({ struct Cyc_Stmt_tok_struct _temp2718; _temp2718.tag= Cyc_Stmt_tok;
_temp2718.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2718;});
_temp2717;}); break; case 267: _LL2716: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2720=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2720[ 0]=({ struct Cyc_Stmt_tok_struct _temp2721; _temp2721.tag= Cyc_Stmt_tok;
_temp2721.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2721;}); _temp2720;}); break; case 268: _LL2719: { struct Cyc_List_List*
_temp2723= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt* _temp2724= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2725=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2725[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2726; _temp2726.tag= Cyc_Stmt_tok; _temp2726.f1= Cyc_Parse_flatten_declarations(
_temp2723, _temp2724); _temp2726;}); _temp2725;}); break;} case 269: _LL2722: {
struct Cyc_List_List* _temp2728= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2729= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2730=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2730[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2731; _temp2731.tag= Cyc_Stmt_tok; _temp2731.f1= Cyc_Parse_flatten_declarations(
_temp2728, _temp2729); _temp2731;}); _temp2730;}); break;} case 270: _LL2727: {
struct Cyc_List_List* _temp2733= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2734= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2735=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2735[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2736; _temp2736.tag= Cyc_Stmt_tok; _temp2736.f1= Cyc_Parse_flatten_declarations(
_temp2733, _temp2734); _temp2736;}); _temp2735;}); break;} case 271: _LL2732: {
struct Cyc_List_List* _temp2738= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2739= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2740=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2740[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2741; _temp2741.tag= Cyc_Stmt_tok; _temp2741.f1= Cyc_Parse_flatten_declarations(
_temp2738, _temp2739); _temp2741;}); _temp2740;}); break;} case 272: _LL2737:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2743=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2743[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2744; _temp2744.tag= Cyc_Stmt_tok; _temp2744.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2745=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2745[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2745;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2744;}); _temp2743;}); break; case 273: _LL2742: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2747=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2747[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2748; _temp2748.tag= Cyc_Stmt_tok; _temp2748.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2748;}); _temp2747;}); break; case 274:
_LL2746: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2750=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2750[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2751; _temp2751.tag= Cyc_Stmt_tok; _temp2751.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2751;}); _temp2750;}); break; case 275: _LL2749: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2753=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2753[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2754; _temp2754.tag= Cyc_Stmt_tok; _temp2754.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2754;}); _temp2753;}); break; case 276:
_LL2752: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2756=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2756[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2757; _temp2757.tag= Cyc_Stmt_tok; _temp2757.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2757;}); _temp2756;}); break; case 277: _LL2755: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2759=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2759[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2760; _temp2760.tag= Cyc_Stmt_tok; _temp2760.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2760;}); _temp2759;}); break; case 278:
_LL2758: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2762=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2762[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2763; _temp2763.tag= Cyc_Stmt_tok; _temp2763.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2763;}); _temp2762;}); break; case 279:
_LL2761: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2765=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2765[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2766; _temp2766.tag= Cyc_Stmt_tok; _temp2766.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2766;}); _temp2765;}); break; case 280: _LL2764: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2768=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2768[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2769; _temp2769.tag= Cyc_Pattern_tok; _temp2769.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2769;});
_temp2768;}); break; case 281: _LL2767: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 282:
_LL2770: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2772=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2772[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2773; _temp2773.tag= Cyc_Pattern_tok; _temp2773.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2774=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2774[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2775; _temp2775.tag= Cyc_Absyn_Int_p; _temp2775.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2775.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2775;}); _temp2774;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2773;});
_temp2772;}); break; case 283: _LL2771: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2777=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2777[ 0]=({ struct Cyc_Pattern_tok_struct _temp2778; _temp2778.tag= Cyc_Pattern_tok;
_temp2778.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2779=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2779[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2780; _temp2780.tag= Cyc_Absyn_Int_p;
_temp2780.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2780.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2780;});
_temp2779;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2778;}); _temp2777;}); break; case 284:
_LL2776: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2782=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2782[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2783; _temp2783.tag= Cyc_Pattern_tok; _temp2783.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2784=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2784[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2785; _temp2785.tag= Cyc_Absyn_Float_p; _temp2785.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2785;}); _temp2784;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2783;});
_temp2782;}); break; case 285: _LL2781: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2787=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2787[ 0]=({ struct Cyc_Pattern_tok_struct _temp2788; _temp2788.tag= Cyc_Pattern_tok;
_temp2788.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2789=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2789[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2790; _temp2790.tag= Cyc_Absyn_Char_p; _temp2790.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2790;});
_temp2789;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2788;}); _temp2787;}); break; case 286:
_LL2786: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2792=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2792[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2793; _temp2793.tag= Cyc_Pattern_tok; _temp2793.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2793;});
_temp2792;}); break; case 287: _LL2791: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2795=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2795[ 0]=({ struct Cyc_Pattern_tok_struct _temp2796; _temp2796.tag= Cyc_Pattern_tok;
_temp2796.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2797=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2797[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2798; _temp2798.tag=
Cyc_Absyn_UnknownId_p; _temp2798.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2798;}); _temp2797;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2796;}); _temp2795;}); break; case 288:
_LL2794: { struct Cyc_List_List* _temp2800=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2801=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2801[ 0]=({ struct Cyc_Pattern_tok_struct _temp2802; _temp2802.tag= Cyc_Pattern_tok;
_temp2802.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2803=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2803[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2804; _temp2804.tag= Cyc_Absyn_UnknownCall_p; _temp2804.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2804.f2=
_temp2800; _temp2804.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2804;});
_temp2803;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2802;}); _temp2801;}); break;} case
289: _LL2799: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2806=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2806[ 0]=({ struct Cyc_Pattern_tok_struct _temp2807; _temp2807.tag= Cyc_Pattern_tok;
_temp2807.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2808=( struct Cyc_Absyn_Tuple_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2808[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2809; _temp2809.tag= Cyc_Absyn_Tuple_p;
_temp2809.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2809;}); _temp2808;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2807;}); _temp2806;}); break; case 290: _LL2805: { struct Cyc_List_List*
_temp2811=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); yyval=(
void*)({ struct Cyc_Pattern_tok_struct* _temp2812=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2812[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2813; _temp2813.tag= Cyc_Pattern_tok; _temp2813.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2814=( struct Cyc_Absyn_UnknownFields_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2814[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2815; _temp2815.tag= Cyc_Absyn_UnknownFields_p;
_temp2815.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2815.f2= _temp2811; _temp2815.f3= 0; _temp2815;});
_temp2814;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2813;}); _temp2812;}); break;} case
291: _LL2810: { struct Cyc_List_List* _temp2817=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2818=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2818[ 0]=({ struct Cyc_Pattern_tok_struct _temp2819; _temp2819.tag= Cyc_Pattern_tok;
_temp2819.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2820=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2820[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2821; _temp2821.tag= Cyc_Absyn_UnknownFields_p; _temp2821.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2821.f2=
_temp2817; _temp2821.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2821;});
_temp2820;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2819;}); _temp2818;}); break;} case
292: _LL2816: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2823=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2823[ 0]=({ struct Cyc_Pattern_tok_struct _temp2824; _temp2824.tag= Cyc_Pattern_tok;
_temp2824.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2825=( struct Cyc_Absyn_Pointer_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2825[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2826; _temp2826.tag= Cyc_Absyn_Pointer_p;
_temp2826.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2826;}); _temp2825;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2824;});
_temp2823;}); break; case 293: _LL2822: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2828=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2828[ 0]=({ struct Cyc_Pattern_tok_struct _temp2829; _temp2829.tag= Cyc_Pattern_tok;
_temp2829.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2830=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2830[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2831; _temp2831.tag=
Cyc_Absyn_Reference_p; _temp2831.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2832=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2832->f1=(
void*) Cyc_Absyn_Loc_n; _temp2832->f2=({ struct _tagged_arr* _temp2833=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2833[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2833;});
_temp2832;}),( void*) Cyc_Absyn_VoidType, 0); _temp2831;}); _temp2830;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2829;}); _temp2828;}); break; case 294: _LL2827: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2835=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2835[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2836; _temp2836.tag= Cyc_PatternList_tok;
_temp2836.f1= 0; _temp2836;}); _temp2835;}); break; case 295: _LL2834: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2838=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2838[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2839; _temp2839.tag= Cyc_PatternList_tok;
_temp2839.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2839;}); _temp2838;}); break; case 296: _LL2837: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2841=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2841[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2842; _temp2842.tag= Cyc_PatternList_tok;
_temp2842.f1=({ struct Cyc_List_List* _temp2843=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2843->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2843->tl= 0;
_temp2843;}); _temp2842;}); _temp2841;}); break; case 297: _LL2840: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2845=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2845[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2846; _temp2846.tag= Cyc_PatternList_tok;
_temp2846.f1=({ struct Cyc_List_List* _temp2847=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2847->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2847->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2847;}); _temp2846;}); _temp2845;}); break; case 298:
_LL2844: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2849=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2849[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2850; _temp2850.tag=
Cyc_FieldPattern_tok; _temp2850.f1=({ struct _tuple13* _temp2851=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2851->f1= 0; _temp2851->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2851;}); _temp2850;}); _temp2849;}); break; case 299:
_LL2848: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2853=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2853[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2854; _temp2854.tag=
Cyc_FieldPattern_tok; _temp2854.f1=({ struct _tuple13* _temp2855=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2855->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2855->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2855;}); _temp2854;}); _temp2853;}); break; case 300:
_LL2852: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2857=(
struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2857[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2858; _temp2858.tag=
Cyc_FieldPatternList_tok; _temp2858.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2858;});
_temp2857;}); break; case 301: _LL2856: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2860=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2860[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2861; _temp2861.tag= Cyc_FieldPatternList_tok; _temp2861.f1=({ struct Cyc_List_List*
_temp2862=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2862->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2862->tl= 0;
_temp2862;}); _temp2861;}); _temp2860;}); break; case 302: _LL2859: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2864=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2864[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2865; _temp2865.tag= Cyc_FieldPatternList_tok;
_temp2865.f1=({ struct Cyc_List_List* _temp2866=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2866->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2866->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2866;}); _temp2865;}); _temp2864;}); break; case 303:
_LL2863: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 304: _LL2867: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2869=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2869[ 0]=({ struct Cyc_Exp_tok_struct _temp2870; _temp2870.tag= Cyc_Exp_tok;
_temp2870.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2870;}); _temp2869;}); break; case 305: _LL2868: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 306: _LL2871:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2873=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2873[ 0]=({ struct Cyc_Exp_tok_struct
_temp2874; _temp2874.tag= Cyc_Exp_tok; _temp2874.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2874;}); _temp2873;}); break; case 307: _LL2872: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2876=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2876[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2877; _temp2877.tag= Cyc_Primopopt_tok; _temp2877.f1=
0; _temp2877;}); _temp2876;}); break; case 308: _LL2875: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2879=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2879[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2880; _temp2880.tag= Cyc_Primopopt_tok; _temp2880.f1=({
struct Cyc_Core_Opt* _temp2881=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2881->v=( void*)(( void*) Cyc_Absyn_Times); _temp2881;});
_temp2880;}); _temp2879;}); break; case 309: _LL2878: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2883=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2883[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2884; _temp2884.tag= Cyc_Primopopt_tok; _temp2884.f1=({
struct Cyc_Core_Opt* _temp2885=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2885->v=( void*)(( void*) Cyc_Absyn_Div); _temp2885;});
_temp2884;}); _temp2883;}); break; case 310: _LL2882: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2887=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2887[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2888; _temp2888.tag= Cyc_Primopopt_tok; _temp2888.f1=({
struct Cyc_Core_Opt* _temp2889=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2889->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2889;});
_temp2888;}); _temp2887;}); break; case 311: _LL2886: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2891=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2891[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2892; _temp2892.tag= Cyc_Primopopt_tok; _temp2892.f1=({
struct Cyc_Core_Opt* _temp2893=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2893->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2893;});
_temp2892;}); _temp2891;}); break; case 312: _LL2890: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2895=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2895[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2896; _temp2896.tag= Cyc_Primopopt_tok; _temp2896.f1=({
struct Cyc_Core_Opt* _temp2897=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2897->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2897;});
_temp2896;}); _temp2895;}); break; case 313: _LL2894: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2899=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2899[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2900; _temp2900.tag= Cyc_Primopopt_tok; _temp2900.f1=({
struct Cyc_Core_Opt* _temp2901=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2901->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2901;});
_temp2900;}); _temp2899;}); break; case 314: _LL2898: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2903=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2903[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2904; _temp2904.tag= Cyc_Primopopt_tok; _temp2904.f1=({
struct Cyc_Core_Opt* _temp2905=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2905->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2905;});
_temp2904;}); _temp2903;}); break; case 315: _LL2902: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2907=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2907[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2908; _temp2908.tag= Cyc_Primopopt_tok; _temp2908.f1=({
struct Cyc_Core_Opt* _temp2909=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2909->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2909;});
_temp2908;}); _temp2907;}); break; case 316: _LL2906: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2911=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2911[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2912; _temp2912.tag= Cyc_Primopopt_tok; _temp2912.f1=({
struct Cyc_Core_Opt* _temp2913=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2913->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2913;});
_temp2912;}); _temp2911;}); break; case 317: _LL2910: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2915=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2915[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2916; _temp2916.tag= Cyc_Primopopt_tok; _temp2916.f1=({
struct Cyc_Core_Opt* _temp2917=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2917->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2917;});
_temp2916;}); _temp2915;}); break; case 318: _LL2914: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 319: _LL2918:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2920=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2920[ 0]=({ struct Cyc_Exp_tok_struct
_temp2921; _temp2921.tag= Cyc_Exp_tok; _temp2921.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2921;});
_temp2920;}); break; case 320: _LL2919: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2923=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2923[ 0]=({ struct Cyc_Exp_tok_struct _temp2924; _temp2924.tag= Cyc_Exp_tok;
_temp2924.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2924;}); _temp2923;}); break; case 321: _LL2922: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2926=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2926[ 0]=({ struct Cyc_Exp_tok_struct
_temp2927; _temp2927.tag= Cyc_Exp_tok; _temp2927.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2927;}); _temp2926;}); break; case 322: _LL2925: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2929=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2929[ 0]=({ struct Cyc_Exp_tok_struct
_temp2930; _temp2930.tag= Cyc_Exp_tok; _temp2930.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2930;}); _temp2929;}); break; case 323: _LL2928: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2932=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2932[ 0]=({ struct Cyc_Exp_tok_struct
_temp2933; _temp2933.tag= Cyc_Exp_tok; _temp2933.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2933;});
_temp2932;}); break; case 324: _LL2931: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2935=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2935[ 0]=({ struct Cyc_Exp_tok_struct _temp2936; _temp2936.tag= Cyc_Exp_tok;
_temp2936.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2936;}); _temp2935;}); break; case 325: _LL2934: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 326: _LL2937:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
327: _LL2938: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2940=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2940[ 0]=({ struct Cyc_Exp_tok_struct
_temp2941; _temp2941.tag= Cyc_Exp_tok; _temp2941.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2941;}); _temp2940;}); break; case 328: _LL2939: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL2942:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2944=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2944[ 0]=({ struct Cyc_Exp_tok_struct
_temp2945; _temp2945.tag= Cyc_Exp_tok; _temp2945.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2945;}); _temp2944;}); break; case 330: _LL2943: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL2946:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2948=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2948[ 0]=({ struct Cyc_Exp_tok_struct
_temp2949; _temp2949.tag= Cyc_Exp_tok; _temp2949.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2949;});
_temp2948;}); break; case 332: _LL2947: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL2950:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2952=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2952[ 0]=({ struct Cyc_Exp_tok_struct
_temp2953; _temp2953.tag= Cyc_Exp_tok; _temp2953.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2953;});
_temp2952;}); break; case 334: _LL2951: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL2954:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2956=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2956[ 0]=({ struct Cyc_Exp_tok_struct
_temp2957; _temp2957.tag= Cyc_Exp_tok; _temp2957.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2957;});
_temp2956;}); break; case 336: _LL2955: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 337: _LL2958:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2960=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2960[ 0]=({ struct Cyc_Exp_tok_struct
_temp2961; _temp2961.tag= Cyc_Exp_tok; _temp2961.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2961;}); _temp2960;}); break; case 338: _LL2959: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2963=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2963[ 0]=({ struct Cyc_Exp_tok_struct
_temp2964; _temp2964.tag= Cyc_Exp_tok; _temp2964.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2964;}); _temp2963;}); break; case 339: _LL2962: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 340: _LL2965:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2967=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2967[ 0]=({ struct Cyc_Exp_tok_struct
_temp2968; _temp2968.tag= Cyc_Exp_tok; _temp2968.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2968;}); _temp2967;}); break; case 341: _LL2966: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2970=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2970[ 0]=({ struct Cyc_Exp_tok_struct
_temp2971; _temp2971.tag= Cyc_Exp_tok; _temp2971.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2971;}); _temp2970;}); break; case 342: _LL2969: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2973=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2973[ 0]=({ struct Cyc_Exp_tok_struct
_temp2974; _temp2974.tag= Cyc_Exp_tok; _temp2974.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2974;}); _temp2973;}); break; case 343: _LL2972: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2976=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2976[ 0]=({ struct Cyc_Exp_tok_struct
_temp2977; _temp2977.tag= Cyc_Exp_tok; _temp2977.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2977;}); _temp2976;}); break; case 344: _LL2975: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 345: _LL2978:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2980=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2980[ 0]=({ struct Cyc_Exp_tok_struct
_temp2981; _temp2981.tag= Cyc_Exp_tok; _temp2981.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2981;}); _temp2980;}); break; case 346: _LL2979: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2983=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2983[ 0]=({ struct Cyc_Exp_tok_struct
_temp2984; _temp2984.tag= Cyc_Exp_tok; _temp2984.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2984;}); _temp2983;}); break; case 347: _LL2982: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 348: _LL2985:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2987=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2987[ 0]=({ struct Cyc_Exp_tok_struct
_temp2988; _temp2988.tag= Cyc_Exp_tok; _temp2988.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2988;});
_temp2987;}); break; case 349: _LL2986: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2990=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2990[ 0]=({ struct Cyc_Exp_tok_struct _temp2991; _temp2991.tag= Cyc_Exp_tok;
_temp2991.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2991;}); _temp2990;}); break; case 350: _LL2989: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 351: _LL2992:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2994=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2994[ 0]=({ struct Cyc_Exp_tok_struct
_temp2995; _temp2995.tag= Cyc_Exp_tok; _temp2995.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2995;});
_temp2994;}); break; case 352: _LL2993: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2997=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2997[ 0]=({ struct Cyc_Exp_tok_struct _temp2998; _temp2998.tag= Cyc_Exp_tok;
_temp2998.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2998;}); _temp2997;}); break; case 353: _LL2996: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3000=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3000[ 0]=({ struct Cyc_Exp_tok_struct
_temp3001; _temp3001.tag= Cyc_Exp_tok; _temp3001.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3001;});
_temp3000;}); break; case 354: _LL2999: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 355: _LL3002:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3004=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3004[ 0]=({ struct Cyc_Exp_tok_struct
_temp3005; _temp3005.tag= Cyc_Exp_tok; _temp3005.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3005;}); _temp3004;}); break; case 356: _LL3003: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 357: _LL3006:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3008=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3008[ 0]=({ struct Cyc_Exp_tok_struct
_temp3009; _temp3009.tag= Cyc_Exp_tok; _temp3009.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3009;}); _temp3008;}); break; case 358: _LL3007: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3011=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3011[ 0]=({ struct Cyc_Exp_tok_struct
_temp3012; _temp3012.tag= Cyc_Exp_tok; _temp3012.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3012;}); _temp3011;}); break; case 359: _LL3010: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3014=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3014[ 0]=({ struct Cyc_Exp_tok_struct
_temp3015; _temp3015.tag= Cyc_Exp_tok; _temp3015.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3015;}); _temp3014;}); break; case 360: _LL3013: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3017=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3017[ 0]=({ struct Cyc_Exp_tok_struct
_temp3018; _temp3018.tag= Cyc_Exp_tok; _temp3018.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3018;}); _temp3017;}); break; case 361: _LL3016: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 362: _LL3019:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3021=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3021[ 0]=({ struct Cyc_Exp_tok_struct
_temp3022; _temp3022.tag= Cyc_Exp_tok; _temp3022.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3022;}); _temp3021;}); break; case 363: _LL3020: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3024=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3024[ 0]=({ struct Cyc_Exp_tok_struct
_temp3025; _temp3025.tag= Cyc_Exp_tok; _temp3025.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3025;});
_temp3024;}); break; case 364: _LL3023: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3027=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3027[ 0]=({ struct Cyc_Exp_tok_struct _temp3028; _temp3028.tag= Cyc_Exp_tok;
_temp3028.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3028;}); _temp3027;}); break; case 365: _LL3026: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3030=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3030[ 0]=({ struct Cyc_Exp_tok_struct
_temp3031; _temp3031.tag= Cyc_Exp_tok; _temp3031.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,({ struct _tagged_arr* _temp3032=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp3032[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3032;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3031;}); _temp3030;}); break; case 366:
_LL3029: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3034=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3034[ 0]=({ struct Cyc_Exp_tok_struct
_temp3035; _temp3035.tag= Cyc_Exp_tok; _temp3035.f1= Cyc_Absyn_gentyp_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3035;}); _temp3034;}); break; case 367: _LL3033: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3037=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3037[ 0]=({ struct Cyc_Exp_tok_struct
_temp3038; _temp3038.tag= Cyc_Exp_tok; _temp3038.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3039=( struct Cyc_Absyn_Malloc_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3039[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3040; _temp3040.tag= Cyc_Absyn_Malloc_e;
_temp3040.f1= 0; _temp3040.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3040;}); _temp3039;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3038;});
_temp3037;}); break; case 368: _LL3036: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3042=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3042[ 0]=({ struct Cyc_Exp_tok_struct _temp3043; _temp3043.tag= Cyc_Exp_tok;
_temp3043.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3044=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3044[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3045; _temp3045.tag= Cyc_Absyn_Malloc_e;
_temp3045.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3045.f2=(
void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3045;}); _temp3044;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3043;});
_temp3042;}); break; case 369: _LL3041: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3047=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3047[ 0]=({ struct Cyc_Primop_tok_struct _temp3048; _temp3048.tag= Cyc_Primop_tok;
_temp3048.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3048;}); _temp3047;});
break; case 370: _LL3046: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3050=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3050[ 0]=({ struct Cyc_Primop_tok_struct _temp3051; _temp3051.tag= Cyc_Primop_tok;
_temp3051.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3051;}); _temp3050;});
break; case 371: _LL3049: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3053=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3053[ 0]=({ struct Cyc_Primop_tok_struct _temp3054; _temp3054.tag= Cyc_Primop_tok;
_temp3054.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3054;}); _temp3053;});
break; case 372: _LL3052: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 373: _LL3055: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3057=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3057[ 0]=({ struct Cyc_Exp_tok_struct _temp3058; _temp3058.tag= Cyc_Exp_tok;
_temp3058.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3058;}); _temp3057;}); break; case 374: _LL3056: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3060=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3060[ 0]=({ struct Cyc_Exp_tok_struct
_temp3061; _temp3061.tag= Cyc_Exp_tok; _temp3061.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3061;}); _temp3060;}); break; case 375:
_LL3059: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3063=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3063[ 0]=({ struct Cyc_Exp_tok_struct
_temp3064; _temp3064.tag= Cyc_Exp_tok; _temp3064.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3064;});
_temp3063;}); break; case 376: _LL3062: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3066=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3066[ 0]=({ struct Cyc_Exp_tok_struct _temp3067; _temp3067.tag= Cyc_Exp_tok;
_temp3067.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3068=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3068[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3068;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3067;}); _temp3066;}); break; case 377: _LL3065: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3070=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3070[ 0]=({ struct Cyc_Exp_tok_struct
_temp3071; _temp3071.tag= Cyc_Exp_tok; _temp3071.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3071;});
_temp3070;}); break;} case 378: _LL3069: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3073=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3073[ 0]=({ struct Cyc_Exp_tok_struct _temp3074; _temp3074.tag= Cyc_Exp_tok;
_temp3074.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3075=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3075[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3075;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3074;}); _temp3073;}); break; case 379: _LL3072: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3077=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3077[ 0]=({ struct Cyc_Exp_tok_struct
_temp3078; _temp3078.tag= Cyc_Exp_tok; _temp3078.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3078;});
_temp3077;}); break;} case 380: _LL3076: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3080=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3080[ 0]=({ struct Cyc_Exp_tok_struct _temp3081; _temp3081.tag= Cyc_Exp_tok;
_temp3081.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3081;}); _temp3080;}); break; case 381: _LL3079: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3083=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3083[ 0]=({ struct Cyc_Exp_tok_struct
_temp3084; _temp3084.tag= Cyc_Exp_tok; _temp3084.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3084;}); _temp3083;}); break; case 382: _LL3082: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3086=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3086[ 0]=({ struct Cyc_Exp_tok_struct
_temp3087; _temp3087.tag= Cyc_Exp_tok; _temp3087.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3088=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3088[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3089; _temp3089.tag= Cyc_Absyn_CompoundLit_e;
_temp3089.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3089.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3089;});
_temp3088;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3087;}); _temp3086;}); break; case 383:
_LL3085: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3091=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3091[ 0]=({ struct Cyc_Exp_tok_struct
_temp3092; _temp3092.tag= Cyc_Exp_tok; _temp3092.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3093=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3093[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3094; _temp3094.tag= Cyc_Absyn_CompoundLit_e;
_temp3094.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3094.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3094;});
_temp3093;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3092;}); _temp3091;}); break; case 384:
_LL3090: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3096=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3096[ 0]=({ struct Cyc_Exp_tok_struct
_temp3097; _temp3097.tag= Cyc_Exp_tok; _temp3097.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3098=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3098[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3099; _temp3099.tag= Cyc_Absyn_Fill_e; _temp3099.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3099;});
_temp3098;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3097;}); _temp3096;}); break; case 385:
_LL3095: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3101=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3101[ 0]=({ struct Cyc_Exp_tok_struct
_temp3102; _temp3102.tag= Cyc_Exp_tok; _temp3102.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3103=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3103[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3104; _temp3104.tag= Cyc_Absyn_Codegen_e;
_temp3104.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3104;}); _temp3103;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3102;}); _temp3101;}); break; case 386: _LL3100: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3106=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3106[ 0]=({ struct Cyc_Exp_tok_struct
_temp3107; _temp3107.tag= Cyc_Exp_tok; _temp3107.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3108=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3108[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3109; _temp3109.tag= Cyc_Absyn_UnknownId_e;
_temp3109.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3109;}); _temp3108;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3107;});
_temp3106;}); break; case 387: _LL3105: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 388: _LL3110:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3112=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3112[ 0]=({ struct Cyc_Exp_tok_struct
_temp3113; _temp3113.tag= Cyc_Exp_tok; _temp3113.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3113;});
_temp3112;}); break; case 389: _LL3111: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 390:
_LL3114: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3116=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3116[ 0]=({ struct Cyc_Exp_tok_struct
_temp3117; _temp3117.tag= Cyc_Exp_tok; _temp3117.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3118=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3118[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3119; _temp3119.tag=
Cyc_Absyn_UnknownId_e; _temp3119.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3119;});
_temp3118;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3117;});
_temp3116;}); break; case 391: _LL3115: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3121=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3121[ 0]=({ struct Cyc_Exp_tok_struct _temp3122; _temp3122.tag= Cyc_Exp_tok;
_temp3122.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3123=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3123[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3124; _temp3124.tag=
Cyc_Absyn_UnknownId_e; _temp3124.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3124;});
_temp3123;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3122;}); _temp3121;}); break; case 392: _LL3120: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3126=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3126[ 0]=({ struct Cyc_Exp_tok_struct
_temp3127; _temp3127.tag= Cyc_Exp_tok; _temp3127.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3127;}); _temp3126;}); break; case 393: _LL3125: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3129=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3129[ 0]=({ struct Cyc_Exp_tok_struct
_temp3130; _temp3130.tag= Cyc_Exp_tok; _temp3130.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3131=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3131[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3132; _temp3132.tag= Cyc_Absyn_Struct_e;
_temp3132.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3132.f2= 0; _temp3132.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3132.f4= 0;
_temp3132;}); _temp3131;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3130;});
_temp3129;}); break; case 394: _LL3128: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3134=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3134[ 0]=({ struct Cyc_Exp_tok_struct _temp3135; _temp3135.tag= Cyc_Exp_tok;
_temp3135.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3135;}); _temp3134;}); break; case 395: _LL3133: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3137=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3137[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3138; _temp3138.tag= Cyc_ExpList_tok; _temp3138.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3138;});
_temp3137;}); break; case 396: _LL3136: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3140=( struct Cyc_ExpList_tok_struct*) _cycalloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3140[ 0]=({ struct Cyc_ExpList_tok_struct _temp3141; _temp3141.tag= Cyc_ExpList_tok;
_temp3141.f1=({ struct Cyc_List_List* _temp3142=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3142->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3142->tl= 0;
_temp3142;}); _temp3141;}); _temp3140;}); break; case 397: _LL3139: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3144=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3144[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3145; _temp3145.tag= Cyc_ExpList_tok; _temp3145.f1=({ struct Cyc_List_List*
_temp3146=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3146->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3146->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp3146;});
_temp3145;}); _temp3144;}); break; case 398: _LL3143: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3148=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3148[ 0]=({ struct Cyc_Exp_tok_struct
_temp3149; _temp3149.tag= Cyc_Exp_tok; _temp3149.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3149;});
_temp3148;}); break; case 399: _LL3147: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3151=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3151[ 0]=({ struct Cyc_Exp_tok_struct _temp3152; _temp3152.tag= Cyc_Exp_tok;
_temp3152.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3152;});
_temp3151;}); break; case 400: _LL3150: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3154=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3154[ 0]=({ struct Cyc_Exp_tok_struct _temp3155; _temp3155.tag= Cyc_Exp_tok;
_temp3155.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3155;});
_temp3154;}); break; case 401: _LL3153: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3157=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3157[ 0]=({ struct Cyc_Exp_tok_struct _temp3158; _temp3158.tag= Cyc_Exp_tok;
_temp3158.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3158;});
_temp3157;}); break; case 402: _LL3156: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3160=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3160[ 0]=({ struct Cyc_QualId_tok_struct _temp3161; _temp3161.tag= Cyc_QualId_tok;
_temp3161.f1=({ struct _tuple1* _temp3162=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3162->f1= Cyc_Absyn_rel_ns_null; _temp3162->f2=({ struct
_tagged_arr* _temp3163=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3163[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3163;}); _temp3162;});
_temp3161;}); _temp3160;}); break; case 403: _LL3159: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3164:(
int) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen; yyssp_offset -=
yylen; yylsp_offset -= yylen; yyvs[ _check_known_subscript_notnull( 10000u, ++
yyvsp_offset)]= yyval; yylsp_offset ++; if( yylen ==  0){( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line= Cyc_yylloc.first_line;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_column= Cyc_yylloc.first_column;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line=( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line;( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_column=( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_column;} else{(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line=( yyls[
_check_known_subscript_notnull( 10000u,( yylsp_offset +  yylen) -  1)]).last_line;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_column=( yyls[
_check_known_subscript_notnull( 10000u,( yylsp_offset +  yylen) -  1)]).last_column;}
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 404u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 112u, yyn -  121)] +  yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >=  0?
yystate <=  4371: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4372u,
yystate)] ==  yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){
yystate=( int) Cyc_yytable[ _check_known_subscript_notnull( 4372u, yystate)];}
else{ yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 
121)];} goto yynewstate; yyerrlab: if( yyerrstatus ==  0){ ++ Cyc_yynerrs; yyn=(
int) Cyc_yypact[ _check_known_subscript_notnull( 798u, yystate)]; if( yyn >  -
32768? yyn <  4371: 0){ int sze= 0; struct _tagged_arr msg; int x; int count;
count= 0; for( x= yyn <  0? - yyn: 0; x <  233u /  sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4372u, x +  yyn)] ==  x){( sze
+= Cyc_Std_strlen( Cyc_yytname[ _check_known_subscript_notnull( 233u, x)]) +  15,
count ++);}} msg=({ unsigned int _temp3166=( unsigned int)( sze +  15);
unsigned char* _temp3167=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp3166)); struct _tagged_arr _temp3169= _tag_arr(
_temp3167, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp3168= _temp3166; unsigned int i; for( i= 0; i <  _temp3168; i ++){
_temp3167[ i]='\000';}}; _temp3169;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
sizeof( unsigned char), 12u)); if( count <  5){ count= 0; for( x= yyn <  0? -
yyn: 0; x <  233u /  sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4372u, x +  yyn)] ==  x){ Cyc_Std_strcat( msg,
count ==  0? _tag_arr(", expecting `", sizeof( unsigned char), 14u): _tag_arr(" or `",
sizeof( unsigned char), 6u)); Cyc_Std_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 233u, x)]); Cyc_Std_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus ==  3){ if( Cyc_yychar ==  0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset ==  0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 798u,
yystate)]; if( yyn ==  - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn <  0? 1:
yyn >  4371)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4372u, yyn)] !=  1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4372u,
yyn)]; if( yyn <  0){ if( yyn ==  - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn ==  0){ goto yyerrpop;}} if( yyn ==  797){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3179= v; struct
_tuple15* _temp3197; struct _tuple15 _temp3199; int _temp3200; unsigned char
_temp3202; short _temp3204; struct _tagged_arr _temp3206; struct Cyc_Core_Opt*
_temp3208; struct Cyc_Core_Opt* _temp3210; struct Cyc_Core_Opt _temp3212; struct
_tagged_arr* _temp3213; struct _tuple1* _temp3215; struct _tuple1 _temp3217;
struct _tagged_arr* _temp3218; void* _temp3220; _LL3181: if(*(( void**)
_temp3179) ==  Cyc_Int_tok){ _LL3198: _temp3197=(( struct Cyc_Int_tok_struct*)
_temp3179)->f1; _temp3199=* _temp3197; _LL3201: _temp3200= _temp3199.f2; goto
_LL3182;} else{ goto _LL3183;} _LL3183: if(*(( void**) _temp3179) ==  Cyc_Char_tok){
_LL3203: _temp3202=(( struct Cyc_Char_tok_struct*) _temp3179)->f1; goto _LL3184;}
else{ goto _LL3185;} _LL3185: if(*(( void**) _temp3179) ==  Cyc_Short_tok){
_LL3205: _temp3204=(( struct Cyc_Short_tok_struct*) _temp3179)->f1; goto _LL3186;}
else{ goto _LL3187;} _LL3187: if(*(( void**) _temp3179) ==  Cyc_String_tok){
_LL3207: _temp3206=(( struct Cyc_String_tok_struct*) _temp3179)->f1; goto
_LL3188;} else{ goto _LL3189;} _LL3189: if(*(( void**) _temp3179) ==  Cyc_Stringopt_tok){
_LL3209: _temp3208=(( struct Cyc_Stringopt_tok_struct*) _temp3179)->f1; if(
_temp3208 ==  0){ goto _LL3190;} else{ goto _LL3191;}} else{ goto _LL3191;}
_LL3191: if(*(( void**) _temp3179) ==  Cyc_Stringopt_tok){ _LL3211: _temp3210=((
struct Cyc_Stringopt_tok_struct*) _temp3179)->f1; if( _temp3210 ==  0){ goto
_LL3193;} else{ _temp3212=* _temp3210; _LL3214: _temp3213=( struct _tagged_arr*)
_temp3212.v; goto _LL3192;}} else{ goto _LL3193;} _LL3193: if(*(( void**)
_temp3179) ==  Cyc_QualId_tok){ _LL3216: _temp3215=(( struct Cyc_QualId_tok_struct*)
_temp3179)->f1; _temp3217=* _temp3215; _LL3221: _temp3220= _temp3217.f1; goto
_LL3219; _LL3219: _temp3218= _temp3217.f2; goto _LL3194;} else{ goto _LL3195;}
_LL3195: goto _LL3196; _LL3182:({ struct Cyc_Std_Int_pa_struct _temp3223;
_temp3223.tag= Cyc_Std_Int_pa; _temp3223.f1=( int)(( unsigned int) _temp3200);{
void* _temp3222[ 1u]={& _temp3223}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3222, sizeof( void*), 1u));}}); goto
_LL3180; _LL3184:({ struct Cyc_Std_Int_pa_struct _temp3225; _temp3225.tag= Cyc_Std_Int_pa;
_temp3225.f1=( int)(( unsigned int)(( int) _temp3202));{ void* _temp3224[ 1u]={&
_temp3225}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3224, sizeof( void*), 1u));}}); goto _LL3180;
_LL3186:({ struct Cyc_Std_Int_pa_struct _temp3227; _temp3227.tag= Cyc_Std_Int_pa;
_temp3227.f1=( int)(( unsigned int)(( int) _temp3204));{ void* _temp3226[ 1u]={&
_temp3227}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3226, sizeof( void*), 1u));}}); goto _LL3180;
_LL3188:({ struct Cyc_Std_String_pa_struct _temp3229; _temp3229.tag= Cyc_Std_String_pa;
_temp3229.f1=( struct _tagged_arr) _temp3206;{ void* _temp3228[ 1u]={& _temp3229};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3228, sizeof( void*), 1u));}}); goto _LL3180; _LL3190:({ void*
_temp3230[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3230, sizeof( void*), 0u));}); goto _LL3180;
_LL3192:({ struct Cyc_Std_String_pa_struct _temp3232; _temp3232.tag= Cyc_Std_String_pa;
_temp3232.f1=( struct _tagged_arr)* _temp3213;{ void* _temp3231[ 1u]={&
_temp3232}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3231, sizeof( void*), 1u));}}); goto _LL3180;
_LL3194: { struct Cyc_List_List* _temp3233= 0;{ void* _temp3234= _temp3220;
struct Cyc_List_List* _temp3242; struct Cyc_List_List* _temp3244; _LL3236: if((
unsigned int) _temp3234 >  1u?*(( int*) _temp3234) ==  Cyc_Absyn_Rel_n: 0){
_LL3243: _temp3242=(( struct Cyc_Absyn_Rel_n_struct*) _temp3234)->f1; goto
_LL3237;} else{ goto _LL3238;} _LL3238: if(( unsigned int) _temp3234 >  1u?*((
int*) _temp3234) ==  Cyc_Absyn_Abs_n: 0){ _LL3245: _temp3244=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3234)->f1; goto _LL3239;} else{ goto _LL3240;} _LL3240: if( _temp3234 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3241;} else{ goto _LL3235;} _LL3237: _temp3233=
_temp3242; goto _LL3235; _LL3239: _temp3233= _temp3244; goto _LL3235; _LL3241:
goto _LL3235; _LL3235:;} for( 0; _temp3233 !=  0; _temp3233=(( struct Cyc_List_List*)
_check_null( _temp3233))->tl){({ struct Cyc_Std_String_pa_struct _temp3247;
_temp3247.tag= Cyc_Std_String_pa; _temp3247.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp3233))->hd);{ void*
_temp3246[ 1u]={& _temp3247}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3246, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3249; _temp3249.tag= Cyc_Std_String_pa;
_temp3249.f1=( struct _tagged_arr)* _temp3218;{ void* _temp3248[ 1u]={&
_temp3249}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3248, sizeof( void*), 1u));}}); goto _LL3180;}
_LL3196:({ void* _temp3250[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3250, sizeof( void*), 0u));}); goto
_LL3180; _LL3180:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3251=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3251->v=(
void*) Cyc_Lexing_from_file( f); _temp3251;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
