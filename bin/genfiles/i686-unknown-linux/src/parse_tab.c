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
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
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
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc !=  0){ void* _temp198=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL200: if( _temp198 == (
void*) Cyc_Parse_Extern_sc){ goto _LL201;} else{ goto _LL202;} _LL202: if(
_temp198 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL203;} else{ goto _LL204;}
_LL204: if( _temp198 == ( void*) Cyc_Parse_Static_sc){ goto _LL205;} else{ goto
_LL206;} _LL206: goto _LL207; _LL201: sc=( void*) Cyc_Absyn_Extern; goto _LL199;
_LL203: sc=( void*) Cyc_Absyn_ExternC; goto _LL199; _LL205: sc=( void*) Cyc_Absyn_Static;
goto _LL199; _LL207: Cyc_Parse_err( _tag_arr("bad storage class on function",
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
4u?*(( int*) _temp224) ==  Cyc_Absyn_FnType: 0){ _LL231: _temp230=(( struct Cyc_Absyn_FnType_struct*)
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
last_loc= loc; for( 0; ts !=  0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp256=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp270; void* _temp272; struct Cyc_Position_Segment*
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
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp329=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp329[ 0]=({ struct Cyc_Absyn_IntType_struct _temp330; _temp330.tag= Cyc_Absyn_IntType;
_temp330.f1=( void*) sgn; _temp330.f2=( void*) sz; _temp330;}); _temp329;});}
else{ if( seen_sign){ void* _temp331= t; void* _temp337; _LL333: if((
unsigned int) _temp331 >  4u?*(( int*) _temp331) ==  Cyc_Absyn_IntType: 0){
_LL338: _temp337=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp331)->f2;
goto _LL334;} else{ goto _LL335;} _LL335: goto _LL336; _LL334: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp339=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp339[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp340; _temp340.tag= Cyc_Absyn_IntType; _temp340.f1=( void*) sgn; _temp340.f2=(
void*) _temp337; _temp340;}); _temp339;}); goto _LL332; _LL336: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL332; _LL332:;} if( seen_size){ void* _temp341= t; void*
_temp347; _LL343: if(( unsigned int) _temp341 >  4u?*(( int*) _temp341) ==  Cyc_Absyn_IntType:
0){ _LL348: _temp347=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp341)->f1;
goto _LL344;} else{ goto _LL345;} _LL345: goto _LL346; _LL344: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp349=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp349[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp350; _temp350.tag= Cyc_Absyn_IntType; _temp350.f1=( void*) _temp347;
_temp350.f2=( void*) sz; _temp350;}); _temp349;}); goto _LL342; _LL346: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL342; _LL342:;}} return({ struct _tuple5 _temp351; _temp351.f1=
t; _temp351.f2= declopt; _temp351;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds ==  0){ return 0;}{ struct Cyc_Parse_Declarator* _temp352=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp353= _temp352->id; struct Cyc_List_List* _temp356; struct
Cyc_List_List* _temp358; void* _temp360; struct Cyc_Absyn_Tqual _temp362; struct
_tuple6 _temp354= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp352->tms);
_LL363: _temp362= _temp354.f1; goto _LL361; _LL361: _temp360= _temp354.f2; goto
_LL359; _LL359: _temp358= _temp354.f3; goto _LL357; _LL357: _temp356= _temp354.f4;
goto _LL355; _LL355: return({ struct Cyc_List_List* _temp364=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp364->hd=( void*)({ struct
_tuple7* _temp365=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7));
_temp365->f1= _temp353; _temp365->f2= _temp362; _temp365->f3= _temp360; _temp365->f4=
_temp358; _temp365->f5= _temp356; _temp365;}); _temp364->tl= Cyc_Parse_apply_tmss(
_temp362, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp364;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms ==  0){ return({ struct _tuple6 _temp366; _temp366.f1= tq;
_temp366.f2= t; _temp366.f3= 0; _temp366.f4= atts; _temp366;});}{ void* _temp367=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp*
_temp381; void* _temp383; struct Cyc_Position_Segment* _temp385; struct Cyc_List_List*
_temp387; struct Cyc_Absyn_Tqual _temp389; void* _temp391; void* _temp393;
struct Cyc_List_List* _temp395; struct Cyc_Position_Segment* _temp397; _LL369:
if( _temp367 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL370;} else{ goto _LL371;}
_LL371: if(( unsigned int) _temp367 >  1u?*(( int*) _temp367) ==  Cyc_Absyn_ConstArray_mod:
0){ _LL382: _temp381=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp367)->f1;
goto _LL372;} else{ goto _LL373;} _LL373: if(( unsigned int) _temp367 >  1u?*((
int*) _temp367) ==  Cyc_Absyn_Function_mod: 0){ _LL384: _temp383=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp367)->f1; goto _LL374;} else{ goto
_LL375;} _LL375: if(( unsigned int) _temp367 >  1u?*(( int*) _temp367) ==  Cyc_Absyn_TypeParams_mod:
0){ _LL388: _temp387=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp367)->f1;
goto _LL386; _LL386: _temp385=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp367)->f2; goto _LL376;} else{ goto _LL377;} _LL377: if(( unsigned int)
_temp367 >  1u?*(( int*) _temp367) ==  Cyc_Absyn_Pointer_mod: 0){ _LL394:
_temp393=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp367)->f1; goto
_LL392; _LL392: _temp391=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp367)->f2; goto _LL390; _LL390: _temp389=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp367)->f3; goto _LL378;} else{ goto _LL379;} _LL379: if(( unsigned int)
_temp367 >  1u?*(( int*) _temp367) ==  Cyc_Absyn_Attributes_mod: 0){ _LL398:
_temp397=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp367)->f1; goto _LL396;
_LL396: _temp395=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp367)->f2; goto
_LL380;} else{ goto _LL368;} _LL370: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp399=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp399[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp400; _temp400.tag= Cyc_Absyn_ArrayType; _temp400.f1=(
void*) t; _temp400.f2= tq; _temp400.f3= 0; _temp400;}); _temp399;}), atts,((
struct Cyc_List_List*) _check_null( tms))->tl); _LL372: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp401=(
struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp401[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp402; _temp402.tag= Cyc_Absyn_ArrayType;
_temp402.f1=( void*) t; _temp402.f2= tq; _temp402.f3=( struct Cyc_Absyn_Exp*)
_temp381; _temp402;}); _temp401;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL374: { void* _temp403= _temp383; struct Cyc_List_List* _temp409;
struct Cyc_Core_Opt* _temp411; struct Cyc_Absyn_VarargInfo* _temp413; int
_temp415; struct Cyc_List_List* _temp417; struct Cyc_Position_Segment* _temp419;
_LL405: if(*(( int*) _temp403) ==  Cyc_Absyn_WithTypes){ _LL418: _temp417=((
struct Cyc_Absyn_WithTypes_struct*) _temp403)->f1; goto _LL416; _LL416: _temp415=((
struct Cyc_Absyn_WithTypes_struct*) _temp403)->f2; goto _LL414; _LL414: _temp413=((
struct Cyc_Absyn_WithTypes_struct*) _temp403)->f3; goto _LL412; _LL412: _temp411=((
struct Cyc_Absyn_WithTypes_struct*) _temp403)->f4; goto _LL410; _LL410: _temp409=((
struct Cyc_Absyn_WithTypes_struct*) _temp403)->f5; goto _LL406;} else{ goto
_LL407;} _LL407: if(*(( int*) _temp403) ==  Cyc_Absyn_NoTypes){ _LL420: _temp419=((
struct Cyc_Absyn_NoTypes_struct*) _temp403)->f2; goto _LL408;} else{ goto _LL404;}
_LL406: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!=  0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp421=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp421->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp421->tl= fn_atts; _temp421;});} else{ new_atts=({ struct Cyc_List_List*
_temp422=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp422->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp422->tl= new_atts; _temp422;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl !=  0){ void* _temp423=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_List_List*
_temp429; _LL425: if(( unsigned int) _temp423 >  1u?*(( int*) _temp423) ==  Cyc_Absyn_TypeParams_mod:
0){ _LL430: _temp429=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp423)->f1;
goto _LL426;} else{ goto _LL427;} _LL427: goto _LL428; _LL426: typvars= _temp429;
tms=(( struct Cyc_List_List*) _check_null( tms))->tl; goto _LL424; _LL428: goto
_LL424; _LL424:;} if((((( ! _temp415? _temp413 ==  0: 0)? _temp417 !=  0: 0)?((
struct Cyc_List_List*) _check_null( _temp417))->tl ==  0: 0)?(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp417))->hd)).f1 ==  0: 0)?(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( _temp417))->hd)).f3 == ( void*)
Cyc_Absyn_VoidType: 0){ _temp417= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)(
void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp417); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp411, t, _temp417, _temp415, _temp413, _temp409, fn_atts), new_atts,((
struct Cyc_List_List*) _check_null( tms))->tl);} _LL408:( int) _throw( Cyc_Parse_abort(
_tag_arr("function declaration without parameter types", sizeof( unsigned char),
45u), _temp419)); _LL404:;} _LL376: if((( struct Cyc_List_List*) _check_null(
tms))->tl ==  0){ return({ struct _tuple6 _temp431; _temp431.f1= tq; _temp431.f2=
t; _temp431.f3= _temp387; _temp431.f4= atts; _temp431;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp385)); _LL378: { void* _temp432= _temp393;
struct Cyc_Absyn_Exp* _temp440; struct Cyc_Absyn_Exp* _temp442; _LL434: if((
unsigned int) _temp432 >  1u?*(( int*) _temp432) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL441: _temp440=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp432)->f1; goto
_LL435;} else{ goto _LL436;} _LL436: if(( unsigned int) _temp432 >  1u?*(( int*)
_temp432) ==  Cyc_Absyn_Nullable_ps: 0){ _LL443: _temp442=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp432)->f1; goto _LL437;} else{ goto _LL438;} _LL438: if( _temp432 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL439;} else{ goto _LL433;} _LL435: return Cyc_Parse_apply_tms(
_temp389, Cyc_Absyn_atb_typ( t, _temp391, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp444=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp444[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp445; _temp445.tag= Cyc_Absyn_Upper_b;
_temp445.f1= _temp440; _temp445;}); _temp444;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL437: return Cyc_Parse_apply_tms( _temp389, Cyc_Absyn_starb_typ(
t, _temp391, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp446=( struct
Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp446[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp447; _temp447.tag= Cyc_Absyn_Upper_b;
_temp447.f1= _temp442; _temp447;}); _temp446;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL439: return Cyc_Parse_apply_tms( _temp389, Cyc_Absyn_tagged_typ(
t, _temp391, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL433:;}
_LL380: return Cyc_Parse_apply_tms( tq, t, Cyc_List_append( atts, _temp395),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL368:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp450; void* _temp452; struct _tuple5 _temp448= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL453: _temp452= _temp448.f1; goto _LL451; _LL451: _temp450=
_temp448.f2; goto _LL449; _LL449: if( _temp450 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp452;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp454=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp454[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp455; _temp455.tag= Cyc_Absyn_Decl_s;
_temp455.f1= d; _temp455.f2= s; _temp455;}); _temp454;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc !=  0){ void* _temp456=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL458: if( _temp456 == (
void*) Cyc_Parse_Typedef_sc){ goto _LL459;} else{ goto _LL460;} _LL460: if(
_temp456 == ( void*) Cyc_Parse_Extern_sc){ goto _LL461;} else{ goto _LL462;}
_LL462: if( _temp456 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL463;} else{ goto
_LL464;} _LL464: if( _temp456 == ( void*) Cyc_Parse_Static_sc){ goto _LL465;}
else{ goto _LL466;} _LL466: if( _temp456 == ( void*) Cyc_Parse_Auto_sc){ goto
_LL467;} else{ goto _LL468;} _LL468: if( _temp456 == ( void*) Cyc_Parse_Register_sc){
goto _LL469;} else{ goto _LL470;} _LL470: if( _temp456 == ( void*) Cyc_Parse_Abstract_sc){
goto _LL471;} else{ goto _LL457;} _LL459: istypedef= 1; goto _LL457; _LL461: s=(
void*) Cyc_Absyn_Extern; goto _LL457; _LL463: s=( void*) Cyc_Absyn_ExternC; goto
_LL457; _LL465: s=( void*) Cyc_Absyn_Static; goto _LL457; _LL467: s=( void*) Cyc_Absyn_Public;
goto _LL457; _LL469: s=( void*) Cyc_Absyn_Public; goto _LL457; _LL471: s=( void*)
Cyc_Absyn_Abstract; goto _LL457; _LL457:;}{ struct Cyc_List_List* _temp474;
struct Cyc_List_List* _temp476; struct _tuple0 _temp472=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL477: _temp476= _temp472.f1;
goto _LL475; _LL475: _temp474= _temp472.f2; goto _LL473; _LL473: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp474; for( 0; es !=  0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd !=  0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp478= Cyc_Parse_collapse_type_specifiers( tss, loc); if(
_temp476 ==  0){ struct Cyc_Core_Opt* _temp481; void* _temp483; struct _tuple5
_temp479= _temp478; _LL484: _temp483= _temp479.f1; goto _LL482; _LL482: _temp481=
_temp479.f2; goto _LL480; _LL480: if( _temp481 !=  0){ struct Cyc_Absyn_Decl* d=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null( _temp481))->v;{
void* _temp485=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp497; struct Cyc_Absyn_Structdecl*
_temp499; struct Cyc_Absyn_Uniondecl* _temp501; struct Cyc_Absyn_Tuniondecl*
_temp503; _LL487: if(*(( int*) _temp485) ==  Cyc_Absyn_Enum_d){ _LL498: _temp497=((
struct Cyc_Absyn_Enum_d_struct*) _temp485)->f1; goto _LL488;} else{ goto _LL489;}
_LL489: if(*(( int*) _temp485) ==  Cyc_Absyn_Struct_d){ _LL500: _temp499=((
struct Cyc_Absyn_Struct_d_struct*) _temp485)->f1; goto _LL490;} else{ goto
_LL491;} _LL491: if(*(( int*) _temp485) ==  Cyc_Absyn_Union_d){ _LL502: _temp501=((
struct Cyc_Absyn_Union_d_struct*) _temp485)->f1; goto _LL492;} else{ goto _LL493;}
_LL493: if(*(( int*) _temp485) ==  Cyc_Absyn_Tunion_d){ _LL504: _temp503=((
struct Cyc_Absyn_Tunion_d_struct*) _temp485)->f1; goto _LL494;} else{ goto
_LL495;} _LL495: goto _LL496; _LL488:( void*)( _temp497->sc=( void*) s); if(
atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} goto _LL486; _LL490:( void*)( _temp499->sc=( void*)
s); _temp499->attributes= atts; goto _LL486; _LL492:( void*)( _temp501->sc=(
void*) s); _temp501->attributes= atts; goto _LL486; _LL494:( void*)( _temp503->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL486; _LL496: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL486:;}
return({ struct Cyc_List_List* _temp505=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp505->hd=( void*) d; _temp505->tl= 0;
_temp505;});} else{ void* _temp506= _temp483; struct Cyc_List_List* _temp520;
struct _tuple1* _temp522; struct Cyc_Absyn_TunionInfo _temp524; void* _temp526;
struct Cyc_Absyn_Tuniondecl** _temp528; struct Cyc_Absyn_TunionInfo _temp530;
struct Cyc_List_List* _temp532; void* _temp534; struct Cyc_Absyn_UnknownTunionInfo
_temp536; int _temp538; struct _tuple1* _temp540; struct Cyc_List_List* _temp542;
struct _tuple1* _temp544; struct _tuple1* _temp546; _LL508: if(( unsigned int)
_temp506 >  4u?*(( int*) _temp506) ==  Cyc_Absyn_StructType: 0){ _LL523:
_temp522=(( struct Cyc_Absyn_StructType_struct*) _temp506)->f1; goto _LL521;
_LL521: _temp520=(( struct Cyc_Absyn_StructType_struct*) _temp506)->f2; goto
_LL509;} else{ goto _LL510;} _LL510: if(( unsigned int) _temp506 >  4u?*(( int*)
_temp506) ==  Cyc_Absyn_TunionType: 0){ _LL525: _temp524=(( struct Cyc_Absyn_TunionType_struct*)
_temp506)->f1; _LL527: _temp526=( void*) _temp524.tunion_info; if(*(( int*)
_temp526) ==  Cyc_Absyn_KnownTunion){ _LL529: _temp528=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp526)->f1; goto _LL511;} else{ goto _LL512;}} else{ goto _LL512;} _LL512:
if(( unsigned int) _temp506 >  4u?*(( int*) _temp506) ==  Cyc_Absyn_TunionType:
0){ _LL531: _temp530=(( struct Cyc_Absyn_TunionType_struct*) _temp506)->f1;
_LL535: _temp534=( void*) _temp530.tunion_info; if(*(( int*) _temp534) ==  Cyc_Absyn_UnknownTunion){
_LL537: _temp536=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp534)->f1;
_LL541: _temp540= _temp536.name; goto _LL539; _LL539: _temp538= _temp536.is_xtunion;
goto _LL533;} else{ goto _LL514;} _LL533: _temp532= _temp530.targs; goto _LL513;}
else{ goto _LL514;} _LL514: if(( unsigned int) _temp506 >  4u?*(( int*) _temp506)
==  Cyc_Absyn_UnionType: 0){ _LL545: _temp544=(( struct Cyc_Absyn_UnionType_struct*)
_temp506)->f1; goto _LL543; _LL543: _temp542=(( struct Cyc_Absyn_UnionType_struct*)
_temp506)->f2; goto _LL515;} else{ goto _LL516;} _LL516: if(( unsigned int)
_temp506 >  4u?*(( int*) _temp506) ==  Cyc_Absyn_EnumType: 0){ _LL547: _temp546=((
struct Cyc_Absyn_EnumType_struct*) _temp506)->f1; goto _LL517;} else{ goto
_LL518;} _LL518: goto _LL519; _LL509: { struct Cyc_List_List* _temp548=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp520); struct Cyc_Absyn_Structdecl* _temp549=({
struct Cyc_Absyn_Structdecl* _temp553=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp553->sc=( void*) s; _temp553->name=({
struct Cyc_Core_Opt* _temp554=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp554->v=( void*)(( struct _tuple1*) _check_null( _temp522));
_temp554;}); _temp553->tvs= _temp548; _temp553->fields= 0; _temp553->attributes=
0; _temp553;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp550=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp550->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp551=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp551[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp552; _temp552.tag= Cyc_Absyn_Struct_d;
_temp552.f1= _temp549; _temp552;}); _temp551;}), loc); _temp550->tl= 0; _temp550;});}
_LL511: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp555=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp555->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp556=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp556[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp557; _temp557.tag= Cyc_Absyn_Tunion_d;
_temp557.f1=* _temp528; _temp557;}); _temp556;}), loc); _temp555->tl= 0;
_temp555;}); _LL513: { struct Cyc_List_List* _temp558=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp532); struct Cyc_Absyn_Decl* _temp559= Cyc_Absyn_tunion_decl( s, _temp540,
_temp558, 0, _temp538, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp560=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp560->hd=(
void*) _temp559; _temp560->tl= 0; _temp560;});} _LL515: { struct Cyc_List_List*
_temp561=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp542); struct Cyc_Absyn_Uniondecl* _temp562=({
struct Cyc_Absyn_Uniondecl* _temp567=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp567->sc=( void*) s; _temp567->name=({
struct Cyc_Core_Opt* _temp568=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp568->v=( void*)(( struct _tuple1*) _check_null( _temp544));
_temp568;}); _temp567->tvs= _temp561; _temp567->fields= 0; _temp567->attributes=
0; _temp567;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp563=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp563->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp564=( struct Cyc_Absyn_Decl*) _cycalloc(
sizeof( struct Cyc_Absyn_Decl)); _temp564->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp565=( struct Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp565[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp566; _temp566.tag= Cyc_Absyn_Union_d;
_temp566.f1= _temp562; _temp566;}); _temp565;})); _temp564->loc= loc; _temp564;});
_temp563->tl= 0; _temp563;});} _LL517: { struct Cyc_Absyn_Enumdecl* _temp569=({
struct Cyc_Absyn_Enumdecl* _temp574=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp574->sc=( void*) s; _temp574->name=
_temp546; _temp574->fields= 0; _temp574;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp570=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp570->hd=( void*)({ struct Cyc_Absyn_Decl* _temp571=(
struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp571->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp572=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp572[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp573; _temp573.tag= Cyc_Absyn_Enum_d; _temp573.f1= _temp569; _temp573;});
_temp572;})); _temp571->loc= loc; _temp571;}); _temp570->tl= 0; _temp570;});}
_LL519: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL507:;}} else{ void* t= _temp478.f1; struct Cyc_List_List*
_temp575= Cyc_Parse_apply_tmss( tq, t, _temp476, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp575); if( _temp478.f2 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp478.f2))->v;{ void* _temp576=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp588; struct Cyc_Absyn_Tuniondecl* _temp590;
struct Cyc_Absyn_Uniondecl* _temp592; struct Cyc_Absyn_Enumdecl* _temp594;
_LL578: if(*(( int*) _temp576) ==  Cyc_Absyn_Struct_d){ _LL589: _temp588=((
struct Cyc_Absyn_Struct_d_struct*) _temp576)->f1; goto _LL579;} else{ goto
_LL580;} _LL580: if(*(( int*) _temp576) ==  Cyc_Absyn_Tunion_d){ _LL591:
_temp590=(( struct Cyc_Absyn_Tunion_d_struct*) _temp576)->f1; goto _LL581;}
else{ goto _LL582;} _LL582: if(*(( int*) _temp576) ==  Cyc_Absyn_Union_d){
_LL593: _temp592=(( struct Cyc_Absyn_Union_d_struct*) _temp576)->f1; goto _LL583;}
else{ goto _LL584;} _LL584: if(*(( int*) _temp576) ==  Cyc_Absyn_Enum_d){ _LL595:
_temp594=(( struct Cyc_Absyn_Enum_d_struct*) _temp576)->f1; goto _LL585;} else{
goto _LL586;} _LL586: goto _LL587; _LL579:( void*)( _temp588->sc=( void*) s);
_temp588->attributes= atts; atts= 0; goto _LL577; _LL581:( void*)( _temp590->sc=(
void*) s); goto _LL577; _LL583:( void*)( _temp592->sc=( void*) s); goto _LL577;
_LL585:( void*)( _temp594->sc=( void*) s); goto _LL577; _LL587: Cyc_Parse_err(
_tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL577; _LL577:;} decls=({ struct Cyc_List_List*
_temp596=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp596->hd=( void*) d; _temp596->tl= decls; _temp596;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp598; _temp598.tag=
Cyc_Std_String_pa; _temp598.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp597[ 1u]={&
_temp598}; Cyc_Std_aprintf( _tag_arr("bad attribute %s in typedef", sizeof(
unsigned char), 28u), _tag_arr( _temp597, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp478.f2 !=  0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp599= _temp575; for( 0; _temp599 !=  0;( _temp599=(( struct
Cyc_List_List*) _check_null( _temp599))->tl, _temp474=(( struct Cyc_List_List*)
_check_null( _temp474))->tl)){ struct _tuple7 _temp602; struct Cyc_List_List*
_temp603; struct Cyc_List_List* _temp605; void* _temp607; struct Cyc_Absyn_Tqual
_temp609; struct _tuple1* _temp611; struct _tuple7* _temp600=( struct _tuple7*)((
struct Cyc_List_List*) _check_null( _temp599))->hd; _temp602=* _temp600; _LL612:
_temp611= _temp602.f1; goto _LL610; _LL610: _temp609= _temp602.f2; goto _LL608;
_LL608: _temp607= _temp602.f3; goto _LL606; _LL606: _temp605= _temp602.f4; goto
_LL604; _LL604: _temp603= _temp602.f5; goto _LL601; _LL601: if( _temp605 !=  0){
Cyc_Parse_warn( _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
loc);} if( _temp474 ==  0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected NULL in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp613=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp474))->hd; struct Cyc_Absyn_Vardecl*
_temp614= Cyc_Absyn_new_vardecl( _temp611, _temp607, _temp613); _temp614->tq=
_temp609;( void*)( _temp614->sc=( void*) s); _temp614->attributes= _temp603;{
struct Cyc_Absyn_Decl* _temp615=({ struct Cyc_Absyn_Decl* _temp617=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp617->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp618=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp618[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp619; _temp619.tag= Cyc_Absyn_Var_d; _temp619.f1= _temp614; _temp619;});
_temp618;})); _temp617->loc= loc; _temp617;}); decls=({ struct Cyc_List_List*
_temp616=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp616->hd=( void*) _temp615; _temp616->tl= decls; _temp616;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_Std_strlen( s) !=  1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp621; _temp621.tag= Cyc_Std_String_pa;
_temp621.f1=( struct _tagged_arr) s;{ void* _temp620[ 1u]={& _temp621}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp620,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL622: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL623: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL624: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL625: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL626: return(
void*) Cyc_Absyn_EffKind; default: _LL627: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp630; _temp630.tag= Cyc_Std_String_pa;
_temp630.f1=( struct _tagged_arr) s;{ void* _temp629[ 1u]={& _temp630}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp629,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts ==  0){ return
tms;} else{ return({ struct Cyc_List_List* _temp631=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp631->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp632=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp632[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp633; _temp633.tag= Cyc_Absyn_Attributes_mod;
_temp633.f1= loc; _temp633.f2= atts; _temp633;}); _temp632;})); _temp631->tl=
tms; _temp631;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp636;
struct Cyc_List_List* _temp637; struct Cyc_List_List* _temp639; void* _temp641;
struct Cyc_Absyn_Tqual _temp643; struct _tuple1* _temp645; struct _tuple7*
_temp634= t; _temp636=* _temp634; _LL646: _temp645= _temp636.f1; goto _LL644;
_LL644: _temp643= _temp636.f2; goto _LL642; _LL642: _temp641= _temp636.f3; goto
_LL640; _LL640: _temp639= _temp636.f4; goto _LL638; _LL638: _temp637= _temp636.f5;
goto _LL635; _LL635: Cyc_Lex_register_typedef( _temp645); if( _temp637 !=  0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp648;
_temp648.tag= Cyc_Std_String_pa; _temp648.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp637))->hd);{ void* _temp647[ 1u]={&
_temp648}; Cyc_Std_aprintf( _tag_arr("bad attribute %s within typedef", sizeof(
unsigned char), 32u), _tag_arr( _temp647, sizeof( void*), 1u));}}), loc);}
return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp649=(
struct Cyc_Absyn_Typedef_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp649[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp650; _temp650.tag= Cyc_Absyn_Typedef_d;
_temp650.f1=({ struct Cyc_Absyn_Typedefdecl* _temp651=( struct Cyc_Absyn_Typedefdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp651->name= _temp645;
_temp651->tvs= _temp639; _temp651->defn=( void*) _temp641; _temp651;}); _temp650;});
_temp649;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
unsigned char _temp653[ 15u]="$(sign_t,int)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp653, _temp653, _temp653 +  15u}};
struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ struct _tuple15* yyzzz;{ void*
_temp654= yy1; struct _tuple15* _temp660; _LL656: if(*(( void**) _temp654) == 
Cyc_Int_tok){ _LL661: _temp660=(( struct Cyc_Int_tok_struct*) _temp654)->f1;
goto _LL657;} else{ goto _LL658;} _LL658: goto _LL659; _LL657: yyzzz= _temp660;
goto _LL655; _LL659:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL655:;}
return yyzzz;} static unsigned char _temp663[ 9u]="string_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp663, _temp663, _temp663 +  9u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ struct _tagged_arr yyzzz;{
void* _temp664= yy1; struct _tagged_arr _temp670; _LL666: if(*(( void**)
_temp664) ==  Cyc_String_tok){ _LL671: _temp670=(( struct Cyc_String_tok_struct*)
_temp664)->f1; goto _LL667;} else{ goto _LL668;} _LL668: goto _LL669; _LL667:
yyzzz= _temp670; goto _LL665; _LL669:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL665:;} return yyzzz;} static unsigned char _temp673[ 5u]="char"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{ _temp673,
_temp673, _temp673 +  5u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
unsigned char yyzzz;{ void* _temp674= yy1; unsigned char _temp680; _LL676: if(*((
void**) _temp674) ==  Cyc_Char_tok){ _LL681: _temp680=(( struct Cyc_Char_tok_struct*)
_temp674)->f1; goto _LL677;} else{ goto _LL678;} _LL678: goto _LL679; _LL677:
yyzzz= _temp680; goto _LL675; _LL679:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL675:;} return yyzzz;} static unsigned char _temp683[ 20u]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{
_temp683, _temp683, _temp683 +  20u}}; void* Cyc_yyget_Pointer_Sort_tok( void*
yy1){ void* yyzzz;{ void* _temp684= yy1; void* _temp690; _LL686: if(*(( void**)
_temp684) ==  Cyc_Pointer_Sort_tok){ _LL691: _temp690=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp684)->f1; goto _LL687;} else{ goto _LL688;} _LL688: goto _LL689; _LL687:
yyzzz= _temp690; goto _LL685; _LL689:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL685:;} return yyzzz;} static unsigned char _temp693[ 6u]="exp_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp693,
_temp693, _temp693 +  6u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp694= yy1; struct Cyc_Absyn_Exp*
_temp700; _LL696: if(*(( void**) _temp694) ==  Cyc_Exp_tok){ _LL701: _temp700=((
struct Cyc_Exp_tok_struct*) _temp694)->f1; goto _LL697;} else{ goto _LL698;}
_LL698: goto _LL699; _LL697: yyzzz= _temp700; goto _LL695; _LL699:( int) _throw((
void*)& Cyc_yyfail_Exp_tok); _LL695:;} return yyzzz;} static unsigned char
_temp703[ 14u]="list_t<exp_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{ _temp703, _temp703, _temp703 +  14u}}; struct Cyc_List_List*
Cyc_yyget_ExpList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp704=
yy1; struct Cyc_List_List* _temp710; _LL706: if(*(( void**) _temp704) ==  Cyc_ExpList_tok){
_LL711: _temp710=(( struct Cyc_ExpList_tok_struct*) _temp704)->f1; goto _LL707;}
else{ goto _LL708;} _LL708: goto _LL709; _LL707: yyzzz= _temp710; goto _LL705;
_LL709:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL705:;} return yyzzz;}
static unsigned char _temp713[ 39u]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{
_temp713, _temp713, _temp713 +  39u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp714= yy1; struct Cyc_List_List*
_temp720; _LL716: if(*(( void**) _temp714) ==  Cyc_InitializerList_tok){ _LL721:
_temp720=(( struct Cyc_InitializerList_tok_struct*) _temp714)->f1; goto _LL717;}
else{ goto _LL718;} _LL718: goto _LL719; _LL717: yyzzz= _temp720; goto _LL715;
_LL719:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL715:;} return
yyzzz;} static unsigned char _temp723[ 9u]="primop_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp723, _temp723, _temp723 +  9u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* yyzzz;{ void* _temp724= yy1; void*
_temp730; _LL726: if(*(( void**) _temp724) ==  Cyc_Primop_tok){ _LL731: _temp730=(
void*)(( struct Cyc_Primop_tok_struct*) _temp724)->f1; goto _LL727;} else{ goto
_LL728;} _LL728: goto _LL729; _LL727: yyzzz= _temp730; goto _LL725; _LL729:( int)
_throw(( void*)& Cyc_yyfail_Primop_tok); _LL725:;} return yyzzz;} static
unsigned char _temp733[ 16u]="opt_t<primop_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp733, _temp733, _temp733 +  16u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ struct Cyc_Core_Opt*
yyzzz;{ void* _temp734= yy1; struct Cyc_Core_Opt* _temp740; _LL736: if(*(( void**)
_temp734) ==  Cyc_Primopopt_tok){ _LL741: _temp740=(( struct Cyc_Primopopt_tok_struct*)
_temp734)->f1; goto _LL737;} else{ goto _LL738;} _LL738: goto _LL739; _LL737:
yyzzz= _temp740; goto _LL735; _LL739:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL735:;} return yyzzz;} static unsigned char _temp743[ 7u]="qvar_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{
_temp743, _temp743, _temp743 +  7u}}; struct _tuple1* Cyc_yyget_QualId_tok( void*
yy1){ struct _tuple1* yyzzz;{ void* _temp744= yy1; struct _tuple1* _temp750;
_LL746: if(*(( void**) _temp744) ==  Cyc_QualId_tok){ _LL751: _temp750=(( struct
Cyc_QualId_tok_struct*) _temp744)->f1; goto _LL747;} else{ goto _LL748;} _LL748:
goto _LL749; _LL747: yyzzz= _temp750; goto _LL745; _LL749:( int) _throw(( void*)&
Cyc_yyfail_QualId_tok); _LL745:;} return yyzzz;} static unsigned char _temp753[
7u]="stmt_t"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{
_temp753, _temp753, _temp753 +  7u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok(
void* yy1){ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp754= yy1; struct Cyc_Absyn_Stmt*
_temp760; _LL756: if(*(( void**) _temp754) ==  Cyc_Stmt_tok){ _LL761: _temp760=((
struct Cyc_Stmt_tok_struct*) _temp754)->f1; goto _LL757;} else{ goto _LL758;}
_LL758: goto _LL759; _LL757: yyzzz= _temp760; goto _LL755; _LL759:( int) _throw((
void*)& Cyc_yyfail_Stmt_tok); _LL755:;} return yyzzz;} static unsigned char
_temp763[ 24u]="list_t<switch_clause_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp763, _temp763,
_temp763 +  24u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ struct Cyc_List_List* yyzzz;{ void* _temp764= yy1; struct Cyc_List_List*
_temp770; _LL766: if(*(( void**) _temp764) ==  Cyc_SwitchClauseList_tok){ _LL771:
_temp770=(( struct Cyc_SwitchClauseList_tok_struct*) _temp764)->f1; goto _LL767;}
else{ goto _LL768;} _LL768: goto _LL769; _LL767: yyzzz= _temp770; goto _LL765;
_LL769:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL765:;}
return yyzzz;} static unsigned char _temp773[ 25u]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp773, _temp773, _temp773 +  25u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp774= yy1; struct Cyc_List_List*
_temp780; _LL776: if(*(( void**) _temp774) ==  Cyc_SwitchCClauseList_tok){
_LL781: _temp780=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp774)->f1;
goto _LL777;} else{ goto _LL778;} _LL778: goto _LL779; _LL777: yyzzz= _temp780;
goto _LL775; _LL779:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL775:;} return yyzzz;} static unsigned char _temp783[ 6u]="pat_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{
_temp783, _temp783, _temp783 +  6u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
void* yy1){ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp784= yy1; struct Cyc_Absyn_Pat*
_temp790; _LL786: if(*(( void**) _temp784) ==  Cyc_Pattern_tok){ _LL791:
_temp790=(( struct Cyc_Pattern_tok_struct*) _temp784)->f1; goto _LL787;} else{
goto _LL788;} _LL788: goto _LL789; _LL787: yyzzz= _temp790; goto _LL785; _LL789:(
int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL785:;} return yyzzz;} static
unsigned char _temp793[ 14u]="list_t<pat_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp793, _temp793, _temp793 + 
14u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp794= yy1; struct Cyc_List_List* _temp800; _LL796: if(*(( void**)
_temp794) ==  Cyc_PatternList_tok){ _LL801: _temp800=(( struct Cyc_PatternList_tok_struct*)
_temp794)->f1; goto _LL797;} else{ goto _LL798;} _LL798: goto _LL799; _LL797:
yyzzz= _temp800; goto _LL795; _LL799:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL795:;} return yyzzz;} static unsigned char _temp803[ 31u]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp803, _temp803, _temp803 +  31u}}; struct _tuple13* Cyc_yyget_FieldPattern_tok(
void* yy1){ struct _tuple13* yyzzz;{ void* _temp804= yy1; struct _tuple13*
_temp810; _LL806: if(*(( void**) _temp804) ==  Cyc_FieldPattern_tok){ _LL811:
_temp810=(( struct Cyc_FieldPattern_tok_struct*) _temp804)->f1; goto _LL807;}
else{ goto _LL808;} _LL808: goto _LL809; _LL807: yyzzz= _temp810; goto _LL805;
_LL809:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL805:;} return
yyzzz;} static unsigned char _temp813[ 39u]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp813, _temp813, _temp813 +  39u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp814= yy1; struct Cyc_List_List*
_temp820; _LL816: if(*(( void**) _temp814) ==  Cyc_FieldPatternList_tok){ _LL821:
_temp820=(( struct Cyc_FieldPatternList_tok_struct*) _temp814)->f1; goto _LL817;}
else{ goto _LL818;} _LL818: goto _LL819; _LL817: yyzzz= _temp820; goto _LL815;
_LL819:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL815:;}
return yyzzz;} static unsigned char _temp823[ 9u]="fndecl_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp823, _temp823, _temp823 +  9u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ struct Cyc_Absyn_Fndecl*
yyzzz;{ void* _temp824= yy1; struct Cyc_Absyn_Fndecl* _temp830; _LL826: if(*((
void**) _temp824) ==  Cyc_FnDecl_tok){ _LL831: _temp830=(( struct Cyc_FnDecl_tok_struct*)
_temp824)->f1; goto _LL827;} else{ goto _LL828;} _LL828: goto _LL829; _LL827:
yyzzz= _temp830; goto _LL825; _LL829:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL825:;} return yyzzz;} static unsigned char _temp833[ 15u]="list_t<decl_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp833, _temp833, _temp833 +  15u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp834= yy1; struct Cyc_List_List*
_temp840; _LL836: if(*(( void**) _temp834) ==  Cyc_DeclList_tok){ _LL841:
_temp840=(( struct Cyc_DeclList_tok_struct*) _temp834)->f1; goto _LL837;} else{
goto _LL838;} _LL838: goto _LL839; _LL837: yyzzz= _temp840; goto _LL835; _LL839:(
int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL835:;} return yyzzz;} static
unsigned char _temp843[ 12u]="decl_spec_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp843, _temp843, _temp843 +  12u}};
struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){ struct
Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp844= yy1; struct Cyc_Parse_Declaration_spec*
_temp850; _LL846: if(*(( void**) _temp844) ==  Cyc_DeclSpec_tok){ _LL851:
_temp850=(( struct Cyc_DeclSpec_tok_struct*) _temp844)->f1; goto _LL847;} else{
goto _LL848;} _LL848: goto _LL849; _LL847: yyzzz= _temp850; goto _LL845; _LL849:(
int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL845:;} return yyzzz;} static
unsigned char _temp853[ 27u]="$(declarator_t,exp_opt_t)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp853, _temp853, _temp853 +  27u}};
struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){ struct _tuple14* yyzzz;{
void* _temp854= yy1; struct _tuple14* _temp860; _LL856: if(*(( void**) _temp854)
==  Cyc_InitDecl_tok){ _LL861: _temp860=(( struct Cyc_InitDecl_tok_struct*)
_temp854)->f1; goto _LL857;} else{ goto _LL858;} _LL858: goto _LL859; _LL857:
yyzzz= _temp860; goto _LL855; _LL859:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL855:;} return yyzzz;} static unsigned char _temp863[ 35u]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp863, _temp863, _temp863 +  35u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp864= yy1; struct Cyc_List_List*
_temp870; _LL866: if(*(( void**) _temp864) ==  Cyc_InitDeclList_tok){ _LL871:
_temp870=(( struct Cyc_InitDeclList_tok_struct*) _temp864)->f1; goto _LL867;}
else{ goto _LL868;} _LL868: goto _LL869; _LL867: yyzzz= _temp870; goto _LL865;
_LL869:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL865:;} return
yyzzz;} static unsigned char _temp873[ 16u]="storage_class_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp873, _temp873, _temp873 + 
16u}}; void* Cyc_yyget_StorageClass_tok( void* yy1){ void* yyzzz;{ void*
_temp874= yy1; void* _temp880; _LL876: if(*(( void**) _temp874) ==  Cyc_StorageClass_tok){
_LL881: _temp880=( void*)(( struct Cyc_StorageClass_tok_struct*) _temp874)->f1;
goto _LL877;} else{ goto _LL878;} _LL878: goto _LL879; _LL877: yyzzz= _temp880;
goto _LL875; _LL879:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL875:;}
return yyzzz;} static unsigned char _temp883[ 17u]="type_specifier_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp883, _temp883, _temp883 +  17u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* yyzzz;{ void* _temp884= yy1; void* _temp890; _LL886: if(*(( void**)
_temp884) ==  Cyc_TypeSpecifier_tok){ _LL891: _temp890=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp884)->f1; goto _LL887;} else{ goto _LL888;} _LL888: goto _LL889; _LL887:
yyzzz= _temp890; goto _LL885; _LL889:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL885:;} return yyzzz;} static unsigned char _temp893[ 18u]="struct_or_union_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp893, _temp893, _temp893 +  18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ enum  Cyc_Parse_Struct_or_union yyzzz;{ void* _temp894= yy1; enum 
Cyc_Parse_Struct_or_union _temp900; _LL896: if(*(( void**) _temp894) ==  Cyc_StructOrUnion_tok){
_LL901: _temp900=(( struct Cyc_StructOrUnion_tok_struct*) _temp894)->f1; goto
_LL897;} else{ goto _LL898;} _LL898: goto _LL899; _LL897: yyzzz= _temp900; goto
_LL895; _LL899:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok); _LL895:;}
return yyzzz;} static unsigned char _temp903[ 8u]="tqual_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp903, _temp903, _temp903 +  8u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp904= yy1; struct Cyc_Absyn_Tqual _temp910; _LL906: if(*((
void**) _temp904) ==  Cyc_TypeQual_tok){ _LL911: _temp910=(( struct Cyc_TypeQual_tok_struct*)
_temp904)->f1; goto _LL907;} else{ goto _LL908;} _LL908: goto _LL909; _LL907:
yyzzz= _temp910; goto _LL905; _LL909:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL905:;} return yyzzz;} static unsigned char _temp913[ 22u]="list_t<structfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp913, _temp913, _temp913 +  22u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp914= yy1; struct Cyc_List_List*
_temp920; _LL916: if(*(( void**) _temp914) ==  Cyc_StructFieldDeclList_tok){
_LL921: _temp920=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp914)->f1;
goto _LL917;} else{ goto _LL918;} _LL918: goto _LL919; _LL917: yyzzz= _temp920;
goto _LL915; _LL919:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL915:;} return yyzzz;} static unsigned char _temp923[ 30u]="list_t<list_t<structfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp923, _temp923, _temp923 +  30u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{
void* _temp924= yy1; struct Cyc_List_List* _temp930; _LL926: if(*(( void**)
_temp924) ==  Cyc_StructFieldDeclListList_tok){ _LL931: _temp930=(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp924)->f1; goto _LL927;} else{ goto _LL928;} _LL928: goto _LL929; _LL927:
yyzzz= _temp930; goto _LL925; _LL929:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL925:;} return yyzzz;} static unsigned char _temp933[ 24u]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{
_temp933, _temp933, _temp933 +  24u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp934= yy1; struct Cyc_List_List*
_temp940; _LL936: if(*(( void**) _temp934) ==  Cyc_TypeModifierList_tok){ _LL941:
_temp940=(( struct Cyc_TypeModifierList_tok_struct*) _temp934)->f1; goto _LL937;}
else{ goto _LL938;} _LL938: goto _LL939; _LL937: yyzzz= _temp940; goto _LL935;
_LL939:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL935:;}
return yyzzz;} static unsigned char _temp943[ 13u]="declarator_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp943,
_temp943, _temp943 +  13u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp944= yy1; struct Cyc_Parse_Declarator*
_temp950; _LL946: if(*(( void**) _temp944) ==  Cyc_Declarator_tok){ _LL951:
_temp950=(( struct Cyc_Declarator_tok_struct*) _temp944)->f1; goto _LL947;}
else{ goto _LL948;} _LL948: goto _LL949; _LL947: yyzzz= _temp950; goto _LL945;
_LL949:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL945:;} return
yyzzz;} static unsigned char _temp953[ 21u]="abstractdeclarator_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{
_temp953, _temp953, _temp953 +  21u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp954= yy1;
struct Cyc_Parse_Abstractdeclarator* _temp960; _LL956: if(*(( void**) _temp954)
==  Cyc_AbstractDeclarator_tok){ _LL961: _temp960=(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp954)->f1; goto _LL957;} else{ goto _LL958;} _LL958: goto _LL959; _LL957:
yyzzz= _temp960; goto _LL955; _LL959:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL955:;} return yyzzz;} static unsigned char _temp963[ 5u]="bool"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp963,
_temp963, _temp963 +  5u}}; int Cyc_yyget_Bool_tok( void* yy1){ int yyzzz;{ void*
_temp964= yy1; int _temp970; _LL966: if(*(( void**) _temp964) ==  Cyc_Bool_tok){
_LL971: _temp970=(( struct Cyc_Bool_tok_struct*) _temp964)->f1; goto _LL967;}
else{ goto _LL968;} _LL968: goto _LL969; _LL967: yyzzz= _temp970; goto _LL965;
_LL969:( int) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL965:;} return yyzzz;}
static unsigned char _temp973[ 8u]="scope_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp973, _temp973, _temp973 +  8u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* yyzzz;{ void* _temp974= yy1; void*
_temp980; _LL976: if(*(( void**) _temp974) ==  Cyc_Scope_tok){ _LL981: _temp980=(
void*)(( struct Cyc_Scope_tok_struct*) _temp974)->f1; goto _LL977;} else{ goto
_LL978;} _LL978: goto _LL979; _LL977: yyzzz= _temp980; goto _LL975; _LL979:( int)
_throw(( void*)& Cyc_yyfail_Scope_tok); _LL975:;} return yyzzz;} static
unsigned char _temp983[ 14u]="tunionfield_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{ _temp983, _temp983, _temp983 + 
14u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok( void* yy1){
struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp984= yy1; struct Cyc_Absyn_Tunionfield*
_temp990; _LL986: if(*(( void**) _temp984) ==  Cyc_TunionField_tok){ _LL991:
_temp990=(( struct Cyc_TunionField_tok_struct*) _temp984)->f1; goto _LL987;}
else{ goto _LL988;} _LL988: goto _LL989; _LL987: yyzzz= _temp990; goto _LL985;
_LL989:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok); _LL985:;} return
yyzzz;} static unsigned char _temp993[ 22u]="list_t<tunionfield_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp993, _temp993, _temp993 +  22u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp994= yy1; struct Cyc_List_List*
_temp1000; _LL996: if(*(( void**) _temp994) ==  Cyc_TunionFieldList_tok){
_LL1001: _temp1000=(( struct Cyc_TunionFieldList_tok_struct*) _temp994)->f1;
goto _LL997;} else{ goto _LL998;} _LL998: goto _LL999; _LL997: yyzzz= _temp1000;
goto _LL995; _LL999:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL995:;} return yyzzz;} static unsigned char _temp1003[ 50u]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1003, _temp1003, _temp1003 +  50u}}; struct _tuple17* Cyc_yyget_QualSpecList_tok(
void* yy1){ struct _tuple17* yyzzz;{ void* _temp1004= yy1; struct _tuple17*
_temp1010; _LL1006: if(*(( void**) _temp1004) ==  Cyc_QualSpecList_tok){ _LL1011:
_temp1010=(( struct Cyc_QualSpecList_tok_struct*) _temp1004)->f1; goto _LL1007;}
else{ goto _LL1008;} _LL1008: goto _LL1009; _LL1007: yyzzz= _temp1010; goto
_LL1005; _LL1009:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL1005:;}
return yyzzz;} static unsigned char _temp1013[ 14u]="list_t<var_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{
_temp1013, _temp1013, _temp1013 +  14u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1014= yy1; struct Cyc_List_List*
_temp1020; _LL1016: if(*(( void**) _temp1014) ==  Cyc_IdList_tok){ _LL1021:
_temp1020=(( struct Cyc_IdList_tok_struct*) _temp1014)->f1; goto _LL1017;} else{
goto _LL1018;} _LL1018: goto _LL1019; _LL1017: yyzzz= _temp1020; goto _LL1015;
_LL1019:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1015:;} return yyzzz;}
static unsigned char _temp1023[ 32u]="$(opt_t<var_t>,tqual_t,type_t)@"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{
_temp1023, _temp1023, _temp1023 +  32u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ struct _tuple2* yyzzz;{ void* _temp1024= yy1; struct _tuple2*
_temp1030; _LL1026: if(*(( void**) _temp1024) ==  Cyc_ParamDecl_tok){ _LL1031:
_temp1030=(( struct Cyc_ParamDecl_tok_struct*) _temp1024)->f1; goto _LL1027;}
else{ goto _LL1028;} _LL1028: goto _LL1029; _LL1027: yyzzz= _temp1030; goto
_LL1025; _LL1029:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1025:;}
return yyzzz;} static unsigned char _temp1033[ 40u]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{
_temp1033, _temp1033, _temp1033 +  40u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1034= yy1; struct Cyc_List_List*
_temp1040; _LL1036: if(*(( void**) _temp1034) ==  Cyc_ParamDeclList_tok){
_LL1041: _temp1040=(( struct Cyc_ParamDeclList_tok_struct*) _temp1034)->f1; goto
_LL1037;} else{ goto _LL1038;} _LL1038: goto _LL1039; _LL1037: yyzzz= _temp1040;
goto _LL1035; _LL1039:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1035:;} return yyzzz;} static unsigned char _temp1043[ 107u]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={ Cyc_Core_Failure,{
_temp1043, _temp1043, _temp1043 +  107u}}; struct _tuple16* Cyc_yyget_YY1( void*
yy1){ struct _tuple16* yyzzz;{ void* _temp1044= yy1; struct _tuple16* _temp1050;
_LL1046: if(*(( void**) _temp1044) ==  Cyc_YY1){ _LL1051: _temp1050=(( struct
Cyc_YY1_struct*) _temp1044)->f1; goto _LL1047;} else{ goto _LL1048;} _LL1048:
goto _LL1049; _LL1047: yyzzz= _temp1050; goto _LL1045; _LL1049:( int) _throw((
void*)& Cyc_yyfail_YY1); _LL1045:;} return yyzzz;} static unsigned char
_temp1053[ 15u]="list_t<type_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1053, _temp1053, _temp1053 +  15u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void*
_temp1054= yy1; struct Cyc_List_List* _temp1060; _LL1056: if(*(( void**)
_temp1054) ==  Cyc_TypeList_tok){ _LL1061: _temp1060=(( struct Cyc_TypeList_tok_struct*)
_temp1054)->f1; goto _LL1057;} else{ goto _LL1058;} _LL1058: goto _LL1059;
_LL1057: yyzzz= _temp1060; goto _LL1055; _LL1059:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok);
_LL1055:;} return yyzzz;} static unsigned char _temp1063[ 21u]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{
_temp1063, _temp1063, _temp1063 +  21u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1064= yy1; struct Cyc_List_List*
_temp1070; _LL1066: if(*(( void**) _temp1064) ==  Cyc_DesignatorList_tok){
_LL1071: _temp1070=(( struct Cyc_DesignatorList_tok_struct*) _temp1064)->f1;
goto _LL1067;} else{ goto _LL1068;} _LL1068: goto _LL1069; _LL1067: yyzzz=
_temp1070; goto _LL1065; _LL1069:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1065:;} return yyzzz;} static unsigned char _temp1073[ 13u]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{
_temp1073, _temp1073, _temp1073 +  13u}}; void* Cyc_yyget_Designator_tok( void*
yy1){ void* yyzzz;{ void* _temp1074= yy1; void* _temp1080; _LL1076: if(*(( void**)
_temp1074) ==  Cyc_Designator_tok){ _LL1081: _temp1080=( void*)(( struct Cyc_Designator_tok_struct*)
_temp1074)->f1; goto _LL1077;} else{ goto _LL1078;} _LL1078: goto _LL1079;
_LL1077: yyzzz= _temp1080; goto _LL1075; _LL1079:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL1075:;} return yyzzz;} static unsigned char _temp1083[ 7u]="kind_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{
_temp1083, _temp1083, _temp1083 +  7u}}; void* Cyc_yyget_Kind_tok( void* yy1){
void* yyzzz;{ void* _temp1084= yy1; void* _temp1090; _LL1086: if(*(( void**)
_temp1084) ==  Cyc_Kind_tok){ _LL1091: _temp1090=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1084)->f1; goto _LL1087;} else{ goto _LL1088;} _LL1088: goto _LL1089;
_LL1087: yyzzz= _temp1090; goto _LL1085; _LL1089:( int) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1085:;} return yyzzz;} static unsigned char _temp1093[ 7u]="type_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1093, _temp1093, _temp1093 +  7u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* yyzzz;{ void* _temp1094= yy1; void* _temp1100; _LL1096: if(*(( void**)
_temp1094) ==  Cyc_Type_tok){ _LL1101: _temp1100=( void*)(( struct Cyc_Type_tok_struct*)
_temp1094)->f1; goto _LL1097;} else{ goto _LL1098;} _LL1098: goto _LL1099;
_LL1097: yyzzz= _temp1100; goto _LL1095; _LL1099:( int) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1095:;} return yyzzz;} static unsigned char _temp1103[ 20u]="list_t<attribute_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1103, _temp1103, _temp1103 +  20u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1104= yy1; struct Cyc_List_List*
_temp1110; _LL1106: if(*(( void**) _temp1104) ==  Cyc_AttributeList_tok){
_LL1111: _temp1110=(( struct Cyc_AttributeList_tok_struct*) _temp1104)->f1; goto
_LL1107;} else{ goto _LL1108;} _LL1108: goto _LL1109; _LL1107: yyzzz= _temp1110;
goto _LL1105; _LL1109:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1105:;} return yyzzz;} static unsigned char _temp1113[ 12u]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{
_temp1113, _temp1113, _temp1113 +  12u}}; void* Cyc_yyget_Attribute_tok( void*
yy1){ void* yyzzz;{ void* _temp1114= yy1; void* _temp1120; _LL1116: if(*(( void**)
_temp1114) ==  Cyc_Attribute_tok){ _LL1121: _temp1120=( void*)(( struct Cyc_Attribute_tok_struct*)
_temp1114)->f1; goto _LL1117;} else{ goto _LL1118;} _LL1118: goto _LL1119;
_LL1117: yyzzz= _temp1120; goto _LL1115; _LL1119:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1115:;} return yyzzz;} static unsigned char _temp1123[ 12u]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{
_temp1123, _temp1123, _temp1123 +  12u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
void* yy1){ struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp1124= yy1; struct Cyc_Absyn_Enumfield*
_temp1130; _LL1126: if(*(( void**) _temp1124) ==  Cyc_Enumfield_tok){ _LL1131:
_temp1130=(( struct Cyc_Enumfield_tok_struct*) _temp1124)->f1; goto _LL1127;}
else{ goto _LL1128;} _LL1128: goto _LL1129; _LL1127: yyzzz= _temp1130; goto
_LL1125; _LL1129:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1125:;}
return yyzzz;} static unsigned char _temp1133[ 20u]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1133, _temp1133, _temp1133 +  20u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1134= yy1; struct Cyc_List_List*
_temp1140; _LL1136: if(*(( void**) _temp1134) ==  Cyc_EnumfieldList_tok){
_LL1141: _temp1140=(( struct Cyc_EnumfieldList_tok_struct*) _temp1134)->f1; goto
_LL1137;} else{ goto _LL1138;} _LL1138: goto _LL1139; _LL1137: yyzzz= _temp1140;
goto _LL1135; _LL1139:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1135:;} return yyzzz;} static unsigned char _temp1143[ 14u]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{
_temp1143, _temp1143, _temp1143 +  14u}}; struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok(
void* yy1){ struct Cyc_Core_Opt* yyzzz;{ void* _temp1144= yy1; struct Cyc_Core_Opt*
_temp1150; _LL1146: if(*(( void**) _temp1144) ==  Cyc_TypeOpt_tok){ _LL1151:
_temp1150=(( struct Cyc_TypeOpt_tok_struct*) _temp1144)->f1; goto _LL1147;}
else{ goto _LL1148;} _LL1148: goto _LL1149; _LL1147: yyzzz= _temp1150; goto
_LL1145; _LL1149:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1145:;}
return yyzzz;} static unsigned char _temp1153[ 26u]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{
_temp1153, _temp1153, _temp1153 +  26u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1154= yy1; struct Cyc_List_List*
_temp1160; _LL1156: if(*(( void**) _temp1154) ==  Cyc_Rgnorder_tok){ _LL1161:
_temp1160=(( struct Cyc_Rgnorder_tok_struct*) _temp1154)->f1; goto _LL1157;}
else{ goto _LL1158;} _LL1158: goto _LL1159; _LL1157: yyzzz= _temp1160; goto
_LL1155; _LL1159:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1155:;}
return yyzzz;} struct Cyc_Yyltype{ int timestamp; int first_line; int
first_column; int last_line; int last_column; } ; struct Cyc_Yyltype Cyc_yynewloc(){
return({ struct Cyc_Yyltype _temp1162; _temp1162.timestamp= 0; _temp1162.first_line=
0; _temp1162.first_column= 0; _temp1162.last_line= 0; _temp1162.last_column= 0;
_temp1162;});} struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[
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
unsigned char _temp1163[ 2u]="$"; static unsigned char _temp1164[ 6u]="error";
static unsigned char _temp1165[ 12u]="$undefined."; static unsigned char
_temp1166[ 5u]="AUTO"; static unsigned char _temp1167[ 9u]="REGISTER"; static
unsigned char _temp1168[ 7u]="STATIC"; static unsigned char _temp1169[ 7u]="EXTERN";
static unsigned char _temp1170[ 8u]="TYPEDEF"; static unsigned char _temp1171[ 5u]="VOID";
static unsigned char _temp1172[ 5u]="CHAR"; static unsigned char _temp1173[ 6u]="SHORT";
static unsigned char _temp1174[ 4u]="INT"; static unsigned char _temp1175[ 5u]="LONG";
static unsigned char _temp1176[ 6u]="FLOAT"; static unsigned char _temp1177[ 7u]="DOUBLE";
static unsigned char _temp1178[ 7u]="SIGNED"; static unsigned char _temp1179[ 9u]="UNSIGNED";
static unsigned char _temp1180[ 6u]="CONST"; static unsigned char _temp1181[ 9u]="VOLATILE";
static unsigned char _temp1182[ 9u]="RESTRICT"; static unsigned char _temp1183[
7u]="STRUCT"; static unsigned char _temp1184[ 6u]="UNION"; static unsigned char
_temp1185[ 5u]="CASE"; static unsigned char _temp1186[ 8u]="DEFAULT"; static
unsigned char _temp1187[ 7u]="INLINE"; static unsigned char _temp1188[ 7u]="SIZEOF";
static unsigned char _temp1189[ 9u]="OFFSETOF"; static unsigned char _temp1190[
3u]="IF"; static unsigned char _temp1191[ 5u]="ELSE"; static unsigned char
_temp1192[ 7u]="SWITCH"; static unsigned char _temp1193[ 6u]="WHILE"; static
unsigned char _temp1194[ 3u]="DO"; static unsigned char _temp1195[ 4u]="FOR";
static unsigned char _temp1196[ 5u]="GOTO"; static unsigned char _temp1197[ 9u]="CONTINUE";
static unsigned char _temp1198[ 6u]="BREAK"; static unsigned char _temp1199[ 7u]="RETURN";
static unsigned char _temp1200[ 5u]="ENUM"; static unsigned char _temp1201[ 8u]="NULL_kw";
static unsigned char _temp1202[ 4u]="LET"; static unsigned char _temp1203[ 6u]="THROW";
static unsigned char _temp1204[ 4u]="TRY"; static unsigned char _temp1205[ 6u]="CATCH";
static unsigned char _temp1206[ 4u]="NEW"; static unsigned char _temp1207[ 9u]="ABSTRACT";
static unsigned char _temp1208[ 9u]="FALLTHRU"; static unsigned char _temp1209[
6u]="USING"; static unsigned char _temp1210[ 10u]="NAMESPACE"; static
unsigned char _temp1211[ 7u]="TUNION"; static unsigned char _temp1212[ 8u]="XTUNION";
static unsigned char _temp1213[ 5u]="FILL"; static unsigned char _temp1214[ 8u]="CODEGEN";
static unsigned char _temp1215[ 4u]="CUT"; static unsigned char _temp1216[ 7u]="SPLICE";
static unsigned char _temp1217[ 7u]="MALLOC"; static unsigned char _temp1218[ 9u]="REGION_T";
static unsigned char _temp1219[ 7u]="REGION"; static unsigned char _temp1220[ 5u]="RNEW";
static unsigned char _temp1221[ 8u]="RMALLOC"; static unsigned char _temp1222[ 8u]="REGIONS";
static unsigned char _temp1223[ 4u]="GEN"; static unsigned char _temp1224[ 7u]="PTR_OP";
static unsigned char _temp1225[ 7u]="INC_OP"; static unsigned char _temp1226[ 7u]="DEC_OP";
static unsigned char _temp1227[ 8u]="LEFT_OP"; static unsigned char _temp1228[ 9u]="RIGHT_OP";
static unsigned char _temp1229[ 6u]="LE_OP"; static unsigned char _temp1230[ 6u]="GE_OP";
static unsigned char _temp1231[ 6u]="EQ_OP"; static unsigned char _temp1232[ 6u]="NE_OP";
static unsigned char _temp1233[ 7u]="AND_OP"; static unsigned char _temp1234[ 6u]="OR_OP";
static unsigned char _temp1235[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1236[ 11u]="DIV_ASSIGN"; static unsigned char _temp1237[ 11u]="MOD_ASSIGN";
static unsigned char _temp1238[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1239[ 11u]="SUB_ASSIGN"; static unsigned char _temp1240[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1241[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1242[ 11u]="AND_ASSIGN"; static unsigned char _temp1243[ 11u]="XOR_ASSIGN";
static unsigned char _temp1244[ 10u]="OR_ASSIGN"; static unsigned char _temp1245[
9u]="ELLIPSIS"; static unsigned char _temp1246[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1247[ 12u]="COLON_COLON"; static unsigned char _temp1248[ 11u]="IDENTIFIER";
static unsigned char _temp1249[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1250[ 7u]="STRING"; static unsigned char _temp1251[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1252[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1253[ 9u]="TYPE_VAR"; static unsigned char _temp1254[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1255[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1256[ 10u]="ATTRIBUTE"; static unsigned char _temp1257[ 4u]="';'"; static
unsigned char _temp1258[ 4u]="'{'"; static unsigned char _temp1259[ 4u]="'}'";
static unsigned char _temp1260[ 4u]="'='"; static unsigned char _temp1261[ 4u]="'('";
static unsigned char _temp1262[ 4u]="')'"; static unsigned char _temp1263[ 4u]="','";
static unsigned char _temp1264[ 4u]="'_'"; static unsigned char _temp1265[ 4u]="'$'";
static unsigned char _temp1266[ 4u]="'<'"; static unsigned char _temp1267[ 4u]="'>'";
static unsigned char _temp1268[ 4u]="':'"; static unsigned char _temp1269[ 4u]="'.'";
static unsigned char _temp1270[ 4u]="'['"; static unsigned char _temp1271[ 4u]="']'";
static unsigned char _temp1272[ 4u]="'*'"; static unsigned char _temp1273[ 4u]="'@'";
static unsigned char _temp1274[ 4u]="'?'"; static unsigned char _temp1275[ 4u]="'+'";
static unsigned char _temp1276[ 4u]="'-'"; static unsigned char _temp1277[ 4u]="'&'";
static unsigned char _temp1278[ 4u]="'|'"; static unsigned char _temp1279[ 4u]="'^'";
static unsigned char _temp1280[ 4u]="'/'"; static unsigned char _temp1281[ 4u]="'%'";
static unsigned char _temp1282[ 4u]="'~'"; static unsigned char _temp1283[ 4u]="'!'";
static unsigned char _temp1284[ 5u]="prog"; static unsigned char _temp1285[ 17u]="translation_unit";
static unsigned char _temp1286[ 21u]="external_declaration"; static
unsigned char _temp1287[ 20u]="function_definition"; static unsigned char
_temp1288[ 21u]="function_definition2"; static unsigned char _temp1289[ 13u]="using_action";
static unsigned char _temp1290[ 15u]="unusing_action"; static unsigned char
_temp1291[ 17u]="namespace_action"; static unsigned char _temp1292[ 19u]="unnamespace_action";
static unsigned char _temp1293[ 12u]="declaration"; static unsigned char
_temp1294[ 17u]="declaration_list"; static unsigned char _temp1295[ 23u]="declaration_specifiers";
static unsigned char _temp1296[ 24u]="storage_class_specifier"; static
unsigned char _temp1297[ 15u]="attributes_opt"; static unsigned char _temp1298[
11u]="attributes"; static unsigned char _temp1299[ 15u]="attribute_list"; static
unsigned char _temp1300[ 10u]="attribute"; static unsigned char _temp1301[ 15u]="type_specifier";
static unsigned char _temp1302[ 5u]="kind"; static unsigned char _temp1303[ 15u]="type_qualifier";
static unsigned char _temp1304[ 15u]="enum_specifier"; static unsigned char
_temp1305[ 11u]="enum_field"; static unsigned char _temp1306[ 22u]="enum_declaration_list";
static unsigned char _temp1307[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1308[ 16u]="type_params_opt"; static unsigned char _temp1309[
16u]="struct_or_union"; static unsigned char _temp1310[ 24u]="struct_declaration_list";
static unsigned char _temp1311[ 25u]="struct_declaration_list0"; static
unsigned char _temp1312[ 21u]="init_declarator_list"; static unsigned char
_temp1313[ 22u]="init_declarator_list0"; static unsigned char _temp1314[ 16u]="init_declarator";
static unsigned char _temp1315[ 19u]="struct_declaration"; static unsigned char
_temp1316[ 25u]="specifier_qualifier_list"; static unsigned char _temp1317[ 23u]="struct_declarator_list";
static unsigned char _temp1318[ 24u]="struct_declarator_list0"; static
unsigned char _temp1319[ 18u]="struct_declarator"; static unsigned char
_temp1320[ 17u]="tunion_specifier"; static unsigned char _temp1321[ 18u]="tunion_or_xtunion";
static unsigned char _temp1322[ 17u]="tunionfield_list"; static unsigned char
_temp1323[ 18u]="tunionfield_scope"; static unsigned char _temp1324[ 12u]="tunionfield";
static unsigned char _temp1325[ 11u]="declarator"; static unsigned char
_temp1326[ 18u]="direct_declarator"; static unsigned char _temp1327[ 8u]="pointer";
static unsigned char _temp1328[ 13u]="pointer_char"; static unsigned char
_temp1329[ 8u]="rgn_opt"; static unsigned char _temp1330[ 4u]="rgn"; static
unsigned char _temp1331[ 20u]="type_qualifier_list"; static unsigned char
_temp1332[ 20u]="parameter_type_list"; static unsigned char _temp1333[ 9u]="type_var";
static unsigned char _temp1334[ 16u]="optional_effect"; static unsigned char
_temp1335[ 19u]="optional_rgn_order"; static unsigned char _temp1336[ 10u]="rgn_order";
static unsigned char _temp1337[ 16u]="optional_inject"; static unsigned char
_temp1338[ 11u]="effect_set"; static unsigned char _temp1339[ 14u]="atomic_effect";
static unsigned char _temp1340[ 11u]="region_set"; static unsigned char
_temp1341[ 15u]="parameter_list"; static unsigned char _temp1342[ 22u]="parameter_declaration";
static unsigned char _temp1343[ 16u]="identifier_list"; static unsigned char
_temp1344[ 17u]="identifier_list0"; static unsigned char _temp1345[ 12u]="initializer";
static unsigned char _temp1346[ 18u]="array_initializer"; static unsigned char
_temp1347[ 17u]="initializer_list"; static unsigned char _temp1348[ 12u]="designation";
static unsigned char _temp1349[ 16u]="designator_list"; static unsigned char
_temp1350[ 11u]="designator"; static unsigned char _temp1351[ 10u]="type_name";
static unsigned char _temp1352[ 14u]="any_type_name"; static unsigned char
_temp1353[ 15u]="type_name_list"; static unsigned char _temp1354[ 20u]="abstract_declarator";
static unsigned char _temp1355[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1356[ 10u]="statement"; static unsigned char _temp1357[ 18u]="labeled_statement";
static unsigned char _temp1358[ 21u]="expression_statement"; static
unsigned char _temp1359[ 19u]="compound_statement"; static unsigned char
_temp1360[ 16u]="block_item_list"; static unsigned char _temp1361[ 20u]="selection_statement";
static unsigned char _temp1362[ 15u]="switch_clauses"; static unsigned char
_temp1363[ 16u]="switchC_clauses"; static unsigned char _temp1364[ 20u]="iteration_statement";
static unsigned char _temp1365[ 15u]="jump_statement"; static unsigned char
_temp1366[ 8u]="pattern"; static unsigned char _temp1367[ 19u]="tuple_pattern_list";
static unsigned char _temp1368[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1369[ 14u]="field_pattern"; static unsigned char _temp1370[ 19u]="field_pattern_list";
static unsigned char _temp1371[ 20u]="field_pattern_list0"; static unsigned char
_temp1372[ 11u]="expression"; static unsigned char _temp1373[ 22u]="assignment_expression";
static unsigned char _temp1374[ 20u]="assignment_operator"; static unsigned char
_temp1375[ 23u]="conditional_expression"; static unsigned char _temp1376[ 20u]="constant_expression";
static unsigned char _temp1377[ 22u]="logical_or_expression"; static
unsigned char _temp1378[ 23u]="logical_and_expression"; static unsigned char
_temp1379[ 24u]="inclusive_or_expression"; static unsigned char _temp1380[ 24u]="exclusive_or_expression";
static unsigned char _temp1381[ 15u]="and_expression"; static unsigned char
_temp1382[ 20u]="equality_expression"; static unsigned char _temp1383[ 22u]="relational_expression";
static unsigned char _temp1384[ 17u]="shift_expression"; static unsigned char
_temp1385[ 20u]="additive_expression"; static unsigned char _temp1386[ 26u]="multiplicative_expression";
static unsigned char _temp1387[ 16u]="cast_expression"; static unsigned char
_temp1388[ 17u]="unary_expression"; static unsigned char _temp1389[ 15u]="unary_operator";
static unsigned char _temp1390[ 19u]="postfix_expression"; static unsigned char
_temp1391[ 19u]="primary_expression"; static unsigned char _temp1392[ 25u]="argument_expression_list";
static unsigned char _temp1393[ 26u]="argument_expression_list0"; static
unsigned char _temp1394[ 9u]="constant"; static unsigned char _temp1395[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 233u]={{ _temp1163, _temp1163, _temp1163
+  2u},{ _temp1164, _temp1164, _temp1164 +  6u},{ _temp1165, _temp1165,
_temp1165 +  12u},{ _temp1166, _temp1166, _temp1166 +  5u},{ _temp1167,
_temp1167, _temp1167 +  9u},{ _temp1168, _temp1168, _temp1168 +  7u},{ _temp1169,
_temp1169, _temp1169 +  7u},{ _temp1170, _temp1170, _temp1170 +  8u},{ _temp1171,
_temp1171, _temp1171 +  5u},{ _temp1172, _temp1172, _temp1172 +  5u},{ _temp1173,
_temp1173, _temp1173 +  6u},{ _temp1174, _temp1174, _temp1174 +  4u},{ _temp1175,
_temp1175, _temp1175 +  5u},{ _temp1176, _temp1176, _temp1176 +  6u},{ _temp1177,
_temp1177, _temp1177 +  7u},{ _temp1178, _temp1178, _temp1178 +  7u},{ _temp1179,
_temp1179, _temp1179 +  9u},{ _temp1180, _temp1180, _temp1180 +  6u},{ _temp1181,
_temp1181, _temp1181 +  9u},{ _temp1182, _temp1182, _temp1182 +  9u},{ _temp1183,
_temp1183, _temp1183 +  7u},{ _temp1184, _temp1184, _temp1184 +  6u},{ _temp1185,
_temp1185, _temp1185 +  5u},{ _temp1186, _temp1186, _temp1186 +  8u},{ _temp1187,
_temp1187, _temp1187 +  7u},{ _temp1188, _temp1188, _temp1188 +  7u},{ _temp1189,
_temp1189, _temp1189 +  9u},{ _temp1190, _temp1190, _temp1190 +  3u},{ _temp1191,
_temp1191, _temp1191 +  5u},{ _temp1192, _temp1192, _temp1192 +  7u},{ _temp1193,
_temp1193, _temp1193 +  6u},{ _temp1194, _temp1194, _temp1194 +  3u},{ _temp1195,
_temp1195, _temp1195 +  4u},{ _temp1196, _temp1196, _temp1196 +  5u},{ _temp1197,
_temp1197, _temp1197 +  9u},{ _temp1198, _temp1198, _temp1198 +  6u},{ _temp1199,
_temp1199, _temp1199 +  7u},{ _temp1200, _temp1200, _temp1200 +  5u},{ _temp1201,
_temp1201, _temp1201 +  8u},{ _temp1202, _temp1202, _temp1202 +  4u},{ _temp1203,
_temp1203, _temp1203 +  6u},{ _temp1204, _temp1204, _temp1204 +  4u},{ _temp1205,
_temp1205, _temp1205 +  6u},{ _temp1206, _temp1206, _temp1206 +  4u},{ _temp1207,
_temp1207, _temp1207 +  9u},{ _temp1208, _temp1208, _temp1208 +  9u},{ _temp1209,
_temp1209, _temp1209 +  6u},{ _temp1210, _temp1210, _temp1210 +  10u},{
_temp1211, _temp1211, _temp1211 +  7u},{ _temp1212, _temp1212, _temp1212 +  8u},{
_temp1213, _temp1213, _temp1213 +  5u},{ _temp1214, _temp1214, _temp1214 +  8u},{
_temp1215, _temp1215, _temp1215 +  4u},{ _temp1216, _temp1216, _temp1216 +  7u},{
_temp1217, _temp1217, _temp1217 +  7u},{ _temp1218, _temp1218, _temp1218 +  9u},{
_temp1219, _temp1219, _temp1219 +  7u},{ _temp1220, _temp1220, _temp1220 +  5u},{
_temp1221, _temp1221, _temp1221 +  8u},{ _temp1222, _temp1222, _temp1222 +  8u},{
_temp1223, _temp1223, _temp1223 +  4u},{ _temp1224, _temp1224, _temp1224 +  7u},{
_temp1225, _temp1225, _temp1225 +  7u},{ _temp1226, _temp1226, _temp1226 +  7u},{
_temp1227, _temp1227, _temp1227 +  8u},{ _temp1228, _temp1228, _temp1228 +  9u},{
_temp1229, _temp1229, _temp1229 +  6u},{ _temp1230, _temp1230, _temp1230 +  6u},{
_temp1231, _temp1231, _temp1231 +  6u},{ _temp1232, _temp1232, _temp1232 +  6u},{
_temp1233, _temp1233, _temp1233 +  7u},{ _temp1234, _temp1234, _temp1234 +  6u},{
_temp1235, _temp1235, _temp1235 +  11u},{ _temp1236, _temp1236, _temp1236 +  11u},{
_temp1237, _temp1237, _temp1237 +  11u},{ _temp1238, _temp1238, _temp1238 +  11u},{
_temp1239, _temp1239, _temp1239 +  11u},{ _temp1240, _temp1240, _temp1240 +  12u},{
_temp1241, _temp1241, _temp1241 +  13u},{ _temp1242, _temp1242, _temp1242 +  11u},{
_temp1243, _temp1243, _temp1243 +  11u},{ _temp1244, _temp1244, _temp1244 +  10u},{
_temp1245, _temp1245, _temp1245 +  9u},{ _temp1246, _temp1246, _temp1246 +  11u},{
_temp1247, _temp1247, _temp1247 +  12u},{ _temp1248, _temp1248, _temp1248 +  11u},{
_temp1249, _temp1249, _temp1249 +  17u},{ _temp1250, _temp1250, _temp1250 +  7u},{
_temp1251, _temp1251, _temp1251 +  19u},{ _temp1252, _temp1252, _temp1252 +  18u},{
_temp1253, _temp1253, _temp1253 +  9u},{ _temp1254, _temp1254, _temp1254 +  16u},{
_temp1255, _temp1255, _temp1255 +  18u},{ _temp1256, _temp1256, _temp1256 +  10u},{
_temp1257, _temp1257, _temp1257 +  4u},{ _temp1258, _temp1258, _temp1258 +  4u},{
_temp1259, _temp1259, _temp1259 +  4u},{ _temp1260, _temp1260, _temp1260 +  4u},{
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
_temp1283, _temp1283, _temp1283 +  4u},{ _temp1284, _temp1284, _temp1284 +  5u},{
_temp1285, _temp1285, _temp1285 +  17u},{ _temp1286, _temp1286, _temp1286 +  21u},{
_temp1287, _temp1287, _temp1287 +  20u},{ _temp1288, _temp1288, _temp1288 +  21u},{
_temp1289, _temp1289, _temp1289 +  13u},{ _temp1290, _temp1290, _temp1290 +  15u},{
_temp1291, _temp1291, _temp1291 +  17u},{ _temp1292, _temp1292, _temp1292 +  19u},{
_temp1293, _temp1293, _temp1293 +  12u},{ _temp1294, _temp1294, _temp1294 +  17u},{
_temp1295, _temp1295, _temp1295 +  23u},{ _temp1296, _temp1296, _temp1296 +  24u},{
_temp1297, _temp1297, _temp1297 +  15u},{ _temp1298, _temp1298, _temp1298 +  11u},{
_temp1299, _temp1299, _temp1299 +  15u},{ _temp1300, _temp1300, _temp1300 +  10u},{
_temp1301, _temp1301, _temp1301 +  15u},{ _temp1302, _temp1302, _temp1302 +  5u},{
_temp1303, _temp1303, _temp1303 +  15u},{ _temp1304, _temp1304, _temp1304 +  15u},{
_temp1305, _temp1305, _temp1305 +  11u},{ _temp1306, _temp1306, _temp1306 +  22u},{
_temp1307, _temp1307, _temp1307 +  26u},{ _temp1308, _temp1308, _temp1308 +  16u},{
_temp1309, _temp1309, _temp1309 +  16u},{ _temp1310, _temp1310, _temp1310 +  24u},{
_temp1311, _temp1311, _temp1311 +  25u},{ _temp1312, _temp1312, _temp1312 +  21u},{
_temp1313, _temp1313, _temp1313 +  22u},{ _temp1314, _temp1314, _temp1314 +  16u},{
_temp1315, _temp1315, _temp1315 +  19u},{ _temp1316, _temp1316, _temp1316 +  25u},{
_temp1317, _temp1317, _temp1317 +  23u},{ _temp1318, _temp1318, _temp1318 +  24u},{
_temp1319, _temp1319, _temp1319 +  18u},{ _temp1320, _temp1320, _temp1320 +  17u},{
_temp1321, _temp1321, _temp1321 +  18u},{ _temp1322, _temp1322, _temp1322 +  17u},{
_temp1323, _temp1323, _temp1323 +  18u},{ _temp1324, _temp1324, _temp1324 +  12u},{
_temp1325, _temp1325, _temp1325 +  11u},{ _temp1326, _temp1326, _temp1326 +  18u},{
_temp1327, _temp1327, _temp1327 +  8u},{ _temp1328, _temp1328, _temp1328 +  13u},{
_temp1329, _temp1329, _temp1329 +  8u},{ _temp1330, _temp1330, _temp1330 +  4u},{
_temp1331, _temp1331, _temp1331 +  20u},{ _temp1332, _temp1332, _temp1332 +  20u},{
_temp1333, _temp1333, _temp1333 +  9u},{ _temp1334, _temp1334, _temp1334 +  16u},{
_temp1335, _temp1335, _temp1335 +  19u},{ _temp1336, _temp1336, _temp1336 +  10u},{
_temp1337, _temp1337, _temp1337 +  16u},{ _temp1338, _temp1338, _temp1338 +  11u},{
_temp1339, _temp1339, _temp1339 +  14u},{ _temp1340, _temp1340, _temp1340 +  11u},{
_temp1341, _temp1341, _temp1341 +  15u},{ _temp1342, _temp1342, _temp1342 +  22u},{
_temp1343, _temp1343, _temp1343 +  16u},{ _temp1344, _temp1344, _temp1344 +  17u},{
_temp1345, _temp1345, _temp1345 +  12u},{ _temp1346, _temp1346, _temp1346 +  18u},{
_temp1347, _temp1347, _temp1347 +  17u},{ _temp1348, _temp1348, _temp1348 +  12u},{
_temp1349, _temp1349, _temp1349 +  16u},{ _temp1350, _temp1350, _temp1350 +  11u},{
_temp1351, _temp1351, _temp1351 +  10u},{ _temp1352, _temp1352, _temp1352 +  14u},{
_temp1353, _temp1353, _temp1353 +  15u},{ _temp1354, _temp1354, _temp1354 +  20u},{
_temp1355, _temp1355, _temp1355 +  27u},{ _temp1356, _temp1356, _temp1356 +  10u},{
_temp1357, _temp1357, _temp1357 +  18u},{ _temp1358, _temp1358, _temp1358 +  21u},{
_temp1359, _temp1359, _temp1359 +  19u},{ _temp1360, _temp1360, _temp1360 +  16u},{
_temp1361, _temp1361, _temp1361 +  20u},{ _temp1362, _temp1362, _temp1362 +  15u},{
_temp1363, _temp1363, _temp1363 +  16u},{ _temp1364, _temp1364, _temp1364 +  20u},{
_temp1365, _temp1365, _temp1365 +  15u},{ _temp1366, _temp1366, _temp1366 +  8u},{
_temp1367, _temp1367, _temp1367 +  19u},{ _temp1368, _temp1368, _temp1368 +  20u},{
_temp1369, _temp1369, _temp1369 +  14u},{ _temp1370, _temp1370, _temp1370 +  19u},{
_temp1371, _temp1371, _temp1371 +  20u},{ _temp1372, _temp1372, _temp1372 +  11u},{
_temp1373, _temp1373, _temp1373 +  22u},{ _temp1374, _temp1374, _temp1374 +  20u},{
_temp1375, _temp1375, _temp1375 +  23u},{ _temp1376, _temp1376, _temp1376 +  20u},{
_temp1377, _temp1377, _temp1377 +  22u},{ _temp1378, _temp1378, _temp1378 +  23u},{
_temp1379, _temp1379, _temp1379 +  24u},{ _temp1380, _temp1380, _temp1380 +  24u},{
_temp1381, _temp1381, _temp1381 +  15u},{ _temp1382, _temp1382, _temp1382 +  20u},{
_temp1383, _temp1383, _temp1383 +  22u},{ _temp1384, _temp1384, _temp1384 +  17u},{
_temp1385, _temp1385, _temp1385 +  20u},{ _temp1386, _temp1386, _temp1386 +  26u},{
_temp1387, _temp1387, _temp1387 +  16u},{ _temp1388, _temp1388, _temp1388 +  17u},{
_temp1389, _temp1389, _temp1389 +  15u},{ _temp1390, _temp1390, _temp1390 +  19u},{
_temp1391, _temp1391, _temp1391 +  19u},{ _temp1392, _temp1392, _temp1392 +  25u},{
_temp1393, _temp1393, _temp1393 +  26u},{ _temp1394, _temp1394, _temp1394 +  9u},{
_temp1395, _temp1395, _temp1395 +  20u}}; static short Cyc_yyr1[ 405u]={ 0, 121,
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
short*)({ unsigned int _temp3201= 10000u; short* _temp3202=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3201));{ unsigned int
_temp3203= _temp3201; unsigned int i; for( i= 0; i <  _temp3203; i ++){
_temp3202[ i]=( short) 0;}}; _temp3202;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3198= 10000u; void** _temp3199=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3198));{ unsigned int _temp3200= _temp3198;
unsigned int i; for( i= 0; i <  _temp3200; i ++){ _temp3199[ i]= Cyc_yylval;}};
_temp3199;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3195= 10000u; struct Cyc_Yyltype* _temp3196=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3195));{
unsigned int _temp3197= _temp3195; unsigned int i; for( i= 0; i <  _temp3197; i
++){ _temp3196[ i]= Cyc_yynewloc();}}; _temp3196;}); int yystacksize= 10000;
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
yyn){ case 1: _LL1396: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1397:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1399=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1399[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1400; _temp1400.tag= Cyc_DeclList_tok; _temp1400.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1400;});
_temp1399;}); break; case 3: _LL1398: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1402=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1402[ 0]=({ struct Cyc_DeclList_tok_struct _temp1403; _temp1403.tag= Cyc_DeclList_tok;
_temp1403.f1=({ struct Cyc_List_List* _temp1404=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1404->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1405=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1405->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1406=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1406[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1407; _temp1407.tag= Cyc_Absyn_Using_d;
_temp1407.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1407.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1407;}); _temp1406;}));
_temp1405->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1405;});
_temp1404->tl= 0; _temp1404;}); _temp1403;}); _temp1402;}); Cyc_Lex_leave_using();
break; case 4: _LL1401: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1409=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1409[ 0]=({ struct Cyc_DeclList_tok_struct _temp1410; _temp1410.tag= Cyc_DeclList_tok;
_temp1410.f1=({ struct Cyc_List_List* _temp1411=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1411->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1412=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1412->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1413=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1413[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1414; _temp1414.tag= Cyc_Absyn_Using_d;
_temp1414.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp1414.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1414;});
_temp1413;})); _temp1412->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1412;}); _temp1411->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1411;}); _temp1410;});
_temp1409;}); break; case 5: _LL1408: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1416=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1416[ 0]=({ struct Cyc_DeclList_tok_struct _temp1417; _temp1417.tag= Cyc_DeclList_tok;
_temp1417.f1=({ struct Cyc_List_List* _temp1418=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1418->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1419=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1419->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1420=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1420[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1421; _temp1421.tag=
Cyc_Absyn_Namespace_d; _temp1421.f1=({ struct _tagged_arr* _temp1422=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1422[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1422;});
_temp1421.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1421;}); _temp1420;})); _temp1419->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1419;}); _temp1418->tl= 0; _temp1418;}); _temp1417;}); _temp1416;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1415: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1424=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1424[ 0]=({ struct Cyc_DeclList_tok_struct _temp1425; _temp1425.tag= Cyc_DeclList_tok;
_temp1425.f1=({ struct Cyc_List_List* _temp1426=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1426->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1427=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1427->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1428=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1428[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1429; _temp1429.tag=
Cyc_Absyn_Namespace_d; _temp1429.f1=({ struct _tagged_arr* _temp1430=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1430[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1430;});
_temp1429.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1429;}); _temp1428;})); _temp1427->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1427;}); _temp1426->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1426;}); _temp1425;});
_temp1424;}); break; case 7: _LL1423: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1432=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1432[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1433; _temp1433.tag= Cyc_DeclList_tok; _temp1433.f1=({ struct Cyc_List_List*
_temp1434=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1434->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1435=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1435->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1436=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1436[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1437; _temp1437.tag= Cyc_Absyn_ExternC_d;
_temp1437.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1437;}); _temp1436;})); _temp1435->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1435;}); _temp1434->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1434;}); _temp1433;});
_temp1432;}); break; case 8: _LL1431: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1439=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1439[ 0]=({ struct Cyc_DeclList_tok_struct _temp1440; _temp1440.tag= Cyc_DeclList_tok;
_temp1440.f1= 0; _temp1440;}); _temp1439;}); break; case 9: _LL1438: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1442=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1442[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1443; _temp1443.tag= Cyc_DeclList_tok; _temp1443.f1=({ struct Cyc_List_List*
_temp1444=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1444->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1445=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1445[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1446; _temp1446.tag= Cyc_Absyn_Fn_d;
_temp1446.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1446;}); _temp1445;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1444->tl=
0; _temp1444;}); _temp1443;}); _temp1442;}); break; case 10: _LL1441: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1447: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1449=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1449[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1450; _temp1450.tag= Cyc_FnDecl_tok; _temp1450.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1450;}); _temp1449;}); break; case 12: _LL1448: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1452=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1452[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1453; _temp1453.tag= Cyc_FnDecl_tok;
_temp1453.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1454=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1454->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1454;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1453;}); _temp1452;}); break; case 13: _LL1451: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1456=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1456[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1457; _temp1457.tag= Cyc_FnDecl_tok;
_temp1457.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1457;}); _temp1456;}); break; case 14: _LL1455: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1459=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1459[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1460; _temp1460.tag= Cyc_FnDecl_tok;
_temp1460.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1461=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1461->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1461;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1460;}); _temp1459;}); break; case 15: _LL1458: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1463=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1463[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1464; _temp1464.tag= Cyc_FnDecl_tok;
_temp1464.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1465=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1465->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1465;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1464;}); _temp1463;}); break; case 16: _LL1462: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1467=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1467[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1468; _temp1468.tag= Cyc_FnDecl_tok;
_temp1468.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1469=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1469->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1469;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1468;}); _temp1467;}); break; case 17: _LL1466: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1470:
Cyc_Lex_leave_using(); break; case 19: _LL1471: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1473=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1473[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1473;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1472: Cyc_Lex_leave_namespace(); break; case 21: _LL1474: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1476=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1476[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1477; _temp1477.tag= Cyc_DeclList_tok; _temp1477.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1477;}); _temp1476;}); break; case 22: _LL1475: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1479=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1479[ 0]=({ struct Cyc_DeclList_tok_struct _temp1480; _temp1480.tag= Cyc_DeclList_tok;
_temp1480.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1480;}); _temp1479;}); break; case 23: _LL1478: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1482=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1482[ 0]=({ struct Cyc_DeclList_tok_struct _temp1483; _temp1483.tag= Cyc_DeclList_tok;
_temp1483.f1=({ struct Cyc_List_List* _temp1484=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1484->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1484->tl=
0; _temp1484;}); _temp1483;}); _temp1482;}); break; case 24: _LL1481: { struct
Cyc_List_List* _temp1486= 0;{ struct Cyc_List_List* _temp1487= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0;
_temp1487 !=  0; _temp1487=(( struct Cyc_List_List*) _check_null( _temp1487))->tl){
struct _tagged_arr* _temp1488=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1487))->hd; struct _tuple1* _temp1489=({ struct _tuple1*
_temp1492=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1492->f1=
Cyc_Absyn_rel_ns_null; _temp1492->f2= _temp1488; _temp1492;}); struct Cyc_Absyn_Vardecl*
_temp1490= Cyc_Absyn_new_vardecl( _temp1489, Cyc_Absyn_wildtyp( 0), 0);
_temp1486=({ struct Cyc_List_List* _temp1491=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1491->hd=( void*) _temp1490; _temp1491->tl=
_temp1486; _temp1491;});}} _temp1486=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1486); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1493=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1493[ 0]=({ struct Cyc_DeclList_tok_struct _temp1494; _temp1494.tag= Cyc_DeclList_tok;
_temp1494.f1=({ struct Cyc_List_List* _temp1495=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1495->hd=( void*) Cyc_Absyn_letv_decl(
_temp1486, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1495->tl= 0; _temp1495;}); _temp1494;});
_temp1493;}); break;} case 25: _LL1485: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1496:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1498=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1498[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1499; _temp1499.tag= Cyc_DeclList_tok; _temp1499.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1499;});
_temp1498;}); break; case 27: _LL1497: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1501=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1501[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1502; _temp1502.tag= Cyc_DeclSpec_tok;
_temp1502.f1=({ struct Cyc_Parse_Declaration_spec* _temp1503=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1503->sc=({ struct
Cyc_Core_Opt* _temp1504=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1504->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1504;});
_temp1503->tq= Cyc_Absyn_empty_tqual(); _temp1503->type_specs= 0; _temp1503->is_inline=
0; _temp1503->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1503;}); _temp1502;});
_temp1501;}); break; case 28: _LL1500: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc !=  0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1506=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1506[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1507; _temp1507.tag= Cyc_DeclSpec_tok; _temp1507.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1508=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1508->sc=({ struct Cyc_Core_Opt* _temp1509=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1509->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1509;});
_temp1508->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1508->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1508->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1508->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1508;}); _temp1507;}); _temp1506;}); break; case 29: _LL1505: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1511=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1511[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1512; _temp1512.tag= Cyc_DeclSpec_tok; _temp1512.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1513=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1513->sc= 0; _temp1513->tq= Cyc_Absyn_empty_tqual(); _temp1513->type_specs=({
struct Cyc_List_List* _temp1514=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1514->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1514->tl= 0;
_temp1514;}); _temp1513->is_inline= 0; _temp1513->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1513;});
_temp1512;}); _temp1511;}); break; case 30: _LL1510: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1516=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1516[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1517; _temp1517.tag= Cyc_DeclSpec_tok;
_temp1517.f1=({ struct Cyc_Parse_Declaration_spec* _temp1518=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1518->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1518->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1518->type_specs=({ struct Cyc_List_List* _temp1519=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1519->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1519->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1519;});
_temp1518->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1518->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1518;}); _temp1517;}); _temp1516;}); break; case 31: _LL1515: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1521=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1521[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1522; _temp1522.tag= Cyc_DeclSpec_tok; _temp1522.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1523=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1523->sc= 0; _temp1523->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1523->type_specs=
0; _temp1523->is_inline= 0; _temp1523->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1523;});
_temp1522;}); _temp1521;}); break; case 32: _LL1520: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1525=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1525[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1526; _temp1526.tag= Cyc_DeclSpec_tok;
_temp1526.f1=({ struct Cyc_Parse_Declaration_spec* _temp1527=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1527->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1527->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1527->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1527->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1527->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1527;}); _temp1526;}); _temp1525;}); break; case 33: _LL1524: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1529=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1529[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1530; _temp1530.tag= Cyc_DeclSpec_tok; _temp1530.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1531=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1531->sc= 0; _temp1531->tq= Cyc_Absyn_empty_tqual(); _temp1531->type_specs=
0; _temp1531->is_inline= 1; _temp1531->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1531;});
_temp1530;}); _temp1529;}); break; case 34: _LL1528: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1533=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1533[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1534; _temp1534.tag= Cyc_DeclSpec_tok;
_temp1534.f1=({ struct Cyc_Parse_Declaration_spec* _temp1535=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1535->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1535->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1535->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1535->is_inline=
1; _temp1535->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1535;}); _temp1534;}); _temp1533;}); break; case 35: _LL1532: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1537=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1537[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1538; _temp1538.tag= Cyc_StorageClass_tok;
_temp1538.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1538;}); _temp1537;});
break; case 36: _LL1536: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1540=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1540[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1541; _temp1541.tag=
Cyc_StorageClass_tok; _temp1541.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1541;}); _temp1540;}); break; case 37: _LL1539: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1543=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1543[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1544; _temp1544.tag=
Cyc_StorageClass_tok; _temp1544.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1544;}); _temp1543;}); break; case 38: _LL1542: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1546=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1546[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1547; _temp1547.tag=
Cyc_StorageClass_tok; _temp1547.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1547;}); _temp1546;}); break; case 39: _LL1545: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1549=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1549[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1550; _temp1550.tag= Cyc_StorageClass_tok;
_temp1550.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1550;}); _temp1549;});
break; case 40: _LL1548: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1552=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1552[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1553; _temp1553.tag=
Cyc_StorageClass_tok; _temp1553.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1553;}); _temp1552;}); break; case 41: _LL1551: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1555=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1555[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1556; _temp1556.tag=
Cyc_StorageClass_tok; _temp1556.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1556;}); _temp1555;}); break; case 42: _LL1554: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1558=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1558[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1559; _temp1559.tag=
Cyc_AttributeList_tok; _temp1559.f1= 0; _temp1559;}); _temp1558;}); break; case
43: _LL1557: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1560: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1562=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1562[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1563; _temp1563.tag=
Cyc_AttributeList_tok; _temp1563.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1563;});
_temp1562;}); break; case 45: _LL1561: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1565=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1565[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1566; _temp1566.tag=
Cyc_AttributeList_tok; _temp1566.f1=({ struct Cyc_List_List* _temp1567=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1567->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1567->tl= 0; _temp1567;}); _temp1566;}); _temp1565;});
break; case 46: _LL1564: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1569=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1569[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1570; _temp1570.tag=
Cyc_AttributeList_tok; _temp1570.f1=({ struct Cyc_List_List* _temp1571=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1571->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1571->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1571;}); _temp1570;});
_temp1569;}); break; case 47: _LL1568: { struct _tagged_arr _temp1573= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1573, sizeof( unsigned char)) >  4?*(( const unsigned char*)
_check_unknown_subscript( _temp1573, sizeof( unsigned char), 0)) == '_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1573, sizeof( unsigned char),
1)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1573,
sizeof( unsigned char),( int)( _get_arr_size( _temp1573, sizeof( unsigned char))
-  2))) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1573,
sizeof( unsigned char),( int)( _get_arr_size( _temp1573, sizeof( unsigned char))
-  3))) == '_': 0){ _temp1573=( struct _tagged_arr) Cyc_Std_substring( _temp1573,
2, _get_arr_size( _temp1573, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0;
i <  16u; ++ i){ if( Cyc_Std_strcmp( _temp1573,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1574=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1574[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1575; _temp1575.tag= Cyc_Attribute_tok; _temp1575.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1575;}); _temp1574;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1576=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1576[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1577; _temp1577.tag= Cyc_Attribute_tok; _temp1577.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1577;}); _temp1576;});} break;}} case
48: _LL1572: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1579=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1579[ 0]=({ struct Cyc_Attribute_tok_struct _temp1580; _temp1580.tag= Cyc_Attribute_tok;
_temp1580.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1580;}); _temp1579;});
break; case 49: _LL1578: { struct _tagged_arr _temp1582= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1585; int _temp1586; struct _tuple15* _temp1583= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1585=*
_temp1583; _LL1587: _temp1586= _temp1585.f2; goto _LL1584; _LL1584: { void* a;
if( Cyc_Std_zstrcmp( _temp1582, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1582, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1586 <  0? 1: _temp1586 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 0 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1588=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1588[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1589; _temp1589.tag= Cyc_Absyn_Regparm_att; _temp1589.f1= _temp1586;
_temp1589;}); _temp1588;});} else{ if( Cyc_Std_zstrcmp( _temp1582, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1582, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1586 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1586;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1590=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1590[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1591; _temp1591.tag= Cyc_Absyn_Aligned_att; _temp1591.f1= _temp1586;
_temp1591;}); _temp1590;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1592=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1592[ 0]=({ struct Cyc_Attribute_tok_struct _temp1593; _temp1593.tag= Cyc_Attribute_tok;
_temp1593.f1=( void*) a; _temp1593;}); _temp1592;}); break;}} case 50: _LL1581: {
struct _tagged_arr _temp1595= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1596= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1595, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1595, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1597=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1597[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1598; _temp1598.tag=
Cyc_Absyn_Section_att; _temp1598.f1= _temp1596; _temp1598;}); _temp1597;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1599=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1599[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1600; _temp1600.tag= Cyc_Attribute_tok; _temp1600.f1=(
void*) a; _temp1600;}); _temp1599;}); break;} case 51: _LL1594: { struct
_tagged_arr _temp1602= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1603= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1606; struct _tuple15 _temp1604=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1607:
_temp1606= _temp1604.f2; goto _LL1605; _LL1605: { int _temp1610; struct _tuple15
_temp1608=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1611: _temp1610= _temp1608.f2; goto _LL1609; _LL1609: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1602, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1602, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1603, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1612=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1612[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1613; _temp1613.tag=
Cyc_Absyn_Format_att; _temp1613.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1613.f2= _temp1606; _temp1613.f3= _temp1610; _temp1613;}); _temp1612;});}
else{ if( Cyc_Std_zstrcmp( _temp1603, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1614=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1614[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1615; _temp1615.tag=
Cyc_Absyn_Format_att; _temp1615.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1615.f2= _temp1606; _temp1615.f3= _temp1610; _temp1615;}); _temp1614;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1616=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1616[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1617; _temp1617.tag= Cyc_Attribute_tok; _temp1617.f1=(
void*) a; _temp1617;}); _temp1616;}); break;}}} case 52: _LL1601: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1619=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1619[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1620; _temp1620.tag= Cyc_TypeSpecifier_tok;
_temp1620.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1620;});
_temp1619;}); break; case 53: _LL1618: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1622=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1622[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1623; _temp1623.tag=
Cyc_TypeSpecifier_tok; _temp1623.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1623;}); _temp1622;}); break; case 54: _LL1621:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1625=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1625[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1626; _temp1626.tag= Cyc_TypeSpecifier_tok;
_temp1626.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1626;});
_temp1625;}); break; case 55: _LL1624: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1628=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1628[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1629; _temp1629.tag=
Cyc_TypeSpecifier_tok; _temp1629.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1630=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1630[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1631; _temp1631.tag=
Cyc_Parse_Short_spec; _temp1631.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1631;});
_temp1630;})); _temp1629;}); _temp1628;}); break; case 56: _LL1627: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1633=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1633[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1634; _temp1634.tag= Cyc_TypeSpecifier_tok;
_temp1634.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1634;});
_temp1633;}); break; case 57: _LL1632: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1636=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1636[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1637; _temp1637.tag=
Cyc_TypeSpecifier_tok; _temp1637.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1638=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1638[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1639; _temp1639.tag= Cyc_Parse_Long_spec;
_temp1639.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1639;}); _temp1638;})); _temp1637;}); _temp1636;});
break; case 58: _LL1635: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1641=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1641[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1642; _temp1642.tag=
Cyc_TypeSpecifier_tok; _temp1642.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1642;}); _temp1641;}); break; case 59: _LL1640:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1644=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1644[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1645; _temp1645.tag= Cyc_TypeSpecifier_tok;
_temp1645.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1645;});
_temp1644;}); break; case 60: _LL1643: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1647=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1647[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1648; _temp1648.tag=
Cyc_TypeSpecifier_tok; _temp1648.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1649=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1649[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1650; _temp1650.tag=
Cyc_Parse_Signed_spec; _temp1650.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1650;});
_temp1649;})); _temp1648;}); _temp1647;}); break; case 61: _LL1646: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1652=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1652[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1653; _temp1653.tag= Cyc_TypeSpecifier_tok;
_temp1653.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1654=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1654[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1655; _temp1655.tag= Cyc_Parse_Unsigned_spec; _temp1655.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1655;});
_temp1654;})); _temp1653;}); _temp1652;}); break; case 62: _LL1651: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1656:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1657: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1658: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1660=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1660[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1661; _temp1661.tag=
Cyc_TypeSpecifier_tok; _temp1661.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1661;});
_temp1660;}); break; case 66: _LL1659: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1663=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1663[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1664; _temp1664.tag=
Cyc_TypeSpecifier_tok; _temp1664.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1665=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1665[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1666; _temp1666.tag= Cyc_Absyn_TypedefType;
_temp1666.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1666.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1666.f3= 0;
_temp1666;}); _temp1665;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1664;});
_temp1663;}); break; case 67: _LL1662: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1668=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1668[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1669; _temp1669.tag=
Cyc_TypeSpecifier_tok; _temp1669.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1670=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1670[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1671; _temp1671.tag= Cyc_Absyn_TupleType;
_temp1671.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1671;});
_temp1670;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1669;}); _temp1668;}); break; case 68:
_LL1667: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1673=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1673[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1674; _temp1674.tag=
Cyc_TypeSpecifier_tok; _temp1674.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1675=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1675[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1676; _temp1676.tag= Cyc_Absyn_RgnHandleType;
_temp1676.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1676;}); _temp1675;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1674;}); _temp1673;}); break; case 69: _LL1672: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1678=( struct Cyc_Kind_tok_struct*) _cycalloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1678[ 0]=({ struct Cyc_Kind_tok_struct _temp1679; _temp1679.tag= Cyc_Kind_tok;
_temp1679.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1679;});
_temp1678;}); break; case 70: _LL1677: { struct _tagged_arr* _temp1683; void*
_temp1685; struct _tuple1 _temp1681=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1686: _temp1685=
_temp1681.f1; goto _LL1684; _LL1684: _temp1683= _temp1681.f2; goto _LL1682;
_LL1682: if( _temp1685 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1687=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1687[ 0]=({ struct Cyc_Kind_tok_struct
_temp1688; _temp1688.tag= Cyc_Kind_tok; _temp1688.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1683, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1688;}); _temp1687;}); break;} case 71:
_LL1680: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1690=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1690[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1691; _temp1691.tag= Cyc_TypeQual_tok;
_temp1691.f1=({ struct Cyc_Absyn_Tqual _temp1692; _temp1692.q_const= 1;
_temp1692.q_volatile= 0; _temp1692.q_restrict= 0; _temp1692;}); _temp1691;});
_temp1690;}); break; case 72: _LL1689: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1694=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1694[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1695; _temp1695.tag= Cyc_TypeQual_tok; _temp1695.f1=({ struct Cyc_Absyn_Tqual
_temp1696; _temp1696.q_const= 0; _temp1696.q_volatile= 1; _temp1696.q_restrict=
0; _temp1696;}); _temp1695;}); _temp1694;}); break; case 73: _LL1693: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1698=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1698[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1699; _temp1699.tag= Cyc_TypeQual_tok;
_temp1699.f1=({ struct Cyc_Absyn_Tqual _temp1700; _temp1700.q_const= 0;
_temp1700.q_volatile= 0; _temp1700.q_restrict= 1; _temp1700;}); _temp1699;});
_temp1698;}); break; case 74: _LL1697: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1702=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1702[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1703; _temp1703.tag=
Cyc_TypeSpecifier_tok; _temp1703.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1704=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1704[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1705; _temp1705.tag= Cyc_Parse_Decl_spec;
_temp1705.f1=({ struct Cyc_Absyn_Decl* _temp1706=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1706->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1707=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1707[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1708; _temp1708.tag= Cyc_Absyn_Enum_d; _temp1708.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1709=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1709->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1709->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1709->fields=({
struct Cyc_Core_Opt* _temp1710=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1710->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1710;});
_temp1709;}); _temp1708;}); _temp1707;})); _temp1706->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1706;}); _temp1705;}); _temp1704;})); _temp1703;}); _temp1702;}); break;
case 75: _LL1701: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1712=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1712[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1713; _temp1713.tag=
Cyc_TypeSpecifier_tok; _temp1713.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1714=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1714[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1715; _temp1715.tag= Cyc_Absyn_EnumType;
_temp1715.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1715.f2= 0; _temp1715;}); _temp1714;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1713;}); _temp1712;}); break; case 76: _LL1711: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1717=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1717[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1718; _temp1718.tag= Cyc_Enumfield_tok;
_temp1718.f1=({ struct Cyc_Absyn_Enumfield* _temp1719=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1719->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1719->tag= 0;
_temp1719->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1719;});
_temp1718;}); _temp1717;}); break; case 77: _LL1716: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1721=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1721[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1722; _temp1722.tag= Cyc_Enumfield_tok;
_temp1722.f1=({ struct Cyc_Absyn_Enumfield* _temp1723=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1723->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1723->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1723->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1723;});
_temp1722;}); _temp1721;}); break; case 78: _LL1720: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1725=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1725[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1726; _temp1726.tag=
Cyc_EnumfieldList_tok; _temp1726.f1=({ struct Cyc_List_List* _temp1727=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1727->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1727->tl= 0; _temp1727;}); _temp1726;}); _temp1725;});
break; case 79: _LL1724: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1729=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1729[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1730; _temp1730.tag=
Cyc_EnumfieldList_tok; _temp1730.f1=({ struct Cyc_List_List* _temp1731=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1731->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1731->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1731;}); _temp1730;});
_temp1729;}); break; case 80: _LL1728: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1733: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1735=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1735[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1736; _temp1736.tag=
Cyc_Absyn_AnonStructType; _temp1736.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1736;});
_temp1735;}); break; case Cyc_Parse_Union_su: _LL1734: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1738=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1738[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1739; _temp1739.tag= Cyc_Absyn_AnonUnionType; _temp1739.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1739;});
_temp1738;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1740=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1740[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1741; _temp1741.tag=
Cyc_TypeSpecifier_tok; _temp1741.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1742=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1742[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1743; _temp1743.tag= Cyc_Parse_Type_spec;
_temp1743.f1=( void*) t; _temp1743.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1743;});
_temp1742;})); _temp1741;}); _temp1740;}); break;} case 81: _LL1732: { struct
Cyc_List_List* _temp1745=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
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
case 82: _LL1744: { struct Cyc_List_List* _temp1758=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1759: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1761=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1761->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1761;}), _temp1758,({ struct Cyc_Core_Opt*
_temp1762=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1762->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1762;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1760: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1764=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1764->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1764;}),
_temp1758,({ struct Cyc_Core_Opt* _temp1765=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1765->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1765;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1766=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1766[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1767; _temp1767.tag=
Cyc_TypeSpecifier_tok; _temp1767.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1768=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1768[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1769; _temp1769.tag= Cyc_Parse_Decl_spec;
_temp1769.f1= d; _temp1769;}); _temp1768;})); _temp1767;}); _temp1766;}); break;}
case 83: _LL1757: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1771: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1773=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1773[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1774; _temp1774.tag=
Cyc_TypeSpecifier_tok; _temp1774.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1775=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1775[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1776; _temp1776.tag= Cyc_Absyn_StructType;
_temp1776.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1776.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1776.f3= 0;
_temp1776;}); _temp1775;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1774;});
_temp1773;}); break; case Cyc_Parse_Union_su: _LL1772: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1778=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1778[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1779; _temp1779.tag= Cyc_TypeSpecifier_tok;
_temp1779.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1780=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1780[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1781; _temp1781.tag= Cyc_Absyn_UnionType;
_temp1781.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1781.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1781.f3= 0;
_temp1781;}); _temp1780;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1779;});
_temp1778;}); break;} break; case 84: _LL1770: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1783: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1785=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1785[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1786; _temp1786.tag=
Cyc_TypeSpecifier_tok; _temp1786.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1787=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1787[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1788; _temp1788.tag= Cyc_Absyn_StructType;
_temp1788.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1788.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1788.f3= 0;
_temp1788;}); _temp1787;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1786;});
_temp1785;}); break; case Cyc_Parse_Union_su: _LL1784: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1790=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1790[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1791; _temp1791.tag= Cyc_TypeSpecifier_tok;
_temp1791.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1792=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1792[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1793; _temp1793.tag= Cyc_Absyn_UnionType;
_temp1793.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1793.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1793.f3= 0;
_temp1793;}); _temp1792;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1791;});
_temp1790;}); break;} break; case 85: _LL1782: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1795=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1795[ 0]=({ struct Cyc_TypeList_tok_struct _temp1796; _temp1796.tag= Cyc_TypeList_tok;
_temp1796.f1= 0; _temp1796;}); _temp1795;}); break; case 86: _LL1794: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1798=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1798[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1799; _temp1799.tag= Cyc_TypeList_tok; _temp1799.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1799;});
_temp1798;}); break; case 87: _LL1797:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1801=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1801[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1802; _temp1802.tag= Cyc_TypeList_tok; _temp1802.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1802;});
_temp1801;}); break; case 88: _LL1800: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1804=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1804[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1805; _temp1805.tag= Cyc_StructOrUnion_tok; _temp1805.f1= Cyc_Parse_Struct_su;
_temp1805;}); _temp1804;}); break; case 89: _LL1803: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1807=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1807[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1808; _temp1808.tag= Cyc_StructOrUnion_tok; _temp1808.f1= Cyc_Parse_Union_su;
_temp1808;}); _temp1807;}); break; case 90: _LL1806: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1810=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1810[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1811; _temp1811.tag= Cyc_StructFieldDeclList_tok; _temp1811.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1811;});
_temp1810;}); break; case 91: _LL1809: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1813=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1813[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1814; _temp1814.tag= Cyc_StructFieldDeclListList_tok; _temp1814.f1=({
struct Cyc_List_List* _temp1815=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1815->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1815->tl= 0;
_temp1815;}); _temp1814;}); _temp1813;}); break; case 92: _LL1812: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1817=( struct Cyc_StructFieldDeclListList_tok_struct*)
_cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1817[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1818; _temp1818.tag= Cyc_StructFieldDeclListList_tok;
_temp1818.f1=({ struct Cyc_List_List* _temp1819=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1819->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1819->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1819;}); _temp1818;}); _temp1817;}); break;
case 93: _LL1816: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1821=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1821[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1822; _temp1822.tag=
Cyc_InitDeclList_tok; _temp1822.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1822;});
_temp1821;}); break; case 94: _LL1820: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1824=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1824[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1825; _temp1825.tag=
Cyc_InitDeclList_tok; _temp1825.f1=({ struct Cyc_List_List* _temp1826=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1826->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1826->tl= 0; _temp1826;}); _temp1825;}); _temp1824;});
break; case 95: _LL1823: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1828=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1828[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1829; _temp1829.tag=
Cyc_InitDeclList_tok; _temp1829.f1=({ struct Cyc_List_List* _temp1830=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1830->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1830->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1830;});
_temp1829;}); _temp1828;}); break; case 96: _LL1827: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1832=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1832[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1833; _temp1833.tag= Cyc_InitDecl_tok;
_temp1833.f1=({ struct _tuple14* _temp1834=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1834->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1834->f2= 0;
_temp1834;}); _temp1833;}); _temp1832;}); break; case 97: _LL1831: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1836=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1836[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1837; _temp1837.tag= Cyc_InitDecl_tok; _temp1837.f1=({ struct _tuple14*
_temp1838=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1838->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1838->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1838;});
_temp1837;}); _temp1836;}); break; case 98: _LL1835: { struct _tuple17 _temp1842;
struct Cyc_List_List* _temp1843; struct Cyc_List_List* _temp1845; struct Cyc_Absyn_Tqual
_temp1847; struct _tuple17* _temp1840= Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1842=*
_temp1840; _LL1848: _temp1847= _temp1842.f1; goto _LL1846; _LL1846: _temp1845=
_temp1842.f2; goto _LL1844; _LL1844: _temp1843= _temp1842.f3; goto _LL1841;
_LL1841: { struct Cyc_List_List* _temp1851; struct Cyc_List_List* _temp1853;
struct _tuple0 _temp1849=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1854: _temp1853= _temp1849.f1; goto _LL1852; _LL1852:
_temp1851= _temp1849.f2; goto _LL1850; _LL1850: { void* _temp1855= Cyc_Parse_speclist2typ(
_temp1845, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1856=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1847, _temp1855, _temp1853, _temp1843), _temp1851);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1857=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1857[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1858; _temp1858.tag=
Cyc_StructFieldDeclList_tok; _temp1858.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1856); _temp1858;}); _temp1857;}); break;}}}
case 99: _LL1839: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1860=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1860[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1861; _temp1861.tag=
Cyc_QualSpecList_tok; _temp1861.f1=({ struct _tuple17* _temp1862=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1862->f1= Cyc_Absyn_empty_tqual();
_temp1862->f2=({ struct Cyc_List_List* _temp1863=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1863->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1863->tl=
0; _temp1863;}); _temp1862->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1862;}); _temp1861;});
_temp1860;}); break; case 100: _LL1859: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1865=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1865[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1866; _temp1866.tag=
Cyc_QualSpecList_tok; _temp1866.f1=({ struct _tuple17* _temp1867=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1867->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1867->f2=({
struct Cyc_List_List* _temp1868=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1868->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1868->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1868;}); _temp1867->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1867;});
_temp1866;}); _temp1865;}); break; case 101: _LL1864: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1870=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1870[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1871; _temp1871.tag= Cyc_QualSpecList_tok;
_temp1871.f1=({ struct _tuple17* _temp1872=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1872->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1872->f2= 0;
_temp1872->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1872;}); _temp1871;}); _temp1870;}); break; case
102: _LL1869: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1874=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1874[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1875; _temp1875.tag=
Cyc_QualSpecList_tok; _temp1875.f1=({ struct _tuple17* _temp1876=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1876->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1876->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1876->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1876;});
_temp1875;}); _temp1874;}); break; case 103: _LL1873: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1878=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1878[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1879; _temp1879.tag= Cyc_InitDeclList_tok;
_temp1879.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1879;}); _temp1878;}); break; case 104: _LL1877: yyval=(
void*)({ struct Cyc_InitDeclList_tok_struct* _temp1881=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1881[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1882; _temp1882.tag= Cyc_InitDeclList_tok;
_temp1882.f1=({ struct Cyc_List_List* _temp1883=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1883->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1883->tl= 0;
_temp1883;}); _temp1882;}); _temp1881;}); break; case 105: _LL1880: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp1885=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1885[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1886; _temp1886.tag= Cyc_InitDeclList_tok;
_temp1886.f1=({ struct Cyc_List_List* _temp1887=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1887->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1887->tl=
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1887;}); _temp1886;}); _temp1885;}); break; case 106:
_LL1884: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp1889=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1889[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1890; _temp1890.tag= Cyc_InitDecl_tok; _temp1890.f1=({ struct _tuple14*
_temp1891=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1891->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1891->f2= 0; _temp1891;}); _temp1890;}); _temp1889;});
break; case 107: _LL1888: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1893=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1893[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1894; _temp1894.tag= Cyc_InitDecl_tok;
_temp1894.f1=({ struct _tuple14* _temp1895=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1895->f1=({ struct Cyc_Parse_Declarator*
_temp1896=( struct Cyc_Parse_Declarator*) _cycalloc( sizeof( struct Cyc_Parse_Declarator));
_temp1896->id=({ struct _tuple1* _temp1897=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp1897->f1= Cyc_Absyn_rel_ns_null; _temp1897->f2=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1897;}); _temp1896->tms= 0; _temp1896;});
_temp1895->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1895;}); _temp1894;});
_temp1893;}); break; case 108: _LL1892: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1899=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1899[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1900; _temp1900.tag= Cyc_InitDecl_tok;
_temp1900.f1=({ struct _tuple14* _temp1901=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1901->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1901->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1901;}); _temp1900;}); _temp1899;}); break; case
109: _LL1898: { struct Cyc_List_List* _temp1903=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1904=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1904[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1905; _temp1905.tag=
Cyc_TypeSpecifier_tok; _temp1905.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1906=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1906[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1907; _temp1907.tag= Cyc_Parse_Decl_spec;
_temp1907.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1903,({
struct Cyc_Core_Opt* _temp1908=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1908->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1908;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1907;}); _temp1906;})); _temp1905;}); _temp1904;}); break;} case 110:
_LL1902: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1910=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1910[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1911; _temp1911.tag=
Cyc_TypeSpecifier_tok; _temp1911.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1912=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1912[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1913; _temp1913.tag= Cyc_Absyn_TunionType;
_temp1913.f1=({ struct Cyc_Absyn_TunionInfo _temp1914; _temp1914.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1915=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1915[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1916; _temp1916.tag= Cyc_Absyn_UnknownTunion;
_temp1916.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1917; _temp1917.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1917.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1917;});
_temp1916;}); _temp1915;})); _temp1914.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1914.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1914;}); _temp1913;}); _temp1912;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1911;}); _temp1910;}); break; case 111: _LL1909: { void* _temp1919= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp1928=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1928->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp1928;}),
0); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1920=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1920[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1921; _temp1921.tag= Cyc_TypeSpecifier_tok;
_temp1921.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1922=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1922[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1923; _temp1923.tag=
Cyc_Absyn_TunionType; _temp1923.f1=({ struct Cyc_Absyn_TunionInfo _temp1924;
_temp1924.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1925=( struct Cyc_Absyn_UnknownTunion_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1925[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1926; _temp1926.tag= Cyc_Absyn_UnknownTunion; _temp1926.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1927; _temp1927.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1927.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1927;}); _temp1926;}); _temp1925;})); _temp1924.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1924.rgn=(
void*) _temp1919; _temp1924;}); _temp1923;}); _temp1922;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1921;}); _temp1920;}); break;} case 112: _LL1918: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1930=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1930[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1931; _temp1931.tag= Cyc_TypeSpecifier_tok;
_temp1931.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1932=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1932[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1933; _temp1933.tag= Cyc_Absyn_TunionFieldType; _temp1933.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1934; _temp1934.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp1935=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp1935[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp1936; _temp1936.tag= Cyc_Absyn_UnknownTunionfield; _temp1936.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp1937; _temp1937.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1937.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1937.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1937;});
_temp1936;}); _temp1935;})); _temp1934.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1934;}); _temp1933;});
_temp1932;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1931;}); _temp1930;}); break; case 113:
_LL1929: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1939=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1939[ 0]=({ struct
Cyc_Bool_tok_struct _temp1940; _temp1940.tag= Cyc_Bool_tok; _temp1940.f1= 0;
_temp1940;}); _temp1939;}); break; case 114: _LL1938: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1942=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1942[ 0]=({ struct Cyc_Bool_tok_struct
_temp1943; _temp1943.tag= Cyc_Bool_tok; _temp1943.f1= 1; _temp1943;}); _temp1942;});
break; case 115: _LL1941: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1945=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1945[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1946; _temp1946.tag= Cyc_TunionFieldList_tok; _temp1946.f1=({ struct Cyc_List_List*
_temp1947=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1947->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1947->tl= 0;
_temp1947;}); _temp1946;}); _temp1945;}); break; case 116: _LL1944: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp1949=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1949[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1950; _temp1950.tag= Cyc_TunionFieldList_tok;
_temp1950.f1=({ struct Cyc_List_List* _temp1951=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1951->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1951->tl=
0; _temp1951;}); _temp1950;}); _temp1949;}); break; case 117: _LL1948: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1953=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1953[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1954; _temp1954.tag= Cyc_TunionFieldList_tok;
_temp1954.f1=({ struct Cyc_List_List* _temp1955=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1955->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1955->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1955;}); _temp1954;}); _temp1953;}); break; case 118:
_LL1952: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1957=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp1957[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp1958; _temp1958.tag=
Cyc_TunionFieldList_tok; _temp1958.f1=({ struct Cyc_List_List* _temp1959=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1959->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1959->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1959;}); _temp1958;});
_temp1957;}); break; case 119: _LL1956: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1961=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1961[ 0]=({ struct Cyc_Scope_tok_struct _temp1962; _temp1962.tag= Cyc_Scope_tok;
_temp1962.f1=( void*)(( void*) Cyc_Absyn_Public); _temp1962;}); _temp1961;});
break; case 120: _LL1960: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1964=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1964[ 0]=({ struct Cyc_Scope_tok_struct _temp1965; _temp1965.tag= Cyc_Scope_tok;
_temp1965.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp1965;}); _temp1964;});
break; case 121: _LL1963: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1967=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1967[ 0]=({ struct Cyc_Scope_tok_struct _temp1968; _temp1968.tag= Cyc_Scope_tok;
_temp1968.f1=( void*)(( void*) Cyc_Absyn_Static); _temp1968;}); _temp1967;});
break; case 122: _LL1966: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp1970=( struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp1970[ 0]=({ struct Cyc_TunionField_tok_struct _temp1971; _temp1971.tag= Cyc_TunionField_tok;
_temp1971.f1=({ struct Cyc_Absyn_Tunionfield* _temp1972=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1972->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1972->tvs= 0;
_temp1972->typs= 0; _temp1972->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1972->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1972;}); _temp1971;}); _temp1970;}); break; case 123:
_LL1969: { struct Cyc_List_List* _temp1974=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp1975=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp1976=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp1976[ 0]=({ struct
Cyc_TunionField_tok_struct _temp1977; _temp1977.tag= Cyc_TunionField_tok;
_temp1977.f1=({ struct Cyc_Absyn_Tunionfield* _temp1978=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1978->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1978->tvs=
_temp1975; _temp1978->typs= _temp1974; _temp1978->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1978->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp1978;}); _temp1977;}); _temp1976;}); break;}
case 124: _LL1973: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 125: _LL1979: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1981=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1981[ 0]=({ struct Cyc_Declarator_tok_struct _temp1982; _temp1982.tag= Cyc_Declarator_tok;
_temp1982.f1=({ struct Cyc_Parse_Declarator* _temp1983=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1983->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp1983->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp1983;});
_temp1982;}); _temp1981;}); break; case 126: _LL1980: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp1985=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp1985[ 0]=({ struct
Cyc_Declarator_tok_struct _temp1986; _temp1986.tag= Cyc_Declarator_tok;
_temp1986.f1=({ struct Cyc_Parse_Declarator* _temp1987=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1987->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1987->tms= 0;
_temp1987;}); _temp1986;}); _temp1985;}); break; case 127: _LL1984: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 128:
_LL1988: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp1990=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1990[ 0]=({ struct Cyc_Declarator_tok_struct _temp1991; _temp1991.tag= Cyc_Declarator_tok;
_temp1991.f1=({ struct Cyc_Parse_Declarator* _temp1992=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1992->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp1992->tms=({ struct Cyc_List_List* _temp1993=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1993->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp1993->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp1993;}); _temp1992;}); _temp1991;});
_temp1990;}); break; case 129: _LL1989: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1995=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1995[ 0]=({ struct Cyc_Declarator_tok_struct _temp1996; _temp1996.tag= Cyc_Declarator_tok;
_temp1996.f1=({ struct Cyc_Parse_Declarator* _temp1997=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp1997->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp1997->tms=({ struct Cyc_List_List* _temp1998=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1998->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp1999=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1999[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2000; _temp2000.tag= Cyc_Absyn_ConstArray_mod;
_temp2000.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2000;}); _temp1999;})); _temp1998->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp1998;}); _temp1997;}); _temp1996;}); _temp1995;}); break; case 130: _LL1994: {
struct _tuple16 _temp2004; struct Cyc_List_List* _temp2005; struct Cyc_Core_Opt*
_temp2007; struct Cyc_Absyn_VarargInfo* _temp2009; int _temp2011; struct Cyc_List_List*
_temp2013; struct _tuple16* _temp2002= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2004=*
_temp2002; _LL2014: _temp2013= _temp2004.f1; goto _LL2012; _LL2012: _temp2011=
_temp2004.f2; goto _LL2010; _LL2010: _temp2009= _temp2004.f3; goto _LL2008;
_LL2008: _temp2007= _temp2004.f4; goto _LL2006; _LL2006: _temp2005= _temp2004.f5;
goto _LL2003; _LL2003: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2015=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2015[ 0]=({ struct Cyc_Declarator_tok_struct _temp2016; _temp2016.tag= Cyc_Declarator_tok;
_temp2016.f1=({ struct Cyc_Parse_Declarator* _temp2017=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2017->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2017->tms=({ struct Cyc_List_List* _temp2018=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2018->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2019=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2019[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2020; _temp2020.tag= Cyc_Absyn_Function_mod;
_temp2020.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2021=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2021[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2022; _temp2022.tag= Cyc_Absyn_WithTypes;
_temp2022.f1= _temp2013; _temp2022.f2= _temp2011; _temp2022.f3= _temp2009;
_temp2022.f4= _temp2007; _temp2022.f5= _temp2005; _temp2022;}); _temp2021;}));
_temp2020;}); _temp2019;})); _temp2018->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2018;});
_temp2017;}); _temp2016;}); _temp2015;}); break;} case 131: _LL2001: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2024=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2024[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2025; _temp2025.tag= Cyc_Declarator_tok;
_temp2025.f1=({ struct Cyc_Parse_Declarator* _temp2026=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2026->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2026->tms=({ struct Cyc_List_List* _temp2027=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2027->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2028=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2028[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2029; _temp2029.tag= Cyc_Absyn_Function_mod;
_temp2029.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2030=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2030[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2031; _temp2031.tag= Cyc_Absyn_WithTypes;
_temp2031.f1= 0; _temp2031.f2= 0; _temp2031.f3= 0; _temp2031.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2031.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2031;}); _temp2030;})); _temp2029;}); _temp2028;}));
_temp2027->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2027;}); _temp2026;}); _temp2025;});
_temp2024;}); break; case 132: _LL2023: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2033=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2033[ 0]=({ struct Cyc_Declarator_tok_struct _temp2034; _temp2034.tag= Cyc_Declarator_tok;
_temp2034.f1=({ struct Cyc_Parse_Declarator* _temp2035=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2035->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2035->tms=({ struct Cyc_List_List* _temp2036=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2036->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2037=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2037[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2038; _temp2038.tag= Cyc_Absyn_Function_mod;
_temp2038.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2039=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2039[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2040; _temp2040.tag= Cyc_Absyn_NoTypes;
_temp2040.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2040.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2040;});
_temp2039;})); _temp2038;}); _temp2037;})); _temp2036->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2036;}); _temp2035;}); _temp2034;}); _temp2033;}); break; case 133: _LL2032: {
struct Cyc_List_List* _temp2042=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2043=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2043[ 0]=({ struct Cyc_Declarator_tok_struct _temp2044; _temp2044.tag= Cyc_Declarator_tok;
_temp2044.f1=({ struct Cyc_Parse_Declarator* _temp2045=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2045->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2045->tms=({ struct Cyc_List_List* _temp2046=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2046->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2047=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2047[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2048; _temp2048.tag= Cyc_Absyn_TypeParams_mod;
_temp2048.f1= _temp2042; _temp2048.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2048.f3=
0; _temp2048;}); _temp2047;})); _temp2046->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2046;});
_temp2045;}); _temp2044;}); _temp2043;}); break;} case 134: _LL2041:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2050=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2051=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2051[ 0]=({ struct Cyc_Declarator_tok_struct _temp2052; _temp2052.tag= Cyc_Declarator_tok;
_temp2052.f1=({ struct Cyc_Parse_Declarator* _temp2053=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2053->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2053->tms=({ struct Cyc_List_List* _temp2054=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2054->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2055=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2055[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2056; _temp2056.tag= Cyc_Absyn_TypeParams_mod;
_temp2056.f1= _temp2050; _temp2056.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2056.f3=
0; _temp2056;}); _temp2055;})); _temp2054->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2054;});
_temp2053;}); _temp2052;}); _temp2051;}); break;} case 135: _LL2049: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2058=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2058[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2059; _temp2059.tag= Cyc_Declarator_tok;
_temp2059.f1=({ struct Cyc_Parse_Declarator* _temp2060=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2060->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2060->tms=({ struct Cyc_List_List* _temp2061=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2061->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2062=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2062[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2063; _temp2063.tag= Cyc_Absyn_Attributes_mod;
_temp2063.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2063.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2063;}); _temp2062;}));
_temp2061->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]))->tms; _temp2061;}); _temp2060;}); _temp2059;});
_temp2058;}); break; case 136: _LL2057: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2065=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2065[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2066; _temp2066.tag= Cyc_Declarator_tok;
_temp2066.f1=({ struct Cyc_Parse_Declarator* _temp2067=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2067->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2067->tms= 0;
_temp2067;}); _temp2066;}); _temp2065;}); break; case 137: _LL2064: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2069=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2069[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2070; _temp2070.tag= Cyc_Declarator_tok;
_temp2070.f1=({ struct Cyc_Parse_Declarator* _temp2071=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2071->id= Cyc_Absyn_exn_name;
_temp2071->tms= 0; _temp2071;}); _temp2070;}); _temp2069;}); break; case 138:
_LL2068: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2073=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2073[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2074; _temp2074.tag=
Cyc_TypeModifierList_tok; _temp2074.f1=({ struct Cyc_List_List* _temp2075=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2075->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2076=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2076[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2077; _temp2077.tag= Cyc_Absyn_Pointer_mod;
_temp2077.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2077.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2077.f3= Cyc_Absyn_empty_tqual(); _temp2077;});
_temp2076;})); _temp2075->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2075;});
_temp2074;}); _temp2073;}); break; case 139: _LL2072: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2079=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2079[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2080; _temp2080.tag= Cyc_TypeModifierList_tok;
_temp2080.f1=({ struct Cyc_List_List* _temp2081=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2081->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2082=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2082[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2083; _temp2083.tag= Cyc_Absyn_Pointer_mod;
_temp2083.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2083.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2083.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2083;}); _temp2082;}));
_temp2081->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0);
_temp2081;}); _temp2080;}); _temp2079;}); break; case 140: _LL2078: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2085=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2085[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2086; _temp2086.tag= Cyc_TypeModifierList_tok;
_temp2086.f1=({ struct Cyc_List_List* _temp2087=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2087->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2088=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2088[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2089; _temp2089.tag= Cyc_Absyn_Pointer_mod;
_temp2089.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2089.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2089.f3= Cyc_Absyn_empty_tqual(); _temp2089;});
_temp2088;})); _temp2087->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2087;});
_temp2086;}); _temp2085;}); break; case 141: _LL2084: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2091=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2091[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2092; _temp2092.tag= Cyc_TypeModifierList_tok;
_temp2092.f1=({ struct Cyc_List_List* _temp2093=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2093->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2094=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2094[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2095; _temp2095.tag= Cyc_Absyn_Pointer_mod;
_temp2095.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2095.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2095.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2095;});
_temp2094;})); _temp2093->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2093;});
_temp2092;}); _temp2091;}); break; case 142: _LL2090: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2097=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2097[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2098; _temp2098.tag= Cyc_Pointer_Sort_tok;
_temp2098.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2099=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2099[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2100; _temp2100.tag=
Cyc_Absyn_Nullable_ps; _temp2100.f1= Cyc_Absyn_exp_unsigned_one; _temp2100;});
_temp2099;})); _temp2098;}); _temp2097;}); break; case 143: _LL2096: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2102=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2102[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2103; _temp2103.tag= Cyc_Pointer_Sort_tok;
_temp2103.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2104=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2104[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2105; _temp2105.tag= Cyc_Absyn_NonNullable_ps; _temp2105.f1= Cyc_Absyn_exp_unsigned_one;
_temp2105;}); _temp2104;})); _temp2103;}); _temp2102;}); break; case 144:
_LL2101: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2107=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2107[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2108; _temp2108.tag=
Cyc_Pointer_Sort_tok; _temp2108.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2109=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2109[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2110; _temp2110.tag=
Cyc_Absyn_Nullable_ps; _temp2110.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2110;});
_temp2109;})); _temp2108;}); _temp2107;}); break; case 145: _LL2106: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2112=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2112[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2113; _temp2113.tag= Cyc_Pointer_Sort_tok;
_temp2113.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2114=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2114[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2115; _temp2115.tag= Cyc_Absyn_NonNullable_ps; _temp2115.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2115;});
_temp2114;})); _temp2113;}); _temp2112;}); break; case 146: _LL2111: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2117=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2117[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2118; _temp2118.tag= Cyc_Pointer_Sort_tok;
_temp2118.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2118;});
_temp2117;}); break; case 147: _LL2116: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2120=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2120[ 0]=({ struct Cyc_Type_tok_struct _temp2121; _temp2121.tag= Cyc_Type_tok;
_temp2121.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2122=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2122->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2122;}), 0); _temp2121;}); _temp2120;});
break; case 148: _LL2119: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 149: _LL2123: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
150: _LL2124: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2126=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2126[
0]=({ struct Cyc_Type_tok_struct _temp2127; _temp2127.tag= Cyc_Type_tok;
_temp2127.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2128=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2128->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2128;}), 0); _temp2127;}); _temp2126;});
break; case 151: _LL2125: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 152: _LL2129: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2131=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2131[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2132; _temp2132.tag= Cyc_TypeQual_tok; _temp2132.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2132;});
_temp2131;}); break; case 153: _LL2130: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2134=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2134[ 0]=({ struct Cyc_YY1_struct _temp2135; _temp2135.tag= Cyc_YY1;
_temp2135.f1=({ struct _tuple16* _temp2136=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2136->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2136->f2= 0;
_temp2136->f3= 0; _temp2136->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2136->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2136;});
_temp2135;}); _temp2134;}); break; case 154: _LL2133: yyval=( void*)({ struct
Cyc_YY1_struct* _temp2138=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct
Cyc_YY1_struct)); _temp2138[ 0]=({ struct Cyc_YY1_struct _temp2139; _temp2139.tag=
Cyc_YY1; _temp2139.f1=({ struct _tuple16* _temp2140=( struct _tuple16*)
_cycalloc( sizeof( struct _tuple16)); _temp2140->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2140->f2= 1;
_temp2140->f3= 0; _temp2140->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2140->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2140;});
_temp2139;}); _temp2138;}); break; case 155: _LL2137: { struct _tuple2 _temp2144;
void* _temp2145; struct Cyc_Absyn_Tqual _temp2147; struct Cyc_Core_Opt*
_temp2149; struct _tuple2* _temp2142= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2144=*
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
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2154->f1= 0;
_temp2154->f2= 0; _temp2154->f3= _temp2151; _temp2154->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2154->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2154;}); _temp2153;}); _temp2152;}); break;}} case 156:
_LL2141: { struct _tuple2 _temp2159; void* _temp2160; struct Cyc_Absyn_Tqual
_temp2162; struct Cyc_Core_Opt* _temp2164; struct _tuple2* _temp2157= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2159=*
_temp2157; _LL2165: _temp2164= _temp2159.f1; goto _LL2163; _LL2163: _temp2162=
_temp2159.f2; goto _LL2161; _LL2161: _temp2160= _temp2159.f3; goto _LL2158;
_LL2158: { struct Cyc_Absyn_VarargInfo* _temp2166=({ struct Cyc_Absyn_VarargInfo*
_temp2170=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2170->name= _temp2164; _temp2170->tq= _temp2162; _temp2170->type=( void*)
_temp2160; _temp2170->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2170;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2167=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2167[ 0]=({ struct Cyc_YY1_struct
_temp2168; _temp2168.tag= Cyc_YY1; _temp2168.f1=({ struct _tuple16* _temp2169=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2169->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2169->f2=
0; _temp2169->f3= _temp2166; _temp2169->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2169->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2169;});
_temp2168;}); _temp2167;}); break;}} case 157: _LL2156: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2172=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2172[ 0]=({ struct Cyc_Type_tok_struct
_temp2173; _temp2173.tag= Cyc_Type_tok; _temp2173.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()); _temp2173;}); _temp2172;}); break; case 158: _LL2171:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2175=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2175[ 0]=({ struct Cyc_Type_tok_struct
_temp2176; _temp2176.tag= Cyc_Type_tok; _temp2176.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp2176;});
_temp2175;}); break; case 159: _LL2174: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2178=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2178[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2179; _temp2179.tag= Cyc_TypeOpt_tok;
_temp2179.f1= 0; _temp2179;}); _temp2178;}); break; case 160: _LL2177: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2181=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2181[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2182; _temp2182.tag= Cyc_TypeOpt_tok; _temp2182.f1=({ struct Cyc_Core_Opt*
_temp2183=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2183->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2184=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2184[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2185; _temp2185.tag= Cyc_Absyn_JoinEff;
_temp2185.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2185;}); _temp2184;})); _temp2183;}); _temp2182;});
_temp2181;}); break; case 161: _LL2180: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2187=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2187[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2188; _temp2188.tag= Cyc_Rgnorder_tok;
_temp2188.f1= 0; _temp2188;}); _temp2187;}); break; case 162: _LL2186: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL2189: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2191=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2191[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2192; _temp2192.tag= Cyc_Rgnorder_tok; _temp2192.f1=({ struct Cyc_List_List*
_temp2193=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2193->hd=( void*)({ struct _tuple18* _temp2194=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2194->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2194->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2194;}); _temp2193->tl= 0; _temp2193;});
_temp2192;}); _temp2191;}); break; case 164: _LL2190: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2196=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2196[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2197; _temp2197.tag= Cyc_Rgnorder_tok; _temp2197.f1=({ struct Cyc_List_List*
_temp2198=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2198->hd=( void*)({ struct _tuple18* _temp2199=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2199->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2199->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2199;}); _temp2198->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2198;});
_temp2197;}); _temp2196;}); break; case 165: _LL2195: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2201=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2201[ 0]=({ struct Cyc_Bool_tok_struct
_temp2202; _temp2202.tag= Cyc_Bool_tok; _temp2202.f1= 0; _temp2202;}); _temp2201;});
break; case 166: _LL2200: { struct _tagged_arr _temp2204= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Std_zstrcmp(
_temp2204, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2205=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2205[ 0]=({ struct Cyc_Bool_tok_struct _temp2206; _temp2206.tag= Cyc_Bool_tok;
_temp2206.f1= 1; _temp2206;}); _temp2205;}); break;} case 167: _LL2203: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168:
_LL2207: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2209=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2209[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2210; _temp2210.tag= Cyc_TypeList_tok; _temp2210.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2210;});
_temp2209;}); break; case 169: _LL2208: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2212=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2212[ 0]=({ struct Cyc_TypeList_tok_struct _temp2213; _temp2213.tag= Cyc_TypeList_tok;
_temp2213.f1= 0; _temp2213;}); _temp2212;}); break; case 170: _LL2211: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case
171: _LL2214: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2216=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2216[ 0]=({ struct Cyc_TypeList_tok_struct _temp2217; _temp2217.tag= Cyc_TypeList_tok;
_temp2217.f1=({ struct Cyc_List_List* _temp2218=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2218->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp2219=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp2219[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp2220; _temp2220.tag= Cyc_Absyn_RgnsEff; _temp2220.f1=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2220;}); _temp2219;})); _temp2218->tl= 0; _temp2218;});
_temp2217;}); _temp2216;}); break; case 172: _LL2215: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2222=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2222[ 0]=({ struct Cyc_TypeList_tok_struct _temp2223; _temp2223.tag= Cyc_TypeList_tok;
_temp2223.f1=({ struct Cyc_List_List* _temp2224=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2224->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2224->tl= 0;
_temp2224;}); _temp2223;}); _temp2222;}); break; case 173: _LL2221: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2226=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2226[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2227; _temp2227.tag= Cyc_TypeList_tok; _temp2227.f1=({ struct Cyc_List_List*
_temp2228=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2228->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2229=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2229[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2230; _temp2230.tag= Cyc_Absyn_AccessEff;
_temp2230.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2230;}); _temp2229;})); _temp2228->tl= 0;
_temp2228;}); _temp2227;}); _temp2226;}); break; case 174: _LL2225: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),( void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2232=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2232[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2233; _temp2233.tag= Cyc_TypeList_tok; _temp2233.f1=({ struct Cyc_List_List*
_temp2234=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2234->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2235=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2235[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2236; _temp2236.tag= Cyc_Absyn_AccessEff;
_temp2236.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2236;}); _temp2235;})); _temp2234->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2234;});
_temp2233;}); _temp2232;}); break; case 175: _LL2231: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2238=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2238[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2239; _temp2239.tag= Cyc_ParamDeclList_tok;
_temp2239.f1=({ struct Cyc_List_List* _temp2240=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2240->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2240->tl= 0;
_temp2240;}); _temp2239;}); _temp2238;}); break; case 176: _LL2237: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2242=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2242[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2243; _temp2243.tag= Cyc_ParamDeclList_tok;
_temp2243.f1=({ struct Cyc_List_List* _temp2244=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2244->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2244->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2244;}); _temp2243;}); _temp2242;}); break; case 177:
_LL2241: { struct _tuple17 _temp2248; struct Cyc_List_List* _temp2249; struct
Cyc_List_List* _temp2251; struct Cyc_Absyn_Tqual _temp2253; struct _tuple17*
_temp2246= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2248=* _temp2246; _LL2254: _temp2253=
_temp2248.f1; goto _LL2252; _LL2252: _temp2251= _temp2248.f2; goto _LL2250;
_LL2250: _temp2249= _temp2248.f3; goto _LL2247; _LL2247: { struct Cyc_Parse_Declarator
_temp2257; struct Cyc_List_List* _temp2258; struct _tuple1* _temp2260; struct
Cyc_Parse_Declarator* _temp2255= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2257=* _temp2255;
_LL2261: _temp2260= _temp2257.id; goto _LL2259; _LL2259: _temp2258= _temp2257.tms;
goto _LL2256; _LL2256: { void* _temp2262= Cyc_Parse_speclist2typ( _temp2251, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2265; struct Cyc_List_List* _temp2267; void*
_temp2269; struct Cyc_Absyn_Tqual _temp2271; struct _tuple6 _temp2263= Cyc_Parse_apply_tms(
_temp2253, _temp2262, _temp2249, _temp2258); _LL2272: _temp2271= _temp2263.f1;
goto _LL2270; _LL2270: _temp2269= _temp2263.f2; goto _LL2268; _LL2268: _temp2267=
_temp2263.f3; goto _LL2266; _LL2266: _temp2265= _temp2263.f4; goto _LL2264;
_LL2264: if( _temp2267 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2260)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2273=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2277=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2277->v=( void*)(* _temp2260).f2; _temp2277;}); if( _temp2265 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2274=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2274[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2275; _temp2275.tag= Cyc_ParamDecl_tok;
_temp2275.f1=({ struct _tuple2* _temp2276=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2276->f1= _temp2273; _temp2276->f2= _temp2271; _temp2276->f3=
_temp2269; _temp2276;}); _temp2275;}); _temp2274;}); break;}}}} case 178:
_LL2245: { struct _tuple17 _temp2281; struct Cyc_List_List* _temp2282; struct
Cyc_List_List* _temp2284; struct Cyc_Absyn_Tqual _temp2286; struct _tuple17*
_temp2279= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2281=* _temp2279; _LL2287: _temp2286= _temp2281.f1;
goto _LL2285; _LL2285: _temp2284= _temp2281.f2; goto _LL2283; _LL2283: _temp2282=
_temp2281.f3; goto _LL2280; _LL2280: { void* _temp2288= Cyc_Parse_speclist2typ(
_temp2284, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2282 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2289=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2289[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2290; _temp2290.tag= Cyc_ParamDecl_tok; _temp2290.f1=({
struct _tuple2* _temp2291=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2291->f1= 0; _temp2291->f2= _temp2286; _temp2291->f3= _temp2288; _temp2291;});
_temp2290;}); _temp2289;}); break;}} case 179: _LL2278: { struct _tuple17
_temp2295; struct Cyc_List_List* _temp2296; struct Cyc_List_List* _temp2298;
struct Cyc_Absyn_Tqual _temp2300; struct _tuple17* _temp2293= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2295=*
_temp2293; _LL2301: _temp2300= _temp2295.f1; goto _LL2299; _LL2299: _temp2298=
_temp2295.f2; goto _LL2297; _LL2297: _temp2296= _temp2295.f3; goto _LL2294;
_LL2294: { void* _temp2302= Cyc_Parse_speclist2typ( _temp2298, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2303=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2306; struct Cyc_List_List* _temp2308; void* _temp2310; struct Cyc_Absyn_Tqual
_temp2312; struct _tuple6 _temp2304= Cyc_Parse_apply_tms( _temp2300, _temp2302,
_temp2296, _temp2303); _LL2313: _temp2312= _temp2304.f1; goto _LL2311; _LL2311:
_temp2310= _temp2304.f2; goto _LL2309; _LL2309: _temp2308= _temp2304.f3; goto
_LL2307; _LL2307: _temp2306= _temp2304.f4; goto _LL2305; _LL2305: if( _temp2308
!=  0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2306 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2314=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2314[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2315; _temp2315.tag= Cyc_ParamDecl_tok; _temp2315.f1=({
struct _tuple2* _temp2316=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2316->f1= 0; _temp2316->f2= _temp2312; _temp2316->f3= _temp2310; _temp2316;});
_temp2315;}); _temp2314;}); break;}} case 180: _LL2292: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2318=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2318[ 0]=({ struct Cyc_IdList_tok_struct
_temp2319; _temp2319.tag= Cyc_IdList_tok; _temp2319.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2319;});
_temp2318;}); break; case 181: _LL2317: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2321=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2321[ 0]=({ struct Cyc_IdList_tok_struct _temp2322; _temp2322.tag= Cyc_IdList_tok;
_temp2322.f1=({ struct Cyc_List_List* _temp2323=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2323->hd=( void*)({ struct
_tagged_arr* _temp2324=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2324[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2324;}); _temp2323->tl=
0; _temp2323;}); _temp2322;}); _temp2321;}); break; case 182: _LL2320: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2326=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2326[ 0]=({ struct Cyc_IdList_tok_struct
_temp2327; _temp2327.tag= Cyc_IdList_tok; _temp2327.f1=({ struct Cyc_List_List*
_temp2328=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2328->hd=( void*)({ struct _tagged_arr* _temp2329=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2329[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2329;});
_temp2328->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2328;}); _temp2327;}); _temp2326;}); break; case 183:
_LL2325: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 184: _LL2330: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 185: _LL2331: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2333=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2333[ 0]=({ struct Cyc_Exp_tok_struct _temp2334; _temp2334.tag= Cyc_Exp_tok;
_temp2334.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2335=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2335[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2336; _temp2336.tag= Cyc_Absyn_UnresolvedMem_e; _temp2336.f1= 0; _temp2336.f2=
0; _temp2336;}); _temp2335;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2334;});
_temp2333;}); break; case 186: _LL2332: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2338=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2338[ 0]=({ struct Cyc_Exp_tok_struct _temp2339; _temp2339.tag= Cyc_Exp_tok;
_temp2339.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2340=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2340[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2341; _temp2341.tag= Cyc_Absyn_UnresolvedMem_e; _temp2341.f1= 0; _temp2341.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2341;});
_temp2340;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2339;}); _temp2338;}); break; case 187:
_LL2337: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2343=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2343[ 0]=({ struct Cyc_Exp_tok_struct
_temp2344; _temp2344.tag= Cyc_Exp_tok; _temp2344.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2345=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2345[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2346; _temp2346.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2346.f1= 0; _temp2346.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2346;});
_temp2345;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2344;}); _temp2343;}); break; case 188:
_LL2342: { struct Cyc_Absyn_Vardecl* _temp2348= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2354=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2354->f1=( void*) Cyc_Absyn_Loc_n; _temp2354->f2=({ struct _tagged_arr*
_temp2355=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp2355[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); _temp2355;}); _temp2354;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp2348->tq=({ struct Cyc_Absyn_Tqual _temp2349; _temp2349.q_const= 1;
_temp2349.q_volatile= 0; _temp2349.q_restrict= 1; _temp2349;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2350=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2350[ 0]=({ struct Cyc_Exp_tok_struct
_temp2351; _temp2351.tag= Cyc_Exp_tok; _temp2351.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2352=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2352[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2353; _temp2353.tag= Cyc_Absyn_Comprehension_e;
_temp2353.f1= _temp2348; _temp2353.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2353.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2353;});
_temp2352;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2351;}); _temp2350;}); break;} case
189: _LL2347: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2357=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2357[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2358; _temp2358.tag=
Cyc_InitializerList_tok; _temp2358.f1=({ struct Cyc_List_List* _temp2359=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2359->hd=(
void*)({ struct _tuple19* _temp2360=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2360->f1= 0; _temp2360->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2360;}); _temp2359->tl=
0; _temp2359;}); _temp2358;}); _temp2357;}); break; case 190: _LL2356: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2362=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2362[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2363; _temp2363.tag= Cyc_InitializerList_tok;
_temp2363.f1=({ struct Cyc_List_List* _temp2364=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2364->hd=( void*)({ struct
_tuple19* _temp2365=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2365->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2365->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2365;});
_temp2364->tl= 0; _temp2364;}); _temp2363;}); _temp2362;}); break; case 191:
_LL2361: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2367=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2367[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2368; _temp2368.tag=
Cyc_InitializerList_tok; _temp2368.f1=({ struct Cyc_List_List* _temp2369=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2369->hd=(
void*)({ struct _tuple19* _temp2370=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2370->f1= 0; _temp2370->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2370;}); _temp2369->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2369;}); _temp2368;}); _temp2367;}); break; case 192:
_LL2366: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2372=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2372[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2373; _temp2373.tag=
Cyc_InitializerList_tok; _temp2373.f1=({ struct Cyc_List_List* _temp2374=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2374->hd=(
void*)({ struct _tuple19* _temp2375=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2375->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2375->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2375;});
_temp2374->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2374;});
_temp2373;}); _temp2372;}); break; case 193: _LL2371: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2377=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2377[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2378; _temp2378.tag= Cyc_DesignatorList_tok;
_temp2378.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2378;});
_temp2377;}); break; case 194: _LL2376: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2380=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2380[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2381; _temp2381.tag=
Cyc_DesignatorList_tok; _temp2381.f1=({ struct Cyc_List_List* _temp2382=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2382->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2382->tl= 0; _temp2382;}); _temp2381;}); _temp2380;});
break; case 195: _LL2379: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2384=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2384[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2385; _temp2385.tag=
Cyc_DesignatorList_tok; _temp2385.f1=({ struct Cyc_List_List* _temp2386=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2386->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2386->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2386;});
_temp2385;}); _temp2384;}); break; case 196: _LL2383: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2388=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2388[ 0]=({ struct
Cyc_Designator_tok_struct _temp2389; _temp2389.tag= Cyc_Designator_tok;
_temp2389.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2390=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2390[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2391; _temp2391.tag=
Cyc_Absyn_ArrayElement; _temp2391.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2391;});
_temp2390;})); _temp2389;}); _temp2388;}); break; case 197: _LL2387: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2393=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2393[ 0]=({ struct
Cyc_Designator_tok_struct _temp2394; _temp2394.tag= Cyc_Designator_tok;
_temp2394.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2395=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2395[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2396; _temp2396.tag= Cyc_Absyn_FieldName;
_temp2396.f1=({ struct _tagged_arr* _temp2397=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2397[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2397;}); _temp2396;});
_temp2395;})); _temp2394;}); _temp2393;}); break; case 198: _LL2392: { void*
_temp2399= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2400=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2400 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2401=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2402=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2402[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2403; _temp2403.tag= Cyc_ParamDecl_tok; _temp2403.f1=({
struct _tuple2* _temp2404=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2404->f1= 0; _temp2404->f2= _temp2401; _temp2404->f3= _temp2399; _temp2404;});
_temp2403;}); _temp2402;}); break;}} case 199: _LL2398: { void* _temp2406= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp2407=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2408=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2409=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2410= Cyc_Parse_apply_tms( _temp2408, _temp2406, _temp2407,
_temp2409); if( _temp2410.f3 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2410.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2411=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2411[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2412; _temp2412.tag= Cyc_ParamDecl_tok; _temp2412.f1=({
struct _tuple2* _temp2413=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2413->f1= 0; _temp2413->f2= _temp2410.f1; _temp2413->f3= _temp2410.f2;
_temp2413;}); _temp2412;}); _temp2411;}); break;} case 200: _LL2405: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2415=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2415[ 0]=({ struct Cyc_Type_tok_struct
_temp2416; _temp2416.tag= Cyc_Type_tok; _temp2416.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2416;});
_temp2415;}); break; case 201: _LL2414: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2418=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2418[ 0]=({ struct Cyc_Type_tok_struct _temp2419; _temp2419.tag= Cyc_Type_tok;
_temp2419.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2420=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2420[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2421; _temp2421.tag= Cyc_Absyn_JoinEff;
_temp2421.f1= 0; _temp2421;}); _temp2420;})); _temp2419;}); _temp2418;}); break;
case 202: _LL2417: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2423=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2423[ 0]=({ struct Cyc_Type_tok_struct _temp2424; _temp2424.tag= Cyc_Type_tok;
_temp2424.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2425=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2425[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2426; _temp2426.tag= Cyc_Absyn_JoinEff;
_temp2426.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2426;}); _temp2425;})); _temp2424;});
_temp2423;}); break; case 203: _LL2422: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2428=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2428[ 0]=({ struct Cyc_Type_tok_struct _temp2429; _temp2429.tag= Cyc_Type_tok;
_temp2429.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2430=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2430[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2431; _temp2431.tag= Cyc_Absyn_RgnsEff;
_temp2431.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2431;}); _temp2430;})); _temp2429;});
_temp2428;}); break; case 204: _LL2427: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2433=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2433[ 0]=({ struct Cyc_Type_tok_struct _temp2434; _temp2434.tag= Cyc_Type_tok;
_temp2434.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2435=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2435[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2436; _temp2436.tag= Cyc_Absyn_JoinEff;
_temp2436.f1=({ struct Cyc_List_List* _temp2437=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2437->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2437->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2437;}); _temp2436;}); _temp2435;})); _temp2434;});
_temp2433;}); break; case 205: _LL2432: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2439=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2439[ 0]=({ struct Cyc_TypeList_tok_struct _temp2440; _temp2440.tag= Cyc_TypeList_tok;
_temp2440.f1=({ struct Cyc_List_List* _temp2441=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2441->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2441->tl= 0;
_temp2441;}); _temp2440;}); _temp2439;}); break; case 206: _LL2438: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2443=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2443[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2444; _temp2444.tag= Cyc_TypeList_tok; _temp2444.f1=({ struct Cyc_List_List*
_temp2445=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2445->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2445->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2445;});
_temp2444;}); _temp2443;}); break; case 207: _LL2442: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2447=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2447[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2448; _temp2448.tag= Cyc_AbstractDeclarator_tok;
_temp2448.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2449=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2449->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2449;});
_temp2448;}); _temp2447;}); break; case 208: _LL2446: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 209: _LL2450:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2452=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2452[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2453; _temp2453.tag=
Cyc_AbstractDeclarator_tok; _temp2453.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2454=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2454->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2454;});
_temp2453;}); _temp2452;}); break; case 210: _LL2451: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 211:
_LL2455: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2457=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2457[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2458; _temp2458.tag=
Cyc_AbstractDeclarator_tok; _temp2458.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2459=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2459->tms=({ struct Cyc_List_List* _temp2460=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2460->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2460->tl= 0; _temp2460;}); _temp2459;}); _temp2458;}); _temp2457;}); break;
case 212: _LL2456: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2462=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2462[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2463; _temp2463.tag= Cyc_AbstractDeclarator_tok; _temp2463.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2464=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2464->tms=({ struct Cyc_List_List* _temp2465=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2465->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2465->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2465;});
_temp2464;}); _temp2463;}); _temp2462;}); break; case 213: _LL2461: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2467=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2467[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2468; _temp2468.tag= Cyc_AbstractDeclarator_tok;
_temp2468.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2469=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2469->tms=({
struct Cyc_List_List* _temp2470=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2470->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2471=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2471[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2472; _temp2472.tag= Cyc_Absyn_ConstArray_mod; _temp2472.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2472;});
_temp2471;})); _temp2470->tl= 0; _temp2470;}); _temp2469;}); _temp2468;});
_temp2467;}); break; case 214: _LL2466: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2474=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2474[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2475; _temp2475.tag= Cyc_AbstractDeclarator_tok; _temp2475.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2476=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2476->tms=({ struct Cyc_List_List* _temp2477=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2477->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2478=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2478[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2479; _temp2479.tag= Cyc_Absyn_ConstArray_mod;
_temp2479.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2479;}); _temp2478;})); _temp2477->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2477;}); _temp2476;}); _temp2475;}); _temp2474;}); break; case 215: _LL2473:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2481=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2481[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2482; _temp2482.tag=
Cyc_AbstractDeclarator_tok; _temp2482.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2483=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2483->tms=({ struct Cyc_List_List* _temp2484=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2484->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2485=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2485[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2486; _temp2486.tag= Cyc_Absyn_Function_mod;
_temp2486.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2487=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2487[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2488; _temp2488.tag= Cyc_Absyn_WithTypes;
_temp2488.f1= 0; _temp2488.f2= 0; _temp2488.f3= 0; _temp2488.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2488.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2488;}); _temp2487;})); _temp2486;}); _temp2485;}));
_temp2484->tl= 0; _temp2484;}); _temp2483;}); _temp2482;}); _temp2481;}); break;
case 216: _LL2480: { struct _tuple16 _temp2492; struct Cyc_List_List* _temp2493;
struct Cyc_Core_Opt* _temp2495; struct Cyc_Absyn_VarargInfo* _temp2497; int
_temp2499; struct Cyc_List_List* _temp2501; struct _tuple16* _temp2490= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2492=*
_temp2490; _LL2502: _temp2501= _temp2492.f1; goto _LL2500; _LL2500: _temp2499=
_temp2492.f2; goto _LL2498; _LL2498: _temp2497= _temp2492.f3; goto _LL2496;
_LL2496: _temp2495= _temp2492.f4; goto _LL2494; _LL2494: _temp2493= _temp2492.f5;
goto _LL2491; _LL2491: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2503=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2503[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2504; _temp2504.tag= Cyc_AbstractDeclarator_tok; _temp2504.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2505=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2505->tms=({ struct Cyc_List_List* _temp2506=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2506->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2507=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2507[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2508; _temp2508.tag= Cyc_Absyn_Function_mod;
_temp2508.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2509=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2509[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2510; _temp2510.tag= Cyc_Absyn_WithTypes;
_temp2510.f1= _temp2501; _temp2510.f2= _temp2499; _temp2510.f3= _temp2497;
_temp2510.f4= _temp2495; _temp2510.f5= _temp2493; _temp2510;}); _temp2509;}));
_temp2508;}); _temp2507;})); _temp2506->tl= 0; _temp2506;}); _temp2505;});
_temp2504;}); _temp2503;}); break;} case 217: _LL2489: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2512=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2512[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2513; _temp2513.tag= Cyc_AbstractDeclarator_tok;
_temp2513.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2514=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2514->tms=({
struct Cyc_List_List* _temp2515=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2515->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2516=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2516[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2517; _temp2517.tag=
Cyc_Absyn_Function_mod; _temp2517.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2518=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2518[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2519; _temp2519.tag= Cyc_Absyn_WithTypes;
_temp2519.f1= 0; _temp2519.f2= 0; _temp2519.f3= 0; _temp2519.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2519.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2519;}); _temp2518;})); _temp2517;}); _temp2516;}));
_temp2515->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2515;});
_temp2514;}); _temp2513;}); _temp2512;}); break; case 218: _LL2511: { struct
_tuple16 _temp2523; struct Cyc_List_List* _temp2524; struct Cyc_Core_Opt*
_temp2526; struct Cyc_Absyn_VarargInfo* _temp2528; int _temp2530; struct Cyc_List_List*
_temp2532; struct _tuple16* _temp2521= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2523=*
_temp2521; _LL2533: _temp2532= _temp2523.f1; goto _LL2531; _LL2531: _temp2530=
_temp2523.f2; goto _LL2529; _LL2529: _temp2528= _temp2523.f3; goto _LL2527;
_LL2527: _temp2526= _temp2523.f4; goto _LL2525; _LL2525: _temp2524= _temp2523.f5;
goto _LL2522; _LL2522: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2534=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2534[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2535; _temp2535.tag= Cyc_AbstractDeclarator_tok; _temp2535.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2536=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2536->tms=({ struct Cyc_List_List* _temp2537=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2537->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2538=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2538[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2539; _temp2539.tag= Cyc_Absyn_Function_mod;
_temp2539.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2540=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2540[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2541; _temp2541.tag= Cyc_Absyn_WithTypes;
_temp2541.f1= _temp2532; _temp2541.f2= _temp2530; _temp2541.f3= _temp2528;
_temp2541.f4= _temp2526; _temp2541.f5= _temp2524; _temp2541;}); _temp2540;}));
_temp2539;}); _temp2538;})); _temp2537->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2537;}); _temp2536;}); _temp2535;}); _temp2534;}); break;} case 219:
_LL2520: { struct Cyc_List_List* _temp2543=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2544=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2544[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2545; _temp2545.tag= Cyc_AbstractDeclarator_tok; _temp2545.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2546=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2546->tms=({ struct Cyc_List_List* _temp2547=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2547->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2548=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2548[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2549; _temp2549.tag= Cyc_Absyn_TypeParams_mod;
_temp2549.f1= _temp2543; _temp2549.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2549.f3=
0; _temp2549;}); _temp2548;})); _temp2547->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2547;}); _temp2546;}); _temp2545;}); _temp2544;}); break;} case 220:
_LL2542:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2551=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2552=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2552[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2553; _temp2553.tag= Cyc_AbstractDeclarator_tok; _temp2553.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2554=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2554->tms=({ struct Cyc_List_List* _temp2555=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2555->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2556=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2556[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2557; _temp2557.tag= Cyc_Absyn_TypeParams_mod;
_temp2557.f1= _temp2551; _temp2557.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2557.f3=
0; _temp2557;}); _temp2556;})); _temp2555->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2555;}); _temp2554;}); _temp2553;}); _temp2552;}); break;} case 221:
_LL2550: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2559=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2559[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2560; _temp2560.tag=
Cyc_AbstractDeclarator_tok; _temp2560.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2561=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2561->tms=({ struct Cyc_List_List* _temp2562=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2562->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2563=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2563[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2564; _temp2564.tag= Cyc_Absyn_Attributes_mod;
_temp2564.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2564.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2564;}); _temp2563;}));
_temp2562->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms; _temp2562;});
_temp2561;}); _temp2560;}); _temp2559;}); break; case 222: _LL2558: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 223: _LL2565:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
224: _LL2566: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 225: _LL2567: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 226: _LL2568: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 227: _LL2569:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
228: _LL2570: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2582=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2582->name=({ struct
_tagged_arr* _temp2583=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2583[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2583;});
_temp2582->identity= 0; _temp2582->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2582;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2580=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2580[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2581; _temp2581.tag= Cyc_Absyn_VarType;
_temp2581.f1= tv; _temp2581;}); _temp2580;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2572=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2572[ 0]=({ struct Cyc_Stmt_tok_struct _temp2573; _temp2573.tag= Cyc_Stmt_tok;
_temp2573.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2574=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2574[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2575; _temp2575.tag= Cyc_Absyn_Region_s;
_temp2575.f1= tv; _temp2575.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2576=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2576->f1=(
void*) Cyc_Absyn_Loc_n; _temp2576->f2=({ struct _tagged_arr* _temp2577=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2577[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2577;});
_temp2576;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2578=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2578[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2579; _temp2579.tag=
Cyc_Absyn_RgnHandleType; _temp2579.f1=( void*) t; _temp2579;}); _temp2578;}), 0);
_temp2575.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2575;}); _temp2574;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2573;});
_temp2572;}); break;} case 229: _LL2571: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2595=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2595->name=({ struct
_tagged_arr* _temp2596=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2596[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2598; _temp2598.tag= Cyc_Std_String_pa; _temp2598.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2597[ 1u]={& _temp2598}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2597, sizeof( void*), 1u));}});
_temp2596;}); _temp2595->identity= 0; _temp2595->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2595;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2593=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2593[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2594; _temp2594.tag= Cyc_Absyn_VarType;
_temp2594.f1= tv; _temp2594;}); _temp2593;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2585=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2585[ 0]=({ struct Cyc_Stmt_tok_struct _temp2586; _temp2586.tag= Cyc_Stmt_tok;
_temp2586.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2587=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2587[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2588; _temp2588.tag= Cyc_Absyn_Region_s;
_temp2588.f1= tv; _temp2588.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2589=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2589->f1=(
void*) Cyc_Absyn_Loc_n; _temp2589->f2=({ struct _tagged_arr* _temp2590=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2590[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2590;});
_temp2589;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2591=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2591[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2592; _temp2592.tag=
Cyc_Absyn_RgnHandleType; _temp2592.f1=( void*) t; _temp2592;}); _temp2591;}), 0);
_temp2588.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2588;}); _temp2587;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2586;});
_temp2585;}); break;} case 230: _LL2584: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2600=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2600[ 0]=({ struct Cyc_Stmt_tok_struct _temp2601; _temp2601.tag= Cyc_Stmt_tok;
_temp2601.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2602=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2602[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2603; _temp2603.tag= Cyc_Absyn_Cut_s;
_temp2603.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2603;}); _temp2602;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2601;});
_temp2600;}); break; case 231: _LL2599: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2605=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2605[ 0]=({ struct Cyc_Stmt_tok_struct _temp2606; _temp2606.tag= Cyc_Stmt_tok;
_temp2606.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2607=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2607[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2608; _temp2608.tag= Cyc_Absyn_Splice_s;
_temp2608.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2608;}); _temp2607;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2606;});
_temp2605;}); break; case 232: _LL2604: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2610=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2610[ 0]=({ struct Cyc_Stmt_tok_struct _temp2611; _temp2611.tag= Cyc_Stmt_tok;
_temp2611.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2612=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2612[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2613; _temp2613.tag= Cyc_Absyn_Label_s;
_temp2613.f1=({ struct _tagged_arr* _temp2614=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2614[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2614;});
_temp2613.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2613;}); _temp2612;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2611;});
_temp2610;}); break; case 233: _LL2609: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2616=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2616[ 0]=({ struct Cyc_Stmt_tok_struct _temp2617; _temp2617.tag= Cyc_Stmt_tok;
_temp2617.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2617;});
_temp2616;}); break; case 234: _LL2615: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2619=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2619[ 0]=({ struct Cyc_Stmt_tok_struct _temp2620; _temp2620.tag= Cyc_Stmt_tok;
_temp2620.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2620;}); _temp2619;}); break; case 235: _LL2618: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2622=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2622[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2623; _temp2623.tag= Cyc_Stmt_tok; _temp2623.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2623;}); _temp2622;}); break; case 236: _LL2621: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 237:
_LL2624: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2626=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2626[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2627; _temp2627.tag= Cyc_Stmt_tok; _temp2627.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2627;}); _temp2626;}); break;
case 238: _LL2625: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2629=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2629[ 0]=({ struct Cyc_Stmt_tok_struct _temp2630; _temp2630.tag= Cyc_Stmt_tok;
_temp2630.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2630;});
_temp2629;}); break; case 239: _LL2628: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 240: _LL2631:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2633=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2633[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2634; _temp2634.tag= Cyc_Stmt_tok; _temp2634.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2634;}); _temp2633;}); break; case 241: _LL2632: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2636=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2636[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2637; _temp2637.tag= Cyc_Stmt_tok; _temp2637.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2638=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2638[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2639; _temp2639.tag= Cyc_Absyn_Fn_d;
_temp2639.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2639;}); _temp2638;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2637;}); _temp2636;}); break; case 242: _LL2635: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2641=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2641[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2642; _temp2642.tag= Cyc_Stmt_tok; _temp2642.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2643=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2643[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2644; _temp2644.tag= Cyc_Absyn_Fn_d;
_temp2644.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2644;}); _temp2643;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2642;}); _temp2641;}); break; case 243: _LL2640: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2646=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2646[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2647; _temp2647.tag= Cyc_Stmt_tok; _temp2647.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2647;});
_temp2646;}); break; case 244: _LL2645: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2649=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2649[ 0]=({ struct Cyc_Stmt_tok_struct _temp2650; _temp2650.tag= Cyc_Stmt_tok;
_temp2650.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2650;}); _temp2649;}); break; case 245: _LL2648: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2652=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2652[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2653; _temp2653.tag= Cyc_Stmt_tok; _temp2653.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2653;}); _temp2652;}); break; case 246: _LL2651: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2655=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2655[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2656; _temp2656.tag= Cyc_Stmt_tok; _temp2656.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2657=( struct Cyc_Absyn_SwitchC_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2657[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2658; _temp2658.tag= Cyc_Absyn_SwitchC_s;
_temp2658.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2658.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2658;});
_temp2657;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2656;}); _temp2655;}); break; case 247:
_LL2654: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2660=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2660[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2661; _temp2661.tag= Cyc_Stmt_tok; _temp2661.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2661;});
_temp2660;}); break; case 248: _LL2659: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2663=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2663[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2664; _temp2664.tag= Cyc_SwitchClauseList_tok; _temp2664.f1= 0; _temp2664;});
_temp2663;}); break; case 249: _LL2662: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2666=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2666[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2667; _temp2667.tag= Cyc_SwitchClauseList_tok; _temp2667.f1=({ struct Cyc_List_List*
_temp2668=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2668->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2669=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2669->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2669->pat_vars= 0; _temp2669->where_clause= 0; _temp2669->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2669->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2669;}); _temp2668->tl= 0; _temp2668;});
_temp2667;}); _temp2666;}); break; case 250: _LL2665: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2671=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2671[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2672; _temp2672.tag= Cyc_SwitchClauseList_tok;
_temp2672.f1=({ struct Cyc_List_List* _temp2673=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2673->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2674=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2674->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2674->pat_vars= 0; _temp2674->where_clause= 0;
_temp2674->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2674->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2674;});
_temp2673->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2673;}); _temp2672;});
_temp2671;}); break; case 251: _LL2670: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2676=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2676[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2677; _temp2677.tag= Cyc_SwitchClauseList_tok; _temp2677.f1=({ struct Cyc_List_List*
_temp2678=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2678->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2679=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2679->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2679->pat_vars=
0; _temp2679->where_clause= 0; _temp2679->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2679->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2679;}); _temp2678->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2678;});
_temp2677;}); _temp2676;}); break; case 252: _LL2675: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2681=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2681[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2682; _temp2682.tag= Cyc_SwitchClauseList_tok;
_temp2682.f1=({ struct Cyc_List_List* _temp2683=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2683->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2684=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2684->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp2684->pat_vars= 0; _temp2684->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2684->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2684->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2684;});
_temp2683->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2683;}); _temp2682;});
_temp2681;}); break; case 253: _LL2680: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2686=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2686[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2687; _temp2687.tag= Cyc_SwitchClauseList_tok; _temp2687.f1=({ struct Cyc_List_List*
_temp2688=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2688->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2689=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2689->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2689->pat_vars=
0; _temp2689->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2689->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2689->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2689;});
_temp2688->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2688;}); _temp2687;});
_temp2686;}); break; case 254: _LL2685: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2691=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2691[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2692; _temp2692.tag= Cyc_SwitchCClauseList_tok; _temp2692.f1= 0; _temp2692;});
_temp2691;}); break; case 255: _LL2690: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2694=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2694[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2695; _temp2695.tag= Cyc_SwitchCClauseList_tok; _temp2695.f1=({ struct Cyc_List_List*
_temp2696=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2696->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2697=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2697->cnst_exp= 0;
_temp2697->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2697->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2697;});
_temp2696->tl= 0; _temp2696;}); _temp2695;}); _temp2694;}); break; case 256:
_LL2693: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2699=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2699[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2700; _temp2700.tag=
Cyc_SwitchCClauseList_tok; _temp2700.f1=({ struct Cyc_List_List* _temp2701=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2701->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2702=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2702->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2702->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2702->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2702;});
_temp2701->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2701;}); _temp2700;});
_temp2699;}); break; case 257: _LL2698: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2704=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2704[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2705; _temp2705.tag= Cyc_SwitchCClauseList_tok; _temp2705.f1=({ struct Cyc_List_List*
_temp2706=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2706->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2707=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2707->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2707->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2707->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2707;});
_temp2706->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2706;}); _temp2705;});
_temp2704;}); break; case 258: _LL2703: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2709=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2709[ 0]=({ struct Cyc_Stmt_tok_struct _temp2710; _temp2710.tag= Cyc_Stmt_tok;
_temp2710.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2710;}); _temp2709;}); break; case 259: _LL2708: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2712=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2712[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2713; _temp2713.tag= Cyc_Stmt_tok; _temp2713.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2713;}); _temp2712;}); break; case 260: _LL2711: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2715=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2715[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2716; _temp2716.tag= Cyc_Stmt_tok; _temp2716.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2716;}); _temp2715;}); break; case 261: _LL2714: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2718=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2718[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2719; _temp2719.tag= Cyc_Stmt_tok; _temp2719.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2719;}); _temp2718;}); break; case 262: _LL2717: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2721=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2721[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2722; _temp2722.tag= Cyc_Stmt_tok; _temp2722.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2722;}); _temp2721;}); break; case 263: _LL2720: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2724=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2724[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2725; _temp2725.tag= Cyc_Stmt_tok; _temp2725.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2725;}); _temp2724;}); break; case 264: _LL2723: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2727=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2727[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2728; _temp2728.tag= Cyc_Stmt_tok; _temp2728.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2728;}); _temp2727;}); break; case 265: _LL2726: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2730=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2730[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2731; _temp2731.tag= Cyc_Stmt_tok; _temp2731.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2731;});
_temp2730;}); break; case 266: _LL2729: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2733=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2733[ 0]=({ struct Cyc_Stmt_tok_struct _temp2734; _temp2734.tag= Cyc_Stmt_tok;
_temp2734.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2734;});
_temp2733;}); break; case 267: _LL2732: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2736=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2736[ 0]=({ struct Cyc_Stmt_tok_struct _temp2737; _temp2737.tag= Cyc_Stmt_tok;
_temp2737.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2737;}); _temp2736;}); break; case 268: _LL2735: { struct Cyc_List_List*
_temp2739= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt* _temp2740= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2741=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2741[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2742; _temp2742.tag= Cyc_Stmt_tok; _temp2742.f1= Cyc_Parse_flatten_declarations(
_temp2739, _temp2740); _temp2742;}); _temp2741;}); break;} case 269: _LL2738: {
struct Cyc_List_List* _temp2744= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2745= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2746=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2746[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2747; _temp2747.tag= Cyc_Stmt_tok; _temp2747.f1= Cyc_Parse_flatten_declarations(
_temp2744, _temp2745); _temp2747;}); _temp2746;}); break;} case 270: _LL2743: {
struct Cyc_List_List* _temp2749= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2750= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2751=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2751[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2752; _temp2752.tag= Cyc_Stmt_tok; _temp2752.f1= Cyc_Parse_flatten_declarations(
_temp2749, _temp2750); _temp2752;}); _temp2751;}); break;} case 271: _LL2748: {
struct Cyc_List_List* _temp2754= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2755= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2756=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2756[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2757; _temp2757.tag= Cyc_Stmt_tok; _temp2757.f1= Cyc_Parse_flatten_declarations(
_temp2754, _temp2755); _temp2757;}); _temp2756;}); break;} case 272: _LL2753:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2759=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2759[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2760; _temp2760.tag= Cyc_Stmt_tok; _temp2760.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2761=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2761[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2761;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2760;}); _temp2759;}); break; case 273: _LL2758: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2763=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2763[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2764; _temp2764.tag= Cyc_Stmt_tok; _temp2764.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2764;}); _temp2763;}); break; case 274:
_LL2762: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2766=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2766[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2767; _temp2767.tag= Cyc_Stmt_tok; _temp2767.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2767;}); _temp2766;}); break; case 275: _LL2765: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2769=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2769[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2770; _temp2770.tag= Cyc_Stmt_tok; _temp2770.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2770;}); _temp2769;}); break; case 276:
_LL2768: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2772=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2772[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2773; _temp2773.tag= Cyc_Stmt_tok; _temp2773.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2773;}); _temp2772;}); break; case 277: _LL2771: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2775=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2775[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2776; _temp2776.tag= Cyc_Stmt_tok; _temp2776.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2776;}); _temp2775;}); break; case 278:
_LL2774: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2778=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2778[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2779; _temp2779.tag= Cyc_Stmt_tok; _temp2779.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2779;}); _temp2778;}); break; case 279:
_LL2777: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2781=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2781[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2782; _temp2782.tag= Cyc_Stmt_tok; _temp2782.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2782;}); _temp2781;}); break; case 280: _LL2780: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2784=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2784[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2785; _temp2785.tag= Cyc_Pattern_tok; _temp2785.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2785;});
_temp2784;}); break; case 281: _LL2783: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 282:
_LL2786: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2788=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2788[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2789; _temp2789.tag= Cyc_Pattern_tok; _temp2789.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2790=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2790[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2791; _temp2791.tag= Cyc_Absyn_Int_p; _temp2791.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2791.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2791;}); _temp2790;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2789;});
_temp2788;}); break; case 283: _LL2787: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2793=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2793[ 0]=({ struct Cyc_Pattern_tok_struct _temp2794; _temp2794.tag= Cyc_Pattern_tok;
_temp2794.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2795=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2795[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2796; _temp2796.tag= Cyc_Absyn_Int_p;
_temp2796.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2796.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2796;});
_temp2795;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2794;}); _temp2793;}); break; case 284:
_LL2792: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2798=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2798[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2799; _temp2799.tag= Cyc_Pattern_tok; _temp2799.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2800=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2800[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2801; _temp2801.tag= Cyc_Absyn_Float_p; _temp2801.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2801;}); _temp2800;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2799;});
_temp2798;}); break; case 285: _LL2797: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2803=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2803[ 0]=({ struct Cyc_Pattern_tok_struct _temp2804; _temp2804.tag= Cyc_Pattern_tok;
_temp2804.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2805=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2805[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2806; _temp2806.tag= Cyc_Absyn_Char_p; _temp2806.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2806;});
_temp2805;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2804;}); _temp2803;}); break; case 286:
_LL2802: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2808=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2808[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2809; _temp2809.tag= Cyc_Pattern_tok; _temp2809.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2809;});
_temp2808;}); break; case 287: _LL2807: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2811=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2811[ 0]=({ struct Cyc_Pattern_tok_struct _temp2812; _temp2812.tag= Cyc_Pattern_tok;
_temp2812.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2813=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2813[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2814; _temp2814.tag=
Cyc_Absyn_UnknownId_p; _temp2814.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2814;}); _temp2813;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2812;}); _temp2811;}); break; case 288:
_LL2810: { struct Cyc_List_List* _temp2816=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2817=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2817[ 0]=({ struct Cyc_Pattern_tok_struct _temp2818; _temp2818.tag= Cyc_Pattern_tok;
_temp2818.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2819=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2819[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2820; _temp2820.tag= Cyc_Absyn_UnknownCall_p; _temp2820.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2820.f2=
_temp2816; _temp2820.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2820;});
_temp2819;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2818;}); _temp2817;}); break;} case
289: _LL2815: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2822=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2822[ 0]=({ struct Cyc_Pattern_tok_struct _temp2823; _temp2823.tag= Cyc_Pattern_tok;
_temp2823.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2824=( struct Cyc_Absyn_Tuple_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2824[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2825; _temp2825.tag= Cyc_Absyn_Tuple_p;
_temp2825.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2825;}); _temp2824;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2823;}); _temp2822;}); break; case 290: _LL2821: { struct Cyc_List_List*
_temp2827=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); yyval=(
void*)({ struct Cyc_Pattern_tok_struct* _temp2828=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2828[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2829; _temp2829.tag= Cyc_Pattern_tok; _temp2829.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2830=( struct Cyc_Absyn_UnknownFields_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2830[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2831; _temp2831.tag= Cyc_Absyn_UnknownFields_p;
_temp2831.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2831.f2= _temp2827; _temp2831.f3= 0; _temp2831;});
_temp2830;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2829;}); _temp2828;}); break;} case
291: _LL2826: { struct Cyc_List_List* _temp2833=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2834=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2834[ 0]=({ struct Cyc_Pattern_tok_struct _temp2835; _temp2835.tag= Cyc_Pattern_tok;
_temp2835.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2836=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2836[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2837; _temp2837.tag= Cyc_Absyn_UnknownFields_p; _temp2837.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2837.f2=
_temp2833; _temp2837.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2837;});
_temp2836;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2835;}); _temp2834;}); break;} case
292: _LL2832: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2839=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2839[ 0]=({ struct Cyc_Pattern_tok_struct _temp2840; _temp2840.tag= Cyc_Pattern_tok;
_temp2840.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2841=( struct Cyc_Absyn_Pointer_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2841[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2842; _temp2842.tag= Cyc_Absyn_Pointer_p;
_temp2842.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2842;}); _temp2841;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2840;});
_temp2839;}); break; case 293: _LL2838: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2844=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2844[ 0]=({ struct Cyc_Pattern_tok_struct _temp2845; _temp2845.tag= Cyc_Pattern_tok;
_temp2845.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2846=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2846[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2847; _temp2847.tag=
Cyc_Absyn_Reference_p; _temp2847.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2848=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2848->f1=(
void*) Cyc_Absyn_Loc_n; _temp2848->f2=({ struct _tagged_arr* _temp2849=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2849[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2849;});
_temp2848;}),( void*) Cyc_Absyn_VoidType, 0); _temp2847;}); _temp2846;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2845;}); _temp2844;}); break; case 294: _LL2843: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2851=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2851[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2852; _temp2852.tag= Cyc_PatternList_tok;
_temp2852.f1= 0; _temp2852;}); _temp2851;}); break; case 295: _LL2850: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2854=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2854[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2855; _temp2855.tag= Cyc_PatternList_tok;
_temp2855.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2855;}); _temp2854;}); break; case 296: _LL2853: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2857=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2857[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2858; _temp2858.tag= Cyc_PatternList_tok;
_temp2858.f1=({ struct Cyc_List_List* _temp2859=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2859->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2859->tl= 0;
_temp2859;}); _temp2858;}); _temp2857;}); break; case 297: _LL2856: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2861=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2861[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2862; _temp2862.tag= Cyc_PatternList_tok;
_temp2862.f1=({ struct Cyc_List_List* _temp2863=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2863->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2863->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2863;}); _temp2862;}); _temp2861;}); break; case 298:
_LL2860: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2865=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2865[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2866; _temp2866.tag=
Cyc_FieldPattern_tok; _temp2866.f1=({ struct _tuple13* _temp2867=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2867->f1= 0; _temp2867->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2867;}); _temp2866;}); _temp2865;}); break; case 299:
_LL2864: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2869=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2869[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2870; _temp2870.tag=
Cyc_FieldPattern_tok; _temp2870.f1=({ struct _tuple13* _temp2871=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2871->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2871->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2871;}); _temp2870;}); _temp2869;}); break; case 300:
_LL2868: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2873=(
struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2873[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2874; _temp2874.tag=
Cyc_FieldPatternList_tok; _temp2874.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2874;});
_temp2873;}); break; case 301: _LL2872: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2876=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2876[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2877; _temp2877.tag= Cyc_FieldPatternList_tok; _temp2877.f1=({ struct Cyc_List_List*
_temp2878=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2878->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2878->tl= 0;
_temp2878;}); _temp2877;}); _temp2876;}); break; case 302: _LL2875: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2880=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2880[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2881; _temp2881.tag= Cyc_FieldPatternList_tok;
_temp2881.f1=({ struct Cyc_List_List* _temp2882=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2882->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2882->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2882;}); _temp2881;}); _temp2880;}); break; case 303:
_LL2879: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 304: _LL2883: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2885=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2885[ 0]=({ struct Cyc_Exp_tok_struct _temp2886; _temp2886.tag= Cyc_Exp_tok;
_temp2886.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2886;}); _temp2885;}); break; case 305: _LL2884: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 306: _LL2887:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2889=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2889[ 0]=({ struct Cyc_Exp_tok_struct
_temp2890; _temp2890.tag= Cyc_Exp_tok; _temp2890.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2890;}); _temp2889;}); break; case 307: _LL2888: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2892=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2892[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2893; _temp2893.tag= Cyc_Primopopt_tok; _temp2893.f1=
0; _temp2893;}); _temp2892;}); break; case 308: _LL2891: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2895=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2895[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2896; _temp2896.tag= Cyc_Primopopt_tok; _temp2896.f1=({
struct Cyc_Core_Opt* _temp2897=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2897->v=( void*)(( void*) Cyc_Absyn_Times); _temp2897;});
_temp2896;}); _temp2895;}); break; case 309: _LL2894: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2899=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2899[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2900; _temp2900.tag= Cyc_Primopopt_tok; _temp2900.f1=({
struct Cyc_Core_Opt* _temp2901=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2901->v=( void*)(( void*) Cyc_Absyn_Div); _temp2901;});
_temp2900;}); _temp2899;}); break; case 310: _LL2898: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2903=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2903[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2904; _temp2904.tag= Cyc_Primopopt_tok; _temp2904.f1=({
struct Cyc_Core_Opt* _temp2905=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2905->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2905;});
_temp2904;}); _temp2903;}); break; case 311: _LL2902: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2907=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2907[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2908; _temp2908.tag= Cyc_Primopopt_tok; _temp2908.f1=({
struct Cyc_Core_Opt* _temp2909=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2909->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2909;});
_temp2908;}); _temp2907;}); break; case 312: _LL2906: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2911=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2911[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2912; _temp2912.tag= Cyc_Primopopt_tok; _temp2912.f1=({
struct Cyc_Core_Opt* _temp2913=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2913->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2913;});
_temp2912;}); _temp2911;}); break; case 313: _LL2910: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2915=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2915[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2916; _temp2916.tag= Cyc_Primopopt_tok; _temp2916.f1=({
struct Cyc_Core_Opt* _temp2917=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2917->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2917;});
_temp2916;}); _temp2915;}); break; case 314: _LL2914: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2919=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2919[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2920; _temp2920.tag= Cyc_Primopopt_tok; _temp2920.f1=({
struct Cyc_Core_Opt* _temp2921=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2921->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2921;});
_temp2920;}); _temp2919;}); break; case 315: _LL2918: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2923=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2923[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2924; _temp2924.tag= Cyc_Primopopt_tok; _temp2924.f1=({
struct Cyc_Core_Opt* _temp2925=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2925->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2925;});
_temp2924;}); _temp2923;}); break; case 316: _LL2922: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2927=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2927[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2928; _temp2928.tag= Cyc_Primopopt_tok; _temp2928.f1=({
struct Cyc_Core_Opt* _temp2929=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2929->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2929;});
_temp2928;}); _temp2927;}); break; case 317: _LL2926: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2931=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2931[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2932; _temp2932.tag= Cyc_Primopopt_tok; _temp2932.f1=({
struct Cyc_Core_Opt* _temp2933=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2933->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2933;});
_temp2932;}); _temp2931;}); break; case 318: _LL2930: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 319: _LL2934:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2936=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2936[ 0]=({ struct Cyc_Exp_tok_struct
_temp2937; _temp2937.tag= Cyc_Exp_tok; _temp2937.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2937;});
_temp2936;}); break; case 320: _LL2935: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2939=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2939[ 0]=({ struct Cyc_Exp_tok_struct _temp2940; _temp2940.tag= Cyc_Exp_tok;
_temp2940.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2940;}); _temp2939;}); break; case 321: _LL2938: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2942=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2942[ 0]=({ struct Cyc_Exp_tok_struct
_temp2943; _temp2943.tag= Cyc_Exp_tok; _temp2943.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2943;}); _temp2942;}); break; case 322: _LL2941: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2945=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2945[ 0]=({ struct Cyc_Exp_tok_struct
_temp2946; _temp2946.tag= Cyc_Exp_tok; _temp2946.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2946;}); _temp2945;}); break; case 323: _LL2944: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2948=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2948[ 0]=({ struct Cyc_Exp_tok_struct
_temp2949; _temp2949.tag= Cyc_Exp_tok; _temp2949.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2949;});
_temp2948;}); break; case 324: _LL2947: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2951=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2951[ 0]=({ struct Cyc_Exp_tok_struct _temp2952; _temp2952.tag= Cyc_Exp_tok;
_temp2952.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2952;}); _temp2951;}); break; case 325: _LL2950: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 326: _LL2953:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
327: _LL2954: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2956=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2956[ 0]=({ struct Cyc_Exp_tok_struct
_temp2957; _temp2957.tag= Cyc_Exp_tok; _temp2957.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2957;}); _temp2956;}); break; case 328: _LL2955: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL2958:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2960=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2960[ 0]=({ struct Cyc_Exp_tok_struct
_temp2961; _temp2961.tag= Cyc_Exp_tok; _temp2961.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2961;}); _temp2960;}); break; case 330: _LL2959: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL2962:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2964=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2964[ 0]=({ struct Cyc_Exp_tok_struct
_temp2965; _temp2965.tag= Cyc_Exp_tok; _temp2965.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2965;});
_temp2964;}); break; case 332: _LL2963: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL2966:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2968=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2968[ 0]=({ struct Cyc_Exp_tok_struct
_temp2969; _temp2969.tag= Cyc_Exp_tok; _temp2969.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2969;});
_temp2968;}); break; case 334: _LL2967: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL2970:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2972=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2972[ 0]=({ struct Cyc_Exp_tok_struct
_temp2973; _temp2973.tag= Cyc_Exp_tok; _temp2973.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2973;});
_temp2972;}); break; case 336: _LL2971: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 337: _LL2974:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2976=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2976[ 0]=({ struct Cyc_Exp_tok_struct
_temp2977; _temp2977.tag= Cyc_Exp_tok; _temp2977.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2977;}); _temp2976;}); break; case 338: _LL2975: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2979=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2979[ 0]=({ struct Cyc_Exp_tok_struct
_temp2980; _temp2980.tag= Cyc_Exp_tok; _temp2980.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2980;}); _temp2979;}); break; case 339: _LL2978: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 340: _LL2981:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2983=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2983[ 0]=({ struct Cyc_Exp_tok_struct
_temp2984; _temp2984.tag= Cyc_Exp_tok; _temp2984.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2984;}); _temp2983;}); break; case 341: _LL2982: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2986=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2986[ 0]=({ struct Cyc_Exp_tok_struct
_temp2987; _temp2987.tag= Cyc_Exp_tok; _temp2987.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2987;}); _temp2986;}); break; case 342: _LL2985: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2989=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2989[ 0]=({ struct Cyc_Exp_tok_struct
_temp2990; _temp2990.tag= Cyc_Exp_tok; _temp2990.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2990;}); _temp2989;}); break; case 343: _LL2988: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2992=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2992[ 0]=({ struct Cyc_Exp_tok_struct
_temp2993; _temp2993.tag= Cyc_Exp_tok; _temp2993.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2993;}); _temp2992;}); break; case 344: _LL2991: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 345: _LL2994:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2996=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2996[ 0]=({ struct Cyc_Exp_tok_struct
_temp2997; _temp2997.tag= Cyc_Exp_tok; _temp2997.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2997;}); _temp2996;}); break; case 346: _LL2995: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2999=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2999[ 0]=({ struct Cyc_Exp_tok_struct
_temp3000; _temp3000.tag= Cyc_Exp_tok; _temp3000.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3000;}); _temp2999;}); break; case 347: _LL2998: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 348: _LL3001:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3003=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3003[ 0]=({ struct Cyc_Exp_tok_struct
_temp3004; _temp3004.tag= Cyc_Exp_tok; _temp3004.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3004;});
_temp3003;}); break; case 349: _LL3002: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3006=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3006[ 0]=({ struct Cyc_Exp_tok_struct _temp3007; _temp3007.tag= Cyc_Exp_tok;
_temp3007.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3007;}); _temp3006;}); break; case 350: _LL3005: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 351: _LL3008:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3010=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3010[ 0]=({ struct Cyc_Exp_tok_struct
_temp3011; _temp3011.tag= Cyc_Exp_tok; _temp3011.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3011;});
_temp3010;}); break; case 352: _LL3009: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3013=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3013[ 0]=({ struct Cyc_Exp_tok_struct _temp3014; _temp3014.tag= Cyc_Exp_tok;
_temp3014.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3014;}); _temp3013;}); break; case 353: _LL3012: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3016=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3016[ 0]=({ struct Cyc_Exp_tok_struct
_temp3017; _temp3017.tag= Cyc_Exp_tok; _temp3017.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3017;});
_temp3016;}); break; case 354: _LL3015: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 355: _LL3018:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3020=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3020[ 0]=({ struct Cyc_Exp_tok_struct
_temp3021; _temp3021.tag= Cyc_Exp_tok; _temp3021.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3021;}); _temp3020;}); break; case 356: _LL3019: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 357: _LL3022:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3024=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3024[ 0]=({ struct Cyc_Exp_tok_struct
_temp3025; _temp3025.tag= Cyc_Exp_tok; _temp3025.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3025;}); _temp3024;}); break; case 358: _LL3023: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3027=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3027[ 0]=({ struct Cyc_Exp_tok_struct
_temp3028; _temp3028.tag= Cyc_Exp_tok; _temp3028.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3028;}); _temp3027;}); break; case 359: _LL3026: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3030=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3030[ 0]=({ struct Cyc_Exp_tok_struct
_temp3031; _temp3031.tag= Cyc_Exp_tok; _temp3031.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3031;}); _temp3030;}); break; case 360: _LL3029: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3033=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3033[ 0]=({ struct Cyc_Exp_tok_struct
_temp3034; _temp3034.tag= Cyc_Exp_tok; _temp3034.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3034;}); _temp3033;}); break; case 361: _LL3032: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 362: _LL3035:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3037=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3037[ 0]=({ struct Cyc_Exp_tok_struct
_temp3038; _temp3038.tag= Cyc_Exp_tok; _temp3038.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3038;}); _temp3037;}); break; case 363: _LL3036: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3040=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3040[ 0]=({ struct Cyc_Exp_tok_struct
_temp3041; _temp3041.tag= Cyc_Exp_tok; _temp3041.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3041;});
_temp3040;}); break; case 364: _LL3039: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3043=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3043[ 0]=({ struct Cyc_Exp_tok_struct _temp3044; _temp3044.tag= Cyc_Exp_tok;
_temp3044.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3044;}); _temp3043;}); break; case 365: _LL3042: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3046=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3046[ 0]=({ struct Cyc_Exp_tok_struct
_temp3047; _temp3047.tag= Cyc_Exp_tok; _temp3047.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_StructField_struct*
_temp3048=( struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp3048[ 0]=({ struct Cyc_Absyn_StructField_struct _temp3049; _temp3049.tag=
Cyc_Absyn_StructField; _temp3049.f1=({ struct _tagged_arr* _temp3050=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp3050[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3050;});
_temp3049;}); _temp3048;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3047;});
_temp3046;}); break; case 366: _LL3045: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3052=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3052[ 0]=({ struct Cyc_Exp_tok_struct _temp3053; _temp3053.tag= Cyc_Exp_tok;
_temp3053.f1= Cyc_Absyn_offsetof_exp((* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])).f3,( void*)({
struct Cyc_Absyn_TupleIndex_struct* _temp3054=( struct Cyc_Absyn_TupleIndex_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Absyn_TupleIndex_struct)); _temp3054[ 0]=({
struct Cyc_Absyn_TupleIndex_struct _temp3055; _temp3055.tag= Cyc_Absyn_TupleIndex;
_temp3055.f1=( unsigned int)(* Cyc_yyget_Int_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f2; _temp3055;});
_temp3054;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3053;}); _temp3052;}); break; case 367:
_LL3051: { struct Cyc_Position_Segment* _temp3057= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); struct
Cyc_List_List* _temp3058=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, _temp3057, Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3059=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3059[ 0]=({ struct Cyc_Exp_tok_struct
_temp3060; _temp3060.tag= Cyc_Exp_tok; _temp3060.f1= Cyc_Absyn_gentyp_exp(
_temp3058,(* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3060;});
_temp3059;}); break;} case 368: _LL3056: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3062=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3062[ 0]=({ struct Cyc_Exp_tok_struct _temp3063; _temp3063.tag= Cyc_Exp_tok;
_temp3063.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3064=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3064[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3065; _temp3065.tag= Cyc_Absyn_Malloc_e;
_temp3065.f1= 0; _temp3065.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3065;}); _temp3064;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3063;});
_temp3062;}); break; case 369: _LL3061: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3067=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3067[ 0]=({ struct Cyc_Exp_tok_struct _temp3068; _temp3068.tag= Cyc_Exp_tok;
_temp3068.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3069=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3069[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3070; _temp3070.tag= Cyc_Absyn_Malloc_e;
_temp3070.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3070.f2=(
void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3070;}); _temp3069;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3068;});
_temp3067;}); break; case 370: _LL3066: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3072=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3072[ 0]=({ struct Cyc_Primop_tok_struct _temp3073; _temp3073.tag= Cyc_Primop_tok;
_temp3073.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3073;}); _temp3072;});
break; case 371: _LL3071: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3075=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3075[ 0]=({ struct Cyc_Primop_tok_struct _temp3076; _temp3076.tag= Cyc_Primop_tok;
_temp3076.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3076;}); _temp3075;});
break; case 372: _LL3074: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3078=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3078[ 0]=({ struct Cyc_Primop_tok_struct _temp3079; _temp3079.tag= Cyc_Primop_tok;
_temp3079.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3079;}); _temp3078;});
break; case 373: _LL3077: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 374: _LL3080: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3082=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3082[ 0]=({ struct Cyc_Exp_tok_struct _temp3083; _temp3083.tag= Cyc_Exp_tok;
_temp3083.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3083;}); _temp3082;}); break; case 375: _LL3081: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3085=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3085[ 0]=({ struct Cyc_Exp_tok_struct
_temp3086; _temp3086.tag= Cyc_Exp_tok; _temp3086.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3086;}); _temp3085;}); break; case 376:
_LL3084: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3088=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3088[ 0]=({ struct Cyc_Exp_tok_struct
_temp3089; _temp3089.tag= Cyc_Exp_tok; _temp3089.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3089;});
_temp3088;}); break; case 377: _LL3087: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3091=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3091[ 0]=({ struct Cyc_Exp_tok_struct _temp3092; _temp3092.tag= Cyc_Exp_tok;
_temp3092.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3093=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3093[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3093;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3092;}); _temp3091;}); break; case 378: _LL3090: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3095=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3095[ 0]=({ struct Cyc_Exp_tok_struct
_temp3096; _temp3096.tag= Cyc_Exp_tok; _temp3096.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3096;});
_temp3095;}); break;} case 379: _LL3094: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3098=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3098[ 0]=({ struct Cyc_Exp_tok_struct _temp3099; _temp3099.tag= Cyc_Exp_tok;
_temp3099.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3100=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3100[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3100;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3099;}); _temp3098;}); break; case 380: _LL3097: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3102=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3102[ 0]=({ struct Cyc_Exp_tok_struct
_temp3103; _temp3103.tag= Cyc_Exp_tok; _temp3103.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3103;});
_temp3102;}); break;} case 381: _LL3101: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3105=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3105[ 0]=({ struct Cyc_Exp_tok_struct _temp3106; _temp3106.tag= Cyc_Exp_tok;
_temp3106.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3106;}); _temp3105;}); break; case 382: _LL3104: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3108=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3108[ 0]=({ struct Cyc_Exp_tok_struct
_temp3109; _temp3109.tag= Cyc_Exp_tok; _temp3109.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3109;}); _temp3108;}); break; case 383: _LL3107: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3111=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3111[ 0]=({ struct Cyc_Exp_tok_struct
_temp3112; _temp3112.tag= Cyc_Exp_tok; _temp3112.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3113=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3113[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3114; _temp3114.tag= Cyc_Absyn_CompoundLit_e;
_temp3114.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3114.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3114;});
_temp3113;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3112;}); _temp3111;}); break; case 384:
_LL3110: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3116=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3116[ 0]=({ struct Cyc_Exp_tok_struct
_temp3117; _temp3117.tag= Cyc_Exp_tok; _temp3117.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3118=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3118[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3119; _temp3119.tag= Cyc_Absyn_CompoundLit_e;
_temp3119.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3119.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3119;});
_temp3118;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3117;}); _temp3116;}); break; case 385:
_LL3115: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3121=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3121[ 0]=({ struct Cyc_Exp_tok_struct
_temp3122; _temp3122.tag= Cyc_Exp_tok; _temp3122.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3123=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3123[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3124; _temp3124.tag= Cyc_Absyn_Fill_e; _temp3124.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3124;});
_temp3123;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3122;}); _temp3121;}); break; case 386:
_LL3120: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3126=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3126[ 0]=({ struct Cyc_Exp_tok_struct
_temp3127; _temp3127.tag= Cyc_Exp_tok; _temp3127.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3128=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3128[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3129; _temp3129.tag= Cyc_Absyn_Codegen_e;
_temp3129.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3129;}); _temp3128;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3127;}); _temp3126;}); break; case 387: _LL3125: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3131=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3131[ 0]=({ struct Cyc_Exp_tok_struct
_temp3132; _temp3132.tag= Cyc_Exp_tok; _temp3132.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3133=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3133[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3134; _temp3134.tag= Cyc_Absyn_UnknownId_e;
_temp3134.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3134;}); _temp3133;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3132;});
_temp3131;}); break; case 388: _LL3130: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 389: _LL3135:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3137=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3137[ 0]=({ struct Cyc_Exp_tok_struct
_temp3138; _temp3138.tag= Cyc_Exp_tok; _temp3138.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3138;});
_temp3137;}); break; case 390: _LL3136: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 391:
_LL3139: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3141=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3141[ 0]=({ struct Cyc_Exp_tok_struct
_temp3142; _temp3142.tag= Cyc_Exp_tok; _temp3142.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3143=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3143[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3144; _temp3144.tag=
Cyc_Absyn_UnknownId_e; _temp3144.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3144;});
_temp3143;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3142;});
_temp3141;}); break; case 392: _LL3140: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3146=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3146[ 0]=({ struct Cyc_Exp_tok_struct _temp3147; _temp3147.tag= Cyc_Exp_tok;
_temp3147.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3148=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3148[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3149; _temp3149.tag=
Cyc_Absyn_UnknownId_e; _temp3149.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3149;});
_temp3148;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3147;}); _temp3146;}); break; case 393: _LL3145: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3151=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3151[ 0]=({ struct Cyc_Exp_tok_struct
_temp3152; _temp3152.tag= Cyc_Exp_tok; _temp3152.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3152;}); _temp3151;}); break; case 394: _LL3150: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3154=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3154[ 0]=({ struct Cyc_Exp_tok_struct
_temp3155; _temp3155.tag= Cyc_Exp_tok; _temp3155.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3156=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3156[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3157; _temp3157.tag= Cyc_Absyn_Struct_e;
_temp3157.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3157.f2= 0; _temp3157.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3157.f4= 0;
_temp3157;}); _temp3156;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3155;});
_temp3154;}); break; case 395: _LL3153: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3159=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3159[ 0]=({ struct Cyc_Exp_tok_struct _temp3160; _temp3160.tag= Cyc_Exp_tok;
_temp3160.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3160;}); _temp3159;}); break; case 396: _LL3158: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3162=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3162[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3163; _temp3163.tag= Cyc_ExpList_tok; _temp3163.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3163;});
_temp3162;}); break; case 397: _LL3161: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3165=( struct Cyc_ExpList_tok_struct*) _cycalloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3165[ 0]=({ struct Cyc_ExpList_tok_struct _temp3166; _temp3166.tag= Cyc_ExpList_tok;
_temp3166.f1=({ struct Cyc_List_List* _temp3167=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3167->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3167->tl= 0;
_temp3167;}); _temp3166;}); _temp3165;}); break; case 398: _LL3164: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3169=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3169[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3170; _temp3170.tag= Cyc_ExpList_tok; _temp3170.f1=({ struct Cyc_List_List*
_temp3171=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3171->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3171->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp3171;});
_temp3170;}); _temp3169;}); break; case 399: _LL3168: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3173=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3173[ 0]=({ struct Cyc_Exp_tok_struct
_temp3174; _temp3174.tag= Cyc_Exp_tok; _temp3174.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3174;});
_temp3173;}); break; case 400: _LL3172: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3176=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3176[ 0]=({ struct Cyc_Exp_tok_struct _temp3177; _temp3177.tag= Cyc_Exp_tok;
_temp3177.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3177;});
_temp3176;}); break; case 401: _LL3175: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3179=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3179[ 0]=({ struct Cyc_Exp_tok_struct _temp3180; _temp3180.tag= Cyc_Exp_tok;
_temp3180.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3180;});
_temp3179;}); break; case 402: _LL3178: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3182=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3182[ 0]=({ struct Cyc_Exp_tok_struct _temp3183; _temp3183.tag= Cyc_Exp_tok;
_temp3183.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3183;});
_temp3182;}); break; case 403: _LL3181: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3185=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3185[ 0]=({ struct Cyc_QualId_tok_struct _temp3186; _temp3186.tag= Cyc_QualId_tok;
_temp3186.f1=({ struct _tuple1* _temp3187=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3187->f1= Cyc_Absyn_rel_ns_null; _temp3187->f2=({ struct
_tagged_arr* _temp3188=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3188[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3188;}); _temp3187;});
_temp3186;}); _temp3185;}); break; case 404: _LL3184: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3189:(
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
count ++);}} msg=({ unsigned int _temp3191=( unsigned int)( sze +  15);
unsigned char* _temp3192=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp3191)); struct _tagged_arr _temp3194= _tag_arr(
_temp3192, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp3193= _temp3191; unsigned int i; for( i= 0; i <  _temp3193; i ++){
_temp3192[ i]='\000';}}; _temp3194;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
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
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3204= v; struct
_tuple15* _temp3222; struct _tuple15 _temp3224; int _temp3225; unsigned char
_temp3227; short _temp3229; struct _tagged_arr _temp3231; struct Cyc_Core_Opt*
_temp3233; struct Cyc_Core_Opt* _temp3235; struct Cyc_Core_Opt _temp3237; struct
_tagged_arr* _temp3238; struct _tuple1* _temp3240; struct _tuple1 _temp3242;
struct _tagged_arr* _temp3243; void* _temp3245; _LL3206: if(*(( void**)
_temp3204) ==  Cyc_Int_tok){ _LL3223: _temp3222=(( struct Cyc_Int_tok_struct*)
_temp3204)->f1; _temp3224=* _temp3222; _LL3226: _temp3225= _temp3224.f2; goto
_LL3207;} else{ goto _LL3208;} _LL3208: if(*(( void**) _temp3204) ==  Cyc_Char_tok){
_LL3228: _temp3227=(( struct Cyc_Char_tok_struct*) _temp3204)->f1; goto _LL3209;}
else{ goto _LL3210;} _LL3210: if(*(( void**) _temp3204) ==  Cyc_Short_tok){
_LL3230: _temp3229=(( struct Cyc_Short_tok_struct*) _temp3204)->f1; goto _LL3211;}
else{ goto _LL3212;} _LL3212: if(*(( void**) _temp3204) ==  Cyc_String_tok){
_LL3232: _temp3231=(( struct Cyc_String_tok_struct*) _temp3204)->f1; goto
_LL3213;} else{ goto _LL3214;} _LL3214: if(*(( void**) _temp3204) ==  Cyc_Stringopt_tok){
_LL3234: _temp3233=(( struct Cyc_Stringopt_tok_struct*) _temp3204)->f1; if(
_temp3233 ==  0){ goto _LL3215;} else{ goto _LL3216;}} else{ goto _LL3216;}
_LL3216: if(*(( void**) _temp3204) ==  Cyc_Stringopt_tok){ _LL3236: _temp3235=((
struct Cyc_Stringopt_tok_struct*) _temp3204)->f1; if( _temp3235 ==  0){ goto
_LL3218;} else{ _temp3237=* _temp3235; _LL3239: _temp3238=( struct _tagged_arr*)
_temp3237.v; goto _LL3217;}} else{ goto _LL3218;} _LL3218: if(*(( void**)
_temp3204) ==  Cyc_QualId_tok){ _LL3241: _temp3240=(( struct Cyc_QualId_tok_struct*)
_temp3204)->f1; _temp3242=* _temp3240; _LL3246: _temp3245= _temp3242.f1; goto
_LL3244; _LL3244: _temp3243= _temp3242.f2; goto _LL3219;} else{ goto _LL3220;}
_LL3220: goto _LL3221; _LL3207:({ struct Cyc_Std_Int_pa_struct _temp3248;
_temp3248.tag= Cyc_Std_Int_pa; _temp3248.f1=( int)(( unsigned int) _temp3225);{
void* _temp3247[ 1u]={& _temp3248}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3247, sizeof( void*), 1u));}}); goto
_LL3205; _LL3209:({ struct Cyc_Std_Int_pa_struct _temp3250; _temp3250.tag= Cyc_Std_Int_pa;
_temp3250.f1=( int)(( unsigned int)(( int) _temp3227));{ void* _temp3249[ 1u]={&
_temp3250}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3249, sizeof( void*), 1u));}}); goto _LL3205;
_LL3211:({ struct Cyc_Std_Int_pa_struct _temp3252; _temp3252.tag= Cyc_Std_Int_pa;
_temp3252.f1=( int)(( unsigned int)(( int) _temp3229));{ void* _temp3251[ 1u]={&
_temp3252}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3251, sizeof( void*), 1u));}}); goto _LL3205;
_LL3213:({ struct Cyc_Std_String_pa_struct _temp3254; _temp3254.tag= Cyc_Std_String_pa;
_temp3254.f1=( struct _tagged_arr) _temp3231;{ void* _temp3253[ 1u]={& _temp3254};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3253, sizeof( void*), 1u));}}); goto _LL3205; _LL3215:({ void*
_temp3255[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3255, sizeof( void*), 0u));}); goto _LL3205;
_LL3217:({ struct Cyc_Std_String_pa_struct _temp3257; _temp3257.tag= Cyc_Std_String_pa;
_temp3257.f1=( struct _tagged_arr)* _temp3238;{ void* _temp3256[ 1u]={&
_temp3257}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3256, sizeof( void*), 1u));}}); goto _LL3205;
_LL3219: { struct Cyc_List_List* _temp3258= 0;{ void* _temp3259= _temp3245;
struct Cyc_List_List* _temp3267; struct Cyc_List_List* _temp3269; _LL3261: if((
unsigned int) _temp3259 >  1u?*(( int*) _temp3259) ==  Cyc_Absyn_Rel_n: 0){
_LL3268: _temp3267=(( struct Cyc_Absyn_Rel_n_struct*) _temp3259)->f1; goto
_LL3262;} else{ goto _LL3263;} _LL3263: if(( unsigned int) _temp3259 >  1u?*((
int*) _temp3259) ==  Cyc_Absyn_Abs_n: 0){ _LL3270: _temp3269=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3259)->f1; goto _LL3264;} else{ goto _LL3265;} _LL3265: if( _temp3259 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3266;} else{ goto _LL3260;} _LL3262: _temp3258=
_temp3267; goto _LL3260; _LL3264: _temp3258= _temp3269; goto _LL3260; _LL3266:
goto _LL3260; _LL3260:;} for( 0; _temp3258 !=  0; _temp3258=(( struct Cyc_List_List*)
_check_null( _temp3258))->tl){({ struct Cyc_Std_String_pa_struct _temp3272;
_temp3272.tag= Cyc_Std_String_pa; _temp3272.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp3258))->hd);{ void*
_temp3271[ 1u]={& _temp3272}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3271, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3274; _temp3274.tag= Cyc_Std_String_pa;
_temp3274.f1=( struct _tagged_arr)* _temp3243;{ void* _temp3273[ 1u]={&
_temp3274}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3273, sizeof( void*), 1u));}}); goto _LL3205;}
_LL3221:({ void* _temp3275[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3275, sizeof( void*), 0u));}); goto
_LL3205; _LL3205:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3276=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3276->v=(
void*) Cyc_Lexing_from_file( f); _temp3276;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
