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
extern void * _region_calloc(struct _RegionHandle *, unsigned int t,
                             unsigned int n);
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
352u]={ 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 123, 2, 2, 105, 121, 117, 2, 101, 102, 112, 115, 103,
116, 109, 120, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 108, 97, 106, 100, 107, 114, 113, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 110,
2, 111, 119, 104, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 98, 118, 99, 122, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96}; static
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
static unsigned char _temp1255[ 7u]="MALLOC"; static unsigned char _temp1256[ 8u]="RMALLOC";
static unsigned char _temp1257[ 7u]="CALLOC"; static unsigned char _temp1258[ 8u]="RCALLOC";
static unsigned char _temp1259[ 9u]="REGION_T"; static unsigned char _temp1260[
9u]="SIZEOF_T"; static unsigned char _temp1261[ 7u]="REGION"; static
unsigned char _temp1262[ 5u]="RNEW"; static unsigned char _temp1263[ 8u]="REGIONS";
static unsigned char _temp1264[ 4u]="GEN"; static unsigned char _temp1265[ 7u]="PTR_OP";
static unsigned char _temp1266[ 7u]="INC_OP"; static unsigned char _temp1267[ 7u]="DEC_OP";
static unsigned char _temp1268[ 8u]="LEFT_OP"; static unsigned char _temp1269[ 9u]="RIGHT_OP";
static unsigned char _temp1270[ 6u]="LE_OP"; static unsigned char _temp1271[ 6u]="GE_OP";
static unsigned char _temp1272[ 6u]="EQ_OP"; static unsigned char _temp1273[ 6u]="NE_OP";
static unsigned char _temp1274[ 7u]="AND_OP"; static unsigned char _temp1275[ 6u]="OR_OP";
static unsigned char _temp1276[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1277[ 11u]="DIV_ASSIGN"; static unsigned char _temp1278[ 11u]="MOD_ASSIGN";
static unsigned char _temp1279[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1280[ 11u]="SUB_ASSIGN"; static unsigned char _temp1281[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1282[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1283[ 11u]="AND_ASSIGN"; static unsigned char _temp1284[ 11u]="XOR_ASSIGN";
static unsigned char _temp1285[ 10u]="OR_ASSIGN"; static unsigned char _temp1286[
9u]="ELLIPSIS"; static unsigned char _temp1287[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1288[ 12u]="COLON_COLON"; static unsigned char _temp1289[ 11u]="IDENTIFIER";
static unsigned char _temp1290[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1291[ 7u]="STRING"; static unsigned char _temp1292[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1293[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1294[ 9u]="TYPE_VAR"; static unsigned char _temp1295[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1296[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1297[ 10u]="ATTRIBUTE"; static unsigned char _temp1298[ 4u]="';'"; static
unsigned char _temp1299[ 4u]="'{'"; static unsigned char _temp1300[ 4u]="'}'";
static unsigned char _temp1301[ 4u]="'='"; static unsigned char _temp1302[ 4u]="'('";
static unsigned char _temp1303[ 4u]="')'"; static unsigned char _temp1304[ 4u]="','";
static unsigned char _temp1305[ 4u]="'_'"; static unsigned char _temp1306[ 4u]="'$'";
static unsigned char _temp1307[ 4u]="'<'"; static unsigned char _temp1308[ 4u]="'>'";
static unsigned char _temp1309[ 4u]="':'"; static unsigned char _temp1310[ 4u]="'.'";
static unsigned char _temp1311[ 4u]="'['"; static unsigned char _temp1312[ 4u]="']'";
static unsigned char _temp1313[ 4u]="'*'"; static unsigned char _temp1314[ 4u]="'@'";
static unsigned char _temp1315[ 4u]="'?'"; static unsigned char _temp1316[ 4u]="'+'";
static unsigned char _temp1317[ 4u]="'-'"; static unsigned char _temp1318[ 4u]="'&'";
static unsigned char _temp1319[ 4u]="'|'"; static unsigned char _temp1320[ 4u]="'^'";
static unsigned char _temp1321[ 4u]="'/'"; static unsigned char _temp1322[ 4u]="'%'";
static unsigned char _temp1323[ 4u]="'~'"; static unsigned char _temp1324[ 4u]="'!'";
static unsigned char _temp1325[ 5u]="prog"; static unsigned char _temp1326[ 17u]="translation_unit";
static unsigned char _temp1327[ 21u]="external_declaration"; static
unsigned char _temp1328[ 20u]="function_definition"; static unsigned char
_temp1329[ 21u]="function_definition2"; static unsigned char _temp1330[ 13u]="using_action";
static unsigned char _temp1331[ 15u]="unusing_action"; static unsigned char
_temp1332[ 17u]="namespace_action"; static unsigned char _temp1333[ 19u]="unnamespace_action";
static unsigned char _temp1334[ 12u]="declaration"; static unsigned char
_temp1335[ 17u]="declaration_list"; static unsigned char _temp1336[ 23u]="declaration_specifiers";
static unsigned char _temp1337[ 24u]="storage_class_specifier"; static
unsigned char _temp1338[ 15u]="attributes_opt"; static unsigned char _temp1339[
11u]="attributes"; static unsigned char _temp1340[ 15u]="attribute_list"; static
unsigned char _temp1341[ 10u]="attribute"; static unsigned char _temp1342[ 15u]="type_specifier";
static unsigned char _temp1343[ 5u]="kind"; static unsigned char _temp1344[ 15u]="type_qualifier";
static unsigned char _temp1345[ 15u]="enum_specifier"; static unsigned char
_temp1346[ 11u]="enum_field"; static unsigned char _temp1347[ 22u]="enum_declaration_list";
static unsigned char _temp1348[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1349[ 16u]="type_params_opt"; static unsigned char _temp1350[
16u]="struct_or_union"; static unsigned char _temp1351[ 24u]="struct_declaration_list";
static unsigned char _temp1352[ 25u]="struct_declaration_list0"; static
unsigned char _temp1353[ 21u]="init_declarator_list"; static unsigned char
_temp1354[ 22u]="init_declarator_list0"; static unsigned char _temp1355[ 16u]="init_declarator";
static unsigned char _temp1356[ 19u]="struct_declaration"; static unsigned char
_temp1357[ 25u]="specifier_qualifier_list"; static unsigned char _temp1358[ 23u]="struct_declarator_list";
static unsigned char _temp1359[ 24u]="struct_declarator_list0"; static
unsigned char _temp1360[ 18u]="struct_declarator"; static unsigned char
_temp1361[ 17u]="tunion_specifier"; static unsigned char _temp1362[ 18u]="tunion_or_xtunion";
static unsigned char _temp1363[ 17u]="tunionfield_list"; static unsigned char
_temp1364[ 18u]="tunionfield_scope"; static unsigned char _temp1365[ 12u]="tunionfield";
static unsigned char _temp1366[ 11u]="declarator"; static unsigned char
_temp1367[ 18u]="direct_declarator"; static unsigned char _temp1368[ 8u]="pointer";
static unsigned char _temp1369[ 13u]="pointer_char"; static unsigned char
_temp1370[ 8u]="rgn_opt"; static unsigned char _temp1371[ 4u]="rgn"; static
unsigned char _temp1372[ 20u]="type_qualifier_list"; static unsigned char
_temp1373[ 20u]="parameter_type_list"; static unsigned char _temp1374[ 9u]="type_var";
static unsigned char _temp1375[ 16u]="optional_effect"; static unsigned char
_temp1376[ 19u]="optional_rgn_order"; static unsigned char _temp1377[ 10u]="rgn_order";
static unsigned char _temp1378[ 16u]="optional_inject"; static unsigned char
_temp1379[ 11u]="effect_set"; static unsigned char _temp1380[ 14u]="atomic_effect";
static unsigned char _temp1381[ 11u]="region_set"; static unsigned char
_temp1382[ 15u]="parameter_list"; static unsigned char _temp1383[ 22u]="parameter_declaration";
static unsigned char _temp1384[ 16u]="identifier_list"; static unsigned char
_temp1385[ 17u]="identifier_list0"; static unsigned char _temp1386[ 12u]="initializer";
static unsigned char _temp1387[ 18u]="array_initializer"; static unsigned char
_temp1388[ 17u]="initializer_list"; static unsigned char _temp1389[ 12u]="designation";
static unsigned char _temp1390[ 16u]="designator_list"; static unsigned char
_temp1391[ 11u]="designator"; static unsigned char _temp1392[ 10u]="type_name";
static unsigned char _temp1393[ 14u]="any_type_name"; static unsigned char
_temp1394[ 15u]="type_name_list"; static unsigned char _temp1395[ 20u]="abstract_declarator";
static unsigned char _temp1396[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1397[ 10u]="statement"; static unsigned char _temp1398[ 18u]="labeled_statement";
static unsigned char _temp1399[ 21u]="expression_statement"; static
unsigned char _temp1400[ 19u]="compound_statement"; static unsigned char
_temp1401[ 16u]="block_item_list"; static unsigned char _temp1402[ 20u]="selection_statement";
static unsigned char _temp1403[ 15u]="switch_clauses"; static unsigned char
_temp1404[ 16u]="switchC_clauses"; static unsigned char _temp1405[ 20u]="iteration_statement";
static unsigned char _temp1406[ 15u]="jump_statement"; static unsigned char
_temp1407[ 8u]="pattern"; static unsigned char _temp1408[ 19u]="tuple_pattern_list";
static unsigned char _temp1409[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1410[ 14u]="field_pattern"; static unsigned char _temp1411[ 19u]="field_pattern_list";
static unsigned char _temp1412[ 20u]="field_pattern_list0"; static unsigned char
_temp1413[ 11u]="expression"; static unsigned char _temp1414[ 22u]="assignment_expression";
static unsigned char _temp1415[ 20u]="assignment_operator"; static unsigned char
_temp1416[ 23u]="conditional_expression"; static unsigned char _temp1417[ 20u]="constant_expression";
static unsigned char _temp1418[ 22u]="logical_or_expression"; static
unsigned char _temp1419[ 23u]="logical_and_expression"; static unsigned char
_temp1420[ 24u]="inclusive_or_expression"; static unsigned char _temp1421[ 24u]="exclusive_or_expression";
static unsigned char _temp1422[ 15u]="and_expression"; static unsigned char
_temp1423[ 20u]="equality_expression"; static unsigned char _temp1424[ 22u]="relational_expression";
static unsigned char _temp1425[ 17u]="shift_expression"; static unsigned char
_temp1426[ 20u]="additive_expression"; static unsigned char _temp1427[ 26u]="multiplicative_expression";
static unsigned char _temp1428[ 16u]="cast_expression"; static unsigned char
_temp1429[ 17u]="unary_expression"; static unsigned char _temp1430[ 15u]="unary_operator";
static unsigned char _temp1431[ 19u]="postfix_expression"; static unsigned char
_temp1432[ 19u]="primary_expression"; static unsigned char _temp1433[ 25u]="argument_expression_list";
static unsigned char _temp1434[ 26u]="argument_expression_list0"; static
unsigned char _temp1435[ 9u]="constant"; static unsigned char _temp1436[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 236u]={{ _temp1201, _temp1201, _temp1201
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
_temp1255, _temp1255, _temp1255 +  7u},{ _temp1256, _temp1256, _temp1256 +  8u},{
_temp1257, _temp1257, _temp1257 +  7u},{ _temp1258, _temp1258, _temp1258 +  8u},{
_temp1259, _temp1259, _temp1259 +  9u},{ _temp1260, _temp1260, _temp1260 +  9u},{
_temp1261, _temp1261, _temp1261 +  7u},{ _temp1262, _temp1262, _temp1262 +  5u},{
_temp1263, _temp1263, _temp1263 +  8u},{ _temp1264, _temp1264, _temp1264 +  4u},{
_temp1265, _temp1265, _temp1265 +  7u},{ _temp1266, _temp1266, _temp1266 +  7u},{
_temp1267, _temp1267, _temp1267 +  7u},{ _temp1268, _temp1268, _temp1268 +  8u},{
_temp1269, _temp1269, _temp1269 +  9u},{ _temp1270, _temp1270, _temp1270 +  6u},{
_temp1271, _temp1271, _temp1271 +  6u},{ _temp1272, _temp1272, _temp1272 +  6u},{
_temp1273, _temp1273, _temp1273 +  6u},{ _temp1274, _temp1274, _temp1274 +  7u},{
_temp1275, _temp1275, _temp1275 +  6u},{ _temp1276, _temp1276, _temp1276 +  11u},{
_temp1277, _temp1277, _temp1277 +  11u},{ _temp1278, _temp1278, _temp1278 +  11u},{
_temp1279, _temp1279, _temp1279 +  11u},{ _temp1280, _temp1280, _temp1280 +  11u},{
_temp1281, _temp1281, _temp1281 +  12u},{ _temp1282, _temp1282, _temp1282 +  13u},{
_temp1283, _temp1283, _temp1283 +  11u},{ _temp1284, _temp1284, _temp1284 +  11u},{
_temp1285, _temp1285, _temp1285 +  10u},{ _temp1286, _temp1286, _temp1286 +  9u},{
_temp1287, _temp1287, _temp1287 +  11u},{ _temp1288, _temp1288, _temp1288 +  12u},{
_temp1289, _temp1289, _temp1289 +  11u},{ _temp1290, _temp1290, _temp1290 +  17u},{
_temp1291, _temp1291, _temp1291 +  7u},{ _temp1292, _temp1292, _temp1292 +  19u},{
_temp1293, _temp1293, _temp1293 +  18u},{ _temp1294, _temp1294, _temp1294 +  9u},{
_temp1295, _temp1295, _temp1295 +  16u},{ _temp1296, _temp1296, _temp1296 +  18u},{
_temp1297, _temp1297, _temp1297 +  10u},{ _temp1298, _temp1298, _temp1298 +  4u},{
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
_temp1321, _temp1321, _temp1321 +  4u},{ _temp1322, _temp1322, _temp1322 +  4u},{
_temp1323, _temp1323, _temp1323 +  4u},{ _temp1324, _temp1324, _temp1324 +  4u},{
_temp1325, _temp1325, _temp1325 +  5u},{ _temp1326, _temp1326, _temp1326 +  17u},{
_temp1327, _temp1327, _temp1327 +  21u},{ _temp1328, _temp1328, _temp1328 +  20u},{
_temp1329, _temp1329, _temp1329 +  21u},{ _temp1330, _temp1330, _temp1330 +  13u},{
_temp1331, _temp1331, _temp1331 +  15u},{ _temp1332, _temp1332, _temp1332 +  17u},{
_temp1333, _temp1333, _temp1333 +  19u},{ _temp1334, _temp1334, _temp1334 +  12u},{
_temp1335, _temp1335, _temp1335 +  17u},{ _temp1336, _temp1336, _temp1336 +  23u},{
_temp1337, _temp1337, _temp1337 +  24u},{ _temp1338, _temp1338, _temp1338 +  15u},{
_temp1339, _temp1339, _temp1339 +  11u},{ _temp1340, _temp1340, _temp1340 +  15u},{
_temp1341, _temp1341, _temp1341 +  10u},{ _temp1342, _temp1342, _temp1342 +  15u},{
_temp1343, _temp1343, _temp1343 +  5u},{ _temp1344, _temp1344, _temp1344 +  15u},{
_temp1345, _temp1345, _temp1345 +  15u},{ _temp1346, _temp1346, _temp1346 +  11u},{
_temp1347, _temp1347, _temp1347 +  22u},{ _temp1348, _temp1348, _temp1348 +  26u},{
_temp1349, _temp1349, _temp1349 +  16u},{ _temp1350, _temp1350, _temp1350 +  16u},{
_temp1351, _temp1351, _temp1351 +  24u},{ _temp1352, _temp1352, _temp1352 +  25u},{
_temp1353, _temp1353, _temp1353 +  21u},{ _temp1354, _temp1354, _temp1354 +  22u},{
_temp1355, _temp1355, _temp1355 +  16u},{ _temp1356, _temp1356, _temp1356 +  19u},{
_temp1357, _temp1357, _temp1357 +  25u},{ _temp1358, _temp1358, _temp1358 +  23u},{
_temp1359, _temp1359, _temp1359 +  24u},{ _temp1360, _temp1360, _temp1360 +  18u},{
_temp1361, _temp1361, _temp1361 +  17u},{ _temp1362, _temp1362, _temp1362 +  18u},{
_temp1363, _temp1363, _temp1363 +  17u},{ _temp1364, _temp1364, _temp1364 +  18u},{
_temp1365, _temp1365, _temp1365 +  12u},{ _temp1366, _temp1366, _temp1366 +  11u},{
_temp1367, _temp1367, _temp1367 +  18u},{ _temp1368, _temp1368, _temp1368 +  8u},{
_temp1369, _temp1369, _temp1369 +  13u},{ _temp1370, _temp1370, _temp1370 +  8u},{
_temp1371, _temp1371, _temp1371 +  4u},{ _temp1372, _temp1372, _temp1372 +  20u},{
_temp1373, _temp1373, _temp1373 +  20u},{ _temp1374, _temp1374, _temp1374 +  9u},{
_temp1375, _temp1375, _temp1375 +  16u},{ _temp1376, _temp1376, _temp1376 +  19u},{
_temp1377, _temp1377, _temp1377 +  10u},{ _temp1378, _temp1378, _temp1378 +  16u},{
_temp1379, _temp1379, _temp1379 +  11u},{ _temp1380, _temp1380, _temp1380 +  14u},{
_temp1381, _temp1381, _temp1381 +  11u},{ _temp1382, _temp1382, _temp1382 +  15u},{
_temp1383, _temp1383, _temp1383 +  22u},{ _temp1384, _temp1384, _temp1384 +  16u},{
_temp1385, _temp1385, _temp1385 +  17u},{ _temp1386, _temp1386, _temp1386 +  12u},{
_temp1387, _temp1387, _temp1387 +  18u},{ _temp1388, _temp1388, _temp1388 +  17u},{
_temp1389, _temp1389, _temp1389 +  12u},{ _temp1390, _temp1390, _temp1390 +  16u},{
_temp1391, _temp1391, _temp1391 +  11u},{ _temp1392, _temp1392, _temp1392 +  10u},{
_temp1393, _temp1393, _temp1393 +  14u},{ _temp1394, _temp1394, _temp1394 +  15u},{
_temp1395, _temp1395, _temp1395 +  20u},{ _temp1396, _temp1396, _temp1396 +  27u},{
_temp1397, _temp1397, _temp1397 +  10u},{ _temp1398, _temp1398, _temp1398 +  18u},{
_temp1399, _temp1399, _temp1399 +  21u},{ _temp1400, _temp1400, _temp1400 +  19u},{
_temp1401, _temp1401, _temp1401 +  16u},{ _temp1402, _temp1402, _temp1402 +  20u},{
_temp1403, _temp1403, _temp1403 +  15u},{ _temp1404, _temp1404, _temp1404 +  16u},{
_temp1405, _temp1405, _temp1405 +  20u},{ _temp1406, _temp1406, _temp1406 +  15u},{
_temp1407, _temp1407, _temp1407 +  8u},{ _temp1408, _temp1408, _temp1408 +  19u},{
_temp1409, _temp1409, _temp1409 +  20u},{ _temp1410, _temp1410, _temp1410 +  14u},{
_temp1411, _temp1411, _temp1411 +  19u},{ _temp1412, _temp1412, _temp1412 +  20u},{
_temp1413, _temp1413, _temp1413 +  11u},{ _temp1414, _temp1414, _temp1414 +  22u},{
_temp1415, _temp1415, _temp1415 +  20u},{ _temp1416, _temp1416, _temp1416 +  23u},{
_temp1417, _temp1417, _temp1417 +  20u},{ _temp1418, _temp1418, _temp1418 +  22u},{
_temp1419, _temp1419, _temp1419 +  23u},{ _temp1420, _temp1420, _temp1420 +  24u},{
_temp1421, _temp1421, _temp1421 +  24u},{ _temp1422, _temp1422, _temp1422 +  15u},{
_temp1423, _temp1423, _temp1423 +  20u},{ _temp1424, _temp1424, _temp1424 +  22u},{
_temp1425, _temp1425, _temp1425 +  17u},{ _temp1426, _temp1426, _temp1426 +  20u},{
_temp1427, _temp1427, _temp1427 +  26u},{ _temp1428, _temp1428, _temp1428 +  16u},{
_temp1429, _temp1429, _temp1429 +  17u},{ _temp1430, _temp1430, _temp1430 +  15u},{
_temp1431, _temp1431, _temp1431 +  19u},{ _temp1432, _temp1432, _temp1432 +  19u},{
_temp1433, _temp1433, _temp1433 +  25u},{ _temp1434, _temp1434, _temp1434 +  26u},{
_temp1435, _temp1435, _temp1435 +  9u},{ _temp1436, _temp1436, _temp1436 +  20u}};
static short Cyc_yyr1[ 409u]={ 0, 124, 125, 125, 125, 125, 125, 125, 125, 126,
126, 127, 127, 127, 127, 128, 128, 129, 130, 131, 132, 133, 133, 133, 133, 134,
134, 135, 135, 135, 135, 135, 135, 135, 135, 136, 136, 136, 136, 136, 136, 136,
137, 137, 138, 139, 139, 140, 140, 140, 140, 140, 141, 141, 141, 141, 141, 141,
141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 142, 142, 143, 143,
143, 144, 144, 144, 145, 145, 146, 146, 147, 147, 147, 147, 147, 148, 148, 148,
149, 149, 150, 151, 151, 152, 153, 153, 154, 154, 155, 156, 156, 156, 156, 157,
158, 158, 159, 159, 159, 160, 160, 160, 160, 161, 161, 162, 162, 162, 162, 163,
163, 163, 164, 164, 165, 165, 166, 166, 166, 166, 166, 166, 166, 166, 166, 166,
166, 166, 167, 167, 167, 167, 168, 168, 168, 168, 168, 169, 169, 170, 170, 171,
171, 172, 172, 172, 172, 173, 173, 174, 174, 175, 175, 176, 176, 177, 177, 178,
178, 179, 179, 179, 179, 180, 180, 181, 181, 182, 182, 182, 183, 184, 184, 185,
185, 186, 186, 186, 186, 187, 187, 187, 187, 188, 189, 189, 190, 190, 191, 191,
192, 192, 192, 192, 192, 193, 193, 194, 194, 194, 195, 195, 195, 195, 195, 195,
195, 195, 195, 195, 195, 195, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196,
197, 198, 198, 199, 199, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201,
202, 202, 202, 202, 202, 202, 203, 203, 203, 203, 204, 204, 204, 204, 204, 204,
204, 204, 204, 204, 204, 204, 204, 204, 205, 205, 205, 205, 205, 205, 205, 205,
206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 207, 207,
208, 208, 209, 209, 210, 211, 211, 212, 212, 213, 213, 214, 214, 214, 214, 214,
214, 214, 214, 214, 214, 214, 215, 215, 215, 215, 215, 215, 215, 216, 217, 217,
218, 218, 219, 219, 220, 220, 221, 221, 222, 222, 222, 223, 223, 223, 223, 223,
224, 224, 224, 225, 225, 225, 226, 226, 226, 226, 227, 227, 228, 228, 228, 228,
228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 229, 229, 229, 230,
230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 231, 231, 231,
231, 231, 231, 231, 231, 231, 232, 233, 233, 234, 234, 234, 234, 235, 235};
static short Cyc_yyr2[ 409u]={ 0, 1, 2, 3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4, 3, 4,
2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1, 0, 1,
6, 1, 3, 1, 1, 4, 4, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4, 4, 1,
1, 1, 1, 1, 5, 2, 4, 1, 3, 1, 3, 4, 6, 6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3,
1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3, 6, 4, 3, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2,
6, 1, 2, 1, 3, 3, 4, 4, 5, 4, 4, 4, 2, 2, 1, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0, 1, 1,
1, 1, 2, 3, 5, 5, 7, 1, 3, 0, 2, 0, 2, 3, 5, 0, 1, 1, 3, 2, 3, 4, 1, 1, 3, 1, 3,
2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 4,
3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2,
2, 3, 1, 2, 2, 3, 1, 2, 1, 2, 1, 2, 5, 7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3, 4, 5,
5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1,
1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3,
1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 6,
6, 5, 4, 6, 9, 11, 1, 1, 1, 1, 4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3,
2, 5, 4, 4, 5, 1, 1, 3, 1, 1, 1, 1, 1, 1}; static short Cyc_yydefact[ 822u]={ 0,
139, 35, 36, 37, 38, 40, 52, 54, 55, 56, 57, 58, 59, 60, 61, 72, 73, 74, 90, 91,
42, 0, 0, 41, 0, 0, 115, 116, 0, 0, 407, 159, 408, 87, 0, 53, 0, 144, 145, 148,
1, 0, 9, 0, 0, 10, 0, 42, 42, 42, 62, 63, 0, 64, 0, 0, 126, 0, 149, 65, 128, 39,
0, 33, 43, 0, 76, 288, 407, 284, 287, 286, 0, 282, 0, 0, 0, 0, 0, 182, 0, 289,
17, 19, 0, 0, 0, 0, 66, 0, 0, 0, 0, 2, 0, 0, 0, 0, 21, 0, 95, 96, 98, 27, 29, 31,
87, 0, 87, 152, 0, 151, 87, 38, 0, 25, 0, 0, 11, 161, 0, 0, 137, 127, 42, 150,
138, 0, 0, 34, 80, 0, 78, 0, 0, 296, 295, 285, 294, 24, 0, 0, 0, 0, 0, 42, 42,
200, 202, 0, 0, 70, 71, 160, 207, 0, 129, 0, 0, 177, 0, 0, 406, 0, 0, 0, 0, 0, 0,
0, 0, 0, 87, 0, 0, 403, 393, 404, 405, 0, 0, 0, 0, 376, 0, 374, 375, 0, 307, 320,
328, 330, 332, 334, 336, 338, 341, 346, 349, 352, 356, 0, 358, 377, 392, 391, 0,
3, 0, 5, 0, 22, 0, 0, 0, 12, 28, 30, 32, 86, 0, 92, 93, 0, 85, 87, 0, 113, 39, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 407, 235, 237, 0, 243, 239, 0, 241, 224,
225, 226, 0, 227, 228, 229, 0, 305, 26, 13, 98, 167, 183, 0, 0, 163, 161, 0, 0,
130, 0, 140, 0, 0, 0, 77, 0, 0, 283, 298, 0, 297, 184, 0, 0, 296, 0, 203, 175, 0,
101, 103, 161, 0, 209, 201, 210, 68, 0, 69, 89, 0, 88, 0, 179, 0, 181, 67, 0, 0,
366, 0, 322, 356, 0, 323, 324, 0, 0, 0, 0, 0, 0, 0, 0, 0, 359, 360, 0, 0, 0, 0,
362, 363, 361, 146, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 310,
311, 312, 313, 314, 315, 316, 317, 318, 319, 309, 0, 364, 0, 385, 386, 0, 0, 0,
395, 0, 0, 147, 18, 0, 20, 0, 97, 99, 186, 185, 14, 0, 82, 94, 0, 0, 105, 106,
108, 0, 112, 87, 121, 0, 0, 0, 0, 0, 0, 0, 275, 276, 277, 0, 0, 279, 0, 232, 233,
0, 0, 0, 0, 244, 240, 98, 242, 238, 236, 0, 168, 0, 0, 0, 174, 162, 169, 132, 0,
0, 0, 163, 134, 136, 135, 131, 153, 142, 141, 0, 48, 47, 0, 45, 81, 327, 79, 75,
291, 0, 23, 292, 0, 0, 0, 0, 196, 300, 303, 0, 302, 0, 0, 0, 204, 102, 104, 0,
163, 0, 213, 0, 211, 161, 0, 0, 223, 206, 208, 178, 0, 0, 0, 187, 191, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 394, 401, 0, 400, 329, 0, 331, 333, 335, 337,
339, 340, 344, 345, 342, 343, 347, 348, 350, 351, 353, 354, 355, 308, 383, 384,
379, 0, 381, 382, 0, 0, 0, 4, 6, 0, 109, 100, 0, 0, 0, 114, 123, 122, 0, 0, 117,
0, 0, 0, 0, 0, 0, 0, 0, 274, 278, 0, 0, 0, 231, 0, 234, 0, 15, 306, 161, 0, 171,
0, 0, 0, 164, 133, 167, 155, 154, 143, 7, 0, 0, 0, 299, 199, 0, 301, 195, 197,
293, 0, 290, 205, 176, 218, 0, 212, 215, 0, 163, 0, 214, 0, 365, 0, 0, 188, 0,
192, 389, 390, 0, 370, 0, 0, 0, 0, 0, 0, 0, 0, 357, 397, 0, 0, 380, 378, 398, 0,
0, 84, 107, 110, 83, 111, 124, 121, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 250, 280,
0, 0, 16, 163, 0, 172, 170, 0, 163, 0, 0, 0, 0, 44, 46, 198, 304, 217, 220, 0,
222, 221, 216, 0, 0, 0, 189, 193, 0, 0, 0, 0, 325, 326, 369, 399, 0, 402, 321,
396, 0, 120, 119, 245, 0, 250, 260, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 281, 0,
157, 173, 165, 156, 161, 0, 49, 50, 219, 367, 368, 0, 194, 371, 0, 0, 387, 0, 0,
0, 256, 0, 0, 262, 0, 0, 0, 270, 0, 0, 0, 0, 0, 0, 0, 0, 249, 230, 0, 163, 0, 0,
0, 0, 388, 0, 246, 0, 0, 0, 247, 261, 263, 264, 0, 272, 271, 0, 266, 0, 0, 0, 0,
250, 251, 166, 158, 0, 0, 0, 0, 125, 0, 0, 248, 265, 273, 267, 268, 0, 0, 250,
252, 0, 190, 372, 0, 256, 257, 269, 250, 253, 51, 0, 256, 258, 250, 254, 373,
259, 255, 0, 0, 0}; static short Cyc_yydefgoto[ 112u]={ 819, 41, 42, 43, 248, 44,
382, 45, 384, 46, 215, 47, 48, 64, 65, 451, 452, 49, 154, 50, 51, 131, 132, 52,
89, 53, 221, 222, 100, 101, 102, 223, 148, 394, 395, 396, 54, 55, 552, 553, 554,
56, 57, 58, 59, 125, 111, 447, 476, 60, 477, 438, 580, 430, 434, 435, 292, 269,
160, 79, 80, 493, 387, 494, 495, 464, 465, 149, 155, 156, 478, 299, 251, 252,
253, 254, 255, 256, 715, 767, 257, 258, 282, 283, 284, 467, 468, 469, 259, 260,
369, 189, 455, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202,
203, 204, 510, 511, 205, 206}; static short Cyc_yypact[ 822u]={ 1976, - -32768,
- -32768, - -32768, - -32768, - 67, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - 20, 197, 615, - -32768, 145, - 44, - -32768, -
-32768, - 4, 8, - -32768, 71, - -32768, 76, 235, - -32768, 101, 146, 162, -
-32768, - -32768, 1861, - -32768, 272, 330, - -32768, 551, - 20, - 20, - 20, -
-32768, - -32768, 189, - -32768, 155, 2374, 47, 25, - 75, - -32768, 145, 164,
177, 2433, - -32768, 145, 182, - -32768, 161, - -32768, - -32768, - -32768, 899,
- -32768, 244, - 34, 265, 899, 237, 253, 294, 35, - -32768, - -32768, 4405, 4405,
91, 4405, - -32768, 300, 1446, 3993, 3993, - -32768, 1861, 2090, 1861, 2090, -
-32768, 319, 315, - -32768, 914, 2433, 2433, 2433, 76, 1446, 76, - -32768, 145,
- -32768, 204, 357, 1263, - -32768, 2374, 551, - -32768, 1798, 4405, 2873, -
-32768, 47, - 20, - -32768, - -32768, 2090, 352, - -32768, 367, 356, 373, 145,
376, 899, - -32768, - -32768, - -32768, - -32768, 396, 3993, 282, 386, 213, - 20,
- 20, 141, - -32768, 57, 116, - -32768, - -32768, - -32768, 380, 67, - -32768,
660, 395, - -32768, 4203, 388, - -32768, 3993, 4063, 390, 399, 401, 407, 409,
411, 413, 76, 4273, 4273, - -32768, - -32768, - -32768, - -32768, 1621, 415,
4343, 4343, - -32768, 4343, - -32768, - -32768, 419, - -32768, - 46, 450, 423,
429, 434, 454, - 27, 467, 421, 241, - -32768, 726, 4343, 325, - -32768, - -32768,
52, 455, - -32768, 457, - -32768, 459, - -32768, 235, 2943, 2374, - -32768, -
-32768, - -32768, - -32768, 462, 464, 1446, - -32768, 683, 474, 76, 145, 481, -
-32768, 482, 4, 484, 2479, 487, 477, 495, 501, 3013, 2479, - 23, 2479, 2479, -
33, 496, - -32768, - -32768, 494, 1384, 1384, 551, 1384, - -32768, - -32768, -
-32768, 507, - -32768, - -32768, - -32768, 212, - -32768, - -32768, - -32768,
518, 534, - -32768, 5, 542, 543, 227, 554, 78, - -32768, 555, 93, 559, 0, 145, -
-32768, 3993, 563, - -32768, - -32768, 573, 588, - -32768, 270, 731, 899, 4405,
- -32768, 597, 603, 1446, 1446, 2315, 3083, - 15, - -32768, 180, - -32768, 5, -
-32768, - -32768, 4405, - -32768, 2208, - -32768, 378, - -32768, - -32768, 1446,
1621, - -32768, 1446, - -32768, - -32768, 2549, - -32768, 631, 3993, 2149, 3993,
3993, 3993, 3993, 3993, 607, 1621, - -32768, - -32768, 1384, 609, 441, 3993, -
-32768, - -32768, - -32768, - -32768, 4343, 3993, 4343, 4343, 4343, 4343, 4343,
4343, 4343, 4343, 4343, 4343, 4343, 4343, 4343, 4343, 4343, 4343, 4343, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, - -32768, 3993, - -32768, 113, - -32768, - -32768, 3153, 133, 3993, -
-32768, 2792, 616, - -32768, - -32768, 1861, - -32768, 1861, - -32768, - -32768,
- -32768, - -32768, - -32768, 1446, - -32768, - -32768, 3993, 624, 620, - -32768,
618, 1446, - -32768, 76, 541, 3993, 627, 3993, 3993, 700, 1505, 636, - -32768, -
-32768, - -32768, 302, 692, - -32768, 3223, - -32768, - -32768, 2479, 643, 2479,
1737, - -32768, - -32768, 914, - -32768, - -32768, - -32768, 3993, - -32768,
1446, 637, 245, - -32768, - -32768, 622, - -32768, 647, 639, 4516, 543, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, 93, 1861, - -32768, 641, 642,
640, - -32768, - -32768, - -32768, - -32768, - -32768, 899, - -32768, - -32768,
657, 3993, 899, 218, - -32768, - -32768, - -32768, 648, 652, 649, 74, 666, -
-32768, - -32768, - -32768, 658, 543, 662, - -32768, 654, 180, 4464, 4405, 3293,
- -32768, - -32768, 380, - -32768, 664, 672, 679, - -32768, - -32768, 193, 2943,
466, 674, 235, 475, 676, 677, 678, 479, 1446, 681, 651, 4133, - -32768, - -32768,
684, 682, 450, 263, 423, 429, 434, 454, - 27, - 27, 467, 467, 467, 467, 421, 421,
241, 241, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
685, - -32768, - -32768, 171, 347, 4405, - -32768, - -32768, 689, - -32768, -
-32768, 683, 3993, 691, - -32768, - -32768, - -32768, 699, 145, 304, 498, 3993,
500, 508, 698, 3363, 3433, 314, - -32768, - -32768, 702, 714, 710, - -32768, 706,
- -32768, 2374, - -32768, - -32768, 717, 4405, - -32768, 716, 5, 711, - -32768,
- -32768, 132, - -32768, - -32768, - -32768, - -32768, 372, 719, 0, - -32768, -
-32768, 705, - -32768, - -32768, - -32768, - -32768, 2249, - -32768, - -32768, -
-32768, - -32768, 725, - -32768, - -32768, 729, 543, 81, - -32768, 707, 735, 524,
732, - -32768, 2630, - -32768, - -32768, - -32768, 2374, - -32768, 3993, 814,
3993, 4063, 742, 735, 744, 2792, - -32768, - -32768, 3993, 3993, - -32768, -
-32768, - -32768, 2792, 115, - -32768, - -32768, - -32768, - -32768, - -32768,
54, 26, 541, 2479, 512, 751, 2479, 3993, 3503, 322, 3573, 335, 3643, 594, -
-32768, 754, 783, - -32768, 543, 96, - -32768, - -32768, 779, 543, 1446, 770,
772, 773, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 774, -
-32768, - -32768, - -32768, 775, 776, 3993, - -32768, - -32768, 2943, 531, 778,
781, - -32768, 631, - -32768, - -32768, 353, - -32768, - -32768, - -32768, 780,
- -32768, - -32768, 858, 790, 594, - -32768, 539, 2479, 547, 3713, 2479, 566,
3783, 3853, 348, 899, 782, 792, - -32768, 2479, - -32768, - -32768, 786, -
-32768, 717, 803, - -32768, - -32768, - -32768, - -32768, - -32768, 289, -
-32768, - -32768, 1446, 868, - -32768, 2711, 1446, 2479, 604, 795, 798, - -32768,
2479, 2479, 568, - -32768, 2479, 2479, 570, 2479, 578, 3923, 19, 1384, - -32768,
- -32768, 647, 543, 793, 3993, 796, 800, - -32768, 580, - -32768, 3993, 789, 804,
- -32768, - -32768, - -32768, - -32768, 2479, - -32768, - -32768, 2479, - -32768,
2479, 2479, 583, 3993, 1018, - -32768, - -32768, - -32768, 810, 355, 802, 1446,
- -32768, 306, 1384, - -32768, - -32768, - -32768, - -32768, - -32768, 2479, 340,
594, - -32768, 805, - -32768, - -32768, 806, 1142, - -32768, - -32768, 1018, -
-32768, - -32768, 807, 604, - -32768, 594, - -32768, - -32768, - -32768, -
-32768, 902, 905, - -32768}; static short Cyc_yypgoto[ 112u]={ - -32768, 119, -
-32768, 589, - -32768, - -32768, - -32768, - -32768, - -32768, - 52, - 36, - 56,
- -32768, 374, - 42, 323, - -32768, 92, - -32768, 46, - -32768, - -32768, - 115,
- -32768, - 22, - -32768, - 101, - -32768, - -32768, - -32768, 701, 694, 10, -
-32768, - -32768, 365, - -32768, - -32768, 50, - -32768, - -32768, 109, - 45, -
65, - -32768, - -32768, 854, - -32768, - 106, 45, - 117, - 415, 183, 354, 363,
644, - 391, - 90, - 236, 822, - -32768, - 202, - 160, - 354, - 271, - -32768,
483, - 141, - 79, - 99, - 113, - 180, - 118, - -32768, - -32768, - 47, - 211, -
-32768, - 562, - 511, - -32768, - -32768, - 21, 661, - -32768, 349, - -32768, -
-32768, 233, 215, - -32768, - 137, - 372, - 155, 613, 614, 606, 611, 612, 344,
157, 362, 364, - 86, 513, - -32768, - -32768, - -32768, - 344, - -32768, -
-32768, 11}; static short Cyc_yytable[ 4622u]={ 118, 159, 81, 268, 116, 318, 150,
151, 130, 119, 319, 61, 386, 124, 267, 123, 463, 449, 32, 280, 117, 544, 271, 62,
539, 583, 1, 315, 339, 110, 535, 550, 551, 67, 82, 298, 83, 422, 423, 332, 425,
577, 347, 348, 84, 309, 61, 118, 217, 218, 219, 116, 135, 61, 137, 418, 216, 139,
61, 250, 143, 118, 602, 249, 109, 261, 113, 431, 340, 61, 262, 567, 127, 419,
414, 488, 63, 133, 415, 349, 350, 600, 123, 297, 82, 220, 295, 225, 450, 82, 592,
228, 780, 308, 403, 296, 335, 336, 32, 337, 112, 158, 85, 432, 112, 404, 61, 61,
61, 61, 16, 17, 18, 31, 86, 406, 370, 481, 224, 33, 506, 413, 226, 416, 417, -
118, 35, 781, 481, 61, 158, 147, 147, - 87, 147, 303, - 87, 147, 377, 61, 739,
88, 454, 63, 90, 133, 442, 82, 120, 677, 378, 327, 440, 121, 147, - 87, 103, 122,
87, 118, 88, 94, 453, 261, 300, 379, 147, 147, 389, 61, 304, 489, 301, 490, 305,
639, 599, 146, 146, 152, 146, 304, 88, 146, 304, 443, 153, 505, 678, 301, 291,
676, 250, 250, 574, 250, 249, 249, 719, 249, 146, 532, 91, 488, 399, 38, 39, 40,
533, 446, 471, 301, 146, 146, 208, 209, 210, 211, 304, 800, 429, 536, 696, 302,
61, 487, 147, 263, 537, 266, 297, 301, 224, 31, - 161, 61, 1, 809, 400, 33, -
161, 308, 295, 31, 92, 718, 815, 275, 32, 33, 721, 296, 818, 38, 39, 40, 454,
485, - 183, 110, 93, 61, 128, 124, - 183, 498, 466, 307, 147, 528, 529, 530, 146,
693, 428, 250, 63, 31, 129, 249, 134, 482, 633, 33, 107, 31, 483, 108, 133, 543,
484, 33, 613, 615, 813, 66, 614, 548, 82, 82, 568, 817, 570, 474, 475, 158, 32,
188, 207, 427, 88, 433, 290, 227, 146, 428, 158, 61, 594, 61, 445, 158, 263, 31,
266, 454, 463, 461, 462, 33, 439, 159, 61, 397, 140, 147, 35, 273, 32, 147, 147,
147, 784, 685, 576, 136, 433, 38, 39, 40, 147, 118, 147, 355, 138, 561, 141, 147,
147, 424, 147, 356, 357, 624, 685, 606, 428, 459, 118, 95, 96, 631, 116, 428,
147, 286, 605, 572, 549, 1, 287, 146, 585, 288, 607, 146, 146, 146, 571, 371,
372, 373, 428, 61, 142, 61, 146, 759, 146, 564, 224, 643, 157, 146, 146, 428,
146, 644, 224, 428, 454, 654, 684, 333, 805, 90, 212, 428, 213, 707, 146, 628,
104, 105, 106, 428, 374, 97, 98, 388, 722, 158, 710, 684, 375, 376, 147, 590,
428, 485, 158, 636, 593, 428, 147, 751, 634, 229, 808, 158, 635, 428, 734, 276,
802, 278, 735, 660, 428, 61, 667, 668, 669, 689, 685, 665, 31, 147, 690, 82, 277,
412, 33, 279, 82, - 209, 147, 291, 281, 306, - 209, - 209, 146, 730, 285, 147, -
209, 289, 296, 314, 146, 320, 158, 584, 695, 301, 661, 310, 311, 274, 321, 541,
322, 542, 520, 521, 522, 523, 323, 61, 324, 480, 325, 146, 326, 118, 334, 291,
338, 261, 293, 294, 146, 341, 659, 345, 346, 700, 147, 147, 703, 146, 388, 684,
351, 352, 353, 354, 500, 501, 502, 342, 782, 508, 428, 333, 550, 551, 343, 509,
147, 344, 1, 496, 380, 499, 381, 61, 383, 503, 390, 333, 118, 391, 642, 408, 116,
586, 616, 428, 799, 216, 398, 513, 146, 146, 466, 619, 428, 401, 806, 623, 428,
402, 531, 405, 147, 741, 407, 509, 745, 760, 409, 388, 812, 421, 146, 814, 410,
755, 645, 428, 647, 428, 420, 757, 426, 618, 82, 538, 648, 428, 680, 681, 701,
428, 713, 714, 214, 764, 697, 147, 429, 433, 770, 771, 765, 766, 773, 774, 509,
776, 146, 731, 428, 555, 509, 557, 558, 31, 562, 740, 428, 573, 436, 33, 763,
804, 99, 742, 428, 437, 35, 68, 793, 397, 441, 794, 448, 795, 796, 1, 456, 38,
39, 40, 444, 146, 746, 428, 772, 428, 775, 428, 313, 457, 158, 316, 316, 807,
777, 428, 789, 311, 1, 797, 428, 329, 330, 518, 519, 458, 752, 698, 699, 316,
316, 250, 316, 609, 472, 249, 473, 69, 70, 339, 71, 72, 504, 33, 388, 507, 147,
524, 525, 316, 73, 526, 527, 74, 75, 545, 540, 546, 82, 250, 547, 76, 556, 249,
559, 77, 78, 563, 565, 250, 569, 578, 575, 249, 579, 581, 587, 589, 588, 591,
158, 596, 31, 250, 626, 598, 250, 249, 33, 597, 249, - 180, 146, 32, 601, 306, -
180, - 180, 603, 604, 610, 612, - 180, 68, 296, 31, 38, 39, 40, 611, 617, 33,
147, 620, 621, 622, 147, 625, 35, 630, 629, 632, 637, 646, 640, 393, 316, 651,
653, 38, 39, 40, 641, 649, 655, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367,
656, 657, 658, 266, 662, 672, 664, 679, 31, 70, 670, 71, 72, 146, 33, 368, 674,
146, 388, 460, 675, 73, 627, 147, 74, 75, 688, 682, 687, 461, 462, 388, 76, 691,
694, 692, 77, 78, 702, 388, 716, 316, 686, 316, 316, 316, 316, 316, 316, 316,
316, 316, 316, 316, 316, 316, 316, 316, 316, 316, 717, 720, 723, 724, 725, 726,
727, 728, 732, 146, 736, 704, 706, 733, 709, 737, 712, 738, 756, 753, 754, 758,
761, 768, 769, 785, 791, 787, 801, 388, 788, 820, 792, 803, 821, 316, 810, 811,
816, 497, 638, 671, 126, 385, 729, 392, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 666, 68, 21, 783, 744, 663, 270, 748, 750, 486, 673,
595, 515, 470, 388, 22, 512, 23, 516, 514, 517, 0, 24, 0, 0, 0, 27, 28, 0, 0, 0,
0, 0, 0, 0, 0, 29, 30, 0, 316, 0, 0, 0, 0, 0, 0, 0, 0, 779, 0, 0, 31, 70, 0, 71,
72, 786, 33, 0, 0, 0, 0, 790, 0, 73, 0, 0, 74, 75, 0, 0, 32, 0, 34, 0, 76, 115,
798, 214, 77, 78, 0, 36, 37, 316, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 713, 714, 21, 161, 162, 230, 0, 231, 232, 233, 234, 235,
236, 237, 238, 22, 163, 23, 164, 239, 316, 165, 24, 240, 0, 0, 27, 28, 166, 167,
241, 242, 168, 169, 170, 171, 29, 30, 243, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 244, 176, 177, 178, 179, 32,
33, 34, 0, 245, 115, 0, 0, 180, 0, 0, 36, 247, 0, 0, 0, 0, 0, 0, 182, 0, 0, 183,
184, 185, 316, 0, 0, 0, 186, 187, 0, 0, 316, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 765, 766, 21, 161, 162, 230, 0, 231, 232,
233, 234, 235, 236, 237, 238, 22, 163, 23, 164, 239, 0, 165, 24, 240, 0, 0, 27,
28, 166, 167, 241, 242, 168, 169, 170, 171, 29, 30, 243, 172, 0, 173, 0, 174,
175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 244, 176,
177, 178, 179, 32, 33, 34, 0, 245, 115, 0, 0, 180, 0, 0, 36, 247, 0, 0, 0, 0, 0,
0, 182, 0, 0, 183, 184, 185, 0, 0, 0, 0, 186, 187, 2, 3, 4, 114, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 161, 162, 230, 0, 231, 232,
233, 234, 235, 236, 237, 238, 22, 163, 23, 164, 239, 0, 165, 24, 240, 0, 0, 27,
28, 166, 167, 241, 242, 168, 169, 170, 171, 29, 30, 243, 172, 0, 173, 0, 174,
175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 244, 176,
177, 178, 179, 32, 33, 34, 0, 245, 115, 246, 0, 180, 0, 0, 36, 247, 0, 0, 0, 0,
0, 0, 182, 0, 0, 183, 184, 185, 0, 0, 0, 0, 186, 187, 2, 3, 4, 114, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 161, 162, 230, 0, 231, 232,
233, 234, 235, 236, 237, 238, 22, 163, 23, 164, 239, 0, 165, 24, 240, 0, 0, 27,
28, 166, 167, 241, 242, 168, 169, 170, 171, 29, 30, 243, 172, 0, 173, 0, 174,
175, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0,
244, 176, 177, 178, 179, 32, 33, 34, 0, 245, 115, 22, 0, 180, 0, 0, 36, 247, 0,
0, 0, 0, 27, 28, 182, 0, 0, 183, 184, 185, 0, 0, 29, 30, 186, 187, 2, 3, 4, 114,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 161, 162, 0, 0,
0, 0, 0, 0, 0, 32, 0, 34, 22, 163, 23, 164, 0, 0, 165, 24, 36, 37, 0, 27, 28,
166, 167, 0, 0, 168, 169, 170, 171, 29, 30, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 32,
33, 34, 0, 560, 0, 0, 0, 180, 0, 0, 36, 247, 0, 0, 0, 0, 0, 0, 182, 0, 0, 183,
184, 185, 0, 0, 0, 0, 186, 187, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 0, 161, 162, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 163, 0, 164, 0, 0, 165,
0, 0, 0, 0, 27, 28, 166, 167, 0, 0, 168, 169, 170, 171, 29, 30, 0, 172, 0, 173,
0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31,
176, 177, 178, 179, 32, 33, 34, 0, 0, 331, 0, 0, 180, 0, 0, 36, 247, 0, 0, 0, 0,
0, 0, 182, 0, 0, 183, 184, 185, 0, 0, 0, 0, 186, 187, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 161, 162, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
163, 0, 164, 0, 0, 165, 0, 0, 0, 0, 27, 28, 166, 167, 0, 0, 168, 169, 170, 171,
29, 30, 0, 172, 0, 173, 0, 174, 175, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 32, 33, 34, 0, 0, 22, 0,
0, 180, 0, 0, 36, 247, 0, 0, 0, 27, 28, 0, 182, 0, 0, 183, 184, 185, 0, 29, 30,
0, 186, 187, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 264, 0, 21, 265, 0, 0, 0, 0, 32, 0, 34, 0, 266, 0, 0, 22, 0, 23, 0,
36, 37, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0,
0, 32, 33, 34, 0, 0, 0, - 8, 0, 35, 0, 0, 36, 37, 0, 0, 0, 0, 0, 0, 38, 39, 40,
- 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26,
27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 32, 33, 34, 0, 0, 0, 0,
0, 35, 0, 0, 36, 37, 0, 0, 0, 0, 0, 0, 38, 39, 40, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0,
29, 30, 1, 0, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 21, 0, 0, 0, 0, 31, 0, 0, 0, 0, 32, 33, 34, 22, 0, 0, - 8, 0, 35, 0,
24, 36, 37, 0, 27, 28, 0, 0, 0, 38, 39, 40, 0, 0, 29, 30, 1, 0, 0, 0, 0, 0, 0, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0,
0, 0, 32, 33, 34, 22, 0, 0, 0, 0, 35, 0, 0, 36, 37, 0, 27, 28, 0, 0, 0, 38, 39,
40, 0, 0, 29, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 0,
0, 0, 0, 0, 264, 0, 0, 31, 0, 0, 0, 0, 32, 33, 34, 0, 266, 0, 0, 0, 306, - 161,
0, 36, 37, 0, 0, - 161, 0, 296, 0, 38, 39, 40, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 31, 70, 0, 71, 72, 0, 33, 0, 0, 0, 0, 0, 0, 73, 0, 22, 74,
75, 0, 0, 0, 461, 462, 0, 76, 0, 27, 28, 77, 78, 0, 0, 0, 0, 0, 0, 29, 30, 0, 0,
2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21,
0, 264, 0, 0, 0, 0, 0, 0, 0, 32, 0, 34, 22, 266, 23, 0, 0, 295, 0, 24, 36, 37, 0,
27, 28, 0, 296, 0, 38, 39, 40, 0, 0, 29, 30, 0, 0, 2, 3, 4, 114, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32,
0, 34, 22, 0, 115, 0, 0, 0, 0, 24, 36, 37, 0, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 29,
30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 161, 162, 230, 0, 231, 232, 233, 234, 235,
236, 237, 238, 0, 163, 0, 164, 239, 0, 165, 0, 240, 0, 32, 0, 34, 166, 167, 241,
242, 168, 169, 170, 171, 36, 37, 243, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 244, 176, 177, 178, 179, 0, 33, 161,
162, 245, 115, 0, 0, 180, 491, 0, 0, 181, 0, 0, 163, 0, 164, 0, 182, 165, 0, 183,
184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0,
174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176,
177, 178, 179, 0, 33, 0, 0, 0, 317, 492, 0, 180, 0, 0, 0, 181, 161, 162, 0, 461,
462, 0, 182, 0, 0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0,
0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179,
0, 33, 0, 0, 0, 317, 683, 0, 180, 0, 0, 0, 181, 161, 162, 0, 461, 462, 0, 182, 0,
0, 183, 184, 185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 166, 167, 0,
0, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0,
317, 762, 0, 180, 0, 0, 0, 181, 161, 162, 0, 461, 462, 0, 182, 0, 0, 183, 184,
185, 0, 163, 0, 164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169,
170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 0, 0, 0, 317, 0, 0,
180, 0, 0, 0, 181, 161, 162, 0, 461, 462, 0, 182, 0, 0, 183, 184, 185, 0, 163, 0,
164, 186, 187, 165, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 169, 170, 171, 0, 0,
0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161, 162, 0, 0, 0, 0, 180, 0, 0, 0,
181, 0, 0, 163, 0, 164, 272, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186,
187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161,
162, 0, 317, 0, 0, 180, 0, 0, 0, 181, 0, 0, 163, 0, 164, 0, 182, 165, 0, 183,
184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0,
174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176,
177, 178, 179, 0, 33, 161, 162, 411, 0, 0, 0, 180, 0, 0, 0, 181, 0, 0, 163, 0,
164, 0, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171,
0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161, 162, 0, 0, 0, 0, 180, 0, 0,
0, 181, 0, 0, 163, 0, 164, 479, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186,
187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161,
162, 0, 0, 0, 0, 180, 534, 0, 0, 181, 0, 0, 163, 0, 164, 0, 182, 165, 0, 183,
184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0,
174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176,
177, 178, 179, 0, 33, 161, 162, 0, 0, 0, 0, 180, 566, 0, 0, 181, 0, 0, 163, 0,
164, 0, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171,
0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161, 162, 0, 0, 0, 0, 180, 0, 0,
0, 181, 0, 0, 163, 0, 164, 608, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186,
187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161,
162, 650, 0, 0, 0, 180, 0, 0, 0, 181, 0, 0, 163, 0, 164, 0, 182, 165, 0, 183,
184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0,
174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176,
177, 178, 179, 0, 33, 161, 162, 652, 0, 0, 0, 180, 0, 0, 0, 181, 0, 0, 163, 0,
164, 0, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171,
0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161, 162, 0, 0, 0, 0, 180, 705,
0, 0, 181, 0, 0, 163, 0, 164, 0, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186,
187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161,
162, 0, 0, 0, 0, 180, 708, 0, 0, 181, 0, 0, 163, 0, 164, 0, 182, 165, 0, 183,
184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0,
174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176,
177, 178, 179, 0, 33, 161, 162, 711, 0, 0, 0, 180, 0, 0, 0, 181, 0, 0, 163, 0,
164, 0, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171,
0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161, 162, 0, 0, 0, 0, 180, 743,
0, 0, 181, 0, 0, 163, 0, 164, 0, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186,
187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161,
162, 0, 0, 0, 0, 180, 747, 0, 0, 181, 0, 0, 163, 0, 164, 0, 182, 165, 0, 183,
184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0,
174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176,
177, 178, 179, 0, 33, 161, 162, 0, 0, 0, 0, 180, 749, 0, 0, 181, 0, 0, 163, 0,
164, 0, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171,
0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161, 162, 0, 0, 0, 0, 180, 778,
0, 0, 181, 0, 0, 163, 0, 164, 0, 182, 165, 0, 183, 184, 185, 0, 0, 166, 167, 186,
187, 168, 169, 170, 171, 0, 0, 0, 172, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161,
162, 0, 0, 0, 0, 180, 0, 0, 0, 181, 0, 0, 163, 0, 0, 0, 182, 0, 0, 183, 184, 185,
0, 0, 166, 167, 186, 187, 168, 169, 170, 171, 0, 0, 0, 0, 0, 173, 0, 174, 175, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178,
179, 0, 33, 161, 162, 0, 317, 0, 0, 180, 0, 0, 0, 181, 0, 0, 163, 0, 0, 0, 182,
0, 0, 183, 184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170, 171, 0, 0, 0, 0, 0,
173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
31, 176, 177, 178, 179, 0, 33, 161, 162, 0, 627, 0, 0, 180, 0, 0, 0, 181, 0, 0,
163, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 0, 166, 167, 186, 187, 168, 169, 170,
171, 0, 0, 0, 0, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161, 162, 0, 0, 0, 0, 312, 0,
0, 0, 181, 0, 0, 163, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0, 0, 166, 167, 186,
187, 168, 169, 170, 171, 0, 0, 0, 0, 0, 173, 0, 174, 175, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 176, 177, 178, 179, 0, 33, 161, 162,
0, 0, 0, 0, 328, 0, 0, 0, 181, 0, 0, 163, 0, 0, 0, 182, 0, 0, 183, 184, 185, 0,
0, 166, 167, 186, 187, 168, 169, 170, 171, 0, 0, 0, 0, 0, 173, 0, 174, 175, 0, 0,
0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 31, 176, 177,
178, 179, 0, 33, 0, 0, 0, 0, 22, 0, 180, 0, 0, 0, 181, 0, 0, 0, 0, 27, 28, 182,
0, 0, 183, 184, 185, 0, 0, 29, 30, 186, 187, 144, 0, 0, 0, 0, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0,
34, 22, 0, 145, 0, 0, 0, 0, 0, 36, 37, 0, 27, 28, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 264, 0, 0, 0, 22, 0, 0, 0, 32, 0, 34, 0, 266, 0, 0, 27, 28, 0, 0, 36, 37,
0, 0, 0, 0, 29, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 582, 0, 0, 0, 0, 0, 0, 0, 32, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 36,
37}; static short Cyc_yycheck[ 4622u]={ 56, 91, 23, 120, 56, 165, 85, 86, 64, 56,
165, 0, 214, 58, 120, 57, 287, 17, 93, 134, 56, 393, 121, 90, 378, 440, 1, 164,
74, 104, 374, 5, 6, 22, 23, 148, 25, 248, 249, 180, 251, 432, 69, 70, 88, 158,
35, 103, 104, 105, 106, 103, 73, 42, 88, 88, 103, 78, 47, 115, 82, 117, 477, 115,
53, 117, 55, 62, 114, 58, 117, 415, 61, 106, 97, 311, 96, 66, 101, 106, 107, 472,
124, 148, 73, 107, 101, 109, 88, 78, 462, 113, 73, 158, 90, 110, 182, 183, 93,
185, 55, 91, 106, 98, 59, 101, 95, 96, 97, 98, 17, 18, 19, 88, 106, 233, 202,
297, 108, 94, 331, 239, 111, 241, 242, 99, 101, 108, 308, 118, 120, 85, 86, 98,
88, 68, 101, 91, 86, 128, 702, 106, 279, 96, 35, 134, 68, 136, 101, 68, 98, 173,
269, 106, 108, 101, 47, 110, 87, 215, 106, 42, 277, 215, 107, 113, 120, 121, 215,
158, 103, 312, 115, 314, 107, 547, 102, 85, 86, 88, 88, 103, 106, 91, 103, 107,
95, 328, 107, 115, 145, 606, 248, 249, 430, 251, 248, 249, 102, 251, 108, 88,
101, 439, 226, 112, 113, 114, 95, 274, 289, 115, 120, 121, 95, 96, 97, 98, 103,
781, 88, 88, 107, 107, 213, 304, 180, 118, 95, 97, 295, 115, 222, 88, 102, 224,
1, 799, 227, 94, 108, 306, 101, 88, 98, 660, 808, 128, 93, 94, 665, 110, 814,
112, 113, 114, 393, 299, 97, 104, 98, 250, 98, 308, 103, 321, 287, 158, 222, 355,
356, 357, 180, 627, 103, 331, 96, 88, 101, 331, 98, 101, 111, 94, 95, 88, 106,
98, 277, 390, 110, 94, 99, 495, 805, 98, 103, 398, 287, 288, 418, 812, 420, 293,
294, 295, 93, 92, 93, 97, 106, 266, 99, 109, 222, 103, 306, 306, 100, 308, 274,
311, 213, 88, 97, 462, 597, 109, 110, 94, 103, 421, 321, 224, 97, 289, 101, 122,
93, 293, 294, 295, 757, 614, 99, 101, 301, 112, 113, 114, 304, 407, 306, 112, 89,
407, 103, 311, 312, 250, 314, 120, 121, 504, 635, 482, 103, 97, 424, 97, 98, 108,
424, 103, 328, 142, 482, 424, 400, 1, 98, 289, 447, 101, 483, 293, 294, 295, 424,
64, 65, 66, 103, 382, 100, 384, 304, 108, 306, 97, 390, 97, 102, 311, 312, 103,
314, 103, 398, 103, 547, 97, 614, 180, 108, 306, 97, 103, 103, 97, 328, 507, 48,
49, 50, 103, 101, 97, 98, 214, 666, 421, 97, 635, 109, 110, 390, 458, 103, 481,
430, 540, 463, 103, 398, 97, 99, 90, 108, 439, 103, 103, 99, 101, 99, 99, 103,
574, 103, 448, 88, 89, 90, 623, 735, 582, 88, 421, 623, 458, 103, 238, 94, 100,
463, 97, 430, 432, 102, 101, 102, 103, 390, 685, 88, 439, 108, 101, 110, 101,
398, 101, 482, 447, 631, 115, 575, 102, 103, 125, 101, 382, 101, 384, 347, 348,
349, 350, 101, 498, 101, 296, 101, 421, 101, 571, 101, 472, 99, 571, 146, 147,
430, 73, 571, 71, 72, 645, 482, 483, 648, 439, 317, 735, 67, 68, 115, 116, 323,
324, 325, 118, 753, 102, 103, 312, 5, 6, 119, 334, 504, 117, 1, 320, 99, 322, 99,
546, 99, 326, 98, 328, 618, 99, 553, 88, 618, 448, 102, 103, 781, 618, 98, 340,
482, 483, 597, 102, 103, 98, 791, 102, 103, 101, 369, 101, 540, 705, 101, 374,
708, 732, 97, 378, 805, 101, 504, 808, 97, 717, 102, 103, 102, 103, 108, 722, 99,
498, 597, 376, 102, 103, 88, 89, 102, 103, 22, 23, 100, 737, 642, 575, 88, 578,
742, 743, 22, 23, 746, 747, 415, 749, 540, 102, 103, 402, 421, 404, 405, 88, 407,
102, 103, 428, 102, 94, 736, 788, 97, 102, 103, 108, 101, 38, 772, 546, 102, 775,
99, 777, 778, 1, 99, 112, 113, 114, 111, 575, 102, 103, 102, 103, 102, 103, 161,
102, 666, 164, 165, 797, 102, 103, 102, 103, 1, 102, 103, 174, 175, 345, 346,
103, 713, 643, 644, 182, 183, 753, 185, 484, 103, 753, 99, 88, 89, 74, 91, 92,
101, 94, 495, 102, 666, 351, 352, 202, 101, 353, 354, 104, 105, 97, 106, 103,
713, 781, 108, 112, 101, 781, 30, 116, 117, 97, 42, 791, 93, 115, 101, 791, 93,
102, 101, 103, 102, 88, 736, 99, 88, 805, 99, 102, 808, 805, 94, 103, 808, 97,
666, 93, 102, 101, 102, 103, 102, 111, 102, 88, 108, 38, 110, 88, 112, 113, 114,
103, 102, 94, 732, 103, 103, 103, 736, 102, 101, 103, 102, 102, 99, 556, 99, 108,
279, 560, 561, 112, 113, 114, 99, 101, 98, 75, 76, 77, 78, 79, 80, 81, 82, 83,
84, 97, 102, 107, 97, 99, 111, 106, 111, 88, 89, 102, 91, 92, 732, 94, 100, 102,
736, 614, 99, 102, 101, 98, 788, 104, 105, 622, 106, 25, 109, 110, 627, 112, 102,
630, 102, 116, 117, 98, 635, 97, 339, 620, 341, 342, 343, 344, 345, 346, 347,
348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 88, 93, 103, 102, 102, 102,
102, 102, 101, 788, 101, 649, 650, 103, 652, 28, 654, 98, 103, 108, 99, 89, 25,
99, 97, 103, 108, 102, 89, 685, 101, 0, 99, 102, 0, 393, 102, 102, 102, 321, 546,
589, 59, 213, 682, 222, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, 582, 38, 24, 756, 707, 578, 120, 710, 711, 301, 597, 464, 342, 288,
735, 37, 339, 39, 343, 341, 344, - 1, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 58, 59, - 1, 462, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 751, - 1, - 1, 88, 89, - 1, 91, 92, 759, 94, - 1, - 1, - 1, - 1, 765, - 1,
101, - 1, - 1, 104, 105, - 1, - 1, 93, - 1, 95, - 1, 112, 98, 780, 100, 116, 117,
- 1, 104, 105, 507, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
40, 41, 547, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
59, 60, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93,
94, 95, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1,
- 1, 112, - 1, - 1, 115, 116, 117, 623, - 1, - 1, - 1, 122, 123, - 1, - 1, 631,
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44,
45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, - 1, 63, -
1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1, 97, 98,
- 1, - 1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1,
115, 116, 117, - 1, - 1, - 1, - 1, 122, 123, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32,
33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51,
52, 53, 54, 55, 56, 57, 58, 59, 60, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1, 97, 98, 99, - 1, 101, - 1, - 1, 104,
105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1,
- 1, 122, 123, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
59, 60, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1,
97, 98, 37, - 1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, 48, 49, 112, - 1,
- 1, 115, 116, 117, - 1, - 1, 58, 59, 122, 123, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 93, - 1, 95, 37, 38, 39, 40, - 1, - 1, 43, 44, 104, 105, - 1, 48,
49, 50, 51, - 1, - 1, 54, 55, 56, 57, 58, 59, - 1, 61, - 1, 63, - 1, 65, 66, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1, 97, - 1, - 1, - 1, 101,
- 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117,
- 1, - 1, - 1, - 1, 122, 123, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37,
38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55,
56, 57, 58, 59, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90,
91, 92, 93, 94, 95, - 1, - 1, 98, - 1, - 1, 101, - 1, - 1, 104, 105, - 1, - 1, -
1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1, 122, 123, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, -
1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, 56, 57, 58, 59, - 1, 61, - 1, 63,
- 1, 65, 66, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1,
- 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1, - 1, 37, - 1, - 1, 101,
- 1, - 1, 104, 105, - 1, - 1, - 1, 48, 49, - 1, 112, - 1, - 1, 115, 116, 117, -
1, 58, 59, - 1, 122, 123, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, 85, - 1, 24, 88, - 1, - 1, - 1, - 1, 93, - 1, 95, - 1,
97, - 1, - 1, 37, - 1, 39, - 1, 104, 105, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 58, 59, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 88, - 1, - 1, - 1, - 1, 93, 94, 95, - 1, - 1, - 1, 99, - 1,
101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, 113, 114, 0, 1, - 1,
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39, - 1,
- 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
58, 59, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, - 1, - 1,
- 1, - 1, 93, 94, 95, - 1, - 1, - 1, - 1, - 1, 101, - 1, - 1, 104, 105, - 1, - 1,
- 1, - 1, - 1, - 1, 112, 113, 114, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48,
49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 58, 59, 1, - 1, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1,
88, - 1, - 1, - 1, - 1, 93, 94, 95, 37, - 1, - 1, 99, - 1, 101, - 1, 44, 104,
105, - 1, 48, 49, - 1, - 1, - 1, 112, 113, 114, - 1, - 1, 58, 59, 1, - 1, - 1, -
1, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 88, - 1, - 1, - 1, - 1, 93, 94, 95, 37, - 1, - 1, - 1,
- 1, 101, - 1, - 1, 104, 105, - 1, 48, 49, - 1, - 1, - 1, 112, 113, 114, - 1, -
1, 58, 59, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, 88, - 1, - 1,
- 1, - 1, 93, 94, 95, - 1, 97, - 1, - 1, - 1, 101, 102, - 1, 104, 105, - 1, - 1,
108, - 1, 110, - 1, 112, 113, 114, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, 88, 89, - 1, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, - 1, 37,
104, 105, - 1, - 1, - 1, 109, 110, - 1, 112, - 1, 48, 49, 116, 117, - 1, - 1, -
1, - 1, - 1, - 1, 58, 59, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, 85, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
93, - 1, 95, 37, 97, 39, - 1, - 1, 101, - 1, 44, 104, 105, - 1, 48, 49, - 1, 110,
- 1, 112, 113, 114, - 1, - 1, 58, 59, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 93, - 1, 95, 37, - 1, 98, - 1, - 1, - 1, - 1, 44, 104, 105, - 1,
48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 58, 59, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, -
1, 38, - 1, 40, 41, - 1, 43, - 1, 45, - 1, 93, - 1, 95, 50, 51, 52, 53, 54, 55,
56, 57, 104, 105, 60, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89,
90, 91, 92, - 1, 94, 25, 26, 97, 98, - 1, - 1, 101, 32, - 1, - 1, 105, - 1, - 1,
38, - 1, 40, - 1, 112, 43, - 1, 115, 116, 117, - 1, - 1, 50, 51, 122, 123, 54,
55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, 99, - 1, 101, - 1, - 1, - 1, 105, 25,
26, - 1, 109, 110, - 1, 112, - 1, - 1, 115, 116, 117, - 1, 38, - 1, 40, 122, 123,
43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, 55, 56, 57, - 1, - 1, -
1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94,
- 1, - 1, - 1, 98, 99, - 1, 101, - 1, - 1, - 1, 105, 25, 26, - 1, 109, 110, - 1,
112, - 1, - 1, 115, 116, 117, - 1, 38, - 1, 40, 122, 123, 43, - 1, - 1, - 1, - 1,
- 1, - 1, 50, 51, - 1, - 1, 54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65,
66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, 99,
- 1, 101, - 1, - 1, - 1, 105, 25, 26, - 1, 109, 110, - 1, 112, - 1, - 1, 115,
116, 117, - 1, 38, - 1, 40, 122, 123, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51,
- 1, - 1, 54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, - 1, - 1, 101, - 1, -
1, - 1, 105, 25, 26, - 1, 109, 110, - 1, 112, - 1, - 1, 115, 116, 117, - 1, 38,
- 1, 40, 122, 123, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, 55,
56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89,
90, 91, 92, - 1, 94, 25, 26, - 1, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, - 1, -
1, 38, - 1, 40, 111, 112, 43, - 1, 115, 116, 117, - 1, - 1, 50, 51, 122, 123, 54,
55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, - 1, 94, 25, 26, - 1, 98, - 1, - 1, 101, - 1, - 1, - 1, 105, - 1,
- 1, 38, - 1, 40, - 1, 112, 43, - 1, 115, 116, 117, - 1, - 1, 50, 51, 122, 123,
54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
88, 89, 90, 91, 92, - 1, 94, 25, 26, 97, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105,
- 1, - 1, 38, - 1, 40, - 1, 112, 43, - 1, 115, 116, 117, - 1, - 1, 50, 51, 122,
123, 54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, - 1, - 1, - 1, - 1, 101, - 1, - 1, - 1,
105, - 1, - 1, 38, - 1, 40, 111, 112, 43, - 1, 115, 116, 117, - 1, - 1, 50, 51,
122, 123, 54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, - 1, - 1, - 1, - 1, 101, 102, - 1,
- 1, 105, - 1, - 1, 38, - 1, 40, - 1, 112, 43, - 1, 115, 116, 117, - 1, - 1, 50,
51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, - 1, - 1, - 1, - 1, 101, 102,
- 1, - 1, 105, - 1, - 1, 38, - 1, 40, - 1, 112, 43, - 1, 115, 116, 117, - 1, - 1,
50, 51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, - 1, - 1, - 1, - 1, 101,
- 1, - 1, - 1, 105, - 1, - 1, 38, - 1, 40, 111, 112, 43, - 1, 115, 116, 117, - 1,
- 1, 50, 51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, 97, - 1, - 1, - 1,
101, - 1, - 1, - 1, 105, - 1, - 1, 38, - 1, 40, - 1, 112, 43, - 1, 115, 116, 117,
- 1, - 1, 50, 51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65,
66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, 97, - 1, - 1, -
1, 101, - 1, - 1, - 1, 105, - 1, - 1, 38, - 1, 40, - 1, 112, 43, - 1, 115, 116,
117, - 1, - 1, 50, 51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1,
65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, - 1, - 1,
- 1, - 1, 101, 102, - 1, - 1, 105, - 1, - 1, 38, - 1, 40, - 1, 112, 43, - 1, 115,
116, 117, - 1, - 1, 50, 51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, 61, - 1, 63,
- 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, - 1,
- 1, - 1, - 1, 101, 102, - 1, - 1, 105, - 1, - 1, 38, - 1, 40, - 1, 112, 43, - 1,
115, 116, 117, - 1, - 1, 50, 51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, 61, -
1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26,
97, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, - 1, - 1, 38, - 1, 40, - 1, 112, 43,
- 1, 115, 116, 117, - 1, - 1, 50, 51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1,
61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94,
25, 26, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105, - 1, - 1, 38, - 1, 40, - 1,
112, 43, - 1, 115, 116, 117, - 1, - 1, 50, 51, 122, 123, 54, 55, 56, 57, - 1, -
1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, -
1, 94, 25, 26, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105, - 1, - 1, 38, - 1,
40, - 1, 112, 43, - 1, 115, 116, 117, - 1, - 1, 50, 51, 122, 123, 54, 55, 56, 57,
- 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91,
92, - 1, 94, 25, 26, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105, - 1, - 1, 38,
- 1, 40, - 1, 112, 43, - 1, 115, 116, 117, - 1, - 1, 50, 51, 122, 123, 54, 55,
56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89,
90, 91, 92, - 1, 94, 25, 26, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105, - 1, -
1, 38, - 1, 40, - 1, 112, 43, - 1, 115, 116, 117, - 1, - 1, 50, 51, 122, 123, 54,
55, 56, 57, - 1, - 1, - 1, 61, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, - 1, 94, 25, 26, - 1, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, -
1, - 1, 38, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, 50, 51, 122,
123, 54, 55, 56, 57, - 1, - 1, - 1, - 1, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, - 1, 98, - 1, - 1, 101, - 1, - 1, -
1, 105, - 1, - 1, 38, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, 50,
51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, - 1, - 1, 63, - 1, 65, 66, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, - 1, 98, - 1, - 1, 101, - 1,
- 1, - 1, 105, - 1, - 1, 38, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, -
1, 50, 51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, - 1, - 1, 63, - 1, 65, 66, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, - 1, - 1, - 1, - 1,
101, - 1, - 1, - 1, 105, - 1, - 1, 38, - 1, - 1, - 1, 112, - 1, - 1, 115, 116,
117, - 1, - 1, 50, 51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, - 1, - 1, 63, -
1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, 25, 26, - 1, - 1,
- 1, - 1, 101, - 1, - 1, - 1, 105, - 1, - 1, 38, - 1, - 1, - 1, 112, - 1, - 1,
115, 116, 117, - 1, - 1, 50, 51, 122, 123, 54, 55, 56, 57, - 1, - 1, - 1, - 1, -
1, 63, - 1, 65, 66, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, 37,
- 1, 101, - 1, - 1, - 1, 105, - 1, - 1, - 1, - 1, 48, 49, 112, - 1, - 1, 115,
116, 117, - 1, - 1, 58, 59, 122, 123, 62, - 1, - 1, - 1, - 1, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 93, - 1, 95, 37, - 1, 98, - 1, - 1, - 1, - 1, - 1, 104, 105, - 1,
48, 49, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 58, 59, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 85, - 1, - 1, - 1, 37, - 1, - 1, - 1, 93, - 1, 95, - 1, 97, - 1, - 1, 48, 49,
- 1, - 1, 104, 105, - 1, - 1, - 1, - 1, 58, 59, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 85, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 93, - 1, 95, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 104, 105}; unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex(); static int
Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static int
Cyc_yynerrs= 0; struct _tuple18{ void* f1; void* f2; } ; struct _tuple19{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int yystate;
int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short* yyss=(
short*)({ unsigned int _temp3294= 10000u; short* _temp3295=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3294));{ unsigned int
_temp3296= _temp3294; unsigned int i; for( i= 0; i <  _temp3296; i ++){
_temp3295[ i]=( short) 0;}}; _temp3295;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3291= 10000u; void** _temp3292=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3291));{ unsigned int _temp3293= _temp3291;
unsigned int i; for( i= 0; i <  _temp3293; i ++){ _temp3292[ i]= Cyc_yylval;}};
_temp3292;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3288= 10000u; struct Cyc_Yyltype* _temp3289=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3288));{
unsigned int _temp3290= _temp3288; unsigned int i; for( i= 0; i <  _temp3290; i
++){ _temp3289[ i]= Cyc_yynewloc();}}; _temp3289;}); int yystacksize= 10000;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:
yyss[ _check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate;
if( yyssp_offset >=  yystacksize -  1){ if( yystacksize >=  10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 822u, yystate)]; if( yyn ==  - 32768){ goto
yydefault;} if( Cyc_yychar ==  - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar >  0? Cyc_yychar <= 
351: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 352u, Cyc_yychar)]:
236;} yyn += yychar1; if(( yyn <  0? 1: yyn >  4621)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4622u, yyn)] !=  yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4622u, yyn)]; if( yyn <  0){
if( yyn ==  - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
==  0){ goto yyerrlab;}} if( yyn ==  821){ return 0;} if( Cyc_yychar !=  0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus !=  0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 822u, yystate)]; if(
yyn ==  0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 409u, yyn)]; if( yylen >  0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset +  1) -  yylen)];} switch(
yyn){ case 1: _LL1437: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1438:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1440=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1440[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1441; _temp1441.tag= Cyc_DeclList_tok; _temp1441.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1441;});
_temp1440;}); break; case 3: _LL1439: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1443=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1443[ 0]=({ struct Cyc_DeclList_tok_struct _temp1444; _temp1444.tag= Cyc_DeclList_tok;
_temp1444.f1=({ struct Cyc_List_List* _temp1445=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1445->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1446=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1446->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1447=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1447[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1448; _temp1448.tag= Cyc_Absyn_Using_d;
_temp1448.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1448.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1448;}); _temp1447;}));
_temp1446->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1446;});
_temp1445->tl= 0; _temp1445;}); _temp1444;}); _temp1443;}); Cyc_Lex_leave_using();
break; case 4: _LL1442: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1450=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1450[ 0]=({ struct Cyc_DeclList_tok_struct _temp1451; _temp1451.tag= Cyc_DeclList_tok;
_temp1451.f1=({ struct Cyc_List_List* _temp1452=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1452->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1453=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1453->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1454=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1454[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1455; _temp1455.tag= Cyc_Absyn_Using_d;
_temp1455.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp1455.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1455;});
_temp1454;})); _temp1453->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1453;}); _temp1452->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1452;}); _temp1451;});
_temp1450;}); break; case 5: _LL1449: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1457=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1457[ 0]=({ struct Cyc_DeclList_tok_struct _temp1458; _temp1458.tag= Cyc_DeclList_tok;
_temp1458.f1=({ struct Cyc_List_List* _temp1459=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1459->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1460=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1460->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1461=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1461[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1462; _temp1462.tag=
Cyc_Absyn_Namespace_d; _temp1462.f1=({ struct _tagged_arr* _temp1463=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1463[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1463;});
_temp1462.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1462;}); _temp1461;})); _temp1460->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1460;}); _temp1459->tl= 0; _temp1459;}); _temp1458;}); _temp1457;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1456: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1465=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1465[ 0]=({ struct Cyc_DeclList_tok_struct _temp1466; _temp1466.tag= Cyc_DeclList_tok;
_temp1466.f1=({ struct Cyc_List_List* _temp1467=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1467->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1468=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1468->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1469=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1469[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1470; _temp1470.tag=
Cyc_Absyn_Namespace_d; _temp1470.f1=({ struct _tagged_arr* _temp1471=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1471[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1471;});
_temp1470.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1470;}); _temp1469;})); _temp1468->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1468;}); _temp1467->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1467;}); _temp1466;});
_temp1465;}); break; case 7: _LL1464: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1473=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1473[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1474; _temp1474.tag= Cyc_DeclList_tok; _temp1474.f1=({ struct Cyc_List_List*
_temp1475=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1475->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1476=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1476->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1477=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1477[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1478; _temp1478.tag= Cyc_Absyn_ExternC_d;
_temp1478.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1478;}); _temp1477;})); _temp1476->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1476;}); _temp1475->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1475;}); _temp1474;});
_temp1473;}); break; case 8: _LL1472: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1480=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1480[ 0]=({ struct Cyc_DeclList_tok_struct _temp1481; _temp1481.tag= Cyc_DeclList_tok;
_temp1481.f1= 0; _temp1481;}); _temp1480;}); break; case 9: _LL1479: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1483=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1483[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1484; _temp1484.tag= Cyc_DeclList_tok; _temp1484.f1=({ struct Cyc_List_List*
_temp1485=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1485->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1486=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1486[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1487; _temp1487.tag= Cyc_Absyn_Fn_d;
_temp1487.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1487;}); _temp1486;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1485->tl=
0; _temp1485;}); _temp1484;}); _temp1483;}); break; case 10: _LL1482: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1488: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1490=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1490[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1491; _temp1491.tag= Cyc_FnDecl_tok; _temp1491.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1491;}); _temp1490;}); break; case 12: _LL1489: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
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
_temp1494;}); _temp1493;}); break; case 13: _LL1492: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1497=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1497[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1498; _temp1498.tag= Cyc_FnDecl_tok;
_temp1498.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1498;}); _temp1497;}); break; case 14: _LL1496: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1500=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1500[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1501; _temp1501.tag= Cyc_FnDecl_tok;
_temp1501.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1502=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1502->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1502;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1501;}); _temp1500;}); break; case 15: _LL1499: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1504=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1504[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1505; _temp1505.tag= Cyc_FnDecl_tok;
_temp1505.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1506=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1506->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1506;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1505;}); _temp1504;}); break; case 16: _LL1503: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1508=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1508[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1509; _temp1509.tag= Cyc_FnDecl_tok;
_temp1509.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1510=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1510->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1510;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1509;}); _temp1508;}); break; case 17: _LL1507: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1511:
Cyc_Lex_leave_using(); break; case 19: _LL1512: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1514=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1514[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1514;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1513: Cyc_Lex_leave_namespace(); break; case 21: _LL1515: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1517=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1517[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1518; _temp1518.tag= Cyc_DeclList_tok; _temp1518.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1518;}); _temp1517;}); break; case 22: _LL1516: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1520=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1520[ 0]=({ struct Cyc_DeclList_tok_struct _temp1521; _temp1521.tag= Cyc_DeclList_tok;
_temp1521.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1521;}); _temp1520;}); break; case 23: _LL1519: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1523=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1523[ 0]=({ struct Cyc_DeclList_tok_struct _temp1524; _temp1524.tag= Cyc_DeclList_tok;
_temp1524.f1=({ struct Cyc_List_List* _temp1525=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1525->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1525->tl=
0; _temp1525;}); _temp1524;}); _temp1523;}); break; case 24: _LL1522: { struct
Cyc_List_List* _temp1527= 0;{ struct Cyc_List_List* _temp1528= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0;
_temp1528 !=  0; _temp1528= _temp1528->tl){ struct _tagged_arr* _temp1529=(
struct _tagged_arr*) _temp1528->hd; struct _tuple1* _temp1530=({ struct _tuple1*
_temp1533=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1533->f1=
Cyc_Absyn_rel_ns_null; _temp1533->f2= _temp1529; _temp1533;}); struct Cyc_Absyn_Vardecl*
_temp1531= Cyc_Absyn_new_vardecl( _temp1530, Cyc_Absyn_wildtyp( 0), 0);
_temp1527=({ struct Cyc_List_List* _temp1532=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1532->hd=( void*) _temp1531; _temp1532->tl=
_temp1527; _temp1532;});}} _temp1527=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1527); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1534=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1534[ 0]=({ struct Cyc_DeclList_tok_struct _temp1535; _temp1535.tag= Cyc_DeclList_tok;
_temp1535.f1=({ struct Cyc_List_List* _temp1536=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1536->hd=( void*) Cyc_Absyn_letv_decl(
_temp1527, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1536->tl= 0; _temp1536;}); _temp1535;});
_temp1534;}); break;} case 25: _LL1526: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1537:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1539=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1539[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1540; _temp1540.tag= Cyc_DeclList_tok; _temp1540.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1540;});
_temp1539;}); break; case 27: _LL1538: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1542=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1542[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1543; _temp1543.tag= Cyc_DeclSpec_tok;
_temp1543.f1=({ struct Cyc_Parse_Declaration_spec* _temp1544=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1544->sc=({ struct
Cyc_Core_Opt* _temp1545=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1545->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1545;});
_temp1544->tq= Cyc_Absyn_empty_tqual(); _temp1544->type_specs= 0; _temp1544->is_inline=
0; _temp1544->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1544;}); _temp1543;});
_temp1542;}); break; case 28: _LL1541: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc !=  0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1547=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1547[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1548; _temp1548.tag= Cyc_DeclSpec_tok; _temp1548.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1549=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1549->sc=({ struct Cyc_Core_Opt* _temp1550=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1550->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1550;});
_temp1549->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1549->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1549->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1549->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1549;}); _temp1548;}); _temp1547;}); break; case 29: _LL1546: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1552=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1552[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1553; _temp1553.tag= Cyc_DeclSpec_tok; _temp1553.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1554=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1554->sc= 0; _temp1554->tq= Cyc_Absyn_empty_tqual(); _temp1554->type_specs=({
struct Cyc_List_List* _temp1555=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1555->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1555->tl= 0;
_temp1555;}); _temp1554->is_inline= 0; _temp1554->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1554;});
_temp1553;}); _temp1552;}); break; case 30: _LL1551: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1557=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1557[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1558; _temp1558.tag= Cyc_DeclSpec_tok;
_temp1558.f1=({ struct Cyc_Parse_Declaration_spec* _temp1559=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1559->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1559->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1559->type_specs=({ struct Cyc_List_List* _temp1560=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1560->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1560->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1560;});
_temp1559->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1559->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1559;}); _temp1558;}); _temp1557;}); break; case 31: _LL1556: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1562=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1562[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1563; _temp1563.tag= Cyc_DeclSpec_tok; _temp1563.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1564=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1564->sc= 0; _temp1564->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1564->type_specs=
0; _temp1564->is_inline= 0; _temp1564->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1564;});
_temp1563;}); _temp1562;}); break; case 32: _LL1561: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1566=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1566[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1567; _temp1567.tag= Cyc_DeclSpec_tok;
_temp1567.f1=({ struct Cyc_Parse_Declaration_spec* _temp1568=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1568->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1568->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1568->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1568->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1568->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1568;}); _temp1567;}); _temp1566;}); break; case 33: _LL1565: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1570=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1570[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1571; _temp1571.tag= Cyc_DeclSpec_tok; _temp1571.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1572=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1572->sc= 0; _temp1572->tq= Cyc_Absyn_empty_tqual(); _temp1572->type_specs=
0; _temp1572->is_inline= 1; _temp1572->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1572;});
_temp1571;}); _temp1570;}); break; case 34: _LL1569: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1574=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1574[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1575; _temp1575.tag= Cyc_DeclSpec_tok;
_temp1575.f1=({ struct Cyc_Parse_Declaration_spec* _temp1576=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1576->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1576->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1576->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1576->is_inline=
1; _temp1576->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1576;}); _temp1575;}); _temp1574;}); break; case 35: _LL1573: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1578=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1578[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1579; _temp1579.tag= Cyc_StorageClass_tok;
_temp1579.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1579;}); _temp1578;});
break; case 36: _LL1577: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1581=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1581[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1582; _temp1582.tag=
Cyc_StorageClass_tok; _temp1582.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1582;}); _temp1581;}); break; case 37: _LL1580: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1584=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1584[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1585; _temp1585.tag=
Cyc_StorageClass_tok; _temp1585.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1585;}); _temp1584;}); break; case 38: _LL1583: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1587=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1587[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1588; _temp1588.tag=
Cyc_StorageClass_tok; _temp1588.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1588;}); _temp1587;}); break; case 39: _LL1586: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1590=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1590[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1591; _temp1591.tag= Cyc_StorageClass_tok;
_temp1591.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1591;}); _temp1590;});
break; case 40: _LL1589: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1593=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1593[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1594; _temp1594.tag=
Cyc_StorageClass_tok; _temp1594.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1594;}); _temp1593;}); break; case 41: _LL1592: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1596=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1596[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1597; _temp1597.tag=
Cyc_StorageClass_tok; _temp1597.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1597;}); _temp1596;}); break; case 42: _LL1595: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1599=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1599[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1600; _temp1600.tag=
Cyc_AttributeList_tok; _temp1600.f1= 0; _temp1600;}); _temp1599;}); break; case
43: _LL1598: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1601: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1603=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1603[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1604; _temp1604.tag=
Cyc_AttributeList_tok; _temp1604.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1604;});
_temp1603;}); break; case 45: _LL1602: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1606=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1606[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1607; _temp1607.tag=
Cyc_AttributeList_tok; _temp1607.f1=({ struct Cyc_List_List* _temp1608=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1608->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1608->tl= 0; _temp1608;}); _temp1607;}); _temp1606;});
break; case 46: _LL1605: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1610=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1610[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1611; _temp1611.tag=
Cyc_AttributeList_tok; _temp1611.f1=({ struct Cyc_List_List* _temp1612=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1612->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1612->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1612;}); _temp1611;});
_temp1610;}); break; case 47: _LL1609: { struct _tagged_arr _temp1614= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1614, sizeof( unsigned char)) >  4?*(( const unsigned char*)
_check_unknown_subscript( _temp1614, sizeof( unsigned char), 0)) == '_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1614, sizeof( unsigned char),
1)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1614,
sizeof( unsigned char),( int)( _get_arr_size( _temp1614, sizeof( unsigned char))
-  2))) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1614,
sizeof( unsigned char),( int)( _get_arr_size( _temp1614, sizeof( unsigned char))
-  3))) == '_': 0){ _temp1614=( struct _tagged_arr) Cyc_Std_substring( _temp1614,
2, _get_arr_size( _temp1614, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0;
i <  16u; ++ i){ if( Cyc_Std_strcmp( _temp1614,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1615=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1615[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1616; _temp1616.tag= Cyc_Attribute_tok; _temp1616.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1616;}); _temp1615;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1617=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1617[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1618; _temp1618.tag= Cyc_Attribute_tok; _temp1618.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1618;}); _temp1617;});} break;}} case
48: _LL1613: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1620=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1620[ 0]=({ struct Cyc_Attribute_tok_struct _temp1621; _temp1621.tag= Cyc_Attribute_tok;
_temp1621.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1621;}); _temp1620;});
break; case 49: _LL1619: { struct _tagged_arr _temp1623= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1626; int _temp1627; struct _tuple15* _temp1624= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1626=*
_temp1624; _LL1628: _temp1627= _temp1626.f2; goto _LL1625; _LL1625: { void* a;
if( Cyc_Std_zstrcmp( _temp1623, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1623, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1627 <  0? 1: _temp1627 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 0 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1629=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1629[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1630; _temp1630.tag= Cyc_Absyn_Regparm_att; _temp1630.f1= _temp1627;
_temp1630;}); _temp1629;});} else{ if( Cyc_Std_zstrcmp( _temp1623, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1623, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1627 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1627;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1631=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1631[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1632; _temp1632.tag= Cyc_Absyn_Aligned_att; _temp1632.f1= _temp1627;
_temp1632;}); _temp1631;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1633=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1633[ 0]=({ struct Cyc_Attribute_tok_struct _temp1634; _temp1634.tag= Cyc_Attribute_tok;
_temp1634.f1=( void*) a; _temp1634;}); _temp1633;}); break;}} case 50: _LL1622: {
struct _tagged_arr _temp1636= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1637= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1636, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1636, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1638=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1638[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1639; _temp1639.tag=
Cyc_Absyn_Section_att; _temp1639.f1= _temp1637; _temp1639;}); _temp1638;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1640=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1640[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1641; _temp1641.tag= Cyc_Attribute_tok; _temp1641.f1=(
void*) a; _temp1641;}); _temp1640;}); break;} case 51: _LL1635: { struct
_tagged_arr _temp1643= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1644= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1647; struct _tuple15 _temp1645=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1648:
_temp1647= _temp1645.f2; goto _LL1646; _LL1646: { int _temp1651; struct _tuple15
_temp1649=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1652: _temp1651= _temp1649.f2; goto _LL1650; _LL1650: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1643, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1643, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1644, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1653=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1653[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1654; _temp1654.tag=
Cyc_Absyn_Format_att; _temp1654.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1654.f2= _temp1647; _temp1654.f3= _temp1651; _temp1654;}); _temp1653;});}
else{ if( Cyc_Std_zstrcmp( _temp1644, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1655=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1655[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1656; _temp1656.tag=
Cyc_Absyn_Format_att; _temp1656.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1656.f2= _temp1647; _temp1656.f3= _temp1651; _temp1656;}); _temp1655;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1657=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1657[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1658; _temp1658.tag= Cyc_Attribute_tok; _temp1658.f1=(
void*) a; _temp1658;}); _temp1657;}); break;}}} case 52: _LL1642: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1660=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1660[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1661; _temp1661.tag= Cyc_TypeSpecifier_tok;
_temp1661.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1661;});
_temp1660;}); break; case 53: _LL1659: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1663=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1663[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1664; _temp1664.tag=
Cyc_TypeSpecifier_tok; _temp1664.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1664;}); _temp1663;}); break; case 54: _LL1662:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1666=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1666[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1667; _temp1667.tag= Cyc_TypeSpecifier_tok;
_temp1667.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1667;});
_temp1666;}); break; case 55: _LL1665: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1669=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1669[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1670; _temp1670.tag=
Cyc_TypeSpecifier_tok; _temp1670.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1671=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1671[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1672; _temp1672.tag=
Cyc_Parse_Short_spec; _temp1672.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1672;});
_temp1671;})); _temp1670;}); _temp1669;}); break; case 56: _LL1668: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1674=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1674[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1675; _temp1675.tag= Cyc_TypeSpecifier_tok;
_temp1675.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1675;});
_temp1674;}); break; case 57: _LL1673: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1677=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1677[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1678; _temp1678.tag=
Cyc_TypeSpecifier_tok; _temp1678.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1679=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1679[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1680; _temp1680.tag= Cyc_Parse_Long_spec;
_temp1680.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1680;}); _temp1679;})); _temp1678;}); _temp1677;});
break; case 58: _LL1676: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1682=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1682[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1683; _temp1683.tag=
Cyc_TypeSpecifier_tok; _temp1683.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1683;}); _temp1682;}); break; case 59: _LL1681:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1685=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1685[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1686; _temp1686.tag= Cyc_TypeSpecifier_tok;
_temp1686.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t( 0), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1686;});
_temp1685;}); break; case 60: _LL1684: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1688=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1688[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1689; _temp1689.tag=
Cyc_TypeSpecifier_tok; _temp1689.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1690=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1690[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1691; _temp1691.tag=
Cyc_Parse_Signed_spec; _temp1691.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1691;});
_temp1690;})); _temp1689;}); _temp1688;}); break; case 61: _LL1687: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1693=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1693[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1694; _temp1694.tag= Cyc_TypeSpecifier_tok;
_temp1694.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1695=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1695[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1696; _temp1696.tag= Cyc_Parse_Unsigned_spec; _temp1696.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1696;});
_temp1695;})); _temp1694;}); _temp1693;}); break; case 62: _LL1692: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1697:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1698: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1699: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1701=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1701[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1702; _temp1702.tag=
Cyc_TypeSpecifier_tok; _temp1702.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1702;});
_temp1701;}); break; case 66: _LL1700: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1704=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1704[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1705; _temp1705.tag=
Cyc_TypeSpecifier_tok; _temp1705.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1706=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1706[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1707; _temp1707.tag= Cyc_Absyn_TypedefType;
_temp1707.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1707.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1707.f3= 0;
_temp1707;}); _temp1706;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1705;});
_temp1704;}); break; case 67: _LL1703: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1709=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1709[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1710; _temp1710.tag=
Cyc_TypeSpecifier_tok; _temp1710.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1711=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1711[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1712; _temp1712.tag= Cyc_Absyn_TupleType;
_temp1712.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1712;});
_temp1711;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1710;}); _temp1709;}); break; case 68:
_LL1708: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1714=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1714[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1715; _temp1715.tag=
Cyc_TypeSpecifier_tok; _temp1715.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1716=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1716[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1717; _temp1717.tag= Cyc_Absyn_RgnHandleType;
_temp1717.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1717;}); _temp1716;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1715;}); _temp1714;}); break; case 69: _LL1713: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1719=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1719[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1720; _temp1720.tag=
Cyc_TypeSpecifier_tok; _temp1720.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_SizeofType_struct* _temp1721=( struct Cyc_Absyn_SizeofType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SizeofType_struct)); _temp1721[ 0]=({ struct
Cyc_Absyn_SizeofType_struct _temp1722; _temp1722.tag= Cyc_Absyn_SizeofType;
_temp1722.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1722;}); _temp1721;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1720;}); _temp1719;}); break; case 70: _LL1718: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1724=( struct Cyc_Kind_tok_struct*) _cycalloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1724[ 0]=({ struct Cyc_Kind_tok_struct _temp1725; _temp1725.tag= Cyc_Kind_tok;
_temp1725.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1725;});
_temp1724;}); break; case 71: _LL1723: { struct _tagged_arr* _temp1729; void*
_temp1731; struct _tuple1 _temp1727=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1732: _temp1731=
_temp1727.f1; goto _LL1730; _LL1730: _temp1729= _temp1727.f2; goto _LL1728;
_LL1728: if( _temp1731 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1733=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1733[ 0]=({ struct Cyc_Kind_tok_struct
_temp1734; _temp1734.tag= Cyc_Kind_tok; _temp1734.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1729, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1734;}); _temp1733;}); break;} case 72:
_LL1726: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1736=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1736[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1737; _temp1737.tag= Cyc_TypeQual_tok;
_temp1737.f1=({ struct Cyc_Absyn_Tqual _temp1738; _temp1738.q_const= 1;
_temp1738.q_volatile= 0; _temp1738.q_restrict= 0; _temp1738;}); _temp1737;});
_temp1736;}); break; case 73: _LL1735: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1740=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1740[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1741; _temp1741.tag= Cyc_TypeQual_tok; _temp1741.f1=({ struct Cyc_Absyn_Tqual
_temp1742; _temp1742.q_const= 0; _temp1742.q_volatile= 1; _temp1742.q_restrict=
0; _temp1742;}); _temp1741;}); _temp1740;}); break; case 74: _LL1739: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1744=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1744[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1745; _temp1745.tag= Cyc_TypeQual_tok;
_temp1745.f1=({ struct Cyc_Absyn_Tqual _temp1746; _temp1746.q_const= 0;
_temp1746.q_volatile= 0; _temp1746.q_restrict= 1; _temp1746;}); _temp1745;});
_temp1744;}); break; case 75: _LL1743: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1748=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1748[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1749; _temp1749.tag=
Cyc_TypeSpecifier_tok; _temp1749.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1750=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1750[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1751; _temp1751.tag= Cyc_Parse_Decl_spec;
_temp1751.f1=({ struct Cyc_Absyn_Decl* _temp1752=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1752->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1753=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1753[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1754; _temp1754.tag= Cyc_Absyn_Enum_d; _temp1754.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1755=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1755->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1755->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1755->fields=({
struct Cyc_Core_Opt* _temp1756=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1756->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1756;});
_temp1755;}); _temp1754;}); _temp1753;})); _temp1752->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1752;}); _temp1751;}); _temp1750;})); _temp1749;}); _temp1748;}); break;
case 76: _LL1747: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1758=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1758[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1759; _temp1759.tag=
Cyc_TypeSpecifier_tok; _temp1759.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1760=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1760[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1761; _temp1761.tag= Cyc_Absyn_EnumType;
_temp1761.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1761.f2= 0; _temp1761;}); _temp1760;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1759;}); _temp1758;}); break; case 77: _LL1757: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1763=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1763[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1764; _temp1764.tag=
Cyc_TypeSpecifier_tok; _temp1764.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1765=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1765[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1766; _temp1766.tag= Cyc_Parse_Type_spec;
_temp1766.f1=( void*)(( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp1767=(
struct Cyc_Absyn_AnonEnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct));
_temp1767[ 0]=({ struct Cyc_Absyn_AnonEnumType_struct _temp1768; _temp1768.tag=
Cyc_Absyn_AnonEnumType; _temp1768.f1= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1768;});
_temp1767;})); _temp1766.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1766;});
_temp1765;})); _temp1764;}); _temp1763;}); break; case 78: _LL1762: yyval=( void*)({
struct Cyc_Enumfield_tok_struct* _temp1770=( struct Cyc_Enumfield_tok_struct*)
_cycalloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp1770[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp1771; _temp1771.tag= Cyc_Enumfield_tok; _temp1771.f1=({
struct Cyc_Absyn_Enumfield* _temp1772=( struct Cyc_Absyn_Enumfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumfield)); _temp1772->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1772->tag= 0;
_temp1772->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1772;});
_temp1771;}); _temp1770;}); break; case 79: _LL1769: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1774=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1774[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1775; _temp1775.tag= Cyc_Enumfield_tok;
_temp1775.f1=({ struct Cyc_Absyn_Enumfield* _temp1776=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1776->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1776->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1776->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1776;});
_temp1775;}); _temp1774;}); break; case 80: _LL1773: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1778=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1778[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1779; _temp1779.tag=
Cyc_EnumfieldList_tok; _temp1779.f1=({ struct Cyc_List_List* _temp1780=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1780->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1780->tl= 0; _temp1780;}); _temp1779;}); _temp1778;});
break; case 81: _LL1777: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1782=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1782[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1783; _temp1783.tag=
Cyc_EnumfieldList_tok; _temp1783.f1=({ struct Cyc_List_List* _temp1784=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1784->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1784->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1784;}); _temp1783;});
_temp1782;}); break; case 82: _LL1781: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1786: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1788=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1788[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1789; _temp1789.tag=
Cyc_Absyn_AnonStructType; _temp1789.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1789;});
_temp1788;}); break; case Cyc_Parse_Union_su: _LL1787: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1791=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1791[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1792; _temp1792.tag= Cyc_Absyn_AnonUnionType; _temp1792.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1792;});
_temp1791;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1793=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1793[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1794; _temp1794.tag=
Cyc_TypeSpecifier_tok; _temp1794.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1795=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1795[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1796; _temp1796.tag= Cyc_Parse_Type_spec;
_temp1796.f1=( void*) t; _temp1796.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1796;});
_temp1795;})); _temp1794;}); _temp1793;}); break;} case 83: _LL1785: { struct
Cyc_List_List* _temp1798=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1799: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1801=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1801->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1801;}), _temp1798,({ struct Cyc_Core_Opt*
_temp1802=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1802->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1802;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1800: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1804=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1804->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1804;}),
_temp1798,({ struct Cyc_Core_Opt* _temp1805=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1805->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1805;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1806=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1806[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1807; _temp1807.tag=
Cyc_TypeSpecifier_tok; _temp1807.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1808=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1808[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1809; _temp1809.tag= Cyc_Parse_Decl_spec;
_temp1809.f1= d; _temp1809;}); _temp1808;})); _temp1807;}); _temp1806;}); break;}
case 84: _LL1797: { struct Cyc_List_List* _temp1811=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1812: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1814=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1814->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1814;}), _temp1811,({ struct Cyc_Core_Opt*
_temp1815=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1815->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1815;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1813: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1817=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1817->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1817;}),
_temp1811,({ struct Cyc_Core_Opt* _temp1818=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1818->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1818;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1819=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1819[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1820; _temp1820.tag=
Cyc_TypeSpecifier_tok; _temp1820.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1821=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1821[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1822; _temp1822.tag= Cyc_Parse_Decl_spec;
_temp1822.f1= d; _temp1822;}); _temp1821;})); _temp1820;}); _temp1819;}); break;}
case 85: _LL1810: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1824: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1826=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1826[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1827; _temp1827.tag=
Cyc_TypeSpecifier_tok; _temp1827.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1828=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1828[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1829; _temp1829.tag= Cyc_Absyn_StructType;
_temp1829.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1829.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1829.f3= 0;
_temp1829;}); _temp1828;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1827;});
_temp1826;}); break; case Cyc_Parse_Union_su: _LL1825: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1831=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1831[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1832; _temp1832.tag= Cyc_TypeSpecifier_tok;
_temp1832.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1833=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1833[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1834; _temp1834.tag= Cyc_Absyn_UnionType;
_temp1834.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1834.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1834.f3= 0;
_temp1834;}); _temp1833;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1832;});
_temp1831;}); break;} break; case 86: _LL1823: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1836: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1838=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1838[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1839; _temp1839.tag=
Cyc_TypeSpecifier_tok; _temp1839.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1840=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1840[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1841; _temp1841.tag= Cyc_Absyn_StructType;
_temp1841.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1841.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1841.f3= 0;
_temp1841;}); _temp1840;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1839;});
_temp1838;}); break; case Cyc_Parse_Union_su: _LL1837: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1843=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1843[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1844; _temp1844.tag= Cyc_TypeSpecifier_tok;
_temp1844.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1845=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1845[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1846; _temp1846.tag= Cyc_Absyn_UnionType;
_temp1846.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1846.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1846.f3= 0;
_temp1846;}); _temp1845;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1844;});
_temp1843;}); break;} break; case 87: _LL1835: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1848=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1848[ 0]=({ struct Cyc_TypeList_tok_struct _temp1849; _temp1849.tag= Cyc_TypeList_tok;
_temp1849.f1= 0; _temp1849;}); _temp1848;}); break; case 88: _LL1847: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1851=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1851[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1852; _temp1852.tag= Cyc_TypeList_tok; _temp1852.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1852;});
_temp1851;}); break; case 89: _LL1850:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1854=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1854[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1855; _temp1855.tag= Cyc_TypeList_tok; _temp1855.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1855;});
_temp1854;}); break; case 90: _LL1853: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1857=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1857[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1858; _temp1858.tag= Cyc_StructOrUnion_tok; _temp1858.f1= Cyc_Parse_Struct_su;
_temp1858;}); _temp1857;}); break; case 91: _LL1856: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1860=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1860[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1861; _temp1861.tag= Cyc_StructOrUnion_tok; _temp1861.f1= Cyc_Parse_Union_su;
_temp1861;}); _temp1860;}); break; case 92: _LL1859: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1863=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1863[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1864; _temp1864.tag= Cyc_StructFieldDeclList_tok; _temp1864.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1864;});
_temp1863;}); break; case 93: _LL1862: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1866=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1866[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1867; _temp1867.tag= Cyc_StructFieldDeclListList_tok; _temp1867.f1=({
struct Cyc_List_List* _temp1868=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1868->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1868->tl= 0;
_temp1868;}); _temp1867;}); _temp1866;}); break; case 94: _LL1865: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1870=( struct Cyc_StructFieldDeclListList_tok_struct*)
_cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1870[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1871; _temp1871.tag= Cyc_StructFieldDeclListList_tok;
_temp1871.f1=({ struct Cyc_List_List* _temp1872=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1872->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1872->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1872;}); _temp1871;}); _temp1870;}); break;
case 95: _LL1869: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1874=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1874[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1875; _temp1875.tag=
Cyc_InitDeclList_tok; _temp1875.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1875;});
_temp1874;}); break; case 96: _LL1873: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1877=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1877[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1878; _temp1878.tag=
Cyc_InitDeclList_tok; _temp1878.f1=({ struct Cyc_List_List* _temp1879=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1879->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1879->tl= 0; _temp1879;}); _temp1878;}); _temp1877;});
break; case 97: _LL1876: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1881=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1881[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1882; _temp1882.tag=
Cyc_InitDeclList_tok; _temp1882.f1=({ struct Cyc_List_List* _temp1883=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1883->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1883->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1883;});
_temp1882;}); _temp1881;}); break; case 98: _LL1880: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1885=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1885[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1886; _temp1886.tag= Cyc_InitDecl_tok;
_temp1886.f1=({ struct _tuple14* _temp1887=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1887->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1887->f2= 0;
_temp1887;}); _temp1886;}); _temp1885;}); break; case 99: _LL1884: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1889=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1889[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1890; _temp1890.tag= Cyc_InitDecl_tok; _temp1890.f1=({ struct _tuple14*
_temp1891=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1891->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1891->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1891;});
_temp1890;}); _temp1889;}); break; case 100: _LL1888: { struct _tuple17
_temp1895; struct Cyc_List_List* _temp1896; struct Cyc_List_List* _temp1898;
struct Cyc_Absyn_Tqual _temp1900; struct _tuple17* _temp1893= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1895=*
_temp1893; _LL1901: _temp1900= _temp1895.f1; goto _LL1899; _LL1899: _temp1898=
_temp1895.f2; goto _LL1897; _LL1897: _temp1896= _temp1895.f3; goto _LL1894;
_LL1894: { struct Cyc_List_List* _temp1904; struct Cyc_List_List* _temp1906;
struct _tuple0 _temp1902=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1907: _temp1906= _temp1902.f1; goto _LL1905; _LL1905:
_temp1904= _temp1902.f2; goto _LL1903; _LL1903: { void* _temp1908= Cyc_Parse_speclist2typ(
_temp1898, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1909=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1900, _temp1908, _temp1906, _temp1896), _temp1904);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1910=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1910[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1911; _temp1911.tag=
Cyc_StructFieldDeclList_tok; _temp1911.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1909); _temp1911;}); _temp1910;}); break;}}}
case 101: _LL1892: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1913=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1913[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1914; _temp1914.tag=
Cyc_QualSpecList_tok; _temp1914.f1=({ struct _tuple17* _temp1915=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1915->f1= Cyc_Absyn_empty_tqual();
_temp1915->f2=({ struct Cyc_List_List* _temp1916=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1916->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1916->tl=
0; _temp1916;}); _temp1915->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1915;}); _temp1914;});
_temp1913;}); break; case 102: _LL1912: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1918=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1918[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1919; _temp1919.tag=
Cyc_QualSpecList_tok; _temp1919.f1=({ struct _tuple17* _temp1920=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1920->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1920->f2=({
struct Cyc_List_List* _temp1921=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1921->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1921->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1921;}); _temp1920->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1920;});
_temp1919;}); _temp1918;}); break; case 103: _LL1917: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1923=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1923[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1924; _temp1924.tag= Cyc_QualSpecList_tok;
_temp1924.f1=({ struct _tuple17* _temp1925=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1925->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1925->f2= 0;
_temp1925->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1925;}); _temp1924;}); _temp1923;}); break; case
104: _LL1922: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1927=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1927[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1928; _temp1928.tag=
Cyc_QualSpecList_tok; _temp1928.f1=({ struct _tuple17* _temp1929=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1929->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1929->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1929->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1929;});
_temp1928;}); _temp1927;}); break; case 105: _LL1926: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1931=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1931[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1932; _temp1932.tag= Cyc_InitDeclList_tok;
_temp1932.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1932;}); _temp1931;}); break; case 106: _LL1930: yyval=(
void*)({ struct Cyc_InitDeclList_tok_struct* _temp1934=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1934[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1935; _temp1935.tag= Cyc_InitDeclList_tok;
_temp1935.f1=({ struct Cyc_List_List* _temp1936=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1936->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1936->tl= 0;
_temp1936;}); _temp1935;}); _temp1934;}); break; case 107: _LL1933: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp1938=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1938[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1939; _temp1939.tag= Cyc_InitDeclList_tok;
_temp1939.f1=({ struct Cyc_List_List* _temp1940=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1940->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1940->tl=
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1940;}); _temp1939;}); _temp1938;}); break; case 108:
_LL1937: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp1942=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1942[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1943; _temp1943.tag= Cyc_InitDecl_tok; _temp1943.f1=({ struct _tuple14*
_temp1944=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1944->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1944->f2= 0; _temp1944;}); _temp1943;}); _temp1942;});
break; case 109: _LL1941: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1946=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1946[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1947; _temp1947.tag= Cyc_InitDecl_tok;
_temp1947.f1=({ struct _tuple14* _temp1948=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1948->f1=({ struct Cyc_Parse_Declarator*
_temp1949=( struct Cyc_Parse_Declarator*) _cycalloc( sizeof( struct Cyc_Parse_Declarator));
_temp1949->id=({ struct _tuple1* _temp1950=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp1950->f1= Cyc_Absyn_rel_ns_null; _temp1950->f2=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1950;}); _temp1949->tms= 0; _temp1949;});
_temp1948->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1948;}); _temp1947;});
_temp1946;}); break; case 110: _LL1945: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1952=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1952[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1953; _temp1953.tag= Cyc_InitDecl_tok;
_temp1953.f1=({ struct _tuple14* _temp1954=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1954->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1954->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1954;}); _temp1953;}); _temp1952;}); break; case
111: _LL1951: { struct Cyc_List_List* _temp1956=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1957=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1957[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1958; _temp1958.tag=
Cyc_TypeSpecifier_tok; _temp1958.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1959=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1959[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1960; _temp1960.tag= Cyc_Parse_Decl_spec;
_temp1960.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1956,({
struct Cyc_Core_Opt* _temp1961=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1961->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1961;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1960;}); _temp1959;})); _temp1958;}); _temp1957;}); break;} case 112:
_LL1955: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1963=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1963[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1964; _temp1964.tag=
Cyc_TypeSpecifier_tok; _temp1964.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1965=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1965[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1966; _temp1966.tag= Cyc_Absyn_TunionType;
_temp1966.f1=({ struct Cyc_Absyn_TunionInfo _temp1967; _temp1967.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1968=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1968[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1969; _temp1969.tag= Cyc_Absyn_UnknownTunion;
_temp1969.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1970; _temp1970.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1970.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1970;});
_temp1969;}); _temp1968;})); _temp1967.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1967.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1967;}); _temp1966;}); _temp1965;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1964;}); _temp1963;}); break; case 113: _LL1962: { void* _temp1972= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp1981=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1981->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp1981;}),
0); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1973=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1973[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1974; _temp1974.tag= Cyc_TypeSpecifier_tok;
_temp1974.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1975=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1975[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1976; _temp1976.tag=
Cyc_Absyn_TunionType; _temp1976.f1=({ struct Cyc_Absyn_TunionInfo _temp1977;
_temp1977.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1978=( struct Cyc_Absyn_UnknownTunion_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1978[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1979; _temp1979.tag= Cyc_Absyn_UnknownTunion; _temp1979.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1980; _temp1980.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1980.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1980;}); _temp1979;}); _temp1978;})); _temp1977.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1977.rgn=(
void*) _temp1972; _temp1977;}); _temp1976;}); _temp1975;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1974;}); _temp1973;}); break;} case 114: _LL1971: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1983=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1983[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1984; _temp1984.tag= Cyc_TypeSpecifier_tok;
_temp1984.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1985=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1985[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1986; _temp1986.tag= Cyc_Absyn_TunionFieldType; _temp1986.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1987; _temp1987.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp1988=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp1988[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp1989; _temp1989.tag= Cyc_Absyn_UnknownTunionfield; _temp1989.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp1990; _temp1990.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1990.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1990.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1990;});
_temp1989;}); _temp1988;})); _temp1987.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1987;}); _temp1986;});
_temp1985;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1984;}); _temp1983;}); break; case 115:
_LL1982: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1992=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1992[ 0]=({ struct
Cyc_Bool_tok_struct _temp1993; _temp1993.tag= Cyc_Bool_tok; _temp1993.f1= 0;
_temp1993;}); _temp1992;}); break; case 116: _LL1991: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1995=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1995[ 0]=({ struct Cyc_Bool_tok_struct
_temp1996; _temp1996.tag= Cyc_Bool_tok; _temp1996.f1= 1; _temp1996;}); _temp1995;});
break; case 117: _LL1994: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1998=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1998[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1999; _temp1999.tag= Cyc_TunionFieldList_tok; _temp1999.f1=({ struct Cyc_List_List*
_temp2000=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2000->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2000->tl= 0;
_temp2000;}); _temp1999;}); _temp1998;}); break; case 118: _LL1997: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2002=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2002[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2003; _temp2003.tag= Cyc_TunionFieldList_tok;
_temp2003.f1=({ struct Cyc_List_List* _temp2004=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2004->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2004->tl=
0; _temp2004;}); _temp2003;}); _temp2002;}); break; case 119: _LL2001: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2006=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2006[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2007; _temp2007.tag= Cyc_TunionFieldList_tok;
_temp2007.f1=({ struct Cyc_List_List* _temp2008=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2008->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2008->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2008;}); _temp2007;}); _temp2006;}); break; case 120:
_LL2005: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2010=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2010[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2011; _temp2011.tag=
Cyc_TunionFieldList_tok; _temp2011.f1=({ struct Cyc_List_List* _temp2012=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2012->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2012->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2012;}); _temp2011;});
_temp2010;}); break; case 121: _LL2009: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2014=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2014[ 0]=({ struct Cyc_Scope_tok_struct _temp2015; _temp2015.tag= Cyc_Scope_tok;
_temp2015.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2015;}); _temp2014;});
break; case 122: _LL2013: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2017=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2017[ 0]=({ struct Cyc_Scope_tok_struct _temp2018; _temp2018.tag= Cyc_Scope_tok;
_temp2018.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2018;}); _temp2017;});
break; case 123: _LL2016: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2020=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2020[ 0]=({ struct Cyc_Scope_tok_struct _temp2021; _temp2021.tag= Cyc_Scope_tok;
_temp2021.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2021;}); _temp2020;});
break; case 124: _LL2019: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2023=( struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2023[ 0]=({ struct Cyc_TunionField_tok_struct _temp2024; _temp2024.tag= Cyc_TunionField_tok;
_temp2024.f1=({ struct Cyc_Absyn_Tunionfield* _temp2025=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2025->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2025->tvs= 0;
_temp2025->typs= 0; _temp2025->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2025->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2025;}); _temp2024;}); _temp2023;}); break; case 125:
_LL2022: { struct Cyc_List_List* _temp2027=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp2028=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp2029=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2029[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2030; _temp2030.tag= Cyc_TunionField_tok;
_temp2030.f1=({ struct Cyc_Absyn_Tunionfield* _temp2031=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2031->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2031->tvs=
_temp2028; _temp2031->typs= _temp2027; _temp2031->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2031->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp2031;}); _temp2030;}); _temp2029;}); break;}
case 126: _LL2026: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 127: _LL2032: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2034=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2034[ 0]=({ struct Cyc_Declarator_tok_struct _temp2035; _temp2035.tag= Cyc_Declarator_tok;
_temp2035.f1=({ struct Cyc_Parse_Declarator* _temp2036=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2036->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2036->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2036;});
_temp2035;}); _temp2034;}); break; case 128: _LL2033: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2038=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2038[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2039; _temp2039.tag= Cyc_Declarator_tok;
_temp2039.f1=({ struct Cyc_Parse_Declarator* _temp2040=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2040->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2040->tms= 0;
_temp2040;}); _temp2039;}); _temp2038;}); break; case 129: _LL2037: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 130:
_LL2041: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2043=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2043[ 0]=({ struct Cyc_Declarator_tok_struct _temp2044; _temp2044.tag= Cyc_Declarator_tok;
_temp2044.f1=({ struct Cyc_Parse_Declarator* _temp2045=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2045->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp2045->tms=({ struct Cyc_List_List* _temp2046=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2046->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2046->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp2046;}); _temp2045;}); _temp2044;});
_temp2043;}); break; case 131: _LL2042: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2048=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2048[ 0]=({ struct Cyc_Declarator_tok_struct _temp2049; _temp2049.tag= Cyc_Declarator_tok;
_temp2049.f1=({ struct Cyc_Parse_Declarator* _temp2050=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2050->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2050->tms=({ struct Cyc_List_List* _temp2051=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2051->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2052=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2052[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2053; _temp2053.tag= Cyc_Absyn_ConstArray_mod;
_temp2053.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2053;}); _temp2052;})); _temp2051->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2051;}); _temp2050;}); _temp2049;}); _temp2048;}); break; case 132: _LL2047: {
struct _tuple16 _temp2057; struct Cyc_List_List* _temp2058; struct Cyc_Core_Opt*
_temp2060; struct Cyc_Absyn_VarargInfo* _temp2062; int _temp2064; struct Cyc_List_List*
_temp2066; struct _tuple16* _temp2055= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2057=*
_temp2055; _LL2067: _temp2066= _temp2057.f1; goto _LL2065; _LL2065: _temp2064=
_temp2057.f2; goto _LL2063; _LL2063: _temp2062= _temp2057.f3; goto _LL2061;
_LL2061: _temp2060= _temp2057.f4; goto _LL2059; _LL2059: _temp2058= _temp2057.f5;
goto _LL2056; _LL2056: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2068=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2068[ 0]=({ struct Cyc_Declarator_tok_struct _temp2069; _temp2069.tag= Cyc_Declarator_tok;
_temp2069.f1=({ struct Cyc_Parse_Declarator* _temp2070=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2070->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2070->tms=({ struct Cyc_List_List* _temp2071=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2071->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2072=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2072[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2073; _temp2073.tag= Cyc_Absyn_Function_mod;
_temp2073.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2074=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2074[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2075; _temp2075.tag= Cyc_Absyn_WithTypes;
_temp2075.f1= _temp2066; _temp2075.f2= _temp2064; _temp2075.f3= _temp2062;
_temp2075.f4= _temp2060; _temp2075.f5= _temp2058; _temp2075;}); _temp2074;}));
_temp2073;}); _temp2072;})); _temp2071->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2071;});
_temp2070;}); _temp2069;}); _temp2068;}); break;} case 133: _LL2054: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2077=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2077[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2078; _temp2078.tag= Cyc_Declarator_tok;
_temp2078.f1=({ struct Cyc_Parse_Declarator* _temp2079=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2079->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2079->tms=({ struct Cyc_List_List* _temp2080=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2080->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2081=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2081[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2082; _temp2082.tag= Cyc_Absyn_Function_mod;
_temp2082.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2083=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2083[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2084; _temp2084.tag= Cyc_Absyn_WithTypes;
_temp2084.f1= 0; _temp2084.f2= 0; _temp2084.f3= 0; _temp2084.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2084.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2084;}); _temp2083;})); _temp2082;}); _temp2081;}));
_temp2080->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2080;}); _temp2079;}); _temp2078;});
_temp2077;}); break; case 134: _LL2076: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2086=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2086[ 0]=({ struct Cyc_Declarator_tok_struct _temp2087; _temp2087.tag= Cyc_Declarator_tok;
_temp2087.f1=({ struct Cyc_Parse_Declarator* _temp2088=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2088->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2088->tms=({ struct Cyc_List_List* _temp2089=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2089->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2090=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2090[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2091; _temp2091.tag= Cyc_Absyn_Function_mod;
_temp2091.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2092=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2092[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2093; _temp2093.tag= Cyc_Absyn_NoTypes;
_temp2093.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2093.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2093;});
_temp2092;})); _temp2091;}); _temp2090;})); _temp2089->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2089;}); _temp2088;}); _temp2087;}); _temp2086;}); break; case 135: _LL2085: {
struct Cyc_List_List* _temp2095=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
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
_temp2098;}); _temp2097;}); _temp2096;}); break;} case 136: _LL2094:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2103=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2104=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2104[ 0]=({ struct Cyc_Declarator_tok_struct _temp2105; _temp2105.tag= Cyc_Declarator_tok;
_temp2105.f1=({ struct Cyc_Parse_Declarator* _temp2106=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2106->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2106->tms=({ struct Cyc_List_List* _temp2107=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2107->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2108=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2108[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2109; _temp2109.tag= Cyc_Absyn_TypeParams_mod;
_temp2109.f1= _temp2103; _temp2109.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2109.f3=
0; _temp2109;}); _temp2108;})); _temp2107->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2107;});
_temp2106;}); _temp2105;}); _temp2104;}); break;} case 137: _LL2102: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2111=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2111[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2112; _temp2112.tag= Cyc_Declarator_tok;
_temp2112.f1=({ struct Cyc_Parse_Declarator* _temp2113=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2113->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2113->tms=({ struct Cyc_List_List* _temp2114=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2114->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2115=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2115[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2116; _temp2116.tag= Cyc_Absyn_Attributes_mod;
_temp2116.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2116.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2116;}); _temp2115;}));
_temp2114->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]))->tms; _temp2114;}); _temp2113;}); _temp2112;});
_temp2111;}); break; case 138: _LL2110: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2118=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2118[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2119; _temp2119.tag= Cyc_Declarator_tok;
_temp2119.f1=({ struct Cyc_Parse_Declarator* _temp2120=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2120->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2120->tms= 0;
_temp2120;}); _temp2119;}); _temp2118;}); break; case 139: _LL2117: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2122=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2122[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2123; _temp2123.tag= Cyc_Declarator_tok;
_temp2123.f1=({ struct Cyc_Parse_Declarator* _temp2124=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2124->id= Cyc_Absyn_exn_name;
_temp2124->tms= 0; _temp2124;}); _temp2123;}); _temp2122;}); break; case 140:
_LL2121: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2126=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2126[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2127; _temp2127.tag=
Cyc_TypeModifierList_tok; _temp2127.f1=({ struct Cyc_List_List* _temp2128=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2128->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2129=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2129[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2130; _temp2130.tag= Cyc_Absyn_Pointer_mod;
_temp2130.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2130.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2130.f3= Cyc_Absyn_empty_tqual(); _temp2130;});
_temp2129;})); _temp2128->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2128;});
_temp2127;}); _temp2126;}); break; case 141: _LL2125: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2132=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2132[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2133; _temp2133.tag= Cyc_TypeModifierList_tok;
_temp2133.f1=({ struct Cyc_List_List* _temp2134=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2134->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2135=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2135[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2136; _temp2136.tag= Cyc_Absyn_Pointer_mod;
_temp2136.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2136.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2136.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2136;}); _temp2135;}));
_temp2134->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0);
_temp2134;}); _temp2133;}); _temp2132;}); break; case 142: _LL2131: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2138=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2138[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2139; _temp2139.tag= Cyc_TypeModifierList_tok;
_temp2139.f1=({ struct Cyc_List_List* _temp2140=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2140->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2141=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2141[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2142; _temp2142.tag= Cyc_Absyn_Pointer_mod;
_temp2142.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2142.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2142.f3= Cyc_Absyn_empty_tqual(); _temp2142;});
_temp2141;})); _temp2140->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2140;});
_temp2139;}); _temp2138;}); break; case 143: _LL2137: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2144=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2144[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2145; _temp2145.tag= Cyc_TypeModifierList_tok;
_temp2145.f1=({ struct Cyc_List_List* _temp2146=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2146->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2147=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2147[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2148; _temp2148.tag= Cyc_Absyn_Pointer_mod;
_temp2148.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2148.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2148.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2148;});
_temp2147;})); _temp2146->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2146;});
_temp2145;}); _temp2144;}); break; case 144: _LL2143: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2150=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2150[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2151; _temp2151.tag= Cyc_Pointer_Sort_tok;
_temp2151.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2152=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2152[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2153; _temp2153.tag=
Cyc_Absyn_Nullable_ps; _temp2153.f1= Cyc_Absyn_exp_unsigned_one; _temp2153;});
_temp2152;})); _temp2151;}); _temp2150;}); break; case 145: _LL2149: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2155=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2155[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2156; _temp2156.tag= Cyc_Pointer_Sort_tok;
_temp2156.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2157=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2157[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2158; _temp2158.tag= Cyc_Absyn_NonNullable_ps; _temp2158.f1= Cyc_Absyn_exp_unsigned_one;
_temp2158;}); _temp2157;})); _temp2156;}); _temp2155;}); break; case 146:
_LL2154: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2160=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2160[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2161; _temp2161.tag=
Cyc_Pointer_Sort_tok; _temp2161.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2162=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2162[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2163; _temp2163.tag=
Cyc_Absyn_Nullable_ps; _temp2163.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2163;});
_temp2162;})); _temp2161;}); _temp2160;}); break; case 147: _LL2159: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2165=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2165[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2166; _temp2166.tag= Cyc_Pointer_Sort_tok;
_temp2166.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2167=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2167[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2168; _temp2168.tag= Cyc_Absyn_NonNullable_ps; _temp2168.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2168;});
_temp2167;})); _temp2166;}); _temp2165;}); break; case 148: _LL2164: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2170=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2170[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2171; _temp2171.tag= Cyc_Pointer_Sort_tok;
_temp2171.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2171;});
_temp2170;}); break; case 149: _LL2169: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2173=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2173[ 0]=({ struct Cyc_Type_tok_struct _temp2174; _temp2174.tag= Cyc_Type_tok;
_temp2174.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2175=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2175->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2175;}), 0); _temp2174;}); _temp2173;});
break; case 150: _LL2172: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 151: _LL2176: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
152: _LL2177: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2179=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2179[
0]=({ struct Cyc_Type_tok_struct _temp2180; _temp2180.tag= Cyc_Type_tok;
_temp2180.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2181=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2181->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2181;}), 0); _temp2180;}); _temp2179;});
break; case 153: _LL2178: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 154: _LL2182: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2184=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2184[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2185; _temp2185.tag= Cyc_TypeQual_tok; _temp2185.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2185;});
_temp2184;}); break; case 155: _LL2183: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2187=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2187[ 0]=({ struct Cyc_YY1_struct _temp2188; _temp2188.tag= Cyc_YY1;
_temp2188.f1=({ struct _tuple16* _temp2189=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2189->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2189->f2= 0;
_temp2189->f3= 0; _temp2189->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2189->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2189;});
_temp2188;}); _temp2187;}); break; case 156: _LL2186: yyval=( void*)({ struct
Cyc_YY1_struct* _temp2191=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct
Cyc_YY1_struct)); _temp2191[ 0]=({ struct Cyc_YY1_struct _temp2192; _temp2192.tag=
Cyc_YY1; _temp2192.f1=({ struct _tuple16* _temp2193=( struct _tuple16*)
_cycalloc( sizeof( struct _tuple16)); _temp2193->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2193->f2= 1;
_temp2193->f3= 0; _temp2193->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2193->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2193;});
_temp2192;}); _temp2191;}); break; case 157: _LL2190: { struct _tuple2 _temp2197;
void* _temp2198; struct Cyc_Absyn_Tqual _temp2200; struct Cyc_Core_Opt*
_temp2202; struct _tuple2* _temp2195= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2197=*
_temp2195; _LL2203: _temp2202= _temp2197.f1; goto _LL2201; _LL2201: _temp2200=
_temp2197.f2; goto _LL2199; _LL2199: _temp2198= _temp2197.f3; goto _LL2196;
_LL2196: { struct Cyc_Absyn_VarargInfo* _temp2204=({ struct Cyc_Absyn_VarargInfo*
_temp2208=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2208->name= _temp2202; _temp2208->tq= _temp2200; _temp2208->type=( void*)
_temp2198; _temp2208->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2208;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2205=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2205[ 0]=({ struct Cyc_YY1_struct
_temp2206; _temp2206.tag= Cyc_YY1; _temp2206.f1=({ struct _tuple16* _temp2207=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2207->f1= 0;
_temp2207->f2= 0; _temp2207->f3= _temp2204; _temp2207->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2207->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2207;}); _temp2206;}); _temp2205;}); break;}} case 158:
_LL2194: { struct _tuple2 _temp2212; void* _temp2213; struct Cyc_Absyn_Tqual
_temp2215; struct Cyc_Core_Opt* _temp2217; struct _tuple2* _temp2210= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2212=*
_temp2210; _LL2218: _temp2217= _temp2212.f1; goto _LL2216; _LL2216: _temp2215=
_temp2212.f2; goto _LL2214; _LL2214: _temp2213= _temp2212.f3; goto _LL2211;
_LL2211: { struct Cyc_Absyn_VarargInfo* _temp2219=({ struct Cyc_Absyn_VarargInfo*
_temp2223=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2223->name= _temp2217; _temp2223->tq= _temp2215; _temp2223->type=( void*)
_temp2213; _temp2223->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2223;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2220=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2220[ 0]=({ struct Cyc_YY1_struct
_temp2221; _temp2221.tag= Cyc_YY1; _temp2221.f1=({ struct _tuple16* _temp2222=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2222->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2222->f2=
0; _temp2222->f3= _temp2219; _temp2222->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2222->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2222;});
_temp2221;}); _temp2220;}); break;}} case 159: _LL2209: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2225=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2225[ 0]=({ struct Cyc_Type_tok_struct
_temp2226; _temp2226.tag= Cyc_Type_tok; _temp2226.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*)({ struct Cyc_Absyn_Unknown_kb_struct* _temp2227=( struct Cyc_Absyn_Unknown_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct)); _temp2227[ 0]=({ struct
Cyc_Absyn_Unknown_kb_struct _temp2228; _temp2228.tag= Cyc_Absyn_Unknown_kb;
_temp2228.f1= 0; _temp2228;}); _temp2227;})); _temp2226;}); _temp2225;}); break;
case 160: _LL2224: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2230=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2230[ 0]=({ struct Cyc_Type_tok_struct _temp2231; _temp2231.tag= Cyc_Type_tok;
_temp2231.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2232=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2232[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2233; _temp2233.tag= Cyc_Absyn_Eq_kb; _temp2233.f1=( void*) Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2233;});
_temp2232;})); _temp2231;}); _temp2230;}); break; case 161: _LL2229: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2235=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2235[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2236; _temp2236.tag= Cyc_TypeOpt_tok; _temp2236.f1= 0; _temp2236;});
_temp2235;}); break; case 162: _LL2234: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2238=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2238[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2239; _temp2239.tag= Cyc_TypeOpt_tok;
_temp2239.f1=({ struct Cyc_Core_Opt* _temp2240=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2240->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2241=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2241[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2242; _temp2242.tag= Cyc_Absyn_JoinEff;
_temp2242.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2242;}); _temp2241;})); _temp2240;}); _temp2239;});
_temp2238;}); break; case 163: _LL2237: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2244=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2244[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2245; _temp2245.tag= Cyc_Rgnorder_tok;
_temp2245.f1= 0; _temp2245;}); _temp2244;}); break; case 164: _LL2243: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 165:
_LL2246: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2248=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2248[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2249; _temp2249.tag= Cyc_Rgnorder_tok; _temp2249.f1=({ struct Cyc_List_List*
_temp2250=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2250->hd=( void*)({ struct _tuple18* _temp2251=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2251->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2254=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2254[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2255; _temp2255.tag= Cyc_Absyn_Eq_kb; _temp2255.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2255;}); _temp2254;})); _temp2251->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2252=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2252[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2253; _temp2253.tag= Cyc_Absyn_Eq_kb; _temp2253.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2253;}); _temp2252;})); _temp2251;}); _temp2250->tl= 0; _temp2250;});
_temp2249;}); _temp2248;}); break; case 166: _LL2247: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2257=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2257[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2258; _temp2258.tag= Cyc_Rgnorder_tok; _temp2258.f1=({ struct Cyc_List_List*
_temp2259=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2259->hd=( void*)({ struct _tuple18* _temp2260=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2260->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2263=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2263[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2264; _temp2264.tag= Cyc_Absyn_Eq_kb; _temp2264.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2264;}); _temp2263;})); _temp2260->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2261=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2261[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2262; _temp2262.tag= Cyc_Absyn_Eq_kb; _temp2262.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2262;}); _temp2261;})); _temp2260;}); _temp2259->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2259;});
_temp2258;}); _temp2257;}); break; case 167: _LL2256: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2266=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2266[ 0]=({ struct Cyc_Bool_tok_struct
_temp2267; _temp2267.tag= Cyc_Bool_tok; _temp2267.f1= 0; _temp2267;}); _temp2266;});
break; case 168: _LL2265: { struct _tagged_arr _temp2269= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Std_zstrcmp(
_temp2269, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2270=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2270[ 0]=({ struct Cyc_Bool_tok_struct _temp2271; _temp2271.tag= Cyc_Bool_tok;
_temp2271.f1= 1; _temp2271;}); _temp2270;}); break;} case 169: _LL2268: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 170:
_LL2272: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2274=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2274[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2275; _temp2275.tag= Cyc_TypeList_tok; _temp2275.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2275;});
_temp2274;}); break; case 171: _LL2273: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2277=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2277[ 0]=({ struct Cyc_TypeList_tok_struct _temp2278; _temp2278.tag= Cyc_TypeList_tok;
_temp2278.f1= 0; _temp2278;}); _temp2277;}); break; case 172: _LL2276: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case
173: _LL2279: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2281=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2281[ 0]=({ struct Cyc_TypeList_tok_struct _temp2282; _temp2282.tag= Cyc_TypeList_tok;
_temp2282.f1=({ struct Cyc_List_List* _temp2283=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2283->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp2284=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp2284[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp2285; _temp2285.tag= Cyc_Absyn_RgnsEff; _temp2285.f1=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2285;}); _temp2284;})); _temp2283->tl= 0; _temp2283;});
_temp2282;}); _temp2281;}); break; case 174: _LL2280: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2287=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2287[ 0]=({ struct Cyc_TypeList_tok_struct _temp2288; _temp2288.tag= Cyc_TypeList_tok;
_temp2288.f1=({ struct Cyc_List_List* _temp2289=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2289->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2289->tl= 0;
_temp2289;}); _temp2288;}); _temp2287;}); break; case 175: _LL2286: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2291=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2291[ 0]=({ struct Cyc_TypeList_tok_struct _temp2292; _temp2292.tag= Cyc_TypeList_tok;
_temp2292.f1=({ struct Cyc_List_List* _temp2293=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2293->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2294=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2294[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2295; _temp2295.tag= Cyc_Absyn_AccessEff;
_temp2295.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2295;}); _temp2294;})); _temp2293->tl= 0;
_temp2293;}); _temp2292;}); _temp2291;}); break; case 176: _LL2290: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),( void*) Cyc_Absyn_RgnKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2297=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2297[ 0]=({ struct Cyc_TypeList_tok_struct _temp2298; _temp2298.tag= Cyc_TypeList_tok;
_temp2298.f1=({ struct Cyc_List_List* _temp2299=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2299->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2300=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2300[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2301; _temp2301.tag= Cyc_Absyn_AccessEff;
_temp2301.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2301;}); _temp2300;})); _temp2299->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2299;});
_temp2298;}); _temp2297;}); break; case 177: _LL2296: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2303=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2303[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2304; _temp2304.tag= Cyc_ParamDeclList_tok;
_temp2304.f1=({ struct Cyc_List_List* _temp2305=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2305->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2305->tl= 0;
_temp2305;}); _temp2304;}); _temp2303;}); break; case 178: _LL2302: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2307=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2307[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2308; _temp2308.tag= Cyc_ParamDeclList_tok;
_temp2308.f1=({ struct Cyc_List_List* _temp2309=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2309->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2309->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2309;}); _temp2308;}); _temp2307;}); break; case 179:
_LL2306: { struct _tuple17 _temp2313; struct Cyc_List_List* _temp2314; struct
Cyc_List_List* _temp2316; struct Cyc_Absyn_Tqual _temp2318; struct _tuple17*
_temp2311= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2313=* _temp2311; _LL2319: _temp2318=
_temp2313.f1; goto _LL2317; _LL2317: _temp2316= _temp2313.f2; goto _LL2315;
_LL2315: _temp2314= _temp2313.f3; goto _LL2312; _LL2312: { struct Cyc_Parse_Declarator
_temp2322; struct Cyc_List_List* _temp2323; struct _tuple1* _temp2325; struct
Cyc_Parse_Declarator* _temp2320= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2322=* _temp2320;
_LL2326: _temp2325= _temp2322.id; goto _LL2324; _LL2324: _temp2323= _temp2322.tms;
goto _LL2321; _LL2321: { void* _temp2327= Cyc_Parse_speclist2typ( _temp2316, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2330; struct Cyc_List_List* _temp2332; void*
_temp2334; struct Cyc_Absyn_Tqual _temp2336; struct _tuple6 _temp2328= Cyc_Parse_apply_tms(
_temp2318, _temp2327, _temp2314, _temp2323); _LL2337: _temp2336= _temp2328.f1;
goto _LL2335; _LL2335: _temp2334= _temp2328.f2; goto _LL2333; _LL2333: _temp2332=
_temp2328.f3; goto _LL2331; _LL2331: _temp2330= _temp2328.f4; goto _LL2329;
_LL2329: if( _temp2332 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2325)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2338=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2342=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2342->v=( void*)(* _temp2325).f2; _temp2342;}); if( _temp2330 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2339=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2339[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2340; _temp2340.tag= Cyc_ParamDecl_tok;
_temp2340.f1=({ struct _tuple2* _temp2341=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2341->f1= _temp2338; _temp2341->f2= _temp2336; _temp2341->f3=
_temp2334; _temp2341;}); _temp2340;}); _temp2339;}); break;}}}} case 180:
_LL2310: { struct _tuple17 _temp2346; struct Cyc_List_List* _temp2347; struct
Cyc_List_List* _temp2349; struct Cyc_Absyn_Tqual _temp2351; struct _tuple17*
_temp2344= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2346=* _temp2344; _LL2352: _temp2351= _temp2346.f1;
goto _LL2350; _LL2350: _temp2349= _temp2346.f2; goto _LL2348; _LL2348: _temp2347=
_temp2346.f3; goto _LL2345; _LL2345: { void* _temp2353= Cyc_Parse_speclist2typ(
_temp2349, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2347 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2354=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2354[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2355; _temp2355.tag= Cyc_ParamDecl_tok; _temp2355.f1=({
struct _tuple2* _temp2356=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2356->f1= 0; _temp2356->f2= _temp2351; _temp2356->f3= _temp2353; _temp2356;});
_temp2355;}); _temp2354;}); break;}} case 181: _LL2343: { struct _tuple17
_temp2360; struct Cyc_List_List* _temp2361; struct Cyc_List_List* _temp2363;
struct Cyc_Absyn_Tqual _temp2365; struct _tuple17* _temp2358= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2360=*
_temp2358; _LL2366: _temp2365= _temp2360.f1; goto _LL2364; _LL2364: _temp2363=
_temp2360.f2; goto _LL2362; _LL2362: _temp2361= _temp2360.f3; goto _LL2359;
_LL2359: { void* _temp2367= Cyc_Parse_speclist2typ( _temp2363, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2368=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2371; struct Cyc_List_List* _temp2373; void* _temp2375; struct Cyc_Absyn_Tqual
_temp2377; struct _tuple6 _temp2369= Cyc_Parse_apply_tms( _temp2365, _temp2367,
_temp2361, _temp2368); _LL2378: _temp2377= _temp2369.f1; goto _LL2376; _LL2376:
_temp2375= _temp2369.f2; goto _LL2374; _LL2374: _temp2373= _temp2369.f3; goto
_LL2372; _LL2372: _temp2371= _temp2369.f4; goto _LL2370; _LL2370: if( _temp2373
!=  0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2371 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2379=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2379[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2380; _temp2380.tag= Cyc_ParamDecl_tok; _temp2380.f1=({
struct _tuple2* _temp2381=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2381->f1= 0; _temp2381->f2= _temp2377; _temp2381->f3= _temp2375; _temp2381;});
_temp2380;}); _temp2379;}); break;}} case 182: _LL2357: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2383=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2383[ 0]=({ struct Cyc_IdList_tok_struct
_temp2384; _temp2384.tag= Cyc_IdList_tok; _temp2384.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2384;});
_temp2383;}); break; case 183: _LL2382: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2386=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2386[ 0]=({ struct Cyc_IdList_tok_struct _temp2387; _temp2387.tag= Cyc_IdList_tok;
_temp2387.f1=({ struct Cyc_List_List* _temp2388=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2388->hd=( void*)({ struct
_tagged_arr* _temp2389=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2389[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2389;}); _temp2388->tl=
0; _temp2388;}); _temp2387;}); _temp2386;}); break; case 184: _LL2385: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2391=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2391[ 0]=({ struct Cyc_IdList_tok_struct
_temp2392; _temp2392.tag= Cyc_IdList_tok; _temp2392.f1=({ struct Cyc_List_List*
_temp2393=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2393->hd=( void*)({ struct _tagged_arr* _temp2394=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2394[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2394;});
_temp2393->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2393;}); _temp2392;}); _temp2391;}); break; case 185:
_LL2390: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 186: _LL2395: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 187: _LL2396: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2398=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2398[ 0]=({ struct Cyc_Exp_tok_struct _temp2399; _temp2399.tag= Cyc_Exp_tok;
_temp2399.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2400=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2400[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2401; _temp2401.tag= Cyc_Absyn_UnresolvedMem_e; _temp2401.f1= 0; _temp2401.f2=
0; _temp2401;}); _temp2400;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2399;});
_temp2398;}); break; case 188: _LL2397: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2403=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2403[ 0]=({ struct Cyc_Exp_tok_struct _temp2404; _temp2404.tag= Cyc_Exp_tok;
_temp2404.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2405=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2405[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2406; _temp2406.tag= Cyc_Absyn_UnresolvedMem_e; _temp2406.f1= 0; _temp2406.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2406;});
_temp2405;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2404;}); _temp2403;}); break; case 189:
_LL2402: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2408=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2408[ 0]=({ struct Cyc_Exp_tok_struct
_temp2409; _temp2409.tag= Cyc_Exp_tok; _temp2409.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2410=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2410[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2411; _temp2411.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2411.f1= 0; _temp2411.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2411;});
_temp2410;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2409;}); _temp2408;}); break; case 190:
_LL2407: { struct Cyc_Absyn_Vardecl* _temp2413= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2419=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2419->f1=( void*) Cyc_Absyn_Loc_n; _temp2419->f2=({ struct _tagged_arr*
_temp2420=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp2420[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); _temp2420;}); _temp2419;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp2413->tq=({ struct Cyc_Absyn_Tqual _temp2414; _temp2414.q_const= 1;
_temp2414.q_volatile= 0; _temp2414.q_restrict= 1; _temp2414;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2415=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2415[ 0]=({ struct Cyc_Exp_tok_struct
_temp2416; _temp2416.tag= Cyc_Exp_tok; _temp2416.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2417=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2417[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2418; _temp2418.tag= Cyc_Absyn_Comprehension_e;
_temp2418.f1= _temp2413; _temp2418.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2418.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2418;});
_temp2417;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2416;}); _temp2415;}); break;} case
191: _LL2412: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2422=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2422[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2423; _temp2423.tag=
Cyc_InitializerList_tok; _temp2423.f1=({ struct Cyc_List_List* _temp2424=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2424->hd=(
void*)({ struct _tuple19* _temp2425=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2425->f1= 0; _temp2425->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2425;}); _temp2424->tl=
0; _temp2424;}); _temp2423;}); _temp2422;}); break; case 192: _LL2421: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2427=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2427[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2428; _temp2428.tag= Cyc_InitializerList_tok;
_temp2428.f1=({ struct Cyc_List_List* _temp2429=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2429->hd=( void*)({ struct
_tuple19* _temp2430=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2430->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2430->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2430;});
_temp2429->tl= 0; _temp2429;}); _temp2428;}); _temp2427;}); break; case 193:
_LL2426: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2432=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2432[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2433; _temp2433.tag=
Cyc_InitializerList_tok; _temp2433.f1=({ struct Cyc_List_List* _temp2434=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2434->hd=(
void*)({ struct _tuple19* _temp2435=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2435->f1= 0; _temp2435->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2435;}); _temp2434->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2434;}); _temp2433;}); _temp2432;}); break; case 194:
_LL2431: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2437=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2437[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2438; _temp2438.tag=
Cyc_InitializerList_tok; _temp2438.f1=({ struct Cyc_List_List* _temp2439=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2439->hd=(
void*)({ struct _tuple19* _temp2440=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2440->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2440->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2440;});
_temp2439->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2439;});
_temp2438;}); _temp2437;}); break; case 195: _LL2436: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2442=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2442[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2443; _temp2443.tag= Cyc_DesignatorList_tok;
_temp2443.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2443;});
_temp2442;}); break; case 196: _LL2441: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2445=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2445[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2446; _temp2446.tag=
Cyc_DesignatorList_tok; _temp2446.f1=({ struct Cyc_List_List* _temp2447=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2447->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2447->tl= 0; _temp2447;}); _temp2446;}); _temp2445;});
break; case 197: _LL2444: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2449=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2449[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2450; _temp2450.tag=
Cyc_DesignatorList_tok; _temp2450.f1=({ struct Cyc_List_List* _temp2451=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2451->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2451->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2451;});
_temp2450;}); _temp2449;}); break; case 198: _LL2448: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2453=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2453[ 0]=({ struct
Cyc_Designator_tok_struct _temp2454; _temp2454.tag= Cyc_Designator_tok;
_temp2454.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2455=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2455[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2456; _temp2456.tag=
Cyc_Absyn_ArrayElement; _temp2456.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2456;});
_temp2455;})); _temp2454;}); _temp2453;}); break; case 199: _LL2452: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2458=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2458[ 0]=({ struct
Cyc_Designator_tok_struct _temp2459; _temp2459.tag= Cyc_Designator_tok;
_temp2459.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2460=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2460[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2461; _temp2461.tag= Cyc_Absyn_FieldName;
_temp2461.f1=({ struct _tagged_arr* _temp2462=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2462[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2462;}); _temp2461;});
_temp2460;})); _temp2459;}); _temp2458;}); break; case 200: _LL2457: { void*
_temp2464= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2465=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2465 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2466=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2467=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2467[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2468; _temp2468.tag= Cyc_ParamDecl_tok; _temp2468.f1=({
struct _tuple2* _temp2469=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2469->f1= 0; _temp2469->f2= _temp2466; _temp2469->f3= _temp2464; _temp2469;});
_temp2468;}); _temp2467;}); break;}} case 201: _LL2463: { void* _temp2471= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp2472=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2473=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2474=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2475= Cyc_Parse_apply_tms( _temp2473, _temp2471, _temp2472,
_temp2474); if( _temp2475.f3 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2475.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2476=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2476[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2477; _temp2477.tag= Cyc_ParamDecl_tok; _temp2477.f1=({
struct _tuple2* _temp2478=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2478->f1= 0; _temp2478->f2= _temp2475.f1; _temp2478->f3= _temp2475.f2;
_temp2478;}); _temp2477;}); _temp2476;}); break;} case 202: _LL2470: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2480=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2480[ 0]=({ struct Cyc_Type_tok_struct
_temp2481; _temp2481.tag= Cyc_Type_tok; _temp2481.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2481;});
_temp2480;}); break; case 203: _LL2479: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2483=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2483[ 0]=({ struct Cyc_Type_tok_struct _temp2484; _temp2484.tag= Cyc_Type_tok;
_temp2484.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2485=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2485[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2486; _temp2486.tag= Cyc_Absyn_JoinEff;
_temp2486.f1= 0; _temp2486;}); _temp2485;})); _temp2484;}); _temp2483;}); break;
case 204: _LL2482: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2488=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2488[ 0]=({ struct Cyc_Type_tok_struct _temp2489; _temp2489.tag= Cyc_Type_tok;
_temp2489.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2490=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2490[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2491; _temp2491.tag= Cyc_Absyn_JoinEff;
_temp2491.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2491;}); _temp2490;})); _temp2489;});
_temp2488;}); break; case 205: _LL2487: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2493=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2493[ 0]=({ struct Cyc_Type_tok_struct _temp2494; _temp2494.tag= Cyc_Type_tok;
_temp2494.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2495=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2495[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2496; _temp2496.tag= Cyc_Absyn_RgnsEff;
_temp2496.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2496;}); _temp2495;})); _temp2494;});
_temp2493;}); break; case 206: _LL2492: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2498=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2498[ 0]=({ struct Cyc_Type_tok_struct _temp2499; _temp2499.tag= Cyc_Type_tok;
_temp2499.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2500=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2500[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2501; _temp2501.tag= Cyc_Absyn_JoinEff;
_temp2501.f1=({ struct Cyc_List_List* _temp2502=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2502->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2502->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2502;}); _temp2501;}); _temp2500;})); _temp2499;});
_temp2498;}); break; case 207: _LL2497: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2504=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2504[ 0]=({ struct Cyc_TypeList_tok_struct _temp2505; _temp2505.tag= Cyc_TypeList_tok;
_temp2505.f1=({ struct Cyc_List_List* _temp2506=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2506->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2506->tl= 0;
_temp2506;}); _temp2505;}); _temp2504;}); break; case 208: _LL2503: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2508=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2508[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2509; _temp2509.tag= Cyc_TypeList_tok; _temp2509.f1=({ struct Cyc_List_List*
_temp2510=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2510->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2510->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2510;});
_temp2509;}); _temp2508;}); break; case 209: _LL2507: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2512=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2512[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2513; _temp2513.tag= Cyc_AbstractDeclarator_tok;
_temp2513.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2514=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2514->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2514;});
_temp2513;}); _temp2512;}); break; case 210: _LL2511: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 211: _LL2515:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2517=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2517[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2518; _temp2518.tag=
Cyc_AbstractDeclarator_tok; _temp2518.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2519=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2519->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2519;});
_temp2518;}); _temp2517;}); break; case 212: _LL2516: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 213:
_LL2520: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2522=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2522[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2523; _temp2523.tag=
Cyc_AbstractDeclarator_tok; _temp2523.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2524=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2524->tms=({ struct Cyc_List_List* _temp2525=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2525->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2525->tl= 0; _temp2525;}); _temp2524;}); _temp2523;}); _temp2522;}); break;
case 214: _LL2521: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2527=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2527[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2528; _temp2528.tag= Cyc_AbstractDeclarator_tok; _temp2528.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2529=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2529->tms=({ struct Cyc_List_List* _temp2530=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2530->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2530->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2530;});
_temp2529;}); _temp2528;}); _temp2527;}); break; case 215: _LL2526: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2532=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2532[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2533; _temp2533.tag= Cyc_AbstractDeclarator_tok;
_temp2533.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2534=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2534->tms=({
struct Cyc_List_List* _temp2535=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2535->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2536=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2536[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2537; _temp2537.tag= Cyc_Absyn_ConstArray_mod; _temp2537.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2537;});
_temp2536;})); _temp2535->tl= 0; _temp2535;}); _temp2534;}); _temp2533;});
_temp2532;}); break; case 216: _LL2531: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2539=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2539[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2540; _temp2540.tag= Cyc_AbstractDeclarator_tok; _temp2540.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2541=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2541->tms=({ struct Cyc_List_List* _temp2542=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2542->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2543=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2543[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2544; _temp2544.tag= Cyc_Absyn_ConstArray_mod;
_temp2544.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2544;}); _temp2543;})); _temp2542->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2542;}); _temp2541;}); _temp2540;}); _temp2539;}); break; case 217: _LL2538:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2546=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2546[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2547; _temp2547.tag=
Cyc_AbstractDeclarator_tok; _temp2547.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2548=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2548->tms=({ struct Cyc_List_List* _temp2549=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2549->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2550=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2550[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2551; _temp2551.tag= Cyc_Absyn_Function_mod;
_temp2551.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2552=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2552[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2553; _temp2553.tag= Cyc_Absyn_WithTypes;
_temp2553.f1= 0; _temp2553.f2= 0; _temp2553.f3= 0; _temp2553.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2553.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2553;}); _temp2552;})); _temp2551;}); _temp2550;}));
_temp2549->tl= 0; _temp2549;}); _temp2548;}); _temp2547;}); _temp2546;}); break;
case 218: _LL2545: { struct _tuple16 _temp2557; struct Cyc_List_List* _temp2558;
struct Cyc_Core_Opt* _temp2560; struct Cyc_Absyn_VarargInfo* _temp2562; int
_temp2564; struct Cyc_List_List* _temp2566; struct _tuple16* _temp2555= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2557=*
_temp2555; _LL2567: _temp2566= _temp2557.f1; goto _LL2565; _LL2565: _temp2564=
_temp2557.f2; goto _LL2563; _LL2563: _temp2562= _temp2557.f3; goto _LL2561;
_LL2561: _temp2560= _temp2557.f4; goto _LL2559; _LL2559: _temp2558= _temp2557.f5;
goto _LL2556; _LL2556: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2568=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2568[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2569; _temp2569.tag= Cyc_AbstractDeclarator_tok; _temp2569.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2570=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2570->tms=({ struct Cyc_List_List* _temp2571=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2571->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2572=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2572[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2573; _temp2573.tag= Cyc_Absyn_Function_mod;
_temp2573.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2574=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2574[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2575; _temp2575.tag= Cyc_Absyn_WithTypes;
_temp2575.f1= _temp2566; _temp2575.f2= _temp2564; _temp2575.f3= _temp2562;
_temp2575.f4= _temp2560; _temp2575.f5= _temp2558; _temp2575;}); _temp2574;}));
_temp2573;}); _temp2572;})); _temp2571->tl= 0; _temp2571;}); _temp2570;});
_temp2569;}); _temp2568;}); break;} case 219: _LL2554: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2577=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2577[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2578; _temp2578.tag= Cyc_AbstractDeclarator_tok;
_temp2578.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2579=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2579->tms=({
struct Cyc_List_List* _temp2580=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2580->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2581=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2581[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2582; _temp2582.tag=
Cyc_Absyn_Function_mod; _temp2582.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2583=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2583[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2584; _temp2584.tag= Cyc_Absyn_WithTypes;
_temp2584.f1= 0; _temp2584.f2= 0; _temp2584.f3= 0; _temp2584.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2584.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2584;}); _temp2583;})); _temp2582;}); _temp2581;}));
_temp2580->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2580;});
_temp2579;}); _temp2578;}); _temp2577;}); break; case 220: _LL2576: { struct
_tuple16 _temp2588; struct Cyc_List_List* _temp2589; struct Cyc_Core_Opt*
_temp2591; struct Cyc_Absyn_VarargInfo* _temp2593; int _temp2595; struct Cyc_List_List*
_temp2597; struct _tuple16* _temp2586= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2588=*
_temp2586; _LL2598: _temp2597= _temp2588.f1; goto _LL2596; _LL2596: _temp2595=
_temp2588.f2; goto _LL2594; _LL2594: _temp2593= _temp2588.f3; goto _LL2592;
_LL2592: _temp2591= _temp2588.f4; goto _LL2590; _LL2590: _temp2589= _temp2588.f5;
goto _LL2587; _LL2587: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2599=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2599[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2600; _temp2600.tag= Cyc_AbstractDeclarator_tok; _temp2600.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2601=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2601->tms=({ struct Cyc_List_List* _temp2602=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2602->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2603=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2603[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2604; _temp2604.tag= Cyc_Absyn_Function_mod;
_temp2604.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2605=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2605[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2606; _temp2606.tag= Cyc_Absyn_WithTypes;
_temp2606.f1= _temp2597; _temp2606.f2= _temp2595; _temp2606.f3= _temp2593;
_temp2606.f4= _temp2591; _temp2606.f5= _temp2589; _temp2606;}); _temp2605;}));
_temp2604;}); _temp2603;})); _temp2602->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2602;}); _temp2601;}); _temp2600;}); _temp2599;}); break;} case 221:
_LL2585: { struct Cyc_List_List* _temp2608=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
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
_LL2607:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2616=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2617=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2617[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2618; _temp2618.tag= Cyc_AbstractDeclarator_tok; _temp2618.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2619=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2619->tms=({ struct Cyc_List_List* _temp2620=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2620->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2621=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2621[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2622; _temp2622.tag= Cyc_Absyn_TypeParams_mod;
_temp2622.f1= _temp2616; _temp2622.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2622.f3=
0; _temp2622;}); _temp2621;})); _temp2620->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2620;}); _temp2619;}); _temp2618;}); _temp2617;}); break;} case 223:
_LL2615: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2624=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2624[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2625; _temp2625.tag=
Cyc_AbstractDeclarator_tok; _temp2625.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2626=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2626->tms=({ struct Cyc_List_List* _temp2627=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2627->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2628=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2628[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2629; _temp2629.tag= Cyc_Absyn_Attributes_mod;
_temp2629.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2629.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2629;}); _temp2628;}));
_temp2627->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms; _temp2627;});
_temp2626;}); _temp2625;}); _temp2624;}); break; case 224: _LL2623: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 225: _LL2630:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
226: _LL2631: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 227: _LL2632: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 228: _LL2633: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 229: _LL2634:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
230: _LL2635: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2647=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2647->name=({ struct
_tagged_arr* _temp2650=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2650[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2650;});
_temp2647->identity= 0; _temp2647->kind=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp2648=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp2648[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp2649; _temp2649.tag= Cyc_Absyn_Eq_kb;
_temp2649.f1=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2649;}); _temp2648;}));
_temp2647;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2645=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2645[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2646; _temp2646.tag= Cyc_Absyn_VarType;
_temp2646.f1= tv; _temp2646;}); _temp2645;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2637=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2637[ 0]=({ struct Cyc_Stmt_tok_struct _temp2638; _temp2638.tag= Cyc_Stmt_tok;
_temp2638.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2639=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2639[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2640; _temp2640.tag= Cyc_Absyn_Region_s;
_temp2640.f1= tv; _temp2640.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2641=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2641->f1=(
void*) Cyc_Absyn_Loc_n; _temp2641->f2=({ struct _tagged_arr* _temp2642=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2642[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2642;});
_temp2641;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2643=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2643[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2644; _temp2644.tag=
Cyc_Absyn_RgnHandleType; _temp2644.f1=( void*) t; _temp2644;}); _temp2643;}), 0);
_temp2640.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2640;}); _temp2639;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2638;});
_temp2637;}); break;} case 231: _LL2636: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2662=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2662->name=({ struct
_tagged_arr* _temp2665=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2665[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2667; _temp2667.tag= Cyc_Std_String_pa; _temp2667.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2666[ 1u]={& _temp2667}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2666, sizeof( void*), 1u));}});
_temp2665;}); _temp2662->identity= 0; _temp2662->kind=( void*)(( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2663=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2663[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2664; _temp2664.tag= Cyc_Absyn_Eq_kb; _temp2664.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2664;}); _temp2663;})); _temp2662;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2660=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2660[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2661; _temp2661.tag= Cyc_Absyn_VarType;
_temp2661.f1= tv; _temp2661;}); _temp2660;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2652=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2652[ 0]=({ struct Cyc_Stmt_tok_struct _temp2653; _temp2653.tag= Cyc_Stmt_tok;
_temp2653.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2654=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2654[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2655; _temp2655.tag= Cyc_Absyn_Region_s;
_temp2655.f1= tv; _temp2655.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2656=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2656->f1=(
void*) Cyc_Absyn_Loc_n; _temp2656->f2=({ struct _tagged_arr* _temp2657=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2657[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2657;});
_temp2656;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2658=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2658[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2659; _temp2659.tag=
Cyc_Absyn_RgnHandleType; _temp2659.f1=( void*) t; _temp2659;}); _temp2658;}), 0);
_temp2655.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2655;}); _temp2654;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2653;});
_temp2652;}); break;} case 232: _LL2651: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2669=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2669[ 0]=({ struct Cyc_Stmt_tok_struct _temp2670; _temp2670.tag= Cyc_Stmt_tok;
_temp2670.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2671=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2671[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2672; _temp2672.tag= Cyc_Absyn_Cut_s;
_temp2672.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2672;}); _temp2671;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2670;});
_temp2669;}); break; case 233: _LL2668: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2674=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2674[ 0]=({ struct Cyc_Stmt_tok_struct _temp2675; _temp2675.tag= Cyc_Stmt_tok;
_temp2675.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2676=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2676[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2677; _temp2677.tag= Cyc_Absyn_Splice_s;
_temp2677.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2677;}); _temp2676;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2675;});
_temp2674;}); break; case 234: _LL2673: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2679=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2679[ 0]=({ struct Cyc_Stmt_tok_struct _temp2680; _temp2680.tag= Cyc_Stmt_tok;
_temp2680.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2681=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2681[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2682; _temp2682.tag= Cyc_Absyn_Label_s;
_temp2682.f1=({ struct _tagged_arr* _temp2683=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2683[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2683;});
_temp2682.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2682;}); _temp2681;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2680;});
_temp2679;}); break; case 235: _LL2678: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2685=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2685[ 0]=({ struct Cyc_Stmt_tok_struct _temp2686; _temp2686.tag= Cyc_Stmt_tok;
_temp2686.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2686;});
_temp2685;}); break; case 236: _LL2684: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2688=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2688[ 0]=({ struct Cyc_Stmt_tok_struct _temp2689; _temp2689.tag= Cyc_Stmt_tok;
_temp2689.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2689;}); _temp2688;}); break; case 237: _LL2687: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2691=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2691[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2692; _temp2692.tag= Cyc_Stmt_tok; _temp2692.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2692;}); _temp2691;}); break; case 238: _LL2690: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 239:
_LL2693: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2695=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2695[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2696; _temp2696.tag= Cyc_Stmt_tok; _temp2696.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2696;}); _temp2695;}); break;
case 240: _LL2694: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2698=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2698[ 0]=({ struct Cyc_Stmt_tok_struct _temp2699; _temp2699.tag= Cyc_Stmt_tok;
_temp2699.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2699;});
_temp2698;}); break; case 241: _LL2697: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 242: _LL2700:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2702=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2702[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2703; _temp2703.tag= Cyc_Stmt_tok; _temp2703.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2703;}); _temp2702;}); break; case 243: _LL2701: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2705=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2705[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2706; _temp2706.tag= Cyc_Stmt_tok; _temp2706.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2707=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2707[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2708; _temp2708.tag= Cyc_Absyn_Fn_d;
_temp2708.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2708;}); _temp2707;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2706;}); _temp2705;}); break; case 244: _LL2704: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2710=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2710[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2711; _temp2711.tag= Cyc_Stmt_tok; _temp2711.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2712=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2712[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2713; _temp2713.tag= Cyc_Absyn_Fn_d;
_temp2713.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2713;}); _temp2712;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2711;}); _temp2710;}); break; case 245: _LL2709: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2715=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2715[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2716; _temp2716.tag= Cyc_Stmt_tok; _temp2716.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2716;});
_temp2715;}); break; case 246: _LL2714: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2718=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2718[ 0]=({ struct Cyc_Stmt_tok_struct _temp2719; _temp2719.tag= Cyc_Stmt_tok;
_temp2719.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2719;}); _temp2718;}); break; case 247: _LL2717: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2721=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2721[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2722; _temp2722.tag= Cyc_Stmt_tok; _temp2722.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2722;}); _temp2721;}); break; case 248: _LL2720: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2724=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2724[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2725; _temp2725.tag= Cyc_Stmt_tok; _temp2725.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2726=( struct Cyc_Absyn_SwitchC_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2726[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2727; _temp2727.tag= Cyc_Absyn_SwitchC_s;
_temp2727.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2727.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2727;});
_temp2726;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2725;}); _temp2724;}); break; case 249:
_LL2723: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2729=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2729[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2730; _temp2730.tag= Cyc_Stmt_tok; _temp2730.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2730;});
_temp2729;}); break; case 250: _LL2728: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2732=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2732[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2733; _temp2733.tag= Cyc_SwitchClauseList_tok; _temp2733.f1= 0; _temp2733;});
_temp2732;}); break; case 251: _LL2731: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2735=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2735[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2736; _temp2736.tag= Cyc_SwitchClauseList_tok; _temp2736.f1=({ struct Cyc_List_List*
_temp2737=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2737->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2738=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2738->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2738->pat_vars= 0; _temp2738->where_clause= 0; _temp2738->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2738->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2738;}); _temp2737->tl= 0; _temp2737;});
_temp2736;}); _temp2735;}); break; case 252: _LL2734: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2740=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2740[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2741; _temp2741.tag= Cyc_SwitchClauseList_tok;
_temp2741.f1=({ struct Cyc_List_List* _temp2742=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2742->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2743=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2743->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2743->pat_vars= 0; _temp2743->where_clause= 0;
_temp2743->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2743->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2743;});
_temp2742->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2742;}); _temp2741;});
_temp2740;}); break; case 253: _LL2739: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2745=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2745[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2746; _temp2746.tag= Cyc_SwitchClauseList_tok; _temp2746.f1=({ struct Cyc_List_List*
_temp2747=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2747->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2748=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2748->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2748->pat_vars=
0; _temp2748->where_clause= 0; _temp2748->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2748->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2748;}); _temp2747->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2747;});
_temp2746;}); _temp2745;}); break; case 254: _LL2744: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2750=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2750[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2751; _temp2751.tag= Cyc_SwitchClauseList_tok;
_temp2751.f1=({ struct Cyc_List_List* _temp2752=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2752->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2753=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2753->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp2753->pat_vars= 0; _temp2753->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2753->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2753->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2753;});
_temp2752->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2752;}); _temp2751;});
_temp2750;}); break; case 255: _LL2749: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2755=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2755[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2756; _temp2756.tag= Cyc_SwitchClauseList_tok; _temp2756.f1=({ struct Cyc_List_List*
_temp2757=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2757->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2758=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2758->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2758->pat_vars=
0; _temp2758->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2758->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2758->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2758;});
_temp2757->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2757;}); _temp2756;});
_temp2755;}); break; case 256: _LL2754: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2760=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2760[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2761; _temp2761.tag= Cyc_SwitchCClauseList_tok; _temp2761.f1= 0; _temp2761;});
_temp2760;}); break; case 257: _LL2759: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2763=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2763[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2764; _temp2764.tag= Cyc_SwitchCClauseList_tok; _temp2764.f1=({ struct Cyc_List_List*
_temp2765=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2765->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2766=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2766->cnst_exp= 0;
_temp2766->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2766->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2766;});
_temp2765->tl= 0; _temp2765;}); _temp2764;}); _temp2763;}); break; case 258:
_LL2762: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2768=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2768[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2769; _temp2769.tag=
Cyc_SwitchCClauseList_tok; _temp2769.f1=({ struct Cyc_List_List* _temp2770=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2770->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2771=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2771->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2771->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2771->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2771;});
_temp2770->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2770;}); _temp2769;});
_temp2768;}); break; case 259: _LL2767: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2773=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2773[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2774; _temp2774.tag= Cyc_SwitchCClauseList_tok; _temp2774.f1=({ struct Cyc_List_List*
_temp2775=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2775->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2776=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2776->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2776->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2776->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2776;});
_temp2775->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2775;}); _temp2774;});
_temp2773;}); break; case 260: _LL2772: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2778=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2778[ 0]=({ struct Cyc_Stmt_tok_struct _temp2779; _temp2779.tag= Cyc_Stmt_tok;
_temp2779.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2779;}); _temp2778;}); break; case 261: _LL2777: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2781=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2781[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2782; _temp2782.tag= Cyc_Stmt_tok; _temp2782.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2782;}); _temp2781;}); break; case 262: _LL2780: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2784=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2784[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2785; _temp2785.tag= Cyc_Stmt_tok; _temp2785.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2785;}); _temp2784;}); break; case 263: _LL2783: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2787=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2787[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2788; _temp2788.tag= Cyc_Stmt_tok; _temp2788.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2788;}); _temp2787;}); break; case 264: _LL2786: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2790=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2790[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2791; _temp2791.tag= Cyc_Stmt_tok; _temp2791.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2791;}); _temp2790;}); break; case 265: _LL2789: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2793=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2793[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2794; _temp2794.tag= Cyc_Stmt_tok; _temp2794.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2794;}); _temp2793;}); break; case 266: _LL2792: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2796=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2796[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2797; _temp2797.tag= Cyc_Stmt_tok; _temp2797.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2797;}); _temp2796;}); break; case 267: _LL2795: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2799=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2799[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2800; _temp2800.tag= Cyc_Stmt_tok; _temp2800.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2800;});
_temp2799;}); break; case 268: _LL2798: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2802=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2802[ 0]=({ struct Cyc_Stmt_tok_struct _temp2803; _temp2803.tag= Cyc_Stmt_tok;
_temp2803.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2803;});
_temp2802;}); break; case 269: _LL2801: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2805=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2805[ 0]=({ struct Cyc_Stmt_tok_struct _temp2806; _temp2806.tag= Cyc_Stmt_tok;
_temp2806.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2806;}); _temp2805;}); break; case 270: _LL2804: { struct Cyc_List_List*
_temp2808= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt* _temp2809= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2810=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2810[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2811; _temp2811.tag= Cyc_Stmt_tok; _temp2811.f1= Cyc_Parse_flatten_declarations(
_temp2808, _temp2809); _temp2811;}); _temp2810;}); break;} case 271: _LL2807: {
struct Cyc_List_List* _temp2813= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2814= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2815=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2815[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2816; _temp2816.tag= Cyc_Stmt_tok; _temp2816.f1= Cyc_Parse_flatten_declarations(
_temp2813, _temp2814); _temp2816;}); _temp2815;}); break;} case 272: _LL2812: {
struct Cyc_List_List* _temp2818= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2819= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2820=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2820[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2821; _temp2821.tag= Cyc_Stmt_tok; _temp2821.f1= Cyc_Parse_flatten_declarations(
_temp2818, _temp2819); _temp2821;}); _temp2820;}); break;} case 273: _LL2817: {
struct Cyc_List_List* _temp2823= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2824= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2825=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2825[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2826; _temp2826.tag= Cyc_Stmt_tok; _temp2826.f1= Cyc_Parse_flatten_declarations(
_temp2823, _temp2824); _temp2826;}); _temp2825;}); break;} case 274: _LL2822:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2828=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2828[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2829; _temp2829.tag= Cyc_Stmt_tok; _temp2829.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2830=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2830[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2830;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2829;}); _temp2828;}); break; case 275: _LL2827: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2832=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2832[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2833; _temp2833.tag= Cyc_Stmt_tok; _temp2833.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2833;}); _temp2832;}); break; case 276:
_LL2831: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2835=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2835[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2836; _temp2836.tag= Cyc_Stmt_tok; _temp2836.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2836;}); _temp2835;}); break; case 277: _LL2834: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2838=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2838[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2839; _temp2839.tag= Cyc_Stmt_tok; _temp2839.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2839;}); _temp2838;}); break; case 278:
_LL2837: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2841=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2841[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2842; _temp2842.tag= Cyc_Stmt_tok; _temp2842.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2842;}); _temp2841;}); break; case 279: _LL2840: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2844=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2844[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2845; _temp2845.tag= Cyc_Stmt_tok; _temp2845.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2845;}); _temp2844;}); break; case 280:
_LL2843: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2847=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2847[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2848; _temp2848.tag= Cyc_Stmt_tok; _temp2848.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2848;}); _temp2847;}); break; case 281:
_LL2846: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2850=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2850[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2851; _temp2851.tag= Cyc_Stmt_tok; _temp2851.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2851;}); _temp2850;}); break; case 282: _LL2849: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2853=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2853[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2854; _temp2854.tag= Cyc_Pattern_tok; _temp2854.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2854;});
_temp2853;}); break; case 283: _LL2852: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 284:
_LL2855: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2857=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2857[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2858; _temp2858.tag= Cyc_Pattern_tok; _temp2858.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2859=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2859[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2860; _temp2860.tag= Cyc_Absyn_Int_p; _temp2860.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2860.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2860;}); _temp2859;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2858;});
_temp2857;}); break; case 285: _LL2856: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2862=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2862[ 0]=({ struct Cyc_Pattern_tok_struct _temp2863; _temp2863.tag= Cyc_Pattern_tok;
_temp2863.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2864=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2864[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2865; _temp2865.tag= Cyc_Absyn_Int_p;
_temp2865.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2865.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2865;});
_temp2864;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2863;}); _temp2862;}); break; case 286:
_LL2861: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2867=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2867[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2868; _temp2868.tag= Cyc_Pattern_tok; _temp2868.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2869=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2869[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2870; _temp2870.tag= Cyc_Absyn_Float_p; _temp2870.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2870;}); _temp2869;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2868;});
_temp2867;}); break; case 287: _LL2866: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2872=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2872[ 0]=({ struct Cyc_Pattern_tok_struct _temp2873; _temp2873.tag= Cyc_Pattern_tok;
_temp2873.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2874=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2874[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2875; _temp2875.tag= Cyc_Absyn_Char_p; _temp2875.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2875;});
_temp2874;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2873;}); _temp2872;}); break; case 288:
_LL2871: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2877=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2877[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2878; _temp2878.tag= Cyc_Pattern_tok; _temp2878.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2878;});
_temp2877;}); break; case 289: _LL2876: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2880=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2880[ 0]=({ struct Cyc_Pattern_tok_struct _temp2881; _temp2881.tag= Cyc_Pattern_tok;
_temp2881.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2882=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2882[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2883; _temp2883.tag=
Cyc_Absyn_UnknownId_p; _temp2883.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2883;}); _temp2882;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2881;}); _temp2880;}); break; case 290:
_LL2879: { struct Cyc_List_List* _temp2885=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2886=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2886[ 0]=({ struct Cyc_Pattern_tok_struct _temp2887; _temp2887.tag= Cyc_Pattern_tok;
_temp2887.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2888=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2888[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2889; _temp2889.tag= Cyc_Absyn_UnknownCall_p; _temp2889.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2889.f2=
_temp2885; _temp2889.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2889;});
_temp2888;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2887;}); _temp2886;}); break;} case
291: _LL2884: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2891=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2891[ 0]=({ struct Cyc_Pattern_tok_struct _temp2892; _temp2892.tag= Cyc_Pattern_tok;
_temp2892.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2893=( struct Cyc_Absyn_Tuple_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2893[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2894; _temp2894.tag= Cyc_Absyn_Tuple_p;
_temp2894.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2894;}); _temp2893;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2892;}); _temp2891;}); break; case 292: _LL2890: { struct Cyc_List_List*
_temp2896=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); yyval=(
void*)({ struct Cyc_Pattern_tok_struct* _temp2897=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2897[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2898; _temp2898.tag= Cyc_Pattern_tok; _temp2898.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2899=( struct Cyc_Absyn_UnknownFields_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2899[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2900; _temp2900.tag= Cyc_Absyn_UnknownFields_p;
_temp2900.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2900.f2= _temp2896; _temp2900.f3= 0; _temp2900;});
_temp2899;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2898;}); _temp2897;}); break;} case
293: _LL2895: { struct Cyc_List_List* _temp2902=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2903=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2903[ 0]=({ struct Cyc_Pattern_tok_struct _temp2904; _temp2904.tag= Cyc_Pattern_tok;
_temp2904.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2905=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2905[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2906; _temp2906.tag= Cyc_Absyn_UnknownFields_p; _temp2906.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2906.f2=
_temp2902; _temp2906.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2906;});
_temp2905;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2904;}); _temp2903;}); break;} case
294: _LL2901: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2908=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2908[ 0]=({ struct Cyc_Pattern_tok_struct _temp2909; _temp2909.tag= Cyc_Pattern_tok;
_temp2909.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2910=( struct Cyc_Absyn_Pointer_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2910[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2911; _temp2911.tag= Cyc_Absyn_Pointer_p;
_temp2911.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2911;}); _temp2910;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2909;});
_temp2908;}); break; case 295: _LL2907: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2913=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2913[ 0]=({ struct Cyc_Pattern_tok_struct _temp2914; _temp2914.tag= Cyc_Pattern_tok;
_temp2914.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2915=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2915[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2916; _temp2916.tag=
Cyc_Absyn_Reference_p; _temp2916.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2917=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2917->f1=(
void*) Cyc_Absyn_Loc_n; _temp2917->f2=({ struct _tagged_arr* _temp2918=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2918[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2918;});
_temp2917;}),( void*) Cyc_Absyn_VoidType, 0); _temp2916;}); _temp2915;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2914;}); _temp2913;}); break; case 296: _LL2912: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2920=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2920[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2921; _temp2921.tag= Cyc_PatternList_tok;
_temp2921.f1= 0; _temp2921;}); _temp2920;}); break; case 297: _LL2919: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2923=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2923[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2924; _temp2924.tag= Cyc_PatternList_tok;
_temp2924.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2924;}); _temp2923;}); break; case 298: _LL2922: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2926=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2926[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2927; _temp2927.tag= Cyc_PatternList_tok;
_temp2927.f1=({ struct Cyc_List_List* _temp2928=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2928->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2928->tl= 0;
_temp2928;}); _temp2927;}); _temp2926;}); break; case 299: _LL2925: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2930=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2930[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2931; _temp2931.tag= Cyc_PatternList_tok;
_temp2931.f1=({ struct Cyc_List_List* _temp2932=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2932->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2932->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2932;}); _temp2931;}); _temp2930;}); break; case 300:
_LL2929: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2934=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2934[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2935; _temp2935.tag=
Cyc_FieldPattern_tok; _temp2935.f1=({ struct _tuple13* _temp2936=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2936->f1= 0; _temp2936->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2936;}); _temp2935;}); _temp2934;}); break; case 301:
_LL2933: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2938=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2938[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2939; _temp2939.tag=
Cyc_FieldPattern_tok; _temp2939.f1=({ struct _tuple13* _temp2940=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2940->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2940->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2940;}); _temp2939;}); _temp2938;}); break; case 302:
_LL2937: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2942=(
struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2942[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2943; _temp2943.tag=
Cyc_FieldPatternList_tok; _temp2943.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2943;});
_temp2942;}); break; case 303: _LL2941: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2945=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2945[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2946; _temp2946.tag= Cyc_FieldPatternList_tok; _temp2946.f1=({ struct Cyc_List_List*
_temp2947=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2947->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2947->tl= 0;
_temp2947;}); _temp2946;}); _temp2945;}); break; case 304: _LL2944: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2949=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2949[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2950; _temp2950.tag= Cyc_FieldPatternList_tok;
_temp2950.f1=({ struct Cyc_List_List* _temp2951=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2951->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2951->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2951;}); _temp2950;}); _temp2949;}); break; case 305:
_LL2948: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 306: _LL2952: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2954=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2954[ 0]=({ struct Cyc_Exp_tok_struct _temp2955; _temp2955.tag= Cyc_Exp_tok;
_temp2955.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2955;}); _temp2954;}); break; case 307: _LL2953: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 308: _LL2956:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2958=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2958[ 0]=({ struct Cyc_Exp_tok_struct
_temp2959; _temp2959.tag= Cyc_Exp_tok; _temp2959.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2959;}); _temp2958;}); break; case 309: _LL2957: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2961=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2961[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2962; _temp2962.tag= Cyc_Primopopt_tok; _temp2962.f1=
0; _temp2962;}); _temp2961;}); break; case 310: _LL2960: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2964=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2964[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2965; _temp2965.tag= Cyc_Primopopt_tok; _temp2965.f1=({
struct Cyc_Core_Opt* _temp2966=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2966->v=( void*)(( void*) Cyc_Absyn_Times); _temp2966;});
_temp2965;}); _temp2964;}); break; case 311: _LL2963: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2968=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2968[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2969; _temp2969.tag= Cyc_Primopopt_tok; _temp2969.f1=({
struct Cyc_Core_Opt* _temp2970=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2970->v=( void*)(( void*) Cyc_Absyn_Div); _temp2970;});
_temp2969;}); _temp2968;}); break; case 312: _LL2967: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2972=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2972[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2973; _temp2973.tag= Cyc_Primopopt_tok; _temp2973.f1=({
struct Cyc_Core_Opt* _temp2974=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2974->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2974;});
_temp2973;}); _temp2972;}); break; case 313: _LL2971: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2976=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2976[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2977; _temp2977.tag= Cyc_Primopopt_tok; _temp2977.f1=({
struct Cyc_Core_Opt* _temp2978=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2978->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2978;});
_temp2977;}); _temp2976;}); break; case 314: _LL2975: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2980=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2980[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2981; _temp2981.tag= Cyc_Primopopt_tok; _temp2981.f1=({
struct Cyc_Core_Opt* _temp2982=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2982->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2982;});
_temp2981;}); _temp2980;}); break; case 315: _LL2979: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2984=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2984[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2985; _temp2985.tag= Cyc_Primopopt_tok; _temp2985.f1=({
struct Cyc_Core_Opt* _temp2986=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2986->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2986;});
_temp2985;}); _temp2984;}); break; case 316: _LL2983: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2988=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2988[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2989; _temp2989.tag= Cyc_Primopopt_tok; _temp2989.f1=({
struct Cyc_Core_Opt* _temp2990=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2990->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2990;});
_temp2989;}); _temp2988;}); break; case 317: _LL2987: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2992=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2992[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2993; _temp2993.tag= Cyc_Primopopt_tok; _temp2993.f1=({
struct Cyc_Core_Opt* _temp2994=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2994->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2994;});
_temp2993;}); _temp2992;}); break; case 318: _LL2991: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2996=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2996[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2997; _temp2997.tag= Cyc_Primopopt_tok; _temp2997.f1=({
struct Cyc_Core_Opt* _temp2998=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2998->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2998;});
_temp2997;}); _temp2996;}); break; case 319: _LL2995: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3000=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3000[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3001; _temp3001.tag= Cyc_Primopopt_tok; _temp3001.f1=({
struct Cyc_Core_Opt* _temp3002=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3002->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3002;});
_temp3001;}); _temp3000;}); break; case 320: _LL2999: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 321: _LL3003:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3005=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3005[ 0]=({ struct Cyc_Exp_tok_struct
_temp3006; _temp3006.tag= Cyc_Exp_tok; _temp3006.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3006;});
_temp3005;}); break; case 322: _LL3004: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3008=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3008[ 0]=({ struct Cyc_Exp_tok_struct _temp3009; _temp3009.tag= Cyc_Exp_tok;
_temp3009.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3009;}); _temp3008;}); break; case 323: _LL3007: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3011=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3011[ 0]=({ struct Cyc_Exp_tok_struct
_temp3012; _temp3012.tag= Cyc_Exp_tok; _temp3012.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3012;}); _temp3011;}); break; case 324: _LL3010: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3014=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3014[ 0]=({ struct Cyc_Exp_tok_struct
_temp3015; _temp3015.tag= Cyc_Exp_tok; _temp3015.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3015;}); _temp3014;}); break; case 325: _LL3013: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3017=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3017[ 0]=({ struct Cyc_Exp_tok_struct
_temp3018; _temp3018.tag= Cyc_Exp_tok; _temp3018.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3018;});
_temp3017;}); break; case 326: _LL3016: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3020=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3020[ 0]=({ struct Cyc_Exp_tok_struct _temp3021; _temp3021.tag= Cyc_Exp_tok;
_temp3021.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3021;}); _temp3020;}); break; case 327: _LL3019: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 328: _LL3022:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
329: _LL3023: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3025=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3025[ 0]=({ struct Cyc_Exp_tok_struct
_temp3026; _temp3026.tag= Cyc_Exp_tok; _temp3026.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3026;}); _temp3025;}); break; case 330: _LL3024: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL3027:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3029=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3029[ 0]=({ struct Cyc_Exp_tok_struct
_temp3030; _temp3030.tag= Cyc_Exp_tok; _temp3030.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3030;}); _temp3029;}); break; case 332: _LL3028: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL3031:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3033=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3033[ 0]=({ struct Cyc_Exp_tok_struct
_temp3034; _temp3034.tag= Cyc_Exp_tok; _temp3034.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3034;});
_temp3033;}); break; case 334: _LL3032: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL3035:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3037=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3037[ 0]=({ struct Cyc_Exp_tok_struct
_temp3038; _temp3038.tag= Cyc_Exp_tok; _temp3038.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3038;});
_temp3037;}); break; case 336: _LL3036: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 337: _LL3039:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3041=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3041[ 0]=({ struct Cyc_Exp_tok_struct
_temp3042; _temp3042.tag= Cyc_Exp_tok; _temp3042.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3042;});
_temp3041;}); break; case 338: _LL3040: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 339: _LL3043:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3045=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3045[ 0]=({ struct Cyc_Exp_tok_struct
_temp3046; _temp3046.tag= Cyc_Exp_tok; _temp3046.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3046;}); _temp3045;}); break; case 340: _LL3044: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3048=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3048[ 0]=({ struct Cyc_Exp_tok_struct
_temp3049; _temp3049.tag= Cyc_Exp_tok; _temp3049.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3049;}); _temp3048;}); break; case 341: _LL3047: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 342: _LL3050:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3052=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3052[ 0]=({ struct Cyc_Exp_tok_struct
_temp3053; _temp3053.tag= Cyc_Exp_tok; _temp3053.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3053;}); _temp3052;}); break; case 343: _LL3051: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3055=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3055[ 0]=({ struct Cyc_Exp_tok_struct
_temp3056; _temp3056.tag= Cyc_Exp_tok; _temp3056.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3056;}); _temp3055;}); break; case 344: _LL3054: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3058=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3058[ 0]=({ struct Cyc_Exp_tok_struct
_temp3059; _temp3059.tag= Cyc_Exp_tok; _temp3059.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3059;}); _temp3058;}); break; case 345: _LL3057: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3061=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3061[ 0]=({ struct Cyc_Exp_tok_struct
_temp3062; _temp3062.tag= Cyc_Exp_tok; _temp3062.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3062;}); _temp3061;}); break; case 346: _LL3060: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 347: _LL3063:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3065=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3065[ 0]=({ struct Cyc_Exp_tok_struct
_temp3066; _temp3066.tag= Cyc_Exp_tok; _temp3066.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3066;}); _temp3065;}); break; case 348: _LL3064: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3068=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3068[ 0]=({ struct Cyc_Exp_tok_struct
_temp3069; _temp3069.tag= Cyc_Exp_tok; _temp3069.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3069;}); _temp3068;}); break; case 349: _LL3067: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 350: _LL3070:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3072=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3072[ 0]=({ struct Cyc_Exp_tok_struct
_temp3073; _temp3073.tag= Cyc_Exp_tok; _temp3073.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3073;});
_temp3072;}); break; case 351: _LL3071: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3075=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3075[ 0]=({ struct Cyc_Exp_tok_struct _temp3076; _temp3076.tag= Cyc_Exp_tok;
_temp3076.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3076;}); _temp3075;}); break; case 352: _LL3074: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 353: _LL3077:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3079=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3079[ 0]=({ struct Cyc_Exp_tok_struct
_temp3080; _temp3080.tag= Cyc_Exp_tok; _temp3080.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3080;});
_temp3079;}); break; case 354: _LL3078: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3082=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3082[ 0]=({ struct Cyc_Exp_tok_struct _temp3083; _temp3083.tag= Cyc_Exp_tok;
_temp3083.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3083;}); _temp3082;}); break; case 355: _LL3081: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3085=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3085[ 0]=({ struct Cyc_Exp_tok_struct
_temp3086; _temp3086.tag= Cyc_Exp_tok; _temp3086.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3086;});
_temp3085;}); break; case 356: _LL3084: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 357: _LL3087:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3089=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3089[ 0]=({ struct Cyc_Exp_tok_struct
_temp3090; _temp3090.tag= Cyc_Exp_tok; _temp3090.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3090;}); _temp3089;}); break; case 358: _LL3088: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 359: _LL3091:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3093=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3093[ 0]=({ struct Cyc_Exp_tok_struct
_temp3094; _temp3094.tag= Cyc_Exp_tok; _temp3094.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3094;}); _temp3093;}); break; case 360: _LL3092: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3096=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3096[ 0]=({ struct Cyc_Exp_tok_struct
_temp3097; _temp3097.tag= Cyc_Exp_tok; _temp3097.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3097;}); _temp3096;}); break; case 361: _LL3095: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3099=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3099[ 0]=({ struct Cyc_Exp_tok_struct
_temp3100; _temp3100.tag= Cyc_Exp_tok; _temp3100.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3100;}); _temp3099;}); break; case 362: _LL3098: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3102=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3102[ 0]=({ struct Cyc_Exp_tok_struct
_temp3103; _temp3103.tag= Cyc_Exp_tok; _temp3103.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3103;}); _temp3102;}); break; case 363: _LL3101: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 364: _LL3104:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3106=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3106[ 0]=({ struct Cyc_Exp_tok_struct
_temp3107; _temp3107.tag= Cyc_Exp_tok; _temp3107.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3107;}); _temp3106;}); break; case 365: _LL3105: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3109=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3109[ 0]=({ struct Cyc_Exp_tok_struct
_temp3110; _temp3110.tag= Cyc_Exp_tok; _temp3110.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3110;});
_temp3109;}); break; case 366: _LL3108: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3112=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3112[ 0]=({ struct Cyc_Exp_tok_struct _temp3113; _temp3113.tag= Cyc_Exp_tok;
_temp3113.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3113;}); _temp3112;}); break; case 367: _LL3111: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3115=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3115[ 0]=({ struct Cyc_Exp_tok_struct
_temp3116; _temp3116.tag= Cyc_Exp_tok; _temp3116.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_StructField_struct*
_temp3117=( struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp3117[ 0]=({ struct Cyc_Absyn_StructField_struct _temp3118; _temp3118.tag=
Cyc_Absyn_StructField; _temp3118.f1=({ struct _tagged_arr* _temp3119=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp3119[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3119;});
_temp3118;}); _temp3117;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3116;});
_temp3115;}); break; case 368: _LL3114: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3121=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3121[ 0]=({ struct Cyc_Exp_tok_struct _temp3122; _temp3122.tag= Cyc_Exp_tok;
_temp3122.f1= Cyc_Absyn_offsetof_exp((* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])).f3,( void*)({
struct Cyc_Absyn_TupleIndex_struct* _temp3123=( struct Cyc_Absyn_TupleIndex_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Absyn_TupleIndex_struct)); _temp3123[ 0]=({
struct Cyc_Absyn_TupleIndex_struct _temp3124; _temp3124.tag= Cyc_Absyn_TupleIndex;
_temp3124.f1=( unsigned int)(* Cyc_yyget_Int_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f2; _temp3124;});
_temp3123;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3122;}); _temp3121;}); break; case 369:
_LL3120: { struct Cyc_Position_Segment* _temp3126= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); struct
Cyc_List_List* _temp3127=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, _temp3126, Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3128=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3128[ 0]=({ struct Cyc_Exp_tok_struct
_temp3129; _temp3129.tag= Cyc_Exp_tok; _temp3129.f1= Cyc_Absyn_gentyp_exp(
_temp3127,(* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3129;});
_temp3128;}); break;} case 370: _LL3125: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3131=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3131[ 0]=({ struct Cyc_Exp_tok_struct _temp3132; _temp3132.tag= Cyc_Exp_tok;
_temp3132.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3133=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3133[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3134; _temp3134.tag= Cyc_Absyn_Malloc_e;
_temp3134.f1=({ struct Cyc_Absyn_MallocInfo _temp3135; _temp3135.is_calloc= 0;
_temp3135.rgn= 0; _temp3135.elt_type= 0; _temp3135.num_elts= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3135.fat_result=
0; _temp3135;}); _temp3134;}); _temp3133;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3132;});
_temp3131;}); break; case 371: _LL3130: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3137=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3137[ 0]=({ struct Cyc_Exp_tok_struct _temp3138; _temp3138.tag= Cyc_Exp_tok;
_temp3138.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3139=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3139[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3140; _temp3140.tag= Cyc_Absyn_Malloc_e;
_temp3140.f1=({ struct Cyc_Absyn_MallocInfo _temp3141; _temp3141.is_calloc= 0;
_temp3141.rgn=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp3141.elt_type=
0; _temp3141.num_elts= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp3141.fat_result= 0; _temp3141;}); _temp3140;});
_temp3139;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3138;}); _temp3137;}); break; case 372:
_LL3136: { void* _temp3145; struct _tuple2 _temp3143=* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _LL3146:
_temp3145= _temp3143.f3; goto _LL3144; _LL3144: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3147=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3147[ 0]=({ struct Cyc_Exp_tok_struct _temp3148; _temp3148.tag= Cyc_Exp_tok;
_temp3148.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3149=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3149[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3150; _temp3150.tag= Cyc_Absyn_Malloc_e;
_temp3150.f1=({ struct Cyc_Absyn_MallocInfo _temp3151; _temp3151.is_calloc= 1;
_temp3151.rgn= 0; _temp3151.elt_type=({ void** _temp3152=( void**) _cycalloc(
sizeof( void*)); _temp3152[ 0]= _temp3145; _temp3152;}); _temp3151.num_elts= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3151.fat_result=
0; _temp3151;}); _temp3150;}); _temp3149;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3148;});
_temp3147;}); break;} case 373: _LL3142: { void* _temp3156; struct _tuple2
_temp3154=* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _LL3157: _temp3156= _temp3154.f3; goto _LL3155; _LL3155:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3158=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3158[ 0]=({ struct Cyc_Exp_tok_struct
_temp3159; _temp3159.tag= Cyc_Exp_tok; _temp3159.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3160=( struct Cyc_Absyn_Malloc_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3160[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3161; _temp3161.tag= Cyc_Absyn_Malloc_e;
_temp3161.f1=({ struct Cyc_Absyn_MallocInfo _temp3162; _temp3162.is_calloc= 1;
_temp3162.rgn=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  8)]); _temp3162.elt_type=({
void** _temp3163=( void**) _cycalloc( sizeof( void*)); _temp3163[ 0]= _temp3156;
_temp3163;}); _temp3162.num_elts= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3162.fat_result=
0; _temp3162;}); _temp3161;}); _temp3160;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  10)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3159;});
_temp3158;}); break;} case 374: _LL3153: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3165=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3165[ 0]=({ struct Cyc_Primop_tok_struct _temp3166; _temp3166.tag= Cyc_Primop_tok;
_temp3166.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3166;}); _temp3165;});
break; case 375: _LL3164: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3168=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3168[ 0]=({ struct Cyc_Primop_tok_struct _temp3169; _temp3169.tag= Cyc_Primop_tok;
_temp3169.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3169;}); _temp3168;});
break; case 376: _LL3167: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3171=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3171[ 0]=({ struct Cyc_Primop_tok_struct _temp3172; _temp3172.tag= Cyc_Primop_tok;
_temp3172.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3172;}); _temp3171;});
break; case 377: _LL3170: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 378: _LL3173: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3175=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3175[ 0]=({ struct Cyc_Exp_tok_struct _temp3176; _temp3176.tag= Cyc_Exp_tok;
_temp3176.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3176;}); _temp3175;}); break; case 379: _LL3174: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3178=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3178[ 0]=({ struct Cyc_Exp_tok_struct
_temp3179; _temp3179.tag= Cyc_Exp_tok; _temp3179.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3179;}); _temp3178;}); break; case 380:
_LL3177: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3181=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3181[ 0]=({ struct Cyc_Exp_tok_struct
_temp3182; _temp3182.tag= Cyc_Exp_tok; _temp3182.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3182;});
_temp3181;}); break; case 381: _LL3180: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3184=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3184[ 0]=({ struct Cyc_Exp_tok_struct _temp3185; _temp3185.tag= Cyc_Exp_tok;
_temp3185.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3186=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3186[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3186;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3185;}); _temp3184;}); break; case 382: _LL3183: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3188=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3188[ 0]=({ struct Cyc_Exp_tok_struct
_temp3189; _temp3189.tag= Cyc_Exp_tok; _temp3189.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3189;});
_temp3188;}); break;} case 383: _LL3187: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3191=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3191[ 0]=({ struct Cyc_Exp_tok_struct _temp3192; _temp3192.tag= Cyc_Exp_tok;
_temp3192.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3193=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3193[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3193;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3192;}); _temp3191;}); break; case 384: _LL3190: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3195=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3195[ 0]=({ struct Cyc_Exp_tok_struct
_temp3196; _temp3196.tag= Cyc_Exp_tok; _temp3196.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3196;});
_temp3195;}); break;} case 385: _LL3194: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3198=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3198[ 0]=({ struct Cyc_Exp_tok_struct _temp3199; _temp3199.tag= Cyc_Exp_tok;
_temp3199.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3199;}); _temp3198;}); break; case 386: _LL3197: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3201=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3201[ 0]=({ struct Cyc_Exp_tok_struct
_temp3202; _temp3202.tag= Cyc_Exp_tok; _temp3202.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3202;}); _temp3201;}); break; case 387: _LL3200: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3204=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3204[ 0]=({ struct Cyc_Exp_tok_struct
_temp3205; _temp3205.tag= Cyc_Exp_tok; _temp3205.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3206=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3206[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3207; _temp3207.tag= Cyc_Absyn_CompoundLit_e;
_temp3207.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3207.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3207;});
_temp3206;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3205;}); _temp3204;}); break; case 388:
_LL3203: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3209=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3209[ 0]=({ struct Cyc_Exp_tok_struct
_temp3210; _temp3210.tag= Cyc_Exp_tok; _temp3210.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3211=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3211[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3212; _temp3212.tag= Cyc_Absyn_CompoundLit_e;
_temp3212.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3212.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3212;});
_temp3211;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3210;}); _temp3209;}); break; case 389:
_LL3208: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3214=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3214[ 0]=({ struct Cyc_Exp_tok_struct
_temp3215; _temp3215.tag= Cyc_Exp_tok; _temp3215.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3216=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3216[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3217; _temp3217.tag= Cyc_Absyn_Fill_e; _temp3217.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3217;});
_temp3216;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3215;}); _temp3214;}); break; case 390:
_LL3213: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3219=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3219[ 0]=({ struct Cyc_Exp_tok_struct
_temp3220; _temp3220.tag= Cyc_Exp_tok; _temp3220.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3221=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3221[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3222; _temp3222.tag= Cyc_Absyn_Codegen_e;
_temp3222.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3222;}); _temp3221;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3220;}); _temp3219;}); break; case 391: _LL3218: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3224=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3224[ 0]=({ struct Cyc_Exp_tok_struct
_temp3225; _temp3225.tag= Cyc_Exp_tok; _temp3225.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3226=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3226[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3227; _temp3227.tag= Cyc_Absyn_UnknownId_e;
_temp3227.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3227;}); _temp3226;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3225;});
_temp3224;}); break; case 392: _LL3223: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 393: _LL3228:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3230=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3230[ 0]=({ struct Cyc_Exp_tok_struct
_temp3231; _temp3231.tag= Cyc_Exp_tok; _temp3231.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3231;});
_temp3230;}); break; case 394: _LL3229: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 395:
_LL3232: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3234=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3234[ 0]=({ struct Cyc_Exp_tok_struct
_temp3235; _temp3235.tag= Cyc_Exp_tok; _temp3235.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3236=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3236[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3237; _temp3237.tag=
Cyc_Absyn_UnknownId_e; _temp3237.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3237;});
_temp3236;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3235;});
_temp3234;}); break; case 396: _LL3233: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3239=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3239[ 0]=({ struct Cyc_Exp_tok_struct _temp3240; _temp3240.tag= Cyc_Exp_tok;
_temp3240.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3241=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3241[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3242; _temp3242.tag=
Cyc_Absyn_UnknownId_e; _temp3242.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3242;});
_temp3241;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3240;}); _temp3239;}); break; case 397: _LL3238: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3244=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3244[ 0]=({ struct Cyc_Exp_tok_struct
_temp3245; _temp3245.tag= Cyc_Exp_tok; _temp3245.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3245;}); _temp3244;}); break; case 398: _LL3243: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3247=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3247[ 0]=({ struct Cyc_Exp_tok_struct
_temp3248; _temp3248.tag= Cyc_Exp_tok; _temp3248.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3249=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3249[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3250; _temp3250.tag= Cyc_Absyn_Struct_e;
_temp3250.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3250.f2= 0; _temp3250.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3250.f4= 0;
_temp3250;}); _temp3249;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3248;});
_temp3247;}); break; case 399: _LL3246: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3252=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3252[ 0]=({ struct Cyc_Exp_tok_struct _temp3253; _temp3253.tag= Cyc_Exp_tok;
_temp3253.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3253;}); _temp3252;}); break; case 400: _LL3251: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3255=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3255[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3256; _temp3256.tag= Cyc_ExpList_tok; _temp3256.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3256;});
_temp3255;}); break; case 401: _LL3254: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3258=( struct Cyc_ExpList_tok_struct*) _cycalloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3258[ 0]=({ struct Cyc_ExpList_tok_struct _temp3259; _temp3259.tag= Cyc_ExpList_tok;
_temp3259.f1=({ struct Cyc_List_List* _temp3260=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3260->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3260->tl= 0;
_temp3260;}); _temp3259;}); _temp3258;}); break; case 402: _LL3257: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3262=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3262[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3263; _temp3263.tag= Cyc_ExpList_tok; _temp3263.f1=({ struct Cyc_List_List*
_temp3264=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3264->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3264->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp3264;});
_temp3263;}); _temp3262;}); break; case 403: _LL3261: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3266=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3266[ 0]=({ struct Cyc_Exp_tok_struct
_temp3267; _temp3267.tag= Cyc_Exp_tok; _temp3267.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3267;});
_temp3266;}); break; case 404: _LL3265: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3269=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3269[ 0]=({ struct Cyc_Exp_tok_struct _temp3270; _temp3270.tag= Cyc_Exp_tok;
_temp3270.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3270;});
_temp3269;}); break; case 405: _LL3268: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3272=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3272[ 0]=({ struct Cyc_Exp_tok_struct _temp3273; _temp3273.tag= Cyc_Exp_tok;
_temp3273.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3273;});
_temp3272;}); break; case 406: _LL3271: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3275=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3275[ 0]=({ struct Cyc_Exp_tok_struct _temp3276; _temp3276.tag= Cyc_Exp_tok;
_temp3276.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3276;});
_temp3275;}); break; case 407: _LL3274: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3278=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3278[ 0]=({ struct Cyc_QualId_tok_struct _temp3279; _temp3279.tag= Cyc_QualId_tok;
_temp3279.f1=({ struct _tuple1* _temp3280=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3280->f1= Cyc_Absyn_rel_ns_null; _temp3280->f2=({ struct
_tagged_arr* _temp3281=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3281[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3281;}); _temp3280;});
_temp3279;}); _temp3278;}); break; case 408: _LL3277: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3282:
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
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 409u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 112u, yyn -  124)] +  yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >=  0?
yystate <=  4621: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4622u,
yystate)] ==  yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){
yystate=( int) Cyc_yytable[ _check_known_subscript_notnull( 4622u, yystate)];}
else{ yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 
124)];} goto yynewstate; yyerrlab: if( yyerrstatus ==  0){ ++ Cyc_yynerrs; yyn=(
int) Cyc_yypact[ _check_known_subscript_notnull( 822u, yystate)]; if( yyn >  -
32768? yyn <  4621: 0){ int sze= 0; struct _tagged_arr msg; int x; int count;
count= 0; for( x= yyn <  0? - yyn: 0; x <  236u /  sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4622u, x +  yyn)] ==  x){( sze
+= Cyc_Std_strlen( Cyc_yytname[ _check_known_subscript_notnull( 236u, x)]) +  15,
count ++);}} msg=({ unsigned int _temp3284=( unsigned int)( sze +  15);
unsigned char* _temp3285=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp3284)); struct _tagged_arr _temp3287= _tag_arr(
_temp3285, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp3286= _temp3284; unsigned int i; for( i= 0; i <  _temp3286; i ++){
_temp3285[ i]='\000';}}; _temp3287;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
sizeof( unsigned char), 12u)); if( count <  5){ count= 0; for( x= yyn <  0? -
yyn: 0; x <  236u /  sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4622u, x +  yyn)] ==  x){ Cyc_Std_strcat( msg,
count ==  0? _tag_arr(", expecting `", sizeof( unsigned char), 14u): _tag_arr(" or `",
sizeof( unsigned char), 6u)); Cyc_Std_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 236u, x)]); Cyc_Std_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus ==  3){ if( Cyc_yychar ==  0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset ==  0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 822u,
yystate)]; if( yyn ==  - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn <  0? 1:
yyn >  4621)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4622u, yyn)] !=  1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4622u,
yyn)]; if( yyn <  0){ if( yyn ==  - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn ==  0){ goto yyerrpop;}} if( yyn ==  821){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3297= v; struct
_tuple15* _temp3315; struct _tuple15 _temp3317; int _temp3318; unsigned char
_temp3320; short _temp3322; struct _tagged_arr _temp3324; struct Cyc_Core_Opt*
_temp3326; struct Cyc_Core_Opt* _temp3328; struct Cyc_Core_Opt _temp3330; struct
_tagged_arr* _temp3331; struct _tuple1* _temp3333; struct _tuple1 _temp3335;
struct _tagged_arr* _temp3336; void* _temp3338; _LL3299: if(*(( void**)
_temp3297) ==  Cyc_Int_tok){ _LL3316: _temp3315=(( struct Cyc_Int_tok_struct*)
_temp3297)->f1; _temp3317=* _temp3315; _LL3319: _temp3318= _temp3317.f2; goto
_LL3300;} else{ goto _LL3301;} _LL3301: if(*(( void**) _temp3297) ==  Cyc_Char_tok){
_LL3321: _temp3320=(( struct Cyc_Char_tok_struct*) _temp3297)->f1; goto _LL3302;}
else{ goto _LL3303;} _LL3303: if(*(( void**) _temp3297) ==  Cyc_Short_tok){
_LL3323: _temp3322=(( struct Cyc_Short_tok_struct*) _temp3297)->f1; goto _LL3304;}
else{ goto _LL3305;} _LL3305: if(*(( void**) _temp3297) ==  Cyc_String_tok){
_LL3325: _temp3324=(( struct Cyc_String_tok_struct*) _temp3297)->f1; goto
_LL3306;} else{ goto _LL3307;} _LL3307: if(*(( void**) _temp3297) ==  Cyc_Stringopt_tok){
_LL3327: _temp3326=(( struct Cyc_Stringopt_tok_struct*) _temp3297)->f1; if(
_temp3326 ==  0){ goto _LL3308;} else{ goto _LL3309;}} else{ goto _LL3309;}
_LL3309: if(*(( void**) _temp3297) ==  Cyc_Stringopt_tok){ _LL3329: _temp3328=((
struct Cyc_Stringopt_tok_struct*) _temp3297)->f1; if( _temp3328 ==  0){ goto
_LL3311;} else{ _temp3330=* _temp3328; _LL3332: _temp3331=( struct _tagged_arr*)
_temp3330.v; goto _LL3310;}} else{ goto _LL3311;} _LL3311: if(*(( void**)
_temp3297) ==  Cyc_QualId_tok){ _LL3334: _temp3333=(( struct Cyc_QualId_tok_struct*)
_temp3297)->f1; _temp3335=* _temp3333; _LL3339: _temp3338= _temp3335.f1; goto
_LL3337; _LL3337: _temp3336= _temp3335.f2; goto _LL3312;} else{ goto _LL3313;}
_LL3313: goto _LL3314; _LL3300:({ struct Cyc_Std_Int_pa_struct _temp3341;
_temp3341.tag= Cyc_Std_Int_pa; _temp3341.f1=( int)(( unsigned int) _temp3318);{
void* _temp3340[ 1u]={& _temp3341}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3340, sizeof( void*), 1u));}}); goto
_LL3298; _LL3302:({ struct Cyc_Std_Int_pa_struct _temp3343; _temp3343.tag= Cyc_Std_Int_pa;
_temp3343.f1=( int)(( unsigned int)(( int) _temp3320));{ void* _temp3342[ 1u]={&
_temp3343}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3342, sizeof( void*), 1u));}}); goto _LL3298;
_LL3304:({ struct Cyc_Std_Int_pa_struct _temp3345; _temp3345.tag= Cyc_Std_Int_pa;
_temp3345.f1=( int)(( unsigned int)(( int) _temp3322));{ void* _temp3344[ 1u]={&
_temp3345}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3344, sizeof( void*), 1u));}}); goto _LL3298;
_LL3306:({ struct Cyc_Std_String_pa_struct _temp3347; _temp3347.tag= Cyc_Std_String_pa;
_temp3347.f1=( struct _tagged_arr) _temp3324;{ void* _temp3346[ 1u]={& _temp3347};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3346, sizeof( void*), 1u));}}); goto _LL3298; _LL3308:({ void*
_temp3348[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3348, sizeof( void*), 0u));}); goto _LL3298;
_LL3310:({ struct Cyc_Std_String_pa_struct _temp3350; _temp3350.tag= Cyc_Std_String_pa;
_temp3350.f1=( struct _tagged_arr)* _temp3331;{ void* _temp3349[ 1u]={&
_temp3350}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3349, sizeof( void*), 1u));}}); goto _LL3298;
_LL3312: { struct Cyc_List_List* _temp3351= 0;{ void* _temp3352= _temp3338;
struct Cyc_List_List* _temp3360; struct Cyc_List_List* _temp3362; _LL3354: if((
unsigned int) _temp3352 >  1u?*(( int*) _temp3352) ==  Cyc_Absyn_Rel_n: 0){
_LL3361: _temp3360=(( struct Cyc_Absyn_Rel_n_struct*) _temp3352)->f1; goto
_LL3355;} else{ goto _LL3356;} _LL3356: if(( unsigned int) _temp3352 >  1u?*((
int*) _temp3352) ==  Cyc_Absyn_Abs_n: 0){ _LL3363: _temp3362=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3352)->f1; goto _LL3357;} else{ goto _LL3358;} _LL3358: if( _temp3352 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3359;} else{ goto _LL3353;} _LL3355: _temp3351=
_temp3360; goto _LL3353; _LL3357: _temp3351= _temp3362; goto _LL3353; _LL3359:
goto _LL3353; _LL3353:;} for( 0; _temp3351 !=  0; _temp3351= _temp3351->tl){({
struct Cyc_Std_String_pa_struct _temp3365; _temp3365.tag= Cyc_Std_String_pa;
_temp3365.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _temp3351->hd);{ void*
_temp3364[ 1u]={& _temp3365}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3364, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3367; _temp3367.tag= Cyc_Std_String_pa;
_temp3367.f1=( struct _tagged_arr)* _temp3336;{ void* _temp3366[ 1u]={&
_temp3367}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3366, sizeof( void*), 1u));}}); goto _LL3298;}
_LL3314:({ void* _temp3368[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3368, sizeof( void*), 0u));}); goto
_LL3298; _LL3298:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3369=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3369->v=(
void*) Cyc_Lexing_from_file( f); _temp3369;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
