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
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern struct _tagged_arr
Cyc_Core_new_string( int); extern unsigned char Cyc_Core_Invalid_argument[ 21u];
struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern unsigned char
Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u];
struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{ int
tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{
int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
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
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void*
env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists(
int(* pred)( void*), struct Cyc_List_List* x); extern int Cyc_List_list_cmp( int(*
cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); extern
int Cyc_List_list_prefix( int(* cmp)( void*, void*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Std___sFILE*
f); extern struct _tagged_arr Cyc_PP_string_of_doc( struct Cyc_PP_Doc* d, int w);
extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc* Cyc_PP_line_doc();
extern struct Cyc_PP_Doc* Cyc_PP_text( struct _tagged_arr s); extern struct Cyc_PP_Doc*
Cyc_PP_textptr( struct _tagged_arr* p); extern struct Cyc_PP_Doc* Cyc_PP_nest(
int k, struct Cyc_PP_Doc* d); extern struct Cyc_PP_Doc* Cyc_PP_cat( struct
_tagged_arr); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)(
void*), struct _tagged_arr sep, struct Cyc_List_List* l); extern struct Cyc_PP_Doc*
Cyc_PP_seql( struct _tagged_arr sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr
sep, struct Cyc_List_List* l); extern struct Cyc_PP_Doc* Cyc_PP_group( struct
_tagged_arr start, struct _tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List*
l); extern struct Cyc_PP_Doc* Cyc_PP_egroup( struct _tagged_arr start, struct
_tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List* l); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; static
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
Cyc_Absyn_AnonStructType= 13; struct Cyc_Absyn_AnonStructType_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType= 14;
struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_AnonEnumType= 15; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
16; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 17; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
4; struct _tuple2{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Break_s= 5; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Continue_s= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Goto_s= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_For_s=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4; } ; static const int
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
tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s=
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
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern struct
_tagged_arr Cyc_Absyn_attribute2string( void*); struct Cyc_Buffer_t; extern int
Cyc_Std_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr* s2); extern
struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List*, struct _tagged_arr);
extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e);
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*); struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r; extern struct
Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r; extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r; extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Std___sFILE*
f); extern struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d);
extern struct _tagged_arr Cyc_Absynpp_typ2string( void*); extern struct
_tagged_arr Cyc_Absynpp_typ2cstring( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_arr Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*); extern
struct _tagged_arr Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt*); extern
struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); extern struct
_tagged_arr Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl); extern
struct _tagged_arr Cyc_Absynpp_prim2string( void* p); extern struct _tagged_arr
Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p); extern struct _tagged_arr Cyc_Absynpp_scope2string(
void* sc); extern struct _tagged_arr Cyc_Absynpp_cyc_string; extern struct
_tagged_arr* Cyc_Absynpp_cyc_stringptr; extern int Cyc_Absynpp_exp_prec( struct
Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_char_escape(
unsigned char); extern struct _tagged_arr Cyc_Absynpp_string_escape( struct
_tagged_arr); extern struct _tagged_arr Cyc_Absynpp_prim2str( void* p); extern
int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern struct
_tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{ struct
Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; extern struct Cyc_PP_Doc*
Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List*
fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e); extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct
_tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es); struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de); extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po); extern
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields);
extern struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs);
static int Cyc_Absynpp_expand_typedefs; static int Cyc_Absynpp_qvar_to_Cids;
static unsigned char _temp0[ 4u]="Cyc"; struct _tagged_arr Cyc_Absynpp_cyc_string={
_temp0, _temp0, _temp0 +  4u}; struct _tagged_arr* Cyc_Absynpp_cyc_stringptr=&
Cyc_Absynpp_cyc_string; static int Cyc_Absynpp_add_cyc_prefix; static int Cyc_Absynpp_to_VC;
static int Cyc_Absynpp_decls_first; static int Cyc_Absynpp_rewrite_temp_tvars;
static int Cyc_Absynpp_print_all_tvars; static int Cyc_Absynpp_print_all_kinds;
static int Cyc_Absynpp_print_using_stmts; static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars; static int Cyc_Absynpp_use_curr_namespace;
static struct Cyc_List_List* Cyc_Absynpp_curr_namespace= 0; struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params* fs){ Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs; Cyc_Absynpp_qvar_to_Cids= fs->qvar_to_Cids; Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix; Cyc_Absynpp_to_VC= fs->to_VC; Cyc_Absynpp_decls_first= fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars= fs->rewrite_temp_tvars; Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars; Cyc_Absynpp_print_all_kinds= fs->print_all_kinds; Cyc_Absynpp_print_using_stmts=
fs->print_using_stmts; Cyc_Absynpp_print_externC_stmts= fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars= fs->print_full_evars; Cyc_Absynpp_use_curr_namespace=
fs->use_curr_namespace; Cyc_Absynpp_curr_namespace= fs->curr_namespace;} struct
Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={ 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1,
0}; struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={ 1, 0, 0, 0, 0, 1, 0, 0,
1, 1, 0, 1, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={ 1, 1, 1, 0, 1,
0, 0, 0, 0, 0, 0, 0, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={ 0,
0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}; static void Cyc_Absynpp_curr_namespace_add(
struct _tagged_arr* v){ Cyc_Absynpp_curr_namespace=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List* _temp1=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp1->hd=( void*) v; _temp1->tl= 0; _temp1;}));} static void
Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if((( struct Cyc_List_List*)
_check_null(* l))->tl ==  0){* l= 0;} else{ Cyc_Absynpp_suppr_last(&(( struct
Cyc_List_List*) _check_null(* l))->tl);}} static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)( struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_arr Cyc_Absynpp_char_escape( unsigned char c){ switch( c){ case '\a':
_LL2: return _tag_arr("\\a", sizeof( unsigned char), 3u); case '\b': _LL3:
return _tag_arr("\\b", sizeof( unsigned char), 3u); case '\f': _LL4: return
_tag_arr("\\f", sizeof( unsigned char), 3u); case '\n': _LL5: return _tag_arr("\\n",
sizeof( unsigned char), 3u); case '\r': _LL6: return _tag_arr("\\r", sizeof(
unsigned char), 3u); case '\t': _LL7: return _tag_arr("\\t", sizeof(
unsigned char), 3u); case '\v': _LL8: return _tag_arr("\\v", sizeof(
unsigned char), 3u); case '\\': _LL9: return _tag_arr("\\\\", sizeof(
unsigned char), 3u); case '"': _LL10: return _tag_arr("\"", sizeof(
unsigned char), 2u); case '\'': _LL11: return _tag_arr("\\'", sizeof(
unsigned char), 3u); default: _LL12: if( c >= ' '? c <= '~': 0){ struct
_tagged_arr _temp14= Cyc_Core_new_string( 1);*(( unsigned char*)
_check_unknown_subscript( _temp14, sizeof( unsigned char), 0))= c; return(
struct _tagged_arr) _temp14;} else{ struct _tagged_arr _temp15= Cyc_Core_new_string(
4); int j= 0;*(( unsigned char*) _check_unknown_subscript( _temp15, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript(
_temp15, sizeof( unsigned char), j ++))=( unsigned char)('0' + (( unsigned char)
c >>  6 &  7));*(( unsigned char*) _check_unknown_subscript( _temp15, sizeof(
unsigned char), j ++))=( unsigned char)('0' + ( c >>  3 &  7));*(( unsigned char*)
_check_unknown_subscript( _temp15, sizeof( unsigned char), j ++))=(
unsigned char)('0' + ( c &  7)); return( struct _tagged_arr) _temp15;}}} static
int Cyc_Absynpp_special( struct _tagged_arr s){ int sz=( int)( _get_arr_size( s,
sizeof( unsigned char)) -  1);{ int i= 0; for( 0; i <  sz; i ++){ unsigned char
c=*(( const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
i)); if((( c <= ' '? 1: c >= '~')? 1: c == '"')? 1: c == '\\'){ return 1;}}}
return 0;} struct _tagged_arr Cyc_Absynpp_string_escape( struct _tagged_arr s){
if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)( _get_arr_size( s,
sizeof( unsigned char)) -  1); if( n >  0?*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), n)) == '\000': 0){ n --;}{
int len= 0;{ int i= 0; for( 0; i <=  n; i ++){ unsigned char _temp16=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); _LL18:
if( _temp16 == '\a'){ goto _LL19;} else{ goto _LL20;} _LL20: if( _temp16 == '\b'){
goto _LL21;} else{ goto _LL22;} _LL22: if( _temp16 == '\f'){ goto _LL23;} else{
goto _LL24;} _LL24: if( _temp16 == '\n'){ goto _LL25;} else{ goto _LL26;} _LL26:
if( _temp16 == '\r'){ goto _LL27;} else{ goto _LL28;} _LL28: if( _temp16 == '\t'){
goto _LL29;} else{ goto _LL30;} _LL30: if( _temp16 == '\v'){ goto _LL31;} else{
goto _LL32;} _LL32: if( _temp16 == '\\'){ goto _LL33;} else{ goto _LL34;} _LL34:
if( _temp16 == '"'){ goto _LL35;} else{ goto _LL36;} _LL36: goto _LL37; _LL19:
goto _LL21; _LL21: goto _LL23; _LL23: goto _LL25; _LL25: goto _LL27; _LL27: goto
_LL29; _LL29: goto _LL31; _LL31: goto _LL33; _LL33: goto _LL35; _LL35: len += 2;
goto _LL17; _LL37: if( _temp16 >= ' '? _temp16 <= '~': 0){ len ++;} else{ len +=
4;} goto _LL17; _LL17:;}}{ struct _tagged_arr t= Cyc_Core_new_string( len); int
j= 0;{ int i= 0; for( 0; i <=  n; i ++){ unsigned char _temp38=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); _LL40:
if( _temp38 == '\a'){ goto _LL41;} else{ goto _LL42;} _LL42: if( _temp38 == '\b'){
goto _LL43;} else{ goto _LL44;} _LL44: if( _temp38 == '\f'){ goto _LL45;} else{
goto _LL46;} _LL46: if( _temp38 == '\n'){ goto _LL47;} else{ goto _LL48;} _LL48:
if( _temp38 == '\r'){ goto _LL49;} else{ goto _LL50;} _LL50: if( _temp38 == '\t'){
goto _LL51;} else{ goto _LL52;} _LL52: if( _temp38 == '\v'){ goto _LL53;} else{
goto _LL54;} _LL54: if( _temp38 == '\\'){ goto _LL55;} else{ goto _LL56;} _LL56:
if( _temp38 == '"'){ goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL41:*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='a';
goto _LL39; _LL43:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='b'; goto _LL39; _LL45:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='f';
goto _LL39; _LL47:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='n'; goto _LL39; _LL49:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='r';
goto _LL39; _LL51:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='t'; goto _LL39; _LL53:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='v';
goto _LL39; _LL55:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='\\'; goto _LL39; _LL57:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='"';
goto _LL39; _LL59: if( _temp38 >= ' '? _temp38 <= '~': 0){*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))= _temp38;} else{*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))=(
unsigned char)('0' + ( _temp38 >>  6 &  7));*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))=( unsigned char)('0'
+ ( _temp38 >>  3 &  7));*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))=( unsigned char)('0' + ( _temp38 &  7));} goto
_LL39; _LL39:;}} return( struct _tagged_arr) t;}}}} static unsigned char _temp60[
9u]="restrict"; static struct _tagged_arr Cyc_Absynpp_restrict_string={ _temp60,
_temp60, _temp60 +  9u}; static unsigned char _temp61[ 9u]="volatile"; static
struct _tagged_arr Cyc_Absynpp_volatile_string={ _temp61, _temp61, _temp61 +  9u};
static unsigned char _temp62[ 6u]="const"; static struct _tagged_arr Cyc_Absynpp_const_str={
_temp62, _temp62, _temp62 +  6u}; static struct _tagged_arr* Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string; static struct _tagged_arr* Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string; static struct _tagged_arr* Cyc_Absynpp_const_sp=&
Cyc_Absynpp_const_str; struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual
tq){ struct Cyc_List_List* l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List*
_temp63=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp63->hd=( void*) Cyc_Absynpp_restrict_sp; _temp63->tl= l; _temp63;});} if(
tq.q_volatile){ l=({ struct Cyc_List_List* _temp64=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp64->hd=( void*) Cyc_Absynpp_volatile_sp;
_temp64->tl= l; _temp64;});} if( tq.q_const){ l=({ struct Cyc_List_List* _temp65=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp65->hd=(
void*) Cyc_Absynpp_const_sp; _temp65->tl= l; _temp65;});} return Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char),
2u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, l));} struct _tagged_arr Cyc_Absynpp_kind2string( void* k){ void*
_temp66= k; _LL68: if( _temp66 == ( void*) Cyc_Absyn_AnyKind){ goto _LL69;}
else{ goto _LL70;} _LL70: if( _temp66 == ( void*) Cyc_Absyn_MemKind){ goto _LL71;}
else{ goto _LL72;} _LL72: if( _temp66 == ( void*) Cyc_Absyn_BoxKind){ goto _LL73;}
else{ goto _LL74;} _LL74: if( _temp66 == ( void*) Cyc_Absyn_RgnKind){ goto _LL75;}
else{ goto _LL76;} _LL76: if( _temp66 == ( void*) Cyc_Absyn_EffKind){ goto _LL77;}
else{ goto _LL67;} _LL69: return _tag_arr("A", sizeof( unsigned char), 2u);
_LL71: return _tag_arr("M", sizeof( unsigned char), 2u); _LL73: return _tag_arr("B",
sizeof( unsigned char), 2u); _LL75: return _tag_arr("R", sizeof( unsigned char),
2u); _LL77: return _tag_arr("E", sizeof( unsigned char), 2u); _LL67:;} struct
_tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void* _temp78=(
void*)( Cyc_Absyn_compress_conref( c))->v; void* _temp84; _LL80: if((
unsigned int) _temp78 >  1u?*(( int*) _temp78) ==  Cyc_Absyn_Eq_constr: 0){
_LL85: _temp84=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp78)->f1; goto
_LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL81: return Cyc_Absynpp_kind2string(
_temp84); _LL83: return _tag_arr("?", sizeof( unsigned char), 2u); _LL79:;}
struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc( void* k){ return Cyc_PP_text( Cyc_Absynpp_kind2string(
k));} struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc( struct Cyc_Absyn_Conref* c){
return Cyc_PP_text( Cyc_Absynpp_ckind2string( c));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc(
struct Cyc_List_List* ts){ return Cyc_PP_egroup( _tag_arr("<", sizeof(
unsigned char), 2u), _tag_arr(">", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc, ts));}
struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){ void*
_temp86=( void*)( Cyc_Absyn_compress_conref( tv->kind))->v; void* _temp94; void*
_temp96; _LL88: if(( unsigned int) _temp86 >  1u?*(( int*) _temp86) ==  Cyc_Absyn_Eq_constr:
0){ _LL95: _temp94=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp86)->f1;
if( _temp94 == ( void*) Cyc_Absyn_BoxKind){ goto _LL89;} else{ goto _LL90;}}
else{ goto _LL90;} _LL90: if(( unsigned int) _temp86 >  1u?*(( int*) _temp86) == 
Cyc_Absyn_Eq_constr: 0){ _LL97: _temp96=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp86)->f1; goto _LL91;} else{ goto _LL92;} _LL92: goto _LL93; _LL89: return
Cyc_PP_textptr( tv->name); _LL91: return({ struct Cyc_PP_Doc* _temp98[ 3u];
_temp98[ 2u]= Cyc_Absynpp_kind2doc( _temp96); _temp98[ 1u]= Cyc_PP_text(
_tag_arr("::", sizeof( unsigned char), 3u)); _temp98[ 0u]= Cyc_PP_textptr( tv->name);
Cyc_PP_cat( _tag_arr( _temp98, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL93:
return({ struct Cyc_PP_Doc* _temp99[ 2u]; _temp99[ 1u]= Cyc_PP_text( _tag_arr("/*::?*/",
sizeof( unsigned char), 8u)); _temp99[ 0u]= Cyc_PP_textptr( tv->name); Cyc_PP_cat(
_tag_arr( _temp99, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL87:;} struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc( struct Cyc_List_List* tvs){ return Cyc_PP_egroup(
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_arr* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_PP_egroup( _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_PP_textptr,(( struct Cyc_List_List*(*)( struct
_tagged_arr*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_get_name, tvs)));} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((*
t).f1,(* t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List*
ts){ return Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_noncallatt2doc(
void* att){ void* _temp100= att; _LL102: if( _temp100 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL103;} else{ goto _LL104;} _LL104: if( _temp100 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL105;} else{ goto _LL106;} _LL106: if( _temp100 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL107;} else{ goto _LL108;} _LL108: goto _LL109; _LL103: return Cyc_PP_nil_doc();
_LL105: return Cyc_PP_nil_doc(); _LL107: return Cyc_PP_nil_doc(); _LL109: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL101:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts !=  0; atts= atts->tl){ void* _temp110=(
void*) atts->hd; _LL112: if( _temp110 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL113;} else{ goto _LL114;} _LL114: if( _temp110 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL115;} else{ goto _LL116;} _LL116: if( _temp110 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL117;} else{ goto _LL118;} _LL118: goto _LL119; _LL113: return Cyc_PP_text(
_tag_arr(" _stdcall ", sizeof( unsigned char), 11u)); _LL115: return Cyc_PP_text(
_tag_arr(" _cdecl ", sizeof( unsigned char), 9u)); _LL117: return Cyc_PP_text(
_tag_arr(" _fastcall ", sizeof( unsigned char), 12u)); _LL119: goto _LL111;
_LL111:;} return Cyc_PP_nil_doc();} struct Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 !=  0; atts2= atts2->tl){ void* _temp120=( void*) atts2->hd;
_LL122: if( _temp120 == ( void*) Cyc_Absyn_Stdcall_att){ goto _LL123;} else{
goto _LL124;} _LL124: if( _temp120 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL125;}
else{ goto _LL126;} _LL126: if( _temp120 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL127;} else{ goto _LL128;} _LL128: goto _LL129; _LL123: goto _LL121;
_LL125: goto _LL121; _LL127: goto _LL121; _LL129: hasatt= 1; goto _LL121; _LL121:;}}
if( ! hasatt){ return Cyc_PP_nil_doc();} return({ struct Cyc_PP_Doc* _temp130[ 3u];
_temp130[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp130[
1u]= Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Absynpp_noncallatt2doc, atts)); _temp130[ 0u]= Cyc_PP_text(
_tag_arr("__declspec(", sizeof( unsigned char), 12u)); Cyc_PP_cat( _tag_arr(
_temp130, sizeof( struct Cyc_PP_Doc*), 3u));});} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc(
void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts ==  0){ return Cyc_PP_nil_doc();}
if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc( atts);} return({
struct Cyc_PP_Doc* _temp131[ 2u]; _temp131[ 1u]= Cyc_PP_group( _tag_arr("((",
sizeof( unsigned char), 3u), _tag_arr("))", sizeof( unsigned char), 3u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc,
atts)); _temp131[ 0u]= Cyc_PP_text( _tag_arr(" __attribute__", sizeof(
unsigned char), 15u)); Cyc_PP_cat( _tag_arr( _temp131, sizeof( struct Cyc_PP_Doc*),
2u));});} int Cyc_Absynpp_next_is_pointer( struct Cyc_List_List* tms){ if( tms
==  0){ return 0;}{ void* _temp132=( void*) tms->hd; _LL134: if(( unsigned int)
_temp132 >  1u?*(( int*) _temp132) ==  Cyc_Absyn_Pointer_mod: 0){ goto _LL135;}
else{ goto _LL136;} _LL136: if(( unsigned int) _temp132 >  1u?*(( int*) _temp132)
==  Cyc_Absyn_Attributes_mod: 0){ goto _LL137;} else{ goto _LL138;} _LL138: goto
_LL139; _LL135: return 1; _LL137: if( ! Cyc_Absynpp_to_VC){ return 0;} return
Cyc_Absynpp_next_is_pointer( tms->tl); _LL139: return 0; _LL133:;}} extern
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms ==  0){ return d;}{
struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d, tms->tl); struct Cyc_PP_Doc*
p_rest=({ struct Cyc_PP_Doc* _temp222[ 3u]; _temp222[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp222[ 1u]= rest; _temp222[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp222,
sizeof( struct Cyc_PP_Doc*), 3u));}); void* _temp140=( void*) tms->hd; struct
Cyc_Absyn_Exp* _temp154; void* _temp156; struct Cyc_List_List* _temp158; int
_temp160; struct Cyc_Position_Segment* _temp162; struct Cyc_List_List* _temp164;
struct Cyc_Absyn_Tqual _temp166; void* _temp168; void* _temp170; _LL142: if(
_temp140 == ( void*) Cyc_Absyn_Carray_mod){ goto _LL143;} else{ goto _LL144;}
_LL144: if(( unsigned int) _temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_ConstArray_mod:
0){ _LL155: _temp154=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp140)->f1;
goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int) _temp140 >  1u?*((
int*) _temp140) ==  Cyc_Absyn_Function_mod: 0){ _LL157: _temp156=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp140)->f1; goto _LL147;} else{ goto
_LL148;} _LL148: if(( unsigned int) _temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_Attributes_mod:
0){ _LL159: _temp158=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp140)->f2;
goto _LL149;} else{ goto _LL150;} _LL150: if(( unsigned int) _temp140 >  1u?*((
int*) _temp140) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL165: _temp164=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp140)->f1; goto _LL163; _LL163: _temp162=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp140)->f2; goto _LL161; _LL161: _temp160=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp140)->f3; goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int)
_temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_Pointer_mod: 0){ _LL171:
_temp170=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f1; goto
_LL169; _LL169: _temp168=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp140)->f2; goto _LL167; _LL167: _temp166=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp140)->f3; goto _LL153;} else{ goto _LL141;} _LL143: if( Cyc_Absynpp_next_is_pointer(
tms->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc* _temp172[ 2u]; _temp172[ 1u]=
Cyc_PP_text( _tag_arr("[]", sizeof( unsigned char), 3u)); _temp172[ 0u]= rest;
Cyc_PP_cat( _tag_arr( _temp172, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL145:
if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*
_temp173[ 4u]; _temp173[ 3u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char),
2u)); _temp173[ 2u]= Cyc_Absynpp_exp2doc( _temp154); _temp173[ 1u]= Cyc_PP_text(
_tag_arr("[", sizeof( unsigned char), 2u)); _temp173[ 0u]= rest; Cyc_PP_cat(
_tag_arr( _temp173, sizeof( struct Cyc_PP_Doc*), 4u));}); _LL147: if( Cyc_Absynpp_next_is_pointer(
tms->tl)){ rest= p_rest;}{ void* _temp174= _temp156; struct Cyc_List_List*
_temp180; struct Cyc_Core_Opt* _temp182; struct Cyc_Absyn_VarargInfo* _temp184;
int _temp186; struct Cyc_List_List* _temp188; struct Cyc_Position_Segment*
_temp190; struct Cyc_List_List* _temp192; _LL176: if(*(( int*) _temp174) ==  Cyc_Absyn_WithTypes){
_LL189: _temp188=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f1; goto
_LL187; _LL187: _temp186=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f2;
goto _LL185; _LL185: _temp184=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f3;
goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f4;
goto _LL181; _LL181: _temp180=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f5;
goto _LL177;} else{ goto _LL178;} _LL178: if(*(( int*) _temp174) ==  Cyc_Absyn_NoTypes){
_LL193: _temp192=(( struct Cyc_Absyn_NoTypes_struct*) _temp174)->f1; goto _LL191;
_LL191: _temp190=(( struct Cyc_Absyn_NoTypes_struct*) _temp174)->f2; goto _LL179;}
else{ goto _LL175;} _LL177: return({ struct Cyc_PP_Doc* _temp194[ 2u]; _temp194[
1u]= Cyc_Absynpp_funargs2doc( _temp188, _temp186, _temp184, _temp182, _temp180);
_temp194[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp194, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL179: return({ struct Cyc_PP_Doc* _temp195[ 2u]; _temp195[ 1u]= Cyc_PP_group(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, _temp192)); _temp195[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp195,
sizeof( struct Cyc_PP_Doc*), 2u));}); _LL175:;} _LL149: if( ! Cyc_Absynpp_to_VC){
if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*
_temp196[ 2u]; _temp196[ 1u]= Cyc_Absynpp_atts2doc( _temp158); _temp196[ 0u]=
rest; Cyc_PP_cat( _tag_arr( _temp196, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ if( Cyc_Absynpp_next_is_pointer( tms->tl)){ return({ struct Cyc_PP_Doc*
_temp197[ 2u]; _temp197[ 1u]= rest; _temp197[ 0u]= Cyc_Absynpp_callconv2doc(
_temp158); Cyc_PP_cat( _tag_arr( _temp197, sizeof( struct Cyc_PP_Doc*), 2u));});}
return rest;} _LL151: if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest= p_rest;}
if( _temp160){ return({ struct Cyc_PP_Doc* _temp198[ 2u]; _temp198[ 1u]= Cyc_Absynpp_ktvars2doc(
_temp164); _temp198[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp198, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp199[ 2u];
_temp199[ 1u]= Cyc_Absynpp_tvars2doc( _temp164); _temp199[ 0u]= rest; Cyc_PP_cat(
_tag_arr( _temp199, sizeof( struct Cyc_PP_Doc*), 2u));});} _LL153: { struct Cyc_PP_Doc*
ptr;{ void* _temp200= _temp170; struct Cyc_Absyn_Exp* _temp208; struct Cyc_Absyn_Exp*
_temp210; _LL202: if(( unsigned int) _temp200 >  1u?*(( int*) _temp200) ==  Cyc_Absyn_Nullable_ps:
0){ _LL209: _temp208=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp200)->f1;
goto _LL203;} else{ goto _LL204;} _LL204: if(( unsigned int) _temp200 >  1u?*((
int*) _temp200) ==  Cyc_Absyn_NonNullable_ps: 0){ _LL211: _temp210=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp200)->f1; goto _LL205;} else{ goto _LL206;} _LL206: if( _temp200 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL207;} else{ goto _LL201;} _LL203: if( Cyc_Evexp_eval_const_uint_exp(
_temp208) ==  1){ ptr= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc* _temp212[ 4u]; _temp212[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp212[ 2u]= Cyc_Absynpp_exp2doc(
_temp208); _temp212[ 1u]= Cyc_PP_text( _tag_arr("{", sizeof( unsigned char), 2u));
_temp212[ 0u]= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp212, sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL201; _LL205:
if( Cyc_Evexp_eval_const_uint_exp( _temp210) ==  1){ ptr= Cyc_PP_text( _tag_arr("@",
sizeof( unsigned char), 2u));} else{ ptr=({ struct Cyc_PP_Doc* _temp213[ 4u];
_temp213[ 3u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp213[
2u]= Cyc_Absynpp_exp2doc( _temp210); _temp213[ 1u]= Cyc_PP_text( _tag_arr("{",
sizeof( unsigned char), 2u)); _temp213[ 0u]= Cyc_PP_text( _tag_arr("@", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp213, sizeof( struct Cyc_PP_Doc*),
4u));});} goto _LL201; _LL207: ptr= Cyc_PP_text( _tag_arr("?", sizeof(
unsigned char), 2u)); goto _LL201; _LL201:;}{ void* _temp214= Cyc_Tcutil_compress(
_temp168); _LL216: if( _temp214 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL217;}
else{ goto _LL218;} _LL218: goto _LL219; _LL217: return({ struct Cyc_PP_Doc*
_temp220[ 2u]; _temp220[ 1u]= rest; _temp220[ 0u]= ptr; Cyc_PP_cat( _tag_arr(
_temp220, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL219: return({ struct Cyc_PP_Doc*
_temp221[ 4u]; _temp221[ 3u]= rest; _temp221[ 2u]= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); _temp221[ 1u]= Cyc_Absynpp_typ2doc( _temp168);
_temp221[ 0u]= ptr; Cyc_PP_cat( _tag_arr( _temp221, sizeof( struct Cyc_PP_Doc*),
4u));}); _LL215:;}} _LL141:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){
void* _temp223= Cyc_Tcutil_compress( t); _LL225: if( _temp223 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL226;} else{ goto _LL227;} _LL227: goto _LL228; _LL226: return Cyc_PP_text(
_tag_arr("`H", sizeof( unsigned char), 3u)); _LL228: return Cyc_Absynpp_ntyp2doc(
t); _LL224:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
rgions, struct Cyc_List_List** effects, void* t){ void* _temp229= Cyc_Tcutil_compress(
t); void* _temp237; struct Cyc_List_List* _temp239; _LL231: if(( unsigned int)
_temp229 >  3u?*(( int*) _temp229) ==  Cyc_Absyn_AccessEff: 0){ _LL238: _temp237=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp229)->f1; goto _LL232;} else{
goto _LL233;} _LL233: if(( unsigned int) _temp229 >  3u?*(( int*) _temp229) == 
Cyc_Absyn_JoinEff: 0){ _LL240: _temp239=(( struct Cyc_Absyn_JoinEff_struct*)
_temp229)->f1; goto _LL234;} else{ goto _LL235;} _LL235: goto _LL236; _LL232:*
rgions=({ struct Cyc_List_List* _temp241=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp241->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp237); _temp241->tl=* rgions; _temp241;}); goto _LL230; _LL234: for( 0;
_temp239 !=  0; _temp239= _temp239->tl){ Cyc_Absynpp_effects2docs( rgions,
effects,( void*) _temp239->hd);} goto _LL230; _LL236:* effects=({ struct Cyc_List_List*
_temp242=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp242->hd=( void*) Cyc_Absynpp_typ2doc( t); _temp242->tl=* effects; _temp242;});
goto _LL230; _LL230:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* rgions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
rgions,& effects, t); rgions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( rgions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( rgions ==  0? effects !=  0: 0){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("+", sizeof( unsigned char), 2u), effects);} else{ struct Cyc_PP_Doc*
_temp243= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u), rgions);
return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("+", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
effects,({ struct Cyc_List_List* _temp244=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp244->hd=( void*) _temp243; _temp244->tl= 0;
_temp244;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp245= t; struct Cyc_Core_Opt* _temp295; int _temp297; struct Cyc_Core_Opt*
_temp299; struct Cyc_Core_Opt* _temp301; struct Cyc_Absyn_Tvar* _temp303; struct
Cyc_Absyn_TunionInfo _temp305; void* _temp307; struct Cyc_List_List* _temp309;
void* _temp311; struct Cyc_Absyn_TunionFieldInfo _temp313; struct Cyc_List_List*
_temp315; void* _temp317; void* _temp319; void* _temp321; int _temp323; struct
Cyc_List_List* _temp325; struct Cyc_List_List* _temp327; struct _tuple0*
_temp329; struct Cyc_List_List* _temp331; struct _tuple0* _temp333; struct Cyc_List_List*
_temp335; struct Cyc_List_List* _temp337; struct Cyc_List_List* _temp339; struct
_tuple0* _temp341; struct Cyc_Core_Opt* _temp343; struct Cyc_List_List* _temp345;
struct _tuple0* _temp347; void* _temp349; void* _temp351; _LL247: if((
unsigned int) _temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_ArrayType: 0){
goto _LL248;} else{ goto _LL249;} _LL249: if(( unsigned int) _temp245 >  3u?*((
int*) _temp245) ==  Cyc_Absyn_FnType: 0){ goto _LL250;} else{ goto _LL251;}
_LL251: if(( unsigned int) _temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_PointerType:
0){ goto _LL252;} else{ goto _LL253;} _LL253: if( _temp245 == ( void*) Cyc_Absyn_VoidType){
goto _LL254;} else{ goto _LL255;} _LL255: if(( unsigned int) _temp245 >  3u?*((
int*) _temp245) ==  Cyc_Absyn_Evar: 0){ _LL302: _temp301=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f1; goto _LL300; _LL300: _temp299=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f2; goto _LL298; _LL298: _temp297=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f3; goto _LL296; _LL296: _temp295=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f4; goto _LL256;} else{ goto _LL257;} _LL257: if(( unsigned int)
_temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_VarType: 0){ _LL304: _temp303=((
struct Cyc_Absyn_VarType_struct*) _temp245)->f1; goto _LL258;} else{ goto _LL259;}
_LL259: if(( unsigned int) _temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_TunionType:
0){ _LL306: _temp305=(( struct Cyc_Absyn_TunionType_struct*) _temp245)->f1;
_LL312: _temp311=( void*) _temp305.tunion_info; goto _LL310; _LL310: _temp309=
_temp305.targs; goto _LL308; _LL308: _temp307=( void*) _temp305.rgn; goto _LL260;}
else{ goto _LL261;} _LL261: if(( unsigned int) _temp245 >  3u?*(( int*) _temp245)
==  Cyc_Absyn_TunionFieldType: 0){ _LL314: _temp313=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp245)->f1; _LL318: _temp317=( void*) _temp313.field_info; goto _LL316;
_LL316: _temp315= _temp313.targs; goto _LL262;} else{ goto _LL263;} _LL263: if((
unsigned int) _temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_IntType: 0){
_LL322: _temp321=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp245)->f1;
goto _LL320; _LL320: _temp319=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp245)->f2; goto _LL264;} else{ goto _LL265;} _LL265: if( _temp245 == ( void*)
Cyc_Absyn_FloatType){ goto _LL266;} else{ goto _LL267;} _LL267: if((
unsigned int) _temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_DoubleType: 0){
_LL324: _temp323=(( struct Cyc_Absyn_DoubleType_struct*) _temp245)->f1; goto
_LL268;} else{ goto _LL269;} _LL269: if(( unsigned int) _temp245 >  3u?*(( int*)
_temp245) ==  Cyc_Absyn_TupleType: 0){ _LL326: _temp325=(( struct Cyc_Absyn_TupleType_struct*)
_temp245)->f1; goto _LL270;} else{ goto _LL271;} _LL271: if(( unsigned int)
_temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_StructType: 0){ _LL330:
_temp329=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f1; goto _LL328;
_LL328: _temp327=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f2; goto
_LL272;} else{ goto _LL273;} _LL273: if(( unsigned int) _temp245 >  3u?*(( int*)
_temp245) ==  Cyc_Absyn_UnionType: 0){ _LL334: _temp333=(( struct Cyc_Absyn_UnionType_struct*)
_temp245)->f1; goto _LL332; _LL332: _temp331=(( struct Cyc_Absyn_UnionType_struct*)
_temp245)->f2; goto _LL274;} else{ goto _LL275;} _LL275: if(( unsigned int)
_temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_AnonStructType: 0){ _LL336:
_temp335=(( struct Cyc_Absyn_AnonStructType_struct*) _temp245)->f1; goto _LL276;}
else{ goto _LL277;} _LL277: if(( unsigned int) _temp245 >  3u?*(( int*) _temp245)
==  Cyc_Absyn_AnonUnionType: 0){ _LL338: _temp337=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp245)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int)
_temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_AnonEnumType: 0){ _LL340:
_temp339=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp245)->f1; goto _LL280;}
else{ goto _LL281;} _LL281: if(( unsigned int) _temp245 >  3u?*(( int*) _temp245)
==  Cyc_Absyn_EnumType: 0){ _LL342: _temp341=(( struct Cyc_Absyn_EnumType_struct*)
_temp245)->f1; goto _LL282;} else{ goto _LL283;} _LL283: if(( unsigned int)
_temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_TypedefType: 0){ _LL348:
_temp347=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f1; goto _LL346;
_LL346: _temp345=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f2; goto
_LL344; _LL344: _temp343=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f3;
goto _LL284;} else{ goto _LL285;} _LL285: if(( unsigned int) _temp245 >  3u?*((
int*) _temp245) ==  Cyc_Absyn_RgnHandleType: 0){ _LL350: _temp349=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp245)->f1; goto _LL286;} else{ goto
_LL287;} _LL287: if( _temp245 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL288;}
else{ goto _LL289;} _LL289: if(( unsigned int) _temp245 >  3u?*(( int*) _temp245)
==  Cyc_Absyn_RgnsEff: 0){ _LL352: _temp351=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp245)->f1; goto _LL290;} else{ goto _LL291;} _LL291: if(( unsigned int)
_temp245 >  3u?*(( int*) _temp245) ==  Cyc_Absyn_AccessEff: 0){ goto _LL292;}
else{ goto _LL293;} _LL293: if(( unsigned int) _temp245 >  3u?*(( int*) _temp245)
==  Cyc_Absyn_JoinEff: 0){ goto _LL294;} else{ goto _LL246;} _LL248: return Cyc_PP_text(
_tag_arr("[[[array]]]", sizeof( unsigned char), 12u)); _LL250: return Cyc_PP_nil_doc();
_LL252: return Cyc_PP_nil_doc(); _LL254: s= Cyc_PP_text( _tag_arr("void",
sizeof( unsigned char), 5u)); goto _LL246; _LL256: if( _temp299 !=  0){ return
Cyc_Absynpp_ntyp2doc(( void*) _temp299->v);} else{ s=({ struct Cyc_PP_Doc*
_temp353[ 6u]; _temp353[ 5u]= _temp301 ==  0? Cyc_PP_text( _tag_arr("?", sizeof(
unsigned char), 2u)): Cyc_Absynpp_kind2doc(( void*) _temp301->v); _temp353[ 4u]=
Cyc_PP_text( _tag_arr(")::", sizeof( unsigned char), 4u)); _temp353[ 3u]=( ! Cyc_Absynpp_print_full_evars?
1: _temp295 ==  0)? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*) _temp295->v); _temp353[ 2u]= Cyc_PP_text(( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp355; _temp355.tag= Cyc_Std_Int_pa;
_temp355.f1=( int)(( unsigned int) _temp297);{ void* _temp354[ 1u]={& _temp355};
Cyc_Std_aprintf( _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp354,
sizeof( void*), 1u));}})); _temp353[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); _temp353[ 0u]= Cyc_PP_text( _tag_arr("%", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp353, sizeof( struct Cyc_PP_Doc*),
6u));});} goto _LL246; _LL258: s= Cyc_PP_textptr( _temp303->name); if( Cyc_Absynpp_print_all_kinds){
s=({ struct Cyc_PP_Doc* _temp356[ 3u]; _temp356[ 2u]= Cyc_Absynpp_ckind2doc(
_temp303->kind); _temp356[ 1u]= Cyc_PP_text( _tag_arr("::", sizeof(
unsigned char), 3u)); _temp356[ 0u]= s; Cyc_PP_cat( _tag_arr( _temp356, sizeof(
struct Cyc_PP_Doc*), 3u));});} if( Cyc_Absynpp_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp303): 0){ s=({ struct Cyc_PP_Doc* _temp357[ 3u]; _temp357[ 2u]= Cyc_PP_text(
_tag_arr(" */", sizeof( unsigned char), 4u)); _temp357[ 1u]= s; _temp357[ 0u]=
Cyc_PP_text( _tag_arr("_ /* ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp357, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL260:{
void* _temp358= _temp311; struct Cyc_Absyn_UnknownTunionInfo _temp364; int
_temp366; struct _tuple0* _temp368; struct Cyc_Absyn_Tuniondecl** _temp370;
struct Cyc_Absyn_Tuniondecl* _temp372; struct Cyc_Absyn_Tuniondecl _temp373; int
_temp374; struct _tuple0* _temp376; _LL360: if(*(( int*) _temp358) ==  Cyc_Absyn_UnknownTunion){
_LL365: _temp364=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp358)->f1;
_LL369: _temp368= _temp364.name; goto _LL367; _LL367: _temp366= _temp364.is_xtunion;
goto _LL361;} else{ goto _LL362;} _LL362: if(*(( int*) _temp358) ==  Cyc_Absyn_KnownTunion){
_LL371: _temp370=(( struct Cyc_Absyn_KnownTunion_struct*) _temp358)->f1;
_temp372=* _temp370; _temp373=* _temp372; _LL377: _temp376= _temp373.name; goto
_LL375; _LL375: _temp374= _temp373.is_xtunion; goto _LL363;} else{ goto _LL359;}
_LL361: _temp376= _temp368; _temp374= _temp366; goto _LL363; _LL363: { struct
Cyc_PP_Doc* _temp378= Cyc_PP_text( _temp374? _tag_arr("xtunion ", sizeof(
unsigned char), 9u): _tag_arr("tunion ", sizeof( unsigned char), 8u));{ void*
_temp379= Cyc_Tcutil_compress( _temp307); _LL381: if( _temp379 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL382;} else{ goto _LL383;} _LL383: goto _LL384; _LL382: s=({ struct Cyc_PP_Doc*
_temp385[ 3u]; _temp385[ 2u]= Cyc_Absynpp_tps2doc( _temp309); _temp385[ 1u]= Cyc_Absynpp_qvar2doc(
_temp376); _temp385[ 0u]= _temp378; Cyc_PP_cat( _tag_arr( _temp385, sizeof(
struct Cyc_PP_Doc*), 3u));}); goto _LL380; _LL384: s=({ struct Cyc_PP_Doc*
_temp386[ 5u]; _temp386[ 4u]= Cyc_Absynpp_tps2doc( _temp309); _temp386[ 3u]= Cyc_Absynpp_qvar2doc(
_temp376); _temp386[ 2u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp386[ 1u]= Cyc_Absynpp_typ2doc( _temp307); _temp386[ 0u]= _temp378; Cyc_PP_cat(
_tag_arr( _temp386, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL380; _LL380:;}
goto _LL359;} _LL359:;} goto _LL246; _LL262:{ void* _temp387= _temp317; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp393; int _temp395; struct _tuple0*
_temp397; struct _tuple0* _temp399; struct Cyc_Absyn_Tunionfield* _temp401;
struct Cyc_Absyn_Tunionfield _temp403; struct _tuple0* _temp404; struct Cyc_Absyn_Tuniondecl*
_temp406; struct Cyc_Absyn_Tuniondecl _temp408; int _temp409; struct _tuple0*
_temp411; _LL389: if(*(( int*) _temp387) ==  Cyc_Absyn_UnknownTunionfield){
_LL394: _temp393=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp387)->f1;
_LL400: _temp399= _temp393.tunion_name; goto _LL398; _LL398: _temp397= _temp393.field_name;
goto _LL396; _LL396: _temp395= _temp393.is_xtunion; goto _LL390;} else{ goto
_LL391;} _LL391: if(*(( int*) _temp387) ==  Cyc_Absyn_KnownTunionfield){ _LL407:
_temp406=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp387)->f1; _temp408=*
_temp406; _LL412: _temp411= _temp408.name; goto _LL410; _LL410: _temp409=
_temp408.is_xtunion; goto _LL402; _LL402: _temp401=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp387)->f2; _temp403=* _temp401; _LL405: _temp404= _temp403.name; goto _LL392;}
else{ goto _LL388;} _LL390: _temp411= _temp399; _temp409= _temp395; _temp404=
_temp397; goto _LL392; _LL392: { struct Cyc_PP_Doc* _temp413= Cyc_PP_text(
_temp409? _tag_arr("xtunion ", sizeof( unsigned char), 9u): _tag_arr("tunion ",
sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc* _temp414[ 4u]; _temp414[ 3u]=
Cyc_Absynpp_qvar2doc( _temp404); _temp414[ 2u]= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); _temp414[ 1u]= Cyc_Absynpp_qvar2doc( _temp411);
_temp414[ 0u]= _temp413; Cyc_PP_cat( _tag_arr( _temp414, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL388;} _LL388:;} goto _LL246; _LL264: { struct _tagged_arr sns;
struct _tagged_arr ts;{ void* _temp415= _temp321; _LL417: if( _temp415 == ( void*)
Cyc_Absyn_Signed){ goto _LL418;} else{ goto _LL419;} _LL419: if( _temp415 == (
void*) Cyc_Absyn_Unsigned){ goto _LL420;} else{ goto _LL416;} _LL418: sns=
_tag_arr("", sizeof( unsigned char), 1u); goto _LL416; _LL420: sns= _tag_arr("unsigned ",
sizeof( unsigned char), 10u); goto _LL416; _LL416:;}{ void* _temp421= _temp319;
_LL423: if( _temp421 == ( void*) Cyc_Absyn_B1){ goto _LL424;} else{ goto _LL425;}
_LL425: if( _temp421 == ( void*) Cyc_Absyn_B2){ goto _LL426;} else{ goto _LL427;}
_LL427: if( _temp421 == ( void*) Cyc_Absyn_B4){ goto _LL428;} else{ goto _LL429;}
_LL429: if( _temp421 == ( void*) Cyc_Absyn_B8){ goto _LL430;} else{ goto _LL422;}
_LL424:{ void* _temp431= _temp321; _LL433: if( _temp431 == ( void*) Cyc_Absyn_Signed){
goto _LL434;} else{ goto _LL435;} _LL435: if( _temp431 == ( void*) Cyc_Absyn_Unsigned){
goto _LL436;} else{ goto _LL432;} _LL434: sns= _tag_arr("signed ", sizeof(
unsigned char), 8u); goto _LL432; _LL436: goto _LL432; _LL432:;} ts= _tag_arr("char",
sizeof( unsigned char), 5u); goto _LL422; _LL426: ts= _tag_arr("short", sizeof(
unsigned char), 6u); goto _LL422; _LL428: ts= _tag_arr("int", sizeof(
unsigned char), 4u); goto _LL422; _LL430: ts= Cyc_Absynpp_to_VC? _tag_arr("__int64",
sizeof( unsigned char), 8u): _tag_arr("long long", sizeof( unsigned char), 10u);
goto _LL422; _LL422:;} s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp439; _temp439.tag= Cyc_Std_String_pa; _temp439.f1=( struct _tagged_arr) ts;{
struct Cyc_Std_String_pa_struct _temp438; _temp438.tag= Cyc_Std_String_pa;
_temp438.f1=( struct _tagged_arr) sns;{ void* _temp437[ 2u]={& _temp438,&
_temp439}; Cyc_Std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp437, sizeof( void*), 2u));}}})); goto _LL246;} _LL266: s= Cyc_PP_text(
_tag_arr("float", sizeof( unsigned char), 6u)); goto _LL246; _LL268: if(
_temp323){ s= Cyc_PP_text( _tag_arr("long double", sizeof( unsigned char), 12u));}
else{ s= Cyc_PP_text( _tag_arr("double", sizeof( unsigned char), 7u));} goto
_LL246; _LL270: s=({ struct Cyc_PP_Doc* _temp440[ 2u]; _temp440[ 1u]= Cyc_Absynpp_args2doc(
_temp325); _temp440[ 0u]= Cyc_PP_text( _tag_arr("$", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp440, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL246; _LL272: if( _temp329 ==  0){ s=({ struct Cyc_PP_Doc* _temp441[ 2u];
_temp441[ 1u]= Cyc_Absynpp_tps2doc( _temp327); _temp441[ 0u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp441,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp442[ 3u];
_temp442[ 2u]= Cyc_Absynpp_tps2doc( _temp327); _temp442[ 1u]= _temp329 ==  0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp329));
_temp442[ 0u]= Cyc_PP_text( _tag_arr("struct ", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp442, sizeof( struct Cyc_PP_Doc*), 3u));});} goto
_LL246; _LL274: if( _temp333 ==  0){ s=({ struct Cyc_PP_Doc* _temp443[ 2u];
_temp443[ 1u]= Cyc_Absynpp_tps2doc( _temp331); _temp443[ 0u]= Cyc_PP_text(
_tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp443,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp444[ 3u];
_temp444[ 2u]= Cyc_Absynpp_tps2doc( _temp331); _temp444[ 1u]= _temp333 ==  0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp333));
_temp444[ 0u]= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp444, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL276:
s=({ struct Cyc_PP_Doc* _temp445[ 3u]; _temp445[ 2u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp445[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp335)); _temp445[ 0u]= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp445, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL278: s=({ struct Cyc_PP_Doc* _temp446[ 3u]; _temp446[ 2u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp446[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_structfields2doc( _temp337)); _temp446[ 0u]= Cyc_PP_text(
_tag_arr("union {", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp446,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL280: s=({ struct Cyc_PP_Doc*
_temp447[ 3u]; _temp447[ 2u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp447[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_enumfields2doc( _temp339));
_temp447[ 0u]= Cyc_PP_text( _tag_arr("enum {", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp447, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL282: s=({
struct Cyc_PP_Doc* _temp448[ 2u]; _temp448[ 1u]= Cyc_Absynpp_qvar2doc( _temp341);
_temp448[ 0u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp448, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL284: s=({
struct Cyc_PP_Doc* _temp449[ 2u]; _temp449[ 1u]= Cyc_Absynpp_tps2doc( _temp345);
_temp449[ 0u]= Cyc_Absynpp_qvar2doc( _temp347); Cyc_PP_cat( _tag_arr( _temp449,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL286: s=({ struct Cyc_PP_Doc*
_temp450[ 3u]; _temp450[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof( unsigned char),
2u)); _temp450[ 1u]= Cyc_Absynpp_rgn2doc( _temp349); _temp450[ 0u]= Cyc_PP_text(
_tag_arr("region_t<", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp450, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL288: s= Cyc_Absynpp_rgn2doc(
t); goto _LL246; _LL290: s=({ struct Cyc_PP_Doc* _temp451[ 3u]; _temp451[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp451[ 1u]= Cyc_Absynpp_typ2doc(
_temp351); _temp451[ 0u]= Cyc_PP_text( _tag_arr("regions(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp451, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL292: goto _LL294; _LL294: s= Cyc_Absynpp_eff2doc( t);
goto _LL246; _LL246:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct
Cyc_Core_Opt* vo){ return vo ==  0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct
_tagged_arr*) vo->v));} struct _tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc*
Cyc_Absynpp_rgn_cmp2doc( struct _tuple8* cmp){ struct _tuple8 _temp454; void*
_temp455; void* _temp457; struct _tuple8* _temp452= cmp; _temp454=* _temp452;
_LL458: _temp457= _temp454.f1; goto _LL456; _LL456: _temp455= _temp454.f2; goto
_LL453; _LL453: return({ struct Cyc_PP_Doc* _temp459[ 3u]; _temp459[ 2u]= Cyc_Absynpp_rgn2doc(
_temp455); _temp459[ 1u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp459[ 0u]= Cyc_Absynpp_rgn2doc( _temp457); Cyc_PP_cat( _tag_arr( _temp459,
sizeof( struct Cyc_PP_Doc*), 3u));});} struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List* po){ return Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 ==  0? 0:({ struct Cyc_Core_Opt* _temp460=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp460->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp460;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp461=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp461=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp461,({
struct Cyc_List_List* _temp462=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp462->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp462->tl= 0; _temp462;}));} else{ if(
cyc_varargs !=  0){ struct Cyc_PP_Doc* _temp463=({ struct Cyc_PP_Doc* _temp465[
3u]; _temp465[ 2u]= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp466=( struct
_tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp466->f1= cyc_varargs->name;
_temp466->f2= cyc_varargs->tq; _temp466->f3=( void*) cyc_varargs->type; _temp466;}));
_temp465[ 1u]= cyc_varargs->inject? Cyc_PP_text( _tag_arr(" inject ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp465[ 0u]= Cyc_PP_text( _tag_arr("...", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp465, sizeof( struct Cyc_PP_Doc*), 3u));}); _temp461=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp461,({
struct Cyc_List_List* _temp464=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp464->hd=( void*) _temp463; _temp464->tl= 0;
_temp464;}));}}{ struct Cyc_PP_Doc* _temp467= Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u), _temp461); if( effopt !=  0){ _temp467=({ struct
Cyc_PP_Doc* _temp468[ 3u]; _temp468[ 2u]= Cyc_Absynpp_eff2doc(( void*) effopt->v);
_temp468[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp468[
0u]= _temp467; Cyc_PP_cat( _tag_arr( _temp468, sizeof( struct Cyc_PP_Doc*), 3u));});}
if( rgn_po !=  0){ _temp467=({ struct Cyc_PP_Doc* _temp469[ 3u]; _temp469[ 2u]=
Cyc_Absynpp_rgnpo2doc( rgn_po); _temp469[ 1u]= Cyc_PP_text( _tag_arr(":",
sizeof( unsigned char), 2u)); _temp469[ 0u]= _temp467; Cyc_PP_cat( _tag_arr(
_temp469, sizeof( struct Cyc_PP_Doc*), 3u));});} return({ struct Cyc_PP_Doc*
_temp470[ 3u]; _temp470[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp470[ 1u]= _temp467; _temp470[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp470, sizeof( struct Cyc_PP_Doc*),
3u));});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){ return({
struct _tuple1* _temp471=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1));
_temp471->f1=({ struct Cyc_Core_Opt* _temp472=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp472->v=( void*)(* arg).f1; _temp472;});
_temp471->f2=(* arg).f2; _temp471->f3=(* arg).f3; _temp471;});} struct Cyc_PP_Doc*
Cyc_Absynpp_var2doc( struct _tagged_arr* v){ return Cyc_PP_text(* v);} struct
Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
_temp473= 0; int match;{ void* _temp474=(* q).f1; struct Cyc_List_List* _temp482;
struct Cyc_List_List* _temp484; _LL476: if( _temp474 == ( void*) Cyc_Absyn_Loc_n){
goto _LL477;} else{ goto _LL478;} _LL478: if(( unsigned int) _temp474 >  1u?*((
int*) _temp474) ==  Cyc_Absyn_Rel_n: 0){ _LL483: _temp482=(( struct Cyc_Absyn_Rel_n_struct*)
_temp474)->f1; goto _LL479;} else{ goto _LL480;} _LL480: if(( unsigned int)
_temp474 >  1u?*(( int*) _temp474) ==  Cyc_Absyn_Abs_n: 0){ _LL485: _temp484=((
struct Cyc_Absyn_Abs_n_struct*) _temp474)->f1; goto _LL481;} else{ goto _LL475;}
_LL477: _temp482= 0; goto _LL479; _LL479: match= 0; _temp473= _temp482; goto
_LL475; _LL481: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_Std_strptrcmp, _temp484, Cyc_Absynpp_curr_namespace):
0; _temp473=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp486=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp486->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp486->tl= _temp484;
_temp486;}): _temp484; goto _LL475; _LL475:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp473,({
struct Cyc_List_List* _temp487=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp487->hd=( void*)(* q).f2; _temp487->tl= 0; _temp487;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp473,({ struct Cyc_List_List* _temp488=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp488->hd=( void*)(* q).f2; _temp488->tl= 0;
_temp488;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp489=(* v).f1; struct Cyc_List_List* _temp499; struct Cyc_List_List*
_temp501; _LL491: if( _temp489 == ( void*) Cyc_Absyn_Loc_n){ goto _LL492;} else{
goto _LL493;} _LL493: if(( unsigned int) _temp489 >  1u?*(( int*) _temp489) == 
Cyc_Absyn_Rel_n: 0){ _LL500: _temp499=(( struct Cyc_Absyn_Rel_n_struct*)
_temp489)->f1; if( _temp499 ==  0){ goto _LL494;} else{ goto _LL495;}} else{
goto _LL495;} _LL495: if(( unsigned int) _temp489 >  1u?*(( int*) _temp489) == 
Cyc_Absyn_Abs_n: 0){ _LL502: _temp501=(( struct Cyc_Absyn_Abs_n_struct*)
_temp489)->f1; goto _LL496;} else{ goto _LL497;} _LL497: goto _LL498; _LL492:
goto _LL494; _LL494: return Cyc_Absynpp_var2doc((* v).f2); _LL496: if((( int(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_strptrcmp, _temp501, Cyc_Absynpp_curr_namespace)
==  0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL498;} _LL498:
return({ struct Cyc_PP_Doc* _temp503[ 2u]; _temp503[ 1u]= Cyc_Absynpp_qvar2doc(
v); _temp503[ 0u]= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof(
unsigned char), 23u)); Cyc_PP_cat( _tag_arr( _temp503, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL490:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp504=(
void*) e->r; void* _temp582; struct Cyc_Absyn_Exp* _temp584; struct Cyc_Absyn_Exp*
_temp586; _LL506: if(*(( int*) _temp504) ==  Cyc_Absyn_Const_e){ goto _LL507;}
else{ goto _LL508;} _LL508: if(*(( int*) _temp504) ==  Cyc_Absyn_Var_e){ goto
_LL509;} else{ goto _LL510;} _LL510: if(*(( int*) _temp504) ==  Cyc_Absyn_UnknownId_e){
goto _LL511;} else{ goto _LL512;} _LL512: if(*(( int*) _temp504) ==  Cyc_Absyn_Primop_e){
_LL583: _temp582=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp504)->f1;
goto _LL513;} else{ goto _LL514;} _LL514: if(*(( int*) _temp504) ==  Cyc_Absyn_AssignOp_e){
goto _LL515;} else{ goto _LL516;} _LL516: if(*(( int*) _temp504) ==  Cyc_Absyn_Increment_e){
goto _LL517;} else{ goto _LL518;} _LL518: if(*(( int*) _temp504) ==  Cyc_Absyn_Conditional_e){
goto _LL519;} else{ goto _LL520;} _LL520: if(*(( int*) _temp504) ==  Cyc_Absyn_SeqExp_e){
goto _LL521;} else{ goto _LL522;} _LL522: if(*(( int*) _temp504) ==  Cyc_Absyn_UnknownCall_e){
goto _LL523;} else{ goto _LL524;} _LL524: if(*(( int*) _temp504) ==  Cyc_Absyn_FnCall_e){
goto _LL525;} else{ goto _LL526;} _LL526: if(*(( int*) _temp504) ==  Cyc_Absyn_Throw_e){
goto _LL527;} else{ goto _LL528;} _LL528: if(*(( int*) _temp504) ==  Cyc_Absyn_NoInstantiate_e){
_LL585: _temp584=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp504)->f1;
goto _LL529;} else{ goto _LL530;} _LL530: if(*(( int*) _temp504) ==  Cyc_Absyn_Instantiate_e){
_LL587: _temp586=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp504)->f1; goto
_LL531;} else{ goto _LL532;} _LL532: if(*(( int*) _temp504) ==  Cyc_Absyn_Cast_e){
goto _LL533;} else{ goto _LL534;} _LL534: if(*(( int*) _temp504) ==  Cyc_Absyn_New_e){
goto _LL535;} else{ goto _LL536;} _LL536: if(*(( int*) _temp504) ==  Cyc_Absyn_Address_e){
goto _LL537;} else{ goto _LL538;} _LL538: if(*(( int*) _temp504) ==  Cyc_Absyn_Sizeoftyp_e){
goto _LL539;} else{ goto _LL540;} _LL540: if(*(( int*) _temp504) ==  Cyc_Absyn_Sizeofexp_e){
goto _LL541;} else{ goto _LL542;} _LL542: if(*(( int*) _temp504) ==  Cyc_Absyn_Offsetof_e){
goto _LL543;} else{ goto _LL544;} _LL544: if(*(( int*) _temp504) ==  Cyc_Absyn_Gentyp_e){
goto _LL545;} else{ goto _LL546;} _LL546: if(*(( int*) _temp504) ==  Cyc_Absyn_Deref_e){
goto _LL547;} else{ goto _LL548;} _LL548: if(*(( int*) _temp504) ==  Cyc_Absyn_StructMember_e){
goto _LL549;} else{ goto _LL550;} _LL550: if(*(( int*) _temp504) ==  Cyc_Absyn_StructArrow_e){
goto _LL551;} else{ goto _LL552;} _LL552: if(*(( int*) _temp504) ==  Cyc_Absyn_Subscript_e){
goto _LL553;} else{ goto _LL554;} _LL554: if(*(( int*) _temp504) ==  Cyc_Absyn_Tuple_e){
goto _LL555;} else{ goto _LL556;} _LL556: if(*(( int*) _temp504) ==  Cyc_Absyn_CompoundLit_e){
goto _LL557;} else{ goto _LL558;} _LL558: if(*(( int*) _temp504) ==  Cyc_Absyn_Array_e){
goto _LL559;} else{ goto _LL560;} _LL560: if(*(( int*) _temp504) ==  Cyc_Absyn_Comprehension_e){
goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp504) ==  Cyc_Absyn_Struct_e){
goto _LL563;} else{ goto _LL564;} _LL564: if(*(( int*) _temp504) ==  Cyc_Absyn_AnonStruct_e){
goto _LL565;} else{ goto _LL566;} _LL566: if(*(( int*) _temp504) ==  Cyc_Absyn_Tunion_e){
goto _LL567;} else{ goto _LL568;} _LL568: if(*(( int*) _temp504) ==  Cyc_Absyn_Enum_e){
goto _LL569;} else{ goto _LL570;} _LL570: if(*(( int*) _temp504) ==  Cyc_Absyn_AnonEnum_e){
goto _LL571;} else{ goto _LL572;} _LL572: if(*(( int*) _temp504) ==  Cyc_Absyn_Malloc_e){
goto _LL573;} else{ goto _LL574;} _LL574: if(*(( int*) _temp504) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL575;} else{ goto _LL576;} _LL576: if(*(( int*) _temp504) ==  Cyc_Absyn_StmtExp_e){
goto _LL577;} else{ goto _LL578;} _LL578: if(*(( int*) _temp504) ==  Cyc_Absyn_Codegen_e){
goto _LL579;} else{ goto _LL580;} _LL580: if(*(( int*) _temp504) ==  Cyc_Absyn_Fill_e){
goto _LL581;} else{ goto _LL505;} _LL507: return 10000; _LL509: return 10000;
_LL511: return 10000; _LL513: { void* _temp588= _temp582; _LL590: if( _temp588
== ( void*) Cyc_Absyn_Plus){ goto _LL591;} else{ goto _LL592;} _LL592: if(
_temp588 == ( void*) Cyc_Absyn_Times){ goto _LL593;} else{ goto _LL594;} _LL594:
if( _temp588 == ( void*) Cyc_Absyn_Minus){ goto _LL595;} else{ goto _LL596;}
_LL596: if( _temp588 == ( void*) Cyc_Absyn_Div){ goto _LL597;} else{ goto _LL598;}
_LL598: if( _temp588 == ( void*) Cyc_Absyn_Mod){ goto _LL599;} else{ goto _LL600;}
_LL600: if( _temp588 == ( void*) Cyc_Absyn_Eq){ goto _LL601;} else{ goto _LL602;}
_LL602: if( _temp588 == ( void*) Cyc_Absyn_Neq){ goto _LL603;} else{ goto _LL604;}
_LL604: if( _temp588 == ( void*) Cyc_Absyn_Gt){ goto _LL605;} else{ goto _LL606;}
_LL606: if( _temp588 == ( void*) Cyc_Absyn_Lt){ goto _LL607;} else{ goto _LL608;}
_LL608: if( _temp588 == ( void*) Cyc_Absyn_Gte){ goto _LL609;} else{ goto _LL610;}
_LL610: if( _temp588 == ( void*) Cyc_Absyn_Lte){ goto _LL611;} else{ goto _LL612;}
_LL612: if( _temp588 == ( void*) Cyc_Absyn_Not){ goto _LL613;} else{ goto _LL614;}
_LL614: if( _temp588 == ( void*) Cyc_Absyn_Bitnot){ goto _LL615;} else{ goto
_LL616;} _LL616: if( _temp588 == ( void*) Cyc_Absyn_Bitand){ goto _LL617;} else{
goto _LL618;} _LL618: if( _temp588 == ( void*) Cyc_Absyn_Bitor){ goto _LL619;}
else{ goto _LL620;} _LL620: if( _temp588 == ( void*) Cyc_Absyn_Bitxor){ goto
_LL621;} else{ goto _LL622;} _LL622: if( _temp588 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL623;} else{ goto _LL624;} _LL624: if( _temp588 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL625;} else{ goto _LL626;} _LL626: if( _temp588 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL627;} else{ goto _LL628;} _LL628: if( _temp588 == ( void*) Cyc_Absyn_Size){
goto _LL629;} else{ goto _LL589;} _LL591: return 100; _LL593: return 110; _LL595:
return 100; _LL597: return 110; _LL599: return 110; _LL601: return 70; _LL603:
return 70; _LL605: return 80; _LL607: return 80; _LL609: return 80; _LL611:
return 80; _LL613: return 130; _LL615: return 130; _LL617: return 60; _LL619:
return 40; _LL621: return 50; _LL623: return 90; _LL625: return 80; _LL627:
return 80; _LL629: return 140; _LL589:;} _LL515: return 20; _LL517: return 130;
_LL519: return 30; _LL521: return 10; _LL523: return 140; _LL525: return 140;
_LL527: return 130; _LL529: return Cyc_Absynpp_exp_prec( _temp584); _LL531:
return Cyc_Absynpp_exp_prec( _temp586); _LL533: return 120; _LL535: return 15;
_LL537: return 130; _LL539: return 130; _LL541: return 130; _LL543: return 130;
_LL545: return 130; _LL547: return 130; _LL549: return 140; _LL551: return 140;
_LL553: return 140; _LL555: return 150; _LL557: goto _LL559; _LL559: goto _LL561;
_LL561: goto _LL563; _LL563: goto _LL565; _LL565: goto _LL567; _LL567: goto
_LL569; _LL569: goto _LL571; _LL571: goto _LL573; _LL573: return 140; _LL575:
return 140; _LL577: return 10000; _LL579: return 140; _LL581: return 140; _LL505:;}
struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp630=(
void*) e->r; void* _temp710; struct _tuple0* _temp712; struct _tuple0* _temp714;
struct Cyc_List_List* _temp716; void* _temp718; struct Cyc_Absyn_Exp* _temp720;
struct Cyc_Core_Opt* _temp722; struct Cyc_Absyn_Exp* _temp724; void* _temp726;
struct Cyc_Absyn_Exp* _temp728; struct Cyc_Absyn_Exp* _temp730; struct Cyc_Absyn_Exp*
_temp732; struct Cyc_Absyn_Exp* _temp734; struct Cyc_Absyn_Exp* _temp736; struct
Cyc_Absyn_Exp* _temp738; struct Cyc_List_List* _temp740; struct Cyc_Absyn_Exp*
_temp742; struct Cyc_List_List* _temp744; struct Cyc_Absyn_Exp* _temp746; struct
Cyc_Absyn_Exp* _temp748; struct Cyc_Absyn_Exp* _temp750; struct Cyc_Absyn_Exp*
_temp752; struct Cyc_Absyn_Exp* _temp754; void* _temp756; struct Cyc_Absyn_Exp*
_temp758; struct Cyc_Absyn_Exp* _temp760; struct Cyc_Absyn_Exp* _temp762; void*
_temp764; struct Cyc_Absyn_Exp* _temp766; void* _temp768; struct _tagged_arr*
_temp770; void* _temp772; void* _temp774; unsigned int _temp776; void* _temp778;
void* _temp780; struct Cyc_List_List* _temp782; struct Cyc_Absyn_Exp* _temp784;
struct _tagged_arr* _temp786; struct Cyc_Absyn_Exp* _temp788; struct _tagged_arr*
_temp790; struct Cyc_Absyn_Exp* _temp792; struct Cyc_Absyn_Exp* _temp794; struct
Cyc_Absyn_Exp* _temp796; struct Cyc_List_List* _temp798; struct Cyc_List_List*
_temp800; struct _tuple1* _temp802; struct Cyc_List_List* _temp804; struct Cyc_Absyn_Exp*
_temp806; struct Cyc_Absyn_Exp* _temp808; struct Cyc_Absyn_Vardecl* _temp810;
struct Cyc_List_List* _temp812; struct _tuple0* _temp814; struct Cyc_List_List*
_temp816; struct Cyc_Absyn_Tunionfield* _temp818; struct Cyc_List_List* _temp820;
struct _tuple0* _temp822; struct _tuple0* _temp824; void* _temp826; struct Cyc_Absyn_Exp*
_temp828; struct Cyc_List_List* _temp830; struct Cyc_Core_Opt* _temp832; struct
Cyc_Absyn_Stmt* _temp834; struct Cyc_Absyn_Fndecl* _temp836; struct Cyc_Absyn_Exp*
_temp838; _LL632: if(*(( int*) _temp630) ==  Cyc_Absyn_Const_e){ _LL711:
_temp710=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp630)->f1; goto _LL633;}
else{ goto _LL634;} _LL634: if(*(( int*) _temp630) ==  Cyc_Absyn_Var_e){ _LL713:
_temp712=(( struct Cyc_Absyn_Var_e_struct*) _temp630)->f1; goto _LL635;} else{
goto _LL636;} _LL636: if(*(( int*) _temp630) ==  Cyc_Absyn_UnknownId_e){ _LL715:
_temp714=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp630)->f1; goto _LL637;}
else{ goto _LL638;} _LL638: if(*(( int*) _temp630) ==  Cyc_Absyn_Primop_e){
_LL719: _temp718=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp630)->f1;
goto _LL717; _LL717: _temp716=(( struct Cyc_Absyn_Primop_e_struct*) _temp630)->f2;
goto _LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp630) ==  Cyc_Absyn_AssignOp_e){
_LL725: _temp724=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp630)->f1; goto
_LL723; _LL723: _temp722=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp630)->f2;
goto _LL721; _LL721: _temp720=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp630)->f3;
goto _LL641;} else{ goto _LL642;} _LL642: if(*(( int*) _temp630) ==  Cyc_Absyn_Increment_e){
_LL729: _temp728=(( struct Cyc_Absyn_Increment_e_struct*) _temp630)->f1; goto
_LL727; _LL727: _temp726=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp630)->f2; goto _LL643;} else{ goto _LL644;} _LL644: if(*(( int*) _temp630)
==  Cyc_Absyn_Conditional_e){ _LL735: _temp734=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp630)->f1; goto _LL733; _LL733: _temp732=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp630)->f2; goto _LL731; _LL731: _temp730=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp630)->f3; goto _LL645;} else{ goto _LL646;} _LL646: if(*(( int*) _temp630)
==  Cyc_Absyn_SeqExp_e){ _LL739: _temp738=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp630)->f1; goto _LL737; _LL737: _temp736=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp630)->f2; goto _LL647;} else{ goto _LL648;} _LL648: if(*(( int*) _temp630)
==  Cyc_Absyn_UnknownCall_e){ _LL743: _temp742=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp630)->f1; goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp630)->f2; goto _LL649;} else{ goto _LL650;} _LL650: if(*(( int*) _temp630)
==  Cyc_Absyn_FnCall_e){ _LL747: _temp746=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp630)->f1; goto _LL745; _LL745: _temp744=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp630)->f2; goto _LL651;} else{ goto _LL652;} _LL652: if(*(( int*) _temp630)
==  Cyc_Absyn_Throw_e){ _LL749: _temp748=(( struct Cyc_Absyn_Throw_e_struct*)
_temp630)->f1; goto _LL653;} else{ goto _LL654;} _LL654: if(*(( int*) _temp630)
==  Cyc_Absyn_NoInstantiate_e){ _LL751: _temp750=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp630)->f1; goto _LL655;} else{ goto _LL656;} _LL656: if(*(( int*) _temp630)
==  Cyc_Absyn_Instantiate_e){ _LL753: _temp752=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp630)->f1; goto _LL657;} else{ goto _LL658;} _LL658: if(*(( int*) _temp630)
==  Cyc_Absyn_Cast_e){ _LL757: _temp756=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp630)->f1; goto _LL755; _LL755: _temp754=(( struct Cyc_Absyn_Cast_e_struct*)
_temp630)->f2; goto _LL659;} else{ goto _LL660;} _LL660: if(*(( int*) _temp630)
==  Cyc_Absyn_Address_e){ _LL759: _temp758=(( struct Cyc_Absyn_Address_e_struct*)
_temp630)->f1; goto _LL661;} else{ goto _LL662;} _LL662: if(*(( int*) _temp630)
==  Cyc_Absyn_New_e){ _LL763: _temp762=(( struct Cyc_Absyn_New_e_struct*)
_temp630)->f1; goto _LL761; _LL761: _temp760=(( struct Cyc_Absyn_New_e_struct*)
_temp630)->f2; goto _LL663;} else{ goto _LL664;} _LL664: if(*(( int*) _temp630)
==  Cyc_Absyn_Sizeoftyp_e){ _LL765: _temp764=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp630)->f1; goto _LL665;} else{ goto _LL666;} _LL666: if(*(( int*) _temp630)
==  Cyc_Absyn_Sizeofexp_e){ _LL767: _temp766=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp630)->f1; goto _LL667;} else{ goto _LL668;} _LL668: if(*(( int*) _temp630)
==  Cyc_Absyn_Offsetof_e){ _LL773: _temp772=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp630)->f1; goto _LL769; _LL769: _temp768=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp630)->f2; if(*(( int*) _temp768) ==  Cyc_Absyn_StructField){ _LL771:
_temp770=(( struct Cyc_Absyn_StructField_struct*) _temp768)->f1; goto _LL669;}
else{ goto _LL670;}} else{ goto _LL670;} _LL670: if(*(( int*) _temp630) ==  Cyc_Absyn_Offsetof_e){
_LL779: _temp778=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp630)->f1;
goto _LL775; _LL775: _temp774=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp630)->f2; if(*(( int*) _temp774) ==  Cyc_Absyn_TupleIndex){ _LL777:
_temp776=(( struct Cyc_Absyn_TupleIndex_struct*) _temp774)->f1; goto _LL671;}
else{ goto _LL672;}} else{ goto _LL672;} _LL672: if(*(( int*) _temp630) ==  Cyc_Absyn_Gentyp_e){
_LL783: _temp782=(( struct Cyc_Absyn_Gentyp_e_struct*) _temp630)->f1; goto
_LL781; _LL781: _temp780=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp630)->f2;
goto _LL673;} else{ goto _LL674;} _LL674: if(*(( int*) _temp630) ==  Cyc_Absyn_Deref_e){
_LL785: _temp784=(( struct Cyc_Absyn_Deref_e_struct*) _temp630)->f1; goto _LL675;}
else{ goto _LL676;} _LL676: if(*(( int*) _temp630) ==  Cyc_Absyn_StructMember_e){
_LL789: _temp788=(( struct Cyc_Absyn_StructMember_e_struct*) _temp630)->f1; goto
_LL787; _LL787: _temp786=(( struct Cyc_Absyn_StructMember_e_struct*) _temp630)->f2;
goto _LL677;} else{ goto _LL678;} _LL678: if(*(( int*) _temp630) ==  Cyc_Absyn_StructArrow_e){
_LL793: _temp792=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp630)->f1; goto
_LL791; _LL791: _temp790=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp630)->f2;
goto _LL679;} else{ goto _LL680;} _LL680: if(*(( int*) _temp630) ==  Cyc_Absyn_Subscript_e){
_LL797: _temp796=(( struct Cyc_Absyn_Subscript_e_struct*) _temp630)->f1; goto
_LL795; _LL795: _temp794=(( struct Cyc_Absyn_Subscript_e_struct*) _temp630)->f2;
goto _LL681;} else{ goto _LL682;} _LL682: if(*(( int*) _temp630) ==  Cyc_Absyn_Tuple_e){
_LL799: _temp798=(( struct Cyc_Absyn_Tuple_e_struct*) _temp630)->f1; goto _LL683;}
else{ goto _LL684;} _LL684: if(*(( int*) _temp630) ==  Cyc_Absyn_CompoundLit_e){
_LL803: _temp802=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp630)->f1; goto
_LL801; _LL801: _temp800=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp630)->f2;
goto _LL685;} else{ goto _LL686;} _LL686: if(*(( int*) _temp630) ==  Cyc_Absyn_Array_e){
_LL805: _temp804=(( struct Cyc_Absyn_Array_e_struct*) _temp630)->f1; goto _LL687;}
else{ goto _LL688;} _LL688: if(*(( int*) _temp630) ==  Cyc_Absyn_Comprehension_e){
_LL811: _temp810=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp630)->f1;
goto _LL809; _LL809: _temp808=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp630)->f2; goto _LL807; _LL807: _temp806=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp630)->f3; goto _LL689;} else{ goto _LL690;} _LL690: if(*(( int*) _temp630)
==  Cyc_Absyn_Struct_e){ _LL815: _temp814=(( struct Cyc_Absyn_Struct_e_struct*)
_temp630)->f1; goto _LL813; _LL813: _temp812=(( struct Cyc_Absyn_Struct_e_struct*)
_temp630)->f3; goto _LL691;} else{ goto _LL692;} _LL692: if(*(( int*) _temp630)
==  Cyc_Absyn_AnonStruct_e){ _LL817: _temp816=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp630)->f2; goto _LL693;} else{ goto _LL694;} _LL694: if(*(( int*) _temp630)
==  Cyc_Absyn_Tunion_e){ _LL821: _temp820=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp630)->f3; goto _LL819; _LL819: _temp818=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp630)->f5; goto _LL695;} else{ goto _LL696;} _LL696: if(*(( int*) _temp630)
==  Cyc_Absyn_Enum_e){ _LL823: _temp822=(( struct Cyc_Absyn_Enum_e_struct*)
_temp630)->f1; goto _LL697;} else{ goto _LL698;} _LL698: if(*(( int*) _temp630)
==  Cyc_Absyn_AnonEnum_e){ _LL825: _temp824=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp630)->f1; goto _LL699;} else{ goto _LL700;} _LL700: if(*(( int*) _temp630)
==  Cyc_Absyn_Malloc_e){ _LL829: _temp828=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp630)->f1; goto _LL827; _LL827: _temp826=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp630)->f2; goto _LL701;} else{ goto _LL702;} _LL702: if(*(( int*) _temp630)
==  Cyc_Absyn_UnresolvedMem_e){ _LL833: _temp832=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp630)->f1; goto _LL831; _LL831: _temp830=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp630)->f2; goto _LL703;} else{ goto _LL704;} _LL704: if(*(( int*) _temp630)
==  Cyc_Absyn_StmtExp_e){ _LL835: _temp834=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp630)->f1; goto _LL705;} else{ goto _LL706;} _LL706: if(*(( int*) _temp630)
==  Cyc_Absyn_Codegen_e){ _LL837: _temp836=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp630)->f1; goto _LL707;} else{ goto _LL708;} _LL708: if(*(( int*) _temp630)
==  Cyc_Absyn_Fill_e){ _LL839: _temp838=(( struct Cyc_Absyn_Fill_e_struct*)
_temp630)->f1; goto _LL709;} else{ goto _LL631;} _LL633: s= Cyc_Absynpp_cnst2doc(
_temp710); goto _LL631; _LL635: _temp714= _temp712; goto _LL637; _LL637: s= Cyc_Absynpp_qvar2doc(
_temp714); goto _LL631; _LL639: s= Cyc_Absynpp_primapp2doc( myprec, _temp718,
_temp716); goto _LL631; _LL641: s=({ struct Cyc_PP_Doc* _temp840[ 5u]; _temp840[
4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp720); _temp840[ 3u]= Cyc_PP_text(
_tag_arr("= ", sizeof( unsigned char), 3u)); _temp840[ 2u]= _temp722 ==  0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)
_temp722->v); _temp840[ 1u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); _temp840[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp724); Cyc_PP_cat(
_tag_arr( _temp840, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL631; _LL643: {
struct Cyc_PP_Doc* _temp841= Cyc_Absynpp_exp2doc_prec( myprec, _temp728);{ void*
_temp842= _temp726; _LL844: if( _temp842 == ( void*) Cyc_Absyn_PreInc){ goto
_LL845;} else{ goto _LL846;} _LL846: if( _temp842 == ( void*) Cyc_Absyn_PreDec){
goto _LL847;} else{ goto _LL848;} _LL848: if( _temp842 == ( void*) Cyc_Absyn_PostInc){
goto _LL849;} else{ goto _LL850;} _LL850: if( _temp842 == ( void*) Cyc_Absyn_PostDec){
goto _LL851;} else{ goto _LL843;} _LL845: s=({ struct Cyc_PP_Doc* _temp852[ 2u];
_temp852[ 1u]= _temp841; _temp852[ 0u]= Cyc_PP_text( _tag_arr("++", sizeof(
unsigned char), 3u)); Cyc_PP_cat( _tag_arr( _temp852, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL843; _LL847: s=({ struct Cyc_PP_Doc* _temp853[ 2u]; _temp853[ 1u]=
_temp841; _temp853[ 0u]= Cyc_PP_text( _tag_arr("--", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp853, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL843; _LL849: s=({ struct Cyc_PP_Doc* _temp854[ 2u]; _temp854[ 1u]= Cyc_PP_text(
_tag_arr("++", sizeof( unsigned char), 3u)); _temp854[ 0u]= _temp841; Cyc_PP_cat(
_tag_arr( _temp854, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL843; _LL851: s=({
struct Cyc_PP_Doc* _temp855[ 2u]; _temp855[ 1u]= Cyc_PP_text( _tag_arr("--",
sizeof( unsigned char), 3u)); _temp855[ 0u]= _temp841; Cyc_PP_cat( _tag_arr(
_temp855, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL843; _LL843:;} goto
_LL631;} _LL645:{ struct _tuple8 _temp857=({ struct _tuple8 _temp856; _temp856.f1=(
void*) _temp732->r; _temp856.f2=( void*) _temp730->r; _temp856;}); _LL859: goto
_LL860; _LL860: s=({ struct Cyc_PP_Doc* _temp861[ 5u]; _temp861[ 4u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp730); _temp861[ 3u]= Cyc_PP_text( _tag_arr(" : ", sizeof(
unsigned char), 4u)); _temp861[ 2u]= Cyc_Absynpp_exp2doc_prec( 0, _temp732);
_temp861[ 1u]= Cyc_PP_text( _tag_arr(" ? ", sizeof( unsigned char), 4u));
_temp861[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp734); Cyc_PP_cat( _tag_arr(
_temp861, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL858; _LL858:;} goto
_LL631; _LL647: s=({ struct Cyc_PP_Doc* _temp862[ 5u]; _temp862[ 4u]= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); _temp862[ 3u]= Cyc_Absynpp_exp2doc(
_temp736); _temp862[ 2u]= Cyc_PP_text( _tag_arr(", ", sizeof( unsigned char), 3u));
_temp862[ 1u]= Cyc_Absynpp_exp2doc( _temp738); _temp862[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp862,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL631; _LL649: s=({ struct Cyc_PP_Doc*
_temp863[ 4u]; _temp863[ 3u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp863[ 2u]= Cyc_Absynpp_exps2doc_prec( 20, _temp740); _temp863[ 1u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); _temp863[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp742); Cyc_PP_cat( _tag_arr( _temp863, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL631; _LL651: s=({ struct Cyc_PP_Doc* _temp864[ 4u]; _temp864[ 3u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp864[ 2u]= Cyc_Absynpp_exps2doc_prec(
20, _temp744); _temp864[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); _temp864[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp746); Cyc_PP_cat(
_tag_arr( _temp864, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL631; _LL653: s=({
struct Cyc_PP_Doc* _temp865[ 2u]; _temp865[ 1u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp748); _temp865[ 0u]= Cyc_PP_text( _tag_arr("throw ", sizeof(
unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp865, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL631; _LL655: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp750);
goto _LL631; _LL657: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp752); goto _LL631;
_LL659: s=({ struct Cyc_PP_Doc* _temp866[ 4u]; _temp866[ 3u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp754); _temp866[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp866[ 1u]= Cyc_Absynpp_typ2doc( _temp756); _temp866[ 0u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr(
_temp866, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL631; _LL661: s=({ struct
Cyc_PP_Doc* _temp867[ 2u]; _temp867[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp758); _temp867[ 0u]= Cyc_PP_text( _tag_arr("&", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp867, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL631; _LL663: if( _temp762 ==  0){ s=({ struct Cyc_PP_Doc* _temp868[ 2u];
_temp868[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp760); _temp868[ 0u]= Cyc_PP_text(
_tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp868,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp869[ 4u];
_temp869[ 3u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp760); _temp869[ 2u]= Cyc_PP_text(
_tag_arr(") ", sizeof( unsigned char), 3u)); _temp869[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp762)); _temp869[ 0u]= Cyc_PP_text(
_tag_arr("rnew(", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp869,
sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL631; _LL665: s=({ struct Cyc_PP_Doc*
_temp870[ 3u]; _temp870[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp870[ 1u]= Cyc_Absynpp_typ2doc( _temp764); _temp870[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp870,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL631; _LL667: s=({ struct Cyc_PP_Doc*
_temp871[ 3u]; _temp871[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp871[ 1u]= Cyc_Absynpp_exp2doc( _temp766); _temp871[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp871,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL631; _LL669: s=({ struct Cyc_PP_Doc*
_temp872[ 5u]; _temp872[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp872[ 3u]= Cyc_PP_textptr( _temp770); _temp872[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp872[ 1u]= Cyc_Absynpp_typ2doc(
_temp772); _temp872[ 0u]= Cyc_PP_text( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp872, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL631; _LL671: s=({ struct Cyc_PP_Doc* _temp873[ 5u]; _temp873[ 4u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp873[ 3u]= Cyc_PP_text((
struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp875; _temp875.tag= Cyc_Std_Int_pa;
_temp875.f1=( int) _temp776;{ void* _temp874[ 1u]={& _temp875}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp874, sizeof( void*),
1u));}})); _temp873[ 2u]= Cyc_PP_text( _tag_arr(",", sizeof( unsigned char), 2u));
_temp873[ 1u]= Cyc_Absynpp_typ2doc( _temp778); _temp873[ 0u]= Cyc_PP_text(
_tag_arr("offsetof(", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp873, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL631; _LL673: s=({ struct
Cyc_PP_Doc* _temp876[ 4u]; _temp876[ 3u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp876[ 2u]= Cyc_Absynpp_typ2doc( _temp780); _temp876[ 1u]=
Cyc_Absynpp_tvars2doc( _temp782); _temp876[ 0u]= Cyc_PP_text( _tag_arr("__gen(",
sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp876, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL631; _LL675: s=({ struct Cyc_PP_Doc* _temp877[ 2u]; _temp877[ 1u]=
Cyc_Absynpp_exp2doc_prec( myprec, _temp784); _temp877[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp877,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL631; _LL677: s=({ struct Cyc_PP_Doc*
_temp878[ 3u]; _temp878[ 2u]= Cyc_PP_textptr( _temp786); _temp878[ 1u]= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); _temp878[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp788); Cyc_PP_cat( _tag_arr( _temp878, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL631; _LL679: s=({ struct Cyc_PP_Doc* _temp879[ 3u]; _temp879[ 2u]=
Cyc_PP_textptr( _temp790); _temp879[ 1u]= Cyc_PP_text( _tag_arr("->", sizeof(
unsigned char), 3u)); _temp879[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp792);
Cyc_PP_cat( _tag_arr( _temp879, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL631; _LL681: s=({ struct Cyc_PP_Doc* _temp880[ 4u]; _temp880[ 3u]= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); _temp880[ 2u]= Cyc_Absynpp_exp2doc(
_temp794); _temp880[ 1u]= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u));
_temp880[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp796); Cyc_PP_cat( _tag_arr(
_temp880, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL631; _LL683: s=({ struct
Cyc_PP_Doc* _temp881[ 3u]; _temp881[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp881[ 1u]= Cyc_Absynpp_exps2doc_prec( 20, _temp798);
_temp881[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp881, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL631; _LL685: s=({
struct Cyc_PP_Doc* _temp882[ 4u]; _temp882[ 3u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp800)); _temp882[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
_temp882[ 1u]= Cyc_Absynpp_typ2doc((* _temp802).f3); _temp882[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp882,
sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL631; _LL687: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp804)); goto _LL631; _LL689: s=({ struct Cyc_PP_Doc*
_temp883[ 7u]; _temp883[ 6u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp883[ 5u]= Cyc_Absynpp_exp2doc( _temp806); _temp883[ 4u]= Cyc_PP_text(
_tag_arr(" : ", sizeof( unsigned char), 4u)); _temp883[ 3u]= Cyc_Absynpp_exp2doc(
_temp808); _temp883[ 2u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp883[ 1u]= Cyc_PP_text(*(* _temp810->name).f2); _temp883[ 0u]= Cyc_PP_text(
_tag_arr("{for ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp883,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL631; _LL691: s=({ struct Cyc_PP_Doc*
_temp884[ 2u]; _temp884[ 1u]= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char),
2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp812));
_temp884[ 0u]= Cyc_Absynpp_qvar2doc( _temp814); Cyc_PP_cat( _tag_arr( _temp884,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL631; _LL693: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp816)); goto _LL631; _LL695: if( _temp820 ==  0){ s= Cyc_Absynpp_qvar2doc(
_temp818->name);} else{ s=({ struct Cyc_PP_Doc* _temp885[ 2u]; _temp885[ 1u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp820)); _temp885[ 0u]= Cyc_Absynpp_qvar2doc( _temp818->name);
Cyc_PP_cat( _tag_arr( _temp885, sizeof( struct Cyc_PP_Doc*), 2u));});} goto
_LL631; _LL697: s= Cyc_Absynpp_qvar2doc( _temp822); goto _LL631; _LL699: s= Cyc_Absynpp_qvar2doc(
_temp824); goto _LL631; _LL701: if( _temp828 ==  0){ s=({ struct Cyc_PP_Doc*
_temp886[ 3u]; _temp886[ 2u]= Cyc_PP_text( _tag_arr("))", sizeof( unsigned char),
3u)); _temp886[ 1u]= Cyc_Absynpp_typ2doc( _temp826); _temp886[ 0u]= Cyc_PP_text(
_tag_arr("malloc(sizeof(", sizeof( unsigned char), 15u)); Cyc_PP_cat( _tag_arr(
_temp886, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ s=({ struct Cyc_PP_Doc*
_temp887[ 6u]; _temp887[ 5u]= Cyc_PP_text( _tag_arr("))", sizeof( unsigned char),
3u)); _temp887[ 4u]= Cyc_Absynpp_typ2doc( _temp826); _temp887[ 3u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); _temp887[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp887[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp828)); _temp887[ 0u]= Cyc_PP_text(
_tag_arr("rmalloc(", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp887, sizeof( struct Cyc_PP_Doc*), 6u));});} goto _LL631; _LL703: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp830)); goto _LL631; _LL705: s=({ struct Cyc_PP_Doc*
_temp888[ 3u]; _temp888[ 2u]= Cyc_PP_text( _tag_arr(" })", sizeof( unsigned char),
4u)); _temp888[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp834)); _temp888[
0u]= Cyc_PP_text( _tag_arr("({ ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp888, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL631; _LL707: s=({
struct Cyc_PP_Doc* _temp889[ 3u]; _temp889[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp889[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp890=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp890->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp891=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp891[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp892; _temp892.tag= Cyc_Absyn_Fn_d;
_temp892.f1= _temp836; _temp892;}); _temp891;})); _temp890->loc= e->loc;
_temp890;}))); _temp889[ 0u]= Cyc_PP_text( _tag_arr("codegen(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp889, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL631; _LL709: s=({ struct Cyc_PP_Doc* _temp893[ 3u]; _temp893[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp893[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_exp2doc( _temp838)); _temp893[ 0u]= Cyc_PP_text( _tag_arr("fill(",
sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp893, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL631; _LL631:;} if( inprec >=  myprec){ s=({ struct Cyc_PP_Doc*
_temp894[ 3u]; _temp894[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp894[ 1u]= s; _temp894[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp894, sizeof( struct Cyc_PP_Doc*),
3u));});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){
void* _temp895= d; struct Cyc_Absyn_Exp* _temp901; struct _tagged_arr* _temp903;
_LL897: if(*(( int*) _temp895) ==  Cyc_Absyn_ArrayElement){ _LL902: _temp901=((
struct Cyc_Absyn_ArrayElement_struct*) _temp895)->f1; goto _LL898;} else{ goto
_LL899;} _LL899: if(*(( int*) _temp895) ==  Cyc_Absyn_FieldName){ _LL904:
_temp903=(( struct Cyc_Absyn_FieldName_struct*) _temp895)->f1; goto _LL900;}
else{ goto _LL896;} _LL898: return({ struct Cyc_PP_Doc* _temp905[ 3u]; _temp905[
2u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char), 2u)); _temp905[ 1u]= Cyc_Absynpp_exp2doc(
_temp901); _temp905[ 0u]= Cyc_PP_text( _tag_arr(".[", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp905, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL900:
return({ struct Cyc_PP_Doc* _temp906[ 2u]; _temp906[ 1u]= Cyc_PP_textptr(
_temp903); _temp906[ 0u]= Cyc_PP_text( _tag_arr(".", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp906, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL896:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 ==  0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({ struct Cyc_PP_Doc*
_temp907[ 2u]; _temp907[ 1u]= Cyc_Absynpp_exp2doc((* de).f2); _temp907[ 0u]= Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char),
2u), _tag_arr("=", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); Cyc_PP_cat( _tag_arr( _temp907, sizeof( struct Cyc_PP_Doc*), 2u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp908= c;
unsigned char _temp926; void* _temp928; short _temp930; void* _temp932; int
_temp934; void* _temp936; int _temp938; void* _temp940; long long _temp942; void*
_temp944; struct _tagged_arr _temp946; struct _tagged_arr _temp948; _LL910: if((
unsigned int) _temp908 >  1u?*(( int*) _temp908) ==  Cyc_Absyn_Char_c: 0){
_LL929: _temp928=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp908)->f1; goto
_LL927; _LL927: _temp926=(( struct Cyc_Absyn_Char_c_struct*) _temp908)->f2; goto
_LL911;} else{ goto _LL912;} _LL912: if(( unsigned int) _temp908 >  1u?*(( int*)
_temp908) ==  Cyc_Absyn_Short_c: 0){ _LL933: _temp932=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp908)->f1; goto _LL931; _LL931: _temp930=(( struct Cyc_Absyn_Short_c_struct*)
_temp908)->f2; goto _LL913;} else{ goto _LL914;} _LL914: if(( unsigned int)
_temp908 >  1u?*(( int*) _temp908) ==  Cyc_Absyn_Int_c: 0){ _LL937: _temp936=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp908)->f1; if( _temp936 == ( void*)
Cyc_Absyn_Signed){ goto _LL935;} else{ goto _LL916;} _LL935: _temp934=(( struct
Cyc_Absyn_Int_c_struct*) _temp908)->f2; goto _LL915;} else{ goto _LL916;} _LL916:
if(( unsigned int) _temp908 >  1u?*(( int*) _temp908) ==  Cyc_Absyn_Int_c: 0){
_LL941: _temp940=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp908)->f1; if(
_temp940 == ( void*) Cyc_Absyn_Unsigned){ goto _LL939;} else{ goto _LL918;}
_LL939: _temp938=(( struct Cyc_Absyn_Int_c_struct*) _temp908)->f2; goto _LL917;}
else{ goto _LL918;} _LL918: if(( unsigned int) _temp908 >  1u?*(( int*) _temp908)
==  Cyc_Absyn_LongLong_c: 0){ _LL945: _temp944=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp908)->f1; goto _LL943; _LL943: _temp942=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp908)->f2; goto _LL919;} else{ goto _LL920;} _LL920: if(( unsigned int)
_temp908 >  1u?*(( int*) _temp908) ==  Cyc_Absyn_Float_c: 0){ _LL947: _temp946=((
struct Cyc_Absyn_Float_c_struct*) _temp908)->f1; goto _LL921;} else{ goto _LL922;}
_LL922: if( _temp908 == ( void*) Cyc_Absyn_Null_c){ goto _LL923;} else{ goto
_LL924;} _LL924: if(( unsigned int) _temp908 >  1u?*(( int*) _temp908) ==  Cyc_Absyn_String_c:
0){ _LL949: _temp948=(( struct Cyc_Absyn_String_c_struct*) _temp908)->f1; goto
_LL925;} else{ goto _LL909;} _LL911: return Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp951; _temp951.tag= Cyc_Std_String_pa;
_temp951.f1=( struct _tagged_arr) Cyc_Absynpp_char_escape( _temp926);{ void*
_temp950[ 1u]={& _temp951}; Cyc_Std_aprintf( _tag_arr("'%s'", sizeof(
unsigned char), 5u), _tag_arr( _temp950, sizeof( void*), 1u));}})); _LL913:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp953;
_temp953.tag= Cyc_Std_Int_pa; _temp953.f1=( int)(( unsigned int)(( int) _temp930));{
void* _temp952[ 1u]={& _temp953}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp952, sizeof( void*), 1u));}})); _LL915:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp955;
_temp955.tag= Cyc_Std_Int_pa; _temp955.f1=( int)(( unsigned int) _temp934);{
void* _temp954[ 1u]={& _temp955}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp954, sizeof( void*), 1u));}})); _LL917:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp957;
_temp957.tag= Cyc_Std_Int_pa; _temp957.f1=( unsigned int) _temp938;{ void*
_temp956[ 1u]={& _temp957}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp956, sizeof( void*), 1u));}})); _LL919:
return Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char),
27u)); _LL921: return Cyc_PP_text( _temp946); _LL923: return Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); _LL925: return({ struct Cyc_PP_Doc*
_temp958[ 3u]; _temp958[ 2u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char),
2u)); _temp958[ 1u]= Cyc_PP_text( Cyc_Absynpp_string_escape( _temp948));
_temp958[ 0u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp958, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL909:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == ( void*) Cyc_Absyn_Size){ if(
es ==  0? 1: es->tl !=  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp959=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp959[ 0]=({ struct Cyc_Core_Failure_struct _temp960; _temp960.tag= Cyc_Core_Failure;
_temp960.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp962;
_temp962.tag= Cyc_Std_String_pa; _temp962.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp961[ 1u]={& _temp962}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof( unsigned char), 44u), _tag_arr( _temp961, sizeof( void*), 1u));}});
_temp960;}); _temp959;}));}{ struct Cyc_PP_Doc* _temp963= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*) es->hd); return({ struct Cyc_PP_Doc* _temp964[ 2u];
_temp964[ 1u]= Cyc_PP_text( _tag_arr(".size", sizeof( unsigned char), 6u));
_temp964[ 0u]= _temp963; Cyc_PP_cat( _tag_arr( _temp964, sizeof( struct Cyc_PP_Doc*),
2u));});}} else{ struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds ==  0){( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp965=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp965[ 0]=({ struct Cyc_Core_Failure_struct
_temp966; _temp966.tag= Cyc_Core_Failure; _temp966.f1=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp968; _temp968.tag= Cyc_Std_String_pa;
_temp968.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp967[
1u]={& _temp968}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof( unsigned char), 38u), _tag_arr( _temp967, sizeof( void*), 1u));}});
_temp966;}); _temp965;}));} else{ if( ds->tl ==  0){ return({ struct Cyc_PP_Doc*
_temp969[ 3u]; _temp969[ 2u]=( struct Cyc_PP_Doc*) ds->hd; _temp969[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp969[ 0u]= ps; Cyc_PP_cat(
_tag_arr( _temp969, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ if((( struct
Cyc_List_List*) _check_null( ds->tl))->tl !=  0){( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp970=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp970[ 0]=({ struct Cyc_Core_Failure_struct
_temp971; _temp971.tag= Cyc_Core_Failure; _temp971.f1=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp973; _temp973.tag= Cyc_Std_String_pa;
_temp973.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp972[
1u]={& _temp973}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof( unsigned char), 47u), _tag_arr( _temp972, sizeof( void*), 1u));}});
_temp971;}); _temp970;}));} else{ return({ struct Cyc_PP_Doc* _temp974[ 5u];
_temp974[ 4u]=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds->tl))->hd;
_temp974[ 3u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); _temp974[
2u]= ps; _temp974[ 1u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp974[ 0u]=( struct Cyc_PP_Doc*) ds->hd; Cyc_PP_cat( _tag_arr( _temp974,
sizeof( struct Cyc_PP_Doc*), 5u));});}}}}} struct _tagged_arr Cyc_Absynpp_prim2str(
void* p){ void* _temp975= p; _LL977: if( _temp975 == ( void*) Cyc_Absyn_Plus){
goto _LL978;} else{ goto _LL979;} _LL979: if( _temp975 == ( void*) Cyc_Absyn_Times){
goto _LL980;} else{ goto _LL981;} _LL981: if( _temp975 == ( void*) Cyc_Absyn_Minus){
goto _LL982;} else{ goto _LL983;} _LL983: if( _temp975 == ( void*) Cyc_Absyn_Div){
goto _LL984;} else{ goto _LL985;} _LL985: if( _temp975 == ( void*) Cyc_Absyn_Mod){
goto _LL986;} else{ goto _LL987;} _LL987: if( _temp975 == ( void*) Cyc_Absyn_Eq){
goto _LL988;} else{ goto _LL989;} _LL989: if( _temp975 == ( void*) Cyc_Absyn_Neq){
goto _LL990;} else{ goto _LL991;} _LL991: if( _temp975 == ( void*) Cyc_Absyn_Gt){
goto _LL992;} else{ goto _LL993;} _LL993: if( _temp975 == ( void*) Cyc_Absyn_Lt){
goto _LL994;} else{ goto _LL995;} _LL995: if( _temp975 == ( void*) Cyc_Absyn_Gte){
goto _LL996;} else{ goto _LL997;} _LL997: if( _temp975 == ( void*) Cyc_Absyn_Lte){
goto _LL998;} else{ goto _LL999;} _LL999: if( _temp975 == ( void*) Cyc_Absyn_Not){
goto _LL1000;} else{ goto _LL1001;} _LL1001: if( _temp975 == ( void*) Cyc_Absyn_Bitnot){
goto _LL1002;} else{ goto _LL1003;} _LL1003: if( _temp975 == ( void*) Cyc_Absyn_Bitand){
goto _LL1004;} else{ goto _LL1005;} _LL1005: if( _temp975 == ( void*) Cyc_Absyn_Bitor){
goto _LL1006;} else{ goto _LL1007;} _LL1007: if( _temp975 == ( void*) Cyc_Absyn_Bitxor){
goto _LL1008;} else{ goto _LL1009;} _LL1009: if( _temp975 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL1010;} else{ goto _LL1011;} _LL1011: if( _temp975 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL1012;} else{ goto _LL1013;} _LL1013: if( _temp975 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL1014;} else{ goto _LL1015;} _LL1015: if( _temp975 == ( void*) Cyc_Absyn_Size){
goto _LL1016;} else{ goto _LL976;} _LL978: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL980: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL982: return _tag_arr("-", sizeof( unsigned char), 2u); _LL984: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL986: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL988: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL990: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL992: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL994: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL996: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL998: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL1000: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL1002: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL1004: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL1006: return _tag_arr("|", sizeof( unsigned char), 2u); _LL1008: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL1010: return _tag_arr("<<",
sizeof( unsigned char), 3u); _LL1012: return _tag_arr(">>", sizeof(
unsigned char), 3u); _LL1014: return _tag_arr(">>>", sizeof( unsigned char), 4u);
_LL1016: return _tag_arr("size", sizeof( unsigned char), 5u); _LL976:;} struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 == 
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*
_temp1017[ 3u]; _temp1017[ 2u]= Cyc_Absynpp_exp2doc((* t).f2); _temp1017[ 1u]=
Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1017[ 0u]= Cyc_PP_textptr((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v); Cyc_PP_cat(
_tag_arr( _temp1017, sizeof( struct Cyc_PP_Doc*), 3u));});}} int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt* s){ void* _temp1018=( void*) s->r; _LL1020: if((
unsigned int) _temp1018 >  1u?*(( int*) _temp1018) ==  Cyc_Absyn_Decl_s: 0){
goto _LL1021;} else{ goto _LL1022;} _LL1022: goto _LL1023; _LL1021: return 1;
_LL1023: return 0; _LL1019:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1024=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1068; struct Cyc_Absyn_Stmt* _temp1070; struct Cyc_Absyn_Stmt*
_temp1072; struct Cyc_Absyn_Exp* _temp1074; struct Cyc_Absyn_Stmt* _temp1076;
struct Cyc_Absyn_Stmt* _temp1078; struct Cyc_Absyn_Exp* _temp1080; struct Cyc_Absyn_Stmt*
_temp1082; struct _tuple2 _temp1084; struct Cyc_Absyn_Exp* _temp1086; struct
_tagged_arr* _temp1088; struct Cyc_Absyn_Stmt* _temp1090; struct _tuple2
_temp1092; struct Cyc_Absyn_Exp* _temp1094; struct _tuple2 _temp1096; struct Cyc_Absyn_Exp*
_temp1098; struct Cyc_Absyn_Exp* _temp1100; struct Cyc_List_List* _temp1102;
struct Cyc_Absyn_Exp* _temp1104; struct Cyc_List_List* _temp1106; struct Cyc_Absyn_Exp*
_temp1108; struct Cyc_List_List* _temp1110; struct Cyc_List_List* _temp1112;
struct Cyc_Absyn_Stmt* _temp1114; struct Cyc_Absyn_Decl* _temp1116; struct Cyc_Absyn_Stmt*
_temp1118; struct Cyc_Absyn_Stmt* _temp1120; struct Cyc_Absyn_Stmt* _temp1122;
struct _tagged_arr* _temp1124; struct _tuple2 _temp1126; struct Cyc_Absyn_Exp*
_temp1128; struct Cyc_Absyn_Stmt* _temp1130; struct Cyc_List_List* _temp1132;
struct Cyc_Absyn_Stmt* _temp1134; struct Cyc_Absyn_Stmt* _temp1136; struct Cyc_Absyn_Vardecl*
_temp1138; struct Cyc_Absyn_Tvar* _temp1140; _LL1026: if( _temp1024 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1027;} else{ goto _LL1028;} _LL1028: if((
unsigned int) _temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_Exp_s: 0){
_LL1069: _temp1068=(( struct Cyc_Absyn_Exp_s_struct*) _temp1024)->f1; goto
_LL1029;} else{ goto _LL1030;} _LL1030: if(( unsigned int) _temp1024 >  1u?*((
int*) _temp1024) ==  Cyc_Absyn_Seq_s: 0){ _LL1073: _temp1072=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1024)->f1; goto _LL1071; _LL1071: _temp1070=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1024)->f2; goto _LL1031;} else{ goto _LL1032;} _LL1032: if(( unsigned int)
_temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_Return_s: 0){ _LL1075:
_temp1074=(( struct Cyc_Absyn_Return_s_struct*) _temp1024)->f1; goto _LL1033;}
else{ goto _LL1034;} _LL1034: if(( unsigned int) _temp1024 >  1u?*(( int*)
_temp1024) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1081: _temp1080=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1024)->f1; goto _LL1079; _LL1079: _temp1078=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1024)->f2; goto _LL1077; _LL1077: _temp1076=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1024)->f3; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(( unsigned int)
_temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_While_s: 0){ _LL1085:
_temp1084=(( struct Cyc_Absyn_While_s_struct*) _temp1024)->f1; _LL1087:
_temp1086= _temp1084.f1; goto _LL1083; _LL1083: _temp1082=(( struct Cyc_Absyn_While_s_struct*)
_temp1024)->f2; goto _LL1037;} else{ goto _LL1038;} _LL1038: if(( unsigned int)
_temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_Break_s: 0){ goto _LL1039;}
else{ goto _LL1040;} _LL1040: if(( unsigned int) _temp1024 >  1u?*(( int*)
_temp1024) ==  Cyc_Absyn_Continue_s: 0){ goto _LL1041;} else{ goto _LL1042;}
_LL1042: if(( unsigned int) _temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_Goto_s:
0){ _LL1089: _temp1088=(( struct Cyc_Absyn_Goto_s_struct*) _temp1024)->f1; goto
_LL1043;} else{ goto _LL1044;} _LL1044: if(( unsigned int) _temp1024 >  1u?*((
int*) _temp1024) ==  Cyc_Absyn_For_s: 0){ _LL1101: _temp1100=(( struct Cyc_Absyn_For_s_struct*)
_temp1024)->f1; goto _LL1097; _LL1097: _temp1096=(( struct Cyc_Absyn_For_s_struct*)
_temp1024)->f2; _LL1099: _temp1098= _temp1096.f1; goto _LL1093; _LL1093:
_temp1092=(( struct Cyc_Absyn_For_s_struct*) _temp1024)->f3; _LL1095: _temp1094=
_temp1092.f1; goto _LL1091; _LL1091: _temp1090=(( struct Cyc_Absyn_For_s_struct*)
_temp1024)->f4; goto _LL1045;} else{ goto _LL1046;} _LL1046: if(( unsigned int)
_temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_Switch_s: 0){ _LL1105:
_temp1104=(( struct Cyc_Absyn_Switch_s_struct*) _temp1024)->f1; goto _LL1103;
_LL1103: _temp1102=(( struct Cyc_Absyn_Switch_s_struct*) _temp1024)->f2; goto
_LL1047;} else{ goto _LL1048;} _LL1048: if(( unsigned int) _temp1024 >  1u?*((
int*) _temp1024) ==  Cyc_Absyn_SwitchC_s: 0){ _LL1109: _temp1108=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1024)->f1; goto _LL1107; _LL1107: _temp1106=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1024)->f2; goto _LL1049;} else{ goto _LL1050;} _LL1050: if(( unsigned int)
_temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1111:
_temp1110=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1024)->f1; if( _temp1110
==  0){ goto _LL1051;} else{ goto _LL1052;}} else{ goto _LL1052;} _LL1052: if((
unsigned int) _temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_Fallthru_s: 0){
_LL1113: _temp1112=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1024)->f1; goto
_LL1053;} else{ goto _LL1054;} _LL1054: if(( unsigned int) _temp1024 >  1u?*((
int*) _temp1024) ==  Cyc_Absyn_Decl_s: 0){ _LL1117: _temp1116=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1024)->f1; goto _LL1115; _LL1115: _temp1114=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1024)->f2; goto _LL1055;} else{ goto _LL1056;} _LL1056: if(( unsigned int)
_temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_Cut_s: 0){ _LL1119: _temp1118=((
struct Cyc_Absyn_Cut_s_struct*) _temp1024)->f1; goto _LL1057;} else{ goto
_LL1058;} _LL1058: if(( unsigned int) _temp1024 >  1u?*(( int*) _temp1024) == 
Cyc_Absyn_Splice_s: 0){ _LL1121: _temp1120=(( struct Cyc_Absyn_Splice_s_struct*)
_temp1024)->f1; goto _LL1059;} else{ goto _LL1060;} _LL1060: if(( unsigned int)
_temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_Label_s: 0){ _LL1125:
_temp1124=(( struct Cyc_Absyn_Label_s_struct*) _temp1024)->f1; goto _LL1123;
_LL1123: _temp1122=(( struct Cyc_Absyn_Label_s_struct*) _temp1024)->f2; goto
_LL1061;} else{ goto _LL1062;} _LL1062: if(( unsigned int) _temp1024 >  1u?*((
int*) _temp1024) ==  Cyc_Absyn_Do_s: 0){ _LL1131: _temp1130=(( struct Cyc_Absyn_Do_s_struct*)
_temp1024)->f1; goto _LL1127; _LL1127: _temp1126=(( struct Cyc_Absyn_Do_s_struct*)
_temp1024)->f2; _LL1129: _temp1128= _temp1126.f1; goto _LL1063;} else{ goto
_LL1064;} _LL1064: if(( unsigned int) _temp1024 >  1u?*(( int*) _temp1024) == 
Cyc_Absyn_TryCatch_s: 0){ _LL1135: _temp1134=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1024)->f1; goto _LL1133; _LL1133: _temp1132=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1024)->f2; goto _LL1065;} else{ goto _LL1066;} _LL1066: if(( unsigned int)
_temp1024 >  1u?*(( int*) _temp1024) ==  Cyc_Absyn_Region_s: 0){ _LL1141:
_temp1140=(( struct Cyc_Absyn_Region_s_struct*) _temp1024)->f1; goto _LL1139;
_LL1139: _temp1138=(( struct Cyc_Absyn_Region_s_struct*) _temp1024)->f2; goto
_LL1137; _LL1137: _temp1136=(( struct Cyc_Absyn_Region_s_struct*) _temp1024)->f3;
goto _LL1067;} else{ goto _LL1025;} _LL1027: s= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); goto _LL1025; _LL1029: s=({ struct Cyc_PP_Doc*
_temp1142[ 2u]; _temp1142[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1142[ 0u]= Cyc_Absynpp_exp2doc( _temp1068); Cyc_PP_cat( _tag_arr(
_temp1142, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1025; _LL1031: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1072)){ s=({ struct Cyc_PP_Doc* _temp1143[
6u]; _temp1143[ 5u]= Cyc_Absynpp_is_declaration( _temp1070)?({ struct Cyc_PP_Doc*
_temp1144[ 4u]; _temp1144[ 3u]= Cyc_PP_line_doc(); _temp1144[ 2u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1144[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1070)); _temp1144[ 0u]= Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1144, sizeof( struct Cyc_PP_Doc*), 4u));}): Cyc_Absynpp_stmt2doc(
_temp1070); _temp1143[ 4u]= Cyc_PP_line_doc(); _temp1143[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1143[ 2u]= Cyc_PP_line_doc();
_temp1143[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1072)); _temp1143[ 0u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr(
_temp1143, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ if( Cyc_Absynpp_is_declaration(
_temp1070)){ s=({ struct Cyc_PP_Doc* _temp1145[ 6u]; _temp1145[ 5u]= Cyc_PP_line_doc();
_temp1145[ 4u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1145[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1070)); _temp1145[ 2u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); _temp1145[ 1u]= Cyc_PP_line_doc();
_temp1145[ 0u]= Cyc_Absynpp_stmt2doc( _temp1072); Cyc_PP_cat( _tag_arr(
_temp1145, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1146=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1146->hd=( void*) _temp1072;
_temp1146->tl=({ struct Cyc_List_List* _temp1147=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1147->hd=( void*) _temp1070;
_temp1147->tl= 0; _temp1147;}); _temp1146;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1148=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1148->hd=( void*) _temp1072;
_temp1148->tl=({ struct Cyc_List_List* _temp1149=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1149->hd=( void*) _temp1070;
_temp1149->tl= 0; _temp1149;}); _temp1148;}));} goto _LL1025; _LL1033: if(
_temp1074 ==  0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc* _temp1150[ 3u]; _temp1150[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1150[ 1u]= _temp1074 ==  0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1074)); _temp1150[
0u]= Cyc_PP_text( _tag_arr("return ", sizeof( unsigned char), 8u)); Cyc_PP_cat(
_tag_arr( _temp1150, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1025;
_LL1035: { int print_else;{ void* _temp1151=( void*) _temp1076->r; _LL1153: if(
_temp1151 == ( void*) Cyc_Absyn_Skip_s){ goto _LL1154;} else{ goto _LL1155;}
_LL1155: goto _LL1156; _LL1154: print_else= 0; goto _LL1152; _LL1156: print_else=
1; goto _LL1152; _LL1152:;} s=({ struct Cyc_PP_Doc* _temp1157[ 7u]; _temp1157[ 6u]=
print_else?({ struct Cyc_PP_Doc* _temp1159[ 5u]; _temp1159[ 4u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1159[ 3u]= Cyc_PP_line_doc();
_temp1159[ 2u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1160[ 2u]; _temp1160[
1u]= Cyc_Absynpp_stmt2doc( _temp1076); _temp1160[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1160, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1159[ 1u]= Cyc_PP_text(
_tag_arr("else {", sizeof( unsigned char), 7u)); _temp1159[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1159, sizeof( struct Cyc_PP_Doc*), 5u));}): Cyc_PP_nil_doc();
_temp1157[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1157[ 4u]= Cyc_PP_line_doc(); _temp1157[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1158[ 2u]; _temp1158[ 1u]= Cyc_Absynpp_stmt2doc( _temp1078); _temp1158[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1158, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1157[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1157[ 1u]= Cyc_Absynpp_exp2doc( _temp1080); _temp1157[ 0u]= Cyc_PP_text(
_tag_arr("if (", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1157,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1025;} _LL1037: s=({ struct Cyc_PP_Doc*
_temp1161[ 6u]; _temp1161[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1161[ 4u]= Cyc_PP_line_doc(); _temp1161[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1162[ 2u]; _temp1162[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1082); _temp1162[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1162,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1161[ 2u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1161[ 1u]= Cyc_Absynpp_exp2doc( _temp1086);
_temp1161[ 0u]= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp1161, sizeof( struct Cyc_PP_Doc*), 6u));}); goto
_LL1025; _LL1039: s= Cyc_PP_text( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL1025; _LL1041: s= Cyc_PP_text( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL1025; _LL1043: s= Cyc_PP_text(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp1164; _temp1164.tag= Cyc_Std_String_pa;
_temp1164.f1=( struct _tagged_arr)* _temp1088;{ void* _temp1163[ 1u]={&
_temp1164}; Cyc_Std_aprintf( _tag_arr("goto %s;", sizeof( unsigned char), 9u),
_tag_arr( _temp1163, sizeof( void*), 1u));}})); goto _LL1025; _LL1045: s=({
struct Cyc_PP_Doc* _temp1165[ 10u]; _temp1165[ 9u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1165[ 8u]= Cyc_PP_line_doc(); _temp1165[ 7u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1166[ 2u]; _temp1166[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1090); _temp1166[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1166,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1165[ 6u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1165[ 5u]= Cyc_Absynpp_exp2doc( _temp1094);
_temp1165[ 4u]= Cyc_PP_text( _tag_arr("; ", sizeof( unsigned char), 3u));
_temp1165[ 3u]= Cyc_Absynpp_exp2doc( _temp1098); _temp1165[ 2u]= Cyc_PP_text(
_tag_arr("; ", sizeof( unsigned char), 3u)); _temp1165[ 1u]= Cyc_Absynpp_exp2doc(
_temp1100); _temp1165[ 0u]= Cyc_PP_text( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1165, sizeof( struct Cyc_PP_Doc*), 10u));});
goto _LL1025; _LL1047: s=({ struct Cyc_PP_Doc* _temp1167[ 7u]; _temp1167[ 6u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1167[ 5u]= Cyc_PP_line_doc();
_temp1167[ 4u]= Cyc_Absynpp_switchclauses2doc( _temp1102); _temp1167[ 3u]= Cyc_PP_line_doc();
_temp1167[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1167[ 1u]= Cyc_Absynpp_exp2doc( _temp1104); _temp1167[ 0u]= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp1167, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1025; _LL1049: s=({
struct Cyc_PP_Doc* _temp1168[ 7u]; _temp1168[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1168[ 5u]= Cyc_PP_line_doc(); _temp1168[ 4u]=
Cyc_Absynpp_switchCclauses2doc( _temp1106); _temp1168[ 3u]= Cyc_PP_line_doc();
_temp1168[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1168[ 1u]= Cyc_Absynpp_exp2doc( _temp1108); _temp1168[ 0u]= Cyc_PP_text(
_tag_arr("switch \"C\" (", sizeof( unsigned char), 13u)); Cyc_PP_cat( _tag_arr(
_temp1168, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1025; _LL1051: s= Cyc_PP_text(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL1025; _LL1053: s=({
struct Cyc_PP_Doc* _temp1169[ 3u]; _temp1169[ 2u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1169[ 1u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp1112); _temp1169[ 0u]= Cyc_PP_text( _tag_arr("fallthru(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1169, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL1025; _LL1055: s=({ struct Cyc_PP_Doc* _temp1170[ 3u];
_temp1170[ 2u]= Cyc_Absynpp_stmt2doc( _temp1114); _temp1170[ 1u]= Cyc_PP_line_doc();
_temp1170[ 0u]= Cyc_Absynpp_decl2doc( _temp1116); Cyc_PP_cat( _tag_arr(
_temp1170, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1025; _LL1057: s=({
struct Cyc_PP_Doc* _temp1171[ 2u]; _temp1171[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1118)); _temp1171[ 0u]= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1171, sizeof( struct Cyc_PP_Doc*), 2u));});
goto _LL1025; _LL1059: s=({ struct Cyc_PP_Doc* _temp1172[ 2u]; _temp1172[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1120)); _temp1172[ 0u]= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr(
_temp1172, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1025; _LL1061: if( Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration( _temp1122): 0){ s=({ struct Cyc_PP_Doc* _temp1173[ 6u];
_temp1173[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1173[ 4u]= Cyc_PP_line_doc(); _temp1173[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1122)); _temp1173[ 2u]= Cyc_PP_line_doc(); _temp1173[ 1u]= Cyc_PP_text(
_tag_arr(": {", sizeof( unsigned char), 4u)); _temp1173[ 0u]= Cyc_PP_textptr(
_temp1124); Cyc_PP_cat( _tag_arr( _temp1173, sizeof( struct Cyc_PP_Doc*), 6u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1174[ 3u]; _temp1174[ 2u]= Cyc_Absynpp_stmt2doc(
_temp1122); _temp1174[ 1u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); _temp1174[ 0u]= Cyc_PP_textptr( _temp1124); Cyc_PP_cat( _tag_arr(
_temp1174, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1025; _LL1063: s=({
struct Cyc_PP_Doc* _temp1175[ 7u]; _temp1175[ 6u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1175[ 5u]= Cyc_Absynpp_exp2doc( _temp1128);
_temp1175[ 4u]= Cyc_PP_text( _tag_arr("} while (", sizeof( unsigned char), 10u));
_temp1175[ 3u]= Cyc_PP_line_doc(); _temp1175[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1130)); _temp1175[ 1u]= Cyc_PP_line_doc(); _temp1175[ 0u]= Cyc_PP_text(
_tag_arr("do {", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1175,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1025; _LL1065: s=({ struct Cyc_PP_Doc*
_temp1176[ 9u]; _temp1176[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1176[ 7u]= Cyc_PP_line_doc(); _temp1176[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1132)); _temp1176[ 5u]= Cyc_PP_line_doc(); _temp1176[ 4u]= Cyc_PP_text(
_tag_arr("} catch {", sizeof( unsigned char), 10u)); _temp1176[ 3u]= Cyc_PP_line_doc();
_temp1176[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1134)); _temp1176[ 1u]=
Cyc_PP_line_doc(); _temp1176[ 0u]= Cyc_PP_text( _tag_arr("try {", sizeof(
unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1176, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL1025; _LL1067: s=({ struct Cyc_PP_Doc* _temp1177[ 9u];
_temp1177[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1177[ 7u]= Cyc_PP_line_doc(); _temp1177[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1136)); _temp1177[ 5u]= Cyc_PP_line_doc(); _temp1177[ 4u]= Cyc_PP_text(
_tag_arr("{", sizeof( unsigned char), 2u)); _temp1177[ 3u]= Cyc_Absynpp_qvar2doc(
_temp1138->name); _temp1177[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); _temp1177[ 1u]= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1140)); _temp1177[ 0u]= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp1177, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL1025; _LL1025:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1178=( void*) p->r;
int _temp1212; void* _temp1214; unsigned char _temp1216; struct _tagged_arr
_temp1218; struct Cyc_Absyn_Vardecl* _temp1220; struct Cyc_List_List* _temp1222;
struct Cyc_Absyn_Pat* _temp1224; struct Cyc_Absyn_Vardecl* _temp1226; struct
_tuple0* _temp1228; struct Cyc_List_List* _temp1230; struct Cyc_List_List*
_temp1232; struct _tuple0* _temp1234; struct Cyc_List_List* _temp1236; struct
Cyc_List_List* _temp1238; struct _tuple0* _temp1240; struct Cyc_List_List*
_temp1242; struct Cyc_List_List* _temp1244; struct Cyc_Absyn_Structdecl*
_temp1246; struct Cyc_Absyn_Enumfield* _temp1248; struct Cyc_Absyn_Enumfield*
_temp1250; struct Cyc_List_List* _temp1252; struct Cyc_List_List* _temp1254;
struct Cyc_Absyn_Tunionfield* _temp1256; _LL1180: if( _temp1178 == ( void*) Cyc_Absyn_Wild_p){
goto _LL1181;} else{ goto _LL1182;} _LL1182: if( _temp1178 == ( void*) Cyc_Absyn_Null_p){
goto _LL1183;} else{ goto _LL1184;} _LL1184: if(( unsigned int) _temp1178 >  2u?*((
int*) _temp1178) ==  Cyc_Absyn_Int_p: 0){ _LL1215: _temp1214=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1178)->f1; goto _LL1213; _LL1213: _temp1212=((
struct Cyc_Absyn_Int_p_struct*) _temp1178)->f2; goto _LL1185;} else{ goto
_LL1186;} _LL1186: if(( unsigned int) _temp1178 >  2u?*(( int*) _temp1178) == 
Cyc_Absyn_Char_p: 0){ _LL1217: _temp1216=(( struct Cyc_Absyn_Char_p_struct*)
_temp1178)->f1; goto _LL1187;} else{ goto _LL1188;} _LL1188: if(( unsigned int)
_temp1178 >  2u?*(( int*) _temp1178) ==  Cyc_Absyn_Float_p: 0){ _LL1219:
_temp1218=(( struct Cyc_Absyn_Float_p_struct*) _temp1178)->f1; goto _LL1189;}
else{ goto _LL1190;} _LL1190: if(( unsigned int) _temp1178 >  2u?*(( int*)
_temp1178) ==  Cyc_Absyn_Var_p: 0){ _LL1221: _temp1220=(( struct Cyc_Absyn_Var_p_struct*)
_temp1178)->f1; goto _LL1191;} else{ goto _LL1192;} _LL1192: if(( unsigned int)
_temp1178 >  2u?*(( int*) _temp1178) ==  Cyc_Absyn_Tuple_p: 0){ _LL1223:
_temp1222=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1178)->f1; goto _LL1193;}
else{ goto _LL1194;} _LL1194: if(( unsigned int) _temp1178 >  2u?*(( int*)
_temp1178) ==  Cyc_Absyn_Pointer_p: 0){ _LL1225: _temp1224=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1178)->f1; goto _LL1195;} else{ goto _LL1196;} _LL1196: if(( unsigned int)
_temp1178 >  2u?*(( int*) _temp1178) ==  Cyc_Absyn_Reference_p: 0){ _LL1227:
_temp1226=(( struct Cyc_Absyn_Reference_p_struct*) _temp1178)->f1; goto _LL1197;}
else{ goto _LL1198;} _LL1198: if(( unsigned int) _temp1178 >  2u?*(( int*)
_temp1178) ==  Cyc_Absyn_UnknownId_p: 0){ _LL1229: _temp1228=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1178)->f1; goto _LL1199;} else{ goto _LL1200;} _LL1200: if(( unsigned int)
_temp1178 >  2u?*(( int*) _temp1178) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL1235:
_temp1234=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1178)->f1; goto
_LL1233; _LL1233: _temp1232=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1178)->f2;
goto _LL1231; _LL1231: _temp1230=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1178)->f3; goto _LL1201;} else{ goto _LL1202;} _LL1202: if(( unsigned int)
_temp1178 >  2u?*(( int*) _temp1178) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL1241:
_temp1240=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1178)->f1; goto
_LL1239; _LL1239: _temp1238=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1178)->f2; goto _LL1237; _LL1237: _temp1236=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1178)->f3; goto _LL1203;} else{ goto _LL1204;} _LL1204: if(( unsigned int)
_temp1178 >  2u?*(( int*) _temp1178) ==  Cyc_Absyn_Struct_p: 0){ _LL1247:
_temp1246=(( struct Cyc_Absyn_Struct_p_struct*) _temp1178)->f1; goto _LL1245;
_LL1245: _temp1244=(( struct Cyc_Absyn_Struct_p_struct*) _temp1178)->f3; goto
_LL1243; _LL1243: _temp1242=(( struct Cyc_Absyn_Struct_p_struct*) _temp1178)->f4;
goto _LL1205;} else{ goto _LL1206;} _LL1206: if(( unsigned int) _temp1178 >  2u?*((
int*) _temp1178) ==  Cyc_Absyn_Enum_p: 0){ _LL1249: _temp1248=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1178)->f2; goto _LL1207;} else{ goto _LL1208;} _LL1208: if(( unsigned int)
_temp1178 >  2u?*(( int*) _temp1178) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL1251:
_temp1250=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1178)->f2; goto _LL1209;}
else{ goto _LL1210;} _LL1210: if(( unsigned int) _temp1178 >  2u?*(( int*)
_temp1178) ==  Cyc_Absyn_Tunion_p: 0){ _LL1257: _temp1256=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1178)->f2; goto _LL1255; _LL1255: _temp1254=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1178)->f3; goto _LL1253; _LL1253: _temp1252=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1178)->f4; goto _LL1211;} else{ goto _LL1179;} _LL1181: s= Cyc_PP_text(
_tag_arr("_", sizeof( unsigned char), 2u)); goto _LL1179; _LL1183: s= Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); goto _LL1179; _LL1185: if(
_temp1214 == ( void*) Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp1259; _temp1259.tag= Cyc_Std_Int_pa; _temp1259.f1=(
int)(( unsigned int) _temp1212);{ void* _temp1258[ 1u]={& _temp1259}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp1258, sizeof( void*),
1u));}}));} else{ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp1261; _temp1261.tag= Cyc_Std_Int_pa; _temp1261.f1=( unsigned int) _temp1212;{
void* _temp1260[ 1u]={& _temp1261}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp1260, sizeof( void*), 1u));}}));} goto
_LL1179; _LL1187: s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1263; _temp1263.tag= Cyc_Std_String_pa; _temp1263.f1=( struct _tagged_arr)
Cyc_Absynpp_char_escape( _temp1216);{ void* _temp1262[ 1u]={& _temp1263}; Cyc_Std_aprintf(
_tag_arr("'%s'", sizeof( unsigned char), 5u), _tag_arr( _temp1262, sizeof( void*),
1u));}})); goto _LL1179; _LL1189: s= Cyc_PP_text( _temp1218); goto _LL1179;
_LL1191: s= Cyc_Absynpp_qvar2doc( _temp1220->name); goto _LL1179; _LL1193: s=({
struct Cyc_PP_Doc* _temp1264[ 3u]; _temp1264[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp1264[ 1u]=(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc, _tag_arr(",", sizeof( unsigned char), 2u),
_temp1222); _temp1264[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1264, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1179; _LL1195: s=({ struct Cyc_PP_Doc* _temp1265[ 2u]; _temp1265[ 1u]=
Cyc_Absynpp_pat2doc( _temp1224); _temp1265[ 0u]= Cyc_PP_text( _tag_arr("&",
sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1265, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL1179; _LL1197: s=({ struct Cyc_PP_Doc* _temp1266[
2u]; _temp1266[ 1u]= Cyc_Absynpp_qvar2doc( _temp1226->name); _temp1266[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1266,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1179; _LL1199: s= Cyc_Absynpp_qvar2doc(
_temp1228); goto _LL1179; _LL1201: s=({ struct Cyc_PP_Doc* _temp1267[ 3u];
_temp1267[ 2u]= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1230));
_temp1267[ 1u]= Cyc_Absynpp_tvars2doc( _temp1232); _temp1267[ 0u]= Cyc_Absynpp_qvar2doc(
_temp1234); Cyc_PP_cat( _tag_arr( _temp1267, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1179; _LL1203: s=({ struct Cyc_PP_Doc* _temp1268[ 3u]; _temp1268[ 2u]=
Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1236)); _temp1268[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1238); _temp1268[ 0u]= Cyc_Absynpp_qvar2doc( _temp1240); Cyc_PP_cat(
_tag_arr( _temp1268, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1179; _LL1205:
s=({ struct Cyc_PP_Doc* _temp1269[ 3u]; _temp1269[ 2u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1242)); _temp1269[ 1u]= Cyc_Absynpp_tvars2doc( _temp1244); _temp1269[ 0u]=
_temp1246->name ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1246->name))->v); Cyc_PP_cat( _tag_arr(
_temp1269, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1179; _LL1207: s= Cyc_Absynpp_qvar2doc(
_temp1248->name); goto _LL1179; _LL1209: s= Cyc_Absynpp_qvar2doc( _temp1250->name);
goto _LL1179; _LL1211: if( _temp1252 ==  0){ s= Cyc_Absynpp_qvar2doc( _temp1256->name);}
else{ s=({ struct Cyc_PP_Doc* _temp1270[ 3u]; _temp1270[ 2u]= Cyc_PP_egroup(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1252)); _temp1270[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1254); _temp1270[ 0u]= Cyc_Absynpp_qvar2doc( _temp1256->name); Cyc_PP_cat(
_tag_arr( _temp1270, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1179;
_LL1179:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc* _temp1271[ 2u]; _temp1271[ 1u]= Cyc_Absynpp_pat2doc((*
dp).f2); _temp1271[ 0u]= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)); Cyc_PP_cat( _tag_arr(
_temp1271, sizeof( struct Cyc_PP_Doc*), 2u));});} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause ==  0?( void*)( c->pattern)->r
== ( void*) Cyc_Absyn_Wild_p: 0){ return({ struct Cyc_PP_Doc* _temp1272[ 2u];
_temp1272[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1273[ 2u]; _temp1273[
1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1273[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1273, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1272[ 0u]= Cyc_PP_text(
_tag_arr("default: ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1272, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ if( c->where_clause == 
0){ return({ struct Cyc_PP_Doc* _temp1274[ 4u]; _temp1274[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1275[ 2u]; _temp1275[ 1u]= Cyc_Absynpp_stmt2doc( c->body);
_temp1275[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1275, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1274[ 2u]= Cyc_PP_text( _tag_arr(": ",
sizeof( unsigned char), 3u)); _temp1274[ 1u]= Cyc_Absynpp_pat2doc( c->pattern);
_temp1274[ 0u]= Cyc_PP_text( _tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp1274, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ return({
struct Cyc_PP_Doc* _temp1276[ 6u]; _temp1276[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1277[ 2u]; _temp1277[ 1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1277[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1277, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1276[ 4u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u));
_temp1276[ 3u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( c->where_clause));
_temp1276[ 2u]= Cyc_PP_text( _tag_arr(" && ", sizeof( unsigned char), 5u));
_temp1276[ 1u]= Cyc_Absynpp_pat2doc( c->pattern); _temp1276[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1276,
sizeof( struct Cyc_PP_Doc*), 6u));});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp1280;
struct Cyc_Absyn_Stmt* _temp1281; struct Cyc_Absyn_Exp* _temp1283; struct Cyc_Absyn_SwitchC_clause*
_temp1278= c; _temp1280=* _temp1278; _LL1284: _temp1283= _temp1280.cnst_exp;
goto _LL1282; _LL1282: _temp1281= _temp1280.body; goto _LL1279; _LL1279: if(
_temp1283 ==  0){ return({ struct Cyc_PP_Doc* _temp1285[ 2u]; _temp1285[ 1u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1286[ 2u]; _temp1286[ 1u]= Cyc_Absynpp_stmt2doc(
c->body); _temp1286[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1286,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1285[ 0u]= Cyc_PP_text( _tag_arr("default: ",
sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1285, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp1287[ 4u];
_temp1287[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1288[ 2u]; _temp1288[
1u]= Cyc_Absynpp_stmt2doc( _temp1281); _temp1288[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1288, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1287[ 2u]= Cyc_PP_text(
_tag_arr(": ", sizeof( unsigned char), 3u)); _temp1287[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1283)); _temp1287[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1287,
sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_SwitchC_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct
Cyc_Absyn_Enumfield* f){ if( f->tag ==  0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc* _temp1289[ 3u]; _temp1289[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->tag)); _temp1289[ 1u]= Cyc_PP_text(
_tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1289[ 0u]= Cyc_Absynpp_qvar2doc(
f->name); Cyc_PP_cat( _tag_arr( _temp1289, sizeof( struct Cyc_PP_Doc*), 3u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,
_tag_arr(",", sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp1290=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp1316; struct Cyc_Absyn_Structdecl* _temp1318;
struct Cyc_Absyn_Uniondecl* _temp1320; struct Cyc_Absyn_Vardecl* _temp1322;
struct Cyc_Absyn_Vardecl _temp1324; struct Cyc_List_List* _temp1325; struct Cyc_Absyn_Exp*
_temp1327; void* _temp1329; struct Cyc_Absyn_Tqual _temp1331; struct _tuple0*
_temp1333; void* _temp1335; struct Cyc_Absyn_Tuniondecl* _temp1337; struct Cyc_Absyn_Tuniondecl
_temp1339; int _temp1340; struct Cyc_Core_Opt* _temp1342; struct Cyc_List_List*
_temp1344; struct _tuple0* _temp1346; void* _temp1348; struct Cyc_Absyn_Exp*
_temp1350; struct Cyc_Absyn_Pat* _temp1352; struct Cyc_List_List* _temp1354;
struct Cyc_Absyn_Enumdecl* _temp1356; struct Cyc_Absyn_Enumdecl _temp1358;
struct Cyc_Core_Opt* _temp1359; struct _tuple0* _temp1361; void* _temp1363;
struct Cyc_Absyn_Typedefdecl* _temp1365; struct Cyc_List_List* _temp1367; struct
_tagged_arr* _temp1369; struct Cyc_List_List* _temp1371; struct _tuple0*
_temp1373; struct Cyc_List_List* _temp1375; _LL1292: if(*(( int*) _temp1290) == 
Cyc_Absyn_Fn_d){ _LL1317: _temp1316=(( struct Cyc_Absyn_Fn_d_struct*) _temp1290)->f1;
goto _LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*) _temp1290) ==  Cyc_Absyn_Struct_d){
_LL1319: _temp1318=(( struct Cyc_Absyn_Struct_d_struct*) _temp1290)->f1; goto
_LL1295;} else{ goto _LL1296;} _LL1296: if(*(( int*) _temp1290) ==  Cyc_Absyn_Union_d){
_LL1321: _temp1320=(( struct Cyc_Absyn_Union_d_struct*) _temp1290)->f1; goto
_LL1297;} else{ goto _LL1298;} _LL1298: if(*(( int*) _temp1290) ==  Cyc_Absyn_Var_d){
_LL1323: _temp1322=(( struct Cyc_Absyn_Var_d_struct*) _temp1290)->f1; _temp1324=*
_temp1322; _LL1336: _temp1335=( void*) _temp1324.sc; goto _LL1334; _LL1334:
_temp1333= _temp1324.name; goto _LL1332; _LL1332: _temp1331= _temp1324.tq; goto
_LL1330; _LL1330: _temp1329=( void*) _temp1324.type; goto _LL1328; _LL1328:
_temp1327= _temp1324.initializer; goto _LL1326; _LL1326: _temp1325= _temp1324.attributes;
goto _LL1299;} else{ goto _LL1300;} _LL1300: if(*(( int*) _temp1290) ==  Cyc_Absyn_Tunion_d){
_LL1338: _temp1337=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1290)->f1;
_temp1339=* _temp1337; _LL1349: _temp1348=( void*) _temp1339.sc; goto _LL1347;
_LL1347: _temp1346= _temp1339.name; goto _LL1345; _LL1345: _temp1344= _temp1339.tvs;
goto _LL1343; _LL1343: _temp1342= _temp1339.fields; goto _LL1341; _LL1341:
_temp1340= _temp1339.is_xtunion; goto _LL1301;} else{ goto _LL1302;} _LL1302:
if(*(( int*) _temp1290) ==  Cyc_Absyn_Let_d){ _LL1353: _temp1352=(( struct Cyc_Absyn_Let_d_struct*)
_temp1290)->f1; goto _LL1351; _LL1351: _temp1350=(( struct Cyc_Absyn_Let_d_struct*)
_temp1290)->f4; goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*)
_temp1290) ==  Cyc_Absyn_Letv_d){ _LL1355: _temp1354=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1290)->f1; goto _LL1305;} else{ goto _LL1306;} _LL1306: if(*(( int*)
_temp1290) ==  Cyc_Absyn_Enum_d){ _LL1357: _temp1356=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1290)->f1; _temp1358=* _temp1356; _LL1364: _temp1363=( void*) _temp1358.sc;
goto _LL1362; _LL1362: _temp1361= _temp1358.name; goto _LL1360; _LL1360:
_temp1359= _temp1358.fields; goto _LL1307;} else{ goto _LL1308;} _LL1308: if(*((
int*) _temp1290) ==  Cyc_Absyn_Typedef_d){ _LL1366: _temp1365=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1290)->f1; goto _LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*)
_temp1290) ==  Cyc_Absyn_Namespace_d){ _LL1370: _temp1369=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1290)->f1; goto _LL1368; _LL1368: _temp1367=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1290)->f2; goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*)
_temp1290) ==  Cyc_Absyn_Using_d){ _LL1374: _temp1373=(( struct Cyc_Absyn_Using_d_struct*)
_temp1290)->f1; goto _LL1372; _LL1372: _temp1371=(( struct Cyc_Absyn_Using_d_struct*)
_temp1290)->f2; goto _LL1313;} else{ goto _LL1314;} _LL1314: if(*(( int*)
_temp1290) ==  Cyc_Absyn_ExternC_d){ _LL1376: _temp1375=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1290)->f1; goto _LL1315;} else{ goto _LL1291;} _LL1293: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp1384=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1384[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1385; _temp1385.tag= Cyc_Absyn_FnType; _temp1385.f1=({ struct Cyc_Absyn_FnInfo
_temp1386; _temp1386.tvars= _temp1316->tvs; _temp1386.effect= _temp1316->effect;
_temp1386.ret_typ=( void*)(( void*) _temp1316->ret_type); _temp1386.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1316->args); _temp1386.c_varargs=
_temp1316->c_varargs; _temp1386.cyc_varargs= _temp1316->cyc_varargs; _temp1386.rgn_po=
_temp1316->rgn_po; _temp1386.attributes= 0; _temp1386;}); _temp1385;});
_temp1384;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1316->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp1316->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp1316->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp1316->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp1316->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1381; _temp1381.q_const= 0; _temp1381.q_volatile= 0; _temp1381.q_restrict=
0; _temp1381;}), t,({ struct Cyc_Core_Opt* _temp1382=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1382->v=( void*)({ struct Cyc_PP_Doc*
_temp1383[ 2u]; _temp1383[ 1u]= namedoc; _temp1383[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1383, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1382;})); struct
Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc* _temp1379[ 4u]; _temp1379[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1379[ 2u]= Cyc_PP_line_doc();
_temp1379[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1380[ 2u]; _temp1380[
1u]= Cyc_Absynpp_stmt2doc( _temp1316->body); _temp1380[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1380, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1379[ 0u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp1379, sizeof( struct Cyc_PP_Doc*), 4u));}); s=({ struct Cyc_PP_Doc*
_temp1377[ 4u]; _temp1377[ 3u]= bodydoc; _temp1377[ 2u]= tqtddoc; _temp1377[ 1u]=
scopedoc; _temp1377[ 0u]= inlinedoc; Cyc_PP_cat( _tag_arr( _temp1377, sizeof(
struct Cyc_PP_Doc*), 4u));}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*
_temp1378[ 2u]; _temp1378[ 1u]= s; _temp1378[ 0u]= attsdoc; Cyc_PP_cat( _tag_arr(
_temp1378, sizeof( struct Cyc_PP_Doc*), 2u));});} goto _LL1291;}} _LL1295: if(
_temp1318->fields ==  0){ s=({ struct Cyc_PP_Doc* _temp1387[ 5u]; _temp1387[ 4u]=
Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp1387[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1318->tvs); _temp1387[ 2u]= _temp1318->name ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1318->name))->v); _temp1387[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1387[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1318->sc); Cyc_PP_cat( _tag_arr( _temp1387, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ s=({ struct Cyc_PP_Doc* _temp1388[ 10u]; _temp1388[ 9u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1388[ 8u]= Cyc_Absynpp_atts2doc(
_temp1318->attributes); _temp1388[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); _temp1388[ 6u]= Cyc_PP_line_doc(); _temp1388[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1389[ 2u]; _temp1389[ 1u]= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1318->fields))->v);
_temp1389[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1389, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1388[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1388[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1318->tvs);
_temp1388[ 2u]= _temp1318->name ==  0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1318->name))->v); _temp1388[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1388[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1318->sc); Cyc_PP_cat( _tag_arr( _temp1388, sizeof( struct Cyc_PP_Doc*),
10u));});} goto _LL1291; _LL1297: if( _temp1320->fields ==  0){ s=({ struct Cyc_PP_Doc*
_temp1390[ 5u]; _temp1390[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1390[ 3u]= Cyc_Absynpp_tvars2doc( _temp1320->tvs); _temp1390[ 2u]=
_temp1320->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1320->name))->v); _temp1390[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1390[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1320->sc);
Cyc_PP_cat( _tag_arr( _temp1390, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({
struct Cyc_PP_Doc* _temp1391[ 10u]; _temp1391[ 9u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1391[ 8u]= Cyc_Absynpp_atts2doc( _temp1320->attributes);
_temp1391[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1391[ 6u]= Cyc_PP_line_doc(); _temp1391[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1392[ 2u]; _temp1392[ 1u]= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1320->fields))->v); _temp1392[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1392, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1391[ 4u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1391[ 3u]= Cyc_Absynpp_tvars2doc( _temp1320->tvs); _temp1391[ 2u]=
_temp1320->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1320->name))->v); _temp1391[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1391[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1320->sc);
Cyc_PP_cat( _tag_arr( _temp1391, sizeof( struct Cyc_PP_Doc*), 10u));});} goto
_LL1291; _LL1299: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp1333); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1325); struct
Cyc_PP_Doc* beforenamedoc; if( ! Cyc_Absynpp_to_VC){ beforenamedoc= attsdoc;}
else{ void* _temp1393= Cyc_Tcutil_compress( _temp1329); struct Cyc_Absyn_FnInfo
_temp1399; struct Cyc_List_List* _temp1401; _LL1395: if(( unsigned int)
_temp1393 >  3u?*(( int*) _temp1393) ==  Cyc_Absyn_FnType: 0){ _LL1400:
_temp1399=(( struct Cyc_Absyn_FnType_struct*) _temp1393)->f1; _LL1402: _temp1401=
_temp1399.attributes; goto _LL1396;} else{ goto _LL1397;} _LL1397: goto _LL1398;
_LL1396: beforenamedoc= Cyc_Absynpp_callconv2doc( _temp1401); goto _LL1394;
_LL1398: beforenamedoc= Cyc_PP_nil_doc(); goto _LL1394; _LL1394:;} s=({ struct
Cyc_PP_Doc* _temp1403[ 5u]; _temp1403[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); _temp1403[ 3u]= _temp1327 ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)):({ struct Cyc_PP_Doc* _temp1406[ 2u]; _temp1406[ 1u]=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1327)); _temp1406[
0u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp1406, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1403[ 2u]= Cyc_Absynpp_tqtd2doc(
_temp1331, _temp1329,({ struct Cyc_Core_Opt* _temp1404=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1404->v=( void*)({ struct Cyc_PP_Doc*
_temp1405[ 2u]; _temp1405[ 1u]= sn; _temp1405[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1405, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1404;}));
_temp1403[ 1u]= Cyc_Absynpp_scope2doc( _temp1335); _temp1403[ 0u]= Cyc_Absynpp_to_VC?
attsdoc: Cyc_PP_nil_doc(); Cyc_PP_cat( _tag_arr( _temp1403, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL1291;} _LL1301: if( _temp1342 ==  0){ s=({ struct Cyc_PP_Doc*
_temp1407[ 5u]; _temp1407[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1407[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1344); _temp1407[ 2u]=
_temp1340? Cyc_Absynpp_qvar2doc( _temp1346): Cyc_Absynpp_typedef_name2doc(
_temp1346); _temp1407[ 1u]= _temp1340? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1407[ 0u]= Cyc_Absynpp_scope2doc( _temp1348); Cyc_PP_cat( _tag_arr(
_temp1407, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1408[ 8u]; _temp1408[ 7u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1408[ 6u]= Cyc_PP_line_doc(); _temp1408[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1409[ 2u]; _temp1409[ 1u]= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*) _temp1342->v); _temp1409[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1409, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1408[ 4u]= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); _temp1408[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1344); _temp1408[ 2u]= _temp1340? Cyc_Absynpp_qvar2doc( _temp1346): Cyc_Absynpp_typedef_name2doc(
_temp1346); _temp1408[ 1u]= _temp1340? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1408[ 0u]= Cyc_Absynpp_scope2doc( _temp1348); Cyc_PP_cat( _tag_arr(
_temp1408, sizeof( struct Cyc_PP_Doc*), 8u));});} goto _LL1291; _LL1303: s=({
struct Cyc_PP_Doc* _temp1410[ 5u]; _temp1410[ 4u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1410[ 3u]= Cyc_Absynpp_exp2doc( _temp1350);
_temp1410[ 2u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp1410[ 1u]= Cyc_Absynpp_pat2doc( _temp1352); _temp1410[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1410,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL1291; _LL1305: s=({ struct Cyc_PP_Doc*
_temp1411[ 3u]; _temp1411[ 2u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1411[ 1u]= Cyc_Absynpp_ids2doc( _temp1354); _temp1411[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1411,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1291; _LL1307: if( _temp1359 ==  0){
s=({ struct Cyc_PP_Doc* _temp1412[ 4u]; _temp1412[ 3u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1412[ 2u]= Cyc_Absynpp_typedef_name2doc(
_temp1361); _temp1412[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1412[ 0u]= Cyc_Absynpp_scope2doc( _temp1363); Cyc_PP_cat( _tag_arr(
_temp1412, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1413[ 7u]; _temp1413[ 6u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1413[ 5u]= Cyc_PP_line_doc(); _temp1413[ 4u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1414[ 2u]; _temp1414[ 1u]= Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*) _temp1359->v); _temp1414[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1414, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1413[ 3u]= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); _temp1413[ 2u]= Cyc_Absynpp_qvar2doc(
_temp1361); _temp1413[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1413[ 0u]= Cyc_Absynpp_scope2doc( _temp1363); Cyc_PP_cat( _tag_arr(
_temp1413, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1291; _LL1309: s=({
struct Cyc_PP_Doc* _temp1415[ 3u]; _temp1415[ 2u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1415[ 1u]= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1416; _temp1416.q_const= 0; _temp1416.q_volatile= 0; _temp1416.q_restrict=
0; _temp1416;}),( void*) _temp1365->defn,({ struct Cyc_Core_Opt* _temp1417=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1417->v=(
void*)({ struct Cyc_PP_Doc* _temp1418[ 2u]; _temp1418[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1365->tvs); _temp1418[ 0u]= Cyc_Absynpp_typedef_name2doc( _temp1365->name);
Cyc_PP_cat( _tag_arr( _temp1418, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1417;}));
_temp1415[ 0u]= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
Cyc_PP_cat( _tag_arr( _temp1415, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL1291; _LL1311: if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add(
_temp1369);} s=({ struct Cyc_PP_Doc* _temp1419[ 7u]; _temp1419[ 6u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1419[ 5u]= Cyc_PP_line_doc();
_temp1419[ 4u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1367); _temp1419[ 3u]= Cyc_PP_line_doc();
_temp1419[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1419[ 1u]= Cyc_PP_textptr( _temp1369); _temp1419[ 0u]= Cyc_PP_text(
_tag_arr("namespace ", sizeof( unsigned char), 11u)); Cyc_PP_cat( _tag_arr(
_temp1419, sizeof( struct Cyc_PP_Doc*), 7u));}); if( Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();} goto _LL1291; _LL1313: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc* _temp1420[ 7u]; _temp1420[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1420[ 5u]= Cyc_PP_line_doc(); _temp1420[ 4u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1371); _temp1420[ 3u]= Cyc_PP_line_doc();
_temp1420[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1420[ 1u]= Cyc_Absynpp_qvar2doc( _temp1373); _temp1420[ 0u]= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp1420,
sizeof( struct Cyc_PP_Doc*), 7u));});} else{ s=({ struct Cyc_PP_Doc* _temp1421[
7u]; _temp1421[ 6u]= Cyc_PP_text( _tag_arr("/* } */", sizeof( unsigned char), 8u));
_temp1421[ 5u]= Cyc_PP_line_doc(); _temp1421[ 4u]=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp1371); _temp1421[ 3u]= Cyc_PP_line_doc();
_temp1421[ 2u]= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u));
_temp1421[ 1u]= Cyc_Absynpp_qvar2doc( _temp1373); _temp1421[ 0u]= Cyc_PP_text(
_tag_arr("/* using ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1421, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1291; _LL1315: if( Cyc_Absynpp_print_externC_stmts){
s=({ struct Cyc_PP_Doc* _temp1422[ 5u]; _temp1422[ 4u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1422[ 3u]= Cyc_PP_line_doc(); _temp1422[ 2u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1375); _temp1422[ 1u]= Cyc_PP_line_doc();
_temp1422[ 0u]= Cyc_PP_text( _tag_arr("extern \"C\" {", sizeof( unsigned char),
13u)); Cyc_PP_cat( _tag_arr( _temp1422, sizeof( struct Cyc_PP_Doc*), 5u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1423[ 5u]; _temp1423[ 4u]= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); _temp1423[ 3u]= Cyc_PP_line_doc();
_temp1423[ 2u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1375); _temp1423[ 1u]= Cyc_PP_line_doc();
_temp1423[ 0u]= Cyc_PP_text( _tag_arr("/* extern \"C\" { */", sizeof(
unsigned char), 19u)); Cyc_PP_cat( _tag_arr( _temp1423, sizeof( struct Cyc_PP_Doc*),
5u));});} goto _LL1291; _LL1291:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp1424= sc; _LL1426: if( _temp1424 == ( void*) Cyc_Absyn_Static){
goto _LL1427;} else{ goto _LL1428;} _LL1428: if( _temp1424 == ( void*) Cyc_Absyn_Public){
goto _LL1429;} else{ goto _LL1430;} _LL1430: if( _temp1424 == ( void*) Cyc_Absyn_Extern){
goto _LL1431;} else{ goto _LL1432;} _LL1432: if( _temp1424 == ( void*) Cyc_Absyn_ExternC){
goto _LL1433;} else{ goto _LL1434;} _LL1434: if( _temp1424 == ( void*) Cyc_Absyn_Abstract){
goto _LL1435;} else{ goto _LL1425;} _LL1427: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL1429: return Cyc_PP_nil_doc(); _LL1431: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL1433: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL1435:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL1425:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp1436= t; struct
Cyc_Absyn_Tvar* _temp1444; struct Cyc_List_List* _temp1446; _LL1438: if((
unsigned int) _temp1436 >  3u?*(( int*) _temp1436) ==  Cyc_Absyn_VarType: 0){
_LL1445: _temp1444=(( struct Cyc_Absyn_VarType_struct*) _temp1436)->f1; goto
_LL1439;} else{ goto _LL1440;} _LL1440: if(( unsigned int) _temp1436 >  3u?*((
int*) _temp1436) ==  Cyc_Absyn_JoinEff: 0){ _LL1447: _temp1446=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1436)->f1; goto _LL1441;} else{ goto _LL1442;} _LL1442: goto _LL1443;
_LL1439: return Cyc_Tcutil_is_temp_tvar( _temp1444); _LL1441: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect, _temp1446); _LL1443: return 0; _LL1437:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t){ void*
_temp1448= t; struct Cyc_Absyn_Exp* _temp1462; struct Cyc_Absyn_Tqual _temp1464;
void* _temp1466; struct Cyc_Absyn_PtrInfo _temp1468; struct Cyc_Absyn_Conref*
_temp1470; struct Cyc_Absyn_Tqual _temp1472; struct Cyc_Absyn_Conref* _temp1474;
void* _temp1476; void* _temp1478; struct Cyc_Absyn_FnInfo _temp1480; struct Cyc_List_List*
_temp1482; struct Cyc_List_List* _temp1484; struct Cyc_Absyn_VarargInfo*
_temp1486; int _temp1488; struct Cyc_List_List* _temp1490; void* _temp1492;
struct Cyc_Core_Opt* _temp1494; struct Cyc_List_List* _temp1496; int _temp1498;
struct Cyc_Core_Opt* _temp1500; struct Cyc_Core_Opt* _temp1502; struct Cyc_Core_Opt*
_temp1504; struct Cyc_List_List* _temp1506; struct _tuple0* _temp1508; _LL1450:
if(( unsigned int) _temp1448 >  3u?*(( int*) _temp1448) ==  Cyc_Absyn_ArrayType:
0){ _LL1467: _temp1466=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1448)->f1;
goto _LL1465; _LL1465: _temp1464=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1448)->f2; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1448)->f3; goto _LL1451;} else{ goto _LL1452;} _LL1452: if(( unsigned int)
_temp1448 >  3u?*(( int*) _temp1448) ==  Cyc_Absyn_PointerType: 0){ _LL1469:
_temp1468=(( struct Cyc_Absyn_PointerType_struct*) _temp1448)->f1; _LL1479:
_temp1478=( void*) _temp1468.elt_typ; goto _LL1477; _LL1477: _temp1476=( void*)
_temp1468.rgn_typ; goto _LL1475; _LL1475: _temp1474= _temp1468.nullable; goto
_LL1473; _LL1473: _temp1472= _temp1468.tq; goto _LL1471; _LL1471: _temp1470=
_temp1468.bounds; goto _LL1453;} else{ goto _LL1454;} _LL1454: if(( unsigned int)
_temp1448 >  3u?*(( int*) _temp1448) ==  Cyc_Absyn_FnType: 0){ _LL1481:
_temp1480=(( struct Cyc_Absyn_FnType_struct*) _temp1448)->f1; _LL1497: _temp1496=
_temp1480.tvars; goto _LL1495; _LL1495: _temp1494= _temp1480.effect; goto
_LL1493; _LL1493: _temp1492=( void*) _temp1480.ret_typ; goto _LL1491; _LL1491:
_temp1490= _temp1480.args; goto _LL1489; _LL1489: _temp1488= _temp1480.c_varargs;
goto _LL1487; _LL1487: _temp1486= _temp1480.cyc_varargs; goto _LL1485; _LL1485:
_temp1484= _temp1480.rgn_po; goto _LL1483; _LL1483: _temp1482= _temp1480.attributes;
goto _LL1455;} else{ goto _LL1456;} _LL1456: if(( unsigned int) _temp1448 >  3u?*((
int*) _temp1448) ==  Cyc_Absyn_Evar: 0){ _LL1503: _temp1502=(( struct Cyc_Absyn_Evar_struct*)
_temp1448)->f1; goto _LL1501; _LL1501: _temp1500=(( struct Cyc_Absyn_Evar_struct*)
_temp1448)->f2; goto _LL1499; _LL1499: _temp1498=(( struct Cyc_Absyn_Evar_struct*)
_temp1448)->f3; goto _LL1457;} else{ goto _LL1458;} _LL1458: if(( unsigned int)
_temp1448 >  3u?*(( int*) _temp1448) ==  Cyc_Absyn_TypedefType: 0){ _LL1509:
_temp1508=(( struct Cyc_Absyn_TypedefType_struct*) _temp1448)->f1; goto _LL1507;
_LL1507: _temp1506=(( struct Cyc_Absyn_TypedefType_struct*) _temp1448)->f2; goto
_LL1505; _LL1505: _temp1504=(( struct Cyc_Absyn_TypedefType_struct*) _temp1448)->f3;
goto _LL1459;} else{ goto _LL1460;} _LL1460: goto _LL1461; _LL1451: { struct Cyc_List_List*
_temp1512; void* _temp1514; struct Cyc_Absyn_Tqual _temp1516; struct _tuple4
_temp1510= Cyc_Absynpp_to_tms( _temp1464, _temp1466); _LL1517: _temp1516=
_temp1510.f1; goto _LL1515; _LL1515: _temp1514= _temp1510.f2; goto _LL1513;
_LL1513: _temp1512= _temp1510.f3; goto _LL1511; _LL1511: { void* tm; if(
_temp1462 ==  0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp1518=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1518[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1519; _temp1519.tag= Cyc_Absyn_ConstArray_mod;
_temp1519.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1462); _temp1519;});
_temp1518;});} return({ struct _tuple4 _temp1520; _temp1520.f1= _temp1516;
_temp1520.f2= _temp1514; _temp1520.f3=({ struct Cyc_List_List* _temp1521=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1521->hd=(
void*) tm; _temp1521->tl= _temp1512; _temp1521;}); _temp1520;});}} _LL1453: {
struct Cyc_List_List* _temp1524; void* _temp1526; struct Cyc_Absyn_Tqual
_temp1528; struct _tuple4 _temp1522= Cyc_Absynpp_to_tms( _temp1472, _temp1478);
_LL1529: _temp1528= _temp1522.f1; goto _LL1527; _LL1527: _temp1526= _temp1522.f2;
goto _LL1525; _LL1525: _temp1524= _temp1522.f3; goto _LL1523; _LL1523: { void*
ps;{ struct _tuple8 _temp1531=({ struct _tuple8 _temp1530; _temp1530.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1474))->v; _temp1530.f2=( void*)( Cyc_Absyn_compress_conref( _temp1470))->v;
_temp1530;}); void* _temp1541; void* _temp1543; struct Cyc_Absyn_Exp* _temp1545;
void* _temp1547; int _temp1549; void* _temp1551; void* _temp1553; struct Cyc_Absyn_Exp*
_temp1555; void* _temp1557; int _temp1559; void* _temp1561; void* _temp1563;
_LL1533: _LL1548: _temp1547= _temp1531.f1; if(( unsigned int) _temp1547 >  1u?*((
int*) _temp1547) ==  Cyc_Absyn_Eq_constr: 0){ _LL1550: _temp1549=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp1547)->f1; if( _temp1549 ==  1){ goto _LL1542;}
else{ goto _LL1535;}} else{ goto _LL1535;} _LL1542: _temp1541= _temp1531.f2; if((
unsigned int) _temp1541 >  1u?*(( int*) _temp1541) ==  Cyc_Absyn_Eq_constr: 0){
_LL1544: _temp1543=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1541)->f1;
if(( unsigned int) _temp1543 >  1u?*(( int*) _temp1543) ==  Cyc_Absyn_Upper_b: 0){
_LL1546: _temp1545=(( struct Cyc_Absyn_Upper_b_struct*) _temp1543)->f1; goto
_LL1534;} else{ goto _LL1535;}} else{ goto _LL1535;} _LL1535: _LL1558: _temp1557=
_temp1531.f1; if(( unsigned int) _temp1557 >  1u?*(( int*) _temp1557) ==  Cyc_Absyn_Eq_constr:
0){ _LL1560: _temp1559=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1557)->f1;
if( _temp1559 ==  0){ goto _LL1552;} else{ goto _LL1537;}} else{ goto _LL1537;}
_LL1552: _temp1551= _temp1531.f2; if(( unsigned int) _temp1551 >  1u?*(( int*)
_temp1551) ==  Cyc_Absyn_Eq_constr: 0){ _LL1554: _temp1553=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1551)->f1; if(( unsigned int) _temp1553 >  1u?*(( int*) _temp1553) ==  Cyc_Absyn_Upper_b:
0){ _LL1556: _temp1555=(( struct Cyc_Absyn_Upper_b_struct*) _temp1553)->f1; goto
_LL1536;} else{ goto _LL1537;}} else{ goto _LL1537;} _LL1537: _LL1562: _temp1561=
_temp1531.f2; if(( unsigned int) _temp1561 >  1u?*(( int*) _temp1561) ==  Cyc_Absyn_Eq_constr:
0){ _LL1564: _temp1563=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1561)->f1;
if( _temp1563 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1538;} else{ goto
_LL1539;}} else{ goto _LL1539;} _LL1539: goto _LL1540; _LL1534: ps=( void*)({
struct Cyc_Absyn_Nullable_ps_struct* _temp1565=( struct Cyc_Absyn_Nullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp1565[ 0]=({
struct Cyc_Absyn_Nullable_ps_struct _temp1566; _temp1566.tag= Cyc_Absyn_Nullable_ps;
_temp1566.f1= _temp1545; _temp1566;}); _temp1565;}); goto _LL1532; _LL1536: ps=(
void*)({ struct Cyc_Absyn_NonNullable_ps_struct* _temp1567=( struct Cyc_Absyn_NonNullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp1567[ 0]=({
struct Cyc_Absyn_NonNullable_ps_struct _temp1568; _temp1568.tag= Cyc_Absyn_NonNullable_ps;
_temp1568.f1= _temp1555; _temp1568;}); _temp1567;}); goto _LL1532; _LL1538: ps=(
void*) Cyc_Absyn_TaggedArray_ps; goto _LL1532; _LL1540: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL1532; _LL1532:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp1571=( struct Cyc_Absyn_Pointer_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp1571[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp1572; _temp1572.tag=
Cyc_Absyn_Pointer_mod; _temp1572.f1=( void*) ps; _temp1572.f2=( void*) _temp1476;
_temp1572.f3= tq; _temp1572;}); _temp1571;}); return({ struct _tuple4 _temp1569;
_temp1569.f1= _temp1528; _temp1569.f2= _temp1526; _temp1569.f3=({ struct Cyc_List_List*
_temp1570=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1570->hd=( void*) tm; _temp1570->tl= _temp1524; _temp1570;}); _temp1569;});}}}
_LL1455: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp1494 ==  0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*) _temp1494->v)){ _temp1494= 0; _temp1496= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp1496);}}{ struct Cyc_List_List* _temp1575;
void* _temp1577; struct Cyc_Absyn_Tqual _temp1579; struct _tuple4 _temp1573= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp1492); _LL1580: _temp1579= _temp1573.f1; goto
_LL1578; _LL1578: _temp1577= _temp1573.f2; goto _LL1576; _LL1576: _temp1575=
_temp1573.f3; goto _LL1574; _LL1574: { struct Cyc_List_List* tms= _temp1575; if(
_temp1482 !=  0? ! Cyc_Absynpp_to_VC: 0){ tms=({ struct Cyc_List_List* _temp1581=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1581->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1582=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1582[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1583; _temp1583.tag= Cyc_Absyn_Attributes_mod;
_temp1583.f1= 0; _temp1583.f2= _temp1482; _temp1583;}); _temp1582;})); _temp1581->tl=
tms; _temp1581;});} tms=({ struct Cyc_List_List* _temp1584=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1584->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp1585=( struct Cyc_Absyn_Function_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp1585[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp1586; _temp1586.tag= Cyc_Absyn_Function_mod;
_temp1586.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp1587=(
struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp1587[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp1588; _temp1588.tag= Cyc_Absyn_WithTypes;
_temp1588.f1= _temp1490; _temp1588.f2= _temp1488; _temp1588.f3= _temp1486;
_temp1588.f4= _temp1494; _temp1588.f5= _temp1484; _temp1588;}); _temp1587;}));
_temp1586;}); _temp1585;})); _temp1584->tl= tms; _temp1584;}); if( Cyc_Absynpp_to_VC){
for( 0; _temp1482 !=  0; _temp1482= _temp1482->tl){ void* _temp1589=( void*)
_temp1482->hd; _LL1591: if( _temp1589 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1589 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1594;} else{ goto _LL1595;} _LL1595: if( _temp1589 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1596;} else{ goto _LL1597;} _LL1597: goto _LL1598; _LL1592: goto _LL1594;
_LL1594: goto _LL1596; _LL1596: tms=({ struct Cyc_List_List* _temp1599=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1599->hd=( void*)((
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1600=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1600[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1601; _temp1601.tag= Cyc_Absyn_Attributes_mod;
_temp1601.f1= 0; _temp1601.f2=({ struct Cyc_List_List* _temp1602=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1602->hd=( void*)(( void*)
_temp1482->hd); _temp1602->tl= 0; _temp1602;}); _temp1601;}); _temp1600;}));
_temp1599->tl= tms; _temp1599;}); goto AfterAtts; _LL1598: goto _LL1590; _LL1590:;}}
AfterAtts: if( _temp1496 !=  0){ tms=({ struct Cyc_List_List* _temp1603=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1603->hd=( void*)((
void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp1604=( struct Cyc_Absyn_TypeParams_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp1604[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp1605; _temp1605.tag= Cyc_Absyn_TypeParams_mod;
_temp1605.f1= _temp1496; _temp1605.f2= 0; _temp1605.f3= 1; _temp1605;});
_temp1604;})); _temp1603->tl= tms; _temp1603;});} return({ struct _tuple4
_temp1606; _temp1606.f1= _temp1579; _temp1606.f2= _temp1577; _temp1606.f3= tms;
_temp1606;});}} _LL1457: if( _temp1500 ==  0){ return({ struct _tuple4 _temp1607;
_temp1607.f1= tq; _temp1607.f2= t; _temp1607.f3= 0; _temp1607;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*) _temp1500->v);} _LL1459: if( _temp1504 ==  0? 1:
! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp1608; _temp1608.f1=
tq; _temp1608.f2= t; _temp1608.f3= 0; _temp1608;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*) _temp1504->v);} _LL1461: return({ struct _tuple4 _temp1609;
_temp1609.f1= tq; _temp1609.f2= t; _temp1609.f3= 0; _temp1609;}); _LL1449:;}
struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual tq, void* t,
struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp1612; void* _temp1614;
struct Cyc_Absyn_Tqual _temp1616; struct _tuple4 _temp1610= Cyc_Absynpp_to_tms(
tq, t); _LL1617: _temp1616= _temp1610.f1; goto _LL1615; _LL1615: _temp1614=
_temp1610.f2; goto _LL1613; _LL1613: _temp1612= _temp1610.f3; goto _LL1611;
_LL1611: _temp1612= Cyc_List_imp_rev( _temp1612); if( _temp1612 ==  0? dopt == 
0: 0){ return({ struct Cyc_PP_Doc* _temp1618[ 2u]; _temp1618[ 1u]= Cyc_Absynpp_ntyp2doc(
_temp1614); _temp1618[ 0u]= Cyc_Absynpp_tqual2doc( _temp1616); Cyc_PP_cat(
_tag_arr( _temp1618, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ return({
struct Cyc_PP_Doc* _temp1619[ 4u]; _temp1619[ 3u]= Cyc_Absynpp_dtms2doc( dopt == 
0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*) dopt->v, _temp1612); _temp1619[ 2u]=
Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); _temp1619[ 1u]= Cyc_Absynpp_ntyp2doc(
_temp1614); _temp1619[ 0u]= Cyc_Absynpp_tqual2doc( _temp1616); Cyc_PP_cat(
_tag_arr( _temp1619, sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfield2doc( struct Cyc_Absyn_Structfield* f){ if( f->width != 
0){ return({ struct Cyc_PP_Doc* _temp1620[ 5u]; _temp1620[ 4u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1620[ 3u]= Cyc_Absynpp_atts2doc(
f->attributes); _temp1620[ 2u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( f->width)); _temp1620[ 1u]= Cyc_PP_text( _tag_arr(":", sizeof(
unsigned char), 2u)); _temp1620[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp1621=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1621->v=( void*) Cyc_PP_textptr( f->name); _temp1621;}));
Cyc_PP_cat( _tag_arr( _temp1620, sizeof( struct Cyc_PP_Doc*), 5u));});} else{
return({ struct Cyc_PP_Doc* _temp1622[ 3u]; _temp1622[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1622[ 1u]= Cyc_Absynpp_atts2doc(
f->attributes); _temp1622[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp1623=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1623->v=( void*) Cyc_PP_textptr( f->name); _temp1623;}));
Cyc_PP_cat( _tag_arr( _temp1622, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct
Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,
_tag_arr("", sizeof( unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc* _temp1624[ 4u];
_temp1624[ 3u]= f->typs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs);
_temp1624[ 2u]= f->tvs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs);
_temp1624[ 1u]= Cyc_Absynpp_typedef_name2doc( f->name); _temp1624[ 0u]= Cyc_Absynpp_scope2doc((
void*) f->sc); Cyc_PP_cat( _tag_arr( _temp1624, sizeof( struct Cyc_PP_Doc*), 4u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Tunionfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fields);} void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f){ for( 0; tdl !=  0; tdl=
tdl->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*) tdl->hd),
72, f);({ void* _temp1625[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1625, sizeof( void*), 0u));});}} struct
_tagged_arr Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc(
Cyc_PP_seql( _tag_arr("", sizeof( unsigned char), 1u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_decl2doc, tdl)), 72);} struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp* e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e),
72);} struct _tagged_arr Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){
return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s), 72);} struct _tagged_arr
Cyc_Absynpp_typ2string( void* t){ return Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc(
t), 72);} struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0* v){ return
Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);} struct _tagged_arr Cyc_Absynpp_typ2cstring(
void* t){ int old_qvar_to_Cids= Cyc_Absynpp_qvar_to_Cids; int old_add_cyc_prefix=
Cyc_Absynpp_add_cyc_prefix; Cyc_Absynpp_qvar_to_Cids= 1; Cyc_Absynpp_add_cyc_prefix=
0;{ struct _tagged_arr s= Cyc_Absynpp_typ2string( t); Cyc_Absynpp_qvar_to_Cids=
old_qvar_to_Cids; Cyc_Absynpp_add_cyc_prefix= old_add_cyc_prefix; return s;}}
struct _tagged_arr Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_arr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);} struct _tagged_arr Cyc_Absynpp_scope2string( void* sc){ void* _temp1626=
sc; _LL1628: if( _temp1626 == ( void*) Cyc_Absyn_Static){ goto _LL1629;} else{
goto _LL1630;} _LL1630: if( _temp1626 == ( void*) Cyc_Absyn_Public){ goto
_LL1631;} else{ goto _LL1632;} _LL1632: if( _temp1626 == ( void*) Cyc_Absyn_Extern){
goto _LL1633;} else{ goto _LL1634;} _LL1634: if( _temp1626 == ( void*) Cyc_Absyn_ExternC){
goto _LL1635;} else{ goto _LL1636;} _LL1636: if( _temp1626 == ( void*) Cyc_Absyn_Abstract){
goto _LL1637;} else{ goto _LL1627;} _LL1629: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL1631: return _tag_arr("public", sizeof( unsigned char),
7u); _LL1633: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL1635:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL1637: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL1627:;}
