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
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
22; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 23; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
24; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 31; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
32; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 33; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 34; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 35; struct Cyc_Absyn_Fill_e_struct{
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
348u]={ 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 119, 2, 2, 101, 117, 113, 2, 97, 98, 108, 111, 99,
112, 105, 116, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 104, 93, 102, 96, 103, 110, 109, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 106,
2, 107, 115, 100, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 94, 114, 95, 118, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92}; static unsigned char
_temp1149[ 2u]="$"; static unsigned char _temp1150[ 6u]="error"; static
unsigned char _temp1151[ 12u]="$undefined."; static unsigned char _temp1152[ 5u]="AUTO";
static unsigned char _temp1153[ 9u]="REGISTER"; static unsigned char _temp1154[
7u]="STATIC"; static unsigned char _temp1155[ 7u]="EXTERN"; static unsigned char
_temp1156[ 8u]="TYPEDEF"; static unsigned char _temp1157[ 5u]="VOID"; static
unsigned char _temp1158[ 5u]="CHAR"; static unsigned char _temp1159[ 6u]="SHORT";
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
static unsigned char _temp1209[ 7u]="PTR_OP"; static unsigned char _temp1210[ 7u]="INC_OP";
static unsigned char _temp1211[ 7u]="DEC_OP"; static unsigned char _temp1212[ 8u]="LEFT_OP";
static unsigned char _temp1213[ 9u]="RIGHT_OP"; static unsigned char _temp1214[
6u]="LE_OP"; static unsigned char _temp1215[ 6u]="GE_OP"; static unsigned char
_temp1216[ 6u]="EQ_OP"; static unsigned char _temp1217[ 6u]="NE_OP"; static
unsigned char _temp1218[ 7u]="AND_OP"; static unsigned char _temp1219[ 6u]="OR_OP";
static unsigned char _temp1220[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1221[ 11u]="DIV_ASSIGN"; static unsigned char _temp1222[ 11u]="MOD_ASSIGN";
static unsigned char _temp1223[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1224[ 11u]="SUB_ASSIGN"; static unsigned char _temp1225[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1226[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1227[ 11u]="AND_ASSIGN"; static unsigned char _temp1228[ 11u]="XOR_ASSIGN";
static unsigned char _temp1229[ 10u]="OR_ASSIGN"; static unsigned char _temp1230[
9u]="ELLIPSIS"; static unsigned char _temp1231[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1232[ 12u]="COLON_COLON"; static unsigned char _temp1233[ 11u]="IDENTIFIER";
static unsigned char _temp1234[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1235[ 7u]="STRING"; static unsigned char _temp1236[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1237[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1238[ 9u]="TYPE_VAR"; static unsigned char _temp1239[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1240[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1241[ 10u]="ATTRIBUTE"; static unsigned char _temp1242[ 4u]="';'"; static
unsigned char _temp1243[ 4u]="'{'"; static unsigned char _temp1244[ 4u]="'}'";
static unsigned char _temp1245[ 4u]="'='"; static unsigned char _temp1246[ 4u]="'('";
static unsigned char _temp1247[ 4u]="')'"; static unsigned char _temp1248[ 4u]="','";
static unsigned char _temp1249[ 4u]="'_'"; static unsigned char _temp1250[ 4u]="'$'";
static unsigned char _temp1251[ 4u]="'<'"; static unsigned char _temp1252[ 4u]="'>'";
static unsigned char _temp1253[ 4u]="':'"; static unsigned char _temp1254[ 4u]="'.'";
static unsigned char _temp1255[ 4u]="'['"; static unsigned char _temp1256[ 4u]="']'";
static unsigned char _temp1257[ 4u]="'*'"; static unsigned char _temp1258[ 4u]="'@'";
static unsigned char _temp1259[ 4u]="'?'"; static unsigned char _temp1260[ 4u]="'+'";
static unsigned char _temp1261[ 4u]="'-'"; static unsigned char _temp1262[ 4u]="'&'";
static unsigned char _temp1263[ 4u]="'|'"; static unsigned char _temp1264[ 4u]="'^'";
static unsigned char _temp1265[ 4u]="'/'"; static unsigned char _temp1266[ 4u]="'%'";
static unsigned char _temp1267[ 4u]="'~'"; static unsigned char _temp1268[ 4u]="'!'";
static unsigned char _temp1269[ 5u]="prog"; static unsigned char _temp1270[ 17u]="translation_unit";
static unsigned char _temp1271[ 21u]="external_declaration"; static
unsigned char _temp1272[ 20u]="function_definition"; static unsigned char
_temp1273[ 21u]="function_definition2"; static unsigned char _temp1274[ 13u]="using_action";
static unsigned char _temp1275[ 15u]="unusing_action"; static unsigned char
_temp1276[ 17u]="namespace_action"; static unsigned char _temp1277[ 19u]="unnamespace_action";
static unsigned char _temp1278[ 12u]="declaration"; static unsigned char
_temp1279[ 17u]="declaration_list"; static unsigned char _temp1280[ 23u]="declaration_specifiers";
static unsigned char _temp1281[ 24u]="storage_class_specifier"; static
unsigned char _temp1282[ 15u]="attributes_opt"; static unsigned char _temp1283[
11u]="attributes"; static unsigned char _temp1284[ 15u]="attribute_list"; static
unsigned char _temp1285[ 10u]="attribute"; static unsigned char _temp1286[ 15u]="type_specifier";
static unsigned char _temp1287[ 5u]="kind"; static unsigned char _temp1288[ 15u]="type_qualifier";
static unsigned char _temp1289[ 15u]="enum_specifier"; static unsigned char
_temp1290[ 11u]="enum_field"; static unsigned char _temp1291[ 22u]="enum_declaration_list";
static unsigned char _temp1292[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1293[ 16u]="type_params_opt"; static unsigned char _temp1294[
16u]="struct_or_union"; static unsigned char _temp1295[ 24u]="struct_declaration_list";
static unsigned char _temp1296[ 25u]="struct_declaration_list0"; static
unsigned char _temp1297[ 21u]="init_declarator_list"; static unsigned char
_temp1298[ 22u]="init_declarator_list0"; static unsigned char _temp1299[ 16u]="init_declarator";
static unsigned char _temp1300[ 19u]="struct_declaration"; static unsigned char
_temp1301[ 25u]="specifier_qualifier_list"; static unsigned char _temp1302[ 23u]="struct_declarator_list";
static unsigned char _temp1303[ 24u]="struct_declarator_list0"; static
unsigned char _temp1304[ 18u]="struct_declarator"; static unsigned char
_temp1305[ 17u]="tunion_specifier"; static unsigned char _temp1306[ 18u]="tunion_or_xtunion";
static unsigned char _temp1307[ 17u]="tunionfield_list"; static unsigned char
_temp1308[ 18u]="tunionfield_scope"; static unsigned char _temp1309[ 12u]="tunionfield";
static unsigned char _temp1310[ 11u]="declarator"; static unsigned char
_temp1311[ 18u]="direct_declarator"; static unsigned char _temp1312[ 8u]="pointer";
static unsigned char _temp1313[ 13u]="pointer_char"; static unsigned char
_temp1314[ 8u]="rgn_opt"; static unsigned char _temp1315[ 4u]="rgn"; static
unsigned char _temp1316[ 20u]="type_qualifier_list"; static unsigned char
_temp1317[ 20u]="parameter_type_list"; static unsigned char _temp1318[ 9u]="type_var";
static unsigned char _temp1319[ 16u]="optional_effect"; static unsigned char
_temp1320[ 19u]="optional_rgn_order"; static unsigned char _temp1321[ 10u]="rgn_order";
static unsigned char _temp1322[ 16u]="optional_inject"; static unsigned char
_temp1323[ 11u]="effect_set"; static unsigned char _temp1324[ 14u]="atomic_effect";
static unsigned char _temp1325[ 11u]="region_set"; static unsigned char
_temp1326[ 15u]="parameter_list"; static unsigned char _temp1327[ 22u]="parameter_declaration";
static unsigned char _temp1328[ 16u]="identifier_list"; static unsigned char
_temp1329[ 17u]="identifier_list0"; static unsigned char _temp1330[ 12u]="initializer";
static unsigned char _temp1331[ 18u]="array_initializer"; static unsigned char
_temp1332[ 17u]="initializer_list"; static unsigned char _temp1333[ 12u]="designation";
static unsigned char _temp1334[ 16u]="designator_list"; static unsigned char
_temp1335[ 11u]="designator"; static unsigned char _temp1336[ 10u]="type_name";
static unsigned char _temp1337[ 14u]="any_type_name"; static unsigned char
_temp1338[ 15u]="type_name_list"; static unsigned char _temp1339[ 20u]="abstract_declarator";
static unsigned char _temp1340[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1341[ 10u]="statement"; static unsigned char _temp1342[ 18u]="labeled_statement";
static unsigned char _temp1343[ 21u]="expression_statement"; static
unsigned char _temp1344[ 19u]="compound_statement"; static unsigned char
_temp1345[ 16u]="block_item_list"; static unsigned char _temp1346[ 20u]="selection_statement";
static unsigned char _temp1347[ 15u]="switch_clauses"; static unsigned char
_temp1348[ 16u]="switchC_clauses"; static unsigned char _temp1349[ 20u]="iteration_statement";
static unsigned char _temp1350[ 15u]="jump_statement"; static unsigned char
_temp1351[ 8u]="pattern"; static unsigned char _temp1352[ 19u]="tuple_pattern_list";
static unsigned char _temp1353[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1354[ 14u]="field_pattern"; static unsigned char _temp1355[ 19u]="field_pattern_list";
static unsigned char _temp1356[ 20u]="field_pattern_list0"; static unsigned char
_temp1357[ 11u]="expression"; static unsigned char _temp1358[ 22u]="assignment_expression";
static unsigned char _temp1359[ 20u]="assignment_operator"; static unsigned char
_temp1360[ 23u]="conditional_expression"; static unsigned char _temp1361[ 20u]="constant_expression";
static unsigned char _temp1362[ 22u]="logical_or_expression"; static
unsigned char _temp1363[ 23u]="logical_and_expression"; static unsigned char
_temp1364[ 24u]="inclusive_or_expression"; static unsigned char _temp1365[ 24u]="exclusive_or_expression";
static unsigned char _temp1366[ 15u]="and_expression"; static unsigned char
_temp1367[ 20u]="equality_expression"; static unsigned char _temp1368[ 22u]="relational_expression";
static unsigned char _temp1369[ 17u]="shift_expression"; static unsigned char
_temp1370[ 20u]="additive_expression"; static unsigned char _temp1371[ 26u]="multiplicative_expression";
static unsigned char _temp1372[ 16u]="cast_expression"; static unsigned char
_temp1373[ 17u]="unary_expression"; static unsigned char _temp1374[ 15u]="unary_operator";
static unsigned char _temp1375[ 19u]="postfix_expression"; static unsigned char
_temp1376[ 19u]="primary_expression"; static unsigned char _temp1377[ 25u]="argument_expression_list";
static unsigned char _temp1378[ 26u]="argument_expression_list0"; static
unsigned char _temp1379[ 9u]="constant"; static unsigned char _temp1380[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 232u]={{ _temp1149, _temp1149, _temp1149
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
_temp1209, _temp1209, _temp1209 +  7u},{ _temp1210, _temp1210, _temp1210 +  7u},{
_temp1211, _temp1211, _temp1211 +  7u},{ _temp1212, _temp1212, _temp1212 +  8u},{
_temp1213, _temp1213, _temp1213 +  9u},{ _temp1214, _temp1214, _temp1214 +  6u},{
_temp1215, _temp1215, _temp1215 +  6u},{ _temp1216, _temp1216, _temp1216 +  6u},{
_temp1217, _temp1217, _temp1217 +  6u},{ _temp1218, _temp1218, _temp1218 +  7u},{
_temp1219, _temp1219, _temp1219 +  6u},{ _temp1220, _temp1220, _temp1220 +  11u},{
_temp1221, _temp1221, _temp1221 +  11u},{ _temp1222, _temp1222, _temp1222 +  11u},{
_temp1223, _temp1223, _temp1223 +  11u},{ _temp1224, _temp1224, _temp1224 +  11u},{
_temp1225, _temp1225, _temp1225 +  12u},{ _temp1226, _temp1226, _temp1226 +  13u},{
_temp1227, _temp1227, _temp1227 +  11u},{ _temp1228, _temp1228, _temp1228 +  11u},{
_temp1229, _temp1229, _temp1229 +  10u},{ _temp1230, _temp1230, _temp1230 +  9u},{
_temp1231, _temp1231, _temp1231 +  11u},{ _temp1232, _temp1232, _temp1232 +  12u},{
_temp1233, _temp1233, _temp1233 +  11u},{ _temp1234, _temp1234, _temp1234 +  17u},{
_temp1235, _temp1235, _temp1235 +  7u},{ _temp1236, _temp1236, _temp1236 +  19u},{
_temp1237, _temp1237, _temp1237 +  18u},{ _temp1238, _temp1238, _temp1238 +  9u},{
_temp1239, _temp1239, _temp1239 +  16u},{ _temp1240, _temp1240, _temp1240 +  18u},{
_temp1241, _temp1241, _temp1241 +  10u},{ _temp1242, _temp1242, _temp1242 +  4u},{
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
_temp1269, _temp1269, _temp1269 +  5u},{ _temp1270, _temp1270, _temp1270 +  17u},{
_temp1271, _temp1271, _temp1271 +  21u},{ _temp1272, _temp1272, _temp1272 +  20u},{
_temp1273, _temp1273, _temp1273 +  21u},{ _temp1274, _temp1274, _temp1274 +  13u},{
_temp1275, _temp1275, _temp1275 +  15u},{ _temp1276, _temp1276, _temp1276 +  17u},{
_temp1277, _temp1277, _temp1277 +  19u},{ _temp1278, _temp1278, _temp1278 +  12u},{
_temp1279, _temp1279, _temp1279 +  17u},{ _temp1280, _temp1280, _temp1280 +  23u},{
_temp1281, _temp1281, _temp1281 +  24u},{ _temp1282, _temp1282, _temp1282 +  15u},{
_temp1283, _temp1283, _temp1283 +  11u},{ _temp1284, _temp1284, _temp1284 +  15u},{
_temp1285, _temp1285, _temp1285 +  10u},{ _temp1286, _temp1286, _temp1286 +  15u},{
_temp1287, _temp1287, _temp1287 +  5u},{ _temp1288, _temp1288, _temp1288 +  15u},{
_temp1289, _temp1289, _temp1289 +  15u},{ _temp1290, _temp1290, _temp1290 +  11u},{
_temp1291, _temp1291, _temp1291 +  22u},{ _temp1292, _temp1292, _temp1292 +  26u},{
_temp1293, _temp1293, _temp1293 +  16u},{ _temp1294, _temp1294, _temp1294 +  16u},{
_temp1295, _temp1295, _temp1295 +  24u},{ _temp1296, _temp1296, _temp1296 +  25u},{
_temp1297, _temp1297, _temp1297 +  21u},{ _temp1298, _temp1298, _temp1298 +  22u},{
_temp1299, _temp1299, _temp1299 +  16u},{ _temp1300, _temp1300, _temp1300 +  19u},{
_temp1301, _temp1301, _temp1301 +  25u},{ _temp1302, _temp1302, _temp1302 +  23u},{
_temp1303, _temp1303, _temp1303 +  24u},{ _temp1304, _temp1304, _temp1304 +  18u},{
_temp1305, _temp1305, _temp1305 +  17u},{ _temp1306, _temp1306, _temp1306 +  18u},{
_temp1307, _temp1307, _temp1307 +  17u},{ _temp1308, _temp1308, _temp1308 +  18u},{
_temp1309, _temp1309, _temp1309 +  12u},{ _temp1310, _temp1310, _temp1310 +  11u},{
_temp1311, _temp1311, _temp1311 +  18u},{ _temp1312, _temp1312, _temp1312 +  8u},{
_temp1313, _temp1313, _temp1313 +  13u},{ _temp1314, _temp1314, _temp1314 +  8u},{
_temp1315, _temp1315, _temp1315 +  4u},{ _temp1316, _temp1316, _temp1316 +  20u},{
_temp1317, _temp1317, _temp1317 +  20u},{ _temp1318, _temp1318, _temp1318 +  9u},{
_temp1319, _temp1319, _temp1319 +  16u},{ _temp1320, _temp1320, _temp1320 +  19u},{
_temp1321, _temp1321, _temp1321 +  10u},{ _temp1322, _temp1322, _temp1322 +  16u},{
_temp1323, _temp1323, _temp1323 +  11u},{ _temp1324, _temp1324, _temp1324 +  14u},{
_temp1325, _temp1325, _temp1325 +  11u},{ _temp1326, _temp1326, _temp1326 +  15u},{
_temp1327, _temp1327, _temp1327 +  22u},{ _temp1328, _temp1328, _temp1328 +  16u},{
_temp1329, _temp1329, _temp1329 +  17u},{ _temp1330, _temp1330, _temp1330 +  12u},{
_temp1331, _temp1331, _temp1331 +  18u},{ _temp1332, _temp1332, _temp1332 +  17u},{
_temp1333, _temp1333, _temp1333 +  12u},{ _temp1334, _temp1334, _temp1334 +  16u},{
_temp1335, _temp1335, _temp1335 +  11u},{ _temp1336, _temp1336, _temp1336 +  10u},{
_temp1337, _temp1337, _temp1337 +  14u},{ _temp1338, _temp1338, _temp1338 +  15u},{
_temp1339, _temp1339, _temp1339 +  20u},{ _temp1340, _temp1340, _temp1340 +  27u},{
_temp1341, _temp1341, _temp1341 +  10u},{ _temp1342, _temp1342, _temp1342 +  18u},{
_temp1343, _temp1343, _temp1343 +  21u},{ _temp1344, _temp1344, _temp1344 +  19u},{
_temp1345, _temp1345, _temp1345 +  16u},{ _temp1346, _temp1346, _temp1346 +  20u},{
_temp1347, _temp1347, _temp1347 +  15u},{ _temp1348, _temp1348, _temp1348 +  16u},{
_temp1349, _temp1349, _temp1349 +  20u},{ _temp1350, _temp1350, _temp1350 +  15u},{
_temp1351, _temp1351, _temp1351 +  8u},{ _temp1352, _temp1352, _temp1352 +  19u},{
_temp1353, _temp1353, _temp1353 +  20u},{ _temp1354, _temp1354, _temp1354 +  14u},{
_temp1355, _temp1355, _temp1355 +  19u},{ _temp1356, _temp1356, _temp1356 +  20u},{
_temp1357, _temp1357, _temp1357 +  11u},{ _temp1358, _temp1358, _temp1358 +  22u},{
_temp1359, _temp1359, _temp1359 +  20u},{ _temp1360, _temp1360, _temp1360 +  23u},{
_temp1361, _temp1361, _temp1361 +  20u},{ _temp1362, _temp1362, _temp1362 +  22u},{
_temp1363, _temp1363, _temp1363 +  23u},{ _temp1364, _temp1364, _temp1364 +  24u},{
_temp1365, _temp1365, _temp1365 +  24u},{ _temp1366, _temp1366, _temp1366 +  15u},{
_temp1367, _temp1367, _temp1367 +  20u},{ _temp1368, _temp1368, _temp1368 +  22u},{
_temp1369, _temp1369, _temp1369 +  17u},{ _temp1370, _temp1370, _temp1370 +  20u},{
_temp1371, _temp1371, _temp1371 +  26u},{ _temp1372, _temp1372, _temp1372 +  16u},{
_temp1373, _temp1373, _temp1373 +  17u},{ _temp1374, _temp1374, _temp1374 +  15u},{
_temp1375, _temp1375, _temp1375 +  19u},{ _temp1376, _temp1376, _temp1376 +  19u},{
_temp1377, _temp1377, _temp1377 +  25u},{ _temp1378, _temp1378, _temp1378 +  26u},{
_temp1379, _temp1379, _temp1379 +  9u},{ _temp1380, _temp1380, _temp1380 +  20u}};
static short Cyc_yyr1[ 403u]={ 0, 120, 121, 121, 121, 121, 121, 121, 121, 122,
122, 123, 123, 123, 123, 124, 124, 125, 126, 127, 128, 129, 129, 129, 129, 130,
130, 131, 131, 131, 131, 131, 131, 131, 131, 132, 132, 132, 132, 132, 132, 132,
133, 133, 134, 135, 135, 136, 136, 136, 136, 136, 137, 137, 137, 137, 137, 137,
137, 137, 137, 137, 137, 137, 137, 137, 137, 137, 137, 138, 138, 139, 139, 139,
140, 140, 141, 141, 142, 142, 143, 143, 143, 143, 143, 144, 144, 144, 145, 145,
146, 147, 147, 148, 149, 149, 150, 150, 151, 152, 152, 152, 152, 153, 154, 154,
155, 155, 155, 156, 156, 156, 156, 157, 157, 158, 158, 158, 158, 159, 159, 159,
160, 160, 161, 161, 162, 162, 162, 162, 162, 162, 162, 162, 162, 162, 162, 162,
163, 163, 163, 163, 164, 164, 164, 164, 164, 165, 165, 166, 166, 167, 167, 168,
168, 168, 168, 169, 169, 170, 170, 171, 171, 172, 172, 173, 173, 174, 174, 175,
175, 175, 175, 176, 176, 177, 177, 178, 178, 178, 179, 180, 180, 181, 181, 182,
182, 182, 182, 183, 183, 183, 183, 184, 185, 185, 186, 186, 187, 187, 188, 188,
188, 188, 188, 189, 189, 190, 190, 190, 191, 191, 191, 191, 191, 191, 191, 191,
191, 191, 191, 191, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 193, 194,
194, 195, 195, 196, 196, 196, 196, 196, 196, 197, 197, 197, 197, 197, 198, 198,
198, 198, 198, 198, 199, 199, 199, 199, 200, 200, 200, 200, 200, 200, 200, 200,
200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201, 201, 202, 202,
202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 203, 203, 204, 204,
205, 205, 206, 207, 207, 208, 208, 209, 209, 210, 210, 210, 210, 210, 210, 210,
210, 210, 210, 210, 211, 211, 211, 211, 211, 211, 211, 212, 213, 213, 214, 214,
215, 215, 216, 216, 217, 217, 218, 218, 218, 219, 219, 219, 219, 219, 220, 220,
220, 221, 221, 221, 222, 222, 222, 222, 223, 223, 224, 224, 224, 224, 224, 224,
224, 224, 224, 224, 224, 224, 225, 225, 225, 226, 226, 226, 226, 226, 226, 226,
226, 226, 226, 226, 226, 226, 226, 227, 227, 227, 227, 227, 227, 227, 227, 227,
228, 229, 229, 230, 230, 230, 230, 231, 231}; static short Cyc_yyr2[ 403u]={ 0,
1, 2, 3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4, 3, 4, 2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2,
3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1, 0, 1, 6, 1, 3, 1, 1, 4, 4, 8, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4, 1, 1, 1, 1, 1, 5, 2, 1, 3, 1, 3, 4, 6,
6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3, 1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3,
6, 4, 3, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2, 6, 1, 2, 1, 3, 3, 4, 4, 5, 4, 4, 4, 2,
2, 1, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0, 1, 1, 1, 1, 2, 3, 5, 5, 7, 1, 3, 0, 2, 0, 2,
3, 5, 0, 1, 1, 3, 2, 3, 4, 1, 1, 3, 1, 3, 2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4, 8, 1,
2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 4, 3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4, 3,
5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2, 2, 3, 1, 2, 2, 3, 1, 2, 1, 2, 1, 2, 5,
7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3, 4, 5, 5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7,
8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1, 1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1, 3,
1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5, 5,
1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1, 3,
3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 6, 7, 9, 1, 1, 1, 1, 4, 3, 4, 3, 3, 3, 3,
2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4, 5, 1, 1, 3, 1, 1, 1, 1, 1, 1}; static
short Cyc_yydefact[ 794u]={ 0, 137, 35, 36, 37, 38, 40, 52, 54, 55, 56, 57, 58,
59, 60, 61, 71, 72, 73, 88, 89, 42, 0, 0, 41, 0, 0, 113, 114, 0, 401, 157, 402,
85, 0, 53, 0, 142, 143, 146, 1, 0, 9, 0, 0, 10, 0, 42, 42, 42, 62, 63, 0, 64, 0,
0, 124, 0, 147, 65, 126, 39, 0, 33, 43, 75, 286, 401, 282, 285, 284, 0, 280, 0,
0, 0, 0, 0, 180, 0, 287, 17, 19, 0, 0, 0, 66, 0, 0, 0, 0, 2, 0, 0, 0, 0, 21, 0,
93, 94, 96, 27, 29, 31, 85, 0, 85, 150, 0, 149, 85, 38, 0, 25, 0, 0, 11, 159, 0,
0, 135, 125, 42, 148, 136, 0, 0, 34, 0, 0, 294, 293, 283, 292, 24, 0, 0, 0, 0, 0,
42, 42, 198, 200, 0, 69, 70, 158, 205, 0, 127, 0, 0, 175, 0, 0, 400, 0, 0, 0, 0,
0, 0, 0, 0, 0, 397, 387, 398, 399, 0, 0, 0, 0, 370, 0, 368, 369, 0, 305, 318,
326, 328, 330, 332, 334, 336, 339, 344, 347, 350, 354, 0, 356, 371, 386, 385, 0,
3, 0, 5, 0, 22, 0, 0, 0, 12, 28, 30, 32, 84, 0, 90, 91, 0, 83, 85, 0, 111, 39, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 401, 233, 235, 0, 241, 237, 0, 239, 222,
223, 224, 0, 225, 226, 227, 0, 303, 26, 13, 96, 165, 181, 0, 0, 161, 159, 0, 0,
128, 0, 138, 0, 0, 78, 0, 76, 281, 296, 0, 295, 182, 0, 0, 294, 0, 201, 173, 0,
99, 101, 159, 0, 207, 199, 208, 68, 0, 87, 0, 86, 0, 177, 0, 179, 67, 0, 0, 364,
0, 320, 354, 0, 321, 322, 0, 0, 0, 0, 0, 0, 357, 358, 0, 0, 0, 0, 360, 361, 359,
144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 308, 309, 310, 311,
312, 313, 314, 315, 316, 317, 307, 0, 362, 0, 379, 380, 0, 0, 0, 389, 0, 0, 145,
18, 0, 20, 0, 95, 97, 184, 183, 14, 0, 80, 92, 0, 0, 103, 104, 106, 0, 110, 85,
119, 0, 0, 0, 0, 0, 0, 0, 273, 274, 275, 0, 0, 277, 0, 230, 231, 0, 0, 0, 0, 242,
238, 96, 240, 236, 234, 0, 166, 0, 0, 0, 172, 160, 167, 130, 0, 0, 0, 161, 132,
134, 133, 129, 151, 140, 139, 0, 48, 47, 0, 45, 0, 74, 0, 289, 0, 23, 290, 0, 0,
0, 0, 194, 298, 301, 0, 300, 0, 0, 0, 202, 100, 102, 0, 161, 0, 211, 0, 209, 159,
0, 0, 221, 204, 206, 176, 0, 0, 0, 185, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
388, 395, 0, 394, 327, 0, 329, 331, 333, 335, 337, 338, 342, 343, 340, 341, 345,
346, 348, 349, 351, 352, 353, 306, 377, 378, 373, 0, 375, 376, 0, 0, 0, 4, 6, 0,
325, 107, 98, 0, 0, 0, 112, 121, 120, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 272,
276, 0, 0, 0, 229, 0, 232, 0, 15, 304, 159, 0, 169, 0, 0, 0, 162, 131, 165, 153,
152, 141, 7, 0, 0, 0, 79, 77, 297, 197, 0, 299, 193, 195, 291, 0, 288, 203, 174,
216, 0, 210, 213, 0, 161, 0, 212, 0, 363, 0, 0, 186, 0, 190, 383, 384, 0, 0, 0,
0, 0, 0, 0, 355, 391, 0, 0, 374, 372, 392, 0, 0, 82, 105, 108, 81, 109, 122, 119,
119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 278, 0, 0, 16, 161, 0, 170, 168, 0, 161,
0, 0, 0, 0, 44, 46, 196, 302, 215, 218, 0, 220, 219, 214, 0, 0, 187, 191, 0, 0,
323, 324, 0, 393, 0, 396, 319, 390, 0, 118, 117, 243, 0, 248, 258, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 279, 0, 155, 171, 163, 154, 159, 0, 49, 50, 217, 365, 0,
192, 0, 0, 381, 0, 0, 0, 254, 0, 0, 260, 0, 0, 0, 268, 0, 0, 0, 0, 0, 0, 0, 0,
247, 228, 0, 161, 0, 0, 366, 0, 382, 0, 244, 0, 0, 0, 245, 259, 261, 262, 0, 270,
269, 0, 264, 0, 0, 0, 0, 248, 249, 164, 156, 0, 0, 0, 123, 0, 0, 246, 263, 271,
265, 266, 0, 0, 248, 250, 0, 188, 367, 254, 255, 267, 248, 251, 51, 254, 256,
248, 252, 257, 253, 0, 0, 0}; static short Cyc_yydefgoto[ 112u]={ 791, 40, 41,
42, 238, 43, 367, 44, 369, 45, 205, 46, 47, 63, 64, 436, 437, 48, 147, 49, 50,
267, 268, 51, 86, 52, 211, 212, 97, 98, 99, 213, 142, 379, 380, 381, 53, 54, 534,
535, 536, 55, 56, 57, 58, 122, 108, 432, 460, 59, 461, 423, 562, 415, 419, 420,
281, 259, 153, 77, 78, 477, 372, 478, 479, 448, 449, 143, 148, 149, 462, 288,
241, 242, 243, 244, 245, 246, 693, 743, 247, 248, 271, 272, 273, 451, 452, 453,
249, 250, 354, 179, 526, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
191, 192, 193, 194, 491, 492, 195, 196}; static short Cyc_yypact[ 794u]={ 1941,
- -32768, - -32768, - -32768, - -32768, - 57, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - 19, 154, 572, - -32768, 154, 105, -
-32768, - -32768, - 59, - -32768, 137, - -32768, 80, 107, - -32768, 112, 185,
200, - -32768, - -32768, 1830, - -32768, 292, 389, - -32768, 180, - 19, - 19, -
19, - -32768, - -32768, 308, - -32768, 182, 2377, 150, 16, 96, - -32768, 154,
211, 206, 2432, - -32768, 241, - -32768, 169, - -32768, - -32768, - -32768, 1012,
- -32768, 277, 264, 298, 1012, 300, 296, 314, 262, - -32768, - -32768, 4183, -
15, 4183, - -32768, 339, 1991, 3914, 3914, - -32768, 1830, 2051, 1830, 2051, -
-32768, 356, 333, - -32768, 2319, 2432, 2432, 2432, 80, 1991, 80, - -32768, 154,
- -32768, 271, 365, 1252, - -32768, 2377, 180, - -32768, 1771, 4183, 2858, -
-32768, 150, - 19, - -32768, - -32768, 2051, 357, - -32768, 154, 361, 1012, -
-32768, - -32768, - -32768, - -32768, 391, 3914, 42, 380, 238, - 19, - 19, 243,
- -32768, 134, - -32768, - -32768, - -32768, 378, 28, - -32768, 545, 387, -
-32768, 4021, 383, - -32768, 3914, 704, 396, 400, 402, 404, 407, 4062, 4062, -
-32768, - -32768, - -32768, - -32768, 1598, 412, 4128, 4128, - -32768, 4128, -
-32768, - -32768, 416, - -32768, - 45, 448, 406, 411, 409, 439, 76, 451, 423,
168, - -32768, 706, 4128, 368, - -32768, - -32768, 17, 436, - -32768, 442, -
-32768, 445, - -32768, 107, 2924, 2377, - -32768, - -32768, - -32768, - -32768,
458, 462, 1991, - -32768, 635, 478, 80, 154, 481, - -32768, 464, 106, 487, 2484,
491, 506, 485, 511, 2990, 2484, - 66, 2484, 2484, - 31, 495, - -32768, - -32768,
520, 1369, 1369, 180, 1369, - -32768, - -32768, - -32768, 536, - -32768, -
-32768, - -32768, 282, - -32768, - -32768, - -32768, 512, 535, - -32768, 118,
539, 530, 307, 543, 34, - -32768, 551, 126, 553, 3, 562, 555, 556, - -32768, -
-32768, 567, 576, - -32768, 310, 794, 1012, 4183, - -32768, 578, 586, 1991, 1991,
1026, 3056, 124, - -32768, 228, - -32768, 118, - -32768, 4183, - -32768, 2211, -
-32768, 483, - -32768, - -32768, 1991, 1598, - -32768, 1991, - -32768, - -32768,
2550, - -32768, 601, 3914, 2161, 671, 3914, 3914, 1598, - -32768, - -32768, 1369,
607, 450, 3914, - -32768, - -32768, - -32768, - -32768, 4128, 3914, 4128, 4128,
4128, 4128, 4128, 4128, 4128, 4128, 4128, 4128, 4128, 4128, 4128, 4128, 4128,
4128, 4128, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, 3914, - -32768, 5, - -32768, - -32768,
3122, 141, 3914, - -32768, 2781, 610, - -32768, - -32768, 1830, - -32768, 1830,
- -32768, - -32768, - -32768, - -32768, - -32768, 1991, - -32768, - -32768, 3914,
614, 615, - -32768, 609, 1991, - -32768, 80, 558, 3914, 619, 3914, 3914, 688,
1486, 627, - -32768, - -32768, - -32768, 312, 679, - -32768, 3188, - -32768, -
-32768, 2484, 633, 2484, 1710, - -32768, - -32768, 2319, - -32768, - -32768, -
-32768, 3914, - -32768, 1991, 626, 318, - -32768, - -32768, 622, - -32768, 637,
636, 4289, 530, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 126,
1830, - -32768, 639, 642, 638, 154, - -32768, 3914, - -32768, 1012, - -32768, -
-32768, 663, 3914, 1012, 123, - -32768, - -32768, - -32768, 653, 650, 654, - 4,
664, - -32768, - -32768, - -32768, 658, 530, 659, - -32768, 652, 228, 4241, 4183,
3254, - -32768, - -32768, 378, - -32768, 662, 665, 677, - -32768, - -32768, 197,
2924, 467, 669, 107, 666, 472, 670, 672, 673, 3955, - -32768, - -32768, 675, 694,
448, - 9, 406, 411, 409, 439, 76, 76, 451, 451, 451, 451, 423, 423, 168, 168, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 676, -
-32768, - -32768, 184, 219, 4183, - -32768, - -32768, 681, - -32768, - -32768, -
-32768, 635, 3914, 692, - -32768, - -32768, - -32768, 700, 154, 315, 494, 3914,
504, 508, 674, 3320, 3386, 324, - -32768, - -32768, 702, 710, 701, - -32768, 703,
- -32768, 2377, - -32768, - -32768, 711, 4183, - -32768, 712, 118, 707, - -32768,
- -32768, 176, - -32768, - -32768, - -32768, - -32768, 355, 713, 3, - -32768, -
-32768, - -32768, - -32768, 714, - -32768, - -32768, - -32768, - -32768, 833, -
-32768, - -32768, - -32768, - -32768, 715, - -32768, - -32768, 716, 530, 99, -
-32768, 717, 725, 724, 708, - -32768, 2627, - -32768, - -32768, - -32768, 2377,
1991, 704, 795, 725, 727, 2781, - -32768, - -32768, 3914, 3914, - -32768, -
-32768, - -32768, 2781, 164, - -32768, - -32768, - -32768, - -32768, - -32768,
70, 33, 558, 2484, 513, 732, 2484, 3914, 3452, 334, 3518, 369, 3584, 591, -
-32768, 734, 745, - -32768, 530, 64, - -32768, - -32768, 741, 530, 1991, 735,
737, 739, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 742, -
-32768, - -32768, - -32768, 744, 3914, - -32768, - -32768, 2924, 746, - -32768,
601, 736, - -32768, 237, - -32768, - -32768, - -32768, 748, - -32768, - -32768,
818, 753, 591, - -32768, 517, 2484, 524, 3650, 2484, 526, 3716, 3782, 373, 1012,
764, 755, - -32768, 2484, - -32768, - -32768, 749, - -32768, 711, 784, - -32768,
- -32768, - -32768, - -32768, 218, - -32768, 774, 1991, - -32768, 2704, 1991,
2484, 605, 778, 781, - -32768, 2484, 2484, 541, - -32768, 2484, 2484, 548, 2484,
565, 3848, - 29, 1369, - -32768, - -32768, 637, 530, 776, 3914, - -32768, 779, -
-32768, 569, - -32768, 3914, 772, 785, - -32768, - -32768, - -32768, - -32768,
2484, - -32768, - -32768, 2484, - -32768, 2484, 2484, 588, 3914, 971, - -32768,
- -32768, - -32768, 798, 323, 788, - -32768, 251, 1369, - -32768, - -32768, -
-32768, - -32768, - -32768, 2484, 321, 591, - -32768, 789, - -32768, - -32768,
1135, - -32768, - -32768, 971, - -32768, - -32768, 605, - -32768, 591, - -32768,
- -32768, - -32768, 888, 890, - -32768}; static short Cyc_yypgoto[ 112u]={ -
-32768, 27, - -32768, 583, - -32768, - -32768, - -32768, - -32768, - -32768, -
51, - 42, - 55, - -32768, - 25, - 49, 327, - -32768, 88, - -32768, 29, - -32768,
- -32768, 463, - -32768, - 22, - -32768, - 67, - -32768, - -32768, - -32768, 705,
693, 63, - -32768, - -32768, 376, - -32768, - -32768, 68, - -32768, - -32768, 8,
- 39, - 41, - -32768, - -32768, 851, - -32768, - 98, 25, - 106, - 395, 178, 347,
352, 623, - 308, - 82, - 258, 797, - -32768, - 201, - 157, - 349, - 255, -
-32768, 468, - 144, - 78, - 102, 98, - 126, - 195, - -32768, - -32768, - 40, -
161, - -32768, - 556, - 309, - -32768, - -32768, - 21, 645, - -32768, 338, -
-32768, - -32768, 208, 220, - -32768, - 145, - 355, - 149, 600, 589, 598, 599,
597, 364, 209, 367, 370, - 43, 525, - -32768, - -32768, - -32768, - 307, -
-32768, - -32768, 10}; static short Cyc_yytable[ 4391u]={ 115, 306, 79, 371, 113,
144, 152, 120, 127, 307, 60, 258, 303, 114, 520, 116, 261, 1, 121, 257, 434, 447,
101, 102, 103, 324, 317, 399, 391, 61, 565, 400, 65, 80, 398, 81, 401, 402, 532,
533, 756, 472, 87, 83, 60, 115, 207, 208, 209, 113, 129, 60, 516, 403, 100, 133,
60, 240, 137, 115, 206, 239, 106, 251, 110, 325, 586, 60, 91, 145, 124, 404, 120,
62, 252, 757, 146, 407, 408, 109, 410, 80, 210, 109, 215, 573, 80, 435, 218, 513,
413, 576, 291, 549, 583, 612, 514, 264, 427, 362, 30, 286, 60, 60, 60, 60, 32,
290, 1, 559, 296, 363, 141, 34, 141, 282, 283, 141, 216, 198, 199, 200, 201, 253,
715, 60, 364, 292, - 116, 320, 321, 293, 322, 292, 141, 60, 276, 428, 269, 277,
80, 332, 333, 16, 17, 18, 141, 141, 584, 355, 115, 151, 265, 425, 251, 487, 473,
556, 474, 295, 465, 60, 697, 658, 280, 374, 472, - 85, 214, 486, 465, 140, 85,
140, 620, 290, 140, 416, 334, 335, 151, 1, 85, 240, 240, 31, 240, 239, 239, 82,
239, 30, 388, 140, 384, 657, 107, 32, 292, 141, 455, 775, 659, 389, 34, 140, 140,
31, 550, 88, 552, 253, 417, 60, 471, 37, 38, 39, 783, 578, 84, 284, 382, 431, 60,
517, 788, 385, 445, 446, 285, 790, 518, 525, 37, 38, 39, 289, 30, 469, 287, 141,
62, 286, 32, 290, 696, 117, 409, 297, 60, 699, 118, 296, 482, 450, 119, 121, 140,
671, 414, 240, - 181, 292, 30, 239, 30, 674, - 181, 256, 32, 31, 32, 96, - 159,
214, 340, 34, 599, 89, - 159, 418, 107, 413, 341, 342, 80, 80, 37, 38, 39, 614,
597, 430, 90, 525, 598, 509, 510, 511, 140, 525, 87, 126, 60, 125, 60, 141, 524,
178, 197, 141, 141, 141, 615, 418, 530, 413, 616, 60, 62, 141, 735, 141, 152,
466, 447, 31, 141, 141, 467, 141, 710, 279, 468, 128, 711, 115, 760, 263, 284,
543, 141, 665, 275, 458, 459, 151, 131, 285, 413, 37, 38, 39, 115, 779, - 85,
151, 113, - 85, 590, 665, 151, 531, 85, 591, 140, 553, 589, 554, 140, 140, 140,
85, 130, 412, 217, 60, 318, 60, 140, 413, 140, 132, 525, 92, 93, 140, 140, 700,
140, 567, 30, 134, 522, 135, 523, 664, 32, 104, 256, 140, 105, 443, 141, 546,
424, 31, 624, 413, 136, 413, 141, 558, 625, 664, 469, 635, 777, 617, 413, 574,
413, 413, 373, 782, 577, 685, 356, 357, 358, 678, 203, 413, 681, 141, 397, 150,
214, 648, 649, 650, 280, 60, 141, 609, 214, 667, 269, 202, 641, 219, 80, 141,
266, 668, 665, 80, 646, 270, 568, 566, 688, 140, 707, 359, 727, 673, 413, 151,
786, 140, 413, 360, 361, 274, 789, 278, 151, 642, 302, 280, 94, 95, 1, 298, 299,
151, 717, 290, 602, 721, 60, 308, 140, 141, 141, 309, 115, 310, 731, 311, 251,
140, 312, 464, 330, 331, 318, 319, 664, 323, 140, 640, 336, 337, 480, 326, 740,
484, 327, 318, 329, 746, 747, 373, 328, 749, 750, 151, 752, 365, 485, 494, 338,
339, 382, 366, 60, 490, 368, 501, 502, 503, 504, 623, 1, 115, 489, 413, 141, 113,
375, 768, 140, 140, 769, 376, 770, 771, 450, 387, 206, 532, 533, 600, 413, 30,
758, 519, 604, 413, 383, 32, 512, 386, - 207, 781, 394, 490, 294, - 207, - 207,
373, 390, 418, 141, - 207, 392, 285, 393, 80, 626, 413, 733, 537, 774, 539, 540,
405, 544, 675, 628, 413, 395, 780, 629, 413, 204, 140, 66, 679, 413, 691, 692,
716, 413, 406, 785, 414, 490, 787, 718, 413, 722, 413, 490, 741, 742, 30, 739,
411, 141, 555, 422, 32, 1, 421, - 178, 748, 413, 426, 294, - 178, - 178, 140,
751, 413, 433, - 178, 439, 285, 440, 37, 38, 39, 67, 68, 429, 69, 70, 438, 32,
753, 413, 441, 666, 764, 299, 71, 728, 324, 72, 73, 240, 442, 141, 456, 239, 301,
74, 457, 304, 304, 75, 76, 772, 413, 593, 314, 315, 140, 676, 677, 499, 500, 483,
304, 304, 373, 304, 80, 240, 505, 506, 488, 239, 527, 507, 508, 151, 240, 521,
529, 528, 239, 538, 304, 541, 30, 545, 547, 551, 557, 240, 32, 561, 240, 239,
154, 155, 239, 34, 560, 563, 140, 569, 571, 141, 378, 570, 141, 156, 37, 38, 39,
627, 575, 580, 581, 632, 634, 582, 31, 159, 160, 585, 587, 161, 588, 594, 596,
163, 603, 595, 164, 165, 601, 607, 605, 606, 630, 737, 610, 613, 151, 618, 343,
344, 345, 346, 347, 348, 349, 350, 351, 352, 621, 30, 166, 167, 168, 169, 611,
32, 622, 636, 140, 305, 638, 140, 170, 353, 637, 256, 171, 639, 643, 661, 645,
662, 651, 172, 655, 656, 173, 174, 175, 373, 608, 669, 653, 176, 177, 660, 670,
680, 694, 373, 695, 698, 672, 66, 709, 701, 702, 373, 703, 682, 684, 704, 687,
705, 690, 708, 712, 713, 714, 732, 304, 730, 304, 304, 304, 304, 304, 304, 304,
304, 304, 304, 304, 304, 304, 304, 304, 304, 304, 729, 734, 706, 66, 736, 744,
745, 761, 766, 763, 30, 68, 767, 69, 70, 776, 32, 373, 778, 784, 792, 444, 793,
71, 481, 720, 72, 73, 724, 726, 652, 445, 446, 572, 74, 304, 619, 377, 75, 76,
370, 123, 759, 647, 644, 470, 260, 495, 579, 30, 68, 654, 69, 70, 454, 32, 493,
496, 498, 497, 0, 0, 71, 373, 0, 72, 73, 755, 0, 0, 445, 446, 0, 74, 0, 762, 0,
75, 76, 0, 0, 765, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 773, 304, 0, 0, 0,
0, 0, 304, 0, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 691, 692, 21, 154, 155, 220, 0, 221, 222, 223, 224, 225, 226, 227, 228,
22, 156, 23, 157, 229, 304, 158, 24, 230, 0, 0, 27, 28, 159, 160, 231, 232, 161,
29, 233, 162, 163, 0, 0, 164, 165, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 0, 0, 66, 0, 0, 0, 304, 234, 166, 167, 168, 169, 31, 32, 33, 22, 235,
112, 0, 0, 170, 0, 0, 35, 237, 0, 27, 28, 0, 0, 0, 172, 0, 29, 173, 174, 175, 0,
0, 0, 0, 176, 177, 0, 0, 0, 0, 0, 30, 68, 0, 69, 70, 0, 32, 0, 0, 0, 0, 254, 0,
71, 0, 0, 72, 73, 0, 31, 0, 33, 0, 256, 74, 0, 0, 284, 75, 76, 35, 36, 0, 304, 0,
0, 285, 0, 37, 38, 39, 304, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 741, 742, 21, 154, 155, 220, 0, 221, 222, 223, 224, 225, 226,
227, 228, 22, 156, 23, 157, 229, 0, 158, 24, 230, 0, 0, 27, 28, 159, 160, 231,
232, 161, 29, 233, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 234, 166, 167, 168, 169, 31, 32, 33, 0, 235, 112, 0,
0, 170, 0, 0, 35, 237, 0, 0, 0, 0, 0, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0,
176, 177, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
0, 0, 21, 154, 155, 220, 0, 221, 222, 223, 224, 225, 226, 227, 228, 22, 156, 23,
157, 229, 0, 158, 24, 230, 0, 0, 27, 28, 159, 160, 231, 232, 161, 29, 233, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 234, 166, 167, 168, 169, 31, 32, 33, 0, 235, 112, 236, 0, 170, 0, 0, 35, 237,
0, 0, 0, 0, 0, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 2, 3, 4, 111,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 154, 155, 220,
0, 221, 222, 223, 224, 225, 226, 227, 228, 22, 156, 23, 157, 229, 0, 158, 24,
230, 0, 0, 27, 28, 159, 160, 231, 232, 161, 29, 233, 162, 163, 0, 0, 164, 165, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 234, 166, 167, 168,
169, 31, 32, 33, 0, 235, 112, 0, 0, 170, 0, 0, 35, 237, 0, 0, 0, 0, 0, 0, 172, 0,
0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
156, 23, 157, 0, 0, 158, 24, 0, 0, 0, 27, 28, 159, 160, 0, 0, 161, 29, 0, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 166, 167, 168, 169, 31, 32, 33, 0, 542, 0, 0, 0, 170, 0, 0, 35, 237, 0, 0,
0, 0, 0, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
22, 156, 0, 157, 0, 0, 158, 0, 0, 0, 0, 27, 28, 159, 160, 0, 0, 161, 29, 0, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 166, 167, 168, 169, 31, 32, 33, 0, 0, 316, 0, 0, 170, 0, 0, 35, 237, 0, 0,
0, 0, 0, 0, 172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
22, 156, 0, 157, 0, 0, 158, 0, 0, 0, 0, 27, 28, 159, 160, 0, 0, 161, 29, 0, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 0, 30, 166, 167, 168, 169, 31, 32, 33, 0, 0, 0, 0, 0, 170, 22, 0, 35,
237, 0, 0, 0, 0, 0, 0, 172, 27, 28, 173, 174, 175, 0, 0, 29, 0, 176, 177, - 8, 1,
0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 254, 0,
21, 255, 0, 0, 0, 0, 31, 0, 33, 0, 256, 0, 0, 22, 0, 23, 0, 35, 36, 0, 24, 0, 25,
26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 0, 0, - 8, 0, 34,
0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28, 35, 36, 0, 0, 0,
29, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 31, 0, 33, 0, 0, 0, 0, 0, 22, 0, 23, 35,
36, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33,
0, 0, 0, - 8, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4,
111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 27, 28, 0, 1, 0, 0,
0, 29, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28, 35,
36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 254, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 256, 0, 0, 0, 294, -
159, 0, 35, 36, 0, 0, - 159, 0, 285, 0, 37, 38, 39, 2, 3, 4, 111, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0,
0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
21, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0, 112, 22, 204, 23, 0, 0, 35, 36, 24, 0, 0,
0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 22, 0,
112, 0, 0, 0, 0, 24, 35, 36, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 155, 220, 0, 221, 222, 223, 224,
225, 226, 227, 228, 31, 156, 33, 157, 229, 0, 158, 0, 230, 0, 0, 35, 36, 159,
160, 231, 232, 161, 0, 233, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 234, 166, 167, 168, 169, 0, 32, 154, 155,
235, 112, 0, 0, 170, 475, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174,
175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32,
0, 0, 0, 305, 476, 0, 170, 0, 0, 0, 171, 154, 155, 0, 445, 446, 0, 172, 0, 0,
173, 174, 175, 0, 156, 0, 157, 176, 177, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0,
161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 0, 0, 0, 305, 663, 0, 170, 0, 0,
0, 171, 154, 155, 0, 445, 446, 0, 172, 0, 0, 173, 174, 175, 0, 156, 0, 157, 176,
177, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0, 162, 163, 0, 0, 164, 165,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 0, 0, 0, 305, 738, 0, 170, 0, 0, 0, 171, 154, 155, 0, 445, 446, 0,
172, 0, 0, 173, 174, 175, 0, 156, 0, 157, 176, 177, 158, 0, 0, 0, 0, 0, 0, 159,
160, 0, 0, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 0, 0, 0, 305, 0, 0,
170, 0, 0, 0, 171, 154, 155, 0, 445, 446, 0, 172, 0, 0, 173, 174, 175, 0, 156, 0,
157, 176, 177, 158, 0, 0, 0, 0, 0, 0, 159, 160, 0, 0, 161, 0, 0, 162, 163, 0, 0,
164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166,
167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157,
262, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163,
0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 305, 0, 0, 170, 0, 0, 0, 171, 0, 0,
156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0,
162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 396, 0, 0, 0, 170, 0, 0, 0,
171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177,
161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 0, 0,
0, 171, 0, 0, 156, 0, 157, 463, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176,
177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170,
515, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160,
176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0,
0, 170, 548, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0,
159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155,
0, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157, 592, 172, 158, 0, 173, 174,
175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32,
154, 155, 631, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0,
173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 154, 155, 633, 0, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172,
158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164,
165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167,
168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 683, 0, 0, 171, 0, 0, 156, 0, 157, 0,
172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0,
164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166,
167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 686, 0, 0, 171, 0, 0, 156, 0,
157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 162,
163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 689, 0, 0, 0, 170, 0, 0, 0, 171, 0,
0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0,
0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 719, 0, 0,
171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176, 177,
161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170, 723,
0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160, 176,
177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0, 0, 170,
725, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0, 159, 160,
176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 154, 155, 0, 0, 0,
0, 170, 754, 0, 0, 171, 0, 0, 156, 0, 157, 0, 172, 158, 0, 173, 174, 175, 0, 0,
159, 160, 176, 177, 161, 0, 0, 162, 163, 0, 0, 164, 165, 0, 0, 0, 154, 155, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32, 159,
160, 0, 0, 161, 0, 170, 0, 163, 0, 171, 164, 165, 0, 0, 0, 0, 172, 0, 0, 173,
174, 175, 0, 0, 0, 0, 176, 177, 0, 0, 0, 0, 0, 30, 166, 167, 168, 169, 0, 32,
154, 155, 0, 608, 0, 0, 170, 0, 0, 0, 171, 0, 0, 156, 0, 0, 0, 172, 0, 0, 173,
174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 0, 163, 0, 0, 164, 165, 0, 0, 0,
154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 156, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 159, 160, 0, 0, 161, 0, 300, 0, 163, 0, 171, 164, 165, 0, 0, 0, 0,
172, 0, 0, 173, 174, 175, 0, 0, 0, 0, 176, 177, 0, 0, 0, 0, 0, 30, 166, 167, 168,
169, 0, 32, 154, 155, 0, 0, 0, 0, 313, 0, 0, 0, 171, 0, 0, 156, 0, 0, 0, 172, 0,
0, 173, 174, 175, 0, 0, 159, 160, 176, 177, 161, 0, 0, 0, 163, 0, 0, 164, 165, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 30, 166,
167, 168, 169, 0, 32, 0, 22, 0, 0, 0, 0, 170, 0, 0, 0, 171, 0, 27, 28, 0, 0, 0,
172, 0, 29, 173, 174, 175, 138, 0, 0, 0, 176, 177, 0, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0, 139, 22,
0, 0, 0, 0, 35, 36, 0, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0,
22, 0, 0, 0, 31, 0, 33, 0, 256, 0, 0, 27, 28, 0, 0, 35, 36, 0, 29, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 564, 0, 0, 0, 0, 0,
0, 0, 31, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 35, 36}; static short Cyc_yycheck[ 4391u]={
55, 158, 23, 204, 55, 83, 88, 56, 63, 158, 0, 117, 157, 55, 363, 55, 118, 1, 57,
117, 17, 276, 47, 48, 49, 70, 170, 93, 223, 86, 425, 97, 22, 23, 229, 25, 231,
232, 5, 6, 69, 299, 34, 102, 34, 100, 101, 102, 103, 100, 71, 41, 359, 84, 46,
76, 46, 112, 80, 114, 100, 112, 52, 114, 54, 110, 461, 57, 41, 84, 60, 102, 121,
92, 114, 104, 91, 238, 239, 54, 241, 71, 104, 58, 106, 440, 76, 84, 110, 84, 99,
446, 64, 400, 98, 104, 91, 122, 64, 82, 84, 142, 92, 93, 94, 95, 90, 111, 1, 417,
151, 94, 83, 97, 85, 140, 141, 88, 108, 92, 93, 94, 95, 115, 680, 115, 109, 99,
95, 172, 173, 103, 175, 99, 105, 125, 94, 103, 128, 97, 130, 65, 66, 17, 18, 19,
117, 118, 456, 192, 205, 88, 125, 259, 205, 316, 300, 415, 302, 151, 286, 151,
98, 64, 139, 205, 424, 97, 105, 313, 296, 83, 102, 85, 529, 111, 88, 59, 102,
103, 117, 1, 102, 238, 239, 89, 241, 238, 239, 84, 241, 84, 86, 105, 216, 590,
100, 90, 99, 170, 278, 757, 103, 97, 97, 117, 118, 89, 403, 97, 405, 203, 94,
203, 292, 108, 109, 110, 774, 96, 83, 97, 214, 264, 214, 84, 782, 217, 105, 106,
106, 787, 91, 378, 108, 109, 110, 103, 84, 288, 142, 212, 92, 284, 90, 111, 641,
97, 240, 151, 240, 646, 102, 294, 309, 276, 106, 296, 170, 608, 84, 316, 93, 99,
84, 316, 84, 103, 99, 93, 90, 89, 90, 93, 98, 212, 108, 97, 479, 94, 104, 256,
100, 99, 116, 117, 276, 277, 108, 109, 110, 107, 95, 264, 94, 440, 99, 340, 341,
342, 212, 446, 294, 97, 294, 94, 296, 278, 375, 89, 90, 282, 283, 284, 95, 290,
383, 99, 99, 309, 92, 292, 104, 294, 406, 97, 581, 89, 299, 300, 102, 302, 95,
95, 106, 94, 99, 392, 733, 119, 97, 392, 313, 598, 136, 282, 283, 284, 84, 106,
99, 108, 109, 110, 409, 104, 94, 294, 409, 97, 466, 616, 299, 385, 102, 467, 278,
409, 466, 409, 282, 283, 284, 102, 97, 93, 105, 367, 170, 369, 292, 99, 294, 85,
529, 93, 94, 299, 300, 647, 302, 432, 84, 93, 367, 99, 369, 598, 90, 91, 93, 313,
94, 93, 375, 93, 99, 89, 93, 99, 96, 99, 383, 95, 99, 616, 465, 93, 95, 521, 99,
442, 99, 99, 204, 104, 447, 93, 60, 61, 62, 626, 99, 99, 629, 406, 228, 98, 375,
84, 85, 86, 417, 433, 415, 488, 383, 604, 438, 93, 556, 86, 442, 424, 97, 604,
711, 447, 564, 98, 433, 432, 93, 375, 665, 97, 93, 612, 99, 406, 779, 383, 99,
105, 106, 84, 785, 97, 415, 557, 97, 456, 93, 94, 1, 98, 99, 424, 683, 111, 482,
686, 482, 97, 406, 466, 467, 97, 553, 97, 695, 97, 553, 415, 97, 285, 67, 68,
300, 97, 711, 95, 424, 553, 63, 64, 308, 69, 713, 311, 114, 313, 113, 718, 719,
305, 115, 722, 723, 466, 725, 95, 312, 325, 111, 112, 528, 95, 528, 319, 95, 332,
333, 334, 335, 535, 1, 602, 98, 99, 521, 602, 94, 748, 466, 467, 751, 95, 753,
754, 581, 97, 602, 5, 6, 98, 99, 84, 729, 361, 98, 99, 94, 90, 354, 94, 93, 772,
93, 359, 97, 98, 99, 363, 97, 560, 557, 104, 97, 106, 84, 581, 98, 99, 700, 387,
757, 389, 390, 104, 392, 623, 98, 99, 93, 766, 98, 99, 96, 521, 38, 98, 99, 22,
23, 98, 99, 97, 779, 84, 400, 782, 98, 99, 98, 99, 406, 22, 23, 84, 712, 95, 603,
413, 104, 90, 1, 98, 93, 98, 99, 98, 97, 98, 99, 557, 98, 99, 95, 104, 95, 106,
96, 108, 109, 110, 84, 85, 107, 87, 88, 99, 90, 98, 99, 98, 603, 98, 99, 97, 691,
70, 100, 101, 729, 99, 647, 99, 729, 154, 108, 95, 157, 158, 112, 113, 98, 99,
468, 164, 165, 603, 624, 625, 330, 331, 25, 172, 173, 479, 175, 691, 757, 336,
337, 98, 757, 93, 338, 339, 647, 766, 102, 104, 99, 766, 97, 192, 30, 84, 93, 42,
89, 97, 779, 90, 89, 782, 779, 25, 26, 782, 97, 111, 98, 647, 97, 99, 709, 104,
98, 712, 38, 108, 109, 110, 538, 84, 95, 99, 542, 543, 98, 89, 50, 51, 98, 98,
54, 107, 98, 84, 58, 97, 99, 61, 62, 98, 95, 99, 98, 97, 709, 98, 98, 712, 95,
71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 95, 84, 85, 86, 87, 88, 99, 90, 95, 94,
709, 94, 98, 712, 97, 96, 93, 93, 101, 103, 95, 84, 102, 102, 98, 108, 98, 98,
111, 112, 113, 598, 94, 25, 107, 118, 119, 107, 98, 94, 93, 608, 84, 89, 611, 38,
97, 99, 98, 616, 98, 630, 631, 98, 633, 98, 635, 98, 97, 28, 94, 99, 324, 95,
326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341,
342, 104, 85, 662, 38, 98, 95, 93, 99, 104, 98, 84, 85, 95, 87, 88, 85, 90, 665,
98, 98, 0, 95, 0, 97, 309, 685, 100, 101, 688, 689, 571, 105, 106, 438, 108, 378,
528, 212, 112, 113, 203, 58, 732, 564, 560, 290, 117, 326, 448, 84, 85, 581, 87,
88, 277, 90, 324, 327, 329, 328, - 1, - 1, 97, 711, - 1, 100, 101, 727, - 1, - 1,
105, 106, - 1, 108, - 1, 735, - 1, 112, 113, - 1, - 1, 741, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 756, 440, - 1, - 1, - 1, - 1,
- 1, 446, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
40, 41, 488, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, -
1, - 1, 61, 62, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
38, - 1, - 1, - 1, 529, 84, 85, 86, 87, 88, 89, 90, 91, 37, 93, 94, - 1, - 1, 97,
- 1, - 1, 100, 101, - 1, 48, 49, - 1, - 1, - 1, 108, - 1, 55, 111, 112, 113, - 1,
- 1, - 1, - 1, 118, 119, - 1, - 1, - 1, - 1, - 1, 84, 85, - 1, 87, 88, - 1, 90,
- 1, - 1, - 1, - 1, 81, - 1, 97, - 1, - 1, 100, 101, - 1, 89, - 1, 91, - 1, 93,
108, - 1, - 1, 97, 112, 113, 100, 101, - 1, 604, - 1, - 1, 106, - 1, 108, 109,
110, 612, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
- 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, - 1,
61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1, 93, 94, -
1, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111,
112, 113, - 1, - 1, - 1, - 1, 118, 119, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33,
34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52,
53, 54, 55, 56, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87,
88, 89, 90, 91, - 1, 93, 94, 95, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1,
- 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26,
27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, -
1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, - 1, 61, 62, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1, 93, 94, - 1, - 1, 97, - 1, -
1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, -
1, - 1, - 1, 118, 119, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 37, 38, 39, 40, - 1, - 1, 43, 44, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54,
55, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88,
89, 90, 91, - 1, 93, - 1, - 1, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1,
- 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1,
- 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1, - 1, 94, - 1, - 1, 97, - 1,
- 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1,
- 1, - 1, - 1, 118, 119, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, -
1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1,
57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, 84, 85, 86, 87, 88, 89, 90, 91, - 1, - 1, - 1,
- 1, - 1, 97, 37, - 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, 48, 49, 111,
112, 113, - 1, - 1, 55, - 1, 118, 119, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 81, - 1, 24, 84, - 1, - 1, - 1, - 1, 89,
- 1, 91, - 1, 93, - 1, - 1, 37, - 1, 39, - 1, 100, 101, - 1, 44, - 1, 46, 47, 48,
49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 84, - 1, - 1, - 1, - 1, 89, 90, 91, - 1, - 1, - 1, 95, - 1, 97, - 1,
- 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108, 109, 110, 0, 1, - 1, 3, 4, 5,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1,
- 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 84, - 1, - 1, 37, - 1, 89, 90, 91, - 1, - 1, - 1, - 1, -
1, 97, 48, 49, 100, 101, - 1, - 1, - 1, 55, - 1, - 1, 108, 109, 110, 1, - 1, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24,
- 1, - 1, - 1, - 1, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39, 100, 101,
- 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, - 1, - 1, - 1, - 1, 89, 90, 91, -
1, - 1, - 1, 95, - 1, 97, - 1, - 1, 100, 101, - 1, - 1, - 1, - 1, - 1, - 1, 108,
109, 110, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
37, - 1, - 1, - 1, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48, 49, - 1, 1, - 1, - 1, -
1, 55, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, - 1, - 1, 37, - 1, 89, 90,
91, - 1, - 1, - 1, - 1, - 1, 97, 48, 49, 100, 101, - 1, - 1, - 1, 55, - 1, - 1,
108, 109, 110, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 81, - 1, - 1, 84, - 1, - 1, - 1, - 1, 89, 90,
91, - 1, 93, - 1, - 1, - 1, 97, 98, - 1, 100, 101, - 1, - 1, 104, - 1, 106, - 1,
108, 109, 110, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37,
- 1, 39, - 1, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1,
55, - 1, - 1, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, 89, - 1, 91, - 1, -
1, 94, 37, 96, 39, - 1, - 1, 100, 101, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1,
- 1, - 1, 55, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, - 1,
91, 37, - 1, 94, - 1, - 1, - 1, - 1, 44, 100, 101, - 1, 48, 49, - 1, - 1, - 1, -
1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35,
36, 89, 38, 91, 40, 41, - 1, 43, - 1, 45, - 1, - 1, 100, 101, 50, 51, 52, 53, 54,
- 1, 56, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88,
- 1, 90, 25, 26, 93, 94, - 1, - 1, 97, 32, - 1, - 1, 101, - 1, - 1, 38, - 1, 40,
- 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57,
58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, - 1, -
1, - 1, 94, 95, - 1, 97, - 1, - 1, - 1, 101, 25, 26, - 1, 105, 106, - 1, 108, -
1, - 1, 111, 112, 113, - 1, 38, - 1, 40, 118, 119, 43, - 1, - 1, - 1, - 1, - 1,
- 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 84, 85, 86, 87, 88, - 1, 90, - 1, - 1, - 1, 94, 95, - 1, 97, - 1, - 1, -
1, 101, 25, 26, - 1, 105, 106, - 1, 108, - 1, - 1, 111, 112, 113, - 1, 38, - 1,
40, 118, 119, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1,
57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, -
1, - 1, - 1, 94, 95, - 1, 97, - 1, - 1, - 1, 101, 25, 26, - 1, 105, 106, - 1,
108, - 1, - 1, 111, 112, 113, - 1, 38, - 1, 40, 118, 119, 43, - 1, - 1, - 1, - 1,
- 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, - 1, - 1, - 1, 94, - 1, - 1, 97, - 1,
- 1, - 1, 101, 25, 26, - 1, 105, 106, - 1, 108, - 1, - 1, 111, 112, 113, - 1, 38,
- 1, 40, 118, 119, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1,
- 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1,
90, 25, 26, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40,
107, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57,
58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26,
- 1, 94, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43,
- 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1,
61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, 93, - 1, -
1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111,
112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1,
97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, 107, 108, 43, - 1, 111, 112, 113,
- 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, -
1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1,
50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1,
101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51,
118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84,
85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1,
- 1, 38, - 1, 40, 107, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119,
54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87,
88, - 1, 90, 25, 26, 93, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, -
1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, -
1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90,
25, 26, 93, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1,
108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, -
1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1,
- 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1,
111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61,
62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1,
- 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112,
113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, 93, - 1, - 1, - 1, 97, -
1, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1,
- 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, -
1, 101, - 1, - 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50,
51, 118, 119, 54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84,
85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1,
- 1, 38, - 1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119,
54, - 1, - 1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87,
88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, -
1, 40, - 1, 108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, -
1, 57, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90,
25, 26, - 1, - 1, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 38, - 1, 40, - 1,
108, 43, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1, - 1, 57, 58, -
1, - 1, 61, 62, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 38, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1, 90, 50, 51, - 1, -
1, 54, - 1, 97, - 1, 58, - 1, 101, 61, 62, - 1, - 1, - 1, - 1, 108, - 1, - 1,
111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, - 1, - 1, - 1, - 1, - 1, 84, 85, 86,
87, 88, - 1, 90, 25, 26, - 1, 94, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 38,
- 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, 50, 51, 118, 119, 54, - 1,
- 1, - 1, 58, - 1, - 1, 61, 62, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, - 1,
90, 50, 51, - 1, - 1, 54, - 1, 97, - 1, 58, - 1, 101, 61, 62, - 1, - 1, - 1, - 1,
108, - 1, - 1, 111, 112, 113, - 1, - 1, - 1, - 1, 118, 119, - 1, - 1, - 1, - 1,
- 1, 84, 85, 86, 87, 88, - 1, 90, 25, 26, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1,
101, - 1, - 1, 38, - 1, - 1, - 1, 108, - 1, - 1, 111, 112, 113, - 1, - 1, 50, 51,
118, 119, 54, - 1, - 1, - 1, 58, - 1, - 1, 61, 62, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 84, 85, 86, 87, 88, -
1, 90, - 1, 37, - 1, - 1, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, 48, 49, - 1, -
1, - 1, 108, - 1, 55, 111, 112, 113, 59, - 1, - 1, - 1, 118, 119, - 1, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 89, - 1, 91, - 1, - 1, 94, 37, - 1, - 1, - 1, - 1, 100, 101, - 1, - 1, -
1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 81, -
1, - 1, - 1, 37, - 1, - 1, - 1, 89, - 1, 91, - 1, 93, - 1, - 1, 48, 49, - 1, - 1,
100, 101, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 81, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 100,
101}; unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex(); static int
Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static int
Cyc_yynerrs= 0; struct _tuple18{ void* f1; void* f2; } ; struct _tuple19{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int yystate;
int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short* yyss=(
short*)({ unsigned int _temp3172= 10000u; short* _temp3173=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3172));{ unsigned int
_temp3174= _temp3172; unsigned int i; for( i= 0; i <  _temp3174; i ++){
_temp3173[ i]=( short) 0;}}; _temp3173;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3169= 10000u; void** _temp3170=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3169));{ unsigned int _temp3171= _temp3169;
unsigned int i; for( i= 0; i <  _temp3171; i ++){ _temp3170[ i]= Cyc_yylval;}};
_temp3170;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3166= 10000u; struct Cyc_Yyltype* _temp3167=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3166));{
unsigned int _temp3168= _temp3166; unsigned int i; for( i= 0; i <  _temp3168; i
++){ _temp3167[ i]= Cyc_yynewloc();}}; _temp3167;}); int yystacksize= 10000;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:
yyss[ _check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate;
if( yyssp_offset >=  yystacksize -  1){ if( yystacksize >=  10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 794u, yystate)]; if( yyn ==  - 32768){ goto
yydefault;} if( Cyc_yychar ==  - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar >  0? Cyc_yychar <= 
347: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 348u, Cyc_yychar)]:
232;} yyn += yychar1; if(( yyn <  0? 1: yyn >  4390)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4391u, yyn)] !=  yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4391u, yyn)]; if( yyn <  0){
if( yyn ==  - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
==  0){ goto yyerrlab;}} if( yyn ==  793){ return 0;} if( Cyc_yychar !=  0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus !=  0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 794u, yystate)]; if(
yyn ==  0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 403u, yyn)]; if( yylen >  0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset +  1) -  yylen)];} switch(
yyn){ case 1: _LL1381: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1382:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1384=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1384[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1385; _temp1385.tag= Cyc_DeclList_tok; _temp1385.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1385;});
_temp1384;}); break; case 3: _LL1383: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1387=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1387[ 0]=({ struct Cyc_DeclList_tok_struct _temp1388; _temp1388.tag= Cyc_DeclList_tok;
_temp1388.f1=({ struct Cyc_List_List* _temp1389=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1389->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1390=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1390->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1391=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1391[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1392; _temp1392.tag= Cyc_Absyn_Using_d;
_temp1392.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1392.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1392;}); _temp1391;}));
_temp1390->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1390;});
_temp1389->tl= 0; _temp1389;}); _temp1388;}); _temp1387;}); Cyc_Lex_leave_using();
break; case 4: _LL1386: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1394=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1394[ 0]=({ struct Cyc_DeclList_tok_struct _temp1395; _temp1395.tag= Cyc_DeclList_tok;
_temp1395.f1=({ struct Cyc_List_List* _temp1396=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1396->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1397=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1397->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1398=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1398[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1399; _temp1399.tag= Cyc_Absyn_Using_d;
_temp1399.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp1399.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1399;});
_temp1398;})); _temp1397->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1397;}); _temp1396->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1396;}); _temp1395;});
_temp1394;}); break; case 5: _LL1393: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1401=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1401[ 0]=({ struct Cyc_DeclList_tok_struct _temp1402; _temp1402.tag= Cyc_DeclList_tok;
_temp1402.f1=({ struct Cyc_List_List* _temp1403=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1403->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1404=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1404->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1405=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1405[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1406; _temp1406.tag=
Cyc_Absyn_Namespace_d; _temp1406.f1=({ struct _tagged_arr* _temp1407=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1407[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1407;});
_temp1406.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1406;}); _temp1405;})); _temp1404->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1404;}); _temp1403->tl= 0; _temp1403;}); _temp1402;}); _temp1401;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1400: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1409=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1409[ 0]=({ struct Cyc_DeclList_tok_struct _temp1410; _temp1410.tag= Cyc_DeclList_tok;
_temp1410.f1=({ struct Cyc_List_List* _temp1411=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1411->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1412=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1412->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1413=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1413[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1414; _temp1414.tag=
Cyc_Absyn_Namespace_d; _temp1414.f1=({ struct _tagged_arr* _temp1415=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1415[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1415;});
_temp1414.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1414;}); _temp1413;})); _temp1412->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1412;}); _temp1411->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1411;}); _temp1410;});
_temp1409;}); break; case 7: _LL1408: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1417=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1417[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1418; _temp1418.tag= Cyc_DeclList_tok; _temp1418.f1=({ struct Cyc_List_List*
_temp1419=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1419->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1420=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1420->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1421=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1421[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1422; _temp1422.tag= Cyc_Absyn_ExternC_d;
_temp1422.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1422;}); _temp1421;})); _temp1420->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1420;}); _temp1419->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1419;}); _temp1418;});
_temp1417;}); break; case 8: _LL1416: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1424=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1424[ 0]=({ struct Cyc_DeclList_tok_struct _temp1425; _temp1425.tag= Cyc_DeclList_tok;
_temp1425.f1= 0; _temp1425;}); _temp1424;}); break; case 9: _LL1423: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1427=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1427[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1428; _temp1428.tag= Cyc_DeclList_tok; _temp1428.f1=({ struct Cyc_List_List*
_temp1429=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1429->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1430=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1430[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1431; _temp1431.tag= Cyc_Absyn_Fn_d;
_temp1431.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1431;}); _temp1430;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1429->tl=
0; _temp1429;}); _temp1428;}); _temp1427;}); break; case 10: _LL1426: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1432: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1434=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1434[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1435; _temp1435.tag= Cyc_FnDecl_tok; _temp1435.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1435;}); _temp1434;}); break; case 12: _LL1433: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1437=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1437[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1438; _temp1438.tag= Cyc_FnDecl_tok;
_temp1438.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1439=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1439->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1439;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1438;}); _temp1437;}); break; case 13: _LL1436: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1441=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1441[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1442; _temp1442.tag= Cyc_FnDecl_tok;
_temp1442.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1442;}); _temp1441;}); break; case 14: _LL1440: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1444=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1444[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1445; _temp1445.tag= Cyc_FnDecl_tok;
_temp1445.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1446=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1446->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1446;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1445;}); _temp1444;}); break; case 15: _LL1443: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1448=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1448[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1449; _temp1449.tag= Cyc_FnDecl_tok;
_temp1449.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1450=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1450->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1450;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1449;}); _temp1448;}); break; case 16: _LL1447: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1452=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1452[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1453; _temp1453.tag= Cyc_FnDecl_tok;
_temp1453.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1454=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1454->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1454;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1453;}); _temp1452;}); break; case 17: _LL1451: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1455:
Cyc_Lex_leave_using(); break; case 19: _LL1456: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1458=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1458[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1458;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1457: Cyc_Lex_leave_namespace(); break; case 21: _LL1459: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1461=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1461[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1462; _temp1462.tag= Cyc_DeclList_tok; _temp1462.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1462;}); _temp1461;}); break; case 22: _LL1460: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1464=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1464[ 0]=({ struct Cyc_DeclList_tok_struct _temp1465; _temp1465.tag= Cyc_DeclList_tok;
_temp1465.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1465;}); _temp1464;}); break; case 23: _LL1463: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1467=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1467[ 0]=({ struct Cyc_DeclList_tok_struct _temp1468; _temp1468.tag= Cyc_DeclList_tok;
_temp1468.f1=({ struct Cyc_List_List* _temp1469=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1469->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1469->tl=
0; _temp1469;}); _temp1468;}); _temp1467;}); break; case 24: _LL1466: { struct
Cyc_List_List* _temp1471= 0;{ struct Cyc_List_List* _temp1472= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0;
_temp1472 !=  0; _temp1472=(( struct Cyc_List_List*) _check_null( _temp1472))->tl){
struct _tagged_arr* _temp1473=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1472))->hd; struct _tuple1* _temp1474=({ struct _tuple1*
_temp1477=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1477->f1=
Cyc_Absyn_rel_ns_null; _temp1477->f2= _temp1473; _temp1477;}); struct Cyc_Absyn_Vardecl*
_temp1475= Cyc_Absyn_new_vardecl( _temp1474, Cyc_Absyn_wildtyp( 0), 0);
_temp1471=({ struct Cyc_List_List* _temp1476=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1476->hd=( void*) _temp1475; _temp1476->tl=
_temp1471; _temp1476;});}} _temp1471=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1471); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1478=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1478[ 0]=({ struct Cyc_DeclList_tok_struct _temp1479; _temp1479.tag= Cyc_DeclList_tok;
_temp1479.f1=({ struct Cyc_List_List* _temp1480=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1480->hd=( void*) Cyc_Absyn_letv_decl(
_temp1471, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1480->tl= 0; _temp1480;}); _temp1479;});
_temp1478;}); break;} case 25: _LL1470: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1481:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1483=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1483[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1484; _temp1484.tag= Cyc_DeclList_tok; _temp1484.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1484;});
_temp1483;}); break; case 27: _LL1482: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1486=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1486[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1487; _temp1487.tag= Cyc_DeclSpec_tok;
_temp1487.f1=({ struct Cyc_Parse_Declaration_spec* _temp1488=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1488->sc=({ struct
Cyc_Core_Opt* _temp1489=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp1489->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1489;});
_temp1488->tq= Cyc_Absyn_empty_tqual(); _temp1488->type_specs= 0; _temp1488->is_inline=
0; _temp1488->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1488;}); _temp1487;});
_temp1486;}); break; case 28: _LL1485: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc !=  0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1491=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1491[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1492; _temp1492.tag= Cyc_DeclSpec_tok; _temp1492.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1493=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1493->sc=({ struct Cyc_Core_Opt* _temp1494=( struct Cyc_Core_Opt*)
_cycalloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1494->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1494;});
_temp1493->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1493->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1493->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1493->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1493;}); _temp1492;}); _temp1491;}); break; case 29: _LL1490: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1496=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1496[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1497; _temp1497.tag= Cyc_DeclSpec_tok; _temp1497.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1498=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1498->sc= 0; _temp1498->tq= Cyc_Absyn_empty_tqual(); _temp1498->type_specs=({
struct Cyc_List_List* _temp1499=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1499->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1499->tl= 0;
_temp1499;}); _temp1498->is_inline= 0; _temp1498->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1498;});
_temp1497;}); _temp1496;}); break; case 30: _LL1495: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1501=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1501[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1502; _temp1502.tag= Cyc_DeclSpec_tok;
_temp1502.f1=({ struct Cyc_Parse_Declaration_spec* _temp1503=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1503->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1503->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1503->type_specs=({ struct Cyc_List_List* _temp1504=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1504->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1504->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1504;});
_temp1503->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1503->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1503;}); _temp1502;}); _temp1501;}); break; case 31: _LL1500: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1506=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1506[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1507; _temp1507.tag= Cyc_DeclSpec_tok; _temp1507.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1508=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1508->sc= 0; _temp1508->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1508->type_specs=
0; _temp1508->is_inline= 0; _temp1508->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1508;});
_temp1507;}); _temp1506;}); break; case 32: _LL1505: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1510=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1510[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1511; _temp1511.tag= Cyc_DeclSpec_tok;
_temp1511.f1=({ struct Cyc_Parse_Declaration_spec* _temp1512=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1512->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1512->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1512->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1512->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1512->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1512;}); _temp1511;}); _temp1510;}); break; case 33: _LL1509: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1514=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1514[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1515; _temp1515.tag= Cyc_DeclSpec_tok; _temp1515.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1516=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1516->sc= 0; _temp1516->tq= Cyc_Absyn_empty_tqual(); _temp1516->type_specs=
0; _temp1516->is_inline= 1; _temp1516->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1516;});
_temp1515;}); _temp1514;}); break; case 34: _LL1513: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1518=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1518[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1519; _temp1519.tag= Cyc_DeclSpec_tok;
_temp1519.f1=({ struct Cyc_Parse_Declaration_spec* _temp1520=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1520->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1520->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1520->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1520->is_inline=
1; _temp1520->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1520;}); _temp1519;}); _temp1518;}); break; case 35: _LL1517: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1522=( struct Cyc_StorageClass_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1522[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1523; _temp1523.tag= Cyc_StorageClass_tok;
_temp1523.f1= Cyc_Parse_Auto_sc; _temp1523;}); _temp1522;}); break; case 36:
_LL1521: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1525=( struct
Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1525[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1526; _temp1526.tag=
Cyc_StorageClass_tok; _temp1526.f1= Cyc_Parse_Register_sc; _temp1526;});
_temp1525;}); break; case 37: _LL1524: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1528=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1528[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1529; _temp1529.tag= Cyc_StorageClass_tok; _temp1529.f1= Cyc_Parse_Static_sc;
_temp1529;}); _temp1528;}); break; case 38: _LL1527: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1531=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1531[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1532; _temp1532.tag= Cyc_StorageClass_tok; _temp1532.f1= Cyc_Parse_Extern_sc;
_temp1532;}); _temp1531;}); break; case 39: _LL1530: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1534=( struct Cyc_StorageClass_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1534[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1535; _temp1535.tag= Cyc_StorageClass_tok;
_temp1535.f1= Cyc_Parse_ExternC_sc; _temp1535;}); _temp1534;}); break; case 40:
_LL1533: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1537=( struct
Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1537[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1538; _temp1538.tag=
Cyc_StorageClass_tok; _temp1538.f1= Cyc_Parse_Typedef_sc; _temp1538;});
_temp1537;}); break; case 41: _LL1536: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1540=( struct Cyc_StorageClass_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1540[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1541; _temp1541.tag= Cyc_StorageClass_tok; _temp1541.f1= Cyc_Parse_Abstract_sc;
_temp1541;}); _temp1540;}); break; case 42: _LL1539: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1543=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1543[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1544; _temp1544.tag=
Cyc_AttributeList_tok; _temp1544.f1= 0; _temp1544;}); _temp1543;}); break; case
43: _LL1542: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1545: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1547=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1547[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1548; _temp1548.tag=
Cyc_AttributeList_tok; _temp1548.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1548;});
_temp1547;}); break; case 45: _LL1546: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1550=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1550[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1551; _temp1551.tag=
Cyc_AttributeList_tok; _temp1551.f1=({ struct Cyc_List_List* _temp1552=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1552->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1552->tl= 0; _temp1552;}); _temp1551;}); _temp1550;});
break; case 46: _LL1549: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1554=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1554[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1555; _temp1555.tag=
Cyc_AttributeList_tok; _temp1555.f1=({ struct Cyc_List_List* _temp1556=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1556->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1556->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1556;}); _temp1555;});
_temp1554;}); break; case 47: _LL1553: { struct _tagged_arr _temp1558= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1558, sizeof( unsigned char)) >  4?*(( const unsigned char*)
_check_unknown_subscript( _temp1558, sizeof( unsigned char), 0)) == '_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1558, sizeof( unsigned char),
1)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1558,
sizeof( unsigned char),( int)( _get_arr_size( _temp1558, sizeof( unsigned char))
-  2))) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1558,
sizeof( unsigned char),( int)( _get_arr_size( _temp1558, sizeof( unsigned char))
-  3))) == '_': 0){ _temp1558=( struct _tagged_arr) Cyc_Std_substring( _temp1558,
2, _get_arr_size( _temp1558, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0;
i <  16u; ++ i){ if( Cyc_Std_strcmp( _temp1558,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1559=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1559[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1560; _temp1560.tag= Cyc_Attribute_tok; _temp1560.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1560;}); _temp1559;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1561=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1561[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1562; _temp1562.tag= Cyc_Attribute_tok; _temp1562.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1562;}); _temp1561;});} break;}} case
48: _LL1557: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1564=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1564[ 0]=({ struct Cyc_Attribute_tok_struct _temp1565; _temp1565.tag= Cyc_Attribute_tok;
_temp1565.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1565;}); _temp1564;});
break; case 49: _LL1563: { struct _tagged_arr _temp1567= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1570; int _temp1571; struct _tuple15* _temp1568= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1570=*
_temp1568; _LL1572: _temp1571= _temp1570.f2; goto _LL1569; _LL1569: { void* a;
if( Cyc_Std_zstrcmp( _temp1567, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1567, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1571 <=  0? 1: _temp1571 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 1 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1573=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1573[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1574; _temp1574.tag= Cyc_Absyn_Regparm_att; _temp1574.f1= _temp1571;
_temp1574;}); _temp1573;});} else{ if( Cyc_Std_zstrcmp( _temp1567, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1567, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1571 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1571;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1575=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1575[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1576; _temp1576.tag= Cyc_Absyn_Aligned_att; _temp1576.f1= _temp1571;
_temp1576;}); _temp1575;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1577=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1577[ 0]=({ struct Cyc_Attribute_tok_struct _temp1578; _temp1578.tag= Cyc_Attribute_tok;
_temp1578.f1=( void*) a; _temp1578;}); _temp1577;}); break;}} case 50: _LL1566: {
struct _tagged_arr _temp1580= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1581= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1580, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1580, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1582=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1582[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1583; _temp1583.tag=
Cyc_Absyn_Section_att; _temp1583.f1= _temp1581; _temp1583;}); _temp1582;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1584=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1584[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1585; _temp1585.tag= Cyc_Attribute_tok; _temp1585.f1=(
void*) a; _temp1585;}); _temp1584;}); break;} case 51: _LL1579: { struct
_tagged_arr _temp1587= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1588= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1591; struct _tuple15 _temp1589=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1592:
_temp1591= _temp1589.f2; goto _LL1590; _LL1590: { int _temp1595; struct _tuple15
_temp1593=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1596: _temp1595= _temp1593.f2; goto _LL1594; _LL1594: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1587, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1587, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1588, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1597=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1597[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1598; _temp1598.tag=
Cyc_Absyn_Format_att; _temp1598.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1598.f2= _temp1591; _temp1598.f3= _temp1595; _temp1598;}); _temp1597;});}
else{ if( Cyc_Std_zstrcmp( _temp1588, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1599=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1599[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1600; _temp1600.tag=
Cyc_Absyn_Format_att; _temp1600.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1600.f2= _temp1591; _temp1600.f3= _temp1595; _temp1600;}); _temp1599;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1601=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1601[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1602; _temp1602.tag= Cyc_Attribute_tok; _temp1602.f1=(
void*) a; _temp1602;}); _temp1601;}); break;}}} case 52: _LL1586: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1604=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1604[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1605; _temp1605.tag= Cyc_TypeSpecifier_tok;
_temp1605.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1605;});
_temp1604;}); break; case 53: _LL1603: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1607=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1607[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1608; _temp1608.tag=
Cyc_TypeSpecifier_tok; _temp1608.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1608;}); _temp1607;}); break; case 54: _LL1606:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1610=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1610[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1611; _temp1611.tag= Cyc_TypeSpecifier_tok;
_temp1611.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1611;});
_temp1610;}); break; case 55: _LL1609: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1613=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1613[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1614; _temp1614.tag=
Cyc_TypeSpecifier_tok; _temp1614.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1615=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1615[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1616; _temp1616.tag=
Cyc_Parse_Short_spec; _temp1616.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1616;});
_temp1615;})); _temp1614;}); _temp1613;}); break; case 56: _LL1612: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1618=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1618[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1619; _temp1619.tag= Cyc_TypeSpecifier_tok;
_temp1619.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1619;});
_temp1618;}); break; case 57: _LL1617: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1621=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1621[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1622; _temp1622.tag=
Cyc_TypeSpecifier_tok; _temp1622.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1623=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1623[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1624; _temp1624.tag= Cyc_Parse_Long_spec;
_temp1624.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1624;}); _temp1623;})); _temp1622;}); _temp1621;});
break; case 58: _LL1620: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1626=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1626[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1627; _temp1627.tag=
Cyc_TypeSpecifier_tok; _temp1627.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1627;}); _temp1626;}); break; case 59: _LL1625:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1629=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1629[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1630; _temp1630.tag= Cyc_TypeSpecifier_tok;
_temp1630.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1630;});
_temp1629;}); break; case 60: _LL1628: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1632=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1632[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1633; _temp1633.tag=
Cyc_TypeSpecifier_tok; _temp1633.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1634=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1634[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1635; _temp1635.tag=
Cyc_Parse_Signed_spec; _temp1635.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1635;});
_temp1634;})); _temp1633;}); _temp1632;}); break; case 61: _LL1631: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1637=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1637[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1638; _temp1638.tag= Cyc_TypeSpecifier_tok;
_temp1638.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1639=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1639[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1640; _temp1640.tag= Cyc_Parse_Unsigned_spec; _temp1640.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1640;});
_temp1639;})); _temp1638;}); _temp1637;}); break; case 62: _LL1636: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1641:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1642: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1643: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1645=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1645[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1646; _temp1646.tag=
Cyc_TypeSpecifier_tok; _temp1646.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1646;});
_temp1645;}); break; case 66: _LL1644: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1648=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1648[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1649; _temp1649.tag=
Cyc_TypeSpecifier_tok; _temp1649.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1650=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1650[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1651; _temp1651.tag= Cyc_Absyn_TypedefType;
_temp1651.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1651.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1651.f3= 0;
_temp1651;}); _temp1650;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1649;});
_temp1648;}); break; case 67: _LL1647: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1653=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1653[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1654; _temp1654.tag=
Cyc_TypeSpecifier_tok; _temp1654.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1655=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1655[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1656; _temp1656.tag= Cyc_Absyn_TupleType;
_temp1656.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1656;});
_temp1655;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1654;}); _temp1653;}); break; case 68:
_LL1652: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1658=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1658[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1659; _temp1659.tag=
Cyc_TypeSpecifier_tok; _temp1659.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1660=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1660[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1661; _temp1661.tag= Cyc_Absyn_RgnHandleType;
_temp1661.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1661;}); _temp1660;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1659;}); _temp1658;}); break; case 69: _LL1657: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1663=( struct Cyc_Kind_tok_struct*) _cycalloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1663[ 0]=({ struct Cyc_Kind_tok_struct _temp1664; _temp1664.tag= Cyc_Kind_tok;
_temp1664.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1664;});
_temp1663;}); break; case 70: _LL1662: { struct _tagged_arr* _temp1668; void*
_temp1670; struct _tuple1 _temp1666=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1671: _temp1670=
_temp1666.f1; goto _LL1669; _LL1669: _temp1668= _temp1666.f2; goto _LL1667;
_LL1667: if( _temp1670 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1672=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1672[ 0]=({ struct Cyc_Kind_tok_struct
_temp1673; _temp1673.tag= Cyc_Kind_tok; _temp1673.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1668, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1673;}); _temp1672;}); break;} case 71:
_LL1665: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1675=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1675[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1676; _temp1676.tag= Cyc_TypeQual_tok;
_temp1676.f1=({ struct Cyc_Absyn_Tqual _temp1677; _temp1677.q_const= 1;
_temp1677.q_volatile= 0; _temp1677.q_restrict= 0; _temp1677;}); _temp1676;});
_temp1675;}); break; case 72: _LL1674: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1679=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1679[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1680; _temp1680.tag= Cyc_TypeQual_tok; _temp1680.f1=({ struct Cyc_Absyn_Tqual
_temp1681; _temp1681.q_const= 0; _temp1681.q_volatile= 1; _temp1681.q_restrict=
0; _temp1681;}); _temp1680;}); _temp1679;}); break; case 73: _LL1678: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1683=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1683[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1684; _temp1684.tag= Cyc_TypeQual_tok;
_temp1684.f1=({ struct Cyc_Absyn_Tqual _temp1685; _temp1685.q_const= 0;
_temp1685.q_volatile= 0; _temp1685.q_restrict= 1; _temp1685;}); _temp1684;});
_temp1683;}); break; case 74: _LL1682: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1687=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1687[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1688; _temp1688.tag=
Cyc_TypeSpecifier_tok; _temp1688.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1689=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1689[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1690; _temp1690.tag= Cyc_Parse_Decl_spec;
_temp1690.f1=({ struct Cyc_Absyn_Decl* _temp1691=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1691->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1692=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1692[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1693; _temp1693.tag= Cyc_Absyn_Enum_d; _temp1693.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1694=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1694->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1694->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1694->fields=({
struct Cyc_Core_Opt* _temp1695=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1695->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1695;});
_temp1694;}); _temp1693;}); _temp1692;})); _temp1691->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1691;}); _temp1690;}); _temp1689;})); _temp1688;}); _temp1687;}); break;
case 75: _LL1686: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1697=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1697[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1698; _temp1698.tag=
Cyc_TypeSpecifier_tok; _temp1698.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1699=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1699[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1700; _temp1700.tag= Cyc_Absyn_EnumType;
_temp1700.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1700.f2= 0; _temp1700;}); _temp1699;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1698;}); _temp1697;}); break; case 76: _LL1696: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1702=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1702[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1703; _temp1703.tag= Cyc_Enumfield_tok;
_temp1703.f1=({ struct Cyc_Absyn_Enumfield* _temp1704=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1704->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1704->tag= 0;
_temp1704->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1704;});
_temp1703;}); _temp1702;}); break; case 77: _LL1701: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1706=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1706[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1707; _temp1707.tag= Cyc_Enumfield_tok;
_temp1707.f1=({ struct Cyc_Absyn_Enumfield* _temp1708=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1708->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1708->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1708->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1708;});
_temp1707;}); _temp1706;}); break; case 78: _LL1705: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1710=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1710[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1711; _temp1711.tag=
Cyc_EnumfieldList_tok; _temp1711.f1=({ struct Cyc_List_List* _temp1712=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1712->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1712->tl= 0; _temp1712;}); _temp1711;}); _temp1710;});
break; case 79: _LL1709: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1714=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1714[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1715; _temp1715.tag=
Cyc_EnumfieldList_tok; _temp1715.f1=({ struct Cyc_List_List* _temp1716=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1716->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1716->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1716;}); _temp1715;});
_temp1714;}); break; case 80: _LL1713: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1718: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1720=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1720[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1721; _temp1721.tag=
Cyc_Absyn_AnonStructType; _temp1721.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1721;});
_temp1720;}); break; case Cyc_Parse_Union_su: _LL1719: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1723=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1723[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1724; _temp1724.tag= Cyc_Absyn_AnonUnionType; _temp1724.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1724;});
_temp1723;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1725=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1725[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1726; _temp1726.tag=
Cyc_TypeSpecifier_tok; _temp1726.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1727=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1727[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1728; _temp1728.tag= Cyc_Parse_Type_spec;
_temp1728.f1=( void*) t; _temp1728.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1728;});
_temp1727;})); _temp1726;}); _temp1725;}); break;} case 81: _LL1717: { struct
Cyc_List_List* _temp1730=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1731: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1733=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1733->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1733;}), _temp1730,({ struct Cyc_Core_Opt*
_temp1734=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1734->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1734;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1732: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1736=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1736->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1736;}),
_temp1730,({ struct Cyc_Core_Opt* _temp1737=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1737->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1737;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1738=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1738[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1739; _temp1739.tag=
Cyc_TypeSpecifier_tok; _temp1739.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1740=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1740[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1741; _temp1741.tag= Cyc_Parse_Decl_spec;
_temp1741.f1= d; _temp1741;}); _temp1740;})); _temp1739;}); _temp1738;}); break;}
case 82: _LL1729: { struct Cyc_List_List* _temp1743=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1744: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1746=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1746->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1746;}), _temp1743,({ struct Cyc_Core_Opt*
_temp1747=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1747->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1747;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1745: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1749=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1749->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1749;}),
_temp1743,({ struct Cyc_Core_Opt* _temp1750=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1750->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1750;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1751=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1751[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1752; _temp1752.tag=
Cyc_TypeSpecifier_tok; _temp1752.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1753=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1753[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1754; _temp1754.tag= Cyc_Parse_Decl_spec;
_temp1754.f1= d; _temp1754;}); _temp1753;})); _temp1752;}); _temp1751;}); break;}
case 83: _LL1742: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1756: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1758=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1758[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1759; _temp1759.tag=
Cyc_TypeSpecifier_tok; _temp1759.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1760=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1760[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1761; _temp1761.tag= Cyc_Absyn_StructType;
_temp1761.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1761.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1761.f3= 0;
_temp1761;}); _temp1760;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1759;});
_temp1758;}); break; case Cyc_Parse_Union_su: _LL1757: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1763=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1763[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1764; _temp1764.tag= Cyc_TypeSpecifier_tok;
_temp1764.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1765=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1765[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1766; _temp1766.tag= Cyc_Absyn_UnionType;
_temp1766.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1766.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1766.f3= 0;
_temp1766;}); _temp1765;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1764;});
_temp1763;}); break;} break; case 84: _LL1755: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1768: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1770=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1770[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1771; _temp1771.tag=
Cyc_TypeSpecifier_tok; _temp1771.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1772=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1772[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1773; _temp1773.tag= Cyc_Absyn_StructType;
_temp1773.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1773.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1773.f3= 0;
_temp1773;}); _temp1772;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1771;});
_temp1770;}); break; case Cyc_Parse_Union_su: _LL1769: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1775=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1775[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1776; _temp1776.tag= Cyc_TypeSpecifier_tok;
_temp1776.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1777=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1777[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1778; _temp1778.tag= Cyc_Absyn_UnionType;
_temp1778.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1778.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1778.f3= 0;
_temp1778;}); _temp1777;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1776;});
_temp1775;}); break;} break; case 85: _LL1767: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1780=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1780[ 0]=({ struct Cyc_TypeList_tok_struct _temp1781; _temp1781.tag= Cyc_TypeList_tok;
_temp1781.f1= 0; _temp1781;}); _temp1780;}); break; case 86: _LL1779: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1783=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1783[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1784; _temp1784.tag= Cyc_TypeList_tok; _temp1784.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1784;});
_temp1783;}); break; case 87: _LL1782:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1786=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1786[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1787; _temp1787.tag= Cyc_TypeList_tok; _temp1787.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1787;});
_temp1786;}); break; case 88: _LL1785: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1789=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1789[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1790; _temp1790.tag= Cyc_StructOrUnion_tok; _temp1790.f1= Cyc_Parse_Struct_su;
_temp1790;}); _temp1789;}); break; case 89: _LL1788: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1792=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1792[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1793; _temp1793.tag= Cyc_StructOrUnion_tok; _temp1793.f1= Cyc_Parse_Union_su;
_temp1793;}); _temp1792;}); break; case 90: _LL1791: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1795=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1795[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1796; _temp1796.tag= Cyc_StructFieldDeclList_tok; _temp1796.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1796;});
_temp1795;}); break; case 91: _LL1794: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1798=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1798[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1799; _temp1799.tag= Cyc_StructFieldDeclListList_tok; _temp1799.f1=({
struct Cyc_List_List* _temp1800=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1800->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1800->tl= 0;
_temp1800;}); _temp1799;}); _temp1798;}); break; case 92: _LL1797: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1802=( struct Cyc_StructFieldDeclListList_tok_struct*)
_cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1802[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1803; _temp1803.tag= Cyc_StructFieldDeclListList_tok;
_temp1803.f1=({ struct Cyc_List_List* _temp1804=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1804->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1804->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1804;}); _temp1803;}); _temp1802;}); break;
case 93: _LL1801: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1806=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1806[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1807; _temp1807.tag=
Cyc_InitDeclList_tok; _temp1807.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1807;});
_temp1806;}); break; case 94: _LL1805: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1809=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1809[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1810; _temp1810.tag=
Cyc_InitDeclList_tok; _temp1810.f1=({ struct Cyc_List_List* _temp1811=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1811->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1811->tl= 0; _temp1811;}); _temp1810;}); _temp1809;});
break; case 95: _LL1808: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1813=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1813[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1814; _temp1814.tag=
Cyc_InitDeclList_tok; _temp1814.f1=({ struct Cyc_List_List* _temp1815=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1815->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1815->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1815;});
_temp1814;}); _temp1813;}); break; case 96: _LL1812: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1817=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1817[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1818; _temp1818.tag= Cyc_InitDecl_tok;
_temp1818.f1=({ struct _tuple14* _temp1819=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1819->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1819->f2= 0;
_temp1819;}); _temp1818;}); _temp1817;}); break; case 97: _LL1816: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1821=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1821[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1822; _temp1822.tag= Cyc_InitDecl_tok; _temp1822.f1=({ struct _tuple14*
_temp1823=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1823->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1823->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1823;});
_temp1822;}); _temp1821;}); break; case 98: _LL1820: { struct _tuple17 _temp1827;
struct Cyc_List_List* _temp1828; struct Cyc_List_List* _temp1830; struct Cyc_Absyn_Tqual
_temp1832; struct _tuple17* _temp1825= Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1827=*
_temp1825; _LL1833: _temp1832= _temp1827.f1; goto _LL1831; _LL1831: _temp1830=
_temp1827.f2; goto _LL1829; _LL1829: _temp1828= _temp1827.f3; goto _LL1826;
_LL1826: { struct Cyc_List_List* _temp1836; struct Cyc_List_List* _temp1838;
struct _tuple0 _temp1834=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1839: _temp1838= _temp1834.f1; goto _LL1837; _LL1837:
_temp1836= _temp1834.f2; goto _LL1835; _LL1835: { void* _temp1840= Cyc_Parse_speclist2typ(
_temp1830, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1841=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1832, _temp1840, _temp1838, _temp1828), _temp1836);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1842=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1842[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1843; _temp1843.tag=
Cyc_StructFieldDeclList_tok; _temp1843.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1841); _temp1843;}); _temp1842;}); break;}}}
case 99: _LL1824: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1845=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1845[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1846; _temp1846.tag=
Cyc_QualSpecList_tok; _temp1846.f1=({ struct _tuple17* _temp1847=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1847->f1= Cyc_Absyn_empty_tqual();
_temp1847->f2=({ struct Cyc_List_List* _temp1848=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1848->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1848->tl=
0; _temp1848;}); _temp1847->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1847;}); _temp1846;});
_temp1845;}); break; case 100: _LL1844: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1850=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1850[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1851; _temp1851.tag=
Cyc_QualSpecList_tok; _temp1851.f1=({ struct _tuple17* _temp1852=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1852->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1852->f2=({
struct Cyc_List_List* _temp1853=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1853->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1853->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1853;}); _temp1852->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1852;});
_temp1851;}); _temp1850;}); break; case 101: _LL1849: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1855=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1855[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1856; _temp1856.tag= Cyc_QualSpecList_tok;
_temp1856.f1=({ struct _tuple17* _temp1857=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1857->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1857->f2= 0;
_temp1857->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1857;}); _temp1856;}); _temp1855;}); break; case
102: _LL1854: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1859=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1859[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1860; _temp1860.tag=
Cyc_QualSpecList_tok; _temp1860.f1=({ struct _tuple17* _temp1861=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1861->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1861->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1861->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1861;});
_temp1860;}); _temp1859;}); break; case 103: _LL1858: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1863=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1863[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1864; _temp1864.tag= Cyc_InitDeclList_tok;
_temp1864.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1864;}); _temp1863;}); break; case 104: _LL1862: yyval=(
void*)({ struct Cyc_InitDeclList_tok_struct* _temp1866=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1866[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1867; _temp1867.tag= Cyc_InitDeclList_tok;
_temp1867.f1=({ struct Cyc_List_List* _temp1868=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1868->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1868->tl= 0;
_temp1868;}); _temp1867;}); _temp1866;}); break; case 105: _LL1865: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp1870=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1870[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1871; _temp1871.tag= Cyc_InitDeclList_tok;
_temp1871.f1=({ struct Cyc_List_List* _temp1872=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1872->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1872->tl=
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1872;}); _temp1871;}); _temp1870;}); break; case 106:
_LL1869: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp1874=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1874[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1875; _temp1875.tag= Cyc_InitDecl_tok; _temp1875.f1=({ struct _tuple14*
_temp1876=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1876->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1876->f2= 0; _temp1876;}); _temp1875;}); _temp1874;});
break; case 107: _LL1873: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1878=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1878[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1879; _temp1879.tag= Cyc_InitDecl_tok;
_temp1879.f1=({ struct _tuple14* _temp1880=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1880->f1=({ struct Cyc_Parse_Declarator*
_temp1881=( struct Cyc_Parse_Declarator*) _cycalloc( sizeof( struct Cyc_Parse_Declarator));
_temp1881->id=({ struct _tuple1* _temp1882=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp1882->f1= Cyc_Absyn_rel_ns_null; _temp1882->f2=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1882;}); _temp1881->tms= 0; _temp1881;});
_temp1880->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1880;}); _temp1879;});
_temp1878;}); break; case 108: _LL1877: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1884=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1884[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1885; _temp1885.tag= Cyc_InitDecl_tok;
_temp1885.f1=({ struct _tuple14* _temp1886=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1886->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1886->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1886;}); _temp1885;}); _temp1884;}); break; case
109: _LL1883: { struct Cyc_List_List* _temp1888=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1889=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1889[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1890; _temp1890.tag=
Cyc_TypeSpecifier_tok; _temp1890.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1891=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1891[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1892; _temp1892.tag= Cyc_Parse_Decl_spec;
_temp1892.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1888,({
struct Cyc_Core_Opt* _temp1893=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1893->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1893;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1892;}); _temp1891;})); _temp1890;}); _temp1889;}); break;} case 110:
_LL1887: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1895=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1895[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1896; _temp1896.tag=
Cyc_TypeSpecifier_tok; _temp1896.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1897=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1897[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1898; _temp1898.tag= Cyc_Absyn_TunionType;
_temp1898.f1=({ struct Cyc_Absyn_TunionInfo _temp1899; _temp1899.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1900=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1900[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1901; _temp1901.tag= Cyc_Absyn_UnknownTunion;
_temp1901.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1902; _temp1902.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1902.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1902;});
_temp1901;}); _temp1900;})); _temp1899.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1899.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1899;}); _temp1898;}); _temp1897;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1896;}); _temp1895;}); break; case 111: _LL1894: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1904=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1904[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1905; _temp1905.tag= Cyc_TypeSpecifier_tok;
_temp1905.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1906=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1906[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1907; _temp1907.tag=
Cyc_Absyn_TunionType; _temp1907.f1=({ struct Cyc_Absyn_TunionInfo _temp1908;
_temp1908.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1909=( struct Cyc_Absyn_UnknownTunion_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1909[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1910; _temp1910.tag= Cyc_Absyn_UnknownTunion; _temp1910.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1911; _temp1911.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1911.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1911;}); _temp1910;}); _temp1909;})); _temp1908.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1908.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp1908;}); _temp1907;}); _temp1906;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1905;}); _temp1904;}); break; case 112:
_LL1903: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1913=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1913[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1914; _temp1914.tag=
Cyc_TypeSpecifier_tok; _temp1914.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1915=( struct Cyc_Absyn_TunionFieldType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1915[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1916; _temp1916.tag= Cyc_Absyn_TunionFieldType;
_temp1916.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1917; _temp1917.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp1918=( struct
Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
_temp1918[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct _temp1919; _temp1919.tag=
Cyc_Absyn_UnknownTunionfield; _temp1919.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp1920; _temp1920.tunion_name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1920.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1920.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1920;});
_temp1919;}); _temp1918;})); _temp1917.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1917;}); _temp1916;});
_temp1915;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1914;}); _temp1913;}); break; case 113:
_LL1912: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1922=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1922[ 0]=({ struct
Cyc_Bool_tok_struct _temp1923; _temp1923.tag= Cyc_Bool_tok; _temp1923.f1= 0;
_temp1923;}); _temp1922;}); break; case 114: _LL1921: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1925=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1925[ 0]=({ struct Cyc_Bool_tok_struct
_temp1926; _temp1926.tag= Cyc_Bool_tok; _temp1926.f1= 1; _temp1926;}); _temp1925;});
break; case 115: _LL1924: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1928=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1928[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1929; _temp1929.tag= Cyc_TunionFieldList_tok; _temp1929.f1=({ struct Cyc_List_List*
_temp1930=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1930->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1930->tl= 0;
_temp1930;}); _temp1929;}); _temp1928;}); break; case 116: _LL1927: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp1932=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1932[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1933; _temp1933.tag= Cyc_TunionFieldList_tok;
_temp1933.f1=({ struct Cyc_List_List* _temp1934=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1934->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1934->tl=
0; _temp1934;}); _temp1933;}); _temp1932;}); break; case 117: _LL1931: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1936=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1936[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1937; _temp1937.tag= Cyc_TunionFieldList_tok;
_temp1937.f1=({ struct Cyc_List_List* _temp1938=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1938->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1938->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1938;}); _temp1937;}); _temp1936;}); break; case 118:
_LL1935: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1940=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp1940[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp1941; _temp1941.tag=
Cyc_TunionFieldList_tok; _temp1941.f1=({ struct Cyc_List_List* _temp1942=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1942->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1942->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1942;}); _temp1941;});
_temp1940;}); break; case 119: _LL1939: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1944=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1944[ 0]=({ struct Cyc_Scope_tok_struct _temp1945; _temp1945.tag= Cyc_Scope_tok;
_temp1945.f1=( void*)(( void*) Cyc_Absyn_Public); _temp1945;}); _temp1944;});
break; case 120: _LL1943: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1947=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1947[ 0]=({ struct Cyc_Scope_tok_struct _temp1948; _temp1948.tag= Cyc_Scope_tok;
_temp1948.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp1948;}); _temp1947;});
break; case 121: _LL1946: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1950=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1950[ 0]=({ struct Cyc_Scope_tok_struct _temp1951; _temp1951.tag= Cyc_Scope_tok;
_temp1951.f1=( void*)(( void*) Cyc_Absyn_Static); _temp1951;}); _temp1950;});
break; case 122: _LL1949: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp1953=( struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp1953[ 0]=({ struct Cyc_TunionField_tok_struct _temp1954; _temp1954.tag= Cyc_TunionField_tok;
_temp1954.f1=({ struct Cyc_Absyn_Tunionfield* _temp1955=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1955->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1955->tvs= 0;
_temp1955->typs= 0; _temp1955->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1955->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1955;}); _temp1954;}); _temp1953;}); break; case 123:
_LL1952: { struct Cyc_List_List* _temp1957=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp1958=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp1959=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp1959[ 0]=({ struct
Cyc_TunionField_tok_struct _temp1960; _temp1960.tag= Cyc_TunionField_tok;
_temp1960.f1=({ struct Cyc_Absyn_Tunionfield* _temp1961=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1961->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1961->tvs=
_temp1958; _temp1961->typs= _temp1957; _temp1961->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1961->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp1961;}); _temp1960;}); _temp1959;}); break;}
case 124: _LL1956: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 125: _LL1962: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1964=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1964[ 0]=({ struct Cyc_Declarator_tok_struct _temp1965; _temp1965.tag= Cyc_Declarator_tok;
_temp1965.f1=({ struct Cyc_Parse_Declarator* _temp1966=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1966->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp1966->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp1966;});
_temp1965;}); _temp1964;}); break; case 126: _LL1963: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp1968=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp1968[ 0]=({ struct
Cyc_Declarator_tok_struct _temp1969; _temp1969.tag= Cyc_Declarator_tok;
_temp1969.f1=({ struct Cyc_Parse_Declarator* _temp1970=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1970->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1970->tms= 0;
_temp1970;}); _temp1969;}); _temp1968;}); break; case 127: _LL1967: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 128:
_LL1971: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp1973=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1973[ 0]=({ struct Cyc_Declarator_tok_struct _temp1974; _temp1974.tag= Cyc_Declarator_tok;
_temp1974.f1=({ struct Cyc_Parse_Declarator* _temp1975=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1975->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp1975->tms=({ struct Cyc_List_List* _temp1976=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1976->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp1976->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp1976;}); _temp1975;}); _temp1974;});
_temp1973;}); break; case 129: _LL1972: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1978=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1978[ 0]=({ struct Cyc_Declarator_tok_struct _temp1979; _temp1979.tag= Cyc_Declarator_tok;
_temp1979.f1=({ struct Cyc_Parse_Declarator* _temp1980=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1980->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp1980->tms=({ struct Cyc_List_List* _temp1981=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1981->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp1982=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1982[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1983; _temp1983.tag= Cyc_Absyn_ConstArray_mod;
_temp1983.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1983;}); _temp1982;})); _temp1981->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp1981;}); _temp1980;}); _temp1979;}); _temp1978;}); break; case 130: _LL1977: {
struct _tuple16 _temp1987; struct Cyc_List_List* _temp1988; struct Cyc_Core_Opt*
_temp1990; struct Cyc_Absyn_VarargInfo* _temp1992; int _temp1994; struct Cyc_List_List*
_temp1996; struct _tuple16* _temp1985= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1987=*
_temp1985; _LL1997: _temp1996= _temp1987.f1; goto _LL1995; _LL1995: _temp1994=
_temp1987.f2; goto _LL1993; _LL1993: _temp1992= _temp1987.f3; goto _LL1991;
_LL1991: _temp1990= _temp1987.f4; goto _LL1989; _LL1989: _temp1988= _temp1987.f5;
goto _LL1986; _LL1986: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1998=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1998[ 0]=({ struct Cyc_Declarator_tok_struct _temp1999; _temp1999.tag= Cyc_Declarator_tok;
_temp1999.f1=({ struct Cyc_Parse_Declarator* _temp2000=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2000->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2000->tms=({ struct Cyc_List_List* _temp2001=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2001->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2002=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2002[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2003; _temp2003.tag= Cyc_Absyn_Function_mod;
_temp2003.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2004=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2004[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2005; _temp2005.tag= Cyc_Absyn_WithTypes;
_temp2005.f1= _temp1996; _temp2005.f2= _temp1994; _temp2005.f3= _temp1992;
_temp2005.f4= _temp1990; _temp2005.f5= _temp1988; _temp2005;}); _temp2004;}));
_temp2003;}); _temp2002;})); _temp2001->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2001;});
_temp2000;}); _temp1999;}); _temp1998;}); break;} case 131: _LL1984: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2007=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2007[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2008; _temp2008.tag= Cyc_Declarator_tok;
_temp2008.f1=({ struct Cyc_Parse_Declarator* _temp2009=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2009->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2009->tms=({ struct Cyc_List_List* _temp2010=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2010->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2011=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2011[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2012; _temp2012.tag= Cyc_Absyn_Function_mod;
_temp2012.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2013=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2013[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2014; _temp2014.tag= Cyc_Absyn_WithTypes;
_temp2014.f1= 0; _temp2014.f2= 0; _temp2014.f3= 0; _temp2014.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2014.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2014;}); _temp2013;})); _temp2012;}); _temp2011;}));
_temp2010->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2010;}); _temp2009;}); _temp2008;});
_temp2007;}); break; case 132: _LL2006: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2016=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2016[ 0]=({ struct Cyc_Declarator_tok_struct _temp2017; _temp2017.tag= Cyc_Declarator_tok;
_temp2017.f1=({ struct Cyc_Parse_Declarator* _temp2018=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2018->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2018->tms=({ struct Cyc_List_List* _temp2019=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2019->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2020=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2020[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2021; _temp2021.tag= Cyc_Absyn_Function_mod;
_temp2021.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2022=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2022[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2023; _temp2023.tag= Cyc_Absyn_NoTypes;
_temp2023.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2023.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2023;});
_temp2022;})); _temp2021;}); _temp2020;})); _temp2019->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2019;}); _temp2018;}); _temp2017;}); _temp2016;}); break; case 133: _LL2015: {
struct Cyc_List_List* _temp2025=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2026=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2026[ 0]=({ struct Cyc_Declarator_tok_struct _temp2027; _temp2027.tag= Cyc_Declarator_tok;
_temp2027.f1=({ struct Cyc_Parse_Declarator* _temp2028=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2028->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2028->tms=({ struct Cyc_List_List* _temp2029=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2029->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2030=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2030[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2031; _temp2031.tag= Cyc_Absyn_TypeParams_mod;
_temp2031.f1= _temp2025; _temp2031.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2031.f3=
0; _temp2031;}); _temp2030;})); _temp2029->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2029;});
_temp2028;}); _temp2027;}); _temp2026;}); break;} case 134: _LL2024:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2033=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2034=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2034[ 0]=({ struct Cyc_Declarator_tok_struct _temp2035; _temp2035.tag= Cyc_Declarator_tok;
_temp2035.f1=({ struct Cyc_Parse_Declarator* _temp2036=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2036->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2036->tms=({ struct Cyc_List_List* _temp2037=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2037->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2038=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2038[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2039; _temp2039.tag= Cyc_Absyn_TypeParams_mod;
_temp2039.f1= _temp2033; _temp2039.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2039.f3=
0; _temp2039;}); _temp2038;})); _temp2037->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2037;});
_temp2036;}); _temp2035;}); _temp2034;}); break;} case 135: _LL2032: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2041=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2041[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2042; _temp2042.tag= Cyc_Declarator_tok;
_temp2042.f1=({ struct Cyc_Parse_Declarator* _temp2043=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2043->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2043->tms=({ struct Cyc_List_List* _temp2044=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2044->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2045=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2045[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2046; _temp2046.tag= Cyc_Absyn_Attributes_mod;
_temp2046.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2046.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2046;}); _temp2045;}));
_temp2044->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]))->tms; _temp2044;}); _temp2043;}); _temp2042;});
_temp2041;}); break; case 136: _LL2040: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2048=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2048[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2049; _temp2049.tag= Cyc_Declarator_tok;
_temp2049.f1=({ struct Cyc_Parse_Declarator* _temp2050=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2050->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2050->tms= 0;
_temp2050;}); _temp2049;}); _temp2048;}); break; case 137: _LL2047: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2052=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2052[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2053; _temp2053.tag= Cyc_Declarator_tok;
_temp2053.f1=({ struct Cyc_Parse_Declarator* _temp2054=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2054->id= Cyc_Absyn_exn_name;
_temp2054->tms= 0; _temp2054;}); _temp2053;}); _temp2052;}); break; case 138:
_LL2051: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2056=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2056[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2057; _temp2057.tag=
Cyc_TypeModifierList_tok; _temp2057.f1=({ struct Cyc_List_List* _temp2058=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2058->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2059=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2059[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2060; _temp2060.tag= Cyc_Absyn_Pointer_mod;
_temp2060.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2060.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2060.f3= Cyc_Absyn_empty_tqual(); _temp2060;});
_temp2059;})); _temp2058->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2058;});
_temp2057;}); _temp2056;}); break; case 139: _LL2055: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2062=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2062[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2063; _temp2063.tag= Cyc_TypeModifierList_tok;
_temp2063.f1=({ struct Cyc_List_List* _temp2064=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2064->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2065=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2065[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2066; _temp2066.tag= Cyc_Absyn_Pointer_mod;
_temp2066.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2066.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2066.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2066;}); _temp2065;}));
_temp2064->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0);
_temp2064;}); _temp2063;}); _temp2062;}); break; case 140: _LL2061: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2068=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2068[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2069; _temp2069.tag= Cyc_TypeModifierList_tok;
_temp2069.f1=({ struct Cyc_List_List* _temp2070=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2070->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2071=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2071[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2072; _temp2072.tag= Cyc_Absyn_Pointer_mod;
_temp2072.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2072.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2072.f3= Cyc_Absyn_empty_tqual(); _temp2072;});
_temp2071;})); _temp2070->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2070;});
_temp2069;}); _temp2068;}); break; case 141: _LL2067: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2074=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2074[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2075; _temp2075.tag= Cyc_TypeModifierList_tok;
_temp2075.f1=({ struct Cyc_List_List* _temp2076=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2076->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2077=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2077[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2078; _temp2078.tag= Cyc_Absyn_Pointer_mod;
_temp2078.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2078.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2078.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2078;});
_temp2077;})); _temp2076->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2076;});
_temp2075;}); _temp2074;}); break; case 142: _LL2073: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2080=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2080[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2081; _temp2081.tag= Cyc_Pointer_Sort_tok;
_temp2081.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2082=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2082[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2083; _temp2083.tag=
Cyc_Absyn_Nullable_ps; _temp2083.f1= Cyc_Absyn_exp_unsigned_one; _temp2083;});
_temp2082;})); _temp2081;}); _temp2080;}); break; case 143: _LL2079: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2085=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2085[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2086; _temp2086.tag= Cyc_Pointer_Sort_tok;
_temp2086.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2087=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2087[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2088; _temp2088.tag= Cyc_Absyn_NonNullable_ps; _temp2088.f1= Cyc_Absyn_exp_unsigned_one;
_temp2088;}); _temp2087;})); _temp2086;}); _temp2085;}); break; case 144:
_LL2084: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2090=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2090[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2091; _temp2091.tag=
Cyc_Pointer_Sort_tok; _temp2091.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2092=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2092[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2093; _temp2093.tag=
Cyc_Absyn_Nullable_ps; _temp2093.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2093;});
_temp2092;})); _temp2091;}); _temp2090;}); break; case 145: _LL2089: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2095=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2095[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2096; _temp2096.tag= Cyc_Pointer_Sort_tok;
_temp2096.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2097=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2097[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2098; _temp2098.tag= Cyc_Absyn_NonNullable_ps; _temp2098.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2098;});
_temp2097;})); _temp2096;}); _temp2095;}); break; case 146: _LL2094: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2100=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2100[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2101; _temp2101.tag= Cyc_Pointer_Sort_tok;
_temp2101.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2101;});
_temp2100;}); break; case 147: _LL2099: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2103=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2103[ 0]=({ struct Cyc_Type_tok_struct _temp2104; _temp2104.tag= Cyc_Type_tok;
_temp2104.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2105=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2105->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2105;}), 0); _temp2104;}); _temp2103;});
break; case 148: _LL2102: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 149: _LL2106: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
150: _LL2107: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2109=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2109[
0]=({ struct Cyc_Type_tok_struct _temp2110; _temp2110.tag= Cyc_Type_tok;
_temp2110.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2111=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2111->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2111;}), 0); _temp2110;}); _temp2109;});
break; case 151: _LL2108: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 152: _LL2112: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2114=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2114[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2115; _temp2115.tag= Cyc_TypeQual_tok; _temp2115.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2115;});
_temp2114;}); break; case 153: _LL2113: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2117=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2117[ 0]=({ struct Cyc_YY1_struct _temp2118; _temp2118.tag= Cyc_YY1;
_temp2118.f1=({ struct _tuple16* _temp2119=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2119->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2119->f2= 0;
_temp2119->f3= 0; _temp2119->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2119->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2119;});
_temp2118;}); _temp2117;}); break; case 154: _LL2116: yyval=( void*)({ struct
Cyc_YY1_struct* _temp2121=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct
Cyc_YY1_struct)); _temp2121[ 0]=({ struct Cyc_YY1_struct _temp2122; _temp2122.tag=
Cyc_YY1; _temp2122.f1=({ struct _tuple16* _temp2123=( struct _tuple16*)
_cycalloc( sizeof( struct _tuple16)); _temp2123->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2123->f2= 1;
_temp2123->f3= 0; _temp2123->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2123->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2123;});
_temp2122;}); _temp2121;}); break; case 155: _LL2120: { struct _tuple2 _temp2127;
void* _temp2128; struct Cyc_Absyn_Tqual _temp2130; struct Cyc_Core_Opt*
_temp2132; struct _tuple2* _temp2125= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2127=*
_temp2125; _LL2133: _temp2132= _temp2127.f1; goto _LL2131; _LL2131: _temp2130=
_temp2127.f2; goto _LL2129; _LL2129: _temp2128= _temp2127.f3; goto _LL2126;
_LL2126: { struct Cyc_Absyn_VarargInfo* _temp2134=({ struct Cyc_Absyn_VarargInfo*
_temp2138=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2138->name= _temp2132; _temp2138->tq= _temp2130; _temp2138->type=( void*)
_temp2128; _temp2138->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2138;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2135=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2135[ 0]=({ struct Cyc_YY1_struct
_temp2136; _temp2136.tag= Cyc_YY1; _temp2136.f1=({ struct _tuple16* _temp2137=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2137->f1= 0;
_temp2137->f2= 0; _temp2137->f3= _temp2134; _temp2137->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2137->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2137;}); _temp2136;}); _temp2135;}); break;}} case 156:
_LL2124: { struct _tuple2 _temp2142; void* _temp2143; struct Cyc_Absyn_Tqual
_temp2145; struct Cyc_Core_Opt* _temp2147; struct _tuple2* _temp2140= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2142=*
_temp2140; _LL2148: _temp2147= _temp2142.f1; goto _LL2146; _LL2146: _temp2145=
_temp2142.f2; goto _LL2144; _LL2144: _temp2143= _temp2142.f3; goto _LL2141;
_LL2141: { struct Cyc_Absyn_VarargInfo* _temp2149=({ struct Cyc_Absyn_VarargInfo*
_temp2153=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2153->name= _temp2147; _temp2153->tq= _temp2145; _temp2153->type=( void*)
_temp2143; _temp2153->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2153;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2150=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2150[ 0]=({ struct Cyc_YY1_struct
_temp2151; _temp2151.tag= Cyc_YY1; _temp2151.f1=({ struct _tuple16* _temp2152=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2152->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2152->f2=
0; _temp2152->f3= _temp2149; _temp2152->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2152->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2152;});
_temp2151;}); _temp2150;}); break;}} case 157: _LL2139: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2155=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2155[ 0]=({ struct Cyc_Type_tok_struct
_temp2156; _temp2156.tag= Cyc_Type_tok; _temp2156.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()); _temp2156;}); _temp2155;}); break; case 158: _LL2154:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2158=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2158[ 0]=({ struct Cyc_Type_tok_struct
_temp2159; _temp2159.tag= Cyc_Type_tok; _temp2159.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp2159;});
_temp2158;}); break; case 159: _LL2157: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2161=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2161[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2162; _temp2162.tag= Cyc_TypeOpt_tok;
_temp2162.f1= 0; _temp2162;}); _temp2161;}); break; case 160: _LL2160: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2164=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2164[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2165; _temp2165.tag= Cyc_TypeOpt_tok; _temp2165.f1=({ struct Cyc_Core_Opt*
_temp2166=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2166->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2167=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2167[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2168; _temp2168.tag= Cyc_Absyn_JoinEff;
_temp2168.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2168;}); _temp2167;})); _temp2166;}); _temp2165;});
_temp2164;}); break; case 161: _LL2163: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2170=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2170[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2171; _temp2171.tag= Cyc_Rgnorder_tok;
_temp2171.f1= 0; _temp2171;}); _temp2170;}); break; case 162: _LL2169: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL2172: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2174=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2174[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2175; _temp2175.tag= Cyc_Rgnorder_tok; _temp2175.f1=({ struct Cyc_List_List*
_temp2176=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2176->hd=( void*)({ struct _tuple18* _temp2177=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2177->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2177->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2177;}); _temp2176->tl= 0; _temp2176;});
_temp2175;}); _temp2174;}); break; case 164: _LL2173: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2179=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2179[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2180; _temp2180.tag= Cyc_Rgnorder_tok; _temp2180.f1=({ struct Cyc_List_List*
_temp2181=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2181->hd=( void*)({ struct _tuple18* _temp2182=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2182->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2182->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2182;}); _temp2181->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2181;});
_temp2180;}); _temp2179;}); break; case 165: _LL2178: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2184=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2184[ 0]=({ struct Cyc_Bool_tok_struct
_temp2185; _temp2185.tag= Cyc_Bool_tok; _temp2185.f1= 0; _temp2185;}); _temp2184;});
break; case 166: _LL2183: { struct _tagged_arr _temp2187= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Std_zstrcmp(
_temp2187, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2188=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2188[ 0]=({ struct Cyc_Bool_tok_struct _temp2189; _temp2189.tag= Cyc_Bool_tok;
_temp2189.f1= 1; _temp2189;}); _temp2188;}); break;} case 167: _LL2186: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168:
_LL2190: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2192=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2192[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2193; _temp2193.tag= Cyc_TypeList_tok; _temp2193.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2193;});
_temp2192;}); break; case 169: _LL2191: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2195=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2195[ 0]=({ struct Cyc_TypeList_tok_struct _temp2196; _temp2196.tag= Cyc_TypeList_tok;
_temp2196.f1= 0; _temp2196;}); _temp2195;}); break; case 170: _LL2194: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case
171: _LL2197: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2199=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2199[ 0]=({ struct Cyc_TypeList_tok_struct _temp2200; _temp2200.tag= Cyc_TypeList_tok;
_temp2200.f1=({ struct Cyc_List_List* _temp2201=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2201->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp2202=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp2202[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp2203; _temp2203.tag= Cyc_Absyn_RgnsEff; _temp2203.f1=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2203;}); _temp2202;})); _temp2201->tl= 0; _temp2201;});
_temp2200;}); _temp2199;}); break; case 172: _LL2198: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2205=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2205[ 0]=({ struct Cyc_TypeList_tok_struct _temp2206; _temp2206.tag= Cyc_TypeList_tok;
_temp2206.f1=({ struct Cyc_List_List* _temp2207=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2207->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2207->tl= 0;
_temp2207;}); _temp2206;}); _temp2205;}); break; case 173: _LL2204: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2209=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2209[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2210; _temp2210.tag= Cyc_TypeList_tok; _temp2210.f1=({ struct Cyc_List_List*
_temp2211=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2211->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2212=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2212[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2213; _temp2213.tag= Cyc_Absyn_AccessEff;
_temp2213.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2213;}); _temp2212;})); _temp2211->tl= 0;
_temp2211;}); _temp2210;}); _temp2209;}); break; case 174: _LL2208: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),( void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2215=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2215[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2216; _temp2216.tag= Cyc_TypeList_tok; _temp2216.f1=({ struct Cyc_List_List*
_temp2217=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2217->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2218=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2218[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2219; _temp2219.tag= Cyc_Absyn_AccessEff;
_temp2219.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2219;}); _temp2218;})); _temp2217->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2217;});
_temp2216;}); _temp2215;}); break; case 175: _LL2214: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2221=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2221[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2222; _temp2222.tag= Cyc_ParamDeclList_tok;
_temp2222.f1=({ struct Cyc_List_List* _temp2223=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2223->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2223->tl= 0;
_temp2223;}); _temp2222;}); _temp2221;}); break; case 176: _LL2220: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2225=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2225[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2226; _temp2226.tag= Cyc_ParamDeclList_tok;
_temp2226.f1=({ struct Cyc_List_List* _temp2227=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2227->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2227->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2227;}); _temp2226;}); _temp2225;}); break; case 177:
_LL2224: { struct _tuple17 _temp2231; struct Cyc_List_List* _temp2232; struct
Cyc_List_List* _temp2234; struct Cyc_Absyn_Tqual _temp2236; struct _tuple17*
_temp2229= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2231=* _temp2229; _LL2237: _temp2236=
_temp2231.f1; goto _LL2235; _LL2235: _temp2234= _temp2231.f2; goto _LL2233;
_LL2233: _temp2232= _temp2231.f3; goto _LL2230; _LL2230: { struct Cyc_Parse_Declarator
_temp2240; struct Cyc_List_List* _temp2241; struct _tuple1* _temp2243; struct
Cyc_Parse_Declarator* _temp2238= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2240=* _temp2238;
_LL2244: _temp2243= _temp2240.id; goto _LL2242; _LL2242: _temp2241= _temp2240.tms;
goto _LL2239; _LL2239: { void* _temp2245= Cyc_Parse_speclist2typ( _temp2234, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2248; struct Cyc_List_List* _temp2250; void*
_temp2252; struct Cyc_Absyn_Tqual _temp2254; struct _tuple6 _temp2246= Cyc_Parse_apply_tms(
_temp2236, _temp2245, _temp2232, _temp2241); _LL2255: _temp2254= _temp2246.f1;
goto _LL2253; _LL2253: _temp2252= _temp2246.f2; goto _LL2251; _LL2251: _temp2250=
_temp2246.f3; goto _LL2249; _LL2249: _temp2248= _temp2246.f4; goto _LL2247;
_LL2247: if( _temp2250 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2243)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2256=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2260=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2260->v=( void*)(* _temp2243).f2; _temp2260;}); if( _temp2248 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2257=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2257[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2258; _temp2258.tag= Cyc_ParamDecl_tok;
_temp2258.f1=({ struct _tuple2* _temp2259=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2259->f1= _temp2256; _temp2259->f2= _temp2254; _temp2259->f3=
_temp2252; _temp2259;}); _temp2258;}); _temp2257;}); break;}}}} case 178:
_LL2228: { struct _tuple17 _temp2264; struct Cyc_List_List* _temp2265; struct
Cyc_List_List* _temp2267; struct Cyc_Absyn_Tqual _temp2269; struct _tuple17*
_temp2262= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2264=* _temp2262; _LL2270: _temp2269= _temp2264.f1;
goto _LL2268; _LL2268: _temp2267= _temp2264.f2; goto _LL2266; _LL2266: _temp2265=
_temp2264.f3; goto _LL2263; _LL2263: { void* _temp2271= Cyc_Parse_speclist2typ(
_temp2267, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2265 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2272=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2272[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2273; _temp2273.tag= Cyc_ParamDecl_tok; _temp2273.f1=({
struct _tuple2* _temp2274=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2274->f1= 0; _temp2274->f2= _temp2269; _temp2274->f3= _temp2271; _temp2274;});
_temp2273;}); _temp2272;}); break;}} case 179: _LL2261: { struct _tuple17
_temp2278; struct Cyc_List_List* _temp2279; struct Cyc_List_List* _temp2281;
struct Cyc_Absyn_Tqual _temp2283; struct _tuple17* _temp2276= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2278=*
_temp2276; _LL2284: _temp2283= _temp2278.f1; goto _LL2282; _LL2282: _temp2281=
_temp2278.f2; goto _LL2280; _LL2280: _temp2279= _temp2278.f3; goto _LL2277;
_LL2277: { void* _temp2285= Cyc_Parse_speclist2typ( _temp2281, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2286=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2289; struct Cyc_List_List* _temp2291; void* _temp2293; struct Cyc_Absyn_Tqual
_temp2295; struct _tuple6 _temp2287= Cyc_Parse_apply_tms( _temp2283, _temp2285,
_temp2279, _temp2286); _LL2296: _temp2295= _temp2287.f1; goto _LL2294; _LL2294:
_temp2293= _temp2287.f2; goto _LL2292; _LL2292: _temp2291= _temp2287.f3; goto
_LL2290; _LL2290: _temp2289= _temp2287.f4; goto _LL2288; _LL2288: if( _temp2291
!=  0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2289 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2297=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2297[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2298; _temp2298.tag= Cyc_ParamDecl_tok; _temp2298.f1=({
struct _tuple2* _temp2299=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2299->f1= 0; _temp2299->f2= _temp2295; _temp2299->f3= _temp2293; _temp2299;});
_temp2298;}); _temp2297;}); break;}} case 180: _LL2275: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2301=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2301[ 0]=({ struct Cyc_IdList_tok_struct
_temp2302; _temp2302.tag= Cyc_IdList_tok; _temp2302.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2302;});
_temp2301;}); break; case 181: _LL2300: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2304=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2304[ 0]=({ struct Cyc_IdList_tok_struct _temp2305; _temp2305.tag= Cyc_IdList_tok;
_temp2305.f1=({ struct Cyc_List_List* _temp2306=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2306->hd=( void*)({ struct
_tagged_arr* _temp2307=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2307[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2307;}); _temp2306->tl=
0; _temp2306;}); _temp2305;}); _temp2304;}); break; case 182: _LL2303: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2309=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2309[ 0]=({ struct Cyc_IdList_tok_struct
_temp2310; _temp2310.tag= Cyc_IdList_tok; _temp2310.f1=({ struct Cyc_List_List*
_temp2311=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2311->hd=( void*)({ struct _tagged_arr* _temp2312=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2312[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2312;});
_temp2311->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2311;}); _temp2310;}); _temp2309;}); break; case 183:
_LL2308: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 184: _LL2313: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 185: _LL2314: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2316=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2316[ 0]=({ struct Cyc_Exp_tok_struct _temp2317; _temp2317.tag= Cyc_Exp_tok;
_temp2317.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2318=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2318[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2319; _temp2319.tag= Cyc_Absyn_UnresolvedMem_e; _temp2319.f1= 0; _temp2319.f2=
0; _temp2319;}); _temp2318;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2317;});
_temp2316;}); break; case 186: _LL2315: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2321=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2321[ 0]=({ struct Cyc_Exp_tok_struct _temp2322; _temp2322.tag= Cyc_Exp_tok;
_temp2322.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2323=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2323[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2324; _temp2324.tag= Cyc_Absyn_UnresolvedMem_e; _temp2324.f1= 0; _temp2324.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2324;});
_temp2323;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2322;}); _temp2321;}); break; case 187:
_LL2320: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2326=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2326[ 0]=({ struct Cyc_Exp_tok_struct
_temp2327; _temp2327.tag= Cyc_Exp_tok; _temp2327.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2328=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2328[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2329; _temp2329.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2329.f1= 0; _temp2329.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2329;});
_temp2328;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2327;}); _temp2326;}); break; case 188:
_LL2325: { struct Cyc_Absyn_Vardecl* _temp2331= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2337=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2337->f1=( void*) Cyc_Absyn_Loc_n; _temp2337->f2=({ struct _tagged_arr*
_temp2338=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp2338[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); _temp2338;}); _temp2337;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp2331->tq=({ struct Cyc_Absyn_Tqual _temp2332; _temp2332.q_const= 1;
_temp2332.q_volatile= 0; _temp2332.q_restrict= 1; _temp2332;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2333=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2333[ 0]=({ struct Cyc_Exp_tok_struct
_temp2334; _temp2334.tag= Cyc_Exp_tok; _temp2334.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2335=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2335[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2336; _temp2336.tag= Cyc_Absyn_Comprehension_e;
_temp2336.f1= _temp2331; _temp2336.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2336.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2336;});
_temp2335;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2334;}); _temp2333;}); break;} case
189: _LL2330: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2340=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2340[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2341; _temp2341.tag=
Cyc_InitializerList_tok; _temp2341.f1=({ struct Cyc_List_List* _temp2342=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2342->hd=(
void*)({ struct _tuple19* _temp2343=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2343->f1= 0; _temp2343->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2343;}); _temp2342->tl=
0; _temp2342;}); _temp2341;}); _temp2340;}); break; case 190: _LL2339: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2345=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2345[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2346; _temp2346.tag= Cyc_InitializerList_tok;
_temp2346.f1=({ struct Cyc_List_List* _temp2347=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2347->hd=( void*)({ struct
_tuple19* _temp2348=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2348->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2348->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2348;});
_temp2347->tl= 0; _temp2347;}); _temp2346;}); _temp2345;}); break; case 191:
_LL2344: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2350=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2350[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2351; _temp2351.tag=
Cyc_InitializerList_tok; _temp2351.f1=({ struct Cyc_List_List* _temp2352=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2352->hd=(
void*)({ struct _tuple19* _temp2353=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2353->f1= 0; _temp2353->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2353;}); _temp2352->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2352;}); _temp2351;}); _temp2350;}); break; case 192:
_LL2349: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2355=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2355[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2356; _temp2356.tag=
Cyc_InitializerList_tok; _temp2356.f1=({ struct Cyc_List_List* _temp2357=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2357->hd=(
void*)({ struct _tuple19* _temp2358=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2358->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2358->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2358;});
_temp2357->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2357;});
_temp2356;}); _temp2355;}); break; case 193: _LL2354: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2360=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2360[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2361; _temp2361.tag= Cyc_DesignatorList_tok;
_temp2361.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2361;});
_temp2360;}); break; case 194: _LL2359: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2363=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2363[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2364; _temp2364.tag=
Cyc_DesignatorList_tok; _temp2364.f1=({ struct Cyc_List_List* _temp2365=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2365->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2365->tl= 0; _temp2365;}); _temp2364;}); _temp2363;});
break; case 195: _LL2362: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2367=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2367[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2368; _temp2368.tag=
Cyc_DesignatorList_tok; _temp2368.f1=({ struct Cyc_List_List* _temp2369=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2369->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2369->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2369;});
_temp2368;}); _temp2367;}); break; case 196: _LL2366: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2371=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2371[ 0]=({ struct
Cyc_Designator_tok_struct _temp2372; _temp2372.tag= Cyc_Designator_tok;
_temp2372.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2373=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2373[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2374; _temp2374.tag=
Cyc_Absyn_ArrayElement; _temp2374.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2374;});
_temp2373;})); _temp2372;}); _temp2371;}); break; case 197: _LL2370: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2376=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2376[ 0]=({ struct
Cyc_Designator_tok_struct _temp2377; _temp2377.tag= Cyc_Designator_tok;
_temp2377.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2378=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2378[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2379; _temp2379.tag= Cyc_Absyn_FieldName;
_temp2379.f1=({ struct _tagged_arr* _temp2380=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2380[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2380;}); _temp2379;});
_temp2378;})); _temp2377;}); _temp2376;}); break; case 198: _LL2375: { void*
_temp2382= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2383=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2383 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2384=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2385=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2385[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2386; _temp2386.tag= Cyc_ParamDecl_tok; _temp2386.f1=({
struct _tuple2* _temp2387=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2387->f1= 0; _temp2387->f2= _temp2384; _temp2387->f3= _temp2382; _temp2387;});
_temp2386;}); _temp2385;}); break;}} case 199: _LL2381: { void* _temp2389= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp2390=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2391=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2392=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2393= Cyc_Parse_apply_tms( _temp2391, _temp2389, _temp2390,
_temp2392); if( _temp2393.f3 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2393.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2394=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2394[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2395; _temp2395.tag= Cyc_ParamDecl_tok; _temp2395.f1=({
struct _tuple2* _temp2396=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2396->f1= 0; _temp2396->f2= _temp2393.f1; _temp2396->f3= _temp2393.f2;
_temp2396;}); _temp2395;}); _temp2394;}); break;} case 200: _LL2388: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2398=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2398[ 0]=({ struct Cyc_Type_tok_struct
_temp2399; _temp2399.tag= Cyc_Type_tok; _temp2399.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2399;});
_temp2398;}); break; case 201: _LL2397: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2401=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2401[ 0]=({ struct Cyc_Type_tok_struct _temp2402; _temp2402.tag= Cyc_Type_tok;
_temp2402.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2403=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2403[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2404; _temp2404.tag= Cyc_Absyn_JoinEff;
_temp2404.f1= 0; _temp2404;}); _temp2403;})); _temp2402;}); _temp2401;}); break;
case 202: _LL2400: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2406=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2406[ 0]=({ struct Cyc_Type_tok_struct _temp2407; _temp2407.tag= Cyc_Type_tok;
_temp2407.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2408=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2408[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2409; _temp2409.tag= Cyc_Absyn_JoinEff;
_temp2409.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2409;}); _temp2408;})); _temp2407;});
_temp2406;}); break; case 203: _LL2405: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2411=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2411[ 0]=({ struct Cyc_Type_tok_struct _temp2412; _temp2412.tag= Cyc_Type_tok;
_temp2412.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2413=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2413[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2414; _temp2414.tag= Cyc_Absyn_RgnsEff;
_temp2414.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2414;}); _temp2413;})); _temp2412;});
_temp2411;}); break; case 204: _LL2410: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2416=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2416[ 0]=({ struct Cyc_Type_tok_struct _temp2417; _temp2417.tag= Cyc_Type_tok;
_temp2417.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2418=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2418[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2419; _temp2419.tag= Cyc_Absyn_JoinEff;
_temp2419.f1=({ struct Cyc_List_List* _temp2420=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2420->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2420->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2420;}); _temp2419;}); _temp2418;})); _temp2417;});
_temp2416;}); break; case 205: _LL2415: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2422=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2422[ 0]=({ struct Cyc_TypeList_tok_struct _temp2423; _temp2423.tag= Cyc_TypeList_tok;
_temp2423.f1=({ struct Cyc_List_List* _temp2424=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2424->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2424->tl= 0;
_temp2424;}); _temp2423;}); _temp2422;}); break; case 206: _LL2421: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2426=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2426[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2427; _temp2427.tag= Cyc_TypeList_tok; _temp2427.f1=({ struct Cyc_List_List*
_temp2428=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2428->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2428->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2428;});
_temp2427;}); _temp2426;}); break; case 207: _LL2425: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2430=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2430[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2431; _temp2431.tag= Cyc_AbstractDeclarator_tok;
_temp2431.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2432=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2432->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2432;});
_temp2431;}); _temp2430;}); break; case 208: _LL2429: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 209: _LL2433:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2435=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2435[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2436; _temp2436.tag=
Cyc_AbstractDeclarator_tok; _temp2436.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2437=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2437->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2437;});
_temp2436;}); _temp2435;}); break; case 210: _LL2434: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 211:
_LL2438: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2440=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2440[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2441; _temp2441.tag=
Cyc_AbstractDeclarator_tok; _temp2441.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2442=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2442->tms=({ struct Cyc_List_List* _temp2443=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2443->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2443->tl= 0; _temp2443;}); _temp2442;}); _temp2441;}); _temp2440;}); break;
case 212: _LL2439: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2445=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2445[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2446; _temp2446.tag= Cyc_AbstractDeclarator_tok; _temp2446.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2447=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2447->tms=({ struct Cyc_List_List* _temp2448=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2448->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2448->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2448;});
_temp2447;}); _temp2446;}); _temp2445;}); break; case 213: _LL2444: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2450=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2450[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2451; _temp2451.tag= Cyc_AbstractDeclarator_tok;
_temp2451.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2452=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2452->tms=({
struct Cyc_List_List* _temp2453=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2453->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2454=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2454[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2455; _temp2455.tag= Cyc_Absyn_ConstArray_mod; _temp2455.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2455;});
_temp2454;})); _temp2453->tl= 0; _temp2453;}); _temp2452;}); _temp2451;});
_temp2450;}); break; case 214: _LL2449: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2457=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2457[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2458; _temp2458.tag= Cyc_AbstractDeclarator_tok; _temp2458.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2459=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2459->tms=({ struct Cyc_List_List* _temp2460=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2460->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2461=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2461[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2462; _temp2462.tag= Cyc_Absyn_ConstArray_mod;
_temp2462.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2462;}); _temp2461;})); _temp2460->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2460;}); _temp2459;}); _temp2458;}); _temp2457;}); break; case 215: _LL2456:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2464=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2464[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2465; _temp2465.tag=
Cyc_AbstractDeclarator_tok; _temp2465.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2466=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2466->tms=({ struct Cyc_List_List* _temp2467=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2467->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2468=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2468[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2469; _temp2469.tag= Cyc_Absyn_Function_mod;
_temp2469.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2470=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2470[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2471; _temp2471.tag= Cyc_Absyn_WithTypes;
_temp2471.f1= 0; _temp2471.f2= 0; _temp2471.f3= 0; _temp2471.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2471.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2471;}); _temp2470;})); _temp2469;}); _temp2468;}));
_temp2467->tl= 0; _temp2467;}); _temp2466;}); _temp2465;}); _temp2464;}); break;
case 216: _LL2463: { struct _tuple16 _temp2475; struct Cyc_List_List* _temp2476;
struct Cyc_Core_Opt* _temp2478; struct Cyc_Absyn_VarargInfo* _temp2480; int
_temp2482; struct Cyc_List_List* _temp2484; struct _tuple16* _temp2473= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2475=*
_temp2473; _LL2485: _temp2484= _temp2475.f1; goto _LL2483; _LL2483: _temp2482=
_temp2475.f2; goto _LL2481; _LL2481: _temp2480= _temp2475.f3; goto _LL2479;
_LL2479: _temp2478= _temp2475.f4; goto _LL2477; _LL2477: _temp2476= _temp2475.f5;
goto _LL2474; _LL2474: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2486=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2486[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2487; _temp2487.tag= Cyc_AbstractDeclarator_tok; _temp2487.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2488=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2488->tms=({ struct Cyc_List_List* _temp2489=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2489->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2490=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2490[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2491; _temp2491.tag= Cyc_Absyn_Function_mod;
_temp2491.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2492=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2492[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2493; _temp2493.tag= Cyc_Absyn_WithTypes;
_temp2493.f1= _temp2484; _temp2493.f2= _temp2482; _temp2493.f3= _temp2480;
_temp2493.f4= _temp2478; _temp2493.f5= _temp2476; _temp2493;}); _temp2492;}));
_temp2491;}); _temp2490;})); _temp2489->tl= 0; _temp2489;}); _temp2488;});
_temp2487;}); _temp2486;}); break;} case 217: _LL2472: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2495=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2495[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2496; _temp2496.tag= Cyc_AbstractDeclarator_tok;
_temp2496.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2497=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2497->tms=({
struct Cyc_List_List* _temp2498=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2498->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2499=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2499[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2500; _temp2500.tag=
Cyc_Absyn_Function_mod; _temp2500.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2501=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2501[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2502; _temp2502.tag= Cyc_Absyn_WithTypes;
_temp2502.f1= 0; _temp2502.f2= 0; _temp2502.f3= 0; _temp2502.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2502.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2502;}); _temp2501;})); _temp2500;}); _temp2499;}));
_temp2498->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2498;});
_temp2497;}); _temp2496;}); _temp2495;}); break; case 218: _LL2494: { struct
_tuple16 _temp2506; struct Cyc_List_List* _temp2507; struct Cyc_Core_Opt*
_temp2509; struct Cyc_Absyn_VarargInfo* _temp2511; int _temp2513; struct Cyc_List_List*
_temp2515; struct _tuple16* _temp2504= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2506=*
_temp2504; _LL2516: _temp2515= _temp2506.f1; goto _LL2514; _LL2514: _temp2513=
_temp2506.f2; goto _LL2512; _LL2512: _temp2511= _temp2506.f3; goto _LL2510;
_LL2510: _temp2509= _temp2506.f4; goto _LL2508; _LL2508: _temp2507= _temp2506.f5;
goto _LL2505; _LL2505: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2517=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2517[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2518; _temp2518.tag= Cyc_AbstractDeclarator_tok; _temp2518.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2519=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2519->tms=({ struct Cyc_List_List* _temp2520=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2520->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2521=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2521[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2522; _temp2522.tag= Cyc_Absyn_Function_mod;
_temp2522.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2523=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2523[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2524; _temp2524.tag= Cyc_Absyn_WithTypes;
_temp2524.f1= _temp2515; _temp2524.f2= _temp2513; _temp2524.f3= _temp2511;
_temp2524.f4= _temp2509; _temp2524.f5= _temp2507; _temp2524;}); _temp2523;}));
_temp2522;}); _temp2521;})); _temp2520->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2520;}); _temp2519;}); _temp2518;}); _temp2517;}); break;} case 219:
_LL2503: { struct Cyc_List_List* _temp2526=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2527=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2527[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2528; _temp2528.tag= Cyc_AbstractDeclarator_tok; _temp2528.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2529=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2529->tms=({ struct Cyc_List_List* _temp2530=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2530->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2531=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2531[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2532; _temp2532.tag= Cyc_Absyn_TypeParams_mod;
_temp2532.f1= _temp2526; _temp2532.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2532.f3=
0; _temp2532;}); _temp2531;})); _temp2530->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2530;}); _temp2529;}); _temp2528;}); _temp2527;}); break;} case 220:
_LL2525:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2534=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2535=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2535[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2536; _temp2536.tag= Cyc_AbstractDeclarator_tok; _temp2536.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2537=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2537->tms=({ struct Cyc_List_List* _temp2538=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2538->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2539=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2539[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2540; _temp2540.tag= Cyc_Absyn_TypeParams_mod;
_temp2540.f1= _temp2534; _temp2540.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2540.f3=
0; _temp2540;}); _temp2539;})); _temp2538->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2538;}); _temp2537;}); _temp2536;}); _temp2535;}); break;} case 221:
_LL2533: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2542=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2542[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2543; _temp2543.tag=
Cyc_AbstractDeclarator_tok; _temp2543.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2544=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2544->tms=({ struct Cyc_List_List* _temp2545=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2545->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2546=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2546[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2547; _temp2547.tag= Cyc_Absyn_Attributes_mod;
_temp2547.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2547.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2547;}); _temp2546;}));
_temp2545->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms; _temp2545;});
_temp2544;}); _temp2543;}); _temp2542;}); break; case 222: _LL2541: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 223: _LL2548:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
224: _LL2549: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 225: _LL2550: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 226: _LL2551: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 227: _LL2552:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
228: _LL2553: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2565=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2565->name=({ struct
_tagged_arr* _temp2566=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2566[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2566;});
_temp2565->identity= 0; _temp2565->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2565;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2563=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2563[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2564; _temp2564.tag= Cyc_Absyn_VarType;
_temp2564.f1= tv; _temp2564;}); _temp2563;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2555=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2555[ 0]=({ struct Cyc_Stmt_tok_struct _temp2556; _temp2556.tag= Cyc_Stmt_tok;
_temp2556.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2557=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2557[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2558; _temp2558.tag= Cyc_Absyn_Region_s;
_temp2558.f1= tv; _temp2558.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2559=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2559->f1=(
void*) Cyc_Absyn_Loc_n; _temp2559->f2=({ struct _tagged_arr* _temp2560=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2560[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2560;});
_temp2559;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2561=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2561[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2562; _temp2562.tag=
Cyc_Absyn_RgnHandleType; _temp2562.f1=( void*) t; _temp2562;}); _temp2561;}), 0);
_temp2558.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2558;}); _temp2557;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2556;});
_temp2555;}); break;} case 229: _LL2554: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2578=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2578->name=({ struct
_tagged_arr* _temp2579=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2579[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2581; _temp2581.tag= Cyc_Std_String_pa; _temp2581.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2580[ 1u]={& _temp2581}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2580, sizeof( void*), 1u));}});
_temp2579;}); _temp2578->identity= 0; _temp2578->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2578;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2576=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2576[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2577; _temp2577.tag= Cyc_Absyn_VarType;
_temp2577.f1= tv; _temp2577;}); _temp2576;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2568=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2568[ 0]=({ struct Cyc_Stmt_tok_struct _temp2569; _temp2569.tag= Cyc_Stmt_tok;
_temp2569.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2570=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2570[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2571; _temp2571.tag= Cyc_Absyn_Region_s;
_temp2571.f1= tv; _temp2571.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2572=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2572->f1=(
void*) Cyc_Absyn_Loc_n; _temp2572->f2=({ struct _tagged_arr* _temp2573=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2573[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2573;});
_temp2572;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2574=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2574[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2575; _temp2575.tag=
Cyc_Absyn_RgnHandleType; _temp2575.f1=( void*) t; _temp2575;}); _temp2574;}), 0);
_temp2571.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2571;}); _temp2570;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2569;});
_temp2568;}); break;} case 230: _LL2567: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2583=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2583[ 0]=({ struct Cyc_Stmt_tok_struct _temp2584; _temp2584.tag= Cyc_Stmt_tok;
_temp2584.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2585=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2585[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2586; _temp2586.tag= Cyc_Absyn_Cut_s;
_temp2586.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2586;}); _temp2585;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2584;});
_temp2583;}); break; case 231: _LL2582: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2588=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2588[ 0]=({ struct Cyc_Stmt_tok_struct _temp2589; _temp2589.tag= Cyc_Stmt_tok;
_temp2589.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2590=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2590[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2591; _temp2591.tag= Cyc_Absyn_Splice_s;
_temp2591.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2591;}); _temp2590;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2589;});
_temp2588;}); break; case 232: _LL2587: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2593=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2593[ 0]=({ struct Cyc_Stmt_tok_struct _temp2594; _temp2594.tag= Cyc_Stmt_tok;
_temp2594.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2595=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2595[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2596; _temp2596.tag= Cyc_Absyn_Label_s;
_temp2596.f1=({ struct _tagged_arr* _temp2597=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2597[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2597;});
_temp2596.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2596;}); _temp2595;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2594;});
_temp2593;}); break; case 233: _LL2592: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2599=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2599[ 0]=({ struct Cyc_Stmt_tok_struct _temp2600; _temp2600.tag= Cyc_Stmt_tok;
_temp2600.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2600;});
_temp2599;}); break; case 234: _LL2598: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2602=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2602[ 0]=({ struct Cyc_Stmt_tok_struct _temp2603; _temp2603.tag= Cyc_Stmt_tok;
_temp2603.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2603;}); _temp2602;}); break; case 235: _LL2601: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2605=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2605[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2606; _temp2606.tag= Cyc_Stmt_tok; _temp2606.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2606;}); _temp2605;}); break; case 236: _LL2604: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 237:
_LL2607: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2609=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2609[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2610; _temp2610.tag= Cyc_Stmt_tok; _temp2610.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2610;}); _temp2609;}); break;
case 238: _LL2608: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2612=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2612[ 0]=({ struct Cyc_Stmt_tok_struct _temp2613; _temp2613.tag= Cyc_Stmt_tok;
_temp2613.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2613;});
_temp2612;}); break; case 239: _LL2611: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 240: _LL2614:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2616=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2616[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2617; _temp2617.tag= Cyc_Stmt_tok; _temp2617.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2617;}); _temp2616;}); break; case 241: _LL2615: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2619=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2619[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2620; _temp2620.tag= Cyc_Stmt_tok; _temp2620.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2621=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2621[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2622; _temp2622.tag= Cyc_Absyn_Fn_d;
_temp2622.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2622;}); _temp2621;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2620;}); _temp2619;}); break; case 242: _LL2618: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2624=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2624[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2625; _temp2625.tag= Cyc_Stmt_tok; _temp2625.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2626=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2626[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2627; _temp2627.tag= Cyc_Absyn_Fn_d;
_temp2627.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2627;}); _temp2626;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2625;}); _temp2624;}); break; case 243: _LL2623: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2629=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2629[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2630; _temp2630.tag= Cyc_Stmt_tok; _temp2630.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2630;});
_temp2629;}); break; case 244: _LL2628: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2632=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2632[ 0]=({ struct Cyc_Stmt_tok_struct _temp2633; _temp2633.tag= Cyc_Stmt_tok;
_temp2633.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2633;}); _temp2632;}); break; case 245: _LL2631: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2635=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2635[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2636; _temp2636.tag= Cyc_Stmt_tok; _temp2636.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2636;}); _temp2635;}); break; case 246: _LL2634: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2638=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2638[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2639; _temp2639.tag= Cyc_Stmt_tok; _temp2639.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2640=( struct Cyc_Absyn_SwitchC_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2640[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2641; _temp2641.tag= Cyc_Absyn_SwitchC_s;
_temp2641.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2641.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2641;});
_temp2640;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2639;}); _temp2638;}); break; case 247:
_LL2637: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2643=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2643[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2644; _temp2644.tag= Cyc_Stmt_tok; _temp2644.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2644;});
_temp2643;}); break; case 248: _LL2642: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2646=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2646[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2647; _temp2647.tag= Cyc_SwitchClauseList_tok; _temp2647.f1= 0; _temp2647;});
_temp2646;}); break; case 249: _LL2645: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2649=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2649[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2650; _temp2650.tag= Cyc_SwitchClauseList_tok; _temp2650.f1=({ struct Cyc_List_List*
_temp2651=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2651->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2652=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2652->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2652->pat_vars= 0; _temp2652->where_clause= 0; _temp2652->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2652->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2652;}); _temp2651->tl= 0; _temp2651;});
_temp2650;}); _temp2649;}); break; case 250: _LL2648: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2654=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2654[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2655; _temp2655.tag= Cyc_SwitchClauseList_tok;
_temp2655.f1=({ struct Cyc_List_List* _temp2656=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2656->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2657=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2657->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2657->pat_vars= 0; _temp2657->where_clause= 0;
_temp2657->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2657->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2657;});
_temp2656->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2656;}); _temp2655;});
_temp2654;}); break; case 251: _LL2653: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2659=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2659[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2660; _temp2660.tag= Cyc_SwitchClauseList_tok; _temp2660.f1=({ struct Cyc_List_List*
_temp2661=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2661->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2662=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2662->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2662->pat_vars=
0; _temp2662->where_clause= 0; _temp2662->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2662->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2662;}); _temp2661->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2661;});
_temp2660;}); _temp2659;}); break; case 252: _LL2658: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2664=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2664[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2665; _temp2665.tag= Cyc_SwitchClauseList_tok;
_temp2665.f1=({ struct Cyc_List_List* _temp2666=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2666->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2667=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2667->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp2667->pat_vars= 0; _temp2667->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2667->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2667->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2667;});
_temp2666->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2666;}); _temp2665;});
_temp2664;}); break; case 253: _LL2663: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2669=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2669[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2670; _temp2670.tag= Cyc_SwitchClauseList_tok; _temp2670.f1=({ struct Cyc_List_List*
_temp2671=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2671->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2672=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2672->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2672->pat_vars=
0; _temp2672->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2672->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2672->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2672;});
_temp2671->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2671;}); _temp2670;});
_temp2669;}); break; case 254: _LL2668: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2674=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2674[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2675; _temp2675.tag= Cyc_SwitchCClauseList_tok; _temp2675.f1= 0; _temp2675;});
_temp2674;}); break; case 255: _LL2673: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2677=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2677[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2678; _temp2678.tag= Cyc_SwitchCClauseList_tok; _temp2678.f1=({ struct Cyc_List_List*
_temp2679=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2679->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2680=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2680->cnst_exp= 0;
_temp2680->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2680->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2680;});
_temp2679->tl= 0; _temp2679;}); _temp2678;}); _temp2677;}); break; case 256:
_LL2676: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2682=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2682[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2683; _temp2683.tag=
Cyc_SwitchCClauseList_tok; _temp2683.f1=({ struct Cyc_List_List* _temp2684=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2684->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2685=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2685->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2685->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2685->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2685;});
_temp2684->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2684;}); _temp2683;});
_temp2682;}); break; case 257: _LL2681: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2687=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2687[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2688; _temp2688.tag= Cyc_SwitchCClauseList_tok; _temp2688.f1=({ struct Cyc_List_List*
_temp2689=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2689->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2690=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2690->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2690->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2690->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2690;});
_temp2689->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2689;}); _temp2688;});
_temp2687;}); break; case 258: _LL2686: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2692=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2692[ 0]=({ struct Cyc_Stmt_tok_struct _temp2693; _temp2693.tag= Cyc_Stmt_tok;
_temp2693.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2693;}); _temp2692;}); break; case 259: _LL2691: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2695=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2695[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2696; _temp2696.tag= Cyc_Stmt_tok; _temp2696.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2696;}); _temp2695;}); break; case 260: _LL2694: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2698=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2698[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2699; _temp2699.tag= Cyc_Stmt_tok; _temp2699.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2699;}); _temp2698;}); break; case 261: _LL2697: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2701=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2701[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2702; _temp2702.tag= Cyc_Stmt_tok; _temp2702.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2702;}); _temp2701;}); break; case 262: _LL2700: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2704=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2704[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2705; _temp2705.tag= Cyc_Stmt_tok; _temp2705.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2705;}); _temp2704;}); break; case 263: _LL2703: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2707=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2707[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2708; _temp2708.tag= Cyc_Stmt_tok; _temp2708.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2708;}); _temp2707;}); break; case 264: _LL2706: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2710=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2710[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2711; _temp2711.tag= Cyc_Stmt_tok; _temp2711.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2711;}); _temp2710;}); break; case 265: _LL2709: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2713=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2713[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2714; _temp2714.tag= Cyc_Stmt_tok; _temp2714.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2714;});
_temp2713;}); break; case 266: _LL2712: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2716=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2716[ 0]=({ struct Cyc_Stmt_tok_struct _temp2717; _temp2717.tag= Cyc_Stmt_tok;
_temp2717.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2717;});
_temp2716;}); break; case 267: _LL2715: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2719=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2719[ 0]=({ struct Cyc_Stmt_tok_struct _temp2720; _temp2720.tag= Cyc_Stmt_tok;
_temp2720.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2720;}); _temp2719;}); break; case 268: _LL2718: { struct Cyc_List_List*
_temp2722= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt* _temp2723= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2724=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2724[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2725; _temp2725.tag= Cyc_Stmt_tok; _temp2725.f1= Cyc_Parse_flatten_declarations(
_temp2722, _temp2723); _temp2725;}); _temp2724;}); break;} case 269: _LL2721: {
struct Cyc_List_List* _temp2727= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2728= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2729=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2729[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2730; _temp2730.tag= Cyc_Stmt_tok; _temp2730.f1= Cyc_Parse_flatten_declarations(
_temp2727, _temp2728); _temp2730;}); _temp2729;}); break;} case 270: _LL2726: {
struct Cyc_List_List* _temp2732= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2733= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2734=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2734[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2735; _temp2735.tag= Cyc_Stmt_tok; _temp2735.f1= Cyc_Parse_flatten_declarations(
_temp2732, _temp2733); _temp2735;}); _temp2734;}); break;} case 271: _LL2731: {
struct Cyc_List_List* _temp2737= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2738= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2739=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2739[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2740; _temp2740.tag= Cyc_Stmt_tok; _temp2740.f1= Cyc_Parse_flatten_declarations(
_temp2737, _temp2738); _temp2740;}); _temp2739;}); break;} case 272: _LL2736:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2742=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2742[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2743; _temp2743.tag= Cyc_Stmt_tok; _temp2743.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2744=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2744[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2744;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2743;}); _temp2742;}); break; case 273: _LL2741: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2746=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2746[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2747; _temp2747.tag= Cyc_Stmt_tok; _temp2747.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2747;}); _temp2746;}); break; case 274:
_LL2745: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2749=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2749[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2750; _temp2750.tag= Cyc_Stmt_tok; _temp2750.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2750;}); _temp2749;}); break; case 275: _LL2748: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2752=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2752[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2753; _temp2753.tag= Cyc_Stmt_tok; _temp2753.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2753;}); _temp2752;}); break; case 276:
_LL2751: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2755=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2755[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2756; _temp2756.tag= Cyc_Stmt_tok; _temp2756.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2756;}); _temp2755;}); break; case 277: _LL2754: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2758=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2758[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2759; _temp2759.tag= Cyc_Stmt_tok; _temp2759.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2759;}); _temp2758;}); break; case 278:
_LL2757: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2761=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2761[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2762; _temp2762.tag= Cyc_Stmt_tok; _temp2762.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2762;}); _temp2761;}); break; case 279:
_LL2760: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2764=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2764[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2765; _temp2765.tag= Cyc_Stmt_tok; _temp2765.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2765;}); _temp2764;}); break; case 280: _LL2763: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2767=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2767[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2768; _temp2768.tag= Cyc_Pattern_tok; _temp2768.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2768;});
_temp2767;}); break; case 281: _LL2766: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 282:
_LL2769: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2771=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2771[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2772; _temp2772.tag= Cyc_Pattern_tok; _temp2772.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2773=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2773[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2774; _temp2774.tag= Cyc_Absyn_Int_p; _temp2774.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2774.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2774;}); _temp2773;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2772;});
_temp2771;}); break; case 283: _LL2770: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2776=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2776[ 0]=({ struct Cyc_Pattern_tok_struct _temp2777; _temp2777.tag= Cyc_Pattern_tok;
_temp2777.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2778=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2778[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2779; _temp2779.tag= Cyc_Absyn_Int_p;
_temp2779.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2779.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2779;});
_temp2778;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2777;}); _temp2776;}); break; case 284:
_LL2775: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2781=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2781[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2782; _temp2782.tag= Cyc_Pattern_tok; _temp2782.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2783=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2783[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2784; _temp2784.tag= Cyc_Absyn_Float_p; _temp2784.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2784;}); _temp2783;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2782;});
_temp2781;}); break; case 285: _LL2780: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2786=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2786[ 0]=({ struct Cyc_Pattern_tok_struct _temp2787; _temp2787.tag= Cyc_Pattern_tok;
_temp2787.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2788=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2788[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2789; _temp2789.tag= Cyc_Absyn_Char_p; _temp2789.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2789;});
_temp2788;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2787;}); _temp2786;}); break; case 286:
_LL2785: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2791=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2791[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2792; _temp2792.tag= Cyc_Pattern_tok; _temp2792.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2792;});
_temp2791;}); break; case 287: _LL2790: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2794=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2794[ 0]=({ struct Cyc_Pattern_tok_struct _temp2795; _temp2795.tag= Cyc_Pattern_tok;
_temp2795.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2796=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2796[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2797; _temp2797.tag=
Cyc_Absyn_UnknownId_p; _temp2797.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2797;}); _temp2796;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2795;}); _temp2794;}); break; case 288:
_LL2793: { struct Cyc_List_List* _temp2799=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2800=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2800[ 0]=({ struct Cyc_Pattern_tok_struct _temp2801; _temp2801.tag= Cyc_Pattern_tok;
_temp2801.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2802=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2802[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2803; _temp2803.tag= Cyc_Absyn_UnknownCall_p; _temp2803.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2803.f2=
_temp2799; _temp2803.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2803;});
_temp2802;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2801;}); _temp2800;}); break;} case
289: _LL2798: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2805=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2805[ 0]=({ struct Cyc_Pattern_tok_struct _temp2806; _temp2806.tag= Cyc_Pattern_tok;
_temp2806.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2807=( struct Cyc_Absyn_Tuple_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2807[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2808; _temp2808.tag= Cyc_Absyn_Tuple_p;
_temp2808.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2808;}); _temp2807;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2806;}); _temp2805;}); break; case 290: _LL2804: { struct Cyc_List_List*
_temp2810=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); yyval=(
void*)({ struct Cyc_Pattern_tok_struct* _temp2811=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2811[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2812; _temp2812.tag= Cyc_Pattern_tok; _temp2812.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2813=( struct Cyc_Absyn_UnknownFields_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2813[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2814; _temp2814.tag= Cyc_Absyn_UnknownFields_p;
_temp2814.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2814.f2= _temp2810; _temp2814.f3= 0; _temp2814;});
_temp2813;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2812;}); _temp2811;}); break;} case
291: _LL2809: { struct Cyc_List_List* _temp2816=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2817=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2817[ 0]=({ struct Cyc_Pattern_tok_struct _temp2818; _temp2818.tag= Cyc_Pattern_tok;
_temp2818.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2819=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2819[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2820; _temp2820.tag= Cyc_Absyn_UnknownFields_p; _temp2820.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2820.f2=
_temp2816; _temp2820.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2820;});
_temp2819;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2818;}); _temp2817;}); break;} case
292: _LL2815: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2822=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2822[ 0]=({ struct Cyc_Pattern_tok_struct _temp2823; _temp2823.tag= Cyc_Pattern_tok;
_temp2823.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2824=( struct Cyc_Absyn_Pointer_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2824[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2825; _temp2825.tag= Cyc_Absyn_Pointer_p;
_temp2825.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2825;}); _temp2824;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2823;});
_temp2822;}); break; case 293: _LL2821: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2827=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2827[ 0]=({ struct Cyc_Pattern_tok_struct _temp2828; _temp2828.tag= Cyc_Pattern_tok;
_temp2828.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2829=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2829[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2830; _temp2830.tag=
Cyc_Absyn_Reference_p; _temp2830.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2831=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2831->f1=(
void*) Cyc_Absyn_Loc_n; _temp2831->f2=({ struct _tagged_arr* _temp2832=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2832[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2832;});
_temp2831;}),( void*) Cyc_Absyn_VoidType, 0); _temp2830;}); _temp2829;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2828;}); _temp2827;}); break; case 294: _LL2826: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2834=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2834[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2835; _temp2835.tag= Cyc_PatternList_tok;
_temp2835.f1= 0; _temp2835;}); _temp2834;}); break; case 295: _LL2833: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2837=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2837[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2838; _temp2838.tag= Cyc_PatternList_tok;
_temp2838.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2838;}); _temp2837;}); break; case 296: _LL2836: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2840=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2840[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2841; _temp2841.tag= Cyc_PatternList_tok;
_temp2841.f1=({ struct Cyc_List_List* _temp2842=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2842->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2842->tl= 0;
_temp2842;}); _temp2841;}); _temp2840;}); break; case 297: _LL2839: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2844=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2844[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2845; _temp2845.tag= Cyc_PatternList_tok;
_temp2845.f1=({ struct Cyc_List_List* _temp2846=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2846->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2846->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2846;}); _temp2845;}); _temp2844;}); break; case 298:
_LL2843: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2848=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2848[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2849; _temp2849.tag=
Cyc_FieldPattern_tok; _temp2849.f1=({ struct _tuple13* _temp2850=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2850->f1= 0; _temp2850->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2850;}); _temp2849;}); _temp2848;}); break; case 299:
_LL2847: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2852=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2852[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2853; _temp2853.tag=
Cyc_FieldPattern_tok; _temp2853.f1=({ struct _tuple13* _temp2854=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2854->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2854->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2854;}); _temp2853;}); _temp2852;}); break; case 300:
_LL2851: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2856=(
struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2856[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2857; _temp2857.tag=
Cyc_FieldPatternList_tok; _temp2857.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2857;});
_temp2856;}); break; case 301: _LL2855: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2859=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2859[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2860; _temp2860.tag= Cyc_FieldPatternList_tok; _temp2860.f1=({ struct Cyc_List_List*
_temp2861=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2861->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2861->tl= 0;
_temp2861;}); _temp2860;}); _temp2859;}); break; case 302: _LL2858: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2863=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2863[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2864; _temp2864.tag= Cyc_FieldPatternList_tok;
_temp2864.f1=({ struct Cyc_List_List* _temp2865=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2865->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2865->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2865;}); _temp2864;}); _temp2863;}); break; case 303:
_LL2862: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 304: _LL2866: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2868=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2868[ 0]=({ struct Cyc_Exp_tok_struct _temp2869; _temp2869.tag= Cyc_Exp_tok;
_temp2869.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2869;}); _temp2868;}); break; case 305: _LL2867: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 306: _LL2870:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2872=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2872[ 0]=({ struct Cyc_Exp_tok_struct
_temp2873; _temp2873.tag= Cyc_Exp_tok; _temp2873.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2873;}); _temp2872;}); break; case 307: _LL2871: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2875=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2875[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2876; _temp2876.tag= Cyc_Primopopt_tok; _temp2876.f1=
0; _temp2876;}); _temp2875;}); break; case 308: _LL2874: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2878=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2878[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2879; _temp2879.tag= Cyc_Primopopt_tok; _temp2879.f1=({
struct Cyc_Core_Opt* _temp2880=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2880->v=( void*)(( void*) Cyc_Absyn_Times); _temp2880;});
_temp2879;}); _temp2878;}); break; case 309: _LL2877: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2882=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2882[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2883; _temp2883.tag= Cyc_Primopopt_tok; _temp2883.f1=({
struct Cyc_Core_Opt* _temp2884=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2884->v=( void*)(( void*) Cyc_Absyn_Div); _temp2884;});
_temp2883;}); _temp2882;}); break; case 310: _LL2881: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2886=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2886[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2887; _temp2887.tag= Cyc_Primopopt_tok; _temp2887.f1=({
struct Cyc_Core_Opt* _temp2888=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2888->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2888;});
_temp2887;}); _temp2886;}); break; case 311: _LL2885: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2890=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2890[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2891; _temp2891.tag= Cyc_Primopopt_tok; _temp2891.f1=({
struct Cyc_Core_Opt* _temp2892=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2892->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2892;});
_temp2891;}); _temp2890;}); break; case 312: _LL2889: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2894=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2894[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2895; _temp2895.tag= Cyc_Primopopt_tok; _temp2895.f1=({
struct Cyc_Core_Opt* _temp2896=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2896->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2896;});
_temp2895;}); _temp2894;}); break; case 313: _LL2893: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2898=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2898[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2899; _temp2899.tag= Cyc_Primopopt_tok; _temp2899.f1=({
struct Cyc_Core_Opt* _temp2900=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2900->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2900;});
_temp2899;}); _temp2898;}); break; case 314: _LL2897: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2902=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2902[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2903; _temp2903.tag= Cyc_Primopopt_tok; _temp2903.f1=({
struct Cyc_Core_Opt* _temp2904=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2904->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2904;});
_temp2903;}); _temp2902;}); break; case 315: _LL2901: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2906=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2906[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2907; _temp2907.tag= Cyc_Primopopt_tok; _temp2907.f1=({
struct Cyc_Core_Opt* _temp2908=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2908->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2908;});
_temp2907;}); _temp2906;}); break; case 316: _LL2905: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2910=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2910[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2911; _temp2911.tag= Cyc_Primopopt_tok; _temp2911.f1=({
struct Cyc_Core_Opt* _temp2912=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2912->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2912;});
_temp2911;}); _temp2910;}); break; case 317: _LL2909: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2914=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2914[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2915; _temp2915.tag= Cyc_Primopopt_tok; _temp2915.f1=({
struct Cyc_Core_Opt* _temp2916=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2916->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2916;});
_temp2915;}); _temp2914;}); break; case 318: _LL2913: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 319: _LL2917:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2919=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2919[ 0]=({ struct Cyc_Exp_tok_struct
_temp2920; _temp2920.tag= Cyc_Exp_tok; _temp2920.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2920;});
_temp2919;}); break; case 320: _LL2918: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2922=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2922[ 0]=({ struct Cyc_Exp_tok_struct _temp2923; _temp2923.tag= Cyc_Exp_tok;
_temp2923.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2923;}); _temp2922;}); break; case 321: _LL2921: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2925=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2925[ 0]=({ struct Cyc_Exp_tok_struct
_temp2926; _temp2926.tag= Cyc_Exp_tok; _temp2926.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2926;}); _temp2925;}); break; case 322: _LL2924: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2928=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2928[ 0]=({ struct Cyc_Exp_tok_struct
_temp2929; _temp2929.tag= Cyc_Exp_tok; _temp2929.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2929;}); _temp2928;}); break; case 323: _LL2927: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2931=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2931[ 0]=({ struct Cyc_Exp_tok_struct
_temp2932; _temp2932.tag= Cyc_Exp_tok; _temp2932.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2932;});
_temp2931;}); break; case 324: _LL2930: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2934=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2934[ 0]=({ struct Cyc_Exp_tok_struct _temp2935; _temp2935.tag= Cyc_Exp_tok;
_temp2935.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2935;}); _temp2934;}); break; case 325: _LL2933: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 326: _LL2936:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
327: _LL2937: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2939=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2939[ 0]=({ struct Cyc_Exp_tok_struct
_temp2940; _temp2940.tag= Cyc_Exp_tok; _temp2940.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2940;}); _temp2939;}); break; case 328: _LL2938: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL2941:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2943=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2943[ 0]=({ struct Cyc_Exp_tok_struct
_temp2944; _temp2944.tag= Cyc_Exp_tok; _temp2944.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2944;}); _temp2943;}); break; case 330: _LL2942: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL2945:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2947=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2947[ 0]=({ struct Cyc_Exp_tok_struct
_temp2948; _temp2948.tag= Cyc_Exp_tok; _temp2948.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2948;});
_temp2947;}); break; case 332: _LL2946: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL2949:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2951=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2951[ 0]=({ struct Cyc_Exp_tok_struct
_temp2952; _temp2952.tag= Cyc_Exp_tok; _temp2952.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2952;});
_temp2951;}); break; case 334: _LL2950: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL2953:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2955=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2955[ 0]=({ struct Cyc_Exp_tok_struct
_temp2956; _temp2956.tag= Cyc_Exp_tok; _temp2956.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2956;});
_temp2955;}); break; case 336: _LL2954: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 337: _LL2957:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2959=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2959[ 0]=({ struct Cyc_Exp_tok_struct
_temp2960; _temp2960.tag= Cyc_Exp_tok; _temp2960.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2960;}); _temp2959;}); break; case 338: _LL2958: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2962=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2962[ 0]=({ struct Cyc_Exp_tok_struct
_temp2963; _temp2963.tag= Cyc_Exp_tok; _temp2963.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2963;}); _temp2962;}); break; case 339: _LL2961: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 340: _LL2964:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2966=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2966[ 0]=({ struct Cyc_Exp_tok_struct
_temp2967; _temp2967.tag= Cyc_Exp_tok; _temp2967.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2967;}); _temp2966;}); break; case 341: _LL2965: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2969=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2969[ 0]=({ struct Cyc_Exp_tok_struct
_temp2970; _temp2970.tag= Cyc_Exp_tok; _temp2970.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2970;}); _temp2969;}); break; case 342: _LL2968: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2972=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2972[ 0]=({ struct Cyc_Exp_tok_struct
_temp2973; _temp2973.tag= Cyc_Exp_tok; _temp2973.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2973;}); _temp2972;}); break; case 343: _LL2971: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2975=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2975[ 0]=({ struct Cyc_Exp_tok_struct
_temp2976; _temp2976.tag= Cyc_Exp_tok; _temp2976.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2976;}); _temp2975;}); break; case 344: _LL2974: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 345: _LL2977:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2979=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2979[ 0]=({ struct Cyc_Exp_tok_struct
_temp2980; _temp2980.tag= Cyc_Exp_tok; _temp2980.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2980;}); _temp2979;}); break; case 346: _LL2978: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2982=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2982[ 0]=({ struct Cyc_Exp_tok_struct
_temp2983; _temp2983.tag= Cyc_Exp_tok; _temp2983.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2983;}); _temp2982;}); break; case 347: _LL2981: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 348: _LL2984:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2986=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2986[ 0]=({ struct Cyc_Exp_tok_struct
_temp2987; _temp2987.tag= Cyc_Exp_tok; _temp2987.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2987;});
_temp2986;}); break; case 349: _LL2985: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2989=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2989[ 0]=({ struct Cyc_Exp_tok_struct _temp2990; _temp2990.tag= Cyc_Exp_tok;
_temp2990.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2990;}); _temp2989;}); break; case 350: _LL2988: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 351: _LL2991:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2993=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2993[ 0]=({ struct Cyc_Exp_tok_struct
_temp2994; _temp2994.tag= Cyc_Exp_tok; _temp2994.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2994;});
_temp2993;}); break; case 352: _LL2992: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2996=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2996[ 0]=({ struct Cyc_Exp_tok_struct _temp2997; _temp2997.tag= Cyc_Exp_tok;
_temp2997.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2997;}); _temp2996;}); break; case 353: _LL2995: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2999=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2999[ 0]=({ struct Cyc_Exp_tok_struct
_temp3000; _temp3000.tag= Cyc_Exp_tok; _temp3000.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3000;});
_temp2999;}); break; case 354: _LL2998: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 355: _LL3001:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3003=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3003[ 0]=({ struct Cyc_Exp_tok_struct
_temp3004; _temp3004.tag= Cyc_Exp_tok; _temp3004.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3004;}); _temp3003;}); break; case 356: _LL3002: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 357: _LL3005:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3007=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3007[ 0]=({ struct Cyc_Exp_tok_struct
_temp3008; _temp3008.tag= Cyc_Exp_tok; _temp3008.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3008;}); _temp3007;}); break; case 358: _LL3006: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3010=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3010[ 0]=({ struct Cyc_Exp_tok_struct
_temp3011; _temp3011.tag= Cyc_Exp_tok; _temp3011.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3011;}); _temp3010;}); break; case 359: _LL3009: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3013=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3013[ 0]=({ struct Cyc_Exp_tok_struct
_temp3014; _temp3014.tag= Cyc_Exp_tok; _temp3014.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3014;}); _temp3013;}); break; case 360: _LL3012: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3016=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3016[ 0]=({ struct Cyc_Exp_tok_struct
_temp3017; _temp3017.tag= Cyc_Exp_tok; _temp3017.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3017;}); _temp3016;}); break; case 361: _LL3015: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 362: _LL3018:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3020=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3020[ 0]=({ struct Cyc_Exp_tok_struct
_temp3021; _temp3021.tag= Cyc_Exp_tok; _temp3021.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3021;}); _temp3020;}); break; case 363: _LL3019: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3023=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3023[ 0]=({ struct Cyc_Exp_tok_struct
_temp3024; _temp3024.tag= Cyc_Exp_tok; _temp3024.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3024;});
_temp3023;}); break; case 364: _LL3022: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3026=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3026[ 0]=({ struct Cyc_Exp_tok_struct _temp3027; _temp3027.tag= Cyc_Exp_tok;
_temp3027.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3027;}); _temp3026;}); break; case 365: _LL3025: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3029=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3029[ 0]=({ struct Cyc_Exp_tok_struct
_temp3030; _temp3030.tag= Cyc_Exp_tok; _temp3030.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,({ struct _tagged_arr* _temp3031=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp3031[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3031;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3030;}); _temp3029;}); break; case 366:
_LL3028: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3033=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3033[ 0]=({ struct Cyc_Exp_tok_struct
_temp3034; _temp3034.tag= Cyc_Exp_tok; _temp3034.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3035=( struct Cyc_Absyn_Malloc_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3035[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3036; _temp3036.tag= Cyc_Absyn_Malloc_e;
_temp3036.f1= 0; _temp3036.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3036;}); _temp3035;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3034;});
_temp3033;}); break; case 367: _LL3032: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3038=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3038[ 0]=({ struct Cyc_Exp_tok_struct _temp3039; _temp3039.tag= Cyc_Exp_tok;
_temp3039.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3040=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3040[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3041; _temp3041.tag= Cyc_Absyn_Malloc_e;
_temp3041.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3041.f2=(
void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3041;}); _temp3040;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3039;});
_temp3038;}); break; case 368: _LL3037: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3043=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3043[ 0]=({ struct Cyc_Primop_tok_struct _temp3044; _temp3044.tag= Cyc_Primop_tok;
_temp3044.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3044;}); _temp3043;});
break; case 369: _LL3042: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3046=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3046[ 0]=({ struct Cyc_Primop_tok_struct _temp3047; _temp3047.tag= Cyc_Primop_tok;
_temp3047.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3047;}); _temp3046;});
break; case 370: _LL3045: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3049=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3049[ 0]=({ struct Cyc_Primop_tok_struct _temp3050; _temp3050.tag= Cyc_Primop_tok;
_temp3050.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3050;}); _temp3049;});
break; case 371: _LL3048: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 372: _LL3051: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3053=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3053[ 0]=({ struct Cyc_Exp_tok_struct _temp3054; _temp3054.tag= Cyc_Exp_tok;
_temp3054.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3054;}); _temp3053;}); break; case 373: _LL3052: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3056=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3056[ 0]=({ struct Cyc_Exp_tok_struct
_temp3057; _temp3057.tag= Cyc_Exp_tok; _temp3057.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3057;}); _temp3056;}); break; case 374:
_LL3055: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3059=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3059[ 0]=({ struct Cyc_Exp_tok_struct
_temp3060; _temp3060.tag= Cyc_Exp_tok; _temp3060.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3060;});
_temp3059;}); break; case 375: _LL3058: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3062=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3062[ 0]=({ struct Cyc_Exp_tok_struct _temp3063; _temp3063.tag= Cyc_Exp_tok;
_temp3063.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3064=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3064[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3064;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3063;}); _temp3062;}); break; case 376: _LL3061: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3066=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3066[ 0]=({ struct Cyc_Exp_tok_struct
_temp3067; _temp3067.tag= Cyc_Exp_tok; _temp3067.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3067;});
_temp3066;}); break;} case 377: _LL3065: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3069=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3069[ 0]=({ struct Cyc_Exp_tok_struct _temp3070; _temp3070.tag= Cyc_Exp_tok;
_temp3070.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3071=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3071[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3071;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3070;}); _temp3069;}); break; case 378: _LL3068: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3073=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3073[ 0]=({ struct Cyc_Exp_tok_struct
_temp3074; _temp3074.tag= Cyc_Exp_tok; _temp3074.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3074;});
_temp3073;}); break;} case 379: _LL3072: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3076=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3076[ 0]=({ struct Cyc_Exp_tok_struct _temp3077; _temp3077.tag= Cyc_Exp_tok;
_temp3077.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3077;}); _temp3076;}); break; case 380: _LL3075: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3079=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3079[ 0]=({ struct Cyc_Exp_tok_struct
_temp3080; _temp3080.tag= Cyc_Exp_tok; _temp3080.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3080;}); _temp3079;}); break; case 381: _LL3078: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3082=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3082[ 0]=({ struct Cyc_Exp_tok_struct
_temp3083; _temp3083.tag= Cyc_Exp_tok; _temp3083.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3084=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3084[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3085; _temp3085.tag= Cyc_Absyn_CompoundLit_e;
_temp3085.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3085.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3085;});
_temp3084;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3083;}); _temp3082;}); break; case 382:
_LL3081: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3087=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3087[ 0]=({ struct Cyc_Exp_tok_struct
_temp3088; _temp3088.tag= Cyc_Exp_tok; _temp3088.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3089=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3089[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3090; _temp3090.tag= Cyc_Absyn_CompoundLit_e;
_temp3090.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3090.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3090;});
_temp3089;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3088;}); _temp3087;}); break; case 383:
_LL3086: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3092=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3092[ 0]=({ struct Cyc_Exp_tok_struct
_temp3093; _temp3093.tag= Cyc_Exp_tok; _temp3093.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3094=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3094[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3095; _temp3095.tag= Cyc_Absyn_Fill_e; _temp3095.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3095;});
_temp3094;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3093;}); _temp3092;}); break; case 384:
_LL3091: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3097=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3097[ 0]=({ struct Cyc_Exp_tok_struct
_temp3098; _temp3098.tag= Cyc_Exp_tok; _temp3098.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3099=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3099[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3100; _temp3100.tag= Cyc_Absyn_Codegen_e;
_temp3100.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3100;}); _temp3099;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3098;}); _temp3097;}); break; case 385: _LL3096: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3102=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3102[ 0]=({ struct Cyc_Exp_tok_struct
_temp3103; _temp3103.tag= Cyc_Exp_tok; _temp3103.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3104=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3104[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3105; _temp3105.tag= Cyc_Absyn_UnknownId_e;
_temp3105.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3105;}); _temp3104;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3103;});
_temp3102;}); break; case 386: _LL3101: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 387: _LL3106:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3108=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3108[ 0]=({ struct Cyc_Exp_tok_struct
_temp3109; _temp3109.tag= Cyc_Exp_tok; _temp3109.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3109;});
_temp3108;}); break; case 388: _LL3107: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 389:
_LL3110: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3112=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3112[ 0]=({ struct Cyc_Exp_tok_struct
_temp3113; _temp3113.tag= Cyc_Exp_tok; _temp3113.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3114=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3114[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3115; _temp3115.tag=
Cyc_Absyn_UnknownId_e; _temp3115.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3115;});
_temp3114;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3113;});
_temp3112;}); break; case 390: _LL3111: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3117=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3117[ 0]=({ struct Cyc_Exp_tok_struct _temp3118; _temp3118.tag= Cyc_Exp_tok;
_temp3118.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3119=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3119[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3120; _temp3120.tag=
Cyc_Absyn_UnknownId_e; _temp3120.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3120;});
_temp3119;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3118;}); _temp3117;}); break; case 391: _LL3116: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3122=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3122[ 0]=({ struct Cyc_Exp_tok_struct
_temp3123; _temp3123.tag= Cyc_Exp_tok; _temp3123.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3123;}); _temp3122;}); break; case 392: _LL3121: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3125=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3125[ 0]=({ struct Cyc_Exp_tok_struct
_temp3126; _temp3126.tag= Cyc_Exp_tok; _temp3126.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3127=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3127[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3128; _temp3128.tag= Cyc_Absyn_Struct_e;
_temp3128.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3128.f2= 0; _temp3128.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3128.f4= 0;
_temp3128;}); _temp3127;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3126;});
_temp3125;}); break; case 393: _LL3124: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3130=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3130[ 0]=({ struct Cyc_Exp_tok_struct _temp3131; _temp3131.tag= Cyc_Exp_tok;
_temp3131.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3131;}); _temp3130;}); break; case 394: _LL3129: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3133=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3133[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3134; _temp3134.tag= Cyc_ExpList_tok; _temp3134.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3134;});
_temp3133;}); break; case 395: _LL3132: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3136=( struct Cyc_ExpList_tok_struct*) _cycalloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3136[ 0]=({ struct Cyc_ExpList_tok_struct _temp3137; _temp3137.tag= Cyc_ExpList_tok;
_temp3137.f1=({ struct Cyc_List_List* _temp3138=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3138->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3138->tl= 0;
_temp3138;}); _temp3137;}); _temp3136;}); break; case 396: _LL3135: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3140=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3140[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3141; _temp3141.tag= Cyc_ExpList_tok; _temp3141.f1=({ struct Cyc_List_List*
_temp3142=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3142->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3142->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp3142;});
_temp3141;}); _temp3140;}); break; case 397: _LL3139: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3144=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3144[ 0]=({ struct Cyc_Exp_tok_struct
_temp3145; _temp3145.tag= Cyc_Exp_tok; _temp3145.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3145;});
_temp3144;}); break; case 398: _LL3143: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3147=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3147[ 0]=({ struct Cyc_Exp_tok_struct _temp3148; _temp3148.tag= Cyc_Exp_tok;
_temp3148.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3148;});
_temp3147;}); break; case 399: _LL3146: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3150=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3150[ 0]=({ struct Cyc_Exp_tok_struct _temp3151; _temp3151.tag= Cyc_Exp_tok;
_temp3151.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3151;});
_temp3150;}); break; case 400: _LL3149: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3153=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3153[ 0]=({ struct Cyc_Exp_tok_struct _temp3154; _temp3154.tag= Cyc_Exp_tok;
_temp3154.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3154;});
_temp3153;}); break; case 401: _LL3152: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3156=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3156[ 0]=({ struct Cyc_QualId_tok_struct _temp3157; _temp3157.tag= Cyc_QualId_tok;
_temp3157.f1=({ struct _tuple1* _temp3158=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3158->f1= Cyc_Absyn_rel_ns_null; _temp3158->f2=({ struct
_tagged_arr* _temp3159=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3159[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3159;}); _temp3158;});
_temp3157;}); _temp3156;}); break; case 402: _LL3155: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3160:(
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
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 403u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 112u, yyn -  120)] +  yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >=  0?
yystate <=  4390: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4391u,
yystate)] ==  yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){
yystate=( int) Cyc_yytable[ _check_known_subscript_notnull( 4391u, yystate)];}
else{ yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 
120)];} goto yynewstate; yyerrlab: if( yyerrstatus ==  0){ ++ Cyc_yynerrs; yyn=(
int) Cyc_yypact[ _check_known_subscript_notnull( 794u, yystate)]; if( yyn >  -
32768? yyn <  4390: 0){ int sze= 0; struct _tagged_arr msg; int x; int count;
count= 0; for( x= yyn <  0? - yyn: 0; x <  232u /  sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4391u, x +  yyn)] ==  x){( sze
+= Cyc_Std_strlen( Cyc_yytname[ _check_known_subscript_notnull( 232u, x)]) +  15,
count ++);}} msg=({ unsigned int _temp3162=( unsigned int)( sze +  15);
unsigned char* _temp3163=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp3162)); struct _tagged_arr _temp3165= _tag_arr(
_temp3163, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp3164= _temp3162; unsigned int i; for( i= 0; i <  _temp3164; i ++){
_temp3163[ i]='\000';}}; _temp3165;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
sizeof( unsigned char), 12u)); if( count <  5){ count= 0; for( x= yyn <  0? -
yyn: 0; x <  232u /  sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4391u, x +  yyn)] ==  x){ Cyc_Std_strcat( msg,
count ==  0? _tag_arr(", expecting `", sizeof( unsigned char), 14u): _tag_arr(" or `",
sizeof( unsigned char), 6u)); Cyc_Std_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 232u, x)]); Cyc_Std_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus ==  3){ if( Cyc_yychar ==  0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset ==  0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 794u,
yystate)]; if( yyn ==  - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn <  0? 1:
yyn >  4390)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4391u, yyn)] !=  1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4391u,
yyn)]; if( yyn <  0){ if( yyn ==  - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn ==  0){ goto yyerrpop;}} if( yyn ==  793){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3175= v; struct
_tuple15* _temp3193; struct _tuple15 _temp3195; int _temp3196; unsigned char
_temp3198; short _temp3200; struct _tagged_arr _temp3202; struct Cyc_Core_Opt*
_temp3204; struct Cyc_Core_Opt* _temp3206; struct Cyc_Core_Opt _temp3208; struct
_tagged_arr* _temp3209; struct _tuple1* _temp3211; struct _tuple1 _temp3213;
struct _tagged_arr* _temp3214; void* _temp3216; _LL3177: if(*(( void**)
_temp3175) ==  Cyc_Int_tok){ _LL3194: _temp3193=(( struct Cyc_Int_tok_struct*)
_temp3175)->f1; _temp3195=* _temp3193; _LL3197: _temp3196= _temp3195.f2; goto
_LL3178;} else{ goto _LL3179;} _LL3179: if(*(( void**) _temp3175) ==  Cyc_Char_tok){
_LL3199: _temp3198=(( struct Cyc_Char_tok_struct*) _temp3175)->f1; goto _LL3180;}
else{ goto _LL3181;} _LL3181: if(*(( void**) _temp3175) ==  Cyc_Short_tok){
_LL3201: _temp3200=(( struct Cyc_Short_tok_struct*) _temp3175)->f1; goto _LL3182;}
else{ goto _LL3183;} _LL3183: if(*(( void**) _temp3175) ==  Cyc_String_tok){
_LL3203: _temp3202=(( struct Cyc_String_tok_struct*) _temp3175)->f1; goto
_LL3184;} else{ goto _LL3185;} _LL3185: if(*(( void**) _temp3175) ==  Cyc_Stringopt_tok){
_LL3205: _temp3204=(( struct Cyc_Stringopt_tok_struct*) _temp3175)->f1; if(
_temp3204 ==  0){ goto _LL3186;} else{ goto _LL3187;}} else{ goto _LL3187;}
_LL3187: if(*(( void**) _temp3175) ==  Cyc_Stringopt_tok){ _LL3207: _temp3206=((
struct Cyc_Stringopt_tok_struct*) _temp3175)->f1; if( _temp3206 ==  0){ goto
_LL3189;} else{ _temp3208=* _temp3206; _LL3210: _temp3209=( struct _tagged_arr*)
_temp3208.v; goto _LL3188;}} else{ goto _LL3189;} _LL3189: if(*(( void**)
_temp3175) ==  Cyc_QualId_tok){ _LL3212: _temp3211=(( struct Cyc_QualId_tok_struct*)
_temp3175)->f1; _temp3213=* _temp3211; _LL3217: _temp3216= _temp3213.f1; goto
_LL3215; _LL3215: _temp3214= _temp3213.f2; goto _LL3190;} else{ goto _LL3191;}
_LL3191: goto _LL3192; _LL3178:({ struct Cyc_Std_Int_pa_struct _temp3219;
_temp3219.tag= Cyc_Std_Int_pa; _temp3219.f1=( int)(( unsigned int) _temp3196);{
void* _temp3218[ 1u]={& _temp3219}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3218, sizeof( void*), 1u));}}); goto
_LL3176; _LL3180:({ struct Cyc_Std_Int_pa_struct _temp3221; _temp3221.tag= Cyc_Std_Int_pa;
_temp3221.f1=( int)(( unsigned int)(( int) _temp3198));{ void* _temp3220[ 1u]={&
_temp3221}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3220, sizeof( void*), 1u));}}); goto _LL3176;
_LL3182:({ struct Cyc_Std_Int_pa_struct _temp3223; _temp3223.tag= Cyc_Std_Int_pa;
_temp3223.f1=( int)(( unsigned int)(( int) _temp3200));{ void* _temp3222[ 1u]={&
_temp3223}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3222, sizeof( void*), 1u));}}); goto _LL3176;
_LL3184:({ struct Cyc_Std_String_pa_struct _temp3225; _temp3225.tag= Cyc_Std_String_pa;
_temp3225.f1=( struct _tagged_arr) _temp3202;{ void* _temp3224[ 1u]={& _temp3225};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3224, sizeof( void*), 1u));}}); goto _LL3176; _LL3186:({ void*
_temp3226[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3226, sizeof( void*), 0u));}); goto _LL3176;
_LL3188:({ struct Cyc_Std_String_pa_struct _temp3228; _temp3228.tag= Cyc_Std_String_pa;
_temp3228.f1=( struct _tagged_arr)* _temp3209;{ void* _temp3227[ 1u]={&
_temp3228}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3227, sizeof( void*), 1u));}}); goto _LL3176;
_LL3190: { struct Cyc_List_List* _temp3229= 0;{ void* _temp3230= _temp3216;
struct Cyc_List_List* _temp3238; struct Cyc_List_List* _temp3240; _LL3232: if((
unsigned int) _temp3230 >  1u?*(( int*) _temp3230) ==  Cyc_Absyn_Rel_n: 0){
_LL3239: _temp3238=(( struct Cyc_Absyn_Rel_n_struct*) _temp3230)->f1; goto
_LL3233;} else{ goto _LL3234;} _LL3234: if(( unsigned int) _temp3230 >  1u?*((
int*) _temp3230) ==  Cyc_Absyn_Abs_n: 0){ _LL3241: _temp3240=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3230)->f1; goto _LL3235;} else{ goto _LL3236;} _LL3236: if( _temp3230 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3237;} else{ goto _LL3231;} _LL3233: _temp3229=
_temp3238; goto _LL3231; _LL3235: _temp3229= _temp3240; goto _LL3231; _LL3237:
goto _LL3231; _LL3231:;} for( 0; _temp3229 !=  0; _temp3229=(( struct Cyc_List_List*)
_check_null( _temp3229))->tl){({ struct Cyc_Std_String_pa_struct _temp3243;
_temp3243.tag= Cyc_Std_String_pa; _temp3243.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp3229))->hd);{ void*
_temp3242[ 1u]={& _temp3243}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3242, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3245; _temp3245.tag= Cyc_Std_String_pa;
_temp3245.f1=( struct _tagged_arr)* _temp3214;{ void* _temp3244[ 1u]={&
_temp3245}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3244, sizeof( void*), 1u));}}); goto _LL3176;}
_LL3192:({ void* _temp3246[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3246, sizeof( void*), 0u));}); goto
_LL3176; _LL3176:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3247=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3247->v=(
void*) Cyc_Lexing_from_file( f); _temp3247;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
