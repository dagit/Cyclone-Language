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
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt, struct
_tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct _tagged_arr Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*); extern
unsigned char Cyc_Lexing_lexeme_char( struct Cyc_Lexing_lexbuf*, int); extern
int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lexeme_end(
struct Cyc_Lexing_lexbuf*); struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* cmp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s,
void* elt); extern void Cyc_Set_iter( void(* f)( void*), struct Cyc_Set_Set* s);
extern unsigned char Cyc_Set_Absent[ 11u]; extern unsigned int Cyc_Std_strlen(
struct _tagged_arr s); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List*,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_zstrncpy( struct
_tagged_arr, struct _tagged_arr, unsigned int); extern struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr, int ofs, unsigned int n); struct Cyc_Xarray_Xarray{ struct
_tagged_arr elmts; int num_elmts; } ; extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*,
int); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int, void*); extern
void Cyc_Xarray_add( struct Cyc_Xarray_Xarray*, void*); extern int Cyc_Xarray_add_ind(
struct Cyc_Xarray_Xarray*, void*); struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; extern struct
Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int); static const int
Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const int
Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr source;
struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; } ;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_lex( struct Cyc_Position_Segment*,
struct _tagged_arr); extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
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
struct _tuple0* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl** f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0* tunion_name; struct _tuple0* field_name; int is_xtunion; } ;
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
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 11; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_EnumType= 12; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_SizeofType= 13; struct Cyc_Absyn_SizeofType_struct{ int tag; void* f1;
} ; static const int Cyc_Absyn_AnonStructType= 14; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
15; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonEnumType= 16; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
17; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 18; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 2; static const int Cyc_Absyn_AccessEff= 19;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
20; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 21; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
int tag; unsigned int f1; } ; struct Cyc_Absyn_MallocInfo{ int is_calloc; struct
Cyc_Absyn_Exp* rgn; void** elt_type; struct Cyc_Absyn_Exp* num_elts; int
fat_result; } ; static const int Cyc_Absyn_Const_e= 0; struct Cyc_Absyn_Const_e_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Var_e= 1; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple0* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; static
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 25; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
26; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 27; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 28; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 29;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 30; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_AnonEnum_e= 32; struct Cyc_Absyn_AnonEnum_e_struct{
int tag; struct _tuple0* f1; void* f2; struct Cyc_Absyn_Enumfield* f3; } ;
static const int Cyc_Absyn_Malloc_e= 33; struct Cyc_Absyn_Malloc_e_struct{ int
tag; struct Cyc_Absyn_MallocInfo f1; } ; static const int Cyc_Absyn_UnresolvedMem_e=
34; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 35; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 36; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 37; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_ForArrayInfo{
struct Cyc_List_List* defns; struct _tuple2 condition; struct _tuple2 delta;
struct Cyc_Absyn_Stmt* body; } ; static const int Cyc_Absyn_Skip_s= 0; static
const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int
Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
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
_tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 18; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
static const int Cyc_Absyn_ForArray_s= 19; struct Cyc_Absyn_ForArray_s_struct{
int tag; struct Cyc_Absyn_ForArrayInfo f1; } ; struct Cyc_Absyn_Stmt{ void* r;
struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds; int
try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p= 0; static const
int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Null_p= 1; static const int Cyc_Absyn_Int_p=
1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{ int tag; unsigned char
f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
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
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_UnknownCall_p= 12;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p= 13;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
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
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; int escapes; } ; struct Cyc_Absyn_Fndecl{ void*
sc; int is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple0* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple0* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple0*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*);
extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value; extern void*
Cyc_Absyn_rel_ns_null; extern struct Cyc_Core_Opt* Cyc_Parse_lbuf; struct Cyc_Declaration_spec;
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
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple3{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple3* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct _tuple4{ struct Cyc_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple4* f1; } ; extern unsigned char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple5{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple5* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple6{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple6* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple1* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple0* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple7{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple7* f1; } ; extern unsigned char Cyc_Rgnorder_tok[
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
struct _tuple6* f1; } ; extern unsigned char Cyc_YYINITIALSVAL[ 18u]; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; extern struct Cyc_Yyltype Cyc_yylloc; extern void* Cyc_yylval; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k,
void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k); void
Cyc_yyerror( struct _tagged_arr s){ Cyc_Position_post_error( Cyc_Position_mk_err_parse(
Cyc_Position_segment_of_abs( Cyc_yylloc.first_line, Cyc_yylloc.last_line), s));}
struct Cyc_Lex_Trie; static const int Cyc_Lex_Zero= 0; static const int Cyc_Lex_One=
0; struct Cyc_Lex_One_struct{ int tag; int f1; struct Cyc_Lex_Trie* f2; } ;
static const int Cyc_Lex_Many= 1; struct Cyc_Lex_Many_struct{ int tag; struct
Cyc_Lex_Trie** f1; } ; struct Cyc_Lex_Trie{ void* children; int shared_str; } ;
static int Cyc_Lex_num_kws= 0; static struct _tagged_arr Cyc_Lex_kw_nums={( void*)
0u,( void*) 0u,( void*)( 0u +  0u)}; static struct Cyc_Xarray_Xarray* Cyc_Lex_symbols=
0; static struct Cyc_Lex_Trie* Cyc_Lex_ids_trie= 0; static struct Cyc_Lex_Trie*
Cyc_Lex_typedefs_trie= 0; static int Cyc_Lex_comment_depth= 0; static struct
_tuple5 Cyc_Lex_token_int_pair={( void*) 0u, 0}; static unsigned char _temp0[ 8u]="*bogus*";
static struct _tagged_arr Cyc_Lex_bogus_string={ _temp0, _temp0, _temp0 +  8u};
static struct _tuple0 Cyc_Lex_token_id_pair={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Lex_bogus_string}; static unsigned char Cyc_Lex_token_char='\000'; static
unsigned char _temp1[ 1u]=""; static struct _tagged_arr Cyc_Lex_token_string={
_temp1, _temp1, _temp1 +  1u}; static struct _tuple5* Cyc_Lex_token_int=& Cyc_Lex_token_int_pair;
static struct _tuple0* Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair; static int
Cyc_Lex_runaway_start= 0; static void Cyc_Lex_err( struct _tagged_arr msg,
struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment* s= Cyc_Position_segment_of_abs(
Cyc_Lexing_lexeme_start( lb), Cyc_Lexing_lexeme_end( lb)); Cyc_Position_post_error(
Cyc_Position_mk_err_lex( s, msg));} static void Cyc_Lex_runaway_err( struct
_tagged_arr msg, struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment* s=
Cyc_Position_segment_of_abs( Cyc_Lex_runaway_start, Cyc_Lexing_lexeme_start( lb));
Cyc_Position_post_error( Cyc_Position_mk_err_lex( s, msg));} struct _tuple8{
struct _tagged_arr f1; short f2; } ; static unsigned char _temp2[ 14u]="__attribute__";
static unsigned char _temp3[ 9u]="abstract"; static unsigned char _temp4[ 5u]="auto";
static unsigned char _temp5[ 6u]="break"; static unsigned char _temp6[ 7u]="calloc";
static unsigned char _temp7[ 5u]="case"; static unsigned char _temp8[ 6u]="catch";
static unsigned char _temp9[ 5u]="char"; static unsigned char _temp10[ 8u]="codegen";
static unsigned char _temp11[ 6u]="const"; static unsigned char _temp12[ 9u]="continue";
static unsigned char _temp13[ 4u]="cut"; static unsigned char _temp14[ 8u]="default";
static unsigned char _temp15[ 3u]="do"; static unsigned char _temp16[ 7u]="double";
static unsigned char _temp17[ 5u]="else"; static unsigned char _temp18[ 5u]="enum";
static unsigned char _temp19[ 7u]="extern"; static unsigned char _temp20[ 9u]="fallthru";
static unsigned char _temp21[ 5u]="fill"; static unsigned char _temp22[ 6u]="float";
static unsigned char _temp23[ 4u]="for"; static unsigned char _temp24[ 9u]="forarray";
static unsigned char _temp25[ 6u]="__gen"; static unsigned char _temp26[ 5u]="goto";
static unsigned char _temp27[ 3u]="if"; static unsigned char _temp28[ 7u]="inline";
static unsigned char _temp29[ 4u]="int"; static unsigned char _temp30[ 4u]="let";
static unsigned char _temp31[ 5u]="long"; static unsigned char _temp32[ 7u]="malloc";
static unsigned char _temp33[ 10u]="namespace"; static unsigned char _temp34[ 4u]="new";
static unsigned char _temp35[ 5u]="NULL"; static unsigned char _temp36[ 9u]="offsetof";
static unsigned char _temp37[ 8u]="rcalloc"; static unsigned char _temp38[ 9u]="region_t";
static unsigned char _temp39[ 7u]="region"; static unsigned char _temp40[ 8u]="regions";
static unsigned char _temp41[ 9u]="register"; static unsigned char _temp42[ 9u]="restrict";
static unsigned char _temp43[ 7u]="return"; static unsigned char _temp44[ 8u]="rmalloc";
static unsigned char _temp45[ 5u]="rnew"; static unsigned char _temp46[ 6u]="short";
static unsigned char _temp47[ 7u]="signed"; static unsigned char _temp48[ 7u]="sizeof";
static unsigned char _temp49[ 9u]="sizeof_t"; static unsigned char _temp50[ 7u]="splice";
static unsigned char _temp51[ 7u]="static"; static unsigned char _temp52[ 7u]="struct";
static unsigned char _temp53[ 7u]="switch"; static unsigned char _temp54[ 6u]="throw";
static unsigned char _temp55[ 4u]="try"; static unsigned char _temp56[ 7u]="tunion";
static unsigned char _temp57[ 8u]="typedef"; static unsigned char _temp58[ 6u]="union";
static unsigned char _temp59[ 9u]="unsigned"; static unsigned char _temp60[ 6u]="using";
static unsigned char _temp61[ 5u]="void"; static unsigned char _temp62[ 9u]="volatile";
static unsigned char _temp63[ 6u]="while"; static unsigned char _temp64[ 8u]="xtunion";
static struct _tuple8 Cyc_Lex_rw_array[ 63u]={{{ _temp2, _temp2, _temp2 +  14u},
352},{{ _temp3, _temp3, _temp3 +  9u}, 299},{{ _temp4, _temp4, _temp4 +  5u},
258},{{ _temp5, _temp5, _temp5 +  6u}, 290},{{ _temp6, _temp6, _temp6 +  7u},
312},{{ _temp7, _temp7, _temp7 +  5u}, 277},{{ _temp8, _temp8, _temp8 +  6u},
297},{{ _temp9, _temp9, _temp9 +  5u}, 264},{{ _temp10, _temp10, _temp10 +  8u},
307},{{ _temp11, _temp11, _temp11 +  6u}, 272},{{ _temp12, _temp12, _temp12 +  9u},
289},{{ _temp13, _temp13, _temp13 +  4u}, 308},{{ _temp14, _temp14, _temp14 +  8u},
278},{{ _temp15, _temp15, _temp15 +  3u}, 286},{{ _temp16, _temp16, _temp16 +  7u},
269},{{ _temp17, _temp17, _temp17 +  5u}, 283},{{ _temp18, _temp18, _temp18 +  5u},
292},{{ _temp19, _temp19, _temp19 +  7u}, 261},{{ _temp20, _temp20, _temp20 +  9u},
300},{{ _temp21, _temp21, _temp21 +  5u}, 306},{{ _temp22, _temp22, _temp22 +  6u},
268},{{ _temp23, _temp23, _temp23 +  4u}, 287},{{ _temp24, _temp24, _temp24 +  9u},
305},{{ _temp25, _temp25, _temp25 +  6u}, 319},{{ _temp26, _temp26, _temp26 +  5u},
288},{{ _temp27, _temp27, _temp27 +  3u}, 282},{{ _temp28, _temp28, _temp28 +  7u},
279},{{ _temp29, _temp29, _temp29 +  4u}, 266},{{ _temp30, _temp30, _temp30 +  4u},
294},{{ _temp31, _temp31, _temp31 +  5u}, 267},{{ _temp32, _temp32, _temp32 +  7u},
310},{{ _temp33, _temp33, _temp33 +  10u}, 302},{{ _temp34, _temp34, _temp34 + 
4u}, 298},{{ _temp35, _temp35, _temp35 +  5u}, 293},{{ _temp36, _temp36, _temp36
+  9u}, 281},{{ _temp37, _temp37, _temp37 +  8u}, 313},{{ _temp38, _temp38,
_temp38 +  9u}, 314},{{ _temp39, _temp39, _temp39 +  7u}, 316},{{ _temp40,
_temp40, _temp40 +  8u}, 318},{{ _temp41, _temp41, _temp41 +  9u}, 259},{{
_temp42, _temp42, _temp42 +  9u}, 274},{{ _temp43, _temp43, _temp43 +  7u}, 291},{{
_temp44, _temp44, _temp44 +  8u}, 311},{{ _temp45, _temp45, _temp45 +  5u}, 317},{{
_temp46, _temp46, _temp46 +  6u}, 265},{{ _temp47, _temp47, _temp47 +  7u}, 270},{{
_temp48, _temp48, _temp48 +  7u}, 280},{{ _temp49, _temp49, _temp49 +  9u}, 315},{{
_temp50, _temp50, _temp50 +  7u}, 309},{{ _temp51, _temp51, _temp51 +  7u}, 260},{{
_temp52, _temp52, _temp52 +  7u}, 275},{{ _temp53, _temp53, _temp53 +  7u}, 284},{{
_temp54, _temp54, _temp54 +  6u}, 295},{{ _temp55, _temp55, _temp55 +  4u}, 296},{{
_temp56, _temp56, _temp56 +  7u}, 303},{{ _temp57, _temp57, _temp57 +  8u}, 262},{{
_temp58, _temp58, _temp58 +  6u}, 276},{{ _temp59, _temp59, _temp59 +  9u}, 271},{{
_temp60, _temp60, _temp60 +  6u}, 301},{{ _temp61, _temp61, _temp61 +  5u}, 263},{{
_temp62, _temp62, _temp62 +  9u}, 273},{{ _temp63, _temp63, _temp63 +  6u}, 285},{{
_temp64, _temp64, _temp64 +  8u}, 304}}; static int Cyc_Lex_trie_char( int c){
if( c >=  95){ return c -  59;} else{ if( c >  64){ return c -  55;}} return c - 
48;} static struct Cyc_Lex_Trie* Cyc_Lex_trie_lookup( struct Cyc_Lex_Trie* t,
struct _tagged_arr buff, int offset, int len){ int i= offset; int last=( offset
+  len) -  1; while( i <=  last) { void* _temp65=( void*)(( struct Cyc_Lex_Trie*)
_check_null( t))->children; struct Cyc_Lex_Trie** _temp73; struct Cyc_Lex_Trie*
_temp75; int _temp77; _LL67: if(( unsigned int) _temp65 >  1u?*(( int*) _temp65)
==  Cyc_Lex_Many: 0){ _LL74: _temp73=(( struct Cyc_Lex_Many_struct*) _temp65)->f1;
goto _LL68;} else{ goto _LL69;} _LL69: if(( unsigned int) _temp65 >  1u?*(( int*)
_temp65) ==  Cyc_Lex_One: 0){ _LL78: _temp77=(( struct Cyc_Lex_One_struct*)
_temp65)->f1; goto _LL76; _LL76: _temp75=(( struct Cyc_Lex_One_struct*) _temp65)->f2;
goto _LL70;} else{ goto _LL71;} _LL71: if( _temp65 == ( void*) Cyc_Lex_Zero){
goto _LL72;} else{ goto _LL66;} _LL68: { int ch= Cyc_Lex_trie_char(( int)((
const unsigned char*) buff.curr)[ i]); if( _temp73[ ch] ==  0){ _temp73[ ch]=({
struct Cyc_Lex_Trie* _temp79=( struct Cyc_Lex_Trie*) _cycalloc( sizeof( struct
Cyc_Lex_Trie)); _temp79->children=( void*)(( void*) Cyc_Lex_Zero); _temp79->shared_str=
0; _temp79;});} t= _temp73[ ch]; ++ i; goto _LL66;} _LL70: if( _temp77 == ((
const unsigned char*) buff.curr)[ i]){ t= _temp75;} else{ struct Cyc_Lex_Trie**
_temp80=({ unsigned int _temp85= 64u; struct Cyc_Lex_Trie** _temp86=( struct Cyc_Lex_Trie**)
_cycalloc( _check_times( sizeof( struct Cyc_Lex_Trie*), _temp85));{ unsigned int
_temp87= _temp85; unsigned int j; for( j= 0; j <  _temp87; j ++){ _temp86[ j]= 0;}};
_temp86;}); _temp80[ _check_known_subscript_notnull( 64u, Cyc_Lex_trie_char(
_temp77))]= _temp75;{ int _temp81= Cyc_Lex_trie_char(( int)(( const
unsigned char*) buff.curr)[ i]); _temp80[ _temp81]=({ struct Cyc_Lex_Trie*
_temp82=( struct Cyc_Lex_Trie*) _cycalloc( sizeof( struct Cyc_Lex_Trie));
_temp82->children=( void*)(( void*) Cyc_Lex_Zero); _temp82->shared_str= 0;
_temp82;});( void*)( t->children=( void*)(( void*)({ struct Cyc_Lex_Many_struct*
_temp83=( struct Cyc_Lex_Many_struct*) _cycalloc( sizeof( struct Cyc_Lex_Many_struct));
_temp83[ 0]=({ struct Cyc_Lex_Many_struct _temp84; _temp84.tag= Cyc_Lex_Many;
_temp84.f1= _temp80; _temp84;}); _temp83;}))); t= _temp80[ _temp81];}} ++ i;
goto _LL66; _LL72: while( i <=  last) { struct Cyc_Lex_Trie* _temp88=({ struct
Cyc_Lex_Trie* _temp91=( struct Cyc_Lex_Trie*) _cycalloc( sizeof( struct Cyc_Lex_Trie));
_temp91->children=( void*)(( void*) Cyc_Lex_Zero); _temp91->shared_str= 0;
_temp91;});( void*)((( struct Cyc_Lex_Trie*) _check_null( t))->children=( void*)((
void*)({ struct Cyc_Lex_One_struct* _temp89=( struct Cyc_Lex_One_struct*)
_cycalloc( sizeof( struct Cyc_Lex_One_struct)); _temp89[ 0]=({ struct Cyc_Lex_One_struct
_temp90; _temp90.tag= Cyc_Lex_One; _temp90.f1=( int)*(( const unsigned char*)
_check_unknown_subscript( buff, sizeof( unsigned char), i ++)); _temp90.f2=
_temp88; _temp90;}); _temp89;}))); t= _temp88;} return t; _LL66:;} return t;}
static int Cyc_Lex_str_index( struct _tagged_arr buff, int offset, int len){
struct Cyc_Lex_Trie* _temp92= Cyc_Lex_trie_lookup( Cyc_Lex_ids_trie, buff,
offset, len); if( _temp92->shared_str ==  0){ struct _tagged_arr _temp93= Cyc_Core_new_string((
unsigned int)( len +  1)); Cyc_Std_zstrncpy( _temp93, _tagged_arr_plus( buff,
sizeof( unsigned char), offset),( unsigned int) len);{ int ans=(( int(*)( struct
Cyc_Xarray_Xarray*, struct _tagged_arr*)) Cyc_Xarray_add_ind)(( struct Cyc_Xarray_Xarray*)
_check_null( Cyc_Lex_symbols),({ struct _tagged_arr* _temp94=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp94[ 0]=( struct
_tagged_arr) _temp93; _temp94;})); _temp92->shared_str= ans;}} return _temp92->shared_str;}
static int Cyc_Lex_str_index_lbuf( struct Cyc_Lexing_lexbuf* lbuf){ return Cyc_Lex_str_index((
struct _tagged_arr) lbuf->lex_buffer, lbuf->lex_start_pos, lbuf->lex_curr_pos - 
lbuf->lex_start_pos);} static void Cyc_Lex_insert_typedef( struct _tagged_arr*
sptr){ struct _tagged_arr _temp95=* sptr; struct Cyc_Lex_Trie* _temp96= Cyc_Lex_trie_lookup(
Cyc_Lex_typedefs_trie, _temp95, 0,( int)( _get_arr_size( _temp95, sizeof(
unsigned char)) -  1));(( struct Cyc_Lex_Trie*) _check_null( _temp96))->shared_str=
1; return;} static struct _tagged_arr* Cyc_Lex_get_symbol( int symbol_num){
return(( struct _tagged_arr*(*)( struct Cyc_Xarray_Xarray*, int)) Cyc_Xarray_get)((
struct Cyc_Xarray_Xarray*) _check_null( Cyc_Lex_symbols), symbol_num);} static
int Cyc_Lex_int_of_char( unsigned char c){ if('0' <=  c? c <= '9': 0){ return c
- '0';} else{ if('a' <=  c? c <= 'f': 0){ return( 10 +  c) - 'a';} else{ if('A'
<=  c? c <= 'F': 0){ return( 10 +  c) - 'A';} else{( int) _throw(( void*)({
struct Cyc_Core_Invalid_argument_struct* _temp97=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp97[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp98; _temp98.tag= Cyc_Core_Invalid_argument;
_temp98.f1= _tag_arr("string to integer conversion", sizeof( unsigned char), 29u);
_temp98;}); _temp97;}));}}}} static struct _tuple5* Cyc_Lex_intconst( struct Cyc_Lexing_lexbuf*
lbuf, int start, int end, int base){ unsigned int n= 0; int end2= lbuf->lex_curr_pos
-  end; struct _tagged_arr buff= lbuf->lex_buffer; int i= start +  lbuf->lex_start_pos;{
int i= start +  lbuf->lex_start_pos; for( 0; i <  end2; ++ i){ unsigned char c=((
unsigned char*) buff.curr)[ i]; switch( c){ case 'u': _LL99: goto _LL100; case 'U':
_LL100: return({ struct _tuple5* _temp102=( struct _tuple5*) _cycalloc( sizeof(
struct _tuple5)); _temp102->f1=( void*) Cyc_Absyn_Unsigned; _temp102->f2=( int)
n; _temp102;}); case 'l': _LL101: break; case 'L': _LL103: break; default:
_LL104: n= n *  base + ( unsigned int) Cyc_Lex_int_of_char( c); break;}}} return({
struct _tuple5* _temp106=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp106->f1=( void*) Cyc_Absyn_Signed; _temp106->f2=( int) n; _temp106;});}
unsigned char Cyc_Lex_string_buffer_v[ 10u]={'x','x','x','x','x','x','x','x','x','x'};
struct _tagged_arr Cyc_Lex_string_buffer={( void*) Cyc_Lex_string_buffer_v,(
void*) Cyc_Lex_string_buffer_v,( void*)( Cyc_Lex_string_buffer_v +  10u)}; int
Cyc_Lex_string_pos= 0; void Cyc_Lex_store_string_char( unsigned char c){ int sz=(
int) _get_arr_size( Cyc_Lex_string_buffer, sizeof( unsigned char)); if( Cyc_Lex_string_pos
>=  sz){ int newsz= sz; while( Cyc_Lex_string_pos >=  newsz) { newsz= newsz *  2;}{
struct _tagged_arr str=({ unsigned int _temp107=( unsigned int) newsz;
unsigned char* _temp108=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp107)); struct _tagged_arr _temp110= _tag_arr(
_temp108, sizeof( unsigned char),( unsigned int) newsz);{ unsigned int _temp109=
_temp107; unsigned int i; for( i= 0; i <  _temp109; i ++){ _temp108[ i]= i <  sz?*((
unsigned char*) _check_unknown_subscript( Cyc_Lex_string_buffer, sizeof(
unsigned char),( int) i)):'\000';}}; _temp110;}); Cyc_Lex_string_buffer= str;}}*((
unsigned char*) _check_unknown_subscript( Cyc_Lex_string_buffer, sizeof(
unsigned char), Cyc_Lex_string_pos))= c; ++ Cyc_Lex_string_pos;} struct
_tagged_arr Cyc_Lex_get_stored_string(){ struct _tagged_arr str= Cyc_Std_substring((
struct _tagged_arr) Cyc_Lex_string_buffer, 0,( unsigned int) Cyc_Lex_string_pos);
Cyc_Lex_string_pos= 0; return str;} struct Cyc_Lex_Ldecls{ struct Cyc_Set_Set*
typedefs; struct Cyc_Set_Set* namespaces; } ; struct Cyc_Lex_Lvis{ struct Cyc_List_List*
current_namespace; struct Cyc_List_List* imported_namespaces; } ; struct Cyc_Lex_Lstate{
struct Cyc_List_List* lstack; struct Cyc_Dict_Dict* decls; } ; static struct Cyc_Core_Opt*
Cyc_Lex_lstate= 0; static void Cyc_Lex_typedef_init(){ struct Cyc_Lex_Lvis*
_temp111=({ struct Cyc_Lex_Lvis* _temp117=( struct Cyc_Lex_Lvis*) _cycalloc(
sizeof( struct Cyc_Lex_Lvis)); _temp117->current_namespace= 0; _temp117->imported_namespaces=
0; _temp117;}); struct Cyc_List_List* _temp112=({ struct Cyc_List_List* _temp116=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp116->hd=(
void*) _temp111; _temp116->tl= 0; _temp116;}); struct Cyc_Dict_Dict* init_decls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* k,
struct Cyc_Lex_Ldecls* v)) Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(*
cmp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp),
0,({ struct Cyc_Lex_Ldecls* _temp115=( struct Cyc_Lex_Ldecls*) _cycalloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp115->typedefs=(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_zstrptrcmp);
_temp115->namespaces=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_zstrptrcmp); _temp115;})); Cyc_Lex_lstate=({
struct Cyc_Core_Opt* _temp113=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp113->v=( void*)({ struct Cyc_Lex_Lstate* _temp114=( struct
Cyc_Lex_Lstate*) _cycalloc( sizeof( struct Cyc_Lex_Lstate)); _temp114->lstack=
_temp112; _temp114->decls= init_decls; _temp114;}); _temp113;});} static struct
Cyc_List_List* Cyc_Lex_get_absolute_namespace( struct Cyc_List_List* ns){ struct
_tagged_arr* n=( struct _tagged_arr*) ns->hd;{ struct Cyc_List_List* ls=( struct
Cyc_List_List*)(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Lex_lstate))->v)->lstack; for( 0; ls !=  0; ls= ls->tl){ struct Cyc_Lex_Lvis*
lv=( struct Cyc_Lex_Lvis*) ls->hd; struct Cyc_List_List* x=({ struct Cyc_List_List*
_temp118=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp118->hd=( void*) lv->current_namespace; _temp118->tl= lv->imported_namespaces;
_temp118;}); for( 0; x !=  0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* k)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( x))->hd); if((( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( ld->namespaces, n)){ return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( x))->hd,( struct
Cyc_List_List*) ns);}}}} Cyc_yyerror(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp120; _temp120.tag= Cyc_Std_String_pa; _temp120.f1=( struct _tagged_arr) Cyc_Std_str_sepstr((
struct Cyc_List_List*) ns, _tag_arr("::", sizeof( unsigned char), 3u));{ void*
_temp119[ 1u]={& _temp120}; Cyc_Std_aprintf( _tag_arr("undeclared namespace %s",
sizeof( unsigned char), 24u), _tag_arr( _temp119, sizeof( void*), 1u));}}));
return 0;} static void Cyc_Lex_recompute_typedefs(){ Cyc_Lex_typedefs_trie=({
struct Cyc_Lex_Trie* _temp121=( struct Cyc_Lex_Trie*) _cycalloc( sizeof( struct
Cyc_Lex_Trie)); _temp121->children=( void*)(( void*) Cyc_Lex_Zero); _temp121->shared_str=
0; _temp121;});{ struct Cyc_List_List* ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack; for( 0; ls != 
0; ls= ls->tl){ struct Cyc_Lex_Lvis* lv=( struct Cyc_Lex_Lvis*) ls->hd; struct
Cyc_List_List* x=({ struct Cyc_List_List* _temp122=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp122->hd=( void*) lv->current_namespace;
_temp122->tl= lv->imported_namespaces; _temp122;}); for( 0; x !=  0; x=(( struct
Cyc_List_List*) _check_null( x))->tl){ struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)((( struct
Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,(
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( x))->hd);(( void(*)(
void(* f)( struct _tagged_arr*), struct Cyc_Set_Set* s)) Cyc_Set_iter)( Cyc_Lex_insert_typedef,
ld->typedefs);}}}} static int Cyc_Lex_is_typedef_in_namespace( struct Cyc_List_List*
ns, struct _tagged_arr* v){ struct Cyc_List_List* ans= Cyc_Lex_get_absolute_namespace(
ns); struct _handler_cons _temp123; _push_handler(& _temp123);{ int _temp125= 0;
if( setjmp( _temp123.handler)){ _temp125= 1;} if( ! _temp125){{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->decls, ans); int _temp126=(( int(*)( struct
Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)( ld->typedefs, v);
_npop_handler( 0u); return _temp126;}; _pop_handler();} else{ void* _temp124=(
void*) _exn_thrown; void* _temp128= _temp124; _LL130: if( _temp128 ==  Cyc_Dict_Absent){
goto _LL131;} else{ goto _LL132;} _LL132: goto _LL133; _LL131: return 0; _LL133:(
void) _throw( _temp128); _LL129:;}}} static int Cyc_Lex_is_typedef( struct Cyc_List_List*
ns, struct _tagged_arr* v){ if( ns !=  0){ return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*) _check_null( ns), v);}{ struct _tagged_arr _temp134=* v;
int len=( int)( _get_arr_size( _temp134, sizeof( unsigned char)) -  1); struct
Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i= 0; for( 0; i <  len; ++ i){ void*
_temp135=( void*)(( struct Cyc_Lex_Trie*) _check_null( t))->children; struct Cyc_Lex_Trie*
_temp146; int _temp148; struct Cyc_Lex_Trie* _temp150; struct Cyc_Lex_Trie**
_temp152; _LL137: if( _temp135 == ( void*) Cyc_Lex_Zero){ goto _LL138;} else{
goto _LL139;} _LL139: if(( unsigned int) _temp135 >  1u?*(( int*) _temp135) == 
Cyc_Lex_One: 0){ _LL149: _temp148=(( struct Cyc_Lex_One_struct*) _temp135)->f1;
goto _LL147; _LL147: _temp146=(( struct Cyc_Lex_One_struct*) _temp135)->f2; goto
_LL145;} else{ goto _LL141;} _LL145: if( _temp148 != (( const unsigned char*)
_temp134.curr)[ i]){ goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int)
_temp135 >  1u?*(( int*) _temp135) ==  Cyc_Lex_One: 0){ _LL151: _temp150=((
struct Cyc_Lex_One_struct*) _temp135)->f2; goto _LL142;} else{ goto _LL143;}
_LL143: if(( unsigned int) _temp135 >  1u?*(( int*) _temp135) ==  Cyc_Lex_Many:
0){ _LL153: _temp152=(( struct Cyc_Lex_Many_struct*) _temp135)->f1; goto _LL144;}
else{ goto _LL136;} _LL138: return 0; _LL140: return 0; _LL142: t= _temp150;
goto _LL136; _LL144: { struct Cyc_Lex_Trie* _temp154= _temp152[
_check_known_subscript_notnull( 64u, Cyc_Lex_trie_char(( int)(( const
unsigned char*) _temp134.curr)[ i]))]; if( _temp154 ==  0){ return 0;} t=
_temp154; goto _LL136;} _LL136:;}} return(( struct Cyc_Lex_Trie*) _check_null( t))->shared_str;}}
void Cyc_Lex_enter_namespace( struct _tagged_arr* s){ struct Cyc_List_List* ns=((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace; struct Cyc_List_List*
new_ns=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( ns,({ struct Cyc_List_List* _temp159=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp159->hd=( void*) s; _temp159->tl=
0; _temp159;}));(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Lex_lstate))->v)->lstack=({ struct Cyc_List_List* _temp155=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp155->hd=( void*)({ struct Cyc_Lex_Lvis*
_temp156=( struct Cyc_Lex_Lvis*) _cycalloc( sizeof( struct Cyc_Lex_Lvis));
_temp156->current_namespace= new_ns; _temp156->imported_namespaces= 0; _temp156;});
_temp155->tl=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack; _temp155;});{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
k)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->decls, ns); if( !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( ld->namespaces, s)){(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k, struct Cyc_Lex_Ldecls* v)) Cyc_Dict_insert)(((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,
ns,({ struct Cyc_Lex_Ldecls* _temp157=( struct Cyc_Lex_Ldecls*) _cycalloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp157->typedefs= ld->typedefs; _temp157->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ld->namespaces, s); _temp157;}));(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->decls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k, struct Cyc_Lex_Ldecls* v)) Cyc_Dict_insert)(((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,
new_ns,({ struct Cyc_Lex_Ldecls* _temp158=( struct Cyc_Lex_Ldecls*) _cycalloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp158->typedefs=(( struct Cyc_Set_Set*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_zstrptrcmp);
_temp158->namespaces=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_Std_zstrptrcmp); _temp158;}));}((
void(*)( void(* f)( struct _tagged_arr*), struct Cyc_Set_Set* s)) Cyc_Set_iter)(
Cyc_Lex_insert_typedef,((( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* k)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls, new_ns))->typedefs);}}
void Cyc_Lex_leave_namespace(){(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack=( struct Cyc_List_List*) _check_null((((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();} void Cyc_Lex_enter_using( struct _tuple0* q){
struct Cyc_List_List* ns;{ void* _temp160=(* q).f1; struct Cyc_List_List*
_temp168; struct Cyc_List_List* _temp170; _LL162: if( _temp160 == ( void*) Cyc_Absyn_Loc_n){
goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int) _temp160 >  1u?*((
int*) _temp160) ==  Cyc_Absyn_Rel_n: 0){ _LL169: _temp168=(( struct Cyc_Absyn_Rel_n_struct*)
_temp160)->f1; goto _LL165;} else{ goto _LL166;} _LL166: if(( unsigned int)
_temp160 >  1u?*(( int*) _temp160) ==  Cyc_Absyn_Abs_n: 0){ _LL171: _temp170=((
struct Cyc_Absyn_Abs_n_struct*) _temp160)->f1; goto _LL167;} else{ goto _LL161;}
_LL163: ns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp172=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp172->hd=( void*)(*
q).f2; _temp172->tl= 0; _temp172;}); goto _LL161; _LL165: _temp170= _temp168;
goto _LL167; _LL167: ns=( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp170,({
struct Cyc_List_List* _temp173=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp173->hd=( void*)(* q).f2; _temp173->tl= 0; _temp173;})));
goto _LL161; _LL161:;}{ struct Cyc_List_List* _temp174= Cyc_Lex_get_absolute_namespace(
ns); struct Cyc_List_List* _temp175=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=({ struct
Cyc_List_List* _temp176=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp176->hd=( void*) _temp174; _temp176->tl= _temp175; _temp176;});(( void(*)(
void(* f)( struct _tagged_arr*), struct Cyc_Set_Set* s)) Cyc_Set_iter)( Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* k))
Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Lex_lstate))->v)->decls, _temp174))->typedefs);}} void Cyc_Lex_leave_using(){
struct Cyc_List_List* _temp177=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=(( struct
Cyc_List_List*) _check_null( _temp177))->tl; Cyc_Lex_recompute_typedefs();} void
Cyc_Lex_register_typedef( struct _tuple0* q){ struct Cyc_List_List* _temp178=((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace; struct Cyc_Dict_Dict*
_temp179=(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls;
struct Cyc_Lex_Ldecls* _temp180=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( _temp179, _temp178); struct Cyc_Lex_Ldecls*
_temp181=({ struct Cyc_Lex_Ldecls* _temp182=( struct Cyc_Lex_Ldecls*) _cycalloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp182->typedefs=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)( _temp180->typedefs,(*
q).f2); _temp182->namespaces= _temp180->namespaces; _temp182;});(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k, struct Cyc_Lex_Ldecls* v)) Cyc_Dict_insert)(
_temp179, _temp178, _temp181); Cyc_Lex_insert_typedef((* q).f2);} static short
Cyc_Lex_process_id( struct Cyc_Lexing_lexbuf* lbuf){ int symbol_num= Cyc_Lex_str_index_lbuf(
lbuf); if( symbol_num <=  Cyc_Lex_num_kws){ return( short)*(( int*)
_check_unknown_subscript( Cyc_Lex_kw_nums, sizeof( int), symbol_num -  1));}{
struct _tagged_arr* _temp183= Cyc_Lex_get_symbol( symbol_num); if( Cyc_Lex_is_typedef(
0, _temp183)){ Cyc_Lex_token_qvar=({ struct _tuple0* _temp184=( struct _tuple0*)
_cycalloc( sizeof( struct _tuple0)); _temp184->f1= Cyc_Absyn_rel_ns_null;
_temp184->f2= _temp183; _temp184;}); return 351;} Cyc_Lex_token_string=*
_temp183; return 344;}} static short Cyc_Lex_process_qual_id( struct Cyc_Lexing_lexbuf*
lbuf){ int i= lbuf->lex_start_pos; int end= lbuf->lex_curr_pos; struct
_tagged_arr s= lbuf->lex_buffer; struct Cyc_List_List* rev_vs= 0; while( i < 
end) { int start= i; for( 0; i <  end?(( unsigned char*) s.curr)[ i] != ':': 0;
i ++){;} if( start ==  i){( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp185=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp185[ 0]=({ struct Cyc_Core_Impossible_struct _temp186; _temp186.tag= Cyc_Core_Impossible;
_temp186.f1= _tag_arr("bad namespace", sizeof( unsigned char), 14u); _temp186;});
_temp185;}));}{ int vlen= i -  start; struct _tagged_arr* v= Cyc_Lex_get_symbol(
Cyc_Lex_str_index(( struct _tagged_arr) s, start, vlen)); rev_vs=({ struct Cyc_List_List*
_temp187=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp187->hd=( void*) v; _temp187->tl= rev_vs; _temp187;}); i += 2;}} if( rev_vs
==  0){( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp188=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp188[ 0]=({ struct Cyc_Core_Impossible_struct _temp189; _temp189.tag= Cyc_Core_Impossible;
_temp189.f1= _tag_arr("bad namespace", sizeof( unsigned char), 14u); _temp189;});
_temp188;}));}{ struct _tagged_arr* v=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( rev_vs))->hd; struct Cyc_List_List* vs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( rev_vs->tl); Cyc_Lex_token_qvar=({
struct _tuple0* _temp190=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp190->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct* _temp191=( struct Cyc_Absyn_Rel_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp191[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp192; _temp192.tag= Cyc_Absyn_Rel_n; _temp192.f1= vs; _temp192;}); _temp191;});
_temp190->f2= v; _temp190;}); if( Cyc_Lex_is_typedef( vs, v)){ return 351;}
else{ return 350;}}} extern int Cyc_Lex_token( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*); int Cyc_yylex(){ int ans=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Parse_lbuf))->v); Cyc_yylloc.first_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v); Cyc_yylloc.last_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_end)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v); switch( ans){ case 344: _LL193: Cyc_yylval=(
void*)({ struct Cyc_String_tok_struct* _temp195=( struct Cyc_String_tok_struct*)
_cycalloc( sizeof( struct Cyc_String_tok_struct)); _temp195[ 0]=({ struct Cyc_String_tok_struct
_temp196; _temp196.tag= Cyc_String_tok; _temp196.f1= Cyc_Lex_token_string;
_temp196;}); _temp195;}); break; case 350: _LL194: Cyc_yylval=( void*)({ struct
Cyc_QualId_tok_struct* _temp198=( struct Cyc_QualId_tok_struct*) _cycalloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp198[ 0]=({ struct Cyc_QualId_tok_struct
_temp199; _temp199.tag= Cyc_QualId_tok; _temp199.f1= Cyc_Lex_token_qvar;
_temp199;}); _temp198;}); break; case 351: _LL197: Cyc_yylval=( void*)({ struct
Cyc_QualId_tok_struct* _temp201=( struct Cyc_QualId_tok_struct*) _cycalloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp201[ 0]=({ struct Cyc_QualId_tok_struct
_temp202; _temp202.tag= Cyc_QualId_tok; _temp202.f1= Cyc_Lex_token_qvar;
_temp202;}); _temp201;}); break; case 349: _LL200: Cyc_yylval=( void*)({ struct
Cyc_String_tok_struct* _temp204=( struct Cyc_String_tok_struct*) _cycalloc(
sizeof( struct Cyc_String_tok_struct)); _temp204[ 0]=({ struct Cyc_String_tok_struct
_temp205; _temp205.tag= Cyc_String_tok; _temp205.f1= Cyc_Lex_token_string;
_temp205;}); _temp204;}); break; case 345: _LL203: Cyc_yylval=( void*)({ struct
Cyc_Int_tok_struct* _temp207=( struct Cyc_Int_tok_struct*) _cycalloc( sizeof(
struct Cyc_Int_tok_struct)); _temp207[ 0]=({ struct Cyc_Int_tok_struct _temp208;
_temp208.tag= Cyc_Int_tok; _temp208.f1= Cyc_Lex_token_int; _temp208;}); _temp207;});
break; case 347: _LL206: Cyc_yylval=( void*)({ struct Cyc_Char_tok_struct*
_temp210=( struct Cyc_Char_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Char_tok_struct));
_temp210[ 0]=({ struct Cyc_Char_tok_struct _temp211; _temp211.tag= Cyc_Char_tok;
_temp211.f1= Cyc_Lex_token_char; _temp211;}); _temp210;}); break; case 348:
_LL209: Cyc_yylval=( void*)({ struct Cyc_String_tok_struct* _temp213=( struct
Cyc_String_tok_struct*) _cycalloc( sizeof( struct Cyc_String_tok_struct));
_temp213[ 0]=({ struct Cyc_String_tok_struct _temp214; _temp214.tag= Cyc_String_tok;
_temp214.f1= Cyc_Lex_token_string; _temp214;}); _temp213;}); break; case 346:
_LL212: Cyc_yylval=( void*)({ struct Cyc_String_tok_struct* _temp216=( struct
Cyc_String_tok_struct*) _cycalloc( sizeof( struct Cyc_String_tok_struct));
_temp216[ 0]=({ struct Cyc_String_tok_struct _temp217; _temp217.tag= Cyc_String_tok;
_temp217.f1= Cyc_Lex_token_string; _temp217;}); _temp216;}); break; default:
_LL215: break;} return ans;} const int Cyc_Lex_lex_base[ 149u]=( const int[ 149u]){
0, 113, 17, 83, 16, 2, - 3, - 1, - 2, - 17, - 18, 4, 118, 119, - 19, 5, - 13, -
12, 85, - 14, - 11, - 4, - 5, - 6, - 7, - 8, - 9, - 10, 192, 215, 111, - 15, 166,
- 18, - 55, 8, 30, - 39, 18, 31, 116, 139, 32, 138, 135, 137, 159, 250, 325, 83,
82, 84, 94, 395, 85, 470, 545, 90, - 54, - 22, - 28, 620, 695, 90, 753, 828, 99,
- 23, 903, 123, - 26, 134, - 31, - 25, - 34, 978, 1007, 363, 117, 128, 292, 1017,
1047, 1083, 335, 1108, 1120, 1190, 133, 143, 1260, 1298, 137, 147, 191, 201, 193,
203, - 6, - 38, 199, - 20, - 37, 6, 157, 1230, - 33, - 16, - 32, - 17, 2, 1338,
168, 169, 279, 171, 174, 176, 185, 186, 187, 188, 189, 191, 1411, 1495, - 52, -
46, - 45, - 44, - 43, - 42, - 41, - 40, - 47, - 50, - 51, 311, 193, - 48, - 49,
- 53, - 27, - 24, - 21, 310, - 35, 12, 425}; const int Cyc_Lex_lex_backtrk[ 149u]=(
const int[ 149u]){ - 1, - 1, - 1, 5, 3, 4, - 1, - 1, - 1, - 1, - 1, 16, 1, 19, -
1, 2, - 1, - 1, 14, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 15, 14, -
1, - 1, - 1, - 1, 35, 54, - 1, 54, 54, 54, 54, 54, 54, 54, 54, 54, 5, 7, 54, 54,
54, 54, 0, 54, 54, 54, 54, - 1, - 1, - 1, 3, 1, - 1, - 1, 2, - 1, - 1, 0, 29, -
1, 28, - 1, - 1, - 1, 9, 7, - 1, 7, 7, - 1, 8, 9, - 1, - 1, 9, 5, 6, 5, 5, - 1,
4, 4, 4, 6, 6, 5, 5, - 1, - 1, - 1, - 1, - 1, 36, - 1, 9, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 34, 35}; const int Cyc_Lex_lex_default[ 149u]=( const int[
149u]){ 34, 9, 3, 3, - 1, - 1, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1, - 1, 0, 145, - 1, -
1, 110, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, 0, - 1, 0, 0, 0, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 100, 0, 0, - 1, - 1, - 1, 0, 0, 0, 0, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, 0, 0, 0, 0, 0, 0, 145, 0, - 1, - 1}; const int
Cyc_Lex_lex_trans[ 1752u]=( const int[ 1752u]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 35,
35, 35, 35, 35, 33, 6, 102, 148, 148, 148, 148, 148, 146, 0, 0, 0, 0, 0, 146, 0,
0, 147, 35, 36, 37, 38, 0, 39, 40, 41, 148, 141, 42, 43, 7, 44, 45, 46, 47, 48,
48, 48, 48, 48, 48, 48, 48, 48, 49, 4, 50, 51, 52, 8, 5, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
17, 144, 14, 54, 55, 56, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 10, 57, - 1, 11, 32, 32, 6, - 1,
32, 15, 30, 30, 30, 30, 30, 30, 30, 30, 74, 71, 16, 72, 20, 67, 12, 64, - 1, 32,
59, 7, 16, 142, 19, 69, 64, 17, 31, 31, 31, 31, 31, 31, 31, 31, 18, 18, 18, 18,
18, 18, 18, 18, 32, 32, 143, - 1, 32, 107, 31, 19, 104, 70, 105, 105, 105, 105,
105, 105, 105, 105, 105, 105, 73, 33, 108, 32, 109, 7, 99, 25, 106, 25, 13, 100,
140, 139, 102, 135, 20, 103, 134, 60, 133, 21, 22, 23, 23, 101, 23, 22, 22, 132,
131, 130, 129, 128, 24, 127, 111, 136, 25, 25, 26, 25, 27, 0, 28, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 23, 23, 0, 0, 22, 22, 58, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 6, - 1, 0, 24, 24, 98, 98, 29, 29, 29, 29, 29,
29, 0, 0, 0, 29, 29, 29, 29, 29, 29, 0, 75, 0, 86, 86, 86, 86, 86, 86, 86, 86,
87, 87, 24, 24, 98, 98, 29, 29, 29, 29, 29, 29, 136, 77, 146, 0, 0, 147, 0, 0,
88, 137, 137, 137, 137, 137, 137, 137, 137, 89, 0, 0, 90, - 1, 81, 81, 81, 81,
81, 81, 81, 81, 81, 81, 136, 77, 0, 0, 0, 0, 0, 0, 88, 138, 138, 138, 138, 138,
138, 138, 138, 89, 0, 14, 90, 75, 0, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 85,
85, 85, 85, 85, 85, 85, 85, 85, 85, 0, 77, - 1, 0, 0, 0, 0, 0, 78, 0, 0, 0, 0,
80, 0, 80, 0, 79, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 0, 0, 0, 0, 0, 77, 0,
0, 0, 0, 0, 0, 78, 148, 148, 148, 148, 148, 0, 0, 0, 79, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 63, 0, - 1, 0, 148, 0, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 0, 0, 0, 0, 68,
0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 63, 0, 0, 0,
0, 0, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0, 62, - 1, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 0, 0, 0, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
0, 0, 0, 0, 61, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 0, 0, 0, 0, 0, 0, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 61, 0, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 63, 0, 0, 0, 0, 0, 0, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 0, 0, 0, 0, 62, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 0, 0, 0, 0, 65, 0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 66, 0, 0, 0, 0, 0, 0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 0, 0, 0, 0, 65, 0, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 63, 0, 0, 0, 0, 0, 0, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 0, 0, 0, 0, 68, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 82, 82, 82, 82,
82, 82, 82, 82, 82, 82, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 27, 0, 0, 0, 0, 75,
27, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 81, 81, 81, 81, 81, 81, 81, 81, 81,
81, 0, 77, 0, 0, 83, 27, 0, 0, 78, 0, 0, 27, 26, 0, 0, 0, 0, 79, 26, 0, 82, 82,
82, 82, 82, 82, 82, 82, 82, 82, 0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 78, 83, 27, 0, 26,
0, 0, 0, 27, 79, 26, 84, 0, 84, 0, 0, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 0,
0, 0, 0, 0, 0, 0, 83, 27, 0, 0, 0, 0, 0, 27, 85, 85, 85, 85, 85, 85, 85, 85, 85,
85, 75, 0, 86, 86, 86, 86, 86, 86, 86, 86, 87, 87, 27, 0, 0, 0, 0, 0, 27, 0, 0,
0, 0, 77, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 97, 0, 0, 0, 0, 27, 0, 0,
0, 0, 0, 27, 0, 0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 75, 97,
87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 77, 0,
0, 0, 0, 0, 0, 94, 0, 0, 0, 0, 0, 0, 0, 0, 95, 0, 0, 105, 105, 105, 105, 105,
105, 105, 105, 105, 105, 0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 94, 83, 27, 0, 0, 0, 0,
0, 27, 95, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0, 0, 0, 0, 0, 0, 91, 91,
91, 91, 91, 91, 83, 27, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 91, 91, 91, 91,
91, 91, 91, 91, 91, 91, 0, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0,
0, 112, 0, 92, 0, 0, 113, 0, 0, 0, 0, 0, 93, 0, 0, 114, 114, 114, 114, 114, 114,
114, 114, 0, 91, 91, 91, 91, 91, 91, 115, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 0, 0,
93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 116, 0, 0, 0, 0, 117, 118, 0, 0, 0,
119, 0, 0, 0, 0, 0, 0, 0, 120, 0, 0, 0, 121, 0, 122, 0, 123, 0, 124, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 0, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 126, 0, 0, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 0, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 0, 0, 0, 0, 0, 0, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int Cyc_Lex_lex_check[ 1752u]=( const int[ 1752u]){ - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 0, 0, 0, 0, 0, 11, 15, 103, 35, 35, 35, 35, 35, 147, - 1,
- 1, - 1, - 1, - 1, 38, - 1, - 1, 38, 0, 0, 0, 0, - 1, 0, 0, 0, 35, 110, 0, 0, 5,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 42, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
3, 1, 12, 12, 13, 3, 12, 13, 18, 18, 18, 18, 18, 18, 18, 18, 49, 50, 50, 50, 51,
54, 1, 63, 41, 12, 57, 12, 13, 40, 52, 52, 66, 13, 30, 30, 30, 30, 30, 30, 30,
30, 13, 13, 13, 13, 13, 13, 13, 13, 32, 32, 40, 41, 32, 44, 43, 13, 45, 69, 45,
45, 45, 45, 45, 45, 45, 45, 45, 45, 71, 44, 44, 32, 43, 32, 46, 78, 104, 79, 1,
46, 112, 113, 100, 115, 13, 100, 116, 57, 117, 13, 13, 88, 89, 46, 13, 92, 93,
118, 119, 120, 121, 122, 13, 123, 41, 138, 13, 78, 13, 79, 13, - 1, 13, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 88, 89, - 1, - 1, 92, 93, 0, 28, 28, 28, 28, 28,
28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 2, 38, - 1, 94, 95, 96, 97, 29, 29,
29, 29, 29, 29, - 1, - 1, - 1, 28, 28, 28, 28, 28, 28, - 1, 47, - 1, 47, 47, 47,
47, 47, 47, 47, 47, 47, 47, 94, 95, 96, 97, 29, 29, 29, 29, 29, 29, 114, 47, 145,
- 1, - 1, 145, - 1, - 1, 47, 114, 114, 114, 114, 114, 114, 114, 114, 47, - 1, -
1, 47, 3, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 137, 47, - 1, - 1, - 1, - 1, -
1, - 1, 47, 137, 137, 137, 137, 137, 137, 137, 137, 47, - 1, 1, 47, 48, - 1, 48,
48, 48, 48, 48, 48, 48, 48, 48, 48, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, - 1,
48, 41, - 1, - 1, - 1, - 1, - 1, 48, - 1, - 1, - 1, - 1, 77, - 1, 77, - 1, 48,
77, 77, 77, 77, 77, 77, 77, 77, 77, 77, - 1, - 1, - 1, - 1, - 1, 48, - 1, - 1, -
1, - 1, - 1, - 1, 48, 148, 148, 148, 148, 148, - 1, - 1, - 1, 48, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, - 1, 100, - 1, 148, - 1, - 1, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
- 1, - 1, - 1, - 1, 53, - 1, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, - 1, - 1, - 1, - 1, - 1, - 1, 55, 55, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, - 1, - 1, -
1, - 1, 55, 145, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, - 1, - 1, - 1, - 1,
56, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, - 1, - 1, - 1, - 1, 61, - 1, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, - 1, - 1, - 1, -
1, - 1, - 1, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, - 1, - 1, - 1, - 1, 62, - 1, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, - 1, - 1, - 1, - 1, 64, - 1, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, - 1, - 1, - 1, - 1, - 1, - 1, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, - 1, - 1, - 1, - 1, 65, - 1, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, - 1, - 1, - 1, - 1, - 1, - 1, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, - 1, - 1, - 1, - 1, 68, - 1, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 75, 75, 75, 75, 75,
75, 75, 75, 75, 75, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 75,
75, - 1, - 1, - 1, - 1, 76, 75, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 81, 81,
81, 81, 81, 81, 81, 81, 81, 81, - 1, 76, - 1, - 1, 75, 75, - 1, - 1, 76, - 1, -
1, 75, 81, - 1, - 1, - 1, - 1, 76, 81, - 1, 82, 82, 82, 82, 82, 82, 82, 82, 82,
82, - 1, - 1, - 1, 76, - 1, - 1, - 1, - 1, - 1, - 1, 76, 82, 82, - 1, 81, - 1, -
1, - 1, 82, 76, 81, 83, - 1, 83, - 1, - 1, 83, 83, 83, 83, 83, 83, 83, 83, 83,
83, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 82, 82, - 1, - 1, - 1, - 1, - 1, 82, 85,
85, 85, 85, 85, 85, 85, 85, 85, 85, 86, - 1, 86, 86, 86, 86, 86, 86, 86, 86, 86,
86, 85, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, - 1, - 1, 86, - 1, - 1, - 1, - 1,
- 1, - 1, 86, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, - 1, - 1, - 1, - 1, 85,
- 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, - 1, - 1, 86, - 1, - 1, - 1, - 1, - 1, -
1, 86, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 87, 86, 87, 87, 87, 87, 87, 87, 87, 87,
87, 87, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1, - 1,
- 1, - 1, - 1, 87, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1, 105,
105, 105, 105, 105, 105, 105, 105, 105, 105, - 1, - 1, - 1, 87, - 1, - 1, - 1, -
1, - 1, - 1, 87, 105, 105, - 1, - 1, - 1, - 1, - 1, 105, 87, 90, 90, 90, 90, 90,
90, 90, 90, 90, 90, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 90, 90, 90, 90, 90, 90,
105, 105, - 1, - 1, - 1, - 1, - 1, 105, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91,
91, 91, 91, 91, 91, 91, 91, 91, 91, - 1, 90, 90, 90, 90, 90, 90, 91, 91, 91, 91,
91, 91, - 1, - 1, - 1, 111, - 1, 91, - 1, - 1, 111, - 1, - 1, - 1, - 1, - 1, 91,
- 1, - 1, 111, 111, 111, 111, 111, 111, 111, 111, - 1, 91, 91, 91, 91, 91, 91,
111, - 1, - 1, - 1, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 111, - 1, - 1, -
1, - 1, 111, 111, - 1, - 1, - 1, 111, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 111, -
1, - 1, - 1, 111, - 1, 111, - 1, 111, - 1, 111, 124, 124, 124, 124, 124, 124,
124, 124, 124, 124, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, - 1, - 1, - 1, - 1, - 1, - 1, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124, 124,
124, 124, 124, 124, 124, 125, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1, - 1, - 1, - 1, - 1, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; int
Cyc_Lex_lex_engine( int start_state, struct Cyc_Lexing_lexbuf* lbuf){ int state;
int base; int backtrk; int c; state= start_state; if( state >=  0){ lbuf->lex_last_pos=(
lbuf->lex_start_pos= lbuf->lex_curr_pos); lbuf->lex_last_action= - 1;} else{
state=( - state) -  1;} while( 1) { base= Cyc_Lex_lex_base[ state]; if( base < 
0){ return( - base) -  1;} backtrk= Cyc_Lex_lex_backtrk[ state]; if( backtrk >= 
0){ lbuf->lex_last_pos= lbuf->lex_curr_pos; lbuf->lex_last_action= backtrk;} if(
lbuf->lex_curr_pos >=  lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){
return( - state) -  1;} else{ c= 256;}} else{ c=( int)*(( unsigned char*)
_check_unknown_subscript( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_curr_pos
++)); if( c ==  - 1){ c= 256;}} if( Cyc_Lex_lex_check[
_check_known_subscript_notnull( 1752u, base +  c)] ==  state){ state= Cyc_Lex_lex_trans[
_check_known_subscript_notnull( 1752u, base +  c)];} else{ state= Cyc_Lex_lex_default[
state];} if( state <  0){ lbuf->lex_curr_pos= lbuf->lex_last_pos; if( lbuf->lex_last_action
==  - 1){( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp219=(
struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp219[ 0]=({ struct Cyc_Lexing_Error_struct _temp220; _temp220.tag= Cyc_Lexing_Error;
_temp220.f1= _tag_arr("empty token", sizeof( unsigned char), 12u); _temp220;});
_temp219;}));} else{ return lbuf->lex_last_action;}} else{ if( c ==  256){ lbuf->lex_eof_reached=
0;}}}} int Cyc_Lex_token_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
lexstate= Cyc_Lex_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0:
_LL221: return( int) Cyc_Lex_process_id( lexbuf); case 1: _LL222: return( int)
Cyc_Lex_process_id( lexbuf); case 2: _LL223: return( int) Cyc_Lex_process_qual_id(
lexbuf); case 3: _LL224: Cyc_Lex_token_string=* Cyc_Lex_get_symbol( Cyc_Lex_str_index_lbuf(
lexbuf)); return 349; case 4: _LL225: Cyc_Lex_token_int= Cyc_Lex_intconst(
lexbuf, 2, 0, 16); return 345; case 5: _LL226: Cyc_Lex_token_int= Cyc_Lex_intconst(
lexbuf, 0, 0, 8); return 345; case 6: _LL227: Cyc_Lex_token_int= Cyc_Lex_intconst(
lexbuf, 0, 0, 10); return 345; case 7: _LL228: Cyc_Lex_token_int= Cyc_Lex_intconst(
lexbuf, 0, 0, 10); return 345; case 8: _LL229: Cyc_Lex_token_string=( struct
_tagged_arr) Cyc_Lexing_lexeme( lexbuf); return 348; case 9: _LL230: Cyc_Lex_token_string=(
struct _tagged_arr) Cyc_Lexing_lexeme( lexbuf); return 348; case 10: _LL231:
return 327; case 11: _LL232: return 328; case 12: _LL233: return 325; case 13:
_LL234: return 326; case 14: _LL235: return 321; case 15: _LL236: return 322;
case 16: _LL237: return 334; case 17: _LL238: return 335; case 18: _LL239:
return 331; case 19: _LL240: return 332; case 20: _LL241: return 333; case 21:
_LL242: return 340; case 22: _LL243: return 339; case 23: _LL244: return 338;
case 24: _LL245: return 336; case 25: _LL246: return 337; case 26: _LL247:
return 329; case 27: _LL248: return 330; case 28: _LL249: return 323; case 29:
_LL250: return 324; case 30: _LL251: return 342; case 31: _LL252: return 320;
case 32: _LL253: return 341; case 33: _LL254: return 343; case 34: _LL255:
return Cyc_Lex_token( lexbuf); case 35: _LL256: return Cyc_Lex_token( lexbuf);
case 36: _LL257: return Cyc_Lex_token( lexbuf); case 37: _LL258: Cyc_Lex_comment_depth=
1; Cyc_Lex_runaway_start= Cyc_Lexing_lexeme_start( lexbuf); Cyc_Lex_comment(
lexbuf); return Cyc_Lex_token( lexbuf); case 38: _LL259: Cyc_Lex_string_pos= 0;
Cyc_Lex_runaway_start= Cyc_Lexing_lexeme_start( lexbuf); while( Cyc_Lex_strng(
lexbuf)) {;} Cyc_Lex_token_string=( struct _tagged_arr) Cyc_Lex_get_stored_string();
return 346; case 39: _LL260: Cyc_Lex_token_char='\a'; return 347; case 40:
_LL261: Cyc_Lex_token_char='\b'; return 347; case 41: _LL262: Cyc_Lex_token_char='\f';
return 347; case 42: _LL263: Cyc_Lex_token_char='\n'; return 347; case 43:
_LL264: Cyc_Lex_token_char='\r'; return 347; case 44: _LL265: Cyc_Lex_token_char='\t';
return 347; case 45: _LL266: Cyc_Lex_token_char='\v'; return 347; case 46:
_LL267: Cyc_Lex_token_char='\\'; return 347; case 47: _LL268: Cyc_Lex_token_char='\'';
return 347; case 48: _LL269: Cyc_Lex_token_char='"'; return 347; case 49: _LL270:
Cyc_Lex_token_char='?'; return 347; case 50: _LL271: Cyc_Lex_token_char=(
unsigned char)(* Cyc_Lex_intconst( lexbuf, 2, 1, 8)).f2; return 347; case 51:
_LL272: Cyc_Lex_token_char=( unsigned char)(* Cyc_Lex_intconst( lexbuf, 3, 1, 16)).f2;
return 347; case 52: _LL273: Cyc_Lex_token_char= Cyc_Lexing_lexeme_char( lexbuf,
1); return 347; case 53: _LL274: return - 1; case 54: _LL275: return( int) Cyc_Lexing_lexeme_char(
lexbuf, 0); default: _LL276:( lexbuf->refill_buff)( lexbuf); return Cyc_Lex_token_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp278=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp278[ 0]=({ struct Cyc_Lexing_Error_struct _temp279; _temp279.tag= Cyc_Lexing_Error;
_temp279.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp279;}); _temp278;}));} int Cyc_Lex_token( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_Lex_token_rec( lexbuf, 0);} int Cyc_Lex_strng_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ lexstate= Cyc_Lex_lex_engine( lexstate, lexbuf); switch(
lexstate){ case 0: _LL280: return 1; case 1: _LL281: return 0; case 2: _LL282:
return 1; case 3: _LL283: Cyc_Lex_store_string_char('\a'); return 1; case 4:
_LL284: Cyc_Lex_store_string_char('\b'); return 1; case 5: _LL285: Cyc_Lex_store_string_char('\f');
return 1; case 6: _LL286: Cyc_Lex_store_string_char('\n'); return 1; case 7:
_LL287: Cyc_Lex_store_string_char('\r'); return 1; case 8: _LL288: Cyc_Lex_store_string_char('\t');
return 1; case 9: _LL289: Cyc_Lex_store_string_char('\v'); return 1; case 10:
_LL290: Cyc_Lex_store_string_char('\\'); return 1; case 11: _LL291: Cyc_Lex_store_string_char('\'');
return 1; case 12: _LL292: Cyc_Lex_store_string_char('"'); return 1; case 13:
_LL293: Cyc_Lex_store_string_char('?'); return 1; case 14: _LL294: Cyc_Lex_store_string_char((
unsigned char)(* Cyc_Lex_intconst( lexbuf, 1, 0, 8)).f2); return 1; case 15:
_LL295: Cyc_Lex_store_string_char(( unsigned char)(* Cyc_Lex_intconst( lexbuf, 2,
0, 16)).f2); return 1; case 16: _LL296: Cyc_Lex_store_string_char( Cyc_Lexing_lexeme_char(
lexbuf, 0)); return 1; case 17: _LL297: Cyc_Lex_runaway_err( _tag_arr("string ends in newline",
sizeof( unsigned char), 23u), lexbuf); return 0; case 18: _LL298: Cyc_Lex_runaway_err(
_tag_arr("unterminated string", sizeof( unsigned char), 20u), lexbuf); return 0;
case 19: _LL299: Cyc_Lex_err( _tag_arr("bad character following backslash in string",
sizeof( unsigned char), 44u), lexbuf); return 1; default: _LL300:( lexbuf->refill_buff)(
lexbuf); return Cyc_Lex_strng_rec( lexbuf, lexstate);}( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp302=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp302[ 0]=({ struct Cyc_Lexing_Error_struct
_temp303; _temp303.tag= Cyc_Lexing_Error; _temp303.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp303;}); _temp302;}));} int Cyc_Lex_strng(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_Lex_strng_rec( lexbuf, 1);} int
Cyc_Lex_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate=
Cyc_Lex_lex_engine( lexstate, lexbuf); switch( lexstate){ case 0: _LL304: ++ Cyc_Lex_comment_depth;
return Cyc_Lex_comment( lexbuf); case 1: _LL305: -- Cyc_Lex_comment_depth; if(
Cyc_Lex_comment_depth >  0){ return Cyc_Lex_comment( lexbuf);} return 0; case 2:
_LL306: Cyc_Lex_runaway_err( _tag_arr("unterminated comment", sizeof(
unsigned char), 21u), lexbuf); return 0; case 3: _LL307: return Cyc_Lex_comment(
lexbuf); case 4: _LL308: return Cyc_Lex_comment( lexbuf); case 5: _LL309: return
Cyc_Lex_comment( lexbuf); default: _LL310:( lexbuf->refill_buff)( lexbuf);
return Cyc_Lex_comment_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp312=( struct Cyc_Lexing_Error_struct*) _cycalloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp312[ 0]=({ struct Cyc_Lexing_Error_struct
_temp313; _temp313.tag= Cyc_Lexing_Error; _temp313.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp313;}); _temp312;}));} int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_Lex_comment_rec( lexbuf, 2);} void
Cyc_Lex_lex_init(){ Cyc_Lex_ids_trie=({ struct Cyc_Lex_Trie* _temp314=( struct
Cyc_Lex_Trie*) _cycalloc( sizeof( struct Cyc_Lex_Trie)); _temp314->children=(
void*)(( void*) Cyc_Lex_Zero); _temp314->shared_str= 0; _temp314;}); Cyc_Lex_typedefs_trie=({
struct Cyc_Lex_Trie* _temp315=( struct Cyc_Lex_Trie*) _cycalloc( sizeof( struct
Cyc_Lex_Trie)); _temp315->children=( void*)(( void*) Cyc_Lex_Zero); _temp315->shared_str=
0; _temp315;}); Cyc_Lex_symbols=( struct Cyc_Xarray_Xarray*)(( struct Cyc_Xarray_Xarray*(*)(
int, struct _tagged_arr*)) Cyc_Xarray_create)( 101, _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char),
1u));(( void(*)( struct Cyc_Xarray_Xarray*, struct _tagged_arr*)) Cyc_Xarray_add)((
struct Cyc_Xarray_Xarray*) _check_null( Cyc_Lex_symbols),& Cyc_Lex_bogus_string);
Cyc_Lex_num_kws=( int) 63u; Cyc_Lex_kw_nums=({ unsigned int _temp316=(
unsigned int) Cyc_Lex_num_kws; int* _temp317=( int*) _cycalloc_atomic(
_check_times( sizeof( int), _temp316)); struct _tagged_arr _temp319= _tag_arr(
_temp317, sizeof( int),( unsigned int) Cyc_Lex_num_kws);{ unsigned int _temp318=
_temp316; unsigned int i; for( i= 0; i <  _temp318; i ++){ _temp317[ i]= 0;}};
_temp319;});{ int i= 0; for( 0; i <  Cyc_Lex_num_kws; ++ i){ struct _tagged_arr
_temp320=( Cyc_Lex_rw_array[ i]).f1; Cyc_Lex_str_index( _temp320, 0,( int) Cyc_Std_strlen(
_temp320));*(( int*) _check_unknown_subscript( Cyc_Lex_kw_nums, sizeof( int), i))=(
int)( Cyc_Lex_rw_array[ i]).f2;}} Cyc_Lex_typedef_init(); Cyc_Lex_comment_depth=
0;}
