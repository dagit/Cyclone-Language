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
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1; static
const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3; static
const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1; int
q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0; static
const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const int
Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
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
15u]; extern void* Cyc_Absyn_rel_ns_null; extern int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual( struct
Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt*
k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*);
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t( int); extern
struct _tuple1* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one;
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
int tag; struct Cyc_Absyn_Decl* f1; } ; static const int Cyc_Parse_Typedef_sc= 0;
static const int Cyc_Parse_Extern_sc= 1; static const int Cyc_Parse_ExternC_sc=
2; static const int Cyc_Parse_Static_sc= 3; static const int Cyc_Parse_Auto_sc=
4; static const int Cyc_Parse_Register_sc= 5; static const int Cyc_Parse_Abstract_sc=
6; struct Cyc_Parse_Declaration_spec{ struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual
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
Cyc_Absyn_is_qvar_qualified(( struct _tuple1*) _temp53->v)){ Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace", sizeof( unsigned char),
47u), loc);} _temp55=({ struct Cyc_Core_Opt* _temp56=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp56->v=( void*)(*(( struct _tuple1*)
_temp53->v)).f2; _temp56;});} if( _temp47 !=  0){(( int(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("parameter should have no type parameters",
sizeof( unsigned char), 41u), loc);} return({ struct _tuple2* _temp57=( struct
_tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp57->f1= _temp55; _temp57->f2=
_temp51; _temp57->f3= _temp49; _temp57;});}} static void* Cyc_Parse_type_spec(
void* t, struct Cyc_Position_Segment* loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp58=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp58[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp59; _temp59.tag= Cyc_Parse_Type_spec;
_temp59.f1=( void*) t; _temp59.f2= loc; _temp59;}); _temp58;});} static void*
Cyc_Parse_array2ptr( void* t){ void* _temp60= t; struct Cyc_Absyn_Exp* _temp66;
struct Cyc_Absyn_Tqual _temp68; void* _temp70; _LL62: if(( unsigned int) _temp60
>  3u?*(( int*) _temp60) ==  Cyc_Absyn_ArrayType: 0){ _LL71: _temp70=( void*)((
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
params= params->tl){ if( Cyc_Std_zstrptrcmp((* _temp101->name).f2,( struct
_tagged_arr*) params->hd) ==  0){ found= 1; break;}} if( ! found){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp104; _temp104.tag=
Cyc_Std_String_pa; _temp104.f1=( struct _tagged_arr)*(* _temp101->name).f2;{
void* _temp103[ 1u]={& _temp104}; Cyc_Std_aprintf( _tag_arr("%s is not listed as a parameter",
sizeof( unsigned char), 32u), _tag_arr( _temp103, sizeof( void*), 1u));}}), x->loc);}
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
struct Cyc_Absyn_Decl*) _temp112->hd)->r; struct Cyc_Absyn_Vardecl* _temp122;
_LL118: if(*(( int*) _temp116) ==  Cyc_Absyn_Var_d){ _LL123: _temp122=(( struct
Cyc_Absyn_Var_d_struct*) _temp116)->f1; goto _LL119;} else{ goto _LL120;} _LL120:
goto _LL121; _LL119: if( Cyc_Absyn_is_qvar_qualified( _temp122->name)){ Cyc_Parse_err(
_tag_arr("namespace not allowed on parameter", sizeof( unsigned char), 35u),
_temp110);} if( Cyc_Std_zstrptrcmp((* _temp122->name).f2, x) ==  0){ return({
struct _tuple2* _temp124=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp124->f1=({ struct Cyc_Core_Opt* _temp125=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp125->v=( void*)(* _temp122->name).f2;
_temp125;}); _temp124->f2= _temp122->tq; _temp124->f3=( void*) _temp122->type;
_temp124;});} else{ return Cyc_Parse_get_param_type(({ struct _tuple12* _temp126=(
struct _tuple12*) _cycalloc( sizeof( struct _tuple12)); _temp126->f1= _temp112->tl;
_temp126->f2= _temp110; _temp126;}), x);} _LL121: return(( struct _tuple2*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("non-variable declaration", sizeof( unsigned char), 25u),(( struct Cyc_Absyn_Decl*)
_temp112->hd)->loc); _LL117:;}} static int Cyc_Parse_is_typeparam( void* tm){
void* _temp127= tm; _LL129: if(( unsigned int) _temp127 >  1u?*(( int*) _temp127)
==  Cyc_Absyn_TypeParams_mod: 0){ goto _LL130;} else{ goto _LL131;} _LL131: goto
_LL132; _LL130: return 1; _LL132: return 0; _LL128:;} static void* Cyc_Parse_id2type(
struct _tagged_arr s, struct Cyc_Absyn_Conref* k){ if( Cyc_Std_zstrcmp( s,
_tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){ return( void*) Cyc_Absyn_HeapRgn;}
else{ return( void*)({ struct Cyc_Absyn_VarType_struct* _temp133=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp133[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp134; _temp134.tag= Cyc_Absyn_VarType; _temp134.f1=({ struct Cyc_Absyn_Tvar*
_temp135=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp135->name=({ struct _tagged_arr* _temp136=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp136[ 0]= s; _temp136;}); _temp135->identity=
0; _temp135->kind= k; _temp135;}); _temp134;}); _temp133;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp137=
t; struct Cyc_Absyn_Tvar* _temp143; _LL139: if(( unsigned int) _temp137 >  3u?*((
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
_LL149: if(( unsigned int) _temp147 >  3u?*(( int*) _temp147) ==  Cyc_Absyn_VarType:
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
return tms;} if( tms ==  0){ return 0;}{ void* _temp172=( void*) tms->hd; void*
_temp178; _LL174: if(( unsigned int) _temp172 >  1u?*(( int*) _temp172) ==  Cyc_Absyn_Function_mod:
0){ _LL179: _temp178=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp172)->f1;
goto _LL175;} else{ goto _LL176;} _LL176: goto _LL177; _LL175: if( tms->tl ==  0?
1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd)?((
struct Cyc_List_List*) _check_null( tms->tl))->tl ==  0: 0)){ void* _temp180=
_temp178; struct Cyc_List_List* _temp186; _LL182: if(*(( int*) _temp180) ==  Cyc_Absyn_WithTypes){
goto _LL183;} else{ goto _LL184;} _LL184: if(*(( int*) _temp180) ==  Cyc_Absyn_NoTypes){
_LL187: _temp186=(( struct Cyc_Absyn_NoTypes_struct*) _temp180)->f1; goto _LL185;}
else{ goto _LL181;} _LL183: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
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
_temp195->hd=( void*)(( void*) tms->hd); _temp195->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp195;});} _LL177: return({ struct Cyc_List_List*
_temp196=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp196->hd=( void*)(( void*) tms->hd); _temp196->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp196;}); _LL173:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds != 
0){ d=({ struct Cyc_Parse_Declarator* _temp197=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp197->id= d->id; _temp197->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp197;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso !=  0){ tss=(( struct
Cyc_Parse_Declaration_spec*) dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline;
atts=(( struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct
Cyc_Parse_Declaration_spec*) dso->v)->sc !=  0){ void* _temp198=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*) dso->v)->sc))->v;
_LL200: if( _temp198 == ( void*) Cyc_Parse_Extern_sc){ goto _LL201;} else{ goto
_LL202;} _LL202: if( _temp198 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL203;}
else{ goto _LL204;} _LL204: if( _temp198 == ( void*) Cyc_Parse_Static_sc){ goto
_LL205;} else{ goto _LL206;} _LL206: goto _LL207; _LL201: sc=( void*) Cyc_Absyn_Extern;
goto _LL199; _LL203: sc=( void*) Cyc_Absyn_ExternC; goto _LL199; _LL205: sc=(
void*) Cyc_Absyn_Static; goto _LL199; _LL207: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL199; _LL199:;}}{ struct Cyc_Core_Opt*
_temp210; void* _temp212; struct _tuple5 _temp208= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL213: _temp212= _temp208.f1; goto _LL211; _LL211: _temp210=
_temp208.f2; goto _LL209; _LL209: { struct Cyc_List_List* _temp216; struct Cyc_List_List*
_temp218; void* _temp220; struct Cyc_Absyn_Tqual _temp222; struct _tuple6
_temp214= Cyc_Parse_apply_tms( tq, _temp212, atts, d->tms); _LL223: _temp222=
_temp214.f1; goto _LL221; _LL221: _temp220= _temp214.f2; goto _LL219; _LL219:
_temp218= _temp214.f3; goto _LL217; _LL217: _temp216= _temp214.f4; goto _LL215;
_LL215: if( _temp210 !=  0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp218 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp224= _temp220; struct Cyc_Absyn_FnInfo _temp230; struct Cyc_List_List*
_temp232; struct Cyc_List_List* _temp234; struct Cyc_Absyn_VarargInfo* _temp236;
int _temp238; struct Cyc_List_List* _temp240; void* _temp242; struct Cyc_Core_Opt*
_temp244; struct Cyc_List_List* _temp246; _LL226: if(( unsigned int) _temp224 > 
3u?*(( int*) _temp224) ==  Cyc_Absyn_FnType: 0){ _LL231: _temp230=(( struct Cyc_Absyn_FnType_struct*)
_temp224)->f1; _LL247: _temp246= _temp230.tvars; goto _LL245; _LL245: _temp244=
_temp230.effect; goto _LL243; _LL243: _temp242=( void*) _temp230.ret_typ; goto
_LL241; _LL241: _temp240= _temp230.args; goto _LL239; _LL239: _temp238= _temp230.c_varargs;
goto _LL237; _LL237: _temp236= _temp230.cyc_varargs; goto _LL235; _LL235:
_temp234= _temp230.rgn_po; goto _LL233; _LL233: _temp232= _temp230.attributes;
goto _LL227;} else{ goto _LL228;} _LL228: goto _LL229; _LL227: { struct Cyc_List_List*
_temp248=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp240); return({ struct Cyc_Absyn_Fndecl*
_temp249=( struct Cyc_Absyn_Fndecl*) _cycalloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp249->sc=( void*) sc; _temp249->name= d->id; _temp249->tvs= _temp246;
_temp249->is_inline= is_inline; _temp249->effect= _temp244; _temp249->ret_type=(
void*) _temp242; _temp249->args= _temp248; _temp249->c_varargs= _temp238;
_temp249->cyc_varargs= _temp236; _temp249->rgn_po= _temp234; _temp249->body=
body; _temp249->cached_typ= 0; _temp249->param_vardecls= 0; _temp249->attributes=
Cyc_List_append( _temp232, _temp216); _temp249;});} _LL229: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL225:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 ==  0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp250=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp250->f1= _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"?", sizeof( unsigned char),
2u); _temp250->f2=(* t).f2; _temp250->f3=(* t).f3; _temp250;});} else{ return({
struct _tuple4* _temp251=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp251->f1=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp251->f2=(* t).f2; _temp251->f3=(* t).f3; _temp251;});}} static
unsigned char _temp252[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp252, _temp252, _temp252 +  52u};
static unsigned char _temp253[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp253, _temp253, _temp253 +  63u};
static unsigned char _temp254[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp254, _temp254, _temp254 +  50u};
static unsigned char _temp255[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp255, _temp255, _temp255 +  60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts !=  0; ts= ts->tl){ void* _temp256=( void*) ts->hd;
struct Cyc_Position_Segment* _temp270; void* _temp272; struct Cyc_Position_Segment*
_temp274; struct Cyc_Position_Segment* _temp276; struct Cyc_Position_Segment*
_temp278; struct Cyc_Position_Segment* _temp280; struct Cyc_Absyn_Decl* _temp282;
_LL258: if(*(( int*) _temp256) ==  Cyc_Parse_Type_spec){ _LL273: _temp272=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp256)->f1; goto _LL271; _LL271: _temp270=((
struct Cyc_Parse_Type_spec_struct*) _temp256)->f2; goto _LL259;} else{ goto
_LL260;} _LL260: if(*(( int*) _temp256) ==  Cyc_Parse_Signed_spec){ _LL275:
_temp274=(( struct Cyc_Parse_Signed_spec_struct*) _temp256)->f1; goto _LL261;}
else{ goto _LL262;} _LL262: if(*(( int*) _temp256) ==  Cyc_Parse_Unsigned_spec){
_LL277: _temp276=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp256)->f1; goto
_LL263;} else{ goto _LL264;} _LL264: if(*(( int*) _temp256) ==  Cyc_Parse_Short_spec){
_LL279: _temp278=(( struct Cyc_Parse_Short_spec_struct*) _temp256)->f1; goto
_LL265;} else{ goto _LL266;} _LL266: if(*(( int*) _temp256) ==  Cyc_Parse_Long_spec){
_LL281: _temp280=(( struct Cyc_Parse_Long_spec_struct*) _temp256)->f1; goto
_LL267;} else{ goto _LL268;} _LL268: if(*(( int*) _temp256) ==  Cyc_Parse_Decl_spec){
_LL283: _temp282=(( struct Cyc_Parse_Decl_spec_struct*) _temp256)->f1; goto
_LL269;} else{ goto _LL257;} _LL259: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp270);} last_loc= _temp270; seen_type= 1; t= _temp272; goto _LL257; _LL261:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp274);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp274);} last_loc= _temp274; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL257; _LL263: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp276);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp276);} last_loc= _temp276; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL257; _LL265: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp278);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp278);} last_loc= _temp278; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL257; _LL267: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp280);} if( seen_size){ void* _temp284= sz; _LL286: if( _temp284 == ( void*)
Cyc_Absyn_B4){ goto _LL287;} else{ goto _LL288;} _LL288: goto _LL289; _LL287: sz=(
void*) Cyc_Absyn_B8; goto _LL285; _LL289: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp280); goto _LL285; _LL285:;} last_loc=
_temp280; seen_size= 1; goto _LL257; _LL269: last_loc= _temp282->loc; if(
declopt ==  0? ! seen_type: 0){ seen_type= 1;{ void* _temp290=( void*) _temp282->r;
struct Cyc_Absyn_Structdecl* _temp302; struct Cyc_Absyn_Tuniondecl* _temp304;
struct Cyc_Absyn_Uniondecl* _temp306; struct Cyc_Absyn_Enumdecl* _temp308;
_LL292: if(*(( int*) _temp290) ==  Cyc_Absyn_Struct_d){ _LL303: _temp302=((
struct Cyc_Absyn_Struct_d_struct*) _temp290)->f1; goto _LL293;} else{ goto
_LL294;} _LL294: if(*(( int*) _temp290) ==  Cyc_Absyn_Tunion_d){ _LL305:
_temp304=(( struct Cyc_Absyn_Tunion_d_struct*) _temp290)->f1; goto _LL295;}
else{ goto _LL296;} _LL296: if(*(( int*) _temp290) ==  Cyc_Absyn_Union_d){
_LL307: _temp306=(( struct Cyc_Absyn_Union_d_struct*) _temp290)->f1; goto _LL297;}
else{ goto _LL298;} _LL298: if(*(( int*) _temp290) ==  Cyc_Absyn_Enum_d){ _LL309:
_temp308=(( struct Cyc_Absyn_Enum_d_struct*) _temp290)->f1; goto _LL299;} else{
goto _LL300;} _LL300: goto _LL301; _LL293: { struct Cyc_List_List* _temp310=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp302->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp311=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp311[ 0]=({ struct Cyc_Absyn_StructType_struct _temp312; _temp312.tag= Cyc_Absyn_StructType;
_temp312.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp302->name))->v); _temp312.f2= _temp310; _temp312.f3= 0;
_temp312;}); _temp311;}); if( _temp302->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp313=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp313->v=( void*) _temp282; _temp313;});} goto _LL291;} _LL295: { struct Cyc_List_List*
_temp314=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp304->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp315=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp315[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp316; _temp316.tag= Cyc_Absyn_TunionType;
_temp316.f1=({ struct Cyc_Absyn_TunionInfo _temp317; _temp317.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp318=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp318[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp319; _temp319.tag= Cyc_Absyn_KnownTunion;
_temp319.f1=({ struct Cyc_Absyn_Tuniondecl** _temp320=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp320[ 0]= _temp304;
_temp320;}); _temp319;}); _temp318;})); _temp317.targs= _temp314; _temp317.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp317;}); _temp316;}); _temp315;}); if(
_temp304->fields !=  0){ declopt=({ struct Cyc_Core_Opt* _temp321=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp321->v=( void*) _temp282;
_temp321;});} goto _LL291;} _LL297: { struct Cyc_List_List* _temp322=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_tvar2typ, _temp306->tvs); t=( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp323=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp323[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp324; _temp324.tag= Cyc_Absyn_UnionType;
_temp324.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp306->name))->v); _temp324.f2= _temp322; _temp324.f3= 0;
_temp324;}); _temp323;}); if( _temp306->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp325=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp325->v=( void*) _temp282; _temp325;});} goto _LL291;} _LL299: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp326=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp326[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp327; _temp327.tag= Cyc_Absyn_EnumType; _temp327.f1=
_temp308->name; _temp327.f2= 0; _temp327;}); _temp326;}); declopt=({ struct Cyc_Core_Opt*
_temp328=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp328->v=( void*) _temp282; _temp328;}); goto _LL291; _LL301:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp282->loc); goto _LL291; _LL291:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp282->loc);} goto _LL257; _LL257:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type within specifier", sizeof(
unsigned char), 30u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp329=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp329[ 0]=({ struct Cyc_Absyn_IntType_struct _temp330; _temp330.tag= Cyc_Absyn_IntType;
_temp330.f1=( void*) sgn; _temp330.f2=( void*) sz; _temp330;}); _temp329;});}
else{ if( seen_sign){ void* _temp331= t; void* _temp337; _LL333: if((
unsigned int) _temp331 >  3u?*(( int*) _temp331) ==  Cyc_Absyn_IntType: 0){
_LL338: _temp337=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp331)->f2;
goto _LL334;} else{ goto _LL335;} _LL335: goto _LL336; _LL334: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp339=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp339[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp340; _temp340.tag= Cyc_Absyn_IntType; _temp340.f1=( void*) sgn; _temp340.f2=(
void*) _temp337; _temp340;}); _temp339;}); goto _LL332; _LL336: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL332; _LL332:;} if( seen_size){ void* _temp341= t; void*
_temp349; _LL343: if(( unsigned int) _temp341 >  3u?*(( int*) _temp341) ==  Cyc_Absyn_IntType:
0){ _LL350: _temp349=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp341)->f1;
goto _LL344;} else{ goto _LL345;} _LL345: if(( unsigned int) _temp341 >  3u?*((
int*) _temp341) ==  Cyc_Absyn_DoubleType: 0){ goto _LL346;} else{ goto _LL347;}
_LL347: goto _LL348; _LL344: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp351=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp351[ 0]=({ struct Cyc_Absyn_IntType_struct _temp352; _temp352.tag= Cyc_Absyn_IntType;
_temp352.f1=( void*) _temp349; _temp352.f2=( void*) sz; _temp352;}); _temp351;});
goto _LL342; _LL346: t=( void*)({ struct Cyc_Absyn_DoubleType_struct* _temp353=(
struct Cyc_Absyn_DoubleType_struct*) _cycalloc_atomic( sizeof( struct Cyc_Absyn_DoubleType_struct));
_temp353[ 0]=({ struct Cyc_Absyn_DoubleType_struct _temp354; _temp354.tag= Cyc_Absyn_DoubleType;
_temp354.f1= 1; _temp354;}); _temp353;}); goto _LL342; _LL348: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL342; _LL342:;}} return({ struct _tuple5 _temp355; _temp355.f1=
t; _temp355.f2= declopt; _temp355;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds ==  0){ return 0;}{ struct Cyc_Parse_Declarator* _temp356=(
struct Cyc_Parse_Declarator*) ds->hd; struct _tuple1* _temp357= _temp356->id;
struct Cyc_List_List* _temp360; struct Cyc_List_List* _temp362; void* _temp364;
struct Cyc_Absyn_Tqual _temp366; struct _tuple6 _temp358= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp356->tms); _LL367: _temp366= _temp358.f1; goto _LL365;
_LL365: _temp364= _temp358.f2; goto _LL363; _LL363: _temp362= _temp358.f3; goto
_LL361; _LL361: _temp360= _temp358.f4; goto _LL359; _LL359: return({ struct Cyc_List_List*
_temp368=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp368->hd=( void*)({ struct _tuple7* _temp369=( struct _tuple7*) _cycalloc(
sizeof( struct _tuple7)); _temp369->f1= _temp357; _temp369->f2= _temp366;
_temp369->f3= _temp364; _temp369->f4= _temp362; _temp369->f5= _temp360; _temp369;});
_temp368->tl= Cyc_Parse_apply_tmss( _temp366, Cyc_Tcutil_copy_type( t), ds->tl,
shared_atts); _temp368;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms ==  0){ return({ struct _tuple6 _temp370; _temp370.f1= tq;
_temp370.f2= t; _temp370.f3= 0; _temp370.f4= atts; _temp370;});}{ void* _temp371=(
void*) tms->hd; struct Cyc_Absyn_Exp* _temp385; void* _temp387; struct Cyc_Position_Segment*
_temp389; struct Cyc_List_List* _temp391; struct Cyc_Absyn_Tqual _temp393; void*
_temp395; void* _temp397; struct Cyc_List_List* _temp399; struct Cyc_Position_Segment*
_temp401; _LL373: if( _temp371 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL374;}
else{ goto _LL375;} _LL375: if(( unsigned int) _temp371 >  1u?*(( int*) _temp371)
==  Cyc_Absyn_ConstArray_mod: 0){ _LL386: _temp385=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp371)->f1; goto _LL376;} else{ goto _LL377;} _LL377: if(( unsigned int)
_temp371 >  1u?*(( int*) _temp371) ==  Cyc_Absyn_Function_mod: 0){ _LL388:
_temp387=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp371)->f1; goto
_LL378;} else{ goto _LL379;} _LL379: if(( unsigned int) _temp371 >  1u?*(( int*)
_temp371) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL392: _temp391=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp371)->f1; goto _LL390; _LL390: _temp389=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp371)->f2; goto _LL380;} else{ goto _LL381;} _LL381: if(( unsigned int)
_temp371 >  1u?*(( int*) _temp371) ==  Cyc_Absyn_Pointer_mod: 0){ _LL398:
_temp397=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp371)->f1; goto
_LL396; _LL396: _temp395=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp371)->f2; goto _LL394; _LL394: _temp393=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp371)->f3; goto _LL382;} else{ goto _LL383;} _LL383: if(( unsigned int)
_temp371 >  1u?*(( int*) _temp371) ==  Cyc_Absyn_Attributes_mod: 0){ _LL402:
_temp401=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp371)->f1; goto _LL400;
_LL400: _temp399=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp371)->f2; goto
_LL384;} else{ goto _LL372;} _LL374: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp403=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp403[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp404; _temp404.tag= Cyc_Absyn_ArrayType; _temp404.f1=(
void*) t; _temp404.f2= tq; _temp404.f3= 0; _temp404;}); _temp403;}), atts, tms->tl);
_LL376: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp405=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp405[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp406; _temp406.tag= Cyc_Absyn_ArrayType; _temp406.f1=(
void*) t; _temp406.f2= tq; _temp406.f3=( struct Cyc_Absyn_Exp*) _temp385;
_temp406;}); _temp405;}), atts, tms->tl); _LL378: { void* _temp407= _temp387;
struct Cyc_List_List* _temp413; struct Cyc_Core_Opt* _temp415; struct Cyc_Absyn_VarargInfo*
_temp417; int _temp419; struct Cyc_List_List* _temp421; struct Cyc_Position_Segment*
_temp423; _LL409: if(*(( int*) _temp407) ==  Cyc_Absyn_WithTypes){ _LL422:
_temp421=(( struct Cyc_Absyn_WithTypes_struct*) _temp407)->f1; goto _LL420;
_LL420: _temp419=(( struct Cyc_Absyn_WithTypes_struct*) _temp407)->f2; goto
_LL418; _LL418: _temp417=(( struct Cyc_Absyn_WithTypes_struct*) _temp407)->f3;
goto _LL416; _LL416: _temp415=(( struct Cyc_Absyn_WithTypes_struct*) _temp407)->f4;
goto _LL414; _LL414: _temp413=(( struct Cyc_Absyn_WithTypes_struct*) _temp407)->f5;
goto _LL410;} else{ goto _LL411;} _LL411: if(*(( int*) _temp407) ==  Cyc_Absyn_NoTypes){
_LL424: _temp423=(( struct Cyc_Absyn_NoTypes_struct*) _temp407)->f2; goto _LL412;}
else{ goto _LL408;} _LL410: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as !=  0; as= as->tl){ if( Cyc_Absyn_fntype_att(( void*) as->hd)){
fn_atts=({ struct Cyc_List_List* _temp425=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp425->hd=( void*)(( void*) as->hd); _temp425->tl=
fn_atts; _temp425;});} else{ new_atts=({ struct Cyc_List_List* _temp426=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp426->hd=( void*)((
void*) as->hd); _temp426->tl= new_atts; _temp426;});}}} if( tms->tl !=  0){ void*
_temp427=( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd; struct
Cyc_List_List* _temp433; _LL429: if(( unsigned int) _temp427 >  1u?*(( int*)
_temp427) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL434: _temp433=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp427)->f1; goto _LL430;} else{ goto _LL431;} _LL431: goto _LL432; _LL430:
typvars= _temp433; tms= tms->tl; goto _LL428; _LL432: goto _LL428; _LL428:;} if(((((
! _temp419? _temp417 ==  0: 0)? _temp421 !=  0: 0)? _temp421->tl ==  0: 0)?(*((
struct _tuple2*) _temp421->hd)).f1 ==  0: 0)?(*(( struct _tuple2*) _temp421->hd)).f3
== ( void*) Cyc_Absyn_VoidType: 0){ _temp421= 0;} t= Cyc_Parse_array2ptr( t);((
void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Parse_arg_array2ptr, _temp421); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),
Cyc_Absyn_function_typ( typvars, _temp415, t, _temp421, _temp419, _temp417,
_temp413, fn_atts), new_atts,(( struct Cyc_List_List*) _check_null( tms))->tl);}
_LL412:( int) _throw( Cyc_Parse_abort( _tag_arr("function declaration without parameter types",
sizeof( unsigned char), 45u), _temp423)); _LL408:;} _LL380: if( tms->tl ==  0){
return({ struct _tuple6 _temp435; _temp435.f1= tq; _temp435.f2= t; _temp435.f3=
_temp391; _temp435.f4= atts; _temp435;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp389)); _LL382: { void* _temp436= _temp397;
struct Cyc_Absyn_Exp* _temp444; struct Cyc_Absyn_Exp* _temp446; _LL438: if((
unsigned int) _temp436 >  1u?*(( int*) _temp436) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL445: _temp444=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp436)->f1; goto
_LL439;} else{ goto _LL440;} _LL440: if(( unsigned int) _temp436 >  1u?*(( int*)
_temp436) ==  Cyc_Absyn_Nullable_ps: 0){ _LL447: _temp446=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp436)->f1; goto _LL441;} else{ goto _LL442;} _LL442: if( _temp436 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL443;} else{ goto _LL437;} _LL439: return Cyc_Parse_apply_tms(
_temp393, Cyc_Absyn_atb_typ( t, _temp395, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp448=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp448[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp449; _temp449.tag= Cyc_Absyn_Upper_b;
_temp449.f1= _temp444; _temp449;}); _temp448;})), atts, tms->tl); _LL441: return
Cyc_Parse_apply_tms( _temp393, Cyc_Absyn_starb_typ( t, _temp395, tq,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp450=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp450[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp451; _temp451.tag= Cyc_Absyn_Upper_b; _temp451.f1= _temp446; _temp451;});
_temp450;})), atts, tms->tl); _LL443: return Cyc_Parse_apply_tms( _temp393, Cyc_Absyn_tagged_typ(
t, _temp395, tq), atts, tms->tl); _LL437:;} _LL384: return Cyc_Parse_apply_tms(
tq, t, Cyc_List_append( atts, _temp399), tms->tl); _LL372:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp454; void* _temp456; struct _tuple5 _temp452= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL457: _temp456= _temp452.f1; goto _LL455; _LL455: _temp454=
_temp452.f2; goto _LL453; _LL453: if( _temp454 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp456;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp458=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp458[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp459; _temp459.tag= Cyc_Absyn_Decl_s;
_temp459.f1= d; _temp459.f2= s; _temp459;}); _temp458;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc !=  0){ void* _temp460=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL462: if( _temp460 == (
void*) Cyc_Parse_Typedef_sc){ goto _LL463;} else{ goto _LL464;} _LL464: if(
_temp460 == ( void*) Cyc_Parse_Extern_sc){ goto _LL465;} else{ goto _LL466;}
_LL466: if( _temp460 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL467;} else{ goto
_LL468;} _LL468: if( _temp460 == ( void*) Cyc_Parse_Static_sc){ goto _LL469;}
else{ goto _LL470;} _LL470: if( _temp460 == ( void*) Cyc_Parse_Auto_sc){ goto
_LL471;} else{ goto _LL472;} _LL472: if( _temp460 == ( void*) Cyc_Parse_Register_sc){
goto _LL473;} else{ goto _LL474;} _LL474: if( _temp460 == ( void*) Cyc_Parse_Abstract_sc){
goto _LL475;} else{ goto _LL461;} _LL463: istypedef= 1; goto _LL461; _LL465: s=(
void*) Cyc_Absyn_Extern; goto _LL461; _LL467: s=( void*) Cyc_Absyn_ExternC; goto
_LL461; _LL469: s=( void*) Cyc_Absyn_Static; goto _LL461; _LL471: s=( void*) Cyc_Absyn_Public;
goto _LL461; _LL473: s=( void*) Cyc_Absyn_Public; goto _LL461; _LL475: s=( void*)
Cyc_Absyn_Abstract; goto _LL461; _LL461:;}{ struct Cyc_List_List* _temp478;
struct Cyc_List_List* _temp480; struct _tuple0 _temp476=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL481: _temp480= _temp476.f1;
goto _LL479; _LL479: _temp478= _temp476.f2; goto _LL477; _LL477: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp478; for( 0; es !=  0; es= es->tl){
if(( struct Cyc_Absyn_Exp*) es->hd !=  0){ exps_empty= 0; break;}}}{ struct
_tuple5 _temp482= Cyc_Parse_collapse_type_specifiers( tss, loc); if( _temp480 == 
0){ struct Cyc_Core_Opt* _temp485; void* _temp487; struct _tuple5 _temp483=
_temp482; _LL488: _temp487= _temp483.f1; goto _LL486; _LL486: _temp485= _temp483.f2;
goto _LL484; _LL484: if( _temp485 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
_temp485->v;{ void* _temp489=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp501;
struct Cyc_Absyn_Structdecl* _temp503; struct Cyc_Absyn_Uniondecl* _temp505;
struct Cyc_Absyn_Tuniondecl* _temp507; _LL491: if(*(( int*) _temp489) ==  Cyc_Absyn_Enum_d){
_LL502: _temp501=(( struct Cyc_Absyn_Enum_d_struct*) _temp489)->f1; goto _LL492;}
else{ goto _LL493;} _LL493: if(*(( int*) _temp489) ==  Cyc_Absyn_Struct_d){
_LL504: _temp503=(( struct Cyc_Absyn_Struct_d_struct*) _temp489)->f1; goto
_LL494;} else{ goto _LL495;} _LL495: if(*(( int*) _temp489) ==  Cyc_Absyn_Union_d){
_LL506: _temp505=(( struct Cyc_Absyn_Union_d_struct*) _temp489)->f1; goto _LL496;}
else{ goto _LL497;} _LL497: if(*(( int*) _temp489) ==  Cyc_Absyn_Tunion_d){
_LL508: _temp507=(( struct Cyc_Absyn_Tunion_d_struct*) _temp489)->f1; goto
_LL498;} else{ goto _LL499;} _LL499: goto _LL500; _LL492:( void*)( _temp501->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum",
sizeof( unsigned char), 23u), loc);} goto _LL490; _LL494:( void*)( _temp503->sc=(
void*) s); _temp503->attributes= atts; goto _LL490; _LL496:( void*)( _temp505->sc=(
void*) s); _temp505->attributes= atts; goto _LL490; _LL498:( void*)( _temp507->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL490; _LL500: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL490:;}
return({ struct Cyc_List_List* _temp509=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp509->hd=( void*) d; _temp509->tl= 0;
_temp509;});} else{ void* _temp510= _temp487; struct Cyc_List_List* _temp524;
struct _tuple1* _temp526; struct Cyc_Absyn_TunionInfo _temp528; void* _temp530;
struct Cyc_Absyn_Tuniondecl** _temp532; struct Cyc_Absyn_TunionInfo _temp534;
struct Cyc_List_List* _temp536; void* _temp538; struct Cyc_Absyn_UnknownTunionInfo
_temp540; int _temp542; struct _tuple1* _temp544; struct Cyc_List_List* _temp546;
struct _tuple1* _temp548; struct _tuple1* _temp550; _LL512: if(( unsigned int)
_temp510 >  3u?*(( int*) _temp510) ==  Cyc_Absyn_StructType: 0){ _LL527:
_temp526=(( struct Cyc_Absyn_StructType_struct*) _temp510)->f1; goto _LL525;
_LL525: _temp524=(( struct Cyc_Absyn_StructType_struct*) _temp510)->f2; goto
_LL513;} else{ goto _LL514;} _LL514: if(( unsigned int) _temp510 >  3u?*(( int*)
_temp510) ==  Cyc_Absyn_TunionType: 0){ _LL529: _temp528=(( struct Cyc_Absyn_TunionType_struct*)
_temp510)->f1; _LL531: _temp530=( void*) _temp528.tunion_info; if(*(( int*)
_temp530) ==  Cyc_Absyn_KnownTunion){ _LL533: _temp532=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp530)->f1; goto _LL515;} else{ goto _LL516;}} else{ goto _LL516;} _LL516:
if(( unsigned int) _temp510 >  3u?*(( int*) _temp510) ==  Cyc_Absyn_TunionType:
0){ _LL535: _temp534=(( struct Cyc_Absyn_TunionType_struct*) _temp510)->f1;
_LL539: _temp538=( void*) _temp534.tunion_info; if(*(( int*) _temp538) ==  Cyc_Absyn_UnknownTunion){
_LL541: _temp540=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp538)->f1;
_LL545: _temp544= _temp540.name; goto _LL543; _LL543: _temp542= _temp540.is_xtunion;
goto _LL537;} else{ goto _LL518;} _LL537: _temp536= _temp534.targs; goto _LL517;}
else{ goto _LL518;} _LL518: if(( unsigned int) _temp510 >  3u?*(( int*) _temp510)
==  Cyc_Absyn_UnionType: 0){ _LL549: _temp548=(( struct Cyc_Absyn_UnionType_struct*)
_temp510)->f1; goto _LL547; _LL547: _temp546=(( struct Cyc_Absyn_UnionType_struct*)
_temp510)->f2; goto _LL519;} else{ goto _LL520;} _LL520: if(( unsigned int)
_temp510 >  3u?*(( int*) _temp510) ==  Cyc_Absyn_EnumType: 0){ _LL551: _temp550=((
struct Cyc_Absyn_EnumType_struct*) _temp510)->f1; goto _LL521;} else{ goto
_LL522;} _LL522: goto _LL523; _LL513: { struct Cyc_List_List* _temp552=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp524); struct Cyc_Absyn_Structdecl* _temp553=({
struct Cyc_Absyn_Structdecl* _temp557=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp557->sc=( void*) s; _temp557->name=({
struct Cyc_Core_Opt* _temp558=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp558->v=( void*)(( struct _tuple1*) _check_null( _temp526));
_temp558;}); _temp557->tvs= _temp552; _temp557->fields= 0; _temp557->attributes=
0; _temp557;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp554=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp554->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp555=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp555[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp556; _temp556.tag= Cyc_Absyn_Struct_d;
_temp556.f1= _temp553; _temp556;}); _temp555;}), loc); _temp554->tl= 0; _temp554;});}
_LL515: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp559=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp559->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp560=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp560[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp561; _temp561.tag= Cyc_Absyn_Tunion_d;
_temp561.f1=* _temp532; _temp561;}); _temp560;}), loc); _temp559->tl= 0;
_temp559;}); _LL517: { struct Cyc_List_List* _temp562=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp536); struct Cyc_Absyn_Decl* _temp563= Cyc_Absyn_tunion_decl( s, _temp544,
_temp562, 0, _temp542, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp564=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp564->hd=(
void*) _temp563; _temp564->tl= 0; _temp564;});} _LL519: { struct Cyc_List_List*
_temp565=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp546); struct Cyc_Absyn_Uniondecl* _temp566=({
struct Cyc_Absyn_Uniondecl* _temp571=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp571->sc=( void*) s; _temp571->name=({
struct Cyc_Core_Opt* _temp572=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp572->v=( void*)(( struct _tuple1*) _check_null( _temp548));
_temp572;}); _temp571->tvs= _temp565; _temp571->fields= 0; _temp571->attributes=
0; _temp571;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp567=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp567->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp568=( struct Cyc_Absyn_Decl*) _cycalloc(
sizeof( struct Cyc_Absyn_Decl)); _temp568->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp569=( struct Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp569[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp570; _temp570.tag= Cyc_Absyn_Union_d;
_temp570.f1= _temp566; _temp570;}); _temp569;})); _temp568->loc= loc; _temp568;});
_temp567->tl= 0; _temp567;});} _LL521: { struct Cyc_Absyn_Enumdecl* _temp573=({
struct Cyc_Absyn_Enumdecl* _temp578=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp578->sc=( void*) s; _temp578->name=
_temp550; _temp578->fields= 0; _temp578;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp574=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp574->hd=( void*)({ struct Cyc_Absyn_Decl* _temp575=(
struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp575->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp576=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp576[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp577; _temp577.tag= Cyc_Absyn_Enum_d; _temp577.f1= _temp573; _temp577;});
_temp576;})); _temp575->loc= loc; _temp575;}); _temp574->tl= 0; _temp574;});}
_LL523: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL511:;}} else{ void* t= _temp482.f1; struct Cyc_List_List*
_temp579= Cyc_Parse_apply_tmss( tq, t, _temp480, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp579); if( _temp482.f2 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(
_temp482.f2)->v;{ void* _temp580=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp592; struct Cyc_Absyn_Tuniondecl* _temp594; struct Cyc_Absyn_Uniondecl*
_temp596; struct Cyc_Absyn_Enumdecl* _temp598; _LL582: if(*(( int*) _temp580) == 
Cyc_Absyn_Struct_d){ _LL593: _temp592=(( struct Cyc_Absyn_Struct_d_struct*)
_temp580)->f1; goto _LL583;} else{ goto _LL584;} _LL584: if(*(( int*) _temp580)
==  Cyc_Absyn_Tunion_d){ _LL595: _temp594=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp580)->f1; goto _LL585;} else{ goto _LL586;} _LL586: if(*(( int*) _temp580)
==  Cyc_Absyn_Union_d){ _LL597: _temp596=(( struct Cyc_Absyn_Union_d_struct*)
_temp580)->f1; goto _LL587;} else{ goto _LL588;} _LL588: if(*(( int*) _temp580)
==  Cyc_Absyn_Enum_d){ _LL599: _temp598=(( struct Cyc_Absyn_Enum_d_struct*)
_temp580)->f1; goto _LL589;} else{ goto _LL590;} _LL590: goto _LL591; _LL583:(
void*)( _temp592->sc=( void*) s); _temp592->attributes= atts; atts= 0; goto
_LL581; _LL585:( void*)( _temp594->sc=( void*) s); goto _LL581; _LL587:( void*)(
_temp596->sc=( void*) s); goto _LL581; _LL589:( void*)( _temp598->sc=( void*) s);
goto _LL581; _LL591: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL581; _LL581:;} decls=({ struct Cyc_List_List*
_temp600=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp600->hd=( void*) d; _temp600->tl= decls; _temp600;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp602; _temp602.tag=
Cyc_Std_String_pa; _temp602.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) atts->hd);{ void* _temp601[ 1u]={& _temp602}; Cyc_Std_aprintf( _tag_arr("bad attribute %s in typedef",
sizeof( unsigned char), 28u), _tag_arr( _temp601, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp482.f2 !=  0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp603= _temp579; for( 0; _temp603 !=  0;( _temp603= _temp603->tl,
_temp478= _temp478->tl)){ struct _tuple7 _temp606; struct Cyc_List_List*
_temp607; struct Cyc_List_List* _temp609; void* _temp611; struct Cyc_Absyn_Tqual
_temp613; struct _tuple1* _temp615; struct _tuple7* _temp604=( struct _tuple7*)
_temp603->hd; _temp606=* _temp604; _LL616: _temp615= _temp606.f1; goto _LL614;
_LL614: _temp613= _temp606.f2; goto _LL612; _LL612: _temp611= _temp606.f3; goto
_LL610; _LL610: _temp609= _temp606.f4; goto _LL608; _LL608: _temp607= _temp606.f5;
goto _LL605; _LL605: if( _temp609 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), loc);} if( _temp478 ==  0){(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("unexpected NULL in parse!",
sizeof( unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp617=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp478))->hd; struct Cyc_Absyn_Vardecl*
_temp618= Cyc_Absyn_new_vardecl( _temp615, _temp611, _temp617); _temp618->tq=
_temp613;( void*)( _temp618->sc=( void*) s); _temp618->attributes= _temp607;{
struct Cyc_Absyn_Decl* _temp619=({ struct Cyc_Absyn_Decl* _temp621=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp621->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp622=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp622[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp623; _temp623.tag= Cyc_Absyn_Var_d; _temp623.f1= _temp618; _temp623;});
_temp622;})); _temp621->loc= loc; _temp621;}); decls=({ struct Cyc_List_List*
_temp620=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp620->hd=( void*) _temp619; _temp620->tl= decls; _temp620;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_Std_strlen( s) !=  1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp625; _temp625.tag= Cyc_Std_String_pa;
_temp625.f1=( struct _tagged_arr) s;{ void* _temp624[ 1u]={& _temp625}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp624,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL626: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL627: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL628: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL629: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL630: return(
void*) Cyc_Absyn_EffKind; default: _LL631: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp634; _temp634.tag= Cyc_Std_String_pa;
_temp634.f1=( struct _tagged_arr) s;{ void* _temp633[ 1u]={& _temp634}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp633,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts ==  0){ return
tms;} else{ return({ struct Cyc_List_List* _temp635=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp635->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp636=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp636[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp637; _temp637.tag= Cyc_Absyn_Attributes_mod;
_temp637.f1= loc; _temp637.f2= atts; _temp637;}); _temp636;})); _temp635->tl=
tms; _temp635;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp640;
struct Cyc_List_List* _temp641; struct Cyc_List_List* _temp643; void* _temp645;
struct Cyc_Absyn_Tqual _temp647; struct _tuple1* _temp649; struct _tuple7*
_temp638= t; _temp640=* _temp638; _LL650: _temp649= _temp640.f1; goto _LL648;
_LL648: _temp647= _temp640.f2; goto _LL646; _LL646: _temp645= _temp640.f3; goto
_LL644; _LL644: _temp643= _temp640.f4; goto _LL642; _LL642: _temp641= _temp640.f5;
goto _LL639; _LL639: Cyc_Lex_register_typedef( _temp649); if( _temp641 !=  0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp652;
_temp652.tag= Cyc_Std_String_pa; _temp652.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) _temp641->hd);{ void* _temp651[ 1u]={& _temp652}; Cyc_Std_aprintf(
_tag_arr("bad attribute %s within typedef", sizeof( unsigned char), 32u),
_tag_arr( _temp651, sizeof( void*), 1u));}}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp653=( struct Cyc_Absyn_Typedef_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp653[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp654; _temp654.tag= Cyc_Absyn_Typedef_d; _temp654.f1=({
struct Cyc_Absyn_Typedefdecl* _temp655=( struct Cyc_Absyn_Typedefdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp655->name= _temp649;
_temp655->tvs= _temp643; _temp655->defn=( void*) _temp645; _temp655;}); _temp654;});
_temp653;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ;
unsigned char Cyc_String_tok[ 15u]="\000\000\000\000String_tok"; struct Cyc_String_tok_struct{
unsigned char* tag; struct _tagged_arr f1; } ; unsigned char Cyc_Stringopt_tok[
18u]="\000\000\000\000Stringopt_tok"; struct Cyc_Stringopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; unsigned char Cyc_StructFieldDeclListList_tok[
32u]="\000\000\000\000StructFieldDeclListList_tok"; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_StructFieldDeclList_tok[
28u]="\000\000\000\000StructFieldDeclList_tok"; struct Cyc_StructFieldDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_StructOrUnion_tok[
22u]="\000\000\000\000StructOrUnion_tok"; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; enum  Cyc_Parse_Struct_or_union f1; } ; unsigned char Cyc_SwitchCClauseList_tok[
26u]="\000\000\000\000SwitchCClauseList_tok"; struct Cyc_SwitchCClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_SwitchClauseList_tok[
25u]="\000\000\000\000SwitchClauseList_tok"; struct Cyc_SwitchClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_TunionFieldList_tok[
24u]="\000\000\000\000TunionFieldList_tok"; struct Cyc_TunionFieldList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_TunionField_tok[
20u]="\000\000\000\000TunionField_tok"; struct Cyc_TunionField_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; unsigned char Cyc_TypeList_tok[
17u]="\000\000\000\000TypeList_tok"; struct Cyc_TypeList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_TypeModifierList_tok[
25u]="\000\000\000\000TypeModifierList_tok"; struct Cyc_TypeModifierList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_TypeOpt_tok[
16u]="\000\000\000\000TypeOpt_tok"; struct Cyc_TypeOpt_tok_struct{ unsigned char*
tag; struct Cyc_Core_Opt* f1; } ; unsigned char Cyc_TypeQual_tok[ 17u]="\000\000\000\000TypeQual_tok";
struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual f1; }
; unsigned char Cyc_TypeSpecifier_tok[ 22u]="\000\000\000\000TypeSpecifier_tok";
struct Cyc_TypeSpecifier_tok_struct{ unsigned char* tag; void* f1; } ;
unsigned char Cyc_Type_tok[ 13u]="\000\000\000\000Type_tok"; struct Cyc_Type_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_YY1[ 8u]="\000\000\000\000YY1";
struct Cyc_YY1_struct{ unsigned char* tag; struct _tuple16* f1; } ;
unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL";
unsigned char Cyc_Okay_tok[ 13u]="\000\000\000\000Okay_tok"; static
unsigned char _temp657[ 15u]="$(sign_t,int)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp657, _temp657, _temp657 +  15u}};
struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ struct _tuple15* yyzzz;{ void*
_temp658= yy1; struct _tuple15* _temp664; _LL660: if(*(( void**) _temp658) == 
Cyc_Int_tok){ _LL665: _temp664=(( struct Cyc_Int_tok_struct*) _temp658)->f1;
goto _LL661;} else{ goto _LL662;} _LL662: goto _LL663; _LL661: yyzzz= _temp664;
goto _LL659; _LL663:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL659:;}
return yyzzz;} static unsigned char _temp667[ 9u]="string_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp667, _temp667, _temp667 +  9u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ struct _tagged_arr yyzzz;{
void* _temp668= yy1; struct _tagged_arr _temp674; _LL670: if(*(( void**)
_temp668) ==  Cyc_String_tok){ _LL675: _temp674=(( struct Cyc_String_tok_struct*)
_temp668)->f1; goto _LL671;} else{ goto _LL672;} _LL672: goto _LL673; _LL671:
yyzzz= _temp674; goto _LL669; _LL673:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL669:;} return yyzzz;} static unsigned char _temp677[ 5u]="char"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{ _temp677,
_temp677, _temp677 +  5u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
unsigned char yyzzz;{ void* _temp678= yy1; unsigned char _temp684; _LL680: if(*((
void**) _temp678) ==  Cyc_Char_tok){ _LL685: _temp684=(( struct Cyc_Char_tok_struct*)
_temp678)->f1; goto _LL681;} else{ goto _LL682;} _LL682: goto _LL683; _LL681:
yyzzz= _temp684; goto _LL679; _LL683:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL679:;} return yyzzz;} static unsigned char _temp687[ 20u]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{
_temp687, _temp687, _temp687 +  20u}}; void* Cyc_yyget_Pointer_Sort_tok( void*
yy1){ void* yyzzz;{ void* _temp688= yy1; void* _temp694; _LL690: if(*(( void**)
_temp688) ==  Cyc_Pointer_Sort_tok){ _LL695: _temp694=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp688)->f1; goto _LL691;} else{ goto _LL692;} _LL692: goto _LL693; _LL691:
yyzzz= _temp694; goto _LL689; _LL693:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL689:;} return yyzzz;} static unsigned char _temp697[ 6u]="exp_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp697,
_temp697, _temp697 +  6u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp698= yy1; struct Cyc_Absyn_Exp*
_temp704; _LL700: if(*(( void**) _temp698) ==  Cyc_Exp_tok){ _LL705: _temp704=((
struct Cyc_Exp_tok_struct*) _temp698)->f1; goto _LL701;} else{ goto _LL702;}
_LL702: goto _LL703; _LL701: yyzzz= _temp704; goto _LL699; _LL703:( int) _throw((
void*)& Cyc_yyfail_Exp_tok); _LL699:;} return yyzzz;} static unsigned char
_temp707[ 14u]="list_t<exp_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{ _temp707, _temp707, _temp707 +  14u}}; struct Cyc_List_List*
Cyc_yyget_ExpList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp708=
yy1; struct Cyc_List_List* _temp714; _LL710: if(*(( void**) _temp708) ==  Cyc_ExpList_tok){
_LL715: _temp714=(( struct Cyc_ExpList_tok_struct*) _temp708)->f1; goto _LL711;}
else{ goto _LL712;} _LL712: goto _LL713; _LL711: yyzzz= _temp714; goto _LL709;
_LL713:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL709:;} return yyzzz;}
static unsigned char _temp717[ 39u]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{
_temp717, _temp717, _temp717 +  39u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp718= yy1; struct Cyc_List_List*
_temp724; _LL720: if(*(( void**) _temp718) ==  Cyc_InitializerList_tok){ _LL725:
_temp724=(( struct Cyc_InitializerList_tok_struct*) _temp718)->f1; goto _LL721;}
else{ goto _LL722;} _LL722: goto _LL723; _LL721: yyzzz= _temp724; goto _LL719;
_LL723:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL719:;} return
yyzzz;} static unsigned char _temp727[ 9u]="primop_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp727, _temp727, _temp727 +  9u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* yyzzz;{ void* _temp728= yy1; void*
_temp734; _LL730: if(*(( void**) _temp728) ==  Cyc_Primop_tok){ _LL735: _temp734=(
void*)(( struct Cyc_Primop_tok_struct*) _temp728)->f1; goto _LL731;} else{ goto
_LL732;} _LL732: goto _LL733; _LL731: yyzzz= _temp734; goto _LL729; _LL733:( int)
_throw(( void*)& Cyc_yyfail_Primop_tok); _LL729:;} return yyzzz;} static
unsigned char _temp737[ 16u]="opt_t<primop_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp737, _temp737, _temp737 +  16u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ struct Cyc_Core_Opt*
yyzzz;{ void* _temp738= yy1; struct Cyc_Core_Opt* _temp744; _LL740: if(*(( void**)
_temp738) ==  Cyc_Primopopt_tok){ _LL745: _temp744=(( struct Cyc_Primopopt_tok_struct*)
_temp738)->f1; goto _LL741;} else{ goto _LL742;} _LL742: goto _LL743; _LL741:
yyzzz= _temp744; goto _LL739; _LL743:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL739:;} return yyzzz;} static unsigned char _temp747[ 7u]="qvar_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{
_temp747, _temp747, _temp747 +  7u}}; struct _tuple1* Cyc_yyget_QualId_tok( void*
yy1){ struct _tuple1* yyzzz;{ void* _temp748= yy1; struct _tuple1* _temp754;
_LL750: if(*(( void**) _temp748) ==  Cyc_QualId_tok){ _LL755: _temp754=(( struct
Cyc_QualId_tok_struct*) _temp748)->f1; goto _LL751;} else{ goto _LL752;} _LL752:
goto _LL753; _LL751: yyzzz= _temp754; goto _LL749; _LL753:( int) _throw(( void*)&
Cyc_yyfail_QualId_tok); _LL749:;} return yyzzz;} static unsigned char _temp757[
7u]="stmt_t"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{
_temp757, _temp757, _temp757 +  7u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok(
void* yy1){ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp758= yy1; struct Cyc_Absyn_Stmt*
_temp764; _LL760: if(*(( void**) _temp758) ==  Cyc_Stmt_tok){ _LL765: _temp764=((
struct Cyc_Stmt_tok_struct*) _temp758)->f1; goto _LL761;} else{ goto _LL762;}
_LL762: goto _LL763; _LL761: yyzzz= _temp764; goto _LL759; _LL763:( int) _throw((
void*)& Cyc_yyfail_Stmt_tok); _LL759:;} return yyzzz;} static unsigned char
_temp767[ 24u]="list_t<switch_clause_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp767, _temp767,
_temp767 +  24u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ struct Cyc_List_List* yyzzz;{ void* _temp768= yy1; struct Cyc_List_List*
_temp774; _LL770: if(*(( void**) _temp768) ==  Cyc_SwitchClauseList_tok){ _LL775:
_temp774=(( struct Cyc_SwitchClauseList_tok_struct*) _temp768)->f1; goto _LL771;}
else{ goto _LL772;} _LL772: goto _LL773; _LL771: yyzzz= _temp774; goto _LL769;
_LL773:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL769:;}
return yyzzz;} static unsigned char _temp777[ 25u]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp777, _temp777, _temp777 +  25u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp778= yy1; struct Cyc_List_List*
_temp784; _LL780: if(*(( void**) _temp778) ==  Cyc_SwitchCClauseList_tok){
_LL785: _temp784=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp778)->f1;
goto _LL781;} else{ goto _LL782;} _LL782: goto _LL783; _LL781: yyzzz= _temp784;
goto _LL779; _LL783:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL779:;} return yyzzz;} static unsigned char _temp787[ 6u]="pat_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{
_temp787, _temp787, _temp787 +  6u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
void* yy1){ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp788= yy1; struct Cyc_Absyn_Pat*
_temp794; _LL790: if(*(( void**) _temp788) ==  Cyc_Pattern_tok){ _LL795:
_temp794=(( struct Cyc_Pattern_tok_struct*) _temp788)->f1; goto _LL791;} else{
goto _LL792;} _LL792: goto _LL793; _LL791: yyzzz= _temp794; goto _LL789; _LL793:(
int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL789:;} return yyzzz;} static
unsigned char _temp797[ 14u]="list_t<pat_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp797, _temp797, _temp797 + 
14u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp798= yy1; struct Cyc_List_List* _temp804; _LL800: if(*(( void**)
_temp798) ==  Cyc_PatternList_tok){ _LL805: _temp804=(( struct Cyc_PatternList_tok_struct*)
_temp798)->f1; goto _LL801;} else{ goto _LL802;} _LL802: goto _LL803; _LL801:
yyzzz= _temp804; goto _LL799; _LL803:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL799:;} return yyzzz;} static unsigned char _temp807[ 31u]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp807, _temp807, _temp807 +  31u}}; struct _tuple13* Cyc_yyget_FieldPattern_tok(
void* yy1){ struct _tuple13* yyzzz;{ void* _temp808= yy1; struct _tuple13*
_temp814; _LL810: if(*(( void**) _temp808) ==  Cyc_FieldPattern_tok){ _LL815:
_temp814=(( struct Cyc_FieldPattern_tok_struct*) _temp808)->f1; goto _LL811;}
else{ goto _LL812;} _LL812: goto _LL813; _LL811: yyzzz= _temp814; goto _LL809;
_LL813:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL809:;} return
yyzzz;} static unsigned char _temp817[ 39u]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp817, _temp817, _temp817 +  39u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp818= yy1; struct Cyc_List_List*
_temp824; _LL820: if(*(( void**) _temp818) ==  Cyc_FieldPatternList_tok){ _LL825:
_temp824=(( struct Cyc_FieldPatternList_tok_struct*) _temp818)->f1; goto _LL821;}
else{ goto _LL822;} _LL822: goto _LL823; _LL821: yyzzz= _temp824; goto _LL819;
_LL823:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL819:;}
return yyzzz;} static unsigned char _temp827[ 9u]="fndecl_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp827, _temp827, _temp827 +  9u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ struct Cyc_Absyn_Fndecl*
yyzzz;{ void* _temp828= yy1; struct Cyc_Absyn_Fndecl* _temp834; _LL830: if(*((
void**) _temp828) ==  Cyc_FnDecl_tok){ _LL835: _temp834=(( struct Cyc_FnDecl_tok_struct*)
_temp828)->f1; goto _LL831;} else{ goto _LL832;} _LL832: goto _LL833; _LL831:
yyzzz= _temp834; goto _LL829; _LL833:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL829:;} return yyzzz;} static unsigned char _temp837[ 15u]="list_t<decl_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp837, _temp837, _temp837 +  15u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp838= yy1; struct Cyc_List_List*
_temp844; _LL840: if(*(( void**) _temp838) ==  Cyc_DeclList_tok){ _LL845:
_temp844=(( struct Cyc_DeclList_tok_struct*) _temp838)->f1; goto _LL841;} else{
goto _LL842;} _LL842: goto _LL843; _LL841: yyzzz= _temp844; goto _LL839; _LL843:(
int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL839:;} return yyzzz;} static
unsigned char _temp847[ 12u]="decl_spec_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp847, _temp847, _temp847 +  12u}};
struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){ struct
Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp848= yy1; struct Cyc_Parse_Declaration_spec*
_temp854; _LL850: if(*(( void**) _temp848) ==  Cyc_DeclSpec_tok){ _LL855:
_temp854=(( struct Cyc_DeclSpec_tok_struct*) _temp848)->f1; goto _LL851;} else{
goto _LL852;} _LL852: goto _LL853; _LL851: yyzzz= _temp854; goto _LL849; _LL853:(
int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL849:;} return yyzzz;} static
unsigned char _temp857[ 27u]="$(declarator_t,exp_opt_t)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp857, _temp857, _temp857 +  27u}};
struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){ struct _tuple14* yyzzz;{
void* _temp858= yy1; struct _tuple14* _temp864; _LL860: if(*(( void**) _temp858)
==  Cyc_InitDecl_tok){ _LL865: _temp864=(( struct Cyc_InitDecl_tok_struct*)
_temp858)->f1; goto _LL861;} else{ goto _LL862;} _LL862: goto _LL863; _LL861:
yyzzz= _temp864; goto _LL859; _LL863:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL859:;} return yyzzz;} static unsigned char _temp867[ 35u]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp867, _temp867, _temp867 +  35u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp868= yy1; struct Cyc_List_List*
_temp874; _LL870: if(*(( void**) _temp868) ==  Cyc_InitDeclList_tok){ _LL875:
_temp874=(( struct Cyc_InitDeclList_tok_struct*) _temp868)->f1; goto _LL871;}
else{ goto _LL872;} _LL872: goto _LL873; _LL871: yyzzz= _temp874; goto _LL869;
_LL873:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL869:;} return
yyzzz;} static unsigned char _temp877[ 16u]="storage_class_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp877, _temp877, _temp877 + 
16u}}; void* Cyc_yyget_StorageClass_tok( void* yy1){ void* yyzzz;{ void*
_temp878= yy1; void* _temp884; _LL880: if(*(( void**) _temp878) ==  Cyc_StorageClass_tok){
_LL885: _temp884=( void*)(( struct Cyc_StorageClass_tok_struct*) _temp878)->f1;
goto _LL881;} else{ goto _LL882;} _LL882: goto _LL883; _LL881: yyzzz= _temp884;
goto _LL879; _LL883:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL879:;}
return yyzzz;} static unsigned char _temp887[ 17u]="type_specifier_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp887, _temp887, _temp887 +  17u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* yyzzz;{ void* _temp888= yy1; void* _temp894; _LL890: if(*(( void**)
_temp888) ==  Cyc_TypeSpecifier_tok){ _LL895: _temp894=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp888)->f1; goto _LL891;} else{ goto _LL892;} _LL892: goto _LL893; _LL891:
yyzzz= _temp894; goto _LL889; _LL893:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL889:;} return yyzzz;} static unsigned char _temp897[ 18u]="struct_or_union_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp897, _temp897, _temp897 +  18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ enum  Cyc_Parse_Struct_or_union yyzzz;{ void* _temp898= yy1; enum 
Cyc_Parse_Struct_or_union _temp904; _LL900: if(*(( void**) _temp898) ==  Cyc_StructOrUnion_tok){
_LL905: _temp904=(( struct Cyc_StructOrUnion_tok_struct*) _temp898)->f1; goto
_LL901;} else{ goto _LL902;} _LL902: goto _LL903; _LL901: yyzzz= _temp904; goto
_LL899; _LL903:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok); _LL899:;}
return yyzzz;} static unsigned char _temp907[ 8u]="tqual_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp907, _temp907, _temp907 +  8u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp908= yy1; struct Cyc_Absyn_Tqual _temp914; _LL910: if(*((
void**) _temp908) ==  Cyc_TypeQual_tok){ _LL915: _temp914=(( struct Cyc_TypeQual_tok_struct*)
_temp908)->f1; goto _LL911;} else{ goto _LL912;} _LL912: goto _LL913; _LL911:
yyzzz= _temp914; goto _LL909; _LL913:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL909:;} return yyzzz;} static unsigned char _temp917[ 22u]="list_t<structfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp917, _temp917, _temp917 +  22u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp918= yy1; struct Cyc_List_List*
_temp924; _LL920: if(*(( void**) _temp918) ==  Cyc_StructFieldDeclList_tok){
_LL925: _temp924=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp918)->f1;
goto _LL921;} else{ goto _LL922;} _LL922: goto _LL923; _LL921: yyzzz= _temp924;
goto _LL919; _LL923:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL919:;} return yyzzz;} static unsigned char _temp927[ 30u]="list_t<list_t<structfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp927, _temp927, _temp927 +  30u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{
void* _temp928= yy1; struct Cyc_List_List* _temp934; _LL930: if(*(( void**)
_temp928) ==  Cyc_StructFieldDeclListList_tok){ _LL935: _temp934=(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp928)->f1; goto _LL931;} else{ goto _LL932;} _LL932: goto _LL933; _LL931:
yyzzz= _temp934; goto _LL929; _LL933:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL929:;} return yyzzz;} static unsigned char _temp937[ 24u]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{
_temp937, _temp937, _temp937 +  24u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp938= yy1; struct Cyc_List_List*
_temp944; _LL940: if(*(( void**) _temp938) ==  Cyc_TypeModifierList_tok){ _LL945:
_temp944=(( struct Cyc_TypeModifierList_tok_struct*) _temp938)->f1; goto _LL941;}
else{ goto _LL942;} _LL942: goto _LL943; _LL941: yyzzz= _temp944; goto _LL939;
_LL943:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL939:;}
return yyzzz;} static unsigned char _temp947[ 13u]="declarator_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp947,
_temp947, _temp947 +  13u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp948= yy1; struct Cyc_Parse_Declarator*
_temp954; _LL950: if(*(( void**) _temp948) ==  Cyc_Declarator_tok){ _LL955:
_temp954=(( struct Cyc_Declarator_tok_struct*) _temp948)->f1; goto _LL951;}
else{ goto _LL952;} _LL952: goto _LL953; _LL951: yyzzz= _temp954; goto _LL949;
_LL953:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL949:;} return
yyzzz;} static unsigned char _temp957[ 21u]="abstractdeclarator_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{
_temp957, _temp957, _temp957 +  21u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp958= yy1;
struct Cyc_Parse_Abstractdeclarator* _temp964; _LL960: if(*(( void**) _temp958)
==  Cyc_AbstractDeclarator_tok){ _LL965: _temp964=(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp958)->f1; goto _LL961;} else{ goto _LL962;} _LL962: goto _LL963; _LL961:
yyzzz= _temp964; goto _LL959; _LL963:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL959:;} return yyzzz;} static unsigned char _temp967[ 5u]="bool"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp967,
_temp967, _temp967 +  5u}}; int Cyc_yyget_Bool_tok( void* yy1){ int yyzzz;{ void*
_temp968= yy1; int _temp974; _LL970: if(*(( void**) _temp968) ==  Cyc_Bool_tok){
_LL975: _temp974=(( struct Cyc_Bool_tok_struct*) _temp968)->f1; goto _LL971;}
else{ goto _LL972;} _LL972: goto _LL973; _LL971: yyzzz= _temp974; goto _LL969;
_LL973:( int) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL969:;} return yyzzz;}
static unsigned char _temp977[ 8u]="scope_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp977, _temp977, _temp977 +  8u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* yyzzz;{ void* _temp978= yy1; void*
_temp984; _LL980: if(*(( void**) _temp978) ==  Cyc_Scope_tok){ _LL985: _temp984=(
void*)(( struct Cyc_Scope_tok_struct*) _temp978)->f1; goto _LL981;} else{ goto
_LL982;} _LL982: goto _LL983; _LL981: yyzzz= _temp984; goto _LL979; _LL983:( int)
_throw(( void*)& Cyc_yyfail_Scope_tok); _LL979:;} return yyzzz;} static
unsigned char _temp987[ 14u]="tunionfield_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{ _temp987, _temp987, _temp987 + 
14u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok( void* yy1){
struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp988= yy1; struct Cyc_Absyn_Tunionfield*
_temp994; _LL990: if(*(( void**) _temp988) ==  Cyc_TunionField_tok){ _LL995:
_temp994=(( struct Cyc_TunionField_tok_struct*) _temp988)->f1; goto _LL991;}
else{ goto _LL992;} _LL992: goto _LL993; _LL991: yyzzz= _temp994; goto _LL989;
_LL993:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok); _LL989:;} return
yyzzz;} static unsigned char _temp997[ 22u]="list_t<tunionfield_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp997, _temp997, _temp997 +  22u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp998= yy1; struct Cyc_List_List*
_temp1004; _LL1000: if(*(( void**) _temp998) ==  Cyc_TunionFieldList_tok){
_LL1005: _temp1004=(( struct Cyc_TunionFieldList_tok_struct*) _temp998)->f1;
goto _LL1001;} else{ goto _LL1002;} _LL1002: goto _LL1003; _LL1001: yyzzz=
_temp1004; goto _LL999; _LL1003:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL999:;} return yyzzz;} static unsigned char _temp1007[ 50u]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1007, _temp1007, _temp1007 +  50u}}; struct _tuple17* Cyc_yyget_QualSpecList_tok(
void* yy1){ struct _tuple17* yyzzz;{ void* _temp1008= yy1; struct _tuple17*
_temp1014; _LL1010: if(*(( void**) _temp1008) ==  Cyc_QualSpecList_tok){ _LL1015:
_temp1014=(( struct Cyc_QualSpecList_tok_struct*) _temp1008)->f1; goto _LL1011;}
else{ goto _LL1012;} _LL1012: goto _LL1013; _LL1011: yyzzz= _temp1014; goto
_LL1009; _LL1013:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL1009:;}
return yyzzz;} static unsigned char _temp1017[ 14u]="list_t<var_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{
_temp1017, _temp1017, _temp1017 +  14u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1018= yy1; struct Cyc_List_List*
_temp1024; _LL1020: if(*(( void**) _temp1018) ==  Cyc_IdList_tok){ _LL1025:
_temp1024=(( struct Cyc_IdList_tok_struct*) _temp1018)->f1; goto _LL1021;} else{
goto _LL1022;} _LL1022: goto _LL1023; _LL1021: yyzzz= _temp1024; goto _LL1019;
_LL1023:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1019:;} return yyzzz;}
static unsigned char _temp1027[ 32u]="$(opt_t<var_t>,tqual_t,type_t)@"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{
_temp1027, _temp1027, _temp1027 +  32u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ struct _tuple2* yyzzz;{ void* _temp1028= yy1; struct _tuple2*
_temp1034; _LL1030: if(*(( void**) _temp1028) ==  Cyc_ParamDecl_tok){ _LL1035:
_temp1034=(( struct Cyc_ParamDecl_tok_struct*) _temp1028)->f1; goto _LL1031;}
else{ goto _LL1032;} _LL1032: goto _LL1033; _LL1031: yyzzz= _temp1034; goto
_LL1029; _LL1033:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1029:;}
return yyzzz;} static unsigned char _temp1037[ 40u]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{
_temp1037, _temp1037, _temp1037 +  40u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1038= yy1; struct Cyc_List_List*
_temp1044; _LL1040: if(*(( void**) _temp1038) ==  Cyc_ParamDeclList_tok){
_LL1045: _temp1044=(( struct Cyc_ParamDeclList_tok_struct*) _temp1038)->f1; goto
_LL1041;} else{ goto _LL1042;} _LL1042: goto _LL1043; _LL1041: yyzzz= _temp1044;
goto _LL1039; _LL1043:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1039:;} return yyzzz;} static unsigned char _temp1047[ 107u]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={ Cyc_Core_Failure,{
_temp1047, _temp1047, _temp1047 +  107u}}; struct _tuple16* Cyc_yyget_YY1( void*
yy1){ struct _tuple16* yyzzz;{ void* _temp1048= yy1; struct _tuple16* _temp1054;
_LL1050: if(*(( void**) _temp1048) ==  Cyc_YY1){ _LL1055: _temp1054=(( struct
Cyc_YY1_struct*) _temp1048)->f1; goto _LL1051;} else{ goto _LL1052;} _LL1052:
goto _LL1053; _LL1051: yyzzz= _temp1054; goto _LL1049; _LL1053:( int) _throw((
void*)& Cyc_yyfail_YY1); _LL1049:;} return yyzzz;} static unsigned char
_temp1057[ 15u]="list_t<type_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1057, _temp1057, _temp1057 +  15u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void*
_temp1058= yy1; struct Cyc_List_List* _temp1064; _LL1060: if(*(( void**)
_temp1058) ==  Cyc_TypeList_tok){ _LL1065: _temp1064=(( struct Cyc_TypeList_tok_struct*)
_temp1058)->f1; goto _LL1061;} else{ goto _LL1062;} _LL1062: goto _LL1063;
_LL1061: yyzzz= _temp1064; goto _LL1059; _LL1063:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok);
_LL1059:;} return yyzzz;} static unsigned char _temp1067[ 21u]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{
_temp1067, _temp1067, _temp1067 +  21u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1068= yy1; struct Cyc_List_List*
_temp1074; _LL1070: if(*(( void**) _temp1068) ==  Cyc_DesignatorList_tok){
_LL1075: _temp1074=(( struct Cyc_DesignatorList_tok_struct*) _temp1068)->f1;
goto _LL1071;} else{ goto _LL1072;} _LL1072: goto _LL1073; _LL1071: yyzzz=
_temp1074; goto _LL1069; _LL1073:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1069:;} return yyzzz;} static unsigned char _temp1077[ 13u]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{
_temp1077, _temp1077, _temp1077 +  13u}}; void* Cyc_yyget_Designator_tok( void*
yy1){ void* yyzzz;{ void* _temp1078= yy1; void* _temp1084; _LL1080: if(*(( void**)
_temp1078) ==  Cyc_Designator_tok){ _LL1085: _temp1084=( void*)(( struct Cyc_Designator_tok_struct*)
_temp1078)->f1; goto _LL1081;} else{ goto _LL1082;} _LL1082: goto _LL1083;
_LL1081: yyzzz= _temp1084; goto _LL1079; _LL1083:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL1079:;} return yyzzz;} static unsigned char _temp1087[ 7u]="kind_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{
_temp1087, _temp1087, _temp1087 +  7u}}; void* Cyc_yyget_Kind_tok( void* yy1){
void* yyzzz;{ void* _temp1088= yy1; void* _temp1094; _LL1090: if(*(( void**)
_temp1088) ==  Cyc_Kind_tok){ _LL1095: _temp1094=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1088)->f1; goto _LL1091;} else{ goto _LL1092;} _LL1092: goto _LL1093;
_LL1091: yyzzz= _temp1094; goto _LL1089; _LL1093:( int) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1089:;} return yyzzz;} static unsigned char _temp1097[ 7u]="type_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1097, _temp1097, _temp1097 +  7u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* yyzzz;{ void* _temp1098= yy1; void* _temp1104; _LL1100: if(*(( void**)
_temp1098) ==  Cyc_Type_tok){ _LL1105: _temp1104=( void*)(( struct Cyc_Type_tok_struct*)
_temp1098)->f1; goto _LL1101;} else{ goto _LL1102;} _LL1102: goto _LL1103;
_LL1101: yyzzz= _temp1104; goto _LL1099; _LL1103:( int) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1099:;} return yyzzz;} static unsigned char _temp1107[ 20u]="list_t<attribute_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1107, _temp1107, _temp1107 +  20u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1108= yy1; struct Cyc_List_List*
_temp1114; _LL1110: if(*(( void**) _temp1108) ==  Cyc_AttributeList_tok){
_LL1115: _temp1114=(( struct Cyc_AttributeList_tok_struct*) _temp1108)->f1; goto
_LL1111;} else{ goto _LL1112;} _LL1112: goto _LL1113; _LL1111: yyzzz= _temp1114;
goto _LL1109; _LL1113:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1109:;} return yyzzz;} static unsigned char _temp1117[ 12u]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{
_temp1117, _temp1117, _temp1117 +  12u}}; void* Cyc_yyget_Attribute_tok( void*
yy1){ void* yyzzz;{ void* _temp1118= yy1; void* _temp1124; _LL1120: if(*(( void**)
_temp1118) ==  Cyc_Attribute_tok){ _LL1125: _temp1124=( void*)(( struct Cyc_Attribute_tok_struct*)
_temp1118)->f1; goto _LL1121;} else{ goto _LL1122;} _LL1122: goto _LL1123;
_LL1121: yyzzz= _temp1124; goto _LL1119; _LL1123:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1119:;} return yyzzz;} static unsigned char _temp1127[ 12u]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{
_temp1127, _temp1127, _temp1127 +  12u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
void* yy1){ struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp1128= yy1; struct Cyc_Absyn_Enumfield*
_temp1134; _LL1130: if(*(( void**) _temp1128) ==  Cyc_Enumfield_tok){ _LL1135:
_temp1134=(( struct Cyc_Enumfield_tok_struct*) _temp1128)->f1; goto _LL1131;}
else{ goto _LL1132;} _LL1132: goto _LL1133; _LL1131: yyzzz= _temp1134; goto
_LL1129; _LL1133:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1129:;}
return yyzzz;} static unsigned char _temp1137[ 20u]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1137, _temp1137, _temp1137 +  20u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1138= yy1; struct Cyc_List_List*
_temp1144; _LL1140: if(*(( void**) _temp1138) ==  Cyc_EnumfieldList_tok){
_LL1145: _temp1144=(( struct Cyc_EnumfieldList_tok_struct*) _temp1138)->f1; goto
_LL1141;} else{ goto _LL1142;} _LL1142: goto _LL1143; _LL1141: yyzzz= _temp1144;
goto _LL1139; _LL1143:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1139:;} return yyzzz;} static unsigned char _temp1147[ 14u]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{
_temp1147, _temp1147, _temp1147 +  14u}}; struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok(
void* yy1){ struct Cyc_Core_Opt* yyzzz;{ void* _temp1148= yy1; struct Cyc_Core_Opt*
_temp1154; _LL1150: if(*(( void**) _temp1148) ==  Cyc_TypeOpt_tok){ _LL1155:
_temp1154=(( struct Cyc_TypeOpt_tok_struct*) _temp1148)->f1; goto _LL1151;}
else{ goto _LL1152;} _LL1152: goto _LL1153; _LL1151: yyzzz= _temp1154; goto
_LL1149; _LL1153:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1149:;}
return yyzzz;} static unsigned char _temp1157[ 26u]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{
_temp1157, _temp1157, _temp1157 +  26u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1158= yy1; struct Cyc_List_List*
_temp1164; _LL1160: if(*(( void**) _temp1158) ==  Cyc_Rgnorder_tok){ _LL1165:
_temp1164=(( struct Cyc_Rgnorder_tok_struct*) _temp1158)->f1; goto _LL1161;}
else{ goto _LL1162;} _LL1162: goto _LL1163; _LL1161: yyzzz= _temp1164; goto
_LL1159; _LL1163:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1159:;}
return yyzzz;} struct Cyc_Yyltype{ int timestamp; int first_line; int
first_column; int last_line; int last_column; } ; struct Cyc_Yyltype Cyc_yynewloc(){
return({ struct Cyc_Yyltype _temp1166; _temp1166.timestamp= 0; _temp1166.first_line=
0; _temp1166.first_column= 0; _temp1166.last_line= 0; _temp1166.last_column= 0;
_temp1166;});} struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[
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
unsigned char _temp1167[ 2u]="$"; static unsigned char _temp1168[ 6u]="error";
static unsigned char _temp1169[ 12u]="$undefined."; static unsigned char
_temp1170[ 5u]="AUTO"; static unsigned char _temp1171[ 9u]="REGISTER"; static
unsigned char _temp1172[ 7u]="STATIC"; static unsigned char _temp1173[ 7u]="EXTERN";
static unsigned char _temp1174[ 8u]="TYPEDEF"; static unsigned char _temp1175[ 5u]="VOID";
static unsigned char _temp1176[ 5u]="CHAR"; static unsigned char _temp1177[ 6u]="SHORT";
static unsigned char _temp1178[ 4u]="INT"; static unsigned char _temp1179[ 5u]="LONG";
static unsigned char _temp1180[ 6u]="FLOAT"; static unsigned char _temp1181[ 7u]="DOUBLE";
static unsigned char _temp1182[ 7u]="SIGNED"; static unsigned char _temp1183[ 9u]="UNSIGNED";
static unsigned char _temp1184[ 6u]="CONST"; static unsigned char _temp1185[ 9u]="VOLATILE";
static unsigned char _temp1186[ 9u]="RESTRICT"; static unsigned char _temp1187[
7u]="STRUCT"; static unsigned char _temp1188[ 6u]="UNION"; static unsigned char
_temp1189[ 5u]="CASE"; static unsigned char _temp1190[ 8u]="DEFAULT"; static
unsigned char _temp1191[ 7u]="INLINE"; static unsigned char _temp1192[ 7u]="SIZEOF";
static unsigned char _temp1193[ 9u]="OFFSETOF"; static unsigned char _temp1194[
3u]="IF"; static unsigned char _temp1195[ 5u]="ELSE"; static unsigned char
_temp1196[ 7u]="SWITCH"; static unsigned char _temp1197[ 6u]="WHILE"; static
unsigned char _temp1198[ 3u]="DO"; static unsigned char _temp1199[ 4u]="FOR";
static unsigned char _temp1200[ 5u]="GOTO"; static unsigned char _temp1201[ 9u]="CONTINUE";
static unsigned char _temp1202[ 6u]="BREAK"; static unsigned char _temp1203[ 7u]="RETURN";
static unsigned char _temp1204[ 5u]="ENUM"; static unsigned char _temp1205[ 8u]="NULL_kw";
static unsigned char _temp1206[ 4u]="LET"; static unsigned char _temp1207[ 6u]="THROW";
static unsigned char _temp1208[ 4u]="TRY"; static unsigned char _temp1209[ 6u]="CATCH";
static unsigned char _temp1210[ 4u]="NEW"; static unsigned char _temp1211[ 9u]="ABSTRACT";
static unsigned char _temp1212[ 9u]="FALLTHRU"; static unsigned char _temp1213[
6u]="USING"; static unsigned char _temp1214[ 10u]="NAMESPACE"; static
unsigned char _temp1215[ 7u]="TUNION"; static unsigned char _temp1216[ 8u]="XTUNION";
static unsigned char _temp1217[ 5u]="FILL"; static unsigned char _temp1218[ 8u]="CODEGEN";
static unsigned char _temp1219[ 4u]="CUT"; static unsigned char _temp1220[ 7u]="SPLICE";
static unsigned char _temp1221[ 7u]="MALLOC"; static unsigned char _temp1222[ 9u]="REGION_T";
static unsigned char _temp1223[ 7u]="REGION"; static unsigned char _temp1224[ 5u]="RNEW";
static unsigned char _temp1225[ 8u]="RMALLOC"; static unsigned char _temp1226[ 8u]="REGIONS";
static unsigned char _temp1227[ 4u]="GEN"; static unsigned char _temp1228[ 7u]="PTR_OP";
static unsigned char _temp1229[ 7u]="INC_OP"; static unsigned char _temp1230[ 7u]="DEC_OP";
static unsigned char _temp1231[ 8u]="LEFT_OP"; static unsigned char _temp1232[ 9u]="RIGHT_OP";
static unsigned char _temp1233[ 6u]="LE_OP"; static unsigned char _temp1234[ 6u]="GE_OP";
static unsigned char _temp1235[ 6u]="EQ_OP"; static unsigned char _temp1236[ 6u]="NE_OP";
static unsigned char _temp1237[ 7u]="AND_OP"; static unsigned char _temp1238[ 6u]="OR_OP";
static unsigned char _temp1239[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1240[ 11u]="DIV_ASSIGN"; static unsigned char _temp1241[ 11u]="MOD_ASSIGN";
static unsigned char _temp1242[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1243[ 11u]="SUB_ASSIGN"; static unsigned char _temp1244[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1245[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1246[ 11u]="AND_ASSIGN"; static unsigned char _temp1247[ 11u]="XOR_ASSIGN";
static unsigned char _temp1248[ 10u]="OR_ASSIGN"; static unsigned char _temp1249[
9u]="ELLIPSIS"; static unsigned char _temp1250[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1251[ 12u]="COLON_COLON"; static unsigned char _temp1252[ 11u]="IDENTIFIER";
static unsigned char _temp1253[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1254[ 7u]="STRING"; static unsigned char _temp1255[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1256[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1257[ 9u]="TYPE_VAR"; static unsigned char _temp1258[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1259[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1260[ 10u]="ATTRIBUTE"; static unsigned char _temp1261[ 4u]="';'"; static
unsigned char _temp1262[ 4u]="'{'"; static unsigned char _temp1263[ 4u]="'}'";
static unsigned char _temp1264[ 4u]="'='"; static unsigned char _temp1265[ 4u]="'('";
static unsigned char _temp1266[ 4u]="')'"; static unsigned char _temp1267[ 4u]="','";
static unsigned char _temp1268[ 4u]="'_'"; static unsigned char _temp1269[ 4u]="'$'";
static unsigned char _temp1270[ 4u]="'<'"; static unsigned char _temp1271[ 4u]="'>'";
static unsigned char _temp1272[ 4u]="':'"; static unsigned char _temp1273[ 4u]="'.'";
static unsigned char _temp1274[ 4u]="'['"; static unsigned char _temp1275[ 4u]="']'";
static unsigned char _temp1276[ 4u]="'*'"; static unsigned char _temp1277[ 4u]="'@'";
static unsigned char _temp1278[ 4u]="'?'"; static unsigned char _temp1279[ 4u]="'+'";
static unsigned char _temp1280[ 4u]="'-'"; static unsigned char _temp1281[ 4u]="'&'";
static unsigned char _temp1282[ 4u]="'|'"; static unsigned char _temp1283[ 4u]="'^'";
static unsigned char _temp1284[ 4u]="'/'"; static unsigned char _temp1285[ 4u]="'%'";
static unsigned char _temp1286[ 4u]="'~'"; static unsigned char _temp1287[ 4u]="'!'";
static unsigned char _temp1288[ 5u]="prog"; static unsigned char _temp1289[ 17u]="translation_unit";
static unsigned char _temp1290[ 21u]="external_declaration"; static
unsigned char _temp1291[ 20u]="function_definition"; static unsigned char
_temp1292[ 21u]="function_definition2"; static unsigned char _temp1293[ 13u]="using_action";
static unsigned char _temp1294[ 15u]="unusing_action"; static unsigned char
_temp1295[ 17u]="namespace_action"; static unsigned char _temp1296[ 19u]="unnamespace_action";
static unsigned char _temp1297[ 12u]="declaration"; static unsigned char
_temp1298[ 17u]="declaration_list"; static unsigned char _temp1299[ 23u]="declaration_specifiers";
static unsigned char _temp1300[ 24u]="storage_class_specifier"; static
unsigned char _temp1301[ 15u]="attributes_opt"; static unsigned char _temp1302[
11u]="attributes"; static unsigned char _temp1303[ 15u]="attribute_list"; static
unsigned char _temp1304[ 10u]="attribute"; static unsigned char _temp1305[ 15u]="type_specifier";
static unsigned char _temp1306[ 5u]="kind"; static unsigned char _temp1307[ 15u]="type_qualifier";
static unsigned char _temp1308[ 15u]="enum_specifier"; static unsigned char
_temp1309[ 11u]="enum_field"; static unsigned char _temp1310[ 22u]="enum_declaration_list";
static unsigned char _temp1311[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1312[ 16u]="type_params_opt"; static unsigned char _temp1313[
16u]="struct_or_union"; static unsigned char _temp1314[ 24u]="struct_declaration_list";
static unsigned char _temp1315[ 25u]="struct_declaration_list0"; static
unsigned char _temp1316[ 21u]="init_declarator_list"; static unsigned char
_temp1317[ 22u]="init_declarator_list0"; static unsigned char _temp1318[ 16u]="init_declarator";
static unsigned char _temp1319[ 19u]="struct_declaration"; static unsigned char
_temp1320[ 25u]="specifier_qualifier_list"; static unsigned char _temp1321[ 23u]="struct_declarator_list";
static unsigned char _temp1322[ 24u]="struct_declarator_list0"; static
unsigned char _temp1323[ 18u]="struct_declarator"; static unsigned char
_temp1324[ 17u]="tunion_specifier"; static unsigned char _temp1325[ 18u]="tunion_or_xtunion";
static unsigned char _temp1326[ 17u]="tunionfield_list"; static unsigned char
_temp1327[ 18u]="tunionfield_scope"; static unsigned char _temp1328[ 12u]="tunionfield";
static unsigned char _temp1329[ 11u]="declarator"; static unsigned char
_temp1330[ 18u]="direct_declarator"; static unsigned char _temp1331[ 8u]="pointer";
static unsigned char _temp1332[ 13u]="pointer_char"; static unsigned char
_temp1333[ 8u]="rgn_opt"; static unsigned char _temp1334[ 4u]="rgn"; static
unsigned char _temp1335[ 20u]="type_qualifier_list"; static unsigned char
_temp1336[ 20u]="parameter_type_list"; static unsigned char _temp1337[ 9u]="type_var";
static unsigned char _temp1338[ 16u]="optional_effect"; static unsigned char
_temp1339[ 19u]="optional_rgn_order"; static unsigned char _temp1340[ 10u]="rgn_order";
static unsigned char _temp1341[ 16u]="optional_inject"; static unsigned char
_temp1342[ 11u]="effect_set"; static unsigned char _temp1343[ 14u]="atomic_effect";
static unsigned char _temp1344[ 11u]="region_set"; static unsigned char
_temp1345[ 15u]="parameter_list"; static unsigned char _temp1346[ 22u]="parameter_declaration";
static unsigned char _temp1347[ 16u]="identifier_list"; static unsigned char
_temp1348[ 17u]="identifier_list0"; static unsigned char _temp1349[ 12u]="initializer";
static unsigned char _temp1350[ 18u]="array_initializer"; static unsigned char
_temp1351[ 17u]="initializer_list"; static unsigned char _temp1352[ 12u]="designation";
static unsigned char _temp1353[ 16u]="designator_list"; static unsigned char
_temp1354[ 11u]="designator"; static unsigned char _temp1355[ 10u]="type_name";
static unsigned char _temp1356[ 14u]="any_type_name"; static unsigned char
_temp1357[ 15u]="type_name_list"; static unsigned char _temp1358[ 20u]="abstract_declarator";
static unsigned char _temp1359[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1360[ 10u]="statement"; static unsigned char _temp1361[ 18u]="labeled_statement";
static unsigned char _temp1362[ 21u]="expression_statement"; static
unsigned char _temp1363[ 19u]="compound_statement"; static unsigned char
_temp1364[ 16u]="block_item_list"; static unsigned char _temp1365[ 20u]="selection_statement";
static unsigned char _temp1366[ 15u]="switch_clauses"; static unsigned char
_temp1367[ 16u]="switchC_clauses"; static unsigned char _temp1368[ 20u]="iteration_statement";
static unsigned char _temp1369[ 15u]="jump_statement"; static unsigned char
_temp1370[ 8u]="pattern"; static unsigned char _temp1371[ 19u]="tuple_pattern_list";
static unsigned char _temp1372[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1373[ 14u]="field_pattern"; static unsigned char _temp1374[ 19u]="field_pattern_list";
static unsigned char _temp1375[ 20u]="field_pattern_list0"; static unsigned char
_temp1376[ 11u]="expression"; static unsigned char _temp1377[ 22u]="assignment_expression";
static unsigned char _temp1378[ 20u]="assignment_operator"; static unsigned char
_temp1379[ 23u]="conditional_expression"; static unsigned char _temp1380[ 20u]="constant_expression";
static unsigned char _temp1381[ 22u]="logical_or_expression"; static
unsigned char _temp1382[ 23u]="logical_and_expression"; static unsigned char
_temp1383[ 24u]="inclusive_or_expression"; static unsigned char _temp1384[ 24u]="exclusive_or_expression";
static unsigned char _temp1385[ 15u]="and_expression"; static unsigned char
_temp1386[ 20u]="equality_expression"; static unsigned char _temp1387[ 22u]="relational_expression";
static unsigned char _temp1388[ 17u]="shift_expression"; static unsigned char
_temp1389[ 20u]="additive_expression"; static unsigned char _temp1390[ 26u]="multiplicative_expression";
static unsigned char _temp1391[ 16u]="cast_expression"; static unsigned char
_temp1392[ 17u]="unary_expression"; static unsigned char _temp1393[ 15u]="unary_operator";
static unsigned char _temp1394[ 19u]="postfix_expression"; static unsigned char
_temp1395[ 19u]="primary_expression"; static unsigned char _temp1396[ 25u]="argument_expression_list";
static unsigned char _temp1397[ 26u]="argument_expression_list0"; static
unsigned char _temp1398[ 9u]="constant"; static unsigned char _temp1399[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 233u]={{ _temp1167, _temp1167, _temp1167
+  2u},{ _temp1168, _temp1168, _temp1168 +  6u},{ _temp1169, _temp1169,
_temp1169 +  12u},{ _temp1170, _temp1170, _temp1170 +  5u},{ _temp1171,
_temp1171, _temp1171 +  9u},{ _temp1172, _temp1172, _temp1172 +  7u},{ _temp1173,
_temp1173, _temp1173 +  7u},{ _temp1174, _temp1174, _temp1174 +  8u},{ _temp1175,
_temp1175, _temp1175 +  5u},{ _temp1176, _temp1176, _temp1176 +  5u},{ _temp1177,
_temp1177, _temp1177 +  6u},{ _temp1178, _temp1178, _temp1178 +  4u},{ _temp1179,
_temp1179, _temp1179 +  5u},{ _temp1180, _temp1180, _temp1180 +  6u},{ _temp1181,
_temp1181, _temp1181 +  7u},{ _temp1182, _temp1182, _temp1182 +  7u},{ _temp1183,
_temp1183, _temp1183 +  9u},{ _temp1184, _temp1184, _temp1184 +  6u},{ _temp1185,
_temp1185, _temp1185 +  9u},{ _temp1186, _temp1186, _temp1186 +  9u},{ _temp1187,
_temp1187, _temp1187 +  7u},{ _temp1188, _temp1188, _temp1188 +  6u},{ _temp1189,
_temp1189, _temp1189 +  5u},{ _temp1190, _temp1190, _temp1190 +  8u},{ _temp1191,
_temp1191, _temp1191 +  7u},{ _temp1192, _temp1192, _temp1192 +  7u},{ _temp1193,
_temp1193, _temp1193 +  9u},{ _temp1194, _temp1194, _temp1194 +  3u},{ _temp1195,
_temp1195, _temp1195 +  5u},{ _temp1196, _temp1196, _temp1196 +  7u},{ _temp1197,
_temp1197, _temp1197 +  6u},{ _temp1198, _temp1198, _temp1198 +  3u},{ _temp1199,
_temp1199, _temp1199 +  4u},{ _temp1200, _temp1200, _temp1200 +  5u},{ _temp1201,
_temp1201, _temp1201 +  9u},{ _temp1202, _temp1202, _temp1202 +  6u},{ _temp1203,
_temp1203, _temp1203 +  7u},{ _temp1204, _temp1204, _temp1204 +  5u},{ _temp1205,
_temp1205, _temp1205 +  8u},{ _temp1206, _temp1206, _temp1206 +  4u},{ _temp1207,
_temp1207, _temp1207 +  6u},{ _temp1208, _temp1208, _temp1208 +  4u},{ _temp1209,
_temp1209, _temp1209 +  6u},{ _temp1210, _temp1210, _temp1210 +  4u},{ _temp1211,
_temp1211, _temp1211 +  9u},{ _temp1212, _temp1212, _temp1212 +  9u},{ _temp1213,
_temp1213, _temp1213 +  6u},{ _temp1214, _temp1214, _temp1214 +  10u},{
_temp1215, _temp1215, _temp1215 +  7u},{ _temp1216, _temp1216, _temp1216 +  8u},{
_temp1217, _temp1217, _temp1217 +  5u},{ _temp1218, _temp1218, _temp1218 +  8u},{
_temp1219, _temp1219, _temp1219 +  4u},{ _temp1220, _temp1220, _temp1220 +  7u},{
_temp1221, _temp1221, _temp1221 +  7u},{ _temp1222, _temp1222, _temp1222 +  9u},{
_temp1223, _temp1223, _temp1223 +  7u},{ _temp1224, _temp1224, _temp1224 +  5u},{
_temp1225, _temp1225, _temp1225 +  8u},{ _temp1226, _temp1226, _temp1226 +  8u},{
_temp1227, _temp1227, _temp1227 +  4u},{ _temp1228, _temp1228, _temp1228 +  7u},{
_temp1229, _temp1229, _temp1229 +  7u},{ _temp1230, _temp1230, _temp1230 +  7u},{
_temp1231, _temp1231, _temp1231 +  8u},{ _temp1232, _temp1232, _temp1232 +  9u},{
_temp1233, _temp1233, _temp1233 +  6u},{ _temp1234, _temp1234, _temp1234 +  6u},{
_temp1235, _temp1235, _temp1235 +  6u},{ _temp1236, _temp1236, _temp1236 +  6u},{
_temp1237, _temp1237, _temp1237 +  7u},{ _temp1238, _temp1238, _temp1238 +  6u},{
_temp1239, _temp1239, _temp1239 +  11u},{ _temp1240, _temp1240, _temp1240 +  11u},{
_temp1241, _temp1241, _temp1241 +  11u},{ _temp1242, _temp1242, _temp1242 +  11u},{
_temp1243, _temp1243, _temp1243 +  11u},{ _temp1244, _temp1244, _temp1244 +  12u},{
_temp1245, _temp1245, _temp1245 +  13u},{ _temp1246, _temp1246, _temp1246 +  11u},{
_temp1247, _temp1247, _temp1247 +  11u},{ _temp1248, _temp1248, _temp1248 +  10u},{
_temp1249, _temp1249, _temp1249 +  9u},{ _temp1250, _temp1250, _temp1250 +  11u},{
_temp1251, _temp1251, _temp1251 +  12u},{ _temp1252, _temp1252, _temp1252 +  11u},{
_temp1253, _temp1253, _temp1253 +  17u},{ _temp1254, _temp1254, _temp1254 +  7u},{
_temp1255, _temp1255, _temp1255 +  19u},{ _temp1256, _temp1256, _temp1256 +  18u},{
_temp1257, _temp1257, _temp1257 +  9u},{ _temp1258, _temp1258, _temp1258 +  16u},{
_temp1259, _temp1259, _temp1259 +  18u},{ _temp1260, _temp1260, _temp1260 +  10u},{
_temp1261, _temp1261, _temp1261 +  4u},{ _temp1262, _temp1262, _temp1262 +  4u},{
_temp1263, _temp1263, _temp1263 +  4u},{ _temp1264, _temp1264, _temp1264 +  4u},{
_temp1265, _temp1265, _temp1265 +  4u},{ _temp1266, _temp1266, _temp1266 +  4u},{
_temp1267, _temp1267, _temp1267 +  4u},{ _temp1268, _temp1268, _temp1268 +  4u},{
_temp1269, _temp1269, _temp1269 +  4u},{ _temp1270, _temp1270, _temp1270 +  4u},{
_temp1271, _temp1271, _temp1271 +  4u},{ _temp1272, _temp1272, _temp1272 +  4u},{
_temp1273, _temp1273, _temp1273 +  4u},{ _temp1274, _temp1274, _temp1274 +  4u},{
_temp1275, _temp1275, _temp1275 +  4u},{ _temp1276, _temp1276, _temp1276 +  4u},{
_temp1277, _temp1277, _temp1277 +  4u},{ _temp1278, _temp1278, _temp1278 +  4u},{
_temp1279, _temp1279, _temp1279 +  4u},{ _temp1280, _temp1280, _temp1280 +  4u},{
_temp1281, _temp1281, _temp1281 +  4u},{ _temp1282, _temp1282, _temp1282 +  4u},{
_temp1283, _temp1283, _temp1283 +  4u},{ _temp1284, _temp1284, _temp1284 +  4u},{
_temp1285, _temp1285, _temp1285 +  4u},{ _temp1286, _temp1286, _temp1286 +  4u},{
_temp1287, _temp1287, _temp1287 +  4u},{ _temp1288, _temp1288, _temp1288 +  5u},{
_temp1289, _temp1289, _temp1289 +  17u},{ _temp1290, _temp1290, _temp1290 +  21u},{
_temp1291, _temp1291, _temp1291 +  20u},{ _temp1292, _temp1292, _temp1292 +  21u},{
_temp1293, _temp1293, _temp1293 +  13u},{ _temp1294, _temp1294, _temp1294 +  15u},{
_temp1295, _temp1295, _temp1295 +  17u},{ _temp1296, _temp1296, _temp1296 +  19u},{
_temp1297, _temp1297, _temp1297 +  12u},{ _temp1298, _temp1298, _temp1298 +  17u},{
_temp1299, _temp1299, _temp1299 +  23u},{ _temp1300, _temp1300, _temp1300 +  24u},{
_temp1301, _temp1301, _temp1301 +  15u},{ _temp1302, _temp1302, _temp1302 +  11u},{
_temp1303, _temp1303, _temp1303 +  15u},{ _temp1304, _temp1304, _temp1304 +  10u},{
_temp1305, _temp1305, _temp1305 +  15u},{ _temp1306, _temp1306, _temp1306 +  5u},{
_temp1307, _temp1307, _temp1307 +  15u},{ _temp1308, _temp1308, _temp1308 +  15u},{
_temp1309, _temp1309, _temp1309 +  11u},{ _temp1310, _temp1310, _temp1310 +  22u},{
_temp1311, _temp1311, _temp1311 +  26u},{ _temp1312, _temp1312, _temp1312 +  16u},{
_temp1313, _temp1313, _temp1313 +  16u},{ _temp1314, _temp1314, _temp1314 +  24u},{
_temp1315, _temp1315, _temp1315 +  25u},{ _temp1316, _temp1316, _temp1316 +  21u},{
_temp1317, _temp1317, _temp1317 +  22u},{ _temp1318, _temp1318, _temp1318 +  16u},{
_temp1319, _temp1319, _temp1319 +  19u},{ _temp1320, _temp1320, _temp1320 +  25u},{
_temp1321, _temp1321, _temp1321 +  23u},{ _temp1322, _temp1322, _temp1322 +  24u},{
_temp1323, _temp1323, _temp1323 +  18u},{ _temp1324, _temp1324, _temp1324 +  17u},{
_temp1325, _temp1325, _temp1325 +  18u},{ _temp1326, _temp1326, _temp1326 +  17u},{
_temp1327, _temp1327, _temp1327 +  18u},{ _temp1328, _temp1328, _temp1328 +  12u},{
_temp1329, _temp1329, _temp1329 +  11u},{ _temp1330, _temp1330, _temp1330 +  18u},{
_temp1331, _temp1331, _temp1331 +  8u},{ _temp1332, _temp1332, _temp1332 +  13u},{
_temp1333, _temp1333, _temp1333 +  8u},{ _temp1334, _temp1334, _temp1334 +  4u},{
_temp1335, _temp1335, _temp1335 +  20u},{ _temp1336, _temp1336, _temp1336 +  20u},{
_temp1337, _temp1337, _temp1337 +  9u},{ _temp1338, _temp1338, _temp1338 +  16u},{
_temp1339, _temp1339, _temp1339 +  19u},{ _temp1340, _temp1340, _temp1340 +  10u},{
_temp1341, _temp1341, _temp1341 +  16u},{ _temp1342, _temp1342, _temp1342 +  11u},{
_temp1343, _temp1343, _temp1343 +  14u},{ _temp1344, _temp1344, _temp1344 +  11u},{
_temp1345, _temp1345, _temp1345 +  15u},{ _temp1346, _temp1346, _temp1346 +  22u},{
_temp1347, _temp1347, _temp1347 +  16u},{ _temp1348, _temp1348, _temp1348 +  17u},{
_temp1349, _temp1349, _temp1349 +  12u},{ _temp1350, _temp1350, _temp1350 +  18u},{
_temp1351, _temp1351, _temp1351 +  17u},{ _temp1352, _temp1352, _temp1352 +  12u},{
_temp1353, _temp1353, _temp1353 +  16u},{ _temp1354, _temp1354, _temp1354 +  11u},{
_temp1355, _temp1355, _temp1355 +  10u},{ _temp1356, _temp1356, _temp1356 +  14u},{
_temp1357, _temp1357, _temp1357 +  15u},{ _temp1358, _temp1358, _temp1358 +  20u},{
_temp1359, _temp1359, _temp1359 +  27u},{ _temp1360, _temp1360, _temp1360 +  10u},{
_temp1361, _temp1361, _temp1361 +  18u},{ _temp1362, _temp1362, _temp1362 +  21u},{
_temp1363, _temp1363, _temp1363 +  19u},{ _temp1364, _temp1364, _temp1364 +  16u},{
_temp1365, _temp1365, _temp1365 +  20u},{ _temp1366, _temp1366, _temp1366 +  15u},{
_temp1367, _temp1367, _temp1367 +  16u},{ _temp1368, _temp1368, _temp1368 +  20u},{
_temp1369, _temp1369, _temp1369 +  15u},{ _temp1370, _temp1370, _temp1370 +  8u},{
_temp1371, _temp1371, _temp1371 +  19u},{ _temp1372, _temp1372, _temp1372 +  20u},{
_temp1373, _temp1373, _temp1373 +  14u},{ _temp1374, _temp1374, _temp1374 +  19u},{
_temp1375, _temp1375, _temp1375 +  20u},{ _temp1376, _temp1376, _temp1376 +  11u},{
_temp1377, _temp1377, _temp1377 +  22u},{ _temp1378, _temp1378, _temp1378 +  20u},{
_temp1379, _temp1379, _temp1379 +  23u},{ _temp1380, _temp1380, _temp1380 +  20u},{
_temp1381, _temp1381, _temp1381 +  22u},{ _temp1382, _temp1382, _temp1382 +  23u},{
_temp1383, _temp1383, _temp1383 +  24u},{ _temp1384, _temp1384, _temp1384 +  24u},{
_temp1385, _temp1385, _temp1385 +  15u},{ _temp1386, _temp1386, _temp1386 +  20u},{
_temp1387, _temp1387, _temp1387 +  22u},{ _temp1388, _temp1388, _temp1388 +  17u},{
_temp1389, _temp1389, _temp1389 +  20u},{ _temp1390, _temp1390, _temp1390 +  26u},{
_temp1391, _temp1391, _temp1391 +  16u},{ _temp1392, _temp1392, _temp1392 +  17u},{
_temp1393, _temp1393, _temp1393 +  15u},{ _temp1394, _temp1394, _temp1394 +  19u},{
_temp1395, _temp1395, _temp1395 +  19u},{ _temp1396, _temp1396, _temp1396 +  25u},{
_temp1397, _temp1397, _temp1397 +  26u},{ _temp1398, _temp1398, _temp1398 +  9u},{
_temp1399, _temp1399, _temp1399 +  20u}}; static short Cyc_yyr1[ 406u]={ 0, 121,
122, 122, 122, 122, 122, 122, 122, 123, 123, 124, 124, 124, 124, 125, 125, 126,
127, 128, 129, 130, 130, 130, 130, 131, 131, 132, 132, 132, 132, 132, 132, 132,
132, 133, 133, 133, 133, 133, 133, 133, 134, 134, 135, 136, 136, 137, 137, 137,
137, 137, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
138, 138, 138, 139, 139, 140, 140, 140, 141, 141, 141, 142, 142, 143, 143, 144,
144, 144, 144, 144, 145, 145, 145, 146, 146, 147, 148, 148, 149, 150, 150, 151,
151, 152, 153, 153, 153, 153, 154, 155, 155, 156, 156, 156, 157, 157, 157, 157,
158, 158, 159, 159, 159, 159, 160, 160, 160, 161, 161, 162, 162, 163, 163, 163,
163, 163, 163, 163, 163, 163, 163, 163, 163, 164, 164, 164, 164, 165, 165, 165,
165, 165, 166, 166, 167, 167, 168, 168, 169, 169, 169, 169, 170, 170, 171, 171,
172, 172, 173, 173, 174, 174, 175, 175, 176, 176, 176, 176, 177, 177, 178, 178,
179, 179, 179, 180, 181, 181, 182, 182, 183, 183, 183, 183, 184, 184, 184, 184,
185, 186, 186, 187, 187, 188, 188, 189, 189, 189, 189, 189, 190, 190, 191, 191,
191, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 193, 193, 193,
193, 193, 193, 193, 193, 193, 193, 194, 195, 195, 196, 196, 197, 197, 197, 197,
197, 197, 198, 198, 198, 198, 198, 199, 199, 199, 199, 199, 199, 200, 200, 200,
200, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 202,
202, 202, 202, 202, 202, 202, 202, 203, 203, 203, 203, 203, 203, 203, 203, 203,
203, 203, 203, 203, 203, 204, 204, 205, 205, 206, 206, 207, 208, 208, 209, 209,
210, 210, 211, 211, 211, 211, 211, 211, 211, 211, 211, 211, 211, 212, 212, 212,
212, 212, 212, 212, 213, 214, 214, 215, 215, 216, 216, 217, 217, 218, 218, 219,
219, 219, 220, 220, 220, 220, 220, 221, 221, 221, 222, 222, 222, 223, 223, 223,
223, 224, 224, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225,
225, 226, 226, 226, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227,
227, 227, 228, 228, 228, 228, 228, 228, 228, 228, 228, 229, 230, 230, 231, 231,
231, 231, 232, 232}; static short Cyc_yyr2[ 406u]={ 0, 1, 2, 3, 5, 3, 5, 6, 0, 1,
1, 2, 3, 3, 4, 3, 4, 2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1,
1, 1, 2, 1, 1, 0, 1, 6, 1, 3, 1, 1, 4, 4, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 2, 4, 4, 1, 1, 1, 1, 1, 5, 2, 4, 1, 3, 1, 3, 4, 6, 6, 3, 3, 0, 3, 3, 1, 1,
1, 1, 2, 1, 1, 3, 1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3, 6, 4, 3, 5, 1, 1, 1, 2,
3, 3, 0, 1, 1, 2, 6, 1, 2, 1, 3, 3, 4, 4, 5, 4, 4, 4, 2, 2, 1, 3, 4, 4, 5, 1, 1,
4, 4, 1, 0, 1, 1, 1, 1, 2, 3, 5, 5, 7, 1, 3, 0, 2, 0, 2, 3, 5, 0, 1, 1, 3, 2, 3,
4, 1, 1, 3, 1, 3, 2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2,
1, 2, 1, 2, 3, 4, 3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 1, 1, 1,
1, 1, 1, 6, 3, 2, 2, 3, 1, 2, 2, 3, 1, 2, 1, 2, 1, 2, 5, 7, 7, 8, 6, 0, 3, 4, 5,
6, 7, 0, 3, 4, 5, 5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2, 3, 2, 4,
5, 1, 3, 1, 2, 1, 1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3, 1, 3, 1,
3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3, 1, 3, 1,
3, 1, 3, 1, 3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1, 2, 2, 2,
2, 2, 2, 4, 2, 6, 6, 5, 7, 9, 1, 1, 1, 1, 4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4,
1, 1, 1, 3, 2, 5, 4, 4, 5, 1, 1, 3, 1, 1, 1, 1, 1, 1}; static short Cyc_yydefact[
804u]={ 0, 138, 35, 36, 37, 38, 40, 52, 54, 55, 56, 57, 58, 59, 60, 61, 71, 72,
73, 89, 90, 42, 0, 0, 41, 0, 0, 114, 115, 0, 404, 158, 405, 86, 0, 53, 0, 143,
144, 147, 1, 0, 9, 0, 0, 10, 0, 42, 42, 42, 62, 63, 0, 64, 0, 0, 125, 0, 148, 65,
127, 39, 0, 33, 43, 0, 75, 287, 404, 283, 286, 285, 0, 281, 0, 0, 0, 0, 0, 181,
0, 288, 17, 19, 0, 0, 0, 66, 0, 0, 0, 0, 2, 0, 0, 0, 0, 21, 0, 94, 95, 97, 27,
29, 31, 86, 0, 86, 151, 0, 150, 86, 38, 0, 25, 0, 0, 11, 160, 0, 0, 136, 126, 42,
149, 137, 0, 0, 34, 79, 0, 77, 0, 0, 295, 294, 284, 293, 24, 0, 0, 0, 0, 0, 42,
42, 199, 201, 0, 69, 70, 159, 206, 0, 128, 0, 0, 176, 0, 0, 403, 0, 0, 0, 0, 0,
0, 0, 86, 0, 0, 400, 390, 401, 402, 0, 0, 0, 0, 373, 0, 371, 372, 0, 306, 319,
327, 329, 331, 333, 335, 337, 340, 345, 348, 351, 355, 0, 357, 374, 389, 388, 0,
3, 0, 5, 0, 22, 0, 0, 0, 12, 28, 30, 32, 85, 0, 91, 92, 0, 84, 86, 0, 112, 39, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 404, 234, 236, 0, 242, 238, 0, 240, 223,
224, 225, 0, 226, 227, 228, 0, 304, 26, 13, 97, 166, 182, 0, 0, 162, 160, 0, 0,
129, 0, 139, 0, 0, 0, 76, 0, 0, 282, 297, 0, 296, 183, 0, 0, 295, 0, 202, 174, 0,
100, 102, 160, 0, 208, 200, 209, 68, 0, 88, 0, 87, 0, 178, 0, 180, 67, 0, 0, 365,
0, 321, 355, 0, 322, 323, 0, 0, 0, 0, 0, 0, 0, 358, 359, 0, 0, 0, 0, 361, 362,
360, 145, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 309, 310, 311,
312, 313, 314, 315, 316, 317, 318, 308, 0, 363, 0, 382, 383, 0, 0, 0, 392, 0, 0,
146, 18, 0, 20, 0, 96, 98, 185, 184, 14, 0, 81, 93, 0, 0, 104, 105, 107, 0, 111,
86, 120, 0, 0, 0, 0, 0, 0, 0, 274, 275, 276, 0, 0, 278, 0, 231, 232, 0, 0, 0, 0,
243, 239, 97, 241, 237, 235, 0, 167, 0, 0, 0, 173, 161, 168, 131, 0, 0, 0, 162,
133, 135, 134, 130, 152, 141, 140, 0, 48, 47, 0, 45, 80, 326, 78, 74, 290, 0, 23,
291, 0, 0, 0, 0, 195, 299, 302, 0, 301, 0, 0, 0, 203, 101, 103, 0, 162, 0, 212,
0, 210, 160, 0, 0, 222, 205, 207, 177, 0, 0, 0, 186, 190, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 391, 398, 0, 397, 328, 0, 330, 332, 334, 336, 338, 339, 343, 344,
341, 342, 346, 347, 349, 350, 352, 353, 354, 307, 380, 381, 376, 0, 378, 379, 0,
0, 0, 4, 6, 0, 108, 99, 0, 0, 0, 113, 122, 121, 0, 0, 116, 0, 0, 0, 0, 0, 0, 0,
0, 273, 277, 0, 0, 0, 230, 0, 233, 0, 15, 305, 160, 0, 170, 0, 0, 0, 163, 132,
166, 154, 153, 142, 7, 0, 0, 0, 298, 198, 0, 300, 194, 196, 292, 0, 289, 204,
175, 217, 0, 211, 214, 0, 162, 0, 213, 0, 364, 0, 0, 187, 0, 191, 386, 387, 0, 0,
0, 0, 0, 0, 0, 0, 356, 394, 0, 0, 377, 375, 395, 0, 0, 83, 106, 109, 82, 110,
123, 120, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 249, 279, 0, 0, 16, 162, 0, 171,
169, 0, 162, 0, 0, 0, 0, 44, 46, 197, 303, 216, 219, 0, 221, 220, 215, 0, 0, 0,
188, 192, 0, 0, 324, 325, 0, 368, 396, 0, 399, 320, 393, 0, 119, 118, 244, 0,
249, 259, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 280, 0, 156, 172, 164, 155, 160, 0,
49, 50, 218, 366, 367, 0, 193, 0, 0, 384, 0, 0, 0, 255, 0, 0, 261, 0, 0, 0, 269,
0, 0, 0, 0, 0, 0, 0, 0, 248, 229, 0, 162, 0, 0, 369, 0, 385, 0, 245, 0, 0, 0,
246, 260, 262, 263, 0, 271, 270, 0, 265, 0, 0, 0, 0, 249, 250, 165, 157, 0, 0, 0,
124, 0, 0, 247, 264, 272, 266, 267, 0, 0, 249, 251, 0, 189, 370, 255, 256, 268,
249, 252, 51, 255, 257, 249, 253, 258, 254, 0, 0, 0}; static short Cyc_yydefgoto[
112u]={ 801, 40, 41, 42, 243, 43, 374, 44, 376, 45, 210, 46, 47, 63, 64, 443,
444, 48, 151, 49, 50, 129, 130, 51, 87, 52, 216, 217, 98, 99, 100, 218, 146, 386,
387, 388, 53, 54, 542, 543, 544, 55, 56, 57, 58, 123, 109, 439, 468, 59, 469,
430, 570, 422, 426, 427, 287, 264, 157, 78, 79, 485, 379, 486, 487, 456, 457,
147, 152, 153, 470, 294, 246, 247, 248, 249, 250, 251, 702, 753, 252, 253, 277,
278, 279, 459, 460, 461, 254, 255, 361, 184, 447, 185, 186, 187, 188, 189, 190,
191, 192, 193, 194, 195, 196, 197, 198, 199, 500, 501, 200, 201}; static short
Cyc_yypact[ 804u]={ 1890, - -32768, - -32768, - -32768, - -32768, - 60, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 39, 275, 514, -
-32768, 266, - 13, - -32768, - -32768, 47, - -32768, 153, - -32768, 144, 417, -
-32768, 112, 168, 183, - -32768, - -32768, 1778, - -32768, 88, 207, - -32768,
133, 39, 39, 39, - -32768, - -32768, 246, - -32768, 131, 2435, 152, 72, 41, -
-32768, 266, 221, 237, 2491, - -32768, 266, 238, - -32768, 289, - -32768, -
-32768, - -32768, 606, - -32768, 242, 268, 314, 606, 278, 354, 316, 313, -
-32768, - -32768, 4282, 128, 4282, - -32768, 398, 1375, 3906, 3906, - -32768,
1778, 2001, 1778, 2001, - -32768, 393, 409, - -32768, 2376, 2491, 2491, 2491,
144, 1375, 144, - -32768, 266, - -32768, 356, 424, 1195, - -32768, 2435, 133, -
-32768, 1719, 4282, 756, - -32768, 152, 39, - -32768, - -32768, 2001, 416, -
-32768, 438, 420, 443, 266, 444, 606, - -32768, - -32768, - -32768, - -32768,
461, 3906, 379, 451, 303, 39, 39, 142, - -32768, 57, - -32768, - -32768, -
-32768, 439, 10, - -32768, 558, 126, - -32768, 4059, 458, - -32768, 3906, 3949,
463, 465, 470, 477, 481, 144, 4126, 4126, - -32768, - -32768, - -32768, - -32768,
1544, 485, 4169, 4169, - -32768, 4169, - -32768, - -32768, 492, - -32768, - 10,
526, 486, 488, 496, 421, 36, 460, 473, 182, - -32768, 931, 4169, 386, - -32768,
- -32768, 107, 546, - -32768, 549, - -32768, 552, - -32768, 417, 2901, 2435, -
-32768, - -32768, - -32768, - -32768, 529, 565, 1375, - -32768, 245, 531, 144,
266, 569, - -32768, 568, 60, 574, 2522, 582, 598, 593, 599, 2968, 2522, - 8,
2522, 2522, - 26, 591, - -32768, - -32768, 600, 1313, 1313, 133, 1313, - -32768,
- -32768, - -32768, 605, - -32768, - -32768, - -32768, 320, - -32768, - -32768,
- -32768, 609, 639, - -32768, 70, 630, 625, 341, 633, 26, - -32768, 627, 62, 637,
37, 266, - -32768, 3906, 646, - -32768, - -32768, 644, 645, - -32768, 350, 793,
606, 4282, - -32768, 649, 650, 1375, 1375, 2267, 3035, 17, - -32768, 225, -
-32768, 70, - -32768, 4282, - -32768, 2163, - -32768, 482, - -32768, - -32768,
1375, 1544, - -32768, 1375, - -32768, - -32768, 2589, - -32768, 679, 3906, 2112,
726, 3906, 3906, 654, 1544, - -32768, - -32768, 1313, 655, 507, 3906, - -32768,
- -32768, - -32768, - -32768, 4169, 3906, 4169, 4169, 4169, 4169, 4169, 4169,
4169, 4169, 4169, 4169, 4169, 4169, 4169, 4169, 4169, 4169, 4169, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, 3906, - -32768, 229, - -32768, - -32768, 3102, 235, 3906, -
-32768, 2823, 656, - -32768, - -32768, 1778, - -32768, 1778, - -32768, - -32768,
- -32768, - -32768, - -32768, 1375, - -32768, - -32768, 3906, 659, 657, - -32768,
658, 1375, - -32768, 144, 603, 3906, 662, 3906, 3906, 732, 1431, 671, - -32768,
- -32768, - -32768, 363, 724, - -32768, 3169, - -32768, - -32768, 2522, 677,
2522, 1657, - -32768, - -32768, 2376, - -32768, - -32768, - -32768, 3906, -
-32768, 1375, 670, 368, - -32768, - -32768, 660, - -32768, 680, 675, 2052, 625,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 62, 1778, - -32768,
678, 681, 669, - -32768, - -32768, - -32768, - -32768, - -32768, 606, - -32768,
- -32768, 692, 3906, 606, 178, - -32768, - -32768, - -32768, 682, 683, 685, 24,
695, - -32768, - -32768, - -32768, 687, 625, 688, - -32768, 697, 225, 1941, 4282,
3236, - -32768, - -32768, 439, - -32768, 689, 690, 704, - -32768, - -32768, 111,
2901, 519, 694, 417, 693, 522, 700, 1375, 696, 707, 4016, - -32768, - -32768,
709, 711, 526, 212, 486, 488, 496, 421, 36, 36, 460, 460, 460, 460, 473, 473,
182, 182, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
716, - -32768, - -32768, 77, 286, 4282, - -32768, - -32768, 721, - -32768, -
-32768, 245, 3906, 725, - -32768, - -32768, - -32768, 727, 266, 375, 530, 3906,
533, 537, 722, 3303, 3370, 376, - -32768, - -32768, 729, 728, 730, - -32768, 731,
- -32768, 2435, - -32768, - -32768, 734, 4282, - -32768, 738, 70, 723, - -32768,
- -32768, 210, - -32768, - -32768, - -32768, - -32768, 508, 739, 37, - -32768, -
-32768, 717, - -32768, - -32768, - -32768, - -32768, 2204, - -32768, - -32768, -
-32768, - -32768, 741, - -32768, - -32768, 747, 625, 134, - -32768, 740, 742,
528, 746, - -32768, 2667, - -32768, - -32768, - -32768, 2435, 1375, 3949, 825,
754, 742, 757, 2823, - -32768, - -32768, 3906, 3906, - -32768, - -32768, -
-32768, 2823, 419, - -32768, - -32768, - -32768, - -32768, - -32768, 184, 91,
603, 2522, 539, 760, 2522, 3906, 3437, 388, 3504, 391, 3571, 618, - -32768, 763,
774, - -32768, 625, 68, - -32768, - -32768, 770, 625, 1375, 761, 764, 767, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, 768, - -32768, -
-32768, - -32768, 772, 773, 3906, - -32768, - -32768, 2901, 775, - -32768, 679,
779, - -32768, - -32768, 435, - -32768, - -32768, - -32768, 782, - -32768, -
-32768, 834, 778, 618, - -32768, 551, 2522, 554, 3638, 2522, 560, 3705, 3772,
400, 606, 780, 787, - -32768, 2522, - -32768, - -32768, 786, - -32768, 734, 801,
- -32768, - -32768, - -32768, - -32768, - -32768, 297, - -32768, 789, 1375, -
-32768, 2745, 1375, 2522, 648, 794, 798, - -32768, 2522, 2522, 575, - -32768,
2522, 2522, 586, 2522, 590, 3839, - 39, 1313, - -32768, - -32768, 680, 625, 796,
3906, - -32768, 799, - -32768, 610, - -32768, 3906, 788, 805, - -32768, - -32768,
- -32768, - -32768, 2522, - -32768, - -32768, 2522, - -32768, 2522, 2522, 614,
3906, 935, - -32768, - -32768, - -32768, 811, 441, 804, - -32768, 304, 1313, -
-32768, - -32768, - -32768, - -32768, - -32768, 2522, 340, 618, - -32768, 806, -
-32768, - -32768, 1073, - -32768, - -32768, 935, - -32768, - -32768, 648, -
-32768, 618, - -32768, - -32768, - -32768, 904, 908, - -32768}; static short Cyc_yypgoto[
112u]={ - -32768, - 1, - -32768, 595, - -32768, - -32768, - -32768, - -32768, -
-32768, - 51, - 52, - 55, - -32768, 298, - 40, 349, - -32768, 117, - -32768, 90,
- -32768, - -32768, - 108, - -32768, - 68, - -32768, - 124, - -32768, - -32768,
- -32768, 769, 712, - 19, - -32768, - -32768, 394, - -32768, - -32768, 46, -
-32768, - -32768, - 4, - 29, 29, - -32768, - -32768, 873, - -32768, - 97, - 32,
- 112, - 391, 190, 361, 366, 640, - 355, - 88, - 287, 817, - -32768, - 177, -
157, - 335, - 265, - -32768, 525, - 152, - 70, - 94, - 38, - 218, 69, - -32768,
- -32768, - 48, - 234, - -32768, - 651, - 291, - -32768, - -32768, - 21, 699, -
-32768, 407, - -32768, - -32768, - 120, 58, - -32768, - 146, - 356, - 143, 665,
604, 666, 664, 698, 380, 230, 384, 377, - 59, 578, - -32768, - -32768, - -32768,
- 322, - -32768, - -32768, 11}; static short Cyc_yytable[ 4385u]={ 116, 156, 80,
115, 114, 312, 263, 117, 128, 414, 415, 60, 417, 141, 148, 309, 121, 455, 480,
313, 281, 262, 110, 324, 275, 266, 110, 61, 122, 534, 88, 766, 378, 66, 81, 529,
82, 215, 725, 220, 92, 573, 101, 223, 525, 60, 116, 212, 213, 214, 114, 133, 60,
211, 441, 325, 137, 60, 245, 410, 116, 331, 244, 107, 256, 111, 767, 257, 60,
567, 155, 125, 83, 1, 473, 297, 131, 411, 592, 16, 17, 18, 121, 81, 473, 557,
406, 219, 81, 496, 407, 434, 203, 204, 205, 206, 540, 541, 582, 155, 319, 332,
339, 340, 60, 60, 60, 60, 293, 590, 298, 286, 258, 404, 299, 290, 785, 303, 327,
328, 221, 329, 442, 589, 291, 270, 298, 60, 446, 423, 435, 31, 62, 793, 1, 564,
296, 60, 362, 341, 342, 798, 108, 131, 480, 81, 800, 395, 183, 202, 84, 301, 432,
391, 481, 116, 482, 30, 396, 256, 31, 295, 381, 32, 445, 424, 60, 706, 495, 296,
34, 37, 38, 39, 145, 292, 145, 420, 268, 145, 296, 627, 93, 94, 302, 621, 325, -
117, 245, 245, 369, 245, 244, 244, 488, 244, 145, 492, 219, 665, 325, 144, 370,
144, 258, 664, 144, 603, 145, 145, 89, 604, 503, 149, 463, 389, 30, 371, 30, 60,
150, 31, 32, 144, 32, 304, 305, 97, 479, 425, 60, 34, 108, 392, 298, 144, 144,
85, 666, 446, 290, 416, 37, 38, 39, 62, 1, 86, 528, 291, 118, 37, 38, 39, 477,
119, 60, 705, 533, 120, 490, 458, 708, 90, 425, 145, 538, 380, 245, 466, 467,
155, 244, 122, 545, 584, 547, 548, 91, 552, 680, 155, - 86, 131, 453, 454, 155,
86, 518, 519, 520, 347, 144, 81, 81, 421, 88, 398, 438, 348, 349, 95, 96, 405,
261, 408, 409, 145, 446, - 160, 605, 60, 420, 60, 522, - 160, 126, 619, 62, 292,
526, 523, 455, 474, 539, 156, 60, 527, 475, 302, 30, 30, 476, 132, 144, 127, 32,
32, 105, 673, 134, 106, 612, 34, 116, 102, 103, 104, 551, 472, 385, 30, 770, 135,
37, 38, 39, 32, 673, 437, 30, 116, 596, 219, 561, 114, 32, 709, 562, 380, 65,
219, 138, 531, 145, 532, 493, 595, 145, 145, 145, 597, 622, - 182, 499, 60, 623,
60, 145, - 182, 145, 446, 286, 31, 155, 145, 145, 420, 145, 285, 136, 144, 745,
155, 420, 144, 144, 144, - 86, 789, 145, - 86, 155, 140, 419, 144, 86, 144, 1,
521, 420, 269, 144, 144, 499, 144, 634, 672, 380, 580, 639, 641, 286, 477, 583,
261, 624, 144, 616, 576, 420, 431, 288, 289, 451, 792, 672, 363, 364, 365, 420,
60, 648, 675, 139, 155, 673, 554, 31, 86, 653, 81, 222, 420, 566, 499, 81, 676,
575, 631, 642, 499, 145, 682, 282, 632, 420, 283, 563, 558, 145, 560, 694, 1,
366, 697, 608, 207, 420, 337, 338, 420, 367, 368, 737, 649, 717, 154, 796, 144,
420, 60, 30, 145, 799, 768, 116, 144, 32, 208, 256, 224, 145, 647, 271, 34, 273,
691, 693, 298, 696, 145, 699, 683, 343, 344, 37, 38, 39, 574, 144, 720, 389, 784,
599, 721, 425, 787, 272, 144, 274, 420, 790, 276, 672, 380, 280, 60, 144, 284,
716, 296, 67, 116, 630, 795, 308, 114, 797, 1, 211, 314, 684, 315, 145, 145, 458,
30, 316, 510, 511, 512, 513, 32, 730, 317, - 208, 734, 736, 318, 300, - 208, -
208, 326, 145, 345, 346, - 208, 330, 291, 674, 144, 144, 655, 656, 657, 333, 743,
81, 68, 69, 334, 70, 71, 335, 32, 498, 420, 540, 541, 336, 144, 72, 668, 669, 73,
74, 765, 606, 420, 145, 610, 420, 75, 382, 772, 390, 76, 77, 633, 420, 775, 635,
420, 749, 155, 636, 420, 688, 420, 700, 701, 372, 30, 67, 373, 783, 144, 375, 32,
726, 420, - 179, 728, 420, 145, 300, - 179, - 179, 732, 420, 383, 380, - 179,
393, 291, 394, 37, 38, 39, 751, 752, 397, 380, 758, 420, 681, 685, 686, 738, 399,
380, 144, 400, 245, 761, 420, 401, 244, 763, 420, 30, 69, 402, 70, 71, 412, 32,
413, 145, 747, 418, 687, 155, 72, 690, 209, 73, 74, 774, 305, 81, 245, 782, 420,
75, 244, 508, 509, 76, 77, 245, 516, 517, 421, 244, 144, 514, 515, 428, 429, 380,
433, 440, 245, 436, 307, 245, 244, 310, 310, 244, 448, 449, 145, 450, 465, 321,
322, 464, 331, 491, 494, 535, 497, 310, 310, 536, 310, 530, 546, 727, 549, 537,
731, 553, 555, 559, 565, 579, 569, 144, 568, 741, 571, 310, 577, 581, 586, 380,
578, 158, 159, 587, 588, 31, 591, 593, 600, 602, 601, 609, 750, 607, 160, 613,
161, 756, 757, 162, 611, 759, 760, 614, 762, 594, 163, 164, 617, 145, 165, 618,
145, 166, 167, 620, 168, 625, 169, 170, 637, 628, 644, 629, 643, 660, 652, 778,
261, 645, 779, 67, 780, 781, 650, 646, 144, 615, 658, 144, 662, 30, 171, 172,
173, 174, 663, 32, 667, 670, 677, 791, 310, 678, 175, 689, 679, 703, 176, 704,
707, 710, 723, 711, 267, 177, 712, 713, 178, 179, 180, 714, 715, 724, 718, 181,
182, 719, 30, 69, 722, 70, 71, 740, 32, 739, 742, 744, 746, 452, 754, 72, 755,
776, 73, 74, 771, 786, 773, 453, 454, 777, 75, 788, 802, 794, 76, 77, 803, 310,
489, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310,
310, 310, 659, 384, 626, 124, 769, 654, 651, 265, 478, 504, 2, 3, 4, 112, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 700, 701, 21, 158, 159, 225,
310, 226, 227, 228, 229, 230, 231, 232, 233, 22, 160, 23, 161, 234, 377, 162, 24,
235, 585, 462, 27, 28, 163, 164, 236, 237, 165, 29, 238, 166, 167, 661, 168, 502,
169, 170, 506, 505, 0, 0, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 0, 0,
0, 0, 0, 0, 0, 239, 171, 172, 173, 174, 31, 32, 33, 360, 240, 113, 0, 310, 175,
507, 0, 35, 242, 0, 0, 0, 0, 0, 0, 177, 0, 0, 178, 179, 180, 0, 0, 0, 0, 181,
182, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 310, 2, 3, 4, 112,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 751, 752, 21, 158, 159,
225, 0, 226, 227, 228, 229, 230, 231, 232, 233, 22, 160, 23, 161, 234, 310, 162,
24, 235, 0, 0, 27, 28, 163, 164, 236, 237, 165, 29, 238, 166, 167, 0, 168, 0,
169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 239,
171, 172, 173, 174, 31, 32, 33, 0, 240, 113, 0, 0, 175, 0, 0, 35, 242, 0, 0, 0,
0, 0, 0, 177, 0, 0, 178, 179, 180, 310, 0, 0, 0, 181, 182, 0, 0, 0, 310, 2, 3, 4,
112, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 158, 159,
225, 0, 226, 227, 228, 229, 230, 231, 232, 233, 22, 160, 23, 161, 234, 0, 162,
24, 235, 0, 0, 27, 28, 163, 164, 236, 237, 165, 29, 238, 166, 167, 0, 168, 0,
169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 239,
171, 172, 173, 174, 31, 32, 33, 0, 240, 113, 241, 0, 175, 0, 0, 35, 242, 0, 0, 0,
0, 0, 0, 177, 0, 0, 178, 179, 180, 0, 0, 0, 0, 181, 182, 2, 3, 4, 112, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 158, 159, 225, 0, 226,
227, 228, 229, 230, 231, 232, 233, 22, 160, 23, 161, 234, 0, 162, 24, 235, 0, 0,
27, 28, 163, 164, 236, 237, 165, 29, 238, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0,
0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 239, 171, 172,
173, 174, 31, 32, 33, 0, 240, 113, 0, 0, 175, 22, 0, 35, 242, 0, 0, 0, 0, 0, 0,
177, 27, 28, 178, 179, 180, 0, 0, 29, 0, 181, 182, 2, 3, 4, 112, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 158, 159, 0, 0, 0, 0, 0, 0, 0,
31, 0, 33, 22, 160, 23, 161, 0, 0, 162, 24, 35, 36, 0, 27, 28, 163, 164, 0, 0,
165, 29, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 31, 32, 33, 0, 550, 0, 0, 0, 175,
0, 0, 35, 242, 0, 0, 0, 0, 0, 0, 177, 0, 0, 178, 179, 180, 0, 0, 0, 0, 181, 182,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 158, 159, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 22, 160, 0, 161, 0, 0, 162, 0, 0, 0, 0, 27, 28, 163, 164, 0,
0, 165, 29, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 31, 32, 33, 0, 0, 323, 0, 0,
175, 0, 0, 35, 242, 0, 0, 0, 0, 0, 0, 177, 0, 0, 178, 179, 180, 0, 0, 0, 0, 181,
182, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 158, 159, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 160, 0, 161, 0, 0, 162, 0, 0, 0, 0, 27, 28, 163,
164, 0, 0, 165, 29, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 30, 171, 172, 173, 174, 31, 32,
33, 0, 0, 0, 0, 0, 175, 22, 0, 35, 242, 0, 0, 0, 0, 0, 0, 177, 27, 28, 178, 179,
180, 0, 0, 29, 0, 181, 182, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 259, 21, 0, 260, 0, 0, 0, 0, 31, 0, 33, 0, 261, 0,
22, 0, 23, 0, 0, 35, 36, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0,
0, 0, 31, 32, 33, 0, 0, 0, - 8, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38,
39, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0,
25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33,
0, 0, 0, 0, 0, 34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4,
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 259, 0, 21, 0, 0, 0,
0, 0, 31, 0, 33, 0, 261, 0, 0, 22, 0, 23, 0, 35, 36, 0, 24, 0, 25, 26, 27, 28, 0,
0, 0, 0, 0, 29, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, - 8, 0,
34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4, 112, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 572, 0, 21, 0, 0, 0, 0, 0, 31, 0,
33, 0, 0, 0, 0, 22, 0, 0, 0, 35, 36, 0, 24, 0, 0, 0, 27, 28, 0, 0, 1, 0, 0, 29,
0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28, 35, 36,
0, 0, 0, 29, 0, 0, 37, 38, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 67, 0, 0, 259, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 261, 0, 0, 0, 300, - 160,
0, 35, 36, 0, 0, - 160, 0, 291, 0, 37, 38, 39, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 30, 69, 0, 70, 71, 0, 32, 0, 0, 0, 0, 0, 0, 72, 0, 22, 73,
74, 0, 0, 0, 453, 454, 0, 75, 0, 27, 28, 76, 77, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 259, 0, 0, 0, 0, 0,
0, 0, 31, 0, 33, 0, 261, 0, 0, 0, 290, 0, 0, 35, 36, 0, 0, 0, 0, 291, 0, 37, 38,
39, 2, 3, 4, 112, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 0, 0, 27,
28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 2, 3, 4, 112, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0, 113,
22, 209, 23, 0, 0, 35, 36, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 2, 3,
4, 112, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0,
0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 22, 0, 113, 0, 0, 0, 0, 24, 35, 36, 0, 27, 28, 0,
0, 0, 0, 0, 29, 158, 159, 225, 0, 226, 227, 228, 229, 230, 231, 232, 233, 0, 160,
0, 161, 234, 0, 162, 0, 235, 0, 0, 0, 0, 163, 164, 236, 237, 165, 0, 238, 166,
167, 31, 168, 33, 169, 170, 0, 0, 0, 0, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 239, 171, 172, 173, 174, 0, 32, 158, 159, 240, 113, 0, 0, 175, 483,
0, 0, 176, 0, 0, 160, 0, 161, 0, 177, 162, 0, 178, 179, 180, 0, 0, 163, 164, 181,
182, 165, 0, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 0, 0, 0, 311, 484,
0, 175, 0, 0, 0, 176, 158, 159, 0, 453, 454, 0, 177, 0, 0, 178, 179, 180, 0, 160,
0, 161, 181, 182, 162, 0, 0, 0, 0, 0, 0, 163, 164, 0, 0, 165, 0, 0, 166, 167, 0,
168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 171, 172, 173, 174, 0, 32, 0, 0, 0, 311, 671, 0, 175, 0, 0, 0, 176, 158, 159,
0, 453, 454, 0, 177, 0, 0, 178, 179, 180, 0, 160, 0, 161, 181, 182, 162, 0, 0, 0,
0, 0, 0, 163, 164, 0, 0, 165, 0, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 0,
0, 0, 311, 748, 0, 175, 0, 0, 0, 176, 158, 159, 0, 453, 454, 0, 177, 0, 0, 178,
179, 180, 0, 160, 0, 161, 181, 182, 162, 0, 0, 0, 0, 0, 0, 163, 164, 0, 0, 165,
0, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 0, 0, 0, 311, 0, 0, 175, 0, 0,
0, 176, 158, 159, 0, 453, 454, 0, 177, 0, 0, 178, 179, 180, 0, 160, 0, 161, 181,
182, 162, 0, 0, 0, 0, 0, 0, 163, 164, 0, 0, 165, 0, 0, 166, 167, 0, 168, 0, 169,
170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172,
173, 174, 0, 32, 158, 159, 0, 311, 0, 0, 175, 0, 0, 0, 176, 0, 0, 160, 0, 161, 0,
177, 162, 0, 178, 179, 180, 0, 0, 163, 164, 181, 182, 165, 0, 0, 166, 167, 0,
168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 171, 172, 173, 174, 0, 32, 158, 159, 403, 0, 0, 0, 175, 0, 0, 0, 176, 0, 0,
160, 0, 161, 0, 177, 162, 0, 178, 179, 180, 0, 0, 163, 164, 181, 182, 165, 0, 0,
166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 158, 159, 0, 0, 0, 0, 175, 0, 0, 0,
176, 0, 0, 160, 0, 161, 471, 177, 162, 0, 178, 179, 180, 0, 0, 163, 164, 181,
182, 165, 0, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 158, 159, 0, 0, 0,
0, 175, 524, 0, 0, 176, 0, 0, 160, 0, 161, 0, 177, 162, 0, 178, 179, 180, 0, 0,
163, 164, 181, 182, 165, 0, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 158,
159, 0, 0, 0, 0, 175, 556, 0, 0, 176, 0, 0, 160, 0, 161, 0, 177, 162, 0, 178,
179, 180, 0, 0, 163, 164, 181, 182, 165, 0, 0, 166, 167, 0, 168, 0, 169, 170, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173,
174, 0, 32, 158, 159, 0, 0, 0, 0, 175, 0, 0, 0, 176, 0, 0, 160, 0, 161, 598, 177,
162, 0, 178, 179, 180, 0, 0, 163, 164, 181, 182, 165, 0, 0, 166, 167, 0, 168, 0,
169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171,
172, 173, 174, 0, 32, 158, 159, 638, 0, 0, 0, 175, 0, 0, 0, 176, 0, 0, 160, 0,
161, 0, 177, 162, 0, 178, 179, 180, 0, 0, 163, 164, 181, 182, 165, 0, 0, 166,
167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 158, 159, 640, 0, 0, 0, 175, 0, 0, 0,
176, 0, 0, 160, 0, 161, 0, 177, 162, 0, 178, 179, 180, 0, 0, 163, 164, 181, 182,
165, 0, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 158, 159, 0, 0, 0, 0, 175,
692, 0, 0, 176, 0, 0, 160, 0, 161, 0, 177, 162, 0, 178, 179, 180, 0, 0, 163, 164,
181, 182, 165, 0, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 158, 159, 0, 0,
0, 0, 175, 695, 0, 0, 176, 0, 0, 160, 0, 161, 0, 177, 162, 0, 178, 179, 180, 0,
0, 163, 164, 181, 182, 165, 0, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32,
158, 159, 698, 0, 0, 0, 175, 0, 0, 0, 176, 0, 0, 160, 0, 161, 0, 177, 162, 0,
178, 179, 180, 0, 0, 163, 164, 181, 182, 165, 0, 0, 166, 167, 0, 168, 0, 169,
170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172,
173, 174, 0, 32, 158, 159, 0, 0, 0, 0, 175, 729, 0, 0, 176, 0, 0, 160, 0, 161, 0,
177, 162, 0, 178, 179, 180, 0, 0, 163, 164, 181, 182, 165, 0, 0, 166, 167, 0,
168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 171, 172, 173, 174, 0, 32, 158, 159, 0, 0, 0, 0, 175, 733, 0, 0, 176, 0, 0,
160, 0, 161, 0, 177, 162, 0, 178, 179, 180, 0, 0, 163, 164, 181, 182, 165, 0, 0,
166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 158, 159, 0, 0, 0, 0, 175, 735, 0, 0,
176, 0, 0, 160, 0, 161, 0, 177, 162, 0, 178, 179, 180, 0, 0, 163, 164, 181, 182,
165, 0, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 158, 159, 0, 0, 0, 0, 175,
764, 0, 0, 176, 0, 0, 160, 0, 161, 0, 177, 162, 0, 178, 179, 180, 0, 0, 163, 164,
181, 182, 165, 0, 0, 166, 167, 0, 168, 0, 169, 170, 0, 0, 0, 0, 158, 159, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32, 0, 163,
164, 0, 0, 165, 175, 0, 0, 167, 176, 168, 0, 169, 170, 0, 0, 177, 0, 0, 178, 179,
180, 0, 0, 0, 0, 181, 182, 0, 0, 0, 0, 0, 0, 0, 30, 171, 172, 173, 174, 0, 32,
158, 159, 0, 311, 0, 0, 175, 0, 0, 0, 176, 0, 0, 160, 0, 0, 0, 177, 0, 0, 178,
179, 180, 0, 0, 163, 164, 181, 182, 165, 0, 0, 0, 167, 0, 168, 0, 169, 170, 0, 0,
0, 0, 158, 159, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0, 30, 171, 172, 173,
174, 0, 32, 0, 163, 164, 615, 0, 165, 175, 0, 0, 167, 176, 168, 0, 169, 170, 0,
0, 177, 0, 0, 178, 179, 180, 0, 0, 0, 0, 181, 182, 0, 0, 0, 0, 0, 0, 0, 30, 171,
172, 173, 174, 0, 32, 158, 159, 0, 0, 0, 0, 306, 0, 0, 0, 176, 0, 0, 160, 0, 0,
0, 177, 0, 0, 178, 179, 180, 0, 0, 163, 164, 181, 182, 165, 0, 0, 0, 167, 0, 168,
0, 169, 170, 0, 0, 0, 0, 158, 159, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 0, 0, 0,
30, 171, 172, 173, 174, 0, 32, 0, 163, 164, 0, 0, 165, 320, 0, 0, 167, 176, 168,
0, 169, 170, 0, 0, 177, 0, 0, 178, 179, 180, 0, 0, 0, 0, 181, 182, 0, 0, 0, 0, 0,
0, 0, 30, 171, 172, 173, 174, 0, 32, 0, 0, 0, 0, 0, 0, 175, 0, 0, 0, 176, 0, 0,
0, 0, 0, 0, 177, 0, 0, 178, 179, 180, 0, 0, 0, 0, 181, 182, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 142, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
31, 0, 33, 0, 0, 143, 0, 0, 0, 0, 0, 35, 36}; static short Cyc_yycheck[ 4385u]={
55, 89, 23, 55, 55, 162, 118, 55, 63, 243, 244, 0, 246, 81, 84, 161, 56, 282,
305, 162, 140, 118, 54, 175, 132, 119, 58, 87, 57, 385, 34, 70, 209, 22, 23, 370,
25, 105, 689, 107, 41, 432, 46, 111, 366, 34, 101, 102, 103, 104, 101, 72, 41,
101, 17, 175, 77, 46, 113, 85, 115, 71, 113, 52, 115, 54, 105, 115, 57, 424, 89,
60, 85, 1, 292, 65, 65, 103, 469, 17, 18, 19, 122, 72, 302, 407, 94, 106, 77,
323, 98, 65, 93, 94, 95, 96, 5, 6, 454, 118, 168, 111, 66, 67, 93, 94, 95, 96,
146, 464, 100, 143, 116, 233, 104, 98, 767, 155, 177, 178, 109, 180, 85, 99, 107,
126, 100, 116, 274, 59, 104, 90, 93, 784, 1, 422, 112, 126, 197, 103, 104, 792,
101, 132, 431, 134, 797, 87, 90, 91, 103, 155, 264, 221, 306, 210, 308, 85, 98,
210, 90, 104, 210, 91, 272, 95, 155, 99, 320, 112, 98, 109, 110, 111, 84, 146,
86, 100, 120, 89, 112, 537, 94, 95, 155, 108, 306, 96, 243, 244, 83, 246, 243,
244, 314, 246, 106, 317, 217, 65, 320, 84, 95, 86, 208, 596, 89, 96, 118, 119,
98, 100, 332, 85, 284, 219, 85, 110, 85, 208, 92, 90, 91, 106, 91, 99, 100, 94,
298, 261, 219, 98, 101, 222, 100, 118, 119, 84, 104, 385, 98, 245, 109, 110, 111,
93, 1, 103, 368, 107, 98, 109, 110, 111, 294, 103, 245, 648, 382, 107, 315, 282,
653, 95, 296, 175, 390, 209, 323, 288, 289, 290, 323, 302, 394, 97, 396, 397, 95,
399, 615, 300, 98, 272, 106, 107, 305, 103, 347, 348, 349, 109, 175, 282, 283,
85, 300, 228, 269, 117, 118, 94, 95, 234, 94, 236, 237, 217, 454, 99, 487, 300,
100, 302, 85, 105, 95, 105, 93, 290, 85, 92, 587, 98, 392, 413, 315, 92, 103,
300, 85, 85, 107, 95, 217, 98, 91, 91, 92, 604, 98, 95, 494, 98, 399, 47, 48, 49,
399, 291, 105, 85, 743, 85, 109, 110, 111, 91, 623, 269, 85, 416, 474, 382, 416,
416, 91, 654, 416, 311, 95, 390, 94, 374, 284, 376, 318, 474, 288, 289, 290, 475,
96, 94, 326, 374, 100, 376, 298, 100, 300, 537, 424, 90, 413, 305, 306, 100, 308,
96, 86, 284, 105, 422, 100, 288, 289, 290, 95, 105, 320, 98, 431, 97, 94, 298,
103, 300, 1, 361, 100, 123, 305, 306, 366, 308, 546, 604, 370, 450, 550, 551,
464, 473, 455, 94, 530, 320, 497, 440, 100, 100, 144, 145, 94, 105, 623, 61, 62,
63, 100, 440, 564, 610, 100, 474, 721, 94, 90, 103, 572, 450, 106, 100, 96, 407,
455, 610, 439, 94, 94, 413, 382, 619, 95, 100, 100, 98, 420, 410, 390, 412, 94,
1, 98, 94, 490, 94, 100, 68, 69, 100, 106, 107, 94, 565, 673, 99, 789, 382, 100,
490, 85, 413, 795, 739, 561, 390, 91, 100, 561, 87, 422, 561, 98, 98, 96, 637,
638, 100, 640, 431, 642, 104, 64, 65, 109, 110, 111, 439, 413, 96, 536, 767, 476,
100, 568, 96, 100, 422, 97, 100, 776, 99, 721, 487, 85, 536, 431, 98, 670, 112,
38, 608, 543, 789, 98, 608, 792, 1, 608, 98, 630, 98, 474, 475, 587, 85, 98, 339,
340, 341, 342, 91, 694, 98, 94, 697, 698, 98, 98, 99, 100, 98, 494, 112, 113,
105, 96, 107, 609, 474, 475, 85, 86, 87, 70, 709, 587, 85, 86, 115, 88, 89, 116,
91, 99, 100, 5, 6, 114, 494, 98, 85, 86, 101, 102, 737, 99, 100, 530, 99, 100,
109, 95, 745, 95, 113, 114, 99, 100, 751, 99, 100, 722, 654, 99, 100, 99, 100,
22, 23, 96, 85, 38, 96, 766, 530, 96, 91, 99, 100, 94, 99, 100, 565, 98, 99, 100,
99, 100, 96, 604, 105, 95, 107, 98, 109, 110, 111, 22, 23, 98, 615, 99, 100, 618,
631, 632, 700, 98, 623, 565, 85, 739, 99, 100, 94, 739, 99, 100, 85, 86, 94, 88,
89, 105, 91, 98, 609, 719, 96, 633, 722, 98, 636, 97, 101, 102, 99, 100, 700,
767, 99, 100, 109, 767, 337, 338, 113, 114, 776, 345, 346, 85, 776, 609, 343,
344, 99, 105, 673, 99, 96, 789, 108, 158, 792, 789, 161, 162, 792, 96, 99, 654,
100, 96, 169, 170, 100, 71, 25, 98, 94, 99, 177, 178, 100, 180, 103, 98, 692, 30,
105, 695, 94, 42, 90, 98, 100, 90, 654, 112, 704, 99, 197, 98, 85, 96, 721, 99,
25, 26, 100, 99, 90, 99, 99, 99, 85, 100, 98, 723, 99, 38, 99, 40, 728, 729, 43,
100, 732, 733, 96, 735, 108, 50, 51, 99, 719, 54, 100, 722, 57, 58, 99, 60, 96,
62, 63, 98, 96, 94, 96, 95, 108, 103, 758, 94, 99, 761, 38, 763, 764, 96, 104,
719, 95, 99, 722, 99, 85, 86, 87, 88, 89, 99, 91, 108, 103, 25, 782, 274, 99, 98,
95, 99, 94, 102, 85, 90, 100, 28, 99, 108, 109, 99, 99, 112, 113, 114, 99, 99,
95, 99, 119, 120, 98, 85, 86, 98, 88, 89, 96, 91, 105, 100, 86, 99, 96, 96, 98,
94, 105, 101, 102, 100, 86, 99, 106, 107, 96, 109, 99, 0, 99, 113, 114, 0, 331,
315, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347,
348, 349, 579, 217, 536, 58, 742, 572, 568, 118, 296, 333, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 385, 29,
30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 208, 43, 44, 45, 456, 283, 48,
49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 587, 60, 331, 62, 63, 335, 334, - 1, - 1,
72, 73, 74, 75, 76, 77, 78, 79, 80, 81, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85,
86, 87, 88, 89, 90, 91, 92, 97, 94, 95, - 1, 454, 98, 336, - 1, 101, 102, - 1, -
1, - 1, - 1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1, - 1, 119,
120, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 497, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
39, 40, 41, 537, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, 90, 91, 92,
- 1, 94, 95, - 1, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, 109,
- 1, - 1, 112, 113, 114, 610, - 1, - 1, - 1, 119, 120, - 1, - 1, - 1, 619, 3, 4,
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25,
26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45,
- 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60, - 1, 62, 63, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, 90, 91, 92, - 1, 94, 95, 96, - 1, 98, -
1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, -
1, - 1, - 1, - 1, 119, 120, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36,
37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55,
56, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 85, 86, 87, 88, 89, 90, 91, 92, - 1, 94,
95, - 1, - 1, 98, 37, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, 109, 48, 49,
112, 113, 114, - 1, - 1, 55, - 1, 119, 120, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 90, - 1, 92, 37, 38, 39, 40, - 1, - 1, 43, 44, 101, 102, - 1, 48, 49, 50,
51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
85, 86, 87, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, - 1, - 1, 101, 102,
- 1, - 1, - 1, - 1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1, - 1,
119, 120, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1,
25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1,
- 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1,
60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, 90, 91, 92, - 1, - 1,
95, - 1, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, 109, - 1, -
1, 112, 113, 114, - 1, - 1, - 1, - 1, 119, 120, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51,
- 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, -
1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 85, 86, 87, 88, 89,
90, 91, 92, - 1, - 1, - 1, - 1, - 1, 98, 37, - 1, 101, 102, - 1, - 1, - 1, - 1,
- 1, - 1, 109, 48, 49, 112, 113, 114, - 1, - 1, 55, - 1, 119, 120, 0, 1, - 1, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 82, 24, -
1, 85, - 1, - 1, - 1, - 1, 90, - 1, 92, - 1, 94, - 1, 37, - 1, 39, - 1, - 1, 101,
102, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, - 1, - 1, 90, 91, 92, -
1, - 1, - 1, 96, - 1, 98, - 1, - 1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, 109,
110, 111, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, -
1, 55, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, 37, - 1, 90,
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
20, 21, 85, 86, - 1, 88, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, 98, - 1, 37,
101, 102, - 1, - 1, - 1, 106, 107, - 1, 109, - 1, 48, 49, 113, 114, - 1, - 1, -
1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 82, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 90, - 1, 92, - 1, 94, - 1, - 1, - 1, 98, - 1, - 1, 101, 102, - 1, -
1, - 1, - 1, 107, - 1, 109, 110, 111, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48, 49,
- 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1,
- 1, - 1, 90, - 1, 92, - 1, - 1, 95, 37, 97, 39, - 1, - 1, 101, 102, 44, - 1, -
1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 90, - 1, 92, 37, - 1, 95, - 1, - 1, - 1, - 1, 44, 101,
102, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, 25, 26, 27, - 1, 29, 30, 31, 32,
33, 34, 35, 36, - 1, 38, - 1, 40, 41, - 1, 43, - 1, 45, - 1, - 1, - 1, - 1, 50,
51, 52, 53, 54, - 1, 56, 57, 58, 90, 60, 92, 62, 63, - 1, - 1, - 1, - 1, - 1, -
1, 101, 102, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85,
86, 87, 88, 89, - 1, 91, 25, 26, 94, 95, - 1, - 1, 98, 32, - 1, - 1, 102, - 1, -
1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54,
- 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88,
89, - 1, 91, - 1, - 1, - 1, 95, 96, - 1, 98, - 1, - 1, - 1, 102, 25, 26, - 1,
106, 107, - 1, 109, - 1, - 1, 112, 113, 114, - 1, 38, - 1, 40, 119, 120, 43, - 1,
- 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, - 1,
62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, - 1, - 1, - 1, 95,
96, - 1, 98, - 1, - 1, - 1, 102, 25, 26, - 1, 106, 107, - 1, 109, - 1, - 1, 112,
113, 114, - 1, 38, - 1, 40, 119, 120, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51,
- 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85,
86, 87, 88, 89, - 1, 91, - 1, - 1, - 1, 95, 96, - 1, 98, - 1, - 1, - 1, 102, 25,
26, - 1, 106, 107, - 1, 109, - 1, - 1, 112, 113, 114, - 1, 38, - 1, 40, 119, 120,
43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1,
60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, - 1, - 1, -
1, 95, - 1, - 1, 98, - 1, - 1, - 1, 102, 25, 26, - 1, 106, 107, - 1, 109, - 1, -
1, 112, 113, 114, - 1, 38, - 1, 40, 119, 120, 43, - 1, - 1, - 1, - 1, - 1, - 1,
50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, 95, - 1, - 1, 98, - 1, - 1, - 1,
102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51,
119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85,
86, 87, 88, 89, - 1, 91, 25, 26, 94, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1,
- 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120,
54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87,
88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1,
38, - 1, 40, 108, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, -
1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88,
89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, 38, -
1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, -
1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, -
1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, 38, - 1, 40,
- 1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57,
58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91,
25, 26, - 1, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, 108,
109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, -
1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26,
94, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43,
- 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, -
1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, 94, - 1,
- 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112,
113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1,
98, 99, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114,
- 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1, - 1, - 1, 98, 99,
- 1, - 1, 102, - 1, - 1, 38, - 1, 40, - 1, 109, 43, - 1, 112, 113, 114, - 1, - 1,
50, 51, 119, 120, 54, - 1, - 1, 57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, 94, - 1, - 1, - 1, 98, - 1, - 1, - 1,
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
57, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1,
91, 25, 26, - 1, - 1, - 1, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, 38, - 1, 40, -
1, 109, 43, - 1, 112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, 57, 58,
- 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, - 1, 50,
51, - 1, - 1, 54, 98, - 1, - 1, 58, 102, 60, - 1, 62, 63, - 1, - 1, 109, - 1, -
1, 112, 113, 114, - 1, - 1, - 1, - 1, 119, 120, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, 95, - 1, - 1, 98, - 1, - 1, - 1,
102, - 1, - 1, 38, - 1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, 50, 51,
119, 120, 54, - 1, - 1, - 1, 58, - 1, 60, - 1, 62, 63, - 1, - 1, - 1, - 1, 25,
26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, 85,
86, 87, 88, 89, - 1, 91, - 1, 50, 51, 95, - 1, 54, 98, - 1, - 1, 58, 102, 60, -
1, 62, 63, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1, - 1, 119, 120,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, 25, 26, - 1, - 1,
- 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, 38, - 1, - 1, - 1, 109, - 1, - 1,
112, 113, 114, - 1, - 1, 50, 51, 119, 120, 54, - 1, - 1, - 1, 58, - 1, 60, - 1,
62, 63, - 1, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 38, - 1, - 1, - 1, 85, 86, 87, 88, 89, - 1, 91, - 1, 50, 51, - 1, - 1,
54, 98, - 1, - 1, 58, 102, 60, - 1, 62, 63, - 1, - 1, 109, - 1, - 1, 112, 113,
114, - 1, - 1, - 1, - 1, 119, 120, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, 86, 87,
88, 89, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1,
- 1, - 1, - 1, - 1, 109, - 1, - 1, 112, 113, 114, - 1, - 1, - 1, - 1, 119, 120,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, 59, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 90, - 1, 92, -
1, - 1, 95, - 1, - 1, - 1, - 1, - 1, 101, 102}; unsigned char Cyc_Yyimpossible[
17u]="\000\000\000\000Yyimpossible"; unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex(); static int
Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static int
Cyc_yynerrs= 0; struct _tuple18{ void* f1; void* f2; } ; struct _tuple19{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int yystate;
int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short* yyss=(
short*)({ unsigned int _temp3212= 10000u; short* _temp3213=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3212));{ unsigned int
_temp3214= _temp3212; unsigned int i; for( i= 0; i <  _temp3214; i ++){
_temp3213[ i]=( short) 0;}}; _temp3213;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3209= 10000u; void** _temp3210=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3209));{ unsigned int _temp3211= _temp3209;
unsigned int i; for( i= 0; i <  _temp3211; i ++){ _temp3210[ i]= Cyc_yylval;}};
_temp3210;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3206= 10000u; struct Cyc_Yyltype* _temp3207=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3206));{
unsigned int _temp3208= _temp3206; unsigned int i; for( i= 0; i <  _temp3208; i
++){ _temp3207[ i]= Cyc_yynewloc();}}; _temp3207;}); int yystacksize= 10000;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:
yyss[ _check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate;
if( yyssp_offset >=  yystacksize -  1){ if( yystacksize >=  10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 804u, yystate)]; if( yyn ==  - 32768){ goto
yydefault;} if( Cyc_yychar ==  - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar >  0? Cyc_yychar <= 
348: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 349u, Cyc_yychar)]:
233;} yyn += yychar1; if(( yyn <  0? 1: yyn >  4384)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4385u, yyn)] !=  yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4385u, yyn)]; if( yyn <  0){
if( yyn ==  - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
==  0){ goto yyerrlab;}} if( yyn ==  803){ return 0;} if( Cyc_yychar !=  0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus !=  0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 804u, yystate)]; if(
yyn ==  0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 406u, yyn)]; if( yylen >  0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset +  1) -  yylen)];} switch(
yyn){ case 1: _LL1400: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1401:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1403=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1403[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1404; _temp1404.tag= Cyc_DeclList_tok; _temp1404.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1404;});
_temp1403;}); break; case 3: _LL1402: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1406=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1406[ 0]=({ struct Cyc_DeclList_tok_struct _temp1407; _temp1407.tag= Cyc_DeclList_tok;
_temp1407.f1=({ struct Cyc_List_List* _temp1408=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1408->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1409=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1409->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1410=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1410[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1411; _temp1411.tag= Cyc_Absyn_Using_d;
_temp1411.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1411.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1411;}); _temp1410;}));
_temp1409->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1409;});
_temp1408->tl= 0; _temp1408;}); _temp1407;}); _temp1406;}); Cyc_Lex_leave_using();
break; case 4: _LL1405: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1413=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1413[ 0]=({ struct Cyc_DeclList_tok_struct _temp1414; _temp1414.tag= Cyc_DeclList_tok;
_temp1414.f1=({ struct Cyc_List_List* _temp1415=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1415->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1416=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1416->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1417=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1417[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1418; _temp1418.tag= Cyc_Absyn_Using_d;
_temp1418.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp1418.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1418;});
_temp1417;})); _temp1416->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1416;}); _temp1415->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1415;}); _temp1414;});
_temp1413;}); break; case 5: _LL1412: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1420=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1420[ 0]=({ struct Cyc_DeclList_tok_struct _temp1421; _temp1421.tag= Cyc_DeclList_tok;
_temp1421.f1=({ struct Cyc_List_List* _temp1422=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1422->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1423=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1423->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1424=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1424[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1425; _temp1425.tag=
Cyc_Absyn_Namespace_d; _temp1425.f1=({ struct _tagged_arr* _temp1426=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1426[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1426;});
_temp1425.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1425;}); _temp1424;})); _temp1423->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1423;}); _temp1422->tl= 0; _temp1422;}); _temp1421;}); _temp1420;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1419: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1428=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1428[ 0]=({ struct Cyc_DeclList_tok_struct _temp1429; _temp1429.tag= Cyc_DeclList_tok;
_temp1429.f1=({ struct Cyc_List_List* _temp1430=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1430->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1431=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1431->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1432=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1432[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1433; _temp1433.tag=
Cyc_Absyn_Namespace_d; _temp1433.f1=({ struct _tagged_arr* _temp1434=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1434[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1434;});
_temp1433.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1433;}); _temp1432;})); _temp1431->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1431;}); _temp1430->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1430;}); _temp1429;});
_temp1428;}); break; case 7: _LL1427: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1436=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1436[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1437; _temp1437.tag= Cyc_DeclList_tok; _temp1437.f1=({ struct Cyc_List_List*
_temp1438=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1438->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1439=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1439->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1440=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1440[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1441; _temp1441.tag= Cyc_Absyn_ExternC_d;
_temp1441.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1441;}); _temp1440;})); _temp1439->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1439;}); _temp1438->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1438;}); _temp1437;});
_temp1436;}); break; case 8: _LL1435: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1443=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1443[ 0]=({ struct Cyc_DeclList_tok_struct _temp1444; _temp1444.tag= Cyc_DeclList_tok;
_temp1444.f1= 0; _temp1444;}); _temp1443;}); break; case 9: _LL1442: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1446=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1446[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1447; _temp1447.tag= Cyc_DeclList_tok; _temp1447.f1=({ struct Cyc_List_List*
_temp1448=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1448->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1449=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1449[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1450; _temp1450.tag= Cyc_Absyn_Fn_d;
_temp1450.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1450;}); _temp1449;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1448->tl=
0; _temp1448;}); _temp1447;}); _temp1446;}); break; case 10: _LL1445: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1451: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1453=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1453[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1454; _temp1454.tag= Cyc_FnDecl_tok; _temp1454.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1454;}); _temp1453;}); break; case 12: _LL1452: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1456=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1456[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1457; _temp1457.tag= Cyc_FnDecl_tok;
_temp1457.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1458=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1458->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1458;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1457;}); _temp1456;}); break; case 13: _LL1455: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1460=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1460[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1461; _temp1461.tag= Cyc_FnDecl_tok;
_temp1461.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1461;}); _temp1460;}); break; case 14: _LL1459: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1463=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1463[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1464; _temp1464.tag= Cyc_FnDecl_tok;
_temp1464.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1465=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1465->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1465;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1464;}); _temp1463;}); break; case 15: _LL1462: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1467=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1467[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1468; _temp1468.tag= Cyc_FnDecl_tok;
_temp1468.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1469=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1469->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1469;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1468;}); _temp1467;}); break; case 16: _LL1466: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1471=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1471[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1472; _temp1472.tag= Cyc_FnDecl_tok;
_temp1472.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1473=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1473->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1473;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1472;}); _temp1471;}); break; case 17: _LL1470: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1474:
Cyc_Lex_leave_using(); break; case 19: _LL1475: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1477=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1477[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1477;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1476: Cyc_Lex_leave_namespace(); break; case 21: _LL1478: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1480=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1480[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1481; _temp1481.tag= Cyc_DeclList_tok; _temp1481.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1481;}); _temp1480;}); break; case 22: _LL1479: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1483=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1483[ 0]=({ struct Cyc_DeclList_tok_struct _temp1484; _temp1484.tag= Cyc_DeclList_tok;
_temp1484.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1484;}); _temp1483;}); break; case 23: _LL1482: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1486=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1486[ 0]=({ struct Cyc_DeclList_tok_struct _temp1487; _temp1487.tag= Cyc_DeclList_tok;
_temp1487.f1=({ struct Cyc_List_List* _temp1488=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1488->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1488->tl=
0; _temp1488;}); _temp1487;}); _temp1486;}); break; case 24: _LL1485: { struct
Cyc_List_List* _temp1490= 0;{ struct Cyc_List_List* _temp1491= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0;
_temp1491 !=  0; _temp1491= _temp1491->tl){ struct _tagged_arr* _temp1492=(
struct _tagged_arr*) _temp1491->hd; struct _tuple1* _temp1493=({ struct _tuple1*
_temp1496=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1496->f1=
Cyc_Absyn_rel_ns_null; _temp1496->f2= _temp1492; _temp1496;}); struct Cyc_Absyn_Vardecl*
_temp1494= Cyc_Absyn_new_vardecl( _temp1493, Cyc_Absyn_wildtyp( 0), 0);
_temp1490=({ struct Cyc_List_List* _temp1495=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1495->hd=( void*) _temp1494; _temp1495->tl=
_temp1490; _temp1495;});}} _temp1490=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1490); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1497=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1497[ 0]=({ struct Cyc_DeclList_tok_struct _temp1498; _temp1498.tag= Cyc_DeclList_tok;
_temp1498.f1=({ struct Cyc_List_List* _temp1499=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1499->hd=( void*) Cyc_Absyn_letv_decl(
_temp1490, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1499->tl= 0; _temp1499;}); _temp1498;});
_temp1497;}); break;} case 25: _LL1489: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1500:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1502=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1502[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1503; _temp1503.tag= Cyc_DeclList_tok; _temp1503.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1503;});
_temp1502;}); break; case 27: _LL1501: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1505=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1505[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1506; _temp1506.tag= Cyc_DeclSpec_tok;
_temp1506.f1=({ struct Cyc_Parse_Declaration_spec* _temp1507=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1507->sc=({ struct
Cyc_Core_Opt* _temp1508=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1508->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1508;});
_temp1507->tq= Cyc_Absyn_empty_tqual(); _temp1507->type_specs= 0; _temp1507->is_inline=
0; _temp1507->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1507;}); _temp1506;});
_temp1505;}); break; case 28: _LL1504: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc !=  0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1510=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1510[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1511; _temp1511.tag= Cyc_DeclSpec_tok; _temp1511.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1512=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1512->sc=({ struct Cyc_Core_Opt* _temp1513=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1513->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1513;});
_temp1512->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1512->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1512->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1512->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1512;}); _temp1511;}); _temp1510;}); break; case 29: _LL1509: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1515=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1515[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1516; _temp1516.tag= Cyc_DeclSpec_tok; _temp1516.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1517=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1517->sc= 0; _temp1517->tq= Cyc_Absyn_empty_tqual(); _temp1517->type_specs=({
struct Cyc_List_List* _temp1518=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1518->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1518->tl= 0;
_temp1518;}); _temp1517->is_inline= 0; _temp1517->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1517;});
_temp1516;}); _temp1515;}); break; case 30: _LL1514: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1520=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1520[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1521; _temp1521.tag= Cyc_DeclSpec_tok;
_temp1521.f1=({ struct Cyc_Parse_Declaration_spec* _temp1522=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1522->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1522->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1522->type_specs=({ struct Cyc_List_List* _temp1523=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1523->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1523->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1523;});
_temp1522->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1522->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1522;}); _temp1521;}); _temp1520;}); break; case 31: _LL1519: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1525=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1525[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1526; _temp1526.tag= Cyc_DeclSpec_tok; _temp1526.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1527=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1527->sc= 0; _temp1527->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1527->type_specs=
0; _temp1527->is_inline= 0; _temp1527->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1527;});
_temp1526;}); _temp1525;}); break; case 32: _LL1524: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1529=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1529[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1530; _temp1530.tag= Cyc_DeclSpec_tok;
_temp1530.f1=({ struct Cyc_Parse_Declaration_spec* _temp1531=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1531->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1531->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1531->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1531->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1531->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1531;}); _temp1530;}); _temp1529;}); break; case 33: _LL1528: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1533=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1533[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1534; _temp1534.tag= Cyc_DeclSpec_tok; _temp1534.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1535=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1535->sc= 0; _temp1535->tq= Cyc_Absyn_empty_tqual(); _temp1535->type_specs=
0; _temp1535->is_inline= 1; _temp1535->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1535;});
_temp1534;}); _temp1533;}); break; case 34: _LL1532: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1537=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1537[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1538; _temp1538.tag= Cyc_DeclSpec_tok;
_temp1538.f1=({ struct Cyc_Parse_Declaration_spec* _temp1539=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1539->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1539->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1539->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1539->is_inline=
1; _temp1539->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1539;}); _temp1538;}); _temp1537;}); break; case 35: _LL1536: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1541=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1541[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1542; _temp1542.tag= Cyc_StorageClass_tok;
_temp1542.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1542;}); _temp1541;});
break; case 36: _LL1540: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1544=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1544[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1545; _temp1545.tag=
Cyc_StorageClass_tok; _temp1545.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1545;}); _temp1544;}); break; case 37: _LL1543: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1547=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1547[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1548; _temp1548.tag=
Cyc_StorageClass_tok; _temp1548.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1548;}); _temp1547;}); break; case 38: _LL1546: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1550=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1550[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1551; _temp1551.tag=
Cyc_StorageClass_tok; _temp1551.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1551;}); _temp1550;}); break; case 39: _LL1549: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1553=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1553[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1554; _temp1554.tag= Cyc_StorageClass_tok;
_temp1554.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1554;}); _temp1553;});
break; case 40: _LL1552: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1556=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1556[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1557; _temp1557.tag=
Cyc_StorageClass_tok; _temp1557.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1557;}); _temp1556;}); break; case 41: _LL1555: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1559=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1559[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1560; _temp1560.tag=
Cyc_StorageClass_tok; _temp1560.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1560;}); _temp1559;}); break; case 42: _LL1558: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1562=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1562[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1563; _temp1563.tag=
Cyc_AttributeList_tok; _temp1563.f1= 0; _temp1563;}); _temp1562;}); break; case
43: _LL1561: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1564: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1566=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1566[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1567; _temp1567.tag=
Cyc_AttributeList_tok; _temp1567.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1567;});
_temp1566;}); break; case 45: _LL1565: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1569=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1569[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1570; _temp1570.tag=
Cyc_AttributeList_tok; _temp1570.f1=({ struct Cyc_List_List* _temp1571=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1571->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1571->tl= 0; _temp1571;}); _temp1570;}); _temp1569;});
break; case 46: _LL1568: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1573=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1573[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1574; _temp1574.tag=
Cyc_AttributeList_tok; _temp1574.f1=({ struct Cyc_List_List* _temp1575=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1575->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1575->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1575;}); _temp1574;});
_temp1573;}); break; case 47: _LL1572: { struct _tagged_arr _temp1577= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1577, sizeof( unsigned char)) >  4?*(( const unsigned char*)
_check_unknown_subscript( _temp1577, sizeof( unsigned char), 0)) == '_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1577, sizeof( unsigned char),
1)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1577,
sizeof( unsigned char),( int)( _get_arr_size( _temp1577, sizeof( unsigned char))
-  2))) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1577,
sizeof( unsigned char),( int)( _get_arr_size( _temp1577, sizeof( unsigned char))
-  3))) == '_': 0){ _temp1577=( struct _tagged_arr) Cyc_Std_substring( _temp1577,
2, _get_arr_size( _temp1577, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0;
i <  16u; ++ i){ if( Cyc_Std_strcmp( _temp1577,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1578=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1578[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1579; _temp1579.tag= Cyc_Attribute_tok; _temp1579.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1579;}); _temp1578;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1580=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1580[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1581; _temp1581.tag= Cyc_Attribute_tok; _temp1581.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1581;}); _temp1580;});} break;}} case
48: _LL1576: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1583=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1583[ 0]=({ struct Cyc_Attribute_tok_struct _temp1584; _temp1584.tag= Cyc_Attribute_tok;
_temp1584.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1584;}); _temp1583;});
break; case 49: _LL1582: { struct _tagged_arr _temp1586= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1589; int _temp1590; struct _tuple15* _temp1587= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1589=*
_temp1587; _LL1591: _temp1590= _temp1589.f2; goto _LL1588; _LL1588: { void* a;
if( Cyc_Std_zstrcmp( _temp1586, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1586, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1590 <  0? 1: _temp1590 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 0 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1592=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1592[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1593; _temp1593.tag= Cyc_Absyn_Regparm_att; _temp1593.f1= _temp1590;
_temp1593;}); _temp1592;});} else{ if( Cyc_Std_zstrcmp( _temp1586, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1586, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1590 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1590;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1594=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1594[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1595; _temp1595.tag= Cyc_Absyn_Aligned_att; _temp1595.f1= _temp1590;
_temp1595;}); _temp1594;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1596=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1596[ 0]=({ struct Cyc_Attribute_tok_struct _temp1597; _temp1597.tag= Cyc_Attribute_tok;
_temp1597.f1=( void*) a; _temp1597;}); _temp1596;}); break;}} case 50: _LL1585: {
struct _tagged_arr _temp1599= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1600= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1599, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1599, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1601=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1601[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1602; _temp1602.tag=
Cyc_Absyn_Section_att; _temp1602.f1= _temp1600; _temp1602;}); _temp1601;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1603=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1603[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1604; _temp1604.tag= Cyc_Attribute_tok; _temp1604.f1=(
void*) a; _temp1604;}); _temp1603;}); break;} case 51: _LL1598: { struct
_tagged_arr _temp1606= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1607= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1610; struct _tuple15 _temp1608=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1611:
_temp1610= _temp1608.f2; goto _LL1609; _LL1609: { int _temp1614; struct _tuple15
_temp1612=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1615: _temp1614= _temp1612.f2; goto _LL1613; _LL1613: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1606, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1606, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1607, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1616=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1616[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1617; _temp1617.tag=
Cyc_Absyn_Format_att; _temp1617.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1617.f2= _temp1610; _temp1617.f3= _temp1614; _temp1617;}); _temp1616;});}
else{ if( Cyc_Std_zstrcmp( _temp1607, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1618=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1618[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1619; _temp1619.tag=
Cyc_Absyn_Format_att; _temp1619.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1619.f2= _temp1610; _temp1619.f3= _temp1614; _temp1619;}); _temp1618;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1620=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1620[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1621; _temp1621.tag= Cyc_Attribute_tok; _temp1621.f1=(
void*) a; _temp1621;}); _temp1620;}); break;}}} case 52: _LL1605: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1623=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1623[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1624; _temp1624.tag= Cyc_TypeSpecifier_tok;
_temp1624.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1624;});
_temp1623;}); break; case 53: _LL1622: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1626=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1626[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1627; _temp1627.tag=
Cyc_TypeSpecifier_tok; _temp1627.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1627;}); _temp1626;}); break; case 54: _LL1625:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1629=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1629[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1630; _temp1630.tag= Cyc_TypeSpecifier_tok;
_temp1630.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1630;});
_temp1629;}); break; case 55: _LL1628: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1632=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1632[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1633; _temp1633.tag=
Cyc_TypeSpecifier_tok; _temp1633.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1634=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1634[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1635; _temp1635.tag=
Cyc_Parse_Short_spec; _temp1635.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1635;});
_temp1634;})); _temp1633;}); _temp1632;}); break; case 56: _LL1631: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1637=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1637[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1638; _temp1638.tag= Cyc_TypeSpecifier_tok;
_temp1638.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1638;});
_temp1637;}); break; case 57: _LL1636: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1640=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1640[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1641; _temp1641.tag=
Cyc_TypeSpecifier_tok; _temp1641.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1642=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1642[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1643; _temp1643.tag= Cyc_Parse_Long_spec;
_temp1643.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1643;}); _temp1642;})); _temp1641;}); _temp1640;});
break; case 58: _LL1639: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1645=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1645[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1646; _temp1646.tag=
Cyc_TypeSpecifier_tok; _temp1646.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1646;}); _temp1645;}); break; case 59: _LL1644:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1648=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1648[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1649; _temp1649.tag= Cyc_TypeSpecifier_tok;
_temp1649.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t( 0), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1649;});
_temp1648;}); break; case 60: _LL1647: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1651=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1651[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1652; _temp1652.tag=
Cyc_TypeSpecifier_tok; _temp1652.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1653=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1653[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1654; _temp1654.tag=
Cyc_Parse_Signed_spec; _temp1654.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1654;});
_temp1653;})); _temp1652;}); _temp1651;}); break; case 61: _LL1650: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1656=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1656[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1657; _temp1657.tag= Cyc_TypeSpecifier_tok;
_temp1657.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1658=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1658[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1659; _temp1659.tag= Cyc_Parse_Unsigned_spec; _temp1659.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1659;});
_temp1658;})); _temp1657;}); _temp1656;}); break; case 62: _LL1655: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1660:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1661: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1662: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1664=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1664[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1665; _temp1665.tag=
Cyc_TypeSpecifier_tok; _temp1665.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1665;});
_temp1664;}); break; case 66: _LL1663: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1667=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1667[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1668; _temp1668.tag=
Cyc_TypeSpecifier_tok; _temp1668.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1669=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1669[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1670; _temp1670.tag= Cyc_Absyn_TypedefType;
_temp1670.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1670.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1670.f3= 0;
_temp1670;}); _temp1669;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1668;});
_temp1667;}); break; case 67: _LL1666: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1672=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1672[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1673; _temp1673.tag=
Cyc_TypeSpecifier_tok; _temp1673.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1674=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1674[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1675; _temp1675.tag= Cyc_Absyn_TupleType;
_temp1675.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1675;});
_temp1674;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1673;}); _temp1672;}); break; case 68:
_LL1671: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1677=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1677[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1678; _temp1678.tag=
Cyc_TypeSpecifier_tok; _temp1678.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1679=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1679[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1680; _temp1680.tag= Cyc_Absyn_RgnHandleType;
_temp1680.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1680;}); _temp1679;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1678;}); _temp1677;}); break; case 69: _LL1676: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1682=( struct Cyc_Kind_tok_struct*) _cycalloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1682[ 0]=({ struct Cyc_Kind_tok_struct _temp1683; _temp1683.tag= Cyc_Kind_tok;
_temp1683.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1683;});
_temp1682;}); break; case 70: _LL1681: { struct _tagged_arr* _temp1687; void*
_temp1689; struct _tuple1 _temp1685=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1690: _temp1689=
_temp1685.f1; goto _LL1688; _LL1688: _temp1687= _temp1685.f2; goto _LL1686;
_LL1686: if( _temp1689 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1691=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1691[ 0]=({ struct Cyc_Kind_tok_struct
_temp1692; _temp1692.tag= Cyc_Kind_tok; _temp1692.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1687, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1692;}); _temp1691;}); break;} case 71:
_LL1684: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1694=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1694[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1695; _temp1695.tag= Cyc_TypeQual_tok;
_temp1695.f1=({ struct Cyc_Absyn_Tqual _temp1696; _temp1696.q_const= 1;
_temp1696.q_volatile= 0; _temp1696.q_restrict= 0; _temp1696;}); _temp1695;});
_temp1694;}); break; case 72: _LL1693: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1698=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1698[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1699; _temp1699.tag= Cyc_TypeQual_tok; _temp1699.f1=({ struct Cyc_Absyn_Tqual
_temp1700; _temp1700.q_const= 0; _temp1700.q_volatile= 1; _temp1700.q_restrict=
0; _temp1700;}); _temp1699;}); _temp1698;}); break; case 73: _LL1697: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1702=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1702[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1703; _temp1703.tag= Cyc_TypeQual_tok;
_temp1703.f1=({ struct Cyc_Absyn_Tqual _temp1704; _temp1704.q_const= 0;
_temp1704.q_volatile= 0; _temp1704.q_restrict= 1; _temp1704;}); _temp1703;});
_temp1702;}); break; case 74: _LL1701: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1706=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1706[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1707; _temp1707.tag=
Cyc_TypeSpecifier_tok; _temp1707.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1708=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1708[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1709; _temp1709.tag= Cyc_Parse_Decl_spec;
_temp1709.f1=({ struct Cyc_Absyn_Decl* _temp1710=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1710->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1711=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1711[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1712; _temp1712.tag= Cyc_Absyn_Enum_d; _temp1712.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1713=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1713->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1713->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1713->fields=({
struct Cyc_Core_Opt* _temp1714=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1714->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1714;});
_temp1713;}); _temp1712;}); _temp1711;})); _temp1710->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1710;}); _temp1709;}); _temp1708;})); _temp1707;}); _temp1706;}); break;
case 75: _LL1705: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1716=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1716[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1717; _temp1717.tag=
Cyc_TypeSpecifier_tok; _temp1717.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1718=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1718[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1719; _temp1719.tag= Cyc_Absyn_EnumType;
_temp1719.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1719.f2= 0; _temp1719;}); _temp1718;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1717;}); _temp1716;}); break; case 76: _LL1715: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1721=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1721[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1722; _temp1722.tag=
Cyc_TypeSpecifier_tok; _temp1722.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1723=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1723[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1724; _temp1724.tag= Cyc_Parse_Type_spec;
_temp1724.f1=( void*)(( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp1725=(
struct Cyc_Absyn_AnonEnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct));
_temp1725[ 0]=({ struct Cyc_Absyn_AnonEnumType_struct _temp1726; _temp1726.tag=
Cyc_Absyn_AnonEnumType; _temp1726.f1= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1726;});
_temp1725;})); _temp1724.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1724;});
_temp1723;})); _temp1722;}); _temp1721;}); break; case 77: _LL1720: yyval=( void*)({
struct Cyc_Enumfield_tok_struct* _temp1728=( struct Cyc_Enumfield_tok_struct*)
_cycalloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp1728[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp1729; _temp1729.tag= Cyc_Enumfield_tok; _temp1729.f1=({
struct Cyc_Absyn_Enumfield* _temp1730=( struct Cyc_Absyn_Enumfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumfield)); _temp1730->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1730->tag= 0;
_temp1730->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1730;});
_temp1729;}); _temp1728;}); break; case 78: _LL1727: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1732=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1732[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1733; _temp1733.tag= Cyc_Enumfield_tok;
_temp1733.f1=({ struct Cyc_Absyn_Enumfield* _temp1734=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1734->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1734->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1734->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1734;});
_temp1733;}); _temp1732;}); break; case 79: _LL1731: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1736=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1736[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1737; _temp1737.tag=
Cyc_EnumfieldList_tok; _temp1737.f1=({ struct Cyc_List_List* _temp1738=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1738->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1738->tl= 0; _temp1738;}); _temp1737;}); _temp1736;});
break; case 80: _LL1735: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1740=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1740[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1741; _temp1741.tag=
Cyc_EnumfieldList_tok; _temp1741.f1=({ struct Cyc_List_List* _temp1742=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1742->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1742->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1742;}); _temp1741;});
_temp1740;}); break; case 81: _LL1739: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1744: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1746=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1746[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1747; _temp1747.tag=
Cyc_Absyn_AnonStructType; _temp1747.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1747;});
_temp1746;}); break; case Cyc_Parse_Union_su: _LL1745: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1749=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1749[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1750; _temp1750.tag= Cyc_Absyn_AnonUnionType; _temp1750.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1750;});
_temp1749;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1751=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1751[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1752; _temp1752.tag=
Cyc_TypeSpecifier_tok; _temp1752.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1753=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1753[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1754; _temp1754.tag= Cyc_Parse_Type_spec;
_temp1754.f1=( void*) t; _temp1754.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1754;});
_temp1753;})); _temp1752;}); _temp1751;}); break;} case 82: _LL1743: { struct
Cyc_List_List* _temp1756=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1757: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1759=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1759->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1759;}), _temp1756,({ struct Cyc_Core_Opt*
_temp1760=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1760->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1760;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1758: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1762=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1762->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1762;}),
_temp1756,({ struct Cyc_Core_Opt* _temp1763=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1763->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1763;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1764=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1764[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1765; _temp1765.tag=
Cyc_TypeSpecifier_tok; _temp1765.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1766=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1766[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1767; _temp1767.tag= Cyc_Parse_Decl_spec;
_temp1767.f1= d; _temp1767;}); _temp1766;})); _temp1765;}); _temp1764;}); break;}
case 83: _LL1755: { struct Cyc_List_List* _temp1769=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1770: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1772=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1772->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1772;}), _temp1769,({ struct Cyc_Core_Opt*
_temp1773=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1773->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1773;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1771: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1775=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1775->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1775;}),
_temp1769,({ struct Cyc_Core_Opt* _temp1776=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1776->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1776;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1777=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1777[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1778; _temp1778.tag=
Cyc_TypeSpecifier_tok; _temp1778.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1779=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1779[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1780; _temp1780.tag= Cyc_Parse_Decl_spec;
_temp1780.f1= d; _temp1780;}); _temp1779;})); _temp1778;}); _temp1777;}); break;}
case 84: _LL1768: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1782: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1784=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1784[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1785; _temp1785.tag=
Cyc_TypeSpecifier_tok; _temp1785.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1786=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1786[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1787; _temp1787.tag= Cyc_Absyn_StructType;
_temp1787.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1787.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1787.f3= 0;
_temp1787;}); _temp1786;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1785;});
_temp1784;}); break; case Cyc_Parse_Union_su: _LL1783: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1789=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1789[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1790; _temp1790.tag= Cyc_TypeSpecifier_tok;
_temp1790.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1791=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1791[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1792; _temp1792.tag= Cyc_Absyn_UnionType;
_temp1792.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1792.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1792.f3= 0;
_temp1792;}); _temp1791;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1790;});
_temp1789;}); break;} break; case 85: _LL1781: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1794: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1796=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1796[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1797; _temp1797.tag=
Cyc_TypeSpecifier_tok; _temp1797.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1798=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1798[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1799; _temp1799.tag= Cyc_Absyn_StructType;
_temp1799.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1799.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1799.f3= 0;
_temp1799;}); _temp1798;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1797;});
_temp1796;}); break; case Cyc_Parse_Union_su: _LL1795: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1801=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1801[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1802; _temp1802.tag= Cyc_TypeSpecifier_tok;
_temp1802.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1803=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1803[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1804; _temp1804.tag= Cyc_Absyn_UnionType;
_temp1804.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1804.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1804.f3= 0;
_temp1804;}); _temp1803;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1802;});
_temp1801;}); break;} break; case 86: _LL1793: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1806=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1806[ 0]=({ struct Cyc_TypeList_tok_struct _temp1807; _temp1807.tag= Cyc_TypeList_tok;
_temp1807.f1= 0; _temp1807;}); _temp1806;}); break; case 87: _LL1805: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1809=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1809[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1810; _temp1810.tag= Cyc_TypeList_tok; _temp1810.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1810;});
_temp1809;}); break; case 88: _LL1808:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1812=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1812[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1813; _temp1813.tag= Cyc_TypeList_tok; _temp1813.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1813;});
_temp1812;}); break; case 89: _LL1811: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1815=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1815[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1816; _temp1816.tag= Cyc_StructOrUnion_tok; _temp1816.f1= Cyc_Parse_Struct_su;
_temp1816;}); _temp1815;}); break; case 90: _LL1814: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1818=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1818[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1819; _temp1819.tag= Cyc_StructOrUnion_tok; _temp1819.f1= Cyc_Parse_Union_su;
_temp1819;}); _temp1818;}); break; case 91: _LL1817: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1821=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1821[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1822; _temp1822.tag= Cyc_StructFieldDeclList_tok; _temp1822.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1822;});
_temp1821;}); break; case 92: _LL1820: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1824=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1824[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1825; _temp1825.tag= Cyc_StructFieldDeclListList_tok; _temp1825.f1=({
struct Cyc_List_List* _temp1826=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1826->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1826->tl= 0;
_temp1826;}); _temp1825;}); _temp1824;}); break; case 93: _LL1823: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1828=( struct Cyc_StructFieldDeclListList_tok_struct*)
_cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1828[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1829; _temp1829.tag= Cyc_StructFieldDeclListList_tok;
_temp1829.f1=({ struct Cyc_List_List* _temp1830=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1830->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1830->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1830;}); _temp1829;}); _temp1828;}); break;
case 94: _LL1827: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1832=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1832[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1833; _temp1833.tag=
Cyc_InitDeclList_tok; _temp1833.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1833;});
_temp1832;}); break; case 95: _LL1831: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1835=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1835[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1836; _temp1836.tag=
Cyc_InitDeclList_tok; _temp1836.f1=({ struct Cyc_List_List* _temp1837=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1837->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1837->tl= 0; _temp1837;}); _temp1836;}); _temp1835;});
break; case 96: _LL1834: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1839=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1839[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1840; _temp1840.tag=
Cyc_InitDeclList_tok; _temp1840.f1=({ struct Cyc_List_List* _temp1841=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1841->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1841->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1841;});
_temp1840;}); _temp1839;}); break; case 97: _LL1838: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1843=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1843[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1844; _temp1844.tag= Cyc_InitDecl_tok;
_temp1844.f1=({ struct _tuple14* _temp1845=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1845->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1845->f2= 0;
_temp1845;}); _temp1844;}); _temp1843;}); break; case 98: _LL1842: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1847=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1847[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1848; _temp1848.tag= Cyc_InitDecl_tok; _temp1848.f1=({ struct _tuple14*
_temp1849=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1849->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1849->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1849;});
_temp1848;}); _temp1847;}); break; case 99: _LL1846: { struct _tuple17 _temp1853;
struct Cyc_List_List* _temp1854; struct Cyc_List_List* _temp1856; struct Cyc_Absyn_Tqual
_temp1858; struct _tuple17* _temp1851= Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1853=*
_temp1851; _LL1859: _temp1858= _temp1853.f1; goto _LL1857; _LL1857: _temp1856=
_temp1853.f2; goto _LL1855; _LL1855: _temp1854= _temp1853.f3; goto _LL1852;
_LL1852: { struct Cyc_List_List* _temp1862; struct Cyc_List_List* _temp1864;
struct _tuple0 _temp1860=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1865: _temp1864= _temp1860.f1; goto _LL1863; _LL1863:
_temp1862= _temp1860.f2; goto _LL1861; _LL1861: { void* _temp1866= Cyc_Parse_speclist2typ(
_temp1856, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1867=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1858, _temp1866, _temp1864, _temp1854), _temp1862);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1868=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1868[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1869; _temp1869.tag=
Cyc_StructFieldDeclList_tok; _temp1869.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1867); _temp1869;}); _temp1868;}); break;}}}
case 100: _LL1850: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1871=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1871[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1872; _temp1872.tag=
Cyc_QualSpecList_tok; _temp1872.f1=({ struct _tuple17* _temp1873=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1873->f1= Cyc_Absyn_empty_tqual();
_temp1873->f2=({ struct Cyc_List_List* _temp1874=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1874->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1874->tl=
0; _temp1874;}); _temp1873->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1873;}); _temp1872;});
_temp1871;}); break; case 101: _LL1870: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1876=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1876[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1877; _temp1877.tag=
Cyc_QualSpecList_tok; _temp1877.f1=({ struct _tuple17* _temp1878=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1878->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1878->f2=({
struct Cyc_List_List* _temp1879=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1879->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1879->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1879;}); _temp1878->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1878;});
_temp1877;}); _temp1876;}); break; case 102: _LL1875: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1881=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1881[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1882; _temp1882.tag= Cyc_QualSpecList_tok;
_temp1882.f1=({ struct _tuple17* _temp1883=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1883->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1883->f2= 0;
_temp1883->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1883;}); _temp1882;}); _temp1881;}); break; case
103: _LL1880: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1885=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1885[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1886; _temp1886.tag=
Cyc_QualSpecList_tok; _temp1886.f1=({ struct _tuple17* _temp1887=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1887->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1887->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1887->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1887;});
_temp1886;}); _temp1885;}); break; case 104: _LL1884: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1889=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1889[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1890; _temp1890.tag= Cyc_InitDeclList_tok;
_temp1890.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1890;}); _temp1889;}); break; case 105: _LL1888: yyval=(
void*)({ struct Cyc_InitDeclList_tok_struct* _temp1892=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1892[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1893; _temp1893.tag= Cyc_InitDeclList_tok;
_temp1893.f1=({ struct Cyc_List_List* _temp1894=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1894->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1894->tl= 0;
_temp1894;}); _temp1893;}); _temp1892;}); break; case 106: _LL1891: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp1896=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1896[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1897; _temp1897.tag= Cyc_InitDeclList_tok;
_temp1897.f1=({ struct Cyc_List_List* _temp1898=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1898->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1898->tl=
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1898;}); _temp1897;}); _temp1896;}); break; case 107:
_LL1895: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp1900=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1900[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1901; _temp1901.tag= Cyc_InitDecl_tok; _temp1901.f1=({ struct _tuple14*
_temp1902=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1902->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1902->f2= 0; _temp1902;}); _temp1901;}); _temp1900;});
break; case 108: _LL1899: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1904=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1904[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1905; _temp1905.tag= Cyc_InitDecl_tok;
_temp1905.f1=({ struct _tuple14* _temp1906=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1906->f1=({ struct Cyc_Parse_Declarator*
_temp1907=( struct Cyc_Parse_Declarator*) _cycalloc( sizeof( struct Cyc_Parse_Declarator));
_temp1907->id=({ struct _tuple1* _temp1908=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp1908->f1= Cyc_Absyn_rel_ns_null; _temp1908->f2=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1908;}); _temp1907->tms= 0; _temp1907;});
_temp1906->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1906;}); _temp1905;});
_temp1904;}); break; case 109: _LL1903: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1910=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1910[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1911; _temp1911.tag= Cyc_InitDecl_tok;
_temp1911.f1=({ struct _tuple14* _temp1912=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1912->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1912->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1912;}); _temp1911;}); _temp1910;}); break; case
110: _LL1909: { struct Cyc_List_List* _temp1914=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1915=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1915[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1916; _temp1916.tag=
Cyc_TypeSpecifier_tok; _temp1916.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1917=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1917[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1918; _temp1918.tag= Cyc_Parse_Decl_spec;
_temp1918.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1914,({
struct Cyc_Core_Opt* _temp1919=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1919->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1919;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1918;}); _temp1917;})); _temp1916;}); _temp1915;}); break;} case 111:
_LL1913: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1921=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1921[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1922; _temp1922.tag=
Cyc_TypeSpecifier_tok; _temp1922.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1923=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1923[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1924; _temp1924.tag= Cyc_Absyn_TunionType;
_temp1924.f1=({ struct Cyc_Absyn_TunionInfo _temp1925; _temp1925.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1926=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1926[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1927; _temp1927.tag= Cyc_Absyn_UnknownTunion;
_temp1927.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1928; _temp1928.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1928.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1928;});
_temp1927;}); _temp1926;})); _temp1925.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1925.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1925;}); _temp1924;}); _temp1923;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1922;}); _temp1921;}); break; case 112: _LL1920: { void* _temp1930= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp1939=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1939->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp1939;}),
0); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1931=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1931[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1932; _temp1932.tag= Cyc_TypeSpecifier_tok;
_temp1932.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1933=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1933[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1934; _temp1934.tag=
Cyc_Absyn_TunionType; _temp1934.f1=({ struct Cyc_Absyn_TunionInfo _temp1935;
_temp1935.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1936=( struct Cyc_Absyn_UnknownTunion_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1936[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1937; _temp1937.tag= Cyc_Absyn_UnknownTunion; _temp1937.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1938; _temp1938.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1938.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1938;}); _temp1937;}); _temp1936;})); _temp1935.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1935.rgn=(
void*) _temp1930; _temp1935;}); _temp1934;}); _temp1933;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1932;}); _temp1931;}); break;} case 113: _LL1929: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1941=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1941[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1942; _temp1942.tag= Cyc_TypeSpecifier_tok;
_temp1942.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1943=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1943[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1944; _temp1944.tag= Cyc_Absyn_TunionFieldType; _temp1944.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1945; _temp1945.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp1946=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp1946[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp1947; _temp1947.tag= Cyc_Absyn_UnknownTunionfield; _temp1947.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp1948; _temp1948.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1948.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1948.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1948;});
_temp1947;}); _temp1946;})); _temp1945.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1945;}); _temp1944;});
_temp1943;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1942;}); _temp1941;}); break; case 114:
_LL1940: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1950=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1950[ 0]=({ struct
Cyc_Bool_tok_struct _temp1951; _temp1951.tag= Cyc_Bool_tok; _temp1951.f1= 0;
_temp1951;}); _temp1950;}); break; case 115: _LL1949: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1953=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1953[ 0]=({ struct Cyc_Bool_tok_struct
_temp1954; _temp1954.tag= Cyc_Bool_tok; _temp1954.f1= 1; _temp1954;}); _temp1953;});
break; case 116: _LL1952: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1956=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1956[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1957; _temp1957.tag= Cyc_TunionFieldList_tok; _temp1957.f1=({ struct Cyc_List_List*
_temp1958=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1958->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1958->tl= 0;
_temp1958;}); _temp1957;}); _temp1956;}); break; case 117: _LL1955: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp1960=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1960[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1961; _temp1961.tag= Cyc_TunionFieldList_tok;
_temp1961.f1=({ struct Cyc_List_List* _temp1962=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1962->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1962->tl=
0; _temp1962;}); _temp1961;}); _temp1960;}); break; case 118: _LL1959: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1964=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1964[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1965; _temp1965.tag= Cyc_TunionFieldList_tok;
_temp1965.f1=({ struct Cyc_List_List* _temp1966=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1966->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1966->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1966;}); _temp1965;}); _temp1964;}); break; case 119:
_LL1963: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1968=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp1968[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp1969; _temp1969.tag=
Cyc_TunionFieldList_tok; _temp1969.f1=({ struct Cyc_List_List* _temp1970=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1970->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1970->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1970;}); _temp1969;});
_temp1968;}); break; case 120: _LL1967: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1972=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1972[ 0]=({ struct Cyc_Scope_tok_struct _temp1973; _temp1973.tag= Cyc_Scope_tok;
_temp1973.f1=( void*)(( void*) Cyc_Absyn_Public); _temp1973;}); _temp1972;});
break; case 121: _LL1971: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1975=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1975[ 0]=({ struct Cyc_Scope_tok_struct _temp1976; _temp1976.tag= Cyc_Scope_tok;
_temp1976.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp1976;}); _temp1975;});
break; case 122: _LL1974: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1978=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1978[ 0]=({ struct Cyc_Scope_tok_struct _temp1979; _temp1979.tag= Cyc_Scope_tok;
_temp1979.f1=( void*)(( void*) Cyc_Absyn_Static); _temp1979;}); _temp1978;});
break; case 123: _LL1977: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp1981=( struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp1981[ 0]=({ struct Cyc_TunionField_tok_struct _temp1982; _temp1982.tag= Cyc_TunionField_tok;
_temp1982.f1=({ struct Cyc_Absyn_Tunionfield* _temp1983=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1983->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1983->tvs= 0;
_temp1983->typs= 0; _temp1983->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1983->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1983;}); _temp1982;}); _temp1981;}); break; case 124:
_LL1980: { struct Cyc_List_List* _temp1985=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp1986=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp1987=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp1987[ 0]=({ struct
Cyc_TunionField_tok_struct _temp1988; _temp1988.tag= Cyc_TunionField_tok;
_temp1988.f1=({ struct Cyc_Absyn_Tunionfield* _temp1989=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1989->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1989->tvs=
_temp1986; _temp1989->typs= _temp1985; _temp1989->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1989->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp1989;}); _temp1988;}); _temp1987;}); break;}
case 125: _LL1984: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 126: _LL1990: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1992=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1992[ 0]=({ struct Cyc_Declarator_tok_struct _temp1993; _temp1993.tag= Cyc_Declarator_tok;
_temp1993.f1=({ struct Cyc_Parse_Declarator* _temp1994=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1994->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp1994->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp1994;});
_temp1993;}); _temp1992;}); break; case 127: _LL1991: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp1996=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp1996[ 0]=({ struct
Cyc_Declarator_tok_struct _temp1997; _temp1997.tag= Cyc_Declarator_tok;
_temp1997.f1=({ struct Cyc_Parse_Declarator* _temp1998=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1998->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1998->tms= 0;
_temp1998;}); _temp1997;}); _temp1996;}); break; case 128: _LL1995: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 129:
_LL1999: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2001=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2001[ 0]=({ struct Cyc_Declarator_tok_struct _temp2002; _temp2002.tag= Cyc_Declarator_tok;
_temp2002.f1=({ struct Cyc_Parse_Declarator* _temp2003=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2003->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp2003->tms=({ struct Cyc_List_List* _temp2004=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2004->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2004->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp2004;}); _temp2003;}); _temp2002;});
_temp2001;}); break; case 130: _LL2000: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2006=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2006[ 0]=({ struct Cyc_Declarator_tok_struct _temp2007; _temp2007.tag= Cyc_Declarator_tok;
_temp2007.f1=({ struct Cyc_Parse_Declarator* _temp2008=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2008->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2008->tms=({ struct Cyc_List_List* _temp2009=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2009->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2010=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2010[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2011; _temp2011.tag= Cyc_Absyn_ConstArray_mod;
_temp2011.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2011;}); _temp2010;})); _temp2009->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2009;}); _temp2008;}); _temp2007;}); _temp2006;}); break; case 131: _LL2005: {
struct _tuple16 _temp2015; struct Cyc_List_List* _temp2016; struct Cyc_Core_Opt*
_temp2018; struct Cyc_Absyn_VarargInfo* _temp2020; int _temp2022; struct Cyc_List_List*
_temp2024; struct _tuple16* _temp2013= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2015=*
_temp2013; _LL2025: _temp2024= _temp2015.f1; goto _LL2023; _LL2023: _temp2022=
_temp2015.f2; goto _LL2021; _LL2021: _temp2020= _temp2015.f3; goto _LL2019;
_LL2019: _temp2018= _temp2015.f4; goto _LL2017; _LL2017: _temp2016= _temp2015.f5;
goto _LL2014; _LL2014: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2026=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2026[ 0]=({ struct Cyc_Declarator_tok_struct _temp2027; _temp2027.tag= Cyc_Declarator_tok;
_temp2027.f1=({ struct Cyc_Parse_Declarator* _temp2028=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2028->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2028->tms=({ struct Cyc_List_List* _temp2029=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2029->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2030=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2030[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2031; _temp2031.tag= Cyc_Absyn_Function_mod;
_temp2031.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2032=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2032[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2033; _temp2033.tag= Cyc_Absyn_WithTypes;
_temp2033.f1= _temp2024; _temp2033.f2= _temp2022; _temp2033.f3= _temp2020;
_temp2033.f4= _temp2018; _temp2033.f5= _temp2016; _temp2033;}); _temp2032;}));
_temp2031;}); _temp2030;})); _temp2029->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2029;});
_temp2028;}); _temp2027;}); _temp2026;}); break;} case 132: _LL2012: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2035=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2035[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2036; _temp2036.tag= Cyc_Declarator_tok;
_temp2036.f1=({ struct Cyc_Parse_Declarator* _temp2037=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2037->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2037->tms=({ struct Cyc_List_List* _temp2038=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2038->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2039=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2039[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2040; _temp2040.tag= Cyc_Absyn_Function_mod;
_temp2040.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2041=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2041[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2042; _temp2042.tag= Cyc_Absyn_WithTypes;
_temp2042.f1= 0; _temp2042.f2= 0; _temp2042.f3= 0; _temp2042.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2042.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2042;}); _temp2041;})); _temp2040;}); _temp2039;}));
_temp2038->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2038;}); _temp2037;}); _temp2036;});
_temp2035;}); break; case 133: _LL2034: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2044=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2044[ 0]=({ struct Cyc_Declarator_tok_struct _temp2045; _temp2045.tag= Cyc_Declarator_tok;
_temp2045.f1=({ struct Cyc_Parse_Declarator* _temp2046=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2046->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2046->tms=({ struct Cyc_List_List* _temp2047=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2047->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2048=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2048[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2049; _temp2049.tag= Cyc_Absyn_Function_mod;
_temp2049.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2050=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2050[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2051; _temp2051.tag= Cyc_Absyn_NoTypes;
_temp2051.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2051.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2051;});
_temp2050;})); _temp2049;}); _temp2048;})); _temp2047->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2047;}); _temp2046;}); _temp2045;}); _temp2044;}); break; case 134: _LL2043: {
struct Cyc_List_List* _temp2053=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2054=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2054[ 0]=({ struct Cyc_Declarator_tok_struct _temp2055; _temp2055.tag= Cyc_Declarator_tok;
_temp2055.f1=({ struct Cyc_Parse_Declarator* _temp2056=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2056->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2056->tms=({ struct Cyc_List_List* _temp2057=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2057->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2058=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2058[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2059; _temp2059.tag= Cyc_Absyn_TypeParams_mod;
_temp2059.f1= _temp2053; _temp2059.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2059.f3=
0; _temp2059;}); _temp2058;})); _temp2057->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2057;});
_temp2056;}); _temp2055;}); _temp2054;}); break;} case 135: _LL2052:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2061=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2062=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2062[ 0]=({ struct Cyc_Declarator_tok_struct _temp2063; _temp2063.tag= Cyc_Declarator_tok;
_temp2063.f1=({ struct Cyc_Parse_Declarator* _temp2064=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2064->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2064->tms=({ struct Cyc_List_List* _temp2065=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2065->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2066=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2066[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2067; _temp2067.tag= Cyc_Absyn_TypeParams_mod;
_temp2067.f1= _temp2061; _temp2067.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2067.f3=
0; _temp2067;}); _temp2066;})); _temp2065->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2065;});
_temp2064;}); _temp2063;}); _temp2062;}); break;} case 136: _LL2060: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2069=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2069[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2070; _temp2070.tag= Cyc_Declarator_tok;
_temp2070.f1=({ struct Cyc_Parse_Declarator* _temp2071=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2071->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2071->tms=({ struct Cyc_List_List* _temp2072=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2072->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2073=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2073[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2074; _temp2074.tag= Cyc_Absyn_Attributes_mod;
_temp2074.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2074.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2074;}); _temp2073;}));
_temp2072->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]))->tms; _temp2072;}); _temp2071;}); _temp2070;});
_temp2069;}); break; case 137: _LL2068: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2076=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2076[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2077; _temp2077.tag= Cyc_Declarator_tok;
_temp2077.f1=({ struct Cyc_Parse_Declarator* _temp2078=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2078->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2078->tms= 0;
_temp2078;}); _temp2077;}); _temp2076;}); break; case 138: _LL2075: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2080=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2080[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2081; _temp2081.tag= Cyc_Declarator_tok;
_temp2081.f1=({ struct Cyc_Parse_Declarator* _temp2082=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2082->id= Cyc_Absyn_exn_name;
_temp2082->tms= 0; _temp2082;}); _temp2081;}); _temp2080;}); break; case 139:
_LL2079: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2084=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2084[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2085; _temp2085.tag=
Cyc_TypeModifierList_tok; _temp2085.f1=({ struct Cyc_List_List* _temp2086=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2086->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2087=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2087[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2088; _temp2088.tag= Cyc_Absyn_Pointer_mod;
_temp2088.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2088.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2088.f3= Cyc_Absyn_empty_tqual(); _temp2088;});
_temp2087;})); _temp2086->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2086;});
_temp2085;}); _temp2084;}); break; case 140: _LL2083: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2090=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2090[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2091; _temp2091.tag= Cyc_TypeModifierList_tok;
_temp2091.f1=({ struct Cyc_List_List* _temp2092=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2092->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2093=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2093[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2094; _temp2094.tag= Cyc_Absyn_Pointer_mod;
_temp2094.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2094.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2094.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2094;}); _temp2093;}));
_temp2092->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0);
_temp2092;}); _temp2091;}); _temp2090;}); break; case 141: _LL2089: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2096=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2096[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2097; _temp2097.tag= Cyc_TypeModifierList_tok;
_temp2097.f1=({ struct Cyc_List_List* _temp2098=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2098->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2099=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2099[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2100; _temp2100.tag= Cyc_Absyn_Pointer_mod;
_temp2100.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2100.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2100.f3= Cyc_Absyn_empty_tqual(); _temp2100;});
_temp2099;})); _temp2098->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2098;});
_temp2097;}); _temp2096;}); break; case 142: _LL2095: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2102=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2102[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2103; _temp2103.tag= Cyc_TypeModifierList_tok;
_temp2103.f1=({ struct Cyc_List_List* _temp2104=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2104->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2105=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2105[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2106; _temp2106.tag= Cyc_Absyn_Pointer_mod;
_temp2106.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2106.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2106.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2106;});
_temp2105;})); _temp2104->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2104;});
_temp2103;}); _temp2102;}); break; case 143: _LL2101: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2108=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2108[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2109; _temp2109.tag= Cyc_Pointer_Sort_tok;
_temp2109.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2110=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2110[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2111; _temp2111.tag=
Cyc_Absyn_Nullable_ps; _temp2111.f1= Cyc_Absyn_exp_unsigned_one; _temp2111;});
_temp2110;})); _temp2109;}); _temp2108;}); break; case 144: _LL2107: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2113=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2113[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2114; _temp2114.tag= Cyc_Pointer_Sort_tok;
_temp2114.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2115=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2115[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2116; _temp2116.tag= Cyc_Absyn_NonNullable_ps; _temp2116.f1= Cyc_Absyn_exp_unsigned_one;
_temp2116;}); _temp2115;})); _temp2114;}); _temp2113;}); break; case 145:
_LL2112: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2118=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2118[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2119; _temp2119.tag=
Cyc_Pointer_Sort_tok; _temp2119.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2120=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2120[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2121; _temp2121.tag=
Cyc_Absyn_Nullable_ps; _temp2121.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2121;});
_temp2120;})); _temp2119;}); _temp2118;}); break; case 146: _LL2117: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2123=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2123[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2124; _temp2124.tag= Cyc_Pointer_Sort_tok;
_temp2124.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2125=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2125[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2126; _temp2126.tag= Cyc_Absyn_NonNullable_ps; _temp2126.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2126;});
_temp2125;})); _temp2124;}); _temp2123;}); break; case 147: _LL2122: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2128=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2128[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2129; _temp2129.tag= Cyc_Pointer_Sort_tok;
_temp2129.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2129;});
_temp2128;}); break; case 148: _LL2127: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2131=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2131[ 0]=({ struct Cyc_Type_tok_struct _temp2132; _temp2132.tag= Cyc_Type_tok;
_temp2132.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2133=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2133->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2133;}), 0); _temp2132;}); _temp2131;});
break; case 149: _LL2130: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 150: _LL2134: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
151: _LL2135: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2137=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2137[
0]=({ struct Cyc_Type_tok_struct _temp2138; _temp2138.tag= Cyc_Type_tok;
_temp2138.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2139=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2139->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2139;}), 0); _temp2138;}); _temp2137;});
break; case 152: _LL2136: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 153: _LL2140: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2142=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2142[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2143; _temp2143.tag= Cyc_TypeQual_tok; _temp2143.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2143;});
_temp2142;}); break; case 154: _LL2141: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2145=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2145[ 0]=({ struct Cyc_YY1_struct _temp2146; _temp2146.tag= Cyc_YY1;
_temp2146.f1=({ struct _tuple16* _temp2147=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2147->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2147->f2= 0;
_temp2147->f3= 0; _temp2147->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2147->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2147;});
_temp2146;}); _temp2145;}); break; case 155: _LL2144: yyval=( void*)({ struct
Cyc_YY1_struct* _temp2149=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct
Cyc_YY1_struct)); _temp2149[ 0]=({ struct Cyc_YY1_struct _temp2150; _temp2150.tag=
Cyc_YY1; _temp2150.f1=({ struct _tuple16* _temp2151=( struct _tuple16*)
_cycalloc( sizeof( struct _tuple16)); _temp2151->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2151->f2= 1;
_temp2151->f3= 0; _temp2151->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2151->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2151;});
_temp2150;}); _temp2149;}); break; case 156: _LL2148: { struct _tuple2 _temp2155;
void* _temp2156; struct Cyc_Absyn_Tqual _temp2158; struct Cyc_Core_Opt*
_temp2160; struct _tuple2* _temp2153= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2155=*
_temp2153; _LL2161: _temp2160= _temp2155.f1; goto _LL2159; _LL2159: _temp2158=
_temp2155.f2; goto _LL2157; _LL2157: _temp2156= _temp2155.f3; goto _LL2154;
_LL2154: { struct Cyc_Absyn_VarargInfo* _temp2162=({ struct Cyc_Absyn_VarargInfo*
_temp2166=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2166->name= _temp2160; _temp2166->tq= _temp2158; _temp2166->type=( void*)
_temp2156; _temp2166->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2166;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2163=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2163[ 0]=({ struct Cyc_YY1_struct
_temp2164; _temp2164.tag= Cyc_YY1; _temp2164.f1=({ struct _tuple16* _temp2165=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2165->f1= 0;
_temp2165->f2= 0; _temp2165->f3= _temp2162; _temp2165->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2165->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2165;}); _temp2164;}); _temp2163;}); break;}} case 157:
_LL2152: { struct _tuple2 _temp2170; void* _temp2171; struct Cyc_Absyn_Tqual
_temp2173; struct Cyc_Core_Opt* _temp2175; struct _tuple2* _temp2168= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2170=*
_temp2168; _LL2176: _temp2175= _temp2170.f1; goto _LL2174; _LL2174: _temp2173=
_temp2170.f2; goto _LL2172; _LL2172: _temp2171= _temp2170.f3; goto _LL2169;
_LL2169: { struct Cyc_Absyn_VarargInfo* _temp2177=({ struct Cyc_Absyn_VarargInfo*
_temp2181=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2181->name= _temp2175; _temp2181->tq= _temp2173; _temp2181->type=( void*)
_temp2171; _temp2181->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2181;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2178=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2178[ 0]=({ struct Cyc_YY1_struct
_temp2179; _temp2179.tag= Cyc_YY1; _temp2179.f1=({ struct _tuple16* _temp2180=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2180->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2180->f2=
0; _temp2180->f3= _temp2177; _temp2180->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2180->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2180;});
_temp2179;}); _temp2178;}); break;}} case 158: _LL2167: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2183=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2183[ 0]=({ struct Cyc_Type_tok_struct
_temp2184; _temp2184.tag= Cyc_Type_tok; _temp2184.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()); _temp2184;}); _temp2183;}); break; case 159: _LL2182:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2186=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2186[ 0]=({ struct Cyc_Type_tok_struct
_temp2187; _temp2187.tag= Cyc_Type_tok; _temp2187.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp2187;});
_temp2186;}); break; case 160: _LL2185: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2189=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2189[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2190; _temp2190.tag= Cyc_TypeOpt_tok;
_temp2190.f1= 0; _temp2190;}); _temp2189;}); break; case 161: _LL2188: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2192=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2192[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2193; _temp2193.tag= Cyc_TypeOpt_tok; _temp2193.f1=({ struct Cyc_Core_Opt*
_temp2194=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2194->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2195=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2195[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2196; _temp2196.tag= Cyc_Absyn_JoinEff;
_temp2196.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2196;}); _temp2195;})); _temp2194;}); _temp2193;});
_temp2192;}); break; case 162: _LL2191: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2198=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2198[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2199; _temp2199.tag= Cyc_Rgnorder_tok;
_temp2199.f1= 0; _temp2199;}); _temp2198;}); break; case 163: _LL2197: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 164:
_LL2200: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2202=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2202[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2203; _temp2203.tag= Cyc_Rgnorder_tok; _temp2203.f1=({ struct Cyc_List_List*
_temp2204=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2204->hd=( void*)({ struct _tuple18* _temp2205=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2205->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2205->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2205;}); _temp2204->tl= 0; _temp2204;});
_temp2203;}); _temp2202;}); break; case 165: _LL2201: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2207=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2207[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2208; _temp2208.tag= Cyc_Rgnorder_tok; _temp2208.f1=({ struct Cyc_List_List*
_temp2209=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2209->hd=( void*)({ struct _tuple18* _temp2210=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2210->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2210->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2210;}); _temp2209->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2209;});
_temp2208;}); _temp2207;}); break; case 166: _LL2206: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2212=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2212[ 0]=({ struct Cyc_Bool_tok_struct
_temp2213; _temp2213.tag= Cyc_Bool_tok; _temp2213.f1= 0; _temp2213;}); _temp2212;});
break; case 167: _LL2211: { struct _tagged_arr _temp2215= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Std_zstrcmp(
_temp2215, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2216=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2216[ 0]=({ struct Cyc_Bool_tok_struct _temp2217; _temp2217.tag= Cyc_Bool_tok;
_temp2217.f1= 1; _temp2217;}); _temp2216;}); break;} case 168: _LL2214: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 169:
_LL2218: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2220=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2220[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2221; _temp2221.tag= Cyc_TypeList_tok; _temp2221.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2221;});
_temp2220;}); break; case 170: _LL2219: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2223=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2223[ 0]=({ struct Cyc_TypeList_tok_struct _temp2224; _temp2224.tag= Cyc_TypeList_tok;
_temp2224.f1= 0; _temp2224;}); _temp2223;}); break; case 171: _LL2222: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case
172: _LL2225: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2227=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2227[ 0]=({ struct Cyc_TypeList_tok_struct _temp2228; _temp2228.tag= Cyc_TypeList_tok;
_temp2228.f1=({ struct Cyc_List_List* _temp2229=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2229->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp2230=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp2230[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp2231; _temp2231.tag= Cyc_Absyn_RgnsEff; _temp2231.f1=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2231;}); _temp2230;})); _temp2229->tl= 0; _temp2229;});
_temp2228;}); _temp2227;}); break; case 173: _LL2226: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2233=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2233[ 0]=({ struct Cyc_TypeList_tok_struct _temp2234; _temp2234.tag= Cyc_TypeList_tok;
_temp2234.f1=({ struct Cyc_List_List* _temp2235=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2235->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2235->tl= 0;
_temp2235;}); _temp2234;}); _temp2233;}); break; case 174: _LL2232: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2237=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2237[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2238; _temp2238.tag= Cyc_TypeList_tok; _temp2238.f1=({ struct Cyc_List_List*
_temp2239=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2239->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2240=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2240[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2241; _temp2241.tag= Cyc_Absyn_AccessEff;
_temp2241.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2241;}); _temp2240;})); _temp2239->tl= 0;
_temp2239;}); _temp2238;}); _temp2237;}); break; case 175: _LL2236: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),( void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2243=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2243[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2244; _temp2244.tag= Cyc_TypeList_tok; _temp2244.f1=({ struct Cyc_List_List*
_temp2245=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2245->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2246=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2246[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2247; _temp2247.tag= Cyc_Absyn_AccessEff;
_temp2247.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2247;}); _temp2246;})); _temp2245->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2245;});
_temp2244;}); _temp2243;}); break; case 176: _LL2242: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2249=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2249[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2250; _temp2250.tag= Cyc_ParamDeclList_tok;
_temp2250.f1=({ struct Cyc_List_List* _temp2251=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2251->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2251->tl= 0;
_temp2251;}); _temp2250;}); _temp2249;}); break; case 177: _LL2248: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2253=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2253[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2254; _temp2254.tag= Cyc_ParamDeclList_tok;
_temp2254.f1=({ struct Cyc_List_List* _temp2255=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2255->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2255->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2255;}); _temp2254;}); _temp2253;}); break; case 178:
_LL2252: { struct _tuple17 _temp2259; struct Cyc_List_List* _temp2260; struct
Cyc_List_List* _temp2262; struct Cyc_Absyn_Tqual _temp2264; struct _tuple17*
_temp2257= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2259=* _temp2257; _LL2265: _temp2264=
_temp2259.f1; goto _LL2263; _LL2263: _temp2262= _temp2259.f2; goto _LL2261;
_LL2261: _temp2260= _temp2259.f3; goto _LL2258; _LL2258: { struct Cyc_Parse_Declarator
_temp2268; struct Cyc_List_List* _temp2269; struct _tuple1* _temp2271; struct
Cyc_Parse_Declarator* _temp2266= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2268=* _temp2266;
_LL2272: _temp2271= _temp2268.id; goto _LL2270; _LL2270: _temp2269= _temp2268.tms;
goto _LL2267; _LL2267: { void* _temp2273= Cyc_Parse_speclist2typ( _temp2262, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2276; struct Cyc_List_List* _temp2278; void*
_temp2280; struct Cyc_Absyn_Tqual _temp2282; struct _tuple6 _temp2274= Cyc_Parse_apply_tms(
_temp2264, _temp2273, _temp2260, _temp2269); _LL2283: _temp2282= _temp2274.f1;
goto _LL2281; _LL2281: _temp2280= _temp2274.f2; goto _LL2279; _LL2279: _temp2278=
_temp2274.f3; goto _LL2277; _LL2277: _temp2276= _temp2274.f4; goto _LL2275;
_LL2275: if( _temp2278 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2271)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2284=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2288=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2288->v=( void*)(* _temp2271).f2; _temp2288;}); if( _temp2276 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2285=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2285[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2286; _temp2286.tag= Cyc_ParamDecl_tok;
_temp2286.f1=({ struct _tuple2* _temp2287=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2287->f1= _temp2284; _temp2287->f2= _temp2282; _temp2287->f3=
_temp2280; _temp2287;}); _temp2286;}); _temp2285;}); break;}}}} case 179:
_LL2256: { struct _tuple17 _temp2292; struct Cyc_List_List* _temp2293; struct
Cyc_List_List* _temp2295; struct Cyc_Absyn_Tqual _temp2297; struct _tuple17*
_temp2290= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2292=* _temp2290; _LL2298: _temp2297= _temp2292.f1;
goto _LL2296; _LL2296: _temp2295= _temp2292.f2; goto _LL2294; _LL2294: _temp2293=
_temp2292.f3; goto _LL2291; _LL2291: { void* _temp2299= Cyc_Parse_speclist2typ(
_temp2295, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2293 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2300=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2300[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2301; _temp2301.tag= Cyc_ParamDecl_tok; _temp2301.f1=({
struct _tuple2* _temp2302=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2302->f1= 0; _temp2302->f2= _temp2297; _temp2302->f3= _temp2299; _temp2302;});
_temp2301;}); _temp2300;}); break;}} case 180: _LL2289: { struct _tuple17
_temp2306; struct Cyc_List_List* _temp2307; struct Cyc_List_List* _temp2309;
struct Cyc_Absyn_Tqual _temp2311; struct _tuple17* _temp2304= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2306=*
_temp2304; _LL2312: _temp2311= _temp2306.f1; goto _LL2310; _LL2310: _temp2309=
_temp2306.f2; goto _LL2308; _LL2308: _temp2307= _temp2306.f3; goto _LL2305;
_LL2305: { void* _temp2313= Cyc_Parse_speclist2typ( _temp2309, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2314=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2317; struct Cyc_List_List* _temp2319; void* _temp2321; struct Cyc_Absyn_Tqual
_temp2323; struct _tuple6 _temp2315= Cyc_Parse_apply_tms( _temp2311, _temp2313,
_temp2307, _temp2314); _LL2324: _temp2323= _temp2315.f1; goto _LL2322; _LL2322:
_temp2321= _temp2315.f2; goto _LL2320; _LL2320: _temp2319= _temp2315.f3; goto
_LL2318; _LL2318: _temp2317= _temp2315.f4; goto _LL2316; _LL2316: if( _temp2319
!=  0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2317 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2325=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2325[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2326; _temp2326.tag= Cyc_ParamDecl_tok; _temp2326.f1=({
struct _tuple2* _temp2327=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2327->f1= 0; _temp2327->f2= _temp2323; _temp2327->f3= _temp2321; _temp2327;});
_temp2326;}); _temp2325;}); break;}} case 181: _LL2303: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2329=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2329[ 0]=({ struct Cyc_IdList_tok_struct
_temp2330; _temp2330.tag= Cyc_IdList_tok; _temp2330.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2330;});
_temp2329;}); break; case 182: _LL2328: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2332=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2332[ 0]=({ struct Cyc_IdList_tok_struct _temp2333; _temp2333.tag= Cyc_IdList_tok;
_temp2333.f1=({ struct Cyc_List_List* _temp2334=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2334->hd=( void*)({ struct
_tagged_arr* _temp2335=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2335[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2335;}); _temp2334->tl=
0; _temp2334;}); _temp2333;}); _temp2332;}); break; case 183: _LL2331: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2337=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2337[ 0]=({ struct Cyc_IdList_tok_struct
_temp2338; _temp2338.tag= Cyc_IdList_tok; _temp2338.f1=({ struct Cyc_List_List*
_temp2339=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2339->hd=( void*)({ struct _tagged_arr* _temp2340=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2340[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2340;});
_temp2339->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2339;}); _temp2338;}); _temp2337;}); break; case 184:
_LL2336: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 185: _LL2341: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 186: _LL2342: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2344=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2344[ 0]=({ struct Cyc_Exp_tok_struct _temp2345; _temp2345.tag= Cyc_Exp_tok;
_temp2345.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2346=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2346[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2347; _temp2347.tag= Cyc_Absyn_UnresolvedMem_e; _temp2347.f1= 0; _temp2347.f2=
0; _temp2347;}); _temp2346;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2345;});
_temp2344;}); break; case 187: _LL2343: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2349=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2349[ 0]=({ struct Cyc_Exp_tok_struct _temp2350; _temp2350.tag= Cyc_Exp_tok;
_temp2350.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2351=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2351[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2352; _temp2352.tag= Cyc_Absyn_UnresolvedMem_e; _temp2352.f1= 0; _temp2352.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2352;});
_temp2351;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2350;}); _temp2349;}); break; case 188:
_LL2348: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2354=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2354[ 0]=({ struct Cyc_Exp_tok_struct
_temp2355; _temp2355.tag= Cyc_Exp_tok; _temp2355.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2356=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2356[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2357; _temp2357.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2357.f1= 0; _temp2357.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2357;});
_temp2356;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2355;}); _temp2354;}); break; case 189:
_LL2353: { struct Cyc_Absyn_Vardecl* _temp2359= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2365=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2365->f1=( void*) Cyc_Absyn_Loc_n; _temp2365->f2=({ struct _tagged_arr*
_temp2366=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp2366[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); _temp2366;}); _temp2365;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp2359->tq=({ struct Cyc_Absyn_Tqual _temp2360; _temp2360.q_const= 1;
_temp2360.q_volatile= 0; _temp2360.q_restrict= 1; _temp2360;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2361=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2361[ 0]=({ struct Cyc_Exp_tok_struct
_temp2362; _temp2362.tag= Cyc_Exp_tok; _temp2362.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2363=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2363[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2364; _temp2364.tag= Cyc_Absyn_Comprehension_e;
_temp2364.f1= _temp2359; _temp2364.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2364.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2364;});
_temp2363;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2362;}); _temp2361;}); break;} case
190: _LL2358: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2368=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2368[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2369; _temp2369.tag=
Cyc_InitializerList_tok; _temp2369.f1=({ struct Cyc_List_List* _temp2370=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2370->hd=(
void*)({ struct _tuple19* _temp2371=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2371->f1= 0; _temp2371->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2371;}); _temp2370->tl=
0; _temp2370;}); _temp2369;}); _temp2368;}); break; case 191: _LL2367: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2373=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2373[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2374; _temp2374.tag= Cyc_InitializerList_tok;
_temp2374.f1=({ struct Cyc_List_List* _temp2375=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2375->hd=( void*)({ struct
_tuple19* _temp2376=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2376->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2376->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2376;});
_temp2375->tl= 0; _temp2375;}); _temp2374;}); _temp2373;}); break; case 192:
_LL2372: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2378=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2378[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2379; _temp2379.tag=
Cyc_InitializerList_tok; _temp2379.f1=({ struct Cyc_List_List* _temp2380=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2380->hd=(
void*)({ struct _tuple19* _temp2381=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2381->f1= 0; _temp2381->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2381;}); _temp2380->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2380;}); _temp2379;}); _temp2378;}); break; case 193:
_LL2377: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2383=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2383[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2384; _temp2384.tag=
Cyc_InitializerList_tok; _temp2384.f1=({ struct Cyc_List_List* _temp2385=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2385->hd=(
void*)({ struct _tuple19* _temp2386=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2386->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2386->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2386;});
_temp2385->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2385;});
_temp2384;}); _temp2383;}); break; case 194: _LL2382: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2388=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2388[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2389; _temp2389.tag= Cyc_DesignatorList_tok;
_temp2389.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2389;});
_temp2388;}); break; case 195: _LL2387: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2391=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2391[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2392; _temp2392.tag=
Cyc_DesignatorList_tok; _temp2392.f1=({ struct Cyc_List_List* _temp2393=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2393->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2393->tl= 0; _temp2393;}); _temp2392;}); _temp2391;});
break; case 196: _LL2390: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2395=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2395[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2396; _temp2396.tag=
Cyc_DesignatorList_tok; _temp2396.f1=({ struct Cyc_List_List* _temp2397=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2397->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2397->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2397;});
_temp2396;}); _temp2395;}); break; case 197: _LL2394: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2399=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2399[ 0]=({ struct
Cyc_Designator_tok_struct _temp2400; _temp2400.tag= Cyc_Designator_tok;
_temp2400.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2401=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2401[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2402; _temp2402.tag=
Cyc_Absyn_ArrayElement; _temp2402.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2402;});
_temp2401;})); _temp2400;}); _temp2399;}); break; case 198: _LL2398: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2404=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2404[ 0]=({ struct
Cyc_Designator_tok_struct _temp2405; _temp2405.tag= Cyc_Designator_tok;
_temp2405.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2406=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2406[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2407; _temp2407.tag= Cyc_Absyn_FieldName;
_temp2407.f1=({ struct _tagged_arr* _temp2408=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2408[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2408;}); _temp2407;});
_temp2406;})); _temp2405;}); _temp2404;}); break; case 199: _LL2403: { void*
_temp2410= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2411=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2411 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2412=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2413=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2413[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2414; _temp2414.tag= Cyc_ParamDecl_tok; _temp2414.f1=({
struct _tuple2* _temp2415=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2415->f1= 0; _temp2415->f2= _temp2412; _temp2415->f3= _temp2410; _temp2415;});
_temp2414;}); _temp2413;}); break;}} case 200: _LL2409: { void* _temp2417= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp2418=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2419=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2420=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2421= Cyc_Parse_apply_tms( _temp2419, _temp2417, _temp2418,
_temp2420); if( _temp2421.f3 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2421.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2422=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2422[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2423; _temp2423.tag= Cyc_ParamDecl_tok; _temp2423.f1=({
struct _tuple2* _temp2424=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2424->f1= 0; _temp2424->f2= _temp2421.f1; _temp2424->f3= _temp2421.f2;
_temp2424;}); _temp2423;}); _temp2422;}); break;} case 201: _LL2416: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2426=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2426[ 0]=({ struct Cyc_Type_tok_struct
_temp2427; _temp2427.tag= Cyc_Type_tok; _temp2427.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2427;});
_temp2426;}); break; case 202: _LL2425: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2429=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2429[ 0]=({ struct Cyc_Type_tok_struct _temp2430; _temp2430.tag= Cyc_Type_tok;
_temp2430.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2431=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2431[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2432; _temp2432.tag= Cyc_Absyn_JoinEff;
_temp2432.f1= 0; _temp2432;}); _temp2431;})); _temp2430;}); _temp2429;}); break;
case 203: _LL2428: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2434=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2434[ 0]=({ struct Cyc_Type_tok_struct _temp2435; _temp2435.tag= Cyc_Type_tok;
_temp2435.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2436=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2436[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2437; _temp2437.tag= Cyc_Absyn_JoinEff;
_temp2437.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2437;}); _temp2436;})); _temp2435;});
_temp2434;}); break; case 204: _LL2433: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2439=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2439[ 0]=({ struct Cyc_Type_tok_struct _temp2440; _temp2440.tag= Cyc_Type_tok;
_temp2440.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2441=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2441[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2442; _temp2442.tag= Cyc_Absyn_RgnsEff;
_temp2442.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2442;}); _temp2441;})); _temp2440;});
_temp2439;}); break; case 205: _LL2438: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2444=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2444[ 0]=({ struct Cyc_Type_tok_struct _temp2445; _temp2445.tag= Cyc_Type_tok;
_temp2445.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2446=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2446[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2447; _temp2447.tag= Cyc_Absyn_JoinEff;
_temp2447.f1=({ struct Cyc_List_List* _temp2448=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2448->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2448->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2448;}); _temp2447;}); _temp2446;})); _temp2445;});
_temp2444;}); break; case 206: _LL2443: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2450=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2450[ 0]=({ struct Cyc_TypeList_tok_struct _temp2451; _temp2451.tag= Cyc_TypeList_tok;
_temp2451.f1=({ struct Cyc_List_List* _temp2452=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2452->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2452->tl= 0;
_temp2452;}); _temp2451;}); _temp2450;}); break; case 207: _LL2449: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2454=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2454[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2455; _temp2455.tag= Cyc_TypeList_tok; _temp2455.f1=({ struct Cyc_List_List*
_temp2456=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2456->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2456->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2456;});
_temp2455;}); _temp2454;}); break; case 208: _LL2453: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2458=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2458[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2459; _temp2459.tag= Cyc_AbstractDeclarator_tok;
_temp2459.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2460=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2460->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2460;});
_temp2459;}); _temp2458;}); break; case 209: _LL2457: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 210: _LL2461:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2463=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2463[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2464; _temp2464.tag=
Cyc_AbstractDeclarator_tok; _temp2464.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2465=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2465->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2465;});
_temp2464;}); _temp2463;}); break; case 211: _LL2462: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 212:
_LL2466: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2468=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2468[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2469; _temp2469.tag=
Cyc_AbstractDeclarator_tok; _temp2469.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2470=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2470->tms=({ struct Cyc_List_List* _temp2471=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2471->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2471->tl= 0; _temp2471;}); _temp2470;}); _temp2469;}); _temp2468;}); break;
case 213: _LL2467: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2473=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2473[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2474; _temp2474.tag= Cyc_AbstractDeclarator_tok; _temp2474.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2475=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2475->tms=({ struct Cyc_List_List* _temp2476=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2476->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2476->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2476;});
_temp2475;}); _temp2474;}); _temp2473;}); break; case 214: _LL2472: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2478=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2478[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2479; _temp2479.tag= Cyc_AbstractDeclarator_tok;
_temp2479.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2480=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2480->tms=({
struct Cyc_List_List* _temp2481=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2481->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2482=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2482[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2483; _temp2483.tag= Cyc_Absyn_ConstArray_mod; _temp2483.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2483;});
_temp2482;})); _temp2481->tl= 0; _temp2481;}); _temp2480;}); _temp2479;});
_temp2478;}); break; case 215: _LL2477: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2485=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2485[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2486; _temp2486.tag= Cyc_AbstractDeclarator_tok; _temp2486.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2487=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2487->tms=({ struct Cyc_List_List* _temp2488=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2488->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2489=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2489[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2490; _temp2490.tag= Cyc_Absyn_ConstArray_mod;
_temp2490.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2490;}); _temp2489;})); _temp2488->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2488;}); _temp2487;}); _temp2486;}); _temp2485;}); break; case 216: _LL2484:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2492=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2492[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2493; _temp2493.tag=
Cyc_AbstractDeclarator_tok; _temp2493.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2494=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2494->tms=({ struct Cyc_List_List* _temp2495=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2495->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2496=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2496[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2497; _temp2497.tag= Cyc_Absyn_Function_mod;
_temp2497.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2498=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2498[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2499; _temp2499.tag= Cyc_Absyn_WithTypes;
_temp2499.f1= 0; _temp2499.f2= 0; _temp2499.f3= 0; _temp2499.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2499.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2499;}); _temp2498;})); _temp2497;}); _temp2496;}));
_temp2495->tl= 0; _temp2495;}); _temp2494;}); _temp2493;}); _temp2492;}); break;
case 217: _LL2491: { struct _tuple16 _temp2503; struct Cyc_List_List* _temp2504;
struct Cyc_Core_Opt* _temp2506; struct Cyc_Absyn_VarargInfo* _temp2508; int
_temp2510; struct Cyc_List_List* _temp2512; struct _tuple16* _temp2501= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2503=*
_temp2501; _LL2513: _temp2512= _temp2503.f1; goto _LL2511; _LL2511: _temp2510=
_temp2503.f2; goto _LL2509; _LL2509: _temp2508= _temp2503.f3; goto _LL2507;
_LL2507: _temp2506= _temp2503.f4; goto _LL2505; _LL2505: _temp2504= _temp2503.f5;
goto _LL2502; _LL2502: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2514=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2514[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2515; _temp2515.tag= Cyc_AbstractDeclarator_tok; _temp2515.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2516=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2516->tms=({ struct Cyc_List_List* _temp2517=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2517->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2518=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2518[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2519; _temp2519.tag= Cyc_Absyn_Function_mod;
_temp2519.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2520=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2520[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2521; _temp2521.tag= Cyc_Absyn_WithTypes;
_temp2521.f1= _temp2512; _temp2521.f2= _temp2510; _temp2521.f3= _temp2508;
_temp2521.f4= _temp2506; _temp2521.f5= _temp2504; _temp2521;}); _temp2520;}));
_temp2519;}); _temp2518;})); _temp2517->tl= 0; _temp2517;}); _temp2516;});
_temp2515;}); _temp2514;}); break;} case 218: _LL2500: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2523=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2523[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2524; _temp2524.tag= Cyc_AbstractDeclarator_tok;
_temp2524.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2525=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2525->tms=({
struct Cyc_List_List* _temp2526=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2526->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2527=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2527[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2528; _temp2528.tag=
Cyc_Absyn_Function_mod; _temp2528.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2529=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2529[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2530; _temp2530.tag= Cyc_Absyn_WithTypes;
_temp2530.f1= 0; _temp2530.f2= 0; _temp2530.f3= 0; _temp2530.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2530.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2530;}); _temp2529;})); _temp2528;}); _temp2527;}));
_temp2526->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2526;});
_temp2525;}); _temp2524;}); _temp2523;}); break; case 219: _LL2522: { struct
_tuple16 _temp2534; struct Cyc_List_List* _temp2535; struct Cyc_Core_Opt*
_temp2537; struct Cyc_Absyn_VarargInfo* _temp2539; int _temp2541; struct Cyc_List_List*
_temp2543; struct _tuple16* _temp2532= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2534=*
_temp2532; _LL2544: _temp2543= _temp2534.f1; goto _LL2542; _LL2542: _temp2541=
_temp2534.f2; goto _LL2540; _LL2540: _temp2539= _temp2534.f3; goto _LL2538;
_LL2538: _temp2537= _temp2534.f4; goto _LL2536; _LL2536: _temp2535= _temp2534.f5;
goto _LL2533; _LL2533: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2545=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2545[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2546; _temp2546.tag= Cyc_AbstractDeclarator_tok; _temp2546.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2547=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2547->tms=({ struct Cyc_List_List* _temp2548=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2548->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2549=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2549[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2550; _temp2550.tag= Cyc_Absyn_Function_mod;
_temp2550.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2551=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2551[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2552; _temp2552.tag= Cyc_Absyn_WithTypes;
_temp2552.f1= _temp2543; _temp2552.f2= _temp2541; _temp2552.f3= _temp2539;
_temp2552.f4= _temp2537; _temp2552.f5= _temp2535; _temp2552;}); _temp2551;}));
_temp2550;}); _temp2549;})); _temp2548->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2548;}); _temp2547;}); _temp2546;}); _temp2545;}); break;} case 220:
_LL2531: { struct Cyc_List_List* _temp2554=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2555=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2555[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2556; _temp2556.tag= Cyc_AbstractDeclarator_tok; _temp2556.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2557=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2557->tms=({ struct Cyc_List_List* _temp2558=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2558->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2559=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2559[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2560; _temp2560.tag= Cyc_Absyn_TypeParams_mod;
_temp2560.f1= _temp2554; _temp2560.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2560.f3=
0; _temp2560;}); _temp2559;})); _temp2558->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2558;}); _temp2557;}); _temp2556;}); _temp2555;}); break;} case 221:
_LL2553:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2562=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2563=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2563[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2564; _temp2564.tag= Cyc_AbstractDeclarator_tok; _temp2564.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2565=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2565->tms=({ struct Cyc_List_List* _temp2566=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2566->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2567=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2567[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2568; _temp2568.tag= Cyc_Absyn_TypeParams_mod;
_temp2568.f1= _temp2562; _temp2568.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2568.f3=
0; _temp2568;}); _temp2567;})); _temp2566->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2566;}); _temp2565;}); _temp2564;}); _temp2563;}); break;} case 222:
_LL2561: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2570=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2570[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2571; _temp2571.tag=
Cyc_AbstractDeclarator_tok; _temp2571.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2572=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2572->tms=({ struct Cyc_List_List* _temp2573=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2573->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2574=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2574[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2575; _temp2575.tag= Cyc_Absyn_Attributes_mod;
_temp2575.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2575.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2575;}); _temp2574;}));
_temp2573->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms; _temp2573;});
_temp2572;}); _temp2571;}); _temp2570;}); break; case 223: _LL2569: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 224: _LL2576:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
225: _LL2577: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 226: _LL2578: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 227: _LL2579: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 228: _LL2580:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
229: _LL2581: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2593=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2593->name=({ struct
_tagged_arr* _temp2594=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2594[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2594;});
_temp2593->identity= 0; _temp2593->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2593;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2591=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2591[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2592; _temp2592.tag= Cyc_Absyn_VarType;
_temp2592.f1= tv; _temp2592;}); _temp2591;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2583=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2583[ 0]=({ struct Cyc_Stmt_tok_struct _temp2584; _temp2584.tag= Cyc_Stmt_tok;
_temp2584.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2585=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2585[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2586; _temp2586.tag= Cyc_Absyn_Region_s;
_temp2586.f1= tv; _temp2586.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2587=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2587->f1=(
void*) Cyc_Absyn_Loc_n; _temp2587->f2=({ struct _tagged_arr* _temp2588=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2588[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2588;});
_temp2587;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2589=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2589[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2590; _temp2590.tag=
Cyc_Absyn_RgnHandleType; _temp2590.f1=( void*) t; _temp2590;}); _temp2589;}), 0);
_temp2586.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2586;}); _temp2585;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2584;});
_temp2583;}); break;} case 230: _LL2582: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2606=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2606->name=({ struct
_tagged_arr* _temp2607=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2607[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2609; _temp2609.tag= Cyc_Std_String_pa; _temp2609.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2608[ 1u]={& _temp2609}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2608, sizeof( void*), 1u));}});
_temp2607;}); _temp2606->identity= 0; _temp2606->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2606;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2604=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2604[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2605; _temp2605.tag= Cyc_Absyn_VarType;
_temp2605.f1= tv; _temp2605;}); _temp2604;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2596=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2596[ 0]=({ struct Cyc_Stmt_tok_struct _temp2597; _temp2597.tag= Cyc_Stmt_tok;
_temp2597.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2598=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2598[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2599; _temp2599.tag= Cyc_Absyn_Region_s;
_temp2599.f1= tv; _temp2599.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2600=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2600->f1=(
void*) Cyc_Absyn_Loc_n; _temp2600->f2=({ struct _tagged_arr* _temp2601=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2601[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2601;});
_temp2600;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2602=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2602[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2603; _temp2603.tag=
Cyc_Absyn_RgnHandleType; _temp2603.f1=( void*) t; _temp2603;}); _temp2602;}), 0);
_temp2599.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2599;}); _temp2598;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2597;});
_temp2596;}); break;} case 231: _LL2595: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2611=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2611[ 0]=({ struct Cyc_Stmt_tok_struct _temp2612; _temp2612.tag= Cyc_Stmt_tok;
_temp2612.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2613=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2613[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2614; _temp2614.tag= Cyc_Absyn_Cut_s;
_temp2614.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2614;}); _temp2613;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2612;});
_temp2611;}); break; case 232: _LL2610: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2616=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2616[ 0]=({ struct Cyc_Stmt_tok_struct _temp2617; _temp2617.tag= Cyc_Stmt_tok;
_temp2617.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2618=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2618[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2619; _temp2619.tag= Cyc_Absyn_Splice_s;
_temp2619.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2619;}); _temp2618;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2617;});
_temp2616;}); break; case 233: _LL2615: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2621=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2621[ 0]=({ struct Cyc_Stmt_tok_struct _temp2622; _temp2622.tag= Cyc_Stmt_tok;
_temp2622.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2623=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2623[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2624; _temp2624.tag= Cyc_Absyn_Label_s;
_temp2624.f1=({ struct _tagged_arr* _temp2625=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2625[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2625;});
_temp2624.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2624;}); _temp2623;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2622;});
_temp2621;}); break; case 234: _LL2620: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2627=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2627[ 0]=({ struct Cyc_Stmt_tok_struct _temp2628; _temp2628.tag= Cyc_Stmt_tok;
_temp2628.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2628;});
_temp2627;}); break; case 235: _LL2626: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2630=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2630[ 0]=({ struct Cyc_Stmt_tok_struct _temp2631; _temp2631.tag= Cyc_Stmt_tok;
_temp2631.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2631;}); _temp2630;}); break; case 236: _LL2629: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2633=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2633[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2634; _temp2634.tag= Cyc_Stmt_tok; _temp2634.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2634;}); _temp2633;}); break; case 237: _LL2632: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 238:
_LL2635: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2637=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2637[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2638; _temp2638.tag= Cyc_Stmt_tok; _temp2638.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2638;}); _temp2637;}); break;
case 239: _LL2636: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2640=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2640[ 0]=({ struct Cyc_Stmt_tok_struct _temp2641; _temp2641.tag= Cyc_Stmt_tok;
_temp2641.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2641;});
_temp2640;}); break; case 240: _LL2639: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 241: _LL2642:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2644=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2644[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2645; _temp2645.tag= Cyc_Stmt_tok; _temp2645.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2645;}); _temp2644;}); break; case 242: _LL2643: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2647=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2647[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2648; _temp2648.tag= Cyc_Stmt_tok; _temp2648.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2649=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2649[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2650; _temp2650.tag= Cyc_Absyn_Fn_d;
_temp2650.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2650;}); _temp2649;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2648;}); _temp2647;}); break; case 243: _LL2646: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2652=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2652[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2653; _temp2653.tag= Cyc_Stmt_tok; _temp2653.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2654=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2654[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2655; _temp2655.tag= Cyc_Absyn_Fn_d;
_temp2655.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2655;}); _temp2654;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2653;}); _temp2652;}); break; case 244: _LL2651: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2657=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2657[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2658; _temp2658.tag= Cyc_Stmt_tok; _temp2658.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2658;});
_temp2657;}); break; case 245: _LL2656: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2660=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2660[ 0]=({ struct Cyc_Stmt_tok_struct _temp2661; _temp2661.tag= Cyc_Stmt_tok;
_temp2661.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2661;}); _temp2660;}); break; case 246: _LL2659: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2663=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2663[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2664; _temp2664.tag= Cyc_Stmt_tok; _temp2664.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2664;}); _temp2663;}); break; case 247: _LL2662: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2666=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2666[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2667; _temp2667.tag= Cyc_Stmt_tok; _temp2667.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2668=( struct Cyc_Absyn_SwitchC_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2668[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2669; _temp2669.tag= Cyc_Absyn_SwitchC_s;
_temp2669.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2669.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2669;});
_temp2668;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2667;}); _temp2666;}); break; case 248:
_LL2665: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2671=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2671[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2672; _temp2672.tag= Cyc_Stmt_tok; _temp2672.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2672;});
_temp2671;}); break; case 249: _LL2670: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2674=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2674[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2675; _temp2675.tag= Cyc_SwitchClauseList_tok; _temp2675.f1= 0; _temp2675;});
_temp2674;}); break; case 250: _LL2673: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2677=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2677[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2678; _temp2678.tag= Cyc_SwitchClauseList_tok; _temp2678.f1=({ struct Cyc_List_List*
_temp2679=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2679->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2680=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2680->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2680->pat_vars= 0; _temp2680->where_clause= 0; _temp2680->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2680->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2680;}); _temp2679->tl= 0; _temp2679;});
_temp2678;}); _temp2677;}); break; case 251: _LL2676: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2682=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2682[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2683; _temp2683.tag= Cyc_SwitchClauseList_tok;
_temp2683.f1=({ struct Cyc_List_List* _temp2684=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2684->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2685=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2685->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2685->pat_vars= 0; _temp2685->where_clause= 0;
_temp2685->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2685->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2685;});
_temp2684->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2684;}); _temp2683;});
_temp2682;}); break; case 252: _LL2681: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2687=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2687[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2688; _temp2688.tag= Cyc_SwitchClauseList_tok; _temp2688.f1=({ struct Cyc_List_List*
_temp2689=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2689->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2690=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2690->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2690->pat_vars=
0; _temp2690->where_clause= 0; _temp2690->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2690->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2690;}); _temp2689->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2689;});
_temp2688;}); _temp2687;}); break; case 253: _LL2686: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2692=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2692[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2693; _temp2693.tag= Cyc_SwitchClauseList_tok;
_temp2693.f1=({ struct Cyc_List_List* _temp2694=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2694->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2695=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2695->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp2695->pat_vars= 0; _temp2695->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2695->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2695->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2695;});
_temp2694->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2694;}); _temp2693;});
_temp2692;}); break; case 254: _LL2691: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2697=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2697[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2698; _temp2698.tag= Cyc_SwitchClauseList_tok; _temp2698.f1=({ struct Cyc_List_List*
_temp2699=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2699->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2700=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2700->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2700->pat_vars=
0; _temp2700->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2700->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2700->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2700;});
_temp2699->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2699;}); _temp2698;});
_temp2697;}); break; case 255: _LL2696: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2702=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2702[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2703; _temp2703.tag= Cyc_SwitchCClauseList_tok; _temp2703.f1= 0; _temp2703;});
_temp2702;}); break; case 256: _LL2701: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2705=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2705[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2706; _temp2706.tag= Cyc_SwitchCClauseList_tok; _temp2706.f1=({ struct Cyc_List_List*
_temp2707=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2707->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2708=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2708->cnst_exp= 0;
_temp2708->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2708->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2708;});
_temp2707->tl= 0; _temp2707;}); _temp2706;}); _temp2705;}); break; case 257:
_LL2704: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2710=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2710[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2711; _temp2711.tag=
Cyc_SwitchCClauseList_tok; _temp2711.f1=({ struct Cyc_List_List* _temp2712=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2712->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2713=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2713->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2713->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2713->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2713;});
_temp2712->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2712;}); _temp2711;});
_temp2710;}); break; case 258: _LL2709: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2715=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2715[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2716; _temp2716.tag= Cyc_SwitchCClauseList_tok; _temp2716.f1=({ struct Cyc_List_List*
_temp2717=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2717->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2718=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2718->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2718->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2718->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2718;});
_temp2717->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2717;}); _temp2716;});
_temp2715;}); break; case 259: _LL2714: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2720=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2720[ 0]=({ struct Cyc_Stmt_tok_struct _temp2721; _temp2721.tag= Cyc_Stmt_tok;
_temp2721.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2721;}); _temp2720;}); break; case 260: _LL2719: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2723=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2723[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2724; _temp2724.tag= Cyc_Stmt_tok; _temp2724.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2724;}); _temp2723;}); break; case 261: _LL2722: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2726=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2726[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2727; _temp2727.tag= Cyc_Stmt_tok; _temp2727.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2727;}); _temp2726;}); break; case 262: _LL2725: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2729=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2729[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2730; _temp2730.tag= Cyc_Stmt_tok; _temp2730.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2730;}); _temp2729;}); break; case 263: _LL2728: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2732=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2732[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2733; _temp2733.tag= Cyc_Stmt_tok; _temp2733.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2733;}); _temp2732;}); break; case 264: _LL2731: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2735=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2735[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2736; _temp2736.tag= Cyc_Stmt_tok; _temp2736.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2736;}); _temp2735;}); break; case 265: _LL2734: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2738=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2738[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2739; _temp2739.tag= Cyc_Stmt_tok; _temp2739.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2739;}); _temp2738;}); break; case 266: _LL2737: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2741=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2741[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2742; _temp2742.tag= Cyc_Stmt_tok; _temp2742.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2742;});
_temp2741;}); break; case 267: _LL2740: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2744=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2744[ 0]=({ struct Cyc_Stmt_tok_struct _temp2745; _temp2745.tag= Cyc_Stmt_tok;
_temp2745.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2745;});
_temp2744;}); break; case 268: _LL2743: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2747=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2747[ 0]=({ struct Cyc_Stmt_tok_struct _temp2748; _temp2748.tag= Cyc_Stmt_tok;
_temp2748.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2748;}); _temp2747;}); break; case 269: _LL2746: { struct Cyc_List_List*
_temp2750= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt* _temp2751= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2752=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2752[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2753; _temp2753.tag= Cyc_Stmt_tok; _temp2753.f1= Cyc_Parse_flatten_declarations(
_temp2750, _temp2751); _temp2753;}); _temp2752;}); break;} case 270: _LL2749: {
struct Cyc_List_List* _temp2755= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2756= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2757=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2757[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2758; _temp2758.tag= Cyc_Stmt_tok; _temp2758.f1= Cyc_Parse_flatten_declarations(
_temp2755, _temp2756); _temp2758;}); _temp2757;}); break;} case 271: _LL2754: {
struct Cyc_List_List* _temp2760= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2761= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2762=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2762[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2763; _temp2763.tag= Cyc_Stmt_tok; _temp2763.f1= Cyc_Parse_flatten_declarations(
_temp2760, _temp2761); _temp2763;}); _temp2762;}); break;} case 272: _LL2759: {
struct Cyc_List_List* _temp2765= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2766= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2767=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2767[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2768; _temp2768.tag= Cyc_Stmt_tok; _temp2768.f1= Cyc_Parse_flatten_declarations(
_temp2765, _temp2766); _temp2768;}); _temp2767;}); break;} case 273: _LL2764:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2770=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2770[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2771; _temp2771.tag= Cyc_Stmt_tok; _temp2771.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2772=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2772[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2772;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2771;}); _temp2770;}); break; case 274: _LL2769: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2774=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2774[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2775; _temp2775.tag= Cyc_Stmt_tok; _temp2775.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2775;}); _temp2774;}); break; case 275:
_LL2773: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2777=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2777[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2778; _temp2778.tag= Cyc_Stmt_tok; _temp2778.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2778;}); _temp2777;}); break; case 276: _LL2776: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2780=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2780[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2781; _temp2781.tag= Cyc_Stmt_tok; _temp2781.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2781;}); _temp2780;}); break; case 277:
_LL2779: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2783=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2783[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2784; _temp2784.tag= Cyc_Stmt_tok; _temp2784.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2784;}); _temp2783;}); break; case 278: _LL2782: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2786=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2786[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2787; _temp2787.tag= Cyc_Stmt_tok; _temp2787.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2787;}); _temp2786;}); break; case 279:
_LL2785: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2789=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2789[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2790; _temp2790.tag= Cyc_Stmt_tok; _temp2790.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2790;}); _temp2789;}); break; case 280:
_LL2788: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2792=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2792[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2793; _temp2793.tag= Cyc_Stmt_tok; _temp2793.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2793;}); _temp2792;}); break; case 281: _LL2791: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2795=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2795[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2796; _temp2796.tag= Cyc_Pattern_tok; _temp2796.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2796;});
_temp2795;}); break; case 282: _LL2794: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 283:
_LL2797: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2799=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2799[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2800; _temp2800.tag= Cyc_Pattern_tok; _temp2800.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2801=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2801[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2802; _temp2802.tag= Cyc_Absyn_Int_p; _temp2802.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2802.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2802;}); _temp2801;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2800;});
_temp2799;}); break; case 284: _LL2798: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2804=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2804[ 0]=({ struct Cyc_Pattern_tok_struct _temp2805; _temp2805.tag= Cyc_Pattern_tok;
_temp2805.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2806=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2806[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2807; _temp2807.tag= Cyc_Absyn_Int_p;
_temp2807.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2807.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2807;});
_temp2806;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2805;}); _temp2804;}); break; case 285:
_LL2803: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2809=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2809[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2810; _temp2810.tag= Cyc_Pattern_tok; _temp2810.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2811=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2811[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2812; _temp2812.tag= Cyc_Absyn_Float_p; _temp2812.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2812;}); _temp2811;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2810;});
_temp2809;}); break; case 286: _LL2808: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2814=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2814[ 0]=({ struct Cyc_Pattern_tok_struct _temp2815; _temp2815.tag= Cyc_Pattern_tok;
_temp2815.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2816=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2816[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2817; _temp2817.tag= Cyc_Absyn_Char_p; _temp2817.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2817;});
_temp2816;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2815;}); _temp2814;}); break; case 287:
_LL2813: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2819=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2819[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2820; _temp2820.tag= Cyc_Pattern_tok; _temp2820.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2820;});
_temp2819;}); break; case 288: _LL2818: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2822=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2822[ 0]=({ struct Cyc_Pattern_tok_struct _temp2823; _temp2823.tag= Cyc_Pattern_tok;
_temp2823.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2824=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2824[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2825; _temp2825.tag=
Cyc_Absyn_UnknownId_p; _temp2825.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2825;}); _temp2824;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2823;}); _temp2822;}); break; case 289:
_LL2821: { struct Cyc_List_List* _temp2827=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2828=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2828[ 0]=({ struct Cyc_Pattern_tok_struct _temp2829; _temp2829.tag= Cyc_Pattern_tok;
_temp2829.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2830=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2830[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2831; _temp2831.tag= Cyc_Absyn_UnknownCall_p; _temp2831.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2831.f2=
_temp2827; _temp2831.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2831;});
_temp2830;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2829;}); _temp2828;}); break;} case
290: _LL2826: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2833=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2833[ 0]=({ struct Cyc_Pattern_tok_struct _temp2834; _temp2834.tag= Cyc_Pattern_tok;
_temp2834.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2835=( struct Cyc_Absyn_Tuple_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2835[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2836; _temp2836.tag= Cyc_Absyn_Tuple_p;
_temp2836.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2836;}); _temp2835;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2834;}); _temp2833;}); break; case 291: _LL2832: { struct Cyc_List_List*
_temp2838=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); yyval=(
void*)({ struct Cyc_Pattern_tok_struct* _temp2839=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2839[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2840; _temp2840.tag= Cyc_Pattern_tok; _temp2840.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2841=( struct Cyc_Absyn_UnknownFields_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2841[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2842; _temp2842.tag= Cyc_Absyn_UnknownFields_p;
_temp2842.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2842.f2= _temp2838; _temp2842.f3= 0; _temp2842;});
_temp2841;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2840;}); _temp2839;}); break;} case
292: _LL2837: { struct Cyc_List_List* _temp2844=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2845=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2845[ 0]=({ struct Cyc_Pattern_tok_struct _temp2846; _temp2846.tag= Cyc_Pattern_tok;
_temp2846.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2847=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2847[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2848; _temp2848.tag= Cyc_Absyn_UnknownFields_p; _temp2848.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2848.f2=
_temp2844; _temp2848.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2848;});
_temp2847;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2846;}); _temp2845;}); break;} case
293: _LL2843: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2850=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2850[ 0]=({ struct Cyc_Pattern_tok_struct _temp2851; _temp2851.tag= Cyc_Pattern_tok;
_temp2851.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2852=( struct Cyc_Absyn_Pointer_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2852[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2853; _temp2853.tag= Cyc_Absyn_Pointer_p;
_temp2853.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2853;}); _temp2852;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2851;});
_temp2850;}); break; case 294: _LL2849: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2855=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2855[ 0]=({ struct Cyc_Pattern_tok_struct _temp2856; _temp2856.tag= Cyc_Pattern_tok;
_temp2856.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2857=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2857[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2858; _temp2858.tag=
Cyc_Absyn_Reference_p; _temp2858.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2859=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2859->f1=(
void*) Cyc_Absyn_Loc_n; _temp2859->f2=({ struct _tagged_arr* _temp2860=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2860[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2860;});
_temp2859;}),( void*) Cyc_Absyn_VoidType, 0); _temp2858;}); _temp2857;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2856;}); _temp2855;}); break; case 295: _LL2854: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2862=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2862[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2863; _temp2863.tag= Cyc_PatternList_tok;
_temp2863.f1= 0; _temp2863;}); _temp2862;}); break; case 296: _LL2861: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2865=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2865[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2866; _temp2866.tag= Cyc_PatternList_tok;
_temp2866.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2866;}); _temp2865;}); break; case 297: _LL2864: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2868=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2868[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2869; _temp2869.tag= Cyc_PatternList_tok;
_temp2869.f1=({ struct Cyc_List_List* _temp2870=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2870->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2870->tl= 0;
_temp2870;}); _temp2869;}); _temp2868;}); break; case 298: _LL2867: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2872=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2872[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2873; _temp2873.tag= Cyc_PatternList_tok;
_temp2873.f1=({ struct Cyc_List_List* _temp2874=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2874->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2874->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2874;}); _temp2873;}); _temp2872;}); break; case 299:
_LL2871: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2876=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2876[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2877; _temp2877.tag=
Cyc_FieldPattern_tok; _temp2877.f1=({ struct _tuple13* _temp2878=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2878->f1= 0; _temp2878->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2878;}); _temp2877;}); _temp2876;}); break; case 300:
_LL2875: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2880=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2880[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2881; _temp2881.tag=
Cyc_FieldPattern_tok; _temp2881.f1=({ struct _tuple13* _temp2882=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2882->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2882->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2882;}); _temp2881;}); _temp2880;}); break; case 301:
_LL2879: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2884=(
struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2884[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2885; _temp2885.tag=
Cyc_FieldPatternList_tok; _temp2885.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2885;});
_temp2884;}); break; case 302: _LL2883: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2887=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2887[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2888; _temp2888.tag= Cyc_FieldPatternList_tok; _temp2888.f1=({ struct Cyc_List_List*
_temp2889=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2889->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2889->tl= 0;
_temp2889;}); _temp2888;}); _temp2887;}); break; case 303: _LL2886: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2891=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2891[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2892; _temp2892.tag= Cyc_FieldPatternList_tok;
_temp2892.f1=({ struct Cyc_List_List* _temp2893=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2893->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2893->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2893;}); _temp2892;}); _temp2891;}); break; case 304:
_LL2890: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 305: _LL2894: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2896=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2896[ 0]=({ struct Cyc_Exp_tok_struct _temp2897; _temp2897.tag= Cyc_Exp_tok;
_temp2897.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2897;}); _temp2896;}); break; case 306: _LL2895: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 307: _LL2898:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2900=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2900[ 0]=({ struct Cyc_Exp_tok_struct
_temp2901; _temp2901.tag= Cyc_Exp_tok; _temp2901.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2901;}); _temp2900;}); break; case 308: _LL2899: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2903=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2903[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2904; _temp2904.tag= Cyc_Primopopt_tok; _temp2904.f1=
0; _temp2904;}); _temp2903;}); break; case 309: _LL2902: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2906=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2906[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2907; _temp2907.tag= Cyc_Primopopt_tok; _temp2907.f1=({
struct Cyc_Core_Opt* _temp2908=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2908->v=( void*)(( void*) Cyc_Absyn_Times); _temp2908;});
_temp2907;}); _temp2906;}); break; case 310: _LL2905: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2910=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2910[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2911; _temp2911.tag= Cyc_Primopopt_tok; _temp2911.f1=({
struct Cyc_Core_Opt* _temp2912=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2912->v=( void*)(( void*) Cyc_Absyn_Div); _temp2912;});
_temp2911;}); _temp2910;}); break; case 311: _LL2909: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2914=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2914[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2915; _temp2915.tag= Cyc_Primopopt_tok; _temp2915.f1=({
struct Cyc_Core_Opt* _temp2916=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2916->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2916;});
_temp2915;}); _temp2914;}); break; case 312: _LL2913: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2918=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2918[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2919; _temp2919.tag= Cyc_Primopopt_tok; _temp2919.f1=({
struct Cyc_Core_Opt* _temp2920=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2920->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2920;});
_temp2919;}); _temp2918;}); break; case 313: _LL2917: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2922=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2922[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2923; _temp2923.tag= Cyc_Primopopt_tok; _temp2923.f1=({
struct Cyc_Core_Opt* _temp2924=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2924->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2924;});
_temp2923;}); _temp2922;}); break; case 314: _LL2921: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2926=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2926[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2927; _temp2927.tag= Cyc_Primopopt_tok; _temp2927.f1=({
struct Cyc_Core_Opt* _temp2928=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2928->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2928;});
_temp2927;}); _temp2926;}); break; case 315: _LL2925: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2930=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2930[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2931; _temp2931.tag= Cyc_Primopopt_tok; _temp2931.f1=({
struct Cyc_Core_Opt* _temp2932=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2932->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2932;});
_temp2931;}); _temp2930;}); break; case 316: _LL2929: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2934=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2934[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2935; _temp2935.tag= Cyc_Primopopt_tok; _temp2935.f1=({
struct Cyc_Core_Opt* _temp2936=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2936->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2936;});
_temp2935;}); _temp2934;}); break; case 317: _LL2933: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2938=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2938[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2939; _temp2939.tag= Cyc_Primopopt_tok; _temp2939.f1=({
struct Cyc_Core_Opt* _temp2940=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2940->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2940;});
_temp2939;}); _temp2938;}); break; case 318: _LL2937: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2942=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2942[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2943; _temp2943.tag= Cyc_Primopopt_tok; _temp2943.f1=({
struct Cyc_Core_Opt* _temp2944=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2944->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2944;});
_temp2943;}); _temp2942;}); break; case 319: _LL2941: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 320: _LL2945:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2947=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2947[ 0]=({ struct Cyc_Exp_tok_struct
_temp2948; _temp2948.tag= Cyc_Exp_tok; _temp2948.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2948;});
_temp2947;}); break; case 321: _LL2946: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2950=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2950[ 0]=({ struct Cyc_Exp_tok_struct _temp2951; _temp2951.tag= Cyc_Exp_tok;
_temp2951.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2951;}); _temp2950;}); break; case 322: _LL2949: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2953=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2953[ 0]=({ struct Cyc_Exp_tok_struct
_temp2954; _temp2954.tag= Cyc_Exp_tok; _temp2954.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2954;}); _temp2953;}); break; case 323: _LL2952: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2956=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2956[ 0]=({ struct Cyc_Exp_tok_struct
_temp2957; _temp2957.tag= Cyc_Exp_tok; _temp2957.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2957;}); _temp2956;}); break; case 324: _LL2955: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2959=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2959[ 0]=({ struct Cyc_Exp_tok_struct
_temp2960; _temp2960.tag= Cyc_Exp_tok; _temp2960.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2960;});
_temp2959;}); break; case 325: _LL2958: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2962=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2962[ 0]=({ struct Cyc_Exp_tok_struct _temp2963; _temp2963.tag= Cyc_Exp_tok;
_temp2963.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2963;}); _temp2962;}); break; case 326: _LL2961: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 327: _LL2964:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
328: _LL2965: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2967=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2967[ 0]=({ struct Cyc_Exp_tok_struct
_temp2968; _temp2968.tag= Cyc_Exp_tok; _temp2968.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2968;}); _temp2967;}); break; case 329: _LL2966: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 330: _LL2969:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2971=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2971[ 0]=({ struct Cyc_Exp_tok_struct
_temp2972; _temp2972.tag= Cyc_Exp_tok; _temp2972.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2972;}); _temp2971;}); break; case 331: _LL2970: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 332: _LL2973:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2975=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2975[ 0]=({ struct Cyc_Exp_tok_struct
_temp2976; _temp2976.tag= Cyc_Exp_tok; _temp2976.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2976;});
_temp2975;}); break; case 333: _LL2974: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 334: _LL2977:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2979=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2979[ 0]=({ struct Cyc_Exp_tok_struct
_temp2980; _temp2980.tag= Cyc_Exp_tok; _temp2980.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2980;});
_temp2979;}); break; case 335: _LL2978: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 336: _LL2981:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2983=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2983[ 0]=({ struct Cyc_Exp_tok_struct
_temp2984; _temp2984.tag= Cyc_Exp_tok; _temp2984.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2984;});
_temp2983;}); break; case 337: _LL2982: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 338: _LL2985:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2987=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2987[ 0]=({ struct Cyc_Exp_tok_struct
_temp2988; _temp2988.tag= Cyc_Exp_tok; _temp2988.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2988;}); _temp2987;}); break; case 339: _LL2986: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2990=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2990[ 0]=({ struct Cyc_Exp_tok_struct
_temp2991; _temp2991.tag= Cyc_Exp_tok; _temp2991.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2991;}); _temp2990;}); break; case 340: _LL2989: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 341: _LL2992:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2994=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2994[ 0]=({ struct Cyc_Exp_tok_struct
_temp2995; _temp2995.tag= Cyc_Exp_tok; _temp2995.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2995;}); _temp2994;}); break; case 342: _LL2993: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2997=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2997[ 0]=({ struct Cyc_Exp_tok_struct
_temp2998; _temp2998.tag= Cyc_Exp_tok; _temp2998.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2998;}); _temp2997;}); break; case 343: _LL2996: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3000=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3000[ 0]=({ struct Cyc_Exp_tok_struct
_temp3001; _temp3001.tag= Cyc_Exp_tok; _temp3001.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3001;}); _temp3000;}); break; case 344: _LL2999: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3003=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3003[ 0]=({ struct Cyc_Exp_tok_struct
_temp3004; _temp3004.tag= Cyc_Exp_tok; _temp3004.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3004;}); _temp3003;}); break; case 345: _LL3002: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 346: _LL3005:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3007=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3007[ 0]=({ struct Cyc_Exp_tok_struct
_temp3008; _temp3008.tag= Cyc_Exp_tok; _temp3008.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3008;}); _temp3007;}); break; case 347: _LL3006: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3010=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3010[ 0]=({ struct Cyc_Exp_tok_struct
_temp3011; _temp3011.tag= Cyc_Exp_tok; _temp3011.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3011;}); _temp3010;}); break; case 348: _LL3009: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 349: _LL3012:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3014=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3014[ 0]=({ struct Cyc_Exp_tok_struct
_temp3015; _temp3015.tag= Cyc_Exp_tok; _temp3015.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3015;});
_temp3014;}); break; case 350: _LL3013: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3017=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3017[ 0]=({ struct Cyc_Exp_tok_struct _temp3018; _temp3018.tag= Cyc_Exp_tok;
_temp3018.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3018;}); _temp3017;}); break; case 351: _LL3016: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 352: _LL3019:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3021=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3021[ 0]=({ struct Cyc_Exp_tok_struct
_temp3022; _temp3022.tag= Cyc_Exp_tok; _temp3022.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3022;});
_temp3021;}); break; case 353: _LL3020: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3024=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3024[ 0]=({ struct Cyc_Exp_tok_struct _temp3025; _temp3025.tag= Cyc_Exp_tok;
_temp3025.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3025;}); _temp3024;}); break; case 354: _LL3023: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3027=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3027[ 0]=({ struct Cyc_Exp_tok_struct
_temp3028; _temp3028.tag= Cyc_Exp_tok; _temp3028.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3028;});
_temp3027;}); break; case 355: _LL3026: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 356: _LL3029:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3031=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3031[ 0]=({ struct Cyc_Exp_tok_struct
_temp3032; _temp3032.tag= Cyc_Exp_tok; _temp3032.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3032;}); _temp3031;}); break; case 357: _LL3030: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 358: _LL3033:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3035=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3035[ 0]=({ struct Cyc_Exp_tok_struct
_temp3036; _temp3036.tag= Cyc_Exp_tok; _temp3036.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3036;}); _temp3035;}); break; case 359: _LL3034: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3038=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3038[ 0]=({ struct Cyc_Exp_tok_struct
_temp3039; _temp3039.tag= Cyc_Exp_tok; _temp3039.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3039;}); _temp3038;}); break; case 360: _LL3037: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3041=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3041[ 0]=({ struct Cyc_Exp_tok_struct
_temp3042; _temp3042.tag= Cyc_Exp_tok; _temp3042.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3042;}); _temp3041;}); break; case 361: _LL3040: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3044=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3044[ 0]=({ struct Cyc_Exp_tok_struct
_temp3045; _temp3045.tag= Cyc_Exp_tok; _temp3045.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3045;}); _temp3044;}); break; case 362: _LL3043: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 363: _LL3046:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3048=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3048[ 0]=({ struct Cyc_Exp_tok_struct
_temp3049; _temp3049.tag= Cyc_Exp_tok; _temp3049.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3049;}); _temp3048;}); break; case 364: _LL3047: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3051=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3051[ 0]=({ struct Cyc_Exp_tok_struct
_temp3052; _temp3052.tag= Cyc_Exp_tok; _temp3052.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3052;});
_temp3051;}); break; case 365: _LL3050: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3054=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3054[ 0]=({ struct Cyc_Exp_tok_struct _temp3055; _temp3055.tag= Cyc_Exp_tok;
_temp3055.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3055;}); _temp3054;}); break; case 366: _LL3053: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3057=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3057[ 0]=({ struct Cyc_Exp_tok_struct
_temp3058; _temp3058.tag= Cyc_Exp_tok; _temp3058.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_StructField_struct*
_temp3059=( struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp3059[ 0]=({ struct Cyc_Absyn_StructField_struct _temp3060; _temp3060.tag=
Cyc_Absyn_StructField; _temp3060.f1=({ struct _tagged_arr* _temp3061=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp3061[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3061;});
_temp3060;}); _temp3059;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3058;});
_temp3057;}); break; case 367: _LL3056: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3063=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3063[ 0]=({ struct Cyc_Exp_tok_struct _temp3064; _temp3064.tag= Cyc_Exp_tok;
_temp3064.f1= Cyc_Absyn_offsetof_exp((* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])).f3,( void*)({
struct Cyc_Absyn_TupleIndex_struct* _temp3065=( struct Cyc_Absyn_TupleIndex_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Absyn_TupleIndex_struct)); _temp3065[ 0]=({
struct Cyc_Absyn_TupleIndex_struct _temp3066; _temp3066.tag= Cyc_Absyn_TupleIndex;
_temp3066.f1=( unsigned int)(* Cyc_yyget_Int_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f2; _temp3066;});
_temp3065;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3064;}); _temp3063;}); break; case 368:
_LL3062: { struct Cyc_Position_Segment* _temp3068= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); struct
Cyc_List_List* _temp3069=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, _temp3068, Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3070=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3070[ 0]=({ struct Cyc_Exp_tok_struct
_temp3071; _temp3071.tag= Cyc_Exp_tok; _temp3071.f1= Cyc_Absyn_gentyp_exp(
_temp3069,(* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3071;});
_temp3070;}); break;} case 369: _LL3067: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3073=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3073[ 0]=({ struct Cyc_Exp_tok_struct _temp3074; _temp3074.tag= Cyc_Exp_tok;
_temp3074.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3075=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3075[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3076; _temp3076.tag= Cyc_Absyn_Malloc_e;
_temp3076.f1= 0; _temp3076.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3076;}); _temp3075;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3074;});
_temp3073;}); break; case 370: _LL3072: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3078=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3078[ 0]=({ struct Cyc_Exp_tok_struct _temp3079; _temp3079.tag= Cyc_Exp_tok;
_temp3079.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3080=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3080[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3081; _temp3081.tag= Cyc_Absyn_Malloc_e;
_temp3081.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3081.f2=(
void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3081;}); _temp3080;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3079;});
_temp3078;}); break; case 371: _LL3077: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3083=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3083[ 0]=({ struct Cyc_Primop_tok_struct _temp3084; _temp3084.tag= Cyc_Primop_tok;
_temp3084.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3084;}); _temp3083;});
break; case 372: _LL3082: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3086=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3086[ 0]=({ struct Cyc_Primop_tok_struct _temp3087; _temp3087.tag= Cyc_Primop_tok;
_temp3087.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3087;}); _temp3086;});
break; case 373: _LL3085: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3089=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3089[ 0]=({ struct Cyc_Primop_tok_struct _temp3090; _temp3090.tag= Cyc_Primop_tok;
_temp3090.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3090;}); _temp3089;});
break; case 374: _LL3088: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 375: _LL3091: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3093=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3093[ 0]=({ struct Cyc_Exp_tok_struct _temp3094; _temp3094.tag= Cyc_Exp_tok;
_temp3094.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3094;}); _temp3093;}); break; case 376: _LL3092: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3096=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3096[ 0]=({ struct Cyc_Exp_tok_struct
_temp3097; _temp3097.tag= Cyc_Exp_tok; _temp3097.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3097;}); _temp3096;}); break; case 377:
_LL3095: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3099=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3099[ 0]=({ struct Cyc_Exp_tok_struct
_temp3100; _temp3100.tag= Cyc_Exp_tok; _temp3100.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3100;});
_temp3099;}); break; case 378: _LL3098: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3102=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3102[ 0]=({ struct Cyc_Exp_tok_struct _temp3103; _temp3103.tag= Cyc_Exp_tok;
_temp3103.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3104=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3104[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3104;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3103;}); _temp3102;}); break; case 379: _LL3101: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3106=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3106[ 0]=({ struct Cyc_Exp_tok_struct
_temp3107; _temp3107.tag= Cyc_Exp_tok; _temp3107.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3107;});
_temp3106;}); break;} case 380: _LL3105: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3109=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3109[ 0]=({ struct Cyc_Exp_tok_struct _temp3110; _temp3110.tag= Cyc_Exp_tok;
_temp3110.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3111=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3111[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3111;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3110;}); _temp3109;}); break; case 381: _LL3108: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3113=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3113[ 0]=({ struct Cyc_Exp_tok_struct
_temp3114; _temp3114.tag= Cyc_Exp_tok; _temp3114.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3114;});
_temp3113;}); break;} case 382: _LL3112: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3116=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3116[ 0]=({ struct Cyc_Exp_tok_struct _temp3117; _temp3117.tag= Cyc_Exp_tok;
_temp3117.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3117;}); _temp3116;}); break; case 383: _LL3115: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3119=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3119[ 0]=({ struct Cyc_Exp_tok_struct
_temp3120; _temp3120.tag= Cyc_Exp_tok; _temp3120.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3120;}); _temp3119;}); break; case 384: _LL3118: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3122=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3122[ 0]=({ struct Cyc_Exp_tok_struct
_temp3123; _temp3123.tag= Cyc_Exp_tok; _temp3123.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3124=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3124[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3125; _temp3125.tag= Cyc_Absyn_CompoundLit_e;
_temp3125.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3125.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3125;});
_temp3124;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3123;}); _temp3122;}); break; case 385:
_LL3121: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3127=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3127[ 0]=({ struct Cyc_Exp_tok_struct
_temp3128; _temp3128.tag= Cyc_Exp_tok; _temp3128.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3129=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3129[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3130; _temp3130.tag= Cyc_Absyn_CompoundLit_e;
_temp3130.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3130.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3130;});
_temp3129;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3128;}); _temp3127;}); break; case 386:
_LL3126: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3132=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3132[ 0]=({ struct Cyc_Exp_tok_struct
_temp3133; _temp3133.tag= Cyc_Exp_tok; _temp3133.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3134=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3134[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3135; _temp3135.tag= Cyc_Absyn_Fill_e; _temp3135.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3135;});
_temp3134;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3133;}); _temp3132;}); break; case 387:
_LL3131: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3137=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3137[ 0]=({ struct Cyc_Exp_tok_struct
_temp3138; _temp3138.tag= Cyc_Exp_tok; _temp3138.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3139=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3139[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3140; _temp3140.tag= Cyc_Absyn_Codegen_e;
_temp3140.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3140;}); _temp3139;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3138;}); _temp3137;}); break; case 388: _LL3136: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3142=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3142[ 0]=({ struct Cyc_Exp_tok_struct
_temp3143; _temp3143.tag= Cyc_Exp_tok; _temp3143.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3144=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3144[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3145; _temp3145.tag= Cyc_Absyn_UnknownId_e;
_temp3145.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3145;}); _temp3144;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3143;});
_temp3142;}); break; case 389: _LL3141: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 390: _LL3146:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3148=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3148[ 0]=({ struct Cyc_Exp_tok_struct
_temp3149; _temp3149.tag= Cyc_Exp_tok; _temp3149.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3149;});
_temp3148;}); break; case 391: _LL3147: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 392:
_LL3150: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3152=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3152[ 0]=({ struct Cyc_Exp_tok_struct
_temp3153; _temp3153.tag= Cyc_Exp_tok; _temp3153.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3154=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3154[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3155; _temp3155.tag=
Cyc_Absyn_UnknownId_e; _temp3155.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3155;});
_temp3154;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3153;});
_temp3152;}); break; case 393: _LL3151: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3157=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3157[ 0]=({ struct Cyc_Exp_tok_struct _temp3158; _temp3158.tag= Cyc_Exp_tok;
_temp3158.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3159=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3159[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3160; _temp3160.tag=
Cyc_Absyn_UnknownId_e; _temp3160.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3160;});
_temp3159;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3158;}); _temp3157;}); break; case 394: _LL3156: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3162=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3162[ 0]=({ struct Cyc_Exp_tok_struct
_temp3163; _temp3163.tag= Cyc_Exp_tok; _temp3163.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3163;}); _temp3162;}); break; case 395: _LL3161: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3165=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3165[ 0]=({ struct Cyc_Exp_tok_struct
_temp3166; _temp3166.tag= Cyc_Exp_tok; _temp3166.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3167=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3167[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3168; _temp3168.tag= Cyc_Absyn_Struct_e;
_temp3168.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3168.f2= 0; _temp3168.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3168.f4= 0;
_temp3168;}); _temp3167;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3166;});
_temp3165;}); break; case 396: _LL3164: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3170=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3170[ 0]=({ struct Cyc_Exp_tok_struct _temp3171; _temp3171.tag= Cyc_Exp_tok;
_temp3171.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3171;}); _temp3170;}); break; case 397: _LL3169: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3173=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3173[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3174; _temp3174.tag= Cyc_ExpList_tok; _temp3174.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3174;});
_temp3173;}); break; case 398: _LL3172: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3176=( struct Cyc_ExpList_tok_struct*) _cycalloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3176[ 0]=({ struct Cyc_ExpList_tok_struct _temp3177; _temp3177.tag= Cyc_ExpList_tok;
_temp3177.f1=({ struct Cyc_List_List* _temp3178=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3178->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3178->tl= 0;
_temp3178;}); _temp3177;}); _temp3176;}); break; case 399: _LL3175: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3180=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3180[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3181; _temp3181.tag= Cyc_ExpList_tok; _temp3181.f1=({ struct Cyc_List_List*
_temp3182=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3182->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3182->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp3182;});
_temp3181;}); _temp3180;}); break; case 400: _LL3179: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3184=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3184[ 0]=({ struct Cyc_Exp_tok_struct
_temp3185; _temp3185.tag= Cyc_Exp_tok; _temp3185.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3185;});
_temp3184;}); break; case 401: _LL3183: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3187=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3187[ 0]=({ struct Cyc_Exp_tok_struct _temp3188; _temp3188.tag= Cyc_Exp_tok;
_temp3188.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3188;});
_temp3187;}); break; case 402: _LL3186: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3190=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3190[ 0]=({ struct Cyc_Exp_tok_struct _temp3191; _temp3191.tag= Cyc_Exp_tok;
_temp3191.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3191;});
_temp3190;}); break; case 403: _LL3189: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3193=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3193[ 0]=({ struct Cyc_Exp_tok_struct _temp3194; _temp3194.tag= Cyc_Exp_tok;
_temp3194.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3194;});
_temp3193;}); break; case 404: _LL3192: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3196=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3196[ 0]=({ struct Cyc_QualId_tok_struct _temp3197; _temp3197.tag= Cyc_QualId_tok;
_temp3197.f1=({ struct _tuple1* _temp3198=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3198->f1= Cyc_Absyn_rel_ns_null; _temp3198->f2=({ struct
_tagged_arr* _temp3199=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3199[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3199;}); _temp3198;});
_temp3197;}); _temp3196;}); break; case 405: _LL3195: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3200:(
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
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 406u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 112u, yyn -  121)] +  yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >=  0?
yystate <=  4384: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4385u,
yystate)] ==  yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){
yystate=( int) Cyc_yytable[ _check_known_subscript_notnull( 4385u, yystate)];}
else{ yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 
121)];} goto yynewstate; yyerrlab: if( yyerrstatus ==  0){ ++ Cyc_yynerrs; yyn=(
int) Cyc_yypact[ _check_known_subscript_notnull( 804u, yystate)]; if( yyn >  -
32768? yyn <  4384: 0){ int sze= 0; struct _tagged_arr msg; int x; int count;
count= 0; for( x= yyn <  0? - yyn: 0; x <  233u /  sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4385u, x +  yyn)] ==  x){( sze
+= Cyc_Std_strlen( Cyc_yytname[ _check_known_subscript_notnull( 233u, x)]) +  15,
count ++);}} msg=({ unsigned int _temp3202=( unsigned int)( sze +  15);
unsigned char* _temp3203=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp3202)); struct _tagged_arr _temp3205= _tag_arr(
_temp3203, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp3204= _temp3202; unsigned int i; for( i= 0; i <  _temp3204; i ++){
_temp3203[ i]='\000';}}; _temp3205;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
sizeof( unsigned char), 12u)); if( count <  5){ count= 0; for( x= yyn <  0? -
yyn: 0; x <  233u /  sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4385u, x +  yyn)] ==  x){ Cyc_Std_strcat( msg,
count ==  0? _tag_arr(", expecting `", sizeof( unsigned char), 14u): _tag_arr(" or `",
sizeof( unsigned char), 6u)); Cyc_Std_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 233u, x)]); Cyc_Std_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus ==  3){ if( Cyc_yychar ==  0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset ==  0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 804u,
yystate)]; if( yyn ==  - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn <  0? 1:
yyn >  4384)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4385u, yyn)] !=  1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4385u,
yyn)]; if( yyn <  0){ if( yyn ==  - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn ==  0){ goto yyerrpop;}} if( yyn ==  803){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3215= v; struct
_tuple15* _temp3233; struct _tuple15 _temp3235; int _temp3236; unsigned char
_temp3238; short _temp3240; struct _tagged_arr _temp3242; struct Cyc_Core_Opt*
_temp3244; struct Cyc_Core_Opt* _temp3246; struct Cyc_Core_Opt _temp3248; struct
_tagged_arr* _temp3249; struct _tuple1* _temp3251; struct _tuple1 _temp3253;
struct _tagged_arr* _temp3254; void* _temp3256; _LL3217: if(*(( void**)
_temp3215) ==  Cyc_Int_tok){ _LL3234: _temp3233=(( struct Cyc_Int_tok_struct*)
_temp3215)->f1; _temp3235=* _temp3233; _LL3237: _temp3236= _temp3235.f2; goto
_LL3218;} else{ goto _LL3219;} _LL3219: if(*(( void**) _temp3215) ==  Cyc_Char_tok){
_LL3239: _temp3238=(( struct Cyc_Char_tok_struct*) _temp3215)->f1; goto _LL3220;}
else{ goto _LL3221;} _LL3221: if(*(( void**) _temp3215) ==  Cyc_Short_tok){
_LL3241: _temp3240=(( struct Cyc_Short_tok_struct*) _temp3215)->f1; goto _LL3222;}
else{ goto _LL3223;} _LL3223: if(*(( void**) _temp3215) ==  Cyc_String_tok){
_LL3243: _temp3242=(( struct Cyc_String_tok_struct*) _temp3215)->f1; goto
_LL3224;} else{ goto _LL3225;} _LL3225: if(*(( void**) _temp3215) ==  Cyc_Stringopt_tok){
_LL3245: _temp3244=(( struct Cyc_Stringopt_tok_struct*) _temp3215)->f1; if(
_temp3244 ==  0){ goto _LL3226;} else{ goto _LL3227;}} else{ goto _LL3227;}
_LL3227: if(*(( void**) _temp3215) ==  Cyc_Stringopt_tok){ _LL3247: _temp3246=((
struct Cyc_Stringopt_tok_struct*) _temp3215)->f1; if( _temp3246 ==  0){ goto
_LL3229;} else{ _temp3248=* _temp3246; _LL3250: _temp3249=( struct _tagged_arr*)
_temp3248.v; goto _LL3228;}} else{ goto _LL3229;} _LL3229: if(*(( void**)
_temp3215) ==  Cyc_QualId_tok){ _LL3252: _temp3251=(( struct Cyc_QualId_tok_struct*)
_temp3215)->f1; _temp3253=* _temp3251; _LL3257: _temp3256= _temp3253.f1; goto
_LL3255; _LL3255: _temp3254= _temp3253.f2; goto _LL3230;} else{ goto _LL3231;}
_LL3231: goto _LL3232; _LL3218:({ struct Cyc_Std_Int_pa_struct _temp3259;
_temp3259.tag= Cyc_Std_Int_pa; _temp3259.f1=( int)(( unsigned int) _temp3236);{
void* _temp3258[ 1u]={& _temp3259}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3258, sizeof( void*), 1u));}}); goto
_LL3216; _LL3220:({ struct Cyc_Std_Int_pa_struct _temp3261; _temp3261.tag= Cyc_Std_Int_pa;
_temp3261.f1=( int)(( unsigned int)(( int) _temp3238));{ void* _temp3260[ 1u]={&
_temp3261}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3260, sizeof( void*), 1u));}}); goto _LL3216;
_LL3222:({ struct Cyc_Std_Int_pa_struct _temp3263; _temp3263.tag= Cyc_Std_Int_pa;
_temp3263.f1=( int)(( unsigned int)(( int) _temp3240));{ void* _temp3262[ 1u]={&
_temp3263}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3262, sizeof( void*), 1u));}}); goto _LL3216;
_LL3224:({ struct Cyc_Std_String_pa_struct _temp3265; _temp3265.tag= Cyc_Std_String_pa;
_temp3265.f1=( struct _tagged_arr) _temp3242;{ void* _temp3264[ 1u]={& _temp3265};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3264, sizeof( void*), 1u));}}); goto _LL3216; _LL3226:({ void*
_temp3266[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3266, sizeof( void*), 0u));}); goto _LL3216;
_LL3228:({ struct Cyc_Std_String_pa_struct _temp3268; _temp3268.tag= Cyc_Std_String_pa;
_temp3268.f1=( struct _tagged_arr)* _temp3249;{ void* _temp3267[ 1u]={&
_temp3268}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3267, sizeof( void*), 1u));}}); goto _LL3216;
_LL3230: { struct Cyc_List_List* _temp3269= 0;{ void* _temp3270= _temp3256;
struct Cyc_List_List* _temp3278; struct Cyc_List_List* _temp3280; _LL3272: if((
unsigned int) _temp3270 >  1u?*(( int*) _temp3270) ==  Cyc_Absyn_Rel_n: 0){
_LL3279: _temp3278=(( struct Cyc_Absyn_Rel_n_struct*) _temp3270)->f1; goto
_LL3273;} else{ goto _LL3274;} _LL3274: if(( unsigned int) _temp3270 >  1u?*((
int*) _temp3270) ==  Cyc_Absyn_Abs_n: 0){ _LL3281: _temp3280=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3270)->f1; goto _LL3275;} else{ goto _LL3276;} _LL3276: if( _temp3270 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3277;} else{ goto _LL3271;} _LL3273: _temp3269=
_temp3278; goto _LL3271; _LL3275: _temp3269= _temp3280; goto _LL3271; _LL3277:
goto _LL3271; _LL3271:;} for( 0; _temp3269 !=  0; _temp3269= _temp3269->tl){({
struct Cyc_Std_String_pa_struct _temp3283; _temp3283.tag= Cyc_Std_String_pa;
_temp3283.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _temp3269->hd);{ void*
_temp3282[ 1u]={& _temp3283}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3282, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3285; _temp3285.tag= Cyc_Std_String_pa;
_temp3285.f1=( struct _tagged_arr)* _temp3254;{ void* _temp3284[ 1u]={&
_temp3285}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3284, sizeof( void*), 1u));}}); goto _LL3216;}
_LL3232:({ void* _temp3286[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3286, sizeof( void*), 0u));}); goto
_LL3216; _LL3216:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3287=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3287->v=(
void*) Cyc_Lexing_from_file( f); _temp3287;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
