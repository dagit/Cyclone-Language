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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
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
29u), _tag_arr( _temp111, sizeof( void*), 1u));}}), x->loc); return;} static
struct Cyc_Absyn_Vardecl* Cyc_Parse_decl2vardecl( struct Cyc_Absyn_Decl* d){
void* _temp113=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp119; _LL115: if(*((
int*) _temp113) ==  Cyc_Absyn_Var_d){ _LL120: _temp119=(( struct Cyc_Absyn_Var_d_struct*)
_temp113)->f1; goto _LL116;} else{ goto _LL117;} _LL117: goto _LL118; _LL116:
return _temp119; _LL118: return(( struct Cyc_Absyn_Vardecl*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("bad declaration in `forarray' statement",
sizeof( unsigned char), 40u), d->loc); _LL114:;} struct _tuple12{ struct Cyc_List_List*
f1; struct Cyc_Position_Segment* f2; } ; static struct _tuple2* Cyc_Parse_get_param_type(
struct _tuple12* env, struct _tagged_arr* x){ struct _tuple12 _temp123; struct
Cyc_Position_Segment* _temp124; struct Cyc_List_List* _temp126; struct _tuple12*
_temp121= env; _temp123=* _temp121; _LL127: _temp126= _temp123.f1; goto _LL125;
_LL125: _temp124= _temp123.f2; goto _LL122; _LL122: if( _temp126 ==  0){ return((
struct _tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp129; _temp129.tag= Cyc_Std_String_pa; _temp129.f1=( struct _tagged_arr)* x;{
void* _temp128[ 1u]={& _temp129}; Cyc_Std_aprintf( _tag_arr("missing type for parameter %s",
sizeof( unsigned char), 30u), _tag_arr( _temp128, sizeof( void*), 1u));}}),
_temp124);}{ void* _temp130=( void*)(( struct Cyc_Absyn_Decl*) _temp126->hd)->r;
struct Cyc_Absyn_Vardecl* _temp136; _LL132: if(*(( int*) _temp130) ==  Cyc_Absyn_Var_d){
_LL137: _temp136=(( struct Cyc_Absyn_Var_d_struct*) _temp130)->f1; goto _LL133;}
else{ goto _LL134;} _LL134: goto _LL135; _LL133: if( Cyc_Absyn_is_qvar_qualified(
_temp136->name)){ Cyc_Parse_err( _tag_arr("namespace not allowed on parameter",
sizeof( unsigned char), 35u), _temp124);} if( Cyc_Std_zstrptrcmp((* _temp136->name).f2,
x) ==  0){ return({ struct _tuple2* _temp138=( struct _tuple2*) _cycalloc(
sizeof( struct _tuple2)); _temp138->f1=({ struct Cyc_Core_Opt* _temp139=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp139->v=( void*)(*
_temp136->name).f2; _temp139;}); _temp138->f2= _temp136->tq; _temp138->f3=( void*)
_temp136->type; _temp138;});} else{ return Cyc_Parse_get_param_type(({ struct
_tuple12* _temp140=( struct _tuple12*) _cycalloc( sizeof( struct _tuple12));
_temp140->f1= _temp126->tl; _temp140->f2= _temp124; _temp140;}), x);} _LL135:
return(( struct _tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration", sizeof(
unsigned char), 25u),(( struct Cyc_Absyn_Decl*) _temp126->hd)->loc); _LL131:;}}
static int Cyc_Parse_is_typeparam( void* tm){ void* _temp141= tm; _LL143: if((
unsigned int) _temp141 >  1u?*(( int*) _temp141) ==  Cyc_Absyn_TypeParams_mod: 0){
goto _LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL144: return 1; _LL146:
return 0; _LL142:;} static void* Cyc_Parse_id2type( struct _tagged_arr s, void*
k){ if( Cyc_Std_zstrcmp( s, _tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){
return( void*) Cyc_Absyn_HeapRgn;} else{ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp147=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp147[ 0]=({ struct Cyc_Absyn_VarType_struct _temp148; _temp148.tag= Cyc_Absyn_VarType;
_temp148.f1=({ struct Cyc_Absyn_Tvar* _temp149=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp149->name=({ struct _tagged_arr*
_temp150=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp150[ 0]= s; _temp150;}); _temp149->identity= 0; _temp149->kind=( void*) k;
_temp149;}); _temp148;}); _temp147;});}} static struct Cyc_Absyn_Tvar* Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar* t){ void* k;{ void* _temp151= Cyc_Absyn_compress_kb((
void*) t->kind); void* _temp159; void* _temp161; _LL153: if(*(( int*) _temp151)
==  Cyc_Absyn_Eq_kb){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp151)->f1; goto _LL154;} else{ goto _LL155;} _LL155: if(*(( int*) _temp151)
==  Cyc_Absyn_Unknown_kb){ goto _LL156;} else{ goto _LL157;} _LL157: if(*(( int*)
_temp151) ==  Cyc_Absyn_Less_kb){ _LL162: _temp161=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp151)->f2; goto _LL158;} else{ goto _LL152;} _LL154: k=( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp163=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp163[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp164; _temp164.tag= Cyc_Absyn_Eq_kb;
_temp164.f1=( void*) _temp159; _temp164;}); _temp163;}); goto _LL152; _LL156: k=(
void*)({ struct Cyc_Absyn_Unknown_kb_struct* _temp165=( struct Cyc_Absyn_Unknown_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct)); _temp165[ 0]=({ struct
Cyc_Absyn_Unknown_kb_struct _temp166; _temp166.tag= Cyc_Absyn_Unknown_kb;
_temp166.f1= 0; _temp166;}); _temp165;}); goto _LL152; _LL158: k=( void*)({
struct Cyc_Absyn_Less_kb_struct* _temp167=( struct Cyc_Absyn_Less_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct)); _temp167[ 0]=({ struct Cyc_Absyn_Less_kb_struct
_temp168; _temp168.tag= Cyc_Absyn_Less_kb; _temp168.f1= 0; _temp168.f2=( void*)
_temp161; _temp168;}); _temp167;}); goto _LL152; _LL152:;} return({ struct Cyc_Absyn_Tvar*
_temp169=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp169->name= t->name; _temp169->identity= 0; _temp169->kind=( void*) k;
_temp169;});} static struct Cyc_Absyn_Tvar* Cyc_Parse_typ2tvar( struct Cyc_Position_Segment*
loc, void* t){ void* _temp170= t; struct Cyc_Absyn_Tvar* _temp176; _LL172: if((
unsigned int) _temp170 >  3u?*(( int*) _temp170) ==  Cyc_Absyn_VarType: 0){
_LL177: _temp176=(( struct Cyc_Absyn_VarType_struct*) _temp170)->f1; goto _LL173;}
else{ goto _LL174;} _LL174: goto _LL175; _LL173: return _temp176; _LL175: return((
struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL171:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp178=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp178[ 0]=({ struct Cyc_Absyn_VarType_struct _temp179; _temp179.tag= Cyc_Absyn_VarType;
_temp179.f1= pr; _temp179;}); _temp178;});} static void Cyc_Parse_set_vartyp_kind(
void* t, void* k){ void* _temp180= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp186; struct Cyc_Absyn_Tvar _temp188; void* _temp189; void** _temp191;
_LL182: if(( unsigned int) _temp180 >  3u?*(( int*) _temp180) ==  Cyc_Absyn_VarType:
0){ _LL187: _temp186=(( struct Cyc_Absyn_VarType_struct*) _temp180)->f1;
_temp188=* _temp186; _LL190: _temp189=( void*) _temp188.kind; _temp191=( void**)&(*((
struct Cyc_Absyn_VarType_struct*) _temp180)->f1).kind; goto _LL183;} else{ goto
_LL184;} _LL184: goto _LL185; _LL183: { void* _temp192= Cyc_Absyn_compress_kb(*
_temp191); void* _temp193= _temp192; _LL195: if(*(( int*) _temp193) ==  Cyc_Absyn_Eq_kb){
goto _LL196;} else{ goto _LL197;} _LL197: if(*(( int*) _temp193) ==  Cyc_Absyn_Less_kb){
goto _LL198;} else{ goto _LL199;} _LL199: if(*(( int*) _temp193) ==  Cyc_Absyn_Unknown_kb){
goto _LL200;} else{ goto _LL194;} _LL196: return; _LL198: return; _LL200:*
_temp191=( void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp201=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp201[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp202; _temp202.tag= Cyc_Absyn_Eq_kb; _temp202.f1=( void*) k; _temp202;});
_temp201;}); return; _LL194:;} _LL185: return; _LL181:;} static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List* tms, struct Cyc_List_List*
tds, struct Cyc_Position_Segment* loc){ if( tds ==  0){ return tms;} if( tms == 
0){ return 0;}{ void* _temp203=( void*) tms->hd; void* _temp209; _LL205: if((
unsigned int) _temp203 >  1u?*(( int*) _temp203) ==  Cyc_Absyn_Function_mod: 0){
_LL210: _temp209=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp203)->f1;
goto _LL206;} else{ goto _LL207;} _LL207: goto _LL208; _LL206: if( tms->tl ==  0?
1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd)?((
struct Cyc_List_List*) _check_null( tms->tl))->tl ==  0: 0)){ void* _temp211=
_temp209; struct Cyc_List_List* _temp217; _LL213: if(*(( int*) _temp211) ==  Cyc_Absyn_WithTypes){
goto _LL214;} else{ goto _LL215;} _LL215: if(*(( int*) _temp211) ==  Cyc_Absyn_NoTypes){
_LL218: _temp217=(( struct Cyc_Absyn_NoTypes_struct*) _temp211)->f1; goto _LL216;}
else{ goto _LL212;} _LL214: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL216:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp217,
tds);{ struct _tuple12* _temp219=({ struct _tuple12* _temp225=( struct _tuple12*)
_cycalloc( sizeof( struct _tuple12)); _temp225->f1= tds; _temp225->f2= loc;
_temp225;}); return({ struct Cyc_List_List* _temp220=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp220->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp221=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp221[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp222; _temp222.tag= Cyc_Absyn_Function_mod;
_temp222.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp223=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp223[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp224; _temp224.tag= Cyc_Absyn_WithTypes;
_temp224.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp219, _temp217); _temp224.f2= 0; _temp224.f3= 0;
_temp224.f4= 0; _temp224.f5= 0; _temp224;}); _temp223;})); _temp222;}); _temp221;}));
_temp220->tl= 0; _temp220;});} _LL212:;} else{ return({ struct Cyc_List_List*
_temp226=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp226->hd=( void*)(( void*) tms->hd); _temp226->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp226;});} _LL208: return({ struct Cyc_List_List*
_temp227=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp227->hd=( void*)(( void*) tms->hd); _temp227->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp227;}); _LL204:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds != 
0){ d=({ struct Cyc_Parse_Declarator* _temp228=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp228->id= d->id; _temp228->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp228;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso !=  0){ tss=(( struct
Cyc_Parse_Declaration_spec*) dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline;
atts=(( struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct
Cyc_Parse_Declaration_spec*) dso->v)->sc !=  0){ void* _temp229=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*) dso->v)->sc))->v;
_LL231: if( _temp229 == ( void*) Cyc_Parse_Extern_sc){ goto _LL232;} else{ goto
_LL233;} _LL233: if( _temp229 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL234;}
else{ goto _LL235;} _LL235: if( _temp229 == ( void*) Cyc_Parse_Static_sc){ goto
_LL236;} else{ goto _LL237;} _LL237: goto _LL238; _LL232: sc=( void*) Cyc_Absyn_Extern;
goto _LL230; _LL234: sc=( void*) Cyc_Absyn_ExternC; goto _LL230; _LL236: sc=(
void*) Cyc_Absyn_Static; goto _LL230; _LL238: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL230; _LL230:;}}{ struct Cyc_Core_Opt*
_temp241; void* _temp243; struct _tuple5 _temp239= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL244: _temp243= _temp239.f1; goto _LL242; _LL242: _temp241=
_temp239.f2; goto _LL240; _LL240: { struct Cyc_List_List* _temp247; struct Cyc_List_List*
_temp249; void* _temp251; struct Cyc_Absyn_Tqual _temp253; struct _tuple6
_temp245= Cyc_Parse_apply_tms( tq, _temp243, atts, d->tms); _LL254: _temp253=
_temp245.f1; goto _LL252; _LL252: _temp251= _temp245.f2; goto _LL250; _LL250:
_temp249= _temp245.f3; goto _LL248; _LL248: _temp247= _temp245.f4; goto _LL246;
_LL246: if( _temp241 !=  0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp249 !=  0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp255= _temp251; struct Cyc_Absyn_FnInfo _temp261; struct Cyc_List_List*
_temp263; struct Cyc_List_List* _temp265; struct Cyc_Absyn_VarargInfo* _temp267;
int _temp269; struct Cyc_List_List* _temp271; void* _temp273; struct Cyc_Core_Opt*
_temp275; struct Cyc_List_List* _temp277; _LL257: if(( unsigned int) _temp255 > 
3u?*(( int*) _temp255) ==  Cyc_Absyn_FnType: 0){ _LL262: _temp261=(( struct Cyc_Absyn_FnType_struct*)
_temp255)->f1; _LL278: _temp277= _temp261.tvars; goto _LL276; _LL276: _temp275=
_temp261.effect; goto _LL274; _LL274: _temp273=( void*) _temp261.ret_typ; goto
_LL272; _LL272: _temp271= _temp261.args; goto _LL270; _LL270: _temp269= _temp261.c_varargs;
goto _LL268; _LL268: _temp267= _temp261.cyc_varargs; goto _LL266; _LL266:
_temp265= _temp261.rgn_po; goto _LL264; _LL264: _temp263= _temp261.attributes;
goto _LL258;} else{ goto _LL259;} _LL259: goto _LL260; _LL258: { struct Cyc_List_List*
_temp279=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp271); return({ struct Cyc_Absyn_Fndecl*
_temp280=( struct Cyc_Absyn_Fndecl*) _cycalloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp280->sc=( void*) sc; _temp280->name= d->id; _temp280->tvs= _temp277;
_temp280->is_inline= is_inline; _temp280->effect= _temp275; _temp280->ret_type=(
void*) _temp273; _temp280->args= _temp279; _temp280->c_varargs= _temp269;
_temp280->cyc_varargs= _temp267; _temp280->rgn_po= _temp265; _temp280->body=
body; _temp280->cached_typ= 0; _temp280->param_vardecls= 0; _temp280->attributes=
Cyc_List_append( _temp263, _temp247); _temp280;});} _LL260: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL256:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 ==  0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp281=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp281->f1= _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"?", sizeof( unsigned char),
2u); _temp281->f2=(* t).f2; _temp281->f3=(* t).f3; _temp281;});} else{ return({
struct _tuple4* _temp282=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp282->f1=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp282->f2=(* t).f2; _temp282->f3=(* t).f3; _temp282;});}} static
unsigned char _temp283[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp283, _temp283, _temp283 +  52u};
static unsigned char _temp284[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp284, _temp284, _temp284 +  63u};
static unsigned char _temp285[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp285, _temp285, _temp285 +  50u};
static unsigned char _temp286[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp286, _temp286, _temp286 +  60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts !=  0; ts= ts->tl){ void* _temp287=( void*) ts->hd;
struct Cyc_Position_Segment* _temp301; void* _temp303; struct Cyc_Position_Segment*
_temp305; struct Cyc_Position_Segment* _temp307; struct Cyc_Position_Segment*
_temp309; struct Cyc_Position_Segment* _temp311; struct Cyc_Absyn_Decl* _temp313;
_LL289: if(*(( int*) _temp287) ==  Cyc_Parse_Type_spec){ _LL304: _temp303=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp287)->f1; goto _LL302; _LL302: _temp301=((
struct Cyc_Parse_Type_spec_struct*) _temp287)->f2; goto _LL290;} else{ goto
_LL291;} _LL291: if(*(( int*) _temp287) ==  Cyc_Parse_Signed_spec){ _LL306:
_temp305=(( struct Cyc_Parse_Signed_spec_struct*) _temp287)->f1; goto _LL292;}
else{ goto _LL293;} _LL293: if(*(( int*) _temp287) ==  Cyc_Parse_Unsigned_spec){
_LL308: _temp307=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp287)->f1; goto
_LL294;} else{ goto _LL295;} _LL295: if(*(( int*) _temp287) ==  Cyc_Parse_Short_spec){
_LL310: _temp309=(( struct Cyc_Parse_Short_spec_struct*) _temp287)->f1; goto
_LL296;} else{ goto _LL297;} _LL297: if(*(( int*) _temp287) ==  Cyc_Parse_Long_spec){
_LL312: _temp311=(( struct Cyc_Parse_Long_spec_struct*) _temp287)->f1; goto
_LL298;} else{ goto _LL299;} _LL299: if(*(( int*) _temp287) ==  Cyc_Parse_Decl_spec){
_LL314: _temp313=(( struct Cyc_Parse_Decl_spec_struct*) _temp287)->f1; goto
_LL300;} else{ goto _LL288;} _LL290: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp301);} last_loc= _temp301; seen_type= 1; t= _temp303; goto _LL288; _LL292:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp305);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp305);} last_loc= _temp305; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL288; _LL294: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp307);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp307);} last_loc= _temp307; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL288; _LL296: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp309);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp309);} last_loc= _temp309; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL288; _LL298: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp311);} if( seen_size){ void* _temp315= sz; _LL317: if( _temp315 == ( void*)
Cyc_Absyn_B4){ goto _LL318;} else{ goto _LL319;} _LL319: goto _LL320; _LL318: sz=(
void*) Cyc_Absyn_B8; goto _LL316; _LL320: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp311); goto _LL316; _LL316:;} last_loc=
_temp311; seen_size= 1; goto _LL288; _LL300: last_loc= _temp313->loc; if(
declopt ==  0? ! seen_type: 0){ seen_type= 1;{ void* _temp321=( void*) _temp313->r;
struct Cyc_Absyn_Structdecl* _temp333; struct Cyc_Absyn_Tuniondecl* _temp335;
struct Cyc_Absyn_Uniondecl* _temp337; struct Cyc_Absyn_Enumdecl* _temp339;
_LL323: if(*(( int*) _temp321) ==  Cyc_Absyn_Struct_d){ _LL334: _temp333=((
struct Cyc_Absyn_Struct_d_struct*) _temp321)->f1; goto _LL324;} else{ goto
_LL325;} _LL325: if(*(( int*) _temp321) ==  Cyc_Absyn_Tunion_d){ _LL336:
_temp335=(( struct Cyc_Absyn_Tunion_d_struct*) _temp321)->f1; goto _LL326;}
else{ goto _LL327;} _LL327: if(*(( int*) _temp321) ==  Cyc_Absyn_Union_d){
_LL338: _temp337=(( struct Cyc_Absyn_Union_d_struct*) _temp321)->f1; goto _LL328;}
else{ goto _LL329;} _LL329: if(*(( int*) _temp321) ==  Cyc_Absyn_Enum_d){ _LL340:
_temp339=(( struct Cyc_Absyn_Enum_d_struct*) _temp321)->f1; goto _LL330;} else{
goto _LL331;} _LL331: goto _LL332; _LL324: { struct Cyc_List_List* _temp341=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_copy_tvar,
_temp333->tvs)); t=( void*)({ struct Cyc_Absyn_StructType_struct* _temp342=(
struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp342[ 0]=({ struct Cyc_Absyn_StructType_struct _temp343; _temp343.tag= Cyc_Absyn_StructType;
_temp343.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp333->name))->v); _temp343.f2= _temp341; _temp343.f3= 0;
_temp343;}); _temp342;}); if( _temp333->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp344=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp344->v=( void*) _temp313; _temp344;});} goto _LL322;} _LL326: { struct Cyc_List_List*
_temp345=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_copy_tvar, _temp335->tvs)); t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp346=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp346[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp347; _temp347.tag= Cyc_Absyn_TunionType;
_temp347.f1=({ struct Cyc_Absyn_TunionInfo _temp348; _temp348.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp349=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp349[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp350; _temp350.tag= Cyc_Absyn_KnownTunion;
_temp350.f1=({ struct Cyc_Absyn_Tuniondecl** _temp351=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp351[ 0]= _temp335;
_temp351;}); _temp350;}); _temp349;})); _temp348.targs= _temp345; _temp348.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp348;}); _temp347;}); _temp346;}); if(
_temp335->fields !=  0){ declopt=({ struct Cyc_Core_Opt* _temp352=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp352->v=( void*) _temp313;
_temp352;});} goto _LL322;} _LL328: { struct Cyc_List_List* _temp353=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_tvar2typ,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_copy_tvar,
_temp337->tvs)); t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp354=(
struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp354[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp355; _temp355.tag= Cyc_Absyn_UnionType;
_temp355.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp337->name))->v); _temp355.f2= _temp353; _temp355.f3= 0;
_temp355;}); _temp354;}); if( _temp337->fields !=  0){ declopt=({ struct Cyc_Core_Opt*
_temp356=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp356->v=( void*) _temp313; _temp356;});} goto _LL322;} _LL330: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp357=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp357[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp358; _temp358.tag= Cyc_Absyn_EnumType; _temp358.f1=
_temp339->name; _temp358.f2= 0; _temp358;}); _temp357;}); declopt=({ struct Cyc_Core_Opt*
_temp359=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp359->v=( void*) _temp313; _temp359;}); goto _LL322; _LL332:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp313->loc); goto _LL322; _LL322:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp313->loc);} goto _LL288; _LL288:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type within specifier", sizeof(
unsigned char), 30u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp360=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp360[ 0]=({ struct Cyc_Absyn_IntType_struct _temp361; _temp361.tag= Cyc_Absyn_IntType;
_temp361.f1=( void*) sgn; _temp361.f2=( void*) sz; _temp361;}); _temp360;});}
else{ if( seen_sign){ void* _temp362= t; void* _temp368; _LL364: if((
unsigned int) _temp362 >  3u?*(( int*) _temp362) ==  Cyc_Absyn_IntType: 0){
_LL369: _temp368=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp362)->f2;
goto _LL365;} else{ goto _LL366;} _LL366: goto _LL367; _LL365: t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp370=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp370[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp371; _temp371.tag= Cyc_Absyn_IntType; _temp371.f1=( void*) sgn; _temp371.f2=(
void*) _temp368; _temp371;}); _temp370;}); goto _LL363; _LL367: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL363; _LL363:;} if( seen_size){ void* _temp372= t; void*
_temp380; _LL374: if(( unsigned int) _temp372 >  3u?*(( int*) _temp372) ==  Cyc_Absyn_IntType:
0){ _LL381: _temp380=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp372)->f1;
goto _LL375;} else{ goto _LL376;} _LL376: if(( unsigned int) _temp372 >  3u?*((
int*) _temp372) ==  Cyc_Absyn_DoubleType: 0){ goto _LL377;} else{ goto _LL378;}
_LL378: goto _LL379; _LL375: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp382=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp382[ 0]=({ struct Cyc_Absyn_IntType_struct _temp383; _temp383.tag= Cyc_Absyn_IntType;
_temp383.f1=( void*) _temp380; _temp383.f2=( void*) sz; _temp383;}); _temp382;});
goto _LL373; _LL377: t=( void*)({ struct Cyc_Absyn_DoubleType_struct* _temp384=(
struct Cyc_Absyn_DoubleType_struct*) _cycalloc_atomic( sizeof( struct Cyc_Absyn_DoubleType_struct));
_temp384[ 0]=({ struct Cyc_Absyn_DoubleType_struct _temp385; _temp385.tag= Cyc_Absyn_DoubleType;
_temp385.f1= 1; _temp385;}); _temp384;}); goto _LL373; _LL379: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL373; _LL373:;}} return({ struct _tuple5 _temp386; _temp386.f1=
t; _temp386.f2= declopt; _temp386;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds ==  0){ return 0;}{ struct Cyc_Parse_Declarator* _temp387=(
struct Cyc_Parse_Declarator*) ds->hd; struct _tuple1* _temp388= _temp387->id;
struct Cyc_List_List* _temp391; struct Cyc_List_List* _temp393; void* _temp395;
struct Cyc_Absyn_Tqual _temp397; struct _tuple6 _temp389= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp387->tms); _LL398: _temp397= _temp389.f1; goto _LL396;
_LL396: _temp395= _temp389.f2; goto _LL394; _LL394: _temp393= _temp389.f3; goto
_LL392; _LL392: _temp391= _temp389.f4; goto _LL390; _LL390: return({ struct Cyc_List_List*
_temp399=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp399->hd=( void*)({ struct _tuple7* _temp400=( struct _tuple7*) _cycalloc(
sizeof( struct _tuple7)); _temp400->f1= _temp388; _temp400->f2= _temp397;
_temp400->f3= _temp395; _temp400->f4= _temp393; _temp400->f5= _temp391; _temp400;});
_temp399->tl= Cyc_Parse_apply_tmss( _temp397, Cyc_Tcutil_copy_type( t), ds->tl,
shared_atts); _temp399;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms ==  0){ return({ struct _tuple6 _temp401; _temp401.f1= tq;
_temp401.f2= t; _temp401.f3= 0; _temp401.f4= atts; _temp401;});}{ void* _temp402=(
void*) tms->hd; struct Cyc_Absyn_Exp* _temp416; void* _temp418; struct Cyc_Position_Segment*
_temp420; struct Cyc_List_List* _temp422; struct Cyc_Absyn_Tqual _temp424; void*
_temp426; void* _temp428; struct Cyc_List_List* _temp430; struct Cyc_Position_Segment*
_temp432; _LL404: if( _temp402 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL405;}
else{ goto _LL406;} _LL406: if(( unsigned int) _temp402 >  1u?*(( int*) _temp402)
==  Cyc_Absyn_ConstArray_mod: 0){ _LL417: _temp416=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp402)->f1; goto _LL407;} else{ goto _LL408;} _LL408: if(( unsigned int)
_temp402 >  1u?*(( int*) _temp402) ==  Cyc_Absyn_Function_mod: 0){ _LL419:
_temp418=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp402)->f1; goto
_LL409;} else{ goto _LL410;} _LL410: if(( unsigned int) _temp402 >  1u?*(( int*)
_temp402) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL423: _temp422=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp402)->f1; goto _LL421; _LL421: _temp420=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp402)->f2; goto _LL411;} else{ goto _LL412;} _LL412: if(( unsigned int)
_temp402 >  1u?*(( int*) _temp402) ==  Cyc_Absyn_Pointer_mod: 0){ _LL429:
_temp428=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp402)->f1; goto
_LL427; _LL427: _temp426=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp402)->f2; goto _LL425; _LL425: _temp424=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp402)->f3; goto _LL413;} else{ goto _LL414;} _LL414: if(( unsigned int)
_temp402 >  1u?*(( int*) _temp402) ==  Cyc_Absyn_Attributes_mod: 0){ _LL433:
_temp432=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp402)->f1; goto _LL431;
_LL431: _temp430=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp402)->f2; goto
_LL415;} else{ goto _LL403;} _LL405: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp434=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp434[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp435; _temp435.tag= Cyc_Absyn_ArrayType; _temp435.f1=(
void*) t; _temp435.f2= tq; _temp435.f3= 0; _temp435;}); _temp434;}), atts, tms->tl);
_LL407: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp436=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp436[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp437; _temp437.tag= Cyc_Absyn_ArrayType; _temp437.f1=(
void*) t; _temp437.f2= tq; _temp437.f3=( struct Cyc_Absyn_Exp*) _temp416;
_temp437;}); _temp436;}), atts, tms->tl); _LL409: { void* _temp438= _temp418;
struct Cyc_List_List* _temp444; struct Cyc_Core_Opt* _temp446; struct Cyc_Absyn_VarargInfo*
_temp448; int _temp450; struct Cyc_List_List* _temp452; struct Cyc_Position_Segment*
_temp454; _LL440: if(*(( int*) _temp438) ==  Cyc_Absyn_WithTypes){ _LL453:
_temp452=(( struct Cyc_Absyn_WithTypes_struct*) _temp438)->f1; goto _LL451;
_LL451: _temp450=(( struct Cyc_Absyn_WithTypes_struct*) _temp438)->f2; goto
_LL449; _LL449: _temp448=(( struct Cyc_Absyn_WithTypes_struct*) _temp438)->f3;
goto _LL447; _LL447: _temp446=(( struct Cyc_Absyn_WithTypes_struct*) _temp438)->f4;
goto _LL445; _LL445: _temp444=(( struct Cyc_Absyn_WithTypes_struct*) _temp438)->f5;
goto _LL441;} else{ goto _LL442;} _LL442: if(*(( int*) _temp438) ==  Cyc_Absyn_NoTypes){
_LL455: _temp454=(( struct Cyc_Absyn_NoTypes_struct*) _temp438)->f2; goto _LL443;}
else{ goto _LL439;} _LL441: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as !=  0; as= as->tl){ if( Cyc_Absyn_fntype_att(( void*) as->hd)){
fn_atts=({ struct Cyc_List_List* _temp456=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp456->hd=( void*)(( void*) as->hd); _temp456->tl=
fn_atts; _temp456;});} else{ new_atts=({ struct Cyc_List_List* _temp457=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp457->hd=( void*)((
void*) as->hd); _temp457->tl= new_atts; _temp457;});}}} if( tms->tl !=  0){ void*
_temp458=( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd; struct
Cyc_List_List* _temp464; _LL460: if(( unsigned int) _temp458 >  1u?*(( int*)
_temp458) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL465: _temp464=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp458)->f1; goto _LL461;} else{ goto _LL462;} _LL462: goto _LL463; _LL461:
typvars= _temp464; tms= tms->tl; goto _LL459; _LL463: goto _LL459; _LL459:;} if(((((
! _temp450? _temp448 ==  0: 0)? _temp452 !=  0: 0)? _temp452->tl ==  0: 0)?(*((
struct _tuple2*) _temp452->hd)).f1 ==  0: 0)?(*(( struct _tuple2*) _temp452->hd)).f3
== ( void*) Cyc_Absyn_VoidType: 0){ _temp452= 0;} t= Cyc_Parse_array2ptr( t);((
void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Parse_arg_array2ptr, _temp452); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),
Cyc_Absyn_function_typ( typvars, _temp446, t, _temp452, _temp450, _temp448,
_temp444, fn_atts), new_atts,(( struct Cyc_List_List*) _check_null( tms))->tl);}
_LL443:( int) _throw( Cyc_Parse_abort( _tag_arr("function declaration without parameter types",
sizeof( unsigned char), 45u), _temp454)); _LL439:;} _LL411: if( tms->tl ==  0){
return({ struct _tuple6 _temp466; _temp466.f1= tq; _temp466.f2= t; _temp466.f3=
_temp422; _temp466.f4= atts; _temp466;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp420)); _LL413: { void* _temp467= _temp428;
struct Cyc_Absyn_Exp* _temp475; struct Cyc_Absyn_Exp* _temp477; _LL469: if((
unsigned int) _temp467 >  1u?*(( int*) _temp467) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL476: _temp475=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp467)->f1; goto
_LL470;} else{ goto _LL471;} _LL471: if(( unsigned int) _temp467 >  1u?*(( int*)
_temp467) ==  Cyc_Absyn_Nullable_ps: 0){ _LL478: _temp477=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp467)->f1; goto _LL472;} else{ goto _LL473;} _LL473: if( _temp467 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL474;} else{ goto _LL468;} _LL470: return Cyc_Parse_apply_tms(
_temp424, Cyc_Absyn_atb_typ( t, _temp426, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp479=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp479[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp480; _temp480.tag= Cyc_Absyn_Upper_b;
_temp480.f1= _temp475; _temp480;}); _temp479;})), atts, tms->tl); _LL472: return
Cyc_Parse_apply_tms( _temp424, Cyc_Absyn_starb_typ( t, _temp426, tq,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp481=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp481[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp482; _temp482.tag= Cyc_Absyn_Upper_b; _temp482.f1= _temp477; _temp482;});
_temp481;})), atts, tms->tl); _LL474: return Cyc_Parse_apply_tms( _temp424, Cyc_Absyn_tagged_typ(
t, _temp426, tq), atts, tms->tl); _LL468:;} _LL415: return Cyc_Parse_apply_tms(
tq, t, Cyc_List_append( atts, _temp430), tms->tl); _LL403:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp485; void* _temp487; struct _tuple5 _temp483= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL488: _temp487= _temp483.f1; goto _LL486; _LL486: _temp485=
_temp483.f2; goto _LL484; _LL484: if( _temp485 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp487;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp489=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp489[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp490; _temp490.tag= Cyc_Absyn_Decl_s;
_temp490.f1= d; _temp490.f2= s; _temp490;}); _temp489;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc !=  0){ void* _temp491=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL493: if( _temp491 == (
void*) Cyc_Parse_Typedef_sc){ goto _LL494;} else{ goto _LL495;} _LL495: if(
_temp491 == ( void*) Cyc_Parse_Extern_sc){ goto _LL496;} else{ goto _LL497;}
_LL497: if( _temp491 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL498;} else{ goto
_LL499;} _LL499: if( _temp491 == ( void*) Cyc_Parse_Static_sc){ goto _LL500;}
else{ goto _LL501;} _LL501: if( _temp491 == ( void*) Cyc_Parse_Auto_sc){ goto
_LL502;} else{ goto _LL503;} _LL503: if( _temp491 == ( void*) Cyc_Parse_Register_sc){
goto _LL504;} else{ goto _LL505;} _LL505: if( _temp491 == ( void*) Cyc_Parse_Abstract_sc){
goto _LL506;} else{ goto _LL492;} _LL494: istypedef= 1; goto _LL492; _LL496: s=(
void*) Cyc_Absyn_Extern; goto _LL492; _LL498: s=( void*) Cyc_Absyn_ExternC; goto
_LL492; _LL500: s=( void*) Cyc_Absyn_Static; goto _LL492; _LL502: s=( void*) Cyc_Absyn_Public;
goto _LL492; _LL504: s=( void*) Cyc_Absyn_Public; goto _LL492; _LL506: s=( void*)
Cyc_Absyn_Abstract; goto _LL492; _LL492:;}{ struct Cyc_List_List* _temp509;
struct Cyc_List_List* _temp511; struct _tuple0 _temp507=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL512: _temp511= _temp507.f1;
goto _LL510; _LL510: _temp509= _temp507.f2; goto _LL508; _LL508: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp509; for( 0; es !=  0; es= es->tl){
if(( struct Cyc_Absyn_Exp*) es->hd !=  0){ exps_empty= 0; break;}}}{ struct
_tuple5 _temp513= Cyc_Parse_collapse_type_specifiers( tss, loc); if( _temp511 == 
0){ struct Cyc_Core_Opt* _temp516; void* _temp518; struct _tuple5 _temp514=
_temp513; _LL519: _temp518= _temp514.f1; goto _LL517; _LL517: _temp516= _temp514.f2;
goto _LL515; _LL515: if( _temp516 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
_temp516->v;{ void* _temp520=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp532;
struct Cyc_Absyn_Structdecl* _temp534; struct Cyc_Absyn_Uniondecl* _temp536;
struct Cyc_Absyn_Tuniondecl* _temp538; _LL522: if(*(( int*) _temp520) ==  Cyc_Absyn_Enum_d){
_LL533: _temp532=(( struct Cyc_Absyn_Enum_d_struct*) _temp520)->f1; goto _LL523;}
else{ goto _LL524;} _LL524: if(*(( int*) _temp520) ==  Cyc_Absyn_Struct_d){
_LL535: _temp534=(( struct Cyc_Absyn_Struct_d_struct*) _temp520)->f1; goto
_LL525;} else{ goto _LL526;} _LL526: if(*(( int*) _temp520) ==  Cyc_Absyn_Union_d){
_LL537: _temp536=(( struct Cyc_Absyn_Union_d_struct*) _temp520)->f1; goto _LL527;}
else{ goto _LL528;} _LL528: if(*(( int*) _temp520) ==  Cyc_Absyn_Tunion_d){
_LL539: _temp538=(( struct Cyc_Absyn_Tunion_d_struct*) _temp520)->f1; goto
_LL529;} else{ goto _LL530;} _LL530: goto _LL531; _LL523:( void*)( _temp532->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum",
sizeof( unsigned char), 23u), loc);} goto _LL521; _LL525:( void*)( _temp534->sc=(
void*) s); _temp534->attributes= atts; goto _LL521; _LL527:( void*)( _temp536->sc=(
void*) s); _temp536->attributes= atts; goto _LL521; _LL529:( void*)( _temp538->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL521; _LL531: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL521:;}
return({ struct Cyc_List_List* _temp540=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp540->hd=( void*) d; _temp540->tl= 0;
_temp540;});} else{ void* _temp541= _temp518; struct Cyc_List_List* _temp557;
struct _tuple1* _temp559; struct Cyc_Absyn_TunionInfo _temp561; void* _temp563;
struct Cyc_Absyn_Tuniondecl** _temp565; struct Cyc_Absyn_TunionInfo _temp567;
struct Cyc_List_List* _temp569; void* _temp571; struct Cyc_Absyn_UnknownTunionInfo
_temp573; int _temp575; struct _tuple1* _temp577; struct Cyc_List_List* _temp579;
struct _tuple1* _temp581; struct _tuple1* _temp583; struct Cyc_List_List*
_temp585; _LL543: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_StructType:
0){ _LL560: _temp559=(( struct Cyc_Absyn_StructType_struct*) _temp541)->f1; goto
_LL558; _LL558: _temp557=(( struct Cyc_Absyn_StructType_struct*) _temp541)->f2;
goto _LL544;} else{ goto _LL545;} _LL545: if(( unsigned int) _temp541 >  3u?*((
int*) _temp541) ==  Cyc_Absyn_TunionType: 0){ _LL562: _temp561=(( struct Cyc_Absyn_TunionType_struct*)
_temp541)->f1; _LL564: _temp563=( void*) _temp561.tunion_info; if(*(( int*)
_temp563) ==  Cyc_Absyn_KnownTunion){ _LL566: _temp565=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp563)->f1; goto _LL546;} else{ goto _LL547;}} else{ goto _LL547;} _LL547:
if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_TunionType:
0){ _LL568: _temp567=(( struct Cyc_Absyn_TunionType_struct*) _temp541)->f1;
_LL572: _temp571=( void*) _temp567.tunion_info; if(*(( int*) _temp571) ==  Cyc_Absyn_UnknownTunion){
_LL574: _temp573=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp571)->f1;
_LL578: _temp577= _temp573.name; goto _LL576; _LL576: _temp575= _temp573.is_xtunion;
goto _LL570;} else{ goto _LL549;} _LL570: _temp569= _temp567.targs; goto _LL548;}
else{ goto _LL549;} _LL549: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541)
==  Cyc_Absyn_UnionType: 0){ _LL582: _temp581=(( struct Cyc_Absyn_UnionType_struct*)
_temp541)->f1; goto _LL580; _LL580: _temp579=(( struct Cyc_Absyn_UnionType_struct*)
_temp541)->f2; goto _LL550;} else{ goto _LL551;} _LL551: if(( unsigned int)
_temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_EnumType: 0){ _LL584: _temp583=((
struct Cyc_Absyn_EnumType_struct*) _temp541)->f1; goto _LL552;} else{ goto
_LL553;} _LL553: if(( unsigned int) _temp541 >  3u?*(( int*) _temp541) ==  Cyc_Absyn_AnonEnumType:
0){ _LL586: _temp585=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp541)->f1;
goto _LL554;} else{ goto _LL555;} _LL555: goto _LL556; _LL544: { struct Cyc_List_List*
_temp587=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp557); struct Cyc_Absyn_Structdecl* _temp588=({
struct Cyc_Absyn_Structdecl* _temp592=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp592->sc=( void*) s; _temp592->name=({
struct Cyc_Core_Opt* _temp593=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp593->v=( void*)(( struct _tuple1*) _check_null( _temp559));
_temp593;}); _temp592->tvs= _temp587; _temp592->fields= 0; _temp592->attributes=
0; _temp592;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp589=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp589->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp590=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp590[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp591; _temp591.tag= Cyc_Absyn_Struct_d;
_temp591.f1= _temp588; _temp591;}); _temp590;}), loc); _temp589->tl= 0; _temp589;});}
_LL546: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp594=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp594->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp595=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp595[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp596; _temp596.tag= Cyc_Absyn_Tunion_d;
_temp596.f1=* _temp565; _temp596;}); _temp595;}), loc); _temp594->tl= 0;
_temp594;}); _LL548: { struct Cyc_List_List* _temp597=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp569); struct Cyc_Absyn_Decl* _temp598= Cyc_Absyn_tunion_decl( s, _temp577,
_temp597, 0, _temp575, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp599=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp599->hd=(
void*) _temp598; _temp599->tl= 0; _temp599;});} _LL550: { struct Cyc_List_List*
_temp600=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp579); struct Cyc_Absyn_Uniondecl* _temp601=({
struct Cyc_Absyn_Uniondecl* _temp606=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp606->sc=( void*) s; _temp606->name=({
struct Cyc_Core_Opt* _temp607=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp607->v=( void*)(( struct _tuple1*) _check_null( _temp581));
_temp607;}); _temp606->tvs= _temp600; _temp606->fields= 0; _temp606->attributes=
0; _temp606;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp602=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp602->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp603=( struct Cyc_Absyn_Decl*) _cycalloc(
sizeof( struct Cyc_Absyn_Decl)); _temp603->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp604=( struct Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp604[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp605; _temp605.tag= Cyc_Absyn_Union_d;
_temp605.f1= _temp601; _temp605;}); _temp604;})); _temp603->loc= loc; _temp603;});
_temp602->tl= 0; _temp602;});} _LL552: { struct Cyc_Absyn_Enumdecl* _temp608=({
struct Cyc_Absyn_Enumdecl* _temp613=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp613->sc=( void*) s; _temp613->name=
_temp583; _temp613->fields= 0; _temp613;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp609=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp609->hd=( void*)({ struct Cyc_Absyn_Decl* _temp610=(
struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp610->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp611=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp611[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp612; _temp612.tag= Cyc_Absyn_Enum_d; _temp612.f1= _temp608; _temp612;});
_temp611;})); _temp610->loc= loc; _temp610;}); _temp609->tl= 0; _temp609;});}
_LL554: { struct Cyc_Absyn_Enumdecl* _temp614=({ struct Cyc_Absyn_Enumdecl*
_temp619=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp619->sc=( void*) s; _temp619->name= Cyc_Parse_gensym_enum(); _temp619->fields=({
struct Cyc_Core_Opt* _temp620=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp620->v=( void*) _temp585; _temp620;}); _temp619;}); if(
atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} return({ struct Cyc_List_List* _temp615=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp615->hd=( void*)({
struct Cyc_Absyn_Decl* _temp616=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp616->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp617=( struct Cyc_Absyn_Enum_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp617[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp618; _temp618.tag= Cyc_Absyn_Enum_d;
_temp618.f1= _temp614; _temp618;}); _temp617;})); _temp616->loc= loc; _temp616;});
_temp615->tl= 0; _temp615;});} _LL556: Cyc_Parse_err( _tag_arr("missing declarator",
sizeof( unsigned char), 19u), loc); return 0; _LL542:;}} else{ void* t= _temp513.f1;
struct Cyc_List_List* _temp621= Cyc_Parse_apply_tmss( tq, t, _temp511, atts);
if( istypedef){ if( ! exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp621); if( _temp513.f2 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(
_temp513.f2)->v;{ void* _temp622=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp634; struct Cyc_Absyn_Tuniondecl* _temp636; struct Cyc_Absyn_Uniondecl*
_temp638; struct Cyc_Absyn_Enumdecl* _temp640; _LL624: if(*(( int*) _temp622) == 
Cyc_Absyn_Struct_d){ _LL635: _temp634=(( struct Cyc_Absyn_Struct_d_struct*)
_temp622)->f1; goto _LL625;} else{ goto _LL626;} _LL626: if(*(( int*) _temp622)
==  Cyc_Absyn_Tunion_d){ _LL637: _temp636=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp622)->f1; goto _LL627;} else{ goto _LL628;} _LL628: if(*(( int*) _temp622)
==  Cyc_Absyn_Union_d){ _LL639: _temp638=(( struct Cyc_Absyn_Union_d_struct*)
_temp622)->f1; goto _LL629;} else{ goto _LL630;} _LL630: if(*(( int*) _temp622)
==  Cyc_Absyn_Enum_d){ _LL641: _temp640=(( struct Cyc_Absyn_Enum_d_struct*)
_temp622)->f1; goto _LL631;} else{ goto _LL632;} _LL632: goto _LL633; _LL625:(
void*)( _temp634->sc=( void*) s); _temp634->attributes= atts; atts= 0; goto
_LL623; _LL627:( void*)( _temp636->sc=( void*) s); goto _LL623; _LL629:( void*)(
_temp638->sc=( void*) s); goto _LL623; _LL631:( void*)( _temp640->sc=( void*) s);
goto _LL623; _LL633: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL623; _LL623:;} decls=({ struct Cyc_List_List*
_temp642=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp642->hd=( void*) d; _temp642->tl= decls; _temp642;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp644; _temp644.tag=
Cyc_Std_String_pa; _temp644.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) atts->hd);{ void* _temp643[ 1u]={& _temp644}; Cyc_Std_aprintf( _tag_arr("bad attribute %s in typedef",
sizeof( unsigned char), 28u), _tag_arr( _temp643, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp513.f2 !=  0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp645= _temp621; for( 0; _temp645 !=  0;( _temp645= _temp645->tl,
_temp509= _temp509->tl)){ struct _tuple7 _temp648; struct Cyc_List_List*
_temp649; struct Cyc_List_List* _temp651; void* _temp653; struct Cyc_Absyn_Tqual
_temp655; struct _tuple1* _temp657; struct _tuple7* _temp646=( struct _tuple7*)
_temp645->hd; _temp648=* _temp646; _LL658: _temp657= _temp648.f1; goto _LL656;
_LL656: _temp655= _temp648.f2; goto _LL654; _LL654: _temp653= _temp648.f3; goto
_LL652; _LL652: _temp651= _temp648.f4; goto _LL650; _LL650: _temp649= _temp648.f5;
goto _LL647; _LL647: if( _temp651 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), loc);} if( _temp509 ==  0){(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("unexpected NULL in parse!",
sizeof( unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp659=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp509))->hd; struct Cyc_Absyn_Vardecl*
_temp660= Cyc_Absyn_new_vardecl( _temp657, _temp653, _temp659); _temp660->tq=
_temp655;( void*)( _temp660->sc=( void*) s); _temp660->attributes= _temp649;{
struct Cyc_Absyn_Decl* _temp661=({ struct Cyc_Absyn_Decl* _temp663=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp663->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp664=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp664[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp665; _temp665.tag= Cyc_Absyn_Var_d; _temp665.f1= _temp660; _temp665;});
_temp664;})); _temp663->loc= loc; _temp663;}); decls=({ struct Cyc_List_List*
_temp662=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp662->hd=( void*) _temp661; _temp662->tl= decls; _temp662;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_Std_strlen( s) !=  1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp667; _temp667.tag= Cyc_Std_String_pa;
_temp667.f1=( struct _tagged_arr) s;{ void* _temp666[ 1u]={& _temp667}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp666,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL668: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL669: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL670: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL671: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL672: return(
void*) Cyc_Absyn_EffKind; default: _LL673: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp676; _temp676.tag= Cyc_Std_String_pa;
_temp676.f1=( struct _tagged_arr) s;{ void* _temp675[ 1u]={& _temp676}; Cyc_Std_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp675,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts ==  0){ return
tms;} else{ return({ struct Cyc_List_List* _temp677=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp677->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp678=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp678[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp679; _temp679.tag= Cyc_Absyn_Attributes_mod;
_temp679.f1= loc; _temp679.f2= atts; _temp679;}); _temp678;})); _temp677->tl=
tms; _temp677;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp682;
struct Cyc_List_List* _temp683; struct Cyc_List_List* _temp685; void* _temp687;
struct Cyc_Absyn_Tqual _temp689; struct _tuple1* _temp691; struct _tuple7*
_temp680= t; _temp682=* _temp680; _LL692: _temp691= _temp682.f1; goto _LL690;
_LL690: _temp689= _temp682.f2; goto _LL688; _LL688: _temp687= _temp682.f3; goto
_LL686; _LL686: _temp685= _temp682.f4; goto _LL684; _LL684: _temp683= _temp682.f5;
goto _LL681; _LL681: Cyc_Lex_register_typedef( _temp691); if( _temp683 !=  0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp694;
_temp694.tag= Cyc_Std_String_pa; _temp694.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) _temp683->hd);{ void* _temp693[ 1u]={& _temp694}; Cyc_Std_aprintf(
_tag_arr("bad attribute %s within typedef", sizeof( unsigned char), 32u),
_tag_arr( _temp693, sizeof( void*), 1u));}}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp695=( struct Cyc_Absyn_Typedef_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp695[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp696; _temp696.tag= Cyc_Absyn_Typedef_d; _temp696.f1=({
struct Cyc_Absyn_Typedefdecl* _temp697=( struct Cyc_Absyn_Typedefdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp697->name= _temp691;
_temp697->tvs= _temp685; _temp697->defn=( void*) _temp687; _temp697;}); _temp696;});
_temp695;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
unsigned char _temp699[ 15u]="$(sign_t,int)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp699, _temp699, _temp699 +  15u}};
struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ struct _tuple15* yyzzz;{ void*
_temp700= yy1; struct _tuple15* _temp706; _LL702: if(*(( void**) _temp700) == 
Cyc_Int_tok){ _LL707: _temp706=(( struct Cyc_Int_tok_struct*) _temp700)->f1;
goto _LL703;} else{ goto _LL704;} _LL704: goto _LL705; _LL703: yyzzz= _temp706;
goto _LL701; _LL705:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL701:;}
return yyzzz;} static unsigned char _temp709[ 9u]="string_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp709, _temp709, _temp709 +  9u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ struct _tagged_arr yyzzz;{
void* _temp710= yy1; struct _tagged_arr _temp716; _LL712: if(*(( void**)
_temp710) ==  Cyc_String_tok){ _LL717: _temp716=(( struct Cyc_String_tok_struct*)
_temp710)->f1; goto _LL713;} else{ goto _LL714;} _LL714: goto _LL715; _LL713:
yyzzz= _temp716; goto _LL711; _LL715:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL711:;} return yyzzz;} static unsigned char _temp719[ 5u]="char"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{ _temp719,
_temp719, _temp719 +  5u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
unsigned char yyzzz;{ void* _temp720= yy1; unsigned char _temp726; _LL722: if(*((
void**) _temp720) ==  Cyc_Char_tok){ _LL727: _temp726=(( struct Cyc_Char_tok_struct*)
_temp720)->f1; goto _LL723;} else{ goto _LL724;} _LL724: goto _LL725; _LL723:
yyzzz= _temp726; goto _LL721; _LL725:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL721:;} return yyzzz;} static unsigned char _temp729[ 20u]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{
_temp729, _temp729, _temp729 +  20u}}; void* Cyc_yyget_Pointer_Sort_tok( void*
yy1){ void* yyzzz;{ void* _temp730= yy1; void* _temp736; _LL732: if(*(( void**)
_temp730) ==  Cyc_Pointer_Sort_tok){ _LL737: _temp736=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp730)->f1; goto _LL733;} else{ goto _LL734;} _LL734: goto _LL735; _LL733:
yyzzz= _temp736; goto _LL731; _LL735:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL731:;} return yyzzz;} static unsigned char _temp739[ 6u]="exp_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp739,
_temp739, _temp739 +  6u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp740= yy1; struct Cyc_Absyn_Exp*
_temp746; _LL742: if(*(( void**) _temp740) ==  Cyc_Exp_tok){ _LL747: _temp746=((
struct Cyc_Exp_tok_struct*) _temp740)->f1; goto _LL743;} else{ goto _LL744;}
_LL744: goto _LL745; _LL743: yyzzz= _temp746; goto _LL741; _LL745:( int) _throw((
void*)& Cyc_yyfail_Exp_tok); _LL741:;} return yyzzz;} static unsigned char
_temp749[ 14u]="list_t<exp_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{ _temp749, _temp749, _temp749 +  14u}}; struct Cyc_List_List*
Cyc_yyget_ExpList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp750=
yy1; struct Cyc_List_List* _temp756; _LL752: if(*(( void**) _temp750) ==  Cyc_ExpList_tok){
_LL757: _temp756=(( struct Cyc_ExpList_tok_struct*) _temp750)->f1; goto _LL753;}
else{ goto _LL754;} _LL754: goto _LL755; _LL753: yyzzz= _temp756; goto _LL751;
_LL755:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL751:;} return yyzzz;}
static unsigned char _temp759[ 39u]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{
_temp759, _temp759, _temp759 +  39u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp760= yy1; struct Cyc_List_List*
_temp766; _LL762: if(*(( void**) _temp760) ==  Cyc_InitializerList_tok){ _LL767:
_temp766=(( struct Cyc_InitializerList_tok_struct*) _temp760)->f1; goto _LL763;}
else{ goto _LL764;} _LL764: goto _LL765; _LL763: yyzzz= _temp766; goto _LL761;
_LL765:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL761:;} return
yyzzz;} static unsigned char _temp769[ 9u]="primop_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp769, _temp769, _temp769 +  9u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* yyzzz;{ void* _temp770= yy1; void*
_temp776; _LL772: if(*(( void**) _temp770) ==  Cyc_Primop_tok){ _LL777: _temp776=(
void*)(( struct Cyc_Primop_tok_struct*) _temp770)->f1; goto _LL773;} else{ goto
_LL774;} _LL774: goto _LL775; _LL773: yyzzz= _temp776; goto _LL771; _LL775:( int)
_throw(( void*)& Cyc_yyfail_Primop_tok); _LL771:;} return yyzzz;} static
unsigned char _temp779[ 16u]="opt_t<primop_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp779, _temp779, _temp779 +  16u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ struct Cyc_Core_Opt*
yyzzz;{ void* _temp780= yy1; struct Cyc_Core_Opt* _temp786; _LL782: if(*(( void**)
_temp780) ==  Cyc_Primopopt_tok){ _LL787: _temp786=(( struct Cyc_Primopopt_tok_struct*)
_temp780)->f1; goto _LL783;} else{ goto _LL784;} _LL784: goto _LL785; _LL783:
yyzzz= _temp786; goto _LL781; _LL785:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL781:;} return yyzzz;} static unsigned char _temp789[ 7u]="qvar_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{
_temp789, _temp789, _temp789 +  7u}}; struct _tuple1* Cyc_yyget_QualId_tok( void*
yy1){ struct _tuple1* yyzzz;{ void* _temp790= yy1; struct _tuple1* _temp796;
_LL792: if(*(( void**) _temp790) ==  Cyc_QualId_tok){ _LL797: _temp796=(( struct
Cyc_QualId_tok_struct*) _temp790)->f1; goto _LL793;} else{ goto _LL794;} _LL794:
goto _LL795; _LL793: yyzzz= _temp796; goto _LL791; _LL795:( int) _throw(( void*)&
Cyc_yyfail_QualId_tok); _LL791:;} return yyzzz;} static unsigned char _temp799[
7u]="stmt_t"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{
_temp799, _temp799, _temp799 +  7u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok(
void* yy1){ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp800= yy1; struct Cyc_Absyn_Stmt*
_temp806; _LL802: if(*(( void**) _temp800) ==  Cyc_Stmt_tok){ _LL807: _temp806=((
struct Cyc_Stmt_tok_struct*) _temp800)->f1; goto _LL803;} else{ goto _LL804;}
_LL804: goto _LL805; _LL803: yyzzz= _temp806; goto _LL801; _LL805:( int) _throw((
void*)& Cyc_yyfail_Stmt_tok); _LL801:;} return yyzzz;} static unsigned char
_temp809[ 24u]="list_t<switch_clause_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp809, _temp809,
_temp809 +  24u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ struct Cyc_List_List* yyzzz;{ void* _temp810= yy1; struct Cyc_List_List*
_temp816; _LL812: if(*(( void**) _temp810) ==  Cyc_SwitchClauseList_tok){ _LL817:
_temp816=(( struct Cyc_SwitchClauseList_tok_struct*) _temp810)->f1; goto _LL813;}
else{ goto _LL814;} _LL814: goto _LL815; _LL813: yyzzz= _temp816; goto _LL811;
_LL815:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL811:;}
return yyzzz;} static unsigned char _temp819[ 25u]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp819, _temp819, _temp819 +  25u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp820= yy1; struct Cyc_List_List*
_temp826; _LL822: if(*(( void**) _temp820) ==  Cyc_SwitchCClauseList_tok){
_LL827: _temp826=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp820)->f1;
goto _LL823;} else{ goto _LL824;} _LL824: goto _LL825; _LL823: yyzzz= _temp826;
goto _LL821; _LL825:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL821:;} return yyzzz;} static unsigned char _temp829[ 6u]="pat_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{
_temp829, _temp829, _temp829 +  6u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
void* yy1){ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp830= yy1; struct Cyc_Absyn_Pat*
_temp836; _LL832: if(*(( void**) _temp830) ==  Cyc_Pattern_tok){ _LL837:
_temp836=(( struct Cyc_Pattern_tok_struct*) _temp830)->f1; goto _LL833;} else{
goto _LL834;} _LL834: goto _LL835; _LL833: yyzzz= _temp836; goto _LL831; _LL835:(
int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL831:;} return yyzzz;} static
unsigned char _temp839[ 14u]="list_t<pat_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp839, _temp839, _temp839 + 
14u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp840= yy1; struct Cyc_List_List* _temp846; _LL842: if(*(( void**)
_temp840) ==  Cyc_PatternList_tok){ _LL847: _temp846=(( struct Cyc_PatternList_tok_struct*)
_temp840)->f1; goto _LL843;} else{ goto _LL844;} _LL844: goto _LL845; _LL843:
yyzzz= _temp846; goto _LL841; _LL845:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL841:;} return yyzzz;} static unsigned char _temp849[ 31u]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp849, _temp849, _temp849 +  31u}}; struct _tuple13* Cyc_yyget_FieldPattern_tok(
void* yy1){ struct _tuple13* yyzzz;{ void* _temp850= yy1; struct _tuple13*
_temp856; _LL852: if(*(( void**) _temp850) ==  Cyc_FieldPattern_tok){ _LL857:
_temp856=(( struct Cyc_FieldPattern_tok_struct*) _temp850)->f1; goto _LL853;}
else{ goto _LL854;} _LL854: goto _LL855; _LL853: yyzzz= _temp856; goto _LL851;
_LL855:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL851:;} return
yyzzz;} static unsigned char _temp859[ 39u]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp859, _temp859, _temp859 +  39u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp860= yy1; struct Cyc_List_List*
_temp866; _LL862: if(*(( void**) _temp860) ==  Cyc_FieldPatternList_tok){ _LL867:
_temp866=(( struct Cyc_FieldPatternList_tok_struct*) _temp860)->f1; goto _LL863;}
else{ goto _LL864;} _LL864: goto _LL865; _LL863: yyzzz= _temp866; goto _LL861;
_LL865:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL861:;}
return yyzzz;} static unsigned char _temp869[ 9u]="fndecl_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp869, _temp869, _temp869 +  9u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ struct Cyc_Absyn_Fndecl*
yyzzz;{ void* _temp870= yy1; struct Cyc_Absyn_Fndecl* _temp876; _LL872: if(*((
void**) _temp870) ==  Cyc_FnDecl_tok){ _LL877: _temp876=(( struct Cyc_FnDecl_tok_struct*)
_temp870)->f1; goto _LL873;} else{ goto _LL874;} _LL874: goto _LL875; _LL873:
yyzzz= _temp876; goto _LL871; _LL875:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL871:;} return yyzzz;} static unsigned char _temp879[ 15u]="list_t<decl_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp879, _temp879, _temp879 +  15u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp880= yy1; struct Cyc_List_List*
_temp886; _LL882: if(*(( void**) _temp880) ==  Cyc_DeclList_tok){ _LL887:
_temp886=(( struct Cyc_DeclList_tok_struct*) _temp880)->f1; goto _LL883;} else{
goto _LL884;} _LL884: goto _LL885; _LL883: yyzzz= _temp886; goto _LL881; _LL885:(
int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL881:;} return yyzzz;} static
unsigned char _temp889[ 12u]="decl_spec_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp889, _temp889, _temp889 +  12u}};
struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){ struct
Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp890= yy1; struct Cyc_Parse_Declaration_spec*
_temp896; _LL892: if(*(( void**) _temp890) ==  Cyc_DeclSpec_tok){ _LL897:
_temp896=(( struct Cyc_DeclSpec_tok_struct*) _temp890)->f1; goto _LL893;} else{
goto _LL894;} _LL894: goto _LL895; _LL893: yyzzz= _temp896; goto _LL891; _LL895:(
int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL891:;} return yyzzz;} static
unsigned char _temp899[ 27u]="$(declarator_t,exp_opt_t)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp899, _temp899, _temp899 +  27u}};
struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){ struct _tuple14* yyzzz;{
void* _temp900= yy1; struct _tuple14* _temp906; _LL902: if(*(( void**) _temp900)
==  Cyc_InitDecl_tok){ _LL907: _temp906=(( struct Cyc_InitDecl_tok_struct*)
_temp900)->f1; goto _LL903;} else{ goto _LL904;} _LL904: goto _LL905; _LL903:
yyzzz= _temp906; goto _LL901; _LL905:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL901:;} return yyzzz;} static unsigned char _temp909[ 35u]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp909, _temp909, _temp909 +  35u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp910= yy1; struct Cyc_List_List*
_temp916; _LL912: if(*(( void**) _temp910) ==  Cyc_InitDeclList_tok){ _LL917:
_temp916=(( struct Cyc_InitDeclList_tok_struct*) _temp910)->f1; goto _LL913;}
else{ goto _LL914;} _LL914: goto _LL915; _LL913: yyzzz= _temp916; goto _LL911;
_LL915:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL911:;} return
yyzzz;} static unsigned char _temp919[ 16u]="storage_class_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp919, _temp919, _temp919 + 
16u}}; void* Cyc_yyget_StorageClass_tok( void* yy1){ void* yyzzz;{ void*
_temp920= yy1; void* _temp926; _LL922: if(*(( void**) _temp920) ==  Cyc_StorageClass_tok){
_LL927: _temp926=( void*)(( struct Cyc_StorageClass_tok_struct*) _temp920)->f1;
goto _LL923;} else{ goto _LL924;} _LL924: goto _LL925; _LL923: yyzzz= _temp926;
goto _LL921; _LL925:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL921:;}
return yyzzz;} static unsigned char _temp929[ 17u]="type_specifier_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp929, _temp929, _temp929 +  17u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* yyzzz;{ void* _temp930= yy1; void* _temp936; _LL932: if(*(( void**)
_temp930) ==  Cyc_TypeSpecifier_tok){ _LL937: _temp936=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp930)->f1; goto _LL933;} else{ goto _LL934;} _LL934: goto _LL935; _LL933:
yyzzz= _temp936; goto _LL931; _LL935:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL931:;} return yyzzz;} static unsigned char _temp939[ 18u]="struct_or_union_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp939, _temp939, _temp939 +  18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ enum  Cyc_Parse_Struct_or_union yyzzz;{ void* _temp940= yy1; enum 
Cyc_Parse_Struct_or_union _temp946; _LL942: if(*(( void**) _temp940) ==  Cyc_StructOrUnion_tok){
_LL947: _temp946=(( struct Cyc_StructOrUnion_tok_struct*) _temp940)->f1; goto
_LL943;} else{ goto _LL944;} _LL944: goto _LL945; _LL943: yyzzz= _temp946; goto
_LL941; _LL945:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok); _LL941:;}
return yyzzz;} static unsigned char _temp949[ 8u]="tqual_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp949, _temp949, _temp949 +  8u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp950= yy1; struct Cyc_Absyn_Tqual _temp956; _LL952: if(*((
void**) _temp950) ==  Cyc_TypeQual_tok){ _LL957: _temp956=(( struct Cyc_TypeQual_tok_struct*)
_temp950)->f1; goto _LL953;} else{ goto _LL954;} _LL954: goto _LL955; _LL953:
yyzzz= _temp956; goto _LL951; _LL955:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL951:;} return yyzzz;} static unsigned char _temp959[ 22u]="list_t<structfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp959, _temp959, _temp959 +  22u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp960= yy1; struct Cyc_List_List*
_temp966; _LL962: if(*(( void**) _temp960) ==  Cyc_StructFieldDeclList_tok){
_LL967: _temp966=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp960)->f1;
goto _LL963;} else{ goto _LL964;} _LL964: goto _LL965; _LL963: yyzzz= _temp966;
goto _LL961; _LL965:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL961:;} return yyzzz;} static unsigned char _temp969[ 30u]="list_t<list_t<structfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp969, _temp969, _temp969 +  30u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{
void* _temp970= yy1; struct Cyc_List_List* _temp976; _LL972: if(*(( void**)
_temp970) ==  Cyc_StructFieldDeclListList_tok){ _LL977: _temp976=(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp970)->f1; goto _LL973;} else{ goto _LL974;} _LL974: goto _LL975; _LL973:
yyzzz= _temp976; goto _LL971; _LL975:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL971:;} return yyzzz;} static unsigned char _temp979[ 24u]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{
_temp979, _temp979, _temp979 +  24u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp980= yy1; struct Cyc_List_List*
_temp986; _LL982: if(*(( void**) _temp980) ==  Cyc_TypeModifierList_tok){ _LL987:
_temp986=(( struct Cyc_TypeModifierList_tok_struct*) _temp980)->f1; goto _LL983;}
else{ goto _LL984;} _LL984: goto _LL985; _LL983: yyzzz= _temp986; goto _LL981;
_LL985:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL981:;}
return yyzzz;} static unsigned char _temp989[ 13u]="declarator_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp989,
_temp989, _temp989 +  13u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp990= yy1; struct Cyc_Parse_Declarator*
_temp996; _LL992: if(*(( void**) _temp990) ==  Cyc_Declarator_tok){ _LL997:
_temp996=(( struct Cyc_Declarator_tok_struct*) _temp990)->f1; goto _LL993;}
else{ goto _LL994;} _LL994: goto _LL995; _LL993: yyzzz= _temp996; goto _LL991;
_LL995:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL991:;} return
yyzzz;} static unsigned char _temp999[ 21u]="abstractdeclarator_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{
_temp999, _temp999, _temp999 +  21u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ struct Cyc_Parse_Abstractdeclarator* yyzzz;{ void* _temp1000= yy1;
struct Cyc_Parse_Abstractdeclarator* _temp1006; _LL1002: if(*(( void**)
_temp1000) ==  Cyc_AbstractDeclarator_tok){ _LL1007: _temp1006=(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp1000)->f1; goto _LL1003;} else{ goto _LL1004;} _LL1004: goto _LL1005;
_LL1003: yyzzz= _temp1006; goto _LL1001; _LL1005:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1001:;} return yyzzz;} static unsigned char _temp1009[ 5u]="bool"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{
_temp1009, _temp1009, _temp1009 +  5u}}; int Cyc_yyget_Bool_tok( void* yy1){ int
yyzzz;{ void* _temp1010= yy1; int _temp1016; _LL1012: if(*(( void**) _temp1010)
==  Cyc_Bool_tok){ _LL1017: _temp1016=(( struct Cyc_Bool_tok_struct*) _temp1010)->f1;
goto _LL1013;} else{ goto _LL1014;} _LL1014: goto _LL1015; _LL1013: yyzzz=
_temp1016; goto _LL1011; _LL1015:( int) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1011:;} return yyzzz;} static unsigned char _temp1019[ 8u]="scope_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{
_temp1019, _temp1019, _temp1019 +  8u}}; void* Cyc_yyget_Scope_tok( void* yy1){
void* yyzzz;{ void* _temp1020= yy1; void* _temp1026; _LL1022: if(*(( void**)
_temp1020) ==  Cyc_Scope_tok){ _LL1027: _temp1026=( void*)(( struct Cyc_Scope_tok_struct*)
_temp1020)->f1; goto _LL1023;} else{ goto _LL1024;} _LL1024: goto _LL1025;
_LL1023: yyzzz= _temp1026; goto _LL1021; _LL1025:( int) _throw(( void*)& Cyc_yyfail_Scope_tok);
_LL1021:;} return yyzzz;} static unsigned char _temp1029[ 14u]="tunionfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1029, _temp1029, _temp1029 +  14u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp1030= yy1; struct
Cyc_Absyn_Tunionfield* _temp1036; _LL1032: if(*(( void**) _temp1030) ==  Cyc_TunionField_tok){
_LL1037: _temp1036=(( struct Cyc_TunionField_tok_struct*) _temp1030)->f1; goto
_LL1033;} else{ goto _LL1034;} _LL1034: goto _LL1035; _LL1033: yyzzz= _temp1036;
goto _LL1031; _LL1035:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL1031:;} return yyzzz;} static unsigned char _temp1039[ 22u]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp1039, _temp1039, _temp1039 +  22u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1040= yy1; struct Cyc_List_List*
_temp1046; _LL1042: if(*(( void**) _temp1040) ==  Cyc_TunionFieldList_tok){
_LL1047: _temp1046=(( struct Cyc_TunionFieldList_tok_struct*) _temp1040)->f1;
goto _LL1043;} else{ goto _LL1044;} _LL1044: goto _LL1045; _LL1043: yyzzz=
_temp1046; goto _LL1041; _LL1045:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1041:;} return yyzzz;} static unsigned char _temp1049[ 50u]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1049, _temp1049, _temp1049 +  50u}}; struct _tuple17* Cyc_yyget_QualSpecList_tok(
void* yy1){ struct _tuple17* yyzzz;{ void* _temp1050= yy1; struct _tuple17*
_temp1056; _LL1052: if(*(( void**) _temp1050) ==  Cyc_QualSpecList_tok){ _LL1057:
_temp1056=(( struct Cyc_QualSpecList_tok_struct*) _temp1050)->f1; goto _LL1053;}
else{ goto _LL1054;} _LL1054: goto _LL1055; _LL1053: yyzzz= _temp1056; goto
_LL1051; _LL1055:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL1051:;}
return yyzzz;} static unsigned char _temp1059[ 14u]="list_t<var_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{
_temp1059, _temp1059, _temp1059 +  14u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1060= yy1; struct Cyc_List_List*
_temp1066; _LL1062: if(*(( void**) _temp1060) ==  Cyc_IdList_tok){ _LL1067:
_temp1066=(( struct Cyc_IdList_tok_struct*) _temp1060)->f1; goto _LL1063;} else{
goto _LL1064;} _LL1064: goto _LL1065; _LL1063: yyzzz= _temp1066; goto _LL1061;
_LL1065:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1061:;} return yyzzz;}
static unsigned char _temp1069[ 32u]="$(opt_t<var_t>,tqual_t,type_t)@"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{
_temp1069, _temp1069, _temp1069 +  32u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ struct _tuple2* yyzzz;{ void* _temp1070= yy1; struct _tuple2*
_temp1076; _LL1072: if(*(( void**) _temp1070) ==  Cyc_ParamDecl_tok){ _LL1077:
_temp1076=(( struct Cyc_ParamDecl_tok_struct*) _temp1070)->f1; goto _LL1073;}
else{ goto _LL1074;} _LL1074: goto _LL1075; _LL1073: yyzzz= _temp1076; goto
_LL1071; _LL1075:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1071:;}
return yyzzz;} static unsigned char _temp1079[ 40u]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{
_temp1079, _temp1079, _temp1079 +  40u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1080= yy1; struct Cyc_List_List*
_temp1086; _LL1082: if(*(( void**) _temp1080) ==  Cyc_ParamDeclList_tok){
_LL1087: _temp1086=(( struct Cyc_ParamDeclList_tok_struct*) _temp1080)->f1; goto
_LL1083;} else{ goto _LL1084;} _LL1084: goto _LL1085; _LL1083: yyzzz= _temp1086;
goto _LL1081; _LL1085:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1081:;} return yyzzz;} static unsigned char _temp1089[ 107u]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={ Cyc_Core_Failure,{
_temp1089, _temp1089, _temp1089 +  107u}}; struct _tuple16* Cyc_yyget_YY1( void*
yy1){ struct _tuple16* yyzzz;{ void* _temp1090= yy1; struct _tuple16* _temp1096;
_LL1092: if(*(( void**) _temp1090) ==  Cyc_YY1){ _LL1097: _temp1096=(( struct
Cyc_YY1_struct*) _temp1090)->f1; goto _LL1093;} else{ goto _LL1094;} _LL1094:
goto _LL1095; _LL1093: yyzzz= _temp1096; goto _LL1091; _LL1095:( int) _throw((
void*)& Cyc_yyfail_YY1); _LL1091:;} return yyzzz;} static unsigned char
_temp1099[ 15u]="list_t<type_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1099, _temp1099, _temp1099 +  15u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void*
_temp1100= yy1; struct Cyc_List_List* _temp1106; _LL1102: if(*(( void**)
_temp1100) ==  Cyc_TypeList_tok){ _LL1107: _temp1106=(( struct Cyc_TypeList_tok_struct*)
_temp1100)->f1; goto _LL1103;} else{ goto _LL1104;} _LL1104: goto _LL1105;
_LL1103: yyzzz= _temp1106; goto _LL1101; _LL1105:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok);
_LL1101:;} return yyzzz;} static unsigned char _temp1109[ 21u]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{
_temp1109, _temp1109, _temp1109 +  21u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1110= yy1; struct Cyc_List_List*
_temp1116; _LL1112: if(*(( void**) _temp1110) ==  Cyc_DesignatorList_tok){
_LL1117: _temp1116=(( struct Cyc_DesignatorList_tok_struct*) _temp1110)->f1;
goto _LL1113;} else{ goto _LL1114;} _LL1114: goto _LL1115; _LL1113: yyzzz=
_temp1116; goto _LL1111; _LL1115:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1111:;} return yyzzz;} static unsigned char _temp1119[ 13u]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{
_temp1119, _temp1119, _temp1119 +  13u}}; void* Cyc_yyget_Designator_tok( void*
yy1){ void* yyzzz;{ void* _temp1120= yy1; void* _temp1126; _LL1122: if(*(( void**)
_temp1120) ==  Cyc_Designator_tok){ _LL1127: _temp1126=( void*)(( struct Cyc_Designator_tok_struct*)
_temp1120)->f1; goto _LL1123;} else{ goto _LL1124;} _LL1124: goto _LL1125;
_LL1123: yyzzz= _temp1126; goto _LL1121; _LL1125:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL1121:;} return yyzzz;} static unsigned char _temp1129[ 7u]="kind_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{
_temp1129, _temp1129, _temp1129 +  7u}}; void* Cyc_yyget_Kind_tok( void* yy1){
void* yyzzz;{ void* _temp1130= yy1; void* _temp1136; _LL1132: if(*(( void**)
_temp1130) ==  Cyc_Kind_tok){ _LL1137: _temp1136=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1130)->f1; goto _LL1133;} else{ goto _LL1134;} _LL1134: goto _LL1135;
_LL1133: yyzzz= _temp1136; goto _LL1131; _LL1135:( int) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1131:;} return yyzzz;} static unsigned char _temp1139[ 7u]="type_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1139, _temp1139, _temp1139 +  7u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* yyzzz;{ void* _temp1140= yy1; void* _temp1146; _LL1142: if(*(( void**)
_temp1140) ==  Cyc_Type_tok){ _LL1147: _temp1146=( void*)(( struct Cyc_Type_tok_struct*)
_temp1140)->f1; goto _LL1143;} else{ goto _LL1144;} _LL1144: goto _LL1145;
_LL1143: yyzzz= _temp1146; goto _LL1141; _LL1145:( int) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1141:;} return yyzzz;} static unsigned char _temp1149[ 20u]="list_t<attribute_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1149, _temp1149, _temp1149 +  20u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1150= yy1; struct Cyc_List_List*
_temp1156; _LL1152: if(*(( void**) _temp1150) ==  Cyc_AttributeList_tok){
_LL1157: _temp1156=(( struct Cyc_AttributeList_tok_struct*) _temp1150)->f1; goto
_LL1153;} else{ goto _LL1154;} _LL1154: goto _LL1155; _LL1153: yyzzz= _temp1156;
goto _LL1151; _LL1155:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1151:;} return yyzzz;} static unsigned char _temp1159[ 12u]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{
_temp1159, _temp1159, _temp1159 +  12u}}; void* Cyc_yyget_Attribute_tok( void*
yy1){ void* yyzzz;{ void* _temp1160= yy1; void* _temp1166; _LL1162: if(*(( void**)
_temp1160) ==  Cyc_Attribute_tok){ _LL1167: _temp1166=( void*)(( struct Cyc_Attribute_tok_struct*)
_temp1160)->f1; goto _LL1163;} else{ goto _LL1164;} _LL1164: goto _LL1165;
_LL1163: yyzzz= _temp1166; goto _LL1161; _LL1165:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1161:;} return yyzzz;} static unsigned char _temp1169[ 12u]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{
_temp1169, _temp1169, _temp1169 +  12u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
void* yy1){ struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp1170= yy1; struct Cyc_Absyn_Enumfield*
_temp1176; _LL1172: if(*(( void**) _temp1170) ==  Cyc_Enumfield_tok){ _LL1177:
_temp1176=(( struct Cyc_Enumfield_tok_struct*) _temp1170)->f1; goto _LL1173;}
else{ goto _LL1174;} _LL1174: goto _LL1175; _LL1173: yyzzz= _temp1176; goto
_LL1171; _LL1175:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1171:;}
return yyzzz;} static unsigned char _temp1179[ 20u]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1179, _temp1179, _temp1179 +  20u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1180= yy1; struct Cyc_List_List*
_temp1186; _LL1182: if(*(( void**) _temp1180) ==  Cyc_EnumfieldList_tok){
_LL1187: _temp1186=(( struct Cyc_EnumfieldList_tok_struct*) _temp1180)->f1; goto
_LL1183;} else{ goto _LL1184;} _LL1184: goto _LL1185; _LL1183: yyzzz= _temp1186;
goto _LL1181; _LL1185:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1181:;} return yyzzz;} static unsigned char _temp1189[ 14u]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{
_temp1189, _temp1189, _temp1189 +  14u}}; struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok(
void* yy1){ struct Cyc_Core_Opt* yyzzz;{ void* _temp1190= yy1; struct Cyc_Core_Opt*
_temp1196; _LL1192: if(*(( void**) _temp1190) ==  Cyc_TypeOpt_tok){ _LL1197:
_temp1196=(( struct Cyc_TypeOpt_tok_struct*) _temp1190)->f1; goto _LL1193;}
else{ goto _LL1194;} _LL1194: goto _LL1195; _LL1193: yyzzz= _temp1196; goto
_LL1191; _LL1195:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1191:;}
return yyzzz;} static unsigned char _temp1199[ 26u]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{
_temp1199, _temp1199, _temp1199 +  26u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1200= yy1; struct Cyc_List_List*
_temp1206; _LL1202: if(*(( void**) _temp1200) ==  Cyc_Rgnorder_tok){ _LL1207:
_temp1206=(( struct Cyc_Rgnorder_tok_struct*) _temp1200)->f1; goto _LL1203;}
else{ goto _LL1204;} _LL1204: goto _LL1205; _LL1203: yyzzz= _temp1206; goto
_LL1201; _LL1205:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1201:;}
return yyzzz;} struct Cyc_Yyltype{ int timestamp; int first_line; int
first_column; int last_line; int last_column; } ; struct Cyc_Yyltype Cyc_yynewloc(){
return({ struct Cyc_Yyltype _temp1208; _temp1208.timestamp= 0; _temp1208.first_line=
0; _temp1208.first_column= 0; _temp1208.last_line= 0; _temp1208.last_column= 0;
_temp1208;});} struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[
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
static unsigned char _temp1209[ 2u]="$"; static unsigned char _temp1210[ 6u]="error";
static unsigned char _temp1211[ 12u]="$undefined."; static unsigned char
_temp1212[ 5u]="AUTO"; static unsigned char _temp1213[ 9u]="REGISTER"; static
unsigned char _temp1214[ 7u]="STATIC"; static unsigned char _temp1215[ 7u]="EXTERN";
static unsigned char _temp1216[ 8u]="TYPEDEF"; static unsigned char _temp1217[ 5u]="VOID";
static unsigned char _temp1218[ 5u]="CHAR"; static unsigned char _temp1219[ 6u]="SHORT";
static unsigned char _temp1220[ 4u]="INT"; static unsigned char _temp1221[ 5u]="LONG";
static unsigned char _temp1222[ 6u]="FLOAT"; static unsigned char _temp1223[ 7u]="DOUBLE";
static unsigned char _temp1224[ 7u]="SIGNED"; static unsigned char _temp1225[ 9u]="UNSIGNED";
static unsigned char _temp1226[ 6u]="CONST"; static unsigned char _temp1227[ 9u]="VOLATILE";
static unsigned char _temp1228[ 9u]="RESTRICT"; static unsigned char _temp1229[
7u]="STRUCT"; static unsigned char _temp1230[ 6u]="UNION"; static unsigned char
_temp1231[ 5u]="CASE"; static unsigned char _temp1232[ 8u]="DEFAULT"; static
unsigned char _temp1233[ 7u]="INLINE"; static unsigned char _temp1234[ 7u]="SIZEOF";
static unsigned char _temp1235[ 9u]="OFFSETOF"; static unsigned char _temp1236[
3u]="IF"; static unsigned char _temp1237[ 5u]="ELSE"; static unsigned char
_temp1238[ 7u]="SWITCH"; static unsigned char _temp1239[ 6u]="WHILE"; static
unsigned char _temp1240[ 3u]="DO"; static unsigned char _temp1241[ 4u]="FOR";
static unsigned char _temp1242[ 5u]="GOTO"; static unsigned char _temp1243[ 9u]="CONTINUE";
static unsigned char _temp1244[ 6u]="BREAK"; static unsigned char _temp1245[ 7u]="RETURN";
static unsigned char _temp1246[ 5u]="ENUM"; static unsigned char _temp1247[ 8u]="NULL_kw";
static unsigned char _temp1248[ 4u]="LET"; static unsigned char _temp1249[ 6u]="THROW";
static unsigned char _temp1250[ 4u]="TRY"; static unsigned char _temp1251[ 6u]="CATCH";
static unsigned char _temp1252[ 4u]="NEW"; static unsigned char _temp1253[ 9u]="ABSTRACT";
static unsigned char _temp1254[ 9u]="FALLTHRU"; static unsigned char _temp1255[
6u]="USING"; static unsigned char _temp1256[ 10u]="NAMESPACE"; static
unsigned char _temp1257[ 7u]="TUNION"; static unsigned char _temp1258[ 8u]="XTUNION";
static unsigned char _temp1259[ 9u]="FORARRAY"; static unsigned char _temp1260[
5u]="FILL"; static unsigned char _temp1261[ 8u]="CODEGEN"; static unsigned char
_temp1262[ 4u]="CUT"; static unsigned char _temp1263[ 7u]="SPLICE"; static
unsigned char _temp1264[ 7u]="MALLOC"; static unsigned char _temp1265[ 8u]="RMALLOC";
static unsigned char _temp1266[ 7u]="CALLOC"; static unsigned char _temp1267[ 8u]="RCALLOC";
static unsigned char _temp1268[ 9u]="REGION_T"; static unsigned char _temp1269[
9u]="SIZEOF_T"; static unsigned char _temp1270[ 7u]="REGION"; static
unsigned char _temp1271[ 5u]="RNEW"; static unsigned char _temp1272[ 8u]="REGIONS";
static unsigned char _temp1273[ 4u]="GEN"; static unsigned char _temp1274[ 7u]="PTR_OP";
static unsigned char _temp1275[ 7u]="INC_OP"; static unsigned char _temp1276[ 7u]="DEC_OP";
static unsigned char _temp1277[ 8u]="LEFT_OP"; static unsigned char _temp1278[ 9u]="RIGHT_OP";
static unsigned char _temp1279[ 6u]="LE_OP"; static unsigned char _temp1280[ 6u]="GE_OP";
static unsigned char _temp1281[ 6u]="EQ_OP"; static unsigned char _temp1282[ 6u]="NE_OP";
static unsigned char _temp1283[ 7u]="AND_OP"; static unsigned char _temp1284[ 6u]="OR_OP";
static unsigned char _temp1285[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1286[ 11u]="DIV_ASSIGN"; static unsigned char _temp1287[ 11u]="MOD_ASSIGN";
static unsigned char _temp1288[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1289[ 11u]="SUB_ASSIGN"; static unsigned char _temp1290[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1291[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1292[ 11u]="AND_ASSIGN"; static unsigned char _temp1293[ 11u]="XOR_ASSIGN";
static unsigned char _temp1294[ 10u]="OR_ASSIGN"; static unsigned char _temp1295[
9u]="ELLIPSIS"; static unsigned char _temp1296[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1297[ 12u]="COLON_COLON"; static unsigned char _temp1298[ 11u]="IDENTIFIER";
static unsigned char _temp1299[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1300[ 7u]="STRING"; static unsigned char _temp1301[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1302[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1303[ 9u]="TYPE_VAR"; static unsigned char _temp1304[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1305[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1306[ 10u]="ATTRIBUTE"; static unsigned char _temp1307[ 4u]="';'"; static
unsigned char _temp1308[ 4u]="'{'"; static unsigned char _temp1309[ 4u]="'}'";
static unsigned char _temp1310[ 4u]="'='"; static unsigned char _temp1311[ 4u]="'('";
static unsigned char _temp1312[ 4u]="')'"; static unsigned char _temp1313[ 4u]="','";
static unsigned char _temp1314[ 4u]="'_'"; static unsigned char _temp1315[ 4u]="'$'";
static unsigned char _temp1316[ 4u]="'<'"; static unsigned char _temp1317[ 4u]="'>'";
static unsigned char _temp1318[ 4u]="':'"; static unsigned char _temp1319[ 4u]="'.'";
static unsigned char _temp1320[ 4u]="'['"; static unsigned char _temp1321[ 4u]="']'";
static unsigned char _temp1322[ 4u]="'*'"; static unsigned char _temp1323[ 4u]="'@'";
static unsigned char _temp1324[ 4u]="'?'"; static unsigned char _temp1325[ 4u]="'+'";
static unsigned char _temp1326[ 4u]="'-'"; static unsigned char _temp1327[ 4u]="'&'";
static unsigned char _temp1328[ 4u]="'|'"; static unsigned char _temp1329[ 4u]="'^'";
static unsigned char _temp1330[ 4u]="'/'"; static unsigned char _temp1331[ 4u]="'%'";
static unsigned char _temp1332[ 4u]="'~'"; static unsigned char _temp1333[ 4u]="'!'";
static unsigned char _temp1334[ 5u]="prog"; static unsigned char _temp1335[ 17u]="translation_unit";
static unsigned char _temp1336[ 21u]="external_declaration"; static
unsigned char _temp1337[ 20u]="function_definition"; static unsigned char
_temp1338[ 21u]="function_definition2"; static unsigned char _temp1339[ 13u]="using_action";
static unsigned char _temp1340[ 15u]="unusing_action"; static unsigned char
_temp1341[ 17u]="namespace_action"; static unsigned char _temp1342[ 19u]="unnamespace_action";
static unsigned char _temp1343[ 12u]="declaration"; static unsigned char
_temp1344[ 17u]="declaration_list"; static unsigned char _temp1345[ 23u]="declaration_specifiers";
static unsigned char _temp1346[ 24u]="storage_class_specifier"; static
unsigned char _temp1347[ 15u]="attributes_opt"; static unsigned char _temp1348[
11u]="attributes"; static unsigned char _temp1349[ 15u]="attribute_list"; static
unsigned char _temp1350[ 10u]="attribute"; static unsigned char _temp1351[ 15u]="type_specifier";
static unsigned char _temp1352[ 5u]="kind"; static unsigned char _temp1353[ 15u]="type_qualifier";
static unsigned char _temp1354[ 15u]="enum_specifier"; static unsigned char
_temp1355[ 11u]="enum_field"; static unsigned char _temp1356[ 22u]="enum_declaration_list";
static unsigned char _temp1357[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1358[ 16u]="type_params_opt"; static unsigned char _temp1359[
16u]="struct_or_union"; static unsigned char _temp1360[ 24u]="struct_declaration_list";
static unsigned char _temp1361[ 25u]="struct_declaration_list0"; static
unsigned char _temp1362[ 21u]="init_declarator_list"; static unsigned char
_temp1363[ 22u]="init_declarator_list0"; static unsigned char _temp1364[ 16u]="init_declarator";
static unsigned char _temp1365[ 19u]="struct_declaration"; static unsigned char
_temp1366[ 25u]="specifier_qualifier_list"; static unsigned char _temp1367[ 23u]="struct_declarator_list";
static unsigned char _temp1368[ 24u]="struct_declarator_list0"; static
unsigned char _temp1369[ 18u]="struct_declarator"; static unsigned char
_temp1370[ 17u]="tunion_specifier"; static unsigned char _temp1371[ 18u]="tunion_or_xtunion";
static unsigned char _temp1372[ 17u]="tunionfield_list"; static unsigned char
_temp1373[ 18u]="tunionfield_scope"; static unsigned char _temp1374[ 12u]="tunionfield";
static unsigned char _temp1375[ 11u]="declarator"; static unsigned char
_temp1376[ 18u]="direct_declarator"; static unsigned char _temp1377[ 8u]="pointer";
static unsigned char _temp1378[ 13u]="pointer_char"; static unsigned char
_temp1379[ 8u]="rgn_opt"; static unsigned char _temp1380[ 4u]="rgn"; static
unsigned char _temp1381[ 20u]="type_qualifier_list"; static unsigned char
_temp1382[ 20u]="parameter_type_list"; static unsigned char _temp1383[ 9u]="type_var";
static unsigned char _temp1384[ 16u]="optional_effect"; static unsigned char
_temp1385[ 19u]="optional_rgn_order"; static unsigned char _temp1386[ 10u]="rgn_order";
static unsigned char _temp1387[ 16u]="optional_inject"; static unsigned char
_temp1388[ 11u]="effect_set"; static unsigned char _temp1389[ 14u]="atomic_effect";
static unsigned char _temp1390[ 11u]="region_set"; static unsigned char
_temp1391[ 15u]="parameter_list"; static unsigned char _temp1392[ 22u]="parameter_declaration";
static unsigned char _temp1393[ 16u]="identifier_list"; static unsigned char
_temp1394[ 17u]="identifier_list0"; static unsigned char _temp1395[ 12u]="initializer";
static unsigned char _temp1396[ 18u]="array_initializer"; static unsigned char
_temp1397[ 17u]="initializer_list"; static unsigned char _temp1398[ 12u]="designation";
static unsigned char _temp1399[ 16u]="designator_list"; static unsigned char
_temp1400[ 11u]="designator"; static unsigned char _temp1401[ 10u]="type_name";
static unsigned char _temp1402[ 14u]="any_type_name"; static unsigned char
_temp1403[ 15u]="type_name_list"; static unsigned char _temp1404[ 20u]="abstract_declarator";
static unsigned char _temp1405[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1406[ 10u]="statement"; static unsigned char _temp1407[ 18u]="labeled_statement";
static unsigned char _temp1408[ 21u]="expression_statement"; static
unsigned char _temp1409[ 19u]="compound_statement"; static unsigned char
_temp1410[ 16u]="block_item_list"; static unsigned char _temp1411[ 20u]="selection_statement";
static unsigned char _temp1412[ 15u]="switch_clauses"; static unsigned char
_temp1413[ 16u]="switchC_clauses"; static unsigned char _temp1414[ 20u]="iteration_statement";
static unsigned char _temp1415[ 15u]="jump_statement"; static unsigned char
_temp1416[ 8u]="pattern"; static unsigned char _temp1417[ 19u]="tuple_pattern_list";
static unsigned char _temp1418[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1419[ 14u]="field_pattern"; static unsigned char _temp1420[ 19u]="field_pattern_list";
static unsigned char _temp1421[ 20u]="field_pattern_list0"; static unsigned char
_temp1422[ 11u]="expression"; static unsigned char _temp1423[ 22u]="assignment_expression";
static unsigned char _temp1424[ 20u]="assignment_operator"; static unsigned char
_temp1425[ 23u]="conditional_expression"; static unsigned char _temp1426[ 20u]="constant_expression";
static unsigned char _temp1427[ 22u]="logical_or_expression"; static
unsigned char _temp1428[ 23u]="logical_and_expression"; static unsigned char
_temp1429[ 24u]="inclusive_or_expression"; static unsigned char _temp1430[ 24u]="exclusive_or_expression";
static unsigned char _temp1431[ 15u]="and_expression"; static unsigned char
_temp1432[ 20u]="equality_expression"; static unsigned char _temp1433[ 22u]="relational_expression";
static unsigned char _temp1434[ 17u]="shift_expression"; static unsigned char
_temp1435[ 20u]="additive_expression"; static unsigned char _temp1436[ 26u]="multiplicative_expression";
static unsigned char _temp1437[ 16u]="cast_expression"; static unsigned char
_temp1438[ 17u]="unary_expression"; static unsigned char _temp1439[ 15u]="unary_operator";
static unsigned char _temp1440[ 19u]="postfix_expression"; static unsigned char
_temp1441[ 19u]="primary_expression"; static unsigned char _temp1442[ 25u]="argument_expression_list";
static unsigned char _temp1443[ 26u]="argument_expression_list0"; static
unsigned char _temp1444[ 9u]="constant"; static unsigned char _temp1445[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 237u]={{ _temp1209, _temp1209, _temp1209
+  2u},{ _temp1210, _temp1210, _temp1210 +  6u},{ _temp1211, _temp1211,
_temp1211 +  12u},{ _temp1212, _temp1212, _temp1212 +  5u},{ _temp1213,
_temp1213, _temp1213 +  9u},{ _temp1214, _temp1214, _temp1214 +  7u},{ _temp1215,
_temp1215, _temp1215 +  7u},{ _temp1216, _temp1216, _temp1216 +  8u},{ _temp1217,
_temp1217, _temp1217 +  5u},{ _temp1218, _temp1218, _temp1218 +  5u},{ _temp1219,
_temp1219, _temp1219 +  6u},{ _temp1220, _temp1220, _temp1220 +  4u},{ _temp1221,
_temp1221, _temp1221 +  5u},{ _temp1222, _temp1222, _temp1222 +  6u},{ _temp1223,
_temp1223, _temp1223 +  7u},{ _temp1224, _temp1224, _temp1224 +  7u},{ _temp1225,
_temp1225, _temp1225 +  9u},{ _temp1226, _temp1226, _temp1226 +  6u},{ _temp1227,
_temp1227, _temp1227 +  9u},{ _temp1228, _temp1228, _temp1228 +  9u},{ _temp1229,
_temp1229, _temp1229 +  7u},{ _temp1230, _temp1230, _temp1230 +  6u},{ _temp1231,
_temp1231, _temp1231 +  5u},{ _temp1232, _temp1232, _temp1232 +  8u},{ _temp1233,
_temp1233, _temp1233 +  7u},{ _temp1234, _temp1234, _temp1234 +  7u},{ _temp1235,
_temp1235, _temp1235 +  9u},{ _temp1236, _temp1236, _temp1236 +  3u},{ _temp1237,
_temp1237, _temp1237 +  5u},{ _temp1238, _temp1238, _temp1238 +  7u},{ _temp1239,
_temp1239, _temp1239 +  6u},{ _temp1240, _temp1240, _temp1240 +  3u},{ _temp1241,
_temp1241, _temp1241 +  4u},{ _temp1242, _temp1242, _temp1242 +  5u},{ _temp1243,
_temp1243, _temp1243 +  9u},{ _temp1244, _temp1244, _temp1244 +  6u},{ _temp1245,
_temp1245, _temp1245 +  7u},{ _temp1246, _temp1246, _temp1246 +  5u},{ _temp1247,
_temp1247, _temp1247 +  8u},{ _temp1248, _temp1248, _temp1248 +  4u},{ _temp1249,
_temp1249, _temp1249 +  6u},{ _temp1250, _temp1250, _temp1250 +  4u},{ _temp1251,
_temp1251, _temp1251 +  6u},{ _temp1252, _temp1252, _temp1252 +  4u},{ _temp1253,
_temp1253, _temp1253 +  9u},{ _temp1254, _temp1254, _temp1254 +  9u},{ _temp1255,
_temp1255, _temp1255 +  6u},{ _temp1256, _temp1256, _temp1256 +  10u},{
_temp1257, _temp1257, _temp1257 +  7u},{ _temp1258, _temp1258, _temp1258 +  8u},{
_temp1259, _temp1259, _temp1259 +  9u},{ _temp1260, _temp1260, _temp1260 +  5u},{
_temp1261, _temp1261, _temp1261 +  8u},{ _temp1262, _temp1262, _temp1262 +  4u},{
_temp1263, _temp1263, _temp1263 +  7u},{ _temp1264, _temp1264, _temp1264 +  7u},{
_temp1265, _temp1265, _temp1265 +  8u},{ _temp1266, _temp1266, _temp1266 +  7u},{
_temp1267, _temp1267, _temp1267 +  8u},{ _temp1268, _temp1268, _temp1268 +  9u},{
_temp1269, _temp1269, _temp1269 +  9u},{ _temp1270, _temp1270, _temp1270 +  7u},{
_temp1271, _temp1271, _temp1271 +  5u},{ _temp1272, _temp1272, _temp1272 +  8u},{
_temp1273, _temp1273, _temp1273 +  4u},{ _temp1274, _temp1274, _temp1274 +  7u},{
_temp1275, _temp1275, _temp1275 +  7u},{ _temp1276, _temp1276, _temp1276 +  7u},{
_temp1277, _temp1277, _temp1277 +  8u},{ _temp1278, _temp1278, _temp1278 +  9u},{
_temp1279, _temp1279, _temp1279 +  6u},{ _temp1280, _temp1280, _temp1280 +  6u},{
_temp1281, _temp1281, _temp1281 +  6u},{ _temp1282, _temp1282, _temp1282 +  6u},{
_temp1283, _temp1283, _temp1283 +  7u},{ _temp1284, _temp1284, _temp1284 +  6u},{
_temp1285, _temp1285, _temp1285 +  11u},{ _temp1286, _temp1286, _temp1286 +  11u},{
_temp1287, _temp1287, _temp1287 +  11u},{ _temp1288, _temp1288, _temp1288 +  11u},{
_temp1289, _temp1289, _temp1289 +  11u},{ _temp1290, _temp1290, _temp1290 +  12u},{
_temp1291, _temp1291, _temp1291 +  13u},{ _temp1292, _temp1292, _temp1292 +  11u},{
_temp1293, _temp1293, _temp1293 +  11u},{ _temp1294, _temp1294, _temp1294 +  10u},{
_temp1295, _temp1295, _temp1295 +  9u},{ _temp1296, _temp1296, _temp1296 +  11u},{
_temp1297, _temp1297, _temp1297 +  12u},{ _temp1298, _temp1298, _temp1298 +  11u},{
_temp1299, _temp1299, _temp1299 +  17u},{ _temp1300, _temp1300, _temp1300 +  7u},{
_temp1301, _temp1301, _temp1301 +  19u},{ _temp1302, _temp1302, _temp1302 +  18u},{
_temp1303, _temp1303, _temp1303 +  9u},{ _temp1304, _temp1304, _temp1304 +  16u},{
_temp1305, _temp1305, _temp1305 +  18u},{ _temp1306, _temp1306, _temp1306 +  10u},{
_temp1307, _temp1307, _temp1307 +  4u},{ _temp1308, _temp1308, _temp1308 +  4u},{
_temp1309, _temp1309, _temp1309 +  4u},{ _temp1310, _temp1310, _temp1310 +  4u},{
_temp1311, _temp1311, _temp1311 +  4u},{ _temp1312, _temp1312, _temp1312 +  4u},{
_temp1313, _temp1313, _temp1313 +  4u},{ _temp1314, _temp1314, _temp1314 +  4u},{
_temp1315, _temp1315, _temp1315 +  4u},{ _temp1316, _temp1316, _temp1316 +  4u},{
_temp1317, _temp1317, _temp1317 +  4u},{ _temp1318, _temp1318, _temp1318 +  4u},{
_temp1319, _temp1319, _temp1319 +  4u},{ _temp1320, _temp1320, _temp1320 +  4u},{
_temp1321, _temp1321, _temp1321 +  4u},{ _temp1322, _temp1322, _temp1322 +  4u},{
_temp1323, _temp1323, _temp1323 +  4u},{ _temp1324, _temp1324, _temp1324 +  4u},{
_temp1325, _temp1325, _temp1325 +  4u},{ _temp1326, _temp1326, _temp1326 +  4u},{
_temp1327, _temp1327, _temp1327 +  4u},{ _temp1328, _temp1328, _temp1328 +  4u},{
_temp1329, _temp1329, _temp1329 +  4u},{ _temp1330, _temp1330, _temp1330 +  4u},{
_temp1331, _temp1331, _temp1331 +  4u},{ _temp1332, _temp1332, _temp1332 +  4u},{
_temp1333, _temp1333, _temp1333 +  4u},{ _temp1334, _temp1334, _temp1334 +  5u},{
_temp1335, _temp1335, _temp1335 +  17u},{ _temp1336, _temp1336, _temp1336 +  21u},{
_temp1337, _temp1337, _temp1337 +  20u},{ _temp1338, _temp1338, _temp1338 +  21u},{
_temp1339, _temp1339, _temp1339 +  13u},{ _temp1340, _temp1340, _temp1340 +  15u},{
_temp1341, _temp1341, _temp1341 +  17u},{ _temp1342, _temp1342, _temp1342 +  19u},{
_temp1343, _temp1343, _temp1343 +  12u},{ _temp1344, _temp1344, _temp1344 +  17u},{
_temp1345, _temp1345, _temp1345 +  23u},{ _temp1346, _temp1346, _temp1346 +  24u},{
_temp1347, _temp1347, _temp1347 +  15u},{ _temp1348, _temp1348, _temp1348 +  11u},{
_temp1349, _temp1349, _temp1349 +  15u},{ _temp1350, _temp1350, _temp1350 +  10u},{
_temp1351, _temp1351, _temp1351 +  15u},{ _temp1352, _temp1352, _temp1352 +  5u},{
_temp1353, _temp1353, _temp1353 +  15u},{ _temp1354, _temp1354, _temp1354 +  15u},{
_temp1355, _temp1355, _temp1355 +  11u},{ _temp1356, _temp1356, _temp1356 +  22u},{
_temp1357, _temp1357, _temp1357 +  26u},{ _temp1358, _temp1358, _temp1358 +  16u},{
_temp1359, _temp1359, _temp1359 +  16u},{ _temp1360, _temp1360, _temp1360 +  24u},{
_temp1361, _temp1361, _temp1361 +  25u},{ _temp1362, _temp1362, _temp1362 +  21u},{
_temp1363, _temp1363, _temp1363 +  22u},{ _temp1364, _temp1364, _temp1364 +  16u},{
_temp1365, _temp1365, _temp1365 +  19u},{ _temp1366, _temp1366, _temp1366 +  25u},{
_temp1367, _temp1367, _temp1367 +  23u},{ _temp1368, _temp1368, _temp1368 +  24u},{
_temp1369, _temp1369, _temp1369 +  18u},{ _temp1370, _temp1370, _temp1370 +  17u},{
_temp1371, _temp1371, _temp1371 +  18u},{ _temp1372, _temp1372, _temp1372 +  17u},{
_temp1373, _temp1373, _temp1373 +  18u},{ _temp1374, _temp1374, _temp1374 +  12u},{
_temp1375, _temp1375, _temp1375 +  11u},{ _temp1376, _temp1376, _temp1376 +  18u},{
_temp1377, _temp1377, _temp1377 +  8u},{ _temp1378, _temp1378, _temp1378 +  13u},{
_temp1379, _temp1379, _temp1379 +  8u},{ _temp1380, _temp1380, _temp1380 +  4u},{
_temp1381, _temp1381, _temp1381 +  20u},{ _temp1382, _temp1382, _temp1382 +  20u},{
_temp1383, _temp1383, _temp1383 +  9u},{ _temp1384, _temp1384, _temp1384 +  16u},{
_temp1385, _temp1385, _temp1385 +  19u},{ _temp1386, _temp1386, _temp1386 +  10u},{
_temp1387, _temp1387, _temp1387 +  16u},{ _temp1388, _temp1388, _temp1388 +  11u},{
_temp1389, _temp1389, _temp1389 +  14u},{ _temp1390, _temp1390, _temp1390 +  11u},{
_temp1391, _temp1391, _temp1391 +  15u},{ _temp1392, _temp1392, _temp1392 +  22u},{
_temp1393, _temp1393, _temp1393 +  16u},{ _temp1394, _temp1394, _temp1394 +  17u},{
_temp1395, _temp1395, _temp1395 +  12u},{ _temp1396, _temp1396, _temp1396 +  18u},{
_temp1397, _temp1397, _temp1397 +  17u},{ _temp1398, _temp1398, _temp1398 +  12u},{
_temp1399, _temp1399, _temp1399 +  16u},{ _temp1400, _temp1400, _temp1400 +  11u},{
_temp1401, _temp1401, _temp1401 +  10u},{ _temp1402, _temp1402, _temp1402 +  14u},{
_temp1403, _temp1403, _temp1403 +  15u},{ _temp1404, _temp1404, _temp1404 +  20u},{
_temp1405, _temp1405, _temp1405 +  27u},{ _temp1406, _temp1406, _temp1406 +  10u},{
_temp1407, _temp1407, _temp1407 +  18u},{ _temp1408, _temp1408, _temp1408 +  21u},{
_temp1409, _temp1409, _temp1409 +  19u},{ _temp1410, _temp1410, _temp1410 +  16u},{
_temp1411, _temp1411, _temp1411 +  20u},{ _temp1412, _temp1412, _temp1412 +  15u},{
_temp1413, _temp1413, _temp1413 +  16u},{ _temp1414, _temp1414, _temp1414 +  20u},{
_temp1415, _temp1415, _temp1415 +  15u},{ _temp1416, _temp1416, _temp1416 +  8u},{
_temp1417, _temp1417, _temp1417 +  19u},{ _temp1418, _temp1418, _temp1418 +  20u},{
_temp1419, _temp1419, _temp1419 +  14u},{ _temp1420, _temp1420, _temp1420 +  19u},{
_temp1421, _temp1421, _temp1421 +  20u},{ _temp1422, _temp1422, _temp1422 +  11u},{
_temp1423, _temp1423, _temp1423 +  22u},{ _temp1424, _temp1424, _temp1424 +  20u},{
_temp1425, _temp1425, _temp1425 +  23u},{ _temp1426, _temp1426, _temp1426 +  20u},{
_temp1427, _temp1427, _temp1427 +  22u},{ _temp1428, _temp1428, _temp1428 +  23u},{
_temp1429, _temp1429, _temp1429 +  24u},{ _temp1430, _temp1430, _temp1430 +  24u},{
_temp1431, _temp1431, _temp1431 +  15u},{ _temp1432, _temp1432, _temp1432 +  20u},{
_temp1433, _temp1433, _temp1433 +  22u},{ _temp1434, _temp1434, _temp1434 +  17u},{
_temp1435, _temp1435, _temp1435 +  20u},{ _temp1436, _temp1436, _temp1436 +  26u},{
_temp1437, _temp1437, _temp1437 +  16u},{ _temp1438, _temp1438, _temp1438 +  17u},{
_temp1439, _temp1439, _temp1439 +  15u},{ _temp1440, _temp1440, _temp1440 +  19u},{
_temp1441, _temp1441, _temp1441 +  19u},{ _temp1442, _temp1442, _temp1442 +  25u},{
_temp1443, _temp1443, _temp1443 +  26u},{ _temp1444, _temp1444, _temp1444 +  9u},{
_temp1445, _temp1445, _temp1445 +  20u}}; static short Cyc_yyr1[ 410u]={ 0, 125,
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
short*)({ unsigned int _temp3307= 10000u; short* _temp3308=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3307));{ unsigned int
_temp3309= _temp3307; unsigned int i; for( i= 0; i <  _temp3309; i ++){
_temp3308[ i]=( short) 0;}}; _temp3308;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3304= 10000u; void** _temp3305=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3304));{ unsigned int _temp3306= _temp3304;
unsigned int i; for( i= 0; i <  _temp3306; i ++){ _temp3305[ i]= Cyc_yylval;}};
_temp3305;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3301= 10000u; struct Cyc_Yyltype* _temp3302=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3301));{
unsigned int _temp3303= _temp3301; unsigned int i; for( i= 0; i <  _temp3303; i
++){ _temp3302[ i]= Cyc_yynewloc();}}; _temp3302;}); int yystacksize= 10000;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:
yyss[ _check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate;
if( yyssp_offset >=  yystacksize -  1){ if( yystacksize >=  10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 831u, yystate)]; if( yyn ==  - 32768){ goto
yydefault;} if( Cyc_yychar ==  - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar >  0? Cyc_yychar <= 
352: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 353u, Cyc_yychar)]:
237;} yyn += yychar1; if(( yyn <  0? 1: yyn >  4995)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4996u, yyn)] !=  yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4996u, yyn)]; if( yyn <  0){
if( yyn ==  - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
==  0){ goto yyerrlab;}} if( yyn ==  830){ return 0;} if( Cyc_yychar !=  0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus !=  0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 831u, yystate)]; if(
yyn ==  0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 410u, yyn)]; if( yylen >  0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset +  1) -  yylen)];} switch(
yyn){ case 1: _LL1446: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1447:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1449=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1449[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1450; _temp1450.tag= Cyc_DeclList_tok; _temp1450.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1450;});
_temp1449;}); break; case 3: _LL1448: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1452=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1452[ 0]=({ struct Cyc_DeclList_tok_struct _temp1453; _temp1453.tag= Cyc_DeclList_tok;
_temp1453.f1=({ struct Cyc_List_List* _temp1454=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1454->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1455=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1455->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1456=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1456[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1457; _temp1457.tag= Cyc_Absyn_Using_d;
_temp1457.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1457.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1457;}); _temp1456;}));
_temp1455->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1455;});
_temp1454->tl= 0; _temp1454;}); _temp1453;}); _temp1452;}); Cyc_Lex_leave_using();
break; case 4: _LL1451: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1459=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1459[ 0]=({ struct Cyc_DeclList_tok_struct _temp1460; _temp1460.tag= Cyc_DeclList_tok;
_temp1460.f1=({ struct Cyc_List_List* _temp1461=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1461->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1462=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1462->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1463=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1463[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1464; _temp1464.tag= Cyc_Absyn_Using_d;
_temp1464.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp1464.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1464;});
_temp1463;})); _temp1462->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1462;}); _temp1461->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1461;}); _temp1460;});
_temp1459;}); break; case 5: _LL1458: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1466=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1466[ 0]=({ struct Cyc_DeclList_tok_struct _temp1467; _temp1467.tag= Cyc_DeclList_tok;
_temp1467.f1=({ struct Cyc_List_List* _temp1468=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1468->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1469=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1469->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1470=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1470[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1471; _temp1471.tag=
Cyc_Absyn_Namespace_d; _temp1471.f1=({ struct _tagged_arr* _temp1472=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1472[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1472;});
_temp1471.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1471;}); _temp1470;})); _temp1469->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1469;}); _temp1468->tl= 0; _temp1468;}); _temp1467;}); _temp1466;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1465: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1474=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1474[ 0]=({ struct Cyc_DeclList_tok_struct _temp1475; _temp1475.tag= Cyc_DeclList_tok;
_temp1475.f1=({ struct Cyc_List_List* _temp1476=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1476->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1477=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1477->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1478=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1478[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1479; _temp1479.tag=
Cyc_Absyn_Namespace_d; _temp1479.f1=({ struct _tagged_arr* _temp1480=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1480[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1480;});
_temp1479.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1479;}); _temp1478;})); _temp1477->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1477;}); _temp1476->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1476;}); _temp1475;});
_temp1474;}); break; case 7: _LL1473: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1482=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1482[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1483; _temp1483.tag= Cyc_DeclList_tok; _temp1483.f1=({ struct Cyc_List_List*
_temp1484=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1484->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1485=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1485->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1486=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1486[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1487; _temp1487.tag= Cyc_Absyn_ExternC_d;
_temp1487.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1487;}); _temp1486;})); _temp1485->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1485;}); _temp1484->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1484;}); _temp1483;});
_temp1482;}); break; case 8: _LL1481: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1489=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1489[ 0]=({ struct Cyc_DeclList_tok_struct _temp1490; _temp1490.tag= Cyc_DeclList_tok;
_temp1490.f1= 0; _temp1490;}); _temp1489;}); break; case 9: _LL1488: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1492=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1492[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1493; _temp1493.tag= Cyc_DeclList_tok; _temp1493.f1=({ struct Cyc_List_List*
_temp1494=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1494->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1495=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1495[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1496; _temp1496.tag= Cyc_Absyn_Fn_d;
_temp1496.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1496;}); _temp1495;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1494->tl=
0; _temp1494;}); _temp1493;}); _temp1492;}); break; case 10: _LL1491: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1497: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1499=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1499[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1500; _temp1500.tag= Cyc_FnDecl_tok; _temp1500.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1500;}); _temp1499;}); break; case 12: _LL1498: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1502=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1502[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1503; _temp1503.tag= Cyc_FnDecl_tok;
_temp1503.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1504=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1504->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1504;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1503;}); _temp1502;}); break; case 13: _LL1501: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1506=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1506[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1507; _temp1507.tag= Cyc_FnDecl_tok;
_temp1507.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1507;}); _temp1506;}); break; case 14: _LL1505: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1509=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1509[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1510; _temp1510.tag= Cyc_FnDecl_tok;
_temp1510.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1511=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1511->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1511;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1510;}); _temp1509;}); break; case 15: _LL1508: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1513=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1513[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1514; _temp1514.tag= Cyc_FnDecl_tok;
_temp1514.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1515=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1515->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1515;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1514;}); _temp1513;}); break; case 16: _LL1512: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1517=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1517[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1518; _temp1518.tag= Cyc_FnDecl_tok;
_temp1518.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1519=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1519->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1519;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1518;}); _temp1517;}); break; case 17: _LL1516: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1520:
Cyc_Lex_leave_using(); break; case 19: _LL1521: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1523=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1523[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1523;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1522: Cyc_Lex_leave_namespace(); break; case 21: _LL1524: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1526=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1526[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1527; _temp1527.tag= Cyc_DeclList_tok; _temp1527.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1527;}); _temp1526;}); break; case 22: _LL1525: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1529=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1529[ 0]=({ struct Cyc_DeclList_tok_struct _temp1530; _temp1530.tag= Cyc_DeclList_tok;
_temp1530.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1530;}); _temp1529;}); break; case 23: _LL1528: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1532=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1532[ 0]=({ struct Cyc_DeclList_tok_struct _temp1533; _temp1533.tag= Cyc_DeclList_tok;
_temp1533.f1=({ struct Cyc_List_List* _temp1534=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1534->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1534->tl=
0; _temp1534;}); _temp1533;}); _temp1532;}); break; case 24: _LL1531: { struct
Cyc_List_List* _temp1536= 0;{ struct Cyc_List_List* _temp1537= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0;
_temp1537 !=  0; _temp1537= _temp1537->tl){ struct _tagged_arr* _temp1538=(
struct _tagged_arr*) _temp1537->hd; struct _tuple1* _temp1539=({ struct _tuple1*
_temp1542=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1542->f1=
Cyc_Absyn_rel_ns_null; _temp1542->f2= _temp1538; _temp1542;}); struct Cyc_Absyn_Vardecl*
_temp1540= Cyc_Absyn_new_vardecl( _temp1539, Cyc_Absyn_wildtyp( 0), 0);
_temp1536=({ struct Cyc_List_List* _temp1541=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1541->hd=( void*) _temp1540; _temp1541->tl=
_temp1536; _temp1541;});}} _temp1536=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1536); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1543=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1543[ 0]=({ struct Cyc_DeclList_tok_struct _temp1544; _temp1544.tag= Cyc_DeclList_tok;
_temp1544.f1=({ struct Cyc_List_List* _temp1545=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1545->hd=( void*) Cyc_Absyn_letv_decl(
_temp1536, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1545->tl= 0; _temp1545;}); _temp1544;});
_temp1543;}); break;} case 25: _LL1535: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1546:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1548=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1548[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1549; _temp1549.tag= Cyc_DeclList_tok; _temp1549.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1549;});
_temp1548;}); break; case 27: _LL1547: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1551=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1551[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1552; _temp1552.tag= Cyc_DeclSpec_tok;
_temp1552.f1=({ struct Cyc_Parse_Declaration_spec* _temp1553=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1553->sc=({ struct
Cyc_Core_Opt* _temp1554=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1554->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1554;});
_temp1553->tq= Cyc_Absyn_empty_tqual(); _temp1553->type_specs= 0; _temp1553->is_inline=
0; _temp1553->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1553;}); _temp1552;});
_temp1551;}); break; case 28: _LL1550: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc !=  0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1556=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1556[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1557; _temp1557.tag= Cyc_DeclSpec_tok; _temp1557.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1558=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1558->sc=({ struct Cyc_Core_Opt* _temp1559=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1559->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1559;});
_temp1558->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1558->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1558->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1558->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1558;}); _temp1557;}); _temp1556;}); break; case 29: _LL1555: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1561=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1561[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1562; _temp1562.tag= Cyc_DeclSpec_tok; _temp1562.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1563=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1563->sc= 0; _temp1563->tq= Cyc_Absyn_empty_tqual(); _temp1563->type_specs=({
struct Cyc_List_List* _temp1564=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1564->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1564->tl= 0;
_temp1564;}); _temp1563->is_inline= 0; _temp1563->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1563;});
_temp1562;}); _temp1561;}); break; case 30: _LL1560: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1566=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1566[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1567; _temp1567.tag= Cyc_DeclSpec_tok;
_temp1567.f1=({ struct Cyc_Parse_Declaration_spec* _temp1568=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1568->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1568->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1568->type_specs=({ struct Cyc_List_List* _temp1569=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1569->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1569->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1569;});
_temp1568->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1568->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1568;}); _temp1567;}); _temp1566;}); break; case 31: _LL1565: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1571=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1571[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1572; _temp1572.tag= Cyc_DeclSpec_tok; _temp1572.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1573=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1573->sc= 0; _temp1573->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1573->type_specs=
0; _temp1573->is_inline= 0; _temp1573->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1573;});
_temp1572;}); _temp1571;}); break; case 32: _LL1570: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1575=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1575[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1576; _temp1576.tag= Cyc_DeclSpec_tok;
_temp1576.f1=({ struct Cyc_Parse_Declaration_spec* _temp1577=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1577->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1577->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1577->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1577->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1577->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1577;}); _temp1576;}); _temp1575;}); break; case 33: _LL1574: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1579=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1579[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1580; _temp1580.tag= Cyc_DeclSpec_tok; _temp1580.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1581=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1581->sc= 0; _temp1581->tq= Cyc_Absyn_empty_tqual(); _temp1581->type_specs=
0; _temp1581->is_inline= 1; _temp1581->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1581;});
_temp1580;}); _temp1579;}); break; case 34: _LL1578: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1583=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1583[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1584; _temp1584.tag= Cyc_DeclSpec_tok;
_temp1584.f1=({ struct Cyc_Parse_Declaration_spec* _temp1585=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1585->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1585->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1585->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1585->is_inline=
1; _temp1585->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1585;}); _temp1584;}); _temp1583;}); break; case 35: _LL1582: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1587=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1587[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1588; _temp1588.tag= Cyc_StorageClass_tok;
_temp1588.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1588;}); _temp1587;});
break; case 36: _LL1586: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1590=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1590[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1591; _temp1591.tag=
Cyc_StorageClass_tok; _temp1591.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1591;}); _temp1590;}); break; case 37: _LL1589: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1593=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1593[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1594; _temp1594.tag=
Cyc_StorageClass_tok; _temp1594.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1594;}); _temp1593;}); break; case 38: _LL1592: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1596=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1596[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1597; _temp1597.tag=
Cyc_StorageClass_tok; _temp1597.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1597;}); _temp1596;}); break; case 39: _LL1595: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1599=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1599[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1600; _temp1600.tag= Cyc_StorageClass_tok;
_temp1600.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1600;}); _temp1599;});
break; case 40: _LL1598: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1602=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1602[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1603; _temp1603.tag=
Cyc_StorageClass_tok; _temp1603.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1603;}); _temp1602;}); break; case 41: _LL1601: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1605=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1605[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1606; _temp1606.tag=
Cyc_StorageClass_tok; _temp1606.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1606;}); _temp1605;}); break; case 42: _LL1604: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1608=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1608[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1609; _temp1609.tag=
Cyc_AttributeList_tok; _temp1609.f1= 0; _temp1609;}); _temp1608;}); break; case
43: _LL1607: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1610: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1612=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1612[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1613; _temp1613.tag=
Cyc_AttributeList_tok; _temp1613.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1613;});
_temp1612;}); break; case 45: _LL1611: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1615=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1615[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1616; _temp1616.tag=
Cyc_AttributeList_tok; _temp1616.f1=({ struct Cyc_List_List* _temp1617=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1617->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1617->tl= 0; _temp1617;}); _temp1616;}); _temp1615;});
break; case 46: _LL1614: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1619=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1619[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1620; _temp1620.tag=
Cyc_AttributeList_tok; _temp1620.f1=({ struct Cyc_List_List* _temp1621=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1621->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1621->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1621;}); _temp1620;});
_temp1619;}); break; case 47: _LL1618: { struct _tagged_arr _temp1623= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1623, sizeof( unsigned char)) >  4?*(( const unsigned char*)
_check_unknown_subscript( _temp1623, sizeof( unsigned char), 0)) == '_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1623, sizeof( unsigned char),
1)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1623,
sizeof( unsigned char),( int)( _get_arr_size( _temp1623, sizeof( unsigned char))
-  2))) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1623,
sizeof( unsigned char),( int)( _get_arr_size( _temp1623, sizeof( unsigned char))
-  3))) == '_': 0){ _temp1623=( struct _tagged_arr) Cyc_Std_substring( _temp1623,
2, _get_arr_size( _temp1623, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0;
i <  16u; ++ i){ if( Cyc_Std_strcmp( _temp1623,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1624=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1624[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1625; _temp1625.tag= Cyc_Attribute_tok; _temp1625.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1625;}); _temp1624;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1626=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1626[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1627; _temp1627.tag= Cyc_Attribute_tok; _temp1627.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1627;}); _temp1626;});} break;}} case
48: _LL1622: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1629=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1629[ 0]=({ struct Cyc_Attribute_tok_struct _temp1630; _temp1630.tag= Cyc_Attribute_tok;
_temp1630.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1630;}); _temp1629;});
break; case 49: _LL1628: { struct _tagged_arr _temp1632= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1635; int _temp1636; struct _tuple15* _temp1633= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1635=*
_temp1633; _LL1637: _temp1636= _temp1635.f2; goto _LL1634; _LL1634: { void* a;
if( Cyc_Std_zstrcmp( _temp1632, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1632, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1636 <  0? 1: _temp1636 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 0 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1638=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1638[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1639; _temp1639.tag= Cyc_Absyn_Regparm_att; _temp1639.f1= _temp1636;
_temp1639;}); _temp1638;});} else{ if( Cyc_Std_zstrcmp( _temp1632, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1632, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1636 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1636;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1640=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1640[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1641; _temp1641.tag= Cyc_Absyn_Aligned_att; _temp1641.f1= _temp1636;
_temp1641;}); _temp1640;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1642=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1642[ 0]=({ struct Cyc_Attribute_tok_struct _temp1643; _temp1643.tag= Cyc_Attribute_tok;
_temp1643.f1=( void*) a; _temp1643;}); _temp1642;}); break;}} case 50: _LL1631: {
struct _tagged_arr _temp1645= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1646= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1645, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1645, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1647=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1647[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1648; _temp1648.tag=
Cyc_Absyn_Section_att; _temp1648.f1= _temp1646; _temp1648;}); _temp1647;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1649=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1649[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1650; _temp1650.tag= Cyc_Attribute_tok; _temp1650.f1=(
void*) a; _temp1650;}); _temp1649;}); break;} case 51: _LL1644: { struct
_tagged_arr _temp1652= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1653= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1656; struct _tuple15 _temp1654=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1657:
_temp1656= _temp1654.f2; goto _LL1655; _LL1655: { int _temp1660; struct _tuple15
_temp1658=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1661: _temp1660= _temp1658.f2; goto _LL1659; _LL1659: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1652, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1652, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1653, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1662=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1662[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1663; _temp1663.tag=
Cyc_Absyn_Format_att; _temp1663.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1663.f2= _temp1656; _temp1663.f3= _temp1660; _temp1663;}); _temp1662;});}
else{ if( Cyc_Std_zstrcmp( _temp1653, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1664=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1664[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1665; _temp1665.tag=
Cyc_Absyn_Format_att; _temp1665.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1665.f2= _temp1656; _temp1665.f3= _temp1660; _temp1665;}); _temp1664;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1666=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1666[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1667; _temp1667.tag= Cyc_Attribute_tok; _temp1667.f1=(
void*) a; _temp1667;}); _temp1666;}); break;}}} case 52: _LL1651: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1669=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1669[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1670; _temp1670.tag= Cyc_TypeSpecifier_tok;
_temp1670.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1670;});
_temp1669;}); break; case 53: _LL1668: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1672=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1672[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1673; _temp1673.tag=
Cyc_TypeSpecifier_tok; _temp1673.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1673;}); _temp1672;}); break; case 54: _LL1671:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1675=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1675[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1676; _temp1676.tag= Cyc_TypeSpecifier_tok;
_temp1676.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1676;});
_temp1675;}); break; case 55: _LL1674: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1678=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1678[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1679; _temp1679.tag=
Cyc_TypeSpecifier_tok; _temp1679.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1680=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1680[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1681; _temp1681.tag=
Cyc_Parse_Short_spec; _temp1681.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1681;});
_temp1680;})); _temp1679;}); _temp1678;}); break; case 56: _LL1677: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1683=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1683[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1684; _temp1684.tag= Cyc_TypeSpecifier_tok;
_temp1684.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1684;});
_temp1683;}); break; case 57: _LL1682: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1686=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1686[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1687; _temp1687.tag=
Cyc_TypeSpecifier_tok; _temp1687.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1688=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1688[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1689; _temp1689.tag= Cyc_Parse_Long_spec;
_temp1689.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1689;}); _temp1688;})); _temp1687;}); _temp1686;});
break; case 58: _LL1685: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1691=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1691[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1692; _temp1692.tag=
Cyc_TypeSpecifier_tok; _temp1692.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1692;}); _temp1691;}); break; case 59: _LL1690:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1694=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1694[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1695; _temp1695.tag= Cyc_TypeSpecifier_tok;
_temp1695.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t( 0), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1695;});
_temp1694;}); break; case 60: _LL1693: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1697=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1697[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1698; _temp1698.tag=
Cyc_TypeSpecifier_tok; _temp1698.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1699=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1699[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1700; _temp1700.tag=
Cyc_Parse_Signed_spec; _temp1700.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1700;});
_temp1699;})); _temp1698;}); _temp1697;}); break; case 61: _LL1696: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1702=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1702[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1703; _temp1703.tag= Cyc_TypeSpecifier_tok;
_temp1703.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1704=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1704[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1705; _temp1705.tag= Cyc_Parse_Unsigned_spec; _temp1705.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1705;});
_temp1704;})); _temp1703;}); _temp1702;}); break; case 62: _LL1701: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1706:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1707: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1708: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1710=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1710[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1711; _temp1711.tag=
Cyc_TypeSpecifier_tok; _temp1711.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1711;});
_temp1710;}); break; case 66: _LL1709: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1713=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1713[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1714; _temp1714.tag=
Cyc_TypeSpecifier_tok; _temp1714.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1715=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1715[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1716; _temp1716.tag= Cyc_Absyn_TypedefType;
_temp1716.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1716.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1716.f3= 0;
_temp1716;}); _temp1715;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1714;});
_temp1713;}); break; case 67: _LL1712: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1718=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1718[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1719; _temp1719.tag=
Cyc_TypeSpecifier_tok; _temp1719.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1720=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1720[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1721; _temp1721.tag= Cyc_Absyn_TupleType;
_temp1721.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1721;});
_temp1720;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1719;}); _temp1718;}); break; case 68:
_LL1717: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1723=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1723[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1724; _temp1724.tag=
Cyc_TypeSpecifier_tok; _temp1724.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1725=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1725[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1726; _temp1726.tag= Cyc_Absyn_RgnHandleType;
_temp1726.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1726;}); _temp1725;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1724;}); _temp1723;}); break; case 69: _LL1722: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1728=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1728[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1729; _temp1729.tag=
Cyc_TypeSpecifier_tok; _temp1729.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_SizeofType_struct* _temp1730=( struct Cyc_Absyn_SizeofType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SizeofType_struct)); _temp1730[ 0]=({ struct
Cyc_Absyn_SizeofType_struct _temp1731; _temp1731.tag= Cyc_Absyn_SizeofType;
_temp1731.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1731;}); _temp1730;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1729;}); _temp1728;}); break; case 70: _LL1727: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1733=( struct Cyc_Kind_tok_struct*) _cycalloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1733[ 0]=({ struct Cyc_Kind_tok_struct _temp1734; _temp1734.tag= Cyc_Kind_tok;
_temp1734.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1734;});
_temp1733;}); break; case 71: _LL1732: { struct _tagged_arr* _temp1738; void*
_temp1740; struct _tuple1 _temp1736=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1741: _temp1740=
_temp1736.f1; goto _LL1739; _LL1739: _temp1738= _temp1736.f2; goto _LL1737;
_LL1737: if( _temp1740 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1742=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1742[ 0]=({ struct Cyc_Kind_tok_struct
_temp1743; _temp1743.tag= Cyc_Kind_tok; _temp1743.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1738, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1743;}); _temp1742;}); break;} case 72:
_LL1735: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1745=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1745[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1746; _temp1746.tag= Cyc_TypeQual_tok;
_temp1746.f1=({ struct Cyc_Absyn_Tqual _temp1747; _temp1747.q_const= 1;
_temp1747.q_volatile= 0; _temp1747.q_restrict= 0; _temp1747;}); _temp1746;});
_temp1745;}); break; case 73: _LL1744: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1749=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1749[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1750; _temp1750.tag= Cyc_TypeQual_tok; _temp1750.f1=({ struct Cyc_Absyn_Tqual
_temp1751; _temp1751.q_const= 0; _temp1751.q_volatile= 1; _temp1751.q_restrict=
0; _temp1751;}); _temp1750;}); _temp1749;}); break; case 74: _LL1748: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1753=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1753[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1754; _temp1754.tag= Cyc_TypeQual_tok;
_temp1754.f1=({ struct Cyc_Absyn_Tqual _temp1755; _temp1755.q_const= 0;
_temp1755.q_volatile= 0; _temp1755.q_restrict= 1; _temp1755;}); _temp1754;});
_temp1753;}); break; case 75: _LL1752: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1757=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1757[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1758; _temp1758.tag=
Cyc_TypeSpecifier_tok; _temp1758.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1759=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1759[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1760; _temp1760.tag= Cyc_Parse_Decl_spec;
_temp1760.f1=({ struct Cyc_Absyn_Decl* _temp1761=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1761->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1762=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1762[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1763; _temp1763.tag= Cyc_Absyn_Enum_d; _temp1763.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1764=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1764->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1764->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1764->fields=({
struct Cyc_Core_Opt* _temp1765=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1765->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1765;});
_temp1764;}); _temp1763;}); _temp1762;})); _temp1761->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1761;}); _temp1760;}); _temp1759;})); _temp1758;}); _temp1757;}); break;
case 76: _LL1756: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1767=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1767[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1768; _temp1768.tag=
Cyc_TypeSpecifier_tok; _temp1768.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1769=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1769[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1770; _temp1770.tag= Cyc_Absyn_EnumType;
_temp1770.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1770.f2= 0; _temp1770;}); _temp1769;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1768;}); _temp1767;}); break; case 77: _LL1766: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1772=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1772[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1773; _temp1773.tag=
Cyc_TypeSpecifier_tok; _temp1773.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1774=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1774[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1775; _temp1775.tag= Cyc_Parse_Type_spec;
_temp1775.f1=( void*)(( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp1776=(
struct Cyc_Absyn_AnonEnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct));
_temp1776[ 0]=({ struct Cyc_Absyn_AnonEnumType_struct _temp1777; _temp1777.tag=
Cyc_Absyn_AnonEnumType; _temp1777.f1= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1777;});
_temp1776;})); _temp1775.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1775;});
_temp1774;})); _temp1773;}); _temp1772;}); break; case 78: _LL1771: yyval=( void*)({
struct Cyc_Enumfield_tok_struct* _temp1779=( struct Cyc_Enumfield_tok_struct*)
_cycalloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp1779[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp1780; _temp1780.tag= Cyc_Enumfield_tok; _temp1780.f1=({
struct Cyc_Absyn_Enumfield* _temp1781=( struct Cyc_Absyn_Enumfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumfield)); _temp1781->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1781->tag= 0;
_temp1781->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1781;});
_temp1780;}); _temp1779;}); break; case 79: _LL1778: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1783=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1783[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1784; _temp1784.tag= Cyc_Enumfield_tok;
_temp1784.f1=({ struct Cyc_Absyn_Enumfield* _temp1785=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1785->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1785->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1785->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1785;});
_temp1784;}); _temp1783;}); break; case 80: _LL1782: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1787=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1787[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1788; _temp1788.tag=
Cyc_EnumfieldList_tok; _temp1788.f1=({ struct Cyc_List_List* _temp1789=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1789->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1789->tl= 0; _temp1789;}); _temp1788;}); _temp1787;});
break; case 81: _LL1786: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1791=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1791[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1792; _temp1792.tag=
Cyc_EnumfieldList_tok; _temp1792.f1=({ struct Cyc_List_List* _temp1793=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1793->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1793->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1793;}); _temp1792;});
_temp1791;}); break; case 82: _LL1790: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1795: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1797=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1797[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1798; _temp1798.tag=
Cyc_Absyn_AnonStructType; _temp1798.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1798;});
_temp1797;}); break; case Cyc_Parse_Union_su: _LL1796: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1800=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1800[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1801; _temp1801.tag= Cyc_Absyn_AnonUnionType; _temp1801.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1801;});
_temp1800;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1802=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1802[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1803; _temp1803.tag=
Cyc_TypeSpecifier_tok; _temp1803.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1804=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1804[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1805; _temp1805.tag= Cyc_Parse_Type_spec;
_temp1805.f1=( void*) t; _temp1805.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1805;});
_temp1804;})); _temp1803;}); _temp1802;}); break;} case 83: _LL1794: { struct
Cyc_List_List* _temp1807=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1808: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1810=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1810->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1810;}), _temp1807,({ struct Cyc_Core_Opt*
_temp1811=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1811->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1811;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1809: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1813=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1813->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1813;}),
_temp1807,({ struct Cyc_Core_Opt* _temp1814=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1814->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1814;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1815=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1815[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1816; _temp1816.tag=
Cyc_TypeSpecifier_tok; _temp1816.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1817=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1817[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1818; _temp1818.tag= Cyc_Parse_Decl_spec;
_temp1818.f1= d; _temp1818;}); _temp1817;})); _temp1816;}); _temp1815;}); break;}
case 84: _LL1806: { struct Cyc_List_List* _temp1820=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1821: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1823=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1823->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1823;}), _temp1820,({ struct Cyc_Core_Opt*
_temp1824=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1824->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1824;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1822: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1826=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1826->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1826;}),
_temp1820,({ struct Cyc_Core_Opt* _temp1827=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1827->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1827;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1828=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1828[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1829; _temp1829.tag=
Cyc_TypeSpecifier_tok; _temp1829.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1830=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1830[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1831; _temp1831.tag= Cyc_Parse_Decl_spec;
_temp1831.f1= d; _temp1831;}); _temp1830;})); _temp1829;}); _temp1828;}); break;}
case 85: _LL1819: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1833: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1835=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1835[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1836; _temp1836.tag=
Cyc_TypeSpecifier_tok; _temp1836.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1837=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1837[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1838; _temp1838.tag= Cyc_Absyn_StructType;
_temp1838.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1838.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1838.f3= 0;
_temp1838;}); _temp1837;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1836;});
_temp1835;}); break; case Cyc_Parse_Union_su: _LL1834: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1840=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1840[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1841; _temp1841.tag= Cyc_TypeSpecifier_tok;
_temp1841.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1842=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1842[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1843; _temp1843.tag= Cyc_Absyn_UnionType;
_temp1843.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1843.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1843.f3= 0;
_temp1843;}); _temp1842;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1841;});
_temp1840;}); break;} break; case 86: _LL1832: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1845: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1847=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1847[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1848; _temp1848.tag=
Cyc_TypeSpecifier_tok; _temp1848.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1849=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1849[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1850; _temp1850.tag= Cyc_Absyn_StructType;
_temp1850.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1850.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1850.f3= 0;
_temp1850;}); _temp1849;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1848;});
_temp1847;}); break; case Cyc_Parse_Union_su: _LL1846: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1852=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1852[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1853; _temp1853.tag= Cyc_TypeSpecifier_tok;
_temp1853.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1854=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1854[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1855; _temp1855.tag= Cyc_Absyn_UnionType;
_temp1855.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1855.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1855.f3= 0;
_temp1855;}); _temp1854;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1853;});
_temp1852;}); break;} break; case 87: _LL1844: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1857=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1857[ 0]=({ struct Cyc_TypeList_tok_struct _temp1858; _temp1858.tag= Cyc_TypeList_tok;
_temp1858.f1= 0; _temp1858;}); _temp1857;}); break; case 88: _LL1856: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1860=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1860[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1861; _temp1861.tag= Cyc_TypeList_tok; _temp1861.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1861;});
_temp1860;}); break; case 89: _LL1859:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1863=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1863[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1864; _temp1864.tag= Cyc_TypeList_tok; _temp1864.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1864;});
_temp1863;}); break; case 90: _LL1862: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1866=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1866[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1867; _temp1867.tag= Cyc_StructOrUnion_tok; _temp1867.f1= Cyc_Parse_Struct_su;
_temp1867;}); _temp1866;}); break; case 91: _LL1865: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1869=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1869[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1870; _temp1870.tag= Cyc_StructOrUnion_tok; _temp1870.f1= Cyc_Parse_Union_su;
_temp1870;}); _temp1869;}); break; case 92: _LL1868: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1872=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1872[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1873; _temp1873.tag= Cyc_StructFieldDeclList_tok; _temp1873.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1873;});
_temp1872;}); break; case 93: _LL1871: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1875=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1875[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1876; _temp1876.tag= Cyc_StructFieldDeclListList_tok; _temp1876.f1=({
struct Cyc_List_List* _temp1877=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1877->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1877->tl= 0;
_temp1877;}); _temp1876;}); _temp1875;}); break; case 94: _LL1874: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1879=( struct Cyc_StructFieldDeclListList_tok_struct*)
_cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1879[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1880; _temp1880.tag= Cyc_StructFieldDeclListList_tok;
_temp1880.f1=({ struct Cyc_List_List* _temp1881=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1881->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1881->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1881;}); _temp1880;}); _temp1879;}); break;
case 95: _LL1878: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1883=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1883[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1884; _temp1884.tag=
Cyc_InitDeclList_tok; _temp1884.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1884;});
_temp1883;}); break; case 96: _LL1882: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1886=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1886[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1887; _temp1887.tag=
Cyc_InitDeclList_tok; _temp1887.f1=({ struct Cyc_List_List* _temp1888=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1888->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1888->tl= 0; _temp1888;}); _temp1887;}); _temp1886;});
break; case 97: _LL1885: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1890=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1890[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1891; _temp1891.tag=
Cyc_InitDeclList_tok; _temp1891.f1=({ struct Cyc_List_List* _temp1892=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1892->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1892->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1892;});
_temp1891;}); _temp1890;}); break; case 98: _LL1889: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1894=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1894[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1895; _temp1895.tag= Cyc_InitDecl_tok;
_temp1895.f1=({ struct _tuple14* _temp1896=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1896->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1896->f2= 0;
_temp1896;}); _temp1895;}); _temp1894;}); break; case 99: _LL1893: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1898=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1898[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1899; _temp1899.tag= Cyc_InitDecl_tok; _temp1899.f1=({ struct _tuple14*
_temp1900=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1900->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1900->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1900;});
_temp1899;}); _temp1898;}); break; case 100: _LL1897: { struct _tuple17
_temp1904; struct Cyc_List_List* _temp1905; struct Cyc_List_List* _temp1907;
struct Cyc_Absyn_Tqual _temp1909; struct _tuple17* _temp1902= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1904=*
_temp1902; _LL1910: _temp1909= _temp1904.f1; goto _LL1908; _LL1908: _temp1907=
_temp1904.f2; goto _LL1906; _LL1906: _temp1905= _temp1904.f3; goto _LL1903;
_LL1903: { struct Cyc_List_List* _temp1913; struct Cyc_List_List* _temp1915;
struct _tuple0 _temp1911=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1916: _temp1915= _temp1911.f1; goto _LL1914; _LL1914:
_temp1913= _temp1911.f2; goto _LL1912; _LL1912: { void* _temp1917= Cyc_Parse_speclist2typ(
_temp1907, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1918=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1909, _temp1917, _temp1915, _temp1905), _temp1913);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1919=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1919[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1920; _temp1920.tag=
Cyc_StructFieldDeclList_tok; _temp1920.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1918); _temp1920;}); _temp1919;}); break;}}}
case 101: _LL1901: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1922=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1922[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1923; _temp1923.tag=
Cyc_QualSpecList_tok; _temp1923.f1=({ struct _tuple17* _temp1924=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1924->f1= Cyc_Absyn_empty_tqual();
_temp1924->f2=({ struct Cyc_List_List* _temp1925=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1925->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1925->tl=
0; _temp1925;}); _temp1924->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1924;}); _temp1923;});
_temp1922;}); break; case 102: _LL1921: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1927=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1927[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1928; _temp1928.tag=
Cyc_QualSpecList_tok; _temp1928.f1=({ struct _tuple17* _temp1929=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1929->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1929->f2=({
struct Cyc_List_List* _temp1930=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1930->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1930->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1930;}); _temp1929->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1929;});
_temp1928;}); _temp1927;}); break; case 103: _LL1926: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1932=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1932[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1933; _temp1933.tag= Cyc_QualSpecList_tok;
_temp1933.f1=({ struct _tuple17* _temp1934=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1934->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1934->f2= 0;
_temp1934->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1934;}); _temp1933;}); _temp1932;}); break; case
104: _LL1931: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1936=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1936[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1937; _temp1937.tag=
Cyc_QualSpecList_tok; _temp1937.f1=({ struct _tuple17* _temp1938=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1938->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1938->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1938->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1938;});
_temp1937;}); _temp1936;}); break; case 105: _LL1935: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1940=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1940[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1941; _temp1941.tag= Cyc_InitDeclList_tok;
_temp1941.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1941;}); _temp1940;}); break; case 106: _LL1939: yyval=(
void*)({ struct Cyc_InitDeclList_tok_struct* _temp1943=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1943[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1944; _temp1944.tag= Cyc_InitDeclList_tok;
_temp1944.f1=({ struct Cyc_List_List* _temp1945=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1945->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1945->tl= 0;
_temp1945;}); _temp1944;}); _temp1943;}); break; case 107: _LL1942: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp1947=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1947[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1948; _temp1948.tag= Cyc_InitDeclList_tok;
_temp1948.f1=({ struct Cyc_List_List* _temp1949=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1949->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1949->tl=
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1949;}); _temp1948;}); _temp1947;}); break; case 108:
_LL1946: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp1951=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1951[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1952; _temp1952.tag= Cyc_InitDecl_tok; _temp1952.f1=({ struct _tuple14*
_temp1953=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1953->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1953->f2= 0; _temp1953;}); _temp1952;}); _temp1951;});
break; case 109: _LL1950: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1955=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1955[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1956; _temp1956.tag= Cyc_InitDecl_tok;
_temp1956.f1=({ struct _tuple14* _temp1957=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1957->f1=({ struct Cyc_Parse_Declarator*
_temp1958=( struct Cyc_Parse_Declarator*) _cycalloc( sizeof( struct Cyc_Parse_Declarator));
_temp1958->id=({ struct _tuple1* _temp1959=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp1959->f1= Cyc_Absyn_rel_ns_null; _temp1959->f2=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1959;}); _temp1958->tms= 0; _temp1958;});
_temp1957->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1957;}); _temp1956;});
_temp1955;}); break; case 110: _LL1954: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1961=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1961[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1962; _temp1962.tag= Cyc_InitDecl_tok;
_temp1962.f1=({ struct _tuple14* _temp1963=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1963->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1963->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1963;}); _temp1962;}); _temp1961;}); break; case
111: _LL1960: { struct Cyc_List_List* _temp1965=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1966=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1966[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1967; _temp1967.tag=
Cyc_TypeSpecifier_tok; _temp1967.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1968=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1968[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1969; _temp1969.tag= Cyc_Parse_Decl_spec;
_temp1969.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1965,({
struct Cyc_Core_Opt* _temp1970=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1970->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1970;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1969;}); _temp1968;})); _temp1967;}); _temp1966;}); break;} case 112:
_LL1964: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1972=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1972[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1973; _temp1973.tag=
Cyc_TypeSpecifier_tok; _temp1973.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1974=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1974[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1975; _temp1975.tag= Cyc_Absyn_TunionType;
_temp1975.f1=({ struct Cyc_Absyn_TunionInfo _temp1976; _temp1976.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1977=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1977[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1978; _temp1978.tag= Cyc_Absyn_UnknownTunion;
_temp1978.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1979; _temp1979.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1979.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1979;});
_temp1978;}); _temp1977;})); _temp1976.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1976.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1976;}); _temp1975;}); _temp1974;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1973;}); _temp1972;}); break; case 113: _LL1971: { void* _temp1981= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp1990=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1990->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp1990;}),
0); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1982=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1982[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1983; _temp1983.tag= Cyc_TypeSpecifier_tok;
_temp1983.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1984=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1984[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1985; _temp1985.tag=
Cyc_Absyn_TunionType; _temp1985.f1=({ struct Cyc_Absyn_TunionInfo _temp1986;
_temp1986.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1987=( struct Cyc_Absyn_UnknownTunion_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1987[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1988; _temp1988.tag= Cyc_Absyn_UnknownTunion; _temp1988.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1989; _temp1989.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1989.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1989;}); _temp1988;}); _temp1987;})); _temp1986.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1986.rgn=(
void*) _temp1981; _temp1986;}); _temp1985;}); _temp1984;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1983;}); _temp1982;}); break;} case 114: _LL1980: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1992=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1992[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1993; _temp1993.tag= Cyc_TypeSpecifier_tok;
_temp1993.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1994=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1994[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1995; _temp1995.tag= Cyc_Absyn_TunionFieldType; _temp1995.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1996; _temp1996.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp1997=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp1997[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp1998; _temp1998.tag= Cyc_Absyn_UnknownTunionfield; _temp1998.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp1999; _temp1999.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1999.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1999.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1999;});
_temp1998;}); _temp1997;})); _temp1996.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1996;}); _temp1995;});
_temp1994;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1993;}); _temp1992;}); break; case 115:
_LL1991: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2001=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp2001[ 0]=({ struct
Cyc_Bool_tok_struct _temp2002; _temp2002.tag= Cyc_Bool_tok; _temp2002.f1= 0;
_temp2002;}); _temp2001;}); break; case 116: _LL2000: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2004=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2004[ 0]=({ struct Cyc_Bool_tok_struct
_temp2005; _temp2005.tag= Cyc_Bool_tok; _temp2005.f1= 1; _temp2005;}); _temp2004;});
break; case 117: _LL2003: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2007=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2007[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2008; _temp2008.tag= Cyc_TunionFieldList_tok; _temp2008.f1=({ struct Cyc_List_List*
_temp2009=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2009->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2009->tl= 0;
_temp2009;}); _temp2008;}); _temp2007;}); break; case 118: _LL2006: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2011=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2011[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2012; _temp2012.tag= Cyc_TunionFieldList_tok;
_temp2012.f1=({ struct Cyc_List_List* _temp2013=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2013->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2013->tl=
0; _temp2013;}); _temp2012;}); _temp2011;}); break; case 119: _LL2010: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2015=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2015[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2016; _temp2016.tag= Cyc_TunionFieldList_tok;
_temp2016.f1=({ struct Cyc_List_List* _temp2017=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2017->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2017->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2017;}); _temp2016;}); _temp2015;}); break; case 120:
_LL2014: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2019=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2019[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2020; _temp2020.tag=
Cyc_TunionFieldList_tok; _temp2020.f1=({ struct Cyc_List_List* _temp2021=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2021->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2021->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2021;}); _temp2020;});
_temp2019;}); break; case 121: _LL2018: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2023=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2023[ 0]=({ struct Cyc_Scope_tok_struct _temp2024; _temp2024.tag= Cyc_Scope_tok;
_temp2024.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2024;}); _temp2023;});
break; case 122: _LL2022: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2026=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2026[ 0]=({ struct Cyc_Scope_tok_struct _temp2027; _temp2027.tag= Cyc_Scope_tok;
_temp2027.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2027;}); _temp2026;});
break; case 123: _LL2025: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2029=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2029[ 0]=({ struct Cyc_Scope_tok_struct _temp2030; _temp2030.tag= Cyc_Scope_tok;
_temp2030.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2030;}); _temp2029;});
break; case 124: _LL2028: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2032=( struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2032[ 0]=({ struct Cyc_TunionField_tok_struct _temp2033; _temp2033.tag= Cyc_TunionField_tok;
_temp2033.f1=({ struct Cyc_Absyn_Tunionfield* _temp2034=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2034->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2034->tvs= 0;
_temp2034->typs= 0; _temp2034->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2034->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2034;}); _temp2033;}); _temp2032;}); break; case 125:
_LL2031: { struct Cyc_List_List* _temp2036=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp2037=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp2038=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2038[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2039; _temp2039.tag= Cyc_TunionField_tok;
_temp2039.f1=({ struct Cyc_Absyn_Tunionfield* _temp2040=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2040->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2040->tvs=
_temp2037; _temp2040->typs= _temp2036; _temp2040->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2040->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp2040;}); _temp2039;}); _temp2038;}); break;}
case 126: _LL2035: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 127: _LL2041: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2043=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2043[ 0]=({ struct Cyc_Declarator_tok_struct _temp2044; _temp2044.tag= Cyc_Declarator_tok;
_temp2044.f1=({ struct Cyc_Parse_Declarator* _temp2045=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2045->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2045->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2045;});
_temp2044;}); _temp2043;}); break; case 128: _LL2042: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2047=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2047[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2048; _temp2048.tag= Cyc_Declarator_tok;
_temp2048.f1=({ struct Cyc_Parse_Declarator* _temp2049=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2049->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2049->tms= 0;
_temp2049;}); _temp2048;}); _temp2047;}); break; case 129: _LL2046: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 130:
_LL2050: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2052=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2052[ 0]=({ struct Cyc_Declarator_tok_struct _temp2053; _temp2053.tag= Cyc_Declarator_tok;
_temp2053.f1=({ struct Cyc_Parse_Declarator* _temp2054=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2054->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp2054->tms=({ struct Cyc_List_List* _temp2055=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2055->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2055->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp2055;}); _temp2054;}); _temp2053;});
_temp2052;}); break; case 131: _LL2051: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2057=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2057[ 0]=({ struct Cyc_Declarator_tok_struct _temp2058; _temp2058.tag= Cyc_Declarator_tok;
_temp2058.f1=({ struct Cyc_Parse_Declarator* _temp2059=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2059->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2059->tms=({ struct Cyc_List_List* _temp2060=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2060->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2061=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2061[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2062; _temp2062.tag= Cyc_Absyn_ConstArray_mod;
_temp2062.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2062;}); _temp2061;})); _temp2060->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2060;}); _temp2059;}); _temp2058;}); _temp2057;}); break; case 132: _LL2056: {
struct _tuple16 _temp2066; struct Cyc_List_List* _temp2067; struct Cyc_Core_Opt*
_temp2069; struct Cyc_Absyn_VarargInfo* _temp2071; int _temp2073; struct Cyc_List_List*
_temp2075; struct _tuple16* _temp2064= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2066=*
_temp2064; _LL2076: _temp2075= _temp2066.f1; goto _LL2074; _LL2074: _temp2073=
_temp2066.f2; goto _LL2072; _LL2072: _temp2071= _temp2066.f3; goto _LL2070;
_LL2070: _temp2069= _temp2066.f4; goto _LL2068; _LL2068: _temp2067= _temp2066.f5;
goto _LL2065; _LL2065: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2077=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2077[ 0]=({ struct Cyc_Declarator_tok_struct _temp2078; _temp2078.tag= Cyc_Declarator_tok;
_temp2078.f1=({ struct Cyc_Parse_Declarator* _temp2079=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2079->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2079->tms=({ struct Cyc_List_List* _temp2080=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2080->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2081=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2081[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2082; _temp2082.tag= Cyc_Absyn_Function_mod;
_temp2082.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2083=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2083[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2084; _temp2084.tag= Cyc_Absyn_WithTypes;
_temp2084.f1= _temp2075; _temp2084.f2= _temp2073; _temp2084.f3= _temp2071;
_temp2084.f4= _temp2069; _temp2084.f5= _temp2067; _temp2084;}); _temp2083;}));
_temp2082;}); _temp2081;})); _temp2080->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2080;});
_temp2079;}); _temp2078;}); _temp2077;}); break;} case 133: _LL2063: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2086=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2086[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2087; _temp2087.tag= Cyc_Declarator_tok;
_temp2087.f1=({ struct Cyc_Parse_Declarator* _temp2088=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2088->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2088->tms=({ struct Cyc_List_List* _temp2089=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2089->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2090=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2090[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2091; _temp2091.tag= Cyc_Absyn_Function_mod;
_temp2091.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2092=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2092[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2093; _temp2093.tag= Cyc_Absyn_WithTypes;
_temp2093.f1= 0; _temp2093.f2= 0; _temp2093.f3= 0; _temp2093.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2093.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2093;}); _temp2092;})); _temp2091;}); _temp2090;}));
_temp2089->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2089;}); _temp2088;}); _temp2087;});
_temp2086;}); break; case 134: _LL2085: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2095=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2095[ 0]=({ struct Cyc_Declarator_tok_struct _temp2096; _temp2096.tag= Cyc_Declarator_tok;
_temp2096.f1=({ struct Cyc_Parse_Declarator* _temp2097=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2097->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2097->tms=({ struct Cyc_List_List* _temp2098=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2098->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2099=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2099[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2100; _temp2100.tag= Cyc_Absyn_Function_mod;
_temp2100.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2101=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2101[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2102; _temp2102.tag= Cyc_Absyn_NoTypes;
_temp2102.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2102.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2102;});
_temp2101;})); _temp2100;}); _temp2099;})); _temp2098->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2098;}); _temp2097;}); _temp2096;}); _temp2095;}); break; case 135: _LL2094: {
struct Cyc_List_List* _temp2104=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2105=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2105[ 0]=({ struct Cyc_Declarator_tok_struct _temp2106; _temp2106.tag= Cyc_Declarator_tok;
_temp2106.f1=({ struct Cyc_Parse_Declarator* _temp2107=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2107->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2107->tms=({ struct Cyc_List_List* _temp2108=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2108->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2109=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2109[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2110; _temp2110.tag= Cyc_Absyn_TypeParams_mod;
_temp2110.f1= _temp2104; _temp2110.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2110.f3=
0; _temp2110;}); _temp2109;})); _temp2108->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2108;});
_temp2107;}); _temp2106;}); _temp2105;}); break;} case 136: _LL2103:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2112=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2113=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2113[ 0]=({ struct Cyc_Declarator_tok_struct _temp2114; _temp2114.tag= Cyc_Declarator_tok;
_temp2114.f1=({ struct Cyc_Parse_Declarator* _temp2115=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2115->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2115->tms=({ struct Cyc_List_List* _temp2116=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2116->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2117=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2117[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2118; _temp2118.tag= Cyc_Absyn_TypeParams_mod;
_temp2118.f1= _temp2112; _temp2118.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2118.f3=
0; _temp2118;}); _temp2117;})); _temp2116->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2116;});
_temp2115;}); _temp2114;}); _temp2113;}); break;} case 137: _LL2111: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2120=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2120[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2121; _temp2121.tag= Cyc_Declarator_tok;
_temp2121.f1=({ struct Cyc_Parse_Declarator* _temp2122=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2122->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2122->tms=({ struct Cyc_List_List* _temp2123=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2123->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2124=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2124[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2125; _temp2125.tag= Cyc_Absyn_Attributes_mod;
_temp2125.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2125.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2125;}); _temp2124;}));
_temp2123->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]))->tms; _temp2123;}); _temp2122;}); _temp2121;});
_temp2120;}); break; case 138: _LL2119: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2127=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2127[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2128; _temp2128.tag= Cyc_Declarator_tok;
_temp2128.f1=({ struct Cyc_Parse_Declarator* _temp2129=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2129->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2129->tms= 0;
_temp2129;}); _temp2128;}); _temp2127;}); break; case 139: _LL2126: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2131=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2131[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2132; _temp2132.tag= Cyc_Declarator_tok;
_temp2132.f1=({ struct Cyc_Parse_Declarator* _temp2133=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2133->id= Cyc_Absyn_exn_name;
_temp2133->tms= 0; _temp2133;}); _temp2132;}); _temp2131;}); break; case 140:
_LL2130: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2135=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2135[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2136; _temp2136.tag=
Cyc_TypeModifierList_tok; _temp2136.f1=({ struct Cyc_List_List* _temp2137=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2137->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2138=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2138[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2139; _temp2139.tag= Cyc_Absyn_Pointer_mod;
_temp2139.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2139.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2139.f3= Cyc_Absyn_empty_tqual(); _temp2139;});
_temp2138;})); _temp2137->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2137;});
_temp2136;}); _temp2135;}); break; case 141: _LL2134: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2141=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2141[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2142; _temp2142.tag= Cyc_TypeModifierList_tok;
_temp2142.f1=({ struct Cyc_List_List* _temp2143=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2143->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2144=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2144[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2145; _temp2145.tag= Cyc_Absyn_Pointer_mod;
_temp2145.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2145.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2145.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2145;}); _temp2144;}));
_temp2143->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0);
_temp2143;}); _temp2142;}); _temp2141;}); break; case 142: _LL2140: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2147=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2147[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2148; _temp2148.tag= Cyc_TypeModifierList_tok;
_temp2148.f1=({ struct Cyc_List_List* _temp2149=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2149->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2150=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2150[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2151; _temp2151.tag= Cyc_Absyn_Pointer_mod;
_temp2151.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2151.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2151.f3= Cyc_Absyn_empty_tqual(); _temp2151;});
_temp2150;})); _temp2149->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2149;});
_temp2148;}); _temp2147;}); break; case 143: _LL2146: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2153=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2153[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2154; _temp2154.tag= Cyc_TypeModifierList_tok;
_temp2154.f1=({ struct Cyc_List_List* _temp2155=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2155->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2156=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2156[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2157; _temp2157.tag= Cyc_Absyn_Pointer_mod;
_temp2157.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2157.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2157.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2157;});
_temp2156;})); _temp2155->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2155;});
_temp2154;}); _temp2153;}); break; case 144: _LL2152: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2159=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2159[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2160; _temp2160.tag= Cyc_Pointer_Sort_tok;
_temp2160.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2161=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2161[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2162; _temp2162.tag=
Cyc_Absyn_Nullable_ps; _temp2162.f1= Cyc_Absyn_exp_unsigned_one; _temp2162;});
_temp2161;})); _temp2160;}); _temp2159;}); break; case 145: _LL2158: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2164=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2164[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2165; _temp2165.tag= Cyc_Pointer_Sort_tok;
_temp2165.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2166=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2166[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2167; _temp2167.tag= Cyc_Absyn_NonNullable_ps; _temp2167.f1= Cyc_Absyn_exp_unsigned_one;
_temp2167;}); _temp2166;})); _temp2165;}); _temp2164;}); break; case 146:
_LL2163: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2169=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2169[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2170; _temp2170.tag=
Cyc_Pointer_Sort_tok; _temp2170.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2171=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2171[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2172; _temp2172.tag=
Cyc_Absyn_Nullable_ps; _temp2172.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2172;});
_temp2171;})); _temp2170;}); _temp2169;}); break; case 147: _LL2168: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2174=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2174[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2175; _temp2175.tag= Cyc_Pointer_Sort_tok;
_temp2175.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2176=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2176[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2177; _temp2177.tag= Cyc_Absyn_NonNullable_ps; _temp2177.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2177;});
_temp2176;})); _temp2175;}); _temp2174;}); break; case 148: _LL2173: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2179=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2179[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2180; _temp2180.tag= Cyc_Pointer_Sort_tok;
_temp2180.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2180;});
_temp2179;}); break; case 149: _LL2178: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2182=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2182[ 0]=({ struct Cyc_Type_tok_struct _temp2183; _temp2183.tag= Cyc_Type_tok;
_temp2183.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2184=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2184->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2184;}), 0); _temp2183;}); _temp2182;});
break; case 150: _LL2181: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 151: _LL2185: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
152: _LL2186: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2188=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2188[
0]=({ struct Cyc_Type_tok_struct _temp2189; _temp2189.tag= Cyc_Type_tok;
_temp2189.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2190=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2190->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2190;}), 0); _temp2189;}); _temp2188;});
break; case 153: _LL2187: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 154: _LL2191: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2193=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2193[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2194; _temp2194.tag= Cyc_TypeQual_tok; _temp2194.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2194;});
_temp2193;}); break; case 155: _LL2192: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2196=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2196[ 0]=({ struct Cyc_YY1_struct _temp2197; _temp2197.tag= Cyc_YY1;
_temp2197.f1=({ struct _tuple16* _temp2198=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2198->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2198->f2= 0;
_temp2198->f3= 0; _temp2198->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2198->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2198;});
_temp2197;}); _temp2196;}); break; case 156: _LL2195: yyval=( void*)({ struct
Cyc_YY1_struct* _temp2200=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct
Cyc_YY1_struct)); _temp2200[ 0]=({ struct Cyc_YY1_struct _temp2201; _temp2201.tag=
Cyc_YY1; _temp2201.f1=({ struct _tuple16* _temp2202=( struct _tuple16*)
_cycalloc( sizeof( struct _tuple16)); _temp2202->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2202->f2= 1;
_temp2202->f3= 0; _temp2202->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2202->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2202;});
_temp2201;}); _temp2200;}); break; case 157: _LL2199: { struct _tuple2 _temp2206;
void* _temp2207; struct Cyc_Absyn_Tqual _temp2209; struct Cyc_Core_Opt*
_temp2211; struct _tuple2* _temp2204= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2206=*
_temp2204; _LL2212: _temp2211= _temp2206.f1; goto _LL2210; _LL2210: _temp2209=
_temp2206.f2; goto _LL2208; _LL2208: _temp2207= _temp2206.f3; goto _LL2205;
_LL2205: { struct Cyc_Absyn_VarargInfo* _temp2213=({ struct Cyc_Absyn_VarargInfo*
_temp2217=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2217->name= _temp2211; _temp2217->tq= _temp2209; _temp2217->type=( void*)
_temp2207; _temp2217->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2217;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2214=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2214[ 0]=({ struct Cyc_YY1_struct
_temp2215; _temp2215.tag= Cyc_YY1; _temp2215.f1=({ struct _tuple16* _temp2216=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2216->f1= 0;
_temp2216->f2= 0; _temp2216->f3= _temp2213; _temp2216->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2216->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2216;}); _temp2215;}); _temp2214;}); break;}} case 158:
_LL2203: { struct _tuple2 _temp2221; void* _temp2222; struct Cyc_Absyn_Tqual
_temp2224; struct Cyc_Core_Opt* _temp2226; struct _tuple2* _temp2219= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2221=*
_temp2219; _LL2227: _temp2226= _temp2221.f1; goto _LL2225; _LL2225: _temp2224=
_temp2221.f2; goto _LL2223; _LL2223: _temp2222= _temp2221.f3; goto _LL2220;
_LL2220: { struct Cyc_Absyn_VarargInfo* _temp2228=({ struct Cyc_Absyn_VarargInfo*
_temp2232=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2232->name= _temp2226; _temp2232->tq= _temp2224; _temp2232->type=( void*)
_temp2222; _temp2232->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2232;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2229=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2229[ 0]=({ struct Cyc_YY1_struct
_temp2230; _temp2230.tag= Cyc_YY1; _temp2230.f1=({ struct _tuple16* _temp2231=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2231->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2231->f2=
0; _temp2231->f3= _temp2228; _temp2231->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2231->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2231;});
_temp2230;}); _temp2229;}); break;}} case 159: _LL2218: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2234=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2234[ 0]=({ struct Cyc_Type_tok_struct
_temp2235; _temp2235.tag= Cyc_Type_tok; _temp2235.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*)({ struct Cyc_Absyn_Unknown_kb_struct* _temp2236=( struct Cyc_Absyn_Unknown_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct)); _temp2236[ 0]=({ struct
Cyc_Absyn_Unknown_kb_struct _temp2237; _temp2237.tag= Cyc_Absyn_Unknown_kb;
_temp2237.f1= 0; _temp2237;}); _temp2236;})); _temp2235;}); _temp2234;}); break;
case 160: _LL2233: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2239=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2239[ 0]=({ struct Cyc_Type_tok_struct _temp2240; _temp2240.tag= Cyc_Type_tok;
_temp2240.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2241=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2241[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2242; _temp2242.tag= Cyc_Absyn_Eq_kb; _temp2242.f1=( void*) Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2242;});
_temp2241;})); _temp2240;}); _temp2239;}); break; case 161: _LL2238: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2244=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2244[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2245; _temp2245.tag= Cyc_TypeOpt_tok; _temp2245.f1= 0; _temp2245;});
_temp2244;}); break; case 162: _LL2243: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2247=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2247[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2248; _temp2248.tag= Cyc_TypeOpt_tok;
_temp2248.f1=({ struct Cyc_Core_Opt* _temp2249=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2249->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2250=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2250[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2251; _temp2251.tag= Cyc_Absyn_JoinEff;
_temp2251.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2251;}); _temp2250;})); _temp2249;}); _temp2248;});
_temp2247;}); break; case 163: _LL2246: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2253=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2253[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2254; _temp2254.tag= Cyc_Rgnorder_tok;
_temp2254.f1= 0; _temp2254;}); _temp2253;}); break; case 164: _LL2252: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 165:
_LL2255: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2257=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2257[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2258; _temp2258.tag= Cyc_Rgnorder_tok; _temp2258.f1=({ struct Cyc_List_List*
_temp2259=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2259->hd=( void*)({ struct _tuple18* _temp2260=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2260->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2263=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2263[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2264; _temp2264.tag= Cyc_Absyn_Eq_kb; _temp2264.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2264;}); _temp2263;})); _temp2260->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2261=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2261[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2262; _temp2262.tag= Cyc_Absyn_Eq_kb; _temp2262.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2262;}); _temp2261;})); _temp2260;}); _temp2259->tl= 0; _temp2259;});
_temp2258;}); _temp2257;}); break; case 166: _LL2256: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2266=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2266[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2267; _temp2267.tag= Cyc_Rgnorder_tok; _temp2267.f1=({ struct Cyc_List_List*
_temp2268=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2268->hd=( void*)({ struct _tuple18* _temp2269=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2269->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2272=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2272[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2273; _temp2273.tag= Cyc_Absyn_Eq_kb; _temp2273.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2273;}); _temp2272;})); _temp2269->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2270=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2270[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2271; _temp2271.tag= Cyc_Absyn_Eq_kb; _temp2271.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2271;}); _temp2270;})); _temp2269;}); _temp2268->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2268;});
_temp2267;}); _temp2266;}); break; case 167: _LL2265: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2275=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2275[ 0]=({ struct Cyc_Bool_tok_struct
_temp2276; _temp2276.tag= Cyc_Bool_tok; _temp2276.f1= 0; _temp2276;}); _temp2275;});
break; case 168: _LL2274: { struct _tagged_arr _temp2278= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Std_zstrcmp(
_temp2278, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2279=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2279[ 0]=({ struct Cyc_Bool_tok_struct _temp2280; _temp2280.tag= Cyc_Bool_tok;
_temp2280.f1= 1; _temp2280;}); _temp2279;}); break;} case 169: _LL2277: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 170:
_LL2281: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2283=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2283[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2284; _temp2284.tag= Cyc_TypeList_tok; _temp2284.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2284;});
_temp2283;}); break; case 171: _LL2282: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2286=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2286[ 0]=({ struct Cyc_TypeList_tok_struct _temp2287; _temp2287.tag= Cyc_TypeList_tok;
_temp2287.f1= 0; _temp2287;}); _temp2286;}); break; case 172: _LL2285: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case
173: _LL2288: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2290=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2290[ 0]=({ struct Cyc_TypeList_tok_struct _temp2291; _temp2291.tag= Cyc_TypeList_tok;
_temp2291.f1=({ struct Cyc_List_List* _temp2292=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2292->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp2293=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp2293[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp2294; _temp2294.tag= Cyc_Absyn_RgnsEff; _temp2294.f1=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2294;}); _temp2293;})); _temp2292->tl= 0; _temp2292;});
_temp2291;}); _temp2290;}); break; case 174: _LL2289: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2296=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2296[ 0]=({ struct Cyc_TypeList_tok_struct _temp2297; _temp2297.tag= Cyc_TypeList_tok;
_temp2297.f1=({ struct Cyc_List_List* _temp2298=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2298->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2298->tl= 0;
_temp2298;}); _temp2297;}); _temp2296;}); break; case 175: _LL2295: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2300=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2300[ 0]=({ struct Cyc_TypeList_tok_struct _temp2301; _temp2301.tag= Cyc_TypeList_tok;
_temp2301.f1=({ struct Cyc_List_List* _temp2302=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2302->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2303=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2303[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2304; _temp2304.tag= Cyc_Absyn_AccessEff;
_temp2304.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2304;}); _temp2303;})); _temp2302->tl= 0;
_temp2302;}); _temp2301;}); _temp2300;}); break; case 176: _LL2299: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),( void*) Cyc_Absyn_RgnKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2306=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2306[ 0]=({ struct Cyc_TypeList_tok_struct _temp2307; _temp2307.tag= Cyc_TypeList_tok;
_temp2307.f1=({ struct Cyc_List_List* _temp2308=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2308->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2309=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2309[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2310; _temp2310.tag= Cyc_Absyn_AccessEff;
_temp2310.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2310;}); _temp2309;})); _temp2308->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2308;});
_temp2307;}); _temp2306;}); break; case 177: _LL2305: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2312=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2312[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2313; _temp2313.tag= Cyc_ParamDeclList_tok;
_temp2313.f1=({ struct Cyc_List_List* _temp2314=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2314->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2314->tl= 0;
_temp2314;}); _temp2313;}); _temp2312;}); break; case 178: _LL2311: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2316=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2316[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2317; _temp2317.tag= Cyc_ParamDeclList_tok;
_temp2317.f1=({ struct Cyc_List_List* _temp2318=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2318->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2318->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2318;}); _temp2317;}); _temp2316;}); break; case 179:
_LL2315: { struct _tuple17 _temp2322; struct Cyc_List_List* _temp2323; struct
Cyc_List_List* _temp2325; struct Cyc_Absyn_Tqual _temp2327; struct _tuple17*
_temp2320= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2322=* _temp2320; _LL2328: _temp2327=
_temp2322.f1; goto _LL2326; _LL2326: _temp2325= _temp2322.f2; goto _LL2324;
_LL2324: _temp2323= _temp2322.f3; goto _LL2321; _LL2321: { struct Cyc_Parse_Declarator
_temp2331; struct Cyc_List_List* _temp2332; struct _tuple1* _temp2334; struct
Cyc_Parse_Declarator* _temp2329= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2331=* _temp2329;
_LL2335: _temp2334= _temp2331.id; goto _LL2333; _LL2333: _temp2332= _temp2331.tms;
goto _LL2330; _LL2330: { void* _temp2336= Cyc_Parse_speclist2typ( _temp2325, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2339; struct Cyc_List_List* _temp2341; void*
_temp2343; struct Cyc_Absyn_Tqual _temp2345; struct _tuple6 _temp2337= Cyc_Parse_apply_tms(
_temp2327, _temp2336, _temp2323, _temp2332); _LL2346: _temp2345= _temp2337.f1;
goto _LL2344; _LL2344: _temp2343= _temp2337.f2; goto _LL2342; _LL2342: _temp2341=
_temp2337.f3; goto _LL2340; _LL2340: _temp2339= _temp2337.f4; goto _LL2338;
_LL2338: if( _temp2341 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2334)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2347=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2351=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2351->v=( void*)(* _temp2334).f2; _temp2351;}); if( _temp2339 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2348=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2348[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2349; _temp2349.tag= Cyc_ParamDecl_tok;
_temp2349.f1=({ struct _tuple2* _temp2350=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2350->f1= _temp2347; _temp2350->f2= _temp2345; _temp2350->f3=
_temp2343; _temp2350;}); _temp2349;}); _temp2348;}); break;}}}} case 180:
_LL2319: { struct _tuple17 _temp2355; struct Cyc_List_List* _temp2356; struct
Cyc_List_List* _temp2358; struct Cyc_Absyn_Tqual _temp2360; struct _tuple17*
_temp2353= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2355=* _temp2353; _LL2361: _temp2360= _temp2355.f1;
goto _LL2359; _LL2359: _temp2358= _temp2355.f2; goto _LL2357; _LL2357: _temp2356=
_temp2355.f3; goto _LL2354; _LL2354: { void* _temp2362= Cyc_Parse_speclist2typ(
_temp2358, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2356 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2363=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2363[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2364; _temp2364.tag= Cyc_ParamDecl_tok; _temp2364.f1=({
struct _tuple2* _temp2365=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2365->f1= 0; _temp2365->f2= _temp2360; _temp2365->f3= _temp2362; _temp2365;});
_temp2364;}); _temp2363;}); break;}} case 181: _LL2352: { struct _tuple17
_temp2369; struct Cyc_List_List* _temp2370; struct Cyc_List_List* _temp2372;
struct Cyc_Absyn_Tqual _temp2374; struct _tuple17* _temp2367= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2369=*
_temp2367; _LL2375: _temp2374= _temp2369.f1; goto _LL2373; _LL2373: _temp2372=
_temp2369.f2; goto _LL2371; _LL2371: _temp2370= _temp2369.f3; goto _LL2368;
_LL2368: { void* _temp2376= Cyc_Parse_speclist2typ( _temp2372, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2377=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2380; struct Cyc_List_List* _temp2382; void* _temp2384; struct Cyc_Absyn_Tqual
_temp2386; struct _tuple6 _temp2378= Cyc_Parse_apply_tms( _temp2374, _temp2376,
_temp2370, _temp2377); _LL2387: _temp2386= _temp2378.f1; goto _LL2385; _LL2385:
_temp2384= _temp2378.f2; goto _LL2383; _LL2383: _temp2382= _temp2378.f3; goto
_LL2381; _LL2381: _temp2380= _temp2378.f4; goto _LL2379; _LL2379: if( _temp2382
!=  0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2380 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2388=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2388[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2389; _temp2389.tag= Cyc_ParamDecl_tok; _temp2389.f1=({
struct _tuple2* _temp2390=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2390->f1= 0; _temp2390->f2= _temp2386; _temp2390->f3= _temp2384; _temp2390;});
_temp2389;}); _temp2388;}); break;}} case 182: _LL2366: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2392=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2392[ 0]=({ struct Cyc_IdList_tok_struct
_temp2393; _temp2393.tag= Cyc_IdList_tok; _temp2393.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2393;});
_temp2392;}); break; case 183: _LL2391: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2395=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2395[ 0]=({ struct Cyc_IdList_tok_struct _temp2396; _temp2396.tag= Cyc_IdList_tok;
_temp2396.f1=({ struct Cyc_List_List* _temp2397=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2397->hd=( void*)({ struct
_tagged_arr* _temp2398=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2398[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2398;}); _temp2397->tl=
0; _temp2397;}); _temp2396;}); _temp2395;}); break; case 184: _LL2394: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2400=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2400[ 0]=({ struct Cyc_IdList_tok_struct
_temp2401; _temp2401.tag= Cyc_IdList_tok; _temp2401.f1=({ struct Cyc_List_List*
_temp2402=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2402->hd=( void*)({ struct _tagged_arr* _temp2403=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2403[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2403;});
_temp2402->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2402;}); _temp2401;}); _temp2400;}); break; case 185:
_LL2399: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 186: _LL2404: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 187: _LL2405: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2407=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2407[ 0]=({ struct Cyc_Exp_tok_struct _temp2408; _temp2408.tag= Cyc_Exp_tok;
_temp2408.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2409=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2409[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2410; _temp2410.tag= Cyc_Absyn_UnresolvedMem_e; _temp2410.f1= 0; _temp2410.f2=
0; _temp2410;}); _temp2409;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2408;});
_temp2407;}); break; case 188: _LL2406: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2412=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2412[ 0]=({ struct Cyc_Exp_tok_struct _temp2413; _temp2413.tag= Cyc_Exp_tok;
_temp2413.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2414=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2414[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2415; _temp2415.tag= Cyc_Absyn_UnresolvedMem_e; _temp2415.f1= 0; _temp2415.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2415;});
_temp2414;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2413;}); _temp2412;}); break; case 189:
_LL2411: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2417=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2417[ 0]=({ struct Cyc_Exp_tok_struct
_temp2418; _temp2418.tag= Cyc_Exp_tok; _temp2418.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2419=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2419[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2420; _temp2420.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2420.f1= 0; _temp2420.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2420;});
_temp2419;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2418;}); _temp2417;}); break; case 190:
_LL2416: { struct Cyc_Absyn_Vardecl* _temp2422= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2428=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2428->f1=( void*) Cyc_Absyn_Loc_n; _temp2428->f2=({ struct _tagged_arr*
_temp2429=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp2429[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); _temp2429;}); _temp2428;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp2422->tq=({ struct Cyc_Absyn_Tqual _temp2423; _temp2423.q_const= 1;
_temp2423.q_volatile= 0; _temp2423.q_restrict= 1; _temp2423;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2424=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2424[ 0]=({ struct Cyc_Exp_tok_struct
_temp2425; _temp2425.tag= Cyc_Exp_tok; _temp2425.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2426=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2426[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2427; _temp2427.tag= Cyc_Absyn_Comprehension_e;
_temp2427.f1= _temp2422; _temp2427.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2427.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2427;});
_temp2426;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2425;}); _temp2424;}); break;} case
191: _LL2421: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2431=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2431[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2432; _temp2432.tag=
Cyc_InitializerList_tok; _temp2432.f1=({ struct Cyc_List_List* _temp2433=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2433->hd=(
void*)({ struct _tuple19* _temp2434=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2434->f1= 0; _temp2434->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2434;}); _temp2433->tl=
0; _temp2433;}); _temp2432;}); _temp2431;}); break; case 192: _LL2430: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2436=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2436[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2437; _temp2437.tag= Cyc_InitializerList_tok;
_temp2437.f1=({ struct Cyc_List_List* _temp2438=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2438->hd=( void*)({ struct
_tuple19* _temp2439=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2439->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2439->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2439;});
_temp2438->tl= 0; _temp2438;}); _temp2437;}); _temp2436;}); break; case 193:
_LL2435: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2441=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2441[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2442; _temp2442.tag=
Cyc_InitializerList_tok; _temp2442.f1=({ struct Cyc_List_List* _temp2443=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2443->hd=(
void*)({ struct _tuple19* _temp2444=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2444->f1= 0; _temp2444->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2444;}); _temp2443->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2443;}); _temp2442;}); _temp2441;}); break; case 194:
_LL2440: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2446=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2446[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2447; _temp2447.tag=
Cyc_InitializerList_tok; _temp2447.f1=({ struct Cyc_List_List* _temp2448=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2448->hd=(
void*)({ struct _tuple19* _temp2449=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2449->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2449->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2449;});
_temp2448->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2448;});
_temp2447;}); _temp2446;}); break; case 195: _LL2445: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2451=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2451[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2452; _temp2452.tag= Cyc_DesignatorList_tok;
_temp2452.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2452;});
_temp2451;}); break; case 196: _LL2450: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2454=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2454[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2455; _temp2455.tag=
Cyc_DesignatorList_tok; _temp2455.f1=({ struct Cyc_List_List* _temp2456=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2456->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2456->tl= 0; _temp2456;}); _temp2455;}); _temp2454;});
break; case 197: _LL2453: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2458=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2458[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2459; _temp2459.tag=
Cyc_DesignatorList_tok; _temp2459.f1=({ struct Cyc_List_List* _temp2460=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2460->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2460->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2460;});
_temp2459;}); _temp2458;}); break; case 198: _LL2457: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2462=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2462[ 0]=({ struct
Cyc_Designator_tok_struct _temp2463; _temp2463.tag= Cyc_Designator_tok;
_temp2463.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2464=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2464[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2465; _temp2465.tag=
Cyc_Absyn_ArrayElement; _temp2465.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2465;});
_temp2464;})); _temp2463;}); _temp2462;}); break; case 199: _LL2461: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2467=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2467[ 0]=({ struct
Cyc_Designator_tok_struct _temp2468; _temp2468.tag= Cyc_Designator_tok;
_temp2468.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2469=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2469[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2470; _temp2470.tag= Cyc_Absyn_FieldName;
_temp2470.f1=({ struct _tagged_arr* _temp2471=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2471[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2471;}); _temp2470;});
_temp2469;})); _temp2468;}); _temp2467;}); break; case 200: _LL2466: { void*
_temp2473= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2474=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2474 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2475=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2476=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2476[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2477; _temp2477.tag= Cyc_ParamDecl_tok; _temp2477.f1=({
struct _tuple2* _temp2478=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2478->f1= 0; _temp2478->f2= _temp2475; _temp2478->f3= _temp2473; _temp2478;});
_temp2477;}); _temp2476;}); break;}} case 201: _LL2472: { void* _temp2480= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp2481=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2482=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2483=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2484= Cyc_Parse_apply_tms( _temp2482, _temp2480, _temp2481,
_temp2483); if( _temp2484.f3 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2484.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2485=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2485[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2486; _temp2486.tag= Cyc_ParamDecl_tok; _temp2486.f1=({
struct _tuple2* _temp2487=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2487->f1= 0; _temp2487->f2= _temp2484.f1; _temp2487->f3= _temp2484.f2;
_temp2487;}); _temp2486;}); _temp2485;}); break;} case 202: _LL2479: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2489=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2489[ 0]=({ struct Cyc_Type_tok_struct
_temp2490; _temp2490.tag= Cyc_Type_tok; _temp2490.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2490;});
_temp2489;}); break; case 203: _LL2488: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2492=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2492[ 0]=({ struct Cyc_Type_tok_struct _temp2493; _temp2493.tag= Cyc_Type_tok;
_temp2493.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2494=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2494[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2495; _temp2495.tag= Cyc_Absyn_JoinEff;
_temp2495.f1= 0; _temp2495;}); _temp2494;})); _temp2493;}); _temp2492;}); break;
case 204: _LL2491: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2497=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2497[ 0]=({ struct Cyc_Type_tok_struct _temp2498; _temp2498.tag= Cyc_Type_tok;
_temp2498.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2499=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2499[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2500; _temp2500.tag= Cyc_Absyn_JoinEff;
_temp2500.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2500;}); _temp2499;})); _temp2498;});
_temp2497;}); break; case 205: _LL2496: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2502=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2502[ 0]=({ struct Cyc_Type_tok_struct _temp2503; _temp2503.tag= Cyc_Type_tok;
_temp2503.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2504=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2504[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2505; _temp2505.tag= Cyc_Absyn_RgnsEff;
_temp2505.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2505;}); _temp2504;})); _temp2503;});
_temp2502;}); break; case 206: _LL2501: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2507=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2507[ 0]=({ struct Cyc_Type_tok_struct _temp2508; _temp2508.tag= Cyc_Type_tok;
_temp2508.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2509=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2509[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2510; _temp2510.tag= Cyc_Absyn_JoinEff;
_temp2510.f1=({ struct Cyc_List_List* _temp2511=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2511->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2511->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2511;}); _temp2510;}); _temp2509;})); _temp2508;});
_temp2507;}); break; case 207: _LL2506: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2513=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2513[ 0]=({ struct Cyc_TypeList_tok_struct _temp2514; _temp2514.tag= Cyc_TypeList_tok;
_temp2514.f1=({ struct Cyc_List_List* _temp2515=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2515->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2515->tl= 0;
_temp2515;}); _temp2514;}); _temp2513;}); break; case 208: _LL2512: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2517=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2517[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2518; _temp2518.tag= Cyc_TypeList_tok; _temp2518.f1=({ struct Cyc_List_List*
_temp2519=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2519->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2519->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2519;});
_temp2518;}); _temp2517;}); break; case 209: _LL2516: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2521=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2521[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2522; _temp2522.tag= Cyc_AbstractDeclarator_tok;
_temp2522.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2523=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2523->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2523;});
_temp2522;}); _temp2521;}); break; case 210: _LL2520: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 211: _LL2524:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2526=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2526[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2527; _temp2527.tag=
Cyc_AbstractDeclarator_tok; _temp2527.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2528=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2528->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2528;});
_temp2527;}); _temp2526;}); break; case 212: _LL2525: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 213:
_LL2529: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2531=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2531[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2532; _temp2532.tag=
Cyc_AbstractDeclarator_tok; _temp2532.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2533=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2533->tms=({ struct Cyc_List_List* _temp2534=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2534->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2534->tl= 0; _temp2534;}); _temp2533;}); _temp2532;}); _temp2531;}); break;
case 214: _LL2530: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2536=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2536[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2537; _temp2537.tag= Cyc_AbstractDeclarator_tok; _temp2537.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2538=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2538->tms=({ struct Cyc_List_List* _temp2539=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2539->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2539->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2539;});
_temp2538;}); _temp2537;}); _temp2536;}); break; case 215: _LL2535: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2541=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2541[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2542; _temp2542.tag= Cyc_AbstractDeclarator_tok;
_temp2542.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2543=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2543->tms=({
struct Cyc_List_List* _temp2544=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2544->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2545=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2545[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2546; _temp2546.tag= Cyc_Absyn_ConstArray_mod; _temp2546.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2546;});
_temp2545;})); _temp2544->tl= 0; _temp2544;}); _temp2543;}); _temp2542;});
_temp2541;}); break; case 216: _LL2540: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2548=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2548[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2549; _temp2549.tag= Cyc_AbstractDeclarator_tok; _temp2549.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2550=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2550->tms=({ struct Cyc_List_List* _temp2551=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2551->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2552=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2552[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2553; _temp2553.tag= Cyc_Absyn_ConstArray_mod;
_temp2553.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2553;}); _temp2552;})); _temp2551->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2551;}); _temp2550;}); _temp2549;}); _temp2548;}); break; case 217: _LL2547:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2555=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2555[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2556; _temp2556.tag=
Cyc_AbstractDeclarator_tok; _temp2556.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2557=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2557->tms=({ struct Cyc_List_List* _temp2558=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2558->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2559=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2559[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2560; _temp2560.tag= Cyc_Absyn_Function_mod;
_temp2560.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2561=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2561[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2562; _temp2562.tag= Cyc_Absyn_WithTypes;
_temp2562.f1= 0; _temp2562.f2= 0; _temp2562.f3= 0; _temp2562.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2562.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2562;}); _temp2561;})); _temp2560;}); _temp2559;}));
_temp2558->tl= 0; _temp2558;}); _temp2557;}); _temp2556;}); _temp2555;}); break;
case 218: _LL2554: { struct _tuple16 _temp2566; struct Cyc_List_List* _temp2567;
struct Cyc_Core_Opt* _temp2569; struct Cyc_Absyn_VarargInfo* _temp2571; int
_temp2573; struct Cyc_List_List* _temp2575; struct _tuple16* _temp2564= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2566=*
_temp2564; _LL2576: _temp2575= _temp2566.f1; goto _LL2574; _LL2574: _temp2573=
_temp2566.f2; goto _LL2572; _LL2572: _temp2571= _temp2566.f3; goto _LL2570;
_LL2570: _temp2569= _temp2566.f4; goto _LL2568; _LL2568: _temp2567= _temp2566.f5;
goto _LL2565; _LL2565: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2577=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2577[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2578; _temp2578.tag= Cyc_AbstractDeclarator_tok; _temp2578.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2579=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2579->tms=({ struct Cyc_List_List* _temp2580=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2580->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2581=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2581[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2582; _temp2582.tag= Cyc_Absyn_Function_mod;
_temp2582.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2583=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2583[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2584; _temp2584.tag= Cyc_Absyn_WithTypes;
_temp2584.f1= _temp2575; _temp2584.f2= _temp2573; _temp2584.f3= _temp2571;
_temp2584.f4= _temp2569; _temp2584.f5= _temp2567; _temp2584;}); _temp2583;}));
_temp2582;}); _temp2581;})); _temp2580->tl= 0; _temp2580;}); _temp2579;});
_temp2578;}); _temp2577;}); break;} case 219: _LL2563: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2586=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2586[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2587; _temp2587.tag= Cyc_AbstractDeclarator_tok;
_temp2587.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2588=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2588->tms=({
struct Cyc_List_List* _temp2589=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2589->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2590=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2590[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2591; _temp2591.tag=
Cyc_Absyn_Function_mod; _temp2591.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2592=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2592[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2593; _temp2593.tag= Cyc_Absyn_WithTypes;
_temp2593.f1= 0; _temp2593.f2= 0; _temp2593.f3= 0; _temp2593.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2593.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2593;}); _temp2592;})); _temp2591;}); _temp2590;}));
_temp2589->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2589;});
_temp2588;}); _temp2587;}); _temp2586;}); break; case 220: _LL2585: { struct
_tuple16 _temp2597; struct Cyc_List_List* _temp2598; struct Cyc_Core_Opt*
_temp2600; struct Cyc_Absyn_VarargInfo* _temp2602; int _temp2604; struct Cyc_List_List*
_temp2606; struct _tuple16* _temp2595= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2597=*
_temp2595; _LL2607: _temp2606= _temp2597.f1; goto _LL2605; _LL2605: _temp2604=
_temp2597.f2; goto _LL2603; _LL2603: _temp2602= _temp2597.f3; goto _LL2601;
_LL2601: _temp2600= _temp2597.f4; goto _LL2599; _LL2599: _temp2598= _temp2597.f5;
goto _LL2596; _LL2596: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2608=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2608[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2609; _temp2609.tag= Cyc_AbstractDeclarator_tok; _temp2609.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2610=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2610->tms=({ struct Cyc_List_List* _temp2611=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2611->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2612=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2612[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2613; _temp2613.tag= Cyc_Absyn_Function_mod;
_temp2613.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2614=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2614[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2615; _temp2615.tag= Cyc_Absyn_WithTypes;
_temp2615.f1= _temp2606; _temp2615.f2= _temp2604; _temp2615.f3= _temp2602;
_temp2615.f4= _temp2600; _temp2615.f5= _temp2598; _temp2615;}); _temp2614;}));
_temp2613;}); _temp2612;})); _temp2611->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2611;}); _temp2610;}); _temp2609;}); _temp2608;}); break;} case 221:
_LL2594: { struct Cyc_List_List* _temp2617=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2618=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2618[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2619; _temp2619.tag= Cyc_AbstractDeclarator_tok; _temp2619.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2620=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2620->tms=({ struct Cyc_List_List* _temp2621=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2621->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2622=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2622[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2623; _temp2623.tag= Cyc_Absyn_TypeParams_mod;
_temp2623.f1= _temp2617; _temp2623.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2623.f3=
0; _temp2623;}); _temp2622;})); _temp2621->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2621;}); _temp2620;}); _temp2619;}); _temp2618;}); break;} case 222:
_LL2616:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2625=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2626=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2626[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2627; _temp2627.tag= Cyc_AbstractDeclarator_tok; _temp2627.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2628=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2628->tms=({ struct Cyc_List_List* _temp2629=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2629->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2630=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2630[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2631; _temp2631.tag= Cyc_Absyn_TypeParams_mod;
_temp2631.f1= _temp2625; _temp2631.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2631.f3=
0; _temp2631;}); _temp2630;})); _temp2629->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2629;}); _temp2628;}); _temp2627;}); _temp2626;}); break;} case 223:
_LL2624: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2633=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2633[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2634; _temp2634.tag=
Cyc_AbstractDeclarator_tok; _temp2634.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2635=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2635->tms=({ struct Cyc_List_List* _temp2636=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2636->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2637=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2637[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2638; _temp2638.tag= Cyc_Absyn_Attributes_mod;
_temp2638.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2638.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2638;}); _temp2637;}));
_temp2636->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms; _temp2636;});
_temp2635;}); _temp2634;}); _temp2633;}); break; case 224: _LL2632: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 225: _LL2639:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
226: _LL2640: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 227: _LL2641: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 228: _LL2642: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 229: _LL2643:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
230: _LL2644: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2656=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2656->name=({ struct
_tagged_arr* _temp2659=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2659[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2659;});
_temp2656->identity= 0; _temp2656->kind=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp2657=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp2657[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp2658; _temp2658.tag= Cyc_Absyn_Eq_kb;
_temp2658.f1=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2658;}); _temp2657;}));
_temp2656;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2654=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2654[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2655; _temp2655.tag= Cyc_Absyn_VarType;
_temp2655.f1= tv; _temp2655;}); _temp2654;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2646=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2646[ 0]=({ struct Cyc_Stmt_tok_struct _temp2647; _temp2647.tag= Cyc_Stmt_tok;
_temp2647.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2648=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2648[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2649; _temp2649.tag= Cyc_Absyn_Region_s;
_temp2649.f1= tv; _temp2649.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2650=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2650->f1=(
void*) Cyc_Absyn_Loc_n; _temp2650->f2=({ struct _tagged_arr* _temp2651=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2651[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2651;});
_temp2650;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2652=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2652[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2653; _temp2653.tag=
Cyc_Absyn_RgnHandleType; _temp2653.f1=( void*) t; _temp2653;}); _temp2652;}), 0);
_temp2649.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2649;}); _temp2648;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2647;});
_temp2646;}); break;} case 231: _LL2645: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2671=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2671->name=({ struct
_tagged_arr* _temp2674=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2674[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2676; _temp2676.tag= Cyc_Std_String_pa; _temp2676.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2675[ 1u]={& _temp2676}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2675, sizeof( void*), 1u));}});
_temp2674;}); _temp2671->identity= 0; _temp2671->kind=( void*)(( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2672=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2672[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2673; _temp2673.tag= Cyc_Absyn_Eq_kb; _temp2673.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2673;}); _temp2672;})); _temp2671;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2669=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2669[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2670; _temp2670.tag= Cyc_Absyn_VarType;
_temp2670.f1= tv; _temp2670;}); _temp2669;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2661=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2661[ 0]=({ struct Cyc_Stmt_tok_struct _temp2662; _temp2662.tag= Cyc_Stmt_tok;
_temp2662.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2663=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2663[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2664; _temp2664.tag= Cyc_Absyn_Region_s;
_temp2664.f1= tv; _temp2664.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2665=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2665->f1=(
void*) Cyc_Absyn_Loc_n; _temp2665->f2=({ struct _tagged_arr* _temp2666=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2666[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2666;});
_temp2665;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2667=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2667[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2668; _temp2668.tag=
Cyc_Absyn_RgnHandleType; _temp2668.f1=( void*) t; _temp2668;}); _temp2667;}), 0);
_temp2664.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2664;}); _temp2663;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2662;});
_temp2661;}); break;} case 232: _LL2660: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2678=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2678[ 0]=({ struct Cyc_Stmt_tok_struct _temp2679; _temp2679.tag= Cyc_Stmt_tok;
_temp2679.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2680=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2680[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2681; _temp2681.tag= Cyc_Absyn_Cut_s;
_temp2681.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2681;}); _temp2680;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2679;});
_temp2678;}); break; case 233: _LL2677: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2683=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2683[ 0]=({ struct Cyc_Stmt_tok_struct _temp2684; _temp2684.tag= Cyc_Stmt_tok;
_temp2684.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2685=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2685[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2686; _temp2686.tag= Cyc_Absyn_Splice_s;
_temp2686.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2686;}); _temp2685;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2684;});
_temp2683;}); break; case 234: _LL2682: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2688=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2688[ 0]=({ struct Cyc_Stmt_tok_struct _temp2689; _temp2689.tag= Cyc_Stmt_tok;
_temp2689.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2690=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2690[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2691; _temp2691.tag= Cyc_Absyn_Label_s;
_temp2691.f1=({ struct _tagged_arr* _temp2692=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2692[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2692;});
_temp2691.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2691;}); _temp2690;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2689;});
_temp2688;}); break; case 235: _LL2687: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2694=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2694[ 0]=({ struct Cyc_Stmt_tok_struct _temp2695; _temp2695.tag= Cyc_Stmt_tok;
_temp2695.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2695;});
_temp2694;}); break; case 236: _LL2693: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2697=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2697[ 0]=({ struct Cyc_Stmt_tok_struct _temp2698; _temp2698.tag= Cyc_Stmt_tok;
_temp2698.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2698;}); _temp2697;}); break; case 237: _LL2696: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2700=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2700[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2701; _temp2701.tag= Cyc_Stmt_tok; _temp2701.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2701;}); _temp2700;}); break; case 238: _LL2699: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 239:
_LL2702: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2704=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2704[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2705; _temp2705.tag= Cyc_Stmt_tok; _temp2705.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2705;}); _temp2704;}); break;
case 240: _LL2703: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2707=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2707[ 0]=({ struct Cyc_Stmt_tok_struct _temp2708; _temp2708.tag= Cyc_Stmt_tok;
_temp2708.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2708;});
_temp2707;}); break; case 241: _LL2706: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 242: _LL2709:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2711=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2711[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2712; _temp2712.tag= Cyc_Stmt_tok; _temp2712.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2712;}); _temp2711;}); break; case 243: _LL2710: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2714=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2714[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2715; _temp2715.tag= Cyc_Stmt_tok; _temp2715.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2716=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2716[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2717; _temp2717.tag= Cyc_Absyn_Fn_d;
_temp2717.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2717;}); _temp2716;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2715;}); _temp2714;}); break; case 244: _LL2713: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2719=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2719[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2720; _temp2720.tag= Cyc_Stmt_tok; _temp2720.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2721=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2721[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2722; _temp2722.tag= Cyc_Absyn_Fn_d;
_temp2722.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2722;}); _temp2721;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2720;}); _temp2719;}); break; case 245: _LL2718: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2724=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2724[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2725; _temp2725.tag= Cyc_Stmt_tok; _temp2725.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2725;});
_temp2724;}); break; case 246: _LL2723: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2727=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2727[ 0]=({ struct Cyc_Stmt_tok_struct _temp2728; _temp2728.tag= Cyc_Stmt_tok;
_temp2728.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2728;}); _temp2727;}); break; case 247: _LL2726: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2730=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2730[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2731; _temp2731.tag= Cyc_Stmt_tok; _temp2731.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2731;}); _temp2730;}); break; case 248: _LL2729: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2733=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2733[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2734; _temp2734.tag= Cyc_Stmt_tok; _temp2734.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2735=( struct Cyc_Absyn_SwitchC_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2735[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2736; _temp2736.tag= Cyc_Absyn_SwitchC_s;
_temp2736.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2736.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2736;});
_temp2735;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2734;}); _temp2733;}); break; case 249:
_LL2732: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2738=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2738[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2739; _temp2739.tag= Cyc_Stmt_tok; _temp2739.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2739;});
_temp2738;}); break; case 250: _LL2737: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2741=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2741[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2742; _temp2742.tag= Cyc_SwitchClauseList_tok; _temp2742.f1= 0; _temp2742;});
_temp2741;}); break; case 251: _LL2740: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2744=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2744[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2745; _temp2745.tag= Cyc_SwitchClauseList_tok; _temp2745.f1=({ struct Cyc_List_List*
_temp2746=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2746->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2747=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2747->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2747->pat_vars= 0; _temp2747->where_clause= 0; _temp2747->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2747->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2747;}); _temp2746->tl= 0; _temp2746;});
_temp2745;}); _temp2744;}); break; case 252: _LL2743: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2749=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2749[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2750; _temp2750.tag= Cyc_SwitchClauseList_tok;
_temp2750.f1=({ struct Cyc_List_List* _temp2751=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2751->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2752=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2752->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2752->pat_vars= 0; _temp2752->where_clause= 0;
_temp2752->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2752->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2752;});
_temp2751->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2751;}); _temp2750;});
_temp2749;}); break; case 253: _LL2748: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2754=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2754[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2755; _temp2755.tag= Cyc_SwitchClauseList_tok; _temp2755.f1=({ struct Cyc_List_List*
_temp2756=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2756->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2757=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2757->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2757->pat_vars=
0; _temp2757->where_clause= 0; _temp2757->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2757->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2757;}); _temp2756->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2756;});
_temp2755;}); _temp2754;}); break; case 254: _LL2753: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2759=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2759[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2760; _temp2760.tag= Cyc_SwitchClauseList_tok;
_temp2760.f1=({ struct Cyc_List_List* _temp2761=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2761->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2762=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2762->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp2762->pat_vars= 0; _temp2762->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2762->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2762->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2762;});
_temp2761->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2761;}); _temp2760;});
_temp2759;}); break; case 255: _LL2758: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2764=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2764[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2765; _temp2765.tag= Cyc_SwitchClauseList_tok; _temp2765.f1=({ struct Cyc_List_List*
_temp2766=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2766->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2767=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2767->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2767->pat_vars=
0; _temp2767->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2767->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2767->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2767;});
_temp2766->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2766;}); _temp2765;});
_temp2764;}); break; case 256: _LL2763: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2769=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2769[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2770; _temp2770.tag= Cyc_SwitchCClauseList_tok; _temp2770.f1= 0; _temp2770;});
_temp2769;}); break; case 257: _LL2768: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2772=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2772[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2773; _temp2773.tag= Cyc_SwitchCClauseList_tok; _temp2773.f1=({ struct Cyc_List_List*
_temp2774=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2774->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2775=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2775->cnst_exp= 0;
_temp2775->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2775->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2775;});
_temp2774->tl= 0; _temp2774;}); _temp2773;}); _temp2772;}); break; case 258:
_LL2771: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2777=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2777[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2778; _temp2778.tag=
Cyc_SwitchCClauseList_tok; _temp2778.f1=({ struct Cyc_List_List* _temp2779=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2779->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2780=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2780->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2780->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2780->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2780;});
_temp2779->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2779;}); _temp2778;});
_temp2777;}); break; case 259: _LL2776: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2782=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2782[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2783; _temp2783.tag= Cyc_SwitchCClauseList_tok; _temp2783.f1=({ struct Cyc_List_List*
_temp2784=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2784->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2785=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2785->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2785->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2785->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2785;});
_temp2784->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2784;}); _temp2783;});
_temp2782;}); break; case 260: _LL2781: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2787=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2787[ 0]=({ struct Cyc_Stmt_tok_struct _temp2788; _temp2788.tag= Cyc_Stmt_tok;
_temp2788.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2788;}); _temp2787;}); break; case 261: _LL2786: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2790=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2790[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2791; _temp2791.tag= Cyc_Stmt_tok; _temp2791.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2791;}); _temp2790;}); break; case 262: _LL2789: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2793=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2793[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2794; _temp2794.tag= Cyc_Stmt_tok; _temp2794.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2794;}); _temp2793;}); break; case 263: _LL2792: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2796=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2796[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2797; _temp2797.tag= Cyc_Stmt_tok; _temp2797.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2797;}); _temp2796;}); break; case 264: _LL2795: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2799=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2799[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2800; _temp2800.tag= Cyc_Stmt_tok; _temp2800.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2800;}); _temp2799;}); break; case 265: _LL2798: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2802=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2802[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2803; _temp2803.tag= Cyc_Stmt_tok; _temp2803.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2803;}); _temp2802;}); break; case 266: _LL2801: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2805=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2805[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2806; _temp2806.tag= Cyc_Stmt_tok; _temp2806.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2806;}); _temp2805;}); break; case 267: _LL2804: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2808=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2808[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2809; _temp2809.tag= Cyc_Stmt_tok; _temp2809.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2809;});
_temp2808;}); break; case 268: _LL2807: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2811=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2811[ 0]=({ struct Cyc_Stmt_tok_struct _temp2812; _temp2812.tag= Cyc_Stmt_tok;
_temp2812.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2812;});
_temp2811;}); break; case 269: _LL2810: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2814=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2814[ 0]=({ struct Cyc_Stmt_tok_struct _temp2815; _temp2815.tag= Cyc_Stmt_tok;
_temp2815.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2815;}); _temp2814;}); break; case 270: _LL2813: { struct Cyc_List_List*
_temp2817= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt* _temp2818= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2819=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2819[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2820; _temp2820.tag= Cyc_Stmt_tok; _temp2820.f1= Cyc_Parse_flatten_declarations(
_temp2817, _temp2818); _temp2820;}); _temp2819;}); break;} case 271: _LL2816: {
struct Cyc_List_List* _temp2822= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2823= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2824=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2824[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2825; _temp2825.tag= Cyc_Stmt_tok; _temp2825.f1= Cyc_Parse_flatten_declarations(
_temp2822, _temp2823); _temp2825;}); _temp2824;}); break;} case 272: _LL2821: {
struct Cyc_List_List* _temp2827= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2828= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2829=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2829[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2830; _temp2830.tag= Cyc_Stmt_tok; _temp2830.f1= Cyc_Parse_flatten_declarations(
_temp2827, _temp2828); _temp2830;}); _temp2829;}); break;} case 273: _LL2826: {
struct Cyc_List_List* _temp2832= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2833= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2834=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2834[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2835; _temp2835.tag= Cyc_Stmt_tok; _temp2835.f1= Cyc_Parse_flatten_declarations(
_temp2832, _temp2833); _temp2835;}); _temp2834;}); break;} case 274: _LL2831: {
struct Cyc_List_List* _temp2837=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Vardecl*(*
f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_decl2vardecl,
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  6)])); yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2838=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2838[ 0]=({ struct Cyc_Stmt_tok_struct _temp2839; _temp2839.tag= Cyc_Stmt_tok;
_temp2839.f1= Cyc_Absyn_forarray_stmt( _temp2837, Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2839;}); _temp2838;}); break;} case 275: _LL2836: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2841=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2841[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2842; _temp2842.tag= Cyc_Stmt_tok; _temp2842.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2843=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2843[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2843;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2842;}); _temp2841;}); break; case 276: _LL2840: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2845=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2845[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2846; _temp2846.tag= Cyc_Stmt_tok; _temp2846.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2846;}); _temp2845;}); break; case 277:
_LL2844: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2848=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2848[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2849; _temp2849.tag= Cyc_Stmt_tok; _temp2849.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2849;}); _temp2848;}); break; case 278: _LL2847: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2851=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2851[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2852; _temp2852.tag= Cyc_Stmt_tok; _temp2852.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2852;}); _temp2851;}); break; case 279:
_LL2850: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2854=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2854[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2855; _temp2855.tag= Cyc_Stmt_tok; _temp2855.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2855;}); _temp2854;}); break; case 280: _LL2853: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2857=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2857[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2858; _temp2858.tag= Cyc_Stmt_tok; _temp2858.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2858;}); _temp2857;}); break; case 281:
_LL2856: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2860=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2860[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2861; _temp2861.tag= Cyc_Stmt_tok; _temp2861.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2861;}); _temp2860;}); break; case 282:
_LL2859: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2863=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2863[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2864; _temp2864.tag= Cyc_Stmt_tok; _temp2864.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2864;}); _temp2863;}); break; case 283: _LL2862: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2866=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2866[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2867; _temp2867.tag= Cyc_Pattern_tok; _temp2867.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2867;});
_temp2866;}); break; case 284: _LL2865: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 285:
_LL2868: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2870=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2870[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2871; _temp2871.tag= Cyc_Pattern_tok; _temp2871.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2872=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2872[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2873; _temp2873.tag= Cyc_Absyn_Int_p; _temp2873.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2873.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2873;}); _temp2872;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2871;});
_temp2870;}); break; case 286: _LL2869: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2875=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2875[ 0]=({ struct Cyc_Pattern_tok_struct _temp2876; _temp2876.tag= Cyc_Pattern_tok;
_temp2876.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2877=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2877[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2878; _temp2878.tag= Cyc_Absyn_Int_p;
_temp2878.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2878.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2878;});
_temp2877;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2876;}); _temp2875;}); break; case 287:
_LL2874: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2880=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2880[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2881; _temp2881.tag= Cyc_Pattern_tok; _temp2881.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2882=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2882[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2883; _temp2883.tag= Cyc_Absyn_Float_p; _temp2883.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2883;}); _temp2882;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2881;});
_temp2880;}); break; case 288: _LL2879: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2885=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2885[ 0]=({ struct Cyc_Pattern_tok_struct _temp2886; _temp2886.tag= Cyc_Pattern_tok;
_temp2886.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2887=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2887[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2888; _temp2888.tag= Cyc_Absyn_Char_p; _temp2888.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2888;});
_temp2887;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2886;}); _temp2885;}); break; case 289:
_LL2884: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2890=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2890[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2891; _temp2891.tag= Cyc_Pattern_tok; _temp2891.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2891;});
_temp2890;}); break; case 290: _LL2889: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2893=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2893[ 0]=({ struct Cyc_Pattern_tok_struct _temp2894; _temp2894.tag= Cyc_Pattern_tok;
_temp2894.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2895=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2895[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2896; _temp2896.tag=
Cyc_Absyn_UnknownId_p; _temp2896.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2896;}); _temp2895;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2894;}); _temp2893;}); break; case 291:
_LL2892: { struct Cyc_List_List* _temp2898=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2899=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2899[ 0]=({ struct Cyc_Pattern_tok_struct _temp2900; _temp2900.tag= Cyc_Pattern_tok;
_temp2900.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2901=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2901[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2902; _temp2902.tag= Cyc_Absyn_UnknownCall_p; _temp2902.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2902.f2=
_temp2898; _temp2902.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2902;});
_temp2901;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2900;}); _temp2899;}); break;} case
292: _LL2897: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2904=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2904[ 0]=({ struct Cyc_Pattern_tok_struct _temp2905; _temp2905.tag= Cyc_Pattern_tok;
_temp2905.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2906=( struct Cyc_Absyn_Tuple_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2906[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2907; _temp2907.tag= Cyc_Absyn_Tuple_p;
_temp2907.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2907;}); _temp2906;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2905;}); _temp2904;}); break; case 293: _LL2903: { struct Cyc_List_List*
_temp2909=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); yyval=(
void*)({ struct Cyc_Pattern_tok_struct* _temp2910=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2910[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2911; _temp2911.tag= Cyc_Pattern_tok; _temp2911.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2912=( struct Cyc_Absyn_UnknownFields_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2912[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2913; _temp2913.tag= Cyc_Absyn_UnknownFields_p;
_temp2913.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2913.f2= _temp2909; _temp2913.f3= 0; _temp2913;});
_temp2912;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2911;}); _temp2910;}); break;} case
294: _LL2908: { struct Cyc_List_List* _temp2915=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2916=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2916[ 0]=({ struct Cyc_Pattern_tok_struct _temp2917; _temp2917.tag= Cyc_Pattern_tok;
_temp2917.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2918=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2918[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2919; _temp2919.tag= Cyc_Absyn_UnknownFields_p; _temp2919.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2919.f2=
_temp2915; _temp2919.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2919;});
_temp2918;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2917;}); _temp2916;}); break;} case
295: _LL2914: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2921=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2921[ 0]=({ struct Cyc_Pattern_tok_struct _temp2922; _temp2922.tag= Cyc_Pattern_tok;
_temp2922.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2923=( struct Cyc_Absyn_Pointer_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2923[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2924; _temp2924.tag= Cyc_Absyn_Pointer_p;
_temp2924.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2924;}); _temp2923;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2922;});
_temp2921;}); break; case 296: _LL2920: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2926=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2926[ 0]=({ struct Cyc_Pattern_tok_struct _temp2927; _temp2927.tag= Cyc_Pattern_tok;
_temp2927.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2928=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2928[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2929; _temp2929.tag=
Cyc_Absyn_Reference_p; _temp2929.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2930=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2930->f1=(
void*) Cyc_Absyn_Loc_n; _temp2930->f2=({ struct _tagged_arr* _temp2931=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2931[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2931;});
_temp2930;}),( void*) Cyc_Absyn_VoidType, 0); _temp2929;}); _temp2928;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2927;}); _temp2926;}); break; case 297: _LL2925: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2933=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2933[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2934; _temp2934.tag= Cyc_PatternList_tok;
_temp2934.f1= 0; _temp2934;}); _temp2933;}); break; case 298: _LL2932: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2936=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2936[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2937; _temp2937.tag= Cyc_PatternList_tok;
_temp2937.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2937;}); _temp2936;}); break; case 299: _LL2935: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2939=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2939[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2940; _temp2940.tag= Cyc_PatternList_tok;
_temp2940.f1=({ struct Cyc_List_List* _temp2941=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2941->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2941->tl= 0;
_temp2941;}); _temp2940;}); _temp2939;}); break; case 300: _LL2938: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2943=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2943[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2944; _temp2944.tag= Cyc_PatternList_tok;
_temp2944.f1=({ struct Cyc_List_List* _temp2945=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2945->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2945->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2945;}); _temp2944;}); _temp2943;}); break; case 301:
_LL2942: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2947=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2947[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2948; _temp2948.tag=
Cyc_FieldPattern_tok; _temp2948.f1=({ struct _tuple13* _temp2949=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2949->f1= 0; _temp2949->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2949;}); _temp2948;}); _temp2947;}); break; case 302:
_LL2946: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2951=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2951[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2952; _temp2952.tag=
Cyc_FieldPattern_tok; _temp2952.f1=({ struct _tuple13* _temp2953=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2953->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2953->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2953;}); _temp2952;}); _temp2951;}); break; case 303:
_LL2950: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2955=(
struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2955[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2956; _temp2956.tag=
Cyc_FieldPatternList_tok; _temp2956.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2956;});
_temp2955;}); break; case 304: _LL2954: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2958=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2958[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2959; _temp2959.tag= Cyc_FieldPatternList_tok; _temp2959.f1=({ struct Cyc_List_List*
_temp2960=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2960->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2960->tl= 0;
_temp2960;}); _temp2959;}); _temp2958;}); break; case 305: _LL2957: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2962=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2962[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2963; _temp2963.tag= Cyc_FieldPatternList_tok;
_temp2963.f1=({ struct Cyc_List_List* _temp2964=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2964->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2964->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2964;}); _temp2963;}); _temp2962;}); break; case 306:
_LL2961: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 307: _LL2965: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2967=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2967[ 0]=({ struct Cyc_Exp_tok_struct _temp2968; _temp2968.tag= Cyc_Exp_tok;
_temp2968.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2968;}); _temp2967;}); break; case 308: _LL2966: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 309: _LL2969:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2971=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2971[ 0]=({ struct Cyc_Exp_tok_struct
_temp2972; _temp2972.tag= Cyc_Exp_tok; _temp2972.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2972;}); _temp2971;}); break; case 310: _LL2970: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2974=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2974[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2975; _temp2975.tag= Cyc_Primopopt_tok; _temp2975.f1=
0; _temp2975;}); _temp2974;}); break; case 311: _LL2973: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2977=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2977[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2978; _temp2978.tag= Cyc_Primopopt_tok; _temp2978.f1=({
struct Cyc_Core_Opt* _temp2979=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2979->v=( void*)(( void*) Cyc_Absyn_Times); _temp2979;});
_temp2978;}); _temp2977;}); break; case 312: _LL2976: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2981=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2981[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2982; _temp2982.tag= Cyc_Primopopt_tok; _temp2982.f1=({
struct Cyc_Core_Opt* _temp2983=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2983->v=( void*)(( void*) Cyc_Absyn_Div); _temp2983;});
_temp2982;}); _temp2981;}); break; case 313: _LL2980: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2985=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2985[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2986; _temp2986.tag= Cyc_Primopopt_tok; _temp2986.f1=({
struct Cyc_Core_Opt* _temp2987=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2987->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2987;});
_temp2986;}); _temp2985;}); break; case 314: _LL2984: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2989=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2989[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2990; _temp2990.tag= Cyc_Primopopt_tok; _temp2990.f1=({
struct Cyc_Core_Opt* _temp2991=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2991->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2991;});
_temp2990;}); _temp2989;}); break; case 315: _LL2988: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2993=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2993[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2994; _temp2994.tag= Cyc_Primopopt_tok; _temp2994.f1=({
struct Cyc_Core_Opt* _temp2995=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2995->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2995;});
_temp2994;}); _temp2993;}); break; case 316: _LL2992: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2997=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2997[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2998; _temp2998.tag= Cyc_Primopopt_tok; _temp2998.f1=({
struct Cyc_Core_Opt* _temp2999=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2999->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2999;});
_temp2998;}); _temp2997;}); break; case 317: _LL2996: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3001=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3001[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3002; _temp3002.tag= Cyc_Primopopt_tok; _temp3002.f1=({
struct Cyc_Core_Opt* _temp3003=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3003->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3003;});
_temp3002;}); _temp3001;}); break; case 318: _LL3000: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3005=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3005[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3006; _temp3006.tag= Cyc_Primopopt_tok; _temp3006.f1=({
struct Cyc_Core_Opt* _temp3007=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3007->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3007;});
_temp3006;}); _temp3005;}); break; case 319: _LL3004: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3009=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3009[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3010; _temp3010.tag= Cyc_Primopopt_tok; _temp3010.f1=({
struct Cyc_Core_Opt* _temp3011=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3011->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3011;});
_temp3010;}); _temp3009;}); break; case 320: _LL3008: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3013=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3013[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3014; _temp3014.tag= Cyc_Primopopt_tok; _temp3014.f1=({
struct Cyc_Core_Opt* _temp3015=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3015->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3015;});
_temp3014;}); _temp3013;}); break; case 321: _LL3012: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 322: _LL3016:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3018=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3018[ 0]=({ struct Cyc_Exp_tok_struct
_temp3019; _temp3019.tag= Cyc_Exp_tok; _temp3019.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3019;});
_temp3018;}); break; case 323: _LL3017: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3021=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3021[ 0]=({ struct Cyc_Exp_tok_struct _temp3022; _temp3022.tag= Cyc_Exp_tok;
_temp3022.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3022;}); _temp3021;}); break; case 324: _LL3020: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3024=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3024[ 0]=({ struct Cyc_Exp_tok_struct
_temp3025; _temp3025.tag= Cyc_Exp_tok; _temp3025.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3025;}); _temp3024;}); break; case 325: _LL3023: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3027=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3027[ 0]=({ struct Cyc_Exp_tok_struct
_temp3028; _temp3028.tag= Cyc_Exp_tok; _temp3028.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3028;}); _temp3027;}); break; case 326: _LL3026: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3030=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3030[ 0]=({ struct Cyc_Exp_tok_struct
_temp3031; _temp3031.tag= Cyc_Exp_tok; _temp3031.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3031;});
_temp3030;}); break; case 327: _LL3029: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3033=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3033[ 0]=({ struct Cyc_Exp_tok_struct _temp3034; _temp3034.tag= Cyc_Exp_tok;
_temp3034.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3034;}); _temp3033;}); break; case 328: _LL3032: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL3035:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
330: _LL3036: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3038=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3038[ 0]=({ struct Cyc_Exp_tok_struct
_temp3039; _temp3039.tag= Cyc_Exp_tok; _temp3039.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3039;}); _temp3038;}); break; case 331: _LL3037: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 332: _LL3040:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3042=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3042[ 0]=({ struct Cyc_Exp_tok_struct
_temp3043; _temp3043.tag= Cyc_Exp_tok; _temp3043.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3043;}); _temp3042;}); break; case 333: _LL3041: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 334: _LL3044:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3046=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3046[ 0]=({ struct Cyc_Exp_tok_struct
_temp3047; _temp3047.tag= Cyc_Exp_tok; _temp3047.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3047;});
_temp3046;}); break; case 335: _LL3045: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 336: _LL3048:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3050=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3050[ 0]=({ struct Cyc_Exp_tok_struct
_temp3051; _temp3051.tag= Cyc_Exp_tok; _temp3051.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3051;});
_temp3050;}); break; case 337: _LL3049: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 338: _LL3052:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3054=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3054[ 0]=({ struct Cyc_Exp_tok_struct
_temp3055; _temp3055.tag= Cyc_Exp_tok; _temp3055.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3055;});
_temp3054;}); break; case 339: _LL3053: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 340: _LL3056:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3058=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3058[ 0]=({ struct Cyc_Exp_tok_struct
_temp3059; _temp3059.tag= Cyc_Exp_tok; _temp3059.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3059;}); _temp3058;}); break; case 341: _LL3057: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3061=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3061[ 0]=({ struct Cyc_Exp_tok_struct
_temp3062; _temp3062.tag= Cyc_Exp_tok; _temp3062.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3062;}); _temp3061;}); break; case 342: _LL3060: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 343: _LL3063:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3065=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3065[ 0]=({ struct Cyc_Exp_tok_struct
_temp3066; _temp3066.tag= Cyc_Exp_tok; _temp3066.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3066;}); _temp3065;}); break; case 344: _LL3064: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3068=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3068[ 0]=({ struct Cyc_Exp_tok_struct
_temp3069; _temp3069.tag= Cyc_Exp_tok; _temp3069.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3069;}); _temp3068;}); break; case 345: _LL3067: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3071=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3071[ 0]=({ struct Cyc_Exp_tok_struct
_temp3072; _temp3072.tag= Cyc_Exp_tok; _temp3072.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3072;}); _temp3071;}); break; case 346: _LL3070: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3074=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3074[ 0]=({ struct Cyc_Exp_tok_struct
_temp3075; _temp3075.tag= Cyc_Exp_tok; _temp3075.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3075;}); _temp3074;}); break; case 347: _LL3073: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 348: _LL3076:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3078=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3078[ 0]=({ struct Cyc_Exp_tok_struct
_temp3079; _temp3079.tag= Cyc_Exp_tok; _temp3079.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3079;}); _temp3078;}); break; case 349: _LL3077: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3081=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3081[ 0]=({ struct Cyc_Exp_tok_struct
_temp3082; _temp3082.tag= Cyc_Exp_tok; _temp3082.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3082;}); _temp3081;}); break; case 350: _LL3080: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 351: _LL3083:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3085=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3085[ 0]=({ struct Cyc_Exp_tok_struct
_temp3086; _temp3086.tag= Cyc_Exp_tok; _temp3086.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3086;});
_temp3085;}); break; case 352: _LL3084: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3088=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3088[ 0]=({ struct Cyc_Exp_tok_struct _temp3089; _temp3089.tag= Cyc_Exp_tok;
_temp3089.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3089;}); _temp3088;}); break; case 353: _LL3087: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 354: _LL3090:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3092=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3092[ 0]=({ struct Cyc_Exp_tok_struct
_temp3093; _temp3093.tag= Cyc_Exp_tok; _temp3093.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3093;});
_temp3092;}); break; case 355: _LL3091: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3095=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3095[ 0]=({ struct Cyc_Exp_tok_struct _temp3096; _temp3096.tag= Cyc_Exp_tok;
_temp3096.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3096;}); _temp3095;}); break; case 356: _LL3094: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3098=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3098[ 0]=({ struct Cyc_Exp_tok_struct
_temp3099; _temp3099.tag= Cyc_Exp_tok; _temp3099.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3099;});
_temp3098;}); break; case 357: _LL3097: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 358: _LL3100:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3102=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3102[ 0]=({ struct Cyc_Exp_tok_struct
_temp3103; _temp3103.tag= Cyc_Exp_tok; _temp3103.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3103;}); _temp3102;}); break; case 359: _LL3101: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 360: _LL3104:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3106=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3106[ 0]=({ struct Cyc_Exp_tok_struct
_temp3107; _temp3107.tag= Cyc_Exp_tok; _temp3107.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3107;}); _temp3106;}); break; case 361: _LL3105: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3109=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3109[ 0]=({ struct Cyc_Exp_tok_struct
_temp3110; _temp3110.tag= Cyc_Exp_tok; _temp3110.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3110;}); _temp3109;}); break; case 362: _LL3108: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3112=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3112[ 0]=({ struct Cyc_Exp_tok_struct
_temp3113; _temp3113.tag= Cyc_Exp_tok; _temp3113.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3113;}); _temp3112;}); break; case 363: _LL3111: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3115=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3115[ 0]=({ struct Cyc_Exp_tok_struct
_temp3116; _temp3116.tag= Cyc_Exp_tok; _temp3116.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3116;}); _temp3115;}); break; case 364: _LL3114: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 365: _LL3117:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3119=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3119[ 0]=({ struct Cyc_Exp_tok_struct
_temp3120; _temp3120.tag= Cyc_Exp_tok; _temp3120.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3120;}); _temp3119;}); break; case 366: _LL3118: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3122=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3122[ 0]=({ struct Cyc_Exp_tok_struct
_temp3123; _temp3123.tag= Cyc_Exp_tok; _temp3123.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3123;});
_temp3122;}); break; case 367: _LL3121: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3125=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3125[ 0]=({ struct Cyc_Exp_tok_struct _temp3126; _temp3126.tag= Cyc_Exp_tok;
_temp3126.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3126;}); _temp3125;}); break; case 368: _LL3124: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3128=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3128[ 0]=({ struct Cyc_Exp_tok_struct
_temp3129; _temp3129.tag= Cyc_Exp_tok; _temp3129.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_StructField_struct*
_temp3130=( struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp3130[ 0]=({ struct Cyc_Absyn_StructField_struct _temp3131; _temp3131.tag=
Cyc_Absyn_StructField; _temp3131.f1=({ struct _tagged_arr* _temp3132=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp3132[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3132;});
_temp3131;}); _temp3130;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3129;});
_temp3128;}); break; case 369: _LL3127: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3134=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3134[ 0]=({ struct Cyc_Exp_tok_struct _temp3135; _temp3135.tag= Cyc_Exp_tok;
_temp3135.f1= Cyc_Absyn_offsetof_exp((* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])).f3,( void*)({
struct Cyc_Absyn_TupleIndex_struct* _temp3136=( struct Cyc_Absyn_TupleIndex_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Absyn_TupleIndex_struct)); _temp3136[ 0]=({
struct Cyc_Absyn_TupleIndex_struct _temp3137; _temp3137.tag= Cyc_Absyn_TupleIndex;
_temp3137.f1=( unsigned int)(* Cyc_yyget_Int_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f2; _temp3137;});
_temp3136;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3135;}); _temp3134;}); break; case 370:
_LL3133: { struct Cyc_Position_Segment* _temp3139= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); struct
Cyc_List_List* _temp3140=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, _temp3139, Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3141=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3141[ 0]=({ struct Cyc_Exp_tok_struct
_temp3142; _temp3142.tag= Cyc_Exp_tok; _temp3142.f1= Cyc_Absyn_gentyp_exp(
_temp3140,(* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3142;});
_temp3141;}); break;} case 371: _LL3138: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3144=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3144[ 0]=({ struct Cyc_Exp_tok_struct _temp3145; _temp3145.tag= Cyc_Exp_tok;
_temp3145.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3146=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3146[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3147; _temp3147.tag= Cyc_Absyn_Malloc_e;
_temp3147.f1=({ struct Cyc_Absyn_MallocInfo _temp3148; _temp3148.is_calloc= 0;
_temp3148.rgn= 0; _temp3148.elt_type= 0; _temp3148.num_elts= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3148.fat_result=
0; _temp3148;}); _temp3147;}); _temp3146;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3145;});
_temp3144;}); break; case 372: _LL3143: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3150=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3150[ 0]=({ struct Cyc_Exp_tok_struct _temp3151; _temp3151.tag= Cyc_Exp_tok;
_temp3151.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3152=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3152[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3153; _temp3153.tag= Cyc_Absyn_Malloc_e;
_temp3153.f1=({ struct Cyc_Absyn_MallocInfo _temp3154; _temp3154.is_calloc= 0;
_temp3154.rgn=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp3154.elt_type=
0; _temp3154.num_elts= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp3154.fat_result= 0; _temp3154;}); _temp3153;});
_temp3152;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3151;}); _temp3150;}); break; case 373:
_LL3149: { void* _temp3158; struct _tuple2 _temp3156=* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _LL3159:
_temp3158= _temp3156.f3; goto _LL3157; _LL3157: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3160=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3160[ 0]=({ struct Cyc_Exp_tok_struct _temp3161; _temp3161.tag= Cyc_Exp_tok;
_temp3161.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3162=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3162[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3163; _temp3163.tag= Cyc_Absyn_Malloc_e;
_temp3163.f1=({ struct Cyc_Absyn_MallocInfo _temp3164; _temp3164.is_calloc= 1;
_temp3164.rgn= 0; _temp3164.elt_type=({ void** _temp3165=( void**) _cycalloc(
sizeof( void*)); _temp3165[ 0]= _temp3158; _temp3165;}); _temp3164.num_elts= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3164.fat_result=
0; _temp3164;}); _temp3163;}); _temp3162;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3161;});
_temp3160;}); break;} case 374: _LL3155: { void* _temp3169; struct _tuple2
_temp3167=* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _LL3170: _temp3169= _temp3167.f3; goto _LL3168; _LL3168:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3171=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3171[ 0]=({ struct Cyc_Exp_tok_struct
_temp3172; _temp3172.tag= Cyc_Exp_tok; _temp3172.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3173=( struct Cyc_Absyn_Malloc_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3173[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3174; _temp3174.tag= Cyc_Absyn_Malloc_e;
_temp3174.f1=({ struct Cyc_Absyn_MallocInfo _temp3175; _temp3175.is_calloc= 1;
_temp3175.rgn=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  8)]); _temp3175.elt_type=({
void** _temp3176=( void**) _cycalloc( sizeof( void*)); _temp3176[ 0]= _temp3169;
_temp3176;}); _temp3175.num_elts= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3175.fat_result=
0; _temp3175;}); _temp3174;}); _temp3173;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  10)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3172;});
_temp3171;}); break;} case 375: _LL3166: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3178=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3178[ 0]=({ struct Cyc_Primop_tok_struct _temp3179; _temp3179.tag= Cyc_Primop_tok;
_temp3179.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3179;}); _temp3178;});
break; case 376: _LL3177: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3181=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3181[ 0]=({ struct Cyc_Primop_tok_struct _temp3182; _temp3182.tag= Cyc_Primop_tok;
_temp3182.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3182;}); _temp3181;});
break; case 377: _LL3180: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3184=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3184[ 0]=({ struct Cyc_Primop_tok_struct _temp3185; _temp3185.tag= Cyc_Primop_tok;
_temp3185.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3185;}); _temp3184;});
break; case 378: _LL3183: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 379: _LL3186: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3188=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3188[ 0]=({ struct Cyc_Exp_tok_struct _temp3189; _temp3189.tag= Cyc_Exp_tok;
_temp3189.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3189;}); _temp3188;}); break; case 380: _LL3187: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3191=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3191[ 0]=({ struct Cyc_Exp_tok_struct
_temp3192; _temp3192.tag= Cyc_Exp_tok; _temp3192.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3192;}); _temp3191;}); break; case 381:
_LL3190: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3194=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3194[ 0]=({ struct Cyc_Exp_tok_struct
_temp3195; _temp3195.tag= Cyc_Exp_tok; _temp3195.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3195;});
_temp3194;}); break; case 382: _LL3193: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3197=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3197[ 0]=({ struct Cyc_Exp_tok_struct _temp3198; _temp3198.tag= Cyc_Exp_tok;
_temp3198.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3199=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3199[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3199;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3198;}); _temp3197;}); break; case 383: _LL3196: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3201=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3201[ 0]=({ struct Cyc_Exp_tok_struct
_temp3202; _temp3202.tag= Cyc_Exp_tok; _temp3202.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3202;});
_temp3201;}); break;} case 384: _LL3200: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3204=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3204[ 0]=({ struct Cyc_Exp_tok_struct _temp3205; _temp3205.tag= Cyc_Exp_tok;
_temp3205.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3206=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3206[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3206;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3205;}); _temp3204;}); break; case 385: _LL3203: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3208=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3208[ 0]=({ struct Cyc_Exp_tok_struct
_temp3209; _temp3209.tag= Cyc_Exp_tok; _temp3209.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3209;});
_temp3208;}); break;} case 386: _LL3207: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3211=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3211[ 0]=({ struct Cyc_Exp_tok_struct _temp3212; _temp3212.tag= Cyc_Exp_tok;
_temp3212.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3212;}); _temp3211;}); break; case 387: _LL3210: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3214=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3214[ 0]=({ struct Cyc_Exp_tok_struct
_temp3215; _temp3215.tag= Cyc_Exp_tok; _temp3215.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3215;}); _temp3214;}); break; case 388: _LL3213: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3217=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3217[ 0]=({ struct Cyc_Exp_tok_struct
_temp3218; _temp3218.tag= Cyc_Exp_tok; _temp3218.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3219=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3219[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3220; _temp3220.tag= Cyc_Absyn_CompoundLit_e;
_temp3220.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3220.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3220;});
_temp3219;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3218;}); _temp3217;}); break; case 389:
_LL3216: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3222=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3222[ 0]=({ struct Cyc_Exp_tok_struct
_temp3223; _temp3223.tag= Cyc_Exp_tok; _temp3223.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3224=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3224[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3225; _temp3225.tag= Cyc_Absyn_CompoundLit_e;
_temp3225.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3225.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3225;});
_temp3224;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3223;}); _temp3222;}); break; case 390:
_LL3221: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3227=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3227[ 0]=({ struct Cyc_Exp_tok_struct
_temp3228; _temp3228.tag= Cyc_Exp_tok; _temp3228.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3229=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3229[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3230; _temp3230.tag= Cyc_Absyn_Fill_e; _temp3230.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3230;});
_temp3229;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3228;}); _temp3227;}); break; case 391:
_LL3226: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3232=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3232[ 0]=({ struct Cyc_Exp_tok_struct
_temp3233; _temp3233.tag= Cyc_Exp_tok; _temp3233.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3234=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3234[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3235; _temp3235.tag= Cyc_Absyn_Codegen_e;
_temp3235.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3235;}); _temp3234;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3233;}); _temp3232;}); break; case 392: _LL3231: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3237=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3237[ 0]=({ struct Cyc_Exp_tok_struct
_temp3238; _temp3238.tag= Cyc_Exp_tok; _temp3238.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3239=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3239[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3240; _temp3240.tag= Cyc_Absyn_UnknownId_e;
_temp3240.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3240;}); _temp3239;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3238;});
_temp3237;}); break; case 393: _LL3236: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 394: _LL3241:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3243=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3243[ 0]=({ struct Cyc_Exp_tok_struct
_temp3244; _temp3244.tag= Cyc_Exp_tok; _temp3244.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3244;});
_temp3243;}); break; case 395: _LL3242: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 396:
_LL3245: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3247=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3247[ 0]=({ struct Cyc_Exp_tok_struct
_temp3248; _temp3248.tag= Cyc_Exp_tok; _temp3248.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3249=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3249[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3250; _temp3250.tag=
Cyc_Absyn_UnknownId_e; _temp3250.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3250;});
_temp3249;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3248;});
_temp3247;}); break; case 397: _LL3246: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3252=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3252[ 0]=({ struct Cyc_Exp_tok_struct _temp3253; _temp3253.tag= Cyc_Exp_tok;
_temp3253.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3254=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3254[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3255; _temp3255.tag=
Cyc_Absyn_UnknownId_e; _temp3255.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3255;});
_temp3254;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3253;}); _temp3252;}); break; case 398: _LL3251: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3257=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3257[ 0]=({ struct Cyc_Exp_tok_struct
_temp3258; _temp3258.tag= Cyc_Exp_tok; _temp3258.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3258;}); _temp3257;}); break; case 399: _LL3256: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3260=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3260[ 0]=({ struct Cyc_Exp_tok_struct
_temp3261; _temp3261.tag= Cyc_Exp_tok; _temp3261.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3262=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3262[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3263; _temp3263.tag= Cyc_Absyn_Struct_e;
_temp3263.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3263.f2= 0; _temp3263.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3263.f4= 0;
_temp3263;}); _temp3262;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3261;});
_temp3260;}); break; case 400: _LL3259: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3265=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3265[ 0]=({ struct Cyc_Exp_tok_struct _temp3266; _temp3266.tag= Cyc_Exp_tok;
_temp3266.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3266;}); _temp3265;}); break; case 401: _LL3264: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3268=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3268[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3269; _temp3269.tag= Cyc_ExpList_tok; _temp3269.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3269;});
_temp3268;}); break; case 402: _LL3267: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3271=( struct Cyc_ExpList_tok_struct*) _cycalloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3271[ 0]=({ struct Cyc_ExpList_tok_struct _temp3272; _temp3272.tag= Cyc_ExpList_tok;
_temp3272.f1=({ struct Cyc_List_List* _temp3273=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3273->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3273->tl= 0;
_temp3273;}); _temp3272;}); _temp3271;}); break; case 403: _LL3270: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3275=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3275[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3276; _temp3276.tag= Cyc_ExpList_tok; _temp3276.f1=({ struct Cyc_List_List*
_temp3277=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3277->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3277->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp3277;});
_temp3276;}); _temp3275;}); break; case 404: _LL3274: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3279=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3279[ 0]=({ struct Cyc_Exp_tok_struct
_temp3280; _temp3280.tag= Cyc_Exp_tok; _temp3280.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3280;});
_temp3279;}); break; case 405: _LL3278: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3282=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3282[ 0]=({ struct Cyc_Exp_tok_struct _temp3283; _temp3283.tag= Cyc_Exp_tok;
_temp3283.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3283;});
_temp3282;}); break; case 406: _LL3281: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3285=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3285[ 0]=({ struct Cyc_Exp_tok_struct _temp3286; _temp3286.tag= Cyc_Exp_tok;
_temp3286.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3286;});
_temp3285;}); break; case 407: _LL3284: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3288=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3288[ 0]=({ struct Cyc_Exp_tok_struct _temp3289; _temp3289.tag= Cyc_Exp_tok;
_temp3289.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3289;});
_temp3288;}); break; case 408: _LL3287: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3291=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3291[ 0]=({ struct Cyc_QualId_tok_struct _temp3292; _temp3292.tag= Cyc_QualId_tok;
_temp3292.f1=({ struct _tuple1* _temp3293=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3293->f1= Cyc_Absyn_rel_ns_null; _temp3293->f2=({ struct
_tagged_arr* _temp3294=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3294[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3294;}); _temp3293;});
_temp3292;}); _temp3291;}); break; case 409: _LL3290: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3295:
break;} yyvsp_offset -= yylen; yyssp_offset -= yylen; yylsp_offset -= yylen;
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= yyval;
yylsp_offset ++; if( yylen ==  0){( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line= Cyc_yylloc.first_line;( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_column= Cyc_yylloc.first_column;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line=( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line;( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_column=( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_column;} else{(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line=( yyls[
_check_known_subscript_notnull( 10000u,( yylsp_offset +  yylen) -  1)]).last_line;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_column=( yyls[
_check_known_subscript_notnull( 10000u,( yylsp_offset +  yylen) -  1)]).last_column;}
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 410u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 112u, yyn -  125)] +  yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >=  0?
yystate <=  4995: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4996u,
yystate)] ==  yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){
yystate=( int) Cyc_yytable[ _check_known_subscript_notnull( 4996u, yystate)];}
else{ yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 
125)];} goto yynewstate; yyerrlab: if( yyerrstatus ==  0){ ++ Cyc_yynerrs; yyn=(
int) Cyc_yypact[ _check_known_subscript_notnull( 831u, yystate)]; if( yyn >  -
32768? yyn <  4995: 0){ int sze= 0; struct _tagged_arr msg; int x; int count;
count= 0; for( x= yyn <  0? - yyn: 0; x <  237u /  sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4996u, x +  yyn)] ==  x){( sze
+= Cyc_Std_strlen( Cyc_yytname[ _check_known_subscript_notnull( 237u, x)]) +  15,
count ++);}} msg=({ unsigned int _temp3297=( unsigned int)( sze +  15);
unsigned char* _temp3298=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp3297)); struct _tagged_arr _temp3300= _tag_arr(
_temp3298, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp3299= _temp3297; unsigned int i; for( i= 0; i <  _temp3299; i ++){
_temp3298[ i]='\000';}}; _temp3300;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
sizeof( unsigned char), 12u)); if( count <  5){ count= 0; for( x= yyn <  0? -
yyn: 0; x <  237u /  sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4996u, x +  yyn)] ==  x){ Cyc_Std_strcat( msg,
count ==  0? _tag_arr(", expecting `", sizeof( unsigned char), 14u): _tag_arr(" or `",
sizeof( unsigned char), 6u)); Cyc_Std_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 237u, x)]); Cyc_Std_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus ==  3){ if( Cyc_yychar ==  0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset ==  0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 831u,
yystate)]; if( yyn ==  - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn <  0? 1:
yyn >  4995)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4996u, yyn)] !=  1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4996u,
yyn)]; if( yyn <  0){ if( yyn ==  - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn ==  0){ goto yyerrpop;}} if( yyn ==  830){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3310= v; struct
_tuple15* _temp3328; struct _tuple15 _temp3330; int _temp3331; unsigned char
_temp3333; short _temp3335; struct _tagged_arr _temp3337; struct Cyc_Core_Opt*
_temp3339; struct Cyc_Core_Opt* _temp3341; struct Cyc_Core_Opt _temp3343; struct
_tagged_arr* _temp3344; struct _tuple1* _temp3346; struct _tuple1 _temp3348;
struct _tagged_arr* _temp3349; void* _temp3351; _LL3312: if(*(( void**)
_temp3310) ==  Cyc_Int_tok){ _LL3329: _temp3328=(( struct Cyc_Int_tok_struct*)
_temp3310)->f1; _temp3330=* _temp3328; _LL3332: _temp3331= _temp3330.f2; goto
_LL3313;} else{ goto _LL3314;} _LL3314: if(*(( void**) _temp3310) ==  Cyc_Char_tok){
_LL3334: _temp3333=(( struct Cyc_Char_tok_struct*) _temp3310)->f1; goto _LL3315;}
else{ goto _LL3316;} _LL3316: if(*(( void**) _temp3310) ==  Cyc_Short_tok){
_LL3336: _temp3335=(( struct Cyc_Short_tok_struct*) _temp3310)->f1; goto _LL3317;}
else{ goto _LL3318;} _LL3318: if(*(( void**) _temp3310) ==  Cyc_String_tok){
_LL3338: _temp3337=(( struct Cyc_String_tok_struct*) _temp3310)->f1; goto
_LL3319;} else{ goto _LL3320;} _LL3320: if(*(( void**) _temp3310) ==  Cyc_Stringopt_tok){
_LL3340: _temp3339=(( struct Cyc_Stringopt_tok_struct*) _temp3310)->f1; if(
_temp3339 ==  0){ goto _LL3321;} else{ goto _LL3322;}} else{ goto _LL3322;}
_LL3322: if(*(( void**) _temp3310) ==  Cyc_Stringopt_tok){ _LL3342: _temp3341=((
struct Cyc_Stringopt_tok_struct*) _temp3310)->f1; if( _temp3341 ==  0){ goto
_LL3324;} else{ _temp3343=* _temp3341; _LL3345: _temp3344=( struct _tagged_arr*)
_temp3343.v; goto _LL3323;}} else{ goto _LL3324;} _LL3324: if(*(( void**)
_temp3310) ==  Cyc_QualId_tok){ _LL3347: _temp3346=(( struct Cyc_QualId_tok_struct*)
_temp3310)->f1; _temp3348=* _temp3346; _LL3352: _temp3351= _temp3348.f1; goto
_LL3350; _LL3350: _temp3349= _temp3348.f2; goto _LL3325;} else{ goto _LL3326;}
_LL3326: goto _LL3327; _LL3313:({ struct Cyc_Std_Int_pa_struct _temp3354;
_temp3354.tag= Cyc_Std_Int_pa; _temp3354.f1=( int)(( unsigned int) _temp3331);{
void* _temp3353[ 1u]={& _temp3354}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3353, sizeof( void*), 1u));}}); goto
_LL3311; _LL3315:({ struct Cyc_Std_Int_pa_struct _temp3356; _temp3356.tag= Cyc_Std_Int_pa;
_temp3356.f1=( int)(( unsigned int)(( int) _temp3333));{ void* _temp3355[ 1u]={&
_temp3356}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3355, sizeof( void*), 1u));}}); goto _LL3311;
_LL3317:({ struct Cyc_Std_Int_pa_struct _temp3358; _temp3358.tag= Cyc_Std_Int_pa;
_temp3358.f1=( int)(( unsigned int)(( int) _temp3335));{ void* _temp3357[ 1u]={&
_temp3358}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3357, sizeof( void*), 1u));}}); goto _LL3311;
_LL3319:({ struct Cyc_Std_String_pa_struct _temp3360; _temp3360.tag= Cyc_Std_String_pa;
_temp3360.f1=( struct _tagged_arr) _temp3337;{ void* _temp3359[ 1u]={& _temp3360};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3359, sizeof( void*), 1u));}}); goto _LL3311; _LL3321:({ void*
_temp3361[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3361, sizeof( void*), 0u));}); goto _LL3311;
_LL3323:({ struct Cyc_Std_String_pa_struct _temp3363; _temp3363.tag= Cyc_Std_String_pa;
_temp3363.f1=( struct _tagged_arr)* _temp3344;{ void* _temp3362[ 1u]={&
_temp3363}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3362, sizeof( void*), 1u));}}); goto _LL3311;
_LL3325: { struct Cyc_List_List* _temp3364= 0;{ void* _temp3365= _temp3351;
struct Cyc_List_List* _temp3373; struct Cyc_List_List* _temp3375; _LL3367: if((
unsigned int) _temp3365 >  1u?*(( int*) _temp3365) ==  Cyc_Absyn_Rel_n: 0){
_LL3374: _temp3373=(( struct Cyc_Absyn_Rel_n_struct*) _temp3365)->f1; goto
_LL3368;} else{ goto _LL3369;} _LL3369: if(( unsigned int) _temp3365 >  1u?*((
int*) _temp3365) ==  Cyc_Absyn_Abs_n: 0){ _LL3376: _temp3375=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3365)->f1; goto _LL3370;} else{ goto _LL3371;} _LL3371: if( _temp3365 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3372;} else{ goto _LL3366;} _LL3368: _temp3364=
_temp3373; goto _LL3366; _LL3370: _temp3364= _temp3375; goto _LL3366; _LL3372:
goto _LL3366; _LL3366:;} for( 0; _temp3364 !=  0; _temp3364= _temp3364->tl){({
struct Cyc_Std_String_pa_struct _temp3378; _temp3378.tag= Cyc_Std_String_pa;
_temp3378.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _temp3364->hd);{ void*
_temp3377[ 1u]={& _temp3378}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3377, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3380; _temp3380.tag= Cyc_Std_String_pa;
_temp3380.f1=( struct _tagged_arr)* _temp3349;{ void* _temp3379[ 1u]={&
_temp3380}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3379, sizeof( void*), 1u));}}); goto _LL3311;}
_LL3327:({ void* _temp3381[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3381, sizeof( void*), 0u));}); goto
_LL3311; _LL3311:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3382=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3382->v=(
void*) Cyc_Lexing_from_file( f); _temp3382;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
