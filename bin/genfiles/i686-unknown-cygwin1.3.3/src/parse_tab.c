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
350u]={ 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 121, 2, 2, 103, 119, 115, 2, 99, 100, 110, 113, 101,
114, 107, 118, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 106, 95, 104, 98, 105, 112, 111, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 108,
2, 109, 117, 102, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 96, 116, 97, 120, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94}; static
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
static unsigned char _temp1257[ 9u]="SIZEOF_T"; static unsigned char _temp1258[
7u]="REGION"; static unsigned char _temp1259[ 5u]="RNEW"; static unsigned char
_temp1260[ 8u]="RMALLOC"; static unsigned char _temp1261[ 8u]="REGIONS"; static
unsigned char _temp1262[ 4u]="GEN"; static unsigned char _temp1263[ 7u]="PTR_OP";
static unsigned char _temp1264[ 7u]="INC_OP"; static unsigned char _temp1265[ 7u]="DEC_OP";
static unsigned char _temp1266[ 8u]="LEFT_OP"; static unsigned char _temp1267[ 9u]="RIGHT_OP";
static unsigned char _temp1268[ 6u]="LE_OP"; static unsigned char _temp1269[ 6u]="GE_OP";
static unsigned char _temp1270[ 6u]="EQ_OP"; static unsigned char _temp1271[ 6u]="NE_OP";
static unsigned char _temp1272[ 7u]="AND_OP"; static unsigned char _temp1273[ 6u]="OR_OP";
static unsigned char _temp1274[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1275[ 11u]="DIV_ASSIGN"; static unsigned char _temp1276[ 11u]="MOD_ASSIGN";
static unsigned char _temp1277[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1278[ 11u]="SUB_ASSIGN"; static unsigned char _temp1279[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1280[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1281[ 11u]="AND_ASSIGN"; static unsigned char _temp1282[ 11u]="XOR_ASSIGN";
static unsigned char _temp1283[ 10u]="OR_ASSIGN"; static unsigned char _temp1284[
9u]="ELLIPSIS"; static unsigned char _temp1285[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1286[ 12u]="COLON_COLON"; static unsigned char _temp1287[ 11u]="IDENTIFIER";
static unsigned char _temp1288[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1289[ 7u]="STRING"; static unsigned char _temp1290[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1291[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1292[ 9u]="TYPE_VAR"; static unsigned char _temp1293[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1294[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1295[ 10u]="ATTRIBUTE"; static unsigned char _temp1296[ 4u]="';'"; static
unsigned char _temp1297[ 4u]="'{'"; static unsigned char _temp1298[ 4u]="'}'";
static unsigned char _temp1299[ 4u]="'='"; static unsigned char _temp1300[ 4u]="'('";
static unsigned char _temp1301[ 4u]="')'"; static unsigned char _temp1302[ 4u]="','";
static unsigned char _temp1303[ 4u]="'_'"; static unsigned char _temp1304[ 4u]="'$'";
static unsigned char _temp1305[ 4u]="'<'"; static unsigned char _temp1306[ 4u]="'>'";
static unsigned char _temp1307[ 4u]="':'"; static unsigned char _temp1308[ 4u]="'.'";
static unsigned char _temp1309[ 4u]="'['"; static unsigned char _temp1310[ 4u]="']'";
static unsigned char _temp1311[ 4u]="'*'"; static unsigned char _temp1312[ 4u]="'@'";
static unsigned char _temp1313[ 4u]="'?'"; static unsigned char _temp1314[ 4u]="'+'";
static unsigned char _temp1315[ 4u]="'-'"; static unsigned char _temp1316[ 4u]="'&'";
static unsigned char _temp1317[ 4u]="'|'"; static unsigned char _temp1318[ 4u]="'^'";
static unsigned char _temp1319[ 4u]="'/'"; static unsigned char _temp1320[ 4u]="'%'";
static unsigned char _temp1321[ 4u]="'~'"; static unsigned char _temp1322[ 4u]="'!'";
static unsigned char _temp1323[ 5u]="prog"; static unsigned char _temp1324[ 17u]="translation_unit";
static unsigned char _temp1325[ 21u]="external_declaration"; static
unsigned char _temp1326[ 20u]="function_definition"; static unsigned char
_temp1327[ 21u]="function_definition2"; static unsigned char _temp1328[ 13u]="using_action";
static unsigned char _temp1329[ 15u]="unusing_action"; static unsigned char
_temp1330[ 17u]="namespace_action"; static unsigned char _temp1331[ 19u]="unnamespace_action";
static unsigned char _temp1332[ 12u]="declaration"; static unsigned char
_temp1333[ 17u]="declaration_list"; static unsigned char _temp1334[ 23u]="declaration_specifiers";
static unsigned char _temp1335[ 24u]="storage_class_specifier"; static
unsigned char _temp1336[ 15u]="attributes_opt"; static unsigned char _temp1337[
11u]="attributes"; static unsigned char _temp1338[ 15u]="attribute_list"; static
unsigned char _temp1339[ 10u]="attribute"; static unsigned char _temp1340[ 15u]="type_specifier";
static unsigned char _temp1341[ 5u]="kind"; static unsigned char _temp1342[ 15u]="type_qualifier";
static unsigned char _temp1343[ 15u]="enum_specifier"; static unsigned char
_temp1344[ 11u]="enum_field"; static unsigned char _temp1345[ 22u]="enum_declaration_list";
static unsigned char _temp1346[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1347[ 16u]="type_params_opt"; static unsigned char _temp1348[
16u]="struct_or_union"; static unsigned char _temp1349[ 24u]="struct_declaration_list";
static unsigned char _temp1350[ 25u]="struct_declaration_list0"; static
unsigned char _temp1351[ 21u]="init_declarator_list"; static unsigned char
_temp1352[ 22u]="init_declarator_list0"; static unsigned char _temp1353[ 16u]="init_declarator";
static unsigned char _temp1354[ 19u]="struct_declaration"; static unsigned char
_temp1355[ 25u]="specifier_qualifier_list"; static unsigned char _temp1356[ 23u]="struct_declarator_list";
static unsigned char _temp1357[ 24u]="struct_declarator_list0"; static
unsigned char _temp1358[ 18u]="struct_declarator"; static unsigned char
_temp1359[ 17u]="tunion_specifier"; static unsigned char _temp1360[ 18u]="tunion_or_xtunion";
static unsigned char _temp1361[ 17u]="tunionfield_list"; static unsigned char
_temp1362[ 18u]="tunionfield_scope"; static unsigned char _temp1363[ 12u]="tunionfield";
static unsigned char _temp1364[ 11u]="declarator"; static unsigned char
_temp1365[ 18u]="direct_declarator"; static unsigned char _temp1366[ 8u]="pointer";
static unsigned char _temp1367[ 13u]="pointer_char"; static unsigned char
_temp1368[ 8u]="rgn_opt"; static unsigned char _temp1369[ 4u]="rgn"; static
unsigned char _temp1370[ 20u]="type_qualifier_list"; static unsigned char
_temp1371[ 20u]="parameter_type_list"; static unsigned char _temp1372[ 9u]="type_var";
static unsigned char _temp1373[ 16u]="optional_effect"; static unsigned char
_temp1374[ 19u]="optional_rgn_order"; static unsigned char _temp1375[ 10u]="rgn_order";
static unsigned char _temp1376[ 16u]="optional_inject"; static unsigned char
_temp1377[ 11u]="effect_set"; static unsigned char _temp1378[ 14u]="atomic_effect";
static unsigned char _temp1379[ 11u]="region_set"; static unsigned char
_temp1380[ 15u]="parameter_list"; static unsigned char _temp1381[ 22u]="parameter_declaration";
static unsigned char _temp1382[ 16u]="identifier_list"; static unsigned char
_temp1383[ 17u]="identifier_list0"; static unsigned char _temp1384[ 12u]="initializer";
static unsigned char _temp1385[ 18u]="array_initializer"; static unsigned char
_temp1386[ 17u]="initializer_list"; static unsigned char _temp1387[ 12u]="designation";
static unsigned char _temp1388[ 16u]="designator_list"; static unsigned char
_temp1389[ 11u]="designator"; static unsigned char _temp1390[ 10u]="type_name";
static unsigned char _temp1391[ 14u]="any_type_name"; static unsigned char
_temp1392[ 15u]="type_name_list"; static unsigned char _temp1393[ 20u]="abstract_declarator";
static unsigned char _temp1394[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1395[ 10u]="statement"; static unsigned char _temp1396[ 18u]="labeled_statement";
static unsigned char _temp1397[ 21u]="expression_statement"; static
unsigned char _temp1398[ 19u]="compound_statement"; static unsigned char
_temp1399[ 16u]="block_item_list"; static unsigned char _temp1400[ 20u]="selection_statement";
static unsigned char _temp1401[ 15u]="switch_clauses"; static unsigned char
_temp1402[ 16u]="switchC_clauses"; static unsigned char _temp1403[ 20u]="iteration_statement";
static unsigned char _temp1404[ 15u]="jump_statement"; static unsigned char
_temp1405[ 8u]="pattern"; static unsigned char _temp1406[ 19u]="tuple_pattern_list";
static unsigned char _temp1407[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1408[ 14u]="field_pattern"; static unsigned char _temp1409[ 19u]="field_pattern_list";
static unsigned char _temp1410[ 20u]="field_pattern_list0"; static unsigned char
_temp1411[ 11u]="expression"; static unsigned char _temp1412[ 22u]="assignment_expression";
static unsigned char _temp1413[ 20u]="assignment_operator"; static unsigned char
_temp1414[ 23u]="conditional_expression"; static unsigned char _temp1415[ 20u]="constant_expression";
static unsigned char _temp1416[ 22u]="logical_or_expression"; static
unsigned char _temp1417[ 23u]="logical_and_expression"; static unsigned char
_temp1418[ 24u]="inclusive_or_expression"; static unsigned char _temp1419[ 24u]="exclusive_or_expression";
static unsigned char _temp1420[ 15u]="and_expression"; static unsigned char
_temp1421[ 20u]="equality_expression"; static unsigned char _temp1422[ 22u]="relational_expression";
static unsigned char _temp1423[ 17u]="shift_expression"; static unsigned char
_temp1424[ 20u]="additive_expression"; static unsigned char _temp1425[ 26u]="multiplicative_expression";
static unsigned char _temp1426[ 16u]="cast_expression"; static unsigned char
_temp1427[ 17u]="unary_expression"; static unsigned char _temp1428[ 15u]="unary_operator";
static unsigned char _temp1429[ 19u]="postfix_expression"; static unsigned char
_temp1430[ 19u]="primary_expression"; static unsigned char _temp1431[ 25u]="argument_expression_list";
static unsigned char _temp1432[ 26u]="argument_expression_list0"; static
unsigned char _temp1433[ 9u]="constant"; static unsigned char _temp1434[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 234u]={{ _temp1201, _temp1201, _temp1201
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
_temp1257, _temp1257, _temp1257 +  9u},{ _temp1258, _temp1258, _temp1258 +  7u},{
_temp1259, _temp1259, _temp1259 +  5u},{ _temp1260, _temp1260, _temp1260 +  8u},{
_temp1261, _temp1261, _temp1261 +  8u},{ _temp1262, _temp1262, _temp1262 +  4u},{
_temp1263, _temp1263, _temp1263 +  7u},{ _temp1264, _temp1264, _temp1264 +  7u},{
_temp1265, _temp1265, _temp1265 +  7u},{ _temp1266, _temp1266, _temp1266 +  8u},{
_temp1267, _temp1267, _temp1267 +  9u},{ _temp1268, _temp1268, _temp1268 +  6u},{
_temp1269, _temp1269, _temp1269 +  6u},{ _temp1270, _temp1270, _temp1270 +  6u},{
_temp1271, _temp1271, _temp1271 +  6u},{ _temp1272, _temp1272, _temp1272 +  7u},{
_temp1273, _temp1273, _temp1273 +  6u},{ _temp1274, _temp1274, _temp1274 +  11u},{
_temp1275, _temp1275, _temp1275 +  11u},{ _temp1276, _temp1276, _temp1276 +  11u},{
_temp1277, _temp1277, _temp1277 +  11u},{ _temp1278, _temp1278, _temp1278 +  11u},{
_temp1279, _temp1279, _temp1279 +  12u},{ _temp1280, _temp1280, _temp1280 +  13u},{
_temp1281, _temp1281, _temp1281 +  11u},{ _temp1282, _temp1282, _temp1282 +  11u},{
_temp1283, _temp1283, _temp1283 +  10u},{ _temp1284, _temp1284, _temp1284 +  9u},{
_temp1285, _temp1285, _temp1285 +  11u},{ _temp1286, _temp1286, _temp1286 +  12u},{
_temp1287, _temp1287, _temp1287 +  11u},{ _temp1288, _temp1288, _temp1288 +  17u},{
_temp1289, _temp1289, _temp1289 +  7u},{ _temp1290, _temp1290, _temp1290 +  19u},{
_temp1291, _temp1291, _temp1291 +  18u},{ _temp1292, _temp1292, _temp1292 +  9u},{
_temp1293, _temp1293, _temp1293 +  16u},{ _temp1294, _temp1294, _temp1294 +  18u},{
_temp1295, _temp1295, _temp1295 +  10u},{ _temp1296, _temp1296, _temp1296 +  4u},{
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
_temp1321, _temp1321, _temp1321 +  4u},{ _temp1322, _temp1322, _temp1322 +  4u},{
_temp1323, _temp1323, _temp1323 +  5u},{ _temp1324, _temp1324, _temp1324 +  17u},{
_temp1325, _temp1325, _temp1325 +  21u},{ _temp1326, _temp1326, _temp1326 +  20u},{
_temp1327, _temp1327, _temp1327 +  21u},{ _temp1328, _temp1328, _temp1328 +  13u},{
_temp1329, _temp1329, _temp1329 +  15u},{ _temp1330, _temp1330, _temp1330 +  17u},{
_temp1331, _temp1331, _temp1331 +  19u},{ _temp1332, _temp1332, _temp1332 +  12u},{
_temp1333, _temp1333, _temp1333 +  17u},{ _temp1334, _temp1334, _temp1334 +  23u},{
_temp1335, _temp1335, _temp1335 +  24u},{ _temp1336, _temp1336, _temp1336 +  15u},{
_temp1337, _temp1337, _temp1337 +  11u},{ _temp1338, _temp1338, _temp1338 +  15u},{
_temp1339, _temp1339, _temp1339 +  10u},{ _temp1340, _temp1340, _temp1340 +  15u},{
_temp1341, _temp1341, _temp1341 +  5u},{ _temp1342, _temp1342, _temp1342 +  15u},{
_temp1343, _temp1343, _temp1343 +  15u},{ _temp1344, _temp1344, _temp1344 +  11u},{
_temp1345, _temp1345, _temp1345 +  22u},{ _temp1346, _temp1346, _temp1346 +  26u},{
_temp1347, _temp1347, _temp1347 +  16u},{ _temp1348, _temp1348, _temp1348 +  16u},{
_temp1349, _temp1349, _temp1349 +  24u},{ _temp1350, _temp1350, _temp1350 +  25u},{
_temp1351, _temp1351, _temp1351 +  21u},{ _temp1352, _temp1352, _temp1352 +  22u},{
_temp1353, _temp1353, _temp1353 +  16u},{ _temp1354, _temp1354, _temp1354 +  19u},{
_temp1355, _temp1355, _temp1355 +  25u},{ _temp1356, _temp1356, _temp1356 +  23u},{
_temp1357, _temp1357, _temp1357 +  24u},{ _temp1358, _temp1358, _temp1358 +  18u},{
_temp1359, _temp1359, _temp1359 +  17u},{ _temp1360, _temp1360, _temp1360 +  18u},{
_temp1361, _temp1361, _temp1361 +  17u},{ _temp1362, _temp1362, _temp1362 +  18u},{
_temp1363, _temp1363, _temp1363 +  12u},{ _temp1364, _temp1364, _temp1364 +  11u},{
_temp1365, _temp1365, _temp1365 +  18u},{ _temp1366, _temp1366, _temp1366 +  8u},{
_temp1367, _temp1367, _temp1367 +  13u},{ _temp1368, _temp1368, _temp1368 +  8u},{
_temp1369, _temp1369, _temp1369 +  4u},{ _temp1370, _temp1370, _temp1370 +  20u},{
_temp1371, _temp1371, _temp1371 +  20u},{ _temp1372, _temp1372, _temp1372 +  9u},{
_temp1373, _temp1373, _temp1373 +  16u},{ _temp1374, _temp1374, _temp1374 +  19u},{
_temp1375, _temp1375, _temp1375 +  10u},{ _temp1376, _temp1376, _temp1376 +  16u},{
_temp1377, _temp1377, _temp1377 +  11u},{ _temp1378, _temp1378, _temp1378 +  14u},{
_temp1379, _temp1379, _temp1379 +  11u},{ _temp1380, _temp1380, _temp1380 +  15u},{
_temp1381, _temp1381, _temp1381 +  22u},{ _temp1382, _temp1382, _temp1382 +  16u},{
_temp1383, _temp1383, _temp1383 +  17u},{ _temp1384, _temp1384, _temp1384 +  12u},{
_temp1385, _temp1385, _temp1385 +  18u},{ _temp1386, _temp1386, _temp1386 +  17u},{
_temp1387, _temp1387, _temp1387 +  12u},{ _temp1388, _temp1388, _temp1388 +  16u},{
_temp1389, _temp1389, _temp1389 +  11u},{ _temp1390, _temp1390, _temp1390 +  10u},{
_temp1391, _temp1391, _temp1391 +  14u},{ _temp1392, _temp1392, _temp1392 +  15u},{
_temp1393, _temp1393, _temp1393 +  20u},{ _temp1394, _temp1394, _temp1394 +  27u},{
_temp1395, _temp1395, _temp1395 +  10u},{ _temp1396, _temp1396, _temp1396 +  18u},{
_temp1397, _temp1397, _temp1397 +  21u},{ _temp1398, _temp1398, _temp1398 +  19u},{
_temp1399, _temp1399, _temp1399 +  16u},{ _temp1400, _temp1400, _temp1400 +  20u},{
_temp1401, _temp1401, _temp1401 +  15u},{ _temp1402, _temp1402, _temp1402 +  16u},{
_temp1403, _temp1403, _temp1403 +  20u},{ _temp1404, _temp1404, _temp1404 +  15u},{
_temp1405, _temp1405, _temp1405 +  8u},{ _temp1406, _temp1406, _temp1406 +  19u},{
_temp1407, _temp1407, _temp1407 +  20u},{ _temp1408, _temp1408, _temp1408 +  14u},{
_temp1409, _temp1409, _temp1409 +  19u},{ _temp1410, _temp1410, _temp1410 +  20u},{
_temp1411, _temp1411, _temp1411 +  11u},{ _temp1412, _temp1412, _temp1412 +  22u},{
_temp1413, _temp1413, _temp1413 +  20u},{ _temp1414, _temp1414, _temp1414 +  23u},{
_temp1415, _temp1415, _temp1415 +  20u},{ _temp1416, _temp1416, _temp1416 +  22u},{
_temp1417, _temp1417, _temp1417 +  23u},{ _temp1418, _temp1418, _temp1418 +  24u},{
_temp1419, _temp1419, _temp1419 +  24u},{ _temp1420, _temp1420, _temp1420 +  15u},{
_temp1421, _temp1421, _temp1421 +  20u},{ _temp1422, _temp1422, _temp1422 +  22u},{
_temp1423, _temp1423, _temp1423 +  17u},{ _temp1424, _temp1424, _temp1424 +  20u},{
_temp1425, _temp1425, _temp1425 +  26u},{ _temp1426, _temp1426, _temp1426 +  16u},{
_temp1427, _temp1427, _temp1427 +  17u},{ _temp1428, _temp1428, _temp1428 +  15u},{
_temp1429, _temp1429, _temp1429 +  19u},{ _temp1430, _temp1430, _temp1430 +  19u},{
_temp1431, _temp1431, _temp1431 +  25u},{ _temp1432, _temp1432, _temp1432 +  26u},{
_temp1433, _temp1433, _temp1433 +  9u},{ _temp1434, _temp1434, _temp1434 +  20u}};
static short Cyc_yyr1[ 407u]={ 0, 122, 123, 123, 123, 123, 123, 123, 123, 124,
124, 125, 125, 125, 125, 126, 126, 127, 128, 129, 130, 131, 131, 131, 131, 132,
132, 133, 133, 133, 133, 133, 133, 133, 133, 134, 134, 134, 134, 134, 134, 134,
135, 135, 136, 137, 137, 138, 138, 138, 138, 138, 139, 139, 139, 139, 139, 139,
139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 140, 140, 141, 141,
141, 142, 142, 142, 143, 143, 144, 144, 145, 145, 145, 145, 145, 146, 146, 146,
147, 147, 148, 149, 149, 150, 151, 151, 152, 152, 153, 154, 154, 154, 154, 155,
156, 156, 157, 157, 157, 158, 158, 158, 158, 159, 159, 160, 160, 160, 160, 161,
161, 161, 162, 162, 163, 163, 164, 164, 164, 164, 164, 164, 164, 164, 164, 164,
164, 164, 165, 165, 165, 165, 166, 166, 166, 166, 166, 167, 167, 168, 168, 169,
169, 170, 170, 170, 170, 171, 171, 172, 172, 173, 173, 174, 174, 175, 175, 176,
176, 177, 177, 177, 177, 178, 178, 179, 179, 180, 180, 180, 181, 182, 182, 183,
183, 184, 184, 184, 184, 185, 185, 185, 185, 186, 187, 187, 188, 188, 189, 189,
190, 190, 190, 190, 190, 191, 191, 192, 192, 192, 193, 193, 193, 193, 193, 193,
193, 193, 193, 193, 193, 193, 194, 194, 194, 194, 194, 194, 194, 194, 194, 194,
195, 196, 196, 197, 197, 198, 198, 198, 198, 198, 198, 199, 199, 199, 199, 199,
200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 202, 202, 202, 202, 202, 202,
202, 202, 202, 202, 202, 202, 202, 202, 203, 203, 203, 203, 203, 203, 203, 203,
204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 205, 205,
206, 206, 207, 207, 208, 209, 209, 210, 210, 211, 211, 212, 212, 212, 212, 212,
212, 212, 212, 212, 212, 212, 213, 213, 213, 213, 213, 213, 213, 214, 215, 215,
216, 216, 217, 217, 218, 218, 219, 219, 220, 220, 220, 221, 221, 221, 221, 221,
222, 222, 222, 223, 223, 223, 224, 224, 224, 224, 225, 225, 226, 226, 226, 226,
226, 226, 226, 226, 226, 226, 226, 226, 226, 226, 227, 227, 227, 228, 228, 228,
228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 229, 229, 229, 229, 229,
229, 229, 229, 229, 230, 231, 231, 232, 232, 232, 232, 233, 233}; static short
Cyc_yyr2[ 407u]={ 0, 1, 2, 3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4, 3, 4, 2, 1, 2, 1,
2, 3, 5, 3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1, 0, 1, 6, 1, 3, 1,
1, 4, 4, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4, 4, 1, 1, 1, 1, 1,
5, 2, 4, 1, 3, 1, 3, 4, 6, 6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3, 1, 3, 3, 2,
3, 2, 3, 1, 1, 3, 1, 2, 3, 6, 4, 3, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2, 6, 1, 2, 1,
3, 3, 4, 4, 5, 4, 4, 4, 2, 2, 1, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0, 1, 1, 1, 1, 2, 3,
5, 5, 7, 1, 3, 0, 2, 0, 2, 3, 5, 0, 1, 1, 3, 2, 3, 4, 1, 1, 3, 1, 3, 2, 1, 2, 1,
1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 4, 3, 1, 3, 1,
1, 2, 3, 2, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2, 2, 3, 1, 2,
2, 3, 1, 2, 1, 2, 1, 2, 5, 7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3, 4, 5, 5, 7, 6, 7,
7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1, 1, 1, 1, 5,
4, 4, 5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 3,
3, 1, 3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 6, 6, 5, 7, 9,
1, 1, 1, 1, 4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4, 5, 1,
1, 3, 1, 1, 1, 1, 1, 1}; static short Cyc_yydefact[ 808u]={ 0, 139, 35, 36, 37,
38, 40, 52, 54, 55, 56, 57, 58, 59, 60, 61, 72, 73, 74, 90, 91, 42, 0, 0, 41, 0,
0, 115, 116, 0, 0, 405, 159, 406, 87, 0, 53, 0, 144, 145, 148, 1, 0, 9, 0, 0, 10,
0, 42, 42, 42, 62, 63, 0, 64, 0, 0, 126, 0, 149, 65, 128, 39, 0, 33, 43, 0, 76,
288, 405, 284, 287, 286, 0, 282, 0, 0, 0, 0, 0, 182, 0, 289, 17, 19, 0, 0, 0, 0,
66, 0, 0, 0, 0, 2, 0, 0, 0, 0, 21, 0, 95, 96, 98, 27, 29, 31, 87, 0, 87, 152, 0,
151, 87, 38, 0, 25, 0, 0, 11, 161, 0, 0, 137, 127, 42, 150, 138, 0, 0, 34, 80, 0,
78, 0, 0, 296, 295, 285, 294, 24, 0, 0, 0, 0, 0, 42, 42, 200, 202, 0, 0, 70, 71,
160, 207, 0, 129, 0, 0, 177, 0, 0, 404, 0, 0, 0, 0, 0, 0, 0, 87, 0, 0, 401, 391,
402, 403, 0, 0, 0, 0, 374, 0, 372, 373, 0, 307, 320, 328, 330, 332, 334, 336,
338, 341, 346, 349, 352, 356, 0, 358, 375, 390, 389, 0, 3, 0, 5, 0, 22, 0, 0, 0,
12, 28, 30, 32, 86, 0, 92, 93, 0, 85, 87, 0, 113, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 405, 235, 237, 0, 243, 239, 0, 241, 224, 225, 226, 0, 227, 228,
229, 0, 305, 26, 13, 98, 167, 183, 0, 0, 163, 161, 0, 0, 130, 0, 140, 0, 0, 0,
77, 0, 0, 283, 298, 0, 297, 184, 0, 0, 296, 0, 203, 175, 0, 101, 103, 161, 0,
209, 201, 210, 68, 0, 69, 89, 0, 88, 0, 179, 0, 181, 67, 0, 0, 366, 0, 322, 356,
0, 323, 324, 0, 0, 0, 0, 0, 0, 0, 359, 360, 0, 0, 0, 0, 362, 363, 361, 146, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 310, 311, 312, 313, 314, 315,
316, 317, 318, 319, 309, 0, 364, 0, 383, 384, 0, 0, 0, 393, 0, 0, 147, 18, 0, 20,
0, 97, 99, 186, 185, 14, 0, 82, 94, 0, 0, 105, 106, 108, 0, 112, 87, 121, 0, 0,
0, 0, 0, 0, 0, 275, 276, 277, 0, 0, 279, 0, 232, 233, 0, 0, 0, 0, 244, 240, 98,
242, 238, 236, 0, 168, 0, 0, 0, 174, 162, 169, 132, 0, 0, 0, 163, 134, 136, 135,
131, 153, 142, 141, 0, 48, 47, 0, 45, 81, 327, 79, 75, 291, 0, 23, 292, 0, 0, 0,
0, 196, 300, 303, 0, 302, 0, 0, 0, 204, 102, 104, 0, 163, 0, 213, 0, 211, 161, 0,
0, 223, 206, 208, 178, 0, 0, 0, 187, 191, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
392, 399, 0, 398, 329, 0, 331, 333, 335, 337, 339, 340, 344, 345, 342, 343, 347,
348, 350, 351, 353, 354, 355, 308, 381, 382, 377, 0, 379, 380, 0, 0, 0, 4, 6, 0,
109, 100, 0, 0, 0, 114, 123, 122, 0, 0, 117, 0, 0, 0, 0, 0, 0, 0, 0, 274, 278, 0,
0, 0, 231, 0, 234, 0, 15, 306, 161, 0, 171, 0, 0, 0, 164, 133, 167, 155, 154,
143, 7, 0, 0, 0, 299, 199, 0, 301, 195, 197, 293, 0, 290, 205, 176, 218, 0, 212,
215, 0, 163, 0, 214, 0, 365, 0, 0, 188, 0, 192, 387, 388, 0, 0, 0, 0, 0, 0, 0, 0,
357, 395, 0, 0, 378, 376, 396, 0, 0, 84, 107, 110, 83, 111, 124, 121, 121, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 250, 280, 0, 0, 16, 163, 0, 172, 170, 0, 163, 0, 0, 0, 0,
44, 46, 198, 304, 217, 220, 0, 222, 221, 216, 0, 0, 0, 189, 193, 0, 0, 325, 326,
0, 369, 397, 0, 400, 321, 394, 0, 120, 119, 245, 0, 250, 260, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 281, 0, 157, 173, 165, 156, 161, 0, 49, 50, 219, 367, 368, 0,
194, 0, 0, 385, 0, 0, 0, 256, 0, 0, 262, 0, 0, 0, 270, 0, 0, 0, 0, 0, 0, 0, 0,
249, 230, 0, 163, 0, 0, 370, 0, 386, 0, 246, 0, 0, 0, 247, 261, 263, 264, 0, 272,
271, 0, 266, 0, 0, 0, 0, 250, 251, 166, 158, 0, 0, 0, 125, 0, 0, 248, 265, 273,
267, 268, 0, 0, 250, 252, 0, 190, 371, 256, 257, 269, 250, 253, 51, 256, 258,
250, 254, 259, 255, 0, 0, 0}; static short Cyc_yydefgoto[ 112u]={ 805, 41, 42,
43, 246, 44, 378, 45, 380, 46, 213, 47, 48, 64, 65, 447, 448, 49, 154, 50, 51,
131, 132, 52, 89, 53, 219, 220, 100, 101, 102, 221, 148, 390, 391, 392, 54, 55,
546, 547, 548, 56, 57, 58, 59, 125, 111, 443, 472, 60, 473, 434, 574, 426, 430,
431, 290, 267, 160, 79, 80, 489, 383, 490, 491, 460, 461, 149, 155, 156, 474,
297, 249, 250, 251, 252, 253, 254, 706, 757, 255, 256, 280, 281, 282, 463, 464,
465, 257, 258, 365, 187, 451, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197,
198, 199, 200, 201, 202, 504, 505, 203, 204}; static short Cyc_yypact[ 808u]={
1888, - -32768, - -32768, - -32768, - -32768, - 12, - -32768, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, - 20, 163, 2255, - -32768, 182, 2, -
-32768, - -32768, 46, 80, - -32768, 5, - -32768, 115, 567, - -32768, 122, 76,
141, - -32768, - -32768, 1775, - -32768, - 63, 41, - -32768, 515, - 20, - 20, -
20, - -32768, - -32768, 180, - -32768, 194, 2439, 268, 93, - 22, - -32768, 182,
169, 199, 748, - -32768, 182, 185, - -32768, 193, - -32768, - -32768, - -32768,
2851, - -32768, 206, 240, 231, 2851, 261, 264, 282, 303, - -32768, - -32768,
2318, 2318, 112, 2318, - -32768, 297, 4287, 3958, 3958, - -32768, 1775, 2000,
1775, 2000, - -32768, 287, 307, - -32768, 2379, 748, 748, 748, 115, 4287, 115, -
-32768, 182, - -32768, 251, 317, 1187, - -32768, 2439, 515, - -32768, 1827, 2318,
2870, - -32768, 268, - 20, - -32768, - -32768, 2000, 322, - -32768, 339, 326,
366, 182, 398, 2851, - -32768, - -32768, - -32768, - -32768, 415, 3958, 18, 404,
216, - 20, - 20, 304, - -32768, 73, 102, - -32768, - -32768, - -32768, 406, 59,
- -32768, 494, 331, - -32768, 4087, 433, - -32768, 3958, 4001, 436, 448, 450,
452, 454, 115, 4130, 4130, - -32768, - -32768, - -32768, - -32768, 1539, 456,
4173, 4173, - -32768, 4173, - -32768, - -32768, 464, - -32768, - 37, 498, 461,
475, 466, 437, 95, 449, 408, 35, - -32768, 734, 4173, 119, - -32768, - -32768,
151, 500, - -32768, 502, - -32768, 506, - -32768, 567, 2938, 2439, - -32768, -
-32768, - -32768, - -32768, 533, 535, 4287, - -32768, 551, 546, 115, 182, 573, -
-32768, 561, 33, 575, 2486, 577, 548, 587, 597, 3006, 2486, 153, 2486, 2486, -
61, 592, - -32768, - -32768, 604, 1306, 1306, 515, 1306, - -32768, - -32768, -
-32768, 607, - -32768, - -32768, - -32768, 243, - -32768, - -32768, - -32768,
609, 619, - -32768, - 14, 608, 605, 245, 613, 109, - -32768, 611, 667, 617, 21,
182, - -32768, 3958, 620, - -32768, - -32768, 616, 621, - -32768, 248, 954, 2851,
2318, - -32768, 626, 624, 4287, 4287, 2269, 3074, 25, - -32768, 275, - -32768, -
14, - -32768, - -32768, 2318, - -32768, 2164, - -32768, 253, - -32768, - -32768,
4287, 1539, - -32768, 4287, - -32768, - -32768, 2554, - -32768, 646, 3958, 2112,
705, 3958, 3958, 632, 1539, - -32768, - -32768, 1306, 634, 426, 3958, - -32768,
- -32768, - -32768, - -32768, 4173, 3958, 4173, 4173, 4173, 4173, 4173, 4173,
4173, 4173, 4173, 4173, 4173, 4173, 4173, 4173, 4173, 4173, 4173, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, 3958, - -32768, 138, - -32768, - -32768, 3142, 184, 3958, -
-32768, 2791, 628, - -32768, - -32768, 1775, - -32768, 1775, - -32768, - -32768,
- -32768, - -32768, - -32768, 4287, - -32768, - -32768, 3958, 643, 638, - -32768,
636, 4287, - -32768, 115, 532, 3958, 644, 3958, 3958, 715, 1425, 651, - -32768,
- -32768, - -32768, 290, 706, - -32768, 3210, - -32768, - -32768, 2486, 656,
2486, 1653, - -32768, - -32768, 2379, - -32768, - -32768, - -32768, 3958, -
-32768, 4287, 650, 298, - -32768, - -32768, 657, - -32768, 680, 674, 2052, 605,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 667, 1775, - -32768,
676, 681, 675, - -32768, - -32768, - -32768, - -32768, - -32768, 2851, - -32768,
- -32768, 694, 3958, 2851, 223, - -32768, - -32768, - -32768, 685, 682, 684, -
58, 697, - -32768, - -32768, - -32768, 690, 605, 693, - -32768, 689, 275, 1940,
2318, 3278, - -32768, - -32768, 406, - -32768, 699, 700, 708, - -32768, - -32768,
232, 2938, 440, 702, 567, 701, 457, 704, 4287, 717, 721, 4044, - -32768, -
-32768, 720, 722, 498, 68, 461, 475, 466, 437, 95, 95, 449, 449, 449, 449, 408,
408, 35, 35, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, 724, - -32768, - -32768, 132, 309, 2318, - -32768, - -32768, 725, -
-32768, - -32768, 551, 3958, 728, - -32768, - -32768, - -32768, 729, 182, 324,
484, 3958, 487, 490, 730, 3346, 3414, 349, - -32768, - -32768, 731, 726, 733, -
-32768, 723, - -32768, 2439, - -32768, - -32768, 735, 2318, - -32768, 737, - 14,
727, - -32768, - -32768, 183, - -32768, - -32768, - -32768, - -32768, 424, 736,
21, - -32768, - -32768, 739, - -32768, - -32768, - -32768, - -32768, 2205, -
-32768, - -32768, - -32768, - -32768, 738, - -32768, - -32768, 740, 605, 111, -
-32768, 743, 741, 522, 745, - -32768, 2633, - -32768, - -32768, - -32768, 2439,
4287, 4001, 810, 742, 741, 744, 2791, - -32768, - -32768, 3958, 3958, - -32768,
- -32768, - -32768, 2791, 375, - -32768, - -32768, - -32768, - -32768, - -32768,
294, 16, 532, 2486, 511, 749, 2486, 3958, 3482, 356, 3550, 367, 3618, 595, -
-32768, 758, 768, - -32768, 605, 6, - -32768, - -32768, 764, 605, 4287, 755, 757,
759, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 761, - -32768,
- -32768, - -32768, 762, 765, 3958, - -32768, - -32768, 2938, 783, - -32768, 646,
785, - -32768, - -32768, 382, - -32768, - -32768, - -32768, 786, - -32768, -
-32768, 830, 791, 595, - -32768, 520, 2486, 530, 3686, 2486, 538, 3754, 3822,
372, 2851, 782, 792, - -32768, 2486, - -32768, - -32768, 789, - -32768, 735, 804,
- -32768, - -32768, - -32768, - -32768, - -32768, 342, - -32768, 793, 4287, -
-32768, 2712, 4287, 2486, 618, 795, 800, - -32768, 2486, 2486, 544, - -32768,
2486, 2486, 547, 2486, 554, 3890, - 44, 1306, - -32768, - -32768, 680, 605, 796,
3958, - -32768, 798, - -32768, 564, - -32768, 3958, 790, 802, - -32768, - -32768,
- -32768, - -32768, 2486, - -32768, - -32768, 2486, - -32768, 2486, 2486, 572,
3958, 918, - -32768, - -32768, - -32768, 814, 403, 803, - -32768, 386, 1306, -
-32768, - -32768, - -32768, - -32768, - -32768, 2486, 396, 595, - -32768, 805, -
-32768, - -32768, 1068, - -32768, - -32768, 918, - -32768, - -32768, 618, -
-32768, 595, - -32768, - -32768, - -32768, 902, 904, - -32768}; static short Cyc_yypgoto[
112u]={ - -32768, 106, - -32768, 588, - -32768, - -32768, - -32768, - -32768, -
-32768, - 52, - 47, - 56, - -32768, 62, 14, 323, - -32768, 137, - -32768, 10, -
-32768, - -32768, - 124, - -32768, - 68, - -32768, - 150, - -32768, - -32768, -
-32768, 698, 688, 0, - -32768, - -32768, 370, - -32768, - -32768, 45, - -32768,
- -32768, 88, - 39, - 43, - -32768, - -32768, 852, - -32768, - 107, 26, - 94, -
357, 166, 337, 344, 615, - 408, - 90, - 292, 797, - -32768, - 207, - 162, - 368,
- 267, - -32768, 459, - 166, - 70, - 92, - 118, - 140, - 203, - -32768, - -32768,
- 45, - 147, - -32768, - 649, - 322, - -32768, - -32768, - 21, 660, - -32768,
329, - -32768, - -32768, 145, 48, - -32768, - 81, - 365, - 158, 625, 627, 640,
641, 645, 310, 230, 341, 347, - 113, 529, - -32768, - -32768, - -32768, - 347, -
-32768, - -32768, 31}; static short Cyc_yytable[ 4391u]={ 118, 159, 81, 316, 116,
382, 533, 317, 130, 117, 278, 119, 328, 265, 143, 150, 151, 484, 459, 124, 571,
544, 545, 529, 538, 414, 266, 770, 402, 269, 296, 61, 95, 96, 409, 335, 412, 413,
445, 218, 307, 223, 593, 415, 729, 226, 427, 118, 215, 216, 217, 116, 135, 67,
82, 299, 83, 139, 214, 248, 594, 118, 771, 247, 561, 259, 61, 331, 332, 32, 333,
123, 260, 61, 63, 336, 62, 32, 61, 577, 110, 112, 428, 313, 109, 112, 113, 366,
84, 61, 87, 158, 127, 586, 1, 147, 147, 133, 147, 418, 419, 147, 421, 323, 82,
295, 710, 446, 222, 82, 104, 105, 106, - 118, 285, 306, 596, 286, 147, 299, 158,
399, 789, 90, 293, 301, 61, 61, 61, 61, 147, 147, 400, 294, 568, 103, 97, 98,
123, 797, 186, 205, 224, 484, 485, 351, 486, 802, 94, 61, 85, 449, 804, 352, 353,
477, 395, 118, 499, 61, 302, 259, 343, 344, 303, 133, 477, 82, 385, 424, 271,
289, 92, 436, 623, 438, 631, 669, 298, 31, 500, 367, 368, 369, 86, 33, 299, 272,
147, 61, 248, 248, 35, 248, 247, 247, 450, 247, 152, 345, 346, 206, 207, 208,
209, 153, 261, 300, 291, 292, 302, 562, 302, 564, 439, 299, 670, 467, 370, 88,
222, 91, 146, 146, 526, 146, 371, 372, 146, 442, 147, 527, 483, 424, 273, 373,
537, 93, 522, 523, 524, 625, 61, 668, 542, 146, 305, 374, 410, 31, 295, 684, 411,
61, 1, 33, 396, 146, 146, 66, 384, 306, 375, 494, 462, 128, 31, 124, 31, 425,
530, 248, 33, 107, 33, 247, 108, 531, 264, 61, 31, 134, 441, - 161, 609, 32, 33,
284, - 183, - 161, 429, 470, 471, 158, - 183, 709, 110, 147, 129, 261, 712, 147,
147, 147, 158, 136, 133, 32, 450, 158, 393, 481, 147, 288, 147, 146, 82, 82, 138,
147, 147, 588, 147, 329, 459, 429, 137, 159, 543, 607, 457, 458, 616, 608, 147,
61, 420, 61, 423, 31, 264, 677, 476, 455, 424, 33, 435, 118, - 209, 424, 61, 555,
304, - 209, - 209, 88, 140, 146, 225, - 209, 677, 294, 63, 384, 118, 141, 713,
120, 116, 63, 497, 599, 121, 565, 478, 566, 122, 450, 503, 479, 142, 408, 210,
480, 600, 558, 222, 601, 620, 32, 774, 424, 90, - 87, 222, 570, 147, 157, 88, -
87, 579, 676, - 87, 293, 147, 227, 626, 88, 211, 61, 627, 61, 294, 525, 38, 39,
40, 158, 503, 635, 676, 274, 384, 276, 146, 636, 158, 147, 146, 146, 146, 308,
309, 584, 691, 158, 147, 694, 587, 146, 275, 146, 628, 424, 646, 147, 146, 146,
749, 146, 424, 698, 679, 578, 289, 329, 680, 424, 677, 503, 450, 146, 701, 492,
277, 503, 496, 741, 424, 329, 721, 800, 567, 424, 652, 61, 302, 803, 158, 724,
687, 507, 657, 725, 535, 82, 536, 424, 147, 147, 82, 481, 793, 731, 289, 1, 735,
424, 279, 653, 791, 283, 796, 287, 424, 745, 341, 342, 147, 118, 659, 660, 661,
259, 347, 348, 1, 532, 676, 299, 651, 349, 350, 146, 754, 61, 502, 424, 603, 760,
761, 146, 312, 763, 764, 318, 766, 544, 545, 384, 610, 424, 686, 549, 147, 551,
552, 319, 556, 320, 580, 321, 1, 322, 146, 330, 118, 614, 424, 782, 116, 334,
783, 146, 784, 785, 688, 214, 1, 337, 462, 61, 146, 514, 515, 516, 517, 338, 634,
147, 31, 340, 612, 795, 637, 424, 33, 639, 424, - 180, 640, 424, 339, 304, - 180,
- 180, 772, 376, 429, 377, - 180, 31, 294, 379, 38, 39, 40, 33, 672, 673, 99,
692, 424, 678, 35, 146, 146, 704, 705, 747, 730, 424, 82, 147, 788, 38, 39, 40,
393, 386, 732, 424, 387, 794, 404, 146, 753, 31, 736, 424, 755, 756, 394, 33,
762, 424, 799, 765, 424, 801, 35, 512, 513, 31, 767, 424, 384, 389, 158, 33, 398,
38, 39, 40, 778, 309, 35, 384, 147, 397, 685, 146, 786, 424, 401, 384, 403, 38,
39, 40, 689, 690, 405, 742, 16, 17, 18, 248, 518, 519, 311, 247, 406, 314, 314,
638, 520, 521, 416, 643, 645, 325, 326, 417, 422, 425, 146, 212, 432, 314, 314,
433, 314, 437, 444, 248, 453, 452, 335, 247, 440, 469, 454, 751, 248, 384, 158,
468, 247, 314, 495, 498, 534, 147, 501, 82, 147, 248, 539, 540, 248, 247, 541,
550, 247, 553, 557, 563, 559, 569, 146, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 572, 573, 21, 384, 575, 581, 583, 38, 39, 40, 585,
582, 590, 591, 592, 22, 695, 697, 32, 700, 595, 703, 24, 597, 606, 146, 27, 28,
598, 604, 613, 605, 611, 29, 30, 615, 314, 354, 355, 356, 357, 358, 359, 360,
361, 362, 363, 617, 618, 720, 621, 648, 629, 622, 624, 632, 633, 647, 650, 641,
264, 656, 364, 649, 654, 681, 662, 619, 666, 32, 667, 34, 682, 734, 683, 693,
738, 740, 664, 674, 36, 37, 671, 707, 708, 711, 714, 715, 727, 716, 146, 717,
718, 146, 314, 719, 314, 314, 314, 314, 314, 314, 314, 314, 314, 314, 314, 314,
314, 314, 314, 314, 314, 722, 723, 726, 769, 728, 743, 744, 746, 748, 758, 750,
776, 759, 780, 775, 777, 781, 779, 790, 806, 792, 807, 798, 663, 493, 388, 381,
630, 126, 773, 658, 482, 787, 655, 268, 314, 589, 665, 2, 3, 4, 114, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 704, 705, 21, 161, 162, 228, 466,
229, 230, 231, 232, 233, 234, 235, 236, 22, 163, 23, 164, 237, 506, 165, 24, 238,
508, 0, 27, 28, 166, 167, 239, 240, 168, 29, 30, 241, 169, 170, 509, 171, 510,
172, 173, 0, 0, 511, 0, 314, 0, 0, 0, 0, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
242, 174, 175, 176, 177, 32, 33, 34, 0, 243, 115, 0, 0, 178, 0, 0, 36, 245, 0, 0,
0, 0, 0, 0, 180, 0, 314, 181, 182, 183, 0, 0, 0, 0, 184, 185, 31, 70, 0, 71, 72,
0, 33, 0, 0, 0, 0, 456, 0, 73, 0, 0, 74, 75, 0, 0, 0, 457, 458, 0, 76, 0, 0, 0,
77, 78, 314, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 755, 756, 21, 161, 162, 228, 0, 229, 230, 231, 232, 233, 234, 235, 236, 22,
163, 23, 164, 237, 0, 165, 24, 238, 0, 0, 27, 28, 166, 167, 239, 240, 168, 29,
30, 241, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 314, 0, 0,
0, 0, 0, 0, 0, 0, 314, 0, 242, 174, 175, 176, 177, 32, 33, 34, 0, 243, 115, 0, 0,
178, 0, 0, 36, 245, 0, 0, 0, 0, 0, 0, 180, 0, 0, 181, 182, 183, 0, 0, 0, 0, 184,
185, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
21, 161, 162, 228, 0, 229, 230, 231, 232, 233, 234, 235, 236, 22, 163, 23, 164,
237, 0, 165, 24, 238, 0, 0, 27, 28, 166, 167, 239, 240, 168, 29, 30, 241, 169,
170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 242, 174, 175, 176, 177, 32, 33, 34, 0, 243, 115, 244, 0, 178, 0, 0, 36,
245, 0, 0, 0, 0, 0, 0, 180, 0, 0, 181, 182, 183, 0, 0, 0, 0, 184, 185, 2, 3, 4,
114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 161, 162,
228, 0, 229, 230, 231, 232, 233, 234, 235, 236, 22, 163, 23, 164, 237, 0, 165,
24, 238, 0, 0, 27, 28, 166, 167, 239, 240, 168, 29, 30, 241, 169, 170, 0, 171, 0,
172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 242,
174, 175, 176, 177, 32, 33, 34, 0, 243, 115, 0, 0, 178, 0, 0, 36, 245, 0, 0, 0,
0, 0, 0, 180, 0, 0, 181, 182, 183, 0, 0, 0, 0, 184, 185, 2, 3, 4, 114, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 161, 162, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 22, 163, 23, 164, 0, 0, 165, 24, 0, 0, 0, 27, 28, 166, 167, 0, 0,
168, 29, 30, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176, 177, 32, 33, 34, 0, 554, 0, 0,
0, 178, 0, 0, 36, 245, 0, 0, 0, 0, 0, 0, 180, 0, 0, 181, 182, 183, 0, 0, 0, 0,
184, 185, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 161, 162,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 163, 0, 164, 0, 0, 165, 0, 0, 0, 0, 27, 28,
166, 167, 0, 0, 168, 29, 30, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176, 177, 32, 33, 34,
0, 0, 327, 0, 0, 178, 0, 0, 36, 245, 0, 0, 0, 0, 0, 0, 180, 0, 0, 181, 182, 183,
0, 0, 0, 0, 184, 185, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
0, 161, 162, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 163, 0, 164, 0, 0, 165, 0, 0, 0,
0, 27, 28, 166, 167, 0, 0, 168, 29, 30, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176, 177,
32, 33, 34, 0, 0, 0, 0, 0, 178, 0, 0, 36, 245, 0, 0, 0, 0, 0, 0, 180, 0, 0, 181,
182, 183, 0, 0, 0, 0, 184, 185, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 30, 0, 0, 0, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 31, 0, 0, 22, 0, 32, 33, 34, 0, 0, 0, - 8, 0, 35, 27, 28, 36, 37, 0, 0, 0, 29,
30, 0, 38, 39, 40, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 262, 0, 21, 263, 0, 0, 0, 0, 32, 0, 34, 0, 264, 0, 0, 22, 0, 23,
0, 36, 37, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 30, 0, 0, 0, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31,
0, 0, 22, 0, 32, 33, 34, 0, 0, 0, 0, 0, 35, 27, 28, 36, 37, 0, 0, 0, 29, 30, 0,
38, 39, 40, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 262, 21, 0, 0, 0, 0, 0, 0, 32, 0, 34, 0, 264, 0, 22, 0, 23, 0, 0, 36, 37,
24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 30, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 22, 0,
32, 33, 34, 0, 0, 0, - 8, 0, 35, 27, 28, 36, 37, 0, 0, 0, 29, 30, 0, 38, 39, 40,
1, 0, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
576, 21, 0, 0, 0, 0, 0, 0, 32, 0, 34, 0, 0, 0, 22, 0, 0, 0, 0, 36, 37, 24, 0, 0,
0, 27, 28, 0, 0, 0, 1, 0, 29, 30, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 22, 0, 32, 33, 34,
0, 0, 0, 0, 0, 35, 27, 28, 36, 37, 0, 0, 0, 29, 30, 0, 38, 39, 40, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 262, 0, 0, 31, 0, 0, 0, 0,
32, 33, 34, 0, 264, 0, 0, 0, 304, - 161, 0, 36, 37, 0, 0, - 161, 0, 294, 0, 38,
39, 40, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 31, 70, 68, 71, 72,
0, 33, 0, 0, 0, 0, 0, 0, 73, 0, 22, 74, 75, 0, 0, 0, 457, 458, 0, 76, 0, 27, 28,
77, 78, 0, 0, 0, 29, 30, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
69, 70, 0, 71, 72, 0, 33, 0, 0, 0, 0, 262, 0, 73, 22, 0, 74, 75, 0, 32, 0, 34, 0,
264, 76, 27, 28, 293, 77, 78, 36, 37, 29, 30, 0, 0, 294, 144, 38, 39, 40, 2, 3,
4, 114, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0,
0, 0, 0, 32, 0, 34, 0, 0, 145, 0, 22, 0, 23, 0, 36, 37, 0, 24, 0, 0, 0, 27, 28,
0, 0, 0, 0, 0, 29, 30, 0, 0, 0, 0, 0, 0, 2, 3, 4, 114, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 32, 0, 34, 0, 0, 115,
22, 212, 23, 0, 0, 36, 37, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 30, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 161, 162, 228, 0, 229, 230, 231, 232, 233,
234, 235, 236, 0, 163, 0, 164, 237, 0, 165, 32, 238, 34, 0, 0, 115, 166, 167,
239, 240, 168, 36, 37, 241, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 242, 174, 175, 176, 177, 0, 33, 161,
162, 243, 115, 0, 0, 178, 487, 0, 0, 179, 0, 0, 163, 0, 164, 0, 180, 165, 0, 181,
182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176,
177, 0, 33, 0, 0, 0, 315, 488, 0, 178, 0, 0, 0, 179, 161, 162, 0, 457, 458, 0,
180, 0, 0, 181, 182, 183, 0, 163, 0, 164, 184, 185, 165, 0, 0, 0, 0, 0, 0, 166,
167, 0, 0, 168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 0, 0, 0,
315, 675, 0, 178, 0, 0, 0, 179, 161, 162, 0, 457, 458, 0, 180, 0, 0, 181, 182,
183, 0, 163, 0, 164, 184, 185, 165, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 0, 0,
0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 0, 0, 0, 315, 752, 0, 178, 0, 0, 0,
179, 161, 162, 0, 457, 458, 0, 180, 0, 0, 181, 182, 183, 0, 163, 0, 164, 184,
185, 165, 0, 0, 0, 0, 0, 0, 166, 167, 0, 0, 168, 0, 0, 0, 169, 170, 0, 171, 0,
172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174,
175, 176, 177, 0, 33, 0, 0, 0, 315, 0, 68, 178, 0, 0, 0, 179, 161, 162, 0, 457,
458, 0, 180, 0, 0, 181, 182, 183, 0, 163, 0, 164, 184, 185, 165, 0, 0, 0, 0, 0,
0, 166, 167, 0, 0, 168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 31, 70, 0,
71, 72, 0, 33, 0, 0, 0, 0, 0, 0, 73, 0, 0, 74, 75, 0, 31, 174, 175, 176, 177, 76,
33, 161, 162, 77, 78, 0, 0, 178, 0, 0, 0, 179, 0, 0, 163, 0, 164, 270, 180, 165,
0, 181, 182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171, 0,
172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174,
175, 176, 177, 0, 33, 161, 162, 0, 315, 0, 0, 178, 0, 0, 0, 179, 0, 0, 163, 0,
164, 0, 180, 165, 0, 181, 182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0, 169,
170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 161, 162, 407, 0, 0, 0, 178, 0, 0, 0,
179, 0, 0, 163, 0, 164, 0, 180, 165, 0, 181, 182, 183, 0, 0, 166, 167, 184, 185,
168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 161, 162, 0, 0, 0, 0,
178, 0, 0, 0, 179, 0, 0, 163, 0, 164, 475, 180, 165, 0, 181, 182, 183, 0, 0, 166,
167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 161,
162, 0, 0, 0, 0, 178, 528, 0, 0, 179, 0, 0, 163, 0, 164, 0, 180, 165, 0, 181,
182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176,
177, 0, 33, 161, 162, 0, 0, 0, 0, 178, 560, 0, 0, 179, 0, 0, 163, 0, 164, 0, 180,
165, 0, 181, 182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171,
0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31,
174, 175, 176, 177, 0, 33, 161, 162, 0, 0, 0, 0, 178, 0, 0, 0, 179, 0, 0, 163, 0,
164, 602, 180, 165, 0, 181, 182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0,
169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 161, 162, 642, 0, 0, 0, 178, 0, 0, 0,
179, 0, 0, 163, 0, 164, 0, 180, 165, 0, 181, 182, 183, 0, 0, 166, 167, 184, 185,
168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 161, 162, 644, 0, 0, 0,
178, 0, 0, 0, 179, 0, 0, 163, 0, 164, 0, 180, 165, 0, 181, 182, 183, 0, 0, 166,
167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 161,
162, 0, 0, 0, 0, 178, 696, 0, 0, 179, 0, 0, 163, 0, 164, 0, 180, 165, 0, 181,
182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176,
177, 0, 33, 161, 162, 0, 0, 0, 0, 178, 699, 0, 0, 179, 0, 0, 163, 0, 164, 0, 180,
165, 0, 181, 182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171,
0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31,
174, 175, 176, 177, 0, 33, 161, 162, 702, 0, 0, 0, 178, 0, 0, 0, 179, 0, 0, 163,
0, 164, 0, 180, 165, 0, 181, 182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0,
169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 161, 162, 0, 0, 0, 0, 178, 733, 0, 0,
179, 0, 0, 163, 0, 164, 0, 180, 165, 0, 181, 182, 183, 0, 0, 166, 167, 184, 185,
168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 161, 162, 0, 0, 0, 0,
178, 737, 0, 0, 179, 0, 0, 163, 0, 164, 0, 180, 165, 0, 181, 182, 183, 0, 0, 166,
167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 161,
162, 0, 0, 0, 0, 178, 739, 0, 0, 179, 0, 0, 163, 0, 164, 0, 180, 165, 0, 181,
182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171, 0, 172, 173,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 174, 175, 176,
177, 0, 33, 161, 162, 0, 0, 0, 0, 178, 768, 0, 0, 179, 0, 0, 163, 0, 164, 0, 180,
165, 0, 181, 182, 183, 0, 0, 166, 167, 184, 185, 168, 0, 0, 0, 169, 170, 0, 171,
0, 172, 173, 0, 0, 0, 161, 162, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 163, 0, 0, 0, 0,
31, 174, 175, 176, 177, 0, 33, 166, 167, 0, 0, 168, 0, 178, 0, 0, 170, 179, 171,
0, 172, 173, 0, 0, 180, 161, 162, 181, 182, 183, 0, 0, 0, 0, 184, 185, 0, 0, 163,
0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 166, 167, 0, 315, 168, 0, 178, 0, 0,
170, 179, 171, 0, 172, 173, 0, 0, 180, 161, 162, 181, 182, 183, 0, 0, 0, 0, 184,
185, 0, 0, 163, 0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 166, 167, 0, 619, 168,
0, 178, 0, 0, 170, 179, 171, 0, 172, 173, 0, 0, 180, 161, 162, 181, 182, 183, 0,
0, 0, 0, 184, 185, 0, 0, 163, 0, 0, 0, 0, 31, 174, 175, 176, 177, 0, 33, 166,
167, 0, 0, 168, 0, 310, 0, 0, 170, 179, 171, 0, 172, 173, 0, 0, 180, 161, 162,
181, 182, 183, 0, 0, 0, 0, 184, 185, 0, 0, 163, 0, 0, 0, 0, 31, 174, 175, 176,
177, 0, 33, 166, 167, 0, 0, 168, 0, 324, 0, 0, 170, 179, 171, 0, 172, 173, 0, 0,
180, 0, 0, 181, 182, 183, 0, 0, 0, 0, 184, 185, 0, 0, 0, 0, 0, 0, 0, 31, 174,
175, 176, 177, 0, 33, 0, 0, 0, 0, 0, 0, 178, 0, 0, 0, 179, 0, 0, 0, 0, 0, 0, 180,
0, 0, 181, 182, 183, 0, 0, 0, 0, 184, 185, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 34, 0,
0, 0, 0, 0, 0, 0, 0, 36, 37}; static short Cyc_yycheck[ 4391u]={ 56, 91, 23, 165,
56, 212, 374, 165, 64, 56, 134, 56, 178, 120, 82, 85, 86, 309, 285, 58, 428, 5,
6, 370, 389, 86, 120, 71, 231, 121, 148, 0, 95, 96, 237, 72, 239, 240, 17, 107,
158, 109, 100, 104, 693, 113, 60, 103, 104, 105, 106, 103, 73, 22, 23, 113, 25,
78, 103, 115, 468, 117, 106, 115, 411, 117, 35, 180, 181, 91, 183, 57, 117, 42,
94, 112, 88, 91, 47, 436, 102, 55, 96, 164, 53, 59, 55, 200, 86, 58, 85, 91, 61,
458, 1, 85, 86, 66, 88, 246, 247, 91, 249, 171, 73, 148, 100, 86, 108, 78, 48,
49, 50, 97, 96, 158, 473, 99, 108, 113, 120, 88, 771, 35, 99, 66, 95, 96, 97, 98,
120, 121, 99, 108, 426, 47, 95, 96, 124, 788, 92, 93, 111, 435, 310, 110, 312,
796, 42, 118, 104, 275, 801, 118, 119, 295, 224, 213, 324, 128, 101, 213, 67, 68,
105, 134, 306, 136, 213, 101, 122, 145, 96, 267, 106, 66, 541, 66, 105, 86, 327,
62, 63, 64, 104, 92, 113, 125, 178, 158, 246, 247, 99, 249, 246, 247, 277, 249,
86, 104, 105, 95, 96, 97, 98, 93, 118, 105, 146, 147, 101, 414, 101, 416, 105,
113, 105, 287, 99, 104, 220, 99, 85, 86, 86, 88, 107, 108, 91, 272, 220, 93, 302,
101, 128, 84, 386, 96, 351, 352, 353, 109, 211, 600, 394, 108, 158, 96, 95, 86,
293, 619, 99, 222, 1, 92, 225, 120, 121, 96, 212, 304, 111, 319, 285, 96, 86,
306, 86, 86, 86, 327, 92, 93, 92, 327, 96, 93, 95, 248, 86, 96, 272, 100, 491,
91, 92, 142, 95, 106, 264, 291, 292, 293, 101, 652, 102, 287, 99, 211, 657, 291,
292, 293, 304, 99, 275, 91, 389, 309, 222, 297, 302, 97, 304, 178, 285, 286, 87,
309, 310, 98, 312, 178, 591, 299, 86, 417, 396, 97, 107, 108, 498, 101, 324, 304,
248, 306, 95, 86, 95, 608, 294, 95, 101, 92, 101, 403, 95, 101, 319, 403, 99,
100, 101, 104, 95, 220, 107, 106, 627, 108, 94, 315, 420, 101, 658, 99, 420, 94,
322, 478, 104, 420, 99, 420, 108, 458, 330, 104, 98, 236, 95, 108, 478, 95, 386,
479, 501, 91, 747, 101, 304, 99, 394, 97, 386, 100, 104, 96, 443, 608, 99, 99,
394, 88, 97, 104, 101, 378, 101, 380, 108, 365, 110, 111, 112, 417, 370, 95, 627,
99, 374, 97, 287, 101, 426, 417, 291, 292, 293, 100, 101, 454, 637, 435, 426,
640, 459, 302, 101, 304, 534, 101, 95, 435, 309, 310, 106, 312, 101, 95, 614,
443, 428, 310, 614, 101, 725, 411, 541, 324, 95, 318, 98, 417, 321, 95, 101, 324,
677, 793, 424, 101, 568, 444, 101, 799, 478, 97, 105, 336, 576, 101, 378, 454,
380, 101, 478, 479, 459, 477, 106, 696, 468, 1, 699, 101, 100, 569, 97, 86, 106,
99, 101, 708, 69, 70, 498, 565, 86, 87, 88, 565, 65, 66, 1, 372, 725, 113, 565,
113, 114, 386, 727, 494, 100, 101, 480, 732, 733, 394, 99, 736, 737, 99, 739, 5,
6, 491, 100, 101, 623, 398, 534, 400, 401, 99, 403, 99, 444, 99, 1, 99, 417, 99,
612, 100, 101, 762, 612, 97, 765, 426, 767, 768, 634, 612, 1, 71, 591, 540, 435,
343, 344, 345, 346, 116, 547, 569, 86, 115, 494, 786, 100, 101, 92, 100, 101, 95,
100, 101, 117, 99, 100, 101, 743, 97, 572, 97, 106, 86, 108, 97, 110, 111, 112,
92, 86, 87, 95, 100, 101, 613, 99, 478, 479, 22, 23, 713, 100, 101, 591, 613,
771, 110, 111, 112, 540, 96, 100, 101, 97, 780, 86, 498, 726, 86, 100, 101, 22,
23, 96, 92, 100, 101, 793, 100, 101, 796, 99, 341, 342, 86, 100, 101, 608, 106,
658, 92, 99, 110, 111, 112, 100, 101, 99, 619, 658, 96, 622, 534, 100, 101, 99,
627, 99, 110, 111, 112, 635, 636, 95, 704, 17, 18, 19, 743, 347, 348, 161, 743,
95, 164, 165, 550, 349, 350, 106, 554, 555, 172, 173, 99, 97, 86, 569, 98, 100,
180, 181, 106, 183, 100, 97, 771, 100, 97, 72, 771, 109, 97, 101, 723, 780, 677,
726, 101, 780, 200, 25, 99, 104, 723, 100, 704, 726, 793, 95, 101, 796, 793, 106,
99, 796, 30, 95, 91, 42, 99, 613, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, 113, 91, 24, 725, 100, 99, 101, 110, 111, 112, 86, 100,
97, 101, 100, 37, 641, 642, 91, 644, 100, 646, 44, 100, 86, 658, 48, 49, 109,
100, 99, 101, 100, 55, 56, 101, 277, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 100,
97, 674, 100, 95, 97, 101, 100, 97, 97, 96, 105, 99, 95, 104, 98, 100, 97, 25,
100, 96, 100, 91, 100, 93, 100, 698, 100, 96, 701, 702, 109, 104, 102, 103, 109,
95, 86, 91, 101, 100, 28, 100, 723, 100, 100, 726, 335, 100, 337, 338, 339, 340,
341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 100, 99, 99,
741, 96, 106, 97, 101, 87, 97, 100, 749, 95, 106, 101, 100, 97, 755, 87, 0, 100,
0, 100, 583, 319, 220, 211, 540, 59, 746, 576, 299, 770, 572, 120, 389, 460, 591,
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
25, 26, 27, 286, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 335, 43, 44,
45, 337, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 338, 61, 339, 63,
64, - 1, - 1, 340, - 1, 458, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90, 91, 92, 93, - 1, 95, 96, - 1, -
1, 99, - 1, - 1, 102, 103, - 1, - 1, - 1, - 1, - 1, - 1, 110, - 1, 501, 113, 114,
115, - 1, - 1, - 1, - 1, 120, 121, 86, 87, - 1, 89, 90, - 1, 92, - 1, - 1, - 1,
- 1, 97, - 1, 99, - 1, - 1, 102, 103, - 1, - 1, - 1, 107, 108, - 1, 110, - 1, -
1, - 1, 114, 115, 541, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
614, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 623, - 1, 86, 87, 88, 89, 90, 91,
92, 93, - 1, 95, 96, - 1, - 1, 99, - 1, - 1, 102, 103, - 1, - 1, - 1, - 1, - 1,
- 1, 110, - 1, - 1, 113, 114, 115, - 1, - 1, - 1, - 1, 120, 121, 3, 4, 5, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27,
- 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, -
1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, - 1, 61, - 1, 63, 64, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 86, 87, 88, 89, 90, 91, 92, 93, - 1, 95, 96, 97, - 1, 99, - 1,
- 1, 102, 103, - 1, - 1, - 1, - 1, - 1, - 1, 110, - 1, - 1, 113, 114, 115, - 1,
- 1, - 1, - 1, 120, 121, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37,
38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56,
57, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90, 91,
92, 93, - 1, 95, 96, - 1, - 1, 99, - 1, - 1, 102, 103, - 1, - 1, - 1, - 1, - 1,
- 1, 110, - 1, - 1, 113, 114, 115, - 1, - 1, - 1, - 1, 120, 121, 3, 4, 5, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, 39, 40, - 1, - 1, 43, 44, -
1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, 56, - 1, 58, 59, - 1, 61, - 1, 63,
64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90, 91, 92, 93, - 1, 95, - 1, - 1, -
1, 99, - 1, - 1, 102, 103, - 1, - 1, - 1, - 1, - 1, - 1, 110, - 1, - 1, 113, 114,
115, - 1, - 1, - 1, - 1, 120, 121, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54,
55, 56, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88,
89, 90, 91, 92, 93, - 1, - 1, 96, - 1, - 1, 99, - 1, - 1, 102, 103, - 1, - 1, -
1, - 1, - 1, - 1, 110, - 1, - 1, 113, 114, 115, - 1, - 1, - 1, - 1, 120, 121, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, -
1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, 56, - 1, 58, 59, - 1, 61, - 1, 63,
64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90, 91, 92, 93, - 1, - 1, - 1, - 1,
- 1, 99, - 1, - 1, 102, 103, - 1, - 1, - 1, - 1, - 1, - 1, 110, - 1, - 1, 113,
114, 115, - 1, - 1, - 1, - 1, 120, 121, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46,
47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, 56, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 86, - 1, - 1, 37, - 1, 91, 92, 93, - 1, - 1, - 1, 97, - 1, 99, 48, 49,
102, 103, - 1, - 1, - 1, 55, 56, - 1, 110, 111, 112, 0, 1, - 1, 3, 4, 5, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 83, - 1, 24, 86, - 1, - 1, -
1, - 1, 91, - 1, 93, - 1, 95, - 1, - 1, 37, - 1, 39, - 1, 102, 103, - 1, 44, - 1,
46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, 56, - 1, - 1, - 1, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 86, - 1, - 1, 37, - 1, 91, 92, 93, - 1, - 1, - 1, - 1, - 1, 99,
48, 49, 102, 103, - 1, - 1, - 1, 55, 56, - 1, 110, 111, 112, 1, - 1, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 83, 24, - 1, - 1,
- 1, - 1, - 1, - 1, 91, - 1, 93, - 1, 95, - 1, 37, - 1, 39, - 1, - 1, 102, 103,
44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, 56, - 1, - 1, - 1, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 86, - 1, - 1, 37, - 1, 91, 92, 93, - 1, - 1, - 1, 97,
- 1, 99, 48, 49, 102, 103, - 1, - 1, - 1, 55, 56, - 1, 110, 111, 112, 1, - 1, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 83, 24, -
1, - 1, - 1, - 1, - 1, - 1, 91, - 1, 93, - 1, - 1, - 1, 37, - 1, - 1, - 1, - 1,
102, 103, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, 1, - 1, 55, 56, - 1, - 1, -
1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, - 1, - 1, 37, - 1, 91, 92, 93, - 1, - 1,
- 1, - 1, - 1, 99, 48, 49, 102, 103, - 1, - 1, - 1, 55, 56, - 1, 110, 111, 112,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 38, - 1, - 1, - 1, 83, - 1, - 1, 86, - 1, - 1, - 1, - 1, 91, 92, 93, -
1, 95, - 1, - 1, - 1, 99, 100, - 1, 102, 103, - 1, - 1, 106, - 1, 108, - 1, 110,
111, 112, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 86, 87, 38, 89,
90, - 1, 92, - 1, - 1, - 1, - 1, - 1, - 1, 99, - 1, 37, 102, 103, - 1, - 1, - 1,
107, 108, - 1, 110, - 1, 48, 49, 114, 115, - 1, - 1, - 1, 55, 56, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 86, 87, - 1, 89, 90, - 1, 92, - 1,
- 1, - 1, - 1, 83, - 1, 99, 37, - 1, 102, 103, - 1, 91, - 1, 93, - 1, 95, 110,
48, 49, 99, 114, 115, 102, 103, 55, 56, - 1, - 1, 108, 60, 110, 111, 112, 3, 4,
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1,
- 1, - 1, - 1, - 1, 91, - 1, 93, - 1, - 1, 96, - 1, 37, - 1, 39, - 1, 102, 103,
- 1, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, 56, - 1, - 1, - 1,
- 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, 91, - 1, 93, - 1, - 1, 96, 37,
98, 39, - 1, - 1, 102, 103, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1,
55, 56, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, - 1, 38, - 1, 40, 41, - 1,
43, 91, 45, 93, - 1, - 1, 96, 50, 51, 52, 53, 54, 102, 103, 57, 58, 59, - 1, 61,
- 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90, - 1, 92, 25, 26, 95,
96, - 1, - 1, 99, 32, - 1, - 1, 103, - 1, - 1, 38, - 1, 40, - 1, 110, 43, - 1,
113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, - 1, - 1, 58, 59, - 1, 61, -
1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90, - 1, 92, - 1, - 1, - 1,
96, 97, - 1, 99, - 1, - 1, - 1, 103, 25, 26, - 1, 107, 108, - 1, 110, - 1, - 1,
113, 114, 115, - 1, 38, - 1, 40, 120, 121, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50,
51, - 1, - 1, 54, - 1, - 1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 86, 87, 88, 89, 90, - 1, 92, - 1, - 1, - 1, 96, 97, - 1, 99, - 1, - 1, -
1, 103, 25, 26, - 1, 107, 108, - 1, 110, - 1, - 1, 113, 114, 115, - 1, 38, - 1,
40, 120, 121, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1,
- 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90,
- 1, 92, - 1, - 1, - 1, 96, 97, - 1, 99, - 1, - 1, - 1, 103, 25, 26, - 1, 107,
108, - 1, 110, - 1, - 1, 113, 114, 115, - 1, 38, - 1, 40, 120, 121, 43, - 1, - 1,
- 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, - 1, 58, 59, - 1, 61, - 1,
63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90, - 1, 92, - 1, - 1, - 1, 96,
- 1, 38, 99, - 1, - 1, - 1, 103, 25, 26, - 1, 107, 108, - 1, 110, - 1, - 1, 113,
114, 115, - 1, 38, - 1, 40, 120, 121, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51,
- 1, - 1, 54, - 1, - 1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, 86, 87, - 1,
89, 90, - 1, 92, - 1, - 1, - 1, - 1, - 1, - 1, 99, - 1, - 1, 102, 103, - 1, 86,
87, 88, 89, 90, 110, 92, 25, 26, 114, 115, - 1, - 1, 99, - 1, - 1, - 1, 103, - 1,
- 1, 38, - 1, 40, 109, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121,
54, - 1, - 1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87,
88, 89, 90, - 1, 92, 25, 26, - 1, 96, - 1, - 1, 99, - 1, - 1, - 1, 103, - 1, - 1,
38, - 1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, -
1, - 1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88,
89, 90, - 1, 92, 25, 26, 95, - 1, - 1, - 1, 99, - 1, - 1, - 1, 103, - 1, - 1, 38,
- 1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1,
- 1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, - 1, - 1, - 1, - 1, 99, - 1, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, 109, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, - 1, - 1, - 1, - 1, 99, 100, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, - 1, - 1, - 1, - 1, 99, 100, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, - 1, - 1, - 1, - 1, 99, - 1, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, 109, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, 95, - 1, - 1, - 1, 99, - 1, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, 95, - 1, - 1, - 1, 99, - 1, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, - 1, - 1, - 1, - 1, 99, 100, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, - 1, - 1, - 1, - 1, 99, 100, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, 95, - 1, - 1, - 1, 99, - 1, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, - 1, - 1, - 1, - 1, 99, 100, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, - 1, - 1, - 1, - 1, 99, 100, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, - 1, - 1, - 1, - 1, 99, 100, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 86, 87, 88, 89,
90, - 1, 92, 25, 26, - 1, - 1, - 1, - 1, 99, 100, - 1, - 1, 103, - 1, - 1, 38, -
1, 40, - 1, 110, 43, - 1, 113, 114, 115, - 1, - 1, 50, 51, 120, 121, 54, - 1, -
1, - 1, 58, 59, - 1, 61, - 1, 63, 64, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90, -
1, 92, 50, 51, - 1, - 1, 54, - 1, 99, - 1, - 1, 59, 103, 61, - 1, 63, 64, - 1, -
1, 110, 25, 26, 113, 114, 115, - 1, - 1, - 1, - 1, 120, 121, - 1, - 1, 38, - 1,
- 1, - 1, - 1, 86, 87, 88, 89, 90, - 1, 92, 50, 51, - 1, 96, 54, - 1, 99, - 1, -
1, 59, 103, 61, - 1, 63, 64, - 1, - 1, 110, 25, 26, 113, 114, 115, - 1, - 1, - 1,
- 1, 120, 121, - 1, - 1, 38, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90, - 1, 92, 50,
51, - 1, 96, 54, - 1, 99, - 1, - 1, 59, 103, 61, - 1, 63, 64, - 1, - 1, 110, 25,
26, 113, 114, 115, - 1, - 1, - 1, - 1, 120, 121, - 1, - 1, 38, - 1, - 1, - 1, -
1, 86, 87, 88, 89, 90, - 1, 92, 50, 51, - 1, - 1, 54, - 1, 99, - 1, - 1, 59, 103,
61, - 1, 63, 64, - 1, - 1, 110, 25, 26, 113, 114, 115, - 1, - 1, - 1, - 1, 120,
121, - 1, - 1, 38, - 1, - 1, - 1, - 1, 86, 87, 88, 89, 90, - 1, 92, 50, 51, - 1,
- 1, 54, - 1, 99, - 1, - 1, 59, 103, 61, - 1, 63, 64, - 1, - 1, 110, - 1, - 1,
113, 114, 115, - 1, - 1, - 1, - 1, 120, 121, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
86, 87, 88, 89, 90, - 1, 92, - 1, - 1, - 1, - 1, - 1, - 1, 99, - 1, - 1, - 1,
103, - 1, - 1, - 1, - 1, - 1, - 1, 110, - 1, - 1, 113, 114, 115, - 1, - 1, - 1,
- 1, 120, 121, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, 56, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 91, - 1, 93, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 102, 103};
unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex(); static int
Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static int
Cyc_yynerrs= 0; struct _tuple18{ void* f1; void* f2; } ; struct _tuple19{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int yystate;
int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short* yyss=(
short*)({ unsigned int _temp3268= 10000u; short* _temp3269=( short*)
_cycalloc_atomic( _check_times( sizeof( short), _temp3268));{ unsigned int
_temp3270= _temp3268; unsigned int i; for( i= 0; i <  _temp3270; i ++){
_temp3269[ i]=( short) 0;}}; _temp3269;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3265= 10000u; void** _temp3266=( void**) _cycalloc(
_check_times( sizeof( void*), _temp3265));{ unsigned int _temp3267= _temp3265;
unsigned int i; for( i= 0; i <  _temp3267; i ++){ _temp3266[ i]= Cyc_yylval;}};
_temp3266;}); int yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({
unsigned int _temp3262= 10000u; struct Cyc_Yyltype* _temp3263=( struct Cyc_Yyltype*)
_cycalloc_atomic( _check_times( sizeof( struct Cyc_Yyltype), _temp3262));{
unsigned int _temp3264= _temp3262; unsigned int i; for( i= 0; i <  _temp3264; i
++){ _temp3263[ i]= Cyc_yynewloc();}}; _temp3263;}); int yystacksize= 10000;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:
yyss[ _check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate;
if( yyssp_offset >=  yystacksize -  1){ if( yystacksize >=  10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 808u, yystate)]; if( yyn ==  - 32768){ goto
yydefault;} if( Cyc_yychar ==  - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar >  0? Cyc_yychar <= 
349: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 350u, Cyc_yychar)]:
234;} yyn += yychar1; if(( yyn <  0? 1: yyn >  4390)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4391u, yyn)] !=  yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4391u, yyn)]; if( yyn <  0){
if( yyn ==  - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
==  0){ goto yyerrlab;}} if( yyn ==  807){ return 0;} if( Cyc_yychar !=  0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus !=  0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 808u, yystate)]; if(
yyn ==  0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 407u, yyn)]; if( yylen >  0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset +  1) -  yylen)];} switch(
yyn){ case 1: _LL1435: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1436:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1438=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1438[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1439; _temp1439.tag= Cyc_DeclList_tok; _temp1439.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1439;});
_temp1438;}); break; case 3: _LL1437: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1441=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1441[ 0]=({ struct Cyc_DeclList_tok_struct _temp1442; _temp1442.tag= Cyc_DeclList_tok;
_temp1442.f1=({ struct Cyc_List_List* _temp1443=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1443->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1444=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1444->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1445=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1445[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1446; _temp1446.tag= Cyc_Absyn_Using_d;
_temp1446.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1446.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1446;}); _temp1445;}));
_temp1444->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1444;});
_temp1443->tl= 0; _temp1443;}); _temp1442;}); _temp1441;}); Cyc_Lex_leave_using();
break; case 4: _LL1440: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1448=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1448[ 0]=({ struct Cyc_DeclList_tok_struct _temp1449; _temp1449.tag= Cyc_DeclList_tok;
_temp1449.f1=({ struct Cyc_List_List* _temp1450=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1450->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1451=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1451->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1452=(
struct Cyc_Absyn_Using_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1452[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1453; _temp1453.tag= Cyc_Absyn_Using_d;
_temp1453.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp1453.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1453;});
_temp1452;})); _temp1451->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1451;}); _temp1450->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1450;}); _temp1449;});
_temp1448;}); break; case 5: _LL1447: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1455=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1455[ 0]=({ struct Cyc_DeclList_tok_struct _temp1456; _temp1456.tag= Cyc_DeclList_tok;
_temp1456.f1=({ struct Cyc_List_List* _temp1457=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1457->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1458=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1458->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1459=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1459[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1460; _temp1460.tag=
Cyc_Absyn_Namespace_d; _temp1460.f1=({ struct _tagged_arr* _temp1461=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1461[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1461;});
_temp1460.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1460;}); _temp1459;})); _temp1458->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1458;}); _temp1457->tl= 0; _temp1457;}); _temp1456;}); _temp1455;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1454: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1463=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1463[ 0]=({ struct Cyc_DeclList_tok_struct _temp1464; _temp1464.tag= Cyc_DeclList_tok;
_temp1464.f1=({ struct Cyc_List_List* _temp1465=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1465->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1466=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp1466->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1467=(
struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1467[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1468; _temp1468.tag=
Cyc_Absyn_Namespace_d; _temp1468.f1=({ struct _tagged_arr* _temp1469=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1469[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1469;});
_temp1468.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1468;}); _temp1467;})); _temp1466->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1466;}); _temp1465->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1465;}); _temp1464;});
_temp1463;}); break; case 7: _LL1462: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1471=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1471[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1472; _temp1472.tag= Cyc_DeclList_tok; _temp1472.f1=({ struct Cyc_List_List*
_temp1473=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1473->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1474=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1474->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1475=( struct Cyc_Absyn_ExternC_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1475[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1476; _temp1476.tag= Cyc_Absyn_ExternC_d;
_temp1476.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp1476;}); _temp1475;})); _temp1474->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line);
_temp1474;}); _temp1473->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1473;}); _temp1472;});
_temp1471;}); break; case 8: _LL1470: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1478=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1478[ 0]=({ struct Cyc_DeclList_tok_struct _temp1479; _temp1479.tag= Cyc_DeclList_tok;
_temp1479.f1= 0; _temp1479;}); _temp1478;}); break; case 9: _LL1477: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1481=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1481[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1482; _temp1482.tag= Cyc_DeclList_tok; _temp1482.f1=({ struct Cyc_List_List*
_temp1483=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1483->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1484=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1484[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1485; _temp1485.tag= Cyc_Absyn_Fn_d;
_temp1485.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1485;}); _temp1484;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1483->tl=
0; _temp1483;}); _temp1482;}); _temp1481;}); break; case 10: _LL1480: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1486: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1488=( struct Cyc_FnDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1488[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1489; _temp1489.tag= Cyc_FnDecl_tok; _temp1489.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1489;}); _temp1488;}); break; case 12: _LL1487: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1491=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1491[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1492; _temp1492.tag= Cyc_FnDecl_tok;
_temp1492.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1493=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1493->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1493;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1492;}); _temp1491;}); break; case 13: _LL1490: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1495=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1495[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1496; _temp1496.tag= Cyc_FnDecl_tok;
_temp1496.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1496;}); _temp1495;}); break; case 14: _LL1494: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1498=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1498[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1499; _temp1499.tag= Cyc_FnDecl_tok;
_temp1499.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1500=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1500->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1500;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1499;}); _temp1498;}); break; case 15: _LL1497: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
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
_temp1503;}); _temp1502;}); break; case 16: _LL1501: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1506=( struct Cyc_FnDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1506[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1507; _temp1507.tag= Cyc_FnDecl_tok;
_temp1507.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1508=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1508->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp1508;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1507;}); _temp1506;}); break; case 17: _LL1505: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1509:
Cyc_Lex_leave_using(); break; case 19: _LL1510: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1512=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp1512[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1512;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1511: Cyc_Lex_leave_namespace(); break; case 21: _LL1513: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1515=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1515[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1516; _temp1516.tag= Cyc_DeclList_tok; _temp1516.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp1516;}); _temp1515;}); break; case 22: _LL1514: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1518=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1518[ 0]=({ struct Cyc_DeclList_tok_struct _temp1519; _temp1519.tag= Cyc_DeclList_tok;
_temp1519.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1519;}); _temp1518;}); break; case 23: _LL1517: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1521=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1521[ 0]=({ struct Cyc_DeclList_tok_struct _temp1522; _temp1522.tag= Cyc_DeclList_tok;
_temp1522.f1=({ struct Cyc_List_List* _temp1523=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1523->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1523->tl=
0; _temp1523;}); _temp1522;}); _temp1521;}); break; case 24: _LL1520: { struct
Cyc_List_List* _temp1525= 0;{ struct Cyc_List_List* _temp1526= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); for( 0;
_temp1526 !=  0; _temp1526= _temp1526->tl){ struct _tagged_arr* _temp1527=(
struct _tagged_arr*) _temp1526->hd; struct _tuple1* _temp1528=({ struct _tuple1*
_temp1531=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp1531->f1=
Cyc_Absyn_rel_ns_null; _temp1531->f2= _temp1527; _temp1531;}); struct Cyc_Absyn_Vardecl*
_temp1529= Cyc_Absyn_new_vardecl( _temp1528, Cyc_Absyn_wildtyp( 0), 0);
_temp1525=({ struct Cyc_List_List* _temp1530=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1530->hd=( void*) _temp1529; _temp1530->tl=
_temp1525; _temp1530;});}} _temp1525=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1525); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1532=( struct Cyc_DeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1532[ 0]=({ struct Cyc_DeclList_tok_struct _temp1533; _temp1533.tag= Cyc_DeclList_tok;
_temp1533.f1=({ struct Cyc_List_List* _temp1534=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1534->hd=( void*) Cyc_Absyn_letv_decl(
_temp1525, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1534->tl= 0; _temp1534;}); _temp1533;});
_temp1532;}); break;} case 25: _LL1524: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1535:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1537=( struct Cyc_DeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1537[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1538; _temp1538.tag= Cyc_DeclList_tok; _temp1538.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1538;});
_temp1537;}); break; case 27: _LL1536: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1540=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1540[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1541; _temp1541.tag= Cyc_DeclSpec_tok;
_temp1541.f1=({ struct Cyc_Parse_Declaration_spec* _temp1542=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1542->sc=({ struct
Cyc_Core_Opt* _temp1543=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1543->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1543;});
_temp1542->tq= Cyc_Absyn_empty_tqual(); _temp1542->type_specs= 0; _temp1542->is_inline=
0; _temp1542->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1542;}); _temp1541;});
_temp1540;}); break; case 28: _LL1539: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc !=  0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1545=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1545[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1546; _temp1546.tag= Cyc_DeclSpec_tok; _temp1546.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1547=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1547->sc=({ struct Cyc_Core_Opt* _temp1548=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1548->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1548;});
_temp1547->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1547->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1547->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1547->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1547;}); _temp1546;}); _temp1545;}); break; case 29: _LL1544: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1550=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1550[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1551; _temp1551.tag= Cyc_DeclSpec_tok; _temp1551.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1552=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1552->sc= 0; _temp1552->tq= Cyc_Absyn_empty_tqual(); _temp1552->type_specs=({
struct Cyc_List_List* _temp1553=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1553->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1553->tl= 0;
_temp1553;}); _temp1552->is_inline= 0; _temp1552->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1552;});
_temp1551;}); _temp1550;}); break; case 30: _LL1549: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1555=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1555[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1556; _temp1556.tag= Cyc_DeclSpec_tok;
_temp1556.f1=({ struct Cyc_Parse_Declaration_spec* _temp1557=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1557->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1557->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1557->type_specs=({ struct Cyc_List_List* _temp1558=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1558->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1558->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1558;});
_temp1557->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1557->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1557;}); _temp1556;}); _temp1555;}); break; case 31: _LL1554: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1560=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1560[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1561; _temp1561.tag= Cyc_DeclSpec_tok; _temp1561.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1562=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1562->sc= 0; _temp1562->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1562->type_specs=
0; _temp1562->is_inline= 0; _temp1562->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1562;});
_temp1561;}); _temp1560;}); break; case 32: _LL1559: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1564=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1564[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1565; _temp1565.tag= Cyc_DeclSpec_tok;
_temp1565.f1=({ struct Cyc_Parse_Declaration_spec* _temp1566=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1566->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1566->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1566->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1566->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1566->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1566;}); _temp1565;}); _temp1564;}); break; case 33: _LL1563: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1568=( struct Cyc_DeclSpec_tok_struct*)
_cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1568[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1569; _temp1569.tag= Cyc_DeclSpec_tok; _temp1569.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1570=( struct Cyc_Parse_Declaration_spec*) _cycalloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1570->sc= 0; _temp1570->tq= Cyc_Absyn_empty_tqual(); _temp1570->type_specs=
0; _temp1570->is_inline= 1; _temp1570->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1570;});
_temp1569;}); _temp1568;}); break; case 34: _LL1567: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1572=( struct Cyc_DeclSpec_tok_struct*) _cycalloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1572[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1573; _temp1573.tag= Cyc_DeclSpec_tok;
_temp1573.f1=({ struct Cyc_Parse_Declaration_spec* _temp1574=( struct Cyc_Parse_Declaration_spec*)
_cycalloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1574->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1574->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1574->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1574->is_inline=
1; _temp1574->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1574;}); _temp1573;}); _temp1572;}); break; case 35: _LL1571: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1576=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1576[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1577; _temp1577.tag= Cyc_StorageClass_tok;
_temp1577.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1577;}); _temp1576;});
break; case 36: _LL1575: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1579=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1579[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1580; _temp1580.tag=
Cyc_StorageClass_tok; _temp1580.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1580;}); _temp1579;}); break; case 37: _LL1578: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1582=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1582[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1583; _temp1583.tag=
Cyc_StorageClass_tok; _temp1583.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1583;}); _temp1582;}); break; case 38: _LL1581: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1585=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1585[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1586; _temp1586.tag=
Cyc_StorageClass_tok; _temp1586.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1586;}); _temp1585;}); break; case 39: _LL1584: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1588=( struct Cyc_StorageClass_tok_struct*)
_cycalloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1588[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1589; _temp1589.tag= Cyc_StorageClass_tok;
_temp1589.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1589;}); _temp1588;});
break; case 40: _LL1587: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1591=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1591[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1592; _temp1592.tag=
Cyc_StorageClass_tok; _temp1592.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1592;}); _temp1591;}); break; case 41: _LL1590: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1594=( struct Cyc_StorageClass_tok_struct*) _cycalloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1594[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1595; _temp1595.tag=
Cyc_StorageClass_tok; _temp1595.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1595;}); _temp1594;}); break; case 42: _LL1593: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1597=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1597[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1598; _temp1598.tag=
Cyc_AttributeList_tok; _temp1598.f1= 0; _temp1598;}); _temp1597;}); break; case
43: _LL1596: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1599: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1601=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1601[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1602; _temp1602.tag=
Cyc_AttributeList_tok; _temp1602.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1602;});
_temp1601;}); break; case 45: _LL1600: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1604=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1604[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1605; _temp1605.tag=
Cyc_AttributeList_tok; _temp1605.f1=({ struct Cyc_List_List* _temp1606=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1606->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1606->tl= 0; _temp1606;}); _temp1605;}); _temp1604;});
break; case 46: _LL1603: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1608=( struct Cyc_AttributeList_tok_struct*) _cycalloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1608[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1609; _temp1609.tag=
Cyc_AttributeList_tok; _temp1609.f1=({ struct Cyc_List_List* _temp1610=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1610->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1610->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1610;}); _temp1609;});
_temp1608;}); break; case 47: _LL1607: { struct _tagged_arr _temp1612= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1612, sizeof( unsigned char)) >  4?*(( const unsigned char*)
_check_unknown_subscript( _temp1612, sizeof( unsigned char), 0)) == '_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1612, sizeof( unsigned char),
1)) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1612,
sizeof( unsigned char),( int)( _get_arr_size( _temp1612, sizeof( unsigned char))
-  2))) == '_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1612,
sizeof( unsigned char),( int)( _get_arr_size( _temp1612, sizeof( unsigned char))
-  3))) == '_': 0){ _temp1612=( struct _tagged_arr) Cyc_Std_substring( _temp1612,
2, _get_arr_size( _temp1612, sizeof( unsigned char)) -  5);}{ int i= 0; for( 0;
i <  16u; ++ i){ if( Cyc_Std_strcmp( _temp1612,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) ==  0){ yyval=( void*)({ struct
Cyc_Attribute_tok_struct* _temp1613=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1613[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1614; _temp1614.tag= Cyc_Attribute_tok; _temp1614.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1614;}); _temp1613;}); break;}} if( i ==  16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1615=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1615[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1616; _temp1616.tag= Cyc_Attribute_tok; _temp1616.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1616;}); _temp1615;});} break;}} case
48: _LL1611: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1618=(
struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1618[ 0]=({ struct Cyc_Attribute_tok_struct _temp1619; _temp1619.tag= Cyc_Attribute_tok;
_temp1619.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1619;}); _temp1618;});
break; case 49: _LL1617: { struct _tagged_arr _temp1621= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct
_tuple15 _temp1624; int _temp1625; struct _tuple15* _temp1622= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1624=*
_temp1622; _LL1626: _temp1625= _temp1624.f2; goto _LL1623; _LL1623: { void* a;
if( Cyc_Std_zstrcmp( _temp1621, _tag_arr("regparm", sizeof( unsigned char), 8u))
==  0? 1: Cyc_Std_zstrcmp( _temp1621, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) ==  0){ if( _temp1625 <  0? 1: _temp1625 >  3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 0 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1627=( struct Cyc_Absyn_Regparm_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1627[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1628; _temp1628.tag= Cyc_Absyn_Regparm_att; _temp1628.f1= _temp1625;
_temp1628;}); _temp1627;});} else{ if( Cyc_Std_zstrcmp( _temp1621, _tag_arr("aligned",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1621, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) ==  0){ if( _temp1625 <  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));}{ unsigned int j=( unsigned int) _temp1625;
for( 0;( j &  1) ==  0; j= j >>  1){;} j= j >>  1; if( j !=  0){ Cyc_Parse_err(
_tag_arr("aligned requires positive power of two", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp1629=( struct Cyc_Absyn_Aligned_att_struct*) _cycalloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp1629[ 0]=({ struct Cyc_Absyn_Aligned_att_struct
_temp1630; _temp1630.tag= Cyc_Absyn_Aligned_att; _temp1630.f1= _temp1625;
_temp1630;}); _temp1629;});}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1631=( struct Cyc_Attribute_tok_struct*) _cycalloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1631[ 0]=({ struct Cyc_Attribute_tok_struct _temp1632; _temp1632.tag= Cyc_Attribute_tok;
_temp1632.f1=( void*) a; _temp1632;}); _temp1631;}); break;}} case 50: _LL1620: {
struct _tagged_arr _temp1634= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); struct _tagged_arr
_temp1635= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); void* a; if( Cyc_Std_zstrcmp( _temp1634, _tag_arr("section",
sizeof( unsigned char), 8u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1634, _tag_arr("__section__",
sizeof( unsigned char), 12u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1636=( struct Cyc_Absyn_Section_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1636[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1637; _temp1637.tag=
Cyc_Absyn_Section_att; _temp1637.f1= _temp1635; _temp1637;}); _temp1636;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1638=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1638[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1639; _temp1639.tag= Cyc_Attribute_tok; _temp1639.f1=(
void*) a; _temp1639;}); _temp1638;}); break;} case 51: _LL1633: { struct
_tagged_arr _temp1641= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  7)]); struct _tagged_arr
_temp1642= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); int _temp1645; struct _tuple15 _temp1643=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _LL1646:
_temp1645= _temp1643.f2; goto _LL1644; _LL1644: { int _temp1649; struct _tuple15
_temp1647=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _LL1650: _temp1649= _temp1647.f2; goto _LL1648; _LL1648: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_Std_zstrcmp( _temp1641, _tag_arr("format",
sizeof( unsigned char), 7u)) ==  0? 1: Cyc_Std_zstrcmp( _temp1641, _tag_arr("__format__",
sizeof( unsigned char), 11u)) ==  0){ if( Cyc_Std_zstrcmp( _temp1642, _tag_arr("printf",
sizeof( unsigned char), 7u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct*
_temp1651=( struct Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1651[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1652; _temp1652.tag=
Cyc_Absyn_Format_att; _temp1652.f1=( void*)(( void*) Cyc_Absyn_Printf_ft);
_temp1652.f2= _temp1645; _temp1652.f3= _temp1649; _temp1652;}); _temp1651;});}
else{ if( Cyc_Std_zstrcmp( _temp1642, _tag_arr("scanf", sizeof( unsigned char),
6u)) ==  0){ a=( void*)({ struct Cyc_Absyn_Format_att_struct* _temp1653=( struct
Cyc_Absyn_Format_att_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Format_att_struct));
_temp1653[ 0]=({ struct Cyc_Absyn_Format_att_struct _temp1654; _temp1654.tag=
Cyc_Absyn_Format_att; _temp1654.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft);
_temp1654.f2= _temp1645; _temp1654.f3= _temp1649; _temp1654;}); _temp1653;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized format type", sizeof( unsigned char),
25u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).last_line));}}} else{ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1655=( struct Cyc_Attribute_tok_struct*)
_cycalloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1655[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1656; _temp1656.tag= Cyc_Attribute_tok; _temp1656.f1=(
void*) a; _temp1656;}); _temp1655;}); break;}}} case 52: _LL1640: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1658=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1658[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1659; _temp1659.tag= Cyc_TypeSpecifier_tok;
_temp1659.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1659;});
_temp1658;}); break; case 53: _LL1657: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1661=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1661[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1662; _temp1662.tag=
Cyc_TypeSpecifier_tok; _temp1662.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1662;}); _temp1661;}); break; case 54: _LL1660:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1664=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1664[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1665; _temp1665.tag= Cyc_TypeSpecifier_tok;
_temp1665.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1665;});
_temp1664;}); break; case 55: _LL1663: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1667=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1667[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1668; _temp1668.tag=
Cyc_TypeSpecifier_tok; _temp1668.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1669=( struct Cyc_Parse_Short_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1669[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1670; _temp1670.tag=
Cyc_Parse_Short_spec; _temp1670.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1670;});
_temp1669;})); _temp1668;}); _temp1667;}); break; case 56: _LL1666: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1672=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1672[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1673; _temp1673.tag= Cyc_TypeSpecifier_tok;
_temp1673.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1673;});
_temp1672;}); break; case 57: _LL1671: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1675=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1675[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1676; _temp1676.tag=
Cyc_TypeSpecifier_tok; _temp1676.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1677=( struct Cyc_Parse_Long_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1677[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1678; _temp1678.tag= Cyc_Parse_Long_spec;
_temp1678.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1678;}); _temp1677;})); _temp1676;}); _temp1675;});
break; case 58: _LL1674: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1680=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1680[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1681; _temp1681.tag=
Cyc_TypeSpecifier_tok; _temp1681.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1681;}); _temp1680;}); break; case 59: _LL1679:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1683=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1683[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1684; _temp1684.tag= Cyc_TypeSpecifier_tok;
_temp1684.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t( 0), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1684;});
_temp1683;}); break; case 60: _LL1682: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1686=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1686[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1687; _temp1687.tag=
Cyc_TypeSpecifier_tok; _temp1687.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1688=( struct Cyc_Parse_Signed_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1688[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1689; _temp1689.tag=
Cyc_Parse_Signed_spec; _temp1689.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1689;});
_temp1688;})); _temp1687;}); _temp1686;}); break; case 61: _LL1685: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1691=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1691[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1692; _temp1692.tag= Cyc_TypeSpecifier_tok;
_temp1692.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1693=( struct Cyc_Parse_Unsigned_spec_struct*) _cycalloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1693[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1694; _temp1694.tag= Cyc_Parse_Unsigned_spec; _temp1694.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1694;});
_temp1693;})); _temp1692;}); _temp1691;}); break; case 62: _LL1690: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1695:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1696: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1697: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1699=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1699[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1700; _temp1700.tag=
Cyc_TypeSpecifier_tok; _temp1700.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1700;});
_temp1699;}); break; case 66: _LL1698: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1702=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1702[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1703; _temp1703.tag=
Cyc_TypeSpecifier_tok; _temp1703.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1704=( struct Cyc_Absyn_TypedefType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1704[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1705; _temp1705.tag= Cyc_Absyn_TypedefType;
_temp1705.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp1705.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1705.f3= 0;
_temp1705;}); _temp1704;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1703;});
_temp1702;}); break; case 67: _LL1701: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1707=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1707[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1708; _temp1708.tag=
Cyc_TypeSpecifier_tok; _temp1708.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1709=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1709[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1710; _temp1710.tag= Cyc_Absyn_TupleType;
_temp1710.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); _temp1710;});
_temp1709;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1708;}); _temp1707;}); break; case 68:
_LL1706: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1712=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1712[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1713; _temp1713.tag=
Cyc_TypeSpecifier_tok; _temp1713.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1714=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1714[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1715; _temp1715.tag= Cyc_Absyn_RgnHandleType;
_temp1715.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1715;}); _temp1714;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1713;}); _temp1712;}); break; case 69: _LL1711: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1717=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1717[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1718; _temp1718.tag=
Cyc_TypeSpecifier_tok; _temp1718.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_SizeofType_struct* _temp1719=( struct Cyc_Absyn_SizeofType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SizeofType_struct)); _temp1719[ 0]=({ struct
Cyc_Absyn_SizeofType_struct _temp1720; _temp1720.tag= Cyc_Absyn_SizeofType;
_temp1720.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1720;}); _temp1719;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1718;}); _temp1717;}); break; case 70: _LL1716: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1722=( struct Cyc_Kind_tok_struct*) _cycalloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1722[ 0]=({ struct Cyc_Kind_tok_struct _temp1723; _temp1723.tag= Cyc_Kind_tok;
_temp1723.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1723;});
_temp1722;}); break; case 71: _LL1721: { struct _tagged_arr* _temp1727; void*
_temp1729; struct _tuple1 _temp1725=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1730: _temp1729=
_temp1725.f1; goto _LL1728; _LL1728: _temp1727= _temp1725.f2; goto _LL1726;
_LL1726: if( _temp1729 != ( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1731=( struct Cyc_Kind_tok_struct*)
_cycalloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1731[ 0]=({ struct Cyc_Kind_tok_struct
_temp1732; _temp1732.tag= Cyc_Kind_tok; _temp1732.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1727, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1732;}); _temp1731;}); break;} case 72:
_LL1724: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1734=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1734[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1735; _temp1735.tag= Cyc_TypeQual_tok;
_temp1735.f1=({ struct Cyc_Absyn_Tqual _temp1736; _temp1736.q_const= 1;
_temp1736.q_volatile= 0; _temp1736.q_restrict= 0; _temp1736;}); _temp1735;});
_temp1734;}); break; case 73: _LL1733: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1738=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1738[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1739; _temp1739.tag= Cyc_TypeQual_tok; _temp1739.f1=({ struct Cyc_Absyn_Tqual
_temp1740; _temp1740.q_const= 0; _temp1740.q_volatile= 1; _temp1740.q_restrict=
0; _temp1740;}); _temp1739;}); _temp1738;}); break; case 74: _LL1737: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1742=( struct Cyc_TypeQual_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1742[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1743; _temp1743.tag= Cyc_TypeQual_tok;
_temp1743.f1=({ struct Cyc_Absyn_Tqual _temp1744; _temp1744.q_const= 0;
_temp1744.q_volatile= 0; _temp1744.q_restrict= 1; _temp1744;}); _temp1743;});
_temp1742;}); break; case 75: _LL1741: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1746=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1746[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1747; _temp1747.tag=
Cyc_TypeSpecifier_tok; _temp1747.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1748=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1748[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1749; _temp1749.tag= Cyc_Parse_Decl_spec;
_temp1749.f1=({ struct Cyc_Absyn_Decl* _temp1750=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp1750->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1751=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1751[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1752; _temp1752.tag= Cyc_Absyn_Enum_d; _temp1752.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1753=( struct Cyc_Absyn_Enumdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1753->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1753->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1753->fields=({
struct Cyc_Core_Opt* _temp1754=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1754->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1754;});
_temp1753;}); _temp1752;}); _temp1751;})); _temp1750->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1750;}); _temp1749;}); _temp1748;})); _temp1747;}); _temp1746;}); break;
case 76: _LL1745: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1756=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1756[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1757; _temp1757.tag=
Cyc_TypeSpecifier_tok; _temp1757.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1758=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1758[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1759; _temp1759.tag= Cyc_Absyn_EnumType;
_temp1759.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1759.f2= 0; _temp1759;}); _temp1758;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1757;}); _temp1756;}); break; case 77: _LL1755: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1761=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1761[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1762; _temp1762.tag=
Cyc_TypeSpecifier_tok; _temp1762.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1763=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1763[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1764; _temp1764.tag= Cyc_Parse_Type_spec;
_temp1764.f1=( void*)(( void*)({ struct Cyc_Absyn_AnonEnumType_struct* _temp1765=(
struct Cyc_Absyn_AnonEnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnumType_struct));
_temp1765[ 0]=({ struct Cyc_Absyn_AnonEnumType_struct _temp1766; _temp1766.tag=
Cyc_Absyn_AnonEnumType; _temp1766.f1= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1766;});
_temp1765;})); _temp1764.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1764;});
_temp1763;})); _temp1762;}); _temp1761;}); break; case 78: _LL1760: yyval=( void*)({
struct Cyc_Enumfield_tok_struct* _temp1768=( struct Cyc_Enumfield_tok_struct*)
_cycalloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp1768[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp1769; _temp1769.tag= Cyc_Enumfield_tok; _temp1769.f1=({
struct Cyc_Absyn_Enumfield* _temp1770=( struct Cyc_Absyn_Enumfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumfield)); _temp1770->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1770->tag= 0;
_temp1770->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1770;});
_temp1769;}); _temp1768;}); break; case 79: _LL1767: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1772=( struct Cyc_Enumfield_tok_struct*) _cycalloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1772[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1773; _temp1773.tag= Cyc_Enumfield_tok;
_temp1773.f1=({ struct Cyc_Absyn_Enumfield* _temp1774=( struct Cyc_Absyn_Enumfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1774->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1774->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1774->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1774;});
_temp1773;}); _temp1772;}); break; case 80: _LL1771: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1776=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1776[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1777; _temp1777.tag=
Cyc_EnumfieldList_tok; _temp1777.f1=({ struct Cyc_List_List* _temp1778=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1778->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1778->tl= 0; _temp1778;}); _temp1777;}); _temp1776;});
break; case 81: _LL1775: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1780=( struct Cyc_EnumfieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1780[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1781; _temp1781.tag=
Cyc_EnumfieldList_tok; _temp1781.f1=({ struct Cyc_List_List* _temp1782=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1782->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1782->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1782;}); _temp1781;});
_temp1780;}); break; case 82: _LL1779: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])){ case Cyc_Parse_Struct_su:
_LL1784: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1786=( struct
Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1786[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1787; _temp1787.tag=
Cyc_Absyn_AnonStructType; _temp1787.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1787;});
_temp1786;}); break; case Cyc_Parse_Union_su: _LL1785: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1789=( struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1789[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1790; _temp1790.tag= Cyc_Absyn_AnonUnionType; _temp1790.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1790;});
_temp1789;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1791=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1791[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1792; _temp1792.tag=
Cyc_TypeSpecifier_tok; _temp1792.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1793=( struct Cyc_Parse_Type_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1793[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1794; _temp1794.tag= Cyc_Parse_Type_spec;
_temp1794.f1=( void*) t; _temp1794.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1794;});
_temp1793;})); _temp1792;}); _temp1791;}); break;} case 83: _LL1783: { struct
Cyc_List_List* _temp1796=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1797: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1799=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1799->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1799;}), _temp1796,({ struct Cyc_Core_Opt*
_temp1800=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1800->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1800;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1798: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1802=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1802->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1802;}),
_temp1796,({ struct Cyc_Core_Opt* _temp1803=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1803->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1803;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1804=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1804[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1805; _temp1805.tag=
Cyc_TypeSpecifier_tok; _temp1805.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1806=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1806[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1807; _temp1807.tag= Cyc_Parse_Decl_spec;
_temp1807.f1= d; _temp1807;}); _temp1806;})); _temp1805;}); _temp1804;}); break;}
case 84: _LL1795: { struct Cyc_List_List* _temp1809=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)])){ case Cyc_Parse_Struct_su:
_LL1810: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1812=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1812->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp1812;}), _temp1809,({ struct Cyc_Core_Opt*
_temp1813=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1813->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1813;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1811: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1815=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1815->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1815;}),
_temp1809,({ struct Cyc_Core_Opt* _temp1816=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1816->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1816;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1817=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1817[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1818; _temp1818.tag=
Cyc_TypeSpecifier_tok; _temp1818.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1819=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1819[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1820; _temp1820.tag= Cyc_Parse_Decl_spec;
_temp1820.f1= d; _temp1820;}); _temp1819;})); _temp1818;}); _temp1817;}); break;}
case 85: _LL1808: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1822: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1824=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1824[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1825; _temp1825.tag=
Cyc_TypeSpecifier_tok; _temp1825.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1826=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1826[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1827; _temp1827.tag= Cyc_Absyn_StructType;
_temp1827.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1827.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1827.f3= 0;
_temp1827;}); _temp1826;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1825;});
_temp1824;}); break; case Cyc_Parse_Union_su: _LL1823: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1829=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1829[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1830; _temp1830.tag= Cyc_TypeSpecifier_tok;
_temp1830.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1831=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1831[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1832; _temp1832.tag= Cyc_Absyn_UnionType;
_temp1832.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1832.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1832.f3= 0;
_temp1832;}); _temp1831;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1830;});
_temp1829;}); break;} break; case 86: _LL1821: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])){ case Cyc_Parse_Struct_su:
_LL1834: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1836=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1836[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1837; _temp1837.tag=
Cyc_TypeSpecifier_tok; _temp1837.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1838=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1838[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1839; _temp1839.tag= Cyc_Absyn_StructType;
_temp1839.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1839.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1839.f3= 0;
_temp1839;}); _temp1838;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1837;});
_temp1836;}); break; case Cyc_Parse_Union_su: _LL1835: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1841=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1841[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1842; _temp1842.tag= Cyc_TypeSpecifier_tok;
_temp1842.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1843=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1843[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1844; _temp1844.tag= Cyc_Absyn_UnionType;
_temp1844.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1844.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1844.f3= 0;
_temp1844;}); _temp1843;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1842;});
_temp1841;}); break;} break; case 87: _LL1833: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1846=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1846[ 0]=({ struct Cyc_TypeList_tok_struct _temp1847; _temp1847.tag= Cyc_TypeList_tok;
_temp1847.f1= 0; _temp1847;}); _temp1846;}); break; case 88: _LL1845: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1849=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1849[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1850; _temp1850.tag= Cyc_TypeList_tok; _temp1850.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1850;});
_temp1849;}); break; case 89: _LL1848:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1852=( struct Cyc_TypeList_tok_struct*) _cycalloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1852[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1853; _temp1853.tag= Cyc_TypeList_tok; _temp1853.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp1853;});
_temp1852;}); break; case 90: _LL1851: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1855=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1855[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1856; _temp1856.tag= Cyc_StructOrUnion_tok; _temp1856.f1= Cyc_Parse_Struct_su;
_temp1856;}); _temp1855;}); break; case 91: _LL1854: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1858=( struct Cyc_StructOrUnion_tok_struct*) _cycalloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1858[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1859; _temp1859.tag= Cyc_StructOrUnion_tok; _temp1859.f1= Cyc_Parse_Union_su;
_temp1859;}); _temp1858;}); break; case 92: _LL1857: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1861=( struct Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1861[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1862; _temp1862.tag= Cyc_StructFieldDeclList_tok; _temp1862.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1862;});
_temp1861;}); break; case 93: _LL1860: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1864=( struct Cyc_StructFieldDeclListList_tok_struct*) _cycalloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1864[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1865; _temp1865.tag= Cyc_StructFieldDeclListList_tok; _temp1865.f1=({
struct Cyc_List_List* _temp1866=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1866->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1866->tl= 0;
_temp1866;}); _temp1865;}); _temp1864;}); break; case 94: _LL1863: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1868=( struct Cyc_StructFieldDeclListList_tok_struct*)
_cycalloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1868[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1869; _temp1869.tag= Cyc_StructFieldDeclListList_tok;
_temp1869.f1=({ struct Cyc_List_List* _temp1870=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1870->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1870->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp1870;}); _temp1869;}); _temp1868;}); break;
case 95: _LL1867: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1872=(
struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1872[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1873; _temp1873.tag=
Cyc_InitDeclList_tok; _temp1873.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1873;});
_temp1872;}); break; case 96: _LL1871: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1875=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1875[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1876; _temp1876.tag=
Cyc_InitDeclList_tok; _temp1876.f1=({ struct Cyc_List_List* _temp1877=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1877->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1877->tl= 0; _temp1877;}); _temp1876;}); _temp1875;});
break; case 97: _LL1874: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1879=( struct Cyc_InitDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1879[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1880; _temp1880.tag=
Cyc_InitDeclList_tok; _temp1880.f1=({ struct Cyc_List_List* _temp1881=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1881->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1881->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1881;});
_temp1880;}); _temp1879;}); break; case 98: _LL1878: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1883=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1883[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1884; _temp1884.tag= Cyc_InitDecl_tok;
_temp1884.f1=({ struct _tuple14* _temp1885=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1885->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1885->f2= 0;
_temp1885;}); _temp1884;}); _temp1883;}); break; case 99: _LL1882: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1887=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1887[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1888; _temp1888.tag= Cyc_InitDecl_tok; _temp1888.f1=({ struct _tuple14*
_temp1889=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1889->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1889->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1889;});
_temp1888;}); _temp1887;}); break; case 100: _LL1886: { struct _tuple17
_temp1893; struct Cyc_List_List* _temp1894; struct Cyc_List_List* _temp1896;
struct Cyc_Absyn_Tqual _temp1898; struct _tuple17* _temp1891= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1893=*
_temp1891; _LL1899: _temp1898= _temp1893.f1; goto _LL1897; _LL1897: _temp1896=
_temp1893.f2; goto _LL1895; _LL1895: _temp1894= _temp1893.f3; goto _LL1892;
_LL1892: { struct Cyc_List_List* _temp1902; struct Cyc_List_List* _temp1904;
struct _tuple0 _temp1900=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])); _LL1905: _temp1904= _temp1900.f1; goto _LL1903; _LL1903:
_temp1902= _temp1900.f2; goto _LL1901; _LL1901: { void* _temp1906= Cyc_Parse_speclist2typ(
_temp1896, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).last_line)); struct Cyc_List_List* _temp1907=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1898, _temp1906, _temp1904, _temp1894), _temp1902);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1908=( struct
Cyc_StructFieldDeclList_tok_struct*) _cycalloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1908[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1909; _temp1909.tag=
Cyc_StructFieldDeclList_tok; _temp1909.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line), _temp1907); _temp1909;}); _temp1908;}); break;}}}
case 101: _LL1890: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1911=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1911[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1912; _temp1912.tag=
Cyc_QualSpecList_tok; _temp1912.f1=({ struct _tuple17* _temp1913=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1913->f1= Cyc_Absyn_empty_tqual();
_temp1913->f2=({ struct Cyc_List_List* _temp1914=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1914->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1914->tl=
0; _temp1914;}); _temp1913->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1913;}); _temp1912;});
_temp1911;}); break; case 102: _LL1910: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1916=( struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1916[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1917; _temp1917.tag=
Cyc_QualSpecList_tok; _temp1917.f1=({ struct _tuple17* _temp1918=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1918->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1918->f2=({
struct Cyc_List_List* _temp1919=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1919->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1919->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1919;}); _temp1918->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1918;});
_temp1917;}); _temp1916;}); break; case 103: _LL1915: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1921=( struct Cyc_QualSpecList_tok_struct*)
_cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1921[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1922; _temp1922.tag= Cyc_QualSpecList_tok;
_temp1922.f1=({ struct _tuple17* _temp1923=( struct _tuple17*) _cycalloc(
sizeof( struct _tuple17)); _temp1923->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1923->f2= 0;
_temp1923->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1923;}); _temp1922;}); _temp1921;}); break; case
104: _LL1920: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1925=(
struct Cyc_QualSpecList_tok_struct*) _cycalloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1925[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1926; _temp1926.tag=
Cyc_QualSpecList_tok; _temp1926.f1=({ struct _tuple17* _temp1927=( struct
_tuple17*) _cycalloc( sizeof( struct _tuple17)); _temp1927->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1927->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1927->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1927;});
_temp1926;}); _temp1925;}); break; case 105: _LL1924: yyval=( void*)({ struct
Cyc_InitDeclList_tok_struct* _temp1929=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1929[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1930; _temp1930.tag= Cyc_InitDeclList_tok;
_temp1930.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1930;}); _temp1929;}); break; case 106: _LL1928: yyval=(
void*)({ struct Cyc_InitDeclList_tok_struct* _temp1932=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1932[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1933; _temp1933.tag= Cyc_InitDeclList_tok;
_temp1933.f1=({ struct Cyc_List_List* _temp1934=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1934->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1934->tl= 0;
_temp1934;}); _temp1933;}); _temp1932;}); break; case 107: _LL1931: yyval=( void*)({
struct Cyc_InitDeclList_tok_struct* _temp1936=( struct Cyc_InitDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp1936[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp1937; _temp1937.tag= Cyc_InitDeclList_tok;
_temp1937.f1=({ struct Cyc_List_List* _temp1938=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1938->hd=( void*) Cyc_yyget_InitDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1938->tl=
Cyc_yyget_InitDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp1938;}); _temp1937;}); _temp1936;}); break; case 108:
_LL1935: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp1940=( struct Cyc_InitDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1940[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1941; _temp1941.tag= Cyc_InitDecl_tok; _temp1941.f1=({ struct _tuple14*
_temp1942=( struct _tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp1942->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1942->f2= 0; _temp1942;}); _temp1941;}); _temp1940;});
break; case 109: _LL1939: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1944=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1944[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1945; _temp1945.tag= Cyc_InitDecl_tok;
_temp1945.f1=({ struct _tuple14* _temp1946=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1946->f1=({ struct Cyc_Parse_Declarator*
_temp1947=( struct Cyc_Parse_Declarator*) _cycalloc( sizeof( struct Cyc_Parse_Declarator));
_temp1947->id=({ struct _tuple1* _temp1948=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp1948->f1= Cyc_Absyn_rel_ns_null; _temp1948->f2=
_init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1948;}); _temp1947->tms= 0; _temp1947;});
_temp1946->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1946;}); _temp1945;});
_temp1944;}); break; case 110: _LL1943: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1950=( struct Cyc_InitDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1950[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1951; _temp1951.tag= Cyc_InitDecl_tok;
_temp1951.f1=({ struct _tuple14* _temp1952=( struct _tuple14*) _cycalloc(
sizeof( struct _tuple14)); _temp1952->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp1952->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1952;}); _temp1951;}); _temp1950;}); break; case
111: _LL1949: { struct Cyc_List_List* _temp1954=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1955=( struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1955[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1956; _temp1956.tag=
Cyc_TypeSpecifier_tok; _temp1956.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1957=( struct Cyc_Parse_Decl_spec_struct*) _cycalloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1957[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1958; _temp1958.tag= Cyc_Parse_Decl_spec;
_temp1958.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), _temp1954,({
struct Cyc_Core_Opt* _temp1959=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1959->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1959;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1958;}); _temp1957;})); _temp1956;}); _temp1955;}); break;} case 112:
_LL1953: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1961=(
struct Cyc_TypeSpecifier_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1961[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1962; _temp1962.tag=
Cyc_TypeSpecifier_tok; _temp1962.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1963=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1963[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1964; _temp1964.tag= Cyc_Absyn_TunionType;
_temp1964.f1=({ struct Cyc_Absyn_TunionInfo _temp1965; _temp1965.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1966=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1966[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1967; _temp1967.tag= Cyc_Absyn_UnknownTunion;
_temp1967.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1968; _temp1968.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1968.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1968;});
_temp1967;}); _temp1966;})); _temp1965.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1965.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1965;}); _temp1964;}); _temp1963;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1962;}); _temp1961;}); break; case 113: _LL1960: { void* _temp1970= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp1979=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1979->v=( void*)(( void*) Cyc_Absyn_RgnKind); _temp1979;}),
0); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1971=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1971[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1972; _temp1972.tag= Cyc_TypeSpecifier_tok;
_temp1972.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1973=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1973[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1974; _temp1974.tag=
Cyc_Absyn_TunionType; _temp1974.f1=({ struct Cyc_Absyn_TunionInfo _temp1975;
_temp1975.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1976=( struct Cyc_Absyn_UnknownTunion_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1976[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1977; _temp1977.tag= Cyc_Absyn_UnknownTunion; _temp1977.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1978; _temp1978.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp1978.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]); _temp1978;}); _temp1977;}); _temp1976;})); _temp1975.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1975.rgn=(
void*) _temp1970; _temp1975;}); _temp1974;}); _temp1973;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1972;}); _temp1971;}); break;} case 114: _LL1969: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1981=( struct Cyc_TypeSpecifier_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1981[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1982; _temp1982.tag= Cyc_TypeSpecifier_tok;
_temp1982.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1983=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1983[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1984; _temp1984.tag= Cyc_Absyn_TunionFieldType; _temp1984.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1985; _temp1985.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp1986=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp1986[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp1987; _temp1987.tag= Cyc_Absyn_UnknownTunionfield; _temp1987.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp1988; _temp1988.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp1988.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]); _temp1988.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp1988;});
_temp1987;}); _temp1986;})); _temp1985.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1985;}); _temp1984;});
_temp1983;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1982;}); _temp1981;}); break; case 115:
_LL1980: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1990=( struct Cyc_Bool_tok_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1990[ 0]=({ struct
Cyc_Bool_tok_struct _temp1991; _temp1991.tag= Cyc_Bool_tok; _temp1991.f1= 0;
_temp1991;}); _temp1990;}); break; case 116: _LL1989: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1993=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1993[ 0]=({ struct Cyc_Bool_tok_struct
_temp1994; _temp1994.tag= Cyc_Bool_tok; _temp1994.f1= 1; _temp1994;}); _temp1993;});
break; case 117: _LL1992: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1996=( struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1996[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1997; _temp1997.tag= Cyc_TunionFieldList_tok; _temp1997.f1=({ struct Cyc_List_List*
_temp1998=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1998->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1998->tl= 0;
_temp1998;}); _temp1997;}); _temp1996;}); break; case 118: _LL1995: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2000=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2000[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2001; _temp2001.tag= Cyc_TunionFieldList_tok;
_temp2001.f1=({ struct Cyc_List_List* _temp2002=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2002->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2002->tl=
0; _temp2002;}); _temp2001;}); _temp2000;}); break; case 119: _LL1999: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2004=( struct Cyc_TunionFieldList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2004[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2005; _temp2005.tag= Cyc_TunionFieldList_tok;
_temp2005.f1=({ struct Cyc_List_List* _temp2006=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2006->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2006->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2006;}); _temp2005;}); _temp2004;}); break; case 120:
_LL2003: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2008=(
struct Cyc_TunionFieldList_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2008[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2009; _temp2009.tag=
Cyc_TunionFieldList_tok; _temp2009.f1=({ struct Cyc_List_List* _temp2010=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2010->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2010->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2010;}); _temp2009;});
_temp2008;}); break; case 121: _LL2007: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2012=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2012[ 0]=({ struct Cyc_Scope_tok_struct _temp2013; _temp2013.tag= Cyc_Scope_tok;
_temp2013.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2013;}); _temp2012;});
break; case 122: _LL2011: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2015=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2015[ 0]=({ struct Cyc_Scope_tok_struct _temp2016; _temp2016.tag= Cyc_Scope_tok;
_temp2016.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2016;}); _temp2015;});
break; case 123: _LL2014: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2018=( struct Cyc_Scope_tok_struct*) _cycalloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2018[ 0]=({ struct Cyc_Scope_tok_struct _temp2019; _temp2019.tag= Cyc_Scope_tok;
_temp2019.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2019;}); _temp2018;});
break; case 124: _LL2017: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2021=( struct Cyc_TunionField_tok_struct*) _cycalloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2021[ 0]=({ struct Cyc_TunionField_tok_struct _temp2022; _temp2022.tag= Cyc_TunionField_tok;
_temp2022.f1=({ struct Cyc_Absyn_Tunionfield* _temp2023=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2023->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2023->tvs= 0;
_temp2023->typs= 0; _temp2023->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2023->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2023;}); _temp2022;}); _temp2021;}); break; case 125:
_LL2020: { struct Cyc_List_List* _temp2025=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))); struct Cyc_List_List*
_temp2026=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_TunionField_tok_struct* _temp2027=( struct Cyc_TunionField_tok_struct*)
_cycalloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2027[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2028; _temp2028.tag= Cyc_TunionField_tok;
_temp2028.f1=({ struct Cyc_Absyn_Tunionfield* _temp2029=( struct Cyc_Absyn_Tunionfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2029->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2029->tvs=
_temp2026; _temp2029->typs= _temp2025; _temp2029->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2029->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp2029;}); _temp2028;}); _temp2027;}); break;}
case 126: _LL2024: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 127: _LL2030: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2032=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2032[ 0]=({ struct Cyc_Declarator_tok_struct _temp2033; _temp2033.tag= Cyc_Declarator_tok;
_temp2033.f1=({ struct Cyc_Parse_Declarator* _temp2034=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2034->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2034->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2034;});
_temp2033;}); _temp2032;}); break; case 128: _LL2031: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2036=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2036[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2037; _temp2037.tag= Cyc_Declarator_tok;
_temp2037.f1=({ struct Cyc_Parse_Declarator* _temp2038=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2038->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2038->tms= 0;
_temp2038;}); _temp2037;}); _temp2036;}); break; case 129: _LL2035: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 130:
_LL2039: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2041=( struct
Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2041[ 0]=({ struct Cyc_Declarator_tok_struct _temp2042; _temp2042.tag= Cyc_Declarator_tok;
_temp2042.f1=({ struct Cyc_Parse_Declarator* _temp2043=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2043->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->id;
_temp2043->tms=({ struct Cyc_List_List* _temp2044=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2044->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2044->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]))->tms; _temp2044;}); _temp2043;}); _temp2042;});
_temp2041;}); break; case 131: _LL2040: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2046=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2046[ 0]=({ struct Cyc_Declarator_tok_struct _temp2047; _temp2047.tag= Cyc_Declarator_tok;
_temp2047.f1=({ struct Cyc_Parse_Declarator* _temp2048=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2048->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2048->tms=({ struct Cyc_List_List* _temp2049=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2049->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2050=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2050[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2051; _temp2051.tag= Cyc_Absyn_ConstArray_mod;
_temp2051.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2051;}); _temp2050;})); _temp2049->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2049;}); _temp2048;}); _temp2047;}); _temp2046;}); break; case 132: _LL2045: {
struct _tuple16 _temp2055; struct Cyc_List_List* _temp2056; struct Cyc_Core_Opt*
_temp2058; struct Cyc_Absyn_VarargInfo* _temp2060; int _temp2062; struct Cyc_List_List*
_temp2064; struct _tuple16* _temp2053= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2055=*
_temp2053; _LL2065: _temp2064= _temp2055.f1; goto _LL2063; _LL2063: _temp2062=
_temp2055.f2; goto _LL2061; _LL2061: _temp2060= _temp2055.f3; goto _LL2059;
_LL2059: _temp2058= _temp2055.f4; goto _LL2057; _LL2057: _temp2056= _temp2055.f5;
goto _LL2054; _LL2054: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2066=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2066[ 0]=({ struct Cyc_Declarator_tok_struct _temp2067; _temp2067.tag= Cyc_Declarator_tok;
_temp2067.f1=({ struct Cyc_Parse_Declarator* _temp2068=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2068->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2068->tms=({ struct Cyc_List_List* _temp2069=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2069->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2070=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2070[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2071; _temp2071.tag= Cyc_Absyn_Function_mod;
_temp2071.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2072=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2072[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2073; _temp2073.tag= Cyc_Absyn_WithTypes;
_temp2073.f1= _temp2064; _temp2073.f2= _temp2062; _temp2073.f3= _temp2060;
_temp2073.f4= _temp2058; _temp2073.f5= _temp2056; _temp2073;}); _temp2072;}));
_temp2071;}); _temp2070;})); _temp2069->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2069;});
_temp2068;}); _temp2067;}); _temp2066;}); break;} case 133: _LL2052: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2075=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2075[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2076; _temp2076.tag= Cyc_Declarator_tok;
_temp2076.f1=({ struct Cyc_Parse_Declarator* _temp2077=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2077->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->id;
_temp2077->tms=({ struct Cyc_List_List* _temp2078=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2078->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2079=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2079[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2080; _temp2080.tag= Cyc_Absyn_Function_mod;
_temp2080.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2081=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2081[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2082; _temp2082.tag= Cyc_Absyn_WithTypes;
_temp2082.f1= 0; _temp2082.f2= 0; _temp2082.f3= 0; _temp2082.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2082.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2082;}); _temp2081;})); _temp2080;}); _temp2079;}));
_temp2078->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]))->tms; _temp2078;}); _temp2077;}); _temp2076;});
_temp2075;}); break; case 134: _LL2074: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2084=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2084[ 0]=({ struct Cyc_Declarator_tok_struct _temp2085; _temp2085.tag= Cyc_Declarator_tok;
_temp2085.f1=({ struct Cyc_Parse_Declarator* _temp2086=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2086->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2086->tms=({ struct Cyc_List_List* _temp2087=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2087->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2088=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2088[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2089; _temp2089.tag= Cyc_Absyn_Function_mod;
_temp2089.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2090=(
struct Cyc_Absyn_NoTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2090[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2091; _temp2091.tag= Cyc_Absyn_NoTypes;
_temp2091.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2091.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2091;});
_temp2090;})); _temp2089;}); _temp2088;})); _temp2087->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2087;}); _temp2086;}); _temp2085;}); _temp2084;}); break; case 135: _LL2083: {
struct Cyc_List_List* _temp2093=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2094=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2094[ 0]=({ struct Cyc_Declarator_tok_struct _temp2095; _temp2095.tag= Cyc_Declarator_tok;
_temp2095.f1=({ struct Cyc_Parse_Declarator* _temp2096=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2096->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2096->tms=({ struct Cyc_List_List* _temp2097=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2097->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2098=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2098[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2099; _temp2099.tag= Cyc_Absyn_TypeParams_mod;
_temp2099.f1= _temp2093; _temp2099.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2099.f3=
0; _temp2099;}); _temp2098;})); _temp2097->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2097;});
_temp2096;}); _temp2095;}); _temp2094;}); break;} case 136: _LL2092:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2101=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2102=( struct Cyc_Declarator_tok_struct*) _cycalloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2102[ 0]=({ struct Cyc_Declarator_tok_struct _temp2103; _temp2103.tag= Cyc_Declarator_tok;
_temp2103.f1=({ struct Cyc_Parse_Declarator* _temp2104=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2104->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->id;
_temp2104->tms=({ struct Cyc_List_List* _temp2105=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2105->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2106=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2106[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2107; _temp2107.tag= Cyc_Absyn_TypeParams_mod;
_temp2107.f1= _temp2101; _temp2107.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2107.f3=
0; _temp2107;}); _temp2106;})); _temp2105->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms; _temp2105;});
_temp2104;}); _temp2103;}); _temp2102;}); break;} case 137: _LL2100: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2109=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2109[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2110; _temp2110.tag= Cyc_Declarator_tok;
_temp2110.f1=({ struct Cyc_Parse_Declarator* _temp2111=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2111->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->id;
_temp2111->tms=({ struct Cyc_List_List* _temp2112=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2112->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2113=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2113[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2114; _temp2114.tag= Cyc_Absyn_Attributes_mod;
_temp2114.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2114.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2114;}); _temp2113;}));
_temp2112->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]))->tms; _temp2112;}); _temp2111;}); _temp2110;});
_temp2109;}); break; case 138: _LL2108: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2116=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2116[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2117; _temp2117.tag= Cyc_Declarator_tok;
_temp2117.f1=({ struct Cyc_Parse_Declarator* _temp2118=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2118->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2118->tms= 0;
_temp2118;}); _temp2117;}); _temp2116;}); break; case 139: _LL2115: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2120=( struct Cyc_Declarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2120[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2121; _temp2121.tag= Cyc_Declarator_tok;
_temp2121.f1=({ struct Cyc_Parse_Declarator* _temp2122=( struct Cyc_Parse_Declarator*)
_cycalloc( sizeof( struct Cyc_Parse_Declarator)); _temp2122->id= Cyc_Absyn_exn_name;
_temp2122->tms= 0; _temp2122;}); _temp2121;}); _temp2120;}); break; case 140:
_LL2119: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2124=(
struct Cyc_TypeModifierList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2124[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2125; _temp2125.tag=
Cyc_TypeModifierList_tok; _temp2125.f1=({ struct Cyc_List_List* _temp2126=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2126->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2127=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2127[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2128; _temp2128.tag= Cyc_Absyn_Pointer_mod;
_temp2128.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2128.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2128.f3= Cyc_Absyn_empty_tqual(); _temp2128;});
_temp2127;})); _temp2126->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2126;});
_temp2125;}); _temp2124;}); break; case 141: _LL2123: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2130=( struct Cyc_TypeModifierList_tok_struct*)
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
yyvsp_offset -  2)]); _temp2134.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2134;}); _temp2133;}));
_temp2132->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), 0);
_temp2132;}); _temp2131;}); _temp2130;}); break; case 142: _LL2129: yyval=( void*)({
struct Cyc_TypeModifierList_tok_struct* _temp2136=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2136[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2137; _temp2137.tag= Cyc_TypeModifierList_tok;
_temp2137.f1=({ struct Cyc_List_List* _temp2138=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2138->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2139=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2139[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2140; _temp2140.tag= Cyc_Absyn_Pointer_mod;
_temp2140.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2140.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2140.f3= Cyc_Absyn_empty_tqual(); _temp2140;});
_temp2139;})); _temp2138->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2138;});
_temp2137;}); _temp2136;}); break; case 143: _LL2135: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2142=( struct Cyc_TypeModifierList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2142[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2143; _temp2143.tag= Cyc_TypeModifierList_tok;
_temp2143.f1=({ struct Cyc_List_List* _temp2144=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2144->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2145=( struct Cyc_Absyn_Pointer_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2145[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2146; _temp2146.tag= Cyc_Absyn_Pointer_mod;
_temp2146.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2146.f2=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2146.f3= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2146;});
_temp2145;})); _temp2144->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line),
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2144;});
_temp2143;}); _temp2142;}); break; case 144: _LL2141: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2148=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2148[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2149; _temp2149.tag= Cyc_Pointer_Sort_tok;
_temp2149.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2150=(
struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2150[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2151; _temp2151.tag=
Cyc_Absyn_Nullable_ps; _temp2151.f1= Cyc_Absyn_exp_unsigned_one; _temp2151;});
_temp2150;})); _temp2149;}); _temp2148;}); break; case 145: _LL2147: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2153=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2153[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2154; _temp2154.tag= Cyc_Pointer_Sort_tok;
_temp2154.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2155=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2155[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2156; _temp2156.tag= Cyc_Absyn_NonNullable_ps; _temp2156.f1= Cyc_Absyn_exp_unsigned_one;
_temp2156;}); _temp2155;})); _temp2154;}); _temp2153;}); break; case 146:
_LL2152: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2158=( struct
Cyc_Pointer_Sort_tok_struct*) _cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2158[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2159; _temp2159.tag=
Cyc_Pointer_Sort_tok; _temp2159.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2160=( struct Cyc_Absyn_Nullable_ps_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2160[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2161; _temp2161.tag=
Cyc_Absyn_Nullable_ps; _temp2161.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2161;});
_temp2160;})); _temp2159;}); _temp2158;}); break; case 147: _LL2157: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2163=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2163[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2164; _temp2164.tag= Cyc_Pointer_Sort_tok;
_temp2164.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2165=( struct Cyc_Absyn_NonNullable_ps_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2165[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2166; _temp2166.tag= Cyc_Absyn_NonNullable_ps; _temp2166.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2166;});
_temp2165;})); _temp2164;}); _temp2163;}); break; case 148: _LL2162: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2168=( struct Cyc_Pointer_Sort_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2168[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2169; _temp2169.tag= Cyc_Pointer_Sort_tok;
_temp2169.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2169;});
_temp2168;}); break; case 149: _LL2167: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2171=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2171[ 0]=({ struct Cyc_Type_tok_struct _temp2172; _temp2172.tag= Cyc_Type_tok;
_temp2172.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2173=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2173->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2173;}), 0); _temp2172;}); _temp2171;});
break; case 150: _LL2170: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 151: _LL2174: Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind);
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
152: _LL2175: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2177=( struct
Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2177[
0]=({ struct Cyc_Type_tok_struct _temp2178; _temp2178.tag= Cyc_Type_tok;
_temp2178.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2179=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2179->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2179;}), 0); _temp2178;}); _temp2177;});
break; case 153: _LL2176: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 154: _LL2180: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2182=( struct Cyc_TypeQual_tok_struct*) _cycalloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2182[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2183; _temp2183.tag= Cyc_TypeQual_tok; _temp2183.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2183;});
_temp2182;}); break; case 155: _LL2181: yyval=( void*)({ struct Cyc_YY1_struct*
_temp2185=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct Cyc_YY1_struct));
_temp2185[ 0]=({ struct Cyc_YY1_struct _temp2186; _temp2186.tag= Cyc_YY1;
_temp2186.f1=({ struct _tuple16* _temp2187=( struct _tuple16*) _cycalloc(
sizeof( struct _tuple16)); _temp2187->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2187->f2= 0;
_temp2187->f3= 0; _temp2187->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2187->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2187;});
_temp2186;}); _temp2185;}); break; case 156: _LL2184: yyval=( void*)({ struct
Cyc_YY1_struct* _temp2189=( struct Cyc_YY1_struct*) _cycalloc( sizeof( struct
Cyc_YY1_struct)); _temp2189[ 0]=({ struct Cyc_YY1_struct _temp2190; _temp2190.tag=
Cyc_YY1; _temp2190.f1=({ struct _tuple16* _temp2191=( struct _tuple16*)
_cycalloc( sizeof( struct _tuple16)); _temp2191->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)])); _temp2191->f2= 1;
_temp2191->f3= 0; _temp2191->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2191->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2191;});
_temp2190;}); _temp2189;}); break; case 157: _LL2188: { struct _tuple2 _temp2195;
void* _temp2196; struct Cyc_Absyn_Tqual _temp2198; struct Cyc_Core_Opt*
_temp2200; struct _tuple2* _temp2193= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2195=*
_temp2193; _LL2201: _temp2200= _temp2195.f1; goto _LL2199; _LL2199: _temp2198=
_temp2195.f2; goto _LL2197; _LL2197: _temp2196= _temp2195.f3; goto _LL2194;
_LL2194: { struct Cyc_Absyn_VarargInfo* _temp2202=({ struct Cyc_Absyn_VarargInfo*
_temp2206=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2206->name= _temp2200; _temp2206->tq= _temp2198; _temp2206->type=( void*)
_temp2196; _temp2206->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2206;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2203=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2203[ 0]=({ struct Cyc_YY1_struct
_temp2204; _temp2204.tag= Cyc_YY1; _temp2204.f1=({ struct _tuple16* _temp2205=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2205->f1= 0;
_temp2205->f2= 0; _temp2205->f3= _temp2202; _temp2205->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2205->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2205;}); _temp2204;}); _temp2203;}); break;}} case 158:
_LL2192: { struct _tuple2 _temp2210; void* _temp2211; struct Cyc_Absyn_Tqual
_temp2213; struct Cyc_Core_Opt* _temp2215; struct _tuple2* _temp2208= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2210=*
_temp2208; _LL2216: _temp2215= _temp2210.f1; goto _LL2214; _LL2214: _temp2213=
_temp2210.f2; goto _LL2212; _LL2212: _temp2211= _temp2210.f3; goto _LL2209;
_LL2209: { struct Cyc_Absyn_VarargInfo* _temp2217=({ struct Cyc_Absyn_VarargInfo*
_temp2221=( struct Cyc_Absyn_VarargInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2221->name= _temp2215; _temp2221->tq= _temp2213; _temp2221->type=( void*)
_temp2211; _temp2221->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2221;});
yyval=( void*)({ struct Cyc_YY1_struct* _temp2218=( struct Cyc_YY1_struct*)
_cycalloc( sizeof( struct Cyc_YY1_struct)); _temp2218[ 0]=({ struct Cyc_YY1_struct
_temp2219; _temp2219.tag= Cyc_YY1; _temp2219.f1=({ struct _tuple16* _temp2220=(
struct _tuple16*) _cycalloc( sizeof( struct _tuple16)); _temp2220->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)])); _temp2220->f2=
0; _temp2220->f3= _temp2217; _temp2220->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2220->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2220;});
_temp2219;}); _temp2218;}); break;}} case 159: _LL2207: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2223=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2223[ 0]=({ struct Cyc_Type_tok_struct
_temp2224; _temp2224.tag= Cyc_Type_tok; _temp2224.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*)({ struct Cyc_Absyn_Unknown_kb_struct* _temp2225=( struct Cyc_Absyn_Unknown_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Unknown_kb_struct)); _temp2225[ 0]=({ struct
Cyc_Absyn_Unknown_kb_struct _temp2226; _temp2226.tag= Cyc_Absyn_Unknown_kb;
_temp2226.f1= 0; _temp2226;}); _temp2225;})); _temp2224;}); _temp2223;}); break;
case 160: _LL2222: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2228=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2228[ 0]=({ struct Cyc_Type_tok_struct _temp2229; _temp2229.tag= Cyc_Type_tok;
_temp2229.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2230=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2230[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2231; _temp2231.tag= Cyc_Absyn_Eq_kb; _temp2231.f1=( void*) Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2231;});
_temp2230;})); _temp2229;}); _temp2228;}); break; case 161: _LL2227: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2233=( struct Cyc_TypeOpt_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2233[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2234; _temp2234.tag= Cyc_TypeOpt_tok; _temp2234.f1= 0; _temp2234;});
_temp2233;}); break; case 162: _LL2232: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2236=( struct Cyc_TypeOpt_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2236[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2237; _temp2237.tag= Cyc_TypeOpt_tok;
_temp2237.f1=({ struct Cyc_Core_Opt* _temp2238=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2238->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2239=( struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2239[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2240; _temp2240.tag= Cyc_Absyn_JoinEff;
_temp2240.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2240;}); _temp2239;})); _temp2238;}); _temp2237;});
_temp2236;}); break; case 163: _LL2235: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2242=( struct Cyc_Rgnorder_tok_struct*) _cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2242[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2243; _temp2243.tag= Cyc_Rgnorder_tok;
_temp2243.f1= 0; _temp2243;}); _temp2242;}); break; case 164: _LL2241: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 165:
_LL2244: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2246=( struct Cyc_Rgnorder_tok_struct*)
_cycalloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2246[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2247; _temp2247.tag= Cyc_Rgnorder_tok; _temp2247.f1=({ struct Cyc_List_List*
_temp2248=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2248->hd=( void*)({ struct _tuple18* _temp2249=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2249->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2252=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2252[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2253; _temp2253.tag= Cyc_Absyn_Eq_kb; _temp2253.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2253;}); _temp2252;})); _temp2249->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2250=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2250[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2251; _temp2251.tag= Cyc_Absyn_Eq_kb; _temp2251.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2251;}); _temp2250;})); _temp2249;}); _temp2248->tl= 0; _temp2248;});
_temp2247;}); _temp2246;}); break; case 166: _LL2245: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2255=( struct Cyc_Rgnorder_tok_struct*) _cycalloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2255[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2256; _temp2256.tag= Cyc_Rgnorder_tok; _temp2256.f1=({ struct Cyc_List_List*
_temp2257=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2257->hd=( void*)({ struct _tuple18* _temp2258=( struct _tuple18*)
_cycalloc( sizeof( struct _tuple18)); _temp2258->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2261=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2261[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2262; _temp2262.tag= Cyc_Absyn_Eq_kb; _temp2262.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2262;}); _temp2261;})); _temp2258->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp2259=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2259[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2260; _temp2260.tag= Cyc_Absyn_Eq_kb; _temp2260.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2260;}); _temp2259;})); _temp2258;}); _temp2257->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2257;});
_temp2256;}); _temp2255;}); break; case 167: _LL2254: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2264=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2264[ 0]=({ struct Cyc_Bool_tok_struct
_temp2265; _temp2265.tag= Cyc_Bool_tok; _temp2265.f1= 0; _temp2265;}); _temp2264;});
break; case 168: _LL2263: { struct _tagged_arr _temp2267= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Std_zstrcmp(
_temp2267, _tag_arr("inject", sizeof( unsigned char), 7u)) !=  0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2268=( struct Cyc_Bool_tok_struct*) _cycalloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2268[ 0]=({ struct Cyc_Bool_tok_struct _temp2269; _temp2269.tag= Cyc_Bool_tok;
_temp2269.f1= 1; _temp2269;}); _temp2268;}); break;} case 169: _LL2266: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 170:
_LL2270: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2272=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2272[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2273; _temp2273.tag= Cyc_TypeList_tok; _temp2273.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2273;});
_temp2272;}); break; case 171: _LL2271: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2275=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2275[ 0]=({ struct Cyc_TypeList_tok_struct _temp2276; _temp2276.tag= Cyc_TypeList_tok;
_temp2276.f1= 0; _temp2276;}); _temp2275;}); break; case 172: _LL2274: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case
173: _LL2277: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2279=(
struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2279[ 0]=({ struct Cyc_TypeList_tok_struct _temp2280; _temp2280.tag= Cyc_TypeList_tok;
_temp2280.f1=({ struct Cyc_List_List* _temp2281=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2281->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp2282=( struct Cyc_Absyn_RgnsEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp2282[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp2283; _temp2283.tag= Cyc_Absyn_RgnsEff; _temp2283.f1=(
void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2283;}); _temp2282;})); _temp2281->tl= 0; _temp2281;});
_temp2280;}); _temp2279;}); break; case 174: _LL2278: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_EffKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2285=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2285[ 0]=({ struct Cyc_TypeList_tok_struct _temp2286; _temp2286.tag= Cyc_TypeList_tok;
_temp2286.f1=({ struct Cyc_List_List* _temp2287=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2287->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2287->tl= 0;
_temp2287;}); _temp2286;}); _temp2285;}); break; case 175: _LL2284: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2289=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2289[ 0]=({ struct Cyc_TypeList_tok_struct _temp2290; _temp2290.tag= Cyc_TypeList_tok;
_temp2290.f1=({ struct Cyc_List_List* _temp2291=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2291->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2292=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2292[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2293; _temp2293.tag= Cyc_Absyn_AccessEff;
_temp2293.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2293;}); _temp2292;})); _temp2291->tl= 0;
_temp2291;}); _temp2290;}); _temp2289;}); break; case 176: _LL2288: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),( void*) Cyc_Absyn_RgnKind); yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2295=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2295[ 0]=({ struct Cyc_TypeList_tok_struct _temp2296; _temp2296.tag= Cyc_TypeList_tok;
_temp2296.f1=({ struct Cyc_List_List* _temp2297=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2297->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2298=( struct Cyc_Absyn_AccessEff_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2298[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2299; _temp2299.tag= Cyc_Absyn_AccessEff;
_temp2299.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2299;}); _temp2298;})); _temp2297->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2297;});
_temp2296;}); _temp2295;}); break; case 177: _LL2294: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2301=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2301[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2302; _temp2302.tag= Cyc_ParamDeclList_tok;
_temp2302.f1=({ struct Cyc_List_List* _temp2303=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2303->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2303->tl= 0;
_temp2303;}); _temp2302;}); _temp2301;}); break; case 178: _LL2300: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2305=( struct Cyc_ParamDeclList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2305[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2306; _temp2306.tag= Cyc_ParamDeclList_tok;
_temp2306.f1=({ struct Cyc_List_List* _temp2307=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2307->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2307->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2307;}); _temp2306;}); _temp2305;}); break; case 179:
_LL2304: { struct _tuple17 _temp2311; struct Cyc_List_List* _temp2312; struct
Cyc_List_List* _temp2314; struct Cyc_Absyn_Tqual _temp2316; struct _tuple17*
_temp2309= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2311=* _temp2309; _LL2317: _temp2316=
_temp2311.f1; goto _LL2315; _LL2315: _temp2314= _temp2311.f2; goto _LL2313;
_LL2313: _temp2312= _temp2311.f3; goto _LL2310; _LL2310: { struct Cyc_Parse_Declarator
_temp2320; struct Cyc_List_List* _temp2321; struct _tuple1* _temp2323; struct
Cyc_Parse_Declarator* _temp2318= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2320=* _temp2318;
_LL2324: _temp2323= _temp2320.id; goto _LL2322; _LL2322: _temp2321= _temp2320.tms;
goto _LL2319; _LL2319: { void* _temp2325= Cyc_Parse_speclist2typ( _temp2314, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2328; struct Cyc_List_List* _temp2330; void*
_temp2332; struct Cyc_Absyn_Tqual _temp2334; struct _tuple6 _temp2326= Cyc_Parse_apply_tms(
_temp2316, _temp2325, _temp2312, _temp2321); _LL2335: _temp2334= _temp2326.f1;
goto _LL2333; _LL2333: _temp2332= _temp2326.f2; goto _LL2331; _LL2331: _temp2330=
_temp2326.f3; goto _LL2329; _LL2329: _temp2328= _temp2326.f4; goto _LL2327;
_LL2327: if( _temp2330 !=  0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2323)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Core_Opt* _temp2336=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2340=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2340->v=( void*)(* _temp2323).f2; _temp2340;}); if( _temp2328 !=  0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2337=( struct Cyc_ParamDecl_tok_struct*) _cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2337[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2338; _temp2338.tag= Cyc_ParamDecl_tok;
_temp2338.f1=({ struct _tuple2* _temp2339=( struct _tuple2*) _cycalloc( sizeof(
struct _tuple2)); _temp2339->f1= _temp2336; _temp2339->f2= _temp2334; _temp2339->f3=
_temp2332; _temp2339;}); _temp2338;}); _temp2337;}); break;}}}} case 180:
_LL2308: { struct _tuple17 _temp2344; struct Cyc_List_List* _temp2345; struct
Cyc_List_List* _temp2347; struct Cyc_Absyn_Tqual _temp2349; struct _tuple17*
_temp2342= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2344=* _temp2342; _LL2350: _temp2349= _temp2344.f1;
goto _LL2348; _LL2348: _temp2347= _temp2344.f2; goto _LL2346; _LL2346: _temp2345=
_temp2344.f3; goto _LL2343; _LL2343: { void* _temp2351= Cyc_Parse_speclist2typ(
_temp2347, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2345 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2352=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2352[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2353; _temp2353.tag= Cyc_ParamDecl_tok; _temp2353.f1=({
struct _tuple2* _temp2354=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2354->f1= 0; _temp2354->f2= _temp2349; _temp2354->f3= _temp2351; _temp2354;});
_temp2353;}); _temp2352;}); break;}} case 181: _LL2341: { struct _tuple17
_temp2358; struct Cyc_List_List* _temp2359; struct Cyc_List_List* _temp2361;
struct Cyc_Absyn_Tqual _temp2363; struct _tuple17* _temp2356= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2358=*
_temp2356; _LL2364: _temp2363= _temp2358.f1; goto _LL2362; _LL2362: _temp2361=
_temp2358.f2; goto _LL2360; _LL2360: _temp2359= _temp2358.f3; goto _LL2357;
_LL2357: { void* _temp2365= Cyc_Parse_speclist2typ( _temp2361, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
struct Cyc_List_List* _temp2366=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2369; struct Cyc_List_List* _temp2371; void* _temp2373; struct Cyc_Absyn_Tqual
_temp2375; struct _tuple6 _temp2367= Cyc_Parse_apply_tms( _temp2363, _temp2365,
_temp2359, _temp2366); _LL2376: _temp2375= _temp2367.f1; goto _LL2374; _LL2374:
_temp2373= _temp2367.f2; goto _LL2372; _LL2372: _temp2371= _temp2367.f3; goto
_LL2370; _LL2370: _temp2369= _temp2367.f4; goto _LL2368; _LL2368: if( _temp2371
!=  0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2369 !=  0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2377=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2377[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2378; _temp2378.tag= Cyc_ParamDecl_tok; _temp2378.f1=({
struct _tuple2* _temp2379=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2379->f1= 0; _temp2379->f2= _temp2375; _temp2379->f3= _temp2373; _temp2379;});
_temp2378;}); _temp2377;}); break;}} case 182: _LL2355: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2381=( struct Cyc_IdList_tok_struct*) _cycalloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2381[ 0]=({ struct Cyc_IdList_tok_struct
_temp2382; _temp2382.tag= Cyc_IdList_tok; _temp2382.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2382;});
_temp2381;}); break; case 183: _LL2380: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2384=( struct Cyc_IdList_tok_struct*) _cycalloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2384[ 0]=({ struct Cyc_IdList_tok_struct _temp2385; _temp2385.tag= Cyc_IdList_tok;
_temp2385.f1=({ struct Cyc_List_List* _temp2386=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2386->hd=( void*)({ struct
_tagged_arr* _temp2387=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2387[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2387;}); _temp2386->tl=
0; _temp2386;}); _temp2385;}); _temp2384;}); break; case 184: _LL2383: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2389=( struct Cyc_IdList_tok_struct*)
_cycalloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2389[ 0]=({ struct Cyc_IdList_tok_struct
_temp2390; _temp2390.tag= Cyc_IdList_tok; _temp2390.f1=({ struct Cyc_List_List*
_temp2391=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2391->hd=( void*)({ struct _tagged_arr* _temp2392=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp2392[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2392;});
_temp2391->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2391;}); _temp2390;}); _temp2389;}); break; case 185:
_LL2388: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 186: _LL2393: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 187: _LL2394: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2396=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2396[ 0]=({ struct Cyc_Exp_tok_struct _temp2397; _temp2397.tag= Cyc_Exp_tok;
_temp2397.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2398=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2398[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2399; _temp2399.tag= Cyc_Absyn_UnresolvedMem_e; _temp2399.f1= 0; _temp2399.f2=
0; _temp2399;}); _temp2398;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2397;});
_temp2396;}); break; case 188: _LL2395: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2401=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2401[ 0]=({ struct Cyc_Exp_tok_struct _temp2402; _temp2402.tag= Cyc_Exp_tok;
_temp2402.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2403=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2403[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2404; _temp2404.tag= Cyc_Absyn_UnresolvedMem_e; _temp2404.f1= 0; _temp2404.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2404;});
_temp2403;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2402;}); _temp2401;}); break; case 189:
_LL2400: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2406=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2406[ 0]=({ struct Cyc_Exp_tok_struct
_temp2407; _temp2407.tag= Cyc_Exp_tok; _temp2407.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2408=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2408[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2409; _temp2409.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2409.f1= 0; _temp2409.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp2409;});
_temp2408;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2407;}); _temp2406;}); break; case 190:
_LL2405: { struct Cyc_Absyn_Vardecl* _temp2411= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2417=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp2417->f1=( void*) Cyc_Absyn_Loc_n; _temp2417->f2=({ struct _tagged_arr*
_temp2418=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp2418[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  5)]); _temp2418;}); _temp2417;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).last_line)));
_temp2411->tq=({ struct Cyc_Absyn_Tqual _temp2412; _temp2412.q_const= 1;
_temp2412.q_volatile= 0; _temp2412.q_restrict= 1; _temp2412;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2413=( struct Cyc_Exp_tok_struct*) _cycalloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2413[ 0]=({ struct Cyc_Exp_tok_struct
_temp2414; _temp2414.tag= Cyc_Exp_tok; _temp2414.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2415=( struct Cyc_Absyn_Comprehension_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2415[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2416; _temp2416.tag= Cyc_Absyn_Comprehension_e;
_temp2416.f1= _temp2411; _temp2416.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2416.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2416;});
_temp2415;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2414;}); _temp2413;}); break;} case
191: _LL2410: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2420=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2420[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2421; _temp2421.tag=
Cyc_InitializerList_tok; _temp2421.f1=({ struct Cyc_List_List* _temp2422=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2422->hd=(
void*)({ struct _tuple19* _temp2423=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2423->f1= 0; _temp2423->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2423;}); _temp2422->tl=
0; _temp2422;}); _temp2421;}); _temp2420;}); break; case 192: _LL2419: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2425=( struct Cyc_InitializerList_tok_struct*)
_cycalloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2425[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2426; _temp2426.tag= Cyc_InitializerList_tok;
_temp2426.f1=({ struct Cyc_List_List* _temp2427=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2427->hd=( void*)({ struct
_tuple19* _temp2428=( struct _tuple19*) _cycalloc( sizeof( struct _tuple19));
_temp2428->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2428->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2428;});
_temp2427->tl= 0; _temp2427;}); _temp2426;}); _temp2425;}); break; case 193:
_LL2424: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2430=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2430[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2431; _temp2431.tag=
Cyc_InitializerList_tok; _temp2431.f1=({ struct Cyc_List_List* _temp2432=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2432->hd=(
void*)({ struct _tuple19* _temp2433=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2433->f1= 0; _temp2433->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2433;}); _temp2432->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2432;}); _temp2431;}); _temp2430;}); break; case 194:
_LL2429: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2435=(
struct Cyc_InitializerList_tok_struct*) _cycalloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2435[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2436; _temp2436.tag=
Cyc_InitializerList_tok; _temp2436.f1=({ struct Cyc_List_List* _temp2437=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2437->hd=(
void*)({ struct _tuple19* _temp2438=( struct _tuple19*) _cycalloc( sizeof(
struct _tuple19)); _temp2438->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2438->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2438;});
_temp2437->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2437;});
_temp2436;}); _temp2435;}); break; case 195: _LL2434: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2440=( struct Cyc_DesignatorList_tok_struct*)
_cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2440[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2441; _temp2441.tag= Cyc_DesignatorList_tok;
_temp2441.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp2441;});
_temp2440;}); break; case 196: _LL2439: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2443=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2443[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2444; _temp2444.tag=
Cyc_DesignatorList_tok; _temp2444.f1=({ struct Cyc_List_List* _temp2445=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2445->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2445->tl= 0; _temp2445;}); _temp2444;}); _temp2443;});
break; case 197: _LL2442: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2447=( struct Cyc_DesignatorList_tok_struct*) _cycalloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2447[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2448; _temp2448.tag=
Cyc_DesignatorList_tok; _temp2448.f1=({ struct Cyc_List_List* _temp2449=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2449->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2449->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2449;});
_temp2448;}); _temp2447;}); break; case 198: _LL2446: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2451=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2451[ 0]=({ struct
Cyc_Designator_tok_struct _temp2452; _temp2452.tag= Cyc_Designator_tok;
_temp2452.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2453=(
struct Cyc_Absyn_ArrayElement_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2453[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2454; _temp2454.tag=
Cyc_Absyn_ArrayElement; _temp2454.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2454;});
_temp2453;})); _temp2452;}); _temp2451;}); break; case 199: _LL2450: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2456=( struct Cyc_Designator_tok_struct*)
_cycalloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2456[ 0]=({ struct
Cyc_Designator_tok_struct _temp2457; _temp2457.tag= Cyc_Designator_tok;
_temp2457.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2458=(
struct Cyc_Absyn_FieldName_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2458[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2459; _temp2459.tag= Cyc_Absyn_FieldName;
_temp2459.f1=({ struct _tagged_arr* _temp2460=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2460[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2460;}); _temp2459;});
_temp2458;})); _temp2457;}); _temp2456;}); break; case 200: _LL2455: { void*
_temp2462= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2463=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2463 !=  0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2464=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2465=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2465[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2466; _temp2466.tag= Cyc_ParamDecl_tok; _temp2466.f1=({
struct _tuple2* _temp2467=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2467->f1= 0; _temp2467->f2= _temp2464; _temp2467->f3= _temp2462; _temp2467;});
_temp2466;}); _temp2465;}); break;}} case 201: _LL2461: { void* _temp2469= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)); struct
Cyc_List_List* _temp2470=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f3; struct Cyc_Absyn_Tqual
_temp2471=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f1; struct Cyc_List_List* _temp2472=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2473= Cyc_Parse_apply_tms( _temp2471, _temp2469, _temp2470,
_temp2472); if( _temp2473.f3 !=  0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2473.f4 !=  0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2474=( struct Cyc_ParamDecl_tok_struct*)
_cycalloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2474[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2475; _temp2475.tag= Cyc_ParamDecl_tok; _temp2475.f1=({
struct _tuple2* _temp2476=( struct _tuple2*) _cycalloc( sizeof( struct _tuple2));
_temp2476->f1= 0; _temp2476->f2= _temp2473.f1; _temp2476->f3= _temp2473.f2;
_temp2476;}); _temp2475;}); _temp2474;}); break;} case 202: _LL2468: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2478=( struct Cyc_Type_tok_struct*)
_cycalloc( sizeof( struct Cyc_Type_tok_struct)); _temp2478[ 0]=({ struct Cyc_Type_tok_struct
_temp2479; _temp2479.tag= Cyc_Type_tok; _temp2479.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2479;});
_temp2478;}); break; case 203: _LL2477: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2481=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2481[ 0]=({ struct Cyc_Type_tok_struct _temp2482; _temp2482.tag= Cyc_Type_tok;
_temp2482.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2483=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2483[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2484; _temp2484.tag= Cyc_Absyn_JoinEff;
_temp2484.f1= 0; _temp2484;}); _temp2483;})); _temp2482;}); _temp2481;}); break;
case 204: _LL2480: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2486=(
struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2486[ 0]=({ struct Cyc_Type_tok_struct _temp2487; _temp2487.tag= Cyc_Type_tok;
_temp2487.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2488=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2488[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2489; _temp2489.tag= Cyc_Absyn_JoinEff;
_temp2489.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2489;}); _temp2488;})); _temp2487;});
_temp2486;}); break; case 205: _LL2485: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2491=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2491[ 0]=({ struct Cyc_Type_tok_struct _temp2492; _temp2492.tag= Cyc_Type_tok;
_temp2492.f1=( void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp2493=(
struct Cyc_Absyn_RgnsEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2493[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2494; _temp2494.tag= Cyc_Absyn_RgnsEff;
_temp2494.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2494;}); _temp2493;})); _temp2492;});
_temp2491;}); break; case 206: _LL2490: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2496=( struct Cyc_Type_tok_struct*) _cycalloc( sizeof( struct Cyc_Type_tok_struct));
_temp2496[ 0]=({ struct Cyc_Type_tok_struct _temp2497; _temp2497.tag= Cyc_Type_tok;
_temp2497.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2498=(
struct Cyc_Absyn_JoinEff_struct*) _cycalloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2498[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2499; _temp2499.tag= Cyc_Absyn_JoinEff;
_temp2499.f1=({ struct Cyc_List_List* _temp2500=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2500->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2500->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2500;}); _temp2499;}); _temp2498;})); _temp2497;});
_temp2496;}); break; case 207: _LL2495: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2502=( struct Cyc_TypeList_tok_struct*) _cycalloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2502[ 0]=({ struct Cyc_TypeList_tok_struct _temp2503; _temp2503.tag= Cyc_TypeList_tok;
_temp2503.f1=({ struct Cyc_List_List* _temp2504=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2504->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2504->tl= 0;
_temp2504;}); _temp2503;}); _temp2502;}); break; case 208: _LL2501: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2506=( struct Cyc_TypeList_tok_struct*)
_cycalloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2506[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2507; _temp2507.tag= Cyc_TypeList_tok; _temp2507.f1=({ struct Cyc_List_List*
_temp2508=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2508->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2508->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2508;});
_temp2507;}); _temp2506;}); break; case 209: _LL2505: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2510=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2510[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2511; _temp2511.tag= Cyc_AbstractDeclarator_tok;
_temp2511.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2512=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2512->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2512;});
_temp2511;}); _temp2510;}); break; case 210: _LL2509: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 211: _LL2513:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2515=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2515[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2516; _temp2516.tag=
Cyc_AbstractDeclarator_tok; _temp2516.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2517=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2517->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2517;});
_temp2516;}); _temp2515;}); break; case 212: _LL2514: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 213:
_LL2518: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2520=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2520[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2521; _temp2521.tag=
Cyc_AbstractDeclarator_tok; _temp2521.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2522=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2522->tms=({ struct Cyc_List_List* _temp2523=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2523->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2523->tl= 0; _temp2523;}); _temp2522;}); _temp2521;}); _temp2520;}); break;
case 214: _LL2519: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2525=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2525[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2526; _temp2526.tag= Cyc_AbstractDeclarator_tok; _temp2526.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2527=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2527->tms=({ struct Cyc_List_List* _temp2528=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2528->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2528->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]))->tms; _temp2528;});
_temp2527;}); _temp2526;}); _temp2525;}); break; case 215: _LL2524: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2530=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2530[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2531; _temp2531.tag= Cyc_AbstractDeclarator_tok;
_temp2531.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2532=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2532->tms=({
struct Cyc_List_List* _temp2533=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2533->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2534=( struct Cyc_Absyn_ConstArray_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2534[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2535; _temp2535.tag= Cyc_Absyn_ConstArray_mod; _temp2535.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2535;});
_temp2534;})); _temp2533->tl= 0; _temp2533;}); _temp2532;}); _temp2531;});
_temp2530;}); break; case 216: _LL2529: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2537=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2537[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2538; _temp2538.tag= Cyc_AbstractDeclarator_tok; _temp2538.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2539=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2539->tms=({ struct Cyc_List_List* _temp2540=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2540->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2541=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2541[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2542; _temp2542.tag= Cyc_Absyn_ConstArray_mod;
_temp2542.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2542;}); _temp2541;})); _temp2540->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2540;}); _temp2539;}); _temp2538;}); _temp2537;}); break; case 217: _LL2536:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2544=( struct
Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2544[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2545; _temp2545.tag=
Cyc_AbstractDeclarator_tok; _temp2545.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2546=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2546->tms=({ struct Cyc_List_List* _temp2547=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2547->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2548=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2548[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2549; _temp2549.tag= Cyc_Absyn_Function_mod;
_temp2549.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2550=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2550[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2551; _temp2551.tag= Cyc_Absyn_WithTypes;
_temp2551.f1= 0; _temp2551.f2= 0; _temp2551.f3= 0; _temp2551.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2551.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2551;}); _temp2550;})); _temp2549;}); _temp2548;}));
_temp2547->tl= 0; _temp2547;}); _temp2546;}); _temp2545;}); _temp2544;}); break;
case 218: _LL2543: { struct _tuple16 _temp2555; struct Cyc_List_List* _temp2556;
struct Cyc_Core_Opt* _temp2558; struct Cyc_Absyn_VarargInfo* _temp2560; int
_temp2562; struct Cyc_List_List* _temp2564; struct _tuple16* _temp2553= Cyc_yyget_YY1(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2555=*
_temp2553; _LL2565: _temp2564= _temp2555.f1; goto _LL2563; _LL2563: _temp2562=
_temp2555.f2; goto _LL2561; _LL2561: _temp2560= _temp2555.f3; goto _LL2559;
_LL2559: _temp2558= _temp2555.f4; goto _LL2557; _LL2557: _temp2556= _temp2555.f5;
goto _LL2554; _LL2554: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2566=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2566[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2567; _temp2567.tag= Cyc_AbstractDeclarator_tok; _temp2567.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2568=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2568->tms=({ struct Cyc_List_List* _temp2569=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2569->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2570=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2570[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2571; _temp2571.tag= Cyc_Absyn_Function_mod;
_temp2571.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2572=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2572[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2573; _temp2573.tag= Cyc_Absyn_WithTypes;
_temp2573.f1= _temp2564; _temp2573.f2= _temp2562; _temp2573.f3= _temp2560;
_temp2573.f4= _temp2558; _temp2573.f5= _temp2556; _temp2573;}); _temp2572;}));
_temp2571;}); _temp2570;})); _temp2569->tl= 0; _temp2569;}); _temp2568;});
_temp2567;}); _temp2566;}); break;} case 219: _LL2552: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2575=( struct Cyc_AbstractDeclarator_tok_struct*)
_cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2575[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2576; _temp2576.tag= Cyc_AbstractDeclarator_tok;
_temp2576.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2577=( struct Cyc_Parse_Abstractdeclarator*)
_cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2577->tms=({
struct Cyc_List_List* _temp2578=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2578->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2579=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2579[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2580; _temp2580.tag=
Cyc_Absyn_Function_mod; _temp2580.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2581=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2581[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2582; _temp2582.tag= Cyc_Absyn_WithTypes;
_temp2582.f1= 0; _temp2582.f2= 0; _temp2582.f3= 0; _temp2582.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2582.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2582;}); _temp2581;})); _temp2580;}); _temp2579;}));
_temp2578->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]))->tms; _temp2578;});
_temp2577;}); _temp2576;}); _temp2575;}); break; case 220: _LL2574: { struct
_tuple16 _temp2586; struct Cyc_List_List* _temp2587; struct Cyc_Core_Opt*
_temp2589; struct Cyc_Absyn_VarargInfo* _temp2591; int _temp2593; struct Cyc_List_List*
_temp2595; struct _tuple16* _temp2584= Cyc_yyget_YY1( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2586=*
_temp2584; _LL2596: _temp2595= _temp2586.f1; goto _LL2594; _LL2594: _temp2593=
_temp2586.f2; goto _LL2592; _LL2592: _temp2591= _temp2586.f3; goto _LL2590;
_LL2590: _temp2589= _temp2586.f4; goto _LL2588; _LL2588: _temp2587= _temp2586.f5;
goto _LL2585; _LL2585: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2597=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2597[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2598; _temp2598.tag= Cyc_AbstractDeclarator_tok; _temp2598.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2599=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2599->tms=({ struct Cyc_List_List* _temp2600=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2600->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2601=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2601[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2602; _temp2602.tag= Cyc_Absyn_Function_mod;
_temp2602.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2603=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2603[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2604; _temp2604.tag= Cyc_Absyn_WithTypes;
_temp2604.f1= _temp2595; _temp2604.f2= _temp2593; _temp2604.f3= _temp2591;
_temp2604.f4= _temp2589; _temp2604.f5= _temp2587; _temp2604;}); _temp2603;}));
_temp2602;}); _temp2601;})); _temp2600->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2600;}); _temp2599;}); _temp2598;}); _temp2597;}); break;} case 221:
_LL2583: { struct Cyc_List_List* _temp2606=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2607=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2607[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2608; _temp2608.tag= Cyc_AbstractDeclarator_tok; _temp2608.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2609=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2609->tms=({ struct Cyc_List_List* _temp2610=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2610->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2611=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2611[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2612; _temp2612.tag= Cyc_Absyn_TypeParams_mod;
_temp2612.f1= _temp2606; _temp2612.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2612.f3=
0; _temp2612;}); _temp2611;})); _temp2610->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2610;}); _temp2609;}); _temp2608;}); _temp2607;}); break;} case 222:
_LL2605:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2614=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2615=( struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2615[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2616; _temp2616.tag= Cyc_AbstractDeclarator_tok; _temp2616.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2617=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2617->tms=({ struct Cyc_List_List* _temp2618=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2618->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2619=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2619[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2620; _temp2620.tag= Cyc_Absyn_TypeParams_mod;
_temp2620.f1= _temp2614; _temp2620.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2620.f3=
0; _temp2620;}); _temp2619;})); _temp2618->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]))->tms;
_temp2618;}); _temp2617;}); _temp2616;}); _temp2615;}); break;} case 223:
_LL2613: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2622=(
struct Cyc_AbstractDeclarator_tok_struct*) _cycalloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2622[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2623; _temp2623.tag=
Cyc_AbstractDeclarator_tok; _temp2623.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2624=( struct Cyc_Parse_Abstractdeclarator*) _cycalloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2624->tms=({ struct Cyc_List_List* _temp2625=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2625->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2626=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2626[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2627; _temp2627.tag= Cyc_Absyn_Attributes_mod;
_temp2627.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2627.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2627;}); _temp2626;}));
_temp2625->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]))->tms; _temp2625;});
_temp2624;}); _temp2623;}); _temp2622;}); break; case 224: _LL2621: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 225: _LL2628:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
226: _LL2629: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 227: _LL2630: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 228: _LL2631: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 229: _LL2632:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
230: _LL2633: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2645=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2645->name=({ struct
_tagged_arr* _temp2648=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2648[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2648;});
_temp2645->identity= 0; _temp2645->kind=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp2646=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp2646[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp2647; _temp2647.tag= Cyc_Absyn_Eq_kb;
_temp2647.f1=( void*)(( void*) Cyc_Absyn_RgnKind); _temp2647;}); _temp2646;}));
_temp2645;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2643=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2643[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2644; _temp2644.tag= Cyc_Absyn_VarType;
_temp2644.f1= tv; _temp2644;}); _temp2643;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2635=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2635[ 0]=({ struct Cyc_Stmt_tok_struct _temp2636; _temp2636.tag= Cyc_Stmt_tok;
_temp2636.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2637=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2637[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2638; _temp2638.tag= Cyc_Absyn_Region_s;
_temp2638.f1= tv; _temp2638.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2639=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2639->f1=(
void*) Cyc_Absyn_Loc_n; _temp2639->f2=({ struct _tagged_arr* _temp2640=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2640[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2640;});
_temp2639;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2641=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2641[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2642; _temp2642.tag=
Cyc_Absyn_RgnHandleType; _temp2642.f1=( void*) t; _temp2642;}); _temp2641;}), 0);
_temp2638.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2638;}); _temp2637;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2636;});
_temp2635;}); break;} case 231: _LL2634: if( Cyc_Std_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) ==  0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2660=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2660->name=({ struct
_tagged_arr* _temp2663=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2663[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp2665; _temp2665.tag= Cyc_Std_String_pa; _temp2665.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
-  1)]);{ void* _temp2664[ 1u]={& _temp2665}; Cyc_Std_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2664, sizeof( void*), 1u));}});
_temp2663;}); _temp2660->identity= 0; _temp2660->kind=( void*)(( void*)({ struct
Cyc_Absyn_Eq_kb_struct* _temp2661=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp2661[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp2662; _temp2662.tag= Cyc_Absyn_Eq_kb; _temp2662.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp2662;}); _temp2661;})); _temp2660;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2658=( struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2658[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2659; _temp2659.tag= Cyc_Absyn_VarType;
_temp2659.f1= tv; _temp2659;}); _temp2658;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2650=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2650[ 0]=({ struct Cyc_Stmt_tok_struct _temp2651; _temp2651.tag= Cyc_Stmt_tok;
_temp2651.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2652=( struct Cyc_Absyn_Region_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2652[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2653; _temp2653.tag= Cyc_Absyn_Region_s;
_temp2653.f1= tv; _temp2653.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2654=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2654->f1=(
void*) Cyc_Absyn_Loc_n; _temp2654->f2=({ struct _tagged_arr* _temp2655=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2655[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2655;});
_temp2654;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2656=(
struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2656[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2657; _temp2657.tag=
Cyc_Absyn_RgnHandleType; _temp2657.f1=( void*) t; _temp2657;}); _temp2656;}), 0);
_temp2653.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2653;}); _temp2652;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2651;});
_temp2650;}); break;} case 232: _LL2649: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2667=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2667[ 0]=({ struct Cyc_Stmt_tok_struct _temp2668; _temp2668.tag= Cyc_Stmt_tok;
_temp2668.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2669=( struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2669[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2670; _temp2670.tag= Cyc_Absyn_Cut_s;
_temp2670.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2670;}); _temp2669;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2668;});
_temp2667;}); break; case 233: _LL2666: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2672=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2672[ 0]=({ struct Cyc_Stmt_tok_struct _temp2673; _temp2673.tag= Cyc_Stmt_tok;
_temp2673.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2674=( struct Cyc_Absyn_Splice_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2674[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2675; _temp2675.tag= Cyc_Absyn_Splice_s;
_temp2675.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2675;}); _temp2674;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2673;});
_temp2672;}); break; case 234: _LL2671: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2677=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2677[ 0]=({ struct Cyc_Stmt_tok_struct _temp2678; _temp2678.tag= Cyc_Stmt_tok;
_temp2678.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2679=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2679[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2680; _temp2680.tag= Cyc_Absyn_Label_s;
_temp2680.f1=({ struct _tagged_arr* _temp2681=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp2681[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp2681;});
_temp2680.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2680;}); _temp2679;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2678;});
_temp2677;}); break; case 235: _LL2676: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2683=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2683[ 0]=({ struct Cyc_Stmt_tok_struct _temp2684; _temp2684.tag= Cyc_Stmt_tok;
_temp2684.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2684;});
_temp2683;}); break; case 236: _LL2682: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2686=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2686[ 0]=({ struct Cyc_Stmt_tok_struct _temp2687; _temp2687.tag= Cyc_Stmt_tok;
_temp2687.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2687;}); _temp2686;}); break; case 237: _LL2685: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2689=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2689[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2690; _temp2690.tag= Cyc_Stmt_tok; _temp2690.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2690;}); _temp2689;}); break; case 238: _LL2688: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 239:
_LL2691: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2693=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2693[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2694; _temp2694.tag= Cyc_Stmt_tok; _temp2694.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2694;}); _temp2693;}); break;
case 240: _LL2692: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2696=(
struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2696[ 0]=({ struct Cyc_Stmt_tok_struct _temp2697; _temp2697.tag= Cyc_Stmt_tok;
_temp2697.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2697;});
_temp2696;}); break; case 241: _LL2695: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 242: _LL2698:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2700=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2700[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2701; _temp2701.tag= Cyc_Stmt_tok; _temp2701.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2701;}); _temp2700;}); break; case 243: _LL2699: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2703=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2703[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2704; _temp2704.tag= Cyc_Stmt_tok; _temp2704.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2705=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2705[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2706; _temp2706.tag= Cyc_Absyn_Fn_d;
_temp2706.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2706;}); _temp2705;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2704;}); _temp2703;}); break; case 244: _LL2702: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2708=( struct Cyc_Stmt_tok_struct*) _cycalloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2708[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2709; _temp2709.tag= Cyc_Stmt_tok; _temp2709.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2710=( struct
Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2710[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2711; _temp2711.tag= Cyc_Absyn_Fn_d;
_temp2711.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp2711;}); _temp2710;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2709;}); _temp2708;}); break; case 245: _LL2707: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2713=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2713[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2714; _temp2714.tag= Cyc_Stmt_tok; _temp2714.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2714;});
_temp2713;}); break; case 246: _LL2712: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2716=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2716[ 0]=({ struct Cyc_Stmt_tok_struct _temp2717; _temp2717.tag= Cyc_Stmt_tok;
_temp2717.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2717;}); _temp2716;}); break; case 247: _LL2715: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2719=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2719[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2720; _temp2720.tag= Cyc_Stmt_tok; _temp2720.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2720;}); _temp2719;}); break; case 248: _LL2718: if( Cyc_Std_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) !=  0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2722=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2722[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2723; _temp2723.tag= Cyc_Stmt_tok; _temp2723.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2724=( struct Cyc_Absyn_SwitchC_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2724[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2725; _temp2725.tag= Cyc_Absyn_SwitchC_s;
_temp2725.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]); _temp2725.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2725;});
_temp2724;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2723;}); _temp2722;}); break; case 249:
_LL2721: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2727=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2727[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2728; _temp2728.tag= Cyc_Stmt_tok; _temp2728.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2728;});
_temp2727;}); break; case 250: _LL2726: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2730=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2730[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2731; _temp2731.tag= Cyc_SwitchClauseList_tok; _temp2731.f1= 0; _temp2731;});
_temp2730;}); break; case 251: _LL2729: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2733=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2733[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2734; _temp2734.tag= Cyc_SwitchClauseList_tok; _temp2734.f1=({ struct Cyc_List_List*
_temp2735=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2735->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2736=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2736->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp2736->pat_vars= 0; _temp2736->where_clause= 0; _temp2736->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2736->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2736;}); _temp2735->tl= 0; _temp2735;});
_temp2734;}); _temp2733;}); break; case 252: _LL2732: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2738=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2738[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2739; _temp2739.tag= Cyc_SwitchClauseList_tok;
_temp2739.f1=({ struct Cyc_List_List* _temp2740=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2740->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2741=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2741->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2741->pat_vars= 0; _temp2741->where_clause= 0;
_temp2741->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2741->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2741;});
_temp2740->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2740;}); _temp2739;});
_temp2738;}); break; case 253: _LL2737: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2743=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2743[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2744; _temp2744.tag= Cyc_SwitchClauseList_tok; _temp2744.f1=({ struct Cyc_List_List*
_temp2745=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2745->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2746=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2746->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2746->pat_vars=
0; _temp2746->where_clause= 0; _temp2746->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2746->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line); _temp2746;}); _temp2745->tl= Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2745;});
_temp2744;}); _temp2743;}); break; case 254: _LL2742: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2748=( struct Cyc_SwitchClauseList_tok_struct*)
_cycalloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2748[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2749; _temp2749.tag= Cyc_SwitchClauseList_tok;
_temp2749.f1=({ struct Cyc_List_List* _temp2750=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2750->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2751=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2751->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp2751->pat_vars= 0; _temp2751->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2751->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2751->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2751;});
_temp2750->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2750;}); _temp2749;});
_temp2748;}); break; case 255: _LL2747: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2753=( struct Cyc_SwitchClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2753[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2754; _temp2754.tag= Cyc_SwitchClauseList_tok; _temp2754.f1=({ struct Cyc_List_List*
_temp2755=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2755->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2756=( struct Cyc_Absyn_Switch_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2756->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); _temp2756->pat_vars=
0; _temp2756->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]); _temp2756->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
1)]); _temp2756->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2756;});
_temp2755->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2755;}); _temp2754;});
_temp2753;}); break; case 256: _LL2752: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2758=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2758[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2759; _temp2759.tag= Cyc_SwitchCClauseList_tok; _temp2759.f1= 0; _temp2759;});
_temp2758;}); break; case 257: _LL2757: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2761=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2761[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2762; _temp2762.tag= Cyc_SwitchCClauseList_tok; _temp2762.f1=({ struct Cyc_List_List*
_temp2763=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2763->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2764=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2764->cnst_exp= 0;
_temp2764->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2764->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2764;});
_temp2763->tl= 0; _temp2763;}); _temp2762;}); _temp2761;}); break; case 258:
_LL2760: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2766=(
struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2766[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2767; _temp2767.tag=
Cyc_SwitchCClauseList_tok; _temp2767.f1=({ struct Cyc_List_List* _temp2768=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp2768->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2769=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2769->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]); _temp2769->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2769->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2769;});
_temp2768->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2768;}); _temp2767;});
_temp2766;}); break; case 259: _LL2765: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2771=( struct Cyc_SwitchCClauseList_tok_struct*) _cycalloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2771[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2772; _temp2772.tag= Cyc_SwitchCClauseList_tok; _temp2772.f1=({ struct Cyc_List_List*
_temp2773=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2773->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2774=( struct Cyc_Absyn_SwitchC_clause*)
_cycalloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2774->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  3)]); _temp2774->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2774->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2774;});
_temp2773->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2773;}); _temp2772;});
_temp2771;}); break; case 260: _LL2770: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2776=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2776[ 0]=({ struct Cyc_Stmt_tok_struct _temp2777; _temp2777.tag= Cyc_Stmt_tok;
_temp2777.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2777;}); _temp2776;}); break; case 261: _LL2775: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2779=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2779[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2780; _temp2780.tag= Cyc_Stmt_tok; _temp2780.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2780;}); _temp2779;}); break; case 262: _LL2778: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2782=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2782[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2783; _temp2783.tag= Cyc_Stmt_tok; _temp2783.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2783;}); _temp2782;}); break; case 263: _LL2781: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2785=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2785[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2786; _temp2786.tag= Cyc_Stmt_tok; _temp2786.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2786;}); _temp2785;}); break; case 264: _LL2784: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2788=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2788[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2789; _temp2789.tag= Cyc_Stmt_tok; _temp2789.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2789;}); _temp2788;}); break; case 265: _LL2787: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2791=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2791[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2792; _temp2792.tag= Cyc_Stmt_tok; _temp2792.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2792;}); _temp2791;}); break; case 266: _LL2790: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2794=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2794[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2795; _temp2795.tag= Cyc_Stmt_tok; _temp2795.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2795;}); _temp2794;}); break; case 267: _LL2793: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2797=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2797[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2798; _temp2798.tag= Cyc_Stmt_tok; _temp2798.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2798;});
_temp2797;}); break; case 268: _LL2796: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2800=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2800[ 0]=({ struct Cyc_Stmt_tok_struct _temp2801; _temp2801.tag= Cyc_Stmt_tok;
_temp2801.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2801;});
_temp2800;}); break; case 269: _LL2799: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2803=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2803[ 0]=({ struct Cyc_Stmt_tok_struct _temp2804; _temp2804.tag= Cyc_Stmt_tok;
_temp2804.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2804;}); _temp2803;}); break; case 270: _LL2802: { struct Cyc_List_List*
_temp2806= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); struct Cyc_Absyn_Stmt* _temp2807= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2808=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2808[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2809; _temp2809.tag= Cyc_Stmt_tok; _temp2809.f1= Cyc_Parse_flatten_declarations(
_temp2806, _temp2807); _temp2809;}); _temp2808;}); break;} case 271: _LL2805: {
struct Cyc_List_List* _temp2811= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2812= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2813=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2813[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2814; _temp2814.tag= Cyc_Stmt_tok; _temp2814.f1= Cyc_Parse_flatten_declarations(
_temp2811, _temp2812); _temp2814;}); _temp2813;}); break;} case 272: _LL2810: {
struct Cyc_List_List* _temp2816= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); struct Cyc_Absyn_Stmt*
_temp2817= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2818=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2818[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2819; _temp2819.tag= Cyc_Stmt_tok; _temp2819.f1= Cyc_Parse_flatten_declarations(
_temp2816, _temp2817); _temp2819;}); _temp2818;}); break;} case 273: _LL2815: {
struct Cyc_List_List* _temp2821= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  5)]); struct Cyc_Absyn_Stmt*
_temp2822= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2823=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2823[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2824; _temp2824.tag= Cyc_Stmt_tok; _temp2824.f1= Cyc_Parse_flatten_declarations(
_temp2821, _temp2822); _temp2824;}); _temp2823;}); break;} case 274: _LL2820:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2826=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2826[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2827; _temp2827.tag= Cyc_Stmt_tok; _temp2827.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2828=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2828[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2828;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2827;}); _temp2826;}); break; case 275: _LL2825: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2830=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2830[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2831; _temp2831.tag= Cyc_Stmt_tok; _temp2831.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2831;}); _temp2830;}); break; case 276:
_LL2829: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2833=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2833[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2834; _temp2834.tag= Cyc_Stmt_tok; _temp2834.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2834;}); _temp2833;}); break; case 277: _LL2832: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2836=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2836[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2837; _temp2837.tag= Cyc_Stmt_tok; _temp2837.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2837;}); _temp2836;}); break; case 278:
_LL2835: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2839=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2839[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2840; _temp2840.tag= Cyc_Stmt_tok; _temp2840.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).last_line));
_temp2840;}); _temp2839;}); break; case 279: _LL2838: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2842=( struct Cyc_Stmt_tok_struct*) _cycalloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2842[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2843; _temp2843.tag= Cyc_Stmt_tok; _temp2843.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  1)]).last_line)); _temp2843;}); _temp2842;}); break; case 280:
_LL2841: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2845=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2845[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2846; _temp2846.tag= Cyc_Stmt_tok; _temp2846.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset -  3)]).last_line)); _temp2846;}); _temp2845;}); break; case 281:
_LL2844: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2848=( struct Cyc_Stmt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2848[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2849; _temp2849.tag= Cyc_Stmt_tok; _temp2849.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).last_line));
_temp2849;}); _temp2848;}); break; case 282: _LL2847: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2851=( struct Cyc_Pattern_tok_struct*) _cycalloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2851[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2852; _temp2852.tag= Cyc_Pattern_tok; _temp2852.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2852;});
_temp2851;}); break; case 283: _LL2850: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 284:
_LL2853: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2855=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2855[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2856; _temp2856.tag= Cyc_Pattern_tok; _temp2856.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2857=( struct Cyc_Absyn_Int_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2857[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2858; _temp2858.tag= Cyc_Absyn_Int_p; _temp2858.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2858.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2858;}); _temp2857;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2856;});
_temp2855;}); break; case 285: _LL2854: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2860=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2860[ 0]=({ struct Cyc_Pattern_tok_struct _temp2861; _temp2861.tag= Cyc_Pattern_tok;
_temp2861.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2862=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2862[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2863; _temp2863.tag= Cyc_Absyn_Int_p;
_temp2863.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2863.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2863;});
_temp2862;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2861;}); _temp2860;}); break; case 286:
_LL2859: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2865=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2865[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2866; _temp2866.tag= Cyc_Pattern_tok; _temp2866.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2867=( struct Cyc_Absyn_Float_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2867[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2868; _temp2868.tag= Cyc_Absyn_Float_p; _temp2868.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2868;}); _temp2867;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2866;});
_temp2865;}); break; case 287: _LL2864: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2870=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2870[ 0]=({ struct Cyc_Pattern_tok_struct _temp2871; _temp2871.tag= Cyc_Pattern_tok;
_temp2871.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2872=( struct Cyc_Absyn_Char_p_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2872[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2873; _temp2873.tag= Cyc_Absyn_Char_p; _temp2873.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2873;});
_temp2872;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2871;}); _temp2870;}); break; case 288:
_LL2869: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2875=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2875[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2876; _temp2876.tag= Cyc_Pattern_tok; _temp2876.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2876;});
_temp2875;}); break; case 289: _LL2874: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2878=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2878[ 0]=({ struct Cyc_Pattern_tok_struct _temp2879; _temp2879.tag= Cyc_Pattern_tok;
_temp2879.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2880=( struct Cyc_Absyn_UnknownId_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2880[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2881; _temp2881.tag=
Cyc_Absyn_UnknownId_p; _temp2881.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2881;}); _temp2880;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2879;}); _temp2878;}); break; case 290:
_LL2877: { struct Cyc_List_List* _temp2883=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2884=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2884[ 0]=({ struct Cyc_Pattern_tok_struct _temp2885; _temp2885.tag= Cyc_Pattern_tok;
_temp2885.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2886=( struct Cyc_Absyn_UnknownCall_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2886[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2887; _temp2887.tag= Cyc_Absyn_UnknownCall_p; _temp2887.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2887.f2=
_temp2883; _temp2887.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2887;});
_temp2886;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2885;}); _temp2884;}); break;} case
291: _LL2882: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2889=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2889[ 0]=({ struct Cyc_Pattern_tok_struct _temp2890; _temp2890.tag= Cyc_Pattern_tok;
_temp2890.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2891=( struct Cyc_Absyn_Tuple_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2891[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2892; _temp2892.tag= Cyc_Absyn_Tuple_p;
_temp2892.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)]); _temp2892;}); _temp2891;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2890;}); _temp2889;}); break; case 292: _LL2888: { struct Cyc_List_List*
_temp2894=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); yyval=(
void*)({ struct Cyc_Pattern_tok_struct* _temp2895=( struct Cyc_Pattern_tok_struct*)
_cycalloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2895[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2896; _temp2896.tag= Cyc_Pattern_tok; _temp2896.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2897=( struct Cyc_Absyn_UnknownFields_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2897[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2898; _temp2898.tag= Cyc_Absyn_UnknownFields_p;
_temp2898.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp2898.f2= _temp2894; _temp2898.f3= 0; _temp2898;});
_temp2897;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2896;}); _temp2895;}); break;} case
293: _LL2893: { struct Cyc_List_List* _temp2900=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2901=(
struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2901[ 0]=({ struct Cyc_Pattern_tok_struct _temp2902; _temp2902.tag= Cyc_Pattern_tok;
_temp2902.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2903=( struct Cyc_Absyn_UnknownFields_p_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2903[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2904; _temp2904.tag= Cyc_Absyn_UnknownFields_p; _temp2904.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp2904.f2=
_temp2900; _temp2904.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2904;});
_temp2903;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2902;}); _temp2901;}); break;} case
294: _LL2899: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2906=( struct
Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2906[ 0]=({ struct Cyc_Pattern_tok_struct _temp2907; _temp2907.tag= Cyc_Pattern_tok;
_temp2907.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2908=( struct Cyc_Absyn_Pointer_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2908[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2909; _temp2909.tag= Cyc_Absyn_Pointer_p;
_temp2909.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2909;}); _temp2908;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2907;});
_temp2906;}); break; case 295: _LL2905: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2911=( struct Cyc_Pattern_tok_struct*) _cycalloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2911[ 0]=({ struct Cyc_Pattern_tok_struct _temp2912; _temp2912.tag= Cyc_Pattern_tok;
_temp2912.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2913=( struct Cyc_Absyn_Reference_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2913[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2914; _temp2914.tag=
Cyc_Absyn_Reference_p; _temp2914.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2915=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp2915->f1=(
void*) Cyc_Absyn_Loc_n; _temp2915->f2=({ struct _tagged_arr* _temp2916=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp2916[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2916;});
_temp2915;}),( void*) Cyc_Absyn_VoidType, 0); _temp2914;}); _temp2913;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2912;}); _temp2911;}); break; case 296: _LL2910: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2918=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2918[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2919; _temp2919.tag= Cyc_PatternList_tok;
_temp2919.f1= 0; _temp2919;}); _temp2918;}); break; case 297: _LL2917: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2921=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2921[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2922; _temp2922.tag= Cyc_PatternList_tok;
_temp2922.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2922;}); _temp2921;}); break; case 298: _LL2920: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2924=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2924[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2925; _temp2925.tag= Cyc_PatternList_tok;
_temp2925.f1=({ struct Cyc_List_List* _temp2926=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2926->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2926->tl= 0;
_temp2926;}); _temp2925;}); _temp2924;}); break; case 299: _LL2923: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2928=( struct Cyc_PatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2928[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2929; _temp2929.tag= Cyc_PatternList_tok;
_temp2929.f1=({ struct Cyc_List_List* _temp2930=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2930->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2930->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2930;}); _temp2929;}); _temp2928;}); break; case 300:
_LL2927: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2932=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2932[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2933; _temp2933.tag=
Cyc_FieldPattern_tok; _temp2933.f1=({ struct _tuple13* _temp2934=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2934->f1= 0; _temp2934->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2934;}); _temp2933;}); _temp2932;}); break; case 301:
_LL2931: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2936=( struct
Cyc_FieldPattern_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2936[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2937; _temp2937.tag=
Cyc_FieldPattern_tok; _temp2937.f1=({ struct _tuple13* _temp2938=( struct
_tuple13*) _cycalloc( sizeof( struct _tuple13)); _temp2938->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp2938->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2938;}); _temp2937;}); _temp2936;}); break; case 302:
_LL2935: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2940=(
struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2940[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2941; _temp2941.tag=
Cyc_FieldPatternList_tok; _temp2941.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2941;});
_temp2940;}); break; case 303: _LL2939: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2943=( struct Cyc_FieldPatternList_tok_struct*) _cycalloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2943[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2944; _temp2944.tag= Cyc_FieldPatternList_tok; _temp2944.f1=({ struct Cyc_List_List*
_temp2945=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2945->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2945->tl= 0;
_temp2945;}); _temp2944;}); _temp2943;}); break; case 304: _LL2942: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2947=( struct Cyc_FieldPatternList_tok_struct*)
_cycalloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2947[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2948; _temp2948.tag= Cyc_FieldPatternList_tok;
_temp2948.f1=({ struct Cyc_List_List* _temp2949=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2949->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2949->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]); _temp2949;}); _temp2948;}); _temp2947;}); break; case 305:
_LL2946: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 306: _LL2950: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2952=(
struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2952[ 0]=({ struct Cyc_Exp_tok_struct _temp2953; _temp2953.tag= Cyc_Exp_tok;
_temp2953.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2953;}); _temp2952;}); break; case 307: _LL2951: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 308: _LL2954:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2956=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2956[ 0]=({ struct Cyc_Exp_tok_struct
_temp2957; _temp2957.tag= Cyc_Exp_tok; _temp2957.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2957;}); _temp2956;}); break; case 309: _LL2955: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2959=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2959[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2960; _temp2960.tag= Cyc_Primopopt_tok; _temp2960.f1=
0; _temp2960;}); _temp2959;}); break; case 310: _LL2958: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2962=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2962[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2963; _temp2963.tag= Cyc_Primopopt_tok; _temp2963.f1=({
struct Cyc_Core_Opt* _temp2964=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2964->v=( void*)(( void*) Cyc_Absyn_Times); _temp2964;});
_temp2963;}); _temp2962;}); break; case 311: _LL2961: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2966=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2966[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2967; _temp2967.tag= Cyc_Primopopt_tok; _temp2967.f1=({
struct Cyc_Core_Opt* _temp2968=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2968->v=( void*)(( void*) Cyc_Absyn_Div); _temp2968;});
_temp2967;}); _temp2966;}); break; case 312: _LL2965: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2970=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2970[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2971; _temp2971.tag= Cyc_Primopopt_tok; _temp2971.f1=({
struct Cyc_Core_Opt* _temp2972=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2972->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2972;});
_temp2971;}); _temp2970;}); break; case 313: _LL2969: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2974=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2974[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2975; _temp2975.tag= Cyc_Primopopt_tok; _temp2975.f1=({
struct Cyc_Core_Opt* _temp2976=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2976->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2976;});
_temp2975;}); _temp2974;}); break; case 314: _LL2973: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2978=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2978[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2979; _temp2979.tag= Cyc_Primopopt_tok; _temp2979.f1=({
struct Cyc_Core_Opt* _temp2980=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2980->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2980;});
_temp2979;}); _temp2978;}); break; case 315: _LL2977: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2982=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2982[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2983; _temp2983.tag= Cyc_Primopopt_tok; _temp2983.f1=({
struct Cyc_Core_Opt* _temp2984=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2984->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2984;});
_temp2983;}); _temp2982;}); break; case 316: _LL2981: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2986=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2986[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2987; _temp2987.tag= Cyc_Primopopt_tok; _temp2987.f1=({
struct Cyc_Core_Opt* _temp2988=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2988->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2988;});
_temp2987;}); _temp2986;}); break; case 317: _LL2985: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2990=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2990[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2991; _temp2991.tag= Cyc_Primopopt_tok; _temp2991.f1=({
struct Cyc_Core_Opt* _temp2992=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2992->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2992;});
_temp2991;}); _temp2990;}); break; case 318: _LL2989: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2994=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2994[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2995; _temp2995.tag= Cyc_Primopopt_tok; _temp2995.f1=({
struct Cyc_Core_Opt* _temp2996=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp2996->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2996;});
_temp2995;}); _temp2994;}); break; case 319: _LL2993: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2998=( struct Cyc_Primopopt_tok_struct*)
_cycalloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2998[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2999; _temp2999.tag= Cyc_Primopopt_tok; _temp2999.f1=({
struct Cyc_Core_Opt* _temp3000=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp3000->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3000;});
_temp2999;}); _temp2998;}); break; case 320: _LL2997: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 321: _LL3001:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3003=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3003[ 0]=({ struct Cyc_Exp_tok_struct
_temp3004; _temp3004.tag= Cyc_Exp_tok; _temp3004.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3004;});
_temp3003;}); break; case 322: _LL3002: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3006=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3006[ 0]=({ struct Cyc_Exp_tok_struct _temp3007; _temp3007.tag= Cyc_Exp_tok;
_temp3007.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3007;}); _temp3006;}); break; case 323: _LL3005: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3009=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3009[ 0]=({ struct Cyc_Exp_tok_struct
_temp3010; _temp3010.tag= Cyc_Exp_tok; _temp3010.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3010;}); _temp3009;}); break; case 324: _LL3008: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3012=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3012[ 0]=({ struct Cyc_Exp_tok_struct
_temp3013; _temp3013.tag= Cyc_Exp_tok; _temp3013.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset +  1)]).last_line));
_temp3013;}); _temp3012;}); break; case 325: _LL3011: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3015=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3015[ 0]=({ struct Cyc_Exp_tok_struct
_temp3016; _temp3016.tag= Cyc_Exp_tok; _temp3016.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3016;});
_temp3015;}); break; case 326: _LL3014: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3018=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3018[ 0]=({ struct Cyc_Exp_tok_struct _temp3019; _temp3019.tag= Cyc_Exp_tok;
_temp3019.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3019;}); _temp3018;}); break; case 327: _LL3017: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 328: _LL3020:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
329: _LL3021: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3023=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3023[ 0]=({ struct Cyc_Exp_tok_struct
_temp3024; _temp3024.tag= Cyc_Exp_tok; _temp3024.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3024;}); _temp3023;}); break; case 330: _LL3022: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL3025:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3027=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3027[ 0]=({ struct Cyc_Exp_tok_struct
_temp3028; _temp3028.tag= Cyc_Exp_tok; _temp3028.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3028;}); _temp3027;}); break; case 332: _LL3026: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL3029:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3031=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3031[ 0]=({ struct Cyc_Exp_tok_struct
_temp3032; _temp3032.tag= Cyc_Exp_tok; _temp3032.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3032;});
_temp3031;}); break; case 334: _LL3030: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL3033:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3035=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3035[ 0]=({ struct Cyc_Exp_tok_struct
_temp3036; _temp3036.tag= Cyc_Exp_tok; _temp3036.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3036;});
_temp3035;}); break; case 336: _LL3034: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 337: _LL3037:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3039=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3039[ 0]=({ struct Cyc_Exp_tok_struct
_temp3040; _temp3040.tag= Cyc_Exp_tok; _temp3040.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3040;});
_temp3039;}); break; case 338: _LL3038: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 339: _LL3041:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3043=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3043[ 0]=({ struct Cyc_Exp_tok_struct
_temp3044; _temp3044.tag= Cyc_Exp_tok; _temp3044.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3044;}); _temp3043;}); break; case 340: _LL3042: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3046=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3046[ 0]=({ struct Cyc_Exp_tok_struct
_temp3047; _temp3047.tag= Cyc_Exp_tok; _temp3047.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3047;}); _temp3046;}); break; case 341: _LL3045: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 342: _LL3048:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3050=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3050[ 0]=({ struct Cyc_Exp_tok_struct
_temp3051; _temp3051.tag= Cyc_Exp_tok; _temp3051.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3051;}); _temp3050;}); break; case 343: _LL3049: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3053=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3053[ 0]=({ struct Cyc_Exp_tok_struct
_temp3054; _temp3054.tag= Cyc_Exp_tok; _temp3054.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3054;}); _temp3053;}); break; case 344: _LL3052: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3056=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3056[ 0]=({ struct Cyc_Exp_tok_struct
_temp3057; _temp3057.tag= Cyc_Exp_tok; _temp3057.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3057;}); _temp3056;}); break; case 345: _LL3055: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3059=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3059[ 0]=({ struct Cyc_Exp_tok_struct
_temp3060; _temp3060.tag= Cyc_Exp_tok; _temp3060.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3060;}); _temp3059;}); break; case 346: _LL3058: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 347: _LL3061:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3063=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3063[ 0]=({ struct Cyc_Exp_tok_struct
_temp3064; _temp3064.tag= Cyc_Exp_tok; _temp3064.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3064;}); _temp3063;}); break; case 348: _LL3062: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3066=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3066[ 0]=({ struct Cyc_Exp_tok_struct
_temp3067; _temp3067.tag= Cyc_Exp_tok; _temp3067.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3067;}); _temp3066;}); break; case 349: _LL3065: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 350: _LL3068:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3070=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3070[ 0]=({ struct Cyc_Exp_tok_struct
_temp3071; _temp3071.tag= Cyc_Exp_tok; _temp3071.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3071;});
_temp3070;}); break; case 351: _LL3069: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3073=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3073[ 0]=({ struct Cyc_Exp_tok_struct _temp3074; _temp3074.tag= Cyc_Exp_tok;
_temp3074.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3074;}); _temp3073;}); break; case 352: _LL3072: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 353: _LL3075:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3077=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3077[ 0]=({ struct Cyc_Exp_tok_struct
_temp3078; _temp3078.tag= Cyc_Exp_tok; _temp3078.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3078;});
_temp3077;}); break; case 354: _LL3076: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3080=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3080[ 0]=({ struct Cyc_Exp_tok_struct _temp3081; _temp3081.tag= Cyc_Exp_tok;
_temp3081.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3081;}); _temp3080;}); break; case 355: _LL3079: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3083=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3083[ 0]=({ struct Cyc_Exp_tok_struct
_temp3084; _temp3084.tag= Cyc_Exp_tok; _temp3084.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3084;});
_temp3083;}); break; case 356: _LL3082: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 357: _LL3085:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3087=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3087[ 0]=({ struct Cyc_Exp_tok_struct
_temp3088; _temp3088.tag= Cyc_Exp_tok; _temp3088.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3088;}); _temp3087;}); break; case 358: _LL3086: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 359: _LL3089:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3091=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3091[ 0]=({ struct Cyc_Exp_tok_struct
_temp3092; _temp3092.tag= Cyc_Exp_tok; _temp3092.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3092;}); _temp3091;}); break; case 360: _LL3090: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3094=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3094[ 0]=({ struct Cyc_Exp_tok_struct
_temp3095; _temp3095.tag= Cyc_Exp_tok; _temp3095.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3095;}); _temp3094;}); break; case 361: _LL3093: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3097=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3097[ 0]=({ struct Cyc_Exp_tok_struct
_temp3098; _temp3098.tag= Cyc_Exp_tok; _temp3098.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3098;}); _temp3097;}); break; case 362: _LL3096: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3100=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3100[ 0]=({ struct Cyc_Exp_tok_struct
_temp3101; _temp3101.tag= Cyc_Exp_tok; _temp3101.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3101;}); _temp3100;}); break; case 363: _LL3099: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 364: _LL3102:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3104=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3104[ 0]=({ struct Cyc_Exp_tok_struct
_temp3105; _temp3105.tag= Cyc_Exp_tok; _temp3105.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3105;}); _temp3104;}); break; case 365: _LL3103: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3107=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3107[ 0]=({ struct Cyc_Exp_tok_struct
_temp3108; _temp3108.tag= Cyc_Exp_tok; _temp3108.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3108;});
_temp3107;}); break; case 366: _LL3106: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3110=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3110[ 0]=({ struct Cyc_Exp_tok_struct _temp3111; _temp3111.tag= Cyc_Exp_tok;
_temp3111.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3111;}); _temp3110;}); break; case 367: _LL3109: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3113=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3113[ 0]=({ struct Cyc_Exp_tok_struct
_temp3114; _temp3114.tag= Cyc_Exp_tok; _temp3114.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)])).f3,( void*)({ struct Cyc_Absyn_StructField_struct*
_temp3115=( struct Cyc_Absyn_StructField_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructField_struct));
_temp3115[ 0]=({ struct Cyc_Absyn_StructField_struct _temp3116; _temp3116.tag=
Cyc_Absyn_StructField; _temp3116.f1=({ struct _tagged_arr* _temp3117=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp3117[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3117;});
_temp3116;}); _temp3115;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3114;});
_temp3113;}); break; case 368: _LL3112: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3119=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3119[ 0]=({ struct Cyc_Exp_tok_struct _temp3120; _temp3120.tag= Cyc_Exp_tok;
_temp3120.f1= Cyc_Absyn_offsetof_exp((* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])).f3,( void*)({
struct Cyc_Absyn_TupleIndex_struct* _temp3121=( struct Cyc_Absyn_TupleIndex_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Absyn_TupleIndex_struct)); _temp3121[ 0]=({
struct Cyc_Absyn_TupleIndex_struct _temp3122; _temp3122.tag= Cyc_Absyn_TupleIndex;
_temp3122.f1=( unsigned int)(* Cyc_yyget_Int_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])).f2; _temp3122;});
_temp3121;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3120;}); _temp3119;}); break; case 369:
_LL3118: { struct Cyc_Position_Segment* _temp3124= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); struct
Cyc_List_List* _temp3125=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, _temp3124, Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  3)])); yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3126=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3126[ 0]=({ struct Cyc_Exp_tok_struct
_temp3127; _temp3127.tag= Cyc_Exp_tok; _temp3127.f1= Cyc_Absyn_gentyp_exp(
_temp3125,(* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3127;});
_temp3126;}); break;} case 370: _LL3123: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3129=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3129[ 0]=({ struct Cyc_Exp_tok_struct _temp3130; _temp3130.tag= Cyc_Exp_tok;
_temp3130.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3131=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3131[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3132; _temp3132.tag= Cyc_Absyn_Malloc_e;
_temp3132.f1= 0; _temp3132.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3132;}); _temp3131;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3130;});
_temp3129;}); break; case 371: _LL3128: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3134=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3134[ 0]=({ struct Cyc_Exp_tok_struct _temp3135; _temp3135.tag= Cyc_Exp_tok;
_temp3135.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3136=( struct Cyc_Absyn_Malloc_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3136[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3137; _temp3137.tag= Cyc_Absyn_Malloc_e;
_temp3137.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  6)]); _temp3137.f2=(
void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).last_line));
_temp3137;}); _temp3136;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3135;});
_temp3134;}); break; case 372: _LL3133: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3139=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3139[ 0]=({ struct Cyc_Primop_tok_struct _temp3140; _temp3140.tag= Cyc_Primop_tok;
_temp3140.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3140;}); _temp3139;});
break; case 373: _LL3138: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3142=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3142[ 0]=({ struct Cyc_Primop_tok_struct _temp3143; _temp3143.tag= Cyc_Primop_tok;
_temp3143.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3143;}); _temp3142;});
break; case 374: _LL3141: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3145=( struct Cyc_Primop_tok_struct*) _cycalloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3145[ 0]=({ struct Cyc_Primop_tok_struct _temp3146; _temp3146.tag= Cyc_Primop_tok;
_temp3146.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3146;}); _temp3145;});
break; case 375: _LL3144: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 376: _LL3147: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3149=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3149[ 0]=({ struct Cyc_Exp_tok_struct _temp3150; _temp3150.tag= Cyc_Exp_tok;
_temp3150.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3150;}); _temp3149;}); break; case 377: _LL3148: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3152=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3152[ 0]=({ struct Cyc_Exp_tok_struct
_temp3153; _temp3153.tag= Cyc_Exp_tok; _temp3153.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3153;}); _temp3152;}); break; case 378:
_LL3151: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3155=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3155[ 0]=({ struct Cyc_Exp_tok_struct
_temp3156; _temp3156.tag= Cyc_Exp_tok; _temp3156.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3156;});
_temp3155;}); break; case 379: _LL3154: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3158=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3158[ 0]=({ struct Cyc_Exp_tok_struct _temp3159; _temp3159.tag= Cyc_Exp_tok;
_temp3159.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3160=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3160[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3160;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3159;}); _temp3158;}); break; case 380: _LL3157: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3162=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3162[ 0]=({ struct Cyc_Exp_tok_struct
_temp3163; _temp3163.tag= Cyc_Exp_tok; _temp3163.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3163;});
_temp3162;}); break;} case 381: _LL3161: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3165=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3165[ 0]=({ struct Cyc_Exp_tok_struct _temp3166; _temp3166.tag= Cyc_Exp_tok;
_temp3166.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]),({ struct
_tagged_arr* _temp3167=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3167[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3167;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3166;}); _temp3165;}); break; case 382: _LL3164: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3169=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3169[ 0]=({ struct Cyc_Exp_tok_struct
_temp3170; _temp3170.tag= Cyc_Exp_tok; _temp3170.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3170;});
_temp3169;}); break;} case 383: _LL3168: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3172=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3172[ 0]=({ struct Cyc_Exp_tok_struct _temp3173; _temp3173.tag= Cyc_Exp_tok;
_temp3173.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3173;}); _temp3172;}); break; case 384: _LL3171: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3175=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3175[ 0]=({ struct Cyc_Exp_tok_struct
_temp3176; _temp3176.tag= Cyc_Exp_tok; _temp3176.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3176;}); _temp3175;}); break; case 385: _LL3174: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3178=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3178[ 0]=({ struct Cyc_Exp_tok_struct
_temp3179; _temp3179.tag= Cyc_Exp_tok; _temp3179.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3180=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3180[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3181; _temp3181.tag= Cyc_Absyn_CompoundLit_e;
_temp3181.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  4)]); _temp3181.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3181;});
_temp3180;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3179;}); _temp3178;}); break; case 386:
_LL3177: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3183=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3183[ 0]=({ struct Cyc_Exp_tok_struct
_temp3184; _temp3184.tag= Cyc_Exp_tok; _temp3184.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3185=( struct Cyc_Absyn_CompoundLit_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3185[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3186; _temp3186.tag= Cyc_Absyn_CompoundLit_e;
_temp3186.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset -  5)]); _temp3186.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)])); _temp3186;});
_temp3185;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3184;}); _temp3183;}); break; case 387:
_LL3182: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3188=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3188[ 0]=({ struct Cyc_Exp_tok_struct
_temp3189; _temp3189.tag= Cyc_Exp_tok; _temp3189.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3190=( struct Cyc_Absyn_Fill_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3190[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3191; _temp3191.tag= Cyc_Absyn_Fill_e; _temp3191.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3191;});
_temp3190;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3189;}); _temp3188;}); break; case 388:
_LL3187: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3193=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3193[ 0]=({ struct Cyc_Exp_tok_struct
_temp3194; _temp3194.tag= Cyc_Exp_tok; _temp3194.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3195=( struct Cyc_Absyn_Codegen_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3195[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3196; _temp3196.tag= Cyc_Absyn_Codegen_e;
_temp3196.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  1)]); _temp3196;}); _temp3195;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3194;}); _temp3193;}); break; case 389: _LL3192: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3198=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3198[ 0]=({ struct Cyc_Exp_tok_struct
_temp3199; _temp3199.tag= Cyc_Exp_tok; _temp3199.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3200=( struct Cyc_Absyn_UnknownId_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3200[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3201; _temp3201.tag= Cyc_Absyn_UnknownId_e;
_temp3201.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3201;}); _temp3200;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3199;});
_temp3198;}); break; case 390: _LL3197: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 391: _LL3202:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3204=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3204[ 0]=({ struct Cyc_Exp_tok_struct
_temp3205; _temp3205.tag= Cyc_Exp_tok; _temp3205.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3205;});
_temp3204;}); break; case 392: _LL3203: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]; break; case 393:
_LL3206: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3208=( struct Cyc_Exp_tok_struct*)
_cycalloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3208[ 0]=({ struct Cyc_Exp_tok_struct
_temp3209; _temp3209.tag= Cyc_Exp_tok; _temp3209.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3210=(
struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3210[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3211; _temp3211.tag=
Cyc_Absyn_UnknownId_e; _temp3211.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]); _temp3211;});
_temp3210;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3209;});
_temp3208;}); break; case 394: _LL3207: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3213=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3213[ 0]=({ struct Cyc_Exp_tok_struct _temp3214; _temp3214.tag= Cyc_Exp_tok;
_temp3214.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3215=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3215[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3216; _temp3216.tag=
Cyc_Absyn_UnknownId_e; _temp3216.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  4)]); _temp3216;});
_temp3215;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset -  4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3214;}); _temp3213;}); break; case 395: _LL3212: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3218=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3218[ 0]=({ struct Cyc_Exp_tok_struct
_temp3219; _temp3219.tag= Cyc_Exp_tok; _temp3219.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -  1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3219;}); _temp3218;}); break; case 396: _LL3217: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3221=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3221[ 0]=({ struct Cyc_Exp_tok_struct
_temp3222; _temp3222.tag= Cyc_Exp_tok; _temp3222.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3223=( struct Cyc_Absyn_Struct_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3223[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3224; _temp3224.tag= Cyc_Absyn_Struct_e;
_temp3224.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset -  3)]); _temp3224.f2= 0; _temp3224.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  1)])); _temp3224.f4= 0;
_temp3224;}); _temp3223;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset -  3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3222;});
_temp3221;}); break; case 397: _LL3220: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3226=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3226[ 0]=({ struct Cyc_Exp_tok_struct _temp3227; _temp3227.tag= Cyc_Exp_tok;
_temp3227.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset -  4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3227;}); _temp3226;}); break; case 398: _LL3225: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3229=( struct Cyc_ExpList_tok_struct*) _cycalloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3229[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3230; _temp3230.tag= Cyc_ExpList_tok; _temp3230.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3230;});
_temp3229;}); break; case 399: _LL3228: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3232=( struct Cyc_ExpList_tok_struct*) _cycalloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3232[ 0]=({ struct Cyc_ExpList_tok_struct _temp3233; _temp3233.tag= Cyc_ExpList_tok;
_temp3233.f1=({ struct Cyc_List_List* _temp3234=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp3234->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3234->tl= 0;
_temp3234;}); _temp3233;}); _temp3232;}); break; case 400: _LL3231: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3236=( struct Cyc_ExpList_tok_struct*)
_cycalloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3236[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3237; _temp3237.tag= Cyc_ExpList_tok; _temp3237.f1=({ struct Cyc_List_List*
_temp3238=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp3238->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3238->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset -  2)]); _temp3238;});
_temp3237;}); _temp3236;}); break; case 401: _LL3235: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3240=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3240[ 0]=({ struct Cyc_Exp_tok_struct
_temp3241; _temp3241.tag= Cyc_Exp_tok; _temp3241.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3241;});
_temp3240;}); break; case 402: _LL3239: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3243=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3243[ 0]=({ struct Cyc_Exp_tok_struct _temp3244; _temp3244.tag= Cyc_Exp_tok;
_temp3244.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3244;});
_temp3243;}); break; case 403: _LL3242: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3246=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3246[ 0]=({ struct Cyc_Exp_tok_struct _temp3247; _temp3247.tag= Cyc_Exp_tok;
_temp3247.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3247;});
_temp3246;}); break; case 404: _LL3245: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3249=( struct Cyc_Exp_tok_struct*) _cycalloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3249[ 0]=({ struct Cyc_Exp_tok_struct _temp3250; _temp3250.tag= Cyc_Exp_tok;
_temp3250.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3250;});
_temp3249;}); break; case 405: _LL3248: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3252=( struct Cyc_QualId_tok_struct*) _cycalloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3252[ 0]=({ struct Cyc_QualId_tok_struct _temp3253; _temp3253.tag= Cyc_QualId_tok;
_temp3253.f1=({ struct _tuple1* _temp3254=( struct _tuple1*) _cycalloc( sizeof(
struct _tuple1)); _temp3254->f1= Cyc_Absyn_rel_ns_null; _temp3254->f2=({ struct
_tagged_arr* _temp3255=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp3255[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3255;}); _temp3254;});
_temp3253;}); _temp3252;}); break; case 406: _LL3251: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3256:
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
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 407u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 112u, yyn -  122)] +  yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >=  0?
yystate <=  4390: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4391u,
yystate)] ==  yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){
yystate=( int) Cyc_yytable[ _check_known_subscript_notnull( 4391u, yystate)];}
else{ yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 
122)];} goto yynewstate; yyerrlab: if( yyerrstatus ==  0){ ++ Cyc_yynerrs; yyn=(
int) Cyc_yypact[ _check_known_subscript_notnull( 808u, yystate)]; if( yyn >  -
32768? yyn <  4390: 0){ int sze= 0; struct _tagged_arr msg; int x; int count;
count= 0; for( x= yyn <  0? - yyn: 0; x <  234u /  sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4391u, x +  yyn)] ==  x){( sze
+= Cyc_Std_strlen( Cyc_yytname[ _check_known_subscript_notnull( 234u, x)]) +  15,
count ++);}} msg=({ unsigned int _temp3258=( unsigned int)( sze +  15);
unsigned char* _temp3259=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp3258)); struct _tagged_arr _temp3261= _tag_arr(
_temp3259, sizeof( unsigned char),( unsigned int)( sze +  15));{ unsigned int
_temp3260= _temp3258; unsigned int i; for( i= 0; i <  _temp3260; i ++){
_temp3259[ i]='\000';}}; _temp3261;}); Cyc_Std_strcpy( msg, _tag_arr("parse error",
sizeof( unsigned char), 12u)); if( count <  5){ count= 0; for( x= yyn <  0? -
yyn: 0; x <  234u /  sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4391u, x +  yyn)] ==  x){ Cyc_Std_strcat( msg,
count ==  0? _tag_arr(", expecting `", sizeof( unsigned char), 14u): _tag_arr(" or `",
sizeof( unsigned char), 6u)); Cyc_Std_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 234u, x)]); Cyc_Std_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus ==  3){ if( Cyc_yychar ==  0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset ==  0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 808u,
yystate)]; if( yyn ==  - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn <  0? 1:
yyn >  4390)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4391u, yyn)] !=  1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4391u,
yyn)]; if( yyn <  0){ if( yyn ==  - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn ==  0){ goto yyerrpop;}} if( yyn ==  807){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3271= v; struct
_tuple15* _temp3289; struct _tuple15 _temp3291; int _temp3292; unsigned char
_temp3294; short _temp3296; struct _tagged_arr _temp3298; struct Cyc_Core_Opt*
_temp3300; struct Cyc_Core_Opt* _temp3302; struct Cyc_Core_Opt _temp3304; struct
_tagged_arr* _temp3305; struct _tuple1* _temp3307; struct _tuple1 _temp3309;
struct _tagged_arr* _temp3310; void* _temp3312; _LL3273: if(*(( void**)
_temp3271) ==  Cyc_Int_tok){ _LL3290: _temp3289=(( struct Cyc_Int_tok_struct*)
_temp3271)->f1; _temp3291=* _temp3289; _LL3293: _temp3292= _temp3291.f2; goto
_LL3274;} else{ goto _LL3275;} _LL3275: if(*(( void**) _temp3271) ==  Cyc_Char_tok){
_LL3295: _temp3294=(( struct Cyc_Char_tok_struct*) _temp3271)->f1; goto _LL3276;}
else{ goto _LL3277;} _LL3277: if(*(( void**) _temp3271) ==  Cyc_Short_tok){
_LL3297: _temp3296=(( struct Cyc_Short_tok_struct*) _temp3271)->f1; goto _LL3278;}
else{ goto _LL3279;} _LL3279: if(*(( void**) _temp3271) ==  Cyc_String_tok){
_LL3299: _temp3298=(( struct Cyc_String_tok_struct*) _temp3271)->f1; goto
_LL3280;} else{ goto _LL3281;} _LL3281: if(*(( void**) _temp3271) ==  Cyc_Stringopt_tok){
_LL3301: _temp3300=(( struct Cyc_Stringopt_tok_struct*) _temp3271)->f1; if(
_temp3300 ==  0){ goto _LL3282;} else{ goto _LL3283;}} else{ goto _LL3283;}
_LL3283: if(*(( void**) _temp3271) ==  Cyc_Stringopt_tok){ _LL3303: _temp3302=((
struct Cyc_Stringopt_tok_struct*) _temp3271)->f1; if( _temp3302 ==  0){ goto
_LL3285;} else{ _temp3304=* _temp3302; _LL3306: _temp3305=( struct _tagged_arr*)
_temp3304.v; goto _LL3284;}} else{ goto _LL3285;} _LL3285: if(*(( void**)
_temp3271) ==  Cyc_QualId_tok){ _LL3308: _temp3307=(( struct Cyc_QualId_tok_struct*)
_temp3271)->f1; _temp3309=* _temp3307; _LL3313: _temp3312= _temp3309.f1; goto
_LL3311; _LL3311: _temp3310= _temp3309.f2; goto _LL3286;} else{ goto _LL3287;}
_LL3287: goto _LL3288; _LL3274:({ struct Cyc_Std_Int_pa_struct _temp3315;
_temp3315.tag= Cyc_Std_Int_pa; _temp3315.f1=( int)(( unsigned int) _temp3292);{
void* _temp3314[ 1u]={& _temp3315}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp3314, sizeof( void*), 1u));}}); goto
_LL3272; _LL3276:({ struct Cyc_Std_Int_pa_struct _temp3317; _temp3317.tag= Cyc_Std_Int_pa;
_temp3317.f1=( int)(( unsigned int)(( int) _temp3294));{ void* _temp3316[ 1u]={&
_temp3317}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%c", sizeof(
unsigned char), 3u), _tag_arr( _temp3316, sizeof( void*), 1u));}}); goto _LL3272;
_LL3278:({ struct Cyc_Std_Int_pa_struct _temp3319; _temp3319.tag= Cyc_Std_Int_pa;
_temp3319.f1=( int)(( unsigned int)(( int) _temp3296));{ void* _temp3318[ 1u]={&
_temp3319}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%ds", sizeof(
unsigned char), 4u), _tag_arr( _temp3318, sizeof( void*), 1u));}}); goto _LL3272;
_LL3280:({ struct Cyc_Std_String_pa_struct _temp3321; _temp3321.tag= Cyc_Std_String_pa;
_temp3321.f1=( struct _tagged_arr) _temp3298;{ void* _temp3320[ 1u]={& _temp3321};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof( unsigned char), 5u),
_tag_arr( _temp3320, sizeof( void*), 1u));}}); goto _LL3272; _LL3282:({ void*
_temp3322[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("NULL", sizeof(
unsigned char), 5u), _tag_arr( _temp3322, sizeof( void*), 0u));}); goto _LL3272;
_LL3284:({ struct Cyc_Std_String_pa_struct _temp3324; _temp3324.tag= Cyc_Std_String_pa;
_temp3324.f1=( struct _tagged_arr)* _temp3305;{ void* _temp3323[ 1u]={&
_temp3324}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\"%s\"", sizeof(
unsigned char), 5u), _tag_arr( _temp3323, sizeof( void*), 1u));}}); goto _LL3272;
_LL3286: { struct Cyc_List_List* _temp3325= 0;{ void* _temp3326= _temp3312;
struct Cyc_List_List* _temp3334; struct Cyc_List_List* _temp3336; _LL3328: if((
unsigned int) _temp3326 >  1u?*(( int*) _temp3326) ==  Cyc_Absyn_Rel_n: 0){
_LL3335: _temp3334=(( struct Cyc_Absyn_Rel_n_struct*) _temp3326)->f1; goto
_LL3329;} else{ goto _LL3330;} _LL3330: if(( unsigned int) _temp3326 >  1u?*((
int*) _temp3326) ==  Cyc_Absyn_Abs_n: 0){ _LL3337: _temp3336=(( struct Cyc_Absyn_Abs_n_struct*)
_temp3326)->f1; goto _LL3331;} else{ goto _LL3332;} _LL3332: if( _temp3326 == (
void*) Cyc_Absyn_Loc_n){ goto _LL3333;} else{ goto _LL3327;} _LL3329: _temp3325=
_temp3334; goto _LL3327; _LL3331: _temp3325= _temp3336; goto _LL3327; _LL3333:
goto _LL3327; _LL3327:;} for( 0; _temp3325 !=  0; _temp3325= _temp3325->tl){({
struct Cyc_Std_String_pa_struct _temp3339; _temp3339.tag= Cyc_Std_String_pa;
_temp3339.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _temp3325->hd);{ void*
_temp3338[ 1u]={& _temp3339}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::",
sizeof( unsigned char), 5u), _tag_arr( _temp3338, sizeof( void*), 1u));}});}({
struct Cyc_Std_String_pa_struct _temp3341; _temp3341.tag= Cyc_Std_String_pa;
_temp3341.f1=( struct _tagged_arr)* _temp3310;{ void* _temp3340[ 1u]={&
_temp3341}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s::", sizeof(
unsigned char), 5u), _tag_arr( _temp3340, sizeof( void*), 1u));}}); goto _LL3272;}
_LL3288:({ void* _temp3342[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("?",
sizeof( unsigned char), 2u), _tag_arr( _temp3342, sizeof( void*), 0u));}); goto
_LL3272; _LL3272:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3343=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp3343->v=(
void*) Cyc_Lexing_from_file( f); _temp3343;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}
