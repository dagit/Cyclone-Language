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
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_CfFlowInfo_Place{ void* root;
struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_NoneIL= 0;
static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; static const int Cyc_CfFlowInfo_PlaceL= 0; struct Cyc_CfFlowInfo_PlaceL_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_UnknownL=
0; static const int Cyc_CfFlowInfo_Zero= 0; static const int Cyc_CfFlowInfo_NotZeroAll=
1; static const int Cyc_CfFlowInfo_NotZeroThis= 2; static const int Cyc_CfFlowInfo_UnknownR=
0; struct Cyc_CfFlowInfo_UnknownR_struct{ int tag; void* f1; } ; static const
int Cyc_CfFlowInfo_Esc= 1; struct Cyc_CfFlowInfo_Esc_struct{ int tag; void* f1;
} ; static const int Cyc_CfFlowInfo_AddressOf= 2; struct Cyc_CfFlowInfo_AddressOf_struct{
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
encloser; int visited; void* flow; } ; unsigned char Cyc_NewControlFlow_CFAnnot[
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
_LL21: for( 0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp24=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp25=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp25[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp26; _temp26.tag= Cyc_CfFlowInfo_VarRoot;
_temp26.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp26;}); _temp25;}); _temp22= Cyc_Dict_insert( _temp22,( void*)
_temp24, Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( vds))->hd)->type, leafval));} return( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp27=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
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
0; _temp42;});} if((( struct Cyc_List_List*) _check_null( es))->tl ==  0){ void*
_temp45; void* _temp47; struct _tuple0 _temp43= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
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
env, inflow,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
_LL57: _temp56= _temp52.f1; goto _LL55; _LL55: _temp54= _temp52.f2; goto _LL53;
_LL53: outflow= _temp56; rvals=({ struct Cyc_List_List* _temp58=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp58->hd=( void*) _temp54; _temp58->tl=
0; _temp58;}); this_all_changed=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( this_all_changed,*(( struct Cyc_Set_Set**)
_check_null( env->all_changed)));{ struct Cyc_List_List* es2=(( struct Cyc_List_List*)
_check_null( es))->tl; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ env->all_changed=({ struct Cyc_Set_Set** _temp59=(
struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*)); _temp59[ 0]= Cyc_CfFlowInfo_mt_place_set();
_temp59;});{ void* _temp62; void* _temp64; struct _tuple0 _temp60= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es2))->hd);
_LL65: _temp64= _temp60.f1; goto _LL63; _LL63: _temp62= _temp60.f2; goto _LL61;
_LL61: rvals=({ struct Cyc_List_List* _temp66=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp66->hd=( void*) _temp62; _temp66->tl= rvals;
_temp66;}); outflow= Cyc_CfFlowInfo_after_flow(( struct Cyc_Set_Set**)&
this_all_changed, outflow, _temp64, this_all_changed,*(( struct Cyc_Set_Set**)
_check_null( env->all_changed))); this_all_changed=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
this_all_changed,*(( struct Cyc_Set_Set**) _check_null( env->all_changed)));}}}
old_inflow= inflow; inflow= Cyc_CfFlowInfo_join_flow( outer_all_changed, inflow,
outflow);}} while ( ! Cyc_CfFlowInfo_flow_lessthan_approx( inflow, old_inflow));
if( outer_all_changed ==  0){ env->all_changed= 0;} else{ env->all_changed=({
struct Cyc_Set_Set** _temp67=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp67[ 0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(*(( struct Cyc_Set_Set**)
_check_null( outer_all_changed)), this_all_changed); _temp67;});} Cyc_NewControlFlow_update_tryflow(
env, outflow); return({ struct _tuple4 _temp68; _temp68.f1= outflow; _temp68.f2=
Cyc_List_imp_rev( rvals); _temp68;});}} static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* es){
struct Cyc_List_List* _temp71; void* _temp73; struct _tuple4 _temp69= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, es); _LL74: _temp73= _temp69.f1; goto _LL72; _LL72: _temp71=
_temp69.f2; goto _LL70; _LL70:{ void* _temp75= _temp73; struct Cyc_Dict_Dict*
_temp81; _LL77: if(( unsigned int) _temp75 >  1u?*(( int*) _temp75) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL82: _temp81=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp75)->f1;
goto _LL78;} else{ goto _LL79;} _LL79: if( _temp75 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL80;} else{ goto _LL76;} _LL78: for( 0; _temp71 !=  0;( _temp71=(( struct
Cyc_List_List*) _check_null( _temp71))->tl, es=(( struct Cyc_List_List*)
_check_null( es))->tl)){ if( Cyc_CfFlowInfo_initlevel( _temp81,( void*)(( struct
Cyc_List_List*) _check_null( _temp71))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({
void* _temp83[ 0u]={}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
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
_temp157; void* _temp159; _LL149: if(( unsigned int) _temp147 >  3u?*(( int*)
_temp147) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL158: _temp157=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp147)->f1; goto _LL150;} else{ goto _LL151;} _LL151: if( _temp147 == ( void*)
Cyc_CfFlowInfo_Zero){ goto _LL152;} else{ goto _LL153;} _LL153: if((
unsigned int) _temp147 >  3u?*(( int*) _temp147) ==  Cyc_CfFlowInfo_UnknownR: 0){
_LL160: _temp159=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp147)->f1;
goto _LL154;} else{ goto _LL155;} _LL155: goto _LL156; _LL150: return({ struct
_tuple0 _temp161; _temp161.f1= f; _temp161.f2= Cyc_CfFlowInfo_lookup_place(
_temp144, _temp157); _temp161;}); _LL152: return({ struct _tuple0 _temp162;
_temp162.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp162.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp136, Cyc_CfFlowInfo_unknown_all); _temp162;}); _LL154: f= Cyc_NewControlFlow_notzero(
env, inflow, f, e, _temp159); goto _LL148; _LL156: goto _LL148; _LL148:;}{ void*
_temp163= Cyc_CfFlowInfo_initlevel( _temp144, r); _LL165: if( _temp163 == ( void*)
Cyc_CfFlowInfo_NoneIL){ goto _LL166;} else{ goto _LL167;} _LL167: if( _temp163
== ( void*) Cyc_CfFlowInfo_AllIL){ goto _LL168;} else{ goto _LL169;} _LL169: if(
_temp163 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL170;} else{ goto _LL164;}
_LL166:({ void* _temp171[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("dereference of possibly uninitialized pointer",
sizeof( unsigned char), 46u), _tag_arr( _temp171, sizeof( void*), 0u));}); goto
_LL168; _LL168: return({ struct _tuple0 _temp172; _temp172.f1= f; _temp172.f2=
Cyc_CfFlowInfo_typ_to_absrval( _temp136, Cyc_CfFlowInfo_unknown_all); _temp172;});
_LL170: return({ struct _tuple0 _temp173; _temp173.f1= f; _temp173.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp136, Cyc_CfFlowInfo_unknown_none); _temp173;}); _LL164:;} _LL139:;} _LL131:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp174=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp174[ 0]=({ struct
Cyc_Core_Impossible_struct _temp175; _temp175.tag= Cyc_Core_Impossible; _temp175.f1=
_tag_arr("right deref of non-pointer-type", sizeof( unsigned char), 32u);
_temp175;}); _temp174;})); _LL127:;} struct _tuple5{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e){
struct Cyc_Dict_Dict* d;{ void* _temp176= inflow; struct Cyc_Dict_Dict* _temp182;
_LL178: if( _temp176 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL179;} else{
goto _LL180;} _LL180: if(( unsigned int) _temp176 >  1u?*(( int*) _temp176) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL183: _temp182=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp176)->f1; goto _LL181;} else{ goto _LL177;} _LL179: return({ struct _tuple0
_temp184; _temp184.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp184.f2= Cyc_CfFlowInfo_unknown_all;
_temp184;}); _LL181: d= _temp182; _LL177:;}{ void* _temp185=( void*) e->r;
struct Cyc_Absyn_Exp* _temp283; struct Cyc_Absyn_Exp* _temp285; struct Cyc_Absyn_Exp*
_temp287; void* _temp289; void* _temp291; int _temp293; void* _temp295; void*
_temp297; void* _temp299; void* _temp301; struct Cyc_List_List* _temp303; void*
_temp305; void* _temp307; struct Cyc_Absyn_Vardecl* _temp309; void* _temp311;
struct Cyc_Absyn_Vardecl* _temp313; void* _temp315; struct Cyc_Absyn_Vardecl*
_temp317; struct Cyc_List_List* _temp319; struct Cyc_Absyn_Exp* _temp321; struct
Cyc_Absyn_Exp* _temp323; struct Cyc_Core_Opt* _temp325; struct Cyc_Core_Opt
_temp327; struct Cyc_Absyn_Exp* _temp328; struct Cyc_Absyn_Exp* _temp330; struct
Cyc_Core_Opt* _temp332; struct Cyc_Absyn_Exp* _temp334; struct Cyc_Absyn_Exp*
_temp336; struct Cyc_Absyn_Exp* _temp338; struct Cyc_Absyn_Exp* _temp340; struct
Cyc_List_List* _temp342; struct Cyc_Absyn_Exp* _temp344; void* _temp346; struct
Cyc_Absyn_Exp* _temp348; struct Cyc_Absyn_Exp* _temp350; struct Cyc_Absyn_Exp*
_temp352; struct Cyc_Absyn_Exp* _temp354; struct Cyc_Absyn_Exp* _temp356; struct
_tagged_arr* _temp358; struct Cyc_Absyn_Exp* _temp360; struct _tagged_arr*
_temp362; struct Cyc_Absyn_Exp* _temp364; struct Cyc_Absyn_Exp* _temp366; struct
Cyc_Absyn_Exp* _temp368; struct Cyc_Absyn_Exp* _temp370; struct Cyc_Absyn_Exp*
_temp372; struct Cyc_Absyn_Exp* _temp374; struct Cyc_List_List* _temp376; struct
Cyc_List_List* _temp378; struct Cyc_List_List* _temp380; struct Cyc_List_List*
_temp382; struct Cyc_List_List* _temp384; struct Cyc_Absyn_Exp* _temp386; struct
Cyc_Absyn_Exp* _temp388; struct Cyc_Absyn_Vardecl* _temp390; struct Cyc_Absyn_Stmt*
_temp392; void* _temp394; _LL187: if(*(( int*) _temp185) ==  Cyc_Absyn_Cast_e){
_LL284: _temp283=(( struct Cyc_Absyn_Cast_e_struct*) _temp185)->f2; goto _LL188;}
else{ goto _LL189;} _LL189: if(*(( int*) _temp185) ==  Cyc_Absyn_NoInstantiate_e){
_LL286: _temp285=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp185)->f1;
goto _LL190;} else{ goto _LL191;} _LL191: if(*(( int*) _temp185) ==  Cyc_Absyn_Instantiate_e){
_LL288: _temp287=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp185)->f1; goto
_LL192;} else{ goto _LL193;} _LL193: if(*(( int*) _temp185) ==  Cyc_Absyn_Const_e){
_LL290: _temp289=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp185)->f1; if(
_temp289 == ( void*) Cyc_Absyn_Null_c){ goto _LL194;} else{ goto _LL195;}} else{
goto _LL195;} _LL195: if(*(( int*) _temp185) ==  Cyc_Absyn_Const_e){ _LL292:
_temp291=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp185)->f1; if((
unsigned int) _temp291 >  1u?*(( int*) _temp291) ==  Cyc_Absyn_Int_c: 0){ _LL296:
_temp295=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp291)->f1; if( _temp295
== ( void*) Cyc_Absyn_Signed){ goto _LL294;} else{ goto _LL197;} _LL294:
_temp293=(( struct Cyc_Absyn_Int_c_struct*) _temp291)->f2; if( _temp293 ==  0){
goto _LL196;} else{ goto _LL197;}} else{ goto _LL197;}} else{ goto _LL197;}
_LL197: if(*(( int*) _temp185) ==  Cyc_Absyn_Const_e){ _LL298: _temp297=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp185)->f1; if(( unsigned int) _temp297 > 
1u?*(( int*) _temp297) ==  Cyc_Absyn_Int_c: 0){ _LL300: _temp299=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp297)->f1; if( _temp299 == ( void*) Cyc_Absyn_Signed){
goto _LL198;} else{ goto _LL199;}} else{ goto _LL199;}} else{ goto _LL199;}
_LL199: if(*(( int*) _temp185) ==  Cyc_Absyn_Var_e){ _LL302: _temp301=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp185)->f2; if(( unsigned int) _temp301 >  1u?*((
int*) _temp301) ==  Cyc_Absyn_Funname_b: 0){ goto _LL200;} else{ goto _LL201;}}
else{ goto _LL201;} _LL201: if(*(( int*) _temp185) ==  Cyc_Absyn_Tunion_e){
_LL304: _temp303=(( struct Cyc_Absyn_Tunion_e_struct*) _temp185)->f3; if(
_temp303 ==  0){ goto _LL202;} else{ goto _LL203;}} else{ goto _LL203;} _LL203:
if(*(( int*) _temp185) ==  Cyc_Absyn_Const_e){ goto _LL204;} else{ goto _LL205;}
_LL205: if(*(( int*) _temp185) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL206;} else{
goto _LL207;} _LL207: if(*(( int*) _temp185) ==  Cyc_Absyn_Sizeoftyp_e){ goto
_LL208;} else{ goto _LL209;} _LL209: if(*(( int*) _temp185) ==  Cyc_Absyn_Offsetof_e){
goto _LL210;} else{ goto _LL211;} _LL211: if(*(( int*) _temp185) ==  Cyc_Absyn_Gentyp_e){
goto _LL212;} else{ goto _LL213;} _LL213: if(*(( int*) _temp185) ==  Cyc_Absyn_AnonEnum_e){
goto _LL214;} else{ goto _LL215;} _LL215: if(*(( int*) _temp185) ==  Cyc_Absyn_Enum_e){
goto _LL216;} else{ goto _LL217;} _LL217: if(*(( int*) _temp185) ==  Cyc_Absyn_Var_e){
_LL306: _temp305=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp185)->f2; if((
unsigned int) _temp305 >  1u?*(( int*) _temp305) ==  Cyc_Absyn_Global_b: 0){
goto _LL218;} else{ goto _LL219;}} else{ goto _LL219;} _LL219: if(*(( int*)
_temp185) ==  Cyc_Absyn_Var_e){ _LL308: _temp307=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp185)->f2; if(( unsigned int) _temp307 >  1u?*(( int*) _temp307) ==  Cyc_Absyn_Param_b:
0){ _LL310: _temp309=(( struct Cyc_Absyn_Param_b_struct*) _temp307)->f1; goto
_LL220;} else{ goto _LL221;}} else{ goto _LL221;} _LL221: if(*(( int*) _temp185)
==  Cyc_Absyn_Var_e){ _LL312: _temp311=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp185)->f2; if(( unsigned int) _temp311 >  1u?*(( int*) _temp311) ==  Cyc_Absyn_Local_b:
0){ _LL314: _temp313=(( struct Cyc_Absyn_Local_b_struct*) _temp311)->f1; goto
_LL222;} else{ goto _LL223;}} else{ goto _LL223;} _LL223: if(*(( int*) _temp185)
==  Cyc_Absyn_Var_e){ _LL316: _temp315=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp185)->f2; if(( unsigned int) _temp315 >  1u?*(( int*) _temp315) ==  Cyc_Absyn_Pat_b:
0){ _LL318: _temp317=(( struct Cyc_Absyn_Pat_b_struct*) _temp315)->f1; goto
_LL224;} else{ goto _LL225;}} else{ goto _LL225;} _LL225: if(*(( int*) _temp185)
==  Cyc_Absyn_Primop_e){ _LL320: _temp319=(( struct Cyc_Absyn_Primop_e_struct*)
_temp185)->f2; goto _LL226;} else{ goto _LL227;} _LL227: if(*(( int*) _temp185)
==  Cyc_Absyn_Increment_e){ _LL322: _temp321=(( struct Cyc_Absyn_Increment_e_struct*)
_temp185)->f1; goto _LL228;} else{ goto _LL229;} _LL229: if(*(( int*) _temp185)
==  Cyc_Absyn_AssignOp_e){ _LL329: _temp328=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp185)->f1; goto _LL326; _LL326: _temp325=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp185)->f2; if( _temp325 ==  0){ goto _LL231;} else{ _temp327=* _temp325;
goto _LL324;} _LL324: _temp323=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp185)->f3;
goto _LL230;} else{ goto _LL231;} _LL231: if(*(( int*) _temp185) ==  Cyc_Absyn_AssignOp_e){
_LL335: _temp334=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp185)->f1; goto
_LL333; _LL333: _temp332=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp185)->f2;
if( _temp332 ==  0){ goto _LL331;} else{ goto _LL233;} _LL331: _temp330=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp185)->f3; goto _LL232;} else{ goto
_LL233;} _LL233: if(*(( int*) _temp185) ==  Cyc_Absyn_SeqExp_e){ _LL339:
_temp338=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp185)->f1; goto _LL337;
_LL337: _temp336=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp185)->f2; goto
_LL234;} else{ goto _LL235;} _LL235: if(*(( int*) _temp185) ==  Cyc_Absyn_Throw_e){
_LL341: _temp340=(( struct Cyc_Absyn_Throw_e_struct*) _temp185)->f1; goto _LL236;}
else{ goto _LL237;} _LL237: if(*(( int*) _temp185) ==  Cyc_Absyn_FnCall_e){
_LL345: _temp344=(( struct Cyc_Absyn_FnCall_e_struct*) _temp185)->f1; goto
_LL343; _LL343: _temp342=(( struct Cyc_Absyn_FnCall_e_struct*) _temp185)->f2;
goto _LL238;} else{ goto _LL239;} _LL239: if(*(( int*) _temp185) ==  Cyc_Absyn_Malloc_e){
_LL349: _temp348=(( struct Cyc_Absyn_Malloc_e_struct*) _temp185)->f1; goto
_LL347; _LL347: _temp346=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp185)->f2;
goto _LL240;} else{ goto _LL241;} _LL241: if(*(( int*) _temp185) ==  Cyc_Absyn_New_e){
_LL353: _temp352=(( struct Cyc_Absyn_New_e_struct*) _temp185)->f1; goto _LL351;
_LL351: _temp350=(( struct Cyc_Absyn_New_e_struct*) _temp185)->f2; goto _LL242;}
else{ goto _LL243;} _LL243: if(*(( int*) _temp185) ==  Cyc_Absyn_Address_e){
_LL355: _temp354=(( struct Cyc_Absyn_Address_e_struct*) _temp185)->f1; goto
_LL244;} else{ goto _LL245;} _LL245: if(*(( int*) _temp185) ==  Cyc_Absyn_Deref_e){
_LL357: _temp356=(( struct Cyc_Absyn_Deref_e_struct*) _temp185)->f1; goto _LL246;}
else{ goto _LL247;} _LL247: if(*(( int*) _temp185) ==  Cyc_Absyn_StructMember_e){
_LL361: _temp360=(( struct Cyc_Absyn_StructMember_e_struct*) _temp185)->f1; goto
_LL359; _LL359: _temp358=(( struct Cyc_Absyn_StructMember_e_struct*) _temp185)->f2;
goto _LL248;} else{ goto _LL249;} _LL249: if(*(( int*) _temp185) ==  Cyc_Absyn_StructArrow_e){
_LL365: _temp364=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp185)->f1; goto
_LL363; _LL363: _temp362=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp185)->f2;
goto _LL250;} else{ goto _LL251;} _LL251: if(*(( int*) _temp185) ==  Cyc_Absyn_Conditional_e){
_LL371: _temp370=(( struct Cyc_Absyn_Conditional_e_struct*) _temp185)->f1; goto
_LL369; _LL369: _temp368=(( struct Cyc_Absyn_Conditional_e_struct*) _temp185)->f2;
goto _LL367; _LL367: _temp366=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp185)->f3; goto _LL252;} else{ goto _LL253;} _LL253: if(*(( int*) _temp185)
==  Cyc_Absyn_Subscript_e){ _LL375: _temp374=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp185)->f1; goto _LL373; _LL373: _temp372=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp185)->f2; goto _LL254;} else{ goto _LL255;} _LL255: if(*(( int*) _temp185)
==  Cyc_Absyn_Tunion_e){ _LL377: _temp376=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp185)->f3; goto _LL256;} else{ goto _LL257;} _LL257: if(*(( int*) _temp185)
==  Cyc_Absyn_Tuple_e){ _LL379: _temp378=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp185)->f1; goto _LL258;} else{ goto _LL259;} _LL259: if(*(( int*) _temp185)
==  Cyc_Absyn_AnonStruct_e){ _LL381: _temp380=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp185)->f2; goto _LL260;} else{ goto _LL261;} _LL261: if(*(( int*) _temp185)
==  Cyc_Absyn_Struct_e){ _LL383: _temp382=(( struct Cyc_Absyn_Struct_e_struct*)
_temp185)->f3; goto _LL262;} else{ goto _LL263;} _LL263: if(*(( int*) _temp185)
==  Cyc_Absyn_Array_e){ _LL385: _temp384=(( struct Cyc_Absyn_Array_e_struct*)
_temp185)->f1; goto _LL264;} else{ goto _LL265;} _LL265: if(*(( int*) _temp185)
==  Cyc_Absyn_Comprehension_e){ _LL391: _temp390=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp185)->f1; goto _LL389; _LL389: _temp388=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp185)->f2; goto _LL387; _LL387: _temp386=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp185)->f3; goto _LL266;} else{ goto _LL267;} _LL267: if(*(( int*) _temp185)
==  Cyc_Absyn_StmtExp_e){ _LL393: _temp392=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp185)->f1; goto _LL268;} else{ goto _LL269;} _LL269: if(*(( int*) _temp185)
==  Cyc_Absyn_Var_e){ _LL395: _temp394=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp185)->f2; if( _temp394 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL270;}
else{ goto _LL271;}} else{ goto _LL271;} _LL271: if(*(( int*) _temp185) ==  Cyc_Absyn_UnknownId_e){
goto _LL272;} else{ goto _LL273;} _LL273: if(*(( int*) _temp185) ==  Cyc_Absyn_UnknownCall_e){
goto _LL274;} else{ goto _LL275;} _LL275: if(*(( int*) _temp185) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL276;} else{ goto _LL277;} _LL277: if(*(( int*) _temp185) ==  Cyc_Absyn_CompoundLit_e){
goto _LL278;} else{ goto _LL279;} _LL279: if(*(( int*) _temp185) ==  Cyc_Absyn_Codegen_e){
goto _LL280;} else{ goto _LL281;} _LL281: if(*(( int*) _temp185) ==  Cyc_Absyn_Fill_e){
goto _LL282;} else{ goto _LL186;} _LL188: _temp285= _temp283; goto _LL190;
_LL190: _temp287= _temp285; goto _LL192; _LL192: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp287); _LL194: goto _LL196; _LL196: return({ struct _tuple0
_temp396; _temp396.f1= inflow; _temp396.f2=( void*) Cyc_CfFlowInfo_Zero;
_temp396;}); _LL198: goto _LL200; _LL200: return({ struct _tuple0 _temp397;
_temp397.f1= inflow; _temp397.f2=( void*) Cyc_CfFlowInfo_NotZeroAll; _temp397;});
_LL202: goto _LL204; _LL204: goto _LL206; _LL206: goto _LL208; _LL208: goto
_LL210; _LL210: goto _LL212; _LL212: goto _LL214; _LL214: goto _LL216; _LL216:
return({ struct _tuple0 _temp398; _temp398.f1= inflow; _temp398.f2= Cyc_CfFlowInfo_unknown_all;
_temp398;}); _LL218: return({ struct _tuple0 _temp399; _temp399.f1= inflow;
_temp399.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp399;}); _LL220:
_temp313= _temp309; goto _LL222; _LL222: _temp317= _temp313; goto _LL224; _LL224:
return({ struct _tuple0 _temp400; _temp400.f1= inflow; _temp400.f2= Cyc_Dict_lookup(
d,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp401=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp401[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp402; _temp402.tag= Cyc_CfFlowInfo_VarRoot;
_temp402.f1= _temp317; _temp402;}); _temp401;})); _temp400;}); _LL226: return
Cyc_NewControlFlow_anal_use_ints( env, inflow, _temp319); _LL228: return Cyc_NewControlFlow_anal_use_ints(
env, inflow,({ struct Cyc_Absyn_Exp* _temp403[ 1u]; _temp403[ 0u]= _temp321;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp403, sizeof( struct Cyc_Absyn_Exp*), 1u));})); _LL230: return Cyc_NewControlFlow_anal_use_ints(
env, inflow,({ struct Cyc_Absyn_Exp* _temp404[ 2u]; _temp404[ 1u]= _temp323;
_temp404[ 0u]= _temp328;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp404, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL232: { struct
Cyc_Set_Set** _temp405= env->all_changed; while( 1) { env->all_changed=({ struct
Cyc_Set_Set** _temp406=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp406[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp406;});{ void* _temp409; void*
_temp411; struct _tuple0 _temp407= Cyc_NewControlFlow_anal_Lexp( env, inflow,
_temp334); _LL412: _temp411= _temp407.f1; goto _LL410; _LL410: _temp409=
_temp407.f2; goto _LL408; _LL408: { struct Cyc_Set_Set* _temp413=*(( struct Cyc_Set_Set**)
_check_null( env->all_changed)); env->all_changed=({ struct Cyc_Set_Set**
_temp414=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*));
_temp414[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp414;});{ void* _temp417; void*
_temp419; struct _tuple0 _temp415= Cyc_NewControlFlow_anal_Rexp( env, inflow,
_temp330); _LL420: _temp419= _temp415.f1; goto _LL418; _LL418: _temp417=
_temp415.f2; goto _LL416; _LL416: { struct Cyc_Set_Set* _temp421=*(( struct Cyc_Set_Set**)
_check_null( env->all_changed)); void* _temp422= Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _temp413, _temp411, _temp419, _temp413, _temp421); void*
_temp423= Cyc_CfFlowInfo_join_flow( _temp405, inflow, _temp422); if( Cyc_CfFlowInfo_flow_lessthan_approx(
_temp423, inflow)){ if( _temp405 ==  0){ env->all_changed= 0;} else{*(( struct
Cyc_Set_Set**) _check_null( env->all_changed))=(( struct Cyc_Set_Set*(*)( struct
Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(*(( struct Cyc_Set_Set**)
_check_null( _temp405)),(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2)) Cyc_Set_union_two)( _temp413, _temp421));}{ void*
_temp424= _temp422; struct Cyc_Dict_Dict* _temp430; _LL426: if( _temp424 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL427;} else{ goto _LL428;} _LL428: if((
unsigned int) _temp424 >  1u?*(( int*) _temp424) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL431: _temp430=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp424)->f1;
goto _LL429;} else{ goto _LL425;} _LL427: return({ struct _tuple0 _temp432;
_temp432.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp432.f2= _temp417; _temp432;});
_LL429: { void* _temp433= _temp409; struct Cyc_CfFlowInfo_Place* _temp439;
_LL435: if(( unsigned int) _temp433 >  1u?*(( int*) _temp433) ==  Cyc_CfFlowInfo_PlaceL:
0){ _LL440: _temp439=(( struct Cyc_CfFlowInfo_PlaceL_struct*) _temp433)->f1;
goto _LL436;} else{ goto _LL437;} _LL437: if( _temp433 == ( void*) Cyc_CfFlowInfo_UnknownL){
goto _LL438;} else{ goto _LL434;} _LL436: _temp430= Cyc_CfFlowInfo_assign_place(
e->loc, _temp430, env->all_changed, _temp439, _temp417); _temp422=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp441=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp441[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp442; _temp442.tag= Cyc_CfFlowInfo_ReachableFL;
_temp442.f1= _temp430; _temp442;}); _temp441;}); Cyc_NewControlFlow_update_tryflow(
env, _temp422); return({ struct _tuple0 _temp443; _temp443.f1= _temp422;
_temp443.f2= _temp417; _temp443;}); _LL438: return({ struct _tuple0 _temp444;
_temp444.f1= Cyc_NewControlFlow_use_Rval( env, _temp330->loc, _temp422, _temp417);
_temp444.f2= _temp417; _temp444;}); _LL434:;} _LL425:;}} inflow= _temp423;}}}}}}
_LL234: return Cyc_NewControlFlow_anal_Rexp( env,( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp338)).f1, _temp336); _LL236: { void* _temp447; void* _temp449;
struct _tuple0 _temp445= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp340);
_LL450: _temp449= _temp445.f1; goto _LL448; _LL448: _temp447= _temp445.f2; goto
_LL446; _LL446: Cyc_NewControlFlow_use_Rval( env, _temp340->loc, _temp449,
_temp447); return({ struct _tuple0 _temp451; _temp451.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp451.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp451;});} _LL238:
_temp342=({ struct Cyc_List_List* _temp452=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp452->hd=( void*) _temp344; _temp452->tl=
_temp342; _temp452;});{ struct Cyc_List_List* _temp455; void* _temp457; struct
_tuple4 _temp453= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp342);
_LL458: _temp457= _temp453.f1; goto _LL456; _LL456: _temp455= _temp453.f2; goto
_LL454; _LL454: for( 0; _temp455 !=  0;( _temp455=(( struct Cyc_List_List*)
_check_null( _temp455))->tl, _temp342=(( struct Cyc_List_List*) _check_null(
_temp342))->tl)){ _temp457= Cyc_NewControlFlow_use_Rval( env,(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp342))->hd)->loc, _temp457,( void*)((
struct Cyc_List_List*) _check_null( _temp455))->hd);} return({ struct _tuple0
_temp459; _temp459.f1= _temp457; _temp459.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp459;});} _LL240: { void* root=( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct*
_temp478=( struct Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp478[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp479; _temp479.tag=
Cyc_CfFlowInfo_MallocPt; _temp479.f1= e; _temp479;}); _temp478;}); struct Cyc_CfFlowInfo_Place*
place=({ struct Cyc_CfFlowInfo_Place* _temp477=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp477->root=( void*) root;
_temp477->fields= 0; _temp477;}); void* rval=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct*
_temp475=( struct Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_AddressOf_struct)); _temp475[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct
_temp476; _temp476.tag= Cyc_CfFlowInfo_AddressOf; _temp476.f1= place; _temp476;});
_temp475;}); void* place_val= Cyc_CfFlowInfo_typ_to_absrval( _temp346, Cyc_CfFlowInfo_unknown_none);
if( env->all_changed !=  0){*(( struct Cyc_Set_Set**) _check_null( env->all_changed))=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( env->all_changed)), place);}
if( _temp348 !=  0){ void* _temp460=( Cyc_NewControlFlow_anal_Rexp( env, inflow,(
struct Cyc_Absyn_Exp*) _check_null( _temp348))).f1; struct Cyc_Dict_Dict*
_temp466; _LL462: if( _temp460 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL463;}
else{ goto _LL464;} _LL464: if(( unsigned int) _temp460 >  1u?*(( int*) _temp460)
==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL467: _temp466=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp460)->f1; goto _LL465;} else{ goto _LL461;} _LL463: return({ struct _tuple0
_temp468; _temp468.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp468.f2= rval;
_temp468;}); _LL465: return({ struct _tuple0 _temp469; _temp469.f1=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp470=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp470[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp471; _temp471.tag= Cyc_CfFlowInfo_ReachableFL;
_temp471.f1= Cyc_Dict_insert( _temp466, root, place_val); _temp471;}); _temp470;});
_temp469.f2= rval; _temp469;}); _LL461:;} return({ struct _tuple0 _temp472;
_temp472.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp473=(
struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct));
_temp473[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct _temp474; _temp474.tag=
Cyc_CfFlowInfo_ReachableFL; _temp474.f1= Cyc_Dict_insert( d, root, place_val);
_temp474;}); _temp473;}); _temp472.f2= rval; _temp472;});} _LL242: { void* root=(
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp508=( struct Cyc_CfFlowInfo_MallocPt_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp508[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp509; _temp509.tag= Cyc_CfFlowInfo_MallocPt;
_temp509.f1= _temp350; _temp509;}); _temp508;}); struct Cyc_CfFlowInfo_Place*
place=({ struct Cyc_CfFlowInfo_Place* _temp507=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp507->root=( void*) root;
_temp507->fields= 0; _temp507;}); void* rval=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct*
_temp505=( struct Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_AddressOf_struct)); _temp505[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct
_temp506; _temp506.tag= Cyc_CfFlowInfo_AddressOf; _temp506.f1= place; _temp506;});
_temp505;}); if( env->all_changed !=  0){*(( struct Cyc_Set_Set**) _check_null(
env->all_changed))=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( env->all_changed)),
place);}{ void* outflow; void* place_val; if( _temp352 !=  0){ struct Cyc_List_List*
_temp483; void* _temp485; struct _tuple4 _temp481= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp480[ 2u]; _temp480[ 1u]= _temp350;
_temp480[ 0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp352);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp480, sizeof( struct Cyc_Absyn_Exp*),
2u));})); _LL486: _temp485= _temp481.f1; goto _LL484; _LL484: _temp483= _temp481.f2;
goto _LL482; _LL482: outflow= _temp485; place_val=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp483))->tl))->hd;} else{
void* _temp489; void* _temp491; struct _tuple0 _temp487= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp350); _LL492: _temp491= _temp487.f1; goto _LL490; _LL490:
_temp489= _temp487.f2; goto _LL488; _LL488: outflow= _temp491; place_val=
_temp489;}{ void* _temp493= outflow; struct Cyc_Dict_Dict* _temp499; _LL495: if(
_temp493 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL496;} else{ goto _LL497;}
_LL497: if(( unsigned int) _temp493 >  1u?*(( int*) _temp493) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL500: _temp499=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp493)->f1;
goto _LL498;} else{ goto _LL494;} _LL496: return({ struct _tuple0 _temp501;
_temp501.f1= outflow; _temp501.f2= rval; _temp501;}); _LL498: return({ struct
_tuple0 _temp502; _temp502.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp503=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp503[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp504; _temp504.tag= Cyc_CfFlowInfo_ReachableFL; _temp504.f1= Cyc_Dict_insert(
_temp499, root, place_val); _temp504;}); _temp503;}); _temp502.f2= rval;
_temp502;}); _LL494:;}}} _LL244: { void* _temp512; void* _temp514; struct
_tuple0 _temp510= Cyc_NewControlFlow_anal_Lexp( env, inflow, _temp354); _LL515:
_temp514= _temp510.f1; goto _LL513; _LL513: _temp512= _temp510.f2; goto _LL511;
_LL511: { void* _temp516= _temp512; struct Cyc_CfFlowInfo_Place* _temp522;
_LL518: if( _temp516 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL519;} else{
goto _LL520;} _LL520: if(( unsigned int) _temp516 >  1u?*(( int*) _temp516) == 
Cyc_CfFlowInfo_PlaceL: 0){ _LL523: _temp522=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp516)->f1; goto _LL521;} else{ goto _LL517;} _LL519: return({ struct _tuple0
_temp524; _temp524.f1= _temp514; _temp524.f2=( void*) Cyc_CfFlowInfo_NotZeroAll;
_temp524;}); _LL521: return({ struct _tuple0 _temp525; _temp525.f1= _temp514;
_temp525.f2=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct* _temp526=( struct
Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_AddressOf_struct));
_temp526[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct _temp527; _temp527.tag=
Cyc_CfFlowInfo_AddressOf; _temp527.f1= _temp522; _temp527;}); _temp526;});
_temp525;}); _LL517:;}} _LL246: { void* _temp530; void* _temp532; struct _tuple0
_temp528= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp356); _LL533: _temp532=
_temp528.f1; goto _LL531; _LL531: _temp530= _temp528.f2; goto _LL529; _LL529:
return Cyc_NewControlFlow_anal_derefR( env, inflow, _temp532, _temp356, _temp530);}
_LL248: { void* _temp536; void* _temp538; struct _tuple0 _temp534= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp360); _LL539: _temp538= _temp534.f1; goto _LL537; _LL537:
_temp536= _temp534.f2; goto _LL535; _LL535:{ void* _temp540= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp360->topt))->v); _LL542: if((
unsigned int) _temp540 >  4u?*(( int*) _temp540) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL543;} else{ goto _LL544;} _LL544: if(( unsigned int) _temp540 >  4u?*((
int*) _temp540) ==  Cyc_Absyn_UnionType: 0){ goto _LL545;} else{ goto _LL546;}
_LL546: goto _LL547; _LL543: goto _LL545; _LL545: return({ struct _tuple0
_temp548; _temp548.f1= _temp538; _temp548.f2= _temp536; _temp548;}); _LL547:
goto _LL541; _LL541:;}{ void* _temp549= _temp536; struct Cyc_Dict_Dict* _temp555;
_LL551: if(( unsigned int) _temp549 >  3u?*(( int*) _temp549) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL556: _temp555=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp549)->f1;
goto _LL552;} else{ goto _LL553;} _LL553: goto _LL554; _LL552: return({ struct
_tuple0 _temp557; _temp557.f1= _temp538; _temp557.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp555, _temp358); _temp557;});
_LL554:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp558=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp558[ 0]=({ struct Cyc_Core_Impossible_struct _temp559; _temp559.tag= Cyc_Core_Impossible;
_temp559.f1= _tag_arr("anal_Rexp: StructMember", sizeof( unsigned char), 24u);
_temp559;}); _temp558;})); _LL550:;}} _LL250: { void* _temp562; void* _temp564;
struct _tuple0 _temp560= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp364);
_LL565: _temp564= _temp560.f1; goto _LL563; _LL563: _temp562= _temp560.f2; goto
_LL561; _LL561: { void* _temp568; void* _temp570; struct _tuple0 _temp566= Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp564, _temp364, _temp562); _LL571: _temp570= _temp566.f1; goto
_LL569; _LL569: _temp568= _temp566.f2; goto _LL567; _LL567:{ void* _temp572= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp364->topt))->v); _LL574: if((
unsigned int) _temp572 >  4u?*(( int*) _temp572) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL575;} else{ goto _LL576;} _LL576: if(( unsigned int) _temp572 >  4u?*((
int*) _temp572) ==  Cyc_Absyn_UnionType: 0){ goto _LL577;} else{ goto _LL578;}
_LL578: goto _LL579; _LL575: goto _LL577; _LL577: return({ struct _tuple0
_temp580; _temp580.f1= _temp570; _temp580.f2= _temp568; _temp580;}); _LL579:
goto _LL573; _LL573:;}{ void* _temp581= _temp568; struct Cyc_Dict_Dict* _temp587;
_LL583: if(( unsigned int) _temp581 >  3u?*(( int*) _temp581) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL588: _temp587=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp581)->f1;
goto _LL584;} else{ goto _LL585;} _LL585: goto _LL586; _LL584: return({ struct
_tuple0 _temp589; _temp589.f1= _temp570; _temp589.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp587, _temp362); _temp589;});
_LL586:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp590=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp590[ 0]=({ struct Cyc_Core_Impossible_struct _temp591; _temp591.tag= Cyc_Core_Impossible;
_temp591.f1= _tag_arr("anal_Rexp: StructArrow", sizeof( unsigned char), 23u);
_temp591;}); _temp590;})); _LL582:;}}} _LL252: { void* _temp594; void* _temp596;
struct _tuple0 _temp592= Cyc_NewControlFlow_anal_test( env, inflow, _temp370);
_LL597: _temp596= _temp592.f1; goto _LL595; _LL595: _temp594= _temp592.f2; goto
_LL593; _LL593: { void* _temp600; void* _temp602; struct _tuple0 _temp598= Cyc_NewControlFlow_anal_Rexp(
env, _temp596, _temp368); _LL603: _temp602= _temp598.f1; goto _LL601; _LL601:
_temp600= _temp598.f2; goto _LL599; _LL599: { void* _temp606; void* _temp608;
struct _tuple0 _temp604= Cyc_NewControlFlow_anal_Rexp( env, _temp594, _temp366);
_LL609: _temp608= _temp604.f1; goto _LL607; _LL607: _temp606= _temp604.f2; goto
_LL605; _LL605: return({ struct _tuple0 _temp610; _temp610.f1= Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_use_Rval( env, _temp368->loc, _temp602,
_temp600), Cyc_NewControlFlow_use_Rval( env, _temp366->loc, _temp608, _temp606));
_temp610.f2= Cyc_CfFlowInfo_unknown_all; _temp610;});}}} _LL254: { struct Cyc_List_List*
_temp614; void* _temp616; struct _tuple4 _temp612= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp611[ 2u]; _temp611[ 1u]= _temp372;
_temp611[ 0u]= _temp374;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp611, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL617: _temp616=
_temp612.f1; goto _LL615; _LL615: _temp614= _temp612.f2; goto _LL613; _LL613:{
void* _temp618= _temp616; struct Cyc_Dict_Dict* _temp624; _LL620: if((
unsigned int) _temp618 >  1u?*(( int*) _temp618) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL625: _temp624=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp618)->f1;
goto _LL621;} else{ goto _LL622;} _LL622: goto _LL623; _LL621: if( Cyc_CfFlowInfo_initlevel(
_temp624,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp614))->tl))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp626[ 0u]={}; Cyc_Tcutil_terr( _temp372->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp626, sizeof( void*), 0u));});} goto
_LL619; _LL623: goto _LL619; _LL619:;}{ void* _temp627= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp374->topt))->v); _LL629: if((
unsigned int) _temp627 >  4u?*(( int*) _temp627) ==  Cyc_Absyn_TupleType: 0){
goto _LL630;} else{ goto _LL631;} _LL631: goto _LL632; _LL630: { void* _temp633=(
void*)(( struct Cyc_List_List*) _check_null( _temp614))->hd; struct Cyc_Dict_Dict*
_temp639; _LL635: if(( unsigned int) _temp633 >  3u?*(( int*) _temp633) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL640: _temp639=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp633)->f1;
goto _LL636;} else{ goto _LL637;} _LL637: goto _LL638; _LL636: return({ struct
_tuple0 _temp641; _temp641.f1= _temp616; _temp641.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp639, Cyc_Absyn_fieldname(( int)
Cyc_Evexp_eval_const_uint_exp( _temp372))); _temp641;}); _LL638:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp642=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp642[ 0]=({ struct
Cyc_Core_Impossible_struct _temp643; _temp643.tag= Cyc_Core_Impossible; _temp643.f1=
_tag_arr("anal_Rexp: Subscript", sizeof( unsigned char), 21u); _temp643;});
_temp642;})); _LL634:;} _LL632: goto _LL628; _LL628:;} return Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp616, _temp374,( void*)(( struct Cyc_List_List*) _check_null(
_temp614))->hd);} _LL256: _temp378= _temp376; goto _LL258; _LL258: { struct Cyc_List_List*
_temp646; void* _temp648; struct _tuple4 _temp644= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp378); _LL649: _temp648= _temp644.f1; goto _LL647; _LL647:
_temp646= _temp644.f2; goto _LL645; _LL645: { struct Cyc_Dict_Dict* aggrdict=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int i= 0; for( 0; _temp646 !=  0;(
_temp646=(( struct Cyc_List_List*) _check_null( _temp646))->tl, ++ i)){ aggrdict=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void*
v)) Cyc_Dict_insert)( aggrdict, Cyc_Absyn_fieldname( i),( void*)(( struct Cyc_List_List*)
_check_null( _temp646))->hd);}} return({ struct _tuple0 _temp650; _temp650.f1=
_temp648; _temp650.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp651=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp651[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp652; _temp652.tag= Cyc_CfFlowInfo_Aggregate; _temp652.f1= aggrdict;
_temp652;}); _temp651;}); _temp650;});}} _LL260: _temp382= _temp380; goto _LL262;
_LL262: { struct Cyc_List_List* _temp655; void* _temp657; struct _tuple4
_temp653= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp382)); _LL658:
_temp657= _temp653.f1; goto _LL656; _LL656: _temp655= _temp653.f2; goto _LL654;
_LL654: { struct Cyc_Dict_Dict* aggrdict=(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{
int i= 0; for( 0; _temp655 !=  0;((( _temp655=(( struct Cyc_List_List*)
_check_null( _temp655))->tl, _temp382=(( struct Cyc_List_List*) _check_null(
_temp382))->tl)), ++ i)){ struct Cyc_List_List* ds=(*(( struct _tuple5*)((
struct Cyc_List_List*) _check_null( _temp382))->hd)).f1; for( 0; ds !=  0; ds=((
struct Cyc_List_List*) _check_null( ds))->tl){ void* _temp659=( void*)(( struct
Cyc_List_List*) _check_null( ds))->hd; struct _tagged_arr* _temp665; _LL661: if(*((
int*) _temp659) ==  Cyc_Absyn_ArrayElement){ goto _LL662;} else{ goto _LL663;}
_LL663: if(*(( int*) _temp659) ==  Cyc_Absyn_FieldName){ _LL666: _temp665=((
struct Cyc_Absyn_FieldName_struct*) _temp659)->f1; goto _LL664;} else{ goto
_LL660;} _LL662:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp667=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp667[ 0]=({ struct Cyc_Core_Impossible_struct _temp668; _temp668.tag= Cyc_Core_Impossible;
_temp668.f1= _tag_arr("anal_Rexp:Struct_e", sizeof( unsigned char), 19u);
_temp668;}); _temp667;})); _LL664: aggrdict=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict,
_temp665,( void*)(( struct Cyc_List_List*) _check_null( _temp655))->hd); _LL660:;}}}
return({ struct _tuple0 _temp669; _temp669.f1= _temp657; _temp669.f2=( void*)({
struct Cyc_CfFlowInfo_Aggregate_struct* _temp670=( struct Cyc_CfFlowInfo_Aggregate_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Aggregate_struct)); _temp670[ 0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _temp671; _temp671.tag= Cyc_CfFlowInfo_Aggregate;
_temp671.f1= aggrdict; _temp671;}); _temp670;}); _temp669;});}} _LL264: { struct
Cyc_List_List* _temp672=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp384); struct Cyc_List_List* _temp675; void*
_temp677; struct _tuple4 _temp673= Cyc_NewControlFlow_anal_unordered_Rexps( env,
inflow, _temp672); _LL678: _temp677= _temp673.f1; goto _LL676; _LL676: _temp675=
_temp673.f2; goto _LL674; _LL674: for( 0; _temp675 !=  0;( _temp675=(( struct
Cyc_List_List*) _check_null( _temp675))->tl, _temp672=(( struct Cyc_List_List*)
_check_null( _temp672))->tl)){ _temp677= Cyc_NewControlFlow_use_Rval( env,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp672))->hd)->loc,
_temp677,( void*)(( struct Cyc_List_List*) _check_null( _temp675))->hd);} return({
struct _tuple0 _temp679; _temp679.f1= _temp677; _temp679.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp679;});} _LL266: { void* _temp682; void* _temp684; struct _tuple0 _temp680=
Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp388); _LL685: _temp684= _temp680.f1;
goto _LL683; _LL683: _temp682= _temp680.f2; goto _LL681; _LL681: { void*
_temp686= _temp684; struct Cyc_Dict_Dict* _temp692; _LL688: if( _temp686 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL689;} else{ goto _LL690;} _LL690: if((
unsigned int) _temp686 >  1u?*(( int*) _temp686) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL693: _temp692=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp686)->f1;
goto _LL691;} else{ goto _LL687;} _LL689: return({ struct _tuple0 _temp694;
_temp694.f1= _temp684; _temp694.f2= Cyc_CfFlowInfo_unknown_all; _temp694;});
_LL691: if( Cyc_CfFlowInfo_initlevel( _temp692, _temp682) == ( void*) Cyc_CfFlowInfo_NoneIL){({
void* _temp695[ 0u]={}; Cyc_Tcutil_terr( _temp388->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp695, sizeof( void*), 0u));});}{
void* _temp696= _temp682; _LL698: if( _temp696 == ( void*) Cyc_CfFlowInfo_Zero){
goto _LL699;} else{ goto _LL700;} _LL700: if( _temp696 == ( void*) Cyc_CfFlowInfo_NotZeroThis){
goto _LL701;} else{ goto _LL702;} _LL702: if( _temp696 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL703;} else{ goto _LL704;} _LL704: if(( unsigned int) _temp696 >  3u?*((
int*) _temp696) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL705;} else{ goto
_LL706;} _LL706: goto _LL707; _LL699: return({ struct _tuple0 _temp708; _temp708.f1=
_temp684; _temp708.f2= Cyc_CfFlowInfo_unknown_all; _temp708;}); _LL701: goto
_LL703; _LL703: goto _LL705; _LL705: { struct Cyc_List_List _temp709=({ struct
Cyc_List_List _temp727; _temp727.hd=( void*) _temp390; _temp727.tl= 0; _temp727;});
_temp684= Cyc_NewControlFlow_add_vars( _temp684,( struct Cyc_List_List*)&
_temp709, Cyc_CfFlowInfo_unknown_all);{ void* _temp712; void* _temp714; struct
_tuple0 _temp710= Cyc_NewControlFlow_anal_Rexp( env, _temp684, _temp386); _LL715:
_temp714= _temp710.f1; goto _LL713; _LL713: _temp712= _temp710.f2; goto _LL711;
_LL711:{ void* _temp716= _temp714; struct Cyc_Dict_Dict* _temp722; _LL718: if(
_temp716 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL719;} else{ goto _LL720;}
_LL720: if(( unsigned int) _temp716 >  1u?*(( int*) _temp716) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL723: _temp722=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp716)->f1;
goto _LL721;} else{ goto _LL717;} _LL719: return({ struct _tuple0 _temp724;
_temp724.f1= _temp714; _temp724.f2= Cyc_CfFlowInfo_unknown_all; _temp724;});
_LL721: if( Cyc_CfFlowInfo_initlevel( _temp722, _temp712) != ( void*) Cyc_CfFlowInfo_AllIL){({
void* _temp725[ 0u]={}; Cyc_Tcutil_terr( _temp388->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp725, sizeof( void*), 0u));});
return({ struct _tuple0 _temp726; _temp726.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp726.f2= Cyc_CfFlowInfo_unknown_all; _temp726;});} _LL717:;} _temp684=
_temp714; goto _LL707;}} _LL707: while( 1) { struct Cyc_List_List _temp728=({
struct Cyc_List_List _temp746; _temp746.hd=( void*) _temp390; _temp746.tl= 0;
_temp746;}); _temp684= Cyc_NewControlFlow_add_vars( _temp684,( struct Cyc_List_List*)&
_temp728, Cyc_CfFlowInfo_unknown_all);{ void* _temp731; void* _temp733; struct
_tuple0 _temp729= Cyc_NewControlFlow_anal_Rexp( env, _temp684, _temp386); _LL734:
_temp733= _temp729.f1; goto _LL732; _LL732: _temp731= _temp729.f2; goto _LL730;
_LL730:{ void* _temp735= _temp733; struct Cyc_Dict_Dict* _temp741; _LL737: if(
_temp735 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL738;} else{ goto _LL739;}
_LL739: if(( unsigned int) _temp735 >  1u?*(( int*) _temp735) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL742: _temp741=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp735)->f1;
goto _LL740;} else{ goto _LL736;} _LL738: goto _LL736; _LL740: if( Cyc_CfFlowInfo_initlevel(
_temp741, _temp731) != ( void*) Cyc_CfFlowInfo_AllIL){({ void* _temp743[ 0u]={};
Cyc_Tcutil_terr( _temp388->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp743, sizeof( void*), 0u));});
return({ struct _tuple0 _temp744; _temp744.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp744.f2= Cyc_CfFlowInfo_unknown_all; _temp744;});} _LL736:;}{ void* _temp745=
Cyc_CfFlowInfo_join_flow( env->all_changed, _temp684, _temp733); if( Cyc_CfFlowInfo_flow_lessthan_approx(
_temp745, _temp684)){ break;} _temp684= _temp745;}}} return({ struct _tuple0
_temp747; _temp747.f1= _temp684; _temp747.f2= Cyc_CfFlowInfo_unknown_all;
_temp747;}); _LL697:;} _LL687:;}} _LL268: while( 1) { void* _temp748=( void*)
_temp392->r; struct Cyc_Absyn_Stmt* _temp758; struct Cyc_Absyn_Stmt* _temp760;
struct Cyc_Absyn_Stmt* _temp762; struct Cyc_Absyn_Decl* _temp764; struct Cyc_Absyn_Exp*
_temp766; _LL750: if(( unsigned int) _temp748 >  1u?*(( int*) _temp748) ==  Cyc_Absyn_Seq_s:
0){ _LL761: _temp760=(( struct Cyc_Absyn_Seq_s_struct*) _temp748)->f1; goto
_LL759; _LL759: _temp758=(( struct Cyc_Absyn_Seq_s_struct*) _temp748)->f2; goto
_LL751;} else{ goto _LL752;} _LL752: if(( unsigned int) _temp748 >  1u?*(( int*)
_temp748) ==  Cyc_Absyn_Decl_s: 0){ _LL765: _temp764=(( struct Cyc_Absyn_Decl_s_struct*)
_temp748)->f1; goto _LL763; _LL763: _temp762=(( struct Cyc_Absyn_Decl_s_struct*)
_temp748)->f2; goto _LL753;} else{ goto _LL754;} _LL754: if(( unsigned int)
_temp748 >  1u?*(( int*) _temp748) ==  Cyc_Absyn_Exp_s: 0){ _LL767: _temp766=((
struct Cyc_Absyn_Exp_s_struct*) _temp748)->f1; goto _LL755;} else{ goto _LL756;}
_LL756: goto _LL757; _LL751: inflow= Cyc_NewControlFlow_anal_stmt( env, inflow,
_temp760); _temp392= _temp758; continue; _LL753: inflow= Cyc_NewControlFlow_anal_decl(
env, inflow, _temp764); _temp392= _temp762; continue; _LL755: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp766); _LL757:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp768=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp768[ 0]=({ struct Cyc_Core_Impossible_struct _temp769; _temp769.tag= Cyc_Core_Impossible;
_temp769.f1= _tag_arr("analyze_Rexp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp769;}); _temp768;})); _LL749:;} _LL270: goto _LL272; _LL272: goto
_LL274; _LL274: goto _LL276; _LL276: goto _LL278; _LL278: goto _LL280; _LL280:
goto _LL282; _LL282:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp770=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp770[ 0]=({ struct Cyc_Core_Impossible_struct _temp771; _temp771.tag= Cyc_Core_Impossible;
_temp771.f1= _tag_arr("anal_Rexp, unexpected exp form", sizeof( unsigned char),
31u); _temp771;}); _temp770;})); _LL186:;}} static struct _tuple0 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* f, struct Cyc_Absyn_Exp*
e, void* r, struct Cyc_List_List* flds){ void* _temp772= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo
_temp778; struct Cyc_Absyn_Conref* _temp780; void* _temp782; _LL774: if((
unsigned int) _temp772 >  4u?*(( int*) _temp772) ==  Cyc_Absyn_PointerType: 0){
_LL779: _temp778=(( struct Cyc_Absyn_PointerType_struct*) _temp772)->f1; _LL783:
_temp782=( void*) _temp778.elt_typ; goto _LL781; _LL781: _temp780= _temp778.bounds;
goto _LL775;} else{ goto _LL776;} _LL776: goto _LL777; _LL775: { void* _temp784=
f; struct Cyc_Dict_Dict* _temp790; _LL786: if( _temp784 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL787;} else{ goto _LL788;} _LL788: if(( unsigned int) _temp784 >  1u?*((
int*) _temp784) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL791: _temp790=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp784)->f1; goto _LL789;} else{ goto
_LL785;} _LL787: return({ struct _tuple0 _temp792; _temp792.f1= f; _temp792.f2=(
void*) Cyc_CfFlowInfo_UnknownL; _temp792;}); _LL789: if( Cyc_Tcutil_is_bound_one(
_temp780)){ void* _temp793= r; struct Cyc_CfFlowInfo_Place* _temp803; struct Cyc_CfFlowInfo_Place
_temp805; struct Cyc_List_List* _temp806; void* _temp808; void* _temp810; _LL795:
if(( unsigned int) _temp793 >  3u?*(( int*) _temp793) ==  Cyc_CfFlowInfo_AddressOf:
0){ _LL804: _temp803=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp793)->f1;
_temp805=* _temp803; _LL809: _temp808=( void*) _temp805.root; goto _LL807;
_LL807: _temp806= _temp805.fields; goto _LL796;} else{ goto _LL797;} _LL797: if(
_temp793 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL798;} else{ goto _LL799;}
_LL799: if(( unsigned int) _temp793 >  3u?*(( int*) _temp793) ==  Cyc_CfFlowInfo_UnknownR:
0){ _LL811: _temp810=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp793)->f1;
goto _LL800;} else{ goto _LL801;} _LL801: goto _LL802; _LL796: return({ struct
_tuple0 _temp812; _temp812.f1= f; _temp812.f2=( void*)({ struct Cyc_CfFlowInfo_PlaceL_struct*
_temp813=( struct Cyc_CfFlowInfo_PlaceL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct));
_temp813[ 0]=({ struct Cyc_CfFlowInfo_PlaceL_struct _temp814; _temp814.tag= Cyc_CfFlowInfo_PlaceL;
_temp814.f1=({ struct Cyc_CfFlowInfo_Place* _temp815=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp815->root=( void*)
_temp808; _temp815->fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp806, flds); _temp815;});
_temp814;}); _temp813;}); _temp812;}); _LL798: return({ struct _tuple0 _temp816;
_temp816.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp816.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp816;}); _LL800: f= Cyc_NewControlFlow_notzero( env, inflow, f, e, _temp810);
goto _LL794; _LL802: goto _LL794; _LL794:;} if( Cyc_CfFlowInfo_initlevel(
_temp790, r) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp817[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("dereference of possibly uninitialized pointer", sizeof(
unsigned char), 46u), _tag_arr( _temp817, sizeof( void*), 0u));});} return({
struct _tuple0 _temp818; _temp818.f1= f; _temp818.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp818;}); _LL785:;} _LL777:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp819=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp819[ 0]=({ struct Cyc_Core_Impossible_struct _temp820; _temp820.tag= Cyc_Core_Impossible;
_temp820.f1= _tag_arr("left deref of non-pointer-type", sizeof( unsigned char),
31u); _temp820;}); _temp819;})); _LL773:;} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* flds){ struct Cyc_Dict_Dict* d;{ void* _temp821= inflow;
struct Cyc_Dict_Dict* _temp827; _LL823: if( _temp821 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL824;} else{ goto _LL825;} _LL825: if(( unsigned int) _temp821 >  1u?*((
int*) _temp821) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL828: _temp827=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp821)->f1; goto _LL826;} else{ goto
_LL822;} _LL824: return({ struct _tuple0 _temp829; _temp829.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp829.f2=( void*) Cyc_CfFlowInfo_UnknownL; _temp829;}); _LL826: d= _temp827;
_LL822:;}{ void* _temp830=( void*) e->r; struct Cyc_Absyn_Exp* _temp854; struct
Cyc_Absyn_Exp* _temp856; struct Cyc_Absyn_Exp* _temp858; void* _temp860; struct
Cyc_Absyn_Vardecl* _temp862; void* _temp864; struct Cyc_Absyn_Vardecl* _temp866;
void* _temp868; struct Cyc_Absyn_Vardecl* _temp870; struct _tagged_arr* _temp872;
struct Cyc_Absyn_Exp* _temp874; struct Cyc_Absyn_Exp* _temp876; struct Cyc_Absyn_Exp*
_temp878; struct Cyc_Absyn_Exp* _temp880; struct _tagged_arr* _temp882; struct
Cyc_Absyn_Exp* _temp884; _LL832: if(*(( int*) _temp830) ==  Cyc_Absyn_Cast_e){
_LL855: _temp854=(( struct Cyc_Absyn_Cast_e_struct*) _temp830)->f2; goto _LL833;}
else{ goto _LL834;} _LL834: if(*(( int*) _temp830) ==  Cyc_Absyn_NoInstantiate_e){
_LL857: _temp856=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp830)->f1;
goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp830) ==  Cyc_Absyn_Instantiate_e){
_LL859: _temp858=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp830)->f1; goto
_LL837;} else{ goto _LL838;} _LL838: if(*(( int*) _temp830) ==  Cyc_Absyn_Var_e){
_LL861: _temp860=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp830)->f2; if((
unsigned int) _temp860 >  1u?*(( int*) _temp860) ==  Cyc_Absyn_Param_b: 0){
_LL863: _temp862=(( struct Cyc_Absyn_Param_b_struct*) _temp860)->f1; goto _LL839;}
else{ goto _LL840;}} else{ goto _LL840;} _LL840: if(*(( int*) _temp830) ==  Cyc_Absyn_Var_e){
_LL865: _temp864=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp830)->f2; if((
unsigned int) _temp864 >  1u?*(( int*) _temp864) ==  Cyc_Absyn_Local_b: 0){
_LL867: _temp866=(( struct Cyc_Absyn_Local_b_struct*) _temp864)->f1; goto _LL841;}
else{ goto _LL842;}} else{ goto _LL842;} _LL842: if(*(( int*) _temp830) ==  Cyc_Absyn_Var_e){
_LL869: _temp868=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp830)->f2; if((
unsigned int) _temp868 >  1u?*(( int*) _temp868) ==  Cyc_Absyn_Pat_b: 0){ _LL871:
_temp870=(( struct Cyc_Absyn_Pat_b_struct*) _temp868)->f1; goto _LL843;} else{
goto _LL844;}} else{ goto _LL844;} _LL844: if(*(( int*) _temp830) ==  Cyc_Absyn_StructArrow_e){
_LL875: _temp874=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp830)->f1; goto
_LL873; _LL873: _temp872=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp830)->f2;
goto _LL845;} else{ goto _LL846;} _LL846: if(*(( int*) _temp830) ==  Cyc_Absyn_Deref_e){
_LL877: _temp876=(( struct Cyc_Absyn_Deref_e_struct*) _temp830)->f1; goto _LL847;}
else{ goto _LL848;} _LL848: if(*(( int*) _temp830) ==  Cyc_Absyn_Subscript_e){
_LL881: _temp880=(( struct Cyc_Absyn_Subscript_e_struct*) _temp830)->f1; goto
_LL879; _LL879: _temp878=(( struct Cyc_Absyn_Subscript_e_struct*) _temp830)->f2;
goto _LL849;} else{ goto _LL850;} _LL850: if(*(( int*) _temp830) ==  Cyc_Absyn_StructMember_e){
_LL885: _temp884=(( struct Cyc_Absyn_StructMember_e_struct*) _temp830)->f1; goto
_LL883; _LL883: _temp882=(( struct Cyc_Absyn_StructMember_e_struct*) _temp830)->f2;
goto _LL851;} else{ goto _LL852;} _LL852: goto _LL853; _LL833: _temp856=
_temp854; goto _LL835; _LL835: _temp858= _temp856; goto _LL837; _LL837: return
Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, _temp858, flds); _LL839: _temp866=
_temp862; goto _LL841; _LL841: _temp870= _temp866; goto _LL843; _LL843: return({
struct _tuple0 _temp886; _temp886.f1= inflow; _temp886.f2=( void*)({ struct Cyc_CfFlowInfo_PlaceL_struct*
_temp887=( struct Cyc_CfFlowInfo_PlaceL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct));
_temp887[ 0]=({ struct Cyc_CfFlowInfo_PlaceL_struct _temp888; _temp888.tag= Cyc_CfFlowInfo_PlaceL;
_temp888.f1=({ struct Cyc_CfFlowInfo_Place* _temp889=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp889->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp890=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp890[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp891; _temp891.tag= Cyc_CfFlowInfo_VarRoot;
_temp891.f1= _temp870; _temp891;}); _temp890;})); _temp889->fields= flds;
_temp889;}); _temp888;}); _temp887;}); _temp886;}); _LL845:{ void* _temp892= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp874->topt))->v); _LL894: if((
unsigned int) _temp892 >  4u?*(( int*) _temp892) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL895;} else{ goto _LL896;} _LL896: if(( unsigned int) _temp892 >  4u?*((
int*) _temp892) ==  Cyc_Absyn_UnionType: 0){ goto _LL897;} else{ goto _LL898;}
_LL898: goto _LL899; _LL895: goto _LL897; _LL897: goto _LL893; _LL899: flds=({
struct Cyc_List_List* _temp900=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp900->hd=( void*) _temp872; _temp900->tl= flds;
_temp900;}); _LL893:;} _temp876= _temp874; goto _LL847; _LL847: { void* _temp903;
void* _temp905; struct _tuple0 _temp901= Cyc_NewControlFlow_anal_Rexp( env,
inflow, _temp876); _LL906: _temp905= _temp901.f1; goto _LL904; _LL904: _temp903=
_temp901.f2; goto _LL902; _LL902: return Cyc_NewControlFlow_anal_derefL( env,
inflow, _temp905, _temp876, _temp903, flds);} _LL849:{ void* _temp907= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp880->topt))->v); _LL909: if((
unsigned int) _temp907 >  4u?*(( int*) _temp907) ==  Cyc_Absyn_TupleType: 0){
goto _LL910;} else{ goto _LL911;} _LL911: goto _LL912; _LL910: { struct
_tagged_arr* _temp913= Cyc_Absyn_fieldname(( int) Cyc_Evexp_eval_const_uint_exp(
_temp878)); return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, _temp880,({
struct Cyc_List_List* _temp914=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp914->hd=( void*) _temp913; _temp914->tl= flds;
_temp914;}));} _LL912: goto _LL908; _LL908:;}{ struct Cyc_List_List* _temp918;
void* _temp920; struct _tuple4 _temp916= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp915[ 2u]; _temp915[ 1u]= _temp878;
_temp915[ 0u]= _temp880;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp915, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL921: _temp920=
_temp916.f1; goto _LL919; _LL919: _temp918= _temp916.f2; goto _LL917; _LL917:{
void* _temp922= _temp920; struct Cyc_Dict_Dict* _temp928; _LL924: if((
unsigned int) _temp922 >  1u?*(( int*) _temp922) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL929: _temp928=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp922)->f1;
goto _LL925;} else{ goto _LL926;} _LL926: goto _LL927; _LL925: if( Cyc_CfFlowInfo_initlevel(
_temp928,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp918))->tl))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp930[ 0u]={}; Cyc_Tcutil_terr( _temp878->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp930, sizeof( void*), 0u));});} goto
_LL923; _LL927: goto _LL923; _LL923:;} return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp920, _temp880,( void*)(( struct Cyc_List_List*) _check_null(
_temp918))->hd, flds);} _LL851:{ void* _temp931= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp884->topt))->v); _LL933: if((
unsigned int) _temp931 >  4u?*(( int*) _temp931) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL934;} else{ goto _LL935;} _LL935: if(( unsigned int) _temp931 >  4u?*((
int*) _temp931) ==  Cyc_Absyn_UnionType: 0){ goto _LL936;} else{ goto _LL937;}
_LL937: goto _LL938; _LL934: goto _LL936; _LL936: goto _LL932; _LL938: flds=({
struct Cyc_List_List* _temp939=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp939->hd=( void*) _temp882; _temp939->tl= flds;
_temp939;}); _LL932:;} return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow,
_temp884, flds); _LL853: return({ struct _tuple0 _temp940; _temp940.f1=( void*)
Cyc_CfFlowInfo_BottomFL; _temp940.f2=( void*) Cyc_CfFlowInfo_UnknownL; _temp940;});
_LL831:;}} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, struct Cyc_Absyn_Exp* e){ return Cyc_NewControlFlow_anal_Lexp_rec(
env, inflow, e, 0);} static struct _tuple0 Cyc_NewControlFlow_anal_test( struct
Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e){{ void*
_temp941=( void*) e->r; struct Cyc_Absyn_Exp* _temp953; struct Cyc_Absyn_Exp*
_temp955; struct Cyc_Absyn_Exp* _temp957; struct Cyc_List_List* _temp959; struct
Cyc_List_List _temp961; struct Cyc_List_List* _temp962; struct Cyc_Absyn_Exp*
_temp964; void* _temp966; struct Cyc_List_List* _temp968; void* _temp970; struct
Cyc_List_List* _temp972; void* _temp974; _LL943: if(*(( int*) _temp941) ==  Cyc_Absyn_Conditional_e){
_LL958: _temp957=(( struct Cyc_Absyn_Conditional_e_struct*) _temp941)->f1; goto
_LL956; _LL956: _temp955=(( struct Cyc_Absyn_Conditional_e_struct*) _temp941)->f2;
goto _LL954; _LL954: _temp953=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp941)->f3; goto _LL944;} else{ goto _LL945;} _LL945: if(*(( int*) _temp941)
==  Cyc_Absyn_Primop_e){ _LL967: _temp966=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp941)->f1; if( _temp966 == ( void*) Cyc_Absyn_Not){ goto _LL960;} else{ goto
_LL947;} _LL960: _temp959=(( struct Cyc_Absyn_Primop_e_struct*) _temp941)->f2;
if( _temp959 ==  0){ goto _LL947;} else{ _temp961=* _temp959; _LL965: _temp964=(
struct Cyc_Absyn_Exp*) _temp961.hd; goto _LL963; _LL963: _temp962= _temp961.tl;
if( _temp962 ==  0){ goto _LL946;} else{ goto _LL947;}}} else{ goto _LL947;}
_LL947: if(*(( int*) _temp941) ==  Cyc_Absyn_Primop_e){ _LL971: _temp970=( void*)((
struct Cyc_Absyn_Primop_e_struct*) _temp941)->f1; if( _temp970 == ( void*) Cyc_Absyn_Eq){
goto _LL969;} else{ goto _LL949;} _LL969: _temp968=(( struct Cyc_Absyn_Primop_e_struct*)
_temp941)->f2; goto _LL948;} else{ goto _LL949;} _LL949: if(*(( int*) _temp941)
==  Cyc_Absyn_Primop_e){ _LL975: _temp974=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp941)->f1; if( _temp974 == ( void*) Cyc_Absyn_Neq){ goto _LL973;} else{ goto
_LL951;} _LL973: _temp972=(( struct Cyc_Absyn_Primop_e_struct*) _temp941)->f2;
goto _LL950;} else{ goto _LL951;} _LL951: goto _LL952; _LL944: { void* _temp978;
void* _temp980; struct _tuple0 _temp976= Cyc_NewControlFlow_anal_test( env,
inflow, _temp957); _LL981: _temp980= _temp976.f1; goto _LL979; _LL979: _temp978=
_temp976.f2; goto _LL977; _LL977: { void* _temp984; void* _temp986; struct
_tuple0 _temp982= Cyc_NewControlFlow_anal_test( env, _temp980, _temp955); _LL987:
_temp986= _temp982.f1; goto _LL985; _LL985: _temp984= _temp982.f2; goto _LL983;
_LL983: { void* _temp990; void* _temp992; struct _tuple0 _temp988= Cyc_NewControlFlow_anal_test(
env, _temp978, _temp953); _LL993: _temp992= _temp988.f1; goto _LL991; _LL991:
_temp990= _temp988.f2; goto _LL989; _LL989: return({ struct _tuple0 _temp994;
_temp994.f1= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp986, _temp992);
_temp994.f2= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp984, _temp990);
_temp994;});}}} _LL946: { void* _temp997; void* _temp999; struct _tuple0
_temp995= Cyc_NewControlFlow_anal_test( env, inflow, _temp964); _LL1000:
_temp999= _temp995.f1; goto _LL998; _LL998: _temp997= _temp995.f2; goto _LL996;
_LL996: return({ struct _tuple0 _temp1001; _temp1001.f1= _temp997; _temp1001.f2=
_temp999; _temp1001;});} _LL948: { struct Cyc_List_List* _temp1004; void*
_temp1006; struct _tuple4 _temp1002= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp968); _LL1007: _temp1006= _temp1002.f1; goto _LL1005; _LL1005:
_temp1004= _temp1002.f2; goto _LL1003; _LL1003: { void* _temp1008= _temp1006;
struct Cyc_Dict_Dict* _temp1014; _LL1010: if( _temp1008 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1011;} else{ goto _LL1012;} _LL1012: if(( unsigned int) _temp1008 >  1u?*((
int*) _temp1008) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1015: _temp1014=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1008)->f1; goto _LL1013;} else{
goto _LL1009;} _LL1011: return({ struct _tuple0 _temp1016; _temp1016.f1=
_temp1006; _temp1016.f2= _temp1006; _temp1016;}); _LL1013: { void* _temp1017=(
void*)(( struct Cyc_List_List*) _check_null( _temp1004))->hd; void* _temp1018=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1004))->tl))->hd; if( Cyc_CfFlowInfo_initlevel( _temp1014,
_temp1017) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1019[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp968))->hd)->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp1019, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1014, _temp1018) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1020[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp968))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1020, sizeof( void*), 0u));});}{
struct _tuple0 _temp1022=({ struct _tuple0 _temp1021; _temp1021.f1= _temp1017;
_temp1021.f2= _temp1018; _temp1021;}); void* _temp1030; void* _temp1032; void*
_temp1034; void* _temp1036; void* _temp1038; void* _temp1040; _LL1024: _LL1033:
_temp1032= _temp1022.f1; if(( unsigned int) _temp1032 >  3u?*(( int*) _temp1032)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1035: _temp1034=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1032)->f1; goto _LL1031;} else{ goto _LL1026;} _LL1031: _temp1030=
_temp1022.f2; if( _temp1030 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1025;}
else{ goto _LL1026;} _LL1026: _LL1041: _temp1040= _temp1022.f1; if( _temp1040 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1037;} else{ goto _LL1028;} _LL1037:
_temp1036= _temp1022.f2; if(( unsigned int) _temp1036 >  3u?*(( int*) _temp1036)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1039: _temp1038=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1036)->f1; goto _LL1027;} else{ goto _LL1028;} _LL1028: goto _LL1029;
_LL1025: { void* _temp1044; void* _temp1046; struct _tuple0 _temp1042= Cyc_NewControlFlow_splitzero(
env, _temp1006, _temp1006,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp968))->hd, _temp1034); _LL1047: _temp1046= _temp1042.f1; goto
_LL1045; _LL1045: _temp1044= _temp1042.f2; goto _LL1043; _LL1043: return({
struct _tuple0 _temp1048; _temp1048.f1= _temp1044; _temp1048.f2= _temp1046;
_temp1048;});} _LL1027: { void* _temp1051; void* _temp1053; struct _tuple0
_temp1049= Cyc_NewControlFlow_splitzero( env, _temp1006, _temp1006,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp968))->tl))->hd, _temp1038); _LL1054: _temp1053= _temp1049.f1; goto _LL1052;
_LL1052: _temp1051= _temp1049.f2; goto _LL1050; _LL1050: return({ struct _tuple0
_temp1055; _temp1055.f1= _temp1051; _temp1055.f2= _temp1053; _temp1055;});}
_LL1029: return({ struct _tuple0 _temp1056; _temp1056.f1= _temp1006; _temp1056.f2=
_temp1006; _temp1056;}); _LL1023:;}} _LL1009:;}} _LL950: { struct Cyc_List_List*
_temp1059; void* _temp1061; struct _tuple4 _temp1057= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp972); _LL1062: _temp1061= _temp1057.f1; goto _LL1060; _LL1060:
_temp1059= _temp1057.f2; goto _LL1058; _LL1058: { void* _temp1063= _temp1061;
struct Cyc_Dict_Dict* _temp1069; _LL1065: if( _temp1063 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1066;} else{ goto _LL1067;} _LL1067: if(( unsigned int) _temp1063 >  1u?*((
int*) _temp1063) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1070: _temp1069=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1063)->f1; goto _LL1068;} else{
goto _LL1064;} _LL1066: return({ struct _tuple0 _temp1071; _temp1071.f1=
_temp1061; _temp1071.f2= _temp1061; _temp1071;}); _LL1068: { void* _temp1072=(
void*)(( struct Cyc_List_List*) _check_null( _temp1059))->hd; void* _temp1073=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1059))->tl))->hd; if( Cyc_CfFlowInfo_initlevel( _temp1069,
_temp1072) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1074[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp972))->hd)->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp1074, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1069, _temp1073) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1075[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp972))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1075, sizeof( void*), 0u));});}{
struct _tuple0 _temp1077=({ struct _tuple0 _temp1076; _temp1076.f1= _temp1072;
_temp1076.f2= _temp1073; _temp1076;}); void* _temp1085; void* _temp1087; void*
_temp1089; void* _temp1091; void* _temp1093; void* _temp1095; _LL1079: _LL1088:
_temp1087= _temp1077.f1; if(( unsigned int) _temp1087 >  3u?*(( int*) _temp1087)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1090: _temp1089=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1087)->f1; goto _LL1086;} else{ goto _LL1081;} _LL1086: _temp1085=
_temp1077.f2; if( _temp1085 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1080;}
else{ goto _LL1081;} _LL1081: _LL1096: _temp1095= _temp1077.f1; if( _temp1095 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1092;} else{ goto _LL1083;} _LL1092:
_temp1091= _temp1077.f2; if(( unsigned int) _temp1091 >  3u?*(( int*) _temp1091)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1094: _temp1093=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1091)->f1; goto _LL1082;} else{ goto _LL1083;} _LL1083: goto _LL1084;
_LL1080: return Cyc_NewControlFlow_splitzero( env, _temp1061, _temp1061,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp972))->hd, _temp1089);
_LL1082: return Cyc_NewControlFlow_splitzero( env, _temp1061, _temp1061,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp972))->tl))->hd, _temp1093); _LL1084: return({ struct _tuple0
_temp1097; _temp1097.f1= _temp1061; _temp1097.f2= _temp1061; _temp1097;});
_LL1078:;}} _LL1064:;}} _LL952: goto _LL942; _LL942:;}{ void* _temp1100; void*
_temp1102; struct _tuple0 _temp1098= Cyc_NewControlFlow_anal_Rexp( env, inflow,
e); _LL1103: _temp1102= _temp1098.f1; goto _LL1101; _LL1101: _temp1100=
_temp1098.f2; goto _LL1099; _LL1099: { void* _temp1104= _temp1102; struct Cyc_Dict_Dict*
_temp1110; _LL1106: if( _temp1104 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1107;} else{ goto _LL1108;} _LL1108: if(( unsigned int) _temp1104 >  1u?*((
int*) _temp1104) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1111: _temp1110=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1104)->f1; goto _LL1109;} else{
goto _LL1105;} _LL1107: return({ struct _tuple0 _temp1112; _temp1112.f1=
_temp1102; _temp1112.f2= _temp1102; _temp1112;}); _LL1109: { void* _temp1113=
_temp1100; void* _temp1133; void* _temp1135; void* _temp1137; _LL1115: if(
_temp1113 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1116;} else{ goto _LL1117;}
_LL1117: if( _temp1113 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto _LL1118;}
else{ goto _LL1119;} _LL1119: if( _temp1113 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL1120;} else{ goto _LL1121;} _LL1121: if(( unsigned int) _temp1113 >  3u?*((
int*) _temp1113) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL1122;} else{ goto
_LL1123;} _LL1123: if(( unsigned int) _temp1113 >  3u?*(( int*) _temp1113) == 
Cyc_CfFlowInfo_UnknownR: 0){ _LL1134: _temp1133=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1113)->f1; if( _temp1133 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL1124;}
else{ goto _LL1125;}} else{ goto _LL1125;} _LL1125: if(( unsigned int) _temp1113
>  3u?*(( int*) _temp1113) ==  Cyc_CfFlowInfo_Esc: 0){ _LL1136: _temp1135=( void*)((
struct Cyc_CfFlowInfo_Esc_struct*) _temp1113)->f1; if( _temp1135 == ( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL1126;} else{ goto _LL1127;}} else{ goto _LL1127;} _LL1127: if((
unsigned int) _temp1113 >  3u?*(( int*) _temp1113) ==  Cyc_CfFlowInfo_UnknownR:
0){ _LL1138: _temp1137=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1113)->f1; goto _LL1128;} else{ goto _LL1129;} _LL1129: if(( unsigned int)
_temp1113 >  3u?*(( int*) _temp1113) ==  Cyc_CfFlowInfo_Esc: 0){ goto _LL1130;}
else{ goto _LL1131;} _LL1131: if(( unsigned int) _temp1113 >  3u?*(( int*)
_temp1113) ==  Cyc_CfFlowInfo_Aggregate: 0){ goto _LL1132;} else{ goto _LL1114;}
_LL1116: return({ struct _tuple0 _temp1139; _temp1139.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1139.f2= _temp1102; _temp1139;}); _LL1118: goto _LL1120; _LL1120: goto
_LL1122; _LL1122: return({ struct _tuple0 _temp1140; _temp1140.f1= _temp1102;
_temp1140.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1140;}); _LL1124: goto
_LL1126; _LL1126:({ void* _temp1141[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1141, sizeof( void*), 0u));});
return({ struct _tuple0 _temp1142; _temp1142.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1142.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1142;}); _LL1128: return Cyc_NewControlFlow_splitzero(
env, inflow, _temp1102, e, _temp1137); _LL1130: return({ struct _tuple0
_temp1143; _temp1143.f1= _temp1102; _temp1143.f2= _temp1102; _temp1143;});
_LL1132:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1144=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1144[ 0]=({ struct Cyc_Core_Impossible_struct _temp1145; _temp1145.tag= Cyc_Core_Impossible;
_temp1145.f1= _tag_arr("anal_test", sizeof( unsigned char), 10u); _temp1145;});
_temp1144;})); _LL1114:;} _LL1105:;}}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( struct Cyc_NewControlFlow_AnalEnv* env, void*
inflow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1146= Cyc_NewControlFlow_get_stmt_annot( s);( void*)( _temp1146->flow=(
void*) Cyc_CfFlowInfo_join_flow( env->all_changed, inflow,( void*) _temp1146->flow));
++ _temp1146->visited; _temp1146->visited ==  env->iteration_num? 0:(((( int(*)(
struct _tagged_arr assertion, struct _tagged_arr file, unsigned int line)) Cyc_Std___assert_fail)(
_tag_arr("annot->visited == env->iteration_num", sizeof( unsigned char), 37u),
_tag_arr("new_control_flow.cyc", sizeof( unsigned char), 21u), 760), 0)); return
_temp1146;} static void* Cyc_NewControlFlow_anal_scs( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, struct Cyc_List_List* scs){ for( 0; scs !=  0; scs=(( struct
Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause
_temp1149; struct Cyc_Absyn_Stmt* _temp1150; struct Cyc_Absyn_Exp* _temp1152;
struct Cyc_Core_Opt* _temp1154; struct Cyc_Absyn_Switch_clause* _temp1147=(
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd;
_temp1149=* _temp1147; _LL1155: _temp1154= _temp1149.pat_vars; goto _LL1153;
_LL1153: _temp1152= _temp1149.where_clause; goto _LL1151; _LL1151: _temp1150=
_temp1149.body; goto _LL1148; _LL1148: { void* clause_inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1154))->v,
Cyc_CfFlowInfo_unknown_all); void* clause_outflow; if( _temp1152 !=  0){ void*
_temp1158; void* _temp1160; struct _tuple0 _temp1156= Cyc_NewControlFlow_anal_test(
env, clause_inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1152)); _LL1161:
_temp1160= _temp1156.f1; goto _LL1159; _LL1159: _temp1158= _temp1156.f2; goto
_LL1157; _LL1157: inflow= _temp1158; clause_outflow= Cyc_NewControlFlow_anal_stmt(
env, _temp1160, _temp1150);} else{ clause_outflow= Cyc_NewControlFlow_anal_stmt(
env, clause_inflow, _temp1150);}{ void* _temp1162= clause_outflow; _LL1164: if(
_temp1162 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1165;} else{ goto
_LL1166;} _LL1166: goto _LL1167; _LL1165: goto _LL1163; _LL1167: if((( struct
Cyc_List_List*) _check_null( scs))->tl ==  0){ return clause_outflow;} else{ if((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( scs))->tl))->hd)->pat_vars))->v
!=  0){({ void* _temp1168[ 0u]={}; Cyc_Tcutil_terr( _temp1150->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1168, sizeof( void*), 0u));});}
else{({ void* _temp1169[ 0u]={}; Cyc_Tcutil_warn( _temp1150->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1169, sizeof( void*), 0u));});} Cyc_NewControlFlow_update_flow(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( scs))->tl))->hd)->body, clause_outflow);}
goto _LL1163; _LL1163:;}}} return( void*) Cyc_CfFlowInfo_BottomFL;} static void*
Cyc_NewControlFlow_anal_stmt( struct Cyc_NewControlFlow_AnalEnv* env, void*
inflow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1170= Cyc_NewControlFlow_pre_stmt_check( env, inflow, s); inflow=( void*)
_temp1170->flow;{ void* _temp1171=( void*) s->r; struct Cyc_Absyn_Exp* _temp1213;
struct Cyc_Absyn_Exp* _temp1215; struct Cyc_Absyn_Exp* _temp1217; struct Cyc_Absyn_Stmt*
_temp1219; struct Cyc_Absyn_Stmt* _temp1221; struct Cyc_Absyn_Stmt* _temp1223;
struct Cyc_Absyn_Stmt* _temp1225; struct Cyc_Absyn_Exp* _temp1227; struct Cyc_Absyn_Stmt*
_temp1229; struct _tuple3 _temp1231; struct Cyc_Absyn_Stmt* _temp1233; struct
Cyc_Absyn_Exp* _temp1235; struct _tuple3 _temp1237; struct Cyc_Absyn_Stmt*
_temp1239; struct Cyc_Absyn_Exp* _temp1241; struct Cyc_Absyn_Stmt* _temp1243;
struct Cyc_Absyn_Stmt* _temp1245; struct _tuple3 _temp1247; struct Cyc_Absyn_Stmt*
_temp1249; struct Cyc_Absyn_Exp* _temp1251; struct _tuple3 _temp1253; struct Cyc_Absyn_Stmt*
_temp1255; struct Cyc_Absyn_Exp* _temp1257; struct Cyc_Absyn_Exp* _temp1259;
struct Cyc_Absyn_Stmt* _temp1261; struct Cyc_Absyn_Switch_clause** _temp1263;
struct Cyc_Absyn_Switch_clause* _temp1265; struct Cyc_List_List* _temp1266;
struct Cyc_Absyn_Stmt* _temp1268; struct Cyc_Absyn_Stmt* _temp1270; struct Cyc_Absyn_Stmt*
_temp1272; struct Cyc_List_List* _temp1274; struct Cyc_Absyn_Exp* _temp1276;
struct Cyc_List_List* _temp1278; struct Cyc_Absyn_Stmt* _temp1280; struct Cyc_Absyn_Stmt*
_temp1282; struct Cyc_Absyn_Decl* _temp1284; struct Cyc_Absyn_Stmt* _temp1286;
struct Cyc_Absyn_Stmt* _temp1288; struct Cyc_Absyn_Vardecl* _temp1290; struct
Cyc_Absyn_Tvar* _temp1292; _LL1173: if( _temp1171 == ( void*) Cyc_Absyn_Skip_s){
goto _LL1174;} else{ goto _LL1175;} _LL1175: if(( unsigned int) _temp1171 >  1u?*((
int*) _temp1171) ==  Cyc_Absyn_Return_s: 0){ _LL1214: _temp1213=(( struct Cyc_Absyn_Return_s_struct*)
_temp1171)->f1; if( _temp1213 ==  0){ goto _LL1176;} else{ goto _LL1177;}} else{
goto _LL1177;} _LL1177: if(( unsigned int) _temp1171 >  1u?*(( int*) _temp1171)
==  Cyc_Absyn_Return_s: 0){ _LL1216: _temp1215=(( struct Cyc_Absyn_Return_s_struct*)
_temp1171)->f1; goto _LL1178;} else{ goto _LL1179;} _LL1179: if(( unsigned int)
_temp1171 >  1u?*(( int*) _temp1171) ==  Cyc_Absyn_Exp_s: 0){ _LL1218: _temp1217=((
struct Cyc_Absyn_Exp_s_struct*) _temp1171)->f1; goto _LL1180;} else{ goto
_LL1181;} _LL1181: if(( unsigned int) _temp1171 >  1u?*(( int*) _temp1171) == 
Cyc_Absyn_Seq_s: 0){ _LL1222: _temp1221=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1171)->f1; goto _LL1220; _LL1220: _temp1219=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1171)->f2; goto _LL1182;} else{ goto _LL1183;} _LL1183: if(( unsigned int)
_temp1171 >  1u?*(( int*) _temp1171) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1228:
_temp1227=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1171)->f1; goto _LL1226;
_LL1226: _temp1225=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1171)->f2;
goto _LL1224; _LL1224: _temp1223=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1171)->f3; goto _LL1184;} else{ goto _LL1185;} _LL1185: if(( unsigned int)
_temp1171 >  1u?*(( int*) _temp1171) ==  Cyc_Absyn_While_s: 0){ _LL1232:
_temp1231=(( struct Cyc_Absyn_While_s_struct*) _temp1171)->f1; _LL1236:
_temp1235= _temp1231.f1; goto _LL1234; _LL1234: _temp1233= _temp1231.f2; goto
_LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_While_s_struct*) _temp1171)->f2;
goto _LL1186;} else{ goto _LL1187;} _LL1187: if(( unsigned int) _temp1171 >  1u?*((
int*) _temp1171) ==  Cyc_Absyn_Do_s: 0){ _LL1244: _temp1243=(( struct Cyc_Absyn_Do_s_struct*)
_temp1171)->f1; goto _LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_Do_s_struct*)
_temp1171)->f2; _LL1242: _temp1241= _temp1237.f1; goto _LL1240; _LL1240:
_temp1239= _temp1237.f2; goto _LL1188;} else{ goto _LL1189;} _LL1189: if((
unsigned int) _temp1171 >  1u?*(( int*) _temp1171) ==  Cyc_Absyn_For_s: 0){
_LL1260: _temp1259=(( struct Cyc_Absyn_For_s_struct*) _temp1171)->f1; goto
_LL1254; _LL1254: _temp1253=(( struct Cyc_Absyn_For_s_struct*) _temp1171)->f2;
_LL1258: _temp1257= _temp1253.f1; goto _LL1256; _LL1256: _temp1255= _temp1253.f2;
goto _LL1248; _LL1248: _temp1247=(( struct Cyc_Absyn_For_s_struct*) _temp1171)->f3;
_LL1252: _temp1251= _temp1247.f1; goto _LL1250; _LL1250: _temp1249= _temp1247.f2;
goto _LL1246; _LL1246: _temp1245=(( struct Cyc_Absyn_For_s_struct*) _temp1171)->f4;
goto _LL1190;} else{ goto _LL1191;} _LL1191: if(( unsigned int) _temp1171 >  1u?*((
int*) _temp1171) ==  Cyc_Absyn_Break_s: 0){ _LL1262: _temp1261=(( struct Cyc_Absyn_Break_s_struct*)
_temp1171)->f1; if( _temp1261 ==  0){ goto _LL1192;} else{ goto _LL1193;}} else{
goto _LL1193;} _LL1193: if(( unsigned int) _temp1171 >  1u?*(( int*) _temp1171)
==  Cyc_Absyn_Fallthru_s: 0){ _LL1267: _temp1266=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1171)->f1; goto _LL1264; _LL1264: _temp1263=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1171)->f2; if( _temp1263 ==  0){ goto _LL1195;} else{ _temp1265=* _temp1263;
goto _LL1194;}} else{ goto _LL1195;} _LL1195: if(( unsigned int) _temp1171 >  1u?*((
int*) _temp1171) ==  Cyc_Absyn_Break_s: 0){ _LL1269: _temp1268=(( struct Cyc_Absyn_Break_s_struct*)
_temp1171)->f1; goto _LL1196;} else{ goto _LL1197;} _LL1197: if(( unsigned int)
_temp1171 >  1u?*(( int*) _temp1171) ==  Cyc_Absyn_Continue_s: 0){ _LL1271:
_temp1270=(( struct Cyc_Absyn_Continue_s_struct*) _temp1171)->f1; goto _LL1198;}
else{ goto _LL1199;} _LL1199: if(( unsigned int) _temp1171 >  1u?*(( int*)
_temp1171) ==  Cyc_Absyn_Goto_s: 0){ _LL1273: _temp1272=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1171)->f2; goto _LL1200;} else{ goto _LL1201;} _LL1201: if(( unsigned int)
_temp1171 >  1u?*(( int*) _temp1171) ==  Cyc_Absyn_Switch_s: 0){ _LL1277:
_temp1276=(( struct Cyc_Absyn_Switch_s_struct*) _temp1171)->f1; goto _LL1275;
_LL1275: _temp1274=(( struct Cyc_Absyn_Switch_s_struct*) _temp1171)->f2; goto
_LL1202;} else{ goto _LL1203;} _LL1203: if(( unsigned int) _temp1171 >  1u?*((
int*) _temp1171) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1281: _temp1280=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1171)->f1; goto _LL1279; _LL1279: _temp1278=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1171)->f2; goto _LL1204;} else{ goto _LL1205;} _LL1205: if(( unsigned int)
_temp1171 >  1u?*(( int*) _temp1171) ==  Cyc_Absyn_Decl_s: 0){ _LL1285:
_temp1284=(( struct Cyc_Absyn_Decl_s_struct*) _temp1171)->f1; goto _LL1283;
_LL1283: _temp1282=(( struct Cyc_Absyn_Decl_s_struct*) _temp1171)->f2; goto
_LL1206;} else{ goto _LL1207;} _LL1207: if(( unsigned int) _temp1171 >  1u?*((
int*) _temp1171) ==  Cyc_Absyn_Label_s: 0){ _LL1287: _temp1286=(( struct Cyc_Absyn_Label_s_struct*)
_temp1171)->f2; goto _LL1208;} else{ goto _LL1209;} _LL1209: if(( unsigned int)
_temp1171 >  1u?*(( int*) _temp1171) ==  Cyc_Absyn_Region_s: 0){ _LL1293:
_temp1292=(( struct Cyc_Absyn_Region_s_struct*) _temp1171)->f1; goto _LL1291;
_LL1291: _temp1290=(( struct Cyc_Absyn_Region_s_struct*) _temp1171)->f2; goto
_LL1289; _LL1289: _temp1288=(( struct Cyc_Absyn_Region_s_struct*) _temp1171)->f3;
goto _LL1210;} else{ goto _LL1211;} _LL1211: goto _LL1212; _LL1174: return
inflow; _LL1176: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1178: { void*
_temp1296; void* _temp1298; struct _tuple0 _temp1294= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1215)); _LL1299:
_temp1298= _temp1294.f1; goto _LL1297; _LL1297: _temp1296= _temp1294.f2; goto
_LL1295; _LL1295: Cyc_NewControlFlow_use_Rval( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp1215))->loc, _temp1298, _temp1296); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1180: return( Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp1217)).f1;
_LL1182: return Cyc_NewControlFlow_anal_stmt( env, Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1221), _temp1219); _LL1184: { void* _temp1302; void* _temp1304;
struct _tuple0 _temp1300= Cyc_NewControlFlow_anal_test( env, inflow, _temp1227);
_LL1305: _temp1304= _temp1300.f1; goto _LL1303; _LL1303: _temp1302= _temp1300.f2;
goto _LL1301; _LL1301: return Cyc_CfFlowInfo_join_flow( env->all_changed, Cyc_NewControlFlow_anal_stmt(
env, _temp1304, _temp1225), Cyc_NewControlFlow_anal_stmt( env, _temp1302,
_temp1223));} _LL1186: { void* _temp1306=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, inflow, _temp1233))->flow; void* _temp1309; void* _temp1311; struct _tuple0
_temp1307= Cyc_NewControlFlow_anal_test( env, _temp1306, _temp1235); _LL1312:
_temp1311= _temp1307.f1; goto _LL1310; _LL1310: _temp1309= _temp1307.f2; goto
_LL1308; _LL1308: { void* _temp1313= Cyc_NewControlFlow_anal_stmt( env,
_temp1311, _temp1229); Cyc_NewControlFlow_update_flow( env, _temp1233, _temp1313);
return _temp1309;}} _LL1188: { void* _temp1314= Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1243); void* _temp1315=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1314, _temp1239))->flow; void* _temp1318; void* _temp1320; struct
_tuple0 _temp1316= Cyc_NewControlFlow_anal_test( env, _temp1315, _temp1241);
_LL1321: _temp1320= _temp1316.f1; goto _LL1319; _LL1319: _temp1318= _temp1316.f2;
goto _LL1317; _LL1317: Cyc_NewControlFlow_update_flow( env, _temp1243, _temp1320);
return _temp1318;} _LL1190: { void* _temp1322=( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1259)).f1; void* _temp1323=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1322, _temp1255))->flow; void* _temp1326; void* _temp1328; struct
_tuple0 _temp1324= Cyc_NewControlFlow_anal_test( env, _temp1323, _temp1257);
_LL1329: _temp1328= _temp1324.f1; goto _LL1327; _LL1327: _temp1326= _temp1324.f2;
goto _LL1325; _LL1325: { void* _temp1330= Cyc_NewControlFlow_anal_stmt( env,
_temp1328, _temp1245); void* _temp1331=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1330, _temp1249))->flow; void* _temp1332=( Cyc_NewControlFlow_anal_Rexp(
env, _temp1331, _temp1251)).f1; Cyc_NewControlFlow_update_flow( env, _temp1255,
_temp1332); return _temp1326;}} _LL1192: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1194: { struct Cyc_List_List* _temp1335; void* _temp1337; struct _tuple4
_temp1333= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1266);
_LL1338: _temp1337= _temp1333.f1; goto _LL1336; _LL1336: _temp1335= _temp1333.f2;
goto _LL1334; _LL1334: for( 0; _temp1335 !=  0;( _temp1335=(( struct Cyc_List_List*)
_check_null( _temp1335))->tl, _temp1266=(( struct Cyc_List_List*) _check_null(
_temp1266))->tl)){ _temp1337= Cyc_NewControlFlow_use_Rval( env,(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1266))->hd)->loc, _temp1337,( void*)((
struct Cyc_List_List*) _check_null( _temp1335))->hd);} _temp1337= Cyc_NewControlFlow_add_vars(
_temp1337,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1265->pat_vars))->v, Cyc_CfFlowInfo_unknown_all); Cyc_NewControlFlow_update_flow(
env,( struct Cyc_Absyn_Stmt*) _temp1265->body, _temp1337); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1196: _temp1270= _temp1268; goto _LL1198; _LL1198: _temp1272= _temp1270; goto
_LL1200; _LL1200: if( env->iteration_num ==  1){ struct Cyc_Absyn_Stmt*
_temp1339= _temp1170->encloser; struct Cyc_Absyn_Stmt* _temp1340=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp1272)))->encloser; while( _temp1340 != 
_temp1339) { struct Cyc_Absyn_Stmt* _temp1341=( Cyc_NewControlFlow_get_stmt_annot(
_temp1339))->encloser; if( _temp1341 ==  _temp1339){({ void* _temp1342[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("goto enters local scope or exception handler",
sizeof( unsigned char), 45u), _tag_arr( _temp1342, sizeof( void*), 0u));});
break;} _temp1339= _temp1341;}} Cyc_NewControlFlow_update_flow( env,( struct Cyc_Absyn_Stmt*)
_check_null( _temp1272), inflow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1202: { void* _temp1345; void* _temp1347; struct _tuple0 _temp1343= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1276); _LL1348: _temp1347= _temp1343.f1; goto _LL1346; _LL1346:
_temp1345= _temp1343.f2; goto _LL1344; _LL1344: _temp1347= Cyc_NewControlFlow_use_Rval(
env, _temp1276->loc, _temp1347, _temp1345); return Cyc_NewControlFlow_anal_scs(
env, _temp1347, _temp1274);} _LL1204: { int old_in_try= env->in_try; void*
old_tryflow=( void*) env->tryflow; env->in_try= 1;( void*)( env->tryflow=( void*)
inflow);{ void* s1_outflow= Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1280);
void* scs_inflow=( void*) env->tryflow; env->in_try= old_in_try;( void*)( env->tryflow=(
void*) old_tryflow); Cyc_NewControlFlow_update_tryflow( env, scs_inflow);{ void*
scs_outflow= Cyc_NewControlFlow_anal_scs( env, scs_inflow, _temp1278);{ void*
_temp1349= scs_outflow; _LL1351: if( _temp1349 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1352;} else{ goto _LL1353;} _LL1353: goto _LL1354; _LL1352: goto _LL1350;
_LL1354:({ void* _temp1355[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("last catch clause may implicitly fallthru",
sizeof( unsigned char), 42u), _tag_arr( _temp1355, sizeof( void*), 0u));});
_LL1350:;} return s1_outflow;}}} _LL1206: return Cyc_NewControlFlow_anal_stmt(
env, Cyc_NewControlFlow_anal_decl( env, inflow, _temp1284), _temp1282); _LL1208:
return Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1286); _LL1210: { struct
Cyc_List_List _temp1356=({ struct Cyc_List_List _temp1357; _temp1357.hd=( void*)
_temp1290; _temp1357.tl= 0; _temp1357;}); inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)& _temp1356, Cyc_CfFlowInfo_unknown_all); return
Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1288);} _LL1212:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp1358=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1358[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1359; _temp1359.tag= Cyc_Core_Impossible;
_temp1359.f1= _tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 56u); _temp1359;}); _temp1358;})); _LL1172:;}} static
void* Cyc_NewControlFlow_anal_decl( struct Cyc_NewControlFlow_AnalEnv* env, void*
inflow, struct Cyc_Absyn_Decl* decl){ void* _temp1360=( void*) decl->r; struct
Cyc_Absyn_Vardecl* _temp1370; struct Cyc_Absyn_Exp* _temp1372; struct Cyc_Core_Opt*
_temp1374; struct Cyc_Core_Opt _temp1376; struct Cyc_List_List* _temp1377;
struct Cyc_List_List* _temp1379; _LL1362: if(*(( int*) _temp1360) ==  Cyc_Absyn_Var_d){
_LL1371: _temp1370=(( struct Cyc_Absyn_Var_d_struct*) _temp1360)->f1; goto
_LL1363;} else{ goto _LL1364;} _LL1364: if(*(( int*) _temp1360) ==  Cyc_Absyn_Let_d){
_LL1375: _temp1374=(( struct Cyc_Absyn_Let_d_struct*) _temp1360)->f2; if(
_temp1374 ==  0){ goto _LL1366;} else{ _temp1376=* _temp1374; _LL1378: _temp1377=(
struct Cyc_List_List*) _temp1376.v; goto _LL1373;} _LL1373: _temp1372=(( struct
Cyc_Absyn_Let_d_struct*) _temp1360)->f4; goto _LL1365;} else{ goto _LL1366;}
_LL1366: if(*(( int*) _temp1360) ==  Cyc_Absyn_Letv_d){ _LL1380: _temp1379=((
struct Cyc_Absyn_Letv_d_struct*) _temp1360)->f1; goto _LL1367;} else{ goto
_LL1368;} _LL1368: goto _LL1369; _LL1363: { struct Cyc_List_List _temp1381=({
struct Cyc_List_List _temp1403; _temp1403.hd=( void*) _temp1370; _temp1403.tl= 0;
_temp1403;}); inflow= Cyc_NewControlFlow_add_vars( inflow,( struct Cyc_List_List*)&
_temp1381, Cyc_CfFlowInfo_unknown_none);{ struct Cyc_Absyn_Exp* _temp1382=
_temp1370->initializer; if( _temp1382 ==  0){ return inflow;}{ void* _temp1385;
void* _temp1387; struct _tuple0 _temp1383= Cyc_NewControlFlow_anal_Rexp( env,
inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1382)); _LL1388: _temp1387=
_temp1383.f1; goto _LL1386; _LL1386: _temp1385= _temp1383.f2; goto _LL1384;
_LL1384: { void* _temp1389= _temp1387; struct Cyc_Dict_Dict* _temp1395; _LL1391:
if( _temp1389 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1392;} else{ goto
_LL1393;} _LL1393: if(( unsigned int) _temp1389 >  1u?*(( int*) _temp1389) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL1396: _temp1395=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1389)->f1; goto _LL1394;} else{ goto _LL1390;} _LL1392: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1394: _temp1395= Cyc_CfFlowInfo_assign_place( decl->loc, _temp1395, env->all_changed,({
struct Cyc_CfFlowInfo_Place* _temp1397=( struct Cyc_CfFlowInfo_Place*) _cycalloc(
sizeof( struct Cyc_CfFlowInfo_Place)); _temp1397->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1398=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1398[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1399; _temp1399.tag= Cyc_CfFlowInfo_VarRoot;
_temp1399.f1= _temp1370; _temp1399;}); _temp1398;})); _temp1397->fields= 0;
_temp1397;}), _temp1385);{ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp1400=({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp1401=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp1401[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp1402; _temp1402.tag= Cyc_CfFlowInfo_ReachableFL;
_temp1402.f1= _temp1395; _temp1402;}); _temp1401;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp1400); return( void*) _temp1400;} _LL1390:;}}}} _LL1365: {
void* _temp1406; void* _temp1408; struct _tuple0 _temp1404= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1372); _LL1409: _temp1408= _temp1404.f1; goto _LL1407; _LL1407:
_temp1406= _temp1404.f2; goto _LL1405; _LL1405: _temp1408= Cyc_NewControlFlow_use_Rval(
env, _temp1372->loc, _temp1408, _temp1406); return Cyc_NewControlFlow_add_vars(
_temp1408, _temp1377, Cyc_CfFlowInfo_unknown_all);} _LL1367: return Cyc_NewControlFlow_add_vars(
inflow, _temp1379, Cyc_CfFlowInfo_unknown_none); _LL1369:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp1410=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1410[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1411; _temp1411.tag= Cyc_Core_Impossible;
_temp1411.f1= _tag_arr("anal_decl: unexpected decl variant", sizeof(
unsigned char), 35u); _temp1411;}); _temp1410;})); _LL1361:;} static void Cyc_NewControlFlow_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp1412=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1436; struct _tuple3
_temp1438; struct Cyc_Absyn_Stmt* _temp1440; struct _tuple3 _temp1442; struct
Cyc_Absyn_Stmt* _temp1444; struct Cyc_Absyn_Stmt* _temp1446; struct Cyc_Absyn_Stmt*
_temp1448; struct Cyc_Absyn_Stmt* _temp1450; struct _tuple3 _temp1452; struct
Cyc_Absyn_Stmt* _temp1454; struct _tuple3 _temp1456; struct Cyc_Absyn_Stmt*
_temp1458; struct Cyc_Absyn_Stmt* _temp1460; struct Cyc_Absyn_Stmt* _temp1462;
struct Cyc_Absyn_Stmt* _temp1464; struct Cyc_Absyn_Stmt* _temp1466; struct Cyc_Absyn_Stmt*
_temp1468; struct Cyc_Absyn_Stmt* _temp1470; struct Cyc_List_List* _temp1472;
struct Cyc_Absyn_Stmt* _temp1474; struct Cyc_List_List* _temp1476; _LL1414: if((
unsigned int) _temp1412 >  1u?*(( int*) _temp1412) ==  Cyc_Absyn_For_s: 0){
_LL1443: _temp1442=(( struct Cyc_Absyn_For_s_struct*) _temp1412)->f2; _LL1445:
_temp1444= _temp1442.f2; goto _LL1439; _LL1439: _temp1438=(( struct Cyc_Absyn_For_s_struct*)
_temp1412)->f3; _LL1441: _temp1440= _temp1438.f2; goto _LL1437; _LL1437:
_temp1436=(( struct Cyc_Absyn_For_s_struct*) _temp1412)->f4; goto _LL1415;}
else{ goto _LL1416;} _LL1416: if(( unsigned int) _temp1412 >  1u?*(( int*)
_temp1412) ==  Cyc_Absyn_Seq_s: 0){ _LL1449: _temp1448=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1412)->f1; goto _LL1447; _LL1447: _temp1446=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1412)->f2; goto _LL1417;} else{ goto _LL1418;} _LL1418: if(( unsigned int)
_temp1412 >  1u?*(( int*) _temp1412) ==  Cyc_Absyn_While_s: 0){ _LL1453:
_temp1452=(( struct Cyc_Absyn_While_s_struct*) _temp1412)->f1; _LL1455:
_temp1454= _temp1452.f2; goto _LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_While_s_struct*)
_temp1412)->f2; goto _LL1419;} else{ goto _LL1420;} _LL1420: if(( unsigned int)
_temp1412 >  1u?*(( int*) _temp1412) ==  Cyc_Absyn_Do_s: 0){ _LL1461: _temp1460=((
struct Cyc_Absyn_Do_s_struct*) _temp1412)->f1; goto _LL1457; _LL1457: _temp1456=((
struct Cyc_Absyn_Do_s_struct*) _temp1412)->f2; _LL1459: _temp1458= _temp1456.f2;
goto _LL1421;} else{ goto _LL1422;} _LL1422: if(( unsigned int) _temp1412 >  1u?*((
int*) _temp1412) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1465: _temp1464=(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp1412)->f2; goto _LL1463; _LL1463: _temp1462=((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1412)->f3; goto _LL1423;} else{ goto
_LL1424;} _LL1424: if(( unsigned int) _temp1412 >  1u?*(( int*) _temp1412) == 
Cyc_Absyn_Region_s: 0){ _LL1467: _temp1466=(( struct Cyc_Absyn_Region_s_struct*)
_temp1412)->f3; goto _LL1425;} else{ goto _LL1426;} _LL1426: if(( unsigned int)
_temp1412 >  1u?*(( int*) _temp1412) ==  Cyc_Absyn_Label_s: 0){ _LL1469:
_temp1468=(( struct Cyc_Absyn_Label_s_struct*) _temp1412)->f2; goto _LL1427;}
else{ goto _LL1428;} _LL1428: if(( unsigned int) _temp1412 >  1u?*(( int*)
_temp1412) ==  Cyc_Absyn_Decl_s: 0){ _LL1471: _temp1470=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1412)->f2; goto _LL1429;} else{ goto _LL1430;} _LL1430: if(( unsigned int)
_temp1412 >  1u?*(( int*) _temp1412) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1475:
_temp1474=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1412)->f1; goto _LL1473;
_LL1473: _temp1472=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1412)->f2; goto
_LL1431;} else{ goto _LL1432;} _LL1432: if(( unsigned int) _temp1412 >  1u?*((
int*) _temp1412) ==  Cyc_Absyn_Switch_s: 0){ _LL1477: _temp1476=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1412)->f2; goto _LL1433;} else{ goto _LL1434;} _LL1434: goto _LL1435;
_LL1415: Cyc_NewControlFlow_postpass( _temp1440); _temp1448= _temp1436;
_temp1446= _temp1444; goto _LL1417; _LL1417: _temp1454= _temp1448; _temp1450=
_temp1446; goto _LL1419; _LL1419: _temp1460= _temp1450; _temp1458= _temp1454;
goto _LL1421; _LL1421: _temp1464= _temp1460; _temp1462= _temp1458; goto _LL1423;
_LL1423: Cyc_NewControlFlow_postpass( _temp1464); _temp1466= _temp1462; goto
_LL1425; _LL1425: _temp1468= _temp1466; goto _LL1427; _LL1427: _temp1470=
_temp1468; goto _LL1429; _LL1429: Cyc_NewControlFlow_postpass( _temp1470);
return; _LL1431: Cyc_NewControlFlow_postpass( _temp1474); _temp1476= _temp1472;
goto _LL1433; _LL1433: for( 0; _temp1476 !=  0; _temp1476=(( struct Cyc_List_List*)
_check_null( _temp1476))->tl){ Cyc_NewControlFlow_postpass((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1476))->hd)->body);} return; _LL1435:
return; _LL1413:;}} static void Cyc_NewControlFlow_check_fun( struct Cyc_Absyn_Fndecl*
fd){ void* inflow= Cyc_NewControlFlow_add_vars(( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp1486=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp1486[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp1487; _temp1487.tag= Cyc_CfFlowInfo_ReachableFL; _temp1487.f1= Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp); _temp1487;}); _temp1486;}),( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v, Cyc_CfFlowInfo_unknown_all);
struct Cyc_NewControlFlow_AnalEnv* env=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1485=( struct Cyc_NewControlFlow_AnalEnv*) _cycalloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1485->iterate_again= 1; _temp1485->iteration_num= 0; _temp1485->in_try= 0;
_temp1485->tryflow=( void*) inflow; _temp1485->all_changed= 0; _temp1485;});
void* outflow= inflow; while( env->iterate_again) { ++ env->iteration_num; env->iterate_again=
0; outflow= Cyc_NewControlFlow_anal_stmt( env, inflow, fd->body);}{ void*
_temp1478= outflow; _LL1480: if( _temp1478 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1481;} else{ goto _LL1482;} _LL1482: goto _LL1483; _LL1481: goto _LL1479;
_LL1483: if(( void*) fd->ret_type != ( void*) Cyc_Absyn_VoidType){({ void*
_temp1484[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc, _tag_arr("function may not return a value",
sizeof( unsigned char), 32u), _tag_arr( _temp1484, sizeof( void*), 0u));});}
_LL1479:;} Cyc_NewControlFlow_postpass( fd->body);} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds !=  0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp1488=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd)->r; struct Cyc_Absyn_Fndecl*
_temp1500; struct Cyc_List_List* _temp1502; struct Cyc_List_List* _temp1504;
struct Cyc_List_List* _temp1506; _LL1490: if(*(( int*) _temp1488) ==  Cyc_Absyn_Fn_d){
_LL1501: _temp1500=(( struct Cyc_Absyn_Fn_d_struct*) _temp1488)->f1; goto
_LL1491;} else{ goto _LL1492;} _LL1492: if(*(( int*) _temp1488) ==  Cyc_Absyn_ExternC_d){
_LL1503: _temp1502=(( struct Cyc_Absyn_ExternC_d_struct*) _temp1488)->f1; goto
_LL1493;} else{ goto _LL1494;} _LL1494: if(*(( int*) _temp1488) ==  Cyc_Absyn_Using_d){
_LL1505: _temp1504=(( struct Cyc_Absyn_Using_d_struct*) _temp1488)->f2; goto
_LL1495;} else{ goto _LL1496;} _LL1496: if(*(( int*) _temp1488) ==  Cyc_Absyn_Namespace_d){
_LL1507: _temp1506=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1488)->f2; goto
_LL1497;} else{ goto _LL1498;} _LL1498: goto _LL1499; _LL1491: Cyc_NewControlFlow_check_fun(
_temp1500); goto _LL1489; _LL1493: _temp1504= _temp1502; goto _LL1495; _LL1495:
_temp1506= _temp1504; goto _LL1497; _LL1497: Cyc_NewControlFlow_cf_check(
_temp1506); goto _LL1489; _LL1499: goto _LL1489; _LL1489:;}}
