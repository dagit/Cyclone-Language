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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list(
struct _tagged_arr); extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_list_cmp( int(* cmp)( void*,
void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); struct Cyc_Lineno_Pos{
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
int tag; struct _tagged_arr* f1; } ; unsigned char Cyc_Absyn_EmptyAnnot[ 15u]="\000\000\000\000EmptyAnnot";
extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Rel_n_struct
Cyc_Absyn_rel_ns_null_value; extern void* Cyc_Absyn_rel_ns_null; extern int Cyc_Absyn_is_qvar_qualified(
struct _tuple0*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual x, struct
Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_ulonglong_t; extern void* Cyc_Absyn_schar_t; extern void*
Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_double_t; extern void* Cyc_Absyn_empty_effect; extern struct _tuple0*
Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud;
extern void* Cyc_Absyn_exn_typ; extern struct _tuple0* Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0* Cyc_Absyn_tunion_scanf_arg_qvar; extern void* Cyc_Absyn_string_typ(
void* rgn); extern void* Cyc_Absyn_const_string_typ( void* rgn); extern void*
Cyc_Absyn_file_typ(); extern struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one;
extern void* Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_starb_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern struct Cyc_Core_Opt* Cyc_Absyn_void_star_typ_opt(); extern void* Cyc_Absyn_strct(
struct _tagged_arr* name); extern void* Cyc_Absyn_strctq( struct _tuple0* name);
extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name); extern void* Cyc_Absyn_union_typ(
struct _tagged_arr* name); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_arr f, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_arr s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*,
struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp( void*, void*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_gentyp_exp( struct Cyc_List_List*, void*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*,
struct _tagged_arr*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_array_exp( struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt( struct _tagged_arr* v, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment* s);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_letv_decl( struct Cyc_List_List*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt*
n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl( void* s, struct _tuple0* n, struct Cyc_List_List*
ts, struct Cyc_Core_Opt* fs, int is_xtunion, struct Cyc_Position_Segment* loc);
extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct Cyc_Core_Opt*
eff_typ, void* ret_typ, struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_List_List* rgn_po, struct Cyc_List_List*); extern void*
Cyc_Absyn_pointer_expand( void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern struct _tuple3*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int); extern struct
_tagged_arr Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); extern struct _tagged_arr* Cyc_Absyn_fieldname( int); extern void Cyc_Absyn_print_decls(
struct Cyc_List_List*); static const int Cyc_Typerep_Var= 0; struct Cyc_Typerep_Var_struct{
int tag; struct _tagged_arr* f1; } ; static const int Cyc_Typerep_Int= 1; struct
Cyc_Typerep_Int_struct{ int tag; unsigned int f1; } ; static const int Cyc_Typerep_Float=
0; static const int Cyc_Typerep_Double= 1; static const int Cyc_Typerep_ThinPtr=
2; struct Cyc_Typerep_ThinPtr_struct{ int tag; unsigned int f1; void* f2; } ;
static const int Cyc_Typerep_FatPtr= 3; struct Cyc_Typerep_FatPtr_struct{ int
tag; void* f1; } ; static const int Cyc_Typerep_Tuple= 4; struct _tuple4{
unsigned int f1; void* f2; } ; struct Cyc_Typerep_Tuple_struct{ int tag;
unsigned int f1; struct _tagged_arr f2; } ; static const int Cyc_Typerep_TUnion=
5; struct Cyc_Typerep_TUnion_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Typerep_XTUnion= 6; struct _tuple5{ struct _tagged_arr f1; void*
f2; } ; struct Cyc_Typerep_XTUnion_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Typerep_Union= 7; struct Cyc_Typerep_Union_struct{ int tag;
struct _tagged_arr f1; } ; static const int Cyc_Typerep_Forall= 8; struct Cyc_Typerep_Forall_struct{
int tag; struct _tagged_arr f1; void** f2; } ; static const int Cyc_Typerep_App=
9; struct Cyc_Typerep_App_struct{ int tag; void* f1; struct _tagged_arr f2; } ;
extern void* Cyc_decls_rep; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Position_Segment;
extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); struct
Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern
unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_printf( struct _tagged_arr fmt,
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
int tag; float* f1; } ; struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; static const
int Cyc_Tcenv_AnonEnumRes= 4; struct Cyc_Tcenv_AnonEnumRes_struct{ int tag; void*
f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls;
struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict*
typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List* availables; }
; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0; static const
int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2; static const
int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct
Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void* f2; } ; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Marshal_print_type( void* rep, void** val); static int
Cyc_Absyn_zstrlist_cmp( struct Cyc_List_List* ss1, struct Cyc_List_List* ss2){
return(( int(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_zstrptrcmp,
ss1, ss2);} int Cyc_Absyn_varlist_cmp( struct Cyc_List_List* vs1, struct Cyc_List_List*
vs2){ return Cyc_Absyn_zstrlist_cmp( vs1, vs2);} struct _tuple6{ void* f1; void*
f2; } ; int Cyc_Absyn_qvar_cmp( struct _tuple0* q1, struct _tuple0* q2){ void*
_temp0=(* q1).f1; void* _temp1=(* q2).f1;{ struct _tuple6 _temp3=({ struct
_tuple6 _temp2; _temp2.f1= _temp0; _temp2.f2= _temp1; _temp2;}); void* _temp19;
void* _temp21; void* _temp23; struct Cyc_List_List* _temp25; void* _temp27;
struct Cyc_List_List* _temp29; void* _temp31; struct Cyc_List_List* _temp33;
void* _temp35; struct Cyc_List_List* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; _LL5: _LL22: _temp21= _temp3.f1; if( _temp21 == ( void*)
Cyc_Absyn_Loc_n){ goto _LL20;} else{ goto _LL7;} _LL20: _temp19= _temp3.f2; if(
_temp19 == ( void*) Cyc_Absyn_Loc_n){ goto _LL6;} else{ goto _LL7;} _LL7: _LL28:
_temp27= _temp3.f1; if(( unsigned int) _temp27 >  1u?*(( int*) _temp27) ==  Cyc_Absyn_Rel_n:
0){ _LL30: _temp29=(( struct Cyc_Absyn_Rel_n_struct*) _temp27)->f1; goto _LL24;}
else{ goto _LL9;} _LL24: _temp23= _temp3.f2; if(( unsigned int) _temp23 >  1u?*((
int*) _temp23) ==  Cyc_Absyn_Rel_n: 0){ _LL26: _temp25=(( struct Cyc_Absyn_Rel_n_struct*)
_temp23)->f1; goto _LL8;} else{ goto _LL9;} _LL9: _LL36: _temp35= _temp3.f1; if((
unsigned int) _temp35 >  1u?*(( int*) _temp35) ==  Cyc_Absyn_Abs_n: 0){ _LL38:
_temp37=(( struct Cyc_Absyn_Abs_n_struct*) _temp35)->f1; goto _LL32;} else{ goto
_LL11;} _LL32: _temp31= _temp3.f2; if(( unsigned int) _temp31 >  1u?*(( int*)
_temp31) ==  Cyc_Absyn_Abs_n: 0){ _LL34: _temp33=(( struct Cyc_Absyn_Abs_n_struct*)
_temp31)->f1; goto _LL10;} else{ goto _LL11;} _LL11: _LL40: _temp39= _temp3.f1;
if( _temp39 == ( void*) Cyc_Absyn_Loc_n){ goto _LL12;} else{ goto _LL13;} _LL13:
_LL42: _temp41= _temp3.f2; if( _temp41 == ( void*) Cyc_Absyn_Loc_n){ goto _LL14;}
else{ goto _LL15;} _LL15: _LL44: _temp43= _temp3.f1; if(( unsigned int) _temp43
>  1u?*(( int*) _temp43) ==  Cyc_Absyn_Rel_n: 0){ goto _LL16;} else{ goto _LL17;}
_LL17: _LL46: _temp45= _temp3.f2; if(( unsigned int) _temp45 >  1u?*(( int*)
_temp45) ==  Cyc_Absyn_Rel_n: 0){ goto _LL18;} else{ goto _LL4;} _LL6: goto _LL4;
_LL8: _temp37= _temp29; _temp33= _temp25; goto _LL10; _LL10: { int i= Cyc_Absyn_zstrlist_cmp(
_temp37, _temp33); if( i !=  0){ return i;} goto _LL4;} _LL12: return - 1; _LL14:
return 1; _LL16: return - 1; _LL18: return 1; _LL4:;} return Cyc_Std_zstrptrcmp((*
q1).f2,(* q2).f2);} int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct
Cyc_Absyn_Tvar* tv2){ int i= Cyc_Std_zstrptrcmp( tv1->name, tv2->name); if( i != 
0){ return i;} if( tv1->identity ==  tv2->identity){ return 0;} if( tv1->identity
!=  0? tv2->identity !=  0: 0){ return*(( int*) _check_null( tv1->identity)) - *((
int*) _check_null( tv2->identity));} else{ if( tv1->identity ==  0){ return - 1;}
else{ return 1;}}} struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value={ 0u,
0}; void* Cyc_Absyn_rel_ns_null=( void*)& Cyc_Absyn_rel_ns_null_value; int Cyc_Absyn_is_qvar_qualified(
struct _tuple0* qv){ void* _temp48=(* qv).f1; struct Cyc_List_List* _temp58;
struct Cyc_List_List* _temp60; _LL50: if(( unsigned int) _temp48 >  1u?*(( int*)
_temp48) ==  Cyc_Absyn_Rel_n: 0){ _LL59: _temp58=(( struct Cyc_Absyn_Rel_n_struct*)
_temp48)->f1; if( _temp58 ==  0){ goto _LL51;} else{ goto _LL52;}} else{ goto
_LL52;} _LL52: if(( unsigned int) _temp48 >  1u?*(( int*) _temp48) ==  Cyc_Absyn_Abs_n:
0){ _LL61: _temp60=(( struct Cyc_Absyn_Abs_n_struct*) _temp48)->f1; if( _temp60
==  0){ goto _LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54: if( _temp48
== ( void*) Cyc_Absyn_Loc_n){ goto _LL55;} else{ goto _LL56;} _LL56: goto _LL57;
_LL51: goto _LL53; _LL53: goto _LL55; _LL55: return 0; _LL57: return 1; _LL49:;}
static int Cyc_Absyn_new_type_counter= 0; void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt*
k, struct Cyc_Core_Opt* env){ return( void*)({ struct Cyc_Absyn_Evar_struct*
_temp62=( struct Cyc_Absyn_Evar_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Evar_struct));
_temp62[ 0]=({ struct Cyc_Absyn_Evar_struct _temp63; _temp63.tag= Cyc_Absyn_Evar;
_temp63.f1= k; _temp63.f2= 0; _temp63.f3= Cyc_Absyn_new_type_counter ++; _temp63.f4=
env; _temp63;}); _temp62;});} static struct Cyc_Core_Opt Cyc_Absyn_mk={( void*)((
void*) 1u)}; void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt* tenv){ return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Absyn_mk, tenv);} struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y){ return({ struct Cyc_Absyn_Tqual
_temp64; _temp64.q_const= x.q_const? 1: y.q_const; _temp64.q_volatile= x.q_volatile?
1: y.q_volatile; _temp64.q_restrict= x.q_restrict? 1: y.q_restrict; _temp64;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(){ return({ struct Cyc_Absyn_Tqual
_temp65; _temp65.q_const= 0; _temp65.q_volatile= 0; _temp65.q_restrict= 0;
_temp65;});} struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(){ return({ struct Cyc_Absyn_Tqual
_temp66; _temp66.q_const= 1; _temp66.q_volatile= 0; _temp66.q_restrict= 0;
_temp66;});} struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x){ return({
struct Cyc_Absyn_Conref* _temp67=( struct Cyc_Absyn_Conref*) _cycalloc( sizeof(
struct Cyc_Absyn_Conref)); _temp67->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp68=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp68[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp69; _temp69.tag= Cyc_Absyn_Eq_constr;
_temp69.f1=( void*) x; _temp69;}); _temp68;})); _temp67;});} struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref(){ return({ struct Cyc_Absyn_Conref* _temp70=( struct Cyc_Absyn_Conref*)
_cycalloc( sizeof( struct Cyc_Absyn_Conref)); _temp70->v=( void*)(( void*) Cyc_Absyn_No_constr);
_temp70;});} static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_constraint={
0u,( void*) 1}; static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_constraint={
0u,( void*) 0}; struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={( void*)(( void*)&
Cyc_Absyn_true_constraint)}; struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(
void*)(( void*)& Cyc_Absyn_false_constraint)}; struct Cyc_Absyn_Conref* Cyc_Absyn_true_conref=&
Cyc_Absyn_true_conref_v; struct Cyc_Absyn_Conref* Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x){
void* _temp73=( void*) x->v; struct Cyc_Absyn_Conref* _temp81; struct Cyc_Absyn_Conref**
_temp83; _LL75: if( _temp73 == ( void*) Cyc_Absyn_No_constr){ goto _LL76;} else{
goto _LL77;} _LL77: if(( unsigned int) _temp73 >  1u?*(( int*) _temp73) ==  Cyc_Absyn_Eq_constr:
0){ goto _LL78;} else{ goto _LL79;} _LL79: if(( unsigned int) _temp73 >  1u?*((
int*) _temp73) ==  Cyc_Absyn_Forward_constr: 0){ _LL82: _temp81=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp73)->f1; _temp83=( struct Cyc_Absyn_Conref**)&(( struct Cyc_Absyn_Forward_constr_struct*)
_temp73)->f1; goto _LL80;} else{ goto _LL74;} _LL76: return x; _LL78: return x;
_LL80: { struct Cyc_Absyn_Conref* _temp84= Cyc_Absyn_compress_conref(* _temp83);*
_temp83= _temp84; return _temp84;} _LL74:;} void* Cyc_Absyn_conref_val( struct
Cyc_Absyn_Conref* x){ void* _temp85=( void*)( Cyc_Absyn_compress_conref( x))->v;
void* _temp91; _LL87: if(( unsigned int) _temp85 >  1u?*(( int*) _temp85) == 
Cyc_Absyn_Eq_constr: 0){ _LL92: _temp91=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp85)->f1; goto _LL88;} else{ goto _LL89;} _LL89: goto _LL90; _LL88: return
_temp91; _LL90: return({ void* _temp93[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("conref_val",
sizeof( unsigned char), 11u), _tag_arr( _temp93, sizeof( void*), 0u));}); _LL86:;}
void* Cyc_Absyn_conref_def( void* y, struct Cyc_Absyn_Conref* x){ void* _temp94=(
void*)( Cyc_Absyn_compress_conref( x))->v; void* _temp100; _LL96: if((
unsigned int) _temp94 >  1u?*(( int*) _temp94) ==  Cyc_Absyn_Eq_constr: 0){
_LL101: _temp100=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp94)->f1;
goto _LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL97: return _temp100; _LL99:
return y; _LL95:;} static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uchar_tt={ 5u,(
void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={ 5u,( void*)(( void*) 1u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={ 5u,( void*)(( void*) 1u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulong_tt={
5u,( void*)(( void*) 1u),( void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulonglong_tt={ 5u,( void*)(( void*) 1u),( void*)(( void*) 3u)}; void*
Cyc_Absyn_uchar_t=( void*)& Cyc_Absyn_uchar_tt; void* Cyc_Absyn_ushort_t=( void*)&
Cyc_Absyn_ushort_tt; void* Cyc_Absyn_uint_t=( void*)& Cyc_Absyn_uint_tt; void*
Cyc_Absyn_ulong_t=( void*)& Cyc_Absyn_ulong_tt; void* Cyc_Absyn_ulonglong_t=(
void*)& Cyc_Absyn_ulonglong_tt; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
5u,( void*)(( void*) 0u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={ 5u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={ 5u,( void*)(( void*) 0u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
5u,( void*)(( void*) 0u),( void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slonglong_tt={ 5u,( void*)(( void*) 0u),( void*)(( void*) 3u)}; void*
Cyc_Absyn_schar_t=( void*)& Cyc_Absyn_schar_tt; void* Cyc_Absyn_sshort_t=( void*)&
Cyc_Absyn_sshort_tt; void* Cyc_Absyn_sint_t=( void*)& Cyc_Absyn_sint_tt; void*
Cyc_Absyn_slong_t=( void*)& Cyc_Absyn_slong_tt; void* Cyc_Absyn_slonglong_t=(
void*)& Cyc_Absyn_slonglong_tt; void* Cyc_Absyn_float_t=( void*) 1u; void* Cyc_Absyn_double_t=(
void*) 2u; static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_abs_null={ 1u, 0};
static unsigned char _temp113[ 4u]="exn"; static struct _tagged_arr Cyc_Absyn_exn_str={
_temp113, _temp113, _temp113 +  4u}; static struct _tuple0 Cyc_Absyn_exn_name_v={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_exn_str}; struct _tuple0* Cyc_Absyn_exn_name=&
Cyc_Absyn_exn_name_v; static unsigned char _temp114[ 15u]="Null_Exception";
static struct _tagged_arr Cyc_Absyn_Null_Exception_str={ _temp114, _temp114,
_temp114 +  15u}; static struct _tuple0 Cyc_Absyn_Null_Exception_pr={( void*)&
Cyc_Absyn_abs_null,& Cyc_Absyn_Null_Exception_str}; struct _tuple0* Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v; static
unsigned char _temp115[ 13u]="Array_bounds"; static struct _tagged_arr Cyc_Absyn_Array_bounds_str={
_temp115, _temp115, _temp115 +  13u}; static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Array_bounds_str}; struct _tuple0* Cyc_Absyn_Array_bounds_name=&
Cyc_Absyn_Array_bounds_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={&
Cyc_Absyn_Array_bounds_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v; static unsigned char
_temp116[ 16u]="Match_Exception"; static struct _tagged_arr Cyc_Absyn_Match_Exception_str={
_temp116, _temp116, _temp116 +  16u}; static struct _tuple0 Cyc_Absyn_Match_Exception_pr={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Match_Exception_str}; struct _tuple0* Cyc_Absyn_Match_Exception_name=&
Cyc_Absyn_Match_Exception_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={&
Cyc_Absyn_Match_Exception_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v; static
unsigned char _temp117[ 10u]="Bad_alloc"; static struct _tagged_arr Cyc_Absyn_Bad_alloc_str={
_temp117, _temp117, _temp117 +  10u}; static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Bad_alloc_str}; struct _tuple0* Cyc_Absyn_Bad_alloc_name=&
Cyc_Absyn_Bad_alloc_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Bad_alloc_tuf_v={&
Cyc_Absyn_Bad_alloc_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v; static struct Cyc_List_List
Cyc_Absyn_exn_l0={( void*)& Cyc_Absyn_Null_Exception_tuf_v, 0}; static struct
Cyc_List_List Cyc_Absyn_exn_l1={( void*)& Cyc_Absyn_Array_bounds_tuf_v,( struct
Cyc_List_List*)& Cyc_Absyn_exn_l0}; static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,( struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={( void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2}; static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)(( struct Cyc_List_List*)& Cyc_Absyn_exn_l3)}; static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={( void*)(( void*) 3u),& Cyc_Absyn_exn_name_v, 0,( struct
Cyc_Core_Opt*)& Cyc_Absyn_exn_ol, 1}; struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud=&
Cyc_Absyn_exn_tud_v; static struct Cyc_Absyn_KnownTunion_struct Cyc_Absyn_exn_tinfou={
1u,& Cyc_Absyn_exn_tud}; static struct Cyc_Absyn_TunionType_struct Cyc_Absyn_exn_typ_tt={
2u,{( void*)(( void*)& Cyc_Absyn_exn_tinfou), 0,( void*)(( void*) 3u)}}; void*
Cyc_Absyn_exn_typ=( void*)& Cyc_Absyn_exn_typ_tt; static unsigned char _temp120[
9u]="PrintArg"; static struct _tagged_arr Cyc_Absyn_printarg_str={ _temp120,
_temp120, _temp120 +  9u}; static unsigned char _temp121[ 9u]="ScanfArg"; static
struct _tagged_arr Cyc_Absyn_scanfarg_str={ _temp121, _temp121, _temp121 +  9u};
static unsigned char _temp122[ 4u]="Cyc"; static struct _tagged_arr Cyc_Absyn_cyc_str={
_temp122, _temp122, _temp122 +  4u}; static unsigned char _temp123[ 4u]="Std";
static struct _tagged_arr Cyc_Absyn_std_str={ _temp123, _temp123, _temp123 +  4u};
static struct Cyc_List_List Cyc_Absyn_std_list={( void*)& Cyc_Absyn_std_str, 0};
static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_std_namespace={ 1u,( struct Cyc_List_List*)&
Cyc_Absyn_std_list}; static struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_printarg_str}; static struct _tuple0
Cyc_Absyn_tunion_scanf_arg_qvar_p={( void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_scanfarg_str};
struct _tuple0* Cyc_Absyn_tunion_print_arg_qvar=& Cyc_Absyn_tunion_print_arg_qvar_p;
struct _tuple0* Cyc_Absyn_tunion_scanf_arg_qvar=& Cyc_Absyn_tunion_scanf_arg_qvar_p;
static struct Cyc_Core_Opt* Cyc_Absyn_string_t_opt= 0; void* Cyc_Absyn_string_typ(
void* rgn){ if( rgn != ( void*) Cyc_Absyn_HeapRgn){ return Cyc_Absyn_starb_typ(
Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b);}
else{ if( Cyc_Absyn_string_t_opt ==  0){ void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b);
Cyc_Absyn_string_t_opt=({ struct Cyc_Core_Opt* _temp125=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp125->v=( void*) t; _temp125;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_string_t_opt))->v;}}
static struct Cyc_Core_Opt* Cyc_Absyn_const_string_t_opt= 0; void* Cyc_Absyn_const_string_typ(
void* rgn){ if( rgn != ( void*) Cyc_Absyn_HeapRgn){ return Cyc_Absyn_starb_typ(
Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b);}
else{ if( Cyc_Absyn_const_string_t_opt ==  0){ void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b);
Cyc_Absyn_const_string_t_opt=({ struct Cyc_Core_Opt* _temp126=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp126->v=( void*) t; _temp126;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_const_string_t_opt))->v;}}
static struct Cyc_Absyn_Int_c_struct Cyc_Absyn_one_intc={ 2u,( void*)(( void*) 1u),
1}; static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={ 0u,( void*)((
void*)& Cyc_Absyn_one_intc)}; struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={
0,( void*)(( void*)& Cyc_Absyn_one_b_raw), 0}; struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one=&
Cyc_Absyn_exp_unsigned_one_v; static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={
0u,& Cyc_Absyn_exp_unsigned_one_v}; void* Cyc_Absyn_bounds_one=( void*)& Cyc_Absyn_one_bt;
void* Cyc_Absyn_starb_typ( void* t, void* r, struct Cyc_Absyn_Tqual tq, void* b){
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp130=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp130[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp131; _temp131.tag= Cyc_Absyn_PointerType;
_temp131.f1=({ struct Cyc_Absyn_PtrInfo _temp132; _temp132.elt_typ=( void*) t;
_temp132.rgn_typ=( void*) r; _temp132.nullable= Cyc_Absyn_true_conref; _temp132.tq=
tq; _temp132.bounds= Cyc_Absyn_new_conref( b); _temp132;}); _temp131;});
_temp130;});} void* Cyc_Absyn_atb_typ( void* t, void* r, struct Cyc_Absyn_Tqual
tq, void* b){ return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp133=(
struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp133[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp134; _temp134.tag= Cyc_Absyn_PointerType;
_temp134.f1=({ struct Cyc_Absyn_PtrInfo _temp135; _temp135.elt_typ=( void*) t;
_temp135.rgn_typ=( void*) r; _temp135.nullable= Cyc_Absyn_false_conref; _temp135.tq=
tq; _temp135.bounds= Cyc_Absyn_new_conref( b); _temp135;}); _temp134;});
_temp133;});} void* Cyc_Absyn_star_typ( void* t, void* r, struct Cyc_Absyn_Tqual
tq){ return Cyc_Absyn_starb_typ( t, r, tq, Cyc_Absyn_bounds_one);} void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual tq){ return Cyc_Absyn_starb_typ( t,( void*) Cyc_Absyn_HeapRgn,
tq, Cyc_Absyn_bounds_one);} void* Cyc_Absyn_at_typ( void* t, void* r, struct Cyc_Absyn_Tqual
tq){ return Cyc_Absyn_atb_typ( t, r, tq, Cyc_Absyn_bounds_one);} void* Cyc_Absyn_tagged_typ(
void* t, void* r, struct Cyc_Absyn_Tqual tq){ return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp136=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp136[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp137; _temp137.tag= Cyc_Absyn_PointerType;
_temp137.f1=({ struct Cyc_Absyn_PtrInfo _temp138; _temp138.elt_typ=( void*) t;
_temp138.rgn_typ=( void*) r; _temp138.nullable= Cyc_Absyn_true_conref; _temp138.tq=
tq; _temp138.bounds= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_Unknown_b);
_temp138;}); _temp137;}); _temp136;});} static struct Cyc_Core_Opt* Cyc_Absyn_file_t_opt=
0; static unsigned char _temp139[ 8u]="__sFILE"; static struct _tagged_arr Cyc_Absyn_sf_str={
_temp139, _temp139, _temp139 +  8u}; static struct _tagged_arr* Cyc_Absyn_sf=&
Cyc_Absyn_sf_str; static unsigned char _temp140[ 4u]="Std"; static struct
_tagged_arr Cyc_Absyn_st_str={ _temp140, _temp140, _temp140 +  4u}; static
struct _tagged_arr* Cyc_Absyn_st=& Cyc_Absyn_st_str; void* Cyc_Absyn_file_typ(){
if( Cyc_Absyn_file_t_opt ==  0){ struct _tuple0* file_t_name=({ struct _tuple0*
_temp147=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp147->f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp148=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp148[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp149; _temp149.tag= Cyc_Absyn_Abs_n; _temp149.f1=({ struct _tagged_arr*
_temp150[ 1u]; _temp150[ 0u]= Cyc_Absyn_st;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp150, sizeof( struct _tagged_arr*),
1u));}); _temp149;}); _temp148;}); _temp147->f2= Cyc_Absyn_sf; _temp147;});
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp145=(
struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp145->sc=( void*)(( void*) Cyc_Absyn_Abstract); _temp145->name=({ struct Cyc_Core_Opt*
_temp146=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp146->v=( void*) file_t_name; _temp146;}); _temp145->tvs= 0; _temp145->fields=
0; _temp145->attributes= 0; _temp145;}); void* file_struct_typ=( void*)({ struct
Cyc_Absyn_StructType_struct* _temp142=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp142[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp143; _temp143.tag= Cyc_Absyn_StructType;
_temp143.f1=( struct _tuple0*) file_t_name; _temp143.f2= 0; _temp143.f3=({
struct Cyc_Absyn_Structdecl** _temp144=( struct Cyc_Absyn_Structdecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl*) *  1); _temp144[ 0]= sd;
_temp144;}); _temp143;}); _temp142;}); Cyc_Absyn_file_t_opt=({ struct Cyc_Core_Opt*
_temp141=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp141->v=( void*) Cyc_Absyn_at_typ( file_struct_typ,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp141;});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Absyn_file_t_opt))->v;} static struct Cyc_Core_Opt* Cyc_Absyn_void_star_t_opt=
0; void* Cyc_Absyn_void_star_typ(){ if( Cyc_Absyn_void_star_t_opt ==  0){ Cyc_Absyn_void_star_t_opt=({
struct Cyc_Core_Opt* _temp151=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp151->v=( void*) Cyc_Absyn_star_typ(( void*) Cyc_Absyn_VoidType,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp151;});} return( void*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_void_star_t_opt))->v;} struct Cyc_Core_Opt*
Cyc_Absyn_void_star_typ_opt(){ if( Cyc_Absyn_void_star_t_opt ==  0){ Cyc_Absyn_void_star_t_opt=({
struct Cyc_Core_Opt* _temp152=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp152->v=( void*) Cyc_Absyn_star_typ(( void*) Cyc_Absyn_VoidType,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp152;});} return Cyc_Absyn_void_star_t_opt;}
static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={ 18u, 0}; void* Cyc_Absyn_empty_effect=(
void*)& Cyc_Absyn_empty_eff; void* Cyc_Absyn_strct( struct _tagged_arr* name){
return( void*)({ struct Cyc_Absyn_StructType_struct* _temp154=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp154[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp155; _temp155.tag= Cyc_Absyn_StructType;
_temp155.f1=({ struct _tuple0* _temp156=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp156->f1= Cyc_Absyn_rel_ns_null; _temp156->f2= name;
_temp156;}); _temp155.f2= 0; _temp155.f3= 0; _temp155;}); _temp154;});} void*
Cyc_Absyn_union_typ( struct _tagged_arr* name){ return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp157=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp157[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp158; _temp158.tag= Cyc_Absyn_UnionType;
_temp158.f1=({ struct _tuple0* _temp159=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp159->f1= Cyc_Absyn_rel_ns_null; _temp159->f2= name;
_temp159;}); _temp158.f2= 0; _temp158.f3= 0; _temp158;}); _temp157;});} void*
Cyc_Absyn_strctq( struct _tuple0* name){ return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp160=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp160[ 0]=({ struct Cyc_Absyn_StructType_struct _temp161; _temp161.tag= Cyc_Absyn_StructType;
_temp161.f1=( struct _tuple0*) name; _temp161.f2= 0; _temp161.f3= 0; _temp161;});
_temp160;});} void* Cyc_Absyn_unionq_typ( struct _tuple0* name){ return( void*)({
struct Cyc_Absyn_UnionType_struct* _temp162=( struct Cyc_Absyn_UnionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp162[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp163; _temp163.tag= Cyc_Absyn_UnionType; _temp163.f1=(
struct _tuple0*) name; _temp163.f2= 0; _temp163.f3= 0; _temp163;}); _temp162;});}
struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void* r, struct Cyc_Position_Segment*
loc){ return({ struct Cyc_Absyn_Exp* _temp164=( struct Cyc_Absyn_Exp*) _cycalloc(
sizeof( struct Cyc_Absyn_Exp)); _temp164->topt= 0; _temp164->r=( void*) r;
_temp164->loc= loc; _temp164;});} struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_New_e_struct*
_temp165=( struct Cyc_Absyn_New_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_New_e_struct));
_temp165[ 0]=({ struct Cyc_Absyn_New_e_struct _temp166; _temp166.tag= Cyc_Absyn_New_e;
_temp166.f1= rgn_handle; _temp166.f2= e; _temp166;}); _temp165;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp* e){ return({ struct Cyc_Absyn_Exp*
_temp167=( struct Cyc_Absyn_Exp*) _cycalloc( sizeof( struct Cyc_Absyn_Exp));
_temp167->topt= e->topt; _temp167->r=( void*)(( void*) e->r); _temp167->loc= e->loc;
_temp167;});} struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void* c, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp168=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp168[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp169; _temp169.tag= Cyc_Absyn_Const_e;
_temp169.f1=( void*) c; _temp169;}); _temp168;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp( struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp170=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp170[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp171; _temp171.tag= Cyc_Absyn_Const_e; _temp171.f1=( void*)(( void*) Cyc_Absyn_Null_c);
_temp171;}); _temp170;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*
s, int i, struct Cyc_Position_Segment* seg){ return Cyc_Absyn_const_exp(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp172=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp172[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp173; _temp173.tag= Cyc_Absyn_Int_c; _temp173.f1=( void*) s; _temp173.f2= i;
_temp173;}); _temp172;}), seg);} struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int i, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed,
i, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int i, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Unsigned,( int) i, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp( int b, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_signed_int_exp( b? 1: 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_bool_exp( 1, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment* loc){ return
Cyc_Absyn_bool_exp( 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp(
unsigned char c, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_Char_c_struct* _temp174=( struct Cyc_Absyn_Char_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp174[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp175; _temp175.tag= Cyc_Absyn_Char_c; _temp175.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp175.f2= c; _temp175;}); _temp174;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_arr f, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_Float_c_struct* _temp176=( struct Cyc_Absyn_Float_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp176[ 0]=({ struct Cyc_Absyn_Float_c_struct
_temp177; _temp177.tag= Cyc_Absyn_Float_c; _temp177.f1= f; _temp177;}); _temp176;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp( struct _tagged_arr s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp(( void*)({ struct Cyc_Absyn_String_c_struct*
_temp178=( struct Cyc_Absyn_String_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp178[ 0]=({ struct Cyc_Absyn_String_c_struct _temp179; _temp179.tag= Cyc_Absyn_String_c;
_temp179.f1= s; _temp179;}); _temp178;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp(
struct _tuple0* q, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp180=( struct Cyc_Absyn_Var_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp180[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp181; _temp181.tag= Cyc_Absyn_Var_e; _temp181.f1= q; _temp181.f2=( void*)((
void*) Cyc_Absyn_Unresolved_b); _temp181;}); _temp180;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp( struct _tuple0* q, void* b, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp182=(
struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp182[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp183; _temp183.tag= Cyc_Absyn_Var_e;
_temp183.f1= q; _temp183.f2=( void*) b; _temp183;}); _temp182;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0* q, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp184=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp184[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp185; _temp185.tag= Cyc_Absyn_UnknownId_e;
_temp185.f1= q; _temp185;}); _temp184;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void* p, struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp186=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp186[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp187; _temp187.tag= Cyc_Absyn_Primop_e; _temp187.f1=(
void*) p; _temp187.f2= es; _temp187;}); _temp186;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void* p, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp188=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp188->hd=( void*)
e; _temp188->tl= 0; _temp188;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp189=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp189->hd=( void*)
e1; _temp189->tl=({ struct Cyc_List_List* _temp190=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp190->hd=( void*) e2; _temp190->tl=
0; _temp190;}); _temp189;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Times,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Div, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Eq, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Neq,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Gt, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Lt, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Lte, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* popt, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_AssignOp_e_struct* _temp191=( struct Cyc_Absyn_AssignOp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AssignOp_e_struct)); _temp191[ 0]=({ struct
Cyc_Absyn_AssignOp_e_struct _temp192; _temp192.tag= Cyc_Absyn_AssignOp_e;
_temp192.f1= e1; _temp192.f2= popt; _temp192.f3= e2; _temp192;}); _temp191;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_assignop_exp(
e1, 0, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp( struct Cyc_Absyn_Exp*
e, void* i, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp193=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp193[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp194; _temp194.tag= Cyc_Absyn_Increment_e;
_temp194.f1= e; _temp194.f2=( void*) i; _temp194;}); _temp193;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostInc, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreInc, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreDec, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostDec, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Conditional_e_struct*
_temp195=( struct Cyc_Absyn_Conditional_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Conditional_e_struct));
_temp195[ 0]=({ struct Cyc_Absyn_Conditional_e_struct _temp196; _temp196.tag=
Cyc_Absyn_Conditional_e; _temp196.f1= e1; _temp196.f2= e2; _temp196.f3= e3;
_temp196;}); _temp195;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_conditional_exp( e1, e2, Cyc_Absyn_false_exp( loc), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_conditional_exp( e1, Cyc_Absyn_true_exp(
loc), e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_SeqExp_e_struct* _temp197=( struct Cyc_Absyn_SeqExp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SeqExp_e_struct)); _temp197[ 0]=({ struct
Cyc_Absyn_SeqExp_e_struct _temp198; _temp198.tag= Cyc_Absyn_SeqExp_e; _temp198.f1=
e1; _temp198.f2= e2; _temp198;}); _temp197;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownCall_e_struct*
_temp199=( struct Cyc_Absyn_UnknownCall_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownCall_e_struct));
_temp199[ 0]=({ struct Cyc_Absyn_UnknownCall_e_struct _temp200; _temp200.tag=
Cyc_Absyn_UnknownCall_e; _temp200.f1= e; _temp200.f2= es; _temp200;}); _temp199;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp201=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp201[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp202; _temp202.tag= Cyc_Absyn_FnCall_e; _temp202.f1=
e; _temp202.f2= es; _temp202.f3= 0; _temp202;}); _temp201;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_NoInstantiate_e_struct*
_temp203=( struct Cyc_Absyn_NoInstantiate_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NoInstantiate_e_struct)); _temp203[ 0]=({ struct Cyc_Absyn_NoInstantiate_e_struct
_temp204; _temp204.tag= Cyc_Absyn_NoInstantiate_e; _temp204.f1= e; _temp204;});
_temp203;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp205=( struct Cyc_Absyn_Instantiate_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp205[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp206; _temp206.tag= Cyc_Absyn_Instantiate_e;
_temp206.f1= e; _temp206.f2= ts; _temp206;}); _temp205;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp207=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp207[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp208; _temp208.tag= Cyc_Absyn_Cast_e;
_temp208.f1=( void*) t; _temp208.f2= e; _temp208;}); _temp207;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Throw_e_struct*
_temp209=( struct Cyc_Absyn_Throw_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Throw_e_struct));
_temp209[ 0]=({ struct Cyc_Absyn_Throw_e_struct _temp210; _temp210.tag= Cyc_Absyn_Throw_e;
_temp210.f1= e; _temp210;}); _temp209;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Address_e_struct* _temp211=( struct Cyc_Absyn_Address_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Address_e_struct)); _temp211[ 0]=({ struct
Cyc_Absyn_Address_e_struct _temp212; _temp212.tag= Cyc_Absyn_Address_e; _temp212.f1=
e; _temp212;}); _temp211;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Sizeoftyp_e_struct* _temp213=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp213[ 0]=({ struct
Cyc_Absyn_Sizeoftyp_e_struct _temp214; _temp214.tag= Cyc_Absyn_Sizeoftyp_e;
_temp214.f1=( void*) t; _temp214;}); _temp213;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Sizeofexp_e_struct*
_temp215=( struct Cyc_Absyn_Sizeofexp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeofexp_e_struct));
_temp215[ 0]=({ struct Cyc_Absyn_Sizeofexp_e_struct _temp216; _temp216.tag= Cyc_Absyn_Sizeofexp_e;
_temp216.f1= e; _temp216;}); _temp215;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_offsetof_exp(
void* t, void* of, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp217=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp217[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp218; _temp218.tag= Cyc_Absyn_Offsetof_e;
_temp218.f1=( void*) t; _temp218.f2=( void*) of; _temp218;}); _temp217;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_gentyp_exp( struct Cyc_List_List* tvs, void* t,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_Gentyp_e_struct* _temp219=( struct Cyc_Absyn_Gentyp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Gentyp_e_struct)); _temp219[ 0]=({ struct
Cyc_Absyn_Gentyp_e_struct _temp220; _temp220.tag= Cyc_Absyn_Gentyp_e; _temp220.f1=
tvs; _temp220.f2=( void*) t; _temp220;}); _temp219;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Deref_e_struct* _temp221=(
struct Cyc_Absyn_Deref_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Deref_e_struct));
_temp221[ 0]=({ struct Cyc_Absyn_Deref_e_struct _temp222; _temp222.tag= Cyc_Absyn_Deref_e;
_temp222.f1= e; _temp222;}); _temp221;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp* e, struct _tagged_arr* n, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_StructMember_e_struct*
_temp223=( struct Cyc_Absyn_StructMember_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_StructMember_e_struct)); _temp223[ 0]=({ struct Cyc_Absyn_StructMember_e_struct
_temp224; _temp224.tag= Cyc_Absyn_StructMember_e; _temp224.f1= e; _temp224.f2= n;
_temp224;}); _temp223;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp* e, struct _tagged_arr* n, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_StructArrow_e_struct*
_temp225=( struct Cyc_Absyn_StructArrow_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructArrow_e_struct));
_temp225[ 0]=({ struct Cyc_Absyn_StructArrow_e_struct _temp226; _temp226.tag=
Cyc_Absyn_StructArrow_e; _temp226.f1= e; _temp226.f2= n; _temp226;}); _temp225;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_arrow_exp( struct Cyc_Absyn_Exp* e,
struct _tagged_arr* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_deref_exp(
Cyc_Absyn_structmember_exp( e, n, loc), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Subscript_e_struct*
_temp227=( struct Cyc_Absyn_Subscript_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Subscript_e_struct));
_temp227[ 0]=({ struct Cyc_Absyn_Subscript_e_struct _temp228; _temp228.tag= Cyc_Absyn_Subscript_e;
_temp228.f1= e1; _temp228.f2= e2; _temp228;}); _temp227;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp( struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Tuple_e_struct* _temp229=(
struct Cyc_Absyn_Tuple_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_e_struct));
_temp229[ 0]=({ struct Cyc_Absyn_Tuple_e_struct _temp230; _temp230.tag= Cyc_Absyn_Tuple_e;
_temp230.f1= es; _temp230;}); _temp229;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_StmtExp_e_struct* _temp231=( struct Cyc_Absyn_StmtExp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StmtExp_e_struct)); _temp231[ 0]=({ struct
Cyc_Absyn_StmtExp_e_struct _temp232; _temp232.tag= Cyc_Absyn_StmtExp_e; _temp232.f1=
s; _temp232;}); _temp231;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_match_exn_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_var_exp( Cyc_Absyn_Match_Exception_name,
loc);} struct _tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_Absyn_Exp* Cyc_Absyn_array_exp( struct Cyc_List_List* es, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* dles= 0; for( 0; es !=  0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ dles=({ struct Cyc_List_List* _temp233=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp233->hd=( void*)({ struct
_tuple7* _temp234=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7));
_temp234->f1= 0; _temp234->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd; _temp234;}); _temp233->tl= dles; _temp233;});} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp235=(
struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp235[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp236; _temp236.tag= Cyc_Absyn_Array_e;
_temp236.f1= dles; _temp236;}); _temp235;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt* n, struct Cyc_List_List* dles, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp237=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp237[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp238; _temp238.tag= Cyc_Absyn_UnresolvedMem_e; _temp238.f1= n; _temp238.f2=
dles; _temp238;}); _temp237;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Stmt*
_temp239=( struct Cyc_Absyn_Stmt*) _cycalloc( sizeof( struct Cyc_Absyn_Stmt));
_temp239->r=( void*) s; _temp239->loc= loc; _temp239->non_local_preds= 0;
_temp239->try_depth= 0; _temp239->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot);
_temp239;});} struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*) Cyc_Absyn_Skip_s, loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Exp_s_struct* _temp240=(
struct Cyc_Absyn_Exp_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Exp_s_struct));
_temp240[ 0]=({ struct Cyc_Absyn_Exp_s_struct _temp241; _temp241.tag= Cyc_Absyn_Exp_s;
_temp241.f1= e; _temp241;}); _temp240;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts(
struct Cyc_List_List* ss, struct Cyc_Position_Segment* loc){ if( ss ==  0){
return Cyc_Absyn_skip_stmt( loc);} else{ if((( struct Cyc_List_List*)
_check_null( ss))->tl ==  0){ return( struct Cyc_Absyn_Stmt*)(( struct Cyc_List_List*)
_check_null( ss))->hd;} else{ return Cyc_Absyn_seq_stmt(( struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*) _check_null( ss))->hd, Cyc_Absyn_seq_stmts((( struct Cyc_List_List*)
_check_null( ss))->tl, loc), loc);}}} struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Return_s_struct* _temp242=( struct Cyc_Absyn_Return_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Return_s_struct)); _temp242[ 0]=({ struct
Cyc_Absyn_Return_s_struct _temp243; _temp243.tag= Cyc_Absyn_Return_s; _temp243.f1=
e; _temp243;}); _temp242;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_IfThenElse_s_struct* _temp244=( struct Cyc_Absyn_IfThenElse_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IfThenElse_s_struct)); _temp244[ 0]=({
struct Cyc_Absyn_IfThenElse_s_struct _temp245; _temp245.tag= Cyc_Absyn_IfThenElse_s;
_temp245.f1= e; _temp245.f2= s1; _temp245.f3= s2; _temp245;}); _temp244;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt*
s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_While_s_struct* _temp246=( struct Cyc_Absyn_While_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_While_s_struct)); _temp246[ 0]=({ struct Cyc_Absyn_While_s_struct
_temp247; _temp247.tag= Cyc_Absyn_While_s; _temp247.f1=({ struct _tuple2
_temp248; _temp248.f1= e; _temp248.f2= Cyc_Absyn_skip_stmt( e->loc); _temp248;});
_temp247.f2= s; _temp247;}); _temp246;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Break_s_struct* _temp249=( struct Cyc_Absyn_Break_s_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Break_s_struct)); _temp249[ 0]=({ struct Cyc_Absyn_Break_s_struct
_temp250; _temp250.tag= Cyc_Absyn_Break_s; _temp250.f1= 0; _temp250;}); _temp249;}),
loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Continue_s_struct*
_temp251=( struct Cyc_Absyn_Continue_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Continue_s_struct));
_temp251[ 0]=({ struct Cyc_Absyn_Continue_s_struct _temp252; _temp252.tag= Cyc_Absyn_Continue_s;
_temp252.f1= 0; _temp252;}); _temp251;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_For_s_struct* _temp253=( struct Cyc_Absyn_For_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_For_s_struct)); _temp253[ 0]=({ struct Cyc_Absyn_For_s_struct
_temp254; _temp254.tag= Cyc_Absyn_For_s; _temp254.f1= e1; _temp254.f2=({ struct
_tuple2 _temp255; _temp255.f1= e2; _temp255.f2= Cyc_Absyn_skip_stmt( e3->loc);
_temp255;}); _temp254.f3=({ struct _tuple2 _temp256; _temp256.f1= e3; _temp256.f2=
Cyc_Absyn_skip_stmt( e3->loc); _temp256;}); _temp254.f4= s; _temp254;});
_temp253;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Switch_s_struct* _temp257=( struct Cyc_Absyn_Switch_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_s_struct)); _temp257[ 0]=({ struct
Cyc_Absyn_Switch_s_struct _temp258; _temp258.tag= Cyc_Absyn_Switch_s; _temp258.f1=
e; _temp258.f2= scs; _temp258;}); _temp257;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc){ void* _temp259=( void*) s1->r; _LL261: if( _temp259 == ( void*) Cyc_Absyn_Skip_s){
goto _LL262;} else{ goto _LL263;} _LL263: goto _LL264; _LL262: return s2; _LL264:
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Seq_s_struct* _temp265=(
struct Cyc_Absyn_Seq_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Seq_s_struct));
_temp265[ 0]=({ struct Cyc_Absyn_Seq_s_struct _temp266; _temp266.tag= Cyc_Absyn_Seq_s;
_temp266.f1= s1; _temp266.f2= s2; _temp266;}); _temp265;}), loc); _LL260:;}
struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Fallthru_s_struct*
_temp267=( struct Cyc_Absyn_Fallthru_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fallthru_s_struct));
_temp267[ 0]=({ struct Cyc_Absyn_Fallthru_s_struct _temp268; _temp268.tag= Cyc_Absyn_Fallthru_s;
_temp268.f1= el; _temp268.f2= 0; _temp268;}); _temp267;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp269=( struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp269[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp270; _temp270.tag= Cyc_Absyn_Decl_s;
_temp270.f1= d; _temp270.f2= s; _temp270;}); _temp269;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Decl*
d= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp273=(
struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp273[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp274; _temp274.tag= Cyc_Absyn_Var_d;
_temp274.f1= Cyc_Absyn_new_vardecl( x, t, init); _temp274;}); _temp273;}), loc);
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp271=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp271[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp272; _temp272.tag= Cyc_Absyn_Decl_s;
_temp272.f1= d; _temp272.f2= s; _temp272;}); _temp271;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct* _temp275=(
struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp275[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp276; _temp276.tag= Cyc_Absyn_Cut_s;
_temp276.f1= s; _temp276;}); _temp275;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp277=( struct Cyc_Absyn_Splice_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp277[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp278; _temp278.tag= Cyc_Absyn_Splice_s; _temp278.f1=
s; _temp278;}); _temp277;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt(
struct _tagged_arr* v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp279=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp279[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp280; _temp280.tag= Cyc_Absyn_Label_s;
_temp280.f1= v; _temp280.f2= s; _temp280;}); _temp279;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Do_s_struct*
_temp281=( struct Cyc_Absyn_Do_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Do_s_struct));
_temp281[ 0]=({ struct Cyc_Absyn_Do_s_struct _temp282; _temp282.tag= Cyc_Absyn_Do_s;
_temp282.f1= s; _temp282.f2=({ struct _tuple2 _temp283; _temp283.f1= e; _temp283.f2=
Cyc_Absyn_skip_stmt( e->loc); _temp283;}); _temp282;}); _temp281;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt* s, struct
Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_TryCatch_s_struct* _temp284=( struct Cyc_Absyn_TryCatch_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TryCatch_s_struct)); _temp284[ 0]=({ struct
Cyc_Absyn_TryCatch_s_struct _temp285; _temp285.tag= Cyc_Absyn_TryCatch_s;
_temp285.f1= s; _temp285.f2= scs; _temp285;}); _temp284;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Goto_s_struct* _temp286=(
struct Cyc_Absyn_Goto_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Goto_s_struct));
_temp286[ 0]=({ struct Cyc_Absyn_Goto_s_struct _temp287; _temp287.tag= Cyc_Absyn_Goto_s;
_temp287.f1= lab; _temp287.f2= 0; _temp287;}); _temp286;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
e1, e2, loc), loc);} struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct
Cyc_Position_Segment* s){ return({ struct Cyc_Absyn_Pat* _temp288=( struct Cyc_Absyn_Pat*)
_cycalloc( sizeof( struct Cyc_Absyn_Pat)); _temp288->r=( void*) p; _temp288->topt=
0; _temp288->loc= s; _temp288;});} struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Decl*
_temp289=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp289->r=( void*) r; _temp289->loc= loc; _temp289;});} struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* t_opt, struct
Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Let_d_struct* _temp290=( struct Cyc_Absyn_Let_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Let_d_struct)); _temp290[ 0]=({ struct Cyc_Absyn_Let_d_struct
_temp291; _temp291.tag= Cyc_Absyn_Let_d; _temp291.f1= p; _temp291.f2= 0;
_temp291.f3= t_opt; _temp291.f4= e; _temp291.f5= 0; _temp291;}); _temp290;}),
loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_letv_decl( struct Cyc_List_List* vds,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(( void*)({ struct
Cyc_Absyn_Letv_d_struct* _temp292=( struct Cyc_Absyn_Letv_d_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Letv_d_struct)); _temp292[ 0]=({ struct Cyc_Absyn_Letv_d_struct
_temp293; _temp293.tag= Cyc_Absyn_Letv_d; _temp293.f1= vds; _temp293;});
_temp292;}), loc);} struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct
_tuple0* x, void* t, struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl*
_temp294=( struct Cyc_Absyn_Vardecl*) _cycalloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp294->sc=( void*)(( void*) Cyc_Absyn_Public); _temp294->name= x; _temp294->tq=
Cyc_Absyn_empty_tqual(); _temp294->type=( void*) t; _temp294->initializer= init;
_temp294->rgn= 0; _temp294->attributes= 0; _temp294;});} struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init){
return({ struct Cyc_Absyn_Vardecl* _temp295=( struct Cyc_Absyn_Vardecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp295->sc=( void*)(( void*)
Cyc_Absyn_Static); _temp295->name= x; _temp295->tq= Cyc_Absyn_empty_tqual();
_temp295->type=( void*) t; _temp295->initializer= init; _temp295->rgn= 0;
_temp295->attributes= 0; _temp295;});} struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp296=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp296[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp297; _temp297.tag= Cyc_Absyn_Struct_d; _temp297.f1=({
struct Cyc_Absyn_Structdecl* _temp298=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp298->sc=( void*) s; _temp298->name=
n; _temp298->tvs= ts; _temp298->fields= fs; _temp298->attributes= atts; _temp298;});
_temp297;}); _temp296;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp299=( struct Cyc_Absyn_Union_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp299[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp300; _temp300.tag= Cyc_Absyn_Union_d; _temp300.f1=({ struct Cyc_Absyn_Uniondecl*
_temp301=( struct Cyc_Absyn_Uniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp301->sc=( void*) s; _temp301->name= n; _temp301->tvs= ts; _temp301->fields=
fs; _temp301->attributes= atts; _temp301;}); _temp300;}); _temp299;}), loc);}
struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl( void* s, struct _tuple0* n, struct
Cyc_List_List* ts, struct Cyc_Core_Opt* fs, int is_xtunion, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp302=( struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp302[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp303; _temp303.tag= Cyc_Absyn_Tunion_d;
_temp303.f1=({ struct Cyc_Absyn_Tuniondecl* _temp304=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp304->sc=( void*) s;
_temp304->name= n; _temp304->tvs= ts; _temp304->fields= fs; _temp304->is_xtunion=
is_xtunion; _temp304;}); _temp303;}); _temp302;}), loc);} static struct _tuple1*
Cyc_Absyn_expand_arg( struct _tuple1* a){ return({ struct _tuple1* _temp305=(
struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp305->f1=(* a).f1;
_temp305->f2=(* a).f2; _temp305->f3= Cyc_Absyn_pointer_expand((* a).f3);
_temp305;});} void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct
Cyc_Core_Opt* eff_typ, void* ret_typ, struct Cyc_List_List* args, int c_varargs,
struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_List_List* rgn_po, struct
Cyc_List_List* atts){ return( void*)({ struct Cyc_Absyn_FnType_struct* _temp306=(
struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp306[ 0]=({ struct Cyc_Absyn_FnType_struct _temp307; _temp307.tag= Cyc_Absyn_FnType;
_temp307.f1=({ struct Cyc_Absyn_FnInfo _temp308; _temp308.tvars= tvs; _temp308.ret_typ=(
void*) Cyc_Absyn_pointer_expand( ret_typ); _temp308.effect= eff_typ; _temp308.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absyn_expand_arg, args); _temp308.c_varargs= c_varargs;
_temp308.cyc_varargs= cyc_varargs; _temp308.rgn_po= rgn_po; _temp308.attributes=
atts; _temp308;}); _temp307;}); _temp306;});} void* Cyc_Absyn_pointer_expand(
void* t){ void* _temp309= Cyc_Tcutil_compress( t); _LL311: if(( unsigned int)
_temp309 >  4u?*(( int*) _temp309) ==  Cyc_Absyn_FnType: 0){ goto _LL312;} else{
goto _LL313;} _LL313: goto _LL314; _LL312: return Cyc_Absyn_at_typ( t,( void*)
Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _LL314: return t; _LL310:;} int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp* e){ void* _temp315=( void*) e->r; void* _temp339; void*
_temp341; struct Cyc_Absyn_Vardecl* _temp343; void* _temp345; struct Cyc_Absyn_Vardecl*
_temp347; struct Cyc_Absyn_Exp* _temp349; struct Cyc_Absyn_Exp* _temp351; struct
_tagged_arr* _temp353; struct Cyc_Absyn_Exp* _temp355; struct _tagged_arr*
_temp357; struct Cyc_Absyn_Exp* _temp359; struct Cyc_Absyn_Exp* _temp361; struct
Cyc_Absyn_Exp* _temp363; struct Cyc_Absyn_Exp* _temp365; _LL317: if(*(( int*)
_temp315) ==  Cyc_Absyn_Var_e){ _LL340: _temp339=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp315)->f2; if(( unsigned int) _temp339 >  1u?*(( int*) _temp339) ==  Cyc_Absyn_Funname_b:
0){ goto _LL318;} else{ goto _LL319;}} else{ goto _LL319;} _LL319: if(*(( int*)
_temp315) ==  Cyc_Absyn_Var_e){ _LL342: _temp341=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp315)->f2; if(( unsigned int) _temp341 >  1u?*(( int*) _temp341) ==  Cyc_Absyn_Global_b:
0){ _LL344: _temp343=(( struct Cyc_Absyn_Global_b_struct*) _temp341)->f1; goto
_LL320;} else{ goto _LL321;}} else{ goto _LL321;} _LL321: if(*(( int*) _temp315)
==  Cyc_Absyn_Var_e){ _LL346: _temp345=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp315)->f2; if(( unsigned int) _temp345 >  1u?*(( int*) _temp345) ==  Cyc_Absyn_Local_b:
0){ _LL348: _temp347=(( struct Cyc_Absyn_Local_b_struct*) _temp345)->f1; goto
_LL322;} else{ goto _LL323;}} else{ goto _LL323;} _LL323: if(*(( int*) _temp315)
==  Cyc_Absyn_Var_e){ goto _LL324;} else{ goto _LL325;} _LL325: if(*(( int*)
_temp315) ==  Cyc_Absyn_Subscript_e){ _LL352: _temp351=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp315)->f1; goto _LL350; _LL350: _temp349=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp315)->f2; goto _LL326;} else{ goto _LL327;} _LL327: if(*(( int*) _temp315)
==  Cyc_Absyn_StructMember_e){ _LL356: _temp355=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp315)->f1; goto _LL354; _LL354: _temp353=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp315)->f2; goto _LL328;} else{ goto _LL329;} _LL329: if(*(( int*) _temp315)
==  Cyc_Absyn_StructArrow_e){ _LL360: _temp359=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp315)->f1; goto _LL358; _LL358: _temp357=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp315)->f2; goto _LL330;} else{ goto _LL331;} _LL331: if(*(( int*) _temp315)
==  Cyc_Absyn_Deref_e){ _LL362: _temp361=(( struct Cyc_Absyn_Deref_e_struct*)
_temp315)->f1; goto _LL332;} else{ goto _LL333;} _LL333: if(*(( int*) _temp315)
==  Cyc_Absyn_Instantiate_e){ _LL364: _temp363=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp315)->f1; goto _LL334;} else{ goto _LL335;} _LL335: if(*(( int*) _temp315)
==  Cyc_Absyn_NoInstantiate_e){ _LL366: _temp365=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp315)->f1; goto _LL336;} else{ goto _LL337;} _LL337: goto _LL338; _LL318:
return 0; _LL320: _temp347= _temp343; goto _LL322; _LL322: { void* _temp367= Cyc_Tcutil_compress((
void*) _temp347->type); _LL369: if(( unsigned int) _temp367 >  4u?*(( int*)
_temp367) ==  Cyc_Absyn_ArrayType: 0){ goto _LL370;} else{ goto _LL371;} _LL371:
goto _LL372; _LL370: return 0; _LL372: return 1; _LL368:;} _LL324: return 1;
_LL326: return 1; _LL328: return Cyc_Absyn_is_lvalue( _temp355); _LL330: return
1; _LL332: return 1; _LL334: return Cyc_Absyn_is_lvalue( _temp363); _LL336:
return Cyc_Absyn_is_lvalue( _temp365); _LL338: return 0; _LL316:;} struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_List_List* fields, struct _tagged_arr* v){{
struct Cyc_List_List* fs= fields; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Structfield* _temp373=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; if( Cyc_Std_zstrptrcmp( _temp373->name,
v) ==  0){ return( struct Cyc_Absyn_Structfield*) _temp373;}}} return 0;} struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*
sd, struct _tagged_arr* v){ return sd->fields ==  0? 0: Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v, v);}
struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*
ud, struct _tagged_arr* v){ return ud->fields ==  0? 0: Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v, v);}
struct _tuple3* Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List* ts, int i){
for( 0; i !=  0; -- i){ if( ts ==  0){ return 0;} ts=(( struct Cyc_List_List*)
_check_null( ts))->tl;} if( ts ==  0){ return 0;} return( struct _tuple3*)((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( ts))->hd);} struct
_tagged_arr Cyc_Absyn_attribute2string( void* a){ void* _temp374= a; int
_temp418; int _temp420; struct _tagged_arr _temp422; int _temp424; int _temp426;
void* _temp428; int _temp430; int _temp432; void* _temp434; _LL376: if((
unsigned int) _temp374 >  16u?*(( int*) _temp374) ==  Cyc_Absyn_Regparm_att: 0){
_LL419: _temp418=(( struct Cyc_Absyn_Regparm_att_struct*) _temp374)->f1; goto
_LL377;} else{ goto _LL378;} _LL378: if( _temp374 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL379;} else{ goto _LL380;} _LL380: if( _temp374 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL381;} else{ goto _LL382;} _LL382: if( _temp374 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL383;} else{ goto _LL384;} _LL384: if( _temp374 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL385;} else{ goto _LL386;} _LL386: if( _temp374 == ( void*) Cyc_Absyn_Const_att){
goto _LL387;} else{ goto _LL388;} _LL388: if(( unsigned int) _temp374 >  16u?*((
int*) _temp374) ==  Cyc_Absyn_Aligned_att: 0){ _LL421: _temp420=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp374)->f1; goto _LL389;} else{ goto _LL390;} _LL390: if( _temp374 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL391;} else{ goto _LL392;} _LL392: if((
unsigned int) _temp374 >  16u?*(( int*) _temp374) ==  Cyc_Absyn_Section_att: 0){
_LL423: _temp422=(( struct Cyc_Absyn_Section_att_struct*) _temp374)->f1; goto
_LL393;} else{ goto _LL394;} _LL394: if( _temp374 == ( void*) Cyc_Absyn_Nocommon_att){
goto _LL395;} else{ goto _LL396;} _LL396: if( _temp374 == ( void*) Cyc_Absyn_Shared_att){
goto _LL397;} else{ goto _LL398;} _LL398: if( _temp374 == ( void*) Cyc_Absyn_Unused_att){
goto _LL399;} else{ goto _LL400;} _LL400: if( _temp374 == ( void*) Cyc_Absyn_Weak_att){
goto _LL401;} else{ goto _LL402;} _LL402: if( _temp374 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL403;} else{ goto _LL404;} _LL404: if( _temp374 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL405;} else{ goto _LL406;} _LL406: if( _temp374 == ( void*) Cyc_Absyn_No_instrument_function_att){
goto _LL407;} else{ goto _LL408;} _LL408: if( _temp374 == ( void*) Cyc_Absyn_Constructor_att){
goto _LL409;} else{ goto _LL410;} _LL410: if( _temp374 == ( void*) Cyc_Absyn_Destructor_att){
goto _LL411;} else{ goto _LL412;} _LL412: if( _temp374 == ( void*) Cyc_Absyn_No_check_memory_usage_att){
goto _LL413;} else{ goto _LL414;} _LL414: if(( unsigned int) _temp374 >  16u?*((
int*) _temp374) ==  Cyc_Absyn_Format_att: 0){ _LL429: _temp428=( void*)(( struct
Cyc_Absyn_Format_att_struct*) _temp374)->f1; if( _temp428 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL427;} else{ goto _LL416;} _LL427: _temp426=(( struct Cyc_Absyn_Format_att_struct*)
_temp374)->f2; goto _LL425; _LL425: _temp424=(( struct Cyc_Absyn_Format_att_struct*)
_temp374)->f3; goto _LL415;} else{ goto _LL416;} _LL416: if(( unsigned int)
_temp374 >  16u?*(( int*) _temp374) ==  Cyc_Absyn_Format_att: 0){ _LL435:
_temp434=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp374)->f1; if(
_temp434 == ( void*) Cyc_Absyn_Scanf_ft){ goto _LL433;} else{ goto _LL375;}
_LL433: _temp432=(( struct Cyc_Absyn_Format_att_struct*) _temp374)->f2; goto
_LL431; _LL431: _temp430=(( struct Cyc_Absyn_Format_att_struct*) _temp374)->f3;
goto _LL417;} else{ goto _LL375;} _LL377: return( struct _tagged_arr)({ struct
Cyc_Std_Int_pa_struct _temp437; _temp437.tag= Cyc_Std_Int_pa; _temp437.f1=( int)((
unsigned int) _temp418);{ void* _temp436[ 1u]={& _temp437}; Cyc_Std_aprintf(
_tag_arr("regparm(%d)", sizeof( unsigned char), 12u), _tag_arr( _temp436,
sizeof( void*), 1u));}}); _LL379: return _tag_arr("stdcall", sizeof(
unsigned char), 8u); _LL381: return _tag_arr("cdecl", sizeof( unsigned char), 6u);
_LL383: return _tag_arr("fastcall", sizeof( unsigned char), 9u); _LL385: return
_tag_arr("noreturn", sizeof( unsigned char), 9u); _LL387: return _tag_arr("const",
sizeof( unsigned char), 6u); _LL389: if( _temp420 ==  - 1){ return _tag_arr("aligned",
sizeof( unsigned char), 8u);} else{ return( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp439; _temp439.tag= Cyc_Std_Int_pa; _temp439.f1=( int)(( unsigned int)
_temp420);{ void* _temp438[ 1u]={& _temp439}; Cyc_Std_aprintf( _tag_arr("aligned(%d)",
sizeof( unsigned char), 12u), _tag_arr( _temp438, sizeof( void*), 1u));}});}
_LL391: return _tag_arr("packed", sizeof( unsigned char), 7u); _LL393: return(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp441; _temp441.tag=
Cyc_Std_String_pa; _temp441.f1=( struct _tagged_arr) _temp422;{ void* _temp440[
1u]={& _temp441}; Cyc_Std_aprintf( _tag_arr("section(\"%s\")", sizeof(
unsigned char), 14u), _tag_arr( _temp440, sizeof( void*), 1u));}}); _LL395:
return _tag_arr("nocommon", sizeof( unsigned char), 9u); _LL397: return _tag_arr("shared",
sizeof( unsigned char), 7u); _LL399: return _tag_arr("unused", sizeof(
unsigned char), 7u); _LL401: return _tag_arr("weak", sizeof( unsigned char), 5u);
_LL403: return _tag_arr("dllimport", sizeof( unsigned char), 10u); _LL405:
return _tag_arr("dllexport", sizeof( unsigned char), 10u); _LL407: return
_tag_arr("no_instrument_function", sizeof( unsigned char), 23u); _LL409: return
_tag_arr("constructor", sizeof( unsigned char), 12u); _LL411: return _tag_arr("destructor",
sizeof( unsigned char), 11u); _LL413: return _tag_arr("no_check_memory_usage",
sizeof( unsigned char), 22u); _LL415: return( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp444; _temp444.tag= Cyc_Std_Int_pa; _temp444.f1=( unsigned int) _temp424;{
struct Cyc_Std_Int_pa_struct _temp443; _temp443.tag= Cyc_Std_Int_pa; _temp443.f1=(
unsigned int) _temp426;{ void* _temp442[ 2u]={& _temp443,& _temp444}; Cyc_Std_aprintf(
_tag_arr("format(printf,%u,%u)", sizeof( unsigned char), 21u), _tag_arr(
_temp442, sizeof( void*), 2u));}}}); _LL417: return( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp447; _temp447.tag= Cyc_Std_Int_pa; _temp447.f1=(
unsigned int) _temp430;{ struct Cyc_Std_Int_pa_struct _temp446; _temp446.tag=
Cyc_Std_Int_pa; _temp446.f1=( unsigned int) _temp432;{ void* _temp445[ 2u]={&
_temp446,& _temp447}; Cyc_Std_aprintf( _tag_arr("format(scanf,%u,%u)", sizeof(
unsigned char), 20u), _tag_arr( _temp445, sizeof( void*), 2u));}}}); _LL375:;}
int Cyc_Absyn_fntype_att( void* a){ void* _temp448= a; _LL450: if(( unsigned int)
_temp448 >  16u?*(( int*) _temp448) ==  Cyc_Absyn_Regparm_att: 0){ goto _LL451;}
else{ goto _LL452;} _LL452: if( _temp448 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL453;} else{ goto _LL454;} _LL454: if( _temp448 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL455;} else{ goto _LL456;} _LL456: if( _temp448 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL457;} else{ goto _LL458;} _LL458: if( _temp448 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL459;} else{ goto _LL460;} _LL460: if(( unsigned int) _temp448 >  16u?*((
int*) _temp448) ==  Cyc_Absyn_Format_att: 0){ goto _LL461;} else{ goto _LL462;}
_LL462: if( _temp448 == ( void*) Cyc_Absyn_Const_att){ goto _LL463;} else{ goto
_LL464;} _LL464: goto _LL465; _LL451: goto _LL453; _LL453: goto _LL455; _LL455:
goto _LL457; _LL457: goto _LL459; _LL459: goto _LL461; _LL461: goto _LL463;
_LL463: return 1; _LL465: return 0; _LL449:;} static unsigned char _temp466[ 3u]="f0";
static struct _tagged_arr Cyc_Absyn_f0={ _temp466, _temp466, _temp466 +  3u};
static struct _tagged_arr* Cyc_Absyn_field_names_v[ 1u]={& Cyc_Absyn_f0}; static
struct _tagged_arr Cyc_Absyn_field_names={( void*)(( struct _tagged_arr**) Cyc_Absyn_field_names_v),(
void*)(( struct _tagged_arr**) Cyc_Absyn_field_names_v),( void*)(( struct
_tagged_arr**) Cyc_Absyn_field_names_v +  1u)}; struct _tagged_arr* Cyc_Absyn_fieldname(
int i){ unsigned int fsz= _get_arr_size( Cyc_Absyn_field_names, sizeof( struct
_tagged_arr*)); if( i >=  fsz){ Cyc_Absyn_field_names=({ unsigned int _temp467=(
unsigned int)( i +  1); struct _tagged_arr** _temp468=( struct _tagged_arr**)
_cycalloc( _check_times( sizeof( struct _tagged_arr*), _temp467)); struct
_tagged_arr _temp473= _tag_arr( _temp468, sizeof( struct _tagged_arr*),(
unsigned int)( i +  1));{ unsigned int _temp469= _temp467; unsigned int j; for(
j= 0; j <  _temp469; j ++){ _temp468[ j]= j <  fsz?*(( struct _tagged_arr**)
_check_unknown_subscript( Cyc_Absyn_field_names, sizeof( struct _tagged_arr*),(
int) j)):({ struct _tagged_arr* _temp470=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp470[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp472; _temp472.tag= Cyc_Std_Int_pa; _temp472.f1=( int) j;{ void* _temp471[ 1u]={&
_temp472}; Cyc_Std_aprintf( _tag_arr("f%d", sizeof( unsigned char), 4u),
_tag_arr( _temp471, sizeof( void*), 1u));}}); _temp470;});}}; _temp473;});}
return*(( struct _tagged_arr**) _check_unknown_subscript( Cyc_Absyn_field_names,
sizeof( struct _tagged_arr*), i));} void Cyc_Absyn_print_decls( struct Cyc_List_List*
decls){(( void(*)( void* rep, struct Cyc_List_List** val)) Cyc_Marshal_print_type)(
Cyc_decls_rep,& decls);({ void* _temp474[ 0u]={}; Cyc_Std_printf( _tag_arr("\n",
sizeof( unsigned char), 2u), _tag_arr( _temp474, sizeof( void*), 0u));});}
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Decl_446H2; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Decl;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Raw_decl; extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_286; static struct Cyc_Typerep_Int_struct
Cyc__genrep_4={ 1u, 32}; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_98;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Vardecl; extern struct
Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Scope; static struct _tuple4* Cyc__genarr_102[
0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Scope={ 5u,{( void*)((
struct _tuple4**) Cyc__genarr_102),( void*)(( struct _tuple4**) Cyc__genarr_102),(
void*)(( struct _tuple4**) Cyc__genarr_102 +  0u)}}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_9; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_10; extern
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Nmspace; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_14; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_15; extern
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0const_unsigned_char_3446H2;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11; extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_12; static struct Cyc_Typerep_Int_struct Cyc__genrep_13={ 1u, 8};
static struct Cyc_Typerep_FatPtr_struct Cyc__genrep_12={ 3u,( void*)(( void*)&
Cyc__genrep_13)}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11={ 2u,
1,( void*)(( void*)& Cyc__genrep_12)}; static struct _tuple4 Cyc__gentuple_16={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_11}; static struct
_tuple4 Cyc__gentuple_17={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_15};
static struct _tuple4* Cyc__genarr_18[ 2u]={& Cyc__gentuple_16,& Cyc__gentuple_17};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0const_unsigned_char_3446H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_18),(
void*)(( struct _tuple4**) Cyc__genarr_18),( void*)(( struct _tuple4**) Cyc__genarr_18
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_15={ 2u, 1,( void*)((
void*)& Cyc_struct_List_List0const_unsigned_char_3446H2)}; struct _tuple8{
unsigned int f1; struct Cyc_List_List* f2; } ; static struct _tuple4 Cyc__gentuple_19={
offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_20={
offsetof( struct _tuple8,f2),( void*)& Cyc__genrep_15}; static struct _tuple4*
Cyc__genarr_21[ 2u]={& Cyc__gentuple_19,& Cyc__gentuple_20}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_14={ 4u, sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_21),(
void*)(( struct _tuple4**) Cyc__genarr_21),( void*)(( struct _tuple4**) Cyc__genarr_21
+  2u)}}; static struct _tuple4 Cyc__gentuple_22={ 0,( void*)& Cyc__genrep_14};
static struct _tuple4 Cyc__gentuple_23={ 1,( void*)& Cyc__genrep_14}; static
struct _tuple4* Cyc__genarr_24[ 2u]={& Cyc__gentuple_22,& Cyc__gentuple_23};
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Nmspace={ 5u,{( void*)((
struct _tuple4**) Cyc__genarr_24),( void*)(( struct _tuple4**) Cyc__genarr_24),(
void*)(( struct _tuple4**) Cyc__genarr_24 +  2u)}}; static struct _tuple4 Cyc__gentuple_25={
offsetof( struct _tuple0,f1),( void*)& Cyc_tunion_Absyn_Nmspace}; static struct
_tuple4 Cyc__gentuple_26={ offsetof( struct _tuple0,f2),( void*)& Cyc__genrep_11};
static struct _tuple4* Cyc__genarr_27[ 2u]={& Cyc__gentuple_25,& Cyc__gentuple_26};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_10={ 4u, sizeof( struct
_tuple0),{( void*)(( struct _tuple4**) Cyc__genarr_27),( void*)(( struct _tuple4**)
Cyc__genarr_27),( void*)(( struct _tuple4**) Cyc__genarr_27 +  2u)}}; static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_9={ 2u, 1,( void*)(( void*)& Cyc__genrep_10)};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_99; struct _tuple9{
unsigned char f1; } ; static struct _tuple4 Cyc__gentuple_100={ offsetof( struct
_tuple9,f1),( void*)(( void*)& Cyc__genrep_13)}; static struct _tuple4* Cyc__genarr_101[
1u]={& Cyc__gentuple_100}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_99={
4u, sizeof( struct _tuple9),{( void*)(( struct _tuple4**) Cyc__genarr_101),(
void*)(( struct _tuple4**) Cyc__genarr_101),( void*)(( struct _tuple4**) Cyc__genarr_101
+  1u)}}; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Type; extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_908; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_912; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0tunion_Absyn_Kind2;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Kind; static struct
_tuple4* Cyc__genarr_130[ 0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Kind={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_130),( void*)(( struct _tuple4**)
Cyc__genarr_130),( void*)(( struct _tuple4**) Cyc__genarr_130 +  0u)}}; static
struct _tuple4 Cyc__gentuple_913={ offsetof( struct Cyc_Core_Opt,v),( void*)&
Cyc_tunion_Absyn_Kind}; static struct _tuple4* Cyc__genarr_914[ 1u]={& Cyc__gentuple_913};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0tunion_Absyn_Kind2={ 4u,
sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_914),(
void*)(( struct _tuple4**) Cyc__genarr_914),( void*)(( struct _tuple4**) Cyc__genarr_914
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_912={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0tunion_Absyn_Kind2)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_52; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0tunion_Absyn_Type2;
static struct _tuple4 Cyc__gentuple_53={ offsetof( struct Cyc_Core_Opt,v),( void*)&
Cyc_tunion_Absyn_Type}; static struct _tuple4* Cyc__genarr_54[ 1u]={& Cyc__gentuple_53};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0tunion_Absyn_Type2={ 4u,
sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_54),(
void*)(( struct _tuple4**) Cyc__genarr_54),( void*)(( struct _tuple4**) Cyc__genarr_54
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_52={ 2u, 1,( void*)((
void*)& Cyc_struct_Core_Opt0tunion_Absyn_Type2)}; static struct Cyc_Typerep_Int_struct
Cyc__genrep_73={ 1u, 32}; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_909;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Tvar_446H2_22;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_174; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Tvar_446H2; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_123; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Tvar;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_139; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_139={ 2u, 1,( void*)(( void*)& Cyc__genrep_73)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_124; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Conref0tunion_Absyn_Kind2;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0tunion_Absyn_Kind2;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_129; static struct _tuple4
Cyc__gentuple_131={ offsetof( struct _tuple4,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_132={ offsetof( struct _tuple4,f2),( void*)&
Cyc_tunion_Absyn_Kind}; static struct _tuple4* Cyc__genarr_133[ 2u]={& Cyc__gentuple_131,&
Cyc__gentuple_132}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_129={ 4u,
sizeof( struct _tuple4),{( void*)(( struct _tuple4**) Cyc__genarr_133),( void*)((
struct _tuple4**) Cyc__genarr_133),( void*)(( struct _tuple4**) Cyc__genarr_133
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_125; struct
_tuple10{ unsigned int f1; struct Cyc_Absyn_Conref* f2; } ; static struct
_tuple4 Cyc__gentuple_126={ offsetof( struct _tuple10,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_127={ offsetof( struct _tuple10,f2),( void*)&
Cyc__genrep_124}; static struct _tuple4* Cyc__genarr_128[ 2u]={& Cyc__gentuple_126,&
Cyc__gentuple_127}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_125={ 4u,
sizeof( struct _tuple10),{( void*)(( struct _tuple4**) Cyc__genarr_128),( void*)((
struct _tuple4**) Cyc__genarr_128),( void*)(( struct _tuple4**) Cyc__genarr_128
+  2u)}}; static struct _tuple4 Cyc__gentuple_134={ 0,( void*)& Cyc__genrep_129};
static struct _tuple4 Cyc__gentuple_135={ 1,( void*)& Cyc__genrep_125}; static
struct _tuple4* Cyc__genarr_136[ 2u]={& Cyc__gentuple_134,& Cyc__gentuple_135};
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0tunion_Absyn_Kind2={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_136),( void*)(( struct _tuple4**)
Cyc__genarr_136),( void*)(( struct _tuple4**) Cyc__genarr_136 +  2u)}}; static
struct _tuple4 Cyc__gentuple_137={ offsetof( struct Cyc_Absyn_Conref,v),( void*)&
Cyc_tunion_Absyn_Constraint0tunion_Absyn_Kind2}; static struct _tuple4* Cyc__genarr_138[
1u]={& Cyc__gentuple_137}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Conref0tunion_Absyn_Kind2={
4u, sizeof( struct Cyc_Absyn_Conref),{( void*)(( struct _tuple4**) Cyc__genarr_138),(
void*)(( struct _tuple4**) Cyc__genarr_138),( void*)(( struct _tuple4**) Cyc__genarr_138
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_124={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Conref0tunion_Absyn_Kind2)}; static struct
_tuple4 Cyc__gentuple_140={ offsetof( struct Cyc_Absyn_Tvar,name),( void*)& Cyc__genrep_11};
static struct _tuple4 Cyc__gentuple_141={ offsetof( struct Cyc_Absyn_Tvar,identity),(
void*)& Cyc__genrep_139}; static struct _tuple4 Cyc__gentuple_142={ offsetof(
struct Cyc_Absyn_Tvar,kind),( void*)& Cyc__genrep_124}; static struct _tuple4*
Cyc__genarr_143[ 3u]={& Cyc__gentuple_140,& Cyc__gentuple_141,& Cyc__gentuple_142};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Tvar={ 4u, sizeof( struct Cyc_Absyn_Tvar),{(
void*)(( struct _tuple4**) Cyc__genarr_143),( void*)(( struct _tuple4**) Cyc__genarr_143),(
void*)(( struct _tuple4**) Cyc__genarr_143 +  3u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_123={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Tvar)}; static
struct _tuple4 Cyc__gentuple_175={ offsetof( struct Cyc_List_List,hd),( void*)&
Cyc__genrep_123}; static struct _tuple4 Cyc__gentuple_176={ offsetof( struct Cyc_List_List,tl),(
void*)& Cyc__genrep_174}; static struct _tuple4* Cyc__genarr_177[ 2u]={& Cyc__gentuple_175,&
Cyc__gentuple_176}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0struct_Absyn_Tvar_446H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_177),(
void*)(( struct _tuple4**) Cyc__genarr_177),( void*)(( struct _tuple4**) Cyc__genarr_177
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_174={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List0struct_Absyn_Tvar_446H2)}; static struct
_tuple4 Cyc__gentuple_910={ offsetof( struct Cyc_Core_Opt,v),( void*)& Cyc__genrep_174};
static struct _tuple4* Cyc__genarr_911[ 1u]={& Cyc__gentuple_910}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Tvar_446H2_22={ 4u, sizeof(
struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_911),( void*)((
struct _tuple4**) Cyc__genarr_911),( void*)(( struct _tuple4**) Cyc__genarr_911
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_909={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Tvar_446H2_22)};
struct _tuple11{ unsigned int f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt*
f3; int f4; struct Cyc_Core_Opt* f5; } ; static struct _tuple4 Cyc__gentuple_915={
offsetof( struct _tuple11,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_916={ offsetof( struct _tuple11,f2),( void*)& Cyc__genrep_912};
static struct _tuple4 Cyc__gentuple_917={ offsetof( struct _tuple11,f3),( void*)&
Cyc__genrep_52}; static struct _tuple4 Cyc__gentuple_918={ offsetof( struct
_tuple11,f4),( void*)(( void*)& Cyc__genrep_73)}; static struct _tuple4 Cyc__gentuple_919={
offsetof( struct _tuple11,f5),( void*)& Cyc__genrep_909}; static struct _tuple4*
Cyc__genarr_920[ 5u]={& Cyc__gentuple_915,& Cyc__gentuple_916,& Cyc__gentuple_917,&
Cyc__gentuple_918,& Cyc__gentuple_919}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_908={ 4u, sizeof( struct _tuple11),{( void*)(( struct _tuple4**) Cyc__genarr_920),(
void*)(( struct _tuple4**) Cyc__genarr_920),( void*)(( struct _tuple4**) Cyc__genarr_920
+  5u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_904; struct
_tuple12{ unsigned int f1; struct Cyc_Absyn_Tvar* f2; } ; static struct _tuple4
Cyc__gentuple_905={ offsetof( struct _tuple12,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_906={ offsetof( struct _tuple12,f2),( void*)&
Cyc__genrep_123}; static struct _tuple4* Cyc__genarr_907[ 2u]={& Cyc__gentuple_905,&
Cyc__gentuple_906}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_904={ 4u,
sizeof( struct _tuple12),{( void*)(( struct _tuple4**) Cyc__genarr_907),( void*)((
struct _tuple4**) Cyc__genarr_907),( void*)(( struct _tuple4**) Cyc__genarr_907
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_881; extern struct
Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_TunionInfo; extern struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_TunionInfoU; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_887;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_UnknownTunionInfo;
static struct _tuple4 Cyc__gentuple_888={ offsetof( struct Cyc_Absyn_UnknownTunionInfo,name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_889={ offsetof(
struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),( void*)(( void*)& Cyc__genrep_73)};
static struct _tuple4* Cyc__genarr_890[ 2u]={& Cyc__gentuple_888,& Cyc__gentuple_889};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_UnknownTunionInfo={ 4u, sizeof(
struct Cyc_Absyn_UnknownTunionInfo),{( void*)(( struct _tuple4**) Cyc__genarr_890),(
void*)(( struct _tuple4**) Cyc__genarr_890),( void*)(( struct _tuple4**) Cyc__genarr_890
+  2u)}}; struct _tuple13{ unsigned int f1; struct Cyc_Absyn_UnknownTunionInfo
f2; } ; static struct _tuple4 Cyc__gentuple_891={ offsetof( struct _tuple13,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_892={ offsetof(
struct _tuple13,f2),( void*)& Cyc_struct_Absyn_UnknownTunionInfo}; static struct
_tuple4* Cyc__genarr_893[ 2u]={& Cyc__gentuple_891,& Cyc__gentuple_892}; static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_887={ 4u, sizeof( struct _tuple13),{(
void*)(( struct _tuple4**) Cyc__genarr_893),( void*)(( struct _tuple4**) Cyc__genarr_893),(
void*)(( struct _tuple4**) Cyc__genarr_893 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_882; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_883;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_232; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Tuniondecl; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_233;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Tunionfield_446H2_22;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_234; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Tunionfield_446H2; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_216; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Tunionfield;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_217; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List060struct_Absyn_Tqual4tunion_Absyn_Type1_446H2; extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_218; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_219; static struct _tuple4 Cyc__gentuple_220={ offsetof( struct
_tuple3,f1),( void*)& Cyc__genrep_99}; static struct _tuple4 Cyc__gentuple_221={
offsetof( struct _tuple3,f2),( void*)& Cyc_tunion_Absyn_Type}; static struct
_tuple4* Cyc__genarr_222[ 2u]={& Cyc__gentuple_220,& Cyc__gentuple_221}; static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_219={ 4u, sizeof( struct _tuple3),{(
void*)(( struct _tuple4**) Cyc__genarr_222),( void*)(( struct _tuple4**) Cyc__genarr_222),(
void*)(( struct _tuple4**) Cyc__genarr_222 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_218={ 2u, 1,( void*)(( void*)& Cyc__genrep_219)}; static struct
_tuple4 Cyc__gentuple_223={ offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_218};
static struct _tuple4 Cyc__gentuple_224={ offsetof( struct Cyc_List_List,tl),(
void*)& Cyc__genrep_217}; static struct _tuple4* Cyc__genarr_225[ 2u]={& Cyc__gentuple_223,&
Cyc__gentuple_224}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060struct_Absyn_Tqual4tunion_Absyn_Type1_446H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_225),(
void*)(( struct _tuple4**) Cyc__genarr_225),( void*)(( struct _tuple4**) Cyc__genarr_225
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_217={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060struct_Absyn_Tqual4tunion_Absyn_Type1_446H2)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_2={ 2u, 1,( void*)(( void*)& Cyc_struct_Position_Segment)}; static
struct _tuple4 Cyc__gentuple_226={ offsetof( struct Cyc_Absyn_Tunionfield,name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_227={ offsetof(
struct Cyc_Absyn_Tunionfield,tvs),( void*)& Cyc__genrep_174}; static struct
_tuple4 Cyc__gentuple_228={ offsetof( struct Cyc_Absyn_Tunionfield,typs),( void*)&
Cyc__genrep_217}; static struct _tuple4 Cyc__gentuple_229={ offsetof( struct Cyc_Absyn_Tunionfield,loc),(
void*)& Cyc__genrep_2}; static struct _tuple4 Cyc__gentuple_230={ offsetof(
struct Cyc_Absyn_Tunionfield,sc),( void*)& Cyc_tunion_Absyn_Scope}; static
struct _tuple4* Cyc__genarr_231[ 5u]={& Cyc__gentuple_226,& Cyc__gentuple_227,&
Cyc__gentuple_228,& Cyc__gentuple_229,& Cyc__gentuple_230}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Tunionfield={ 4u, sizeof( struct Cyc_Absyn_Tunionfield),{( void*)((
struct _tuple4**) Cyc__genarr_231),( void*)(( struct _tuple4**) Cyc__genarr_231),(
void*)(( struct _tuple4**) Cyc__genarr_231 +  5u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_216={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Tunionfield)};
static struct _tuple4 Cyc__gentuple_235={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_216}; static struct _tuple4 Cyc__gentuple_236={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_234}; static struct _tuple4* Cyc__genarr_237[
2u]={& Cyc__gentuple_235,& Cyc__gentuple_236}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Tunionfield_446H2={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_237),( void*)(( struct _tuple4**) Cyc__genarr_237),(
void*)(( struct _tuple4**) Cyc__genarr_237 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_234={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0struct_Absyn_Tunionfield_446H2)};
static struct _tuple4 Cyc__gentuple_238={ offsetof( struct Cyc_Core_Opt,v),(
void*)& Cyc__genrep_234}; static struct _tuple4* Cyc__genarr_239[ 1u]={& Cyc__gentuple_238};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Tunionfield_446H2_22={
4u, sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_239),(
void*)(( struct _tuple4**) Cyc__genarr_239),( void*)(( struct _tuple4**) Cyc__genarr_239
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_233={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Tunionfield_446H2_22)};
static struct _tuple4 Cyc__gentuple_240={ offsetof( struct Cyc_Absyn_Tuniondecl,sc),(
void*)& Cyc_tunion_Absyn_Scope}; static struct _tuple4 Cyc__gentuple_241={
offsetof( struct Cyc_Absyn_Tuniondecl,name),( void*)& Cyc__genrep_9}; static
struct _tuple4 Cyc__gentuple_242={ offsetof( struct Cyc_Absyn_Tuniondecl,tvs),(
void*)& Cyc__genrep_174}; static struct _tuple4 Cyc__gentuple_243={ offsetof(
struct Cyc_Absyn_Tuniondecl,fields),( void*)& Cyc__genrep_233}; static struct
_tuple4 Cyc__gentuple_244={ offsetof( struct Cyc_Absyn_Tuniondecl,is_xtunion),(
void*)(( void*)& Cyc__genrep_73)}; static struct _tuple4* Cyc__genarr_245[ 5u]={&
Cyc__gentuple_240,& Cyc__gentuple_241,& Cyc__gentuple_242,& Cyc__gentuple_243,&
Cyc__gentuple_244}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Tuniondecl={
4u, sizeof( struct Cyc_Absyn_Tuniondecl),{( void*)(( struct _tuple4**) Cyc__genarr_245),(
void*)(( struct _tuple4**) Cyc__genarr_245),( void*)(( struct _tuple4**) Cyc__genarr_245
+  5u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_232={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Tuniondecl)}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_883={ 2u, 1,( void*)(( void*)& Cyc__genrep_232)}; struct _tuple14{
unsigned int f1; struct Cyc_Absyn_Tuniondecl** f2; } ; static struct _tuple4 Cyc__gentuple_884={
offsetof( struct _tuple14,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_885={ offsetof( struct _tuple14,f2),( void*)& Cyc__genrep_883};
static struct _tuple4* Cyc__genarr_886[ 2u]={& Cyc__gentuple_884,& Cyc__gentuple_885};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_882={ 4u, sizeof( struct
_tuple14),{( void*)(( struct _tuple4**) Cyc__genarr_886),( void*)(( struct
_tuple4**) Cyc__genarr_886),( void*)(( struct _tuple4**) Cyc__genarr_886 +  2u)}};
static struct _tuple4 Cyc__gentuple_894={ 0,( void*)& Cyc__genrep_887}; static
struct _tuple4 Cyc__gentuple_895={ 1,( void*)& Cyc__genrep_882}; static struct
_tuple4* Cyc__genarr_896[ 2u]={& Cyc__gentuple_894,& Cyc__gentuple_895}; struct
Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU={ 5u,{( void*)(( struct
_tuple4**) Cyc__genarr_896),( void*)(( struct _tuple4**) Cyc__genarr_896),( void*)((
struct _tuple4**) Cyc__genarr_896 +  2u)}}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_44; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0tunion_Absyn_Type46H2;
static struct _tuple4 Cyc__gentuple_45={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc_tunion_Absyn_Type}; static struct _tuple4 Cyc__gentuple_46={
offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_44}; static struct
_tuple4* Cyc__genarr_47[ 2u]={& Cyc__gentuple_45,& Cyc__gentuple_46}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0tunion_Absyn_Type46H2={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_47),( void*)(( struct _tuple4**) Cyc__genarr_47),(
void*)(( struct _tuple4**) Cyc__genarr_47 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_44={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0tunion_Absyn_Type46H2)};
static struct _tuple4 Cyc__gentuple_897={ offsetof( struct Cyc_Absyn_TunionInfo,tunion_info),(
void*)& Cyc_tunion_Absyn_TunionInfoU}; static struct _tuple4 Cyc__gentuple_898={
offsetof( struct Cyc_Absyn_TunionInfo,targs),( void*)& Cyc__genrep_44}; static
struct _tuple4 Cyc__gentuple_899={ offsetof( struct Cyc_Absyn_TunionInfo,rgn),(
void*)& Cyc_tunion_Absyn_Type}; static struct _tuple4* Cyc__genarr_900[ 3u]={&
Cyc__gentuple_897,& Cyc__gentuple_898,& Cyc__gentuple_899}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_TunionInfo={ 4u, sizeof( struct Cyc_Absyn_TunionInfo),{( void*)((
struct _tuple4**) Cyc__genarr_900),( void*)(( struct _tuple4**) Cyc__genarr_900),(
void*)(( struct _tuple4**) Cyc__genarr_900 +  3u)}}; struct _tuple15{
unsigned int f1; struct Cyc_Absyn_TunionInfo f2; } ; static struct _tuple4 Cyc__gentuple_901={
offsetof( struct _tuple15,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_902={ offsetof( struct _tuple15,f2),( void*)& Cyc_struct_Absyn_TunionInfo};
static struct _tuple4* Cyc__genarr_903[ 2u]={& Cyc__gentuple_901,& Cyc__gentuple_902};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_881={ 4u, sizeof( struct
_tuple15),{( void*)(( struct _tuple4**) Cyc__genarr_903),( void*)(( struct
_tuple4**) Cyc__genarr_903),( void*)(( struct _tuple4**) Cyc__genarr_903 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_858; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_TunionFieldInfo; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_864; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo; static struct _tuple4 Cyc__gentuple_865={
offsetof( struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),( void*)& Cyc__genrep_9};
static struct _tuple4 Cyc__gentuple_866={ offsetof( struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_867={ offsetof(
struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),( void*)(( void*)& Cyc__genrep_73)};
static struct _tuple4* Cyc__genarr_868[ 3u]={& Cyc__gentuple_865,& Cyc__gentuple_866,&
Cyc__gentuple_867}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_UnknownTunionFieldInfo={
4u, sizeof( struct Cyc_Absyn_UnknownTunionFieldInfo),{( void*)(( struct _tuple4**)
Cyc__genarr_868),( void*)(( struct _tuple4**) Cyc__genarr_868),( void*)(( struct
_tuple4**) Cyc__genarr_868 +  3u)}}; struct _tuple16{ unsigned int f1; struct
Cyc_Absyn_UnknownTunionFieldInfo f2; } ; static struct _tuple4 Cyc__gentuple_869={
offsetof( struct _tuple16,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_870={ offsetof( struct _tuple16,f2),( void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo};
static struct _tuple4* Cyc__genarr_871[ 2u]={& Cyc__gentuple_869,& Cyc__gentuple_870};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_864={ 4u, sizeof( struct
_tuple16),{( void*)(( struct _tuple4**) Cyc__genarr_871),( void*)(( struct
_tuple4**) Cyc__genarr_871),( void*)(( struct _tuple4**) Cyc__genarr_871 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_859; struct _tuple17{
unsigned int f1; struct Cyc_Absyn_Tuniondecl* f2; struct Cyc_Absyn_Tunionfield*
f3; } ; static struct _tuple4 Cyc__gentuple_860={ offsetof( struct _tuple17,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_861={ offsetof(
struct _tuple17,f2),( void*)(( void*)& Cyc__genrep_232)}; static struct _tuple4
Cyc__gentuple_862={ offsetof( struct _tuple17,f3),( void*)& Cyc__genrep_216};
static struct _tuple4* Cyc__genarr_863[ 3u]={& Cyc__gentuple_860,& Cyc__gentuple_861,&
Cyc__gentuple_862}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_859={ 4u,
sizeof( struct _tuple17),{( void*)(( struct _tuple4**) Cyc__genarr_863),( void*)((
struct _tuple4**) Cyc__genarr_863),( void*)(( struct _tuple4**) Cyc__genarr_863
+  3u)}}; static struct _tuple4 Cyc__gentuple_872={ 0,( void*)& Cyc__genrep_864};
static struct _tuple4 Cyc__gentuple_873={ 1,( void*)& Cyc__genrep_859}; static
struct _tuple4* Cyc__genarr_874[ 2u]={& Cyc__gentuple_872,& Cyc__gentuple_873};
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU={ 5u,{( void*)((
struct _tuple4**) Cyc__genarr_874),( void*)(( struct _tuple4**) Cyc__genarr_874),(
void*)(( struct _tuple4**) Cyc__genarr_874 +  2u)}}; static struct _tuple4 Cyc__gentuple_875={
offsetof( struct Cyc_Absyn_TunionFieldInfo,field_info),( void*)& Cyc_tunion_Absyn_TunionFieldInfoU};
static struct _tuple4 Cyc__gentuple_876={ offsetof( struct Cyc_Absyn_TunionFieldInfo,targs),(
void*)& Cyc__genrep_44}; static struct _tuple4* Cyc__genarr_877[ 2u]={& Cyc__gentuple_875,&
Cyc__gentuple_876}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_TunionFieldInfo={
4u, sizeof( struct Cyc_Absyn_TunionFieldInfo),{( void*)(( struct _tuple4**) Cyc__genarr_877),(
void*)(( struct _tuple4**) Cyc__genarr_877),( void*)(( struct _tuple4**) Cyc__genarr_877
+  2u)}}; struct _tuple18{ unsigned int f1; struct Cyc_Absyn_TunionFieldInfo f2;
} ; static struct _tuple4 Cyc__gentuple_878={ offsetof( struct _tuple18,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_879={ offsetof(
struct _tuple18,f2),( void*)& Cyc_struct_Absyn_TunionFieldInfo}; static struct
_tuple4* Cyc__genarr_880[ 2u]={& Cyc__gentuple_878,& Cyc__gentuple_879}; static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_858={ 4u, sizeof( struct _tuple18),{(
void*)(( struct _tuple4**) Cyc__genarr_880),( void*)(( struct _tuple4**) Cyc__genarr_880),(
void*)(( struct _tuple4**) Cyc__genarr_880 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_822; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_PtrInfo;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_839; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Conref0int2; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0int2;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_84; struct _tuple19{
unsigned int f1; int f2; } ; static struct _tuple4 Cyc__gentuple_85={ offsetof(
struct _tuple19,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_86={
offsetof( struct _tuple19,f2),( void*)(( void*)& Cyc__genrep_73)}; static struct
_tuple4* Cyc__genarr_87[ 2u]={& Cyc__gentuple_85,& Cyc__gentuple_86}; static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_84={ 4u, sizeof( struct _tuple19),{(
void*)(( struct _tuple4**) Cyc__genarr_87),( void*)(( struct _tuple4**) Cyc__genarr_87),(
void*)(( struct _tuple4**) Cyc__genarr_87 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_840; static struct _tuple4 Cyc__gentuple_841={ offsetof( struct
_tuple10,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_842={
offsetof( struct _tuple10,f2),( void*)& Cyc__genrep_839}; static struct _tuple4*
Cyc__genarr_843[ 2u]={& Cyc__gentuple_841,& Cyc__gentuple_842}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_840={ 4u, sizeof( struct _tuple10),{( void*)((
struct _tuple4**) Cyc__genarr_843),( void*)(( struct _tuple4**) Cyc__genarr_843),(
void*)(( struct _tuple4**) Cyc__genarr_843 +  2u)}}; static struct _tuple4 Cyc__gentuple_844={
0,( void*)& Cyc__genrep_84}; static struct _tuple4 Cyc__gentuple_845={ 1,( void*)&
Cyc__genrep_840}; static struct _tuple4* Cyc__genarr_846[ 2u]={& Cyc__gentuple_844,&
Cyc__gentuple_845}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0int2={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_846),( void*)(( struct _tuple4**)
Cyc__genarr_846),( void*)(( struct _tuple4**) Cyc__genarr_846 +  2u)}}; static
struct _tuple4 Cyc__gentuple_847={ offsetof( struct Cyc_Absyn_Conref,v),( void*)&
Cyc_tunion_Absyn_Constraint0int2}; static struct _tuple4* Cyc__genarr_848[ 1u]={&
Cyc__gentuple_847}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Conref0int2={
4u, sizeof( struct Cyc_Absyn_Conref),{( void*)(( struct _tuple4**) Cyc__genarr_848),(
void*)(( struct _tuple4**) Cyc__genarr_848),( void*)(( struct _tuple4**) Cyc__genarr_848
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_839={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Conref0int2)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_823; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Conref0tunion_Absyn_Bounds2;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0tunion_Absyn_Bounds2;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_828; extern struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_Bounds; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_64;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_65; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Exp; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Raw_exp;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_677; extern struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_Cnst; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_690;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Sign; static struct
_tuple4* Cyc__genarr_303[ 0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Sign={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_303),( void*)(( struct _tuple4**)
Cyc__genarr_303),( void*)(( struct _tuple4**) Cyc__genarr_303 +  0u)}}; struct
_tuple20{ unsigned int f1; void* f2; unsigned char f3; } ; static struct _tuple4
Cyc__gentuple_691={ offsetof( struct _tuple20,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_692={ offsetof( struct _tuple20,f2),( void*)&
Cyc_tunion_Absyn_Sign}; static struct _tuple4 Cyc__gentuple_693={ offsetof(
struct _tuple20,f3),( void*)(( void*)& Cyc__genrep_13)}; static struct _tuple4*
Cyc__genarr_694[ 3u]={& Cyc__gentuple_691,& Cyc__gentuple_692,& Cyc__gentuple_693};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_690={ 4u, sizeof( struct
_tuple20),{( void*)(( struct _tuple4**) Cyc__genarr_694),( void*)(( struct
_tuple4**) Cyc__genarr_694),( void*)(( struct _tuple4**) Cyc__genarr_694 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_684; static struct Cyc_Typerep_Int_struct
Cyc__genrep_685={ 1u, 16}; struct _tuple21{ unsigned int f1; void* f2; short f3;
} ; static struct _tuple4 Cyc__gentuple_686={ offsetof( struct _tuple21,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_687={ offsetof(
struct _tuple21,f2),( void*)& Cyc_tunion_Absyn_Sign}; static struct _tuple4 Cyc__gentuple_688={
offsetof( struct _tuple21,f3),( void*)& Cyc__genrep_685}; static struct _tuple4*
Cyc__genarr_689[ 3u]={& Cyc__gentuple_686,& Cyc__gentuple_687,& Cyc__gentuple_688};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_684={ 4u, sizeof( struct
_tuple21),{( void*)(( struct _tuple4**) Cyc__genarr_689),( void*)(( struct
_tuple4**) Cyc__genarr_689),( void*)(( struct _tuple4**) Cyc__genarr_689 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_302; struct _tuple22{
unsigned int f1; void* f2; int f3; } ; static struct _tuple4 Cyc__gentuple_304={
offsetof( struct _tuple22,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_305={ offsetof( struct _tuple22,f2),( void*)& Cyc_tunion_Absyn_Sign};
static struct _tuple4 Cyc__gentuple_306={ offsetof( struct _tuple22,f3),( void*)((
void*)& Cyc__genrep_73)}; static struct _tuple4* Cyc__genarr_307[ 3u]={& Cyc__gentuple_304,&
Cyc__gentuple_305,& Cyc__gentuple_306}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_302={ 4u, sizeof( struct _tuple22),{( void*)(( struct _tuple4**) Cyc__genarr_307),(
void*)(( struct _tuple4**) Cyc__genarr_307),( void*)(( struct _tuple4**) Cyc__genarr_307
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_678; static struct
Cyc_Typerep_Int_struct Cyc__genrep_679={ 1u, 64}; struct _tuple23{ unsigned int
f1; void* f2; long long f3; } ; static struct _tuple4 Cyc__gentuple_680={
offsetof( struct _tuple23,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_681={ offsetof( struct _tuple23,f2),( void*)& Cyc_tunion_Absyn_Sign};
static struct _tuple4 Cyc__gentuple_682={ offsetof( struct _tuple23,f3),( void*)&
Cyc__genrep_679}; static struct _tuple4* Cyc__genarr_683[ 3u]={& Cyc__gentuple_680,&
Cyc__gentuple_681,& Cyc__gentuple_682}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_678={ 4u, sizeof( struct _tuple23),{( void*)(( struct _tuple4**) Cyc__genarr_683),(
void*)(( struct _tuple4**) Cyc__genarr_683),( void*)(( struct _tuple4**) Cyc__genarr_683
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_80; struct _tuple24{
unsigned int f1; struct _tagged_arr f2; } ; static struct _tuple4 Cyc__gentuple_81={
offsetof( struct _tuple24,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_82={ offsetof( struct _tuple24,f2),( void*)(( void*)& Cyc__genrep_12)};
static struct _tuple4* Cyc__genarr_83[ 2u]={& Cyc__gentuple_81,& Cyc__gentuple_82};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_80={ 4u, sizeof( struct
_tuple24),{( void*)(( struct _tuple4**) Cyc__genarr_83),( void*)(( struct
_tuple4**) Cyc__genarr_83),( void*)(( struct _tuple4**) Cyc__genarr_83 +  2u)}};
static struct _tuple4 Cyc__gentuple_695={ 0,( void*)& Cyc__genrep_690}; static
struct _tuple4 Cyc__gentuple_696={ 1,( void*)& Cyc__genrep_684}; static struct
_tuple4 Cyc__gentuple_697={ 2,( void*)& Cyc__genrep_302}; static struct _tuple4
Cyc__gentuple_698={ 3,( void*)& Cyc__genrep_678}; static struct _tuple4 Cyc__gentuple_699={
4,( void*)& Cyc__genrep_80}; static struct _tuple4 Cyc__gentuple_700={ 5,( void*)&
Cyc__genrep_80}; static struct _tuple4* Cyc__genarr_701[ 6u]={& Cyc__gentuple_695,&
Cyc__gentuple_696,& Cyc__gentuple_697,& Cyc__gentuple_698,& Cyc__gentuple_699,&
Cyc__gentuple_700}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Cnst={ 5u,{(
void*)(( struct _tuple4**) Cyc__genarr_701),( void*)(( struct _tuple4**) Cyc__genarr_701),(
void*)(( struct _tuple4**) Cyc__genarr_701 +  6u)}}; static struct _tuple4 Cyc__gentuple_702={
offsetof( struct _tuple4,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_703={
offsetof( struct _tuple4,f2),( void*)& Cyc_tunion_Absyn_Cnst}; static struct
_tuple4* Cyc__genarr_704[ 2u]={& Cyc__gentuple_702,& Cyc__gentuple_703}; static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_677={ 4u, sizeof( struct _tuple4),{(
void*)(( struct _tuple4**) Cyc__genarr_704),( void*)(( struct _tuple4**) Cyc__genarr_704),(
void*)(( struct _tuple4**) Cyc__genarr_704 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_666; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Binding;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_69; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_70; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Fndecl;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_473; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List060const_unsigned_char_344struct_Absyn_Tqual4tunion_Absyn_Type1_44099_6H2;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_474; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_475; struct _tuple25{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static struct _tuple4 Cyc__gentuple_476={ offsetof( struct
_tuple25,f1),( void*)& Cyc__genrep_11}; static struct _tuple4 Cyc__gentuple_477={
offsetof( struct _tuple25,f2),( void*)& Cyc__genrep_99}; static struct _tuple4
Cyc__gentuple_478={ offsetof( struct _tuple25,f3),( void*)& Cyc_tunion_Absyn_Type};
static struct _tuple4* Cyc__genarr_479[ 3u]={& Cyc__gentuple_476,& Cyc__gentuple_477,&
Cyc__gentuple_478}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_475={ 4u,
sizeof( struct _tuple25),{( void*)(( struct _tuple4**) Cyc__genarr_479),( void*)((
struct _tuple4**) Cyc__genarr_479),( void*)(( struct _tuple4**) Cyc__genarr_479
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_474={ 2u, 1,(
void*)(( void*)& Cyc__genrep_475)}; static struct _tuple4 Cyc__gentuple_480={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_474}; static struct
_tuple4 Cyc__gentuple_481={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_473};
static struct _tuple4* Cyc__genarr_482[ 2u]={& Cyc__gentuple_480,& Cyc__gentuple_481};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060const_unsigned_char_344struct_Absyn_Tqual4tunion_Absyn_Type1_44099_6H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_482),(
void*)(( struct _tuple4**) Cyc__genarr_482),( void*)(( struct _tuple4**) Cyc__genarr_482
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_473={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060const_unsigned_char_344struct_Absyn_Tqual4tunion_Absyn_Type1_44099_6H2)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_464; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_VarargInfo; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_465;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0const_unsigned_char_342;
static struct _tuple4 Cyc__gentuple_466={ offsetof( struct Cyc_Core_Opt,v),(
void*)& Cyc__genrep_11}; static struct _tuple4* Cyc__genarr_467[ 1u]={& Cyc__gentuple_466};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0const_unsigned_char_342={ 4u,
sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_467),(
void*)(( struct _tuple4**) Cyc__genarr_467),( void*)(( struct _tuple4**) Cyc__genarr_467
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_465={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0const_unsigned_char_342)}; static struct
_tuple4 Cyc__gentuple_468={ offsetof( struct Cyc_Absyn_VarargInfo,name),( void*)&
Cyc__genrep_465}; static struct _tuple4 Cyc__gentuple_469={ offsetof( struct Cyc_Absyn_VarargInfo,tq),(
void*)& Cyc__genrep_99}; static struct _tuple4 Cyc__gentuple_470={ offsetof(
struct Cyc_Absyn_VarargInfo,type),( void*)& Cyc_tunion_Absyn_Type}; static
struct _tuple4 Cyc__gentuple_471={ offsetof( struct Cyc_Absyn_VarargInfo,inject),(
void*)(( void*)& Cyc__genrep_73)}; static struct _tuple4* Cyc__genarr_472[ 4u]={&
Cyc__gentuple_468,& Cyc__gentuple_469,& Cyc__gentuple_470,& Cyc__gentuple_471};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_VarargInfo={ 4u, sizeof( struct
Cyc_Absyn_VarargInfo),{( void*)(( struct _tuple4**) Cyc__genarr_472),( void*)((
struct _tuple4**) Cyc__genarr_472),( void*)(( struct _tuple4**) Cyc__genarr_472
+  4u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_464={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_VarargInfo)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_455; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060tunion_Absyn_Type4tunion_Absyn_Type1_446H2;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_456; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_457; static struct _tuple4 Cyc__gentuple_458={ offsetof( struct
_tuple6,f1),( void*)& Cyc_tunion_Absyn_Type}; static struct _tuple4 Cyc__gentuple_459={
offsetof( struct _tuple6,f2),( void*)& Cyc_tunion_Absyn_Type}; static struct
_tuple4* Cyc__genarr_460[ 2u]={& Cyc__gentuple_458,& Cyc__gentuple_459}; static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_457={ 4u, sizeof( struct _tuple6),{(
void*)(( struct _tuple4**) Cyc__genarr_460),( void*)(( struct _tuple4**) Cyc__genarr_460),(
void*)(( struct _tuple4**) Cyc__genarr_460 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_456={ 2u, 1,( void*)(( void*)& Cyc__genrep_457)}; static struct
_tuple4 Cyc__gentuple_461={ offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_456};
static struct _tuple4 Cyc__gentuple_462={ offsetof( struct Cyc_List_List,tl),(
void*)& Cyc__genrep_455}; static struct _tuple4* Cyc__genarr_463[ 2u]={& Cyc__gentuple_461,&
Cyc__gentuple_462}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060tunion_Absyn_Type4tunion_Absyn_Type1_446H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_463),(
void*)(( struct _tuple4**) Cyc__genarr_463),( void*)(( struct _tuple4**) Cyc__genarr_463
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_455={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060tunion_Absyn_Type4tunion_Absyn_Type1_446H2)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_116; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Stmt; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Raw_stmt;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_424; struct _tuple26{
unsigned int f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3; } ;
static struct _tuple4 Cyc__gentuple_425={ offsetof( struct _tuple26,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_426={ offsetof( struct
_tuple26,f2),( void*)& Cyc__genrep_116}; static struct _tuple4 Cyc__gentuple_427={
offsetof( struct _tuple26,f3),( void*)& Cyc__genrep_116}; static struct _tuple4*
Cyc__genarr_428[ 3u]={& Cyc__gentuple_425,& Cyc__gentuple_426,& Cyc__gentuple_427};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_424={ 4u, sizeof( struct
_tuple26),{( void*)(( struct _tuple4**) Cyc__genarr_428),( void*)(( struct
_tuple4**) Cyc__genarr_428),( void*)(( struct _tuple4**) Cyc__genarr_428 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_420; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_63; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_63={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Exp)}; struct _tuple27{ unsigned int f1;
struct Cyc_Absyn_Exp* f2; } ; static struct _tuple4 Cyc__gentuple_421={
offsetof( struct _tuple27,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_422={ offsetof( struct _tuple27,f2),( void*)& Cyc__genrep_63};
static struct _tuple4* Cyc__genarr_423[ 2u]={& Cyc__gentuple_421,& Cyc__gentuple_422};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_420={ 4u, sizeof( struct
_tuple27),{( void*)(( struct _tuple4**) Cyc__genarr_423),( void*)(( struct
_tuple4**) Cyc__genarr_423),( void*)(( struct _tuple4**) Cyc__genarr_423 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_414; struct _tuple28{
unsigned int f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Stmt* f3; struct Cyc_Absyn_Stmt*
f4; } ; static struct _tuple4 Cyc__gentuple_415={ offsetof( struct _tuple28,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_416={ offsetof(
struct _tuple28,f2),( void*)& Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_417={
offsetof( struct _tuple28,f3),( void*)& Cyc__genrep_116}; static struct _tuple4
Cyc__gentuple_418={ offsetof( struct _tuple28,f4),( void*)& Cyc__genrep_116};
static struct _tuple4* Cyc__genarr_419[ 4u]={& Cyc__gentuple_415,& Cyc__gentuple_416,&
Cyc__gentuple_417,& Cyc__gentuple_418}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_414={ 4u, sizeof( struct _tuple28),{( void*)(( struct _tuple4**) Cyc__genarr_419),(
void*)(( struct _tuple4**) Cyc__genarr_419),( void*)(( struct _tuple4**) Cyc__genarr_419
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_409; extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_341; static struct _tuple4 Cyc__gentuple_342={
offsetof( struct _tuple2,f1),( void*)& Cyc__genrep_65}; static struct _tuple4
Cyc__gentuple_343={ offsetof( struct _tuple2,f2),( void*)& Cyc__genrep_116};
static struct _tuple4* Cyc__genarr_344[ 2u]={& Cyc__gentuple_342,& Cyc__gentuple_343};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_341={ 4u, sizeof( struct
_tuple2),{( void*)(( struct _tuple4**) Cyc__genarr_344),( void*)(( struct
_tuple4**) Cyc__genarr_344),( void*)(( struct _tuple4**) Cyc__genarr_344 +  2u)}};
struct _tuple29{ unsigned int f1; struct _tuple2 f2; struct Cyc_Absyn_Stmt* f3;
} ; static struct _tuple4 Cyc__gentuple_410={ offsetof( struct _tuple29,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_411={ offsetof(
struct _tuple29,f2),( void*)& Cyc__genrep_341}; static struct _tuple4 Cyc__gentuple_412={
offsetof( struct _tuple29,f3),( void*)& Cyc__genrep_116}; static struct _tuple4*
Cyc__genarr_413[ 3u]={& Cyc__gentuple_410,& Cyc__gentuple_411,& Cyc__gentuple_412};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_409={ 4u, sizeof( struct
_tuple29),{( void*)(( struct _tuple4**) Cyc__genarr_413),( void*)(( struct
_tuple4**) Cyc__genarr_413),( void*)(( struct _tuple4**) Cyc__genarr_413 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_405; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_400; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_400={ 2u,
1,( void*)(( void*)& Cyc_struct_Absyn_Stmt)}; struct _tuple30{ unsigned int f1;
struct Cyc_Absyn_Stmt* f2; } ; static struct _tuple4 Cyc__gentuple_406={
offsetof( struct _tuple30,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_407={ offsetof( struct _tuple30,f2),( void*)& Cyc__genrep_400};
static struct _tuple4* Cyc__genarr_408[ 2u]={& Cyc__gentuple_406,& Cyc__gentuple_407};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_405={ 4u, sizeof( struct
_tuple30),{( void*)(( struct _tuple4**) Cyc__genarr_408),( void*)(( struct
_tuple4**) Cyc__genarr_408),( void*)(( struct _tuple4**) Cyc__genarr_408 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_399; struct _tuple31{
unsigned int f1; struct _tagged_arr* f2; struct Cyc_Absyn_Stmt* f3; } ; static
struct _tuple4 Cyc__gentuple_401={ offsetof( struct _tuple31,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_402={ offsetof( struct _tuple31,f2),( void*)&
Cyc__genrep_11}; static struct _tuple4 Cyc__gentuple_403={ offsetof( struct
_tuple31,f3),( void*)& Cyc__genrep_400}; static struct _tuple4* Cyc__genarr_404[
3u]={& Cyc__gentuple_401,& Cyc__gentuple_402,& Cyc__gentuple_403}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_399={ 4u, sizeof( struct _tuple31),{( void*)((
struct _tuple4**) Cyc__genarr_404),( void*)(( struct _tuple4**) Cyc__genarr_404),(
void*)(( struct _tuple4**) Cyc__genarr_404 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_392; struct _tuple32{ unsigned int f1; struct Cyc_Absyn_Exp* f2;
struct _tuple2 f3; struct _tuple2 f4; struct Cyc_Absyn_Stmt* f5; } ; static
struct _tuple4 Cyc__gentuple_393={ offsetof( struct _tuple32,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_394={ offsetof( struct _tuple32,f2),( void*)&
Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_395={ offsetof( struct
_tuple32,f3),( void*)& Cyc__genrep_341}; static struct _tuple4 Cyc__gentuple_396={
offsetof( struct _tuple32,f4),( void*)& Cyc__genrep_341}; static struct _tuple4
Cyc__gentuple_397={ offsetof( struct _tuple32,f5),( void*)& Cyc__genrep_116};
static struct _tuple4* Cyc__genarr_398[ 5u]={& Cyc__gentuple_393,& Cyc__gentuple_394,&
Cyc__gentuple_395,& Cyc__gentuple_396,& Cyc__gentuple_397}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_392={ 4u, sizeof( struct _tuple32),{( void*)(( struct _tuple4**) Cyc__genarr_398),(
void*)(( struct _tuple4**) Cyc__genarr_398),( void*)(( struct _tuple4**) Cyc__genarr_398
+  5u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_387; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_150; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Switch_clause_446H2; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_151; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Switch_clause;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_152; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Pat; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Raw_pat;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_298; struct _tuple33{
unsigned int f1; unsigned char f2; } ; static struct _tuple4 Cyc__gentuple_299={
offsetof( struct _tuple33,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_300={ offsetof( struct _tuple33,f2),( void*)(( void*)& Cyc__genrep_13)};
static struct _tuple4* Cyc__genarr_301[ 2u]={& Cyc__gentuple_299,& Cyc__gentuple_300};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_298={ 4u, sizeof( struct
_tuple33),{( void*)(( struct _tuple4**) Cyc__genarr_301),( void*)(( struct
_tuple4**) Cyc__genarr_301),( void*)(( struct _tuple4**) Cyc__genarr_301 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_294; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_184; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0struct_Absyn_Pat_446H2;
static struct _tuple4 Cyc__gentuple_185={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_152}; static struct _tuple4 Cyc__gentuple_186={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_184}; static struct _tuple4* Cyc__genarr_187[
2u]={& Cyc__gentuple_185,& Cyc__gentuple_186}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Pat_446H2={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_187),( void*)(( struct _tuple4**) Cyc__genarr_187),(
void*)(( struct _tuple4**) Cyc__genarr_187 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_184={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0struct_Absyn_Pat_446H2)};
static struct _tuple4 Cyc__gentuple_295={ offsetof( struct _tuple8,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_296={ offsetof( struct
_tuple8,f2),( void*)& Cyc__genrep_184}; static struct _tuple4* Cyc__genarr_297[
2u]={& Cyc__gentuple_295,& Cyc__gentuple_296}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_294={ 4u, sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_297),(
void*)(( struct _tuple4**) Cyc__genarr_297),( void*)(( struct _tuple4**) Cyc__genarr_297
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_290; struct
_tuple34{ unsigned int f1; struct Cyc_Absyn_Pat* f2; } ; static struct _tuple4
Cyc__gentuple_291={ offsetof( struct _tuple34,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_292={ offsetof( struct _tuple34,f2),( void*)&
Cyc__genrep_152}; static struct _tuple4* Cyc__genarr_293[ 2u]={& Cyc__gentuple_291,&
Cyc__gentuple_292}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_290={ 4u,
sizeof( struct _tuple34),{( void*)(( struct _tuple4**) Cyc__genarr_293),( void*)((
struct _tuple4**) Cyc__genarr_293),( void*)(( struct _tuple4**) Cyc__genarr_293
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_252; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_256; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Structdecl; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_271;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt060tunion_Absyn_Nmspace4const_unsigned_char_341_42;
static struct _tuple4 Cyc__gentuple_272={ offsetof( struct Cyc_Core_Opt,v),(
void*)& Cyc__genrep_9}; static struct _tuple4* Cyc__genarr_273[ 1u]={& Cyc__gentuple_272};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt060tunion_Absyn_Nmspace4const_unsigned_char_341_42={
4u, sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_273),(
void*)(( struct _tuple4**) Cyc__genarr_273),( void*)(( struct _tuple4**) Cyc__genarr_273
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_271={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt060tunion_Absyn_Nmspace4const_unsigned_char_341_42)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_257; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Structfield_446H2_22; extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_258; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Structfield_446H2; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_259; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Structfield;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_71; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0tunion_Absyn_Attribute46H2; extern struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_Attribute; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_72;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type; static
struct _tuple4* Cyc__genarr_74[ 0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_74),( void*)(( struct _tuple4**)
Cyc__genarr_74),( void*)(( struct _tuple4**) Cyc__genarr_74 +  0u)}}; struct
_tuple35{ unsigned int f1; void* f2; int f3; int f4; } ; static struct _tuple4
Cyc__gentuple_75={ offsetof( struct _tuple35,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_76={ offsetof( struct _tuple35,f2),( void*)&
Cyc_tunion_Absyn_Format_Type}; static struct _tuple4 Cyc__gentuple_77={
offsetof( struct _tuple35,f3),( void*)(( void*)& Cyc__genrep_73)}; static struct
_tuple4 Cyc__gentuple_78={ offsetof( struct _tuple35,f4),( void*)(( void*)& Cyc__genrep_73)};
static struct _tuple4* Cyc__genarr_79[ 4u]={& Cyc__gentuple_75,& Cyc__gentuple_76,&
Cyc__gentuple_77,& Cyc__gentuple_78}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_72={
4u, sizeof( struct _tuple35),{( void*)(( struct _tuple4**) Cyc__genarr_79),(
void*)(( struct _tuple4**) Cyc__genarr_79),( void*)(( struct _tuple4**) Cyc__genarr_79
+  4u)}}; static struct _tuple4 Cyc__gentuple_88={ 0,( void*)& Cyc__genrep_84};
static struct _tuple4 Cyc__gentuple_89={ 1,( void*)& Cyc__genrep_84}; static
struct _tuple4 Cyc__gentuple_90={ 2,( void*)& Cyc__genrep_80}; static struct
_tuple4 Cyc__gentuple_91={ 3,( void*)& Cyc__genrep_72}; static struct _tuple4*
Cyc__genarr_92[ 4u]={& Cyc__gentuple_88,& Cyc__gentuple_89,& Cyc__gentuple_90,&
Cyc__gentuple_91}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Attribute={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_92),( void*)(( struct _tuple4**)
Cyc__genarr_92),( void*)(( struct _tuple4**) Cyc__genarr_92 +  4u)}}; static
struct _tuple4 Cyc__gentuple_93={ offsetof( struct Cyc_List_List,hd),( void*)&
Cyc_tunion_Absyn_Attribute}; static struct _tuple4 Cyc__gentuple_94={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_71}; static struct _tuple4* Cyc__genarr_95[
2u]={& Cyc__gentuple_93,& Cyc__gentuple_94}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0tunion_Absyn_Attribute46H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_95),(
void*)(( struct _tuple4**) Cyc__genarr_95),( void*)(( struct _tuple4**) Cyc__genarr_95
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_71={ 2u, 1,( void*)((
void*)& Cyc_struct_List_List0tunion_Absyn_Attribute46H2)}; static struct _tuple4
Cyc__gentuple_260={ offsetof( struct Cyc_Absyn_Structfield,name),( void*)& Cyc__genrep_11};
static struct _tuple4 Cyc__gentuple_261={ offsetof( struct Cyc_Absyn_Structfield,tq),(
void*)& Cyc__genrep_99}; static struct _tuple4 Cyc__gentuple_262={ offsetof(
struct Cyc_Absyn_Structfield,type),( void*)& Cyc_tunion_Absyn_Type}; static
struct _tuple4 Cyc__gentuple_263={ offsetof( struct Cyc_Absyn_Structfield,width),(
void*)& Cyc__genrep_63}; static struct _tuple4 Cyc__gentuple_264={ offsetof(
struct Cyc_Absyn_Structfield,attributes),( void*)& Cyc__genrep_71}; static
struct _tuple4* Cyc__genarr_265[ 5u]={& Cyc__gentuple_260,& Cyc__gentuple_261,&
Cyc__gentuple_262,& Cyc__gentuple_263,& Cyc__gentuple_264}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Structfield={ 4u, sizeof( struct Cyc_Absyn_Structfield),{( void*)((
struct _tuple4**) Cyc__genarr_265),( void*)(( struct _tuple4**) Cyc__genarr_265),(
void*)(( struct _tuple4**) Cyc__genarr_265 +  5u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_259={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Structfield)};
static struct _tuple4 Cyc__gentuple_266={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_259}; static struct _tuple4 Cyc__gentuple_267={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_258}; static struct _tuple4* Cyc__genarr_268[
2u]={& Cyc__gentuple_266,& Cyc__gentuple_267}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Structfield_446H2={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_268),( void*)(( struct _tuple4**) Cyc__genarr_268),(
void*)(( struct _tuple4**) Cyc__genarr_268 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_258={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0struct_Absyn_Structfield_446H2)};
static struct _tuple4 Cyc__gentuple_269={ offsetof( struct Cyc_Core_Opt,v),(
void*)& Cyc__genrep_258}; static struct _tuple4* Cyc__genarr_270[ 1u]={& Cyc__gentuple_269};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Structfield_446H2_22={
4u, sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_270),(
void*)(( struct _tuple4**) Cyc__genarr_270),( void*)(( struct _tuple4**) Cyc__genarr_270
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_257={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Structfield_446H2_22)};
static struct _tuple4 Cyc__gentuple_274={ offsetof( struct Cyc_Absyn_Structdecl,sc),(
void*)& Cyc_tunion_Absyn_Scope}; static struct _tuple4 Cyc__gentuple_275={
offsetof( struct Cyc_Absyn_Structdecl,name),( void*)& Cyc__genrep_271}; static
struct _tuple4 Cyc__gentuple_276={ offsetof( struct Cyc_Absyn_Structdecl,tvs),(
void*)& Cyc__genrep_174}; static struct _tuple4 Cyc__gentuple_277={ offsetof(
struct Cyc_Absyn_Structdecl,fields),( void*)& Cyc__genrep_257}; static struct
_tuple4 Cyc__gentuple_278={ offsetof( struct Cyc_Absyn_Structdecl,attributes),(
void*)& Cyc__genrep_71}; static struct _tuple4* Cyc__genarr_279[ 5u]={& Cyc__gentuple_274,&
Cyc__gentuple_275,& Cyc__gentuple_276,& Cyc__gentuple_277,& Cyc__gentuple_278};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Structdecl={ 4u, sizeof( struct
Cyc_Absyn_Structdecl),{( void*)(( struct _tuple4**) Cyc__genarr_279),( void*)((
struct _tuple4**) Cyc__genarr_279),( void*)(( struct _tuple4**) Cyc__genarr_279
+  5u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_256={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Structdecl)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_253; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0struct_List_List0tunion_Absyn_Type46H2_22;
static struct _tuple4 Cyc__gentuple_254={ offsetof( struct Cyc_Core_Opt,v),(
void*)& Cyc__genrep_44}; static struct _tuple4* Cyc__genarr_255[ 1u]={& Cyc__gentuple_254};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0struct_List_List0tunion_Absyn_Type46H2_22={
4u, sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_255),(
void*)(( struct _tuple4**) Cyc__genarr_255),( void*)(( struct _tuple4**) Cyc__genarr_255
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_253={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0struct_List_List0tunion_Absyn_Type46H2_22)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_154; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List060struct_List_List0tunion_Absyn_Designator46H2_24struct_Absyn_Pat_41_44099_6H2;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_155; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_156; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_157;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0tunion_Absyn_Designator46H2;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Designator; extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_158; struct _tuple36{ unsigned int
f1; struct _tagged_arr* f2; } ; static struct _tuple4 Cyc__gentuple_159={
offsetof( struct _tuple36,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_160={ offsetof( struct _tuple36,f2),( void*)& Cyc__genrep_11};
static struct _tuple4* Cyc__genarr_161[ 2u]={& Cyc__gentuple_159,& Cyc__gentuple_160};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_158={ 4u, sizeof( struct
_tuple36),{( void*)(( struct _tuple4**) Cyc__genarr_161),( void*)(( struct
_tuple4**) Cyc__genarr_161),( void*)(( struct _tuple4**) Cyc__genarr_161 +  2u)}};
static struct _tuple4 Cyc__gentuple_162={ 0,( void*)& Cyc__genrep_64}; static
struct _tuple4 Cyc__gentuple_163={ 1,( void*)& Cyc__genrep_158}; static struct
_tuple4* Cyc__genarr_164[ 2u]={& Cyc__gentuple_162,& Cyc__gentuple_163}; struct
Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Designator={ 5u,{( void*)(( struct
_tuple4**) Cyc__genarr_164),( void*)(( struct _tuple4**) Cyc__genarr_164),( void*)((
struct _tuple4**) Cyc__genarr_164 +  2u)}}; static struct _tuple4 Cyc__gentuple_165={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc_tunion_Absyn_Designator};
static struct _tuple4 Cyc__gentuple_166={ offsetof( struct Cyc_List_List,tl),(
void*)& Cyc__genrep_157}; static struct _tuple4* Cyc__genarr_167[ 2u]={& Cyc__gentuple_165,&
Cyc__gentuple_166}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0tunion_Absyn_Designator46H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_167),(
void*)(( struct _tuple4**) Cyc__genarr_167),( void*)(( struct _tuple4**) Cyc__genarr_167
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_157={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List0tunion_Absyn_Designator46H2)}; struct
_tuple37{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static struct
_tuple4 Cyc__gentuple_168={ offsetof( struct _tuple37,f1),( void*)& Cyc__genrep_157};
static struct _tuple4 Cyc__gentuple_169={ offsetof( struct _tuple37,f2),( void*)&
Cyc__genrep_152}; static struct _tuple4* Cyc__genarr_170[ 2u]={& Cyc__gentuple_168,&
Cyc__gentuple_169}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_156={ 4u,
sizeof( struct _tuple37),{( void*)(( struct _tuple4**) Cyc__genarr_170),( void*)((
struct _tuple4**) Cyc__genarr_170),( void*)(( struct _tuple4**) Cyc__genarr_170
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_155={ 2u, 1,(
void*)(( void*)& Cyc__genrep_156)}; static struct _tuple4 Cyc__gentuple_171={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_155}; static struct
_tuple4 Cyc__gentuple_172={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_154};
static struct _tuple4* Cyc__genarr_173[ 2u]={& Cyc__gentuple_171,& Cyc__gentuple_172};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060struct_List_List0tunion_Absyn_Designator46H2_24struct_Absyn_Pat_41_44099_6H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_173),(
void*)(( struct _tuple4**) Cyc__genarr_173),( void*)(( struct _tuple4**) Cyc__genarr_173
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_154={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060struct_List_List0tunion_Absyn_Designator46H2_24struct_Absyn_Pat_41_44099_6H2)};
struct _tuple38{ unsigned int f1; struct Cyc_Absyn_Structdecl* f2; struct Cyc_Core_Opt*
f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; } ; static struct
_tuple4 Cyc__gentuple_280={ offsetof( struct _tuple38,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_281={ offsetof( struct _tuple38,f2),( void*)((
void*)& Cyc__genrep_256)}; static struct _tuple4 Cyc__gentuple_282={ offsetof(
struct _tuple38,f3),( void*)& Cyc__genrep_253}; static struct _tuple4 Cyc__gentuple_283={
offsetof( struct _tuple38,f4),( void*)& Cyc__genrep_174}; static struct _tuple4
Cyc__gentuple_284={ offsetof( struct _tuple38,f5),( void*)& Cyc__genrep_154};
static struct _tuple4* Cyc__genarr_285[ 5u]={& Cyc__gentuple_280,& Cyc__gentuple_281,&
Cyc__gentuple_282,& Cyc__gentuple_283,& Cyc__gentuple_284}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_252={ 4u, sizeof( struct _tuple38),{( void*)(( struct _tuple4**) Cyc__genarr_285),(
void*)(( struct _tuple4**) Cyc__genarr_285),( void*)(( struct _tuple4**) Cyc__genarr_285
+  5u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_215; struct
_tuple39{ unsigned int f1; struct Cyc_Absyn_Tuniondecl* f2; struct Cyc_Absyn_Tunionfield*
f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; } ; static struct
_tuple4 Cyc__gentuple_246={ offsetof( struct _tuple39,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_247={ offsetof( struct _tuple39,f2),( void*)((
void*)& Cyc__genrep_232)}; static struct _tuple4 Cyc__gentuple_248={ offsetof(
struct _tuple39,f3),( void*)& Cyc__genrep_216}; static struct _tuple4 Cyc__gentuple_249={
offsetof( struct _tuple39,f4),( void*)& Cyc__genrep_174}; static struct _tuple4
Cyc__gentuple_250={ offsetof( struct _tuple39,f5),( void*)& Cyc__genrep_184};
static struct _tuple4* Cyc__genarr_251[ 5u]={& Cyc__gentuple_246,& Cyc__gentuple_247,&
Cyc__gentuple_248,& Cyc__gentuple_249,& Cyc__gentuple_250}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_215={ 4u, sizeof( struct _tuple39),{( void*)(( struct _tuple4**) Cyc__genarr_251),(
void*)(( struct _tuple4**) Cyc__genarr_251),( void*)(( struct _tuple4**) Cyc__genarr_251
+  5u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_202; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_203; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Enumdecl; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_204;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Enumfield_446H2_22;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Enumfield_446H2; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_62; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Enumfield;
static struct _tuple4 Cyc__gentuple_748={ offsetof( struct Cyc_Absyn_Enumfield,name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_749={ offsetof(
struct Cyc_Absyn_Enumfield,tag),( void*)& Cyc__genrep_63}; static struct _tuple4
Cyc__gentuple_750={ offsetof( struct Cyc_Absyn_Enumfield,loc),( void*)& Cyc__genrep_2};
static struct _tuple4* Cyc__genarr_751[ 3u]={& Cyc__gentuple_748,& Cyc__gentuple_749,&
Cyc__gentuple_750}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Enumfield={
4u, sizeof( struct Cyc_Absyn_Enumfield),{( void*)(( struct _tuple4**) Cyc__genarr_751),(
void*)(( struct _tuple4**) Cyc__genarr_751),( void*)(( struct _tuple4**) Cyc__genarr_751
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62={ 2u, 1,( void*)((
void*)& Cyc_struct_Absyn_Enumfield)}; static struct _tuple4 Cyc__gentuple_752={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_62}; static struct
_tuple4 Cyc__gentuple_753={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_61};
static struct _tuple4* Cyc__genarr_754[ 2u]={& Cyc__gentuple_752,& Cyc__gentuple_753};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0struct_Absyn_Enumfield_446H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_754),(
void*)(( struct _tuple4**) Cyc__genarr_754),( void*)(( struct _tuple4**) Cyc__genarr_754
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={ 2u, 1,( void*)((
void*)& Cyc_struct_List_List0struct_Absyn_Enumfield_446H2)}; static struct
_tuple4 Cyc__gentuple_205={ offsetof( struct Cyc_Core_Opt,v),( void*)& Cyc__genrep_61};
static struct _tuple4* Cyc__genarr_206[ 1u]={& Cyc__gentuple_205}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Enumfield_446H2_22={ 4u,
sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_206),(
void*)(( struct _tuple4**) Cyc__genarr_206),( void*)(( struct _tuple4**) Cyc__genarr_206
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_204={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Enumfield_446H2_22)};
static struct _tuple4 Cyc__gentuple_207={ offsetof( struct Cyc_Absyn_Enumdecl,sc),(
void*)& Cyc_tunion_Absyn_Scope}; static struct _tuple4 Cyc__gentuple_208={
offsetof( struct Cyc_Absyn_Enumdecl,name),( void*)& Cyc__genrep_9}; static
struct _tuple4 Cyc__gentuple_209={ offsetof( struct Cyc_Absyn_Enumdecl,fields),(
void*)& Cyc__genrep_204}; static struct _tuple4* Cyc__genarr_210[ 3u]={& Cyc__gentuple_207,&
Cyc__gentuple_208,& Cyc__gentuple_209}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Enumdecl={
4u, sizeof( struct Cyc_Absyn_Enumdecl),{( void*)(( struct _tuple4**) Cyc__genarr_210),(
void*)(( struct _tuple4**) Cyc__genarr_210),( void*)(( struct _tuple4**) Cyc__genarr_210
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_203={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Enumdecl)}; struct _tuple40{ unsigned int f1;
struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield* f3; } ; static struct
_tuple4 Cyc__gentuple_211={ offsetof( struct _tuple40,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_212={ offsetof( struct _tuple40,f2),( void*)&
Cyc__genrep_203}; static struct _tuple4 Cyc__gentuple_213={ offsetof( struct
_tuple40,f3),( void*)& Cyc__genrep_62}; static struct _tuple4* Cyc__genarr_214[
3u]={& Cyc__gentuple_211,& Cyc__gentuple_212,& Cyc__gentuple_213}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_202={ 4u, sizeof( struct _tuple40),{( void*)((
struct _tuple4**) Cyc__genarr_214),( void*)(( struct _tuple4**) Cyc__genarr_214),(
void*)(( struct _tuple4**) Cyc__genarr_214 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_197; struct _tuple41{ unsigned int f1; void* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static struct _tuple4 Cyc__gentuple_198={ offsetof( struct _tuple41,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_199={ offsetof(
struct _tuple41,f2),( void*)& Cyc_tunion_Absyn_Type}; static struct _tuple4 Cyc__gentuple_200={
offsetof( struct _tuple41,f3),( void*)& Cyc__genrep_62}; static struct _tuple4*
Cyc__genarr_201[ 3u]={& Cyc__gentuple_198,& Cyc__gentuple_199,& Cyc__gentuple_200};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_197={ 4u, sizeof( struct
_tuple41),{( void*)(( struct _tuple4**) Cyc__genarr_201),( void*)(( struct
_tuple4**) Cyc__genarr_201),( void*)(( struct _tuple4**) Cyc__genarr_201 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_193; struct _tuple42{
unsigned int f1; struct _tuple0* f2; } ; static struct _tuple4 Cyc__gentuple_194={
offsetof( struct _tuple42,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_195={ offsetof( struct _tuple42,f2),( void*)& Cyc__genrep_9};
static struct _tuple4* Cyc__genarr_196[ 2u]={& Cyc__gentuple_194,& Cyc__gentuple_195};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_193={ 4u, sizeof( struct
_tuple42),{( void*)(( struct _tuple4**) Cyc__genarr_196),( void*)(( struct
_tuple4**) Cyc__genarr_196),( void*)(( struct _tuple4**) Cyc__genarr_196 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_183; struct _tuple43{
unsigned int f1; struct _tuple0* f2; struct Cyc_List_List* f3; struct Cyc_List_List*
f4; } ; static struct _tuple4 Cyc__gentuple_188={ offsetof( struct _tuple43,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_189={ offsetof(
struct _tuple43,f2),( void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_190={
offsetof( struct _tuple43,f3),( void*)& Cyc__genrep_174}; static struct _tuple4
Cyc__gentuple_191={ offsetof( struct _tuple43,f4),( void*)& Cyc__genrep_184};
static struct _tuple4* Cyc__genarr_192[ 4u]={& Cyc__gentuple_188,& Cyc__gentuple_189,&
Cyc__gentuple_190,& Cyc__gentuple_191}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_183={ 4u, sizeof( struct _tuple43),{( void*)(( struct _tuple4**) Cyc__genarr_192),(
void*)(( struct _tuple4**) Cyc__genarr_192),( void*)(( struct _tuple4**) Cyc__genarr_192
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_153; static struct
_tuple4 Cyc__gentuple_178={ offsetof( struct _tuple43,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_179={ offsetof( struct _tuple43,f2),( void*)&
Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_180={ offsetof( struct
_tuple43,f3),( void*)& Cyc__genrep_174}; static struct _tuple4 Cyc__gentuple_181={
offsetof( struct _tuple43,f4),( void*)& Cyc__genrep_154}; static struct _tuple4*
Cyc__genarr_182[ 4u]={& Cyc__gentuple_178,& Cyc__gentuple_179,& Cyc__gentuple_180,&
Cyc__gentuple_181}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_153={ 4u,
sizeof( struct _tuple43),{( void*)(( struct _tuple4**) Cyc__genarr_182),( void*)((
struct _tuple4**) Cyc__genarr_182),( void*)(( struct _tuple4**) Cyc__genarr_182
+  4u)}}; static struct _tuple4 Cyc__gentuple_308={ 0,( void*)& Cyc__genrep_286};
static struct _tuple4 Cyc__gentuple_309={ 1,( void*)& Cyc__genrep_302}; static
struct _tuple4 Cyc__gentuple_310={ 2,( void*)& Cyc__genrep_298}; static struct
_tuple4 Cyc__gentuple_311={ 3,( void*)& Cyc__genrep_80}; static struct _tuple4
Cyc__gentuple_312={ 4,( void*)& Cyc__genrep_294}; static struct _tuple4 Cyc__gentuple_313={
5,( void*)& Cyc__genrep_290}; static struct _tuple4 Cyc__gentuple_314={ 6,( void*)&
Cyc__genrep_286}; static struct _tuple4 Cyc__gentuple_315={ 7,( void*)& Cyc__genrep_252};
static struct _tuple4 Cyc__gentuple_316={ 8,( void*)& Cyc__genrep_215}; static
struct _tuple4 Cyc__gentuple_317={ 9,( void*)& Cyc__genrep_202}; static struct
_tuple4 Cyc__gentuple_318={ 10,( void*)& Cyc__genrep_197}; static struct _tuple4
Cyc__gentuple_319={ 11,( void*)& Cyc__genrep_193}; static struct _tuple4 Cyc__gentuple_320={
12,( void*)& Cyc__genrep_183}; static struct _tuple4 Cyc__gentuple_321={ 13,(
void*)& Cyc__genrep_153}; static struct _tuple4* Cyc__genarr_322[ 14u]={& Cyc__gentuple_308,&
Cyc__gentuple_309,& Cyc__gentuple_310,& Cyc__gentuple_311,& Cyc__gentuple_312,&
Cyc__gentuple_313,& Cyc__gentuple_314,& Cyc__gentuple_315,& Cyc__gentuple_316,&
Cyc__gentuple_317,& Cyc__gentuple_318,& Cyc__gentuple_319,& Cyc__gentuple_320,&
Cyc__gentuple_321}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Raw_pat={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_322),( void*)(( struct _tuple4**)
Cyc__genarr_322),( void*)(( struct _tuple4**) Cyc__genarr_322 +  14u)}}; static
struct _tuple4 Cyc__gentuple_323={ offsetof( struct Cyc_Absyn_Pat,r),( void*)&
Cyc_tunion_Absyn_Raw_pat}; static struct _tuple4 Cyc__gentuple_324={ offsetof(
struct Cyc_Absyn_Pat,topt),( void*)& Cyc__genrep_52}; static struct _tuple4 Cyc__gentuple_325={
offsetof( struct Cyc_Absyn_Pat,loc),( void*)& Cyc__genrep_2}; static struct
_tuple4* Cyc__genarr_326[ 3u]={& Cyc__gentuple_323,& Cyc__gentuple_324,& Cyc__gentuple_325};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Pat={ 4u, sizeof( struct Cyc_Absyn_Pat),{(
void*)(( struct _tuple4**) Cyc__genarr_326),( void*)(( struct _tuple4**) Cyc__genarr_326),(
void*)(( struct _tuple4**) Cyc__genarr_326 +  3u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_152={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Pat)}; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_96; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Vardecl_446H2_22; extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_97; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Vardecl_446H2; static struct _tuple4 Cyc__gentuple_111={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_98}; static struct
_tuple4 Cyc__gentuple_112={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_97};
static struct _tuple4* Cyc__genarr_113[ 2u]={& Cyc__gentuple_111,& Cyc__gentuple_112};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0struct_Absyn_Vardecl_446H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_113),(
void*)(( struct _tuple4**) Cyc__genarr_113),( void*)(( struct _tuple4**) Cyc__genarr_113
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_97={ 2u, 1,( void*)((
void*)& Cyc_struct_List_List0struct_Absyn_Vardecl_446H2)}; static struct _tuple4
Cyc__gentuple_114={ offsetof( struct Cyc_Core_Opt,v),( void*)& Cyc__genrep_97};
static struct _tuple4* Cyc__genarr_115[ 1u]={& Cyc__gentuple_114}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Vardecl_446H2_22={ 4u, sizeof(
struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_115),( void*)((
struct _tuple4**) Cyc__genarr_115),( void*)(( struct _tuple4**) Cyc__genarr_115
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_96={ 2u, 1,( void*)((
void*)& Cyc_struct_Core_Opt0struct_List_List0struct_Absyn_Vardecl_446H2_22)};
static struct _tuple4 Cyc__gentuple_327={ offsetof( struct Cyc_Absyn_Switch_clause,pattern),(
void*)& Cyc__genrep_152}; static struct _tuple4 Cyc__gentuple_328={ offsetof(
struct Cyc_Absyn_Switch_clause,pat_vars),( void*)& Cyc__genrep_96}; static
struct _tuple4 Cyc__gentuple_329={ offsetof( struct Cyc_Absyn_Switch_clause,where_clause),(
void*)& Cyc__genrep_63}; static struct _tuple4 Cyc__gentuple_330={ offsetof(
struct Cyc_Absyn_Switch_clause,body),( void*)& Cyc__genrep_116}; static struct
_tuple4 Cyc__gentuple_331={ offsetof( struct Cyc_Absyn_Switch_clause,loc),( void*)&
Cyc__genrep_2}; static struct _tuple4* Cyc__genarr_332[ 5u]={& Cyc__gentuple_327,&
Cyc__gentuple_328,& Cyc__gentuple_329,& Cyc__gentuple_330,& Cyc__gentuple_331};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Switch_clause={ 4u, sizeof(
struct Cyc_Absyn_Switch_clause),{( void*)(( struct _tuple4**) Cyc__genarr_332),(
void*)(( struct _tuple4**) Cyc__genarr_332),( void*)(( struct _tuple4**) Cyc__genarr_332
+  5u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_151={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Switch_clause)}; static struct _tuple4 Cyc__gentuple_333={
offsetof( struct Cyc_List_List,hd),( void*)(( void*)& Cyc__genrep_151)}; static
struct _tuple4 Cyc__gentuple_334={ offsetof( struct Cyc_List_List,tl),( void*)&
Cyc__genrep_150}; static struct _tuple4* Cyc__genarr_335[ 2u]={& Cyc__gentuple_333,&
Cyc__gentuple_334}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0struct_Absyn_Switch_clause_446H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_335),(
void*)(( struct _tuple4**) Cyc__genarr_335),( void*)(( struct _tuple4**) Cyc__genarr_335
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_150={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List0struct_Absyn_Switch_clause_446H2)}; struct
_tuple44{ unsigned int f1; struct Cyc_Absyn_Exp* f2; struct Cyc_List_List* f3; }
; static struct _tuple4 Cyc__gentuple_388={ offsetof( struct _tuple44,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_389={ offsetof( struct
_tuple44,f2),( void*)& Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_390={
offsetof( struct _tuple44,f3),( void*)& Cyc__genrep_150}; static struct _tuple4*
Cyc__genarr_391[ 3u]={& Cyc__gentuple_388,& Cyc__gentuple_389,& Cyc__gentuple_390};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_387={ 4u, sizeof( struct
_tuple44),{( void*)(( struct _tuple4**) Cyc__genarr_391),( void*)(( struct
_tuple4**) Cyc__genarr_391),( void*)(( struct _tuple4**) Cyc__genarr_391 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_373; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_374; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0struct_Absyn_SwitchC_clause_446H2;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_375; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_SwitchC_clause; static struct _tuple4 Cyc__gentuple_376={
offsetof( struct Cyc_Absyn_SwitchC_clause,cnst_exp),( void*)& Cyc__genrep_63};
static struct _tuple4 Cyc__gentuple_377={ offsetof( struct Cyc_Absyn_SwitchC_clause,body),(
void*)& Cyc__genrep_116}; static struct _tuple4 Cyc__gentuple_378={ offsetof(
struct Cyc_Absyn_SwitchC_clause,loc),( void*)& Cyc__genrep_2}; static struct
_tuple4* Cyc__genarr_379[ 3u]={& Cyc__gentuple_376,& Cyc__gentuple_377,& Cyc__gentuple_378};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_SwitchC_clause={ 4u, sizeof(
struct Cyc_Absyn_SwitchC_clause),{( void*)(( struct _tuple4**) Cyc__genarr_379),(
void*)(( struct _tuple4**) Cyc__genarr_379),( void*)(( struct _tuple4**) Cyc__genarr_379
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_375={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_SwitchC_clause)}; static struct _tuple4 Cyc__gentuple_380={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_375}; static struct
_tuple4 Cyc__gentuple_381={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_374};
static struct _tuple4* Cyc__genarr_382[ 2u]={& Cyc__gentuple_380,& Cyc__gentuple_381};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0struct_Absyn_SwitchC_clause_446H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_382),(
void*)(( struct _tuple4**) Cyc__genarr_382),( void*)(( struct _tuple4**) Cyc__genarr_382
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_374={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List0struct_Absyn_SwitchC_clause_446H2)};
static struct _tuple4 Cyc__gentuple_383={ offsetof( struct _tuple44,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_384={ offsetof( struct
_tuple44,f2),( void*)& Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_385={
offsetof( struct _tuple44,f3),( void*)& Cyc__genrep_374}; static struct _tuple4*
Cyc__genarr_386[ 3u]={& Cyc__gentuple_383,& Cyc__gentuple_384,& Cyc__gentuple_385};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_373={ 4u, sizeof( struct
_tuple44),{( void*)(( struct _tuple4**) Cyc__genarr_386),( void*)(( struct
_tuple4**) Cyc__genarr_386),( void*)(( struct _tuple4**) Cyc__genarr_386 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_363; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_365; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0struct_Absyn_Exp_446H2;
static struct _tuple4 Cyc__gentuple_366={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_367={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_365}; static struct _tuple4* Cyc__genarr_368[
2u]={& Cyc__gentuple_366,& Cyc__gentuple_367}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Exp_446H2={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_368),( void*)(( struct _tuple4**) Cyc__genarr_368),(
void*)(( struct _tuple4**) Cyc__genarr_368 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_365={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0struct_Absyn_Exp_446H2)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_364; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_364={ 2u, 1,( void*)(( void*)& Cyc__genrep_151)}; struct _tuple45{
unsigned int f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Switch_clause** f3;
} ; static struct _tuple4 Cyc__gentuple_369={ offsetof( struct _tuple45,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_370={ offsetof(
struct _tuple45,f2),( void*)& Cyc__genrep_365}; static struct _tuple4 Cyc__gentuple_371={
offsetof( struct _tuple45,f3),( void*)& Cyc__genrep_364}; static struct _tuple4*
Cyc__genarr_372[ 3u]={& Cyc__gentuple_369,& Cyc__gentuple_370,& Cyc__gentuple_371};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_363={ 4u, sizeof( struct
_tuple45),{( void*)(( struct _tuple4**) Cyc__genarr_372),( void*)(( struct
_tuple4**) Cyc__genarr_372),( void*)(( struct _tuple4**) Cyc__genarr_372 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_358; struct _tuple46{
unsigned int f1; struct Cyc_Absyn_Decl* f2; struct Cyc_Absyn_Stmt* f3; } ;
static struct _tuple4 Cyc__gentuple_359={ offsetof( struct _tuple46,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_360={ offsetof( struct
_tuple46,f2),( void*)& Cyc__genrep_1}; static struct _tuple4 Cyc__gentuple_361={
offsetof( struct _tuple46,f3),( void*)& Cyc__genrep_116}; static struct _tuple4*
Cyc__genarr_362[ 3u]={& Cyc__gentuple_359,& Cyc__gentuple_360,& Cyc__gentuple_361};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_358={ 4u, sizeof( struct
_tuple46),{( void*)(( struct _tuple4**) Cyc__genarr_362),( void*)(( struct
_tuple4**) Cyc__genarr_362),( void*)(( struct _tuple4**) Cyc__genarr_362 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_354; static struct _tuple4
Cyc__gentuple_355={ offsetof( struct _tuple30,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_356={ offsetof( struct _tuple30,f2),( void*)&
Cyc__genrep_116}; static struct _tuple4* Cyc__genarr_357[ 2u]={& Cyc__gentuple_355,&
Cyc__gentuple_356}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_354={ 4u,
sizeof( struct _tuple30),{( void*)(( struct _tuple4**) Cyc__genarr_357),( void*)((
struct _tuple4**) Cyc__genarr_357),( void*)(( struct _tuple4**) Cyc__genarr_357
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_349; static struct
_tuple4 Cyc__gentuple_350={ offsetof( struct _tuple31,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_351={ offsetof( struct _tuple31,f2),( void*)&
Cyc__genrep_11}; static struct _tuple4 Cyc__gentuple_352={ offsetof( struct
_tuple31,f3),( void*)& Cyc__genrep_116}; static struct _tuple4* Cyc__genarr_353[
3u]={& Cyc__gentuple_350,& Cyc__gentuple_351,& Cyc__gentuple_352}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_349={ 4u, sizeof( struct _tuple31),{( void*)((
struct _tuple4**) Cyc__genarr_353),( void*)(( struct _tuple4**) Cyc__genarr_353),(
void*)(( struct _tuple4**) Cyc__genarr_353 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_340; struct _tuple47{ unsigned int f1; struct Cyc_Absyn_Stmt* f2;
struct _tuple2 f3; } ; static struct _tuple4 Cyc__gentuple_345={ offsetof(
struct _tuple47,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_346={
offsetof( struct _tuple47,f2),( void*)& Cyc__genrep_116}; static struct _tuple4
Cyc__gentuple_347={ offsetof( struct _tuple47,f3),( void*)& Cyc__genrep_341};
static struct _tuple4* Cyc__genarr_348[ 3u]={& Cyc__gentuple_345,& Cyc__gentuple_346,&
Cyc__gentuple_347}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_340={ 4u,
sizeof( struct _tuple47),{( void*)(( struct _tuple4**) Cyc__genarr_348),( void*)((
struct _tuple4**) Cyc__genarr_348),( void*)(( struct _tuple4**) Cyc__genarr_348
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_149; struct
_tuple48{ unsigned int f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_List_List* f3;
} ; static struct _tuple4 Cyc__gentuple_336={ offsetof( struct _tuple48,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_337={ offsetof(
struct _tuple48,f2),( void*)& Cyc__genrep_116}; static struct _tuple4 Cyc__gentuple_338={
offsetof( struct _tuple48,f3),( void*)& Cyc__genrep_150}; static struct _tuple4*
Cyc__genarr_339[ 3u]={& Cyc__gentuple_336,& Cyc__gentuple_337,& Cyc__gentuple_338};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_149={ 4u, sizeof( struct
_tuple48),{( void*)(( struct _tuple4**) Cyc__genarr_339),( void*)(( struct
_tuple4**) Cyc__genarr_339),( void*)(( struct _tuple4**) Cyc__genarr_339 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_122; struct _tuple49{
unsigned int f1; struct Cyc_Absyn_Tvar* f2; struct Cyc_Absyn_Vardecl* f3; struct
Cyc_Absyn_Stmt* f4; } ; static struct _tuple4 Cyc__gentuple_144={ offsetof(
struct _tuple49,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_145={
offsetof( struct _tuple49,f2),( void*)& Cyc__genrep_123}; static struct _tuple4
Cyc__gentuple_146={ offsetof( struct _tuple49,f3),( void*)& Cyc__genrep_98};
static struct _tuple4 Cyc__gentuple_147={ offsetof( struct _tuple49,f4),( void*)&
Cyc__genrep_116}; static struct _tuple4* Cyc__genarr_148[ 4u]={& Cyc__gentuple_144,&
Cyc__gentuple_145,& Cyc__gentuple_146,& Cyc__gentuple_147}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_122={ 4u, sizeof( struct _tuple49),{( void*)(( struct _tuple4**) Cyc__genarr_148),(
void*)(( struct _tuple4**) Cyc__genarr_148),( void*)(( struct _tuple4**) Cyc__genarr_148
+  4u)}}; static struct _tuple4 Cyc__gentuple_429={ 0,( void*)& Cyc__genrep_64};
static struct _tuple4 Cyc__gentuple_430={ 1,( void*)& Cyc__genrep_424}; static
struct _tuple4 Cyc__gentuple_431={ 2,( void*)& Cyc__genrep_420}; static struct
_tuple4 Cyc__gentuple_432={ 3,( void*)& Cyc__genrep_414}; static struct _tuple4
Cyc__gentuple_433={ 4,( void*)& Cyc__genrep_409}; static struct _tuple4 Cyc__gentuple_434={
5,( void*)& Cyc__genrep_405}; static struct _tuple4 Cyc__gentuple_435={ 6,( void*)&
Cyc__genrep_405}; static struct _tuple4 Cyc__gentuple_436={ 7,( void*)& Cyc__genrep_399};
static struct _tuple4 Cyc__gentuple_437={ 8,( void*)& Cyc__genrep_392}; static
struct _tuple4 Cyc__gentuple_438={ 9,( void*)& Cyc__genrep_387}; static struct
_tuple4 Cyc__gentuple_439={ 10,( void*)& Cyc__genrep_373}; static struct _tuple4
Cyc__gentuple_440={ 11,( void*)& Cyc__genrep_363}; static struct _tuple4 Cyc__gentuple_441={
12,( void*)& Cyc__genrep_358}; static struct _tuple4 Cyc__gentuple_442={ 13,(
void*)& Cyc__genrep_354}; static struct _tuple4 Cyc__gentuple_443={ 14,( void*)&
Cyc__genrep_354}; static struct _tuple4 Cyc__gentuple_444={ 15,( void*)& Cyc__genrep_349};
static struct _tuple4 Cyc__gentuple_445={ 16,( void*)& Cyc__genrep_340}; static
struct _tuple4 Cyc__gentuple_446={ 17,( void*)& Cyc__genrep_149}; static struct
_tuple4 Cyc__gentuple_447={ 18,( void*)& Cyc__genrep_122}; static struct _tuple4*
Cyc__genarr_448[ 19u]={& Cyc__gentuple_429,& Cyc__gentuple_430,& Cyc__gentuple_431,&
Cyc__gentuple_432,& Cyc__gentuple_433,& Cyc__gentuple_434,& Cyc__gentuple_435,&
Cyc__gentuple_436,& Cyc__gentuple_437,& Cyc__gentuple_438,& Cyc__gentuple_439,&
Cyc__gentuple_440,& Cyc__gentuple_441,& Cyc__gentuple_442,& Cyc__gentuple_443,&
Cyc__gentuple_444,& Cyc__gentuple_445,& Cyc__gentuple_446,& Cyc__gentuple_447};
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Raw_stmt={ 5u,{( void*)((
struct _tuple4**) Cyc__genarr_448),( void*)(( struct _tuple4**) Cyc__genarr_448),(
void*)(( struct _tuple4**) Cyc__genarr_448 +  19u)}}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_118; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0struct_Absyn_Stmt_446H2;
static struct _tuple4 Cyc__gentuple_119={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_116}; static struct _tuple4 Cyc__gentuple_120={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_118}; static struct _tuple4* Cyc__genarr_121[
2u]={& Cyc__gentuple_119,& Cyc__gentuple_120}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0struct_Absyn_Stmt_446H2={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_121),( void*)(( struct _tuple4**) Cyc__genarr_121),(
void*)(( struct _tuple4**) Cyc__genarr_121 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_118={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0struct_Absyn_Stmt_446H2)};
extern struct Cyc_Typerep_XTUnion_struct Cyc_xtunion_Absyn_StmtAnnot; static
struct _tuple5* Cyc__genarr_117[ 0u]={}; struct Cyc_Typerep_XTUnion_struct Cyc_xtunion_Absyn_StmtAnnot={
6u,{( void*)(( struct _tuple5**) Cyc__genarr_117),( void*)(( struct _tuple5**)
Cyc__genarr_117),( void*)(( struct _tuple5**) Cyc__genarr_117 +  0u)}}; static
struct _tuple4 Cyc__gentuple_449={ offsetof( struct Cyc_Absyn_Stmt,r),( void*)&
Cyc_tunion_Absyn_Raw_stmt}; static struct _tuple4 Cyc__gentuple_450={ offsetof(
struct Cyc_Absyn_Stmt,loc),( void*)& Cyc__genrep_2}; static struct _tuple4 Cyc__gentuple_451={
offsetof( struct Cyc_Absyn_Stmt,non_local_preds),( void*)& Cyc__genrep_118};
static struct _tuple4 Cyc__gentuple_452={ offsetof( struct Cyc_Absyn_Stmt,try_depth),(
void*)(( void*)& Cyc__genrep_73)}; static struct _tuple4 Cyc__gentuple_453={
offsetof( struct Cyc_Absyn_Stmt,annot),( void*)& Cyc_xtunion_Absyn_StmtAnnot};
static struct _tuple4* Cyc__genarr_454[ 5u]={& Cyc__gentuple_449,& Cyc__gentuple_450,&
Cyc__gentuple_451,& Cyc__gentuple_452,& Cyc__gentuple_453}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Stmt={ 4u, sizeof( struct Cyc_Absyn_Stmt),{( void*)(( struct
_tuple4**) Cyc__genarr_454),( void*)(( struct _tuple4**) Cyc__genarr_454),( void*)((
struct _tuple4**) Cyc__genarr_454 +  5u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_116={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Stmt)}; static
struct _tuple4 Cyc__gentuple_483={ offsetof( struct Cyc_Absyn_Fndecl,sc),( void*)&
Cyc_tunion_Absyn_Scope}; static struct _tuple4 Cyc__gentuple_484={ offsetof(
struct Cyc_Absyn_Fndecl,is_inline),( void*)(( void*)& Cyc__genrep_73)}; static
struct _tuple4 Cyc__gentuple_485={ offsetof( struct Cyc_Absyn_Fndecl,name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_486={ offsetof(
struct Cyc_Absyn_Fndecl,tvs),( void*)& Cyc__genrep_174}; static struct _tuple4
Cyc__gentuple_487={ offsetof( struct Cyc_Absyn_Fndecl,effect),( void*)& Cyc__genrep_52};
static struct _tuple4 Cyc__gentuple_488={ offsetof( struct Cyc_Absyn_Fndecl,ret_type),(
void*)& Cyc_tunion_Absyn_Type}; static struct _tuple4 Cyc__gentuple_489={
offsetof( struct Cyc_Absyn_Fndecl,args),( void*)& Cyc__genrep_473}; static
struct _tuple4 Cyc__gentuple_490={ offsetof( struct Cyc_Absyn_Fndecl,c_varargs),(
void*)(( void*)& Cyc__genrep_73)}; static struct _tuple4 Cyc__gentuple_491={
offsetof( struct Cyc_Absyn_Fndecl,cyc_varargs),( void*)& Cyc__genrep_464};
static struct _tuple4 Cyc__gentuple_492={ offsetof( struct Cyc_Absyn_Fndecl,rgn_po),(
void*)& Cyc__genrep_455}; static struct _tuple4 Cyc__gentuple_493={ offsetof(
struct Cyc_Absyn_Fndecl,body),( void*)& Cyc__genrep_116}; static struct _tuple4
Cyc__gentuple_494={ offsetof( struct Cyc_Absyn_Fndecl,cached_typ),( void*)& Cyc__genrep_52};
static struct _tuple4 Cyc__gentuple_495={ offsetof( struct Cyc_Absyn_Fndecl,param_vardecls),(
void*)& Cyc__genrep_96}; static struct _tuple4 Cyc__gentuple_496={ offsetof(
struct Cyc_Absyn_Fndecl,attributes),( void*)& Cyc__genrep_71}; static struct
_tuple4* Cyc__genarr_497[ 14u]={& Cyc__gentuple_483,& Cyc__gentuple_484,& Cyc__gentuple_485,&
Cyc__gentuple_486,& Cyc__gentuple_487,& Cyc__gentuple_488,& Cyc__gentuple_489,&
Cyc__gentuple_490,& Cyc__gentuple_491,& Cyc__gentuple_492,& Cyc__gentuple_493,&
Cyc__gentuple_494,& Cyc__gentuple_495,& Cyc__gentuple_496}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Fndecl={ 4u, sizeof( struct Cyc_Absyn_Fndecl),{( void*)((
struct _tuple4**) Cyc__genarr_497),( void*)(( struct _tuple4**) Cyc__genarr_497),(
void*)(( struct _tuple4**) Cyc__genarr_497 +  14u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_70={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Fndecl)}; struct
_tuple50{ unsigned int f1; struct Cyc_Absyn_Fndecl* f2; } ; static struct
_tuple4 Cyc__gentuple_498={ offsetof( struct _tuple50,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_499={ offsetof( struct _tuple50,f2),( void*)&
Cyc__genrep_70}; static struct _tuple4* Cyc__genarr_500[ 2u]={& Cyc__gentuple_498,&
Cyc__gentuple_499}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_69={ 4u,
sizeof( struct _tuple50),{( void*)(( struct _tuple4**) Cyc__genarr_500),( void*)((
struct _tuple4**) Cyc__genarr_500),( void*)(( struct _tuple4**) Cyc__genarr_500
+  2u)}}; static struct _tuple4 Cyc__gentuple_667={ 0,( void*)& Cyc__genrep_286};
static struct _tuple4 Cyc__gentuple_668={ 1,( void*)& Cyc__genrep_69}; static
struct _tuple4 Cyc__gentuple_669={ 2,( void*)& Cyc__genrep_286}; static struct
_tuple4 Cyc__gentuple_670={ 3,( void*)& Cyc__genrep_286}; static struct _tuple4
Cyc__gentuple_671={ 4,( void*)& Cyc__genrep_286}; static struct _tuple4* Cyc__genarr_672[
5u]={& Cyc__gentuple_667,& Cyc__gentuple_668,& Cyc__gentuple_669,& Cyc__gentuple_670,&
Cyc__gentuple_671}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Binding={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_672),( void*)(( struct _tuple4**)
Cyc__genarr_672),( void*)(( struct _tuple4**) Cyc__genarr_672 +  5u)}}; struct
_tuple51{ unsigned int f1; struct _tuple0* f2; void* f3; } ; static struct
_tuple4 Cyc__gentuple_673={ offsetof( struct _tuple51,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_674={ offsetof( struct _tuple51,f2),( void*)&
Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_675={ offsetof( struct
_tuple51,f3),( void*)& Cyc_tunion_Absyn_Binding}; static struct _tuple4* Cyc__genarr_676[
3u]={& Cyc__gentuple_673,& Cyc__gentuple_674,& Cyc__gentuple_675}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_666={ 4u, sizeof( struct _tuple51),{( void*)((
struct _tuple4**) Cyc__genarr_676),( void*)(( struct _tuple4**) Cyc__genarr_676),(
void*)(( struct _tuple4**) Cyc__genarr_676 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_661; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Primop;
static struct _tuple4* Cyc__genarr_653[ 0u]={}; struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_Primop={ 5u,{( void*)(( struct _tuple4**) Cyc__genarr_653),(
void*)(( struct _tuple4**) Cyc__genarr_653),( void*)(( struct _tuple4**) Cyc__genarr_653
+  0u)}}; struct _tuple52{ unsigned int f1; void* f2; struct Cyc_List_List* f3;
} ; static struct _tuple4 Cyc__gentuple_662={ offsetof( struct _tuple52,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_663={ offsetof(
struct _tuple52,f2),( void*)& Cyc_tunion_Absyn_Primop}; static struct _tuple4
Cyc__gentuple_664={ offsetof( struct _tuple52,f3),( void*)& Cyc__genrep_365};
static struct _tuple4* Cyc__genarr_665[ 3u]={& Cyc__gentuple_662,& Cyc__gentuple_663,&
Cyc__gentuple_664}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_661={ 4u,
sizeof( struct _tuple52),{( void*)(( struct _tuple4**) Cyc__genarr_665),( void*)((
struct _tuple4**) Cyc__genarr_665),( void*)(( struct _tuple4**) Cyc__genarr_665
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_651; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_652; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0tunion_Absyn_Primop2; static struct _tuple4 Cyc__gentuple_654={
offsetof( struct Cyc_Core_Opt,v),( void*)& Cyc_tunion_Absyn_Primop}; static
struct _tuple4* Cyc__genarr_655[ 1u]={& Cyc__gentuple_654}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0tunion_Absyn_Primop2={ 4u, sizeof( struct Cyc_Core_Opt),{(
void*)(( struct _tuple4**) Cyc__genarr_655),( void*)(( struct _tuple4**) Cyc__genarr_655),(
void*)(( struct _tuple4**) Cyc__genarr_655 +  1u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_652={ 2u, 1,( void*)(( void*)& Cyc_struct_Core_Opt0tunion_Absyn_Primop2)};
struct _tuple53{ unsigned int f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Core_Opt*
f3; struct Cyc_Absyn_Exp* f4; } ; static struct _tuple4 Cyc__gentuple_656={
offsetof( struct _tuple53,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_657={ offsetof( struct _tuple53,f2),( void*)& Cyc__genrep_65};
static struct _tuple4 Cyc__gentuple_658={ offsetof( struct _tuple53,f3),( void*)&
Cyc__genrep_652}; static struct _tuple4 Cyc__gentuple_659={ offsetof( struct
_tuple53,f4),( void*)& Cyc__genrep_65}; static struct _tuple4* Cyc__genarr_660[
4u]={& Cyc__gentuple_656,& Cyc__gentuple_657,& Cyc__gentuple_658,& Cyc__gentuple_659};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_651={ 4u, sizeof( struct
_tuple53),{( void*)(( struct _tuple4**) Cyc__genarr_660),( void*)(( struct
_tuple4**) Cyc__genarr_660),( void*)(( struct _tuple4**) Cyc__genarr_660 +  4u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_645; extern struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_Incrementor; static struct _tuple4* Cyc__genarr_646[ 0u]={};
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Incrementor={ 5u,{( void*)((
struct _tuple4**) Cyc__genarr_646),( void*)(( struct _tuple4**) Cyc__genarr_646),(
void*)(( struct _tuple4**) Cyc__genarr_646 +  0u)}}; struct _tuple54{
unsigned int f1; struct Cyc_Absyn_Exp* f2; void* f3; } ; static struct _tuple4
Cyc__gentuple_647={ offsetof( struct _tuple54,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_648={ offsetof( struct _tuple54,f2),( void*)&
Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_649={ offsetof( struct
_tuple54,f3),( void*)& Cyc_tunion_Absyn_Incrementor}; static struct _tuple4* Cyc__genarr_650[
3u]={& Cyc__gentuple_647,& Cyc__gentuple_648,& Cyc__gentuple_649}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_645={ 4u, sizeof( struct _tuple54),{( void*)((
struct _tuple4**) Cyc__genarr_650),( void*)(( struct _tuple4**) Cyc__genarr_650),(
void*)(( struct _tuple4**) Cyc__genarr_650 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_639; struct _tuple55{ unsigned int f1; struct Cyc_Absyn_Exp* f2;
struct Cyc_Absyn_Exp* f3; struct Cyc_Absyn_Exp* f4; } ; static struct _tuple4
Cyc__gentuple_640={ offsetof( struct _tuple55,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_641={ offsetof( struct _tuple55,f2),( void*)&
Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_642={ offsetof( struct
_tuple55,f3),( void*)& Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_643={
offsetof( struct _tuple55,f4),( void*)& Cyc__genrep_65}; static struct _tuple4*
Cyc__genarr_644[ 4u]={& Cyc__gentuple_640,& Cyc__gentuple_641,& Cyc__gentuple_642,&
Cyc__gentuple_643}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_639={ 4u,
sizeof( struct _tuple55),{( void*)(( struct _tuple4**) Cyc__genarr_644),( void*)((
struct _tuple4**) Cyc__genarr_644),( void*)(( struct _tuple4**) Cyc__genarr_644
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_580; struct
_tuple56{ unsigned int f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; }
; static struct _tuple4 Cyc__gentuple_581={ offsetof( struct _tuple56,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_582={ offsetof( struct
_tuple56,f2),( void*)& Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_583={
offsetof( struct _tuple56,f3),( void*)& Cyc__genrep_65}; static struct _tuple4*
Cyc__genarr_584[ 3u]={& Cyc__gentuple_581,& Cyc__gentuple_582,& Cyc__gentuple_583};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_580={ 4u, sizeof( struct
_tuple56),{( void*)(( struct _tuple4**) Cyc__genarr_584),( void*)(( struct
_tuple4**) Cyc__genarr_584),( void*)(( struct _tuple4**) Cyc__genarr_584 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_634; static struct _tuple4
Cyc__gentuple_635={ offsetof( struct _tuple44,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_636={ offsetof( struct _tuple44,f2),( void*)&
Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_637={ offsetof( struct
_tuple44,f3),( void*)& Cyc__genrep_365}; static struct _tuple4* Cyc__genarr_638[
3u]={& Cyc__gentuple_635,& Cyc__gentuple_636,& Cyc__gentuple_637}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_634={ 4u, sizeof( struct _tuple44),{( void*)((
struct _tuple4**) Cyc__genarr_638),( void*)(( struct _tuple4**) Cyc__genarr_638),(
void*)(( struct _tuple4**) Cyc__genarr_638 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_622; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_623;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_VarargCallInfo; extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_624; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_624={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_VarargInfo)}; static
struct _tuple4 Cyc__gentuple_625={ offsetof( struct Cyc_Absyn_VarargCallInfo,num_varargs),(
void*)(( void*)& Cyc__genrep_73)}; static struct _tuple4 Cyc__gentuple_626={
offsetof( struct Cyc_Absyn_VarargCallInfo,injectors),( void*)& Cyc__genrep_234};
static struct _tuple4 Cyc__gentuple_627={ offsetof( struct Cyc_Absyn_VarargCallInfo,vai),(
void*)& Cyc__genrep_624}; static struct _tuple4* Cyc__genarr_628[ 3u]={& Cyc__gentuple_625,&
Cyc__gentuple_626,& Cyc__gentuple_627}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_VarargCallInfo={
4u, sizeof( struct Cyc_Absyn_VarargCallInfo),{( void*)(( struct _tuple4**) Cyc__genarr_628),(
void*)(( struct _tuple4**) Cyc__genarr_628),( void*)(( struct _tuple4**) Cyc__genarr_628
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_623={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_VarargCallInfo)}; struct _tuple57{
unsigned int f1; struct Cyc_Absyn_Exp* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_VarargCallInfo*
f4; } ; static struct _tuple4 Cyc__gentuple_629={ offsetof( struct _tuple57,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_630={ offsetof(
struct _tuple57,f2),( void*)& Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_631={
offsetof( struct _tuple57,f3),( void*)& Cyc__genrep_365}; static struct _tuple4
Cyc__gentuple_632={ offsetof( struct _tuple57,f4),( void*)& Cyc__genrep_623};
static struct _tuple4* Cyc__genarr_633[ 4u]={& Cyc__gentuple_629,& Cyc__gentuple_630,&
Cyc__gentuple_631,& Cyc__gentuple_632}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_622={ 4u, sizeof( struct _tuple57),{( void*)(( struct _tuple4**) Cyc__genarr_633),(
void*)(( struct _tuple4**) Cyc__genarr_633),( void*)(( struct _tuple4**) Cyc__genarr_633
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_617; static struct
_tuple4 Cyc__gentuple_618={ offsetof( struct _tuple44,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_619={ offsetof( struct _tuple44,f2),( void*)&
Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_620={ offsetof( struct
_tuple44,f3),( void*)& Cyc__genrep_44}; static struct _tuple4* Cyc__genarr_621[
3u]={& Cyc__gentuple_618,& Cyc__gentuple_619,& Cyc__gentuple_620}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_617={ 4u, sizeof( struct _tuple44),{( void*)((
struct _tuple4**) Cyc__genarr_621),( void*)(( struct _tuple4**) Cyc__genarr_621),(
void*)(( struct _tuple4**) Cyc__genarr_621 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_612; struct _tuple58{ unsigned int f1; void* f2; struct Cyc_Absyn_Exp*
f3; } ; static struct _tuple4 Cyc__gentuple_613={ offsetof( struct _tuple58,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_614={ offsetof(
struct _tuple58,f2),( void*)& Cyc_tunion_Absyn_Type}; static struct _tuple4 Cyc__gentuple_615={
offsetof( struct _tuple58,f3),( void*)& Cyc__genrep_65}; static struct _tuple4*
Cyc__genarr_616[ 3u]={& Cyc__gentuple_613,& Cyc__gentuple_614,& Cyc__gentuple_615};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_612={ 4u, sizeof( struct
_tuple58),{( void*)(( struct _tuple4**) Cyc__genarr_616),( void*)(( struct
_tuple4**) Cyc__genarr_616),( void*)(( struct _tuple4**) Cyc__genarr_616 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_607; static struct _tuple4
Cyc__gentuple_608={ offsetof( struct _tuple56,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_609={ offsetof( struct _tuple56,f2),( void*)&
Cyc__genrep_63}; static struct _tuple4 Cyc__gentuple_610={ offsetof( struct
_tuple56,f3),( void*)& Cyc__genrep_65}; static struct _tuple4* Cyc__genarr_611[
3u]={& Cyc__gentuple_608,& Cyc__gentuple_609,& Cyc__gentuple_610}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_607={ 4u, sizeof( struct _tuple56),{( void*)((
struct _tuple4**) Cyc__genarr_611),( void*)(( struct _tuple4**) Cyc__genarr_611),(
void*)(( struct _tuple4**) Cyc__genarr_611 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_39; static struct _tuple4 Cyc__gentuple_40={ offsetof( struct
_tuple4,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_41={
offsetof( struct _tuple4,f2),( void*)& Cyc_tunion_Absyn_Type}; static struct
_tuple4* Cyc__genarr_42[ 2u]={& Cyc__gentuple_40,& Cyc__gentuple_41}; static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_39={ 4u, sizeof( struct _tuple4),{(
void*)(( struct _tuple4**) Cyc__genarr_42),( void*)(( struct _tuple4**) Cyc__genarr_42),(
void*)(( struct _tuple4**) Cyc__genarr_42 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_595; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_OffsetofField;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_596; struct _tuple59{
unsigned int f1; unsigned int f2; } ; static struct _tuple4 Cyc__gentuple_597={
offsetof( struct _tuple59,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_598={ offsetof( struct _tuple59,f2),( void*)& Cyc__genrep_4};
static struct _tuple4* Cyc__genarr_599[ 2u]={& Cyc__gentuple_597,& Cyc__gentuple_598};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_596={ 4u, sizeof( struct
_tuple59),{( void*)(( struct _tuple4**) Cyc__genarr_599),( void*)(( struct
_tuple4**) Cyc__genarr_599),( void*)(( struct _tuple4**) Cyc__genarr_599 +  2u)}};
static struct _tuple4 Cyc__gentuple_600={ 0,( void*)& Cyc__genrep_158}; static
struct _tuple4 Cyc__gentuple_601={ 1,( void*)& Cyc__genrep_596}; static struct
_tuple4* Cyc__genarr_602[ 2u]={& Cyc__gentuple_600,& Cyc__gentuple_601}; struct
Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_OffsetofField={ 5u,{( void*)(( struct
_tuple4**) Cyc__genarr_602),( void*)(( struct _tuple4**) Cyc__genarr_602),( void*)((
struct _tuple4**) Cyc__genarr_602 +  2u)}}; struct _tuple60{ unsigned int f1;
void* f2; void* f3; } ; static struct _tuple4 Cyc__gentuple_603={ offsetof(
struct _tuple60,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_604={
offsetof( struct _tuple60,f2),( void*)& Cyc_tunion_Absyn_Type}; static struct
_tuple4 Cyc__gentuple_605={ offsetof( struct _tuple60,f3),( void*)& Cyc_tunion_Absyn_OffsetofField};
static struct _tuple4* Cyc__genarr_606[ 3u]={& Cyc__gentuple_603,& Cyc__gentuple_604,&
Cyc__gentuple_605}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_595={ 4u,
sizeof( struct _tuple60),{( void*)(( struct _tuple4**) Cyc__genarr_606),( void*)((
struct _tuple4**) Cyc__genarr_606),( void*)(( struct _tuple4**) Cyc__genarr_606
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_590; struct
_tuple61{ unsigned int f1; struct Cyc_List_List* f2; void* f3; } ; static struct
_tuple4 Cyc__gentuple_591={ offsetof( struct _tuple61,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_592={ offsetof( struct _tuple61,f2),( void*)&
Cyc__genrep_174}; static struct _tuple4 Cyc__gentuple_593={ offsetof( struct
_tuple61,f3),( void*)& Cyc_tunion_Absyn_Type}; static struct _tuple4* Cyc__genarr_594[
3u]={& Cyc__gentuple_591,& Cyc__gentuple_592,& Cyc__gentuple_593}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_590={ 4u, sizeof( struct _tuple61),{( void*)((
struct _tuple4**) Cyc__genarr_594),( void*)(( struct _tuple4**) Cyc__genarr_594),(
void*)(( struct _tuple4**) Cyc__genarr_594 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_585; struct _tuple62{ unsigned int f1; struct Cyc_Absyn_Exp* f2;
struct _tagged_arr* f3; } ; static struct _tuple4 Cyc__gentuple_586={ offsetof(
struct _tuple62,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_587={
offsetof( struct _tuple62,f2),( void*)& Cyc__genrep_65}; static struct _tuple4
Cyc__gentuple_588={ offsetof( struct _tuple62,f3),( void*)& Cyc__genrep_11};
static struct _tuple4* Cyc__genarr_589[ 3u]={& Cyc__gentuple_586,& Cyc__gentuple_587,&
Cyc__gentuple_588}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_585={ 4u,
sizeof( struct _tuple62),{( void*)(( struct _tuple4**) Cyc__genarr_589),( void*)((
struct _tuple4**) Cyc__genarr_589),( void*)(( struct _tuple4**) Cyc__genarr_589
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_576; static struct
_tuple4 Cyc__gentuple_577={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_578={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_365}; static struct _tuple4* Cyc__genarr_579[ 2u]={& Cyc__gentuple_577,&
Cyc__gentuple_578}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_576={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_579),( void*)((
struct _tuple4**) Cyc__genarr_579),( void*)(( struct _tuple4**) Cyc__genarr_579
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_565; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_566; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_567; static struct _tuple4 Cyc__gentuple_568={ offsetof( struct
_tuple1,f1),( void*)& Cyc__genrep_465}; static struct _tuple4 Cyc__gentuple_569={
offsetof( struct _tuple1,f2),( void*)& Cyc__genrep_99}; static struct _tuple4
Cyc__gentuple_570={ offsetof( struct _tuple1,f3),( void*)& Cyc_tunion_Absyn_Type};
static struct _tuple4* Cyc__genarr_571[ 3u]={& Cyc__gentuple_568,& Cyc__gentuple_569,&
Cyc__gentuple_570}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_567={ 4u,
sizeof( struct _tuple1),{( void*)(( struct _tuple4**) Cyc__genarr_571),( void*)((
struct _tuple4**) Cyc__genarr_571),( void*)(( struct _tuple4**) Cyc__genarr_571
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_566={ 2u, 1,(
void*)(( void*)& Cyc__genrep_567)}; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_502;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060struct_List_List0tunion_Absyn_Designator46H2_24struct_Absyn_Exp_41_44099_6H2;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_503; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_504; static struct _tuple4 Cyc__gentuple_505={ offsetof( struct
_tuple7,f1),( void*)& Cyc__genrep_157}; static struct _tuple4 Cyc__gentuple_506={
offsetof( struct _tuple7,f2),( void*)& Cyc__genrep_65}; static struct _tuple4*
Cyc__genarr_507[ 2u]={& Cyc__gentuple_505,& Cyc__gentuple_506}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_504={ 4u, sizeof( struct _tuple7),{( void*)((
struct _tuple4**) Cyc__genarr_507),( void*)(( struct _tuple4**) Cyc__genarr_507),(
void*)(( struct _tuple4**) Cyc__genarr_507 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_503={ 2u, 1,( void*)(( void*)& Cyc__genrep_504)}; static struct
_tuple4 Cyc__gentuple_508={ offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_503};
static struct _tuple4 Cyc__gentuple_509={ offsetof( struct Cyc_List_List,tl),(
void*)& Cyc__genrep_502}; static struct _tuple4* Cyc__genarr_510[ 2u]={& Cyc__gentuple_508,&
Cyc__gentuple_509}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060struct_List_List0tunion_Absyn_Designator46H2_24struct_Absyn_Exp_41_44099_6H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_510),(
void*)(( struct _tuple4**) Cyc__genarr_510),( void*)(( struct _tuple4**) Cyc__genarr_510
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_502={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060struct_List_List0tunion_Absyn_Designator46H2_24struct_Absyn_Exp_41_44099_6H2)};
struct _tuple63{ unsigned int f1; struct _tuple1* f2; struct Cyc_List_List* f3;
} ; static struct _tuple4 Cyc__gentuple_572={ offsetof( struct _tuple63,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_573={ offsetof(
struct _tuple63,f2),( void*)& Cyc__genrep_566}; static struct _tuple4 Cyc__gentuple_574={
offsetof( struct _tuple63,f3),( void*)& Cyc__genrep_502}; static struct _tuple4*
Cyc__genarr_575[ 3u]={& Cyc__gentuple_572,& Cyc__gentuple_573,& Cyc__gentuple_574};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_565={ 4u, sizeof( struct
_tuple63),{( void*)(( struct _tuple4**) Cyc__genarr_575),( void*)(( struct
_tuple4**) Cyc__genarr_575),( void*)(( struct _tuple4**) Cyc__genarr_575 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_561; static struct _tuple4
Cyc__gentuple_562={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_563={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_502}; static struct _tuple4* Cyc__genarr_564[ 2u]={& Cyc__gentuple_562,&
Cyc__gentuple_563}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_561={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_564),( void*)((
struct _tuple4**) Cyc__genarr_564),( void*)(( struct _tuple4**) Cyc__genarr_564
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_555; struct
_tuple64{ unsigned int f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Exp*
f3; struct Cyc_Absyn_Exp* f4; } ; static struct _tuple4 Cyc__gentuple_556={
offsetof( struct _tuple64,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_557={ offsetof( struct _tuple64,f2),( void*)& Cyc__genrep_98};
static struct _tuple4 Cyc__gentuple_558={ offsetof( struct _tuple64,f3),( void*)&
Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_559={ offsetof( struct
_tuple64,f4),( void*)& Cyc__genrep_65}; static struct _tuple4* Cyc__genarr_560[
4u]={& Cyc__gentuple_556,& Cyc__gentuple_557,& Cyc__gentuple_558,& Cyc__gentuple_559};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_555={ 4u, sizeof( struct
_tuple64),{( void*)(( struct _tuple4**) Cyc__genarr_560),( void*)(( struct
_tuple4**) Cyc__genarr_560),( void*)(( struct _tuple4**) Cyc__genarr_560 +  4u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_547; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_548; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_548={ 2u,
1,( void*)(( void*)& Cyc_struct_Absyn_Structdecl)}; struct _tuple65{
unsigned int f1; struct _tuple0* f2; struct Cyc_Core_Opt* f3; struct Cyc_List_List*
f4; struct Cyc_Absyn_Structdecl* f5; } ; static struct _tuple4 Cyc__gentuple_549={
offsetof( struct _tuple65,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_550={ offsetof( struct _tuple65,f2),( void*)& Cyc__genrep_9};
static struct _tuple4 Cyc__gentuple_551={ offsetof( struct _tuple65,f3),( void*)&
Cyc__genrep_253}; static struct _tuple4 Cyc__gentuple_552={ offsetof( struct
_tuple65,f4),( void*)& Cyc__genrep_502}; static struct _tuple4 Cyc__gentuple_553={
offsetof( struct _tuple65,f5),( void*)& Cyc__genrep_548}; static struct _tuple4*
Cyc__genarr_554[ 5u]={& Cyc__gentuple_549,& Cyc__gentuple_550,& Cyc__gentuple_551,&
Cyc__gentuple_552,& Cyc__gentuple_553}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_547={ 4u, sizeof( struct _tuple65),{( void*)(( struct _tuple4**) Cyc__genarr_554),(
void*)(( struct _tuple4**) Cyc__genarr_554),( void*)(( struct _tuple4**) Cyc__genarr_554
+  5u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_542; static struct
_tuple4 Cyc__gentuple_543={ offsetof( struct _tuple52,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_544={ offsetof( struct _tuple52,f2),( void*)&
Cyc_tunion_Absyn_Type}; static struct _tuple4 Cyc__gentuple_545={ offsetof(
struct _tuple52,f3),( void*)& Cyc__genrep_502}; static struct _tuple4* Cyc__genarr_546[
3u]={& Cyc__gentuple_543,& Cyc__gentuple_544,& Cyc__gentuple_545}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_542={ 4u, sizeof( struct _tuple52),{( void*)((
struct _tuple4**) Cyc__genarr_546),( void*)(( struct _tuple4**) Cyc__genarr_546),(
void*)(( struct _tuple4**) Cyc__genarr_546 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_534; struct _tuple66{ unsigned int f1; struct Cyc_Core_Opt* f2;
struct Cyc_Core_Opt* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Tuniondecl*
f5; struct Cyc_Absyn_Tunionfield* f6; } ; static struct _tuple4 Cyc__gentuple_535={
offsetof( struct _tuple66,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_536={ offsetof( struct _tuple66,f2),( void*)& Cyc__genrep_253};
static struct _tuple4 Cyc__gentuple_537={ offsetof( struct _tuple66,f3),( void*)&
Cyc__genrep_253}; static struct _tuple4 Cyc__gentuple_538={ offsetof( struct
_tuple66,f4),( void*)& Cyc__genrep_365}; static struct _tuple4 Cyc__gentuple_539={
offsetof( struct _tuple66,f5),( void*)(( void*)& Cyc__genrep_232)}; static
struct _tuple4 Cyc__gentuple_540={ offsetof( struct _tuple66,f6),( void*)& Cyc__genrep_216};
static struct _tuple4* Cyc__genarr_541[ 6u]={& Cyc__gentuple_535,& Cyc__gentuple_536,&
Cyc__gentuple_537,& Cyc__gentuple_538,& Cyc__gentuple_539,& Cyc__gentuple_540};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_534={ 4u, sizeof( struct
_tuple66),{( void*)(( struct _tuple4**) Cyc__genarr_541),( void*)(( struct
_tuple4**) Cyc__genarr_541),( void*)(( struct _tuple4**) Cyc__genarr_541 +  6u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_527; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_528; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_528={ 2u,
1,( void*)(( void*)& Cyc_struct_Absyn_Enumdecl)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_521; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_521={ 2u,
1,( void*)(( void*)& Cyc_struct_Absyn_Enumfield)}; struct _tuple67{ unsigned int
f1; struct _tuple0* f2; struct Cyc_Absyn_Enumdecl* f3; struct Cyc_Absyn_Enumfield*
f4; } ; static struct _tuple4 Cyc__gentuple_529={ offsetof( struct _tuple67,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_530={ offsetof(
struct _tuple67,f2),( void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_531={
offsetof( struct _tuple67,f3),( void*)& Cyc__genrep_528}; static struct _tuple4
Cyc__gentuple_532={ offsetof( struct _tuple67,f4),( void*)& Cyc__genrep_521};
static struct _tuple4* Cyc__genarr_533[ 4u]={& Cyc__gentuple_529,& Cyc__gentuple_530,&
Cyc__gentuple_531,& Cyc__gentuple_532}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_527={ 4u, sizeof( struct _tuple67),{( void*)(( struct _tuple4**) Cyc__genarr_533),(
void*)(( struct _tuple4**) Cyc__genarr_533),( void*)(( struct _tuple4**) Cyc__genarr_533
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_520; struct
_tuple68{ unsigned int f1; struct _tuple0* f2; void* f3; struct Cyc_Absyn_Enumfield*
f4; } ; static struct _tuple4 Cyc__gentuple_522={ offsetof( struct _tuple68,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_523={ offsetof(
struct _tuple68,f2),( void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_524={
offsetof( struct _tuple68,f3),( void*)& Cyc_tunion_Absyn_Type}; static struct
_tuple4 Cyc__gentuple_525={ offsetof( struct _tuple68,f4),( void*)& Cyc__genrep_521};
static struct _tuple4* Cyc__genarr_526[ 4u]={& Cyc__gentuple_522,& Cyc__gentuple_523,&
Cyc__gentuple_524,& Cyc__gentuple_525}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_520={ 4u, sizeof( struct _tuple68),{( void*)(( struct _tuple4**) Cyc__genarr_526),(
void*)(( struct _tuple4**) Cyc__genarr_526),( void*)(( struct _tuple4**) Cyc__genarr_526
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_515; static struct
_tuple4 Cyc__gentuple_516={ offsetof( struct _tuple54,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_517={ offsetof( struct _tuple54,f2),( void*)&
Cyc__genrep_63}; static struct _tuple4 Cyc__gentuple_518={ offsetof( struct
_tuple54,f3),( void*)& Cyc_tunion_Absyn_Type}; static struct _tuple4* Cyc__genarr_519[
3u]={& Cyc__gentuple_516,& Cyc__gentuple_517,& Cyc__gentuple_518}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_515={ 4u, sizeof( struct _tuple54),{( void*)((
struct _tuple4**) Cyc__genarr_519),( void*)(( struct _tuple4**) Cyc__genarr_519),(
void*)(( struct _tuple4**) Cyc__genarr_519 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_501; struct _tuple69{ unsigned int f1; struct Cyc_Core_Opt* f2;
struct Cyc_List_List* f3; } ; static struct _tuple4 Cyc__gentuple_511={
offsetof( struct _tuple69,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_512={ offsetof( struct _tuple69,f2),( void*)& Cyc__genrep_271};
static struct _tuple4 Cyc__gentuple_513={ offsetof( struct _tuple69,f3),( void*)&
Cyc__genrep_502}; static struct _tuple4* Cyc__genarr_514[ 3u]={& Cyc__gentuple_511,&
Cyc__gentuple_512,& Cyc__gentuple_513}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_501={ 4u, sizeof( struct _tuple69),{( void*)(( struct _tuple4**) Cyc__genarr_514),(
void*)(( struct _tuple4**) Cyc__genarr_514),( void*)(( struct _tuple4**) Cyc__genarr_514
+  3u)}}; static struct _tuple4 Cyc__gentuple_705={ 0,( void*)& Cyc__genrep_677};
static struct _tuple4 Cyc__gentuple_706={ 1,( void*)& Cyc__genrep_666}; static
struct _tuple4 Cyc__gentuple_707={ 2,( void*)& Cyc__genrep_193}; static struct
_tuple4 Cyc__gentuple_708={ 3,( void*)& Cyc__genrep_661}; static struct _tuple4
Cyc__gentuple_709={ 4,( void*)& Cyc__genrep_651}; static struct _tuple4 Cyc__gentuple_710={
5,( void*)& Cyc__genrep_645}; static struct _tuple4 Cyc__gentuple_711={ 6,( void*)&
Cyc__genrep_639}; static struct _tuple4 Cyc__gentuple_712={ 7,( void*)& Cyc__genrep_580};
static struct _tuple4 Cyc__gentuple_713={ 8,( void*)& Cyc__genrep_634}; static
struct _tuple4 Cyc__gentuple_714={ 9,( void*)& Cyc__genrep_622}; static struct
_tuple4 Cyc__gentuple_715={ 10,( void*)& Cyc__genrep_64}; static struct _tuple4
Cyc__gentuple_716={ 11,( void*)& Cyc__genrep_64}; static struct _tuple4 Cyc__gentuple_717={
12,( void*)& Cyc__genrep_617}; static struct _tuple4 Cyc__gentuple_718={ 13,(
void*)& Cyc__genrep_612}; static struct _tuple4 Cyc__gentuple_719={ 14,( void*)&
Cyc__genrep_64}; static struct _tuple4 Cyc__gentuple_720={ 15,( void*)& Cyc__genrep_607};
static struct _tuple4 Cyc__gentuple_721={ 16,( void*)& Cyc__genrep_39}; static
struct _tuple4 Cyc__gentuple_722={ 17,( void*)& Cyc__genrep_64}; static struct
_tuple4 Cyc__gentuple_723={ 18,( void*)& Cyc__genrep_595}; static struct _tuple4
Cyc__gentuple_724={ 19,( void*)& Cyc__genrep_590}; static struct _tuple4 Cyc__gentuple_725={
20,( void*)& Cyc__genrep_64}; static struct _tuple4 Cyc__gentuple_726={ 21,(
void*)& Cyc__genrep_585}; static struct _tuple4 Cyc__gentuple_727={ 22,( void*)&
Cyc__genrep_585}; static struct _tuple4 Cyc__gentuple_728={ 23,( void*)& Cyc__genrep_580};
static struct _tuple4 Cyc__gentuple_729={ 24,( void*)& Cyc__genrep_576}; static
struct _tuple4 Cyc__gentuple_730={ 25,( void*)& Cyc__genrep_565}; static struct
_tuple4 Cyc__gentuple_731={ 26,( void*)& Cyc__genrep_561}; static struct _tuple4
Cyc__gentuple_732={ 27,( void*)& Cyc__genrep_555}; static struct _tuple4 Cyc__gentuple_733={
28,( void*)& Cyc__genrep_547}; static struct _tuple4 Cyc__gentuple_734={ 29,(
void*)& Cyc__genrep_542}; static struct _tuple4 Cyc__gentuple_735={ 30,( void*)&
Cyc__genrep_534}; static struct _tuple4 Cyc__gentuple_736={ 31,( void*)& Cyc__genrep_527};
static struct _tuple4 Cyc__gentuple_737={ 32,( void*)& Cyc__genrep_520}; static
struct _tuple4 Cyc__gentuple_738={ 33,( void*)& Cyc__genrep_515}; static struct
_tuple4 Cyc__gentuple_739={ 34,( void*)& Cyc__genrep_501}; static struct _tuple4
Cyc__gentuple_740={ 35,( void*)& Cyc__genrep_354}; static struct _tuple4 Cyc__gentuple_741={
36,( void*)& Cyc__genrep_69}; static struct _tuple4 Cyc__gentuple_742={ 37,(
void*)& Cyc__genrep_64}; static struct _tuple4* Cyc__genarr_743[ 38u]={& Cyc__gentuple_705,&
Cyc__gentuple_706,& Cyc__gentuple_707,& Cyc__gentuple_708,& Cyc__gentuple_709,&
Cyc__gentuple_710,& Cyc__gentuple_711,& Cyc__gentuple_712,& Cyc__gentuple_713,&
Cyc__gentuple_714,& Cyc__gentuple_715,& Cyc__gentuple_716,& Cyc__gentuple_717,&
Cyc__gentuple_718,& Cyc__gentuple_719,& Cyc__gentuple_720,& Cyc__gentuple_721,&
Cyc__gentuple_722,& Cyc__gentuple_723,& Cyc__gentuple_724,& Cyc__gentuple_725,&
Cyc__gentuple_726,& Cyc__gentuple_727,& Cyc__gentuple_728,& Cyc__gentuple_729,&
Cyc__gentuple_730,& Cyc__gentuple_731,& Cyc__gentuple_732,& Cyc__gentuple_733,&
Cyc__gentuple_734,& Cyc__gentuple_735,& Cyc__gentuple_736,& Cyc__gentuple_737,&
Cyc__gentuple_738,& Cyc__gentuple_739,& Cyc__gentuple_740,& Cyc__gentuple_741,&
Cyc__gentuple_742}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Raw_exp={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_743),( void*)(( struct _tuple4**)
Cyc__genarr_743),( void*)(( struct _tuple4**) Cyc__genarr_743 +  38u)}}; static
struct _tuple4 Cyc__gentuple_744={ offsetof( struct Cyc_Absyn_Exp,topt),( void*)&
Cyc__genrep_52}; static struct _tuple4 Cyc__gentuple_745={ offsetof( struct Cyc_Absyn_Exp,r),(
void*)& Cyc_tunion_Absyn_Raw_exp}; static struct _tuple4 Cyc__gentuple_746={
offsetof( struct Cyc_Absyn_Exp,loc),( void*)& Cyc__genrep_2}; static struct
_tuple4* Cyc__genarr_747[ 3u]={& Cyc__gentuple_744,& Cyc__gentuple_745,& Cyc__gentuple_746};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Exp={ 4u, sizeof( struct Cyc_Absyn_Exp),{(
void*)(( struct _tuple4**) Cyc__genarr_747),( void*)(( struct _tuple4**) Cyc__genarr_747),(
void*)(( struct _tuple4**) Cyc__genarr_747 +  3u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_65={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Exp)}; static struct
_tuple4 Cyc__gentuple_66={ offsetof( struct _tuple27,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_67={ offsetof( struct _tuple27,f2),( void*)&
Cyc__genrep_65}; static struct _tuple4* Cyc__genarr_68[ 2u]={& Cyc__gentuple_66,&
Cyc__gentuple_67}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_64={ 4u,
sizeof( struct _tuple27),{( void*)(( struct _tuple4**) Cyc__genarr_68),( void*)((
struct _tuple4**) Cyc__genarr_68),( void*)(( struct _tuple4**) Cyc__genarr_68 + 
2u)}}; static struct _tuple4 Cyc__gentuple_829={ 0,( void*)& Cyc__genrep_64};
static struct _tuple4* Cyc__genarr_830[ 1u]={& Cyc__gentuple_829}; struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_Bounds={ 5u,{( void*)(( struct _tuple4**) Cyc__genarr_830),(
void*)(( struct _tuple4**) Cyc__genarr_830),( void*)(( struct _tuple4**) Cyc__genarr_830
+  1u)}}; static struct _tuple4 Cyc__gentuple_831={ offsetof( struct _tuple4,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_832={ offsetof(
struct _tuple4,f2),( void*)& Cyc_tunion_Absyn_Bounds}; static struct _tuple4*
Cyc__genarr_833[ 2u]={& Cyc__gentuple_831,& Cyc__gentuple_832}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_828={ 4u, sizeof( struct _tuple4),{( void*)((
struct _tuple4**) Cyc__genarr_833),( void*)(( struct _tuple4**) Cyc__genarr_833),(
void*)(( struct _tuple4**) Cyc__genarr_833 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_824; static struct _tuple4 Cyc__gentuple_825={ offsetof( struct
_tuple10,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_826={
offsetof( struct _tuple10,f2),( void*)& Cyc__genrep_823}; static struct _tuple4*
Cyc__genarr_827[ 2u]={& Cyc__gentuple_825,& Cyc__gentuple_826}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_824={ 4u, sizeof( struct _tuple10),{( void*)((
struct _tuple4**) Cyc__genarr_827),( void*)(( struct _tuple4**) Cyc__genarr_827),(
void*)(( struct _tuple4**) Cyc__genarr_827 +  2u)}}; static struct _tuple4 Cyc__gentuple_834={
0,( void*)& Cyc__genrep_828}; static struct _tuple4 Cyc__gentuple_835={ 1,( void*)&
Cyc__genrep_824}; static struct _tuple4* Cyc__genarr_836[ 2u]={& Cyc__gentuple_834,&
Cyc__gentuple_835}; struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0tunion_Absyn_Bounds2={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_836),( void*)(( struct _tuple4**)
Cyc__genarr_836),( void*)(( struct _tuple4**) Cyc__genarr_836 +  2u)}}; static
struct _tuple4 Cyc__gentuple_837={ offsetof( struct Cyc_Absyn_Conref,v),( void*)&
Cyc_tunion_Absyn_Constraint0tunion_Absyn_Bounds2}; static struct _tuple4* Cyc__genarr_838[
1u]={& Cyc__gentuple_837}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Conref0tunion_Absyn_Bounds2={
4u, sizeof( struct Cyc_Absyn_Conref),{( void*)(( struct _tuple4**) Cyc__genarr_838),(
void*)(( struct _tuple4**) Cyc__genarr_838),( void*)(( struct _tuple4**) Cyc__genarr_838
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_823={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Conref0tunion_Absyn_Bounds2)}; static struct
_tuple4 Cyc__gentuple_849={ offsetof( struct Cyc_Absyn_PtrInfo,elt_typ),( void*)&
Cyc_tunion_Absyn_Type}; static struct _tuple4 Cyc__gentuple_850={ offsetof(
struct Cyc_Absyn_PtrInfo,rgn_typ),( void*)& Cyc_tunion_Absyn_Type}; static
struct _tuple4 Cyc__gentuple_851={ offsetof( struct Cyc_Absyn_PtrInfo,nullable),(
void*)& Cyc__genrep_839}; static struct _tuple4 Cyc__gentuple_852={ offsetof(
struct Cyc_Absyn_PtrInfo,tq),( void*)& Cyc__genrep_99}; static struct _tuple4
Cyc__gentuple_853={ offsetof( struct Cyc_Absyn_PtrInfo,bounds),( void*)& Cyc__genrep_823};
static struct _tuple4* Cyc__genarr_854[ 5u]={& Cyc__gentuple_849,& Cyc__gentuple_850,&
Cyc__gentuple_851,& Cyc__gentuple_852,& Cyc__gentuple_853}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_PtrInfo={ 4u, sizeof( struct Cyc_Absyn_PtrInfo),{( void*)((
struct _tuple4**) Cyc__genarr_854),( void*)(( struct _tuple4**) Cyc__genarr_854),(
void*)(( struct _tuple4**) Cyc__genarr_854 +  5u)}}; struct _tuple70{
unsigned int f1; struct Cyc_Absyn_PtrInfo f2; } ; static struct _tuple4 Cyc__gentuple_855={
offsetof( struct _tuple70,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_856={ offsetof( struct _tuple70,f2),( void*)& Cyc_struct_Absyn_PtrInfo};
static struct _tuple4* Cyc__genarr_857[ 2u]={& Cyc__gentuple_855,& Cyc__gentuple_856};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_822={ 4u, sizeof( struct
_tuple70),{( void*)(( struct _tuple4**) Cyc__genarr_857),( void*)(( struct
_tuple4**) Cyc__genarr_857),( void*)(( struct _tuple4**) Cyc__genarr_857 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_816; extern struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_Size_of; static struct _tuple4* Cyc__genarr_817[ 0u]={}; struct
Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Size_of={ 5u,{( void*)(( struct
_tuple4**) Cyc__genarr_817),( void*)(( struct _tuple4**) Cyc__genarr_817),( void*)((
struct _tuple4**) Cyc__genarr_817 +  0u)}}; static struct _tuple4 Cyc__gentuple_818={
offsetof( struct _tuple60,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_819={ offsetof( struct _tuple60,f2),( void*)& Cyc_tunion_Absyn_Sign};
static struct _tuple4 Cyc__gentuple_820={ offsetof( struct _tuple60,f3),( void*)&
Cyc_tunion_Absyn_Size_of}; static struct _tuple4* Cyc__genarr_821[ 3u]={& Cyc__gentuple_818,&
Cyc__gentuple_819,& Cyc__gentuple_820}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_816={ 4u, sizeof( struct _tuple60),{( void*)(( struct _tuple4**) Cyc__genarr_821),(
void*)(( struct _tuple4**) Cyc__genarr_821),( void*)(( struct _tuple4**) Cyc__genarr_821
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_810; struct
_tuple71{ unsigned int f1; void* f2; struct Cyc_Absyn_Tqual f3; struct Cyc_Absyn_Exp*
f4; } ; static struct _tuple4 Cyc__gentuple_811={ offsetof( struct _tuple71,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_812={ offsetof(
struct _tuple71,f2),( void*)& Cyc_tunion_Absyn_Type}; static struct _tuple4 Cyc__gentuple_813={
offsetof( struct _tuple71,f3),( void*)& Cyc__genrep_99}; static struct _tuple4
Cyc__gentuple_814={ offsetof( struct _tuple71,f4),( void*)& Cyc__genrep_63};
static struct _tuple4* Cyc__genarr_815[ 4u]={& Cyc__gentuple_811,& Cyc__gentuple_812,&
Cyc__gentuple_813,& Cyc__gentuple_814}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_810={ 4u, sizeof( struct _tuple71),{( void*)(( struct _tuple4**) Cyc__genarr_815),(
void*)(( struct _tuple4**) Cyc__genarr_815),( void*)(( struct _tuple4**) Cyc__genarr_815
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_793; extern struct
Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_FnInfo; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_794; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060struct_Core_Opt0const_unsigned_char_342_24struct_Absyn_Tqual4099___tunion_Absyn_Type1_446H2;
static struct _tuple4 Cyc__gentuple_795={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_566}; static struct _tuple4 Cyc__gentuple_796={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_794}; static struct _tuple4* Cyc__genarr_797[
2u]={& Cyc__gentuple_795,& Cyc__gentuple_796}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List060struct_Core_Opt0const_unsigned_char_342_24struct_Absyn_Tqual4099___tunion_Absyn_Type1_446H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_797),(
void*)(( struct _tuple4**) Cyc__genarr_797),( void*)(( struct _tuple4**) Cyc__genarr_797
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_794={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060struct_Core_Opt0const_unsigned_char_342_24struct_Absyn_Tqual4099___tunion_Absyn_Type1_446H2)};
static struct _tuple4 Cyc__gentuple_798={ offsetof( struct Cyc_Absyn_FnInfo,tvars),(
void*)& Cyc__genrep_174}; static struct _tuple4 Cyc__gentuple_799={ offsetof(
struct Cyc_Absyn_FnInfo,effect),( void*)& Cyc__genrep_52}; static struct _tuple4
Cyc__gentuple_800={ offsetof( struct Cyc_Absyn_FnInfo,ret_typ),( void*)& Cyc_tunion_Absyn_Type};
static struct _tuple4 Cyc__gentuple_801={ offsetof( struct Cyc_Absyn_FnInfo,args),(
void*)& Cyc__genrep_794}; static struct _tuple4 Cyc__gentuple_802={ offsetof(
struct Cyc_Absyn_FnInfo,c_varargs),( void*)(( void*)& Cyc__genrep_73)}; static
struct _tuple4 Cyc__gentuple_803={ offsetof( struct Cyc_Absyn_FnInfo,cyc_varargs),(
void*)& Cyc__genrep_464}; static struct _tuple4 Cyc__gentuple_804={ offsetof(
struct Cyc_Absyn_FnInfo,rgn_po),( void*)& Cyc__genrep_455}; static struct
_tuple4 Cyc__gentuple_805={ offsetof( struct Cyc_Absyn_FnInfo,attributes),( void*)&
Cyc__genrep_71}; static struct _tuple4* Cyc__genarr_806[ 8u]={& Cyc__gentuple_798,&
Cyc__gentuple_799,& Cyc__gentuple_800,& Cyc__gentuple_801,& Cyc__gentuple_802,&
Cyc__gentuple_803,& Cyc__gentuple_804,& Cyc__gentuple_805}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_FnInfo={ 4u, sizeof( struct Cyc_Absyn_FnInfo),{( void*)((
struct _tuple4**) Cyc__genarr_806),( void*)(( struct _tuple4**) Cyc__genarr_806),(
void*)(( struct _tuple4**) Cyc__genarr_806 +  8u)}}; struct _tuple72{
unsigned int f1; struct Cyc_Absyn_FnInfo f2; } ; static struct _tuple4 Cyc__gentuple_807={
offsetof( struct _tuple72,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_808={ offsetof( struct _tuple72,f2),( void*)& Cyc_struct_Absyn_FnInfo};
static struct _tuple4* Cyc__genarr_809[ 2u]={& Cyc__gentuple_807,& Cyc__gentuple_808};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_793={ 4u, sizeof( struct
_tuple72),{( void*)(( struct _tuple4**) Cyc__genarr_809),( void*)(( struct
_tuple4**) Cyc__genarr_809),( void*)(( struct _tuple4**) Cyc__genarr_809 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_789; static struct _tuple4
Cyc__gentuple_790={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_791={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_217}; static struct _tuple4* Cyc__genarr_792[ 2u]={& Cyc__gentuple_790,&
Cyc__gentuple_791}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_789={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_792),( void*)((
struct _tuple4**) Cyc__genarr_792),( void*)(( struct _tuple4**) Cyc__genarr_792
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_782; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_776; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_776={ 2u, 1,( void*)(( void*)& Cyc__genrep_10)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_783; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_783={ 2u,
1,( void*)(( void*)& Cyc__genrep_256)}; struct _tuple73{ unsigned int f1; struct
_tuple0* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl** f4; } ;
static struct _tuple4 Cyc__gentuple_784={ offsetof( struct _tuple73,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_785={ offsetof( struct
_tuple73,f2),( void*)& Cyc__genrep_776}; static struct _tuple4 Cyc__gentuple_786={
offsetof( struct _tuple73,f3),( void*)& Cyc__genrep_44}; static struct _tuple4
Cyc__gentuple_787={ offsetof( struct _tuple73,f4),( void*)& Cyc__genrep_783};
static struct _tuple4* Cyc__genarr_788[ 4u]={& Cyc__gentuple_784,& Cyc__gentuple_785,&
Cyc__gentuple_786,& Cyc__gentuple_787}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_782={ 4u, sizeof( struct _tuple73),{( void*)(( struct _tuple4**) Cyc__genarr_788),(
void*)(( struct _tuple4**) Cyc__genarr_788),( void*)(( struct _tuple4**) Cyc__genarr_788
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_767; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_768; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_769; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Uniondecl;
static struct _tuple4 Cyc__gentuple_770={ offsetof( struct Cyc_Absyn_Uniondecl,sc),(
void*)& Cyc_tunion_Absyn_Scope}; static struct _tuple4 Cyc__gentuple_771={
offsetof( struct Cyc_Absyn_Uniondecl,name),( void*)& Cyc__genrep_271}; static
struct _tuple4 Cyc__gentuple_772={ offsetof( struct Cyc_Absyn_Uniondecl,tvs),(
void*)& Cyc__genrep_174}; static struct _tuple4 Cyc__gentuple_773={ offsetof(
struct Cyc_Absyn_Uniondecl,fields),( void*)& Cyc__genrep_257}; static struct
_tuple4 Cyc__gentuple_774={ offsetof( struct Cyc_Absyn_Uniondecl,attributes),(
void*)& Cyc__genrep_71}; static struct _tuple4* Cyc__genarr_775[ 5u]={& Cyc__gentuple_770,&
Cyc__gentuple_771,& Cyc__gentuple_772,& Cyc__gentuple_773,& Cyc__gentuple_774};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Uniondecl={ 4u, sizeof( struct
Cyc_Absyn_Uniondecl),{( void*)(( struct _tuple4**) Cyc__genarr_775),( void*)((
struct _tuple4**) Cyc__genarr_775),( void*)(( struct _tuple4**) Cyc__genarr_775
+  5u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_769={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Uniondecl)}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_768={ 2u, 1,( void*)(( void*)& Cyc__genrep_769)}; struct _tuple74{
unsigned int f1; struct _tuple0* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Uniondecl**
f4; } ; static struct _tuple4 Cyc__gentuple_777={ offsetof( struct _tuple74,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_778={ offsetof(
struct _tuple74,f2),( void*)& Cyc__genrep_776}; static struct _tuple4 Cyc__gentuple_779={
offsetof( struct _tuple74,f3),( void*)& Cyc__genrep_44}; static struct _tuple4
Cyc__gentuple_780={ offsetof( struct _tuple74,f4),( void*)& Cyc__genrep_768};
static struct _tuple4* Cyc__genarr_781[ 4u]={& Cyc__gentuple_777,& Cyc__gentuple_778,&
Cyc__gentuple_779,& Cyc__gentuple_780}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_767={ 4u, sizeof( struct _tuple74),{( void*)(( struct _tuple4**) Cyc__genarr_781),(
void*)(( struct _tuple4**) Cyc__genarr_781),( void*)(( struct _tuple4**) Cyc__genarr_781
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_763; static struct
_tuple4 Cyc__gentuple_764={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_765={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_258}; static struct _tuple4* Cyc__genarr_766[ 2u]={& Cyc__gentuple_764,&
Cyc__gentuple_765}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_763={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_766),( void*)((
struct _tuple4**) Cyc__genarr_766),( void*)(( struct _tuple4**) Cyc__genarr_766
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_758; struct
_tuple75{ unsigned int f1; struct _tuple0* f2; struct Cyc_Absyn_Enumdecl* f3; }
; static struct _tuple4 Cyc__gentuple_759={ offsetof( struct _tuple75,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_760={ offsetof( struct
_tuple75,f2),( void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_761={
offsetof( struct _tuple75,f3),( void*)& Cyc__genrep_528}; static struct _tuple4*
Cyc__genarr_762[ 3u]={& Cyc__gentuple_759,& Cyc__gentuple_760,& Cyc__gentuple_761};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_758={ 4u, sizeof( struct
_tuple75),{( void*)(( struct _tuple4**) Cyc__genarr_762),( void*)(( struct
_tuple4**) Cyc__genarr_762),( void*)(( struct _tuple4**) Cyc__genarr_762 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_60; static struct _tuple4 Cyc__gentuple_755={
offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_756={
offsetof( struct _tuple8,f2),( void*)& Cyc__genrep_61}; static struct _tuple4*
Cyc__genarr_757[ 2u]={& Cyc__gentuple_755,& Cyc__gentuple_756}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_60={ 4u, sizeof( struct _tuple8),{( void*)((
struct _tuple4**) Cyc__genarr_757),( void*)(( struct _tuple4**) Cyc__genarr_757),(
void*)(( struct _tuple4**) Cyc__genarr_757 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_51; struct _tuple76{ unsigned int f1; struct _tuple0* f2; struct Cyc_List_List*
f3; struct Cyc_Core_Opt* f4; } ; static struct _tuple4 Cyc__gentuple_55={
offsetof( struct _tuple76,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_56={ offsetof( struct _tuple76,f2),( void*)& Cyc__genrep_9};
static struct _tuple4 Cyc__gentuple_57={ offsetof( struct _tuple76,f3),( void*)&
Cyc__genrep_44}; static struct _tuple4 Cyc__gentuple_58={ offsetof( struct
_tuple76,f4),( void*)& Cyc__genrep_52}; static struct _tuple4* Cyc__genarr_59[ 4u]={&
Cyc__gentuple_55,& Cyc__gentuple_56,& Cyc__gentuple_57,& Cyc__gentuple_58};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_51={ 4u, sizeof( struct
_tuple76),{( void*)(( struct _tuple4**) Cyc__genarr_59),( void*)(( struct
_tuple4**) Cyc__genarr_59),( void*)(( struct _tuple4**) Cyc__genarr_59 +  4u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_43; static struct _tuple4 Cyc__gentuple_48={
offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_49={
offsetof( struct _tuple8,f2),( void*)& Cyc__genrep_44}; static struct _tuple4*
Cyc__genarr_50[ 2u]={& Cyc__gentuple_48,& Cyc__gentuple_49}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_43={ 4u, sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_50),(
void*)(( struct _tuple4**) Cyc__genarr_50),( void*)(( struct _tuple4**) Cyc__genarr_50
+  2u)}}; static struct _tuple4 Cyc__gentuple_921={ 0,( void*)& Cyc__genrep_908};
static struct _tuple4 Cyc__gentuple_922={ 1,( void*)& Cyc__genrep_904}; static
struct _tuple4 Cyc__gentuple_923={ 2,( void*)& Cyc__genrep_881}; static struct
_tuple4 Cyc__gentuple_924={ 3,( void*)& Cyc__genrep_858}; static struct _tuple4
Cyc__gentuple_925={ 4,( void*)& Cyc__genrep_822}; static struct _tuple4 Cyc__gentuple_926={
5,( void*)& Cyc__genrep_816}; static struct _tuple4 Cyc__gentuple_927={ 6,( void*)&
Cyc__genrep_810}; static struct _tuple4 Cyc__gentuple_928={ 7,( void*)& Cyc__genrep_793};
static struct _tuple4 Cyc__gentuple_929={ 8,( void*)& Cyc__genrep_789}; static
struct _tuple4 Cyc__gentuple_930={ 9,( void*)& Cyc__genrep_782}; static struct
_tuple4 Cyc__gentuple_931={ 10,( void*)& Cyc__genrep_767}; static struct _tuple4
Cyc__gentuple_932={ 11,( void*)& Cyc__genrep_763}; static struct _tuple4 Cyc__gentuple_933={
12,( void*)& Cyc__genrep_763}; static struct _tuple4 Cyc__gentuple_934={ 13,(
void*)& Cyc__genrep_758}; static struct _tuple4 Cyc__gentuple_935={ 14,( void*)&
Cyc__genrep_60}; static struct _tuple4 Cyc__gentuple_936={ 15,( void*)& Cyc__genrep_39};
static struct _tuple4 Cyc__gentuple_937={ 16,( void*)& Cyc__genrep_51}; static
struct _tuple4 Cyc__gentuple_938={ 17,( void*)& Cyc__genrep_39}; static struct
_tuple4 Cyc__gentuple_939={ 18,( void*)& Cyc__genrep_43}; static struct _tuple4
Cyc__gentuple_940={ 19,( void*)& Cyc__genrep_39}; static struct _tuple4* Cyc__genarr_941[
20u]={& Cyc__gentuple_921,& Cyc__gentuple_922,& Cyc__gentuple_923,& Cyc__gentuple_924,&
Cyc__gentuple_925,& Cyc__gentuple_926,& Cyc__gentuple_927,& Cyc__gentuple_928,&
Cyc__gentuple_929,& Cyc__gentuple_930,& Cyc__gentuple_931,& Cyc__gentuple_932,&
Cyc__gentuple_933,& Cyc__gentuple_934,& Cyc__gentuple_935,& Cyc__gentuple_936,&
Cyc__gentuple_937,& Cyc__gentuple_938,& Cyc__gentuple_939,& Cyc__gentuple_940};
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Type={ 5u,{( void*)(( struct
_tuple4**) Cyc__genarr_941),( void*)(( struct _tuple4**) Cyc__genarr_941),( void*)((
struct _tuple4**) Cyc__genarr_941 +  20u)}}; static struct _tuple4 Cyc__gentuple_103={
offsetof( struct Cyc_Absyn_Vardecl,sc),( void*)& Cyc_tunion_Absyn_Scope}; static
struct _tuple4 Cyc__gentuple_104={ offsetof( struct Cyc_Absyn_Vardecl,name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_105={ offsetof(
struct Cyc_Absyn_Vardecl,tq),( void*)& Cyc__genrep_99}; static struct _tuple4
Cyc__gentuple_106={ offsetof( struct Cyc_Absyn_Vardecl,type),( void*)& Cyc_tunion_Absyn_Type};
static struct _tuple4 Cyc__gentuple_107={ offsetof( struct Cyc_Absyn_Vardecl,initializer),(
void*)& Cyc__genrep_63}; static struct _tuple4 Cyc__gentuple_108={ offsetof(
struct Cyc_Absyn_Vardecl,rgn),( void*)& Cyc__genrep_52}; static struct _tuple4
Cyc__gentuple_109={ offsetof( struct Cyc_Absyn_Vardecl,attributes),( void*)& Cyc__genrep_71};
static struct _tuple4* Cyc__genarr_110[ 7u]={& Cyc__gentuple_103,& Cyc__gentuple_104,&
Cyc__gentuple_105,& Cyc__gentuple_106,& Cyc__gentuple_107,& Cyc__gentuple_108,&
Cyc__gentuple_109}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Vardecl={ 4u,
sizeof( struct Cyc_Absyn_Vardecl),{( void*)(( struct _tuple4**) Cyc__genarr_110),(
void*)(( struct _tuple4**) Cyc__genarr_110),( void*)(( struct _tuple4**) Cyc__genarr_110
+  7u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_98={ 2u, 1,( void*)((
void*)& Cyc_struct_Absyn_Vardecl)}; struct _tuple77{ unsigned int f1; struct Cyc_Absyn_Vardecl*
f2; } ; static struct _tuple4 Cyc__gentuple_287={ offsetof( struct _tuple77,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_288={ offsetof(
struct _tuple77,f2),( void*)& Cyc__genrep_98}; static struct _tuple4* Cyc__genarr_289[
2u]={& Cyc__gentuple_287,& Cyc__gentuple_288}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_286={ 4u, sizeof( struct _tuple77),{( void*)(( struct _tuple4**) Cyc__genarr_289),(
void*)(( struct _tuple4**) Cyc__genarr_289),( void*)(( struct _tuple4**) Cyc__genarr_289
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_969; struct
_tuple78{ unsigned int f1; struct Cyc_Absyn_Pat* f2; struct Cyc_Core_Opt* f3;
struct Cyc_Core_Opt* f4; struct Cyc_Absyn_Exp* f5; int f6; } ; static struct
_tuple4 Cyc__gentuple_970={ offsetof( struct _tuple78,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_971={ offsetof( struct _tuple78,f2),( void*)&
Cyc__genrep_152}; static struct _tuple4 Cyc__gentuple_972={ offsetof( struct
_tuple78,f3),( void*)& Cyc__genrep_96}; static struct _tuple4 Cyc__gentuple_973={
offsetof( struct _tuple78,f4),( void*)& Cyc__genrep_52}; static struct _tuple4
Cyc__gentuple_974={ offsetof( struct _tuple78,f5),( void*)& Cyc__genrep_65};
static struct _tuple4 Cyc__gentuple_975={ offsetof( struct _tuple78,f6),( void*)((
void*)& Cyc__genrep_73)}; static struct _tuple4* Cyc__genarr_976[ 6u]={& Cyc__gentuple_970,&
Cyc__gentuple_971,& Cyc__gentuple_972,& Cyc__gentuple_973,& Cyc__gentuple_974,&
Cyc__gentuple_975}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_969={ 4u,
sizeof( struct _tuple78),{( void*)(( struct _tuple4**) Cyc__genarr_976),( void*)((
struct _tuple4**) Cyc__genarr_976),( void*)(( struct _tuple4**) Cyc__genarr_976
+  6u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_965; static struct
_tuple4 Cyc__gentuple_966={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_967={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_97}; static struct _tuple4* Cyc__genarr_968[ 2u]={& Cyc__gentuple_966,&
Cyc__gentuple_967}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_965={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_968),( void*)((
struct _tuple4**) Cyc__genarr_968),( void*)(( struct _tuple4**) Cyc__genarr_968
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_961; struct
_tuple79{ unsigned int f1; struct Cyc_Absyn_Structdecl* f2; } ; static struct
_tuple4 Cyc__gentuple_962={ offsetof( struct _tuple79,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_963={ offsetof( struct _tuple79,f2),( void*)((
void*)& Cyc__genrep_256)}; static struct _tuple4* Cyc__genarr_964[ 2u]={& Cyc__gentuple_962,&
Cyc__gentuple_963}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_961={ 4u,
sizeof( struct _tuple79),{( void*)(( struct _tuple4**) Cyc__genarr_964),( void*)((
struct _tuple4**) Cyc__genarr_964),( void*)(( struct _tuple4**) Cyc__genarr_964
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_957; struct
_tuple80{ unsigned int f1; struct Cyc_Absyn_Uniondecl* f2; } ; static struct
_tuple4 Cyc__gentuple_958={ offsetof( struct _tuple80,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_959={ offsetof( struct _tuple80,f2),( void*)((
void*)& Cyc__genrep_769)}; static struct _tuple4* Cyc__genarr_960[ 2u]={& Cyc__gentuple_958,&
Cyc__gentuple_959}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_957={ 4u,
sizeof( struct _tuple80),{( void*)(( struct _tuple4**) Cyc__genarr_960),( void*)((
struct _tuple4**) Cyc__genarr_960),( void*)(( struct _tuple4**) Cyc__genarr_960
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_953; struct
_tuple81{ unsigned int f1; struct Cyc_Absyn_Tuniondecl* f2; } ; static struct
_tuple4 Cyc__gentuple_954={ offsetof( struct _tuple81,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_955={ offsetof( struct _tuple81,f2),( void*)((
void*)& Cyc__genrep_232)}; static struct _tuple4* Cyc__genarr_956[ 2u]={& Cyc__gentuple_954,&
Cyc__gentuple_955}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_953={ 4u,
sizeof( struct _tuple81),{( void*)(( struct _tuple4**) Cyc__genarr_956),( void*)((
struct _tuple4**) Cyc__genarr_956),( void*)(( struct _tuple4**) Cyc__genarr_956
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_949; struct
_tuple82{ unsigned int f1; struct Cyc_Absyn_Enumdecl* f2; } ; static struct
_tuple4 Cyc__gentuple_950={ offsetof( struct _tuple82,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_951={ offsetof( struct _tuple82,f2),( void*)&
Cyc__genrep_203}; static struct _tuple4* Cyc__genarr_952[ 2u]={& Cyc__gentuple_950,&
Cyc__gentuple_951}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_949={ 4u,
sizeof( struct _tuple82),{( void*)(( struct _tuple4**) Cyc__genarr_952),( void*)((
struct _tuple4**) Cyc__genarr_952),( void*)(( struct _tuple4**) Cyc__genarr_952
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_37; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_38; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Typedefdecl; static struct _tuple4 Cyc__gentuple_942={
offsetof( struct Cyc_Absyn_Typedefdecl,name),( void*)& Cyc__genrep_9}; static
struct _tuple4 Cyc__gentuple_943={ offsetof( struct Cyc_Absyn_Typedefdecl,tvs),(
void*)& Cyc__genrep_174}; static struct _tuple4 Cyc__gentuple_944={ offsetof(
struct Cyc_Absyn_Typedefdecl,defn),( void*)& Cyc_tunion_Absyn_Type}; static
struct _tuple4* Cyc__genarr_945[ 3u]={& Cyc__gentuple_942,& Cyc__gentuple_943,&
Cyc__gentuple_944}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Typedefdecl={
4u, sizeof( struct Cyc_Absyn_Typedefdecl),{( void*)(( struct _tuple4**) Cyc__genarr_945),(
void*)(( struct _tuple4**) Cyc__genarr_945),( void*)(( struct _tuple4**) Cyc__genarr_945
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_38={ 2u, 1,( void*)((
void*)& Cyc_struct_Absyn_Typedefdecl)}; struct _tuple83{ unsigned int f1; struct
Cyc_Absyn_Typedefdecl* f2; } ; static struct _tuple4 Cyc__gentuple_946={
offsetof( struct _tuple83,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_947={ offsetof( struct _tuple83,f2),( void*)& Cyc__genrep_38};
static struct _tuple4* Cyc__genarr_948[ 2u]={& Cyc__gentuple_946,& Cyc__gentuple_947};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_37={ 4u, sizeof( struct
_tuple83),{( void*)(( struct _tuple4**) Cyc__genarr_948),( void*)(( struct
_tuple4**) Cyc__genarr_948),( void*)(( struct _tuple4**) Cyc__genarr_948 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_32; struct _tuple84{
unsigned int f1; struct _tagged_arr* f2; struct Cyc_List_List* f3; } ; static
struct _tuple4 Cyc__gentuple_33={ offsetof( struct _tuple84,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_34={ offsetof( struct _tuple84,f2),( void*)&
Cyc__genrep_11}; static struct _tuple4 Cyc__gentuple_35={ offsetof( struct
_tuple84,f3),( void*)& Cyc__genrep_0}; static struct _tuple4* Cyc__genarr_36[ 3u]={&
Cyc__gentuple_33,& Cyc__gentuple_34,& Cyc__gentuple_35}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_32={ 4u, sizeof( struct _tuple84),{( void*)(( struct _tuple4**) Cyc__genarr_36),(
void*)(( struct _tuple4**) Cyc__genarr_36),( void*)(( struct _tuple4**) Cyc__genarr_36
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_8; struct _tuple85{
unsigned int f1; struct _tuple0* f2; struct Cyc_List_List* f3; } ; static struct
_tuple4 Cyc__gentuple_28={ offsetof( struct _tuple85,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_29={ offsetof( struct _tuple85,f2),( void*)&
Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_30={ offsetof( struct
_tuple85,f3),( void*)& Cyc__genrep_0}; static struct _tuple4* Cyc__genarr_31[ 3u]={&
Cyc__gentuple_28,& Cyc__gentuple_29,& Cyc__gentuple_30}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_8={ 4u, sizeof( struct _tuple85),{( void*)(( struct _tuple4**) Cyc__genarr_31),(
void*)(( struct _tuple4**) Cyc__genarr_31),( void*)(( struct _tuple4**) Cyc__genarr_31
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_3; static struct
_tuple4 Cyc__gentuple_5={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_6={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_0}; static struct _tuple4* Cyc__genarr_7[ 2u]={& Cyc__gentuple_5,&
Cyc__gentuple_6}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_3={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_7),( void*)((
struct _tuple4**) Cyc__genarr_7),( void*)(( struct _tuple4**) Cyc__genarr_7 +  2u)}};
static struct _tuple4 Cyc__gentuple_977={ 0,( void*)& Cyc__genrep_286}; static
struct _tuple4 Cyc__gentuple_978={ 1,( void*)& Cyc__genrep_69}; static struct
_tuple4 Cyc__gentuple_979={ 2,( void*)& Cyc__genrep_969}; static struct _tuple4
Cyc__gentuple_980={ 3,( void*)& Cyc__genrep_965}; static struct _tuple4 Cyc__gentuple_981={
4,( void*)& Cyc__genrep_961}; static struct _tuple4 Cyc__gentuple_982={ 5,( void*)&
Cyc__genrep_957}; static struct _tuple4 Cyc__gentuple_983={ 6,( void*)& Cyc__genrep_953};
static struct _tuple4 Cyc__gentuple_984={ 7,( void*)& Cyc__genrep_949}; static
struct _tuple4 Cyc__gentuple_985={ 8,( void*)& Cyc__genrep_37}; static struct
_tuple4 Cyc__gentuple_986={ 9,( void*)& Cyc__genrep_32}; static struct _tuple4
Cyc__gentuple_987={ 10,( void*)& Cyc__genrep_8}; static struct _tuple4 Cyc__gentuple_988={
11,( void*)& Cyc__genrep_3}; static struct _tuple4* Cyc__genarr_989[ 12u]={& Cyc__gentuple_977,&
Cyc__gentuple_978,& Cyc__gentuple_979,& Cyc__gentuple_980,& Cyc__gentuple_981,&
Cyc__gentuple_982,& Cyc__gentuple_983,& Cyc__gentuple_984,& Cyc__gentuple_985,&
Cyc__gentuple_986,& Cyc__gentuple_987,& Cyc__gentuple_988}; struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_Raw_decl={ 5u,{( void*)(( struct _tuple4**) Cyc__genarr_989),(
void*)(( struct _tuple4**) Cyc__genarr_989),( void*)(( struct _tuple4**) Cyc__genarr_989
+  12u)}}; static struct _tuple4 Cyc__gentuple_990={ offsetof( struct Cyc_Absyn_Decl,r),(
void*)& Cyc_tunion_Absyn_Raw_decl}; static struct _tuple4 Cyc__gentuple_991={
offsetof( struct Cyc_Absyn_Decl,loc),( void*)& Cyc__genrep_2}; static struct
_tuple4* Cyc__genarr_992[ 2u]={& Cyc__gentuple_990,& Cyc__gentuple_991}; struct
Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Decl={ 4u, sizeof( struct Cyc_Absyn_Decl),{(
void*)(( struct _tuple4**) Cyc__genarr_992),( void*)(( struct _tuple4**) Cyc__genarr_992),(
void*)(( struct _tuple4**) Cyc__genarr_992 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Decl)}; static struct
_tuple4 Cyc__gentuple_993={ offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_1};
static struct _tuple4 Cyc__gentuple_994={ offsetof( struct Cyc_List_List,tl),(
void*)& Cyc__genrep_0}; static struct _tuple4* Cyc__genarr_995[ 2u]={& Cyc__gentuple_993,&
Cyc__gentuple_994}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0struct_Absyn_Decl_446H2={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_995),(
void*)(( struct _tuple4**) Cyc__genarr_995),( void*)(( struct _tuple4**) Cyc__genarr_995
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0={ 2u, 1,( void*)((
void*)& Cyc_struct_List_List0struct_Absyn_Decl_446H2)}; void* Cyc_decls_rep=(
void*)& Cyc__genrep_0;
