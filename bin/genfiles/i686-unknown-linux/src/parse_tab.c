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
void*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
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
_temp314.f1=({ struct Cyc_Absyn_Tuniondecl** _temp315=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp315[ 0]= _temp299;
_temp315;}); _temp314;}); _temp313;})); _temp312.targs= _temp309; _temp312.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp312;}); _temp311;}); _temp310;}); if(
_temp299->fields !=  0){ declopt=({ struct Cyc_Core_Opt* _temp316=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp316->v=( void*) _temp277;
_temp316;});} goto _LL286;} _LL292: { struct Cyc_List_List* _temp317=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_tvar2typ, _temp301->tvs); t=( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp318=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp318[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp319; _temp319.tag= Cyc_Absyn_UnionType;
_temp319.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp301->name))->v); _temp319.f2= _temp317; _temp319.f3= 0;
_temp319;}); _temp318;}); if( _temp301->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp320=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp320->v=( void*) _temp277; _temp320;});} goto _LL286;} _LL294: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp321=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp321[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp322; _temp322.tag= Cyc_Absyn_EnumType; _temp322.f1=
_temp303->name; _temp322.f2= 0; _temp322;}); _temp321;}); declopt=({ struct Cyc_Core_Opt*
_temp323=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp323->v=( void*) _temp277; _temp323;}); goto _LL286; _LL296:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp277->loc); goto _LL286; _LL286:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp277->loc);} goto _LL252; _LL252:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp324=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp324[ 0]=({ struct Cyc_Absyn_IntType_struct _temp325; _temp325.tag= Cyc_Absyn_IntType;
_temp325.f1=( void*) sgn; _temp325.f2=( void*) sz; _temp325;}); _temp324;});}
else{ if( seen_sign){ void* _temp326= t; void* _temp332; _LL328: if((
unsigned int) _temp326 >  4u?*(( int*) _temp326) ==  Cyc_Absyn_IntType: 0){
_LL333: _temp332=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp326)->f2;
goto _LL329;} else{ goto _LL330;} _LL330: goto _LL331; _LL329: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp334=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp334[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp335; _temp335.tag= Cyc_Absyn_IntType; _temp335.f1=( void*) sgn; _temp335.f2=(
void*) _temp332; _temp335;}); _temp334;}); goto _LL327; _LL331: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL327; _LL327:;} if( seen_size){ void* _temp336= t; void*
_temp342; _LL338: if(( unsigned int) _temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_IntType:
0){ _LL343: _temp342=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp336)->f1;
goto _LL339;} else{ goto _LL340;} _LL340: goto _LL341; _LL339: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp344=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp344[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp345; _temp345.tag= Cyc_Absyn_IntType; _temp345.f1=( void*) _temp342;
_temp345.f2=( void*) sz; _temp345;}); _temp344;}); goto _LL337; _LL341: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL337; _LL337:;}} return({ struct _tuple5 _temp346; _temp346.f1=
t; _temp346.f2= declopt; _temp346;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds ==  0){ return 0;}{ struct Cyc_Parse_Declarator* _temp347=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp348= _temp347->id; struct Cyc_List_List* _temp351; struct
Cyc_List_List* _temp353; void* _temp355; struct Cyc_Absyn_Tqual _temp357; struct
_tuple6 _temp349= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp347->tms);
_LL358: _temp357= _temp349.f1; goto _LL356; _LL356: _temp355= _temp349.f2; goto
_LL354; _LL354: _temp353= _temp349.f3; goto _LL352; _LL352: _temp351= _temp349.f4;
goto _LL350; _LL350: return({ struct Cyc_List_List* _temp359=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp359->hd=( void*)({ struct
_tuple7* _temp360=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7));
_temp360->f1= _temp348; _temp360->f2= _temp357; _temp360->f3= _temp355; _temp360->f4=
_temp353; _temp360->f5= _temp351; _temp360;}); _temp359->tl= Cyc_Parse_apply_tmss(
_temp357, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp359;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms ==  0){ return({ struct _tuple6 _temp361; _temp361.f1= tq;
_temp361.f2= t; _temp361.f3= 0; _temp361.f4= atts; _temp361;});}{ void* _temp362=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp*
_temp376; void* _temp378; struct Cyc_Position_Segment* _temp380; struct Cyc_List_List*
_temp382; struct Cyc_Absyn_Tqual _temp384; void* _temp386; void* _temp388;
struct Cyc_List_List* _temp390; struct Cyc_Position_Segment* _temp392; _LL364:
if( _temp362 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL365;} else{ goto _LL366;}
_LL366: if(( unsigned int) _temp362 >  1u?*(( int*) _temp362) ==  Cyc_Absyn_ConstArray_mod:
0){ _LL377: _temp376=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp362)->f1;
goto _LL367;} else{ goto _LL368;} _LL368: if(( unsigned int) _temp362 >  1u?*((
int*) _temp362) ==  Cyc_Absyn_Function_mod: 0){ _LL379: _temp378=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp362)->f1; goto _LL369;} else{ goto
_LL370;} _LL370: if(( unsigned int) _temp362 >  1u?*(( int*) _temp362) ==  Cyc_Absyn_TypeParams_mod:
0){ _LL383: _temp382=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp362)->f1;
goto _LL381; _LL381: _temp380=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp362)->f2; goto _LL371;} else{ goto _LL372;} _LL372: if(( unsigned int)
_temp362 >  1u?*(( int*) _temp362) ==  Cyc_Absyn_Pointer_mod: 0){ _LL389:
_temp388=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp362)->f1; goto
_LL387; _LL387: _temp386=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp362)->f2; goto _LL385; _LL385: _temp384=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp362)->f3; goto _LL373;} else{ goto _LL374;} _LL374: if(( unsigned int)
_temp362 >  1u?*(( int*) _temp362) ==  Cyc_Absyn_Attributes_mod: 0){ _LL393:
_temp392=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp362)->f1; goto _LL391;
_LL391: _temp390=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp362)->f2; goto
_LL375;} else{ goto _LL363;} _LL365: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp394=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp394[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp395; _temp395.tag= Cyc_Absyn_ArrayType; _temp395.f1=(
void*) t; _temp395.f2= tq; _temp395.f3= 0; _temp395;}); _temp394;}), atts,((
struct Cyc_List_List*) _check_null( tms))->tl); _LL367: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp396=(
struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp396[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp397; _temp397.tag= Cyc_Absyn_ArrayType;
_temp397.f1=( void*) t; _temp397.f2= tq; _temp397.f3=( struct Cyc_Absyn_Exp*)
_temp376; _temp397;}); _temp396;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL369: { void* _temp398= _temp378; struct Cyc_List_List* _temp404;
struct Cyc_Core_Opt* _temp406; struct Cyc_Absyn_VarargInfo* _temp408; int
_temp410; struct Cyc_List_List* _temp412; struct Cyc_Position_Segment* _temp414;
_LL400: if(*(( int*) _temp398) ==  Cyc_Absyn_WithTypes){ _LL413: _temp412=((
struct Cyc_Absyn_WithTypes_struct*) _temp398)->f1; goto _LL411; _LL411: _temp410=((
struct Cyc_Absyn_WithTypes_struct*) _temp398)->f2; goto _LL409; _LL409: _temp408=((
struct Cyc_Absyn_WithTypes_struct*) _temp398)->f3; goto _LL407; _LL407: _temp406=((
struct Cyc_Absyn_WithTypes_struct*) _temp398)->f4; goto _LL405; _LL405: _temp404=((
struct Cyc_Absyn_WithTypes_struct*) _temp398)->f5; goto _LL401;} else{ goto
_LL402;} _LL402: if(*(( int*) _temp398) ==  Cyc_Absyn_NoTypes){ _LL415: _temp414=((
struct Cyc_Absyn_NoTypes_struct*) _temp398)->f2; goto _LL403;} else{ goto _LL399;}
_LL401: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!=  0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp416=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp416->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp416->tl= fn_atts; _temp416;});} else{ new_atts=({ struct Cyc_List_List*
_temp417=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp417->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp417->tl= new_atts; _temp417;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl !=  0){ void* _temp418=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_List_List*
_temp424; _LL420: if(( unsigned int) _temp418 >  1u?*(( int*) _temp418) ==  Cyc_Absyn_TypeParams_mod:
0){ _LL425: _temp424=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp418)->f1;
goto _LL421;} else{ goto _LL422;} _LL422: goto _LL423; _LL421: typvars= _temp424;
tms=(( struct Cyc_List_List*) _check_null( tms))->tl; goto _LL419; _LL423: goto
_LL419; _LL419:;} if((((( ! _temp410? _temp408 ==  0: 0)? _temp412 !=  0: 0)?((
struct Cyc_List_List*) _check_null( _temp412))->tl ==  0: 0)?(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp412))->hd)).f1 ==  0: 0)?(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( _temp412))->hd)).f3 == ( void*)
Cyc_Absyn_VoidType: 0){ _temp412= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)(
void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp412); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp406, t, _temp412, _temp410, _temp408, _temp404, fn_atts), new_atts,((
struct Cyc_List_List*) _check_null( tms))->tl);} _LL403:( int) _throw( Cyc_Parse_abort(
_tag_arr("function declaration without parameter types", sizeof( unsigned char),
45u), _temp414)); _LL399:;} _LL371: if((( struct Cyc_List_List*) _check_null(
tms))->tl ==  0){ return({ struct _tuple6 _temp426; _temp426.f1= tq; _temp426.f2=
t; _temp426.f3= _temp382; _temp426.f4= atts; _temp426;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp380)); _LL373: { void* _temp427= _temp388;
struct Cyc_Absyn_Exp* _temp435; struct Cyc_Absyn_Exp* _temp437; _LL429: if((
unsigned int) _temp427 >  1u?*(( int*) _temp427) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL436: _temp435=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp427)->f1; goto
_LL430;} else{ goto _LL431;} _LL431: if(( unsigned int) _temp427 >  1u?*(( int*)
_temp427) ==  Cyc_Absyn_Nullable_ps: 0){ _LL438: _temp437=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp427)->f1; goto _LL432;} else{ goto _LL433;} _LL433: if( _temp427 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL434;} else{ goto _LL428;} _LL430: return Cyc_Parse_apply_tms(
_temp384, Cyc_Absyn_atb_typ( t, _temp386, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp439=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp439[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp440; _temp440.tag= Cyc_Absyn_Upper_b;
_temp440.f1= _temp435; _temp440;}); _temp439;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL432: return Cyc_Parse_apply_tms( _temp384, Cyc_Absyn_starb_typ(
t, _temp386, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp441=( struct
Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp441[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp442; _temp442.tag= Cyc_Absyn_Upper_b;
_temp442.f1= _temp437; _temp442;}); _temp441;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL434: return Cyc_Parse_apply_tms( _temp384, Cyc_Absyn_tagged_typ(
t, _temp386, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL428:;}
_LL375: return Cyc_Parse_apply_tms( tq, t, Cyc_List_append( atts, _temp390),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL363:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp445; void* _temp447; struct _tuple5 _temp443= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL448: _temp447= _temp443.f1; goto _LL446; _LL446: _temp445=
_temp443.f2; goto _LL444; _LL444: if( _temp445 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp447;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp449=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp449[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp450; _temp450.tag= Cyc_Absyn_Decl_s;
_temp450.f1= d; _temp450.f2= s; _temp450;}); _temp449;}), Cyc_Position_segment_join(
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
Cyc_Parse_Typedef_sc: _LL451: istypedef= 1; break; case Cyc_Parse_Extern_sc:
_LL452: s=( void*) Cyc_Absyn_Extern; break; case Cyc_Parse_ExternC_sc: _LL453: s=(
void*) Cyc_Absyn_ExternC; break; case Cyc_Parse_Static_sc: _LL454: s=( void*)
Cyc_Absyn_Static; break; case Cyc_Parse_Auto_sc: _LL455: s=( void*) Cyc_Absyn_Public;
break; case Cyc_Parse_Register_sc: _LL456: s=( void*) Cyc_Absyn_Public; break;
case Cyc_Parse_Abstract_sc: _LL457: s=( void*) Cyc_Absyn_Abstract; break;}}{
struct Cyc_List_List* _temp461; struct Cyc_List_List* _temp463; struct _tuple0
_temp459=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( ids);
_LL464: _temp463= _temp459.f1; goto _LL462; _LL462: _temp461= _temp459.f2; goto
_LL460; _LL460: { int exps_empty= 1;{ struct Cyc_List_List* es= _temp461; for( 0;
es !=  0; es=(( struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd !=  0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp465= Cyc_Parse_collapse_type_specifiers( tss, loc); if(
_temp463 ==  0){ struct Cyc_Core_Opt* _temp468; void* _temp470; struct _tuple5
_temp466= _temp465; _LL471: _temp470= _temp466.f1; goto _LL469; _LL469: _temp468=
_temp466.f2; goto _LL467; _LL467: if( _temp468 !=  0){ struct Cyc_Absyn_Decl* d=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null( _temp468))->v;{
void* _temp472=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp484; struct Cyc_Absyn_Structdecl*
_temp486; struct Cyc_Absyn_Uniondecl* _temp488; struct Cyc_Absyn_Tuniondecl*
_temp490; _LL474: if(*(( int*) _temp472) ==  Cyc_Absyn_Enum_d){ _LL485: _temp484=((
struct Cyc_Absyn_Enum_d_struct*) _temp472)->f1; goto _LL475;} else{ goto _LL476;}
_LL476: if(*(( int*) _temp472) ==  Cyc_Absyn_Struct_d){ _LL487: _temp486=((
struct Cyc_Absyn_Struct_d_struct*) _temp472)->f1; goto _LL477;} else{ goto
_LL478;} _LL478: if(*(( int*) _temp472) ==  Cyc_Absyn_Union_d){ _LL489: _temp488=((
struct Cyc_Absyn_Union_d_struct*) _temp472)->f1; goto _LL479;} else{ goto _LL480;}
_LL480: if(*(( int*) _temp472) ==  Cyc_Absyn_Tunion_d){ _LL491: _temp490=((
struct Cyc_Absyn_Tunion_d_struct*) _temp472)->f1; goto _LL481;} else{ goto
_LL482;} _LL482: goto _LL483; _LL475:( void*)( _temp484->sc=( void*) s); if(
atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} goto _LL473; _LL477:( void*)( _temp486->sc=( void*)
s); _temp486->attributes= atts; goto _LL473; _LL479:( void*)( _temp488->sc=(
void*) s); _temp488->attributes= atts; goto _LL473; _LL481:( void*)( _temp490->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL473; _LL483: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL473:;}
return({ struct Cyc_List_List* _temp492=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp492->hd=( void*) d; _temp492->tl= 0;
_temp492;});} else{ void* _temp493= _temp470; struct Cyc_List_List* _temp507;
struct _tuple1* _temp509; struct Cyc_Absyn_TunionInfo _temp511; void* _temp513;
struct Cyc_Absyn_Tuniondecl** _temp515; struct Cyc_Absyn_TunionInfo _temp517;
struct Cyc_List_List* _temp519; void* _temp521; struct Cyc_Absyn_UnknownTunionInfo
_temp523; int _temp525; struct _tuple1* _temp527; struct Cyc_List_List* _temp529;
struct _tuple1* _temp531; struct _tuple1* _temp533; _LL495: if(( unsigned int)
_temp493 >  4u?*(( int*) _temp493) ==  Cyc_Absyn_StructType: 0){ _LL510:
_temp509=(( struct Cyc_Absyn_StructType_struct*) _temp493)->f1; goto _LL508;
_LL508: _temp507=(( struct Cyc_Absyn_StructType_struct*) _temp493)->f2; goto
_LL496;} else{ goto _LL497;} _LL497: if(( unsigned int) _temp493 >  4u?*(( int*)
_temp493) ==  Cyc_Absyn_TunionType: 0){ _LL512: _temp511=(( struct Cyc_Absyn_TunionType_struct*)
_temp493)->f1; _LL514: _temp513=( void*) _temp511.tunion_info; if(*(( int*)
_temp513) ==  Cyc_Absyn_KnownTunion){ _LL516: _temp515=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp513)->f1; goto _LL498;} else{ goto _LL499;}} else{ goto _LL499;} _LL499:
if(( unsigned int) _temp493 >  4u?*(( int*) _temp493) ==  Cyc_Absyn_TunionType:
0){ _LL518: _temp517=(( struct Cyc_Absyn_TunionType_struct*) _temp493)->f1;
_LL522: _temp521=( void*) _temp517.tunion_info; if(*(( int*) _temp521) ==  Cyc_Absyn_UnknownTunion){
_LL524: _temp523=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp521)->f1;
_LL528: _temp527= _temp523.name; goto _LL526; _LL526: _temp525= _temp523.is_xtunion;
goto _LL520;} else{ goto _LL501;} _LL520: _temp519= _temp517.targs; goto _LL500;}
else{ goto _LL501;} _LL501: if(( unsigned int) _temp493 >  4u?*(( int*) _temp493)
==  Cyc_Absyn_UnionType: 0){ _LL532: _temp531=(( struct Cyc_Absyn_UnionType_struct*)
_temp493)->f1; goto _LL530; _LL530: _temp529=(( struct Cyc_Absyn_UnionType_struct*)
_temp493)->f2; goto _LL502;} else{ goto _LL503;} _LL503: if(( unsigned int)
_temp493 >  4u?*(( int*) _temp493) ==  Cyc_Absyn_EnumType: 0){ _LL534: _temp533=((
struct Cyc_Absyn_EnumType_struct*) _temp493)->f1; goto _LL504;} else{ goto
_LL505;} _LL505: goto _LL506; _LL496: { struct Cyc_List_List* _temp535=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp507); struct Cyc_Absyn_Structdecl* _temp536=({
struct Cyc_Absyn_Structdecl* _temp540=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp540->sc=( void*) s; _temp540->name=({
struct Cyc_Core_Opt* _temp541=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp541->v=( void*)(( struct _tuple1*) _check_null( _temp509));
_temp541;}); _temp540->tvs= _temp535; _temp540->fields= 0; _temp540->attributes=
0; _temp540;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp537=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp537->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp538=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp538[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp539; _temp539.tag= Cyc_Absyn_Struct_d;
_temp539.f1= _temp536; _temp539;}); _temp538;}), loc); _temp537->tl= 0; _temp537;});}
_LL498: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp542=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp542->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp543=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp543[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp544; _temp544.tag= Cyc_Absyn_Tunion_d;
_temp544.f1=* _temp515; _temp544;}); _temp543;}), loc); _temp542->tl= 0;
_temp542;}); _LL500: { struct Cyc_List_List* _temp545=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp519); struct Cyc_Absyn_Decl* _temp546= Cyc_Absyn_tunion_decl( s, _temp527,
_temp545, 0, _temp525, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp547=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp547->hd=(
void*) _temp546; _temp547->tl= 0; _temp547;});} _LL502: { struct Cyc_List_List*
_temp548=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp529); struct Cyc_Absyn_Uniondecl* _temp549=({
struct Cyc_Absyn_Uniondecl* _temp554=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp554->sc=( void*) s; _temp554->name=({
struct Cyc_Core_Opt* _temp555=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp555->v=( void*)(( struct _tuple1*) _check_null( _temp531));
_temp555;}); _temp554->tvs= _temp548; _temp554->fields= 0; _temp554->attributes=
0; _temp554;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp550=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp550->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp551=( struct Cyc_Absyn_Decl*) _cycalloc(
sizeof( struct Cyc_Absyn_Decl)); _temp551->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp552=( struct Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp552[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp553; _temp553.tag= Cyc_Absyn_Union_d;
_temp553.f1= _temp549; _temp553;}); _temp552;})); _temp551->loc= loc; _temp551;});
_temp550->tl= 0; _temp550;});} _LL504: { struct Cyc_Absyn_Enumdecl* _temp556=({
struct Cyc_Absyn_Enumdecl* _temp561=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp561->sc=( void*) s; _temp561->name=
_temp533; _temp561->fields= 0; _temp561;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp557=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp557->hd=( void*)({ struct Cyc_Absyn_Decl* _temp558=(
struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp558->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp559=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp559[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp560; _temp560.tag= Cyc_Absyn_Enum_d; _temp560.f1= _temp556; _temp560;});
_temp559;})); _temp558->loc= loc; _temp558;}); _temp557->tl= 0; _temp557;});}
_LL506: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL494:;}} else{ void* t= _temp465.f1; struct Cyc_List_List*
_temp562= Cyc_Parse_apply_tmss( tq, t, _temp463, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp562); if( _temp465.f2 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp465.f2))->v;{ void* _temp563=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp575; struct Cyc_Absyn_Tuniondecl* _temp577;
struct Cyc_Absyn_Uniondecl* _temp579; struct Cyc_Absyn_Enumdecl* _temp581;
_LL565: if(*(( int*) _temp563) ==  Cyc_Absyn_Struct_d){ _LL576: _temp575=((
struct Cyc_Absyn_Struct_d_struct*) _temp563)->f1; goto _LL566;} else{ goto
_LL567;} _LL567: if(*(( int*) _temp563) ==  Cyc_Absyn_Tunion_d){ _LL578:
_temp577=(( struct Cyc_Absyn_Tunion_d_struct*) _temp563)->f1; goto _LL568;}
else{ goto _LL569;} _LL569: if(*(( int*) _temp563) ==  Cyc_Absyn_Union_d){
_LL580: _temp579=(( struct Cyc_Absyn_Union_d_struct*) _temp563)->f1; goto _LL570;}
else{ goto _LL571;} _LL571: if(*(( int*) _temp563) ==  Cyc_Absyn_Enum_d){ _LL582:
_temp581=(( struct Cyc_Absyn_Enum_d_struct*) _temp563)->f1; goto _LL572;} else{
goto _LL573;} _LL573: goto _LL574; _LL566:( void*)( _temp575->sc=( void*) s);
_temp575->attributes= atts; atts= 0; goto _LL564; _LL568:( void*)( _temp577->sc=(
void*) s); goto _LL564; _LL570:( void*)( _temp579->sc=( void*) s); goto _LL564;
_LL572:( void*)( _temp581->sc=( void*) s); goto _LL564; _LL574: Cyc_Parse_err(
_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL564; _LL564:;} decls=({ struct Cyc_List_List*
_temp583=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp583->hd=( void*) d; _temp583->tl= decls; _temp583;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp585; _temp585.tag=
Cyc_Std_String_pa; _temp585.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp584[ 1u]={&
_temp585}; Cyc_Std_aprintf( _tag_arr("bad attribute %s in typedef", sizeof(
unsigned char), 28u), _tag_arr( _temp584, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp465.f2 !=  0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp586= _temp562; for( 0; _temp586 !=  0;( _temp586=(( struct
Cyc_List_List*) _check_null( _temp586))->tl, _temp461=(( struct Cyc_List_List*)
_check_null( _temp461))->tl)){ struct _tuple7 _temp589; struct Cyc_List_List*
_temp590; struct Cyc_List_List* _temp592; void* _temp594; struct Cyc_Absyn_Tqual
_temp596; struct _tuple1* _temp598; struct _tuple7* _temp587=( struct _tuple7*)((
struct Cyc_List_List*) _check_null( _temp586))->hd; _temp589=* _temp587; _LL599:
_temp598= _temp589.f1; goto _LL597; _LL597: _temp596= _temp589.f2; goto _LL595;
_LL595: _temp594= _temp589.f3; goto _LL593; _LL593: _temp592= _temp589.f4; goto
_LL591; _LL591: _temp590= _temp589.f5; goto _LL588; _LL588: if( _temp592 !=  0){
Cyc_Parse_warn( _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
loc);} if( _temp461 ==  0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected NULL in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp600=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp461))->hd; struct Cyc_Absyn_Vardecl*
_temp601= Cyc_Absyn_new_vardecl( _temp598, _temp594, _temp600); _temp601->tq=
_temp596;( void*)( _temp601->sc=( void*) s); _temp601->attributes= _temp590;{
struct Cyc_Absyn_Decl* _temp602=({ struct Cyc_Absyn_Decl* _temp604=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp604->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp605=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp605[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp606; _temp606.tag= Cyc_Absyn_Var_d; _temp606.f1= _temp601; _temp606;});
_temp605;})); _temp604->loc= loc; _temp604;}); decls=({ struct Cyc_List_List*
_temp603=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp603->hd=( void*) _temp602; _temp603->tl= decls; _temp603;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_Std_strlen( s) !=  1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp608; _temp608.tag= Cyc_Std_String_pa;
_temp608.f1=( struct _tagged_arr) s;{ void* _temp607[ 1u]={& _temp608}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp607,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL609: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL610: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL611: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL612: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL613: return(
void*) Cyc_Absyn_EffKind; default: _LL614: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp617; _temp617.tag= Cyc_Std_String_pa;
_temp617.f1=( struct _tagged_arr) s;{ void* _temp616[ 1u]={& _temp617}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp616,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts ==  0){ return
tms;} else{ return({ struct Cyc_List_List* _temp618=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp618->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp619=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp619[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp620; _temp620.tag= Cyc_Absyn_Attributes_mod;
_temp620.f1= loc; _temp620.f2= atts; _temp620;}); _temp619;})); _temp618->tl=
tms; _temp618;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp623;
struct Cyc_List_List* _temp624; struct Cyc_List_List* _temp626; void* _temp628;
struct Cyc_Absyn_Tqual _temp630; struct _tuple1* _temp632; struct _tuple7*
_temp621= t; _temp623=* _temp621; _LL633: _temp632= _temp623.f1; goto _LL631;
_LL631: _temp630= _temp623.f2; goto _LL629; _LL629: _temp628= _temp623.f3; goto
_LL627; _LL627: _temp626= _temp623.f4; goto _LL625; _LL625: _temp624= _temp623.f5;
goto _LL622; _LL622: Cyc_Lex_register_typedef( _temp632); if( _temp624 !=  0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp635;
_temp635.tag= Cyc_Std_String_pa; _temp635.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp624))->hd);{ void* _temp634[ 1u]={&
_temp635}; Cyc_Std_aprintf( _tag_arr("bad attribute %s within typedef", sizeof(
unsigned char), 32u), _tag_arr( _temp634, sizeof( void*), 1u));}}), loc);}
return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp636=(
struct Cyc_Absyn_Typedef_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp636[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp637; _temp637.tag= Cyc_Absyn_Typedef_d;
_temp637.f1=({ struct Cyc_Absyn_Typedefdecl* _temp638=( struct Cyc_Absyn_Typedefdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp638->name= _temp632;
_temp638->tvs= _temp626; _temp638->defn=( void*) _temp628; _temp638;}); _temp637;});
_temp636;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
unsigned char _temp640[ 15u]="$(sign_t,int)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp640, _temp640, _temp640 +  15u}};
struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ struct _tuple15* yyzzz;{ void*
_temp641= yy1; struct _tuple15* _temp647; _LL643: if(*(( void**) _temp641) == 
Cyc_Int_tok){ _LL648: _temp647=(( struct Cyc_Int_tok_struct*) _temp641)->f1;
goto _LL644;} else{ goto _LL645;} _LL645: goto _LL646; _LL644: yyzzz= _temp647;
goto _LL642; _LL646:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL642:;}
return yyzzz;} static unsigned char _temp650[ 9u]="string_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp650, _temp650, _temp650 +  9u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ struct _tagged_arr yyzzz;{
void* _temp651= yy1; struct _tagged_arr _temp657; _LL653: if(*(( void**)
_temp651) ==  Cyc_String_tok){ _LL658: _temp657=(( struct Cyc_String_tok_struct*)
_temp651)->f1; goto _LL654;} else{ goto _LL655;} _LL655: goto _LL656; _LL654:
yyzzz= _temp657; goto _LL652; _LL656:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL652:;} return yyzzz;} static unsigned char _temp660[ 5u]="char"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{ _temp660,
_temp660, _temp660 +  5u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
unsigned char yyzzz;{ void* _temp661= yy1; unsigned char _temp667; _LL663: if(*((
void**) _temp661) ==  Cyc_Char_tok){ _LL668: _temp667=(( struct Cyc_Char_tok_struct*)
_temp661)->f1; goto _LL664;} else{ goto _LL665;} _LL665: goto _LL666; _LL664:
yyzzz= _temp667; goto _LL662; _LL666:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL662:;} return yyzzz;} static unsigned char _temp670[ 20u]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{
_temp670, _temp670, _temp670 +  20u}}; void* Cyc_yyget_Pointer_Sort_tok( void*
yy1){ void* yyzzz;{ void* _temp671= yy1; void* _temp677; _LL673: if(*(( void**)
_temp671) ==  Cyc_Pointer_Sort_tok){ _LL678: _temp677=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp671)->f1; goto _LL674;} else{ goto _LL675;} _LL675: goto _LL676; _LL674:
yyzzz= _temp677; goto _LL672; _LL676:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL672:;} return yyzzz;} static unsigned char _temp680[ 6u]="exp_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp680,
_temp680, _temp680 +  6u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp681= yy1; struct Cyc_Absyn_Exp*
_temp687; _LL683: if(*(( void**) _temp681) ==  Cyc_Exp_tok){ _LL688: _temp687=((
struct Cyc_Exp_tok_struct*) _temp681)->f1; goto _LL684;} else{ goto _LL685;}
_LL685: goto _LL686; _LL684: yyzzz= _temp687; goto _LL682; _LL686:( int) _throw((
void*)& Cyc_yyfail_Exp_tok); _LL682:;} return yyzzz;} static unsigned char
_temp690[ 14u]="list_t<exp_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{ _temp690, _temp690, _temp690 +  14u}}; struct Cyc_List_List*
Cyc_yyget_ExpList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp691=
yy1; struct Cyc_List_List* _temp697; _LL693: if(*(( void**) _temp691) ==  Cyc_ExpList_tok){
_LL698: _temp697=(( struct Cyc_ExpList_tok_struct*) _temp691)->f1; goto _LL694;}
else{ goto _LL695;} _LL695: goto _LL696; _LL694: yyzzz= _temp697; goto _LL692;
_LL696:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL692:;} return yyzzz;}
static unsigned char _temp700[ 39u]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{
_temp700, _temp700, _temp700 +  39u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp701= yy1; struct Cyc_List_List*
_temp707; _LL703: if(*(( void**) _temp701) ==  Cyc_InitializerList_tok){ _LL708:
_temp707=(( struct Cyc_InitializerList_tok_struct*) _temp701)->f1; goto _LL704;}
else{ goto _LL705;} _LL705: goto _LL706; _LL704: yyzzz= _temp707; goto _LL702;
_LL706:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL702:;} return
yyzzz;} static unsigned char _temp710[ 9u]="primop_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp710, _temp710, _temp710 +  9u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* yyzzz;{ void* _temp711= yy1; void*
_temp717; _LL713: if(*(( void**) _temp711) ==  Cyc_Primop_tok){ _LL718: _temp717=(
void*)(( struct Cyc_Primop_tok_struct*) _temp711)->f1; goto _LL714;} else{ goto
_LL715;} _LL715: goto _LL716; _LL714: yyzzz= _temp717; goto _LL712; _LL716:( int)
_throw(( void*)& Cyc_yyfail_Primop_tok); _LL712:;} return yyzzz;} static
unsigned char _temp720[ 16u]="opt_t<primop_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp720, _temp720, _temp720 +  16u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ struct Cyc_Core_Opt*
yyzzz;{ void* _temp721= yy1; struct Cyc_Core_Opt* _temp727; _LL723: if(*(( void**)
_temp721) ==  Cyc_Primopopt_tok){ _LL728: _temp727=(( struct Cyc_Primopopt_tok_struct*)
_temp721)->f1; goto _LL724;} else{ goto _LL725;} _LL725: goto _LL726; _LL724:
yyzzz= _temp727; goto _LL722; _LL726:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL722:;} return yyzzz;} static unsigned char _temp730[ 7u]="qvar_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{
_temp730, _temp730, _temp730 +  7u}}; struct _tuple1* Cyc_yyget_QualId_tok( void*
yy1){ struct _tuple1* yyzzz;{ void* _temp731= yy1; struct _tuple1* _temp737;
_LL733: if(*(( void**) _temp731) ==  Cyc_QualId_tok){ _LL738: _temp737=(( struct
Cyc_QualId_tok_struct*) _temp731)->f1; goto _LL734;} else{ goto _LL735;} _LL735:
goto _LL736; _LL734: yyzzz= _temp737; goto _LL732; _LL736:( int) _throw(( void*)&
Cyc_yyfail_QualId_tok); _LL732:;} return yyzzz;} static unsigned char _temp740[
7u]="stmt_t"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{
_temp740, _temp740, _temp740 +  7u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok(
void* yy1){ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp741= yy1; struct Cyc_Absyn_Stmt*
_temp747; _LL743: if(*(( void**) _temp741) ==  Cyc_Stmt_tok){ _LL748: _temp747=((
struct Cyc_Stmt_tok_struct*) _temp741)->f1; goto _LL744;} else{ goto _LL745;}
_LL745: goto _LL746; _LL744: yyzzz= _temp747; goto _LL742; _LL746:( int) _throw((
void*)& Cyc_yyfail_Stmt_tok); _LL742:;} return yyzzz;} static unsigned char
_temp750[ 24u]="list_t<switch_clause_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp750, _temp750,
_temp750 +  24u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ struct Cyc_List_List* yyzzz;{ void* _temp751= yy1; struct Cyc_List_List*
_temp757; _LL753: if(*(( void**) _temp751) ==  Cyc_SwitchClauseList_tok){ _LL758:
_temp757=(( struct Cyc_SwitchClauseList_tok_struct*) _temp751)->f1; goto _LL754;}
else{ goto _LL755;} _LL755: goto _LL756; _LL754: yyzzz= _temp757; goto _LL752;
_LL756:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL752:;}
return yyzzz;} static unsigned char _temp760[ 25u]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp760, _temp760, _temp760 +  25u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp761= yy1; struct Cyc_List_List*
_temp767; _LL763: if(*(( void**) _temp761) ==  Cyc_SwitchCClauseList_tok){
_LL768: _temp767=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp761)->f1;
goto _LL764;} else{ goto _LL765;} _LL765: goto _LL766; _LL764: yyzzz= _temp767;
goto _LL762; _LL766:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL762:;} return yyzzz;} static unsigned char _temp770[ 6u]="pat_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{
_temp770, _temp770, _temp770 +  6u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
void* yy1){ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp771= yy1; struct Cyc_Absyn_Pat*
_temp777; _LL773: if(*(( void**) _temp771) ==  Cyc_Pattern_tok){ _LL778:
_temp777=(( struct Cyc_Pattern_tok_struct*) _temp771)->f1; goto _LL774;} else{
goto _LL775;} _LL775: goto _LL776; _LL774: yyzzz= _temp777; goto _LL772; _LL776:(
int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL772:;} return yyzzz;} static
unsigned char _temp780[ 14u]="list_t<pat_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp780, _temp780, _temp780 + 
14u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp781= yy1; struct Cyc_List_List* _temp787; _LL783: if(*(( void**)
_temp781) ==  Cyc_PatternList_tok){ _LL788: _temp787=(( struct Cyc_PatternList_tok_struct*)
_temp781)->f1; goto _LL784;} else{ goto _LL785;} _LL785: goto _LL786; _LL784:
yyzzz= _temp787; goto _LL782; _LL786:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL782:;} return yyzzz;} static unsigned char _temp790[ 31u]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp790, _temp790, _temp790 +  31u}}; struct _tuple13* Cyc_yyget_FieldPattern_tok(
void* yy1){ struct _tuple13* yyzzz;{ void* _temp791= yy1; struct _tuple13*
_temp797; _LL793: if(*(( void**) _temp791) ==  Cyc_FieldPattern_tok){ _LL798:
_temp797=(( struct Cyc_FieldPattern_tok_struct*) _temp791)->f1; goto _LL794;}
else{ goto _LL795;} _LL795: goto _LL796; _LL794: yyzzz= _temp797; goto _LL792;
_LL796:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL792:;} return
yyzzz;} static unsigned char _temp800[ 39u]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp800, _temp800, _temp800 +  39u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp801= yy1; struct Cyc_List_List*
_temp807; _LL803: if(*(( void**) _temp801) ==  Cyc_FieldPatternList_tok){ _LL808:
_temp807=(( struct Cyc_FieldPatternList_tok_struct*) _temp801)->f1; goto _LL804;}
else{ goto _LL805;} _LL805: goto _LL806; _LL804: yyzzz= _temp807; goto _LL802;
_LL806:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL802:;}
return yyzzz;} static unsigned char _temp810[ 9u]="fndecl_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp810, _temp810, _temp810 +  9u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ struct Cyc_Absyn_Fndecl*
yyzzz;{ void* _temp811= yy1; struct Cyc_Absyn_Fndecl* _temp817; _LL813: if(*((
void**) _temp811) ==  Cyc_FnDecl_tok){ _LL818: _temp817=(( struct Cyc_FnDecl_tok_struct*)
_temp811)->f1; goto _LL814;} else{ goto _LL815;} _LL815: goto _LL816; _LL814:
yyzzz= _temp817; goto _LL812; _LL816:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL812:;} return yyzzz;} static unsigned char _temp820[ 15u]="list_t<decl_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp820, _temp820, _temp820 +  15u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp821= yy1; struct Cyc_List_List*
_temp827; _LL823: if(*(( void**) _temp821) ==  Cyc_DeclList_tok){ _LL828:
_temp827=(( struct Cyc_DeclList_tok_struct*) _temp821)->f1; goto _LL824;} else{
goto _LL825;} _LL825: goto _LL826; _LL824: yyzzz= _temp827; goto _LL822; _LL826:(
int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL822:;} return yyzzz;} static
unsigned char _temp830[ 12u]="decl_spec_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp830, _temp830, _temp830 +  12u}};
struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){ struct
Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp831= yy1; struct Cyc_Parse_Declaration_spec*
_temp837; _LL833: if(*(( void**) _temp831) ==  Cyc_DeclSpec_tok){ _LL838:
_temp837=(( struct Cyc_DeclSpec_tok_struct*) _temp831)->f1; goto _LL834;} else{
goto _LL835;} _LL835: goto _LL836; _LL834: yyzzz= _temp837; goto _LL832; _LL836:(
int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL832:;} return yyzzz;} static
unsigned char _temp840[ 27u]="$(declarator_t,exp_opt_t)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp840, _temp840, _temp840 +  27u}};
struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){ struct _tuple14* yyzzz;{
void* _temp841= yy1; struct _tuple14* _temp847; _LL843: if(*(( void**) _temp841)
==  Cyc_InitDecl_tok){ _LL848: _temp847=(( struct Cyc_InitDecl_tok_struct*)
_temp841)->f1; goto _LL844;} else{ goto _LL845;} _LL845: goto _LL846; _LL844:
yyzzz= _temp847; goto _LL842; _LL846:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL842:;} return yyzzz;} static unsigned char _temp850[ 35u]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp850, _temp850, _temp850 +  35u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp851= yy1; struct Cyc_List_List*
_temp857; _LL853: if(*(( void**) _temp851) ==  Cyc_InitDeclList_tok){ _LL858:
_temp857=(( struct Cyc_InitDeclList_tok_struct*) _temp851)->f1; goto _LL854;}
else{ goto _LL855;} _LL855: goto _LL856; _LL854: yyzzz= _temp857; goto _LL852;
_LL856:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL852:;} return
yyzzz;} static unsigned char _temp860[ 16u]="storage_class_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp860, _temp860, _temp860 + 
16u}}; enum  Cyc_Parse_Storage_class Cyc_yyget_StorageClass_tok( void* yy1){
enum  Cyc_Parse_Storage_class yyzzz;{ void* _temp861= yy1; enum  Cyc_Parse_Storage_class
_temp867; _LL863: if(*(( void**) _temp861) ==  Cyc_StorageClass_tok){ _LL868:
_temp867=(( struct Cyc_StorageClass_tok_struct*) _temp861)->f1; goto _LL864;}
else{ goto _LL865;} _LL865: goto _LL866; _LL864: yyzzz= _temp867; goto _LL862;
_LL866:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL862:;} return
yyzzz;} static unsigned char _temp870[ 17u]="type_specifier_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp870, _temp870, _temp870 +  17u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* yyzzz;{ void* _temp871= yy1; void* _temp877; _LL873: if(*(( void**)
_temp871) ==  Cyc_TypeSpecifier_tok){ _LL878: _temp877=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp871)->f1; goto _LL874;} else{ goto _LL875;} _LL875: goto _LL876; _LL874:
yyzzz= _temp877; goto _LL872; _LL876:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL872:;} return yyzzz;} static unsigned char _temp880[ 18u]="struct_or_union_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp880, _temp880, _temp880 +  18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ enum  Cyc_Parse_Struct_or_union yyzzz;{ void* _temp881= yy1; enum 
Cyc_Parse_Struct_or_union _temp887; _LL883: if(*(( void**) _temp881) ==  Cyc_StructOrUnion_tok){
_LL888: _temp887=(( struct Cyc_StructOrUnion_tok_struct*) _temp881)->f1; goto
_LL884;} else{ goto _LL885;} _LL885: goto _LL886; _LL884: yyzzz= _temp887; goto
_LL882; _LL886:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok); _LL882:;}
return yyzzz;} static unsigned char _temp890[ 8u]="tqual_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp890, _temp890, _temp890 +  8u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp891= yy1; struct Cyc_Absyn_Tqual _temp897; _LL893: if(*((
void**) _temp891) ==  Cyc_TypeQual_tok){ _LL898: _temp897=(( struct Cyc_TypeQual_tok_struct*)
_temp891)->f1; goto _LL894;} else{ goto _LL895;} _LL895: goto _LL896; _LL894:
yyzzz= _temp897; goto _LL892; _LL896:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL892:;} return yyzzz;} static unsigned char _temp900[ 22u]="list_t<structfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp900, _temp900, _temp900 +  22u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp901= yy1; struct Cyc_List_List*
_temp907; _LL903: if(*(( void**) _temp901) ==  Cyc_StructFieldDeclList_tok){
_LL908: _temp907=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp901)->f1;
goto _LL904;} else{ goto _LL905;} _LL905: goto _LL906; _LL904: yyzzz= _temp907;
goto _LL902; _LL906:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL902:;} return yyzzz;} static unsigned char _temp910[ 30u]="list_t<list_t<structfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp910, _temp910, _temp910 +  30u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{
void* _temp911= yy1; struct Cyc_List_List* _temp917; _LL913: if(*(( void**)
_temp911) ==  Cyc_StructFieldDeclListList_tok){ _LL918: _temp917=(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp911)->f1; goto _LL914;} else{ goto _LL915;} _LL915: goto _LL916; _LL914:
yyzzz= _temp917; goto _LL912; _LL916:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL912:;} return yyzzz;} static unsigned char _temp920[ 24u]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{
_temp920, _temp920, _temp920 +  24u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp921= yy1; struct Cyc_List_List*
_temp927; _LL923: if(*(( void**) _temp921) ==  Cyc_TypeModifierList_tok){ _LL928:
_temp927=(( struct Cyc_TypeModifierList_tok_struct*) _temp921)->f1; goto _LL924;}
else{ goto _LL925;} _LL925: goto _LL926; _LL924: yyzzz= _temp927; goto _LL922;
_LL926:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL922:;}
return yyzzz;} static unsigned char _temp930[ 13u]="declarator_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp930,
_temp930, _temp930 +  13u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp931= yy1; struct Cyc_Parse_Declarator*
_temp937; _LL933: if(*(( void**) _temp931) ==  Cyc_Declarator_tok){ _LL938:
_temp937=(( struct Cyc_Declarator_tok_struct*) _temp931)->f1; goto _LL934;}
else{ goto _LL935;} _LL935: goto _LL936; _LL934: yyzzz= _temp937; goto _LL932;
_LL936:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL932:;} return
yyzzz;} static unsigned char _temp940[ 21u]="abstractdeclarator_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{
_temp940, _temp940, _temp940 +  21u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp941= yy1;
struct Cyc_Parse_Abstractdeclarator* _temp947; _LL943: if(*(( void**) _temp941)
==  Cyc_AbstractDeclarator_tok){ _LL948: _temp947=(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp941)->f1; goto _LL944;} else{ goto _LL945;} _LL945: goto _LL946; _LL944:
yyzzz= _temp947; goto _LL942; _LL946:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL942:;} return yyzzz;} static unsigned char _temp950[ 5u]="bool"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp950,
_temp950, _temp950 +  5u}}; int Cyc_yyget_Bool_tok( void* yy1){ int yyzzz;{ void*
_temp951= yy1; int _temp957; _LL953: if(*(( void**) _temp951) ==  Cyc_Bool_tok){
_LL958: _temp957=(( struct Cyc_Bool_tok_struct*) _temp951)->f1; goto _LL954;}
else{ goto _LL955;} _LL955: goto _LL956; _LL954: yyzzz= _temp957; goto _LL952;
_LL956:( int) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL952:;} return yyzzz;}
static unsigned char _temp960[ 8u]="scope_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp960, _temp960, _temp960 +  8u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* yyzzz;{ void* _temp961= yy1; void*
_temp967; _LL963: if(*(( void**) _temp961) ==  Cyc_Scope_tok){ _LL968: _temp967=(
void*)(( struct Cyc_Scope_tok_struct*) _temp961)->f1; goto _LL964;} else{ goto
_LL965;} _LL965: goto _LL966; _LL964: yyzzz= _temp967; goto _LL962; _LL966:( int)
_throw(( void*)& Cyc_yyfail_Scope_tok); _LL962:;} return yyzzz;} static
unsigned char _temp970[ 14u]="tunionfield_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{ _temp970, _temp970, _temp970 + 
14u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok( void* yy1){
struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp971= yy1; struct Cyc_Absyn_Tunionfield*
_temp977; _LL973: if(*(( void**) _temp971) ==  Cyc_TunionField_tok){ _LL978:
_temp977=(( struct Cyc_TunionField_tok_struct*) _temp971)->f1; goto _LL974;}
else{ goto _LL975;} _LL975: goto _LL976; _LL974: yyzzz= _temp977; goto _LL972;
_LL976:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok); _LL972:;} return
yyzzz;} static unsigned char _temp980[ 22u]="list_t<tunionfield_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp980, _temp980, _temp980 +  22u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp981= yy1; struct Cyc_List_List*
_temp987; _LL983: if(*(( void**) _temp981) ==  Cyc_TunionFieldList_tok){ _LL988:
_temp987=(( struct Cyc_TunionFieldList_tok_struct*) _temp981)->f1; goto _LL984;}
else{ goto _LL985;} _LL985: goto _LL986; _LL984: yyzzz= _temp987; goto _LL982;
_LL986:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok); _LL982:;} return
yyzzz;} static unsigned char _temp990[ 50u]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp990, _temp990, _temp990 +  50u}}; struct _tuple17* Cyc_yyget_QualSpecList_tok(
void* yy1){ struct _tuple17* yyzzz;{ void* _temp991= yy1; struct _tuple17*
_temp997; _LL993: if(*(( void**) _temp991) ==  Cyc_QualSpecList_tok){ _LL998:
_temp997=(( struct Cyc_QualSpecList_tok_struct*) _temp991)->f1; goto _LL994;}
else{ goto _LL995;} _LL995: goto _LL996; _LL994: yyzzz= _temp997; goto _LL992;
_LL996:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL992:;} return
yyzzz;} static unsigned char _temp1000[ 14u]="list_t<var_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1000, _temp1000, _temp1000 +  14u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp1001= yy1; struct Cyc_List_List* _temp1007; _LL1003: if(*((
void**) _temp1001) ==  Cyc_IdList_tok){ _LL1008: _temp1007=(( struct Cyc_IdList_tok_struct*)
_temp1001)->f1; goto _LL1004;} else{ goto _LL1005;} _LL1005: goto _LL1006;
_LL1004: yyzzz= _temp1007; goto _LL1002; _LL1006:( int) _throw(( void*)& Cyc_yyfail_IdList_tok);
_LL1002:;} return yyzzz;} static unsigned char _temp1010[ 32u]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{
_temp1010, _temp1010, _temp1010 +  32u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ struct _tuple2* yyzzz;{ void* _temp1011= yy1; struct _tuple2*
_temp1017; _LL1013: if(*(( void**) _temp1011) ==  Cyc_ParamDecl_tok){ _LL1018:
_temp1017=(( struct Cyc_ParamDecl_tok_struct*) _temp1011)->f1; goto _LL1014;}
else{ goto _LL1015;} _LL1015: goto _LL1016; _LL1014: yyzzz= _temp1017; goto
_LL1012; _LL1016:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1012:;}
return yyzzz;} static unsigned char _temp1020[ 40u]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{
_temp1020, _temp1020, _temp1020 +  40u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1021= yy1; struct Cyc_List_List*
_temp1027; _LL1023: if(*(( void**) _temp1021) ==  Cyc_ParamDeclList_tok){
_LL1028: _temp1027=(( struct Cyc_ParamDeclList_tok_struct*) _temp1021)->f1; goto
_LL1024;} else{ goto _LL1025;} _LL1025: goto _LL1026; _LL1024: yyzzz= _temp1027;
goto _LL1022; _LL1026:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1022:;} return yyzzz;} static unsigned char _temp1030[ 107u]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={ Cyc_Core_Failure,{
_temp1030, _temp1030, _temp1030 +  107u}}; struct _tuple16* Cyc_yyget_YY1( void*
yy1){ struct _tuple16* yyzzz;{ void* _temp1031= yy1; struct _tuple16* _temp1037;
_LL1033: if(*(( void**) _temp1031) ==  Cyc_YY1){ _LL1038: _temp1037=(( struct
Cyc_YY1_struct*) _temp1031)->f1; goto _LL1034;} else{ goto _LL1035;} _LL1035:
goto _LL1036; _LL1034: yyzzz= _temp1037; goto _LL1032; _LL1036:( int) _throw((
void*)& Cyc_yyfail_YY1); _LL1032:;} return yyzzz;} static unsigned char
_temp1040[ 15u]="list_t<type_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1040, _temp1040, _temp1040 +  15u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void*
_temp1041= yy1; struct Cyc_List_List* _temp1047; _LL1043: if(*(( void**)
_temp1041) ==  Cyc_TypeList_tok){ _LL1048: _temp1047=(( struct Cyc_TypeList_tok_struct*)
_temp1041)->f1; goto _LL1044;} else{ goto _LL1045;} _LL1045: goto _LL1046;
_LL1044: yyzzz= _temp1047; goto _LL1042; _LL1046:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok);
_LL1042:;} return yyzzz;} static unsigned char _temp1050[ 21u]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{
_temp1050, _temp1050, _temp1050 +  21u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1051= yy1; struct Cyc_List_List*
_temp1057; _LL1053: if(*(( void**) _temp1051) ==  Cyc_DesignatorList_tok){
_LL1058: _temp1057=(( struct Cyc_DesignatorList_tok_struct*) _temp1051)->f1;
goto _LL1054;} else{ goto _LL1055;} _LL1055: goto _LL1056; _LL1054: yyzzz=
_temp1057; goto _LL1052; _LL1056:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1052:;} return yyzzz;} static unsigned char _temp1060[ 13u]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{
_temp1060, _temp1060, _temp1060 +  13u}}; void* Cyc_yyget_Designator_tok( void*
yy1){ void* yyzzz;{ void* _temp1061= yy1; void* _temp1067; _LL1063: if(*(( void**)
_temp1061) ==  Cyc_Designator_tok){ _LL1068: _temp1067=( void*)(( struct Cyc_Designator_tok_struct*)
_temp1061)->f1; goto _LL1064;} else{ goto _LL1065;} _LL1065: goto _LL1066;
_LL1064: yyzzz= _temp1067; goto _LL1062; _LL1066:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL1062:;} return yyzzz;} static unsigned char _temp1070[ 7u]="kind_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{
_temp1070, _temp1070, _temp1070 +  7u}}; void* Cyc_yyget_Kind_tok( void* yy1){
void* yyzzz;{ void* _temp1071= yy1; void* _temp1077; _LL1073: if(*(( void**)
_temp1071) ==  Cyc_Kind_tok){ _LL1078: _temp1077=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1071)->f1; goto _LL1074;} else{ goto _LL1075;} _LL1075: goto _LL1076;
_LL1074: yyzzz= _temp1077; goto _LL1072; _LL1076:( int) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1072:;} return yyzzz;} static unsigned char _temp1080[ 7u]="type_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1080, _temp1080, _temp1080 +  7u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* yyzzz;{ void* _temp1081= yy1; void* _temp1087; _LL1083: if(*(( void**)
_temp1081) ==  Cyc_Type_tok){ _LL1088: _temp1087=( void*)(( struct Cyc_Type_tok_struct*)
_temp1081)->f1; goto _LL1084;} else{ goto _LL1085;} _LL1085: goto _LL1086;
_LL1084: yyzzz= _temp1087; goto _LL1082; _LL1086:( int) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1082:;} return yyzzz;} static unsigned char _temp1090[ 20u]="list_t<attribute_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1090, _temp1090, _temp1090 +  20u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1091= yy1; struct Cyc_List_List*
_temp1097; _LL1093: if(*(( void**) _temp1091) ==  Cyc_AttributeList_tok){
_LL1098: _temp1097=(( struct Cyc_AttributeList_tok_struct*) _temp1091)->f1; goto
_LL1094;} else{ goto _LL1095;} _LL1095: goto _LL1096; _LL1094: yyzzz= _temp1097;
goto _LL1092; _LL1096:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1092:;} return yyzzz;} static unsigned char _temp1100[ 12u]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{
_temp1100, _temp1100, _temp1100 +  12u}}; void* Cyc_yyget_Attribute_tok( void*
yy1){ void* yyzzz;{ void* _temp1101= yy1; void* _temp1107; _LL1103: if(*(( void**)
_temp1101) ==  Cyc_Attribute_tok){ _LL1108: _temp1107=( void*)(( struct Cyc_Attribute_tok_struct*)
_temp1101)->f1; goto _LL1104;} else{ goto _LL1105;} _LL1105: goto _LL1106;
_LL1104: yyzzz= _temp1107; goto _LL1102; _LL1106:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1102:;} return yyzzz;} static unsigned char _temp1110[ 12u]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{
_temp1110, _temp1110, _temp1110 +  12u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
void* yy1){ struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp1111= yy1; struct Cyc_Absyn_Enumfield*
_temp1117; _LL1113: if(*(( void**) _temp1111) ==  Cyc_Enumfield_tok){ _LL1118:
_temp1117=(( struct Cyc_Enumfield_tok_struct*) _temp1111)->f1; goto _LL1114;}
else{ goto _LL1115;} _LL1115: goto _LL1116; _LL1114: yyzzz= _temp1117; goto
_LL1112; _LL1116:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1112:;}
return yyzzz;} static unsigned char _temp1120[ 20u]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1120, _temp1120, _temp1120 +  20u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1121= yy1; struct Cyc_List_List*
_temp1127; _LL1123: if(*(( void**) _temp1121) ==  Cyc_EnumfieldList_tok){
_LL1128: _temp1127=(( struct Cyc_EnumfieldList_tok_struct*) _temp1121)->f1; goto
_LL1124;} else{ goto _LL1125;} _LL1125: goto _LL1126; _LL1124: yyzzz= _temp1127;
goto _LL1122; _LL1126:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1122:;} return yyzzz;} static unsigned char _temp1130[ 14u]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{
_temp1130, _temp1130, _temp1130 +  14u}}; struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok(
void* yy1){ struct Cyc_Core_Opt* yyzzz;{ void* _temp1131= yy1; struct Cyc_Core_Opt*
_temp1137; _LL1133: if(*(( void**) _temp1131) ==  Cyc_TypeOpt_tok){ _LL1138:
_temp1137=(( struct Cyc_TypeOpt_tok_struct*) _temp1131)->f1; goto _LL1134;}
else{ goto _LL1135;} _LL1135: goto _LL1136; _LL1134: yyzzz= _temp1137; goto
_LL1132; _LL1136:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1132:;}
return yyzzz;} static unsigned char _temp1140[ 26u]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{
_temp1140, _temp1140, _temp1140 +  26u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1141= yy1; struct Cyc_List_List*
_temp1147; _LL1143: if(*(( void**) _temp1141) ==  Cyc_Rgnorder_tok){ _LL1148:
_temp1147=(( struct Cyc_Rgnorder_tok_struct*) _temp1141)->f1; goto _LL1144;}
else{ goto _LL1145;} _LL1145: goto _LL1146; _LL1144: yyzzz= _temp1147; goto
_LL1142; _LL1146:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1142:;}
return yyzzz;} struct Cyc_Yyltype{ int timestamp; int first_line; int
first_column; int last_line; int last_column; } ; struct Cyc_Yyltype Cyc_yynewloc(){
return({ struct Cyc_Yyltype _temp1149; _temp1149.timestamp= 0; _temp1149.first_line=
0; _temp1149.first_column= 0; _temp1149.last_line= 0; _temp1149.last_column= 0;
_temp1149;});} struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[
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
unsigned char _temp1150[ 2u]="$"; static unsigned char _temp1151[ 6u]="error";
static unsigned char _temp1152[ 12u]="$undefined."; static unsigned char
_temp1153[ 5u]="AUTO"; static unsigned char _temp1154[ 9u]="REGISTER"; static
unsigned char _temp1155[ 7u]="STATIC"; static unsigned char _temp1156[ 7u]="EXTERN";
static unsigned char _temp1157[ 8u]="TYPEDEF"; static unsigned char _temp1158[ 5u]="VOID";
static unsigned char _temp1159[ 5u]="CHAR"; static unsigned char _temp1160[ 6u]="SHORT";
static unsigned char _temp1161[ 4u]="INT"; static unsigned char _temp1162[ 5u]="LONG";
static unsigned char _temp1163[ 6u]="FLOAT"; static unsigned char _temp1164[ 7u]="DOUBLE";
static unsigned char _temp1165[ 7u]="SIGNED"; static unsigned char _temp1166[ 9u]="UNSIGNED";
static unsigned char _temp1167[ 6u]="CONST"; static unsigned char _temp1168[ 9u]="VOLATILE";
static unsigned char _temp1169[ 9u]="RESTRICT"; static unsigned char _temp1170[
7u]="STRUCT"; static unsigned char _temp1171[ 6u]="UNION"; static unsigned char
_temp1172[ 5u]="CASE"; static unsigned char _temp1173[ 8u]="DEFAULT"; static
unsigned char _temp1174[ 7u]="INLINE"; static unsigned char _temp1175[ 7u]="SIZEOF";
static unsigned char _temp1176[ 9u]="OFFSETOF"; static unsigned char _temp1177[
3u]="IF"; static unsigned char _temp1178[ 5u]="ELSE"; static unsigned char
_temp1179[ 7u]="SWITCH"; static unsigned char _temp1180[ 6u]="WHILE"; static
unsigned char _temp1181[ 3u]="DO"; static unsigned char _temp1182[ 4u]="FOR";
static unsigned char _temp1183[ 5u]="GOTO"; static unsigned char _temp1184[ 9u]="CONTINUE";
static unsigned char _temp1185[ 6u]="BREAK"; static unsigned char _temp1186[ 7u]="RETURN";
static unsigned char _temp1187[ 5u]="ENUM"; static unsigned char _temp1188[ 8u]="NULL_kw";
static unsigned char _temp1189[ 4u]="LET"; static unsigned char _temp1190[ 6u]="THROW";
static unsigned char _temp1191[ 4u]="TRY"; static unsigned char _temp1192[ 6u]="CATCH";
static unsigned char _temp1193[ 4u]="NEW"; static unsigned char _temp1194[ 9u]="ABSTRACT";
static unsigned char _temp1195[ 9u]="FALLTHRU"; static unsigned char _temp1196[
6u]="USING"; static unsigned char _temp1197[ 10u]="NAMESPACE"; static
unsigned char _temp1198[ 7u]="TUNION"; static unsigned char _temp1199[ 8u]="XTUNION";
static unsigned char _temp1200[ 5u]="FILL"; static unsigned char _temp1201[ 8u]="CODEGEN";
static unsigned char _temp1202[ 4u]="CUT"; static unsigned char _temp1203[ 7u]="SPLICE";
static unsigned char _temp1204[ 7u]="MALLOC"; static unsigned char _temp1205[ 9u]="REGION_T";
static unsigned char _temp1206[ 7u]="REGION"; static unsigned char _temp1207[ 5u]="RNEW";
static unsigned char _temp1208[ 8u]="RMALLOC"; static unsigned char _temp1209[ 8u]="REGIONS";
static unsigned char _temp1210[ 4u]="GEN"; static unsigned char _temp1211[ 7u]="PTR_OP";
static unsigned char _temp1212[ 7u]="INC_OP"; static unsigned char _temp1213[ 7u]="DEC_OP";
static unsigned char _temp1214[ 8u]="LEFT_OP"; static unsigned char _temp1215[ 9u]="RIGHT_OP";
static unsigned char _temp1216[ 6u]="LE_OP"; static unsigned char _temp1217[ 6u]="GE_OP";
static unsigned char _temp1218[ 6u]="EQ_OP"; static unsigned char _temp1219[ 6u]="NE_OP";
static unsigned char _temp1220[ 7u]="AND_OP"; static unsigned char _temp1221[ 6u]="OR_OP";
static unsigned char _temp1222[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1223[ 11u]="DIV_ASSIGN"; static unsigned char _temp1224[ 11u]="MOD_ASSIGN";
static unsigned char _temp1225[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1226[ 11u]="SUB_ASSIGN"; static unsigned char _temp1227[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1228[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1229[ 11u]="AND_ASSIGN"; static unsigned char _temp1230[ 11u]="XOR_ASSIGN";
static unsigned char _temp1231[ 10u]="OR_ASSIGN"; static unsigned char _temp1232[
9u]="ELLIPSIS"; static unsigned char _temp1233[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1234[ 12u]="COLON_COLON"; static unsigned char _temp1235[ 11u]="IDENTIFIER";
static unsigned char _temp1236[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1237[ 7u]="STRING"; static unsigned char _temp1238[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1239[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1240[ 9u]="TYPE_VAR"; static unsigned char _temp1241[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1242[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1243[ 10u]="ATTRIBUTE"; static unsigned char _temp1244[ 4u]="';'"; static
unsigned char _temp1245[ 4u]="'{'"; static unsigned char _temp1246[ 4u]="'}'";
static unsigned char _temp1247[ 4u]="'='"; static unsigned char _temp1248[ 4u]="'('";
static unsigned char _temp1249[ 4u]="')'"; static unsigned char _temp1250[ 4u]="','";
static unsigned char _temp1251[ 4u]="'_'"; static unsigned char _temp1252[ 4u]="'$'";
static unsigned char _temp1253[ 4u]="'<'"; static unsigned char _temp1254[ 4u]="'>'";
static unsigned char _temp1255[ 4u]="':'"; static unsigned char _temp1256[ 4u]="'.'";
static unsigned char _temp1257[ 4u]="'['"; static unsigned char _temp1258[ 4u]="']'";
static unsigned char _temp1259[ 4u]="'*'"; static unsigned char _temp1260[ 4u]="'@'";
static unsigned char _temp1261[ 4u]="'?'"; static unsigned char _temp1262[ 4u]="'+'";
static unsigned char _temp1263[ 4u]="'-'"; static unsigned char _temp1264[ 4u]="'&'";
static unsigned char _temp1265[ 4u]="'|'"; static unsigned char _temp1266[ 4u]="'^'";
static unsigned char _temp1267[ 4u]="'/'"; static unsigned char _temp1268[ 4u]="'%'";
static unsigned char _temp1269[ 4u]="'~'"; static unsigned char _temp1270[ 4u]="'!'";
static unsigned char _temp1271[ 5u]="prog"; static unsigned char _temp1272[ 17u]="translation_unit";
static unsigned char _temp1273[ 21u]="external_declaration"; static
unsigned char _temp1274[ 20u]="function_definition"; static unsigned char
_temp1275[ 21u]="function_definition2"; static unsigned char _temp1276[ 13u]="using_action";
static unsigned char _temp1277[ 15u]="unusing_action"; static unsigned char
_temp1278[ 17u]="namespace_action"; static unsigned char _temp1279[ 19u]="unnamespace_action";
static unsigned char _temp1280[ 12u]="declaration"; static unsigned char
_temp1281[ 17u]="declaration_list"; static unsigned char _temp1282[ 23u]="declaration_specifiers";
static unsigned char _temp1283[ 24u]="storage_class_specifier"; static
unsigned char _temp1284[ 15u]="attributes_opt"; static unsigned char _temp1285[
11u]="attributes"; static unsigned char _temp1286[ 15u]="attribute_list"; static
unsigned char _temp1287[ 10u]="attribute"; static unsigned char _temp1288[ 15u]="type_specifier";
static unsigned char _temp1289[ 5u]="kind"; static unsigned char _temp1290[ 15u]="type_qualifier";
static unsigned char _temp1291[ 15u]="enum_specifier"; static unsigned char
_temp1292[ 11u]="enum_field"; static unsigned char _temp1293[ 22u]="enum_declaration_list";
static unsigned char _temp1294[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1295[ 16u]="type_params_opt"; static unsigned char _temp1296[
16u]="struct_or_union"; static unsigned char _temp1297[ 24u]="struct_declaration_list";
static unsigned char _temp1298[ 25u]="struct_declaration_list0"; static
unsigned char _temp1299[ 21u]="init_declarator_list"; static unsigned char
_temp1300[ 22u]="init_declarator_list0"; static unsigned char _temp1301[ 16u]="init_declarator";
static unsigned char _temp1302[ 19u]="struct_declaration"; static unsigned char
_temp1303[ 25u]="specifier_qualifier_list"; static unsigned char _temp1304[ 23u]="struct_declarator_list";
static unsigned char _temp1305[ 24u]="struct_declarator_list0"; static
unsigned char _temp1306[ 18u]="struct_declarator"; static unsigned char
_temp1307[ 17u]="tunion_specifier"; static unsigned char _temp1308[ 18u]="tunion_or_xtunion";
static unsigned char _temp1309[ 17u]="tunionfield_list"; static unsigned char
_temp1310[ 18u]="tunionfield_scope"; static unsigned char _temp1311[ 12u]="tunionfield";
static unsigned char _temp1312[ 11u]="declarator"; static unsigned char
_temp1313[ 18u]="direct_declarator"; static unsigned char _temp1314[ 8u]="pointer";
static unsigned char _temp1315[ 13u]="pointer_char"; static unsigned char
_temp1316[ 8u]="rgn_opt"; static unsigned char _temp1317[ 4u]="rgn"; static
unsigned char _temp1318[ 20u]="type_qualifier_list"; static unsigned char
_temp1319[ 20u]="parameter_type_list"; static unsigned char _temp1320[ 9u]="type_var";
static unsigned char _temp1321[ 16u]="optional_effect"; static unsigned char
_temp1322[ 19u]="optional_rgn_order"; static unsigned char _temp1323[ 10u]="rgn_order";
static unsigned char _temp1324[ 16u]="optional_inject"; static unsigned char
_temp1325[ 11u]="effect_set"; static unsigned char _temp1326[ 14u]="atomic_effect";
static unsigned char _temp1327[ 11u]="region_set"; static unsigned char
_temp1328[ 15u]="parameter_list"; static unsigned char _temp1329[ 22u]="parameter_declaration";
static unsigned char _temp1330[ 16u]="identifier_list"; static unsigned char
_temp1331[ 17u]="identifier_list0"; static unsigned char _temp1332[ 12u]="initializer";
static unsigned char _temp1333[ 18u]="array_initializer"; static unsigned char
_temp1334[ 17u]="initializer_list"; static unsigned char _temp1335[ 12u]="designation";
static unsigned char _temp1336[ 16u]="designator_list"; static unsigned char
_temp1337[ 11u]="designator"; static unsigned char _temp1338[ 10u]="type_name";
static unsigned char _temp1339[ 14u]="any_type_name"; static unsigned char
_temp1340[ 15u]="type_name_list"; static unsigned char _temp1341[ 20u]="abstract_declarator";
static unsigned char _temp1342[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1343[ 10u]="statement"; static unsigned char _temp1344[ 18u]="labeled_statement";
static unsigned char _temp1345[ 21u]="expression_statement"; static
unsigned char _temp1346[ 19u]="compound_statement"; static unsigned char
_temp1347[ 16u]="block_item_list"; static unsigned char _temp1348[ 20u]="selection_statement";
static unsigned char _temp1349[ 15u]="switch_clauses"; static unsigned char
_temp1350[ 16u]="switchC_clauses"; static unsigned char _temp1351[ 20u]="iteration_statement";
static unsigned char _temp1352[ 15u]="jump_statement"; static unsigned char
_temp1353[ 8u]="pattern"; static unsigned char _temp1354[ 19u]="tuple_pattern_list";
static unsigned char _temp1355[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1356[ 14u]="field_pattern"; static unsigned char _temp1357[ 19u]="field_pattern_list";
static unsigned char _temp1358[ 20u]="field_pattern_list0"; static unsigned char
_temp1359[ 11u]="expression"; static unsigned char _temp1360[ 22u]="assignment_expression";
static unsigned char _temp1361[ 20u]="assignment_operator"; static unsigned char
_temp1362[ 23u]="conditional_expression"; static unsigned char _temp1363[ 20u]="constant_expression";
static unsigned char _temp1364[ 22u]="logical_or_expression"; static
unsigned char _temp1365[ 23u]="logical_and_expression"; static unsigned char
_temp1366[ 24u]="inclusive_or_expression"; static unsigned char _temp1367[ 24u]="exclusive_or_expression";
static unsigned char _temp1368[ 15u]="and_expression"; static unsigned char
_temp1369[ 20u]="equality_expression"; static unsigned char _temp1370[ 22u]="relational_expression";
static unsigned char _temp1371[ 17u]="shift_expression"; static unsigned char
_temp1372[ 20u]="additive_expression"; static unsigned char _temp1373[ 26u]="multiplicative_expression";
static unsigned char _temp1374[ 16u]="cast_expression"; static unsigned char
_temp1375[ 17u]="unary_expression"; static unsigned char _temp1376[ 15u]="unary_operator";
static unsigned char _temp1377[ 19u]="postfix_expression"; static unsigned char
_temp1378[ 19u]="primary_expression"; static unsigned char _temp1379[ 25u]="argument_expression_list";
static unsigned char _temp1380[ 26u]="argument_expression_list0"; static
unsigned char _temp1381[ 9u]="constant"; static unsigned char _temp1382[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 233u]={{ _temp1150, _temp1150, _temp1150
+  2u},{ _temp1151, _temp1151, _temp1151 +  6u},{ _temp1152, _temp1152,
_temp1152 +  12u},{ _temp1153, _temp1153, _temp1153 +  5u},{ _temp1154,
_temp1154, _temp1154 +  9u},{ _temp1155, _temp1155, _temp1155 +  7u},{ _temp1156,
_temp1156, _temp1156 +  7u},{ _temp1157, _temp1157, _temp1157 +  8u},{ _temp1158,
_temp1158, _temp1158 +  5u},{ _temp1159, _temp1159, _temp1159 +  5u},{ _temp1160,
_temp1160, _temp1160 +  6u},{ _temp1161, _temp1161, _temp1161 +  4u},{ _temp1162,
_temp1162, _temp1162 +  5u},{ _temp1163, _temp1163, _temp1163 +  6u},{ _temp1164,
_temp1164, _temp1164 +  7u},{ _temp1165, _temp1165, _temp1165 +  7u},{ _temp1166,
_temp1166, _temp1166 +  9u},{ _temp1167, _temp1167, _temp1167 +  6u},{ _temp1168,
_temp1168, _temp1168 +  9u},{ _temp1169, _temp1169, _temp1169 +  9u},{ _temp1170,
_temp1170, _temp1170 +  7u},{ _temp1171, _temp1171, _temp1171 +  6u},{ _temp1172,
_temp1172, _temp1172 +  5u},{ _temp1173, _temp1173, _temp1173 +  8u},{ _temp1174,
_temp1174, _temp1174 +  7u},{ _temp1175, _temp1175, _temp1175 +  7u},{ _temp1176,
_temp1176, _temp1176 +  9u},{ _temp1177, _temp1177, _temp1177 +  3u},{ _temp1178,
_temp1178, _temp1178 +  5u},{ _temp1179, _temp1179, _temp1179 +  7u},{ _temp1180,
_temp1180, _temp1180 +  6u},{ _temp1181, _temp1181, _temp1181 +  3u},{ _temp1182,
_temp1182, _temp1182 +  4u},{ _temp1183, _temp1183, _temp1183 +  5u},{ _temp1184,
_temp1184, _temp1184 +  9u},{ _temp1185, _temp1185, _temp1185 +  6u},{ _temp1186,
_temp1186, _temp1186 +  7u},{ _temp1187, _temp1187, _temp1187 +  5u},{ _temp1188,
_temp1188, _temp1188 +  8u},{ _temp1189, _temp1189, _temp1189 +  4u},{ _temp1190,
_temp1190, _temp1190 +  6u},{ _temp1191, _temp1191, _temp1191 +  4u},{ _temp1192,
_temp1192, _temp1192 +  6u},{ _temp1193, _temp1193, _temp1193 +  4u},{ _temp1194,
_temp1194, _temp1194 +  9u},{ _temp1195, _temp1195, _temp1195 +  9u},{ _temp1196,
_temp1196, _temp1196 +  6u},{ _temp1197, _temp1197, _temp1197 +  10u},{
_temp1198, _temp1198, _temp1198 +  7u},{ _temp1199, _temp1199, _temp1199 +  8u},{
_temp1200, _temp1200, _temp1200 +  5u},{ _temp1201, _temp1201, _temp1201 +  8u},{
_temp1202, _temp1202, _temp1202 +  4u},{ _temp1203, _temp1203, _temp1203 +  7u},{
_temp1204, _temp1204, _temp1204 +  7u},{ _temp1205, _temp1205, _temp1205 +  9u},{
_temp1206, _temp1206, _temp1206 +  7u},{ _temp1207, _temp1207, _temp1207 +  5u},{
_temp1208, _temp1208, _temp1208 +  8u},{ _temp1209, _temp1209, _temp1209 +  8u},{
_temp1210, _temp1210, _temp1210 +  4u},{ _temp1211, _temp1211, _temp1211 +  7u},{
_temp1212, _temp1212, _temp1212 +  7u},{ _temp1213, _temp1213, _temp1213 +  7u},{
_temp1214, _temp1214, _temp1214 +  8u},{ _temp1215, _temp1215, _temp1215 +  9u},{
_temp1216, _temp1216, _temp1216 +  6u},{ _temp1217, _temp1217, _temp1217 +  6u},{
_temp1218, _temp1218, _temp1218 +  6u},{ _temp1219, _temp1219, _temp1219 +  6u},{
_temp1220, _temp1220, _temp1220 +  7u},{ _temp1221, _temp1221, _temp1221 +  6u},{
_temp1222, _temp1222, _temp1222 +  11u},{ _temp1223, _temp1223, _temp1223 +  11u},{
_temp1224, _temp1224, _temp1224 +  11u},{ _temp1225, _temp1225, _temp1225 +  11u},{
_temp1226, _temp1226, _temp1226 +  11u},{ _temp1227, _temp1227, _temp1227 +  12u},{
_temp1228, _temp1228, _temp1228 +  13u},{ _temp1229, _temp1229, _temp1229 +  11u},{
_temp1230, _temp1230, _temp1230 +  11u},{ _temp1231, _temp1231, _temp1231 +  10u},{
_temp1232, _temp1232, _temp1232 +  9u},{ _temp1233, _temp1233, _temp1233 +  11u},{
_temp1234, _temp1234, _temp1234 +  12u},{ _temp1235, _temp1235, _temp1235 +  11u},{
_temp1236, _temp1236, _temp1236 +  17u},{ _temp1237, _temp1237, _temp1237 +  7u},{
_temp1238, _temp1238, _temp1238 +  19u},{ _temp1239, _temp1239, _temp1239 +  18u},{
_temp1240, _temp1240, _temp1240 +  9u},{ _temp1241, _temp1241, _temp1241 +  16u},{
_temp1242, _temp1242, _temp1242 +  18u},{ _temp1243, _temp1243, _temp1243 +  10u},{
_temp1244, _temp1244, _temp1244 +  4u},{ _temp1245, _temp1245, _temp1245 +  4u},{
_temp1246, _temp1246, _temp1246 +  4u},{ _temp1247, _temp1247, _temp1247 +  4u},{
_temp1248, _temp1248, _temp1248 +  4u},{ _temp1249, _temp1249, _temp1249 +  4u},{
_temp1250, _temp1250, _temp1250 +  4u},{ _temp1251, _temp1251, _temp1251 +  4u},{
_temp1252, _temp1252, _temp1252 +  4u},{ _temp1253, _temp1253, _temp1253 +  4u},{
_temp1254, _temp1254, _temp1254 +  4u},{ _temp1255, _temp1255, _temp1255 +  4u},{
_temp1256, _temp1256, _temp1256 +  4u},{ _temp1257, _temp1257, _temp1257 +  4u},{
_temp1258, _temp1258, _temp1258 +  4u},{ _temp1259, _temp1259, _temp1259 +  4u},{
_temp1260, _temp1260, _temp1260 +  4u},{ _temp1261, _temp1261, _temp1261 +  4u},{
_temp1262, _temp1262, _temp1262 +  4u},{ _temp1263, _temp1263, _temp1263 +  4u},{
_temp1264, _temp1264, _temp1264 +  4u},{ _temp1265, _temp1265, _temp1265 +  4u},{
_temp1266, _temp1266, _temp1266 +  4u},{ _temp1267, _temp1267, _temp1267 +  4u},{
_temp1268, _temp1268, _temp1268 +  4u},{ _temp1269, _temp1269, _temp1269 +  4u},{
_temp1270, _temp1270, _temp1270 +  4u},{ _temp1271, _temp1271, _temp1271 +  5u},{
_temp1272, _temp1272, _temp1272 +  17u},{ _temp1273, _temp1273, _temp1273 +  21u},{
_temp1274, _temp1274, _temp1274 +  20u},{ _temp1275, _temp1275, _temp1275 +  21u},{
_temp1276, _temp1276, _temp1276 +  13u},{ _temp1277, _temp1277, _temp1277 +  15u},{
_temp1278, _temp1278, _temp1278 +  17u},{ _temp1279, _temp1279, _temp1279 +  19u},{
_temp1280, _temp1280, _temp1280 +  12u},{ _temp1281, _temp1281, _temp1281 +  17u},{
_temp1282, _temp1282, _temp1282 +  23u},{ _temp1283, _temp1283, _temp1283 +  24u},{
_temp1284, _temp1284, _temp1284 +  15u},{ _temp1285, _temp1285, _temp1285 +  11u},{
_temp1286, _temp1286, _temp1286 +  15u},{ _temp1287, _temp1287, _temp1287 +  10u},{
_temp1288, _temp1288, _temp1288 +  15u},{ _temp1289, _temp1289, _temp1289 +  5u},{
_temp1290, _temp1290, _temp1290 +  15u},{ _temp1291, _temp1291, _temp1291 +  15u},{
_temp1292, _temp1292, _temp1292 +  11u},{ _temp1293, _temp1293, _temp1293 +  22u},{
_temp1294, _temp1294, _temp1294 +  26u},{ _temp1295, _temp1295, _temp1295 +  16u},{
_temp1296, _temp1296, _temp1296 +  16u},{ _temp1297, _temp1297, _temp1297 +  24u},{
_temp1298, _temp1298, _temp1298 +  25u},{ _temp1299, _temp1299, _temp1299 +  21u},{
_temp1300, _temp1300, _temp1300 +  22u},{ _temp1301, _temp1301, _temp1301 +  16u},{
_temp1302, _temp1302, _temp1302 +  19u},{ _temp1303, _temp1303, _temp1303 +  25u},{
_temp1304, _temp1304, _temp1304 +  23u},{ _temp1305, _temp1305, _temp1305 +  24u},{
_temp1306, _temp1306, _temp1306 +  18u},{ _temp1307, _temp1307, _temp1307 +  17u},{
_temp1308, _temp1308, _temp1308 +  18u},{ _temp1309, _temp1309, _temp1309 +  17u},{
_temp1310, _temp1310, _temp1310 +  18u},{ _temp1311, _temp1311, _temp1311 +  12u},{
_temp1312, _temp1312, _temp1312 +  11u},{ _temp1313, _temp1313, _temp1313 +  18u},{
_temp1314, _temp1314, _temp1314 +  8u},{ _temp1315, _temp1315, _temp1315 +  13u},{
_temp1316, _temp1316, _temp1316 +  8u},{ _temp1317, _temp1317, _temp1317 +  4u},{
_temp1318, _temp1318, _temp1318 +  20u},{ _temp1319, _temp1319, _temp1319 +  20u},{
_temp1320, _temp1320, _temp1320 +  9u},{ _temp1321, _temp1321, _temp1321 +  16u},{
_temp1322, _temp1322, _temp1322 +  19u},{ _temp1323, _temp1323, _temp1323 +  10u},{
_temp1324, _temp1324, _temp1324 +  16u},{ _temp1325, _temp1325, _temp1325 +  11u},{
_temp1326, _temp1326, _temp1326 +  14u},{ _temp1327, _temp1327, _temp1327 +  11u},{
_temp1328, _temp1328, _temp1328 +  15u},{ _temp1329, _temp1329, _temp1329 +  22u},{
_temp1330, _temp1330, _temp1330 +  16u},{ _temp1331, _temp1331, _temp1331 +  17u},{
_temp1332, _temp1332, _temp1332 +  12u},{ _temp1333, _temp1333, _temp1333 +  18u},{
_temp1334, _temp1334, _temp1334 +  17u},{ _temp1335, _temp1335, _temp1335 +  12u},{
_temp1336, _temp1336, _temp1336 +  16u},{ _temp1337, _temp1337, _temp1337 +  11u},{
_temp1338, _temp1338, _temp1338 +  10u},{ _temp1339, _temp1339, _temp1339 +  14u},{
_temp1340, _temp1340, _temp1340 +  15u},{ _temp1341, _temp1341, _temp1341 +  20u},{
_temp1342, _temp1342, _temp1342 +  27u},{ _temp1343, _temp1343, _temp1343 +  10u},{
_temp1344, _temp1344, _temp1344 +  18u},{ _temp1345, _temp1345, _temp1345 +  21u},{
_temp1346, _temp1346, _temp1346 +  19u},{ _temp1347, _temp1347, _temp1347 +  16u},{
_temp1348, _temp1348, _temp1348 +  20u},{ _temp1349, _temp1349, _temp1349 +  15u},{
_temp1350, _temp1350, _temp1350 +  16u},{ _temp1351, _temp1351, _temp1351 +  20u},{
_temp1352, _temp1352, _temp1352 +  15u},{ _temp1353, _temp1353, _temp1353 +  8u},{
_temp1354, _temp1354, _temp1354 +  19u},{ _temp1355, _temp1355, _temp1355 +  20u},{
_temp1356, _temp1356, _temp1356 +  14u},{ _temp1357, _temp1357, _temp1357 +  19u},{
_temp1358, _temp1358, _temp1358 +  20u},{ _temp1359, _temp1359, _temp1359 +  11u},{
_temp1360, _temp1360, _temp1360 +  22u},{ _temp1361, _temp1361, _temp1361 +  20u},{
_temp1362, _temp1362, _temp1362 +  23u},{ _temp1363, _temp1363, _temp1363 +  20u},{
_temp1364, _temp1364, _temp1364 +  22u},{ _temp1365, _temp1365, _temp1365 +  23u},{
_temp1366, _temp1366, _temp1366 +  24u},{ _temp1367, _temp1367, _temp1367 +  24u},{
_temp1368, _temp1368, _temp1368 +  15u},{ _temp1369, _temp1369, _temp1369 +  20u},{
_temp1370, _temp1370, _temp1370 +  22u},{ _temp1371, _temp1371, _temp1371 +  17u},{
_temp1372, _temp1372, _temp1372 +  20u},{ _temp1373, _temp1373, _temp1373 +  26u},{
_temp1374, _temp1374, _temp1374 +  16u},{ _temp1375, _temp1375, _temp1375 +  17u},{
_temp1376, _temp1376, _temp1376 +  15u},{ _temp1377, _temp1377, _temp1377 +  19u},{
_temp1378, _temp1378, _temp1378 +  19u},{ _temp1379, _temp1379, _temp1379 +  25u},{
_temp1380, _temp1380, _temp1380 +  26u},{ _temp1381, _temp1381, _temp1381 +  9u},{
_temp1382, _temp1382, _temp1382 +  20u}}; static short Cyc_yyr1[ 405u]={ 0, 121,
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
224, 224, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225,
226, 226, 226, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227,
227, 228, 228, 228, 228, 228, 228, 228, 228, 228, 229, 230, 230, 231, 231, 231,
231, 232, 232}; static short Cyc_yyr2[ 405u]={ 0, 1, 2, 3, 5, 3, 5, 6, 0, 1, 1,
2, 3, 3, 4, 3, 4, 2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1,
1, 2, 1, 1, 0, 1, 6, 1, 3, 1, 1, 4, 4, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 2, 4, 4, 1, 1, 1, 1, 1, 5, 2, 1, 3, 1, 3, 4, 6, 6, 3, 3, 0, 3, 3, 1, 1, 1, 1,
2, 1, 1, 3, 1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3, 6, 4, 3, 5, 1, 1, 1, 2, 3, 3,
0, 1, 1, 2, 6, 1, 2, 1, 3, 3, 4, 4, 5, 4, 4, 4, 2, 2, 1, 3, 4, 4, 5, 1, 1, 4, 4,
1, 0, 1, 1, 1, 1, 2, 3, 5, 5, 7, 1, 3, 0, 2, 0, 2, 3, 5, 0, 1, 1, 3, 2, 3, 4, 1,
1, 3, 1, 3, 2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2,
1, 2, 3, 4, 3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1,
1, 6, 3, 2, 2, 3, 1, 2, 2, 3, 1, 2, 1, 2, 1, 2, 5, 7, 7, 8, 6, 0, 3, 4, 5, 6, 7,
0, 3, 4, 5, 5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2, 3, 2, 4, 5, 1,
3, 1, 2, 1, 1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1,
3, 1, 3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2,
2, 4, 2, 6, 6, 5, 7, 9, 1, 1, 1, 1, 4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1,
1, 3, 2, 5, 4, 4, 5, 1, 1, 3, 1, 1, 1, 1, 1, 1}; static short Cyc_yydefact[ 801u]={
0, 137, 35, 36, 37, 38, 40, 52, 54, 55, 56, 57, 58, 59, 60, 61, 71, 72, 73, 88,
89, 42, 0, 0, 41, 0, 0, 113, 114, 0, 403, 157, 404, 85, 0, 53, 0, 142, 143, 146,
1, 0, 9, 0, 0, 10, 0, 42, 42, 42, 62, 63, 0, 64, 0, 0, 124, 0, 147, 65, 126, 39,
0, 33, 43, 75, 286, 403, 282, 285, 284, 0, 280, 0, 0, 0, 0, 0, 180, 0, 287, 17,
19, 0, 0, 0, 66, 0, 0, 0, 0, 2, 0, 0, 0, 0, 21, 0, 93, 94, 96, 27, 29, 31, 85, 0,
85, 150, 0, 149, 85, 38, 0, 25, 0, 0, 11, 159, 0, 0, 135, 125, 42, 148, 136, 0,
0, 34, 0, 0, 294, 293, 283, 292, 24, 0, 0, 0, 0, 0, 42, 42, 198, 200, 0, 69, 70,
158, 205, 0, 127, 0, 0, 175, 0, 0, 402, 0, 0, 0, 0, 0, 0, 0, 85, 0, 0, 399, 389,
400, 401, 0, 0, 0, 0, 372, 0, 370, 371, 0, 305, 318, 326, 328, 330, 332, 334,
336, 339, 344, 347, 350, 354, 0, 356, 373, 388, 387, 0, 3, 0, 5, 0, 22, 0, 0, 0,
12, 28, 30, 32, 84, 0, 90, 91, 0, 83, 85, 0, 111, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 403, 233, 235, 0, 241, 237, 0, 239, 222, 223, 224, 0, 225, 226,
227, 0, 303, 26, 13, 96, 165, 181, 0, 0, 161, 159, 0, 0, 128, 0, 138, 0, 0, 78,
0, 76, 281, 296, 0, 295, 182, 0, 0, 294, 0, 201, 173, 0, 99, 101, 159, 0, 207,
199, 208, 68, 0, 87, 0, 86, 0, 177, 0, 179, 67, 0, 0, 364, 0, 320, 354, 0, 321,
322, 0, 0, 0, 0, 0, 0, 0, 357, 358, 0, 0, 0, 0, 360, 361, 359, 144, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 308, 309, 310, 311, 312, 313, 314,
315, 316, 317, 307, 0, 362, 0, 381, 382, 0, 0, 0, 391, 0, 0, 145, 18, 0, 20, 0,
95, 97, 184, 183, 14, 0, 80, 92, 0, 0, 103, 104, 106, 0, 110, 85, 119, 0, 0, 0,
0, 0, 0, 0, 273, 274, 275, 0, 0, 277, 0, 230, 231, 0, 0, 0, 0, 242, 238, 96, 240,
236, 234, 0, 166, 0, 0, 0, 172, 160, 167, 130, 0, 0, 0, 161, 132, 134, 133, 129,
151, 140, 139, 0, 48, 47, 0, 45, 0, 74, 0, 289, 0, 23, 290, 0, 0, 0, 0, 194, 298,
301, 0, 300, 0, 0, 0, 202, 100, 102, 0, 161, 0, 211, 0, 209, 159, 0, 0, 221, 204,
206, 176, 0, 0, 0, 185, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 390, 397, 0,
396, 327, 0, 329, 331, 333, 335, 337, 338, 342, 343, 340, 341, 345, 346, 348,
349, 351, 352, 353, 306, 379, 380, 375, 0, 377, 378, 0, 0, 0, 4, 6, 0, 325, 107,
98, 0, 0, 0, 112, 121, 120, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 272, 276, 0, 0, 0,
229, 0, 232, 0, 15, 304, 159, 0, 169, 0, 0, 0, 162, 131, 165, 153, 152, 141, 7,
0, 0, 0, 79, 77, 297, 197, 0, 299, 193, 195, 291, 0, 288, 203, 174, 216, 0, 210,
213, 0, 161, 0, 212, 0, 363, 0, 0, 186, 0, 190, 385, 386, 0, 0, 0, 0, 0, 0, 0, 0,
355, 393, 0, 0, 376, 374, 394, 0, 0, 82, 105, 108, 81, 109, 122, 119, 119, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 248, 278, 0, 0, 16, 161, 0, 170, 168, 0, 161, 0, 0, 0, 0,
44, 46, 196, 302, 215, 218, 0, 220, 219, 214, 0, 0, 0, 187, 191, 0, 0, 323, 324,
0, 367, 395, 0, 398, 319, 392, 0, 118, 117, 243, 0, 248, 258, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 279, 0, 155, 171, 163, 154, 159, 0, 49, 50, 217, 365, 366, 0,
192, 0, 0, 383, 0, 0, 0, 254, 0, 0, 260, 0, 0, 0, 268, 0, 0, 0, 0, 0, 0, 0, 0,
247, 228, 0, 161, 0, 0, 368, 0, 384, 0, 244, 0, 0, 0, 245, 259, 261, 262, 0, 270,
269, 0, 264, 0, 0, 0, 0, 248, 249, 164, 156, 0, 0, 0, 123, 0, 0, 246, 263, 271,
265, 266, 0, 0, 248, 250, 0, 188, 369, 254, 255, 267, 248, 251, 51, 254, 256,
248, 252, 257, 253, 0, 0, 0}; static short Cyc_yydefgoto[ 112u]={ 798, 40, 41,
42, 239, 43, 369, 44, 371, 45, 206, 46, 47, 63, 64, 438, 439, 48, 147, 49, 50,
268, 269, 51, 86, 52, 212, 213, 97, 98, 99, 214, 142, 381, 382, 383, 53, 54, 537,
538, 539, 55, 56, 57, 58, 122, 108, 434, 462, 59, 463, 425, 565, 417, 421, 422,
282, 260, 153, 77, 78, 479, 374, 480, 481, 450, 451, 143, 148, 149, 464, 289,
242, 243, 244, 245, 246, 247, 699, 750, 248, 249, 272, 273, 274, 453, 454, 455,
250, 251, 356, 180, 529, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
192, 193, 194, 195, 494, 495, 196, 197}; static short Cyc_yypact[ 801u]={ 1846,
- -32768, - -32768, - -32768, - -32768, - 56, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, 6, 9, 2209, - -32768, 9, - 71, - -32768, -
-32768, 54, - -32768, 86, - -32768, 112, 487, - -32768, 119, 151, 166, - -32768,
- -32768, 881, - -32768, 88, 102, - -32768, 206, 6, 6, 6, - -32768, - -32768,
291, - -32768, 192, 2391, 178, 28, 19, - -32768, 9, 191, 150, 2447, - -32768,
204, - -32768, 59, - -32768, - -32768, - -32768, 2276, - -32768, 212, 260, 152,
2276, 273, 255, 281, 95, - -32768, - -32768, 4227, 142, 4227, - -32768, 293,
1443, 3851, 3851, - -32768, 881, 1957, 881, 1957, - -32768, 321, 322, - -32768,
2332, 2447, 2447, 2447, 112, 1443, 112, - -32768, 9, - -32768, 220, 364, 1263, -
-32768, 2391, 206, - -32768, 1787, 4227, 2779, - -32768, 178, 6, - -32768, -
-32768, 1957, 362, - -32768, 9, 372, 2276, - -32768, - -32768, - -32768, -
-32768, 384, 3851, 310, 381, 205, 6, 6, 409, - -32768, 67, - -32768, - -32768, -
-32768, 374, 104, - -32768, 592, 179, - -32768, 4004, 398, - -32768, 3851, 3894,
408, 425, 430, 438, 449, 112, 4071, 4071, - -32768, - -32768, - -32768, - -32768,
1612, 451, 4114, 4114, - -32768, 4114, - -32768, - -32768, 404, - -32768, - 43,
491, 437, 441, 454, 442, 7, 448, 402, - 79, - -32768, 997, 4114, 168, - -32768,
- -32768, 12, 469, - -32768, 474, - -32768, 477, - -32768, 487, 2846, 2391, -
-32768, - -32768, - -32768, - -32768, 484, 481, 1443, - -32768, 289, 489, 112, 9,
493, - -32768, 497, 69, 501, 2478, 514, 542, 544, 547, 2913, 2478, 208, 2478,
2478, 13, 517, - -32768, - -32768, 545, 1381, 1381, 206, 1381, - -32768, -
-32768, - -32768, 549, - -32768, - -32768, - -32768, 149, - -32768, - -32768, -
-32768, 557, 577, - -32768, 91, 566, 562, 225, 569, 140, - -32768, 561, 110, 575,
8, 572, 578, 576, - -32768, - -32768, 580, 582, - -32768, 253, 1030, 2276, 4227,
- -32768, 584, 579, 1443, 1443, 2223, 2980, - 6, - -32768, 261, - -32768, 91, -
-32768, 4227, - -32768, 2119, - -32768, 559, - -32768, - -32768, 1443, 1612, -
-32768, 1443, - -32768, - -32768, 734, - -32768, 616, 3851, 2068, 663, 3851,
3851, 591, 1612, - -32768, - -32768, 1381, 594, 431, 3851, - -32768, - -32768, -
-32768, - -32768, 4114, 3851, 4114, 4114, 4114, 4114, 4114, 4114, 4114, 4114,
4114, 4114, 4114, 4114, 4114, 4114, 4114, 4114, 4114, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, 3851, - -32768, 180, - -32768, - -32768, 3047, 188, 3851, - -32768, 2701,
593, - -32768, - -32768, 881, - -32768, 881, - -32768, - -32768, - -32768, -
-32768, - -32768, 1443, - -32768, - -32768, 3851, 600, 595, - -32768, 599, 1443,
- -32768, 112, 528, 3851, 602, 3851, 3851, 668, 1499, 611, - -32768, - -32768, -
-32768, 272, 664, - -32768, 3114, - -32768, - -32768, 2478, 620, 2478, 1725, -
-32768, - -32768, 2332, - -32768, - -32768, - -32768, 3851, - -32768, 1443, 614,
295, - -32768, - -32768, 603, - -32768, 621, 615, 2008, 562, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, 110, 881, - -32768, 619, 624, 625, 9, -
-32768, 3851, - -32768, 2276, - -32768, - -32768, 634, 3851, 2276, 221, - -32768,
- -32768, - -32768, 630, 627, 629, 37, 639, - -32768, - -32768, - -32768, 631,
562, 633, - -32768, 628, 261, 1897, 4227, 3181, - -32768, - -32768, 374, -
-32768, 638, 640, 648, - -32768, - -32768, 41, 2846, 440, 642, 487, 641, 443,
643, 1443, 647, 646, 3961, - -32768, - -32768, 650, 651, 491, 339, 437, 441, 454,
442, 7, 7, 448, 448, 448, 448, 402, 402, - 79, - 79, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, 653, - -32768, - -32768, 116,
340, 4227, - -32768, - -32768, 654, - -32768, - -32768, - -32768, 289, 3851, 657,
- -32768, - -32768, - -32768, 658, 9, 296, 456, 3851, 476, 482, 649, 3248, 3315,
316, - -32768, - -32768, 661, 667, 665, - -32768, 671, - -32768, 2391, - -32768,
- -32768, 674, 4227, - -32768, 666, 91, 662, - -32768, - -32768, 290, - -32768,
- -32768, - -32768, - -32768, 406, 681, 8, - -32768, - -32768, - -32768, -
-32768, 673, - -32768, - -32768, - -32768, - -32768, 2160, - -32768, - -32768, -
-32768, - -32768, 683, - -32768, - -32768, 684, 562, 153, - -32768, 682, 698,
505, 692, - -32768, 2545, - -32768, - -32768, - -32768, 2391, 1443, 3894, 732,
699, 698, 700, 2701, - -32768, - -32768, 3851, 3851, - -32768, - -32768, -
-32768, 2701, 368, - -32768, - -32768, - -32768, - -32768, - -32768, 345, 48,
528, 2478, 502, 705, 2478, 3851, 3382, 326, 3449, 330, 3516, 581, - -32768, 707,
717, - -32768, 562, 53, - -32768, - -32768, 713, 562, 1443, 704, 706, 708, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, 709, - -32768, -
-32768, - -32768, 712, 716, 3851, - -32768, - -32768, 2846, 718, - -32768, 616,
720, - -32768, - -32768, 382, - -32768, - -32768, - -32768, 726, - -32768, -
-32768, 788, 731, 581, - -32768, 508, 2478, 510, 3583, 2478, 515, 3650, 3717,
338, 2276, 722, 735, - -32768, 2478, - -32768, - -32768, 728, - -32768, 674, 747,
- -32768, - -32768, - -32768, - -32768, - -32768, 347, - -32768, 736, 1443, -
-32768, 2623, 1443, 2478, 596, 742, 745, - -32768, 2478, 2478, 526, - -32768,
2478, 2478, 529, 2478, 531, 3784, - 18, 1381, - -32768, - -32768, 621, 562, 744,
3851, - -32768, 746, - -32768, 534, - -32768, 3851, 750, 753, - -32768, - -32768,
- -32768, - -32768, 2478, - -32768, - -32768, 2478, - -32768, 2478, 2478, 537,
3851, 994, - -32768, - -32768, - -32768, 764, 389, 752, - -32768, 357, 1381, -
-32768, - -32768, - -32768, - -32768, - -32768, 2478, 370, 581, - -32768, 757, -
-32768, - -32768, 1145, - -32768, - -32768, 994, - -32768, - -32768, 596, -
-32768, 581, - -32768, - -32768, - -32768, 852, 857, - -32768}; static short Cyc_yypgoto[
112u]={ - -32768, 38, - -32768, 548, - -32768, - -32768, - -32768, - -32768, -
-32768, - 51, - 48, - 55, - -32768, 128, - 50, 285, - -32768, 118, - -32768, 29,
- -32768, - -32768, 420, - -32768, - 22, - -32768, - 140, - -32768, - -32768, -
-32768, 659, 652, 57, - -32768, - -32768, 331, - -32768, - -32768, 11, - -32768,
- -32768, 43, - 33, - 73, - -32768, - -32768, 806, - -32768, - 97, - 31, - 106,
- 391, 129, 300, 308, 583, - 382, - 87, - 278, 755, - -32768, - 189, - 149, -
360, - 251, - -32768, 423, - 137, - 80, - 99, - 45, - 212, - 109, - -32768, -
-32768, - 34, - 227, - -32768, - 621, - 351, - -32768, - -32768, - 21, 597, -
-32768, 292, - -32768, - -32768, 175, 143, - -32768, - 139, - 352, - 141, 551,
550, 554, 573, 585, 315, 0, 317, 320, - 133, 613, - -32768, - -32768, - -32768,
- 319, - -32768, - -32768, 10}; static short Cyc_yytable[ 4330u]={ 115, 152, 79,
144, 113, 523, 120, 114, 127, 307, 60, 259, 409, 410, 82, 412, 373, 308, 304,
262, 258, 116, 474, 109, 121, 436, 449, 109, 326, 1, 342, 61, 65, 80, 319, 81,
568, 562, 343, 344, 322, 323, 519, 324, 60, 115, 208, 209, 210, 113, 129, 60,
763, 535, 536, 133, 60, 241, 137, 115, 357, 240, 106, 252, 110, 722, 207, 60,
327, 287, 124, 120, 589, 334, 335, 467, 587, 87, 297, 91, 253, 80, 211, 552, 216,
467, 80, 764, 219, 100, 576, 490, 285, 437, 30, 364, 579, 288, 405, 62, 32, 286,
60, 60, 60, 60, 298, 365, 281, 31, 336, 337, 141, 30, 141, 393, 406, 141, 217,
32, 107, 400, 366, 403, 404, 60, 34, 16, 17, 18, 199, 200, 201, 202, 141, 60,
586, 600, 270, 559, 80, 601, 314, 782, - 116, 151, 141, 141, 474, 291, 418, 115,
703, - 181, 427, 252, 390, 83, 254, - 181, 790, 60, 215, 266, 475, 291, 476, 391,
795, 292, 84, 290, 376, 797, 151, 101, 102, 103, 489, 291, 624, 31, 92, 93, 241,
241, 419, 241, 240, 240, - 85, 240, 433, - 85, 296, 386, 94, 95, 85, 457, 141,
140, 661, 140, 293, 429, 140, 1, 294, 512, 513, 514, 287, 473, 60, 85, 415, 88,
662, 37, 38, 39, 297, 140, 618, 60, 420, 145, 387, 358, 359, 360, 179, 198, 146,
140, 140, 527, 132, 471, 293, 528, 141, 414, 430, 533, 89, 254, 126, 415, 265,
60, 677, 293, 702, 484, 452, 663, 384, 705, 420, 90, 264, 241, 121, 516, 361,
240, 283, 284, 215, 62, 517, 520, 362, 363, 117, 30, 299, 300, 521, 118, 31, 32,
411, 119, 125, 80, 80, 140, 1, 30, 602, 107, 432, 31, 553, 32, 555, 128, 96, 280,
401, 528, 34, 60, 402, 60, 141, 528, 130, 276, 141, 141, 141, 37, 38, 39, 581,
257, 60, 152, 141, 85, 141, 426, 218, 447, 448, 141, 141, 140, 141, 449, 504,
505, 506, 507, 87, 115, 460, 461, 151, 546, 141, 131, 320, 445, 375, 767, 670,
609, 151, 415, 62, 135, 115, 151, 613, 468, 113, 570, 593, 556, 469, 534, 549,
134, 470, 670, 594, 592, 415, 706, 30, 416, 30, 557, 136, 60, 32, 60, 32, 104,
257, 31, 105, 34, 281, - 159, 628, 561, 150, 528, 380, - 159, 629, 140, 37, 38,
39, 140, 140, 140, 399, 277, 141, 525, 278, 526, 639, 140, 669, 140, 141, 203,
415, 471, 140, 140, 691, 140, 204, 577, 694, 621, 415, 281, 580, 466, 415, 669,
734, 140, 215, 793, 619, 141, 415, 415, 620, 796, 215, - 85, 616, 60, 141, 415,
85, 375, 270, 220, 742, 645, 80, 141, 487, 415, 672, 80, 267, 650, 786, 569, 493,
151, 673, 670, 293, 275, 415, 271, 680, 571, 151, 789, 320, 679, 717, 279, 646,
714, 718, 151, 482, 784, 291, 486, 1, 415, 320, 652, 653, 654, 60, 140, 303, 141,
141, 515, 325, 115, 497, 140, 493, 252, 309, 285, 375, 765, 332, 333, 338, 339,
340, 341, 286, 141, 37, 38, 39, 684, 644, 310, 687, 151, 140, 605, 311, 669, 492,
415, 420, 535, 536, 140, 312, 781, 522, 603, 415, 60, 607, 415, 140, 493, 787,
313, 627, 321, 115, 493, 329, 141, 113, 630, 415, 330, 558, 792, 1, 328, 794,
452, 540, 367, 542, 543, 331, 547, 368, 207, 30, 370, 384, 632, 415, 378, 32,
377, 724, 633, 415, 728, 385, 34, 140, 140, 388, 141, 665, 666, 738, 1, 80, 389,
37, 38, 39, 392, 740, 685, 415, 697, 698, 681, 140, 723, 415, 725, 415, 747, 394,
596, 729, 415, 753, 754, 748, 749, 756, 757, 407, 759, 375, 755, 415, 395, 758,
415, 760, 415, 746, 771, 300, 141, 779, 415, 396, 682, 683, 397, 140, 408, 30,
413, 775, 502, 503, 776, 32, 777, 778, - 207, 205, 508, 509, 295, - 207, - 207,
510, 511, 416, 671, - 207, 423, 286, 424, 428, 431, 788, 435, 440, 442, 441, 459,
735, 30, 140, 443, 141, 241, 444, 32, 458, 240, - 178, 326, 485, 488, 295, - 178,
- 178, 491, 530, 531, 524, - 178, 544, 286, 541, 37, 38, 39, 532, 548, 550, 80,
151, 241, 554, 564, 560, 240, 566, 563, 631, 572, 241, 578, 636, 638, 240, 573,
140, 574, 583, 584, 585, 31, 588, 241, 590, 599, 241, 240, 591, 597, 240, 606,
598, 604, 611, 608, 375, 141, 610, 634, 141, 614, 622, 615, 617, 625, 626, 375,
640, 674, 678, 154, 155, 641, 647, 375, 642, 649, 477, 302, 257, 140, 305, 305,
156, 744, 157, 643, 151, 158, 316, 317, 655, 657, 659, 660, 159, 160, 305, 305,
161, 305, 664, 162, 163, 612, 164, 667, 165, 166, 675, 676, 686, 700, 701, 704,
707, 708, 305, 709, 710, 688, 690, 711, 693, 375, 696, 712, 720, 715, 716, 30,
167, 168, 169, 170, 719, 32, 721, 736, 739, 306, 478, 737, 171, 741, 140, 743,
172, 140, 751, 752, 447, 448, 713, 173, 768, 770, 174, 175, 176, 774, 783, 785,
799, 177, 178, 773, 791, 800, 483, 656, 575, 375, 623, 372, 123, 379, 727, 651,
766, 731, 733, 648, 261, 582, 472, 456, 658, 496, 498, 0, 0, - 8, 1, 499, 2, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 500, 0, 21, 0, 0,
0, 762, 0, 0, 0, 0, 0, 0, 501, 769, 22, 0, 23, 0, 0, 772, 0, 24, 0, 25, 26, 27,
28, 0, 0, 0, 0, 0, 29, 0, 780, 305, 0, 305, 305, 305, 305, 305, 305, 305, 305,
305, 305, 305, 305, 305, 305, 305, 305, 305, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0,
0, 31, 32, 33, 0, 0, 0, - 8, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39,
305, 0, 0, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 697, 698, 21, 154, 155, 221, 0, 222, 223, 224, 225, 226, 227, 228, 229, 22,
156, 23, 157, 230, 0, 158, 24, 231, 0, 0, 27, 28, 159, 160, 232, 233, 161, 29,
234, 162, 163, 0, 164, 305, 165, 166, 0, 0, 0, 305, 0, 0, 0, 0, 0, 0, 66, 345,
346, 347, 348, 349, 350, 351, 352, 353, 354, 235, 167, 168, 169, 170, 31, 32, 33,
0, 236, 112, 0, 0, 171, 0, 355, 35, 238, 0, 0, 0, 0, 0, 0, 173, 305, 0, 174, 175,
176, 0, 0, 0, 0, 177, 178, 30, 68, 0, 69, 70, 0, 32, 0, 0, 0, 0, 446, 0, 71, 0,
0, 72, 73, 0, 0, 0, 447, 448, 0, 74, 0, 0, 0, 75, 76, 305, 0, 0, 2, 3, 4, 111, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 748, 749, 21, 154, 155, 221,
0, 222, 223, 224, 225, 226, 227, 228, 229, 22, 156, 23, 157, 230, 0, 158, 24,
231, 0, 0, 27, 28, 159, 160, 232, 233, 161, 29, 234, 162, 163, 0, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 305, 0, 0, 0, 0, 0, 0, 0, 0, 305, 235, 167,
168, 169, 170, 31, 32, 33, 0, 236, 112, 0, 0, 171, 0, 0, 35, 238, 0, 0, 0, 0, 0,
0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 2, 3, 4, 111, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 154, 155, 221, 0, 222, 223,
224, 225, 226, 227, 228, 229, 22, 156, 23, 157, 230, 0, 158, 24, 231, 0, 0, 27,
28, 159, 160, 232, 233, 161, 29, 234, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 235, 167, 168, 169, 170, 31,
32, 33, 0, 236, 112, 237, 0, 171, 0, 0, 35, 238, 0, 0, 0, 0, 0, 0, 173, 0, 0,
174, 175, 176, 0, 0, 0, 0, 177, 178, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 154, 155, 221, 0, 222, 223, 224, 225, 226,
227, 228, 229, 22, 156, 23, 157, 230, 0, 158, 24, 231, 0, 0, 27, 28, 159, 160,
232, 233, 161, 29, 234, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 235, 167, 168, 169, 170, 31, 32,
33, 0, 236, 112, 0, 0, 171, 22, 0, 35, 238, 0, 0, 0, 0, 0, 0, 173, 27, 28, 174,
175, 176, 0, 0, 29, 0, 177, 178, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 0, 0, 21, 154, 155, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 22,
156, 23, 157, 0, 0, 158, 24, 35, 36, 0, 27, 28, 159, 160, 0, 0, 161, 29, 0, 162,
163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 167, 168, 169, 170, 31, 32, 33, 0, 545, 0, 0, 0, 171, 0, 0, 35, 238,
0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 22, 156, 0, 157, 0, 0, 158, 0, 0, 0, 0, 27, 28, 159, 160, 0, 0, 161, 29,
0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 31, 32, 33, 0, 0, 318, 0, 0, 171, 0, 0,
35, 238, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 22, 156, 0, 157, 0, 0, 158, 0, 0, 0, 0, 27, 28, 159, 160, 0, 0,
161, 29, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 30, 167, 168, 169, 170, 31, 32, 33, 0, 0, 0,
0, 0, 171, 22, 0, 35, 238, 0, 0, 0, 0, 0, 0, 173, 27, 28, 174, 175, 176, 0, 0,
29, 0, 177, 178, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 255, 21, 0, 256, 0, 0, 0, 0, 31, 0, 33, 0, 257, 0, 22, 0, 23,
0, 0, 35, 36, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0,
22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0, 37, 38,
39, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
255, 0, 21, 0, 0, 0, 0, 0, 31, 0, 33, 0, 257, 0, 0, 22, 0, 23, 0, 35, 36, 0, 24,
0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32,
33, 0, 0, 0, - 8, 0, 34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 1, 0, 2,
3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 567, 0, 21, 0,
0, 0, 0, 0, 31, 0, 33, 0, 0, 0, 0, 22, 0, 0, 0, 35, 36, 0, 24, 0, 0, 0, 27, 28,
0, 0, 1, 0, 0, 29, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0,
34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 66, 0, 0, 255, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 257,
0, 0, 0, 295, - 159, 0, 35, 36, 0, 0, - 159, 0, 286, 0, 37, 38, 39, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 30, 68, 66, 69, 70, 0, 32, 0, 0, 0, 0, 0,
0, 71, 0, 22, 72, 73, 0, 0, 0, 447, 448, 0, 74, 0, 27, 28, 75, 76, 0, 0, 0, 29,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 67, 68, 0, 69, 70, 0, 32, 0, 0, 0,
0, 255, 0, 71, 0, 0, 72, 73, 0, 31, 66, 33, 0, 257, 74, 0, 0, 285, 75, 76, 35,
36, 0, 0, 0, 0, 286, 0, 37, 38, 39, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 30, 68, 0, 69, 70, 0, 32, 0, 22, 0,
23, 0, 0, 71, 0, 24, 72, 73, 0, 27, 28, 0, 0, 0, 74, 0, 29, 0, 75, 76, 0, 0, 0,
2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21,
0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0, 112, 22, 205, 23, 0, 0, 35, 36, 24, 0, 0, 0,
27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 22, 0,
112, 0, 0, 0, 0, 24, 35, 36, 0, 27, 28, 0, 0, 0, 0, 0, 29, 154, 155, 221, 0, 222,
223, 224, 225, 226, 227, 228, 229, 0, 156, 0, 157, 230, 0, 158, 0, 231, 0, 0, 0,
0, 159, 160, 232, 233, 161, 0, 234, 162, 163, 31, 164, 33, 165, 166, 0, 0, 0, 0,
0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 235, 167, 168, 169, 170, 0,
32, 154, 155, 236, 112, 0, 0, 171, 0, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158,
0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168,
169, 170, 0, 32, 0, 0, 0, 306, 668, 0, 171, 0, 0, 0, 172, 154, 155, 0, 447, 448,
0, 173, 0, 0, 174, 175, 176, 0, 156, 0, 157, 177, 178, 158, 0, 0, 0, 0, 0, 0,
159, 160, 0, 0, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 0, 0, 0,
306, 745, 0, 171, 0, 0, 0, 172, 154, 155, 0, 447, 448, 0, 173, 0, 0, 174, 175,
176, 0, 156, 0, 157, 177, 178, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0,
162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 0, 0, 0, 306, 0, 0, 171, 0, 0, 0, 172,
154, 155, 0, 447, 448, 0, 173, 0, 0, 174, 175, 176, 0, 156, 0, 157, 177, 178,
158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169,
170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 0, 0, 0, 172, 0, 0, 156, 0, 157, 263, 173,
158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0,
165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167,
168, 169, 170, 0, 32, 154, 155, 0, 306, 0, 0, 171, 0, 0, 0, 172, 0, 0, 156, 0,
157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162,
163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 398, 0, 0, 0, 171, 0, 0, 0,
172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178,
161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171,
0, 0, 0, 172, 0, 0, 156, 0, 157, 465, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160,
177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0,
0, 0, 171, 518, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0,
0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32,
154, 155, 0, 0, 0, 0, 171, 551, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0,
174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168,
169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 0, 0, 0, 172, 0, 0, 156, 0, 157, 595,
173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0,
164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 167, 168, 169, 170, 0, 32, 154, 155, 635, 0, 0, 0, 171, 0, 0, 0, 172, 0, 0,
156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0,
162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 637, 0, 0, 0, 171, 0, 0, 0,
172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178,
161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171,
689, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160,
177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0,
0, 0, 171, 692, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0,
0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32,
154, 155, 695, 0, 0, 0, 171, 0, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0,
174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168,
169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 726, 0, 0, 172, 0, 0, 156, 0, 157, 0,
173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 162, 163, 0,
164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 730, 0, 0, 172, 0, 0,
156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0,
162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171, 732, 0, 0,
172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178,
161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 171,
761, 0, 0, 172, 0, 0, 156, 0, 157, 0, 173, 158, 0, 174, 175, 176, 0, 0, 159, 160,
177, 178, 161, 0, 0, 162, 163, 0, 164, 0, 165, 166, 0, 0, 0, 0, 154, 155, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32, 0, 159,
160, 0, 0, 161, 171, 0, 0, 163, 172, 164, 0, 165, 166, 0, 0, 173, 0, 0, 174, 175,
176, 0, 0, 0, 0, 177, 178, 0, 0, 0, 0, 0, 0, 0, 30, 167, 168, 169, 170, 0, 32,
154, 155, 0, 306, 0, 0, 171, 0, 0, 0, 172, 0, 0, 156, 0, 0, 0, 173, 0, 0, 174,
175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 0, 163, 0, 164, 0, 165, 166, 0, 0,
0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 30, 167, 168, 169,
170, 0, 32, 0, 159, 160, 612, 0, 161, 171, 0, 0, 163, 172, 164, 0, 165, 166, 0,
0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 0, 0, 0, 0, 0, 0, 0, 30, 167,
168, 169, 170, 0, 32, 154, 155, 0, 0, 0, 0, 301, 0, 0, 0, 172, 0, 0, 156, 0, 0,
0, 173, 0, 0, 174, 175, 176, 0, 0, 159, 160, 177, 178, 161, 0, 0, 0, 163, 0, 164,
0, 165, 166, 0, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0,
30, 167, 168, 169, 170, 0, 32, 0, 159, 160, 0, 0, 161, 315, 0, 0, 163, 172, 164,
0, 165, 166, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 0, 0, 0, 0, 0,
0, 0, 30, 167, 168, 169, 170, 0, 32, 0, 0, 0, 0, 0, 0, 171, 0, 0, 0, 172, 0, 0,
0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 138, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
31, 0, 33, 0, 0, 139, 0, 0, 0, 0, 0, 35, 36}; static short Cyc_yycheck[ 4330u]={
55, 88, 23, 83, 55, 365, 56, 55, 63, 158, 0, 117, 239, 240, 85, 242, 205, 158,
157, 118, 117, 55, 300, 54, 57, 17, 277, 58, 71, 1, 109, 87, 22, 23, 171, 25,
427, 419, 117, 118, 173, 174, 361, 176, 34, 100, 101, 102, 103, 100, 71, 41, 70,
5, 6, 76, 46, 112, 80, 114, 193, 112, 52, 114, 54, 686, 100, 57, 111, 142, 60,
121, 463, 66, 67, 287, 458, 34, 151, 41, 114, 71, 104, 402, 106, 297, 76, 105,
110, 46, 442, 318, 98, 85, 85, 83, 448, 142, 85, 93, 91, 107, 92, 93, 94, 95,
151, 95, 139, 90, 103, 104, 83, 85, 85, 224, 103, 88, 108, 91, 101, 230, 110,
232, 233, 115, 98, 17, 18, 19, 92, 93, 94, 95, 105, 125, 99, 96, 128, 417, 130,
100, 164, 764, 96, 88, 117, 118, 426, 112, 59, 206, 99, 94, 260, 206, 87, 103,
115, 100, 781, 151, 105, 125, 301, 112, 303, 98, 789, 65, 84, 104, 206, 794, 117,
47, 48, 49, 315, 112, 532, 90, 94, 95, 239, 240, 95, 242, 239, 240, 95, 242, 265,
98, 151, 217, 94, 95, 103, 279, 171, 83, 593, 85, 100, 65, 88, 1, 104, 342, 343,
344, 285, 293, 204, 103, 100, 98, 65, 109, 110, 111, 295, 105, 108, 215, 257, 85,
218, 61, 62, 63, 89, 90, 92, 117, 118, 377, 86, 289, 100, 380, 213, 94, 104, 385,
95, 204, 98, 100, 122, 241, 612, 100, 645, 310, 277, 104, 215, 650, 291, 95, 119,
318, 297, 85, 98, 318, 140, 141, 213, 93, 92, 85, 106, 107, 98, 85, 99, 100, 92,
103, 90, 91, 241, 107, 95, 277, 278, 171, 1, 85, 481, 101, 265, 90, 405, 91, 407,
95, 94, 96, 94, 442, 98, 295, 98, 297, 279, 448, 98, 136, 283, 284, 285, 109,
110, 111, 97, 94, 310, 408, 293, 103, 295, 100, 106, 106, 107, 300, 301, 213,
303, 584, 334, 335, 336, 337, 295, 394, 283, 284, 285, 394, 315, 85, 171, 94,
205, 740, 601, 488, 295, 100, 93, 100, 411, 300, 491, 98, 411, 434, 468, 411,
103, 387, 94, 94, 107, 620, 469, 468, 100, 651, 85, 85, 85, 411, 97, 369, 91,
371, 91, 92, 94, 90, 95, 98, 419, 99, 94, 96, 99, 532, 105, 105, 100, 279, 109,
110, 111, 283, 284, 285, 229, 95, 377, 369, 98, 371, 94, 293, 601, 295, 385, 94,
100, 467, 300, 301, 94, 303, 100, 444, 94, 524, 100, 458, 449, 286, 100, 620, 94,
315, 377, 786, 96, 408, 100, 100, 100, 792, 385, 98, 105, 435, 417, 100, 103,
306, 440, 87, 105, 559, 444, 426, 313, 100, 607, 449, 98, 567, 105, 434, 321,
408, 607, 718, 100, 85, 100, 99, 104, 435, 417, 105, 301, 616, 96, 98, 560, 670,
100, 426, 309, 96, 112, 312, 1, 100, 315, 85, 86, 87, 484, 377, 98, 468, 469,
356, 96, 556, 327, 385, 361, 556, 98, 98, 365, 736, 68, 69, 64, 65, 112, 113,
107, 488, 109, 110, 111, 630, 556, 98, 633, 468, 408, 484, 98, 718, 99, 100, 563,
5, 6, 417, 98, 764, 363, 99, 100, 531, 99, 100, 426, 402, 773, 98, 538, 98, 605,
408, 115, 524, 605, 99, 100, 116, 415, 786, 1, 70, 789, 584, 389, 96, 391, 392,
114, 394, 96, 605, 85, 96, 531, 99, 100, 96, 91, 95, 689, 99, 100, 692, 95, 98,
468, 469, 95, 560, 85, 86, 701, 1, 584, 98, 109, 110, 111, 98, 706, 99, 100, 22,
23, 627, 488, 99, 100, 99, 100, 720, 98, 470, 99, 100, 725, 726, 22, 23, 729,
730, 105, 732, 481, 99, 100, 85, 99, 100, 99, 100, 719, 99, 100, 606, 99, 100,
94, 628, 629, 94, 524, 98, 85, 96, 755, 332, 333, 758, 91, 760, 761, 94, 97, 338,
339, 98, 99, 100, 340, 341, 85, 606, 105, 99, 107, 105, 99, 108, 779, 96, 100,
97, 96, 96, 697, 85, 560, 99, 651, 736, 100, 91, 100, 736, 94, 71, 25, 98, 98,
99, 100, 99, 94, 100, 103, 105, 30, 107, 98, 109, 110, 111, 105, 94, 42, 697,
651, 764, 90, 90, 98, 764, 99, 112, 541, 98, 773, 85, 545, 546, 773, 99, 606,
100, 96, 100, 99, 90, 99, 786, 99, 85, 789, 786, 108, 99, 789, 98, 100, 99, 96,
100, 601, 716, 99, 98, 719, 99, 96, 100, 99, 96, 96, 612, 95, 25, 615, 25, 26,
94, 96, 620, 99, 103, 32, 154, 94, 651, 157, 158, 38, 716, 40, 104, 719, 43, 165,
166, 99, 108, 99, 99, 50, 51, 173, 174, 54, 176, 108, 57, 58, 95, 60, 103, 62,
63, 99, 99, 95, 94, 85, 90, 100, 99, 193, 99, 99, 634, 635, 99, 637, 670, 639,
99, 28, 99, 98, 85, 86, 87, 88, 89, 98, 91, 95, 105, 100, 95, 96, 96, 98, 86,
716, 99, 102, 719, 96, 94, 106, 107, 667, 109, 100, 99, 112, 113, 114, 96, 86,
99, 0, 119, 120, 105, 99, 0, 310, 574, 440, 718, 531, 204, 58, 213, 691, 567,
739, 694, 695, 563, 117, 450, 291, 278, 584, 326, 328, - 1, - 1, 0, 1, 329, 3, 4,
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 330, - 1, 24, - 1,
- 1, - 1, 734, - 1, - 1, - 1, - 1, - 1, - 1, 331, 742, 37, - 1, 39, - 1, - 1,
748, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, 763, 326, -
1, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342,
343, 344, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, - 1, - 1, 90, 91,
92, - 1, - 1, - 1, 96, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1,
109, 110, 111, 380, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35,
36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54,
55, 56, 57, 58, - 1, 60, 442, 62, 63, - 1, - 1, - 1, 448, - 1, - 1, - 1, - 1, -
1, - 1, 38, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 85, 86, 87, 88, 89, 90, 91,
92, - 1, 94, 95, - 1, - 1, 98, - 1, 97, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1,
109, 491, - 1, 112, 113, 114, - 1, - 1, - 1, - 1, 119, 120, 85, 86, - 1, 88, 89,
- 1, 91, - 1, - 1, - 1, - 1, 96, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, 106,
107, - 1, 109, - 1, - 1, - 1, 113, 114, 532, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30,
31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49,
50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 607, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
616, 85, 86, 87, 88, 89, 90, 91, 92, - 1, 94, 95, - 1, - 1, 98, - 1, - 1, 101,
102, - 1, - 1, - 1, - 1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1,
- 1, 119, 120, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, -
1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, 90, 91, 92, - 1,
94, 95, 96, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, 109, - 1,
- 1, 112, 113, 114, - 1, - 1, - 1, - 1, 119, 120, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30,
31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49,
50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, -
1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 85, 86, 87,
88, 89, 90, 91, 92, - 1, 94, 95, - 1, - 1, 98, 37, - 1, 101, 102, - 1, - 1, - 1,
- 1, - 1, - 1, 109, 48, 49, 112, 113, 114, - 1, - 1, 55, - 1, 119, 120, 3, 4, 5,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 90, - 1, 92, 37, 38, 39, 40, - 1, - 1, 43, 44,
101, 102, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, - 1, 62,
63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, -
1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, 109, - 1, - 1, 112, 113,
114, - 1, - 1, - 1, - 1, 119, 120, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54,
55, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88,
89, 90, 91, 92, - 1, - 1, 95, - 1, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, -
1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1, - 1, 119, 120, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1,
- 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, - 1, 62, 63, -
1, - 1, - 1, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
- 1, 85, 86, 87, 88, 89, 90, 91, 92, - 1, - 1, - 1, - 1, - 1, 98, 37, - 1, 101,
102, - 1, - 1, - 1, - 1, - 1, - 1, 109, 48, 49, 112, 113, 114, - 1, - 1, 55, - 1,
119, 120, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, 82, 24, - 1, 85, - 1, - 1, - 1, - 1, 90, - 1, 92, - 1, 94, - 1, 37,
- 1, 39, - 1, - 1, 101, 102, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1,
55, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, 37, - 1, 90,
91, 92, - 1, - 1, - 1, - 1, - 1, 98, 48, 49, 101, 102, - 1, - 1, - 1, 55, - 1, -
1, 109, 110, 111, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, 82, - 1, 24, - 1, - 1, - 1, - 1, - 1, 90, - 1, 92, - 1, 94, - 1,
- 1, 37, - 1, 39, - 1, 101, 102, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, -
1, - 1, 55, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, 37, -
1, 90, 91, 92, - 1, - 1, - 1, 96, - 1, 98, 48, 49, 101, 102, - 1, - 1, - 1, 55,
- 1, - 1, 109, 110, 111, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, 82, - 1, 24, - 1, - 1, - 1, - 1, - 1, 90, - 1, 92, - 1, - 1,
- 1, - 1, 37, - 1, - 1, - 1, 101, 102, - 1, 44, - 1, - 1, - 1, 48, 49, - 1, - 1,
1, - 1, - 1, 55, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, 37,
- 1, 90, 91, 92, - 1, - 1, - 1, - 1, - 1, 98, 48, 49, 101, 102, - 1, - 1, - 1,
55, - 1, - 1, 109, 110, 111, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, 82, - 1, - 1, 85, - 1, - 1,
- 1, - 1, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, 99, - 1, 101, 102, - 1, - 1,
105, - 1, 107, - 1, 109, 110, 111, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, 85, 86, 38, 88, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, 98, - 1, 37,
101, 102, - 1, - 1, - 1, 106, 107, - 1, 109, - 1, 48, 49, 113, 114, - 1, - 1, -
1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
85, 86, - 1, 88, 89, - 1, 91, - 1, - 1, - 1, - 1, 82, - 1, 98, - 1, - 1, 101,
102, - 1, 90, 38, 92, - 1, 94, 109, - 1, - 1, 98, 113, 114, 101, 102, - 1, - 1,
- 1, - 1, 107, - 1, 109, 110, 111, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, 85, 86, - 1, 88, 89, -
1, 91, - 1, 37, - 1, 39, - 1, - 1, 98, - 1, 44, 101, 102, - 1, 48, 49, - 1, - 1,
- 1, 109, - 1, 55, - 1, 113, 114, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1,
90, - 1, 92, - 1, - 1, 95, 37, 97, 39, - 1, - 1, 101, 102, 44, - 1, - 1, - 1, 48,
49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 90, - 1, 92, 37, - 1, 95, - 1, - 1, - 1, - 1, 44, 101, 102, - 1,
48, 49, - 1, - 1, - 1, - 1, - 1, 55, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35,
36, - 1, 38, - 1, 40, 41, - 1, 43, - 1, 45, - 1, - 1, - 1, - 1, 50, 51, 52, 53,
54, - 1, 56, 57, 58, 90, 60, 92, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88,
89, - 1, 91, 25, 26, 94, 95, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1,
40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1,
57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1,
91, - 1, - 1, - 1, 95, 96, - 1, 98, - 1, - 1, - 1, 102, 25, 26, - 1, 106, 107, -
1, 109, - 1, - 1, 112, 113, 114, - 1, 38, - 1, 40, 119, 120, 43, - 1, - 1, - 1,
- 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, - 1, - 1, - 1, 95, 96, - 1, 98,
- 1, - 1, - 1, 102, 25, 26, - 1, 106, 107, - 1, 109, - 1, - 1, 112, 113, 114, -
1, 38, - 1, 40, 119, 120, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54,
- 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88,
89, - 1, 91, - 1, - 1, - 1, 95, - 1, - 1, 98, - 1, - 1, - 1, 102, 25, 26, - 1,
106, 107, - 1, 109, - 1, - 1, 112, 113, 114, - 1, 38, - 1, 40, 119, 120, 43, - 1,
- 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, - 1,
62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1,
- 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, 108, 109, 43, - 1, 112,
113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, 95, - 1, - 1,
98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114,
- 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, 94, - 1, - 1, - 1, 98, - 1,
- 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1,
50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, - 1, - 1, - 1,
102, - 1, - 1, 38, - 1, 40, 108, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51,
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
- 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1,
40, 108, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1,
57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1,
91, 25, 26, 94, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, -
1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58,
- 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
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
short*)({ unsigned int _temp3186= 10000u; short* _temp3187=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3186));{ unsigned int
_temp3188= _temp3186; unsigned int i; for( i= 0; i <  _temp3188; i ++){
_temp3187[ i]=( short) 0;}}; _temp3187;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3183= 10000u; void** _temp3184=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3183));{ unsigned int _temp3185= _temp3183;
unsigned int i; for( i= 0; i <  _temp3185; i ++){ _temp3184[ i]= Cyc_yylval;}};
_temp3184;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3180= 10000u; struct Cyc_Yyltype* _temp3181=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3180));{
unsigned int _temp3182= _temp3180; unsigned int i; for( i= 0; i <  _temp3182; i
++){ _temp3181[ i]= Cyc_yynewloc();}}; _temp3181;}); int yystacksize= 10000;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:
yyss[ _check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate;
if( yyssp_offset >=  yystacksize -  1){ if( yystacksize >=  10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 801u, yystate)]; if( yyn ==  - 32768){ goto
yydefault;} if( Cyc_yychar ==  - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar >  0? Cyc_yychar <= 
348: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 349u, Cyc_yychar)]:
233;} yyn += yychar1; if(( yyn <  0? 1: yyn >  4329)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4330u, yyn)] !=  yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4330u, yyn)]; if( yyn <  0){
if( yyn ==  - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
==  0){ goto yyerrlab;}} if( yyn ==  800){ return 0;} if( Cyc_yychar !=  0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus !=  0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 801u, yystate)]; if(
yyn ==  0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 405u, yyn)]; if( yylen >  0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset +  1) -  yylen)];} switch(
yyn){ case 1: _LL1383: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1384:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1386=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1386[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1387; _temp1387.tag= Cyc_DeclList_tok; _temp1387.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1387;});
_temp1386;}); break; case 3: _LL1385: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1389=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1389[ 0]=({ struct Cyc_DeclList_tok_struct _temp1390; _temp1390.tag= Cyc_DeclList_tok;
_temp1390.f1=({ struct Cyc_List_List* _temp1391=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1391->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1392=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1392->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1393=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1393[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1394; _temp1394.tag= Cyc_Absyn_Using_d;
_temp1394.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1394.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1394;}); _temp1393;}));
_temp1392->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1392;});
_temp1391->tl= 0; _temp1391;}); _temp1390;}); _temp1389;}); Cyc_Lex_leave_using();
break; case 4: _LL1388: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1396=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1396[ 0]=({ struct Cyc_DeclList_tok_struct _temp1397; _temp1397.tag= Cyc_DeclList_tok;
_temp1397.f1=({ struct Cyc_List_List* _temp1398=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1398->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1399=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1399->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1400=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1400[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1401; _temp1401.tag= Cyc_Absyn_Using_d;
_temp1401.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp1401.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1401;});
_temp1400;})); _temp1399->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1399;}); _temp1398->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1398;}); _temp1397;});
_temp1396;}); break; case 5: _LL1395: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1403=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1403[ 0]=({ struct Cyc_DeclList_tok_struct _temp1404; _temp1404.tag= Cyc_DeclList_tok;
_temp1404.f1=({ struct Cyc_List_List* _temp1405=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1405->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1406=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1406->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1407=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1407[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1408; _temp1408.tag=
Cyc_Absyn_Namespace_d; _temp1408.f1=({ struct _tagged_arr* _temp1409=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1409[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1409;});
_temp1408.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1408;}); _temp1407;})); _temp1406->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1406;}); _temp1405->tl= 0; _temp1405;}); _temp1404;}); _temp1403;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1402: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1411=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1411[ 0]=({ struct Cyc_DeclList_tok_struct _temp1412; _temp1412.tag= Cyc_DeclList_tok;
_temp1412.f1=({ struct Cyc_List_List* _temp1413=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1413->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1414=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1414->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1415=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1415[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1416; _temp1416.tag=
Cyc_Absyn_Namespace_d; _temp1416.f1=({ struct _tagged_arr* _temp1417=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1417[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1417;});
_temp1416.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1416;}); _temp1415;})); _temp1414->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1414;}); _temp1413->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1413;}); _temp1412;});
_temp1411;}); break; case 7: _LL1410: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1419=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1419[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1420; _temp1420.tag= Cyc_DeclList_tok; _temp1420.f1=({ struct Cyc_List_List*
_temp1421=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1421->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1422=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1422->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1423=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1423[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1424; _temp1424.tag= Cyc_Absyn_ExternC_d;
_temp1424.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1424;}); _temp1423;})); _temp1422->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1422;}); _temp1421->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1421;}); _temp1420;});
_temp1419;}); break; case 8: _LL1418: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1426=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1426[ 0]=({ struct Cyc_DeclList_tok_struct _temp1427; _temp1427.tag= Cyc_DeclList_tok;
_temp1427.f1= 0; _temp1427;}); _temp1426;}); break; case 9: _LL1425: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1429=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1429[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1430; _temp1430.tag= Cyc_DeclList_tok; _temp1430.f1=({ struct Cyc_List_List*
_temp1431=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1431->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1432=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1432[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1433; _temp1433.tag= Cyc_Absyn_Fn_d;
_temp1433.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1433;}); _temp1432;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1431->tl=
0; _temp1431;}); _temp1430;}); _temp1429;}); break; case 10: _LL1428: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1434: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1436=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1436[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1437; _temp1437.tag= Cyc_FnDecl_tok; _temp1437.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1437;}); _temp1436;}); break; case 12: _LL1435: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1439=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1439[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1440; _temp1440.tag= Cyc_FnDecl_tok;
_temp1440.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1441=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1441->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1441;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1440;}); _temp1439;}); break; case 13: _LL1438: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1443=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1443[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1444; _temp1444.tag= Cyc_FnDecl_tok;
_temp1444.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1444;}); _temp1443;}); break; case 14: _LL1442: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1446=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1446[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1447; _temp1447.tag= Cyc_FnDecl_tok;
_temp1447.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1448=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1448->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1448;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1447;}); _temp1446;}); break; case 15: _LL1445: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1450=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1450[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1451; _temp1451.tag= Cyc_FnDecl_tok;
_temp1451.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1452=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1452->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1452;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1451;}); _temp1450;}); break; case 16: _LL1449: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1454=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1454[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1455; _temp1455.tag= Cyc_FnDecl_tok;
_temp1455.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1456=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1456->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1456;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1455;}); _temp1454;}); break; case 17: _LL1453: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1457:
Cyc_Lex_leave_using(); break; case 19: _LL1458: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1460=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1460[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1460;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1459: Cyc_Lex_leave_namespace(); break; case 21: _LL1461: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1463=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1463[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1464; _temp1464.tag= Cyc_DeclList_tok; _temp1464.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1464;}); _temp1463;}); break; case 22: _LL1462: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1466=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1466[ 0]=({ struct Cyc_DeclList_tok_struct _temp1467; _temp1467.tag= Cyc_DeclList_tok;
_temp1467.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1467;}); _temp1466;}); break; case 23: _LL1465: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1469=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1469[ 0]=({ struct Cyc_DeclList_tok_struct _temp1470; _temp1470.tag= Cyc_DeclList_tok;
_temp1470.f1=({ struct Cyc_List_List* _temp1471=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1471->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1471->tl=
0; _temp1471;}); _temp1470;}); _temp1469;}); break; case 24: _LL1468: { struct
Cyc_List_List* _temp1473= 0;{ struct Cyc_List_List* _temp1474= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0;
_temp1474 !=  0; _temp1474=(( struct Cyc_List_List*) _check_null( _temp1474))->tl){
struct _tagged_arr* _temp1475=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1474))->hd; struct _tuple1* _temp1476=({ struct _tuple1*
_temp1479=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1479->f1=
Cyc_Absyn_rel_ns_null; _temp1479->f2= _temp1475; _temp1479;}); struct Cyc_Absyn_Vardecl*
_temp1477= Cyc_Absyn_new_vardecl( _temp1476, Cyc_Absyn_wildtyp( 0), 0);
_temp1473=({ struct Cyc_List_List* _temp1478=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1478->hd=( void*) _temp1477; _temp1478->tl=
_temp1473; _temp1478;});}} _temp1473=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1473); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1480=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1480[ 0]=({ struct Cyc_DeclList_tok_struct _temp1481; _temp1481.tag= Cyc_DeclList_tok;
_temp1481.f1=({ struct Cyc_List_List* _temp1482=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1482->hd=( void*) Cyc_Absyn_letv_decl(
_temp1473, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1482->tl= 0; _temp1482;}); _temp1481;});
_temp1480;}); break;} case 25: _LL1472: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1483:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1485=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1485[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1486; _temp1486.tag= Cyc_DeclList_tok; _temp1486.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1486;});
_temp1485;}); break; case 27: _LL1484: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1488=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1488[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1489; _temp1489.tag= Cyc_DeclSpec_tok;
_temp1489.f1=({ struct Cyc_Parse_Declaration_spec* _temp1490=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1490->sc=({ struct
Cyc_Core_Opt* _temp1491=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp1491->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1491;});
_temp1490->tq= Cyc_Absyn_empty_tqual(); _temp1490->type_specs= 0; _temp1490->is_inline=
0; _temp1490->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1490;}); _temp1489;});
_temp1488;}); break; case 28: _LL1487: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc !=  0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1493=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1493[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1494; _temp1494.tag= Cyc_DeclSpec_tok; _temp1494.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1495=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1495->sc=({ struct Cyc_Core_Opt* _temp1496=( struct Cyc_Core_Opt*)
_cycalloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1496->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1496;});
_temp1495->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1495->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1495->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1495->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1495;}); _temp1494;}); _temp1493;}); break; case 29: _LL1492: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1498=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1498[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1499; _temp1499.tag= Cyc_DeclSpec_tok; _temp1499.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1500=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1500->sc= 0; _temp1500->tq= Cyc_Absyn_empty_tqual(); _temp1500->type_specs=({
struct Cyc_List_List* _temp1501=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1501->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1501->tl= 0;
_temp1501;}); _temp1500->is_inline= 0; _temp1500->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1500;});
_temp1499;}); _temp1498;}); break; case 30: _LL1497: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1503=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1503[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1504; _temp1504.tag= Cyc_DeclSpec_tok;
_temp1504.f1=({ struct Cyc_Parse_Declaration_spec* _temp1505=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1505->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1505->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1505->type_specs=({ struct Cyc_List_List* _temp1506=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1506->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1506->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1506;});
_temp1505->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1505->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1505;}); _temp1504;}); _temp1503;}); break; case 31: _LL1502: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1508=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1508[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1509; _temp1509.tag= Cyc_DeclSpec_tok; _temp1509.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1510=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1510->sc= 0; _temp1510->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1510->type_specs=
0; _temp1510->is_inline= 0; _temp1510->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1510;});
_temp1509;}); _temp1508;}); break; case 32: _LL1507: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1512=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1512[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1513; _temp1513.tag= Cyc_DeclSpec_tok;
_temp1513.f1=({ struct Cyc_Parse_Declaration_spec* _temp1514=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1514->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1514->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1514->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1514->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1514->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1514;}); _temp1513;}); _temp1512;}); break; case 33: _LL1511: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1516=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1516[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1517; _temp1517.tag= Cyc_DeclSpec_tok; _temp1517.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1518=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1518->sc= 0; _temp1518->tq= Cyc_Absyn_empty_tqual(); _temp1518->type_specs=
0; _temp1518->is_inline= 1; _temp1518->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1518;});
_temp1517;}); _temp1516;}); break; case 34: _LL1515: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1520=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1520[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1521; _temp1521.tag= Cyc_DeclSpec_tok;
_temp1521.f1=({ struct Cyc_Parse_Declaration_spec* _temp1522=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1522->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1522->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1522->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1522->is_inline=
1; _temp1522->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1522;}); _temp1521;}); _temp1520;}); break; case 35: _LL1519: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1524=( struct Cyc_StorageClass_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1524[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1525; _temp1525.tag= Cyc_StorageClass_tok;
_temp1525.f1= Cyc_Parse_Auto_sc; _temp1525;}); _temp1524;}); break; case 36:
_LL1523: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1527=( struct
Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1527[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1528; _temp1528.tag=
Cyc_StorageClass_tok; _temp1528.f1= Cyc_Parse_Register_sc; _temp1528;});
_temp1527;}); break; case 37: _LL1526: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1530=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1530[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1531; _temp1531.tag= Cyc_StorageClass_tok; _temp1531.f1= Cyc_Parse_Static_sc;
_temp1531;}); _temp1530;}); break; case 38: _LL1529: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1533=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1533[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1534; _temp1534.tag= Cyc_StorageClass_tok; _temp1534.f1= Cyc_Parse_Extern_sc;
_temp1534;}); _temp1533;}); break; case 39: _LL1532: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1536=( struct Cyc_StorageClass_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1536[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1537; _temp1537.tag= Cyc_StorageClass_tok;
_temp1537.f1= Cyc_Parse_ExternC_sc; _temp1537;}); _temp1536;}); break; case 40:
_LL1535: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1539=( struct
Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1539[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1540; _temp1540.tag=
Cyc_StorageClass_tok; _temp1540.f1= Cyc_Parse_Typedef_sc; _temp1540;});
_temp1539;}); break; case 41: _LL1538: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1542=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1542[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1543; _temp1543.tag= Cyc_StorageClass_tok; _temp1543.f1= Cyc_Parse_Abstract_sc;
_temp1543;}); _temp1542;}); break; case 42: _LL1541: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1545=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1545[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1546; _temp1546.tag=
Cyc_AttributeList_tok; _temp1546.f1= 0; _temp1546;}); _temp1545;}); break; case
43: _LL1544: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1547: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1549=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1549[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1550; _temp1550.tag=
Cyc_AttributeList_tok; _temp1550.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1550;});
_temp1549;}); break; case 45: _LL1548: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1552=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1552[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1553; _temp1553.tag=
Cyc_AttributeList_tok; _temp1553.f1=({ struct Cyc_List_List* _temp1554=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1554->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1554->tl= 0; _temp1554;}); _temp1553;}); _temp1552;});
break; case 46: _LL1551: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1556=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1556[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1557; _temp1557.tag=
Cyc_AttributeList_tok; _temp1557.f1=({ struct Cyc_List_List* _temp1558=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1558->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1558->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1558;}); _temp1557;});
_temp1556;}); break; case 47: _LL1555: { struct _tagged_arr _temp1560= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1560, sizeof( unsigned char)) >  4?*(( const unsigned char*)
_check_unknown_subscript( _temp1560, sizeof( unsigned char), 0)) == '_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1560, sizeof( unsigned char),
1)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1560,
sizeof( unsigned char),( int)( _get_arr_size( _temp1560, sizeof( unsigned char))
-  2))) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1560,
sizeof( unsigned char),( int)( _get_arr_size( _temp1560, sizeof( unsigned char))
-  3))) == '_': 0){ _temp1560=( struct _tagged_arr) Cyc_Std_substring( _temp1560,
2, _get_arr_size( _temp1560, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0;
i <  16u; ++ i){ if( Cyc_Std_strcmp( _temp1560,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1561=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1561[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1562; _temp1562.tag= Cyc_Attribute_tok; _temp1562.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1562;}); _temp1561;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1563=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1563[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1564; _temp1564.tag= Cyc_Attribute_tok; _temp1564.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1564;}); _temp1563;});} break;}} case
48: _LL1559: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1566=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1566[ 0]=({ struct Cyc_Attribute_tok_struct _temp1567; _temp1567.tag= Cyc_Attribute_tok;
_temp1567.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1567;}); _temp1566;});
break; case 49: _LL1565: { struct _tagged_arr _temp1569= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1572; int _temp1573; struct _tuple15* _temp1570= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1572=*
_temp1570; _LL1574: _temp1573= _temp1572.f2; goto _LL1571; _LL1571: { void* a;
if( Cyc_Std_zstrcmp( _temp1569, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1569, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1573 <  0? 1: _temp1573 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 0 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1575=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1575[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1576; _temp1576.tag= Cyc_Absyn_Regparm_att; _temp1576.f1= _temp1573;
_temp1576;}); _temp1575;});} else{ if( Cyc_Std_zstrcmp( _temp1569, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1569, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1573 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1573;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1577=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1577[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1578; _temp1578.tag= Cyc_Absyn_Aligned_att; _temp1578.f1= _temp1573;
_temp1578;}); _temp1577;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1579=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1579[ 0]=({ struct Cyc_Attribute_tok_struct _temp1580; _temp1580.tag= Cyc_Attribute_tok;
_temp1580.f1=( void*) a; _temp1580;}); _temp1579;}); break;}} case 50: _LL1568: {
struct _tagged_arr _temp1582= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1583= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1582, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1582, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1584=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1584[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1585; _temp1585.tag=
Cyc_Absyn_Section_att; _temp1585.f1= _temp1583; _temp1585;}); _temp1584;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1586=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1586[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1587; _temp1587.tag= Cyc_Attribute_tok; _temp1587.f1=(
void*) a; _temp1587;}); _temp1586;}); break;} case 51: _LL1581: { struct
_tagged_arr _temp1589= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1590= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1593; struct _tuple15 _temp1591=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1594:
_temp1593= _temp1591.f2; goto _LL1592; _LL1592: { int _temp1597; struct _tuple15
_temp1595=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1598: _temp1597= _temp1595.f2; goto _LL1596; _LL1596: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1589, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1589, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1590, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1599=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1599[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1600; _temp1600.tag=
Cyc_Absyn_Format_att; _temp1600.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1600.f2= _temp1593; _temp1600.f3= _temp1597; _temp1600;}); _temp1599;});}
else{ if( Cyc_Std_zstrcmp( _temp1590, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1601=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1601[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1602; _temp1602.tag=
Cyc_Absyn_Format_att; _temp1602.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1602.f2= _temp1593; _temp1602.f3= _temp1597; _temp1602;}); _temp1601;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1603=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1603[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1604; _temp1604.tag= Cyc_Attribute_tok; _temp1604.f1=(
void*) a; _temp1604;}); _temp1603;}); break;}}} case 52: _LL1588: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1606=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1606[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1607; _temp1607.tag= Cyc_TypeSpecifier_tok;
_temp1607.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1607;});
_temp1606;}); break; case 53: _LL1605: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1609=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1609[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1610; _temp1610.tag=
Cyc_TypeSpecifier_tok; _temp1610.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1610;}); _temp1609;}); break; case 54: _LL1608:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1612=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1612[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1613; _temp1613.tag= Cyc_TypeSpecifier_tok;
_temp1613.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1613;});
_temp1612;}); break; case 55: _LL1611: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1615=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1615[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1616; _temp1616.tag=
Cyc_TypeSpecifier_tok; _temp1616.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1617=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1617[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1618; _temp1618.tag=
Cyc_Parse_Short_spec; _temp1618.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1618;});
_temp1617;})); _temp1616;}); _temp1615;}); break; case 56: _LL1614: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1620=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1620[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1621; _temp1621.tag= Cyc_TypeSpecifier_tok;
_temp1621.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1621;});
_temp1620;}); break; case 57: _LL1619: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1623=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1623[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1624; _temp1624.tag=
Cyc_TypeSpecifier_tok; _temp1624.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1625=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1625[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1626; _temp1626.tag= Cyc_Parse_Long_spec;
_temp1626.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1626;}); _temp1625;})); _temp1624;}); _temp1623;});
break; case 58: _LL1622: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1628=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1628[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1629; _temp1629.tag=
Cyc_TypeSpecifier_tok; _temp1629.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1629;}); _temp1628;}); break; case 59: _LL1627:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1631=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1631[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1632; _temp1632.tag= Cyc_TypeSpecifier_tok;
_temp1632.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1632;});
_temp1631;}); break; case 60: _LL1630: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1634=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1634[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1635; _temp1635.tag=
Cyc_TypeSpecifier_tok; _temp1635.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1636=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1636[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1637; _temp1637.tag=
Cyc_Parse_Signed_spec; _temp1637.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1637;});
_temp1636;})); _temp1635;}); _temp1634;}); break; case 61: _LL1633: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1639=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1639[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1640; _temp1640.tag= Cyc_TypeSpecifier_tok;
_temp1640.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1641=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1641[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1642; _temp1642.tag= Cyc_Parse_Unsigned_spec; _temp1642.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1642;});
_temp1641;})); _temp1640;}); _temp1639;}); break; case 62: _LL1638: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1643:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1644: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1645: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1647=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1647[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1648; _temp1648.tag=
Cyc_TypeSpecifier_tok; _temp1648.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1648;});
_temp1647;}); break; case 66: _LL1646: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1650=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1650[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1651; _temp1651.tag=
Cyc_TypeSpecifier_tok; _temp1651.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1652=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1652[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1653; _temp1653.tag= Cyc_Absyn_TypedefType;
_temp1653.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1653.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1653.f3= 0;
_temp1653;}); _temp1652;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1651;});
_temp1650;}); break; case 67: _LL1649: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1655=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1655[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1656; _temp1656.tag=
Cyc_TypeSpecifier_tok; _temp1656.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1657=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1657[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1658; _temp1658.tag= Cyc_Absyn_TupleType;
_temp1658.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1658;});
_temp1657;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1656;}); _temp1655;}); break; case 68:
_LL1654: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1660=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1660[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1661; _temp1661.tag=
Cyc_TypeSpecifier_tok; _temp1661.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1662=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1662[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1663; _temp1663.tag= Cyc_Absyn_RgnHandleType;
_temp1663.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1663;}); _temp1662;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1661;}); _temp1660;}); break; case 69: _LL1659: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1665=( struct Cyc_Kind_tok_struct*) _cycalloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1665[ 0]=({ struct Cyc_Kind_tok_struct _temp1666; _temp1666.tag= Cyc_Kind_tok;
_temp1666.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1666;});
_temp1665;}); break; case 70: _LL1664: { struct _tagged_arr* _temp1670; void*
_temp1672; struct _tuple1 _temp1668=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1673: _temp1672=
_temp1668.f1; goto _LL1671; _LL1671: _temp1670= _temp1668.f2; goto _LL1669;
_LL1669: if( _temp1672 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1674=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1674[ 0]=({ struct Cyc_Kind_tok_struct
_temp1675; _temp1675.tag= Cyc_Kind_tok; _temp1675.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1670, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1675;}); _temp1674;}); break;} case 71:
_LL1667: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1677=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1677[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1678; _temp1678.tag= Cyc_TypeQual_tok;
_temp1678.f1=({ struct Cyc_Absyn_Tqual _temp1679; _temp1679.q_const= 1;
_temp1679.q_volatile= 0; _temp1679.q_restrict= 0; _temp1679;}); _temp1678;});
_temp1677;}); break; case 72: _LL1676: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1681=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1681[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1682; _temp1682.tag= Cyc_TypeQual_tok; _temp1682.f1=({ struct Cyc_Absyn_Tqual
_temp1683; _temp1683.q_const= 0; _temp1683.q_volatile= 1; _temp1683.q_restrict=
0; _temp1683;}); _temp1682;}); _temp1681;}); break; case 73: _LL1680: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1685=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1685[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1686; _temp1686.tag= Cyc_TypeQual_tok;
_temp1686.f1=({ struct Cyc_Absyn_Tqual _temp1687; _temp1687.q_const= 0;
_temp1687.q_volatile= 0; _temp1687.q_restrict= 1; _temp1687;}); _temp1686;});
_temp1685;}); break; case 74: _LL1684: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1689=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1689[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1690; _temp1690.tag=
Cyc_TypeSpecifier_tok; _temp1690.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1691=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1691[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1692; _temp1692.tag= Cyc_Parse_Decl_spec;
_temp1692.f1=({ struct Cyc_Absyn_Decl* _temp1693=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1693->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1694=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1694[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1695; _temp1695.tag= Cyc_Absyn_Enum_d; _temp1695.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1696=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1696->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1696->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1696->fields=({
struct Cyc_Core_Opt* _temp1697=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1697->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1697;});
_temp1696;}); _temp1695;}); _temp1694;})); _temp1693->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1693;}); _temp1692;}); _temp1691;})); _temp1690;}); _temp1689;}); break;
case 75: _LL1688: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1699=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1699[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1700; _temp1700.tag=
Cyc_TypeSpecifier_tok; _temp1700.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1701=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1701[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1702; _temp1702.tag= Cyc_Absyn_EnumType;
_temp1702.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1702.f2= 0; _temp1702;}); _temp1701;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1700;}); _temp1699;}); break; case 76: _LL1698: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1704=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1704[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1705; _temp1705.tag= Cyc_Enumfield_tok;
_temp1705.f1=({ struct Cyc_Absyn_Enumfield* _temp1706=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1706->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1706->tag= 0;
_temp1706->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1706;});
_temp1705;}); _temp1704;}); break; case 77: _LL1703: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1708=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1708[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1709; _temp1709.tag= Cyc_Enumfield_tok;
_temp1709.f1=({ struct Cyc_Absyn_Enumfield* _temp1710=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1710->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1710->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1710->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1710;});
_temp1709;}); _temp1708;}); break; case 78: _LL1707: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1712=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1712[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1713; _temp1713.tag=
Cyc_EnumfieldList_tok; _temp1713.f1=({ struct Cyc_List_List* _temp1714=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1714->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1714->tl= 0; _temp1714;}); _temp1713;}); _temp1712;});
break; case 79: _LL1711: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1716=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1716[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1717; _temp1717.tag=
Cyc_EnumfieldList_tok; _temp1717.f1=({ struct Cyc_List_List* _temp1718=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1718->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1718->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1718;}); _temp1717;});
_temp1716;}); break; case 80: _LL1715: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1720: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1722=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1722[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1723; _temp1723.tag=
Cyc_Absyn_AnonStructType; _temp1723.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1723;});
_temp1722;}); break; case Cyc_Parse_Union_su: _LL1721: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1725=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1725[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1726; _temp1726.tag= Cyc_Absyn_AnonUnionType; _temp1726.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1726;});
_temp1725;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1727=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1727[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1728; _temp1728.tag=
Cyc_TypeSpecifier_tok; _temp1728.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1729=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1729[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1730; _temp1730.tag= Cyc_Parse_Type_spec;
_temp1730.f1=( void*) t; _temp1730.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1730;});
_temp1729;})); _temp1728;}); _temp1727;}); break;} case 81: _LL1719: { struct
Cyc_List_List* _temp1732=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1733: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1735=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1735->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1735;}), _temp1732,({ struct Cyc_Core_Opt*
_temp1736=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1736->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1736;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1734: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1738=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1738->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1738;}),
_temp1732,({ struct Cyc_Core_Opt* _temp1739=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1739->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1739;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1740=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1740[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1741; _temp1741.tag=
Cyc_TypeSpecifier_tok; _temp1741.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1742=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1742[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1743; _temp1743.tag= Cyc_Parse_Decl_spec;
_temp1743.f1= d; _temp1743;}); _temp1742;})); _temp1741;}); _temp1740;}); break;}
case 82: _LL1731: { struct Cyc_List_List* _temp1745=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1746: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1748=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1748->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1748;}), _temp1745,({ struct Cyc_Core_Opt*
_temp1749=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1749->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1749;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1747: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1751=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1751->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1751;}),
_temp1745,({ struct Cyc_Core_Opt* _temp1752=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1752->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1752;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1753=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1753[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1754; _temp1754.tag=
Cyc_TypeSpecifier_tok; _temp1754.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1755=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1755[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1756; _temp1756.tag= Cyc_Parse_Decl_spec;
_temp1756.f1= d; _temp1756;}); _temp1755;})); _temp1754;}); _temp1753;}); break;}
case 83: _LL1744: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1758: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1760=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1760[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1761; _temp1761.tag=
Cyc_TypeSpecifier_tok; _temp1761.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1762=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1762[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1763; _temp1763.tag= Cyc_Absyn_StructType;
_temp1763.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1763.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1763.f3= 0;
_temp1763;}); _temp1762;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1761;});
_temp1760;}); break; case Cyc_Parse_Union_su: _LL1759: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1765=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1765[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1766; _temp1766.tag= Cyc_TypeSpecifier_tok;
_temp1766.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1767=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1767[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1768; _temp1768.tag= Cyc_Absyn_UnionType;
_temp1768.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1768.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1768.f3= 0;
_temp1768;}); _temp1767;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1766;});
_temp1765;}); break;} break; case 84: _LL1757: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1770: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1772=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1772[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1773; _temp1773.tag=
Cyc_TypeSpecifier_tok; _temp1773.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1774=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1774[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1775; _temp1775.tag= Cyc_Absyn_StructType;
_temp1775.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1775.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1775.f3= 0;
_temp1775;}); _temp1774;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1773;});
_temp1772;}); break; case Cyc_Parse_Union_su: _LL1771: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1777=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1777[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1778; _temp1778.tag= Cyc_TypeSpecifier_tok;
_temp1778.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1779=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1779[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1780; _temp1780.tag= Cyc_Absyn_UnionType;
_temp1780.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1780.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1780.f3= 0;
_temp1780;}); _temp1779;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1778;});
_temp1777;}); break;} break; case 85: _LL1769: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1782=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1782[ 0]=({ struct Cyc_TypeList_tok_struct _temp1783; _temp1783.tag= Cyc_TypeList_tok;
_temp1783.f1= 0; _temp1783;}); _temp1782;}); break; case 86: _LL1781: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1785=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1785[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1786; _temp1786.tag= Cyc_TypeList_tok; _temp1786.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1786;});
_temp1785;}); break; case 87: _LL1784:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1788=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1788[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1789; _temp1789.tag= Cyc_TypeList_tok; _temp1789.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1789;});
_temp1788;}); break; case 88: _LL1787: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1791=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1791[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1792; _temp1792.tag= Cyc_StructOrUnion_tok; _temp1792.f1= Cyc_Parse_Struct_su;
_temp1792;}); _temp1791;}); break; case 89: _LL1790: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1794=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1794[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1795; _temp1795.tag= Cyc_StructOrUnion_tok; _temp1795.f1= Cyc_Parse_Union_su;
_temp1795;}); _temp1794;}); break; case 90: _LL1793: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1797=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1797[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1798; _temp1798.tag= Cyc_StructFieldDeclList_tok; _temp1798.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1798;});
_temp1797;}); break; case 91: _LL1796: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1800=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1800[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1801; _temp1801.tag= Cyc_StructFieldDeclListList_tok; _temp1801.f1=({
struct Cyc_List_List* _temp1802=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1802->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1802->tl= 0;
_temp1802;}); _temp1801;}); _temp1800;}); break; case 92: _LL1799: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1804=( struct Cyc_StructFieldDeclListList_tok_struct*)
_cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1804[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1805; _temp1805.tag= Cyc_StructFieldDeclListList_tok;
_temp1805.f1=({ struct Cyc_List_List* _temp1806=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1806->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1806->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1806;}); _temp1805;}); _temp1804;}); break;
case 93: _LL1803: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1808=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1808[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1809; _temp1809.tag=
Cyc_InitDeclList_tok; _temp1809.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1809;});
_temp1808;}); break; case 94: _LL1807: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1811=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1811[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1812; _temp1812.tag=
Cyc_InitDeclList_tok; _temp1812.f1=({ struct Cyc_List_List* _temp1813=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1813->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1813->tl= 0; _temp1813;}); _temp1812;}); _temp1811;});
break; case 95: _LL1810: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1815=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1815[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1816; _temp1816.tag=
Cyc_InitDeclList_tok; _temp1816.f1=({ struct Cyc_List_List* _temp1817=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1817->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1817->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1817;});
_temp1816;}); _temp1815;}); break; case 96: _LL1814: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1819=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1819[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1820; _temp1820.tag= Cyc_InitDecl_tok;
_temp1820.f1=({ struct _tuple14* _temp1821=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1821->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1821->f2= 0;
_temp1821;}); _temp1820;}); _temp1819;}); break; case 97: _LL1818: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1823=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1823[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1824; _temp1824.tag= Cyc_InitDecl_tok; _temp1824.f1=({ struct _tuple14*
_temp1825=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1825->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1825->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1825;});
_temp1824;}); _temp1823;}); break; case 98: _LL1822: { struct _tuple17 _temp1829;
struct Cyc_List_List* _temp1830; struct Cyc_List_List* _temp1832; struct Cyc_Absyn_Tqual
_temp1834; struct _tuple17* _temp1827= Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1829=*
_temp1827; _LL1835: _temp1834= _temp1829.f1; goto _LL1833; _LL1833: _temp1832=
_temp1829.f2; goto _LL1831; _LL1831: _temp1830= _temp1829.f3; goto _LL1828;
_LL1828: { struct Cyc_List_List* _temp1838; struct Cyc_List_List* _temp1840;
struct _tuple0 _temp1836=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1841: _temp1840= _temp1836.f1; goto _LL1839; _LL1839:
_temp1838= _temp1836.f2; goto _LL1837; _LL1837: { void* _temp1842= Cyc_Parse_speclist2typ(
_temp1832, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1843=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1834, _temp1842, _temp1840, _temp1830), _temp1838);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1844=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1844[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1845; _temp1845.tag=
Cyc_StructFieldDeclList_tok; _temp1845.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1843); _temp1845;}); _temp1844;}); break;}}}
case 99: _LL1826: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1847=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1847[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1848; _temp1848.tag=
Cyc_QualSpecList_tok; _temp1848.f1=({ struct _tuple17* _temp1849=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1849->f1= Cyc_Absyn_empty_tqual();
_temp1849->f2=({ struct Cyc_List_List* _temp1850=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1850->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1850->tl=
0; _temp1850;}); _temp1849->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1849;}); _temp1848;});
_temp1847;}); break; case 100: _LL1846: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1852=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1852[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1853; _temp1853.tag=
Cyc_QualSpecList_tok; _temp1853.f1=({ struct _tuple17* _temp1854=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1854->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1854->f2=({
struct Cyc_List_List* _temp1855=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1855->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1855->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1855;}); _temp1854->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1854;});
_temp1853;}); _temp1852;}); break; case 101: _LL1851: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1857=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1857[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1858; _temp1858.tag= Cyc_QualSpecList_tok;
_temp1858.f1=({ struct _tuple17* _temp1859=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1859->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1859->f2= 0;
_temp1859->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1859;}); _temp1858;}); _temp1857;}); break; case
102: _LL1856: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1861=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1861[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1862; _temp1862.tag=
Cyc_QualSpecList_tok; _temp1862.f1=({ struct _tuple17* _temp1863=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1863->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1863->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1863->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1863;});
_temp1862;}); _temp1861;}); break; case 103: _LL1860: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1865=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1865[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1866; _temp1866.tag= Cyc_InitDeclList_tok;
_temp1866.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1866;}); _temp1865;}); break; case 104: _LL1864: yyval=(
void*)({ struct Cyc_InitDeclList_tok_struct* _temp1868=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1868[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1869; _temp1869.tag= Cyc_InitDeclList_tok;
_temp1869.f1=({ struct Cyc_List_List* _temp1870=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1870->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1870->tl= 0;
_temp1870;}); _temp1869;}); _temp1868;}); break; case 105: _LL1867: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp1872=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1872[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1873; _temp1873.tag= Cyc_InitDeclList_tok;
_temp1873.f1=({ struct Cyc_List_List* _temp1874=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1874->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1874->tl=
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1874;}); _temp1873;}); _temp1872;}); break; case 106:
_LL1871: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp1876=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1876[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1877; _temp1877.tag= Cyc_InitDecl_tok; _temp1877.f1=({ struct _tuple14*
_temp1878=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1878->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1878->f2= 0; _temp1878;}); _temp1877;}); _temp1876;});
break; case 107: _LL1875: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1880=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1880[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1881; _temp1881.tag= Cyc_InitDecl_tok;
_temp1881.f1=({ struct _tuple14* _temp1882=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1882->f1=({ struct Cyc_Parse_Declarator*
_temp1883=( struct Cyc_Parse_Declarator*) _cycalloc( sizeof( struct Cyc_Parse_Declarator));
_temp1883->id=({ struct _tuple1* _temp1884=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp1884->f1= Cyc_Absyn_rel_ns_null; _temp1884->f2=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1884;}); _temp1883->tms= 0; _temp1883;});
_temp1882->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1882;}); _temp1881;});
_temp1880;}); break; case 108: _LL1879: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1886=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1886[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1887; _temp1887.tag= Cyc_InitDecl_tok;
_temp1887.f1=({ struct _tuple14* _temp1888=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1888->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1888->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1888;}); _temp1887;}); _temp1886;}); break; case
109: _LL1885: { struct Cyc_List_List* _temp1890=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1891=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1891[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1892; _temp1892.tag=
Cyc_TypeSpecifier_tok; _temp1892.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1893=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1893[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1894; _temp1894.tag= Cyc_Parse_Decl_spec;
_temp1894.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1890,({
struct Cyc_Core_Opt* _temp1895=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1895->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1895;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1894;}); _temp1893;})); _temp1892;}); _temp1891;}); break;} case 110:
_LL1889: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1897=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1897[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1898; _temp1898.tag=
Cyc_TypeSpecifier_tok; _temp1898.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1899=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1899[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1900; _temp1900.tag= Cyc_Absyn_TunionType;
_temp1900.f1=({ struct Cyc_Absyn_TunionInfo _temp1901; _temp1901.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1902=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1902[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1903; _temp1903.tag= Cyc_Absyn_UnknownTunion;
_temp1903.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1904; _temp1904.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1904.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1904;});
_temp1903;}); _temp1902;})); _temp1901.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1901.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1901;}); _temp1900;}); _temp1899;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1898;}); _temp1897;}); break; case 111: _LL1896: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1906=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1906[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1907; _temp1907.tag= Cyc_TypeSpecifier_tok;
_temp1907.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1908=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1908[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1909; _temp1909.tag=
Cyc_Absyn_TunionType; _temp1909.f1=({ struct Cyc_Absyn_TunionInfo _temp1910;
_temp1910.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1911=( struct Cyc_Absyn_UnknownTunion_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1911[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1912; _temp1912.tag= Cyc_Absyn_UnknownTunion; _temp1912.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1913; _temp1913.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1913.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1913;}); _temp1912;}); _temp1911;})); _temp1910.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1910.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp1910;}); _temp1909;}); _temp1908;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1907;}); _temp1906;}); break; case 112:
_LL1905: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1915=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1915[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1916; _temp1916.tag=
Cyc_TypeSpecifier_tok; _temp1916.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1917=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1917[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1918; _temp1918.tag= Cyc_Absyn_TunionFieldType;
_temp1918.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1919; _temp1919.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp1920=( struct
Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
_temp1920[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct _temp1921; _temp1921.tag=
Cyc_Absyn_UnknownTunionfield; _temp1921.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp1922; _temp1922.tunion_name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1922.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1922.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1922;});
_temp1921;}); _temp1920;})); _temp1919.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1919;}); _temp1918;});
_temp1917;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1916;}); _temp1915;}); break; case 113:
_LL1914: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1924=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1924[ 0]=({ struct
Cyc_Bool_tok_struct _temp1925; _temp1925.tag= Cyc_Bool_tok; _temp1925.f1= 0;
_temp1925;}); _temp1924;}); break; case 114: _LL1923: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1927=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1927[ 0]=({ struct Cyc_Bool_tok_struct
_temp1928; _temp1928.tag= Cyc_Bool_tok; _temp1928.f1= 1; _temp1928;}); _temp1927;});
break; case 115: _LL1926: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1930=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1930[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1931; _temp1931.tag= Cyc_TunionFieldList_tok; _temp1931.f1=({ struct Cyc_List_List*
_temp1932=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1932->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1932->tl= 0;
_temp1932;}); _temp1931;}); _temp1930;}); break; case 116: _LL1929: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp1934=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1934[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1935; _temp1935.tag= Cyc_TunionFieldList_tok;
_temp1935.f1=({ struct Cyc_List_List* _temp1936=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1936->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1936->tl=
0; _temp1936;}); _temp1935;}); _temp1934;}); break; case 117: _LL1933: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1938=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1938[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1939; _temp1939.tag= Cyc_TunionFieldList_tok;
_temp1939.f1=({ struct Cyc_List_List* _temp1940=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1940->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1940->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1940;}); _temp1939;}); _temp1938;}); break; case 118:
_LL1937: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1942=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp1942[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp1943; _temp1943.tag=
Cyc_TunionFieldList_tok; _temp1943.f1=({ struct Cyc_List_List* _temp1944=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1944->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1944->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1944;}); _temp1943;});
_temp1942;}); break; case 119: _LL1941: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1946=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1946[ 0]=({ struct Cyc_Scope_tok_struct _temp1947; _temp1947.tag= Cyc_Scope_tok;
_temp1947.f1=( void*)(( void*) Cyc_Absyn_Public); _temp1947;}); _temp1946;});
break; case 120: _LL1945: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1949=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1949[ 0]=({ struct Cyc_Scope_tok_struct _temp1950; _temp1950.tag= Cyc_Scope_tok;
_temp1950.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp1950;}); _temp1949;});
break; case 121: _LL1948: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1952=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1952[ 0]=({ struct Cyc_Scope_tok_struct _temp1953; _temp1953.tag= Cyc_Scope_tok;
_temp1953.f1=( void*)(( void*) Cyc_Absyn_Static); _temp1953;}); _temp1952;});
break; case 122: _LL1951: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp1955=( struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp1955[ 0]=({ struct Cyc_TunionField_tok_struct _temp1956; _temp1956.tag= Cyc_TunionField_tok;
_temp1956.f1=({ struct Cyc_Absyn_Tunionfield* _temp1957=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1957->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1957->tvs= 0;
_temp1957->typs= 0; _temp1957->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1957->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1957;}); _temp1956;}); _temp1955;}); break; case 123:
_LL1954: { struct Cyc_List_List* _temp1959=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp1960=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp1961=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp1961[ 0]=({ struct
Cyc_TunionField_tok_struct _temp1962; _temp1962.tag= Cyc_TunionField_tok;
_temp1962.f1=({ struct Cyc_Absyn_Tunionfield* _temp1963=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1963->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1963->tvs=
_temp1960; _temp1963->typs= _temp1959; _temp1963->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1963->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp1963;}); _temp1962;}); _temp1961;}); break;}
case 124: _LL1958: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 125: _LL1964: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1966=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1966[ 0]=({ struct Cyc_Declarator_tok_struct _temp1967; _temp1967.tag= Cyc_Declarator_tok;
_temp1967.f1=({ struct Cyc_Parse_Declarator* _temp1968=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1968->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp1968->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp1968;});
_temp1967;}); _temp1966;}); break; case 126: _LL1965: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp1970=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp1970[ 0]=({ struct
Cyc_Declarator_tok_struct _temp1971; _temp1971.tag= Cyc_Declarator_tok;
_temp1971.f1=({ struct Cyc_Parse_Declarator* _temp1972=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1972->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1972->tms= 0;
_temp1972;}); _temp1971;}); _temp1970;}); break; case 127: _LL1969: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 128:
_LL1973: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp1975=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1975[ 0]=({ struct Cyc_Declarator_tok_struct _temp1976; _temp1976.tag= Cyc_Declarator_tok;
_temp1976.f1=({ struct Cyc_Parse_Declarator* _temp1977=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1977->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp1977->tms=({ struct Cyc_List_List* _temp1978=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1978->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp1978->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp1978;}); _temp1977;}); _temp1976;});
_temp1975;}); break; case 129: _LL1974: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1980=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1980[ 0]=({ struct Cyc_Declarator_tok_struct _temp1981; _temp1981.tag= Cyc_Declarator_tok;
_temp1981.f1=({ struct Cyc_Parse_Declarator* _temp1982=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1982->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp1982->tms=({ struct Cyc_List_List* _temp1983=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1983->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp1984=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1984[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1985; _temp1985.tag= Cyc_Absyn_ConstArray_mod;
_temp1985.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1985;}); _temp1984;})); _temp1983->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp1983;}); _temp1982;}); _temp1981;}); _temp1980;}); break; case 130: _LL1979: {
struct _tuple16 _temp1989; struct Cyc_List_List* _temp1990; struct Cyc_Core_Opt*
_temp1992; struct Cyc_Absyn_VarargInfo* _temp1994; int _temp1996; struct Cyc_List_List*
_temp1998; struct _tuple16* _temp1987= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1989=*
_temp1987; _LL1999: _temp1998= _temp1989.f1; goto _LL1997; _LL1997: _temp1996=
_temp1989.f2; goto _LL1995; _LL1995: _temp1994= _temp1989.f3; goto _LL1993;
_LL1993: _temp1992= _temp1989.f4; goto _LL1991; _LL1991: _temp1990= _temp1989.f5;
goto _LL1988; _LL1988: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2000=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2000[ 0]=({ struct Cyc_Declarator_tok_struct _temp2001; _temp2001.tag= Cyc_Declarator_tok;
_temp2001.f1=({ struct Cyc_Parse_Declarator* _temp2002=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2002->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2002->tms=({ struct Cyc_List_List* _temp2003=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2003->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2004=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2004[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2005; _temp2005.tag= Cyc_Absyn_Function_mod;
_temp2005.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2006=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2006[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2007; _temp2007.tag= Cyc_Absyn_WithTypes;
_temp2007.f1= _temp1998; _temp2007.f2= _temp1996; _temp2007.f3= _temp1994;
_temp2007.f4= _temp1992; _temp2007.f5= _temp1990; _temp2007;}); _temp2006;}));
_temp2005;}); _temp2004;})); _temp2003->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2003;});
_temp2002;}); _temp2001;}); _temp2000;}); break;} case 131: _LL1986: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2009=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2009[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2010; _temp2010.tag= Cyc_Declarator_tok;
_temp2010.f1=({ struct Cyc_Parse_Declarator* _temp2011=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2011->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2011->tms=({ struct Cyc_List_List* _temp2012=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2012->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2013=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2013[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2014; _temp2014.tag= Cyc_Absyn_Function_mod;
_temp2014.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2015=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2015[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2016; _temp2016.tag= Cyc_Absyn_WithTypes;
_temp2016.f1= 0; _temp2016.f2= 0; _temp2016.f3= 0; _temp2016.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2016.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2016;}); _temp2015;})); _temp2014;}); _temp2013;}));
_temp2012->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2012;}); _temp2011;}); _temp2010;});
_temp2009;}); break; case 132: _LL2008: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2018=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2018[ 0]=({ struct Cyc_Declarator_tok_struct _temp2019; _temp2019.tag= Cyc_Declarator_tok;
_temp2019.f1=({ struct Cyc_Parse_Declarator* _temp2020=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2020->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2020->tms=({ struct Cyc_List_List* _temp2021=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2021->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2022=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2022[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2023; _temp2023.tag= Cyc_Absyn_Function_mod;
_temp2023.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2024=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2024[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2025; _temp2025.tag= Cyc_Absyn_NoTypes;
_temp2025.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2025.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2025;});
_temp2024;})); _temp2023;}); _temp2022;})); _temp2021->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2021;}); _temp2020;}); _temp2019;}); _temp2018;}); break; case 133: _LL2017: {
struct Cyc_List_List* _temp2027=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2028=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2028[ 0]=({ struct Cyc_Declarator_tok_struct _temp2029; _temp2029.tag= Cyc_Declarator_tok;
_temp2029.f1=({ struct Cyc_Parse_Declarator* _temp2030=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2030->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2030->tms=({ struct Cyc_List_List* _temp2031=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2031->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2032=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2032[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2033; _temp2033.tag= Cyc_Absyn_TypeParams_mod;
_temp2033.f1= _temp2027; _temp2033.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2033.f3=
0; _temp2033;}); _temp2032;})); _temp2031->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2031;});
_temp2030;}); _temp2029;}); _temp2028;}); break;} case 134: _LL2026:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2035=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2036=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2036[ 0]=({ struct Cyc_Declarator_tok_struct _temp2037; _temp2037.tag= Cyc_Declarator_tok;
_temp2037.f1=({ struct Cyc_Parse_Declarator* _temp2038=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2038->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2038->tms=({ struct Cyc_List_List* _temp2039=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2039->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2040=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2040[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2041; _temp2041.tag= Cyc_Absyn_TypeParams_mod;
_temp2041.f1= _temp2035; _temp2041.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2041.f3=
0; _temp2041;}); _temp2040;})); _temp2039->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2039;});
_temp2038;}); _temp2037;}); _temp2036;}); break;} case 135: _LL2034: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2043=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2043[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2044; _temp2044.tag= Cyc_Declarator_tok;
_temp2044.f1=({ struct Cyc_Parse_Declarator* _temp2045=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2045->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2045->tms=({ struct Cyc_List_List* _temp2046=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2046->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2047=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2047[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2048; _temp2048.tag= Cyc_Absyn_Attributes_mod;
_temp2048.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2048.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2048;}); _temp2047;}));
_temp2046->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]))->tms; _temp2046;}); _temp2045;}); _temp2044;});
_temp2043;}); break; case 136: _LL2042: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2050=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2050[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2051; _temp2051.tag= Cyc_Declarator_tok;
_temp2051.f1=({ struct Cyc_Parse_Declarator* _temp2052=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2052->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2052->tms= 0;
_temp2052;}); _temp2051;}); _temp2050;}); break; case 137: _LL2049: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2054=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2054[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2055; _temp2055.tag= Cyc_Declarator_tok;
_temp2055.f1=({ struct Cyc_Parse_Declarator* _temp2056=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2056->id= Cyc_Absyn_exn_name;
_temp2056->tms= 0; _temp2056;}); _temp2055;}); _temp2054;}); break; case 138:
_LL2053: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2058=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2058[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2059; _temp2059.tag=
Cyc_TypeModifierList_tok; _temp2059.f1=({ struct Cyc_List_List* _temp2060=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2060->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2061=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2061[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2062; _temp2062.tag= Cyc_Absyn_Pointer_mod;
_temp2062.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2062.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2062.f3= Cyc_Absyn_empty_tqual(); _temp2062;});
_temp2061;})); _temp2060->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2060;});
_temp2059;}); _temp2058;}); break; case 139: _LL2057: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2064=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2064[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2065; _temp2065.tag= Cyc_TypeModifierList_tok;
_temp2065.f1=({ struct Cyc_List_List* _temp2066=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2066->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2067=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2067[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2068; _temp2068.tag= Cyc_Absyn_Pointer_mod;
_temp2068.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2068.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2068.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2068;}); _temp2067;}));
_temp2066->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0);
_temp2066;}); _temp2065;}); _temp2064;}); break; case 140: _LL2063: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2070=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2070[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2071; _temp2071.tag= Cyc_TypeModifierList_tok;
_temp2071.f1=({ struct Cyc_List_List* _temp2072=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2072->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2073=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2073[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2074; _temp2074.tag= Cyc_Absyn_Pointer_mod;
_temp2074.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2074.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2074.f3= Cyc_Absyn_empty_tqual(); _temp2074;});
_temp2073;})); _temp2072->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2072;});
_temp2071;}); _temp2070;}); break; case 141: _LL2069: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2076=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2076[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2077; _temp2077.tag= Cyc_TypeModifierList_tok;
_temp2077.f1=({ struct Cyc_List_List* _temp2078=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2078->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2079=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2079[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2080; _temp2080.tag= Cyc_Absyn_Pointer_mod;
_temp2080.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2080.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2080.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2080;});
_temp2079;})); _temp2078->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2078;});
_temp2077;}); _temp2076;}); break; case 142: _LL2075: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2082=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2082[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2083; _temp2083.tag= Cyc_Pointer_Sort_tok;
_temp2083.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2084=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2084[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2085; _temp2085.tag=
Cyc_Absyn_Nullable_ps; _temp2085.f1= Cyc_Absyn_exp_unsigned_one; _temp2085;});
_temp2084;})); _temp2083;}); _temp2082;}); break; case 143: _LL2081: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2087=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2087[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2088; _temp2088.tag= Cyc_Pointer_Sort_tok;
_temp2088.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2089=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2089[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2090; _temp2090.tag= Cyc_Absyn_NonNullable_ps; _temp2090.f1= Cyc_Absyn_exp_unsigned_one;
_temp2090;}); _temp2089;})); _temp2088;}); _temp2087;}); break; case 144:
_LL2086: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2092=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2092[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2093; _temp2093.tag=
Cyc_Pointer_Sort_tok; _temp2093.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2094=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2094[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2095; _temp2095.tag=
Cyc_Absyn_Nullable_ps; _temp2095.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2095;});
_temp2094;})); _temp2093;}); _temp2092;}); break; case 145: _LL2091: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2097=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2097[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2098; _temp2098.tag= Cyc_Pointer_Sort_tok;
_temp2098.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2099=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2099[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2100; _temp2100.tag= Cyc_Absyn_NonNullable_ps; _temp2100.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2100;});
_temp2099;})); _temp2098;}); _temp2097;}); break; case 146: _LL2096: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2102=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2102[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2103; _temp2103.tag= Cyc_Pointer_Sort_tok;
_temp2103.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2103;});
_temp2102;}); break; case 147: _LL2101: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2105=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2105[ 0]=({ struct Cyc_Type_tok_struct _temp2106; _temp2106.tag= Cyc_Type_tok;
_temp2106.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2107=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2107->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2107;}), 0); _temp2106;}); _temp2105;});
break; case 148: _LL2104: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 149: _LL2108: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
150: _LL2109: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2111=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2111[
0]=({ struct Cyc_Type_tok_struct _temp2112; _temp2112.tag= Cyc_Type_tok;
_temp2112.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2113=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2113->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2113;}), 0); _temp2112;}); _temp2111;});
break; case 151: _LL2110: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 152: _LL2114: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2116=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2116[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2117; _temp2117.tag= Cyc_TypeQual_tok; _temp2117.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2117;});
_temp2116;}); break; case 153: _LL2115: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2119=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2119[ 0]=({ struct Cyc_YY1_struct _temp2120; _temp2120.tag= Cyc_YY1;
_temp2120.f1=({ struct _tuple16* _temp2121=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2121->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2121->f2= 0;
_temp2121->f3= 0; _temp2121->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2121->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2121;});
_temp2120;}); _temp2119;}); break; case 154: _LL2118: yyval=( void*)({ struct
Cyc_YY1_struct* _temp2123=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct
Cyc_YY1_struct)); _temp2123[ 0]=({ struct Cyc_YY1_struct _temp2124; _temp2124.tag=
Cyc_YY1; _temp2124.f1=({ struct _tuple16* _temp2125=( struct _tuple16*)
_cycalloc( sizeof( struct _tuple16)); _temp2125->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2125->f2= 1;
_temp2125->f3= 0; _temp2125->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2125->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2125;});
_temp2124;}); _temp2123;}); break; case 155: _LL2122: { struct _tuple2 _temp2129;
void* _temp2130; struct Cyc_Absyn_Tqual _temp2132; struct Cyc_Core_Opt*
_temp2134; struct _tuple2* _temp2127= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2129=*
_temp2127; _LL2135: _temp2134= _temp2129.f1; goto _LL2133; _LL2133: _temp2132=
_temp2129.f2; goto _LL2131; _LL2131: _temp2130= _temp2129.f3; goto _LL2128;
_LL2128: { struct Cyc_Absyn_VarargInfo* _temp2136=({ struct Cyc_Absyn_VarargInfo*
_temp2140=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2140->name= _temp2134; _temp2140->tq= _temp2132; _temp2140->type=( void*)
_temp2130; _temp2140->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2140;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2137=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2137[ 0]=({ struct Cyc_YY1_struct
_temp2138; _temp2138.tag= Cyc_YY1; _temp2138.f1=({ struct _tuple16* _temp2139=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2139->f1= 0;
_temp2139->f2= 0; _temp2139->f3= _temp2136; _temp2139->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2139->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2139;}); _temp2138;}); _temp2137;}); break;}} case 156:
_LL2126: { struct _tuple2 _temp2144; void* _temp2145; struct Cyc_Absyn_Tqual
_temp2147; struct Cyc_Core_Opt* _temp2149; struct _tuple2* _temp2142= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2144=*
_temp2142; _LL2150: _temp2149= _temp2144.f1; goto _LL2148; _LL2148: _temp2147=
_temp2144.f2; goto _LL2146; _LL2146: _temp2145= _temp2144.f3; goto _LL2143;
_LL2143: { struct Cyc_Absyn_VarargInfo* _temp2151=({ struct Cyc_Absyn_VarargInfo*
_temp2155=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2155->name= _temp2149; _temp2155->tq= _temp2147; _temp2155->type=( void*)
_temp2145; _temp2155->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2155;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2152=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2152[ 0]=({ struct Cyc_YY1_struct
_temp2153; _temp2153.tag= Cyc_YY1; _temp2153.f1=({ struct _tuple16* _temp2154=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2154->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2154->f2=
0; _temp2154->f3= _temp2151; _temp2154->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2154->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2154;});
_temp2153;}); _temp2152;}); break;}} case 157: _LL2141: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2157=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2157[ 0]=({ struct Cyc_Type_tok_struct
_temp2158; _temp2158.tag= Cyc_Type_tok; _temp2158.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()); _temp2158;}); _temp2157;}); break; case 158: _LL2156:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2160=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2160[ 0]=({ struct Cyc_Type_tok_struct
_temp2161; _temp2161.tag= Cyc_Type_tok; _temp2161.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp2161;});
_temp2160;}); break; case 159: _LL2159: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2163=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2163[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2164; _temp2164.tag= Cyc_TypeOpt_tok;
_temp2164.f1= 0; _temp2164;}); _temp2163;}); break; case 160: _LL2162: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2166=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2166[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2167; _temp2167.tag= Cyc_TypeOpt_tok; _temp2167.f1=({ struct Cyc_Core_Opt*
_temp2168=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2168->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2169=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2169[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2170; _temp2170.tag= Cyc_Absyn_JoinEff;
_temp2170.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2170;}); _temp2169;})); _temp2168;}); _temp2167;});
_temp2166;}); break; case 161: _LL2165: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2172=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2172[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2173; _temp2173.tag= Cyc_Rgnorder_tok;
_temp2173.f1= 0; _temp2173;}); _temp2172;}); break; case 162: _LL2171: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL2174: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2176=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2176[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2177; _temp2177.tag= Cyc_Rgnorder_tok; _temp2177.f1=({ struct Cyc_List_List*
_temp2178=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2178->hd=( void*)({ struct _tuple18* _temp2179=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2179->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2179->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2179;}); _temp2178->tl= 0; _temp2178;});
_temp2177;}); _temp2176;}); break; case 164: _LL2175: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2181=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2181[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2182; _temp2182.tag= Cyc_Rgnorder_tok; _temp2182.f1=({ struct Cyc_List_List*
_temp2183=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2183->hd=( void*)({ struct _tuple18* _temp2184=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2184->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2184->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2184;}); _temp2183->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2183;});
_temp2182;}); _temp2181;}); break; case 165: _LL2180: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2186=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2186[ 0]=({ struct Cyc_Bool_tok_struct
_temp2187; _temp2187.tag= Cyc_Bool_tok; _temp2187.f1= 0; _temp2187;}); _temp2186;});
break; case 166: _LL2185: { struct _tagged_arr _temp2189= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Std_zstrcmp(
_temp2189, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2190=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2190[ 0]=({ struct Cyc_Bool_tok_struct _temp2191; _temp2191.tag= Cyc_Bool_tok;
_temp2191.f1= 1; _temp2191;}); _temp2190;}); break;} case 167: _LL2188: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168:
_LL2192: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2194=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2194[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2195; _temp2195.tag= Cyc_TypeList_tok; _temp2195.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2195;});
_temp2194;}); break; case 169: _LL2193: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2197=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2197[ 0]=({ struct Cyc_TypeList_tok_struct _temp2198; _temp2198.tag= Cyc_TypeList_tok;
_temp2198.f1= 0; _temp2198;}); _temp2197;}); break; case 170: _LL2196: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case
171: _LL2199: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2201=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2201[ 0]=({ struct Cyc_TypeList_tok_struct _temp2202; _temp2202.tag= Cyc_TypeList_tok;
_temp2202.f1=({ struct Cyc_List_List* _temp2203=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2203->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp2204=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp2204[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp2205; _temp2205.tag= Cyc_Absyn_RgnsEff; _temp2205.f1=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2205;}); _temp2204;})); _temp2203->tl= 0; _temp2203;});
_temp2202;}); _temp2201;}); break; case 172: _LL2200: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2207=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2207[ 0]=({ struct Cyc_TypeList_tok_struct _temp2208; _temp2208.tag= Cyc_TypeList_tok;
_temp2208.f1=({ struct Cyc_List_List* _temp2209=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2209->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2209->tl= 0;
_temp2209;}); _temp2208;}); _temp2207;}); break; case 173: _LL2206: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2211=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2211[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2212; _temp2212.tag= Cyc_TypeList_tok; _temp2212.f1=({ struct Cyc_List_List*
_temp2213=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2213->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2214=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2214[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2215; _temp2215.tag= Cyc_Absyn_AccessEff;
_temp2215.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2215;}); _temp2214;})); _temp2213->tl= 0;
_temp2213;}); _temp2212;}); _temp2211;}); break; case 174: _LL2210: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),( void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2217=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2217[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2218; _temp2218.tag= Cyc_TypeList_tok; _temp2218.f1=({ struct Cyc_List_List*
_temp2219=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2219->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2220=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2220[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2221; _temp2221.tag= Cyc_Absyn_AccessEff;
_temp2221.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2221;}); _temp2220;})); _temp2219->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2219;});
_temp2218;}); _temp2217;}); break; case 175: _LL2216: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2223=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2223[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2224; _temp2224.tag= Cyc_ParamDeclList_tok;
_temp2224.f1=({ struct Cyc_List_List* _temp2225=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2225->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2225->tl= 0;
_temp2225;}); _temp2224;}); _temp2223;}); break; case 176: _LL2222: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2227=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2227[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2228; _temp2228.tag= Cyc_ParamDeclList_tok;
_temp2228.f1=({ struct Cyc_List_List* _temp2229=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2229->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2229->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2229;}); _temp2228;}); _temp2227;}); break; case 177:
_LL2226: { struct _tuple17 _temp2233; struct Cyc_List_List* _temp2234; struct
Cyc_List_List* _temp2236; struct Cyc_Absyn_Tqual _temp2238; struct _tuple17*
_temp2231= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2233=* _temp2231; _LL2239: _temp2238=
_temp2233.f1; goto _LL2237; _LL2237: _temp2236= _temp2233.f2; goto _LL2235;
_LL2235: _temp2234= _temp2233.f3; goto _LL2232; _LL2232: { struct Cyc_Parse_Declarator
_temp2242; struct Cyc_List_List* _temp2243; struct _tuple1* _temp2245; struct
Cyc_Parse_Declarator* _temp2240= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2242=* _temp2240;
_LL2246: _temp2245= _temp2242.id; goto _LL2244; _LL2244: _temp2243= _temp2242.tms;
goto _LL2241; _LL2241: { void* _temp2247= Cyc_Parse_speclist2typ( _temp2236, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2250; struct Cyc_List_List* _temp2252; void*
_temp2254; struct Cyc_Absyn_Tqual _temp2256; struct _tuple6 _temp2248= Cyc_Parse_apply_tms(
_temp2238, _temp2247, _temp2234, _temp2243); _LL2257: _temp2256= _temp2248.f1;
goto _LL2255; _LL2255: _temp2254= _temp2248.f2; goto _LL2253; _LL2253: _temp2252=
_temp2248.f3; goto _LL2251; _LL2251: _temp2250= _temp2248.f4; goto _LL2249;
_LL2249: if( _temp2252 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2245)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2258=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2262=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2262->v=( void*)(* _temp2245).f2; _temp2262;}); if( _temp2250 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2259=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2259[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2260; _temp2260.tag= Cyc_ParamDecl_tok;
_temp2260.f1=({ struct _tuple2* _temp2261=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2261->f1= _temp2258; _temp2261->f2= _temp2256; _temp2261->f3=
_temp2254; _temp2261;}); _temp2260;}); _temp2259;}); break;}}}} case 178:
_LL2230: { struct _tuple17 _temp2266; struct Cyc_List_List* _temp2267; struct
Cyc_List_List* _temp2269; struct Cyc_Absyn_Tqual _temp2271; struct _tuple17*
_temp2264= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2266=* _temp2264; _LL2272: _temp2271= _temp2266.f1;
goto _LL2270; _LL2270: _temp2269= _temp2266.f2; goto _LL2268; _LL2268: _temp2267=
_temp2266.f3; goto _LL2265; _LL2265: { void* _temp2273= Cyc_Parse_speclist2typ(
_temp2269, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2267 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2274=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2274[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2275; _temp2275.tag= Cyc_ParamDecl_tok; _temp2275.f1=({
struct _tuple2* _temp2276=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2276->f1= 0; _temp2276->f2= _temp2271; _temp2276->f3= _temp2273; _temp2276;});
_temp2275;}); _temp2274;}); break;}} case 179: _LL2263: { struct _tuple17
_temp2280; struct Cyc_List_List* _temp2281; struct Cyc_List_List* _temp2283;
struct Cyc_Absyn_Tqual _temp2285; struct _tuple17* _temp2278= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2280=*
_temp2278; _LL2286: _temp2285= _temp2280.f1; goto _LL2284; _LL2284: _temp2283=
_temp2280.f2; goto _LL2282; _LL2282: _temp2281= _temp2280.f3; goto _LL2279;
_LL2279: { void* _temp2287= Cyc_Parse_speclist2typ( _temp2283, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2288=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2291; struct Cyc_List_List* _temp2293; void* _temp2295; struct Cyc_Absyn_Tqual
_temp2297; struct _tuple6 _temp2289= Cyc_Parse_apply_tms( _temp2285, _temp2287,
_temp2281, _temp2288); _LL2298: _temp2297= _temp2289.f1; goto _LL2296; _LL2296:
_temp2295= _temp2289.f2; goto _LL2294; _LL2294: _temp2293= _temp2289.f3; goto
_LL2292; _LL2292: _temp2291= _temp2289.f4; goto _LL2290; _LL2290: if( _temp2293
!=  0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2291 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2299=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2299[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2300; _temp2300.tag= Cyc_ParamDecl_tok; _temp2300.f1=({
struct _tuple2* _temp2301=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2301->f1= 0; _temp2301->f2= _temp2297; _temp2301->f3= _temp2295; _temp2301;});
_temp2300;}); _temp2299;}); break;}} case 180: _LL2277: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2303=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2303[ 0]=({ struct Cyc_IdList_tok_struct
_temp2304; _temp2304.tag= Cyc_IdList_tok; _temp2304.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2304;});
_temp2303;}); break; case 181: _LL2302: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2306=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2306[ 0]=({ struct Cyc_IdList_tok_struct _temp2307; _temp2307.tag= Cyc_IdList_tok;
_temp2307.f1=({ struct Cyc_List_List* _temp2308=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2308->hd=( void*)({ struct
_tagged_arr* _temp2309=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2309[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2309;}); _temp2308->tl=
0; _temp2308;}); _temp2307;}); _temp2306;}); break; case 182: _LL2305: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2311=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2311[ 0]=({ struct Cyc_IdList_tok_struct
_temp2312; _temp2312.tag= Cyc_IdList_tok; _temp2312.f1=({ struct Cyc_List_List*
_temp2313=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2313->hd=( void*)({ struct _tagged_arr* _temp2314=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2314[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2314;});
_temp2313->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2313;}); _temp2312;}); _temp2311;}); break; case 183:
_LL2310: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 184: _LL2315: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 185: _LL2316: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2318=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2318[ 0]=({ struct Cyc_Exp_tok_struct _temp2319; _temp2319.tag= Cyc_Exp_tok;
_temp2319.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2320=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2320[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2321; _temp2321.tag= Cyc_Absyn_UnresolvedMem_e; _temp2321.f1= 0; _temp2321.f2=
0; _temp2321;}); _temp2320;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2319;});
_temp2318;}); break; case 186: _LL2317: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2323=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2323[ 0]=({ struct Cyc_Exp_tok_struct _temp2324; _temp2324.tag= Cyc_Exp_tok;
_temp2324.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2325=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2325[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2326; _temp2326.tag= Cyc_Absyn_UnresolvedMem_e; _temp2326.f1= 0; _temp2326.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2326;});
_temp2325;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2324;}); _temp2323;}); break; case 187:
_LL2322: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2328=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2328[ 0]=({ struct Cyc_Exp_tok_struct
_temp2329; _temp2329.tag= Cyc_Exp_tok; _temp2329.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2330=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2330[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2331; _temp2331.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2331.f1= 0; _temp2331.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2331;});
_temp2330;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2329;}); _temp2328;}); break; case 188:
_LL2327: { struct Cyc_Absyn_Vardecl* _temp2333= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2339=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2339->f1=( void*) Cyc_Absyn_Loc_n; _temp2339->f2=({ struct _tagged_arr*
_temp2340=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp2340[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); _temp2340;}); _temp2339;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp2333->tq=({ struct Cyc_Absyn_Tqual _temp2334; _temp2334.q_const= 1;
_temp2334.q_volatile= 0; _temp2334.q_restrict= 1; _temp2334;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2335=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2335[ 0]=({ struct Cyc_Exp_tok_struct
_temp2336; _temp2336.tag= Cyc_Exp_tok; _temp2336.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2337=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2337[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2338; _temp2338.tag= Cyc_Absyn_Comprehension_e;
_temp2338.f1= _temp2333; _temp2338.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2338.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2338;});
_temp2337;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2336;}); _temp2335;}); break;} case
189: _LL2332: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2342=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2342[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2343; _temp2343.tag=
Cyc_InitializerList_tok; _temp2343.f1=({ struct Cyc_List_List* _temp2344=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2344->hd=(
void*)({ struct _tuple19* _temp2345=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2345->f1= 0; _temp2345->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2345;}); _temp2344->tl=
0; _temp2344;}); _temp2343;}); _temp2342;}); break; case 190: _LL2341: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2347=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2347[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2348; _temp2348.tag= Cyc_InitializerList_tok;
_temp2348.f1=({ struct Cyc_List_List* _temp2349=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2349->hd=( void*)({ struct
_tuple19* _temp2350=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2350->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2350->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2350;});
_temp2349->tl= 0; _temp2349;}); _temp2348;}); _temp2347;}); break; case 191:
_LL2346: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2352=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2352[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2353; _temp2353.tag=
Cyc_InitializerList_tok; _temp2353.f1=({ struct Cyc_List_List* _temp2354=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2354->hd=(
void*)({ struct _tuple19* _temp2355=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2355->f1= 0; _temp2355->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2355;}); _temp2354->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2354;}); _temp2353;}); _temp2352;}); break; case 192:
_LL2351: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2357=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2357[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2358; _temp2358.tag=
Cyc_InitializerList_tok; _temp2358.f1=({ struct Cyc_List_List* _temp2359=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2359->hd=(
void*)({ struct _tuple19* _temp2360=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2360->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2360->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2360;});
_temp2359->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2359;});
_temp2358;}); _temp2357;}); break; case 193: _LL2356: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2362=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2362[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2363; _temp2363.tag= Cyc_DesignatorList_tok;
_temp2363.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2363;});
_temp2362;}); break; case 194: _LL2361: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2365=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2365[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2366; _temp2366.tag=
Cyc_DesignatorList_tok; _temp2366.f1=({ struct Cyc_List_List* _temp2367=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2367->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2367->tl= 0; _temp2367;}); _temp2366;}); _temp2365;});
break; case 195: _LL2364: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2369=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2369[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2370; _temp2370.tag=
Cyc_DesignatorList_tok; _temp2370.f1=({ struct Cyc_List_List* _temp2371=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2371->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2371->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2371;});
_temp2370;}); _temp2369;}); break; case 196: _LL2368: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2373=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2373[ 0]=({ struct
Cyc_Designator_tok_struct _temp2374; _temp2374.tag= Cyc_Designator_tok;
_temp2374.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2375=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2375[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2376; _temp2376.tag=
Cyc_Absyn_ArrayElement; _temp2376.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2376;});
_temp2375;})); _temp2374;}); _temp2373;}); break; case 197: _LL2372: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2378=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2378[ 0]=({ struct
Cyc_Designator_tok_struct _temp2379; _temp2379.tag= Cyc_Designator_tok;
_temp2379.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2380=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2380[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2381; _temp2381.tag= Cyc_Absyn_FieldName;
_temp2381.f1=({ struct _tagged_arr* _temp2382=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2382[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2382;}); _temp2381;});
_temp2380;})); _temp2379;}); _temp2378;}); break; case 198: _LL2377: { void*
_temp2384= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2385=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2385 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2386=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2387=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2387[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2388; _temp2388.tag= Cyc_ParamDecl_tok; _temp2388.f1=({
struct _tuple2* _temp2389=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2389->f1= 0; _temp2389->f2= _temp2386; _temp2389->f3= _temp2384; _temp2389;});
_temp2388;}); _temp2387;}); break;}} case 199: _LL2383: { void* _temp2391= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp2392=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2393=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2394=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2395= Cyc_Parse_apply_tms( _temp2393, _temp2391, _temp2392,
_temp2394); if( _temp2395.f3 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2395.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2396=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2396[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2397; _temp2397.tag= Cyc_ParamDecl_tok; _temp2397.f1=({
struct _tuple2* _temp2398=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2398->f1= 0; _temp2398->f2= _temp2395.f1; _temp2398->f3= _temp2395.f2;
_temp2398;}); _temp2397;}); _temp2396;}); break;} case 200: _LL2390: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2400=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2400[ 0]=({ struct Cyc_Type_tok_struct
_temp2401; _temp2401.tag= Cyc_Type_tok; _temp2401.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2401;});
_temp2400;}); break; case 201: _LL2399: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2403=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2403[ 0]=({ struct Cyc_Type_tok_struct _temp2404; _temp2404.tag= Cyc_Type_tok;
_temp2404.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2405=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2405[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2406; _temp2406.tag= Cyc_Absyn_JoinEff;
_temp2406.f1= 0; _temp2406;}); _temp2405;})); _temp2404;}); _temp2403;}); break;
case 202: _LL2402: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2408=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2408[ 0]=({ struct Cyc_Type_tok_struct _temp2409; _temp2409.tag= Cyc_Type_tok;
_temp2409.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2410=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2410[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2411; _temp2411.tag= Cyc_Absyn_JoinEff;
_temp2411.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2411;}); _temp2410;})); _temp2409;});
_temp2408;}); break; case 203: _LL2407: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2413=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2413[ 0]=({ struct Cyc_Type_tok_struct _temp2414; _temp2414.tag= Cyc_Type_tok;
_temp2414.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2415=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2415[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2416; _temp2416.tag= Cyc_Absyn_RgnsEff;
_temp2416.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2416;}); _temp2415;})); _temp2414;});
_temp2413;}); break; case 204: _LL2412: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2418=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2418[ 0]=({ struct Cyc_Type_tok_struct _temp2419; _temp2419.tag= Cyc_Type_tok;
_temp2419.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2420=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2420[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2421; _temp2421.tag= Cyc_Absyn_JoinEff;
_temp2421.f1=({ struct Cyc_List_List* _temp2422=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2422->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2422->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2422;}); _temp2421;}); _temp2420;})); _temp2419;});
_temp2418;}); break; case 205: _LL2417: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2424=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2424[ 0]=({ struct Cyc_TypeList_tok_struct _temp2425; _temp2425.tag= Cyc_TypeList_tok;
_temp2425.f1=({ struct Cyc_List_List* _temp2426=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2426->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2426->tl= 0;
_temp2426;}); _temp2425;}); _temp2424;}); break; case 206: _LL2423: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2428=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2428[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2429; _temp2429.tag= Cyc_TypeList_tok; _temp2429.f1=({ struct Cyc_List_List*
_temp2430=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2430->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2430->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2430;});
_temp2429;}); _temp2428;}); break; case 207: _LL2427: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2432=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2432[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2433; _temp2433.tag= Cyc_AbstractDeclarator_tok;
_temp2433.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2434=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2434->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2434;});
_temp2433;}); _temp2432;}); break; case 208: _LL2431: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 209: _LL2435:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2437=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2437[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2438; _temp2438.tag=
Cyc_AbstractDeclarator_tok; _temp2438.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2439=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2439->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2439;});
_temp2438;}); _temp2437;}); break; case 210: _LL2436: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 211:
_LL2440: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2442=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2442[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2443; _temp2443.tag=
Cyc_AbstractDeclarator_tok; _temp2443.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2444=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2444->tms=({ struct Cyc_List_List* _temp2445=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2445->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2445->tl= 0; _temp2445;}); _temp2444;}); _temp2443;}); _temp2442;}); break;
case 212: _LL2441: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2447=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2447[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2448; _temp2448.tag= Cyc_AbstractDeclarator_tok; _temp2448.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2449=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2449->tms=({ struct Cyc_List_List* _temp2450=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2450->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2450->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2450;});
_temp2449;}); _temp2448;}); _temp2447;}); break; case 213: _LL2446: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2452=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2452[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2453; _temp2453.tag= Cyc_AbstractDeclarator_tok;
_temp2453.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2454=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2454->tms=({
struct Cyc_List_List* _temp2455=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2455->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2456=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2456[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2457; _temp2457.tag= Cyc_Absyn_ConstArray_mod; _temp2457.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2457;});
_temp2456;})); _temp2455->tl= 0; _temp2455;}); _temp2454;}); _temp2453;});
_temp2452;}); break; case 214: _LL2451: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2459=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2459[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2460; _temp2460.tag= Cyc_AbstractDeclarator_tok; _temp2460.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2461=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2461->tms=({ struct Cyc_List_List* _temp2462=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2462->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2463=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2463[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2464; _temp2464.tag= Cyc_Absyn_ConstArray_mod;
_temp2464.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2464;}); _temp2463;})); _temp2462->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2462;}); _temp2461;}); _temp2460;}); _temp2459;}); break; case 215: _LL2458:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2466=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2466[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2467; _temp2467.tag=
Cyc_AbstractDeclarator_tok; _temp2467.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2468=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2468->tms=({ struct Cyc_List_List* _temp2469=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2469->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2470=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2470[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2471; _temp2471.tag= Cyc_Absyn_Function_mod;
_temp2471.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2472=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2472[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2473; _temp2473.tag= Cyc_Absyn_WithTypes;
_temp2473.f1= 0; _temp2473.f2= 0; _temp2473.f3= 0; _temp2473.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2473.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2473;}); _temp2472;})); _temp2471;}); _temp2470;}));
_temp2469->tl= 0; _temp2469;}); _temp2468;}); _temp2467;}); _temp2466;}); break;
case 216: _LL2465: { struct _tuple16 _temp2477; struct Cyc_List_List* _temp2478;
struct Cyc_Core_Opt* _temp2480; struct Cyc_Absyn_VarargInfo* _temp2482; int
_temp2484; struct Cyc_List_List* _temp2486; struct _tuple16* _temp2475= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2477=*
_temp2475; _LL2487: _temp2486= _temp2477.f1; goto _LL2485; _LL2485: _temp2484=
_temp2477.f2; goto _LL2483; _LL2483: _temp2482= _temp2477.f3; goto _LL2481;
_LL2481: _temp2480= _temp2477.f4; goto _LL2479; _LL2479: _temp2478= _temp2477.f5;
goto _LL2476; _LL2476: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2488=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2488[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2489; _temp2489.tag= Cyc_AbstractDeclarator_tok; _temp2489.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2490=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2490->tms=({ struct Cyc_List_List* _temp2491=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2491->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2492=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2492[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2493; _temp2493.tag= Cyc_Absyn_Function_mod;
_temp2493.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2494=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2494[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2495; _temp2495.tag= Cyc_Absyn_WithTypes;
_temp2495.f1= _temp2486; _temp2495.f2= _temp2484; _temp2495.f3= _temp2482;
_temp2495.f4= _temp2480; _temp2495.f5= _temp2478; _temp2495;}); _temp2494;}));
_temp2493;}); _temp2492;})); _temp2491->tl= 0; _temp2491;}); _temp2490;});
_temp2489;}); _temp2488;}); break;} case 217: _LL2474: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2497=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2497[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2498; _temp2498.tag= Cyc_AbstractDeclarator_tok;
_temp2498.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2499=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2499->tms=({
struct Cyc_List_List* _temp2500=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2500->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2501=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2501[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2502; _temp2502.tag=
Cyc_Absyn_Function_mod; _temp2502.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2503=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2503[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2504; _temp2504.tag= Cyc_Absyn_WithTypes;
_temp2504.f1= 0; _temp2504.f2= 0; _temp2504.f3= 0; _temp2504.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2504.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2504;}); _temp2503;})); _temp2502;}); _temp2501;}));
_temp2500->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2500;});
_temp2499;}); _temp2498;}); _temp2497;}); break; case 218: _LL2496: { struct
_tuple16 _temp2508; struct Cyc_List_List* _temp2509; struct Cyc_Core_Opt*
_temp2511; struct Cyc_Absyn_VarargInfo* _temp2513; int _temp2515; struct Cyc_List_List*
_temp2517; struct _tuple16* _temp2506= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2508=*
_temp2506; _LL2518: _temp2517= _temp2508.f1; goto _LL2516; _LL2516: _temp2515=
_temp2508.f2; goto _LL2514; _LL2514: _temp2513= _temp2508.f3; goto _LL2512;
_LL2512: _temp2511= _temp2508.f4; goto _LL2510; _LL2510: _temp2509= _temp2508.f5;
goto _LL2507; _LL2507: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2519=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2519[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2520; _temp2520.tag= Cyc_AbstractDeclarator_tok; _temp2520.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2521=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2521->tms=({ struct Cyc_List_List* _temp2522=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2522->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2523=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2523[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2524; _temp2524.tag= Cyc_Absyn_Function_mod;
_temp2524.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2525=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2525[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2526; _temp2526.tag= Cyc_Absyn_WithTypes;
_temp2526.f1= _temp2517; _temp2526.f2= _temp2515; _temp2526.f3= _temp2513;
_temp2526.f4= _temp2511; _temp2526.f5= _temp2509; _temp2526;}); _temp2525;}));
_temp2524;}); _temp2523;})); _temp2522->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2522;}); _temp2521;}); _temp2520;}); _temp2519;}); break;} case 219:
_LL2505: { struct Cyc_List_List* _temp2528=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2529=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2529[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2530; _temp2530.tag= Cyc_AbstractDeclarator_tok; _temp2530.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2531=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2531->tms=({ struct Cyc_List_List* _temp2532=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2532->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2533=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2533[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2534; _temp2534.tag= Cyc_Absyn_TypeParams_mod;
_temp2534.f1= _temp2528; _temp2534.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2534.f3=
0; _temp2534;}); _temp2533;})); _temp2532->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2532;}); _temp2531;}); _temp2530;}); _temp2529;}); break;} case 220:
_LL2527:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2536=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2537=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2537[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2538; _temp2538.tag= Cyc_AbstractDeclarator_tok; _temp2538.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2539=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2539->tms=({ struct Cyc_List_List* _temp2540=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2540->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2541=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2541[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2542; _temp2542.tag= Cyc_Absyn_TypeParams_mod;
_temp2542.f1= _temp2536; _temp2542.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2542.f3=
0; _temp2542;}); _temp2541;})); _temp2540->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2540;}); _temp2539;}); _temp2538;}); _temp2537;}); break;} case 221:
_LL2535: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2544=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2544[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2545; _temp2545.tag=
Cyc_AbstractDeclarator_tok; _temp2545.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2546=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2546->tms=({ struct Cyc_List_List* _temp2547=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2547->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2548=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2548[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2549; _temp2549.tag= Cyc_Absyn_Attributes_mod;
_temp2549.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2549.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2549;}); _temp2548;}));
_temp2547->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms; _temp2547;});
_temp2546;}); _temp2545;}); _temp2544;}); break; case 222: _LL2543: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 223: _LL2550:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
224: _LL2551: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 225: _LL2552: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 226: _LL2553: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 227: _LL2554:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
228: _LL2555: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2567=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2567->name=({ struct
_tagged_arr* _temp2568=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2568[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2568;});
_temp2567->identity= 0; _temp2567->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2567;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2565=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2565[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2566; _temp2566.tag= Cyc_Absyn_VarType;
_temp2566.f1= tv; _temp2566;}); _temp2565;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2557=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2557[ 0]=({ struct Cyc_Stmt_tok_struct _temp2558; _temp2558.tag= Cyc_Stmt_tok;
_temp2558.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2559=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2559[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2560; _temp2560.tag= Cyc_Absyn_Region_s;
_temp2560.f1= tv; _temp2560.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2561=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2561->f1=(
void*) Cyc_Absyn_Loc_n; _temp2561->f2=({ struct _tagged_arr* _temp2562=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2562[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2562;});
_temp2561;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2563=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2563[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2564; _temp2564.tag=
Cyc_Absyn_RgnHandleType; _temp2564.f1=( void*) t; _temp2564;}); _temp2563;}), 0);
_temp2560.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2560;}); _temp2559;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2558;});
_temp2557;}); break;} case 229: _LL2556: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2580=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2580->name=({ struct
_tagged_arr* _temp2581=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2581[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2583; _temp2583.tag= Cyc_Std_String_pa; _temp2583.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2582[ 1u]={& _temp2583}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2582, sizeof( void*), 1u));}});
_temp2581;}); _temp2580->identity= 0; _temp2580->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2580;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2578=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2578[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2579; _temp2579.tag= Cyc_Absyn_VarType;
_temp2579.f1= tv; _temp2579;}); _temp2578;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2570=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2570[ 0]=({ struct Cyc_Stmt_tok_struct _temp2571; _temp2571.tag= Cyc_Stmt_tok;
_temp2571.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2572=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2572[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2573; _temp2573.tag= Cyc_Absyn_Region_s;
_temp2573.f1= tv; _temp2573.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2574=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2574->f1=(
void*) Cyc_Absyn_Loc_n; _temp2574->f2=({ struct _tagged_arr* _temp2575=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2575[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2575;});
_temp2574;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2576=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2576[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2577; _temp2577.tag=
Cyc_Absyn_RgnHandleType; _temp2577.f1=( void*) t; _temp2577;}); _temp2576;}), 0);
_temp2573.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2573;}); _temp2572;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2571;});
_temp2570;}); break;} case 230: _LL2569: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2585=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2585[ 0]=({ struct Cyc_Stmt_tok_struct _temp2586; _temp2586.tag= Cyc_Stmt_tok;
_temp2586.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2587=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2587[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2588; _temp2588.tag= Cyc_Absyn_Cut_s;
_temp2588.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2588;}); _temp2587;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2586;});
_temp2585;}); break; case 231: _LL2584: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2590=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2590[ 0]=({ struct Cyc_Stmt_tok_struct _temp2591; _temp2591.tag= Cyc_Stmt_tok;
_temp2591.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2592=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2592[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2593; _temp2593.tag= Cyc_Absyn_Splice_s;
_temp2593.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2593;}); _temp2592;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2591;});
_temp2590;}); break; case 232: _LL2589: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2595=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2595[ 0]=({ struct Cyc_Stmt_tok_struct _temp2596; _temp2596.tag= Cyc_Stmt_tok;
_temp2596.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2597=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2597[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2598; _temp2598.tag= Cyc_Absyn_Label_s;
_temp2598.f1=({ struct _tagged_arr* _temp2599=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2599[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2599;});
_temp2598.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2598;}); _temp2597;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2596;});
_temp2595;}); break; case 233: _LL2594: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2601=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2601[ 0]=({ struct Cyc_Stmt_tok_struct _temp2602; _temp2602.tag= Cyc_Stmt_tok;
_temp2602.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2602;});
_temp2601;}); break; case 234: _LL2600: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2604=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2604[ 0]=({ struct Cyc_Stmt_tok_struct _temp2605; _temp2605.tag= Cyc_Stmt_tok;
_temp2605.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2605;}); _temp2604;}); break; case 235: _LL2603: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2607=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2607[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2608; _temp2608.tag= Cyc_Stmt_tok; _temp2608.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2608;}); _temp2607;}); break; case 236: _LL2606: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 237:
_LL2609: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2611=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2611[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2612; _temp2612.tag= Cyc_Stmt_tok; _temp2612.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2612;}); _temp2611;}); break;
case 238: _LL2610: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2614=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2614[ 0]=({ struct Cyc_Stmt_tok_struct _temp2615; _temp2615.tag= Cyc_Stmt_tok;
_temp2615.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2615;});
_temp2614;}); break; case 239: _LL2613: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 240: _LL2616:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2618=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2618[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2619; _temp2619.tag= Cyc_Stmt_tok; _temp2619.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2619;}); _temp2618;}); break; case 241: _LL2617: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2621=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2621[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2622; _temp2622.tag= Cyc_Stmt_tok; _temp2622.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2623=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2623[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2624; _temp2624.tag= Cyc_Absyn_Fn_d;
_temp2624.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2624;}); _temp2623;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2622;}); _temp2621;}); break; case 242: _LL2620: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2626=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2626[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2627; _temp2627.tag= Cyc_Stmt_tok; _temp2627.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2628=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2628[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2629; _temp2629.tag= Cyc_Absyn_Fn_d;
_temp2629.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2629;}); _temp2628;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2627;}); _temp2626;}); break; case 243: _LL2625: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2631=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2631[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2632; _temp2632.tag= Cyc_Stmt_tok; _temp2632.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2632;});
_temp2631;}); break; case 244: _LL2630: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2634=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2634[ 0]=({ struct Cyc_Stmt_tok_struct _temp2635; _temp2635.tag= Cyc_Stmt_tok;
_temp2635.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2635;}); _temp2634;}); break; case 245: _LL2633: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2637=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2637[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2638; _temp2638.tag= Cyc_Stmt_tok; _temp2638.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2638;}); _temp2637;}); break; case 246: _LL2636: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2640=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2640[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2641; _temp2641.tag= Cyc_Stmt_tok; _temp2641.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2642=( struct Cyc_Absyn_SwitchC_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2642[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2643; _temp2643.tag= Cyc_Absyn_SwitchC_s;
_temp2643.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2643.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2643;});
_temp2642;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2641;}); _temp2640;}); break; case 247:
_LL2639: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2645=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2645[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2646; _temp2646.tag= Cyc_Stmt_tok; _temp2646.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2646;});
_temp2645;}); break; case 248: _LL2644: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2648=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2648[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2649; _temp2649.tag= Cyc_SwitchClauseList_tok; _temp2649.f1= 0; _temp2649;});
_temp2648;}); break; case 249: _LL2647: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2651=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2651[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2652; _temp2652.tag= Cyc_SwitchClauseList_tok; _temp2652.f1=({ struct Cyc_List_List*
_temp2653=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2653->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2654=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2654->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2654->pat_vars= 0; _temp2654->where_clause= 0; _temp2654->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2654->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2654;}); _temp2653->tl= 0; _temp2653;});
_temp2652;}); _temp2651;}); break; case 250: _LL2650: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2656=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2656[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2657; _temp2657.tag= Cyc_SwitchClauseList_tok;
_temp2657.f1=({ struct Cyc_List_List* _temp2658=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2658->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2659=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2659->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2659->pat_vars= 0; _temp2659->where_clause= 0;
_temp2659->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2659->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2659;});
_temp2658->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2658;}); _temp2657;});
_temp2656;}); break; case 251: _LL2655: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2661=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2661[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2662; _temp2662.tag= Cyc_SwitchClauseList_tok; _temp2662.f1=({ struct Cyc_List_List*
_temp2663=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2663->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2664=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2664->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2664->pat_vars=
0; _temp2664->where_clause= 0; _temp2664->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2664->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2664;}); _temp2663->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2663;});
_temp2662;}); _temp2661;}); break; case 252: _LL2660: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2666=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2666[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2667; _temp2667.tag= Cyc_SwitchClauseList_tok;
_temp2667.f1=({ struct Cyc_List_List* _temp2668=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2668->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2669=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2669->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp2669->pat_vars= 0; _temp2669->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2669->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2669->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2669;});
_temp2668->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2668;}); _temp2667;});
_temp2666;}); break; case 253: _LL2665: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2671=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2671[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2672; _temp2672.tag= Cyc_SwitchClauseList_tok; _temp2672.f1=({ struct Cyc_List_List*
_temp2673=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2673->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2674=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2674->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2674->pat_vars=
0; _temp2674->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2674->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2674->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2674;});
_temp2673->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2673;}); _temp2672;});
_temp2671;}); break; case 254: _LL2670: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2676=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2676[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2677; _temp2677.tag= Cyc_SwitchCClauseList_tok; _temp2677.f1= 0; _temp2677;});
_temp2676;}); break; case 255: _LL2675: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2679=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2679[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2680; _temp2680.tag= Cyc_SwitchCClauseList_tok; _temp2680.f1=({ struct Cyc_List_List*
_temp2681=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2681->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2682=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2682->cnst_exp= 0;
_temp2682->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2682->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2682;});
_temp2681->tl= 0; _temp2681;}); _temp2680;}); _temp2679;}); break; case 256:
_LL2678: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2684=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2684[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2685; _temp2685.tag=
Cyc_SwitchCClauseList_tok; _temp2685.f1=({ struct Cyc_List_List* _temp2686=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2686->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2687=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2687->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2687->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2687->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2687;});
_temp2686->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2686;}); _temp2685;});
_temp2684;}); break; case 257: _LL2683: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2689=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2689[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2690; _temp2690.tag= Cyc_SwitchCClauseList_tok; _temp2690.f1=({ struct Cyc_List_List*
_temp2691=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2691->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2692=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2692->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2692->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2692->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2692;});
_temp2691->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2691;}); _temp2690;});
_temp2689;}); break; case 258: _LL2688: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2694=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2694[ 0]=({ struct Cyc_Stmt_tok_struct _temp2695; _temp2695.tag= Cyc_Stmt_tok;
_temp2695.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2695;}); _temp2694;}); break; case 259: _LL2693: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2697=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2697[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2698; _temp2698.tag= Cyc_Stmt_tok; _temp2698.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2698;}); _temp2697;}); break; case 260: _LL2696: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2700=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2700[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2701; _temp2701.tag= Cyc_Stmt_tok; _temp2701.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2701;}); _temp2700;}); break; case 261: _LL2699: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2703=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2703[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2704; _temp2704.tag= Cyc_Stmt_tok; _temp2704.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2704;}); _temp2703;}); break; case 262: _LL2702: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2706=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2706[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2707; _temp2707.tag= Cyc_Stmt_tok; _temp2707.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2707;}); _temp2706;}); break; case 263: _LL2705: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2709=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2709[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2710; _temp2710.tag= Cyc_Stmt_tok; _temp2710.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2710;}); _temp2709;}); break; case 264: _LL2708: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2712=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2712[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2713; _temp2713.tag= Cyc_Stmt_tok; _temp2713.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2713;}); _temp2712;}); break; case 265: _LL2711: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2715=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2715[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2716; _temp2716.tag= Cyc_Stmt_tok; _temp2716.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2716;});
_temp2715;}); break; case 266: _LL2714: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2718=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2718[ 0]=({ struct Cyc_Stmt_tok_struct _temp2719; _temp2719.tag= Cyc_Stmt_tok;
_temp2719.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2719;});
_temp2718;}); break; case 267: _LL2717: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2721=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2721[ 0]=({ struct Cyc_Stmt_tok_struct _temp2722; _temp2722.tag= Cyc_Stmt_tok;
_temp2722.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2722;}); _temp2721;}); break; case 268: _LL2720: { struct Cyc_List_List*
_temp2724= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt* _temp2725= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2726=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2726[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2727; _temp2727.tag= Cyc_Stmt_tok; _temp2727.f1= Cyc_Parse_flatten_declarations(
_temp2724, _temp2725); _temp2727;}); _temp2726;}); break;} case 269: _LL2723: {
struct Cyc_List_List* _temp2729= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2730= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2731=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2731[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2732; _temp2732.tag= Cyc_Stmt_tok; _temp2732.f1= Cyc_Parse_flatten_declarations(
_temp2729, _temp2730); _temp2732;}); _temp2731;}); break;} case 270: _LL2728: {
struct Cyc_List_List* _temp2734= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2735= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2736=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2736[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2737; _temp2737.tag= Cyc_Stmt_tok; _temp2737.f1= Cyc_Parse_flatten_declarations(
_temp2734, _temp2735); _temp2737;}); _temp2736;}); break;} case 271: _LL2733: {
struct Cyc_List_List* _temp2739= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2740= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2741=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2741[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2742; _temp2742.tag= Cyc_Stmt_tok; _temp2742.f1= Cyc_Parse_flatten_declarations(
_temp2739, _temp2740); _temp2742;}); _temp2741;}); break;} case 272: _LL2738:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2744=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2744[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2745; _temp2745.tag= Cyc_Stmt_tok; _temp2745.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2746=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2746[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2746;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2745;}); _temp2744;}); break; case 273: _LL2743: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2748=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2748[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2749; _temp2749.tag= Cyc_Stmt_tok; _temp2749.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2749;}); _temp2748;}); break; case 274:
_LL2747: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2751=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2751[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2752; _temp2752.tag= Cyc_Stmt_tok; _temp2752.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2752;}); _temp2751;}); break; case 275: _LL2750: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2754=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2754[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2755; _temp2755.tag= Cyc_Stmt_tok; _temp2755.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2755;}); _temp2754;}); break; case 276:
_LL2753: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2757=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2757[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2758; _temp2758.tag= Cyc_Stmt_tok; _temp2758.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2758;}); _temp2757;}); break; case 277: _LL2756: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2760=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2760[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2761; _temp2761.tag= Cyc_Stmt_tok; _temp2761.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2761;}); _temp2760;}); break; case 278:
_LL2759: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2763=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2763[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2764; _temp2764.tag= Cyc_Stmt_tok; _temp2764.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2764;}); _temp2763;}); break; case 279:
_LL2762: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2766=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2766[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2767; _temp2767.tag= Cyc_Stmt_tok; _temp2767.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2767;}); _temp2766;}); break; case 280: _LL2765: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2769=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2769[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2770; _temp2770.tag= Cyc_Pattern_tok; _temp2770.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2770;});
_temp2769;}); break; case 281: _LL2768: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 282:
_LL2771: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2773=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2773[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2774; _temp2774.tag= Cyc_Pattern_tok; _temp2774.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2775=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2775[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2776; _temp2776.tag= Cyc_Absyn_Int_p; _temp2776.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2776.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2776;}); _temp2775;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2774;});
_temp2773;}); break; case 283: _LL2772: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2778=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2778[ 0]=({ struct Cyc_Pattern_tok_struct _temp2779; _temp2779.tag= Cyc_Pattern_tok;
_temp2779.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2780=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2780[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2781; _temp2781.tag= Cyc_Absyn_Int_p;
_temp2781.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2781.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2781;});
_temp2780;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2779;}); _temp2778;}); break; case 284:
_LL2777: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2783=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2783[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2784; _temp2784.tag= Cyc_Pattern_tok; _temp2784.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2785=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2785[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2786; _temp2786.tag= Cyc_Absyn_Float_p; _temp2786.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2786;}); _temp2785;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2784;});
_temp2783;}); break; case 285: _LL2782: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2788=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2788[ 0]=({ struct Cyc_Pattern_tok_struct _temp2789; _temp2789.tag= Cyc_Pattern_tok;
_temp2789.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2790=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2790[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2791; _temp2791.tag= Cyc_Absyn_Char_p; _temp2791.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2791;});
_temp2790;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2789;}); _temp2788;}); break; case 286:
_LL2787: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2793=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2793[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2794; _temp2794.tag= Cyc_Pattern_tok; _temp2794.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2794;});
_temp2793;}); break; case 287: _LL2792: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2796=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2796[ 0]=({ struct Cyc_Pattern_tok_struct _temp2797; _temp2797.tag= Cyc_Pattern_tok;
_temp2797.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2798=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2798[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2799; _temp2799.tag=
Cyc_Absyn_UnknownId_p; _temp2799.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2799;}); _temp2798;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2797;}); _temp2796;}); break; case 288:
_LL2795: { struct Cyc_List_List* _temp2801=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2802=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2802[ 0]=({ struct Cyc_Pattern_tok_struct _temp2803; _temp2803.tag= Cyc_Pattern_tok;
_temp2803.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2804=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2804[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2805; _temp2805.tag= Cyc_Absyn_UnknownCall_p; _temp2805.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2805.f2=
_temp2801; _temp2805.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2805;});
_temp2804;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2803;}); _temp2802;}); break;} case
289: _LL2800: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2807=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2807[ 0]=({ struct Cyc_Pattern_tok_struct _temp2808; _temp2808.tag= Cyc_Pattern_tok;
_temp2808.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2809=( struct Cyc_Absyn_Tuple_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2809[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2810; _temp2810.tag= Cyc_Absyn_Tuple_p;
_temp2810.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2810;}); _temp2809;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2808;}); _temp2807;}); break; case 290: _LL2806: { struct Cyc_List_List*
_temp2812=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); yyval=(
void*)({ struct Cyc_Pattern_tok_struct* _temp2813=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2813[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2814; _temp2814.tag= Cyc_Pattern_tok; _temp2814.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2815=( struct Cyc_Absyn_UnknownFields_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2815[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2816; _temp2816.tag= Cyc_Absyn_UnknownFields_p;
_temp2816.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2816.f2= _temp2812; _temp2816.f3= 0; _temp2816;});
_temp2815;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2814;}); _temp2813;}); break;} case
291: _LL2811: { struct Cyc_List_List* _temp2818=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2819=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2819[ 0]=({ struct Cyc_Pattern_tok_struct _temp2820; _temp2820.tag= Cyc_Pattern_tok;
_temp2820.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2821=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2821[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2822; _temp2822.tag= Cyc_Absyn_UnknownFields_p; _temp2822.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2822.f2=
_temp2818; _temp2822.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2822;});
_temp2821;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2820;}); _temp2819;}); break;} case
292: _LL2817: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2824=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2824[ 0]=({ struct Cyc_Pattern_tok_struct _temp2825; _temp2825.tag= Cyc_Pattern_tok;
_temp2825.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2826=( struct Cyc_Absyn_Pointer_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2826[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2827; _temp2827.tag= Cyc_Absyn_Pointer_p;
_temp2827.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2827;}); _temp2826;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2825;});
_temp2824;}); break; case 293: _LL2823: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2829=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2829[ 0]=({ struct Cyc_Pattern_tok_struct _temp2830; _temp2830.tag= Cyc_Pattern_tok;
_temp2830.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2831=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2831[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2832; _temp2832.tag=
Cyc_Absyn_Reference_p; _temp2832.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2833=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2833->f1=(
void*) Cyc_Absyn_Loc_n; _temp2833->f2=({ struct _tagged_arr* _temp2834=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2834[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2834;});
_temp2833;}),( void*) Cyc_Absyn_VoidType, 0); _temp2832;}); _temp2831;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2830;}); _temp2829;}); break; case 294: _LL2828: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2836=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2836[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2837; _temp2837.tag= Cyc_PatternList_tok;
_temp2837.f1= 0; _temp2837;}); _temp2836;}); break; case 295: _LL2835: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2839=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2839[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2840; _temp2840.tag= Cyc_PatternList_tok;
_temp2840.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2840;}); _temp2839;}); break; case 296: _LL2838: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2842=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2842[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2843; _temp2843.tag= Cyc_PatternList_tok;
_temp2843.f1=({ struct Cyc_List_List* _temp2844=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2844->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2844->tl= 0;
_temp2844;}); _temp2843;}); _temp2842;}); break; case 297: _LL2841: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2846=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2846[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2847; _temp2847.tag= Cyc_PatternList_tok;
_temp2847.f1=({ struct Cyc_List_List* _temp2848=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2848->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2848->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2848;}); _temp2847;}); _temp2846;}); break; case 298:
_LL2845: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2850=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2850[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2851; _temp2851.tag=
Cyc_FieldPattern_tok; _temp2851.f1=({ struct _tuple13* _temp2852=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2852->f1= 0; _temp2852->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2852;}); _temp2851;}); _temp2850;}); break; case 299:
_LL2849: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2854=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2854[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2855; _temp2855.tag=
Cyc_FieldPattern_tok; _temp2855.f1=({ struct _tuple13* _temp2856=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2856->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2856->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2856;}); _temp2855;}); _temp2854;}); break; case 300:
_LL2853: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2858=(
struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2858[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2859; _temp2859.tag=
Cyc_FieldPatternList_tok; _temp2859.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2859;});
_temp2858;}); break; case 301: _LL2857: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2861=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2861[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2862; _temp2862.tag= Cyc_FieldPatternList_tok; _temp2862.f1=({ struct Cyc_List_List*
_temp2863=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2863->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2863->tl= 0;
_temp2863;}); _temp2862;}); _temp2861;}); break; case 302: _LL2860: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2865=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2865[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2866; _temp2866.tag= Cyc_FieldPatternList_tok;
_temp2866.f1=({ struct Cyc_List_List* _temp2867=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2867->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2867->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2867;}); _temp2866;}); _temp2865;}); break; case 303:
_LL2864: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 304: _LL2868: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2870=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2870[ 0]=({ struct Cyc_Exp_tok_struct _temp2871; _temp2871.tag= Cyc_Exp_tok;
_temp2871.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2871;}); _temp2870;}); break; case 305: _LL2869: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 306: _LL2872:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2874=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2874[ 0]=({ struct Cyc_Exp_tok_struct
_temp2875; _temp2875.tag= Cyc_Exp_tok; _temp2875.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2875;}); _temp2874;}); break; case 307: _LL2873: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2877=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2877[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2878; _temp2878.tag= Cyc_Primopopt_tok; _temp2878.f1=
0; _temp2878;}); _temp2877;}); break; case 308: _LL2876: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2880=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2880[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2881; _temp2881.tag= Cyc_Primopopt_tok; _temp2881.f1=({
struct Cyc_Core_Opt* _temp2882=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2882->v=( void*)(( void*) Cyc_Absyn_Times); _temp2882;});
_temp2881;}); _temp2880;}); break; case 309: _LL2879: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2884=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2884[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2885; _temp2885.tag= Cyc_Primopopt_tok; _temp2885.f1=({
struct Cyc_Core_Opt* _temp2886=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2886->v=( void*)(( void*) Cyc_Absyn_Div); _temp2886;});
_temp2885;}); _temp2884;}); break; case 310: _LL2883: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2888=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2888[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2889; _temp2889.tag= Cyc_Primopopt_tok; _temp2889.f1=({
struct Cyc_Core_Opt* _temp2890=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2890->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2890;});
_temp2889;}); _temp2888;}); break; case 311: _LL2887: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2892=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2892[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2893; _temp2893.tag= Cyc_Primopopt_tok; _temp2893.f1=({
struct Cyc_Core_Opt* _temp2894=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2894->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2894;});
_temp2893;}); _temp2892;}); break; case 312: _LL2891: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2896=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2896[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2897; _temp2897.tag= Cyc_Primopopt_tok; _temp2897.f1=({
struct Cyc_Core_Opt* _temp2898=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2898->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2898;});
_temp2897;}); _temp2896;}); break; case 313: _LL2895: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2900=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2900[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2901; _temp2901.tag= Cyc_Primopopt_tok; _temp2901.f1=({
struct Cyc_Core_Opt* _temp2902=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2902->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2902;});
_temp2901;}); _temp2900;}); break; case 314: _LL2899: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2904=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2904[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2905; _temp2905.tag= Cyc_Primopopt_tok; _temp2905.f1=({
struct Cyc_Core_Opt* _temp2906=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2906->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2906;});
_temp2905;}); _temp2904;}); break; case 315: _LL2903: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2908=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2908[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2909; _temp2909.tag= Cyc_Primopopt_tok; _temp2909.f1=({
struct Cyc_Core_Opt* _temp2910=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2910->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2910;});
_temp2909;}); _temp2908;}); break; case 316: _LL2907: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2912=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2912[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2913; _temp2913.tag= Cyc_Primopopt_tok; _temp2913.f1=({
struct Cyc_Core_Opt* _temp2914=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2914->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2914;});
_temp2913;}); _temp2912;}); break; case 317: _LL2911: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2916=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2916[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2917; _temp2917.tag= Cyc_Primopopt_tok; _temp2917.f1=({
struct Cyc_Core_Opt* _temp2918=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2918->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2918;});
_temp2917;}); _temp2916;}); break; case 318: _LL2915: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 319: _LL2919:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2921=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2921[ 0]=({ struct Cyc_Exp_tok_struct
_temp2922; _temp2922.tag= Cyc_Exp_tok; _temp2922.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2922;});
_temp2921;}); break; case 320: _LL2920: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2924=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2924[ 0]=({ struct Cyc_Exp_tok_struct _temp2925; _temp2925.tag= Cyc_Exp_tok;
_temp2925.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2925;}); _temp2924;}); break; case 321: _LL2923: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2927=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2927[ 0]=({ struct Cyc_Exp_tok_struct
_temp2928; _temp2928.tag= Cyc_Exp_tok; _temp2928.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2928;}); _temp2927;}); break; case 322: _LL2926: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2930=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2930[ 0]=({ struct Cyc_Exp_tok_struct
_temp2931; _temp2931.tag= Cyc_Exp_tok; _temp2931.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2931;}); _temp2930;}); break; case 323: _LL2929: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2933=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2933[ 0]=({ struct Cyc_Exp_tok_struct
_temp2934; _temp2934.tag= Cyc_Exp_tok; _temp2934.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2934;});
_temp2933;}); break; case 324: _LL2932: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2936=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2936[ 0]=({ struct Cyc_Exp_tok_struct _temp2937; _temp2937.tag= Cyc_Exp_tok;
_temp2937.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2937;}); _temp2936;}); break; case 325: _LL2935: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 326: _LL2938:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
327: _LL2939: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2941=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2941[ 0]=({ struct Cyc_Exp_tok_struct
_temp2942; _temp2942.tag= Cyc_Exp_tok; _temp2942.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2942;}); _temp2941;}); break; case 328: _LL2940: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL2943:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2945=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2945[ 0]=({ struct Cyc_Exp_tok_struct
_temp2946; _temp2946.tag= Cyc_Exp_tok; _temp2946.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2946;}); _temp2945;}); break; case 330: _LL2944: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL2947:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2949=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2949[ 0]=({ struct Cyc_Exp_tok_struct
_temp2950; _temp2950.tag= Cyc_Exp_tok; _temp2950.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2950;});
_temp2949;}); break; case 332: _LL2948: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL2951:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2953=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2953[ 0]=({ struct Cyc_Exp_tok_struct
_temp2954; _temp2954.tag= Cyc_Exp_tok; _temp2954.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2954;});
_temp2953;}); break; case 334: _LL2952: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL2955:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2957=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2957[ 0]=({ struct Cyc_Exp_tok_struct
_temp2958; _temp2958.tag= Cyc_Exp_tok; _temp2958.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2958;});
_temp2957;}); break; case 336: _LL2956: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 337: _LL2959:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2961=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2961[ 0]=({ struct Cyc_Exp_tok_struct
_temp2962; _temp2962.tag= Cyc_Exp_tok; _temp2962.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2962;}); _temp2961;}); break; case 338: _LL2960: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2964=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2964[ 0]=({ struct Cyc_Exp_tok_struct
_temp2965; _temp2965.tag= Cyc_Exp_tok; _temp2965.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2965;}); _temp2964;}); break; case 339: _LL2963: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 340: _LL2966:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2968=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2968[ 0]=({ struct Cyc_Exp_tok_struct
_temp2969; _temp2969.tag= Cyc_Exp_tok; _temp2969.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2969;}); _temp2968;}); break; case 341: _LL2967: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2971=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2971[ 0]=({ struct Cyc_Exp_tok_struct
_temp2972; _temp2972.tag= Cyc_Exp_tok; _temp2972.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2972;}); _temp2971;}); break; case 342: _LL2970: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2974=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2974[ 0]=({ struct Cyc_Exp_tok_struct
_temp2975; _temp2975.tag= Cyc_Exp_tok; _temp2975.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2975;}); _temp2974;}); break; case 343: _LL2973: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2977=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2977[ 0]=({ struct Cyc_Exp_tok_struct
_temp2978; _temp2978.tag= Cyc_Exp_tok; _temp2978.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2978;}); _temp2977;}); break; case 344: _LL2976: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 345: _LL2979:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2981=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2981[ 0]=({ struct Cyc_Exp_tok_struct
_temp2982; _temp2982.tag= Cyc_Exp_tok; _temp2982.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2982;}); _temp2981;}); break; case 346: _LL2980: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2984=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2984[ 0]=({ struct Cyc_Exp_tok_struct
_temp2985; _temp2985.tag= Cyc_Exp_tok; _temp2985.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2985;}); _temp2984;}); break; case 347: _LL2983: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 348: _LL2986:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2988=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2988[ 0]=({ struct Cyc_Exp_tok_struct
_temp2989; _temp2989.tag= Cyc_Exp_tok; _temp2989.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2989;});
_temp2988;}); break; case 349: _LL2987: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2991=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2991[ 0]=({ struct Cyc_Exp_tok_struct _temp2992; _temp2992.tag= Cyc_Exp_tok;
_temp2992.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2992;}); _temp2991;}); break; case 350: _LL2990: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 351: _LL2993:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2995=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2995[ 0]=({ struct Cyc_Exp_tok_struct
_temp2996; _temp2996.tag= Cyc_Exp_tok; _temp2996.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2996;});
_temp2995;}); break; case 352: _LL2994: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2998=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2998[ 0]=({ struct Cyc_Exp_tok_struct _temp2999; _temp2999.tag= Cyc_Exp_tok;
_temp2999.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2999;}); _temp2998;}); break; case 353: _LL2997: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3001=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3001[ 0]=({ struct Cyc_Exp_tok_struct
_temp3002; _temp3002.tag= Cyc_Exp_tok; _temp3002.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3002;});
_temp3001;}); break; case 354: _LL3000: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 355: _LL3003:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3005=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3005[ 0]=({ struct Cyc_Exp_tok_struct
_temp3006; _temp3006.tag= Cyc_Exp_tok; _temp3006.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3006;}); _temp3005;}); break; case 356: _LL3004: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 357: _LL3007:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3009=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3009[ 0]=({ struct Cyc_Exp_tok_struct
_temp3010; _temp3010.tag= Cyc_Exp_tok; _temp3010.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3010;}); _temp3009;}); break; case 358: _LL3008: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3012=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3012[ 0]=({ struct Cyc_Exp_tok_struct
_temp3013; _temp3013.tag= Cyc_Exp_tok; _temp3013.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3013;}); _temp3012;}); break; case 359: _LL3011: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3015=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3015[ 0]=({ struct Cyc_Exp_tok_struct
_temp3016; _temp3016.tag= Cyc_Exp_tok; _temp3016.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3016;}); _temp3015;}); break; case 360: _LL3014: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3018=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3018[ 0]=({ struct Cyc_Exp_tok_struct
_temp3019; _temp3019.tag= Cyc_Exp_tok; _temp3019.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3019;}); _temp3018;}); break; case 361: _LL3017: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 362: _LL3020:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3022=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3022[ 0]=({ struct Cyc_Exp_tok_struct
_temp3023; _temp3023.tag= Cyc_Exp_tok; _temp3023.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3023;}); _temp3022;}); break; case 363: _LL3021: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3025=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3025[ 0]=({ struct Cyc_Exp_tok_struct
_temp3026; _temp3026.tag= Cyc_Exp_tok; _temp3026.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3026;});
_temp3025;}); break; case 364: _LL3024: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3028=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3028[ 0]=({ struct Cyc_Exp_tok_struct _temp3029; _temp3029.tag= Cyc_Exp_tok;
_temp3029.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3029;}); _temp3028;}); break; case 365: _LL3027: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3031=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3031[ 0]=({ struct Cyc_Exp_tok_struct
_temp3032; _temp3032.tag= Cyc_Exp_tok; _temp3032.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_StructField_struct*
_temp3033=( struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp3033[ 0]=({ struct Cyc_Absyn_StructField_struct _temp3034; _temp3034.tag=
Cyc_Absyn_StructField; _temp3034.f1=({ struct _tagged_arr* _temp3035=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp3035[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3035;});
_temp3034;}); _temp3033;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3032;});
_temp3031;}); break; case 366: _LL3030: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3037=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3037[ 0]=({ struct Cyc_Exp_tok_struct _temp3038; _temp3038.tag= Cyc_Exp_tok;
_temp3038.f1= Cyc_Absyn_offsetof_exp((* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])).f3,( void*)({
struct Cyc_Absyn_TupleIndex_struct* _temp3039=( struct Cyc_Absyn_TupleIndex_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Absyn_TupleIndex_struct)); _temp3039[ 0]=({
struct Cyc_Absyn_TupleIndex_struct _temp3040; _temp3040.tag= Cyc_Absyn_TupleIndex;
_temp3040.f1=( unsigned int)(* Cyc_yyget_Int_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f2; _temp3040;});
_temp3039;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3038;}); _temp3037;}); break; case 367:
_LL3036: { struct Cyc_Position_Segment* _temp3042= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); struct
Cyc_List_List* _temp3043=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, _temp3042, Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3044=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3044[ 0]=({ struct Cyc_Exp_tok_struct
_temp3045; _temp3045.tag= Cyc_Exp_tok; _temp3045.f1= Cyc_Absyn_gentyp_exp(
_temp3043,(* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3045;});
_temp3044;}); break;} case 368: _LL3041: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3047=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3047[ 0]=({ struct Cyc_Exp_tok_struct _temp3048; _temp3048.tag= Cyc_Exp_tok;
_temp3048.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3049=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3049[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3050; _temp3050.tag= Cyc_Absyn_Malloc_e;
_temp3050.f1= 0; _temp3050.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3050;}); _temp3049;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3048;});
_temp3047;}); break; case 369: _LL3046: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3052=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3052[ 0]=({ struct Cyc_Exp_tok_struct _temp3053; _temp3053.tag= Cyc_Exp_tok;
_temp3053.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3054=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3054[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3055; _temp3055.tag= Cyc_Absyn_Malloc_e;
_temp3055.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3055.f2=(
void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3055;}); _temp3054;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3053;});
_temp3052;}); break; case 370: _LL3051: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3057=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3057[ 0]=({ struct Cyc_Primop_tok_struct _temp3058; _temp3058.tag= Cyc_Primop_tok;
_temp3058.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3058;}); _temp3057;});
break; case 371: _LL3056: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3060=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3060[ 0]=({ struct Cyc_Primop_tok_struct _temp3061; _temp3061.tag= Cyc_Primop_tok;
_temp3061.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3061;}); _temp3060;});
break; case 372: _LL3059: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3063=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3063[ 0]=({ struct Cyc_Primop_tok_struct _temp3064; _temp3064.tag= Cyc_Primop_tok;
_temp3064.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3064;}); _temp3063;});
break; case 373: _LL3062: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 374: _LL3065: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3067=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3067[ 0]=({ struct Cyc_Exp_tok_struct _temp3068; _temp3068.tag= Cyc_Exp_tok;
_temp3068.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3068;}); _temp3067;}); break; case 375: _LL3066: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3070=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3070[ 0]=({ struct Cyc_Exp_tok_struct
_temp3071; _temp3071.tag= Cyc_Exp_tok; _temp3071.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3071;}); _temp3070;}); break; case 376:
_LL3069: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3073=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3073[ 0]=({ struct Cyc_Exp_tok_struct
_temp3074; _temp3074.tag= Cyc_Exp_tok; _temp3074.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3074;});
_temp3073;}); break; case 377: _LL3072: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3076=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3076[ 0]=({ struct Cyc_Exp_tok_struct _temp3077; _temp3077.tag= Cyc_Exp_tok;
_temp3077.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3078=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3078[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3078;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3077;}); _temp3076;}); break; case 378: _LL3075: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3080=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3080[ 0]=({ struct Cyc_Exp_tok_struct
_temp3081; _temp3081.tag= Cyc_Exp_tok; _temp3081.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3081;});
_temp3080;}); break;} case 379: _LL3079: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3083=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3083[ 0]=({ struct Cyc_Exp_tok_struct _temp3084; _temp3084.tag= Cyc_Exp_tok;
_temp3084.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3085=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3085[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3085;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3084;}); _temp3083;}); break; case 380: _LL3082: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3087=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3087[ 0]=({ struct Cyc_Exp_tok_struct
_temp3088; _temp3088.tag= Cyc_Exp_tok; _temp3088.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3088;});
_temp3087;}); break;} case 381: _LL3086: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3090=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3090[ 0]=({ struct Cyc_Exp_tok_struct _temp3091; _temp3091.tag= Cyc_Exp_tok;
_temp3091.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3091;}); _temp3090;}); break; case 382: _LL3089: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3093=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3093[ 0]=({ struct Cyc_Exp_tok_struct
_temp3094; _temp3094.tag= Cyc_Exp_tok; _temp3094.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3094;}); _temp3093;}); break; case 383: _LL3092: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3096=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3096[ 0]=({ struct Cyc_Exp_tok_struct
_temp3097; _temp3097.tag= Cyc_Exp_tok; _temp3097.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3098=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3098[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3099; _temp3099.tag= Cyc_Absyn_CompoundLit_e;
_temp3099.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3099.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3099;});
_temp3098;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3097;}); _temp3096;}); break; case 384:
_LL3095: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3101=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3101[ 0]=({ struct Cyc_Exp_tok_struct
_temp3102; _temp3102.tag= Cyc_Exp_tok; _temp3102.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3103=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3103[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3104; _temp3104.tag= Cyc_Absyn_CompoundLit_e;
_temp3104.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3104.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3104;});
_temp3103;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3102;}); _temp3101;}); break; case 385:
_LL3100: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3106=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3106[ 0]=({ struct Cyc_Exp_tok_struct
_temp3107; _temp3107.tag= Cyc_Exp_tok; _temp3107.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3108=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3108[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3109; _temp3109.tag= Cyc_Absyn_Fill_e; _temp3109.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3109;});
_temp3108;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3107;}); _temp3106;}); break; case 386:
_LL3105: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3111=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3111[ 0]=({ struct Cyc_Exp_tok_struct
_temp3112; _temp3112.tag= Cyc_Exp_tok; _temp3112.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3113=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3113[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3114; _temp3114.tag= Cyc_Absyn_Codegen_e;
_temp3114.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3114;}); _temp3113;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3112;}); _temp3111;}); break; case 387: _LL3110: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3116=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3116[ 0]=({ struct Cyc_Exp_tok_struct
_temp3117; _temp3117.tag= Cyc_Exp_tok; _temp3117.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3118=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3118[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3119; _temp3119.tag= Cyc_Absyn_UnknownId_e;
_temp3119.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3119;}); _temp3118;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3117;});
_temp3116;}); break; case 388: _LL3115: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 389: _LL3120:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3122=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3122[ 0]=({ struct Cyc_Exp_tok_struct
_temp3123; _temp3123.tag= Cyc_Exp_tok; _temp3123.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3123;});
_temp3122;}); break; case 390: _LL3121: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 391:
_LL3124: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3126=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3126[ 0]=({ struct Cyc_Exp_tok_struct
_temp3127; _temp3127.tag= Cyc_Exp_tok; _temp3127.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3128=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3128[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3129; _temp3129.tag=
Cyc_Absyn_UnknownId_e; _temp3129.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3129;});
_temp3128;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3127;});
_temp3126;}); break; case 392: _LL3125: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3131=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3131[ 0]=({ struct Cyc_Exp_tok_struct _temp3132; _temp3132.tag= Cyc_Exp_tok;
_temp3132.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3133=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3133[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3134; _temp3134.tag=
Cyc_Absyn_UnknownId_e; _temp3134.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3134;});
_temp3133;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3132;}); _temp3131;}); break; case 393: _LL3130: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3136=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3136[ 0]=({ struct Cyc_Exp_tok_struct
_temp3137; _temp3137.tag= Cyc_Exp_tok; _temp3137.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3137;}); _temp3136;}); break; case 394: _LL3135: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3139=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3139[ 0]=({ struct Cyc_Exp_tok_struct
_temp3140; _temp3140.tag= Cyc_Exp_tok; _temp3140.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3141=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3141[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3142; _temp3142.tag= Cyc_Absyn_Struct_e;
_temp3142.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3142.f2= 0; _temp3142.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3142.f4= 0;
_temp3142;}); _temp3141;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3140;});
_temp3139;}); break; case 395: _LL3138: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3144=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3144[ 0]=({ struct Cyc_Exp_tok_struct _temp3145; _temp3145.tag= Cyc_Exp_tok;
_temp3145.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3145;}); _temp3144;}); break; case 396: _LL3143: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3147=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3147[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3148; _temp3148.tag= Cyc_ExpList_tok; _temp3148.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3148;});
_temp3147;}); break; case 397: _LL3146: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3150=( struct Cyc_ExpList_tok_struct*) _cycalloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3150[ 0]=({ struct Cyc_ExpList_tok_struct _temp3151; _temp3151.tag= Cyc_ExpList_tok;
_temp3151.f1=({ struct Cyc_List_List* _temp3152=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3152->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3152->tl= 0;
_temp3152;}); _temp3151;}); _temp3150;}); break; case 398: _LL3149: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3154=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3154[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3155; _temp3155.tag= Cyc_ExpList_tok; _temp3155.f1=({ struct Cyc_List_List*
_temp3156=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3156->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3156->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp3156;});
_temp3155;}); _temp3154;}); break; case 399: _LL3153: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3158=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3158[ 0]=({ struct Cyc_Exp_tok_struct
_temp3159; _temp3159.tag= Cyc_Exp_tok; _temp3159.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3159;});
_temp3158;}); break; case 400: _LL3157: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3161=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3161[ 0]=({ struct Cyc_Exp_tok_struct _temp3162; _temp3162.tag= Cyc_Exp_tok;
_temp3162.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3162;});
_temp3161;}); break; case 401: _LL3160: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3164=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3164[ 0]=({ struct Cyc_Exp_tok_struct _temp3165; _temp3165.tag= Cyc_Exp_tok;
_temp3165.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3165;});
_temp3164;}); break; case 402: _LL3163: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3167=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3167[ 0]=({ struct Cyc_Exp_tok_struct _temp3168; _temp3168.tag= Cyc_Exp_tok;
_temp3168.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3168;});
_temp3167;}); break; case 403: _LL3166: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3170=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3170[ 0]=({ struct Cyc_QualId_tok_struct _temp3171; _temp3171.tag= Cyc_QualId_tok;
_temp3171.f1=({ struct _tuple1* _temp3172=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3172->f1= Cyc_Absyn_rel_ns_null; _temp3172->f2=({ struct
_tagged_arr* _temp3173=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3173[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3173;}); _temp3172;});
_temp3171;}); _temp3170;}); break; case 404: _LL3169: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3174:(
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
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 405u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 112u, yyn -  121)] +  yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >=  0?
yystate <=  4329: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4330u,
yystate)] ==  yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){
yystate=( int) Cyc_yytable[ _check_known_subscript_notnull( 4330u, yystate)];}
else{ yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 
121)];} goto yynewstate; yyerrlab: if( yyerrstatus ==  0){ ++ Cyc_yynerrs; yyn=(
int) Cyc_yypact[ _check_known_subscript_notnull( 801u, yystate)]; if( yyn >  -
32768? yyn <  4329: 0){ int sze= 0; struct _tagged_arr msg; int x; int count;
count= 0; for( x= yyn <  0? - yyn: 0; x <  233u /  sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4330u, x +  yyn)] ==  x){( sze
+= Cyc_Std_strlen( Cyc_yytname[ _check_known_subscript_notnull( 233u, x)]) +  15,
count ++);}} msg=({ unsigned int _temp3176=( unsigned int)( sze +  15);
unsigned char* _temp3177=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp3176)); struct _tagged_arr _temp3179= _tag_arr(
_temp3177, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp3178= _temp3176; unsigned int i; for( i= 0; i <  _temp3178; i ++){
_temp3177[ i]='\000';}}; _temp3179;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
sizeof( unsigned char), 12u)); if( count <  5){ count= 0; for( x= yyn <  0? -
yyn: 0; x <  233u /  sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4330u, x +  yyn)] ==  x){ Cyc_Std_strcat( msg,
count ==  0? _tag_arr(", expecting `", sizeof( unsigned char), 14u): _tag_arr(" or `",
sizeof( unsigned char), 6u)); Cyc_Std_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 233u, x)]); Cyc_Std_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus ==  3){ if( Cyc_yychar ==  0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset ==  0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 801u,
yystate)]; if( yyn ==  - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn <  0? 1:
yyn >  4329)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4330u, yyn)] !=  1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4330u,
yyn)]; if( yyn <  0){ if( yyn ==  - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn ==  0){ goto yyerrpop;}} if( yyn ==  800){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3189= v; struct
_tuple15* _temp3207; struct _tuple15 _temp3209; int _temp3210; unsigned char
_temp3212; short _temp3214; struct _tagged_arr _temp3216; struct Cyc_Core_Opt*
_temp3218; struct Cyc_Core_Opt* _temp3220; struct Cyc_Core_Opt _temp3222; struct
_tagged_arr* _temp3223; struct _tuple1* _temp3225; struct _tuple1 _temp3227;
struct _tagged_arr* _temp3228; void* _temp3230; _LL3191: if(*(( void**)
_temp3189) ==  Cyc_Int_tok){ _LL3208: _temp3207=(( struct Cyc_Int_tok_struct*)
_temp3189)->f1; _temp3209=* _temp3207; _LL3211: _temp3210= _temp3209.f2; goto
_LL3192;} else{ goto _LL3193;} _LL3193: if(*(( void**) _temp3189) ==  Cyc_Char_tok){
_LL3213: _temp3212=(( struct Cyc_Char_tok_struct*) _temp3189)->f1; goto _LL3194;}
else{ goto _LL3195;} _LL3195: if(*(( void**) _temp3189) ==  Cyc_Short_tok){
_LL3215: _temp3214=(( struct Cyc_Short_tok_struct*) _temp3189)->f1; goto _LL3196;}
else{ goto _LL3197;} _LL3197: if(*(( void**) _temp3189) ==  Cyc_String_tok){
_LL3217: _temp3216=(( struct Cyc_String_tok_struct*) _temp3189)->f1; goto
_LL3198;} else{ goto _LL3199;} _LL3199: if(*(( void**) _temp3189) ==  Cyc_Stringopt_tok){
_LL3219: _temp3218=(( struct Cyc_Stringopt_tok_struct*) _temp3189)->f1; if(
_temp3218 ==  0){ goto _LL3200;} else{ goto _LL3201;}} else{ goto _LL3201;}
_LL3201: if(*(( void**) _temp3189) ==  Cyc_Stringopt_tok){ _LL3221: _temp3220=((
struct Cyc_Stringopt_tok_struct*) _temp3189)->f1; if( _temp3220 ==  0){ goto
_LL3203;} else{ _temp3222=* _temp3220; _LL3224: _temp3223=( struct _tagged_arr*)
_temp3222.v; goto _LL3202;}} else{ goto _LL3203;} _LL3203: if(*(( void**)
_temp3189) ==  Cyc_QualId_tok){ _LL3226: _temp3225=(( struct Cyc_QualId_tok_struct*)
_temp3189)->f1; _temp3227=* _temp3225; _LL3231: _temp3230= _temp3227.f1; goto
_LL3229; _LL3229: _temp3228= _temp3227.f2; goto _LL3204;} else{ goto _LL3205;}
_LL3205: goto _LL3206; _LL3192:({ struct Cyc_Std_Int_pa_struct _temp3233;
_temp3233.tag= Cyc_Std_Int_pa; _temp3233.f1=( int)(( unsigned int) _temp3210);{
void* _temp3232[ 1u]={& _temp3233}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3232, sizeof( void*), 1u));}}); goto
_LL3190; _LL3194:({ struct Cyc_Std_Int_pa_struct _temp3235; _temp3235.tag= Cyc_Std_Int_pa;
_temp3235.f1=( int)(( unsigned int)(( int) _temp3212));{ void* _temp3234[ 1u]={&
_temp3235}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3234, sizeof( void*), 1u));}}); goto _LL3190;
_LL3196:({ struct Cyc_Std_Int_pa_struct _temp3237; _temp3237.tag= Cyc_Std_Int_pa;
_temp3237.f1=( int)(( unsigned int)(( int) _temp3214));{ void* _temp3236[ 1u]={&
_temp3237}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3236, sizeof( void*), 1u));}}); goto _LL3190;
_LL3198:({ struct Cyc_Std_String_pa_struct _temp3239; _temp3239.tag= Cyc_Std_String_pa;
_temp3239.f1=( struct _tagged_arr) _temp3216;{ void* _temp3238[ 1u]={& _temp3239};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3238, sizeof( void*), 1u));}}); goto _LL3190; _LL3200:({ void*
_temp3240[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3240, sizeof( void*), 0u));}); goto _LL3190;
_LL3202:({ struct Cyc_Std_String_pa_struct _temp3242; _temp3242.tag= Cyc_Std_String_pa;
_temp3242.f1=( struct _tagged_arr)* _temp3223;{ void* _temp3241[ 1u]={&
_temp3242}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3241, sizeof( void*), 1u));}}); goto _LL3190;
_LL3204: { struct Cyc_List_List* _temp3243= 0;{ void* _temp3244= _temp3230;
struct Cyc_List_List* _temp3252; struct Cyc_List_List* _temp3254; _LL3246: if((
unsigned int) _temp3244 >  1u?*(( int*) _temp3244) ==  Cyc_Absyn_Rel_n: 0){
_LL3253: _temp3252=(( struct Cyc_Absyn_Rel_n_struct*) _temp3244)->f1; goto
_LL3247;} else{ goto _LL3248;} _LL3248: if(( unsigned int) _temp3244 >  1u?*((
int*) _temp3244) ==  Cyc_Absyn_Abs_n: 0){ _LL3255: _temp3254=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3244)->f1; goto _LL3249;} else{ goto _LL3250;} _LL3250: if( _temp3244 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3251;} else{ goto _LL3245;} _LL3247: _temp3243=
_temp3252; goto _LL3245; _LL3249: _temp3243= _temp3254; goto _LL3245; _LL3251:
goto _LL3245; _LL3245:;} for( 0; _temp3243 !=  0; _temp3243=(( struct Cyc_List_List*)
_check_null( _temp3243))->tl){({ struct Cyc_Std_String_pa_struct _temp3257;
_temp3257.tag= Cyc_Std_String_pa; _temp3257.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp3243))->hd);{ void*
_temp3256[ 1u]={& _temp3257}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3256, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3259; _temp3259.tag= Cyc_Std_String_pa;
_temp3259.f1=( struct _tagged_arr)* _temp3228;{ void* _temp3258[ 1u]={&
_temp3259}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3258, sizeof( void*), 1u));}}); goto _LL3190;}
_LL3206:({ void* _temp3260[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3260, sizeof( void*), 0u));}); goto
_LL3190; _LL3190:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3261=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3261->v=(
void*) Cyc_Lexing_from_file( f); _temp3261;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
