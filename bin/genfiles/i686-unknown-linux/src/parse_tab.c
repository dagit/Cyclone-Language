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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_copy_type( void* t); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Lex_register_typedef( struct _tuple1* s); extern void
Cyc_Lex_enter_namespace( struct _tagged_arr*); extern void Cyc_Lex_leave_namespace();
extern void Cyc_Lex_enter_using( struct _tuple1*); extern void Cyc_Lex_leave_using();
enum  Cyc_Parse_Struct_or_union{ Cyc_Parse_Struct_su  =  0u, Cyc_Parse_Union_su
 =  1u}; static const int Cyc_Parse_Signed_spec= 0; struct Cyc_Parse_Signed_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Unsigned_spec=
1; struct Cyc_Parse_Unsigned_spec_struct{ int tag; struct Cyc_Position_Segment*
f1; } ; static const int Cyc_Parse_Short_spec= 2; struct Cyc_Parse_Short_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Long_spec=
3; struct Cyc_Parse_Long_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Type_spec= 4; struct Cyc_Parse_Type_spec_struct{
int tag; void* f1; struct Cyc_Position_Segment* f2; } ; static const int Cyc_Parse_Decl_spec=
5; struct Cyc_Parse_Decl_spec_struct{ int tag; struct Cyc_Absyn_Decl* f1; } ;
static const int Cyc_Parse_Typedef_sc= 0; static const int Cyc_Parse_Extern_sc=
1; static const int Cyc_Parse_ExternC_sc= 2; static const int Cyc_Parse_Static_sc=
3; static const int Cyc_Parse_Auto_sc= 4; static const int Cyc_Parse_Register_sc=
5; static const int Cyc_Parse_Abstract_sc= 6; struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual tq; struct Cyc_List_List*
type_specs; int is_inline; struct Cyc_List_List* attributes; } ; struct Cyc_Parse_Declarator{
struct _tuple1* id; struct Cyc_List_List* tms; } ; struct Cyc_Parse_Abstractdeclarator{
struct Cyc_List_List* tms; } ; struct _tuple4{ struct _tagged_arr* f1; struct
Cyc_Absyn_Tqual f2; void* f3; } ; static struct _tuple4* Cyc_Parse_fnargspec_to_arg(
struct Cyc_Position_Segment* loc, struct _tuple2* t); struct _tuple5{ void* f1;
struct Cyc_Core_Opt* f2; } ; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc); struct _tuple6{
struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; struct Cyc_List_List*
f4; } ; static struct _tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual, void*,
struct Cyc_List_List*, struct Cyc_List_List*); struct _tuple7{ struct _tuple1*
f1; struct Cyc_Absyn_Tqual f2; void* f3; struct Cyc_List_List* f4; struct Cyc_List_List*
f5; } ; static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
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
msg, sg);( int) _throw(( void*) Cyc_Position_Exit);} static void* Cyc_Parse_unimp(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){ return Cyc_Parse_abort((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp18; _temp18.tag= Cyc_Std_String_pa;
_temp18.f1=( struct _tagged_arr) msg;{ void* _temp17[ 1u]={& _temp18}; Cyc_Std_aprintf(
_tag_arr("%s unimplemented", sizeof( unsigned char), 17u), _tag_arr( _temp17,
sizeof( void*), 1u));}}), sg);} static void Cyc_Parse_unimp2( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg){({ struct Cyc_Std_String_pa_struct _temp21;
_temp21.tag= Cyc_Std_String_pa; _temp21.f1=( struct _tagged_arr) msg;{ struct
Cyc_Std_String_pa_struct _temp20; _temp20.tag= Cyc_Std_String_pa; _temp20.f1=(
struct _tagged_arr) Cyc_Position_string_of_segment( sg);{ void* _temp19[ 2u]={&
_temp20,& _temp21}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s: Warning: Cyclone does not yet support %s\n",
sizeof( unsigned char), 46u), _tag_arr( _temp19, sizeof( void*), 2u));}}});
return;} static int Cyc_Parse_enum_counter= 0; struct _tuple1* Cyc_Parse_gensym_enum(){
return({ struct _tuple1* _temp22=( struct _tuple1*) _cycalloc( sizeof( struct
_tuple1)); _temp22->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct* _temp26=(
struct Cyc_Absyn_Rel_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp26[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp27; _temp27.tag= Cyc_Absyn_Rel_n;
_temp27.f1= 0; _temp27;}); _temp26;}); _temp22->f2=({ struct _tagged_arr*
_temp23=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp23[
0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp25; _temp25.tag=
Cyc_Std_Int_pa; _temp25.f1=( int)(( unsigned int) Cyc_Parse_enum_counter ++);{
void* _temp24[ 1u]={& _temp25}; Cyc_Std_aprintf( _tag_arr("__anonymous_enum_%d__",
sizeof( unsigned char), 22u), _tag_arr( _temp24, sizeof( void*), 1u));}});
_temp23;}); _temp22;});} struct _tuple9{ struct _tuple7* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field( struct
Cyc_Position_Segment* loc, struct _tuple9* field_info){ struct _tuple9 _temp30;
struct Cyc_Absyn_Exp* _temp31; struct _tuple7* _temp33; struct _tuple7 _temp35;
struct Cyc_List_List* _temp36; struct Cyc_List_List* _temp38; void* _temp40;
struct Cyc_Absyn_Tqual _temp42; struct _tuple1* _temp44; struct _tuple9* _temp28=
field_info; _temp30=* _temp28; _LL34: _temp33= _temp30.f1; _temp35=* _temp33;
_LL45: _temp44= _temp35.f1; goto _LL43; _LL43: _temp42= _temp35.f2; goto _LL41;
_LL41: _temp40= _temp35.f3; goto _LL39; _LL39: _temp38= _temp35.f4; goto _LL37;
_LL37: _temp36= _temp35.f5; goto _LL32; _LL32: _temp31= _temp30.f2; goto _LL29;
_LL29: if( _temp38 !=  0){ Cyc_Parse_err( _tag_arr("bad type params in struct field",
sizeof( unsigned char), 32u), loc);} if( Cyc_Absyn_is_qvar_qualified( _temp44)){
Cyc_Parse_err( _tag_arr("struct field cannot be qualified with a namespace",
sizeof( unsigned char), 50u), loc);} return({ struct Cyc_Absyn_Structfield*
_temp46=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp46->name=(* _temp44).f2; _temp46->tq= _temp42; _temp46->type=( void*)
_temp40; _temp46->width= _temp31; _temp46->attributes= _temp36; _temp46;});}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp49; struct Cyc_List_List* _temp50; void* _temp52; struct Cyc_Absyn_Tqual
_temp54; struct Cyc_Core_Opt* _temp56; struct _tuple10* _temp47= field; _temp49=*
_temp47; _LL57: _temp56= _temp49.f1; goto _LL55; _LL55: _temp54= _temp49.f2;
goto _LL53; _LL53: _temp52= _temp49.f3; goto _LL51; _LL51: _temp50= _temp49.f4;
goto _LL48; _LL48: { struct Cyc_Core_Opt* _temp58= 0; if( _temp56 !=  0){ if(
Cyc_Absyn_is_qvar_qualified(( struct _tuple1*) _temp56->v)){ Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace", sizeof( unsigned char),
47u), loc);} _temp58=({ struct Cyc_Core_Opt* _temp59=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp59->v=( void*)(*(( struct _tuple1*)
_temp56->v)).f2; _temp59;});} if( _temp50 !=  0){(( int(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("parameter should have no type parameters",
sizeof( unsigned char), 41u), loc);} return({ struct _tuple2* _temp60=( struct
_tuple2*) _cycalloc( sizeof( struct _tuple2)); _temp60->f1= _temp58; _temp60->f2=
_temp54; _temp60->f3= _temp52; _temp60;});}} static void* Cyc_Parse_type_spec(
void* t, struct Cyc_Position_Segment* loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp61=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp61[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp62; _temp62.tag= Cyc_Parse_Type_spec;
_temp62.f1=( void*) t; _temp62.f2= loc; _temp62;}); _temp61;});} static void*
Cyc_Parse_array2ptr( void* t, int argposn){ void* _temp63= t; struct Cyc_Absyn_Exp*
_temp69; struct Cyc_Absyn_Tqual _temp71; void* _temp73; _LL65: if(( unsigned int)
_temp63 >  3u?*(( int*) _temp63) ==  Cyc_Absyn_ArrayType: 0){ _LL74: _temp73=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp63)->f1; goto _LL72; _LL72:
_temp71=(( struct Cyc_Absyn_ArrayType_struct*) _temp63)->f2; goto _LL70; _LL70:
_temp69=(( struct Cyc_Absyn_ArrayType_struct*) _temp63)->f3; goto _LL66;} else{
goto _LL67;} _LL67: goto _LL68; _LL66: return Cyc_Absyn_starb_typ( _temp73,
argposn? Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp75=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp75->v=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp75;}), 0):( void*) Cyc_Absyn_HeapRgn, _temp71, _temp69 ==  0?( void*) Cyc_Absyn_Unknown_b:(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp76=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp76[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp77; _temp77.tag= Cyc_Absyn_Upper_b; _temp77.f1=( struct Cyc_Absyn_Exp*)
_check_null( _temp69); _temp77;}); _temp76;})); _LL68: return t; _LL64:;} static
void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(* x).f3= Cyc_Parse_array2ptr((*
x).f3, 1);} struct _tuple11{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static
struct _tuple11* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ return({ struct _tuple11* _temp78=( struct _tuple11*)
_cycalloc( sizeof( struct _tuple11)); _temp78->f1=(* t).f2; _temp78->f2=(* t).f3;
_temp78;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_arr decl_kind;{ void* _temp79=( void*)
x->r; struct Cyc_Absyn_Vardecl* _temp105; _LL81: if(*(( int*) _temp79) ==  Cyc_Absyn_Var_d){
_LL106: _temp105=(( struct Cyc_Absyn_Var_d_struct*) _temp79)->f1; goto _LL82;}
else{ goto _LL83;} _LL83: if(*(( int*) _temp79) ==  Cyc_Absyn_Let_d){ goto _LL84;}
else{ goto _LL85;} _LL85: if(*(( int*) _temp79) ==  Cyc_Absyn_Letv_d){ goto
_LL86;} else{ goto _LL87;} _LL87: if(*(( int*) _temp79) ==  Cyc_Absyn_Fn_d){
goto _LL88;} else{ goto _LL89;} _LL89: if(*(( int*) _temp79) ==  Cyc_Absyn_Struct_d){
goto _LL90;} else{ goto _LL91;} _LL91: if(*(( int*) _temp79) ==  Cyc_Absyn_Union_d){
goto _LL92;} else{ goto _LL93;} _LL93: if(*(( int*) _temp79) ==  Cyc_Absyn_Tunion_d){
goto _LL94;} else{ goto _LL95;} _LL95: if(*(( int*) _temp79) ==  Cyc_Absyn_Typedef_d){
goto _LL96;} else{ goto _LL97;} _LL97: if(*(( int*) _temp79) ==  Cyc_Absyn_Enum_d){
goto _LL98;} else{ goto _LL99;} _LL99: if(*(( int*) _temp79) ==  Cyc_Absyn_Namespace_d){
goto _LL100;} else{ goto _LL101;} _LL101: if(*(( int*) _temp79) ==  Cyc_Absyn_Using_d){
goto _LL102;} else{ goto _LL103;} _LL103: if(*(( int*) _temp79) ==  Cyc_Absyn_ExternC_d){
goto _LL104;} else{ goto _LL80;} _LL82: if( _temp105->initializer !=  0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);} if( Cyc_Absyn_is_qvar_qualified( _temp105->name)){
Cyc_Parse_err( _tag_arr("namespaces not allowed on parameter declarations",
sizeof( unsigned char), 49u), x->loc);}{ int found= 0; for( 0; params !=  0;
params= params->tl){ if( Cyc_Std_zstrptrcmp((* _temp105->name).f2,( struct
_tagged_arr*) params->hd) ==  0){ found= 1; break;}} if( ! found){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp108; _temp108.tag=
Cyc_Std_String_pa; _temp108.f1=( struct _tagged_arr)*(* _temp105->name).f2;{
void* _temp107[ 1u]={& _temp108}; Cyc_Std_aprintf( _tag_arr("%s is not listed as a parameter",
sizeof( unsigned char), 32u), _tag_arr( _temp107, sizeof( void*), 1u));}}), x->loc);}
return;} _LL84: decl_kind= _tag_arr("let declaration", sizeof( unsigned char),
16u); goto _LL80; _LL86: decl_kind= _tag_arr("let declaration", sizeof(
unsigned char), 16u); goto _LL80; _LL88: decl_kind= _tag_arr("function declaration",
sizeof( unsigned char), 21u); goto _LL80; _LL90: decl_kind= _tag_arr("struct declaration",
sizeof( unsigned char), 19u); goto _LL80; _LL92: decl_kind= _tag_arr("union declaration",
sizeof( unsigned char), 18u); goto _LL80; _LL94: decl_kind= _tag_arr("tunion declaration",
sizeof( unsigned char), 19u); goto _LL80; _LL96: decl_kind= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL80; _LL98: decl_kind= _tag_arr("enum declaration",
sizeof( unsigned char), 17u); goto _LL80; _LL100: decl_kind= _tag_arr("namespace declaration",
sizeof( unsigned char), 22u); goto _LL80; _LL102: decl_kind= _tag_arr("using declaration",
sizeof( unsigned char), 18u); goto _LL80; _LL104: decl_kind= _tag_arr("extern C declaration",
sizeof( unsigned char), 21u); goto _LL80; _LL80:;}(( int(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp110; _temp110.tag= Cyc_Std_String_pa;
_temp110.f1=( struct _tagged_arr) decl_kind;{ void* _temp109[ 1u]={& _temp110};
Cyc_Std_aprintf( _tag_arr("%s appears in parameter type", sizeof( unsigned char),
29u), _tag_arr( _temp109, sizeof( void*), 1u));}}), x->loc); return;} static
struct Cyc_Absyn_Vardecl* Cyc_Parse_decl2vardecl( struct Cyc_Absyn_Decl* d){
void* _temp111=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp117; _LL113: if(*((
int*) _temp111) ==  Cyc_Absyn_Var_d){ _LL118: _temp117=(( struct Cyc_Absyn_Var_d_struct*)
_temp111)->f1; goto _LL114;} else{ goto _LL115;} _LL115: goto _LL116; _LL114:
return _temp117; _LL116: return(( struct Cyc_Absyn_Vardecl*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("bad declaration in `forarray' statement",
sizeof( unsigned char), 40u), d->loc); _LL112:;} struct _tuple12{ struct Cyc_List_List*
f1; struct Cyc_Position_Segment* f2; } ; static struct _tuple2* Cyc_Parse_get_param_type(
struct _tuple12* env, struct _tagged_arr* x){ struct _tuple12 _temp121; struct
Cyc_Position_Segment* _temp122; struct Cyc_List_List* _temp124; struct _tuple12*
_temp119= env; _temp121=* _temp119; _LL125: _temp124= _temp121.f1; goto _LL123;
_LL123: _temp122= _temp121.f2; goto _LL120; _LL120: if( _temp124 ==  0){ return((
struct _tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp127; _temp127.tag= Cyc_Std_String_pa; _temp127.f1=( struct _tagged_arr)* x;{
void* _temp126[ 1u]={& _temp127}; Cyc_Std_aprintf( _tag_arr("missing type for parameter %s",
sizeof( unsigned char), 30u), _tag_arr( _temp126, sizeof( void*), 1u));}}),
_temp122);}{ void* _temp128=( void*)(( struct Cyc_Absyn_Decl*) _temp124->hd)->r;
struct Cyc_Absyn_Vardecl* _temp134; _LL130: if(*(( int*) _temp128) ==  Cyc_Absyn_Var_d){
_LL135: _temp134=(( struct Cyc_Absyn_Var_d_struct*) _temp128)->f1; goto _LL131;}
else{ goto _LL132;} _LL132: goto _LL133; _LL131: if( Cyc_Absyn_is_qvar_qualified(
_temp134->name)){ Cyc_Parse_err( _tag_arr("namespace not allowed on parameter",
sizeof( unsigned char), 35u), _temp122);} if( Cyc_Std_zstrptrcmp((* _temp134->name).f2,
x) ==  0){ return({ struct _tuple2* _temp136=( struct _tuple2*) _cycalloc(
sizeof( struct _tuple2)); _temp136->f1=({ struct Cyc_Core_Opt* _temp137=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp137->v=( void*)(*
_temp134->name).f2; _temp137;}); _temp136->f2= _temp134->tq; _temp136->f3=( void*)
_temp134->type; _temp136;});} else{ return Cyc_Parse_get_param_type(({ struct
_tuple12* _temp138=( struct _tuple12*) _cycalloc( sizeof( struct _tuple12));
_temp138->f1= _temp124->tl; _temp138->f2= _temp122; _temp138;}), x);} _LL133:
return(( struct _tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration", sizeof(
unsigned char), 25u),(( struct Cyc_Absyn_Decl*) _temp124->hd)->loc); _LL129:;}}
static int Cyc_Parse_is_typeparam( void* tm){ void* _temp139= tm; _LL141: if((
unsigned int) _temp139 >  1u?*(( int*) _temp139) ==  Cyc_Absyn_TypeParams_mod: 0){
goto _LL142;} else{ goto _LL143;} _LL143: goto _LL144; _LL142: return 1; _LL144:
return 0; _LL140:;} static void* Cyc_Parse_id2type( struct _tagged_arr s, void*
k){ if( Cyc_Std_zstrcmp( s, _tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){
return( void*) Cyc_Absyn_HeapRgn;} else{ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp145=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp145[ 0]=({ struct Cyc_Absyn_VarType_struct _temp146; _temp146.tag= Cyc_Absyn_VarType;
_temp146.f1=({ struct Cyc_Absyn_Tvar* _temp147=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp147->name=({ struct _tagged_arr*
_temp148=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp148[ 0]= s; _temp148;}); _temp147->identity= 0; _temp147->kind=( void*) k;
_temp147;}); _temp146;}); _temp145;});}} static struct Cyc_Absyn_Tvar* Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar* t){ void* k;{ void* _temp149= Cyc_Absyn_compress_kb((
void*) t->kind); void* _temp157; void* _temp159; _LL151: if(*(( int*) _temp149)
==  Cyc_Absyn_Eq_kb){ _LL158: _temp157=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp149)->f1; goto _LL152;} else{ goto _LL153;} _LL153: if(*(( int*) _temp149)
==  Cyc_Absyn_Unknown_kb){ goto _LL154;} else{ goto _LL155;} _LL155: if(*(( int*)
_temp149) ==  Cyc_Absyn_Less_kb){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp149)->f2; goto _LL156;} else{ goto _LL150;} _LL152: k=( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp161=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp161[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp162; _temp162.tag= Cyc_Absyn_Eq_kb;
_temp162.f1=( void*) _temp157; _temp162;}); _temp161;}); goto _LL150; _LL154: k=(
void*)({ struct Cyc_Absyn_Unknown_kb_struct* _temp163=( struct Cyc_Absyn_Unknown_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct)); _temp163[ 0]=({ struct
Cyc_Absyn_Unknown_kb_struct _temp164; _temp164.tag= Cyc_Absyn_Unknown_kb;
_temp164.f1= 0; _temp164;}); _temp163;}); goto _LL150; _LL156: k=( void*)({
struct Cyc_Absyn_Less_kb_struct* _temp165=( struct Cyc_Absyn_Less_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Less_kb_struct)); _temp165[ 0]=({ struct Cyc_Absyn_Less_kb_struct
_temp166; _temp166.tag= Cyc_Absyn_Less_kb; _temp166.f1= 0; _temp166.f2=( void*)
_temp159; _temp166;}); _temp165;}); goto _LL150; _LL150:;} return({ struct Cyc_Absyn_Tvar*
_temp167=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp167->name= t->name; _temp167->identity= 0; _temp167->kind=( void*) k;
_temp167;});} static struct Cyc_Absyn_Tvar* Cyc_Parse_typ2tvar( struct Cyc_Position_Segment*
loc, void* t){ void* _temp168= t; struct Cyc_Absyn_Tvar* _temp174; _LL170: if((
unsigned int) _temp168 >  3u?*(( int*) _temp168) ==  Cyc_Absyn_VarType: 0){
_LL175: _temp174=(( struct Cyc_Absyn_VarType_struct*) _temp168)->f1; goto _LL171;}
else{ goto _LL172;} _LL172: goto _LL173; _LL171: return _temp174; _LL173: return((
struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL169:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp176=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp176[ 0]=({ struct Cyc_Absyn_VarType_struct _temp177; _temp177.tag= Cyc_Absyn_VarType;
_temp177.f1= pr; _temp177;}); _temp176;});} static void Cyc_Parse_set_vartyp_kind(
void* t, void* k){ void* _temp178= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp184; struct Cyc_Absyn_Tvar _temp186; void* _temp187; void** _temp189;
_LL180: if(( unsigned int) _temp178 >  3u?*(( int*) _temp178) ==  Cyc_Absyn_VarType:
0){ _LL185: _temp184=(( struct Cyc_Absyn_VarType_struct*) _temp178)->f1;
_temp186=* _temp184; _LL188: _temp187=( void*) _temp186.kind; _temp189=( void**)&(*((
struct Cyc_Absyn_VarType_struct*) _temp178)->f1).kind; goto _LL181;} else{ goto
_LL182;} _LL182: goto _LL183; _LL181: { void* _temp190= Cyc_Absyn_compress_kb(*
_temp189); void* _temp191= _temp190; _LL193: if(*(( int*) _temp191) ==  Cyc_Absyn_Eq_kb){
goto _LL194;} else{ goto _LL195;} _LL195: if(*(( int*) _temp191) ==  Cyc_Absyn_Less_kb){
goto _LL196;} else{ goto _LL197;} _LL197: if(*(( int*) _temp191) ==  Cyc_Absyn_Unknown_kb){
goto _LL198;} else{ goto _LL192;} _LL194: return; _LL196: return; _LL198:*
_temp189=( void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp199=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp199[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp200; _temp200.tag= Cyc_Absyn_Eq_kb; _temp200.f1=( void*) k; _temp200;});
_temp199;}); return; _LL192:;} _LL183: return; _LL179:;} static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List* tms, struct Cyc_List_List*
tds, struct Cyc_Position_Segment* loc){ if( tds ==  0){ return tms;} if( tms == 
0){ return 0;}{ void* _temp201=( void*) tms->hd; void* _temp207; _LL203: if((
unsigned int) _temp201 >  1u?*(( int*) _temp201) ==  Cyc_Absyn_Function_mod: 0){
_LL208: _temp207=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp201)->f1;
goto _LL204;} else{ goto _LL205;} _LL205: goto _LL206; _LL204: if( tms->tl ==  0?
1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd)?((
struct Cyc_List_List*) _check_null( tms->tl))->tl ==  0: 0)){ void* _temp209=
_temp207; struct Cyc_List_List* _temp215; _LL211: if(*(( int*) _temp209) ==  Cyc_Absyn_WithTypes){
goto _LL212;} else{ goto _LL213;} _LL213: if(*(( int*) _temp209) ==  Cyc_Absyn_NoTypes){
_LL216: _temp215=(( struct Cyc_Absyn_NoTypes_struct*) _temp209)->f1; goto _LL214;}
else{ goto _LL210;} _LL212:({ void* _temp217[ 0u]={}; Cyc_Tcutil_warn( loc,
_tag_arr("function declaration with both new- and old-style parameter declarations; ignoring old-style",
sizeof( unsigned char), 93u), _tag_arr( _temp217, sizeof( void*), 0u));});
return tms; _LL214:(( void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp215, tds);{ struct _tuple12* _temp218=({ struct _tuple12* _temp224=( struct
_tuple12*) _cycalloc( sizeof( struct _tuple12)); _temp224->f1= tds; _temp224->f2=
loc; _temp224;}); return({ struct Cyc_List_List* _temp219=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp219->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp220=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp220[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp221; _temp221.tag= Cyc_Absyn_Function_mod;
_temp221.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp222=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp222[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp223; _temp223.tag= Cyc_Absyn_WithTypes;
_temp223.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp218, _temp215); _temp223.f2= 0; _temp223.f3= 0;
_temp223.f4= 0; _temp223.f5= 0; _temp223;}); _temp222;})); _temp221;}); _temp220;}));
_temp219->tl= 0; _temp219;});} _LL210:;} else{ return({ struct Cyc_List_List*
_temp225=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp225->hd=( void*)(( void*) tms->hd); _temp225->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp225;});} _LL206: return({ struct Cyc_List_List*
_temp226=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp226->hd=( void*)(( void*) tms->hd); _temp226->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp226;}); _LL202:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds != 
0){ d=({ struct Cyc_Parse_Declarator* _temp227=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp227->id= d->id; _temp227->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp227;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso !=  0){ tss=(( struct
Cyc_Parse_Declaration_spec*) dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline;
atts=(( struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct
Cyc_Parse_Declaration_spec*) dso->v)->sc !=  0){ void* _temp228=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*) dso->v)->sc))->v;
_LL230: if( _temp228 == ( void*) Cyc_Parse_Extern_sc){ goto _LL231;} else{ goto
_LL232;} _LL232: if( _temp228 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL233;}
else{ goto _LL234;} _LL234: if( _temp228 == ( void*) Cyc_Parse_Static_sc){ goto
_LL235;} else{ goto _LL236;} _LL236: goto _LL237; _LL231: sc=( void*) Cyc_Absyn_Extern;
goto _LL229; _LL233: sc=( void*) Cyc_Absyn_ExternC; goto _LL229; _LL235: sc=(
void*) Cyc_Absyn_Static; goto _LL229; _LL237: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL229; _LL229:;}}{ struct Cyc_Core_Opt*
_temp240; void* _temp242; struct _tuple5 _temp238= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL243: _temp242= _temp238.f1; goto _LL241; _LL241: _temp240=
_temp238.f2; goto _LL239; _LL239: { struct Cyc_List_List* _temp246; struct Cyc_List_List*
_temp248; void* _temp250; struct Cyc_Absyn_Tqual _temp252; struct _tuple6
_temp244= Cyc_Parse_apply_tms( tq, _temp242, atts, d->tms); _LL253: _temp252=
_temp244.f1; goto _LL251; _LL251: _temp250= _temp244.f2; goto _LL249; _LL249:
_temp248= _temp244.f3; goto _LL247; _LL247: _temp246= _temp244.f4; goto _LL245;
_LL245: if( _temp240 !=  0){({ void* _temp254[ 0u]={}; Cyc_Tcutil_warn( loc,
_tag_arr("nested type declaration within function prototype", sizeof(
unsigned char), 50u), _tag_arr( _temp254, sizeof( void*), 0u));});} if( _temp248
!=  0){({ void* _temp255[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), _tag_arr( _temp255, sizeof( void*), 0u));});}{
void* _temp256= _temp250; struct Cyc_Absyn_FnInfo _temp262; struct Cyc_List_List*
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
Cyc_List_append( _temp264, _temp246); _temp281;});} _LL261: return(( struct Cyc_Absyn_Fndecl*(*)(
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
seen_size: 0){({ void* _temp361[ 0u]={}; Cyc_Tcutil_warn( last_loc, _tag_arr("missing type within specifier",
sizeof( unsigned char), 30u), _tag_arr( _temp361, sizeof( void*), 0u));});} t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp362=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp362[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp363; _temp363.tag= Cyc_Absyn_IntType; _temp363.f1=( void*) sgn; _temp363.f2=(
void*) sz; _temp363;}); _temp362;});} else{ if( seen_sign){ void* _temp364= t;
void* _temp370; _LL366: if(( unsigned int) _temp364 >  3u?*(( int*) _temp364) == 
Cyc_Absyn_IntType: 0){ _LL371: _temp370=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp364)->f2; goto _LL367;} else{ goto _LL368;} _LL368: goto _LL369; _LL367: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp372=( struct Cyc_Absyn_IntType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp372[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp373; _temp373.tag= Cyc_Absyn_IntType; _temp373.f1=( void*) sgn; _temp373.f2=(
void*) _temp370; _temp373;}); _temp372;}); goto _LL365; _LL369: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL365; _LL365:;} if( seen_size){ void* _temp374= t; void*
_temp382; _LL376: if(( unsigned int) _temp374 >  3u?*(( int*) _temp374) ==  Cyc_Absyn_IntType:
0){ _LL383: _temp382=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp374)->f1;
goto _LL377;} else{ goto _LL378;} _LL378: if(( unsigned int) _temp374 >  3u?*((
int*) _temp374) ==  Cyc_Absyn_DoubleType: 0){ goto _LL379;} else{ goto _LL380;}
_LL380: goto _LL381; _LL377: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp384=( struct Cyc_Absyn_IntType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp384[ 0]=({ struct Cyc_Absyn_IntType_struct _temp385; _temp385.tag= Cyc_Absyn_IntType;
_temp385.f1=( void*) _temp382; _temp385.f2=( void*) sz; _temp385;}); _temp384;});
goto _LL375; _LL379: t=( void*)({ struct Cyc_Absyn_DoubleType_struct* _temp386=(
struct Cyc_Absyn_DoubleType_struct*) _cycalloc_atomic( sizeof( struct Cyc_Absyn_DoubleType_struct));
_temp386[ 0]=({ struct Cyc_Absyn_DoubleType_struct _temp387; _temp387.tag= Cyc_Absyn_DoubleType;
_temp387.f1= 1; _temp387;}); _temp386;}); goto _LL375; _LL381: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL375; _LL375:;}} return({ struct _tuple5 _temp388; _temp388.f1=
t; _temp388.f2= declopt; _temp388;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds ==  0){ return 0;}{ struct Cyc_Parse_Declarator* _temp389=(
struct Cyc_Parse_Declarator*) ds->hd; struct _tuple1* _temp390= _temp389->id;
struct Cyc_List_List* _temp393; struct Cyc_List_List* _temp395; void* _temp397;
struct Cyc_Absyn_Tqual _temp399; struct _tuple6 _temp391= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp389->tms); _LL400: _temp399= _temp391.f1; goto _LL398;
_LL398: _temp397= _temp391.f2; goto _LL396; _LL396: _temp395= _temp391.f3; goto
_LL394; _LL394: _temp393= _temp391.f4; goto _LL392; _LL392: return({ struct Cyc_List_List*
_temp401=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp401->hd=( void*)({ struct _tuple7* _temp402=( struct _tuple7*) _cycalloc(
sizeof( struct _tuple7)); _temp402->f1= _temp390; _temp402->f2= _temp399;
_temp402->f3= _temp397; _temp402->f4= _temp395; _temp402->f5= _temp393; _temp402;});
_temp401->tl= Cyc_Parse_apply_tmss( _temp399, Cyc_Tcutil_copy_type( t), ds->tl,
shared_atts); _temp401;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms ==  0){ return({ struct _tuple6 _temp403; _temp403.f1= tq;
_temp403.f2= t; _temp403.f3= 0; _temp403.f4= atts; _temp403;});}{ void* _temp404=(
void*) tms->hd; struct Cyc_Absyn_Exp* _temp418; void* _temp420; struct Cyc_Position_Segment*
_temp422; struct Cyc_List_List* _temp424; struct Cyc_Absyn_Tqual _temp426; void*
_temp428; void* _temp430; struct Cyc_List_List* _temp432; struct Cyc_Position_Segment*
_temp434; _LL406: if( _temp404 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL407;}
else{ goto _LL408;} _LL408: if(( unsigned int) _temp404 >  1u?*(( int*) _temp404)
==  Cyc_Absyn_ConstArray_mod: 0){ _LL419: _temp418=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp404)->f1; goto _LL409;} else{ goto _LL410;} _LL410: if(( unsigned int)
_temp404 >  1u?*(( int*) _temp404) ==  Cyc_Absyn_Function_mod: 0){ _LL421:
_temp420=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp404)->f1; goto
_LL411;} else{ goto _LL412;} _LL412: if(( unsigned int) _temp404 >  1u?*(( int*)
_temp404) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL425: _temp424=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp404)->f1; goto _LL423; _LL423: _temp422=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp404)->f2; goto _LL413;} else{ goto _LL414;} _LL414: if(( unsigned int)
_temp404 >  1u?*(( int*) _temp404) ==  Cyc_Absyn_Pointer_mod: 0){ _LL431:
_temp430=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp404)->f1; goto
_LL429; _LL429: _temp428=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp404)->f2; goto _LL427; _LL427: _temp426=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp404)->f3; goto _LL415;} else{ goto _LL416;} _LL416: if(( unsigned int)
_temp404 >  1u?*(( int*) _temp404) ==  Cyc_Absyn_Attributes_mod: 0){ _LL435:
_temp434=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp404)->f1; goto _LL433;
_LL433: _temp432=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp404)->f2; goto
_LL417;} else{ goto _LL405;} _LL407: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp436=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp436[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp437; _temp437.tag= Cyc_Absyn_ArrayType; _temp437.f1=(
void*) t; _temp437.f2= tq; _temp437.f3= 0; _temp437;}); _temp436;}), atts, tms->tl);
_LL409: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp438=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp438[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp439; _temp439.tag= Cyc_Absyn_ArrayType; _temp439.f1=(
void*) t; _temp439.f2= tq; _temp439.f3=( struct Cyc_Absyn_Exp*) _temp418;
_temp439;}); _temp438;}), atts, tms->tl); _LL411: { void* _temp440= _temp420;
struct Cyc_List_List* _temp446; struct Cyc_Core_Opt* _temp448; struct Cyc_Absyn_VarargInfo*
_temp450; int _temp452; struct Cyc_List_List* _temp454; struct Cyc_Position_Segment*
_temp456; _LL442: if(*(( int*) _temp440) ==  Cyc_Absyn_WithTypes){ _LL455:
_temp454=(( struct Cyc_Absyn_WithTypes_struct*) _temp440)->f1; goto _LL453;
_LL453: _temp452=(( struct Cyc_Absyn_WithTypes_struct*) _temp440)->f2; goto
_LL451; _LL451: _temp450=(( struct Cyc_Absyn_WithTypes_struct*) _temp440)->f3;
goto _LL449; _LL449: _temp448=(( struct Cyc_Absyn_WithTypes_struct*) _temp440)->f4;
goto _LL447; _LL447: _temp446=(( struct Cyc_Absyn_WithTypes_struct*) _temp440)->f5;
goto _LL443;} else{ goto _LL444;} _LL444: if(*(( int*) _temp440) ==  Cyc_Absyn_NoTypes){
_LL457: _temp456=(( struct Cyc_Absyn_NoTypes_struct*) _temp440)->f2; goto _LL445;}
else{ goto _LL441;} _LL443: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as !=  0; as= as->tl){ if( Cyc_Absyn_fntype_att(( void*) as->hd)){
fn_atts=({ struct Cyc_List_List* _temp458=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp458->hd=( void*)(( void*) as->hd); _temp458->tl=
fn_atts; _temp458;});} else{ new_atts=({ struct Cyc_List_List* _temp459=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp459->hd=( void*)((
void*) as->hd); _temp459->tl= new_atts; _temp459;});}}} if( tms->tl !=  0){ void*
_temp460=( void*)(( struct Cyc_List_List*) _check_null( tms->tl))->hd; struct
Cyc_List_List* _temp466; _LL462: if(( unsigned int) _temp460 >  1u?*(( int*)
_temp460) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL467: _temp466=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp460)->f1; goto _LL463;} else{ goto _LL464;} _LL464: goto _LL465; _LL463:
typvars= _temp466; tms= tms->tl; goto _LL461; _LL465: goto _LL461; _LL461:;} if(((((
! _temp452? _temp450 ==  0: 0)? _temp454 !=  0: 0)? _temp454->tl ==  0: 0)?(*((
struct _tuple2*) _temp454->hd)).f1 ==  0: 0)?(*(( struct _tuple2*) _temp454->hd)).f3
== ( void*) Cyc_Absyn_VoidType: 0){ _temp454= 0;} t= Cyc_Parse_array2ptr( t, 0);((
void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Parse_arg_array2ptr, _temp454); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),
Cyc_Absyn_function_typ( typvars, _temp448, t, _temp454, _temp452, _temp450,
_temp446, fn_atts), new_atts,(( struct Cyc_List_List*) _check_null( tms))->tl);}
_LL445:( int) _throw( Cyc_Parse_abort( _tag_arr("function declaration without parameter types",
sizeof( unsigned char), 45u), _temp456)); _LL441:;} _LL413: if( tms->tl ==  0){
return({ struct _tuple6 _temp468; _temp468.f1= tq; _temp468.f2= t; _temp468.f3=
_temp424; _temp468.f4= atts; _temp468;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp422)); _LL415: { void* _temp469= _temp430;
struct Cyc_Absyn_Exp* _temp477; struct Cyc_Absyn_Exp* _temp479; _LL471: if((
unsigned int) _temp469 >  1u?*(( int*) _temp469) ==  Cyc_Absyn_NonNullable_ps: 0){
_LL478: _temp477=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp469)->f1; goto
_LL472;} else{ goto _LL473;} _LL473: if(( unsigned int) _temp469 >  1u?*(( int*)
_temp469) ==  Cyc_Absyn_Nullable_ps: 0){ _LL480: _temp479=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp469)->f1; goto _LL474;} else{ goto _LL475;} _LL475: if( _temp469 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL476;} else{ goto _LL470;} _LL472: return Cyc_Parse_apply_tms(
_temp426, Cyc_Absyn_atb_typ( t, _temp428, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp481=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp481[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp482; _temp482.tag= Cyc_Absyn_Upper_b;
_temp482.f1= _temp477; _temp482;}); _temp481;})), atts, tms->tl); _LL474: return
Cyc_Parse_apply_tms( _temp426, Cyc_Absyn_starb_typ( t, _temp428, tq,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp483=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp483[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp484; _temp484.tag= Cyc_Absyn_Upper_b; _temp484.f1= _temp479; _temp484;});
_temp483;})), atts, tms->tl); _LL476: return Cyc_Parse_apply_tms( _temp426, Cyc_Absyn_tagged_typ(
t, _temp428, tq), atts, tms->tl); _LL470:;} _LL417: return Cyc_Parse_apply_tms(
tq, t, Cyc_List_append( atts, _temp432), tms->tl); _LL405:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp487; void* _temp489; struct _tuple5 _temp485= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL490: _temp489= _temp485.f1; goto _LL488; _LL488: _temp487=
_temp485.f2; goto _LL486; _LL486: if( _temp487 !=  0){({ void* _temp491[ 0u]={};
Cyc_Tcutil_warn( loc, _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), _tag_arr( _temp491, sizeof( void*), 0u));});}
return _temp489;} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct
Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Decl_s_struct* _temp492=( struct Cyc_Absyn_Decl_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct)); _temp492[ 0]=({ struct Cyc_Absyn_Decl_s_struct
_temp493; _temp493.tag= Cyc_Absyn_Decl_s; _temp493.f1= d; _temp493.f2= s;
_temp493;}); _temp492;}), Cyc_Position_segment_join( d->loc, s->loc));} static
struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations( struct Cyc_List_List* ds,
struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)( struct Cyc_Absyn_Stmt*(*
f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*), struct Cyc_List_List* x,
struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)( Cyc_Parse_flatten_decl, ds,
s);} static struct Cyc_List_List* Cyc_Parse_make_declarations( struct Cyc_Parse_Declaration_spec*
ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment* loc){ struct Cyc_List_List*
tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq; int istypedef= 0; void*
s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts= ds->attributes; if( ds->is_inline){
Cyc_Parse_err( _tag_arr("inline is only allowed on function definitions",
sizeof( unsigned char), 47u), loc);} if( tss ==  0){ Cyc_Parse_err( _tag_arr("missing type specifiers in declaration",
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc !=  0){ void* _temp494=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL496: if( _temp494 == (
void*) Cyc_Parse_Typedef_sc){ goto _LL497;} else{ goto _LL498;} _LL498: if(
_temp494 == ( void*) Cyc_Parse_Extern_sc){ goto _LL499;} else{ goto _LL500;}
_LL500: if( _temp494 == ( void*) Cyc_Parse_ExternC_sc){ goto _LL501;} else{ goto
_LL502;} _LL502: if( _temp494 == ( void*) Cyc_Parse_Static_sc){ goto _LL503;}
else{ goto _LL504;} _LL504: if( _temp494 == ( void*) Cyc_Parse_Auto_sc){ goto
_LL505;} else{ goto _LL506;} _LL506: if( _temp494 == ( void*) Cyc_Parse_Register_sc){
goto _LL507;} else{ goto _LL508;} _LL508: if( _temp494 == ( void*) Cyc_Parse_Abstract_sc){
goto _LL509;} else{ goto _LL495;} _LL497: istypedef= 1; goto _LL495; _LL499: s=(
void*) Cyc_Absyn_Extern; goto _LL495; _LL501: s=( void*) Cyc_Absyn_ExternC; goto
_LL495; _LL503: s=( void*) Cyc_Absyn_Static; goto _LL495; _LL505: s=( void*) Cyc_Absyn_Public;
goto _LL495; _LL507: s=( void*) Cyc_Absyn_Public; goto _LL495; _LL509: s=( void*)
Cyc_Absyn_Abstract; goto _LL495; _LL495:;}{ struct Cyc_List_List* _temp512;
struct Cyc_List_List* _temp514; struct _tuple0 _temp510=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL515: _temp514= _temp510.f1;
goto _LL513; _LL513: _temp512= _temp510.f2; goto _LL511; _LL511: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp512; for( 0; es !=  0; es= es->tl){
if(( struct Cyc_Absyn_Exp*) es->hd !=  0){ exps_empty= 0; break;}}}{ struct
_tuple5 _temp516= Cyc_Parse_collapse_type_specifiers( tss, loc); if( _temp514 == 
0){ struct Cyc_Core_Opt* _temp519; void* _temp521; struct _tuple5 _temp517=
_temp516; _LL522: _temp521= _temp517.f1; goto _LL520; _LL520: _temp519= _temp517.f2;
goto _LL518; _LL518: if( _temp519 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
_temp519->v;{ void* _temp523=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp535;
struct Cyc_Absyn_Structdecl* _temp537; struct Cyc_Absyn_Uniondecl* _temp539;
struct Cyc_Absyn_Tuniondecl* _temp541; _LL525: if(*(( int*) _temp523) ==  Cyc_Absyn_Enum_d){
_LL536: _temp535=(( struct Cyc_Absyn_Enum_d_struct*) _temp523)->f1; goto _LL526;}
else{ goto _LL527;} _LL527: if(*(( int*) _temp523) ==  Cyc_Absyn_Struct_d){
_LL538: _temp537=(( struct Cyc_Absyn_Struct_d_struct*) _temp523)->f1; goto
_LL528;} else{ goto _LL529;} _LL529: if(*(( int*) _temp523) ==  Cyc_Absyn_Union_d){
_LL540: _temp539=(( struct Cyc_Absyn_Union_d_struct*) _temp523)->f1; goto _LL530;}
else{ goto _LL531;} _LL531: if(*(( int*) _temp523) ==  Cyc_Absyn_Tunion_d){
_LL542: _temp541=(( struct Cyc_Absyn_Tunion_d_struct*) _temp523)->f1; goto
_LL532;} else{ goto _LL533;} _LL533: goto _LL534; _LL526:( void*)( _temp535->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum",
sizeof( unsigned char), 23u), loc);} goto _LL524; _LL528:( void*)( _temp537->sc=(
void*) s); _temp537->attributes= atts; goto _LL524; _LL530:( void*)( _temp539->sc=(
void*) s); _temp539->attributes= atts; goto _LL524; _LL532:( void*)( _temp541->sc=(
void*) s); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL524; _LL534: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL524:;}
return({ struct Cyc_List_List* _temp543=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp543->hd=( void*) d; _temp543->tl= 0;
_temp543;});} else{ void* _temp544= _temp521; struct Cyc_List_List* _temp560;
struct _tuple1* _temp562; struct Cyc_Absyn_TunionInfo _temp564; void* _temp566;
struct Cyc_Absyn_Tuniondecl** _temp568; struct Cyc_Absyn_TunionInfo _temp570;
struct Cyc_List_List* _temp572; void* _temp574; struct Cyc_Absyn_UnknownTunionInfo
_temp576; int _temp578; struct _tuple1* _temp580; struct Cyc_List_List* _temp582;
struct _tuple1* _temp584; struct _tuple1* _temp586; struct Cyc_List_List*
_temp588; _LL546: if(( unsigned int) _temp544 >  3u?*(( int*) _temp544) ==  Cyc_Absyn_StructType:
0){ _LL563: _temp562=(( struct Cyc_Absyn_StructType_struct*) _temp544)->f1; goto
_LL561; _LL561: _temp560=(( struct Cyc_Absyn_StructType_struct*) _temp544)->f2;
goto _LL547;} else{ goto _LL548;} _LL548: if(( unsigned int) _temp544 >  3u?*((
int*) _temp544) ==  Cyc_Absyn_TunionType: 0){ _LL565: _temp564=(( struct Cyc_Absyn_TunionType_struct*)
_temp544)->f1; _LL567: _temp566=( void*) _temp564.tunion_info; if(*(( int*)
_temp566) ==  Cyc_Absyn_KnownTunion){ _LL569: _temp568=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp566)->f1; goto _LL549;} else{ goto _LL550;}} else{ goto _LL550;} _LL550:
if(( unsigned int) _temp544 >  3u?*(( int*) _temp544) ==  Cyc_Absyn_TunionType:
0){ _LL571: _temp570=(( struct Cyc_Absyn_TunionType_struct*) _temp544)->f1;
_LL575: _temp574=( void*) _temp570.tunion_info; if(*(( int*) _temp574) ==  Cyc_Absyn_UnknownTunion){
_LL577: _temp576=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp574)->f1;
_LL581: _temp580= _temp576.name; goto _LL579; _LL579: _temp578= _temp576.is_xtunion;
goto _LL573;} else{ goto _LL552;} _LL573: _temp572= _temp570.targs; goto _LL551;}
else{ goto _LL552;} _LL552: if(( unsigned int) _temp544 >  3u?*(( int*) _temp544)
==  Cyc_Absyn_UnionType: 0){ _LL585: _temp584=(( struct Cyc_Absyn_UnionType_struct*)
_temp544)->f1; goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_UnionType_struct*)
_temp544)->f2; goto _LL553;} else{ goto _LL554;} _LL554: if(( unsigned int)
_temp544 >  3u?*(( int*) _temp544) ==  Cyc_Absyn_EnumType: 0){ _LL587: _temp586=((
struct Cyc_Absyn_EnumType_struct*) _temp544)->f1; goto _LL555;} else{ goto
_LL556;} _LL556: if(( unsigned int) _temp544 >  3u?*(( int*) _temp544) ==  Cyc_Absyn_AnonEnumType:
0){ _LL589: _temp588=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp544)->f1;
goto _LL557;} else{ goto _LL558;} _LL558: goto _LL559; _LL547: { struct Cyc_List_List*
_temp590=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp560); struct Cyc_Absyn_Structdecl* _temp591=({
struct Cyc_Absyn_Structdecl* _temp595=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp595->sc=( void*) s; _temp595->name=({
struct Cyc_Core_Opt* _temp596=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp596->v=( void*)(( struct _tuple1*) _check_null( _temp562));
_temp596;}); _temp595->tvs= _temp590; _temp595->fields= 0; _temp595->attributes=
0; _temp595;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp592=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp592->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp593=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp593[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp594; _temp594.tag= Cyc_Absyn_Struct_d;
_temp594.f1= _temp591; _temp594;}); _temp593;}), loc); _temp592->tl= 0; _temp592;});}
_LL549: if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp597=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp597->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp598=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp598[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp599; _temp599.tag= Cyc_Absyn_Tunion_d;
_temp599.f1=* _temp568; _temp599;}); _temp598;}), loc); _temp597->tl= 0;
_temp597;}); _LL551: { struct Cyc_List_List* _temp600=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp572); struct Cyc_Absyn_Decl* _temp601= Cyc_Absyn_tunion_decl( s, _temp580,
_temp600, 0, _temp578, loc); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp602=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp602->hd=(
void*) _temp601; _temp602->tl= 0; _temp602;});} _LL553: { struct Cyc_List_List*
_temp603=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp582); struct Cyc_Absyn_Uniondecl* _temp604=({
struct Cyc_Absyn_Uniondecl* _temp609=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp609->sc=( void*) s; _temp609->name=({
struct Cyc_Core_Opt* _temp610=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp610->v=( void*)(( struct _tuple1*) _check_null( _temp584));
_temp610;}); _temp609->tvs= _temp603; _temp609->fields= 0; _temp609->attributes=
0; _temp609;}); if( atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp605=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp605->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp606=( struct Cyc_Absyn_Decl*) _cycalloc(
sizeof( struct Cyc_Absyn_Decl)); _temp606->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp607=( struct Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp607[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp608; _temp608.tag= Cyc_Absyn_Union_d;
_temp608.f1= _temp604; _temp608;}); _temp607;})); _temp606->loc= loc; _temp606;});
_temp605->tl= 0; _temp605;});} _LL555: { struct Cyc_Absyn_Enumdecl* _temp611=({
struct Cyc_Absyn_Enumdecl* _temp616=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp616->sc=( void*) s; _temp616->name=
_temp586; _temp616->fields= 0; _temp616;}); if( atts !=  0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp612=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp612->hd=( void*)({ struct Cyc_Absyn_Decl* _temp613=(
struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp613->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp614=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp614[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp615; _temp615.tag= Cyc_Absyn_Enum_d; _temp615.f1= _temp611; _temp615;});
_temp614;})); _temp613->loc= loc; _temp613;}); _temp612->tl= 0; _temp612;});}
_LL557: { struct Cyc_Absyn_Enumdecl* _temp617=({ struct Cyc_Absyn_Enumdecl*
_temp622=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp622->sc=( void*) s; _temp622->name= Cyc_Parse_gensym_enum(); _temp622->fields=({
struct Cyc_Core_Opt* _temp623=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp623->v=( void*) _temp588; _temp623;}); _temp622;}); if(
atts !=  0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} return({ struct Cyc_List_List* _temp618=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp618->hd=( void*)({
struct Cyc_Absyn_Decl* _temp619=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp619->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp620=( struct Cyc_Absyn_Enum_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp620[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp621; _temp621.tag= Cyc_Absyn_Enum_d;
_temp621.f1= _temp617; _temp621;}); _temp620;})); _temp619->loc= loc; _temp619;});
_temp618->tl= 0; _temp618;});} _LL559: Cyc_Parse_err( _tag_arr("missing declarator",
sizeof( unsigned char), 19u), loc); return 0; _LL545:;}} else{ void* t= _temp516.f1;
struct Cyc_List_List* _temp624= Cyc_Parse_apply_tmss( tq, t, _temp514, atts);
if( istypedef){ if( ! exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp624); if( _temp516.f2 !=  0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(
_temp516.f2)->v;{ void* _temp625=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp637; struct Cyc_Absyn_Tuniondecl* _temp639; struct Cyc_Absyn_Uniondecl*
_temp641; struct Cyc_Absyn_Enumdecl* _temp643; _LL627: if(*(( int*) _temp625) == 
Cyc_Absyn_Struct_d){ _LL638: _temp637=(( struct Cyc_Absyn_Struct_d_struct*)
_temp625)->f1; goto _LL628;} else{ goto _LL629;} _LL629: if(*(( int*) _temp625)
==  Cyc_Absyn_Tunion_d){ _LL640: _temp639=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp625)->f1; goto _LL630;} else{ goto _LL631;} _LL631: if(*(( int*) _temp625)
==  Cyc_Absyn_Union_d){ _LL642: _temp641=(( struct Cyc_Absyn_Union_d_struct*)
_temp625)->f1; goto _LL632;} else{ goto _LL633;} _LL633: if(*(( int*) _temp625)
==  Cyc_Absyn_Enum_d){ _LL644: _temp643=(( struct Cyc_Absyn_Enum_d_struct*)
_temp625)->f1; goto _LL634;} else{ goto _LL635;} _LL635: goto _LL636; _LL628:(
void*)( _temp637->sc=( void*) s); _temp637->attributes= atts; atts= 0; goto
_LL626; _LL630:( void*)( _temp639->sc=( void*) s); goto _LL626; _LL632:( void*)(
_temp641->sc=( void*) s); goto _LL626; _LL634:( void*)( _temp643->sc=( void*) s);
goto _LL626; _LL636: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL626; _LL626:;} decls=({ struct Cyc_List_List*
_temp645=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp645->hd=( void*) d; _temp645->tl= decls; _temp645;});} if( atts !=  0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp647; _temp647.tag=
Cyc_Std_String_pa; _temp647.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) atts->hd);{ void* _temp646[ 1u]={& _temp647}; Cyc_Std_aprintf( _tag_arr("bad attribute %s in typedef",
sizeof( unsigned char), 28u), _tag_arr( _temp646, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp516.f2 !=  0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp648= _temp624; for( 0; _temp648 !=  0;( _temp648= _temp648->tl,
_temp512= _temp512->tl)){ struct _tuple7 _temp651; struct Cyc_List_List*
_temp652; struct Cyc_List_List* _temp654; void* _temp656; struct Cyc_Absyn_Tqual
_temp658; struct _tuple1* _temp660; struct _tuple7* _temp649=( struct _tuple7*)
_temp648->hd; _temp651=* _temp649; _LL661: _temp660= _temp651.f1; goto _LL659;
_LL659: _temp658= _temp651.f2; goto _LL657; _LL657: _temp656= _temp651.f3; goto
_LL655; _LL655: _temp654= _temp651.f4; goto _LL653; _LL653: _temp652= _temp651.f5;
goto _LL650; _LL650: if( _temp654 !=  0){({ void* _temp662[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
_tag_arr( _temp662, sizeof( void*), 0u));});} if( _temp512 ==  0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("unexpected NULL in parse!", sizeof( unsigned char), 26u), loc);}{
struct Cyc_Absyn_Exp* _temp663=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp512))->hd; struct Cyc_Absyn_Vardecl* _temp664= Cyc_Absyn_new_vardecl(
_temp660, _temp656, _temp663); _temp664->tq= _temp658;( void*)( _temp664->sc=(
void*) s); _temp664->attributes= _temp652;{ struct Cyc_Absyn_Decl* _temp665=({
struct Cyc_Absyn_Decl* _temp667=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp667->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp668=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp668[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp669; _temp669.tag= Cyc_Absyn_Var_d;
_temp669.f1= _temp664; _temp669;}); _temp668;})); _temp667->loc= loc; _temp667;});
decls=({ struct Cyc_List_List* _temp666=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp666->hd=( void*) _temp665; _temp666->tl=
decls; _temp666;});}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind( struct
_tagged_arr s, struct Cyc_Position_Segment* loc){ if( Cyc_Std_strlen( s) !=  1){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp671;
_temp671.tag= Cyc_Std_String_pa; _temp671.f1=( struct _tagged_arr) s;{ void*
_temp670[ 1u]={& _temp671}; Cyc_Std_aprintf( _tag_arr("bad kind: %s", sizeof(
unsigned char), 13u), _tag_arr( _temp670, sizeof( void*), 1u));}}), loc); return(
void*) Cyc_Absyn_BoxKind;} else{ switch(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0))){ case 'A': _LL672:
return( void*) Cyc_Absyn_AnyKind; case 'M': _LL673: return( void*) Cyc_Absyn_MemKind;
case 'B': _LL674: return( void*) Cyc_Absyn_BoxKind; case 'R': _LL675: return(
void*) Cyc_Absyn_RgnKind; case 'E': _LL676: return( void*) Cyc_Absyn_EffKind;
default: _LL677: Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp680; _temp680.tag= Cyc_Std_String_pa; _temp680.f1=( struct _tagged_arr) s;{
void* _temp679[ 1u]={& _temp680}; Cyc_Std_aprintf( _tag_arr("bad kind: %s",
sizeof( unsigned char), 13u), _tag_arr( _temp679, sizeof( void*), 1u));}}), loc);
return( void*) Cyc_Absyn_BoxKind;}}} static struct Cyc_List_List* Cyc_Parse_attopt_to_tms(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( atts ==  0){ return tms;} else{ return({ struct Cyc_List_List*
_temp681=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp681->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp682=( struct Cyc_Absyn_Attributes_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp682[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp683; _temp683.tag= Cyc_Absyn_Attributes_mod; _temp683.f1= loc; _temp683.f2=
atts; _temp683;}); _temp682;})); _temp681->tl= tms; _temp681;});}} static struct
Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment* loc,
struct _tuple7* t){ struct _tuple7 _temp686; struct Cyc_List_List* _temp687;
struct Cyc_List_List* _temp689; void* _temp691; struct Cyc_Absyn_Tqual _temp693;
struct _tuple1* _temp695; struct _tuple7* _temp684= t; _temp686=* _temp684;
_LL696: _temp695= _temp686.f1; goto _LL694; _LL694: _temp693= _temp686.f2; goto
_LL692; _LL692: _temp691= _temp686.f3; goto _LL690; _LL690: _temp689= _temp686.f4;
goto _LL688; _LL688: _temp687= _temp686.f5; goto _LL685; _LL685: Cyc_Lex_register_typedef(
_temp695); if( _temp687 !=  0){ Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp698; _temp698.tag= Cyc_Std_String_pa; _temp698.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) _temp687->hd);{ void* _temp697[ 1u]={& _temp698}; Cyc_Std_aprintf(
_tag_arr("bad attribute %s within typedef", sizeof( unsigned char), 32u),
_tag_arr( _temp697, sizeof( void*), 1u));}}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp699=( struct Cyc_Absyn_Typedef_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp699[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp700; _temp700.tag= Cyc_Absyn_Typedef_d; _temp700.f1=({
struct Cyc_Absyn_Typedefdecl* _temp701=( struct Cyc_Absyn_Typedefdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp701->name= _temp695;
_temp701->tvs= _temp689; _temp701->defn=( void*) _temp691; _temp701;}); _temp700;});
_temp699;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
unsigned char _temp703[ 15u]="$(sign_t,int)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp703, _temp703, _temp703 +  15u}};
struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ struct _tuple15* yyzzz;{ void*
_temp704= yy1; struct _tuple15* _temp710; _LL706: if(*(( void**) _temp704) == 
Cyc_Int_tok){ _LL711: _temp710=(( struct Cyc_Int_tok_struct*) _temp704)->f1;
goto _LL707;} else{ goto _LL708;} _LL708: goto _LL709; _LL707: yyzzz= _temp710;
goto _LL705; _LL709:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL705:;}
return yyzzz;} static unsigned char _temp713[ 9u]="string_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp713, _temp713, _temp713 +  9u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ struct _tagged_arr yyzzz;{
void* _temp714= yy1; struct _tagged_arr _temp720; _LL716: if(*(( void**)
_temp714) ==  Cyc_String_tok){ _LL721: _temp720=(( struct Cyc_String_tok_struct*)
_temp714)->f1; goto _LL717;} else{ goto _LL718;} _LL718: goto _LL719; _LL717:
yyzzz= _temp720; goto _LL715; _LL719:( int) _throw(( void*)& Cyc_yyfail_String_tok);
_LL715:;} return yyzzz;} static unsigned char _temp723[ 5u]="char"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{ _temp723,
_temp723, _temp723 +  5u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
unsigned char yyzzz;{ void* _temp724= yy1; unsigned char _temp730; _LL726: if(*((
void**) _temp724) ==  Cyc_Char_tok){ _LL731: _temp730=(( struct Cyc_Char_tok_struct*)
_temp724)->f1; goto _LL727;} else{ goto _LL728;} _LL728: goto _LL729; _LL727:
yyzzz= _temp730; goto _LL725; _LL729:( int) _throw(( void*)& Cyc_yyfail_Char_tok);
_LL725:;} return yyzzz;} static unsigned char _temp733[ 20u]="tunion Pointer_Sort";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{
_temp733, _temp733, _temp733 +  20u}}; void* Cyc_yyget_Pointer_Sort_tok( void*
yy1){ void* yyzzz;{ void* _temp734= yy1; void* _temp740; _LL736: if(*(( void**)
_temp734) ==  Cyc_Pointer_Sort_tok){ _LL741: _temp740=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp734)->f1; goto _LL737;} else{ goto _LL738;} _LL738: goto _LL739; _LL737:
yyzzz= _temp740; goto _LL735; _LL739:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL735:;} return yyzzz;} static unsigned char _temp743[ 6u]="exp_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp743,
_temp743, _temp743 +  6u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
struct Cyc_Absyn_Exp* yyzzz;{ void* _temp744= yy1; struct Cyc_Absyn_Exp*
_temp750; _LL746: if(*(( void**) _temp744) ==  Cyc_Exp_tok){ _LL751: _temp750=((
struct Cyc_Exp_tok_struct*) _temp744)->f1; goto _LL747;} else{ goto _LL748;}
_LL748: goto _LL749; _LL747: yyzzz= _temp750; goto _LL745; _LL749:( int) _throw((
void*)& Cyc_yyfail_Exp_tok); _LL745:;} return yyzzz;} static unsigned char
_temp753[ 14u]="list_t<exp_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure,{ _temp753, _temp753, _temp753 +  14u}}; struct Cyc_List_List*
Cyc_yyget_ExpList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp754=
yy1; struct Cyc_List_List* _temp760; _LL756: if(*(( void**) _temp754) ==  Cyc_ExpList_tok){
_LL761: _temp760=(( struct Cyc_ExpList_tok_struct*) _temp754)->f1; goto _LL757;}
else{ goto _LL758;} _LL758: goto _LL759; _LL757: yyzzz= _temp760; goto _LL755;
_LL759:( int) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL755:;} return yyzzz;}
static unsigned char _temp763[ 39u]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{
_temp763, _temp763, _temp763 +  39u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp764= yy1; struct Cyc_List_List*
_temp770; _LL766: if(*(( void**) _temp764) ==  Cyc_InitializerList_tok){ _LL771:
_temp770=(( struct Cyc_InitializerList_tok_struct*) _temp764)->f1; goto _LL767;}
else{ goto _LL768;} _LL768: goto _LL769; _LL767: yyzzz= _temp770; goto _LL765;
_LL769:( int) _throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL765:;} return
yyzzz;} static unsigned char _temp773[ 9u]="primop_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp773, _temp773, _temp773 +  9u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* yyzzz;{ void* _temp774= yy1; void*
_temp780; _LL776: if(*(( void**) _temp774) ==  Cyc_Primop_tok){ _LL781: _temp780=(
void*)(( struct Cyc_Primop_tok_struct*) _temp774)->f1; goto _LL777;} else{ goto
_LL778;} _LL778: goto _LL779; _LL777: yyzzz= _temp780; goto _LL775; _LL779:( int)
_throw(( void*)& Cyc_yyfail_Primop_tok); _LL775:;} return yyzzz;} static
unsigned char _temp783[ 16u]="opt_t<primop_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp783, _temp783, _temp783 +  16u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ struct Cyc_Core_Opt*
yyzzz;{ void* _temp784= yy1; struct Cyc_Core_Opt* _temp790; _LL786: if(*(( void**)
_temp784) ==  Cyc_Primopopt_tok){ _LL791: _temp790=(( struct Cyc_Primopopt_tok_struct*)
_temp784)->f1; goto _LL787;} else{ goto _LL788;} _LL788: goto _LL789; _LL787:
yyzzz= _temp790; goto _LL785; _LL789:( int) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL785:;} return yyzzz;} static unsigned char _temp793[ 7u]="qvar_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{
_temp793, _temp793, _temp793 +  7u}}; struct _tuple1* Cyc_yyget_QualId_tok( void*
yy1){ struct _tuple1* yyzzz;{ void* _temp794= yy1; struct _tuple1* _temp800;
_LL796: if(*(( void**) _temp794) ==  Cyc_QualId_tok){ _LL801: _temp800=(( struct
Cyc_QualId_tok_struct*) _temp794)->f1; goto _LL797;} else{ goto _LL798;} _LL798:
goto _LL799; _LL797: yyzzz= _temp800; goto _LL795; _LL799:( int) _throw(( void*)&
Cyc_yyfail_QualId_tok); _LL795:;} return yyzzz;} static unsigned char _temp803[
7u]="stmt_t"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{
_temp803, _temp803, _temp803 +  7u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok(
void* yy1){ struct Cyc_Absyn_Stmt* yyzzz;{ void* _temp804= yy1; struct Cyc_Absyn_Stmt*
_temp810; _LL806: if(*(( void**) _temp804) ==  Cyc_Stmt_tok){ _LL811: _temp810=((
struct Cyc_Stmt_tok_struct*) _temp804)->f1; goto _LL807;} else{ goto _LL808;}
_LL808: goto _LL809; _LL807: yyzzz= _temp810; goto _LL805; _LL809:( int) _throw((
void*)& Cyc_yyfail_Stmt_tok); _LL805:;} return yyzzz;} static unsigned char
_temp813[ 24u]="list_t<switch_clause_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp813, _temp813,
_temp813 +  24u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ struct Cyc_List_List* yyzzz;{ void* _temp814= yy1; struct Cyc_List_List*
_temp820; _LL816: if(*(( void**) _temp814) ==  Cyc_SwitchClauseList_tok){ _LL821:
_temp820=(( struct Cyc_SwitchClauseList_tok_struct*) _temp814)->f1; goto _LL817;}
else{ goto _LL818;} _LL818: goto _LL819; _LL817: yyzzz= _temp820; goto _LL815;
_LL819:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL815:;}
return yyzzz;} static unsigned char _temp823[ 25u]="list_t<switchC_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp823, _temp823, _temp823 +  25u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp824= yy1; struct Cyc_List_List*
_temp830; _LL826: if(*(( void**) _temp824) ==  Cyc_SwitchCClauseList_tok){
_LL831: _temp830=(( struct Cyc_SwitchCClauseList_tok_struct*) _temp824)->f1;
goto _LL827;} else{ goto _LL828;} _LL828: goto _LL829; _LL827: yyzzz= _temp830;
goto _LL825; _LL829:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL825:;} return yyzzz;} static unsigned char _temp833[ 6u]="pat_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{
_temp833, _temp833, _temp833 +  6u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
void* yy1){ struct Cyc_Absyn_Pat* yyzzz;{ void* _temp834= yy1; struct Cyc_Absyn_Pat*
_temp840; _LL836: if(*(( void**) _temp834) ==  Cyc_Pattern_tok){ _LL841:
_temp840=(( struct Cyc_Pattern_tok_struct*) _temp834)->f1; goto _LL837;} else{
goto _LL838;} _LL838: goto _LL839; _LL837: yyzzz= _temp840; goto _LL835; _LL839:(
int) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL835:;} return yyzzz;} static
unsigned char _temp843[ 14u]="list_t<pat_t>"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp843, _temp843, _temp843 + 
14u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ struct Cyc_List_List*
yyzzz;{ void* _temp844= yy1; struct Cyc_List_List* _temp850; _LL846: if(*(( void**)
_temp844) ==  Cyc_PatternList_tok){ _LL851: _temp850=(( struct Cyc_PatternList_tok_struct*)
_temp844)->f1; goto _LL847;} else{ goto _LL848;} _LL848: goto _LL849; _LL847:
yyzzz= _temp850; goto _LL845; _LL849:( int) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL845:;} return yyzzz;} static unsigned char _temp853[ 31u]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp853, _temp853, _temp853 +  31u}}; struct _tuple13* Cyc_yyget_FieldPattern_tok(
void* yy1){ struct _tuple13* yyzzz;{ void* _temp854= yy1; struct _tuple13*
_temp860; _LL856: if(*(( void**) _temp854) ==  Cyc_FieldPattern_tok){ _LL861:
_temp860=(( struct Cyc_FieldPattern_tok_struct*) _temp854)->f1; goto _LL857;}
else{ goto _LL858;} _LL858: goto _LL859; _LL857: yyzzz= _temp860; goto _LL855;
_LL859:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok); _LL855:;} return
yyzzz;} static unsigned char _temp863[ 39u]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp863, _temp863, _temp863 +  39u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp864= yy1; struct Cyc_List_List*
_temp870; _LL866: if(*(( void**) _temp864) ==  Cyc_FieldPatternList_tok){ _LL871:
_temp870=(( struct Cyc_FieldPatternList_tok_struct*) _temp864)->f1; goto _LL867;}
else{ goto _LL868;} _LL868: goto _LL869; _LL867: yyzzz= _temp870; goto _LL865;
_LL869:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL865:;}
return yyzzz;} static unsigned char _temp873[ 9u]="fndecl_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp873, _temp873, _temp873 +  9u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ struct Cyc_Absyn_Fndecl*
yyzzz;{ void* _temp874= yy1; struct Cyc_Absyn_Fndecl* _temp880; _LL876: if(*((
void**) _temp874) ==  Cyc_FnDecl_tok){ _LL881: _temp880=(( struct Cyc_FnDecl_tok_struct*)
_temp874)->f1; goto _LL877;} else{ goto _LL878;} _LL878: goto _LL879; _LL877:
yyzzz= _temp880; goto _LL875; _LL879:( int) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL875:;} return yyzzz;} static unsigned char _temp883[ 15u]="list_t<decl_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp883, _temp883, _temp883 +  15u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp884= yy1; struct Cyc_List_List*
_temp890; _LL886: if(*(( void**) _temp884) ==  Cyc_DeclList_tok){ _LL891:
_temp890=(( struct Cyc_DeclList_tok_struct*) _temp884)->f1; goto _LL887;} else{
goto _LL888;} _LL888: goto _LL889; _LL887: yyzzz= _temp890; goto _LL885; _LL889:(
int) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL885:;} return yyzzz;} static
unsigned char _temp893[ 12u]="decl_spec_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp893, _temp893, _temp893 +  12u}};
struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){ struct
Cyc_Parse_Declaration_spec* yyzzz;{ void* _temp894= yy1; struct Cyc_Parse_Declaration_spec*
_temp900; _LL896: if(*(( void**) _temp894) ==  Cyc_DeclSpec_tok){ _LL901:
_temp900=(( struct Cyc_DeclSpec_tok_struct*) _temp894)->f1; goto _LL897;} else{
goto _LL898;} _LL898: goto _LL899; _LL897: yyzzz= _temp900; goto _LL895; _LL899:(
int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok); _LL895:;} return yyzzz;} static
unsigned char _temp903[ 27u]="$(declarator_t,exp_opt_t)@"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp903, _temp903, _temp903 +  27u}};
struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){ struct _tuple14* yyzzz;{
void* _temp904= yy1; struct _tuple14* _temp910; _LL906: if(*(( void**) _temp904)
==  Cyc_InitDecl_tok){ _LL911: _temp910=(( struct Cyc_InitDecl_tok_struct*)
_temp904)->f1; goto _LL907;} else{ goto _LL908;} _LL908: goto _LL909; _LL907:
yyzzz= _temp910; goto _LL905; _LL909:( int) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL905:;} return yyzzz;} static unsigned char _temp913[ 35u]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp913, _temp913, _temp913 +  35u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp914= yy1; struct Cyc_List_List*
_temp920; _LL916: if(*(( void**) _temp914) ==  Cyc_InitDeclList_tok){ _LL921:
_temp920=(( struct Cyc_InitDeclList_tok_struct*) _temp914)->f1; goto _LL917;}
else{ goto _LL918;} _LL918: goto _LL919; _LL917: yyzzz= _temp920; goto _LL915;
_LL919:( int) _throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL915:;} return
yyzzz;} static unsigned char _temp923[ 16u]="storage_class_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp923, _temp923, _temp923 + 
16u}}; void* Cyc_yyget_StorageClass_tok( void* yy1){ void* yyzzz;{ void*
_temp924= yy1; void* _temp930; _LL926: if(*(( void**) _temp924) ==  Cyc_StorageClass_tok){
_LL931: _temp930=( void*)(( struct Cyc_StorageClass_tok_struct*) _temp924)->f1;
goto _LL927;} else{ goto _LL928;} _LL928: goto _LL929; _LL927: yyzzz= _temp930;
goto _LL925; _LL929:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL925:;}
return yyzzz;} static unsigned char _temp933[ 17u]="type_specifier_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp933, _temp933, _temp933 +  17u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* yyzzz;{ void* _temp934= yy1; void* _temp940; _LL936: if(*(( void**)
_temp934) ==  Cyc_TypeSpecifier_tok){ _LL941: _temp940=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp934)->f1; goto _LL937;} else{ goto _LL938;} _LL938: goto _LL939; _LL937:
yyzzz= _temp940; goto _LL935; _LL939:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL935:;} return yyzzz;} static unsigned char _temp943[ 18u]="struct_or_union_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp943, _temp943, _temp943 +  18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ enum  Cyc_Parse_Struct_or_union yyzzz;{ void* _temp944= yy1; enum 
Cyc_Parse_Struct_or_union _temp950; _LL946: if(*(( void**) _temp944) ==  Cyc_StructOrUnion_tok){
_LL951: _temp950=(( struct Cyc_StructOrUnion_tok_struct*) _temp944)->f1; goto
_LL947;} else{ goto _LL948;} _LL948: goto _LL949; _LL947: yyzzz= _temp950; goto
_LL945; _LL949:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok); _LL945:;}
return yyzzz;} static unsigned char _temp953[ 8u]="tqual_t"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp953, _temp953, _temp953 +  8u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ struct Cyc_Absyn_Tqual
yyzzz;{ void* _temp954= yy1; struct Cyc_Absyn_Tqual _temp960; _LL956: if(*((
void**) _temp954) ==  Cyc_TypeQual_tok){ _LL961: _temp960=(( struct Cyc_TypeQual_tok_struct*)
_temp954)->f1; goto _LL957;} else{ goto _LL958;} _LL958: goto _LL959; _LL957:
yyzzz= _temp960; goto _LL955; _LL959:( int) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL955:;} return yyzzz;} static unsigned char _temp963[ 22u]="list_t<structfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp963, _temp963, _temp963 +  22u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp964= yy1; struct Cyc_List_List*
_temp970; _LL966: if(*(( void**) _temp964) ==  Cyc_StructFieldDeclList_tok){
_LL971: _temp970=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp964)->f1;
goto _LL967;} else{ goto _LL968;} _LL968: goto _LL969; _LL967: yyzzz= _temp970;
goto _LL965; _LL969:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL965:;} return yyzzz;} static unsigned char _temp973[ 30u]="list_t<list_t<structfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={
Cyc_Core_Failure,{ _temp973, _temp973, _temp973 +  30u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{
void* _temp974= yy1; struct Cyc_List_List* _temp980; _LL976: if(*(( void**)
_temp974) ==  Cyc_StructFieldDeclListList_tok){ _LL981: _temp980=(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp974)->f1; goto _LL977;} else{ goto _LL978;} _LL978: goto _LL979; _LL977:
yyzzz= _temp980; goto _LL975; _LL979:( int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL975:;} return yyzzz;} static unsigned char _temp983[ 24u]="list_t<type_modifier_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{
_temp983, _temp983, _temp983 +  24u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp984= yy1; struct Cyc_List_List*
_temp990; _LL986: if(*(( void**) _temp984) ==  Cyc_TypeModifierList_tok){ _LL991:
_temp990=(( struct Cyc_TypeModifierList_tok_struct*) _temp984)->f1; goto _LL987;}
else{ goto _LL988;} _LL988: goto _LL989; _LL987: yyzzz= _temp990; goto _LL985;
_LL989:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL985:;}
return yyzzz;} static unsigned char _temp993[ 13u]="declarator_t"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp993,
_temp993, _temp993 +  13u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ struct Cyc_Parse_Declarator* yyzzz;{ void* _temp994= yy1; struct Cyc_Parse_Declarator*
_temp1000; _LL996: if(*(( void**) _temp994) ==  Cyc_Declarator_tok){ _LL1001:
_temp1000=(( struct Cyc_Declarator_tok_struct*) _temp994)->f1; goto _LL997;}
else{ goto _LL998;} _LL998: goto _LL999; _LL997: yyzzz= _temp1000; goto _LL995;
_LL999:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok); _LL995:;} return
yyzzz;} static unsigned char _temp1003[ 21u]="abstractdeclarator_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{
_temp1003, _temp1003, _temp1003 +  21u}}; struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_AbstractDeclarator_tok( void* yy1){ struct Cyc_Parse_Abstractdeclarator*
yyzzz;{ void* _temp1004= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1010;
_LL1006: if(*(( void**) _temp1004) ==  Cyc_AbstractDeclarator_tok){ _LL1011:
_temp1010=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1004)->f1; goto
_LL1007;} else{ goto _LL1008;} _LL1008: goto _LL1009; _LL1007: yyzzz= _temp1010;
goto _LL1005; _LL1009:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1005:;} return yyzzz;} static unsigned char _temp1013[ 5u]="bool"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{
_temp1013, _temp1013, _temp1013 +  5u}}; int Cyc_yyget_Bool_tok( void* yy1){ int
yyzzz;{ void* _temp1014= yy1; int _temp1020; _LL1016: if(*(( void**) _temp1014)
==  Cyc_Bool_tok){ _LL1021: _temp1020=(( struct Cyc_Bool_tok_struct*) _temp1014)->f1;
goto _LL1017;} else{ goto _LL1018;} _LL1018: goto _LL1019; _LL1017: yyzzz=
_temp1020; goto _LL1015; _LL1019:( int) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1015:;} return yyzzz;} static unsigned char _temp1023[ 8u]="scope_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{
_temp1023, _temp1023, _temp1023 +  8u}}; void* Cyc_yyget_Scope_tok( void* yy1){
void* yyzzz;{ void* _temp1024= yy1; void* _temp1030; _LL1026: if(*(( void**)
_temp1024) ==  Cyc_Scope_tok){ _LL1031: _temp1030=( void*)(( struct Cyc_Scope_tok_struct*)
_temp1024)->f1; goto _LL1027;} else{ goto _LL1028;} _LL1028: goto _LL1029;
_LL1027: yyzzz= _temp1030; goto _LL1025; _LL1029:( int) _throw(( void*)& Cyc_yyfail_Scope_tok);
_LL1025:;} return yyzzz;} static unsigned char _temp1033[ 14u]="tunionfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1033, _temp1033, _temp1033 +  14u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ struct Cyc_Absyn_Tunionfield* yyzzz;{ void* _temp1034= yy1; struct
Cyc_Absyn_Tunionfield* _temp1040; _LL1036: if(*(( void**) _temp1034) ==  Cyc_TunionField_tok){
_LL1041: _temp1040=(( struct Cyc_TunionField_tok_struct*) _temp1034)->f1; goto
_LL1037;} else{ goto _LL1038;} _LL1038: goto _LL1039; _LL1037: yyzzz= _temp1040;
goto _LL1035; _LL1039:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL1035:;} return yyzzz;} static unsigned char _temp1043[ 22u]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp1043, _temp1043, _temp1043 +  22u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1044= yy1; struct Cyc_List_List*
_temp1050; _LL1046: if(*(( void**) _temp1044) ==  Cyc_TunionFieldList_tok){
_LL1051: _temp1050=(( struct Cyc_TunionFieldList_tok_struct*) _temp1044)->f1;
goto _LL1047;} else{ goto _LL1048;} _LL1048: goto _LL1049; _LL1047: yyzzz=
_temp1050; goto _LL1045; _LL1049:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1045:;} return yyzzz;} static unsigned char _temp1053[ 50u]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1053, _temp1053, _temp1053 +  50u}}; struct _tuple17* Cyc_yyget_QualSpecList_tok(
void* yy1){ struct _tuple17* yyzzz;{ void* _temp1054= yy1; struct _tuple17*
_temp1060; _LL1056: if(*(( void**) _temp1054) ==  Cyc_QualSpecList_tok){ _LL1061:
_temp1060=(( struct Cyc_QualSpecList_tok_struct*) _temp1054)->f1; goto _LL1057;}
else{ goto _LL1058;} _LL1058: goto _LL1059; _LL1057: yyzzz= _temp1060; goto
_LL1055; _LL1059:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL1055:;}
return yyzzz;} static unsigned char _temp1063[ 14u]="list_t<var_t>"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{
_temp1063, _temp1063, _temp1063 +  14u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1064= yy1; struct Cyc_List_List*
_temp1070; _LL1066: if(*(( void**) _temp1064) ==  Cyc_IdList_tok){ _LL1071:
_temp1070=(( struct Cyc_IdList_tok_struct*) _temp1064)->f1; goto _LL1067;} else{
goto _LL1068;} _LL1068: goto _LL1069; _LL1067: yyzzz= _temp1070; goto _LL1065;
_LL1069:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1065:;} return yyzzz;}
static unsigned char _temp1073[ 32u]="$(opt_t<var_t>,tqual_t,type_t)@"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{
_temp1073, _temp1073, _temp1073 +  32u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ struct _tuple2* yyzzz;{ void* _temp1074= yy1; struct _tuple2*
_temp1080; _LL1076: if(*(( void**) _temp1074) ==  Cyc_ParamDecl_tok){ _LL1081:
_temp1080=(( struct Cyc_ParamDecl_tok_struct*) _temp1074)->f1; goto _LL1077;}
else{ goto _LL1078;} _LL1078: goto _LL1079; _LL1077: yyzzz= _temp1080; goto
_LL1075; _LL1079:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1075:;}
return yyzzz;} static unsigned char _temp1083[ 40u]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{
_temp1083, _temp1083, _temp1083 +  40u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1084= yy1; struct Cyc_List_List*
_temp1090; _LL1086: if(*(( void**) _temp1084) ==  Cyc_ParamDeclList_tok){
_LL1091: _temp1090=(( struct Cyc_ParamDeclList_tok_struct*) _temp1084)->f1; goto
_LL1087;} else{ goto _LL1088;} _LL1088: goto _LL1089; _LL1087: yyzzz= _temp1090;
goto _LL1085; _LL1089:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1085:;} return yyzzz;} static unsigned char _temp1093[ 107u]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={ Cyc_Core_Failure,{
_temp1093, _temp1093, _temp1093 +  107u}}; struct _tuple16* Cyc_yyget_YY1( void*
yy1){ struct _tuple16* yyzzz;{ void* _temp1094= yy1; struct _tuple16* _temp1100;
_LL1096: if(*(( void**) _temp1094) ==  Cyc_YY1){ _LL1101: _temp1100=(( struct
Cyc_YY1_struct*) _temp1094)->f1; goto _LL1097;} else{ goto _LL1098;} _LL1098:
goto _LL1099; _LL1097: yyzzz= _temp1100; goto _LL1095; _LL1099:( int) _throw((
void*)& Cyc_yyfail_YY1); _LL1095:;} return yyzzz;} static unsigned char
_temp1103[ 15u]="list_t<type_t>"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1103, _temp1103, _temp1103 +  15u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ struct Cyc_List_List* yyzzz;{ void*
_temp1104= yy1; struct Cyc_List_List* _temp1110; _LL1106: if(*(( void**)
_temp1104) ==  Cyc_TypeList_tok){ _LL1111: _temp1110=(( struct Cyc_TypeList_tok_struct*)
_temp1104)->f1; goto _LL1107;} else{ goto _LL1108;} _LL1108: goto _LL1109;
_LL1107: yyzzz= _temp1110; goto _LL1105; _LL1109:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok);
_LL1105:;} return yyzzz;} static unsigned char _temp1113[ 21u]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{
_temp1113, _temp1113, _temp1113 +  21u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1114= yy1; struct Cyc_List_List*
_temp1120; _LL1116: if(*(( void**) _temp1114) ==  Cyc_DesignatorList_tok){
_LL1121: _temp1120=(( struct Cyc_DesignatorList_tok_struct*) _temp1114)->f1;
goto _LL1117;} else{ goto _LL1118;} _LL1118: goto _LL1119; _LL1117: yyzzz=
_temp1120; goto _LL1115; _LL1119:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1115:;} return yyzzz;} static unsigned char _temp1123[ 13u]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{
_temp1123, _temp1123, _temp1123 +  13u}}; void* Cyc_yyget_Designator_tok( void*
yy1){ void* yyzzz;{ void* _temp1124= yy1; void* _temp1130; _LL1126: if(*(( void**)
_temp1124) ==  Cyc_Designator_tok){ _LL1131: _temp1130=( void*)(( struct Cyc_Designator_tok_struct*)
_temp1124)->f1; goto _LL1127;} else{ goto _LL1128;} _LL1128: goto _LL1129;
_LL1127: yyzzz= _temp1130; goto _LL1125; _LL1129:( int) _throw(( void*)& Cyc_yyfail_Designator_tok);
_LL1125:;} return yyzzz;} static unsigned char _temp1133[ 7u]="kind_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{
_temp1133, _temp1133, _temp1133 +  7u}}; void* Cyc_yyget_Kind_tok( void* yy1){
void* yyzzz;{ void* _temp1134= yy1; void* _temp1140; _LL1136: if(*(( void**)
_temp1134) ==  Cyc_Kind_tok){ _LL1141: _temp1140=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1134)->f1; goto _LL1137;} else{ goto _LL1138;} _LL1138: goto _LL1139;
_LL1137: yyzzz= _temp1140; goto _LL1135; _LL1139:( int) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1135:;} return yyzzz;} static unsigned char _temp1143[ 7u]="type_t"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1143, _temp1143, _temp1143 +  7u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* yyzzz;{ void* _temp1144= yy1; void* _temp1150; _LL1146: if(*(( void**)
_temp1144) ==  Cyc_Type_tok){ _LL1151: _temp1150=( void*)(( struct Cyc_Type_tok_struct*)
_temp1144)->f1; goto _LL1147;} else{ goto _LL1148;} _LL1148: goto _LL1149;
_LL1147: yyzzz= _temp1150; goto _LL1145; _LL1149:( int) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1145:;} return yyzzz;} static unsigned char _temp1153[ 20u]="list_t<attribute_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1153, _temp1153, _temp1153 +  20u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1154= yy1; struct Cyc_List_List*
_temp1160; _LL1156: if(*(( void**) _temp1154) ==  Cyc_AttributeList_tok){
_LL1161: _temp1160=(( struct Cyc_AttributeList_tok_struct*) _temp1154)->f1; goto
_LL1157;} else{ goto _LL1158;} _LL1158: goto _LL1159; _LL1157: yyzzz= _temp1160;
goto _LL1155; _LL1159:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1155:;} return yyzzz;} static unsigned char _temp1163[ 12u]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{
_temp1163, _temp1163, _temp1163 +  12u}}; void* Cyc_yyget_Attribute_tok( void*
yy1){ void* yyzzz;{ void* _temp1164= yy1; void* _temp1170; _LL1166: if(*(( void**)
_temp1164) ==  Cyc_Attribute_tok){ _LL1171: _temp1170=( void*)(( struct Cyc_Attribute_tok_struct*)
_temp1164)->f1; goto _LL1167;} else{ goto _LL1168;} _LL1168: goto _LL1169;
_LL1167: yyzzz= _temp1170; goto _LL1165; _LL1169:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok);
_LL1165:;} return yyzzz;} static unsigned char _temp1173[ 12u]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{
_temp1173, _temp1173, _temp1173 +  12u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
void* yy1){ struct Cyc_Absyn_Enumfield* yyzzz;{ void* _temp1174= yy1; struct Cyc_Absyn_Enumfield*
_temp1180; _LL1176: if(*(( void**) _temp1174) ==  Cyc_Enumfield_tok){ _LL1181:
_temp1180=(( struct Cyc_Enumfield_tok_struct*) _temp1174)->f1; goto _LL1177;}
else{ goto _LL1178;} _LL1178: goto _LL1179; _LL1177: yyzzz= _temp1180; goto
_LL1175; _LL1179:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1175:;}
return yyzzz;} static unsigned char _temp1183[ 20u]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1183, _temp1183, _temp1183 +  20u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1184= yy1; struct Cyc_List_List*
_temp1190; _LL1186: if(*(( void**) _temp1184) ==  Cyc_EnumfieldList_tok){
_LL1191: _temp1190=(( struct Cyc_EnumfieldList_tok_struct*) _temp1184)->f1; goto
_LL1187;} else{ goto _LL1188;} _LL1188: goto _LL1189; _LL1187: yyzzz= _temp1190;
goto _LL1185; _LL1189:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1185:;} return yyzzz;} static unsigned char _temp1193[ 14u]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{
_temp1193, _temp1193, _temp1193 +  14u}}; struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok(
void* yy1){ struct Cyc_Core_Opt* yyzzz;{ void* _temp1194= yy1; struct Cyc_Core_Opt*
_temp1200; _LL1196: if(*(( void**) _temp1194) ==  Cyc_TypeOpt_tok){ _LL1201:
_temp1200=(( struct Cyc_TypeOpt_tok_struct*) _temp1194)->f1; goto _LL1197;}
else{ goto _LL1198;} _LL1198: goto _LL1199; _LL1197: yyzzz= _temp1200; goto
_LL1195; _LL1199:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1195:;}
return yyzzz;} static unsigned char _temp1203[ 26u]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{
_temp1203, _temp1203, _temp1203 +  26u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok(
void* yy1){ struct Cyc_List_List* yyzzz;{ void* _temp1204= yy1; struct Cyc_List_List*
_temp1210; _LL1206: if(*(( void**) _temp1204) ==  Cyc_Rgnorder_tok){ _LL1211:
_temp1210=(( struct Cyc_Rgnorder_tok_struct*) _temp1204)->f1; goto _LL1207;}
else{ goto _LL1208;} _LL1208: goto _LL1209; _LL1207: yyzzz= _temp1210; goto
_LL1205; _LL1209:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1205:;}
return yyzzz;} struct Cyc_Yyltype{ int timestamp; int first_line; int
first_column; int last_line; int last_column; } ; struct Cyc_Yyltype Cyc_yynewloc(){
return({ struct Cyc_Yyltype _temp1212; _temp1212.timestamp= 0; _temp1212.first_line=
0; _temp1212.first_column= 0; _temp1212.last_line= 0; _temp1212.last_column= 0;
_temp1212;});} struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[
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
static unsigned char _temp1213[ 2u]="$"; static unsigned char _temp1214[ 6u]="error";
static unsigned char _temp1215[ 12u]="$undefined."; static unsigned char
_temp1216[ 5u]="AUTO"; static unsigned char _temp1217[ 9u]="REGISTER"; static
unsigned char _temp1218[ 7u]="STATIC"; static unsigned char _temp1219[ 7u]="EXTERN";
static unsigned char _temp1220[ 8u]="TYPEDEF"; static unsigned char _temp1221[ 5u]="VOID";
static unsigned char _temp1222[ 5u]="CHAR"; static unsigned char _temp1223[ 6u]="SHORT";
static unsigned char _temp1224[ 4u]="INT"; static unsigned char _temp1225[ 5u]="LONG";
static unsigned char _temp1226[ 6u]="FLOAT"; static unsigned char _temp1227[ 7u]="DOUBLE";
static unsigned char _temp1228[ 7u]="SIGNED"; static unsigned char _temp1229[ 9u]="UNSIGNED";
static unsigned char _temp1230[ 6u]="CONST"; static unsigned char _temp1231[ 9u]="VOLATILE";
static unsigned char _temp1232[ 9u]="RESTRICT"; static unsigned char _temp1233[
7u]="STRUCT"; static unsigned char _temp1234[ 6u]="UNION"; static unsigned char
_temp1235[ 5u]="CASE"; static unsigned char _temp1236[ 8u]="DEFAULT"; static
unsigned char _temp1237[ 7u]="INLINE"; static unsigned char _temp1238[ 7u]="SIZEOF";
static unsigned char _temp1239[ 9u]="OFFSETOF"; static unsigned char _temp1240[
3u]="IF"; static unsigned char _temp1241[ 5u]="ELSE"; static unsigned char
_temp1242[ 7u]="SWITCH"; static unsigned char _temp1243[ 6u]="WHILE"; static
unsigned char _temp1244[ 3u]="DO"; static unsigned char _temp1245[ 4u]="FOR";
static unsigned char _temp1246[ 5u]="GOTO"; static unsigned char _temp1247[ 9u]="CONTINUE";
static unsigned char _temp1248[ 6u]="BREAK"; static unsigned char _temp1249[ 7u]="RETURN";
static unsigned char _temp1250[ 5u]="ENUM"; static unsigned char _temp1251[ 8u]="NULL_kw";
static unsigned char _temp1252[ 4u]="LET"; static unsigned char _temp1253[ 6u]="THROW";
static unsigned char _temp1254[ 4u]="TRY"; static unsigned char _temp1255[ 6u]="CATCH";
static unsigned char _temp1256[ 4u]="NEW"; static unsigned char _temp1257[ 9u]="ABSTRACT";
static unsigned char _temp1258[ 9u]="FALLTHRU"; static unsigned char _temp1259[
6u]="USING"; static unsigned char _temp1260[ 10u]="NAMESPACE"; static
unsigned char _temp1261[ 7u]="TUNION"; static unsigned char _temp1262[ 8u]="XTUNION";
static unsigned char _temp1263[ 9u]="FORARRAY"; static unsigned char _temp1264[
5u]="FILL"; static unsigned char _temp1265[ 8u]="CODEGEN"; static unsigned char
_temp1266[ 4u]="CUT"; static unsigned char _temp1267[ 7u]="SPLICE"; static
unsigned char _temp1268[ 7u]="MALLOC"; static unsigned char _temp1269[ 8u]="RMALLOC";
static unsigned char _temp1270[ 7u]="CALLOC"; static unsigned char _temp1271[ 8u]="RCALLOC";
static unsigned char _temp1272[ 9u]="REGION_T"; static unsigned char _temp1273[
9u]="SIZEOF_T"; static unsigned char _temp1274[ 7u]="REGION"; static
unsigned char _temp1275[ 5u]="RNEW"; static unsigned char _temp1276[ 8u]="REGIONS";
static unsigned char _temp1277[ 4u]="GEN"; static unsigned char _temp1278[ 7u]="PTR_OP";
static unsigned char _temp1279[ 7u]="INC_OP"; static unsigned char _temp1280[ 7u]="DEC_OP";
static unsigned char _temp1281[ 8u]="LEFT_OP"; static unsigned char _temp1282[ 9u]="RIGHT_OP";
static unsigned char _temp1283[ 6u]="LE_OP"; static unsigned char _temp1284[ 6u]="GE_OP";
static unsigned char _temp1285[ 6u]="EQ_OP"; static unsigned char _temp1286[ 6u]="NE_OP";
static unsigned char _temp1287[ 7u]="AND_OP"; static unsigned char _temp1288[ 6u]="OR_OP";
static unsigned char _temp1289[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1290[ 11u]="DIV_ASSIGN"; static unsigned char _temp1291[ 11u]="MOD_ASSIGN";
static unsigned char _temp1292[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1293[ 11u]="SUB_ASSIGN"; static unsigned char _temp1294[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1295[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1296[ 11u]="AND_ASSIGN"; static unsigned char _temp1297[ 11u]="XOR_ASSIGN";
static unsigned char _temp1298[ 10u]="OR_ASSIGN"; static unsigned char _temp1299[
9u]="ELLIPSIS"; static unsigned char _temp1300[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1301[ 12u]="COLON_COLON"; static unsigned char _temp1302[ 11u]="IDENTIFIER";
static unsigned char _temp1303[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1304[ 7u]="STRING"; static unsigned char _temp1305[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1306[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1307[ 9u]="TYPE_VAR"; static unsigned char _temp1308[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1309[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1310[ 10u]="ATTRIBUTE"; static unsigned char _temp1311[ 4u]="';'"; static
unsigned char _temp1312[ 4u]="'{'"; static unsigned char _temp1313[ 4u]="'}'";
static unsigned char _temp1314[ 4u]="'='"; static unsigned char _temp1315[ 4u]="'('";
static unsigned char _temp1316[ 4u]="')'"; static unsigned char _temp1317[ 4u]="','";
static unsigned char _temp1318[ 4u]="'_'"; static unsigned char _temp1319[ 4u]="'$'";
static unsigned char _temp1320[ 4u]="'<'"; static unsigned char _temp1321[ 4u]="'>'";
static unsigned char _temp1322[ 4u]="':'"; static unsigned char _temp1323[ 4u]="'.'";
static unsigned char _temp1324[ 4u]="'['"; static unsigned char _temp1325[ 4u]="']'";
static unsigned char _temp1326[ 4u]="'*'"; static unsigned char _temp1327[ 4u]="'@'";
static unsigned char _temp1328[ 4u]="'?'"; static unsigned char _temp1329[ 4u]="'+'";
static unsigned char _temp1330[ 4u]="'-'"; static unsigned char _temp1331[ 4u]="'&'";
static unsigned char _temp1332[ 4u]="'|'"; static unsigned char _temp1333[ 4u]="'^'";
static unsigned char _temp1334[ 4u]="'/'"; static unsigned char _temp1335[ 4u]="'%'";
static unsigned char _temp1336[ 4u]="'~'"; static unsigned char _temp1337[ 4u]="'!'";
static unsigned char _temp1338[ 5u]="prog"; static unsigned char _temp1339[ 17u]="translation_unit";
static unsigned char _temp1340[ 21u]="external_declaration"; static
unsigned char _temp1341[ 20u]="function_definition"; static unsigned char
_temp1342[ 21u]="function_definition2"; static unsigned char _temp1343[ 13u]="using_action";
static unsigned char _temp1344[ 15u]="unusing_action"; static unsigned char
_temp1345[ 17u]="namespace_action"; static unsigned char _temp1346[ 19u]="unnamespace_action";
static unsigned char _temp1347[ 12u]="declaration"; static unsigned char
_temp1348[ 17u]="declaration_list"; static unsigned char _temp1349[ 23u]="declaration_specifiers";
static unsigned char _temp1350[ 24u]="storage_class_specifier"; static
unsigned char _temp1351[ 15u]="attributes_opt"; static unsigned char _temp1352[
11u]="attributes"; static unsigned char _temp1353[ 15u]="attribute_list"; static
unsigned char _temp1354[ 10u]="attribute"; static unsigned char _temp1355[ 15u]="type_specifier";
static unsigned char _temp1356[ 5u]="kind"; static unsigned char _temp1357[ 15u]="type_qualifier";
static unsigned char _temp1358[ 15u]="enum_specifier"; static unsigned char
_temp1359[ 11u]="enum_field"; static unsigned char _temp1360[ 22u]="enum_declaration_list";
static unsigned char _temp1361[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1362[ 16u]="type_params_opt"; static unsigned char _temp1363[
16u]="struct_or_union"; static unsigned char _temp1364[ 24u]="struct_declaration_list";
static unsigned char _temp1365[ 25u]="struct_declaration_list0"; static
unsigned char _temp1366[ 21u]="init_declarator_list"; static unsigned char
_temp1367[ 22u]="init_declarator_list0"; static unsigned char _temp1368[ 16u]="init_declarator";
static unsigned char _temp1369[ 19u]="struct_declaration"; static unsigned char
_temp1370[ 25u]="specifier_qualifier_list"; static unsigned char _temp1371[ 23u]="struct_declarator_list";
static unsigned char _temp1372[ 24u]="struct_declarator_list0"; static
unsigned char _temp1373[ 18u]="struct_declarator"; static unsigned char
_temp1374[ 17u]="tunion_specifier"; static unsigned char _temp1375[ 18u]="tunion_or_xtunion";
static unsigned char _temp1376[ 17u]="tunionfield_list"; static unsigned char
_temp1377[ 18u]="tunionfield_scope"; static unsigned char _temp1378[ 12u]="tunionfield";
static unsigned char _temp1379[ 11u]="declarator"; static unsigned char
_temp1380[ 18u]="direct_declarator"; static unsigned char _temp1381[ 8u]="pointer";
static unsigned char _temp1382[ 13u]="pointer_char"; static unsigned char
_temp1383[ 8u]="rgn_opt"; static unsigned char _temp1384[ 4u]="rgn"; static
unsigned char _temp1385[ 20u]="type_qualifier_list"; static unsigned char
_temp1386[ 20u]="parameter_type_list"; static unsigned char _temp1387[ 9u]="type_var";
static unsigned char _temp1388[ 16u]="optional_effect"; static unsigned char
_temp1389[ 19u]="optional_rgn_order"; static unsigned char _temp1390[ 10u]="rgn_order";
static unsigned char _temp1391[ 16u]="optional_inject"; static unsigned char
_temp1392[ 11u]="effect_set"; static unsigned char _temp1393[ 14u]="atomic_effect";
static unsigned char _temp1394[ 11u]="region_set"; static unsigned char
_temp1395[ 15u]="parameter_list"; static unsigned char _temp1396[ 22u]="parameter_declaration";
static unsigned char _temp1397[ 16u]="identifier_list"; static unsigned char
_temp1398[ 17u]="identifier_list0"; static unsigned char _temp1399[ 12u]="initializer";
static unsigned char _temp1400[ 18u]="array_initializer"; static unsigned char
_temp1401[ 17u]="initializer_list"; static unsigned char _temp1402[ 12u]="designation";
static unsigned char _temp1403[ 16u]="designator_list"; static unsigned char
_temp1404[ 11u]="designator"; static unsigned char _temp1405[ 10u]="type_name";
static unsigned char _temp1406[ 14u]="any_type_name"; static unsigned char
_temp1407[ 15u]="type_name_list"; static unsigned char _temp1408[ 20u]="abstract_declarator";
static unsigned char _temp1409[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1410[ 10u]="statement"; static unsigned char _temp1411[ 18u]="labeled_statement";
static unsigned char _temp1412[ 21u]="expression_statement"; static
unsigned char _temp1413[ 19u]="compound_statement"; static unsigned char
_temp1414[ 16u]="block_item_list"; static unsigned char _temp1415[ 20u]="selection_statement";
static unsigned char _temp1416[ 15u]="switch_clauses"; static unsigned char
_temp1417[ 16u]="switchC_clauses"; static unsigned char _temp1418[ 20u]="iteration_statement";
static unsigned char _temp1419[ 15u]="jump_statement"; static unsigned char
_temp1420[ 8u]="pattern"; static unsigned char _temp1421[ 19u]="tuple_pattern_list";
static unsigned char _temp1422[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1423[ 14u]="field_pattern"; static unsigned char _temp1424[ 19u]="field_pattern_list";
static unsigned char _temp1425[ 20u]="field_pattern_list0"; static unsigned char
_temp1426[ 11u]="expression"; static unsigned char _temp1427[ 22u]="assignment_expression";
static unsigned char _temp1428[ 20u]="assignment_operator"; static unsigned char
_temp1429[ 23u]="conditional_expression"; static unsigned char _temp1430[ 20u]="constant_expression";
static unsigned char _temp1431[ 22u]="logical_or_expression"; static
unsigned char _temp1432[ 23u]="logical_and_expression"; static unsigned char
_temp1433[ 24u]="inclusive_or_expression"; static unsigned char _temp1434[ 24u]="exclusive_or_expression";
static unsigned char _temp1435[ 15u]="and_expression"; static unsigned char
_temp1436[ 20u]="equality_expression"; static unsigned char _temp1437[ 22u]="relational_expression";
static unsigned char _temp1438[ 17u]="shift_expression"; static unsigned char
_temp1439[ 20u]="additive_expression"; static unsigned char _temp1440[ 26u]="multiplicative_expression";
static unsigned char _temp1441[ 16u]="cast_expression"; static unsigned char
_temp1442[ 17u]="unary_expression"; static unsigned char _temp1443[ 15u]="unary_operator";
static unsigned char _temp1444[ 19u]="postfix_expression"; static unsigned char
_temp1445[ 19u]="primary_expression"; static unsigned char _temp1446[ 25u]="argument_expression_list";
static unsigned char _temp1447[ 26u]="argument_expression_list0"; static
unsigned char _temp1448[ 9u]="constant"; static unsigned char _temp1449[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 237u]={{ _temp1213, _temp1213, _temp1213
+  2u},{ _temp1214, _temp1214, _temp1214 +  6u},{ _temp1215, _temp1215,
_temp1215 +  12u},{ _temp1216, _temp1216, _temp1216 +  5u},{ _temp1217,
_temp1217, _temp1217 +  9u},{ _temp1218, _temp1218, _temp1218 +  7u},{ _temp1219,
_temp1219, _temp1219 +  7u},{ _temp1220, _temp1220, _temp1220 +  8u},{ _temp1221,
_temp1221, _temp1221 +  5u},{ _temp1222, _temp1222, _temp1222 +  5u},{ _temp1223,
_temp1223, _temp1223 +  6u},{ _temp1224, _temp1224, _temp1224 +  4u},{ _temp1225,
_temp1225, _temp1225 +  5u},{ _temp1226, _temp1226, _temp1226 +  6u},{ _temp1227,
_temp1227, _temp1227 +  7u},{ _temp1228, _temp1228, _temp1228 +  7u},{ _temp1229,
_temp1229, _temp1229 +  9u},{ _temp1230, _temp1230, _temp1230 +  6u},{ _temp1231,
_temp1231, _temp1231 +  9u},{ _temp1232, _temp1232, _temp1232 +  9u},{ _temp1233,
_temp1233, _temp1233 +  7u},{ _temp1234, _temp1234, _temp1234 +  6u},{ _temp1235,
_temp1235, _temp1235 +  5u},{ _temp1236, _temp1236, _temp1236 +  8u},{ _temp1237,
_temp1237, _temp1237 +  7u},{ _temp1238, _temp1238, _temp1238 +  7u},{ _temp1239,
_temp1239, _temp1239 +  9u},{ _temp1240, _temp1240, _temp1240 +  3u},{ _temp1241,
_temp1241, _temp1241 +  5u},{ _temp1242, _temp1242, _temp1242 +  7u},{ _temp1243,
_temp1243, _temp1243 +  6u},{ _temp1244, _temp1244, _temp1244 +  3u},{ _temp1245,
_temp1245, _temp1245 +  4u},{ _temp1246, _temp1246, _temp1246 +  5u},{ _temp1247,
_temp1247, _temp1247 +  9u},{ _temp1248, _temp1248, _temp1248 +  6u},{ _temp1249,
_temp1249, _temp1249 +  7u},{ _temp1250, _temp1250, _temp1250 +  5u},{ _temp1251,
_temp1251, _temp1251 +  8u},{ _temp1252, _temp1252, _temp1252 +  4u},{ _temp1253,
_temp1253, _temp1253 +  6u},{ _temp1254, _temp1254, _temp1254 +  4u},{ _temp1255,
_temp1255, _temp1255 +  6u},{ _temp1256, _temp1256, _temp1256 +  4u},{ _temp1257,
_temp1257, _temp1257 +  9u},{ _temp1258, _temp1258, _temp1258 +  9u},{ _temp1259,
_temp1259, _temp1259 +  6u},{ _temp1260, _temp1260, _temp1260 +  10u},{
_temp1261, _temp1261, _temp1261 +  7u},{ _temp1262, _temp1262, _temp1262 +  8u},{
_temp1263, _temp1263, _temp1263 +  9u},{ _temp1264, _temp1264, _temp1264 +  5u},{
_temp1265, _temp1265, _temp1265 +  8u},{ _temp1266, _temp1266, _temp1266 +  4u},{
_temp1267, _temp1267, _temp1267 +  7u},{ _temp1268, _temp1268, _temp1268 +  7u},{
_temp1269, _temp1269, _temp1269 +  8u},{ _temp1270, _temp1270, _temp1270 +  7u},{
_temp1271, _temp1271, _temp1271 +  8u},{ _temp1272, _temp1272, _temp1272 +  9u},{
_temp1273, _temp1273, _temp1273 +  9u},{ _temp1274, _temp1274, _temp1274 +  7u},{
_temp1275, _temp1275, _temp1275 +  5u},{ _temp1276, _temp1276, _temp1276 +  8u},{
_temp1277, _temp1277, _temp1277 +  4u},{ _temp1278, _temp1278, _temp1278 +  7u},{
_temp1279, _temp1279, _temp1279 +  7u},{ _temp1280, _temp1280, _temp1280 +  7u},{
_temp1281, _temp1281, _temp1281 +  8u},{ _temp1282, _temp1282, _temp1282 +  9u},{
_temp1283, _temp1283, _temp1283 +  6u},{ _temp1284, _temp1284, _temp1284 +  6u},{
_temp1285, _temp1285, _temp1285 +  6u},{ _temp1286, _temp1286, _temp1286 +  6u},{
_temp1287, _temp1287, _temp1287 +  7u},{ _temp1288, _temp1288, _temp1288 +  6u},{
_temp1289, _temp1289, _temp1289 +  11u},{ _temp1290, _temp1290, _temp1290 +  11u},{
_temp1291, _temp1291, _temp1291 +  11u},{ _temp1292, _temp1292, _temp1292 +  11u},{
_temp1293, _temp1293, _temp1293 +  11u},{ _temp1294, _temp1294, _temp1294 +  12u},{
_temp1295, _temp1295, _temp1295 +  13u},{ _temp1296, _temp1296, _temp1296 +  11u},{
_temp1297, _temp1297, _temp1297 +  11u},{ _temp1298, _temp1298, _temp1298 +  10u},{
_temp1299, _temp1299, _temp1299 +  9u},{ _temp1300, _temp1300, _temp1300 +  11u},{
_temp1301, _temp1301, _temp1301 +  12u},{ _temp1302, _temp1302, _temp1302 +  11u},{
_temp1303, _temp1303, _temp1303 +  17u},{ _temp1304, _temp1304, _temp1304 +  7u},{
_temp1305, _temp1305, _temp1305 +  19u},{ _temp1306, _temp1306, _temp1306 +  18u},{
_temp1307, _temp1307, _temp1307 +  9u},{ _temp1308, _temp1308, _temp1308 +  16u},{
_temp1309, _temp1309, _temp1309 +  18u},{ _temp1310, _temp1310, _temp1310 +  10u},{
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
_temp1333, _temp1333, _temp1333 +  4u},{ _temp1334, _temp1334, _temp1334 +  4u},{
_temp1335, _temp1335, _temp1335 +  4u},{ _temp1336, _temp1336, _temp1336 +  4u},{
_temp1337, _temp1337, _temp1337 +  4u},{ _temp1338, _temp1338, _temp1338 +  5u},{
_temp1339, _temp1339, _temp1339 +  17u},{ _temp1340, _temp1340, _temp1340 +  21u},{
_temp1341, _temp1341, _temp1341 +  20u},{ _temp1342, _temp1342, _temp1342 +  21u},{
_temp1343, _temp1343, _temp1343 +  13u},{ _temp1344, _temp1344, _temp1344 +  15u},{
_temp1345, _temp1345, _temp1345 +  17u},{ _temp1346, _temp1346, _temp1346 +  19u},{
_temp1347, _temp1347, _temp1347 +  12u},{ _temp1348, _temp1348, _temp1348 +  17u},{
_temp1349, _temp1349, _temp1349 +  23u},{ _temp1350, _temp1350, _temp1350 +  24u},{
_temp1351, _temp1351, _temp1351 +  15u},{ _temp1352, _temp1352, _temp1352 +  11u},{
_temp1353, _temp1353, _temp1353 +  15u},{ _temp1354, _temp1354, _temp1354 +  10u},{
_temp1355, _temp1355, _temp1355 +  15u},{ _temp1356, _temp1356, _temp1356 +  5u},{
_temp1357, _temp1357, _temp1357 +  15u},{ _temp1358, _temp1358, _temp1358 +  15u},{
_temp1359, _temp1359, _temp1359 +  11u},{ _temp1360, _temp1360, _temp1360 +  22u},{
_temp1361, _temp1361, _temp1361 +  26u},{ _temp1362, _temp1362, _temp1362 +  16u},{
_temp1363, _temp1363, _temp1363 +  16u},{ _temp1364, _temp1364, _temp1364 +  24u},{
_temp1365, _temp1365, _temp1365 +  25u},{ _temp1366, _temp1366, _temp1366 +  21u},{
_temp1367, _temp1367, _temp1367 +  22u},{ _temp1368, _temp1368, _temp1368 +  16u},{
_temp1369, _temp1369, _temp1369 +  19u},{ _temp1370, _temp1370, _temp1370 +  25u},{
_temp1371, _temp1371, _temp1371 +  23u},{ _temp1372, _temp1372, _temp1372 +  24u},{
_temp1373, _temp1373, _temp1373 +  18u},{ _temp1374, _temp1374, _temp1374 +  17u},{
_temp1375, _temp1375, _temp1375 +  18u},{ _temp1376, _temp1376, _temp1376 +  17u},{
_temp1377, _temp1377, _temp1377 +  18u},{ _temp1378, _temp1378, _temp1378 +  12u},{
_temp1379, _temp1379, _temp1379 +  11u},{ _temp1380, _temp1380, _temp1380 +  18u},{
_temp1381, _temp1381, _temp1381 +  8u},{ _temp1382, _temp1382, _temp1382 +  13u},{
_temp1383, _temp1383, _temp1383 +  8u},{ _temp1384, _temp1384, _temp1384 +  4u},{
_temp1385, _temp1385, _temp1385 +  20u},{ _temp1386, _temp1386, _temp1386 +  20u},{
_temp1387, _temp1387, _temp1387 +  9u},{ _temp1388, _temp1388, _temp1388 +  16u},{
_temp1389, _temp1389, _temp1389 +  19u},{ _temp1390, _temp1390, _temp1390 +  10u},{
_temp1391, _temp1391, _temp1391 +  16u},{ _temp1392, _temp1392, _temp1392 +  11u},{
_temp1393, _temp1393, _temp1393 +  14u},{ _temp1394, _temp1394, _temp1394 +  11u},{
_temp1395, _temp1395, _temp1395 +  15u},{ _temp1396, _temp1396, _temp1396 +  22u},{
_temp1397, _temp1397, _temp1397 +  16u},{ _temp1398, _temp1398, _temp1398 +  17u},{
_temp1399, _temp1399, _temp1399 +  12u},{ _temp1400, _temp1400, _temp1400 +  18u},{
_temp1401, _temp1401, _temp1401 +  17u},{ _temp1402, _temp1402, _temp1402 +  12u},{
_temp1403, _temp1403, _temp1403 +  16u},{ _temp1404, _temp1404, _temp1404 +  11u},{
_temp1405, _temp1405, _temp1405 +  10u},{ _temp1406, _temp1406, _temp1406 +  14u},{
_temp1407, _temp1407, _temp1407 +  15u},{ _temp1408, _temp1408, _temp1408 +  20u},{
_temp1409, _temp1409, _temp1409 +  27u},{ _temp1410, _temp1410, _temp1410 +  10u},{
_temp1411, _temp1411, _temp1411 +  18u},{ _temp1412, _temp1412, _temp1412 +  21u},{
_temp1413, _temp1413, _temp1413 +  19u},{ _temp1414, _temp1414, _temp1414 +  16u},{
_temp1415, _temp1415, _temp1415 +  20u},{ _temp1416, _temp1416, _temp1416 +  15u},{
_temp1417, _temp1417, _temp1417 +  16u},{ _temp1418, _temp1418, _temp1418 +  20u},{
_temp1419, _temp1419, _temp1419 +  15u},{ _temp1420, _temp1420, _temp1420 +  8u},{
_temp1421, _temp1421, _temp1421 +  19u},{ _temp1422, _temp1422, _temp1422 +  20u},{
_temp1423, _temp1423, _temp1423 +  14u},{ _temp1424, _temp1424, _temp1424 +  19u},{
_temp1425, _temp1425, _temp1425 +  20u},{ _temp1426, _temp1426, _temp1426 +  11u},{
_temp1427, _temp1427, _temp1427 +  22u},{ _temp1428, _temp1428, _temp1428 +  20u},{
_temp1429, _temp1429, _temp1429 +  23u},{ _temp1430, _temp1430, _temp1430 +  20u},{
_temp1431, _temp1431, _temp1431 +  22u},{ _temp1432, _temp1432, _temp1432 +  23u},{
_temp1433, _temp1433, _temp1433 +  24u},{ _temp1434, _temp1434, _temp1434 +  24u},{
_temp1435, _temp1435, _temp1435 +  15u},{ _temp1436, _temp1436, _temp1436 +  20u},{
_temp1437, _temp1437, _temp1437 +  22u},{ _temp1438, _temp1438, _temp1438 +  17u},{
_temp1439, _temp1439, _temp1439 +  20u},{ _temp1440, _temp1440, _temp1440 +  26u},{
_temp1441, _temp1441, _temp1441 +  16u},{ _temp1442, _temp1442, _temp1442 +  17u},{
_temp1443, _temp1443, _temp1443 +  15u},{ _temp1444, _temp1444, _temp1444 +  19u},{
_temp1445, _temp1445, _temp1445 +  19u},{ _temp1446, _temp1446, _temp1446 +  25u},{
_temp1447, _temp1447, _temp1447 +  26u},{ _temp1448, _temp1448, _temp1448 +  9u},{
_temp1449, _temp1449, _temp1449 +  20u}}; static short Cyc_yyr1[ 410u]={ 0, 125,
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
short*)({ unsigned int _temp3319= 10000u; short* _temp3320=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3319));{ unsigned int
_temp3321= _temp3319; unsigned int i; for( i= 0; i <  _temp3321; i ++){
_temp3320[ i]=( short) 0;}}; _temp3320;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3316= 10000u; void** _temp3317=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3316));{ unsigned int _temp3318= _temp3316;
unsigned int i; for( i= 0; i <  _temp3318; i ++){ _temp3317[ i]= Cyc_yylval;}};
_temp3317;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3313= 10000u; struct Cyc_Yyltype* _temp3314=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3313));{
unsigned int _temp3315= _temp3313; unsigned int i; for( i= 0; i <  _temp3315; i
++){ _temp3314[ i]= Cyc_yynewloc();}}; _temp3314;}); int yystacksize= 10000;
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
yyn){ case 1: _LL1450: yyval= yyvs[ yyvsp_offset]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(
yyvs[ yyvsp_offset]); break; case 2: _LL1451: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1453=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1453[ 0]=({ struct Cyc_DeclList_tok_struct _temp1454; _temp1454.tag= Cyc_DeclList_tok;
_temp1454.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ yyvsp_offset])); _temp1454;}); _temp1453;}); break; case 3: _LL1452: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1456=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1456[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1457; _temp1457.tag= Cyc_DeclList_tok; _temp1457.f1=({ struct Cyc_List_List*
_temp1458=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1458->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1459=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1459->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp1460=( struct Cyc_Absyn_Using_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp1460[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp1461; _temp1461.tag= Cyc_Absyn_Using_d; _temp1461.f1=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  2)]); _temp1461.f2= Cyc_yyget_DeclList_tok( yyvs[ yyvsp_offset]); _temp1461;});
_temp1460;})); _temp1459->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line); _temp1459;}); _temp1458->tl= 0; _temp1458;});
_temp1457;}); _temp1456;}); Cyc_Lex_leave_using(); break; case 4: _LL1455: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1463=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1463[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1464; _temp1464.tag= Cyc_DeclList_tok; _temp1464.f1=({ struct Cyc_List_List*
_temp1465=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1465->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1466=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1466->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp1467=( struct Cyc_Absyn_Using_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp1467[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp1468; _temp1468.tag= Cyc_Absyn_Using_d; _temp1468.f1=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  4)]); _temp1468.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1468;});
_temp1467;})); _temp1466->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1466;}); _temp1465->tl= Cyc_yyget_DeclList_tok( yyvs[ yyvsp_offset]);
_temp1465;}); _temp1464;}); _temp1463;}); break; case 5: _LL1462: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1470=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1470[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1471; _temp1471.tag= Cyc_DeclList_tok; _temp1471.f1=({ struct Cyc_List_List*
_temp1472=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1472->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1473=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1473->r=( void*)(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp1474=( struct Cyc_Absyn_Namespace_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp1474[ 0]=({
struct Cyc_Absyn_Namespace_d_struct _temp1475; _temp1475.tag= Cyc_Absyn_Namespace_d;
_temp1475.f1=({ struct _tagged_arr* _temp1476=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp1476[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1476;});
_temp1475.f2= Cyc_yyget_DeclList_tok( yyvs[ yyvsp_offset]); _temp1475;});
_temp1474;})); _temp1473->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line); _temp1473;}); _temp1472->tl= 0; _temp1472;});
_temp1471;}); _temp1470;}); Cyc_Lex_leave_namespace(); break; case 6: _LL1469:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1478=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1478[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1479; _temp1479.tag= Cyc_DeclList_tok; _temp1479.f1=({ struct Cyc_List_List*
_temp1480=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1480->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1481=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1481->r=( void*)(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp1482=( struct Cyc_Absyn_Namespace_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp1482[ 0]=({
struct Cyc_Absyn_Namespace_d_struct _temp1483; _temp1483.tag= Cyc_Absyn_Namespace_d;
_temp1483.f1=({ struct _tagged_arr* _temp1484=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp1484[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1484;});
_temp1483.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1483;}); _temp1482;})); _temp1481->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1481;}); _temp1480->tl= Cyc_yyget_DeclList_tok( yyvs[ yyvsp_offset]);
_temp1480;}); _temp1479;}); _temp1478;}); break; case 7: _LL1477: if( Cyc_Std_strcmp(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  4)]), _tag_arr("C", sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err(
_tag_arr("only extern \"C\" { ... } is allowed", sizeof( unsigned char), 35u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).last_line));} yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1486=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1486[ 0]=({ struct Cyc_DeclList_tok_struct _temp1487; _temp1487.tag= Cyc_DeclList_tok;
_temp1487.f1=({ struct Cyc_List_List* _temp1488=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1488->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1489=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1489->r=( void*)(( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp1490=(
struct Cyc_Absyn_ExternC_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp1490[ 0]=({ struct Cyc_Absyn_ExternC_d_struct _temp1491; _temp1491.tag= Cyc_Absyn_ExternC_d;
_temp1491.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1491;}); _temp1490;})); _temp1489->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1489;}); _temp1488->tl= Cyc_yyget_DeclList_tok( yyvs[ yyvsp_offset]);
_temp1488;}); _temp1487;}); _temp1486;}); break; case 8: _LL1485: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1493=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1493[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1494; _temp1494.tag= Cyc_DeclList_tok; _temp1494.f1= 0; _temp1494;});
_temp1493;}); break; case 9: _LL1492: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1496=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1496[ 0]=({ struct Cyc_DeclList_tok_struct _temp1497; _temp1497.tag= Cyc_DeclList_tok;
_temp1497.f1=({ struct Cyc_List_List* _temp1498=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1498->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp1499=( struct Cyc_Absyn_Fn_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1499[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1500; _temp1500.tag= Cyc_Absyn_Fn_d; _temp1500.f1= Cyc_yyget_FnDecl_tok(
yyvs[ yyvsp_offset]); _temp1500;}); _temp1499;}), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1498->tl=
0; _temp1498;}); _temp1497;}); _temp1496;}); break; case 10: _LL1495: yyval=
yyvs[ yyvsp_offset]; break; case 11: _LL1501: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1503=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1503[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1504; _temp1504.tag= Cyc_FnDecl_tok;
_temp1504.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1504;}); _temp1503;}); break; case 12: _LL1502:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1506=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1506[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1507; _temp1507.tag= Cyc_FnDecl_tok; _temp1507.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp1508=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1508->v=( void*) Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1508;}), Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1507;}); _temp1506;}); break; case 13: _LL1505:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1510=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1510[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1511; _temp1511.tag= Cyc_FnDecl_tok; _temp1511.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1511;}); _temp1510;}); break; case 14: _LL1509:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1513=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1513[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1514; _temp1514.tag= Cyc_FnDecl_tok; _temp1514.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp1515=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1515->v=( void*) Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1515;}), Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1514;}); _temp1513;}); break; case 15: _LL1512:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1517=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1517[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1518; _temp1518.tag= Cyc_FnDecl_tok; _temp1518.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp1519=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1519->v=( void*) Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1519;}), Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1518;}); _temp1517;}); break; case 16: _LL1516:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1521=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1521[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1522; _temp1522.tag= Cyc_FnDecl_tok; _temp1522.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp1523=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1523->v=( void*) Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1523;}), Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1522;}); _temp1521;}); break; case 17: _LL1520:
Cyc_Lex_enter_using( Cyc_yyget_QualId_tok( yyvs[ yyvsp_offset])); yyval= yyvs[
yyvsp_offset]; break; case 18: _LL1524: Cyc_Lex_leave_using(); break; case 19:
_LL1525: Cyc_Lex_enter_namespace(({ struct _tagged_arr* _temp1527=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1527[ 0]= Cyc_yyget_String_tok(
yyvs[ yyvsp_offset]); _temp1527;})); yyval= yyvs[ yyvsp_offset]; break; case 20:
_LL1526: Cyc_Lex_leave_namespace(); break; case 21: _LL1528: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1530=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1530[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1531; _temp1531.tag= Cyc_DeclList_tok; _temp1531.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1531;}); _temp1530;}); break; case 22: _LL1529: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1533=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1533[ 0]=({ struct Cyc_DeclList_tok_struct _temp1534; _temp1534.tag= Cyc_DeclList_tok;
_temp1534.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1534;}); _temp1533;}); break; case 23:
_LL1532: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1536=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1536[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1537; _temp1537.tag= Cyc_DeclList_tok; _temp1537.f1=({ struct Cyc_List_List*
_temp1538=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1538->hd=( void*) Cyc_Absyn_let_decl( Cyc_yyget_Pattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1538->tl= 0; _temp1538;}); _temp1537;});
_temp1536;}); break; case 24: _LL1535: { struct Cyc_List_List* _temp1540= 0;{
struct Cyc_List_List* _temp1541= Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0; _temp1541
!=  0; _temp1541= _temp1541->tl){ struct _tagged_arr* _temp1542=( struct
_tagged_arr*) _temp1541->hd; struct _tuple1* _temp1543=({ struct _tuple1*
_temp1546=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1546->f1=
Cyc_Absyn_rel_ns_null; _temp1546->f2= _temp1542; _temp1546;}); struct Cyc_Absyn_Vardecl*
_temp1544= Cyc_Absyn_new_vardecl( _temp1543, Cyc_Absyn_wildtyp( 0), 0);
_temp1540=({ struct Cyc_List_List* _temp1545=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1545->hd=( void*) _temp1544; _temp1545->tl=
_temp1540; _temp1545;});}} _temp1540=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1540); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1547=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1547[ 0]=({ struct Cyc_DeclList_tok_struct _temp1548; _temp1548.tag= Cyc_DeclList_tok;
_temp1548.f1=({ struct Cyc_List_List* _temp1549=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1549->hd=( void*) Cyc_Absyn_letv_decl(
_temp1540, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp1549->tl= 0; _temp1549;}); _temp1548;}); _temp1547;}); break;} case 25:
_LL1539: yyval= yyvs[ yyvsp_offset]; break; case 26: _LL1550: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1552=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1552[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1553; _temp1553.tag= Cyc_DeclList_tok; _temp1553.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ yyvsp_offset])); _temp1553;}); _temp1552;}); break; case 27: _LL1551:
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1555=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1555[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1556; _temp1556.tag= Cyc_DeclSpec_tok; _temp1556.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1557=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1557->sc=({ struct Cyc_Core_Opt* _temp1558=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1558->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1558;});
_temp1557->tq= Cyc_Absyn_empty_tqual(); _temp1557->type_specs= 0; _temp1557->is_inline=
0; _temp1557->attributes= Cyc_yyget_AttributeList_tok( yyvs[ yyvsp_offset]);
_temp1557;}); _temp1556;}); _temp1555;}); break; case 28: _LL1554: if(( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->sc !=  0){({ void* _temp1560[ 0u]={}; Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _tag_arr("Only one storage class is allowed in a declaration",
sizeof( unsigned char), 51u), _tag_arr( _temp1560, sizeof( void*), 0u));});}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1561=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1561[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1562; _temp1562.tag= Cyc_DeclSpec_tok; _temp1562.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1563=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1563->sc=({ struct Cyc_Core_Opt* _temp1564=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1564->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1564;});
_temp1563->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->tq; _temp1563->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->type_specs; _temp1563->is_inline=(
Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->is_inline; _temp1563->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->attributes); _temp1563;}); _temp1562;}); _temp1561;});
break; case 29: _LL1559: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1566=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1566[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1567; _temp1567.tag= Cyc_DeclSpec_tok;
_temp1567.f1=({ struct Cyc_Parse_Declaration_spec* _temp1568=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1568->sc= 0;
_temp1568->tq= Cyc_Absyn_empty_tqual(); _temp1568->type_specs=({ struct Cyc_List_List*
_temp1569=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1569->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1569->tl= 0;
_temp1569;}); _temp1568->is_inline= 0; _temp1568->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ yyvsp_offset]); _temp1568;}); _temp1567;}); _temp1566;}); break; case 30:
_LL1565: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp1571=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1571[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1572; _temp1572.tag= Cyc_DeclSpec_tok; _temp1572.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1573=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1573->sc=( Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->sc; _temp1573->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->tq; _temp1573->type_specs=({
struct Cyc_List_List* _temp1574=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1574->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1574->tl=(
Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->type_specs; _temp1574;});
_temp1573->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->is_inline;
_temp1573->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->attributes); _temp1573;}); _temp1572;}); _temp1571;});
break; case 31: _LL1570: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1576=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1576[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1577; _temp1577.tag= Cyc_DeclSpec_tok;
_temp1577.f1=({ struct Cyc_Parse_Declaration_spec* _temp1578=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1578->sc= 0;
_temp1578->tq= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1578->type_specs= 0; _temp1578->is_inline= 0;
_temp1578->attributes= Cyc_yyget_AttributeList_tok( yyvs[ yyvsp_offset]);
_temp1578;}); _temp1577;}); _temp1576;}); break; case 32: _LL1575: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1580=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1580[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1581; _temp1581.tag= Cyc_DeclSpec_tok; _temp1581.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1582=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1582->sc=( Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->sc; _temp1582->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->tq); _temp1582->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
yyvsp_offset]))->type_specs; _temp1582->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
yyvsp_offset]))->is_inline; _temp1582->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->attributes); _temp1582;}); _temp1581;}); _temp1580;});
break; case 33: _LL1579: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1584=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1584[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1585; _temp1585.tag= Cyc_DeclSpec_tok;
_temp1585.f1=({ struct Cyc_Parse_Declaration_spec* _temp1586=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1586->sc= 0;
_temp1586->tq= Cyc_Absyn_empty_tqual(); _temp1586->type_specs= 0; _temp1586->is_inline=
1; _temp1586->attributes= Cyc_yyget_AttributeList_tok( yyvs[ yyvsp_offset]);
_temp1586;}); _temp1585;}); _temp1584;}); break; case 34: _LL1583: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1588=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1588[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1589; _temp1589.tag= Cyc_DeclSpec_tok; _temp1589.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1590=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1590->sc=( Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->sc; _temp1590->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ yyvsp_offset]))->tq; _temp1590->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->type_specs; _temp1590->is_inline= 1; _temp1590->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ yyvsp_offset]))->attributes); _temp1590;}); _temp1589;}); _temp1588;});
break; case 35: _LL1587: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1592=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1592[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1593; _temp1593.tag=
Cyc_StorageClass_tok; _temp1593.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp1593;}); _temp1592;}); break; case 36: _LL1591: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1595=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1595[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1596; _temp1596.tag=
Cyc_StorageClass_tok; _temp1596.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1596;}); _temp1595;}); break; case 37: _LL1594: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1598=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1598[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1599; _temp1599.tag=
Cyc_StorageClass_tok; _temp1599.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1599;}); _temp1598;}); break; case 38: _LL1597: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1601=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1601[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1602; _temp1602.tag=
Cyc_StorageClass_tok; _temp1602.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1602;}); _temp1601;}); break; case 39: _LL1600: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ yyvsp_offset]), _tag_arr("C", sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err(
_tag_arr("only extern or extern \"C\" is allowed", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1604=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1604[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1605; _temp1605.tag= Cyc_StorageClass_tok;
_temp1605.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1605;}); _temp1604;});
break; case 40: _LL1603: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1607=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1607[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1608; _temp1608.tag=
Cyc_StorageClass_tok; _temp1608.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1608;}); _temp1607;}); break; case 41: _LL1606: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1610=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1610[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1611; _temp1611.tag=
Cyc_StorageClass_tok; _temp1611.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1611;}); _temp1610;}); break; case 42: _LL1609: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1613=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1613[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1614; _temp1614.tag=
Cyc_AttributeList_tok; _temp1614.f1= 0; _temp1614;}); _temp1613;}); break; case
43: _LL1612: yyval= yyvs[ yyvsp_offset]; break; case 44: _LL1615: yyval=( void*)({
struct Cyc_AttributeList_tok_struct* _temp1617=( struct Cyc_AttributeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp1617[ 0]=({
struct Cyc_AttributeList_tok_struct _temp1618; _temp1618.tag= Cyc_AttributeList_tok;
_temp1618.f1= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1618;}); _temp1617;}); break; case 45:
_LL1616: yyval=( void*)({ struct Cyc_AttributeList_tok_struct* _temp1620=(
struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1620[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1621; _temp1621.tag=
Cyc_AttributeList_tok; _temp1621.f1=({ struct Cyc_List_List* _temp1622=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1622->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ yyvsp_offset]); _temp1622->tl= 0; _temp1622;});
_temp1621;}); _temp1620;}); break; case 46: _LL1619: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1624=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1624[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1625; _temp1625.tag=
Cyc_AttributeList_tok; _temp1625.f1=({ struct Cyc_List_List* _temp1626=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1626->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1626->tl= Cyc_yyget_AttributeList_tok( yyvs[
yyvsp_offset]); _temp1626;}); _temp1625;}); _temp1624;}); break; case 47:
_LL1623: { struct _tagged_arr _temp1628= Cyc_yyget_String_tok( yyvs[
yyvsp_offset]); if(((( _get_arr_size( _temp1628, sizeof( unsigned char)) >  4?*((
const unsigned char*) _check_unknown_subscript( _temp1628, sizeof( unsigned char),
0)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1628,
sizeof( unsigned char), 1)) == '_': 0)?*(( const unsigned char*)
_check_unknown_subscript( _temp1628, sizeof( unsigned char),( int)(
_get_arr_size( _temp1628, sizeof( unsigned char)) -  2))) == '_': 0)?*(( const
unsigned char*) _check_unknown_subscript( _temp1628, sizeof( unsigned char),(
int)( _get_arr_size( _temp1628, sizeof( unsigned char)) -  3))) == '_': 0){
_temp1628=( struct _tagged_arr) Cyc_Std_substring( _temp1628, 2, _get_arr_size(
_temp1628, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0; i <  16u; ++ i){
if( Cyc_Std_strcmp( _temp1628,( Cyc_Parse_att_map[ i]).f1) ==  0){ yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1629=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1629[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1630; _temp1630.tag= Cyc_Attribute_tok; _temp1630.f1=(
void*)( Cyc_Parse_att_map[ i]).f2; _temp1630;}); _temp1629;}); break;}} if( i == 
16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1631=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1631[ 0]=({ struct Cyc_Attribute_tok_struct _temp1632; _temp1632.tag= Cyc_Attribute_tok;
_temp1632.f1=( void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1632;}); _temp1631;});}
break;}} case 48: _LL1627: yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1634=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1634[ 0]=({ struct Cyc_Attribute_tok_struct _temp1635; _temp1635.tag= Cyc_Attribute_tok;
_temp1635.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1635;}); _temp1634;});
break; case 49: _LL1633: { struct _tagged_arr _temp1637= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1640; int _temp1641; struct _tuple15* _temp1638= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1640=*
_temp1638; _LL1642: _temp1641= _temp1640.f2; goto _LL1639; _LL1639: { void* a;
if( Cyc_Std_zstrcmp( _temp1637, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1637, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1641 <  0? 1: _temp1641 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 0 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1643=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1643[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1644; _temp1644.tag= Cyc_Absyn_Regparm_att; _temp1644.f1= _temp1641;
_temp1644;}); _temp1643;});} else{ if( Cyc_Std_zstrcmp( _temp1637, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1637, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1641 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1641;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1645=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1645[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1646; _temp1646.tag= Cyc_Absyn_Aligned_att; _temp1646.f1= _temp1641;
_temp1646;}); _temp1645;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1647=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1647[ 0]=({ struct Cyc_Attribute_tok_struct _temp1648; _temp1648.tag= Cyc_Attribute_tok;
_temp1648.f1=( void*) a; _temp1648;}); _temp1647;}); break;}} case 50: _LL1636: {
struct _tagged_arr _temp1650= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1651= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1650, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1650, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1652=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1652[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1653; _temp1653.tag=
Cyc_Absyn_Section_att; _temp1653.f1= _temp1651; _temp1653;}); _temp1652;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1654=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1654[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1655; _temp1655.tag= Cyc_Attribute_tok; _temp1655.f1=(
void*) a; _temp1655;}); _temp1654;}); break;} case 51: _LL1649: { struct
_tagged_arr _temp1657= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1658= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1661; struct _tuple15 _temp1659=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1662:
_temp1661= _temp1659.f2; goto _LL1660; _LL1660: { int _temp1665; struct _tuple15
_temp1663=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1666: _temp1665= _temp1663.f2; goto _LL1664; _LL1664: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1657, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1657, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1658, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1667=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1667[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1668; _temp1668.tag=
Cyc_Absyn_Format_att; _temp1668.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1668.f2= _temp1661; _temp1668.f3= _temp1665; _temp1668;}); _temp1667;});}
else{ if( Cyc_Std_zstrcmp( _temp1658, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1669=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1669[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1670; _temp1670.tag=
Cyc_Absyn_Format_att; _temp1670.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1670.f2= _temp1661; _temp1670.f3= _temp1665; _temp1670;}); _temp1669;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
yylsp_offset]).last_line));} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1671=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1671[ 0]=({ struct Cyc_Attribute_tok_struct _temp1672; _temp1672.tag= Cyc_Attribute_tok;
_temp1672.f1=( void*) a; _temp1672;}); _temp1671;}); break;}}} case 52: _LL1656:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1674=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1674[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1675; _temp1675.tag= Cyc_TypeSpecifier_tok;
_temp1675.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1675;});
_temp1674;}); break; case 53: _LL1673: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1677=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1677[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1678; _temp1678.tag=
Cyc_TypeSpecifier_tok; _temp1678.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1678;}); _temp1677;}); break; case 54: _LL1676:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1680=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1680[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1681; _temp1681.tag= Cyc_TypeSpecifier_tok;
_temp1681.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1681;});
_temp1680;}); break; case 55: _LL1679: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1683=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1683[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1684; _temp1684.tag=
Cyc_TypeSpecifier_tok; _temp1684.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1685=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1685[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1686; _temp1686.tag=
Cyc_Parse_Short_spec; _temp1686.f1= Cyc_Position_segment_of_abs(( yyls[
yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line); _temp1686;});
_temp1685;})); _temp1684;}); _temp1683;}); break; case 56: _LL1682: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1688=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1688[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1689; _temp1689.tag= Cyc_TypeSpecifier_tok;
_temp1689.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1689;});
_temp1688;}); break; case 57: _LL1687: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1691=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1691[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1692; _temp1692.tag=
Cyc_TypeSpecifier_tok; _temp1692.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1693=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1693[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1694; _temp1694.tag= Cyc_Parse_Long_spec;
_temp1694.f1= Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line); _temp1694;}); _temp1693;})); _temp1692;});
_temp1691;}); break; case 58: _LL1690: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1696=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1696[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1697; _temp1697.tag=
Cyc_TypeSpecifier_tok; _temp1697.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1697;}); _temp1696;}); break; case 59: _LL1695:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1699=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1699[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1700; _temp1700.tag= Cyc_TypeSpecifier_tok;
_temp1700.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t( 0), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1700;});
_temp1699;}); break; case 60: _LL1698: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1702=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1702[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1703; _temp1703.tag=
Cyc_TypeSpecifier_tok; _temp1703.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1704=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1704[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1705; _temp1705.tag=
Cyc_Parse_Signed_spec; _temp1705.f1= Cyc_Position_segment_of_abs(( yyls[
yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line); _temp1705;});
_temp1704;})); _temp1703;}); _temp1702;}); break; case 61: _LL1701: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1707=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1707[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1708; _temp1708.tag= Cyc_TypeSpecifier_tok;
_temp1708.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1709=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1709[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1710; _temp1710.tag= Cyc_Parse_Unsigned_spec; _temp1710.f1= Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line); _temp1710;});
_temp1709;})); _temp1708;}); _temp1707;}); break; case 62: _LL1706: yyval= yyvs[
yyvsp_offset]; break; case 63: _LL1711: yyval= yyvs[ yyvsp_offset]; break; case
64: _LL1712: yyval= yyvs[ yyvsp_offset]; break; case 65: _LL1713: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1715=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1715[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1716; _temp1716.tag= Cyc_TypeSpecifier_tok;
_temp1716.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok( yyvs[
yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1716;}); _temp1715;}); break; case 66:
_LL1714: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1718=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1718[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1719; _temp1719.tag=
Cyc_TypeSpecifier_tok; _temp1719.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1720=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1720[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1721; _temp1721.tag= Cyc_Absyn_TypedefType;
_temp1721.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1721.f2= Cyc_yyget_TypeList_tok( yyvs[ yyvsp_offset]);
_temp1721.f3= 0; _temp1721;}); _temp1720;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1719;}); _temp1718;}); break; case 67: _LL1717:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1723=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1723[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1724; _temp1724.tag= Cyc_TypeSpecifier_tok;
_temp1724.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1725=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1725[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1726; _temp1726.tag= Cyc_Absyn_TupleType;
_temp1726.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1726;});
_temp1725;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp1724;}); _temp1723;}); break; case 68: _LL1722: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1728=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1728[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1729; _temp1729.tag=
Cyc_TypeSpecifier_tok; _temp1729.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1730=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1730[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1731; _temp1731.tag= Cyc_Absyn_RgnHandleType;
_temp1731.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1731;}); _temp1730;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1729;}); _temp1728;}); break; case 69:
_LL1727: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1733=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1733[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1734; _temp1734.tag=
Cyc_TypeSpecifier_tok; _temp1734.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_SizeofType_struct* _temp1735=( struct Cyc_Absyn_SizeofType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SizeofType_struct)); _temp1735[ 0]=({ struct
Cyc_Absyn_SizeofType_struct _temp1736; _temp1736.tag= Cyc_Absyn_SizeofType;
_temp1736.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1736;}); _temp1735;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1734;}); _temp1733;}); break; case 70:
_LL1732: yyval=( void*)({ struct Cyc_Kind_tok_struct* _temp1738=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1738[ 0]=({ struct Cyc_Kind_tok_struct
_temp1739; _temp1739.tag= Cyc_Kind_tok; _temp1739.f1=( void*) Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp1739;});
_temp1738;}); break; case 71: _LL1737: { struct _tagged_arr* _temp1743; void*
_temp1745; struct _tuple1 _temp1741=* Cyc_yyget_QualId_tok( yyvs[ yyvsp_offset]);
_LL1746: _temp1745= _temp1741.f1; goto _LL1744; _LL1744: _temp1743= _temp1741.f2;
goto _LL1742; _LL1742: if( _temp1745 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err(
_tag_arr("bad kind in type specifier", sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1747=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1747[ 0]=({ struct Cyc_Kind_tok_struct
_temp1748; _temp1748.tag= Cyc_Kind_tok; _temp1748.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1743, Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1748;}); _temp1747;}); break;} case 72: _LL1740:
yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1750=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1750[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1751; _temp1751.tag= Cyc_TypeQual_tok;
_temp1751.f1=({ struct Cyc_Absyn_Tqual _temp1752; _temp1752.q_const= 1;
_temp1752.q_volatile= 0; _temp1752.q_restrict= 0; _temp1752;}); _temp1751;});
_temp1750;}); break; case 73: _LL1749: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1754=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1754[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1755; _temp1755.tag= Cyc_TypeQual_tok; _temp1755.f1=({ struct Cyc_Absyn_Tqual
_temp1756; _temp1756.q_const= 0; _temp1756.q_volatile= 1; _temp1756.q_restrict=
0; _temp1756;}); _temp1755;}); _temp1754;}); break; case 74: _LL1753: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1758=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1758[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1759; _temp1759.tag= Cyc_TypeQual_tok;
_temp1759.f1=({ struct Cyc_Absyn_Tqual _temp1760; _temp1760.q_const= 0;
_temp1760.q_volatile= 0; _temp1760.q_restrict= 1; _temp1760;}); _temp1759;});
_temp1758;}); break; case 75: _LL1757: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1762=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1762[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1763; _temp1763.tag=
Cyc_TypeSpecifier_tok; _temp1763.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1764=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1764[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1765; _temp1765.tag= Cyc_Parse_Decl_spec;
_temp1765.f1=({ struct Cyc_Absyn_Decl* _temp1766=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1766->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1767=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1767[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1768; _temp1768.tag= Cyc_Absyn_Enum_d; _temp1768.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1769=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1769->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1769->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1769->fields=({
struct Cyc_Core_Opt* _temp1770=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1770->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1770;});
_temp1769;}); _temp1768;}); _temp1767;})); _temp1766->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line); _temp1766;}); _temp1765;}); _temp1764;}));
_temp1763;}); _temp1762;}); break; case 76: _LL1761: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1772=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1772[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1773; _temp1773.tag=
Cyc_TypeSpecifier_tok; _temp1773.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1774=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1774[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1775; _temp1775.tag= Cyc_Absyn_EnumType;
_temp1775.f1= Cyc_yyget_QualId_tok( yyvs[ yyvsp_offset]); _temp1775.f2= 0;
_temp1775;}); _temp1774;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1773;}); _temp1772;}); break; case 77: _LL1771:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1777=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1777[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1778; _temp1778.tag= Cyc_TypeSpecifier_tok;
_temp1778.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct* _temp1779=(
struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1779[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1780; _temp1780.tag= Cyc_Parse_Type_spec;
_temp1780.f1=( void*)(( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp1781=(
struct Cyc_Absyn_AnonEnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct));
_temp1781[ 0]=({ struct Cyc_Absyn_AnonEnumType_struct _temp1782; _temp1782.tag=
Cyc_Absyn_AnonEnumType; _temp1782.f1= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1782;});
_temp1781;})); _temp1780.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp1780;}); _temp1779;})); _temp1778;}); _temp1777;});
break; case 78: _LL1776: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1784=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1784[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1785; _temp1785.tag= Cyc_Enumfield_tok;
_temp1785.f1=({ struct Cyc_Absyn_Enumfield* _temp1786=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1786->name= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); _temp1786->tag= 0; _temp1786->loc= Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line); _temp1786;});
_temp1785;}); _temp1784;}); break; case 79: _LL1783: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1788=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1788[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1789; _temp1789.tag= Cyc_Enumfield_tok;
_temp1789.f1=({ struct Cyc_Absyn_Enumfield* _temp1790=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1790->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1790->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp1790->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line); _temp1790;});
_temp1789;}); _temp1788;}); break; case 80: _LL1787: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1792=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1792[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1793; _temp1793.tag=
Cyc_EnumfieldList_tok; _temp1793.f1=({ struct Cyc_List_List* _temp1794=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1794->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ yyvsp_offset]); _temp1794->tl= 0; _temp1794;});
_temp1793;}); _temp1792;}); break; case 81: _LL1791: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1796=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1796[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1797; _temp1797.tag=
Cyc_EnumfieldList_tok; _temp1797.f1=({ struct Cyc_List_List* _temp1798=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1798->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1798->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
yyvsp_offset]); _temp1798;}); _temp1797;}); _temp1796;}); break; case 82:
_LL1795: { void* t; switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1800: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1802=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1802[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1803; _temp1803.tag=
Cyc_Absyn_AnonStructType; _temp1803.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1803;});
_temp1802;}); break; case Cyc_Parse_Union_su: _LL1801: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1805=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1805[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1806; _temp1806.tag= Cyc_Absyn_AnonUnionType; _temp1806.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1806;});
_temp1805;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1807=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1807[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1808; _temp1808.tag=
Cyc_TypeSpecifier_tok; _temp1808.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1809=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1809[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1810; _temp1810.tag= Cyc_Parse_Type_spec;
_temp1810.f1=( void*) t; _temp1810.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp1810;}); _temp1809;})); _temp1808;}); _temp1807;});
break;} case 83: _LL1799: { struct Cyc_List_List* _temp1812=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1813: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1815=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1815->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1815;}), _temp1812,({ struct Cyc_Core_Opt*
_temp1816=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1816->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1816;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line)); break; case
Cyc_Parse_Union_su: _LL1814: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp1818=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1818->v=( void*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1818;}),
_temp1812,({ struct Cyc_Core_Opt* _temp1819=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1819->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1819;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line)); break;}
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1820=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1820[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1821; _temp1821.tag= Cyc_TypeSpecifier_tok;
_temp1821.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1822=(
struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1822[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1823; _temp1823.tag= Cyc_Parse_Decl_spec;
_temp1823.f1= d; _temp1823;}); _temp1822;})); _temp1821;}); _temp1820;}); break;}
case 84: _LL1811: { struct Cyc_List_List* _temp1825=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1826: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1828=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1828->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1828;}), _temp1825,({ struct Cyc_Core_Opt*
_temp1829=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1829->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1829;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line)); break; case
Cyc_Parse_Union_su: _LL1827: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp1831=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1831->v=( void*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1831;}),
_temp1825,({ struct Cyc_Core_Opt* _temp1832=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1832->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1832;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line)); break;}
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1833=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1833[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1834; _temp1834.tag= Cyc_TypeSpecifier_tok;
_temp1834.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1835=(
struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1835[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1836; _temp1836.tag= Cyc_Parse_Decl_spec;
_temp1836.f1= d; _temp1836;}); _temp1835;})); _temp1834;}); _temp1833;}); break;}
case 85: _LL1824: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1838: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1840=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1840[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1841; _temp1841.tag=
Cyc_TypeSpecifier_tok; _temp1841.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1842=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1842[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1843; _temp1843.tag= Cyc_Absyn_StructType;
_temp1843.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1843.f2= Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset]); _temp1843.f3= 0; _temp1843;}); _temp1842;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1841;}); _temp1840;}); break; case Cyc_Parse_Union_su:
_LL1839: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1845=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1845[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1846; _temp1846.tag=
Cyc_TypeSpecifier_tok; _temp1846.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1847=( struct Cyc_Absyn_UnionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1847[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1848; _temp1848.tag= Cyc_Absyn_UnionType;
_temp1848.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1848.f2= Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset]); _temp1848.f3= 0; _temp1848;}); _temp1847;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1846;}); _temp1845;}); break;} break;
case 86: _LL1837: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1850: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1852=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1852[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1853; _temp1853.tag=
Cyc_TypeSpecifier_tok; _temp1853.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1854=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1854[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1855; _temp1855.tag= Cyc_Absyn_StructType;
_temp1855.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1855.f2= Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset]); _temp1855.f3= 0; _temp1855;}); _temp1854;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1853;}); _temp1852;}); break; case Cyc_Parse_Union_su:
_LL1851: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1857=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1857[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1858; _temp1858.tag=
Cyc_TypeSpecifier_tok; _temp1858.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1859=( struct Cyc_Absyn_UnionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1859[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1860; _temp1860.tag= Cyc_Absyn_UnionType;
_temp1860.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1860.f2= Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset]); _temp1860.f3= 0; _temp1860;}); _temp1859;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1858;}); _temp1857;}); break;} break;
case 87: _LL1849: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp1862=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1862[ 0]=({ struct Cyc_TypeList_tok_struct _temp1863; _temp1863.tag= Cyc_TypeList_tok;
_temp1863.f1= 0; _temp1863;}); _temp1862;}); break; case 88: _LL1861: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1865=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1865[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1866; _temp1866.tag= Cyc_TypeList_tok; _temp1866.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1866;});
_temp1865;}); break; case 89: _LL1864:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1868=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1868[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1869; _temp1869.tag= Cyc_TypeList_tok; _temp1869.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1869;});
_temp1868;}); break; case 90: _LL1867: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1871=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1871[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1872; _temp1872.tag= Cyc_StructOrUnion_tok; _temp1872.f1= Cyc_Parse_Struct_su;
_temp1872;}); _temp1871;}); break; case 91: _LL1870: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1874=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1874[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1875; _temp1875.tag= Cyc_StructOrUnion_tok; _temp1875.f1= Cyc_Parse_Union_su;
_temp1875;}); _temp1874;}); break; case 92: _LL1873: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1877=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1877[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1878; _temp1878.tag= Cyc_StructFieldDeclList_tok; _temp1878.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ yyvsp_offset]))); _temp1878;}); _temp1877;}); break; case 93: _LL1876:
yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct* _temp1880=(
struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct));
_temp1880[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct _temp1881;
_temp1881.tag= Cyc_StructFieldDeclListList_tok; _temp1881.f1=({ struct Cyc_List_List*
_temp1882=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1882->hd=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[ yyvsp_offset]);
_temp1882->tl= 0; _temp1882;}); _temp1881;}); _temp1880;}); break; case 94:
_LL1879: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1884=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1884[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1885; _temp1885.tag= Cyc_StructFieldDeclListList_tok; _temp1885.f1=({
struct Cyc_List_List* _temp1886=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1886->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ yyvsp_offset]); _temp1886->tl= Cyc_yyget_StructFieldDeclListList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1886;});
_temp1885;}); _temp1884;}); break; case 95: _LL1883: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1888=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1888[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1889; _temp1889.tag=
Cyc_InitDeclList_tok; _temp1889.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[ yyvsp_offset]));
_temp1889;}); _temp1888;}); break; case 96: _LL1887: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1891=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1891[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1892; _temp1892.tag=
Cyc_InitDeclList_tok; _temp1892.f1=({ struct Cyc_List_List* _temp1893=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1893->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ yyvsp_offset]); _temp1893->tl= 0; _temp1893;});
_temp1892;}); _temp1891;}); break; case 97: _LL1890: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1895=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1895[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1896; _temp1896.tag=
Cyc_InitDeclList_tok; _temp1896.f1=({ struct Cyc_List_List* _temp1897=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1897->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ yyvsp_offset]); _temp1897->tl= Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1897;});
_temp1896;}); _temp1895;}); break; case 98: _LL1894: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1899=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1899[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1900; _temp1900.tag= Cyc_InitDecl_tok;
_temp1900.f1=({ struct _tuple14* _temp1901=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1901->f1= Cyc_yyget_Declarator_tok( yyvs[
yyvsp_offset]); _temp1901->f2= 0; _temp1901;}); _temp1900;}); _temp1899;});
break; case 99: _LL1898: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1903=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1903[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1904; _temp1904.tag= Cyc_InitDecl_tok;
_temp1904.f1=({ struct _tuple14* _temp1905=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1905->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1905->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp1905;});
_temp1904;}); _temp1903;}); break; case 100: _LL1902: { struct _tuple17
_temp1909; struct Cyc_List_List* _temp1910; struct Cyc_List_List* _temp1912;
struct Cyc_Absyn_Tqual _temp1914; struct _tuple17* _temp1907= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1909=*
_temp1907; _LL1915: _temp1914= _temp1909.f1; goto _LL1913; _LL1913: _temp1912=
_temp1909.f2; goto _LL1911; _LL1911: _temp1910= _temp1909.f3; goto _LL1908;
_LL1908: { struct Cyc_List_List* _temp1918; struct Cyc_List_List* _temp1920;
struct _tuple0 _temp1916=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1921: _temp1920= _temp1916.f1; goto _LL1919; _LL1919:
_temp1918= _temp1916.f2; goto _LL1917; _LL1917: { void* _temp1922= Cyc_Parse_speclist2typ(
_temp1912, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1923=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1914, _temp1922, _temp1920, _temp1910), _temp1918);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1924=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1924[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1925; _temp1925.tag=
Cyc_StructFieldDeclList_tok; _temp1925.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1923); _temp1925;}); _temp1924;}); break;}}}
case 101: _LL1906: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1927=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1927[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1928; _temp1928.tag=
Cyc_QualSpecList_tok; _temp1928.f1=({ struct _tuple17* _temp1929=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1929->f1= Cyc_Absyn_empty_tqual();
_temp1929->f2=({ struct Cyc_List_List* _temp1930=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1930->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1930->tl=
0; _temp1930;}); _temp1929->f3= Cyc_yyget_AttributeList_tok( yyvs[ yyvsp_offset]);
_temp1929;}); _temp1928;}); _temp1927;}); break; case 102: _LL1926: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp1932=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1932[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1933; _temp1933.tag= Cyc_QualSpecList_tok;
_temp1933.f1=({ struct _tuple17* _temp1934=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1934->f1=(* Cyc_yyget_QualSpecList_tok( yyvs[
yyvsp_offset])).f1; _temp1934->f2=({ struct Cyc_List_List* _temp1935=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1935->hd=( void*)
Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1935->tl=(* Cyc_yyget_QualSpecList_tok( yyvs[
yyvsp_offset])).f2; _temp1935;}); _temp1934->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ yyvsp_offset])).f3); _temp1934;}); _temp1933;}); _temp1932;}); break; case
103: _LL1931: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1937=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1937[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1938; _temp1938.tag=
Cyc_QualSpecList_tok; _temp1938.f1=({ struct _tuple17* _temp1939=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1939->f1= Cyc_yyget_TypeQual_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1939->f2=
0; _temp1939->f3= Cyc_yyget_AttributeList_tok( yyvs[ yyvsp_offset]); _temp1939;});
_temp1938;}); _temp1937;}); break; case 104: _LL1936: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1941=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1941[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1942; _temp1942.tag= Cyc_QualSpecList_tok;
_temp1942.f1=({ struct _tuple17* _temp1943=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1943->f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ yyvsp_offset])).f1); _temp1943->f2=(* Cyc_yyget_QualSpecList_tok( yyvs[
yyvsp_offset])).f2; _temp1943->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ yyvsp_offset])).f3); _temp1943;}); _temp1942;}); _temp1941;}); break; case
105: _LL1940: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1945=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1945[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1946; _temp1946.tag=
Cyc_InitDeclList_tok; _temp1946.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[ yyvsp_offset]));
_temp1946;}); _temp1945;}); break; case 106: _LL1944: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1948=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1948[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1949; _temp1949.tag= Cyc_InitDeclList_tok;
_temp1949.f1=({ struct Cyc_List_List* _temp1950=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1950->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ yyvsp_offset]); _temp1950->tl= 0; _temp1950;}); _temp1949;}); _temp1948;});
break; case 107: _LL1947: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1952=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1952[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1953; _temp1953.tag=
Cyc_InitDeclList_tok; _temp1953.f1=({ struct Cyc_List_List* _temp1954=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1954->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ yyvsp_offset]); _temp1954->tl= Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1954;});
_temp1953;}); _temp1952;}); break; case 108: _LL1951: yyval=( void*)({ struct
Cyc_InitDecl_tok_struct* _temp1956=( struct Cyc_InitDecl_tok_struct*) _cycalloc(
sizeof( struct Cyc_InitDecl_tok_struct)); _temp1956[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1957; _temp1957.tag= Cyc_InitDecl_tok; _temp1957.f1=({ struct _tuple14*
_temp1958=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1958->f1=
Cyc_yyget_Declarator_tok( yyvs[ yyvsp_offset]); _temp1958->f2= 0; _temp1958;});
_temp1957;}); _temp1956;}); break; case 109: _LL1955: yyval=( void*)({ struct
Cyc_InitDecl_tok_struct* _temp1960=( struct Cyc_InitDecl_tok_struct*) _cycalloc(
sizeof( struct Cyc_InitDecl_tok_struct)); _temp1960[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1961; _temp1961.tag= Cyc_InitDecl_tok; _temp1961.f1=({ struct _tuple14*
_temp1962=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1962->f1=({
struct Cyc_Parse_Declarator* _temp1963=( struct Cyc_Parse_Declarator*) _cycalloc(
sizeof( struct Cyc_Parse_Declarator)); _temp1963->id=({ struct _tuple1*
_temp1964=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1964->f1=
Cyc_Absyn_rel_ns_null; _temp1964->f2= _init_tag_arr(( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char), 1u);
_temp1964;}); _temp1963->tms= 0; _temp1963;}); _temp1962->f2=( struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp1962;}); _temp1961;}); _temp1960;});
break; case 110: _LL1959: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1966=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1966[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1967; _temp1967.tag= Cyc_InitDecl_tok;
_temp1967.f1=({ struct _tuple14* _temp1968=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1968->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1968->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp1968;});
_temp1967;}); _temp1966;}); break; case 111: _LL1965: { struct Cyc_List_List*
_temp1970=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1971=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1971[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1972; _temp1972.tag= Cyc_TypeSpecifier_tok;
_temp1972.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1973=(
struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1973[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1974; _temp1974.tag= Cyc_Parse_Decl_spec;
_temp1974.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1970,({
struct Cyc_Core_Opt* _temp1975=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1975->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1975;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1974;}); _temp1973;})); _temp1972;});
_temp1971;}); break;} case 112: _LL1969: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1977=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1977[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1978; _temp1978.tag=
Cyc_TypeSpecifier_tok; _temp1978.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1979=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1979[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1980; _temp1980.tag= Cyc_Absyn_TunionType;
_temp1980.f1=({ struct Cyc_Absyn_TunionInfo _temp1981; _temp1981.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1982=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1982[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1983; _temp1983.tag= Cyc_Absyn_UnknownTunion;
_temp1983.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1984; _temp1984.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1984.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1984;});
_temp1983;}); _temp1982;})); _temp1981.targs= Cyc_yyget_TypeList_tok( yyvs[
yyvsp_offset]); _temp1981.rgn=( void*) Cyc_yyget_Type_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1981;});
_temp1980;}); _temp1979;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp1978;}); _temp1977;}); break; case 113: _LL1976: {
void* _temp1986= Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp1995=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1995->v=( void*)((
void*) Cyc_Absyn_RgnKind); _temp1995;}), 0); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1987=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1987[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1988; _temp1988.tag=
Cyc_TypeSpecifier_tok; _temp1988.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1989=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1989[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1990; _temp1990.tag= Cyc_Absyn_TunionType;
_temp1990.f1=({ struct Cyc_Absyn_TunionInfo _temp1991; _temp1991.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1992=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1992[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1993; _temp1993.tag= Cyc_Absyn_UnknownTunion;
_temp1993.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1994; _temp1994.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1994.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1994;});
_temp1993;}); _temp1992;})); _temp1991.targs= Cyc_yyget_TypeList_tok( yyvs[
yyvsp_offset]); _temp1991.rgn=( void*) _temp1986; _temp1991;}); _temp1990;});
_temp1989;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp1988;}); _temp1987;}); break;} case 114: _LL1985: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1997=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1997[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1998; _temp1998.tag= Cyc_TypeSpecifier_tok;
_temp1998.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1999=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1999[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2000; _temp2000.tag= Cyc_Absyn_TunionFieldType; _temp2000.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2001; _temp2001.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2002=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2002[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2003; _temp2003.tag= Cyc_Absyn_UnknownTunionfield; _temp2003.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2004; _temp2004.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2004.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp2004.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2004;});
_temp2003;}); _temp2002;})); _temp2001.targs= Cyc_yyget_TypeList_tok( yyvs[
yyvsp_offset]); _temp2001;}); _temp2000;}); _temp1999;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp1998;}); _temp1997;}); break; case 115:
_LL1996: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2006=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp2006[ 0]=({ struct
Cyc_Bool_tok_struct _temp2007; _temp2007.tag= Cyc_Bool_tok; _temp2007.f1= 0;
_temp2007;}); _temp2006;}); break; case 116: _LL2005: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2009=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2009[ 0]=({ struct Cyc_Bool_tok_struct
_temp2010; _temp2010.tag= Cyc_Bool_tok; _temp2010.f1= 1; _temp2010;}); _temp2009;});
break; case 117: _LL2008: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2012=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2012[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2013; _temp2013.tag= Cyc_TunionFieldList_tok; _temp2013.f1=({ struct Cyc_List_List*
_temp2014=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2014->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[ yyvsp_offset]);
_temp2014->tl= 0; _temp2014;}); _temp2013;}); _temp2012;}); break; case 118:
_LL2011: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2016=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2016[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2017; _temp2017.tag=
Cyc_TunionFieldList_tok; _temp2017.f1=({ struct Cyc_List_List* _temp2018=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2018->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2018->tl= 0; _temp2018;}); _temp2017;}); _temp2016;});
break; case 119: _LL2015: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2020=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2020[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2021; _temp2021.tag= Cyc_TunionFieldList_tok; _temp2021.f1=({ struct Cyc_List_List*
_temp2022=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2022->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2022->tl= Cyc_yyget_TunionFieldList_tok(
yyvs[ yyvsp_offset]); _temp2022;}); _temp2021;}); _temp2020;}); break; case 120:
_LL2019: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2024=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2024[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2025; _temp2025.tag=
Cyc_TunionFieldList_tok; _temp2025.f1=({ struct Cyc_List_List* _temp2026=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2026->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2026->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
yyvsp_offset]); _temp2026;}); _temp2025;}); _temp2024;}); break; case 121:
_LL2023: yyval=( void*)({ struct Cyc_Scope_tok_struct* _temp2028=( struct Cyc_Scope_tok_struct*)
_cycalloc( sizeof( struct Cyc_Scope_tok_struct)); _temp2028[ 0]=({ struct Cyc_Scope_tok_struct
_temp2029; _temp2029.tag= Cyc_Scope_tok; _temp2029.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp2029;}); _temp2028;}); break; case 122: _LL2027: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2031=( struct Cyc_Scope_tok_struct*) _cycalloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2031[ 0]=({ struct Cyc_Scope_tok_struct
_temp2032; _temp2032.tag= Cyc_Scope_tok; _temp2032.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp2032;}); _temp2031;}); break; case 123: _LL2030: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2034=( struct Cyc_Scope_tok_struct*) _cycalloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2034[ 0]=({ struct Cyc_Scope_tok_struct
_temp2035; _temp2035.tag= Cyc_Scope_tok; _temp2035.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp2035;}); _temp2034;}); break; case 124: _LL2033: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp2037=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2037[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2038; _temp2038.tag= Cyc_TunionField_tok;
_temp2038.f1=({ struct Cyc_Absyn_Tunionfield* _temp2039=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2039->name= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); _temp2039->tvs= 0; _temp2039->typs= 0; _temp2039->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line); _temp2039->sc=( void*) Cyc_yyget_Scope_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2039;});
_temp2038;}); _temp2037;}); break; case 125: _LL2036: { struct Cyc_List_List*
_temp2041=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp2042=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp2043=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2043[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2044; _temp2044.tag= Cyc_TunionField_tok;
_temp2044.f1=({ struct Cyc_Absyn_Tunionfield* _temp2045=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2045->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2045->tvs=
_temp2042; _temp2045->typs= _temp2041; _temp2045->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ yylsp_offset]).last_line); _temp2045->sc=( void*) Cyc_yyget_Scope_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2045;});
_temp2044;}); _temp2043;}); break;} case 126: _LL2040: yyval= yyvs[ yyvsp_offset];
break; case 127: _LL2046: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2048=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2048[ 0]=({ struct Cyc_Declarator_tok_struct _temp2049; _temp2049.tag= Cyc_Declarator_tok;
_temp2049.f1=({ struct Cyc_Parse_Declarator* _temp2050=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2050->id=( Cyc_yyget_Declarator_tok(
yyvs[ yyvsp_offset]))->id; _temp2050->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ yyvsp_offset]))->tms); _temp2050;}); _temp2049;}); _temp2048;}); break;
case 128: _LL2047: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2052=(
struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2052[ 0]=({ struct Cyc_Declarator_tok_struct _temp2053; _temp2053.tag= Cyc_Declarator_tok;
_temp2053.f1=({ struct Cyc_Parse_Declarator* _temp2054=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2054->id= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); _temp2054->tms= 0; _temp2054;}); _temp2053;}); _temp2052;});
break; case 129: _LL2051: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; break; case 130: _LL2055: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2057=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2057[ 0]=({ struct Cyc_Declarator_tok_struct _temp2058; _temp2058.tag= Cyc_Declarator_tok;
_temp2058.f1=({ struct Cyc_Parse_Declarator* _temp2059=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2059->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp2059->tms=({ struct Cyc_List_List* _temp2060=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2060->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2060->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp2060;}); _temp2059;}); _temp2058;});
_temp2057;}); break; case 131: _LL2056: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2062=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2062[ 0]=({ struct Cyc_Declarator_tok_struct _temp2063; _temp2063.tag= Cyc_Declarator_tok;
_temp2063.f1=({ struct Cyc_Parse_Declarator* _temp2064=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2064->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2064->tms=({ struct Cyc_List_List* _temp2065=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2065->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2066=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2066[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2067; _temp2067.tag= Cyc_Absyn_ConstArray_mod;
_temp2067.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2067;}); _temp2066;})); _temp2065->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2065;}); _temp2064;}); _temp2063;}); _temp2062;}); break; case 132: _LL2061: {
struct _tuple16 _temp2071; struct Cyc_List_List* _temp2072; struct Cyc_Core_Opt*
_temp2074; struct Cyc_Absyn_VarargInfo* _temp2076; int _temp2078; struct Cyc_List_List*
_temp2080; struct _tuple16* _temp2069= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2071=*
_temp2069; _LL2081: _temp2080= _temp2071.f1; goto _LL2079; _LL2079: _temp2078=
_temp2071.f2; goto _LL2077; _LL2077: _temp2076= _temp2071.f3; goto _LL2075;
_LL2075: _temp2074= _temp2071.f4; goto _LL2073; _LL2073: _temp2072= _temp2071.f5;
goto _LL2070; _LL2070: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2082=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2082[ 0]=({ struct Cyc_Declarator_tok_struct _temp2083; _temp2083.tag= Cyc_Declarator_tok;
_temp2083.f1=({ struct Cyc_Parse_Declarator* _temp2084=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2084->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2084->tms=({ struct Cyc_List_List* _temp2085=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2085->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2086=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2086[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2087; _temp2087.tag= Cyc_Absyn_Function_mod;
_temp2087.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2088=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2088[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2089; _temp2089.tag= Cyc_Absyn_WithTypes;
_temp2089.f1= _temp2080; _temp2089.f2= _temp2078; _temp2089.f3= _temp2076;
_temp2089.f4= _temp2074; _temp2089.f5= _temp2072; _temp2089;}); _temp2088;}));
_temp2087;}); _temp2086;})); _temp2085->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2085;});
_temp2084;}); _temp2083;}); _temp2082;}); break;} case 133: _LL2068: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2091=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2091[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2092; _temp2092.tag= Cyc_Declarator_tok;
_temp2092.f1=({ struct Cyc_Parse_Declarator* _temp2093=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2093->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2093->tms=({ struct Cyc_List_List* _temp2094=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2094->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2095=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2095[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2096; _temp2096.tag= Cyc_Absyn_Function_mod;
_temp2096.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2097=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2097[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2098; _temp2098.tag= Cyc_Absyn_WithTypes;
_temp2098.f1= 0; _temp2098.f2= 0; _temp2098.f3= 0; _temp2098.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2098.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2098;}); _temp2097;})); _temp2096;}); _temp2095;}));
_temp2094->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2094;}); _temp2093;}); _temp2092;});
_temp2091;}); break; case 134: _LL2090: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2100=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2100[ 0]=({ struct Cyc_Declarator_tok_struct _temp2101; _temp2101.tag= Cyc_Declarator_tok;
_temp2101.f1=({ struct Cyc_Parse_Declarator* _temp2102=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2102->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2102->tms=({ struct Cyc_List_List* _temp2103=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2103->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2104=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2104[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2105; _temp2105.tag= Cyc_Absyn_Function_mod;
_temp2105.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2106=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2106[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2107; _temp2107.tag= Cyc_Absyn_NoTypes;
_temp2107.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2107.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2107;}); _temp2106;})); _temp2105;}); _temp2104;}));
_temp2103->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]))->tms; _temp2103;}); _temp2102;}); _temp2101;});
_temp2100;}); break; case 135: _LL2099: { struct Cyc_List_List* _temp2109=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2110=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2110[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2111; _temp2111.tag= Cyc_Declarator_tok;
_temp2111.f1=({ struct Cyc_Parse_Declarator* _temp2112=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2112->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2112->tms=({ struct Cyc_List_List* _temp2113=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2113->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2114=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2114[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2115; _temp2115.tag= Cyc_Absyn_TypeParams_mod;
_temp2115.f1= _temp2109; _temp2115.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2115.f3= 0; _temp2115;}); _temp2114;}));
_temp2113->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]))->tms; _temp2113;}); _temp2112;}); _temp2111;});
_temp2110;}); break;} case 136: _LL2108:(( struct Cyc_Lexing_lexbuf*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;{ struct Cyc_List_List*
_temp2117=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2118=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2118[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2119; _temp2119.tag= Cyc_Declarator_tok;
_temp2119.f1=({ struct Cyc_Parse_Declarator* _temp2120=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2120->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2120->tms=({ struct Cyc_List_List* _temp2121=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2121->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2122=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2122[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2123; _temp2123.tag= Cyc_Absyn_TypeParams_mod;
_temp2123.f1= _temp2117; _temp2123.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2123.f3= 0; _temp2123;}); _temp2122;}));
_temp2121->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]))->tms; _temp2121;}); _temp2120;}); _temp2119;});
_temp2118;}); break;} case 137: _LL2116: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2125=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2125[ 0]=({ struct Cyc_Declarator_tok_struct _temp2126; _temp2126.tag= Cyc_Declarator_tok;
_temp2126.f1=({ struct Cyc_Parse_Declarator* _temp2127=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2127->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2127->tms=({ struct Cyc_List_List* _temp2128=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2128->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2129=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2129[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2130; _temp2130.tag= Cyc_Absyn_Attributes_mod;
_temp2130.f1= Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line); _temp2130.f2= Cyc_yyget_AttributeList_tok( yyvs[
yyvsp_offset]); _temp2130;}); _temp2129;})); _temp2128->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms;
_temp2128;}); _temp2127;}); _temp2126;}); _temp2125;}); break; case 138: _LL2124:
Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef", sizeof(
unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2132=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2132[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2133; _temp2133.tag= Cyc_Declarator_tok;
_temp2133.f1=({ struct Cyc_Parse_Declarator* _temp2134=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2134->id= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); _temp2134->tms= 0; _temp2134;}); _temp2133;}); _temp2132;});
break; case 139: _LL2131: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2136=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2136[ 0]=({ struct Cyc_Declarator_tok_struct _temp2137; _temp2137.tag= Cyc_Declarator_tok;
_temp2137.f1=({ struct Cyc_Parse_Declarator* _temp2138=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2138->id= Cyc_Absyn_exn_name;
_temp2138->tms= 0; _temp2138;}); _temp2137;}); _temp2136;}); break; case 140:
_LL2135: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2140=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2140[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2141; _temp2141.tag=
Cyc_TypeModifierList_tok; _temp2141.f1=({ struct Cyc_List_List* _temp2142=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2142->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2143=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2143[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2144; _temp2144.tag= Cyc_Absyn_Pointer_mod;
_temp2144.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2144.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2144.f3= Cyc_Absyn_empty_tqual(); _temp2144;});
_temp2143;})); _temp2142->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ yyvsp_offset]), 0); _temp2142;}); _temp2141;}); _temp2140;}); break; case
141: _LL2139: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2146=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2146[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2147; _temp2147.tag=
Cyc_TypeModifierList_tok; _temp2147.f1=({ struct Cyc_List_List* _temp2148=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2148->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2149=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2149[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2150; _temp2150.tag= Cyc_Absyn_Pointer_mod;
_temp2150.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2150.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2150.f3= Cyc_yyget_TypeQual_tok( yyvs[ yyvsp_offset]);
_temp2150;}); _temp2149;})); _temp2148->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0); _temp2148;}); _temp2147;}); _temp2146;}); break; case
142: _LL2145: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2152=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2152[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2153; _temp2153.tag=
Cyc_TypeModifierList_tok; _temp2153.f1=({ struct Cyc_List_List* _temp2154=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2154->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2155=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2155[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2156; _temp2156.tag= Cyc_Absyn_Pointer_mod;
_temp2156.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2156.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2156.f3= Cyc_Absyn_empty_tqual(); _temp2156;});
_temp2155;})); _temp2154->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[ yyvsp_offset]));
_temp2154;}); _temp2153;}); _temp2152;}); break; case 143: _LL2151: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2158=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2158[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2159; _temp2159.tag= Cyc_TypeModifierList_tok;
_temp2159.f1=({ struct Cyc_List_List* _temp2160=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2160->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2161=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2161[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2162; _temp2162.tag= Cyc_Absyn_Pointer_mod;
_temp2162.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2162.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2162.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2162;});
_temp2161;})); _temp2160->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[ yyvsp_offset]));
_temp2160;}); _temp2159;}); _temp2158;}); break; case 144: _LL2157: yyval=( void*)({
struct Cyc_Pointer_Sort_tok_struct* _temp2164=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2164[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2165; _temp2165.tag= Cyc_Pointer_Sort_tok;
_temp2165.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2166=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2166[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2167; _temp2167.tag=
Cyc_Absyn_Nullable_ps; _temp2167.f1= Cyc_Absyn_exp_unsigned_one; _temp2167;});
_temp2166;})); _temp2165;}); _temp2164;}); break; case 145: _LL2163: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2169=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2169[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2170; _temp2170.tag= Cyc_Pointer_Sort_tok;
_temp2170.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2171=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2171[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2172; _temp2172.tag= Cyc_Absyn_NonNullable_ps; _temp2172.f1= Cyc_Absyn_exp_unsigned_one;
_temp2172;}); _temp2171;})); _temp2170;}); _temp2169;}); break; case 146:
_LL2168: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2174=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2174[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2175; _temp2175.tag=
Cyc_Pointer_Sort_tok; _temp2175.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2176=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2176[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2177; _temp2177.tag=
Cyc_Absyn_Nullable_ps; _temp2177.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2177;});
_temp2176;})); _temp2175;}); _temp2174;}); break; case 147: _LL2173: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2179=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2179[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2180; _temp2180.tag= Cyc_Pointer_Sort_tok;
_temp2180.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2181=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2181[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2182; _temp2182.tag= Cyc_Absyn_NonNullable_ps; _temp2182.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2182;});
_temp2181;})); _temp2180;}); _temp2179;}); break; case 148: _LL2178: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2184=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2184[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2185; _temp2185.tag= Cyc_Pointer_Sort_tok;
_temp2185.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2185;});
_temp2184;}); break; case 149: _LL2183: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2187=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2187[ 0]=({ struct Cyc_Type_tok_struct _temp2188; _temp2188.tag= Cyc_Type_tok;
_temp2188.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2189=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2189->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2189;}), 0); _temp2188;}); _temp2187;});
break; case 150: _LL2186: yyval= yyvs[ yyvsp_offset]; break; case 151: _LL2190:
Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ yyvsp_offset]; break; case 152: _LL2191: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2193=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2193[ 0]=({ struct Cyc_Type_tok_struct
_temp2194; _temp2194.tag= Cyc_Type_tok; _temp2194.f1=( void*) Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2195=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2195->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2195;}),
0); _temp2194;}); _temp2193;}); break; case 153: _LL2192: yyval= yyvs[
yyvsp_offset]; break; case 154: _LL2196: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2198=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2198[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2199; _temp2199.tag= Cyc_TypeQual_tok; _temp2199.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[ yyvsp_offset])); _temp2199;});
_temp2198;}); break; case 155: _LL2197: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2201=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2201[ 0]=({ struct Cyc_YY1_struct _temp2202; _temp2202.tag= Cyc_YY1;
_temp2202.f1=({ struct _tuple16* _temp2203=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2203->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2203->f2= 0;
_temp2203->f3= 0; _temp2203->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2203->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ yyvsp_offset]); _temp2203;}); _temp2202;}); _temp2201;}); break; case 156:
_LL2200: yyval=( void*)({ struct Cyc_YY1_struct* _temp2205=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2205[ 0]=({ struct Cyc_YY1_struct
_temp2206; _temp2206.tag= Cyc_YY1; _temp2206.f1=({ struct _tuple16* _temp2207=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2207->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2207->f2=
1; _temp2207->f3= 0; _temp2207->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2207->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ yyvsp_offset]); _temp2207;}); _temp2206;}); _temp2205;}); break; case 157:
_LL2204: { struct _tuple2 _temp2211; void* _temp2212; struct Cyc_Absyn_Tqual
_temp2214; struct Cyc_Core_Opt* _temp2216; struct _tuple2* _temp2209= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2211=*
_temp2209; _LL2217: _temp2216= _temp2211.f1; goto _LL2215; _LL2215: _temp2214=
_temp2211.f2; goto _LL2213; _LL2213: _temp2212= _temp2211.f3; goto _LL2210;
_LL2210: { struct Cyc_Absyn_VarargInfo* _temp2218=({ struct Cyc_Absyn_VarargInfo*
_temp2222=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2222->name= _temp2216; _temp2222->tq= _temp2214; _temp2222->type=( void*)
_temp2212; _temp2222->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2222;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2219=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2219[ 0]=({ struct Cyc_YY1_struct
_temp2220; _temp2220.tag= Cyc_YY1; _temp2220.f1=({ struct _tuple16* _temp2221=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2221->f1= 0;
_temp2221->f2= 0; _temp2221->f3= _temp2218; _temp2221->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2221->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ yyvsp_offset]); _temp2221;}); _temp2220;});
_temp2219;}); break;}} case 158: _LL2208: { struct _tuple2 _temp2226; void*
_temp2227; struct Cyc_Absyn_Tqual _temp2229; struct Cyc_Core_Opt* _temp2231;
struct _tuple2* _temp2224= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2226=*
_temp2224; _LL2232: _temp2231= _temp2226.f1; goto _LL2230; _LL2230: _temp2229=
_temp2226.f2; goto _LL2228; _LL2228: _temp2227= _temp2226.f3; goto _LL2225;
_LL2225: { struct Cyc_Absyn_VarargInfo* _temp2233=({ struct Cyc_Absyn_VarargInfo*
_temp2237=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2237->name= _temp2231; _temp2237->tq= _temp2229; _temp2237->type=( void*)
_temp2227; _temp2237->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2237;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2234=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2234[ 0]=({ struct Cyc_YY1_struct
_temp2235; _temp2235.tag= Cyc_YY1; _temp2235.f1=({ struct _tuple16* _temp2236=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2236->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2236->f2=
0; _temp2236->f3= _temp2233; _temp2236->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2236->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ yyvsp_offset]); _temp2236;}); _temp2235;}); _temp2234;}); break;}} case
159: _LL2223: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2239=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2239[
0]=({ struct Cyc_Type_tok_struct _temp2240; _temp2240.tag= Cyc_Type_tok;
_temp2240.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
yyvsp_offset]),( void*)({ struct Cyc_Absyn_Unknown_kb_struct* _temp2241=( struct
Cyc_Absyn_Unknown_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct));
_temp2241[ 0]=({ struct Cyc_Absyn_Unknown_kb_struct _temp2242; _temp2242.tag=
Cyc_Absyn_Unknown_kb; _temp2242.f1= 0; _temp2242;}); _temp2241;})); _temp2240;});
_temp2239;}); break; case 160: _LL2238: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2244=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2244[ 0]=({ struct Cyc_Type_tok_struct _temp2245; _temp2245.tag= Cyc_Type_tok;
_temp2245.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2246=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2246[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2247; _temp2247.tag= Cyc_Absyn_Eq_kb; _temp2247.f1=( void*) Cyc_yyget_Kind_tok(
yyvs[ yyvsp_offset]); _temp2247;}); _temp2246;})); _temp2245;}); _temp2244;});
break; case 161: _LL2243: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2249=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2249[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2250; _temp2250.tag= Cyc_TypeOpt_tok;
_temp2250.f1= 0; _temp2250;}); _temp2249;}); break; case 162: _LL2248: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2252=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2252[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2253; _temp2253.tag= Cyc_TypeOpt_tok; _temp2253.f1=({ struct Cyc_Core_Opt*
_temp2254=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2254->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2255=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2255[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2256; _temp2256.tag= Cyc_Absyn_JoinEff;
_temp2256.f1= Cyc_yyget_TypeList_tok( yyvs[ yyvsp_offset]); _temp2256;});
_temp2255;})); _temp2254;}); _temp2253;}); _temp2252;}); break; case 163:
_LL2251: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2258=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2258[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2259; _temp2259.tag= Cyc_Rgnorder_tok; _temp2259.f1= 0; _temp2259;});
_temp2258;}); break; case 164: _LL2257: yyval= yyvs[ yyvsp_offset]; break; case
165: _LL2260: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2262=(
struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2262[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2263; _temp2263.tag= Cyc_Rgnorder_tok;
_temp2263.f1=({ struct Cyc_List_List* _temp2264=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2264->hd=( void*)({ struct
_tuple18* _temp2265=( struct _tuple18*) _cycalloc( sizeof( struct _tuple18));
_temp2265->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2268=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2268[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2269; _temp2269.tag= Cyc_Absyn_Eq_kb; _temp2269.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2269;}); _temp2268;})); _temp2265->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ yyvsp_offset]),( void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp2266=(
struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp2266[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp2267; _temp2267.tag= Cyc_Absyn_Eq_kb;
_temp2267.f1=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2267;}); _temp2266;}));
_temp2265;}); _temp2264->tl= 0; _temp2264;}); _temp2263;}); _temp2262;}); break;
case 166: _LL2261: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2271=(
struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2271[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2272; _temp2272.tag= Cyc_Rgnorder_tok;
_temp2272.f1=({ struct Cyc_List_List* _temp2273=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2273->hd=( void*)({ struct
_tuple18* _temp2274=( struct _tuple18*) _cycalloc( sizeof( struct _tuple18));
_temp2274->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2277=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2277[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2278; _temp2278.tag= Cyc_Absyn_Eq_kb; _temp2278.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2278;}); _temp2277;})); _temp2274->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2275=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2275[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2276; _temp2276.tag= Cyc_Absyn_Eq_kb; _temp2276.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2276;}); _temp2275;})); _temp2274;}); _temp2273->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ yyvsp_offset]); _temp2273;}); _temp2272;}); _temp2271;}); break; case 167:
_LL2270: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2280=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp2280[ 0]=({ struct
Cyc_Bool_tok_struct _temp2281; _temp2281.tag= Cyc_Bool_tok; _temp2281.f1= 0;
_temp2281;}); _temp2280;}); break; case 168: _LL2279: { struct _tagged_arr
_temp2283= Cyc_yyget_String_tok( yyvs[ yyvsp_offset]); if( Cyc_Std_zstrcmp(
_temp2283, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2284=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2284[ 0]=({ struct Cyc_Bool_tok_struct _temp2285; _temp2285.tag= Cyc_Bool_tok;
_temp2285.f1= 1; _temp2285;}); _temp2284;}); break;} case 169: _LL2282: yyval=
yyvs[ yyvsp_offset]; break; case 170: _LL2286: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2288=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2288[ 0]=({ struct Cyc_TypeList_tok_struct _temp2289; _temp2289.tag= Cyc_TypeList_tok;
_temp2289.f1= Cyc_List_imp_append( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset])); _temp2289;}); _temp2288;}); break; case 171: _LL2287:
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2291=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2291[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2292; _temp2292.tag= Cyc_TypeList_tok; _temp2292.f1= 0; _temp2292;});
_temp2291;}); break; case 172: _LL2290: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 173:
_LL2293: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2295=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2295[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2296; _temp2296.tag= Cyc_TypeList_tok; _temp2296.f1=({ struct Cyc_List_List*
_temp2297=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2297->hd=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2298=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2298[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2299; _temp2299.tag= Cyc_Absyn_RgnsEff;
_temp2299.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2299;}); _temp2298;})); _temp2297->tl= 0;
_temp2297;}); _temp2296;}); _temp2295;}); break; case 174: _LL2294: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]),( void*) Cyc_Absyn_EffKind); yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2301=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2301[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2302; _temp2302.tag= Cyc_TypeList_tok; _temp2302.f1=({ struct Cyc_List_List*
_temp2303=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2303->hd=( void*) Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]); _temp2303->tl=
0; _temp2303;}); _temp2302;}); _temp2301;}); break; case 175: _LL2300: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]),( void*) Cyc_Absyn_RgnKind); yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2305=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2305[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2306; _temp2306.tag= Cyc_TypeList_tok; _temp2306.f1=({ struct Cyc_List_List*
_temp2307=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2307->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2308=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2308[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2309; _temp2309.tag= Cyc_Absyn_AccessEff;
_temp2309.f1=( void*) Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]); _temp2309;});
_temp2308;})); _temp2307->tl= 0; _temp2307;}); _temp2306;}); _temp2305;});
break; case 176: _LL2304: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*) Cyc_Absyn_RgnKind);
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2311=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2311[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2312; _temp2312.tag= Cyc_TypeList_tok; _temp2312.f1=({ struct Cyc_List_List*
_temp2313=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2313->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2314=(
struct Cyc_Absyn_AccessEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2314[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2315; _temp2315.tag= Cyc_Absyn_AccessEff;
_temp2315.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2315;}); _temp2314;})); _temp2313->tl= Cyc_yyget_TypeList_tok(
yyvs[ yyvsp_offset]); _temp2313;}); _temp2312;}); _temp2311;}); break; case 177:
_LL2310: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp2317=(
struct Cyc_ParamDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp2317[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp2318; _temp2318.tag=
Cyc_ParamDeclList_tok; _temp2318.f1=({ struct Cyc_List_List* _temp2319=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2319->hd=( void*)
Cyc_yyget_ParamDecl_tok( yyvs[ yyvsp_offset]); _temp2319->tl= 0; _temp2319;});
_temp2318;}); _temp2317;}); break; case 178: _LL2316: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2321=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2321[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2322; _temp2322.tag= Cyc_ParamDeclList_tok;
_temp2322.f1=({ struct Cyc_List_List* _temp2323=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2323->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ yyvsp_offset]); _temp2323->tl= Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2323;});
_temp2322;}); _temp2321;}); break; case 179: _LL2320: { struct _tuple17
_temp2327; struct Cyc_List_List* _temp2328; struct Cyc_List_List* _temp2330;
struct Cyc_Absyn_Tqual _temp2332; struct _tuple17* _temp2325= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2327=*
_temp2325; _LL2333: _temp2332= _temp2327.f1; goto _LL2331; _LL2331: _temp2330=
_temp2327.f2; goto _LL2329; _LL2329: _temp2328= _temp2327.f3; goto _LL2326;
_LL2326: { struct Cyc_Parse_Declarator _temp2336; struct Cyc_List_List*
_temp2337; struct _tuple1* _temp2339; struct Cyc_Parse_Declarator* _temp2334=
Cyc_yyget_Declarator_tok( yyvs[ yyvsp_offset]); _temp2336=* _temp2334; _LL2340:
_temp2339= _temp2336.id; goto _LL2338; _LL2338: _temp2337= _temp2336.tms; goto
_LL2335; _LL2335: { void* _temp2341= Cyc_Parse_speclist2typ( _temp2330, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2344; struct Cyc_List_List* _temp2346; void*
_temp2348; struct Cyc_Absyn_Tqual _temp2350; struct _tuple6 _temp2342= Cyc_Parse_apply_tms(
_temp2332, _temp2341, _temp2328, _temp2337); _LL2351: _temp2350= _temp2342.f1;
goto _LL2349; _LL2349: _temp2348= _temp2342.f2; goto _LL2347; _LL2347: _temp2346=
_temp2342.f3; goto _LL2345; _LL2345: _temp2344= _temp2342.f4; goto _LL2343;
_LL2343: if( _temp2346 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line));} if( Cyc_Absyn_is_qvar_qualified( _temp2339)){
Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2352=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2357=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2357->v=( void*)(* _temp2339).f2; _temp2357;}); if( _temp2344 !=  0){({
void* _temp2353[ 0u]={}; Cyc_Tcutil_warn( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line), _tag_arr("extra attributes on parameter, ignoring",
sizeof( unsigned char), 40u), _tag_arr( _temp2353, sizeof( void*), 0u));});}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp2354=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2354[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2355; _temp2355.tag= Cyc_ParamDecl_tok; _temp2355.f1=({
struct _tuple2* _temp2356=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2356->f1= _temp2352; _temp2356->f2= _temp2350; _temp2356->f3= _temp2348;
_temp2356;}); _temp2355;}); _temp2354;}); break;}}}} case 180: _LL2324: { struct
_tuple17 _temp2361; struct Cyc_List_List* _temp2362; struct Cyc_List_List*
_temp2364; struct Cyc_Absyn_Tqual _temp2366; struct _tuple17* _temp2359= Cyc_yyget_QualSpecList_tok(
yyvs[ yyvsp_offset]); _temp2361=* _temp2359; _LL2367: _temp2366= _temp2361.f1;
goto _LL2365; _LL2365: _temp2364= _temp2361.f2; goto _LL2363; _LL2363: _temp2362=
_temp2361.f3; goto _LL2360; _LL2360: { void* _temp2368= Cyc_Parse_speclist2typ(
_temp2364, Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); if( _temp2362 !=  0){({ void* _temp2369[ 0u]={}; Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line), _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), _tag_arr( _temp2369, sizeof( void*), 0u));});}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp2370=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2370[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2371; _temp2371.tag= Cyc_ParamDecl_tok; _temp2371.f1=({
struct _tuple2* _temp2372=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2372->f1= 0; _temp2372->f2= _temp2366; _temp2372->f3= _temp2368; _temp2372;});
_temp2371;}); _temp2370;}); break;}} case 181: _LL2358: { struct _tuple17
_temp2376; struct Cyc_List_List* _temp2377; struct Cyc_List_List* _temp2379;
struct Cyc_Absyn_Tqual _temp2381; struct _tuple17* _temp2374= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2376=*
_temp2374; _LL2382: _temp2381= _temp2376.f1; goto _LL2380; _LL2380: _temp2379=
_temp2376.f2; goto _LL2378; _LL2378: _temp2377= _temp2376.f3; goto _LL2375;
_LL2375: { void* _temp2383= Cyc_Parse_speclist2typ( _temp2379, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2384=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
yyvsp_offset]))->tms; struct Cyc_List_List* _temp2387; struct Cyc_List_List*
_temp2389; void* _temp2391; struct Cyc_Absyn_Tqual _temp2393; struct _tuple6
_temp2385= Cyc_Parse_apply_tms( _temp2381, _temp2383, _temp2377, _temp2384);
_LL2394: _temp2393= _temp2385.f1; goto _LL2392; _LL2392: _temp2391= _temp2385.f2;
goto _LL2390; _LL2390: _temp2389= _temp2385.f3; goto _LL2388; _LL2388: _temp2387=
_temp2385.f4; goto _LL2386; _LL2386: if( _temp2389 !=  0){({ void* _temp2395[ 0u]={};
Cyc_Tcutil_warn( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line), _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), _tag_arr( _temp2395, sizeof( void*), 0u));});} if(
_temp2387 !=  0){({ void* _temp2396[ 0u]={}; Cyc_Tcutil_warn( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line), _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), _tag_arr( _temp2396, sizeof( void*), 0u));});}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp2397=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2397[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2398; _temp2398.tag= Cyc_ParamDecl_tok; _temp2398.f1=({
struct _tuple2* _temp2399=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2399->f1= 0; _temp2399->f2= _temp2393; _temp2399->f3= _temp2391; _temp2399;});
_temp2398;}); _temp2397;}); break;}} case 182: _LL2373: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2401=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2401[ 0]=({ struct Cyc_IdList_tok_struct
_temp2402; _temp2402.tag= Cyc_IdList_tok; _temp2402.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
yyvsp_offset])); _temp2402;}); _temp2401;}); break; case 183: _LL2400: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2404=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2404[ 0]=({ struct Cyc_IdList_tok_struct
_temp2405; _temp2405.tag= Cyc_IdList_tok; _temp2405.f1=({ struct Cyc_List_List*
_temp2406=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2406->hd=( void*)({ struct _tagged_arr* _temp2407=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2407[ 0]= Cyc_yyget_String_tok(
yyvs[ yyvsp_offset]); _temp2407;}); _temp2406->tl= 0; _temp2406;}); _temp2405;});
_temp2404;}); break; case 184: _LL2403: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2409=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2409[ 0]=({ struct Cyc_IdList_tok_struct _temp2410; _temp2410.tag= Cyc_IdList_tok;
_temp2410.f1=({ struct Cyc_List_List* _temp2411=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2411->hd=( void*)({ struct
_tagged_arr* _temp2412=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2412[ 0]= Cyc_yyget_String_tok( yyvs[ yyvsp_offset]);
_temp2412;}); _temp2411->tl= Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2411;});
_temp2410;}); _temp2409;}); break; case 185: _LL2408: yyval= yyvs[ yyvsp_offset];
break; case 186: _LL2413: yyval= yyvs[ yyvsp_offset]; break; case 187: _LL2414:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2416=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2416[ 0]=({ struct Cyc_Exp_tok_struct
_temp2417; _temp2417.tag= Cyc_Exp_tok; _temp2417.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2418=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2418[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2419; _temp2419.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2419.f1= 0; _temp2419.f2= 0; _temp2419;}); _temp2418;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2417;}); _temp2416;}); break; case 188:
_LL2415: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2421=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2421[ 0]=({ struct Cyc_Exp_tok_struct
_temp2422; _temp2422.tag= Cyc_Exp_tok; _temp2422.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2423=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2423[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2424; _temp2424.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2424.f1= 0; _temp2424.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2424;});
_temp2423;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2422;}); _temp2421;}); break; case 189: _LL2420: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2426=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2426[ 0]=({ struct Cyc_Exp_tok_struct
_temp2427; _temp2427.tag= Cyc_Exp_tok; _temp2427.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2428=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2428[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2429; _temp2429.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2429.f1= 0; _temp2429.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2429;});
_temp2428;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2427;}); _temp2426;}); break; case 190: _LL2425: { struct Cyc_Absyn_Vardecl*
_temp2431= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp2437=( struct _tuple1*)
_cycalloc( sizeof( struct _tuple1)); _temp2437->f1=( void*) Cyc_Absyn_Loc_n;
_temp2437->f2=({ struct _tagged_arr* _temp2438=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2438[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2438;});
_temp2437;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line))); _temp2431->tq=({ struct Cyc_Absyn_Tqual
_temp2432; _temp2432.q_const= 1; _temp2432.q_volatile= 0; _temp2432.q_restrict=
1; _temp2432;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2433=( struct
Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2433[
0]=({ struct Cyc_Exp_tok_struct _temp2434; _temp2434.tag= Cyc_Exp_tok; _temp2434.f1=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Comprehension_e_struct* _temp2435=(
struct Cyc_Absyn_Comprehension_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct));
_temp2435[ 0]=({ struct Cyc_Absyn_Comprehension_e_struct _temp2436; _temp2436.tag=
Cyc_Absyn_Comprehension_e; _temp2436.f1= _temp2431; _temp2436.f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2436.f3=
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2436;}); _temp2435;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2434;}); _temp2433;}); break;} case 191:
_LL2430: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2440=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2440[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2441; _temp2441.tag=
Cyc_InitializerList_tok; _temp2441.f1=({ struct Cyc_List_List* _temp2442=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2442->hd=(
void*)({ struct _tuple19* _temp2443=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2443->f1= 0; _temp2443->f2= Cyc_yyget_Exp_tok( yyvs[
yyvsp_offset]); _temp2443;}); _temp2442->tl= 0; _temp2442;}); _temp2441;});
_temp2440;}); break; case 192: _LL2439: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp2445=( struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp2445[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp2446; _temp2446.tag= Cyc_InitializerList_tok; _temp2446.f1=({ struct Cyc_List_List*
_temp2447=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2447->hd=( void*)({ struct _tuple19* _temp2448=( struct _tuple19*)
_cycalloc( sizeof( struct _tuple19)); _temp2448->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2448->f2=
Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp2448;}); _temp2447->tl= 0;
_temp2447;}); _temp2446;}); _temp2445;}); break; case 193: _LL2444: yyval=( void*)({
struct Cyc_InitializerList_tok_struct* _temp2450=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2450[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2451; _temp2451.tag= Cyc_InitializerList_tok;
_temp2451.f1=({ struct Cyc_List_List* _temp2452=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2452->hd=( void*)({ struct
_tuple19* _temp2453=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2453->f1= 0; _temp2453->f2= Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]);
_temp2453;}); _temp2452->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2452;});
_temp2451;}); _temp2450;}); break; case 194: _LL2449: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp2455=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2455[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2456; _temp2456.tag= Cyc_InitializerList_tok;
_temp2456.f1=({ struct Cyc_List_List* _temp2457=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2457->hd=( void*)({ struct
_tuple19* _temp2458=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2458->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2458->f2= Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]); _temp2458;}); _temp2457->tl= Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2457;});
_temp2456;}); _temp2455;}); break; case 195: _LL2454: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2460=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2460[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2461; _temp2461.tag= Cyc_DesignatorList_tok;
_temp2461.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2461;});
_temp2460;}); break; case 196: _LL2459: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2463=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2463[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2464; _temp2464.tag=
Cyc_DesignatorList_tok; _temp2464.f1=({ struct Cyc_List_List* _temp2465=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2465->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ yyvsp_offset]); _temp2465->tl= 0; _temp2465;});
_temp2464;}); _temp2463;}); break; case 197: _LL2462: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2467=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2467[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2468; _temp2468.tag= Cyc_DesignatorList_tok;
_temp2468.f1=({ struct Cyc_List_List* _temp2469=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2469->hd=( void*) Cyc_yyget_Designator_tok(
yyvs[ yyvsp_offset]); _temp2469->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2469;});
_temp2468;}); _temp2467;}); break; case 198: _LL2466: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2471=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2471[ 0]=({ struct
Cyc_Designator_tok_struct _temp2472; _temp2472.tag= Cyc_Designator_tok;
_temp2472.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2473=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2473[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2474; _temp2474.tag=
Cyc_Absyn_ArrayElement; _temp2474.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2474;});
_temp2473;})); _temp2472;}); _temp2471;}); break; case 199: _LL2470: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2476=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2476[ 0]=({ struct
Cyc_Designator_tok_struct _temp2477; _temp2477.tag= Cyc_Designator_tok;
_temp2477.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2478=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2478[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2479; _temp2479.tag= Cyc_Absyn_FieldName;
_temp2479.f1=({ struct _tagged_arr* _temp2480=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2480[ 0]= Cyc_yyget_String_tok( yyvs[
yyvsp_offset]); _temp2480;}); _temp2479;}); _temp2478;})); _temp2477;});
_temp2476;}); break; case 200: _LL2475: { void* _temp2482= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ yyvsp_offset])).f2, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); struct Cyc_List_List*
_temp2483=(* Cyc_yyget_QualSpecList_tok( yyvs[ yyvsp_offset])).f3; if( _temp2483
!=  0){({ void* _temp2484[ 0u]={}; Cyc_Tcutil_warn( Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line), _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), _tag_arr( _temp2484, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Tqual _temp2485=(* Cyc_yyget_QualSpecList_tok( yyvs[
yyvsp_offset])).f1; yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp2486=(
struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2486[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2487; _temp2487.tag= Cyc_ParamDecl_tok;
_temp2487.f1=({ struct _tuple2* _temp2488=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2488->f1= 0; _temp2488->f2= _temp2485; _temp2488->f3=
_temp2482; _temp2488;}); _temp2487;}); _temp2486;}); break;}} case 201: _LL2481: {
void* _temp2490= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2491=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2492=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2493=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ yyvsp_offset]))->tms; struct _tuple6 _temp2494= Cyc_Parse_apply_tms(
_temp2492, _temp2490, _temp2491, _temp2493); if( _temp2494.f3 !=  0){({ void*
_temp2495[ 0u]={}; Cyc_Tcutil_warn( Cyc_Position_segment_of_abs(( yyls[
yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line), _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), _tag_arr( _temp2495, sizeof( void*), 0u));});} if(
_temp2494.f4 !=  0){({ void* _temp2496[ 0u]={}; Cyc_Tcutil_warn( Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line), _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), _tag_arr( _temp2496, sizeof( void*), 0u));});}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp2497=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2497[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2498; _temp2498.tag= Cyc_ParamDecl_tok; _temp2498.f1=({
struct _tuple2* _temp2499=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2499->f1= 0; _temp2499->f2= _temp2494.f1; _temp2499->f3= _temp2494.f2;
_temp2499;}); _temp2498;}); _temp2497;}); break;} case 202: _LL2489: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2501=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2501[ 0]=({ struct Cyc_Type_tok_struct
_temp2502; _temp2502.tag= Cyc_Type_tok; _temp2502.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ yyvsp_offset])).f3; _temp2502;}); _temp2501;}); break; case 203: _LL2500:
yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2504=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2504[ 0]=({ struct Cyc_Type_tok_struct
_temp2505; _temp2505.tag= Cyc_Type_tok; _temp2505.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp2506=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2506[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2507; _temp2507.tag= Cyc_Absyn_JoinEff; _temp2507.f1=
0; _temp2507;}); _temp2506;})); _temp2505;}); _temp2504;}); break; case 204:
_LL2503: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2509=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2509[ 0]=({ struct Cyc_Type_tok_struct
_temp2510; _temp2510.tag= Cyc_Type_tok; _temp2510.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp2511=( struct Cyc_Absyn_JoinEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2511[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2512; _temp2512.tag= Cyc_Absyn_JoinEff; _temp2512.f1=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2512;}); _temp2511;})); _temp2510;}); _temp2509;});
break; case 205: _LL2508: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2514=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2514[ 0]=({ struct Cyc_Type_tok_struct _temp2515; _temp2515.tag= Cyc_Type_tok;
_temp2515.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2516=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2516[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2517; _temp2517.tag= Cyc_Absyn_RgnsEff;
_temp2517.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2517;}); _temp2516;})); _temp2515;});
_temp2514;}); break; case 206: _LL2513: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2519=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2519[ 0]=({ struct Cyc_Type_tok_struct _temp2520; _temp2520.tag= Cyc_Type_tok;
_temp2520.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2521=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2521[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2522; _temp2522.tag= Cyc_Absyn_JoinEff;
_temp2522.f1=({ struct Cyc_List_List* _temp2523=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2523->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2523->tl=
Cyc_yyget_TypeList_tok( yyvs[ yyvsp_offset]); _temp2523;}); _temp2522;});
_temp2521;})); _temp2520;}); _temp2519;}); break; case 207: _LL2518: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2525=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2525[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2526; _temp2526.tag= Cyc_TypeList_tok; _temp2526.f1=({ struct Cyc_List_List*
_temp2527=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2527->hd=( void*) Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]); _temp2527->tl=
0; _temp2527;}); _temp2526;}); _temp2525;}); break; case 208: _LL2524: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2529=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2529[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2530; _temp2530.tag= Cyc_TypeList_tok; _temp2530.f1=({ struct Cyc_List_List*
_temp2531=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2531->hd=( void*) Cyc_yyget_Type_tok( yyvs[ yyvsp_offset]); _temp2531->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2531;}); _temp2530;}); _temp2529;}); break; case 209:
_LL2528: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2533=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2533[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2534; _temp2534.tag=
Cyc_AbstractDeclarator_tok; _temp2534.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2535=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2535->tms= Cyc_yyget_TypeModifierList_tok( yyvs[ yyvsp_offset]); _temp2535;});
_temp2534;}); _temp2533;}); break; case 210: _LL2532: yyval= yyvs[ yyvsp_offset];
break; case 211: _LL2536: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2538=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2538[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2539; _temp2539.tag= Cyc_AbstractDeclarator_tok; _temp2539.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2540=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2540->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ yyvsp_offset]))->tms); _temp2540;}); _temp2539;}); _temp2538;}); break;
case 212: _LL2537: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]; break; case 213: _LL2541: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2543=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2543[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2544; _temp2544.tag= Cyc_AbstractDeclarator_tok; _temp2544.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2545=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2545->tms=({ struct Cyc_List_List* _temp2546=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2546->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2546->tl= 0; _temp2546;}); _temp2545;}); _temp2544;}); _temp2543;}); break;
case 214: _LL2542: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2548=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2548[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2549; _temp2549.tag= Cyc_AbstractDeclarator_tok; _temp2549.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2550=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2550->tms=({ struct Cyc_List_List* _temp2551=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2551->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2551->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2551;});
_temp2550;}); _temp2549;}); _temp2548;}); break; case 215: _LL2547: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2553=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2553[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2554; _temp2554.tag= Cyc_AbstractDeclarator_tok;
_temp2554.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2555=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2555->tms=({
struct Cyc_List_List* _temp2556=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2556->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2557=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2557[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2558; _temp2558.tag= Cyc_Absyn_ConstArray_mod; _temp2558.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2558;});
_temp2557;})); _temp2556->tl= 0; _temp2556;}); _temp2555;}); _temp2554;});
_temp2553;}); break; case 216: _LL2552: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2560=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2560[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2561; _temp2561.tag= Cyc_AbstractDeclarator_tok; _temp2561.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2562=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2562->tms=({ struct Cyc_List_List* _temp2563=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2563->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2564=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2564[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2565; _temp2565.tag= Cyc_Absyn_ConstArray_mod;
_temp2565.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2565;}); _temp2564;})); _temp2563->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2563;}); _temp2562;}); _temp2561;}); _temp2560;}); break; case 217: _LL2559:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2567=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2567[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2568; _temp2568.tag=
Cyc_AbstractDeclarator_tok; _temp2568.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2569=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2569->tms=({ struct Cyc_List_List* _temp2570=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2570->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2571=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2571[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2572; _temp2572.tag= Cyc_Absyn_Function_mod;
_temp2572.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2573=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2573[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2574; _temp2574.tag= Cyc_Absyn_WithTypes;
_temp2574.f1= 0; _temp2574.f2= 0; _temp2574.f3= 0; _temp2574.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2574.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2574;}); _temp2573;})); _temp2572;}); _temp2571;}));
_temp2570->tl= 0; _temp2570;}); _temp2569;}); _temp2568;}); _temp2567;}); break;
case 218: _LL2566: { struct _tuple16 _temp2578; struct Cyc_List_List* _temp2579;
struct Cyc_Core_Opt* _temp2581; struct Cyc_Absyn_VarargInfo* _temp2583; int
_temp2585; struct Cyc_List_List* _temp2587; struct _tuple16* _temp2576= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2578=*
_temp2576; _LL2588: _temp2587= _temp2578.f1; goto _LL2586; _LL2586: _temp2585=
_temp2578.f2; goto _LL2584; _LL2584: _temp2583= _temp2578.f3; goto _LL2582;
_LL2582: _temp2581= _temp2578.f4; goto _LL2580; _LL2580: _temp2579= _temp2578.f5;
goto _LL2577; _LL2577: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2589=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2589[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2590; _temp2590.tag= Cyc_AbstractDeclarator_tok; _temp2590.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2591=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2591->tms=({ struct Cyc_List_List* _temp2592=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2592->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2593=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2593[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2594; _temp2594.tag= Cyc_Absyn_Function_mod;
_temp2594.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2595=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2595[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2596; _temp2596.tag= Cyc_Absyn_WithTypes;
_temp2596.f1= _temp2587; _temp2596.f2= _temp2585; _temp2596.f3= _temp2583;
_temp2596.f4= _temp2581; _temp2596.f5= _temp2579; _temp2596;}); _temp2595;}));
_temp2594;}); _temp2593;})); _temp2592->tl= 0; _temp2592;}); _temp2591;});
_temp2590;}); _temp2589;}); break;} case 219: _LL2575: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2598=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2598[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2599; _temp2599.tag= Cyc_AbstractDeclarator_tok;
_temp2599.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2600=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2600->tms=({
struct Cyc_List_List* _temp2601=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2601->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2602=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2602[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2603; _temp2603.tag=
Cyc_Absyn_Function_mod; _temp2603.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2604=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2604[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2605; _temp2605.tag= Cyc_Absyn_WithTypes;
_temp2605.f1= 0; _temp2605.f2= 0; _temp2605.f3= 0; _temp2605.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2605.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2605;}); _temp2604;})); _temp2603;}); _temp2602;}));
_temp2601->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2601;});
_temp2600;}); _temp2599;}); _temp2598;}); break; case 220: _LL2597: { struct
_tuple16 _temp2609; struct Cyc_List_List* _temp2610; struct Cyc_Core_Opt*
_temp2612; struct Cyc_Absyn_VarargInfo* _temp2614; int _temp2616; struct Cyc_List_List*
_temp2618; struct _tuple16* _temp2607= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2609=*
_temp2607; _LL2619: _temp2618= _temp2609.f1; goto _LL2617; _LL2617: _temp2616=
_temp2609.f2; goto _LL2615; _LL2615: _temp2614= _temp2609.f3; goto _LL2613;
_LL2613: _temp2612= _temp2609.f4; goto _LL2611; _LL2611: _temp2610= _temp2609.f5;
goto _LL2608; _LL2608: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2620=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2620[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2621; _temp2621.tag= Cyc_AbstractDeclarator_tok; _temp2621.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2622=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2622->tms=({ struct Cyc_List_List* _temp2623=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2623->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2624=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2624[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2625; _temp2625.tag= Cyc_Absyn_Function_mod;
_temp2625.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2626=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2626[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2627; _temp2627.tag= Cyc_Absyn_WithTypes;
_temp2627.f1= _temp2618; _temp2627.f2= _temp2616; _temp2627.f3= _temp2614;
_temp2627.f4= _temp2612; _temp2627.f5= _temp2610; _temp2627;}); _temp2626;}));
_temp2625;}); _temp2624;})); _temp2623->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2623;}); _temp2622;}); _temp2621;}); _temp2620;}); break;} case 221:
_LL2606: { struct Cyc_List_List* _temp2629=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2630=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2630[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2631; _temp2631.tag= Cyc_AbstractDeclarator_tok;
_temp2631.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2632=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2632->tms=({
struct Cyc_List_List* _temp2633=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2633->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2634=( struct Cyc_Absyn_TypeParams_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2634[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2635; _temp2635.tag= Cyc_Absyn_TypeParams_mod; _temp2635.f1= _temp2629;
_temp2635.f2= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line);
_temp2635.f3= 0; _temp2635;}); _temp2634;})); _temp2633->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2633;}); _temp2632;}); _temp2631;}); _temp2630;}); break;} case 222:
_LL2628:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2637=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2638=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2638[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2639; _temp2639.tag= Cyc_AbstractDeclarator_tok;
_temp2639.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2640=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2640->tms=({
struct Cyc_List_List* _temp2641=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2641->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2642=( struct Cyc_Absyn_TypeParams_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2642[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2643; _temp2643.tag= Cyc_Absyn_TypeParams_mod; _temp2643.f1= _temp2637;
_temp2643.f2= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line);
_temp2643.f3= 0; _temp2643;}); _temp2642;})); _temp2641->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2641;}); _temp2640;}); _temp2639;}); _temp2638;}); break;} case 223:
_LL2636: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2645=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2645[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2646; _temp2646.tag=
Cyc_AbstractDeclarator_tok; _temp2646.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2647=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2647->tms=({ struct Cyc_List_List* _temp2648=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2648->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2649=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2649[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2650; _temp2650.tag= Cyc_Absyn_Attributes_mod;
_temp2650.f1= Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line); _temp2650.f2= Cyc_yyget_AttributeList_tok( yyvs[
yyvsp_offset]); _temp2650;}); _temp2649;})); _temp2648->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms;
_temp2648;}); _temp2647;}); _temp2646;}); _temp2645;}); break; case 224: _LL2644:
yyval= yyvs[ yyvsp_offset]; break; case 225: _LL2651: yyval= yyvs[ yyvsp_offset];
break; case 226: _LL2652: yyval= yyvs[ yyvsp_offset]; break; case 227: _LL2653:
yyval= yyvs[ yyvsp_offset]; break; case 228: _LL2654: yyval= yyvs[ yyvsp_offset];
break; case 229: _LL2655: yyval= yyvs[ yyvsp_offset]; break; case 230: _LL2656:
if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]), _tag_arr("`H", sizeof( unsigned char), 3u)) ==  0){
Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H", sizeof(
unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2668=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2668->name=({ struct
_tagged_arr* _temp2671=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2671[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2671;});
_temp2668->identity= 0; _temp2668->kind=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp2669=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp2669[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp2670; _temp2670.tag= Cyc_Absyn_Eq_kb;
_temp2670.f1=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2670;}); _temp2669;}));
_temp2668;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2666=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2666[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2667; _temp2667.tag= Cyc_Absyn_VarType;
_temp2667.f1= tv; _temp2667;}); _temp2666;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2658=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2658[ 0]=({ struct Cyc_Stmt_tok_struct _temp2659; _temp2659.tag= Cyc_Stmt_tok;
_temp2659.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2660=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2660[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2661; _temp2661.tag= Cyc_Absyn_Region_s;
_temp2661.f1= tv; _temp2661.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2662=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2662->f1=(
void*) Cyc_Absyn_Loc_n; _temp2662->f2=({ struct _tagged_arr* _temp2663=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2663[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2663;});
_temp2662;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2664=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2664[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2665; _temp2665.tag=
Cyc_Absyn_RgnHandleType; _temp2665.f1=( void*) t; _temp2665;}); _temp2664;}), 0);
_temp2661.f3= Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]); _temp2661;}); _temp2660;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2659;});
_temp2658;}); break;} case 231: _LL2657: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2683=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2683->name=({ struct
_tagged_arr* _temp2686=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2686[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2688; _temp2688.tag= Cyc_Std_String_pa; _temp2688.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2687[ 1u]={& _temp2688}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2687, sizeof( void*), 1u));}});
_temp2686;}); _temp2683->identity= 0; _temp2683->kind=( void*)(( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2684=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2684[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2685; _temp2685.tag= Cyc_Absyn_Eq_kb; _temp2685.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2685;}); _temp2684;})); _temp2683;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2681=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2681[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2682; _temp2682.tag= Cyc_Absyn_VarType;
_temp2682.f1= tv; _temp2682;}); _temp2681;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2673=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2673[ 0]=({ struct Cyc_Stmt_tok_struct _temp2674; _temp2674.tag= Cyc_Stmt_tok;
_temp2674.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2675=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2675[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2676; _temp2676.tag= Cyc_Absyn_Region_s;
_temp2676.f1= tv; _temp2676.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2677=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2677->f1=(
void*) Cyc_Absyn_Loc_n; _temp2677->f2=({ struct _tagged_arr* _temp2678=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2678[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2678;});
_temp2677;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2679=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2679[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2680; _temp2680.tag=
Cyc_Absyn_RgnHandleType; _temp2680.f1=( void*) t; _temp2680;}); _temp2679;}), 0);
_temp2676.f3= Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]); _temp2676;}); _temp2675;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2674;});
_temp2673;}); break;} case 232: _LL2672: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2690=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2690[ 0]=({ struct Cyc_Stmt_tok_struct _temp2691; _temp2691.tag= Cyc_Stmt_tok;
_temp2691.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2692=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2692[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2693; _temp2693.tag= Cyc_Absyn_Cut_s;
_temp2693.f1= Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]); _temp2693;}); _temp2692;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2691;});
_temp2690;}); break; case 233: _LL2689: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2695=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2695[ 0]=({ struct Cyc_Stmt_tok_struct _temp2696; _temp2696.tag= Cyc_Stmt_tok;
_temp2696.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2697=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2697[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2698; _temp2698.tag= Cyc_Absyn_Splice_s;
_temp2698.f1= Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]); _temp2698;}); _temp2697;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2696;});
_temp2695;}); break; case 234: _LL2694: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2700=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2700[ 0]=({ struct Cyc_Stmt_tok_struct _temp2701; _temp2701.tag= Cyc_Stmt_tok;
_temp2701.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2702=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2702[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2703; _temp2703.tag= Cyc_Absyn_Label_s;
_temp2703.f1=({ struct _tagged_arr* _temp2704=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2704[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2704;});
_temp2703.f2= Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]); _temp2703;}); _temp2702;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2701;});
_temp2700;}); break; case 235: _LL2699: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2706=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2706[ 0]=({ struct Cyc_Stmt_tok_struct _temp2707; _temp2707.tag= Cyc_Stmt_tok;
_temp2707.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2707;});
_temp2706;}); break; case 236: _LL2705: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2709=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2709[ 0]=({ struct Cyc_Stmt_tok_struct _temp2710; _temp2710.tag= Cyc_Stmt_tok;
_temp2710.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2710;}); _temp2709;}); break; case 237:
_LL2708: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2712=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2712[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2713; _temp2713.tag= Cyc_Stmt_tok; _temp2713.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2713;}); _temp2712;}); break; case 238:
_LL2711: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
break; case 239: _LL2714: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2716=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2716[ 0]=({ struct Cyc_Stmt_tok_struct _temp2717; _temp2717.tag= Cyc_Stmt_tok;
_temp2717.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
yyvsp_offset]), Cyc_Absyn_skip_stmt( 0)); _temp2717;}); _temp2716;}); break;
case 240: _LL2715: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2719=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2719[ 0]=({ struct Cyc_Stmt_tok_struct _temp2720; _temp2720.tag= Cyc_Stmt_tok;
_temp2720.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset])); _temp2720;}); _temp2719;}); break; case 241: _LL2718:
yyval= yyvs[ yyvsp_offset]; break; case 242: _LL2721: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2723=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2723[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2724; _temp2724.tag= Cyc_Stmt_tok; _temp2724.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2724;}); _temp2723;}); break; case 243: _LL2722:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2726=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2726[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2727; _temp2727.tag= Cyc_Stmt_tok; _temp2727.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2728=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2728[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2729; _temp2729.tag= Cyc_Absyn_Fn_d;
_temp2729.f1= Cyc_yyget_FnDecl_tok( yyvs[ yyvsp_offset]); _temp2729;});
_temp2728;}), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)), Cyc_Absyn_skip_stmt( 0)); _temp2727;});
_temp2726;}); break; case 244: _LL2725: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2731=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2731[ 0]=({ struct Cyc_Stmt_tok_struct _temp2732; _temp2732.tag= Cyc_Stmt_tok;
_temp2732.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2733=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2733[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2734; _temp2734.tag= Cyc_Absyn_Fn_d;
_temp2734.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2734;}); _temp2733;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset])); _temp2732;}); _temp2731;}); break;
case 245: _LL2730: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2736=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2736[ 0]=({ struct Cyc_Stmt_tok_struct _temp2737; _temp2737.tag= Cyc_Stmt_tok;
_temp2737.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2737;}); _temp2736;}); break; case 246:
_LL2735: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2739=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2739[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2740; _temp2740.tag= Cyc_Stmt_tok; _temp2740.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2740;}); _temp2739;}); break; case 247:
_LL2738: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2742=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2742[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2743; _temp2743.tag= Cyc_Stmt_tok; _temp2743.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2743;}); _temp2742;}); break; case 248:
_LL2741: if( Cyc_Std_strcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
yylsp_offset]).last_line));} yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2745=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2745[ 0]=({ struct Cyc_Stmt_tok_struct _temp2746; _temp2746.tag= Cyc_Stmt_tok;
_temp2746.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_SwitchC_s_struct*
_temp2747=( struct Cyc_Absyn_SwitchC_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct));
_temp2747[ 0]=({ struct Cyc_Absyn_SwitchC_s_struct _temp2748; _temp2748.tag= Cyc_Absyn_SwitchC_s;
_temp2748.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2748.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2748;});
_temp2747;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2746;}); _temp2745;}); break; case 249: _LL2744: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2750=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2750[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2751; _temp2751.tag= Cyc_Stmt_tok; _temp2751.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2751;}); _temp2750;}); break; case 250: _LL2749:
yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2753=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2753[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2754; _temp2754.tag= Cyc_SwitchClauseList_tok;
_temp2754.f1= 0; _temp2754;}); _temp2753;}); break; case 251: _LL2752: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2756=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2756[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2757; _temp2757.tag= Cyc_SwitchClauseList_tok;
_temp2757.f1=({ struct Cyc_List_List* _temp2758=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2758->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2759=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2759->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2759->pat_vars= 0; _temp2759->where_clause= 0; _temp2759->body= Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]); _temp2759->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2759;}); _temp2758->tl= 0; _temp2758;});
_temp2757;}); _temp2756;}); break; case 252: _LL2755: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2761=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2761[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2762; _temp2762.tag= Cyc_SwitchClauseList_tok;
_temp2762.f1=({ struct Cyc_List_List* _temp2763=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2763->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2764=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2764->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2764->pat_vars= 0; _temp2764->where_clause= 0;
_temp2764->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2764->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2764;}); _temp2763->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ yyvsp_offset]); _temp2763;}); _temp2762;}); _temp2761;}); break; case 253:
_LL2760: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2766=(
struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp2766[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp2767; _temp2767.tag=
Cyc_SwitchClauseList_tok; _temp2767.f1=({ struct Cyc_List_List* _temp2768=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2768->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp2769=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2769->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2769->pat_vars=
0; _temp2769->where_clause= 0; _temp2769->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2769->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2769;}); _temp2768->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ yyvsp_offset]); _temp2768;}); _temp2767;}); _temp2766;}); break; case 254:
_LL2765: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2771=(
struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp2771[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp2772; _temp2772.tag=
Cyc_SwitchClauseList_tok; _temp2772.f1=({ struct Cyc_List_List* _temp2773=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2773->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp2774=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2774->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2774->pat_vars=
0; _temp2774->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2774->body=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2774->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2774;}); _temp2773->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ yyvsp_offset]); _temp2773;}); _temp2772;}); _temp2771;}); break; case 255:
_LL2770: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp2776=(
struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp2776[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp2777; _temp2777.tag=
Cyc_SwitchClauseList_tok; _temp2777.f1=({ struct Cyc_List_List* _temp2778=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2778->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp2779=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2779->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2779->pat_vars=
0; _temp2779->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2779->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2779->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2779;}); _temp2778->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ yyvsp_offset]); _temp2778;}); _temp2777;}); _temp2776;}); break; case 256:
_LL2775: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2781=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2781[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2782; _temp2782.tag=
Cyc_SwitchCClauseList_tok; _temp2782.f1= 0; _temp2782;}); _temp2781;}); break;
case 257: _LL2780: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2784=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2784[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2785; _temp2785.tag= Cyc_SwitchCClauseList_tok; _temp2785.f1=({ struct Cyc_List_List*
_temp2786=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2786->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2787=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2787->cnst_exp= 0;
_temp2787->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]),
Cyc_Absyn_break_stmt( 0), 0); _temp2787->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2787;}); _temp2786->tl= 0; _temp2786;});
_temp2785;}); _temp2784;}); break; case 258: _LL2783: yyval=( void*)({ struct
Cyc_SwitchCClauseList_tok_struct* _temp2789=( struct Cyc_SwitchCClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct)); _temp2789[ 0]=({
struct Cyc_SwitchCClauseList_tok_struct _temp2790; _temp2790.tag= Cyc_SwitchCClauseList_tok;
_temp2790.f1=({ struct Cyc_List_List* _temp2791=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2791->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause*
_temp2792=( struct Cyc_Absyn_SwitchC_clause*) _cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause));
_temp2792->cnst_exp=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2792->body=
Cyc_Absyn_fallthru_stmt( 0, 0); _temp2792->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line); _temp2792;}); _temp2791->tl= Cyc_yyget_SwitchCClauseList_tok(
yyvs[ yyvsp_offset]); _temp2791;}); _temp2790;}); _temp2789;}); break; case 259:
_LL2788: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2794=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2794[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2795; _temp2795.tag=
Cyc_SwitchCClauseList_tok; _temp2795.f1=({ struct Cyc_List_List* _temp2796=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2796->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2797=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2797->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2797->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2797->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
yylsp_offset]).last_line); _temp2797;}); _temp2796->tl= Cyc_yyget_SwitchCClauseList_tok(
yyvs[ yyvsp_offset]); _temp2796;}); _temp2795;}); _temp2794;}); break; case 260:
_LL2793: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2799=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2799[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2800; _temp2800.tag= Cyc_Stmt_tok; _temp2800.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2800;}); _temp2799;}); break; case 261: _LL2798:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2802=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2802[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2803; _temp2803.tag= Cyc_Stmt_tok; _temp2803.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2803;}); _temp2802;}); break; case 262:
_LL2801: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2805=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2805[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2806; _temp2806.tag= Cyc_Stmt_tok; _temp2806.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2806;}); _temp2805;}); break; case 263: _LL2804:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2808=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2808[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2809; _temp2809.tag= Cyc_Stmt_tok; _temp2809.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2809;}); _temp2808;}); break; case 264: _LL2807:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2811=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2811[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2812; _temp2812.tag= Cyc_Stmt_tok; _temp2812.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2812;}); _temp2811;}); break; case 265: _LL2810:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2814=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2814[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2815; _temp2815.tag= Cyc_Stmt_tok; _temp2815.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2815;}); _temp2814;}); break; case 266: _LL2813: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2817=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2817[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2818; _temp2818.tag= Cyc_Stmt_tok; _temp2818.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2818;}); _temp2817;}); break; case 267:
_LL2816: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2820=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2820[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2821; _temp2821.tag= Cyc_Stmt_tok; _temp2821.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2821;}); _temp2820;}); break; case 268:
_LL2819: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2823=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2823[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2824; _temp2824.tag= Cyc_Stmt_tok; _temp2824.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2824;}); _temp2823;}); break; case 269: _LL2822:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2826=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2826[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2827; _temp2827.tag= Cyc_Stmt_tok; _temp2827.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2827;}); _temp2826;}); break; case 270: _LL2825: {
struct Cyc_List_List* _temp2829= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt*
_temp2830= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ yylsp_offset]).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2831=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2831[ 0]=({ struct Cyc_Stmt_tok_struct _temp2832; _temp2832.tag= Cyc_Stmt_tok;
_temp2832.f1= Cyc_Parse_flatten_declarations( _temp2829, _temp2830); _temp2832;});
_temp2831;}); break;} case 271: _LL2828: { struct Cyc_List_List* _temp2834= Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2835= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
yylsp_offset]).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2836=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2836[ 0]=({ struct Cyc_Stmt_tok_struct _temp2837; _temp2837.tag= Cyc_Stmt_tok;
_temp2837.f1= Cyc_Parse_flatten_declarations( _temp2834, _temp2835); _temp2837;});
_temp2836;}); break;} case 272: _LL2833: { struct Cyc_List_List* _temp2839= Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2840= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ yylsp_offset]).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2841=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2841[ 0]=({ struct Cyc_Stmt_tok_struct _temp2842; _temp2842.tag= Cyc_Stmt_tok;
_temp2842.f1= Cyc_Parse_flatten_declarations( _temp2839, _temp2840); _temp2842;});
_temp2841;}); break;} case 273: _LL2838: { struct Cyc_List_List* _temp2844= Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2845= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
yylsp_offset]).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2846=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2846[ 0]=({ struct Cyc_Stmt_tok_struct _temp2847; _temp2847.tag= Cyc_Stmt_tok;
_temp2847.f1= Cyc_Parse_flatten_declarations( _temp2844, _temp2845); _temp2847;});
_temp2846;}); break;} case 274: _LL2843: { struct Cyc_List_List* _temp2849=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Vardecl*(* f)( struct Cyc_Absyn_Decl*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_decl2vardecl, Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2850=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2850[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2851; _temp2851.tag= Cyc_Stmt_tok; _temp2851.f1= Cyc_Absyn_forarray_stmt(
_temp2849, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2851;}); _temp2850;}); break;} case 275:
_LL2848: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2853=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2853[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2854; _temp2854.tag= Cyc_Stmt_tok; _temp2854.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2855=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2855[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2855;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2854;}); _temp2853;}); break; case 276: _LL2852: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2857=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2857[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2858; _temp2858.tag= Cyc_Stmt_tok; _temp2858.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2858;}); _temp2857;}); break; case 277:
_LL2856: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2860=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2860[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2861; _temp2861.tag= Cyc_Stmt_tok; _temp2861.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2861;}); _temp2860;}); break; case 278: _LL2859: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2863=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2863[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2864; _temp2864.tag= Cyc_Stmt_tok; _temp2864.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2864;}); _temp2863;}); break; case 279:
_LL2862: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2866=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2866[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2867; _temp2867.tag= Cyc_Stmt_tok; _temp2867.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2867;}); _temp2866;}); break; case 280: _LL2865: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2869=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2869[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2870; _temp2870.tag= Cyc_Stmt_tok; _temp2870.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2870;}); _temp2869;}); break; case 281:
_LL2868: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2872=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2872[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2873; _temp2873.tag= Cyc_Stmt_tok; _temp2873.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2873;}); _temp2872;}); break; case 282:
_LL2871: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2875=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2875[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2876; _temp2876.tag= Cyc_Stmt_tok; _temp2876.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2876;}); _temp2875;}); break; case 283: _LL2874: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2878=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2878[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2879; _temp2879.tag= Cyc_Pattern_tok; _temp2879.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2879;}); _temp2878;}); break; case 284:
_LL2877: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
break; case 285: _LL2880: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2882=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2882[ 0]=({ struct Cyc_Pattern_tok_struct _temp2883; _temp2883.tag= Cyc_Pattern_tok;
_temp2883.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2884=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2884[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2885; _temp2885.tag= Cyc_Absyn_Int_p;
_temp2885.f1=( void*)(* Cyc_yyget_Int_tok( yyvs[ yyvsp_offset])).f1; _temp2885.f2=(*
Cyc_yyget_Int_tok( yyvs[ yyvsp_offset])).f2; _temp2885;}); _temp2884;}), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2883;});
_temp2882;}); break; case 286: _LL2881: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2887=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2887[ 0]=({ struct Cyc_Pattern_tok_struct _temp2888; _temp2888.tag= Cyc_Pattern_tok;
_temp2888.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2889=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2889[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2890; _temp2890.tag= Cyc_Absyn_Int_p;
_temp2890.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2890.f2= -(* Cyc_yyget_Int_tok(
yyvs[ yyvsp_offset])).f2; _temp2890;}); _temp2889;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2888;}); _temp2887;}); break; case 287:
_LL2886: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2892=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2892[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2893; _temp2893.tag= Cyc_Pattern_tok; _temp2893.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2894=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2894[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2895; _temp2895.tag= Cyc_Absyn_Float_p; _temp2895.f1=
Cyc_yyget_String_tok( yyvs[ yyvsp_offset]); _temp2895;}); _temp2894;}), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2893;});
_temp2892;}); break; case 288: _LL2891: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2897=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2897[ 0]=({ struct Cyc_Pattern_tok_struct _temp2898; _temp2898.tag= Cyc_Pattern_tok;
_temp2898.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2899=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2899[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2900; _temp2900.tag= Cyc_Absyn_Char_p; _temp2900.f1= Cyc_yyget_Char_tok(
yyvs[ yyvsp_offset]); _temp2900;}); _temp2899;}), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2898;});
_temp2897;}); break; case 289: _LL2896: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2902=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2902[ 0]=({ struct Cyc_Pattern_tok_struct _temp2903; _temp2903.tag= Cyc_Pattern_tok;
_temp2903.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp2903;});
_temp2902;}); break; case 290: _LL2901: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2905=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2905[ 0]=({ struct Cyc_Pattern_tok_struct _temp2906; _temp2906.tag= Cyc_Pattern_tok;
_temp2906.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2907=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2907[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2908; _temp2908.tag=
Cyc_Absyn_UnknownId_p; _temp2908.f1= Cyc_yyget_QualId_tok( yyvs[ yyvsp_offset]);
_temp2908;}); _temp2907;}), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2906;}); _temp2905;}); break; case 291:
_LL2904: { struct Cyc_List_List* _temp2910=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2911=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2911[ 0]=({ struct Cyc_Pattern_tok_struct _temp2912; _temp2912.tag= Cyc_Pattern_tok;
_temp2912.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2913=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2913[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2914; _temp2914.tag= Cyc_Absyn_UnknownCall_p; _temp2914.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2914.f2=
_temp2910; _temp2914.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2914;});
_temp2913;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2912;}); _temp2911;}); break;} case 292: _LL2909: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2916=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2916[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2917; _temp2917.tag= Cyc_Pattern_tok; _temp2917.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp2918=( struct Cyc_Absyn_Tuple_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp2918[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp2919; _temp2919.tag= Cyc_Absyn_Tuple_p; _temp2919.f1=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2919;}); _temp2918;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2917;}); _temp2916;}); break; case 293:
_LL2915: { struct Cyc_List_List* _temp2921=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2922=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2922[ 0]=({ struct Cyc_Pattern_tok_struct _temp2923; _temp2923.tag= Cyc_Pattern_tok;
_temp2923.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2924=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2924[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2925; _temp2925.tag= Cyc_Absyn_UnknownFields_p; _temp2925.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2925.f2=
_temp2921; _temp2925.f3= 0; _temp2925;}); _temp2924;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp2923;}); _temp2922;}); break;} case 294:
_LL2920: { struct Cyc_List_List* _temp2927=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2928=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2928[ 0]=({ struct Cyc_Pattern_tok_struct _temp2929; _temp2929.tag= Cyc_Pattern_tok;
_temp2929.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2930=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2930[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2931; _temp2931.tag= Cyc_Absyn_UnknownFields_p; _temp2931.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2931.f2=
_temp2927; _temp2931.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2931;});
_temp2930;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2929;}); _temp2928;}); break;} case 295: _LL2926: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2933=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2933[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2934; _temp2934.tag= Cyc_Pattern_tok; _temp2934.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp2935=( struct Cyc_Absyn_Pointer_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp2935[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp2936; _temp2936.tag= Cyc_Absyn_Pointer_p;
_temp2936.f1= Cyc_yyget_Pattern_tok( yyvs[ yyvsp_offset]); _temp2936;});
_temp2935;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp2934;}); _temp2933;}); break; case 296: _LL2932: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2938=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2938[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2939; _temp2939.tag= Cyc_Pattern_tok; _temp2939.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp2940=( struct Cyc_Absyn_Reference_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp2940[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp2941; _temp2941.tag= Cyc_Absyn_Reference_p;
_temp2941.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp2942=( struct
_tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2942->f1=( void*) Cyc_Absyn_Loc_n;
_temp2942->f2=({ struct _tagged_arr* _temp2943=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2943[ 0]= Cyc_yyget_String_tok( yyvs[
yyvsp_offset]); _temp2943;}); _temp2942;}),( void*) Cyc_Absyn_VoidType, 0);
_temp2941;}); _temp2940;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2939;}); _temp2938;}); break; case 297: _LL2937:
yyval=( void*)({ struct Cyc_PatternList_tok_struct* _temp2945=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2945[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2946; _temp2946.tag= Cyc_PatternList_tok;
_temp2946.f1= 0; _temp2946;}); _temp2945;}); break; case 298: _LL2944: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2948=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2948[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2949; _temp2949.tag= Cyc_PatternList_tok;
_temp2949.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ yyvsp_offset])); _temp2949;}); _temp2948;});
break; case 299: _LL2947: yyval=( void*)({ struct Cyc_PatternList_tok_struct*
_temp2951=( struct Cyc_PatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp2951[ 0]=({ struct Cyc_PatternList_tok_struct _temp2952; _temp2952.tag= Cyc_PatternList_tok;
_temp2952.f1=({ struct Cyc_List_List* _temp2953=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2953->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ yyvsp_offset]); _temp2953->tl= 0; _temp2953;}); _temp2952;}); _temp2951;});
break; case 300: _LL2950: yyval=( void*)({ struct Cyc_PatternList_tok_struct*
_temp2955=( struct Cyc_PatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp2955[ 0]=({ struct Cyc_PatternList_tok_struct _temp2956; _temp2956.tag= Cyc_PatternList_tok;
_temp2956.f1=({ struct Cyc_List_List* _temp2957=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2957->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ yyvsp_offset]); _temp2957->tl= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2957;});
_temp2956;}); _temp2955;}); break; case 301: _LL2954: yyval=( void*)({ struct
Cyc_FieldPattern_tok_struct* _temp2959=( struct Cyc_FieldPattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp2959[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp2960; _temp2960.tag= Cyc_FieldPattern_tok;
_temp2960.f1=({ struct _tuple13* _temp2961=( struct _tuple13*) _cycalloc(
sizeof( struct _tuple13)); _temp2961->f1= 0; _temp2961->f2= Cyc_yyget_Pattern_tok(
yyvs[ yyvsp_offset]); _temp2961;}); _temp2960;}); _temp2959;}); break; case 302:
_LL2958: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2963=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2963[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2964; _temp2964.tag=
Cyc_FieldPattern_tok; _temp2964.f1=({ struct _tuple13* _temp2965=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2965->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2965->f2=
Cyc_yyget_Pattern_tok( yyvs[ yyvsp_offset]); _temp2965;}); _temp2964;});
_temp2963;}); break; case 303: _LL2962: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2967=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2967[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2968; _temp2968.tag= Cyc_FieldPatternList_tok; _temp2968.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(
yyvs[ yyvsp_offset])); _temp2968;}); _temp2967;}); break; case 304: _LL2966:
yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2970=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2970[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2971; _temp2971.tag= Cyc_FieldPatternList_tok;
_temp2971.f1=({ struct Cyc_List_List* _temp2972=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2972->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ yyvsp_offset]); _temp2972->tl= 0; _temp2972;}); _temp2971;}); _temp2970;});
break; case 305: _LL2969: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2974=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2974[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2975; _temp2975.tag= Cyc_FieldPatternList_tok; _temp2975.f1=({ struct Cyc_List_List*
_temp2976=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2976->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[ yyvsp_offset]);
_temp2976->tl= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2976;});
_temp2975;}); _temp2974;}); break; case 306: _LL2973: yyval= yyvs[ yyvsp_offset];
break; case 307: _LL2977: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2979=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2979[ 0]=({ struct Cyc_Exp_tok_struct _temp2980; _temp2980.tag= Cyc_Exp_tok;
_temp2980.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2980;}); _temp2979;}); break; case 308: _LL2978:
yyval= yyvs[ yyvsp_offset]; break; case 309: _LL2981: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2983=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2983[ 0]=({ struct Cyc_Exp_tok_struct
_temp2984; _temp2984.tag= Cyc_Exp_tok; _temp2984.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp2984;}); _temp2983;}); break; case 310: _LL2982:
yyval=( void*)({ struct Cyc_Primopopt_tok_struct* _temp2986=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2986[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2987; _temp2987.tag= Cyc_Primopopt_tok; _temp2987.f1=
0; _temp2987;}); _temp2986;}); break; case 311: _LL2985: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2989=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2989[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2990; _temp2990.tag= Cyc_Primopopt_tok; _temp2990.f1=({
struct Cyc_Core_Opt* _temp2991=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2991->v=( void*)(( void*) Cyc_Absyn_Times); _temp2991;});
_temp2990;}); _temp2989;}); break; case 312: _LL2988: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2993=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2993[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2994; _temp2994.tag= Cyc_Primopopt_tok; _temp2994.f1=({
struct Cyc_Core_Opt* _temp2995=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2995->v=( void*)(( void*) Cyc_Absyn_Div); _temp2995;});
_temp2994;}); _temp2993;}); break; case 313: _LL2992: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2997=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2997[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2998; _temp2998.tag= Cyc_Primopopt_tok; _temp2998.f1=({
struct Cyc_Core_Opt* _temp2999=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2999->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2999;});
_temp2998;}); _temp2997;}); break; case 314: _LL2996: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3001=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3001[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3002; _temp3002.tag= Cyc_Primopopt_tok; _temp3002.f1=({
struct Cyc_Core_Opt* _temp3003=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3003->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3003;});
_temp3002;}); _temp3001;}); break; case 315: _LL3000: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3005=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3005[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3006; _temp3006.tag= Cyc_Primopopt_tok; _temp3006.f1=({
struct Cyc_Core_Opt* _temp3007=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3007->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3007;});
_temp3006;}); _temp3005;}); break; case 316: _LL3004: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3009=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3009[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3010; _temp3010.tag= Cyc_Primopopt_tok; _temp3010.f1=({
struct Cyc_Core_Opt* _temp3011=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3011->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3011;});
_temp3010;}); _temp3009;}); break; case 317: _LL3008: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3013=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3013[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3014; _temp3014.tag= Cyc_Primopopt_tok; _temp3014.f1=({
struct Cyc_Core_Opt* _temp3015=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3015->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3015;});
_temp3014;}); _temp3013;}); break; case 318: _LL3012: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3017=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3017[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3018; _temp3018.tag= Cyc_Primopopt_tok; _temp3018.f1=({
struct Cyc_Core_Opt* _temp3019=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3019->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3019;});
_temp3018;}); _temp3017;}); break; case 319: _LL3016: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3021=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3021[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3022; _temp3022.tag= Cyc_Primopopt_tok; _temp3022.f1=({
struct Cyc_Core_Opt* _temp3023=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3023->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3023;});
_temp3022;}); _temp3021;}); break; case 320: _LL3020: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3025=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3025[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3026; _temp3026.tag= Cyc_Primopopt_tok; _temp3026.f1=({
struct Cyc_Core_Opt* _temp3027=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3027->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3027;});
_temp3026;}); _temp3025;}); break; case 321: _LL3024: yyval= yyvs[ yyvsp_offset];
break; case 322: _LL3028: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3030=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3030[ 0]=({ struct Cyc_Exp_tok_struct _temp3031; _temp3031.tag= Cyc_Exp_tok;
_temp3031.f1= Cyc_Absyn_conditional_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3031;}); _temp3030;}); break; case 323: _LL3029:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3033=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3033[ 0]=({ struct Cyc_Exp_tok_struct
_temp3034; _temp3034.tag= Cyc_Exp_tok; _temp3034.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3034;}); _temp3033;}); break; case 324: _LL3032:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3036=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3036[ 0]=({ struct Cyc_Exp_tok_struct
_temp3037; _temp3037.tag= Cyc_Exp_tok; _temp3037.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3037;}); _temp3036;}); break; case 325: _LL3035: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3039=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3039[ 0]=({ struct Cyc_Exp_tok_struct
_temp3040; _temp3040.tag= Cyc_Exp_tok; _temp3040.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3040;}); _temp3039;}); break; case 326: _LL3038: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3042=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3042[ 0]=({ struct Cyc_Exp_tok_struct
_temp3043; _temp3043.tag= Cyc_Exp_tok; _temp3043.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3043;}); _temp3042;}); break; case 327:
_LL3041: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3045=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3045[ 0]=({ struct Cyc_Exp_tok_struct
_temp3046; _temp3046.tag= Cyc_Exp_tok; _temp3046.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3046;}); _temp3045;}); break; case 328:
_LL3044: yyval= yyvs[ yyvsp_offset]; break; case 329: _LL3047: yyval= yyvs[
yyvsp_offset]; break; case 330: _LL3048: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3050=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3050[ 0]=({ struct Cyc_Exp_tok_struct _temp3051; _temp3051.tag= Cyc_Exp_tok;
_temp3051.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3051;}); _temp3050;}); break; case 331: _LL3049:
yyval= yyvs[ yyvsp_offset]; break; case 332: _LL3052: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3054=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3054[ 0]=({ struct Cyc_Exp_tok_struct
_temp3055; _temp3055.tag= Cyc_Exp_tok; _temp3055.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3055;}); _temp3054;}); break; case 333: _LL3053:
yyval= yyvs[ yyvsp_offset]; break; case 334: _LL3056: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3058=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3058[ 0]=({ struct Cyc_Exp_tok_struct
_temp3059; _temp3059.tag= Cyc_Exp_tok; _temp3059.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3059;}); _temp3058;}); break; case 335:
_LL3057: yyval= yyvs[ yyvsp_offset]; break; case 336: _LL3060: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3062=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3062[ 0]=({ struct Cyc_Exp_tok_struct
_temp3063; _temp3063.tag= Cyc_Exp_tok; _temp3063.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3063;}); _temp3062;}); break; case 337:
_LL3061: yyval= yyvs[ yyvsp_offset]; break; case 338: _LL3064: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3066=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3066[ 0]=({ struct Cyc_Exp_tok_struct
_temp3067; _temp3067.tag= Cyc_Exp_tok; _temp3067.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3067;}); _temp3066;}); break; case 339:
_LL3065: yyval= yyvs[ yyvsp_offset]; break; case 340: _LL3068: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3070=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3070[ 0]=({ struct Cyc_Exp_tok_struct
_temp3071; _temp3071.tag= Cyc_Exp_tok; _temp3071.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3071;}); _temp3070;}); break; case 341: _LL3069:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3073=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3073[ 0]=({ struct Cyc_Exp_tok_struct
_temp3074; _temp3074.tag= Cyc_Exp_tok; _temp3074.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3074;}); _temp3073;}); break; case 342: _LL3072:
yyval= yyvs[ yyvsp_offset]; break; case 343: _LL3075: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3077=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3077[ 0]=({ struct Cyc_Exp_tok_struct
_temp3078; _temp3078.tag= Cyc_Exp_tok; _temp3078.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3078;}); _temp3077;}); break; case 344: _LL3076:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3080=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3080[ 0]=({ struct Cyc_Exp_tok_struct
_temp3081; _temp3081.tag= Cyc_Exp_tok; _temp3081.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3081;}); _temp3080;}); break; case 345: _LL3079:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3083=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3083[ 0]=({ struct Cyc_Exp_tok_struct
_temp3084; _temp3084.tag= Cyc_Exp_tok; _temp3084.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3084;}); _temp3083;}); break; case 346: _LL3082:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3086=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3086[ 0]=({ struct Cyc_Exp_tok_struct
_temp3087; _temp3087.tag= Cyc_Exp_tok; _temp3087.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3087;}); _temp3086;}); break; case 347: _LL3085:
yyval= yyvs[ yyvsp_offset]; break; case 348: _LL3088: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3090=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3090[ 0]=({ struct Cyc_Exp_tok_struct
_temp3091; _temp3091.tag= Cyc_Exp_tok; _temp3091.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3091;}); _temp3090;}); break; case 349:
_LL3089: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3093=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3093[ 0]=({ struct Cyc_Exp_tok_struct
_temp3094; _temp3094.tag= Cyc_Exp_tok; _temp3094.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3094;}); _temp3093;}); break; case 350:
_LL3092: yyval= yyvs[ yyvsp_offset]; break; case 351: _LL3095: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3097=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3097[ 0]=({ struct Cyc_Exp_tok_struct
_temp3098; _temp3098.tag= Cyc_Exp_tok; _temp3098.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3098;}); _temp3097;}); break; case 352:
_LL3096: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3100=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3100[ 0]=({ struct Cyc_Exp_tok_struct
_temp3101; _temp3101.tag= Cyc_Exp_tok; _temp3101.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3101;}); _temp3100;}); break; case 353:
_LL3099: yyval= yyvs[ yyvsp_offset]; break; case 354: _LL3102: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3104=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3104[ 0]=({ struct Cyc_Exp_tok_struct
_temp3105; _temp3105.tag= Cyc_Exp_tok; _temp3105.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3105;}); _temp3104;}); break; case 355:
_LL3103: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3107=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3107[ 0]=({ struct Cyc_Exp_tok_struct
_temp3108; _temp3108.tag= Cyc_Exp_tok; _temp3108.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3108;}); _temp3107;}); break; case 356:
_LL3106: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3110=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3110[ 0]=({ struct Cyc_Exp_tok_struct
_temp3111; _temp3111.tag= Cyc_Exp_tok; _temp3111.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3111;}); _temp3110;}); break; case 357:
_LL3109: yyval= yyvs[ yyvsp_offset]; break; case 358: _LL3112: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3114=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3114[ 0]=({ struct Cyc_Exp_tok_struct
_temp3115; _temp3115.tag= Cyc_Exp_tok; _temp3115.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3115;}); _temp3114;}); break; case 359: _LL3113:
yyval= yyvs[ yyvsp_offset]; break; case 360: _LL3116: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3118=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3118[ 0]=({ struct Cyc_Exp_tok_struct
_temp3119; _temp3119.tag= Cyc_Exp_tok; _temp3119.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3119;}); _temp3118;}); break; case 361: _LL3117:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3121=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3121[ 0]=({ struct Cyc_Exp_tok_struct
_temp3122; _temp3122.tag= Cyc_Exp_tok; _temp3122.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3122;}); _temp3121;}); break; case 362: _LL3120:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3124=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3124[ 0]=({ struct Cyc_Exp_tok_struct
_temp3125; _temp3125.tag= Cyc_Exp_tok; _temp3125.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3125;}); _temp3124;}); break; case 363: _LL3123:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3127=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3127[ 0]=({ struct Cyc_Exp_tok_struct
_temp3128; _temp3128.tag= Cyc_Exp_tok; _temp3128.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3128;}); _temp3127;}); break; case 364: _LL3126:
yyval= yyvs[ yyvsp_offset]; break; case 365: _LL3129: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3131=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3131[ 0]=({ struct Cyc_Exp_tok_struct
_temp3132; _temp3132.tag= Cyc_Exp_tok; _temp3132.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3132;}); _temp3131;}); break; case 366: _LL3130:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3134=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3134[ 0]=({ struct Cyc_Exp_tok_struct
_temp3135; _temp3135.tag= Cyc_Exp_tok; _temp3135.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3135;}); _temp3134;}); break; case 367: _LL3133:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3137=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3137[ 0]=({ struct Cyc_Exp_tok_struct
_temp3138; _temp3138.tag= Cyc_Exp_tok; _temp3138.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3138;}); _temp3137;}); break; case 368: _LL3136:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3140=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3140[ 0]=({ struct Cyc_Exp_tok_struct
_temp3141; _temp3141.tag= Cyc_Exp_tok; _temp3141.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_StructField_struct*
_temp3142=( struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp3142[ 0]=({ struct Cyc_Absyn_StructField_struct _temp3143; _temp3143.tag=
Cyc_Absyn_StructField; _temp3143.f1=({ struct _tagged_arr* _temp3144=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp3144[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3144;});
_temp3143;}); _temp3142;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3141;}); _temp3140;}); break; case 369: _LL3139:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3146=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3146[ 0]=({ struct Cyc_Exp_tok_struct
_temp3147; _temp3147.tag= Cyc_Exp_tok; _temp3147.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_TupleIndex_struct*
_temp3148=( struct Cyc_Absyn_TupleIndex_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_TupleIndex_struct)); _temp3148[ 0]=({ struct Cyc_Absyn_TupleIndex_struct
_temp3149; _temp3149.tag= Cyc_Absyn_TupleIndex; _temp3149.f1=( unsigned int)(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)])).f2; _temp3149;}); _temp3148;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3147;}); _temp3146;}); break; case 370: _LL3145: {
struct Cyc_Position_Segment* _temp3151= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
yylsp_offset]).last_line); struct Cyc_List_List* _temp3152=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, _temp3151,
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3153=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3153[ 0]=({ struct Cyc_Exp_tok_struct _temp3154; _temp3154.tag= Cyc_Exp_tok;
_temp3154.f1= Cyc_Absyn_gentyp_exp( _temp3152,(* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3154;}); _temp3153;}); break;} case 371:
_LL3150: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3156=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3156[ 0]=({ struct Cyc_Exp_tok_struct
_temp3157; _temp3157.tag= Cyc_Exp_tok; _temp3157.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3158=( struct Cyc_Absyn_Malloc_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3158[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3159; _temp3159.tag= Cyc_Absyn_Malloc_e;
_temp3159.f1=({ struct Cyc_Absyn_MallocInfo _temp3160; _temp3160.is_calloc= 0;
_temp3160.rgn= 0; _temp3160.elt_type= 0; _temp3160.num_elts= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3160.fat_result=
0; _temp3160;}); _temp3159;}); _temp3158;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3157;}); _temp3156;}); break; case 372: _LL3155:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3162=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3162[ 0]=({ struct Cyc_Exp_tok_struct
_temp3163; _temp3163.tag= Cyc_Exp_tok; _temp3163.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3164=( struct Cyc_Absyn_Malloc_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3164[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3165; _temp3165.tag= Cyc_Absyn_Malloc_e;
_temp3165.f1=({ struct Cyc_Absyn_MallocInfo _temp3166; _temp3166.is_calloc= 0;
_temp3166.rgn=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp3166.elt_type=
0; _temp3166.num_elts= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp3166.fat_result= 0; _temp3166;}); _temp3165;});
_temp3164;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3163;}); _temp3162;}); break; case 373: _LL3161: { void* _temp3170; struct
_tuple2 _temp3168=* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _LL3171: _temp3170=
_temp3168.f3; goto _LL3169; _LL3169: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3172=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3172[ 0]=({ struct Cyc_Exp_tok_struct _temp3173; _temp3173.tag= Cyc_Exp_tok;
_temp3173.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3174=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3174[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3175; _temp3175.tag= Cyc_Absyn_Malloc_e;
_temp3175.f1=({ struct Cyc_Absyn_MallocInfo _temp3176; _temp3176.is_calloc= 1;
_temp3176.rgn= 0; _temp3176.elt_type=({ void** _temp3177=( void**) _cycalloc(
sizeof( void*)); _temp3177[ 0]= _temp3170; _temp3177;}); _temp3176.num_elts= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3176.fat_result=
0; _temp3176;}); _temp3175;}); _temp3174;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3173;}); _temp3172;}); break;} case 374:
_LL3167: { void* _temp3181; struct _tuple2 _temp3179=* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _LL3182:
_temp3181= _temp3179.f3; goto _LL3180; _LL3180: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3183=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3183[ 0]=({ struct Cyc_Exp_tok_struct _temp3184; _temp3184.tag= Cyc_Exp_tok;
_temp3184.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3185=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3185[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3186; _temp3186.tag= Cyc_Absyn_Malloc_e;
_temp3186.f1=({ struct Cyc_Absyn_MallocInfo _temp3187; _temp3187.is_calloc= 1;
_temp3187.rgn=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  8)]); _temp3187.elt_type=({
void** _temp3188=( void**) _cycalloc( sizeof( void*)); _temp3188[ 0]= _temp3181;
_temp3188;}); _temp3187.num_elts= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3187.fat_result=
0; _temp3187;}); _temp3186;}); _temp3185;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  10)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3184;}); _temp3183;}); break;} case 375:
_LL3178: yyval=( void*)({ struct Cyc_Primop_tok_struct* _temp3190=( struct Cyc_Primop_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primop_tok_struct)); _temp3190[ 0]=({ struct Cyc_Primop_tok_struct
_temp3191; _temp3191.tag= Cyc_Primop_tok; _temp3191.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp3191;}); _temp3190;}); break; case 376: _LL3189: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3193=( struct Cyc_Primop_tok_struct*) _cycalloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3193[ 0]=({ struct Cyc_Primop_tok_struct
_temp3194; _temp3194.tag= Cyc_Primop_tok; _temp3194.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp3194;}); _temp3193;}); break; case 377: _LL3192: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3196=( struct Cyc_Primop_tok_struct*) _cycalloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3196[ 0]=({ struct Cyc_Primop_tok_struct
_temp3197; _temp3197.tag= Cyc_Primop_tok; _temp3197.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp3197;}); _temp3196;}); break; case 378: _LL3195: yyval= yyvs[ yyvsp_offset];
break; case 379: _LL3198: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3200=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3200[ 0]=({ struct Cyc_Exp_tok_struct _temp3201; _temp3201.tag= Cyc_Exp_tok;
_temp3201.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3201;}); _temp3200;}); break; case 380:
_LL3199: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3203=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3203[ 0]=({ struct Cyc_Exp_tok_struct
_temp3204; _temp3204.tag= Cyc_Exp_tok; _temp3204.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3204;}); _temp3203;}); break; case 381: _LL3202: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3206=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3206[ 0]=({ struct Cyc_Exp_tok_struct
_temp3207; _temp3207.tag= Cyc_Exp_tok; _temp3207.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3207;}); _temp3206;}); break; case 382: _LL3205:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3209=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3209[ 0]=({ struct Cyc_Exp_tok_struct
_temp3210; _temp3210.tag= Cyc_Exp_tok; _temp3210.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),({ struct _tagged_arr* _temp3211=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp3211[ 0]= Cyc_yyget_String_tok( yyvs[ yyvsp_offset]);
_temp3211;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3210;}); _temp3209;}); break; case 383: _LL3208: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); if( Cyc_Absyn_is_qvar_qualified( q)){ Cyc_Parse_err(
_tag_arr("struct field name is qualified", sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3213=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3213[ 0]=({ struct Cyc_Exp_tok_struct
_temp3214; _temp3214.tag= Cyc_Exp_tok; _temp3214.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3214;}); _temp3213;}); break;} case 384:
_LL3212: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3216=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3216[ 0]=({ struct Cyc_Exp_tok_struct
_temp3217; _temp3217.tag= Cyc_Exp_tok; _temp3217.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),({ struct _tagged_arr* _temp3218=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp3218[ 0]= Cyc_yyget_String_tok( yyvs[ yyvsp_offset]);
_temp3218;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3217;}); _temp3216;}); break; case 385: _LL3215: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ yyvsp_offset]); if( Cyc_Absyn_is_qvar_qualified( q)){ Cyc_Parse_err(
_tag_arr("struct field name is qualified", sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3220=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3220[ 0]=({ struct Cyc_Exp_tok_struct
_temp3221; _temp3221.tag= Cyc_Exp_tok; _temp3221.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3221;}); _temp3220;}); break;} case 386:
_LL3219: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3223=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3223[ 0]=({ struct Cyc_Exp_tok_struct
_temp3224; _temp3224.tag= Cyc_Exp_tok; _temp3224.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3224;}); _temp3223;}); break; case 387:
_LL3222: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3226=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3226[ 0]=({ struct Cyc_Exp_tok_struct
_temp3227; _temp3227.tag= Cyc_Exp_tok; _temp3227.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3227;}); _temp3226;}); break; case 388:
_LL3225: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3229=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3229[ 0]=({ struct Cyc_Exp_tok_struct
_temp3230; _temp3230.tag= Cyc_Exp_tok; _temp3230.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3231=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3231[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3232; _temp3232.tag= Cyc_Absyn_CompoundLit_e;
_temp3232.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3232.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3232;});
_temp3231;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3230;}); _temp3229;}); break; case 389: _LL3228: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3234=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3234[ 0]=({ struct Cyc_Exp_tok_struct
_temp3235; _temp3235.tag= Cyc_Exp_tok; _temp3235.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3236=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3236[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3237; _temp3237.tag= Cyc_Absyn_CompoundLit_e;
_temp3237.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3237.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3237;});
_temp3236;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3235;}); _temp3234;}); break; case 390: _LL3233: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3239=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3239[ 0]=({ struct Cyc_Exp_tok_struct
_temp3240; _temp3240.tag= Cyc_Exp_tok; _temp3240.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3241=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3241[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3242; _temp3242.tag= Cyc_Absyn_Fill_e; _temp3242.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3242;});
_temp3241;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ yylsp_offset]).last_line));
_temp3240;}); _temp3239;}); break; case 391: _LL3238: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3244=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3244[ 0]=({ struct Cyc_Exp_tok_struct
_temp3245; _temp3245.tag= Cyc_Exp_tok; _temp3245.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3246=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3246[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3247; _temp3247.tag= Cyc_Absyn_Codegen_e;
_temp3247.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3247;}); _temp3246;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3245;}); _temp3244;}); break; case 392:
_LL3243: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3249=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3249[ 0]=({ struct Cyc_Exp_tok_struct
_temp3250; _temp3250.tag= Cyc_Exp_tok; _temp3250.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3251=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3251[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3252; _temp3252.tag= Cyc_Absyn_UnknownId_e;
_temp3252.f1= Cyc_yyget_QualId_tok( yyvs[ yyvsp_offset]); _temp3252;});
_temp3251;}), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3250;}); _temp3249;}); break; case 393:
_LL3248: yyval= yyvs[ yyvsp_offset]; break; case 394: _LL3253: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3255=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3255[ 0]=({ struct Cyc_Exp_tok_struct
_temp3256; _temp3256.tag= Cyc_Exp_tok; _temp3256.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3256;}); _temp3255;}); break; case 395:
_LL3254: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)];
break; case 396: _LL3257: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3259=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3259[ 0]=({ struct Cyc_Exp_tok_struct _temp3260; _temp3260.tag= Cyc_Exp_tok;
_temp3260.f1= Cyc_Absyn_noinstantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_UnknownId_e_struct* _temp3261=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3261[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3262; _temp3262.tag= Cyc_Absyn_UnknownId_e;
_temp3262.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3262;}); _temp3261;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ yylsp_offset]).last_line)); _temp3260;});
_temp3259;}); break; case 397: _LL3258: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3264=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3264[ 0]=({ struct Cyc_Exp_tok_struct _temp3265; _temp3265.tag= Cyc_Exp_tok;
_temp3265.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3266=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3266[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3267; _temp3267.tag=
Cyc_Absyn_UnknownId_e; _temp3267.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3267;});
_temp3266;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3265;}); _temp3264;}); break; case 398:
_LL3263: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3269=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3269[ 0]=({ struct Cyc_Exp_tok_struct
_temp3270; _temp3270.tag= Cyc_Exp_tok; _temp3270.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3270;}); _temp3269;}); break; case 399:
_LL3268: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3272=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3272[ 0]=({ struct Cyc_Exp_tok_struct
_temp3273; _temp3273.tag= Cyc_Exp_tok; _temp3273.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3274=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3274[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3275; _temp3275.tag= Cyc_Absyn_Struct_e;
_temp3275.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3275.f2= 0; _temp3275.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3275.f4= 0;
_temp3275;}); _temp3274;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3273;}); _temp3272;}); break; case 400: _LL3271:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3277=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3277[ 0]=({ struct Cyc_Exp_tok_struct
_temp3278; _temp3278.tag= Cyc_Exp_tok; _temp3278.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ yylsp_offset]).last_line)); _temp3278;}); _temp3277;}); break; case 401:
_LL3276: yyval=( void*)({ struct Cyc_ExpList_tok_struct* _temp3280=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3280[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3281; _temp3281.tag= Cyc_ExpList_tok; _temp3281.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
yyvsp_offset])); _temp3281;}); _temp3280;}); break; case 402: _LL3279: yyval=(
void*)({ struct Cyc_ExpList_tok_struct* _temp3283=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3283[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3284; _temp3284.tag= Cyc_ExpList_tok; _temp3284.f1=({ struct Cyc_List_List*
_temp3285=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3285->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp3285->tl= 0;
_temp3285;}); _temp3284;}); _temp3283;}); break; case 403: _LL3282: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3287=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3287[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3288; _temp3288.tag= Cyc_ExpList_tok; _temp3288.f1=({ struct Cyc_List_List*
_temp3289=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3289->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ yyvsp_offset]); _temp3289->tl=
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp3289;}); _temp3288;}); _temp3287;}); break; case 404:
_LL3286: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3291=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3291[ 0]=({ struct Cyc_Exp_tok_struct
_temp3292; _temp3292.tag= Cyc_Exp_tok; _temp3292.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ yyvsp_offset])).f1,(* Cyc_yyget_Int_tok( yyvs[ yyvsp_offset])).f2, Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp3292;});
_temp3291;}); break; case 405: _LL3290: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3294=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3294[ 0]=({ struct Cyc_Exp_tok_struct _temp3295; _temp3295.tag= Cyc_Exp_tok;
_temp3295.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[ yyvsp_offset]), Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp3295;});
_temp3294;}); break; case 406: _LL3293: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3297=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3297[ 0]=({ struct Cyc_Exp_tok_struct _temp3298; _temp3298.tag= Cyc_Exp_tok;
_temp3298.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[ yyvsp_offset]),
Cyc_Position_segment_of_abs(( yyls[ yylsp_offset]).first_line,( yyls[
yylsp_offset]).last_line)); _temp3298;}); _temp3297;}); break; case 407: _LL3296:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3300=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3300[ 0]=({ struct Cyc_Exp_tok_struct
_temp3301; _temp3301.tag= Cyc_Exp_tok; _temp3301.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs((
yyls[ yylsp_offset]).first_line,( yyls[ yylsp_offset]).last_line)); _temp3301;});
_temp3300;}); break; case 408: _LL3299: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3303=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3303[ 0]=({ struct Cyc_QualId_tok_struct _temp3304; _temp3304.tag= Cyc_QualId_tok;
_temp3304.f1=({ struct _tuple1* _temp3305=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3305->f1= Cyc_Absyn_rel_ns_null; _temp3305->f2=({ struct
_tagged_arr* _temp3306=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3306[ 0]= Cyc_yyget_String_tok( yyvs[ yyvsp_offset]);
_temp3306;}); _temp3305;}); _temp3304;}); _temp3303;}); break; case 409: _LL3302:
yyval= yyvs[ yyvsp_offset]; break; default: _LL3307: break;} yyvsp_offset -=
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
Cyc_yytname[ x]) +  15, count ++);}} msg=({ unsigned int _temp3309=(
unsigned int)( sze +  15); unsigned char* _temp3310=( unsigned char*)
_cycalloc_atomic( _check_times( sizeof( unsigned char), _temp3309)); struct
_tagged_arr _temp3312= _tag_arr( _temp3310, sizeof( unsigned char),(
unsigned int)( sze +  15));{ unsigned int _temp3311= _temp3309; unsigned int i;
for( i= 0; i <  _temp3311; i ++){ _temp3310[ i]='\000';}}; _temp3312;}); Cyc_Std_strcpy(
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
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3322= v; struct
_tuple15* _temp3340; struct _tuple15 _temp3342; int _temp3343; unsigned char
_temp3345; short _temp3347; struct _tagged_arr _temp3349; struct Cyc_Core_Opt*
_temp3351; struct Cyc_Core_Opt* _temp3353; struct Cyc_Core_Opt _temp3355; struct
_tagged_arr* _temp3356; struct _tuple1* _temp3358; struct _tuple1 _temp3360;
struct _tagged_arr* _temp3361; void* _temp3363; _LL3324: if(*(( void**)
_temp3322) ==  Cyc_Int_tok){ _LL3341: _temp3340=(( struct Cyc_Int_tok_struct*)
_temp3322)->f1; _temp3342=* _temp3340; _LL3344: _temp3343= _temp3342.f2; goto
_LL3325;} else{ goto _LL3326;} _LL3326: if(*(( void**) _temp3322) ==  Cyc_Char_tok){
_LL3346: _temp3345=(( struct Cyc_Char_tok_struct*) _temp3322)->f1; goto _LL3327;}
else{ goto _LL3328;} _LL3328: if(*(( void**) _temp3322) ==  Cyc_Short_tok){
_LL3348: _temp3347=(( struct Cyc_Short_tok_struct*) _temp3322)->f1; goto _LL3329;}
else{ goto _LL3330;} _LL3330: if(*(( void**) _temp3322) ==  Cyc_String_tok){
_LL3350: _temp3349=(( struct Cyc_String_tok_struct*) _temp3322)->f1; goto
_LL3331;} else{ goto _LL3332;} _LL3332: if(*(( void**) _temp3322) ==  Cyc_Stringopt_tok){
_LL3352: _temp3351=(( struct Cyc_Stringopt_tok_struct*) _temp3322)->f1; if(
_temp3351 ==  0){ goto _LL3333;} else{ goto _LL3334;}} else{ goto _LL3334;}
_LL3334: if(*(( void**) _temp3322) ==  Cyc_Stringopt_tok){ _LL3354: _temp3353=((
struct Cyc_Stringopt_tok_struct*) _temp3322)->f1; if( _temp3353 ==  0){ goto
_LL3336;} else{ _temp3355=* _temp3353; _LL3357: _temp3356=( struct _tagged_arr*)
_temp3355.v; goto _LL3335;}} else{ goto _LL3336;} _LL3336: if(*(( void**)
_temp3322) ==  Cyc_QualId_tok){ _LL3359: _temp3358=(( struct Cyc_QualId_tok_struct*)
_temp3322)->f1; _temp3360=* _temp3358; _LL3364: _temp3363= _temp3360.f1; goto
_LL3362; _LL3362: _temp3361= _temp3360.f2; goto _LL3337;} else{ goto _LL3338;}
_LL3338: goto _LL3339; _LL3325:({ struct Cyc_Std_Int_pa_struct _temp3366;
_temp3366.tag= Cyc_Std_Int_pa; _temp3366.f1=( int)(( unsigned int) _temp3343);{
void* _temp3365[ 1u]={& _temp3366}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3365, sizeof( void*), 1u));}}); goto
_LL3323; _LL3327:({ struct Cyc_Std_Int_pa_struct _temp3368; _temp3368.tag= Cyc_Std_Int_pa;
_temp3368.f1=( int)(( unsigned int)(( int) _temp3345));{ void* _temp3367[ 1u]={&
_temp3368}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3367, sizeof( void*), 1u));}}); goto _LL3323;
_LL3329:({ struct Cyc_Std_Int_pa_struct _temp3370; _temp3370.tag= Cyc_Std_Int_pa;
_temp3370.f1=( int)(( unsigned int)(( int) _temp3347));{ void* _temp3369[ 1u]={&
_temp3370}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3369, sizeof( void*), 1u));}}); goto _LL3323;
_LL3331:({ struct Cyc_Std_String_pa_struct _temp3372; _temp3372.tag= Cyc_Std_String_pa;
_temp3372.f1=( struct _tagged_arr) _temp3349;{ void* _temp3371[ 1u]={& _temp3372};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3371, sizeof( void*), 1u));}}); goto _LL3323; _LL3333:({ void*
_temp3373[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3373, sizeof( void*), 0u));}); goto _LL3323;
_LL3335:({ struct Cyc_Std_String_pa_struct _temp3375; _temp3375.tag= Cyc_Std_String_pa;
_temp3375.f1=( struct _tagged_arr)* _temp3356;{ void* _temp3374[ 1u]={&
_temp3375}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3374, sizeof( void*), 1u));}}); goto _LL3323;
_LL3337: { struct Cyc_List_List* _temp3376= 0;{ void* _temp3377= _temp3363;
struct Cyc_List_List* _temp3385; struct Cyc_List_List* _temp3387; _LL3379: if((
unsigned int) _temp3377 >  1u?*(( int*) _temp3377) ==  Cyc_Absyn_Rel_n: 0){
_LL3386: _temp3385=(( struct Cyc_Absyn_Rel_n_struct*) _temp3377)->f1; goto
_LL3380;} else{ goto _LL3381;} _LL3381: if(( unsigned int) _temp3377 >  1u?*((
int*) _temp3377) ==  Cyc_Absyn_Abs_n: 0){ _LL3388: _temp3387=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3377)->f1; goto _LL3382;} else{ goto _LL3383;} _LL3383: if( _temp3377 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3384;} else{ goto _LL3378;} _LL3380: _temp3376=
_temp3385; goto _LL3378; _LL3382: _temp3376= _temp3387; goto _LL3378; _LL3384:
goto _LL3378; _LL3378:;} for( 0; _temp3376 !=  0; _temp3376= _temp3376->tl){({
struct Cyc_Std_String_pa_struct _temp3390; _temp3390.tag= Cyc_Std_String_pa;
_temp3390.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _temp3376->hd);{ void*
_temp3389[ 1u]={& _temp3390}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3389, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3392; _temp3392.tag= Cyc_Std_String_pa;
_temp3392.f1=( struct _tagged_arr)* _temp3361;{ void* _temp3391[ 1u]={&
_temp3392}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3391, sizeof( void*), 1u));}}); goto _LL3323;}
_LL3339:({ void* _temp3393[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3393, sizeof( void*), 0u));}); goto
_LL3323; _LL3323:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3394=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3394->v=(
void*) Cyc_Lexing_from_file( f); _temp3394;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
