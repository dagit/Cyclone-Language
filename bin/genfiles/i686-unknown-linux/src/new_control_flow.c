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
struct _tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd( struct
_tuple0*); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
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
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Set_Set; extern unsigned char
Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* cmp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* k, void* v); extern struct Cyc_Dict_Dict* Cyc_Dict_insert_new(
struct Cyc_Dict_Dict* d, void* k, void* v); extern void* Cyc_Dict_lookup( struct
Cyc_Dict_Dict* d, void* k); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern void* Cyc_Std___assert_fail( struct _tagged_arr
assertion, struct _tagged_arr file, unsigned int line); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
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
const int Cyc_Absyn_DoubleType= 2; static const int Cyc_Absyn_ArrayType= 6;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual f2;
struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 8;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_StructType= 9; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_AnonEnumType=
14; struct Cyc_Absyn_AnonEnumType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnHandleType= 15; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 16; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
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
15u]; extern struct _tagged_arr* Cyc_Absyn_fieldname( int); struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress( void* t); extern
int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b); extern unsigned int
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_CfFlowInfo_Place;
struct Cyc_CfFlowInfo_InitState; static const int Cyc_CfFlowInfo_VarRoot= 0;
struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_CfFlowInfo_Place{ void* root;
struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static
const int Cyc_CfFlowInfo_Unesc= 1; static const int Cyc_CfFlowInfo_NoneIL= 0;
static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; static const int Cyc_CfFlowInfo_MustPointTo= 0; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; struct Cyc_CfFlowInfo_InitState{
void* esc; void* level; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct
Cyc_CfFlowInfo_LeafPI_struct{ int tag; struct Cyc_CfFlowInfo_InitState f1; } ;
static const int Cyc_CfFlowInfo_DictPI= 1; struct Cyc_CfFlowInfo_DictPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t);
extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*, void*); extern void* Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**, void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt*
enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; struct Cyc_Set_Set**
all_changed; } ; extern void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv*
env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt( struct
Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); extern void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u];
static const int Cyc_CfAbsexp_OrderedG= 0; static const int Cyc_CfAbsexp_UnorderedG=
1; static const int Cyc_CfAbsexp_OneofG= 2; extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkLocalOp( struct Cyc_Absyn_Vardecl*); extern void*
Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp*); extern void* Cyc_CfAbsexp_mkDictOp(
struct Cyc_Dict_Dict*); extern void* Cyc_CfAbsexp_mkAddressOp( void*); extern
void* Cyc_CfAbsexp_mkMemberOp( void*, struct _tagged_arr* f); extern void* Cyc_CfAbsexp_mkDerefOp(
void*); extern void* Cyc_CfAbsexp_mkBottomAE(); extern void* Cyc_CfAbsexp_mkSkipAE();
extern void* Cyc_CfAbsexp_mkUseAE( void* ao); extern void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e);
extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s); extern void* Cyc_CfAbsexp_mkComprehensionAE(
struct Cyc_Absyn_Vardecl* vd, void* size_true_exp, void* size_false_exp, void*
size_aop, void* body_exp, void* body_op); extern void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2); extern void* Cyc_CfAbsexp_mkGroupAE_l( void* g,
struct Cyc_List_List* ael); extern void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Position_Segment* loc, void* ae, void* in_flow); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; struct Cyc_NewControlFlow_CFStmtAnnot{ struct Cyc_Absyn_Stmt*
encloser; int visited; void* flow; struct Cyc_List_List* absexps; } ;
unsigned char Cyc_NewControlFlow_CFAnnot[ 12u]="\000\000\000\000CFAnnot"; struct
Cyc_NewControlFlow_CFAnnot_struct{ unsigned char* tag; struct Cyc_NewControlFlow_CFStmtAnnot
f1; } ; void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser){( void*)( enclosee->annot=( void*)(( void*)({
struct Cyc_NewControlFlow_CFAnnot_struct* _temp0=( struct Cyc_NewControlFlow_CFAnnot_struct*)
_cycalloc( sizeof( struct Cyc_NewControlFlow_CFAnnot_struct)); _temp0[ 0]=({
struct Cyc_NewControlFlow_CFAnnot_struct _temp1; _temp1.tag= Cyc_NewControlFlow_CFAnnot;
_temp1.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot _temp2; _temp2.encloser=
encloser; _temp2.visited= 0; _temp2.flow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp2.absexps= 0; _temp2;}); _temp1;}); _temp0;})));} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot( struct Cyc_Absyn_Stmt* s){ void* _temp3=(
void*) s->annot; struct Cyc_NewControlFlow_CFStmtAnnot _temp9; struct Cyc_NewControlFlow_CFStmtAnnot*
_temp11; _LL5: if(*(( void**) _temp3) ==  Cyc_NewControlFlow_CFAnnot){ _LL10:
_temp9=(( struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1; _temp11=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp3)->f1; goto _LL6;} else{ goto _LL7;} _LL7: goto _LL8; _LL6: return _temp11;
_LL8:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp12=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp12[ 0]=({ struct Cyc_Core_Impossible_struct _temp13; _temp13.tag= Cyc_Core_Impossible;
_temp13.f1= _tag_arr("ControlFlow -- wrong stmt annotation", sizeof(
unsigned char), 37u); _temp13;}); _temp12;})); _LL4:;} static void Cyc_NewControlFlow_cf_set_absexps(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* ael){( Cyc_NewControlFlow_get_stmt_annot(
s))->absexps= ael;} struct Cyc_NewControlFlow_AbsEnv{ struct Cyc_Dict_Dict*
all_roots; } ; struct Cyc_NewControlFlow_AbsSyn{ void* when_true_exp; void*
when_false_exp; void* absop; } ; static void Cyc_NewControlFlow_add_var_root(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Vardecl* vd){ env->all_roots=
Cyc_Dict_insert_new( env->all_roots,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp14=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp14[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp15; _temp15.tag= Cyc_CfFlowInfo_VarRoot;
_temp15.f1= vd; _temp15;}); _temp14;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((
void*) vd->type));} static void Cyc_NewControlFlow_add_malloc_root( struct Cyc_NewControlFlow_AbsEnv*
env, struct Cyc_Absyn_Exp* e, void* t){ env->all_roots= Cyc_Dict_insert_new( env->all_roots,(
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp16=( struct Cyc_CfFlowInfo_MallocPt_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp16[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp17; _temp17.tag= Cyc_CfFlowInfo_MallocPt;
_temp17.f1= e; _temp17;}); _temp16;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));}
static void* Cyc_NewControlFlow_meet_absexp( struct Cyc_NewControlFlow_AbsSyn
syn){ if(( void*) syn.when_true_exp == ( void*) syn.when_false_exp){ return(
void*) syn.when_true_exp;} return Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,(
void*) syn.when_true_exp,( void*) syn.when_false_exp);} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_abstract_exp( struct Cyc_NewControlFlow_AbsEnv* env, struct
Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv*
env, struct Cyc_Absyn_Stmt* s); static void* Cyc_NewControlFlow_use_exp( struct
Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
_temp18= Cyc_NewControlFlow_abstract_exp( env, e); return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_meet_absexp( _temp18), Cyc_CfAbsexp_mkUseAE((
void*) _temp18.absop));} static struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_use_exp_unordered(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_List_List* es){ void* _temp19=
Cyc_CfAbsexp_mkGroupAE_l(( void*) Cyc_CfAbsexp_UnorderedG,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_NewControlFlow_AbsEnv*, struct Cyc_Absyn_Exp*), struct
Cyc_NewControlFlow_AbsEnv* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_NewControlFlow_use_exp,
env, es)); return({ struct Cyc_NewControlFlow_AbsSyn _temp20; _temp20.when_true_exp=(
void*) _temp19; _temp20.when_false_exp=( void*) _temp19; _temp20.absop=( void*)
Cyc_CfAbsexp_mkUnknownOp(); _temp20;});} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_malloc(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp*
rgn_exp, struct Cyc_Absyn_Exp* init_exp){ void* _temp21= Cyc_CfAbsexp_mkMallocOp(
outer_e); void* ans_aexp= Cyc_CfAbsexp_mkSkipAE(); if( init_exp !=  0){ struct
Cyc_NewControlFlow_AbsSyn _temp22= Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*) _check_null( init_exp)); ans_aexp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_meet_absexp( _temp22), Cyc_CfAbsexp_mkAssignAE(
_temp21,( void*) _temp22.absop));} ans_aexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( outer_e), ans_aexp); if( rgn_exp !=  0){ ans_aexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env,( struct Cyc_Absyn_Exp*) _check_null( rgn_exp))), ans_aexp);} return({
struct Cyc_NewControlFlow_AbsSyn _temp23; _temp23.when_true_exp=( void*)
ans_aexp; _temp23.when_false_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp23.absop=(
void*) Cyc_CfAbsexp_mkAddressOp( _temp21); _temp23;});} struct _tuple4{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_abstract_exp( struct Cyc_NewControlFlow_AbsEnv* env, struct
Cyc_Absyn_Exp* e){ void* _temp24=( void*) e->r; struct Cyc_Absyn_Exp* _temp120;
struct Cyc_Absyn_Exp* _temp122; struct Cyc_Absyn_Exp* _temp124; void* _temp126;
void* _temp128; int _temp130; void* _temp132; void* _temp134; int _temp136; void*
_temp138; void* _temp140; void* _temp142; void* _temp144; struct Cyc_Absyn_Vardecl*
_temp146; void* _temp148; struct Cyc_Absyn_Vardecl* _temp150; void* _temp152;
struct Cyc_Absyn_Vardecl* _temp154; struct Cyc_Absyn_Exp* _temp156; struct Cyc_Core_Opt*
_temp158; struct Cyc_Absyn_Exp* _temp160; struct Cyc_List_List* _temp162; struct
Cyc_Absyn_Exp* _temp164; struct Cyc_Absyn_Exp* _temp166; struct Cyc_Absyn_Exp*
_temp168; struct Cyc_List_List* _temp170; struct Cyc_Absyn_Exp* _temp172; struct
Cyc_Absyn_Exp* _temp174; struct Cyc_Absyn_Exp* _temp176; struct Cyc_Absyn_Exp*
_temp178; struct _tagged_arr* _temp180; struct Cyc_Absyn_Exp* _temp182; struct
_tagged_arr* _temp184; struct Cyc_Absyn_Exp* _temp186; struct Cyc_Absyn_Exp*
_temp188; struct Cyc_Absyn_Exp* _temp190; void* _temp192; struct Cyc_Absyn_Exp*
_temp194; struct Cyc_List_List* _temp196; struct Cyc_List_List* _temp198; struct
Cyc_List_List* _temp200; struct Cyc_List_List* _temp202; struct Cyc_List_List*
_temp204; struct Cyc_Absyn_Exp* _temp206; struct Cyc_Absyn_Exp* _temp208; struct
Cyc_Absyn_Exp* _temp210; struct Cyc_Absyn_Exp* _temp212; struct Cyc_Absyn_Exp*
_temp214; struct Cyc_Absyn_Stmt* _temp216; struct Cyc_Absyn_Exp* _temp218;
struct Cyc_Absyn_Exp* _temp220; struct Cyc_Absyn_Exp* _temp222; struct Cyc_Absyn_Exp*
_temp224; struct Cyc_Absyn_Vardecl* _temp226; void* _temp228; _LL26: if(*(( int*)
_temp24) ==  Cyc_Absyn_Cast_e){ _LL121: _temp120=(( struct Cyc_Absyn_Cast_e_struct*)
_temp24)->f2; goto _LL27;} else{ goto _LL28;} _LL28: if(*(( int*) _temp24) == 
Cyc_Absyn_NoInstantiate_e){ _LL123: _temp122=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp24)->f1; goto _LL29;} else{ goto _LL30;} _LL30: if(*(( int*) _temp24) == 
Cyc_Absyn_Instantiate_e){ _LL125: _temp124=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp24)->f1; goto _LL31;} else{ goto _LL32;} _LL32: if(*(( int*) _temp24) == 
Cyc_Absyn_Const_e){ _LL127: _temp126=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp24)->f1; if( _temp126 == ( void*) Cyc_Absyn_Null_c){ goto _LL33;} else{
goto _LL34;}} else{ goto _LL34;} _LL34: if(*(( int*) _temp24) ==  Cyc_Absyn_Const_e){
_LL129: _temp128=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp24)->f1; if((
unsigned int) _temp128 >  1u?*(( int*) _temp128) ==  Cyc_Absyn_Int_c: 0){ _LL133:
_temp132=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp128)->f1; if( _temp132
== ( void*) Cyc_Absyn_Signed){ goto _LL131;} else{ goto _LL36;} _LL131: _temp130=((
struct Cyc_Absyn_Int_c_struct*) _temp128)->f2; if( _temp130 ==  0){ goto _LL35;}
else{ goto _LL36;}} else{ goto _LL36;}} else{ goto _LL36;} _LL36: if(*(( int*)
_temp24) ==  Cyc_Absyn_Const_e){ _LL135: _temp134=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp24)->f1; if(( unsigned int) _temp134 >  1u?*(( int*) _temp134) ==  Cyc_Absyn_Int_c:
0){ _LL139: _temp138=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp134)->f1;
if( _temp138 == ( void*) Cyc_Absyn_Signed){ goto _LL137;} else{ goto _LL38;}
_LL137: _temp136=(( struct Cyc_Absyn_Int_c_struct*) _temp134)->f2; if( _temp136
==  1){ goto _LL37;} else{ goto _LL38;}} else{ goto _LL38;}} else{ goto _LL38;}
_LL38: if(*(( int*) _temp24) ==  Cyc_Absyn_Const_e){ goto _LL39;} else{ goto
_LL40;} _LL40: if(*(( int*) _temp24) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL41;}
else{ goto _LL42;} _LL42: if(*(( int*) _temp24) ==  Cyc_Absyn_Sizeoftyp_e){ goto
_LL43;} else{ goto _LL44;} _LL44: if(*(( int*) _temp24) ==  Cyc_Absyn_Offsetof_e){
goto _LL45;} else{ goto _LL46;} _LL46: if(*(( int*) _temp24) ==  Cyc_Absyn_Gentyp_e){
goto _LL47;} else{ goto _LL48;} _LL48: if(*(( int*) _temp24) ==  Cyc_Absyn_Enum_e){
goto _LL49;} else{ goto _LL50;} _LL50: if(*(( int*) _temp24) ==  Cyc_Absyn_AnonEnum_e){
goto _LL51;} else{ goto _LL52;} _LL52: if(*(( int*) _temp24) ==  Cyc_Absyn_Var_e){
_LL141: _temp140=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp24)->f2; if((
unsigned int) _temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_Funname_b: 0){
goto _LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54: if(*(( int*) _temp24)
==  Cyc_Absyn_Var_e){ _LL143: _temp142=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp142 >  1u?*(( int*) _temp142) ==  Cyc_Absyn_Global_b:
0){ goto _LL55;} else{ goto _LL56;}} else{ goto _LL56;} _LL56: if(*(( int*)
_temp24) ==  Cyc_Absyn_Var_e){ _LL145: _temp144=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp144 >  1u?*(( int*) _temp144) ==  Cyc_Absyn_Param_b:
0){ _LL147: _temp146=(( struct Cyc_Absyn_Param_b_struct*) _temp144)->f1; goto
_LL57;} else{ goto _LL58;}} else{ goto _LL58;} _LL58: if(*(( int*) _temp24) == 
Cyc_Absyn_Var_e){ _LL149: _temp148=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp148 >  1u?*(( int*) _temp148) ==  Cyc_Absyn_Local_b:
0){ _LL151: _temp150=(( struct Cyc_Absyn_Local_b_struct*) _temp148)->f1; goto
_LL59;} else{ goto _LL60;}} else{ goto _LL60;} _LL60: if(*(( int*) _temp24) == 
Cyc_Absyn_Var_e){ _LL153: _temp152=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp152 >  1u?*(( int*) _temp152) ==  Cyc_Absyn_Pat_b:
0){ _LL155: _temp154=(( struct Cyc_Absyn_Pat_b_struct*) _temp152)->f1; goto
_LL61;} else{ goto _LL62;}} else{ goto _LL62;} _LL62: if(*(( int*) _temp24) == 
Cyc_Absyn_AssignOp_e){ _LL161: _temp160=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp24)->f1; goto _LL159; _LL159: _temp158=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp24)->f2; if( _temp158 ==  0){ goto _LL157;} else{ goto _LL64;} _LL157:
_temp156=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp24)->f3; goto _LL63;}
else{ goto _LL64;} _LL64: if(*(( int*) _temp24) ==  Cyc_Absyn_FnCall_e){ _LL165:
_temp164=(( struct Cyc_Absyn_FnCall_e_struct*) _temp24)->f1; goto _LL163; _LL163:
_temp162=(( struct Cyc_Absyn_FnCall_e_struct*) _temp24)->f2; goto _LL65;} else{
goto _LL66;} _LL66: if(*(( int*) _temp24) ==  Cyc_Absyn_AssignOp_e){ _LL169:
_temp168=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp24)->f1; goto _LL167;
_LL167: _temp166=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp24)->f3; goto
_LL67;} else{ goto _LL68;} _LL68: if(*(( int*) _temp24) ==  Cyc_Absyn_Primop_e){
_LL171: _temp170=(( struct Cyc_Absyn_Primop_e_struct*) _temp24)->f2; goto _LL69;}
else{ goto _LL70;} _LL70: if(*(( int*) _temp24) ==  Cyc_Absyn_Address_e){ _LL173:
_temp172=(( struct Cyc_Absyn_Address_e_struct*) _temp24)->f1; goto _LL71;} else{
goto _LL72;} _LL72: if(*(( int*) _temp24) ==  Cyc_Absyn_Subscript_e){ _LL177:
_temp176=(( struct Cyc_Absyn_Subscript_e_struct*) _temp24)->f1; goto _LL175;
_LL175: _temp174=(( struct Cyc_Absyn_Subscript_e_struct*) _temp24)->f2; goto
_LL73;} else{ goto _LL74;} _LL74: if(*(( int*) _temp24) ==  Cyc_Absyn_Deref_e){
_LL179: _temp178=(( struct Cyc_Absyn_Deref_e_struct*) _temp24)->f1; goto _LL75;}
else{ goto _LL76;} _LL76: if(*(( int*) _temp24) ==  Cyc_Absyn_StructArrow_e){
_LL183: _temp182=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp24)->f1; goto
_LL181; _LL181: _temp180=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp24)->f2;
goto _LL77;} else{ goto _LL78;} _LL78: if(*(( int*) _temp24) ==  Cyc_Absyn_StructMember_e){
_LL187: _temp186=(( struct Cyc_Absyn_StructMember_e_struct*) _temp24)->f1; goto
_LL185; _LL185: _temp184=(( struct Cyc_Absyn_StructMember_e_struct*) _temp24)->f2;
goto _LL79;} else{ goto _LL80;} _LL80: if(*(( int*) _temp24) ==  Cyc_Absyn_New_e){
_LL191: _temp190=(( struct Cyc_Absyn_New_e_struct*) _temp24)->f1; goto _LL189;
_LL189: _temp188=(( struct Cyc_Absyn_New_e_struct*) _temp24)->f2; goto _LL81;}
else{ goto _LL82;} _LL82: if(*(( int*) _temp24) ==  Cyc_Absyn_Malloc_e){ _LL195:
_temp194=(( struct Cyc_Absyn_Malloc_e_struct*) _temp24)->f1; goto _LL193; _LL193:
_temp192=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp24)->f2; goto _LL83;}
else{ goto _LL84;} _LL84: if(*(( int*) _temp24) ==  Cyc_Absyn_Tunion_e){ _LL197:
_temp196=(( struct Cyc_Absyn_Tunion_e_struct*) _temp24)->f3; goto _LL85;} else{
goto _LL86;} _LL86: if(*(( int*) _temp24) ==  Cyc_Absyn_Tuple_e){ _LL199:
_temp198=(( struct Cyc_Absyn_Tuple_e_struct*) _temp24)->f1; goto _LL87;} else{
goto _LL88;} _LL88: if(*(( int*) _temp24) ==  Cyc_Absyn_AnonStruct_e){ _LL201:
_temp200=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp24)->f2; goto _LL89;}
else{ goto _LL90;} _LL90: if(*(( int*) _temp24) ==  Cyc_Absyn_Struct_e){ _LL203:
_temp202=(( struct Cyc_Absyn_Struct_e_struct*) _temp24)->f3; goto _LL91;} else{
goto _LL92;} _LL92: if(*(( int*) _temp24) ==  Cyc_Absyn_Array_e){ _LL205:
_temp204=(( struct Cyc_Absyn_Array_e_struct*) _temp24)->f1; goto _LL93;} else{
goto _LL94;} _LL94: if(*(( int*) _temp24) ==  Cyc_Absyn_Increment_e){ _LL207:
_temp206=(( struct Cyc_Absyn_Increment_e_struct*) _temp24)->f1; goto _LL95;}
else{ goto _LL96;} _LL96: if(*(( int*) _temp24) ==  Cyc_Absyn_Throw_e){ _LL209:
_temp208=(( struct Cyc_Absyn_Throw_e_struct*) _temp24)->f1; goto _LL97;} else{
goto _LL98;} _LL98: if(*(( int*) _temp24) ==  Cyc_Absyn_Conditional_e){ _LL215:
_temp214=(( struct Cyc_Absyn_Conditional_e_struct*) _temp24)->f1; goto _LL213;
_LL213: _temp212=(( struct Cyc_Absyn_Conditional_e_struct*) _temp24)->f2; goto
_LL211; _LL211: _temp210=(( struct Cyc_Absyn_Conditional_e_struct*) _temp24)->f3;
goto _LL99;} else{ goto _LL100;} _LL100: if(*(( int*) _temp24) ==  Cyc_Absyn_StmtExp_e){
_LL217: _temp216=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp24)->f1; goto
_LL101;} else{ goto _LL102;} _LL102: if(*(( int*) _temp24) ==  Cyc_Absyn_SeqExp_e){
_LL221: _temp220=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp24)->f1; goto _LL219;
_LL219: _temp218=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp24)->f2; goto _LL103;}
else{ goto _LL104;} _LL104: if(*(( int*) _temp24) ==  Cyc_Absyn_Comprehension_e){
_LL227: _temp226=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp24)->f1; goto
_LL225; _LL225: _temp224=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp24)->f2;
goto _LL223; _LL223: _temp222=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp24)->f3; goto _LL105;} else{ goto _LL106;} _LL106: if(*(( int*) _temp24) == 
Cyc_Absyn_Var_e){ _LL229: _temp228=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if( _temp228 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL107;}
else{ goto _LL108;}} else{ goto _LL108;} _LL108: if(*(( int*) _temp24) ==  Cyc_Absyn_UnknownId_e){
goto _LL109;} else{ goto _LL110;} _LL110: if(*(( int*) _temp24) ==  Cyc_Absyn_UnknownCall_e){
goto _LL111;} else{ goto _LL112;} _LL112: if(*(( int*) _temp24) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL113;} else{ goto _LL114;} _LL114: if(*(( int*) _temp24) ==  Cyc_Absyn_CompoundLit_e){
goto _LL115;} else{ goto _LL116;} _LL116: if(*(( int*) _temp24) ==  Cyc_Absyn_Codegen_e){
goto _LL117;} else{ goto _LL118;} _LL118: if(*(( int*) _temp24) ==  Cyc_Absyn_Fill_e){
goto _LL119;} else{ goto _LL25;} _LL27: _temp122= _temp120; goto _LL29; _LL29:
_temp124= _temp122; goto _LL31; _LL31: return Cyc_NewControlFlow_abstract_exp(
env, _temp124); _LL33: goto _LL35; _LL35: return({ struct Cyc_NewControlFlow_AbsSyn
_temp230; _temp230.when_true_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp230.when_false_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp230.absop=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp230;}); _LL37: return({ struct Cyc_NewControlFlow_AbsSyn _temp231; _temp231.when_true_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp231.when_false_exp=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp231.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp231;}); _LL39: goto
_LL41; _LL41: goto _LL43; _LL43: goto _LL45; _LL45: goto _LL47; _LL47: goto
_LL49; _LL49: goto _LL51; _LL51: goto _LL53; _LL53: goto _LL55; _LL55: return({
struct Cyc_NewControlFlow_AbsSyn _temp232; _temp232.when_true_exp=( void*) Cyc_CfAbsexp_mkSkipAE();
_temp232.when_false_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp232.absop=( void*)
Cyc_CfAbsexp_mkUnknownOp(); _temp232;}); _LL57: _temp150= _temp146; goto _LL59;
_LL59: _temp154= _temp150; goto _LL61; _LL61: return({ struct Cyc_NewControlFlow_AbsSyn
_temp233; _temp233.when_true_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp233.when_false_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp233.absop=( void*) Cyc_CfAbsexp_mkLocalOp(
_temp154); _temp233;}); _LL63: { struct Cyc_NewControlFlow_AbsSyn _temp234= Cyc_NewControlFlow_abstract_exp(
env, _temp160); struct Cyc_NewControlFlow_AbsSyn _temp235= Cyc_NewControlFlow_abstract_exp(
env, _temp156); void* _temp236= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, Cyc_NewControlFlow_meet_absexp(
_temp234), Cyc_NewControlFlow_meet_absexp( _temp235)), Cyc_CfAbsexp_mkAssignAE((
void*) _temp234.absop,( void*) _temp235.absop)); return({ struct Cyc_NewControlFlow_AbsSyn
_temp237; _temp237.when_true_exp=( void*) _temp236; _temp237.when_false_exp=(
void*) _temp236; _temp237.absop=( void*)(( void*) _temp234.absop); _temp237;});}
_LL65: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp238=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp238->hd=( void*) _temp164; _temp238->tl= _temp162; _temp238;})); _LL67:
_temp170=({ struct Cyc_List_List* _temp239=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp239->hd=( void*) _temp168; _temp239->tl=({
struct Cyc_List_List* _temp240=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp240->hd=( void*) _temp166; _temp240->tl= 0;
_temp240;}); _temp239;}); goto _LL69; _LL69: return Cyc_NewControlFlow_use_exp_unordered(
env, _temp170); _LL71: { void* _temp241=( void*) _temp172->r; _LL243: if(*(( int*)
_temp241) ==  Cyc_Absyn_Struct_e){ goto _LL244;} else{ goto _LL245;} _LL245: if(*((
int*) _temp241) ==  Cyc_Absyn_Tuple_e){ goto _LL246;} else{ goto _LL247;} _LL247:
goto _LL248; _LL244: goto _LL246; _LL246: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp172->topt))->v);
return Cyc_NewControlFlow_abstract_malloc( env, e, 0,( struct Cyc_Absyn_Exp*)
_temp172); _LL248: { struct Cyc_NewControlFlow_AbsSyn _temp249= Cyc_NewControlFlow_abstract_exp(
env, _temp172); void* _temp250= Cyc_NewControlFlow_meet_absexp( _temp249);
return({ struct Cyc_NewControlFlow_AbsSyn _temp251; _temp251.when_true_exp=(
void*) _temp250; _temp251.when_false_exp=( void*) _temp250; _temp251.absop=(
void*) Cyc_CfAbsexp_mkAddressOp(( void*) _temp249.absop); _temp251;});} _LL242:;}
_LL73: { void* _temp252= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp176->topt))->v); _LL254: if(( unsigned int) _temp252 >  4u?*((
int*) _temp252) ==  Cyc_Absyn_TupleType: 0){ goto _LL255;} else{ goto _LL256;}
_LL256: goto _LL257; _LL255: { struct Cyc_NewControlFlow_AbsSyn _temp258= Cyc_NewControlFlow_abstract_exp(
env, _temp176); void* _temp259= Cyc_NewControlFlow_meet_absexp( _temp258);
struct _tagged_arr* _temp260= Cyc_Absyn_fieldname(( int) Cyc_Evexp_eval_const_uint_exp(
_temp174)); return({ struct Cyc_NewControlFlow_AbsSyn _temp261; _temp261.when_true_exp=(
void*) _temp259; _temp261.when_false_exp=( void*) _temp259; _temp261.absop=(
void*) Cyc_CfAbsexp_mkMemberOp(( void*) _temp258.absop, _temp260); _temp261;});}
_LL257: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp262=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp262->hd=( void*) _temp176; _temp262->tl=({ struct Cyc_List_List* _temp263=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp263->hd=(
void*) _temp174; _temp263->tl= 0; _temp263;}); _temp262;})); _LL253:;} _LL75: {
struct Cyc_NewControlFlow_AbsSyn _temp264= Cyc_NewControlFlow_abstract_exp( env,
_temp178); void* _temp265= Cyc_NewControlFlow_meet_absexp( _temp264); void*
absop;{ void* _temp266= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp178->topt))->v); struct Cyc_Absyn_PtrInfo _temp272; struct Cyc_Absyn_Conref*
_temp274; void* _temp276; _LL268: if(( unsigned int) _temp266 >  4u?*(( int*)
_temp266) ==  Cyc_Absyn_PointerType: 0){ _LL273: _temp272=(( struct Cyc_Absyn_PointerType_struct*)
_temp266)->f1; _LL277: _temp276=( void*) _temp272.elt_typ; goto _LL275; _LL275:
_temp274= _temp272.bounds; goto _LL269;} else{ goto _LL270;} _LL270: goto _LL271;
_LL269: if( Cyc_Tcutil_is_bound_one( _temp274)){ absop= Cyc_CfAbsexp_mkDerefOp((
void*) _temp264.absop);} else{ _temp265= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp265, Cyc_CfAbsexp_mkUseAE(( void*) _temp264.absop)); absop= Cyc_CfAbsexp_mkUnknownOp();}
goto _LL267; _LL271:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp278=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp278[ 0]=({ struct Cyc_Core_Impossible_struct _temp279; _temp279.tag= Cyc_Core_Impossible;
_temp279.f1= _tag_arr("NewControlFlow: bad type in Deref", sizeof( unsigned char),
34u); _temp279;}); _temp278;})); _LL267:;} return({ struct Cyc_NewControlFlow_AbsSyn
_temp280; _temp280.when_true_exp=( void*) _temp265; _temp280.when_false_exp=(
void*) _temp265; _temp280.absop=( void*) absop; _temp280;});} _LL77: { struct
Cyc_NewControlFlow_AbsSyn _temp281= Cyc_NewControlFlow_abstract_exp( env,
_temp182); void* _temp282= Cyc_NewControlFlow_meet_absexp( _temp281); void*
absop;{ void* _temp283= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp182->topt))->v); struct Cyc_Absyn_PtrInfo _temp289; struct Cyc_Absyn_Conref*
_temp291; void* _temp293; _LL285: if(( unsigned int) _temp283 >  4u?*(( int*)
_temp283) ==  Cyc_Absyn_PointerType: 0){ _LL290: _temp289=(( struct Cyc_Absyn_PointerType_struct*)
_temp283)->f1; _LL294: _temp293=( void*) _temp289.elt_typ; goto _LL292; _LL292:
_temp291= _temp289.bounds; goto _LL286;} else{ goto _LL287;} _LL287: goto _LL288;
_LL286: if( Cyc_Tcutil_is_bound_one( _temp291)){ void* _temp295= Cyc_Tcutil_compress(
_temp293); _LL297: if(( unsigned int) _temp295 >  4u?*(( int*) _temp295) ==  Cyc_Absyn_UnionType:
0){ goto _LL298;} else{ goto _LL299;} _LL299: if(( unsigned int) _temp295 >  4u?*((
int*) _temp295) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL300;} else{ goto _LL301;}
_LL301: goto _LL302; _LL298: goto _LL300; _LL300: absop= Cyc_CfAbsexp_mkDerefOp((
void*) _temp281.absop); goto _LL296; _LL302: absop= Cyc_CfAbsexp_mkMemberOp( Cyc_CfAbsexp_mkDerefOp((
void*) _temp281.absop), _temp180); _LL296:;} else{ _temp282= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp282, Cyc_CfAbsexp_mkUseAE(( void*) _temp281.absop));
absop= Cyc_CfAbsexp_mkUnknownOp();} goto _LL284; _LL288:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp303=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp303[ 0]=({ struct
Cyc_Core_Impossible_struct _temp304; _temp304.tag= Cyc_Core_Impossible; _temp304.f1=
_tag_arr("NewControlFlow: bad type in StructArrow", sizeof( unsigned char), 40u);
_temp304;}); _temp303;})); _LL284:;} return({ struct Cyc_NewControlFlow_AbsSyn
_temp305; _temp305.when_true_exp=( void*) _temp282; _temp305.when_false_exp=(
void*) _temp282; _temp305.absop=( void*) absop; _temp305;});} _LL79: { struct
Cyc_NewControlFlow_AbsSyn _temp306= Cyc_NewControlFlow_abstract_exp( env,
_temp186); void* _temp307= Cyc_NewControlFlow_meet_absexp( _temp306); void*
absop;{ void* _temp308= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp186->topt))->v); _LL310: if(( unsigned int) _temp308 >  4u?*((
int*) _temp308) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL311;} else{ goto _LL312;}
_LL312: if(( unsigned int) _temp308 >  4u?*(( int*) _temp308) ==  Cyc_Absyn_UnionType:
0){ goto _LL313;} else{ goto _LL314;} _LL314: goto _LL315; _LL311: goto _LL313;
_LL313: absop=( void*) _temp306.absop; goto _LL309; _LL315: absop= Cyc_CfAbsexp_mkMemberOp((
void*) _temp306.absop, _temp184); _LL309:;} return({ struct Cyc_NewControlFlow_AbsSyn
_temp316; _temp316.when_true_exp=( void*) _temp307; _temp316.when_false_exp=(
void*) _temp307; _temp316.absop=( void*) absop; _temp316;});} _LL81: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp188->topt))->v);
return Cyc_NewControlFlow_abstract_malloc( env, e, _temp190,( struct Cyc_Absyn_Exp*)
_temp188); _LL83: Cyc_NewControlFlow_add_malloc_root( env, e, _temp192); return
Cyc_NewControlFlow_abstract_malloc( env, e, _temp194, 0); _LL85: if( _temp196 == 
0){ return({ struct Cyc_NewControlFlow_AbsSyn _temp317; _temp317.when_true_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp317.when_false_exp=( void*) Cyc_CfAbsexp_mkSkipAE();
_temp317.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp317;});} _temp198=
_temp196; goto _LL87; _LL87: { struct Cyc_List_List* arg_aexps= 0; struct Cyc_Dict_Dict*
absop_dict=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int j= 0; for( 0;
_temp198 !=  0;( _temp198=(( struct Cyc_List_List*) _check_null( _temp198))->tl,
++ j)){ struct Cyc_NewControlFlow_AbsSyn _temp318= Cyc_NewControlFlow_abstract_exp(
env,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp198))->hd);
arg_aexps=({ struct Cyc_List_List* _temp319=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp319->hd=( void*) Cyc_NewControlFlow_meet_absexp(
_temp318); _temp319->tl= arg_aexps; _temp319;}); absop_dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)(
absop_dict, Cyc_Absyn_fieldname( j),( void*) _temp318.absop);}}{ void* _temp320=
Cyc_CfAbsexp_mkGroupAE_l(( void*) Cyc_CfAbsexp_UnorderedG, arg_aexps); return({
struct Cyc_NewControlFlow_AbsSyn _temp321; _temp321.when_true_exp=( void*)
_temp320; _temp321.when_false_exp=( void*) _temp320; _temp321.absop=( void*) Cyc_CfAbsexp_mkDictOp(
absop_dict); _temp321;});}} _LL89: _temp202= _temp200; goto _LL91; _LL91: {
struct Cyc_List_List* arg_aexps= 0; struct Cyc_Dict_Dict* absop_dict=(( struct
Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp); for( 0; _temp202 !=  0; _temp202=(( struct Cyc_List_List*)
_check_null( _temp202))->tl){ struct Cyc_NewControlFlow_AbsSyn _temp322= Cyc_NewControlFlow_abstract_exp(
env,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp202))->hd)).f2);
arg_aexps=({ struct Cyc_List_List* _temp323=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp323->hd=( void*) Cyc_NewControlFlow_meet_absexp(
_temp322); _temp323->tl= arg_aexps; _temp323;});{ struct Cyc_List_List* ds=(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp202))->hd)).f1; for(
0; ds !=  0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void* _temp324=(
void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct _tagged_arr*
_temp330; _LL326: if(*(( int*) _temp324) ==  Cyc_Absyn_ArrayElement){ goto
_LL327;} else{ goto _LL328;} _LL328: if(*(( int*) _temp324) ==  Cyc_Absyn_FieldName){
_LL331: _temp330=(( struct Cyc_Absyn_FieldName_struct*) _temp324)->f1; goto
_LL329;} else{ goto _LL325;} _LL327:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp332=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp332[ 0]=({ struct Cyc_Core_Impossible_struct _temp333; _temp333.tag= Cyc_Core_Impossible;
_temp333.f1= _tag_arr("bad struct designator", sizeof( unsigned char), 22u);
_temp333;}); _temp332;})); _LL329: absop_dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)(
absop_dict, _temp330,( void*) _temp322.absop); _LL325:;}}}{ void* _temp334= Cyc_CfAbsexp_mkGroupAE_l((
void*) Cyc_CfAbsexp_UnorderedG, arg_aexps); return({ struct Cyc_NewControlFlow_AbsSyn
_temp335; _temp335.when_true_exp=( void*) _temp334; _temp335.when_false_exp=(
void*) _temp334; _temp335.absop=( void*) Cyc_CfAbsexp_mkDictOp( absop_dict);
_temp335;});}} _LL93: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple4*)) Cyc_Core_snd,
_temp204)); _LL95: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct
Cyc_List_List* _temp336=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp336->hd=( void*) _temp206; _temp336->tl= 0; _temp336;})); _LL97: { void*
_temp337= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_use_exp(
env, _temp208), Cyc_CfAbsexp_mkBottomAE()); return({ struct Cyc_NewControlFlow_AbsSyn
_temp338; _temp338.when_true_exp=( void*) _temp337; _temp338.when_false_exp=(
void*) _temp337; _temp338.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp338;});}
_LL99: { struct Cyc_NewControlFlow_AbsSyn _temp339= Cyc_NewControlFlow_abstract_exp(
env, _temp214); struct Cyc_NewControlFlow_AbsSyn _temp340= Cyc_NewControlFlow_abstract_exp(
env, _temp212); struct Cyc_NewControlFlow_AbsSyn _temp341= Cyc_NewControlFlow_abstract_exp(
env, _temp210); void* _temp342= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp339.when_true_exp, Cyc_CfAbsexp_mkUseAE(( void*) _temp339.absop));
void* _temp343= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp339.when_false_exp, Cyc_CfAbsexp_mkUseAE(( void*) _temp339.absop)); void*
_temp344= Cyc_CfAbsexp_mkUseAE(( void*) _temp340.absop); void* _temp345= Cyc_CfAbsexp_mkUseAE((
void*) _temp341.absop); void* _temp346= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp342, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp340.when_true_exp, _temp344)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp343, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp341.when_true_exp, _temp345))); void* _temp347= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp342, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp340.when_false_exp, _temp344)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp343, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp341.when_false_exp, _temp345))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp348; _temp348.when_true_exp=( void*) _temp346; _temp348.when_false_exp=(
void*) _temp347; _temp348.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp348;});}
_LL101: Cyc_NewControlFlow_cf_prepass( env, _temp216);{ struct Cyc_Absyn_Stmt*
_temp349= _temp216; while( 1) { void* _temp350=( void*) _temp349->r; struct Cyc_Absyn_Stmt*
_temp360; struct Cyc_Absyn_Stmt* _temp362; struct Cyc_Absyn_Stmt* _temp364;
struct Cyc_Absyn_Decl* _temp366; struct Cyc_Absyn_Exp* _temp368; _LL352: if((
unsigned int) _temp350 >  1u?*(( int*) _temp350) ==  Cyc_Absyn_Seq_s: 0){ _LL363:
_temp362=(( struct Cyc_Absyn_Seq_s_struct*) _temp350)->f1; goto _LL361; _LL361:
_temp360=(( struct Cyc_Absyn_Seq_s_struct*) _temp350)->f2; goto _LL353;} else{
goto _LL354;} _LL354: if(( unsigned int) _temp350 >  1u?*(( int*) _temp350) == 
Cyc_Absyn_Decl_s: 0){ _LL367: _temp366=(( struct Cyc_Absyn_Decl_s_struct*)
_temp350)->f1; goto _LL365; _LL365: _temp364=(( struct Cyc_Absyn_Decl_s_struct*)
_temp350)->f2; goto _LL355;} else{ goto _LL356;} _LL356: if(( unsigned int)
_temp350 >  1u?*(( int*) _temp350) ==  Cyc_Absyn_Exp_s: 0){ _LL369: _temp368=((
struct Cyc_Absyn_Exp_s_struct*) _temp350)->f1; goto _LL357;} else{ goto _LL358;}
_LL358: goto _LL359; _LL353: _temp349= _temp360; continue; _LL355: _temp349=
_temp364; continue; _LL357: { void* _temp370=( void*)( Cyc_NewControlFlow_abstract_exp(
env, _temp368)).absop; void* _temp371= Cyc_CfAbsexp_mkStmtAE( _temp216); return({
struct Cyc_NewControlFlow_AbsSyn _temp372; _temp372.when_true_exp=( void*)
_temp371; _temp372.when_false_exp=( void*) _temp371; _temp372.absop=( void*)
_temp370; _temp372;});} _LL359:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp373=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp373[ 0]=({ struct Cyc_Core_Impossible_struct _temp374; _temp374.tag= Cyc_Core_Impossible;
_temp374.f1= _tag_arr("abstract_exp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp374;}); _temp373;})); _LL351:;}} _LL103: { struct Cyc_NewControlFlow_AbsSyn
_temp375= Cyc_NewControlFlow_abstract_exp( env, _temp220); struct Cyc_NewControlFlow_AbsSyn
_temp376= Cyc_NewControlFlow_abstract_exp( env, _temp218); void* _temp377= Cyc_NewControlFlow_meet_absexp(
_temp375); return({ struct Cyc_NewControlFlow_AbsSyn _temp378; _temp378.when_true_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp377,( void*)
_temp376.when_true_exp); _temp378.when_false_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp377,( void*) _temp376.when_false_exp);
_temp378.absop=( void*)(( void*) _temp376.absop); _temp378;});} _LL105: Cyc_NewControlFlow_add_var_root(
env, _temp226);{ struct Cyc_NewControlFlow_AbsSyn _temp379= Cyc_NewControlFlow_abstract_exp(
env, _temp224); struct Cyc_NewControlFlow_AbsSyn _temp380= Cyc_NewControlFlow_abstract_exp(
env, _temp222); void* _temp381= Cyc_CfAbsexp_mkComprehensionAE( _temp226,( void*)
_temp379.when_true_exp,( void*) _temp379.when_false_exp,( void*) _temp379.absop,
Cyc_NewControlFlow_meet_absexp( _temp380),( void*) _temp380.absop); return({
struct Cyc_NewControlFlow_AbsSyn _temp382; _temp382.when_true_exp=( void*)
_temp381; _temp382.when_false_exp=( void*) _temp381; _temp382.absop=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp382;});} _LL107: goto _LL109; _LL109: goto _LL111; _LL111: goto _LL113;
_LL113: goto _LL115; _LL115: goto _LL117; _LL117: goto _LL119; _LL119:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp383=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp383[ 0]=({ struct
Cyc_Core_Impossible_struct _temp384; _temp384.tag= Cyc_Core_Impossible; _temp384.f1=
_tag_arr("abstract_exp, unexpected exp form", sizeof( unsigned char), 34u);
_temp384;}); _temp383;})); _LL25:;} static struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
_temp385= Cyc_NewControlFlow_abstract_exp( env, e); void* _temp386= Cyc_CfAbsexp_mkUseAE((
void*) _temp385.absop); if(( void*) _temp385.when_true_exp == ( void*) _temp385.when_false_exp){
void* _temp387= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp385.when_true_exp, _temp386); return({ struct _tuple0 _temp388; _temp388.f1=
_temp387; _temp388.f2= _temp387; _temp388;});} return({ struct _tuple0 _temp389;
_temp389.f1= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp385.when_true_exp, _temp386); _temp389.f2= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG,( void*) _temp385.when_false_exp, _temp386); _temp389;});}
struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs( struct Cyc_NewControlFlow_AbsEnv*
env, struct Cyc_List_List* scs){ struct Cyc_List_List* _temp390= 0; for( 0; scs
!=  0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause
_temp393; struct Cyc_Absyn_Stmt* _temp394; struct Cyc_Absyn_Exp* _temp396;
struct Cyc_Core_Opt* _temp398; struct Cyc_Absyn_Switch_clause* _temp391=( struct
Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd;
_temp393=* _temp391; _LL399: _temp398= _temp393.pat_vars; goto _LL397; _LL397:
_temp396= _temp393.where_clause; goto _LL395; _LL395: _temp394= _temp393.body;
goto _LL392; _LL392: if( _temp398 ==  0){( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp400=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp400[ 0]=({ struct Cyc_Core_Impossible_struct _temp401; _temp401.tag= Cyc_Core_Impossible;
_temp401.f1= _tag_arr("switch clause vds not set", sizeof( unsigned char), 26u);
_temp401;}); _temp400;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp398))->v; for( 0; vds !=  0; vds=((
struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
if( _temp396 !=  0){ void* _temp404; void* _temp406; struct _tuple0 _temp402=
Cyc_NewControlFlow_abstract_guard( env,( struct Cyc_Absyn_Exp*) _check_null(
_temp396)); _LL407: _temp406= _temp402.f1; goto _LL405; _LL405: _temp404=
_temp402.f2; goto _LL403; _LL403: _temp390=({ struct Cyc_List_List* _temp408=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp408->hd=(
void*) _temp406; _temp408->tl=({ struct Cyc_List_List* _temp409=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp409->hd=( void*) _temp404;
_temp409->tl= _temp390; _temp409;}); _temp408;});} Cyc_NewControlFlow_cf_prepass(
env, _temp394);} return Cyc_List_imp_rev( _temp390);} void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Stmt* s){ void* _temp410=(
void*) s->r; struct Cyc_Absyn_Exp* _temp462; struct Cyc_Absyn_Stmt* _temp464;
struct Cyc_Absyn_Stmt* _temp466; struct Cyc_Absyn_Exp* _temp468; struct Cyc_Absyn_Exp*
_temp470; struct Cyc_Absyn_Stmt* _temp472; struct Cyc_Absyn_Stmt* _temp474;
struct Cyc_Absyn_Exp* _temp476; struct _tuple3 _temp478; struct Cyc_Absyn_Stmt*
_temp480; struct Cyc_Absyn_Exp* _temp482; struct Cyc_Absyn_Stmt* _temp484;
struct Cyc_Absyn_Stmt* _temp486; struct _tuple3 _temp488; struct Cyc_Absyn_Stmt*
_temp490; struct Cyc_Absyn_Exp* _temp492; struct Cyc_Absyn_Stmt* _temp494;
struct Cyc_Absyn_Stmt* _temp496; struct Cyc_Absyn_Stmt* _temp498; struct _tuple3
_temp500; struct Cyc_Absyn_Stmt* _temp502; struct Cyc_Absyn_Exp* _temp504;
struct _tuple3 _temp506; struct Cyc_Absyn_Stmt* _temp508; struct Cyc_Absyn_Exp*
_temp510; struct Cyc_Absyn_Exp* _temp512; struct Cyc_List_List* _temp514; struct
Cyc_Absyn_Exp* _temp516; struct Cyc_List_List* _temp518; struct Cyc_Absyn_Stmt*
_temp520; struct Cyc_Absyn_Decl* _temp522; struct Cyc_Absyn_Decl _temp524; void*
_temp525; struct Cyc_Absyn_Vardecl* _temp527; struct Cyc_Absyn_Stmt* _temp529;
struct Cyc_Absyn_Decl* _temp531; struct Cyc_Absyn_Decl _temp533; void* _temp534;
struct Cyc_Absyn_Exp* _temp536; struct Cyc_Core_Opt* _temp538; struct Cyc_Core_Opt
_temp540; struct Cyc_List_List* _temp541; struct Cyc_Absyn_Stmt* _temp543;
struct Cyc_Absyn_Decl* _temp545; struct Cyc_Absyn_Decl _temp547; void* _temp548;
struct Cyc_List_List* _temp550; struct Cyc_Absyn_Stmt* _temp552; struct Cyc_List_List*
_temp554; struct Cyc_Absyn_Stmt* _temp556; struct Cyc_Absyn_Stmt* _temp558;
struct Cyc_Absyn_Vardecl* _temp560; struct Cyc_Absyn_Tvar* _temp562; _LL412: if(
_temp410 == ( void*) Cyc_Absyn_Skip_s){ goto _LL413;} else{ goto _LL414;} _LL414:
if(( unsigned int) _temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Exp_s: 0){
_LL463: _temp462=(( struct Cyc_Absyn_Exp_s_struct*) _temp410)->f1; goto _LL415;}
else{ goto _LL416;} _LL416: if(( unsigned int) _temp410 >  1u?*(( int*) _temp410)
==  Cyc_Absyn_Seq_s: 0){ _LL467: _temp466=(( struct Cyc_Absyn_Seq_s_struct*)
_temp410)->f1; goto _LL465; _LL465: _temp464=(( struct Cyc_Absyn_Seq_s_struct*)
_temp410)->f2; goto _LL417;} else{ goto _LL418;} _LL418: if(( unsigned int)
_temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Return_s: 0){ _LL469: _temp468=((
struct Cyc_Absyn_Return_s_struct*) _temp410)->f1; if( _temp468 ==  0){ goto
_LL419;} else{ goto _LL420;}} else{ goto _LL420;} _LL420: if(( unsigned int)
_temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Return_s: 0){ _LL471: _temp470=((
struct Cyc_Absyn_Return_s_struct*) _temp410)->f1; goto _LL421;} else{ goto
_LL422;} _LL422: if(( unsigned int) _temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_IfThenElse_s:
0){ _LL477: _temp476=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp410)->f1;
goto _LL475; _LL475: _temp474=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp410)->f2;
goto _LL473; _LL473: _temp472=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp410)->f3;
goto _LL423;} else{ goto _LL424;} _LL424: if(( unsigned int) _temp410 >  1u?*((
int*) _temp410) ==  Cyc_Absyn_Do_s: 0){ _LL485: _temp484=(( struct Cyc_Absyn_Do_s_struct*)
_temp410)->f1; goto _LL479; _LL479: _temp478=(( struct Cyc_Absyn_Do_s_struct*)
_temp410)->f2; _LL483: _temp482= _temp478.f1; goto _LL481; _LL481: _temp480=
_temp478.f2; goto _LL425;} else{ goto _LL426;} _LL426: if(( unsigned int)
_temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_While_s: 0){ _LL489: _temp488=((
struct Cyc_Absyn_While_s_struct*) _temp410)->f1; _LL493: _temp492= _temp488.f1;
goto _LL491; _LL491: _temp490= _temp488.f2; goto _LL487; _LL487: _temp486=((
struct Cyc_Absyn_While_s_struct*) _temp410)->f2; goto _LL427;} else{ goto _LL428;}
_LL428: if(( unsigned int) _temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Break_s:
0){ goto _LL429;} else{ goto _LL430;} _LL430: if(( unsigned int) _temp410 >  1u?*((
int*) _temp410) ==  Cyc_Absyn_Continue_s: 0){ goto _LL431;} else{ goto _LL432;}
_LL432: if(( unsigned int) _temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Goto_s:
0){ _LL495: _temp494=(( struct Cyc_Absyn_Goto_s_struct*) _temp410)->f2; if(
_temp494 ==  0){ goto _LL433;} else{ goto _LL434;}} else{ goto _LL434;} _LL434:
if(( unsigned int) _temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Goto_s: 0){
_LL497: _temp496=(( struct Cyc_Absyn_Goto_s_struct*) _temp410)->f2; goto _LL435;}
else{ goto _LL436;} _LL436: if(( unsigned int) _temp410 >  1u?*(( int*) _temp410)
==  Cyc_Absyn_For_s: 0){ _LL513: _temp512=(( struct Cyc_Absyn_For_s_struct*)
_temp410)->f1; goto _LL507; _LL507: _temp506=(( struct Cyc_Absyn_For_s_struct*)
_temp410)->f2; _LL511: _temp510= _temp506.f1; goto _LL509; _LL509: _temp508=
_temp506.f2; goto _LL501; _LL501: _temp500=(( struct Cyc_Absyn_For_s_struct*)
_temp410)->f3; _LL505: _temp504= _temp500.f1; goto _LL503; _LL503: _temp502=
_temp500.f2; goto _LL499; _LL499: _temp498=(( struct Cyc_Absyn_For_s_struct*)
_temp410)->f4; goto _LL437;} else{ goto _LL438;} _LL438: if(( unsigned int)
_temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Switch_s: 0){ _LL517: _temp516=((
struct Cyc_Absyn_Switch_s_struct*) _temp410)->f1; goto _LL515; _LL515: _temp514=((
struct Cyc_Absyn_Switch_s_struct*) _temp410)->f2; goto _LL439;} else{ goto
_LL440;} _LL440: if(( unsigned int) _temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Fallthru_s:
0){ _LL519: _temp518=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp410)->f1; goto
_LL441;} else{ goto _LL442;} _LL442: if(( unsigned int) _temp410 >  1u?*(( int*)
_temp410) ==  Cyc_Absyn_Decl_s: 0){ _LL523: _temp522=(( struct Cyc_Absyn_Decl_s_struct*)
_temp410)->f1; _temp524=* _temp522; _LL526: _temp525=( void*) _temp524.r; if(*((
int*) _temp525) ==  Cyc_Absyn_Var_d){ _LL528: _temp527=(( struct Cyc_Absyn_Var_d_struct*)
_temp525)->f1; goto _LL521;} else{ goto _LL444;} _LL521: _temp520=(( struct Cyc_Absyn_Decl_s_struct*)
_temp410)->f2; goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int)
_temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Decl_s: 0){ _LL532: _temp531=((
struct Cyc_Absyn_Decl_s_struct*) _temp410)->f1; _temp533=* _temp531; _LL535:
_temp534=( void*) _temp533.r; if(*(( int*) _temp534) ==  Cyc_Absyn_Let_d){
_LL539: _temp538=(( struct Cyc_Absyn_Let_d_struct*) _temp534)->f2; if( _temp538
==  0){ goto _LL446;} else{ _temp540=* _temp538; _LL542: _temp541=( struct Cyc_List_List*)
_temp540.v; goto _LL537;} _LL537: _temp536=(( struct Cyc_Absyn_Let_d_struct*)
_temp534)->f4; goto _LL530;} else{ goto _LL446;} _LL530: _temp529=(( struct Cyc_Absyn_Decl_s_struct*)
_temp410)->f2; goto _LL445;} else{ goto _LL446;} _LL446: if(( unsigned int)
_temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Decl_s: 0){ _LL546: _temp545=((
struct Cyc_Absyn_Decl_s_struct*) _temp410)->f1; _temp547=* _temp545; _LL549:
_temp548=( void*) _temp547.r; if(*(( int*) _temp548) ==  Cyc_Absyn_Letv_d){
_LL551: _temp550=(( struct Cyc_Absyn_Letv_d_struct*) _temp548)->f1; goto _LL544;}
else{ goto _LL448;} _LL544: _temp543=(( struct Cyc_Absyn_Decl_s_struct*)
_temp410)->f2; goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int)
_temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Label_s: 0){ _LL553: _temp552=((
struct Cyc_Absyn_Label_s_struct*) _temp410)->f2; goto _LL449;} else{ goto _LL450;}
_LL450: if(( unsigned int) _temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_TryCatch_s:
0){ _LL557: _temp556=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp410)->f1; goto
_LL555; _LL555: _temp554=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp410)->f2;
goto _LL451;} else{ goto _LL452;} _LL452: if(( unsigned int) _temp410 >  1u?*((
int*) _temp410) ==  Cyc_Absyn_Region_s: 0){ _LL563: _temp562=(( struct Cyc_Absyn_Region_s_struct*)
_temp410)->f1; goto _LL561; _LL561: _temp560=(( struct Cyc_Absyn_Region_s_struct*)
_temp410)->f2; goto _LL559; _LL559: _temp558=(( struct Cyc_Absyn_Region_s_struct*)
_temp410)->f3; goto _LL453;} else{ goto _LL454;} _LL454: if(( unsigned int)
_temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Decl_s: 0){ goto _LL455;} else{
goto _LL456;} _LL456: if(( unsigned int) _temp410 >  1u?*(( int*) _temp410) == 
Cyc_Absyn_SwitchC_s: 0){ goto _LL457;} else{ goto _LL458;} _LL458: if((
unsigned int) _temp410 >  1u?*(( int*) _temp410) ==  Cyc_Absyn_Cut_s: 0){ goto
_LL459;} else{ goto _LL460;} _LL460: if(( unsigned int) _temp410 >  1u?*(( int*)
_temp410) ==  Cyc_Absyn_Splice_s: 0){ goto _LL461;} else{ goto _LL411;} _LL413:
return; _LL415: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp564=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp564->hd=( void*) Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env, _temp462)); _temp564->tl= 0; _temp564;})); return; _LL417: Cyc_NewControlFlow_cf_prepass(
env, _temp466); Cyc_NewControlFlow_cf_prepass( env, _temp464); return; _LL419:
return; _LL421: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp565=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp565->hd=( void*) Cyc_NewControlFlow_use_exp( env,( struct Cyc_Absyn_Exp*)
_check_null( _temp470)); _temp565->tl= 0; _temp565;})); return; _LL423: { void*
_temp568; void* _temp570; struct _tuple0 _temp566= Cyc_NewControlFlow_abstract_guard(
env, _temp476); _LL571: _temp570= _temp566.f1; goto _LL569; _LL569: _temp568=
_temp566.f2; goto _LL567; _LL567: Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp572=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp572->hd=( void*) _temp570; _temp572->tl=({ struct Cyc_List_List* _temp573=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp573->hd=(
void*) _temp568; _temp573->tl= 0; _temp573;}); _temp572;})); Cyc_NewControlFlow_cf_prepass(
env, _temp474); Cyc_NewControlFlow_cf_prepass( env, _temp472); return;} _LL425:
_temp492= _temp482; _temp490= _temp480; _temp486= _temp484; goto _LL427; _LL427: {
void* _temp576; void* _temp578; struct _tuple0 _temp574= Cyc_NewControlFlow_abstract_guard(
env, _temp492); _LL579: _temp578= _temp574.f1; goto _LL577; _LL577: _temp576=
_temp574.f2; goto _LL575; _LL575: Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp580=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp580->hd=( void*) _temp578; _temp580->tl=({ struct Cyc_List_List* _temp581=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp581->hd=(
void*) _temp576; _temp581->tl= 0; _temp581;}); _temp580;})); Cyc_NewControlFlow_cf_prepass(
env, _temp486); return;} _LL429: goto _LL431; _LL431: return; _LL433:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp582=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp582[ 0]=({ struct
Cyc_Core_Impossible_struct _temp583; _temp583.tag= Cyc_Core_Impossible; _temp583.f1=
_tag_arr("cf_prepass: goto w/o dest", sizeof( unsigned char), 26u); _temp583;});
_temp582;})); _LL435: { struct Cyc_Absyn_Stmt* _temp584=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* _temp585=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp496)))->encloser; while( _temp585 != 
_temp584) { struct Cyc_Absyn_Stmt* _temp586=( Cyc_NewControlFlow_get_stmt_annot(
_temp584))->encloser; if( _temp586 ==  _temp584){({ void* _temp587[ 0u]={}; Cyc_Tcutil_terr(
s->loc, _tag_arr("goto enters local scope or exception handler", sizeof(
unsigned char), 45u), _tag_arr( _temp587, sizeof( void*), 0u));}); break;}
_temp584= _temp586;} return;} _LL437: { void* _temp588= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, _temp512)); void* _temp591; void* _temp593;
struct _tuple0 _temp589= Cyc_NewControlFlow_abstract_guard( env, _temp510);
_LL594: _temp593= _temp589.f1; goto _LL592; _LL592: _temp591= _temp589.f2; goto
_LL590; _LL590: { void* _temp595= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env, _temp504)); Cyc_NewControlFlow_cf_set_absexps( s,({ void* _temp596[ 4u];
_temp596[ 3u]= _temp595; _temp596[ 2u]= _temp591; _temp596[ 1u]= _temp593;
_temp596[ 0u]= _temp588; Cyc_List_list( _tag_arr( _temp596, sizeof( void*), 4u));}));
Cyc_NewControlFlow_cf_prepass( env, _temp498); return;}} _LL439: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp597=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp597->hd=( void*) Cyc_NewControlFlow_use_exp( env,
_temp516); _temp597->tl= Cyc_NewControlFlow_cf_prepass_scs( env, _temp514);
_temp597;})); return; _LL441: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp598=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp598->hd=( void*) Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp518)); _temp598->tl= 0; _temp598;})); return; _LL443: Cyc_NewControlFlow_add_var_root(
env, _temp527); if( _temp527->initializer !=  0){ struct Cyc_NewControlFlow_AbsSyn
_temp599= Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*)
_check_null( _temp527->initializer)); void* _temp600= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_meet_absexp( _temp599), Cyc_CfAbsexp_mkAssignAE(
Cyc_CfAbsexp_mkLocalOp( _temp527),( void*) _temp599.absop)); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp601=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp601->hd=( void*) _temp600; _temp601->tl= 0;
_temp601;}));} Cyc_NewControlFlow_cf_prepass( env, _temp520); return; _LL445:
for( 0; _temp541 !=  0; _temp541=(( struct Cyc_List_List*) _check_null( _temp541))->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp541))->hd);} Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp602=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp602->hd=( void*) Cyc_NewControlFlow_use_exp( env, _temp536); _temp602->tl=
0; _temp602;})); Cyc_NewControlFlow_cf_prepass( env, _temp529); return; _LL447:
for( 0; _temp550 !=  0; _temp550=(( struct Cyc_List_List*) _check_null( _temp550))->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp550))->hd);} Cyc_NewControlFlow_cf_prepass( env, _temp543);
return; _LL449: Cyc_NewControlFlow_cf_prepass( env, _temp552); return; _LL451:
Cyc_NewControlFlow_cf_prepass( env, _temp556); Cyc_NewControlFlow_cf_set_absexps(
s, Cyc_NewControlFlow_cf_prepass_scs( env, _temp554)); return; _LL453: Cyc_NewControlFlow_add_var_root(
env, _temp560); Cyc_NewControlFlow_cf_prepass( env, _temp558); return; _LL455:
goto _LL457; _LL457: goto _LL459; _LL459: goto _LL461; _LL461:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp603=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp603[ 0]=({ struct
Cyc_Core_Impossible_struct _temp604; _temp604.tag= Cyc_Core_Impossible; _temp604.f1=
_tag_arr("cf_prepass: bad stmt form", sizeof( unsigned char), 26u); _temp604;});
_temp603;})); _LL411:;} static int Cyc_NewControlFlow_iterate_cf_check= 0;
static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
env->all_changed, new_flow,( void*) env->tryflow));}} static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp605= Cyc_NewControlFlow_get_stmt_annot(
s); void* _temp606= Cyc_CfFlowInfo_join_flow( env->all_changed, flow,( void*)
_temp605->flow); if( ! Cyc_CfFlowInfo_flow_lessthan_approx( _temp606,( void*)
_temp605->flow)){( void*)( _temp605->flow=( void*) _temp606); if( _temp605->visited
==  Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check= 1;}}}
static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv* env, void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp607= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( _temp607->flow=( void*) Cyc_CfFlowInfo_join_flow( env->all_changed,
in_flow,( void*) _temp607->flow)); ++ _temp607->visited; _temp607->visited == 
Cyc_NewControlFlow_iteration_num? 0:(((( int(*)( struct _tagged_arr assertion,
struct _tagged_arr file, unsigned int line)) Cyc_Std___assert_fail)( _tag_arr("annot->visited == iteration_num",
sizeof( unsigned char), 32u), _tag_arr("new_control_flow.cyc", sizeof(
unsigned char), 21u), 645), 0)); return _temp607;} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp608= flow; struct Cyc_Dict_Dict* _temp614; _LL610: if( _temp608 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL611;} else{ goto _LL612;} _LL612: if((
unsigned int) _temp608 >  1u?*(( int*) _temp608) ==  Cyc_CfFlowInfo_InitsFL: 0){
_LL615: _temp614=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp608)->f1; goto
_LL613;} else{ goto _LL609;} _LL611: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL613: for( 0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp616=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp618=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp618[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp619; _temp619.tag= Cyc_CfFlowInfo_VarRoot;
_temp619.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp619;}); _temp618;}); void* _temp617= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL, Cyc_Dict_lookup( env->roots,( void*) _temp616));
_temp614= Cyc_Dict_insert( _temp614,( void*) _temp616, _temp617);} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp620=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp620[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp621; _temp621.tag= Cyc_CfFlowInfo_InitsFL;
_temp621.f1= _temp614; _temp621;}); _temp620;}); _LL609:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp622= flow; struct Cyc_Dict_Dict* _temp628; _LL624: if( _temp622 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL625;} else{ goto _LL626;} _LL626: if((
unsigned int) _temp622 >  1u?*(( int*) _temp622) ==  Cyc_CfFlowInfo_InitsFL: 0){
_LL629: _temp628=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp622)->f1; goto
_LL627;} else{ goto _LL623;} _LL625: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL627: for( 0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp630=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp631=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp631[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp632; _temp632.tag= Cyc_CfFlowInfo_VarRoot;
_temp632.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp632;}); _temp631;}); _temp628= Cyc_Dict_insert( _temp628,( void*)
_temp630, Cyc_Dict_lookup( env->roots,( void*) _temp630));} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp633=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp633[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp634; _temp634.tag= Cyc_CfFlowInfo_InitsFL;
_temp634.f1= _temp628; _temp634;}); _temp633;}); _LL623:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)((
struct Cyc_List_List*) _check_null( aes))->hd == ( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){ void*
_temp635= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); return({ struct _tuple0 _temp636; _temp636.f1=
_temp635; _temp636.f2= _temp635; _temp636;});} return({ struct _tuple0 _temp637;
_temp637.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); _temp637.f2= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->hd, in_flow); _temp637;});} static void* Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs !=  0; scs=(( struct Cyc_List_List*)
_check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause _temp640; struct Cyc_Absyn_Stmt*
_temp641; struct Cyc_Absyn_Exp* _temp643; struct Cyc_Core_Opt* _temp645; struct
Cyc_Absyn_Switch_clause* _temp638=( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( scs))->hd; _temp640=* _temp638; _LL646: _temp645=
_temp640.pat_vars; goto _LL644; _LL644: _temp643= _temp640.where_clause; goto
_LL642; _LL642: _temp641= _temp640.body; goto _LL639; _LL639: { void* _temp647=
Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp645))->v); _temp647= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp647,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp645))->v);{ void* body_outflow; if( _temp643 !=  0){ void* _temp650; void*
_temp652; struct _tuple0 _temp648= Cyc_NewControlFlow_cf_evalguard( env,((
struct Cyc_Absyn_Exp*) _check_null( _temp643))->loc, aes, _temp647); _LL653:
_temp652= _temp648.f1; goto _LL651; _LL651: _temp650= _temp648.f2; goto _LL649;
_LL649: aes=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->tl; in_flow= _temp650; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp641, _temp652);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp641, _temp647);}{ void* _temp654= body_outflow; _LL656: if( _temp654
== ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL657;} else{ goto _LL658;} _LL658:
goto _LL659; _LL657: goto _LL655; _LL659: if((( struct Cyc_List_List*)
_check_null( scs))->tl ==  0){ return body_outflow;} else{ if(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( scs))->tl))->hd)->pat_vars))->v
!=  0){({ void* _temp660[ 0u]={}; Cyc_Tcutil_terr( _temp641->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp660, sizeof( void*), 0u));});}
else{({ void* _temp661[ 0u]={}; Cyc_Tcutil_warn( _temp641->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp661, sizeof( void*), 0u));});} Cyc_NewControlFlow_update_flow(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( scs))->tl))->hd)->body, body_outflow);} goto
_LL655; _LL655:;}}}} return( void*) Cyc_CfFlowInfo_BottomFL;} void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* in_flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp662= Cyc_NewControlFlow_pre_stmt_check(
env, in_flow, s); in_flow=( void*) _temp662->flow;{ struct Cyc_List_List*
_temp663= _temp662->absexps; void* _temp664=( void*) s->r; struct Cyc_Absyn_Exp*
_temp710; struct Cyc_Absyn_Exp* _temp712; struct Cyc_Absyn_Exp* _temp714; struct
Cyc_Absyn_Stmt* _temp716; struct Cyc_Absyn_Stmt* _temp718; struct Cyc_Absyn_Stmt*
_temp720; struct Cyc_Absyn_Stmt* _temp722; struct Cyc_Absyn_Exp* _temp724;
struct Cyc_Absyn_Stmt* _temp726; struct _tuple3 _temp728; struct Cyc_Absyn_Stmt*
_temp730; struct Cyc_Absyn_Exp* _temp732; struct _tuple3 _temp734; struct Cyc_Absyn_Stmt*
_temp736; struct Cyc_Absyn_Exp* _temp738; struct Cyc_Absyn_Stmt* _temp740;
struct Cyc_Absyn_Stmt* _temp742; struct _tuple3 _temp744; struct Cyc_Absyn_Stmt*
_temp746; struct Cyc_Absyn_Exp* _temp748; struct _tuple3 _temp750; struct Cyc_Absyn_Stmt*
_temp752; struct Cyc_Absyn_Exp* _temp754; struct Cyc_Absyn_Exp* _temp756; struct
Cyc_Absyn_Stmt* _temp758; struct Cyc_Absyn_Switch_clause** _temp760; struct Cyc_Absyn_Switch_clause*
_temp762; struct Cyc_List_List* _temp763; struct Cyc_Absyn_Stmt* _temp765;
struct Cyc_Absyn_Stmt* _temp767; struct Cyc_Absyn_Stmt* _temp769; struct Cyc_List_List*
_temp771; struct Cyc_Absyn_Exp* _temp773; struct Cyc_List_List* _temp775; struct
Cyc_Absyn_Stmt* _temp777; struct Cyc_Absyn_Stmt* _temp779; struct Cyc_Absyn_Decl*
_temp781; struct Cyc_Absyn_Decl _temp783; void* _temp784; struct Cyc_Absyn_Vardecl*
_temp786; struct Cyc_Absyn_Stmt* _temp788; struct Cyc_Absyn_Decl* _temp790;
struct Cyc_Absyn_Decl _temp792; void* _temp793; struct Cyc_Absyn_Exp* _temp795;
struct Cyc_Core_Opt* _temp797; struct Cyc_Core_Opt _temp799; struct Cyc_List_List*
_temp800; struct Cyc_Absyn_Stmt* _temp802; struct Cyc_Absyn_Decl* _temp804;
struct Cyc_Absyn_Decl _temp806; void* _temp807; struct Cyc_List_List* _temp809;
struct Cyc_Absyn_Stmt* _temp811; struct Cyc_Absyn_Stmt* _temp813; struct Cyc_Absyn_Vardecl*
_temp815; struct Cyc_Absyn_Tvar* _temp817; _LL666: if( _temp664 == ( void*) Cyc_Absyn_Skip_s){
goto _LL667;} else{ goto _LL668;} _LL668: if(( unsigned int) _temp664 >  1u?*((
int*) _temp664) ==  Cyc_Absyn_Return_s: 0){ _LL711: _temp710=(( struct Cyc_Absyn_Return_s_struct*)
_temp664)->f1; if( _temp710 ==  0){ goto _LL669;} else{ goto _LL670;}} else{
goto _LL670;} _LL670: if(( unsigned int) _temp664 >  1u?*(( int*) _temp664) == 
Cyc_Absyn_Return_s: 0){ _LL713: _temp712=(( struct Cyc_Absyn_Return_s_struct*)
_temp664)->f1; goto _LL671;} else{ goto _LL672;} _LL672: if(( unsigned int)
_temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Exp_s: 0){ _LL715: _temp714=((
struct Cyc_Absyn_Exp_s_struct*) _temp664)->f1; goto _LL673;} else{ goto _LL674;}
_LL674: if(( unsigned int) _temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Seq_s:
0){ _LL719: _temp718=(( struct Cyc_Absyn_Seq_s_struct*) _temp664)->f1; goto
_LL717; _LL717: _temp716=(( struct Cyc_Absyn_Seq_s_struct*) _temp664)->f2; goto
_LL675;} else{ goto _LL676;} _LL676: if(( unsigned int) _temp664 >  1u?*(( int*)
_temp664) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL725: _temp724=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp664)->f1; goto _LL723; _LL723: _temp722=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp664)->f2; goto _LL721; _LL721: _temp720=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp664)->f3; goto _LL677;} else{ goto _LL678;} _LL678: if(( unsigned int)
_temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_While_s: 0){ _LL729: _temp728=((
struct Cyc_Absyn_While_s_struct*) _temp664)->f1; _LL733: _temp732= _temp728.f1;
goto _LL731; _LL731: _temp730= _temp728.f2; goto _LL727; _LL727: _temp726=((
struct Cyc_Absyn_While_s_struct*) _temp664)->f2; goto _LL679;} else{ goto _LL680;}
_LL680: if(( unsigned int) _temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Do_s:
0){ _LL741: _temp740=(( struct Cyc_Absyn_Do_s_struct*) _temp664)->f1; goto
_LL735; _LL735: _temp734=(( struct Cyc_Absyn_Do_s_struct*) _temp664)->f2; _LL739:
_temp738= _temp734.f1; goto _LL737; _LL737: _temp736= _temp734.f2; goto _LL681;}
else{ goto _LL682;} _LL682: if(( unsigned int) _temp664 >  1u?*(( int*) _temp664)
==  Cyc_Absyn_For_s: 0){ _LL757: _temp756=(( struct Cyc_Absyn_For_s_struct*)
_temp664)->f1; goto _LL751; _LL751: _temp750=(( struct Cyc_Absyn_For_s_struct*)
_temp664)->f2; _LL755: _temp754= _temp750.f1; goto _LL753; _LL753: _temp752=
_temp750.f2; goto _LL745; _LL745: _temp744=(( struct Cyc_Absyn_For_s_struct*)
_temp664)->f3; _LL749: _temp748= _temp744.f1; goto _LL747; _LL747: _temp746=
_temp744.f2; goto _LL743; _LL743: _temp742=(( struct Cyc_Absyn_For_s_struct*)
_temp664)->f4; goto _LL683;} else{ goto _LL684;} _LL684: if(( unsigned int)
_temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Break_s: 0){ _LL759: _temp758=((
struct Cyc_Absyn_Break_s_struct*) _temp664)->f1; if( _temp758 ==  0){ goto
_LL685;} else{ goto _LL686;}} else{ goto _LL686;} _LL686: if(( unsigned int)
_temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Fallthru_s: 0){ _LL764:
_temp763=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp664)->f1; goto _LL761;
_LL761: _temp760=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp664)->f2; if(
_temp760 ==  0){ goto _LL688;} else{ _temp762=* _temp760; goto _LL687;}} else{
goto _LL688;} _LL688: if(( unsigned int) _temp664 >  1u?*(( int*) _temp664) == 
Cyc_Absyn_Break_s: 0){ _LL766: _temp765=(( struct Cyc_Absyn_Break_s_struct*)
_temp664)->f1; goto _LL689;} else{ goto _LL690;} _LL690: if(( unsigned int)
_temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Continue_s: 0){ _LL768:
_temp767=(( struct Cyc_Absyn_Continue_s_struct*) _temp664)->f1; goto _LL691;}
else{ goto _LL692;} _LL692: if(( unsigned int) _temp664 >  1u?*(( int*) _temp664)
==  Cyc_Absyn_Goto_s: 0){ _LL770: _temp769=(( struct Cyc_Absyn_Goto_s_struct*)
_temp664)->f2; goto _LL693;} else{ goto _LL694;} _LL694: if(( unsigned int)
_temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Switch_s: 0){ _LL774: _temp773=((
struct Cyc_Absyn_Switch_s_struct*) _temp664)->f1; goto _LL772; _LL772: _temp771=((
struct Cyc_Absyn_Switch_s_struct*) _temp664)->f2; goto _LL695;} else{ goto
_LL696;} _LL696: if(( unsigned int) _temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_TryCatch_s:
0){ _LL778: _temp777=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp664)->f1; goto
_LL776; _LL776: _temp775=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp664)->f2;
goto _LL697;} else{ goto _LL698;} _LL698: if(( unsigned int) _temp664 >  1u?*((
int*) _temp664) ==  Cyc_Absyn_Decl_s: 0){ _LL782: _temp781=(( struct Cyc_Absyn_Decl_s_struct*)
_temp664)->f1; _temp783=* _temp781; _LL785: _temp784=( void*) _temp783.r; if(*((
int*) _temp784) ==  Cyc_Absyn_Var_d){ _LL787: _temp786=(( struct Cyc_Absyn_Var_d_struct*)
_temp784)->f1; goto _LL780;} else{ goto _LL700;} _LL780: _temp779=(( struct Cyc_Absyn_Decl_s_struct*)
_temp664)->f2; goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int)
_temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Decl_s: 0){ _LL791: _temp790=((
struct Cyc_Absyn_Decl_s_struct*) _temp664)->f1; _temp792=* _temp790; _LL794:
_temp793=( void*) _temp792.r; if(*(( int*) _temp793) ==  Cyc_Absyn_Let_d){
_LL798: _temp797=(( struct Cyc_Absyn_Let_d_struct*) _temp793)->f2; if( _temp797
==  0){ goto _LL702;} else{ _temp799=* _temp797; _LL801: _temp800=( struct Cyc_List_List*)
_temp799.v; goto _LL796;} _LL796: _temp795=(( struct Cyc_Absyn_Let_d_struct*)
_temp793)->f4; goto _LL789;} else{ goto _LL702;} _LL789: _temp788=(( struct Cyc_Absyn_Decl_s_struct*)
_temp664)->f2; goto _LL701;} else{ goto _LL702;} _LL702: if(( unsigned int)
_temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Decl_s: 0){ _LL805: _temp804=((
struct Cyc_Absyn_Decl_s_struct*) _temp664)->f1; _temp806=* _temp804; _LL808:
_temp807=( void*) _temp806.r; if(*(( int*) _temp807) ==  Cyc_Absyn_Letv_d){
_LL810: _temp809=(( struct Cyc_Absyn_Letv_d_struct*) _temp807)->f1; goto _LL803;}
else{ goto _LL704;} _LL803: _temp802=(( struct Cyc_Absyn_Decl_s_struct*)
_temp664)->f2; goto _LL703;} else{ goto _LL704;} _LL704: if(( unsigned int)
_temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Label_s: 0){ _LL812: _temp811=((
struct Cyc_Absyn_Label_s_struct*) _temp664)->f2; goto _LL705;} else{ goto _LL706;}
_LL706: if(( unsigned int) _temp664 >  1u?*(( int*) _temp664) ==  Cyc_Absyn_Region_s:
0){ _LL818: _temp817=(( struct Cyc_Absyn_Region_s_struct*) _temp664)->f1; goto
_LL816; _LL816: _temp815=(( struct Cyc_Absyn_Region_s_struct*) _temp664)->f2;
goto _LL814; _LL814: _temp813=(( struct Cyc_Absyn_Region_s_struct*) _temp664)->f3;
goto _LL707;} else{ goto _LL708;} _LL708: goto _LL709; _LL667: return in_flow;
_LL669: return( void*) Cyc_CfFlowInfo_BottomFL; _LL671: Cyc_CfAbsexp_eval_absexp(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp712))->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp663))->hd, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL673: return Cyc_CfAbsexp_eval_absexp( env, _temp714->loc,( void*)(( struct
Cyc_List_List*) _check_null( _temp663))->hd, in_flow); _LL675: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp716, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp718, in_flow));
_LL677: { void* _temp821; void* _temp823; struct _tuple0 _temp819= Cyc_NewControlFlow_cf_evalguard(
env, _temp724->loc, _temp663, in_flow); _LL824: _temp823= _temp819.f1; goto
_LL822; _LL822: _temp821= _temp819.f2; goto _LL820; _LL820: return Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp722, _temp823),
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp720, _temp821));} _LL679: { void*
_temp825=( void*)( Cyc_NewControlFlow_pre_stmt_check( env, in_flow, _temp730))->flow;
void* _temp828; void* _temp830; struct _tuple0 _temp826= Cyc_NewControlFlow_cf_evalguard(
env, _temp732->loc, _temp663, _temp825); _LL831: _temp830= _temp826.f1; goto
_LL829; _LL829: _temp828= _temp826.f2; goto _LL827; _LL827: { void* _temp832=
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp726, _temp830); Cyc_NewControlFlow_update_flow(
env, _temp730, _temp832); return _temp828;}} _LL681: { void* _temp833= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp740, in_flow); void* _temp834=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp833, _temp736))->flow; void* _temp837; void* _temp839; struct _tuple0
_temp835= Cyc_NewControlFlow_cf_evalguard( env, _temp738->loc, _temp663,
_temp834); _LL840: _temp839= _temp835.f1; goto _LL838; _LL838: _temp837=
_temp835.f2; goto _LL836; _LL836: Cyc_NewControlFlow_update_flow( env, _temp740,
_temp839); return _temp837;} _LL683: { void* _temp841= Cyc_CfAbsexp_eval_absexp(
env, _temp756->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp663))->hd,
in_flow); void* _temp842=( void*)( Cyc_NewControlFlow_pre_stmt_check( env,
_temp841, _temp752))->flow; void* _temp845; void* _temp847; struct _tuple0
_temp843= Cyc_NewControlFlow_cf_evalguard( env, _temp754->loc,(( struct Cyc_List_List*)
_check_null( _temp663))->tl, _temp842); _LL848: _temp847= _temp843.f1; goto
_LL846; _LL846: _temp845= _temp843.f2; goto _LL844; _LL844: { void* _temp849=
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp742, _temp847); void* _temp850=(
void*)( Cyc_NewControlFlow_pre_stmt_check( env, _temp849, _temp746))->flow; void*
_temp851= Cyc_CfAbsexp_eval_absexp( env, _temp748->loc,( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp663))->tl))->tl))->tl))->hd,
_temp850); Cyc_NewControlFlow_update_flow( env, _temp752, _temp851); return
_temp845;}} _LL685: return( void*) Cyc_CfFlowInfo_BottomFL; _LL687: { void*
_temp852= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp663))->hd, in_flow); struct Cyc_List_List* _temp853=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp762->pat_vars))->v;
_temp852= Cyc_NewControlFlow_add_vardecls_flow( env, _temp852, _temp853);
_temp852= Cyc_NewControlFlow_add_init_vars_flow( env, _temp852, _temp853); Cyc_NewControlFlow_update_flow(
env,( struct Cyc_Absyn_Stmt*) _temp762->body, _temp852); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL689: _temp767= _temp765; goto _LL691; _LL691: _temp769= _temp767; goto _LL693;
_LL693: Cyc_NewControlFlow_update_flow( env,( struct Cyc_Absyn_Stmt*)
_check_null( _temp769), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL695:
return Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp771,(( struct Cyc_List_List*)
_check_null( _temp663))->tl, Cyc_CfAbsexp_eval_absexp( env, _temp773->loc,( void*)((
struct Cyc_List_List*) _check_null( _temp663))->hd, in_flow)); _LL697: { struct
Cyc_NewControlFlow_AnalEnv _temp856; void* _temp857; int _temp859; struct Cyc_NewControlFlow_AnalEnv*
_temp854= env; _temp856=* _temp854; _LL860: _temp859= _temp856.in_try; goto
_LL858; _LL858: _temp857=( void*) _temp856.tryflow; goto _LL855; _LL855: env->in_try=
1;( void*)( env->tryflow=( void*) in_flow);{ void* _temp861= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp777, in_flow); void* _temp862=( void*) env->tryflow; env->in_try=
_temp859;( void*)( env->tryflow=( void*) _temp857); Cyc_NewControlFlow_update_tryflow(
env, _temp862);{ void* _temp863= Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp775, _temp663, _temp862); return Cyc_CfFlowInfo_join_flow( env->all_changed,
_temp861, _temp863);}}} _LL699: { struct Cyc_List_List _temp864=({ struct Cyc_List_List
_temp867; _temp867.hd=( void*) _temp786; _temp867.tl= 0; _temp867;}); void*
_temp865= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)&
_temp864); struct Cyc_Absyn_Exp* _temp866= _temp786->initializer; if( _temp866
!=  0){ _temp865= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp866))->loc,( void*)(( struct Cyc_List_List*) _check_null(
_temp663))->hd, _temp865);} return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp779, _temp865);} _LL701: { void* _temp868= Cyc_CfAbsexp_eval_absexp( env,
_temp795->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp663))->hd, Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp800)); _temp868= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp868, _temp800); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp788,
_temp868);} _LL703: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp802,
Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp809)); _LL705: return
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp811, in_flow); _LL707: { struct
Cyc_List_List* _temp869=({ struct Cyc_List_List* _temp871=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp871->hd=( void*) _temp815;
_temp871->tl= 0; _temp871;}); void* _temp870= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp869); _temp870= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp870, _temp869); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp813,
_temp870);} _LL709:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp872=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp872[ 0]=({ struct Cyc_Core_Impossible_struct _temp873; _temp873.tag= Cyc_Core_Impossible;
_temp873.f1= _tag_arr("cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 62u); _temp873;}); _temp872;})); _LL665:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp874=( void*) s->r; struct Cyc_Absyn_Stmt* _temp898; struct Cyc_Absyn_Stmt*
_temp900; struct Cyc_Absyn_Stmt* _temp902; struct Cyc_Absyn_Stmt* _temp904;
struct Cyc_Absyn_Stmt* _temp906; struct _tuple3 _temp908; struct Cyc_Absyn_Stmt*
_temp910; struct _tuple3 _temp912; struct Cyc_Absyn_Stmt* _temp914; struct Cyc_Absyn_Stmt*
_temp916; struct _tuple3 _temp918; struct Cyc_Absyn_Stmt* _temp920; struct
_tuple3 _temp922; struct Cyc_Absyn_Stmt* _temp924; struct Cyc_Absyn_Stmt*
_temp926; struct Cyc_Absyn_Stmt* _temp928; struct Cyc_Absyn_Stmt* _temp930;
struct Cyc_Absyn_Stmt* _temp932; struct Cyc_List_List* _temp934; struct Cyc_Absyn_Stmt*
_temp936; struct Cyc_List_List* _temp938; _LL876: if(( unsigned int) _temp874 > 
1u?*(( int*) _temp874) ==  Cyc_Absyn_Seq_s: 0){ _LL901: _temp900=(( struct Cyc_Absyn_Seq_s_struct*)
_temp874)->f1; goto _LL899; _LL899: _temp898=(( struct Cyc_Absyn_Seq_s_struct*)
_temp874)->f2; goto _LL877;} else{ goto _LL878;} _LL878: if(( unsigned int)
_temp874 >  1u?*(( int*) _temp874) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL905:
_temp904=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp874)->f2; goto _LL903;
_LL903: _temp902=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp874)->f3; goto
_LL879;} else{ goto _LL880;} _LL880: if(( unsigned int) _temp874 >  1u?*(( int*)
_temp874) ==  Cyc_Absyn_For_s: 0){ _LL913: _temp912=(( struct Cyc_Absyn_For_s_struct*)
_temp874)->f2; _LL915: _temp914= _temp912.f2; goto _LL909; _LL909: _temp908=((
struct Cyc_Absyn_For_s_struct*) _temp874)->f3; _LL911: _temp910= _temp908.f2;
goto _LL907; _LL907: _temp906=(( struct Cyc_Absyn_For_s_struct*) _temp874)->f4;
goto _LL881;} else{ goto _LL882;} _LL882: if(( unsigned int) _temp874 >  1u?*((
int*) _temp874) ==  Cyc_Absyn_While_s: 0){ _LL919: _temp918=(( struct Cyc_Absyn_While_s_struct*)
_temp874)->f1; _LL921: _temp920= _temp918.f2; goto _LL917; _LL917: _temp916=((
struct Cyc_Absyn_While_s_struct*) _temp874)->f2; goto _LL883;} else{ goto _LL884;}
_LL884: if(( unsigned int) _temp874 >  1u?*(( int*) _temp874) ==  Cyc_Absyn_Do_s:
0){ _LL927: _temp926=(( struct Cyc_Absyn_Do_s_struct*) _temp874)->f1; goto
_LL923; _LL923: _temp922=(( struct Cyc_Absyn_Do_s_struct*) _temp874)->f2; _LL925:
_temp924= _temp922.f2; goto _LL885;} else{ goto _LL886;} _LL886: if((
unsigned int) _temp874 >  1u?*(( int*) _temp874) ==  Cyc_Absyn_Region_s: 0){
_LL929: _temp928=(( struct Cyc_Absyn_Region_s_struct*) _temp874)->f3; goto
_LL887;} else{ goto _LL888;} _LL888: if(( unsigned int) _temp874 >  1u?*(( int*)
_temp874) ==  Cyc_Absyn_Label_s: 0){ _LL931: _temp930=(( struct Cyc_Absyn_Label_s_struct*)
_temp874)->f2; goto _LL889;} else{ goto _LL890;} _LL890: if(( unsigned int)
_temp874 >  1u?*(( int*) _temp874) ==  Cyc_Absyn_Decl_s: 0){ _LL933: _temp932=((
struct Cyc_Absyn_Decl_s_struct*) _temp874)->f2; goto _LL891;} else{ goto _LL892;}
_LL892: if(( unsigned int) _temp874 >  1u?*(( int*) _temp874) ==  Cyc_Absyn_TryCatch_s:
0){ _LL937: _temp936=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp874)->f1; goto
_LL935; _LL935: _temp934=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp874)->f2;
goto _LL893;} else{ goto _LL894;} _LL894: if(( unsigned int) _temp874 >  1u?*((
int*) _temp874) ==  Cyc_Absyn_Switch_s: 0){ _LL939: _temp938=(( struct Cyc_Absyn_Switch_s_struct*)
_temp874)->f2; goto _LL895;} else{ goto _LL896;} _LL896: goto _LL897; _LL877:
_temp904= _temp900; _temp902= _temp898; goto _LL879; _LL879: Cyc_NewControlFlow_cf_postpass(
_temp904); Cyc_NewControlFlow_cf_postpass( _temp902); return; _LL881: Cyc_NewControlFlow_cf_postpass(
_temp910); _temp920= _temp906; _temp916= _temp914; goto _LL883; _LL883: _temp926=
_temp916; _temp924= _temp920; goto _LL885; _LL885: Cyc_NewControlFlow_cf_postpass(
_temp924); _temp928= _temp926; goto _LL887; _LL887: _temp930= _temp928; goto
_LL889; _LL889: _temp932= _temp930; goto _LL891; _LL891: Cyc_NewControlFlow_cf_postpass(
_temp932); return; _LL893: Cyc_NewControlFlow_cf_postpass( _temp936); _temp938=
_temp934; goto _LL895; _LL895: for( 0; _temp938 !=  0; _temp938=(( struct Cyc_List_List*)
_check_null( _temp938))->tl){ Cyc_NewControlFlow_cf_postpass((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp938))->hd)->body);} return; _LL897:
return; _LL875:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv* absenv=({ struct Cyc_NewControlFlow_AbsEnv*
_temp952=( struct Cyc_NewControlFlow_AbsEnv*) _cycalloc( sizeof( struct Cyc_NewControlFlow_AbsEnv));
_temp952->all_roots= Cyc_Dict_empty( Cyc_CfFlowInfo_local_root_cmp); _temp952;});
struct Cyc_List_List* _temp940=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( fd->param_vardecls))->v;{ struct Cyc_List_List* vds= _temp940; for(
0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
Cyc_NewControlFlow_cf_prepass( absenv, fd->body);{ struct Cyc_NewControlFlow_AnalEnv*
_temp941=({ struct Cyc_NewControlFlow_AnalEnv* _temp951=( struct Cyc_NewControlFlow_AnalEnv*)
_cycalloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)); _temp951->roots= absenv->all_roots;
_temp951->in_try= 0; _temp951->tryflow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp951->all_changed= 0; _temp951;}); void* in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp949=( struct Cyc_CfFlowInfo_InitsFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp949[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp950; _temp950.tag= Cyc_CfFlowInfo_InitsFL;
_temp950.f1= Cyc_Dict_empty( Cyc_CfFlowInfo_local_root_cmp); _temp950;});
_temp949;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( _temp941, in_flow,
_temp940);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num= 0; Cyc_NewControlFlow_iterate_cf_check=
1; while( Cyc_NewControlFlow_iterate_cf_check) { ++ Cyc_NewControlFlow_iteration_num;
Cyc_NewControlFlow_iterate_cf_check= 0; out_flow= Cyc_NewControlFlow_cf_analyze_stmt(
_temp941, fd->body, in_flow);}{ void* _temp942= out_flow; _LL944: if( _temp942
== ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL945;} else{ goto _LL946;} _LL946:
goto _LL947; _LL945: goto _LL943; _LL947: if(( void*) fd->ret_type != ( void*)
Cyc_Absyn_VoidType){({ void* _temp948[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc,
_tag_arr("function may complete without returning a value", sizeof(
unsigned char), 48u), _tag_arr( _temp948, sizeof( void*), 0u));});} goto _LL943;
_LL943:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds !=  0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp953=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd)->r; struct Cyc_Absyn_Fndecl*
_temp965; struct Cyc_List_List* _temp967; struct Cyc_List_List* _temp969; struct
Cyc_List_List* _temp971; _LL955: if(*(( int*) _temp953) ==  Cyc_Absyn_Fn_d){
_LL966: _temp965=(( struct Cyc_Absyn_Fn_d_struct*) _temp953)->f1; goto _LL956;}
else{ goto _LL957;} _LL957: if(*(( int*) _temp953) ==  Cyc_Absyn_ExternC_d){
_LL968: _temp967=(( struct Cyc_Absyn_ExternC_d_struct*) _temp953)->f1; goto
_LL958;} else{ goto _LL959;} _LL959: if(*(( int*) _temp953) ==  Cyc_Absyn_Using_d){
_LL970: _temp969=(( struct Cyc_Absyn_Using_d_struct*) _temp953)->f2; goto _LL960;}
else{ goto _LL961;} _LL961: if(*(( int*) _temp953) ==  Cyc_Absyn_Namespace_d){
_LL972: _temp971=(( struct Cyc_Absyn_Namespace_d_struct*) _temp953)->f2; goto
_LL962;} else{ goto _LL963;} _LL963: goto _LL964; _LL956: Cyc_NewControlFlow_cf_check_fun(
_temp965); goto _LL954; _LL958: _temp969= _temp967; goto _LL960; _LL960:
_temp971= _temp969; goto _LL962; _LL962: Cyc_NewControlFlow_cf_check( _temp971);
goto _LL954; _LL964: goto _LL954; _LL954:;}}
