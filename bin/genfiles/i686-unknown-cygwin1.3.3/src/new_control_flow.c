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
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Set_Set; extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern struct
Cyc_Set_Set* Cyc_Set_union_two( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2);
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k, void* v);
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k); extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern void* Cyc_Std___assert_fail(
struct _tagged_arr assertion, struct _tagged_arr file, unsigned int line);
struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line;
int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct
Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
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
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_CfFlowInfo_Place{ void* root;
struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_NoneIL= 0;
static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; extern unsigned char Cyc_CfFlowInfo_IsZero[ 11u]; extern unsigned char Cyc_CfFlowInfo_NotZero[
12u]; extern unsigned char Cyc_CfFlowInfo_UnknownZ[ 13u]; static const int Cyc_CfFlowInfo_PlaceL=
0; struct Cyc_CfFlowInfo_PlaceL_struct{ int tag; struct Cyc_CfFlowInfo_Place* f1;
} ; static const int Cyc_CfFlowInfo_UnknownL= 0; static const int Cyc_CfFlowInfo_Zero=
0; static const int Cyc_CfFlowInfo_NotZeroAll= 1; static const int Cyc_CfFlowInfo_NotZeroThis=
2; static const int Cyc_CfFlowInfo_UnknownR= 0; struct Cyc_CfFlowInfo_UnknownR_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_Esc= 1; struct Cyc_CfFlowInfo_Esc_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_AddressOf= 2; struct Cyc_CfFlowInfo_AddressOf_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_Aggregate=
3; struct Cyc_CfFlowInfo_Aggregate_struct{ int tag; struct Cyc_Dict_Dict* f1; }
; static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_ReachableFL=
0; struct Cyc_CfFlowInfo_ReachableFL_struct{ int tag; struct Cyc_Dict_Dict* f1;
} ; extern struct Cyc_Set_Set* Cyc_CfFlowInfo_mt_place_set(); extern void* Cyc_CfFlowInfo_unknown_none;
extern void* Cyc_CfFlowInfo_unknown_all; extern int Cyc_CfFlowInfo_root_cmp(
void*, void*); extern void* Cyc_CfFlowInfo_typ_to_absrval( void* t, void*
leafval); extern void* Cyc_CfFlowInfo_initlevel( struct Cyc_Dict_Dict* d, void*
r); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d, struct
Cyc_CfFlowInfo_Place* place); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed, void* r); extern
struct Cyc_Dict_Dict* Cyc_CfFlowInfo_assign_place( struct Cyc_Position_Segment*
loc, struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed, struct Cyc_CfFlowInfo_Place*
place, void* r); extern void* Cyc_CfFlowInfo_join_flow( struct Cyc_Set_Set**,
void*, void*); extern void* Cyc_CfFlowInfo_after_flow( struct Cyc_Set_Set**,
void*, void*, struct Cyc_Set_Set*, struct Cyc_Set_Set*); extern void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser); extern void
Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; struct Cyc_NewControlFlow_CFStmtAnnot{ struct Cyc_Absyn_Stmt*
encloser; int visited; void* flow; } ; static unsigned char Cyc_NewControlFlow_CFAnnot[
12u]="\000\000\000\000CFAnnot"; struct Cyc_NewControlFlow_CFAnnot_struct{
unsigned char* tag; struct Cyc_NewControlFlow_CFStmtAnnot f1; } ; void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser){( void*)(
enclosee->annot=( void*)(( void*)({ struct Cyc_NewControlFlow_CFAnnot_struct*
_temp0=( struct Cyc_NewControlFlow_CFAnnot_struct*) _cycalloc( sizeof( struct
Cyc_NewControlFlow_CFAnnot_struct)); _temp0[ 0]=({ struct Cyc_NewControlFlow_CFAnnot_struct
_temp1; _temp1.tag= Cyc_NewControlFlow_CFAnnot; _temp1.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot
_temp2; _temp2.encloser= encloser; _temp2.visited= 0; _temp2.flow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp2;}); _temp1;}); _temp0;})));} static
struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_get_stmt_annot( struct
Cyc_Absyn_Stmt* s){ void* _temp3=( void*) s->annot; struct Cyc_NewControlFlow_CFStmtAnnot
_temp9; struct Cyc_NewControlFlow_CFStmtAnnot* _temp11; _LL5: if(*(( void**)
_temp3) ==  Cyc_NewControlFlow_CFAnnot){ _LL10: _temp9=(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp3)->f1; _temp11=( struct Cyc_NewControlFlow_CFStmtAnnot*)&(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp3)->f1; goto _LL6;} else{ goto _LL7;} _LL7: goto _LL8; _LL6: return _temp11;
_LL8:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp12=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp12[ 0]=({ struct Cyc_Core_Impossible_struct _temp13; _temp13.tag= Cyc_Core_Impossible;
_temp13.f1= _tag_arr("ControlFlow -- wrong stmt annotation", sizeof(
unsigned char), 37u); _temp13;}); _temp12;})); _LL4:;} struct Cyc_NewControlFlow_AnalEnv{
int iterate_again; int iteration_num; int in_try; void* tryflow; struct Cyc_Set_Set**
all_changed; } ; static void* Cyc_NewControlFlow_anal_stmt( struct Cyc_NewControlFlow_AnalEnv*,
void*, struct Cyc_Absyn_Stmt*); static void* Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*, void*, struct Cyc_Absyn_Decl*); static
struct _tuple0 Cyc_NewControlFlow_anal_Lexp( struct Cyc_NewControlFlow_AnalEnv*,
void*, struct Cyc_Absyn_Exp*); static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*, void*, struct Cyc_Absyn_Exp*); static struct
_tuple0 Cyc_NewControlFlow_anal_test( struct Cyc_NewControlFlow_AnalEnv*, void*,
struct Cyc_Absyn_Exp*); static void Cyc_NewControlFlow_update_tryflow( struct
Cyc_NewControlFlow_AnalEnv* env, void* new_flow){ if( env->in_try){( void*)( env->tryflow=(
void*) Cyc_CfFlowInfo_join_flow( env->all_changed, new_flow,( void*) env->tryflow));}}
static void Cyc_NewControlFlow_update_flow( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp14= Cyc_NewControlFlow_get_stmt_annot( s); void* _temp15= Cyc_CfFlowInfo_join_flow(
env->all_changed, flow,( void*) _temp14->flow); if( ! Cyc_CfFlowInfo_flow_lessthan_approx(
_temp15,( void*) _temp14->flow)){( void*)( _temp14->flow=( void*) _temp15); if(
_temp14->visited ==  env->iteration_num){ env->iterate_again= 1;}}} static void*
Cyc_NewControlFlow_add_vars( void* inflow, struct Cyc_List_List* vds, void*
leafval){ void* _temp16= inflow; struct Cyc_Dict_Dict* _temp22; _LL18: if(
_temp16 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL19;} else{ goto _LL20;}
_LL20: if(( unsigned int) _temp16 >  1u?*(( int*) _temp16) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL23: _temp22=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp16)->f1;
goto _LL21;} else{ goto _LL17;} _LL19: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL21: for( 0; vds !=  0; vds= vds->tl){ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp24=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp25=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp25[ 0]=({ struct
Cyc_CfFlowInfo_VarRoot_struct _temp26; _temp26.tag= Cyc_CfFlowInfo_VarRoot;
_temp26.f1=( struct Cyc_Absyn_Vardecl*) vds->hd; _temp26;}); _temp25;}); _temp22=
Cyc_Dict_insert( _temp22,( void*) _temp24, Cyc_CfFlowInfo_typ_to_absrval(( void*)((
struct Cyc_Absyn_Vardecl*) vds->hd)->type, leafval));} return( void*)({ struct
Cyc_CfFlowInfo_ReachableFL_struct* _temp27=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp27[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp28; _temp28.tag= Cyc_CfFlowInfo_ReachableFL;
_temp28.f1= _temp22; _temp28;}); _temp27;}); _LL17:;} static void* Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void*
inflow, void* r){ void* _temp29= inflow; struct Cyc_Dict_Dict* _temp35; _LL31:
if( _temp29 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL32;} else{ goto _LL33;}
_LL33: if(( unsigned int) _temp29 >  1u?*(( int*) _temp29) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL36: _temp35=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp29)->f1;
goto _LL34;} else{ goto _LL30;} _LL32: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL34: if( Cyc_CfFlowInfo_initlevel( _temp35, r) != ( void*) Cyc_CfFlowInfo_AllIL){({
void* _temp37[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("expression may not be fully initialized",
sizeof( unsigned char), 40u), _tag_arr( _temp37, sizeof( void*), 0u));});}{
struct Cyc_Dict_Dict* _temp38= Cyc_CfFlowInfo_escape_deref( _temp35, env->all_changed,
r); if( _temp35 ==  _temp38){ return inflow;}{ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp39=({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp40=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp40[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp41; _temp41.tag= Cyc_CfFlowInfo_ReachableFL;
_temp41.f1= _temp38; _temp41;}); _temp40;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp39); return( void*) _temp39;}} _LL30:;} struct _tuple4{ void*
f1; struct Cyc_List_List* f2; } ; static struct _tuple4 Cyc_NewControlFlow_anal_unordered_Rexps(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* es){
if( es ==  0){ return({ struct _tuple4 _temp42; _temp42.f1= inflow; _temp42.f2=
0; _temp42;});} if( es->tl ==  0){ void* _temp45; void* _temp47; struct _tuple0
_temp43= Cyc_NewControlFlow_anal_Rexp( env, inflow,( struct Cyc_Absyn_Exp*) es->hd);
_LL48: _temp47= _temp43.f1; goto _LL46; _LL46: _temp45= _temp43.f2; goto _LL44;
_LL44: return({ struct _tuple4 _temp49; _temp49.f1= _temp47; _temp49.f2=({
struct Cyc_List_List* _temp50=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp50->hd=( void*) _temp45; _temp50->tl= 0; _temp50;});
_temp49;});}{ struct Cyc_Set_Set** outer_all_changed= env->all_changed; struct
Cyc_Set_Set* this_all_changed; void* old_inflow; void* outflow; struct Cyc_List_List*
rvals; do { this_all_changed= Cyc_CfFlowInfo_mt_place_set(); env->all_changed=({
struct Cyc_Set_Set** _temp51=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp51[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp51;});{ void*
_temp54; void* _temp56; struct _tuple0 _temp52= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) es->hd); _LL57: _temp56= _temp52.f1; goto
_LL55; _LL55: _temp54= _temp52.f2; goto _LL53; _LL53: outflow= _temp56; rvals=({
struct Cyc_List_List* _temp58=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp58->hd=( void*) _temp54; _temp58->tl= 0; _temp58;});
this_all_changed=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)( this_all_changed,*(( struct Cyc_Set_Set**) _check_null(
env->all_changed)));{ struct Cyc_List_List* es2= es->tl; for( 0; es2 !=  0; es2=
es2->tl){ env->all_changed=({ struct Cyc_Set_Set** _temp59=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp59[ 0]= Cyc_CfFlowInfo_mt_place_set();
_temp59;});{ void* _temp62; void* _temp64; struct _tuple0 _temp60= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) es2->hd); _LL65: _temp64= _temp60.f1; goto
_LL63; _LL63: _temp62= _temp60.f2; goto _LL61; _LL61: rvals=({ struct Cyc_List_List*
_temp66=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp66->hd=( void*) _temp62; _temp66->tl= rvals; _temp66;}); outflow= Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& this_all_changed, outflow, _temp64, this_all_changed,*((
struct Cyc_Set_Set**) _check_null( env->all_changed))); this_all_changed=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
this_all_changed,*(( struct Cyc_Set_Set**) _check_null( env->all_changed)));}}}
old_inflow= inflow; inflow= Cyc_CfFlowInfo_join_flow( outer_all_changed, inflow,
outflow);}} while ( ! Cyc_CfFlowInfo_flow_lessthan_approx( inflow, old_inflow));
if( outer_all_changed ==  0){ env->all_changed= 0;} else{ env->all_changed=({
struct Cyc_Set_Set** _temp67=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp67[ 0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(* outer_all_changed,
this_all_changed); _temp67;});} Cyc_NewControlFlow_update_tryflow( env, outflow);
return({ struct _tuple4 _temp68; _temp68.f1= outflow; _temp68.f2= Cyc_List_imp_rev(
rvals); _temp68;});}} static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* es){
struct Cyc_List_List* _temp71; void* _temp73; struct _tuple4 _temp69= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, es); _LL74: _temp73= _temp69.f1; goto _LL72; _LL72: _temp71=
_temp69.f2; goto _LL70; _LL70:{ void* _temp75= _temp73; struct Cyc_Dict_Dict*
_temp81; _LL77: if(( unsigned int) _temp75 >  1u?*(( int*) _temp75) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL82: _temp81=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp75)->f1;
goto _LL78;} else{ goto _LL79;} _LL79: if( _temp75 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL80;} else{ goto _LL76;} _LL78: for( 0; _temp71 !=  0;( _temp71= _temp71->tl,
es=(( struct Cyc_List_List*) _check_null( es))->tl)){ if( Cyc_CfFlowInfo_initlevel(
_temp81,( void*) _temp71->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp83[ 0u]={}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp83, sizeof( void*), 0u));});}} goto
_LL76; _LL80: goto _LL76; _LL76:;} return({ struct _tuple0 _temp84; _temp84.f1=
_temp73; _temp84.f2= Cyc_CfFlowInfo_unknown_all; _temp84;});} static void* Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* outflow, struct Cyc_Absyn_Exp*
e, void* il){ void* _temp85= outflow; struct Cyc_Dict_Dict* _temp91; _LL87: if(
_temp85 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL88;} else{ goto _LL89;}
_LL89: if(( unsigned int) _temp85 >  1u?*(( int*) _temp85) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL92: _temp91=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp85)->f1;
goto _LL90;} else{ goto _LL86;} _LL88: return outflow; _LL90: { void* _temp93=(
Cyc_NewControlFlow_anal_Lexp( env, inflow, e)).f2; struct Cyc_CfFlowInfo_Place*
_temp99; _LL95: if( _temp93 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL96;}
else{ goto _LL97;} _LL97: if(( unsigned int) _temp93 >  1u?*(( int*) _temp93) == 
Cyc_CfFlowInfo_PlaceL: 0){ _LL100: _temp99=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp93)->f1; goto _LL98;} else{ goto _LL94;} _LL96: return outflow; _LL98: {
void* nzval= il == ( void*) Cyc_CfFlowInfo_AllIL?( void*) Cyc_CfFlowInfo_NotZeroAll:(
void*) Cyc_CfFlowInfo_NotZeroThis; return( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp101=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp101[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp102; _temp102.tag= Cyc_CfFlowInfo_ReachableFL; _temp102.f1= Cyc_CfFlowInfo_assign_place(
e->loc, _temp91, env->all_changed, _temp99, nzval); _temp102;}); _temp101;});}
_LL94:;} _LL86:;} static struct _tuple0 Cyc_NewControlFlow_splitzero( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, void* outflow, struct Cyc_Absyn_Exp* e, void* il){ void*
_temp103= outflow; struct Cyc_Dict_Dict* _temp109; _LL105: if( _temp103 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL106;} else{ goto _LL107;} _LL107: if((
unsigned int) _temp103 >  1u?*(( int*) _temp103) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL110: _temp109=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp103)->f1;
goto _LL108;} else{ goto _LL104;} _LL106: return({ struct _tuple0 _temp111;
_temp111.f1= outflow; _temp111.f2= outflow; _temp111;}); _LL108: { void*
_temp112=( Cyc_NewControlFlow_anal_Lexp( env, inflow, e)).f2; struct Cyc_CfFlowInfo_Place*
_temp118; _LL114: if( _temp112 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL115;}
else{ goto _LL116;} _LL116: if(( unsigned int) _temp112 >  1u?*(( int*) _temp112)
==  Cyc_CfFlowInfo_PlaceL: 0){ _LL119: _temp118=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp112)->f1; goto _LL117;} else{ goto _LL113;} _LL115: return({ struct _tuple0
_temp120; _temp120.f1= outflow; _temp120.f2= outflow; _temp120;}); _LL117: {
void* nzval= il == ( void*) Cyc_CfFlowInfo_AllIL?( void*) Cyc_CfFlowInfo_NotZeroAll:(
void*) Cyc_CfFlowInfo_NotZeroThis; return({ struct _tuple0 _temp121; _temp121.f1=(
void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp124=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp124[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp125; _temp125.tag= Cyc_CfFlowInfo_ReachableFL;
_temp125.f1= Cyc_CfFlowInfo_assign_place( e->loc, _temp109, env->all_changed,
_temp118, nzval); _temp125;}); _temp124;}); _temp121.f2=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp122=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp122[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp123; _temp123.tag= Cyc_CfFlowInfo_ReachableFL; _temp123.f1= Cyc_CfFlowInfo_assign_place(
e->loc, _temp109, env->all_changed, _temp118,( void*) Cyc_CfFlowInfo_Zero);
_temp123;}); _temp122;}); _temp121;});} _LL113:;} _LL104:;} static struct
_tuple0 Cyc_NewControlFlow_anal_derefR( struct Cyc_NewControlFlow_AnalEnv* env,
void* inflow, void* f, struct Cyc_Absyn_Exp* e, void* r){ void* _temp126= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo
_temp132; struct Cyc_Absyn_Conref* _temp134; void* _temp136; _LL128: if((
unsigned int) _temp126 >  4u?*(( int*) _temp126) ==  Cyc_Absyn_PointerType: 0){
_LL133: _temp132=(( struct Cyc_Absyn_PointerType_struct*) _temp126)->f1; _LL137:
_temp136=( void*) _temp132.elt_typ; goto _LL135; _LL135: _temp134= _temp132.bounds;
goto _LL129;} else{ goto _LL130;} _LL130: goto _LL131; _LL129: { void* _temp138=
f; struct Cyc_Dict_Dict* _temp144; _LL140: if( _temp138 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL141;} else{ goto _LL142;} _LL142: if(( unsigned int) _temp138 >  1u?*((
int*) _temp138) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL145: _temp144=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp138)->f1; goto _LL143;} else{ goto
_LL139;} _LL141: return({ struct _tuple0 _temp146; _temp146.f1= f; _temp146.f2=
Cyc_CfFlowInfo_typ_to_absrval( _temp136, Cyc_CfFlowInfo_unknown_all); _temp146;});
_LL143: if( Cyc_Tcutil_is_bound_one( _temp134)){ void* _temp147= r; struct Cyc_CfFlowInfo_Place*
_temp161; void* _temp163; _LL149: if( _temp147 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL150;} else{ goto _LL151;} _LL151: if( _temp147 == ( void*) Cyc_CfFlowInfo_NotZeroThis){
goto _LL152;} else{ goto _LL153;} _LL153: if(( unsigned int) _temp147 >  3u?*((
int*) _temp147) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL162: _temp161=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp147)->f1; goto _LL154;} else{ goto _LL155;} _LL155: if( _temp147 == ( void*)
Cyc_CfFlowInfo_Zero){ goto _LL156;} else{ goto _LL157;} _LL157: if((
unsigned int) _temp147 >  3u?*(( int*) _temp147) ==  Cyc_CfFlowInfo_UnknownR: 0){
_LL164: _temp163=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp147)->f1;
goto _LL158;} else{ goto _LL159;} _LL159: goto _LL160; _LL150: goto _LL152;
_LL152:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero)); goto
_LL148; _LL154:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero));
return({ struct _tuple0 _temp165; _temp165.f1= f; _temp165.f2= Cyc_CfFlowInfo_lookup_place(
_temp144, _temp161); _temp165;}); _LL156:( void*)( e->annot=( void*)(( void*)
Cyc_CfFlowInfo_IsZero)); return({ struct _tuple0 _temp166; _temp166.f1=( void*)
Cyc_CfFlowInfo_BottomFL; _temp166.f2= Cyc_CfFlowInfo_typ_to_absrval( _temp136,
Cyc_CfFlowInfo_unknown_all); _temp166;}); _LL158: f= Cyc_NewControlFlow_notzero(
env, inflow, f, e, _temp163); goto _LL160; _LL160:( void*)( e->annot=( void*)((
void*) Cyc_CfFlowInfo_UnknownZ)); _LL148:;} else{( void*)( e->annot=( void*)((
void*) Cyc_CfFlowInfo_UnknownZ));}{ void* _temp167= Cyc_CfFlowInfo_initlevel(
_temp144, r); _LL169: if( _temp167 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL170;} else{ goto _LL171;} _LL171: if( _temp167 == ( void*) Cyc_CfFlowInfo_AllIL){
goto _LL172;} else{ goto _LL173;} _LL173: if( _temp167 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL174;} else{ goto _LL168;} _LL170:({ void* _temp175[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("dereference of possibly uninitialized pointer", sizeof(
unsigned char), 46u), _tag_arr( _temp175, sizeof( void*), 0u));}); goto _LL172;
_LL172: return({ struct _tuple0 _temp176; _temp176.f1= f; _temp176.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp136, Cyc_CfFlowInfo_unknown_all); _temp176;}); _LL174: return({ struct
_tuple0 _temp177; _temp177.f1= f; _temp177.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp136, Cyc_CfFlowInfo_unknown_none); _temp177;}); _LL168:;} _LL139:;} _LL131:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp178=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp178[ 0]=({ struct
Cyc_Core_Impossible_struct _temp179; _temp179.tag= Cyc_Core_Impossible; _temp179.f1=
_tag_arr("right deref of non-pointer-type", sizeof( unsigned char), 32u);
_temp179;}); _temp178;})); _LL127:;} struct _tuple5{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e){
struct Cyc_Dict_Dict* d;{ void* _temp180= inflow; struct Cyc_Dict_Dict* _temp186;
_LL182: if( _temp180 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL183;} else{
goto _LL184;} _LL184: if(( unsigned int) _temp180 >  1u?*(( int*) _temp180) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL187: _temp186=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp180)->f1; goto _LL185;} else{ goto _LL181;} _LL183: return({ struct _tuple0
_temp188; _temp188.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp188.f2= Cyc_CfFlowInfo_unknown_all;
_temp188;}); _LL185: d= _temp186; _LL181:;}{ void* _temp189=( void*) e->r;
struct Cyc_Absyn_Exp* _temp287; struct Cyc_Absyn_Exp* _temp289; struct Cyc_Absyn_Exp*
_temp291; void* _temp293; void* _temp295; int _temp297; void* _temp299; void*
_temp301; void* _temp303; void* _temp305; struct Cyc_List_List* _temp307; void*
_temp309; void* _temp311; struct Cyc_Absyn_Vardecl* _temp313; void* _temp315;
struct Cyc_Absyn_Vardecl* _temp317; void* _temp319; struct Cyc_Absyn_Vardecl*
_temp321; struct Cyc_List_List* _temp323; struct Cyc_Absyn_Exp* _temp325; struct
Cyc_Absyn_Exp* _temp327; struct Cyc_Core_Opt* _temp329; struct Cyc_Core_Opt
_temp331; struct Cyc_Absyn_Exp* _temp332; struct Cyc_Absyn_Exp* _temp334; struct
Cyc_Core_Opt* _temp336; struct Cyc_Absyn_Exp* _temp338; struct Cyc_Absyn_Exp*
_temp340; struct Cyc_Absyn_Exp* _temp342; struct Cyc_Absyn_Exp* _temp344; struct
Cyc_List_List* _temp346; struct Cyc_Absyn_Exp* _temp348; void* _temp350; struct
Cyc_Absyn_Exp* _temp352; struct Cyc_Absyn_Exp* _temp354; struct Cyc_Absyn_Exp*
_temp356; struct Cyc_Absyn_Exp* _temp358; struct Cyc_Absyn_Exp* _temp360; struct
_tagged_arr* _temp362; struct Cyc_Absyn_Exp* _temp364; struct _tagged_arr*
_temp366; struct Cyc_Absyn_Exp* _temp368; struct Cyc_Absyn_Exp* _temp370; struct
Cyc_Absyn_Exp* _temp372; struct Cyc_Absyn_Exp* _temp374; struct Cyc_Absyn_Exp*
_temp376; struct Cyc_Absyn_Exp* _temp378; struct Cyc_List_List* _temp380; struct
Cyc_List_List* _temp382; struct Cyc_List_List* _temp384; struct Cyc_List_List*
_temp386; struct Cyc_List_List* _temp388; struct Cyc_Absyn_Exp* _temp390; struct
Cyc_Absyn_Exp* _temp392; struct Cyc_Absyn_Vardecl* _temp394; struct Cyc_Absyn_Stmt*
_temp396; void* _temp398; _LL191: if(*(( int*) _temp189) ==  Cyc_Absyn_Cast_e){
_LL288: _temp287=(( struct Cyc_Absyn_Cast_e_struct*) _temp189)->f2; goto _LL192;}
else{ goto _LL193;} _LL193: if(*(( int*) _temp189) ==  Cyc_Absyn_NoInstantiate_e){
_LL290: _temp289=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp189)->f1;
goto _LL194;} else{ goto _LL195;} _LL195: if(*(( int*) _temp189) ==  Cyc_Absyn_Instantiate_e){
_LL292: _temp291=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp189)->f1; goto
_LL196;} else{ goto _LL197;} _LL197: if(*(( int*) _temp189) ==  Cyc_Absyn_Const_e){
_LL294: _temp293=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp189)->f1; if(
_temp293 == ( void*) Cyc_Absyn_Null_c){ goto _LL198;} else{ goto _LL199;}} else{
goto _LL199;} _LL199: if(*(( int*) _temp189) ==  Cyc_Absyn_Const_e){ _LL296:
_temp295=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp189)->f1; if((
unsigned int) _temp295 >  1u?*(( int*) _temp295) ==  Cyc_Absyn_Int_c: 0){ _LL300:
_temp299=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp295)->f1; if( _temp299
== ( void*) Cyc_Absyn_Signed){ goto _LL298;} else{ goto _LL201;} _LL298:
_temp297=(( struct Cyc_Absyn_Int_c_struct*) _temp295)->f2; if( _temp297 ==  0){
goto _LL200;} else{ goto _LL201;}} else{ goto _LL201;}} else{ goto _LL201;}
_LL201: if(*(( int*) _temp189) ==  Cyc_Absyn_Const_e){ _LL302: _temp301=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp189)->f1; if(( unsigned int) _temp301 > 
1u?*(( int*) _temp301) ==  Cyc_Absyn_Int_c: 0){ _LL304: _temp303=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp301)->f1; if( _temp303 == ( void*) Cyc_Absyn_Signed){
goto _LL202;} else{ goto _LL203;}} else{ goto _LL203;}} else{ goto _LL203;}
_LL203: if(*(( int*) _temp189) ==  Cyc_Absyn_Var_e){ _LL306: _temp305=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp189)->f2; if(( unsigned int) _temp305 >  1u?*((
int*) _temp305) ==  Cyc_Absyn_Funname_b: 0){ goto _LL204;} else{ goto _LL205;}}
else{ goto _LL205;} _LL205: if(*(( int*) _temp189) ==  Cyc_Absyn_Tunion_e){
_LL308: _temp307=(( struct Cyc_Absyn_Tunion_e_struct*) _temp189)->f3; if(
_temp307 ==  0){ goto _LL206;} else{ goto _LL207;}} else{ goto _LL207;} _LL207:
if(*(( int*) _temp189) ==  Cyc_Absyn_Const_e){ goto _LL208;} else{ goto _LL209;}
_LL209: if(*(( int*) _temp189) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL210;} else{
goto _LL211;} _LL211: if(*(( int*) _temp189) ==  Cyc_Absyn_Sizeoftyp_e){ goto
_LL212;} else{ goto _LL213;} _LL213: if(*(( int*) _temp189) ==  Cyc_Absyn_Offsetof_e){
goto _LL214;} else{ goto _LL215;} _LL215: if(*(( int*) _temp189) ==  Cyc_Absyn_Gentyp_e){
goto _LL216;} else{ goto _LL217;} _LL217: if(*(( int*) _temp189) ==  Cyc_Absyn_AnonEnum_e){
goto _LL218;} else{ goto _LL219;} _LL219: if(*(( int*) _temp189) ==  Cyc_Absyn_Enum_e){
goto _LL220;} else{ goto _LL221;} _LL221: if(*(( int*) _temp189) ==  Cyc_Absyn_Var_e){
_LL310: _temp309=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp189)->f2; if((
unsigned int) _temp309 >  1u?*(( int*) _temp309) ==  Cyc_Absyn_Global_b: 0){
goto _LL222;} else{ goto _LL223;}} else{ goto _LL223;} _LL223: if(*(( int*)
_temp189) ==  Cyc_Absyn_Var_e){ _LL312: _temp311=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp189)->f2; if(( unsigned int) _temp311 >  1u?*(( int*) _temp311) ==  Cyc_Absyn_Param_b:
0){ _LL314: _temp313=(( struct Cyc_Absyn_Param_b_struct*) _temp311)->f1; goto
_LL224;} else{ goto _LL225;}} else{ goto _LL225;} _LL225: if(*(( int*) _temp189)
==  Cyc_Absyn_Var_e){ _LL316: _temp315=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp189)->f2; if(( unsigned int) _temp315 >  1u?*(( int*) _temp315) ==  Cyc_Absyn_Local_b:
0){ _LL318: _temp317=(( struct Cyc_Absyn_Local_b_struct*) _temp315)->f1; goto
_LL226;} else{ goto _LL227;}} else{ goto _LL227;} _LL227: if(*(( int*) _temp189)
==  Cyc_Absyn_Var_e){ _LL320: _temp319=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp189)->f2; if(( unsigned int) _temp319 >  1u?*(( int*) _temp319) ==  Cyc_Absyn_Pat_b:
0){ _LL322: _temp321=(( struct Cyc_Absyn_Pat_b_struct*) _temp319)->f1; goto
_LL228;} else{ goto _LL229;}} else{ goto _LL229;} _LL229: if(*(( int*) _temp189)
==  Cyc_Absyn_Primop_e){ _LL324: _temp323=(( struct Cyc_Absyn_Primop_e_struct*)
_temp189)->f2; goto _LL230;} else{ goto _LL231;} _LL231: if(*(( int*) _temp189)
==  Cyc_Absyn_Increment_e){ _LL326: _temp325=(( struct Cyc_Absyn_Increment_e_struct*)
_temp189)->f1; goto _LL232;} else{ goto _LL233;} _LL233: if(*(( int*) _temp189)
==  Cyc_Absyn_AssignOp_e){ _LL333: _temp332=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp189)->f1; goto _LL330; _LL330: _temp329=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp189)->f2; if( _temp329 ==  0){ goto _LL235;} else{ _temp331=* _temp329;
goto _LL328;} _LL328: _temp327=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp189)->f3;
goto _LL234;} else{ goto _LL235;} _LL235: if(*(( int*) _temp189) ==  Cyc_Absyn_AssignOp_e){
_LL339: _temp338=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp189)->f1; goto
_LL337; _LL337: _temp336=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp189)->f2;
if( _temp336 ==  0){ goto _LL335;} else{ goto _LL237;} _LL335: _temp334=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp189)->f3; goto _LL236;} else{ goto
_LL237;} _LL237: if(*(( int*) _temp189) ==  Cyc_Absyn_SeqExp_e){ _LL343:
_temp342=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp189)->f1; goto _LL341;
_LL341: _temp340=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp189)->f2; goto
_LL238;} else{ goto _LL239;} _LL239: if(*(( int*) _temp189) ==  Cyc_Absyn_Throw_e){
_LL345: _temp344=(( struct Cyc_Absyn_Throw_e_struct*) _temp189)->f1; goto _LL240;}
else{ goto _LL241;} _LL241: if(*(( int*) _temp189) ==  Cyc_Absyn_FnCall_e){
_LL349: _temp348=(( struct Cyc_Absyn_FnCall_e_struct*) _temp189)->f1; goto
_LL347; _LL347: _temp346=(( struct Cyc_Absyn_FnCall_e_struct*) _temp189)->f2;
goto _LL242;} else{ goto _LL243;} _LL243: if(*(( int*) _temp189) ==  Cyc_Absyn_Malloc_e){
_LL353: _temp352=(( struct Cyc_Absyn_Malloc_e_struct*) _temp189)->f1; goto
_LL351; _LL351: _temp350=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp189)->f2;
goto _LL244;} else{ goto _LL245;} _LL245: if(*(( int*) _temp189) ==  Cyc_Absyn_New_e){
_LL357: _temp356=(( struct Cyc_Absyn_New_e_struct*) _temp189)->f1; goto _LL355;
_LL355: _temp354=(( struct Cyc_Absyn_New_e_struct*) _temp189)->f2; goto _LL246;}
else{ goto _LL247;} _LL247: if(*(( int*) _temp189) ==  Cyc_Absyn_Address_e){
_LL359: _temp358=(( struct Cyc_Absyn_Address_e_struct*) _temp189)->f1; goto
_LL248;} else{ goto _LL249;} _LL249: if(*(( int*) _temp189) ==  Cyc_Absyn_Deref_e){
_LL361: _temp360=(( struct Cyc_Absyn_Deref_e_struct*) _temp189)->f1; goto _LL250;}
else{ goto _LL251;} _LL251: if(*(( int*) _temp189) ==  Cyc_Absyn_StructMember_e){
_LL365: _temp364=(( struct Cyc_Absyn_StructMember_e_struct*) _temp189)->f1; goto
_LL363; _LL363: _temp362=(( struct Cyc_Absyn_StructMember_e_struct*) _temp189)->f2;
goto _LL252;} else{ goto _LL253;} _LL253: if(*(( int*) _temp189) ==  Cyc_Absyn_StructArrow_e){
_LL369: _temp368=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp189)->f1; goto
_LL367; _LL367: _temp366=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp189)->f2;
goto _LL254;} else{ goto _LL255;} _LL255: if(*(( int*) _temp189) ==  Cyc_Absyn_Conditional_e){
_LL375: _temp374=(( struct Cyc_Absyn_Conditional_e_struct*) _temp189)->f1; goto
_LL373; _LL373: _temp372=(( struct Cyc_Absyn_Conditional_e_struct*) _temp189)->f2;
goto _LL371; _LL371: _temp370=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp189)->f3; goto _LL256;} else{ goto _LL257;} _LL257: if(*(( int*) _temp189)
==  Cyc_Absyn_Subscript_e){ _LL379: _temp378=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp189)->f1; goto _LL377; _LL377: _temp376=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp189)->f2; goto _LL258;} else{ goto _LL259;} _LL259: if(*(( int*) _temp189)
==  Cyc_Absyn_Tunion_e){ _LL381: _temp380=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp189)->f3; goto _LL260;} else{ goto _LL261;} _LL261: if(*(( int*) _temp189)
==  Cyc_Absyn_Tuple_e){ _LL383: _temp382=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp189)->f1; goto _LL262;} else{ goto _LL263;} _LL263: if(*(( int*) _temp189)
==  Cyc_Absyn_AnonStruct_e){ _LL385: _temp384=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp189)->f2; goto _LL264;} else{ goto _LL265;} _LL265: if(*(( int*) _temp189)
==  Cyc_Absyn_Struct_e){ _LL387: _temp386=(( struct Cyc_Absyn_Struct_e_struct*)
_temp189)->f3; goto _LL266;} else{ goto _LL267;} _LL267: if(*(( int*) _temp189)
==  Cyc_Absyn_Array_e){ _LL389: _temp388=(( struct Cyc_Absyn_Array_e_struct*)
_temp189)->f1; goto _LL268;} else{ goto _LL269;} _LL269: if(*(( int*) _temp189)
==  Cyc_Absyn_Comprehension_e){ _LL395: _temp394=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp189)->f1; goto _LL393; _LL393: _temp392=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp189)->f2; goto _LL391; _LL391: _temp390=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp189)->f3; goto _LL270;} else{ goto _LL271;} _LL271: if(*(( int*) _temp189)
==  Cyc_Absyn_StmtExp_e){ _LL397: _temp396=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp189)->f1; goto _LL272;} else{ goto _LL273;} _LL273: if(*(( int*) _temp189)
==  Cyc_Absyn_Var_e){ _LL399: _temp398=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp189)->f2; if( _temp398 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL274;}
else{ goto _LL275;}} else{ goto _LL275;} _LL275: if(*(( int*) _temp189) ==  Cyc_Absyn_UnknownId_e){
goto _LL276;} else{ goto _LL277;} _LL277: if(*(( int*) _temp189) ==  Cyc_Absyn_UnknownCall_e){
goto _LL278;} else{ goto _LL279;} _LL279: if(*(( int*) _temp189) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL280;} else{ goto _LL281;} _LL281: if(*(( int*) _temp189) ==  Cyc_Absyn_CompoundLit_e){
goto _LL282;} else{ goto _LL283;} _LL283: if(*(( int*) _temp189) ==  Cyc_Absyn_Codegen_e){
goto _LL284;} else{ goto _LL285;} _LL285: if(*(( int*) _temp189) ==  Cyc_Absyn_Fill_e){
goto _LL286;} else{ goto _LL190;} _LL192: _temp289= _temp287; goto _LL194;
_LL194: _temp291= _temp289; goto _LL196; _LL196: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp291); _LL198: goto _LL200; _LL200: return({ struct _tuple0
_temp400; _temp400.f1= inflow; _temp400.f2=( void*) Cyc_CfFlowInfo_Zero;
_temp400;}); _LL202: goto _LL204; _LL204: return({ struct _tuple0 _temp401;
_temp401.f1= inflow; _temp401.f2=( void*) Cyc_CfFlowInfo_NotZeroAll; _temp401;});
_LL206: goto _LL208; _LL208: goto _LL210; _LL210: goto _LL212; _LL212: goto
_LL214; _LL214: goto _LL216; _LL216: goto _LL218; _LL218: goto _LL220; _LL220:
return({ struct _tuple0 _temp402; _temp402.f1= inflow; _temp402.f2= Cyc_CfFlowInfo_unknown_all;
_temp402;}); _LL222: return({ struct _tuple0 _temp403; _temp403.f1= inflow;
_temp403.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp403;}); _LL224:
_temp317= _temp313; goto _LL226; _LL226: _temp321= _temp317; goto _LL228; _LL228:
return({ struct _tuple0 _temp404; _temp404.f1= inflow; _temp404.f2= Cyc_Dict_lookup(
d,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp405=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp405[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp406; _temp406.tag= Cyc_CfFlowInfo_VarRoot;
_temp406.f1= _temp321; _temp406;}); _temp405;})); _temp404;}); _LL230: return
Cyc_NewControlFlow_anal_use_ints( env, inflow, _temp323); _LL232: { void*
_temp410; struct _tuple0 _temp408= Cyc_NewControlFlow_anal_use_ints( env, inflow,({
struct Cyc_Absyn_Exp* _temp407[ 1u]; _temp407[ 0u]= _temp325;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp407, sizeof( struct Cyc_Absyn_Exp*),
1u));})); _LL411: _temp410= _temp408.f1; goto _LL409; _LL409: { void* _temp414;
struct _tuple0 _temp412= Cyc_NewControlFlow_anal_Lexp( env, _temp410, _temp325);
_LL415: _temp414= _temp412.f2; goto _LL413; _LL413:{ struct _tuple0 _temp417=({
struct _tuple0 _temp416; _temp416.f1= _temp414; _temp416.f2= _temp410; _temp416;});
void* _temp423; struct Cyc_Dict_Dict* _temp425; void* _temp427; struct Cyc_CfFlowInfo_Place*
_temp429; _LL419: _LL428: _temp427= _temp417.f1; if(( unsigned int) _temp427 > 
1u?*(( int*) _temp427) ==  Cyc_CfFlowInfo_PlaceL: 0){ _LL430: _temp429=(( struct
Cyc_CfFlowInfo_PlaceL_struct*) _temp427)->f1; goto _LL424;} else{ goto _LL421;}
_LL424: _temp423= _temp417.f2; if(( unsigned int) _temp423 >  1u?*(( int*)
_temp423) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL426: _temp425=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp423)->f1; goto _LL420;} else{ goto _LL421;} _LL421: goto _LL422; _LL420:
_temp410=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp431=( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct));
_temp431[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct _temp432; _temp432.tag=
Cyc_CfFlowInfo_ReachableFL; _temp432.f1= Cyc_CfFlowInfo_assign_place( _temp325->loc,
_temp425, env->all_changed, _temp429, Cyc_CfFlowInfo_unknown_all); _temp432;});
_temp431;}); goto _LL418; _LL422: goto _LL418; _LL418:;} return({ struct _tuple0
_temp433; _temp433.f1= _temp410; _temp433.f2= Cyc_CfFlowInfo_unknown_all;
_temp433;});}} _LL234: { void* _temp437; struct _tuple0 _temp435= Cyc_NewControlFlow_anal_use_ints(
env, inflow,({ struct Cyc_Absyn_Exp* _temp434[ 2u]; _temp434[ 1u]= _temp327;
_temp434[ 0u]= _temp332;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp434, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL438: _temp437=
_temp435.f1; goto _LL436; _LL436: { void* _temp441; struct _tuple0 _temp439= Cyc_NewControlFlow_anal_Lexp(
env, _temp437, e); _LL442: _temp441= _temp439.f2; goto _LL440; _LL440:{ struct
_tuple0 _temp444=({ struct _tuple0 _temp443; _temp443.f1= _temp441; _temp443.f2=
_temp437; _temp443;}); void* _temp450; struct Cyc_Dict_Dict* _temp452; void*
_temp454; struct Cyc_CfFlowInfo_Place* _temp456; _LL446: _LL455: _temp454=
_temp444.f1; if(( unsigned int) _temp454 >  1u?*(( int*) _temp454) ==  Cyc_CfFlowInfo_PlaceL:
0){ _LL457: _temp456=(( struct Cyc_CfFlowInfo_PlaceL_struct*) _temp454)->f1;
goto _LL451;} else{ goto _LL448;} _LL451: _temp450= _temp444.f2; if((
unsigned int) _temp450 >  1u?*(( int*) _temp450) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL453: _temp452=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp450)->f1;
goto _LL447;} else{ goto _LL448;} _LL448: goto _LL449; _LL447: _temp437=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp458=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp458[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp459; _temp459.tag= Cyc_CfFlowInfo_ReachableFL;
_temp459.f1= Cyc_CfFlowInfo_assign_place( _temp332->loc, _temp452, env->all_changed,
_temp456, Cyc_CfFlowInfo_unknown_all); _temp459;}); _temp458;}); goto _LL445;
_LL449: goto _LL445; _LL445:;} return({ struct _tuple0 _temp460; _temp460.f1=
_temp437; _temp460.f2= Cyc_CfFlowInfo_unknown_all; _temp460;});}} _LL236: {
struct Cyc_Set_Set** _temp461= env->all_changed; while( 1) { env->all_changed=({
struct Cyc_Set_Set** _temp462=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp462[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp462;});{ void*
_temp465; void* _temp467; struct _tuple0 _temp463= Cyc_NewControlFlow_anal_Lexp(
env, inflow, _temp338); _LL468: _temp467= _temp463.f1; goto _LL466; _LL466:
_temp465= _temp463.f2; goto _LL464; _LL464: { struct Cyc_Set_Set* _temp469=*((
struct Cyc_Set_Set**) _check_null( env->all_changed)); env->all_changed=({
struct Cyc_Set_Set** _temp470=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp470[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp470;});{ void*
_temp473; void* _temp475; struct _tuple0 _temp471= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp334); _LL476: _temp475= _temp471.f1; goto _LL474; _LL474:
_temp473= _temp471.f2; goto _LL472; _LL472: { struct Cyc_Set_Set* _temp477=*((
struct Cyc_Set_Set**) _check_null( env->all_changed)); void* _temp478= Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _temp469, _temp467, _temp475, _temp469, _temp477); void*
_temp479= Cyc_CfFlowInfo_join_flow( _temp461, inflow, _temp478); if( Cyc_CfFlowInfo_flow_lessthan_approx(
_temp479, inflow)){ if( _temp461 ==  0){ env->all_changed= 0;} else{*(( struct
Cyc_Set_Set**) _check_null( env->all_changed))=(( struct Cyc_Set_Set*(*)( struct
Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(* _temp461,((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
_temp469, _temp477));}{ void* _temp480= _temp478; struct Cyc_Dict_Dict* _temp486;
_LL482: if( _temp480 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL483;} else{
goto _LL484;} _LL484: if(( unsigned int) _temp480 >  1u?*(( int*) _temp480) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL487: _temp486=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp480)->f1; goto _LL485;} else{ goto _LL481;} _LL483: return({ struct _tuple0
_temp488; _temp488.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp488.f2= _temp473;
_temp488;}); _LL485: { void* _temp489= _temp465; struct Cyc_CfFlowInfo_Place*
_temp495; _LL491: if(( unsigned int) _temp489 >  1u?*(( int*) _temp489) ==  Cyc_CfFlowInfo_PlaceL:
0){ _LL496: _temp495=(( struct Cyc_CfFlowInfo_PlaceL_struct*) _temp489)->f1;
goto _LL492;} else{ goto _LL493;} _LL493: if( _temp489 == ( void*) Cyc_CfFlowInfo_UnknownL){
goto _LL494;} else{ goto _LL490;} _LL492: _temp486= Cyc_CfFlowInfo_assign_place(
e->loc, _temp486, env->all_changed, _temp495, _temp473); _temp478=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp497=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp497[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp498; _temp498.tag= Cyc_CfFlowInfo_ReachableFL;
_temp498.f1= _temp486; _temp498;}); _temp497;}); Cyc_NewControlFlow_update_tryflow(
env, _temp478); return({ struct _tuple0 _temp499; _temp499.f1= _temp478;
_temp499.f2= _temp473; _temp499;}); _LL494: return({ struct _tuple0 _temp500;
_temp500.f1= Cyc_NewControlFlow_use_Rval( env, _temp334->loc, _temp478, _temp473);
_temp500.f2= _temp473; _temp500;}); _LL490:;} _LL481:;}} inflow= _temp479;}}}}}}
_LL238: return Cyc_NewControlFlow_anal_Rexp( env,( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp342)).f1, _temp340); _LL240: { void* _temp503; void* _temp505;
struct _tuple0 _temp501= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp344);
_LL506: _temp505= _temp501.f1; goto _LL504; _LL504: _temp503= _temp501.f2; goto
_LL502; _LL502: Cyc_NewControlFlow_use_Rval( env, _temp344->loc, _temp505,
_temp503); return({ struct _tuple0 _temp507; _temp507.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp507.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp507;});} _LL242:
_temp346=({ struct Cyc_List_List* _temp508=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp508->hd=( void*) _temp348; _temp508->tl=
_temp346; _temp508;});{ struct Cyc_List_List* _temp511; void* _temp513; struct
_tuple4 _temp509= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp346);
_LL514: _temp513= _temp509.f1; goto _LL512; _LL512: _temp511= _temp509.f2; goto
_LL510; _LL510: for( 0; _temp511 !=  0;( _temp511= _temp511->tl, _temp346=((
struct Cyc_List_List*) _check_null( _temp346))->tl)){ _temp513= Cyc_NewControlFlow_use_Rval(
env,(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp346))->hd)->loc,
_temp513,( void*) _temp511->hd);} return({ struct _tuple0 _temp515; _temp515.f1=
_temp513; _temp515.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp515;});} _LL244: {
void* root=( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp534=( struct
Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp534[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp535; _temp535.tag=
Cyc_CfFlowInfo_MallocPt; _temp535.f1= e; _temp535;}); _temp534;}); struct Cyc_CfFlowInfo_Place*
place=({ struct Cyc_CfFlowInfo_Place* _temp533=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp533->root=( void*) root;
_temp533->fields= 0; _temp533;}); void* rval=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct*
_temp531=( struct Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_AddressOf_struct)); _temp531[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct
_temp532; _temp532.tag= Cyc_CfFlowInfo_AddressOf; _temp532.f1= place; _temp532;});
_temp531;}); void* place_val= Cyc_CfFlowInfo_typ_to_absrval( _temp350, Cyc_CfFlowInfo_unknown_none);
if( env->all_changed !=  0){*(( struct Cyc_Set_Set**) _check_null( env->all_changed))=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( env->all_changed)), place);}
if( _temp352 !=  0){ void* _temp516=( Cyc_NewControlFlow_anal_Rexp( env, inflow,(
struct Cyc_Absyn_Exp*) _check_null( _temp352))).f1; struct Cyc_Dict_Dict*
_temp522; _LL518: if( _temp516 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL519;}
else{ goto _LL520;} _LL520: if(( unsigned int) _temp516 >  1u?*(( int*) _temp516)
==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL523: _temp522=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp516)->f1; goto _LL521;} else{ goto _LL517;} _LL519: return({ struct _tuple0
_temp524; _temp524.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp524.f2= rval;
_temp524;}); _LL521: return({ struct _tuple0 _temp525; _temp525.f1=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp526=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp526[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp527; _temp527.tag= Cyc_CfFlowInfo_ReachableFL;
_temp527.f1= Cyc_Dict_insert( _temp522, root, place_val); _temp527;}); _temp526;});
_temp525.f2= rval; _temp525;}); _LL517:;} return({ struct _tuple0 _temp528;
_temp528.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp529=(
struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct));
_temp529[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct _temp530; _temp530.tag=
Cyc_CfFlowInfo_ReachableFL; _temp530.f1= Cyc_Dict_insert( d, root, place_val);
_temp530;}); _temp529;}); _temp528.f2= rval; _temp528;});} _LL246: { void* root=(
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp564=( struct Cyc_CfFlowInfo_MallocPt_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp564[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp565; _temp565.tag= Cyc_CfFlowInfo_MallocPt;
_temp565.f1= _temp354; _temp565;}); _temp564;}); struct Cyc_CfFlowInfo_Place*
place=({ struct Cyc_CfFlowInfo_Place* _temp563=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp563->root=( void*) root;
_temp563->fields= 0; _temp563;}); void* rval=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct*
_temp561=( struct Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_AddressOf_struct)); _temp561[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct
_temp562; _temp562.tag= Cyc_CfFlowInfo_AddressOf; _temp562.f1= place; _temp562;});
_temp561;}); if( env->all_changed !=  0){*(( struct Cyc_Set_Set**) _check_null(
env->all_changed))=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( env->all_changed)),
place);}{ void* outflow; void* place_val; if( _temp356 !=  0){ struct Cyc_List_List*
_temp539; void* _temp541; struct _tuple4 _temp537= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp536[ 2u]; _temp536[ 1u]= _temp354;
_temp536[ 0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp356);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp536, sizeof( struct Cyc_Absyn_Exp*),
2u));})); _LL542: _temp541= _temp537.f1; goto _LL540; _LL540: _temp539= _temp537.f2;
goto _LL538; _LL538: outflow= _temp541; place_val=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp539))->tl))->hd;} else{
void* _temp545; void* _temp547; struct _tuple0 _temp543= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp354); _LL548: _temp547= _temp543.f1; goto _LL546; _LL546:
_temp545= _temp543.f2; goto _LL544; _LL544: outflow= _temp547; place_val=
_temp545;}{ void* _temp549= outflow; struct Cyc_Dict_Dict* _temp555; _LL551: if(
_temp549 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL552;} else{ goto _LL553;}
_LL553: if(( unsigned int) _temp549 >  1u?*(( int*) _temp549) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL556: _temp555=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp549)->f1;
goto _LL554;} else{ goto _LL550;} _LL552: return({ struct _tuple0 _temp557;
_temp557.f1= outflow; _temp557.f2= rval; _temp557;}); _LL554: return({ struct
_tuple0 _temp558; _temp558.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp559=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp559[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp560; _temp560.tag= Cyc_CfFlowInfo_ReachableFL; _temp560.f1= Cyc_Dict_insert(
_temp555, root, place_val); _temp560;}); _temp559;}); _temp558.f2= rval;
_temp558;}); _LL550:;}}} _LL248: { void* _temp568; void* _temp570; struct
_tuple0 _temp566= Cyc_NewControlFlow_anal_Lexp( env, inflow, _temp358); _LL571:
_temp570= _temp566.f1; goto _LL569; _LL569: _temp568= _temp566.f2; goto _LL567;
_LL567: { void* _temp572= _temp568; struct Cyc_CfFlowInfo_Place* _temp578;
_LL574: if( _temp572 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL575;} else{
goto _LL576;} _LL576: if(( unsigned int) _temp572 >  1u?*(( int*) _temp572) == 
Cyc_CfFlowInfo_PlaceL: 0){ _LL579: _temp578=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp572)->f1; goto _LL577;} else{ goto _LL573;} _LL575: return({ struct _tuple0
_temp580; _temp580.f1= _temp570; _temp580.f2=( void*) Cyc_CfFlowInfo_NotZeroAll;
_temp580;}); _LL577: return({ struct _tuple0 _temp581; _temp581.f1= _temp570;
_temp581.f2=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct* _temp582=( struct
Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_AddressOf_struct));
_temp582[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct _temp583; _temp583.tag=
Cyc_CfFlowInfo_AddressOf; _temp583.f1= _temp578; _temp583;}); _temp582;});
_temp581;}); _LL573:;}} _LL250: { void* _temp586; void* _temp588; struct _tuple0
_temp584= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp360); _LL589: _temp588=
_temp584.f1; goto _LL587; _LL587: _temp586= _temp584.f2; goto _LL585; _LL585:
return Cyc_NewControlFlow_anal_derefR( env, inflow, _temp588, _temp360, _temp586);}
_LL252: { void* _temp592; void* _temp594; struct _tuple0 _temp590= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp364); _LL595: _temp594= _temp590.f1; goto _LL593; _LL593:
_temp592= _temp590.f2; goto _LL591; _LL591:{ void* _temp596= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp364->topt))->v); _LL598: if((
unsigned int) _temp596 >  4u?*(( int*) _temp596) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL599;} else{ goto _LL600;} _LL600: if(( unsigned int) _temp596 >  4u?*((
int*) _temp596) ==  Cyc_Absyn_UnionType: 0){ goto _LL601;} else{ goto _LL602;}
_LL602: goto _LL603; _LL599: goto _LL601; _LL601: return({ struct _tuple0
_temp604; _temp604.f1= _temp594; _temp604.f2= _temp592; _temp604;}); _LL603:
goto _LL597; _LL597:;}{ void* _temp605= _temp592; struct Cyc_Dict_Dict* _temp611;
_LL607: if(( unsigned int) _temp605 >  3u?*(( int*) _temp605) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL612: _temp611=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp605)->f1;
goto _LL608;} else{ goto _LL609;} _LL609: goto _LL610; _LL608: return({ struct
_tuple0 _temp613; _temp613.f1= _temp594; _temp613.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp611, _temp362); _temp613;});
_LL610:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp614=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp614[ 0]=({ struct Cyc_Core_Impossible_struct _temp615; _temp615.tag= Cyc_Core_Impossible;
_temp615.f1= _tag_arr("anal_Rexp: StructMember", sizeof( unsigned char), 24u);
_temp615;}); _temp614;})); _LL606:;}} _LL254: { void* _temp618; void* _temp620;
struct _tuple0 _temp616= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp368);
_LL621: _temp620= _temp616.f1; goto _LL619; _LL619: _temp618= _temp616.f2; goto
_LL617; _LL617: { void* _temp624; void* _temp626; struct _tuple0 _temp622= Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp620, _temp368, _temp618); _LL627: _temp626= _temp622.f1; goto
_LL625; _LL625: _temp624= _temp622.f2; goto _LL623; _LL623:{ void* _temp628= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp368->topt))->v); _LL630: if((
unsigned int) _temp628 >  4u?*(( int*) _temp628) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL631;} else{ goto _LL632;} _LL632: if(( unsigned int) _temp628 >  4u?*((
int*) _temp628) ==  Cyc_Absyn_UnionType: 0){ goto _LL633;} else{ goto _LL634;}
_LL634: goto _LL635; _LL631: goto _LL633; _LL633: return({ struct _tuple0
_temp636; _temp636.f1= _temp626; _temp636.f2= _temp624; _temp636;}); _LL635:
goto _LL629; _LL629:;}{ void* _temp637= _temp624; struct Cyc_Dict_Dict* _temp643;
_LL639: if(( unsigned int) _temp637 >  3u?*(( int*) _temp637) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL644: _temp643=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp637)->f1;
goto _LL640;} else{ goto _LL641;} _LL641: goto _LL642; _LL640: return({ struct
_tuple0 _temp645; _temp645.f1= _temp626; _temp645.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp643, _temp366); _temp645;});
_LL642:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp646=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp646[ 0]=({ struct Cyc_Core_Impossible_struct _temp647; _temp647.tag= Cyc_Core_Impossible;
_temp647.f1= _tag_arr("anal_Rexp: StructArrow", sizeof( unsigned char), 23u);
_temp647;}); _temp646;})); _LL638:;}}} _LL256: { void* _temp650; void* _temp652;
struct _tuple0 _temp648= Cyc_NewControlFlow_anal_test( env, inflow, _temp374);
_LL653: _temp652= _temp648.f1; goto _LL651; _LL651: _temp650= _temp648.f2; goto
_LL649; _LL649: { void* _temp656; void* _temp658; struct _tuple0 _temp654= Cyc_NewControlFlow_anal_Rexp(
env, _temp652, _temp372); _LL659: _temp658= _temp654.f1; goto _LL657; _LL657:
_temp656= _temp654.f2; goto _LL655; _LL655: { void* _temp662; void* _temp664;
struct _tuple0 _temp660= Cyc_NewControlFlow_anal_Rexp( env, _temp650, _temp370);
_LL665: _temp664= _temp660.f1; goto _LL663; _LL663: _temp662= _temp660.f2; goto
_LL661; _LL661: return({ struct _tuple0 _temp666; _temp666.f1= Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_use_Rval( env, _temp372->loc, _temp658,
_temp656), Cyc_NewControlFlow_use_Rval( env, _temp370->loc, _temp664, _temp662));
_temp666.f2= Cyc_CfFlowInfo_unknown_all; _temp666;});}}} _LL258: { struct Cyc_List_List*
_temp670; void* _temp672; struct _tuple4 _temp668= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp667[ 2u]; _temp667[ 1u]= _temp376;
_temp667[ 0u]= _temp378;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp667, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL673: _temp672=
_temp668.f1; goto _LL671; _LL671: _temp670= _temp668.f2; goto _LL669; _LL669:{
void* _temp674= _temp672; struct Cyc_Dict_Dict* _temp680; _LL676: if((
unsigned int) _temp674 >  1u?*(( int*) _temp674) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL681: _temp680=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp674)->f1;
goto _LL677;} else{ goto _LL678;} _LL678: goto _LL679; _LL677: if( Cyc_CfFlowInfo_initlevel(
_temp680,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp670))->tl))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp682[ 0u]={}; Cyc_Tcutil_terr( _temp376->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp682, sizeof( void*), 0u));});} goto
_LL675; _LL679: goto _LL675; _LL675:;}{ void* _temp683= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp378->topt))->v); _LL685: if((
unsigned int) _temp683 >  4u?*(( int*) _temp683) ==  Cyc_Absyn_TupleType: 0){
goto _LL686;} else{ goto _LL687;} _LL687: goto _LL688; _LL686: { void* _temp689=(
void*)(( struct Cyc_List_List*) _check_null( _temp670))->hd; struct Cyc_Dict_Dict*
_temp695; _LL691: if(( unsigned int) _temp689 >  3u?*(( int*) _temp689) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL696: _temp695=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp689)->f1;
goto _LL692;} else{ goto _LL693;} _LL693: goto _LL694; _LL692: return({ struct
_tuple0 _temp697; _temp697.f1= _temp672; _temp697.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp695, Cyc_Absyn_fieldname(( int)
Cyc_Evexp_eval_const_uint_exp( _temp376))); _temp697;}); _LL694:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp698=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp698[ 0]=({ struct
Cyc_Core_Impossible_struct _temp699; _temp699.tag= Cyc_Core_Impossible; _temp699.f1=
_tag_arr("anal_Rexp: Subscript", sizeof( unsigned char), 21u); _temp699;});
_temp698;})); _LL690:;} _LL688: goto _LL684; _LL684:;} return Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp672, _temp378,( void*)(( struct Cyc_List_List*) _check_null(
_temp670))->hd);} _LL260: _temp382= _temp380; goto _LL262; _LL262: { struct Cyc_List_List*
_temp702; void* _temp704; struct _tuple4 _temp700= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp382); _LL705: _temp704= _temp700.f1; goto _LL703; _LL703:
_temp702= _temp700.f2; goto _LL701; _LL701: { struct Cyc_Dict_Dict* aggrdict=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int i= 0; for( 0; _temp702 !=  0;(
_temp702= _temp702->tl, ++ i)){ aggrdict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict, Cyc_Absyn_fieldname(
i),( void*) _temp702->hd);}} return({ struct _tuple0 _temp706; _temp706.f1=
_temp704; _temp706.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp707=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp707[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp708; _temp708.tag= Cyc_CfFlowInfo_Aggregate; _temp708.f1= aggrdict;
_temp708;}); _temp707;}); _temp706;});}} _LL264: _temp386= _temp384; goto _LL266;
_LL266: { struct Cyc_List_List* _temp711; void* _temp713; struct _tuple4
_temp709= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp386)); _LL714:
_temp713= _temp709.f1; goto _LL712; _LL712: _temp711= _temp709.f2; goto _LL710;
_LL710: { struct Cyc_Dict_Dict* aggrdict=(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{
int i= 0; for( 0; _temp711 !=  0;((( _temp711= _temp711->tl, _temp386= _temp386->tl)),
++ i)){ struct Cyc_List_List* ds=(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp386))->hd)).f1; for( 0; ds !=  0; ds= ds->tl){ void* _temp715=(
void*) ds->hd; struct _tagged_arr* _temp721; _LL717: if(*(( int*) _temp715) == 
Cyc_Absyn_ArrayElement){ goto _LL718;} else{ goto _LL719;} _LL719: if(*(( int*)
_temp715) ==  Cyc_Absyn_FieldName){ _LL722: _temp721=(( struct Cyc_Absyn_FieldName_struct*)
_temp715)->f1; goto _LL720;} else{ goto _LL716;} _LL718:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp723=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp723[ 0]=({ struct
Cyc_Core_Impossible_struct _temp724; _temp724.tag= Cyc_Core_Impossible; _temp724.f1=
_tag_arr("anal_Rexp:Struct_e", sizeof( unsigned char), 19u); _temp724;});
_temp723;})); _LL720: aggrdict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict, _temp721,( void*)
_temp711->hd); _LL716:;}}} return({ struct _tuple0 _temp725; _temp725.f1=
_temp713; _temp725.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp726=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp726[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp727; _temp727.tag= Cyc_CfFlowInfo_Aggregate; _temp727.f1= aggrdict;
_temp727;}); _temp726;}); _temp725;});}} _LL268: { struct Cyc_List_List*
_temp728=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple5*)) Cyc_Core_snd, _temp388); struct Cyc_List_List* _temp731; void*
_temp733; struct _tuple4 _temp729= Cyc_NewControlFlow_anal_unordered_Rexps( env,
inflow, _temp728); _LL734: _temp733= _temp729.f1; goto _LL732; _LL732: _temp731=
_temp729.f2; goto _LL730; _LL730: for( 0; _temp731 !=  0;( _temp731= _temp731->tl,
_temp728= _temp728->tl)){ _temp733= Cyc_NewControlFlow_use_Rval( env,(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp728))->hd)->loc,
_temp733,( void*) _temp731->hd);} return({ struct _tuple0 _temp735; _temp735.f1=
_temp733; _temp735.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp735;});} _LL270: {
void* _temp738; void* _temp740; struct _tuple0 _temp736= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp392); _LL741: _temp740= _temp736.f1; goto _LL739; _LL739:
_temp738= _temp736.f2; goto _LL737; _LL737: { void* _temp742= _temp740; struct
Cyc_Dict_Dict* _temp748; _LL744: if( _temp742 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL745;} else{ goto _LL746;} _LL746: if(( unsigned int) _temp742 >  1u?*((
int*) _temp742) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL749: _temp748=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp742)->f1; goto _LL747;} else{ goto
_LL743;} _LL745: return({ struct _tuple0 _temp750; _temp750.f1= _temp740;
_temp750.f2= Cyc_CfFlowInfo_unknown_all; _temp750;}); _LL747: if( Cyc_CfFlowInfo_initlevel(
_temp748, _temp738) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp751[ 0u]={};
Cyc_Tcutil_terr( _temp392->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp751, sizeof( void*), 0u));});}{
void* _temp752= _temp738; _LL754: if( _temp752 == ( void*) Cyc_CfFlowInfo_Zero){
goto _LL755;} else{ goto _LL756;} _LL756: if( _temp752 == ( void*) Cyc_CfFlowInfo_NotZeroThis){
goto _LL757;} else{ goto _LL758;} _LL758: if( _temp752 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL759;} else{ goto _LL760;} _LL760: if(( unsigned int) _temp752 >  3u?*((
int*) _temp752) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL761;} else{ goto
_LL762;} _LL762: goto _LL763; _LL755: return({ struct _tuple0 _temp764; _temp764.f1=
_temp740; _temp764.f2= Cyc_CfFlowInfo_unknown_all; _temp764;}); _LL757: goto
_LL759; _LL759: goto _LL761; _LL761: { struct Cyc_List_List _temp765=({ struct
Cyc_List_List _temp783; _temp783.hd=( void*) _temp394; _temp783.tl= 0; _temp783;});
_temp740= Cyc_NewControlFlow_add_vars( _temp740,( struct Cyc_List_List*)&
_temp765, Cyc_CfFlowInfo_unknown_all);{ void* _temp768; void* _temp770; struct
_tuple0 _temp766= Cyc_NewControlFlow_anal_Rexp( env, _temp740, _temp390); _LL771:
_temp770= _temp766.f1; goto _LL769; _LL769: _temp768= _temp766.f2; goto _LL767;
_LL767:{ void* _temp772= _temp770; struct Cyc_Dict_Dict* _temp778; _LL774: if(
_temp772 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL775;} else{ goto _LL776;}
_LL776: if(( unsigned int) _temp772 >  1u?*(( int*) _temp772) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL779: _temp778=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp772)->f1;
goto _LL777;} else{ goto _LL773;} _LL775: return({ struct _tuple0 _temp780;
_temp780.f1= _temp770; _temp780.f2= Cyc_CfFlowInfo_unknown_all; _temp780;});
_LL777: if( Cyc_CfFlowInfo_initlevel( _temp778, _temp768) != ( void*) Cyc_CfFlowInfo_AllIL){({
void* _temp781[ 0u]={}; Cyc_Tcutil_terr( _temp392->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp781, sizeof( void*), 0u));});
return({ struct _tuple0 _temp782; _temp782.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp782.f2= Cyc_CfFlowInfo_unknown_all; _temp782;});} _LL773:;} _temp740=
_temp770; goto _LL763;}} _LL763: while( 1) { struct Cyc_List_List _temp784=({
struct Cyc_List_List _temp802; _temp802.hd=( void*) _temp394; _temp802.tl= 0;
_temp802;}); _temp740= Cyc_NewControlFlow_add_vars( _temp740,( struct Cyc_List_List*)&
_temp784, Cyc_CfFlowInfo_unknown_all);{ void* _temp787; void* _temp789; struct
_tuple0 _temp785= Cyc_NewControlFlow_anal_Rexp( env, _temp740, _temp390); _LL790:
_temp789= _temp785.f1; goto _LL788; _LL788: _temp787= _temp785.f2; goto _LL786;
_LL786:{ void* _temp791= _temp789; struct Cyc_Dict_Dict* _temp797; _LL793: if(
_temp791 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL794;} else{ goto _LL795;}
_LL795: if(( unsigned int) _temp791 >  1u?*(( int*) _temp791) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL798: _temp797=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp791)->f1;
goto _LL796;} else{ goto _LL792;} _LL794: goto _LL792; _LL796: if( Cyc_CfFlowInfo_initlevel(
_temp797, _temp787) != ( void*) Cyc_CfFlowInfo_AllIL){({ void* _temp799[ 0u]={};
Cyc_Tcutil_terr( _temp392->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp799, sizeof( void*), 0u));});
return({ struct _tuple0 _temp800; _temp800.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp800.f2= Cyc_CfFlowInfo_unknown_all; _temp800;});} _LL792:;}{ void* _temp801=
Cyc_CfFlowInfo_join_flow( env->all_changed, _temp740, _temp789); if( Cyc_CfFlowInfo_flow_lessthan_approx(
_temp801, _temp740)){ break;} _temp740= _temp801;}}} return({ struct _tuple0
_temp803; _temp803.f1= _temp740; _temp803.f2= Cyc_CfFlowInfo_unknown_all;
_temp803;}); _LL753:;} _LL743:;}} _LL272: while( 1) { void* _temp804=( void*)
_temp396->r; struct Cyc_Absyn_Stmt* _temp814; struct Cyc_Absyn_Stmt* _temp816;
struct Cyc_Absyn_Stmt* _temp818; struct Cyc_Absyn_Decl* _temp820; struct Cyc_Absyn_Exp*
_temp822; _LL806: if(( unsigned int) _temp804 >  1u?*(( int*) _temp804) ==  Cyc_Absyn_Seq_s:
0){ _LL817: _temp816=(( struct Cyc_Absyn_Seq_s_struct*) _temp804)->f1; goto
_LL815; _LL815: _temp814=(( struct Cyc_Absyn_Seq_s_struct*) _temp804)->f2; goto
_LL807;} else{ goto _LL808;} _LL808: if(( unsigned int) _temp804 >  1u?*(( int*)
_temp804) ==  Cyc_Absyn_Decl_s: 0){ _LL821: _temp820=(( struct Cyc_Absyn_Decl_s_struct*)
_temp804)->f1; goto _LL819; _LL819: _temp818=(( struct Cyc_Absyn_Decl_s_struct*)
_temp804)->f2; goto _LL809;} else{ goto _LL810;} _LL810: if(( unsigned int)
_temp804 >  1u?*(( int*) _temp804) ==  Cyc_Absyn_Exp_s: 0){ _LL823: _temp822=((
struct Cyc_Absyn_Exp_s_struct*) _temp804)->f1; goto _LL811;} else{ goto _LL812;}
_LL812: goto _LL813; _LL807: inflow= Cyc_NewControlFlow_anal_stmt( env, inflow,
_temp816); _temp396= _temp814; continue; _LL809: inflow= Cyc_NewControlFlow_anal_decl(
env, inflow, _temp820); _temp396= _temp818; continue; _LL811: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp822); _LL813:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp824=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp824[ 0]=({ struct Cyc_Core_Impossible_struct _temp825; _temp825.tag= Cyc_Core_Impossible;
_temp825.f1= _tag_arr("analyze_Rexp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp825;}); _temp824;})); _LL805:;} _LL274: goto _LL276; _LL276: goto
_LL278; _LL278: goto _LL280; _LL280: goto _LL282; _LL282: goto _LL284; _LL284:
goto _LL286; _LL286:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp826=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp826[ 0]=({ struct Cyc_Core_Impossible_struct _temp827; _temp827.tag= Cyc_Core_Impossible;
_temp827.f1= _tag_arr("anal_Rexp, unexpected exp form", sizeof( unsigned char),
31u); _temp827;}); _temp826;})); _LL190:;}} static struct _tuple0 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* f, struct Cyc_Absyn_Exp*
e, void* r, struct Cyc_List_List* flds){ void* _temp828= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo
_temp834; struct Cyc_Absyn_Conref* _temp836; void* _temp838; _LL830: if((
unsigned int) _temp828 >  4u?*(( int*) _temp828) ==  Cyc_Absyn_PointerType: 0){
_LL835: _temp834=(( struct Cyc_Absyn_PointerType_struct*) _temp828)->f1; _LL839:
_temp838=( void*) _temp834.elt_typ; goto _LL837; _LL837: _temp836= _temp834.bounds;
goto _LL831;} else{ goto _LL832;} _LL832: goto _LL833; _LL831: { void* _temp840=
f; struct Cyc_Dict_Dict* _temp846; _LL842: if( _temp840 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL843;} else{ goto _LL844;} _LL844: if(( unsigned int) _temp840 >  1u?*((
int*) _temp840) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL847: _temp846=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp840)->f1; goto _LL845;} else{ goto
_LL841;} _LL843: return({ struct _tuple0 _temp848; _temp848.f1= f; _temp848.f2=(
void*) Cyc_CfFlowInfo_UnknownL; _temp848;}); _LL845: if( Cyc_Tcutil_is_bound_one(
_temp836)){ void* _temp849= r; struct Cyc_CfFlowInfo_Place* _temp863; struct Cyc_CfFlowInfo_Place
_temp865; struct Cyc_List_List* _temp866; void* _temp868; void* _temp870; _LL851:
if( _temp849 == ( void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL852;} else{ goto
_LL853;} _LL853: if( _temp849 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto
_LL854;} else{ goto _LL855;} _LL855: if(( unsigned int) _temp849 >  3u?*(( int*)
_temp849) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL864: _temp863=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp849)->f1; _temp865=* _temp863; _LL869: _temp868=( void*) _temp865.root;
goto _LL867; _LL867: _temp866= _temp865.fields; goto _LL856;} else{ goto _LL857;}
_LL857: if( _temp849 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL858;} else{ goto
_LL859;} _LL859: if(( unsigned int) _temp849 >  3u?*(( int*) _temp849) ==  Cyc_CfFlowInfo_UnknownR:
0){ _LL871: _temp870=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp849)->f1;
goto _LL860;} else{ goto _LL861;} _LL861: goto _LL862; _LL852: goto _LL854;
_LL854:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero)); goto
_LL850; _LL856:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero));
return({ struct _tuple0 _temp872; _temp872.f1= f; _temp872.f2=( void*)({ struct
Cyc_CfFlowInfo_PlaceL_struct* _temp873=( struct Cyc_CfFlowInfo_PlaceL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct)); _temp873[ 0]=({ struct
Cyc_CfFlowInfo_PlaceL_struct _temp874; _temp874.tag= Cyc_CfFlowInfo_PlaceL;
_temp874.f1=({ struct Cyc_CfFlowInfo_Place* _temp875=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp875->root=( void*)
_temp868; _temp875->fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp866, flds); _temp875;});
_temp874;}); _temp873;}); _temp872;}); _LL858:( void*)( e->annot=( void*)(( void*)
Cyc_CfFlowInfo_IsZero)); return({ struct _tuple0 _temp876; _temp876.f1=( void*)
Cyc_CfFlowInfo_BottomFL; _temp876.f2=( void*) Cyc_CfFlowInfo_UnknownL; _temp876;});
_LL860: f= Cyc_NewControlFlow_notzero( env, inflow, f, e, _temp870); goto _LL862;
_LL862:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_UnknownZ)); _LL850:;}
else{( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_UnknownZ));} if( Cyc_CfFlowInfo_initlevel(
_temp846, r) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp877[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("dereference of possibly uninitialized pointer", sizeof(
unsigned char), 46u), _tag_arr( _temp877, sizeof( void*), 0u));});} return({
struct _tuple0 _temp878; _temp878.f1= f; _temp878.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp878;}); _LL841:;} _LL833:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp879=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp879[ 0]=({ struct Cyc_Core_Impossible_struct _temp880; _temp880.tag= Cyc_Core_Impossible;
_temp880.f1= _tag_arr("left deref of non-pointer-type", sizeof( unsigned char),
31u); _temp880;}); _temp879;})); _LL829:;} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* flds){ struct Cyc_Dict_Dict* d;{ void* _temp881= inflow;
struct Cyc_Dict_Dict* _temp887; _LL883: if( _temp881 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL884;} else{ goto _LL885;} _LL885: if(( unsigned int) _temp881 >  1u?*((
int*) _temp881) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL888: _temp887=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp881)->f1; goto _LL886;} else{ goto
_LL882;} _LL884: return({ struct _tuple0 _temp889; _temp889.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp889.f2=( void*) Cyc_CfFlowInfo_UnknownL; _temp889;}); _LL886: d= _temp887;
_LL882:;}{ void* _temp890=( void*) e->r; struct Cyc_Absyn_Exp* _temp916; struct
Cyc_Absyn_Exp* _temp918; struct Cyc_Absyn_Exp* _temp920; void* _temp922; struct
Cyc_Absyn_Vardecl* _temp924; void* _temp926; struct Cyc_Absyn_Vardecl* _temp928;
void* _temp930; struct Cyc_Absyn_Vardecl* _temp932; void* _temp934; struct Cyc_Absyn_Vardecl*
_temp936; struct _tagged_arr* _temp938; struct Cyc_Absyn_Exp* _temp940; struct
Cyc_Absyn_Exp* _temp942; struct Cyc_Absyn_Exp* _temp944; struct Cyc_Absyn_Exp*
_temp946; struct _tagged_arr* _temp948; struct Cyc_Absyn_Exp* _temp950; _LL892:
if(*(( int*) _temp890) ==  Cyc_Absyn_Cast_e){ _LL917: _temp916=(( struct Cyc_Absyn_Cast_e_struct*)
_temp890)->f2; goto _LL893;} else{ goto _LL894;} _LL894: if(*(( int*) _temp890)
==  Cyc_Absyn_NoInstantiate_e){ _LL919: _temp918=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp890)->f1; goto _LL895;} else{ goto _LL896;} _LL896: if(*(( int*) _temp890)
==  Cyc_Absyn_Instantiate_e){ _LL921: _temp920=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp890)->f1; goto _LL897;} else{ goto _LL898;} _LL898: if(*(( int*) _temp890)
==  Cyc_Absyn_Var_e){ _LL923: _temp922=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp890)->f2; if(( unsigned int) _temp922 >  1u?*(( int*) _temp922) ==  Cyc_Absyn_Param_b:
0){ _LL925: _temp924=(( struct Cyc_Absyn_Param_b_struct*) _temp922)->f1; goto
_LL899;} else{ goto _LL900;}} else{ goto _LL900;} _LL900: if(*(( int*) _temp890)
==  Cyc_Absyn_Var_e){ _LL927: _temp926=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp890)->f2; if(( unsigned int) _temp926 >  1u?*(( int*) _temp926) ==  Cyc_Absyn_Local_b:
0){ _LL929: _temp928=(( struct Cyc_Absyn_Local_b_struct*) _temp926)->f1; goto
_LL901;} else{ goto _LL902;}} else{ goto _LL902;} _LL902: if(*(( int*) _temp890)
==  Cyc_Absyn_Var_e){ _LL931: _temp930=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp890)->f2; if(( unsigned int) _temp930 >  1u?*(( int*) _temp930) ==  Cyc_Absyn_Pat_b:
0){ _LL933: _temp932=(( struct Cyc_Absyn_Pat_b_struct*) _temp930)->f1; goto
_LL903;} else{ goto _LL904;}} else{ goto _LL904;} _LL904: if(*(( int*) _temp890)
==  Cyc_Absyn_Var_e){ _LL935: _temp934=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp890)->f2; if(( unsigned int) _temp934 >  1u?*(( int*) _temp934) ==  Cyc_Absyn_Global_b:
0){ _LL937: _temp936=(( struct Cyc_Absyn_Global_b_struct*) _temp934)->f1; goto
_LL905;} else{ goto _LL906;}} else{ goto _LL906;} _LL906: if(*(( int*) _temp890)
==  Cyc_Absyn_StructArrow_e){ _LL941: _temp940=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp890)->f1; goto _LL939; _LL939: _temp938=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp890)->f2; goto _LL907;} else{ goto _LL908;} _LL908: if(*(( int*) _temp890)
==  Cyc_Absyn_Deref_e){ _LL943: _temp942=(( struct Cyc_Absyn_Deref_e_struct*)
_temp890)->f1; goto _LL909;} else{ goto _LL910;} _LL910: if(*(( int*) _temp890)
==  Cyc_Absyn_Subscript_e){ _LL947: _temp946=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp890)->f1; goto _LL945; _LL945: _temp944=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp890)->f2; goto _LL911;} else{ goto _LL912;} _LL912: if(*(( int*) _temp890)
==  Cyc_Absyn_StructMember_e){ _LL951: _temp950=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp890)->f1; goto _LL949; _LL949: _temp948=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp890)->f2; goto _LL913;} else{ goto _LL914;} _LL914: goto _LL915; _LL893:
_temp918= _temp916; goto _LL895; _LL895: _temp920= _temp918; goto _LL897; _LL897:
return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, _temp920, flds); _LL899:
_temp928= _temp924; goto _LL901; _LL901: _temp932= _temp928; goto _LL903; _LL903:
return({ struct _tuple0 _temp952; _temp952.f1= inflow; _temp952.f2=( void*)({
struct Cyc_CfFlowInfo_PlaceL_struct* _temp953=( struct Cyc_CfFlowInfo_PlaceL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct)); _temp953[ 0]=({ struct
Cyc_CfFlowInfo_PlaceL_struct _temp954; _temp954.tag= Cyc_CfFlowInfo_PlaceL;
_temp954.f1=({ struct Cyc_CfFlowInfo_Place* _temp955=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp955->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp956=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp956[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp957; _temp957.tag= Cyc_CfFlowInfo_VarRoot;
_temp957.f1= _temp932; _temp957;}); _temp956;})); _temp955->fields= flds;
_temp955;}); _temp954;}); _temp953;}); _temp952;}); _LL905: return({ struct
_tuple0 _temp958; _temp958.f1= inflow; _temp958.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp958;}); _LL907:{ void* _temp959= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp940->topt))->v); _LL961: if(( unsigned int) _temp959 >  4u?*((
int*) _temp959) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL962;} else{ goto _LL963;}
_LL963: if(( unsigned int) _temp959 >  4u?*(( int*) _temp959) ==  Cyc_Absyn_UnionType:
0){ goto _LL964;} else{ goto _LL965;} _LL965: goto _LL966; _LL962: goto _LL964;
_LL964: goto _LL960; _LL966: flds=({ struct Cyc_List_List* _temp967=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp967->hd=( void*) _temp938;
_temp967->tl= flds; _temp967;}); _LL960:;} _temp942= _temp940; goto _LL909;
_LL909: { void* _temp970; void* _temp972; struct _tuple0 _temp968= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp942); _LL973: _temp972= _temp968.f1; goto _LL971; _LL971:
_temp970= _temp968.f2; goto _LL969; _LL969: return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp972, _temp942, _temp970, flds);} _LL911:{ void* _temp974= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp946->topt))->v); _LL976: if((
unsigned int) _temp974 >  4u?*(( int*) _temp974) ==  Cyc_Absyn_TupleType: 0){
goto _LL977;} else{ goto _LL978;} _LL978: goto _LL979; _LL977: { struct
_tagged_arr* _temp980= Cyc_Absyn_fieldname(( int) Cyc_Evexp_eval_const_uint_exp(
_temp944)); return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, _temp946,({
struct Cyc_List_List* _temp981=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp981->hd=( void*) _temp980; _temp981->tl= flds;
_temp981;}));} _LL979: goto _LL975; _LL975:;}{ struct Cyc_List_List* _temp985;
void* _temp987; struct _tuple4 _temp983= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp982[ 2u]; _temp982[ 1u]= _temp944;
_temp982[ 0u]= _temp946;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp982, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL988: _temp987=
_temp983.f1; goto _LL986; _LL986: _temp985= _temp983.f2; goto _LL984; _LL984:{
void* _temp989= _temp987; struct Cyc_Dict_Dict* _temp995; _LL991: if((
unsigned int) _temp989 >  1u?*(( int*) _temp989) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL996: _temp995=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp989)->f1;
goto _LL992;} else{ goto _LL993;} _LL993: goto _LL994; _LL992: if( Cyc_CfFlowInfo_initlevel(
_temp995,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp985))->tl))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp997[ 0u]={}; Cyc_Tcutil_terr( _temp944->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp997, sizeof( void*), 0u));});} goto
_LL990; _LL994: goto _LL990; _LL990:;} return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp987, _temp946,( void*)(( struct Cyc_List_List*) _check_null(
_temp985))->hd, flds);} _LL913:{ void* _temp998= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp950->topt))->v); _LL1000: if((
unsigned int) _temp998 >  4u?*(( int*) _temp998) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL1001;} else{ goto _LL1002;} _LL1002: if(( unsigned int) _temp998 >  4u?*((
int*) _temp998) ==  Cyc_Absyn_UnionType: 0){ goto _LL1003;} else{ goto _LL1004;}
_LL1004: goto _LL1005; _LL1001: goto _LL1003; _LL1003: goto _LL999; _LL1005:
flds=({ struct Cyc_List_List* _temp1006=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1006->hd=( void*) _temp948; _temp1006->tl=
flds; _temp1006;}); _LL999:;} return Cyc_NewControlFlow_anal_Lexp_rec( env,
inflow, _temp950, flds); _LL915: return({ struct _tuple0 _temp1007; _temp1007.f1=(
void*) Cyc_CfFlowInfo_BottomFL; _temp1007.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1007;}); _LL891:;}} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e){
return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, e, 0);} static struct
_tuple0 Cyc_NewControlFlow_anal_test( struct Cyc_NewControlFlow_AnalEnv* env,
void* inflow, struct Cyc_Absyn_Exp* e){{ void* _temp1008=( void*) e->r; struct
Cyc_Absyn_Exp* _temp1020; struct Cyc_Absyn_Exp* _temp1022; struct Cyc_Absyn_Exp*
_temp1024; struct Cyc_List_List* _temp1026; struct Cyc_List_List _temp1028;
struct Cyc_List_List* _temp1029; struct Cyc_Absyn_Exp* _temp1031; void*
_temp1033; struct Cyc_List_List* _temp1035; void* _temp1037; struct Cyc_List_List*
_temp1039; void* _temp1041; _LL1010: if(*(( int*) _temp1008) ==  Cyc_Absyn_Conditional_e){
_LL1025: _temp1024=(( struct Cyc_Absyn_Conditional_e_struct*) _temp1008)->f1;
goto _LL1023; _LL1023: _temp1022=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1008)->f2; goto _LL1021; _LL1021: _temp1020=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1008)->f3; goto _LL1011;} else{ goto _LL1012;} _LL1012: if(*(( int*)
_temp1008) ==  Cyc_Absyn_Primop_e){ _LL1034: _temp1033=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1008)->f1; if( _temp1033 == ( void*) Cyc_Absyn_Not){ goto _LL1027;} else{
goto _LL1014;} _LL1027: _temp1026=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1008)->f2; if( _temp1026 ==  0){ goto _LL1014;} else{ _temp1028=* _temp1026;
_LL1032: _temp1031=( struct Cyc_Absyn_Exp*) _temp1028.hd; goto _LL1030; _LL1030:
_temp1029= _temp1028.tl; if( _temp1029 ==  0){ goto _LL1013;} else{ goto _LL1014;}}}
else{ goto _LL1014;} _LL1014: if(*(( int*) _temp1008) ==  Cyc_Absyn_Primop_e){
_LL1038: _temp1037=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1008)->f1;
if( _temp1037 == ( void*) Cyc_Absyn_Eq){ goto _LL1036;} else{ goto _LL1016;}
_LL1036: _temp1035=(( struct Cyc_Absyn_Primop_e_struct*) _temp1008)->f2; goto
_LL1015;} else{ goto _LL1016;} _LL1016: if(*(( int*) _temp1008) ==  Cyc_Absyn_Primop_e){
_LL1042: _temp1041=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1008)->f1;
if( _temp1041 == ( void*) Cyc_Absyn_Neq){ goto _LL1040;} else{ goto _LL1018;}
_LL1040: _temp1039=(( struct Cyc_Absyn_Primop_e_struct*) _temp1008)->f2; goto
_LL1017;} else{ goto _LL1018;} _LL1018: goto _LL1019; _LL1011: { void* _temp1045;
void* _temp1047; struct _tuple0 _temp1043= Cyc_NewControlFlow_anal_test( env,
inflow, _temp1024); _LL1048: _temp1047= _temp1043.f1; goto _LL1046; _LL1046:
_temp1045= _temp1043.f2; goto _LL1044; _LL1044: { void* _temp1051; void*
_temp1053; struct _tuple0 _temp1049= Cyc_NewControlFlow_anal_test( env,
_temp1047, _temp1022); _LL1054: _temp1053= _temp1049.f1; goto _LL1052; _LL1052:
_temp1051= _temp1049.f2; goto _LL1050; _LL1050: { void* _temp1057; void*
_temp1059; struct _tuple0 _temp1055= Cyc_NewControlFlow_anal_test( env,
_temp1045, _temp1020); _LL1060: _temp1059= _temp1055.f1; goto _LL1058; _LL1058:
_temp1057= _temp1055.f2; goto _LL1056; _LL1056: return({ struct _tuple0
_temp1061; _temp1061.f1= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp1053,
_temp1059); _temp1061.f2= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp1051,
_temp1057); _temp1061;});}}} _LL1013: { void* _temp1064; void* _temp1066; struct
_tuple0 _temp1062= Cyc_NewControlFlow_anal_test( env, inflow, _temp1031);
_LL1067: _temp1066= _temp1062.f1; goto _LL1065; _LL1065: _temp1064= _temp1062.f2;
goto _LL1063; _LL1063: return({ struct _tuple0 _temp1068; _temp1068.f1=
_temp1064; _temp1068.f2= _temp1066; _temp1068;});} _LL1015: { struct Cyc_List_List*
_temp1071; void* _temp1073; struct _tuple4 _temp1069= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp1035); _LL1074: _temp1073= _temp1069.f1; goto _LL1072; _LL1072:
_temp1071= _temp1069.f2; goto _LL1070; _LL1070: { void* _temp1075= _temp1073;
struct Cyc_Dict_Dict* _temp1081; _LL1077: if( _temp1075 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1078;} else{ goto _LL1079;} _LL1079: if(( unsigned int) _temp1075 >  1u?*((
int*) _temp1075) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1082: _temp1081=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1075)->f1; goto _LL1080;} else{
goto _LL1076;} _LL1078: return({ struct _tuple0 _temp1083; _temp1083.f1=
_temp1073; _temp1083.f2= _temp1073; _temp1083;}); _LL1080: { void* _temp1084=(
void*)(( struct Cyc_List_List*) _check_null( _temp1071))->hd; void* _temp1085=(
void*)(( struct Cyc_List_List*) _check_null( _temp1071->tl))->hd; if( Cyc_CfFlowInfo_initlevel(
_temp1081, _temp1084) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1086[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1035))->hd)->loc, _tag_arr("expression may not be initialized", sizeof(
unsigned char), 34u), _tag_arr( _temp1086, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1081, _temp1085) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1087[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1035))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1087, sizeof( void*), 0u));});}{
struct _tuple0 _temp1089=({ struct _tuple0 _temp1088; _temp1088.f1= _temp1084;
_temp1088.f2= _temp1085; _temp1088;}); void* _temp1097; void* _temp1099; void*
_temp1101; void* _temp1103; void* _temp1105; void* _temp1107; _LL1091: _LL1100:
_temp1099= _temp1089.f1; if(( unsigned int) _temp1099 >  3u?*(( int*) _temp1099)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1102: _temp1101=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1099)->f1; goto _LL1098;} else{ goto _LL1093;} _LL1098: _temp1097=
_temp1089.f2; if( _temp1097 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1092;}
else{ goto _LL1093;} _LL1093: _LL1108: _temp1107= _temp1089.f1; if( _temp1107 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1104;} else{ goto _LL1095;} _LL1104:
_temp1103= _temp1089.f2; if(( unsigned int) _temp1103 >  3u?*(( int*) _temp1103)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1106: _temp1105=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1103)->f1; goto _LL1094;} else{ goto _LL1095;} _LL1095: goto _LL1096;
_LL1092: { void* _temp1111; void* _temp1113; struct _tuple0 _temp1109= Cyc_NewControlFlow_splitzero(
env, _temp1073, _temp1073,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1035))->hd, _temp1101); _LL1114: _temp1113= _temp1109.f1; goto
_LL1112; _LL1112: _temp1111= _temp1109.f2; goto _LL1110; _LL1110: return({
struct _tuple0 _temp1115; _temp1115.f1= _temp1111; _temp1115.f2= _temp1113;
_temp1115;});} _LL1094: { void* _temp1118; void* _temp1120; struct _tuple0
_temp1116= Cyc_NewControlFlow_splitzero( env, _temp1073, _temp1073,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1035))->tl))->hd, _temp1105); _LL1121: _temp1120= _temp1116.f1; goto
_LL1119; _LL1119: _temp1118= _temp1116.f2; goto _LL1117; _LL1117: return({
struct _tuple0 _temp1122; _temp1122.f1= _temp1118; _temp1122.f2= _temp1120;
_temp1122;});} _LL1096: return({ struct _tuple0 _temp1123; _temp1123.f1=
_temp1073; _temp1123.f2= _temp1073; _temp1123;}); _LL1090:;}} _LL1076:;}}
_LL1017: { struct Cyc_List_List* _temp1126; void* _temp1128; struct _tuple4
_temp1124= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1039);
_LL1129: _temp1128= _temp1124.f1; goto _LL1127; _LL1127: _temp1126= _temp1124.f2;
goto _LL1125; _LL1125: { void* _temp1130= _temp1128; struct Cyc_Dict_Dict*
_temp1136; _LL1132: if( _temp1130 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1133;} else{ goto _LL1134;} _LL1134: if(( unsigned int) _temp1130 >  1u?*((
int*) _temp1130) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1137: _temp1136=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1130)->f1; goto _LL1135;} else{
goto _LL1131;} _LL1133: return({ struct _tuple0 _temp1138; _temp1138.f1=
_temp1128; _temp1138.f2= _temp1128; _temp1138;}); _LL1135: { void* _temp1139=(
void*)(( struct Cyc_List_List*) _check_null( _temp1126))->hd; void* _temp1140=(
void*)(( struct Cyc_List_List*) _check_null( _temp1126->tl))->hd; if( Cyc_CfFlowInfo_initlevel(
_temp1136, _temp1139) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1141[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1039))->hd)->loc, _tag_arr("expression may not be initialized", sizeof(
unsigned char), 34u), _tag_arr( _temp1141, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1136, _temp1140) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1142[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1039))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1142, sizeof( void*), 0u));});}{
struct _tuple0 _temp1144=({ struct _tuple0 _temp1143; _temp1143.f1= _temp1139;
_temp1143.f2= _temp1140; _temp1143;}); void* _temp1152; void* _temp1154; void*
_temp1156; void* _temp1158; void* _temp1160; void* _temp1162; _LL1146: _LL1155:
_temp1154= _temp1144.f1; if(( unsigned int) _temp1154 >  3u?*(( int*) _temp1154)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1157: _temp1156=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1154)->f1; goto _LL1153;} else{ goto _LL1148;} _LL1153: _temp1152=
_temp1144.f2; if( _temp1152 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1147;}
else{ goto _LL1148;} _LL1148: _LL1163: _temp1162= _temp1144.f1; if( _temp1162 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1159;} else{ goto _LL1150;} _LL1159:
_temp1158= _temp1144.f2; if(( unsigned int) _temp1158 >  3u?*(( int*) _temp1158)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1161: _temp1160=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1158)->f1; goto _LL1149;} else{ goto _LL1150;} _LL1150: goto _LL1151;
_LL1147: return Cyc_NewControlFlow_splitzero( env, _temp1128, _temp1128,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1039))->hd, _temp1156);
_LL1149: return Cyc_NewControlFlow_splitzero( env, _temp1128, _temp1128,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1039))->tl))->hd, _temp1160); _LL1151: return({ struct _tuple0
_temp1164; _temp1164.f1= _temp1128; _temp1164.f2= _temp1128; _temp1164;});
_LL1145:;}} _LL1131:;}} _LL1019: goto _LL1009; _LL1009:;}{ void* _temp1167; void*
_temp1169; struct _tuple0 _temp1165= Cyc_NewControlFlow_anal_Rexp( env, inflow,
e); _LL1170: _temp1169= _temp1165.f1; goto _LL1168; _LL1168: _temp1167=
_temp1165.f2; goto _LL1166; _LL1166: { void* _temp1171= _temp1169; struct Cyc_Dict_Dict*
_temp1177; _LL1173: if( _temp1171 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1174;} else{ goto _LL1175;} _LL1175: if(( unsigned int) _temp1171 >  1u?*((
int*) _temp1171) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1178: _temp1177=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1171)->f1; goto _LL1176;} else{
goto _LL1172;} _LL1174: return({ struct _tuple0 _temp1179; _temp1179.f1=
_temp1169; _temp1179.f2= _temp1169; _temp1179;}); _LL1176: { void* _temp1180=
_temp1167; void* _temp1200; void* _temp1202; void* _temp1204; _LL1182: if(
_temp1180 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1183;} else{ goto _LL1184;}
_LL1184: if( _temp1180 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto _LL1185;}
else{ goto _LL1186;} _LL1186: if( _temp1180 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL1187;} else{ goto _LL1188;} _LL1188: if(( unsigned int) _temp1180 >  3u?*((
int*) _temp1180) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL1189;} else{ goto
_LL1190;} _LL1190: if(( unsigned int) _temp1180 >  3u?*(( int*) _temp1180) == 
Cyc_CfFlowInfo_UnknownR: 0){ _LL1201: _temp1200=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1180)->f1; if( _temp1200 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL1191;}
else{ goto _LL1192;}} else{ goto _LL1192;} _LL1192: if(( unsigned int) _temp1180
>  3u?*(( int*) _temp1180) ==  Cyc_CfFlowInfo_Esc: 0){ _LL1203: _temp1202=( void*)((
struct Cyc_CfFlowInfo_Esc_struct*) _temp1180)->f1; if( _temp1202 == ( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL1193;} else{ goto _LL1194;}} else{ goto _LL1194;} _LL1194: if((
unsigned int) _temp1180 >  3u?*(( int*) _temp1180) ==  Cyc_CfFlowInfo_UnknownR:
0){ _LL1205: _temp1204=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1180)->f1; goto _LL1195;} else{ goto _LL1196;} _LL1196: if(( unsigned int)
_temp1180 >  3u?*(( int*) _temp1180) ==  Cyc_CfFlowInfo_Esc: 0){ goto _LL1197;}
else{ goto _LL1198;} _LL1198: if(( unsigned int) _temp1180 >  3u?*(( int*)
_temp1180) ==  Cyc_CfFlowInfo_Aggregate: 0){ goto _LL1199;} else{ goto _LL1181;}
_LL1183: return({ struct _tuple0 _temp1206; _temp1206.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1206.f2= _temp1169; _temp1206;}); _LL1185: goto _LL1187; _LL1187: goto
_LL1189; _LL1189: return({ struct _tuple0 _temp1207; _temp1207.f1= _temp1169;
_temp1207.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1207;}); _LL1191: goto
_LL1193; _LL1193:({ void* _temp1208[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1208, sizeof( void*), 0u));});
return({ struct _tuple0 _temp1209; _temp1209.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1209.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1209;}); _LL1195: return Cyc_NewControlFlow_splitzero(
env, inflow, _temp1169, e, _temp1204); _LL1197: return({ struct _tuple0
_temp1210; _temp1210.f1= _temp1169; _temp1210.f2= _temp1169; _temp1210;});
_LL1199:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1211=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1211[ 0]=({ struct Cyc_Core_Impossible_struct _temp1212; _temp1212.tag= Cyc_Core_Impossible;
_temp1212.f1= _tag_arr("anal_test", sizeof( unsigned char), 10u); _temp1212;});
_temp1211;})); _LL1181:;} _LL1172:;}}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( struct Cyc_NewControlFlow_AnalEnv* env, void*
inflow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1213= Cyc_NewControlFlow_get_stmt_annot( s);( void*)( _temp1213->flow=(
void*) Cyc_CfFlowInfo_join_flow( env->all_changed, inflow,( void*) _temp1213->flow));
++ _temp1213->visited; _temp1213->visited ==  env->iteration_num? 0:(((( int(*)(
struct _tagged_arr assertion, struct _tagged_arr file, unsigned int line)) Cyc_Std___assert_fail)(
_tag_arr("annot->visited == env->iteration_num", sizeof( unsigned char), 37u),
_tag_arr("new_control_flow.cyc", sizeof( unsigned char), 21u), 799), 0)); return
_temp1213;} static void* Cyc_NewControlFlow_anal_scs( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, struct Cyc_List_List* scs){ for( 0; scs !=  0; scs= scs->tl){
struct Cyc_Absyn_Switch_clause _temp1216; struct Cyc_Absyn_Stmt* _temp1217;
struct Cyc_Absyn_Exp* _temp1219; struct Cyc_Core_Opt* _temp1221; struct Cyc_Absyn_Switch_clause*
_temp1214=( struct Cyc_Absyn_Switch_clause*) scs->hd; _temp1216=* _temp1214;
_LL1222: _temp1221= _temp1216.pat_vars; goto _LL1220; _LL1220: _temp1219=
_temp1216.where_clause; goto _LL1218; _LL1218: _temp1217= _temp1216.body; goto
_LL1215; _LL1215: { void* clause_inflow= Cyc_NewControlFlow_add_vars( inflow,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1221))->v, Cyc_CfFlowInfo_unknown_all);
void* clause_outflow; if( _temp1219 !=  0){ void* _temp1225; void* _temp1227;
struct _tuple0 _temp1223= Cyc_NewControlFlow_anal_test( env, clause_inflow,(
struct Cyc_Absyn_Exp*) _check_null( _temp1219)); _LL1228: _temp1227= _temp1223.f1;
goto _LL1226; _LL1226: _temp1225= _temp1223.f2; goto _LL1224; _LL1224: inflow=
_temp1225; clause_outflow= Cyc_NewControlFlow_anal_stmt( env, _temp1227,
_temp1217);} else{ clause_outflow= Cyc_NewControlFlow_anal_stmt( env,
clause_inflow, _temp1217);}{ void* _temp1229= clause_outflow; _LL1231: if(
_temp1229 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1232;} else{ goto
_LL1233;} _LL1233: goto _LL1234; _LL1232: goto _LL1230; _LL1234: if( scs->tl == 
0){ return clause_outflow;} else{ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs->tl))->hd)->pat_vars))->v !=  0){({ void* _temp1235[ 0u]={};
Cyc_Tcutil_terr( _temp1217->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1235, sizeof( void*), 0u));});}
else{({ void* _temp1236[ 0u]={}; Cyc_Tcutil_warn( _temp1217->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1236, sizeof( void*), 0u));});} Cyc_NewControlFlow_update_flow(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs->tl))->hd)->body, clause_outflow);} goto _LL1230; _LL1230:;}}} return( void*)
Cyc_CfFlowInfo_BottomFL;} static void* Cyc_NewControlFlow_anal_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1237= Cyc_NewControlFlow_pre_stmt_check( env, inflow, s); inflow=( void*)
_temp1237->flow;{ void* _temp1238=( void*) s->r; struct Cyc_Absyn_Exp* _temp1280;
struct Cyc_Absyn_Exp* _temp1282; struct Cyc_Absyn_Exp* _temp1284; struct Cyc_Absyn_Stmt*
_temp1286; struct Cyc_Absyn_Stmt* _temp1288; struct Cyc_Absyn_Stmt* _temp1290;
struct Cyc_Absyn_Stmt* _temp1292; struct Cyc_Absyn_Exp* _temp1294; struct Cyc_Absyn_Stmt*
_temp1296; struct _tuple3 _temp1298; struct Cyc_Absyn_Stmt* _temp1300; struct
Cyc_Absyn_Exp* _temp1302; struct _tuple3 _temp1304; struct Cyc_Absyn_Stmt*
_temp1306; struct Cyc_Absyn_Exp* _temp1308; struct Cyc_Absyn_Stmt* _temp1310;
struct Cyc_Absyn_Stmt* _temp1312; struct _tuple3 _temp1314; struct Cyc_Absyn_Stmt*
_temp1316; struct Cyc_Absyn_Exp* _temp1318; struct _tuple3 _temp1320; struct Cyc_Absyn_Stmt*
_temp1322; struct Cyc_Absyn_Exp* _temp1324; struct Cyc_Absyn_Exp* _temp1326;
struct Cyc_Absyn_Stmt* _temp1328; struct Cyc_Absyn_Switch_clause** _temp1330;
struct Cyc_Absyn_Switch_clause* _temp1332; struct Cyc_List_List* _temp1333;
struct Cyc_Absyn_Stmt* _temp1335; struct Cyc_Absyn_Stmt* _temp1337; struct Cyc_Absyn_Stmt*
_temp1339; struct Cyc_List_List* _temp1341; struct Cyc_Absyn_Exp* _temp1343;
struct Cyc_List_List* _temp1345; struct Cyc_Absyn_Stmt* _temp1347; struct Cyc_Absyn_Stmt*
_temp1349; struct Cyc_Absyn_Decl* _temp1351; struct Cyc_Absyn_Stmt* _temp1353;
struct Cyc_Absyn_Stmt* _temp1355; struct Cyc_Absyn_Vardecl* _temp1357; struct
Cyc_Absyn_Tvar* _temp1359; _LL1240: if( _temp1238 == ( void*) Cyc_Absyn_Skip_s){
goto _LL1241;} else{ goto _LL1242;} _LL1242: if(( unsigned int) _temp1238 >  1u?*((
int*) _temp1238) ==  Cyc_Absyn_Return_s: 0){ _LL1281: _temp1280=(( struct Cyc_Absyn_Return_s_struct*)
_temp1238)->f1; if( _temp1280 ==  0){ goto _LL1243;} else{ goto _LL1244;}} else{
goto _LL1244;} _LL1244: if(( unsigned int) _temp1238 >  1u?*(( int*) _temp1238)
==  Cyc_Absyn_Return_s: 0){ _LL1283: _temp1282=(( struct Cyc_Absyn_Return_s_struct*)
_temp1238)->f1; goto _LL1245;} else{ goto _LL1246;} _LL1246: if(( unsigned int)
_temp1238 >  1u?*(( int*) _temp1238) ==  Cyc_Absyn_Exp_s: 0){ _LL1285: _temp1284=((
struct Cyc_Absyn_Exp_s_struct*) _temp1238)->f1; goto _LL1247;} else{ goto
_LL1248;} _LL1248: if(( unsigned int) _temp1238 >  1u?*(( int*) _temp1238) == 
Cyc_Absyn_Seq_s: 0){ _LL1289: _temp1288=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1238)->f1; goto _LL1287; _LL1287: _temp1286=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1238)->f2; goto _LL1249;} else{ goto _LL1250;} _LL1250: if(( unsigned int)
_temp1238 >  1u?*(( int*) _temp1238) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1295:
_temp1294=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1238)->f1; goto _LL1293;
_LL1293: _temp1292=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1238)->f2;
goto _LL1291; _LL1291: _temp1290=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1238)->f3; goto _LL1251;} else{ goto _LL1252;} _LL1252: if(( unsigned int)
_temp1238 >  1u?*(( int*) _temp1238) ==  Cyc_Absyn_While_s: 0){ _LL1299:
_temp1298=(( struct Cyc_Absyn_While_s_struct*) _temp1238)->f1; _LL1303:
_temp1302= _temp1298.f1; goto _LL1301; _LL1301: _temp1300= _temp1298.f2; goto
_LL1297; _LL1297: _temp1296=(( struct Cyc_Absyn_While_s_struct*) _temp1238)->f2;
goto _LL1253;} else{ goto _LL1254;} _LL1254: if(( unsigned int) _temp1238 >  1u?*((
int*) _temp1238) ==  Cyc_Absyn_Do_s: 0){ _LL1311: _temp1310=(( struct Cyc_Absyn_Do_s_struct*)
_temp1238)->f1; goto _LL1305; _LL1305: _temp1304=(( struct Cyc_Absyn_Do_s_struct*)
_temp1238)->f2; _LL1309: _temp1308= _temp1304.f1; goto _LL1307; _LL1307:
_temp1306= _temp1304.f2; goto _LL1255;} else{ goto _LL1256;} _LL1256: if((
unsigned int) _temp1238 >  1u?*(( int*) _temp1238) ==  Cyc_Absyn_For_s: 0){
_LL1327: _temp1326=(( struct Cyc_Absyn_For_s_struct*) _temp1238)->f1; goto
_LL1321; _LL1321: _temp1320=(( struct Cyc_Absyn_For_s_struct*) _temp1238)->f2;
_LL1325: _temp1324= _temp1320.f1; goto _LL1323; _LL1323: _temp1322= _temp1320.f2;
goto _LL1315; _LL1315: _temp1314=(( struct Cyc_Absyn_For_s_struct*) _temp1238)->f3;
_LL1319: _temp1318= _temp1314.f1; goto _LL1317; _LL1317: _temp1316= _temp1314.f2;
goto _LL1313; _LL1313: _temp1312=(( struct Cyc_Absyn_For_s_struct*) _temp1238)->f4;
goto _LL1257;} else{ goto _LL1258;} _LL1258: if(( unsigned int) _temp1238 >  1u?*((
int*) _temp1238) ==  Cyc_Absyn_Break_s: 0){ _LL1329: _temp1328=(( struct Cyc_Absyn_Break_s_struct*)
_temp1238)->f1; if( _temp1328 ==  0){ goto _LL1259;} else{ goto _LL1260;}} else{
goto _LL1260;} _LL1260: if(( unsigned int) _temp1238 >  1u?*(( int*) _temp1238)
==  Cyc_Absyn_Fallthru_s: 0){ _LL1334: _temp1333=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1238)->f1; goto _LL1331; _LL1331: _temp1330=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1238)->f2; if( _temp1330 ==  0){ goto _LL1262;} else{ _temp1332=* _temp1330;
goto _LL1261;}} else{ goto _LL1262;} _LL1262: if(( unsigned int) _temp1238 >  1u?*((
int*) _temp1238) ==  Cyc_Absyn_Break_s: 0){ _LL1336: _temp1335=(( struct Cyc_Absyn_Break_s_struct*)
_temp1238)->f1; goto _LL1263;} else{ goto _LL1264;} _LL1264: if(( unsigned int)
_temp1238 >  1u?*(( int*) _temp1238) ==  Cyc_Absyn_Continue_s: 0){ _LL1338:
_temp1337=(( struct Cyc_Absyn_Continue_s_struct*) _temp1238)->f1; goto _LL1265;}
else{ goto _LL1266;} _LL1266: if(( unsigned int) _temp1238 >  1u?*(( int*)
_temp1238) ==  Cyc_Absyn_Goto_s: 0){ _LL1340: _temp1339=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1238)->f2; goto _LL1267;} else{ goto _LL1268;} _LL1268: if(( unsigned int)
_temp1238 >  1u?*(( int*) _temp1238) ==  Cyc_Absyn_Switch_s: 0){ _LL1344:
_temp1343=(( struct Cyc_Absyn_Switch_s_struct*) _temp1238)->f1; goto _LL1342;
_LL1342: _temp1341=(( struct Cyc_Absyn_Switch_s_struct*) _temp1238)->f2; goto
_LL1269;} else{ goto _LL1270;} _LL1270: if(( unsigned int) _temp1238 >  1u?*((
int*) _temp1238) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1348: _temp1347=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1238)->f1; goto _LL1346; _LL1346: _temp1345=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1238)->f2; goto _LL1271;} else{ goto _LL1272;} _LL1272: if(( unsigned int)
_temp1238 >  1u?*(( int*) _temp1238) ==  Cyc_Absyn_Decl_s: 0){ _LL1352:
_temp1351=(( struct Cyc_Absyn_Decl_s_struct*) _temp1238)->f1; goto _LL1350;
_LL1350: _temp1349=(( struct Cyc_Absyn_Decl_s_struct*) _temp1238)->f2; goto
_LL1273;} else{ goto _LL1274;} _LL1274: if(( unsigned int) _temp1238 >  1u?*((
int*) _temp1238) ==  Cyc_Absyn_Label_s: 0){ _LL1354: _temp1353=(( struct Cyc_Absyn_Label_s_struct*)
_temp1238)->f2; goto _LL1275;} else{ goto _LL1276;} _LL1276: if(( unsigned int)
_temp1238 >  1u?*(( int*) _temp1238) ==  Cyc_Absyn_Region_s: 0){ _LL1360:
_temp1359=(( struct Cyc_Absyn_Region_s_struct*) _temp1238)->f1; goto _LL1358;
_LL1358: _temp1357=(( struct Cyc_Absyn_Region_s_struct*) _temp1238)->f2; goto
_LL1356; _LL1356: _temp1355=(( struct Cyc_Absyn_Region_s_struct*) _temp1238)->f3;
goto _LL1277;} else{ goto _LL1278;} _LL1278: goto _LL1279; _LL1241: return
inflow; _LL1243: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1245: { void*
_temp1363; void* _temp1365; struct _tuple0 _temp1361= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1282)); _LL1366:
_temp1365= _temp1361.f1; goto _LL1364; _LL1364: _temp1363= _temp1361.f2; goto
_LL1362; _LL1362: Cyc_NewControlFlow_use_Rval( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp1282))->loc, _temp1365, _temp1363); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1247: return( Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp1284)).f1;
_LL1249: return Cyc_NewControlFlow_anal_stmt( env, Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1288), _temp1286); _LL1251: { void* _temp1369; void* _temp1371;
struct _tuple0 _temp1367= Cyc_NewControlFlow_anal_test( env, inflow, _temp1294);
_LL1372: _temp1371= _temp1367.f1; goto _LL1370; _LL1370: _temp1369= _temp1367.f2;
goto _LL1368; _LL1368: return Cyc_CfFlowInfo_join_flow( env->all_changed, Cyc_NewControlFlow_anal_stmt(
env, _temp1371, _temp1292), Cyc_NewControlFlow_anal_stmt( env, _temp1369,
_temp1290));} _LL1253: { void* _temp1373=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, inflow, _temp1300))->flow; void* _temp1376; void* _temp1378; struct _tuple0
_temp1374= Cyc_NewControlFlow_anal_test( env, _temp1373, _temp1302); _LL1379:
_temp1378= _temp1374.f1; goto _LL1377; _LL1377: _temp1376= _temp1374.f2; goto
_LL1375; _LL1375: { void* _temp1380= Cyc_NewControlFlow_anal_stmt( env,
_temp1378, _temp1296); Cyc_NewControlFlow_update_flow( env, _temp1300, _temp1380);
return _temp1376;}} _LL1255: { void* _temp1381= Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1310); void* _temp1382=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1381, _temp1306))->flow; void* _temp1385; void* _temp1387; struct
_tuple0 _temp1383= Cyc_NewControlFlow_anal_test( env, _temp1382, _temp1308);
_LL1388: _temp1387= _temp1383.f1; goto _LL1386; _LL1386: _temp1385= _temp1383.f2;
goto _LL1384; _LL1384: Cyc_NewControlFlow_update_flow( env, _temp1310, _temp1387);
return _temp1385;} _LL1257: { void* _temp1389=( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1326)).f1; void* _temp1390=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1389, _temp1322))->flow; void* _temp1393; void* _temp1395; struct
_tuple0 _temp1391= Cyc_NewControlFlow_anal_test( env, _temp1390, _temp1324);
_LL1396: _temp1395= _temp1391.f1; goto _LL1394; _LL1394: _temp1393= _temp1391.f2;
goto _LL1392; _LL1392: { void* _temp1397= Cyc_NewControlFlow_anal_stmt( env,
_temp1395, _temp1312); void* _temp1398=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1397, _temp1316))->flow; void* _temp1399=( Cyc_NewControlFlow_anal_Rexp(
env, _temp1398, _temp1318)).f1; Cyc_NewControlFlow_update_flow( env, _temp1322,
_temp1399); return _temp1393;}} _LL1259: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1261: { struct Cyc_List_List* _temp1402; void* _temp1404; struct _tuple4
_temp1400= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1333);
_LL1405: _temp1404= _temp1400.f1; goto _LL1403; _LL1403: _temp1402= _temp1400.f2;
goto _LL1401; _LL1401: for( 0; _temp1402 !=  0;( _temp1402= _temp1402->tl,
_temp1333= _temp1333->tl)){ _temp1404= Cyc_NewControlFlow_use_Rval( env,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1333))->hd)->loc,
_temp1404,( void*) _temp1402->hd);} _temp1404= Cyc_NewControlFlow_add_vars(
_temp1404,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1332->pat_vars))->v, Cyc_CfFlowInfo_unknown_all); Cyc_NewControlFlow_update_flow(
env,( struct Cyc_Absyn_Stmt*) _temp1332->body, _temp1404); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1263: _temp1337= _temp1335; goto _LL1265; _LL1265: _temp1339= _temp1337; goto
_LL1267; _LL1267: if( env->iteration_num ==  1){ struct Cyc_Absyn_Stmt*
_temp1406= _temp1237->encloser; struct Cyc_Absyn_Stmt* _temp1407=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp1339)))->encloser; while( _temp1407 != 
_temp1406) { struct Cyc_Absyn_Stmt* _temp1408=( Cyc_NewControlFlow_get_stmt_annot(
_temp1406))->encloser; if( _temp1408 ==  _temp1406){({ void* _temp1409[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("goto enters local scope or exception handler",
sizeof( unsigned char), 45u), _tag_arr( _temp1409, sizeof( void*), 0u));});
break;} _temp1406= _temp1408;}} Cyc_NewControlFlow_update_flow( env,( struct Cyc_Absyn_Stmt*)
_check_null( _temp1339), inflow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1269: { void* _temp1412; void* _temp1414; struct _tuple0 _temp1410= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1343); _LL1415: _temp1414= _temp1410.f1; goto _LL1413; _LL1413:
_temp1412= _temp1410.f2; goto _LL1411; _LL1411: _temp1414= Cyc_NewControlFlow_use_Rval(
env, _temp1343->loc, _temp1414, _temp1412); return Cyc_NewControlFlow_anal_scs(
env, _temp1414, _temp1341);} _LL1271: { int old_in_try= env->in_try; void*
old_tryflow=( void*) env->tryflow; env->in_try= 1;( void*)( env->tryflow=( void*)
inflow);{ void* s1_outflow= Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1347);
void* scs_inflow=( void*) env->tryflow; env->in_try= old_in_try;( void*)( env->tryflow=(
void*) old_tryflow); Cyc_NewControlFlow_update_tryflow( env, scs_inflow);{ void*
scs_outflow= Cyc_NewControlFlow_anal_scs( env, scs_inflow, _temp1345);{ void*
_temp1416= scs_outflow; _LL1418: if( _temp1416 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1419;} else{ goto _LL1420;} _LL1420: goto _LL1421; _LL1419: goto _LL1417;
_LL1421:({ void* _temp1422[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("last catch clause may implicitly fallthru",
sizeof( unsigned char), 42u), _tag_arr( _temp1422, sizeof( void*), 0u));});
_LL1417:;} return s1_outflow;}}} _LL1273: return Cyc_NewControlFlow_anal_stmt(
env, Cyc_NewControlFlow_anal_decl( env, inflow, _temp1351), _temp1349); _LL1275:
return Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1353); _LL1277: { struct
Cyc_List_List _temp1423=({ struct Cyc_List_List _temp1424; _temp1424.hd=( void*)
_temp1357; _temp1424.tl= 0; _temp1424;}); inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)& _temp1423, Cyc_CfFlowInfo_unknown_all); return
Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1355);} _LL1279:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp1425=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1425[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1426; _temp1426.tag= Cyc_Core_Impossible;
_temp1426.f1= _tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 56u); _temp1426;}); _temp1425;})); _LL1239:;}} static
void* Cyc_NewControlFlow_anal_decl( struct Cyc_NewControlFlow_AnalEnv* env, void*
inflow, struct Cyc_Absyn_Decl* decl){ void* _temp1427=( void*) decl->r; struct
Cyc_Absyn_Vardecl* _temp1437; struct Cyc_Absyn_Exp* _temp1439; struct Cyc_Core_Opt*
_temp1441; struct Cyc_Core_Opt _temp1443; struct Cyc_List_List* _temp1444;
struct Cyc_List_List* _temp1446; _LL1429: if(*(( int*) _temp1427) ==  Cyc_Absyn_Var_d){
_LL1438: _temp1437=(( struct Cyc_Absyn_Var_d_struct*) _temp1427)->f1; goto
_LL1430;} else{ goto _LL1431;} _LL1431: if(*(( int*) _temp1427) ==  Cyc_Absyn_Let_d){
_LL1442: _temp1441=(( struct Cyc_Absyn_Let_d_struct*) _temp1427)->f2; if(
_temp1441 ==  0){ goto _LL1433;} else{ _temp1443=* _temp1441; _LL1445: _temp1444=(
struct Cyc_List_List*) _temp1443.v; goto _LL1440;} _LL1440: _temp1439=(( struct
Cyc_Absyn_Let_d_struct*) _temp1427)->f4; goto _LL1432;} else{ goto _LL1433;}
_LL1433: if(*(( int*) _temp1427) ==  Cyc_Absyn_Letv_d){ _LL1447: _temp1446=((
struct Cyc_Absyn_Letv_d_struct*) _temp1427)->f1; goto _LL1434;} else{ goto
_LL1435;} _LL1435: goto _LL1436; _LL1430: { struct Cyc_List_List _temp1448=({
struct Cyc_List_List _temp1470; _temp1470.hd=( void*) _temp1437; _temp1470.tl= 0;
_temp1470;}); inflow= Cyc_NewControlFlow_add_vars( inflow,( struct Cyc_List_List*)&
_temp1448, Cyc_CfFlowInfo_unknown_none);{ struct Cyc_Absyn_Exp* _temp1449=
_temp1437->initializer; if( _temp1449 ==  0){ return inflow;}{ void* _temp1452;
void* _temp1454; struct _tuple0 _temp1450= Cyc_NewControlFlow_anal_Rexp( env,
inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1449)); _LL1455: _temp1454=
_temp1450.f1; goto _LL1453; _LL1453: _temp1452= _temp1450.f2; goto _LL1451;
_LL1451: { void* _temp1456= _temp1454; struct Cyc_Dict_Dict* _temp1462; _LL1458:
if( _temp1456 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1459;} else{ goto
_LL1460;} _LL1460: if(( unsigned int) _temp1456 >  1u?*(( int*) _temp1456) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL1463: _temp1462=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1456)->f1; goto _LL1461;} else{ goto _LL1457;} _LL1459: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1461: _temp1462= Cyc_CfFlowInfo_assign_place( decl->loc, _temp1462, env->all_changed,({
struct Cyc_CfFlowInfo_Place* _temp1464=( struct Cyc_CfFlowInfo_Place*) _cycalloc(
sizeof( struct Cyc_CfFlowInfo_Place)); _temp1464->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1465=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1465[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1466; _temp1466.tag= Cyc_CfFlowInfo_VarRoot;
_temp1466.f1= _temp1437; _temp1466;}); _temp1465;})); _temp1464->fields= 0;
_temp1464;}), _temp1452);{ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp1467=({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp1468=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp1468[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp1469; _temp1469.tag= Cyc_CfFlowInfo_ReachableFL;
_temp1469.f1= _temp1462; _temp1469;}); _temp1468;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp1467); return( void*) _temp1467;} _LL1457:;}}}} _LL1432: {
void* _temp1473; void* _temp1475; struct _tuple0 _temp1471= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1439); _LL1476: _temp1475= _temp1471.f1; goto _LL1474; _LL1474:
_temp1473= _temp1471.f2; goto _LL1472; _LL1472: _temp1475= Cyc_NewControlFlow_use_Rval(
env, _temp1439->loc, _temp1475, _temp1473); return Cyc_NewControlFlow_add_vars(
_temp1475, _temp1444, Cyc_CfFlowInfo_unknown_all);} _LL1434: return Cyc_NewControlFlow_add_vars(
inflow, _temp1446, Cyc_CfFlowInfo_unknown_none); _LL1436:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp1477=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1477[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1478; _temp1478.tag= Cyc_Core_Impossible;
_temp1478.f1= _tag_arr("anal_decl: unexpected decl variant", sizeof(
unsigned char), 35u); _temp1478;}); _temp1477;})); _LL1428:;} static void Cyc_NewControlFlow_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp1479=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1503; struct _tuple3
_temp1505; struct Cyc_Absyn_Stmt* _temp1507; struct _tuple3 _temp1509; struct
Cyc_Absyn_Stmt* _temp1511; struct Cyc_Absyn_Stmt* _temp1513; struct Cyc_Absyn_Stmt*
_temp1515; struct Cyc_Absyn_Stmt* _temp1517; struct _tuple3 _temp1519; struct
Cyc_Absyn_Stmt* _temp1521; struct _tuple3 _temp1523; struct Cyc_Absyn_Stmt*
_temp1525; struct Cyc_Absyn_Stmt* _temp1527; struct Cyc_Absyn_Stmt* _temp1529;
struct Cyc_Absyn_Stmt* _temp1531; struct Cyc_Absyn_Stmt* _temp1533; struct Cyc_Absyn_Stmt*
_temp1535; struct Cyc_Absyn_Stmt* _temp1537; struct Cyc_List_List* _temp1539;
struct Cyc_Absyn_Stmt* _temp1541; struct Cyc_List_List* _temp1543; _LL1481: if((
unsigned int) _temp1479 >  1u?*(( int*) _temp1479) ==  Cyc_Absyn_For_s: 0){
_LL1510: _temp1509=(( struct Cyc_Absyn_For_s_struct*) _temp1479)->f2; _LL1512:
_temp1511= _temp1509.f2; goto _LL1506; _LL1506: _temp1505=(( struct Cyc_Absyn_For_s_struct*)
_temp1479)->f3; _LL1508: _temp1507= _temp1505.f2; goto _LL1504; _LL1504:
_temp1503=(( struct Cyc_Absyn_For_s_struct*) _temp1479)->f4; goto _LL1482;}
else{ goto _LL1483;} _LL1483: if(( unsigned int) _temp1479 >  1u?*(( int*)
_temp1479) ==  Cyc_Absyn_Seq_s: 0){ _LL1516: _temp1515=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1479)->f1; goto _LL1514; _LL1514: _temp1513=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1479)->f2; goto _LL1484;} else{ goto _LL1485;} _LL1485: if(( unsigned int)
_temp1479 >  1u?*(( int*) _temp1479) ==  Cyc_Absyn_While_s: 0){ _LL1520:
_temp1519=(( struct Cyc_Absyn_While_s_struct*) _temp1479)->f1; _LL1522:
_temp1521= _temp1519.f2; goto _LL1518; _LL1518: _temp1517=(( struct Cyc_Absyn_While_s_struct*)
_temp1479)->f2; goto _LL1486;} else{ goto _LL1487;} _LL1487: if(( unsigned int)
_temp1479 >  1u?*(( int*) _temp1479) ==  Cyc_Absyn_Do_s: 0){ _LL1528: _temp1527=((
struct Cyc_Absyn_Do_s_struct*) _temp1479)->f1; goto _LL1524; _LL1524: _temp1523=((
struct Cyc_Absyn_Do_s_struct*) _temp1479)->f2; _LL1526: _temp1525= _temp1523.f2;
goto _LL1488;} else{ goto _LL1489;} _LL1489: if(( unsigned int) _temp1479 >  1u?*((
int*) _temp1479) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1532: _temp1531=(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp1479)->f2; goto _LL1530; _LL1530: _temp1529=((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1479)->f3; goto _LL1490;} else{ goto
_LL1491;} _LL1491: if(( unsigned int) _temp1479 >  1u?*(( int*) _temp1479) == 
Cyc_Absyn_Region_s: 0){ _LL1534: _temp1533=(( struct Cyc_Absyn_Region_s_struct*)
_temp1479)->f3; goto _LL1492;} else{ goto _LL1493;} _LL1493: if(( unsigned int)
_temp1479 >  1u?*(( int*) _temp1479) ==  Cyc_Absyn_Label_s: 0){ _LL1536:
_temp1535=(( struct Cyc_Absyn_Label_s_struct*) _temp1479)->f2; goto _LL1494;}
else{ goto _LL1495;} _LL1495: if(( unsigned int) _temp1479 >  1u?*(( int*)
_temp1479) ==  Cyc_Absyn_Decl_s: 0){ _LL1538: _temp1537=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1479)->f2; goto _LL1496;} else{ goto _LL1497;} _LL1497: if(( unsigned int)
_temp1479 >  1u?*(( int*) _temp1479) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1542:
_temp1541=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1479)->f1; goto _LL1540;
_LL1540: _temp1539=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1479)->f2; goto
_LL1498;} else{ goto _LL1499;} _LL1499: if(( unsigned int) _temp1479 >  1u?*((
int*) _temp1479) ==  Cyc_Absyn_Switch_s: 0){ _LL1544: _temp1543=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1479)->f2; goto _LL1500;} else{ goto _LL1501;} _LL1501: goto _LL1502;
_LL1482: Cyc_NewControlFlow_postpass( _temp1507); _temp1515= _temp1503;
_temp1513= _temp1511; goto _LL1484; _LL1484: _temp1521= _temp1515; _temp1517=
_temp1513; goto _LL1486; _LL1486: _temp1527= _temp1517; _temp1525= _temp1521;
goto _LL1488; _LL1488: _temp1531= _temp1527; _temp1529= _temp1525; goto _LL1490;
_LL1490: Cyc_NewControlFlow_postpass( _temp1531); _temp1533= _temp1529; goto
_LL1492; _LL1492: _temp1535= _temp1533; goto _LL1494; _LL1494: _temp1537=
_temp1535; goto _LL1496; _LL1496: Cyc_NewControlFlow_postpass( _temp1537);
return; _LL1498: Cyc_NewControlFlow_postpass( _temp1541); _temp1543= _temp1539;
goto _LL1500; _LL1500: for( 0; _temp1543 !=  0; _temp1543= _temp1543->tl){ Cyc_NewControlFlow_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1543->hd)->body);} return; _LL1502:
return; _LL1480:;}} static void Cyc_NewControlFlow_check_fun( struct Cyc_Absyn_Fndecl*
fd){ void* inflow= Cyc_NewControlFlow_add_vars(( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp1553=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp1553[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp1554; _temp1554.tag= Cyc_CfFlowInfo_ReachableFL; _temp1554.f1= Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp); _temp1554;}); _temp1553;}),( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v, Cyc_CfFlowInfo_unknown_all);
struct Cyc_NewControlFlow_AnalEnv* env=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1552=( struct Cyc_NewControlFlow_AnalEnv*) _cycalloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1552->iterate_again= 1; _temp1552->iteration_num= 0; _temp1552->in_try= 0;
_temp1552->tryflow=( void*) inflow; _temp1552->all_changed= 0; _temp1552;});
void* outflow= inflow; while( env->iterate_again) { ++ env->iteration_num; env->iterate_again=
0; outflow= Cyc_NewControlFlow_anal_stmt( env, inflow, fd->body);}{ void*
_temp1545= outflow; _LL1547: if( _temp1545 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1548;} else{ goto _LL1549;} _LL1549: goto _LL1550; _LL1548: goto _LL1546;
_LL1550: if(( void*) fd->ret_type != ( void*) Cyc_Absyn_VoidType){({ void*
_temp1551[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc, _tag_arr("function may not return a value",
sizeof( unsigned char), 32u), _tag_arr( _temp1551, sizeof( void*), 0u));});}
_LL1546:;} Cyc_NewControlFlow_postpass( fd->body);} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds !=  0; ds= ds->tl){ void* _temp1555=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1567; struct
Cyc_List_List* _temp1569; struct Cyc_List_List* _temp1571; struct Cyc_List_List*
_temp1573; _LL1557: if(*(( int*) _temp1555) ==  Cyc_Absyn_Fn_d){ _LL1568:
_temp1567=(( struct Cyc_Absyn_Fn_d_struct*) _temp1555)->f1; goto _LL1558;} else{
goto _LL1559;} _LL1559: if(*(( int*) _temp1555) ==  Cyc_Absyn_ExternC_d){
_LL1570: _temp1569=(( struct Cyc_Absyn_ExternC_d_struct*) _temp1555)->f1; goto
_LL1560;} else{ goto _LL1561;} _LL1561: if(*(( int*) _temp1555) ==  Cyc_Absyn_Using_d){
_LL1572: _temp1571=(( struct Cyc_Absyn_Using_d_struct*) _temp1555)->f2; goto
_LL1562;} else{ goto _LL1563;} _LL1563: if(*(( int*) _temp1555) ==  Cyc_Absyn_Namespace_d){
_LL1574: _temp1573=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1555)->f2; goto
_LL1564;} else{ goto _LL1565;} _LL1565: goto _LL1566; _LL1558: Cyc_NewControlFlow_check_fun(
_temp1567); goto _LL1556; _LL1560: _temp1571= _temp1569; goto _LL1562; _LL1562:
_temp1573= _temp1571; goto _LL1564; _LL1564: Cyc_NewControlFlow_cf_check(
_temp1573); goto _LL1556; _LL1566: goto _LL1556; _LL1556:;}}
