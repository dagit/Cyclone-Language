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
extern struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_Invalid_argument[
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
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
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
const int Cyc_Absyn_DoubleType= 2; static const int Cyc_Absyn_ArrayType= 6;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual f2;
struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 8;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_StructType= 9; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_AnonEnumType=
14; struct Cyc_Absyn_AnonEnumType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnHandleType= 15; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 16; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
17; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 18; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_RgnsEff= 19; struct Cyc_Absyn_RgnsEff_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
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
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
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
struct Cyc_List_List* atts){ for( 0; atts !=  0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp110=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL112: if( _temp110 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp110 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL115;} else{ goto _LL116;} _LL116: if( _temp110 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL117;} else{ goto _LL118;} _LL118: goto _LL119; _LL113: return Cyc_PP_text(
_tag_arr(" _stdcall ", sizeof( unsigned char), 11u)); _LL115: return Cyc_PP_text(
_tag_arr(" _cdecl ", sizeof( unsigned char), 9u)); _LL117: return Cyc_PP_text(
_tag_arr(" _fastcall ", sizeof( unsigned char), 12u)); _LL119: goto _LL111;
_LL111:;} return Cyc_PP_nil_doc();} struct Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 !=  0; atts2=(( struct Cyc_List_List*) _check_null( atts2))->tl){
void* _temp120=( void*)(( struct Cyc_List_List*) _check_null( atts2))->hd;
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
==  0){ return 0;}{ void* _temp132=( void*)(( struct Cyc_List_List*) _check_null(
tms))->hd; _LL134: if(( unsigned int) _temp132 >  1u?*(( int*) _temp132) ==  Cyc_Absyn_Pointer_mod:
0){ goto _LL135;} else{ goto _LL136;} _LL136: if(( unsigned int) _temp132 >  1u?*((
int*) _temp132) ==  Cyc_Absyn_Attributes_mod: 0){ goto _LL137;} else{ goto
_LL138;} _LL138: goto _LL139; _LL135: return 1; _LL137: if( ! Cyc_Absynpp_to_VC){
return 0;} return Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl); _LL139: return 0; _LL133:;}} extern struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct
Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms ==  0){ return d;}{ struct
Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d,(( struct Cyc_List_List*) _check_null(
tms))->tl); struct Cyc_PP_Doc* p_rest=({ struct Cyc_PP_Doc* _temp222[ 3u];
_temp222[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp222[
1u]= rest; _temp222[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp222, sizeof( struct Cyc_PP_Doc*), 3u));}); void*
_temp140=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp*
_temp154; void* _temp156; struct Cyc_List_List* _temp158; int _temp160; struct
Cyc_Position_Segment* _temp162; struct Cyc_List_List* _temp164; struct Cyc_Absyn_Tqual
_temp166; void* _temp168; void* _temp170; _LL142: if( _temp140 == ( void*) Cyc_Absyn_Carray_mod){
goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int) _temp140 >  1u?*((
int*) _temp140) ==  Cyc_Absyn_ConstArray_mod: 0){ _LL155: _temp154=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp140)->f1; goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int)
_temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_Function_mod: 0){ _LL157:
_temp156=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp140)->f1; goto
_LL147;} else{ goto _LL148;} _LL148: if(( unsigned int) _temp140 >  1u?*(( int*)
_temp140) ==  Cyc_Absyn_Attributes_mod: 0){ _LL159: _temp158=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp140)->f2; goto _LL149;} else{ goto _LL150;} _LL150: if(( unsigned int)
_temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL165:
_temp164=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f1; goto _LL163;
_LL163: _temp162=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f2; goto
_LL161; _LL161: _temp160=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f3;
goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp140 >  1u?*((
int*) _temp140) ==  Cyc_Absyn_Pointer_mod: 0){ _LL171: _temp170=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f1; goto _LL169; _LL169:
_temp168=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f2; goto
_LL167; _LL167: _temp166=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f3;
goto _LL153;} else{ goto _LL141;} _LL143: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc* _temp172[ 2u]; _temp172[ 1u]= Cyc_PP_text( _tag_arr("[]", sizeof(
unsigned char), 3u)); _temp172[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp172,
sizeof( struct Cyc_PP_Doc*), 2u));}); _LL145: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc* _temp173[ 4u]; _temp173[ 3u]= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); _temp173[ 2u]= Cyc_Absynpp_exp2doc( _temp154); _temp173[ 1u]=
Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u)); _temp173[ 0u]= rest;
Cyc_PP_cat( _tag_arr( _temp173, sizeof( struct Cyc_PP_Doc*), 4u));}); _LL147:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;}{ void* _temp174= _temp156; struct Cyc_List_List* _temp180; struct
Cyc_Core_Opt* _temp182; struct Cyc_Absyn_VarargInfo* _temp184; int _temp186;
struct Cyc_List_List* _temp188; struct Cyc_Position_Segment* _temp190; struct
Cyc_List_List* _temp192; _LL176: if(*(( int*) _temp174) ==  Cyc_Absyn_WithTypes){
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
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;} return({ struct Cyc_PP_Doc* _temp196[ 2u]; _temp196[ 1u]= Cyc_Absynpp_atts2doc(
_temp158); _temp196[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp196, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ return({ struct Cyc_PP_Doc* _temp197[ 2u]; _temp197[ 1u]=
rest; _temp197[ 0u]= Cyc_Absynpp_callconv2doc( _temp158); Cyc_PP_cat( _tag_arr(
_temp197, sizeof( struct Cyc_PP_Doc*), 2u));});} return rest;} _LL151: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} if( _temp160){
return({ struct Cyc_PP_Doc* _temp198[ 2u]; _temp198[ 1u]= Cyc_Absynpp_ktvars2doc(
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
_temp229 >  4u?*(( int*) _temp229) ==  Cyc_Absyn_AccessEff: 0){ _LL238: _temp237=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp229)->f1; goto _LL232;} else{
goto _LL233;} _LL233: if(( unsigned int) _temp229 >  4u?*(( int*) _temp229) == 
Cyc_Absyn_JoinEff: 0){ _LL240: _temp239=(( struct Cyc_Absyn_JoinEff_struct*)
_temp229)->f1; goto _LL234;} else{ goto _LL235;} _LL235: goto _LL236; _LL232:*
rgions=({ struct Cyc_List_List* _temp241=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp241->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp237); _temp241->tl=* rgions; _temp241;}); goto _LL230; _LL234: for( 0;
_temp239 !=  0; _temp239=(( struct Cyc_List_List*) _check_null( _temp239))->tl){
Cyc_Absynpp_effects2docs( rgions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp239))->hd);} goto _LL230; _LL236:* effects=({ struct Cyc_List_List*
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
_temp315; void* _temp317; void* _temp319; void* _temp321; struct Cyc_List_List*
_temp323; struct Cyc_List_List* _temp325; struct _tuple0* _temp327; struct Cyc_List_List*
_temp329; struct _tuple0* _temp331; struct Cyc_List_List* _temp333; struct Cyc_List_List*
_temp335; struct Cyc_List_List* _temp337; struct _tuple0* _temp339; struct Cyc_Core_Opt*
_temp341; struct Cyc_List_List* _temp343; struct _tuple0* _temp345; void*
_temp347; void* _temp349; _LL247: if(( unsigned int) _temp245 >  4u?*(( int*)
_temp245) ==  Cyc_Absyn_ArrayType: 0){ goto _LL248;} else{ goto _LL249;} _LL249:
if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_FnType: 0){
goto _LL250;} else{ goto _LL251;} _LL251: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_PointerType: 0){ goto _LL252;} else{ goto _LL253;}
_LL253: if( _temp245 == ( void*) Cyc_Absyn_VoidType){ goto _LL254;} else{ goto
_LL255;} _LL255: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_Evar:
0){ _LL302: _temp301=(( struct Cyc_Absyn_Evar_struct*) _temp245)->f1; goto
_LL300; _LL300: _temp299=(( struct Cyc_Absyn_Evar_struct*) _temp245)->f2; goto
_LL298; _LL298: _temp297=(( struct Cyc_Absyn_Evar_struct*) _temp245)->f3; goto
_LL296; _LL296: _temp295=(( struct Cyc_Absyn_Evar_struct*) _temp245)->f4; goto
_LL256;} else{ goto _LL257;} _LL257: if(( unsigned int) _temp245 >  4u?*(( int*)
_temp245) ==  Cyc_Absyn_VarType: 0){ _LL304: _temp303=(( struct Cyc_Absyn_VarType_struct*)
_temp245)->f1; goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_TunionType: 0){ _LL306:
_temp305=(( struct Cyc_Absyn_TunionType_struct*) _temp245)->f1; _LL312: _temp311=(
void*) _temp305.tunion_info; goto _LL310; _LL310: _temp309= _temp305.targs; goto
_LL308; _LL308: _temp307=( void*) _temp305.rgn; goto _LL260;} else{ goto _LL261;}
_LL261: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_TunionFieldType:
0){ _LL314: _temp313=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp245)->f1;
_LL318: _temp317=( void*) _temp313.field_info; goto _LL316; _LL316: _temp315=
_temp313.targs; goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_IntType: 0){ _LL322: _temp321=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp245)->f1; goto _LL320; _LL320:
_temp319=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp245)->f2; goto _LL264;}
else{ goto _LL265;} _LL265: if( _temp245 == ( void*) Cyc_Absyn_FloatType){ goto
_LL266;} else{ goto _LL267;} _LL267: if( _temp245 == ( void*) Cyc_Absyn_DoubleType){
goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_TupleType: 0){ _LL324: _temp323=(( struct Cyc_Absyn_TupleType_struct*)
_temp245)->f1; goto _LL270;} else{ goto _LL271;} _LL271: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_StructType: 0){ _LL328:
_temp327=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f1; goto _LL326;
_LL326: _temp325=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f2; goto
_LL272;} else{ goto _LL273;} _LL273: if(( unsigned int) _temp245 >  4u?*(( int*)
_temp245) ==  Cyc_Absyn_UnionType: 0){ _LL332: _temp331=(( struct Cyc_Absyn_UnionType_struct*)
_temp245)->f1; goto _LL330; _LL330: _temp329=(( struct Cyc_Absyn_UnionType_struct*)
_temp245)->f2; goto _LL274;} else{ goto _LL275;} _LL275: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_AnonStructType: 0){ _LL334:
_temp333=(( struct Cyc_Absyn_AnonStructType_struct*) _temp245)->f1; goto _LL276;}
else{ goto _LL277;} _LL277: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245)
==  Cyc_Absyn_AnonUnionType: 0){ _LL336: _temp335=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp245)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_AnonEnumType: 0){ _LL338:
_temp337=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp245)->f1; goto _LL280;}
else{ goto _LL281;} _LL281: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245)
==  Cyc_Absyn_EnumType: 0){ _LL340: _temp339=(( struct Cyc_Absyn_EnumType_struct*)
_temp245)->f1; goto _LL282;} else{ goto _LL283;} _LL283: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_TypedefType: 0){ _LL346:
_temp345=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f1; goto _LL344;
_LL344: _temp343=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f2; goto
_LL342; _LL342: _temp341=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f3;
goto _LL284;} else{ goto _LL285;} _LL285: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_RgnHandleType: 0){ _LL348: _temp347=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp245)->f1; goto _LL286;} else{ goto
_LL287;} _LL287: if( _temp245 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL288;}
else{ goto _LL289;} _LL289: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245)
==  Cyc_Absyn_RgnsEff: 0){ _LL350: _temp349=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp245)->f1; goto _LL290;} else{ goto _LL291;} _LL291: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_AccessEff: 0){ goto _LL292;}
else{ goto _LL293;} _LL293: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245)
==  Cyc_Absyn_JoinEff: 0){ goto _LL294;} else{ goto _LL246;} _LL248: return Cyc_PP_text(
_tag_arr("[[[array]]]", sizeof( unsigned char), 12u)); _LL250: return Cyc_PP_nil_doc();
_LL252: return Cyc_PP_nil_doc(); _LL254: s= Cyc_PP_text( _tag_arr("void",
sizeof( unsigned char), 5u)); goto _LL246; _LL256: if( _temp299 !=  0){ return
Cyc_Absynpp_ntyp2doc(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp299))->v);}
else{ s=({ struct Cyc_PP_Doc* _temp351[ 6u]; _temp351[ 5u]= _temp301 ==  0? Cyc_PP_text(
_tag_arr("?", sizeof( unsigned char), 2u)): Cyc_Absynpp_kind2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp301))->v); _temp351[ 4u]= Cyc_PP_text(
_tag_arr(")::", sizeof( unsigned char), 4u)); _temp351[ 3u]=( ! Cyc_Absynpp_print_full_evars?
1: _temp295 ==  0)? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp295))->v);
_temp351[ 2u]= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp353; _temp353.tag= Cyc_Std_Int_pa; _temp353.f1=( int)(( unsigned int)
_temp297);{ void* _temp352[ 1u]={& _temp353}; Cyc_Std_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp352, sizeof( void*), 1u));}}));
_temp351[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); _temp351[
0u]= Cyc_PP_text( _tag_arr("%", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp351, sizeof( struct Cyc_PP_Doc*), 6u));});} goto _LL246; _LL258:
s= Cyc_PP_textptr( _temp303->name); if( Cyc_Absynpp_print_all_kinds){ s=({
struct Cyc_PP_Doc* _temp354[ 3u]; _temp354[ 2u]= Cyc_Absynpp_ckind2doc( _temp303->kind);
_temp354[ 1u]= Cyc_PP_text( _tag_arr("::", sizeof( unsigned char), 3u));
_temp354[ 0u]= s; Cyc_PP_cat( _tag_arr( _temp354, sizeof( struct Cyc_PP_Doc*), 3u));});}
if( Cyc_Absynpp_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar( _temp303): 0){ s=({
struct Cyc_PP_Doc* _temp355[ 3u]; _temp355[ 2u]= Cyc_PP_text( _tag_arr(" */",
sizeof( unsigned char), 4u)); _temp355[ 1u]= s; _temp355[ 0u]= Cyc_PP_text(
_tag_arr("_ /* ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp355,
sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL260:{ void* _temp356=
_temp311; struct Cyc_Absyn_UnknownTunionInfo _temp362; int _temp364; struct
_tuple0* _temp366; struct Cyc_Absyn_Tuniondecl** _temp368; struct Cyc_Absyn_Tuniondecl*
_temp370; struct Cyc_Absyn_Tuniondecl _temp371; int _temp372; struct _tuple0*
_temp374; _LL358: if(*(( int*) _temp356) ==  Cyc_Absyn_UnknownTunion){ _LL363:
_temp362=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp356)->f1; _LL367:
_temp366= _temp362.name; goto _LL365; _LL365: _temp364= _temp362.is_xtunion;
goto _LL359;} else{ goto _LL360;} _LL360: if(*(( int*) _temp356) ==  Cyc_Absyn_KnownTunion){
_LL369: _temp368=(( struct Cyc_Absyn_KnownTunion_struct*) _temp356)->f1;
_temp370=* _temp368; _temp371=* _temp370; _LL375: _temp374= _temp371.name; goto
_LL373; _LL373: _temp372= _temp371.is_xtunion; goto _LL361;} else{ goto _LL357;}
_LL359: _temp374= _temp366; _temp372= _temp364; goto _LL361; _LL361: { struct
Cyc_PP_Doc* _temp376= Cyc_PP_text( _temp372? _tag_arr("xtunion ", sizeof(
unsigned char), 9u): _tag_arr("tunion ", sizeof( unsigned char), 8u));{ void*
_temp377= Cyc_Tcutil_compress( _temp307); _LL379: if( _temp377 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL380;} else{ goto _LL381;} _LL381: goto _LL382; _LL380: s=({ struct Cyc_PP_Doc*
_temp383[ 3u]; _temp383[ 2u]= Cyc_Absynpp_tps2doc( _temp309); _temp383[ 1u]= Cyc_Absynpp_qvar2doc(
_temp374); _temp383[ 0u]= _temp376; Cyc_PP_cat( _tag_arr( _temp383, sizeof(
struct Cyc_PP_Doc*), 3u));}); goto _LL378; _LL382: s=({ struct Cyc_PP_Doc*
_temp384[ 5u]; _temp384[ 4u]= Cyc_Absynpp_tps2doc( _temp309); _temp384[ 3u]= Cyc_Absynpp_qvar2doc(
_temp374); _temp384[ 2u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp384[ 1u]= Cyc_Absynpp_typ2doc( _temp307); _temp384[ 0u]= _temp376; Cyc_PP_cat(
_tag_arr( _temp384, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL378; _LL378:;}
goto _LL357;} _LL357:;} goto _LL246; _LL262:{ void* _temp385= _temp317; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp391; int _temp393; struct _tuple0*
_temp395; struct _tuple0* _temp397; struct Cyc_Absyn_Tunionfield* _temp399;
struct Cyc_Absyn_Tunionfield _temp401; struct _tuple0* _temp402; struct Cyc_Absyn_Tuniondecl*
_temp404; struct Cyc_Absyn_Tuniondecl _temp406; int _temp407; struct _tuple0*
_temp409; _LL387: if(*(( int*) _temp385) ==  Cyc_Absyn_UnknownTunionfield){
_LL392: _temp391=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp385)->f1;
_LL398: _temp397= _temp391.tunion_name; goto _LL396; _LL396: _temp395= _temp391.field_name;
goto _LL394; _LL394: _temp393= _temp391.is_xtunion; goto _LL388;} else{ goto
_LL389;} _LL389: if(*(( int*) _temp385) ==  Cyc_Absyn_KnownTunionfield){ _LL405:
_temp404=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp385)->f1; _temp406=*
_temp404; _LL410: _temp409= _temp406.name; goto _LL408; _LL408: _temp407=
_temp406.is_xtunion; goto _LL400; _LL400: _temp399=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp385)->f2; _temp401=* _temp399; _LL403: _temp402= _temp401.name; goto _LL390;}
else{ goto _LL386;} _LL388: _temp409= _temp397; _temp407= _temp393; _temp402=
_temp395; goto _LL390; _LL390: { struct Cyc_PP_Doc* _temp411= Cyc_PP_text(
_temp407? _tag_arr("xtunion ", sizeof( unsigned char), 9u): _tag_arr("tunion ",
sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc* _temp412[ 4u]; _temp412[ 3u]=
Cyc_Absynpp_qvar2doc( _temp402); _temp412[ 2u]= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); _temp412[ 1u]= Cyc_Absynpp_qvar2doc( _temp409);
_temp412[ 0u]= _temp411; Cyc_PP_cat( _tag_arr( _temp412, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL386;} _LL386:;} goto _LL246; _LL264: { struct _tagged_arr sns;
struct _tagged_arr ts;{ void* _temp413= _temp321; _LL415: if( _temp413 == ( void*)
Cyc_Absyn_Signed){ goto _LL416;} else{ goto _LL417;} _LL417: if( _temp413 == (
void*) Cyc_Absyn_Unsigned){ goto _LL418;} else{ goto _LL414;} _LL416: sns=
_tag_arr("", sizeof( unsigned char), 1u); goto _LL414; _LL418: sns= _tag_arr("unsigned ",
sizeof( unsigned char), 10u); goto _LL414; _LL414:;}{ void* _temp419= _temp319;
_LL421: if( _temp419 == ( void*) Cyc_Absyn_B1){ goto _LL422;} else{ goto _LL423;}
_LL423: if( _temp419 == ( void*) Cyc_Absyn_B2){ goto _LL424;} else{ goto _LL425;}
_LL425: if( _temp419 == ( void*) Cyc_Absyn_B4){ goto _LL426;} else{ goto _LL427;}
_LL427: if( _temp419 == ( void*) Cyc_Absyn_B8){ goto _LL428;} else{ goto _LL420;}
_LL422:{ void* _temp429= _temp321; _LL431: if( _temp429 == ( void*) Cyc_Absyn_Signed){
goto _LL432;} else{ goto _LL433;} _LL433: if( _temp429 == ( void*) Cyc_Absyn_Unsigned){
goto _LL434;} else{ goto _LL430;} _LL432: sns= _tag_arr("signed ", sizeof(
unsigned char), 8u); goto _LL430; _LL434: goto _LL430; _LL430:;} ts= _tag_arr("char",
sizeof( unsigned char), 5u); goto _LL420; _LL424: ts= _tag_arr("short", sizeof(
unsigned char), 6u); goto _LL420; _LL426: ts= _tag_arr("int", sizeof(
unsigned char), 4u); goto _LL420; _LL428: ts= Cyc_Absynpp_to_VC? _tag_arr("__int64",
sizeof( unsigned char), 8u): _tag_arr("long long", sizeof( unsigned char), 10u);
goto _LL420; _LL420:;} s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp437; _temp437.tag= Cyc_Std_String_pa; _temp437.f1=( struct _tagged_arr) ts;{
struct Cyc_Std_String_pa_struct _temp436; _temp436.tag= Cyc_Std_String_pa;
_temp436.f1=( struct _tagged_arr) sns;{ void* _temp435[ 2u]={& _temp436,&
_temp437}; Cyc_Std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp435, sizeof( void*), 2u));}}})); goto _LL246;} _LL266: s= Cyc_PP_text(
_tag_arr("float", sizeof( unsigned char), 6u)); goto _LL246; _LL268: s= Cyc_PP_text(
_tag_arr("double", sizeof( unsigned char), 7u)); goto _LL246; _LL270: s=({
struct Cyc_PP_Doc* _temp438[ 2u]; _temp438[ 1u]= Cyc_Absynpp_args2doc( _temp323);
_temp438[ 0u]= Cyc_PP_text( _tag_arr("$", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp438, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL272:
if( _temp327 ==  0){ s=({ struct Cyc_PP_Doc* _temp439[ 2u]; _temp439[ 1u]= Cyc_Absynpp_tps2doc(
_temp325); _temp439[ 0u]= Cyc_PP_text( _tag_arr("struct ", sizeof( unsigned char),
8u)); Cyc_PP_cat( _tag_arr( _temp439, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ s=({ struct Cyc_PP_Doc* _temp440[ 3u]; _temp440[ 2u]= Cyc_Absynpp_tps2doc(
_temp325); _temp440[ 1u]= _temp327 ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp327)); _temp440[ 0u]= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp440, sizeof( struct Cyc_PP_Doc*),
3u));});} goto _LL246; _LL274: if( _temp331 ==  0){ s=({ struct Cyc_PP_Doc*
_temp441[ 2u]; _temp441[ 1u]= Cyc_Absynpp_tps2doc( _temp329); _temp441[ 0u]= Cyc_PP_text(
_tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp441,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp442[ 3u];
_temp442[ 2u]= Cyc_Absynpp_tps2doc( _temp329); _temp442[ 1u]= _temp331 ==  0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp331));
_temp442[ 0u]= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp442, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL276:
s=({ struct Cyc_PP_Doc* _temp443[ 3u]; _temp443[ 2u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp443[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp333)); _temp443[ 0u]= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp443, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL278: s=({ struct Cyc_PP_Doc* _temp444[ 3u]; _temp444[ 2u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp444[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_structfields2doc( _temp335)); _temp444[ 0u]= Cyc_PP_text(
_tag_arr("union {", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp444,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL280: s=({ struct Cyc_PP_Doc*
_temp445[ 3u]; _temp445[ 2u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp445[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_enumfields2doc( _temp337));
_temp445[ 0u]= Cyc_PP_text( _tag_arr("enum {", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp445, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL282: s=({
struct Cyc_PP_Doc* _temp446[ 2u]; _temp446[ 1u]= Cyc_Absynpp_qvar2doc( _temp339);
_temp446[ 0u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp446, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL284: s=({
struct Cyc_PP_Doc* _temp447[ 2u]; _temp447[ 1u]= Cyc_Absynpp_tps2doc( _temp343);
_temp447[ 0u]= Cyc_Absynpp_qvar2doc( _temp345); Cyc_PP_cat( _tag_arr( _temp447,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL286: s=({ struct Cyc_PP_Doc*
_temp448[ 3u]; _temp448[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof( unsigned char),
2u)); _temp448[ 1u]= Cyc_Absynpp_rgn2doc( _temp347); _temp448[ 0u]= Cyc_PP_text(
_tag_arr("region_t<", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp448, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL288: s= Cyc_Absynpp_rgn2doc(
t); goto _LL246; _LL290: s=({ struct Cyc_PP_Doc* _temp449[ 3u]; _temp449[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp449[ 1u]= Cyc_Absynpp_typ2doc(
_temp349); _temp449[ 0u]= Cyc_PP_text( _tag_arr("regions(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp449, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL292: goto _LL294; _LL294: s= Cyc_Absynpp_eff2doc( t);
goto _LL246; _LL246:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct
Cyc_Core_Opt* vo){ return vo ==  0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct _tuple8{
void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc( struct
_tuple8* cmp){ struct _tuple8 _temp452; void* _temp453; void* _temp455; struct
_tuple8* _temp450= cmp; _temp452=* _temp450; _LL456: _temp455= _temp452.f1; goto
_LL454; _LL454: _temp453= _temp452.f2; goto _LL451; _LL451: return({ struct Cyc_PP_Doc*
_temp457[ 3u]; _temp457[ 2u]= Cyc_Absynpp_rgn2doc( _temp453); _temp457[ 1u]= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); _temp457[ 0u]= Cyc_Absynpp_rgn2doc(
_temp455); Cyc_PP_cat( _tag_arr( _temp457, sizeof( struct Cyc_PP_Doc*), 3u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc( struct Cyc_List_List* po){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 ==  0? 0:({ struct Cyc_Core_Opt* _temp458=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp458->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp458;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp459=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp459=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp459,({
struct Cyc_List_List* _temp460=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp460->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp460->tl= 0; _temp460;}));} else{ if(
cyc_varargs !=  0){ struct Cyc_PP_Doc* _temp461=({ struct Cyc_PP_Doc* _temp463[
3u]; _temp463[ 2u]= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp464=( struct
_tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp464->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp464->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp464->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp464;})); _temp463[ 1u]=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text( _tag_arr(" inject ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp463[ 0u]= Cyc_PP_text( _tag_arr("...", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp463, sizeof( struct Cyc_PP_Doc*), 3u));}); _temp459=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp459,({
struct Cyc_List_List* _temp462=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp462->hd=( void*) _temp461; _temp462->tl= 0;
_temp462;}));}}{ struct Cyc_PP_Doc* _temp465= Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u), _temp459); if( effopt !=  0){ _temp465=({ struct
Cyc_PP_Doc* _temp466[ 3u]; _temp466[ 2u]= Cyc_Absynpp_eff2doc(( void*)(( struct
Cyc_Core_Opt*) _check_null( effopt))->v); _temp466[ 1u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp466[ 0u]= _temp465; Cyc_PP_cat( _tag_arr(
_temp466, sizeof( struct Cyc_PP_Doc*), 3u));});} if( rgn_po !=  0){ _temp465=({
struct Cyc_PP_Doc* _temp467[ 3u]; _temp467[ 2u]= Cyc_Absynpp_rgnpo2doc( rgn_po);
_temp467[ 1u]= Cyc_PP_text( _tag_arr(":", sizeof( unsigned char), 2u)); _temp467[
0u]= _temp465; Cyc_PP_cat( _tag_arr( _temp467, sizeof( struct Cyc_PP_Doc*), 3u));});}
return({ struct Cyc_PP_Doc* _temp468[ 3u]; _temp468[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp468[ 1u]= _temp465; _temp468[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp468,
sizeof( struct Cyc_PP_Doc*), 3u));});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp469=( struct _tuple1*)
_cycalloc( sizeof( struct _tuple1)); _temp469->f1=({ struct Cyc_Core_Opt*
_temp470=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp470->v=( void*)(* arg).f1; _temp470;}); _temp469->f2=(* arg).f2; _temp469->f3=(*
arg).f3; _temp469;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_arr* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp471= 0; int match;{ void*
_temp472=(* q).f1; struct Cyc_List_List* _temp480; struct Cyc_List_List*
_temp482; _LL474: if( _temp472 == ( void*) Cyc_Absyn_Loc_n){ goto _LL475;} else{
goto _LL476;} _LL476: if(( unsigned int) _temp472 >  1u?*(( int*) _temp472) == 
Cyc_Absyn_Rel_n: 0){ _LL481: _temp480=(( struct Cyc_Absyn_Rel_n_struct*)
_temp472)->f1; goto _LL477;} else{ goto _LL478;} _LL478: if(( unsigned int)
_temp472 >  1u?*(( int*) _temp472) ==  Cyc_Absyn_Abs_n: 0){ _LL483: _temp482=((
struct Cyc_Absyn_Abs_n_struct*) _temp472)->f1; goto _LL479;} else{ goto _LL473;}
_LL475: _temp480= 0; goto _LL477; _LL477: match= 0; _temp471= _temp480; goto
_LL473; _LL479: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_Std_strptrcmp, _temp482, Cyc_Absynpp_curr_namespace):
0; _temp471=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp484=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp484->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp484->tl= _temp482;
_temp484;}): _temp482; goto _LL473; _LL473:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp471,({
struct Cyc_List_List* _temp485=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp485->hd=( void*)(* q).f2; _temp485->tl= 0; _temp485;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp471,({ struct Cyc_List_List* _temp486=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp486->hd=( void*)(* q).f2; _temp486->tl= 0;
_temp486;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp487=(* v).f1; struct Cyc_List_List* _temp497; struct Cyc_List_List*
_temp499; _LL489: if( _temp487 == ( void*) Cyc_Absyn_Loc_n){ goto _LL490;} else{
goto _LL491;} _LL491: if(( unsigned int) _temp487 >  1u?*(( int*) _temp487) == 
Cyc_Absyn_Rel_n: 0){ _LL498: _temp497=(( struct Cyc_Absyn_Rel_n_struct*)
_temp487)->f1; if( _temp497 ==  0){ goto _LL492;} else{ goto _LL493;}} else{
goto _LL493;} _LL493: if(( unsigned int) _temp487 >  1u?*(( int*) _temp487) == 
Cyc_Absyn_Abs_n: 0){ _LL500: _temp499=(( struct Cyc_Absyn_Abs_n_struct*)
_temp487)->f1; goto _LL494;} else{ goto _LL495;} _LL495: goto _LL496; _LL490:
goto _LL492; _LL492: return Cyc_Absynpp_var2doc((* v).f2); _LL494: if((( int(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_strptrcmp, _temp499, Cyc_Absynpp_curr_namespace)
==  0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL496;} _LL496:
return({ struct Cyc_PP_Doc* _temp501[ 2u]; _temp501[ 1u]= Cyc_Absynpp_qvar2doc(
v); _temp501[ 0u]= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof(
unsigned char), 23u)); Cyc_PP_cat( _tag_arr( _temp501, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL488:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp502=(
void*) e->r; void* _temp580; struct Cyc_Absyn_Exp* _temp582; struct Cyc_Absyn_Exp*
_temp584; _LL504: if(*(( int*) _temp502) ==  Cyc_Absyn_Const_e){ goto _LL505;}
else{ goto _LL506;} _LL506: if(*(( int*) _temp502) ==  Cyc_Absyn_Var_e){ goto
_LL507;} else{ goto _LL508;} _LL508: if(*(( int*) _temp502) ==  Cyc_Absyn_UnknownId_e){
goto _LL509;} else{ goto _LL510;} _LL510: if(*(( int*) _temp502) ==  Cyc_Absyn_Primop_e){
_LL581: _temp580=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp502)->f1;
goto _LL511;} else{ goto _LL512;} _LL512: if(*(( int*) _temp502) ==  Cyc_Absyn_AssignOp_e){
goto _LL513;} else{ goto _LL514;} _LL514: if(*(( int*) _temp502) ==  Cyc_Absyn_Increment_e){
goto _LL515;} else{ goto _LL516;} _LL516: if(*(( int*) _temp502) ==  Cyc_Absyn_Conditional_e){
goto _LL517;} else{ goto _LL518;} _LL518: if(*(( int*) _temp502) ==  Cyc_Absyn_SeqExp_e){
goto _LL519;} else{ goto _LL520;} _LL520: if(*(( int*) _temp502) ==  Cyc_Absyn_UnknownCall_e){
goto _LL521;} else{ goto _LL522;} _LL522: if(*(( int*) _temp502) ==  Cyc_Absyn_FnCall_e){
goto _LL523;} else{ goto _LL524;} _LL524: if(*(( int*) _temp502) ==  Cyc_Absyn_Throw_e){
goto _LL525;} else{ goto _LL526;} _LL526: if(*(( int*) _temp502) ==  Cyc_Absyn_NoInstantiate_e){
_LL583: _temp582=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp502)->f1;
goto _LL527;} else{ goto _LL528;} _LL528: if(*(( int*) _temp502) ==  Cyc_Absyn_Instantiate_e){
_LL585: _temp584=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp502)->f1; goto
_LL529;} else{ goto _LL530;} _LL530: if(*(( int*) _temp502) ==  Cyc_Absyn_Cast_e){
goto _LL531;} else{ goto _LL532;} _LL532: if(*(( int*) _temp502) ==  Cyc_Absyn_New_e){
goto _LL533;} else{ goto _LL534;} _LL534: if(*(( int*) _temp502) ==  Cyc_Absyn_Address_e){
goto _LL535;} else{ goto _LL536;} _LL536: if(*(( int*) _temp502) ==  Cyc_Absyn_Sizeoftyp_e){
goto _LL537;} else{ goto _LL538;} _LL538: if(*(( int*) _temp502) ==  Cyc_Absyn_Sizeofexp_e){
goto _LL539;} else{ goto _LL540;} _LL540: if(*(( int*) _temp502) ==  Cyc_Absyn_Offsetof_e){
goto _LL541;} else{ goto _LL542;} _LL542: if(*(( int*) _temp502) ==  Cyc_Absyn_Gentyp_e){
goto _LL543;} else{ goto _LL544;} _LL544: if(*(( int*) _temp502) ==  Cyc_Absyn_Deref_e){
goto _LL545;} else{ goto _LL546;} _LL546: if(*(( int*) _temp502) ==  Cyc_Absyn_StructMember_e){
goto _LL547;} else{ goto _LL548;} _LL548: if(*(( int*) _temp502) ==  Cyc_Absyn_StructArrow_e){
goto _LL549;} else{ goto _LL550;} _LL550: if(*(( int*) _temp502) ==  Cyc_Absyn_Subscript_e){
goto _LL551;} else{ goto _LL552;} _LL552: if(*(( int*) _temp502) ==  Cyc_Absyn_Tuple_e){
goto _LL553;} else{ goto _LL554;} _LL554: if(*(( int*) _temp502) ==  Cyc_Absyn_CompoundLit_e){
goto _LL555;} else{ goto _LL556;} _LL556: if(*(( int*) _temp502) ==  Cyc_Absyn_Array_e){
goto _LL557;} else{ goto _LL558;} _LL558: if(*(( int*) _temp502) ==  Cyc_Absyn_Comprehension_e){
goto _LL559;} else{ goto _LL560;} _LL560: if(*(( int*) _temp502) ==  Cyc_Absyn_Struct_e){
goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp502) ==  Cyc_Absyn_AnonStruct_e){
goto _LL563;} else{ goto _LL564;} _LL564: if(*(( int*) _temp502) ==  Cyc_Absyn_Tunion_e){
goto _LL565;} else{ goto _LL566;} _LL566: if(*(( int*) _temp502) ==  Cyc_Absyn_Enum_e){
goto _LL567;} else{ goto _LL568;} _LL568: if(*(( int*) _temp502) ==  Cyc_Absyn_AnonEnum_e){
goto _LL569;} else{ goto _LL570;} _LL570: if(*(( int*) _temp502) ==  Cyc_Absyn_Malloc_e){
goto _LL571;} else{ goto _LL572;} _LL572: if(*(( int*) _temp502) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL573;} else{ goto _LL574;} _LL574: if(*(( int*) _temp502) ==  Cyc_Absyn_StmtExp_e){
goto _LL575;} else{ goto _LL576;} _LL576: if(*(( int*) _temp502) ==  Cyc_Absyn_Codegen_e){
goto _LL577;} else{ goto _LL578;} _LL578: if(*(( int*) _temp502) ==  Cyc_Absyn_Fill_e){
goto _LL579;} else{ goto _LL503;} _LL505: return 10000; _LL507: return 10000;
_LL509: return 10000; _LL511: { void* _temp586= _temp580; _LL588: if( _temp586
== ( void*) Cyc_Absyn_Plus){ goto _LL589;} else{ goto _LL590;} _LL590: if(
_temp586 == ( void*) Cyc_Absyn_Times){ goto _LL591;} else{ goto _LL592;} _LL592:
if( _temp586 == ( void*) Cyc_Absyn_Minus){ goto _LL593;} else{ goto _LL594;}
_LL594: if( _temp586 == ( void*) Cyc_Absyn_Div){ goto _LL595;} else{ goto _LL596;}
_LL596: if( _temp586 == ( void*) Cyc_Absyn_Mod){ goto _LL597;} else{ goto _LL598;}
_LL598: if( _temp586 == ( void*) Cyc_Absyn_Eq){ goto _LL599;} else{ goto _LL600;}
_LL600: if( _temp586 == ( void*) Cyc_Absyn_Neq){ goto _LL601;} else{ goto _LL602;}
_LL602: if( _temp586 == ( void*) Cyc_Absyn_Gt){ goto _LL603;} else{ goto _LL604;}
_LL604: if( _temp586 == ( void*) Cyc_Absyn_Lt){ goto _LL605;} else{ goto _LL606;}
_LL606: if( _temp586 == ( void*) Cyc_Absyn_Gte){ goto _LL607;} else{ goto _LL608;}
_LL608: if( _temp586 == ( void*) Cyc_Absyn_Lte){ goto _LL609;} else{ goto _LL610;}
_LL610: if( _temp586 == ( void*) Cyc_Absyn_Not){ goto _LL611;} else{ goto _LL612;}
_LL612: if( _temp586 == ( void*) Cyc_Absyn_Bitnot){ goto _LL613;} else{ goto
_LL614;} _LL614: if( _temp586 == ( void*) Cyc_Absyn_Bitand){ goto _LL615;} else{
goto _LL616;} _LL616: if( _temp586 == ( void*) Cyc_Absyn_Bitor){ goto _LL617;}
else{ goto _LL618;} _LL618: if( _temp586 == ( void*) Cyc_Absyn_Bitxor){ goto
_LL619;} else{ goto _LL620;} _LL620: if( _temp586 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL621;} else{ goto _LL622;} _LL622: if( _temp586 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL623;} else{ goto _LL624;} _LL624: if( _temp586 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL625;} else{ goto _LL626;} _LL626: if( _temp586 == ( void*) Cyc_Absyn_Size){
goto _LL627;} else{ goto _LL587;} _LL589: return 100; _LL591: return 110; _LL593:
return 100; _LL595: return 110; _LL597: return 110; _LL599: return 70; _LL601:
return 70; _LL603: return 80; _LL605: return 80; _LL607: return 80; _LL609:
return 80; _LL611: return 130; _LL613: return 130; _LL615: return 60; _LL617:
return 40; _LL619: return 50; _LL621: return 90; _LL623: return 80; _LL625:
return 80; _LL627: return 140; _LL587:;} _LL513: return 20; _LL515: return 130;
_LL517: return 30; _LL519: return 10; _LL521: return 140; _LL523: return 140;
_LL525: return 130; _LL527: return Cyc_Absynpp_exp_prec( _temp582); _LL529:
return Cyc_Absynpp_exp_prec( _temp584); _LL531: return 120; _LL533: return 15;
_LL535: return 130; _LL537: return 130; _LL539: return 130; _LL541: return 130;
_LL543: return 130; _LL545: return 130; _LL547: return 140; _LL549: return 140;
_LL551: return 140; _LL553: return 150; _LL555: goto _LL557; _LL557: goto _LL559;
_LL559: goto _LL561; _LL561: goto _LL563; _LL563: goto _LL565; _LL565: goto
_LL567; _LL567: goto _LL569; _LL569: goto _LL571; _LL571: return 140; _LL573:
return 140; _LL575: return 10000; _LL577: return 140; _LL579: return 140; _LL503:;}
struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp628=(
void*) e->r; void* _temp708; struct _tuple0* _temp710; struct _tuple0* _temp712;
struct Cyc_List_List* _temp714; void* _temp716; struct Cyc_Absyn_Exp* _temp718;
struct Cyc_Core_Opt* _temp720; struct Cyc_Absyn_Exp* _temp722; void* _temp724;
struct Cyc_Absyn_Exp* _temp726; struct Cyc_Absyn_Exp* _temp728; struct Cyc_Absyn_Exp*
_temp730; struct Cyc_Absyn_Exp* _temp732; struct Cyc_Absyn_Exp* _temp734; struct
Cyc_Absyn_Exp* _temp736; struct Cyc_List_List* _temp738; struct Cyc_Absyn_Exp*
_temp740; struct Cyc_List_List* _temp742; struct Cyc_Absyn_Exp* _temp744; struct
Cyc_Absyn_Exp* _temp746; struct Cyc_Absyn_Exp* _temp748; struct Cyc_Absyn_Exp*
_temp750; struct Cyc_Absyn_Exp* _temp752; void* _temp754; struct Cyc_Absyn_Exp*
_temp756; struct Cyc_Absyn_Exp* _temp758; struct Cyc_Absyn_Exp* _temp760; void*
_temp762; struct Cyc_Absyn_Exp* _temp764; void* _temp766; struct _tagged_arr*
_temp768; void* _temp770; void* _temp772; unsigned int _temp774; void* _temp776;
void* _temp778; struct Cyc_List_List* _temp780; struct Cyc_Absyn_Exp* _temp782;
struct _tagged_arr* _temp784; struct Cyc_Absyn_Exp* _temp786; struct _tagged_arr*
_temp788; struct Cyc_Absyn_Exp* _temp790; struct Cyc_Absyn_Exp* _temp792; struct
Cyc_Absyn_Exp* _temp794; struct Cyc_List_List* _temp796; struct Cyc_List_List*
_temp798; struct _tuple1* _temp800; struct Cyc_List_List* _temp802; struct Cyc_Absyn_Exp*
_temp804; struct Cyc_Absyn_Exp* _temp806; struct Cyc_Absyn_Vardecl* _temp808;
struct Cyc_List_List* _temp810; struct _tuple0* _temp812; struct Cyc_List_List*
_temp814; struct Cyc_Absyn_Tunionfield* _temp816; struct Cyc_List_List* _temp818;
struct _tuple0* _temp820; struct _tuple0* _temp822; void* _temp824; struct Cyc_Absyn_Exp*
_temp826; struct Cyc_List_List* _temp828; struct Cyc_Core_Opt* _temp830; struct
Cyc_Absyn_Stmt* _temp832; struct Cyc_Absyn_Fndecl* _temp834; struct Cyc_Absyn_Exp*
_temp836; _LL630: if(*(( int*) _temp628) ==  Cyc_Absyn_Const_e){ _LL709:
_temp708=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp628)->f1; goto _LL631;}
else{ goto _LL632;} _LL632: if(*(( int*) _temp628) ==  Cyc_Absyn_Var_e){ _LL711:
_temp710=(( struct Cyc_Absyn_Var_e_struct*) _temp628)->f1; goto _LL633;} else{
goto _LL634;} _LL634: if(*(( int*) _temp628) ==  Cyc_Absyn_UnknownId_e){ _LL713:
_temp712=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp628)->f1; goto _LL635;}
else{ goto _LL636;} _LL636: if(*(( int*) _temp628) ==  Cyc_Absyn_Primop_e){
_LL717: _temp716=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp628)->f1;
goto _LL715; _LL715: _temp714=(( struct Cyc_Absyn_Primop_e_struct*) _temp628)->f2;
goto _LL637;} else{ goto _LL638;} _LL638: if(*(( int*) _temp628) ==  Cyc_Absyn_AssignOp_e){
_LL723: _temp722=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp628)->f1; goto
_LL721; _LL721: _temp720=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp628)->f2;
goto _LL719; _LL719: _temp718=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp628)->f3;
goto _LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp628) ==  Cyc_Absyn_Increment_e){
_LL727: _temp726=(( struct Cyc_Absyn_Increment_e_struct*) _temp628)->f1; goto
_LL725; _LL725: _temp724=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp628)->f2; goto _LL641;} else{ goto _LL642;} _LL642: if(*(( int*) _temp628)
==  Cyc_Absyn_Conditional_e){ _LL733: _temp732=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp628)->f1; goto _LL731; _LL731: _temp730=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp628)->f2; goto _LL729; _LL729: _temp728=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp628)->f3; goto _LL643;} else{ goto _LL644;} _LL644: if(*(( int*) _temp628)
==  Cyc_Absyn_SeqExp_e){ _LL737: _temp736=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp628)->f1; goto _LL735; _LL735: _temp734=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp628)->f2; goto _LL645;} else{ goto _LL646;} _LL646: if(*(( int*) _temp628)
==  Cyc_Absyn_UnknownCall_e){ _LL741: _temp740=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp628)->f1; goto _LL739; _LL739: _temp738=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp628)->f2; goto _LL647;} else{ goto _LL648;} _LL648: if(*(( int*) _temp628)
==  Cyc_Absyn_FnCall_e){ _LL745: _temp744=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp628)->f1; goto _LL743; _LL743: _temp742=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp628)->f2; goto _LL649;} else{ goto _LL650;} _LL650: if(*(( int*) _temp628)
==  Cyc_Absyn_Throw_e){ _LL747: _temp746=(( struct Cyc_Absyn_Throw_e_struct*)
_temp628)->f1; goto _LL651;} else{ goto _LL652;} _LL652: if(*(( int*) _temp628)
==  Cyc_Absyn_NoInstantiate_e){ _LL749: _temp748=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp628)->f1; goto _LL653;} else{ goto _LL654;} _LL654: if(*(( int*) _temp628)
==  Cyc_Absyn_Instantiate_e){ _LL751: _temp750=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp628)->f1; goto _LL655;} else{ goto _LL656;} _LL656: if(*(( int*) _temp628)
==  Cyc_Absyn_Cast_e){ _LL755: _temp754=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp628)->f1; goto _LL753; _LL753: _temp752=(( struct Cyc_Absyn_Cast_e_struct*)
_temp628)->f2; goto _LL657;} else{ goto _LL658;} _LL658: if(*(( int*) _temp628)
==  Cyc_Absyn_Address_e){ _LL757: _temp756=(( struct Cyc_Absyn_Address_e_struct*)
_temp628)->f1; goto _LL659;} else{ goto _LL660;} _LL660: if(*(( int*) _temp628)
==  Cyc_Absyn_New_e){ _LL761: _temp760=(( struct Cyc_Absyn_New_e_struct*)
_temp628)->f1; goto _LL759; _LL759: _temp758=(( struct Cyc_Absyn_New_e_struct*)
_temp628)->f2; goto _LL661;} else{ goto _LL662;} _LL662: if(*(( int*) _temp628)
==  Cyc_Absyn_Sizeoftyp_e){ _LL763: _temp762=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp628)->f1; goto _LL663;} else{ goto _LL664;} _LL664: if(*(( int*) _temp628)
==  Cyc_Absyn_Sizeofexp_e){ _LL765: _temp764=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp628)->f1; goto _LL665;} else{ goto _LL666;} _LL666: if(*(( int*) _temp628)
==  Cyc_Absyn_Offsetof_e){ _LL771: _temp770=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp628)->f1; goto _LL767; _LL767: _temp766=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp628)->f2; if(*(( int*) _temp766) ==  Cyc_Absyn_StructField){ _LL769:
_temp768=(( struct Cyc_Absyn_StructField_struct*) _temp766)->f1; goto _LL667;}
else{ goto _LL668;}} else{ goto _LL668;} _LL668: if(*(( int*) _temp628) ==  Cyc_Absyn_Offsetof_e){
_LL777: _temp776=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp628)->f1;
goto _LL773; _LL773: _temp772=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp628)->f2; if(*(( int*) _temp772) ==  Cyc_Absyn_TupleIndex){ _LL775:
_temp774=(( struct Cyc_Absyn_TupleIndex_struct*) _temp772)->f1; goto _LL669;}
else{ goto _LL670;}} else{ goto _LL670;} _LL670: if(*(( int*) _temp628) ==  Cyc_Absyn_Gentyp_e){
_LL781: _temp780=(( struct Cyc_Absyn_Gentyp_e_struct*) _temp628)->f1; goto
_LL779; _LL779: _temp778=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp628)->f2;
goto _LL671;} else{ goto _LL672;} _LL672: if(*(( int*) _temp628) ==  Cyc_Absyn_Deref_e){
_LL783: _temp782=(( struct Cyc_Absyn_Deref_e_struct*) _temp628)->f1; goto _LL673;}
else{ goto _LL674;} _LL674: if(*(( int*) _temp628) ==  Cyc_Absyn_StructMember_e){
_LL787: _temp786=(( struct Cyc_Absyn_StructMember_e_struct*) _temp628)->f1; goto
_LL785; _LL785: _temp784=(( struct Cyc_Absyn_StructMember_e_struct*) _temp628)->f2;
goto _LL675;} else{ goto _LL676;} _LL676: if(*(( int*) _temp628) ==  Cyc_Absyn_StructArrow_e){
_LL791: _temp790=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp628)->f1; goto
_LL789; _LL789: _temp788=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp628)->f2;
goto _LL677;} else{ goto _LL678;} _LL678: if(*(( int*) _temp628) ==  Cyc_Absyn_Subscript_e){
_LL795: _temp794=(( struct Cyc_Absyn_Subscript_e_struct*) _temp628)->f1; goto
_LL793; _LL793: _temp792=(( struct Cyc_Absyn_Subscript_e_struct*) _temp628)->f2;
goto _LL679;} else{ goto _LL680;} _LL680: if(*(( int*) _temp628) ==  Cyc_Absyn_Tuple_e){
_LL797: _temp796=(( struct Cyc_Absyn_Tuple_e_struct*) _temp628)->f1; goto _LL681;}
else{ goto _LL682;} _LL682: if(*(( int*) _temp628) ==  Cyc_Absyn_CompoundLit_e){
_LL801: _temp800=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp628)->f1; goto
_LL799; _LL799: _temp798=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp628)->f2;
goto _LL683;} else{ goto _LL684;} _LL684: if(*(( int*) _temp628) ==  Cyc_Absyn_Array_e){
_LL803: _temp802=(( struct Cyc_Absyn_Array_e_struct*) _temp628)->f1; goto _LL685;}
else{ goto _LL686;} _LL686: if(*(( int*) _temp628) ==  Cyc_Absyn_Comprehension_e){
_LL809: _temp808=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp628)->f1;
goto _LL807; _LL807: _temp806=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp628)->f2; goto _LL805; _LL805: _temp804=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp628)->f3; goto _LL687;} else{ goto _LL688;} _LL688: if(*(( int*) _temp628)
==  Cyc_Absyn_Struct_e){ _LL813: _temp812=(( struct Cyc_Absyn_Struct_e_struct*)
_temp628)->f1; goto _LL811; _LL811: _temp810=(( struct Cyc_Absyn_Struct_e_struct*)
_temp628)->f3; goto _LL689;} else{ goto _LL690;} _LL690: if(*(( int*) _temp628)
==  Cyc_Absyn_AnonStruct_e){ _LL815: _temp814=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp628)->f2; goto _LL691;} else{ goto _LL692;} _LL692: if(*(( int*) _temp628)
==  Cyc_Absyn_Tunion_e){ _LL819: _temp818=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp628)->f3; goto _LL817; _LL817: _temp816=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp628)->f5; goto _LL693;} else{ goto _LL694;} _LL694: if(*(( int*) _temp628)
==  Cyc_Absyn_Enum_e){ _LL821: _temp820=(( struct Cyc_Absyn_Enum_e_struct*)
_temp628)->f1; goto _LL695;} else{ goto _LL696;} _LL696: if(*(( int*) _temp628)
==  Cyc_Absyn_AnonEnum_e){ _LL823: _temp822=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp628)->f1; goto _LL697;} else{ goto _LL698;} _LL698: if(*(( int*) _temp628)
==  Cyc_Absyn_Malloc_e){ _LL827: _temp826=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp628)->f1; goto _LL825; _LL825: _temp824=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp628)->f2; goto _LL699;} else{ goto _LL700;} _LL700: if(*(( int*) _temp628)
==  Cyc_Absyn_UnresolvedMem_e){ _LL831: _temp830=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp628)->f1; goto _LL829; _LL829: _temp828=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp628)->f2; goto _LL701;} else{ goto _LL702;} _LL702: if(*(( int*) _temp628)
==  Cyc_Absyn_StmtExp_e){ _LL833: _temp832=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp628)->f1; goto _LL703;} else{ goto _LL704;} _LL704: if(*(( int*) _temp628)
==  Cyc_Absyn_Codegen_e){ _LL835: _temp834=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp628)->f1; goto _LL705;} else{ goto _LL706;} _LL706: if(*(( int*) _temp628)
==  Cyc_Absyn_Fill_e){ _LL837: _temp836=(( struct Cyc_Absyn_Fill_e_struct*)
_temp628)->f1; goto _LL707;} else{ goto _LL629;} _LL631: s= Cyc_Absynpp_cnst2doc(
_temp708); goto _LL629; _LL633: _temp712= _temp710; goto _LL635; _LL635: s= Cyc_Absynpp_qvar2doc(
_temp712); goto _LL629; _LL637: s= Cyc_Absynpp_primapp2doc( myprec, _temp716,
_temp714); goto _LL629; _LL639: s=({ struct Cyc_PP_Doc* _temp838[ 5u]; _temp838[
4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp718); _temp838[ 3u]= Cyc_PP_text(
_tag_arr("= ", sizeof( unsigned char), 3u)); _temp838[ 2u]= _temp720 ==  0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp720))->v); _temp838[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp838[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp722); Cyc_PP_cat( _tag_arr( _temp838, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL629; _LL641: { struct Cyc_PP_Doc* _temp839= Cyc_Absynpp_exp2doc_prec(
myprec, _temp726);{ void* _temp840= _temp724; _LL842: if( _temp840 == ( void*)
Cyc_Absyn_PreInc){ goto _LL843;} else{ goto _LL844;} _LL844: if( _temp840 == (
void*) Cyc_Absyn_PreDec){ goto _LL845;} else{ goto _LL846;} _LL846: if( _temp840
== ( void*) Cyc_Absyn_PostInc){ goto _LL847;} else{ goto _LL848;} _LL848: if(
_temp840 == ( void*) Cyc_Absyn_PostDec){ goto _LL849;} else{ goto _LL841;}
_LL843: s=({ struct Cyc_PP_Doc* _temp850[ 2u]; _temp850[ 1u]= _temp839; _temp850[
0u]= Cyc_PP_text( _tag_arr("++", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp850, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL841; _LL845: s=({
struct Cyc_PP_Doc* _temp851[ 2u]; _temp851[ 1u]= _temp839; _temp851[ 0u]= Cyc_PP_text(
_tag_arr("--", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr( _temp851,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL841; _LL847: s=({ struct Cyc_PP_Doc*
_temp852[ 2u]; _temp852[ 1u]= Cyc_PP_text( _tag_arr("++", sizeof( unsigned char),
3u)); _temp852[ 0u]= _temp839; Cyc_PP_cat( _tag_arr( _temp852, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL841; _LL849: s=({ struct Cyc_PP_Doc* _temp853[ 2u];
_temp853[ 1u]= Cyc_PP_text( _tag_arr("--", sizeof( unsigned char), 3u));
_temp853[ 0u]= _temp839; Cyc_PP_cat( _tag_arr( _temp853, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL841; _LL841:;} goto _LL629;} _LL643:{ struct _tuple8 _temp855=({
struct _tuple8 _temp854; _temp854.f1=( void*) _temp730->r; _temp854.f2=( void*)
_temp728->r; _temp854;}); _LL857: goto _LL858; _LL858: s=({ struct Cyc_PP_Doc*
_temp859[ 5u]; _temp859[ 4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp728);
_temp859[ 3u]= Cyc_PP_text( _tag_arr(" : ", sizeof( unsigned char), 4u));
_temp859[ 2u]= Cyc_Absynpp_exp2doc_prec( 0, _temp730); _temp859[ 1u]= Cyc_PP_text(
_tag_arr(" ? ", sizeof( unsigned char), 4u)); _temp859[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp732); Cyc_PP_cat( _tag_arr( _temp859, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL856; _LL856:;} goto _LL629; _LL645: s=({ struct Cyc_PP_Doc*
_temp860[ 5u]; _temp860[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp860[ 3u]= Cyc_Absynpp_exp2doc( _temp734); _temp860[ 2u]= Cyc_PP_text(
_tag_arr(", ", sizeof( unsigned char), 3u)); _temp860[ 1u]= Cyc_Absynpp_exp2doc(
_temp736); _temp860[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp860, sizeof( struct Cyc_PP_Doc*), 5u));}); goto
_LL629; _LL647: s=({ struct Cyc_PP_Doc* _temp861[ 4u]; _temp861[ 3u]= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); _temp861[ 2u]= Cyc_Absynpp_exps2doc_prec(
20, _temp738); _temp861[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); _temp861[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp740); Cyc_PP_cat(
_tag_arr( _temp861, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL629; _LL649: s=({
struct Cyc_PP_Doc* _temp862[ 4u]; _temp862[ 3u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp862[ 2u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp742); _temp862[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
_temp862[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp744); Cyc_PP_cat( _tag_arr(
_temp862, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL629; _LL651: s=({ struct
Cyc_PP_Doc* _temp863[ 2u]; _temp863[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp746); _temp863[ 0u]= Cyc_PP_text( _tag_arr("throw ", sizeof( unsigned char),
7u)); Cyc_PP_cat( _tag_arr( _temp863, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL629; _LL653: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp748); goto _LL629;
_LL655: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp750); goto _LL629; _LL657: s=({
struct Cyc_PP_Doc* _temp864[ 4u]; _temp864[ 3u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp752); _temp864[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp864[ 1u]= Cyc_Absynpp_typ2doc( _temp754); _temp864[ 0u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr(
_temp864, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL629; _LL659: s=({ struct
Cyc_PP_Doc* _temp865[ 2u]; _temp865[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp756); _temp865[ 0u]= Cyc_PP_text( _tag_arr("&", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp865, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL629; _LL661: if( _temp760 ==  0){ s=({ struct Cyc_PP_Doc* _temp866[ 2u];
_temp866[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp758); _temp866[ 0u]= Cyc_PP_text(
_tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp866,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp867[ 4u];
_temp867[ 3u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp758); _temp867[ 2u]= Cyc_PP_text(
_tag_arr(") ", sizeof( unsigned char), 3u)); _temp867[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp760)); _temp867[ 0u]= Cyc_PP_text(
_tag_arr("rnew(", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp867,
sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL629; _LL663: s=({ struct Cyc_PP_Doc*
_temp868[ 3u]; _temp868[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp868[ 1u]= Cyc_Absynpp_typ2doc( _temp762); _temp868[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp868,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL629; _LL665: s=({ struct Cyc_PP_Doc*
_temp869[ 3u]; _temp869[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp869[ 1u]= Cyc_Absynpp_exp2doc( _temp764); _temp869[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp869,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL629; _LL667: s=({ struct Cyc_PP_Doc*
_temp870[ 5u]; _temp870[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp870[ 3u]= Cyc_PP_textptr( _temp768); _temp870[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp870[ 1u]= Cyc_Absynpp_typ2doc(
_temp770); _temp870[ 0u]= Cyc_PP_text( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp870, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL629; _LL669: s=({ struct Cyc_PP_Doc* _temp871[ 5u]; _temp871[ 4u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp871[ 3u]= Cyc_PP_text((
struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp873; _temp873.tag= Cyc_Std_Int_pa;
_temp873.f1=( int) _temp774;{ void* _temp872[ 1u]={& _temp873}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp872, sizeof( void*),
1u));}})); _temp871[ 2u]= Cyc_PP_text( _tag_arr(",", sizeof( unsigned char), 2u));
_temp871[ 1u]= Cyc_Absynpp_typ2doc( _temp776); _temp871[ 0u]= Cyc_PP_text(
_tag_arr("offsetof(", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp871, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL629; _LL671: s=({ struct
Cyc_PP_Doc* _temp874[ 4u]; _temp874[ 3u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp874[ 2u]= Cyc_Absynpp_typ2doc( _temp778); _temp874[ 1u]=
Cyc_Absynpp_tvars2doc( _temp780); _temp874[ 0u]= Cyc_PP_text( _tag_arr("__gen(",
sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp874, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL629; _LL673: s=({ struct Cyc_PP_Doc* _temp875[ 2u]; _temp875[ 1u]=
Cyc_Absynpp_exp2doc_prec( myprec, _temp782); _temp875[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp875,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL629; _LL675: s=({ struct Cyc_PP_Doc*
_temp876[ 3u]; _temp876[ 2u]= Cyc_PP_textptr( _temp784); _temp876[ 1u]= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); _temp876[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp786); Cyc_PP_cat( _tag_arr( _temp876, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL629; _LL677: s=({ struct Cyc_PP_Doc* _temp877[ 3u]; _temp877[ 2u]=
Cyc_PP_textptr( _temp788); _temp877[ 1u]= Cyc_PP_text( _tag_arr("->", sizeof(
unsigned char), 3u)); _temp877[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp790);
Cyc_PP_cat( _tag_arr( _temp877, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL629; _LL679: s=({ struct Cyc_PP_Doc* _temp878[ 4u]; _temp878[ 3u]= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); _temp878[ 2u]= Cyc_Absynpp_exp2doc(
_temp792); _temp878[ 1u]= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u));
_temp878[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp794); Cyc_PP_cat( _tag_arr(
_temp878, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL629; _LL681: s=({ struct
Cyc_PP_Doc* _temp879[ 3u]; _temp879[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp879[ 1u]= Cyc_Absynpp_exps2doc_prec( 20, _temp796);
_temp879[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp879, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL629; _LL683: s=({
struct Cyc_PP_Doc* _temp880[ 4u]; _temp880[ 3u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp798)); _temp880[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
_temp880[ 1u]= Cyc_Absynpp_typ2doc((* _temp800).f3); _temp880[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp880,
sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL629; _LL685: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp802)); goto _LL629; _LL687: s=({ struct Cyc_PP_Doc*
_temp881[ 7u]; _temp881[ 6u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp881[ 5u]= Cyc_Absynpp_exp2doc( _temp804); _temp881[ 4u]= Cyc_PP_text(
_tag_arr(" : ", sizeof( unsigned char), 4u)); _temp881[ 3u]= Cyc_Absynpp_exp2doc(
_temp806); _temp881[ 2u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp881[ 1u]= Cyc_PP_text(*(* _temp808->name).f2); _temp881[ 0u]= Cyc_PP_text(
_tag_arr("{for ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp881,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL629; _LL689: s=({ struct Cyc_PP_Doc*
_temp882[ 2u]; _temp882[ 1u]= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char),
2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp810));
_temp882[ 0u]= Cyc_Absynpp_qvar2doc( _temp812); Cyc_PP_cat( _tag_arr( _temp882,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL629; _LL691: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp814)); goto _LL629; _LL693: if( _temp818 ==  0){ s= Cyc_Absynpp_qvar2doc(
_temp816->name);} else{ s=({ struct Cyc_PP_Doc* _temp883[ 2u]; _temp883[ 1u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp818)); _temp883[ 0u]= Cyc_Absynpp_qvar2doc( _temp816->name);
Cyc_PP_cat( _tag_arr( _temp883, sizeof( struct Cyc_PP_Doc*), 2u));});} goto
_LL629; _LL695: s= Cyc_Absynpp_qvar2doc( _temp820); goto _LL629; _LL697: s= Cyc_Absynpp_qvar2doc(
_temp822); goto _LL629; _LL699: if( _temp826 ==  0){ s=({ struct Cyc_PP_Doc*
_temp884[ 3u]; _temp884[ 2u]= Cyc_PP_text( _tag_arr("))", sizeof( unsigned char),
3u)); _temp884[ 1u]= Cyc_Absynpp_typ2doc( _temp824); _temp884[ 0u]= Cyc_PP_text(
_tag_arr("malloc(sizeof(", sizeof( unsigned char), 15u)); Cyc_PP_cat( _tag_arr(
_temp884, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ s=({ struct Cyc_PP_Doc*
_temp885[ 6u]; _temp885[ 5u]= Cyc_PP_text( _tag_arr("))", sizeof( unsigned char),
3u)); _temp885[ 4u]= Cyc_Absynpp_typ2doc( _temp824); _temp885[ 3u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); _temp885[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp885[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp826)); _temp885[ 0u]= Cyc_PP_text(
_tag_arr("rmalloc(", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp885, sizeof( struct Cyc_PP_Doc*), 6u));});} goto _LL629; _LL701: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp828)); goto _LL629; _LL703: s=({ struct Cyc_PP_Doc*
_temp886[ 3u]; _temp886[ 2u]= Cyc_PP_text( _tag_arr(" })", sizeof( unsigned char),
4u)); _temp886[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp832)); _temp886[
0u]= Cyc_PP_text( _tag_arr("({ ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp886, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL629; _LL705: s=({
struct Cyc_PP_Doc* _temp887[ 3u]; _temp887[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp887[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp888=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof(
struct Cyc_Absyn_Decl)); _temp888->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp889=( struct Cyc_Absyn_Fn_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp889[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp890; _temp890.tag= Cyc_Absyn_Fn_d;
_temp890.f1= _temp834; _temp890;}); _temp889;})); _temp888->loc= e->loc;
_temp888;}))); _temp887[ 0u]= Cyc_PP_text( _tag_arr("codegen(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp887, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL629; _LL707: s=({ struct Cyc_PP_Doc* _temp891[ 3u]; _temp891[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp891[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_exp2doc( _temp836)); _temp891[ 0u]= Cyc_PP_text( _tag_arr("fill(",
sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp891, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL629; _LL629:;} if( inprec >=  myprec){ s=({ struct Cyc_PP_Doc*
_temp892[ 3u]; _temp892[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp892[ 1u]= s; _temp892[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp892, sizeof( struct Cyc_PP_Doc*),
3u));});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){
void* _temp893= d; struct Cyc_Absyn_Exp* _temp899; struct _tagged_arr* _temp901;
_LL895: if(*(( int*) _temp893) ==  Cyc_Absyn_ArrayElement){ _LL900: _temp899=((
struct Cyc_Absyn_ArrayElement_struct*) _temp893)->f1; goto _LL896;} else{ goto
_LL897;} _LL897: if(*(( int*) _temp893) ==  Cyc_Absyn_FieldName){ _LL902:
_temp901=(( struct Cyc_Absyn_FieldName_struct*) _temp893)->f1; goto _LL898;}
else{ goto _LL894;} _LL896: return({ struct Cyc_PP_Doc* _temp903[ 3u]; _temp903[
2u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char), 2u)); _temp903[ 1u]= Cyc_Absynpp_exp2doc(
_temp899); _temp903[ 0u]= Cyc_PP_text( _tag_arr(".[", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp903, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL898:
return({ struct Cyc_PP_Doc* _temp904[ 2u]; _temp904[ 1u]= Cyc_PP_textptr(
_temp901); _temp904[ 0u]= Cyc_PP_text( _tag_arr(".", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp904, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL894:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 ==  0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({ struct Cyc_PP_Doc*
_temp905[ 2u]; _temp905[ 1u]= Cyc_Absynpp_exp2doc((* de).f2); _temp905[ 0u]= Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char),
2u), _tag_arr("=", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); Cyc_PP_cat( _tag_arr( _temp905, sizeof( struct Cyc_PP_Doc*), 2u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp906= c;
unsigned char _temp924; void* _temp926; short _temp928; void* _temp930; int
_temp932; void* _temp934; int _temp936; void* _temp938; long long _temp940; void*
_temp942; struct _tagged_arr _temp944; struct _tagged_arr _temp946; _LL908: if((
unsigned int) _temp906 >  1u?*(( int*) _temp906) ==  Cyc_Absyn_Char_c: 0){
_LL927: _temp926=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp906)->f1; goto
_LL925; _LL925: _temp924=(( struct Cyc_Absyn_Char_c_struct*) _temp906)->f2; goto
_LL909;} else{ goto _LL910;} _LL910: if(( unsigned int) _temp906 >  1u?*(( int*)
_temp906) ==  Cyc_Absyn_Short_c: 0){ _LL931: _temp930=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp906)->f1; goto _LL929; _LL929: _temp928=(( struct Cyc_Absyn_Short_c_struct*)
_temp906)->f2; goto _LL911;} else{ goto _LL912;} _LL912: if(( unsigned int)
_temp906 >  1u?*(( int*) _temp906) ==  Cyc_Absyn_Int_c: 0){ _LL935: _temp934=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp906)->f1; if( _temp934 == ( void*)
Cyc_Absyn_Signed){ goto _LL933;} else{ goto _LL914;} _LL933: _temp932=(( struct
Cyc_Absyn_Int_c_struct*) _temp906)->f2; goto _LL913;} else{ goto _LL914;} _LL914:
if(( unsigned int) _temp906 >  1u?*(( int*) _temp906) ==  Cyc_Absyn_Int_c: 0){
_LL939: _temp938=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp906)->f1; if(
_temp938 == ( void*) Cyc_Absyn_Unsigned){ goto _LL937;} else{ goto _LL916;}
_LL937: _temp936=(( struct Cyc_Absyn_Int_c_struct*) _temp906)->f2; goto _LL915;}
else{ goto _LL916;} _LL916: if(( unsigned int) _temp906 >  1u?*(( int*) _temp906)
==  Cyc_Absyn_LongLong_c: 0){ _LL943: _temp942=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp906)->f1; goto _LL941; _LL941: _temp940=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp906)->f2; goto _LL917;} else{ goto _LL918;} _LL918: if(( unsigned int)
_temp906 >  1u?*(( int*) _temp906) ==  Cyc_Absyn_Float_c: 0){ _LL945: _temp944=((
struct Cyc_Absyn_Float_c_struct*) _temp906)->f1; goto _LL919;} else{ goto _LL920;}
_LL920: if( _temp906 == ( void*) Cyc_Absyn_Null_c){ goto _LL921;} else{ goto
_LL922;} _LL922: if(( unsigned int) _temp906 >  1u?*(( int*) _temp906) ==  Cyc_Absyn_String_c:
0){ _LL947: _temp946=(( struct Cyc_Absyn_String_c_struct*) _temp906)->f1; goto
_LL923;} else{ goto _LL907;} _LL909: return Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp949; _temp949.tag= Cyc_Std_String_pa;
_temp949.f1=( struct _tagged_arr) Cyc_Absynpp_char_escape( _temp924);{ void*
_temp948[ 1u]={& _temp949}; Cyc_Std_aprintf( _tag_arr("'%s'", sizeof(
unsigned char), 5u), _tag_arr( _temp948, sizeof( void*), 1u));}})); _LL911:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp951;
_temp951.tag= Cyc_Std_Int_pa; _temp951.f1=( int)(( unsigned int)(( int) _temp928));{
void* _temp950[ 1u]={& _temp951}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp950, sizeof( void*), 1u));}})); _LL913:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp953;
_temp953.tag= Cyc_Std_Int_pa; _temp953.f1=( int)(( unsigned int) _temp932);{
void* _temp952[ 1u]={& _temp953}; Cyc_Std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp952, sizeof( void*), 1u));}})); _LL915:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp955;
_temp955.tag= Cyc_Std_Int_pa; _temp955.f1=( unsigned int) _temp936;{ void*
_temp954[ 1u]={& _temp955}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp954, sizeof( void*), 1u));}})); _LL917:
return Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char),
27u)); _LL919: return Cyc_PP_text( _temp944); _LL921: return Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); _LL923: return({ struct Cyc_PP_Doc*
_temp956[ 3u]; _temp956[ 2u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char),
2u)); _temp956[ 1u]= Cyc_PP_text( Cyc_Absynpp_string_escape( _temp946));
_temp956[ 0u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp956, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL907:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == ( void*) Cyc_Absyn_Size){ if(
es ==  0? 1:(( struct Cyc_List_List*) _check_null( es))->tl !=  0){( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp957=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp957[ 0]=({ struct Cyc_Core_Failure_struct
_temp958; _temp958.tag= Cyc_Core_Failure; _temp958.f1=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp960; _temp960.tag= Cyc_Std_String_pa;
_temp960.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp959[
1u]={& _temp960}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof( unsigned char), 44u), _tag_arr( _temp959, sizeof( void*), 1u));}});
_temp958;}); _temp957;}));}{ struct Cyc_PP_Doc* _temp961= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
return({ struct Cyc_PP_Doc* _temp962[ 2u]; _temp962[ 1u]= Cyc_PP_text( _tag_arr(".size",
sizeof( unsigned char), 6u)); _temp962[ 0u]= _temp961; Cyc_PP_cat( _tag_arr(
_temp962, sizeof( struct Cyc_PP_Doc*), 2u));});}} else{ struct Cyc_List_List* ds=((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds ==  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp963=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp963[ 0]=({ struct Cyc_Core_Failure_struct _temp964; _temp964.tag= Cyc_Core_Failure;
_temp964.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp966;
_temp966.tag= Cyc_Std_String_pa; _temp966.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp965[ 1u]={& _temp966}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof( unsigned char), 38u), _tag_arr( _temp965, sizeof( void*), 1u));}});
_temp964;}); _temp963;}));} else{ if((( struct Cyc_List_List*) _check_null( ds))->tl
==  0){ return({ struct Cyc_PP_Doc* _temp967[ 3u]; _temp967[ 2u]=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd; _temp967[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp967[ 0u]= ps; Cyc_PP_cat(
_tag_arr( _temp967, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ if((( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->tl
!=  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp968=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp968[ 0]=({ struct Cyc_Core_Failure_struct _temp969; _temp969.tag= Cyc_Core_Failure;
_temp969.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp971;
_temp971.tag= Cyc_Std_String_pa; _temp971.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp970[ 1u]={& _temp971}; Cyc_Std_aprintf( _tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof( unsigned char), 47u), _tag_arr( _temp970, sizeof( void*), 1u));}});
_temp969;}); _temp968;}));} else{ return({ struct Cyc_PP_Doc* _temp972[ 5u];
_temp972[ 4u]=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( ds))->tl))->hd; _temp972[ 3u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp972[ 2u]= ps; _temp972[ 1u]=
Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); _temp972[ 0u]=( struct
Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds))->hd; Cyc_PP_cat(
_tag_arr( _temp972, sizeof( struct Cyc_PP_Doc*), 5u));});}}}}} struct
_tagged_arr Cyc_Absynpp_prim2str( void* p){ void* _temp973= p; _LL975: if(
_temp973 == ( void*) Cyc_Absyn_Plus){ goto _LL976;} else{ goto _LL977;} _LL977:
if( _temp973 == ( void*) Cyc_Absyn_Times){ goto _LL978;} else{ goto _LL979;}
_LL979: if( _temp973 == ( void*) Cyc_Absyn_Minus){ goto _LL980;} else{ goto
_LL981;} _LL981: if( _temp973 == ( void*) Cyc_Absyn_Div){ goto _LL982;} else{
goto _LL983;} _LL983: if( _temp973 == ( void*) Cyc_Absyn_Mod){ goto _LL984;}
else{ goto _LL985;} _LL985: if( _temp973 == ( void*) Cyc_Absyn_Eq){ goto _LL986;}
else{ goto _LL987;} _LL987: if( _temp973 == ( void*) Cyc_Absyn_Neq){ goto _LL988;}
else{ goto _LL989;} _LL989: if( _temp973 == ( void*) Cyc_Absyn_Gt){ goto _LL990;}
else{ goto _LL991;} _LL991: if( _temp973 == ( void*) Cyc_Absyn_Lt){ goto _LL992;}
else{ goto _LL993;} _LL993: if( _temp973 == ( void*) Cyc_Absyn_Gte){ goto _LL994;}
else{ goto _LL995;} _LL995: if( _temp973 == ( void*) Cyc_Absyn_Lte){ goto _LL996;}
else{ goto _LL997;} _LL997: if( _temp973 == ( void*) Cyc_Absyn_Not){ goto _LL998;}
else{ goto _LL999;} _LL999: if( _temp973 == ( void*) Cyc_Absyn_Bitnot){ goto
_LL1000;} else{ goto _LL1001;} _LL1001: if( _temp973 == ( void*) Cyc_Absyn_Bitand){
goto _LL1002;} else{ goto _LL1003;} _LL1003: if( _temp973 == ( void*) Cyc_Absyn_Bitor){
goto _LL1004;} else{ goto _LL1005;} _LL1005: if( _temp973 == ( void*) Cyc_Absyn_Bitxor){
goto _LL1006;} else{ goto _LL1007;} _LL1007: if( _temp973 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL1008;} else{ goto _LL1009;} _LL1009: if( _temp973 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL1010;} else{ goto _LL1011;} _LL1011: if( _temp973 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL1012;} else{ goto _LL1013;} _LL1013: if( _temp973 == ( void*) Cyc_Absyn_Size){
goto _LL1014;} else{ goto _LL974;} _LL976: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL978: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL980: return _tag_arr("-", sizeof( unsigned char), 2u); _LL982: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL984: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL986: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL988: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL990: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL992: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL994: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL996: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL998: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL1000: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL1002: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL1004: return _tag_arr("|", sizeof( unsigned char), 2u); _LL1006: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL1008: return _tag_arr("<<",
sizeof( unsigned char), 3u); _LL1010: return _tag_arr(">>", sizeof(
unsigned char), 3u); _LL1012: return _tag_arr(">>>", sizeof( unsigned char), 4u);
_LL1014: return _tag_arr("size", sizeof( unsigned char), 5u); _LL974:;} struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 == 
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*
_temp1015[ 3u]; _temp1015[ 2u]= Cyc_Absynpp_exp2doc((* t).f2); _temp1015[ 1u]=
Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1015[ 0u]= Cyc_PP_textptr((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v); Cyc_PP_cat(
_tag_arr( _temp1015, sizeof( struct Cyc_PP_Doc*), 3u));});}} int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt* s){ void* _temp1016=( void*) s->r; _LL1018: if((
unsigned int) _temp1016 >  1u?*(( int*) _temp1016) ==  Cyc_Absyn_Decl_s: 0){
goto _LL1019;} else{ goto _LL1020;} _LL1020: goto _LL1021; _LL1019: return 1;
_LL1021: return 0; _LL1017:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1022=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1066; struct Cyc_Absyn_Stmt* _temp1068; struct Cyc_Absyn_Stmt*
_temp1070; struct Cyc_Absyn_Exp* _temp1072; struct Cyc_Absyn_Stmt* _temp1074;
struct Cyc_Absyn_Stmt* _temp1076; struct Cyc_Absyn_Exp* _temp1078; struct Cyc_Absyn_Stmt*
_temp1080; struct _tuple2 _temp1082; struct Cyc_Absyn_Exp* _temp1084; struct
_tagged_arr* _temp1086; struct Cyc_Absyn_Stmt* _temp1088; struct _tuple2
_temp1090; struct Cyc_Absyn_Exp* _temp1092; struct _tuple2 _temp1094; struct Cyc_Absyn_Exp*
_temp1096; struct Cyc_Absyn_Exp* _temp1098; struct Cyc_List_List* _temp1100;
struct Cyc_Absyn_Exp* _temp1102; struct Cyc_List_List* _temp1104; struct Cyc_Absyn_Exp*
_temp1106; struct Cyc_List_List* _temp1108; struct Cyc_List_List* _temp1110;
struct Cyc_Absyn_Stmt* _temp1112; struct Cyc_Absyn_Decl* _temp1114; struct Cyc_Absyn_Stmt*
_temp1116; struct Cyc_Absyn_Stmt* _temp1118; struct Cyc_Absyn_Stmt* _temp1120;
struct _tagged_arr* _temp1122; struct _tuple2 _temp1124; struct Cyc_Absyn_Exp*
_temp1126; struct Cyc_Absyn_Stmt* _temp1128; struct Cyc_List_List* _temp1130;
struct Cyc_Absyn_Stmt* _temp1132; struct Cyc_Absyn_Stmt* _temp1134; struct Cyc_Absyn_Vardecl*
_temp1136; struct Cyc_Absyn_Tvar* _temp1138; _LL1024: if( _temp1022 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1025;} else{ goto _LL1026;} _LL1026: if((
unsigned int) _temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_Exp_s: 0){
_LL1067: _temp1066=(( struct Cyc_Absyn_Exp_s_struct*) _temp1022)->f1; goto
_LL1027;} else{ goto _LL1028;} _LL1028: if(( unsigned int) _temp1022 >  1u?*((
int*) _temp1022) ==  Cyc_Absyn_Seq_s: 0){ _LL1071: _temp1070=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1022)->f1; goto _LL1069; _LL1069: _temp1068=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1022)->f2; goto _LL1029;} else{ goto _LL1030;} _LL1030: if(( unsigned int)
_temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_Return_s: 0){ _LL1073:
_temp1072=(( struct Cyc_Absyn_Return_s_struct*) _temp1022)->f1; goto _LL1031;}
else{ goto _LL1032;} _LL1032: if(( unsigned int) _temp1022 >  1u?*(( int*)
_temp1022) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1079: _temp1078=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1022)->f1; goto _LL1077; _LL1077: _temp1076=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1022)->f2; goto _LL1075; _LL1075: _temp1074=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1022)->f3; goto _LL1033;} else{ goto _LL1034;} _LL1034: if(( unsigned int)
_temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_While_s: 0){ _LL1083:
_temp1082=(( struct Cyc_Absyn_While_s_struct*) _temp1022)->f1; _LL1085:
_temp1084= _temp1082.f1; goto _LL1081; _LL1081: _temp1080=(( struct Cyc_Absyn_While_s_struct*)
_temp1022)->f2; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(( unsigned int)
_temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_Break_s: 0){ goto _LL1037;}
else{ goto _LL1038;} _LL1038: if(( unsigned int) _temp1022 >  1u?*(( int*)
_temp1022) ==  Cyc_Absyn_Continue_s: 0){ goto _LL1039;} else{ goto _LL1040;}
_LL1040: if(( unsigned int) _temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_Goto_s:
0){ _LL1087: _temp1086=(( struct Cyc_Absyn_Goto_s_struct*) _temp1022)->f1; goto
_LL1041;} else{ goto _LL1042;} _LL1042: if(( unsigned int) _temp1022 >  1u?*((
int*) _temp1022) ==  Cyc_Absyn_For_s: 0){ _LL1099: _temp1098=(( struct Cyc_Absyn_For_s_struct*)
_temp1022)->f1; goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_For_s_struct*)
_temp1022)->f2; _LL1097: _temp1096= _temp1094.f1; goto _LL1091; _LL1091:
_temp1090=(( struct Cyc_Absyn_For_s_struct*) _temp1022)->f3; _LL1093: _temp1092=
_temp1090.f1; goto _LL1089; _LL1089: _temp1088=(( struct Cyc_Absyn_For_s_struct*)
_temp1022)->f4; goto _LL1043;} else{ goto _LL1044;} _LL1044: if(( unsigned int)
_temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_Switch_s: 0){ _LL1103:
_temp1102=(( struct Cyc_Absyn_Switch_s_struct*) _temp1022)->f1; goto _LL1101;
_LL1101: _temp1100=(( struct Cyc_Absyn_Switch_s_struct*) _temp1022)->f2; goto
_LL1045;} else{ goto _LL1046;} _LL1046: if(( unsigned int) _temp1022 >  1u?*((
int*) _temp1022) ==  Cyc_Absyn_SwitchC_s: 0){ _LL1107: _temp1106=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1022)->f1; goto _LL1105; _LL1105: _temp1104=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1022)->f2; goto _LL1047;} else{ goto _LL1048;} _LL1048: if(( unsigned int)
_temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1109:
_temp1108=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1022)->f1; if( _temp1108
==  0){ goto _LL1049;} else{ goto _LL1050;}} else{ goto _LL1050;} _LL1050: if((
unsigned int) _temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_Fallthru_s: 0){
_LL1111: _temp1110=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1022)->f1; goto
_LL1051;} else{ goto _LL1052;} _LL1052: if(( unsigned int) _temp1022 >  1u?*((
int*) _temp1022) ==  Cyc_Absyn_Decl_s: 0){ _LL1115: _temp1114=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1022)->f1; goto _LL1113; _LL1113: _temp1112=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1022)->f2; goto _LL1053;} else{ goto _LL1054;} _LL1054: if(( unsigned int)
_temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_Cut_s: 0){ _LL1117: _temp1116=((
struct Cyc_Absyn_Cut_s_struct*) _temp1022)->f1; goto _LL1055;} else{ goto
_LL1056;} _LL1056: if(( unsigned int) _temp1022 >  1u?*(( int*) _temp1022) == 
Cyc_Absyn_Splice_s: 0){ _LL1119: _temp1118=(( struct Cyc_Absyn_Splice_s_struct*)
_temp1022)->f1; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(( unsigned int)
_temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_Label_s: 0){ _LL1123:
_temp1122=(( struct Cyc_Absyn_Label_s_struct*) _temp1022)->f1; goto _LL1121;
_LL1121: _temp1120=(( struct Cyc_Absyn_Label_s_struct*) _temp1022)->f2; goto
_LL1059;} else{ goto _LL1060;} _LL1060: if(( unsigned int) _temp1022 >  1u?*((
int*) _temp1022) ==  Cyc_Absyn_Do_s: 0){ _LL1129: _temp1128=(( struct Cyc_Absyn_Do_s_struct*)
_temp1022)->f1; goto _LL1125; _LL1125: _temp1124=(( struct Cyc_Absyn_Do_s_struct*)
_temp1022)->f2; _LL1127: _temp1126= _temp1124.f1; goto _LL1061;} else{ goto
_LL1062;} _LL1062: if(( unsigned int) _temp1022 >  1u?*(( int*) _temp1022) == 
Cyc_Absyn_TryCatch_s: 0){ _LL1133: _temp1132=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1022)->f1; goto _LL1131; _LL1131: _temp1130=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1022)->f2; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(( unsigned int)
_temp1022 >  1u?*(( int*) _temp1022) ==  Cyc_Absyn_Region_s: 0){ _LL1139:
_temp1138=(( struct Cyc_Absyn_Region_s_struct*) _temp1022)->f1; goto _LL1137;
_LL1137: _temp1136=(( struct Cyc_Absyn_Region_s_struct*) _temp1022)->f2; goto
_LL1135; _LL1135: _temp1134=(( struct Cyc_Absyn_Region_s_struct*) _temp1022)->f3;
goto _LL1065;} else{ goto _LL1023;} _LL1025: s= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); goto _LL1023; _LL1027: s=({ struct Cyc_PP_Doc*
_temp1140[ 2u]; _temp1140[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1140[ 0u]= Cyc_Absynpp_exp2doc( _temp1066); Cyc_PP_cat( _tag_arr(
_temp1140, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1023; _LL1029: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1070)){ s=({ struct Cyc_PP_Doc* _temp1141[
6u]; _temp1141[ 5u]= Cyc_Absynpp_is_declaration( _temp1068)?({ struct Cyc_PP_Doc*
_temp1142[ 4u]; _temp1142[ 3u]= Cyc_PP_line_doc(); _temp1142[ 2u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1142[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1068)); _temp1142[ 0u]= Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1142, sizeof( struct Cyc_PP_Doc*), 4u));}): Cyc_Absynpp_stmt2doc(
_temp1068); _temp1141[ 4u]= Cyc_PP_line_doc(); _temp1141[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1141[ 2u]= Cyc_PP_line_doc();
_temp1141[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1070)); _temp1141[ 0u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr(
_temp1141, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ if( Cyc_Absynpp_is_declaration(
_temp1068)){ s=({ struct Cyc_PP_Doc* _temp1143[ 6u]; _temp1143[ 5u]= Cyc_PP_line_doc();
_temp1143[ 4u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1143[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1068)); _temp1143[ 2u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); _temp1143[ 1u]= Cyc_PP_line_doc();
_temp1143[ 0u]= Cyc_Absynpp_stmt2doc( _temp1070); Cyc_PP_cat( _tag_arr(
_temp1143, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1144=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1144->hd=( void*) _temp1070;
_temp1144->tl=({ struct Cyc_List_List* _temp1145=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1145->hd=( void*) _temp1068;
_temp1145->tl= 0; _temp1145;}); _temp1144;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1146=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1146->hd=( void*) _temp1070;
_temp1146->tl=({ struct Cyc_List_List* _temp1147=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1147->hd=( void*) _temp1068;
_temp1147->tl= 0; _temp1147;}); _temp1146;}));} goto _LL1023; _LL1031: if(
_temp1072 ==  0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc* _temp1148[ 3u]; _temp1148[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1148[ 1u]= _temp1072 ==  0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1072)); _temp1148[
0u]= Cyc_PP_text( _tag_arr("return ", sizeof( unsigned char), 8u)); Cyc_PP_cat(
_tag_arr( _temp1148, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1023;
_LL1033: { int print_else;{ void* _temp1149=( void*) _temp1074->r; _LL1151: if(
_temp1149 == ( void*) Cyc_Absyn_Skip_s){ goto _LL1152;} else{ goto _LL1153;}
_LL1153: goto _LL1154; _LL1152: print_else= 0; goto _LL1150; _LL1154: print_else=
1; goto _LL1150; _LL1150:;} s=({ struct Cyc_PP_Doc* _temp1155[ 7u]; _temp1155[ 6u]=
print_else?({ struct Cyc_PP_Doc* _temp1157[ 5u]; _temp1157[ 4u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1157[ 3u]= Cyc_PP_line_doc();
_temp1157[ 2u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1158[ 2u]; _temp1158[
1u]= Cyc_Absynpp_stmt2doc( _temp1074); _temp1158[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1158, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1157[ 1u]= Cyc_PP_text(
_tag_arr("else {", sizeof( unsigned char), 7u)); _temp1157[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1157, sizeof( struct Cyc_PP_Doc*), 5u));}): Cyc_PP_nil_doc();
_temp1155[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1155[ 4u]= Cyc_PP_line_doc(); _temp1155[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1156[ 2u]; _temp1156[ 1u]= Cyc_Absynpp_stmt2doc( _temp1076); _temp1156[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1156, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1155[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1155[ 1u]= Cyc_Absynpp_exp2doc( _temp1078); _temp1155[ 0u]= Cyc_PP_text(
_tag_arr("if (", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1155,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1023;} _LL1035: s=({ struct Cyc_PP_Doc*
_temp1159[ 6u]; _temp1159[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1159[ 4u]= Cyc_PP_line_doc(); _temp1159[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1160[ 2u]; _temp1160[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1080); _temp1160[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1160,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1159[ 2u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1159[ 1u]= Cyc_Absynpp_exp2doc( _temp1084);
_temp1159[ 0u]= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp1159, sizeof( struct Cyc_PP_Doc*), 6u));}); goto
_LL1023; _LL1037: s= Cyc_PP_text( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL1023; _LL1039: s= Cyc_PP_text( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL1023; _LL1041: s= Cyc_PP_text(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp1162; _temp1162.tag= Cyc_Std_String_pa;
_temp1162.f1=( struct _tagged_arr)* _temp1086;{ void* _temp1161[ 1u]={&
_temp1162}; Cyc_Std_aprintf( _tag_arr("goto %s;", sizeof( unsigned char), 9u),
_tag_arr( _temp1161, sizeof( void*), 1u));}})); goto _LL1023; _LL1043: s=({
struct Cyc_PP_Doc* _temp1163[ 10u]; _temp1163[ 9u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1163[ 8u]= Cyc_PP_line_doc(); _temp1163[ 7u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1164[ 2u]; _temp1164[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1088); _temp1164[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1164,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1163[ 6u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1163[ 5u]= Cyc_Absynpp_exp2doc( _temp1092);
_temp1163[ 4u]= Cyc_PP_text( _tag_arr("; ", sizeof( unsigned char), 3u));
_temp1163[ 3u]= Cyc_Absynpp_exp2doc( _temp1096); _temp1163[ 2u]= Cyc_PP_text(
_tag_arr("; ", sizeof( unsigned char), 3u)); _temp1163[ 1u]= Cyc_Absynpp_exp2doc(
_temp1098); _temp1163[ 0u]= Cyc_PP_text( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1163, sizeof( struct Cyc_PP_Doc*), 10u));});
goto _LL1023; _LL1045: s=({ struct Cyc_PP_Doc* _temp1165[ 7u]; _temp1165[ 6u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1165[ 5u]= Cyc_PP_line_doc();
_temp1165[ 4u]= Cyc_Absynpp_switchclauses2doc( _temp1100); _temp1165[ 3u]= Cyc_PP_line_doc();
_temp1165[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1165[ 1u]= Cyc_Absynpp_exp2doc( _temp1102); _temp1165[ 0u]= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp1165, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1023; _LL1047: s=({
struct Cyc_PP_Doc* _temp1166[ 7u]; _temp1166[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1166[ 5u]= Cyc_PP_line_doc(); _temp1166[ 4u]=
Cyc_Absynpp_switchCclauses2doc( _temp1104); _temp1166[ 3u]= Cyc_PP_line_doc();
_temp1166[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1166[ 1u]= Cyc_Absynpp_exp2doc( _temp1106); _temp1166[ 0u]= Cyc_PP_text(
_tag_arr("switch \"C\" (", sizeof( unsigned char), 13u)); Cyc_PP_cat( _tag_arr(
_temp1166, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1023; _LL1049: s= Cyc_PP_text(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL1023; _LL1051: s=({
struct Cyc_PP_Doc* _temp1167[ 3u]; _temp1167[ 2u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1167[ 1u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp1110); _temp1167[ 0u]= Cyc_PP_text( _tag_arr("fallthru(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1167, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL1023; _LL1053: s=({ struct Cyc_PP_Doc* _temp1168[ 3u];
_temp1168[ 2u]= Cyc_Absynpp_stmt2doc( _temp1112); _temp1168[ 1u]= Cyc_PP_line_doc();
_temp1168[ 0u]= Cyc_Absynpp_decl2doc( _temp1114); Cyc_PP_cat( _tag_arr(
_temp1168, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1023; _LL1055: s=({
struct Cyc_PP_Doc* _temp1169[ 2u]; _temp1169[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1116)); _temp1169[ 0u]= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1169, sizeof( struct Cyc_PP_Doc*), 2u));});
goto _LL1023; _LL1057: s=({ struct Cyc_PP_Doc* _temp1170[ 2u]; _temp1170[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1118)); _temp1170[ 0u]= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr(
_temp1170, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1023; _LL1059: if( Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration( _temp1120): 0){ s=({ struct Cyc_PP_Doc* _temp1171[ 6u];
_temp1171[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1171[ 4u]= Cyc_PP_line_doc(); _temp1171[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1120)); _temp1171[ 2u]= Cyc_PP_line_doc(); _temp1171[ 1u]= Cyc_PP_text(
_tag_arr(": {", sizeof( unsigned char), 4u)); _temp1171[ 0u]= Cyc_PP_textptr(
_temp1122); Cyc_PP_cat( _tag_arr( _temp1171, sizeof( struct Cyc_PP_Doc*), 6u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1172[ 3u]; _temp1172[ 2u]= Cyc_Absynpp_stmt2doc(
_temp1120); _temp1172[ 1u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); _temp1172[ 0u]= Cyc_PP_textptr( _temp1122); Cyc_PP_cat( _tag_arr(
_temp1172, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1023; _LL1061: s=({
struct Cyc_PP_Doc* _temp1173[ 7u]; _temp1173[ 6u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1173[ 5u]= Cyc_Absynpp_exp2doc( _temp1126);
_temp1173[ 4u]= Cyc_PP_text( _tag_arr("} while (", sizeof( unsigned char), 10u));
_temp1173[ 3u]= Cyc_PP_line_doc(); _temp1173[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1128)); _temp1173[ 1u]= Cyc_PP_line_doc(); _temp1173[ 0u]= Cyc_PP_text(
_tag_arr("do {", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1173,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL1023; _LL1063: s=({ struct Cyc_PP_Doc*
_temp1174[ 9u]; _temp1174[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1174[ 7u]= Cyc_PP_line_doc(); _temp1174[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1130)); _temp1174[ 5u]= Cyc_PP_line_doc(); _temp1174[ 4u]= Cyc_PP_text(
_tag_arr("} catch {", sizeof( unsigned char), 10u)); _temp1174[ 3u]= Cyc_PP_line_doc();
_temp1174[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1132)); _temp1174[ 1u]=
Cyc_PP_line_doc(); _temp1174[ 0u]= Cyc_PP_text( _tag_arr("try {", sizeof(
unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1174, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL1023; _LL1065: s=({ struct Cyc_PP_Doc* _temp1175[ 9u];
_temp1175[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1175[ 7u]= Cyc_PP_line_doc(); _temp1175[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1134)); _temp1175[ 5u]= Cyc_PP_line_doc(); _temp1175[ 4u]= Cyc_PP_text(
_tag_arr("{", sizeof( unsigned char), 2u)); _temp1175[ 3u]= Cyc_Absynpp_qvar2doc(
_temp1136->name); _temp1175[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); _temp1175[ 1u]= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1138)); _temp1175[ 0u]= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp1175, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL1023; _LL1023:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1176=( void*) p->r;
int _temp1210; void* _temp1212; unsigned char _temp1214; struct _tagged_arr
_temp1216; struct Cyc_Absyn_Vardecl* _temp1218; struct Cyc_List_List* _temp1220;
struct Cyc_Absyn_Pat* _temp1222; struct Cyc_Absyn_Vardecl* _temp1224; struct
_tuple0* _temp1226; struct Cyc_List_List* _temp1228; struct Cyc_List_List*
_temp1230; struct _tuple0* _temp1232; struct Cyc_List_List* _temp1234; struct
Cyc_List_List* _temp1236; struct _tuple0* _temp1238; struct Cyc_List_List*
_temp1240; struct Cyc_List_List* _temp1242; struct Cyc_Absyn_Structdecl*
_temp1244; struct Cyc_Absyn_Enumfield* _temp1246; struct Cyc_Absyn_Enumfield*
_temp1248; struct Cyc_List_List* _temp1250; struct Cyc_List_List* _temp1252;
struct Cyc_Absyn_Tunionfield* _temp1254; _LL1178: if( _temp1176 == ( void*) Cyc_Absyn_Wild_p){
goto _LL1179;} else{ goto _LL1180;} _LL1180: if( _temp1176 == ( void*) Cyc_Absyn_Null_p){
goto _LL1181;} else{ goto _LL1182;} _LL1182: if(( unsigned int) _temp1176 >  2u?*((
int*) _temp1176) ==  Cyc_Absyn_Int_p: 0){ _LL1213: _temp1212=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1176)->f1; goto _LL1211; _LL1211: _temp1210=((
struct Cyc_Absyn_Int_p_struct*) _temp1176)->f2; goto _LL1183;} else{ goto
_LL1184;} _LL1184: if(( unsigned int) _temp1176 >  2u?*(( int*) _temp1176) == 
Cyc_Absyn_Char_p: 0){ _LL1215: _temp1214=(( struct Cyc_Absyn_Char_p_struct*)
_temp1176)->f1; goto _LL1185;} else{ goto _LL1186;} _LL1186: if(( unsigned int)
_temp1176 >  2u?*(( int*) _temp1176) ==  Cyc_Absyn_Float_p: 0){ _LL1217:
_temp1216=(( struct Cyc_Absyn_Float_p_struct*) _temp1176)->f1; goto _LL1187;}
else{ goto _LL1188;} _LL1188: if(( unsigned int) _temp1176 >  2u?*(( int*)
_temp1176) ==  Cyc_Absyn_Var_p: 0){ _LL1219: _temp1218=(( struct Cyc_Absyn_Var_p_struct*)
_temp1176)->f1; goto _LL1189;} else{ goto _LL1190;} _LL1190: if(( unsigned int)
_temp1176 >  2u?*(( int*) _temp1176) ==  Cyc_Absyn_Tuple_p: 0){ _LL1221:
_temp1220=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1176)->f1; goto _LL1191;}
else{ goto _LL1192;} _LL1192: if(( unsigned int) _temp1176 >  2u?*(( int*)
_temp1176) ==  Cyc_Absyn_Pointer_p: 0){ _LL1223: _temp1222=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1176)->f1; goto _LL1193;} else{ goto _LL1194;} _LL1194: if(( unsigned int)
_temp1176 >  2u?*(( int*) _temp1176) ==  Cyc_Absyn_Reference_p: 0){ _LL1225:
_temp1224=(( struct Cyc_Absyn_Reference_p_struct*) _temp1176)->f1; goto _LL1195;}
else{ goto _LL1196;} _LL1196: if(( unsigned int) _temp1176 >  2u?*(( int*)
_temp1176) ==  Cyc_Absyn_UnknownId_p: 0){ _LL1227: _temp1226=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1176)->f1; goto _LL1197;} else{ goto _LL1198;} _LL1198: if(( unsigned int)
_temp1176 >  2u?*(( int*) _temp1176) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL1233:
_temp1232=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1176)->f1; goto
_LL1231; _LL1231: _temp1230=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1176)->f2;
goto _LL1229; _LL1229: _temp1228=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1176)->f3; goto _LL1199;} else{ goto _LL1200;} _LL1200: if(( unsigned int)
_temp1176 >  2u?*(( int*) _temp1176) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL1239:
_temp1238=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1176)->f1; goto
_LL1237; _LL1237: _temp1236=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1176)->f2; goto _LL1235; _LL1235: _temp1234=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1176)->f3; goto _LL1201;} else{ goto _LL1202;} _LL1202: if(( unsigned int)
_temp1176 >  2u?*(( int*) _temp1176) ==  Cyc_Absyn_Struct_p: 0){ _LL1245:
_temp1244=(( struct Cyc_Absyn_Struct_p_struct*) _temp1176)->f1; goto _LL1243;
_LL1243: _temp1242=(( struct Cyc_Absyn_Struct_p_struct*) _temp1176)->f3; goto
_LL1241; _LL1241: _temp1240=(( struct Cyc_Absyn_Struct_p_struct*) _temp1176)->f4;
goto _LL1203;} else{ goto _LL1204;} _LL1204: if(( unsigned int) _temp1176 >  2u?*((
int*) _temp1176) ==  Cyc_Absyn_Enum_p: 0){ _LL1247: _temp1246=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1176)->f2; goto _LL1205;} else{ goto _LL1206;} _LL1206: if(( unsigned int)
_temp1176 >  2u?*(( int*) _temp1176) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL1249:
_temp1248=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp1176)->f2; goto _LL1207;}
else{ goto _LL1208;} _LL1208: if(( unsigned int) _temp1176 >  2u?*(( int*)
_temp1176) ==  Cyc_Absyn_Tunion_p: 0){ _LL1255: _temp1254=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1176)->f2; goto _LL1253; _LL1253: _temp1252=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1176)->f3; goto _LL1251; _LL1251: _temp1250=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1176)->f4; goto _LL1209;} else{ goto _LL1177;} _LL1179: s= Cyc_PP_text(
_tag_arr("_", sizeof( unsigned char), 2u)); goto _LL1177; _LL1181: s= Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); goto _LL1177; _LL1183: if(
_temp1212 == ( void*) Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp1257; _temp1257.tag= Cyc_Std_Int_pa; _temp1257.f1=(
int)(( unsigned int) _temp1210);{ void* _temp1256[ 1u]={& _temp1257}; Cyc_Std_aprintf(
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp1256, sizeof( void*),
1u));}}));} else{ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp1259; _temp1259.tag= Cyc_Std_Int_pa; _temp1259.f1=( unsigned int) _temp1210;{
void* _temp1258[ 1u]={& _temp1259}; Cyc_Std_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp1258, sizeof( void*), 1u));}}));} goto
_LL1177; _LL1185: s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1261; _temp1261.tag= Cyc_Std_String_pa; _temp1261.f1=( struct _tagged_arr)
Cyc_Absynpp_char_escape( _temp1214);{ void* _temp1260[ 1u]={& _temp1261}; Cyc_Std_aprintf(
_tag_arr("'%s'", sizeof( unsigned char), 5u), _tag_arr( _temp1260, sizeof( void*),
1u));}})); goto _LL1177; _LL1187: s= Cyc_PP_text( _temp1216); goto _LL1177;
_LL1189: s= Cyc_Absynpp_qvar2doc( _temp1218->name); goto _LL1177; _LL1191: s=({
struct Cyc_PP_Doc* _temp1262[ 3u]; _temp1262[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp1262[ 1u]=(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc, _tag_arr(",", sizeof( unsigned char), 2u),
_temp1220); _temp1262[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1262, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1177; _LL1193: s=({ struct Cyc_PP_Doc* _temp1263[ 2u]; _temp1263[ 1u]=
Cyc_Absynpp_pat2doc( _temp1222); _temp1263[ 0u]= Cyc_PP_text( _tag_arr("&",
sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1263, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL1177; _LL1195: s=({ struct Cyc_PP_Doc* _temp1264[
2u]; _temp1264[ 1u]= Cyc_Absynpp_qvar2doc( _temp1224->name); _temp1264[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1264,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1177; _LL1197: s= Cyc_Absynpp_qvar2doc(
_temp1226); goto _LL1177; _LL1199: s=({ struct Cyc_PP_Doc* _temp1265[ 3u];
_temp1265[ 2u]= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1228));
_temp1265[ 1u]= Cyc_Absynpp_tvars2doc( _temp1230); _temp1265[ 0u]= Cyc_Absynpp_qvar2doc(
_temp1232); Cyc_PP_cat( _tag_arr( _temp1265, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1177; _LL1201: s=({ struct Cyc_PP_Doc* _temp1266[ 3u]; _temp1266[ 2u]=
Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1234)); _temp1266[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1236); _temp1266[ 0u]= Cyc_Absynpp_qvar2doc( _temp1238); Cyc_PP_cat(
_tag_arr( _temp1266, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1177; _LL1203:
s=({ struct Cyc_PP_Doc* _temp1267[ 3u]; _temp1267[ 2u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1240)); _temp1267[ 1u]= Cyc_Absynpp_tvars2doc( _temp1242); _temp1267[ 0u]=
_temp1244->name ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1244->name))->v); Cyc_PP_cat( _tag_arr(
_temp1267, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1177; _LL1205: s= Cyc_Absynpp_qvar2doc(
_temp1246->name); goto _LL1177; _LL1207: s= Cyc_Absynpp_qvar2doc( _temp1248->name);
goto _LL1177; _LL1209: if( _temp1250 ==  0){ s= Cyc_Absynpp_qvar2doc( _temp1254->name);}
else{ s=({ struct Cyc_PP_Doc* _temp1268[ 3u]; _temp1268[ 2u]= Cyc_PP_egroup(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1250)); _temp1268[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1252); _temp1268[ 0u]= Cyc_Absynpp_qvar2doc( _temp1254->name); Cyc_PP_cat(
_tag_arr( _temp1268, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1177;
_LL1177:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc* _temp1269[ 2u]; _temp1269[ 1u]= Cyc_Absynpp_pat2doc((*
dp).f2); _temp1269[ 0u]= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)); Cyc_PP_cat( _tag_arr(
_temp1269, sizeof( struct Cyc_PP_Doc*), 2u));});} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause ==  0?( void*)( c->pattern)->r
== ( void*) Cyc_Absyn_Wild_p: 0){ return({ struct Cyc_PP_Doc* _temp1270[ 2u];
_temp1270[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1271[ 2u]; _temp1271[
1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1271[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1271, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1270[ 0u]= Cyc_PP_text(
_tag_arr("default: ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1270, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ if( c->where_clause == 
0){ return({ struct Cyc_PP_Doc* _temp1272[ 4u]; _temp1272[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1273[ 2u]; _temp1273[ 1u]= Cyc_Absynpp_stmt2doc( c->body);
_temp1273[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1273, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1272[ 2u]= Cyc_PP_text( _tag_arr(": ",
sizeof( unsigned char), 3u)); _temp1272[ 1u]= Cyc_Absynpp_pat2doc( c->pattern);
_temp1272[ 0u]= Cyc_PP_text( _tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp1272, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ return({
struct Cyc_PP_Doc* _temp1274[ 6u]; _temp1274[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1275[ 2u]; _temp1275[ 1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1275[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1275, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1274[ 4u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u));
_temp1274[ 3u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( c->where_clause));
_temp1274[ 2u]= Cyc_PP_text( _tag_arr(" && ", sizeof( unsigned char), 5u));
_temp1274[ 1u]= Cyc_Absynpp_pat2doc( c->pattern); _temp1274[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1274,
sizeof( struct Cyc_PP_Doc*), 6u));});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp1278;
struct Cyc_Absyn_Stmt* _temp1279; struct Cyc_Absyn_Exp* _temp1281; struct Cyc_Absyn_SwitchC_clause*
_temp1276= c; _temp1278=* _temp1276; _LL1282: _temp1281= _temp1278.cnst_exp;
goto _LL1280; _LL1280: _temp1279= _temp1278.body; goto _LL1277; _LL1277: if(
_temp1281 ==  0){ return({ struct Cyc_PP_Doc* _temp1283[ 2u]; _temp1283[ 1u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1284[ 2u]; _temp1284[ 1u]= Cyc_Absynpp_stmt2doc(
c->body); _temp1284[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1284,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1283[ 0u]= Cyc_PP_text( _tag_arr("default: ",
sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1283, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp1285[ 4u];
_temp1285[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1286[ 2u]; _temp1286[
1u]= Cyc_Absynpp_stmt2doc( _temp1279); _temp1286[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1286, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1285[ 2u]= Cyc_PP_text(
_tag_arr(": ", sizeof( unsigned char), 3u)); _temp1285[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1281)); _temp1285[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1285,
sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_SwitchC_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct
Cyc_Absyn_Enumfield* f){ if( f->tag ==  0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc* _temp1287[ 3u]; _temp1287[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->tag)); _temp1287[ 1u]= Cyc_PP_text(
_tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1287[ 0u]= Cyc_Absynpp_qvar2doc(
f->name); Cyc_PP_cat( _tag_arr( _temp1287, sizeof( struct Cyc_PP_Doc*), 3u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,
_tag_arr(",", sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp1288=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp1314; struct Cyc_Absyn_Structdecl* _temp1316;
struct Cyc_Absyn_Uniondecl* _temp1318; struct Cyc_Absyn_Vardecl* _temp1320;
struct Cyc_Absyn_Vardecl _temp1322; struct Cyc_List_List* _temp1323; struct Cyc_Absyn_Exp*
_temp1325; void* _temp1327; struct Cyc_Absyn_Tqual _temp1329; struct _tuple0*
_temp1331; void* _temp1333; struct Cyc_Absyn_Tuniondecl* _temp1335; struct Cyc_Absyn_Tuniondecl
_temp1337; int _temp1338; struct Cyc_Core_Opt* _temp1340; struct Cyc_List_List*
_temp1342; struct _tuple0* _temp1344; void* _temp1346; struct Cyc_Absyn_Exp*
_temp1348; struct Cyc_Absyn_Pat* _temp1350; struct Cyc_List_List* _temp1352;
struct Cyc_Absyn_Enumdecl* _temp1354; struct Cyc_Absyn_Enumdecl _temp1356;
struct Cyc_Core_Opt* _temp1357; struct _tuple0* _temp1359; void* _temp1361;
struct Cyc_Absyn_Typedefdecl* _temp1363; struct Cyc_List_List* _temp1365; struct
_tagged_arr* _temp1367; struct Cyc_List_List* _temp1369; struct _tuple0*
_temp1371; struct Cyc_List_List* _temp1373; _LL1290: if(*(( int*) _temp1288) == 
Cyc_Absyn_Fn_d){ _LL1315: _temp1314=(( struct Cyc_Absyn_Fn_d_struct*) _temp1288)->f1;
goto _LL1291;} else{ goto _LL1292;} _LL1292: if(*(( int*) _temp1288) ==  Cyc_Absyn_Struct_d){
_LL1317: _temp1316=(( struct Cyc_Absyn_Struct_d_struct*) _temp1288)->f1; goto
_LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*) _temp1288) ==  Cyc_Absyn_Union_d){
_LL1319: _temp1318=(( struct Cyc_Absyn_Union_d_struct*) _temp1288)->f1; goto
_LL1295;} else{ goto _LL1296;} _LL1296: if(*(( int*) _temp1288) ==  Cyc_Absyn_Var_d){
_LL1321: _temp1320=(( struct Cyc_Absyn_Var_d_struct*) _temp1288)->f1; _temp1322=*
_temp1320; _LL1334: _temp1333=( void*) _temp1322.sc; goto _LL1332; _LL1332:
_temp1331= _temp1322.name; goto _LL1330; _LL1330: _temp1329= _temp1322.tq; goto
_LL1328; _LL1328: _temp1327=( void*) _temp1322.type; goto _LL1326; _LL1326:
_temp1325= _temp1322.initializer; goto _LL1324; _LL1324: _temp1323= _temp1322.attributes;
goto _LL1297;} else{ goto _LL1298;} _LL1298: if(*(( int*) _temp1288) ==  Cyc_Absyn_Tunion_d){
_LL1336: _temp1335=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1288)->f1;
_temp1337=* _temp1335; _LL1347: _temp1346=( void*) _temp1337.sc; goto _LL1345;
_LL1345: _temp1344= _temp1337.name; goto _LL1343; _LL1343: _temp1342= _temp1337.tvs;
goto _LL1341; _LL1341: _temp1340= _temp1337.fields; goto _LL1339; _LL1339:
_temp1338= _temp1337.is_xtunion; goto _LL1299;} else{ goto _LL1300;} _LL1300:
if(*(( int*) _temp1288) ==  Cyc_Absyn_Let_d){ _LL1351: _temp1350=(( struct Cyc_Absyn_Let_d_struct*)
_temp1288)->f1; goto _LL1349; _LL1349: _temp1348=(( struct Cyc_Absyn_Let_d_struct*)
_temp1288)->f4; goto _LL1301;} else{ goto _LL1302;} _LL1302: if(*(( int*)
_temp1288) ==  Cyc_Absyn_Letv_d){ _LL1353: _temp1352=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1288)->f1; goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*)
_temp1288) ==  Cyc_Absyn_Enum_d){ _LL1355: _temp1354=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1288)->f1; _temp1356=* _temp1354; _LL1362: _temp1361=( void*) _temp1356.sc;
goto _LL1360; _LL1360: _temp1359= _temp1356.name; goto _LL1358; _LL1358:
_temp1357= _temp1356.fields; goto _LL1305;} else{ goto _LL1306;} _LL1306: if(*((
int*) _temp1288) ==  Cyc_Absyn_Typedef_d){ _LL1364: _temp1363=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1288)->f1; goto _LL1307;} else{ goto _LL1308;} _LL1308: if(*(( int*)
_temp1288) ==  Cyc_Absyn_Namespace_d){ _LL1368: _temp1367=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1288)->f1; goto _LL1366; _LL1366: _temp1365=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1288)->f2; goto _LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*)
_temp1288) ==  Cyc_Absyn_Using_d){ _LL1372: _temp1371=(( struct Cyc_Absyn_Using_d_struct*)
_temp1288)->f1; goto _LL1370; _LL1370: _temp1369=(( struct Cyc_Absyn_Using_d_struct*)
_temp1288)->f2; goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*)
_temp1288) ==  Cyc_Absyn_ExternC_d){ _LL1374: _temp1373=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1288)->f1; goto _LL1313;} else{ goto _LL1289;} _LL1291: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp1382=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1382[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1383; _temp1383.tag= Cyc_Absyn_FnType; _temp1383.f1=({ struct Cyc_Absyn_FnInfo
_temp1384; _temp1384.tvars= _temp1314->tvs; _temp1384.effect= _temp1314->effect;
_temp1384.ret_typ=( void*)(( void*) _temp1314->ret_type); _temp1384.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1314->args); _temp1384.c_varargs=
_temp1314->c_varargs; _temp1384.cyc_varargs= _temp1314->cyc_varargs; _temp1384.rgn_po=
_temp1314->rgn_po; _temp1384.attributes= 0; _temp1384;}); _temp1383;});
_temp1382;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1314->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp1314->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp1314->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp1314->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp1314->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1379; _temp1379.q_const= 0; _temp1379.q_volatile= 0; _temp1379.q_restrict=
0; _temp1379;}), t,({ struct Cyc_Core_Opt* _temp1380=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1380->v=( void*)({ struct Cyc_PP_Doc*
_temp1381[ 2u]; _temp1381[ 1u]= namedoc; _temp1381[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1381, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1380;})); struct
Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc* _temp1377[ 4u]; _temp1377[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1377[ 2u]= Cyc_PP_line_doc();
_temp1377[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1378[ 2u]; _temp1378[
1u]= Cyc_Absynpp_stmt2doc( _temp1314->body); _temp1378[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1378, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1377[ 0u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp1377, sizeof( struct Cyc_PP_Doc*), 4u));}); s=({ struct Cyc_PP_Doc*
_temp1375[ 4u]; _temp1375[ 3u]= bodydoc; _temp1375[ 2u]= tqtddoc; _temp1375[ 1u]=
scopedoc; _temp1375[ 0u]= inlinedoc; Cyc_PP_cat( _tag_arr( _temp1375, sizeof(
struct Cyc_PP_Doc*), 4u));}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*
_temp1376[ 2u]; _temp1376[ 1u]= s; _temp1376[ 0u]= attsdoc; Cyc_PP_cat( _tag_arr(
_temp1376, sizeof( struct Cyc_PP_Doc*), 2u));});} goto _LL1289;}} _LL1293: if(
_temp1316->fields ==  0){ s=({ struct Cyc_PP_Doc* _temp1385[ 5u]; _temp1385[ 4u]=
Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp1385[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1316->tvs); _temp1385[ 2u]= _temp1316->name ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1316->name))->v); _temp1385[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1385[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1316->sc); Cyc_PP_cat( _tag_arr( _temp1385, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ s=({ struct Cyc_PP_Doc* _temp1386[ 10u]; _temp1386[ 9u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1386[ 8u]= Cyc_Absynpp_atts2doc(
_temp1316->attributes); _temp1386[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); _temp1386[ 6u]= Cyc_PP_line_doc(); _temp1386[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1387[ 2u]; _temp1387[ 1u]= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1316->fields))->v);
_temp1387[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1387, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1386[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1386[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1316->tvs);
_temp1386[ 2u]= _temp1316->name ==  0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1316->name))->v); _temp1386[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1386[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1316->sc); Cyc_PP_cat( _tag_arr( _temp1386, sizeof( struct Cyc_PP_Doc*),
10u));});} goto _LL1289; _LL1295: if( _temp1318->fields ==  0){ s=({ struct Cyc_PP_Doc*
_temp1388[ 5u]; _temp1388[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1388[ 3u]= Cyc_Absynpp_tvars2doc( _temp1318->tvs); _temp1388[ 2u]=
_temp1318->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1318->name))->v); _temp1388[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1388[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1318->sc);
Cyc_PP_cat( _tag_arr( _temp1388, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({
struct Cyc_PP_Doc* _temp1389[ 10u]; _temp1389[ 9u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1389[ 8u]= Cyc_Absynpp_atts2doc( _temp1318->attributes);
_temp1389[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1389[ 6u]= Cyc_PP_line_doc(); _temp1389[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1390[ 2u]; _temp1390[ 1u]= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1318->fields))->v); _temp1390[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1390, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1389[ 4u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1389[ 3u]= Cyc_Absynpp_tvars2doc( _temp1318->tvs); _temp1389[ 2u]=
_temp1318->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1318->name))->v); _temp1389[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1389[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1318->sc);
Cyc_PP_cat( _tag_arr( _temp1389, sizeof( struct Cyc_PP_Doc*), 10u));});} goto
_LL1289; _LL1297: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp1331); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1323); struct
Cyc_PP_Doc* beforenamedoc; if( ! Cyc_Absynpp_to_VC){ beforenamedoc= attsdoc;}
else{ void* _temp1391= Cyc_Tcutil_compress( _temp1327); struct Cyc_Absyn_FnInfo
_temp1397; struct Cyc_List_List* _temp1399; _LL1393: if(( unsigned int)
_temp1391 >  4u?*(( int*) _temp1391) ==  Cyc_Absyn_FnType: 0){ _LL1398:
_temp1397=(( struct Cyc_Absyn_FnType_struct*) _temp1391)->f1; _LL1400: _temp1399=
_temp1397.attributes; goto _LL1394;} else{ goto _LL1395;} _LL1395: goto _LL1396;
_LL1394: beforenamedoc= Cyc_Absynpp_callconv2doc( _temp1399); goto _LL1392;
_LL1396: beforenamedoc= Cyc_PP_nil_doc(); goto _LL1392; _LL1392:;} s=({ struct
Cyc_PP_Doc* _temp1401[ 5u]; _temp1401[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); _temp1401[ 3u]= _temp1325 ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)):({ struct Cyc_PP_Doc* _temp1404[ 2u]; _temp1404[ 1u]=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1325)); _temp1404[
0u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp1404, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1401[ 2u]= Cyc_Absynpp_tqtd2doc(
_temp1329, _temp1327,({ struct Cyc_Core_Opt* _temp1402=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1402->v=( void*)({ struct Cyc_PP_Doc*
_temp1403[ 2u]; _temp1403[ 1u]= sn; _temp1403[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1403, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1402;}));
_temp1401[ 1u]= Cyc_Absynpp_scope2doc( _temp1333); _temp1401[ 0u]= Cyc_Absynpp_to_VC?
attsdoc: Cyc_PP_nil_doc(); Cyc_PP_cat( _tag_arr( _temp1401, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL1289;} _LL1299: if( _temp1340 ==  0){ s=({ struct Cyc_PP_Doc*
_temp1405[ 5u]; _temp1405[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1405[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1342); _temp1405[ 2u]=
_temp1338? Cyc_Absynpp_qvar2doc( _temp1344): Cyc_Absynpp_typedef_name2doc(
_temp1344); _temp1405[ 1u]= _temp1338? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1405[ 0u]= Cyc_Absynpp_scope2doc( _temp1346); Cyc_PP_cat( _tag_arr(
_temp1405, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1406[ 8u]; _temp1406[ 7u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1406[ 6u]= Cyc_PP_line_doc(); _temp1406[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1407[ 2u]; _temp1407[ 1u]= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1340))->v);
_temp1407[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1407, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1406[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1406[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1342);
_temp1406[ 2u]= _temp1338? Cyc_Absynpp_qvar2doc( _temp1344): Cyc_Absynpp_typedef_name2doc(
_temp1344); _temp1406[ 1u]= _temp1338? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1406[ 0u]= Cyc_Absynpp_scope2doc( _temp1346); Cyc_PP_cat( _tag_arr(
_temp1406, sizeof( struct Cyc_PP_Doc*), 8u));});} goto _LL1289; _LL1301: s=({
struct Cyc_PP_Doc* _temp1408[ 5u]; _temp1408[ 4u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1408[ 3u]= Cyc_Absynpp_exp2doc( _temp1348);
_temp1408[ 2u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp1408[ 1u]= Cyc_Absynpp_pat2doc( _temp1350); _temp1408[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1408,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL1289; _LL1303: s=({ struct Cyc_PP_Doc*
_temp1409[ 3u]; _temp1409[ 2u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1409[ 1u]= Cyc_Absynpp_ids2doc( _temp1352); _temp1409[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1409,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1289; _LL1305: if( _temp1357 ==  0){
s=({ struct Cyc_PP_Doc* _temp1410[ 4u]; _temp1410[ 3u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1410[ 2u]= Cyc_Absynpp_typedef_name2doc(
_temp1359); _temp1410[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1410[ 0u]= Cyc_Absynpp_scope2doc( _temp1361); Cyc_PP_cat( _tag_arr(
_temp1410, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1411[ 7u]; _temp1411[ 6u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1411[ 5u]= Cyc_PP_line_doc(); _temp1411[ 4u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1412[ 2u]; _temp1412[ 1u]= Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1357))->v);
_temp1412[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1412, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1411[ 3u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1411[ 2u]= Cyc_Absynpp_qvar2doc( _temp1359);
_temp1411[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u));
_temp1411[ 0u]= Cyc_Absynpp_scope2doc( _temp1361); Cyc_PP_cat( _tag_arr(
_temp1411, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1289; _LL1307: s=({
struct Cyc_PP_Doc* _temp1413[ 3u]; _temp1413[ 2u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1413[ 1u]= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1414; _temp1414.q_const= 0; _temp1414.q_volatile= 0; _temp1414.q_restrict=
0; _temp1414;}),( void*) _temp1363->defn,({ struct Cyc_Core_Opt* _temp1415=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1415->v=(
void*)({ struct Cyc_PP_Doc* _temp1416[ 2u]; _temp1416[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1363->tvs); _temp1416[ 0u]= Cyc_Absynpp_typedef_name2doc( _temp1363->name);
Cyc_PP_cat( _tag_arr( _temp1416, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1415;}));
_temp1413[ 0u]= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
Cyc_PP_cat( _tag_arr( _temp1413, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL1289; _LL1309: if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add(
_temp1367);} s=({ struct Cyc_PP_Doc* _temp1417[ 7u]; _temp1417[ 6u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1417[ 5u]= Cyc_PP_line_doc();
_temp1417[ 4u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1365); _temp1417[ 3u]= Cyc_PP_line_doc();
_temp1417[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1417[ 1u]= Cyc_PP_textptr( _temp1367); _temp1417[ 0u]= Cyc_PP_text(
_tag_arr("namespace ", sizeof( unsigned char), 11u)); Cyc_PP_cat( _tag_arr(
_temp1417, sizeof( struct Cyc_PP_Doc*), 7u));}); if( Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();} goto _LL1289; _LL1311: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc* _temp1418[ 7u]; _temp1418[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1418[ 5u]= Cyc_PP_line_doc(); _temp1418[ 4u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1369); _temp1418[ 3u]= Cyc_PP_line_doc();
_temp1418[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1418[ 1u]= Cyc_Absynpp_qvar2doc( _temp1371); _temp1418[ 0u]= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp1418,
sizeof( struct Cyc_PP_Doc*), 7u));});} else{ s=({ struct Cyc_PP_Doc* _temp1419[
7u]; _temp1419[ 6u]= Cyc_PP_text( _tag_arr("/* } */", sizeof( unsigned char), 8u));
_temp1419[ 5u]= Cyc_PP_line_doc(); _temp1419[ 4u]=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp1369); _temp1419[ 3u]= Cyc_PP_line_doc();
_temp1419[ 2u]= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u));
_temp1419[ 1u]= Cyc_Absynpp_qvar2doc( _temp1371); _temp1419[ 0u]= Cyc_PP_text(
_tag_arr("/* using ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1419, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1289; _LL1313: if( Cyc_Absynpp_print_externC_stmts){
s=({ struct Cyc_PP_Doc* _temp1420[ 5u]; _temp1420[ 4u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1420[ 3u]= Cyc_PP_line_doc(); _temp1420[ 2u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1373); _temp1420[ 1u]= Cyc_PP_line_doc();
_temp1420[ 0u]= Cyc_PP_text( _tag_arr("extern \"C\" {", sizeof( unsigned char),
13u)); Cyc_PP_cat( _tag_arr( _temp1420, sizeof( struct Cyc_PP_Doc*), 5u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1421[ 5u]; _temp1421[ 4u]= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); _temp1421[ 3u]= Cyc_PP_line_doc();
_temp1421[ 2u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1373); _temp1421[ 1u]= Cyc_PP_line_doc();
_temp1421[ 0u]= Cyc_PP_text( _tag_arr("/* extern \"C\" { */", sizeof(
unsigned char), 19u)); Cyc_PP_cat( _tag_arr( _temp1421, sizeof( struct Cyc_PP_Doc*),
5u));});} goto _LL1289; _LL1289:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp1422= sc; _LL1424: if( _temp1422 == ( void*) Cyc_Absyn_Static){
goto _LL1425;} else{ goto _LL1426;} _LL1426: if( _temp1422 == ( void*) Cyc_Absyn_Public){
goto _LL1427;} else{ goto _LL1428;} _LL1428: if( _temp1422 == ( void*) Cyc_Absyn_Extern){
goto _LL1429;} else{ goto _LL1430;} _LL1430: if( _temp1422 == ( void*) Cyc_Absyn_ExternC){
goto _LL1431;} else{ goto _LL1432;} _LL1432: if( _temp1422 == ( void*) Cyc_Absyn_Abstract){
goto _LL1433;} else{ goto _LL1423;} _LL1425: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL1427: return Cyc_PP_nil_doc(); _LL1429: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL1431: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL1433:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL1423:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp1434= t; struct
Cyc_Absyn_Tvar* _temp1442; struct Cyc_List_List* _temp1444; _LL1436: if((
unsigned int) _temp1434 >  4u?*(( int*) _temp1434) ==  Cyc_Absyn_VarType: 0){
_LL1443: _temp1442=(( struct Cyc_Absyn_VarType_struct*) _temp1434)->f1; goto
_LL1437;} else{ goto _LL1438;} _LL1438: if(( unsigned int) _temp1434 >  4u?*((
int*) _temp1434) ==  Cyc_Absyn_JoinEff: 0){ _LL1445: _temp1444=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1434)->f1; goto _LL1439;} else{ goto _LL1440;} _LL1440: goto _LL1441;
_LL1437: return Cyc_Tcutil_is_temp_tvar( _temp1442); _LL1439: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect, _temp1444); _LL1441: return 0; _LL1435:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t){ void*
_temp1446= t; struct Cyc_Absyn_Exp* _temp1460; struct Cyc_Absyn_Tqual _temp1462;
void* _temp1464; struct Cyc_Absyn_PtrInfo _temp1466; struct Cyc_Absyn_Conref*
_temp1468; struct Cyc_Absyn_Tqual _temp1470; struct Cyc_Absyn_Conref* _temp1472;
void* _temp1474; void* _temp1476; struct Cyc_Absyn_FnInfo _temp1478; struct Cyc_List_List*
_temp1480; struct Cyc_List_List* _temp1482; struct Cyc_Absyn_VarargInfo*
_temp1484; int _temp1486; struct Cyc_List_List* _temp1488; void* _temp1490;
struct Cyc_Core_Opt* _temp1492; struct Cyc_List_List* _temp1494; int _temp1496;
struct Cyc_Core_Opt* _temp1498; struct Cyc_Core_Opt* _temp1500; struct Cyc_Core_Opt*
_temp1502; struct Cyc_List_List* _temp1504; struct _tuple0* _temp1506; _LL1448:
if(( unsigned int) _temp1446 >  4u?*(( int*) _temp1446) ==  Cyc_Absyn_ArrayType:
0){ _LL1465: _temp1464=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1446)->f1;
goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1446)->f2; goto _LL1461; _LL1461: _temp1460=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1446)->f3; goto _LL1449;} else{ goto _LL1450;} _LL1450: if(( unsigned int)
_temp1446 >  4u?*(( int*) _temp1446) ==  Cyc_Absyn_PointerType: 0){ _LL1467:
_temp1466=(( struct Cyc_Absyn_PointerType_struct*) _temp1446)->f1; _LL1477:
_temp1476=( void*) _temp1466.elt_typ; goto _LL1475; _LL1475: _temp1474=( void*)
_temp1466.rgn_typ; goto _LL1473; _LL1473: _temp1472= _temp1466.nullable; goto
_LL1471; _LL1471: _temp1470= _temp1466.tq; goto _LL1469; _LL1469: _temp1468=
_temp1466.bounds; goto _LL1451;} else{ goto _LL1452;} _LL1452: if(( unsigned int)
_temp1446 >  4u?*(( int*) _temp1446) ==  Cyc_Absyn_FnType: 0){ _LL1479:
_temp1478=(( struct Cyc_Absyn_FnType_struct*) _temp1446)->f1; _LL1495: _temp1494=
_temp1478.tvars; goto _LL1493; _LL1493: _temp1492= _temp1478.effect; goto
_LL1491; _LL1491: _temp1490=( void*) _temp1478.ret_typ; goto _LL1489; _LL1489:
_temp1488= _temp1478.args; goto _LL1487; _LL1487: _temp1486= _temp1478.c_varargs;
goto _LL1485; _LL1485: _temp1484= _temp1478.cyc_varargs; goto _LL1483; _LL1483:
_temp1482= _temp1478.rgn_po; goto _LL1481; _LL1481: _temp1480= _temp1478.attributes;
goto _LL1453;} else{ goto _LL1454;} _LL1454: if(( unsigned int) _temp1446 >  4u?*((
int*) _temp1446) ==  Cyc_Absyn_Evar: 0){ _LL1501: _temp1500=(( struct Cyc_Absyn_Evar_struct*)
_temp1446)->f1; goto _LL1499; _LL1499: _temp1498=(( struct Cyc_Absyn_Evar_struct*)
_temp1446)->f2; goto _LL1497; _LL1497: _temp1496=(( struct Cyc_Absyn_Evar_struct*)
_temp1446)->f3; goto _LL1455;} else{ goto _LL1456;} _LL1456: if(( unsigned int)
_temp1446 >  4u?*(( int*) _temp1446) ==  Cyc_Absyn_TypedefType: 0){ _LL1507:
_temp1506=(( struct Cyc_Absyn_TypedefType_struct*) _temp1446)->f1; goto _LL1505;
_LL1505: _temp1504=(( struct Cyc_Absyn_TypedefType_struct*) _temp1446)->f2; goto
_LL1503; _LL1503: _temp1502=(( struct Cyc_Absyn_TypedefType_struct*) _temp1446)->f3;
goto _LL1457;} else{ goto _LL1458;} _LL1458: goto _LL1459; _LL1449: { struct Cyc_List_List*
_temp1510; void* _temp1512; struct Cyc_Absyn_Tqual _temp1514; struct _tuple4
_temp1508= Cyc_Absynpp_to_tms( _temp1462, _temp1464); _LL1515: _temp1514=
_temp1508.f1; goto _LL1513; _LL1513: _temp1512= _temp1508.f2; goto _LL1511;
_LL1511: _temp1510= _temp1508.f3; goto _LL1509; _LL1509: { void* tm; if(
_temp1460 ==  0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp1516=( struct Cyc_Absyn_ConstArray_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1516[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1517; _temp1517.tag= Cyc_Absyn_ConstArray_mod;
_temp1517.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1460); _temp1517;});
_temp1516;});} return({ struct _tuple4 _temp1518; _temp1518.f1= _temp1514;
_temp1518.f2= _temp1512; _temp1518.f3=({ struct Cyc_List_List* _temp1519=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1519->hd=(
void*) tm; _temp1519->tl= _temp1510; _temp1519;}); _temp1518;});}} _LL1451: {
struct Cyc_List_List* _temp1522; void* _temp1524; struct Cyc_Absyn_Tqual
_temp1526; struct _tuple4 _temp1520= Cyc_Absynpp_to_tms( _temp1470, _temp1476);
_LL1527: _temp1526= _temp1520.f1; goto _LL1525; _LL1525: _temp1524= _temp1520.f2;
goto _LL1523; _LL1523: _temp1522= _temp1520.f3; goto _LL1521; _LL1521: { void*
ps;{ struct _tuple8 _temp1529=({ struct _tuple8 _temp1528; _temp1528.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1472))->v; _temp1528.f2=( void*)( Cyc_Absyn_compress_conref( _temp1468))->v;
_temp1528;}); void* _temp1539; void* _temp1541; struct Cyc_Absyn_Exp* _temp1543;
void* _temp1545; int _temp1547; void* _temp1549; void* _temp1551; struct Cyc_Absyn_Exp*
_temp1553; void* _temp1555; int _temp1557; void* _temp1559; void* _temp1561;
_LL1531: _LL1546: _temp1545= _temp1529.f1; if(( unsigned int) _temp1545 >  1u?*((
int*) _temp1545) ==  Cyc_Absyn_Eq_constr: 0){ _LL1548: _temp1547=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp1545)->f1; if( _temp1547 ==  1){ goto _LL1540;}
else{ goto _LL1533;}} else{ goto _LL1533;} _LL1540: _temp1539= _temp1529.f2; if((
unsigned int) _temp1539 >  1u?*(( int*) _temp1539) ==  Cyc_Absyn_Eq_constr: 0){
_LL1542: _temp1541=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1539)->f1;
if(( unsigned int) _temp1541 >  1u?*(( int*) _temp1541) ==  Cyc_Absyn_Upper_b: 0){
_LL1544: _temp1543=(( struct Cyc_Absyn_Upper_b_struct*) _temp1541)->f1; goto
_LL1532;} else{ goto _LL1533;}} else{ goto _LL1533;} _LL1533: _LL1556: _temp1555=
_temp1529.f1; if(( unsigned int) _temp1555 >  1u?*(( int*) _temp1555) ==  Cyc_Absyn_Eq_constr:
0){ _LL1558: _temp1557=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1555)->f1;
if( _temp1557 ==  0){ goto _LL1550;} else{ goto _LL1535;}} else{ goto _LL1535;}
_LL1550: _temp1549= _temp1529.f2; if(( unsigned int) _temp1549 >  1u?*(( int*)
_temp1549) ==  Cyc_Absyn_Eq_constr: 0){ _LL1552: _temp1551=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1549)->f1; if(( unsigned int) _temp1551 >  1u?*(( int*) _temp1551) ==  Cyc_Absyn_Upper_b:
0){ _LL1554: _temp1553=(( struct Cyc_Absyn_Upper_b_struct*) _temp1551)->f1; goto
_LL1534;} else{ goto _LL1535;}} else{ goto _LL1535;} _LL1535: _LL1560: _temp1559=
_temp1529.f2; if(( unsigned int) _temp1559 >  1u?*(( int*) _temp1559) ==  Cyc_Absyn_Eq_constr:
0){ _LL1562: _temp1561=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1559)->f1;
if( _temp1561 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1536;} else{ goto
_LL1537;}} else{ goto _LL1537;} _LL1537: goto _LL1538; _LL1532: ps=( void*)({
struct Cyc_Absyn_Nullable_ps_struct* _temp1563=( struct Cyc_Absyn_Nullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp1563[ 0]=({
struct Cyc_Absyn_Nullable_ps_struct _temp1564; _temp1564.tag= Cyc_Absyn_Nullable_ps;
_temp1564.f1= _temp1543; _temp1564;}); _temp1563;}); goto _LL1530; _LL1534: ps=(
void*)({ struct Cyc_Absyn_NonNullable_ps_struct* _temp1565=( struct Cyc_Absyn_NonNullable_ps_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp1565[ 0]=({
struct Cyc_Absyn_NonNullable_ps_struct _temp1566; _temp1566.tag= Cyc_Absyn_NonNullable_ps;
_temp1566.f1= _temp1553; _temp1566;}); _temp1565;}); goto _LL1530; _LL1536: ps=(
void*) Cyc_Absyn_TaggedArray_ps; goto _LL1530; _LL1538: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL1530; _LL1530:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp1569=( struct Cyc_Absyn_Pointer_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp1569[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp1570; _temp1570.tag=
Cyc_Absyn_Pointer_mod; _temp1570.f1=( void*) ps; _temp1570.f2=( void*) _temp1474;
_temp1570.f3= tq; _temp1570;}); _temp1569;}); return({ struct _tuple4 _temp1567;
_temp1567.f1= _temp1526; _temp1567.f2= _temp1524; _temp1567.f3=({ struct Cyc_List_List*
_temp1568=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1568->hd=( void*) tm; _temp1568->tl= _temp1522; _temp1568;}); _temp1567;});}}}
_LL1453: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp1492 ==  0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1492))->v)){ _temp1492= 0;
_temp1494= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_rewrite_temp_tvar,
_temp1494);}}{ struct Cyc_List_List* _temp1573; void* _temp1575; struct Cyc_Absyn_Tqual
_temp1577; struct _tuple4 _temp1571= Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(),
_temp1490); _LL1578: _temp1577= _temp1571.f1; goto _LL1576; _LL1576: _temp1575=
_temp1571.f2; goto _LL1574; _LL1574: _temp1573= _temp1571.f3; goto _LL1572;
_LL1572: { struct Cyc_List_List* tms= _temp1573; if( _temp1480 !=  0? ! Cyc_Absynpp_to_VC:
0){ tms=({ struct Cyc_List_List* _temp1579=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1579->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp1580=( struct Cyc_Absyn_Attributes_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp1580[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp1581; _temp1581.tag= Cyc_Absyn_Attributes_mod; _temp1581.f1= 0; _temp1581.f2=
_temp1480; _temp1581;}); _temp1580;})); _temp1579->tl= tms; _temp1579;});} tms=({
struct Cyc_List_List* _temp1582=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1582->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp1583=( struct Cyc_Absyn_Function_mod_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp1583[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp1584; _temp1584.tag=
Cyc_Absyn_Function_mod; _temp1584.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp1585=( struct Cyc_Absyn_WithTypes_struct*) _cycalloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp1585[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp1586; _temp1586.tag= Cyc_Absyn_WithTypes;
_temp1586.f1= _temp1488; _temp1586.f2= _temp1486; _temp1586.f3= _temp1484;
_temp1586.f4= _temp1492; _temp1586.f5= _temp1482; _temp1586;}); _temp1585;}));
_temp1584;}); _temp1583;})); _temp1582->tl= tms; _temp1582;}); if( Cyc_Absynpp_to_VC){
for( 0; _temp1480 !=  0; _temp1480=(( struct Cyc_List_List*) _check_null(
_temp1480))->tl){ void* _temp1587=( void*)(( struct Cyc_List_List*) _check_null(
_temp1480))->hd; _LL1589: if( _temp1587 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1590;} else{ goto _LL1591;} _LL1591: if( _temp1587 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1587 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1594;} else{ goto _LL1595;} _LL1595: goto _LL1596; _LL1590: goto _LL1592;
_LL1592: goto _LL1594; _LL1594: tms=({ struct Cyc_List_List* _temp1597=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1597->hd=( void*)((
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1598=( struct Cyc_Absyn_Attributes_mod_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1598[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1599; _temp1599.tag= Cyc_Absyn_Attributes_mod;
_temp1599.f1= 0; _temp1599.f2=({ struct Cyc_List_List* _temp1600=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1600->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp1480))->hd); _temp1600->tl= 0;
_temp1600;}); _temp1599;}); _temp1598;})); _temp1597->tl= tms; _temp1597;});
goto AfterAtts; _LL1596: goto _LL1588; _LL1588:;}} AfterAtts: if( _temp1494 != 
0){ tms=({ struct Cyc_List_List* _temp1601=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1601->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp1602=( struct Cyc_Absyn_TypeParams_mod_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp1602[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp1603; _temp1603.tag= Cyc_Absyn_TypeParams_mod; _temp1603.f1= _temp1494;
_temp1603.f2= 0; _temp1603.f3= 1; _temp1603;}); _temp1602;})); _temp1601->tl=
tms; _temp1601;});} return({ struct _tuple4 _temp1604; _temp1604.f1= _temp1577;
_temp1604.f2= _temp1575; _temp1604.f3= tms; _temp1604;});}} _LL1455: if(
_temp1498 ==  0){ return({ struct _tuple4 _temp1605; _temp1605.f1= tq; _temp1605.f2=
t; _temp1605.f3= 0; _temp1605;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1498))->v);} _LL1457: if( _temp1502 == 
0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp1606;
_temp1606.f1= tq; _temp1606.f2= t; _temp1606.f3= 0; _temp1606;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1502))->v);}
_LL1459: return({ struct _tuple4 _temp1607; _temp1607.f1= tq; _temp1607.f2= t;
_temp1607.f3= 0; _temp1607;}); _LL1447:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp1610; void* _temp1612; struct Cyc_Absyn_Tqual _temp1614; struct _tuple4
_temp1608= Cyc_Absynpp_to_tms( tq, t); _LL1615: _temp1614= _temp1608.f1; goto
_LL1613; _LL1613: _temp1612= _temp1608.f2; goto _LL1611; _LL1611: _temp1610=
_temp1608.f3; goto _LL1609; _LL1609: _temp1610= Cyc_List_imp_rev( _temp1610);
if( _temp1610 ==  0? dopt ==  0: 0){ return({ struct Cyc_PP_Doc* _temp1616[ 2u];
_temp1616[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1612); _temp1616[ 0u]= Cyc_Absynpp_tqual2doc(
_temp1614); Cyc_PP_cat( _tag_arr( _temp1616, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ return({ struct Cyc_PP_Doc* _temp1617[ 4u]; _temp1617[ 3u]= Cyc_Absynpp_dtms2doc(
dopt ==  0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*)
_check_null( dopt))->v, _temp1610); _temp1617[ 2u]= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); _temp1617[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1612);
_temp1617[ 0u]= Cyc_Absynpp_tqual2doc( _temp1614); Cyc_PP_cat( _tag_arr(
_temp1617, sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width !=  0){ return({ struct Cyc_PP_Doc*
_temp1618[ 5u]; _temp1618[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1618[ 3u]= Cyc_Absynpp_atts2doc( f->attributes); _temp1618[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->width)); _temp1618[ 1u]= Cyc_PP_text(
_tag_arr(":", sizeof( unsigned char), 2u)); _temp1618[ 0u]= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp1619=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1619->v=( void*) Cyc_PP_textptr(
f->name); _temp1619;})); Cyc_PP_cat( _tag_arr( _temp1618, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ return({ struct Cyc_PP_Doc* _temp1620[ 3u]; _temp1620[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1620[ 1u]= Cyc_Absynpp_atts2doc(
f->attributes); _temp1620[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp1621=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1621->v=( void*) Cyc_PP_textptr( f->name); _temp1621;}));
Cyc_PP_cat( _tag_arr( _temp1620, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct
Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,
_tag_arr("", sizeof( unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc* _temp1622[ 4u];
_temp1622[ 3u]= f->typs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs);
_temp1622[ 2u]= f->tvs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs);
_temp1622[ 1u]= Cyc_Absynpp_typedef_name2doc( f->name); _temp1622[ 0u]= Cyc_Absynpp_scope2doc((
void*) f->sc); Cyc_PP_cat( _tag_arr( _temp1622, sizeof( struct Cyc_PP_Doc*), 4u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Tunionfield*),
struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fields);} void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f){ for( 0; tdl !=  0; tdl=((
struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd), 72, f);({
void* _temp1623[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1623, sizeof( void*), 0u));});}} struct
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
72);} struct _tagged_arr Cyc_Absynpp_scope2string( void* sc){ void* _temp1624=
sc; _LL1626: if( _temp1624 == ( void*) Cyc_Absyn_Static){ goto _LL1627;} else{
goto _LL1628;} _LL1628: if( _temp1624 == ( void*) Cyc_Absyn_Public){ goto
_LL1629;} else{ goto _LL1630;} _LL1630: if( _temp1624 == ( void*) Cyc_Absyn_Extern){
goto _LL1631;} else{ goto _LL1632;} _LL1632: if( _temp1624 == ( void*) Cyc_Absyn_ExternC){
goto _LL1633;} else{ goto _LL1634;} _LL1634: if( _temp1624 == ( void*) Cyc_Absyn_Abstract){
goto _LL1635;} else{ goto _LL1625;} _LL1627: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL1629: return _tag_arr("public", sizeof( unsigned char),
7u); _LL1631: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL1633:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL1635: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL1625:;}
