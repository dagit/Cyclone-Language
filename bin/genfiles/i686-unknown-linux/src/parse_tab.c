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

#if defined(CYC_REGION_PROFILE) 
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
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
46u), _tag_arr( _temp22, sizeof( void*), 2u));}}}); return;} static int Cyc_Parse_enum_counter=
0; struct _tuple1* Cyc_Parse_gensym_enum(){ return({ struct _tuple1* _temp25=(
struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp25->f1=( void*)({
struct Cyc_Absyn_Rel_n_struct* _temp29=( struct Cyc_Absyn_Rel_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp29[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp30; _temp30.tag= Cyc_Absyn_Rel_n; _temp30.f1= 0; _temp30;}); _temp29;});
_temp25->f2=({ struct _tagged_arr* _temp26=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp26[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp28; _temp28.tag= Cyc_Std_Int_pa; _temp28.f1=( int)(( unsigned int) Cyc_Parse_enum_counter
++);{ void* _temp27[ 1u]={& _temp28}; Cyc_Std_aprintf( _tag_arr("__anonymous_enum_%d__",
sizeof( unsigned char), 22u), _tag_arr( _temp27, sizeof( void*), 1u));}});
_temp26;}); _temp25;});} struct _tuple9{ struct _tuple7* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field( struct
Cyc_Position_Segment* loc, struct _tuple9* field_info){ struct _tuple9 _temp33;
struct Cyc_Absyn_Exp* _temp34; struct _tuple7* _temp36; struct _tuple7 _temp38;
struct Cyc_List_List* _temp39; struct Cyc_List_List* _temp41; void* _temp43;
struct Cyc_Absyn_Tqual _temp45; struct _tuple1* _temp47; struct _tuple9* _temp31=
field_info; _temp33=* _temp31; _LL37: _temp36= _temp33.f1; _temp38=* _temp36;
_LL48: _temp47= _temp38.f1; goto _LL46; _LL46: _temp45= _temp38.f2; goto _LL44;
_LL44: _temp43= _temp38.f3; goto _LL42; _LL42: _temp41= _temp38.f4; goto _LL40;
_LL40: _temp39= _temp38.f5; goto _LL35; _LL35: _temp34= _temp33.f2; goto _LL32;
_LL32: if( _temp41 !=  0){ Cyc_Parse_err( _tag_arr("bad type params in struct field",
sizeof( unsigned char), 32u), loc);} if( Cyc_Absyn_is_qvar_qualified( _temp47)){
Cyc_Parse_err( _tag_arr("struct field cannot be qualified with a namespace",
sizeof( unsigned char), 50u), loc);} return({ struct Cyc_Absyn_Structfield*
_temp49=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp49->name=(* _temp47).f2; _temp49->tq= _temp45; _temp49->type=( void*)
_temp43; _temp49->width= _temp34; _temp49->attributes= _temp39; _temp49;});}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp52; struct Cyc_List_List* _temp53; void* _temp55; struct Cyc_Absyn_Tqual
_temp57; struct Cyc_Core_Opt* _temp59; struct _tuple10* _temp50= field; _temp52=*
_temp50; _LL60: _temp59= _temp52.f1; goto _LL58; _LL58: _temp57= _temp52.f2;
goto _LL56; _LL56: _temp55= _temp52.f3; goto _LL54; _LL54: _temp53= _temp52.f4;
goto _LL51; _LL51: { struct Cyc_Core_Opt* _temp61= 0; if( _temp59 !=  0){ if(
Cyc_Absyn_is_qvar_qualified(( struct _tuple1*) _temp59->v)){ Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace", sizeof( unsigned char),
47u), loc);} _temp61=({ struct Cyc_Core_Opt* _temp62=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp62->v=( void*)(*(( struct _tuple1*)
_temp59->v)).f2; _temp62;});} if( _temp53 !=  0){(( int(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("parameter should have no type parameters",
sizeof( unsigned char), 41u), loc);} return({ struct _tuple2* _temp63=( struct
_tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp63->f1= _temp61; _temp63->f2=
_temp57; _temp63->f3= _temp55; _temp63;});}} static void* Cyc_Parse_type_spec(
void* t, struct Cyc_Position_Segment* loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp64=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp64[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp65; _temp65.tag= Cyc_Parse_Type_spec;
_temp65.f1=( void*) t; _temp65.f2= loc; _temp65;}); _temp64;});} static void*
Cyc_Parse_array2ptr( void* t){ void* _temp66= t; struct Cyc_Absyn_Exp* _temp72;
struct Cyc_Absyn_Tqual _temp74; void* _temp76; _LL68: if(( unsigned int) _temp66
>  3u?*(( int*) _temp66) ==  Cyc_Absyn_ArrayType: 0){ _LL77: _temp76=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp66)->f1; goto _LL75; _LL75: _temp74=((
struct Cyc_Absyn_ArrayType_struct*) _temp66)->f2; goto _LL73; _LL73: _temp72=((
struct Cyc_Absyn_ArrayType_struct*) _temp66)->f3; goto _LL69;} else{ goto _LL70;}
_LL70: goto _LL71; _LL69: return Cyc_Absyn_starb_typ( _temp76,( void*) Cyc_Absyn_HeapRgn,
_temp74, _temp72 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp78=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp78[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp79; _temp79.tag= Cyc_Absyn_Upper_b;
_temp79.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp72); _temp79;}); _temp78;}));
_LL71: return t; _LL67:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment*
loc, struct _tuple2* t){ return({ struct _tuple11* _temp80=( struct _tuple11*)
_cycalloc( sizeof( struct _tuple11)); _temp80->f1=(* t).f2; _temp80->f2=(* t).f3;
_temp80;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_arr decl_kind;{ void* _temp81=( void*)
x->r; struct Cyc_Absyn_Vardecl* _temp107; _LL83: if(*(( int*) _temp81) ==  Cyc_Absyn_Var_d){
_LL108: _temp107=(( struct Cyc_Absyn_Var_d_struct*) _temp81)->f1; goto _LL84;}
else{ goto _LL85;} _LL85: if(*(( int*) _temp81) ==  Cyc_Absyn_Let_d){ goto _LL86;}
else{ goto _LL87;} _LL87: if(*(( int*) _temp81) ==  Cyc_Absyn_Letv_d){ goto
_LL88;} else{ goto _LL89;} _LL89: if(*(( int*) _temp81) ==  Cyc_Absyn_Fn_d){
goto _LL90;} else{ goto _LL91;} _LL91: if(*(( int*) _temp81) ==  Cyc_Absyn_Struct_d){
goto _LL92;} else{ goto _LL93;} _LL93: if(*(( int*) _temp81) ==  Cyc_Absyn_Union_d){
goto _LL94;} else{ goto _LL95;} _LL95: if(*(( int*) _temp81) ==  Cyc_Absyn_Tunion_d){
goto _LL96;} else{ goto _LL97;} _LL97: if(*(( int*) _temp81) ==  Cyc_Absyn_Typedef_d){
goto _LL98;} else{ goto _LL99;} _LL99: if(*(( int*) _temp81) ==  Cyc_Absyn_Enum_d){
goto _LL100;} else{ goto _LL101;} _LL101: if(*(( int*) _temp81) ==  Cyc_Absyn_Namespace_d){
goto _LL102;} else{ goto _LL103;} _LL103: if(*(( int*) _temp81) ==  Cyc_Absyn_Using_d){
goto _LL104;} else{ goto _LL105;} _LL105: if(*(( int*) _temp81) ==  Cyc_Absyn_ExternC_d){
goto _LL106;} else{ goto _LL82;} _LL84: if( _temp107->initializer !=  0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);} if( Cyc_Absyn_is_qvar_qualified( _temp107->name)){
Cyc_Parse_err( _tag_arr("namespaces not allowed on parameter declarations",
sizeof( unsigned char), 49u), x->loc);}{ int found= 0; for( 0; params !=  0;
params= params->tl){ if( Cyc_Std_zstrptrcmp((* _temp107->name).f2,( struct
_tagged_arr*) params->hd) ==  0){ found= 1; break;}} if( ! found){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp110; _temp110.tag=
Cyc_Std_String_pa; _temp110.f1=( struct _tagged_arr)*(* _temp107->name).f2;{
void* _temp109[ 1u]={& _temp110}; Cyc_Std_aprintf( _tag_arr("%s is not listed as a parameter",
sizeof( unsigned char), 32u), _tag_arr( _temp109, sizeof( void*), 1u));}}), x->loc);}
return;} _LL86: decl_kind= _tag_arr("let declaration", sizeof( unsigned char),
16u); goto _LL82; _LL88: decl_kind= _tag_arr("let declaration", sizeof(
unsigned char), 16u); goto _LL82; _LL90: decl_kind= _tag_arr("function declaration",
sizeof( unsigned char), 21u); goto _LL82; _LL92: decl_kind= _tag_arr("struct declaration",
sizeof( unsigned char), 19u); goto _LL82; _LL94: decl_kind= _tag_arr("union declaration",
sizeof( unsigned char), 18u); goto _LL82; _LL96: decl_kind= _tag_arr("tunion declaration",
sizeof( unsigned char), 19u); goto _LL82; _LL98: decl_kind= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL82; _LL100: decl_kind= _tag_arr("enum declaration",
sizeof( unsigned char), 17u); goto _LL82; _LL102: decl_kind= _tag_arr("namespace declaration",
sizeof( unsigned char), 22u); goto _LL82; _LL104: decl_kind= _tag_arr("using declaration",
sizeof( unsigned char), 18u); goto _LL82; _LL106: decl_kind= _tag_arr("extern C declaration",
sizeof( unsigned char), 21u); goto _LL82; _LL82:;}(( int(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp112; _temp112.tag= Cyc_Std_String_pa;
_temp112.f1=( struct _tagged_arr) decl_kind;{ void* _temp111[ 1u]={& _temp112};
Cyc_Std_aprintf( _tag_arr("%s appears in parameter type", sizeof( unsigned char),
29u), _tag_arr( _temp111, sizeof( void*), 1u));}}), x->loc); return;} struct
_tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_arr* x){ struct _tuple12 _temp115; struct Cyc_Position_Segment* _temp116;
struct Cyc_List_List* _temp118; struct _tuple12* _temp113= env; _temp115=*
_temp113; _LL119: _temp118= _temp115.f1; goto _LL117; _LL117: _temp116= _temp115.f2;
goto _LL114; _LL114: if( _temp118 ==  0){ return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp121; _temp121.tag= Cyc_Std_String_pa;
_temp121.f1=( struct _tagged_arr)* x;{ void* _temp120[ 1u]={& _temp121}; Cyc_Std_aprintf(
_tag_arr("missing type for parameter %s", sizeof( unsigned char), 30u), _tag_arr(
_temp120, sizeof( void*), 1u));}}), _temp116);}{ void* _temp122=( void*)((
struct Cyc_Absyn_Decl*) _temp118->hd)->r; struct Cyc_Absyn_Vardecl* _temp128;
_LL124: if(*(( int*) _temp122) ==  Cyc_Absyn_Var_d){ _LL129: _temp128=(( struct
Cyc_Absyn_Var_d_struct*) _temp122)->f1; goto _LL125;} else{ goto _LL126;} _LL126:
goto _LL127; _LL125: if( Cyc_Absyn_is_qvar_qualified( _temp128->name)){ Cyc_Parse_err(
_tag_arr("namespace not allowed on parameter", sizeof( unsigned char), 35u),
_temp116);} if( Cyc_Std_zstrptrcmp((* _temp128->name).f2, x) ==  0){ return({
struct _tuple2* _temp130=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp130->f1=({ struct Cyc_Core_Opt* _temp131=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp131->v=( void*)(* _temp128->name).f2;
_temp131;}); _temp130->f2= _temp128->tq; _temp130->f3=( void*) _temp128->type;
_temp130;});} else{ return Cyc_Parse_get_param_type(({ struct _tuple12* _temp132=(
struct _tuple12*) _cycalloc( sizeof( struct _tuple12)); _temp132->f1= _temp118->tl;
_temp132->f2= _temp116; _temp132;}), x);} _LL127: return(( struct _tuple2*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("non-variable declaration", sizeof( unsigned char), 25u),(( struct Cyc_Absyn_Decl*)
_temp118->hd)->loc); _LL123:;}} static int Cyc_Parse_is_typeparam( void* tm){
void* _temp133= tm; _LL135: if(( unsigned int) _temp133 >  1u?*(( int*) _temp133)
==  Cyc_Absyn_TypeParams_mod: 0){ goto _LL136;} else{ goto _LL137;} _LL137: goto
_LL138; _LL136: return 1; _LL138: return 0; _LL134:;} static void* Cyc_Parse_id2type(
struct _tagged_arr s, struct Cyc_Absyn_Conref* k){ if( Cyc_Std_zstrcmp( s,
_tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){ return( void*) Cyc_Absyn_HeapRgn;}
else{ return( void*)({ struct Cyc_Absyn_VarType_struct* _temp139=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp139[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp140; _temp140.tag= Cyc_Absyn_VarType; _temp140.f1=({ struct Cyc_Absyn_Tvar*
_temp141=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp141->name=({ struct _tagged_arr* _temp142=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp142[ 0]= s; _temp142;}); _temp141->identity=
0; _temp141->kind= k; _temp141;}); _temp140;}); _temp139;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar( struct Cyc_Absyn_Tvar* t){ struct Cyc_Absyn_Conref* k;{
void* _temp143=( void*)( Cyc_Absyn_compress_conref( t->kind))->v; void* _temp149;
_LL145: if(( unsigned int) _temp143 >  1u?*(( int*) _temp143) ==  Cyc_Absyn_Eq_constr:
0){ _LL150: _temp149=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp143)->f1;
goto _LL146;} else{ goto _LL147;} _LL147: goto _LL148; _LL146: k= Cyc_Absyn_new_conref(
_temp149); goto _LL144; _LL148: k= Cyc_Absyn_empty_conref(); goto _LL144; _LL144:;}
return({ struct Cyc_Absyn_Tvar* _temp151=( struct Cyc_Absyn_Tvar*) _cycalloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp151->name= t->name; _temp151->identity= 0;
_temp151->kind= k; _temp151;});} static struct Cyc_Absyn_Tvar* Cyc_Parse_typ2tvar(
struct Cyc_Position_Segment* loc, void* t){ void* _temp152= t; struct Cyc_Absyn_Tvar*
_temp158; _LL154: if(( unsigned int) _temp152 >  3u?*(( int*) _temp152) ==  Cyc_Absyn_VarType:
0){ _LL159: _temp158=(( struct Cyc_Absyn_VarType_struct*) _temp152)->f1; goto
_LL155;} else{ goto _LL156;} _LL156: goto _LL157; _LL155: return _temp158;
_LL157: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL153:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp160=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp160[ 0]=({ struct Cyc_Absyn_VarType_struct _temp161; _temp161.tag= Cyc_Absyn_VarType;
_temp161.f1= pr; _temp161;}); _temp160;});} static int Cyc_Parse_set_vartyp_kind(
void* t, void* k){ void* _temp162= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp168; struct Cyc_Absyn_Tvar _temp170; struct Cyc_Absyn_Conref* _temp171;
_LL164: if(( unsigned int) _temp162 >  3u?*(( int*) _temp162) ==  Cyc_Absyn_VarType:
0){ _LL169: _temp168=(( struct Cyc_Absyn_VarType_struct*) _temp162)->f1;
_temp170=* _temp168; _LL172: _temp171= _temp170.kind; goto _LL165;} else{ goto
_LL166;} _LL166: goto _LL167; _LL165: _temp171= Cyc_Absyn_compress_conref(
_temp171);{ void* _temp173=( void*) _temp171->v; void* _temp181; _LL175: if(
_temp173 == ( void*) Cyc_Absyn_No_constr){ goto _LL176;} else{ goto _LL177;}
_LL177: if(( unsigned int) _temp173 >  1u?*(( int*) _temp173) ==  Cyc_Absyn_Eq_constr:
0){ _LL182: _temp181=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp173)->f1;
goto _LL178;} else{ goto _LL179;} _LL179: goto _LL180; _LL176:( void*)( _temp171->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp183=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp183[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp184; _temp184.tag= Cyc_Absyn_Eq_constr; _temp184.f1=(
void*) k; _temp184;}); _temp183;}))); return 1; _LL178: return k ==  _temp181;
_LL180:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp185=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp185[ 0]=({ struct Cyc_Core_Impossible_struct _temp186; _temp186.tag= Cyc_Core_Impossible;
_temp186.f1= _tag_arr("forward after compress_conref", sizeof( unsigned char),
30u); _temp186;}); _temp185;})); _LL174:;} _LL167: return 0; _LL163:;} static
struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List* tms,
struct Cyc_List_List* tds, struct Cyc_Position_Segment* loc){ if( tds ==  0){
return tms;} if( tms ==  0){ return 0;}{ void* _temp187=( void*) tms->hd; void*
_temp193; _LL189: if(( unsigned int) _temp187 >  1u?*(( int*) _temp187) ==  Cyc_Absyn_Function_mod:
0){ _LL194: _temp193=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp187)->f1;
goto _LL190;} else{ goto _LL191;} _LL191: goto _LL192; _LL190: if( tms->tl ==  0?
1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd)?((
struct Cyc_List_List*) _check_null( tms->tl))->tl ==  0: 0)){ void* _temp195=
_temp193; struct Cyc_List_List* _temp201; _LL197: if(*(( int*) _temp195) ==  Cyc_Absyn_WithTypes){
goto _LL198;} else{ goto _LL199;} _LL199: if(*(( int*) _temp195) ==  Cyc_Absyn_NoTypes){
_LL202: _temp201=(( struct Cyc_Absyn_NoTypes_struct*) _temp195)->f1; goto _LL200;}
else{ goto _LL196;} _LL198: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL200:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp201,
tds);{ struct _tuple12* _temp203=({ struct _tuple12* _temp209=( struct _tuple12*)
_cycalloc( sizeof( struct _tuple12)); _temp209->f1= tds; _temp209->f2= loc;
_temp209;}); return({ struct Cyc_List_List* _temp204=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp204->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp205=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp205[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp206; _temp206.tag= Cyc_Absyn_Function_mod;
_temp206.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp207=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp207[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp208; _temp208.tag= Cyc_Absyn_WithTypes;
_temp208.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp203, _temp201); _temp208.f2= 0; _temp208.f3= 0;
_temp208.f4= 0; _temp208.f5= 0; _temp208;}); _temp207;})); _temp206;}); _temp205;}));
_temp204->tl= 0; _temp204;});} _LL196:;} else{ return({ struct Cyc_List_List*
_temp210=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp210->hd=( void*)(( void*) tms->hd); _temp210->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp210;});} _LL192: return({ struct Cyc_List_List*
_temp211=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp211->hd=( void*)(( void*) tms->hd); _temp211->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp211;}); _LL188:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds != 
0){ d=({ struct Cyc_Parse_Declarator* _temp212=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp212->id= d->id; _temp212->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp212;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso !=  0){ tss=(( struct
Cyc_Parse_Declaration_spec*) dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline;
atts=(( struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct
Cyc_Parse_Declaration_spec*) dso->v)->sc !=  0){ void* _temp213=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*) dso->v)->sc))->v;
_LL215: if( _temp213 == ( void*) Cyc_Parse_Extern_sc){ goto _LL216;} else{ goto
_LL217;} _LL217: if( _temp213 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL218;}
else{ goto _LL219;} _LL219: if( _temp213 == ( void*) Cyc_Parse_Static_sc){ goto
_LL220;} else{ goto _LL221;} _LL221: goto _LL222; _LL216: sc=( void*) Cyc_Absyn_Extern;
goto _LL214; _LL218: sc=( void*) Cyc_Absyn_ExternC; goto _LL214; _LL220: sc=(
void*) Cyc_Absyn_Static; goto _LL214; _LL222: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL214; _LL214:;}}{ struct Cyc_Core_Opt*
_temp225; void* _temp227; struct _tuple5 _temp223= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL228: _temp227= _temp223.f1; goto _LL226; _LL226: _temp225=
_temp223.f2; goto _LL224; _LL224: { struct Cyc_List_List* _temp231; struct Cyc_List_List*
_temp233; void* _temp235; struct Cyc_Absyn_Tqual _temp237; struct _tuple6
_temp229= Cyc_Parse_apply_tms( tq, _temp227, atts, d->tms); _LL238: _temp237=
_temp229.f1; goto _LL236; _LL236: _temp235= _temp229.f2; goto _LL234; _LL234:
_temp233= _temp229.f3; goto _LL232; _LL232: _temp231= _temp229.f4; goto _LL230;
_LL230: if( _temp225 !=  0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp233 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp239= _temp235; struct Cyc_Absyn_FnInfo _temp245; struct Cyc_List_List*
_temp247; struct Cyc_List_List* _temp249; struct Cyc_Absyn_VarargInfo* _temp251;
int _temp253; struct Cyc_List_List* _temp255; void* _temp257; struct Cyc_Core_Opt*
_temp259; struct Cyc_List_List* _temp261; _LL241: if(( unsigned int) _temp239 > 
3u?*(( int*) _temp239) ==  Cyc_Absyn_FnType: 0){ _LL246: _temp245=(( struct Cyc_Absyn_FnType_struct*)
_temp239)->f1; _LL262: _temp261= _temp245.tvars; goto _LL260; _LL260: _temp259=
_temp245.effect; goto _LL258; _LL258: _temp257=( void*) _temp245.ret_typ; goto
_LL256; _LL256: _temp255= _temp245.args; goto _LL254; _LL254: _temp253= _temp245.c_varargs;
goto _LL252; _LL252: _temp251= _temp245.cyc_varargs; goto _LL250; _LL250:
_temp249= _temp245.rgn_po; goto _LL248; _LL248: _temp247= _temp245.attributes;
goto _LL242;} else{ goto _LL243;} _LL243: goto _LL244; _LL242: { struct Cyc_List_List*
_temp263=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp255); return({ struct Cyc_Absyn_Fndecl*
_temp264=( struct Cyc_Absyn_Fndecl*) _cycalloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp264->sc=( void*) sc; _temp264->name= d->id; _temp264->tvs= _temp261;
_temp264->is_inline= is_inline; _temp264->effect= _temp259; _temp264->ret_type=(
void*) _temp257; _temp264->args= _temp263; _temp264->c_varargs= _temp253;
_temp264->cyc_varargs= _temp251; _temp264->rgn_po= _temp249; _temp264->body=
body; _temp264->cached_typ= 0; _temp264->param_vardecls= 0; _temp264->attributes=
Cyc_List_append( _temp247, _temp231); _temp264;});} _LL244: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL240:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 ==  0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp265=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp265->f1= _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"?", sizeof( unsigned char),
2u); _temp265->f2=(* t).f2; _temp265->f3=(* t).f3; _temp265;});} else{ return({
struct _tuple4* _temp266=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp266->f1=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp266->f2=(* t).f2; _temp266->f3=(* t).f3; _temp266;});}} static
unsigned char _temp267[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp267, _temp267, _temp267 +  52u};
static unsigned char _temp268[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp268, _temp268, _temp268 +  63u};
static unsigned char _temp269[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp269, _temp269, _temp269 +  50u};
static unsigned char _temp270[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp270, _temp270, _temp270 +  60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts !=  0; ts= ts->tl){ void* _temp271=( void*) ts->hd;
struct Cyc_Position_Segment* _temp285; void* _temp287; struct Cyc_Position_Segment*
_temp289; struct Cyc_Position_Segment* _temp291; struct Cyc_Position_Segment*
_temp293; struct Cyc_Position_Segment* _temp295; struct Cyc_Absyn_Decl* _temp297;
_LL273: if(*(( int*) _temp271) ==  Cyc_Parse_Type_spec){ _LL288: _temp287=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp271)->f1; goto _LL286; _LL286: _temp285=((
struct Cyc_Parse_Type_spec_struct*) _temp271)->f2; goto _LL274;} else{ goto
_LL275;} _LL275: if(*(( int*) _temp271) ==  Cyc_Parse_Signed_spec){ _LL290:
_temp289=(( struct Cyc_Parse_Signed_spec_struct*) _temp271)->f1; goto _LL276;}
else{ goto _LL277;} _LL277: if(*(( int*) _temp271) ==  Cyc_Parse_Unsigned_spec){
_LL292: _temp291=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp271)->f1; goto
_LL278;} else{ goto _LL279;} _LL279: if(*(( int*) _temp271) ==  Cyc_Parse_Short_spec){
_LL294: _temp293=(( struct Cyc_Parse_Short_spec_struct*) _temp271)->f1; goto
_LL280;} else{ goto _LL281;} _LL281: if(*(( int*) _temp271) ==  Cyc_Parse_Long_spec){
_LL296: _temp295=(( struct Cyc_Parse_Long_spec_struct*) _temp271)->f1; goto
_LL282;} else{ goto _LL283;} _LL283: if(*(( int*) _temp271) ==  Cyc_Parse_Decl_spec){
_LL298: _temp297=(( struct Cyc_Parse_Decl_spec_struct*) _temp271)->f1; goto
_LL284;} else{ goto _LL272;} _LL274: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp285);} last_loc= _temp285; seen_type= 1; t= _temp287; goto _LL272; _LL276:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp289);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp289);} last_loc= _temp289; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL272; _LL278: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp291);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp291);} last_loc= _temp291; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL272; _LL280: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp293);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp293);} last_loc= _temp293; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL272; _LL282: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp295);} if( seen_size){ void* _temp299= sz; _LL301: if( _temp299 == ( void*)
Cyc_Absyn_B4){ goto _LL302;} else{ goto _LL303;} _LL303: goto _LL304; _LL302: sz=(
void*) Cyc_Absyn_B8; goto _LL300; _LL304: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp295); goto _LL300; _LL300:;} last_loc=
_temp295; seen_size= 1; goto _LL272; _LL284: last_loc= _temp297->loc; if(
declopt ==  0? ! seen_type: 0){ seen_type= 1;{ void* _temp305=( void*) _temp297->r;
struct Cyc_Absyn_Structdecl* _temp317; struct Cyc_Absyn_Tuniondecl* _temp319;
struct Cyc_Absyn_Uniondecl* _temp321; struct Cyc_Absyn_Enumdecl* _temp323;
_LL307: if(*(( int*) _temp305) ==  Cyc_Absyn_Struct_d){ _LL318: _temp317=((
struct Cyc_Absyn_Struct_d_struct*) _temp305)->f1; goto _LL308;} else{ goto
_LL309;} _LL309: if(*(( int*) _temp305) ==  Cyc_Absyn_Tunion_d){ _LL320:
_temp319=(( struct Cyc_Absyn_Tunion_d_struct*) _temp305)->f1; goto _LL310;}
else{ goto _LL311;} _LL311: if(*(( int*) _temp305) ==  Cyc_Absyn_Union_d){
_LL322: _temp321=(( struct Cyc_Absyn_Union_d_struct*) _temp305)->f1; goto _LL312;}
else{ goto _LL313;} _LL313: if(*(( int*) _temp305) ==  Cyc_Absyn_Enum_d){ _LL324:
_temp323=(( struct Cyc_Absyn_Enum_d_struct*) _temp305)->f1; goto _LL314;} else{
goto _LL315;} _LL315: goto _LL316; _LL308: { struct Cyc_List_List* _temp325=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_copy_tvar,
_temp317->tvs)); t=( void*)({ struct Cyc_Absyn_StructType_struct* _temp326=(
struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp326[ 0]=({ struct Cyc_Absyn_StructType_struct _temp327; _temp327.tag= Cyc_Absyn_StructType;
_temp327.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp317->name))->v); _temp327.f2= _temp325; _temp327.f3= 0;
_temp327;}); _temp326;}); if( _temp317->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp328=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp328->v=( void*) _temp297; _temp328;});} goto _LL306;} _LL310: { struct Cyc_List_List*
_temp329=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_copy_tvar, _temp319->tvs)); t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp330=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp330[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp331; _temp331.tag= Cyc_Absyn_TunionType;
_temp331.f1=({ struct Cyc_Absyn_TunionInfo _temp332; _temp332.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp333=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp333[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp334; _temp334.tag= Cyc_Absyn_KnownTunion;
_temp334.f1=({ struct Cyc_Absyn_Tuniondecl** _temp335=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp335[ 0]= _temp319;
_temp335;}); _temp334;}); _temp333;})); _temp332.targs= _temp329; _temp332.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp332;}); _temp331;}); _temp330;}); if(
_temp319->fields !=  0){ declopt=({ struct Cyc_Core_Opt* _temp336=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp336->v=( void*) _temp297;
_temp336;});} goto _LL306;} _LL312: { struct Cyc_List_List* _temp337=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_copy_tvar,
_temp321->tvs)); t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp338=(
struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp338[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp339; _temp339.tag= Cyc_Absyn_UnionType;
_temp339.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp321->name))->v); _temp339.f2= _temp337; _temp339.f3= 0;
_temp339;}); _temp338;}); if( _temp321->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp340=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp340->v=( void*) _temp297; _temp340;});} goto _LL306;} _LL314: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp341=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp341[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp342; _temp342.tag= Cyc_Absyn_EnumType; _temp342.f1=
_temp323->name; _temp342.f2= 0; _temp342;}); _temp341;}); declopt=({ struct Cyc_Core_Opt*
_temp343=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp343->v=( void*) _temp297; _temp343;}); goto _LL306; _LL316:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp297->loc); goto _LL306; _LL306:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp297->loc);} goto _LL272; _LL272:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type within specifier", sizeof(
unsigned char), 30u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp344=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp344[ 0]=({ struct Cyc_Absyn_IntType_struct _temp345; _temp345.tag= Cyc_Absyn_IntType;
_temp345.f1=( void*) sgn; _temp345.f2=( void*) sz; _temp345;}); _temp344;});}
else{ if( seen_sign){ void* _temp346= t; void* _temp352; _LL348: if((
unsigned int) _temp346 >  3u?*(( int*) _temp346) ==  Cyc_Absyn_IntType: 0){
_LL353: _temp352=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp346)->f2;
goto _LL349;} else{ goto _LL350;} _LL350: goto _LL351; _LL349: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp354=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp354[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp355; _temp355.tag= Cyc_Absyn_IntType; _temp355.f1=( void*) sgn; _temp355.f2=(
void*) _temp352; _temp355;}); _temp354;}); goto _LL347; _LL351: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL347; _LL347:;} if( seen_size){ void* _temp356= t; void*
_temp364; _LL358: if(( unsigned int) _temp356 >  3u?*(( int*) _temp356) ==  Cyc_Absyn_IntType:
0){ _LL365: _temp364=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp356)->f1;
goto _LL359;} else{ goto _LL360;} _LL360: if(( unsigned int) _temp356 >  3u?*((
int*) _temp356) ==  Cyc_Absyn_DoubleType: 0){ goto _LL361;} else{ goto _LL362;}
_LL362: goto _LL363; _LL359: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp366=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp366[ 0]=({ struct Cyc_Absyn_IntType_struct _temp367; _temp367.tag= Cyc_Absyn_IntType;
_temp367.f1=( void*) _temp364; _temp367.f2=( void*) sz; _temp367;}); _temp366;});
goto _LL357; _LL361: t=( void*)({ struct Cyc_Absyn_DoubleType_struct* _temp368=(
struct Cyc_Absyn_DoubleType_struct*) _cycalloc_atomic( sizeof( struct Cyc_Absyn_DoubleType_struct));
_temp368[ 0]=({ struct Cyc_Absyn_DoubleType_struct _temp369; _temp369.tag= Cyc_Absyn_DoubleType;
_temp369.f1= 1; _temp369;}); _temp368;}); goto _LL357; _LL363: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL357; _LL357:;}} return({ struct _tuple5 _temp370; _temp370.f1=
t; _temp370.f2= declopt; _temp370;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds ==  0){ return 0;}{ struct Cyc_Parse_Declarator* _temp371=(
struct Cyc_Parse_Declarator*) ds->hd; struct _tuple1* _temp372= _temp371->id;
struct Cyc_List_List* _temp375; struct Cyc_List_List* _temp377; void* _temp379;
struct Cyc_Absyn_Tqual _temp381; struct _tuple6 _temp373= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp371->tms); _LL382: _temp381= _temp373.f1; goto _LL380;
_LL380: _temp379= _temp373.f2; goto _LL378; _LL378: _temp377= _temp373.f3; goto
_LL376; _LL376: _temp375= _temp373.f4; goto _LL374; _LL374: return({ struct Cyc_List_List*
_temp383=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp383->hd=( void*)({ struct _tuple7* _temp384=( struct _tuple7*) _cycalloc(
sizeof( struct _tuple7)); _temp384->f1= _temp372; _temp384->f2= _temp381;
_temp384->f3= _temp379; _temp384->f4= _temp377; _temp384->f5= _temp375; _temp384;});
_temp383->tl= Cyc_Parse_apply_tmss( _temp381, Cyc_Tcutil_copy_type( t), ds->tl,
shared_atts); _temp383;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms ==  0){ return({ struct _tuple6 _temp385; _temp385.f1= tq;
_temp385.f2= t; _temp385.f3= 0; _temp385.f4= atts; _temp385;});}{ void* _temp386=(
void*) tms->hd; struct Cyc_Absyn_Exp* _temp400; void* _temp402; struct Cyc_Position_Segment*
_temp404; struct Cyc_List_List* _temp406; struct Cyc_Absyn_Tqual _temp408; void*
_temp410; void* _temp412; struct Cyc_List_List* _temp414; struct Cyc_Position_Segment*
_temp416; _LL388: if( _temp386 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL389;}
else{ goto _LL390;} _LL390: if(( unsigned int) _temp386 >  1u?*(( int*) _temp386)
==  Cyc_Absyn_ConstArray_mod: 0){ _LL401: _temp400=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp386)->f1; goto _LL391;} else{ goto _LL392;} _LL392: if(( unsigned int)
_temp386 >  1u?*(( int*) _temp386) ==  Cyc_Absyn_Function_mod: 0){ _LL403:
_temp402=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp386)->f1; goto
_LL393;} else{ goto _LL394;} _LL394: if(( unsigned int) _temp386 >  1u?*(( int*)
_temp386) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL407: _temp406=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp386)->f1; goto _LL405; _LL405: _temp404=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp386)->f2; goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int)
_temp386 >  1u?*(( int*) _temp386) ==  Cyc_Absyn_Pointer_mod: 0){ _LL413:
_temp412=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp386)->f1; goto
_LL411; _LL411: _temp410=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp386)->f2; goto _LL409; _LL409: _temp408=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp386)->f3; goto _LL397;} else{ goto _LL398;} _LL398: if(( unsigned int)
_temp386 >  1u?*(( int*) _temp386) ==  Cyc_Absyn_Attributes_mod: 0){ _LL417:
_temp416=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp386)->f1; goto _LL415;
_LL415: _temp414=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp386)->f2; goto
_LL399;} else{ goto _LL387;} _LL389: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp418=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp418[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp419; _temp419.tag= Cyc_Absyn_ArrayType; _temp419.f1=(
void*) t; _temp419.f2= tq; _temp419.f3= 0; _temp419;}); _temp418;}), atts, tms->tl);
_LL391: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp420=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp420[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp421; _temp421.tag= Cyc_Absyn_ArrayType; _temp421.f1=(
void*) t; _temp421.f2= tq; _temp421.f3=( struct Cyc_Absyn_Exp*) _temp400;
_temp421;}); _temp420;}), atts, tms->tl); _LL393: { void* _temp422= _temp402;
struct Cyc_List_List* _temp428; struct Cyc_Core_Opt* _temp430; struct Cyc_Absyn_VarargInfo*
_temp432; int _temp434; struct Cyc_List_List* _temp436; struct Cyc_Position_Segment*
_temp438; _LL424: if(*(( int*) _temp422) ==  Cyc_Absyn_WithTypes){ _LL437:
_temp436=(( struct Cyc_Absyn_WithTypes_struct*) _temp422)->f1; goto _LL435;
_LL435: _temp434=(( struct Cyc_Absyn_WithTypes_struct*) _temp422)->f2; goto
_LL433; _LL433: _temp432=(( struct Cyc_Absyn_WithTypes_struct*) _temp422)->f3;
goto _LL431; _LL431: _temp430=(( struct Cyc_Absyn_WithTypes_struct*) _temp422)->f4;
goto _LL429; _LL429: _temp428=(( struct Cyc_Absyn_WithTypes_struct*) _temp422)->f5;
goto _LL425;} else{ goto _LL426;} _LL426: if(*(( int*) _temp422) ==  Cyc_Absyn_NoTypes){
_LL439: _temp438=(( struct Cyc_Absyn_NoTypes_struct*) _temp422)->f2; goto _LL427;}
else{ goto _LL423;} _LL425: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as !=  0; as= as->tl){ if( Cyc_Absyn_fntype_att(( void*) as->hd)){
fn_atts=({ struct Cyc_List_List* _temp440=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp440->hd=( void*)(( void*) as->hd); _temp440->tl=
fn_atts; _temp440;});} else{ new_atts=({ struct Cyc_List_List* _temp441=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp441->hd=( void*)((
void*) as->hd); _temp441->tl= new_atts; _temp441;});}}} if( tms->tl !=  0){ void*
_temp442=( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd; struct
Cyc_List_List* _temp448; _LL444: if(( unsigned int) _temp442 >  1u?*(( int*)
_temp442) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL449: _temp448=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp442)->f1; goto _LL445;} else{ goto _LL446;} _LL446: goto _LL447; _LL445:
typvars= _temp448; tms= tms->tl; goto _LL443; _LL447: goto _LL443; _LL443:;} if(((((
! _temp434? _temp432 ==  0: 0)? _temp436 !=  0: 0)? _temp436->tl ==  0: 0)?(*((
struct _tuple2*) _temp436->hd)).f1 ==  0: 0)?(*(( struct _tuple2*) _temp436->hd)).f3
== ( void*) Cyc_Absyn_VoidType: 0){ _temp436= 0;} t= Cyc_Parse_array2ptr( t);((
void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Parse_arg_array2ptr, _temp436); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),
Cyc_Absyn_function_typ( typvars, _temp430, t, _temp436, _temp434, _temp432,
_temp428, fn_atts), new_atts,(( struct Cyc_List_List*) _check_null( tms))->tl);}
_LL427:( int) _throw( Cyc_Parse_abort( _tag_arr("function declaration without parameter types",
sizeof( unsigned char), 45u), _temp438)); _LL423:;} _LL395: if( tms->tl ==  0){
return({ struct _tuple6 _temp450; _temp450.f1= tq; _temp450.f2= t; _temp450.f3=
_temp406; _temp450.f4= atts; _temp450;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp404)); _LL397: { void* _temp451= _temp412;
struct Cyc_Absyn_Exp* _temp459; struct Cyc_Absyn_Exp* _temp461; _LL453: if((
unsigned int) _temp451 >  1u?*(( int*) _temp451) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL460: _temp459=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp451)->f1; goto
_LL454;} else{ goto _LL455;} _LL455: if(( unsigned int) _temp451 >  1u?*(( int*)
_temp451) ==  Cyc_Absyn_Nullable_ps: 0){ _LL462: _temp461=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp451)->f1; goto _LL456;} else{ goto _LL457;} _LL457: if( _temp451 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL458;} else{ goto _LL452;} _LL454: return Cyc_Parse_apply_tms(
_temp408, Cyc_Absyn_atb_typ( t, _temp410, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp463=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp463[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp464; _temp464.tag= Cyc_Absyn_Upper_b;
_temp464.f1= _temp459; _temp464;}); _temp463;})), atts, tms->tl); _LL456: return
Cyc_Parse_apply_tms( _temp408, Cyc_Absyn_starb_typ( t, _temp410, tq,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp465=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp465[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp466; _temp466.tag= Cyc_Absyn_Upper_b; _temp466.f1= _temp461; _temp466;});
_temp465;})), atts, tms->tl); _LL458: return Cyc_Parse_apply_tms( _temp408, Cyc_Absyn_tagged_typ(
t, _temp410, tq), atts, tms->tl); _LL452:;} _LL399: return Cyc_Parse_apply_tms(
tq, t, Cyc_List_append( atts, _temp414), tms->tl); _LL387:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp469; void* _temp471; struct _tuple5 _temp467= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL472: _temp471= _temp467.f1; goto _LL470; _LL470: _temp469=
_temp467.f2; goto _LL468; _LL468: if( _temp469 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp471;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp473=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp473[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp474; _temp474.tag= Cyc_Absyn_Decl_s;
_temp474.f1= d; _temp474.f2= s; _temp474;}); _temp473;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc !=  0){ void* _temp475=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL477: if( _temp475 == (
void*) Cyc_Parse_Typedef_sc){ goto _LL478;} else{ goto _LL479;} _LL479: if(
_temp475 == ( void*) Cyc_Parse_Extern_sc){ goto _LL480;} else{ goto _LL481;}
_LL481: if( _temp475 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL482;} else{ goto
_LL483;} _LL483: if( _temp475 == ( void*) Cyc_Parse_Static_sc){ goto _LL484;}
else{ goto _LL485;} _LL485: if( _temp475 == ( void*) Cyc_Parse_Auto_sc){ goto
_LL486;} else{ goto _LL487;} _LL487: if( _temp475 == ( void*) Cyc_Parse_Register_sc){
goto _LL488;} else{ goto _LL489;} _LL489: if( _temp475 == ( void*) Cyc_Parse_Abstract_sc){
goto _LL490;} else{ goto _LL476;} _LL478: istypedef= 1; goto _LL476; _LL480: s=(
void*) Cyc_Absyn_Extern; goto _LL476; _LL482: s=( void*) Cyc_Absyn_ExternC; goto
_LL476; _LL484: s=( void*) Cyc_Absyn_Static; goto _LL476; _LL486: s=( void*) Cyc_Absyn_Public;
goto _LL476; _LL488: s=( void*) Cyc_Absyn_Public; goto _LL476; _LL490: s=( void*)
Cyc_Absyn_Abstract; goto _LL476; _LL476:;}{ struct Cyc_List_List* _temp493;
struct Cyc_List_List* _temp495; struct _tuple0 _temp491=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL496: _temp495= _temp491.f1;
goto _LL494; _LL494: _temp493= _temp491.f2; goto _LL492; _LL492: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp493; for( 0; es !=  0; es= es->tl){
if(( struct Cyc_Absyn_Exp*) es->hd !=  0){ exps_empty= 0; break;}}}{ struct
_tuple5 _temp497= Cyc_Parse_collapse_type_specifiers( tss, loc); if( _temp495 == 
0){ struct Cyc_Core_Opt* _temp500; void* _temp502; struct _tuple5 _temp498=
_temp497; _LL503: _temp502= _temp498.f1; goto _LL501; _LL501: _temp500= _temp498.f2;
goto _LL499; _LL499: if( _temp500 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
_temp500->v;{ void* _temp504=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp516;
struct Cyc_Absyn_Structdecl* _temp518; struct Cyc_Absyn_Uniondecl* _temp520;
struct Cyc_Absyn_Tuniondecl* _temp522; _LL506: if(*(( int*) _temp504) ==  Cyc_Absyn_Enum_d){
_LL517: _temp516=(( struct Cyc_Absyn_Enum_d_struct*) _temp504)->f1; goto _LL507;}
else{ goto _LL508;} _LL508: if(*(( int*) _temp504) ==  Cyc_Absyn_Struct_d){
_LL519: _temp518=(( struct Cyc_Absyn_Struct_d_struct*) _temp504)->f1; goto
_LL509;} else{ goto _LL510;} _LL510: if(*(( int*) _temp504) ==  Cyc_Absyn_Union_d){
_LL521: _temp520=(( struct Cyc_Absyn_Union_d_struct*) _temp504)->f1; goto _LL511;}
else{ goto _LL512;} _LL512: if(*(( int*) _temp504) ==  Cyc_Absyn_Tunion_d){
_LL523: _temp522=(( struct Cyc_Absyn_Tunion_d_struct*) _temp504)->f1; goto
_LL513;} else{ goto _LL514;} _LL514: goto _LL515; _LL507:( void*)( _temp516->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum",
sizeof( unsigned char), 23u), loc);} goto _LL505; _LL509:( void*)( _temp518->sc=(
void*) s); _temp518->attributes= atts; goto _LL505; _LL511:( void*)( _temp520->sc=(
void*) s); _temp520->attributes= atts; goto _LL505; _LL513:( void*)( _temp522->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL505; _LL515: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL505:;}
return({ struct Cyc_List_List* _temp524=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp524->hd=( void*) d; _temp524->tl= 0;
_temp524;});} else{ void* _temp525= _temp502; struct Cyc_List_List* _temp541;
struct _tuple1* _temp543; struct Cyc_Absyn_TunionInfo _temp545; void* _temp547;
struct Cyc_Absyn_Tuniondecl** _temp549; struct Cyc_Absyn_TunionInfo _temp551;
struct Cyc_List_List* _temp553; void* _temp555; struct Cyc_Absyn_UnknownTunionInfo
_temp557; int _temp559; struct _tuple1* _temp561; struct Cyc_List_List* _temp563;
struct _tuple1* _temp565; struct _tuple1* _temp567; struct Cyc_List_List*
_temp569; _LL527: if(( unsigned int) _temp525 >  3u?*(( int*) _temp525) ==  Cyc_Absyn_StructType:
0){ _LL544: _temp543=(( struct Cyc_Absyn_StructType_struct*) _temp525)->f1; goto
_LL542; _LL542: _temp541=(( struct Cyc_Absyn_StructType_struct*) _temp525)->f2;
goto _LL528;} else{ goto _LL529;} _LL529: if(( unsigned int) _temp525 >  3u?*((
int*) _temp525) ==  Cyc_Absyn_TunionType: 0){ _LL546: _temp545=(( struct Cyc_Absyn_TunionType_struct*)
_temp525)->f1; _LL548: _temp547=( void*) _temp545.tunion_info; if(*(( int*)
_temp547) ==  Cyc_Absyn_KnownTunion){ _LL550: _temp549=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp547)->f1; goto _LL530;} else{ goto _LL531;}} else{ goto _LL531;} _LL531:
if(( unsigned int) _temp525 >  3u?*(( int*) _temp525) ==  Cyc_Absyn_TunionType:
0){ _LL552: _temp551=(( struct Cyc_Absyn_TunionType_struct*) _temp525)->f1;
_LL556: _temp555=( void*) _temp551.tunion_info; if(*(( int*) _temp555) ==  Cyc_Absyn_UnknownTunion){
_LL558: _temp557=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp555)->f1;
_LL562: _temp561= _temp557.name; goto _LL560; _LL560: _temp559= _temp557.is_xtunion;
goto _LL554;} else{ goto _LL533;} _LL554: _temp553= _temp551.targs; goto _LL532;}
else{ goto _LL533;} _LL533: if(( unsigned int) _temp525 >  3u?*(( int*) _temp525)
==  Cyc_Absyn_UnionType: 0){ _LL566: _temp565=(( struct Cyc_Absyn_UnionType_struct*)
_temp525)->f1; goto _LL564; _LL564: _temp563=(( struct Cyc_Absyn_UnionType_struct*)
_temp525)->f2; goto _LL534;} else{ goto _LL535;} _LL535: if(( unsigned int)
_temp525 >  3u?*(( int*) _temp525) ==  Cyc_Absyn_EnumType: 0){ _LL568: _temp567=((
struct Cyc_Absyn_EnumType_struct*) _temp525)->f1; goto _LL536;} else{ goto
_LL537;} _LL537: if(( unsigned int) _temp525 >  3u?*(( int*) _temp525) ==  Cyc_Absyn_AnonEnumType:
0){ _LL570: _temp569=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp525)->f1;
goto _LL538;} else{ goto _LL539;} _LL539: goto _LL540; _LL528: { struct Cyc_List_List*
_temp571=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp541); struct Cyc_Absyn_Structdecl* _temp572=({
struct Cyc_Absyn_Structdecl* _temp576=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp576->sc=( void*) s; _temp576->name=({
struct Cyc_Core_Opt* _temp577=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp577->v=( void*)(( struct _tuple1*) _check_null( _temp543));
_temp577;}); _temp576->tvs= _temp571; _temp576->fields= 0; _temp576->attributes=
0; _temp576;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp573=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp573->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp574=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp574[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp575; _temp575.tag= Cyc_Absyn_Struct_d;
_temp575.f1= _temp572; _temp575;}); _temp574;}), loc); _temp573->tl= 0; _temp573;});}
_LL530: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp578=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp578->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp579=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp579[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp580; _temp580.tag= Cyc_Absyn_Tunion_d;
_temp580.f1=* _temp549; _temp580;}); _temp579;}), loc); _temp578->tl= 0;
_temp578;}); _LL532: { struct Cyc_List_List* _temp581=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp553); struct Cyc_Absyn_Decl* _temp582= Cyc_Absyn_tunion_decl( s, _temp561,
_temp581, 0, _temp559, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp583=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp583->hd=(
void*) _temp582; _temp583->tl= 0; _temp583;});} _LL534: { struct Cyc_List_List*
_temp584=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp563); struct Cyc_Absyn_Uniondecl* _temp585=({
struct Cyc_Absyn_Uniondecl* _temp590=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp590->sc=( void*) s; _temp590->name=({
struct Cyc_Core_Opt* _temp591=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp591->v=( void*)(( struct _tuple1*) _check_null( _temp565));
_temp591;}); _temp590->tvs= _temp584; _temp590->fields= 0; _temp590->attributes=
0; _temp590;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp586=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp586->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp587=( struct Cyc_Absyn_Decl*) _cycalloc(
sizeof( struct Cyc_Absyn_Decl)); _temp587->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp588=( struct Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp588[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp589; _temp589.tag= Cyc_Absyn_Union_d;
_temp589.f1= _temp585; _temp589;}); _temp588;})); _temp587->loc= loc; _temp587;});
_temp586->tl= 0; _temp586;});} _LL536: { struct Cyc_Absyn_Enumdecl* _temp592=({
struct Cyc_Absyn_Enumdecl* _temp597=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp597->sc=( void*) s; _temp597->name=
_temp567; _temp597->fields= 0; _temp597;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp593=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp593->hd=( void*)({ struct Cyc_Absyn_Decl* _temp594=(
struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp594->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp595=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp595[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp596; _temp596.tag= Cyc_Absyn_Enum_d; _temp596.f1= _temp592; _temp596;});
_temp595;})); _temp594->loc= loc; _temp594;}); _temp593->tl= 0; _temp593;});}
_LL538: { struct Cyc_Absyn_Enumdecl* _temp598=({ struct Cyc_Absyn_Enumdecl*
_temp603=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp603->sc=( void*) s; _temp603->name= Cyc_Parse_gensym_enum(); _temp603->fields=({
struct Cyc_Core_Opt* _temp604=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp604->v=( void*) _temp569; _temp604;}); _temp603;}); if(
atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} return({ struct Cyc_List_List* _temp599=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp599->hd=( void*)({
struct Cyc_Absyn_Decl* _temp600=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp600->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp601=( struct Cyc_Absyn_Enum_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp601[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp602; _temp602.tag= Cyc_Absyn_Enum_d;
_temp602.f1= _temp598; _temp602;}); _temp601;})); _temp600->loc= loc; _temp600;});
_temp599->tl= 0; _temp599;});} _LL540: Cyc_Parse_err( _tag_arr("missing declarator",
sizeof( unsigned char), 19u), loc); return 0; _LL526:;}} else{ void* t= _temp497.f1;
struct Cyc_List_List* _temp605= Cyc_Parse_apply_tmss( tq, t, _temp495, atts);
if( istypedef){ if( ! exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp605); if( _temp497.f2 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(
_temp497.f2)->v;{ void* _temp606=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp618; struct Cyc_Absyn_Tuniondecl* _temp620; struct Cyc_Absyn_Uniondecl*
_temp622; struct Cyc_Absyn_Enumdecl* _temp624; _LL608: if(*(( int*) _temp606) == 
Cyc_Absyn_Struct_d){ _LL619: _temp618=(( struct Cyc_Absyn_Struct_d_struct*)
_temp606)->f1; goto _LL609;} else{ goto _LL610;} _LL610: if(*(( int*) _temp606)
==  Cyc_Absyn_Tunion_d){ _LL621: _temp620=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp606)->f1; goto _LL611;} else{ goto _LL612;} _LL612: if(*(( int*) _temp606)
==  Cyc_Absyn_Union_d){ _LL623: _temp622=(( struct Cyc_Absyn_Union_d_struct*)
_temp606)->f1; goto _LL613;} else{ goto _LL614;} _LL614: if(*(( int*) _temp606)
==  Cyc_Absyn_Enum_d){ _LL625: _temp624=(( struct Cyc_Absyn_Enum_d_struct*)
_temp606)->f1; goto _LL615;} else{ goto _LL616;} _LL616: goto _LL617; _LL609:(
void*)( _temp618->sc=( void*) s); _temp618->attributes= atts; atts= 0; goto
_LL607; _LL611:( void*)( _temp620->sc=( void*) s); goto _LL607; _LL613:( void*)(
_temp622->sc=( void*) s); goto _LL607; _LL615:( void*)( _temp624->sc=( void*) s);
goto _LL607; _LL617: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL607; _LL607:;} decls=({ struct Cyc_List_List*
_temp626=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp626->hd=( void*) d; _temp626->tl= decls; _temp626;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp628; _temp628.tag=
Cyc_Std_String_pa; _temp628.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) atts->hd);{ void* _temp627[ 1u]={& _temp628}; Cyc_Std_aprintf( _tag_arr("bad attribute %s in typedef",
sizeof( unsigned char), 28u), _tag_arr( _temp627, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp497.f2 !=  0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp629= _temp605; for( 0; _temp629 !=  0;( _temp629= _temp629->tl,
_temp493= _temp493->tl)){ struct _tuple7 _temp632; struct Cyc_List_List*
_temp633; struct Cyc_List_List* _temp635; void* _temp637; struct Cyc_Absyn_Tqual
_temp639; struct _tuple1* _temp641; struct _tuple7* _temp630=( struct _tuple7*)
_temp629->hd; _temp632=* _temp630; _LL642: _temp641= _temp632.f1; goto _LL640;
_LL640: _temp639= _temp632.f2; goto _LL638; _LL638: _temp637= _temp632.f3; goto
_LL636; _LL636: _temp635= _temp632.f4; goto _LL634; _LL634: _temp633= _temp632.f5;
goto _LL631; _LL631: if( _temp635 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), loc);} if( _temp493 ==  0){(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("unexpected NULL in parse!",
sizeof( unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp643=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp493))->hd; struct Cyc_Absyn_Vardecl*
_temp644= Cyc_Absyn_new_vardecl( _temp641, _temp637, _temp643); _temp644->tq=
_temp639;( void*)( _temp644->sc=( void*) s); _temp644->attributes= _temp633;{
struct Cyc_Absyn_Decl* _temp645=({ struct Cyc_Absyn_Decl* _temp647=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp647->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp648=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp648[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp649; _temp649.tag= Cyc_Absyn_Var_d; _temp649.f1= _temp644; _temp649;});
_temp648;})); _temp647->loc= loc; _temp647;}); decls=({ struct Cyc_List_List*
_temp646=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp646->hd=( void*) _temp645; _temp646->tl= decls; _temp646;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_Std_strlen( s) !=  1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp651; _temp651.tag= Cyc_Std_String_pa;
_temp651.f1=( struct _tagged_arr) s;{ void* _temp650[ 1u]={& _temp651}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp650,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL652: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL653: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL654: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL655: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL656: return(
void*) Cyc_Absyn_EffKind; default: _LL657: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp660; _temp660.tag= Cyc_Std_String_pa;
_temp660.f1=( struct _tagged_arr) s;{ void* _temp659[ 1u]={& _temp660}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp659,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts ==  0){ return
tms;} else{ return({ struct Cyc_List_List* _temp661=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp661->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp662=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp662[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp663; _temp663.tag= Cyc_Absyn_Attributes_mod;
_temp663.f1= loc; _temp663.f2= atts; _temp663;}); _temp662;})); _temp661->tl=
tms; _temp661;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp666;
struct Cyc_List_List* _temp667; struct Cyc_List_List* _temp669; void* _temp671;
struct Cyc_Absyn_Tqual _temp673; struct _tuple1* _temp675; struct _tuple7*
_temp664= t; _temp666=* _temp664; _LL676: _temp675= _temp666.f1; goto _LL674;
_LL674: _temp673= _temp666.f2; goto _LL672; _LL672: _temp671= _temp666.f3; goto
_LL670; _LL670: _temp669= _temp666.f4; goto _LL668; _LL668: _temp667= _temp666.f5;
goto _LL665; _LL665: Cyc_Lex_register_typedef( _temp675); if( _temp667 !=  0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp678;
_temp678.tag= Cyc_Std_String_pa; _temp678.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) _temp667->hd);{ void* _temp677[ 1u]={& _temp678}; Cyc_Std_aprintf(
_tag_arr("bad attribute %s within typedef", sizeof( unsigned char), 32u),
_tag_arr( _temp677, sizeof( void*), 1u));}}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp679=( struct Cyc_Absyn_Typedef_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp679[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp680; _temp680.tag= Cyc_Absyn_Typedef_d; _temp680.f1=({
struct Cyc_Absyn_Typedefdecl* _temp681=( struct Cyc_Absyn_Typedefdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp681->name= _temp675;
_temp681->tvs= _temp669; _temp681->defn=( void*) _temp671; _temp681;}); _temp680;});
_temp679;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
unsigned char _temp683[ 15u]="$(sign_t,int)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp683, _temp683, _temp683 +  15u}};
struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ struct _tuple15* yyzzz;{ void*
_temp684= yy1; struct _tuple15* _temp690; _LL686: if(*(( void**) _temp684) == 
Cyc_Int_tok){ _LL691: _temp690=(( struct Cyc_Int_tok_struct*) _temp684)->f1;
goto _LL687;} else{ goto _LL688;} _LL688: goto _LL689; _LL687: yyzzz= _temp690;
goto _LL685; _LL689:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL685:;}
return yyzzz;} static unsigned char _temp693[ 9u]="string_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp693, _temp693, _temp693 +  9u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ struct _tagged_arr yyzzz;{
void* _temp694= yy1; struct _tagged_arr _temp700; _LL696: if(*(( void**)
_temp694) ==  Cyc_String_tok){ _LL701: _temp700=(( struct Cyc_String_tok_struct*)
_temp694)->f1; goto _LL697;} else{ goto _LL698;} _LL698: goto _LL699; _LL697:
yyzzz= _temp700; goto _LL695; _LL699:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL695:;} return yyzzz;} static unsigned char _temp703[ 5u]="char"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{ _temp703,
_temp703, _temp703 +  5u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
unsigned char yyzzz;{ void* _temp704= yy1; unsigned char _temp710; _LL706: if(*((
void**) _temp704) ==  Cyc_Char_tok){ _LL711: _temp710=(( struct Cyc_Char_tok_struct*)
_temp704)->f1; goto _LL707;} else{ goto _LL708;} _LL708: goto _LL709; _LL707:
yyzzz= _temp710; goto _LL705; _LL709:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL705:;} return yyzzz;} static unsigned char _temp713[ 20u]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{
_temp713, _temp713, _temp713 +  20u}}; void* Cyc_yyget_Pointer_Sort_tok( void*
yy1){ void* yyzzz;{ void* _temp714= yy1; void* _temp720; _LL716: if(*(( void**)
_temp714) ==  Cyc_Pointer_Sort_tok){ _LL721: _temp720=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp714)->f1; goto _LL717;} else{ goto _LL718;} _LL718: goto _LL719; _LL717:
yyzzz= _temp720; goto _LL715; _LL719:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL715:;} return yyzzz;} static unsigned char _temp723[ 6u]="exp_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp723,
_temp723, _temp723 +  6u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp724= yy1; struct Cyc_Absyn_Exp*
_temp730; _LL726: if(*(( void**) _temp724) ==  Cyc_Exp_tok){ _LL731: _temp730=((
struct Cyc_Exp_tok_struct*) _temp724)->f1; goto _LL727;} else{ goto _LL728;}
_LL728: goto _LL729; _LL727: yyzzz= _temp730; goto _LL725; _LL729:( int) _throw((
void*)& Cyc_yyfail_Exp_tok); _LL725:;} return yyzzz;} static unsigned char
_temp733[ 14u]="list_t<exp_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{ _temp733, _temp733, _temp733 +  14u}}; struct Cyc_List_List*
Cyc_yyget_ExpList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp734=
yy1; struct Cyc_List_List* _temp740; _LL736: if(*(( void**) _temp734) ==  Cyc_ExpList_tok){
_LL741: _temp740=(( struct Cyc_ExpList_tok_struct*) _temp734)->f1; goto _LL737;}
else{ goto _LL738;} _LL738: goto _LL739; _LL737: yyzzz= _temp740; goto _LL735;
_LL739:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL735:;} return yyzzz;}
static unsigned char _temp743[ 39u]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{
_temp743, _temp743, _temp743 +  39u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp744= yy1; struct Cyc_List_List*
_temp750; _LL746: if(*(( void**) _temp744) ==  Cyc_InitializerList_tok){ _LL751:
_temp750=(( struct Cyc_InitializerList_tok_struct*) _temp744)->f1; goto _LL747;}
else{ goto _LL748;} _LL748: goto _LL749; _LL747: yyzzz= _temp750; goto _LL745;
_LL749:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL745:;} return
yyzzz;} static unsigned char _temp753[ 9u]="primop_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp753, _temp753, _temp753 +  9u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* yyzzz;{ void* _temp754= yy1; void*
_temp760; _LL756: if(*(( void**) _temp754) ==  Cyc_Primop_tok){ _LL761: _temp760=(
void*)(( struct Cyc_Primop_tok_struct*) _temp754)->f1; goto _LL757;} else{ goto
_LL758;} _LL758: goto _LL759; _LL757: yyzzz= _temp760; goto _LL755; _LL759:( int)
_throw(( void*)& Cyc_yyfail_Primop_tok); _LL755:;} return yyzzz;} static
unsigned char _temp763[ 16u]="opt_t<primop_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp763, _temp763, _temp763 +  16u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ struct Cyc_Core_Opt*
yyzzz;{ void* _temp764= yy1; struct Cyc_Core_Opt* _temp770; _LL766: if(*(( void**)
_temp764) ==  Cyc_Primopopt_tok){ _LL771: _temp770=(( struct Cyc_Primopopt_tok_struct*)
_temp764)->f1; goto _LL767;} else{ goto _LL768;} _LL768: goto _LL769; _LL767:
yyzzz= _temp770; goto _LL765; _LL769:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL765:;} return yyzzz;} static unsigned char _temp773[ 7u]="qvar_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{
_temp773, _temp773, _temp773 +  7u}}; struct _tuple1* Cyc_yyget_QualId_tok( void*
yy1){ struct _tuple1* yyzzz;{ void* _temp774= yy1; struct _tuple1* _temp780;
_LL776: if(*(( void**) _temp774) ==  Cyc_QualId_tok){ _LL781: _temp780=(( struct
Cyc_QualId_tok_struct*) _temp774)->f1; goto _LL777;} else{ goto _LL778;} _LL778:
goto _LL779; _LL777: yyzzz= _temp780; goto _LL775; _LL779:( int) _throw(( void*)&
Cyc_yyfail_QualId_tok); _LL775:;} return yyzzz;} static unsigned char _temp783[
7u]="stmt_t"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{
_temp783, _temp783, _temp783 +  7u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok(
void* yy1){ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp784= yy1; struct Cyc_Absyn_Stmt*
_temp790; _LL786: if(*(( void**) _temp784) ==  Cyc_Stmt_tok){ _LL791: _temp790=((
struct Cyc_Stmt_tok_struct*) _temp784)->f1; goto _LL787;} else{ goto _LL788;}
_LL788: goto _LL789; _LL787: yyzzz= _temp790; goto _LL785; _LL789:( int) _throw((
void*)& Cyc_yyfail_Stmt_tok); _LL785:;} return yyzzz;} static unsigned char
_temp793[ 24u]="list_t<switch_clause_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp793, _temp793,
_temp793 +  24u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ struct Cyc_List_List* yyzzz;{ void* _temp794= yy1; struct Cyc_List_List*
_temp800; _LL796: if(*(( void**) _temp794) ==  Cyc_SwitchClauseList_tok){ _LL801:
_temp800=(( struct Cyc_SwitchClauseList_tok_struct*) _temp794)->f1; goto _LL797;}
else{ goto _LL798;} _LL798: goto _LL799; _LL797: yyzzz= _temp800; goto _LL795;
_LL799:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL795:;}
return yyzzz;} static unsigned char _temp803[ 25u]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp803, _temp803, _temp803 +  25u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp804= yy1; struct Cyc_List_List*
_temp810; _LL806: if(*(( void**) _temp804) ==  Cyc_SwitchCClauseList_tok){
_LL811: _temp810=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp804)->f1;
goto _LL807;} else{ goto _LL808;} _LL808: goto _LL809; _LL807: yyzzz= _temp810;
goto _LL805; _LL809:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL805:;} return yyzzz;} static unsigned char _temp813[ 6u]="pat_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{
_temp813, _temp813, _temp813 +  6u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
void* yy1){ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp814= yy1; struct Cyc_Absyn_Pat*
_temp820; _LL816: if(*(( void**) _temp814) ==  Cyc_Pattern_tok){ _LL821:
_temp820=(( struct Cyc_Pattern_tok_struct*) _temp814)->f1; goto _LL817;} else{
goto _LL818;} _LL818: goto _LL819; _LL817: yyzzz= _temp820; goto _LL815; _LL819:(
int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL815:;} return yyzzz;} static
unsigned char _temp823[ 14u]="list_t<pat_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp823, _temp823, _temp823 + 
14u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp824= yy1; struct Cyc_List_List* _temp830; _LL826: if(*(( void**)
_temp824) ==  Cyc_PatternList_tok){ _LL831: _temp830=(( struct Cyc_PatternList_tok_struct*)
_temp824)->f1; goto _LL827;} else{ goto _LL828;} _LL828: goto _LL829; _LL827:
yyzzz= _temp830; goto _LL825; _LL829:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL825:;} return yyzzz;} static unsigned char _temp833[ 31u]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp833, _temp833, _temp833 +  31u}}; struct _tuple13* Cyc_yyget_FieldPattern_tok(
void* yy1){ struct _tuple13* yyzzz;{ void* _temp834= yy1; struct _tuple13*
_temp840; _LL836: if(*(( void**) _temp834) ==  Cyc_FieldPattern_tok){ _LL841:
_temp840=(( struct Cyc_FieldPattern_tok_struct*) _temp834)->f1; goto _LL837;}
else{ goto _LL838;} _LL838: goto _LL839; _LL837: yyzzz= _temp840; goto _LL835;
_LL839:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL835:;} return
yyzzz;} static unsigned char _temp843[ 39u]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp843, _temp843, _temp843 +  39u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp844= yy1; struct Cyc_List_List*
_temp850; _LL846: if(*(( void**) _temp844) ==  Cyc_FieldPatternList_tok){ _LL851:
_temp850=(( struct Cyc_FieldPatternList_tok_struct*) _temp844)->f1; goto _LL847;}
else{ goto _LL848;} _LL848: goto _LL849; _LL847: yyzzz= _temp850; goto _LL845;
_LL849:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL845:;}
return yyzzz;} static unsigned char _temp853[ 9u]="fndecl_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp853, _temp853, _temp853 +  9u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ struct Cyc_Absyn_Fndecl*
yyzzz;{ void* _temp854= yy1; struct Cyc_Absyn_Fndecl* _temp860; _LL856: if(*((
void**) _temp854) ==  Cyc_FnDecl_tok){ _LL861: _temp860=(( struct Cyc_FnDecl_tok_struct*)
_temp854)->f1; goto _LL857;} else{ goto _LL858;} _LL858: goto _LL859; _LL857:
yyzzz= _temp860; goto _LL855; _LL859:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL855:;} return yyzzz;} static unsigned char _temp863[ 15u]="list_t<decl_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp863, _temp863, _temp863 +  15u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp864= yy1; struct Cyc_List_List*
_temp870; _LL866: if(*(( void**) _temp864) ==  Cyc_DeclList_tok){ _LL871:
_temp870=(( struct Cyc_DeclList_tok_struct*) _temp864)->f1; goto _LL867;} else{
goto _LL868;} _LL868: goto _LL869; _LL867: yyzzz= _temp870; goto _LL865; _LL869:(
int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL865:;} return yyzzz;} static
unsigned char _temp873[ 12u]="decl_spec_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp873, _temp873, _temp873 +  12u}};
struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){ struct
Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp874= yy1; struct Cyc_Parse_Declaration_spec*
_temp880; _LL876: if(*(( void**) _temp874) ==  Cyc_DeclSpec_tok){ _LL881:
_temp880=(( struct Cyc_DeclSpec_tok_struct*) _temp874)->f1; goto _LL877;} else{
goto _LL878;} _LL878: goto _LL879; _LL877: yyzzz= _temp880; goto _LL875; _LL879:(
int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL875:;} return yyzzz;} static
unsigned char _temp883[ 27u]="$(declarator_t,exp_opt_t)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp883, _temp883, _temp883 +  27u}};
struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){ struct _tuple14* yyzzz;{
void* _temp884= yy1; struct _tuple14* _temp890; _LL886: if(*(( void**) _temp884)
==  Cyc_InitDecl_tok){ _LL891: _temp890=(( struct Cyc_InitDecl_tok_struct*)
_temp884)->f1; goto _LL887;} else{ goto _LL888;} _LL888: goto _LL889; _LL887:
yyzzz= _temp890; goto _LL885; _LL889:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL885:;} return yyzzz;} static unsigned char _temp893[ 35u]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp893, _temp893, _temp893 +  35u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp894= yy1; struct Cyc_List_List*
_temp900; _LL896: if(*(( void**) _temp894) ==  Cyc_InitDeclList_tok){ _LL901:
_temp900=(( struct Cyc_InitDeclList_tok_struct*) _temp894)->f1; goto _LL897;}
else{ goto _LL898;} _LL898: goto _LL899; _LL897: yyzzz= _temp900; goto _LL895;
_LL899:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL895:;} return
yyzzz;} static unsigned char _temp903[ 16u]="storage_class_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp903, _temp903, _temp903 + 
16u}}; void* Cyc_yyget_StorageClass_tok( void* yy1){ void* yyzzz;{ void*
_temp904= yy1; void* _temp910; _LL906: if(*(( void**) _temp904) ==  Cyc_StorageClass_tok){
_LL911: _temp910=( void*)(( struct Cyc_StorageClass_tok_struct*) _temp904)->f1;
goto _LL907;} else{ goto _LL908;} _LL908: goto _LL909; _LL907: yyzzz= _temp910;
goto _LL905; _LL909:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL905:;}
return yyzzz;} static unsigned char _temp913[ 17u]="type_specifier_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp913, _temp913, _temp913 +  17u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* yyzzz;{ void* _temp914= yy1; void* _temp920; _LL916: if(*(( void**)
_temp914) ==  Cyc_TypeSpecifier_tok){ _LL921: _temp920=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp914)->f1; goto _LL917;} else{ goto _LL918;} _LL918: goto _LL919; _LL917:
yyzzz= _temp920; goto _LL915; _LL919:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL915:;} return yyzzz;} static unsigned char _temp923[ 18u]="struct_or_union_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp923, _temp923, _temp923 +  18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ enum  Cyc_Parse_Struct_or_union yyzzz;{ void* _temp924= yy1; enum 
Cyc_Parse_Struct_or_union _temp930; _LL926: if(*(( void**) _temp924) ==  Cyc_StructOrUnion_tok){
_LL931: _temp930=(( struct Cyc_StructOrUnion_tok_struct*) _temp924)->f1; goto
_LL927;} else{ goto _LL928;} _LL928: goto _LL929; _LL927: yyzzz= _temp930; goto
_LL925; _LL929:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok); _LL925:;}
return yyzzz;} static unsigned char _temp933[ 8u]="tqual_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp933, _temp933, _temp933 +  8u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp934= yy1; struct Cyc_Absyn_Tqual _temp940; _LL936: if(*((
void**) _temp934) ==  Cyc_TypeQual_tok){ _LL941: _temp940=(( struct Cyc_TypeQual_tok_struct*)
_temp934)->f1; goto _LL937;} else{ goto _LL938;} _LL938: goto _LL939; _LL937:
yyzzz= _temp940; goto _LL935; _LL939:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL935:;} return yyzzz;} static unsigned char _temp943[ 22u]="list_t<structfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp943, _temp943, _temp943 +  22u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp944= yy1; struct Cyc_List_List*
_temp950; _LL946: if(*(( void**) _temp944) ==  Cyc_StructFieldDeclList_tok){
_LL951: _temp950=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp944)->f1;
goto _LL947;} else{ goto _LL948;} _LL948: goto _LL949; _LL947: yyzzz= _temp950;
goto _LL945; _LL949:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL945:;} return yyzzz;} static unsigned char _temp953[ 30u]="list_t<list_t<structfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp953, _temp953, _temp953 +  30u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{
void* _temp954= yy1; struct Cyc_List_List* _temp960; _LL956: if(*(( void**)
_temp954) ==  Cyc_StructFieldDeclListList_tok){ _LL961: _temp960=(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp954)->f1; goto _LL957;} else{ goto _LL958;} _LL958: goto _LL959; _LL957:
yyzzz= _temp960; goto _LL955; _LL959:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL955:;} return yyzzz;} static unsigned char _temp963[ 24u]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{
_temp963, _temp963, _temp963 +  24u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp964= yy1; struct Cyc_List_List*
_temp970; _LL966: if(*(( void**) _temp964) ==  Cyc_TypeModifierList_tok){ _LL971:
_temp970=(( struct Cyc_TypeModifierList_tok_struct*) _temp964)->f1; goto _LL967;}
else{ goto _LL968;} _LL968: goto _LL969; _LL967: yyzzz= _temp970; goto _LL965;
_LL969:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL965:;}
return yyzzz;} static unsigned char _temp973[ 13u]="declarator_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp973,
_temp973, _temp973 +  13u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp974= yy1; struct Cyc_Parse_Declarator*
_temp980; _LL976: if(*(( void**) _temp974) ==  Cyc_Declarator_tok){ _LL981:
_temp980=(( struct Cyc_Declarator_tok_struct*) _temp974)->f1; goto _LL977;}
else{ goto _LL978;} _LL978: goto _LL979; _LL977: yyzzz= _temp980; goto _LL975;
_LL979:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL975:;} return
yyzzz;} static unsigned char _temp983[ 21u]="abstractdeclarator_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{
_temp983, _temp983, _temp983 +  21u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp984= yy1;
struct Cyc_Parse_Abstractdeclarator* _temp990; _LL986: if(*(( void**) _temp984)
==  Cyc_AbstractDeclarator_tok){ _LL991: _temp990=(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp984)->f1; goto _LL987;} else{ goto _LL988;} _LL988: goto _LL989; _LL987:
yyzzz= _temp990; goto _LL985; _LL989:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL985:;} return yyzzz;} static unsigned char _temp993[ 5u]="bool"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp993,
_temp993, _temp993 +  5u}}; int Cyc_yyget_Bool_tok( void* yy1){ int yyzzz;{ void*
_temp994= yy1; int _temp1000; _LL996: if(*(( void**) _temp994) ==  Cyc_Bool_tok){
_LL1001: _temp1000=(( struct Cyc_Bool_tok_struct*) _temp994)->f1; goto _LL997;}
else{ goto _LL998;} _LL998: goto _LL999; _LL997: yyzzz= _temp1000; goto _LL995;
_LL999:( int) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL995:;} return yyzzz;}
static unsigned char _temp1003[ 8u]="scope_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1003, _temp1003, _temp1003 +  8u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* yyzzz;{ void* _temp1004= yy1; void*
_temp1010; _LL1006: if(*(( void**) _temp1004) ==  Cyc_Scope_tok){ _LL1011:
_temp1010=( void*)(( struct Cyc_Scope_tok_struct*) _temp1004)->f1; goto _LL1007;}
else{ goto _LL1008;} _LL1008: goto _LL1009; _LL1007: yyzzz= _temp1010; goto
_LL1005; _LL1009:( int) _throw(( void*)& Cyc_yyfail_Scope_tok); _LL1005:;}
return yyzzz;} static unsigned char _temp1013[ 14u]="tunionfield_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1013, _temp1013, _temp1013 +  14u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp1014= yy1; struct
Cyc_Absyn_Tunionfield* _temp1020; _LL1016: if(*(( void**) _temp1014) ==  Cyc_TunionField_tok){
_LL1021: _temp1020=(( struct Cyc_TunionField_tok_struct*) _temp1014)->f1; goto
_LL1017;} else{ goto _LL1018;} _LL1018: goto _LL1019; _LL1017: yyzzz= _temp1020;
goto _LL1015; _LL1019:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL1015:;} return yyzzz;} static unsigned char _temp1023[ 22u]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp1023, _temp1023, _temp1023 +  22u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1024= yy1; struct Cyc_List_List*
_temp1030; _LL1026: if(*(( void**) _temp1024) ==  Cyc_TunionFieldList_tok){
_LL1031: _temp1030=(( struct Cyc_TunionFieldList_tok_struct*) _temp1024)->f1;
goto _LL1027;} else{ goto _LL1028;} _LL1028: goto _LL1029; _LL1027: yyzzz=
_temp1030; goto _LL1025; _LL1029:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1025:;} return yyzzz;} static unsigned char _temp1033[ 50u]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1033, _temp1033, _temp1033 +  50u}}; struct _tuple17* Cyc_yyget_QualSpecList_tok(
void* yy1){ struct _tuple17* yyzzz;{ void* _temp1034= yy1; struct _tuple17*
_temp1040; _LL1036: if(*(( void**) _temp1034) ==  Cyc_QualSpecList_tok){ _LL1041:
_temp1040=(( struct Cyc_QualSpecList_tok_struct*) _temp1034)->f1; goto _LL1037;}
else{ goto _LL1038;} _LL1038: goto _LL1039; _LL1037: yyzzz= _temp1040; goto
_LL1035; _LL1039:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL1035:;}
return yyzzz;} static unsigned char _temp1043[ 14u]="list_t<var_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{
_temp1043, _temp1043, _temp1043 +  14u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1044= yy1; struct Cyc_List_List*
_temp1050; _LL1046: if(*(( void**) _temp1044) ==  Cyc_IdList_tok){ _LL1051:
_temp1050=(( struct Cyc_IdList_tok_struct*) _temp1044)->f1; goto _LL1047;} else{
goto _LL1048;} _LL1048: goto _LL1049; _LL1047: yyzzz= _temp1050; goto _LL1045;
_LL1049:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1045:;} return yyzzz;}
static unsigned char _temp1053[ 32u]="$(opt_t<var_t>,tqual_t,type_t)@"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{
_temp1053, _temp1053, _temp1053 +  32u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ struct _tuple2* yyzzz;{ void* _temp1054= yy1; struct _tuple2*
_temp1060; _LL1056: if(*(( void**) _temp1054) ==  Cyc_ParamDecl_tok){ _LL1061:
_temp1060=(( struct Cyc_ParamDecl_tok_struct*) _temp1054)->f1; goto _LL1057;}
else{ goto _LL1058;} _LL1058: goto _LL1059; _LL1057: yyzzz= _temp1060; goto
_LL1055; _LL1059:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1055:;}
return yyzzz;} static unsigned char _temp1063[ 40u]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{
_temp1063, _temp1063, _temp1063 +  40u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1064= yy1; struct Cyc_List_List*
_temp1070; _LL1066: if(*(( void**) _temp1064) ==  Cyc_ParamDeclList_tok){
_LL1071: _temp1070=(( struct Cyc_ParamDeclList_tok_struct*) _temp1064)->f1; goto
_LL1067;} else{ goto _LL1068;} _LL1068: goto _LL1069; _LL1067: yyzzz= _temp1070;
goto _LL1065; _LL1069:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1065:;} return yyzzz;} static unsigned char _temp1073[ 107u]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={ Cyc_Core_Failure,{
_temp1073, _temp1073, _temp1073 +  107u}}; struct _tuple16* Cyc_yyget_YY1( void*
yy1){ struct _tuple16* yyzzz;{ void* _temp1074= yy1; struct _tuple16* _temp1080;
_LL1076: if(*(( void**) _temp1074) ==  Cyc_YY1){ _LL1081: _temp1080=(( struct
Cyc_YY1_struct*) _temp1074)->f1; goto _LL1077;} else{ goto _LL1078;} _LL1078:
goto _LL1079; _LL1077: yyzzz= _temp1080; goto _LL1075; _LL1079:( int) _throw((
void*)& Cyc_yyfail_YY1); _LL1075:;} return yyzzz;} static unsigned char
_temp1083[ 15u]="list_t<type_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1083, _temp1083, _temp1083 +  15u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void*
_temp1084= yy1; struct Cyc_List_List* _temp1090; _LL1086: if(*(( void**)
_temp1084) ==  Cyc_TypeList_tok){ _LL1091: _temp1090=(( struct Cyc_TypeList_tok_struct*)
_temp1084)->f1; goto _LL1087;} else{ goto _LL1088;} _LL1088: goto _LL1089;
_LL1087: yyzzz= _temp1090; goto _LL1085; _LL1089:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok);
_LL1085:;} return yyzzz;} static unsigned char _temp1093[ 21u]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{
_temp1093, _temp1093, _temp1093 +  21u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1094= yy1; struct Cyc_List_List*
_temp1100; _LL1096: if(*(( void**) _temp1094) ==  Cyc_DesignatorList_tok){
_LL1101: _temp1100=(( struct Cyc_DesignatorList_tok_struct*) _temp1094)->f1;
goto _LL1097;} else{ goto _LL1098;} _LL1098: goto _LL1099; _LL1097: yyzzz=
_temp1100; goto _LL1095; _LL1099:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1095:;} return yyzzz;} static unsigned char _temp1103[ 13u]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{
_temp1103, _temp1103, _temp1103 +  13u}}; void* Cyc_yyget_Designator_tok( void*
yy1){ void* yyzzz;{ void* _temp1104= yy1; void* _temp1110; _LL1106: if(*(( void**)
_temp1104) ==  Cyc_Designator_tok){ _LL1111: _temp1110=( void*)(( struct Cyc_Designator_tok_struct*)
_temp1104)->f1; goto _LL1107;} else{ goto _LL1108;} _LL1108: goto _LL1109;
_LL1107: yyzzz= _temp1110; goto _LL1105; _LL1109:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL1105:;} return yyzzz;} static unsigned char _temp1113[ 7u]="kind_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{
_temp1113, _temp1113, _temp1113 +  7u}}; void* Cyc_yyget_Kind_tok( void* yy1){
void* yyzzz;{ void* _temp1114= yy1; void* _temp1120; _LL1116: if(*(( void**)
_temp1114) ==  Cyc_Kind_tok){ _LL1121: _temp1120=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1114)->f1; goto _LL1117;} else{ goto _LL1118;} _LL1118: goto _LL1119;
_LL1117: yyzzz= _temp1120; goto _LL1115; _LL1119:( int) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1115:;} return yyzzz;} static unsigned char _temp1123[ 7u]="type_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1123, _temp1123, _temp1123 +  7u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* yyzzz;{ void* _temp1124= yy1; void* _temp1130; _LL1126: if(*(( void**)
_temp1124) ==  Cyc_Type_tok){ _LL1131: _temp1130=( void*)(( struct Cyc_Type_tok_struct*)
_temp1124)->f1; goto _LL1127;} else{ goto _LL1128;} _LL1128: goto _LL1129;
_LL1127: yyzzz= _temp1130; goto _LL1125; _LL1129:( int) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1125:;} return yyzzz;} static unsigned char _temp1133[ 20u]="list_t<attribute_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1133, _temp1133, _temp1133 +  20u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1134= yy1; struct Cyc_List_List*
_temp1140; _LL1136: if(*(( void**) _temp1134) ==  Cyc_AttributeList_tok){
_LL1141: _temp1140=(( struct Cyc_AttributeList_tok_struct*) _temp1134)->f1; goto
_LL1137;} else{ goto _LL1138;} _LL1138: goto _LL1139; _LL1137: yyzzz= _temp1140;
goto _LL1135; _LL1139:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1135:;} return yyzzz;} static unsigned char _temp1143[ 12u]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{
_temp1143, _temp1143, _temp1143 +  12u}}; void* Cyc_yyget_Attribute_tok( void*
yy1){ void* yyzzz;{ void* _temp1144= yy1; void* _temp1150; _LL1146: if(*(( void**)
_temp1144) ==  Cyc_Attribute_tok){ _LL1151: _temp1150=( void*)(( struct Cyc_Attribute_tok_struct*)
_temp1144)->f1; goto _LL1147;} else{ goto _LL1148;} _LL1148: goto _LL1149;
_LL1147: yyzzz= _temp1150; goto _LL1145; _LL1149:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1145:;} return yyzzz;} static unsigned char _temp1153[ 12u]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{
_temp1153, _temp1153, _temp1153 +  12u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
void* yy1){ struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp1154= yy1; struct Cyc_Absyn_Enumfield*
_temp1160; _LL1156: if(*(( void**) _temp1154) ==  Cyc_Enumfield_tok){ _LL1161:
_temp1160=(( struct Cyc_Enumfield_tok_struct*) _temp1154)->f1; goto _LL1157;}
else{ goto _LL1158;} _LL1158: goto _LL1159; _LL1157: yyzzz= _temp1160; goto
_LL1155; _LL1159:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1155:;}
return yyzzz;} static unsigned char _temp1163[ 20u]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1163, _temp1163, _temp1163 +  20u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1164= yy1; struct Cyc_List_List*
_temp1170; _LL1166: if(*(( void**) _temp1164) ==  Cyc_EnumfieldList_tok){
_LL1171: _temp1170=(( struct Cyc_EnumfieldList_tok_struct*) _temp1164)->f1; goto
_LL1167;} else{ goto _LL1168;} _LL1168: goto _LL1169; _LL1167: yyzzz= _temp1170;
goto _LL1165; _LL1169:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1165:;} return yyzzz;} static unsigned char _temp1173[ 14u]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{
_temp1173, _temp1173, _temp1173 +  14u}}; struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok(
void* yy1){ struct Cyc_Core_Opt* yyzzz;{ void* _temp1174= yy1; struct Cyc_Core_Opt*
_temp1180; _LL1176: if(*(( void**) _temp1174) ==  Cyc_TypeOpt_tok){ _LL1181:
_temp1180=(( struct Cyc_TypeOpt_tok_struct*) _temp1174)->f1; goto _LL1177;}
else{ goto _LL1178;} _LL1178: goto _LL1179; _LL1177: yyzzz= _temp1180; goto
_LL1175; _LL1179:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1175:;}
return yyzzz;} static unsigned char _temp1183[ 26u]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{
_temp1183, _temp1183, _temp1183 +  26u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1184= yy1; struct Cyc_List_List*
_temp1190; _LL1186: if(*(( void**) _temp1184) ==  Cyc_Rgnorder_tok){ _LL1191:
_temp1190=(( struct Cyc_Rgnorder_tok_struct*) _temp1184)->f1; goto _LL1187;}
else{ goto _LL1188;} _LL1188: goto _LL1189; _LL1187: yyzzz= _temp1190; goto
_LL1185; _LL1189:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1185:;}
return yyzzz;} struct Cyc_Yyltype{ int timestamp; int first_line; int
first_column; int last_line; int last_column; } ; struct Cyc_Yyltype Cyc_yynewloc(){
return({ struct Cyc_Yyltype _temp1192; _temp1192.timestamp= 0; _temp1192.first_line=
0; _temp1192.first_column= 0; _temp1192.last_line= 0; _temp1192.last_column= 0;
_temp1192;});} struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[
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
unsigned char _temp1193[ 2u]="$"; static unsigned char _temp1194[ 6u]="error";
static unsigned char _temp1195[ 12u]="$undefined."; static unsigned char
_temp1196[ 5u]="AUTO"; static unsigned char _temp1197[ 9u]="REGISTER"; static
unsigned char _temp1198[ 7u]="STATIC"; static unsigned char _temp1199[ 7u]="EXTERN";
static unsigned char _temp1200[ 8u]="TYPEDEF"; static unsigned char _temp1201[ 5u]="VOID";
static unsigned char _temp1202[ 5u]="CHAR"; static unsigned char _temp1203[ 6u]="SHORT";
static unsigned char _temp1204[ 4u]="INT"; static unsigned char _temp1205[ 5u]="LONG";
static unsigned char _temp1206[ 6u]="FLOAT"; static unsigned char _temp1207[ 7u]="DOUBLE";
static unsigned char _temp1208[ 7u]="SIGNED"; static unsigned char _temp1209[ 9u]="UNSIGNED";
static unsigned char _temp1210[ 6u]="CONST"; static unsigned char _temp1211[ 9u]="VOLATILE";
static unsigned char _temp1212[ 9u]="RESTRICT"; static unsigned char _temp1213[
7u]="STRUCT"; static unsigned char _temp1214[ 6u]="UNION"; static unsigned char
_temp1215[ 5u]="CASE"; static unsigned char _temp1216[ 8u]="DEFAULT"; static
unsigned char _temp1217[ 7u]="INLINE"; static unsigned char _temp1218[ 7u]="SIZEOF";
static unsigned char _temp1219[ 9u]="OFFSETOF"; static unsigned char _temp1220[
3u]="IF"; static unsigned char _temp1221[ 5u]="ELSE"; static unsigned char
_temp1222[ 7u]="SWITCH"; static unsigned char _temp1223[ 6u]="WHILE"; static
unsigned char _temp1224[ 3u]="DO"; static unsigned char _temp1225[ 4u]="FOR";
static unsigned char _temp1226[ 5u]="GOTO"; static unsigned char _temp1227[ 9u]="CONTINUE";
static unsigned char _temp1228[ 6u]="BREAK"; static unsigned char _temp1229[ 7u]="RETURN";
static unsigned char _temp1230[ 5u]="ENUM"; static unsigned char _temp1231[ 8u]="NULL_kw";
static unsigned char _temp1232[ 4u]="LET"; static unsigned char _temp1233[ 6u]="THROW";
static unsigned char _temp1234[ 4u]="TRY"; static unsigned char _temp1235[ 6u]="CATCH";
static unsigned char _temp1236[ 4u]="NEW"; static unsigned char _temp1237[ 9u]="ABSTRACT";
static unsigned char _temp1238[ 9u]="FALLTHRU"; static unsigned char _temp1239[
6u]="USING"; static unsigned char _temp1240[ 10u]="NAMESPACE"; static
unsigned char _temp1241[ 7u]="TUNION"; static unsigned char _temp1242[ 8u]="XTUNION";
static unsigned char _temp1243[ 5u]="FILL"; static unsigned char _temp1244[ 8u]="CODEGEN";
static unsigned char _temp1245[ 4u]="CUT"; static unsigned char _temp1246[ 7u]="SPLICE";
static unsigned char _temp1247[ 7u]="MALLOC"; static unsigned char _temp1248[ 9u]="REGION_T";
static unsigned char _temp1249[ 7u]="REGION"; static unsigned char _temp1250[ 5u]="RNEW";
static unsigned char _temp1251[ 8u]="RMALLOC"; static unsigned char _temp1252[ 8u]="REGIONS";
static unsigned char _temp1253[ 4u]="GEN"; static unsigned char _temp1254[ 7u]="PTR_OP";
static unsigned char _temp1255[ 7u]="INC_OP"; static unsigned char _temp1256[ 7u]="DEC_OP";
static unsigned char _temp1257[ 8u]="LEFT_OP"; static unsigned char _temp1258[ 9u]="RIGHT_OP";
static unsigned char _temp1259[ 6u]="LE_OP"; static unsigned char _temp1260[ 6u]="GE_OP";
static unsigned char _temp1261[ 6u]="EQ_OP"; static unsigned char _temp1262[ 6u]="NE_OP";
static unsigned char _temp1263[ 7u]="AND_OP"; static unsigned char _temp1264[ 6u]="OR_OP";
static unsigned char _temp1265[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1266[ 11u]="DIV_ASSIGN"; static unsigned char _temp1267[ 11u]="MOD_ASSIGN";
static unsigned char _temp1268[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1269[ 11u]="SUB_ASSIGN"; static unsigned char _temp1270[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1271[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1272[ 11u]="AND_ASSIGN"; static unsigned char _temp1273[ 11u]="XOR_ASSIGN";
static unsigned char _temp1274[ 10u]="OR_ASSIGN"; static unsigned char _temp1275[
9u]="ELLIPSIS"; static unsigned char _temp1276[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1277[ 12u]="COLON_COLON"; static unsigned char _temp1278[ 11u]="IDENTIFIER";
static unsigned char _temp1279[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1280[ 7u]="STRING"; static unsigned char _temp1281[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1282[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1283[ 9u]="TYPE_VAR"; static unsigned char _temp1284[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1285[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1286[ 10u]="ATTRIBUTE"; static unsigned char _temp1287[ 4u]="';'"; static
unsigned char _temp1288[ 4u]="'{'"; static unsigned char _temp1289[ 4u]="'}'";
static unsigned char _temp1290[ 4u]="'='"; static unsigned char _temp1291[ 4u]="'('";
static unsigned char _temp1292[ 4u]="')'"; static unsigned char _temp1293[ 4u]="','";
static unsigned char _temp1294[ 4u]="'_'"; static unsigned char _temp1295[ 4u]="'$'";
static unsigned char _temp1296[ 4u]="'<'"; static unsigned char _temp1297[ 4u]="'>'";
static unsigned char _temp1298[ 4u]="':'"; static unsigned char _temp1299[ 4u]="'.'";
static unsigned char _temp1300[ 4u]="'['"; static unsigned char _temp1301[ 4u]="']'";
static unsigned char _temp1302[ 4u]="'*'"; static unsigned char _temp1303[ 4u]="'@'";
static unsigned char _temp1304[ 4u]="'?'"; static unsigned char _temp1305[ 4u]="'+'";
static unsigned char _temp1306[ 4u]="'-'"; static unsigned char _temp1307[ 4u]="'&'";
static unsigned char _temp1308[ 4u]="'|'"; static unsigned char _temp1309[ 4u]="'^'";
static unsigned char _temp1310[ 4u]="'/'"; static unsigned char _temp1311[ 4u]="'%'";
static unsigned char _temp1312[ 4u]="'~'"; static unsigned char _temp1313[ 4u]="'!'";
static unsigned char _temp1314[ 5u]="prog"; static unsigned char _temp1315[ 17u]="translation_unit";
static unsigned char _temp1316[ 21u]="external_declaration"; static
unsigned char _temp1317[ 20u]="function_definition"; static unsigned char
_temp1318[ 21u]="function_definition2"; static unsigned char _temp1319[ 13u]="using_action";
static unsigned char _temp1320[ 15u]="unusing_action"; static unsigned char
_temp1321[ 17u]="namespace_action"; static unsigned char _temp1322[ 19u]="unnamespace_action";
static unsigned char _temp1323[ 12u]="declaration"; static unsigned char
_temp1324[ 17u]="declaration_list"; static unsigned char _temp1325[ 23u]="declaration_specifiers";
static unsigned char _temp1326[ 24u]="storage_class_specifier"; static
unsigned char _temp1327[ 15u]="attributes_opt"; static unsigned char _temp1328[
11u]="attributes"; static unsigned char _temp1329[ 15u]="attribute_list"; static
unsigned char _temp1330[ 10u]="attribute"; static unsigned char _temp1331[ 15u]="type_specifier";
static unsigned char _temp1332[ 5u]="kind"; static unsigned char _temp1333[ 15u]="type_qualifier";
static unsigned char _temp1334[ 15u]="enum_specifier"; static unsigned char
_temp1335[ 11u]="enum_field"; static unsigned char _temp1336[ 22u]="enum_declaration_list";
static unsigned char _temp1337[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1338[ 16u]="type_params_opt"; static unsigned char _temp1339[
16u]="struct_or_union"; static unsigned char _temp1340[ 24u]="struct_declaration_list";
static unsigned char _temp1341[ 25u]="struct_declaration_list0"; static
unsigned char _temp1342[ 21u]="init_declarator_list"; static unsigned char
_temp1343[ 22u]="init_declarator_list0"; static unsigned char _temp1344[ 16u]="init_declarator";
static unsigned char _temp1345[ 19u]="struct_declaration"; static unsigned char
_temp1346[ 25u]="specifier_qualifier_list"; static unsigned char _temp1347[ 23u]="struct_declarator_list";
static unsigned char _temp1348[ 24u]="struct_declarator_list0"; static
unsigned char _temp1349[ 18u]="struct_declarator"; static unsigned char
_temp1350[ 17u]="tunion_specifier"; static unsigned char _temp1351[ 18u]="tunion_or_xtunion";
static unsigned char _temp1352[ 17u]="tunionfield_list"; static unsigned char
_temp1353[ 18u]="tunionfield_scope"; static unsigned char _temp1354[ 12u]="tunionfield";
static unsigned char _temp1355[ 11u]="declarator"; static unsigned char
_temp1356[ 18u]="direct_declarator"; static unsigned char _temp1357[ 8u]="pointer";
static unsigned char _temp1358[ 13u]="pointer_char"; static unsigned char
_temp1359[ 8u]="rgn_opt"; static unsigned char _temp1360[ 4u]="rgn"; static
unsigned char _temp1361[ 20u]="type_qualifier_list"; static unsigned char
_temp1362[ 20u]="parameter_type_list"; static unsigned char _temp1363[ 9u]="type_var";
static unsigned char _temp1364[ 16u]="optional_effect"; static unsigned char
_temp1365[ 19u]="optional_rgn_order"; static unsigned char _temp1366[ 10u]="rgn_order";
static unsigned char _temp1367[ 16u]="optional_inject"; static unsigned char
_temp1368[ 11u]="effect_set"; static unsigned char _temp1369[ 14u]="atomic_effect";
static unsigned char _temp1370[ 11u]="region_set"; static unsigned char
_temp1371[ 15u]="parameter_list"; static unsigned char _temp1372[ 22u]="parameter_declaration";
static unsigned char _temp1373[ 16u]="identifier_list"; static unsigned char
_temp1374[ 17u]="identifier_list0"; static unsigned char _temp1375[ 12u]="initializer";
static unsigned char _temp1376[ 18u]="array_initializer"; static unsigned char
_temp1377[ 17u]="initializer_list"; static unsigned char _temp1378[ 12u]="designation";
static unsigned char _temp1379[ 16u]="designator_list"; static unsigned char
_temp1380[ 11u]="designator"; static unsigned char _temp1381[ 10u]="type_name";
static unsigned char _temp1382[ 14u]="any_type_name"; static unsigned char
_temp1383[ 15u]="type_name_list"; static unsigned char _temp1384[ 20u]="abstract_declarator";
static unsigned char _temp1385[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1386[ 10u]="statement"; static unsigned char _temp1387[ 18u]="labeled_statement";
static unsigned char _temp1388[ 21u]="expression_statement"; static
unsigned char _temp1389[ 19u]="compound_statement"; static unsigned char
_temp1390[ 16u]="block_item_list"; static unsigned char _temp1391[ 20u]="selection_statement";
static unsigned char _temp1392[ 15u]="switch_clauses"; static unsigned char
_temp1393[ 16u]="switchC_clauses"; static unsigned char _temp1394[ 20u]="iteration_statement";
static unsigned char _temp1395[ 15u]="jump_statement"; static unsigned char
_temp1396[ 8u]="pattern"; static unsigned char _temp1397[ 19u]="tuple_pattern_list";
static unsigned char _temp1398[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1399[ 14u]="field_pattern"; static unsigned char _temp1400[ 19u]="field_pattern_list";
static unsigned char _temp1401[ 20u]="field_pattern_list0"; static unsigned char
_temp1402[ 11u]="expression"; static unsigned char _temp1403[ 22u]="assignment_expression";
static unsigned char _temp1404[ 20u]="assignment_operator"; static unsigned char
_temp1405[ 23u]="conditional_expression"; static unsigned char _temp1406[ 20u]="constant_expression";
static unsigned char _temp1407[ 22u]="logical_or_expression"; static
unsigned char _temp1408[ 23u]="logical_and_expression"; static unsigned char
_temp1409[ 24u]="inclusive_or_expression"; static unsigned char _temp1410[ 24u]="exclusive_or_expression";
static unsigned char _temp1411[ 15u]="and_expression"; static unsigned char
_temp1412[ 20u]="equality_expression"; static unsigned char _temp1413[ 22u]="relational_expression";
static unsigned char _temp1414[ 17u]="shift_expression"; static unsigned char
_temp1415[ 20u]="additive_expression"; static unsigned char _temp1416[ 26u]="multiplicative_expression";
static unsigned char _temp1417[ 16u]="cast_expression"; static unsigned char
_temp1418[ 17u]="unary_expression"; static unsigned char _temp1419[ 15u]="unary_operator";
static unsigned char _temp1420[ 19u]="postfix_expression"; static unsigned char
_temp1421[ 19u]="primary_expression"; static unsigned char _temp1422[ 25u]="argument_expression_list";
static unsigned char _temp1423[ 26u]="argument_expression_list0"; static
unsigned char _temp1424[ 9u]="constant"; static unsigned char _temp1425[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 233u]={{ _temp1193, _temp1193, _temp1193
+  2u},{ _temp1194, _temp1194, _temp1194 +  6u},{ _temp1195, _temp1195,
_temp1195 +  12u},{ _temp1196, _temp1196, _temp1196 +  5u},{ _temp1197,
_temp1197, _temp1197 +  9u},{ _temp1198, _temp1198, _temp1198 +  7u},{ _temp1199,
_temp1199, _temp1199 +  7u},{ _temp1200, _temp1200, _temp1200 +  8u},{ _temp1201,
_temp1201, _temp1201 +  5u},{ _temp1202, _temp1202, _temp1202 +  5u},{ _temp1203,
_temp1203, _temp1203 +  6u},{ _temp1204, _temp1204, _temp1204 +  4u},{ _temp1205,
_temp1205, _temp1205 +  5u},{ _temp1206, _temp1206, _temp1206 +  6u},{ _temp1207,
_temp1207, _temp1207 +  7u},{ _temp1208, _temp1208, _temp1208 +  7u},{ _temp1209,
_temp1209, _temp1209 +  9u},{ _temp1210, _temp1210, _temp1210 +  6u},{ _temp1211,
_temp1211, _temp1211 +  9u},{ _temp1212, _temp1212, _temp1212 +  9u},{ _temp1213,
_temp1213, _temp1213 +  7u},{ _temp1214, _temp1214, _temp1214 +  6u},{ _temp1215,
_temp1215, _temp1215 +  5u},{ _temp1216, _temp1216, _temp1216 +  8u},{ _temp1217,
_temp1217, _temp1217 +  7u},{ _temp1218, _temp1218, _temp1218 +  7u},{ _temp1219,
_temp1219, _temp1219 +  9u},{ _temp1220, _temp1220, _temp1220 +  3u},{ _temp1221,
_temp1221, _temp1221 +  5u},{ _temp1222, _temp1222, _temp1222 +  7u},{ _temp1223,
_temp1223, _temp1223 +  6u},{ _temp1224, _temp1224, _temp1224 +  3u},{ _temp1225,
_temp1225, _temp1225 +  4u},{ _temp1226, _temp1226, _temp1226 +  5u},{ _temp1227,
_temp1227, _temp1227 +  9u},{ _temp1228, _temp1228, _temp1228 +  6u},{ _temp1229,
_temp1229, _temp1229 +  7u},{ _temp1230, _temp1230, _temp1230 +  5u},{ _temp1231,
_temp1231, _temp1231 +  8u},{ _temp1232, _temp1232, _temp1232 +  4u},{ _temp1233,
_temp1233, _temp1233 +  6u},{ _temp1234, _temp1234, _temp1234 +  4u},{ _temp1235,
_temp1235, _temp1235 +  6u},{ _temp1236, _temp1236, _temp1236 +  4u},{ _temp1237,
_temp1237, _temp1237 +  9u},{ _temp1238, _temp1238, _temp1238 +  9u},{ _temp1239,
_temp1239, _temp1239 +  6u},{ _temp1240, _temp1240, _temp1240 +  10u},{
_temp1241, _temp1241, _temp1241 +  7u},{ _temp1242, _temp1242, _temp1242 +  8u},{
_temp1243, _temp1243, _temp1243 +  5u},{ _temp1244, _temp1244, _temp1244 +  8u},{
_temp1245, _temp1245, _temp1245 +  4u},{ _temp1246, _temp1246, _temp1246 +  7u},{
_temp1247, _temp1247, _temp1247 +  7u},{ _temp1248, _temp1248, _temp1248 +  9u},{
_temp1249, _temp1249, _temp1249 +  7u},{ _temp1250, _temp1250, _temp1250 +  5u},{
_temp1251, _temp1251, _temp1251 +  8u},{ _temp1252, _temp1252, _temp1252 +  8u},{
_temp1253, _temp1253, _temp1253 +  4u},{ _temp1254, _temp1254, _temp1254 +  7u},{
_temp1255, _temp1255, _temp1255 +  7u},{ _temp1256, _temp1256, _temp1256 +  7u},{
_temp1257, _temp1257, _temp1257 +  8u},{ _temp1258, _temp1258, _temp1258 +  9u},{
_temp1259, _temp1259, _temp1259 +  6u},{ _temp1260, _temp1260, _temp1260 +  6u},{
_temp1261, _temp1261, _temp1261 +  6u},{ _temp1262, _temp1262, _temp1262 +  6u},{
_temp1263, _temp1263, _temp1263 +  7u},{ _temp1264, _temp1264, _temp1264 +  6u},{
_temp1265, _temp1265, _temp1265 +  11u},{ _temp1266, _temp1266, _temp1266 +  11u},{
_temp1267, _temp1267, _temp1267 +  11u},{ _temp1268, _temp1268, _temp1268 +  11u},{
_temp1269, _temp1269, _temp1269 +  11u},{ _temp1270, _temp1270, _temp1270 +  12u},{
_temp1271, _temp1271, _temp1271 +  13u},{ _temp1272, _temp1272, _temp1272 +  11u},{
_temp1273, _temp1273, _temp1273 +  11u},{ _temp1274, _temp1274, _temp1274 +  10u},{
_temp1275, _temp1275, _temp1275 +  9u},{ _temp1276, _temp1276, _temp1276 +  11u},{
_temp1277, _temp1277, _temp1277 +  12u},{ _temp1278, _temp1278, _temp1278 +  11u},{
_temp1279, _temp1279, _temp1279 +  17u},{ _temp1280, _temp1280, _temp1280 +  7u},{
_temp1281, _temp1281, _temp1281 +  19u},{ _temp1282, _temp1282, _temp1282 +  18u},{
_temp1283, _temp1283, _temp1283 +  9u},{ _temp1284, _temp1284, _temp1284 +  16u},{
_temp1285, _temp1285, _temp1285 +  18u},{ _temp1286, _temp1286, _temp1286 +  10u},{
_temp1287, _temp1287, _temp1287 +  4u},{ _temp1288, _temp1288, _temp1288 +  4u},{
_temp1289, _temp1289, _temp1289 +  4u},{ _temp1290, _temp1290, _temp1290 +  4u},{
_temp1291, _temp1291, _temp1291 +  4u},{ _temp1292, _temp1292, _temp1292 +  4u},{
_temp1293, _temp1293, _temp1293 +  4u},{ _temp1294, _temp1294, _temp1294 +  4u},{
_temp1295, _temp1295, _temp1295 +  4u},{ _temp1296, _temp1296, _temp1296 +  4u},{
_temp1297, _temp1297, _temp1297 +  4u},{ _temp1298, _temp1298, _temp1298 +  4u},{
_temp1299, _temp1299, _temp1299 +  4u},{ _temp1300, _temp1300, _temp1300 +  4u},{
_temp1301, _temp1301, _temp1301 +  4u},{ _temp1302, _temp1302, _temp1302 +  4u},{
_temp1303, _temp1303, _temp1303 +  4u},{ _temp1304, _temp1304, _temp1304 +  4u},{
_temp1305, _temp1305, _temp1305 +  4u},{ _temp1306, _temp1306, _temp1306 +  4u},{
_temp1307, _temp1307, _temp1307 +  4u},{ _temp1308, _temp1308, _temp1308 +  4u},{
_temp1309, _temp1309, _temp1309 +  4u},{ _temp1310, _temp1310, _temp1310 +  4u},{
_temp1311, _temp1311, _temp1311 +  4u},{ _temp1312, _temp1312, _temp1312 +  4u},{
_temp1313, _temp1313, _temp1313 +  4u},{ _temp1314, _temp1314, _temp1314 +  5u},{
_temp1315, _temp1315, _temp1315 +  17u},{ _temp1316, _temp1316, _temp1316 +  21u},{
_temp1317, _temp1317, _temp1317 +  20u},{ _temp1318, _temp1318, _temp1318 +  21u},{
_temp1319, _temp1319, _temp1319 +  13u},{ _temp1320, _temp1320, _temp1320 +  15u},{
_temp1321, _temp1321, _temp1321 +  17u},{ _temp1322, _temp1322, _temp1322 +  19u},{
_temp1323, _temp1323, _temp1323 +  12u},{ _temp1324, _temp1324, _temp1324 +  17u},{
_temp1325, _temp1325, _temp1325 +  23u},{ _temp1326, _temp1326, _temp1326 +  24u},{
_temp1327, _temp1327, _temp1327 +  15u},{ _temp1328, _temp1328, _temp1328 +  11u},{
_temp1329, _temp1329, _temp1329 +  15u},{ _temp1330, _temp1330, _temp1330 +  10u},{
_temp1331, _temp1331, _temp1331 +  15u},{ _temp1332, _temp1332, _temp1332 +  5u},{
_temp1333, _temp1333, _temp1333 +  15u},{ _temp1334, _temp1334, _temp1334 +  15u},{
_temp1335, _temp1335, _temp1335 +  11u},{ _temp1336, _temp1336, _temp1336 +  22u},{
_temp1337, _temp1337, _temp1337 +  26u},{ _temp1338, _temp1338, _temp1338 +  16u},{
_temp1339, _temp1339, _temp1339 +  16u},{ _temp1340, _temp1340, _temp1340 +  24u},{
_temp1341, _temp1341, _temp1341 +  25u},{ _temp1342, _temp1342, _temp1342 +  21u},{
_temp1343, _temp1343, _temp1343 +  22u},{ _temp1344, _temp1344, _temp1344 +  16u},{
_temp1345, _temp1345, _temp1345 +  19u},{ _temp1346, _temp1346, _temp1346 +  25u},{
_temp1347, _temp1347, _temp1347 +  23u},{ _temp1348, _temp1348, _temp1348 +  24u},{
_temp1349, _temp1349, _temp1349 +  18u},{ _temp1350, _temp1350, _temp1350 +  17u},{
_temp1351, _temp1351, _temp1351 +  18u},{ _temp1352, _temp1352, _temp1352 +  17u},{
_temp1353, _temp1353, _temp1353 +  18u},{ _temp1354, _temp1354, _temp1354 +  12u},{
_temp1355, _temp1355, _temp1355 +  11u},{ _temp1356, _temp1356, _temp1356 +  18u},{
_temp1357, _temp1357, _temp1357 +  8u},{ _temp1358, _temp1358, _temp1358 +  13u},{
_temp1359, _temp1359, _temp1359 +  8u},{ _temp1360, _temp1360, _temp1360 +  4u},{
_temp1361, _temp1361, _temp1361 +  20u},{ _temp1362, _temp1362, _temp1362 +  20u},{
_temp1363, _temp1363, _temp1363 +  9u},{ _temp1364, _temp1364, _temp1364 +  16u},{
_temp1365, _temp1365, _temp1365 +  19u},{ _temp1366, _temp1366, _temp1366 +  10u},{
_temp1367, _temp1367, _temp1367 +  16u},{ _temp1368, _temp1368, _temp1368 +  11u},{
_temp1369, _temp1369, _temp1369 +  14u},{ _temp1370, _temp1370, _temp1370 +  11u},{
_temp1371, _temp1371, _temp1371 +  15u},{ _temp1372, _temp1372, _temp1372 +  22u},{
_temp1373, _temp1373, _temp1373 +  16u},{ _temp1374, _temp1374, _temp1374 +  17u},{
_temp1375, _temp1375, _temp1375 +  12u},{ _temp1376, _temp1376, _temp1376 +  18u},{
_temp1377, _temp1377, _temp1377 +  17u},{ _temp1378, _temp1378, _temp1378 +  12u},{
_temp1379, _temp1379, _temp1379 +  16u},{ _temp1380, _temp1380, _temp1380 +  11u},{
_temp1381, _temp1381, _temp1381 +  10u},{ _temp1382, _temp1382, _temp1382 +  14u},{
_temp1383, _temp1383, _temp1383 +  15u},{ _temp1384, _temp1384, _temp1384 +  20u},{
_temp1385, _temp1385, _temp1385 +  27u},{ _temp1386, _temp1386, _temp1386 +  10u},{
_temp1387, _temp1387, _temp1387 +  18u},{ _temp1388, _temp1388, _temp1388 +  21u},{
_temp1389, _temp1389, _temp1389 +  19u},{ _temp1390, _temp1390, _temp1390 +  16u},{
_temp1391, _temp1391, _temp1391 +  20u},{ _temp1392, _temp1392, _temp1392 +  15u},{
_temp1393, _temp1393, _temp1393 +  16u},{ _temp1394, _temp1394, _temp1394 +  20u},{
_temp1395, _temp1395, _temp1395 +  15u},{ _temp1396, _temp1396, _temp1396 +  8u},{
_temp1397, _temp1397, _temp1397 +  19u},{ _temp1398, _temp1398, _temp1398 +  20u},{
_temp1399, _temp1399, _temp1399 +  14u},{ _temp1400, _temp1400, _temp1400 +  19u},{
_temp1401, _temp1401, _temp1401 +  20u},{ _temp1402, _temp1402, _temp1402 +  11u},{
_temp1403, _temp1403, _temp1403 +  22u},{ _temp1404, _temp1404, _temp1404 +  20u},{
_temp1405, _temp1405, _temp1405 +  23u},{ _temp1406, _temp1406, _temp1406 +  20u},{
_temp1407, _temp1407, _temp1407 +  22u},{ _temp1408, _temp1408, _temp1408 +  23u},{
_temp1409, _temp1409, _temp1409 +  24u},{ _temp1410, _temp1410, _temp1410 +  24u},{
_temp1411, _temp1411, _temp1411 +  15u},{ _temp1412, _temp1412, _temp1412 +  20u},{
_temp1413, _temp1413, _temp1413 +  22u},{ _temp1414, _temp1414, _temp1414 +  17u},{
_temp1415, _temp1415, _temp1415 +  20u},{ _temp1416, _temp1416, _temp1416 +  26u},{
_temp1417, _temp1417, _temp1417 +  16u},{ _temp1418, _temp1418, _temp1418 +  17u},{
_temp1419, _temp1419, _temp1419 +  15u},{ _temp1420, _temp1420, _temp1420 +  19u},{
_temp1421, _temp1421, _temp1421 +  19u},{ _temp1422, _temp1422, _temp1422 +  25u},{
_temp1423, _temp1423, _temp1423 +  26u},{ _temp1424, _temp1424, _temp1424 +  9u},{
_temp1425, _temp1425, _temp1425 +  20u}}; static short Cyc_yyr1[ 406u]={ 0, 121,
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
short*)({ unsigned int _temp3238= 10000u; short* _temp3239=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3238));{ unsigned int
_temp3240= _temp3238; unsigned int i; for( i= 0; i <  _temp3240; i ++){
_temp3239[ i]=( short) 0;}}; _temp3239;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3235= 10000u; void** _temp3236=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3235));{ unsigned int _temp3237= _temp3235;
unsigned int i; for( i= 0; i <  _temp3237; i ++){ _temp3236[ i]= Cyc_yylval;}};
_temp3236;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3232= 10000u; struct Cyc_Yyltype* _temp3233=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3232));{
unsigned int _temp3234= _temp3232; unsigned int i; for( i= 0; i <  _temp3234; i
++){ _temp3233[ i]= Cyc_yynewloc();}}; _temp3233;}); int yystacksize= 10000;
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
yyn){ case 1: _LL1426: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1427:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1429=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1429[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1430; _temp1430.tag= Cyc_DeclList_tok; _temp1430.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1430;});
_temp1429;}); break; case 3: _LL1428: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1432=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1432[ 0]=({ struct Cyc_DeclList_tok_struct _temp1433; _temp1433.tag= Cyc_DeclList_tok;
_temp1433.f1=({ struct Cyc_List_List* _temp1434=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1434->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1435=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1435->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1436=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1436[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1437; _temp1437.tag= Cyc_Absyn_Using_d;
_temp1437.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1437.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1437;}); _temp1436;}));
_temp1435->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1435;});
_temp1434->tl= 0; _temp1434;}); _temp1433;}); _temp1432;}); Cyc_Lex_leave_using();
break; case 4: _LL1431: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1439=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1439[ 0]=({ struct Cyc_DeclList_tok_struct _temp1440; _temp1440.tag= Cyc_DeclList_tok;
_temp1440.f1=({ struct Cyc_List_List* _temp1441=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1441->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1442=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1442->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1443=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1443[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1444; _temp1444.tag= Cyc_Absyn_Using_d;
_temp1444.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp1444.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1444;});
_temp1443;})); _temp1442->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1442;}); _temp1441->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1441;}); _temp1440;});
_temp1439;}); break; case 5: _LL1438: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1446=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1446[ 0]=({ struct Cyc_DeclList_tok_struct _temp1447; _temp1447.tag= Cyc_DeclList_tok;
_temp1447.f1=({ struct Cyc_List_List* _temp1448=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1448->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1449=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1449->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1450=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1450[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1451; _temp1451.tag=
Cyc_Absyn_Namespace_d; _temp1451.f1=({ struct _tagged_arr* _temp1452=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1452[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1452;});
_temp1451.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1451;}); _temp1450;})); _temp1449->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1449;}); _temp1448->tl= 0; _temp1448;}); _temp1447;}); _temp1446;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1445: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1454=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1454[ 0]=({ struct Cyc_DeclList_tok_struct _temp1455; _temp1455.tag= Cyc_DeclList_tok;
_temp1455.f1=({ struct Cyc_List_List* _temp1456=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1456->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1457=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1457->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1458=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1458[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1459; _temp1459.tag=
Cyc_Absyn_Namespace_d; _temp1459.f1=({ struct _tagged_arr* _temp1460=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1460[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1460;});
_temp1459.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1459;}); _temp1458;})); _temp1457->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1457;}); _temp1456->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1456;}); _temp1455;});
_temp1454;}); break; case 7: _LL1453: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1462=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1462[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1463; _temp1463.tag= Cyc_DeclList_tok; _temp1463.f1=({ struct Cyc_List_List*
_temp1464=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1464->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1465=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1465->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1466=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1466[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1467; _temp1467.tag= Cyc_Absyn_ExternC_d;
_temp1467.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1467;}); _temp1466;})); _temp1465->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1465;}); _temp1464->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1464;}); _temp1463;});
_temp1462;}); break; case 8: _LL1461: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1469=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1469[ 0]=({ struct Cyc_DeclList_tok_struct _temp1470; _temp1470.tag= Cyc_DeclList_tok;
_temp1470.f1= 0; _temp1470;}); _temp1469;}); break; case 9: _LL1468: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1472=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1472[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1473; _temp1473.tag= Cyc_DeclList_tok; _temp1473.f1=({ struct Cyc_List_List*
_temp1474=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1474->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1475=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1475[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1476; _temp1476.tag= Cyc_Absyn_Fn_d;
_temp1476.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1476;}); _temp1475;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1474->tl=
0; _temp1474;}); _temp1473;}); _temp1472;}); break; case 10: _LL1471: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1477: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1479=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1479[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1480; _temp1480.tag= Cyc_FnDecl_tok; _temp1480.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1480;}); _temp1479;}); break; case 12: _LL1478: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1482=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1482[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1483; _temp1483.tag= Cyc_FnDecl_tok;
_temp1483.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1484=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1484->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1484;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1483;}); _temp1482;}); break; case 13: _LL1481: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1486=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1486[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1487; _temp1487.tag= Cyc_FnDecl_tok;
_temp1487.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1487;}); _temp1486;}); break; case 14: _LL1485: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1489=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1489[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1490; _temp1490.tag= Cyc_FnDecl_tok;
_temp1490.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1491=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1491->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1491;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1490;}); _temp1489;}); break; case 15: _LL1488: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1493=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1493[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1494; _temp1494.tag= Cyc_FnDecl_tok;
_temp1494.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1495=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1495->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1495;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1494;}); _temp1493;}); break; case 16: _LL1492: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1497=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1497[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1498; _temp1498.tag= Cyc_FnDecl_tok;
_temp1498.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1499=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1499->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1499;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1498;}); _temp1497;}); break; case 17: _LL1496: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1500:
Cyc_Lex_leave_using(); break; case 19: _LL1501: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1503=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1503[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1503;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1502: Cyc_Lex_leave_namespace(); break; case 21: _LL1504: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1506=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1506[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1507; _temp1507.tag= Cyc_DeclList_tok; _temp1507.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1507;}); _temp1506;}); break; case 22: _LL1505: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1509=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1509[ 0]=({ struct Cyc_DeclList_tok_struct _temp1510; _temp1510.tag= Cyc_DeclList_tok;
_temp1510.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1510;}); _temp1509;}); break; case 23: _LL1508: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1512=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1512[ 0]=({ struct Cyc_DeclList_tok_struct _temp1513; _temp1513.tag= Cyc_DeclList_tok;
_temp1513.f1=({ struct Cyc_List_List* _temp1514=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1514->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1514->tl=
0; _temp1514;}); _temp1513;}); _temp1512;}); break; case 24: _LL1511: { struct
Cyc_List_List* _temp1516= 0;{ struct Cyc_List_List* _temp1517= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0;
_temp1517 !=  0; _temp1517= _temp1517->tl){ struct _tagged_arr* _temp1518=(
struct _tagged_arr*) _temp1517->hd; struct _tuple1* _temp1519=({ struct _tuple1*
_temp1522=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1522->f1=
Cyc_Absyn_rel_ns_null; _temp1522->f2= _temp1518; _temp1522;}); struct Cyc_Absyn_Vardecl*
_temp1520= Cyc_Absyn_new_vardecl( _temp1519, Cyc_Absyn_wildtyp( 0), 0);
_temp1516=({ struct Cyc_List_List* _temp1521=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1521->hd=( void*) _temp1520; _temp1521->tl=
_temp1516; _temp1521;});}} _temp1516=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1516); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1523=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1523[ 0]=({ struct Cyc_DeclList_tok_struct _temp1524; _temp1524.tag= Cyc_DeclList_tok;
_temp1524.f1=({ struct Cyc_List_List* _temp1525=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1525->hd=( void*) Cyc_Absyn_letv_decl(
_temp1516, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1525->tl= 0; _temp1525;}); _temp1524;});
_temp1523;}); break;} case 25: _LL1515: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1526:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1528=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1528[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1529; _temp1529.tag= Cyc_DeclList_tok; _temp1529.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1529;});
_temp1528;}); break; case 27: _LL1527: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1531=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1531[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1532; _temp1532.tag= Cyc_DeclSpec_tok;
_temp1532.f1=({ struct Cyc_Parse_Declaration_spec* _temp1533=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1533->sc=({ struct
Cyc_Core_Opt* _temp1534=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1534->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1534;});
_temp1533->tq= Cyc_Absyn_empty_tqual(); _temp1533->type_specs= 0; _temp1533->is_inline=
0; _temp1533->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1533;}); _temp1532;});
_temp1531;}); break; case 28: _LL1530: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc !=  0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1536=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1536[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1537; _temp1537.tag= Cyc_DeclSpec_tok; _temp1537.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1538=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1538->sc=({ struct Cyc_Core_Opt* _temp1539=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1539->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1539;});
_temp1538->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1538->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1538->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1538->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1538;}); _temp1537;}); _temp1536;}); break; case 29: _LL1535: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1541=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1541[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1542; _temp1542.tag= Cyc_DeclSpec_tok; _temp1542.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1543=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1543->sc= 0; _temp1543->tq= Cyc_Absyn_empty_tqual(); _temp1543->type_specs=({
struct Cyc_List_List* _temp1544=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1544->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1544->tl= 0;
_temp1544;}); _temp1543->is_inline= 0; _temp1543->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1543;});
_temp1542;}); _temp1541;}); break; case 30: _LL1540: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1546=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1546[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1547; _temp1547.tag= Cyc_DeclSpec_tok;
_temp1547.f1=({ struct Cyc_Parse_Declaration_spec* _temp1548=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1548->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1548->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1548->type_specs=({ struct Cyc_List_List* _temp1549=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1549->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1549->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1549;});
_temp1548->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1548->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1548;}); _temp1547;}); _temp1546;}); break; case 31: _LL1545: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1551=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1551[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1552; _temp1552.tag= Cyc_DeclSpec_tok; _temp1552.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1553=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1553->sc= 0; _temp1553->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1553->type_specs=
0; _temp1553->is_inline= 0; _temp1553->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1553;});
_temp1552;}); _temp1551;}); break; case 32: _LL1550: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1555=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1555[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1556; _temp1556.tag= Cyc_DeclSpec_tok;
_temp1556.f1=({ struct Cyc_Parse_Declaration_spec* _temp1557=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1557->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1557->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1557->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1557->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1557->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1557;}); _temp1556;}); _temp1555;}); break; case 33: _LL1554: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1559=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1559[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1560; _temp1560.tag= Cyc_DeclSpec_tok; _temp1560.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1561=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1561->sc= 0; _temp1561->tq= Cyc_Absyn_empty_tqual(); _temp1561->type_specs=
0; _temp1561->is_inline= 1; _temp1561->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1561;});
_temp1560;}); _temp1559;}); break; case 34: _LL1558: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1563=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1563[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1564; _temp1564.tag= Cyc_DeclSpec_tok;
_temp1564.f1=({ struct Cyc_Parse_Declaration_spec* _temp1565=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1565->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1565->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1565->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1565->is_inline=
1; _temp1565->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1565;}); _temp1564;}); _temp1563;}); break; case 35: _LL1562: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1567=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1567[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1568; _temp1568.tag= Cyc_StorageClass_tok;
_temp1568.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1568;}); _temp1567;});
break; case 36: _LL1566: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1570=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1570[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1571; _temp1571.tag=
Cyc_StorageClass_tok; _temp1571.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1571;}); _temp1570;}); break; case 37: _LL1569: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1573=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1573[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1574; _temp1574.tag=
Cyc_StorageClass_tok; _temp1574.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1574;}); _temp1573;}); break; case 38: _LL1572: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1576=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1576[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1577; _temp1577.tag=
Cyc_StorageClass_tok; _temp1577.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1577;}); _temp1576;}); break; case 39: _LL1575: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1579=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1579[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1580; _temp1580.tag= Cyc_StorageClass_tok;
_temp1580.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1580;}); _temp1579;});
break; case 40: _LL1578: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1582=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1582[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1583; _temp1583.tag=
Cyc_StorageClass_tok; _temp1583.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1583;}); _temp1582;}); break; case 41: _LL1581: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1585=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1585[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1586; _temp1586.tag=
Cyc_StorageClass_tok; _temp1586.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1586;}); _temp1585;}); break; case 42: _LL1584: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1588=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1588[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1589; _temp1589.tag=
Cyc_AttributeList_tok; _temp1589.f1= 0; _temp1589;}); _temp1588;}); break; case
43: _LL1587: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1590: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1592=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1592[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1593; _temp1593.tag=
Cyc_AttributeList_tok; _temp1593.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1593;});
_temp1592;}); break; case 45: _LL1591: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1595=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1595[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1596; _temp1596.tag=
Cyc_AttributeList_tok; _temp1596.f1=({ struct Cyc_List_List* _temp1597=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1597->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1597->tl= 0; _temp1597;}); _temp1596;}); _temp1595;});
break; case 46: _LL1594: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1599=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1599[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1600; _temp1600.tag=
Cyc_AttributeList_tok; _temp1600.f1=({ struct Cyc_List_List* _temp1601=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1601->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1601->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1601;}); _temp1600;});
_temp1599;}); break; case 47: _LL1598: { struct _tagged_arr _temp1603= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1603, sizeof( unsigned char)) >  4?*(( const unsigned char*)
_check_unknown_subscript( _temp1603, sizeof( unsigned char), 0)) == '_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1603, sizeof( unsigned char),
1)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1603,
sizeof( unsigned char),( int)( _get_arr_size( _temp1603, sizeof( unsigned char))
-  2))) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1603,
sizeof( unsigned char),( int)( _get_arr_size( _temp1603, sizeof( unsigned char))
-  3))) == '_': 0){ _temp1603=( struct _tagged_arr) Cyc_Std_substring( _temp1603,
2, _get_arr_size( _temp1603, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0;
i <  16u; ++ i){ if( Cyc_Std_strcmp( _temp1603,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1604=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1604[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1605; _temp1605.tag= Cyc_Attribute_tok; _temp1605.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1605;}); _temp1604;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1606=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1606[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1607; _temp1607.tag= Cyc_Attribute_tok; _temp1607.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1607;}); _temp1606;});} break;}} case
48: _LL1602: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1609=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1609[ 0]=({ struct Cyc_Attribute_tok_struct _temp1610; _temp1610.tag= Cyc_Attribute_tok;
_temp1610.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1610;}); _temp1609;});
break; case 49: _LL1608: { struct _tagged_arr _temp1612= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1615; int _temp1616; struct _tuple15* _temp1613= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1615=*
_temp1613; _LL1617: _temp1616= _temp1615.f2; goto _LL1614; _LL1614: { void* a;
if( Cyc_Std_zstrcmp( _temp1612, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1612, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1616 <  0? 1: _temp1616 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 0 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1618=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1618[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1619; _temp1619.tag= Cyc_Absyn_Regparm_att; _temp1619.f1= _temp1616;
_temp1619;}); _temp1618;});} else{ if( Cyc_Std_zstrcmp( _temp1612, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1612, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1616 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1616;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1620=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1620[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1621; _temp1621.tag= Cyc_Absyn_Aligned_att; _temp1621.f1= _temp1616;
_temp1621;}); _temp1620;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1622=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1622[ 0]=({ struct Cyc_Attribute_tok_struct _temp1623; _temp1623.tag= Cyc_Attribute_tok;
_temp1623.f1=( void*) a; _temp1623;}); _temp1622;}); break;}} case 50: _LL1611: {
struct _tagged_arr _temp1625= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1626= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1625, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1625, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1627=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1627[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1628; _temp1628.tag=
Cyc_Absyn_Section_att; _temp1628.f1= _temp1626; _temp1628;}); _temp1627;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1629=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1629[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1630; _temp1630.tag= Cyc_Attribute_tok; _temp1630.f1=(
void*) a; _temp1630;}); _temp1629;}); break;} case 51: _LL1624: { struct
_tagged_arr _temp1632= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1633= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1636; struct _tuple15 _temp1634=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1637:
_temp1636= _temp1634.f2; goto _LL1635; _LL1635: { int _temp1640; struct _tuple15
_temp1638=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1641: _temp1640= _temp1638.f2; goto _LL1639; _LL1639: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1632, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1632, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1633, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1642=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1642[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1643; _temp1643.tag=
Cyc_Absyn_Format_att; _temp1643.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1643.f2= _temp1636; _temp1643.f3= _temp1640; _temp1643;}); _temp1642;});}
else{ if( Cyc_Std_zstrcmp( _temp1633, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1644=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1644[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1645; _temp1645.tag=
Cyc_Absyn_Format_att; _temp1645.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1645.f2= _temp1636; _temp1645.f3= _temp1640; _temp1645;}); _temp1644;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1646=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1646[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1647; _temp1647.tag= Cyc_Attribute_tok; _temp1647.f1=(
void*) a; _temp1647;}); _temp1646;}); break;}}} case 52: _LL1631: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1649=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1649[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1650; _temp1650.tag= Cyc_TypeSpecifier_tok;
_temp1650.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1650;});
_temp1649;}); break; case 53: _LL1648: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1652=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1652[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1653; _temp1653.tag=
Cyc_TypeSpecifier_tok; _temp1653.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1653;}); _temp1652;}); break; case 54: _LL1651:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1655=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1655[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1656; _temp1656.tag= Cyc_TypeSpecifier_tok;
_temp1656.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1656;});
_temp1655;}); break; case 55: _LL1654: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1658=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1658[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1659; _temp1659.tag=
Cyc_TypeSpecifier_tok; _temp1659.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1660=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1660[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1661; _temp1661.tag=
Cyc_Parse_Short_spec; _temp1661.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1661;});
_temp1660;})); _temp1659;}); _temp1658;}); break; case 56: _LL1657: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1663=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1663[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1664; _temp1664.tag= Cyc_TypeSpecifier_tok;
_temp1664.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1664;});
_temp1663;}); break; case 57: _LL1662: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1666=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1666[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1667; _temp1667.tag=
Cyc_TypeSpecifier_tok; _temp1667.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1668=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1668[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1669; _temp1669.tag= Cyc_Parse_Long_spec;
_temp1669.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1669;}); _temp1668;})); _temp1667;}); _temp1666;});
break; case 58: _LL1665: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1671=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1671[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1672; _temp1672.tag=
Cyc_TypeSpecifier_tok; _temp1672.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1672;}); _temp1671;}); break; case 59: _LL1670:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1674=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1674[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1675; _temp1675.tag= Cyc_TypeSpecifier_tok;
_temp1675.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t( 0), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1675;});
_temp1674;}); break; case 60: _LL1673: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1677=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1677[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1678; _temp1678.tag=
Cyc_TypeSpecifier_tok; _temp1678.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1679=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1679[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1680; _temp1680.tag=
Cyc_Parse_Signed_spec; _temp1680.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1680;});
_temp1679;})); _temp1678;}); _temp1677;}); break; case 61: _LL1676: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1682=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1682[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1683; _temp1683.tag= Cyc_TypeSpecifier_tok;
_temp1683.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1684=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1684[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1685; _temp1685.tag= Cyc_Parse_Unsigned_spec; _temp1685.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1685;});
_temp1684;})); _temp1683;}); _temp1682;}); break; case 62: _LL1681: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1686:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1687: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1688: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1690=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1690[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1691; _temp1691.tag=
Cyc_TypeSpecifier_tok; _temp1691.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1691;});
_temp1690;}); break; case 66: _LL1689: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1693=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1693[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1694; _temp1694.tag=
Cyc_TypeSpecifier_tok; _temp1694.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1695=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1695[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1696; _temp1696.tag= Cyc_Absyn_TypedefType;
_temp1696.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1696.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1696.f3= 0;
_temp1696;}); _temp1695;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1694;});
_temp1693;}); break; case 67: _LL1692: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1698=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1698[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1699; _temp1699.tag=
Cyc_TypeSpecifier_tok; _temp1699.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1700=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1700[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1701; _temp1701.tag= Cyc_Absyn_TupleType;
_temp1701.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1701;});
_temp1700;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1699;}); _temp1698;}); break; case 68:
_LL1697: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1703=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1703[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1704; _temp1704.tag=
Cyc_TypeSpecifier_tok; _temp1704.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1705=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1705[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1706; _temp1706.tag= Cyc_Absyn_RgnHandleType;
_temp1706.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1706;}); _temp1705;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1704;}); _temp1703;}); break; case 69: _LL1702: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1708=( struct Cyc_Kind_tok_struct*) _cycalloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1708[ 0]=({ struct Cyc_Kind_tok_struct _temp1709; _temp1709.tag= Cyc_Kind_tok;
_temp1709.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1709;});
_temp1708;}); break; case 70: _LL1707: { struct _tagged_arr* _temp1713; void*
_temp1715; struct _tuple1 _temp1711=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1716: _temp1715=
_temp1711.f1; goto _LL1714; _LL1714: _temp1713= _temp1711.f2; goto _LL1712;
_LL1712: if( _temp1715 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1717=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1717[ 0]=({ struct Cyc_Kind_tok_struct
_temp1718; _temp1718.tag= Cyc_Kind_tok; _temp1718.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1713, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1718;}); _temp1717;}); break;} case 71:
_LL1710: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1720=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1720[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1721; _temp1721.tag= Cyc_TypeQual_tok;
_temp1721.f1=({ struct Cyc_Absyn_Tqual _temp1722; _temp1722.q_const= 1;
_temp1722.q_volatile= 0; _temp1722.q_restrict= 0; _temp1722;}); _temp1721;});
_temp1720;}); break; case 72: _LL1719: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1724=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1724[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1725; _temp1725.tag= Cyc_TypeQual_tok; _temp1725.f1=({ struct Cyc_Absyn_Tqual
_temp1726; _temp1726.q_const= 0; _temp1726.q_volatile= 1; _temp1726.q_restrict=
0; _temp1726;}); _temp1725;}); _temp1724;}); break; case 73: _LL1723: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1728=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1728[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1729; _temp1729.tag= Cyc_TypeQual_tok;
_temp1729.f1=({ struct Cyc_Absyn_Tqual _temp1730; _temp1730.q_const= 0;
_temp1730.q_volatile= 0; _temp1730.q_restrict= 1; _temp1730;}); _temp1729;});
_temp1728;}); break; case 74: _LL1727: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1732=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1732[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1733; _temp1733.tag=
Cyc_TypeSpecifier_tok; _temp1733.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1734=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1734[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1735; _temp1735.tag= Cyc_Parse_Decl_spec;
_temp1735.f1=({ struct Cyc_Absyn_Decl* _temp1736=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1736->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1737=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1737[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1738; _temp1738.tag= Cyc_Absyn_Enum_d; _temp1738.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1739=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1739->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1739->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1739->fields=({
struct Cyc_Core_Opt* _temp1740=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1740->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1740;});
_temp1739;}); _temp1738;}); _temp1737;})); _temp1736->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1736;}); _temp1735;}); _temp1734;})); _temp1733;}); _temp1732;}); break;
case 75: _LL1731: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1742=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1742[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1743; _temp1743.tag=
Cyc_TypeSpecifier_tok; _temp1743.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1744=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1744[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1745; _temp1745.tag= Cyc_Absyn_EnumType;
_temp1745.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1745.f2= 0; _temp1745;}); _temp1744;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1743;}); _temp1742;}); break; case 76: _LL1741: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1747=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1747[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1748; _temp1748.tag=
Cyc_TypeSpecifier_tok; _temp1748.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1749=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1749[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1750; _temp1750.tag= Cyc_Parse_Type_spec;
_temp1750.f1=( void*)(( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp1751=(
struct Cyc_Absyn_AnonEnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct));
_temp1751[ 0]=({ struct Cyc_Absyn_AnonEnumType_struct _temp1752; _temp1752.tag=
Cyc_Absyn_AnonEnumType; _temp1752.f1= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1752;});
_temp1751;})); _temp1750.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1750;});
_temp1749;})); _temp1748;}); _temp1747;}); break; case 77: _LL1746: yyval=( void*)({
struct Cyc_Enumfield_tok_struct* _temp1754=( struct Cyc_Enumfield_tok_struct*)
_cycalloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp1754[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp1755; _temp1755.tag= Cyc_Enumfield_tok; _temp1755.f1=({
struct Cyc_Absyn_Enumfield* _temp1756=( struct Cyc_Absyn_Enumfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumfield)); _temp1756->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1756->tag= 0;
_temp1756->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1756;});
_temp1755;}); _temp1754;}); break; case 78: _LL1753: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1758=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1758[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1759; _temp1759.tag= Cyc_Enumfield_tok;
_temp1759.f1=({ struct Cyc_Absyn_Enumfield* _temp1760=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1760->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1760->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1760->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1760;});
_temp1759;}); _temp1758;}); break; case 79: _LL1757: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1762=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1762[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1763; _temp1763.tag=
Cyc_EnumfieldList_tok; _temp1763.f1=({ struct Cyc_List_List* _temp1764=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1764->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1764->tl= 0; _temp1764;}); _temp1763;}); _temp1762;});
break; case 80: _LL1761: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1766=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1766[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1767; _temp1767.tag=
Cyc_EnumfieldList_tok; _temp1767.f1=({ struct Cyc_List_List* _temp1768=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1768->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1768->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1768;}); _temp1767;});
_temp1766;}); break; case 81: _LL1765: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1770: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1772=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1772[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1773; _temp1773.tag=
Cyc_Absyn_AnonStructType; _temp1773.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1773;});
_temp1772;}); break; case Cyc_Parse_Union_su: _LL1771: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1775=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1775[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1776; _temp1776.tag= Cyc_Absyn_AnonUnionType; _temp1776.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1776;});
_temp1775;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1777=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1777[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1778; _temp1778.tag=
Cyc_TypeSpecifier_tok; _temp1778.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1779=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1779[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1780; _temp1780.tag= Cyc_Parse_Type_spec;
_temp1780.f1=( void*) t; _temp1780.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1780;});
_temp1779;})); _temp1778;}); _temp1777;}); break;} case 82: _LL1769: { struct
Cyc_List_List* _temp1782=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1783: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1785=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1785->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1785;}), _temp1782,({ struct Cyc_Core_Opt*
_temp1786=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1786->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1786;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1784: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1788=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1788->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1788;}),
_temp1782,({ struct Cyc_Core_Opt* _temp1789=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1789->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1789;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1790=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1790[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1791; _temp1791.tag=
Cyc_TypeSpecifier_tok; _temp1791.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1792=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1792[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1793; _temp1793.tag= Cyc_Parse_Decl_spec;
_temp1793.f1= d; _temp1793;}); _temp1792;})); _temp1791;}); _temp1790;}); break;}
case 83: _LL1781: { struct Cyc_List_List* _temp1795=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1796: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1798=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1798->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1798;}), _temp1795,({ struct Cyc_Core_Opt*
_temp1799=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1799->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1799;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1797: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1801=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1801->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1801;}),
_temp1795,({ struct Cyc_Core_Opt* _temp1802=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1802->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1802;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1803=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1803[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1804; _temp1804.tag=
Cyc_TypeSpecifier_tok; _temp1804.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1805=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1805[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1806; _temp1806.tag= Cyc_Parse_Decl_spec;
_temp1806.f1= d; _temp1806;}); _temp1805;})); _temp1804;}); _temp1803;}); break;}
case 84: _LL1794: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1808: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1810=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1810[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1811; _temp1811.tag=
Cyc_TypeSpecifier_tok; _temp1811.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1812=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1812[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1813; _temp1813.tag= Cyc_Absyn_StructType;
_temp1813.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1813.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1813.f3= 0;
_temp1813;}); _temp1812;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1811;});
_temp1810;}); break; case Cyc_Parse_Union_su: _LL1809: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1815=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1815[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1816; _temp1816.tag= Cyc_TypeSpecifier_tok;
_temp1816.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1817=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1817[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1818; _temp1818.tag= Cyc_Absyn_UnionType;
_temp1818.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1818.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1818.f3= 0;
_temp1818;}); _temp1817;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1816;});
_temp1815;}); break;} break; case 85: _LL1807: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1820: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1822=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1822[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1823; _temp1823.tag=
Cyc_TypeSpecifier_tok; _temp1823.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1824=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1824[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1825; _temp1825.tag= Cyc_Absyn_StructType;
_temp1825.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1825.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1825.f3= 0;
_temp1825;}); _temp1824;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1823;});
_temp1822;}); break; case Cyc_Parse_Union_su: _LL1821: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1827=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1827[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1828; _temp1828.tag= Cyc_TypeSpecifier_tok;
_temp1828.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1829=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1829[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1830; _temp1830.tag= Cyc_Absyn_UnionType;
_temp1830.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1830.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1830.f3= 0;
_temp1830;}); _temp1829;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1828;});
_temp1827;}); break;} break; case 86: _LL1819: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1832=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1832[ 0]=({ struct Cyc_TypeList_tok_struct _temp1833; _temp1833.tag= Cyc_TypeList_tok;
_temp1833.f1= 0; _temp1833;}); _temp1832;}); break; case 87: _LL1831: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1835=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1835[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1836; _temp1836.tag= Cyc_TypeList_tok; _temp1836.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1836;});
_temp1835;}); break; case 88: _LL1834:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1838=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1838[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1839; _temp1839.tag= Cyc_TypeList_tok; _temp1839.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1839;});
_temp1838;}); break; case 89: _LL1837: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1841=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1841[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1842; _temp1842.tag= Cyc_StructOrUnion_tok; _temp1842.f1= Cyc_Parse_Struct_su;
_temp1842;}); _temp1841;}); break; case 90: _LL1840: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1844=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1844[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1845; _temp1845.tag= Cyc_StructOrUnion_tok; _temp1845.f1= Cyc_Parse_Union_su;
_temp1845;}); _temp1844;}); break; case 91: _LL1843: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1847=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1847[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1848; _temp1848.tag= Cyc_StructFieldDeclList_tok; _temp1848.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1848;});
_temp1847;}); break; case 92: _LL1846: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1850=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1850[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1851; _temp1851.tag= Cyc_StructFieldDeclListList_tok; _temp1851.f1=({
struct Cyc_List_List* _temp1852=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1852->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1852->tl= 0;
_temp1852;}); _temp1851;}); _temp1850;}); break; case 93: _LL1849: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1854=( struct Cyc_StructFieldDeclListList_tok_struct*)
_cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1854[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1855; _temp1855.tag= Cyc_StructFieldDeclListList_tok;
_temp1855.f1=({ struct Cyc_List_List* _temp1856=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1856->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1856->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1856;}); _temp1855;}); _temp1854;}); break;
case 94: _LL1853: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1858=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1858[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1859; _temp1859.tag=
Cyc_InitDeclList_tok; _temp1859.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1859;});
_temp1858;}); break; case 95: _LL1857: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1861=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1861[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1862; _temp1862.tag=
Cyc_InitDeclList_tok; _temp1862.f1=({ struct Cyc_List_List* _temp1863=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1863->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1863->tl= 0; _temp1863;}); _temp1862;}); _temp1861;});
break; case 96: _LL1860: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1865=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1865[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1866; _temp1866.tag=
Cyc_InitDeclList_tok; _temp1866.f1=({ struct Cyc_List_List* _temp1867=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1867->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1867->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1867;});
_temp1866;}); _temp1865;}); break; case 97: _LL1864: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1869=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1869[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1870; _temp1870.tag= Cyc_InitDecl_tok;
_temp1870.f1=({ struct _tuple14* _temp1871=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1871->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1871->f2= 0;
_temp1871;}); _temp1870;}); _temp1869;}); break; case 98: _LL1868: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1873=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1873[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1874; _temp1874.tag= Cyc_InitDecl_tok; _temp1874.f1=({ struct _tuple14*
_temp1875=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1875->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1875->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1875;});
_temp1874;}); _temp1873;}); break; case 99: _LL1872: { struct _tuple17 _temp1879;
struct Cyc_List_List* _temp1880; struct Cyc_List_List* _temp1882; struct Cyc_Absyn_Tqual
_temp1884; struct _tuple17* _temp1877= Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1879=*
_temp1877; _LL1885: _temp1884= _temp1879.f1; goto _LL1883; _LL1883: _temp1882=
_temp1879.f2; goto _LL1881; _LL1881: _temp1880= _temp1879.f3; goto _LL1878;
_LL1878: { struct Cyc_List_List* _temp1888; struct Cyc_List_List* _temp1890;
struct _tuple0 _temp1886=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1891: _temp1890= _temp1886.f1; goto _LL1889; _LL1889:
_temp1888= _temp1886.f2; goto _LL1887; _LL1887: { void* _temp1892= Cyc_Parse_speclist2typ(
_temp1882, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1893=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1884, _temp1892, _temp1890, _temp1880), _temp1888);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1894=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1894[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1895; _temp1895.tag=
Cyc_StructFieldDeclList_tok; _temp1895.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1893); _temp1895;}); _temp1894;}); break;}}}
case 100: _LL1876: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1897=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1897[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1898; _temp1898.tag=
Cyc_QualSpecList_tok; _temp1898.f1=({ struct _tuple17* _temp1899=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1899->f1= Cyc_Absyn_empty_tqual();
_temp1899->f2=({ struct Cyc_List_List* _temp1900=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1900->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1900->tl=
0; _temp1900;}); _temp1899->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1899;}); _temp1898;});
_temp1897;}); break; case 101: _LL1896: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1902=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1902[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1903; _temp1903.tag=
Cyc_QualSpecList_tok; _temp1903.f1=({ struct _tuple17* _temp1904=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1904->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1904->f2=({
struct Cyc_List_List* _temp1905=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1905->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1905->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1905;}); _temp1904->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1904;});
_temp1903;}); _temp1902;}); break; case 102: _LL1901: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1907=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1907[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1908; _temp1908.tag= Cyc_QualSpecList_tok;
_temp1908.f1=({ struct _tuple17* _temp1909=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1909->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1909->f2= 0;
_temp1909->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1909;}); _temp1908;}); _temp1907;}); break; case
103: _LL1906: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1911=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1911[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1912; _temp1912.tag=
Cyc_QualSpecList_tok; _temp1912.f1=({ struct _tuple17* _temp1913=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1913->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1913->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1913->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1913;});
_temp1912;}); _temp1911;}); break; case 104: _LL1910: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1915=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1915[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1916; _temp1916.tag= Cyc_InitDeclList_tok;
_temp1916.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1916;}); _temp1915;}); break; case 105: _LL1914: yyval=(
void*)({ struct Cyc_InitDeclList_tok_struct* _temp1918=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1918[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1919; _temp1919.tag= Cyc_InitDeclList_tok;
_temp1919.f1=({ struct Cyc_List_List* _temp1920=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1920->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1920->tl= 0;
_temp1920;}); _temp1919;}); _temp1918;}); break; case 106: _LL1917: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp1922=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1922[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1923; _temp1923.tag= Cyc_InitDeclList_tok;
_temp1923.f1=({ struct Cyc_List_List* _temp1924=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1924->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1924->tl=
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1924;}); _temp1923;}); _temp1922;}); break; case 107:
_LL1921: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp1926=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1926[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1927; _temp1927.tag= Cyc_InitDecl_tok; _temp1927.f1=({ struct _tuple14*
_temp1928=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1928->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1928->f2= 0; _temp1928;}); _temp1927;}); _temp1926;});
break; case 108: _LL1925: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1930=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1930[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1931; _temp1931.tag= Cyc_InitDecl_tok;
_temp1931.f1=({ struct _tuple14* _temp1932=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1932->f1=({ struct Cyc_Parse_Declarator*
_temp1933=( struct Cyc_Parse_Declarator*) _cycalloc( sizeof( struct Cyc_Parse_Declarator));
_temp1933->id=({ struct _tuple1* _temp1934=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp1934->f1= Cyc_Absyn_rel_ns_null; _temp1934->f2=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1934;}); _temp1933->tms= 0; _temp1933;});
_temp1932->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1932;}); _temp1931;});
_temp1930;}); break; case 109: _LL1929: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1936=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1936[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1937; _temp1937.tag= Cyc_InitDecl_tok;
_temp1937.f1=({ struct _tuple14* _temp1938=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1938->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1938->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1938;}); _temp1937;}); _temp1936;}); break; case
110: _LL1935: { struct Cyc_List_List* _temp1940=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1941=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1941[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1942; _temp1942.tag=
Cyc_TypeSpecifier_tok; _temp1942.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1943=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1943[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1944; _temp1944.tag= Cyc_Parse_Decl_spec;
_temp1944.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1940,({
struct Cyc_Core_Opt* _temp1945=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1945->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1945;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1944;}); _temp1943;})); _temp1942;}); _temp1941;}); break;} case 111:
_LL1939: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1947=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1947[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1948; _temp1948.tag=
Cyc_TypeSpecifier_tok; _temp1948.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1949=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1949[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1950; _temp1950.tag= Cyc_Absyn_TunionType;
_temp1950.f1=({ struct Cyc_Absyn_TunionInfo _temp1951; _temp1951.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1952=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1952[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1953; _temp1953.tag= Cyc_Absyn_UnknownTunion;
_temp1953.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1954; _temp1954.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1954.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1954;});
_temp1953;}); _temp1952;})); _temp1951.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1951.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1951;}); _temp1950;}); _temp1949;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1948;}); _temp1947;}); break; case 112: _LL1946: { void* _temp1956= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp1965=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1965->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp1965;}),
0); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1957=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1957[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1958; _temp1958.tag= Cyc_TypeSpecifier_tok;
_temp1958.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1959=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1959[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1960; _temp1960.tag=
Cyc_Absyn_TunionType; _temp1960.f1=({ struct Cyc_Absyn_TunionInfo _temp1961;
_temp1961.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1962=( struct Cyc_Absyn_UnknownTunion_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1962[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1963; _temp1963.tag= Cyc_Absyn_UnknownTunion; _temp1963.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1964; _temp1964.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1964.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1964;}); _temp1963;}); _temp1962;})); _temp1961.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1961.rgn=(
void*) _temp1956; _temp1961;}); _temp1960;}); _temp1959;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1958;}); _temp1957;}); break;} case 113: _LL1955: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1967=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1967[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1968; _temp1968.tag= Cyc_TypeSpecifier_tok;
_temp1968.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1969=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1969[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1970; _temp1970.tag= Cyc_Absyn_TunionFieldType; _temp1970.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1971; _temp1971.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp1972=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp1972[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp1973; _temp1973.tag= Cyc_Absyn_UnknownTunionfield; _temp1973.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp1974; _temp1974.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1974.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1974.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1974;});
_temp1973;}); _temp1972;})); _temp1971.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1971;}); _temp1970;});
_temp1969;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1968;}); _temp1967;}); break; case 114:
_LL1966: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1976=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1976[ 0]=({ struct
Cyc_Bool_tok_struct _temp1977; _temp1977.tag= Cyc_Bool_tok; _temp1977.f1= 0;
_temp1977;}); _temp1976;}); break; case 115: _LL1975: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1979=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1979[ 0]=({ struct Cyc_Bool_tok_struct
_temp1980; _temp1980.tag= Cyc_Bool_tok; _temp1980.f1= 1; _temp1980;}); _temp1979;});
break; case 116: _LL1978: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1982=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1982[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1983; _temp1983.tag= Cyc_TunionFieldList_tok; _temp1983.f1=({ struct Cyc_List_List*
_temp1984=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1984->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1984->tl= 0;
_temp1984;}); _temp1983;}); _temp1982;}); break; case 117: _LL1981: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp1986=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1986[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1987; _temp1987.tag= Cyc_TunionFieldList_tok;
_temp1987.f1=({ struct Cyc_List_List* _temp1988=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1988->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1988->tl=
0; _temp1988;}); _temp1987;}); _temp1986;}); break; case 118: _LL1985: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1990=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1990[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1991; _temp1991.tag= Cyc_TunionFieldList_tok;
_temp1991.f1=({ struct Cyc_List_List* _temp1992=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1992->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1992->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1992;}); _temp1991;}); _temp1990;}); break; case 119:
_LL1989: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1994=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp1994[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp1995; _temp1995.tag=
Cyc_TunionFieldList_tok; _temp1995.f1=({ struct Cyc_List_List* _temp1996=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1996->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1996->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1996;}); _temp1995;});
_temp1994;}); break; case 120: _LL1993: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1998=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1998[ 0]=({ struct Cyc_Scope_tok_struct _temp1999; _temp1999.tag= Cyc_Scope_tok;
_temp1999.f1=( void*)(( void*) Cyc_Absyn_Public); _temp1999;}); _temp1998;});
break; case 121: _LL1997: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2001=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2001[ 0]=({ struct Cyc_Scope_tok_struct _temp2002; _temp2002.tag= Cyc_Scope_tok;
_temp2002.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2002;}); _temp2001;});
break; case 122: _LL2000: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2004=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2004[ 0]=({ struct Cyc_Scope_tok_struct _temp2005; _temp2005.tag= Cyc_Scope_tok;
_temp2005.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2005;}); _temp2004;});
break; case 123: _LL2003: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2007=( struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2007[ 0]=({ struct Cyc_TunionField_tok_struct _temp2008; _temp2008.tag= Cyc_TunionField_tok;
_temp2008.f1=({ struct Cyc_Absyn_Tunionfield* _temp2009=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2009->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2009->tvs= 0;
_temp2009->typs= 0; _temp2009->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2009->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2009;}); _temp2008;}); _temp2007;}); break; case 124:
_LL2006: { struct Cyc_List_List* _temp2011=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp2012=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp2013=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2013[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2014; _temp2014.tag= Cyc_TunionField_tok;
_temp2014.f1=({ struct Cyc_Absyn_Tunionfield* _temp2015=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2015->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2015->tvs=
_temp2012; _temp2015->typs= _temp2011; _temp2015->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2015->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp2015;}); _temp2014;}); _temp2013;}); break;}
case 125: _LL2010: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 126: _LL2016: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2018=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2018[ 0]=({ struct Cyc_Declarator_tok_struct _temp2019; _temp2019.tag= Cyc_Declarator_tok;
_temp2019.f1=({ struct Cyc_Parse_Declarator* _temp2020=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2020->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2020->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2020;});
_temp2019;}); _temp2018;}); break; case 127: _LL2017: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2022=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2022[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2023; _temp2023.tag= Cyc_Declarator_tok;
_temp2023.f1=({ struct Cyc_Parse_Declarator* _temp2024=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2024->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2024->tms= 0;
_temp2024;}); _temp2023;}); _temp2022;}); break; case 128: _LL2021: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 129:
_LL2025: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2027=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2027[ 0]=({ struct Cyc_Declarator_tok_struct _temp2028; _temp2028.tag= Cyc_Declarator_tok;
_temp2028.f1=({ struct Cyc_Parse_Declarator* _temp2029=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2029->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp2029->tms=({ struct Cyc_List_List* _temp2030=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2030->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2030->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp2030;}); _temp2029;}); _temp2028;});
_temp2027;}); break; case 130: _LL2026: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2032=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2032[ 0]=({ struct Cyc_Declarator_tok_struct _temp2033; _temp2033.tag= Cyc_Declarator_tok;
_temp2033.f1=({ struct Cyc_Parse_Declarator* _temp2034=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2034->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2034->tms=({ struct Cyc_List_List* _temp2035=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2035->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2036=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2036[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2037; _temp2037.tag= Cyc_Absyn_ConstArray_mod;
_temp2037.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2037;}); _temp2036;})); _temp2035->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2035;}); _temp2034;}); _temp2033;}); _temp2032;}); break; case 131: _LL2031: {
struct _tuple16 _temp2041; struct Cyc_List_List* _temp2042; struct Cyc_Core_Opt*
_temp2044; struct Cyc_Absyn_VarargInfo* _temp2046; int _temp2048; struct Cyc_List_List*
_temp2050; struct _tuple16* _temp2039= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2041=*
_temp2039; _LL2051: _temp2050= _temp2041.f1; goto _LL2049; _LL2049: _temp2048=
_temp2041.f2; goto _LL2047; _LL2047: _temp2046= _temp2041.f3; goto _LL2045;
_LL2045: _temp2044= _temp2041.f4; goto _LL2043; _LL2043: _temp2042= _temp2041.f5;
goto _LL2040; _LL2040: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2052=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2052[ 0]=({ struct Cyc_Declarator_tok_struct _temp2053; _temp2053.tag= Cyc_Declarator_tok;
_temp2053.f1=({ struct Cyc_Parse_Declarator* _temp2054=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2054->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2054->tms=({ struct Cyc_List_List* _temp2055=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2055->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2056=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2056[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2057; _temp2057.tag= Cyc_Absyn_Function_mod;
_temp2057.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2058=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2058[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2059; _temp2059.tag= Cyc_Absyn_WithTypes;
_temp2059.f1= _temp2050; _temp2059.f2= _temp2048; _temp2059.f3= _temp2046;
_temp2059.f4= _temp2044; _temp2059.f5= _temp2042; _temp2059;}); _temp2058;}));
_temp2057;}); _temp2056;})); _temp2055->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2055;});
_temp2054;}); _temp2053;}); _temp2052;}); break;} case 132: _LL2038: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2061=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2061[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2062; _temp2062.tag= Cyc_Declarator_tok;
_temp2062.f1=({ struct Cyc_Parse_Declarator* _temp2063=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2063->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2063->tms=({ struct Cyc_List_List* _temp2064=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2064->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2065=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2065[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2066; _temp2066.tag= Cyc_Absyn_Function_mod;
_temp2066.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2067=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2067[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2068; _temp2068.tag= Cyc_Absyn_WithTypes;
_temp2068.f1= 0; _temp2068.f2= 0; _temp2068.f3= 0; _temp2068.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2068.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2068;}); _temp2067;})); _temp2066;}); _temp2065;}));
_temp2064->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2064;}); _temp2063;}); _temp2062;});
_temp2061;}); break; case 133: _LL2060: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2070=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2070[ 0]=({ struct Cyc_Declarator_tok_struct _temp2071; _temp2071.tag= Cyc_Declarator_tok;
_temp2071.f1=({ struct Cyc_Parse_Declarator* _temp2072=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2072->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2072->tms=({ struct Cyc_List_List* _temp2073=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2073->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2074=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2074[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2075; _temp2075.tag= Cyc_Absyn_Function_mod;
_temp2075.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2076=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2076[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2077; _temp2077.tag= Cyc_Absyn_NoTypes;
_temp2077.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2077.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2077;});
_temp2076;})); _temp2075;}); _temp2074;})); _temp2073->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2073;}); _temp2072;}); _temp2071;}); _temp2070;}); break; case 134: _LL2069: {
struct Cyc_List_List* _temp2079=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2080=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2080[ 0]=({ struct Cyc_Declarator_tok_struct _temp2081; _temp2081.tag= Cyc_Declarator_tok;
_temp2081.f1=({ struct Cyc_Parse_Declarator* _temp2082=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2082->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2082->tms=({ struct Cyc_List_List* _temp2083=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2083->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2084=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2084[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2085; _temp2085.tag= Cyc_Absyn_TypeParams_mod;
_temp2085.f1= _temp2079; _temp2085.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2085.f3=
0; _temp2085;}); _temp2084;})); _temp2083->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2083;});
_temp2082;}); _temp2081;}); _temp2080;}); break;} case 135: _LL2078:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2087=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2088=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2088[ 0]=({ struct Cyc_Declarator_tok_struct _temp2089; _temp2089.tag= Cyc_Declarator_tok;
_temp2089.f1=({ struct Cyc_Parse_Declarator* _temp2090=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2090->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2090->tms=({ struct Cyc_List_List* _temp2091=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2091->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2092=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2092[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2093; _temp2093.tag= Cyc_Absyn_TypeParams_mod;
_temp2093.f1= _temp2087; _temp2093.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2093.f3=
0; _temp2093;}); _temp2092;})); _temp2091->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2091;});
_temp2090;}); _temp2089;}); _temp2088;}); break;} case 136: _LL2086: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2095=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2095[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2096; _temp2096.tag= Cyc_Declarator_tok;
_temp2096.f1=({ struct Cyc_Parse_Declarator* _temp2097=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2097->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2097->tms=({ struct Cyc_List_List* _temp2098=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2098->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2099=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2099[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2100; _temp2100.tag= Cyc_Absyn_Attributes_mod;
_temp2100.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2100.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2100;}); _temp2099;}));
_temp2098->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]))->tms; _temp2098;}); _temp2097;}); _temp2096;});
_temp2095;}); break; case 137: _LL2094: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2102=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2102[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2103; _temp2103.tag= Cyc_Declarator_tok;
_temp2103.f1=({ struct Cyc_Parse_Declarator* _temp2104=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2104->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2104->tms= 0;
_temp2104;}); _temp2103;}); _temp2102;}); break; case 138: _LL2101: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2106=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2106[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2107; _temp2107.tag= Cyc_Declarator_tok;
_temp2107.f1=({ struct Cyc_Parse_Declarator* _temp2108=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2108->id= Cyc_Absyn_exn_name;
_temp2108->tms= 0; _temp2108;}); _temp2107;}); _temp2106;}); break; case 139:
_LL2105: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2110=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2110[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2111; _temp2111.tag=
Cyc_TypeModifierList_tok; _temp2111.f1=({ struct Cyc_List_List* _temp2112=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2112->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2113=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2113[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2114; _temp2114.tag= Cyc_Absyn_Pointer_mod;
_temp2114.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2114.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2114.f3= Cyc_Absyn_empty_tqual(); _temp2114;});
_temp2113;})); _temp2112->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2112;});
_temp2111;}); _temp2110;}); break; case 140: _LL2109: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2116=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2116[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2117; _temp2117.tag= Cyc_TypeModifierList_tok;
_temp2117.f1=({ struct Cyc_List_List* _temp2118=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2118->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2119=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2119[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2120; _temp2120.tag= Cyc_Absyn_Pointer_mod;
_temp2120.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2120.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2120.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2120;}); _temp2119;}));
_temp2118->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0);
_temp2118;}); _temp2117;}); _temp2116;}); break; case 141: _LL2115: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2122=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2122[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2123; _temp2123.tag= Cyc_TypeModifierList_tok;
_temp2123.f1=({ struct Cyc_List_List* _temp2124=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2124->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2125=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2125[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2126; _temp2126.tag= Cyc_Absyn_Pointer_mod;
_temp2126.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2126.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2126.f3= Cyc_Absyn_empty_tqual(); _temp2126;});
_temp2125;})); _temp2124->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2124;});
_temp2123;}); _temp2122;}); break; case 142: _LL2121: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2128=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2128[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2129; _temp2129.tag= Cyc_TypeModifierList_tok;
_temp2129.f1=({ struct Cyc_List_List* _temp2130=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2130->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2131=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2131[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2132; _temp2132.tag= Cyc_Absyn_Pointer_mod;
_temp2132.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2132.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2132.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2132;});
_temp2131;})); _temp2130->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2130;});
_temp2129;}); _temp2128;}); break; case 143: _LL2127: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2134=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2134[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2135; _temp2135.tag= Cyc_Pointer_Sort_tok;
_temp2135.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2136=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2136[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2137; _temp2137.tag=
Cyc_Absyn_Nullable_ps; _temp2137.f1= Cyc_Absyn_exp_unsigned_one; _temp2137;});
_temp2136;})); _temp2135;}); _temp2134;}); break; case 144: _LL2133: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2139=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2139[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2140; _temp2140.tag= Cyc_Pointer_Sort_tok;
_temp2140.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2141=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2141[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2142; _temp2142.tag= Cyc_Absyn_NonNullable_ps; _temp2142.f1= Cyc_Absyn_exp_unsigned_one;
_temp2142;}); _temp2141;})); _temp2140;}); _temp2139;}); break; case 145:
_LL2138: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2144=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2144[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2145; _temp2145.tag=
Cyc_Pointer_Sort_tok; _temp2145.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2146=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2146[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2147; _temp2147.tag=
Cyc_Absyn_Nullable_ps; _temp2147.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2147;});
_temp2146;})); _temp2145;}); _temp2144;}); break; case 146: _LL2143: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2149=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2149[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2150; _temp2150.tag= Cyc_Pointer_Sort_tok;
_temp2150.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2151=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2151[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2152; _temp2152.tag= Cyc_Absyn_NonNullable_ps; _temp2152.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2152;});
_temp2151;})); _temp2150;}); _temp2149;}); break; case 147: _LL2148: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2154=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2154[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2155; _temp2155.tag= Cyc_Pointer_Sort_tok;
_temp2155.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2155;});
_temp2154;}); break; case 148: _LL2153: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2157=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2157[ 0]=({ struct Cyc_Type_tok_struct _temp2158; _temp2158.tag= Cyc_Type_tok;
_temp2158.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2159=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2159->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2159;}), 0); _temp2158;}); _temp2157;});
break; case 149: _LL2156: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 150: _LL2160: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
151: _LL2161: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2163=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2163[
0]=({ struct Cyc_Type_tok_struct _temp2164; _temp2164.tag= Cyc_Type_tok;
_temp2164.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2165=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2165->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2165;}), 0); _temp2164;}); _temp2163;});
break; case 152: _LL2162: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 153: _LL2166: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2168=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2168[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2169; _temp2169.tag= Cyc_TypeQual_tok; _temp2169.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2169;});
_temp2168;}); break; case 154: _LL2167: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2171=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2171[ 0]=({ struct Cyc_YY1_struct _temp2172; _temp2172.tag= Cyc_YY1;
_temp2172.f1=({ struct _tuple16* _temp2173=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2173->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2173->f2= 0;
_temp2173->f3= 0; _temp2173->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2173->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2173;});
_temp2172;}); _temp2171;}); break; case 155: _LL2170: yyval=( void*)({ struct
Cyc_YY1_struct* _temp2175=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct
Cyc_YY1_struct)); _temp2175[ 0]=({ struct Cyc_YY1_struct _temp2176; _temp2176.tag=
Cyc_YY1; _temp2176.f1=({ struct _tuple16* _temp2177=( struct _tuple16*)
_cycalloc( sizeof( struct _tuple16)); _temp2177->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2177->f2= 1;
_temp2177->f3= 0; _temp2177->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2177->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2177;});
_temp2176;}); _temp2175;}); break; case 156: _LL2174: { struct _tuple2 _temp2181;
void* _temp2182; struct Cyc_Absyn_Tqual _temp2184; struct Cyc_Core_Opt*
_temp2186; struct _tuple2* _temp2179= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2181=*
_temp2179; _LL2187: _temp2186= _temp2181.f1; goto _LL2185; _LL2185: _temp2184=
_temp2181.f2; goto _LL2183; _LL2183: _temp2182= _temp2181.f3; goto _LL2180;
_LL2180: { struct Cyc_Absyn_VarargInfo* _temp2188=({ struct Cyc_Absyn_VarargInfo*
_temp2192=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2192->name= _temp2186; _temp2192->tq= _temp2184; _temp2192->type=( void*)
_temp2182; _temp2192->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2192;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2189=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2189[ 0]=({ struct Cyc_YY1_struct
_temp2190; _temp2190.tag= Cyc_YY1; _temp2190.f1=({ struct _tuple16* _temp2191=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2191->f1= 0;
_temp2191->f2= 0; _temp2191->f3= _temp2188; _temp2191->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2191->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2191;}); _temp2190;}); _temp2189;}); break;}} case 157:
_LL2178: { struct _tuple2 _temp2196; void* _temp2197; struct Cyc_Absyn_Tqual
_temp2199; struct Cyc_Core_Opt* _temp2201; struct _tuple2* _temp2194= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2196=*
_temp2194; _LL2202: _temp2201= _temp2196.f1; goto _LL2200; _LL2200: _temp2199=
_temp2196.f2; goto _LL2198; _LL2198: _temp2197= _temp2196.f3; goto _LL2195;
_LL2195: { struct Cyc_Absyn_VarargInfo* _temp2203=({ struct Cyc_Absyn_VarargInfo*
_temp2207=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2207->name= _temp2201; _temp2207->tq= _temp2199; _temp2207->type=( void*)
_temp2197; _temp2207->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2207;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2204=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2204[ 0]=({ struct Cyc_YY1_struct
_temp2205; _temp2205.tag= Cyc_YY1; _temp2205.f1=({ struct _tuple16* _temp2206=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2206->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2206->f2=
0; _temp2206->f3= _temp2203; _temp2206->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2206->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2206;});
_temp2205;}); _temp2204;}); break;}} case 158: _LL2193: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2209=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2209[ 0]=({ struct Cyc_Type_tok_struct
_temp2210; _temp2210.tag= Cyc_Type_tok; _temp2210.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()); _temp2210;}); _temp2209;}); break; case 159: _LL2208:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2212=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2212[ 0]=({ struct Cyc_Type_tok_struct
_temp2213; _temp2213.tag= Cyc_Type_tok; _temp2213.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp2213;});
_temp2212;}); break; case 160: _LL2211: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2215=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2215[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2216; _temp2216.tag= Cyc_TypeOpt_tok;
_temp2216.f1= 0; _temp2216;}); _temp2215;}); break; case 161: _LL2214: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2218=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2218[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2219; _temp2219.tag= Cyc_TypeOpt_tok; _temp2219.f1=({ struct Cyc_Core_Opt*
_temp2220=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2220->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2221=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2221[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2222; _temp2222.tag= Cyc_Absyn_JoinEff;
_temp2222.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2222;}); _temp2221;})); _temp2220;}); _temp2219;});
_temp2218;}); break; case 162: _LL2217: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2224=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2224[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2225; _temp2225.tag= Cyc_Rgnorder_tok;
_temp2225.f1= 0; _temp2225;}); _temp2224;}); break; case 163: _LL2223: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 164:
_LL2226: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2228=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2228[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2229; _temp2229.tag= Cyc_Rgnorder_tok; _temp2229.f1=({ struct Cyc_List_List*
_temp2230=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2230->hd=( void*)({ struct _tuple18* _temp2231=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2231->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2231->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2231;}); _temp2230->tl= 0; _temp2230;});
_temp2229;}); _temp2228;}); break; case 165: _LL2227: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2233=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2233[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2234; _temp2234.tag= Cyc_Rgnorder_tok; _temp2234.f1=({ struct Cyc_List_List*
_temp2235=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2235->hd=( void*)({ struct _tuple18* _temp2236=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2236->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2236->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2236;}); _temp2235->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2235;});
_temp2234;}); _temp2233;}); break; case 166: _LL2232: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2238=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2238[ 0]=({ struct Cyc_Bool_tok_struct
_temp2239; _temp2239.tag= Cyc_Bool_tok; _temp2239.f1= 0; _temp2239;}); _temp2238;});
break; case 167: _LL2237: { struct _tagged_arr _temp2241= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Std_zstrcmp(
_temp2241, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2242=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2242[ 0]=({ struct Cyc_Bool_tok_struct _temp2243; _temp2243.tag= Cyc_Bool_tok;
_temp2243.f1= 1; _temp2243;}); _temp2242;}); break;} case 168: _LL2240: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 169:
_LL2244: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2246=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2246[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2247; _temp2247.tag= Cyc_TypeList_tok; _temp2247.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2247;});
_temp2246;}); break; case 170: _LL2245: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2249=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2249[ 0]=({ struct Cyc_TypeList_tok_struct _temp2250; _temp2250.tag= Cyc_TypeList_tok;
_temp2250.f1= 0; _temp2250;}); _temp2249;}); break; case 171: _LL2248: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case
172: _LL2251: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2253=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2253[ 0]=({ struct Cyc_TypeList_tok_struct _temp2254; _temp2254.tag= Cyc_TypeList_tok;
_temp2254.f1=({ struct Cyc_List_List* _temp2255=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2255->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp2256=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp2256[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp2257; _temp2257.tag= Cyc_Absyn_RgnsEff; _temp2257.f1=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2257;}); _temp2256;})); _temp2255->tl= 0; _temp2255;});
_temp2254;}); _temp2253;}); break; case 173: _LL2252: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2259=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2259[ 0]=({ struct Cyc_TypeList_tok_struct _temp2260; _temp2260.tag= Cyc_TypeList_tok;
_temp2260.f1=({ struct Cyc_List_List* _temp2261=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2261->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2261->tl= 0;
_temp2261;}); _temp2260;}); _temp2259;}); break; case 174: _LL2258: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2263=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2263[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2264; _temp2264.tag= Cyc_TypeList_tok; _temp2264.f1=({ struct Cyc_List_List*
_temp2265=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2265->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2266=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2266[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2267; _temp2267.tag= Cyc_Absyn_AccessEff;
_temp2267.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2267;}); _temp2266;})); _temp2265->tl= 0;
_temp2265;}); _temp2264;}); _temp2263;}); break; case 175: _LL2262: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),( void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2269=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2269[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2270; _temp2270.tag= Cyc_TypeList_tok; _temp2270.f1=({ struct Cyc_List_List*
_temp2271=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2271->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2272=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2272[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2273; _temp2273.tag= Cyc_Absyn_AccessEff;
_temp2273.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2273;}); _temp2272;})); _temp2271->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2271;});
_temp2270;}); _temp2269;}); break; case 176: _LL2268: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2275=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2275[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2276; _temp2276.tag= Cyc_ParamDeclList_tok;
_temp2276.f1=({ struct Cyc_List_List* _temp2277=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2277->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2277->tl= 0;
_temp2277;}); _temp2276;}); _temp2275;}); break; case 177: _LL2274: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2279=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2279[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2280; _temp2280.tag= Cyc_ParamDeclList_tok;
_temp2280.f1=({ struct Cyc_List_List* _temp2281=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2281->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2281->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2281;}); _temp2280;}); _temp2279;}); break; case 178:
_LL2278: { struct _tuple17 _temp2285; struct Cyc_List_List* _temp2286; struct
Cyc_List_List* _temp2288; struct Cyc_Absyn_Tqual _temp2290; struct _tuple17*
_temp2283= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2285=* _temp2283; _LL2291: _temp2290=
_temp2285.f1; goto _LL2289; _LL2289: _temp2288= _temp2285.f2; goto _LL2287;
_LL2287: _temp2286= _temp2285.f3; goto _LL2284; _LL2284: { struct Cyc_Parse_Declarator
_temp2294; struct Cyc_List_List* _temp2295; struct _tuple1* _temp2297; struct
Cyc_Parse_Declarator* _temp2292= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2294=* _temp2292;
_LL2298: _temp2297= _temp2294.id; goto _LL2296; _LL2296: _temp2295= _temp2294.tms;
goto _LL2293; _LL2293: { void* _temp2299= Cyc_Parse_speclist2typ( _temp2288, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2302; struct Cyc_List_List* _temp2304; void*
_temp2306; struct Cyc_Absyn_Tqual _temp2308; struct _tuple6 _temp2300= Cyc_Parse_apply_tms(
_temp2290, _temp2299, _temp2286, _temp2295); _LL2309: _temp2308= _temp2300.f1;
goto _LL2307; _LL2307: _temp2306= _temp2300.f2; goto _LL2305; _LL2305: _temp2304=
_temp2300.f3; goto _LL2303; _LL2303: _temp2302= _temp2300.f4; goto _LL2301;
_LL2301: if( _temp2304 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2297)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2310=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2314=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2314->v=( void*)(* _temp2297).f2; _temp2314;}); if( _temp2302 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2311=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2311[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2312; _temp2312.tag= Cyc_ParamDecl_tok;
_temp2312.f1=({ struct _tuple2* _temp2313=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2313->f1= _temp2310; _temp2313->f2= _temp2308; _temp2313->f3=
_temp2306; _temp2313;}); _temp2312;}); _temp2311;}); break;}}}} case 179:
_LL2282: { struct _tuple17 _temp2318; struct Cyc_List_List* _temp2319; struct
Cyc_List_List* _temp2321; struct Cyc_Absyn_Tqual _temp2323; struct _tuple17*
_temp2316= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2318=* _temp2316; _LL2324: _temp2323= _temp2318.f1;
goto _LL2322; _LL2322: _temp2321= _temp2318.f2; goto _LL2320; _LL2320: _temp2319=
_temp2318.f3; goto _LL2317; _LL2317: { void* _temp2325= Cyc_Parse_speclist2typ(
_temp2321, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2319 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2326=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2326[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2327; _temp2327.tag= Cyc_ParamDecl_tok; _temp2327.f1=({
struct _tuple2* _temp2328=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2328->f1= 0; _temp2328->f2= _temp2323; _temp2328->f3= _temp2325; _temp2328;});
_temp2327;}); _temp2326;}); break;}} case 180: _LL2315: { struct _tuple17
_temp2332; struct Cyc_List_List* _temp2333; struct Cyc_List_List* _temp2335;
struct Cyc_Absyn_Tqual _temp2337; struct _tuple17* _temp2330= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2332=*
_temp2330; _LL2338: _temp2337= _temp2332.f1; goto _LL2336; _LL2336: _temp2335=
_temp2332.f2; goto _LL2334; _LL2334: _temp2333= _temp2332.f3; goto _LL2331;
_LL2331: { void* _temp2339= Cyc_Parse_speclist2typ( _temp2335, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2340=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2343; struct Cyc_List_List* _temp2345; void* _temp2347; struct Cyc_Absyn_Tqual
_temp2349; struct _tuple6 _temp2341= Cyc_Parse_apply_tms( _temp2337, _temp2339,
_temp2333, _temp2340); _LL2350: _temp2349= _temp2341.f1; goto _LL2348; _LL2348:
_temp2347= _temp2341.f2; goto _LL2346; _LL2346: _temp2345= _temp2341.f3; goto
_LL2344; _LL2344: _temp2343= _temp2341.f4; goto _LL2342; _LL2342: if( _temp2345
!=  0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2343 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2351=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2351[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2352; _temp2352.tag= Cyc_ParamDecl_tok; _temp2352.f1=({
struct _tuple2* _temp2353=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2353->f1= 0; _temp2353->f2= _temp2349; _temp2353->f3= _temp2347; _temp2353;});
_temp2352;}); _temp2351;}); break;}} case 181: _LL2329: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2355=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2355[ 0]=({ struct Cyc_IdList_tok_struct
_temp2356; _temp2356.tag= Cyc_IdList_tok; _temp2356.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2356;});
_temp2355;}); break; case 182: _LL2354: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2358=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2358[ 0]=({ struct Cyc_IdList_tok_struct _temp2359; _temp2359.tag= Cyc_IdList_tok;
_temp2359.f1=({ struct Cyc_List_List* _temp2360=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2360->hd=( void*)({ struct
_tagged_arr* _temp2361=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2361[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2361;}); _temp2360->tl=
0; _temp2360;}); _temp2359;}); _temp2358;}); break; case 183: _LL2357: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2363=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2363[ 0]=({ struct Cyc_IdList_tok_struct
_temp2364; _temp2364.tag= Cyc_IdList_tok; _temp2364.f1=({ struct Cyc_List_List*
_temp2365=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2365->hd=( void*)({ struct _tagged_arr* _temp2366=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2366[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2366;});
_temp2365->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2365;}); _temp2364;}); _temp2363;}); break; case 184:
_LL2362: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 185: _LL2367: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 186: _LL2368: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2370=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2370[ 0]=({ struct Cyc_Exp_tok_struct _temp2371; _temp2371.tag= Cyc_Exp_tok;
_temp2371.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2372=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2372[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2373; _temp2373.tag= Cyc_Absyn_UnresolvedMem_e; _temp2373.f1= 0; _temp2373.f2=
0; _temp2373;}); _temp2372;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2371;});
_temp2370;}); break; case 187: _LL2369: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2375=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2375[ 0]=({ struct Cyc_Exp_tok_struct _temp2376; _temp2376.tag= Cyc_Exp_tok;
_temp2376.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2377=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2377[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2378; _temp2378.tag= Cyc_Absyn_UnresolvedMem_e; _temp2378.f1= 0; _temp2378.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2378;});
_temp2377;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2376;}); _temp2375;}); break; case 188:
_LL2374: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2380=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2380[ 0]=({ struct Cyc_Exp_tok_struct
_temp2381; _temp2381.tag= Cyc_Exp_tok; _temp2381.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2382=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2382[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2383; _temp2383.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2383.f1= 0; _temp2383.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2383;});
_temp2382;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2381;}); _temp2380;}); break; case 189:
_LL2379: { struct Cyc_Absyn_Vardecl* _temp2385= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2391=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2391->f1=( void*) Cyc_Absyn_Loc_n; _temp2391->f2=({ struct _tagged_arr*
_temp2392=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp2392[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); _temp2392;}); _temp2391;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp2385->tq=({ struct Cyc_Absyn_Tqual _temp2386; _temp2386.q_const= 1;
_temp2386.q_volatile= 0; _temp2386.q_restrict= 1; _temp2386;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2387=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2387[ 0]=({ struct Cyc_Exp_tok_struct
_temp2388; _temp2388.tag= Cyc_Exp_tok; _temp2388.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2389=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2389[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2390; _temp2390.tag= Cyc_Absyn_Comprehension_e;
_temp2390.f1= _temp2385; _temp2390.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2390.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2390;});
_temp2389;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2388;}); _temp2387;}); break;} case
190: _LL2384: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2394=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2394[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2395; _temp2395.tag=
Cyc_InitializerList_tok; _temp2395.f1=({ struct Cyc_List_List* _temp2396=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2396->hd=(
void*)({ struct _tuple19* _temp2397=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2397->f1= 0; _temp2397->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2397;}); _temp2396->tl=
0; _temp2396;}); _temp2395;}); _temp2394;}); break; case 191: _LL2393: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2399=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2399[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2400; _temp2400.tag= Cyc_InitializerList_tok;
_temp2400.f1=({ struct Cyc_List_List* _temp2401=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2401->hd=( void*)({ struct
_tuple19* _temp2402=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2402->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2402->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2402;});
_temp2401->tl= 0; _temp2401;}); _temp2400;}); _temp2399;}); break; case 192:
_LL2398: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2404=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2404[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2405; _temp2405.tag=
Cyc_InitializerList_tok; _temp2405.f1=({ struct Cyc_List_List* _temp2406=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2406->hd=(
void*)({ struct _tuple19* _temp2407=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2407->f1= 0; _temp2407->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2407;}); _temp2406->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2406;}); _temp2405;}); _temp2404;}); break; case 193:
_LL2403: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2409=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2409[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2410; _temp2410.tag=
Cyc_InitializerList_tok; _temp2410.f1=({ struct Cyc_List_List* _temp2411=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2411->hd=(
void*)({ struct _tuple19* _temp2412=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2412->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2412->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2412;});
_temp2411->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2411;});
_temp2410;}); _temp2409;}); break; case 194: _LL2408: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2414=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2414[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2415; _temp2415.tag= Cyc_DesignatorList_tok;
_temp2415.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2415;});
_temp2414;}); break; case 195: _LL2413: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2417=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2417[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2418; _temp2418.tag=
Cyc_DesignatorList_tok; _temp2418.f1=({ struct Cyc_List_List* _temp2419=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2419->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2419->tl= 0; _temp2419;}); _temp2418;}); _temp2417;});
break; case 196: _LL2416: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2421=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2421[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2422; _temp2422.tag=
Cyc_DesignatorList_tok; _temp2422.f1=({ struct Cyc_List_List* _temp2423=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2423->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2423->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2423;});
_temp2422;}); _temp2421;}); break; case 197: _LL2420: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2425=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2425[ 0]=({ struct
Cyc_Designator_tok_struct _temp2426; _temp2426.tag= Cyc_Designator_tok;
_temp2426.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2427=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2427[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2428; _temp2428.tag=
Cyc_Absyn_ArrayElement; _temp2428.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2428;});
_temp2427;})); _temp2426;}); _temp2425;}); break; case 198: _LL2424: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2430=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2430[ 0]=({ struct
Cyc_Designator_tok_struct _temp2431; _temp2431.tag= Cyc_Designator_tok;
_temp2431.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2432=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2432[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2433; _temp2433.tag= Cyc_Absyn_FieldName;
_temp2433.f1=({ struct _tagged_arr* _temp2434=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2434[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2434;}); _temp2433;});
_temp2432;})); _temp2431;}); _temp2430;}); break; case 199: _LL2429: { void*
_temp2436= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2437=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2437 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2438=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2439=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2439[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2440; _temp2440.tag= Cyc_ParamDecl_tok; _temp2440.f1=({
struct _tuple2* _temp2441=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2441->f1= 0; _temp2441->f2= _temp2438; _temp2441->f3= _temp2436; _temp2441;});
_temp2440;}); _temp2439;}); break;}} case 200: _LL2435: { void* _temp2443= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp2444=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2445=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2446=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2447= Cyc_Parse_apply_tms( _temp2445, _temp2443, _temp2444,
_temp2446); if( _temp2447.f3 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2447.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2448=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2448[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2449; _temp2449.tag= Cyc_ParamDecl_tok; _temp2449.f1=({
struct _tuple2* _temp2450=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2450->f1= 0; _temp2450->f2= _temp2447.f1; _temp2450->f3= _temp2447.f2;
_temp2450;}); _temp2449;}); _temp2448;}); break;} case 201: _LL2442: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2452=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2452[ 0]=({ struct Cyc_Type_tok_struct
_temp2453; _temp2453.tag= Cyc_Type_tok; _temp2453.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2453;});
_temp2452;}); break; case 202: _LL2451: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2455=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2455[ 0]=({ struct Cyc_Type_tok_struct _temp2456; _temp2456.tag= Cyc_Type_tok;
_temp2456.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2457=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2457[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2458; _temp2458.tag= Cyc_Absyn_JoinEff;
_temp2458.f1= 0; _temp2458;}); _temp2457;})); _temp2456;}); _temp2455;}); break;
case 203: _LL2454: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2460=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2460[ 0]=({ struct Cyc_Type_tok_struct _temp2461; _temp2461.tag= Cyc_Type_tok;
_temp2461.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2462=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2462[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2463; _temp2463.tag= Cyc_Absyn_JoinEff;
_temp2463.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2463;}); _temp2462;})); _temp2461;});
_temp2460;}); break; case 204: _LL2459: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2465=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2465[ 0]=({ struct Cyc_Type_tok_struct _temp2466; _temp2466.tag= Cyc_Type_tok;
_temp2466.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2467=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2467[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2468; _temp2468.tag= Cyc_Absyn_RgnsEff;
_temp2468.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2468;}); _temp2467;})); _temp2466;});
_temp2465;}); break; case 205: _LL2464: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2470=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2470[ 0]=({ struct Cyc_Type_tok_struct _temp2471; _temp2471.tag= Cyc_Type_tok;
_temp2471.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2472=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2472[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2473; _temp2473.tag= Cyc_Absyn_JoinEff;
_temp2473.f1=({ struct Cyc_List_List* _temp2474=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2474->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2474->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2474;}); _temp2473;}); _temp2472;})); _temp2471;});
_temp2470;}); break; case 206: _LL2469: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2476=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2476[ 0]=({ struct Cyc_TypeList_tok_struct _temp2477; _temp2477.tag= Cyc_TypeList_tok;
_temp2477.f1=({ struct Cyc_List_List* _temp2478=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2478->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2478->tl= 0;
_temp2478;}); _temp2477;}); _temp2476;}); break; case 207: _LL2475: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2480=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2480[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2481; _temp2481.tag= Cyc_TypeList_tok; _temp2481.f1=({ struct Cyc_List_List*
_temp2482=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2482->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2482->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2482;});
_temp2481;}); _temp2480;}); break; case 208: _LL2479: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2484=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2484[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2485; _temp2485.tag= Cyc_AbstractDeclarator_tok;
_temp2485.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2486=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2486->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2486;});
_temp2485;}); _temp2484;}); break; case 209: _LL2483: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 210: _LL2487:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2489=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2489[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2490; _temp2490.tag=
Cyc_AbstractDeclarator_tok; _temp2490.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2491=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2491->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2491;});
_temp2490;}); _temp2489;}); break; case 211: _LL2488: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 212:
_LL2492: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2494=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2494[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2495; _temp2495.tag=
Cyc_AbstractDeclarator_tok; _temp2495.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2496=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2496->tms=({ struct Cyc_List_List* _temp2497=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2497->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2497->tl= 0; _temp2497;}); _temp2496;}); _temp2495;}); _temp2494;}); break;
case 213: _LL2493: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2499=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2499[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2500; _temp2500.tag= Cyc_AbstractDeclarator_tok; _temp2500.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2501=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2501->tms=({ struct Cyc_List_List* _temp2502=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2502->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2502->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2502;});
_temp2501;}); _temp2500;}); _temp2499;}); break; case 214: _LL2498: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2504=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2504[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2505; _temp2505.tag= Cyc_AbstractDeclarator_tok;
_temp2505.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2506=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2506->tms=({
struct Cyc_List_List* _temp2507=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2507->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2508=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2508[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2509; _temp2509.tag= Cyc_Absyn_ConstArray_mod; _temp2509.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2509;});
_temp2508;})); _temp2507->tl= 0; _temp2507;}); _temp2506;}); _temp2505;});
_temp2504;}); break; case 215: _LL2503: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2511=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2511[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2512; _temp2512.tag= Cyc_AbstractDeclarator_tok; _temp2512.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2513=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2513->tms=({ struct Cyc_List_List* _temp2514=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2514->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2515=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2515[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2516; _temp2516.tag= Cyc_Absyn_ConstArray_mod;
_temp2516.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2516;}); _temp2515;})); _temp2514->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2514;}); _temp2513;}); _temp2512;}); _temp2511;}); break; case 216: _LL2510:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2518=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2518[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2519; _temp2519.tag=
Cyc_AbstractDeclarator_tok; _temp2519.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2520=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2520->tms=({ struct Cyc_List_List* _temp2521=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2521->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2522=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2522[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2523; _temp2523.tag= Cyc_Absyn_Function_mod;
_temp2523.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2524=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2524[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2525; _temp2525.tag= Cyc_Absyn_WithTypes;
_temp2525.f1= 0; _temp2525.f2= 0; _temp2525.f3= 0; _temp2525.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2525.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2525;}); _temp2524;})); _temp2523;}); _temp2522;}));
_temp2521->tl= 0; _temp2521;}); _temp2520;}); _temp2519;}); _temp2518;}); break;
case 217: _LL2517: { struct _tuple16 _temp2529; struct Cyc_List_List* _temp2530;
struct Cyc_Core_Opt* _temp2532; struct Cyc_Absyn_VarargInfo* _temp2534; int
_temp2536; struct Cyc_List_List* _temp2538; struct _tuple16* _temp2527= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2529=*
_temp2527; _LL2539: _temp2538= _temp2529.f1; goto _LL2537; _LL2537: _temp2536=
_temp2529.f2; goto _LL2535; _LL2535: _temp2534= _temp2529.f3; goto _LL2533;
_LL2533: _temp2532= _temp2529.f4; goto _LL2531; _LL2531: _temp2530= _temp2529.f5;
goto _LL2528; _LL2528: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2540=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2540[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2541; _temp2541.tag= Cyc_AbstractDeclarator_tok; _temp2541.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2542=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2542->tms=({ struct Cyc_List_List* _temp2543=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2543->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2544=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2544[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2545; _temp2545.tag= Cyc_Absyn_Function_mod;
_temp2545.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2546=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2546[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2547; _temp2547.tag= Cyc_Absyn_WithTypes;
_temp2547.f1= _temp2538; _temp2547.f2= _temp2536; _temp2547.f3= _temp2534;
_temp2547.f4= _temp2532; _temp2547.f5= _temp2530; _temp2547;}); _temp2546;}));
_temp2545;}); _temp2544;})); _temp2543->tl= 0; _temp2543;}); _temp2542;});
_temp2541;}); _temp2540;}); break;} case 218: _LL2526: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2549=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2549[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2550; _temp2550.tag= Cyc_AbstractDeclarator_tok;
_temp2550.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2551=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2551->tms=({
struct Cyc_List_List* _temp2552=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2552->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2553=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2553[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2554; _temp2554.tag=
Cyc_Absyn_Function_mod; _temp2554.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2555=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2555[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2556; _temp2556.tag= Cyc_Absyn_WithTypes;
_temp2556.f1= 0; _temp2556.f2= 0; _temp2556.f3= 0; _temp2556.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2556.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2556;}); _temp2555;})); _temp2554;}); _temp2553;}));
_temp2552->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2552;});
_temp2551;}); _temp2550;}); _temp2549;}); break; case 219: _LL2548: { struct
_tuple16 _temp2560; struct Cyc_List_List* _temp2561; struct Cyc_Core_Opt*
_temp2563; struct Cyc_Absyn_VarargInfo* _temp2565; int _temp2567; struct Cyc_List_List*
_temp2569; struct _tuple16* _temp2558= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2560=*
_temp2558; _LL2570: _temp2569= _temp2560.f1; goto _LL2568; _LL2568: _temp2567=
_temp2560.f2; goto _LL2566; _LL2566: _temp2565= _temp2560.f3; goto _LL2564;
_LL2564: _temp2563= _temp2560.f4; goto _LL2562; _LL2562: _temp2561= _temp2560.f5;
goto _LL2559; _LL2559: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2571=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2571[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2572; _temp2572.tag= Cyc_AbstractDeclarator_tok; _temp2572.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2573=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2573->tms=({ struct Cyc_List_List* _temp2574=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2574->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2575=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2575[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2576; _temp2576.tag= Cyc_Absyn_Function_mod;
_temp2576.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2577=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2577[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2578; _temp2578.tag= Cyc_Absyn_WithTypes;
_temp2578.f1= _temp2569; _temp2578.f2= _temp2567; _temp2578.f3= _temp2565;
_temp2578.f4= _temp2563; _temp2578.f5= _temp2561; _temp2578;}); _temp2577;}));
_temp2576;}); _temp2575;})); _temp2574->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2574;}); _temp2573;}); _temp2572;}); _temp2571;}); break;} case 220:
_LL2557: { struct Cyc_List_List* _temp2580=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2581=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2581[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2582; _temp2582.tag= Cyc_AbstractDeclarator_tok; _temp2582.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2583=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2583->tms=({ struct Cyc_List_List* _temp2584=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2584->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2585=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2585[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2586; _temp2586.tag= Cyc_Absyn_TypeParams_mod;
_temp2586.f1= _temp2580; _temp2586.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2586.f3=
0; _temp2586;}); _temp2585;})); _temp2584->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2584;}); _temp2583;}); _temp2582;}); _temp2581;}); break;} case 221:
_LL2579:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2588=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2589=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2589[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2590; _temp2590.tag= Cyc_AbstractDeclarator_tok; _temp2590.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2591=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2591->tms=({ struct Cyc_List_List* _temp2592=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2592->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2593=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2593[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2594; _temp2594.tag= Cyc_Absyn_TypeParams_mod;
_temp2594.f1= _temp2588; _temp2594.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2594.f3=
0; _temp2594;}); _temp2593;})); _temp2592->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2592;}); _temp2591;}); _temp2590;}); _temp2589;}); break;} case 222:
_LL2587: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2596=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2596[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2597; _temp2597.tag=
Cyc_AbstractDeclarator_tok; _temp2597.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2598=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2598->tms=({ struct Cyc_List_List* _temp2599=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2599->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2600=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2600[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2601; _temp2601.tag= Cyc_Absyn_Attributes_mod;
_temp2601.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2601.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2601;}); _temp2600;}));
_temp2599->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms; _temp2599;});
_temp2598;}); _temp2597;}); _temp2596;}); break; case 223: _LL2595: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 224: _LL2602:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
225: _LL2603: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 226: _LL2604: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 227: _LL2605: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 228: _LL2606:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
229: _LL2607: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2619=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2619->name=({ struct
_tagged_arr* _temp2620=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2620[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2620;});
_temp2619->identity= 0; _temp2619->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2619;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2617=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2617[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2618; _temp2618.tag= Cyc_Absyn_VarType;
_temp2618.f1= tv; _temp2618;}); _temp2617;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2609=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2609[ 0]=({ struct Cyc_Stmt_tok_struct _temp2610; _temp2610.tag= Cyc_Stmt_tok;
_temp2610.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2611=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2611[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2612; _temp2612.tag= Cyc_Absyn_Region_s;
_temp2612.f1= tv; _temp2612.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2613=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2613->f1=(
void*) Cyc_Absyn_Loc_n; _temp2613->f2=({ struct _tagged_arr* _temp2614=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2614[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2614;});
_temp2613;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2615=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2615[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2616; _temp2616.tag=
Cyc_Absyn_RgnHandleType; _temp2616.f1=( void*) t; _temp2616;}); _temp2615;}), 0);
_temp2612.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2612;}); _temp2611;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2610;});
_temp2609;}); break;} case 230: _LL2608: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2632=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2632->name=({ struct
_tagged_arr* _temp2633=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2633[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2635; _temp2635.tag= Cyc_Std_String_pa; _temp2635.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2634[ 1u]={& _temp2635}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2634, sizeof( void*), 1u));}});
_temp2633;}); _temp2632->identity= 0; _temp2632->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2632;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2630=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2630[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2631; _temp2631.tag= Cyc_Absyn_VarType;
_temp2631.f1= tv; _temp2631;}); _temp2630;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2622=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2622[ 0]=({ struct Cyc_Stmt_tok_struct _temp2623; _temp2623.tag= Cyc_Stmt_tok;
_temp2623.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2624=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2624[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2625; _temp2625.tag= Cyc_Absyn_Region_s;
_temp2625.f1= tv; _temp2625.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2626=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2626->f1=(
void*) Cyc_Absyn_Loc_n; _temp2626->f2=({ struct _tagged_arr* _temp2627=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2627[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2627;});
_temp2626;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2628=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2628[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2629; _temp2629.tag=
Cyc_Absyn_RgnHandleType; _temp2629.f1=( void*) t; _temp2629;}); _temp2628;}), 0);
_temp2625.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2625;}); _temp2624;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2623;});
_temp2622;}); break;} case 231: _LL2621: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2637=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2637[ 0]=({ struct Cyc_Stmt_tok_struct _temp2638; _temp2638.tag= Cyc_Stmt_tok;
_temp2638.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2639=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2639[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2640; _temp2640.tag= Cyc_Absyn_Cut_s;
_temp2640.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2640;}); _temp2639;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2638;});
_temp2637;}); break; case 232: _LL2636: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2642=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2642[ 0]=({ struct Cyc_Stmt_tok_struct _temp2643; _temp2643.tag= Cyc_Stmt_tok;
_temp2643.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2644=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2644[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2645; _temp2645.tag= Cyc_Absyn_Splice_s;
_temp2645.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2645;}); _temp2644;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2643;});
_temp2642;}); break; case 233: _LL2641: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2647=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2647[ 0]=({ struct Cyc_Stmt_tok_struct _temp2648; _temp2648.tag= Cyc_Stmt_tok;
_temp2648.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2649=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2649[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2650; _temp2650.tag= Cyc_Absyn_Label_s;
_temp2650.f1=({ struct _tagged_arr* _temp2651=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2651[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2651;});
_temp2650.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2650;}); _temp2649;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2648;});
_temp2647;}); break; case 234: _LL2646: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2653=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2653[ 0]=({ struct Cyc_Stmt_tok_struct _temp2654; _temp2654.tag= Cyc_Stmt_tok;
_temp2654.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2654;});
_temp2653;}); break; case 235: _LL2652: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2656=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2656[ 0]=({ struct Cyc_Stmt_tok_struct _temp2657; _temp2657.tag= Cyc_Stmt_tok;
_temp2657.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2657;}); _temp2656;}); break; case 236: _LL2655: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2659=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2659[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2660; _temp2660.tag= Cyc_Stmt_tok; _temp2660.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2660;}); _temp2659;}); break; case 237: _LL2658: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 238:
_LL2661: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2663=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2663[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2664; _temp2664.tag= Cyc_Stmt_tok; _temp2664.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2664;}); _temp2663;}); break;
case 239: _LL2662: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2666=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2666[ 0]=({ struct Cyc_Stmt_tok_struct _temp2667; _temp2667.tag= Cyc_Stmt_tok;
_temp2667.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2667;});
_temp2666;}); break; case 240: _LL2665: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 241: _LL2668:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2670=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2670[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2671; _temp2671.tag= Cyc_Stmt_tok; _temp2671.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2671;}); _temp2670;}); break; case 242: _LL2669: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2673=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2673[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2674; _temp2674.tag= Cyc_Stmt_tok; _temp2674.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2675=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2675[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2676; _temp2676.tag= Cyc_Absyn_Fn_d;
_temp2676.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2676;}); _temp2675;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2674;}); _temp2673;}); break; case 243: _LL2672: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2678=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2678[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2679; _temp2679.tag= Cyc_Stmt_tok; _temp2679.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2680=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2680[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2681; _temp2681.tag= Cyc_Absyn_Fn_d;
_temp2681.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2681;}); _temp2680;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2679;}); _temp2678;}); break; case 244: _LL2677: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2683=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2683[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2684; _temp2684.tag= Cyc_Stmt_tok; _temp2684.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2684;});
_temp2683;}); break; case 245: _LL2682: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2686=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2686[ 0]=({ struct Cyc_Stmt_tok_struct _temp2687; _temp2687.tag= Cyc_Stmt_tok;
_temp2687.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2687;}); _temp2686;}); break; case 246: _LL2685: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2689=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2689[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2690; _temp2690.tag= Cyc_Stmt_tok; _temp2690.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2690;}); _temp2689;}); break; case 247: _LL2688: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2692=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2692[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2693; _temp2693.tag= Cyc_Stmt_tok; _temp2693.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2694=( struct Cyc_Absyn_SwitchC_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2694[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2695; _temp2695.tag= Cyc_Absyn_SwitchC_s;
_temp2695.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2695.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2695;});
_temp2694;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2693;}); _temp2692;}); break; case 248:
_LL2691: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2697=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2697[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2698; _temp2698.tag= Cyc_Stmt_tok; _temp2698.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2698;});
_temp2697;}); break; case 249: _LL2696: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2700=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2700[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2701; _temp2701.tag= Cyc_SwitchClauseList_tok; _temp2701.f1= 0; _temp2701;});
_temp2700;}); break; case 250: _LL2699: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2703=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2703[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2704; _temp2704.tag= Cyc_SwitchClauseList_tok; _temp2704.f1=({ struct Cyc_List_List*
_temp2705=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2705->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2706=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2706->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2706->pat_vars= 0; _temp2706->where_clause= 0; _temp2706->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2706->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2706;}); _temp2705->tl= 0; _temp2705;});
_temp2704;}); _temp2703;}); break; case 251: _LL2702: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2708=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2708[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2709; _temp2709.tag= Cyc_SwitchClauseList_tok;
_temp2709.f1=({ struct Cyc_List_List* _temp2710=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2710->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2711=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2711->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2711->pat_vars= 0; _temp2711->where_clause= 0;
_temp2711->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2711->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2711;});
_temp2710->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2710;}); _temp2709;});
_temp2708;}); break; case 252: _LL2707: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2713=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2713[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2714; _temp2714.tag= Cyc_SwitchClauseList_tok; _temp2714.f1=({ struct Cyc_List_List*
_temp2715=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2715->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2716=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2716->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2716->pat_vars=
0; _temp2716->where_clause= 0; _temp2716->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2716->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2716;}); _temp2715->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2715;});
_temp2714;}); _temp2713;}); break; case 253: _LL2712: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2718=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2718[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2719; _temp2719.tag= Cyc_SwitchClauseList_tok;
_temp2719.f1=({ struct Cyc_List_List* _temp2720=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2720->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2721=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2721->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp2721->pat_vars= 0; _temp2721->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2721->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2721->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2721;});
_temp2720->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2720;}); _temp2719;});
_temp2718;}); break; case 254: _LL2717: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2723=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2723[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2724; _temp2724.tag= Cyc_SwitchClauseList_tok; _temp2724.f1=({ struct Cyc_List_List*
_temp2725=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2725->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2726=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2726->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2726->pat_vars=
0; _temp2726->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2726->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2726->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2726;});
_temp2725->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2725;}); _temp2724;});
_temp2723;}); break; case 255: _LL2722: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2728=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2728[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2729; _temp2729.tag= Cyc_SwitchCClauseList_tok; _temp2729.f1= 0; _temp2729;});
_temp2728;}); break; case 256: _LL2727: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2731=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2731[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2732; _temp2732.tag= Cyc_SwitchCClauseList_tok; _temp2732.f1=({ struct Cyc_List_List*
_temp2733=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2733->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2734=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2734->cnst_exp= 0;
_temp2734->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2734->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2734;});
_temp2733->tl= 0; _temp2733;}); _temp2732;}); _temp2731;}); break; case 257:
_LL2730: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2736=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2736[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2737; _temp2737.tag=
Cyc_SwitchCClauseList_tok; _temp2737.f1=({ struct Cyc_List_List* _temp2738=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2738->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2739=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2739->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2739->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2739->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2739;});
_temp2738->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2738;}); _temp2737;});
_temp2736;}); break; case 258: _LL2735: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2741=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2741[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2742; _temp2742.tag= Cyc_SwitchCClauseList_tok; _temp2742.f1=({ struct Cyc_List_List*
_temp2743=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2743->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2744=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2744->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2744->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2744->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2744;});
_temp2743->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2743;}); _temp2742;});
_temp2741;}); break; case 259: _LL2740: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2746=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2746[ 0]=({ struct Cyc_Stmt_tok_struct _temp2747; _temp2747.tag= Cyc_Stmt_tok;
_temp2747.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2747;}); _temp2746;}); break; case 260: _LL2745: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2749=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2749[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2750; _temp2750.tag= Cyc_Stmt_tok; _temp2750.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2750;}); _temp2749;}); break; case 261: _LL2748: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2752=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2752[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2753; _temp2753.tag= Cyc_Stmt_tok; _temp2753.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2753;}); _temp2752;}); break; case 262: _LL2751: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2755=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2755[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2756; _temp2756.tag= Cyc_Stmt_tok; _temp2756.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2756;}); _temp2755;}); break; case 263: _LL2754: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2758=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2758[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2759; _temp2759.tag= Cyc_Stmt_tok; _temp2759.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2759;}); _temp2758;}); break; case 264: _LL2757: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2761=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2761[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2762; _temp2762.tag= Cyc_Stmt_tok; _temp2762.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2762;}); _temp2761;}); break; case 265: _LL2760: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2764=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2764[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2765; _temp2765.tag= Cyc_Stmt_tok; _temp2765.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2765;}); _temp2764;}); break; case 266: _LL2763: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2767=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2767[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2768; _temp2768.tag= Cyc_Stmt_tok; _temp2768.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2768;});
_temp2767;}); break; case 267: _LL2766: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2770=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2770[ 0]=({ struct Cyc_Stmt_tok_struct _temp2771; _temp2771.tag= Cyc_Stmt_tok;
_temp2771.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2771;});
_temp2770;}); break; case 268: _LL2769: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2773=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2773[ 0]=({ struct Cyc_Stmt_tok_struct _temp2774; _temp2774.tag= Cyc_Stmt_tok;
_temp2774.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2774;}); _temp2773;}); break; case 269: _LL2772: { struct Cyc_List_List*
_temp2776= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt* _temp2777= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2778=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2778[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2779; _temp2779.tag= Cyc_Stmt_tok; _temp2779.f1= Cyc_Parse_flatten_declarations(
_temp2776, _temp2777); _temp2779;}); _temp2778;}); break;} case 270: _LL2775: {
struct Cyc_List_List* _temp2781= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2782= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2783=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2783[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2784; _temp2784.tag= Cyc_Stmt_tok; _temp2784.f1= Cyc_Parse_flatten_declarations(
_temp2781, _temp2782); _temp2784;}); _temp2783;}); break;} case 271: _LL2780: {
struct Cyc_List_List* _temp2786= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2787= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2788=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2788[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2789; _temp2789.tag= Cyc_Stmt_tok; _temp2789.f1= Cyc_Parse_flatten_declarations(
_temp2786, _temp2787); _temp2789;}); _temp2788;}); break;} case 272: _LL2785: {
struct Cyc_List_List* _temp2791= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2792= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2793=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2793[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2794; _temp2794.tag= Cyc_Stmt_tok; _temp2794.f1= Cyc_Parse_flatten_declarations(
_temp2791, _temp2792); _temp2794;}); _temp2793;}); break;} case 273: _LL2790:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2796=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2796[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2797; _temp2797.tag= Cyc_Stmt_tok; _temp2797.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2798=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2798[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2798;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2797;}); _temp2796;}); break; case 274: _LL2795: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2800=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2800[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2801; _temp2801.tag= Cyc_Stmt_tok; _temp2801.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2801;}); _temp2800;}); break; case 275:
_LL2799: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2803=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2803[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2804; _temp2804.tag= Cyc_Stmt_tok; _temp2804.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2804;}); _temp2803;}); break; case 276: _LL2802: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2806=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2806[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2807; _temp2807.tag= Cyc_Stmt_tok; _temp2807.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2807;}); _temp2806;}); break; case 277:
_LL2805: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2809=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2809[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2810; _temp2810.tag= Cyc_Stmt_tok; _temp2810.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2810;}); _temp2809;}); break; case 278: _LL2808: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2812=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2812[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2813; _temp2813.tag= Cyc_Stmt_tok; _temp2813.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2813;}); _temp2812;}); break; case 279:
_LL2811: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2815=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2815[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2816; _temp2816.tag= Cyc_Stmt_tok; _temp2816.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2816;}); _temp2815;}); break; case 280:
_LL2814: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2818=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2818[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2819; _temp2819.tag= Cyc_Stmt_tok; _temp2819.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2819;}); _temp2818;}); break; case 281: _LL2817: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2821=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2821[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2822; _temp2822.tag= Cyc_Pattern_tok; _temp2822.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2822;});
_temp2821;}); break; case 282: _LL2820: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 283:
_LL2823: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2825=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2825[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2826; _temp2826.tag= Cyc_Pattern_tok; _temp2826.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2827=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2827[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2828; _temp2828.tag= Cyc_Absyn_Int_p; _temp2828.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2828.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2828;}); _temp2827;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2826;});
_temp2825;}); break; case 284: _LL2824: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2830=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2830[ 0]=({ struct Cyc_Pattern_tok_struct _temp2831; _temp2831.tag= Cyc_Pattern_tok;
_temp2831.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2832=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2832[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2833; _temp2833.tag= Cyc_Absyn_Int_p;
_temp2833.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2833.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2833;});
_temp2832;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2831;}); _temp2830;}); break; case 285:
_LL2829: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2835=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2835[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2836; _temp2836.tag= Cyc_Pattern_tok; _temp2836.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2837=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2837[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2838; _temp2838.tag= Cyc_Absyn_Float_p; _temp2838.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2838;}); _temp2837;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2836;});
_temp2835;}); break; case 286: _LL2834: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2840=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2840[ 0]=({ struct Cyc_Pattern_tok_struct _temp2841; _temp2841.tag= Cyc_Pattern_tok;
_temp2841.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2842=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2842[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2843; _temp2843.tag= Cyc_Absyn_Char_p; _temp2843.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2843;});
_temp2842;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2841;}); _temp2840;}); break; case 287:
_LL2839: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2845=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2845[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2846; _temp2846.tag= Cyc_Pattern_tok; _temp2846.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2846;});
_temp2845;}); break; case 288: _LL2844: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2848=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2848[ 0]=({ struct Cyc_Pattern_tok_struct _temp2849; _temp2849.tag= Cyc_Pattern_tok;
_temp2849.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2850=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2850[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2851; _temp2851.tag=
Cyc_Absyn_UnknownId_p; _temp2851.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2851;}); _temp2850;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2849;}); _temp2848;}); break; case 289:
_LL2847: { struct Cyc_List_List* _temp2853=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2854=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2854[ 0]=({ struct Cyc_Pattern_tok_struct _temp2855; _temp2855.tag= Cyc_Pattern_tok;
_temp2855.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2856=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2856[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2857; _temp2857.tag= Cyc_Absyn_UnknownCall_p; _temp2857.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2857.f2=
_temp2853; _temp2857.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2857;});
_temp2856;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2855;}); _temp2854;}); break;} case
290: _LL2852: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2859=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2859[ 0]=({ struct Cyc_Pattern_tok_struct _temp2860; _temp2860.tag= Cyc_Pattern_tok;
_temp2860.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2861=( struct Cyc_Absyn_Tuple_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2861[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2862; _temp2862.tag= Cyc_Absyn_Tuple_p;
_temp2862.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2862;}); _temp2861;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2860;}); _temp2859;}); break; case 291: _LL2858: { struct Cyc_List_List*
_temp2864=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); yyval=(
void*)({ struct Cyc_Pattern_tok_struct* _temp2865=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2865[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2866; _temp2866.tag= Cyc_Pattern_tok; _temp2866.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2867=( struct Cyc_Absyn_UnknownFields_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2867[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2868; _temp2868.tag= Cyc_Absyn_UnknownFields_p;
_temp2868.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2868.f2= _temp2864; _temp2868.f3= 0; _temp2868;});
_temp2867;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2866;}); _temp2865;}); break;} case
292: _LL2863: { struct Cyc_List_List* _temp2870=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2871=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2871[ 0]=({ struct Cyc_Pattern_tok_struct _temp2872; _temp2872.tag= Cyc_Pattern_tok;
_temp2872.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2873=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2873[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2874; _temp2874.tag= Cyc_Absyn_UnknownFields_p; _temp2874.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2874.f2=
_temp2870; _temp2874.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2874;});
_temp2873;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2872;}); _temp2871;}); break;} case
293: _LL2869: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2876=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2876[ 0]=({ struct Cyc_Pattern_tok_struct _temp2877; _temp2877.tag= Cyc_Pattern_tok;
_temp2877.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2878=( struct Cyc_Absyn_Pointer_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2878[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2879; _temp2879.tag= Cyc_Absyn_Pointer_p;
_temp2879.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2879;}); _temp2878;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2877;});
_temp2876;}); break; case 294: _LL2875: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2881=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2881[ 0]=({ struct Cyc_Pattern_tok_struct _temp2882; _temp2882.tag= Cyc_Pattern_tok;
_temp2882.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2883=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2883[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2884; _temp2884.tag=
Cyc_Absyn_Reference_p; _temp2884.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2885=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2885->f1=(
void*) Cyc_Absyn_Loc_n; _temp2885->f2=({ struct _tagged_arr* _temp2886=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2886[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2886;});
_temp2885;}),( void*) Cyc_Absyn_VoidType, 0); _temp2884;}); _temp2883;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2882;}); _temp2881;}); break; case 295: _LL2880: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2888=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2888[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2889; _temp2889.tag= Cyc_PatternList_tok;
_temp2889.f1= 0; _temp2889;}); _temp2888;}); break; case 296: _LL2887: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2891=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2891[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2892; _temp2892.tag= Cyc_PatternList_tok;
_temp2892.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2892;}); _temp2891;}); break; case 297: _LL2890: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2894=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2894[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2895; _temp2895.tag= Cyc_PatternList_tok;
_temp2895.f1=({ struct Cyc_List_List* _temp2896=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2896->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2896->tl= 0;
_temp2896;}); _temp2895;}); _temp2894;}); break; case 298: _LL2893: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2898=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2898[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2899; _temp2899.tag= Cyc_PatternList_tok;
_temp2899.f1=({ struct Cyc_List_List* _temp2900=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2900->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2900->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2900;}); _temp2899;}); _temp2898;}); break; case 299:
_LL2897: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2902=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2902[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2903; _temp2903.tag=
Cyc_FieldPattern_tok; _temp2903.f1=({ struct _tuple13* _temp2904=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2904->f1= 0; _temp2904->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2904;}); _temp2903;}); _temp2902;}); break; case 300:
_LL2901: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2906=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2906[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2907; _temp2907.tag=
Cyc_FieldPattern_tok; _temp2907.f1=({ struct _tuple13* _temp2908=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2908->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2908->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2908;}); _temp2907;}); _temp2906;}); break; case 301:
_LL2905: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2910=(
struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2910[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2911; _temp2911.tag=
Cyc_FieldPatternList_tok; _temp2911.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2911;});
_temp2910;}); break; case 302: _LL2909: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2913=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2913[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2914; _temp2914.tag= Cyc_FieldPatternList_tok; _temp2914.f1=({ struct Cyc_List_List*
_temp2915=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2915->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2915->tl= 0;
_temp2915;}); _temp2914;}); _temp2913;}); break; case 303: _LL2912: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2917=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2917[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2918; _temp2918.tag= Cyc_FieldPatternList_tok;
_temp2918.f1=({ struct Cyc_List_List* _temp2919=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2919->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2919->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2919;}); _temp2918;}); _temp2917;}); break; case 304:
_LL2916: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 305: _LL2920: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2922=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2922[ 0]=({ struct Cyc_Exp_tok_struct _temp2923; _temp2923.tag= Cyc_Exp_tok;
_temp2923.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2923;}); _temp2922;}); break; case 306: _LL2921: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 307: _LL2924:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2926=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2926[ 0]=({ struct Cyc_Exp_tok_struct
_temp2927; _temp2927.tag= Cyc_Exp_tok; _temp2927.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2927;}); _temp2926;}); break; case 308: _LL2925: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2929=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2929[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2930; _temp2930.tag= Cyc_Primopopt_tok; _temp2930.f1=
0; _temp2930;}); _temp2929;}); break; case 309: _LL2928: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2932=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2932[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2933; _temp2933.tag= Cyc_Primopopt_tok; _temp2933.f1=({
struct Cyc_Core_Opt* _temp2934=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2934->v=( void*)(( void*) Cyc_Absyn_Times); _temp2934;});
_temp2933;}); _temp2932;}); break; case 310: _LL2931: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2936=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2936[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2937; _temp2937.tag= Cyc_Primopopt_tok; _temp2937.f1=({
struct Cyc_Core_Opt* _temp2938=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2938->v=( void*)(( void*) Cyc_Absyn_Div); _temp2938;});
_temp2937;}); _temp2936;}); break; case 311: _LL2935: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2940=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2940[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2941; _temp2941.tag= Cyc_Primopopt_tok; _temp2941.f1=({
struct Cyc_Core_Opt* _temp2942=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2942->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2942;});
_temp2941;}); _temp2940;}); break; case 312: _LL2939: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2944=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2944[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2945; _temp2945.tag= Cyc_Primopopt_tok; _temp2945.f1=({
struct Cyc_Core_Opt* _temp2946=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2946->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2946;});
_temp2945;}); _temp2944;}); break; case 313: _LL2943: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2948=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2948[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2949; _temp2949.tag= Cyc_Primopopt_tok; _temp2949.f1=({
struct Cyc_Core_Opt* _temp2950=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2950->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2950;});
_temp2949;}); _temp2948;}); break; case 314: _LL2947: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2952=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2952[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2953; _temp2953.tag= Cyc_Primopopt_tok; _temp2953.f1=({
struct Cyc_Core_Opt* _temp2954=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2954->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2954;});
_temp2953;}); _temp2952;}); break; case 315: _LL2951: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2956=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2956[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2957; _temp2957.tag= Cyc_Primopopt_tok; _temp2957.f1=({
struct Cyc_Core_Opt* _temp2958=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2958->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2958;});
_temp2957;}); _temp2956;}); break; case 316: _LL2955: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2960=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2960[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2961; _temp2961.tag= Cyc_Primopopt_tok; _temp2961.f1=({
struct Cyc_Core_Opt* _temp2962=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2962->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2962;});
_temp2961;}); _temp2960;}); break; case 317: _LL2959: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2964=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2964[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2965; _temp2965.tag= Cyc_Primopopt_tok; _temp2965.f1=({
struct Cyc_Core_Opt* _temp2966=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2966->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2966;});
_temp2965;}); _temp2964;}); break; case 318: _LL2963: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2968=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2968[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2969; _temp2969.tag= Cyc_Primopopt_tok; _temp2969.f1=({
struct Cyc_Core_Opt* _temp2970=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2970->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2970;});
_temp2969;}); _temp2968;}); break; case 319: _LL2967: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 320: _LL2971:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2973=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2973[ 0]=({ struct Cyc_Exp_tok_struct
_temp2974; _temp2974.tag= Cyc_Exp_tok; _temp2974.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2974;});
_temp2973;}); break; case 321: _LL2972: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2976=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2976[ 0]=({ struct Cyc_Exp_tok_struct _temp2977; _temp2977.tag= Cyc_Exp_tok;
_temp2977.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2977;}); _temp2976;}); break; case 322: _LL2975: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2979=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2979[ 0]=({ struct Cyc_Exp_tok_struct
_temp2980; _temp2980.tag= Cyc_Exp_tok; _temp2980.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2980;}); _temp2979;}); break; case 323: _LL2978: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2982=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2982[ 0]=({ struct Cyc_Exp_tok_struct
_temp2983; _temp2983.tag= Cyc_Exp_tok; _temp2983.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp2983;}); _temp2982;}); break; case 324: _LL2981: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2985=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2985[ 0]=({ struct Cyc_Exp_tok_struct
_temp2986; _temp2986.tag= Cyc_Exp_tok; _temp2986.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2986;});
_temp2985;}); break; case 325: _LL2984: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2988=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2988[ 0]=({ struct Cyc_Exp_tok_struct _temp2989; _temp2989.tag= Cyc_Exp_tok;
_temp2989.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2989;}); _temp2988;}); break; case 326: _LL2987: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 327: _LL2990:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
328: _LL2991: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2993=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2993[ 0]=({ struct Cyc_Exp_tok_struct
_temp2994; _temp2994.tag= Cyc_Exp_tok; _temp2994.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2994;}); _temp2993;}); break; case 329: _LL2992: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 330: _LL2995:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2997=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2997[ 0]=({ struct Cyc_Exp_tok_struct
_temp2998; _temp2998.tag= Cyc_Exp_tok; _temp2998.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2998;}); _temp2997;}); break; case 331: _LL2996: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 332: _LL2999:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3001=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3001[ 0]=({ struct Cyc_Exp_tok_struct
_temp3002; _temp3002.tag= Cyc_Exp_tok; _temp3002.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3002;});
_temp3001;}); break; case 333: _LL3000: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 334: _LL3003:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3005=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3005[ 0]=({ struct Cyc_Exp_tok_struct
_temp3006; _temp3006.tag= Cyc_Exp_tok; _temp3006.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3006;});
_temp3005;}); break; case 335: _LL3004: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 336: _LL3007:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3009=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3009[ 0]=({ struct Cyc_Exp_tok_struct
_temp3010; _temp3010.tag= Cyc_Exp_tok; _temp3010.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3010;});
_temp3009;}); break; case 337: _LL3008: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 338: _LL3011:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3013=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3013[ 0]=({ struct Cyc_Exp_tok_struct
_temp3014; _temp3014.tag= Cyc_Exp_tok; _temp3014.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3014;}); _temp3013;}); break; case 339: _LL3012: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3016=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3016[ 0]=({ struct Cyc_Exp_tok_struct
_temp3017; _temp3017.tag= Cyc_Exp_tok; _temp3017.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3017;}); _temp3016;}); break; case 340: _LL3015: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 341: _LL3018:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3020=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3020[ 0]=({ struct Cyc_Exp_tok_struct
_temp3021; _temp3021.tag= Cyc_Exp_tok; _temp3021.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3021;}); _temp3020;}); break; case 342: _LL3019: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3023=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3023[ 0]=({ struct Cyc_Exp_tok_struct
_temp3024; _temp3024.tag= Cyc_Exp_tok; _temp3024.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3024;}); _temp3023;}); break; case 343: _LL3022: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3026=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3026[ 0]=({ struct Cyc_Exp_tok_struct
_temp3027; _temp3027.tag= Cyc_Exp_tok; _temp3027.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3027;}); _temp3026;}); break; case 344: _LL3025: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3029=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3029[ 0]=({ struct Cyc_Exp_tok_struct
_temp3030; _temp3030.tag= Cyc_Exp_tok; _temp3030.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3030;}); _temp3029;}); break; case 345: _LL3028: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 346: _LL3031:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3033=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3033[ 0]=({ struct Cyc_Exp_tok_struct
_temp3034; _temp3034.tag= Cyc_Exp_tok; _temp3034.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3034;}); _temp3033;}); break; case 347: _LL3032: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3036=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3036[ 0]=({ struct Cyc_Exp_tok_struct
_temp3037; _temp3037.tag= Cyc_Exp_tok; _temp3037.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3037;}); _temp3036;}); break; case 348: _LL3035: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 349: _LL3038:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3040=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3040[ 0]=({ struct Cyc_Exp_tok_struct
_temp3041; _temp3041.tag= Cyc_Exp_tok; _temp3041.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3041;});
_temp3040;}); break; case 350: _LL3039: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3043=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3043[ 0]=({ struct Cyc_Exp_tok_struct _temp3044; _temp3044.tag= Cyc_Exp_tok;
_temp3044.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3044;}); _temp3043;}); break; case 351: _LL3042: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 352: _LL3045:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3047=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3047[ 0]=({ struct Cyc_Exp_tok_struct
_temp3048; _temp3048.tag= Cyc_Exp_tok; _temp3048.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3048;});
_temp3047;}); break; case 353: _LL3046: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3050=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3050[ 0]=({ struct Cyc_Exp_tok_struct _temp3051; _temp3051.tag= Cyc_Exp_tok;
_temp3051.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3051;}); _temp3050;}); break; case 354: _LL3049: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3053=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3053[ 0]=({ struct Cyc_Exp_tok_struct
_temp3054; _temp3054.tag= Cyc_Exp_tok; _temp3054.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3054;});
_temp3053;}); break; case 355: _LL3052: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 356: _LL3055:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3057=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3057[ 0]=({ struct Cyc_Exp_tok_struct
_temp3058; _temp3058.tag= Cyc_Exp_tok; _temp3058.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3058;}); _temp3057;}); break; case 357: _LL3056: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 358: _LL3059:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3061=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3061[ 0]=({ struct Cyc_Exp_tok_struct
_temp3062; _temp3062.tag= Cyc_Exp_tok; _temp3062.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3062;}); _temp3061;}); break; case 359: _LL3060: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3064=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3064[ 0]=({ struct Cyc_Exp_tok_struct
_temp3065; _temp3065.tag= Cyc_Exp_tok; _temp3065.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3065;}); _temp3064;}); break; case 360: _LL3063: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3067=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3067[ 0]=({ struct Cyc_Exp_tok_struct
_temp3068; _temp3068.tag= Cyc_Exp_tok; _temp3068.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3068;}); _temp3067;}); break; case 361: _LL3066: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3070=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3070[ 0]=({ struct Cyc_Exp_tok_struct
_temp3071; _temp3071.tag= Cyc_Exp_tok; _temp3071.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3071;}); _temp3070;}); break; case 362: _LL3069: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 363: _LL3072:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3074=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3074[ 0]=({ struct Cyc_Exp_tok_struct
_temp3075; _temp3075.tag= Cyc_Exp_tok; _temp3075.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3075;}); _temp3074;}); break; case 364: _LL3073: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3077=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3077[ 0]=({ struct Cyc_Exp_tok_struct
_temp3078; _temp3078.tag= Cyc_Exp_tok; _temp3078.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3078;});
_temp3077;}); break; case 365: _LL3076: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3080=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3080[ 0]=({ struct Cyc_Exp_tok_struct _temp3081; _temp3081.tag= Cyc_Exp_tok;
_temp3081.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3081;}); _temp3080;}); break; case 366: _LL3079: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3083=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3083[ 0]=({ struct Cyc_Exp_tok_struct
_temp3084; _temp3084.tag= Cyc_Exp_tok; _temp3084.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_StructField_struct*
_temp3085=( struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp3085[ 0]=({ struct Cyc_Absyn_StructField_struct _temp3086; _temp3086.tag=
Cyc_Absyn_StructField; _temp3086.f1=({ struct _tagged_arr* _temp3087=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp3087[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3087;});
_temp3086;}); _temp3085;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3084;});
_temp3083;}); break; case 367: _LL3082: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3089=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3089[ 0]=({ struct Cyc_Exp_tok_struct _temp3090; _temp3090.tag= Cyc_Exp_tok;
_temp3090.f1= Cyc_Absyn_offsetof_exp((* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])).f3,( void*)({
struct Cyc_Absyn_TupleIndex_struct* _temp3091=( struct Cyc_Absyn_TupleIndex_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Absyn_TupleIndex_struct)); _temp3091[ 0]=({
struct Cyc_Absyn_TupleIndex_struct _temp3092; _temp3092.tag= Cyc_Absyn_TupleIndex;
_temp3092.f1=( unsigned int)(* Cyc_yyget_Int_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f2; _temp3092;});
_temp3091;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3090;}); _temp3089;}); break; case 368:
_LL3088: { struct Cyc_Position_Segment* _temp3094= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); struct
Cyc_List_List* _temp3095=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, _temp3094, Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3096=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3096[ 0]=({ struct Cyc_Exp_tok_struct
_temp3097; _temp3097.tag= Cyc_Exp_tok; _temp3097.f1= Cyc_Absyn_gentyp_exp(
_temp3095,(* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3097;});
_temp3096;}); break;} case 369: _LL3093: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3099=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3099[ 0]=({ struct Cyc_Exp_tok_struct _temp3100; _temp3100.tag= Cyc_Exp_tok;
_temp3100.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3101=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3101[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3102; _temp3102.tag= Cyc_Absyn_Malloc_e;
_temp3102.f1= 0; _temp3102.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3102;}); _temp3101;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3100;});
_temp3099;}); break; case 370: _LL3098: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3104=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3104[ 0]=({ struct Cyc_Exp_tok_struct _temp3105; _temp3105.tag= Cyc_Exp_tok;
_temp3105.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3106=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3106[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3107; _temp3107.tag= Cyc_Absyn_Malloc_e;
_temp3107.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3107.f2=(
void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3107;}); _temp3106;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3105;});
_temp3104;}); break; case 371: _LL3103: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3109=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3109[ 0]=({ struct Cyc_Primop_tok_struct _temp3110; _temp3110.tag= Cyc_Primop_tok;
_temp3110.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3110;}); _temp3109;});
break; case 372: _LL3108: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3112=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3112[ 0]=({ struct Cyc_Primop_tok_struct _temp3113; _temp3113.tag= Cyc_Primop_tok;
_temp3113.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3113;}); _temp3112;});
break; case 373: _LL3111: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3115=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3115[ 0]=({ struct Cyc_Primop_tok_struct _temp3116; _temp3116.tag= Cyc_Primop_tok;
_temp3116.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3116;}); _temp3115;});
break; case 374: _LL3114: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 375: _LL3117: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3119=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3119[ 0]=({ struct Cyc_Exp_tok_struct _temp3120; _temp3120.tag= Cyc_Exp_tok;
_temp3120.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3120;}); _temp3119;}); break; case 376: _LL3118: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3122=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3122[ 0]=({ struct Cyc_Exp_tok_struct
_temp3123; _temp3123.tag= Cyc_Exp_tok; _temp3123.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3123;}); _temp3122;}); break; case 377:
_LL3121: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3125=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3125[ 0]=({ struct Cyc_Exp_tok_struct
_temp3126; _temp3126.tag= Cyc_Exp_tok; _temp3126.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3126;});
_temp3125;}); break; case 378: _LL3124: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3128=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3128[ 0]=({ struct Cyc_Exp_tok_struct _temp3129; _temp3129.tag= Cyc_Exp_tok;
_temp3129.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3130=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3130[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3130;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3129;}); _temp3128;}); break; case 379: _LL3127: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3132=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3132[ 0]=({ struct Cyc_Exp_tok_struct
_temp3133; _temp3133.tag= Cyc_Exp_tok; _temp3133.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3133;});
_temp3132;}); break;} case 380: _LL3131: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3135=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3135[ 0]=({ struct Cyc_Exp_tok_struct _temp3136; _temp3136.tag= Cyc_Exp_tok;
_temp3136.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3137=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3137[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3137;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3136;}); _temp3135;}); break; case 381: _LL3134: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3139=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3139[ 0]=({ struct Cyc_Exp_tok_struct
_temp3140; _temp3140.tag= Cyc_Exp_tok; _temp3140.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3140;});
_temp3139;}); break;} case 382: _LL3138: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3142=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3142[ 0]=({ struct Cyc_Exp_tok_struct _temp3143; _temp3143.tag= Cyc_Exp_tok;
_temp3143.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3143;}); _temp3142;}); break; case 383: _LL3141: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3145=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3145[ 0]=({ struct Cyc_Exp_tok_struct
_temp3146; _temp3146.tag= Cyc_Exp_tok; _temp3146.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3146;}); _temp3145;}); break; case 384: _LL3144: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3148=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3148[ 0]=({ struct Cyc_Exp_tok_struct
_temp3149; _temp3149.tag= Cyc_Exp_tok; _temp3149.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3150=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3150[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3151; _temp3151.tag= Cyc_Absyn_CompoundLit_e;
_temp3151.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3151.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3151;});
_temp3150;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3149;}); _temp3148;}); break; case 385:
_LL3147: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3153=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3153[ 0]=({ struct Cyc_Exp_tok_struct
_temp3154; _temp3154.tag= Cyc_Exp_tok; _temp3154.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3155=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3155[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3156; _temp3156.tag= Cyc_Absyn_CompoundLit_e;
_temp3156.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3156.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3156;});
_temp3155;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3154;}); _temp3153;}); break; case 386:
_LL3152: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3158=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3158[ 0]=({ struct Cyc_Exp_tok_struct
_temp3159; _temp3159.tag= Cyc_Exp_tok; _temp3159.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3160=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3160[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3161; _temp3161.tag= Cyc_Absyn_Fill_e; _temp3161.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3161;});
_temp3160;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3159;}); _temp3158;}); break; case 387:
_LL3157: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3163=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3163[ 0]=({ struct Cyc_Exp_tok_struct
_temp3164; _temp3164.tag= Cyc_Exp_tok; _temp3164.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3165=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3165[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3166; _temp3166.tag= Cyc_Absyn_Codegen_e;
_temp3166.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3166;}); _temp3165;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3164;}); _temp3163;}); break; case 388: _LL3162: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3168=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3168[ 0]=({ struct Cyc_Exp_tok_struct
_temp3169; _temp3169.tag= Cyc_Exp_tok; _temp3169.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3170=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3170[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3171; _temp3171.tag= Cyc_Absyn_UnknownId_e;
_temp3171.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3171;}); _temp3170;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3169;});
_temp3168;}); break; case 389: _LL3167: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 390: _LL3172:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3174=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3174[ 0]=({ struct Cyc_Exp_tok_struct
_temp3175; _temp3175.tag= Cyc_Exp_tok; _temp3175.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3175;});
_temp3174;}); break; case 391: _LL3173: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 392:
_LL3176: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3178=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3178[ 0]=({ struct Cyc_Exp_tok_struct
_temp3179; _temp3179.tag= Cyc_Exp_tok; _temp3179.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3180=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3180[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3181; _temp3181.tag=
Cyc_Absyn_UnknownId_e; _temp3181.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3181;});
_temp3180;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3179;});
_temp3178;}); break; case 393: _LL3177: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3183=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3183[ 0]=({ struct Cyc_Exp_tok_struct _temp3184; _temp3184.tag= Cyc_Exp_tok;
_temp3184.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3185=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3185[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3186; _temp3186.tag=
Cyc_Absyn_UnknownId_e; _temp3186.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3186;});
_temp3185;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3184;}); _temp3183;}); break; case 394: _LL3182: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3188=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3188[ 0]=({ struct Cyc_Exp_tok_struct
_temp3189; _temp3189.tag= Cyc_Exp_tok; _temp3189.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3189;}); _temp3188;}); break; case 395: _LL3187: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3191=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3191[ 0]=({ struct Cyc_Exp_tok_struct
_temp3192; _temp3192.tag= Cyc_Exp_tok; _temp3192.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3193=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3193[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3194; _temp3194.tag= Cyc_Absyn_Struct_e;
_temp3194.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3194.f2= 0; _temp3194.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3194.f4= 0;
_temp3194;}); _temp3193;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3192;});
_temp3191;}); break; case 396: _LL3190: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3196=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3196[ 0]=({ struct Cyc_Exp_tok_struct _temp3197; _temp3197.tag= Cyc_Exp_tok;
_temp3197.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3197;}); _temp3196;}); break; case 397: _LL3195: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3199=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3199[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3200; _temp3200.tag= Cyc_ExpList_tok; _temp3200.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3200;});
_temp3199;}); break; case 398: _LL3198: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3202=( struct Cyc_ExpList_tok_struct*) _cycalloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3202[ 0]=({ struct Cyc_ExpList_tok_struct _temp3203; _temp3203.tag= Cyc_ExpList_tok;
_temp3203.f1=({ struct Cyc_List_List* _temp3204=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3204->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3204->tl= 0;
_temp3204;}); _temp3203;}); _temp3202;}); break; case 399: _LL3201: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3206=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3206[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3207; _temp3207.tag= Cyc_ExpList_tok; _temp3207.f1=({ struct Cyc_List_List*
_temp3208=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3208->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3208->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp3208;});
_temp3207;}); _temp3206;}); break; case 400: _LL3205: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3210=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3210[ 0]=({ struct Cyc_Exp_tok_struct
_temp3211; _temp3211.tag= Cyc_Exp_tok; _temp3211.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3211;});
_temp3210;}); break; case 401: _LL3209: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3213=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3213[ 0]=({ struct Cyc_Exp_tok_struct _temp3214; _temp3214.tag= Cyc_Exp_tok;
_temp3214.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3214;});
_temp3213;}); break; case 402: _LL3212: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3216=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3216[ 0]=({ struct Cyc_Exp_tok_struct _temp3217; _temp3217.tag= Cyc_Exp_tok;
_temp3217.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3217;});
_temp3216;}); break; case 403: _LL3215: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3219=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3219[ 0]=({ struct Cyc_Exp_tok_struct _temp3220; _temp3220.tag= Cyc_Exp_tok;
_temp3220.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3220;});
_temp3219;}); break; case 404: _LL3218: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3222=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3222[ 0]=({ struct Cyc_QualId_tok_struct _temp3223; _temp3223.tag= Cyc_QualId_tok;
_temp3223.f1=({ struct _tuple1* _temp3224=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3224->f1= Cyc_Absyn_rel_ns_null; _temp3224->f2=({ struct
_tagged_arr* _temp3225=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3225[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3225;}); _temp3224;});
_temp3223;}); _temp3222;}); break; case 405: _LL3221: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3226:(
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
count ++);}} msg=({ unsigned int _temp3228=( unsigned int)( sze +  15);
unsigned char* _temp3229=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp3228)); struct _tagged_arr _temp3231= _tag_arr(
_temp3229, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp3230= _temp3228; unsigned int i; for( i= 0; i <  _temp3230; i ++){
_temp3229[ i]='\000';}}; _temp3231;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
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
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3241= v; struct
_tuple15* _temp3259; struct _tuple15 _temp3261; int _temp3262; unsigned char
_temp3264; short _temp3266; struct _tagged_arr _temp3268; struct Cyc_Core_Opt*
_temp3270; struct Cyc_Core_Opt* _temp3272; struct Cyc_Core_Opt _temp3274; struct
_tagged_arr* _temp3275; struct _tuple1* _temp3277; struct _tuple1 _temp3279;
struct _tagged_arr* _temp3280; void* _temp3282; _LL3243: if(*(( void**)
_temp3241) ==  Cyc_Int_tok){ _LL3260: _temp3259=(( struct Cyc_Int_tok_struct*)
_temp3241)->f1; _temp3261=* _temp3259; _LL3263: _temp3262= _temp3261.f2; goto
_LL3244;} else{ goto _LL3245;} _LL3245: if(*(( void**) _temp3241) ==  Cyc_Char_tok){
_LL3265: _temp3264=(( struct Cyc_Char_tok_struct*) _temp3241)->f1; goto _LL3246;}
else{ goto _LL3247;} _LL3247: if(*(( void**) _temp3241) ==  Cyc_Short_tok){
_LL3267: _temp3266=(( struct Cyc_Short_tok_struct*) _temp3241)->f1; goto _LL3248;}
else{ goto _LL3249;} _LL3249: if(*(( void**) _temp3241) ==  Cyc_String_tok){
_LL3269: _temp3268=(( struct Cyc_String_tok_struct*) _temp3241)->f1; goto
_LL3250;} else{ goto _LL3251;} _LL3251: if(*(( void**) _temp3241) ==  Cyc_Stringopt_tok){
_LL3271: _temp3270=(( struct Cyc_Stringopt_tok_struct*) _temp3241)->f1; if(
_temp3270 ==  0){ goto _LL3252;} else{ goto _LL3253;}} else{ goto _LL3253;}
_LL3253: if(*(( void**) _temp3241) ==  Cyc_Stringopt_tok){ _LL3273: _temp3272=((
struct Cyc_Stringopt_tok_struct*) _temp3241)->f1; if( _temp3272 ==  0){ goto
_LL3255;} else{ _temp3274=* _temp3272; _LL3276: _temp3275=( struct _tagged_arr*)
_temp3274.v; goto _LL3254;}} else{ goto _LL3255;} _LL3255: if(*(( void**)
_temp3241) ==  Cyc_QualId_tok){ _LL3278: _temp3277=(( struct Cyc_QualId_tok_struct*)
_temp3241)->f1; _temp3279=* _temp3277; _LL3283: _temp3282= _temp3279.f1; goto
_LL3281; _LL3281: _temp3280= _temp3279.f2; goto _LL3256;} else{ goto _LL3257;}
_LL3257: goto _LL3258; _LL3244:({ struct Cyc_Std_Int_pa_struct _temp3285;
_temp3285.tag= Cyc_Std_Int_pa; _temp3285.f1=( int)(( unsigned int) _temp3262);{
void* _temp3284[ 1u]={& _temp3285}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3284, sizeof( void*), 1u));}}); goto
_LL3242; _LL3246:({ struct Cyc_Std_Int_pa_struct _temp3287; _temp3287.tag= Cyc_Std_Int_pa;
_temp3287.f1=( int)(( unsigned int)(( int) _temp3264));{ void* _temp3286[ 1u]={&
_temp3287}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3286, sizeof( void*), 1u));}}); goto _LL3242;
_LL3248:({ struct Cyc_Std_Int_pa_struct _temp3289; _temp3289.tag= Cyc_Std_Int_pa;
_temp3289.f1=( int)(( unsigned int)(( int) _temp3266));{ void* _temp3288[ 1u]={&
_temp3289}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3288, sizeof( void*), 1u));}}); goto _LL3242;
_LL3250:({ struct Cyc_Std_String_pa_struct _temp3291; _temp3291.tag= Cyc_Std_String_pa;
_temp3291.f1=( struct _tagged_arr) _temp3268;{ void* _temp3290[ 1u]={& _temp3291};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3290, sizeof( void*), 1u));}}); goto _LL3242; _LL3252:({ void*
_temp3292[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3292, sizeof( void*), 0u));}); goto _LL3242;
_LL3254:({ struct Cyc_Std_String_pa_struct _temp3294; _temp3294.tag= Cyc_Std_String_pa;
_temp3294.f1=( struct _tagged_arr)* _temp3275;{ void* _temp3293[ 1u]={&
_temp3294}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3293, sizeof( void*), 1u));}}); goto _LL3242;
_LL3256: { struct Cyc_List_List* _temp3295= 0;{ void* _temp3296= _temp3282;
struct Cyc_List_List* _temp3304; struct Cyc_List_List* _temp3306; _LL3298: if((
unsigned int) _temp3296 >  1u?*(( int*) _temp3296) ==  Cyc_Absyn_Rel_n: 0){
_LL3305: _temp3304=(( struct Cyc_Absyn_Rel_n_struct*) _temp3296)->f1; goto
_LL3299;} else{ goto _LL3300;} _LL3300: if(( unsigned int) _temp3296 >  1u?*((
int*) _temp3296) ==  Cyc_Absyn_Abs_n: 0){ _LL3307: _temp3306=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3296)->f1; goto _LL3301;} else{ goto _LL3302;} _LL3302: if( _temp3296 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3303;} else{ goto _LL3297;} _LL3299: _temp3295=
_temp3304; goto _LL3297; _LL3301: _temp3295= _temp3306; goto _LL3297; _LL3303:
goto _LL3297; _LL3297:;} for( 0; _temp3295 !=  0; _temp3295= _temp3295->tl){({
struct Cyc_Std_String_pa_struct _temp3309; _temp3309.tag= Cyc_Std_String_pa;
_temp3309.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _temp3295->hd);{ void*
_temp3308[ 1u]={& _temp3309}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3308, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3311; _temp3311.tag= Cyc_Std_String_pa;
_temp3311.f1=( struct _tagged_arr)* _temp3280;{ void* _temp3310[ 1u]={&
_temp3311}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3310, sizeof( void*), 1u));}}); goto _LL3242;}
_LL3258:({ void* _temp3312[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3312, sizeof( void*), 0u));}); goto
_LL3242; _LL3242:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3313=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3313->v=(
void*) Cyc_Lexing_from_file( f); _temp3313;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
