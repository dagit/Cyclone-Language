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
ntCsl_to_ntsl( unsigned char**); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE;
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
Cyc_Absyn_SizeofType= 13; struct Cyc_Absyn_SizeofType_struct{ int tag; void* f1;
} ; static const int Cyc_Absyn_AnonStructType= 14; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
15; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonEnumType= 16; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
17; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 18; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
tag; struct Cyc_Absyn_MallocInfo f1; } ; static const int Cyc_Absyn_UnresolvedMem_e=
34; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 35; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 36; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 37; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; struct _tuple3{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_ForArrayInfo{
struct Cyc_List_List* defns; struct _tuple3 condition; struct _tuple3 delta;
struct Cyc_Absyn_Stmt* body; } ; static const int Cyc_Absyn_Skip_s= 0; static
const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int
Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
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
struct Cyc_List_List* attributes; int escapes; } ; struct Cyc_Absyn_Fndecl{ void*
sc; int is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
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
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_forarray_stmt( struct Cyc_List_List*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment*
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
Cyc_Parse_array2ptr( void* t, int argposn){ void* _temp66= t; struct Cyc_Absyn_Exp*
_temp72; struct Cyc_Absyn_Tqual _temp74; void* _temp76; _LL68: if(( unsigned int)
_temp66 >  3u?*(( int*) _temp66) ==  Cyc_Absyn_ArrayType: 0){ _LL77: _temp76=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp66)->f1; goto _LL75; _LL75:
_temp74=(( struct Cyc_Absyn_ArrayType_struct*) _temp66)->f2; goto _LL73; _LL73:
_temp72=(( struct Cyc_Absyn_ArrayType_struct*) _temp66)->f3; goto _LL69;} else{
goto _LL70;} _LL70: goto _LL71; _LL69: return Cyc_Absyn_starb_typ( _temp76,
argposn? Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp78=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp78->v=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp78;}), 0):( void*) Cyc_Absyn_HeapRgn, _temp74, _temp72 ==  0?( void*) Cyc_Absyn_Unknown_b:(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp79=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp79[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp80; _temp80.tag= Cyc_Absyn_Upper_b; _temp80.f1=( struct Cyc_Absyn_Exp*)
_check_null( _temp72); _temp80;}); _temp79;})); _LL71: return t; _LL67:;} static
void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(* x).f3= Cyc_Parse_array2ptr((*
x).f3, 1);} struct _tuple11{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static
struct _tuple11* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ return({ struct _tuple11* _temp81=( struct _tuple11*)
_cycalloc( sizeof( struct _tuple11)); _temp81->f1=(* t).f2; _temp81->f2=(* t).f3;
_temp81;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_arr decl_kind;{ void* _temp82=( void*)
x->r; struct Cyc_Absyn_Vardecl* _temp108; _LL84: if(*(( int*) _temp82) ==  Cyc_Absyn_Var_d){
_LL109: _temp108=(( struct Cyc_Absyn_Var_d_struct*) _temp82)->f1; goto _LL85;}
else{ goto _LL86;} _LL86: if(*(( int*) _temp82) ==  Cyc_Absyn_Let_d){ goto _LL87;}
else{ goto _LL88;} _LL88: if(*(( int*) _temp82) ==  Cyc_Absyn_Letv_d){ goto
_LL89;} else{ goto _LL90;} _LL90: if(*(( int*) _temp82) ==  Cyc_Absyn_Fn_d){
goto _LL91;} else{ goto _LL92;} _LL92: if(*(( int*) _temp82) ==  Cyc_Absyn_Struct_d){
goto _LL93;} else{ goto _LL94;} _LL94: if(*(( int*) _temp82) ==  Cyc_Absyn_Union_d){
goto _LL95;} else{ goto _LL96;} _LL96: if(*(( int*) _temp82) ==  Cyc_Absyn_Tunion_d){
goto _LL97;} else{ goto _LL98;} _LL98: if(*(( int*) _temp82) ==  Cyc_Absyn_Typedef_d){
goto _LL99;} else{ goto _LL100;} _LL100: if(*(( int*) _temp82) ==  Cyc_Absyn_Enum_d){
goto _LL101;} else{ goto _LL102;} _LL102: if(*(( int*) _temp82) ==  Cyc_Absyn_Namespace_d){
goto _LL103;} else{ goto _LL104;} _LL104: if(*(( int*) _temp82) ==  Cyc_Absyn_Using_d){
goto _LL105;} else{ goto _LL106;} _LL106: if(*(( int*) _temp82) ==  Cyc_Absyn_ExternC_d){
goto _LL107;} else{ goto _LL83;} _LL85: if( _temp108->initializer !=  0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);} if( Cyc_Absyn_is_qvar_qualified( _temp108->name)){
Cyc_Parse_err( _tag_arr("namespaces not allowed on parameter declarations",
sizeof( unsigned char), 49u), x->loc);}{ int found= 0; for( 0; params !=  0;
params= params->tl){ if( Cyc_Std_zstrptrcmp((* _temp108->name).f2,( struct
_tagged_arr*) params->hd) ==  0){ found= 1; break;}} if( ! found){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp111; _temp111.tag=
Cyc_Std_String_pa; _temp111.f1=( struct _tagged_arr)*(* _temp108->name).f2;{
void* _temp110[ 1u]={& _temp111}; Cyc_Std_aprintf( _tag_arr("%s is not listed as a parameter",
sizeof( unsigned char), 32u), _tag_arr( _temp110, sizeof( void*), 1u));}}), x->loc);}
return;} _LL87: decl_kind= _tag_arr("let declaration", sizeof( unsigned char),
16u); goto _LL83; _LL89: decl_kind= _tag_arr("let declaration", sizeof(
unsigned char), 16u); goto _LL83; _LL91: decl_kind= _tag_arr("function declaration",
sizeof( unsigned char), 21u); goto _LL83; _LL93: decl_kind= _tag_arr("struct declaration",
sizeof( unsigned char), 19u); goto _LL83; _LL95: decl_kind= _tag_arr("union declaration",
sizeof( unsigned char), 18u); goto _LL83; _LL97: decl_kind= _tag_arr("tunion declaration",
sizeof( unsigned char), 19u); goto _LL83; _LL99: decl_kind= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL83; _LL101: decl_kind= _tag_arr("enum declaration",
sizeof( unsigned char), 17u); goto _LL83; _LL103: decl_kind= _tag_arr("namespace declaration",
sizeof( unsigned char), 22u); goto _LL83; _LL105: decl_kind= _tag_arr("using declaration",
sizeof( unsigned char), 18u); goto _LL83; _LL107: decl_kind= _tag_arr("extern C declaration",
sizeof( unsigned char), 21u); goto _LL83; _LL83:;}(( int(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp113; _temp113.tag= Cyc_Std_String_pa;
_temp113.f1=( struct _tagged_arr) decl_kind;{ void* _temp112[ 1u]={& _temp113};
Cyc_Std_aprintf( _tag_arr("%s appears in parameter type", sizeof( unsigned char),
29u), _tag_arr( _temp112, sizeof( void*), 1u));}}), x->loc); return;} static
struct Cyc_Absyn_Vardecl* Cyc_Parse_decl2vardecl( struct Cyc_Absyn_Decl* d){
void* _temp114=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp120; _LL116: if(*((
int*) _temp114) ==  Cyc_Absyn_Var_d){ _LL121: _temp120=(( struct Cyc_Absyn_Var_d_struct*)
_temp114)->f1; goto _LL117;} else{ goto _LL118;} _LL118: goto _LL119; _LL117:
return _temp120; _LL119: return(( struct Cyc_Absyn_Vardecl*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("bad declaration in `forarray' statement",
sizeof( unsigned char), 40u), d->loc); _LL115:;} struct _tuple12{ struct Cyc_List_List*
f1; struct Cyc_Position_Segment* f2; } ; static struct _tuple2* Cyc_Parse_get_param_type(
struct _tuple12* env, struct _tagged_arr* x){ struct _tuple12 _temp124; struct
Cyc_Position_Segment* _temp125; struct Cyc_List_List* _temp127; struct _tuple12*
_temp122= env; _temp124=* _temp122; _LL128: _temp127= _temp124.f1; goto _LL126;
_LL126: _temp125= _temp124.f2; goto _LL123; _LL123: if( _temp127 ==  0){ return((
struct _tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp130; _temp130.tag= Cyc_Std_String_pa; _temp130.f1=( struct _tagged_arr)* x;{
void* _temp129[ 1u]={& _temp130}; Cyc_Std_aprintf( _tag_arr("missing type for parameter %s",
sizeof( unsigned char), 30u), _tag_arr( _temp129, sizeof( void*), 1u));}}),
_temp125);}{ void* _temp131=( void*)(( struct Cyc_Absyn_Decl*) _temp127->hd)->r;
struct Cyc_Absyn_Vardecl* _temp137; _LL133: if(*(( int*) _temp131) ==  Cyc_Absyn_Var_d){
_LL138: _temp137=(( struct Cyc_Absyn_Var_d_struct*) _temp131)->f1; goto _LL134;}
else{ goto _LL135;} _LL135: goto _LL136; _LL134: if( Cyc_Absyn_is_qvar_qualified(
_temp137->name)){ Cyc_Parse_err( _tag_arr("namespace not allowed on parameter",
sizeof( unsigned char), 35u), _temp125);} if( Cyc_Std_zstrptrcmp((* _temp137->name).f2,
x) ==  0){ return({ struct _tuple2* _temp139=( struct _tuple2*) _cycalloc(
sizeof( struct _tuple2)); _temp139->f1=({ struct Cyc_Core_Opt* _temp140=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp140->v=( void*)(*
_temp137->name).f2; _temp140;}); _temp139->f2= _temp137->tq; _temp139->f3=( void*)
_temp137->type; _temp139;});} else{ return Cyc_Parse_get_param_type(({ struct
_tuple12* _temp141=( struct _tuple12*) _cycalloc( sizeof( struct _tuple12));
_temp141->f1= _temp127->tl; _temp141->f2= _temp125; _temp141;}), x);} _LL136:
return(( struct _tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration", sizeof(
unsigned char), 25u),(( struct Cyc_Absyn_Decl*) _temp127->hd)->loc); _LL132:;}}
static int Cyc_Parse_is_typeparam( void* tm){ void* _temp142= tm; _LL144: if((
unsigned int) _temp142 >  1u?*(( int*) _temp142) ==  Cyc_Absyn_TypeParams_mod: 0){
goto _LL145;} else{ goto _LL146;} _LL146: goto _LL147; _LL145: return 1; _LL147:
return 0; _LL143:;} static void* Cyc_Parse_id2type( struct _tagged_arr s, void*
k){ if( Cyc_Std_zstrcmp( s, _tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){
return( void*) Cyc_Absyn_HeapRgn;} else{ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp148=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp148[ 0]=({ struct Cyc_Absyn_VarType_struct _temp149; _temp149.tag= Cyc_Absyn_VarType;
_temp149.f1=({ struct Cyc_Absyn_Tvar* _temp150=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp150->name=({ struct _tagged_arr*
_temp151=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp151[ 0]= s; _temp151;}); _temp150->identity= 0; _temp150->kind=( void*) k;
_temp150;}); _temp149;}); _temp148;});}} static struct Cyc_Absyn_Tvar* Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar* t){ void* k;{ void* _temp152= Cyc_Absyn_compress_kb((
void*) t->kind); void* _temp160; void* _temp162; _LL154: if(*(( int*) _temp152)
==  Cyc_Absyn_Eq_kb){ _LL161: _temp160=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp152)->f1; goto _LL155;} else{ goto _LL156;} _LL156: if(*(( int*) _temp152)
==  Cyc_Absyn_Unknown_kb){ goto _LL157;} else{ goto _LL158;} _LL158: if(*(( int*)
_temp152) ==  Cyc_Absyn_Less_kb){ _LL163: _temp162=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp152)->f2; goto _LL159;} else{ goto _LL153;} _LL155: k=( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp164=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp164[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp165; _temp165.tag= Cyc_Absyn_Eq_kb;
_temp165.f1=( void*) _temp160; _temp165;}); _temp164;}); goto _LL153; _LL157: k=(
void*)({ struct Cyc_Absyn_Unknown_kb_struct* _temp166=( struct Cyc_Absyn_Unknown_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct)); _temp166[ 0]=({ struct
Cyc_Absyn_Unknown_kb_struct _temp167; _temp167.tag= Cyc_Absyn_Unknown_kb;
_temp167.f1= 0; _temp167;}); _temp166;}); goto _LL153; _LL159: k=( void*)({
struct Cyc_Absyn_Less_kb_struct* _temp168=( struct Cyc_Absyn_Less_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct)); _temp168[ 0]=({ struct Cyc_Absyn_Less_kb_struct
_temp169; _temp169.tag= Cyc_Absyn_Less_kb; _temp169.f1= 0; _temp169.f2=( void*)
_temp162; _temp169;}); _temp168;}); goto _LL153; _LL153:;} return({ struct Cyc_Absyn_Tvar*
_temp170=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp170->name= t->name; _temp170->identity= 0; _temp170->kind=( void*) k;
_temp170;});} static struct Cyc_Absyn_Tvar* Cyc_Parse_typ2tvar( struct Cyc_Position_Segment*
loc, void* t){ void* _temp171= t; struct Cyc_Absyn_Tvar* _temp177; _LL173: if((
unsigned int) _temp171 >  3u?*(( int*) _temp171) ==  Cyc_Absyn_VarType: 0){
_LL178: _temp177=(( struct Cyc_Absyn_VarType_struct*) _temp171)->f1; goto _LL174;}
else{ goto _LL175;} _LL175: goto _LL176; _LL174: return _temp177; _LL176: return((
struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL172:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp179=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp179[ 0]=({ struct Cyc_Absyn_VarType_struct _temp180; _temp180.tag= Cyc_Absyn_VarType;
_temp180.f1= pr; _temp180;}); _temp179;});} static void Cyc_Parse_set_vartyp_kind(
void* t, void* k){ void* _temp181= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp187; struct Cyc_Absyn_Tvar _temp189; void* _temp190; void** _temp192;
_LL183: if(( unsigned int) _temp181 >  3u?*(( int*) _temp181) ==  Cyc_Absyn_VarType:
0){ _LL188: _temp187=(( struct Cyc_Absyn_VarType_struct*) _temp181)->f1;
_temp189=* _temp187; _LL191: _temp190=( void*) _temp189.kind; _temp192=( void**)&(*((
struct Cyc_Absyn_VarType_struct*) _temp181)->f1).kind; goto _LL184;} else{ goto
_LL185;} _LL185: goto _LL186; _LL184: { void* _temp193= Cyc_Absyn_compress_kb(*
_temp192); void* _temp194= _temp193; _LL196: if(*(( int*) _temp194) ==  Cyc_Absyn_Eq_kb){
goto _LL197;} else{ goto _LL198;} _LL198: if(*(( int*) _temp194) ==  Cyc_Absyn_Less_kb){
goto _LL199;} else{ goto _LL200;} _LL200: if(*(( int*) _temp194) ==  Cyc_Absyn_Unknown_kb){
goto _LL201;} else{ goto _LL195;} _LL197: return; _LL199: return; _LL201:*
_temp192=( void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp202=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp202[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp203; _temp203.tag= Cyc_Absyn_Eq_kb; _temp203.f1=( void*) k; _temp203;});
_temp202;}); return; _LL195:;} _LL186: return; _LL182:;} static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List* tms, struct Cyc_List_List*
tds, struct Cyc_Position_Segment* loc){ if( tds ==  0){ return tms;} if( tms == 
0){ return 0;}{ void* _temp204=( void*) tms->hd; void* _temp210; _LL206: if((
unsigned int) _temp204 >  1u?*(( int*) _temp204) ==  Cyc_Absyn_Function_mod: 0){
_LL211: _temp210=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp204)->f1;
goto _LL207;} else{ goto _LL208;} _LL208: goto _LL209; _LL207: if( tms->tl ==  0?
1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd)?((
struct Cyc_List_List*) _check_null( tms->tl))->tl ==  0: 0)){ void* _temp212=
_temp210; struct Cyc_List_List* _temp218; _LL214: if(*(( int*) _temp212) ==  Cyc_Absyn_WithTypes){
goto _LL215;} else{ goto _LL216;} _LL216: if(*(( int*) _temp212) ==  Cyc_Absyn_NoTypes){
_LL219: _temp218=(( struct Cyc_Absyn_NoTypes_struct*) _temp212)->f1; goto _LL217;}
else{ goto _LL213;} _LL215: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL217:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp218,
tds);{ struct _tuple12* _temp220=({ struct _tuple12* _temp226=( struct _tuple12*)
_cycalloc( sizeof( struct _tuple12)); _temp226->f1= tds; _temp226->f2= loc;
_temp226;}); return({ struct Cyc_List_List* _temp221=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp221->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp222=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp222[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp223; _temp223.tag= Cyc_Absyn_Function_mod;
_temp223.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp224=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp224[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp225; _temp225.tag= Cyc_Absyn_WithTypes;
_temp225.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp220, _temp218); _temp225.f2= 0; _temp225.f3= 0;
_temp225.f4= 0; _temp225.f5= 0; _temp225;}); _temp224;})); _temp223;}); _temp222;}));
_temp221->tl= 0; _temp221;});} _LL213:;} else{ return({ struct Cyc_List_List*
_temp227=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp227->hd=( void*)(( void*) tms->hd); _temp227->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp227;});} _LL209: return({ struct Cyc_List_List*
_temp228=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp228->hd=( void*)(( void*) tms->hd); _temp228->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp228;}); _LL205:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds != 
0){ d=({ struct Cyc_Parse_Declarator* _temp229=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp229->id= d->id; _temp229->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp229;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso !=  0){ tss=(( struct
Cyc_Parse_Declaration_spec*) dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline;
atts=(( struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct
Cyc_Parse_Declaration_spec*) dso->v)->sc !=  0){ void* _temp230=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*) dso->v)->sc))->v;
_LL232: if( _temp230 == ( void*) Cyc_Parse_Extern_sc){ goto _LL233;} else{ goto
_LL234;} _LL234: if( _temp230 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL235;}
else{ goto _LL236;} _LL236: if( _temp230 == ( void*) Cyc_Parse_Static_sc){ goto
_LL237;} else{ goto _LL238;} _LL238: goto _LL239; _LL233: sc=( void*) Cyc_Absyn_Extern;
goto _LL231; _LL235: sc=( void*) Cyc_Absyn_ExternC; goto _LL231; _LL237: sc=(
void*) Cyc_Absyn_Static; goto _LL231; _LL239: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL231; _LL231:;}}{ struct Cyc_Core_Opt*
_temp242; void* _temp244; struct _tuple5 _temp240= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL245: _temp244= _temp240.f1; goto _LL243; _LL243: _temp242=
_temp240.f2; goto _LL241; _LL241: { struct Cyc_List_List* _temp248; struct Cyc_List_List*
_temp250; void* _temp252; struct Cyc_Absyn_Tqual _temp254; struct _tuple6
_temp246= Cyc_Parse_apply_tms( tq, _temp244, atts, d->tms); _LL255: _temp254=
_temp246.f1; goto _LL253; _LL253: _temp252= _temp246.f2; goto _LL251; _LL251:
_temp250= _temp246.f3; goto _LL249; _LL249: _temp248= _temp246.f4; goto _LL247;
_LL247: if( _temp242 !=  0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp250 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp256= _temp252; struct Cyc_Absyn_FnInfo _temp262; struct Cyc_List_List*
_temp264; struct Cyc_List_List* _temp266; struct Cyc_Absyn_VarargInfo* _temp268;
int _temp270; struct Cyc_List_List* _temp272; void* _temp274; struct Cyc_Core_Opt*
_temp276; struct Cyc_List_List* _temp278; _LL258: if(( unsigned int) _temp256 > 
3u?*(( int*) _temp256) ==  Cyc_Absyn_FnType: 0){ _LL263: _temp262=(( struct Cyc_Absyn_FnType_struct*)
_temp256)->f1; _LL279: _temp278= _temp262.tvars; goto _LL277; _LL277: _temp276=
_temp262.effect; goto _LL275; _LL275: _temp274=( void*) _temp262.ret_typ; goto
_LL273; _LL273: _temp272= _temp262.args; goto _LL271; _LL271: _temp270= _temp262.c_varargs;
goto _LL269; _LL269: _temp268= _temp262.cyc_varargs; goto _LL267; _LL267:
_temp266= _temp262.rgn_po; goto _LL265; _LL265: _temp264= _temp262.attributes;
goto _LL259;} else{ goto _LL260;} _LL260: goto _LL261; _LL259: { struct Cyc_List_List*
_temp280=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp272); return({ struct Cyc_Absyn_Fndecl*
_temp281=( struct Cyc_Absyn_Fndecl*) _cycalloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp281->sc=( void*) sc; _temp281->name= d->id; _temp281->tvs= _temp278;
_temp281->is_inline= is_inline; _temp281->effect= _temp276; _temp281->ret_type=(
void*) _temp274; _temp281->args= _temp280; _temp281->c_varargs= _temp270;
_temp281->cyc_varargs= _temp268; _temp281->rgn_po= _temp266; _temp281->body=
body; _temp281->cached_typ= 0; _temp281->param_vardecls= 0; _temp281->attributes=
Cyc_List_append( _temp264, _temp248); _temp281;});} _LL261: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL257:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 ==  0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp282=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp282->f1= _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"?", sizeof( unsigned char),
2u); _temp282->f2=(* t).f2; _temp282->f3=(* t).f3; _temp282;});} else{ return({
struct _tuple4* _temp283=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp283->f1=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp283->f2=(* t).f2; _temp283->f3=(* t).f3; _temp283;});}} static
unsigned char _temp284[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp284, _temp284, _temp284 +  52u};
static unsigned char _temp285[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp285, _temp285, _temp285 +  63u};
static unsigned char _temp286[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp286, _temp286, _temp286 +  50u};
static unsigned char _temp287[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp287, _temp287, _temp287 +  60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts !=  0; ts= ts->tl){ void* _temp288=( void*) ts->hd;
struct Cyc_Position_Segment* _temp302; void* _temp304; struct Cyc_Position_Segment*
_temp306; struct Cyc_Position_Segment* _temp308; struct Cyc_Position_Segment*
_temp310; struct Cyc_Position_Segment* _temp312; struct Cyc_Absyn_Decl* _temp314;
_LL290: if(*(( int*) _temp288) ==  Cyc_Parse_Type_spec){ _LL305: _temp304=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp288)->f1; goto _LL303; _LL303: _temp302=((
struct Cyc_Parse_Type_spec_struct*) _temp288)->f2; goto _LL291;} else{ goto
_LL292;} _LL292: if(*(( int*) _temp288) ==  Cyc_Parse_Signed_spec){ _LL307:
_temp306=(( struct Cyc_Parse_Signed_spec_struct*) _temp288)->f1; goto _LL293;}
else{ goto _LL294;} _LL294: if(*(( int*) _temp288) ==  Cyc_Parse_Unsigned_spec){
_LL309: _temp308=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp288)->f1; goto
_LL295;} else{ goto _LL296;} _LL296: if(*(( int*) _temp288) ==  Cyc_Parse_Short_spec){
_LL311: _temp310=(( struct Cyc_Parse_Short_spec_struct*) _temp288)->f1; goto
_LL297;} else{ goto _LL298;} _LL298: if(*(( int*) _temp288) ==  Cyc_Parse_Long_spec){
_LL313: _temp312=(( struct Cyc_Parse_Long_spec_struct*) _temp288)->f1; goto
_LL299;} else{ goto _LL300;} _LL300: if(*(( int*) _temp288) ==  Cyc_Parse_Decl_spec){
_LL315: _temp314=(( struct Cyc_Parse_Decl_spec_struct*) _temp288)->f1; goto
_LL301;} else{ goto _LL289;} _LL291: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp302);} last_loc= _temp302; seen_type= 1; t= _temp304; goto _LL289; _LL293:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp306);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp306);} last_loc= _temp306; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL289; _LL295: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp308);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp308);} last_loc= _temp308; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL289; _LL297: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp310);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp310);} last_loc= _temp310; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL289; _LL299: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp312);} if( seen_size){ void* _temp316= sz; _LL318: if( _temp316 == ( void*)
Cyc_Absyn_B4){ goto _LL319;} else{ goto _LL320;} _LL320: goto _LL321; _LL319: sz=(
void*) Cyc_Absyn_B8; goto _LL317; _LL321: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp312); goto _LL317; _LL317:;} last_loc=
_temp312; seen_size= 1; goto _LL289; _LL301: last_loc= _temp314->loc; if(
declopt ==  0? ! seen_type: 0){ seen_type= 1;{ void* _temp322=( void*) _temp314->r;
struct Cyc_Absyn_Structdecl* _temp334; struct Cyc_Absyn_Tuniondecl* _temp336;
struct Cyc_Absyn_Uniondecl* _temp338; struct Cyc_Absyn_Enumdecl* _temp340;
_LL324: if(*(( int*) _temp322) ==  Cyc_Absyn_Struct_d){ _LL335: _temp334=((
struct Cyc_Absyn_Struct_d_struct*) _temp322)->f1; goto _LL325;} else{ goto
_LL326;} _LL326: if(*(( int*) _temp322) ==  Cyc_Absyn_Tunion_d){ _LL337:
_temp336=(( struct Cyc_Absyn_Tunion_d_struct*) _temp322)->f1; goto _LL327;}
else{ goto _LL328;} _LL328: if(*(( int*) _temp322) ==  Cyc_Absyn_Union_d){
_LL339: _temp338=(( struct Cyc_Absyn_Union_d_struct*) _temp322)->f1; goto _LL329;}
else{ goto _LL330;} _LL330: if(*(( int*) _temp322) ==  Cyc_Absyn_Enum_d){ _LL341:
_temp340=(( struct Cyc_Absyn_Enum_d_struct*) _temp322)->f1; goto _LL331;} else{
goto _LL332;} _LL332: goto _LL333; _LL325: { struct Cyc_List_List* _temp342=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_copy_tvar,
_temp334->tvs)); t=( void*)({ struct Cyc_Absyn_StructType_struct* _temp343=(
struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp343[ 0]=({ struct Cyc_Absyn_StructType_struct _temp344; _temp344.tag= Cyc_Absyn_StructType;
_temp344.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp334->name))->v); _temp344.f2= _temp342; _temp344.f3= 0;
_temp344;}); _temp343;}); if( _temp334->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp345=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp345->v=( void*) _temp314; _temp345;});} goto _LL323;} _LL327: { struct Cyc_List_List*
_temp346=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_copy_tvar, _temp336->tvs)); t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp347=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp347[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp348; _temp348.tag= Cyc_Absyn_TunionType;
_temp348.f1=({ struct Cyc_Absyn_TunionInfo _temp349; _temp349.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp350=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp350[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp351; _temp351.tag= Cyc_Absyn_KnownTunion;
_temp351.f1=({ struct Cyc_Absyn_Tuniondecl** _temp352=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp352[ 0]= _temp336;
_temp352;}); _temp351;}); _temp350;})); _temp349.targs= _temp346; _temp349.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp349;}); _temp348;}); _temp347;}); if(
_temp336->fields !=  0){ declopt=({ struct Cyc_Core_Opt* _temp353=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp353->v=( void*) _temp314;
_temp353;});} goto _LL323;} _LL329: { struct Cyc_List_List* _temp354=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_copy_tvar,
_temp338->tvs)); t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp355=(
struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp355[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp356; _temp356.tag= Cyc_Absyn_UnionType;
_temp356.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp338->name))->v); _temp356.f2= _temp354; _temp356.f3= 0;
_temp356;}); _temp355;}); if( _temp338->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp357=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp357->v=( void*) _temp314; _temp357;});} goto _LL323;} _LL331: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp358=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp358[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp359; _temp359.tag= Cyc_Absyn_EnumType; _temp359.f1=
_temp340->name; _temp359.f2= 0; _temp359;}); _temp358;}); declopt=({ struct Cyc_Core_Opt*
_temp360=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp360->v=( void*) _temp314; _temp360;}); goto _LL323; _LL333:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp314->loc); goto _LL323; _LL323:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp314->loc);} goto _LL289; _LL289:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type within specifier", sizeof(
unsigned char), 30u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp361=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp361[ 0]=({ struct Cyc_Absyn_IntType_struct _temp362; _temp362.tag= Cyc_Absyn_IntType;
_temp362.f1=( void*) sgn; _temp362.f2=( void*) sz; _temp362;}); _temp361;});}
else{ if( seen_sign){ void* _temp363= t; void* _temp369; _LL365: if((
unsigned int) _temp363 >  3u?*(( int*) _temp363) ==  Cyc_Absyn_IntType: 0){
_LL370: _temp369=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp363)->f2;
goto _LL366;} else{ goto _LL367;} _LL367: goto _LL368; _LL366: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp371=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp371[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp372; _temp372.tag= Cyc_Absyn_IntType; _temp372.f1=( void*) sgn; _temp372.f2=(
void*) _temp369; _temp372;}); _temp371;}); goto _LL364; _LL368: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL364; _LL364:;} if( seen_size){ void* _temp373= t; void*
_temp381; _LL375: if(( unsigned int) _temp373 >  3u?*(( int*) _temp373) ==  Cyc_Absyn_IntType:
0){ _LL382: _temp381=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp373)->f1;
goto _LL376;} else{ goto _LL377;} _LL377: if(( unsigned int) _temp373 >  3u?*((
int*) _temp373) ==  Cyc_Absyn_DoubleType: 0){ goto _LL378;} else{ goto _LL379;}
_LL379: goto _LL380; _LL376: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp383=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp383[ 0]=({ struct Cyc_Absyn_IntType_struct _temp384; _temp384.tag= Cyc_Absyn_IntType;
_temp384.f1=( void*) _temp381; _temp384.f2=( void*) sz; _temp384;}); _temp383;});
goto _LL374; _LL378: t=( void*)({ struct Cyc_Absyn_DoubleType_struct* _temp385=(
struct Cyc_Absyn_DoubleType_struct*) _cycalloc_atomic( sizeof( struct Cyc_Absyn_DoubleType_struct));
_temp385[ 0]=({ struct Cyc_Absyn_DoubleType_struct _temp386; _temp386.tag= Cyc_Absyn_DoubleType;
_temp386.f1= 1; _temp386;}); _temp385;}); goto _LL374; _LL380: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL374; _LL374:;}} return({ struct _tuple5 _temp387; _temp387.f1=
t; _temp387.f2= declopt; _temp387;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds ==  0){ return 0;}{ struct Cyc_Parse_Declarator* _temp388=(
struct Cyc_Parse_Declarator*) ds->hd; struct _tuple1* _temp389= _temp388->id;
struct Cyc_List_List* _temp392; struct Cyc_List_List* _temp394; void* _temp396;
struct Cyc_Absyn_Tqual _temp398; struct _tuple6 _temp390= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp388->tms); _LL399: _temp398= _temp390.f1; goto _LL397;
_LL397: _temp396= _temp390.f2; goto _LL395; _LL395: _temp394= _temp390.f3; goto
_LL393; _LL393: _temp392= _temp390.f4; goto _LL391; _LL391: return({ struct Cyc_List_List*
_temp400=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp400->hd=( void*)({ struct _tuple7* _temp401=( struct _tuple7*) _cycalloc(
sizeof( struct _tuple7)); _temp401->f1= _temp389; _temp401->f2= _temp398;
_temp401->f3= _temp396; _temp401->f4= _temp394; _temp401->f5= _temp392; _temp401;});
_temp400->tl= Cyc_Parse_apply_tmss( _temp398, Cyc_Tcutil_copy_type( t), ds->tl,
shared_atts); _temp400;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms ==  0){ return({ struct _tuple6 _temp402; _temp402.f1= tq;
_temp402.f2= t; _temp402.f3= 0; _temp402.f4= atts; _temp402;});}{ void* _temp403=(
void*) tms->hd; struct Cyc_Absyn_Exp* _temp417; void* _temp419; struct Cyc_Position_Segment*
_temp421; struct Cyc_List_List* _temp423; struct Cyc_Absyn_Tqual _temp425; void*
_temp427; void* _temp429; struct Cyc_List_List* _temp431; struct Cyc_Position_Segment*
_temp433; _LL405: if( _temp403 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL406;}
else{ goto _LL407;} _LL407: if(( unsigned int) _temp403 >  1u?*(( int*) _temp403)
==  Cyc_Absyn_ConstArray_mod: 0){ _LL418: _temp417=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp403)->f1; goto _LL408;} else{ goto _LL409;} _LL409: if(( unsigned int)
_temp403 >  1u?*(( int*) _temp403) ==  Cyc_Absyn_Function_mod: 0){ _LL420:
_temp419=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp403)->f1; goto
_LL410;} else{ goto _LL411;} _LL411: if(( unsigned int) _temp403 >  1u?*(( int*)
_temp403) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL424: _temp423=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp403)->f1; goto _LL422; _LL422: _temp421=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp403)->f2; goto _LL412;} else{ goto _LL413;} _LL413: if(( unsigned int)
_temp403 >  1u?*(( int*) _temp403) ==  Cyc_Absyn_Pointer_mod: 0){ _LL430:
_temp429=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp403)->f1; goto
_LL428; _LL428: _temp427=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp403)->f2; goto _LL426; _LL426: _temp425=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp403)->f3; goto _LL414;} else{ goto _LL415;} _LL415: if(( unsigned int)
_temp403 >  1u?*(( int*) _temp403) ==  Cyc_Absyn_Attributes_mod: 0){ _LL434:
_temp433=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp403)->f1; goto _LL432;
_LL432: _temp431=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp403)->f2; goto
_LL416;} else{ goto _LL404;} _LL406: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp435=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp435[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp436; _temp436.tag= Cyc_Absyn_ArrayType; _temp436.f1=(
void*) t; _temp436.f2= tq; _temp436.f3= 0; _temp436;}); _temp435;}), atts, tms->tl);
_LL408: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp437=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp437[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp438; _temp438.tag= Cyc_Absyn_ArrayType; _temp438.f1=(
void*) t; _temp438.f2= tq; _temp438.f3=( struct Cyc_Absyn_Exp*) _temp417;
_temp438;}); _temp437;}), atts, tms->tl); _LL410: { void* _temp439= _temp419;
struct Cyc_List_List* _temp445; struct Cyc_Core_Opt* _temp447; struct Cyc_Absyn_VarargInfo*
_temp449; int _temp451; struct Cyc_List_List* _temp453; struct Cyc_Position_Segment*
_temp455; _LL441: if(*(( int*) _temp439) ==  Cyc_Absyn_WithTypes){ _LL454:
_temp453=(( struct Cyc_Absyn_WithTypes_struct*) _temp439)->f1; goto _LL452;
_LL452: _temp451=(( struct Cyc_Absyn_WithTypes_struct*) _temp439)->f2; goto
_LL450; _LL450: _temp449=(( struct Cyc_Absyn_WithTypes_struct*) _temp439)->f3;
goto _LL448; _LL448: _temp447=(( struct Cyc_Absyn_WithTypes_struct*) _temp439)->f4;
goto _LL446; _LL446: _temp445=(( struct Cyc_Absyn_WithTypes_struct*) _temp439)->f5;
goto _LL442;} else{ goto _LL443;} _LL443: if(*(( int*) _temp439) ==  Cyc_Absyn_NoTypes){
_LL456: _temp455=(( struct Cyc_Absyn_NoTypes_struct*) _temp439)->f2; goto _LL444;}
else{ goto _LL440;} _LL442: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as !=  0; as= as->tl){ if( Cyc_Absyn_fntype_att(( void*) as->hd)){
fn_atts=({ struct Cyc_List_List* _temp457=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp457->hd=( void*)(( void*) as->hd); _temp457->tl=
fn_atts; _temp457;});} else{ new_atts=({ struct Cyc_List_List* _temp458=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp458->hd=( void*)((
void*) as->hd); _temp458->tl= new_atts; _temp458;});}}} if( tms->tl !=  0){ void*
_temp459=( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd; struct
Cyc_List_List* _temp465; _LL461: if(( unsigned int) _temp459 >  1u?*(( int*)
_temp459) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL466: _temp465=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp459)->f1; goto _LL462;} else{ goto _LL463;} _LL463: goto _LL464; _LL462:
typvars= _temp465; tms= tms->tl; goto _LL460; _LL464: goto _LL460; _LL460:;} if(((((
! _temp451? _temp449 ==  0: 0)? _temp453 !=  0: 0)? _temp453->tl ==  0: 0)?(*((
struct _tuple2*) _temp453->hd)).f1 ==  0: 0)?(*(( struct _tuple2*) _temp453->hd)).f3
== ( void*) Cyc_Absyn_VoidType: 0){ _temp453= 0;} t= Cyc_Parse_array2ptr( t, 0);((
void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Parse_arg_array2ptr, _temp453); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),
Cyc_Absyn_function_typ( typvars, _temp447, t, _temp453, _temp451, _temp449,
_temp445, fn_atts), new_atts,(( struct Cyc_List_List*) _check_null( tms))->tl);}
_LL444:( int) _throw( Cyc_Parse_abort( _tag_arr("function declaration without parameter types",
sizeof( unsigned char), 45u), _temp455)); _LL440:;} _LL412: if( tms->tl ==  0){
return({ struct _tuple6 _temp467; _temp467.f1= tq; _temp467.f2= t; _temp467.f3=
_temp423; _temp467.f4= atts; _temp467;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp421)); _LL414: { void* _temp468= _temp429;
struct Cyc_Absyn_Exp* _temp476; struct Cyc_Absyn_Exp* _temp478; _LL470: if((
unsigned int) _temp468 >  1u?*(( int*) _temp468) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL477: _temp476=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp468)->f1; goto
_LL471;} else{ goto _LL472;} _LL472: if(( unsigned int) _temp468 >  1u?*(( int*)
_temp468) ==  Cyc_Absyn_Nullable_ps: 0){ _LL479: _temp478=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp468)->f1; goto _LL473;} else{ goto _LL474;} _LL474: if( _temp468 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL475;} else{ goto _LL469;} _LL471: return Cyc_Parse_apply_tms(
_temp425, Cyc_Absyn_atb_typ( t, _temp427, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp480=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp480[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp481; _temp481.tag= Cyc_Absyn_Upper_b;
_temp481.f1= _temp476; _temp481;}); _temp480;})), atts, tms->tl); _LL473: return
Cyc_Parse_apply_tms( _temp425, Cyc_Absyn_starb_typ( t, _temp427, tq,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp482=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp482[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp483; _temp483.tag= Cyc_Absyn_Upper_b; _temp483.f1= _temp478; _temp483;});
_temp482;})), atts, tms->tl); _LL475: return Cyc_Parse_apply_tms( _temp425, Cyc_Absyn_tagged_typ(
t, _temp427, tq), atts, tms->tl); _LL469:;} _LL416: return Cyc_Parse_apply_tms(
tq, t, Cyc_List_append( atts, _temp431), tms->tl); _LL404:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp486; void* _temp488; struct _tuple5 _temp484= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL489: _temp488= _temp484.f1; goto _LL487; _LL487: _temp486=
_temp484.f2; goto _LL485; _LL485: if( _temp486 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp488;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp490=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp490[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp491; _temp491.tag= Cyc_Absyn_Decl_s;
_temp491.f1= d; _temp491.f2= s; _temp491;}); _temp490;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc !=  0){ void* _temp492=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL494: if( _temp492 == (
void*) Cyc_Parse_Typedef_sc){ goto _LL495;} else{ goto _LL496;} _LL496: if(
_temp492 == ( void*) Cyc_Parse_Extern_sc){ goto _LL497;} else{ goto _LL498;}
_LL498: if( _temp492 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL499;} else{ goto
_LL500;} _LL500: if( _temp492 == ( void*) Cyc_Parse_Static_sc){ goto _LL501;}
else{ goto _LL502;} _LL502: if( _temp492 == ( void*) Cyc_Parse_Auto_sc){ goto
_LL503;} else{ goto _LL504;} _LL504: if( _temp492 == ( void*) Cyc_Parse_Register_sc){
goto _LL505;} else{ goto _LL506;} _LL506: if( _temp492 == ( void*) Cyc_Parse_Abstract_sc){
goto _LL507;} else{ goto _LL493;} _LL495: istypedef= 1; goto _LL493; _LL497: s=(
void*) Cyc_Absyn_Extern; goto _LL493; _LL499: s=( void*) Cyc_Absyn_ExternC; goto
_LL493; _LL501: s=( void*) Cyc_Absyn_Static; goto _LL493; _LL503: s=( void*) Cyc_Absyn_Public;
goto _LL493; _LL505: s=( void*) Cyc_Absyn_Public; goto _LL493; _LL507: s=( void*)
Cyc_Absyn_Abstract; goto _LL493; _LL493:;}{ struct Cyc_List_List* _temp510;
struct Cyc_List_List* _temp512; struct _tuple0 _temp508=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL513: _temp512= _temp508.f1;
goto _LL511; _LL511: _temp510= _temp508.f2; goto _LL509; _LL509: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp510; for( 0; es !=  0; es= es->tl){
if(( struct Cyc_Absyn_Exp*) es->hd !=  0){ exps_empty= 0; break;}}}{ struct
_tuple5 _temp514= Cyc_Parse_collapse_type_specifiers( tss, loc); if( _temp512 == 
0){ struct Cyc_Core_Opt* _temp517; void* _temp519; struct _tuple5 _temp515=
_temp514; _LL520: _temp519= _temp515.f1; goto _LL518; _LL518: _temp517= _temp515.f2;
goto _LL516; _LL516: if( _temp517 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
_temp517->v;{ void* _temp521=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp533;
struct Cyc_Absyn_Structdecl* _temp535; struct Cyc_Absyn_Uniondecl* _temp537;
struct Cyc_Absyn_Tuniondecl* _temp539; _LL523: if(*(( int*) _temp521) ==  Cyc_Absyn_Enum_d){
_LL534: _temp533=(( struct Cyc_Absyn_Enum_d_struct*) _temp521)->f1; goto _LL524;}
else{ goto _LL525;} _LL525: if(*(( int*) _temp521) ==  Cyc_Absyn_Struct_d){
_LL536: _temp535=(( struct Cyc_Absyn_Struct_d_struct*) _temp521)->f1; goto
_LL526;} else{ goto _LL527;} _LL527: if(*(( int*) _temp521) ==  Cyc_Absyn_Union_d){
_LL538: _temp537=(( struct Cyc_Absyn_Union_d_struct*) _temp521)->f1; goto _LL528;}
else{ goto _LL529;} _LL529: if(*(( int*) _temp521) ==  Cyc_Absyn_Tunion_d){
_LL540: _temp539=(( struct Cyc_Absyn_Tunion_d_struct*) _temp521)->f1; goto
_LL530;} else{ goto _LL531;} _LL531: goto _LL532; _LL524:( void*)( _temp533->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum",
sizeof( unsigned char), 23u), loc);} goto _LL522; _LL526:( void*)( _temp535->sc=(
void*) s); _temp535->attributes= atts; goto _LL522; _LL528:( void*)( _temp537->sc=(
void*) s); _temp537->attributes= atts; goto _LL522; _LL530:( void*)( _temp539->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL522; _LL532: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL522:;}
return({ struct Cyc_List_List* _temp541=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp541->hd=( void*) d; _temp541->tl= 0;
_temp541;});} else{ void* _temp542= _temp519; struct Cyc_List_List* _temp558;
struct _tuple1* _temp560; struct Cyc_Absyn_TunionInfo _temp562; void* _temp564;
struct Cyc_Absyn_Tuniondecl** _temp566; struct Cyc_Absyn_TunionInfo _temp568;
struct Cyc_List_List* _temp570; void* _temp572; struct Cyc_Absyn_UnknownTunionInfo
_temp574; int _temp576; struct _tuple1* _temp578; struct Cyc_List_List* _temp580;
struct _tuple1* _temp582; struct _tuple1* _temp584; struct Cyc_List_List*
_temp586; _LL544: if(( unsigned int) _temp542 >  3u?*(( int*) _temp542) ==  Cyc_Absyn_StructType:
0){ _LL561: _temp560=(( struct Cyc_Absyn_StructType_struct*) _temp542)->f1; goto
_LL559; _LL559: _temp558=(( struct Cyc_Absyn_StructType_struct*) _temp542)->f2;
goto _LL545;} else{ goto _LL546;} _LL546: if(( unsigned int) _temp542 >  3u?*((
int*) _temp542) ==  Cyc_Absyn_TunionType: 0){ _LL563: _temp562=(( struct Cyc_Absyn_TunionType_struct*)
_temp542)->f1; _LL565: _temp564=( void*) _temp562.tunion_info; if(*(( int*)
_temp564) ==  Cyc_Absyn_KnownTunion){ _LL567: _temp566=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp564)->f1; goto _LL547;} else{ goto _LL548;}} else{ goto _LL548;} _LL548:
if(( unsigned int) _temp542 >  3u?*(( int*) _temp542) ==  Cyc_Absyn_TunionType:
0){ _LL569: _temp568=(( struct Cyc_Absyn_TunionType_struct*) _temp542)->f1;
_LL573: _temp572=( void*) _temp568.tunion_info; if(*(( int*) _temp572) ==  Cyc_Absyn_UnknownTunion){
_LL575: _temp574=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp572)->f1;
_LL579: _temp578= _temp574.name; goto _LL577; _LL577: _temp576= _temp574.is_xtunion;
goto _LL571;} else{ goto _LL550;} _LL571: _temp570= _temp568.targs; goto _LL549;}
else{ goto _LL550;} _LL550: if(( unsigned int) _temp542 >  3u?*(( int*) _temp542)
==  Cyc_Absyn_UnionType: 0){ _LL583: _temp582=(( struct Cyc_Absyn_UnionType_struct*)
_temp542)->f1; goto _LL581; _LL581: _temp580=(( struct Cyc_Absyn_UnionType_struct*)
_temp542)->f2; goto _LL551;} else{ goto _LL552;} _LL552: if(( unsigned int)
_temp542 >  3u?*(( int*) _temp542) ==  Cyc_Absyn_EnumType: 0){ _LL585: _temp584=((
struct Cyc_Absyn_EnumType_struct*) _temp542)->f1; goto _LL553;} else{ goto
_LL554;} _LL554: if(( unsigned int) _temp542 >  3u?*(( int*) _temp542) ==  Cyc_Absyn_AnonEnumType:
0){ _LL587: _temp586=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp542)->f1;
goto _LL555;} else{ goto _LL556;} _LL556: goto _LL557; _LL545: { struct Cyc_List_List*
_temp588=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp558); struct Cyc_Absyn_Structdecl* _temp589=({
struct Cyc_Absyn_Structdecl* _temp593=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp593->sc=( void*) s; _temp593->name=({
struct Cyc_Core_Opt* _temp594=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp594->v=( void*)(( struct _tuple1*) _check_null( _temp560));
_temp594;}); _temp593->tvs= _temp588; _temp593->fields= 0; _temp593->attributes=
0; _temp593;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp590=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp590->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp591=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp591[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp592; _temp592.tag= Cyc_Absyn_Struct_d;
_temp592.f1= _temp589; _temp592;}); _temp591;}), loc); _temp590->tl= 0; _temp590;});}
_LL547: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp595=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp595->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp596=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp596[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp597; _temp597.tag= Cyc_Absyn_Tunion_d;
_temp597.f1=* _temp566; _temp597;}); _temp596;}), loc); _temp595->tl= 0;
_temp595;}); _LL549: { struct Cyc_List_List* _temp598=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp570); struct Cyc_Absyn_Decl* _temp599= Cyc_Absyn_tunion_decl( s, _temp578,
_temp598, 0, _temp576, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp600=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp600->hd=(
void*) _temp599; _temp600->tl= 0; _temp600;});} _LL551: { struct Cyc_List_List*
_temp601=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp580); struct Cyc_Absyn_Uniondecl* _temp602=({
struct Cyc_Absyn_Uniondecl* _temp607=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp607->sc=( void*) s; _temp607->name=({
struct Cyc_Core_Opt* _temp608=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp608->v=( void*)(( struct _tuple1*) _check_null( _temp582));
_temp608;}); _temp607->tvs= _temp601; _temp607->fields= 0; _temp607->attributes=
0; _temp607;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp603=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp603->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp604=( struct Cyc_Absyn_Decl*) _cycalloc(
sizeof( struct Cyc_Absyn_Decl)); _temp604->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp605=( struct Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp605[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp606; _temp606.tag= Cyc_Absyn_Union_d;
_temp606.f1= _temp602; _temp606;}); _temp605;})); _temp604->loc= loc; _temp604;});
_temp603->tl= 0; _temp603;});} _LL553: { struct Cyc_Absyn_Enumdecl* _temp609=({
struct Cyc_Absyn_Enumdecl* _temp614=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp614->sc=( void*) s; _temp614->name=
_temp584; _temp614->fields= 0; _temp614;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp610=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp610->hd=( void*)({ struct Cyc_Absyn_Decl* _temp611=(
struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp611->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp612=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp612[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp613; _temp613.tag= Cyc_Absyn_Enum_d; _temp613.f1= _temp609; _temp613;});
_temp612;})); _temp611->loc= loc; _temp611;}); _temp610->tl= 0; _temp610;});}
_LL555: { struct Cyc_Absyn_Enumdecl* _temp615=({ struct Cyc_Absyn_Enumdecl*
_temp620=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp620->sc=( void*) s; _temp620->name= Cyc_Parse_gensym_enum(); _temp620->fields=({
struct Cyc_Core_Opt* _temp621=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp621->v=( void*) _temp586; _temp621;}); _temp620;}); if(
atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} return({ struct Cyc_List_List* _temp616=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp616->hd=( void*)({
struct Cyc_Absyn_Decl* _temp617=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp617->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp618=( struct Cyc_Absyn_Enum_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp618[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp619; _temp619.tag= Cyc_Absyn_Enum_d;
_temp619.f1= _temp615; _temp619;}); _temp618;})); _temp617->loc= loc; _temp617;});
_temp616->tl= 0; _temp616;});} _LL557: Cyc_Parse_err( _tag_arr("missing declarator",
sizeof( unsigned char), 19u), loc); return 0; _LL543:;}} else{ void* t= _temp514.f1;
struct Cyc_List_List* _temp622= Cyc_Parse_apply_tmss( tq, t, _temp512, atts);
if( istypedef){ if( ! exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp622); if( _temp514.f2 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(
_temp514.f2)->v;{ void* _temp623=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp635; struct Cyc_Absyn_Tuniondecl* _temp637; struct Cyc_Absyn_Uniondecl*
_temp639; struct Cyc_Absyn_Enumdecl* _temp641; _LL625: if(*(( int*) _temp623) == 
Cyc_Absyn_Struct_d){ _LL636: _temp635=(( struct Cyc_Absyn_Struct_d_struct*)
_temp623)->f1; goto _LL626;} else{ goto _LL627;} _LL627: if(*(( int*) _temp623)
==  Cyc_Absyn_Tunion_d){ _LL638: _temp637=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp623)->f1; goto _LL628;} else{ goto _LL629;} _LL629: if(*(( int*) _temp623)
==  Cyc_Absyn_Union_d){ _LL640: _temp639=(( struct Cyc_Absyn_Union_d_struct*)
_temp623)->f1; goto _LL630;} else{ goto _LL631;} _LL631: if(*(( int*) _temp623)
==  Cyc_Absyn_Enum_d){ _LL642: _temp641=(( struct Cyc_Absyn_Enum_d_struct*)
_temp623)->f1; goto _LL632;} else{ goto _LL633;} _LL633: goto _LL634; _LL626:(
void*)( _temp635->sc=( void*) s); _temp635->attributes= atts; atts= 0; goto
_LL624; _LL628:( void*)( _temp637->sc=( void*) s); goto _LL624; _LL630:( void*)(
_temp639->sc=( void*) s); goto _LL624; _LL632:( void*)( _temp641->sc=( void*) s);
goto _LL624; _LL634: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL624; _LL624:;} decls=({ struct Cyc_List_List*
_temp643=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp643->hd=( void*) d; _temp643->tl= decls; _temp643;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp645; _temp645.tag=
Cyc_Std_String_pa; _temp645.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) atts->hd);{ void* _temp644[ 1u]={& _temp645}; Cyc_Std_aprintf( _tag_arr("bad attribute %s in typedef",
sizeof( unsigned char), 28u), _tag_arr( _temp644, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp514.f2 !=  0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp646= _temp622; for( 0; _temp646 !=  0;( _temp646= _temp646->tl,
_temp510= _temp510->tl)){ struct _tuple7 _temp649; struct Cyc_List_List*
_temp650; struct Cyc_List_List* _temp652; void* _temp654; struct Cyc_Absyn_Tqual
_temp656; struct _tuple1* _temp658; struct _tuple7* _temp647=( struct _tuple7*)
_temp646->hd; _temp649=* _temp647; _LL659: _temp658= _temp649.f1; goto _LL657;
_LL657: _temp656= _temp649.f2; goto _LL655; _LL655: _temp654= _temp649.f3; goto
_LL653; _LL653: _temp652= _temp649.f4; goto _LL651; _LL651: _temp650= _temp649.f5;
goto _LL648; _LL648: if( _temp652 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), loc);} if( _temp510 ==  0){(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("unexpected NULL in parse!",
sizeof( unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp660=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp510))->hd; struct Cyc_Absyn_Vardecl*
_temp661= Cyc_Absyn_new_vardecl( _temp658, _temp654, _temp660); _temp661->tq=
_temp656;( void*)( _temp661->sc=( void*) s); _temp661->attributes= _temp650;{
struct Cyc_Absyn_Decl* _temp662=({ struct Cyc_Absyn_Decl* _temp664=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp664->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp665=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp665[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp666; _temp666.tag= Cyc_Absyn_Var_d; _temp666.f1= _temp661; _temp666;});
_temp665;})); _temp664->loc= loc; _temp664;}); decls=({ struct Cyc_List_List*
_temp663=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp663->hd=( void*) _temp662; _temp663->tl= decls; _temp663;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_Std_strlen( s) !=  1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp668; _temp668.tag= Cyc_Std_String_pa;
_temp668.f1=( struct _tagged_arr) s;{ void* _temp667[ 1u]={& _temp668}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp667,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL669: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL670: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL671: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL672: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL673: return(
void*) Cyc_Absyn_EffKind; default: _LL674: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp677; _temp677.tag= Cyc_Std_String_pa;
_temp677.f1=( struct _tagged_arr) s;{ void* _temp676[ 1u]={& _temp677}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp676,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts ==  0){ return
tms;} else{ return({ struct Cyc_List_List* _temp678=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp678->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp679=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp679[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp680; _temp680.tag= Cyc_Absyn_Attributes_mod;
_temp680.f1= loc; _temp680.f2= atts; _temp680;}); _temp679;})); _temp678->tl=
tms; _temp678;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp683;
struct Cyc_List_List* _temp684; struct Cyc_List_List* _temp686; void* _temp688;
struct Cyc_Absyn_Tqual _temp690; struct _tuple1* _temp692; struct _tuple7*
_temp681= t; _temp683=* _temp681; _LL693: _temp692= _temp683.f1; goto _LL691;
_LL691: _temp690= _temp683.f2; goto _LL689; _LL689: _temp688= _temp683.f3; goto
_LL687; _LL687: _temp686= _temp683.f4; goto _LL685; _LL685: _temp684= _temp683.f5;
goto _LL682; _LL682: Cyc_Lex_register_typedef( _temp692); if( _temp684 !=  0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp695;
_temp695.tag= Cyc_Std_String_pa; _temp695.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) _temp684->hd);{ void* _temp694[ 1u]={& _temp695}; Cyc_Std_aprintf(
_tag_arr("bad attribute %s within typedef", sizeof( unsigned char), 32u),
_tag_arr( _temp694, sizeof( void*), 1u));}}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp696=( struct Cyc_Absyn_Typedef_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp696[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp697; _temp697.tag= Cyc_Absyn_Typedef_d; _temp697.f1=({
struct Cyc_Absyn_Typedefdecl* _temp698=( struct Cyc_Absyn_Typedefdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp698->name= _temp692;
_temp698->tvs= _temp686; _temp698->defn=( void*) _temp688; _temp698;}); _temp697;});
_temp696;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
unsigned char _temp700[ 15u]="$(sign_t,int)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp700, _temp700, _temp700 +  15u}};
struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ struct _tuple15* yyzzz;{ void*
_temp701= yy1; struct _tuple15* _temp707; _LL703: if(*(( void**) _temp701) == 
Cyc_Int_tok){ _LL708: _temp707=(( struct Cyc_Int_tok_struct*) _temp701)->f1;
goto _LL704;} else{ goto _LL705;} _LL705: goto _LL706; _LL704: yyzzz= _temp707;
goto _LL702; _LL706:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL702:;}
return yyzzz;} static unsigned char _temp710[ 9u]="string_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp710, _temp710, _temp710 +  9u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ struct _tagged_arr yyzzz;{
void* _temp711= yy1; struct _tagged_arr _temp717; _LL713: if(*(( void**)
_temp711) ==  Cyc_String_tok){ _LL718: _temp717=(( struct Cyc_String_tok_struct*)
_temp711)->f1; goto _LL714;} else{ goto _LL715;} _LL715: goto _LL716; _LL714:
yyzzz= _temp717; goto _LL712; _LL716:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL712:;} return yyzzz;} static unsigned char _temp720[ 5u]="char"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{ _temp720,
_temp720, _temp720 +  5u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
unsigned char yyzzz;{ void* _temp721= yy1; unsigned char _temp727; _LL723: if(*((
void**) _temp721) ==  Cyc_Char_tok){ _LL728: _temp727=(( struct Cyc_Char_tok_struct*)
_temp721)->f1; goto _LL724;} else{ goto _LL725;} _LL725: goto _LL726; _LL724:
yyzzz= _temp727; goto _LL722; _LL726:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL722:;} return yyzzz;} static unsigned char _temp730[ 20u]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{
_temp730, _temp730, _temp730 +  20u}}; void* Cyc_yyget_Pointer_Sort_tok( void*
yy1){ void* yyzzz;{ void* _temp731= yy1; void* _temp737; _LL733: if(*(( void**)
_temp731) ==  Cyc_Pointer_Sort_tok){ _LL738: _temp737=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp731)->f1; goto _LL734;} else{ goto _LL735;} _LL735: goto _LL736; _LL734:
yyzzz= _temp737; goto _LL732; _LL736:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL732:;} return yyzzz;} static unsigned char _temp740[ 6u]="exp_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp740,
_temp740, _temp740 +  6u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp741= yy1; struct Cyc_Absyn_Exp*
_temp747; _LL743: if(*(( void**) _temp741) ==  Cyc_Exp_tok){ _LL748: _temp747=((
struct Cyc_Exp_tok_struct*) _temp741)->f1; goto _LL744;} else{ goto _LL745;}
_LL745: goto _LL746; _LL744: yyzzz= _temp747; goto _LL742; _LL746:( int) _throw((
void*)& Cyc_yyfail_Exp_tok); _LL742:;} return yyzzz;} static unsigned char
_temp750[ 14u]="list_t<exp_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{ _temp750, _temp750, _temp750 +  14u}}; struct Cyc_List_List*
Cyc_yyget_ExpList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp751=
yy1; struct Cyc_List_List* _temp757; _LL753: if(*(( void**) _temp751) ==  Cyc_ExpList_tok){
_LL758: _temp757=(( struct Cyc_ExpList_tok_struct*) _temp751)->f1; goto _LL754;}
else{ goto _LL755;} _LL755: goto _LL756; _LL754: yyzzz= _temp757; goto _LL752;
_LL756:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL752:;} return yyzzz;}
static unsigned char _temp760[ 39u]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{
_temp760, _temp760, _temp760 +  39u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp761= yy1; struct Cyc_List_List*
_temp767; _LL763: if(*(( void**) _temp761) ==  Cyc_InitializerList_tok){ _LL768:
_temp767=(( struct Cyc_InitializerList_tok_struct*) _temp761)->f1; goto _LL764;}
else{ goto _LL765;} _LL765: goto _LL766; _LL764: yyzzz= _temp767; goto _LL762;
_LL766:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL762:;} return
yyzzz;} static unsigned char _temp770[ 9u]="primop_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp770, _temp770, _temp770 +  9u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* yyzzz;{ void* _temp771= yy1; void*
_temp777; _LL773: if(*(( void**) _temp771) ==  Cyc_Primop_tok){ _LL778: _temp777=(
void*)(( struct Cyc_Primop_tok_struct*) _temp771)->f1; goto _LL774;} else{ goto
_LL775;} _LL775: goto _LL776; _LL774: yyzzz= _temp777; goto _LL772; _LL776:( int)
_throw(( void*)& Cyc_yyfail_Primop_tok); _LL772:;} return yyzzz;} static
unsigned char _temp780[ 16u]="opt_t<primop_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp780, _temp780, _temp780 +  16u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ struct Cyc_Core_Opt*
yyzzz;{ void* _temp781= yy1; struct Cyc_Core_Opt* _temp787; _LL783: if(*(( void**)
_temp781) ==  Cyc_Primopopt_tok){ _LL788: _temp787=(( struct Cyc_Primopopt_tok_struct*)
_temp781)->f1; goto _LL784;} else{ goto _LL785;} _LL785: goto _LL786; _LL784:
yyzzz= _temp787; goto _LL782; _LL786:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL782:;} return yyzzz;} static unsigned char _temp790[ 7u]="qvar_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{
_temp790, _temp790, _temp790 +  7u}}; struct _tuple1* Cyc_yyget_QualId_tok( void*
yy1){ struct _tuple1* yyzzz;{ void* _temp791= yy1; struct _tuple1* _temp797;
_LL793: if(*(( void**) _temp791) ==  Cyc_QualId_tok){ _LL798: _temp797=(( struct
Cyc_QualId_tok_struct*) _temp791)->f1; goto _LL794;} else{ goto _LL795;} _LL795:
goto _LL796; _LL794: yyzzz= _temp797; goto _LL792; _LL796:( int) _throw(( void*)&
Cyc_yyfail_QualId_tok); _LL792:;} return yyzzz;} static unsigned char _temp800[
7u]="stmt_t"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{
_temp800, _temp800, _temp800 +  7u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok(
void* yy1){ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp801= yy1; struct Cyc_Absyn_Stmt*
_temp807; _LL803: if(*(( void**) _temp801) ==  Cyc_Stmt_tok){ _LL808: _temp807=((
struct Cyc_Stmt_tok_struct*) _temp801)->f1; goto _LL804;} else{ goto _LL805;}
_LL805: goto _LL806; _LL804: yyzzz= _temp807; goto _LL802; _LL806:( int) _throw((
void*)& Cyc_yyfail_Stmt_tok); _LL802:;} return yyzzz;} static unsigned char
_temp810[ 24u]="list_t<switch_clause_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp810, _temp810,
_temp810 +  24u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ struct Cyc_List_List* yyzzz;{ void* _temp811= yy1; struct Cyc_List_List*
_temp817; _LL813: if(*(( void**) _temp811) ==  Cyc_SwitchClauseList_tok){ _LL818:
_temp817=(( struct Cyc_SwitchClauseList_tok_struct*) _temp811)->f1; goto _LL814;}
else{ goto _LL815;} _LL815: goto _LL816; _LL814: yyzzz= _temp817; goto _LL812;
_LL816:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL812:;}
return yyzzz;} static unsigned char _temp820[ 25u]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp820, _temp820, _temp820 +  25u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp821= yy1; struct Cyc_List_List*
_temp827; _LL823: if(*(( void**) _temp821) ==  Cyc_SwitchCClauseList_tok){
_LL828: _temp827=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp821)->f1;
goto _LL824;} else{ goto _LL825;} _LL825: goto _LL826; _LL824: yyzzz= _temp827;
goto _LL822; _LL826:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL822:;} return yyzzz;} static unsigned char _temp830[ 6u]="pat_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{
_temp830, _temp830, _temp830 +  6u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
void* yy1){ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp831= yy1; struct Cyc_Absyn_Pat*
_temp837; _LL833: if(*(( void**) _temp831) ==  Cyc_Pattern_tok){ _LL838:
_temp837=(( struct Cyc_Pattern_tok_struct*) _temp831)->f1; goto _LL834;} else{
goto _LL835;} _LL835: goto _LL836; _LL834: yyzzz= _temp837; goto _LL832; _LL836:(
int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL832:;} return yyzzz;} static
unsigned char _temp840[ 14u]="list_t<pat_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp840, _temp840, _temp840 + 
14u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp841= yy1; struct Cyc_List_List* _temp847; _LL843: if(*(( void**)
_temp841) ==  Cyc_PatternList_tok){ _LL848: _temp847=(( struct Cyc_PatternList_tok_struct*)
_temp841)->f1; goto _LL844;} else{ goto _LL845;} _LL845: goto _LL846; _LL844:
yyzzz= _temp847; goto _LL842; _LL846:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL842:;} return yyzzz;} static unsigned char _temp850[ 31u]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp850, _temp850, _temp850 +  31u}}; struct _tuple13* Cyc_yyget_FieldPattern_tok(
void* yy1){ struct _tuple13* yyzzz;{ void* _temp851= yy1; struct _tuple13*
_temp857; _LL853: if(*(( void**) _temp851) ==  Cyc_FieldPattern_tok){ _LL858:
_temp857=(( struct Cyc_FieldPattern_tok_struct*) _temp851)->f1; goto _LL854;}
else{ goto _LL855;} _LL855: goto _LL856; _LL854: yyzzz= _temp857; goto _LL852;
_LL856:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL852:;} return
yyzzz;} static unsigned char _temp860[ 39u]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp860, _temp860, _temp860 +  39u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp861= yy1; struct Cyc_List_List*
_temp867; _LL863: if(*(( void**) _temp861) ==  Cyc_FieldPatternList_tok){ _LL868:
_temp867=(( struct Cyc_FieldPatternList_tok_struct*) _temp861)->f1; goto _LL864;}
else{ goto _LL865;} _LL865: goto _LL866; _LL864: yyzzz= _temp867; goto _LL862;
_LL866:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL862:;}
return yyzzz;} static unsigned char _temp870[ 9u]="fndecl_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp870, _temp870, _temp870 +  9u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ struct Cyc_Absyn_Fndecl*
yyzzz;{ void* _temp871= yy1; struct Cyc_Absyn_Fndecl* _temp877; _LL873: if(*((
void**) _temp871) ==  Cyc_FnDecl_tok){ _LL878: _temp877=(( struct Cyc_FnDecl_tok_struct*)
_temp871)->f1; goto _LL874;} else{ goto _LL875;} _LL875: goto _LL876; _LL874:
yyzzz= _temp877; goto _LL872; _LL876:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL872:;} return yyzzz;} static unsigned char _temp880[ 15u]="list_t<decl_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp880, _temp880, _temp880 +  15u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp881= yy1; struct Cyc_List_List*
_temp887; _LL883: if(*(( void**) _temp881) ==  Cyc_DeclList_tok){ _LL888:
_temp887=(( struct Cyc_DeclList_tok_struct*) _temp881)->f1; goto _LL884;} else{
goto _LL885;} _LL885: goto _LL886; _LL884: yyzzz= _temp887; goto _LL882; _LL886:(
int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL882:;} return yyzzz;} static
unsigned char _temp890[ 12u]="decl_spec_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp890, _temp890, _temp890 +  12u}};
struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){ struct
Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp891= yy1; struct Cyc_Parse_Declaration_spec*
_temp897; _LL893: if(*(( void**) _temp891) ==  Cyc_DeclSpec_tok){ _LL898:
_temp897=(( struct Cyc_DeclSpec_tok_struct*) _temp891)->f1; goto _LL894;} else{
goto _LL895;} _LL895: goto _LL896; _LL894: yyzzz= _temp897; goto _LL892; _LL896:(
int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL892:;} return yyzzz;} static
unsigned char _temp900[ 27u]="$(declarator_t,exp_opt_t)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp900, _temp900, _temp900 +  27u}};
struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){ struct _tuple14* yyzzz;{
void* _temp901= yy1; struct _tuple14* _temp907; _LL903: if(*(( void**) _temp901)
==  Cyc_InitDecl_tok){ _LL908: _temp907=(( struct Cyc_InitDecl_tok_struct*)
_temp901)->f1; goto _LL904;} else{ goto _LL905;} _LL905: goto _LL906; _LL904:
yyzzz= _temp907; goto _LL902; _LL906:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL902:;} return yyzzz;} static unsigned char _temp910[ 35u]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp910, _temp910, _temp910 +  35u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp911= yy1; struct Cyc_List_List*
_temp917; _LL913: if(*(( void**) _temp911) ==  Cyc_InitDeclList_tok){ _LL918:
_temp917=(( struct Cyc_InitDeclList_tok_struct*) _temp911)->f1; goto _LL914;}
else{ goto _LL915;} _LL915: goto _LL916; _LL914: yyzzz= _temp917; goto _LL912;
_LL916:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL912:;} return
yyzzz;} static unsigned char _temp920[ 16u]="storage_class_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp920, _temp920, _temp920 + 
16u}}; void* Cyc_yyget_StorageClass_tok( void* yy1){ void* yyzzz;{ void*
_temp921= yy1; void* _temp927; _LL923: if(*(( void**) _temp921) ==  Cyc_StorageClass_tok){
_LL928: _temp927=( void*)(( struct Cyc_StorageClass_tok_struct*) _temp921)->f1;
goto _LL924;} else{ goto _LL925;} _LL925: goto _LL926; _LL924: yyzzz= _temp927;
goto _LL922; _LL926:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL922:;}
return yyzzz;} static unsigned char _temp930[ 17u]="type_specifier_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp930, _temp930, _temp930 +  17u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* yyzzz;{ void* _temp931= yy1; void* _temp937; _LL933: if(*(( void**)
_temp931) ==  Cyc_TypeSpecifier_tok){ _LL938: _temp937=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp931)->f1; goto _LL934;} else{ goto _LL935;} _LL935: goto _LL936; _LL934:
yyzzz= _temp937; goto _LL932; _LL936:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL932:;} return yyzzz;} static unsigned char _temp940[ 18u]="struct_or_union_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp940, _temp940, _temp940 +  18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ enum  Cyc_Parse_Struct_or_union yyzzz;{ void* _temp941= yy1; enum 
Cyc_Parse_Struct_or_union _temp947; _LL943: if(*(( void**) _temp941) ==  Cyc_StructOrUnion_tok){
_LL948: _temp947=(( struct Cyc_StructOrUnion_tok_struct*) _temp941)->f1; goto
_LL944;} else{ goto _LL945;} _LL945: goto _LL946; _LL944: yyzzz= _temp947; goto
_LL942; _LL946:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok); _LL942:;}
return yyzzz;} static unsigned char _temp950[ 8u]="tqual_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp950, _temp950, _temp950 +  8u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp951= yy1; struct Cyc_Absyn_Tqual _temp957; _LL953: if(*((
void**) _temp951) ==  Cyc_TypeQual_tok){ _LL958: _temp957=(( struct Cyc_TypeQual_tok_struct*)
_temp951)->f1; goto _LL954;} else{ goto _LL955;} _LL955: goto _LL956; _LL954:
yyzzz= _temp957; goto _LL952; _LL956:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL952:;} return yyzzz;} static unsigned char _temp960[ 22u]="list_t<structfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp960, _temp960, _temp960 +  22u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp961= yy1; struct Cyc_List_List*
_temp967; _LL963: if(*(( void**) _temp961) ==  Cyc_StructFieldDeclList_tok){
_LL968: _temp967=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp961)->f1;
goto _LL964;} else{ goto _LL965;} _LL965: goto _LL966; _LL964: yyzzz= _temp967;
goto _LL962; _LL966:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL962:;} return yyzzz;} static unsigned char _temp970[ 30u]="list_t<list_t<structfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp970, _temp970, _temp970 +  30u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{
void* _temp971= yy1; struct Cyc_List_List* _temp977; _LL973: if(*(( void**)
_temp971) ==  Cyc_StructFieldDeclListList_tok){ _LL978: _temp977=(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp971)->f1; goto _LL974;} else{ goto _LL975;} _LL975: goto _LL976; _LL974:
yyzzz= _temp977; goto _LL972; _LL976:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL972:;} return yyzzz;} static unsigned char _temp980[ 24u]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{
_temp980, _temp980, _temp980 +  24u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp981= yy1; struct Cyc_List_List*
_temp987; _LL983: if(*(( void**) _temp981) ==  Cyc_TypeModifierList_tok){ _LL988:
_temp987=(( struct Cyc_TypeModifierList_tok_struct*) _temp981)->f1; goto _LL984;}
else{ goto _LL985;} _LL985: goto _LL986; _LL984: yyzzz= _temp987; goto _LL982;
_LL986:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL982:;}
return yyzzz;} static unsigned char _temp990[ 13u]="declarator_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp990,
_temp990, _temp990 +  13u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp991= yy1; struct Cyc_Parse_Declarator*
_temp997; _LL993: if(*(( void**) _temp991) ==  Cyc_Declarator_tok){ _LL998:
_temp997=(( struct Cyc_Declarator_tok_struct*) _temp991)->f1; goto _LL994;}
else{ goto _LL995;} _LL995: goto _LL996; _LL994: yyzzz= _temp997; goto _LL992;
_LL996:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL992:;} return
yyzzz;} static unsigned char _temp1000[ 21u]="abstractdeclarator_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{
_temp1000, _temp1000, _temp1000 +  21u}}; struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_AbstractDeclarator_tok( void* yy1){ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp1001= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1007;
_LL1003: if(*(( void**) _temp1001) ==  Cyc_AbstractDeclarator_tok){ _LL1008:
_temp1007=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1001)->f1; goto
_LL1004;} else{ goto _LL1005;} _LL1005: goto _LL1006; _LL1004: yyzzz= _temp1007;
goto _LL1002; _LL1006:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1002:;} return yyzzz;} static unsigned char _temp1010[ 5u]="bool"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{
_temp1010, _temp1010, _temp1010 +  5u}}; int Cyc_yyget_Bool_tok( void* yy1){ int
yyzzz;{ void* _temp1011= yy1; int _temp1017; _LL1013: if(*(( void**) _temp1011)
==  Cyc_Bool_tok){ _LL1018: _temp1017=(( struct Cyc_Bool_tok_struct*) _temp1011)->f1;
goto _LL1014;} else{ goto _LL1015;} _LL1015: goto _LL1016; _LL1014: yyzzz=
_temp1017; goto _LL1012; _LL1016:( int) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1012:;} return yyzzz;} static unsigned char _temp1020[ 8u]="scope_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{
_temp1020, _temp1020, _temp1020 +  8u}}; void* Cyc_yyget_Scope_tok( void* yy1){
void* yyzzz;{ void* _temp1021= yy1; void* _temp1027; _LL1023: if(*(( void**)
_temp1021) ==  Cyc_Scope_tok){ _LL1028: _temp1027=( void*)(( struct Cyc_Scope_tok_struct*)
_temp1021)->f1; goto _LL1024;} else{ goto _LL1025;} _LL1025: goto _LL1026;
_LL1024: yyzzz= _temp1027; goto _LL1022; _LL1026:( int) _throw(( void*)& Cyc_yyfail_Scope_tok);
_LL1022:;} return yyzzz;} static unsigned char _temp1030[ 14u]="tunionfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1030, _temp1030, _temp1030 +  14u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp1031= yy1; struct
Cyc_Absyn_Tunionfield* _temp1037; _LL1033: if(*(( void**) _temp1031) ==  Cyc_TunionField_tok){
_LL1038: _temp1037=(( struct Cyc_TunionField_tok_struct*) _temp1031)->f1; goto
_LL1034;} else{ goto _LL1035;} _LL1035: goto _LL1036; _LL1034: yyzzz= _temp1037;
goto _LL1032; _LL1036:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL1032:;} return yyzzz;} static unsigned char _temp1040[ 22u]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp1040, _temp1040, _temp1040 +  22u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1041= yy1; struct Cyc_List_List*
_temp1047; _LL1043: if(*(( void**) _temp1041) ==  Cyc_TunionFieldList_tok){
_LL1048: _temp1047=(( struct Cyc_TunionFieldList_tok_struct*) _temp1041)->f1;
goto _LL1044;} else{ goto _LL1045;} _LL1045: goto _LL1046; _LL1044: yyzzz=
_temp1047; goto _LL1042; _LL1046:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1042:;} return yyzzz;} static unsigned char _temp1050[ 50u]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1050, _temp1050, _temp1050 +  50u}}; struct _tuple17* Cyc_yyget_QualSpecList_tok(
void* yy1){ struct _tuple17* yyzzz;{ void* _temp1051= yy1; struct _tuple17*
_temp1057; _LL1053: if(*(( void**) _temp1051) ==  Cyc_QualSpecList_tok){ _LL1058:
_temp1057=(( struct Cyc_QualSpecList_tok_struct*) _temp1051)->f1; goto _LL1054;}
else{ goto _LL1055;} _LL1055: goto _LL1056; _LL1054: yyzzz= _temp1057; goto
_LL1052; _LL1056:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL1052:;}
return yyzzz;} static unsigned char _temp1060[ 14u]="list_t<var_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{
_temp1060, _temp1060, _temp1060 +  14u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1061= yy1; struct Cyc_List_List*
_temp1067; _LL1063: if(*(( void**) _temp1061) ==  Cyc_IdList_tok){ _LL1068:
_temp1067=(( struct Cyc_IdList_tok_struct*) _temp1061)->f1; goto _LL1064;} else{
goto _LL1065;} _LL1065: goto _LL1066; _LL1064: yyzzz= _temp1067; goto _LL1062;
_LL1066:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1062:;} return yyzzz;}
static unsigned char _temp1070[ 32u]="$(opt_t<var_t>,tqual_t,type_t)@"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{
_temp1070, _temp1070, _temp1070 +  32u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ struct _tuple2* yyzzz;{ void* _temp1071= yy1; struct _tuple2*
_temp1077; _LL1073: if(*(( void**) _temp1071) ==  Cyc_ParamDecl_tok){ _LL1078:
_temp1077=(( struct Cyc_ParamDecl_tok_struct*) _temp1071)->f1; goto _LL1074;}
else{ goto _LL1075;} _LL1075: goto _LL1076; _LL1074: yyzzz= _temp1077; goto
_LL1072; _LL1076:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1072:;}
return yyzzz;} static unsigned char _temp1080[ 40u]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{
_temp1080, _temp1080, _temp1080 +  40u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1081= yy1; struct Cyc_List_List*
_temp1087; _LL1083: if(*(( void**) _temp1081) ==  Cyc_ParamDeclList_tok){
_LL1088: _temp1087=(( struct Cyc_ParamDeclList_tok_struct*) _temp1081)->f1; goto
_LL1084;} else{ goto _LL1085;} _LL1085: goto _LL1086; _LL1084: yyzzz= _temp1087;
goto _LL1082; _LL1086:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1082:;} return yyzzz;} static unsigned char _temp1090[ 107u]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={ Cyc_Core_Failure,{
_temp1090, _temp1090, _temp1090 +  107u}}; struct _tuple16* Cyc_yyget_YY1( void*
yy1){ struct _tuple16* yyzzz;{ void* _temp1091= yy1; struct _tuple16* _temp1097;
_LL1093: if(*(( void**) _temp1091) ==  Cyc_YY1){ _LL1098: _temp1097=(( struct
Cyc_YY1_struct*) _temp1091)->f1; goto _LL1094;} else{ goto _LL1095;} _LL1095:
goto _LL1096; _LL1094: yyzzz= _temp1097; goto _LL1092; _LL1096:( int) _throw((
void*)& Cyc_yyfail_YY1); _LL1092:;} return yyzzz;} static unsigned char
_temp1100[ 15u]="list_t<type_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1100, _temp1100, _temp1100 +  15u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void*
_temp1101= yy1; struct Cyc_List_List* _temp1107; _LL1103: if(*(( void**)
_temp1101) ==  Cyc_TypeList_tok){ _LL1108: _temp1107=(( struct Cyc_TypeList_tok_struct*)
_temp1101)->f1; goto _LL1104;} else{ goto _LL1105;} _LL1105: goto _LL1106;
_LL1104: yyzzz= _temp1107; goto _LL1102; _LL1106:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok);
_LL1102:;} return yyzzz;} static unsigned char _temp1110[ 21u]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{
_temp1110, _temp1110, _temp1110 +  21u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1111= yy1; struct Cyc_List_List*
_temp1117; _LL1113: if(*(( void**) _temp1111) ==  Cyc_DesignatorList_tok){
_LL1118: _temp1117=(( struct Cyc_DesignatorList_tok_struct*) _temp1111)->f1;
goto _LL1114;} else{ goto _LL1115;} _LL1115: goto _LL1116; _LL1114: yyzzz=
_temp1117; goto _LL1112; _LL1116:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1112:;} return yyzzz;} static unsigned char _temp1120[ 13u]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{
_temp1120, _temp1120, _temp1120 +  13u}}; void* Cyc_yyget_Designator_tok( void*
yy1){ void* yyzzz;{ void* _temp1121= yy1; void* _temp1127; _LL1123: if(*(( void**)
_temp1121) ==  Cyc_Designator_tok){ _LL1128: _temp1127=( void*)(( struct Cyc_Designator_tok_struct*)
_temp1121)->f1; goto _LL1124;} else{ goto _LL1125;} _LL1125: goto _LL1126;
_LL1124: yyzzz= _temp1127; goto _LL1122; _LL1126:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL1122:;} return yyzzz;} static unsigned char _temp1130[ 7u]="kind_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{
_temp1130, _temp1130, _temp1130 +  7u}}; void* Cyc_yyget_Kind_tok( void* yy1){
void* yyzzz;{ void* _temp1131= yy1; void* _temp1137; _LL1133: if(*(( void**)
_temp1131) ==  Cyc_Kind_tok){ _LL1138: _temp1137=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1131)->f1; goto _LL1134;} else{ goto _LL1135;} _LL1135: goto _LL1136;
_LL1134: yyzzz= _temp1137; goto _LL1132; _LL1136:( int) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1132:;} return yyzzz;} static unsigned char _temp1140[ 7u]="type_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1140, _temp1140, _temp1140 +  7u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* yyzzz;{ void* _temp1141= yy1; void* _temp1147; _LL1143: if(*(( void**)
_temp1141) ==  Cyc_Type_tok){ _LL1148: _temp1147=( void*)(( struct Cyc_Type_tok_struct*)
_temp1141)->f1; goto _LL1144;} else{ goto _LL1145;} _LL1145: goto _LL1146;
_LL1144: yyzzz= _temp1147; goto _LL1142; _LL1146:( int) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1142:;} return yyzzz;} static unsigned char _temp1150[ 20u]="list_t<attribute_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1150, _temp1150, _temp1150 +  20u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1151= yy1; struct Cyc_List_List*
_temp1157; _LL1153: if(*(( void**) _temp1151) ==  Cyc_AttributeList_tok){
_LL1158: _temp1157=(( struct Cyc_AttributeList_tok_struct*) _temp1151)->f1; goto
_LL1154;} else{ goto _LL1155;} _LL1155: goto _LL1156; _LL1154: yyzzz= _temp1157;
goto _LL1152; _LL1156:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1152:;} return yyzzz;} static unsigned char _temp1160[ 12u]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{
_temp1160, _temp1160, _temp1160 +  12u}}; void* Cyc_yyget_Attribute_tok( void*
yy1){ void* yyzzz;{ void* _temp1161= yy1; void* _temp1167; _LL1163: if(*(( void**)
_temp1161) ==  Cyc_Attribute_tok){ _LL1168: _temp1167=( void*)(( struct Cyc_Attribute_tok_struct*)
_temp1161)->f1; goto _LL1164;} else{ goto _LL1165;} _LL1165: goto _LL1166;
_LL1164: yyzzz= _temp1167; goto _LL1162; _LL1166:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1162:;} return yyzzz;} static unsigned char _temp1170[ 12u]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{
_temp1170, _temp1170, _temp1170 +  12u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
void* yy1){ struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp1171= yy1; struct Cyc_Absyn_Enumfield*
_temp1177; _LL1173: if(*(( void**) _temp1171) ==  Cyc_Enumfield_tok){ _LL1178:
_temp1177=(( struct Cyc_Enumfield_tok_struct*) _temp1171)->f1; goto _LL1174;}
else{ goto _LL1175;} _LL1175: goto _LL1176; _LL1174: yyzzz= _temp1177; goto
_LL1172; _LL1176:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1172:;}
return yyzzz;} static unsigned char _temp1180[ 20u]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1180, _temp1180, _temp1180 +  20u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1181= yy1; struct Cyc_List_List*
_temp1187; _LL1183: if(*(( void**) _temp1181) ==  Cyc_EnumfieldList_tok){
_LL1188: _temp1187=(( struct Cyc_EnumfieldList_tok_struct*) _temp1181)->f1; goto
_LL1184;} else{ goto _LL1185;} _LL1185: goto _LL1186; _LL1184: yyzzz= _temp1187;
goto _LL1182; _LL1186:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1182:;} return yyzzz;} static unsigned char _temp1190[ 14u]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{
_temp1190, _temp1190, _temp1190 +  14u}}; struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok(
void* yy1){ struct Cyc_Core_Opt* yyzzz;{ void* _temp1191= yy1; struct Cyc_Core_Opt*
_temp1197; _LL1193: if(*(( void**) _temp1191) ==  Cyc_TypeOpt_tok){ _LL1198:
_temp1197=(( struct Cyc_TypeOpt_tok_struct*) _temp1191)->f1; goto _LL1194;}
else{ goto _LL1195;} _LL1195: goto _LL1196; _LL1194: yyzzz= _temp1197; goto
_LL1192; _LL1196:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1192:;}
return yyzzz;} static unsigned char _temp1200[ 26u]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{
_temp1200, _temp1200, _temp1200 +  26u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1201= yy1; struct Cyc_List_List*
_temp1207; _LL1203: if(*(( void**) _temp1201) ==  Cyc_Rgnorder_tok){ _LL1208:
_temp1207=(( struct Cyc_Rgnorder_tok_struct*) _temp1201)->f1; goto _LL1204;}
else{ goto _LL1205;} _LL1205: goto _LL1206; _LL1204: yyzzz= _temp1207; goto
_LL1202; _LL1206:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1202:;}
return yyzzz;} struct Cyc_Yyltype{ int timestamp; int first_line; int
first_column; int last_line; int last_column; } ; struct Cyc_Yyltype Cyc_yynewloc(){
return({ struct Cyc_Yyltype _temp1209; _temp1209.timestamp= 0; _temp1209.first_line=
0; _temp1209.first_column= 0; _temp1209.last_line= 0; _temp1209.last_column= 0;
_temp1209;});} struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[
353u]={ 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 124, 2, 2, 106, 122, 118, 2, 102, 103, 113, 116, 104,
117, 110, 121, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 109, 98, 107, 101, 108, 115, 114, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 111,
2, 112, 120, 105, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 99, 119, 100, 123, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97};
static unsigned char _temp1210[ 2u]="$"; static unsigned char _temp1211[ 6u]="error";
static unsigned char _temp1212[ 12u]="$undefined."; static unsigned char
_temp1213[ 5u]="AUTO"; static unsigned char _temp1214[ 9u]="REGISTER"; static
unsigned char _temp1215[ 7u]="STATIC"; static unsigned char _temp1216[ 7u]="EXTERN";
static unsigned char _temp1217[ 8u]="TYPEDEF"; static unsigned char _temp1218[ 5u]="VOID";
static unsigned char _temp1219[ 5u]="CHAR"; static unsigned char _temp1220[ 6u]="SHORT";
static unsigned char _temp1221[ 4u]="INT"; static unsigned char _temp1222[ 5u]="LONG";
static unsigned char _temp1223[ 6u]="FLOAT"; static unsigned char _temp1224[ 7u]="DOUBLE";
static unsigned char _temp1225[ 7u]="SIGNED"; static unsigned char _temp1226[ 9u]="UNSIGNED";
static unsigned char _temp1227[ 6u]="CONST"; static unsigned char _temp1228[ 9u]="VOLATILE";
static unsigned char _temp1229[ 9u]="RESTRICT"; static unsigned char _temp1230[
7u]="STRUCT"; static unsigned char _temp1231[ 6u]="UNION"; static unsigned char
_temp1232[ 5u]="CASE"; static unsigned char _temp1233[ 8u]="DEFAULT"; static
unsigned char _temp1234[ 7u]="INLINE"; static unsigned char _temp1235[ 7u]="SIZEOF";
static unsigned char _temp1236[ 9u]="OFFSETOF"; static unsigned char _temp1237[
3u]="IF"; static unsigned char _temp1238[ 5u]="ELSE"; static unsigned char
_temp1239[ 7u]="SWITCH"; static unsigned char _temp1240[ 6u]="WHILE"; static
unsigned char _temp1241[ 3u]="DO"; static unsigned char _temp1242[ 4u]="FOR";
static unsigned char _temp1243[ 5u]="GOTO"; static unsigned char _temp1244[ 9u]="CONTINUE";
static unsigned char _temp1245[ 6u]="BREAK"; static unsigned char _temp1246[ 7u]="RETURN";
static unsigned char _temp1247[ 5u]="ENUM"; static unsigned char _temp1248[ 8u]="NULL_kw";
static unsigned char _temp1249[ 4u]="LET"; static unsigned char _temp1250[ 6u]="THROW";
static unsigned char _temp1251[ 4u]="TRY"; static unsigned char _temp1252[ 6u]="CATCH";
static unsigned char _temp1253[ 4u]="NEW"; static unsigned char _temp1254[ 9u]="ABSTRACT";
static unsigned char _temp1255[ 9u]="FALLTHRU"; static unsigned char _temp1256[
6u]="USING"; static unsigned char _temp1257[ 10u]="NAMESPACE"; static
unsigned char _temp1258[ 7u]="TUNION"; static unsigned char _temp1259[ 8u]="XTUNION";
static unsigned char _temp1260[ 9u]="FORARRAY"; static unsigned char _temp1261[
5u]="FILL"; static unsigned char _temp1262[ 8u]="CODEGEN"; static unsigned char
_temp1263[ 4u]="CUT"; static unsigned char _temp1264[ 7u]="SPLICE"; static
unsigned char _temp1265[ 7u]="MALLOC"; static unsigned char _temp1266[ 8u]="RMALLOC";
static unsigned char _temp1267[ 7u]="CALLOC"; static unsigned char _temp1268[ 8u]="RCALLOC";
static unsigned char _temp1269[ 9u]="REGION_T"; static unsigned char _temp1270[
9u]="SIZEOF_T"; static unsigned char _temp1271[ 7u]="REGION"; static
unsigned char _temp1272[ 5u]="RNEW"; static unsigned char _temp1273[ 8u]="REGIONS";
static unsigned char _temp1274[ 4u]="GEN"; static unsigned char _temp1275[ 7u]="PTR_OP";
static unsigned char _temp1276[ 7u]="INC_OP"; static unsigned char _temp1277[ 7u]="DEC_OP";
static unsigned char _temp1278[ 8u]="LEFT_OP"; static unsigned char _temp1279[ 9u]="RIGHT_OP";
static unsigned char _temp1280[ 6u]="LE_OP"; static unsigned char _temp1281[ 6u]="GE_OP";
static unsigned char _temp1282[ 6u]="EQ_OP"; static unsigned char _temp1283[ 6u]="NE_OP";
static unsigned char _temp1284[ 7u]="AND_OP"; static unsigned char _temp1285[ 6u]="OR_OP";
static unsigned char _temp1286[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1287[ 11u]="DIV_ASSIGN"; static unsigned char _temp1288[ 11u]="MOD_ASSIGN";
static unsigned char _temp1289[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1290[ 11u]="SUB_ASSIGN"; static unsigned char _temp1291[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1292[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1293[ 11u]="AND_ASSIGN"; static unsigned char _temp1294[ 11u]="XOR_ASSIGN";
static unsigned char _temp1295[ 10u]="OR_ASSIGN"; static unsigned char _temp1296[
9u]="ELLIPSIS"; static unsigned char _temp1297[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1298[ 12u]="COLON_COLON"; static unsigned char _temp1299[ 11u]="IDENTIFIER";
static unsigned char _temp1300[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1301[ 7u]="STRING"; static unsigned char _temp1302[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1303[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1304[ 9u]="TYPE_VAR"; static unsigned char _temp1305[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1306[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1307[ 10u]="ATTRIBUTE"; static unsigned char _temp1308[ 4u]="';'"; static
unsigned char _temp1309[ 4u]="'{'"; static unsigned char _temp1310[ 4u]="'}'";
static unsigned char _temp1311[ 4u]="'='"; static unsigned char _temp1312[ 4u]="'('";
static unsigned char _temp1313[ 4u]="')'"; static unsigned char _temp1314[ 4u]="','";
static unsigned char _temp1315[ 4u]="'_'"; static unsigned char _temp1316[ 4u]="'$'";
static unsigned char _temp1317[ 4u]="'<'"; static unsigned char _temp1318[ 4u]="'>'";
static unsigned char _temp1319[ 4u]="':'"; static unsigned char _temp1320[ 4u]="'.'";
static unsigned char _temp1321[ 4u]="'['"; static unsigned char _temp1322[ 4u]="']'";
static unsigned char _temp1323[ 4u]="'*'"; static unsigned char _temp1324[ 4u]="'@'";
static unsigned char _temp1325[ 4u]="'?'"; static unsigned char _temp1326[ 4u]="'+'";
static unsigned char _temp1327[ 4u]="'-'"; static unsigned char _temp1328[ 4u]="'&'";
static unsigned char _temp1329[ 4u]="'|'"; static unsigned char _temp1330[ 4u]="'^'";
static unsigned char _temp1331[ 4u]="'/'"; static unsigned char _temp1332[ 4u]="'%'";
static unsigned char _temp1333[ 4u]="'~'"; static unsigned char _temp1334[ 4u]="'!'";
static unsigned char _temp1335[ 5u]="prog"; static unsigned char _temp1336[ 17u]="translation_unit";
static unsigned char _temp1337[ 21u]="external_declaration"; static
unsigned char _temp1338[ 20u]="function_definition"; static unsigned char
_temp1339[ 21u]="function_definition2"; static unsigned char _temp1340[ 13u]="using_action";
static unsigned char _temp1341[ 15u]="unusing_action"; static unsigned char
_temp1342[ 17u]="namespace_action"; static unsigned char _temp1343[ 19u]="unnamespace_action";
static unsigned char _temp1344[ 12u]="declaration"; static unsigned char
_temp1345[ 17u]="declaration_list"; static unsigned char _temp1346[ 23u]="declaration_specifiers";
static unsigned char _temp1347[ 24u]="storage_class_specifier"; static
unsigned char _temp1348[ 15u]="attributes_opt"; static unsigned char _temp1349[
11u]="attributes"; static unsigned char _temp1350[ 15u]="attribute_list"; static
unsigned char _temp1351[ 10u]="attribute"; static unsigned char _temp1352[ 15u]="type_specifier";
static unsigned char _temp1353[ 5u]="kind"; static unsigned char _temp1354[ 15u]="type_qualifier";
static unsigned char _temp1355[ 15u]="enum_specifier"; static unsigned char
_temp1356[ 11u]="enum_field"; static unsigned char _temp1357[ 22u]="enum_declaration_list";
static unsigned char _temp1358[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1359[ 16u]="type_params_opt"; static unsigned char _temp1360[
16u]="struct_or_union"; static unsigned char _temp1361[ 24u]="struct_declaration_list";
static unsigned char _temp1362[ 25u]="struct_declaration_list0"; static
unsigned char _temp1363[ 21u]="init_declarator_list"; static unsigned char
_temp1364[ 22u]="init_declarator_list0"; static unsigned char _temp1365[ 16u]="init_declarator";
static unsigned char _temp1366[ 19u]="struct_declaration"; static unsigned char
_temp1367[ 25u]="specifier_qualifier_list"; static unsigned char _temp1368[ 23u]="struct_declarator_list";
static unsigned char _temp1369[ 24u]="struct_declarator_list0"; static
unsigned char _temp1370[ 18u]="struct_declarator"; static unsigned char
_temp1371[ 17u]="tunion_specifier"; static unsigned char _temp1372[ 18u]="tunion_or_xtunion";
static unsigned char _temp1373[ 17u]="tunionfield_list"; static unsigned char
_temp1374[ 18u]="tunionfield_scope"; static unsigned char _temp1375[ 12u]="tunionfield";
static unsigned char _temp1376[ 11u]="declarator"; static unsigned char
_temp1377[ 18u]="direct_declarator"; static unsigned char _temp1378[ 8u]="pointer";
static unsigned char _temp1379[ 13u]="pointer_char"; static unsigned char
_temp1380[ 8u]="rgn_opt"; static unsigned char _temp1381[ 4u]="rgn"; static
unsigned char _temp1382[ 20u]="type_qualifier_list"; static unsigned char
_temp1383[ 20u]="parameter_type_list"; static unsigned char _temp1384[ 9u]="type_var";
static unsigned char _temp1385[ 16u]="optional_effect"; static unsigned char
_temp1386[ 19u]="optional_rgn_order"; static unsigned char _temp1387[ 10u]="rgn_order";
static unsigned char _temp1388[ 16u]="optional_inject"; static unsigned char
_temp1389[ 11u]="effect_set"; static unsigned char _temp1390[ 14u]="atomic_effect";
static unsigned char _temp1391[ 11u]="region_set"; static unsigned char
_temp1392[ 15u]="parameter_list"; static unsigned char _temp1393[ 22u]="parameter_declaration";
static unsigned char _temp1394[ 16u]="identifier_list"; static unsigned char
_temp1395[ 17u]="identifier_list0"; static unsigned char _temp1396[ 12u]="initializer";
static unsigned char _temp1397[ 18u]="array_initializer"; static unsigned char
_temp1398[ 17u]="initializer_list"; static unsigned char _temp1399[ 12u]="designation";
static unsigned char _temp1400[ 16u]="designator_list"; static unsigned char
_temp1401[ 11u]="designator"; static unsigned char _temp1402[ 10u]="type_name";
static unsigned char _temp1403[ 14u]="any_type_name"; static unsigned char
_temp1404[ 15u]="type_name_list"; static unsigned char _temp1405[ 20u]="abstract_declarator";
static unsigned char _temp1406[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1407[ 10u]="statement"; static unsigned char _temp1408[ 18u]="labeled_statement";
static unsigned char _temp1409[ 21u]="expression_statement"; static
unsigned char _temp1410[ 19u]="compound_statement"; static unsigned char
_temp1411[ 16u]="block_item_list"; static unsigned char _temp1412[ 20u]="selection_statement";
static unsigned char _temp1413[ 15u]="switch_clauses"; static unsigned char
_temp1414[ 16u]="switchC_clauses"; static unsigned char _temp1415[ 20u]="iteration_statement";
static unsigned char _temp1416[ 15u]="jump_statement"; static unsigned char
_temp1417[ 8u]="pattern"; static unsigned char _temp1418[ 19u]="tuple_pattern_list";
static unsigned char _temp1419[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1420[ 14u]="field_pattern"; static unsigned char _temp1421[ 19u]="field_pattern_list";
static unsigned char _temp1422[ 20u]="field_pattern_list0"; static unsigned char
_temp1423[ 11u]="expression"; static unsigned char _temp1424[ 22u]="assignment_expression";
static unsigned char _temp1425[ 20u]="assignment_operator"; static unsigned char
_temp1426[ 23u]="conditional_expression"; static unsigned char _temp1427[ 20u]="constant_expression";
static unsigned char _temp1428[ 22u]="logical_or_expression"; static
unsigned char _temp1429[ 23u]="logical_and_expression"; static unsigned char
_temp1430[ 24u]="inclusive_or_expression"; static unsigned char _temp1431[ 24u]="exclusive_or_expression";
static unsigned char _temp1432[ 15u]="and_expression"; static unsigned char
_temp1433[ 20u]="equality_expression"; static unsigned char _temp1434[ 22u]="relational_expression";
static unsigned char _temp1435[ 17u]="shift_expression"; static unsigned char
_temp1436[ 20u]="additive_expression"; static unsigned char _temp1437[ 26u]="multiplicative_expression";
static unsigned char _temp1438[ 16u]="cast_expression"; static unsigned char
_temp1439[ 17u]="unary_expression"; static unsigned char _temp1440[ 15u]="unary_operator";
static unsigned char _temp1441[ 19u]="postfix_expression"; static unsigned char
_temp1442[ 19u]="primary_expression"; static unsigned char _temp1443[ 25u]="argument_expression_list";
static unsigned char _temp1444[ 26u]="argument_expression_list0"; static
unsigned char _temp1445[ 9u]="constant"; static unsigned char _temp1446[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 237u]={{ _temp1210, _temp1210, _temp1210
+  2u},{ _temp1211, _temp1211, _temp1211 +  6u},{ _temp1212, _temp1212,
_temp1212 +  12u},{ _temp1213, _temp1213, _temp1213 +  5u},{ _temp1214,
_temp1214, _temp1214 +  9u},{ _temp1215, _temp1215, _temp1215 +  7u},{ _temp1216,
_temp1216, _temp1216 +  7u},{ _temp1217, _temp1217, _temp1217 +  8u},{ _temp1218,
_temp1218, _temp1218 +  5u},{ _temp1219, _temp1219, _temp1219 +  5u},{ _temp1220,
_temp1220, _temp1220 +  6u},{ _temp1221, _temp1221, _temp1221 +  4u},{ _temp1222,
_temp1222, _temp1222 +  5u},{ _temp1223, _temp1223, _temp1223 +  6u},{ _temp1224,
_temp1224, _temp1224 +  7u},{ _temp1225, _temp1225, _temp1225 +  7u},{ _temp1226,
_temp1226, _temp1226 +  9u},{ _temp1227, _temp1227, _temp1227 +  6u},{ _temp1228,
_temp1228, _temp1228 +  9u},{ _temp1229, _temp1229, _temp1229 +  9u},{ _temp1230,
_temp1230, _temp1230 +  7u},{ _temp1231, _temp1231, _temp1231 +  6u},{ _temp1232,
_temp1232, _temp1232 +  5u},{ _temp1233, _temp1233, _temp1233 +  8u},{ _temp1234,
_temp1234, _temp1234 +  7u},{ _temp1235, _temp1235, _temp1235 +  7u},{ _temp1236,
_temp1236, _temp1236 +  9u},{ _temp1237, _temp1237, _temp1237 +  3u},{ _temp1238,
_temp1238, _temp1238 +  5u},{ _temp1239, _temp1239, _temp1239 +  7u},{ _temp1240,
_temp1240, _temp1240 +  6u},{ _temp1241, _temp1241, _temp1241 +  3u},{ _temp1242,
_temp1242, _temp1242 +  4u},{ _temp1243, _temp1243, _temp1243 +  5u},{ _temp1244,
_temp1244, _temp1244 +  9u},{ _temp1245, _temp1245, _temp1245 +  6u},{ _temp1246,
_temp1246, _temp1246 +  7u},{ _temp1247, _temp1247, _temp1247 +  5u},{ _temp1248,
_temp1248, _temp1248 +  8u},{ _temp1249, _temp1249, _temp1249 +  4u},{ _temp1250,
_temp1250, _temp1250 +  6u},{ _temp1251, _temp1251, _temp1251 +  4u},{ _temp1252,
_temp1252, _temp1252 +  6u},{ _temp1253, _temp1253, _temp1253 +  4u},{ _temp1254,
_temp1254, _temp1254 +  9u},{ _temp1255, _temp1255, _temp1255 +  9u},{ _temp1256,
_temp1256, _temp1256 +  6u},{ _temp1257, _temp1257, _temp1257 +  10u},{
_temp1258, _temp1258, _temp1258 +  7u},{ _temp1259, _temp1259, _temp1259 +  8u},{
_temp1260, _temp1260, _temp1260 +  9u},{ _temp1261, _temp1261, _temp1261 +  5u},{
_temp1262, _temp1262, _temp1262 +  8u},{ _temp1263, _temp1263, _temp1263 +  4u},{
_temp1264, _temp1264, _temp1264 +  7u},{ _temp1265, _temp1265, _temp1265 +  7u},{
_temp1266, _temp1266, _temp1266 +  8u},{ _temp1267, _temp1267, _temp1267 +  7u},{
_temp1268, _temp1268, _temp1268 +  8u},{ _temp1269, _temp1269, _temp1269 +  9u},{
_temp1270, _temp1270, _temp1270 +  9u},{ _temp1271, _temp1271, _temp1271 +  7u},{
_temp1272, _temp1272, _temp1272 +  5u},{ _temp1273, _temp1273, _temp1273 +  8u},{
_temp1274, _temp1274, _temp1274 +  4u},{ _temp1275, _temp1275, _temp1275 +  7u},{
_temp1276, _temp1276, _temp1276 +  7u},{ _temp1277, _temp1277, _temp1277 +  7u},{
_temp1278, _temp1278, _temp1278 +  8u},{ _temp1279, _temp1279, _temp1279 +  9u},{
_temp1280, _temp1280, _temp1280 +  6u},{ _temp1281, _temp1281, _temp1281 +  6u},{
_temp1282, _temp1282, _temp1282 +  6u},{ _temp1283, _temp1283, _temp1283 +  6u},{
_temp1284, _temp1284, _temp1284 +  7u},{ _temp1285, _temp1285, _temp1285 +  6u},{
_temp1286, _temp1286, _temp1286 +  11u},{ _temp1287, _temp1287, _temp1287 +  11u},{
_temp1288, _temp1288, _temp1288 +  11u},{ _temp1289, _temp1289, _temp1289 +  11u},{
_temp1290, _temp1290, _temp1290 +  11u},{ _temp1291, _temp1291, _temp1291 +  12u},{
_temp1292, _temp1292, _temp1292 +  13u},{ _temp1293, _temp1293, _temp1293 +  11u},{
_temp1294, _temp1294, _temp1294 +  11u},{ _temp1295, _temp1295, _temp1295 +  10u},{
_temp1296, _temp1296, _temp1296 +  9u},{ _temp1297, _temp1297, _temp1297 +  11u},{
_temp1298, _temp1298, _temp1298 +  12u},{ _temp1299, _temp1299, _temp1299 +  11u},{
_temp1300, _temp1300, _temp1300 +  17u},{ _temp1301, _temp1301, _temp1301 +  7u},{
_temp1302, _temp1302, _temp1302 +  19u},{ _temp1303, _temp1303, _temp1303 +  18u},{
_temp1304, _temp1304, _temp1304 +  9u},{ _temp1305, _temp1305, _temp1305 +  16u},{
_temp1306, _temp1306, _temp1306 +  18u},{ _temp1307, _temp1307, _temp1307 +  10u},{
_temp1308, _temp1308, _temp1308 +  4u},{ _temp1309, _temp1309, _temp1309 +  4u},{
_temp1310, _temp1310, _temp1310 +  4u},{ _temp1311, _temp1311, _temp1311 +  4u},{
_temp1312, _temp1312, _temp1312 +  4u},{ _temp1313, _temp1313, _temp1313 +  4u},{
_temp1314, _temp1314, _temp1314 +  4u},{ _temp1315, _temp1315, _temp1315 +  4u},{
_temp1316, _temp1316, _temp1316 +  4u},{ _temp1317, _temp1317, _temp1317 +  4u},{
_temp1318, _temp1318, _temp1318 +  4u},{ _temp1319, _temp1319, _temp1319 +  4u},{
_temp1320, _temp1320, _temp1320 +  4u},{ _temp1321, _temp1321, _temp1321 +  4u},{
_temp1322, _temp1322, _temp1322 +  4u},{ _temp1323, _temp1323, _temp1323 +  4u},{
_temp1324, _temp1324, _temp1324 +  4u},{ _temp1325, _temp1325, _temp1325 +  4u},{
_temp1326, _temp1326, _temp1326 +  4u},{ _temp1327, _temp1327, _temp1327 +  4u},{
_temp1328, _temp1328, _temp1328 +  4u},{ _temp1329, _temp1329, _temp1329 +  4u},{
_temp1330, _temp1330, _temp1330 +  4u},{ _temp1331, _temp1331, _temp1331 +  4u},{
_temp1332, _temp1332, _temp1332 +  4u},{ _temp1333, _temp1333, _temp1333 +  4u},{
_temp1334, _temp1334, _temp1334 +  4u},{ _temp1335, _temp1335, _temp1335 +  5u},{
_temp1336, _temp1336, _temp1336 +  17u},{ _temp1337, _temp1337, _temp1337 +  21u},{
_temp1338, _temp1338, _temp1338 +  20u},{ _temp1339, _temp1339, _temp1339 +  21u},{
_temp1340, _temp1340, _temp1340 +  13u},{ _temp1341, _temp1341, _temp1341 +  15u},{
_temp1342, _temp1342, _temp1342 +  17u},{ _temp1343, _temp1343, _temp1343 +  19u},{
_temp1344, _temp1344, _temp1344 +  12u},{ _temp1345, _temp1345, _temp1345 +  17u},{
_temp1346, _temp1346, _temp1346 +  23u},{ _temp1347, _temp1347, _temp1347 +  24u},{
_temp1348, _temp1348, _temp1348 +  15u},{ _temp1349, _temp1349, _temp1349 +  11u},{
_temp1350, _temp1350, _temp1350 +  15u},{ _temp1351, _temp1351, _temp1351 +  10u},{
_temp1352, _temp1352, _temp1352 +  15u},{ _temp1353, _temp1353, _temp1353 +  5u},{
_temp1354, _temp1354, _temp1354 +  15u},{ _temp1355, _temp1355, _temp1355 +  15u},{
_temp1356, _temp1356, _temp1356 +  11u},{ _temp1357, _temp1357, _temp1357 +  22u},{
_temp1358, _temp1358, _temp1358 +  26u},{ _temp1359, _temp1359, _temp1359 +  16u},{
_temp1360, _temp1360, _temp1360 +  16u},{ _temp1361, _temp1361, _temp1361 +  24u},{
_temp1362, _temp1362, _temp1362 +  25u},{ _temp1363, _temp1363, _temp1363 +  21u},{
_temp1364, _temp1364, _temp1364 +  22u},{ _temp1365, _temp1365, _temp1365 +  16u},{
_temp1366, _temp1366, _temp1366 +  19u},{ _temp1367, _temp1367, _temp1367 +  25u},{
_temp1368, _temp1368, _temp1368 +  23u},{ _temp1369, _temp1369, _temp1369 +  24u},{
_temp1370, _temp1370, _temp1370 +  18u},{ _temp1371, _temp1371, _temp1371 +  17u},{
_temp1372, _temp1372, _temp1372 +  18u},{ _temp1373, _temp1373, _temp1373 +  17u},{
_temp1374, _temp1374, _temp1374 +  18u},{ _temp1375, _temp1375, _temp1375 +  12u},{
_temp1376, _temp1376, _temp1376 +  11u},{ _temp1377, _temp1377, _temp1377 +  18u},{
_temp1378, _temp1378, _temp1378 +  8u},{ _temp1379, _temp1379, _temp1379 +  13u},{
_temp1380, _temp1380, _temp1380 +  8u},{ _temp1381, _temp1381, _temp1381 +  4u},{
_temp1382, _temp1382, _temp1382 +  20u},{ _temp1383, _temp1383, _temp1383 +  20u},{
_temp1384, _temp1384, _temp1384 +  9u},{ _temp1385, _temp1385, _temp1385 +  16u},{
_temp1386, _temp1386, _temp1386 +  19u},{ _temp1387, _temp1387, _temp1387 +  10u},{
_temp1388, _temp1388, _temp1388 +  16u},{ _temp1389, _temp1389, _temp1389 +  11u},{
_temp1390, _temp1390, _temp1390 +  14u},{ _temp1391, _temp1391, _temp1391 +  11u},{
_temp1392, _temp1392, _temp1392 +  15u},{ _temp1393, _temp1393, _temp1393 +  22u},{
_temp1394, _temp1394, _temp1394 +  16u},{ _temp1395, _temp1395, _temp1395 +  17u},{
_temp1396, _temp1396, _temp1396 +  12u},{ _temp1397, _temp1397, _temp1397 +  18u},{
_temp1398, _temp1398, _temp1398 +  17u},{ _temp1399, _temp1399, _temp1399 +  12u},{
_temp1400, _temp1400, _temp1400 +  16u},{ _temp1401, _temp1401, _temp1401 +  11u},{
_temp1402, _temp1402, _temp1402 +  10u},{ _temp1403, _temp1403, _temp1403 +  14u},{
_temp1404, _temp1404, _temp1404 +  15u},{ _temp1405, _temp1405, _temp1405 +  20u},{
_temp1406, _temp1406, _temp1406 +  27u},{ _temp1407, _temp1407, _temp1407 +  10u},{
_temp1408, _temp1408, _temp1408 +  18u},{ _temp1409, _temp1409, _temp1409 +  21u},{
_temp1410, _temp1410, _temp1410 +  19u},{ _temp1411, _temp1411, _temp1411 +  16u},{
_temp1412, _temp1412, _temp1412 +  20u},{ _temp1413, _temp1413, _temp1413 +  15u},{
_temp1414, _temp1414, _temp1414 +  16u},{ _temp1415, _temp1415, _temp1415 +  20u},{
_temp1416, _temp1416, _temp1416 +  15u},{ _temp1417, _temp1417, _temp1417 +  8u},{
_temp1418, _temp1418, _temp1418 +  19u},{ _temp1419, _temp1419, _temp1419 +  20u},{
_temp1420, _temp1420, _temp1420 +  14u},{ _temp1421, _temp1421, _temp1421 +  19u},{
_temp1422, _temp1422, _temp1422 +  20u},{ _temp1423, _temp1423, _temp1423 +  11u},{
_temp1424, _temp1424, _temp1424 +  22u},{ _temp1425, _temp1425, _temp1425 +  20u},{
_temp1426, _temp1426, _temp1426 +  23u},{ _temp1427, _temp1427, _temp1427 +  20u},{
_temp1428, _temp1428, _temp1428 +  22u},{ _temp1429, _temp1429, _temp1429 +  23u},{
_temp1430, _temp1430, _temp1430 +  24u},{ _temp1431, _temp1431, _temp1431 +  24u},{
_temp1432, _temp1432, _temp1432 +  15u},{ _temp1433, _temp1433, _temp1433 +  20u},{
_temp1434, _temp1434, _temp1434 +  22u},{ _temp1435, _temp1435, _temp1435 +  17u},{
_temp1436, _temp1436, _temp1436 +  20u},{ _temp1437, _temp1437, _temp1437 +  26u},{
_temp1438, _temp1438, _temp1438 +  16u},{ _temp1439, _temp1439, _temp1439 +  17u},{
_temp1440, _temp1440, _temp1440 +  15u},{ _temp1441, _temp1441, _temp1441 +  19u},{
_temp1442, _temp1442, _temp1442 +  19u},{ _temp1443, _temp1443, _temp1443 +  25u},{
_temp1444, _temp1444, _temp1444 +  26u},{ _temp1445, _temp1445, _temp1445 +  9u},{
_temp1446, _temp1446, _temp1446 +  20u}}; static short Cyc_yyr1[ 410u]={ 0, 125,
126, 126, 126, 126, 126, 126, 126, 127, 127, 128, 128, 128, 128, 129, 129, 130,
131, 132, 133, 134, 134, 134, 134, 135, 135, 136, 136, 136, 136, 136, 136, 136,
136, 137, 137, 137, 137, 137, 137, 137, 138, 138, 139, 140, 140, 141, 141, 141,
141, 141, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142, 142,
142, 142, 142, 142, 143, 143, 144, 144, 144, 145, 145, 145, 146, 146, 147, 147,
148, 148, 148, 148, 148, 149, 149, 149, 150, 150, 151, 152, 152, 153, 154, 154,
155, 155, 156, 157, 157, 157, 157, 158, 159, 159, 160, 160, 160, 161, 161, 161,
161, 162, 162, 163, 163, 163, 163, 164, 164, 164, 165, 165, 166, 166, 167, 167,
167, 167, 167, 167, 167, 167, 167, 167, 167, 167, 168, 168, 168, 168, 169, 169,
169, 169, 169, 170, 170, 171, 171, 172, 172, 173, 173, 173, 173, 174, 174, 175,
175, 176, 176, 177, 177, 178, 178, 179, 179, 180, 180, 180, 180, 181, 181, 182,
182, 183, 183, 183, 184, 185, 185, 186, 186, 187, 187, 187, 187, 188, 188, 188,
188, 189, 190, 190, 191, 191, 192, 192, 193, 193, 193, 193, 193, 194, 194, 195,
195, 195, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 197, 197,
197, 197, 197, 197, 197, 197, 197, 197, 198, 199, 199, 200, 200, 201, 201, 201,
201, 201, 201, 202, 202, 202, 202, 202, 203, 203, 203, 203, 203, 203, 204, 204,
204, 204, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205,
205, 206, 206, 206, 206, 206, 206, 206, 206, 207, 207, 207, 207, 207, 207, 207,
207, 207, 207, 207, 207, 207, 207, 208, 208, 209, 209, 210, 210, 211, 212, 212,
213, 213, 214, 214, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 216,
216, 216, 216, 216, 216, 216, 217, 218, 218, 219, 219, 220, 220, 221, 221, 222,
222, 223, 223, 223, 224, 224, 224, 224, 224, 225, 225, 225, 226, 226, 226, 227,
227, 227, 227, 228, 228, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229, 229,
229, 229, 229, 229, 229, 230, 230, 230, 231, 231, 231, 231, 231, 231, 231, 231,
231, 231, 231, 231, 231, 231, 232, 232, 232, 232, 232, 232, 232, 232, 232, 233,
234, 234, 235, 235, 235, 235, 236, 236}; static short Cyc_yyr2[ 410u]={ 0, 1, 2,
3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4, 3, 4, 2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2, 3, 2,
3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1, 0, 1, 6, 1, 3, 1, 1, 4, 4, 8, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4, 4, 1, 1, 1, 1, 1, 5, 2, 4, 1, 3, 1, 3, 4, 6,
6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3, 1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3,
6, 4, 3, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2, 6, 1, 2, 1, 3, 3, 4, 4, 5, 4, 4, 4, 2,
2, 1, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0, 1, 1, 1, 1, 2, 3, 5, 5, 7, 1, 3, 0, 2, 0, 2,
3, 5, 0, 1, 1, 3, 2, 3, 4, 1, 1, 3, 1, 3, 2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4, 8, 1,
2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 4, 3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4, 3,
5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2, 2, 3, 1, 2, 2, 3, 1, 2, 1, 2, 1, 2, 5,
7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3, 4, 5, 5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7,
8, 9, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1, 1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1,
3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5,
5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1,
3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 6, 6, 5, 4, 6, 9, 11, 1, 1, 1, 1, 4, 3,
4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4, 5, 1, 1, 3, 1, 1, 1, 1,
1, 1}; static short Cyc_yydefact[ 831u]={ 0, 139, 35, 36, 37, 38, 40, 52, 54, 55,
56, 57, 58, 59, 60, 61, 72, 73, 74, 90, 91, 42, 0, 0, 41, 0, 0, 115, 116, 0, 0,
408, 159, 409, 87, 0, 53, 0, 144, 145, 148, 1, 0, 9, 0, 0, 10, 0, 42, 42, 42, 62,
63, 0, 64, 0, 0, 126, 0, 149, 65, 128, 39, 0, 33, 43, 0, 76, 289, 408, 285, 288,
287, 0, 283, 0, 0, 0, 0, 0, 182, 0, 290, 17, 19, 0, 0, 0, 0, 66, 0, 0, 0, 0, 2,
0, 0, 0, 0, 21, 0, 95, 96, 98, 27, 29, 31, 87, 0, 87, 152, 0, 151, 87, 38, 0, 25,
0, 0, 11, 161, 0, 0, 137, 127, 42, 150, 138, 0, 0, 34, 80, 0, 78, 0, 0, 297, 296,
286, 295, 24, 0, 0, 0, 0, 0, 42, 42, 200, 202, 0, 0, 70, 71, 160, 207, 0, 129, 0,
0, 177, 0, 0, 407, 0, 0, 0, 0, 0, 0, 0, 0, 0, 87, 0, 0, 404, 394, 405, 406, 0, 0,
0, 0, 377, 0, 375, 376, 0, 308, 321, 329, 331, 333, 335, 337, 339, 342, 347, 350,
353, 357, 0, 359, 378, 393, 392, 0, 3, 0, 5, 0, 22, 0, 0, 0, 12, 28, 30, 32, 86,
0, 92, 93, 0, 85, 87, 0, 113, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
408, 235, 237, 0, 243, 239, 0, 241, 224, 225, 226, 0, 227, 228, 229, 0, 306, 26,
13, 98, 167, 183, 0, 0, 163, 161, 0, 0, 130, 0, 140, 0, 0, 0, 77, 0, 0, 284, 299,
0, 298, 184, 0, 0, 297, 0, 203, 175, 0, 101, 103, 161, 0, 209, 201, 210, 68, 0,
69, 89, 0, 88, 0, 179, 0, 181, 67, 0, 0, 367, 0, 323, 357, 0, 324, 325, 0, 0, 0,
0, 0, 0, 0, 0, 0, 360, 361, 0, 0, 0, 0, 363, 364, 362, 146, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 311, 312, 313, 314, 315, 316, 317, 318, 319,
320, 310, 0, 365, 0, 386, 387, 0, 0, 0, 396, 0, 0, 147, 18, 0, 20, 0, 97, 99,
186, 185, 14, 0, 82, 94, 0, 0, 105, 106, 108, 0, 112, 87, 121, 0, 0, 0, 0, 0, 0,
0, 276, 277, 278, 0, 0, 280, 0, 0, 232, 233, 0, 0, 0, 0, 244, 240, 98, 242, 238,
236, 0, 168, 0, 0, 0, 174, 162, 169, 132, 0, 0, 0, 163, 134, 136, 135, 131, 153,
142, 141, 0, 48, 47, 0, 45, 81, 328, 79, 75, 292, 0, 23, 293, 0, 0, 0, 0, 196,
301, 304, 0, 303, 0, 0, 0, 204, 102, 104, 0, 163, 0, 213, 0, 211, 161, 0, 0, 223,
206, 208, 178, 0, 0, 0, 187, 191, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 395,
402, 0, 401, 330, 0, 332, 334, 336, 338, 340, 341, 345, 346, 343, 344, 348, 349,
351, 352, 354, 355, 356, 309, 384, 385, 380, 0, 382, 383, 0, 0, 0, 4, 6, 0, 109,
100, 0, 0, 0, 114, 123, 122, 0, 0, 117, 0, 0, 0, 0, 0, 0, 0, 0, 275, 279, 0, 0,
0, 0, 231, 0, 234, 0, 15, 307, 161, 0, 171, 0, 0, 0, 164, 133, 167, 155, 154,
143, 7, 0, 0, 0, 300, 199, 0, 302, 195, 197, 294, 0, 291, 205, 176, 218, 0, 212,
215, 0, 163, 0, 214, 0, 366, 0, 0, 188, 0, 192, 390, 391, 0, 371, 0, 0, 0, 0, 0,
0, 0, 0, 358, 398, 0, 0, 381, 379, 399, 0, 0, 84, 107, 110, 83, 111, 124, 121,
121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 250, 281, 0, 0, 0, 16, 163, 0, 172, 170, 0,
163, 0, 0, 0, 0, 44, 46, 198, 305, 217, 220, 0, 222, 221, 216, 0, 0, 0, 189, 193,
0, 0, 0, 0, 326, 327, 370, 400, 0, 403, 322, 397, 0, 120, 119, 245, 0, 250, 260,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 282, 0, 0, 157, 173, 165, 156, 161, 0, 49,
50, 219, 368, 369, 0, 194, 372, 0, 0, 388, 0, 0, 0, 256, 0, 0, 262, 0, 0, 0, 270,
0, 0, 0, 0, 0, 0, 0, 0, 249, 0, 230, 0, 163, 0, 0, 0, 0, 389, 0, 246, 0, 0, 0,
247, 261, 263, 264, 0, 272, 271, 0, 266, 0, 0, 0, 0, 250, 251, 0, 166, 158, 0, 0,
0, 0, 125, 0, 0, 248, 265, 273, 267, 268, 0, 0, 250, 252, 0, 0, 190, 373, 0, 256,
257, 269, 250, 253, 274, 51, 0, 256, 258, 250, 254, 374, 259, 255, 0, 0, 0};
static short Cyc_yydefgoto[ 112u]={ 828, 41, 42, 43, 249, 44, 383, 45, 385, 46,
215, 47, 48, 64, 65, 453, 454, 49, 154, 50, 51, 131, 132, 52, 89, 53, 221, 222,
100, 101, 102, 223, 148, 395, 396, 397, 54, 55, 554, 555, 556, 56, 57, 58, 59,
125, 111, 449, 478, 60, 479, 440, 583, 432, 436, 437, 293, 270, 160, 79, 80, 495,
388, 496, 497, 466, 467, 149, 155, 156, 480, 300, 252, 253, 254, 255, 256, 257,
719, 773, 258, 259, 283, 284, 285, 469, 470, 471, 260, 261, 370, 189, 457, 190,
191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 512, 513,
205, 206}; static short Cyc_yypact[ 831u]={ 2078, - -32768, - -32768, - -32768,
- -32768, 84, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - 4, 222, 905, - -32768, - 50, 133, - -32768, - -32768, 165, 186, -
-32768, 218, - -32768, 212, 500, - -32768, 244, 249, 272, - -32768, - -32768,
1962, - -32768, 19, 107, - -32768, 227, - 4, - 4, - 4, - -32768, - -32768, 182,
- -32768, 55, 674, 80, 131, 16, - -32768, - 50, 274, 280, 2496, - -32768, - 50,
292, - -32768, 240, - -32768, - -32768, - -32768, 1120, - -32768, 291, 313, 343,
1120, 342, 354, 365, 161, - -32768, - -32768, 4723, 4723, 145, 4723, - -32768,
361, 4889, 4275, 4275, - -32768, 1962, 2193, 1962, 2193, - -32768, 376, 379, -
-32768, 2373, 2496, 2496, 2496, 212, 4889, 212, - -32768, - 50, - -32768, 217,
397, 1359, - -32768, 674, 227, - -32768, 1898, 4723, 2963, - -32768, 80, - 4, -
-32768, - -32768, 2193, 389, - -32768, 396, 369, 405, - 50, 407, 1120, - -32768,
- -32768, - -32768, - -32768, 425, 4275, 53, 415, 203, - 4, - 4, 199, - -32768,
- 22, 91, - -32768, - -32768, - -32768, 412, - 7, - -32768, 514, 260, - -32768,
4513, 419, - -32768, 4275, 4349, 435, 440, 442, 445, 447, 449, 454, 212, 4587,
4587, - -32768, - -32768, - -32768, - -32768, 1720, 465, 4661, 4661, - -32768,
4661, - -32768, - -32768, 476, - -32768, - 40, 510, 472, 473, 480, 430, 63, 436,
391, 90, - -32768, 671, 4661, 597, - -32768, - -32768, 29, 519, - -32768, 521, -
-32768, 524, - -32768, 500, 3045, 674, - -32768, - -32768, - -32768, - -32768,
501, 526, 4889, - -32768, 459, 531, 212, - 50, 533, - -32768, 532, 81, 535, 2553,
537, 563, 577, 599, 3127, 2553, 326, 598, 2553, 2553, - 2, 594, - -32768, -
-32768, 603, 1481, 1481, 227, 1481, - -32768, - -32768, - -32768, 609, - -32768,
- -32768, - -32768, 241, - -32768, - -32768, - -32768, 620, 635, - -32768, 36,
624, 630, 253, 626, 33, - -32768, 628, 623, 641, 2, - 50, - -32768, 4275, 643, -
-32768, - -32768, 629, 654, - -32768, 255, 706, 1120, 4723, - -32768, 655, 645,
4889, 4889, 2313, 3209, 56, - -32768, 154, - -32768, 36, - -32768, - -32768,
4723, - -32768, 966, - -32768, 348, - -32768, - -32768, 4889, 1720, - -32768,
4889, - -32768, - -32768, 2635, - -32768, 687, 4275, 2253, 4275, 4275, 4275,
4275, 4275, 663, 1720, - -32768, - -32768, 1481, 664, 416, 4275, - -32768, -
-32768, - -32768, - -32768, 4661, 4275, 4661, 4661, 4661, 4661, 4661, 4661, 4661,
4661, 4661, 4661, 4661, 4661, 4661, 4661, 4661, 4661, 4661, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, 4275, - -32768, 209, - -32768, - -32768, 3291, 247, 4275, - -32768,
2881, 659, - -32768, - -32768, 1962, - -32768, 1962, - -32768, - -32768, -
-32768, - -32768, - -32768, 4889, - -32768, - -32768, 4275, 673, 665, - -32768,
667, 4889, - -32768, 212, 518, 4275, 672, 4275, 4275, 747, 1603, 680, - -32768,
- -32768, - -32768, 268, 739, - -32768, 3373, 2436, - -32768, - -32768, 2553,
688, 2553, 1837, - -32768, - -32768, 2373, - -32768, - -32768, - -32768, 4275, -
-32768, 4889, 681, 281, - -32768, - -32768, 668, - -32768, 691, 683, 4836, 630,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 623, 1962, - -32768,
685, 686, 684, - -32768, - -32768, - -32768, - -32768, - -32768, 1120, - -32768,
- -32768, 701, 4275, 1120, 144, - -32768, - -32768, - -32768, 692, 689, 694, -
13, 697, - -32768, - -32768, - -32768, 699, 630, 700, - -32768, 682, 154, 4783,
4723, 3455, - -32768, - -32768, 412, - -32768, 707, 696, 725, - -32768, - -32768,
349, 3045, 423, 712, 500, 432, 714, 716, 717, 460, 4889, 719, 727, 4431, -
-32768, - -32768, 722, 724, 510, 135, 472, 473, 480, 430, 63, 63, 436, 436, 436,
436, 391, 391, 90, 90, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, 726, - -32768, - -32768, 179, 363, 4723, - -32768, - -32768,
731, - -32768, - -32768, 459, 4275, 733, - -32768, - -32768, - -32768, 735, - 50,
303, 467, 4275, 477, 479, 740, 3537, 3619, 312, - -32768, - -32768, 738, 743,
741, 745, - -32768, 742, - -32768, 674, - -32768, - -32768, 748, 4723, - -32768,
749, 36, 750, - -32768, - -32768, 134, - -32768, - -32768, - -32768, - -32768,
99, 744, 2, - -32768, - -32768, 746, - -32768, - -32768, - -32768, - -32768, 788,
- -32768, - -32768, - -32768, - -32768, 753, - -32768, - -32768, 756, 630, 58, -
-32768, 757, 761, 498, 754, - -32768, 2717, - -32768, - -32768, - -32768, 674, -
-32768, 4275, 827, 4275, 4349, 759, 761, 765, 2881, - -32768, - -32768, 4275,
4275, - -32768, - -32768, - -32768, 2881, 378, - -32768, - -32768, - -32768, -
-32768, - -32768, 167, 74, 518, 2553, 493, 771, 2553, 4275, 3701, 314, 3783, 319,
3865, 584, - -32768, 773, 4275, 785, - -32768, 630, 45, - -32768, - -32768, 782,
630, 4889, 775, 779, 781, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, 783, - -32768, - -32768, - -32768, 784, 786, 4275, - -32768, - -32768,
3045, 540, 789, 791, - -32768, 687, - -32768, - -32768, 385, - -32768, - -32768,
- -32768, 794, - -32768, - -32768, 857, 803, 584, - -32768, 543, 2553, 545, 3947,
2553, 547, 4029, 4111, 328, 1120, 798, 809, - -32768, 344, 2553, - -32768, -
-32768, 799, - -32768, 748, 821, - -32768, - -32768, - -32768, - -32768, -
-32768, 219, - -32768, - -32768, 4889, 888, - -32768, 2799, 4889, 2553, 632, 814,
817, - -32768, 2553, 2553, 553, - -32768, 2553, 2553, 555, 2553, 557, 4193, - 42,
1481, - -32768, 4275, - -32768, 691, 630, 812, 4275, 815, 818, - -32768, 562, -
-32768, 4275, 810, 822, - -32768, - -32768, - -32768, - -32768, 2553, - -32768,
- -32768, 2553, - -32768, 2553, 2553, 564, 4275, 1084, - -32768, 566, - -32768,
- -32768, 831, 390, 823, 4889, - -32768, 246, 1481, - -32768, - -32768, - -32768,
- -32768, - -32768, 2553, 305, 584, - -32768, 2553, 824, - -32768, - -32768, 825,
1237, - -32768, - -32768, 1084, - -32768, - -32768, - -32768, 826, 632, - -32768,
584, - -32768, - -32768, - -32768, - -32768, 924, 925, - -32768}; static short
Cyc_yypgoto[ 112u]={ - -32768, 28, - -32768, 610, - -32768, - -32768, - -32768,
- -32768, - -32768, - 52, - 27, - 56, - -32768, 406, - 48, 339, - -32768, 93, -
-32768, - 65, - -32768, - -32768, - 110, - -32768, - 9, - -32768, - 107, -
-32768, - -32768, - -32768, 720, 715, - 37, - -32768, - -32768, 386, - -32768, -
-32768, 27, - -32768, - -32768, - 5, - 30, - 80, - -32768, - -32768, 882, -
-32768, - 106, - 49, - 105, - 360, 183, 359, 366, 644, - 393, - 88, - 290, 828,
- -32768, - 202, - 164, - 348, - 270, - -32768, 483, - 175, - 69, - 108, - 12, -
94, 188, - -32768, - -32768, - 29, - 212, - -32768, - 614, - 525, - -32768, -
-32768, - 21, 661, - -32768, 351, - -32768, - -32768, 152, 215, - -32768, - 104,
- 369, - 158, 612, 611, 613, 614, 615, 370, 148, 367, 371, - 71, 690, - -32768,
- -32768, - -32768, - 331, - -32768, - -32768, 11}; static short Cyc_yytable[
4996u]={ 118, 319, 81, 159, 116, 333, 112, 320, 130, 123, 112, 61, 387, 272, 268,
269, 150, 151, 465, 451, 147, 147, 490, 147, 281, 546, 147, 119, 124, 117, 90,
541, 786, 67, 82, 340, 83, 424, 425, 31, 427, 580, 103, 147, 537, 33, 61, 118,
217, 218, 219, 116, 135, 61, 158, 147, 147, 139, 61, 251, 316, 118, 304, 250,
109, 262, 113, 787, 298, 61, 94, 224, 127, 143, 216, 341, 123, 133, 309, 552,
553, 603, 586, 158, 82, 569, 301, 420, 263, 82, 602, 452, 744, 63, 302, 595, 292,
305, 220, 433, 225, 306, 444, 302, 228, 421, 61, 61, 61, 61, 32, 336, 337, 264,
338, 147, 378, 95, 96, 605, 508, 110, 226, 208, 209, 210, 211, 681, 379, 61, 32,
371, 1, 348, 349, 434, 299, 305, 491, 61, 492, 445, 577, 380, 31, 133, 310, 82,
724, 32, 33, 490, 288, 308, 507, 289, 276, 147, 296, 118, 110, 302, 305, 262,
328, 442, 682, 297, 455, 61, 350, 351, 404, 807, - 118, 62, 456, 63, 146, 146,
642, 146, 120, 405, 146, 224, 390, 121, 671, 672, 673, 122, 817, 251, 251, 448,
251, 250, 250, 303, 250, 146, 824, 356, 483, 97, 98, 302, 264, 827, 447, 357,
358, 146, 146, 483, 298, 400, 435, 398, 31, 473, 84, 431, 61, 147, 33, 309, 1,
147, 147, 147, 267, 35, 152, 61, 489, - 161, 401, 430, 147, 153, 147, - 161, 634,
597, 426, 147, 147, 680, 147, 63, 487, 435, 463, 464, 484, 476, 477, 158, - 87,
485, 61, - 87, 147, 486, 500, 468, 88, - 87, 158, 31, 85, 146, 88, 158, 251, 33,
107, 124, 250, 108, 697, 430, 545, 530, 531, 532, 822, 133, 456, 636, 550, 86,
287, 618, 826, 32, 534, 82, 82, 296, 90, 291, 723, 535, 87, 188, 207, 726, 297,
31, 38, 39, 40, 146, 31, 33, 61, 88, 61, 66, 33, 430, 88, 99, 147, 227, 765, 35,
465, 627, 334, 61, 147, 159, 538, 274, - 183, 429, 38, 39, 40, 539, - 183, 430,
91, 689, 92, 1, 430, 267, 118, 461, 224, 813, 563, 441, 147, 430, 456, 118, 224,
311, 312, 570, 566, 147, 689, 588, 118, 93, 430, 128, 116, 32, 147, 610, 608,
609, 727, 579, 129, 146, 587, 292, 158, 146, 146, 146, 413, 134, 551, 136, 61,
158, 61, 575, 146, 574, 146, 646, 137, 791, 158, 146, 146, 647, 146, 430, 657,
543, 711, 544, 816, 688, 430, 714, 430, 147, 147, 407, 146, 430, 415, 292, 756,
414, 416, 389, 418, 419, 430, 138, 639, 487, 688, 31, 631, 593, 140, 147, 760,
33, 596, 456, - 209, 158, 430, 616, 307, - 209, - 209, 617, 104, 105, 106, - 209,
141, 297, 1, 61, 693, 637, 157, 334, 142, 638, 694, 279, 689, 82, 664, 498, 212,
501, 82, 147, 589, 505, 669, 334, 305, 213, 146, 739, 700, 735, 229, 740, 810,
277, 146, 515, 430, 621, 522, 523, 524, 525, 278, 1, 346, 347, 352, 353, 280,
354, 355, 665, 282, 61, 482, 147, 286, 1, 146, 290, 118, 510, 430, 315, 262, 552,
553, 146, 619, 430, 302, 540, 699, 275, 435, 389, 146, 622, 430, 321, 688, 502,
503, 504, 322, 398, 323, 663, 788, 324, 31, 325, 511, 326, 294, 295, 33, 557,
327, 559, 560, 61, 564, 35, 766, 626, 430, 118, 645, 335, 394, 116, 648, 430, 38,
39, 40, 806, 339, 146, 146, 468, 650, 430, 651, 430, 342, 533, 814, 684, 685, 31,
511, 343, 216, 344, 389, 33, 705, 430, 345, 146, 391, 821, 35, 31, 823, 147, 717,
718, 571, 33, 573, 82, - 180, 38, 39, 40, 307, - 180, - 180, 381, 812, 382, 763,
- 180, 384, 297, 392, 38, 39, 40, 399, 511, 402, 158, 403, 146, 701, 406, 511,
408, 16, 17, 18, 736, 430, 576, 745, 430, 747, 430, 751, 430, 409, 769, 771, 772,
778, 430, 781, 430, 783, 430, 372, 373, 374, 796, 312, 804, 430, 808, 430, 146,
147, 702, 703, 410, 147, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 757, 411, 21, 375, 417, 612, 251, 422, 158, 423, 250, 376, 377,
428, 649, 22, 389, 23, 654, 656, 520, 521, 24, 526, 527, 214, 27, 28, 431, 528,
529, 438, 82, 443, 147, 251, 459, 29, 30, 250, 38, 39, 40, 439, 446, 450, 251,
458, 68, 475, 250, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 251, 460,
474, 251, 250, 340, 146, 250, 506, 542, 509, 32, 548, 34, 547, 369, 115, 558,
690, 549, 561, 565, 36, 37, 567, 572, 578, 581, 582, 584, 590, 592, 591, 594, 32,
599, 600, 607, 31, 70, 601, 71, 72, 614, 33, 604, 606, 708, 710, 462, 713, 73,
716, 613, 74, 75, 721, 615, 620, 463, 464, 623, 76, 624, 625, 628, 77, 78, 632,
68, 629, 633, 635, 146, 640, 389, 643, 146, 644, 704, 658, 734, 707, 692, 659,
652, 661, 660, 389, 267, 674, 698, 666, 662, 314, 691, 389, 317, 317, 678, 668,
676, 679, 630, 686, 695, 749, 330, 331, 753, 755, 696, 683, 706, 720, 317, 317,
722, 317, 725, 31, 70, 728, 71, 72, 729, 33, 730, 742, 731, 732, 146, 733, 73,
737, 317, 74, 75, 738, 741, 746, 463, 464, 750, 76, 743, 762, 389, 77, 78, 758,
785, 759, 761, 764, 789, 767, 774, 775, 792, 793, 794, 798, 795, 809, 799, 797,
829, 830, 811, 819, 820, 825, 770, 675, 499, 386, 641, 776, 777, 393, 805, 779,
780, 126, 782, 68, 670, 790, 488, 667, 271, 598, 472, 677, 514, 516, 0, 389, 517,
0, 518, 0, 519, 0, 0, 0, 0, 0, 800, 1, 0, 801, 317, 802, 803, 0, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 815, 0, 69, 70, 818, 71, 72, 0,
33, 0, 0, 22, 0, 0, 0, 73, 0, 0, 74, 75, 0, 0, 27, 28, 0, 0, 76, 0, 0, 0, 77, 78,
0, 29, 30, 0, 0, 0, 317, 0, 317, 317, 317, 317, 317, 317, 317, 317, 317, 317,
317, 317, 317, 317, 317, 317, 317, 0, 0, 0, 265, 0, 0, 31, 0, 0, 0, 0, 32, 33,
34, 0, 267, 0, 0, 0, 307, - 161, 0, 36, 37, 0, 0, - 161, 0, 297, 0, 38, 39, 40,
0, 0, 317, 0, 0, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 717, 718, 21, 161, 162, 230, 0, 231, 232, 233, 234, 235, 236, 237, 238,
22, 163, 23, 164, 239, 0, 165, 24, 240, 0, 0, 27, 28, 241, 166, 167, 242, 243,
168, 169, 170, 171, 29, 30, 244, 172, 0, 173, 0, 174, 175, 0, 0, 317, 0, 0, 0,
68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 245, 176, 177, 178, 179, 32, 33,
34, 0, 246, 115, 0, 0, 180, 0, 0, 36, 248, 0, 0, 0, 0, 0, 0, 182, 0, 317, 183,
184, 185, 0, 0, 0, 0, 186, 187, 31, 70, 0, 71, 72, 0, 33, 0, 0, 0, 0, 0, 0, 73,
0, 0, 74, 75, 0, 0, 0, 0, 0, 0, 76, 0, 0, 0, 77, 78, 317, 2, 3, 4, 114, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 771, 772, 21, 161, 162, 230, 0,
231, 232, 233, 234, 235, 236, 237, 238, 22, 163, 23, 164, 239, 0, 165, 24, 240,
0, 0, 27, 28, 241, 166, 167, 242, 243, 168, 169, 170, 171, 29, 30, 244, 172, 0,
173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 317, 0, 0, 0, 0, 0, 0, 0, 317,
0, 245, 176, 177, 178, 179, 32, 33, 34, 0, 246, 115, 0, 0, 180, 0, 0, 36, 248, 0,
0, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 0, 0, 0, 186, 187, 2, 3, 4, 114, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 161, 162, 230, 0,
231, 232, 233, 234, 235, 236, 237, 238, 22, 163, 23, 164, 239, 0, 165, 24, 240,
0, 0, 27, 28, 241, 166, 167, 242, 243, 168, 169, 170, 171, 29, 30, 244, 172, 0,
173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
245, 176, 177, 178, 179, 32, 33, 34, 0, 246, 115, 247, 0, 180, 0, 0, 36, 248, 0,
0, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 0, 0, 0, 186, 187, 2, 3, 4, 114, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 161, 162, 230, 0,
231, 232, 233, 234, 235, 236, 237, 238, 22, 163, 23, 164, 239, 0, 165, 24, 240,
0, 0, 27, 28, 241, 166, 167, 242, 243, 168, 169, 170, 171, 29, 30, 244, 172, 0,
173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
245, 176, 177, 178, 179, 32, 33, 34, 0, 246, 115, 0, 0, 180, 0, 0, 36, 248, 0, 0,
0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 0, 0, 0, 186, 187, 2, 3, 4, 114, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 161, 162, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 22, 163, 23, 164, 0, 0, 165, 24, 0, 0, 0, 27, 28, 0, 166, 167,
0, 0, 168, 169, 170, 171, 29, 30, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 32, 33, 34,
0, 562, 0, 0, 0, 180, 0, 0, 36, 248, 0, 0, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185,
0, 0, 0, 0, 186, 187, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
0, 161, 162, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 163, 0, 164, 0, 0, 165, 0, 0, 0,
0, 27, 28, 0, 166, 167, 0, 0, 168, 169, 170, 171, 29, 30, 0, 172, 0, 173, 0, 174,
175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177,
178, 179, 32, 33, 34, 0, 0, 332, 0, 0, 180, 0, 0, 36, 248, 0, 0, 0, 0, 0, 0, 182,
0, 0, 183, 184, 185, 0, 0, 0, 0, 186, 187, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 0, 0, 161, 162, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 163, 0, 164,
0, 0, 165, 0, 0, 0, 0, 27, 28, 0, 166, 167, 0, 0, 168, 169, 170, 171, 29, 30, 0,
172, 0, 173, 0, 174, 175, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 32, 33, 34, 0, 22, 0, 0, 0, 180, 0, 0,
36, 248, 0, 0, 27, 28, 0, 0, 182, 0, 0, 183, 184, 185, 0, 29, 30, 0, 186, 187, -
8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 265,
0, 21, 266, 0, 0, 0, 0, 32, 0, 34, 0, 267, 0, 0, 22, 0, 23, 0, 36, 37, 0, 24, 0,
25, 26, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 32, 33, 34,
0, 0, 0, - 8, 0, 35, 0, 0, 36, 37, 0, 0, 0, 0, 0, 0, 38, 39, 40, - 8, 1, 0, 2, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0,
0, 0, 0, 0, 0, 0, 29, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 32, 33, 34, 0, 0, 0, 0, 0, 35, 0,
0, 36, 37, 0, 0, 0, 0, 0, 0, 38, 39, 40, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30,
1, 0, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
21, 0, 0, 0, 0, 31, 0, 0, 0, 0, 32, 33, 34, 22, 0, 0, - 8, 0, 35, 0, 24, 36, 37,
0, 27, 28, 0, 0, 0, 38, 39, 40, 0, 0, 0, 29, 30, 0, 0, 0, 0, 0, 0, 0, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0,
32, 33, 34, 22, 0, 0, 0, 0, 35, 0, 0, 36, 37, 0, 27, 28, 0, 0, 0, 38, 39, 40, 0,
0, 0, 29, 30, 0, 0, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 0, 0, 21, 0, 265, 0, 0, 0, 0, 0, 0, 0, 32, 0, 34, 22, 267, 23, 0, 0, 296,
0, 24, 36, 37, 0, 27, 28, 0, 297, 0, 38, 39, 40, 0, 0, 0, 29, 30, 0, 0, 0, 0, 0,
2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21,
0, 0, 0, 0, 0, 0, 32, 0, 34, 0, 0, 115, 22, 214, 23, 0, 0, 36, 37, 24, 0, 0, 0,
27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30, 0, 0, 2, 3, 4, 114, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32,
0, 34, 22, 0, 0, 0, 0, 0, 0, 24, 36, 37, 0, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0,
29, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 161, 162,
230, 0, 231, 232, 233, 234, 235, 236, 237, 238, 32, 163, 34, 164, 239, 0, 165, 0,
240, 0, 0, 36, 37, 241, 166, 167, 242, 243, 168, 169, 170, 171, 0, 0, 244, 172,
0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 245, 176, 177, 178, 179, 0, 33, 0, 0, 246, 115, 0, 0, 180, 0, 0, 0, 181, 161,
162, 0, 0, 0, 0, 182, 493, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0,
0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174,
175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177,
178, 179, 0, 33, 0, 0, 0, 318, 494, 0, 180, 0, 0, 0, 181, 161, 162, 0, 463, 464,
0, 182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0,
166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0,
33, 0, 0, 0, 318, 687, 0, 180, 0, 0, 0, 181, 161, 162, 0, 463, 464, 0, 182, 0, 0,
183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0,
0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0,
318, 768, 0, 180, 0, 0, 0, 181, 161, 162, 0, 463, 464, 0, 182, 0, 0, 183, 184,
185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168,
169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 318, 0,
0, 180, 0, 0, 0, 181, 161, 162, 0, 463, 464, 0, 182, 0, 0, 183, 184, 185, 0, 163,
0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171,
0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 0, 0, 0, 180, 0, 0, 0,
181, 161, 162, 0, 0, 0, 273, 182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187,
165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0,
173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 318, 0, 0, 180, 0, 0, 0, 181, 161, 162,
0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0,
0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178,
179, 0, 33, 0, 0, 412, 0, 0, 0, 180, 0, 0, 0, 181, 161, 162, 0, 0, 0, 0, 182, 0,
0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167,
0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0,
0, 0, 0, 180, 0, 0, 0, 181, 161, 162, 0, 0, 0, 481, 182, 0, 0, 183, 184, 185, 0,
163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170,
171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 0, 0, 0, 180, 536,
0, 0, 181, 161, 162, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186,
187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172,
0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 0, 0, 0, 180, 568, 0, 0, 181, 161,
162, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0,
0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174,
175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177,
178, 179, 0, 33, 0, 0, 0, 0, 0, 0, 180, 0, 0, 0, 181, 161, 162, 0, 0, 0, 611,
182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0,
166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0,
33, 0, 0, 653, 0, 0, 0, 180, 0, 0, 0, 181, 161, 162, 0, 0, 0, 0, 182, 0, 0, 183,
184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0,
168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 655, 0,
0, 0, 180, 0, 0, 0, 181, 161, 162, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 163,
0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171,
0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 0, 0, 0, 180, 709, 0,
0, 181, 161, 162, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187,
165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0,
173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 0, 0, 0, 180, 712, 0, 0, 181, 161, 162,
0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0,
0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178,
179, 0, 33, 0, 0, 715, 0, 0, 0, 180, 0, 0, 0, 181, 161, 162, 0, 0, 0, 0, 182, 0,
0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167,
0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0,
0, 0, 0, 180, 748, 0, 0, 181, 161, 162, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0,
163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170,
171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 0, 0, 0, 180, 752,
0, 0, 181, 161, 162, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186,
187, 165, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172,
0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 0, 0, 0, 180, 754, 0, 0, 181, 161,
162, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0,
0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174,
175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177,
178, 179, 0, 33, 0, 0, 0, 0, 0, 0, 180, 784, 0, 0, 181, 161, 162, 0, 0, 0, 0,
182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 0,
166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0,
33, 0, 0, 0, 161, 162, 0, 180, 0, 0, 0, 181, 0, 0, 0, 0, 0, 163, 182, 0, 0, 183,
184, 185, 0, 0, 0, 0, 186, 187, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 0,
0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 318, 0, 0, 180, 0, 0, 0, 181, 161,
162, 0, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 163, 0, 0, 186, 187, 0, 0, 0, 0, 0,
0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 0, 0, 173, 0, 174, 175, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178,
179, 0, 33, 0, 0, 0, 630, 0, 0, 180, 0, 0, 0, 181, 161, 162, 0, 0, 0, 0, 182, 0,
0, 183, 184, 185, 0, 163, 0, 0, 186, 187, 0, 0, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0,
168, 169, 170, 171, 0, 0, 0, 0, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 161,
162, 0, 313, 0, 0, 0, 181, 0, 0, 0, 0, 0, 163, 182, 0, 0, 183, 184, 185, 0, 0, 0,
0, 186, 187, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 0, 0, 173, 0, 174, 175,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178,
179, 0, 33, 0, 0, 0, 161, 162, 0, 329, 0, 0, 0, 181, 0, 0, 0, 0, 0, 163, 182, 0,
0, 183, 184, 185, 0, 0, 0, 0, 186, 187, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0,
0, 0, 0, 173, 0, 174, 175, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 22, 0, 0, 180, 0, 0,
0, 181, 0, 0, 0, 27, 28, 0, 182, 0, 0, 183, 184, 185, 0, 0, 29, 30, 186, 187,
144, 0, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 34, 22, 0, 145, 0, 0, 0, 0, 0, 36, 37, 0, 27, 28,
0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 265, 0, 0, 0, 22, 0, 0, 0, 32, 0, 34, 0,
267, 0, 0, 27, 28, 0, 0, 36, 37, 0, 0, 0, 0, 0, 29, 30, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 585, 0, 0, 0, 22, 0,
0, 0, 32, 0, 34, 0, 0, 0, 0, 27, 28, 0, 0, 36, 37, 0, 0, 0, 0, 0, 29, 30, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 32, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 36, 37}; static short Cyc_yycheck[
4996u]={ 56, 165, 23, 91, 56, 180, 55, 165, 64, 57, 59, 0, 214, 121, 120, 120,
85, 86, 288, 17, 85, 86, 312, 88, 134, 394, 91, 56, 58, 56, 35, 379, 74, 22, 23,
75, 25, 249, 250, 89, 252, 434, 47, 108, 375, 95, 35, 103, 104, 105, 106, 103,
73, 42, 91, 120, 121, 78, 47, 115, 164, 117, 69, 115, 53, 117, 55, 109, 148, 58,
42, 108, 61, 82, 103, 115, 124, 66, 158, 5, 6, 474, 442, 120, 73, 416, 108, 89,
117, 78, 103, 89, 706, 97, 116, 464, 145, 104, 107, 63, 109, 108, 69, 116, 113,
107, 95, 96, 97, 98, 94, 182, 183, 118, 185, 180, 87, 98, 99, 479, 332, 105, 111,
95, 96, 97, 98, 69, 99, 118, 94, 202, 1, 70, 71, 99, 148, 104, 313, 128, 315,
108, 432, 114, 89, 134, 158, 136, 103, 94, 95, 441, 99, 158, 329, 102, 128, 222,
102, 215, 105, 116, 104, 215, 173, 270, 108, 111, 278, 158, 107, 108, 91, 787,
100, 91, 280, 97, 85, 86, 549, 88, 102, 102, 91, 222, 215, 107, 89, 90, 91, 111,
806, 249, 250, 275, 252, 249, 250, 108, 252, 108, 816, 113, 298, 98, 99, 116,
213, 823, 275, 121, 122, 120, 121, 309, 296, 226, 267, 224, 89, 290, 89, 89, 213,
290, 95, 307, 1, 294, 295, 296, 98, 102, 89, 224, 305, 103, 227, 104, 305, 96,
307, 109, 109, 101, 251, 312, 313, 609, 315, 97, 300, 302, 110, 111, 102, 294,
295, 296, 99, 107, 251, 102, 329, 111, 322, 288, 107, 102, 307, 89, 107, 180,
107, 312, 332, 95, 96, 309, 332, 99, 630, 104, 391, 356, 357, 358, 813, 278, 394,
112, 399, 107, 142, 497, 821, 94, 89, 288, 289, 102, 307, 100, 664, 96, 88, 92,
93, 669, 111, 89, 113, 114, 115, 222, 89, 95, 307, 107, 309, 99, 95, 104, 107,
98, 391, 110, 109, 102, 600, 506, 180, 322, 399, 423, 89, 122, 98, 98, 113, 114,
115, 96, 104, 104, 102, 617, 99, 1, 104, 98, 408, 98, 391, 109, 408, 104, 423,
104, 464, 417, 399, 103, 104, 417, 98, 432, 638, 449, 426, 99, 104, 99, 426, 94,
441, 485, 484, 484, 670, 100, 102, 290, 449, 434, 423, 294, 295, 296, 238, 99,
401, 102, 383, 432, 385, 426, 305, 426, 307, 98, 89, 763, 441, 312, 313, 104,
315, 104, 98, 383, 98, 385, 109, 617, 104, 98, 104, 484, 485, 233, 329, 104, 98,
474, 98, 239, 102, 214, 242, 243, 104, 90, 542, 483, 638, 89, 509, 460, 98, 506,
98, 95, 465, 549, 98, 484, 104, 100, 102, 103, 104, 104, 48, 49, 50, 109, 104,
111, 1, 450, 626, 100, 103, 313, 101, 104, 626, 100, 740, 460, 577, 321, 98, 323,
465, 542, 450, 327, 585, 329, 104, 104, 391, 100, 108, 689, 91, 104, 100, 102,
399, 341, 104, 500, 348, 349, 350, 351, 104, 1, 72, 73, 68, 69, 101, 116, 117,
578, 103, 500, 297, 578, 89, 1, 423, 102, 574, 103, 104, 102, 574, 5, 6, 432,
103, 104, 116, 377, 634, 125, 581, 318, 441, 103, 104, 102, 740, 324, 325, 326,
102, 548, 102, 574, 758, 102, 89, 102, 335, 102, 146, 147, 95, 403, 102, 405,
406, 548, 408, 102, 737, 103, 104, 621, 555, 102, 109, 621, 103, 104, 113, 114,
115, 787, 100, 484, 485, 600, 103, 104, 103, 104, 74, 370, 798, 89, 90, 89, 375,
119, 621, 120, 379, 95, 103, 104, 118, 506, 99, 813, 102, 89, 816, 670, 22, 23,
420, 95, 422, 600, 98, 113, 114, 115, 102, 103, 104, 100, 795, 100, 727, 109,
100, 111, 100, 113, 114, 115, 99, 416, 99, 670, 102, 542, 645, 102, 423, 102, 17,
18, 19, 103, 104, 430, 103, 104, 103, 104, 103, 104, 89, 741, 22, 23, 103, 104,
103, 104, 103, 104, 65, 66, 67, 103, 104, 103, 104, 103, 104, 578, 737, 646, 647,
98, 741, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
717, 98, 24, 102, 102, 486, 758, 109, 741, 102, 758, 110, 111, 100, 558, 37, 497,
39, 562, 563, 346, 347, 44, 352, 353, 101, 48, 49, 89, 354, 355, 103, 717, 103,
795, 787, 103, 59, 60, 787, 113, 114, 115, 109, 112, 100, 798, 100, 38, 100, 798,
76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 813, 104, 104, 816, 813, 75, 670, 816,
102, 107, 103, 94, 104, 96, 98, 101, 99, 102, 623, 109, 30, 98, 105, 106, 42, 94,
102, 116, 94, 103, 102, 104, 103, 89, 94, 100, 104, 112, 89, 90, 103, 92, 93,
104, 95, 103, 103, 652, 653, 100, 655, 102, 657, 103, 105, 106, 661, 89, 103,
110, 111, 104, 113, 104, 104, 103, 117, 118, 103, 38, 100, 104, 103, 737, 100,
617, 100, 741, 100, 648, 99, 686, 651, 625, 98, 102, 98, 103, 630, 98, 103, 633,
100, 108, 161, 25, 638, 164, 165, 103, 107, 112, 103, 99, 107, 103, 711, 174,
175, 714, 715, 103, 112, 99, 98, 182, 183, 89, 185, 94, 89, 90, 104, 92, 93, 103,
95, 103, 28, 103, 103, 795, 103, 102, 102, 202, 105, 106, 104, 102, 709, 110,
111, 712, 113, 99, 104, 689, 117, 118, 109, 756, 100, 722, 90, 760, 25, 100, 98,
104, 765, 103, 109, 102, 90, 100, 771, 0, 0, 103, 103, 103, 103, 742, 592, 322,
213, 548, 747, 748, 222, 786, 751, 752, 59, 754, 38, 585, 762, 302, 581, 120,
466, 289, 600, 340, 342, - 1, 740, 343, - 1, 344, - 1, 345, - 1, - 1, - 1, - 1,
- 1, 778, 1, - 1, 781, 280, 783, 784, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, - 1, - 1, - 1, - 1, 804, - 1, 89, 90, 808, 92, 93, - 1, 95, - 1,
- 1, 37, - 1, - 1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, 48, 49, - 1, - 1, 113,
- 1, - 1, - 1, 117, 118, - 1, 59, 60, - 1, - 1, - 1, 340, - 1, 342, 343, 344,
345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, - 1, - 1,
- 1, 86, - 1, - 1, 89, - 1, - 1, - 1, - 1, 94, 95, 96, - 1, 98, - 1, - 1, - 1,
102, 103, - 1, 105, 106, - 1, - 1, 109, - 1, 111, - 1, 113, 114, 115, - 1, - 1,
394, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
60, 61, 62, - 1, 64, - 1, 66, 67, - 1, - 1, 464, - 1, - 1, - 1, 38, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94,
95, 96, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1,
- 1, 113, - 1, 509, 116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 89, 90, - 1, 92,
93, - 1, 95, - 1, - 1, - 1, - 1, - 1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, -
1, - 1, - 1, - 1, 113, - 1, - 1, - 1, 117, 118, 549, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31,
32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50,
51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, - 1, 64, - 1, 66, 67, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 626, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 634, - 1, 89, 90, 91, 92, 93, 94, 95, 96, - 1, 98, 99, - 1, - 1, 102, - 1,
- 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1,
- 1, - 1, - 1, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37,
38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56,
57, 58, 59, 60, 61, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91,
92, 93, 94, 95, 96, - 1, 98, 99, 100, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, -
1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24,
25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44,
45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, - 1,
64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96, - 1, 98,
99, - 1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, - 1, -
1, 116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 37, 38, 39, 40, - 1, - 1, 43, 44, - 1, - 1, - 1, 48,
49, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, 59, 60, - 1, 62, - 1, 64, - 1, 66, 67,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96, - 1, 98, - 1, - 1, - 1,
102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117,
118, - 1, - 1, - 1, - 1, 123, 124, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, - 1, 51, 52, - 1, - 1,
55, 56, 57, 58, 59, 60, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
89, 90, 91, 92, 93, 94, 95, 96, - 1, - 1, 99, - 1, - 1, 102, - 1, - 1, 105, 106,
- 1, - 1, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, - 1, - 1, - 1,
123, 124, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1,
25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1,
- 1, 43, - 1, - 1, - 1, - 1, 48, 49, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, 59,
60, - 1, 62, - 1, 64, - 1, 66, 67, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, 94, 95, 96, - 1,
37, - 1, - 1, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, 48, 49, - 1, - 1, 113, - 1,
- 1, 116, 117, 118, - 1, 59, 60, - 1, 123, 124, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 86, - 1, 24, 89, - 1, - 1, - 1,
- 1, 94, - 1, 96, - 1, 98, - 1, - 1, 37, - 1, 39, - 1, 105, 106, - 1, 44, - 1,
46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 59, 60, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, - 1, - 1, - 1, - 1, 94, 95,
96, - 1, - 1, - 1, 100, - 1, 102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, -
1, 113, 114, 115, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 59, 60, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 89, - 1, - 1, - 1, - 1, 94, 95, 96, - 1, - 1, - 1, - 1, - 1,
102, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, - 1, 113, 114, 115, 1, - 1, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39, - 1, -
1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 59, 60, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, 89, - 1, - 1, - 1, - 1, 94, 95, 96, 37,
- 1, - 1, 100, - 1, 102, - 1, 44, 105, 106, - 1, 48, 49, - 1, - 1, - 1, 113, 114,
115, - 1, - 1, - 1, 59, 60, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, - 1,
- 1, - 1, - 1, 94, 95, 96, 37, - 1, - 1, - 1, - 1, 102, - 1, - 1, 105, 106, - 1,
48, 49, - 1, - 1, - 1, 113, 114, 115, - 1, - 1, - 1, 59, 60, - 1, - 1, 3, 4, 5,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1,
86, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96, 37, 98, 39, - 1, - 1, 102, -
1, 44, 105, 106, - 1, 48, 49, - 1, 111, - 1, 113, 114, 115, - 1, - 1, - 1, 59,
60, - 1, - 1, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96, - 1, -
1, 99, 37, 101, 39, - 1, - 1, 105, 106, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 59, 60, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 94, - 1, 96, 37, - 1, - 1, - 1, - 1, - 1, - 1, 44, 105, 106, - 1,
48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 59, 60, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 94, 38, 96, 40, 41, - 1,
43, - 1, 45, - 1, - 1, 105, 106, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, - 1,
61, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1,
95, - 1, - 1, 98, 99, - 1, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1, - 1, - 1,
- 1, 113, 32, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64,
- 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1,
99, 100, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1, 110, 111, - 1, 113, - 1, - 1,
116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, 99, 100, - 1,
102, - 1, - 1, - 1, 106, 25, 26, - 1, 110, 111, - 1, 113, - 1, - 1, 116, 117,
118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52,
- 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, 99, 100, - 1, 102, - 1, -
1, - 1, 106, 25, 26, - 1, 110, 111, - 1, 113, - 1, - 1, 116, 117, 118, - 1, 38,
- 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52, - 1, - 1, 55,
56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89,
90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, 99, - 1, - 1, 102, - 1, - 1, - 1, 106,
25, 26, - 1, 110, 111, - 1, 113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123,
124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, -
1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92,
93, - 1, 95, - 1, - 1, - 1, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1,
- 1, - 1, 112, 113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1,
62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, -
1, - 1, - 1, 99, - 1, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1,
113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1,
66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, 98, - 1,
- 1, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1,
116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, - 1, - 1, - 1,
102, - 1, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, 112, 113, - 1, - 1, 116, 117,
118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52,
- 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, - 1, - 1, - 1, 102, 103, -
1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, 38,
- 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52, - 1, - 1, 55,
56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89,
90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, - 1, - 1, - 1, 102, 103, - 1, - 1, 106,
25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123,
124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, -
1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92,
93, - 1, 95, - 1, - 1, - 1, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1,
- 1, - 1, 112, 113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1,
62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, -
1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1,
113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1,
66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, 98, - 1,
- 1, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1,
116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, - 1, - 1, - 1,
102, 103, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117,
118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52,
- 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, - 1, - 1, - 1, 102, 103, -
1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, 38,
- 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52, - 1, - 1, 55,
56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89,
90, 91, 92, 93, - 1, 95, - 1, - 1, 98, - 1, - 1, - 1, 102, - 1, - 1, - 1, 106,
25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123,
124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, -
1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92,
93, - 1, 95, - 1, - 1, - 1, - 1, - 1, - 1, 102, 103, - 1, - 1, 106, 25, 26, - 1,
- 1, - 1, - 1, 113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1,
62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, -
1, - 1, - 1, - 1, - 1, - 1, 102, 103, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1,
113, - 1, - 1, 116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1,
66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, - 1,
- 1, - 1, 102, 103, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1,
116, 117, 118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, - 1, - 1, - 1,
102, 103, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117,
118, - 1, 38, - 1, 40, 123, 124, 43, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52,
- 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, 62, - 1, 64, - 1, 66, 67, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, 25, 26, - 1, 102, - 1, - 1,
- 1, 106, - 1, - 1, - 1, - 1, - 1, 38, 113, - 1, - 1, 116, 117, 118, - 1, - 1, -
1, - 1, 123, 124, 51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, - 1, - 1, 64,
- 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1,
99, - 1, - 1, 102, - 1, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1,
116, 117, 118, - 1, 38, - 1, - 1, 123, 124, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, - 1, - 1, 64, - 1, 66, 67, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, 99, - 1, - 1,
102, - 1, - 1, - 1, 106, 25, 26, - 1, - 1, - 1, - 1, 113, - 1, - 1, 116, 117,
118, - 1, 38, - 1, - 1, 123, 124, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 51, 52,
- 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, - 1, - 1, 64, - 1, 66, 67, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, - 1, 25, 26, - 1, 102, - 1,
- 1, - 1, 106, - 1, - 1, - 1, - 1, - 1, 38, 113, - 1, - 1, 116, 117, 118, - 1, -
1, - 1, - 1, 123, 124, 51, 52, - 1, - 1, 55, 56, 57, 58, - 1, - 1, - 1, - 1, - 1,
64, - 1, 66, 67, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, -
1, 25, 26, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, - 1, - 1, - 1, 38, 113, - 1,
- 1, 116, 117, 118, - 1, - 1, - 1, - 1, 123, 124, 51, 52, - 1, - 1, 55, 56, 57,
58, - 1, - 1, - 1, - 1, - 1, 64, - 1, 66, 67, - 1, - 1, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, 89, 90, 91, 92, 93, - 1, 95,
- 1, - 1, - 1, 37, - 1, - 1, 102, - 1, - 1, - 1, 106, - 1, - 1, - 1, 48, 49, - 1,
113, - 1, - 1, 116, 117, 118, - 1, - 1, 59, 60, 123, 124, 63, - 1, - 1, - 1, - 1,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 94, - 1, 96, 37, - 1, 99, - 1, - 1, - 1, - 1, -
1, 105, 106, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 59, 60, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 86, - 1, - 1, - 1, 37, - 1, - 1, - 1, 94, - 1, 96, - 1,
98, - 1, - 1, 48, 49, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, 59, 60, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 86, - 1, - 1, - 1, 37, - 1, - 1, - 1, 94, - 1, 96, - 1, -
1, - 1, - 1, 48, 49, - 1, - 1, 105, 106, - 1, - 1, - 1, - 1, - 1, 59, 60, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 94,
- 1, 96, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 105, 106}; unsigned char Cyc_Yyimpossible[
17u]="\000\000\000\000Yyimpossible"; unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex(); static int
Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static int
Cyc_yynerrs= 0; struct _tuple18{ void* f1; void* f2; } ; struct _tuple19{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int yystate;
int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short* yyss=(
short*)({ unsigned int _temp3308= 10000u; short* _temp3309=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3308));{ unsigned int
_temp3310= _temp3308; unsigned int i; for( i= 0; i <  _temp3310; i ++){
_temp3309[ i]=( short) 0;}}; _temp3309;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3305= 10000u; void** _temp3306=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3305));{ unsigned int _temp3307= _temp3305;
unsigned int i; for( i= 0; i <  _temp3307; i ++){ _temp3306[ i]= Cyc_yylval;}};
_temp3306;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3302= 10000u; struct Cyc_Yyltype* _temp3303=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3302));{
unsigned int _temp3304= _temp3302; unsigned int i; for( i= 0; i <  _temp3304; i
++){ _temp3303[ i]= Cyc_yynewloc();}}; _temp3303;}); int yystacksize= 10000;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:
yyss[ _check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate;
if( yyssp_offset >=  yystacksize -  1){ if( yystacksize >=  10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
yystate]; if( yyn ==  - 32768){ goto yydefault;} if( Cyc_yychar ==  - 2){ Cyc_yychar=
Cyc_yylex();} if( Cyc_yychar <=  0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=(
Cyc_yychar >  0? Cyc_yychar <=  352: 0)?( int) Cyc_yytranslate[
_check_known_subscript_notnull( 353u, Cyc_yychar)]: 237;} yyn += yychar1; if((
yyn <  0? 1: yyn >  4995)? 1: Cyc_yycheck[ yyn] !=  yychar1){ goto yydefault;}
yyn=( int) Cyc_yytable[ yyn]; if( yyn <  0){ if( yyn ==  - 32768){ goto yyerrlab;}
yyn= - yyn; goto yyreduce;} else{ if( yyn ==  0){ goto yyerrlab;}} if( yyn == 
830){ return 0;} if( Cyc_yychar !=  0){ Cyc_yychar= - 2;} yyvs[
_check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval; yyls[
_check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus !=  0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ yystate]; if( yyn ==  0){ goto yyerrlab;} yyreduce:
yylen=( int) Cyc_yyr2[ yyn]; if( yylen >  0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset +  1) -  yylen)];} switch(
yyn){ case 1: _LL1447: yyval= yyvs[ yyvsp_offset]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(
yyvs[ yyvsp_offset]); break; case 2: _LL1448: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1450=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1450[ 0]=({ struct Cyc_DeclList_tok_struct _temp1451; _temp1451.tag= Cyc_DeclList_tok;
_temp1451.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ yyvsp_offset])); _temp1451;}); _temp1450;}); break; case 3: _LL1449: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1453=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1453[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1454; _temp1454.tag= Cyc_DeclList_tok; _temp1454.f1=({ struct Cyc_List_List*
_temp1455=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1455->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1456=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1456->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp1457=( struct Cyc_Absyn_Using_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp1457[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp1458; _temp1458.tag= Cyc_Absyn_Using_d; _temp1458.f1=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  2)]); _temp1458.f2= Cyc_yyget_DeclList_tok( yyvs[ yyvsp_offset]); _temp1458;});
_temp1457;})); _temp1456->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line); _temp1456;}); _temp1455->tl= 0; _temp1455;});
_temp1454;}); _temp1453;}); Cyc_Lex_leave_using(); break; case 4: _LL1452: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1460=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1460[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1461; _temp1461.tag= Cyc_DeclList_tok; _temp1461.f1=({ struct Cyc_List_List*
_temp1462=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1462->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1463=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1463->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp1464=( struct Cyc_Absyn_Using_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp1464[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp1465; _temp1465.tag= Cyc_Absyn_Using_d; _temp1465.f1=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  4)]); _temp1465.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1465;});
_temp1464;})); _temp1463->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1463;}); _temp1462->tl= Cyc_yyget_DeclList_tok( yyvs[ yyvsp_offset]);
_temp1462;}); _temp1461;}); _temp1460;}); break; case 5: _LL1459: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1467=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1467[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1468; _temp1468.tag= Cyc_DeclList_tok; _temp1468.f1=({ struct Cyc_List_List*
_temp1469=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1469->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1470=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1470->r=( void*)(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp1471=( struct Cyc_Absyn_Namespace_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp1471[ 0]=({
struct Cyc_Absyn_Namespace_d_struct _temp1472; _temp1472.tag= Cyc_Absyn_Namespace_d;
_temp1472.f1=({ struct _tagged_arr* _temp1473=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp1473[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1473;});
_temp1472.f2= Cyc_yyget_DeclList_tok( yyvs[ yyvsp_offset]); _temp1472;});
_temp1471;})); _temp1470->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line); _temp1470;}); _temp1469->tl= 0; _temp1469;});
_temp1468;}); _temp1467;}); Cyc_Lex_leave_namespace(); break; case 6: _LL1466:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1475=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1475[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1476; _temp1476.tag= Cyc_DeclList_tok; _temp1476.f1=({ struct Cyc_List_List*
_temp1477=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1477->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1478=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1478->r=( void*)(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp1479=( struct Cyc_Absyn_Namespace_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp1479[ 0]=({
struct Cyc_Absyn_Namespace_d_struct _temp1480; _temp1480.tag= Cyc_Absyn_Namespace_d;
_temp1480.f1=({ struct _tagged_arr* _temp1481=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp1481[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1481;});
_temp1480.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1480;}); _temp1479;})); _temp1478->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1478;}); _temp1477->tl= Cyc_yyget_DeclList_tok( yyvs[ yyvsp_offset]);
_temp1477;}); _temp1476;}); _temp1475;}); break; case 7: _LL1474: if( Cyc_Std_strcmp(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  4)]), _tag_arr("C", sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err(
_tag_arr("only extern \"C\" { ... } is allowed", sizeof( unsigned char), 35u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).last_line));} yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1483=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1483[ 0]=({ struct Cyc_DeclList_tok_struct _temp1484; _temp1484.tag= Cyc_DeclList_tok;
_temp1484.f1=({ struct Cyc_List_List* _temp1485=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1485->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1486=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1486->r=( void*)(( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp1487=(
struct Cyc_Absyn_ExternC_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp1487[ 0]=({ struct Cyc_Absyn_ExternC_d_struct _temp1488; _temp1488.tag= Cyc_Absyn_ExternC_d;
_temp1488.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1488;}); _temp1487;})); _temp1486->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1486;}); _temp1485->tl= Cyc_yyget_DeclList_tok( yyvs[ yyvsp_offset]);
_temp1485;}); _temp1484;}); _temp1483;}); break; case 8: _LL1482: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1490=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1490[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1491; _temp1491.tag= Cyc_DeclList_tok; _temp1491.f1= 0; _temp1491;});
_temp1490;}); break; case 9: _LL1489: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1493=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1493[ 0]=({ struct Cyc_DeclList_tok_struct _temp1494; _temp1494.tag= Cyc_DeclList_tok;
_temp1494.f1=({ struct Cyc_List_List* _temp1495=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1495->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp1496=( struct Cyc_Absyn_Fn_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1496[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1497; _temp1497.tag= Cyc_Absyn_Fn_d; _temp1497.f1= Cyc_yyget_FnDecl_tok(
yyvs[ yyvsp_offset]); _temp1497;}); _temp1496;}), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1495->tl=
0; _temp1495;}); _temp1494;}); _temp1493;}); break; case 10: _LL1492: yyval=
yyvs[ yyvsp_offset]; break; case 11: _LL1498: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1500=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1500[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1501; _temp1501.tag= Cyc_FnDecl_tok;
_temp1501.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1501;}); _temp1500;}); break; case 12: _LL1499:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1503=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1503[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1504; _temp1504.tag= Cyc_FnDecl_tok; _temp1504.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp1505=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1505->v=( void*) Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1505;}), Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1504;}); _temp1503;}); break; case 13: _LL1502:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1507=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1507[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1508; _temp1508.tag= Cyc_FnDecl_tok; _temp1508.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1508;}); _temp1507;}); break; case 14: _LL1506:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1510=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1510[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1511; _temp1511.tag= Cyc_FnDecl_tok; _temp1511.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp1512=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1512->v=( void*) Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1512;}), Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1511;}); _temp1510;}); break; case 15: _LL1509:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1514=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1514[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1515; _temp1515.tag= Cyc_FnDecl_tok; _temp1515.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp1516=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1516->v=( void*) Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1516;}), Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1515;}); _temp1514;}); break; case 16: _LL1513:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1518=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1518[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1519; _temp1519.tag= Cyc_FnDecl_tok; _temp1519.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp1520=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1520->v=( void*) Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1520;}), Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1519;}); _temp1518;}); break; case 17: _LL1517:
Cyc_Lex_enter_using( Cyc_yyget_QualId_tok( yyvs[ yyvsp_offset])); yyval= yyvs[
yyvsp_offset]; break; case 18: _LL1521: Cyc_Lex_leave_using(); break; case 19:
_LL1522: Cyc_Lex_enter_namespace(({ struct _tagged_arr* _temp1524=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1524[ 0]= Cyc_yyget_String_tok(
yyvs[ yyvsp_offset]); _temp1524;})); yyval= yyvs[ yyvsp_offset]; break; case 20:
_LL1523: Cyc_Lex_leave_namespace(); break; case 21: _LL1525: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1527=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1527[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1528; _temp1528.tag= Cyc_DeclList_tok; _temp1528.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1528;}); _temp1527;}); break; case 22: _LL1526: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1530=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1530[ 0]=({ struct Cyc_DeclList_tok_struct _temp1531; _temp1531.tag= Cyc_DeclList_tok;
_temp1531.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1531;}); _temp1530;}); break; case 23:
_LL1529: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1533=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1533[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1534; _temp1534.tag= Cyc_DeclList_tok; _temp1534.f1=({ struct Cyc_List_List*
_temp1535=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1535->hd=( void*) Cyc_Absyn_let_decl( Cyc_yyget_Pattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1535->tl= 0; _temp1535;}); _temp1534;});
_temp1533;}); break; case 24: _LL1532: { struct Cyc_List_List* _temp1537= 0;{
struct Cyc_List_List* _temp1538= Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0; _temp1538
!=  0; _temp1538= _temp1538->tl){ struct _tagged_arr* _temp1539=( struct
_tagged_arr*) _temp1538->hd; struct _tuple1* _temp1540=({ struct _tuple1*
_temp1543=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1543->f1=
Cyc_Absyn_rel_ns_null; _temp1543->f2= _temp1539; _temp1543;}); struct Cyc_Absyn_Vardecl*
_temp1541= Cyc_Absyn_new_vardecl( _temp1540, Cyc_Absyn_wildtyp( 0), 0);
_temp1537=({ struct Cyc_List_List* _temp1542=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1542->hd=( void*) _temp1541; _temp1542->tl=
_temp1537; _temp1542;});}} _temp1537=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1537); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1544=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1544[ 0]=({ struct Cyc_DeclList_tok_struct _temp1545; _temp1545.tag= Cyc_DeclList_tok;
_temp1545.f1=({ struct Cyc_List_List* _temp1546=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1546->hd=( void*) Cyc_Absyn_letv_decl(
_temp1537, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp1546->tl= 0; _temp1546;}); _temp1545;}); _temp1544;}); break;} case 25:
_LL1536: yyval= yyvs[ yyvsp_offset]; break; case 26: _LL1547: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1549=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1549[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1550; _temp1550.tag= Cyc_DeclList_tok; _temp1550.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ yyvsp_offset])); _temp1550;}); _temp1549;}); break; case 27: _LL1548:
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1552=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1552[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1553; _temp1553.tag= Cyc_DeclSpec_tok; _temp1553.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1554=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1554->sc=({ struct Cyc_Core_Opt* _temp1555=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1555->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1555;});
_temp1554->tq= Cyc_Absyn_empty_tqual(); _temp1554->type_specs= 0; _temp1554->is_inline=
0; _temp1554->attributes= Cyc_yyget_AttributeList_tok( yyvs[ yyvsp_offset]);
_temp1554;}); _temp1553;}); _temp1552;}); break; case 28: _LL1551: if(( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->sc !=  0){ Cyc_Parse_warn( _tag_arr("Only one storage class is allowed in a declaration",
sizeof( unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1557=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1557[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1558; _temp1558.tag= Cyc_DeclSpec_tok; _temp1558.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1559=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1559->sc=({ struct Cyc_Core_Opt* _temp1560=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1560->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1560;});
_temp1559->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->tq; _temp1559->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->type_specs; _temp1559->is_inline=(
Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->is_inline; _temp1559->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->attributes); _temp1559;}); _temp1558;}); _temp1557;});
break; case 29: _LL1556: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1562=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1562[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1563; _temp1563.tag= Cyc_DeclSpec_tok;
_temp1563.f1=({ struct Cyc_Parse_Declaration_spec* _temp1564=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1564->sc= 0;
_temp1564->tq= Cyc_Absyn_empty_tqual(); _temp1564->type_specs=({ struct Cyc_List_List*
_temp1565=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1565->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1565->tl= 0;
_temp1565;}); _temp1564->is_inline= 0; _temp1564->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ yyvsp_offset]); _temp1564;}); _temp1563;}); _temp1562;}); break; case 30:
_LL1561: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1567=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1567[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1568; _temp1568.tag= Cyc_DeclSpec_tok; _temp1568.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1569=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1569->sc=( Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->sc; _temp1569->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->tq; _temp1569->type_specs=({
struct Cyc_List_List* _temp1570=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1570->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1570->tl=(
Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->type_specs; _temp1570;});
_temp1569->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->is_inline;
_temp1569->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->attributes); _temp1569;}); _temp1568;}); _temp1567;});
break; case 31: _LL1566: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1572=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1572[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1573; _temp1573.tag= Cyc_DeclSpec_tok;
_temp1573.f1=({ struct Cyc_Parse_Declaration_spec* _temp1574=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1574->sc= 0;
_temp1574->tq= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1574->type_specs= 0; _temp1574->is_inline= 0;
_temp1574->attributes= Cyc_yyget_AttributeList_tok( yyvs[ yyvsp_offset]);
_temp1574;}); _temp1573;}); _temp1572;}); break; case 32: _LL1571: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1576=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1576[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1577; _temp1577.tag= Cyc_DeclSpec_tok; _temp1577.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1578=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1578->sc=( Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->sc; _temp1578->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->tq); _temp1578->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
yyvsp_offset]))->type_specs; _temp1578->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
yyvsp_offset]))->is_inline; _temp1578->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->attributes); _temp1578;}); _temp1577;}); _temp1576;});
break; case 33: _LL1575: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1580=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1580[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1581; _temp1581.tag= Cyc_DeclSpec_tok;
_temp1581.f1=({ struct Cyc_Parse_Declaration_spec* _temp1582=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1582->sc= 0;
_temp1582->tq= Cyc_Absyn_empty_tqual(); _temp1582->type_specs= 0; _temp1582->is_inline=
1; _temp1582->attributes= Cyc_yyget_AttributeList_tok( yyvs[ yyvsp_offset]);
_temp1582;}); _temp1581;}); _temp1580;}); break; case 34: _LL1579: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1584=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1584[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1585; _temp1585.tag= Cyc_DeclSpec_tok; _temp1585.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1586=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1586->sc=( Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->sc; _temp1586->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->tq; _temp1586->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->type_specs; _temp1586->is_inline= 1; _temp1586->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->attributes); _temp1586;}); _temp1585;}); _temp1584;});
break; case 35: _LL1583: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1588=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1588[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1589; _temp1589.tag=
Cyc_StorageClass_tok; _temp1589.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp1589;}); _temp1588;}); break; case 36: _LL1587: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1591=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1591[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1592; _temp1592.tag=
Cyc_StorageClass_tok; _temp1592.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1592;}); _temp1591;}); break; case 37: _LL1590: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1594=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1594[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1595; _temp1595.tag=
Cyc_StorageClass_tok; _temp1595.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1595;}); _temp1594;}); break; case 38: _LL1593: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1597=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1597[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1598; _temp1598.tag=
Cyc_StorageClass_tok; _temp1598.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1598;}); _temp1597;}); break; case 39: _LL1596: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ yyvsp_offset]), _tag_arr("C", sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err(
_tag_arr("only extern or extern \"C\" is allowed", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1600=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1600[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1601; _temp1601.tag= Cyc_StorageClass_tok;
_temp1601.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1601;}); _temp1600;});
break; case 40: _LL1599: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1603=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1603[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1604; _temp1604.tag=
Cyc_StorageClass_tok; _temp1604.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1604;}); _temp1603;}); break; case 41: _LL1602: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1606=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1606[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1607; _temp1607.tag=
Cyc_StorageClass_tok; _temp1607.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1607;}); _temp1606;}); break; case 42: _LL1605: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1609=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1609[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1610; _temp1610.tag=
Cyc_AttributeList_tok; _temp1610.f1= 0; _temp1610;}); _temp1609;}); break; case
43: _LL1608: yyval= yyvs[ yyvsp_offset]; break; case 44: _LL1611: yyval=( void*)({
struct Cyc_AttributeList_tok_struct* _temp1613=( struct Cyc_AttributeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp1613[ 0]=({
struct Cyc_AttributeList_tok_struct _temp1614; _temp1614.tag= Cyc_AttributeList_tok;
_temp1614.f1= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1614;}); _temp1613;}); break; case 45:
_LL1612: yyval=( void*)({ struct Cyc_AttributeList_tok_struct* _temp1616=(
struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1616[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1617; _temp1617.tag=
Cyc_AttributeList_tok; _temp1617.f1=({ struct Cyc_List_List* _temp1618=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1618->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ yyvsp_offset]); _temp1618->tl= 0; _temp1618;});
_temp1617;}); _temp1616;}); break; case 46: _LL1615: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1620=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1620[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1621; _temp1621.tag=
Cyc_AttributeList_tok; _temp1621.f1=({ struct Cyc_List_List* _temp1622=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1622->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1622->tl= Cyc_yyget_AttributeList_tok( yyvs[
yyvsp_offset]); _temp1622;}); _temp1621;}); _temp1620;}); break; case 47:
_LL1619: { struct _tagged_arr _temp1624= Cyc_yyget_String_tok( yyvs[
yyvsp_offset]); if(((( _get_arr_size( _temp1624, sizeof( unsigned char)) >  4?*((
const unsigned char*) _check_unknown_subscript( _temp1624, sizeof( unsigned char),
0)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1624,
sizeof( unsigned char), 1)) == '_': 0)?*(( const unsigned char*)
_check_unknown_subscript( _temp1624, sizeof( unsigned char),( int)(
_get_arr_size( _temp1624, sizeof( unsigned char)) -  2))) == '_': 0)?*(( const
unsigned char*) _check_unknown_subscript( _temp1624, sizeof( unsigned char),(
int)( _get_arr_size( _temp1624, sizeof( unsigned char)) -  3))) == '_': 0){
_temp1624=( struct _tagged_arr) Cyc_Std_substring( _temp1624, 2, _get_arr_size(
_temp1624, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0; i <  16u; ++ i){
if( Cyc_Std_strcmp( _temp1624,( Cyc_Parse_att_map[ i]).f1) ==  0){ yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1625=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1625[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1626; _temp1626.tag= Cyc_Attribute_tok; _temp1626.f1=(
void*)( Cyc_Parse_att_map[ i]).f2; _temp1626;}); _temp1625;}); break;}} if( i == 
16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1627=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1627[ 0]=({ struct Cyc_Attribute_tok_struct _temp1628; _temp1628.tag= Cyc_Attribute_tok;
_temp1628.f1=( void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1628;}); _temp1627;});}
break;}} case 48: _LL1623: yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1630=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1630[ 0]=({ struct Cyc_Attribute_tok_struct _temp1631; _temp1631.tag= Cyc_Attribute_tok;
_temp1631.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1631;}); _temp1630;});
break; case 49: _LL1629: { struct _tagged_arr _temp1633= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1636; int _temp1637; struct _tuple15* _temp1634= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1636=*
_temp1634; _LL1638: _temp1637= _temp1636.f2; goto _LL1635; _LL1635: { void* a;
if( Cyc_Std_zstrcmp( _temp1633, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1633, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1637 <  0? 1: _temp1637 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 0 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1639=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1639[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1640; _temp1640.tag= Cyc_Absyn_Regparm_att; _temp1640.f1= _temp1637;
_temp1640;}); _temp1639;});} else{ if( Cyc_Std_zstrcmp( _temp1633, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1633, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1637 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1637;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1641=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1641[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1642; _temp1642.tag= Cyc_Absyn_Aligned_att; _temp1642.f1= _temp1637;
_temp1642;}); _temp1641;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1643=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1643[ 0]=({ struct Cyc_Attribute_tok_struct _temp1644; _temp1644.tag= Cyc_Attribute_tok;
_temp1644.f1=( void*) a; _temp1644;}); _temp1643;}); break;}} case 50: _LL1632: {
struct _tagged_arr _temp1646= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1647= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1646, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1646, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1648=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1648[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1649; _temp1649.tag=
Cyc_Absyn_Section_att; _temp1649.f1= _temp1647; _temp1649;}); _temp1648;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1650=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1650[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1651; _temp1651.tag= Cyc_Attribute_tok; _temp1651.f1=(
void*) a; _temp1651;}); _temp1650;}); break;} case 51: _LL1645: { struct
_tagged_arr _temp1653= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1654= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1657; struct _tuple15 _temp1655=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1658:
_temp1657= _temp1655.f2; goto _LL1656; _LL1656: { int _temp1661; struct _tuple15
_temp1659=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1662: _temp1661= _temp1659.f2; goto _LL1660; _LL1660: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1653, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1653, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1654, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1663=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1663[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1664; _temp1664.tag=
Cyc_Absyn_Format_att; _temp1664.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1664.f2= _temp1657; _temp1664.f3= _temp1661; _temp1664;}); _temp1663;});}
else{ if( Cyc_Std_zstrcmp( _temp1654, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1665=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1665[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1666; _temp1666.tag=
Cyc_Absyn_Format_att; _temp1666.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1666.f2= _temp1657; _temp1666.f3= _temp1661; _temp1666;}); _temp1665;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
yylsp_offset]).last_line));} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1667=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1667[ 0]=({ struct Cyc_Attribute_tok_struct _temp1668; _temp1668.tag= Cyc_Attribute_tok;
_temp1668.f1=( void*) a; _temp1668;}); _temp1667;}); break;}}} case 52: _LL1652:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1670=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1670[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1671; _temp1671.tag= Cyc_TypeSpecifier_tok;
_temp1671.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1671;});
_temp1670;}); break; case 53: _LL1669: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1673=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1673[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1674; _temp1674.tag=
Cyc_TypeSpecifier_tok; _temp1674.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1674;}); _temp1673;}); break; case 54: _LL1672:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1676=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1676[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1677; _temp1677.tag= Cyc_TypeSpecifier_tok;
_temp1677.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1677;});
_temp1676;}); break; case 55: _LL1675: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1679=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1679[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1680; _temp1680.tag=
Cyc_TypeSpecifier_tok; _temp1680.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1681=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1681[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1682; _temp1682.tag=
Cyc_Parse_Short_spec; _temp1682.f1= Cyc_Position_segment_of_abs(( yyls[
yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line); _temp1682;});
_temp1681;})); _temp1680;}); _temp1679;}); break; case 56: _LL1678: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1684=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1684[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1685; _temp1685.tag= Cyc_TypeSpecifier_tok;
_temp1685.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1685;});
_temp1684;}); break; case 57: _LL1683: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1687=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1687[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1688; _temp1688.tag=
Cyc_TypeSpecifier_tok; _temp1688.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1689=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1689[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1690; _temp1690.tag= Cyc_Parse_Long_spec;
_temp1690.f1= Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line); _temp1690;}); _temp1689;})); _temp1688;});
_temp1687;}); break; case 58: _LL1686: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1692=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1692[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1693; _temp1693.tag=
Cyc_TypeSpecifier_tok; _temp1693.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1693;}); _temp1692;}); break; case 59: _LL1691:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1695=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1695[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1696; _temp1696.tag= Cyc_TypeSpecifier_tok;
_temp1696.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t( 0), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1696;});
_temp1695;}); break; case 60: _LL1694: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1698=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1698[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1699; _temp1699.tag=
Cyc_TypeSpecifier_tok; _temp1699.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1700=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1700[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1701; _temp1701.tag=
Cyc_Parse_Signed_spec; _temp1701.f1= Cyc_Position_segment_of_abs(( yyls[
yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line); _temp1701;});
_temp1700;})); _temp1699;}); _temp1698;}); break; case 61: _LL1697: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1703=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1703[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1704; _temp1704.tag= Cyc_TypeSpecifier_tok;
_temp1704.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1705=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1705[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1706; _temp1706.tag= Cyc_Parse_Unsigned_spec; _temp1706.f1= Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line); _temp1706;});
_temp1705;})); _temp1704;}); _temp1703;}); break; case 62: _LL1702: yyval= yyvs[
yyvsp_offset]; break; case 63: _LL1707: yyval= yyvs[ yyvsp_offset]; break; case
64: _LL1708: yyval= yyvs[ yyvsp_offset]; break; case 65: _LL1709: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1711=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1711[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1712; _temp1712.tag= Cyc_TypeSpecifier_tok;
_temp1712.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok( yyvs[
yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1712;}); _temp1711;}); break; case 66:
_LL1710: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1714=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1714[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1715; _temp1715.tag=
Cyc_TypeSpecifier_tok; _temp1715.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1716=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1716[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1717; _temp1717.tag= Cyc_Absyn_TypedefType;
_temp1717.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1717.f2= Cyc_yyget_TypeList_tok( yyvs[ yyvsp_offset]);
_temp1717.f3= 0; _temp1717;}); _temp1716;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1715;}); _temp1714;}); break; case 67: _LL1713:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1719=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1719[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1720; _temp1720.tag= Cyc_TypeSpecifier_tok;
_temp1720.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1721=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1721[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1722; _temp1722.tag= Cyc_Absyn_TupleType;
_temp1722.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1722;});
_temp1721;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp1720;}); _temp1719;}); break; case 68: _LL1718: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1724=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1724[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1725; _temp1725.tag=
Cyc_TypeSpecifier_tok; _temp1725.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1726=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1726[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1727; _temp1727.tag= Cyc_Absyn_RgnHandleType;
_temp1727.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1727;}); _temp1726;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1725;}); _temp1724;}); break; case 69:
_LL1723: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1729=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1729[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1730; _temp1730.tag=
Cyc_TypeSpecifier_tok; _temp1730.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_SizeofType_struct* _temp1731=( struct Cyc_Absyn_SizeofType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SizeofType_struct)); _temp1731[ 0]=({ struct
Cyc_Absyn_SizeofType_struct _temp1732; _temp1732.tag= Cyc_Absyn_SizeofType;
_temp1732.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1732;}); _temp1731;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1730;}); _temp1729;}); break; case 70:
_LL1728: yyval=( void*)({ struct Cyc_Kind_tok_struct* _temp1734=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1734[ 0]=({ struct Cyc_Kind_tok_struct
_temp1735; _temp1735.tag= Cyc_Kind_tok; _temp1735.f1=( void*) Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1735;});
_temp1734;}); break; case 71: _LL1733: { struct _tagged_arr* _temp1739; void*
_temp1741; struct _tuple1 _temp1737=* Cyc_yyget_QualId_tok( yyvs[ yyvsp_offset]);
_LL1742: _temp1741= _temp1737.f1; goto _LL1740; _LL1740: _temp1739= _temp1737.f2;
goto _LL1738; _LL1738: if( _temp1741 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err(
_tag_arr("bad kind in type specifier", sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1743=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1743[ 0]=({ struct Cyc_Kind_tok_struct
_temp1744; _temp1744.tag= Cyc_Kind_tok; _temp1744.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1739, Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1744;}); _temp1743;}); break;} case 72: _LL1736:
yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1746=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1746[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1747; _temp1747.tag= Cyc_TypeQual_tok;
_temp1747.f1=({ struct Cyc_Absyn_Tqual _temp1748; _temp1748.q_const= 1;
_temp1748.q_volatile= 0; _temp1748.q_restrict= 0; _temp1748;}); _temp1747;});
_temp1746;}); break; case 73: _LL1745: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1750=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1750[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1751; _temp1751.tag= Cyc_TypeQual_tok; _temp1751.f1=({ struct Cyc_Absyn_Tqual
_temp1752; _temp1752.q_const= 0; _temp1752.q_volatile= 1; _temp1752.q_restrict=
0; _temp1752;}); _temp1751;}); _temp1750;}); break; case 74: _LL1749: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1754=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1754[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1755; _temp1755.tag= Cyc_TypeQual_tok;
_temp1755.f1=({ struct Cyc_Absyn_Tqual _temp1756; _temp1756.q_const= 0;
_temp1756.q_volatile= 0; _temp1756.q_restrict= 1; _temp1756;}); _temp1755;});
_temp1754;}); break; case 75: _LL1753: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1758=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1758[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1759; _temp1759.tag=
Cyc_TypeSpecifier_tok; _temp1759.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1760=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1760[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1761; _temp1761.tag= Cyc_Parse_Decl_spec;
_temp1761.f1=({ struct Cyc_Absyn_Decl* _temp1762=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1762->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1763=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1763[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1764; _temp1764.tag= Cyc_Absyn_Enum_d; _temp1764.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1765=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1765->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1765->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1765->fields=({
struct Cyc_Core_Opt* _temp1766=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1766->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1766;});
_temp1765;}); _temp1764;}); _temp1763;})); _temp1762->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line); _temp1762;}); _temp1761;}); _temp1760;}));
_temp1759;}); _temp1758;}); break; case 76: _LL1757: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1768=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1768[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1769; _temp1769.tag=
Cyc_TypeSpecifier_tok; _temp1769.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1770=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1770[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1771; _temp1771.tag= Cyc_Absyn_EnumType;
_temp1771.f1= Cyc_yyget_QualId_tok( yyvs[ yyvsp_offset]); _temp1771.f2= 0;
_temp1771;}); _temp1770;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1769;}); _temp1768;}); break; case 77: _LL1767:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1773=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1773[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1774; _temp1774.tag= Cyc_TypeSpecifier_tok;
_temp1774.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct* _temp1775=(
struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1775[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1776; _temp1776.tag= Cyc_Parse_Type_spec;
_temp1776.f1=( void*)(( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp1777=(
struct Cyc_Absyn_AnonEnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct));
_temp1777[ 0]=({ struct Cyc_Absyn_AnonEnumType_struct _temp1778; _temp1778.tag=
Cyc_Absyn_AnonEnumType; _temp1778.f1= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1778;});
_temp1777;})); _temp1776.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp1776;}); _temp1775;})); _temp1774;}); _temp1773;});
break; case 78: _LL1772: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1780=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1780[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1781; _temp1781.tag= Cyc_Enumfield_tok;
_temp1781.f1=({ struct Cyc_Absyn_Enumfield* _temp1782=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1782->name= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); _temp1782->tag= 0; _temp1782->loc= Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line); _temp1782;});
_temp1781;}); _temp1780;}); break; case 79: _LL1779: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1784=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1784[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1785; _temp1785.tag= Cyc_Enumfield_tok;
_temp1785.f1=({ struct Cyc_Absyn_Enumfield* _temp1786=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1786->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1786->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp1786->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line); _temp1786;});
_temp1785;}); _temp1784;}); break; case 80: _LL1783: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1788=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1788[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1789; _temp1789.tag=
Cyc_EnumfieldList_tok; _temp1789.f1=({ struct Cyc_List_List* _temp1790=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1790->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ yyvsp_offset]); _temp1790->tl= 0; _temp1790;});
_temp1789;}); _temp1788;}); break; case 81: _LL1787: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1792=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1792[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1793; _temp1793.tag=
Cyc_EnumfieldList_tok; _temp1793.f1=({ struct Cyc_List_List* _temp1794=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1794->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1794->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
yyvsp_offset]); _temp1794;}); _temp1793;}); _temp1792;}); break; case 82:
_LL1791: { void* t; switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1796: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1798=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1798[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1799; _temp1799.tag=
Cyc_Absyn_AnonStructType; _temp1799.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1799;});
_temp1798;}); break; case Cyc_Parse_Union_su: _LL1797: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1801=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1801[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1802; _temp1802.tag= Cyc_Absyn_AnonUnionType; _temp1802.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1802;});
_temp1801;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1803=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1803[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1804; _temp1804.tag=
Cyc_TypeSpecifier_tok; _temp1804.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1805=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1805[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1806; _temp1806.tag= Cyc_Parse_Type_spec;
_temp1806.f1=( void*) t; _temp1806.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp1806;}); _temp1805;})); _temp1804;}); _temp1803;});
break;} case 83: _LL1795: { struct Cyc_List_List* _temp1808=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1809: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1811=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1811->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1811;}), _temp1808,({ struct Cyc_Core_Opt*
_temp1812=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1812->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1812;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line)); break; case
Cyc_Parse_Union_su: _LL1810: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp1814=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1814->v=( void*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1814;}),
_temp1808,({ struct Cyc_Core_Opt* _temp1815=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1815->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1815;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line)); break;}
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1816=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1816[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1817; _temp1817.tag= Cyc_TypeSpecifier_tok;
_temp1817.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1818=(
struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1818[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1819; _temp1819.tag= Cyc_Parse_Decl_spec;
_temp1819.f1= d; _temp1819;}); _temp1818;})); _temp1817;}); _temp1816;}); break;}
case 84: _LL1807: { struct Cyc_List_List* _temp1821=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1822: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1824=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1824->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1824;}), _temp1821,({ struct Cyc_Core_Opt*
_temp1825=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1825->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1825;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line)); break; case
Cyc_Parse_Union_su: _LL1823: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp1827=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1827->v=( void*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1827;}),
_temp1821,({ struct Cyc_Core_Opt* _temp1828=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1828->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1828;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line)); break;}
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1829=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1829[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1830; _temp1830.tag= Cyc_TypeSpecifier_tok;
_temp1830.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1831=(
struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1831[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1832; _temp1832.tag= Cyc_Parse_Decl_spec;
_temp1832.f1= d; _temp1832;}); _temp1831;})); _temp1830;}); _temp1829;}); break;}
case 85: _LL1820: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1834: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1836=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1836[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1837; _temp1837.tag=
Cyc_TypeSpecifier_tok; _temp1837.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1838=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1838[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1839; _temp1839.tag= Cyc_Absyn_StructType;
_temp1839.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1839.f2= Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset]); _temp1839.f3= 0; _temp1839;}); _temp1838;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1837;}); _temp1836;}); break; case Cyc_Parse_Union_su:
_LL1835: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1841=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1841[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1842; _temp1842.tag=
Cyc_TypeSpecifier_tok; _temp1842.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1843=( struct Cyc_Absyn_UnionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1843[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1844; _temp1844.tag= Cyc_Absyn_UnionType;
_temp1844.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1844.f2= Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset]); _temp1844.f3= 0; _temp1844;}); _temp1843;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1842;}); _temp1841;}); break;} break;
case 86: _LL1833: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1846: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1848=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1848[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1849; _temp1849.tag=
Cyc_TypeSpecifier_tok; _temp1849.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1850=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1850[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1851; _temp1851.tag= Cyc_Absyn_StructType;
_temp1851.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1851.f2= Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset]); _temp1851.f3= 0; _temp1851;}); _temp1850;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1849;}); _temp1848;}); break; case Cyc_Parse_Union_su:
_LL1847: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1853=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1853[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1854; _temp1854.tag=
Cyc_TypeSpecifier_tok; _temp1854.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1855=( struct Cyc_Absyn_UnionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1855[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1856; _temp1856.tag= Cyc_Absyn_UnionType;
_temp1856.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1856.f2= Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset]); _temp1856.f3= 0; _temp1856;}); _temp1855;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1854;}); _temp1853;}); break;} break;
case 87: _LL1845: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp1858=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1858[ 0]=({ struct Cyc_TypeList_tok_struct _temp1859; _temp1859.tag= Cyc_TypeList_tok;
_temp1859.f1= 0; _temp1859;}); _temp1858;}); break; case 88: _LL1857: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1861=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1861[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1862; _temp1862.tag= Cyc_TypeList_tok; _temp1862.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1862;});
_temp1861;}); break; case 89: _LL1860:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1864=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1864[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1865; _temp1865.tag= Cyc_TypeList_tok; _temp1865.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1865;});
_temp1864;}); break; case 90: _LL1863: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1867=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1867[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1868; _temp1868.tag= Cyc_StructOrUnion_tok; _temp1868.f1= Cyc_Parse_Struct_su;
_temp1868;}); _temp1867;}); break; case 91: _LL1866: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1870=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1870[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1871; _temp1871.tag= Cyc_StructOrUnion_tok; _temp1871.f1= Cyc_Parse_Union_su;
_temp1871;}); _temp1870;}); break; case 92: _LL1869: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1873=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1873[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1874; _temp1874.tag= Cyc_StructFieldDeclList_tok; _temp1874.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ yyvsp_offset]))); _temp1874;}); _temp1873;}); break; case 93: _LL1872:
yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct* _temp1876=(
struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct));
_temp1876[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct _temp1877;
_temp1877.tag= Cyc_StructFieldDeclListList_tok; _temp1877.f1=({ struct Cyc_List_List*
_temp1878=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1878->hd=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[ yyvsp_offset]);
_temp1878->tl= 0; _temp1878;}); _temp1877;}); _temp1876;}); break; case 94:
_LL1875: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1880=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1880[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1881; _temp1881.tag= Cyc_StructFieldDeclListList_tok; _temp1881.f1=({
struct Cyc_List_List* _temp1882=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1882->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ yyvsp_offset]); _temp1882->tl= Cyc_yyget_StructFieldDeclListList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1882;});
_temp1881;}); _temp1880;}); break; case 95: _LL1879: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1884=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1884[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1885; _temp1885.tag=
Cyc_InitDeclList_tok; _temp1885.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[ yyvsp_offset]));
_temp1885;}); _temp1884;}); break; case 96: _LL1883: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1887=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1887[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1888; _temp1888.tag=
Cyc_InitDeclList_tok; _temp1888.f1=({ struct Cyc_List_List* _temp1889=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1889->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ yyvsp_offset]); _temp1889->tl= 0; _temp1889;});
_temp1888;}); _temp1887;}); break; case 97: _LL1886: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1891=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1891[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1892; _temp1892.tag=
Cyc_InitDeclList_tok; _temp1892.f1=({ struct Cyc_List_List* _temp1893=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1893->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ yyvsp_offset]); _temp1893->tl= Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1893;});
_temp1892;}); _temp1891;}); break; case 98: _LL1890: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1895=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1895[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1896; _temp1896.tag= Cyc_InitDecl_tok;
_temp1896.f1=({ struct _tuple14* _temp1897=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1897->f1= Cyc_yyget_Declarator_tok( yyvs[
yyvsp_offset]); _temp1897->f2= 0; _temp1897;}); _temp1896;}); _temp1895;});
break; case 99: _LL1894: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1899=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1899[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1900; _temp1900.tag= Cyc_InitDecl_tok;
_temp1900.f1=({ struct _tuple14* _temp1901=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1901->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1901->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp1901;});
_temp1900;}); _temp1899;}); break; case 100: _LL1898: { struct _tuple17
_temp1905; struct Cyc_List_List* _temp1906; struct Cyc_List_List* _temp1908;
struct Cyc_Absyn_Tqual _temp1910; struct _tuple17* _temp1903= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1905=*
_temp1903; _LL1911: _temp1910= _temp1905.f1; goto _LL1909; _LL1909: _temp1908=
_temp1905.f2; goto _LL1907; _LL1907: _temp1906= _temp1905.f3; goto _LL1904;
_LL1904: { struct Cyc_List_List* _temp1914; struct Cyc_List_List* _temp1916;
struct _tuple0 _temp1912=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1917: _temp1916= _temp1912.f1; goto _LL1915; _LL1915:
_temp1914= _temp1912.f2; goto _LL1913; _LL1913: { void* _temp1918= Cyc_Parse_speclist2typ(
_temp1908, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1919=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1910, _temp1918, _temp1916, _temp1906), _temp1914);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1920=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1920[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1921; _temp1921.tag=
Cyc_StructFieldDeclList_tok; _temp1921.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1919); _temp1921;}); _temp1920;}); break;}}}
case 101: _LL1902: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1923=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1923[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1924; _temp1924.tag=
Cyc_QualSpecList_tok; _temp1924.f1=({ struct _tuple17* _temp1925=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1925->f1= Cyc_Absyn_empty_tqual();
_temp1925->f2=({ struct Cyc_List_List* _temp1926=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1926->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1926->tl=
0; _temp1926;}); _temp1925->f3= Cyc_yyget_AttributeList_tok( yyvs[ yyvsp_offset]);
_temp1925;}); _temp1924;}); _temp1923;}); break; case 102: _LL1922: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp1928=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1928[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1929; _temp1929.tag= Cyc_QualSpecList_tok;
_temp1929.f1=({ struct _tuple17* _temp1930=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1930->f1=(* Cyc_yyget_QualSpecList_tok( yyvs[
yyvsp_offset])).f1; _temp1930->f2=({ struct Cyc_List_List* _temp1931=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1931->hd=( void*)
Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1931->tl=(* Cyc_yyget_QualSpecList_tok( yyvs[
yyvsp_offset])).f2; _temp1931;}); _temp1930->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ yyvsp_offset])).f3); _temp1930;}); _temp1929;}); _temp1928;}); break; case
103: _LL1927: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1933=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1933[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1934; _temp1934.tag=
Cyc_QualSpecList_tok; _temp1934.f1=({ struct _tuple17* _temp1935=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1935->f1= Cyc_yyget_TypeQual_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1935->f2=
0; _temp1935->f3= Cyc_yyget_AttributeList_tok( yyvs[ yyvsp_offset]); _temp1935;});
_temp1934;}); _temp1933;}); break; case 104: _LL1932: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1937=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1937[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1938; _temp1938.tag= Cyc_QualSpecList_tok;
_temp1938.f1=({ struct _tuple17* _temp1939=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1939->f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ yyvsp_offset])).f1); _temp1939->f2=(* Cyc_yyget_QualSpecList_tok( yyvs[
yyvsp_offset])).f2; _temp1939->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ yyvsp_offset])).f3); _temp1939;}); _temp1938;}); _temp1937;}); break; case
105: _LL1936: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1941=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1941[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1942; _temp1942.tag=
Cyc_InitDeclList_tok; _temp1942.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[ yyvsp_offset]));
_temp1942;}); _temp1941;}); break; case 106: _LL1940: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1944=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1944[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1945; _temp1945.tag= Cyc_InitDeclList_tok;
_temp1945.f1=({ struct Cyc_List_List* _temp1946=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1946->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ yyvsp_offset]); _temp1946->tl= 0; _temp1946;}); _temp1945;}); _temp1944;});
break; case 107: _LL1943: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1948=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1948[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1949; _temp1949.tag=
Cyc_InitDeclList_tok; _temp1949.f1=({ struct Cyc_List_List* _temp1950=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1950->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ yyvsp_offset]); _temp1950->tl= Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1950;});
_temp1949;}); _temp1948;}); break; case 108: _LL1947: yyval=( void*)({ struct
Cyc_InitDecl_tok_struct* _temp1952=( struct Cyc_InitDecl_tok_struct*) _cycalloc(
sizeof( struct Cyc_InitDecl_tok_struct)); _temp1952[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1953; _temp1953.tag= Cyc_InitDecl_tok; _temp1953.f1=({ struct _tuple14*
_temp1954=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1954->f1=
Cyc_yyget_Declarator_tok( yyvs[ yyvsp_offset]); _temp1954->f2= 0; _temp1954;});
_temp1953;}); _temp1952;}); break; case 109: _LL1951: yyval=( void*)({ struct
Cyc_InitDecl_tok_struct* _temp1956=( struct Cyc_InitDecl_tok_struct*) _cycalloc(
sizeof( struct Cyc_InitDecl_tok_struct)); _temp1956[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1957; _temp1957.tag= Cyc_InitDecl_tok; _temp1957.f1=({ struct _tuple14*
_temp1958=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1958->f1=({
struct Cyc_Parse_Declarator* _temp1959=( struct Cyc_Parse_Declarator*) _cycalloc(
sizeof( struct Cyc_Parse_Declarator)); _temp1959->id=({ struct _tuple1*
_temp1960=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1960->f1=
Cyc_Absyn_rel_ns_null; _temp1960->f2= _init_tag_arr(( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char), 1u);
_temp1960;}); _temp1959->tms= 0; _temp1959;}); _temp1958->f2=( struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp1958;}); _temp1957;}); _temp1956;});
break; case 110: _LL1955: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1962=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1962[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1963; _temp1963.tag= Cyc_InitDecl_tok;
_temp1963.f1=({ struct _tuple14* _temp1964=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1964->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1964->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp1964;});
_temp1963;}); _temp1962;}); break; case 111: _LL1961: { struct Cyc_List_List*
_temp1966=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1967=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1967[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1968; _temp1968.tag= Cyc_TypeSpecifier_tok;
_temp1968.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1969=(
struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1969[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1970; _temp1970.tag= Cyc_Parse_Decl_spec;
_temp1970.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1966,({
struct Cyc_Core_Opt* _temp1971=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1971->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1971;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1970;}); _temp1969;})); _temp1968;});
_temp1967;}); break;} case 112: _LL1965: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1973=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1973[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1974; _temp1974.tag=
Cyc_TypeSpecifier_tok; _temp1974.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1975=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1975[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1976; _temp1976.tag= Cyc_Absyn_TunionType;
_temp1976.f1=({ struct Cyc_Absyn_TunionInfo _temp1977; _temp1977.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1978=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1978[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1979; _temp1979.tag= Cyc_Absyn_UnknownTunion;
_temp1979.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1980; _temp1980.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1980.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1980;});
_temp1979;}); _temp1978;})); _temp1977.targs= Cyc_yyget_TypeList_tok( yyvs[
yyvsp_offset]); _temp1977.rgn=( void*) Cyc_yyget_Type_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1977;});
_temp1976;}); _temp1975;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1974;}); _temp1973;}); break; case 113: _LL1972: {
void* _temp1982= Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp1991=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1991->v=( void*)((
void*) Cyc_Absyn_RgnKind); _temp1991;}), 0); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1983=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1983[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1984; _temp1984.tag=
Cyc_TypeSpecifier_tok; _temp1984.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1985=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1985[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1986; _temp1986.tag= Cyc_Absyn_TunionType;
_temp1986.f1=({ struct Cyc_Absyn_TunionInfo _temp1987; _temp1987.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1988=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1988[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1989; _temp1989.tag= Cyc_Absyn_UnknownTunion;
_temp1989.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1990; _temp1990.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1990.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1990;});
_temp1989;}); _temp1988;})); _temp1987.targs= Cyc_yyget_TypeList_tok( yyvs[
yyvsp_offset]); _temp1987.rgn=( void*) _temp1982; _temp1987;}); _temp1986;});
_temp1985;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp1984;}); _temp1983;}); break;} case 114: _LL1981: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1993=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1993[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1994; _temp1994.tag= Cyc_TypeSpecifier_tok;
_temp1994.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1995=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1995[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1996; _temp1996.tag= Cyc_Absyn_TunionFieldType; _temp1996.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1997; _temp1997.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp1998=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp1998[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp1999; _temp1999.tag= Cyc_Absyn_UnknownTunionfield; _temp1999.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2000; _temp2000.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2000.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp2000.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2000;});
_temp1999;}); _temp1998;})); _temp1997.targs= Cyc_yyget_TypeList_tok( yyvs[
yyvsp_offset]); _temp1997;}); _temp1996;}); _temp1995;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1994;}); _temp1993;}); break; case 115:
_LL1992: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2002=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp2002[ 0]=({ struct
Cyc_Bool_tok_struct _temp2003; _temp2003.tag= Cyc_Bool_tok; _temp2003.f1= 0;
_temp2003;}); _temp2002;}); break; case 116: _LL2001: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2005=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2005[ 0]=({ struct Cyc_Bool_tok_struct
_temp2006; _temp2006.tag= Cyc_Bool_tok; _temp2006.f1= 1; _temp2006;}); _temp2005;});
break; case 117: _LL2004: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2008=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2008[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2009; _temp2009.tag= Cyc_TunionFieldList_tok; _temp2009.f1=({ struct Cyc_List_List*
_temp2010=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2010->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[ yyvsp_offset]);
_temp2010->tl= 0; _temp2010;}); _temp2009;}); _temp2008;}); break; case 118:
_LL2007: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2012=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2012[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2013; _temp2013.tag=
Cyc_TunionFieldList_tok; _temp2013.f1=({ struct Cyc_List_List* _temp2014=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2014->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2014->tl= 0; _temp2014;}); _temp2013;}); _temp2012;});
break; case 119: _LL2011: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2016=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2016[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2017; _temp2017.tag= Cyc_TunionFieldList_tok; _temp2017.f1=({ struct Cyc_List_List*
_temp2018=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2018->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2018->tl= Cyc_yyget_TunionFieldList_tok(
yyvs[ yyvsp_offset]); _temp2018;}); _temp2017;}); _temp2016;}); break; case 120:
_LL2015: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2020=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2020[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2021; _temp2021.tag=
Cyc_TunionFieldList_tok; _temp2021.f1=({ struct Cyc_List_List* _temp2022=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2022->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2022->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
yyvsp_offset]); _temp2022;}); _temp2021;}); _temp2020;}); break; case 121:
_LL2019: yyval=( void*)({ struct Cyc_Scope_tok_struct* _temp2024=( struct Cyc_Scope_tok_struct*)
_cycalloc( sizeof( struct Cyc_Scope_tok_struct)); _temp2024[ 0]=({ struct Cyc_Scope_tok_struct
_temp2025; _temp2025.tag= Cyc_Scope_tok; _temp2025.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp2025;}); _temp2024;}); break; case 122: _LL2023: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2027=( struct Cyc_Scope_tok_struct*) _cycalloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2027[ 0]=({ struct Cyc_Scope_tok_struct
_temp2028; _temp2028.tag= Cyc_Scope_tok; _temp2028.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp2028;}); _temp2027;}); break; case 123: _LL2026: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2030=( struct Cyc_Scope_tok_struct*) _cycalloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2030[ 0]=({ struct Cyc_Scope_tok_struct
_temp2031; _temp2031.tag= Cyc_Scope_tok; _temp2031.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp2031;}); _temp2030;}); break; case 124: _LL2029: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp2033=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2033[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2034; _temp2034.tag= Cyc_TunionField_tok;
_temp2034.f1=({ struct Cyc_Absyn_Tunionfield* _temp2035=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2035->name= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); _temp2035->tvs= 0; _temp2035->typs= 0; _temp2035->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line); _temp2035->sc=( void*) Cyc_yyget_Scope_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2035;});
_temp2034;}); _temp2033;}); break; case 125: _LL2032: { struct Cyc_List_List*
_temp2037=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp2038=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp2039=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2039[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2040; _temp2040.tag= Cyc_TunionField_tok;
_temp2040.f1=({ struct Cyc_Absyn_Tunionfield* _temp2041=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2041->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2041->tvs=
_temp2038; _temp2041->typs= _temp2037; _temp2041->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ yylsp_offset]).last_line); _temp2041->sc=( void*) Cyc_yyget_Scope_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2041;});
_temp2040;}); _temp2039;}); break;} case 126: _LL2036: yyval= yyvs[ yyvsp_offset];
break; case 127: _LL2042: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2044=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2044[ 0]=({ struct Cyc_Declarator_tok_struct _temp2045; _temp2045.tag= Cyc_Declarator_tok;
_temp2045.f1=({ struct Cyc_Parse_Declarator* _temp2046=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2046->id=( Cyc_yyget_Declarator_tok(
yyvs[ yyvsp_offset]))->id; _temp2046->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ yyvsp_offset]))->tms); _temp2046;}); _temp2045;}); _temp2044;}); break;
case 128: _LL2043: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2048=(
struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2048[ 0]=({ struct Cyc_Declarator_tok_struct _temp2049; _temp2049.tag= Cyc_Declarator_tok;
_temp2049.f1=({ struct Cyc_Parse_Declarator* _temp2050=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2050->id= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); _temp2050->tms= 0; _temp2050;}); _temp2049;}); _temp2048;});
break; case 129: _LL2047: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; break; case 130: _LL2051: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2053=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2053[ 0]=({ struct Cyc_Declarator_tok_struct _temp2054; _temp2054.tag= Cyc_Declarator_tok;
_temp2054.f1=({ struct Cyc_Parse_Declarator* _temp2055=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2055->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp2055->tms=({ struct Cyc_List_List* _temp2056=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2056->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2056->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp2056;}); _temp2055;}); _temp2054;});
_temp2053;}); break; case 131: _LL2052: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2058=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2058[ 0]=({ struct Cyc_Declarator_tok_struct _temp2059; _temp2059.tag= Cyc_Declarator_tok;
_temp2059.f1=({ struct Cyc_Parse_Declarator* _temp2060=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2060->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2060->tms=({ struct Cyc_List_List* _temp2061=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2061->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2062=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2062[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2063; _temp2063.tag= Cyc_Absyn_ConstArray_mod;
_temp2063.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2063;}); _temp2062;})); _temp2061->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2061;}); _temp2060;}); _temp2059;}); _temp2058;}); break; case 132: _LL2057: {
struct _tuple16 _temp2067; struct Cyc_List_List* _temp2068; struct Cyc_Core_Opt*
_temp2070; struct Cyc_Absyn_VarargInfo* _temp2072; int _temp2074; struct Cyc_List_List*
_temp2076; struct _tuple16* _temp2065= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2067=*
_temp2065; _LL2077: _temp2076= _temp2067.f1; goto _LL2075; _LL2075: _temp2074=
_temp2067.f2; goto _LL2073; _LL2073: _temp2072= _temp2067.f3; goto _LL2071;
_LL2071: _temp2070= _temp2067.f4; goto _LL2069; _LL2069: _temp2068= _temp2067.f5;
goto _LL2066; _LL2066: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
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
_temp2083.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2084=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2084[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2085; _temp2085.tag= Cyc_Absyn_WithTypes;
_temp2085.f1= _temp2076; _temp2085.f2= _temp2074; _temp2085.f3= _temp2072;
_temp2085.f4= _temp2070; _temp2085.f5= _temp2068; _temp2085;}); _temp2084;}));
_temp2083;}); _temp2082;})); _temp2081->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2081;});
_temp2080;}); _temp2079;}); _temp2078;}); break;} case 133: _LL2064: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2087=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2087[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2088; _temp2088.tag= Cyc_Declarator_tok;
_temp2088.f1=({ struct Cyc_Parse_Declarator* _temp2089=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2089->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2089->tms=({ struct Cyc_List_List* _temp2090=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2090->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2091=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2091[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2092; _temp2092.tag= Cyc_Absyn_Function_mod;
_temp2092.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2093=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2093[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2094; _temp2094.tag= Cyc_Absyn_WithTypes;
_temp2094.f1= 0; _temp2094.f2= 0; _temp2094.f3= 0; _temp2094.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2094.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2094;}); _temp2093;})); _temp2092;}); _temp2091;}));
_temp2090->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2090;}); _temp2089;}); _temp2088;});
_temp2087;}); break; case 134: _LL2086: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2096=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2096[ 0]=({ struct Cyc_Declarator_tok_struct _temp2097; _temp2097.tag= Cyc_Declarator_tok;
_temp2097.f1=({ struct Cyc_Parse_Declarator* _temp2098=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2098->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2098->tms=({ struct Cyc_List_List* _temp2099=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2099->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2100=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2100[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2101; _temp2101.tag= Cyc_Absyn_Function_mod;
_temp2101.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2102=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2102[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2103; _temp2103.tag= Cyc_Absyn_NoTypes;
_temp2103.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2103.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2103;}); _temp2102;})); _temp2101;}); _temp2100;}));
_temp2099->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]))->tms; _temp2099;}); _temp2098;}); _temp2097;});
_temp2096;}); break; case 135: _LL2095: { struct Cyc_List_List* _temp2105=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2106=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2106[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2107; _temp2107.tag= Cyc_Declarator_tok;
_temp2107.f1=({ struct Cyc_Parse_Declarator* _temp2108=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2108->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2108->tms=({ struct Cyc_List_List* _temp2109=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2109->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2110=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2110[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2111; _temp2111.tag= Cyc_Absyn_TypeParams_mod;
_temp2111.f1= _temp2105; _temp2111.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2111.f3= 0; _temp2111;}); _temp2110;}));
_temp2109->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]))->tms; _temp2109;}); _temp2108;}); _temp2107;});
_temp2106;}); break;} case 136: _LL2104:(( struct Cyc_Lexing_lexbuf*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;{ struct Cyc_List_List*
_temp2113=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2114=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2114[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2115; _temp2115.tag= Cyc_Declarator_tok;
_temp2115.f1=({ struct Cyc_Parse_Declarator* _temp2116=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2116->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2116->tms=({ struct Cyc_List_List* _temp2117=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2117->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2118=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2118[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2119; _temp2119.tag= Cyc_Absyn_TypeParams_mod;
_temp2119.f1= _temp2113; _temp2119.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2119.f3= 0; _temp2119;}); _temp2118;}));
_temp2117->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]))->tms; _temp2117;}); _temp2116;}); _temp2115;});
_temp2114;}); break;} case 137: _LL2112: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2121=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2121[ 0]=({ struct Cyc_Declarator_tok_struct _temp2122; _temp2122.tag= Cyc_Declarator_tok;
_temp2122.f1=({ struct Cyc_Parse_Declarator* _temp2123=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2123->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2123->tms=({ struct Cyc_List_List* _temp2124=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2124->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2125=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2125[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2126; _temp2126.tag= Cyc_Absyn_Attributes_mod;
_temp2126.f1= Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line); _temp2126.f2= Cyc_yyget_AttributeList_tok( yyvs[
yyvsp_offset]); _temp2126;}); _temp2125;})); _temp2124->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms;
_temp2124;}); _temp2123;}); _temp2122;}); _temp2121;}); break; case 138: _LL2120:
Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef", sizeof(
unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2128=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2128[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2129; _temp2129.tag= Cyc_Declarator_tok;
_temp2129.f1=({ struct Cyc_Parse_Declarator* _temp2130=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2130->id= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); _temp2130->tms= 0; _temp2130;}); _temp2129;}); _temp2128;});
break; case 139: _LL2127: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2132=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2132[ 0]=({ struct Cyc_Declarator_tok_struct _temp2133; _temp2133.tag= Cyc_Declarator_tok;
_temp2133.f1=({ struct Cyc_Parse_Declarator* _temp2134=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2134->id= Cyc_Absyn_exn_name;
_temp2134->tms= 0; _temp2134;}); _temp2133;}); _temp2132;}); break; case 140:
_LL2131: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2136=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2136[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2137; _temp2137.tag=
Cyc_TypeModifierList_tok; _temp2137.f1=({ struct Cyc_List_List* _temp2138=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2138->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2139=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2139[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2140; _temp2140.tag= Cyc_Absyn_Pointer_mod;
_temp2140.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2140.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2140.f3= Cyc_Absyn_empty_tqual(); _temp2140;});
_temp2139;})); _temp2138->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ yyvsp_offset]), 0); _temp2138;}); _temp2137;}); _temp2136;}); break; case
141: _LL2135: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2142=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2142[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2143; _temp2143.tag=
Cyc_TypeModifierList_tok; _temp2143.f1=({ struct Cyc_List_List* _temp2144=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2144->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2145=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2145[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2146; _temp2146.tag= Cyc_Absyn_Pointer_mod;
_temp2146.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2146.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2146.f3= Cyc_yyget_TypeQual_tok( yyvs[ yyvsp_offset]);
_temp2146;}); _temp2145;})); _temp2144->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0); _temp2144;}); _temp2143;}); _temp2142;}); break; case
142: _LL2141: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2148=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2148[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2149; _temp2149.tag=
Cyc_TypeModifierList_tok; _temp2149.f1=({ struct Cyc_List_List* _temp2150=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2150->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2151=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2151[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2152; _temp2152.tag= Cyc_Absyn_Pointer_mod;
_temp2152.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2152.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2152.f3= Cyc_Absyn_empty_tqual(); _temp2152;});
_temp2151;})); _temp2150->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[ yyvsp_offset]));
_temp2150;}); _temp2149;}); _temp2148;}); break; case 143: _LL2147: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2154=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2154[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2155; _temp2155.tag= Cyc_TypeModifierList_tok;
_temp2155.f1=({ struct Cyc_List_List* _temp2156=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2156->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2157=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2157[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2158; _temp2158.tag= Cyc_Absyn_Pointer_mod;
_temp2158.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2158.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2158.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2158;});
_temp2157;})); _temp2156->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[ yyvsp_offset]));
_temp2156;}); _temp2155;}); _temp2154;}); break; case 144: _LL2153: yyval=( void*)({
struct Cyc_Pointer_Sort_tok_struct* _temp2160=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2160[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2161; _temp2161.tag= Cyc_Pointer_Sort_tok;
_temp2161.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2162=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2162[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2163; _temp2163.tag=
Cyc_Absyn_Nullable_ps; _temp2163.f1= Cyc_Absyn_exp_unsigned_one; _temp2163;});
_temp2162;})); _temp2161;}); _temp2160;}); break; case 145: _LL2159: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2165=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2165[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2166; _temp2166.tag= Cyc_Pointer_Sort_tok;
_temp2166.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2167=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2167[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2168; _temp2168.tag= Cyc_Absyn_NonNullable_ps; _temp2168.f1= Cyc_Absyn_exp_unsigned_one;
_temp2168;}); _temp2167;})); _temp2166;}); _temp2165;}); break; case 146:
_LL2164: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2170=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2170[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2171; _temp2171.tag=
Cyc_Pointer_Sort_tok; _temp2171.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2172=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2172[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2173; _temp2173.tag=
Cyc_Absyn_Nullable_ps; _temp2173.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2173;});
_temp2172;})); _temp2171;}); _temp2170;}); break; case 147: _LL2169: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2175=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2175[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2176; _temp2176.tag= Cyc_Pointer_Sort_tok;
_temp2176.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2177=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2177[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2178; _temp2178.tag= Cyc_Absyn_NonNullable_ps; _temp2178.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2178;});
_temp2177;})); _temp2176;}); _temp2175;}); break; case 148: _LL2174: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2180=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2180[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2181; _temp2181.tag= Cyc_Pointer_Sort_tok;
_temp2181.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2181;});
_temp2180;}); break; case 149: _LL2179: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2183=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2183[ 0]=({ struct Cyc_Type_tok_struct _temp2184; _temp2184.tag= Cyc_Type_tok;
_temp2184.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2185=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2185->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2185;}), 0); _temp2184;}); _temp2183;});
break; case 150: _LL2182: yyval= yyvs[ yyvsp_offset]; break; case 151: _LL2186:
Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ yyvsp_offset]; break; case 152: _LL2187: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2189=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2189[ 0]=({ struct Cyc_Type_tok_struct
_temp2190; _temp2190.tag= Cyc_Type_tok; _temp2190.f1=( void*) Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2191=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2191->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2191;}),
0); _temp2190;}); _temp2189;}); break; case 153: _LL2188: yyval= yyvs[
yyvsp_offset]; break; case 154: _LL2192: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2194=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2194[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2195; _temp2195.tag= Cyc_TypeQual_tok; _temp2195.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[ yyvsp_offset])); _temp2195;});
_temp2194;}); break; case 155: _LL2193: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2197=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2197[ 0]=({ struct Cyc_YY1_struct _temp2198; _temp2198.tag= Cyc_YY1;
_temp2198.f1=({ struct _tuple16* _temp2199=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2199->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2199->f2= 0;
_temp2199->f3= 0; _temp2199->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2199->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ yyvsp_offset]); _temp2199;}); _temp2198;}); _temp2197;}); break; case 156:
_LL2196: yyval=( void*)({ struct Cyc_YY1_struct* _temp2201=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2201[ 0]=({ struct Cyc_YY1_struct
_temp2202; _temp2202.tag= Cyc_YY1; _temp2202.f1=({ struct _tuple16* _temp2203=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2203->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2203->f2=
1; _temp2203->f3= 0; _temp2203->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2203->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ yyvsp_offset]); _temp2203;}); _temp2202;}); _temp2201;}); break; case 157:
_LL2200: { struct _tuple2 _temp2207; void* _temp2208; struct Cyc_Absyn_Tqual
_temp2210; struct Cyc_Core_Opt* _temp2212; struct _tuple2* _temp2205= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2207=*
_temp2205; _LL2213: _temp2212= _temp2207.f1; goto _LL2211; _LL2211: _temp2210=
_temp2207.f2; goto _LL2209; _LL2209: _temp2208= _temp2207.f3; goto _LL2206;
_LL2206: { struct Cyc_Absyn_VarargInfo* _temp2214=({ struct Cyc_Absyn_VarargInfo*
_temp2218=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2218->name= _temp2212; _temp2218->tq= _temp2210; _temp2218->type=( void*)
_temp2208; _temp2218->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2218;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2215=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2215[ 0]=({ struct Cyc_YY1_struct
_temp2216; _temp2216.tag= Cyc_YY1; _temp2216.f1=({ struct _tuple16* _temp2217=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2217->f1= 0;
_temp2217->f2= 0; _temp2217->f3= _temp2214; _temp2217->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2217->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ yyvsp_offset]); _temp2217;}); _temp2216;});
_temp2215;}); break;}} case 158: _LL2204: { struct _tuple2 _temp2222; void*
_temp2223; struct Cyc_Absyn_Tqual _temp2225; struct Cyc_Core_Opt* _temp2227;
struct _tuple2* _temp2220= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2222=*
_temp2220; _LL2228: _temp2227= _temp2222.f1; goto _LL2226; _LL2226: _temp2225=
_temp2222.f2; goto _LL2224; _LL2224: _temp2223= _temp2222.f3; goto _LL2221;
_LL2221: { struct Cyc_Absyn_VarargInfo* _temp2229=({ struct Cyc_Absyn_VarargInfo*
_temp2233=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2233->name= _temp2227; _temp2233->tq= _temp2225; _temp2233->type=( void*)
_temp2223; _temp2233->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2233;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2230=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2230[ 0]=({ struct Cyc_YY1_struct
_temp2231; _temp2231.tag= Cyc_YY1; _temp2231.f1=({ struct _tuple16* _temp2232=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2232->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2232->f2=
0; _temp2232->f3= _temp2229; _temp2232->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2232->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ yyvsp_offset]); _temp2232;}); _temp2231;}); _temp2230;}); break;}} case
159: _LL2219: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2235=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2235[
0]=({ struct Cyc_Type_tok_struct _temp2236; _temp2236.tag= Cyc_Type_tok;
_temp2236.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
yyvsp_offset]),( void*)({ struct Cyc_Absyn_Unknown_kb_struct* _temp2237=( struct
Cyc_Absyn_Unknown_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct));
_temp2237[ 0]=({ struct Cyc_Absyn_Unknown_kb_struct _temp2238; _temp2238.tag=
Cyc_Absyn_Unknown_kb; _temp2238.f1= 0; _temp2238;}); _temp2237;})); _temp2236;});
_temp2235;}); break; case 160: _LL2234: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2240=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2240[ 0]=({ struct Cyc_Type_tok_struct _temp2241; _temp2241.tag= Cyc_Type_tok;
_temp2241.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2242=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2242[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2243; _temp2243.tag= Cyc_Absyn_Eq_kb; _temp2243.f1=( void*) Cyc_yyget_Kind_tok(
yyvs[ yyvsp_offset]); _temp2243;}); _temp2242;})); _temp2241;}); _temp2240;});
break; case 161: _LL2239: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2245=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2245[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2246; _temp2246.tag= Cyc_TypeOpt_tok;
_temp2246.f1= 0; _temp2246;}); _temp2245;}); break; case 162: _LL2244: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2248=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2248[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2249; _temp2249.tag= Cyc_TypeOpt_tok; _temp2249.f1=({ struct Cyc_Core_Opt*
_temp2250=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2250->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2251=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2251[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2252; _temp2252.tag= Cyc_Absyn_JoinEff;
_temp2252.f1= Cyc_yyget_TypeList_tok( yyvs[ yyvsp_offset]); _temp2252;});
_temp2251;})); _temp2250;}); _temp2249;}); _temp2248;}); break; case 163:
_LL2247: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2254=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2254[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2255; _temp2255.tag= Cyc_Rgnorder_tok; _temp2255.f1= 0; _temp2255;});
_temp2254;}); break; case 164: _LL2253: yyval= yyvs[ yyvsp_offset]; break; case
165: _LL2256: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2258=(
struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2258[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2259; _temp2259.tag= Cyc_Rgnorder_tok;
_temp2259.f1=({ struct Cyc_List_List* _temp2260=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2260->hd=( void*)({ struct
_tuple18* _temp2261=( struct _tuple18*) _cycalloc( sizeof( struct _tuple18));
_temp2261->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2264=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2264[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2265; _temp2265.tag= Cyc_Absyn_Eq_kb; _temp2265.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2265;}); _temp2264;})); _temp2261->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ yyvsp_offset]),( void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp2262=(
struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp2262[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp2263; _temp2263.tag= Cyc_Absyn_Eq_kb;
_temp2263.f1=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2263;}); _temp2262;}));
_temp2261;}); _temp2260->tl= 0; _temp2260;}); _temp2259;}); _temp2258;}); break;
case 166: _LL2257: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2267=(
struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2267[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2268; _temp2268.tag= Cyc_Rgnorder_tok;
_temp2268.f1=({ struct Cyc_List_List* _temp2269=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2269->hd=( void*)({ struct
_tuple18* _temp2270=( struct _tuple18*) _cycalloc( sizeof( struct _tuple18));
_temp2270->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2273=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2273[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2274; _temp2274.tag= Cyc_Absyn_Eq_kb; _temp2274.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2274;}); _temp2273;})); _temp2270->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2271=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2271[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2272; _temp2272.tag= Cyc_Absyn_Eq_kb; _temp2272.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2272;}); _temp2271;})); _temp2270;}); _temp2269->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ yyvsp_offset]); _temp2269;}); _temp2268;}); _temp2267;}); break; case 167:
_LL2266: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2276=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp2276[ 0]=({ struct
Cyc_Bool_tok_struct _temp2277; _temp2277.tag= Cyc_Bool_tok; _temp2277.f1= 0;
_temp2277;}); _temp2276;}); break; case 168: _LL2275: { struct _tagged_arr
_temp2279= Cyc_yyget_String_tok( yyvs[ yyvsp_offset]); if( Cyc_Std_zstrcmp(
_temp2279, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2280=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2280[ 0]=({ struct Cyc_Bool_tok_struct _temp2281; _temp2281.tag= Cyc_Bool_tok;
_temp2281.f1= 1; _temp2281;}); _temp2280;}); break;} case 169: _LL2278: yyval=
yyvs[ yyvsp_offset]; break; case 170: _LL2282: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2284=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2284[ 0]=({ struct Cyc_TypeList_tok_struct _temp2285; _temp2285.tag= Cyc_TypeList_tok;
_temp2285.f1= Cyc_List_imp_append( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset])); _temp2285;}); _temp2284;}); break; case 171: _LL2283:
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2287=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2287[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2288; _temp2288.tag= Cyc_TypeList_tok; _temp2288.f1= 0; _temp2288;});
_temp2287;}); break; case 172: _LL2286: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 173:
_LL2289: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2291=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2291[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2292; _temp2292.tag= Cyc_TypeList_tok; _temp2292.f1=({ struct Cyc_List_List*
_temp2293=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2293->hd=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2294=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2294[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2295; _temp2295.tag= Cyc_Absyn_RgnsEff;
_temp2295.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2295;}); _temp2294;})); _temp2293->tl= 0;
_temp2293;}); _temp2292;}); _temp2291;}); break; case 174: _LL2290: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]),( void*) Cyc_Absyn_EffKind); yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2297=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2297[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2298; _temp2298.tag= Cyc_TypeList_tok; _temp2298.f1=({ struct Cyc_List_List*
_temp2299=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2299->hd=( void*) Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]); _temp2299->tl=
0; _temp2299;}); _temp2298;}); _temp2297;}); break; case 175: _LL2296: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]),( void*) Cyc_Absyn_RgnKind); yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2301=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2301[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2302; _temp2302.tag= Cyc_TypeList_tok; _temp2302.f1=({ struct Cyc_List_List*
_temp2303=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2303->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2304=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2304[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2305; _temp2305.tag= Cyc_Absyn_AccessEff;
_temp2305.f1=( void*) Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]); _temp2305;});
_temp2304;})); _temp2303->tl= 0; _temp2303;}); _temp2302;}); _temp2301;});
break; case 176: _LL2300: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*) Cyc_Absyn_RgnKind);
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2307=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2307[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2308; _temp2308.tag= Cyc_TypeList_tok; _temp2308.f1=({ struct Cyc_List_List*
_temp2309=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2309->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2310=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2310[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2311; _temp2311.tag= Cyc_Absyn_AccessEff;
_temp2311.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2311;}); _temp2310;})); _temp2309->tl= Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset]); _temp2309;}); _temp2308;}); _temp2307;}); break; case 177:
_LL2306: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp2313=(
struct Cyc_ParamDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp2313[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp2314; _temp2314.tag=
Cyc_ParamDeclList_tok; _temp2314.f1=({ struct Cyc_List_List* _temp2315=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2315->hd=( void*)
Cyc_yyget_ParamDecl_tok( yyvs[ yyvsp_offset]); _temp2315->tl= 0; _temp2315;});
_temp2314;}); _temp2313;}); break; case 178: _LL2312: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2317=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2317[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2318; _temp2318.tag= Cyc_ParamDeclList_tok;
_temp2318.f1=({ struct Cyc_List_List* _temp2319=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2319->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ yyvsp_offset]); _temp2319->tl= Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2319;});
_temp2318;}); _temp2317;}); break; case 179: _LL2316: { struct _tuple17
_temp2323; struct Cyc_List_List* _temp2324; struct Cyc_List_List* _temp2326;
struct Cyc_Absyn_Tqual _temp2328; struct _tuple17* _temp2321= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2323=*
_temp2321; _LL2329: _temp2328= _temp2323.f1; goto _LL2327; _LL2327: _temp2326=
_temp2323.f2; goto _LL2325; _LL2325: _temp2324= _temp2323.f3; goto _LL2322;
_LL2322: { struct Cyc_Parse_Declarator _temp2332; struct Cyc_List_List*
_temp2333; struct _tuple1* _temp2335; struct Cyc_Parse_Declarator* _temp2330=
Cyc_yyget_Declarator_tok( yyvs[ yyvsp_offset]); _temp2332=* _temp2330; _LL2336:
_temp2335= _temp2332.id; goto _LL2334; _LL2334: _temp2333= _temp2332.tms; goto
_LL2331; _LL2331: { void* _temp2337= Cyc_Parse_speclist2typ( _temp2326, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2340; struct Cyc_List_List* _temp2342; void*
_temp2344; struct Cyc_Absyn_Tqual _temp2346; struct _tuple6 _temp2338= Cyc_Parse_apply_tms(
_temp2328, _temp2337, _temp2324, _temp2333); _LL2347: _temp2346= _temp2338.f1;
goto _LL2345; _LL2345: _temp2344= _temp2338.f2; goto _LL2343; _LL2343: _temp2342=
_temp2338.f3; goto _LL2341; _LL2341: _temp2340= _temp2338.f4; goto _LL2339;
_LL2339: if( _temp2342 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line));} if( Cyc_Absyn_is_qvar_qualified( _temp2335)){
Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2348=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2352=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2352->v=( void*)(* _temp2335).f2; _temp2352;}); if( _temp2340 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2349=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2349[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2350; _temp2350.tag= Cyc_ParamDecl_tok; _temp2350.f1=({
struct _tuple2* _temp2351=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2351->f1= _temp2348; _temp2351->f2= _temp2346; _temp2351->f3= _temp2344;
_temp2351;}); _temp2350;}); _temp2349;}); break;}}}} case 180: _LL2320: { struct
_tuple17 _temp2356; struct Cyc_List_List* _temp2357; struct Cyc_List_List*
_temp2359; struct Cyc_Absyn_Tqual _temp2361; struct _tuple17* _temp2354= Cyc_yyget_QualSpecList_tok(
yyvs[ yyvsp_offset]); _temp2356=* _temp2354; _LL2362: _temp2361= _temp2356.f1;
goto _LL2360; _LL2360: _temp2359= _temp2356.f2; goto _LL2358; _LL2358: _temp2357=
_temp2356.f3; goto _LL2355; _LL2355: { void* _temp2363= Cyc_Parse_speclist2typ(
_temp2359, Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); if( _temp2357 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2364=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2364[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2365; _temp2365.tag= Cyc_ParamDecl_tok;
_temp2365.f1=({ struct _tuple2* _temp2366=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2366->f1= 0; _temp2366->f2= _temp2361; _temp2366->f3=
_temp2363; _temp2366;}); _temp2365;}); _temp2364;}); break;}} case 181: _LL2353: {
struct _tuple17 _temp2370; struct Cyc_List_List* _temp2371; struct Cyc_List_List*
_temp2373; struct Cyc_Absyn_Tqual _temp2375; struct _tuple17* _temp2368= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2370=*
_temp2368; _LL2376: _temp2375= _temp2370.f1; goto _LL2374; _LL2374: _temp2373=
_temp2370.f2; goto _LL2372; _LL2372: _temp2371= _temp2370.f3; goto _LL2369;
_LL2369: { void* _temp2377= Cyc_Parse_speclist2typ( _temp2373, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2378=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
yyvsp_offset]))->tms; struct Cyc_List_List* _temp2381; struct Cyc_List_List*
_temp2383; void* _temp2385; struct Cyc_Absyn_Tqual _temp2387; struct _tuple6
_temp2379= Cyc_Parse_apply_tms( _temp2375, _temp2377, _temp2371, _temp2378);
_LL2388: _temp2387= _temp2379.f1; goto _LL2386; _LL2386: _temp2385= _temp2379.f2;
goto _LL2384; _LL2384: _temp2383= _temp2379.f3; goto _LL2382; _LL2382: _temp2381=
_temp2379.f4; goto _LL2380; _LL2380: if( _temp2383 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type parameters on formal argument, ignoring", sizeof(
unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line));} if( _temp2381 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2389=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2389[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2390; _temp2390.tag= Cyc_ParamDecl_tok;
_temp2390.f1=({ struct _tuple2* _temp2391=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2391->f1= 0; _temp2391->f2= _temp2387; _temp2391->f3=
_temp2385; _temp2391;}); _temp2390;}); _temp2389;}); break;}} case 182: _LL2367:
yyval=( void*)({ struct Cyc_IdList_tok_struct* _temp2393=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2393[ 0]=({ struct Cyc_IdList_tok_struct
_temp2394; _temp2394.tag= Cyc_IdList_tok; _temp2394.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
yyvsp_offset])); _temp2394;}); _temp2393;}); break; case 183: _LL2392: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2396=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2396[ 0]=({ struct Cyc_IdList_tok_struct
_temp2397; _temp2397.tag= Cyc_IdList_tok; _temp2397.f1=({ struct Cyc_List_List*
_temp2398=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2398->hd=( void*)({ struct _tagged_arr* _temp2399=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2399[ 0]= Cyc_yyget_String_tok(
yyvs[ yyvsp_offset]); _temp2399;}); _temp2398->tl= 0; _temp2398;}); _temp2397;});
_temp2396;}); break; case 184: _LL2395: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2401=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2401[ 0]=({ struct Cyc_IdList_tok_struct _temp2402; _temp2402.tag= Cyc_IdList_tok;
_temp2402.f1=({ struct Cyc_List_List* _temp2403=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2403->hd=( void*)({ struct
_tagged_arr* _temp2404=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2404[ 0]= Cyc_yyget_String_tok( yyvs[ yyvsp_offset]);
_temp2404;}); _temp2403->tl= Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2403;});
_temp2402;}); _temp2401;}); break; case 185: _LL2400: yyval= yyvs[ yyvsp_offset];
break; case 186: _LL2405: yyval= yyvs[ yyvsp_offset]; break; case 187: _LL2406:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2408=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2408[ 0]=({ struct Cyc_Exp_tok_struct
_temp2409; _temp2409.tag= Cyc_Exp_tok; _temp2409.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2410=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2410[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2411; _temp2411.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2411.f1= 0; _temp2411.f2= 0; _temp2411;}); _temp2410;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2409;}); _temp2408;}); break; case 188:
_LL2407: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2413=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2413[ 0]=({ struct Cyc_Exp_tok_struct
_temp2414; _temp2414.tag= Cyc_Exp_tok; _temp2414.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2415=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2415[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2416; _temp2416.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2416.f1= 0; _temp2416.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2416;});
_temp2415;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2414;}); _temp2413;}); break; case 189: _LL2412: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2418=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2418[ 0]=({ struct Cyc_Exp_tok_struct
_temp2419; _temp2419.tag= Cyc_Exp_tok; _temp2419.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2420=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2420[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2421; _temp2421.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2421.f1= 0; _temp2421.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2421;});
_temp2420;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2419;}); _temp2418;}); break; case 190: _LL2417: { struct Cyc_Absyn_Vardecl*
_temp2423= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp2429=( struct _tuple1*)
_cycalloc( sizeof( struct _tuple1)); _temp2429->f1=( void*) Cyc_Absyn_Loc_n;
_temp2429->f2=({ struct _tagged_arr* _temp2430=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2430[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2430;});
_temp2429;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line))); _temp2423->tq=({ struct Cyc_Absyn_Tqual
_temp2424; _temp2424.q_const= 1; _temp2424.q_volatile= 0; _temp2424.q_restrict=
1; _temp2424;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2425=( struct
Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2425[
0]=({ struct Cyc_Exp_tok_struct _temp2426; _temp2426.tag= Cyc_Exp_tok; _temp2426.f1=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Comprehension_e_struct* _temp2427=(
struct Cyc_Absyn_Comprehension_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct));
_temp2427[ 0]=({ struct Cyc_Absyn_Comprehension_e_struct _temp2428; _temp2428.tag=
Cyc_Absyn_Comprehension_e; _temp2428.f1= _temp2423; _temp2428.f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2428.f3=
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2428;}); _temp2427;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2426;}); _temp2425;}); break;} case 191:
_LL2422: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2432=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2432[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2433; _temp2433.tag=
Cyc_InitializerList_tok; _temp2433.f1=({ struct Cyc_List_List* _temp2434=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2434->hd=(
void*)({ struct _tuple19* _temp2435=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2435->f1= 0; _temp2435->f2= Cyc_yyget_Exp_tok( yyvs[
yyvsp_offset]); _temp2435;}); _temp2434->tl= 0; _temp2434;}); _temp2433;});
_temp2432;}); break; case 192: _LL2431: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp2437=( struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp2437[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp2438; _temp2438.tag= Cyc_InitializerList_tok; _temp2438.f1=({ struct Cyc_List_List*
_temp2439=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2439->hd=( void*)({ struct _tuple19* _temp2440=( struct _tuple19*)
_cycalloc( sizeof( struct _tuple19)); _temp2440->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2440->f2=
Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp2440;}); _temp2439->tl= 0;
_temp2439;}); _temp2438;}); _temp2437;}); break; case 193: _LL2436: yyval=( void*)({
struct Cyc_InitializerList_tok_struct* _temp2442=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2442[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2443; _temp2443.tag= Cyc_InitializerList_tok;
_temp2443.f1=({ struct Cyc_List_List* _temp2444=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2444->hd=( void*)({ struct
_tuple19* _temp2445=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2445->f1= 0; _temp2445->f2= Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]);
_temp2445;}); _temp2444->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2444;});
_temp2443;}); _temp2442;}); break; case 194: _LL2441: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp2447=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2447[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2448; _temp2448.tag= Cyc_InitializerList_tok;
_temp2448.f1=({ struct Cyc_List_List* _temp2449=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2449->hd=( void*)({ struct
_tuple19* _temp2450=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2450->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2450->f2= Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]); _temp2450;}); _temp2449->tl= Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2449;});
_temp2448;}); _temp2447;}); break; case 195: _LL2446: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2452=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2452[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2453; _temp2453.tag= Cyc_DesignatorList_tok;
_temp2453.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2453;});
_temp2452;}); break; case 196: _LL2451: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2455=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2455[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2456; _temp2456.tag=
Cyc_DesignatorList_tok; _temp2456.f1=({ struct Cyc_List_List* _temp2457=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2457->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ yyvsp_offset]); _temp2457->tl= 0; _temp2457;});
_temp2456;}); _temp2455;}); break; case 197: _LL2454: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2459=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2459[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2460; _temp2460.tag= Cyc_DesignatorList_tok;
_temp2460.f1=({ struct Cyc_List_List* _temp2461=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2461->hd=( void*) Cyc_yyget_Designator_tok(
yyvs[ yyvsp_offset]); _temp2461->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2461;});
_temp2460;}); _temp2459;}); break; case 198: _LL2458: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2463=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2463[ 0]=({ struct
Cyc_Designator_tok_struct _temp2464; _temp2464.tag= Cyc_Designator_tok;
_temp2464.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2465=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2465[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2466; _temp2466.tag=
Cyc_Absyn_ArrayElement; _temp2466.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2466;});
_temp2465;})); _temp2464;}); _temp2463;}); break; case 199: _LL2462: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2468=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2468[ 0]=({ struct
Cyc_Designator_tok_struct _temp2469; _temp2469.tag= Cyc_Designator_tok;
_temp2469.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2470=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2470[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2471; _temp2471.tag= Cyc_Absyn_FieldName;
_temp2471.f1=({ struct _tagged_arr* _temp2472=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2472[ 0]= Cyc_yyget_String_tok( yyvs[
yyvsp_offset]); _temp2472;}); _temp2471;}); _temp2470;})); _temp2469;});
_temp2468;}); break; case 200: _LL2467: { void* _temp2474= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ yyvsp_offset])).f2, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); struct Cyc_List_List*
_temp2475=(* Cyc_yyget_QualSpecList_tok( yyvs[ yyvsp_offset])).f3; if( _temp2475
!=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type", sizeof(
unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line));}{ struct Cyc_Absyn_Tqual _temp2476=(* Cyc_yyget_QualSpecList_tok(
yyvs[ yyvsp_offset])).f1; yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2477=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2477[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2478; _temp2478.tag= Cyc_ParamDecl_tok;
_temp2478.f1=({ struct _tuple2* _temp2479=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2479->f1= 0; _temp2479->f2= _temp2476; _temp2479->f3=
_temp2474; _temp2479;}); _temp2478;}); _temp2477;}); break;}} case 201: _LL2473: {
void* _temp2481= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2482=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2483=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2484=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ yyvsp_offset]))->tms; struct _tuple6 _temp2485= Cyc_Parse_apply_tms(
_temp2483, _temp2481, _temp2482, _temp2484); if( _temp2485.f3 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line));} if(
_temp2485.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2486=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2486[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2487; _temp2487.tag= Cyc_ParamDecl_tok;
_temp2487.f1=({ struct _tuple2* _temp2488=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2488->f1= 0; _temp2488->f2= _temp2485.f1; _temp2488->f3=
_temp2485.f2; _temp2488;}); _temp2487;}); _temp2486;}); break;} case 202:
_LL2480: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2490=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2490[ 0]=({ struct Cyc_Type_tok_struct
_temp2491; _temp2491.tag= Cyc_Type_tok; _temp2491.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ yyvsp_offset])).f3; _temp2491;}); _temp2490;}); break; case 203: _LL2489:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2493=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2493[ 0]=({ struct Cyc_Type_tok_struct
_temp2494; _temp2494.tag= Cyc_Type_tok; _temp2494.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp2495=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2495[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2496; _temp2496.tag= Cyc_Absyn_JoinEff; _temp2496.f1=
0; _temp2496;}); _temp2495;})); _temp2494;}); _temp2493;}); break; case 204:
_LL2492: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2498=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2498[ 0]=({ struct Cyc_Type_tok_struct
_temp2499; _temp2499.tag= Cyc_Type_tok; _temp2499.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp2500=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2500[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2501; _temp2501.tag= Cyc_Absyn_JoinEff; _temp2501.f1=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2501;}); _temp2500;})); _temp2499;}); _temp2498;});
break; case 205: _LL2497: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2503=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2503[ 0]=({ struct Cyc_Type_tok_struct _temp2504; _temp2504.tag= Cyc_Type_tok;
_temp2504.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2505=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2505[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2506; _temp2506.tag= Cyc_Absyn_RgnsEff;
_temp2506.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2506;}); _temp2505;})); _temp2504;});
_temp2503;}); break; case 206: _LL2502: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2508=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2508[ 0]=({ struct Cyc_Type_tok_struct _temp2509; _temp2509.tag= Cyc_Type_tok;
_temp2509.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2510=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2510[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2511; _temp2511.tag= Cyc_Absyn_JoinEff;
_temp2511.f1=({ struct Cyc_List_List* _temp2512=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2512->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2512->tl=
Cyc_yyget_TypeList_tok( yyvs[ yyvsp_offset]); _temp2512;}); _temp2511;});
_temp2510;})); _temp2509;}); _temp2508;}); break; case 207: _LL2507: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2514=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2514[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2515; _temp2515.tag= Cyc_TypeList_tok; _temp2515.f1=({ struct Cyc_List_List*
_temp2516=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2516->hd=( void*) Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]); _temp2516->tl=
0; _temp2516;}); _temp2515;}); _temp2514;}); break; case 208: _LL2513: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2518=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2518[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2519; _temp2519.tag= Cyc_TypeList_tok; _temp2519.f1=({ struct Cyc_List_List*
_temp2520=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2520->hd=( void*) Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]); _temp2520->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2520;}); _temp2519;}); _temp2518;}); break; case 209:
_LL2517: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2522=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2522[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2523; _temp2523.tag=
Cyc_AbstractDeclarator_tok; _temp2523.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2524=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2524->tms= Cyc_yyget_TypeModifierList_tok( yyvs[ yyvsp_offset]); _temp2524;});
_temp2523;}); _temp2522;}); break; case 210: _LL2521: yyval= yyvs[ yyvsp_offset];
break; case 211: _LL2525: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2527=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2527[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2528; _temp2528.tag= Cyc_AbstractDeclarator_tok; _temp2528.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2529=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2529->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ yyvsp_offset]))->tms); _temp2529;}); _temp2528;}); _temp2527;}); break;
case 212: _LL2526: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; break; case 213: _LL2530: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2532=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2532[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2533; _temp2533.tag= Cyc_AbstractDeclarator_tok; _temp2533.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2534=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2534->tms=({ struct Cyc_List_List* _temp2535=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2535->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2535->tl= 0; _temp2535;}); _temp2534;}); _temp2533;}); _temp2532;}); break;
case 214: _LL2531: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2537=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2537[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2538; _temp2538.tag= Cyc_AbstractDeclarator_tok; _temp2538.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2539=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2539->tms=({ struct Cyc_List_List* _temp2540=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2540->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2540->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2540;});
_temp2539;}); _temp2538;}); _temp2537;}); break; case 215: _LL2536: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2542=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2542[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2543; _temp2543.tag= Cyc_AbstractDeclarator_tok;
_temp2543.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2544=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2544->tms=({
struct Cyc_List_List* _temp2545=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2545->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2546=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2546[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2547; _temp2547.tag= Cyc_Absyn_ConstArray_mod; _temp2547.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2547;});
_temp2546;})); _temp2545->tl= 0; _temp2545;}); _temp2544;}); _temp2543;});
_temp2542;}); break; case 216: _LL2541: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2549=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2549[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2550; _temp2550.tag= Cyc_AbstractDeclarator_tok; _temp2550.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2551=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2551->tms=({ struct Cyc_List_List* _temp2552=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2552->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2553=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2553[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2554; _temp2554.tag= Cyc_Absyn_ConstArray_mod;
_temp2554.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2554;}); _temp2553;})); _temp2552->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2552;}); _temp2551;}); _temp2550;}); _temp2549;}); break; case 217: _LL2548:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2556=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2556[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2557; _temp2557.tag=
Cyc_AbstractDeclarator_tok; _temp2557.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2558=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2558->tms=({ struct Cyc_List_List* _temp2559=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2559->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2560=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2560[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2561; _temp2561.tag= Cyc_Absyn_Function_mod;
_temp2561.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2562=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2562[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2563; _temp2563.tag= Cyc_Absyn_WithTypes;
_temp2563.f1= 0; _temp2563.f2= 0; _temp2563.f3= 0; _temp2563.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2563.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2563;}); _temp2562;})); _temp2561;}); _temp2560;}));
_temp2559->tl= 0; _temp2559;}); _temp2558;}); _temp2557;}); _temp2556;}); break;
case 218: _LL2555: { struct _tuple16 _temp2567; struct Cyc_List_List* _temp2568;
struct Cyc_Core_Opt* _temp2570; struct Cyc_Absyn_VarargInfo* _temp2572; int
_temp2574; struct Cyc_List_List* _temp2576; struct _tuple16* _temp2565= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2567=*
_temp2565; _LL2577: _temp2576= _temp2567.f1; goto _LL2575; _LL2575: _temp2574=
_temp2567.f2; goto _LL2573; _LL2573: _temp2572= _temp2567.f3; goto _LL2571;
_LL2571: _temp2570= _temp2567.f4; goto _LL2569; _LL2569: _temp2568= _temp2567.f5;
goto _LL2566; _LL2566: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2578=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2578[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2579; _temp2579.tag= Cyc_AbstractDeclarator_tok; _temp2579.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2580=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2580->tms=({ struct Cyc_List_List* _temp2581=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2581->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2582=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2582[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2583; _temp2583.tag= Cyc_Absyn_Function_mod;
_temp2583.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2584=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2584[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2585; _temp2585.tag= Cyc_Absyn_WithTypes;
_temp2585.f1= _temp2576; _temp2585.f2= _temp2574; _temp2585.f3= _temp2572;
_temp2585.f4= _temp2570; _temp2585.f5= _temp2568; _temp2585;}); _temp2584;}));
_temp2583;}); _temp2582;})); _temp2581->tl= 0; _temp2581;}); _temp2580;});
_temp2579;}); _temp2578;}); break;} case 219: _LL2564: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2587=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2587[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2588; _temp2588.tag= Cyc_AbstractDeclarator_tok;
_temp2588.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2589=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2589->tms=({
struct Cyc_List_List* _temp2590=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2590->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2591=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2591[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2592; _temp2592.tag=
Cyc_Absyn_Function_mod; _temp2592.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2593=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2593[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2594; _temp2594.tag= Cyc_Absyn_WithTypes;
_temp2594.f1= 0; _temp2594.f2= 0; _temp2594.f3= 0; _temp2594.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2594.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2594;}); _temp2593;})); _temp2592;}); _temp2591;}));
_temp2590->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2590;});
_temp2589;}); _temp2588;}); _temp2587;}); break; case 220: _LL2586: { struct
_tuple16 _temp2598; struct Cyc_List_List* _temp2599; struct Cyc_Core_Opt*
_temp2601; struct Cyc_Absyn_VarargInfo* _temp2603; int _temp2605; struct Cyc_List_List*
_temp2607; struct _tuple16* _temp2596= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2598=*
_temp2596; _LL2608: _temp2607= _temp2598.f1; goto _LL2606; _LL2606: _temp2605=
_temp2598.f2; goto _LL2604; _LL2604: _temp2603= _temp2598.f3; goto _LL2602;
_LL2602: _temp2601= _temp2598.f4; goto _LL2600; _LL2600: _temp2599= _temp2598.f5;
goto _LL2597; _LL2597: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2609=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2609[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2610; _temp2610.tag= Cyc_AbstractDeclarator_tok; _temp2610.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2611=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2611->tms=({ struct Cyc_List_List* _temp2612=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2612->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2613=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2613[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2614; _temp2614.tag= Cyc_Absyn_Function_mod;
_temp2614.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2615=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2615[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2616; _temp2616.tag= Cyc_Absyn_WithTypes;
_temp2616.f1= _temp2607; _temp2616.f2= _temp2605; _temp2616.f3= _temp2603;
_temp2616.f4= _temp2601; _temp2616.f5= _temp2599; _temp2616;}); _temp2615;}));
_temp2614;}); _temp2613;})); _temp2612->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2612;}); _temp2611;}); _temp2610;}); _temp2609;}); break;} case 221:
_LL2595: { struct Cyc_List_List* _temp2618=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2619=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2619[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2620; _temp2620.tag= Cyc_AbstractDeclarator_tok;
_temp2620.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2621=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2621->tms=({
struct Cyc_List_List* _temp2622=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2622->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2623=( struct Cyc_Absyn_TypeParams_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2623[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2624; _temp2624.tag= Cyc_Absyn_TypeParams_mod; _temp2624.f1= _temp2618;
_temp2624.f2= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line);
_temp2624.f3= 0; _temp2624;}); _temp2623;})); _temp2622->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2622;}); _temp2621;}); _temp2620;}); _temp2619;}); break;} case 222:
_LL2617:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2626=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2627=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2627[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2628; _temp2628.tag= Cyc_AbstractDeclarator_tok;
_temp2628.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2629=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2629->tms=({
struct Cyc_List_List* _temp2630=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2630->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2631=( struct Cyc_Absyn_TypeParams_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2631[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2632; _temp2632.tag= Cyc_Absyn_TypeParams_mod; _temp2632.f1= _temp2626;
_temp2632.f2= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line);
_temp2632.f3= 0; _temp2632;}); _temp2631;})); _temp2630->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2630;}); _temp2629;}); _temp2628;}); _temp2627;}); break;} case 223:
_LL2625: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2634=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2634[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2635; _temp2635.tag=
Cyc_AbstractDeclarator_tok; _temp2635.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2636=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2636->tms=({ struct Cyc_List_List* _temp2637=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2637->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2638=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2638[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2639; _temp2639.tag= Cyc_Absyn_Attributes_mod;
_temp2639.f1= Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line); _temp2639.f2= Cyc_yyget_AttributeList_tok( yyvs[
yyvsp_offset]); _temp2639;}); _temp2638;})); _temp2637->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms;
_temp2637;}); _temp2636;}); _temp2635;}); _temp2634;}); break; case 224: _LL2633:
yyval= yyvs[ yyvsp_offset]; break; case 225: _LL2640: yyval= yyvs[ yyvsp_offset];
break; case 226: _LL2641: yyval= yyvs[ yyvsp_offset]; break; case 227: _LL2642:
yyval= yyvs[ yyvsp_offset]; break; case 228: _LL2643: yyval= yyvs[ yyvsp_offset];
break; case 229: _LL2644: yyval= yyvs[ yyvsp_offset]; break; case 230: _LL2645:
if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]), _tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){
Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H", sizeof(
unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2657=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2657->name=({ struct
_tagged_arr* _temp2660=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2660[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2660;});
_temp2657->identity= 0; _temp2657->kind=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp2658=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp2658[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp2659; _temp2659.tag= Cyc_Absyn_Eq_kb;
_temp2659.f1=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2659;}); _temp2658;}));
_temp2657;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2655=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2655[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2656; _temp2656.tag= Cyc_Absyn_VarType;
_temp2656.f1= tv; _temp2656;}); _temp2655;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2647=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2647[ 0]=({ struct Cyc_Stmt_tok_struct _temp2648; _temp2648.tag= Cyc_Stmt_tok;
_temp2648.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2649=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2649[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2650; _temp2650.tag= Cyc_Absyn_Region_s;
_temp2650.f1= tv; _temp2650.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2651=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2651->f1=(
void*) Cyc_Absyn_Loc_n; _temp2651->f2=({ struct _tagged_arr* _temp2652=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2652[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2652;});
_temp2651;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2653=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2653[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2654; _temp2654.tag=
Cyc_Absyn_RgnHandleType; _temp2654.f1=( void*) t; _temp2654;}); _temp2653;}), 0);
_temp2650.f3= Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]); _temp2650;}); _temp2649;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2648;});
_temp2647;}); break;} case 231: _LL2646: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2672=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2672->name=({ struct
_tagged_arr* _temp2675=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2675[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2677; _temp2677.tag= Cyc_Std_String_pa; _temp2677.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2676[ 1u]={& _temp2677}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2676, sizeof( void*), 1u));}});
_temp2675;}); _temp2672->identity= 0; _temp2672->kind=( void*)(( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2673=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2673[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2674; _temp2674.tag= Cyc_Absyn_Eq_kb; _temp2674.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2674;}); _temp2673;})); _temp2672;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2670=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2670[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2671; _temp2671.tag= Cyc_Absyn_VarType;
_temp2671.f1= tv; _temp2671;}); _temp2670;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2662=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2662[ 0]=({ struct Cyc_Stmt_tok_struct _temp2663; _temp2663.tag= Cyc_Stmt_tok;
_temp2663.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2664=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2664[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2665; _temp2665.tag= Cyc_Absyn_Region_s;
_temp2665.f1= tv; _temp2665.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2666=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2666->f1=(
void*) Cyc_Absyn_Loc_n; _temp2666->f2=({ struct _tagged_arr* _temp2667=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2667[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2667;});
_temp2666;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2668=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2668[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2669; _temp2669.tag=
Cyc_Absyn_RgnHandleType; _temp2669.f1=( void*) t; _temp2669;}); _temp2668;}), 0);
_temp2665.f3= Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]); _temp2665;}); _temp2664;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2663;});
_temp2662;}); break;} case 232: _LL2661: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2679=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2679[ 0]=({ struct Cyc_Stmt_tok_struct _temp2680; _temp2680.tag= Cyc_Stmt_tok;
_temp2680.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2681=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2681[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2682; _temp2682.tag= Cyc_Absyn_Cut_s;
_temp2682.f1= Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]); _temp2682;}); _temp2681;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2680;});
_temp2679;}); break; case 233: _LL2678: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2684=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2684[ 0]=({ struct Cyc_Stmt_tok_struct _temp2685; _temp2685.tag= Cyc_Stmt_tok;
_temp2685.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2686=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2686[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2687; _temp2687.tag= Cyc_Absyn_Splice_s;
_temp2687.f1= Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]); _temp2687;}); _temp2686;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2685;});
_temp2684;}); break; case 234: _LL2683: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2689=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2689[ 0]=({ struct Cyc_Stmt_tok_struct _temp2690; _temp2690.tag= Cyc_Stmt_tok;
_temp2690.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2691=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2691[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2692; _temp2692.tag= Cyc_Absyn_Label_s;
_temp2692.f1=({ struct _tagged_arr* _temp2693=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2693[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2693;});
_temp2692.f2= Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]); _temp2692;}); _temp2691;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2690;});
_temp2689;}); break; case 235: _LL2688: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2695=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2695[ 0]=({ struct Cyc_Stmt_tok_struct _temp2696; _temp2696.tag= Cyc_Stmt_tok;
_temp2696.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2696;});
_temp2695;}); break; case 236: _LL2694: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2698=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2698[ 0]=({ struct Cyc_Stmt_tok_struct _temp2699; _temp2699.tag= Cyc_Stmt_tok;
_temp2699.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2699;}); _temp2698;}); break; case 237:
_LL2697: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2701=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2701[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2702; _temp2702.tag= Cyc_Stmt_tok; _temp2702.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2702;}); _temp2701;}); break; case 238:
_LL2700: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
break; case 239: _LL2703: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2705=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2705[ 0]=({ struct Cyc_Stmt_tok_struct _temp2706; _temp2706.tag= Cyc_Stmt_tok;
_temp2706.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
yyvsp_offset]), Cyc_Absyn_skip_stmt( 0)); _temp2706;}); _temp2705;}); break;
case 240: _LL2704: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2708=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2708[ 0]=({ struct Cyc_Stmt_tok_struct _temp2709; _temp2709.tag= Cyc_Stmt_tok;
_temp2709.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset])); _temp2709;}); _temp2708;}); break; case 241: _LL2707:
yyval= yyvs[ yyvsp_offset]; break; case 242: _LL2710: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2712=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2712[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2713; _temp2713.tag= Cyc_Stmt_tok; _temp2713.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2713;}); _temp2712;}); break; case 243: _LL2711:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2715=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2715[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2716; _temp2716.tag= Cyc_Stmt_tok; _temp2716.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2717=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2717[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2718; _temp2718.tag= Cyc_Absyn_Fn_d;
_temp2718.f1= Cyc_yyget_FnDecl_tok( yyvs[ yyvsp_offset]); _temp2718;});
_temp2717;}), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)), Cyc_Absyn_skip_stmt( 0)); _temp2716;});
_temp2715;}); break; case 244: _LL2714: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2720=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2720[ 0]=({ struct Cyc_Stmt_tok_struct _temp2721; _temp2721.tag= Cyc_Stmt_tok;
_temp2721.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2722=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2722[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2723; _temp2723.tag= Cyc_Absyn_Fn_d;
_temp2723.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2723;}); _temp2722;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset])); _temp2721;}); _temp2720;}); break;
case 245: _LL2719: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2725=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2725[ 0]=({ struct Cyc_Stmt_tok_struct _temp2726; _temp2726.tag= Cyc_Stmt_tok;
_temp2726.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2726;}); _temp2725;}); break; case 246:
_LL2724: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2728=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2728[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2729; _temp2729.tag= Cyc_Stmt_tok; _temp2729.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2729;}); _temp2728;}); break; case 247:
_LL2727: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2731=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2731[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2732; _temp2732.tag= Cyc_Stmt_tok; _temp2732.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2732;}); _temp2731;}); break; case 248:
_LL2730: if( Cyc_Std_strcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
yylsp_offset]).last_line));} yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2734=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2734[ 0]=({ struct Cyc_Stmt_tok_struct _temp2735; _temp2735.tag= Cyc_Stmt_tok;
_temp2735.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_SwitchC_s_struct*
_temp2736=( struct Cyc_Absyn_SwitchC_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct));
_temp2736[ 0]=({ struct Cyc_Absyn_SwitchC_s_struct _temp2737; _temp2737.tag= Cyc_Absyn_SwitchC_s;
_temp2737.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2737.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2737;});
_temp2736;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2735;}); _temp2734;}); break; case 249: _LL2733: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2739=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2739[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2740; _temp2740.tag= Cyc_Stmt_tok; _temp2740.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2740;}); _temp2739;}); break; case 250: _LL2738:
yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2742=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2742[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2743; _temp2743.tag= Cyc_SwitchClauseList_tok;
_temp2743.f1= 0; _temp2743;}); _temp2742;}); break; case 251: _LL2741: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2745=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2745[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2746; _temp2746.tag= Cyc_SwitchClauseList_tok;
_temp2746.f1=({ struct Cyc_List_List* _temp2747=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2747->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2748=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2748->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2748->pat_vars= 0; _temp2748->where_clause= 0; _temp2748->body= Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]); _temp2748->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2748;}); _temp2747->tl= 0; _temp2747;});
_temp2746;}); _temp2745;}); break; case 252: _LL2744: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2750=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2750[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2751; _temp2751.tag= Cyc_SwitchClauseList_tok;
_temp2751.f1=({ struct Cyc_List_List* _temp2752=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2752->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2753=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2753->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2753->pat_vars= 0; _temp2753->where_clause= 0;
_temp2753->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2753->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2753;}); _temp2752->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ yyvsp_offset]); _temp2752;}); _temp2751;}); _temp2750;}); break; case 253:
_LL2749: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2755=(
struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp2755[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp2756; _temp2756.tag=
Cyc_SwitchClauseList_tok; _temp2756.f1=({ struct Cyc_List_List* _temp2757=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2757->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp2758=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2758->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2758->pat_vars=
0; _temp2758->where_clause= 0; _temp2758->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2758->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2758;}); _temp2757->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ yyvsp_offset]); _temp2757;}); _temp2756;}); _temp2755;}); break; case 254:
_LL2754: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2760=(
struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp2760[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp2761; _temp2761.tag=
Cyc_SwitchClauseList_tok; _temp2761.f1=({ struct Cyc_List_List* _temp2762=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2762->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp2763=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2763->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2763->pat_vars=
0; _temp2763->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2763->body=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2763->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2763;}); _temp2762->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ yyvsp_offset]); _temp2762;}); _temp2761;}); _temp2760;}); break; case 255:
_LL2759: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2765=(
struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp2765[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp2766; _temp2766.tag=
Cyc_SwitchClauseList_tok; _temp2766.f1=({ struct Cyc_List_List* _temp2767=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2767->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp2768=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2768->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2768->pat_vars=
0; _temp2768->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2768->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2768->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2768;}); _temp2767->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ yyvsp_offset]); _temp2767;}); _temp2766;}); _temp2765;}); break; case 256:
_LL2764: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2770=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2770[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2771; _temp2771.tag=
Cyc_SwitchCClauseList_tok; _temp2771.f1= 0; _temp2771;}); _temp2770;}); break;
case 257: _LL2769: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2773=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2773[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2774; _temp2774.tag= Cyc_SwitchCClauseList_tok; _temp2774.f1=({ struct Cyc_List_List*
_temp2775=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2775->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2776=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2776->cnst_exp= 0;
_temp2776->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]),
Cyc_Absyn_break_stmt( 0), 0); _temp2776->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2776;}); _temp2775->tl= 0; _temp2775;});
_temp2774;}); _temp2773;}); break; case 258: _LL2772: yyval=( void*)({ struct
Cyc_SwitchCClauseList_tok_struct* _temp2778=( struct Cyc_SwitchCClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp2778[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp2779; _temp2779.tag= Cyc_SwitchCClauseList_tok;
_temp2779.f1=({ struct Cyc_List_List* _temp2780=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2780->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause*
_temp2781=( struct Cyc_Absyn_SwitchC_clause*) _cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause));
_temp2781->cnst_exp=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2781->body=
Cyc_Absyn_fallthru_stmt( 0, 0); _temp2781->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line); _temp2781;}); _temp2780->tl= Cyc_yyget_SwitchCClauseList_tok(
yyvs[ yyvsp_offset]); _temp2780;}); _temp2779;}); _temp2778;}); break; case 259:
_LL2777: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2783=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2783[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2784; _temp2784.tag=
Cyc_SwitchCClauseList_tok; _temp2784.f1=({ struct Cyc_List_List* _temp2785=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2785->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2786=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2786->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2786->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2786->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2786;}); _temp2785->tl= Cyc_yyget_SwitchCClauseList_tok(
yyvs[ yyvsp_offset]); _temp2785;}); _temp2784;}); _temp2783;}); break; case 260:
_LL2782: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2788=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2788[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2789; _temp2789.tag= Cyc_Stmt_tok; _temp2789.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2789;}); _temp2788;}); break; case 261: _LL2787:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2791=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2791[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2792; _temp2792.tag= Cyc_Stmt_tok; _temp2792.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2792;}); _temp2791;}); break; case 262:
_LL2790: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2794=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2794[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2795; _temp2795.tag= Cyc_Stmt_tok; _temp2795.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2795;}); _temp2794;}); break; case 263: _LL2793:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2797=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2797[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2798; _temp2798.tag= Cyc_Stmt_tok; _temp2798.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2798;}); _temp2797;}); break; case 264: _LL2796:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2800=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2800[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2801; _temp2801.tag= Cyc_Stmt_tok; _temp2801.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2801;}); _temp2800;}); break; case 265: _LL2799:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2803=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2803[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2804; _temp2804.tag= Cyc_Stmt_tok; _temp2804.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2804;}); _temp2803;}); break; case 266: _LL2802: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2806=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2806[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2807; _temp2807.tag= Cyc_Stmt_tok; _temp2807.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2807;}); _temp2806;}); break; case 267:
_LL2805: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2809=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2809[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2810; _temp2810.tag= Cyc_Stmt_tok; _temp2810.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2810;}); _temp2809;}); break; case 268:
_LL2808: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2812=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2812[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2813; _temp2813.tag= Cyc_Stmt_tok; _temp2813.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2813;}); _temp2812;}); break; case 269: _LL2811:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2815=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2815[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2816; _temp2816.tag= Cyc_Stmt_tok; _temp2816.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2816;}); _temp2815;}); break; case 270: _LL2814: {
struct Cyc_List_List* _temp2818= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt*
_temp2819= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ yylsp_offset]).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2820=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2820[ 0]=({ struct Cyc_Stmt_tok_struct _temp2821; _temp2821.tag= Cyc_Stmt_tok;
_temp2821.f1= Cyc_Parse_flatten_declarations( _temp2818, _temp2819); _temp2821;});
_temp2820;}); break;} case 271: _LL2817: { struct Cyc_List_List* _temp2823= Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2824= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
yylsp_offset]).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2825=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2825[ 0]=({ struct Cyc_Stmt_tok_struct _temp2826; _temp2826.tag= Cyc_Stmt_tok;
_temp2826.f1= Cyc_Parse_flatten_declarations( _temp2823, _temp2824); _temp2826;});
_temp2825;}); break;} case 272: _LL2822: { struct Cyc_List_List* _temp2828= Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2829= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ yylsp_offset]).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2830=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2830[ 0]=({ struct Cyc_Stmt_tok_struct _temp2831; _temp2831.tag= Cyc_Stmt_tok;
_temp2831.f1= Cyc_Parse_flatten_declarations( _temp2828, _temp2829); _temp2831;});
_temp2830;}); break;} case 273: _LL2827: { struct Cyc_List_List* _temp2833= Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2834= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
yylsp_offset]).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2835=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2835[ 0]=({ struct Cyc_Stmt_tok_struct _temp2836; _temp2836.tag= Cyc_Stmt_tok;
_temp2836.f1= Cyc_Parse_flatten_declarations( _temp2833, _temp2834); _temp2836;});
_temp2835;}); break;} case 274: _LL2832: { struct Cyc_List_List* _temp2838=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Vardecl*(* f)( struct Cyc_Absyn_Decl*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_decl2vardecl, Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2839=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2839[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2840; _temp2840.tag= Cyc_Stmt_tok; _temp2840.f1= Cyc_Absyn_forarray_stmt(
_temp2838, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2840;}); _temp2839;}); break;} case 275:
_LL2837: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2842=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2842[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2843; _temp2843.tag= Cyc_Stmt_tok; _temp2843.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2844=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2844[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2844;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2843;}); _temp2842;}); break; case 276: _LL2841: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2846=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2846[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2847; _temp2847.tag= Cyc_Stmt_tok; _temp2847.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2847;}); _temp2846;}); break; case 277:
_LL2845: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2849=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2849[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2850; _temp2850.tag= Cyc_Stmt_tok; _temp2850.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2850;}); _temp2849;}); break; case 278: _LL2848: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2852=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2852[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2853; _temp2853.tag= Cyc_Stmt_tok; _temp2853.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2853;}); _temp2852;}); break; case 279:
_LL2851: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2855=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2855[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2856; _temp2856.tag= Cyc_Stmt_tok; _temp2856.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2856;}); _temp2855;}); break; case 280: _LL2854: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2858=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2858[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2859; _temp2859.tag= Cyc_Stmt_tok; _temp2859.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2859;}); _temp2858;}); break; case 281:
_LL2857: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2861=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2861[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2862; _temp2862.tag= Cyc_Stmt_tok; _temp2862.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2862;}); _temp2861;}); break; case 282:
_LL2860: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2864=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2864[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2865; _temp2865.tag= Cyc_Stmt_tok; _temp2865.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2865;}); _temp2864;}); break; case 283: _LL2863: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2867=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2867[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2868; _temp2868.tag= Cyc_Pattern_tok; _temp2868.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2868;}); _temp2867;}); break; case 284:
_LL2866: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
break; case 285: _LL2869: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2871=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2871[ 0]=({ struct Cyc_Pattern_tok_struct _temp2872; _temp2872.tag= Cyc_Pattern_tok;
_temp2872.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2873=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2873[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2874; _temp2874.tag= Cyc_Absyn_Int_p;
_temp2874.f1=( void*)(* Cyc_yyget_Int_tok( yyvs[ yyvsp_offset])).f1; _temp2874.f2=(*
Cyc_yyget_Int_tok( yyvs[ yyvsp_offset])).f2; _temp2874;}); _temp2873;}), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2872;});
_temp2871;}); break; case 286: _LL2870: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2876=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2876[ 0]=({ struct Cyc_Pattern_tok_struct _temp2877; _temp2877.tag= Cyc_Pattern_tok;
_temp2877.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2878=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2878[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2879; _temp2879.tag= Cyc_Absyn_Int_p;
_temp2879.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2879.f2= -(* Cyc_yyget_Int_tok(
yyvs[ yyvsp_offset])).f2; _temp2879;}); _temp2878;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2877;}); _temp2876;}); break; case 287:
_LL2875: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2881=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2881[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2882; _temp2882.tag= Cyc_Pattern_tok; _temp2882.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2883=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2883[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2884; _temp2884.tag= Cyc_Absyn_Float_p; _temp2884.f1=
Cyc_yyget_String_tok( yyvs[ yyvsp_offset]); _temp2884;}); _temp2883;}), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2882;});
_temp2881;}); break; case 288: _LL2880: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2886=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2886[ 0]=({ struct Cyc_Pattern_tok_struct _temp2887; _temp2887.tag= Cyc_Pattern_tok;
_temp2887.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2888=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2888[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2889; _temp2889.tag= Cyc_Absyn_Char_p; _temp2889.f1= Cyc_yyget_Char_tok(
yyvs[ yyvsp_offset]); _temp2889;}); _temp2888;}), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2887;});
_temp2886;}); break; case 289: _LL2885: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2891=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2891[ 0]=({ struct Cyc_Pattern_tok_struct _temp2892; _temp2892.tag= Cyc_Pattern_tok;
_temp2892.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2892;});
_temp2891;}); break; case 290: _LL2890: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2894=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2894[ 0]=({ struct Cyc_Pattern_tok_struct _temp2895; _temp2895.tag= Cyc_Pattern_tok;
_temp2895.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2896=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2896[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2897; _temp2897.tag=
Cyc_Absyn_UnknownId_p; _temp2897.f1= Cyc_yyget_QualId_tok( yyvs[ yyvsp_offset]);
_temp2897;}); _temp2896;}), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2895;}); _temp2894;}); break; case 291:
_LL2893: { struct Cyc_List_List* _temp2899=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2900=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2900[ 0]=({ struct Cyc_Pattern_tok_struct _temp2901; _temp2901.tag= Cyc_Pattern_tok;
_temp2901.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2902=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2902[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2903; _temp2903.tag= Cyc_Absyn_UnknownCall_p; _temp2903.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2903.f2=
_temp2899; _temp2903.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2903;});
_temp2902;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2901;}); _temp2900;}); break;} case 292: _LL2898: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2905=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2905[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2906; _temp2906.tag= Cyc_Pattern_tok; _temp2906.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp2907=( struct Cyc_Absyn_Tuple_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp2907[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp2908; _temp2908.tag= Cyc_Absyn_Tuple_p; _temp2908.f1=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2908;}); _temp2907;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2906;}); _temp2905;}); break; case 293:
_LL2904: { struct Cyc_List_List* _temp2910=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2911=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2911[ 0]=({ struct Cyc_Pattern_tok_struct _temp2912; _temp2912.tag= Cyc_Pattern_tok;
_temp2912.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2913=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2913[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2914; _temp2914.tag= Cyc_Absyn_UnknownFields_p; _temp2914.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2914.f2=
_temp2910; _temp2914.f3= 0; _temp2914;}); _temp2913;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2912;}); _temp2911;}); break;} case 294:
_LL2909: { struct Cyc_List_List* _temp2916=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2917=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2917[ 0]=({ struct Cyc_Pattern_tok_struct _temp2918; _temp2918.tag= Cyc_Pattern_tok;
_temp2918.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2919=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2919[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2920; _temp2920.tag= Cyc_Absyn_UnknownFields_p; _temp2920.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2920.f2=
_temp2916; _temp2920.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2920;});
_temp2919;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2918;}); _temp2917;}); break;} case 295: _LL2915: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2922=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2922[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2923; _temp2923.tag= Cyc_Pattern_tok; _temp2923.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp2924=( struct Cyc_Absyn_Pointer_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp2924[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp2925; _temp2925.tag= Cyc_Absyn_Pointer_p;
_temp2925.f1= Cyc_yyget_Pattern_tok( yyvs[ yyvsp_offset]); _temp2925;});
_temp2924;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2923;}); _temp2922;}); break; case 296: _LL2921: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2927=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2927[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2928; _temp2928.tag= Cyc_Pattern_tok; _temp2928.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp2929=( struct Cyc_Absyn_Reference_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp2929[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp2930; _temp2930.tag= Cyc_Absyn_Reference_p;
_temp2930.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp2931=( struct
_tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2931->f1=( void*) Cyc_Absyn_Loc_n;
_temp2931->f2=({ struct _tagged_arr* _temp2932=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2932[ 0]= Cyc_yyget_String_tok( yyvs[
yyvsp_offset]); _temp2932;}); _temp2931;}),( void*) Cyc_Absyn_VoidType, 0);
_temp2930;}); _temp2929;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2928;}); _temp2927;}); break; case 297: _LL2926:
yyval=( void*)({ struct Cyc_PatternList_tok_struct* _temp2934=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2934[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2935; _temp2935.tag= Cyc_PatternList_tok;
_temp2935.f1= 0; _temp2935;}); _temp2934;}); break; case 298: _LL2933: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2937=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2937[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2938; _temp2938.tag= Cyc_PatternList_tok;
_temp2938.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ yyvsp_offset])); _temp2938;}); _temp2937;});
break; case 299: _LL2936: yyval=( void*)({ struct Cyc_PatternList_tok_struct*
_temp2940=( struct Cyc_PatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp2940[ 0]=({ struct Cyc_PatternList_tok_struct _temp2941; _temp2941.tag= Cyc_PatternList_tok;
_temp2941.f1=({ struct Cyc_List_List* _temp2942=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2942->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ yyvsp_offset]); _temp2942->tl= 0; _temp2942;}); _temp2941;}); _temp2940;});
break; case 300: _LL2939: yyval=( void*)({ struct Cyc_PatternList_tok_struct*
_temp2944=( struct Cyc_PatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp2944[ 0]=({ struct Cyc_PatternList_tok_struct _temp2945; _temp2945.tag= Cyc_PatternList_tok;
_temp2945.f1=({ struct Cyc_List_List* _temp2946=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2946->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ yyvsp_offset]); _temp2946->tl= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2946;});
_temp2945;}); _temp2944;}); break; case 301: _LL2943: yyval=( void*)({ struct
Cyc_FieldPattern_tok_struct* _temp2948=( struct Cyc_FieldPattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp2948[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp2949; _temp2949.tag= Cyc_FieldPattern_tok;
_temp2949.f1=({ struct _tuple13* _temp2950=( struct _tuple13*) _cycalloc(
sizeof( struct _tuple13)); _temp2950->f1= 0; _temp2950->f2= Cyc_yyget_Pattern_tok(
yyvs[ yyvsp_offset]); _temp2950;}); _temp2949;}); _temp2948;}); break; case 302:
_LL2947: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2952=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2952[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2953; _temp2953.tag=
Cyc_FieldPattern_tok; _temp2953.f1=({ struct _tuple13* _temp2954=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2954->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2954->f2=
Cyc_yyget_Pattern_tok( yyvs[ yyvsp_offset]); _temp2954;}); _temp2953;});
_temp2952;}); break; case 303: _LL2951: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2956=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2956[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2957; _temp2957.tag= Cyc_FieldPatternList_tok; _temp2957.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(
yyvs[ yyvsp_offset])); _temp2957;}); _temp2956;}); break; case 304: _LL2955:
yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2959=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2959[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2960; _temp2960.tag= Cyc_FieldPatternList_tok;
_temp2960.f1=({ struct Cyc_List_List* _temp2961=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2961->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ yyvsp_offset]); _temp2961->tl= 0; _temp2961;}); _temp2960;}); _temp2959;});
break; case 305: _LL2958: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2963=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2963[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2964; _temp2964.tag= Cyc_FieldPatternList_tok; _temp2964.f1=({ struct Cyc_List_List*
_temp2965=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2965->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[ yyvsp_offset]);
_temp2965->tl= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2965;});
_temp2964;}); _temp2963;}); break; case 306: _LL2962: yyval= yyvs[ yyvsp_offset];
break; case 307: _LL2966: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2968=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2968[ 0]=({ struct Cyc_Exp_tok_struct _temp2969; _temp2969.tag= Cyc_Exp_tok;
_temp2969.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2969;}); _temp2968;}); break; case 308: _LL2967:
yyval= yyvs[ yyvsp_offset]; break; case 309: _LL2970: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2972=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2972[ 0]=({ struct Cyc_Exp_tok_struct
_temp2973; _temp2973.tag= Cyc_Exp_tok; _temp2973.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2973;}); _temp2972;}); break; case 310: _LL2971:
yyval=( void*)({ struct Cyc_Primopopt_tok_struct* _temp2975=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2975[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2976; _temp2976.tag= Cyc_Primopopt_tok; _temp2976.f1=
0; _temp2976;}); _temp2975;}); break; case 311: _LL2974: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2978=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2978[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2979; _temp2979.tag= Cyc_Primopopt_tok; _temp2979.f1=({
struct Cyc_Core_Opt* _temp2980=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2980->v=( void*)(( void*) Cyc_Absyn_Times); _temp2980;});
_temp2979;}); _temp2978;}); break; case 312: _LL2977: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2982=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2982[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2983; _temp2983.tag= Cyc_Primopopt_tok; _temp2983.f1=({
struct Cyc_Core_Opt* _temp2984=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2984->v=( void*)(( void*) Cyc_Absyn_Div); _temp2984;});
_temp2983;}); _temp2982;}); break; case 313: _LL2981: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2986=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2986[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2987; _temp2987.tag= Cyc_Primopopt_tok; _temp2987.f1=({
struct Cyc_Core_Opt* _temp2988=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2988->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2988;});
_temp2987;}); _temp2986;}); break; case 314: _LL2985: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2990=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2990[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2991; _temp2991.tag= Cyc_Primopopt_tok; _temp2991.f1=({
struct Cyc_Core_Opt* _temp2992=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2992->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2992;});
_temp2991;}); _temp2990;}); break; case 315: _LL2989: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2994=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2994[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2995; _temp2995.tag= Cyc_Primopopt_tok; _temp2995.f1=({
struct Cyc_Core_Opt* _temp2996=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2996->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2996;});
_temp2995;}); _temp2994;}); break; case 316: _LL2993: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2998=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2998[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2999; _temp2999.tag= Cyc_Primopopt_tok; _temp2999.f1=({
struct Cyc_Core_Opt* _temp3000=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3000->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3000;});
_temp2999;}); _temp2998;}); break; case 317: _LL2997: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3002=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3002[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3003; _temp3003.tag= Cyc_Primopopt_tok; _temp3003.f1=({
struct Cyc_Core_Opt* _temp3004=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3004->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3004;});
_temp3003;}); _temp3002;}); break; case 318: _LL3001: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3006=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3006[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3007; _temp3007.tag= Cyc_Primopopt_tok; _temp3007.f1=({
struct Cyc_Core_Opt* _temp3008=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3008->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3008;});
_temp3007;}); _temp3006;}); break; case 319: _LL3005: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3010=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3010[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3011; _temp3011.tag= Cyc_Primopopt_tok; _temp3011.f1=({
struct Cyc_Core_Opt* _temp3012=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3012->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3012;});
_temp3011;}); _temp3010;}); break; case 320: _LL3009: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3014=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3014[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3015; _temp3015.tag= Cyc_Primopopt_tok; _temp3015.f1=({
struct Cyc_Core_Opt* _temp3016=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3016->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3016;});
_temp3015;}); _temp3014;}); break; case 321: _LL3013: yyval= yyvs[ yyvsp_offset];
break; case 322: _LL3017: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3019=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3019[ 0]=({ struct Cyc_Exp_tok_struct _temp3020; _temp3020.tag= Cyc_Exp_tok;
_temp3020.f1= Cyc_Absyn_conditional_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3020;}); _temp3019;}); break; case 323: _LL3018:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3022=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3022[ 0]=({ struct Cyc_Exp_tok_struct
_temp3023; _temp3023.tag= Cyc_Exp_tok; _temp3023.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3023;}); _temp3022;}); break; case 324: _LL3021:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3025=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3025[ 0]=({ struct Cyc_Exp_tok_struct
_temp3026; _temp3026.tag= Cyc_Exp_tok; _temp3026.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3026;}); _temp3025;}); break; case 325: _LL3024: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3028=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3028[ 0]=({ struct Cyc_Exp_tok_struct
_temp3029; _temp3029.tag= Cyc_Exp_tok; _temp3029.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3029;}); _temp3028;}); break; case 326: _LL3027: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3031=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3031[ 0]=({ struct Cyc_Exp_tok_struct
_temp3032; _temp3032.tag= Cyc_Exp_tok; _temp3032.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3032;}); _temp3031;}); break; case 327:
_LL3030: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3034=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3034[ 0]=({ struct Cyc_Exp_tok_struct
_temp3035; _temp3035.tag= Cyc_Exp_tok; _temp3035.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3035;}); _temp3034;}); break; case 328:
_LL3033: yyval= yyvs[ yyvsp_offset]; break; case 329: _LL3036: yyval= yyvs[
yyvsp_offset]; break; case 330: _LL3037: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3039=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3039[ 0]=({ struct Cyc_Exp_tok_struct _temp3040; _temp3040.tag= Cyc_Exp_tok;
_temp3040.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3040;}); _temp3039;}); break; case 331: _LL3038:
yyval= yyvs[ yyvsp_offset]; break; case 332: _LL3041: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3043=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3043[ 0]=({ struct Cyc_Exp_tok_struct
_temp3044; _temp3044.tag= Cyc_Exp_tok; _temp3044.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3044;}); _temp3043;}); break; case 333: _LL3042:
yyval= yyvs[ yyvsp_offset]; break; case 334: _LL3045: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3047=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3047[ 0]=({ struct Cyc_Exp_tok_struct
_temp3048; _temp3048.tag= Cyc_Exp_tok; _temp3048.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3048;}); _temp3047;}); break; case 335:
_LL3046: yyval= yyvs[ yyvsp_offset]; break; case 336: _LL3049: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3051=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3051[ 0]=({ struct Cyc_Exp_tok_struct
_temp3052; _temp3052.tag= Cyc_Exp_tok; _temp3052.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3052;}); _temp3051;}); break; case 337:
_LL3050: yyval= yyvs[ yyvsp_offset]; break; case 338: _LL3053: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3055=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3055[ 0]=({ struct Cyc_Exp_tok_struct
_temp3056; _temp3056.tag= Cyc_Exp_tok; _temp3056.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3056;}); _temp3055;}); break; case 339:
_LL3054: yyval= yyvs[ yyvsp_offset]; break; case 340: _LL3057: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3059=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3059[ 0]=({ struct Cyc_Exp_tok_struct
_temp3060; _temp3060.tag= Cyc_Exp_tok; _temp3060.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3060;}); _temp3059;}); break; case 341: _LL3058:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3062=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3062[ 0]=({ struct Cyc_Exp_tok_struct
_temp3063; _temp3063.tag= Cyc_Exp_tok; _temp3063.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3063;}); _temp3062;}); break; case 342: _LL3061:
yyval= yyvs[ yyvsp_offset]; break; case 343: _LL3064: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3066=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3066[ 0]=({ struct Cyc_Exp_tok_struct
_temp3067; _temp3067.tag= Cyc_Exp_tok; _temp3067.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3067;}); _temp3066;}); break; case 344: _LL3065:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3069=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3069[ 0]=({ struct Cyc_Exp_tok_struct
_temp3070; _temp3070.tag= Cyc_Exp_tok; _temp3070.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3070;}); _temp3069;}); break; case 345: _LL3068:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3072=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3072[ 0]=({ struct Cyc_Exp_tok_struct
_temp3073; _temp3073.tag= Cyc_Exp_tok; _temp3073.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3073;}); _temp3072;}); break; case 346: _LL3071:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3075=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3075[ 0]=({ struct Cyc_Exp_tok_struct
_temp3076; _temp3076.tag= Cyc_Exp_tok; _temp3076.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3076;}); _temp3075;}); break; case 347: _LL3074:
yyval= yyvs[ yyvsp_offset]; break; case 348: _LL3077: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3079=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3079[ 0]=({ struct Cyc_Exp_tok_struct
_temp3080; _temp3080.tag= Cyc_Exp_tok; _temp3080.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3080;}); _temp3079;}); break; case 349:
_LL3078: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3082=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3082[ 0]=({ struct Cyc_Exp_tok_struct
_temp3083; _temp3083.tag= Cyc_Exp_tok; _temp3083.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3083;}); _temp3082;}); break; case 350:
_LL3081: yyval= yyvs[ yyvsp_offset]; break; case 351: _LL3084: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3086=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3086[ 0]=({ struct Cyc_Exp_tok_struct
_temp3087; _temp3087.tag= Cyc_Exp_tok; _temp3087.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3087;}); _temp3086;}); break; case 352:
_LL3085: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3089=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3089[ 0]=({ struct Cyc_Exp_tok_struct
_temp3090; _temp3090.tag= Cyc_Exp_tok; _temp3090.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3090;}); _temp3089;}); break; case 353:
_LL3088: yyval= yyvs[ yyvsp_offset]; break; case 354: _LL3091: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3093=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3093[ 0]=({ struct Cyc_Exp_tok_struct
_temp3094; _temp3094.tag= Cyc_Exp_tok; _temp3094.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3094;}); _temp3093;}); break; case 355:
_LL3092: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3096=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3096[ 0]=({ struct Cyc_Exp_tok_struct
_temp3097; _temp3097.tag= Cyc_Exp_tok; _temp3097.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3097;}); _temp3096;}); break; case 356:
_LL3095: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3099=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3099[ 0]=({ struct Cyc_Exp_tok_struct
_temp3100; _temp3100.tag= Cyc_Exp_tok; _temp3100.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3100;}); _temp3099;}); break; case 357:
_LL3098: yyval= yyvs[ yyvsp_offset]; break; case 358: _LL3101: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3103=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3103[ 0]=({ struct Cyc_Exp_tok_struct
_temp3104; _temp3104.tag= Cyc_Exp_tok; _temp3104.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3104;}); _temp3103;}); break; case 359: _LL3102:
yyval= yyvs[ yyvsp_offset]; break; case 360: _LL3105: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3107=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3107[ 0]=({ struct Cyc_Exp_tok_struct
_temp3108; _temp3108.tag= Cyc_Exp_tok; _temp3108.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3108;}); _temp3107;}); break; case 361: _LL3106:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3110=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3110[ 0]=({ struct Cyc_Exp_tok_struct
_temp3111; _temp3111.tag= Cyc_Exp_tok; _temp3111.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3111;}); _temp3110;}); break; case 362: _LL3109:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3113=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3113[ 0]=({ struct Cyc_Exp_tok_struct
_temp3114; _temp3114.tag= Cyc_Exp_tok; _temp3114.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3114;}); _temp3113;}); break; case 363: _LL3112:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3116=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3116[ 0]=({ struct Cyc_Exp_tok_struct
_temp3117; _temp3117.tag= Cyc_Exp_tok; _temp3117.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3117;}); _temp3116;}); break; case 364: _LL3115:
yyval= yyvs[ yyvsp_offset]; break; case 365: _LL3118: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3120=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3120[ 0]=({ struct Cyc_Exp_tok_struct
_temp3121; _temp3121.tag= Cyc_Exp_tok; _temp3121.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3121;}); _temp3120;}); break; case 366: _LL3119:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3123=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3123[ 0]=({ struct Cyc_Exp_tok_struct
_temp3124; _temp3124.tag= Cyc_Exp_tok; _temp3124.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3124;}); _temp3123;}); break; case 367: _LL3122:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3126=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3126[ 0]=({ struct Cyc_Exp_tok_struct
_temp3127; _temp3127.tag= Cyc_Exp_tok; _temp3127.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3127;}); _temp3126;}); break; case 368: _LL3125:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3129=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3129[ 0]=({ struct Cyc_Exp_tok_struct
_temp3130; _temp3130.tag= Cyc_Exp_tok; _temp3130.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_StructField_struct*
_temp3131=( struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp3131[ 0]=({ struct Cyc_Absyn_StructField_struct _temp3132; _temp3132.tag=
Cyc_Absyn_StructField; _temp3132.f1=({ struct _tagged_arr* _temp3133=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp3133[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3133;});
_temp3132;}); _temp3131;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3130;}); _temp3129;}); break; case 369: _LL3128:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3135=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3135[ 0]=({ struct Cyc_Exp_tok_struct
_temp3136; _temp3136.tag= Cyc_Exp_tok; _temp3136.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_TupleIndex_struct*
_temp3137=( struct Cyc_Absyn_TupleIndex_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_TupleIndex_struct)); _temp3137[ 0]=({ struct Cyc_Absyn_TupleIndex_struct
_temp3138; _temp3138.tag= Cyc_Absyn_TupleIndex; _temp3138.f1=( unsigned int)(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)])).f2; _temp3138;}); _temp3137;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3136;}); _temp3135;}); break; case 370: _LL3134: {
struct Cyc_Position_Segment* _temp3140= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
yylsp_offset]).last_line); struct Cyc_List_List* _temp3141=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, _temp3140,
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3142=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3142[ 0]=({ struct Cyc_Exp_tok_struct _temp3143; _temp3143.tag= Cyc_Exp_tok;
_temp3143.f1= Cyc_Absyn_gentyp_exp( _temp3141,(* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3143;}); _temp3142;}); break;} case 371:
_LL3139: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3145=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3145[ 0]=({ struct Cyc_Exp_tok_struct
_temp3146; _temp3146.tag= Cyc_Exp_tok; _temp3146.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3147=( struct Cyc_Absyn_Malloc_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3147[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3148; _temp3148.tag= Cyc_Absyn_Malloc_e;
_temp3148.f1=({ struct Cyc_Absyn_MallocInfo _temp3149; _temp3149.is_calloc= 0;
_temp3149.rgn= 0; _temp3149.elt_type= 0; _temp3149.num_elts= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3149.fat_result=
0; _temp3149;}); _temp3148;}); _temp3147;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3146;}); _temp3145;}); break; case 372: _LL3144:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3151=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3151[ 0]=({ struct Cyc_Exp_tok_struct
_temp3152; _temp3152.tag= Cyc_Exp_tok; _temp3152.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3153=( struct Cyc_Absyn_Malloc_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3153[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3154; _temp3154.tag= Cyc_Absyn_Malloc_e;
_temp3154.f1=({ struct Cyc_Absyn_MallocInfo _temp3155; _temp3155.is_calloc= 0;
_temp3155.rgn=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp3155.elt_type=
0; _temp3155.num_elts= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp3155.fat_result= 0; _temp3155;}); _temp3154;});
_temp3153;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3152;}); _temp3151;}); break; case 373: _LL3150: { void* _temp3159; struct
_tuple2 _temp3157=* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _LL3160: _temp3159=
_temp3157.f3; goto _LL3158; _LL3158: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3161=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3161[ 0]=({ struct Cyc_Exp_tok_struct _temp3162; _temp3162.tag= Cyc_Exp_tok;
_temp3162.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3163=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3163[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3164; _temp3164.tag= Cyc_Absyn_Malloc_e;
_temp3164.f1=({ struct Cyc_Absyn_MallocInfo _temp3165; _temp3165.is_calloc= 1;
_temp3165.rgn= 0; _temp3165.elt_type=({ void** _temp3166=( void**) _cycalloc(
sizeof( void*)); _temp3166[ 0]= _temp3159; _temp3166;}); _temp3165.num_elts= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3165.fat_result=
0; _temp3165;}); _temp3164;}); _temp3163;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3162;}); _temp3161;}); break;} case 374:
_LL3156: { void* _temp3170; struct _tuple2 _temp3168=* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _LL3171:
_temp3170= _temp3168.f3; goto _LL3169; _LL3169: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3172=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3172[ 0]=({ struct Cyc_Exp_tok_struct _temp3173; _temp3173.tag= Cyc_Exp_tok;
_temp3173.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3174=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3174[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3175; _temp3175.tag= Cyc_Absyn_Malloc_e;
_temp3175.f1=({ struct Cyc_Absyn_MallocInfo _temp3176; _temp3176.is_calloc= 1;
_temp3176.rgn=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  8)]); _temp3176.elt_type=({
void** _temp3177=( void**) _cycalloc( sizeof( void*)); _temp3177[ 0]= _temp3170;
_temp3177;}); _temp3176.num_elts= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3176.fat_result=
0; _temp3176;}); _temp3175;}); _temp3174;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  10)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3173;}); _temp3172;}); break;} case 375:
_LL3167: yyval=( void*)({ struct Cyc_Primop_tok_struct* _temp3179=( struct Cyc_Primop_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primop_tok_struct)); _temp3179[ 0]=({ struct Cyc_Primop_tok_struct
_temp3180; _temp3180.tag= Cyc_Primop_tok; _temp3180.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp3180;}); _temp3179;}); break; case 376: _LL3178: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3182=( struct Cyc_Primop_tok_struct*) _cycalloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3182[ 0]=({ struct Cyc_Primop_tok_struct
_temp3183; _temp3183.tag= Cyc_Primop_tok; _temp3183.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp3183;}); _temp3182;}); break; case 377: _LL3181: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3185=( struct Cyc_Primop_tok_struct*) _cycalloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3185[ 0]=({ struct Cyc_Primop_tok_struct
_temp3186; _temp3186.tag= Cyc_Primop_tok; _temp3186.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp3186;}); _temp3185;}); break; case 378: _LL3184: yyval= yyvs[ yyvsp_offset];
break; case 379: _LL3187: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3189=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3189[ 0]=({ struct Cyc_Exp_tok_struct _temp3190; _temp3190.tag= Cyc_Exp_tok;
_temp3190.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3190;}); _temp3189;}); break; case 380:
_LL3188: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3192=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3192[ 0]=({ struct Cyc_Exp_tok_struct
_temp3193; _temp3193.tag= Cyc_Exp_tok; _temp3193.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3193;}); _temp3192;}); break; case 381: _LL3191: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3195=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3195[ 0]=({ struct Cyc_Exp_tok_struct
_temp3196; _temp3196.tag= Cyc_Exp_tok; _temp3196.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3196;}); _temp3195;}); break; case 382: _LL3194:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3198=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3198[ 0]=({ struct Cyc_Exp_tok_struct
_temp3199; _temp3199.tag= Cyc_Exp_tok; _temp3199.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),({ struct _tagged_arr* _temp3200=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp3200[ 0]= Cyc_yyget_String_tok( yyvs[ yyvsp_offset]);
_temp3200;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3199;}); _temp3198;}); break; case 383: _LL3197: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); if( Cyc_Absyn_is_qvar_qualified( q)){ Cyc_Parse_err(
_tag_arr("struct field name is qualified", sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3202=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3202[ 0]=({ struct Cyc_Exp_tok_struct
_temp3203; _temp3203.tag= Cyc_Exp_tok; _temp3203.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3203;}); _temp3202;}); break;} case 384:
_LL3201: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3205=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3205[ 0]=({ struct Cyc_Exp_tok_struct
_temp3206; _temp3206.tag= Cyc_Exp_tok; _temp3206.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),({ struct _tagged_arr* _temp3207=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp3207[ 0]= Cyc_yyget_String_tok( yyvs[ yyvsp_offset]);
_temp3207;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3206;}); _temp3205;}); break; case 385: _LL3204: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); if( Cyc_Absyn_is_qvar_qualified( q)){ Cyc_Parse_err(
_tag_arr("struct field name is qualified", sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3209=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3209[ 0]=({ struct Cyc_Exp_tok_struct
_temp3210; _temp3210.tag= Cyc_Exp_tok; _temp3210.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3210;}); _temp3209;}); break;} case 386:
_LL3208: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3212=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3212[ 0]=({ struct Cyc_Exp_tok_struct
_temp3213; _temp3213.tag= Cyc_Exp_tok; _temp3213.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3213;}); _temp3212;}); break; case 387:
_LL3211: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3215=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3215[ 0]=({ struct Cyc_Exp_tok_struct
_temp3216; _temp3216.tag= Cyc_Exp_tok; _temp3216.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3216;}); _temp3215;}); break; case 388:
_LL3214: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3218=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3218[ 0]=({ struct Cyc_Exp_tok_struct
_temp3219; _temp3219.tag= Cyc_Exp_tok; _temp3219.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3220=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3220[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3221; _temp3221.tag= Cyc_Absyn_CompoundLit_e;
_temp3221.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3221.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3221;});
_temp3220;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3219;}); _temp3218;}); break; case 389: _LL3217: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3223=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3223[ 0]=({ struct Cyc_Exp_tok_struct
_temp3224; _temp3224.tag= Cyc_Exp_tok; _temp3224.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3225=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3225[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3226; _temp3226.tag= Cyc_Absyn_CompoundLit_e;
_temp3226.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3226.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3226;});
_temp3225;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3224;}); _temp3223;}); break; case 390: _LL3222: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3228=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3228[ 0]=({ struct Cyc_Exp_tok_struct
_temp3229; _temp3229.tag= Cyc_Exp_tok; _temp3229.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3230=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3230[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3231; _temp3231.tag= Cyc_Absyn_Fill_e; _temp3231.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3231;});
_temp3230;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3229;}); _temp3228;}); break; case 391: _LL3227: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3233=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3233[ 0]=({ struct Cyc_Exp_tok_struct
_temp3234; _temp3234.tag= Cyc_Exp_tok; _temp3234.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3235=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3235[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3236; _temp3236.tag= Cyc_Absyn_Codegen_e;
_temp3236.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3236;}); _temp3235;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3234;}); _temp3233;}); break; case 392:
_LL3232: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3238=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3238[ 0]=({ struct Cyc_Exp_tok_struct
_temp3239; _temp3239.tag= Cyc_Exp_tok; _temp3239.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3240=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3240[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3241; _temp3241.tag= Cyc_Absyn_UnknownId_e;
_temp3241.f1= Cyc_yyget_QualId_tok( yyvs[ yyvsp_offset]); _temp3241;});
_temp3240;}), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3239;}); _temp3238;}); break; case 393:
_LL3237: yyval= yyvs[ yyvsp_offset]; break; case 394: _LL3242: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3244=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3244[ 0]=({ struct Cyc_Exp_tok_struct
_temp3245; _temp3245.tag= Cyc_Exp_tok; _temp3245.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3245;}); _temp3244;}); break; case 395:
_LL3243: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
break; case 396: _LL3246: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3248=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3248[ 0]=({ struct Cyc_Exp_tok_struct _temp3249; _temp3249.tag= Cyc_Exp_tok;
_temp3249.f1= Cyc_Absyn_noinstantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_UnknownId_e_struct* _temp3250=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3250[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3251; _temp3251.tag= Cyc_Absyn_UnknownId_e;
_temp3251.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3251;}); _temp3250;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp3249;});
_temp3248;}); break; case 397: _LL3247: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3253=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3253[ 0]=({ struct Cyc_Exp_tok_struct _temp3254; _temp3254.tag= Cyc_Exp_tok;
_temp3254.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3255=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3255[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3256; _temp3256.tag=
Cyc_Absyn_UnknownId_e; _temp3256.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3256;});
_temp3255;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3254;}); _temp3253;}); break; case 398:
_LL3252: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3258=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3258[ 0]=({ struct Cyc_Exp_tok_struct
_temp3259; _temp3259.tag= Cyc_Exp_tok; _temp3259.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3259;}); _temp3258;}); break; case 399:
_LL3257: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3261=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3261[ 0]=({ struct Cyc_Exp_tok_struct
_temp3262; _temp3262.tag= Cyc_Exp_tok; _temp3262.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3263=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3263[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3264; _temp3264.tag= Cyc_Absyn_Struct_e;
_temp3264.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3264.f2= 0; _temp3264.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3264.f4= 0;
_temp3264;}); _temp3263;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3262;}); _temp3261;}); break; case 400: _LL3260:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3266=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3266[ 0]=({ struct Cyc_Exp_tok_struct
_temp3267; _temp3267.tag= Cyc_Exp_tok; _temp3267.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3267;}); _temp3266;}); break; case 401:
_LL3265: yyval=( void*)({ struct Cyc_ExpList_tok_struct* _temp3269=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3269[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3270; _temp3270.tag= Cyc_ExpList_tok; _temp3270.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
yyvsp_offset])); _temp3270;}); _temp3269;}); break; case 402: _LL3268: yyval=(
void*)({ struct Cyc_ExpList_tok_struct* _temp3272=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3272[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3273; _temp3273.tag= Cyc_ExpList_tok; _temp3273.f1=({ struct Cyc_List_List*
_temp3274=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3274->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp3274->tl= 0;
_temp3274;}); _temp3273;}); _temp3272;}); break; case 403: _LL3271: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3276=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3276[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3277; _temp3277.tag= Cyc_ExpList_tok; _temp3277.f1=({ struct Cyc_List_List*
_temp3278=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3278->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp3278->tl=
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp3278;}); _temp3277;}); _temp3276;}); break; case 404:
_LL3275: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3280=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3280[ 0]=({ struct Cyc_Exp_tok_struct
_temp3281; _temp3281.tag= Cyc_Exp_tok; _temp3281.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ yyvsp_offset])).f1,(* Cyc_yyget_Int_tok( yyvs[ yyvsp_offset])).f2, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp3281;});
_temp3280;}); break; case 405: _LL3279: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3283=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3283[ 0]=({ struct Cyc_Exp_tok_struct _temp3284; _temp3284.tag= Cyc_Exp_tok;
_temp3284.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp3284;});
_temp3283;}); break; case 406: _LL3282: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3286=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3286[ 0]=({ struct Cyc_Exp_tok_struct _temp3287; _temp3287.tag= Cyc_Exp_tok;
_temp3287.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[ yyvsp_offset]),
Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3287;}); _temp3286;}); break; case 407: _LL3285:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3289=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3289[ 0]=({ struct Cyc_Exp_tok_struct
_temp3290; _temp3290.tag= Cyc_Exp_tok; _temp3290.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp3290;});
_temp3289;}); break; case 408: _LL3288: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3292=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3292[ 0]=({ struct Cyc_QualId_tok_struct _temp3293; _temp3293.tag= Cyc_QualId_tok;
_temp3293.f1=({ struct _tuple1* _temp3294=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3294->f1= Cyc_Absyn_rel_ns_null; _temp3294->f2=({ struct
_tagged_arr* _temp3295=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3295[ 0]= Cyc_yyget_String_tok( yyvs[ yyvsp_offset]);
_temp3295;}); _temp3294;}); _temp3293;}); _temp3292;}); break; case 409: _LL3291:
yyval= yyvs[ yyvsp_offset]; break; default: _LL3296: break;} yyvsp_offset -=
yylen; yyssp_offset -= yylen; yylsp_offset -= yylen; yyvs[
_check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= yyval; yylsp_offset
++; if( yylen ==  0){( yyls[ yylsp_offset]).first_line= Cyc_yylloc.first_line;(
yyls[ yylsp_offset]).first_column= Cyc_yylloc.first_column;( yyls[ yylsp_offset]).last_line=(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line;(
yyls[ yylsp_offset]).last_column=( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_column;} else{( yyls[ yylsp_offset]).last_line=( yyls[
_check_known_subscript_notnull( 10000u,( yylsp_offset +  yylen) -  1)]).last_line;(
yyls[ yylsp_offset]).last_column=( yyls[ _check_known_subscript_notnull( 10000u,(
yylsp_offset +  yylen) -  1)]).last_column;} yyn=( int) Cyc_yyr1[ yyn]; yystate=
Cyc_yypgoto[ _check_known_subscript_notnull( 112u, yyn -  125)] +  yyss[
yyssp_offset]; if(( yystate >=  0? yystate <=  4995: 0)? Cyc_yycheck[ yystate]
==  yyss[ yyssp_offset]: 0){ yystate=( int) Cyc_yytable[ yystate];} else{
yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn -  125)];}
goto yynewstate; yyerrlab: if( yyerrstatus ==  0){ ++ Cyc_yynerrs; yyn=( int)
Cyc_yypact[ yystate]; if( yyn >  - 32768? yyn <  4995: 0){ int sze= 0; struct
_tagged_arr msg; int x; int count; count= 0; for( x= yyn <  0? - yyn: 0; x < 
237u /  sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4996u, x +  yyn)] ==  x){( sze += Cyc_Std_strlen(
Cyc_yytname[ x]) +  15, count ++);}} msg=({ unsigned int _temp3298=(
unsigned int)( sze +  15); unsigned char* _temp3299=( unsigned char*)
_cycalloc_atomic( _check_times( sizeof( unsigned char), _temp3298)); struct
_tagged_arr _temp3301= _tag_arr( _temp3299, sizeof( unsigned char),(
unsigned int)( sze +  15));{ unsigned int _temp3300= _temp3298; unsigned int i;
for( i= 0; i <  _temp3300; i ++){ _temp3299[ i]='\000';}}; _temp3301;}); Cyc_Std_strcpy(
msg, _tag_arr("parse error", sizeof( unsigned char), 12u)); if( count <  5){
count= 0; for( x= yyn <  0? - yyn: 0; x <  237u /  sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4996u, x +  yyn)] ==  x){ Cyc_Std_strcat(
msg, count ==  0? _tag_arr(", expecting `", sizeof( unsigned char), 14u):
_tag_arr(" or `", sizeof( unsigned char), 6u)); Cyc_Std_strcat( msg, Cyc_yytname[
x]); Cyc_Std_strcat( msg, _tag_arr("'", sizeof( unsigned char), 2u)); count ++;}}}
Cyc_yyerror(( struct _tagged_arr) msg);} else{ Cyc_yyerror( _tag_arr("parse error",
sizeof( unsigned char), 12u));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 
3){ if( Cyc_yychar ==  0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset ==  0){ return 1;}
yyvsp_offset --; yystate=( int) yyss[ _check_known_subscript_notnull( 10000u, --
yyssp_offset)]; yylsp_offset --; yyerrhandle: yyn=( int) Cyc_yypact[ yystate];
if( yyn ==  - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn <  0? 1: yyn > 
4995)? 1: Cyc_yycheck[ yyn] !=  1){ goto yyerrdefault;} yyn=( int) Cyc_yytable[
yyn]; if( yyn <  0){ if( yyn ==  - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn ==  0){ goto yyerrpop;}} if( yyn ==  830){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3311= v; struct
_tuple15* _temp3329; struct _tuple15 _temp3331; int _temp3332; unsigned char
_temp3334; short _temp3336; struct _tagged_arr _temp3338; struct Cyc_Core_Opt*
_temp3340; struct Cyc_Core_Opt* _temp3342; struct Cyc_Core_Opt _temp3344; struct
_tagged_arr* _temp3345; struct _tuple1* _temp3347; struct _tuple1 _temp3349;
struct _tagged_arr* _temp3350; void* _temp3352; _LL3313: if(*(( void**)
_temp3311) ==  Cyc_Int_tok){ _LL3330: _temp3329=(( struct Cyc_Int_tok_struct*)
_temp3311)->f1; _temp3331=* _temp3329; _LL3333: _temp3332= _temp3331.f2; goto
_LL3314;} else{ goto _LL3315;} _LL3315: if(*(( void**) _temp3311) ==  Cyc_Char_tok){
_LL3335: _temp3334=(( struct Cyc_Char_tok_struct*) _temp3311)->f1; goto _LL3316;}
else{ goto _LL3317;} _LL3317: if(*(( void**) _temp3311) ==  Cyc_Short_tok){
_LL3337: _temp3336=(( struct Cyc_Short_tok_struct*) _temp3311)->f1; goto _LL3318;}
else{ goto _LL3319;} _LL3319: if(*(( void**) _temp3311) ==  Cyc_String_tok){
_LL3339: _temp3338=(( struct Cyc_String_tok_struct*) _temp3311)->f1; goto
_LL3320;} else{ goto _LL3321;} _LL3321: if(*(( void**) _temp3311) ==  Cyc_Stringopt_tok){
_LL3341: _temp3340=(( struct Cyc_Stringopt_tok_struct*) _temp3311)->f1; if(
_temp3340 ==  0){ goto _LL3322;} else{ goto _LL3323;}} else{ goto _LL3323;}
_LL3323: if(*(( void**) _temp3311) ==  Cyc_Stringopt_tok){ _LL3343: _temp3342=((
struct Cyc_Stringopt_tok_struct*) _temp3311)->f1; if( _temp3342 ==  0){ goto
_LL3325;} else{ _temp3344=* _temp3342; _LL3346: _temp3345=( struct _tagged_arr*)
_temp3344.v; goto _LL3324;}} else{ goto _LL3325;} _LL3325: if(*(( void**)
_temp3311) ==  Cyc_QualId_tok){ _LL3348: _temp3347=(( struct Cyc_QualId_tok_struct*)
_temp3311)->f1; _temp3349=* _temp3347; _LL3353: _temp3352= _temp3349.f1; goto
_LL3351; _LL3351: _temp3350= _temp3349.f2; goto _LL3326;} else{ goto _LL3327;}
_LL3327: goto _LL3328; _LL3314:({ struct Cyc_Std_Int_pa_struct _temp3355;
_temp3355.tag= Cyc_Std_Int_pa; _temp3355.f1=( int)(( unsigned int) _temp3332);{
void* _temp3354[ 1u]={& _temp3355}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3354, sizeof( void*), 1u));}}); goto
_LL3312; _LL3316:({ struct Cyc_Std_Int_pa_struct _temp3357; _temp3357.tag= Cyc_Std_Int_pa;
_temp3357.f1=( int)(( unsigned int)(( int) _temp3334));{ void* _temp3356[ 1u]={&
_temp3357}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3356, sizeof( void*), 1u));}}); goto _LL3312;
_LL3318:({ struct Cyc_Std_Int_pa_struct _temp3359; _temp3359.tag= Cyc_Std_Int_pa;
_temp3359.f1=( int)(( unsigned int)(( int) _temp3336));{ void* _temp3358[ 1u]={&
_temp3359}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3358, sizeof( void*), 1u));}}); goto _LL3312;
_LL3320:({ struct Cyc_Std_String_pa_struct _temp3361; _temp3361.tag= Cyc_Std_String_pa;
_temp3361.f1=( struct _tagged_arr) _temp3338;{ void* _temp3360[ 1u]={& _temp3361};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3360, sizeof( void*), 1u));}}); goto _LL3312; _LL3322:({ void*
_temp3362[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3362, sizeof( void*), 0u));}); goto _LL3312;
_LL3324:({ struct Cyc_Std_String_pa_struct _temp3364; _temp3364.tag= Cyc_Std_String_pa;
_temp3364.f1=( struct _tagged_arr)* _temp3345;{ void* _temp3363[ 1u]={&
_temp3364}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3363, sizeof( void*), 1u));}}); goto _LL3312;
_LL3326: { struct Cyc_List_List* _temp3365= 0;{ void* _temp3366= _temp3352;
struct Cyc_List_List* _temp3374; struct Cyc_List_List* _temp3376; _LL3368: if((
unsigned int) _temp3366 >  1u?*(( int*) _temp3366) ==  Cyc_Absyn_Rel_n: 0){
_LL3375: _temp3374=(( struct Cyc_Absyn_Rel_n_struct*) _temp3366)->f1; goto
_LL3369;} else{ goto _LL3370;} _LL3370: if(( unsigned int) _temp3366 >  1u?*((
int*) _temp3366) ==  Cyc_Absyn_Abs_n: 0){ _LL3377: _temp3376=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3366)->f1; goto _LL3371;} else{ goto _LL3372;} _LL3372: if( _temp3366 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3373;} else{ goto _LL3367;} _LL3369: _temp3365=
_temp3374; goto _LL3367; _LL3371: _temp3365= _temp3376; goto _LL3367; _LL3373:
goto _LL3367; _LL3367:;} for( 0; _temp3365 !=  0; _temp3365= _temp3365->tl){({
struct Cyc_Std_String_pa_struct _temp3379; _temp3379.tag= Cyc_Std_String_pa;
_temp3379.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _temp3365->hd);{ void*
_temp3378[ 1u]={& _temp3379}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3378, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3381; _temp3381.tag= Cyc_Std_String_pa;
_temp3381.f1=( struct _tagged_arr)* _temp3350;{ void* _temp3380[ 1u]={&
_temp3381}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3380, sizeof( void*), 1u));}}); goto _LL3312;}
_LL3328:({ void* _temp3382[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3382, sizeof( void*), 0u));}); goto
_LL3312; _LL3312:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3383=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3383->v=(
void*) Cyc_Lexing_from_file( f); _temp3383;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
