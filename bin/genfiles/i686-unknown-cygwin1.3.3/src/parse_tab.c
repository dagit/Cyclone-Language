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
15u]; extern void* Cyc_Absyn_rel_ns_null; extern int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual( struct
Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern void* Cyc_Absyn_compress_kb( void*); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t(
int); extern struct _tuple1* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Exp*
Cyc_Absyn_exp_unsigned_one; extern void* Cyc_Absyn_starb_typ( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_atb_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
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
struct _tagged_arr s, void* k){ if( Cyc_Std_zstrcmp( s, _tag_arr("`H", sizeof(
unsigned char), 3u)) ==  0){ return( void*) Cyc_Absyn_HeapRgn;} else{ return(
void*)({ struct Cyc_Absyn_VarType_struct* _temp139=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp139[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp140; _temp140.tag= Cyc_Absyn_VarType; _temp140.f1=({ struct Cyc_Absyn_Tvar*
_temp141=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp141->name=({ struct _tagged_arr* _temp142=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp142[ 0]= s; _temp142;}); _temp141->identity=
0; _temp141->kind=( void*) k; _temp141;}); _temp140;}); _temp139;});}} static
struct Cyc_Absyn_Tvar* Cyc_Parse_copy_tvar( struct Cyc_Absyn_Tvar* t){ void* k;{
void* _temp143= Cyc_Absyn_compress_kb(( void*) t->kind); void* _temp151; void*
_temp153; _LL145: if(*(( int*) _temp143) ==  Cyc_Absyn_Eq_kb){ _LL152: _temp151=(
void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp143)->f1; goto _LL146;} else{ goto
_LL147;} _LL147: if(*(( int*) _temp143) ==  Cyc_Absyn_Unknown_kb){ goto _LL148;}
else{ goto _LL149;} _LL149: if(*(( int*) _temp143) ==  Cyc_Absyn_Less_kb){
_LL154: _temp153=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp143)->f2;
goto _LL150;} else{ goto _LL144;} _LL146: k=( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp155=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp155[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp156; _temp156.tag= Cyc_Absyn_Eq_kb;
_temp156.f1=( void*) _temp151; _temp156;}); _temp155;}); goto _LL144; _LL148: k=(
void*)({ struct Cyc_Absyn_Unknown_kb_struct* _temp157=( struct Cyc_Absyn_Unknown_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct)); _temp157[ 0]=({ struct
Cyc_Absyn_Unknown_kb_struct _temp158; _temp158.tag= Cyc_Absyn_Unknown_kb;
_temp158.f1= 0; _temp158;}); _temp157;}); goto _LL144; _LL150: k=( void*)({
struct Cyc_Absyn_Less_kb_struct* _temp159=( struct Cyc_Absyn_Less_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct)); _temp159[ 0]=({ struct Cyc_Absyn_Less_kb_struct
_temp160; _temp160.tag= Cyc_Absyn_Less_kb; _temp160.f1= 0; _temp160.f2=( void*)
_temp153; _temp160;}); _temp159;}); goto _LL144; _LL144:;} return({ struct Cyc_Absyn_Tvar*
_temp161=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp161->name= t->name; _temp161->identity= 0; _temp161->kind=( void*) k;
_temp161;});} static struct Cyc_Absyn_Tvar* Cyc_Parse_typ2tvar( struct Cyc_Position_Segment*
loc, void* t){ void* _temp162= t; struct Cyc_Absyn_Tvar* _temp168; _LL164: if((
unsigned int) _temp162 >  3u?*(( int*) _temp162) ==  Cyc_Absyn_VarType: 0){
_LL169: _temp168=(( struct Cyc_Absyn_VarType_struct*) _temp162)->f1; goto _LL165;}
else{ goto _LL166;} _LL166: goto _LL167; _LL165: return _temp168; _LL167: return((
struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL163:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp170=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp170[ 0]=({ struct Cyc_Absyn_VarType_struct _temp171; _temp171.tag= Cyc_Absyn_VarType;
_temp171.f1= pr; _temp171;}); _temp170;});} static void Cyc_Parse_set_vartyp_kind(
void* t, void* k){ void* _temp172= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp178; struct Cyc_Absyn_Tvar _temp180; void* _temp181; void** _temp183;
_LL174: if(( unsigned int) _temp172 >  3u?*(( int*) _temp172) ==  Cyc_Absyn_VarType:
0){ _LL179: _temp178=(( struct Cyc_Absyn_VarType_struct*) _temp172)->f1;
_temp180=* _temp178; _LL182: _temp181=( void*) _temp180.kind; _temp183=( void**)&(*((
struct Cyc_Absyn_VarType_struct*) _temp172)->f1).kind; goto _LL175;} else{ goto
_LL176;} _LL176: goto _LL177; _LL175: { void* _temp184= Cyc_Absyn_compress_kb(*
_temp183); void* _temp185= _temp184; _LL187: if(*(( int*) _temp185) ==  Cyc_Absyn_Eq_kb){
goto _LL188;} else{ goto _LL189;} _LL189: if(*(( int*) _temp185) ==  Cyc_Absyn_Less_kb){
goto _LL190;} else{ goto _LL191;} _LL191: if(*(( int*) _temp185) ==  Cyc_Absyn_Unknown_kb){
goto _LL192;} else{ goto _LL186;} _LL188: return; _LL190: return; _LL192:*
_temp183=( void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp193=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp193[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp194; _temp194.tag= Cyc_Absyn_Eq_kb; _temp194.f1=( void*) k; _temp194;});
_temp193;}); return; _LL186:;} _LL177: return; _LL173:;} static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List* tms, struct Cyc_List_List*
tds, struct Cyc_Position_Segment* loc){ if( tds ==  0){ return tms;} if( tms == 
0){ return 0;}{ void* _temp195=( void*) tms->hd; void* _temp201; _LL197: if((
unsigned int) _temp195 >  1u?*(( int*) _temp195) ==  Cyc_Absyn_Function_mod: 0){
_LL202: _temp201=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp195)->f1;
goto _LL198;} else{ goto _LL199;} _LL199: goto _LL200; _LL198: if( tms->tl ==  0?
1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd)?((
struct Cyc_List_List*) _check_null( tms->tl))->tl ==  0: 0)){ void* _temp203=
_temp201; struct Cyc_List_List* _temp209; _LL205: if(*(( int*) _temp203) ==  Cyc_Absyn_WithTypes){
goto _LL206;} else{ goto _LL207;} _LL207: if(*(( int*) _temp203) ==  Cyc_Absyn_NoTypes){
_LL210: _temp209=(( struct Cyc_Absyn_NoTypes_struct*) _temp203)->f1; goto _LL208;}
else{ goto _LL204;} _LL206: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL208:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp209,
tds);{ struct _tuple12* _temp211=({ struct _tuple12* _temp217=( struct _tuple12*)
_cycalloc( sizeof( struct _tuple12)); _temp217->f1= tds; _temp217->f2= loc;
_temp217;}); return({ struct Cyc_List_List* _temp212=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp212->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp213=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp213[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp214; _temp214.tag= Cyc_Absyn_Function_mod;
_temp214.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp215=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp215[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp216; _temp216.tag= Cyc_Absyn_WithTypes;
_temp216.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp211, _temp209); _temp216.f2= 0; _temp216.f3= 0;
_temp216.f4= 0; _temp216.f5= 0; _temp216;}); _temp215;})); _temp214;}); _temp213;}));
_temp212->tl= 0; _temp212;});} _LL204:;} else{ return({ struct Cyc_List_List*
_temp218=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp218->hd=( void*)(( void*) tms->hd); _temp218->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp218;});} _LL200: return({ struct Cyc_List_List*
_temp219=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp219->hd=( void*)(( void*) tms->hd); _temp219->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp219;}); _LL196:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds != 
0){ d=({ struct Cyc_Parse_Declarator* _temp220=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp220->id= d->id; _temp220->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp220;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso !=  0){ tss=(( struct
Cyc_Parse_Declaration_spec*) dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline;
atts=(( struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct
Cyc_Parse_Declaration_spec*) dso->v)->sc !=  0){ void* _temp221=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*) dso->v)->sc))->v;
_LL223: if( _temp221 == ( void*) Cyc_Parse_Extern_sc){ goto _LL224;} else{ goto
_LL225;} _LL225: if( _temp221 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL226;}
else{ goto _LL227;} _LL227: if( _temp221 == ( void*) Cyc_Parse_Static_sc){ goto
_LL228;} else{ goto _LL229;} _LL229: goto _LL230; _LL224: sc=( void*) Cyc_Absyn_Extern;
goto _LL222; _LL226: sc=( void*) Cyc_Absyn_ExternC; goto _LL222; _LL228: sc=(
void*) Cyc_Absyn_Static; goto _LL222; _LL230: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL222; _LL222:;}}{ struct Cyc_Core_Opt*
_temp233; void* _temp235; struct _tuple5 _temp231= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL236: _temp235= _temp231.f1; goto _LL234; _LL234: _temp233=
_temp231.f2; goto _LL232; _LL232: { struct Cyc_List_List* _temp239; struct Cyc_List_List*
_temp241; void* _temp243; struct Cyc_Absyn_Tqual _temp245; struct _tuple6
_temp237= Cyc_Parse_apply_tms( tq, _temp235, atts, d->tms); _LL246: _temp245=
_temp237.f1; goto _LL244; _LL244: _temp243= _temp237.f2; goto _LL242; _LL242:
_temp241= _temp237.f3; goto _LL240; _LL240: _temp239= _temp237.f4; goto _LL238;
_LL238: if( _temp233 !=  0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp241 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp247= _temp243; struct Cyc_Absyn_FnInfo _temp253; struct Cyc_List_List*
_temp255; struct Cyc_List_List* _temp257; struct Cyc_Absyn_VarargInfo* _temp259;
int _temp261; struct Cyc_List_List* _temp263; void* _temp265; struct Cyc_Core_Opt*
_temp267; struct Cyc_List_List* _temp269; _LL249: if(( unsigned int) _temp247 > 
3u?*(( int*) _temp247) ==  Cyc_Absyn_FnType: 0){ _LL254: _temp253=(( struct Cyc_Absyn_FnType_struct*)
_temp247)->f1; _LL270: _temp269= _temp253.tvars; goto _LL268; _LL268: _temp267=
_temp253.effect; goto _LL266; _LL266: _temp265=( void*) _temp253.ret_typ; goto
_LL264; _LL264: _temp263= _temp253.args; goto _LL262; _LL262: _temp261= _temp253.c_varargs;
goto _LL260; _LL260: _temp259= _temp253.cyc_varargs; goto _LL258; _LL258:
_temp257= _temp253.rgn_po; goto _LL256; _LL256: _temp255= _temp253.attributes;
goto _LL250;} else{ goto _LL251;} _LL251: goto _LL252; _LL250: { struct Cyc_List_List*
_temp271=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp263); return({ struct Cyc_Absyn_Fndecl*
_temp272=( struct Cyc_Absyn_Fndecl*) _cycalloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp272->sc=( void*) sc; _temp272->name= d->id; _temp272->tvs= _temp269;
_temp272->is_inline= is_inline; _temp272->effect= _temp267; _temp272->ret_type=(
void*) _temp265; _temp272->args= _temp271; _temp272->c_varargs= _temp261;
_temp272->cyc_varargs= _temp259; _temp272->rgn_po= _temp257; _temp272->body=
body; _temp272->cached_typ= 0; _temp272->param_vardecls= 0; _temp272->attributes=
Cyc_List_append( _temp255, _temp239); _temp272;});} _LL252: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL248:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 ==  0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp273=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp273->f1= _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"?", sizeof( unsigned char),
2u); _temp273->f2=(* t).f2; _temp273->f3=(* t).f3; _temp273;});} else{ return({
struct _tuple4* _temp274=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp274->f1=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp274->f2=(* t).f2; _temp274->f3=(* t).f3; _temp274;});}} static
unsigned char _temp275[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp275, _temp275, _temp275 +  52u};
static unsigned char _temp276[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp276, _temp276, _temp276 +  63u};
static unsigned char _temp277[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp277, _temp277, _temp277 +  50u};
static unsigned char _temp278[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp278, _temp278, _temp278 +  60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts !=  0; ts= ts->tl){ void* _temp279=( void*) ts->hd;
struct Cyc_Position_Segment* _temp293; void* _temp295; struct Cyc_Position_Segment*
_temp297; struct Cyc_Position_Segment* _temp299; struct Cyc_Position_Segment*
_temp301; struct Cyc_Position_Segment* _temp303; struct Cyc_Absyn_Decl* _temp305;
_LL281: if(*(( int*) _temp279) ==  Cyc_Parse_Type_spec){ _LL296: _temp295=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp279)->f1; goto _LL294; _LL294: _temp293=((
struct Cyc_Parse_Type_spec_struct*) _temp279)->f2; goto _LL282;} else{ goto
_LL283;} _LL283: if(*(( int*) _temp279) ==  Cyc_Parse_Signed_spec){ _LL298:
_temp297=(( struct Cyc_Parse_Signed_spec_struct*) _temp279)->f1; goto _LL284;}
else{ goto _LL285;} _LL285: if(*(( int*) _temp279) ==  Cyc_Parse_Unsigned_spec){
_LL300: _temp299=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp279)->f1; goto
_LL286;} else{ goto _LL287;} _LL287: if(*(( int*) _temp279) ==  Cyc_Parse_Short_spec){
_LL302: _temp301=(( struct Cyc_Parse_Short_spec_struct*) _temp279)->f1; goto
_LL288;} else{ goto _LL289;} _LL289: if(*(( int*) _temp279) ==  Cyc_Parse_Long_spec){
_LL304: _temp303=(( struct Cyc_Parse_Long_spec_struct*) _temp279)->f1; goto
_LL290;} else{ goto _LL291;} _LL291: if(*(( int*) _temp279) ==  Cyc_Parse_Decl_spec){
_LL306: _temp305=(( struct Cyc_Parse_Decl_spec_struct*) _temp279)->f1; goto
_LL292;} else{ goto _LL280;} _LL282: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp293);} last_loc= _temp293; seen_type= 1; t= _temp295; goto _LL280; _LL284:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp297);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp297);} last_loc= _temp297; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL280; _LL286: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp299);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp299);} last_loc= _temp299; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL280; _LL288: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp301);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp301);} last_loc= _temp301; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL280; _LL290: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp303);} if( seen_size){ void* _temp307= sz; _LL309: if( _temp307 == ( void*)
Cyc_Absyn_B4){ goto _LL310;} else{ goto _LL311;} _LL311: goto _LL312; _LL310: sz=(
void*) Cyc_Absyn_B8; goto _LL308; _LL312: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp303); goto _LL308; _LL308:;} last_loc=
_temp303; seen_size= 1; goto _LL280; _LL292: last_loc= _temp305->loc; if(
declopt ==  0? ! seen_type: 0){ seen_type= 1;{ void* _temp313=( void*) _temp305->r;
struct Cyc_Absyn_Structdecl* _temp325; struct Cyc_Absyn_Tuniondecl* _temp327;
struct Cyc_Absyn_Uniondecl* _temp329; struct Cyc_Absyn_Enumdecl* _temp331;
_LL315: if(*(( int*) _temp313) ==  Cyc_Absyn_Struct_d){ _LL326: _temp325=((
struct Cyc_Absyn_Struct_d_struct*) _temp313)->f1; goto _LL316;} else{ goto
_LL317;} _LL317: if(*(( int*) _temp313) ==  Cyc_Absyn_Tunion_d){ _LL328:
_temp327=(( struct Cyc_Absyn_Tunion_d_struct*) _temp313)->f1; goto _LL318;}
else{ goto _LL319;} _LL319: if(*(( int*) _temp313) ==  Cyc_Absyn_Union_d){
_LL330: _temp329=(( struct Cyc_Absyn_Union_d_struct*) _temp313)->f1; goto _LL320;}
else{ goto _LL321;} _LL321: if(*(( int*) _temp313) ==  Cyc_Absyn_Enum_d){ _LL332:
_temp331=(( struct Cyc_Absyn_Enum_d_struct*) _temp313)->f1; goto _LL322;} else{
goto _LL323;} _LL323: goto _LL324; _LL316: { struct Cyc_List_List* _temp333=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_copy_tvar,
_temp325->tvs)); t=( void*)({ struct Cyc_Absyn_StructType_struct* _temp334=(
struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp334[ 0]=({ struct Cyc_Absyn_StructType_struct _temp335; _temp335.tag= Cyc_Absyn_StructType;
_temp335.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp325->name))->v); _temp335.f2= _temp333; _temp335.f3= 0;
_temp335;}); _temp334;}); if( _temp325->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp336=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp336->v=( void*) _temp305; _temp336;});} goto _LL314;} _LL318: { struct Cyc_List_List*
_temp337=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_copy_tvar, _temp327->tvs)); t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp338=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp338[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp339; _temp339.tag= Cyc_Absyn_TunionType;
_temp339.f1=({ struct Cyc_Absyn_TunionInfo _temp340; _temp340.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp341=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp341[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp342; _temp342.tag= Cyc_Absyn_KnownTunion;
_temp342.f1=({ struct Cyc_Absyn_Tuniondecl** _temp343=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp343[ 0]= _temp327;
_temp343;}); _temp342;}); _temp341;})); _temp340.targs= _temp337; _temp340.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp340;}); _temp339;}); _temp338;}); if(
_temp327->fields !=  0){ declopt=({ struct Cyc_Core_Opt* _temp344=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp344->v=( void*) _temp305;
_temp344;});} goto _LL314;} _LL320: { struct Cyc_List_List* _temp345=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_copy_tvar,
_temp329->tvs)); t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp346=(
struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp346[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp347; _temp347.tag= Cyc_Absyn_UnionType;
_temp347.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp329->name))->v); _temp347.f2= _temp345; _temp347.f3= 0;
_temp347;}); _temp346;}); if( _temp329->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp348=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp348->v=( void*) _temp305; _temp348;});} goto _LL314;} _LL322: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp349=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp349[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp350; _temp350.tag= Cyc_Absyn_EnumType; _temp350.f1=
_temp331->name; _temp350.f2= 0; _temp350;}); _temp349;}); declopt=({ struct Cyc_Core_Opt*
_temp351=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp351->v=( void*) _temp305; _temp351;}); goto _LL314; _LL324:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp305->loc); goto _LL314; _LL314:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp305->loc);} goto _LL280; _LL280:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type within specifier", sizeof(
unsigned char), 30u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp352=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp352[ 0]=({ struct Cyc_Absyn_IntType_struct _temp353; _temp353.tag= Cyc_Absyn_IntType;
_temp353.f1=( void*) sgn; _temp353.f2=( void*) sz; _temp353;}); _temp352;});}
else{ if( seen_sign){ void* _temp354= t; void* _temp360; _LL356: if((
unsigned int) _temp354 >  3u?*(( int*) _temp354) ==  Cyc_Absyn_IntType: 0){
_LL361: _temp360=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp354)->f2;
goto _LL357;} else{ goto _LL358;} _LL358: goto _LL359; _LL357: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp362=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp362[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp363; _temp363.tag= Cyc_Absyn_IntType; _temp363.f1=( void*) sgn; _temp363.f2=(
void*) _temp360; _temp363;}); _temp362;}); goto _LL355; _LL359: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL355; _LL355:;} if( seen_size){ void* _temp364= t; void*
_temp372; _LL366: if(( unsigned int) _temp364 >  3u?*(( int*) _temp364) ==  Cyc_Absyn_IntType:
0){ _LL373: _temp372=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp364)->f1;
goto _LL367;} else{ goto _LL368;} _LL368: if(( unsigned int) _temp364 >  3u?*((
int*) _temp364) ==  Cyc_Absyn_DoubleType: 0){ goto _LL369;} else{ goto _LL370;}
_LL370: goto _LL371; _LL367: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp374=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp374[ 0]=({ struct Cyc_Absyn_IntType_struct _temp375; _temp375.tag= Cyc_Absyn_IntType;
_temp375.f1=( void*) _temp372; _temp375.f2=( void*) sz; _temp375;}); _temp374;});
goto _LL365; _LL369: t=( void*)({ struct Cyc_Absyn_DoubleType_struct* _temp376=(
struct Cyc_Absyn_DoubleType_struct*) _cycalloc_atomic( sizeof( struct Cyc_Absyn_DoubleType_struct));
_temp376[ 0]=({ struct Cyc_Absyn_DoubleType_struct _temp377; _temp377.tag= Cyc_Absyn_DoubleType;
_temp377.f1= 1; _temp377;}); _temp376;}); goto _LL365; _LL371: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL365; _LL365:;}} return({ struct _tuple5 _temp378; _temp378.f1=
t; _temp378.f2= declopt; _temp378;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds ==  0){ return 0;}{ struct Cyc_Parse_Declarator* _temp379=(
struct Cyc_Parse_Declarator*) ds->hd; struct _tuple1* _temp380= _temp379->id;
struct Cyc_List_List* _temp383; struct Cyc_List_List* _temp385; void* _temp387;
struct Cyc_Absyn_Tqual _temp389; struct _tuple6 _temp381= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp379->tms); _LL390: _temp389= _temp381.f1; goto _LL388;
_LL388: _temp387= _temp381.f2; goto _LL386; _LL386: _temp385= _temp381.f3; goto
_LL384; _LL384: _temp383= _temp381.f4; goto _LL382; _LL382: return({ struct Cyc_List_List*
_temp391=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp391->hd=( void*)({ struct _tuple7* _temp392=( struct _tuple7*) _cycalloc(
sizeof( struct _tuple7)); _temp392->f1= _temp380; _temp392->f2= _temp389;
_temp392->f3= _temp387; _temp392->f4= _temp385; _temp392->f5= _temp383; _temp392;});
_temp391->tl= Cyc_Parse_apply_tmss( _temp389, Cyc_Tcutil_copy_type( t), ds->tl,
shared_atts); _temp391;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms ==  0){ return({ struct _tuple6 _temp393; _temp393.f1= tq;
_temp393.f2= t; _temp393.f3= 0; _temp393.f4= atts; _temp393;});}{ void* _temp394=(
void*) tms->hd; struct Cyc_Absyn_Exp* _temp408; void* _temp410; struct Cyc_Position_Segment*
_temp412; struct Cyc_List_List* _temp414; struct Cyc_Absyn_Tqual _temp416; void*
_temp418; void* _temp420; struct Cyc_List_List* _temp422; struct Cyc_Position_Segment*
_temp424; _LL396: if( _temp394 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL397;}
else{ goto _LL398;} _LL398: if(( unsigned int) _temp394 >  1u?*(( int*) _temp394)
==  Cyc_Absyn_ConstArray_mod: 0){ _LL409: _temp408=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp394)->f1; goto _LL399;} else{ goto _LL400;} _LL400: if(( unsigned int)
_temp394 >  1u?*(( int*) _temp394) ==  Cyc_Absyn_Function_mod: 0){ _LL411:
_temp410=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp394)->f1; goto
_LL401;} else{ goto _LL402;} _LL402: if(( unsigned int) _temp394 >  1u?*(( int*)
_temp394) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL415: _temp414=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp394)->f1; goto _LL413; _LL413: _temp412=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp394)->f2; goto _LL403;} else{ goto _LL404;} _LL404: if(( unsigned int)
_temp394 >  1u?*(( int*) _temp394) ==  Cyc_Absyn_Pointer_mod: 0){ _LL421:
_temp420=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp394)->f1; goto
_LL419; _LL419: _temp418=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp394)->f2; goto _LL417; _LL417: _temp416=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp394)->f3; goto _LL405;} else{ goto _LL406;} _LL406: if(( unsigned int)
_temp394 >  1u?*(( int*) _temp394) ==  Cyc_Absyn_Attributes_mod: 0){ _LL425:
_temp424=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp394)->f1; goto _LL423;
_LL423: _temp422=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp394)->f2; goto
_LL407;} else{ goto _LL395;} _LL397: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp426=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp426[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp427; _temp427.tag= Cyc_Absyn_ArrayType; _temp427.f1=(
void*) t; _temp427.f2= tq; _temp427.f3= 0; _temp427;}); _temp426;}), atts, tms->tl);
_LL399: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp428=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp428[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp429; _temp429.tag= Cyc_Absyn_ArrayType; _temp429.f1=(
void*) t; _temp429.f2= tq; _temp429.f3=( struct Cyc_Absyn_Exp*) _temp408;
_temp429;}); _temp428;}), atts, tms->tl); _LL401: { void* _temp430= _temp410;
struct Cyc_List_List* _temp436; struct Cyc_Core_Opt* _temp438; struct Cyc_Absyn_VarargInfo*
_temp440; int _temp442; struct Cyc_List_List* _temp444; struct Cyc_Position_Segment*
_temp446; _LL432: if(*(( int*) _temp430) ==  Cyc_Absyn_WithTypes){ _LL445:
_temp444=(( struct Cyc_Absyn_WithTypes_struct*) _temp430)->f1; goto _LL443;
_LL443: _temp442=(( struct Cyc_Absyn_WithTypes_struct*) _temp430)->f2; goto
_LL441; _LL441: _temp440=(( struct Cyc_Absyn_WithTypes_struct*) _temp430)->f3;
goto _LL439; _LL439: _temp438=(( struct Cyc_Absyn_WithTypes_struct*) _temp430)->f4;
goto _LL437; _LL437: _temp436=(( struct Cyc_Absyn_WithTypes_struct*) _temp430)->f5;
goto _LL433;} else{ goto _LL434;} _LL434: if(*(( int*) _temp430) ==  Cyc_Absyn_NoTypes){
_LL447: _temp446=(( struct Cyc_Absyn_NoTypes_struct*) _temp430)->f2; goto _LL435;}
else{ goto _LL431;} _LL433: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as !=  0; as= as->tl){ if( Cyc_Absyn_fntype_att(( void*) as->hd)){
fn_atts=({ struct Cyc_List_List* _temp448=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp448->hd=( void*)(( void*) as->hd); _temp448->tl=
fn_atts; _temp448;});} else{ new_atts=({ struct Cyc_List_List* _temp449=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp449->hd=( void*)((
void*) as->hd); _temp449->tl= new_atts; _temp449;});}}} if( tms->tl !=  0){ void*
_temp450=( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd; struct
Cyc_List_List* _temp456; _LL452: if(( unsigned int) _temp450 >  1u?*(( int*)
_temp450) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL457: _temp456=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp450)->f1; goto _LL453;} else{ goto _LL454;} _LL454: goto _LL455; _LL453:
typvars= _temp456; tms= tms->tl; goto _LL451; _LL455: goto _LL451; _LL451:;} if(((((
! _temp442? _temp440 ==  0: 0)? _temp444 !=  0: 0)? _temp444->tl ==  0: 0)?(*((
struct _tuple2*) _temp444->hd)).f1 ==  0: 0)?(*(( struct _tuple2*) _temp444->hd)).f3
== ( void*) Cyc_Absyn_VoidType: 0){ _temp444= 0;} t= Cyc_Parse_array2ptr( t);((
void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Parse_arg_array2ptr, _temp444); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),
Cyc_Absyn_function_typ( typvars, _temp438, t, _temp444, _temp442, _temp440,
_temp436, fn_atts), new_atts,(( struct Cyc_List_List*) _check_null( tms))->tl);}
_LL435:( int) _throw( Cyc_Parse_abort( _tag_arr("function declaration without parameter types",
sizeof( unsigned char), 45u), _temp446)); _LL431:;} _LL403: if( tms->tl ==  0){
return({ struct _tuple6 _temp458; _temp458.f1= tq; _temp458.f2= t; _temp458.f3=
_temp414; _temp458.f4= atts; _temp458;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp412)); _LL405: { void* _temp459= _temp420;
struct Cyc_Absyn_Exp* _temp467; struct Cyc_Absyn_Exp* _temp469; _LL461: if((
unsigned int) _temp459 >  1u?*(( int*) _temp459) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL468: _temp467=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp459)->f1; goto
_LL462;} else{ goto _LL463;} _LL463: if(( unsigned int) _temp459 >  1u?*(( int*)
_temp459) ==  Cyc_Absyn_Nullable_ps: 0){ _LL470: _temp469=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp459)->f1; goto _LL464;} else{ goto _LL465;} _LL465: if( _temp459 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL466;} else{ goto _LL460;} _LL462: return Cyc_Parse_apply_tms(
_temp416, Cyc_Absyn_atb_typ( t, _temp418, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp471=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp471[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp472; _temp472.tag= Cyc_Absyn_Upper_b;
_temp472.f1= _temp467; _temp472;}); _temp471;})), atts, tms->tl); _LL464: return
Cyc_Parse_apply_tms( _temp416, Cyc_Absyn_starb_typ( t, _temp418, tq,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp473=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp473[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp474; _temp474.tag= Cyc_Absyn_Upper_b; _temp474.f1= _temp469; _temp474;});
_temp473;})), atts, tms->tl); _LL466: return Cyc_Parse_apply_tms( _temp416, Cyc_Absyn_tagged_typ(
t, _temp418, tq), atts, tms->tl); _LL460:;} _LL407: return Cyc_Parse_apply_tms(
tq, t, Cyc_List_append( atts, _temp422), tms->tl); _LL395:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp477; void* _temp479; struct _tuple5 _temp475= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL480: _temp479= _temp475.f1; goto _LL478; _LL478: _temp477=
_temp475.f2; goto _LL476; _LL476: if( _temp477 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp479;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp481=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp481[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp482; _temp482.tag= Cyc_Absyn_Decl_s;
_temp482.f1= d; _temp482.f2= s; _temp482;}); _temp481;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc !=  0){ void* _temp483=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL485: if( _temp483 == (
void*) Cyc_Parse_Typedef_sc){ goto _LL486;} else{ goto _LL487;} _LL487: if(
_temp483 == ( void*) Cyc_Parse_Extern_sc){ goto _LL488;} else{ goto _LL489;}
_LL489: if( _temp483 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL490;} else{ goto
_LL491;} _LL491: if( _temp483 == ( void*) Cyc_Parse_Static_sc){ goto _LL492;}
else{ goto _LL493;} _LL493: if( _temp483 == ( void*) Cyc_Parse_Auto_sc){ goto
_LL494;} else{ goto _LL495;} _LL495: if( _temp483 == ( void*) Cyc_Parse_Register_sc){
goto _LL496;} else{ goto _LL497;} _LL497: if( _temp483 == ( void*) Cyc_Parse_Abstract_sc){
goto _LL498;} else{ goto _LL484;} _LL486: istypedef= 1; goto _LL484; _LL488: s=(
void*) Cyc_Absyn_Extern; goto _LL484; _LL490: s=( void*) Cyc_Absyn_ExternC; goto
_LL484; _LL492: s=( void*) Cyc_Absyn_Static; goto _LL484; _LL494: s=( void*) Cyc_Absyn_Public;
goto _LL484; _LL496: s=( void*) Cyc_Absyn_Public; goto _LL484; _LL498: s=( void*)
Cyc_Absyn_Abstract; goto _LL484; _LL484:;}{ struct Cyc_List_List* _temp501;
struct Cyc_List_List* _temp503; struct _tuple0 _temp499=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL504: _temp503= _temp499.f1;
goto _LL502; _LL502: _temp501= _temp499.f2; goto _LL500; _LL500: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp501; for( 0; es !=  0; es= es->tl){
if(( struct Cyc_Absyn_Exp*) es->hd !=  0){ exps_empty= 0; break;}}}{ struct
_tuple5 _temp505= Cyc_Parse_collapse_type_specifiers( tss, loc); if( _temp503 == 
0){ struct Cyc_Core_Opt* _temp508; void* _temp510; struct _tuple5 _temp506=
_temp505; _LL511: _temp510= _temp506.f1; goto _LL509; _LL509: _temp508= _temp506.f2;
goto _LL507; _LL507: if( _temp508 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
_temp508->v;{ void* _temp512=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp524;
struct Cyc_Absyn_Structdecl* _temp526; struct Cyc_Absyn_Uniondecl* _temp528;
struct Cyc_Absyn_Tuniondecl* _temp530; _LL514: if(*(( int*) _temp512) ==  Cyc_Absyn_Enum_d){
_LL525: _temp524=(( struct Cyc_Absyn_Enum_d_struct*) _temp512)->f1; goto _LL515;}
else{ goto _LL516;} _LL516: if(*(( int*) _temp512) ==  Cyc_Absyn_Struct_d){
_LL527: _temp526=(( struct Cyc_Absyn_Struct_d_struct*) _temp512)->f1; goto
_LL517;} else{ goto _LL518;} _LL518: if(*(( int*) _temp512) ==  Cyc_Absyn_Union_d){
_LL529: _temp528=(( struct Cyc_Absyn_Union_d_struct*) _temp512)->f1; goto _LL519;}
else{ goto _LL520;} _LL520: if(*(( int*) _temp512) ==  Cyc_Absyn_Tunion_d){
_LL531: _temp530=(( struct Cyc_Absyn_Tunion_d_struct*) _temp512)->f1; goto
_LL521;} else{ goto _LL522;} _LL522: goto _LL523; _LL515:( void*)( _temp524->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum",
sizeof( unsigned char), 23u), loc);} goto _LL513; _LL517:( void*)( _temp526->sc=(
void*) s); _temp526->attributes= atts; goto _LL513; _LL519:( void*)( _temp528->sc=(
void*) s); _temp528->attributes= atts; goto _LL513; _LL521:( void*)( _temp530->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL513; _LL523: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL513:;}
return({ struct Cyc_List_List* _temp532=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp532->hd=( void*) d; _temp532->tl= 0;
_temp532;});} else{ void* _temp533= _temp510; struct Cyc_List_List* _temp549;
struct _tuple1* _temp551; struct Cyc_Absyn_TunionInfo _temp553; void* _temp555;
struct Cyc_Absyn_Tuniondecl** _temp557; struct Cyc_Absyn_TunionInfo _temp559;
struct Cyc_List_List* _temp561; void* _temp563; struct Cyc_Absyn_UnknownTunionInfo
_temp565; int _temp567; struct _tuple1* _temp569; struct Cyc_List_List* _temp571;
struct _tuple1* _temp573; struct _tuple1* _temp575; struct Cyc_List_List*
_temp577; _LL535: if(( unsigned int) _temp533 >  3u?*(( int*) _temp533) ==  Cyc_Absyn_StructType:
0){ _LL552: _temp551=(( struct Cyc_Absyn_StructType_struct*) _temp533)->f1; goto
_LL550; _LL550: _temp549=(( struct Cyc_Absyn_StructType_struct*) _temp533)->f2;
goto _LL536;} else{ goto _LL537;} _LL537: if(( unsigned int) _temp533 >  3u?*((
int*) _temp533) ==  Cyc_Absyn_TunionType: 0){ _LL554: _temp553=(( struct Cyc_Absyn_TunionType_struct*)
_temp533)->f1; _LL556: _temp555=( void*) _temp553.tunion_info; if(*(( int*)
_temp555) ==  Cyc_Absyn_KnownTunion){ _LL558: _temp557=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp555)->f1; goto _LL538;} else{ goto _LL539;}} else{ goto _LL539;} _LL539:
if(( unsigned int) _temp533 >  3u?*(( int*) _temp533) ==  Cyc_Absyn_TunionType:
0){ _LL560: _temp559=(( struct Cyc_Absyn_TunionType_struct*) _temp533)->f1;
_LL564: _temp563=( void*) _temp559.tunion_info; if(*(( int*) _temp563) ==  Cyc_Absyn_UnknownTunion){
_LL566: _temp565=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp563)->f1;
_LL570: _temp569= _temp565.name; goto _LL568; _LL568: _temp567= _temp565.is_xtunion;
goto _LL562;} else{ goto _LL541;} _LL562: _temp561= _temp559.targs; goto _LL540;}
else{ goto _LL541;} _LL541: if(( unsigned int) _temp533 >  3u?*(( int*) _temp533)
==  Cyc_Absyn_UnionType: 0){ _LL574: _temp573=(( struct Cyc_Absyn_UnionType_struct*)
_temp533)->f1; goto _LL572; _LL572: _temp571=(( struct Cyc_Absyn_UnionType_struct*)
_temp533)->f2; goto _LL542;} else{ goto _LL543;} _LL543: if(( unsigned int)
_temp533 >  3u?*(( int*) _temp533) ==  Cyc_Absyn_EnumType: 0){ _LL576: _temp575=((
struct Cyc_Absyn_EnumType_struct*) _temp533)->f1; goto _LL544;} else{ goto
_LL545;} _LL545: if(( unsigned int) _temp533 >  3u?*(( int*) _temp533) ==  Cyc_Absyn_AnonEnumType:
0){ _LL578: _temp577=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp533)->f1;
goto _LL546;} else{ goto _LL547;} _LL547: goto _LL548; _LL536: { struct Cyc_List_List*
_temp579=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp549); struct Cyc_Absyn_Structdecl* _temp580=({
struct Cyc_Absyn_Structdecl* _temp584=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp584->sc=( void*) s; _temp584->name=({
struct Cyc_Core_Opt* _temp585=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp585->v=( void*)(( struct _tuple1*) _check_null( _temp551));
_temp585;}); _temp584->tvs= _temp579; _temp584->fields= 0; _temp584->attributes=
0; _temp584;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp581=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp581->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp582=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp582[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp583; _temp583.tag= Cyc_Absyn_Struct_d;
_temp583.f1= _temp580; _temp583;}); _temp582;}), loc); _temp581->tl= 0; _temp581;});}
_LL538: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp586=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp586->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp587=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp587[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp588; _temp588.tag= Cyc_Absyn_Tunion_d;
_temp588.f1=* _temp557; _temp588;}); _temp587;}), loc); _temp586->tl= 0;
_temp586;}); _LL540: { struct Cyc_List_List* _temp589=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp561); struct Cyc_Absyn_Decl* _temp590= Cyc_Absyn_tunion_decl( s, _temp569,
_temp589, 0, _temp567, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp591=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp591->hd=(
void*) _temp590; _temp591->tl= 0; _temp591;});} _LL542: { struct Cyc_List_List*
_temp592=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp571); struct Cyc_Absyn_Uniondecl* _temp593=({
struct Cyc_Absyn_Uniondecl* _temp598=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp598->sc=( void*) s; _temp598->name=({
struct Cyc_Core_Opt* _temp599=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp599->v=( void*)(( struct _tuple1*) _check_null( _temp573));
_temp599;}); _temp598->tvs= _temp592; _temp598->fields= 0; _temp598->attributes=
0; _temp598;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp594=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp594->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp595=( struct Cyc_Absyn_Decl*) _cycalloc(
sizeof( struct Cyc_Absyn_Decl)); _temp595->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp596=( struct Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp596[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp597; _temp597.tag= Cyc_Absyn_Union_d;
_temp597.f1= _temp593; _temp597;}); _temp596;})); _temp595->loc= loc; _temp595;});
_temp594->tl= 0; _temp594;});} _LL544: { struct Cyc_Absyn_Enumdecl* _temp600=({
struct Cyc_Absyn_Enumdecl* _temp605=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp605->sc=( void*) s; _temp605->name=
_temp575; _temp605->fields= 0; _temp605;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp601=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp601->hd=( void*)({ struct Cyc_Absyn_Decl* _temp602=(
struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp602->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp603=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp603[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp604; _temp604.tag= Cyc_Absyn_Enum_d; _temp604.f1= _temp600; _temp604;});
_temp603;})); _temp602->loc= loc; _temp602;}); _temp601->tl= 0; _temp601;});}
_LL546: { struct Cyc_Absyn_Enumdecl* _temp606=({ struct Cyc_Absyn_Enumdecl*
_temp611=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp611->sc=( void*) s; _temp611->name= Cyc_Parse_gensym_enum(); _temp611->fields=({
struct Cyc_Core_Opt* _temp612=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp612->v=( void*) _temp577; _temp612;}); _temp611;}); if(
atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} return({ struct Cyc_List_List* _temp607=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp607->hd=( void*)({
struct Cyc_Absyn_Decl* _temp608=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp608->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp609=( struct Cyc_Absyn_Enum_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp609[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp610; _temp610.tag= Cyc_Absyn_Enum_d;
_temp610.f1= _temp606; _temp610;}); _temp609;})); _temp608->loc= loc; _temp608;});
_temp607->tl= 0; _temp607;});} _LL548: Cyc_Parse_err( _tag_arr("missing declarator",
sizeof( unsigned char), 19u), loc); return 0; _LL534:;}} else{ void* t= _temp505.f1;
struct Cyc_List_List* _temp613= Cyc_Parse_apply_tmss( tq, t, _temp503, atts);
if( istypedef){ if( ! exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp613); if( _temp505.f2 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(
_temp505.f2)->v;{ void* _temp614=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp626; struct Cyc_Absyn_Tuniondecl* _temp628; struct Cyc_Absyn_Uniondecl*
_temp630; struct Cyc_Absyn_Enumdecl* _temp632; _LL616: if(*(( int*) _temp614) == 
Cyc_Absyn_Struct_d){ _LL627: _temp626=(( struct Cyc_Absyn_Struct_d_struct*)
_temp614)->f1; goto _LL617;} else{ goto _LL618;} _LL618: if(*(( int*) _temp614)
==  Cyc_Absyn_Tunion_d){ _LL629: _temp628=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp614)->f1; goto _LL619;} else{ goto _LL620;} _LL620: if(*(( int*) _temp614)
==  Cyc_Absyn_Union_d){ _LL631: _temp630=(( struct Cyc_Absyn_Union_d_struct*)
_temp614)->f1; goto _LL621;} else{ goto _LL622;} _LL622: if(*(( int*) _temp614)
==  Cyc_Absyn_Enum_d){ _LL633: _temp632=(( struct Cyc_Absyn_Enum_d_struct*)
_temp614)->f1; goto _LL623;} else{ goto _LL624;} _LL624: goto _LL625; _LL617:(
void*)( _temp626->sc=( void*) s); _temp626->attributes= atts; atts= 0; goto
_LL615; _LL619:( void*)( _temp628->sc=( void*) s); goto _LL615; _LL621:( void*)(
_temp630->sc=( void*) s); goto _LL615; _LL623:( void*)( _temp632->sc=( void*) s);
goto _LL615; _LL625: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL615; _LL615:;} decls=({ struct Cyc_List_List*
_temp634=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp634->hd=( void*) d; _temp634->tl= decls; _temp634;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp636; _temp636.tag=
Cyc_Std_String_pa; _temp636.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) atts->hd);{ void* _temp635[ 1u]={& _temp636}; Cyc_Std_aprintf( _tag_arr("bad attribute %s in typedef",
sizeof( unsigned char), 28u), _tag_arr( _temp635, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp505.f2 !=  0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp637= _temp613; for( 0; _temp637 !=  0;( _temp637= _temp637->tl,
_temp501= _temp501->tl)){ struct _tuple7 _temp640; struct Cyc_List_List*
_temp641; struct Cyc_List_List* _temp643; void* _temp645; struct Cyc_Absyn_Tqual
_temp647; struct _tuple1* _temp649; struct _tuple7* _temp638=( struct _tuple7*)
_temp637->hd; _temp640=* _temp638; _LL650: _temp649= _temp640.f1; goto _LL648;
_LL648: _temp647= _temp640.f2; goto _LL646; _LL646: _temp645= _temp640.f3; goto
_LL644; _LL644: _temp643= _temp640.f4; goto _LL642; _LL642: _temp641= _temp640.f5;
goto _LL639; _LL639: if( _temp643 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), loc);} if( _temp501 ==  0){(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("unexpected NULL in parse!",
sizeof( unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp651=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp501))->hd; struct Cyc_Absyn_Vardecl*
_temp652= Cyc_Absyn_new_vardecl( _temp649, _temp645, _temp651); _temp652->tq=
_temp647;( void*)( _temp652->sc=( void*) s); _temp652->attributes= _temp641;{
struct Cyc_Absyn_Decl* _temp653=({ struct Cyc_Absyn_Decl* _temp655=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp655->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp656=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp656[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp657; _temp657.tag= Cyc_Absyn_Var_d; _temp657.f1= _temp652; _temp657;});
_temp656;})); _temp655->loc= loc; _temp655;}); decls=({ struct Cyc_List_List*
_temp654=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp654->hd=( void*) _temp653; _temp654->tl= decls; _temp654;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_Std_strlen( s) !=  1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp659; _temp659.tag= Cyc_Std_String_pa;
_temp659.f1=( struct _tagged_arr) s;{ void* _temp658[ 1u]={& _temp659}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp658,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL660: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL661: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL662: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL663: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL664: return(
void*) Cyc_Absyn_EffKind; default: _LL665: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp668; _temp668.tag= Cyc_Std_String_pa;
_temp668.f1=( struct _tagged_arr) s;{ void* _temp667[ 1u]={& _temp668}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp667,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts ==  0){ return
tms;} else{ return({ struct Cyc_List_List* _temp669=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp669->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp670=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp670[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp671; _temp671.tag= Cyc_Absyn_Attributes_mod;
_temp671.f1= loc; _temp671.f2= atts; _temp671;}); _temp670;})); _temp669->tl=
tms; _temp669;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp674;
struct Cyc_List_List* _temp675; struct Cyc_List_List* _temp677; void* _temp679;
struct Cyc_Absyn_Tqual _temp681; struct _tuple1* _temp683; struct _tuple7*
_temp672= t; _temp674=* _temp672; _LL684: _temp683= _temp674.f1; goto _LL682;
_LL682: _temp681= _temp674.f2; goto _LL680; _LL680: _temp679= _temp674.f3; goto
_LL678; _LL678: _temp677= _temp674.f4; goto _LL676; _LL676: _temp675= _temp674.f5;
goto _LL673; _LL673: Cyc_Lex_register_typedef( _temp683); if( _temp675 !=  0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp686;
_temp686.tag= Cyc_Std_String_pa; _temp686.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) _temp675->hd);{ void* _temp685[ 1u]={& _temp686}; Cyc_Std_aprintf(
_tag_arr("bad attribute %s within typedef", sizeof( unsigned char), 32u),
_tag_arr( _temp685, sizeof( void*), 1u));}}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp687=( struct Cyc_Absyn_Typedef_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp687[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp688; _temp688.tag= Cyc_Absyn_Typedef_d; _temp688.f1=({
struct Cyc_Absyn_Typedefdecl* _temp689=( struct Cyc_Absyn_Typedefdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp689->name= _temp683;
_temp689->tvs= _temp677; _temp689->defn=( void*) _temp679; _temp689;}); _temp688;});
_temp687;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
unsigned char _temp691[ 15u]="$(sign_t,int)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp691, _temp691, _temp691 +  15u}};
struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ struct _tuple15* yyzzz;{ void*
_temp692= yy1; struct _tuple15* _temp698; _LL694: if(*(( void**) _temp692) == 
Cyc_Int_tok){ _LL699: _temp698=(( struct Cyc_Int_tok_struct*) _temp692)->f1;
goto _LL695;} else{ goto _LL696;} _LL696: goto _LL697; _LL695: yyzzz= _temp698;
goto _LL693; _LL697:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL693:;}
return yyzzz;} static unsigned char _temp701[ 9u]="string_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp701, _temp701, _temp701 +  9u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ struct _tagged_arr yyzzz;{
void* _temp702= yy1; struct _tagged_arr _temp708; _LL704: if(*(( void**)
_temp702) ==  Cyc_String_tok){ _LL709: _temp708=(( struct Cyc_String_tok_struct*)
_temp702)->f1; goto _LL705;} else{ goto _LL706;} _LL706: goto _LL707; _LL705:
yyzzz= _temp708; goto _LL703; _LL707:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL703:;} return yyzzz;} static unsigned char _temp711[ 5u]="char"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{ _temp711,
_temp711, _temp711 +  5u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
unsigned char yyzzz;{ void* _temp712= yy1; unsigned char _temp718; _LL714: if(*((
void**) _temp712) ==  Cyc_Char_tok){ _LL719: _temp718=(( struct Cyc_Char_tok_struct*)
_temp712)->f1; goto _LL715;} else{ goto _LL716;} _LL716: goto _LL717; _LL715:
yyzzz= _temp718; goto _LL713; _LL717:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL713:;} return yyzzz;} static unsigned char _temp721[ 20u]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{
_temp721, _temp721, _temp721 +  20u}}; void* Cyc_yyget_Pointer_Sort_tok( void*
yy1){ void* yyzzz;{ void* _temp722= yy1; void* _temp728; _LL724: if(*(( void**)
_temp722) ==  Cyc_Pointer_Sort_tok){ _LL729: _temp728=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp722)->f1; goto _LL725;} else{ goto _LL726;} _LL726: goto _LL727; _LL725:
yyzzz= _temp728; goto _LL723; _LL727:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL723:;} return yyzzz;} static unsigned char _temp731[ 6u]="exp_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp731,
_temp731, _temp731 +  6u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp732= yy1; struct Cyc_Absyn_Exp*
_temp738; _LL734: if(*(( void**) _temp732) ==  Cyc_Exp_tok){ _LL739: _temp738=((
struct Cyc_Exp_tok_struct*) _temp732)->f1; goto _LL735;} else{ goto _LL736;}
_LL736: goto _LL737; _LL735: yyzzz= _temp738; goto _LL733; _LL737:( int) _throw((
void*)& Cyc_yyfail_Exp_tok); _LL733:;} return yyzzz;} static unsigned char
_temp741[ 14u]="list_t<exp_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{ _temp741, _temp741, _temp741 +  14u}}; struct Cyc_List_List*
Cyc_yyget_ExpList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp742=
yy1; struct Cyc_List_List* _temp748; _LL744: if(*(( void**) _temp742) ==  Cyc_ExpList_tok){
_LL749: _temp748=(( struct Cyc_ExpList_tok_struct*) _temp742)->f1; goto _LL745;}
else{ goto _LL746;} _LL746: goto _LL747; _LL745: yyzzz= _temp748; goto _LL743;
_LL747:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL743:;} return yyzzz;}
static unsigned char _temp751[ 39u]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{
_temp751, _temp751, _temp751 +  39u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp752= yy1; struct Cyc_List_List*
_temp758; _LL754: if(*(( void**) _temp752) ==  Cyc_InitializerList_tok){ _LL759:
_temp758=(( struct Cyc_InitializerList_tok_struct*) _temp752)->f1; goto _LL755;}
else{ goto _LL756;} _LL756: goto _LL757; _LL755: yyzzz= _temp758; goto _LL753;
_LL757:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL753:;} return
yyzzz;} static unsigned char _temp761[ 9u]="primop_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp761, _temp761, _temp761 +  9u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* yyzzz;{ void* _temp762= yy1; void*
_temp768; _LL764: if(*(( void**) _temp762) ==  Cyc_Primop_tok){ _LL769: _temp768=(
void*)(( struct Cyc_Primop_tok_struct*) _temp762)->f1; goto _LL765;} else{ goto
_LL766;} _LL766: goto _LL767; _LL765: yyzzz= _temp768; goto _LL763; _LL767:( int)
_throw(( void*)& Cyc_yyfail_Primop_tok); _LL763:;} return yyzzz;} static
unsigned char _temp771[ 16u]="opt_t<primop_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp771, _temp771, _temp771 +  16u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ struct Cyc_Core_Opt*
yyzzz;{ void* _temp772= yy1; struct Cyc_Core_Opt* _temp778; _LL774: if(*(( void**)
_temp772) ==  Cyc_Primopopt_tok){ _LL779: _temp778=(( struct Cyc_Primopopt_tok_struct*)
_temp772)->f1; goto _LL775;} else{ goto _LL776;} _LL776: goto _LL777; _LL775:
yyzzz= _temp778; goto _LL773; _LL777:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL773:;} return yyzzz;} static unsigned char _temp781[ 7u]="qvar_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{
_temp781, _temp781, _temp781 +  7u}}; struct _tuple1* Cyc_yyget_QualId_tok( void*
yy1){ struct _tuple1* yyzzz;{ void* _temp782= yy1; struct _tuple1* _temp788;
_LL784: if(*(( void**) _temp782) ==  Cyc_QualId_tok){ _LL789: _temp788=(( struct
Cyc_QualId_tok_struct*) _temp782)->f1; goto _LL785;} else{ goto _LL786;} _LL786:
goto _LL787; _LL785: yyzzz= _temp788; goto _LL783; _LL787:( int) _throw(( void*)&
Cyc_yyfail_QualId_tok); _LL783:;} return yyzzz;} static unsigned char _temp791[
7u]="stmt_t"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{
_temp791, _temp791, _temp791 +  7u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok(
void* yy1){ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp792= yy1; struct Cyc_Absyn_Stmt*
_temp798; _LL794: if(*(( void**) _temp792) ==  Cyc_Stmt_tok){ _LL799: _temp798=((
struct Cyc_Stmt_tok_struct*) _temp792)->f1; goto _LL795;} else{ goto _LL796;}
_LL796: goto _LL797; _LL795: yyzzz= _temp798; goto _LL793; _LL797:( int) _throw((
void*)& Cyc_yyfail_Stmt_tok); _LL793:;} return yyzzz;} static unsigned char
_temp801[ 24u]="list_t<switch_clause_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp801, _temp801,
_temp801 +  24u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ struct Cyc_List_List* yyzzz;{ void* _temp802= yy1; struct Cyc_List_List*
_temp808; _LL804: if(*(( void**) _temp802) ==  Cyc_SwitchClauseList_tok){ _LL809:
_temp808=(( struct Cyc_SwitchClauseList_tok_struct*) _temp802)->f1; goto _LL805;}
else{ goto _LL806;} _LL806: goto _LL807; _LL805: yyzzz= _temp808; goto _LL803;
_LL807:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL803:;}
return yyzzz;} static unsigned char _temp811[ 25u]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp811, _temp811, _temp811 +  25u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp812= yy1; struct Cyc_List_List*
_temp818; _LL814: if(*(( void**) _temp812) ==  Cyc_SwitchCClauseList_tok){
_LL819: _temp818=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp812)->f1;
goto _LL815;} else{ goto _LL816;} _LL816: goto _LL817; _LL815: yyzzz= _temp818;
goto _LL813; _LL817:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL813:;} return yyzzz;} static unsigned char _temp821[ 6u]="pat_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{
_temp821, _temp821, _temp821 +  6u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
void* yy1){ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp822= yy1; struct Cyc_Absyn_Pat*
_temp828; _LL824: if(*(( void**) _temp822) ==  Cyc_Pattern_tok){ _LL829:
_temp828=(( struct Cyc_Pattern_tok_struct*) _temp822)->f1; goto _LL825;} else{
goto _LL826;} _LL826: goto _LL827; _LL825: yyzzz= _temp828; goto _LL823; _LL827:(
int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL823:;} return yyzzz;} static
unsigned char _temp831[ 14u]="list_t<pat_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp831, _temp831, _temp831 + 
14u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp832= yy1; struct Cyc_List_List* _temp838; _LL834: if(*(( void**)
_temp832) ==  Cyc_PatternList_tok){ _LL839: _temp838=(( struct Cyc_PatternList_tok_struct*)
_temp832)->f1; goto _LL835;} else{ goto _LL836;} _LL836: goto _LL837; _LL835:
yyzzz= _temp838; goto _LL833; _LL837:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL833:;} return yyzzz;} static unsigned char _temp841[ 31u]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp841, _temp841, _temp841 +  31u}}; struct _tuple13* Cyc_yyget_FieldPattern_tok(
void* yy1){ struct _tuple13* yyzzz;{ void* _temp842= yy1; struct _tuple13*
_temp848; _LL844: if(*(( void**) _temp842) ==  Cyc_FieldPattern_tok){ _LL849:
_temp848=(( struct Cyc_FieldPattern_tok_struct*) _temp842)->f1; goto _LL845;}
else{ goto _LL846;} _LL846: goto _LL847; _LL845: yyzzz= _temp848; goto _LL843;
_LL847:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL843:;} return
yyzzz;} static unsigned char _temp851[ 39u]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp851, _temp851, _temp851 +  39u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp852= yy1; struct Cyc_List_List*
_temp858; _LL854: if(*(( void**) _temp852) ==  Cyc_FieldPatternList_tok){ _LL859:
_temp858=(( struct Cyc_FieldPatternList_tok_struct*) _temp852)->f1; goto _LL855;}
else{ goto _LL856;} _LL856: goto _LL857; _LL855: yyzzz= _temp858; goto _LL853;
_LL857:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL853:;}
return yyzzz;} static unsigned char _temp861[ 9u]="fndecl_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp861, _temp861, _temp861 +  9u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ struct Cyc_Absyn_Fndecl*
yyzzz;{ void* _temp862= yy1; struct Cyc_Absyn_Fndecl* _temp868; _LL864: if(*((
void**) _temp862) ==  Cyc_FnDecl_tok){ _LL869: _temp868=(( struct Cyc_FnDecl_tok_struct*)
_temp862)->f1; goto _LL865;} else{ goto _LL866;} _LL866: goto _LL867; _LL865:
yyzzz= _temp868; goto _LL863; _LL867:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL863:;} return yyzzz;} static unsigned char _temp871[ 15u]="list_t<decl_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp871, _temp871, _temp871 +  15u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp872= yy1; struct Cyc_List_List*
_temp878; _LL874: if(*(( void**) _temp872) ==  Cyc_DeclList_tok){ _LL879:
_temp878=(( struct Cyc_DeclList_tok_struct*) _temp872)->f1; goto _LL875;} else{
goto _LL876;} _LL876: goto _LL877; _LL875: yyzzz= _temp878; goto _LL873; _LL877:(
int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL873:;} return yyzzz;} static
unsigned char _temp881[ 12u]="decl_spec_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp881, _temp881, _temp881 +  12u}};
struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){ struct
Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp882= yy1; struct Cyc_Parse_Declaration_spec*
_temp888; _LL884: if(*(( void**) _temp882) ==  Cyc_DeclSpec_tok){ _LL889:
_temp888=(( struct Cyc_DeclSpec_tok_struct*) _temp882)->f1; goto _LL885;} else{
goto _LL886;} _LL886: goto _LL887; _LL885: yyzzz= _temp888; goto _LL883; _LL887:(
int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL883:;} return yyzzz;} static
unsigned char _temp891[ 27u]="$(declarator_t,exp_opt_t)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp891, _temp891, _temp891 +  27u}};
struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){ struct _tuple14* yyzzz;{
void* _temp892= yy1; struct _tuple14* _temp898; _LL894: if(*(( void**) _temp892)
==  Cyc_InitDecl_tok){ _LL899: _temp898=(( struct Cyc_InitDecl_tok_struct*)
_temp892)->f1; goto _LL895;} else{ goto _LL896;} _LL896: goto _LL897; _LL895:
yyzzz= _temp898; goto _LL893; _LL897:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL893:;} return yyzzz;} static unsigned char _temp901[ 35u]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp901, _temp901, _temp901 +  35u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp902= yy1; struct Cyc_List_List*
_temp908; _LL904: if(*(( void**) _temp902) ==  Cyc_InitDeclList_tok){ _LL909:
_temp908=(( struct Cyc_InitDeclList_tok_struct*) _temp902)->f1; goto _LL905;}
else{ goto _LL906;} _LL906: goto _LL907; _LL905: yyzzz= _temp908; goto _LL903;
_LL907:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL903:;} return
yyzzz;} static unsigned char _temp911[ 16u]="storage_class_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp911, _temp911, _temp911 + 
16u}}; void* Cyc_yyget_StorageClass_tok( void* yy1){ void* yyzzz;{ void*
_temp912= yy1; void* _temp918; _LL914: if(*(( void**) _temp912) ==  Cyc_StorageClass_tok){
_LL919: _temp918=( void*)(( struct Cyc_StorageClass_tok_struct*) _temp912)->f1;
goto _LL915;} else{ goto _LL916;} _LL916: goto _LL917; _LL915: yyzzz= _temp918;
goto _LL913; _LL917:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL913:;}
return yyzzz;} static unsigned char _temp921[ 17u]="type_specifier_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp921, _temp921, _temp921 +  17u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* yyzzz;{ void* _temp922= yy1; void* _temp928; _LL924: if(*(( void**)
_temp922) ==  Cyc_TypeSpecifier_tok){ _LL929: _temp928=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp922)->f1; goto _LL925;} else{ goto _LL926;} _LL926: goto _LL927; _LL925:
yyzzz= _temp928; goto _LL923; _LL927:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL923:;} return yyzzz;} static unsigned char _temp931[ 18u]="struct_or_union_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp931, _temp931, _temp931 +  18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ enum  Cyc_Parse_Struct_or_union yyzzz;{ void* _temp932= yy1; enum 
Cyc_Parse_Struct_or_union _temp938; _LL934: if(*(( void**) _temp932) ==  Cyc_StructOrUnion_tok){
_LL939: _temp938=(( struct Cyc_StructOrUnion_tok_struct*) _temp932)->f1; goto
_LL935;} else{ goto _LL936;} _LL936: goto _LL937; _LL935: yyzzz= _temp938; goto
_LL933; _LL937:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok); _LL933:;}
return yyzzz;} static unsigned char _temp941[ 8u]="tqual_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp941, _temp941, _temp941 +  8u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp942= yy1; struct Cyc_Absyn_Tqual _temp948; _LL944: if(*((
void**) _temp942) ==  Cyc_TypeQual_tok){ _LL949: _temp948=(( struct Cyc_TypeQual_tok_struct*)
_temp942)->f1; goto _LL945;} else{ goto _LL946;} _LL946: goto _LL947; _LL945:
yyzzz= _temp948; goto _LL943; _LL947:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL943:;} return yyzzz;} static unsigned char _temp951[ 22u]="list_t<structfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp951, _temp951, _temp951 +  22u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp952= yy1; struct Cyc_List_List*
_temp958; _LL954: if(*(( void**) _temp952) ==  Cyc_StructFieldDeclList_tok){
_LL959: _temp958=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp952)->f1;
goto _LL955;} else{ goto _LL956;} _LL956: goto _LL957; _LL955: yyzzz= _temp958;
goto _LL953; _LL957:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL953:;} return yyzzz;} static unsigned char _temp961[ 30u]="list_t<list_t<structfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp961, _temp961, _temp961 +  30u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{
void* _temp962= yy1; struct Cyc_List_List* _temp968; _LL964: if(*(( void**)
_temp962) ==  Cyc_StructFieldDeclListList_tok){ _LL969: _temp968=(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp962)->f1; goto _LL965;} else{ goto _LL966;} _LL966: goto _LL967; _LL965:
yyzzz= _temp968; goto _LL963; _LL967:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL963:;} return yyzzz;} static unsigned char _temp971[ 24u]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{
_temp971, _temp971, _temp971 +  24u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp972= yy1; struct Cyc_List_List*
_temp978; _LL974: if(*(( void**) _temp972) ==  Cyc_TypeModifierList_tok){ _LL979:
_temp978=(( struct Cyc_TypeModifierList_tok_struct*) _temp972)->f1; goto _LL975;}
else{ goto _LL976;} _LL976: goto _LL977; _LL975: yyzzz= _temp978; goto _LL973;
_LL977:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL973:;}
return yyzzz;} static unsigned char _temp981[ 13u]="declarator_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp981,
_temp981, _temp981 +  13u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp982= yy1; struct Cyc_Parse_Declarator*
_temp988; _LL984: if(*(( void**) _temp982) ==  Cyc_Declarator_tok){ _LL989:
_temp988=(( struct Cyc_Declarator_tok_struct*) _temp982)->f1; goto _LL985;}
else{ goto _LL986;} _LL986: goto _LL987; _LL985: yyzzz= _temp988; goto _LL983;
_LL987:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL983:;} return
yyzzz;} static unsigned char _temp991[ 21u]="abstractdeclarator_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{
_temp991, _temp991, _temp991 +  21u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp992= yy1;
struct Cyc_Parse_Abstractdeclarator* _temp998; _LL994: if(*(( void**) _temp992)
==  Cyc_AbstractDeclarator_tok){ _LL999: _temp998=(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp992)->f1; goto _LL995;} else{ goto _LL996;} _LL996: goto _LL997; _LL995:
yyzzz= _temp998; goto _LL993; _LL997:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL993:;} return yyzzz;} static unsigned char _temp1001[ 5u]="bool"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{
_temp1001, _temp1001, _temp1001 +  5u}}; int Cyc_yyget_Bool_tok( void* yy1){ int
yyzzz;{ void* _temp1002= yy1; int _temp1008; _LL1004: if(*(( void**) _temp1002)
==  Cyc_Bool_tok){ _LL1009: _temp1008=(( struct Cyc_Bool_tok_struct*) _temp1002)->f1;
goto _LL1005;} else{ goto _LL1006;} _LL1006: goto _LL1007; _LL1005: yyzzz=
_temp1008; goto _LL1003; _LL1007:( int) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1003:;} return yyzzz;} static unsigned char _temp1011[ 8u]="scope_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{
_temp1011, _temp1011, _temp1011 +  8u}}; void* Cyc_yyget_Scope_tok( void* yy1){
void* yyzzz;{ void* _temp1012= yy1; void* _temp1018; _LL1014: if(*(( void**)
_temp1012) ==  Cyc_Scope_tok){ _LL1019: _temp1018=( void*)(( struct Cyc_Scope_tok_struct*)
_temp1012)->f1; goto _LL1015;} else{ goto _LL1016;} _LL1016: goto _LL1017;
_LL1015: yyzzz= _temp1018; goto _LL1013; _LL1017:( int) _throw(( void*)& Cyc_yyfail_Scope_tok);
_LL1013:;} return yyzzz;} static unsigned char _temp1021[ 14u]="tunionfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1021, _temp1021, _temp1021 +  14u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp1022= yy1; struct
Cyc_Absyn_Tunionfield* _temp1028; _LL1024: if(*(( void**) _temp1022) ==  Cyc_TunionField_tok){
_LL1029: _temp1028=(( struct Cyc_TunionField_tok_struct*) _temp1022)->f1; goto
_LL1025;} else{ goto _LL1026;} _LL1026: goto _LL1027; _LL1025: yyzzz= _temp1028;
goto _LL1023; _LL1027:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL1023:;} return yyzzz;} static unsigned char _temp1031[ 22u]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp1031, _temp1031, _temp1031 +  22u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1032= yy1; struct Cyc_List_List*
_temp1038; _LL1034: if(*(( void**) _temp1032) ==  Cyc_TunionFieldList_tok){
_LL1039: _temp1038=(( struct Cyc_TunionFieldList_tok_struct*) _temp1032)->f1;
goto _LL1035;} else{ goto _LL1036;} _LL1036: goto _LL1037; _LL1035: yyzzz=
_temp1038; goto _LL1033; _LL1037:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1033:;} return yyzzz;} static unsigned char _temp1041[ 50u]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1041, _temp1041, _temp1041 +  50u}}; struct _tuple17* Cyc_yyget_QualSpecList_tok(
void* yy1){ struct _tuple17* yyzzz;{ void* _temp1042= yy1; struct _tuple17*
_temp1048; _LL1044: if(*(( void**) _temp1042) ==  Cyc_QualSpecList_tok){ _LL1049:
_temp1048=(( struct Cyc_QualSpecList_tok_struct*) _temp1042)->f1; goto _LL1045;}
else{ goto _LL1046;} _LL1046: goto _LL1047; _LL1045: yyzzz= _temp1048; goto
_LL1043; _LL1047:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL1043:;}
return yyzzz;} static unsigned char _temp1051[ 14u]="list_t<var_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{
_temp1051, _temp1051, _temp1051 +  14u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1052= yy1; struct Cyc_List_List*
_temp1058; _LL1054: if(*(( void**) _temp1052) ==  Cyc_IdList_tok){ _LL1059:
_temp1058=(( struct Cyc_IdList_tok_struct*) _temp1052)->f1; goto _LL1055;} else{
goto _LL1056;} _LL1056: goto _LL1057; _LL1055: yyzzz= _temp1058; goto _LL1053;
_LL1057:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1053:;} return yyzzz;}
static unsigned char _temp1061[ 32u]="$(opt_t<var_t>,tqual_t,type_t)@"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{
_temp1061, _temp1061, _temp1061 +  32u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ struct _tuple2* yyzzz;{ void* _temp1062= yy1; struct _tuple2*
_temp1068; _LL1064: if(*(( void**) _temp1062) ==  Cyc_ParamDecl_tok){ _LL1069:
_temp1068=(( struct Cyc_ParamDecl_tok_struct*) _temp1062)->f1; goto _LL1065;}
else{ goto _LL1066;} _LL1066: goto _LL1067; _LL1065: yyzzz= _temp1068; goto
_LL1063; _LL1067:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1063:;}
return yyzzz;} static unsigned char _temp1071[ 40u]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{
_temp1071, _temp1071, _temp1071 +  40u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1072= yy1; struct Cyc_List_List*
_temp1078; _LL1074: if(*(( void**) _temp1072) ==  Cyc_ParamDeclList_tok){
_LL1079: _temp1078=(( struct Cyc_ParamDeclList_tok_struct*) _temp1072)->f1; goto
_LL1075;} else{ goto _LL1076;} _LL1076: goto _LL1077; _LL1075: yyzzz= _temp1078;
goto _LL1073; _LL1077:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1073:;} return yyzzz;} static unsigned char _temp1081[ 107u]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={ Cyc_Core_Failure,{
_temp1081, _temp1081, _temp1081 +  107u}}; struct _tuple16* Cyc_yyget_YY1( void*
yy1){ struct _tuple16* yyzzz;{ void* _temp1082= yy1; struct _tuple16* _temp1088;
_LL1084: if(*(( void**) _temp1082) ==  Cyc_YY1){ _LL1089: _temp1088=(( struct
Cyc_YY1_struct*) _temp1082)->f1; goto _LL1085;} else{ goto _LL1086;} _LL1086:
goto _LL1087; _LL1085: yyzzz= _temp1088; goto _LL1083; _LL1087:( int) _throw((
void*)& Cyc_yyfail_YY1); _LL1083:;} return yyzzz;} static unsigned char
_temp1091[ 15u]="list_t<type_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1091, _temp1091, _temp1091 +  15u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void*
_temp1092= yy1; struct Cyc_List_List* _temp1098; _LL1094: if(*(( void**)
_temp1092) ==  Cyc_TypeList_tok){ _LL1099: _temp1098=(( struct Cyc_TypeList_tok_struct*)
_temp1092)->f1; goto _LL1095;} else{ goto _LL1096;} _LL1096: goto _LL1097;
_LL1095: yyzzz= _temp1098; goto _LL1093; _LL1097:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok);
_LL1093:;} return yyzzz;} static unsigned char _temp1101[ 21u]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{
_temp1101, _temp1101, _temp1101 +  21u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1102= yy1; struct Cyc_List_List*
_temp1108; _LL1104: if(*(( void**) _temp1102) ==  Cyc_DesignatorList_tok){
_LL1109: _temp1108=(( struct Cyc_DesignatorList_tok_struct*) _temp1102)->f1;
goto _LL1105;} else{ goto _LL1106;} _LL1106: goto _LL1107; _LL1105: yyzzz=
_temp1108; goto _LL1103; _LL1107:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1103:;} return yyzzz;} static unsigned char _temp1111[ 13u]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{
_temp1111, _temp1111, _temp1111 +  13u}}; void* Cyc_yyget_Designator_tok( void*
yy1){ void* yyzzz;{ void* _temp1112= yy1; void* _temp1118; _LL1114: if(*(( void**)
_temp1112) ==  Cyc_Designator_tok){ _LL1119: _temp1118=( void*)(( struct Cyc_Designator_tok_struct*)
_temp1112)->f1; goto _LL1115;} else{ goto _LL1116;} _LL1116: goto _LL1117;
_LL1115: yyzzz= _temp1118; goto _LL1113; _LL1117:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL1113:;} return yyzzz;} static unsigned char _temp1121[ 7u]="kind_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{
_temp1121, _temp1121, _temp1121 +  7u}}; void* Cyc_yyget_Kind_tok( void* yy1){
void* yyzzz;{ void* _temp1122= yy1; void* _temp1128; _LL1124: if(*(( void**)
_temp1122) ==  Cyc_Kind_tok){ _LL1129: _temp1128=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1122)->f1; goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127;
_LL1125: yyzzz= _temp1128; goto _LL1123; _LL1127:( int) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1123:;} return yyzzz;} static unsigned char _temp1131[ 7u]="type_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1131, _temp1131, _temp1131 +  7u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* yyzzz;{ void* _temp1132= yy1; void* _temp1138; _LL1134: if(*(( void**)
_temp1132) ==  Cyc_Type_tok){ _LL1139: _temp1138=( void*)(( struct Cyc_Type_tok_struct*)
_temp1132)->f1; goto _LL1135;} else{ goto _LL1136;} _LL1136: goto _LL1137;
_LL1135: yyzzz= _temp1138; goto _LL1133; _LL1137:( int) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1133:;} return yyzzz;} static unsigned char _temp1141[ 20u]="list_t<attribute_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1141, _temp1141, _temp1141 +  20u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1142= yy1; struct Cyc_List_List*
_temp1148; _LL1144: if(*(( void**) _temp1142) ==  Cyc_AttributeList_tok){
_LL1149: _temp1148=(( struct Cyc_AttributeList_tok_struct*) _temp1142)->f1; goto
_LL1145;} else{ goto _LL1146;} _LL1146: goto _LL1147; _LL1145: yyzzz= _temp1148;
goto _LL1143; _LL1147:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1143:;} return yyzzz;} static unsigned char _temp1151[ 12u]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{
_temp1151, _temp1151, _temp1151 +  12u}}; void* Cyc_yyget_Attribute_tok( void*
yy1){ void* yyzzz;{ void* _temp1152= yy1; void* _temp1158; _LL1154: if(*(( void**)
_temp1152) ==  Cyc_Attribute_tok){ _LL1159: _temp1158=( void*)(( struct Cyc_Attribute_tok_struct*)
_temp1152)->f1; goto _LL1155;} else{ goto _LL1156;} _LL1156: goto _LL1157;
_LL1155: yyzzz= _temp1158; goto _LL1153; _LL1157:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1153:;} return yyzzz;} static unsigned char _temp1161[ 12u]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{
_temp1161, _temp1161, _temp1161 +  12u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
void* yy1){ struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp1162= yy1; struct Cyc_Absyn_Enumfield*
_temp1168; _LL1164: if(*(( void**) _temp1162) ==  Cyc_Enumfield_tok){ _LL1169:
_temp1168=(( struct Cyc_Enumfield_tok_struct*) _temp1162)->f1; goto _LL1165;}
else{ goto _LL1166;} _LL1166: goto _LL1167; _LL1165: yyzzz= _temp1168; goto
_LL1163; _LL1167:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1163:;}
return yyzzz;} static unsigned char _temp1171[ 20u]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1171, _temp1171, _temp1171 +  20u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1172= yy1; struct Cyc_List_List*
_temp1178; _LL1174: if(*(( void**) _temp1172) ==  Cyc_EnumfieldList_tok){
_LL1179: _temp1178=(( struct Cyc_EnumfieldList_tok_struct*) _temp1172)->f1; goto
_LL1175;} else{ goto _LL1176;} _LL1176: goto _LL1177; _LL1175: yyzzz= _temp1178;
goto _LL1173; _LL1177:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1173:;} return yyzzz;} static unsigned char _temp1181[ 14u]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{
_temp1181, _temp1181, _temp1181 +  14u}}; struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok(
void* yy1){ struct Cyc_Core_Opt* yyzzz;{ void* _temp1182= yy1; struct Cyc_Core_Opt*
_temp1188; _LL1184: if(*(( void**) _temp1182) ==  Cyc_TypeOpt_tok){ _LL1189:
_temp1188=(( struct Cyc_TypeOpt_tok_struct*) _temp1182)->f1; goto _LL1185;}
else{ goto _LL1186;} _LL1186: goto _LL1187; _LL1185: yyzzz= _temp1188; goto
_LL1183; _LL1187:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1183:;}
return yyzzz;} static unsigned char _temp1191[ 26u]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{
_temp1191, _temp1191, _temp1191 +  26u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1192= yy1; struct Cyc_List_List*
_temp1198; _LL1194: if(*(( void**) _temp1192) ==  Cyc_Rgnorder_tok){ _LL1199:
_temp1198=(( struct Cyc_Rgnorder_tok_struct*) _temp1192)->f1; goto _LL1195;}
else{ goto _LL1196;} _LL1196: goto _LL1197; _LL1195: yyzzz= _temp1198; goto
_LL1193; _LL1197:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1193:;}
return yyzzz;} struct Cyc_Yyltype{ int timestamp; int first_line; int
first_column; int last_line; int last_column; } ; struct Cyc_Yyltype Cyc_yynewloc(){
return({ struct Cyc_Yyltype _temp1200; _temp1200.timestamp= 0; _temp1200.first_line=
0; _temp1200.first_column= 0; _temp1200.last_line= 0; _temp1200.last_column= 0;
_temp1200;});} struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[
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
unsigned char _temp1201[ 2u]="$"; static unsigned char _temp1202[ 6u]="error";
static unsigned char _temp1203[ 12u]="$undefined."; static unsigned char
_temp1204[ 5u]="AUTO"; static unsigned char _temp1205[ 9u]="REGISTER"; static
unsigned char _temp1206[ 7u]="STATIC"; static unsigned char _temp1207[ 7u]="EXTERN";
static unsigned char _temp1208[ 8u]="TYPEDEF"; static unsigned char _temp1209[ 5u]="VOID";
static unsigned char _temp1210[ 5u]="CHAR"; static unsigned char _temp1211[ 6u]="SHORT";
static unsigned char _temp1212[ 4u]="INT"; static unsigned char _temp1213[ 5u]="LONG";
static unsigned char _temp1214[ 6u]="FLOAT"; static unsigned char _temp1215[ 7u]="DOUBLE";
static unsigned char _temp1216[ 7u]="SIGNED"; static unsigned char _temp1217[ 9u]="UNSIGNED";
static unsigned char _temp1218[ 6u]="CONST"; static unsigned char _temp1219[ 9u]="VOLATILE";
static unsigned char _temp1220[ 9u]="RESTRICT"; static unsigned char _temp1221[
7u]="STRUCT"; static unsigned char _temp1222[ 6u]="UNION"; static unsigned char
_temp1223[ 5u]="CASE"; static unsigned char _temp1224[ 8u]="DEFAULT"; static
unsigned char _temp1225[ 7u]="INLINE"; static unsigned char _temp1226[ 7u]="SIZEOF";
static unsigned char _temp1227[ 9u]="OFFSETOF"; static unsigned char _temp1228[
3u]="IF"; static unsigned char _temp1229[ 5u]="ELSE"; static unsigned char
_temp1230[ 7u]="SWITCH"; static unsigned char _temp1231[ 6u]="WHILE"; static
unsigned char _temp1232[ 3u]="DO"; static unsigned char _temp1233[ 4u]="FOR";
static unsigned char _temp1234[ 5u]="GOTO"; static unsigned char _temp1235[ 9u]="CONTINUE";
static unsigned char _temp1236[ 6u]="BREAK"; static unsigned char _temp1237[ 7u]="RETURN";
static unsigned char _temp1238[ 5u]="ENUM"; static unsigned char _temp1239[ 8u]="NULL_kw";
static unsigned char _temp1240[ 4u]="LET"; static unsigned char _temp1241[ 6u]="THROW";
static unsigned char _temp1242[ 4u]="TRY"; static unsigned char _temp1243[ 6u]="CATCH";
static unsigned char _temp1244[ 4u]="NEW"; static unsigned char _temp1245[ 9u]="ABSTRACT";
static unsigned char _temp1246[ 9u]="FALLTHRU"; static unsigned char _temp1247[
6u]="USING"; static unsigned char _temp1248[ 10u]="NAMESPACE"; static
unsigned char _temp1249[ 7u]="TUNION"; static unsigned char _temp1250[ 8u]="XTUNION";
static unsigned char _temp1251[ 5u]="FILL"; static unsigned char _temp1252[ 8u]="CODEGEN";
static unsigned char _temp1253[ 4u]="CUT"; static unsigned char _temp1254[ 7u]="SPLICE";
static unsigned char _temp1255[ 7u]="MALLOC"; static unsigned char _temp1256[ 9u]="REGION_T";
static unsigned char _temp1257[ 7u]="REGION"; static unsigned char _temp1258[ 5u]="RNEW";
static unsigned char _temp1259[ 8u]="RMALLOC"; static unsigned char _temp1260[ 8u]="REGIONS";
static unsigned char _temp1261[ 4u]="GEN"; static unsigned char _temp1262[ 7u]="PTR_OP";
static unsigned char _temp1263[ 7u]="INC_OP"; static unsigned char _temp1264[ 7u]="DEC_OP";
static unsigned char _temp1265[ 8u]="LEFT_OP"; static unsigned char _temp1266[ 9u]="RIGHT_OP";
static unsigned char _temp1267[ 6u]="LE_OP"; static unsigned char _temp1268[ 6u]="GE_OP";
static unsigned char _temp1269[ 6u]="EQ_OP"; static unsigned char _temp1270[ 6u]="NE_OP";
static unsigned char _temp1271[ 7u]="AND_OP"; static unsigned char _temp1272[ 6u]="OR_OP";
static unsigned char _temp1273[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1274[ 11u]="DIV_ASSIGN"; static unsigned char _temp1275[ 11u]="MOD_ASSIGN";
static unsigned char _temp1276[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1277[ 11u]="SUB_ASSIGN"; static unsigned char _temp1278[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1279[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1280[ 11u]="AND_ASSIGN"; static unsigned char _temp1281[ 11u]="XOR_ASSIGN";
static unsigned char _temp1282[ 10u]="OR_ASSIGN"; static unsigned char _temp1283[
9u]="ELLIPSIS"; static unsigned char _temp1284[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1285[ 12u]="COLON_COLON"; static unsigned char _temp1286[ 11u]="IDENTIFIER";
static unsigned char _temp1287[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1288[ 7u]="STRING"; static unsigned char _temp1289[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1290[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1291[ 9u]="TYPE_VAR"; static unsigned char _temp1292[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1293[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1294[ 10u]="ATTRIBUTE"; static unsigned char _temp1295[ 4u]="';'"; static
unsigned char _temp1296[ 4u]="'{'"; static unsigned char _temp1297[ 4u]="'}'";
static unsigned char _temp1298[ 4u]="'='"; static unsigned char _temp1299[ 4u]="'('";
static unsigned char _temp1300[ 4u]="')'"; static unsigned char _temp1301[ 4u]="','";
static unsigned char _temp1302[ 4u]="'_'"; static unsigned char _temp1303[ 4u]="'$'";
static unsigned char _temp1304[ 4u]="'<'"; static unsigned char _temp1305[ 4u]="'>'";
static unsigned char _temp1306[ 4u]="':'"; static unsigned char _temp1307[ 4u]="'.'";
static unsigned char _temp1308[ 4u]="'['"; static unsigned char _temp1309[ 4u]="']'";
static unsigned char _temp1310[ 4u]="'*'"; static unsigned char _temp1311[ 4u]="'@'";
static unsigned char _temp1312[ 4u]="'?'"; static unsigned char _temp1313[ 4u]="'+'";
static unsigned char _temp1314[ 4u]="'-'"; static unsigned char _temp1315[ 4u]="'&'";
static unsigned char _temp1316[ 4u]="'|'"; static unsigned char _temp1317[ 4u]="'^'";
static unsigned char _temp1318[ 4u]="'/'"; static unsigned char _temp1319[ 4u]="'%'";
static unsigned char _temp1320[ 4u]="'~'"; static unsigned char _temp1321[ 4u]="'!'";
static unsigned char _temp1322[ 5u]="prog"; static unsigned char _temp1323[ 17u]="translation_unit";
static unsigned char _temp1324[ 21u]="external_declaration"; static
unsigned char _temp1325[ 20u]="function_definition"; static unsigned char
_temp1326[ 21u]="function_definition2"; static unsigned char _temp1327[ 13u]="using_action";
static unsigned char _temp1328[ 15u]="unusing_action"; static unsigned char
_temp1329[ 17u]="namespace_action"; static unsigned char _temp1330[ 19u]="unnamespace_action";
static unsigned char _temp1331[ 12u]="declaration"; static unsigned char
_temp1332[ 17u]="declaration_list"; static unsigned char _temp1333[ 23u]="declaration_specifiers";
static unsigned char _temp1334[ 24u]="storage_class_specifier"; static
unsigned char _temp1335[ 15u]="attributes_opt"; static unsigned char _temp1336[
11u]="attributes"; static unsigned char _temp1337[ 15u]="attribute_list"; static
unsigned char _temp1338[ 10u]="attribute"; static unsigned char _temp1339[ 15u]="type_specifier";
static unsigned char _temp1340[ 5u]="kind"; static unsigned char _temp1341[ 15u]="type_qualifier";
static unsigned char _temp1342[ 15u]="enum_specifier"; static unsigned char
_temp1343[ 11u]="enum_field"; static unsigned char _temp1344[ 22u]="enum_declaration_list";
static unsigned char _temp1345[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1346[ 16u]="type_params_opt"; static unsigned char _temp1347[
16u]="struct_or_union"; static unsigned char _temp1348[ 24u]="struct_declaration_list";
static unsigned char _temp1349[ 25u]="struct_declaration_list0"; static
unsigned char _temp1350[ 21u]="init_declarator_list"; static unsigned char
_temp1351[ 22u]="init_declarator_list0"; static unsigned char _temp1352[ 16u]="init_declarator";
static unsigned char _temp1353[ 19u]="struct_declaration"; static unsigned char
_temp1354[ 25u]="specifier_qualifier_list"; static unsigned char _temp1355[ 23u]="struct_declarator_list";
static unsigned char _temp1356[ 24u]="struct_declarator_list0"; static
unsigned char _temp1357[ 18u]="struct_declarator"; static unsigned char
_temp1358[ 17u]="tunion_specifier"; static unsigned char _temp1359[ 18u]="tunion_or_xtunion";
static unsigned char _temp1360[ 17u]="tunionfield_list"; static unsigned char
_temp1361[ 18u]="tunionfield_scope"; static unsigned char _temp1362[ 12u]="tunionfield";
static unsigned char _temp1363[ 11u]="declarator"; static unsigned char
_temp1364[ 18u]="direct_declarator"; static unsigned char _temp1365[ 8u]="pointer";
static unsigned char _temp1366[ 13u]="pointer_char"; static unsigned char
_temp1367[ 8u]="rgn_opt"; static unsigned char _temp1368[ 4u]="rgn"; static
unsigned char _temp1369[ 20u]="type_qualifier_list"; static unsigned char
_temp1370[ 20u]="parameter_type_list"; static unsigned char _temp1371[ 9u]="type_var";
static unsigned char _temp1372[ 16u]="optional_effect"; static unsigned char
_temp1373[ 19u]="optional_rgn_order"; static unsigned char _temp1374[ 10u]="rgn_order";
static unsigned char _temp1375[ 16u]="optional_inject"; static unsigned char
_temp1376[ 11u]="effect_set"; static unsigned char _temp1377[ 14u]="atomic_effect";
static unsigned char _temp1378[ 11u]="region_set"; static unsigned char
_temp1379[ 15u]="parameter_list"; static unsigned char _temp1380[ 22u]="parameter_declaration";
static unsigned char _temp1381[ 16u]="identifier_list"; static unsigned char
_temp1382[ 17u]="identifier_list0"; static unsigned char _temp1383[ 12u]="initializer";
static unsigned char _temp1384[ 18u]="array_initializer"; static unsigned char
_temp1385[ 17u]="initializer_list"; static unsigned char _temp1386[ 12u]="designation";
static unsigned char _temp1387[ 16u]="designator_list"; static unsigned char
_temp1388[ 11u]="designator"; static unsigned char _temp1389[ 10u]="type_name";
static unsigned char _temp1390[ 14u]="any_type_name"; static unsigned char
_temp1391[ 15u]="type_name_list"; static unsigned char _temp1392[ 20u]="abstract_declarator";
static unsigned char _temp1393[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1394[ 10u]="statement"; static unsigned char _temp1395[ 18u]="labeled_statement";
static unsigned char _temp1396[ 21u]="expression_statement"; static
unsigned char _temp1397[ 19u]="compound_statement"; static unsigned char
_temp1398[ 16u]="block_item_list"; static unsigned char _temp1399[ 20u]="selection_statement";
static unsigned char _temp1400[ 15u]="switch_clauses"; static unsigned char
_temp1401[ 16u]="switchC_clauses"; static unsigned char _temp1402[ 20u]="iteration_statement";
static unsigned char _temp1403[ 15u]="jump_statement"; static unsigned char
_temp1404[ 8u]="pattern"; static unsigned char _temp1405[ 19u]="tuple_pattern_list";
static unsigned char _temp1406[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1407[ 14u]="field_pattern"; static unsigned char _temp1408[ 19u]="field_pattern_list";
static unsigned char _temp1409[ 20u]="field_pattern_list0"; static unsigned char
_temp1410[ 11u]="expression"; static unsigned char _temp1411[ 22u]="assignment_expression";
static unsigned char _temp1412[ 20u]="assignment_operator"; static unsigned char
_temp1413[ 23u]="conditional_expression"; static unsigned char _temp1414[ 20u]="constant_expression";
static unsigned char _temp1415[ 22u]="logical_or_expression"; static
unsigned char _temp1416[ 23u]="logical_and_expression"; static unsigned char
_temp1417[ 24u]="inclusive_or_expression"; static unsigned char _temp1418[ 24u]="exclusive_or_expression";
static unsigned char _temp1419[ 15u]="and_expression"; static unsigned char
_temp1420[ 20u]="equality_expression"; static unsigned char _temp1421[ 22u]="relational_expression";
static unsigned char _temp1422[ 17u]="shift_expression"; static unsigned char
_temp1423[ 20u]="additive_expression"; static unsigned char _temp1424[ 26u]="multiplicative_expression";
static unsigned char _temp1425[ 16u]="cast_expression"; static unsigned char
_temp1426[ 17u]="unary_expression"; static unsigned char _temp1427[ 15u]="unary_operator";
static unsigned char _temp1428[ 19u]="postfix_expression"; static unsigned char
_temp1429[ 19u]="primary_expression"; static unsigned char _temp1430[ 25u]="argument_expression_list";
static unsigned char _temp1431[ 26u]="argument_expression_list0"; static
unsigned char _temp1432[ 9u]="constant"; static unsigned char _temp1433[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 233u]={{ _temp1201, _temp1201, _temp1201
+  2u},{ _temp1202, _temp1202, _temp1202 +  6u},{ _temp1203, _temp1203,
_temp1203 +  12u},{ _temp1204, _temp1204, _temp1204 +  5u},{ _temp1205,
_temp1205, _temp1205 +  9u},{ _temp1206, _temp1206, _temp1206 +  7u},{ _temp1207,
_temp1207, _temp1207 +  7u},{ _temp1208, _temp1208, _temp1208 +  8u},{ _temp1209,
_temp1209, _temp1209 +  5u},{ _temp1210, _temp1210, _temp1210 +  5u},{ _temp1211,
_temp1211, _temp1211 +  6u},{ _temp1212, _temp1212, _temp1212 +  4u},{ _temp1213,
_temp1213, _temp1213 +  5u},{ _temp1214, _temp1214, _temp1214 +  6u},{ _temp1215,
_temp1215, _temp1215 +  7u},{ _temp1216, _temp1216, _temp1216 +  7u},{ _temp1217,
_temp1217, _temp1217 +  9u},{ _temp1218, _temp1218, _temp1218 +  6u},{ _temp1219,
_temp1219, _temp1219 +  9u},{ _temp1220, _temp1220, _temp1220 +  9u},{ _temp1221,
_temp1221, _temp1221 +  7u},{ _temp1222, _temp1222, _temp1222 +  6u},{ _temp1223,
_temp1223, _temp1223 +  5u},{ _temp1224, _temp1224, _temp1224 +  8u},{ _temp1225,
_temp1225, _temp1225 +  7u},{ _temp1226, _temp1226, _temp1226 +  7u},{ _temp1227,
_temp1227, _temp1227 +  9u},{ _temp1228, _temp1228, _temp1228 +  3u},{ _temp1229,
_temp1229, _temp1229 +  5u},{ _temp1230, _temp1230, _temp1230 +  7u},{ _temp1231,
_temp1231, _temp1231 +  6u},{ _temp1232, _temp1232, _temp1232 +  3u},{ _temp1233,
_temp1233, _temp1233 +  4u},{ _temp1234, _temp1234, _temp1234 +  5u},{ _temp1235,
_temp1235, _temp1235 +  9u},{ _temp1236, _temp1236, _temp1236 +  6u},{ _temp1237,
_temp1237, _temp1237 +  7u},{ _temp1238, _temp1238, _temp1238 +  5u},{ _temp1239,
_temp1239, _temp1239 +  8u},{ _temp1240, _temp1240, _temp1240 +  4u},{ _temp1241,
_temp1241, _temp1241 +  6u},{ _temp1242, _temp1242, _temp1242 +  4u},{ _temp1243,
_temp1243, _temp1243 +  6u},{ _temp1244, _temp1244, _temp1244 +  4u},{ _temp1245,
_temp1245, _temp1245 +  9u},{ _temp1246, _temp1246, _temp1246 +  9u},{ _temp1247,
_temp1247, _temp1247 +  6u},{ _temp1248, _temp1248, _temp1248 +  10u},{
_temp1249, _temp1249, _temp1249 +  7u},{ _temp1250, _temp1250, _temp1250 +  8u},{
_temp1251, _temp1251, _temp1251 +  5u},{ _temp1252, _temp1252, _temp1252 +  8u},{
_temp1253, _temp1253, _temp1253 +  4u},{ _temp1254, _temp1254, _temp1254 +  7u},{
_temp1255, _temp1255, _temp1255 +  7u},{ _temp1256, _temp1256, _temp1256 +  9u},{
_temp1257, _temp1257, _temp1257 +  7u},{ _temp1258, _temp1258, _temp1258 +  5u},{
_temp1259, _temp1259, _temp1259 +  8u},{ _temp1260, _temp1260, _temp1260 +  8u},{
_temp1261, _temp1261, _temp1261 +  4u},{ _temp1262, _temp1262, _temp1262 +  7u},{
_temp1263, _temp1263, _temp1263 +  7u},{ _temp1264, _temp1264, _temp1264 +  7u},{
_temp1265, _temp1265, _temp1265 +  8u},{ _temp1266, _temp1266, _temp1266 +  9u},{
_temp1267, _temp1267, _temp1267 +  6u},{ _temp1268, _temp1268, _temp1268 +  6u},{
_temp1269, _temp1269, _temp1269 +  6u},{ _temp1270, _temp1270, _temp1270 +  6u},{
_temp1271, _temp1271, _temp1271 +  7u},{ _temp1272, _temp1272, _temp1272 +  6u},{
_temp1273, _temp1273, _temp1273 +  11u},{ _temp1274, _temp1274, _temp1274 +  11u},{
_temp1275, _temp1275, _temp1275 +  11u},{ _temp1276, _temp1276, _temp1276 +  11u},{
_temp1277, _temp1277, _temp1277 +  11u},{ _temp1278, _temp1278, _temp1278 +  12u},{
_temp1279, _temp1279, _temp1279 +  13u},{ _temp1280, _temp1280, _temp1280 +  11u},{
_temp1281, _temp1281, _temp1281 +  11u},{ _temp1282, _temp1282, _temp1282 +  10u},{
_temp1283, _temp1283, _temp1283 +  9u},{ _temp1284, _temp1284, _temp1284 +  11u},{
_temp1285, _temp1285, _temp1285 +  12u},{ _temp1286, _temp1286, _temp1286 +  11u},{
_temp1287, _temp1287, _temp1287 +  17u},{ _temp1288, _temp1288, _temp1288 +  7u},{
_temp1289, _temp1289, _temp1289 +  19u},{ _temp1290, _temp1290, _temp1290 +  18u},{
_temp1291, _temp1291, _temp1291 +  9u},{ _temp1292, _temp1292, _temp1292 +  16u},{
_temp1293, _temp1293, _temp1293 +  18u},{ _temp1294, _temp1294, _temp1294 +  10u},{
_temp1295, _temp1295, _temp1295 +  4u},{ _temp1296, _temp1296, _temp1296 +  4u},{
_temp1297, _temp1297, _temp1297 +  4u},{ _temp1298, _temp1298, _temp1298 +  4u},{
_temp1299, _temp1299, _temp1299 +  4u},{ _temp1300, _temp1300, _temp1300 +  4u},{
_temp1301, _temp1301, _temp1301 +  4u},{ _temp1302, _temp1302, _temp1302 +  4u},{
_temp1303, _temp1303, _temp1303 +  4u},{ _temp1304, _temp1304, _temp1304 +  4u},{
_temp1305, _temp1305, _temp1305 +  4u},{ _temp1306, _temp1306, _temp1306 +  4u},{
_temp1307, _temp1307, _temp1307 +  4u},{ _temp1308, _temp1308, _temp1308 +  4u},{
_temp1309, _temp1309, _temp1309 +  4u},{ _temp1310, _temp1310, _temp1310 +  4u},{
_temp1311, _temp1311, _temp1311 +  4u},{ _temp1312, _temp1312, _temp1312 +  4u},{
_temp1313, _temp1313, _temp1313 +  4u},{ _temp1314, _temp1314, _temp1314 +  4u},{
_temp1315, _temp1315, _temp1315 +  4u},{ _temp1316, _temp1316, _temp1316 +  4u},{
_temp1317, _temp1317, _temp1317 +  4u},{ _temp1318, _temp1318, _temp1318 +  4u},{
_temp1319, _temp1319, _temp1319 +  4u},{ _temp1320, _temp1320, _temp1320 +  4u},{
_temp1321, _temp1321, _temp1321 +  4u},{ _temp1322, _temp1322, _temp1322 +  5u},{
_temp1323, _temp1323, _temp1323 +  17u},{ _temp1324, _temp1324, _temp1324 +  21u},{
_temp1325, _temp1325, _temp1325 +  20u},{ _temp1326, _temp1326, _temp1326 +  21u},{
_temp1327, _temp1327, _temp1327 +  13u},{ _temp1328, _temp1328, _temp1328 +  15u},{
_temp1329, _temp1329, _temp1329 +  17u},{ _temp1330, _temp1330, _temp1330 +  19u},{
_temp1331, _temp1331, _temp1331 +  12u},{ _temp1332, _temp1332, _temp1332 +  17u},{
_temp1333, _temp1333, _temp1333 +  23u},{ _temp1334, _temp1334, _temp1334 +  24u},{
_temp1335, _temp1335, _temp1335 +  15u},{ _temp1336, _temp1336, _temp1336 +  11u},{
_temp1337, _temp1337, _temp1337 +  15u},{ _temp1338, _temp1338, _temp1338 +  10u},{
_temp1339, _temp1339, _temp1339 +  15u},{ _temp1340, _temp1340, _temp1340 +  5u},{
_temp1341, _temp1341, _temp1341 +  15u},{ _temp1342, _temp1342, _temp1342 +  15u},{
_temp1343, _temp1343, _temp1343 +  11u},{ _temp1344, _temp1344, _temp1344 +  22u},{
_temp1345, _temp1345, _temp1345 +  26u},{ _temp1346, _temp1346, _temp1346 +  16u},{
_temp1347, _temp1347, _temp1347 +  16u},{ _temp1348, _temp1348, _temp1348 +  24u},{
_temp1349, _temp1349, _temp1349 +  25u},{ _temp1350, _temp1350, _temp1350 +  21u},{
_temp1351, _temp1351, _temp1351 +  22u},{ _temp1352, _temp1352, _temp1352 +  16u},{
_temp1353, _temp1353, _temp1353 +  19u},{ _temp1354, _temp1354, _temp1354 +  25u},{
_temp1355, _temp1355, _temp1355 +  23u},{ _temp1356, _temp1356, _temp1356 +  24u},{
_temp1357, _temp1357, _temp1357 +  18u},{ _temp1358, _temp1358, _temp1358 +  17u},{
_temp1359, _temp1359, _temp1359 +  18u},{ _temp1360, _temp1360, _temp1360 +  17u},{
_temp1361, _temp1361, _temp1361 +  18u},{ _temp1362, _temp1362, _temp1362 +  12u},{
_temp1363, _temp1363, _temp1363 +  11u},{ _temp1364, _temp1364, _temp1364 +  18u},{
_temp1365, _temp1365, _temp1365 +  8u},{ _temp1366, _temp1366, _temp1366 +  13u},{
_temp1367, _temp1367, _temp1367 +  8u},{ _temp1368, _temp1368, _temp1368 +  4u},{
_temp1369, _temp1369, _temp1369 +  20u},{ _temp1370, _temp1370, _temp1370 +  20u},{
_temp1371, _temp1371, _temp1371 +  9u},{ _temp1372, _temp1372, _temp1372 +  16u},{
_temp1373, _temp1373, _temp1373 +  19u},{ _temp1374, _temp1374, _temp1374 +  10u},{
_temp1375, _temp1375, _temp1375 +  16u},{ _temp1376, _temp1376, _temp1376 +  11u},{
_temp1377, _temp1377, _temp1377 +  14u},{ _temp1378, _temp1378, _temp1378 +  11u},{
_temp1379, _temp1379, _temp1379 +  15u},{ _temp1380, _temp1380, _temp1380 +  22u},{
_temp1381, _temp1381, _temp1381 +  16u},{ _temp1382, _temp1382, _temp1382 +  17u},{
_temp1383, _temp1383, _temp1383 +  12u},{ _temp1384, _temp1384, _temp1384 +  18u},{
_temp1385, _temp1385, _temp1385 +  17u},{ _temp1386, _temp1386, _temp1386 +  12u},{
_temp1387, _temp1387, _temp1387 +  16u},{ _temp1388, _temp1388, _temp1388 +  11u},{
_temp1389, _temp1389, _temp1389 +  10u},{ _temp1390, _temp1390, _temp1390 +  14u},{
_temp1391, _temp1391, _temp1391 +  15u},{ _temp1392, _temp1392, _temp1392 +  20u},{
_temp1393, _temp1393, _temp1393 +  27u},{ _temp1394, _temp1394, _temp1394 +  10u},{
_temp1395, _temp1395, _temp1395 +  18u},{ _temp1396, _temp1396, _temp1396 +  21u},{
_temp1397, _temp1397, _temp1397 +  19u},{ _temp1398, _temp1398, _temp1398 +  16u},{
_temp1399, _temp1399, _temp1399 +  20u},{ _temp1400, _temp1400, _temp1400 +  15u},{
_temp1401, _temp1401, _temp1401 +  16u},{ _temp1402, _temp1402, _temp1402 +  20u},{
_temp1403, _temp1403, _temp1403 +  15u},{ _temp1404, _temp1404, _temp1404 +  8u},{
_temp1405, _temp1405, _temp1405 +  19u},{ _temp1406, _temp1406, _temp1406 +  20u},{
_temp1407, _temp1407, _temp1407 +  14u},{ _temp1408, _temp1408, _temp1408 +  19u},{
_temp1409, _temp1409, _temp1409 +  20u},{ _temp1410, _temp1410, _temp1410 +  11u},{
_temp1411, _temp1411, _temp1411 +  22u},{ _temp1412, _temp1412, _temp1412 +  20u},{
_temp1413, _temp1413, _temp1413 +  23u},{ _temp1414, _temp1414, _temp1414 +  20u},{
_temp1415, _temp1415, _temp1415 +  22u},{ _temp1416, _temp1416, _temp1416 +  23u},{
_temp1417, _temp1417, _temp1417 +  24u},{ _temp1418, _temp1418, _temp1418 +  24u},{
_temp1419, _temp1419, _temp1419 +  15u},{ _temp1420, _temp1420, _temp1420 +  20u},{
_temp1421, _temp1421, _temp1421 +  22u},{ _temp1422, _temp1422, _temp1422 +  17u},{
_temp1423, _temp1423, _temp1423 +  20u},{ _temp1424, _temp1424, _temp1424 +  26u},{
_temp1425, _temp1425, _temp1425 +  16u},{ _temp1426, _temp1426, _temp1426 +  17u},{
_temp1427, _temp1427, _temp1427 +  15u},{ _temp1428, _temp1428, _temp1428 +  19u},{
_temp1429, _temp1429, _temp1429 +  19u},{ _temp1430, _temp1430, _temp1430 +  25u},{
_temp1431, _temp1431, _temp1431 +  26u},{ _temp1432, _temp1432, _temp1432 +  9u},{
_temp1433, _temp1433, _temp1433 +  20u}}; static short Cyc_yyr1[ 406u]={ 0, 121,
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
short*)({ unsigned int _temp3262= 10000u; short* _temp3263=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3262));{ unsigned int
_temp3264= _temp3262; unsigned int i; for( i= 0; i <  _temp3264; i ++){
_temp3263[ i]=( short) 0;}}; _temp3263;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3259= 10000u; void** _temp3260=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3259));{ unsigned int _temp3261= _temp3259;
unsigned int i; for( i= 0; i <  _temp3261; i ++){ _temp3260[ i]= Cyc_yylval;}};
_temp3260;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3256= 10000u; struct Cyc_Yyltype* _temp3257=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3256));{
unsigned int _temp3258= _temp3256; unsigned int i; for( i= 0; i <  _temp3258; i
++){ _temp3257[ i]= Cyc_yynewloc();}}; _temp3257;}); int yystacksize= 10000;
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
yyn){ case 1: _LL1434: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1435:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1437=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1437[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1438; _temp1438.tag= Cyc_DeclList_tok; _temp1438.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1438;});
_temp1437;}); break; case 3: _LL1436: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1440=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1440[ 0]=({ struct Cyc_DeclList_tok_struct _temp1441; _temp1441.tag= Cyc_DeclList_tok;
_temp1441.f1=({ struct Cyc_List_List* _temp1442=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1442->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1443=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1443->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1444=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1444[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1445; _temp1445.tag= Cyc_Absyn_Using_d;
_temp1445.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1445.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1445;}); _temp1444;}));
_temp1443->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1443;});
_temp1442->tl= 0; _temp1442;}); _temp1441;}); _temp1440;}); Cyc_Lex_leave_using();
break; case 4: _LL1439: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1447=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1447[ 0]=({ struct Cyc_DeclList_tok_struct _temp1448; _temp1448.tag= Cyc_DeclList_tok;
_temp1448.f1=({ struct Cyc_List_List* _temp1449=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1449->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1450=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1450->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1451=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1451[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1452; _temp1452.tag= Cyc_Absyn_Using_d;
_temp1452.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp1452.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1452;});
_temp1451;})); _temp1450->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1450;}); _temp1449->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1449;}); _temp1448;});
_temp1447;}); break; case 5: _LL1446: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
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
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1460;});
_temp1459.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1459;}); _temp1458;})); _temp1457->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1457;}); _temp1456->tl= 0; _temp1456;}); _temp1455;}); _temp1454;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1453: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1462=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1462[ 0]=({ struct Cyc_DeclList_tok_struct _temp1463; _temp1463.tag= Cyc_DeclList_tok;
_temp1463.f1=({ struct Cyc_List_List* _temp1464=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1464->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1465=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1465->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1466=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1466[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1467; _temp1467.tag=
Cyc_Absyn_Namespace_d; _temp1467.f1=({ struct _tagged_arr* _temp1468=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1468[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1468;});
_temp1467.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1467;}); _temp1466;})); _temp1465->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1465;}); _temp1464->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1464;}); _temp1463;});
_temp1462;}); break; case 7: _LL1461: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1470=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1470[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1471; _temp1471.tag= Cyc_DeclList_tok; _temp1471.f1=({ struct Cyc_List_List*
_temp1472=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1472->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1473=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1473->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1474=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1474[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1475; _temp1475.tag= Cyc_Absyn_ExternC_d;
_temp1475.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1475;}); _temp1474;})); _temp1473->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1473;}); _temp1472->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1472;}); _temp1471;});
_temp1470;}); break; case 8: _LL1469: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1477=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1477[ 0]=({ struct Cyc_DeclList_tok_struct _temp1478; _temp1478.tag= Cyc_DeclList_tok;
_temp1478.f1= 0; _temp1478;}); _temp1477;}); break; case 9: _LL1476: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1480=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1480[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1481; _temp1481.tag= Cyc_DeclList_tok; _temp1481.f1=({ struct Cyc_List_List*
_temp1482=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1482->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1483=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1483[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1484; _temp1484.tag= Cyc_Absyn_Fn_d;
_temp1484.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1484;}); _temp1483;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1482->tl=
0; _temp1482;}); _temp1481;}); _temp1480;}); break; case 10: _LL1479: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1485: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1487=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1487[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1488; _temp1488.tag= Cyc_FnDecl_tok; _temp1488.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1488;}); _temp1487;}); break; case 12: _LL1486: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1490=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1490[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1491; _temp1491.tag= Cyc_FnDecl_tok;
_temp1491.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1492=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1492->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1492;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1491;}); _temp1490;}); break; case 13: _LL1489: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1494=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1494[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1495; _temp1495.tag= Cyc_FnDecl_tok;
_temp1495.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1495;}); _temp1494;}); break; case 14: _LL1493: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
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
_temp1498;}); _temp1497;}); break; case 15: _LL1496: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1501=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1501[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1502; _temp1502.tag= Cyc_FnDecl_tok;
_temp1502.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1503=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1503->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1503;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1502;}); _temp1501;}); break; case 16: _LL1500: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1505=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1505[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1506; _temp1506.tag= Cyc_FnDecl_tok;
_temp1506.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1507=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1507->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1507;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1506;}); _temp1505;}); break; case 17: _LL1504: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1508:
Cyc_Lex_leave_using(); break; case 19: _LL1509: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1511=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1511[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1511;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1510: Cyc_Lex_leave_namespace(); break; case 21: _LL1512: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1514=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1514[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1515; _temp1515.tag= Cyc_DeclList_tok; _temp1515.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1515;}); _temp1514;}); break; case 22: _LL1513: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1517=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1517[ 0]=({ struct Cyc_DeclList_tok_struct _temp1518; _temp1518.tag= Cyc_DeclList_tok;
_temp1518.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1518;}); _temp1517;}); break; case 23: _LL1516: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1520=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1520[ 0]=({ struct Cyc_DeclList_tok_struct _temp1521; _temp1521.tag= Cyc_DeclList_tok;
_temp1521.f1=({ struct Cyc_List_List* _temp1522=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1522->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1522->tl=
0; _temp1522;}); _temp1521;}); _temp1520;}); break; case 24: _LL1519: { struct
Cyc_List_List* _temp1524= 0;{ struct Cyc_List_List* _temp1525= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0;
_temp1525 !=  0; _temp1525= _temp1525->tl){ struct _tagged_arr* _temp1526=(
struct _tagged_arr*) _temp1525->hd; struct _tuple1* _temp1527=({ struct _tuple1*
_temp1530=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1530->f1=
Cyc_Absyn_rel_ns_null; _temp1530->f2= _temp1526; _temp1530;}); struct Cyc_Absyn_Vardecl*
_temp1528= Cyc_Absyn_new_vardecl( _temp1527, Cyc_Absyn_wildtyp( 0), 0);
_temp1524=({ struct Cyc_List_List* _temp1529=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1529->hd=( void*) _temp1528; _temp1529->tl=
_temp1524; _temp1529;});}} _temp1524=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1524); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1531=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1531[ 0]=({ struct Cyc_DeclList_tok_struct _temp1532; _temp1532.tag= Cyc_DeclList_tok;
_temp1532.f1=({ struct Cyc_List_List* _temp1533=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1533->hd=( void*) Cyc_Absyn_letv_decl(
_temp1524, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1533->tl= 0; _temp1533;}); _temp1532;});
_temp1531;}); break;} case 25: _LL1523: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1534:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1536=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1536[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1537; _temp1537.tag= Cyc_DeclList_tok; _temp1537.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1537;});
_temp1536;}); break; case 27: _LL1535: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1539=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1539[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1540; _temp1540.tag= Cyc_DeclSpec_tok;
_temp1540.f1=({ struct Cyc_Parse_Declaration_spec* _temp1541=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1541->sc=({ struct
Cyc_Core_Opt* _temp1542=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1542->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1542;});
_temp1541->tq= Cyc_Absyn_empty_tqual(); _temp1541->type_specs= 0; _temp1541->is_inline=
0; _temp1541->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1541;}); _temp1540;});
_temp1539;}); break; case 28: _LL1538: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc !=  0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1544=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1544[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1545; _temp1545.tag= Cyc_DeclSpec_tok; _temp1545.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1546=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1546->sc=({ struct Cyc_Core_Opt* _temp1547=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1547->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1547;});
_temp1546->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1546->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1546->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1546->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1546;}); _temp1545;}); _temp1544;}); break; case 29: _LL1543: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1549=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1549[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1550; _temp1550.tag= Cyc_DeclSpec_tok; _temp1550.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1551=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1551->sc= 0; _temp1551->tq= Cyc_Absyn_empty_tqual(); _temp1551->type_specs=({
struct Cyc_List_List* _temp1552=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1552->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1552->tl= 0;
_temp1552;}); _temp1551->is_inline= 0; _temp1551->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1551;});
_temp1550;}); _temp1549;}); break; case 30: _LL1548: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1554=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1554[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1555; _temp1555.tag= Cyc_DeclSpec_tok;
_temp1555.f1=({ struct Cyc_Parse_Declaration_spec* _temp1556=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1556->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1556->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1556->type_specs=({ struct Cyc_List_List* _temp1557=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1557->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1557->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1557;});
_temp1556->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1556->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1556;}); _temp1555;}); _temp1554;}); break; case 31: _LL1553: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1559=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1559[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1560; _temp1560.tag= Cyc_DeclSpec_tok; _temp1560.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1561=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1561->sc= 0; _temp1561->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1561->type_specs=
0; _temp1561->is_inline= 0; _temp1561->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1561;});
_temp1560;}); _temp1559;}); break; case 32: _LL1558: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1563=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1563[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1564; _temp1564.tag= Cyc_DeclSpec_tok;
_temp1564.f1=({ struct Cyc_Parse_Declaration_spec* _temp1565=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1565->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1565->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1565->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1565->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1565->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1565;}); _temp1564;}); _temp1563;}); break; case 33: _LL1562: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1567=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1567[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1568; _temp1568.tag= Cyc_DeclSpec_tok; _temp1568.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1569=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1569->sc= 0; _temp1569->tq= Cyc_Absyn_empty_tqual(); _temp1569->type_specs=
0; _temp1569->is_inline= 1; _temp1569->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1569;});
_temp1568;}); _temp1567;}); break; case 34: _LL1566: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1571=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1571[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1572; _temp1572.tag= Cyc_DeclSpec_tok;
_temp1572.f1=({ struct Cyc_Parse_Declaration_spec* _temp1573=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1573->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1573->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1573->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1573->is_inline=
1; _temp1573->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1573;}); _temp1572;}); _temp1571;}); break; case 35: _LL1570: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1575=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1575[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1576; _temp1576.tag= Cyc_StorageClass_tok;
_temp1576.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1576;}); _temp1575;});
break; case 36: _LL1574: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1578=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1578[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1579; _temp1579.tag=
Cyc_StorageClass_tok; _temp1579.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1579;}); _temp1578;}); break; case 37: _LL1577: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1581=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1581[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1582; _temp1582.tag=
Cyc_StorageClass_tok; _temp1582.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1582;}); _temp1581;}); break; case 38: _LL1580: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1584=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1584[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1585; _temp1585.tag=
Cyc_StorageClass_tok; _temp1585.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1585;}); _temp1584;}); break; case 39: _LL1583: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1587=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1587[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1588; _temp1588.tag= Cyc_StorageClass_tok;
_temp1588.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1588;}); _temp1587;});
break; case 40: _LL1586: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1590=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1590[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1591; _temp1591.tag=
Cyc_StorageClass_tok; _temp1591.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1591;}); _temp1590;}); break; case 41: _LL1589: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1593=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1593[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1594; _temp1594.tag=
Cyc_StorageClass_tok; _temp1594.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1594;}); _temp1593;}); break; case 42: _LL1592: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1596=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1596[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1597; _temp1597.tag=
Cyc_AttributeList_tok; _temp1597.f1= 0; _temp1597;}); _temp1596;}); break; case
43: _LL1595: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1598: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1600=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1600[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1601; _temp1601.tag=
Cyc_AttributeList_tok; _temp1601.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1601;});
_temp1600;}); break; case 45: _LL1599: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1603=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1603[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1604; _temp1604.tag=
Cyc_AttributeList_tok; _temp1604.f1=({ struct Cyc_List_List* _temp1605=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1605->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1605->tl= 0; _temp1605;}); _temp1604;}); _temp1603;});
break; case 46: _LL1602: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1607=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1607[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1608; _temp1608.tag=
Cyc_AttributeList_tok; _temp1608.f1=({ struct Cyc_List_List* _temp1609=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1609->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1609->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1609;}); _temp1608;});
_temp1607;}); break; case 47: _LL1606: { struct _tagged_arr _temp1611= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1611, sizeof( unsigned char)) >  4?*(( const unsigned char*)
_check_unknown_subscript( _temp1611, sizeof( unsigned char), 0)) == '_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1611, sizeof( unsigned char),
1)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1611,
sizeof( unsigned char),( int)( _get_arr_size( _temp1611, sizeof( unsigned char))
-  2))) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1611,
sizeof( unsigned char),( int)( _get_arr_size( _temp1611, sizeof( unsigned char))
-  3))) == '_': 0){ _temp1611=( struct _tagged_arr) Cyc_Std_substring( _temp1611,
2, _get_arr_size( _temp1611, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0;
i <  16u; ++ i){ if( Cyc_Std_strcmp( _temp1611,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1612=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1612[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1613; _temp1613.tag= Cyc_Attribute_tok; _temp1613.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1613;}); _temp1612;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1614=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1614[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1615; _temp1615.tag= Cyc_Attribute_tok; _temp1615.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1615;}); _temp1614;});} break;}} case
48: _LL1610: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1617=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1617[ 0]=({ struct Cyc_Attribute_tok_struct _temp1618; _temp1618.tag= Cyc_Attribute_tok;
_temp1618.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1618;}); _temp1617;});
break; case 49: _LL1616: { struct _tagged_arr _temp1620= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1623; int _temp1624; struct _tuple15* _temp1621= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1623=*
_temp1621; _LL1625: _temp1624= _temp1623.f2; goto _LL1622; _LL1622: { void* a;
if( Cyc_Std_zstrcmp( _temp1620, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1620, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1624 <  0? 1: _temp1624 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 0 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1626=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1626[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1627; _temp1627.tag= Cyc_Absyn_Regparm_att; _temp1627.f1= _temp1624;
_temp1627;}); _temp1626;});} else{ if( Cyc_Std_zstrcmp( _temp1620, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1620, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1624 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1624;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1628=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1628[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1629; _temp1629.tag= Cyc_Absyn_Aligned_att; _temp1629.f1= _temp1624;
_temp1629;}); _temp1628;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1630=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1630[ 0]=({ struct Cyc_Attribute_tok_struct _temp1631; _temp1631.tag= Cyc_Attribute_tok;
_temp1631.f1=( void*) a; _temp1631;}); _temp1630;}); break;}} case 50: _LL1619: {
struct _tagged_arr _temp1633= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1634= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1633, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1633, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1635=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1635[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1636; _temp1636.tag=
Cyc_Absyn_Section_att; _temp1636.f1= _temp1634; _temp1636;}); _temp1635;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1637=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1637[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1638; _temp1638.tag= Cyc_Attribute_tok; _temp1638.f1=(
void*) a; _temp1638;}); _temp1637;}); break;} case 51: _LL1632: { struct
_tagged_arr _temp1640= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1641= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1644; struct _tuple15 _temp1642=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1645:
_temp1644= _temp1642.f2; goto _LL1643; _LL1643: { int _temp1648; struct _tuple15
_temp1646=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1649: _temp1648= _temp1646.f2; goto _LL1647; _LL1647: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1640, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1640, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1641, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1650=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1650[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1651; _temp1651.tag=
Cyc_Absyn_Format_att; _temp1651.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1651.f2= _temp1644; _temp1651.f3= _temp1648; _temp1651;}); _temp1650;});}
else{ if( Cyc_Std_zstrcmp( _temp1641, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1652=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1652[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1653; _temp1653.tag=
Cyc_Absyn_Format_att; _temp1653.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1653.f2= _temp1644; _temp1653.f3= _temp1648; _temp1653;}); _temp1652;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1654=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1654[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1655; _temp1655.tag= Cyc_Attribute_tok; _temp1655.f1=(
void*) a; _temp1655;}); _temp1654;}); break;}}} case 52: _LL1639: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1657=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1657[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1658; _temp1658.tag= Cyc_TypeSpecifier_tok;
_temp1658.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1658;});
_temp1657;}); break; case 53: _LL1656: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1660=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1660[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1661; _temp1661.tag=
Cyc_TypeSpecifier_tok; _temp1661.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1661;}); _temp1660;}); break; case 54: _LL1659:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1663=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1663[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1664; _temp1664.tag= Cyc_TypeSpecifier_tok;
_temp1664.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1664;});
_temp1663;}); break; case 55: _LL1662: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1666=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1666[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1667; _temp1667.tag=
Cyc_TypeSpecifier_tok; _temp1667.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1668=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1668[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1669; _temp1669.tag=
Cyc_Parse_Short_spec; _temp1669.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1669;});
_temp1668;})); _temp1667;}); _temp1666;}); break; case 56: _LL1665: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1671=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1671[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1672; _temp1672.tag= Cyc_TypeSpecifier_tok;
_temp1672.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1672;});
_temp1671;}); break; case 57: _LL1670: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1674=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1674[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1675; _temp1675.tag=
Cyc_TypeSpecifier_tok; _temp1675.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1676=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1676[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1677; _temp1677.tag= Cyc_Parse_Long_spec;
_temp1677.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1677;}); _temp1676;})); _temp1675;}); _temp1674;});
break; case 58: _LL1673: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1679=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1679[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1680; _temp1680.tag=
Cyc_TypeSpecifier_tok; _temp1680.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1680;}); _temp1679;}); break; case 59: _LL1678:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1682=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1682[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1683; _temp1683.tag= Cyc_TypeSpecifier_tok;
_temp1683.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t( 0), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1683;});
_temp1682;}); break; case 60: _LL1681: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1685=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1685[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1686; _temp1686.tag=
Cyc_TypeSpecifier_tok; _temp1686.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1687=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1687[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1688; _temp1688.tag=
Cyc_Parse_Signed_spec; _temp1688.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1688;});
_temp1687;})); _temp1686;}); _temp1685;}); break; case 61: _LL1684: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1690=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1690[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1691; _temp1691.tag= Cyc_TypeSpecifier_tok;
_temp1691.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1692=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1692[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1693; _temp1693.tag= Cyc_Parse_Unsigned_spec; _temp1693.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1693;});
_temp1692;})); _temp1691;}); _temp1690;}); break; case 62: _LL1689: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1694:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1695: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1696: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1698=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1698[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1699; _temp1699.tag=
Cyc_TypeSpecifier_tok; _temp1699.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1699;});
_temp1698;}); break; case 66: _LL1697: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1701=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1701[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1702; _temp1702.tag=
Cyc_TypeSpecifier_tok; _temp1702.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1703=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1703[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1704; _temp1704.tag= Cyc_Absyn_TypedefType;
_temp1704.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1704.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1704.f3= 0;
_temp1704;}); _temp1703;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1702;});
_temp1701;}); break; case 67: _LL1700: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1706=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1706[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1707; _temp1707.tag=
Cyc_TypeSpecifier_tok; _temp1707.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1708=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1708[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1709; _temp1709.tag= Cyc_Absyn_TupleType;
_temp1709.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1709;});
_temp1708;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1707;}); _temp1706;}); break; case 68:
_LL1705: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1711=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1711[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1712; _temp1712.tag=
Cyc_TypeSpecifier_tok; _temp1712.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1713=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1713[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1714; _temp1714.tag= Cyc_Absyn_RgnHandleType;
_temp1714.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1714;}); _temp1713;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1712;}); _temp1711;}); break; case 69: _LL1710: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1716=( struct Cyc_Kind_tok_struct*) _cycalloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1716[ 0]=({ struct Cyc_Kind_tok_struct _temp1717; _temp1717.tag= Cyc_Kind_tok;
_temp1717.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1717;});
_temp1716;}); break; case 70: _LL1715: { struct _tagged_arr* _temp1721; void*
_temp1723; struct _tuple1 _temp1719=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1724: _temp1723=
_temp1719.f1; goto _LL1722; _LL1722: _temp1721= _temp1719.f2; goto _LL1720;
_LL1720: if( _temp1723 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1725=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1725[ 0]=({ struct Cyc_Kind_tok_struct
_temp1726; _temp1726.tag= Cyc_Kind_tok; _temp1726.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1721, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1726;}); _temp1725;}); break;} case 71:
_LL1718: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1728=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1728[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1729; _temp1729.tag= Cyc_TypeQual_tok;
_temp1729.f1=({ struct Cyc_Absyn_Tqual _temp1730; _temp1730.q_const= 1;
_temp1730.q_volatile= 0; _temp1730.q_restrict= 0; _temp1730;}); _temp1729;});
_temp1728;}); break; case 72: _LL1727: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1732=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1732[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1733; _temp1733.tag= Cyc_TypeQual_tok; _temp1733.f1=({ struct Cyc_Absyn_Tqual
_temp1734; _temp1734.q_const= 0; _temp1734.q_volatile= 1; _temp1734.q_restrict=
0; _temp1734;}); _temp1733;}); _temp1732;}); break; case 73: _LL1731: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1736=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1736[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1737; _temp1737.tag= Cyc_TypeQual_tok;
_temp1737.f1=({ struct Cyc_Absyn_Tqual _temp1738; _temp1738.q_const= 0;
_temp1738.q_volatile= 0; _temp1738.q_restrict= 1; _temp1738;}); _temp1737;});
_temp1736;}); break; case 74: _LL1735: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1740=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1740[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1741; _temp1741.tag=
Cyc_TypeSpecifier_tok; _temp1741.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1742=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1742[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1743; _temp1743.tag= Cyc_Parse_Decl_spec;
_temp1743.f1=({ struct Cyc_Absyn_Decl* _temp1744=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1744->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1745=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1745[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1746; _temp1746.tag= Cyc_Absyn_Enum_d; _temp1746.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1747=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1747->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1747->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1747->fields=({
struct Cyc_Core_Opt* _temp1748=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1748->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1748;});
_temp1747;}); _temp1746;}); _temp1745;})); _temp1744->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1744;}); _temp1743;}); _temp1742;})); _temp1741;}); _temp1740;}); break;
case 75: _LL1739: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1750=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1750[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1751; _temp1751.tag=
Cyc_TypeSpecifier_tok; _temp1751.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1752=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1752[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1753; _temp1753.tag= Cyc_Absyn_EnumType;
_temp1753.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1753.f2= 0; _temp1753;}); _temp1752;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1751;}); _temp1750;}); break; case 76: _LL1749: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1755=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1755[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1756; _temp1756.tag=
Cyc_TypeSpecifier_tok; _temp1756.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1757=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1757[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1758; _temp1758.tag= Cyc_Parse_Type_spec;
_temp1758.f1=( void*)(( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp1759=(
struct Cyc_Absyn_AnonEnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct));
_temp1759[ 0]=({ struct Cyc_Absyn_AnonEnumType_struct _temp1760; _temp1760.tag=
Cyc_Absyn_AnonEnumType; _temp1760.f1= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1760;});
_temp1759;})); _temp1758.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1758;});
_temp1757;})); _temp1756;}); _temp1755;}); break; case 77: _LL1754: yyval=( void*)({
struct Cyc_Enumfield_tok_struct* _temp1762=( struct Cyc_Enumfield_tok_struct*)
_cycalloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp1762[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp1763; _temp1763.tag= Cyc_Enumfield_tok; _temp1763.f1=({
struct Cyc_Absyn_Enumfield* _temp1764=( struct Cyc_Absyn_Enumfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumfield)); _temp1764->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1764->tag= 0;
_temp1764->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1764;});
_temp1763;}); _temp1762;}); break; case 78: _LL1761: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1766=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1766[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1767; _temp1767.tag= Cyc_Enumfield_tok;
_temp1767.f1=({ struct Cyc_Absyn_Enumfield* _temp1768=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1768->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1768->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1768->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1768;});
_temp1767;}); _temp1766;}); break; case 79: _LL1765: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1770=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1770[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1771; _temp1771.tag=
Cyc_EnumfieldList_tok; _temp1771.f1=({ struct Cyc_List_List* _temp1772=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1772->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1772->tl= 0; _temp1772;}); _temp1771;}); _temp1770;});
break; case 80: _LL1769: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1774=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1774[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1775; _temp1775.tag=
Cyc_EnumfieldList_tok; _temp1775.f1=({ struct Cyc_List_List* _temp1776=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1776->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1776->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1776;}); _temp1775;});
_temp1774;}); break; case 81: _LL1773: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1778: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1780=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1780[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1781; _temp1781.tag=
Cyc_Absyn_AnonStructType; _temp1781.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1781;});
_temp1780;}); break; case Cyc_Parse_Union_su: _LL1779: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1783=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1783[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1784; _temp1784.tag= Cyc_Absyn_AnonUnionType; _temp1784.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1784;});
_temp1783;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1785=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1785[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1786; _temp1786.tag=
Cyc_TypeSpecifier_tok; _temp1786.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1787=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1787[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1788; _temp1788.tag= Cyc_Parse_Type_spec;
_temp1788.f1=( void*) t; _temp1788.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1788;});
_temp1787;})); _temp1786;}); _temp1785;}); break;} case 82: _LL1777: { struct
Cyc_List_List* _temp1790=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1791: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1793=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1793->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1793;}), _temp1790,({ struct Cyc_Core_Opt*
_temp1794=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1794->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1794;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1792: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1796=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1796->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1796;}),
_temp1790,({ struct Cyc_Core_Opt* _temp1797=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1797->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1797;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1798=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1798[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1799; _temp1799.tag=
Cyc_TypeSpecifier_tok; _temp1799.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1800=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1800[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1801; _temp1801.tag= Cyc_Parse_Decl_spec;
_temp1801.f1= d; _temp1801;}); _temp1800;})); _temp1799;}); _temp1798;}); break;}
case 83: _LL1789: { struct Cyc_List_List* _temp1803=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1804: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1806=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1806->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1806;}), _temp1803,({ struct Cyc_Core_Opt*
_temp1807=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1807->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1807;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1805: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1809=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1809->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1809;}),
_temp1803,({ struct Cyc_Core_Opt* _temp1810=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1810->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1810;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1811=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1811[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1812; _temp1812.tag=
Cyc_TypeSpecifier_tok; _temp1812.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1813=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1813[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1814; _temp1814.tag= Cyc_Parse_Decl_spec;
_temp1814.f1= d; _temp1814;}); _temp1813;})); _temp1812;}); _temp1811;}); break;}
case 84: _LL1802: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1816: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1818=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1818[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1819; _temp1819.tag=
Cyc_TypeSpecifier_tok; _temp1819.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1820=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1820[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1821; _temp1821.tag= Cyc_Absyn_StructType;
_temp1821.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1821.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1821.f3= 0;
_temp1821;}); _temp1820;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1819;});
_temp1818;}); break; case Cyc_Parse_Union_su: _LL1817: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1823=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1823[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1824; _temp1824.tag= Cyc_TypeSpecifier_tok;
_temp1824.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1825=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1825[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1826; _temp1826.tag= Cyc_Absyn_UnionType;
_temp1826.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1826.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1826.f3= 0;
_temp1826;}); _temp1825;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1824;});
_temp1823;}); break;} break; case 85: _LL1815: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1828: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1830=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1830[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1831; _temp1831.tag=
Cyc_TypeSpecifier_tok; _temp1831.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1832=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1832[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1833; _temp1833.tag= Cyc_Absyn_StructType;
_temp1833.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1833.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1833.f3= 0;
_temp1833;}); _temp1832;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1831;});
_temp1830;}); break; case Cyc_Parse_Union_su: _LL1829: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1835=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1835[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1836; _temp1836.tag= Cyc_TypeSpecifier_tok;
_temp1836.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1837=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1837[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1838; _temp1838.tag= Cyc_Absyn_UnionType;
_temp1838.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1838.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1838.f3= 0;
_temp1838;}); _temp1837;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1836;});
_temp1835;}); break;} break; case 86: _LL1827: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1840=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1840[ 0]=({ struct Cyc_TypeList_tok_struct _temp1841; _temp1841.tag= Cyc_TypeList_tok;
_temp1841.f1= 0; _temp1841;}); _temp1840;}); break; case 87: _LL1839: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1843=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1843[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1844; _temp1844.tag= Cyc_TypeList_tok; _temp1844.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1844;});
_temp1843;}); break; case 88: _LL1842:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1846=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1846[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1847; _temp1847.tag= Cyc_TypeList_tok; _temp1847.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1847;});
_temp1846;}); break; case 89: _LL1845: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1849=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1849[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1850; _temp1850.tag= Cyc_StructOrUnion_tok; _temp1850.f1= Cyc_Parse_Struct_su;
_temp1850;}); _temp1849;}); break; case 90: _LL1848: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1852=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1852[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1853; _temp1853.tag= Cyc_StructOrUnion_tok; _temp1853.f1= Cyc_Parse_Union_su;
_temp1853;}); _temp1852;}); break; case 91: _LL1851: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1855=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1855[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1856; _temp1856.tag= Cyc_StructFieldDeclList_tok; _temp1856.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1856;});
_temp1855;}); break; case 92: _LL1854: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1858=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1858[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1859; _temp1859.tag= Cyc_StructFieldDeclListList_tok; _temp1859.f1=({
struct Cyc_List_List* _temp1860=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1860->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1860->tl= 0;
_temp1860;}); _temp1859;}); _temp1858;}); break; case 93: _LL1857: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1862=( struct Cyc_StructFieldDeclListList_tok_struct*)
_cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1862[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1863; _temp1863.tag= Cyc_StructFieldDeclListList_tok;
_temp1863.f1=({ struct Cyc_List_List* _temp1864=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1864->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1864->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1864;}); _temp1863;}); _temp1862;}); break;
case 94: _LL1861: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1866=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1866[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1867; _temp1867.tag=
Cyc_InitDeclList_tok; _temp1867.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1867;});
_temp1866;}); break; case 95: _LL1865: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1869=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1869[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1870; _temp1870.tag=
Cyc_InitDeclList_tok; _temp1870.f1=({ struct Cyc_List_List* _temp1871=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1871->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1871->tl= 0; _temp1871;}); _temp1870;}); _temp1869;});
break; case 96: _LL1868: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1873=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1873[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1874; _temp1874.tag=
Cyc_InitDeclList_tok; _temp1874.f1=({ struct Cyc_List_List* _temp1875=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1875->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1875->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1875;});
_temp1874;}); _temp1873;}); break; case 97: _LL1872: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1877=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1877[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1878; _temp1878.tag= Cyc_InitDecl_tok;
_temp1878.f1=({ struct _tuple14* _temp1879=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1879->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1879->f2= 0;
_temp1879;}); _temp1878;}); _temp1877;}); break; case 98: _LL1876: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1881=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1881[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1882; _temp1882.tag= Cyc_InitDecl_tok; _temp1882.f1=({ struct _tuple14*
_temp1883=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1883->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1883->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1883;});
_temp1882;}); _temp1881;}); break; case 99: _LL1880: { struct _tuple17 _temp1887;
struct Cyc_List_List* _temp1888; struct Cyc_List_List* _temp1890; struct Cyc_Absyn_Tqual
_temp1892; struct _tuple17* _temp1885= Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1887=*
_temp1885; _LL1893: _temp1892= _temp1887.f1; goto _LL1891; _LL1891: _temp1890=
_temp1887.f2; goto _LL1889; _LL1889: _temp1888= _temp1887.f3; goto _LL1886;
_LL1886: { struct Cyc_List_List* _temp1896; struct Cyc_List_List* _temp1898;
struct _tuple0 _temp1894=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1899: _temp1898= _temp1894.f1; goto _LL1897; _LL1897:
_temp1896= _temp1894.f2; goto _LL1895; _LL1895: { void* _temp1900= Cyc_Parse_speclist2typ(
_temp1890, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1901=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1892, _temp1900, _temp1898, _temp1888), _temp1896);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1902=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1902[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1903; _temp1903.tag=
Cyc_StructFieldDeclList_tok; _temp1903.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1901); _temp1903;}); _temp1902;}); break;}}}
case 100: _LL1884: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1905=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1905[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1906; _temp1906.tag=
Cyc_QualSpecList_tok; _temp1906.f1=({ struct _tuple17* _temp1907=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1907->f1= Cyc_Absyn_empty_tqual();
_temp1907->f2=({ struct Cyc_List_List* _temp1908=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1908->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1908->tl=
0; _temp1908;}); _temp1907->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1907;}); _temp1906;});
_temp1905;}); break; case 101: _LL1904: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1910=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1910[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1911; _temp1911.tag=
Cyc_QualSpecList_tok; _temp1911.f1=({ struct _tuple17* _temp1912=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1912->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1912->f2=({
struct Cyc_List_List* _temp1913=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1913->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1913->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1913;}); _temp1912->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1912;});
_temp1911;}); _temp1910;}); break; case 102: _LL1909: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1915=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1915[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1916; _temp1916.tag= Cyc_QualSpecList_tok;
_temp1916.f1=({ struct _tuple17* _temp1917=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1917->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1917->f2= 0;
_temp1917->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1917;}); _temp1916;}); _temp1915;}); break; case
103: _LL1914: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1919=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1919[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1920; _temp1920.tag=
Cyc_QualSpecList_tok; _temp1920.f1=({ struct _tuple17* _temp1921=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1921->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1921->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1921->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1921;});
_temp1920;}); _temp1919;}); break; case 104: _LL1918: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1923=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1923[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1924; _temp1924.tag= Cyc_InitDeclList_tok;
_temp1924.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1924;}); _temp1923;}); break; case 105: _LL1922: yyval=(
void*)({ struct Cyc_InitDeclList_tok_struct* _temp1926=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1926[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1927; _temp1927.tag= Cyc_InitDeclList_tok;
_temp1927.f1=({ struct Cyc_List_List* _temp1928=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1928->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1928->tl= 0;
_temp1928;}); _temp1927;}); _temp1926;}); break; case 106: _LL1925: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp1930=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1930[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1931; _temp1931.tag= Cyc_InitDeclList_tok;
_temp1931.f1=({ struct Cyc_List_List* _temp1932=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1932->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1932->tl=
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1932;}); _temp1931;}); _temp1930;}); break; case 107:
_LL1929: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp1934=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1934[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1935; _temp1935.tag= Cyc_InitDecl_tok; _temp1935.f1=({ struct _tuple14*
_temp1936=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1936->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1936->f2= 0; _temp1936;}); _temp1935;}); _temp1934;});
break; case 108: _LL1933: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1938=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1938[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1939; _temp1939.tag= Cyc_InitDecl_tok;
_temp1939.f1=({ struct _tuple14* _temp1940=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1940->f1=({ struct Cyc_Parse_Declarator*
_temp1941=( struct Cyc_Parse_Declarator*) _cycalloc( sizeof( struct Cyc_Parse_Declarator));
_temp1941->id=({ struct _tuple1* _temp1942=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp1942->f1= Cyc_Absyn_rel_ns_null; _temp1942->f2=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1942;}); _temp1941->tms= 0; _temp1941;});
_temp1940->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1940;}); _temp1939;});
_temp1938;}); break; case 109: _LL1937: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1944=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1944[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1945; _temp1945.tag= Cyc_InitDecl_tok;
_temp1945.f1=({ struct _tuple14* _temp1946=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1946->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1946->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1946;}); _temp1945;}); _temp1944;}); break; case
110: _LL1943: { struct Cyc_List_List* _temp1948=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1949=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1949[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1950; _temp1950.tag=
Cyc_TypeSpecifier_tok; _temp1950.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1951=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1951[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1952; _temp1952.tag= Cyc_Parse_Decl_spec;
_temp1952.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1948,({
struct Cyc_Core_Opt* _temp1953=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1953->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1953;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1952;}); _temp1951;})); _temp1950;}); _temp1949;}); break;} case 111:
_LL1947: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1955=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1955[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1956; _temp1956.tag=
Cyc_TypeSpecifier_tok; _temp1956.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1957=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1957[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1958; _temp1958.tag= Cyc_Absyn_TunionType;
_temp1958.f1=({ struct Cyc_Absyn_TunionInfo _temp1959; _temp1959.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1960=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1960[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1961; _temp1961.tag= Cyc_Absyn_UnknownTunion;
_temp1961.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1962; _temp1962.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1962.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1962;});
_temp1961;}); _temp1960;})); _temp1959.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1959.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1959;}); _temp1958;}); _temp1957;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1956;}); _temp1955;}); break; case 112: _LL1954: { void* _temp1964= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp1973=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1973->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp1973;}),
0); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1965=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1965[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1966; _temp1966.tag= Cyc_TypeSpecifier_tok;
_temp1966.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1967=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1967[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1968; _temp1968.tag=
Cyc_Absyn_TunionType; _temp1968.f1=({ struct Cyc_Absyn_TunionInfo _temp1969;
_temp1969.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1970=( struct Cyc_Absyn_UnknownTunion_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1970[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1971; _temp1971.tag= Cyc_Absyn_UnknownTunion; _temp1971.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1972; _temp1972.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1972.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1972;}); _temp1971;}); _temp1970;})); _temp1969.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1969.rgn=(
void*) _temp1964; _temp1969;}); _temp1968;}); _temp1967;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1966;}); _temp1965;}); break;} case 113: _LL1963: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1975=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1975[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1976; _temp1976.tag= Cyc_TypeSpecifier_tok;
_temp1976.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1977=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1977[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1978; _temp1978.tag= Cyc_Absyn_TunionFieldType; _temp1978.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1979; _temp1979.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp1980=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp1980[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp1981; _temp1981.tag= Cyc_Absyn_UnknownTunionfield; _temp1981.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp1982; _temp1982.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1982.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1982.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1982;});
_temp1981;}); _temp1980;})); _temp1979.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1979;}); _temp1978;});
_temp1977;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1976;}); _temp1975;}); break; case 114:
_LL1974: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1984=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1984[ 0]=({ struct
Cyc_Bool_tok_struct _temp1985; _temp1985.tag= Cyc_Bool_tok; _temp1985.f1= 0;
_temp1985;}); _temp1984;}); break; case 115: _LL1983: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1987=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1987[ 0]=({ struct Cyc_Bool_tok_struct
_temp1988; _temp1988.tag= Cyc_Bool_tok; _temp1988.f1= 1; _temp1988;}); _temp1987;});
break; case 116: _LL1986: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1990=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1990[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1991; _temp1991.tag= Cyc_TunionFieldList_tok; _temp1991.f1=({ struct Cyc_List_List*
_temp1992=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1992->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1992->tl= 0;
_temp1992;}); _temp1991;}); _temp1990;}); break; case 117: _LL1989: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp1994=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1994[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1995; _temp1995.tag= Cyc_TunionFieldList_tok;
_temp1995.f1=({ struct Cyc_List_List* _temp1996=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1996->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1996->tl=
0; _temp1996;}); _temp1995;}); _temp1994;}); break; case 118: _LL1993: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1998=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1998[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1999; _temp1999.tag= Cyc_TunionFieldList_tok;
_temp1999.f1=({ struct Cyc_List_List* _temp2000=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2000->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2000->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2000;}); _temp1999;}); _temp1998;}); break; case 119:
_LL1997: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2002=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2002[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2003; _temp2003.tag=
Cyc_TunionFieldList_tok; _temp2003.f1=({ struct Cyc_List_List* _temp2004=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2004->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2004->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2004;}); _temp2003;});
_temp2002;}); break; case 120: _LL2001: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2006=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2006[ 0]=({ struct Cyc_Scope_tok_struct _temp2007; _temp2007.tag= Cyc_Scope_tok;
_temp2007.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2007;}); _temp2006;});
break; case 121: _LL2005: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2009=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2009[ 0]=({ struct Cyc_Scope_tok_struct _temp2010; _temp2010.tag= Cyc_Scope_tok;
_temp2010.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2010;}); _temp2009;});
break; case 122: _LL2008: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2012=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2012[ 0]=({ struct Cyc_Scope_tok_struct _temp2013; _temp2013.tag= Cyc_Scope_tok;
_temp2013.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2013;}); _temp2012;});
break; case 123: _LL2011: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2015=( struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2015[ 0]=({ struct Cyc_TunionField_tok_struct _temp2016; _temp2016.tag= Cyc_TunionField_tok;
_temp2016.f1=({ struct Cyc_Absyn_Tunionfield* _temp2017=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2017->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2017->tvs= 0;
_temp2017->typs= 0; _temp2017->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2017->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2017;}); _temp2016;}); _temp2015;}); break; case 124:
_LL2014: { struct Cyc_List_List* _temp2019=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp2020=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp2021=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2021[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2022; _temp2022.tag= Cyc_TunionField_tok;
_temp2022.f1=({ struct Cyc_Absyn_Tunionfield* _temp2023=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2023->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2023->tvs=
_temp2020; _temp2023->typs= _temp2019; _temp2023->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2023->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp2023;}); _temp2022;}); _temp2021;}); break;}
case 125: _LL2018: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 126: _LL2024: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2026=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2026[ 0]=({ struct Cyc_Declarator_tok_struct _temp2027; _temp2027.tag= Cyc_Declarator_tok;
_temp2027.f1=({ struct Cyc_Parse_Declarator* _temp2028=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2028->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2028->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2028;});
_temp2027;}); _temp2026;}); break; case 127: _LL2025: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2030=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2030[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2031; _temp2031.tag= Cyc_Declarator_tok;
_temp2031.f1=({ struct Cyc_Parse_Declarator* _temp2032=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2032->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2032->tms= 0;
_temp2032;}); _temp2031;}); _temp2030;}); break; case 128: _LL2029: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 129:
_LL2033: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2035=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2035[ 0]=({ struct Cyc_Declarator_tok_struct _temp2036; _temp2036.tag= Cyc_Declarator_tok;
_temp2036.f1=({ struct Cyc_Parse_Declarator* _temp2037=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2037->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp2037->tms=({ struct Cyc_List_List* _temp2038=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2038->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2038->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp2038;}); _temp2037;}); _temp2036;});
_temp2035;}); break; case 130: _LL2034: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2040=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2040[ 0]=({ struct Cyc_Declarator_tok_struct _temp2041; _temp2041.tag= Cyc_Declarator_tok;
_temp2041.f1=({ struct Cyc_Parse_Declarator* _temp2042=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2042->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2042->tms=({ struct Cyc_List_List* _temp2043=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2043->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2044=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2044[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2045; _temp2045.tag= Cyc_Absyn_ConstArray_mod;
_temp2045.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2045;}); _temp2044;})); _temp2043->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2043;}); _temp2042;}); _temp2041;}); _temp2040;}); break; case 131: _LL2039: {
struct _tuple16 _temp2049; struct Cyc_List_List* _temp2050; struct Cyc_Core_Opt*
_temp2052; struct Cyc_Absyn_VarargInfo* _temp2054; int _temp2056; struct Cyc_List_List*
_temp2058; struct _tuple16* _temp2047= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2049=*
_temp2047; _LL2059: _temp2058= _temp2049.f1; goto _LL2057; _LL2057: _temp2056=
_temp2049.f2; goto _LL2055; _LL2055: _temp2054= _temp2049.f3; goto _LL2053;
_LL2053: _temp2052= _temp2049.f4; goto _LL2051; _LL2051: _temp2050= _temp2049.f5;
goto _LL2048; _LL2048: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2060=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2060[ 0]=({ struct Cyc_Declarator_tok_struct _temp2061; _temp2061.tag= Cyc_Declarator_tok;
_temp2061.f1=({ struct Cyc_Parse_Declarator* _temp2062=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2062->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2062->tms=({ struct Cyc_List_List* _temp2063=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2063->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2064=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2064[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2065; _temp2065.tag= Cyc_Absyn_Function_mod;
_temp2065.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2066=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2066[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2067; _temp2067.tag= Cyc_Absyn_WithTypes;
_temp2067.f1= _temp2058; _temp2067.f2= _temp2056; _temp2067.f3= _temp2054;
_temp2067.f4= _temp2052; _temp2067.f5= _temp2050; _temp2067;}); _temp2066;}));
_temp2065;}); _temp2064;})); _temp2063->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2063;});
_temp2062;}); _temp2061;}); _temp2060;}); break;} case 132: _LL2046: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2069=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2069[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2070; _temp2070.tag= Cyc_Declarator_tok;
_temp2070.f1=({ struct Cyc_Parse_Declarator* _temp2071=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2071->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2071->tms=({ struct Cyc_List_List* _temp2072=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2072->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2073=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2073[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2074; _temp2074.tag= Cyc_Absyn_Function_mod;
_temp2074.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2075=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2075[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2076; _temp2076.tag= Cyc_Absyn_WithTypes;
_temp2076.f1= 0; _temp2076.f2= 0; _temp2076.f3= 0; _temp2076.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2076.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2076;}); _temp2075;})); _temp2074;}); _temp2073;}));
_temp2072->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2072;}); _temp2071;}); _temp2070;});
_temp2069;}); break; case 133: _LL2068: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2078=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2078[ 0]=({ struct Cyc_Declarator_tok_struct _temp2079; _temp2079.tag= Cyc_Declarator_tok;
_temp2079.f1=({ struct Cyc_Parse_Declarator* _temp2080=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2080->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2080->tms=({ struct Cyc_List_List* _temp2081=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2081->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2082=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2082[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2083; _temp2083.tag= Cyc_Absyn_Function_mod;
_temp2083.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2084=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2084[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2085; _temp2085.tag= Cyc_Absyn_NoTypes;
_temp2085.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2085.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2085;});
_temp2084;})); _temp2083;}); _temp2082;})); _temp2081->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2081;}); _temp2080;}); _temp2079;}); _temp2078;}); break; case 134: _LL2077: {
struct Cyc_List_List* _temp2087=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
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
_temp2090;}); _temp2089;}); _temp2088;}); break;} case 135: _LL2086:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2095=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2096=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2096[ 0]=({ struct Cyc_Declarator_tok_struct _temp2097; _temp2097.tag= Cyc_Declarator_tok;
_temp2097.f1=({ struct Cyc_Parse_Declarator* _temp2098=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2098->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2098->tms=({ struct Cyc_List_List* _temp2099=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2099->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2100=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2100[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2101; _temp2101.tag= Cyc_Absyn_TypeParams_mod;
_temp2101.f1= _temp2095; _temp2101.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2101.f3=
0; _temp2101;}); _temp2100;})); _temp2099->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2099;});
_temp2098;}); _temp2097;}); _temp2096;}); break;} case 136: _LL2094: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2103=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2103[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2104; _temp2104.tag= Cyc_Declarator_tok;
_temp2104.f1=({ struct Cyc_Parse_Declarator* _temp2105=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2105->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2105->tms=({ struct Cyc_List_List* _temp2106=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2106->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2107=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2107[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2108; _temp2108.tag= Cyc_Absyn_Attributes_mod;
_temp2108.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2108.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2108;}); _temp2107;}));
_temp2106->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]))->tms; _temp2106;}); _temp2105;}); _temp2104;});
_temp2103;}); break; case 137: _LL2102: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2110=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2110[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2111; _temp2111.tag= Cyc_Declarator_tok;
_temp2111.f1=({ struct Cyc_Parse_Declarator* _temp2112=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2112->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2112->tms= 0;
_temp2112;}); _temp2111;}); _temp2110;}); break; case 138: _LL2109: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2114=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2114[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2115; _temp2115.tag= Cyc_Declarator_tok;
_temp2115.f1=({ struct Cyc_Parse_Declarator* _temp2116=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2116->id= Cyc_Absyn_exn_name;
_temp2116->tms= 0; _temp2116;}); _temp2115;}); _temp2114;}); break; case 139:
_LL2113: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2118=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2118[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2119; _temp2119.tag=
Cyc_TypeModifierList_tok; _temp2119.f1=({ struct Cyc_List_List* _temp2120=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2120->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2121=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2121[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2122; _temp2122.tag= Cyc_Absyn_Pointer_mod;
_temp2122.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2122.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2122.f3= Cyc_Absyn_empty_tqual(); _temp2122;});
_temp2121;})); _temp2120->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2120;});
_temp2119;}); _temp2118;}); break; case 140: _LL2117: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2124=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2124[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2125; _temp2125.tag= Cyc_TypeModifierList_tok;
_temp2125.f1=({ struct Cyc_List_List* _temp2126=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2126->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2127=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2127[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2128; _temp2128.tag= Cyc_Absyn_Pointer_mod;
_temp2128.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2128.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2128.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2128;}); _temp2127;}));
_temp2126->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0);
_temp2126;}); _temp2125;}); _temp2124;}); break; case 141: _LL2123: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2130=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2130[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2131; _temp2131.tag= Cyc_TypeModifierList_tok;
_temp2131.f1=({ struct Cyc_List_List* _temp2132=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2132->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2133=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2133[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2134; _temp2134.tag= Cyc_Absyn_Pointer_mod;
_temp2134.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2134.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2134.f3= Cyc_Absyn_empty_tqual(); _temp2134;});
_temp2133;})); _temp2132->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2132;});
_temp2131;}); _temp2130;}); break; case 142: _LL2129: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2136=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2136[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2137; _temp2137.tag= Cyc_TypeModifierList_tok;
_temp2137.f1=({ struct Cyc_List_List* _temp2138=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2138->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2139=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2139[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2140; _temp2140.tag= Cyc_Absyn_Pointer_mod;
_temp2140.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2140.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2140.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2140;});
_temp2139;})); _temp2138->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2138;});
_temp2137;}); _temp2136;}); break; case 143: _LL2135: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2142=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2142[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2143; _temp2143.tag= Cyc_Pointer_Sort_tok;
_temp2143.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2144=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2144[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2145; _temp2145.tag=
Cyc_Absyn_Nullable_ps; _temp2145.f1= Cyc_Absyn_exp_unsigned_one; _temp2145;});
_temp2144;})); _temp2143;}); _temp2142;}); break; case 144: _LL2141: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2147=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2147[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2148; _temp2148.tag= Cyc_Pointer_Sort_tok;
_temp2148.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2149=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2149[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2150; _temp2150.tag= Cyc_Absyn_NonNullable_ps; _temp2150.f1= Cyc_Absyn_exp_unsigned_one;
_temp2150;}); _temp2149;})); _temp2148;}); _temp2147;}); break; case 145:
_LL2146: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2152=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2152[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2153; _temp2153.tag=
Cyc_Pointer_Sort_tok; _temp2153.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2154=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2154[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2155; _temp2155.tag=
Cyc_Absyn_Nullable_ps; _temp2155.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2155;});
_temp2154;})); _temp2153;}); _temp2152;}); break; case 146: _LL2151: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2157=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2157[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2158; _temp2158.tag= Cyc_Pointer_Sort_tok;
_temp2158.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2159=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2159[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2160; _temp2160.tag= Cyc_Absyn_NonNullable_ps; _temp2160.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2160;});
_temp2159;})); _temp2158;}); _temp2157;}); break; case 147: _LL2156: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2162=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2162[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2163; _temp2163.tag= Cyc_Pointer_Sort_tok;
_temp2163.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2163;});
_temp2162;}); break; case 148: _LL2161: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2165=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2165[ 0]=({ struct Cyc_Type_tok_struct _temp2166; _temp2166.tag= Cyc_Type_tok;
_temp2166.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2167=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2167->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2167;}), 0); _temp2166;}); _temp2165;});
break; case 149: _LL2164: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 150: _LL2168: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
151: _LL2169: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2171=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2171[
0]=({ struct Cyc_Type_tok_struct _temp2172; _temp2172.tag= Cyc_Type_tok;
_temp2172.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2173=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2173->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2173;}), 0); _temp2172;}); _temp2171;});
break; case 152: _LL2170: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 153: _LL2174: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2176=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2176[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2177; _temp2177.tag= Cyc_TypeQual_tok; _temp2177.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2177;});
_temp2176;}); break; case 154: _LL2175: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2179=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2179[ 0]=({ struct Cyc_YY1_struct _temp2180; _temp2180.tag= Cyc_YY1;
_temp2180.f1=({ struct _tuple16* _temp2181=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2181->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2181->f2= 0;
_temp2181->f3= 0; _temp2181->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2181->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2181;});
_temp2180;}); _temp2179;}); break; case 155: _LL2178: yyval=( void*)({ struct
Cyc_YY1_struct* _temp2183=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct
Cyc_YY1_struct)); _temp2183[ 0]=({ struct Cyc_YY1_struct _temp2184; _temp2184.tag=
Cyc_YY1; _temp2184.f1=({ struct _tuple16* _temp2185=( struct _tuple16*)
_cycalloc( sizeof( struct _tuple16)); _temp2185->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2185->f2= 1;
_temp2185->f3= 0; _temp2185->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2185->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2185;});
_temp2184;}); _temp2183;}); break; case 156: _LL2182: { struct _tuple2 _temp2189;
void* _temp2190; struct Cyc_Absyn_Tqual _temp2192; struct Cyc_Core_Opt*
_temp2194; struct _tuple2* _temp2187= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2189=*
_temp2187; _LL2195: _temp2194= _temp2189.f1; goto _LL2193; _LL2193: _temp2192=
_temp2189.f2; goto _LL2191; _LL2191: _temp2190= _temp2189.f3; goto _LL2188;
_LL2188: { struct Cyc_Absyn_VarargInfo* _temp2196=({ struct Cyc_Absyn_VarargInfo*
_temp2200=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2200->name= _temp2194; _temp2200->tq= _temp2192; _temp2200->type=( void*)
_temp2190; _temp2200->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2200;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2197=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2197[ 0]=({ struct Cyc_YY1_struct
_temp2198; _temp2198.tag= Cyc_YY1; _temp2198.f1=({ struct _tuple16* _temp2199=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2199->f1= 0;
_temp2199->f2= 0; _temp2199->f3= _temp2196; _temp2199->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2199->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2199;}); _temp2198;}); _temp2197;}); break;}} case 157:
_LL2186: { struct _tuple2 _temp2204; void* _temp2205; struct Cyc_Absyn_Tqual
_temp2207; struct Cyc_Core_Opt* _temp2209; struct _tuple2* _temp2202= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2204=*
_temp2202; _LL2210: _temp2209= _temp2204.f1; goto _LL2208; _LL2208: _temp2207=
_temp2204.f2; goto _LL2206; _LL2206: _temp2205= _temp2204.f3; goto _LL2203;
_LL2203: { struct Cyc_Absyn_VarargInfo* _temp2211=({ struct Cyc_Absyn_VarargInfo*
_temp2215=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2215->name= _temp2209; _temp2215->tq= _temp2207; _temp2215->type=( void*)
_temp2205; _temp2215->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2215;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2212=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2212[ 0]=({ struct Cyc_YY1_struct
_temp2213; _temp2213.tag= Cyc_YY1; _temp2213.f1=({ struct _tuple16* _temp2214=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2214->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2214->f2=
0; _temp2214->f3= _temp2211; _temp2214->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2214->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2214;});
_temp2213;}); _temp2212;}); break;}} case 158: _LL2201: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2217=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2217[ 0]=({ struct Cyc_Type_tok_struct
_temp2218; _temp2218.tag= Cyc_Type_tok; _temp2218.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*)({ struct Cyc_Absyn_Unknown_kb_struct* _temp2219=( struct Cyc_Absyn_Unknown_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct)); _temp2219[ 0]=({ struct
Cyc_Absyn_Unknown_kb_struct _temp2220; _temp2220.tag= Cyc_Absyn_Unknown_kb;
_temp2220.f1= 0; _temp2220;}); _temp2219;})); _temp2218;}); _temp2217;}); break;
case 159: _LL2216: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2222=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2222[ 0]=({ struct Cyc_Type_tok_struct _temp2223; _temp2223.tag= Cyc_Type_tok;
_temp2223.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2224=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2224[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2225; _temp2225.tag= Cyc_Absyn_Eq_kb; _temp2225.f1=( void*) Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2225;});
_temp2224;})); _temp2223;}); _temp2222;}); break; case 160: _LL2221: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2227=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2227[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2228; _temp2228.tag= Cyc_TypeOpt_tok; _temp2228.f1= 0; _temp2228;});
_temp2227;}); break; case 161: _LL2226: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2230=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2230[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2231; _temp2231.tag= Cyc_TypeOpt_tok;
_temp2231.f1=({ struct Cyc_Core_Opt* _temp2232=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2232->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2233=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2233[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2234; _temp2234.tag= Cyc_Absyn_JoinEff;
_temp2234.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2234;}); _temp2233;})); _temp2232;}); _temp2231;});
_temp2230;}); break; case 162: _LL2229: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2236=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2236[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2237; _temp2237.tag= Cyc_Rgnorder_tok;
_temp2237.f1= 0; _temp2237;}); _temp2236;}); break; case 163: _LL2235: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 164:
_LL2238: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2240=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2240[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2241; _temp2241.tag= Cyc_Rgnorder_tok; _temp2241.f1=({ struct Cyc_List_List*
_temp2242=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2242->hd=( void*)({ struct _tuple18* _temp2243=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2243->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2246=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2246[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2247; _temp2247.tag= Cyc_Absyn_Eq_kb; _temp2247.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2247;}); _temp2246;})); _temp2243->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2244=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2244[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2245; _temp2245.tag= Cyc_Absyn_Eq_kb; _temp2245.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2245;}); _temp2244;})); _temp2243;}); _temp2242->tl= 0; _temp2242;});
_temp2241;}); _temp2240;}); break; case 165: _LL2239: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2249=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2249[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2250; _temp2250.tag= Cyc_Rgnorder_tok; _temp2250.f1=({ struct Cyc_List_List*
_temp2251=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2251->hd=( void*)({ struct _tuple18* _temp2252=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2252->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2255=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2255[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2256; _temp2256.tag= Cyc_Absyn_Eq_kb; _temp2256.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2256;}); _temp2255;})); _temp2252->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2253=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2253[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2254; _temp2254.tag= Cyc_Absyn_Eq_kb; _temp2254.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2254;}); _temp2253;})); _temp2252;}); _temp2251->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2251;});
_temp2250;}); _temp2249;}); break; case 166: _LL2248: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2258=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2258[ 0]=({ struct Cyc_Bool_tok_struct
_temp2259; _temp2259.tag= Cyc_Bool_tok; _temp2259.f1= 0; _temp2259;}); _temp2258;});
break; case 167: _LL2257: { struct _tagged_arr _temp2261= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Std_zstrcmp(
_temp2261, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2262=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2262[ 0]=({ struct Cyc_Bool_tok_struct _temp2263; _temp2263.tag= Cyc_Bool_tok;
_temp2263.f1= 1; _temp2263;}); _temp2262;}); break;} case 168: _LL2260: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 169:
_LL2264: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2266=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2266[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2267; _temp2267.tag= Cyc_TypeList_tok; _temp2267.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2267;});
_temp2266;}); break; case 170: _LL2265: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2269=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2269[ 0]=({ struct Cyc_TypeList_tok_struct _temp2270; _temp2270.tag= Cyc_TypeList_tok;
_temp2270.f1= 0; _temp2270;}); _temp2269;}); break; case 171: _LL2268: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case
172: _LL2271: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2273=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2273[ 0]=({ struct Cyc_TypeList_tok_struct _temp2274; _temp2274.tag= Cyc_TypeList_tok;
_temp2274.f1=({ struct Cyc_List_List* _temp2275=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2275->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp2276=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp2276[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp2277; _temp2277.tag= Cyc_Absyn_RgnsEff; _temp2277.f1=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2277;}); _temp2276;})); _temp2275->tl= 0; _temp2275;});
_temp2274;}); _temp2273;}); break; case 173: _LL2272: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2279=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2279[ 0]=({ struct Cyc_TypeList_tok_struct _temp2280; _temp2280.tag= Cyc_TypeList_tok;
_temp2280.f1=({ struct Cyc_List_List* _temp2281=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2281->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2281->tl= 0;
_temp2281;}); _temp2280;}); _temp2279;}); break; case 174: _LL2278: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2283=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2283[ 0]=({ struct Cyc_TypeList_tok_struct _temp2284; _temp2284.tag= Cyc_TypeList_tok;
_temp2284.f1=({ struct Cyc_List_List* _temp2285=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2285->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2286=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2286[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2287; _temp2287.tag= Cyc_Absyn_AccessEff;
_temp2287.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2287;}); _temp2286;})); _temp2285->tl= 0;
_temp2285;}); _temp2284;}); _temp2283;}); break; case 175: _LL2282: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),( void*) Cyc_Absyn_RgnKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2289=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2289[ 0]=({ struct Cyc_TypeList_tok_struct _temp2290; _temp2290.tag= Cyc_TypeList_tok;
_temp2290.f1=({ struct Cyc_List_List* _temp2291=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2291->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2292=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2292[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2293; _temp2293.tag= Cyc_Absyn_AccessEff;
_temp2293.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2293;}); _temp2292;})); _temp2291->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2291;});
_temp2290;}); _temp2289;}); break; case 176: _LL2288: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2295=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2295[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2296; _temp2296.tag= Cyc_ParamDeclList_tok;
_temp2296.f1=({ struct Cyc_List_List* _temp2297=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2297->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2297->tl= 0;
_temp2297;}); _temp2296;}); _temp2295;}); break; case 177: _LL2294: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2299=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2299[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2300; _temp2300.tag= Cyc_ParamDeclList_tok;
_temp2300.f1=({ struct Cyc_List_List* _temp2301=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2301->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2301->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2301;}); _temp2300;}); _temp2299;}); break; case 178:
_LL2298: { struct _tuple17 _temp2305; struct Cyc_List_List* _temp2306; struct
Cyc_List_List* _temp2308; struct Cyc_Absyn_Tqual _temp2310; struct _tuple17*
_temp2303= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2305=* _temp2303; _LL2311: _temp2310=
_temp2305.f1; goto _LL2309; _LL2309: _temp2308= _temp2305.f2; goto _LL2307;
_LL2307: _temp2306= _temp2305.f3; goto _LL2304; _LL2304: { struct Cyc_Parse_Declarator
_temp2314; struct Cyc_List_List* _temp2315; struct _tuple1* _temp2317; struct
Cyc_Parse_Declarator* _temp2312= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2314=* _temp2312;
_LL2318: _temp2317= _temp2314.id; goto _LL2316; _LL2316: _temp2315= _temp2314.tms;
goto _LL2313; _LL2313: { void* _temp2319= Cyc_Parse_speclist2typ( _temp2308, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2322; struct Cyc_List_List* _temp2324; void*
_temp2326; struct Cyc_Absyn_Tqual _temp2328; struct _tuple6 _temp2320= Cyc_Parse_apply_tms(
_temp2310, _temp2319, _temp2306, _temp2315); _LL2329: _temp2328= _temp2320.f1;
goto _LL2327; _LL2327: _temp2326= _temp2320.f2; goto _LL2325; _LL2325: _temp2324=
_temp2320.f3; goto _LL2323; _LL2323: _temp2322= _temp2320.f4; goto _LL2321;
_LL2321: if( _temp2324 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2317)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2330=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2334=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2334->v=( void*)(* _temp2317).f2; _temp2334;}); if( _temp2322 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2331=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2331[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2332; _temp2332.tag= Cyc_ParamDecl_tok;
_temp2332.f1=({ struct _tuple2* _temp2333=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2333->f1= _temp2330; _temp2333->f2= _temp2328; _temp2333->f3=
_temp2326; _temp2333;}); _temp2332;}); _temp2331;}); break;}}}} case 179:
_LL2302: { struct _tuple17 _temp2338; struct Cyc_List_List* _temp2339; struct
Cyc_List_List* _temp2341; struct Cyc_Absyn_Tqual _temp2343; struct _tuple17*
_temp2336= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2338=* _temp2336; _LL2344: _temp2343= _temp2338.f1;
goto _LL2342; _LL2342: _temp2341= _temp2338.f2; goto _LL2340; _LL2340: _temp2339=
_temp2338.f3; goto _LL2337; _LL2337: { void* _temp2345= Cyc_Parse_speclist2typ(
_temp2341, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2339 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2346=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2346[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2347; _temp2347.tag= Cyc_ParamDecl_tok; _temp2347.f1=({
struct _tuple2* _temp2348=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2348->f1= 0; _temp2348->f2= _temp2343; _temp2348->f3= _temp2345; _temp2348;});
_temp2347;}); _temp2346;}); break;}} case 180: _LL2335: { struct _tuple17
_temp2352; struct Cyc_List_List* _temp2353; struct Cyc_List_List* _temp2355;
struct Cyc_Absyn_Tqual _temp2357; struct _tuple17* _temp2350= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2352=*
_temp2350; _LL2358: _temp2357= _temp2352.f1; goto _LL2356; _LL2356: _temp2355=
_temp2352.f2; goto _LL2354; _LL2354: _temp2353= _temp2352.f3; goto _LL2351;
_LL2351: { void* _temp2359= Cyc_Parse_speclist2typ( _temp2355, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2360=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2363; struct Cyc_List_List* _temp2365; void* _temp2367; struct Cyc_Absyn_Tqual
_temp2369; struct _tuple6 _temp2361= Cyc_Parse_apply_tms( _temp2357, _temp2359,
_temp2353, _temp2360); _LL2370: _temp2369= _temp2361.f1; goto _LL2368; _LL2368:
_temp2367= _temp2361.f2; goto _LL2366; _LL2366: _temp2365= _temp2361.f3; goto
_LL2364; _LL2364: _temp2363= _temp2361.f4; goto _LL2362; _LL2362: if( _temp2365
!=  0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2363 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2371=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2371[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2372; _temp2372.tag= Cyc_ParamDecl_tok; _temp2372.f1=({
struct _tuple2* _temp2373=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2373->f1= 0; _temp2373->f2= _temp2369; _temp2373->f3= _temp2367; _temp2373;});
_temp2372;}); _temp2371;}); break;}} case 181: _LL2349: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2375=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2375[ 0]=({ struct Cyc_IdList_tok_struct
_temp2376; _temp2376.tag= Cyc_IdList_tok; _temp2376.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2376;});
_temp2375;}); break; case 182: _LL2374: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2378=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2378[ 0]=({ struct Cyc_IdList_tok_struct _temp2379; _temp2379.tag= Cyc_IdList_tok;
_temp2379.f1=({ struct Cyc_List_List* _temp2380=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2380->hd=( void*)({ struct
_tagged_arr* _temp2381=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2381[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2381;}); _temp2380->tl=
0; _temp2380;}); _temp2379;}); _temp2378;}); break; case 183: _LL2377: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2383=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2383[ 0]=({ struct Cyc_IdList_tok_struct
_temp2384; _temp2384.tag= Cyc_IdList_tok; _temp2384.f1=({ struct Cyc_List_List*
_temp2385=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2385->hd=( void*)({ struct _tagged_arr* _temp2386=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2386[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2386;});
_temp2385->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2385;}); _temp2384;}); _temp2383;}); break; case 184:
_LL2382: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 185: _LL2387: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 186: _LL2388: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2390=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2390[ 0]=({ struct Cyc_Exp_tok_struct _temp2391; _temp2391.tag= Cyc_Exp_tok;
_temp2391.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2392=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2392[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2393; _temp2393.tag= Cyc_Absyn_UnresolvedMem_e; _temp2393.f1= 0; _temp2393.f2=
0; _temp2393;}); _temp2392;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2391;});
_temp2390;}); break; case 187: _LL2389: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2395=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2395[ 0]=({ struct Cyc_Exp_tok_struct _temp2396; _temp2396.tag= Cyc_Exp_tok;
_temp2396.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2397=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2397[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2398; _temp2398.tag= Cyc_Absyn_UnresolvedMem_e; _temp2398.f1= 0; _temp2398.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2398;});
_temp2397;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2396;}); _temp2395;}); break; case 188:
_LL2394: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2400=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2400[ 0]=({ struct Cyc_Exp_tok_struct
_temp2401; _temp2401.tag= Cyc_Exp_tok; _temp2401.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2402=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2402[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2403; _temp2403.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2403.f1= 0; _temp2403.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2403;});
_temp2402;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2401;}); _temp2400;}); break; case 189:
_LL2399: { struct Cyc_Absyn_Vardecl* _temp2405= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2411=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2411->f1=( void*) Cyc_Absyn_Loc_n; _temp2411->f2=({ struct _tagged_arr*
_temp2412=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp2412[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); _temp2412;}); _temp2411;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp2405->tq=({ struct Cyc_Absyn_Tqual _temp2406; _temp2406.q_const= 1;
_temp2406.q_volatile= 0; _temp2406.q_restrict= 1; _temp2406;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2407=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2407[ 0]=({ struct Cyc_Exp_tok_struct
_temp2408; _temp2408.tag= Cyc_Exp_tok; _temp2408.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2409=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2409[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2410; _temp2410.tag= Cyc_Absyn_Comprehension_e;
_temp2410.f1= _temp2405; _temp2410.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2410.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2410;});
_temp2409;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2408;}); _temp2407;}); break;} case
190: _LL2404: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2414=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2414[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2415; _temp2415.tag=
Cyc_InitializerList_tok; _temp2415.f1=({ struct Cyc_List_List* _temp2416=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2416->hd=(
void*)({ struct _tuple19* _temp2417=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2417->f1= 0; _temp2417->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2417;}); _temp2416->tl=
0; _temp2416;}); _temp2415;}); _temp2414;}); break; case 191: _LL2413: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2419=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2419[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2420; _temp2420.tag= Cyc_InitializerList_tok;
_temp2420.f1=({ struct Cyc_List_List* _temp2421=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2421->hd=( void*)({ struct
_tuple19* _temp2422=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2422->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2422->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2422;});
_temp2421->tl= 0; _temp2421;}); _temp2420;}); _temp2419;}); break; case 192:
_LL2418: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2424=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2424[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2425; _temp2425.tag=
Cyc_InitializerList_tok; _temp2425.f1=({ struct Cyc_List_List* _temp2426=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2426->hd=(
void*)({ struct _tuple19* _temp2427=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2427->f1= 0; _temp2427->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2427;}); _temp2426->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2426;}); _temp2425;}); _temp2424;}); break; case 193:
_LL2423: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2429=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2429[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2430; _temp2430.tag=
Cyc_InitializerList_tok; _temp2430.f1=({ struct Cyc_List_List* _temp2431=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2431->hd=(
void*)({ struct _tuple19* _temp2432=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2432->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2432->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2432;});
_temp2431->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2431;});
_temp2430;}); _temp2429;}); break; case 194: _LL2428: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2434=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2434[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2435; _temp2435.tag= Cyc_DesignatorList_tok;
_temp2435.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2435;});
_temp2434;}); break; case 195: _LL2433: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2437=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2437[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2438; _temp2438.tag=
Cyc_DesignatorList_tok; _temp2438.f1=({ struct Cyc_List_List* _temp2439=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2439->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2439->tl= 0; _temp2439;}); _temp2438;}); _temp2437;});
break; case 196: _LL2436: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2441=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2441[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2442; _temp2442.tag=
Cyc_DesignatorList_tok; _temp2442.f1=({ struct Cyc_List_List* _temp2443=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2443->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2443->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2443;});
_temp2442;}); _temp2441;}); break; case 197: _LL2440: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2445=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2445[ 0]=({ struct
Cyc_Designator_tok_struct _temp2446; _temp2446.tag= Cyc_Designator_tok;
_temp2446.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2447=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2447[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2448; _temp2448.tag=
Cyc_Absyn_ArrayElement; _temp2448.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2448;});
_temp2447;})); _temp2446;}); _temp2445;}); break; case 198: _LL2444: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2450=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2450[ 0]=({ struct
Cyc_Designator_tok_struct _temp2451; _temp2451.tag= Cyc_Designator_tok;
_temp2451.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2452=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2452[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2453; _temp2453.tag= Cyc_Absyn_FieldName;
_temp2453.f1=({ struct _tagged_arr* _temp2454=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2454[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2454;}); _temp2453;});
_temp2452;})); _temp2451;}); _temp2450;}); break; case 199: _LL2449: { void*
_temp2456= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2457=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2457 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2458=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2459=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2459[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2460; _temp2460.tag= Cyc_ParamDecl_tok; _temp2460.f1=({
struct _tuple2* _temp2461=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2461->f1= 0; _temp2461->f2= _temp2458; _temp2461->f3= _temp2456; _temp2461;});
_temp2460;}); _temp2459;}); break;}} case 200: _LL2455: { void* _temp2463= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp2464=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2465=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2466=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2467= Cyc_Parse_apply_tms( _temp2465, _temp2463, _temp2464,
_temp2466); if( _temp2467.f3 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2467.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2468=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2468[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2469; _temp2469.tag= Cyc_ParamDecl_tok; _temp2469.f1=({
struct _tuple2* _temp2470=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2470->f1= 0; _temp2470->f2= _temp2467.f1; _temp2470->f3= _temp2467.f2;
_temp2470;}); _temp2469;}); _temp2468;}); break;} case 201: _LL2462: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2472=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2472[ 0]=({ struct Cyc_Type_tok_struct
_temp2473; _temp2473.tag= Cyc_Type_tok; _temp2473.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2473;});
_temp2472;}); break; case 202: _LL2471: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2475=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2475[ 0]=({ struct Cyc_Type_tok_struct _temp2476; _temp2476.tag= Cyc_Type_tok;
_temp2476.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2477=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2477[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2478; _temp2478.tag= Cyc_Absyn_JoinEff;
_temp2478.f1= 0; _temp2478;}); _temp2477;})); _temp2476;}); _temp2475;}); break;
case 203: _LL2474: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2480=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2480[ 0]=({ struct Cyc_Type_tok_struct _temp2481; _temp2481.tag= Cyc_Type_tok;
_temp2481.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2482=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2482[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2483; _temp2483.tag= Cyc_Absyn_JoinEff;
_temp2483.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2483;}); _temp2482;})); _temp2481;});
_temp2480;}); break; case 204: _LL2479: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2485=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2485[ 0]=({ struct Cyc_Type_tok_struct _temp2486; _temp2486.tag= Cyc_Type_tok;
_temp2486.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2487=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2487[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2488; _temp2488.tag= Cyc_Absyn_RgnsEff;
_temp2488.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2488;}); _temp2487;})); _temp2486;});
_temp2485;}); break; case 205: _LL2484: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2490=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2490[ 0]=({ struct Cyc_Type_tok_struct _temp2491; _temp2491.tag= Cyc_Type_tok;
_temp2491.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2492=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2492[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2493; _temp2493.tag= Cyc_Absyn_JoinEff;
_temp2493.f1=({ struct Cyc_List_List* _temp2494=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2494->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2494->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2494;}); _temp2493;}); _temp2492;})); _temp2491;});
_temp2490;}); break; case 206: _LL2489: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2496=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2496[ 0]=({ struct Cyc_TypeList_tok_struct _temp2497; _temp2497.tag= Cyc_TypeList_tok;
_temp2497.f1=({ struct Cyc_List_List* _temp2498=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2498->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2498->tl= 0;
_temp2498;}); _temp2497;}); _temp2496;}); break; case 207: _LL2495: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2500=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2500[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2501; _temp2501.tag= Cyc_TypeList_tok; _temp2501.f1=({ struct Cyc_List_List*
_temp2502=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2502->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2502->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2502;});
_temp2501;}); _temp2500;}); break; case 208: _LL2499: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2504=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2504[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2505; _temp2505.tag= Cyc_AbstractDeclarator_tok;
_temp2505.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2506=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2506->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2506;});
_temp2505;}); _temp2504;}); break; case 209: _LL2503: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 210: _LL2507:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2509=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2509[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2510; _temp2510.tag=
Cyc_AbstractDeclarator_tok; _temp2510.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2511=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2511->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2511;});
_temp2510;}); _temp2509;}); break; case 211: _LL2508: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 212:
_LL2512: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2514=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2514[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2515; _temp2515.tag=
Cyc_AbstractDeclarator_tok; _temp2515.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2516=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2516->tms=({ struct Cyc_List_List* _temp2517=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2517->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2517->tl= 0; _temp2517;}); _temp2516;}); _temp2515;}); _temp2514;}); break;
case 213: _LL2513: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2519=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2519[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2520; _temp2520.tag= Cyc_AbstractDeclarator_tok; _temp2520.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2521=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2521->tms=({ struct Cyc_List_List* _temp2522=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2522->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2522->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2522;});
_temp2521;}); _temp2520;}); _temp2519;}); break; case 214: _LL2518: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2524=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2524[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2525; _temp2525.tag= Cyc_AbstractDeclarator_tok;
_temp2525.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2526=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2526->tms=({
struct Cyc_List_List* _temp2527=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2527->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2528=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2528[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2529; _temp2529.tag= Cyc_Absyn_ConstArray_mod; _temp2529.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2529;});
_temp2528;})); _temp2527->tl= 0; _temp2527;}); _temp2526;}); _temp2525;});
_temp2524;}); break; case 215: _LL2523: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2531=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2531[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2532; _temp2532.tag= Cyc_AbstractDeclarator_tok; _temp2532.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2533=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2533->tms=({ struct Cyc_List_List* _temp2534=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2534->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2535=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2535[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2536; _temp2536.tag= Cyc_Absyn_ConstArray_mod;
_temp2536.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2536;}); _temp2535;})); _temp2534->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2534;}); _temp2533;}); _temp2532;}); _temp2531;}); break; case 216: _LL2530:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2538=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2538[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2539; _temp2539.tag=
Cyc_AbstractDeclarator_tok; _temp2539.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2540=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2540->tms=({ struct Cyc_List_List* _temp2541=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2541->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2542=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2542[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2543; _temp2543.tag= Cyc_Absyn_Function_mod;
_temp2543.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2544=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2544[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2545; _temp2545.tag= Cyc_Absyn_WithTypes;
_temp2545.f1= 0; _temp2545.f2= 0; _temp2545.f3= 0; _temp2545.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2545.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2545;}); _temp2544;})); _temp2543;}); _temp2542;}));
_temp2541->tl= 0; _temp2541;}); _temp2540;}); _temp2539;}); _temp2538;}); break;
case 217: _LL2537: { struct _tuple16 _temp2549; struct Cyc_List_List* _temp2550;
struct Cyc_Core_Opt* _temp2552; struct Cyc_Absyn_VarargInfo* _temp2554; int
_temp2556; struct Cyc_List_List* _temp2558; struct _tuple16* _temp2547= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2549=*
_temp2547; _LL2559: _temp2558= _temp2549.f1; goto _LL2557; _LL2557: _temp2556=
_temp2549.f2; goto _LL2555; _LL2555: _temp2554= _temp2549.f3; goto _LL2553;
_LL2553: _temp2552= _temp2549.f4; goto _LL2551; _LL2551: _temp2550= _temp2549.f5;
goto _LL2548; _LL2548: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2560=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2560[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2561; _temp2561.tag= Cyc_AbstractDeclarator_tok; _temp2561.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2562=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2562->tms=({ struct Cyc_List_List* _temp2563=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2563->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2564=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2564[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2565; _temp2565.tag= Cyc_Absyn_Function_mod;
_temp2565.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2566=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2566[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2567; _temp2567.tag= Cyc_Absyn_WithTypes;
_temp2567.f1= _temp2558; _temp2567.f2= _temp2556; _temp2567.f3= _temp2554;
_temp2567.f4= _temp2552; _temp2567.f5= _temp2550; _temp2567;}); _temp2566;}));
_temp2565;}); _temp2564;})); _temp2563->tl= 0; _temp2563;}); _temp2562;});
_temp2561;}); _temp2560;}); break;} case 218: _LL2546: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2569=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2569[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2570; _temp2570.tag= Cyc_AbstractDeclarator_tok;
_temp2570.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2571=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2571->tms=({
struct Cyc_List_List* _temp2572=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2572->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2573=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2573[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2574; _temp2574.tag=
Cyc_Absyn_Function_mod; _temp2574.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2575=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2575[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2576; _temp2576.tag= Cyc_Absyn_WithTypes;
_temp2576.f1= 0; _temp2576.f2= 0; _temp2576.f3= 0; _temp2576.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2576.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2576;}); _temp2575;})); _temp2574;}); _temp2573;}));
_temp2572->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2572;});
_temp2571;}); _temp2570;}); _temp2569;}); break; case 219: _LL2568: { struct
_tuple16 _temp2580; struct Cyc_List_List* _temp2581; struct Cyc_Core_Opt*
_temp2583; struct Cyc_Absyn_VarargInfo* _temp2585; int _temp2587; struct Cyc_List_List*
_temp2589; struct _tuple16* _temp2578= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2580=*
_temp2578; _LL2590: _temp2589= _temp2580.f1; goto _LL2588; _LL2588: _temp2587=
_temp2580.f2; goto _LL2586; _LL2586: _temp2585= _temp2580.f3; goto _LL2584;
_LL2584: _temp2583= _temp2580.f4; goto _LL2582; _LL2582: _temp2581= _temp2580.f5;
goto _LL2579; _LL2579: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2591=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2591[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2592; _temp2592.tag= Cyc_AbstractDeclarator_tok; _temp2592.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2593=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2593->tms=({ struct Cyc_List_List* _temp2594=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2594->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2595=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2595[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2596; _temp2596.tag= Cyc_Absyn_Function_mod;
_temp2596.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2597=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2597[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2598; _temp2598.tag= Cyc_Absyn_WithTypes;
_temp2598.f1= _temp2589; _temp2598.f2= _temp2587; _temp2598.f3= _temp2585;
_temp2598.f4= _temp2583; _temp2598.f5= _temp2581; _temp2598;}); _temp2597;}));
_temp2596;}); _temp2595;})); _temp2594->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2594;}); _temp2593;}); _temp2592;}); _temp2591;}); break;} case 220:
_LL2577: { struct Cyc_List_List* _temp2600=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2601=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2601[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2602; _temp2602.tag= Cyc_AbstractDeclarator_tok; _temp2602.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2603=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2603->tms=({ struct Cyc_List_List* _temp2604=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2604->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2605=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2605[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2606; _temp2606.tag= Cyc_Absyn_TypeParams_mod;
_temp2606.f1= _temp2600; _temp2606.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2606.f3=
0; _temp2606;}); _temp2605;})); _temp2604->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2604;}); _temp2603;}); _temp2602;}); _temp2601;}); break;} case 221:
_LL2599:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2608=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2609=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2609[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2610; _temp2610.tag= Cyc_AbstractDeclarator_tok; _temp2610.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2611=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2611->tms=({ struct Cyc_List_List* _temp2612=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2612->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2613=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2613[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2614; _temp2614.tag= Cyc_Absyn_TypeParams_mod;
_temp2614.f1= _temp2608; _temp2614.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2614.f3=
0; _temp2614;}); _temp2613;})); _temp2612->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2612;}); _temp2611;}); _temp2610;}); _temp2609;}); break;} case 222:
_LL2607: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2616=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2616[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2617; _temp2617.tag=
Cyc_AbstractDeclarator_tok; _temp2617.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2618=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2618->tms=({ struct Cyc_List_List* _temp2619=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2619->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2620=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2620[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2621; _temp2621.tag= Cyc_Absyn_Attributes_mod;
_temp2621.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2621.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2621;}); _temp2620;}));
_temp2619->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms; _temp2619;});
_temp2618;}); _temp2617;}); _temp2616;}); break; case 223: _LL2615: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 224: _LL2622:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
225: _LL2623: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 226: _LL2624: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 227: _LL2625: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 228: _LL2626:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
229: _LL2627: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2639=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2639->name=({ struct
_tagged_arr* _temp2642=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2642[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2642;});
_temp2639->identity= 0; _temp2639->kind=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp2640=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp2640[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp2641; _temp2641.tag= Cyc_Absyn_Eq_kb;
_temp2641.f1=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2641;}); _temp2640;}));
_temp2639;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2637=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2637[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2638; _temp2638.tag= Cyc_Absyn_VarType;
_temp2638.f1= tv; _temp2638;}); _temp2637;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2629=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2629[ 0]=({ struct Cyc_Stmt_tok_struct _temp2630; _temp2630.tag= Cyc_Stmt_tok;
_temp2630.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2631=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2631[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2632; _temp2632.tag= Cyc_Absyn_Region_s;
_temp2632.f1= tv; _temp2632.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2633=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2633->f1=(
void*) Cyc_Absyn_Loc_n; _temp2633->f2=({ struct _tagged_arr* _temp2634=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2634[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2634;});
_temp2633;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2635=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2635[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2636; _temp2636.tag=
Cyc_Absyn_RgnHandleType; _temp2636.f1=( void*) t; _temp2636;}); _temp2635;}), 0);
_temp2632.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2632;}); _temp2631;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2630;});
_temp2629;}); break;} case 230: _LL2628: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2654=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2654->name=({ struct
_tagged_arr* _temp2657=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2657[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2659; _temp2659.tag= Cyc_Std_String_pa; _temp2659.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2658[ 1u]={& _temp2659}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2658, sizeof( void*), 1u));}});
_temp2657;}); _temp2654->identity= 0; _temp2654->kind=( void*)(( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2655=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2655[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2656; _temp2656.tag= Cyc_Absyn_Eq_kb; _temp2656.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2656;}); _temp2655;})); _temp2654;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2652=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2652[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2653; _temp2653.tag= Cyc_Absyn_VarType;
_temp2653.f1= tv; _temp2653;}); _temp2652;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2644=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2644[ 0]=({ struct Cyc_Stmt_tok_struct _temp2645; _temp2645.tag= Cyc_Stmt_tok;
_temp2645.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2646=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2646[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2647; _temp2647.tag= Cyc_Absyn_Region_s;
_temp2647.f1= tv; _temp2647.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2648=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2648->f1=(
void*) Cyc_Absyn_Loc_n; _temp2648->f2=({ struct _tagged_arr* _temp2649=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2649[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2649;});
_temp2648;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2650=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2650[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2651; _temp2651.tag=
Cyc_Absyn_RgnHandleType; _temp2651.f1=( void*) t; _temp2651;}); _temp2650;}), 0);
_temp2647.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2647;}); _temp2646;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2645;});
_temp2644;}); break;} case 231: _LL2643: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2661=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2661[ 0]=({ struct Cyc_Stmt_tok_struct _temp2662; _temp2662.tag= Cyc_Stmt_tok;
_temp2662.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2663=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2663[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2664; _temp2664.tag= Cyc_Absyn_Cut_s;
_temp2664.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2664;}); _temp2663;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2662;});
_temp2661;}); break; case 232: _LL2660: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2666=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2666[ 0]=({ struct Cyc_Stmt_tok_struct _temp2667; _temp2667.tag= Cyc_Stmt_tok;
_temp2667.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2668=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2668[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2669; _temp2669.tag= Cyc_Absyn_Splice_s;
_temp2669.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2669;}); _temp2668;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2667;});
_temp2666;}); break; case 233: _LL2665: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2671=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2671[ 0]=({ struct Cyc_Stmt_tok_struct _temp2672; _temp2672.tag= Cyc_Stmt_tok;
_temp2672.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2673=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2673[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2674; _temp2674.tag= Cyc_Absyn_Label_s;
_temp2674.f1=({ struct _tagged_arr* _temp2675=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2675[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2675;});
_temp2674.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2674;}); _temp2673;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2672;});
_temp2671;}); break; case 234: _LL2670: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2677=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2677[ 0]=({ struct Cyc_Stmt_tok_struct _temp2678; _temp2678.tag= Cyc_Stmt_tok;
_temp2678.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2678;});
_temp2677;}); break; case 235: _LL2676: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2680=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2680[ 0]=({ struct Cyc_Stmt_tok_struct _temp2681; _temp2681.tag= Cyc_Stmt_tok;
_temp2681.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2681;}); _temp2680;}); break; case 236: _LL2679: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2683=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2683[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2684; _temp2684.tag= Cyc_Stmt_tok; _temp2684.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2684;}); _temp2683;}); break; case 237: _LL2682: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 238:
_LL2685: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2687=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2687[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2688; _temp2688.tag= Cyc_Stmt_tok; _temp2688.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2688;}); _temp2687;}); break;
case 239: _LL2686: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2690=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2690[ 0]=({ struct Cyc_Stmt_tok_struct _temp2691; _temp2691.tag= Cyc_Stmt_tok;
_temp2691.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2691;});
_temp2690;}); break; case 240: _LL2689: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 241: _LL2692:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2694=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2694[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2695; _temp2695.tag= Cyc_Stmt_tok; _temp2695.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2695;}); _temp2694;}); break; case 242: _LL2693: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2697=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2697[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2698; _temp2698.tag= Cyc_Stmt_tok; _temp2698.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2699=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2699[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2700; _temp2700.tag= Cyc_Absyn_Fn_d;
_temp2700.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2700;}); _temp2699;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2698;}); _temp2697;}); break; case 243: _LL2696: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2702=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2702[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2703; _temp2703.tag= Cyc_Stmt_tok; _temp2703.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2704=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2704[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2705; _temp2705.tag= Cyc_Absyn_Fn_d;
_temp2705.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2705;}); _temp2704;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2703;}); _temp2702;}); break; case 244: _LL2701: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2707=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2707[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2708; _temp2708.tag= Cyc_Stmt_tok; _temp2708.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2708;});
_temp2707;}); break; case 245: _LL2706: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2710=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2710[ 0]=({ struct Cyc_Stmt_tok_struct _temp2711; _temp2711.tag= Cyc_Stmt_tok;
_temp2711.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2711;}); _temp2710;}); break; case 246: _LL2709: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2713=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2713[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2714; _temp2714.tag= Cyc_Stmt_tok; _temp2714.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2714;}); _temp2713;}); break; case 247: _LL2712: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2716=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2716[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2717; _temp2717.tag= Cyc_Stmt_tok; _temp2717.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2718=( struct Cyc_Absyn_SwitchC_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2718[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2719; _temp2719.tag= Cyc_Absyn_SwitchC_s;
_temp2719.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2719.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2719;});
_temp2718;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2717;}); _temp2716;}); break; case 248:
_LL2715: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2721=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2721[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2722; _temp2722.tag= Cyc_Stmt_tok; _temp2722.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2722;});
_temp2721;}); break; case 249: _LL2720: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2724=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2724[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2725; _temp2725.tag= Cyc_SwitchClauseList_tok; _temp2725.f1= 0; _temp2725;});
_temp2724;}); break; case 250: _LL2723: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2727=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2727[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2728; _temp2728.tag= Cyc_SwitchClauseList_tok; _temp2728.f1=({ struct Cyc_List_List*
_temp2729=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2729->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2730=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2730->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2730->pat_vars= 0; _temp2730->where_clause= 0; _temp2730->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2730->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2730;}); _temp2729->tl= 0; _temp2729;});
_temp2728;}); _temp2727;}); break; case 251: _LL2726: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2732=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2732[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2733; _temp2733.tag= Cyc_SwitchClauseList_tok;
_temp2733.f1=({ struct Cyc_List_List* _temp2734=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2734->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2735=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2735->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2735->pat_vars= 0; _temp2735->where_clause= 0;
_temp2735->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2735->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2735;});
_temp2734->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2734;}); _temp2733;});
_temp2732;}); break; case 252: _LL2731: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2737=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2737[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2738; _temp2738.tag= Cyc_SwitchClauseList_tok; _temp2738.f1=({ struct Cyc_List_List*
_temp2739=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2739->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2740=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2740->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2740->pat_vars=
0; _temp2740->where_clause= 0; _temp2740->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2740->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2740;}); _temp2739->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2739;});
_temp2738;}); _temp2737;}); break; case 253: _LL2736: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2742=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2742[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2743; _temp2743.tag= Cyc_SwitchClauseList_tok;
_temp2743.f1=({ struct Cyc_List_List* _temp2744=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2744->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2745=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2745->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp2745->pat_vars= 0; _temp2745->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2745->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2745->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2745;});
_temp2744->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2744;}); _temp2743;});
_temp2742;}); break; case 254: _LL2741: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2747=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2747[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2748; _temp2748.tag= Cyc_SwitchClauseList_tok; _temp2748.f1=({ struct Cyc_List_List*
_temp2749=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2749->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2750=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2750->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2750->pat_vars=
0; _temp2750->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2750->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2750->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2750;});
_temp2749->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2749;}); _temp2748;});
_temp2747;}); break; case 255: _LL2746: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2752=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2752[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2753; _temp2753.tag= Cyc_SwitchCClauseList_tok; _temp2753.f1= 0; _temp2753;});
_temp2752;}); break; case 256: _LL2751: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2755=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2755[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2756; _temp2756.tag= Cyc_SwitchCClauseList_tok; _temp2756.f1=({ struct Cyc_List_List*
_temp2757=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2757->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2758=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2758->cnst_exp= 0;
_temp2758->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2758->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2758;});
_temp2757->tl= 0; _temp2757;}); _temp2756;}); _temp2755;}); break; case 257:
_LL2754: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2760=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2760[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2761; _temp2761.tag=
Cyc_SwitchCClauseList_tok; _temp2761.f1=({ struct Cyc_List_List* _temp2762=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2762->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2763=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2763->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2763->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2763->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2763;});
_temp2762->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2762;}); _temp2761;});
_temp2760;}); break; case 258: _LL2759: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2765=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2765[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2766; _temp2766.tag= Cyc_SwitchCClauseList_tok; _temp2766.f1=({ struct Cyc_List_List*
_temp2767=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2767->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2768=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2768->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2768->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2768->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2768;});
_temp2767->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2767;}); _temp2766;});
_temp2765;}); break; case 259: _LL2764: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2770=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2770[ 0]=({ struct Cyc_Stmt_tok_struct _temp2771; _temp2771.tag= Cyc_Stmt_tok;
_temp2771.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2771;}); _temp2770;}); break; case 260: _LL2769: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2773=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2773[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2774; _temp2774.tag= Cyc_Stmt_tok; _temp2774.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2774;}); _temp2773;}); break; case 261: _LL2772: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2776=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2776[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2777; _temp2777.tag= Cyc_Stmt_tok; _temp2777.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2777;}); _temp2776;}); break; case 262: _LL2775: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2779=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2779[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2780; _temp2780.tag= Cyc_Stmt_tok; _temp2780.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2780;}); _temp2779;}); break; case 263: _LL2778: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2782=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2782[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2783; _temp2783.tag= Cyc_Stmt_tok; _temp2783.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2783;}); _temp2782;}); break; case 264: _LL2781: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2785=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2785[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2786; _temp2786.tag= Cyc_Stmt_tok; _temp2786.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2786;}); _temp2785;}); break; case 265: _LL2784: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2788=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2788[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2789; _temp2789.tag= Cyc_Stmt_tok; _temp2789.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2789;}); _temp2788;}); break; case 266: _LL2787: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2791=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2791[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2792; _temp2792.tag= Cyc_Stmt_tok; _temp2792.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2792;});
_temp2791;}); break; case 267: _LL2790: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2794=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2794[ 0]=({ struct Cyc_Stmt_tok_struct _temp2795; _temp2795.tag= Cyc_Stmt_tok;
_temp2795.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2795;});
_temp2794;}); break; case 268: _LL2793: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2797=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2797[ 0]=({ struct Cyc_Stmt_tok_struct _temp2798; _temp2798.tag= Cyc_Stmt_tok;
_temp2798.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2798;}); _temp2797;}); break; case 269: _LL2796: { struct Cyc_List_List*
_temp2800= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt* _temp2801= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2802=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2802[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2803; _temp2803.tag= Cyc_Stmt_tok; _temp2803.f1= Cyc_Parse_flatten_declarations(
_temp2800, _temp2801); _temp2803;}); _temp2802;}); break;} case 270: _LL2799: {
struct Cyc_List_List* _temp2805= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2806= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2807=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2807[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2808; _temp2808.tag= Cyc_Stmt_tok; _temp2808.f1= Cyc_Parse_flatten_declarations(
_temp2805, _temp2806); _temp2808;}); _temp2807;}); break;} case 271: _LL2804: {
struct Cyc_List_List* _temp2810= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2811= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2812=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2812[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2813; _temp2813.tag= Cyc_Stmt_tok; _temp2813.f1= Cyc_Parse_flatten_declarations(
_temp2810, _temp2811); _temp2813;}); _temp2812;}); break;} case 272: _LL2809: {
struct Cyc_List_List* _temp2815= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2816= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2817=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2817[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2818; _temp2818.tag= Cyc_Stmt_tok; _temp2818.f1= Cyc_Parse_flatten_declarations(
_temp2815, _temp2816); _temp2818;}); _temp2817;}); break;} case 273: _LL2814:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2820=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2820[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2821; _temp2821.tag= Cyc_Stmt_tok; _temp2821.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2822=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2822[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2822;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2821;}); _temp2820;}); break; case 274: _LL2819: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2824=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2824[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2825; _temp2825.tag= Cyc_Stmt_tok; _temp2825.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2825;}); _temp2824;}); break; case 275:
_LL2823: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2827=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2827[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2828; _temp2828.tag= Cyc_Stmt_tok; _temp2828.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2828;}); _temp2827;}); break; case 276: _LL2826: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2830=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2830[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2831; _temp2831.tag= Cyc_Stmt_tok; _temp2831.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2831;}); _temp2830;}); break; case 277:
_LL2829: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2833=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2833[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2834; _temp2834.tag= Cyc_Stmt_tok; _temp2834.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2834;}); _temp2833;}); break; case 278: _LL2832: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2836=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2836[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2837; _temp2837.tag= Cyc_Stmt_tok; _temp2837.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2837;}); _temp2836;}); break; case 279:
_LL2835: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2839=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2839[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2840; _temp2840.tag= Cyc_Stmt_tok; _temp2840.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2840;}); _temp2839;}); break; case 280:
_LL2838: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2842=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2842[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2843; _temp2843.tag= Cyc_Stmt_tok; _temp2843.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2843;}); _temp2842;}); break; case 281: _LL2841: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2845=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2845[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2846; _temp2846.tag= Cyc_Pattern_tok; _temp2846.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2846;});
_temp2845;}); break; case 282: _LL2844: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 283:
_LL2847: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2849=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2849[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2850; _temp2850.tag= Cyc_Pattern_tok; _temp2850.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2851=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2851[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2852; _temp2852.tag= Cyc_Absyn_Int_p; _temp2852.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2852.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2852;}); _temp2851;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2850;});
_temp2849;}); break; case 284: _LL2848: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2854=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2854[ 0]=({ struct Cyc_Pattern_tok_struct _temp2855; _temp2855.tag= Cyc_Pattern_tok;
_temp2855.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2856=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2856[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2857; _temp2857.tag= Cyc_Absyn_Int_p;
_temp2857.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2857.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2857;});
_temp2856;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2855;}); _temp2854;}); break; case 285:
_LL2853: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2859=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2859[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2860; _temp2860.tag= Cyc_Pattern_tok; _temp2860.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2861=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2861[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2862; _temp2862.tag= Cyc_Absyn_Float_p; _temp2862.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2862;}); _temp2861;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2860;});
_temp2859;}); break; case 286: _LL2858: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2864=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2864[ 0]=({ struct Cyc_Pattern_tok_struct _temp2865; _temp2865.tag= Cyc_Pattern_tok;
_temp2865.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2866=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2866[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2867; _temp2867.tag= Cyc_Absyn_Char_p; _temp2867.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2867;});
_temp2866;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2865;}); _temp2864;}); break; case 287:
_LL2863: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2869=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2869[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2870; _temp2870.tag= Cyc_Pattern_tok; _temp2870.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2870;});
_temp2869;}); break; case 288: _LL2868: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2872=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2872[ 0]=({ struct Cyc_Pattern_tok_struct _temp2873; _temp2873.tag= Cyc_Pattern_tok;
_temp2873.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2874=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2874[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2875; _temp2875.tag=
Cyc_Absyn_UnknownId_p; _temp2875.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2875;}); _temp2874;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2873;}); _temp2872;}); break; case 289:
_LL2871: { struct Cyc_List_List* _temp2877=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2878=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2878[ 0]=({ struct Cyc_Pattern_tok_struct _temp2879; _temp2879.tag= Cyc_Pattern_tok;
_temp2879.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2880=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2880[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2881; _temp2881.tag= Cyc_Absyn_UnknownCall_p; _temp2881.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2881.f2=
_temp2877; _temp2881.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2881;});
_temp2880;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2879;}); _temp2878;}); break;} case
290: _LL2876: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2883=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2883[ 0]=({ struct Cyc_Pattern_tok_struct _temp2884; _temp2884.tag= Cyc_Pattern_tok;
_temp2884.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2885=( struct Cyc_Absyn_Tuple_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2885[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2886; _temp2886.tag= Cyc_Absyn_Tuple_p;
_temp2886.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2886;}); _temp2885;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2884;}); _temp2883;}); break; case 291: _LL2882: { struct Cyc_List_List*
_temp2888=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); yyval=(
void*)({ struct Cyc_Pattern_tok_struct* _temp2889=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2889[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2890; _temp2890.tag= Cyc_Pattern_tok; _temp2890.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2891=( struct Cyc_Absyn_UnknownFields_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2891[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2892; _temp2892.tag= Cyc_Absyn_UnknownFields_p;
_temp2892.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2892.f2= _temp2888; _temp2892.f3= 0; _temp2892;});
_temp2891;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2890;}); _temp2889;}); break;} case
292: _LL2887: { struct Cyc_List_List* _temp2894=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2895=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2895[ 0]=({ struct Cyc_Pattern_tok_struct _temp2896; _temp2896.tag= Cyc_Pattern_tok;
_temp2896.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2897=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2897[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2898; _temp2898.tag= Cyc_Absyn_UnknownFields_p; _temp2898.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2898.f2=
_temp2894; _temp2898.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2898;});
_temp2897;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2896;}); _temp2895;}); break;} case
293: _LL2893: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2900=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2900[ 0]=({ struct Cyc_Pattern_tok_struct _temp2901; _temp2901.tag= Cyc_Pattern_tok;
_temp2901.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2902=( struct Cyc_Absyn_Pointer_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2902[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2903; _temp2903.tag= Cyc_Absyn_Pointer_p;
_temp2903.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2903;}); _temp2902;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2901;});
_temp2900;}); break; case 294: _LL2899: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2905=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2905[ 0]=({ struct Cyc_Pattern_tok_struct _temp2906; _temp2906.tag= Cyc_Pattern_tok;
_temp2906.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2907=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2907[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2908; _temp2908.tag=
Cyc_Absyn_Reference_p; _temp2908.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2909=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2909->f1=(
void*) Cyc_Absyn_Loc_n; _temp2909->f2=({ struct _tagged_arr* _temp2910=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2910[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2910;});
_temp2909;}),( void*) Cyc_Absyn_VoidType, 0); _temp2908;}); _temp2907;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2906;}); _temp2905;}); break; case 295: _LL2904: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2912=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2912[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2913; _temp2913.tag= Cyc_PatternList_tok;
_temp2913.f1= 0; _temp2913;}); _temp2912;}); break; case 296: _LL2911: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2915=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2915[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2916; _temp2916.tag= Cyc_PatternList_tok;
_temp2916.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2916;}); _temp2915;}); break; case 297: _LL2914: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2918=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2918[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2919; _temp2919.tag= Cyc_PatternList_tok;
_temp2919.f1=({ struct Cyc_List_List* _temp2920=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2920->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2920->tl= 0;
_temp2920;}); _temp2919;}); _temp2918;}); break; case 298: _LL2917: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2922=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2922[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2923; _temp2923.tag= Cyc_PatternList_tok;
_temp2923.f1=({ struct Cyc_List_List* _temp2924=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2924->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2924->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2924;}); _temp2923;}); _temp2922;}); break; case 299:
_LL2921: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2926=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2926[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2927; _temp2927.tag=
Cyc_FieldPattern_tok; _temp2927.f1=({ struct _tuple13* _temp2928=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2928->f1= 0; _temp2928->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2928;}); _temp2927;}); _temp2926;}); break; case 300:
_LL2925: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2930=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2930[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2931; _temp2931.tag=
Cyc_FieldPattern_tok; _temp2931.f1=({ struct _tuple13* _temp2932=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2932->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2932->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2932;}); _temp2931;}); _temp2930;}); break; case 301:
_LL2929: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2934=(
struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2934[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2935; _temp2935.tag=
Cyc_FieldPatternList_tok; _temp2935.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2935;});
_temp2934;}); break; case 302: _LL2933: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2937=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2937[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2938; _temp2938.tag= Cyc_FieldPatternList_tok; _temp2938.f1=({ struct Cyc_List_List*
_temp2939=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2939->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2939->tl= 0;
_temp2939;}); _temp2938;}); _temp2937;}); break; case 303: _LL2936: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2941=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2941[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2942; _temp2942.tag= Cyc_FieldPatternList_tok;
_temp2942.f1=({ struct Cyc_List_List* _temp2943=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2943->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2943->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2943;}); _temp2942;}); _temp2941;}); break; case 304:
_LL2940: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 305: _LL2944: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2946=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2946[ 0]=({ struct Cyc_Exp_tok_struct _temp2947; _temp2947.tag= Cyc_Exp_tok;
_temp2947.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2947;}); _temp2946;}); break; case 306: _LL2945: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 307: _LL2948:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2950=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2950[ 0]=({ struct Cyc_Exp_tok_struct
_temp2951; _temp2951.tag= Cyc_Exp_tok; _temp2951.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2951;}); _temp2950;}); break; case 308: _LL2949: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2953=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2953[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2954; _temp2954.tag= Cyc_Primopopt_tok; _temp2954.f1=
0; _temp2954;}); _temp2953;}); break; case 309: _LL2952: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2956=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2956[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2957; _temp2957.tag= Cyc_Primopopt_tok; _temp2957.f1=({
struct Cyc_Core_Opt* _temp2958=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2958->v=( void*)(( void*) Cyc_Absyn_Times); _temp2958;});
_temp2957;}); _temp2956;}); break; case 310: _LL2955: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2960=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2960[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2961; _temp2961.tag= Cyc_Primopopt_tok; _temp2961.f1=({
struct Cyc_Core_Opt* _temp2962=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2962->v=( void*)(( void*) Cyc_Absyn_Div); _temp2962;});
_temp2961;}); _temp2960;}); break; case 311: _LL2959: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2964=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2964[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2965; _temp2965.tag= Cyc_Primopopt_tok; _temp2965.f1=({
struct Cyc_Core_Opt* _temp2966=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2966->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2966;});
_temp2965;}); _temp2964;}); break; case 312: _LL2963: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2968=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2968[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2969; _temp2969.tag= Cyc_Primopopt_tok; _temp2969.f1=({
struct Cyc_Core_Opt* _temp2970=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2970->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2970;});
_temp2969;}); _temp2968;}); break; case 313: _LL2967: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2972=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2972[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2973; _temp2973.tag= Cyc_Primopopt_tok; _temp2973.f1=({
struct Cyc_Core_Opt* _temp2974=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2974->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2974;});
_temp2973;}); _temp2972;}); break; case 314: _LL2971: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2976=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2976[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2977; _temp2977.tag= Cyc_Primopopt_tok; _temp2977.f1=({
struct Cyc_Core_Opt* _temp2978=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2978->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2978;});
_temp2977;}); _temp2976;}); break; case 315: _LL2975: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2980=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2980[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2981; _temp2981.tag= Cyc_Primopopt_tok; _temp2981.f1=({
struct Cyc_Core_Opt* _temp2982=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2982->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2982;});
_temp2981;}); _temp2980;}); break; case 316: _LL2979: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2984=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2984[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2985; _temp2985.tag= Cyc_Primopopt_tok; _temp2985.f1=({
struct Cyc_Core_Opt* _temp2986=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2986->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2986;});
_temp2985;}); _temp2984;}); break; case 317: _LL2983: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2988=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2988[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2989; _temp2989.tag= Cyc_Primopopt_tok; _temp2989.f1=({
struct Cyc_Core_Opt* _temp2990=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2990->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2990;});
_temp2989;}); _temp2988;}); break; case 318: _LL2987: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2992=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2992[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2993; _temp2993.tag= Cyc_Primopopt_tok; _temp2993.f1=({
struct Cyc_Core_Opt* _temp2994=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2994->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2994;});
_temp2993;}); _temp2992;}); break; case 319: _LL2991: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 320: _LL2995:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2997=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2997[ 0]=({ struct Cyc_Exp_tok_struct
_temp2998; _temp2998.tag= Cyc_Exp_tok; _temp2998.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2998;});
_temp2997;}); break; case 321: _LL2996: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3000=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3000[ 0]=({ struct Cyc_Exp_tok_struct _temp3001; _temp3001.tag= Cyc_Exp_tok;
_temp3001.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3001;}); _temp3000;}); break; case 322: _LL2999: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3003=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3003[ 0]=({ struct Cyc_Exp_tok_struct
_temp3004; _temp3004.tag= Cyc_Exp_tok; _temp3004.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3004;}); _temp3003;}); break; case 323: _LL3002: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3006=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3006[ 0]=({ struct Cyc_Exp_tok_struct
_temp3007; _temp3007.tag= Cyc_Exp_tok; _temp3007.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3007;}); _temp3006;}); break; case 324: _LL3005: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3009=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3009[ 0]=({ struct Cyc_Exp_tok_struct
_temp3010; _temp3010.tag= Cyc_Exp_tok; _temp3010.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3010;});
_temp3009;}); break; case 325: _LL3008: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3012=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3012[ 0]=({ struct Cyc_Exp_tok_struct _temp3013; _temp3013.tag= Cyc_Exp_tok;
_temp3013.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3013;}); _temp3012;}); break; case 326: _LL3011: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 327: _LL3014:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
328: _LL3015: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3017=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3017[ 0]=({ struct Cyc_Exp_tok_struct
_temp3018; _temp3018.tag= Cyc_Exp_tok; _temp3018.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3018;}); _temp3017;}); break; case 329: _LL3016: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 330: _LL3019:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3021=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3021[ 0]=({ struct Cyc_Exp_tok_struct
_temp3022; _temp3022.tag= Cyc_Exp_tok; _temp3022.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3022;}); _temp3021;}); break; case 331: _LL3020: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 332: _LL3023:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3025=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3025[ 0]=({ struct Cyc_Exp_tok_struct
_temp3026; _temp3026.tag= Cyc_Exp_tok; _temp3026.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3026;});
_temp3025;}); break; case 333: _LL3024: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 334: _LL3027:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3029=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3029[ 0]=({ struct Cyc_Exp_tok_struct
_temp3030; _temp3030.tag= Cyc_Exp_tok; _temp3030.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3030;});
_temp3029;}); break; case 335: _LL3028: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 336: _LL3031:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3033=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3033[ 0]=({ struct Cyc_Exp_tok_struct
_temp3034; _temp3034.tag= Cyc_Exp_tok; _temp3034.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3034;});
_temp3033;}); break; case 337: _LL3032: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 338: _LL3035:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3037=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3037[ 0]=({ struct Cyc_Exp_tok_struct
_temp3038; _temp3038.tag= Cyc_Exp_tok; _temp3038.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3038;}); _temp3037;}); break; case 339: _LL3036: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3040=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3040[ 0]=({ struct Cyc_Exp_tok_struct
_temp3041; _temp3041.tag= Cyc_Exp_tok; _temp3041.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3041;}); _temp3040;}); break; case 340: _LL3039: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 341: _LL3042:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3044=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3044[ 0]=({ struct Cyc_Exp_tok_struct
_temp3045; _temp3045.tag= Cyc_Exp_tok; _temp3045.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3045;}); _temp3044;}); break; case 342: _LL3043: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3047=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3047[ 0]=({ struct Cyc_Exp_tok_struct
_temp3048; _temp3048.tag= Cyc_Exp_tok; _temp3048.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3048;}); _temp3047;}); break; case 343: _LL3046: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3050=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3050[ 0]=({ struct Cyc_Exp_tok_struct
_temp3051; _temp3051.tag= Cyc_Exp_tok; _temp3051.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3051;}); _temp3050;}); break; case 344: _LL3049: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3053=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3053[ 0]=({ struct Cyc_Exp_tok_struct
_temp3054; _temp3054.tag= Cyc_Exp_tok; _temp3054.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3054;}); _temp3053;}); break; case 345: _LL3052: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 346: _LL3055:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3057=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3057[ 0]=({ struct Cyc_Exp_tok_struct
_temp3058; _temp3058.tag= Cyc_Exp_tok; _temp3058.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3058;}); _temp3057;}); break; case 347: _LL3056: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3060=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3060[ 0]=({ struct Cyc_Exp_tok_struct
_temp3061; _temp3061.tag= Cyc_Exp_tok; _temp3061.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3061;}); _temp3060;}); break; case 348: _LL3059: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 349: _LL3062:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3064=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3064[ 0]=({ struct Cyc_Exp_tok_struct
_temp3065; _temp3065.tag= Cyc_Exp_tok; _temp3065.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3065;});
_temp3064;}); break; case 350: _LL3063: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3067=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3067[ 0]=({ struct Cyc_Exp_tok_struct _temp3068; _temp3068.tag= Cyc_Exp_tok;
_temp3068.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3068;}); _temp3067;}); break; case 351: _LL3066: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 352: _LL3069:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3071=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3071[ 0]=({ struct Cyc_Exp_tok_struct
_temp3072; _temp3072.tag= Cyc_Exp_tok; _temp3072.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3072;});
_temp3071;}); break; case 353: _LL3070: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3074=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3074[ 0]=({ struct Cyc_Exp_tok_struct _temp3075; _temp3075.tag= Cyc_Exp_tok;
_temp3075.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3075;}); _temp3074;}); break; case 354: _LL3073: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3077=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3077[ 0]=({ struct Cyc_Exp_tok_struct
_temp3078; _temp3078.tag= Cyc_Exp_tok; _temp3078.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3078;});
_temp3077;}); break; case 355: _LL3076: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 356: _LL3079:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3081=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3081[ 0]=({ struct Cyc_Exp_tok_struct
_temp3082; _temp3082.tag= Cyc_Exp_tok; _temp3082.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3082;}); _temp3081;}); break; case 357: _LL3080: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 358: _LL3083:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3085=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3085[ 0]=({ struct Cyc_Exp_tok_struct
_temp3086; _temp3086.tag= Cyc_Exp_tok; _temp3086.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3086;}); _temp3085;}); break; case 359: _LL3084: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3088=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3088[ 0]=({ struct Cyc_Exp_tok_struct
_temp3089; _temp3089.tag= Cyc_Exp_tok; _temp3089.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3089;}); _temp3088;}); break; case 360: _LL3087: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3091=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3091[ 0]=({ struct Cyc_Exp_tok_struct
_temp3092; _temp3092.tag= Cyc_Exp_tok; _temp3092.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3092;}); _temp3091;}); break; case 361: _LL3090: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3094=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3094[ 0]=({ struct Cyc_Exp_tok_struct
_temp3095; _temp3095.tag= Cyc_Exp_tok; _temp3095.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3095;}); _temp3094;}); break; case 362: _LL3093: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 363: _LL3096:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3098=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3098[ 0]=({ struct Cyc_Exp_tok_struct
_temp3099; _temp3099.tag= Cyc_Exp_tok; _temp3099.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3099;}); _temp3098;}); break; case 364: _LL3097: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3101=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3101[ 0]=({ struct Cyc_Exp_tok_struct
_temp3102; _temp3102.tag= Cyc_Exp_tok; _temp3102.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3102;});
_temp3101;}); break; case 365: _LL3100: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3104=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3104[ 0]=({ struct Cyc_Exp_tok_struct _temp3105; _temp3105.tag= Cyc_Exp_tok;
_temp3105.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3105;}); _temp3104;}); break; case 366: _LL3103: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3107=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3107[ 0]=({ struct Cyc_Exp_tok_struct
_temp3108; _temp3108.tag= Cyc_Exp_tok; _temp3108.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_StructField_struct*
_temp3109=( struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp3109[ 0]=({ struct Cyc_Absyn_StructField_struct _temp3110; _temp3110.tag=
Cyc_Absyn_StructField; _temp3110.f1=({ struct _tagged_arr* _temp3111=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp3111[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3111;});
_temp3110;}); _temp3109;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3108;});
_temp3107;}); break; case 367: _LL3106: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3113=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3113[ 0]=({ struct Cyc_Exp_tok_struct _temp3114; _temp3114.tag= Cyc_Exp_tok;
_temp3114.f1= Cyc_Absyn_offsetof_exp((* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])).f3,( void*)({
struct Cyc_Absyn_TupleIndex_struct* _temp3115=( struct Cyc_Absyn_TupleIndex_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Absyn_TupleIndex_struct)); _temp3115[ 0]=({
struct Cyc_Absyn_TupleIndex_struct _temp3116; _temp3116.tag= Cyc_Absyn_TupleIndex;
_temp3116.f1=( unsigned int)(* Cyc_yyget_Int_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f2; _temp3116;});
_temp3115;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3114;}); _temp3113;}); break; case 368:
_LL3112: { struct Cyc_Position_Segment* _temp3118= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); struct
Cyc_List_List* _temp3119=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, _temp3118, Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3120=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3120[ 0]=({ struct Cyc_Exp_tok_struct
_temp3121; _temp3121.tag= Cyc_Exp_tok; _temp3121.f1= Cyc_Absyn_gentyp_exp(
_temp3119,(* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3121;});
_temp3120;}); break;} case 369: _LL3117: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3123=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3123[ 0]=({ struct Cyc_Exp_tok_struct _temp3124; _temp3124.tag= Cyc_Exp_tok;
_temp3124.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3125=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3125[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3126; _temp3126.tag= Cyc_Absyn_Malloc_e;
_temp3126.f1= 0; _temp3126.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3126;}); _temp3125;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3124;});
_temp3123;}); break; case 370: _LL3122: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3128=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3128[ 0]=({ struct Cyc_Exp_tok_struct _temp3129; _temp3129.tag= Cyc_Exp_tok;
_temp3129.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3130=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3130[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3131; _temp3131.tag= Cyc_Absyn_Malloc_e;
_temp3131.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3131.f2=(
void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3131;}); _temp3130;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3129;});
_temp3128;}); break; case 371: _LL3127: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3133=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3133[ 0]=({ struct Cyc_Primop_tok_struct _temp3134; _temp3134.tag= Cyc_Primop_tok;
_temp3134.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3134;}); _temp3133;});
break; case 372: _LL3132: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3136=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3136[ 0]=({ struct Cyc_Primop_tok_struct _temp3137; _temp3137.tag= Cyc_Primop_tok;
_temp3137.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3137;}); _temp3136;});
break; case 373: _LL3135: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3139=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3139[ 0]=({ struct Cyc_Primop_tok_struct _temp3140; _temp3140.tag= Cyc_Primop_tok;
_temp3140.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3140;}); _temp3139;});
break; case 374: _LL3138: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 375: _LL3141: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3143=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3143[ 0]=({ struct Cyc_Exp_tok_struct _temp3144; _temp3144.tag= Cyc_Exp_tok;
_temp3144.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3144;}); _temp3143;}); break; case 376: _LL3142: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3146=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3146[ 0]=({ struct Cyc_Exp_tok_struct
_temp3147; _temp3147.tag= Cyc_Exp_tok; _temp3147.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3147;}); _temp3146;}); break; case 377:
_LL3145: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3149=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3149[ 0]=({ struct Cyc_Exp_tok_struct
_temp3150; _temp3150.tag= Cyc_Exp_tok; _temp3150.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3150;});
_temp3149;}); break; case 378: _LL3148: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3152=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3152[ 0]=({ struct Cyc_Exp_tok_struct _temp3153; _temp3153.tag= Cyc_Exp_tok;
_temp3153.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3154=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3154[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3154;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3153;}); _temp3152;}); break; case 379: _LL3151: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3156=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3156[ 0]=({ struct Cyc_Exp_tok_struct
_temp3157; _temp3157.tag= Cyc_Exp_tok; _temp3157.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3157;});
_temp3156;}); break;} case 380: _LL3155: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3159=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3159[ 0]=({ struct Cyc_Exp_tok_struct _temp3160; _temp3160.tag= Cyc_Exp_tok;
_temp3160.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3161=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3161[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3161;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3160;}); _temp3159;}); break; case 381: _LL3158: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3163=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3163[ 0]=({ struct Cyc_Exp_tok_struct
_temp3164; _temp3164.tag= Cyc_Exp_tok; _temp3164.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3164;});
_temp3163;}); break;} case 382: _LL3162: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3166=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3166[ 0]=({ struct Cyc_Exp_tok_struct _temp3167; _temp3167.tag= Cyc_Exp_tok;
_temp3167.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3167;}); _temp3166;}); break; case 383: _LL3165: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3169=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3169[ 0]=({ struct Cyc_Exp_tok_struct
_temp3170; _temp3170.tag= Cyc_Exp_tok; _temp3170.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3170;}); _temp3169;}); break; case 384: _LL3168: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3172=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3172[ 0]=({ struct Cyc_Exp_tok_struct
_temp3173; _temp3173.tag= Cyc_Exp_tok; _temp3173.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3174=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3174[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3175; _temp3175.tag= Cyc_Absyn_CompoundLit_e;
_temp3175.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3175.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3175;});
_temp3174;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3173;}); _temp3172;}); break; case 385:
_LL3171: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3177=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3177[ 0]=({ struct Cyc_Exp_tok_struct
_temp3178; _temp3178.tag= Cyc_Exp_tok; _temp3178.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3179=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3179[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3180; _temp3180.tag= Cyc_Absyn_CompoundLit_e;
_temp3180.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3180.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3180;});
_temp3179;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3178;}); _temp3177;}); break; case 386:
_LL3176: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3182=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3182[ 0]=({ struct Cyc_Exp_tok_struct
_temp3183; _temp3183.tag= Cyc_Exp_tok; _temp3183.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3184=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3184[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3185; _temp3185.tag= Cyc_Absyn_Fill_e; _temp3185.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3185;});
_temp3184;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3183;}); _temp3182;}); break; case 387:
_LL3181: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3187=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3187[ 0]=({ struct Cyc_Exp_tok_struct
_temp3188; _temp3188.tag= Cyc_Exp_tok; _temp3188.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3189=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3189[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3190; _temp3190.tag= Cyc_Absyn_Codegen_e;
_temp3190.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3190;}); _temp3189;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3188;}); _temp3187;}); break; case 388: _LL3186: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3192=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3192[ 0]=({ struct Cyc_Exp_tok_struct
_temp3193; _temp3193.tag= Cyc_Exp_tok; _temp3193.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3194=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3194[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3195; _temp3195.tag= Cyc_Absyn_UnknownId_e;
_temp3195.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3195;}); _temp3194;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3193;});
_temp3192;}); break; case 389: _LL3191: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 390: _LL3196:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3198=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3198[ 0]=({ struct Cyc_Exp_tok_struct
_temp3199; _temp3199.tag= Cyc_Exp_tok; _temp3199.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3199;});
_temp3198;}); break; case 391: _LL3197: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 392:
_LL3200: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3202=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3202[ 0]=({ struct Cyc_Exp_tok_struct
_temp3203; _temp3203.tag= Cyc_Exp_tok; _temp3203.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3204=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3204[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3205; _temp3205.tag=
Cyc_Absyn_UnknownId_e; _temp3205.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3205;});
_temp3204;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3203;});
_temp3202;}); break; case 393: _LL3201: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3207=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3207[ 0]=({ struct Cyc_Exp_tok_struct _temp3208; _temp3208.tag= Cyc_Exp_tok;
_temp3208.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3209=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3209[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3210; _temp3210.tag=
Cyc_Absyn_UnknownId_e; _temp3210.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3210;});
_temp3209;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3208;}); _temp3207;}); break; case 394: _LL3206: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3212=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3212[ 0]=({ struct Cyc_Exp_tok_struct
_temp3213; _temp3213.tag= Cyc_Exp_tok; _temp3213.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3213;}); _temp3212;}); break; case 395: _LL3211: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3215=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3215[ 0]=({ struct Cyc_Exp_tok_struct
_temp3216; _temp3216.tag= Cyc_Exp_tok; _temp3216.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3217=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3217[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3218; _temp3218.tag= Cyc_Absyn_Struct_e;
_temp3218.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3218.f2= 0; _temp3218.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3218.f4= 0;
_temp3218;}); _temp3217;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3216;});
_temp3215;}); break; case 396: _LL3214: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3220=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3220[ 0]=({ struct Cyc_Exp_tok_struct _temp3221; _temp3221.tag= Cyc_Exp_tok;
_temp3221.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3221;}); _temp3220;}); break; case 397: _LL3219: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3223=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3223[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3224; _temp3224.tag= Cyc_ExpList_tok; _temp3224.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3224;});
_temp3223;}); break; case 398: _LL3222: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3226=( struct Cyc_ExpList_tok_struct*) _cycalloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3226[ 0]=({ struct Cyc_ExpList_tok_struct _temp3227; _temp3227.tag= Cyc_ExpList_tok;
_temp3227.f1=({ struct Cyc_List_List* _temp3228=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3228->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3228->tl= 0;
_temp3228;}); _temp3227;}); _temp3226;}); break; case 399: _LL3225: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3230=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3230[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3231; _temp3231.tag= Cyc_ExpList_tok; _temp3231.f1=({ struct Cyc_List_List*
_temp3232=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3232->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3232->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp3232;});
_temp3231;}); _temp3230;}); break; case 400: _LL3229: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3234=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3234[ 0]=({ struct Cyc_Exp_tok_struct
_temp3235; _temp3235.tag= Cyc_Exp_tok; _temp3235.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3235;});
_temp3234;}); break; case 401: _LL3233: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3237=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3237[ 0]=({ struct Cyc_Exp_tok_struct _temp3238; _temp3238.tag= Cyc_Exp_tok;
_temp3238.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3238;});
_temp3237;}); break; case 402: _LL3236: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3240=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3240[ 0]=({ struct Cyc_Exp_tok_struct _temp3241; _temp3241.tag= Cyc_Exp_tok;
_temp3241.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3241;});
_temp3240;}); break; case 403: _LL3239: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3243=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3243[ 0]=({ struct Cyc_Exp_tok_struct _temp3244; _temp3244.tag= Cyc_Exp_tok;
_temp3244.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3244;});
_temp3243;}); break; case 404: _LL3242: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3246=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3246[ 0]=({ struct Cyc_QualId_tok_struct _temp3247; _temp3247.tag= Cyc_QualId_tok;
_temp3247.f1=({ struct _tuple1* _temp3248=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3248->f1= Cyc_Absyn_rel_ns_null; _temp3248->f2=({ struct
_tagged_arr* _temp3249=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3249[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3249;}); _temp3248;});
_temp3247;}); _temp3246;}); break; case 405: _LL3245: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3250:(
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
count ++);}} msg=({ unsigned int _temp3252=( unsigned int)( sze +  15);
unsigned char* _temp3253=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp3252)); struct _tagged_arr _temp3255= _tag_arr(
_temp3253, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp3254= _temp3252; unsigned int i; for( i= 0; i <  _temp3254; i ++){
_temp3253[ i]='\000';}}; _temp3255;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
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
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3265= v; struct
_tuple15* _temp3283; struct _tuple15 _temp3285; int _temp3286; unsigned char
_temp3288; short _temp3290; struct _tagged_arr _temp3292; struct Cyc_Core_Opt*
_temp3294; struct Cyc_Core_Opt* _temp3296; struct Cyc_Core_Opt _temp3298; struct
_tagged_arr* _temp3299; struct _tuple1* _temp3301; struct _tuple1 _temp3303;
struct _tagged_arr* _temp3304; void* _temp3306; _LL3267: if(*(( void**)
_temp3265) ==  Cyc_Int_tok){ _LL3284: _temp3283=(( struct Cyc_Int_tok_struct*)
_temp3265)->f1; _temp3285=* _temp3283; _LL3287: _temp3286= _temp3285.f2; goto
_LL3268;} else{ goto _LL3269;} _LL3269: if(*(( void**) _temp3265) ==  Cyc_Char_tok){
_LL3289: _temp3288=(( struct Cyc_Char_tok_struct*) _temp3265)->f1; goto _LL3270;}
else{ goto _LL3271;} _LL3271: if(*(( void**) _temp3265) ==  Cyc_Short_tok){
_LL3291: _temp3290=(( struct Cyc_Short_tok_struct*) _temp3265)->f1; goto _LL3272;}
else{ goto _LL3273;} _LL3273: if(*(( void**) _temp3265) ==  Cyc_String_tok){
_LL3293: _temp3292=(( struct Cyc_String_tok_struct*) _temp3265)->f1; goto
_LL3274;} else{ goto _LL3275;} _LL3275: if(*(( void**) _temp3265) ==  Cyc_Stringopt_tok){
_LL3295: _temp3294=(( struct Cyc_Stringopt_tok_struct*) _temp3265)->f1; if(
_temp3294 ==  0){ goto _LL3276;} else{ goto _LL3277;}} else{ goto _LL3277;}
_LL3277: if(*(( void**) _temp3265) ==  Cyc_Stringopt_tok){ _LL3297: _temp3296=((
struct Cyc_Stringopt_tok_struct*) _temp3265)->f1; if( _temp3296 ==  0){ goto
_LL3279;} else{ _temp3298=* _temp3296; _LL3300: _temp3299=( struct _tagged_arr*)
_temp3298.v; goto _LL3278;}} else{ goto _LL3279;} _LL3279: if(*(( void**)
_temp3265) ==  Cyc_QualId_tok){ _LL3302: _temp3301=(( struct Cyc_QualId_tok_struct*)
_temp3265)->f1; _temp3303=* _temp3301; _LL3307: _temp3306= _temp3303.f1; goto
_LL3305; _LL3305: _temp3304= _temp3303.f2; goto _LL3280;} else{ goto _LL3281;}
_LL3281: goto _LL3282; _LL3268:({ struct Cyc_Std_Int_pa_struct _temp3309;
_temp3309.tag= Cyc_Std_Int_pa; _temp3309.f1=( int)(( unsigned int) _temp3286);{
void* _temp3308[ 1u]={& _temp3309}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3308, sizeof( void*), 1u));}}); goto
_LL3266; _LL3270:({ struct Cyc_Std_Int_pa_struct _temp3311; _temp3311.tag= Cyc_Std_Int_pa;
_temp3311.f1=( int)(( unsigned int)(( int) _temp3288));{ void* _temp3310[ 1u]={&
_temp3311}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3310, sizeof( void*), 1u));}}); goto _LL3266;
_LL3272:({ struct Cyc_Std_Int_pa_struct _temp3313; _temp3313.tag= Cyc_Std_Int_pa;
_temp3313.f1=( int)(( unsigned int)(( int) _temp3290));{ void* _temp3312[ 1u]={&
_temp3313}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3312, sizeof( void*), 1u));}}); goto _LL3266;
_LL3274:({ struct Cyc_Std_String_pa_struct _temp3315; _temp3315.tag= Cyc_Std_String_pa;
_temp3315.f1=( struct _tagged_arr) _temp3292;{ void* _temp3314[ 1u]={& _temp3315};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3314, sizeof( void*), 1u));}}); goto _LL3266; _LL3276:({ void*
_temp3316[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3316, sizeof( void*), 0u));}); goto _LL3266;
_LL3278:({ struct Cyc_Std_String_pa_struct _temp3318; _temp3318.tag= Cyc_Std_String_pa;
_temp3318.f1=( struct _tagged_arr)* _temp3299;{ void* _temp3317[ 1u]={&
_temp3318}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3317, sizeof( void*), 1u));}}); goto _LL3266;
_LL3280: { struct Cyc_List_List* _temp3319= 0;{ void* _temp3320= _temp3306;
struct Cyc_List_List* _temp3328; struct Cyc_List_List* _temp3330; _LL3322: if((
unsigned int) _temp3320 >  1u?*(( int*) _temp3320) ==  Cyc_Absyn_Rel_n: 0){
_LL3329: _temp3328=(( struct Cyc_Absyn_Rel_n_struct*) _temp3320)->f1; goto
_LL3323;} else{ goto _LL3324;} _LL3324: if(( unsigned int) _temp3320 >  1u?*((
int*) _temp3320) ==  Cyc_Absyn_Abs_n: 0){ _LL3331: _temp3330=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3320)->f1; goto _LL3325;} else{ goto _LL3326;} _LL3326: if( _temp3320 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3327;} else{ goto _LL3321;} _LL3323: _temp3319=
_temp3328; goto _LL3321; _LL3325: _temp3319= _temp3330; goto _LL3321; _LL3327:
goto _LL3321; _LL3321:;} for( 0; _temp3319 !=  0; _temp3319= _temp3319->tl){({
struct Cyc_Std_String_pa_struct _temp3333; _temp3333.tag= Cyc_Std_String_pa;
_temp3333.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _temp3319->hd);{ void*
_temp3332[ 1u]={& _temp3333}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3332, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3335; _temp3335.tag= Cyc_Std_String_pa;
_temp3335.f1=( struct _tagged_arr)* _temp3304;{ void* _temp3334[ 1u]={&
_temp3335}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3334, sizeof( void*), 1u));}}); goto _LL3266;}
_LL3282:({ void* _temp3336[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3336, sizeof( void*), 0u));}); goto
_LL3266; _LL3266:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3337=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3337->v=(
void*) Cyc_Lexing_from_file( f); _temp3337;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
