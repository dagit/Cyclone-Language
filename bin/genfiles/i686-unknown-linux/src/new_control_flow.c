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
struct _tagged_arr*, struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static
const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static
const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; }
; extern unsigned char Cyc_Position_Nocontext[ 14u]; static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; struct _tuple1{ void* f1; struct
_tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static const int Cyc_Absyn_Static=
0; static const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2;
static const int Cyc_Absyn_Extern= 3; static const int Cyc_Absyn_ExternC= 4;
struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1; }
; static const int Cyc_Absyn_B1= 0; static const int Cyc_Absyn_B2= 1; static
const int Cyc_Absyn_B4= 2; static const int Cyc_Absyn_B8= 3; static const int
Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind= 1; static const int
Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3; static const int
Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0; static const int Cyc_Absyn_Unsigned=
1; struct Cyc_Absyn_Conref{ void* v; } ; static const int Cyc_Absyn_Eq_constr= 0;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr=
1; struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1;
} ; static const int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_arr* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; int inject; } ; struct Cyc_Absyn_FnInfo{
struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct
Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs;
struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
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
f3; } ; static const int Cyc_Absyn_EnumType= 11; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_AnonStructType= 12; struct Cyc_Absyn_AnonStructType_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType= 13;
struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_AnonEnumType= 14; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
15; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 16; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff= 17;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
18; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 19; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
struct Cyc_Absyn_Exp*); static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp14= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( _temp14->flow=( void*) Cyc_CfFlowInfo_join_flow( env->all_changed,
inflow,( void*) _temp14->flow)); _temp14->visited= env->iteration_num; return
_temp14;} static void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv*
env, void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
env->all_changed, new_flow,( void*) env->tryflow));}} static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp15= Cyc_NewControlFlow_get_stmt_annot(
s); void* _temp16= Cyc_CfFlowInfo_join_flow( env->all_changed, flow,( void*)
_temp15->flow); if( ! Cyc_CfFlowInfo_flow_lessthan_approx( _temp16,( void*)
_temp15->flow)){( void*)( _temp15->flow=( void*) _temp16); if( _temp15->visited
==  env->iteration_num){ env->iterate_again= 1;}}} static void* Cyc_NewControlFlow_add_vars(
void* inflow, struct Cyc_List_List* vds, void* leafval){ void* _temp17= inflow;
struct Cyc_Dict_Dict* _temp23; _LL19: if( _temp17 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp17 >  1u?*(( int*)
_temp17) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL24: _temp23=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp17)->f1; goto _LL22;} else{ goto _LL18;} _LL20: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL22: for( 0; vds !=  0; vds= vds->tl){ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp25=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp26=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp26[ 0]=({ struct
Cyc_CfFlowInfo_VarRoot_struct _temp27; _temp27.tag= Cyc_CfFlowInfo_VarRoot;
_temp27.f1=( struct Cyc_Absyn_Vardecl*) vds->hd; _temp27;}); _temp26;}); _temp23=
Cyc_Dict_insert( _temp23,( void*) _temp25, Cyc_CfFlowInfo_typ_to_absrval(( void*)((
struct Cyc_Absyn_Vardecl*) vds->hd)->type, leafval));} return( void*)({ struct
Cyc_CfFlowInfo_ReachableFL_struct* _temp28=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp28[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp29; _temp29.tag= Cyc_CfFlowInfo_ReachableFL;
_temp29.f1= _temp23; _temp29;}); _temp28;}); _LL18:;} static void* Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void*
inflow, void* r){ void* _temp30= inflow; struct Cyc_Dict_Dict* _temp36; _LL32:
if( _temp30 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL33;} else{ goto _LL34;}
_LL34: if(( unsigned int) _temp30 >  1u?*(( int*) _temp30) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL37: _temp36=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp30)->f1;
goto _LL35;} else{ goto _LL31;} _LL33: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL35: if( Cyc_CfFlowInfo_initlevel( _temp36, r) != ( void*) Cyc_CfFlowInfo_AllIL){({
void* _temp38[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("expression may not be fully initialized",
sizeof( unsigned char), 40u), _tag_arr( _temp38, sizeof( void*), 0u));});}{
struct Cyc_Dict_Dict* _temp39= Cyc_CfFlowInfo_escape_deref( _temp36, env->all_changed,
r); if( _temp36 ==  _temp39){ return inflow;}{ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp40=({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp41=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp41[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp42; _temp42.tag= Cyc_CfFlowInfo_ReachableFL;
_temp42.f1= _temp39; _temp42;}); _temp41;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp40); return( void*) _temp40;}} _LL31:;} struct _tuple4{ void*
f1; struct Cyc_List_List* f2; } ; static struct _tuple4 Cyc_NewControlFlow_anal_unordered_Rexps(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* es){
if( es ==  0){ return({ struct _tuple4 _temp43; _temp43.f1= inflow; _temp43.f2=
0; _temp43;});} if( es->tl ==  0){ void* _temp46; void* _temp48; struct _tuple0
_temp44= Cyc_NewControlFlow_anal_Rexp( env, inflow,( struct Cyc_Absyn_Exp*) es->hd);
_LL49: _temp48= _temp44.f1; goto _LL47; _LL47: _temp46= _temp44.f2; goto _LL45;
_LL45: return({ struct _tuple4 _temp50; _temp50.f1= _temp48; _temp50.f2=({
struct Cyc_List_List* _temp51=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp51->hd=( void*) _temp46; _temp51->tl= 0; _temp51;});
_temp50;});}{ struct Cyc_Set_Set** outer_all_changed= env->all_changed; struct
Cyc_Set_Set* this_all_changed; void* old_inflow; void* outflow; struct Cyc_List_List*
rvals; do { this_all_changed= Cyc_CfFlowInfo_mt_place_set(); env->all_changed=({
struct Cyc_Set_Set** _temp52=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp52[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp52;});{ void*
_temp55; void* _temp57; struct _tuple0 _temp53= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) es->hd); _LL58: _temp57= _temp53.f1; goto
_LL56; _LL56: _temp55= _temp53.f2; goto _LL54; _LL54: outflow= _temp57; rvals=({
struct Cyc_List_List* _temp59=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp59->hd=( void*) _temp55; _temp59->tl= 0; _temp59;});
this_all_changed=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)( this_all_changed,*(( struct Cyc_Set_Set**) _check_null(
env->all_changed)));{ struct Cyc_List_List* es2= es->tl; for( 0; es2 !=  0; es2=
es2->tl){ env->all_changed=({ struct Cyc_Set_Set** _temp60=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp60[ 0]= Cyc_CfFlowInfo_mt_place_set();
_temp60;});{ void* _temp63; void* _temp65; struct _tuple0 _temp61= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) es2->hd); _LL66: _temp65= _temp61.f1; goto
_LL64; _LL64: _temp63= _temp61.f2; goto _LL62; _LL62: rvals=({ struct Cyc_List_List*
_temp67=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp67->hd=( void*) _temp63; _temp67->tl= rvals; _temp67;}); outflow= Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& this_all_changed, outflow, _temp65, this_all_changed,*((
struct Cyc_Set_Set**) _check_null( env->all_changed))); this_all_changed=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
this_all_changed,*(( struct Cyc_Set_Set**) _check_null( env->all_changed)));}}}
old_inflow= inflow; inflow= Cyc_CfFlowInfo_join_flow( outer_all_changed, inflow,
outflow);}} while ( ! Cyc_CfFlowInfo_flow_lessthan_approx( inflow, old_inflow));
if( outer_all_changed ==  0){ env->all_changed= 0;} else{ env->all_changed=({
struct Cyc_Set_Set** _temp68=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp68[ 0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(* outer_all_changed,
this_all_changed); _temp68;});} Cyc_NewControlFlow_update_tryflow( env, outflow);
return({ struct _tuple4 _temp69; _temp69.f1= outflow; _temp69.f2= Cyc_List_imp_rev(
rvals); _temp69;});}} static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* es){
struct Cyc_List_List* _temp72; void* _temp74; struct _tuple4 _temp70= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, es); _LL75: _temp74= _temp70.f1; goto _LL73; _LL73: _temp72=
_temp70.f2; goto _LL71; _LL71:{ void* _temp76= _temp74; struct Cyc_Dict_Dict*
_temp82; _LL78: if(( unsigned int) _temp76 >  1u?*(( int*) _temp76) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL83: _temp82=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp76)->f1;
goto _LL79;} else{ goto _LL80;} _LL80: if( _temp76 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL81;} else{ goto _LL77;} _LL79: for( 0; _temp72 !=  0;( _temp72= _temp72->tl,
es=(( struct Cyc_List_List*) _check_null( es))->tl)){ if( Cyc_CfFlowInfo_initlevel(
_temp82,( void*) _temp72->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp84[ 0u]={}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp84, sizeof( void*), 0u));});}} goto
_LL77; _LL81: goto _LL77; _LL77:;} return({ struct _tuple0 _temp85; _temp85.f1=
_temp74; _temp85.f2= Cyc_CfFlowInfo_unknown_all; _temp85;});} static void* Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* outflow, struct Cyc_Absyn_Exp*
e, void* il){ void* _temp86= outflow; struct Cyc_Dict_Dict* _temp92; _LL88: if(
_temp86 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL89;} else{ goto _LL90;}
_LL90: if(( unsigned int) _temp86 >  1u?*(( int*) _temp86) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL93: _temp92=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp86)->f1;
goto _LL91;} else{ goto _LL87;} _LL89: return outflow; _LL91: { void* _temp94=(
Cyc_NewControlFlow_anal_Lexp( env, inflow, e)).f2; struct Cyc_CfFlowInfo_Place*
_temp100; _LL96: if( _temp94 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL97;}
else{ goto _LL98;} _LL98: if(( unsigned int) _temp94 >  1u?*(( int*) _temp94) == 
Cyc_CfFlowInfo_PlaceL: 0){ _LL101: _temp100=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp94)->f1; goto _LL99;} else{ goto _LL95;} _LL97: return outflow; _LL99: {
void* nzval= il == ( void*) Cyc_CfFlowInfo_AllIL?( void*) Cyc_CfFlowInfo_NotZeroAll:(
void*) Cyc_CfFlowInfo_NotZeroThis; return( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp102=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp102[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp103; _temp103.tag= Cyc_CfFlowInfo_ReachableFL; _temp103.f1= Cyc_CfFlowInfo_assign_place(
e->loc, _temp92, env->all_changed, _temp100, nzval); _temp103;}); _temp102;});}
_LL95:;} _LL87:;} static struct _tuple0 Cyc_NewControlFlow_splitzero( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, void* outflow, struct Cyc_Absyn_Exp* e, void* il){ void*
_temp104= outflow; struct Cyc_Dict_Dict* _temp110; _LL106: if( _temp104 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL107;} else{ goto _LL108;} _LL108: if((
unsigned int) _temp104 >  1u?*(( int*) _temp104) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL111: _temp110=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp104)->f1;
goto _LL109;} else{ goto _LL105;} _LL107: return({ struct _tuple0 _temp112;
_temp112.f1= outflow; _temp112.f2= outflow; _temp112;}); _LL109: { void*
_temp113=( Cyc_NewControlFlow_anal_Lexp( env, inflow, e)).f2; struct Cyc_CfFlowInfo_Place*
_temp119; _LL115: if( _temp113 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL116;}
else{ goto _LL117;} _LL117: if(( unsigned int) _temp113 >  1u?*(( int*) _temp113)
==  Cyc_CfFlowInfo_PlaceL: 0){ _LL120: _temp119=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp113)->f1; goto _LL118;} else{ goto _LL114;} _LL116: return({ struct _tuple0
_temp121; _temp121.f1= outflow; _temp121.f2= outflow; _temp121;}); _LL118: {
void* nzval= il == ( void*) Cyc_CfFlowInfo_AllIL?( void*) Cyc_CfFlowInfo_NotZeroAll:(
void*) Cyc_CfFlowInfo_NotZeroThis; return({ struct _tuple0 _temp122; _temp122.f1=(
void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp125=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp125[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp126; _temp126.tag= Cyc_CfFlowInfo_ReachableFL;
_temp126.f1= Cyc_CfFlowInfo_assign_place( e->loc, _temp110, env->all_changed,
_temp119, nzval); _temp126;}); _temp125;}); _temp122.f2=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp123=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp123[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp124; _temp124.tag= Cyc_CfFlowInfo_ReachableFL; _temp124.f1= Cyc_CfFlowInfo_assign_place(
e->loc, _temp110, env->all_changed, _temp119,( void*) Cyc_CfFlowInfo_Zero);
_temp124;}); _temp123;}); _temp122;});} _LL114:;} _LL105:;} static struct
_tuple0 Cyc_NewControlFlow_anal_derefR( struct Cyc_NewControlFlow_AnalEnv* env,
void* inflow, void* f, struct Cyc_Absyn_Exp* e, void* r){ void* _temp127= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo
_temp133; struct Cyc_Absyn_Conref* _temp135; void* _temp137; _LL129: if((
unsigned int) _temp127 >  4u?*(( int*) _temp127) ==  Cyc_Absyn_PointerType: 0){
_LL134: _temp133=(( struct Cyc_Absyn_PointerType_struct*) _temp127)->f1; _LL138:
_temp137=( void*) _temp133.elt_typ; goto _LL136; _LL136: _temp135= _temp133.bounds;
goto _LL130;} else{ goto _LL131;} _LL131: goto _LL132; _LL130: { void* _temp139=
f; struct Cyc_Dict_Dict* _temp145; _LL141: if( _temp139 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int) _temp139 >  1u?*((
int*) _temp139) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL146: _temp145=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp139)->f1; goto _LL144;} else{ goto
_LL140;} _LL142: return({ struct _tuple0 _temp147; _temp147.f1= f; _temp147.f2=
Cyc_CfFlowInfo_typ_to_absrval( _temp137, Cyc_CfFlowInfo_unknown_all); _temp147;});
_LL144: if( Cyc_Tcutil_is_bound_one( _temp135)){ void* _temp148= r; struct Cyc_CfFlowInfo_Place*
_temp162; void* _temp164; _LL150: if( _temp148 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL151;} else{ goto _LL152;} _LL152: if( _temp148 == ( void*) Cyc_CfFlowInfo_NotZeroThis){
goto _LL153;} else{ goto _LL154;} _LL154: if(( unsigned int) _temp148 >  3u?*((
int*) _temp148) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL163: _temp162=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp148)->f1; goto _LL155;} else{ goto _LL156;} _LL156: if( _temp148 == ( void*)
Cyc_CfFlowInfo_Zero){ goto _LL157;} else{ goto _LL158;} _LL158: if((
unsigned int) _temp148 >  3u?*(( int*) _temp148) ==  Cyc_CfFlowInfo_UnknownR: 0){
_LL165: _temp164=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp148)->f1;
goto _LL159;} else{ goto _LL160;} _LL160: goto _LL161; _LL151: goto _LL153;
_LL153:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero)); goto
_LL149; _LL155:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero));
return({ struct _tuple0 _temp166; _temp166.f1= f; _temp166.f2= Cyc_CfFlowInfo_lookup_place(
_temp145, _temp162); _temp166;}); _LL157:( void*)( e->annot=( void*)(( void*)
Cyc_CfFlowInfo_IsZero)); return({ struct _tuple0 _temp167; _temp167.f1=( void*)
Cyc_CfFlowInfo_BottomFL; _temp167.f2= Cyc_CfFlowInfo_typ_to_absrval( _temp137,
Cyc_CfFlowInfo_unknown_all); _temp167;}); _LL159: f= Cyc_NewControlFlow_notzero(
env, inflow, f, e, _temp164); goto _LL161; _LL161:( void*)( e->annot=( void*)((
void*) Cyc_CfFlowInfo_UnknownZ)); _LL149:;} else{( void*)( e->annot=( void*)((
void*) Cyc_CfFlowInfo_UnknownZ));}{ void* _temp168= Cyc_CfFlowInfo_initlevel(
_temp145, r); _LL170: if( _temp168 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL171;} else{ goto _LL172;} _LL172: if( _temp168 == ( void*) Cyc_CfFlowInfo_AllIL){
goto _LL173;} else{ goto _LL174;} _LL174: if( _temp168 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL175;} else{ goto _LL169;} _LL171:({ void* _temp176[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("dereference of possibly uninitialized pointer", sizeof(
unsigned char), 46u), _tag_arr( _temp176, sizeof( void*), 0u));}); goto _LL173;
_LL173: return({ struct _tuple0 _temp177; _temp177.f1= f; _temp177.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp137, Cyc_CfFlowInfo_unknown_all); _temp177;}); _LL175: return({ struct
_tuple0 _temp178; _temp178.f1= f; _temp178.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp137, Cyc_CfFlowInfo_unknown_none); _temp178;}); _LL169:;} _LL140:;} _LL132:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp179=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp179[ 0]=({ struct
Cyc_Core_Impossible_struct _temp180; _temp180.tag= Cyc_Core_Impossible; _temp180.f1=
_tag_arr("right deref of non-pointer-type", sizeof( unsigned char), 32u);
_temp180;}); _temp179;})); _LL128:;} struct _tuple5{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e){
struct Cyc_Dict_Dict* d;{ void* _temp181= inflow; struct Cyc_Dict_Dict* _temp187;
_LL183: if( _temp181 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL184;} else{
goto _LL185;} _LL185: if(( unsigned int) _temp181 >  1u?*(( int*) _temp181) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL188: _temp187=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp181)->f1; goto _LL186;} else{ goto _LL182;} _LL184: return({ struct _tuple0
_temp189; _temp189.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp189.f2= Cyc_CfFlowInfo_unknown_all;
_temp189;}); _LL186: d= _temp187; _LL182:;}{ void* _temp190=( void*) e->r;
struct Cyc_Absyn_Exp* _temp288; struct Cyc_Absyn_Exp* _temp290; struct Cyc_Absyn_Exp*
_temp292; void* _temp294; void* _temp296; int _temp298; void* _temp300; void*
_temp302; void* _temp304; void* _temp306; struct Cyc_List_List* _temp308; void*
_temp310; void* _temp312; struct Cyc_Absyn_Vardecl* _temp314; void* _temp316;
struct Cyc_Absyn_Vardecl* _temp318; void* _temp320; struct Cyc_Absyn_Vardecl*
_temp322; struct Cyc_List_List* _temp324; struct Cyc_Absyn_Exp* _temp326; struct
Cyc_Absyn_Exp* _temp328; struct Cyc_Core_Opt* _temp330; struct Cyc_Core_Opt
_temp332; struct Cyc_Absyn_Exp* _temp333; struct Cyc_Absyn_Exp* _temp335; struct
Cyc_Core_Opt* _temp337; struct Cyc_Absyn_Exp* _temp339; struct Cyc_Absyn_Exp*
_temp341; struct Cyc_Absyn_Exp* _temp343; struct Cyc_Absyn_Exp* _temp345; struct
Cyc_List_List* _temp347; struct Cyc_Absyn_Exp* _temp349; void* _temp351; struct
Cyc_Absyn_Exp* _temp353; struct Cyc_Absyn_Exp* _temp355; struct Cyc_Absyn_Exp*
_temp357; struct Cyc_Absyn_Exp* _temp359; struct Cyc_Absyn_Exp* _temp361; struct
_tagged_arr* _temp363; struct Cyc_Absyn_Exp* _temp365; struct _tagged_arr*
_temp367; struct Cyc_Absyn_Exp* _temp369; struct Cyc_Absyn_Exp* _temp371; struct
Cyc_Absyn_Exp* _temp373; struct Cyc_Absyn_Exp* _temp375; struct Cyc_Absyn_Exp*
_temp377; struct Cyc_Absyn_Exp* _temp379; struct Cyc_List_List* _temp381; struct
Cyc_List_List* _temp383; struct Cyc_List_List* _temp385; struct Cyc_List_List*
_temp387; struct Cyc_List_List* _temp389; struct Cyc_Absyn_Exp* _temp391; struct
Cyc_Absyn_Exp* _temp393; struct Cyc_Absyn_Vardecl* _temp395; struct Cyc_Absyn_Stmt*
_temp397; void* _temp399; _LL192: if(*(( int*) _temp190) ==  Cyc_Absyn_Cast_e){
_LL289: _temp288=(( struct Cyc_Absyn_Cast_e_struct*) _temp190)->f2; goto _LL193;}
else{ goto _LL194;} _LL194: if(*(( int*) _temp190) ==  Cyc_Absyn_NoInstantiate_e){
_LL291: _temp290=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp190)->f1;
goto _LL195;} else{ goto _LL196;} _LL196: if(*(( int*) _temp190) ==  Cyc_Absyn_Instantiate_e){
_LL293: _temp292=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp190)->f1; goto
_LL197;} else{ goto _LL198;} _LL198: if(*(( int*) _temp190) ==  Cyc_Absyn_Const_e){
_LL295: _temp294=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp190)->f1; if(
_temp294 == ( void*) Cyc_Absyn_Null_c){ goto _LL199;} else{ goto _LL200;}} else{
goto _LL200;} _LL200: if(*(( int*) _temp190) ==  Cyc_Absyn_Const_e){ _LL297:
_temp296=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp190)->f1; if((
unsigned int) _temp296 >  1u?*(( int*) _temp296) ==  Cyc_Absyn_Int_c: 0){ _LL301:
_temp300=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp296)->f1; if( _temp300
== ( void*) Cyc_Absyn_Signed){ goto _LL299;} else{ goto _LL202;} _LL299:
_temp298=(( struct Cyc_Absyn_Int_c_struct*) _temp296)->f2; if( _temp298 ==  0){
goto _LL201;} else{ goto _LL202;}} else{ goto _LL202;}} else{ goto _LL202;}
_LL202: if(*(( int*) _temp190) ==  Cyc_Absyn_Const_e){ _LL303: _temp302=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp190)->f1; if(( unsigned int) _temp302 > 
1u?*(( int*) _temp302) ==  Cyc_Absyn_Int_c: 0){ _LL305: _temp304=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp302)->f1; if( _temp304 == ( void*) Cyc_Absyn_Signed){
goto _LL203;} else{ goto _LL204;}} else{ goto _LL204;}} else{ goto _LL204;}
_LL204: if(*(( int*) _temp190) ==  Cyc_Absyn_Var_e){ _LL307: _temp306=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp190)->f2; if(( unsigned int) _temp306 >  1u?*((
int*) _temp306) ==  Cyc_Absyn_Funname_b: 0){ goto _LL205;} else{ goto _LL206;}}
else{ goto _LL206;} _LL206: if(*(( int*) _temp190) ==  Cyc_Absyn_Tunion_e){
_LL309: _temp308=(( struct Cyc_Absyn_Tunion_e_struct*) _temp190)->f3; if(
_temp308 ==  0){ goto _LL207;} else{ goto _LL208;}} else{ goto _LL208;} _LL208:
if(*(( int*) _temp190) ==  Cyc_Absyn_Const_e){ goto _LL209;} else{ goto _LL210;}
_LL210: if(*(( int*) _temp190) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL211;} else{
goto _LL212;} _LL212: if(*(( int*) _temp190) ==  Cyc_Absyn_Sizeoftyp_e){ goto
_LL213;} else{ goto _LL214;} _LL214: if(*(( int*) _temp190) ==  Cyc_Absyn_Offsetof_e){
goto _LL215;} else{ goto _LL216;} _LL216: if(*(( int*) _temp190) ==  Cyc_Absyn_Gentyp_e){
goto _LL217;} else{ goto _LL218;} _LL218: if(*(( int*) _temp190) ==  Cyc_Absyn_AnonEnum_e){
goto _LL219;} else{ goto _LL220;} _LL220: if(*(( int*) _temp190) ==  Cyc_Absyn_Enum_e){
goto _LL221;} else{ goto _LL222;} _LL222: if(*(( int*) _temp190) ==  Cyc_Absyn_Var_e){
_LL311: _temp310=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp190)->f2; if((
unsigned int) _temp310 >  1u?*(( int*) _temp310) ==  Cyc_Absyn_Global_b: 0){
goto _LL223;} else{ goto _LL224;}} else{ goto _LL224;} _LL224: if(*(( int*)
_temp190) ==  Cyc_Absyn_Var_e){ _LL313: _temp312=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp190)->f2; if(( unsigned int) _temp312 >  1u?*(( int*) _temp312) ==  Cyc_Absyn_Param_b:
0){ _LL315: _temp314=(( struct Cyc_Absyn_Param_b_struct*) _temp312)->f1; goto
_LL225;} else{ goto _LL226;}} else{ goto _LL226;} _LL226: if(*(( int*) _temp190)
==  Cyc_Absyn_Var_e){ _LL317: _temp316=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp190)->f2; if(( unsigned int) _temp316 >  1u?*(( int*) _temp316) ==  Cyc_Absyn_Local_b:
0){ _LL319: _temp318=(( struct Cyc_Absyn_Local_b_struct*) _temp316)->f1; goto
_LL227;} else{ goto _LL228;}} else{ goto _LL228;} _LL228: if(*(( int*) _temp190)
==  Cyc_Absyn_Var_e){ _LL321: _temp320=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp190)->f2; if(( unsigned int) _temp320 >  1u?*(( int*) _temp320) ==  Cyc_Absyn_Pat_b:
0){ _LL323: _temp322=(( struct Cyc_Absyn_Pat_b_struct*) _temp320)->f1; goto
_LL229;} else{ goto _LL230;}} else{ goto _LL230;} _LL230: if(*(( int*) _temp190)
==  Cyc_Absyn_Primop_e){ _LL325: _temp324=(( struct Cyc_Absyn_Primop_e_struct*)
_temp190)->f2; goto _LL231;} else{ goto _LL232;} _LL232: if(*(( int*) _temp190)
==  Cyc_Absyn_Increment_e){ _LL327: _temp326=(( struct Cyc_Absyn_Increment_e_struct*)
_temp190)->f1; goto _LL233;} else{ goto _LL234;} _LL234: if(*(( int*) _temp190)
==  Cyc_Absyn_AssignOp_e){ _LL334: _temp333=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp190)->f1; goto _LL331; _LL331: _temp330=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp190)->f2; if( _temp330 ==  0){ goto _LL236;} else{ _temp332=* _temp330;
goto _LL329;} _LL329: _temp328=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp190)->f3;
goto _LL235;} else{ goto _LL236;} _LL236: if(*(( int*) _temp190) ==  Cyc_Absyn_AssignOp_e){
_LL340: _temp339=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp190)->f1; goto
_LL338; _LL338: _temp337=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp190)->f2;
if( _temp337 ==  0){ goto _LL336;} else{ goto _LL238;} _LL336: _temp335=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp190)->f3; goto _LL237;} else{ goto
_LL238;} _LL238: if(*(( int*) _temp190) ==  Cyc_Absyn_SeqExp_e){ _LL344:
_temp343=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp190)->f1; goto _LL342;
_LL342: _temp341=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp190)->f2; goto
_LL239;} else{ goto _LL240;} _LL240: if(*(( int*) _temp190) ==  Cyc_Absyn_Throw_e){
_LL346: _temp345=(( struct Cyc_Absyn_Throw_e_struct*) _temp190)->f1; goto _LL241;}
else{ goto _LL242;} _LL242: if(*(( int*) _temp190) ==  Cyc_Absyn_FnCall_e){
_LL350: _temp349=(( struct Cyc_Absyn_FnCall_e_struct*) _temp190)->f1; goto
_LL348; _LL348: _temp347=(( struct Cyc_Absyn_FnCall_e_struct*) _temp190)->f2;
goto _LL243;} else{ goto _LL244;} _LL244: if(*(( int*) _temp190) ==  Cyc_Absyn_Malloc_e){
_LL354: _temp353=(( struct Cyc_Absyn_Malloc_e_struct*) _temp190)->f1; goto
_LL352; _LL352: _temp351=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp190)->f2;
goto _LL245;} else{ goto _LL246;} _LL246: if(*(( int*) _temp190) ==  Cyc_Absyn_New_e){
_LL358: _temp357=(( struct Cyc_Absyn_New_e_struct*) _temp190)->f1; goto _LL356;
_LL356: _temp355=(( struct Cyc_Absyn_New_e_struct*) _temp190)->f2; goto _LL247;}
else{ goto _LL248;} _LL248: if(*(( int*) _temp190) ==  Cyc_Absyn_Address_e){
_LL360: _temp359=(( struct Cyc_Absyn_Address_e_struct*) _temp190)->f1; goto
_LL249;} else{ goto _LL250;} _LL250: if(*(( int*) _temp190) ==  Cyc_Absyn_Deref_e){
_LL362: _temp361=(( struct Cyc_Absyn_Deref_e_struct*) _temp190)->f1; goto _LL251;}
else{ goto _LL252;} _LL252: if(*(( int*) _temp190) ==  Cyc_Absyn_StructMember_e){
_LL366: _temp365=(( struct Cyc_Absyn_StructMember_e_struct*) _temp190)->f1; goto
_LL364; _LL364: _temp363=(( struct Cyc_Absyn_StructMember_e_struct*) _temp190)->f2;
goto _LL253;} else{ goto _LL254;} _LL254: if(*(( int*) _temp190) ==  Cyc_Absyn_StructArrow_e){
_LL370: _temp369=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp190)->f1; goto
_LL368; _LL368: _temp367=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp190)->f2;
goto _LL255;} else{ goto _LL256;} _LL256: if(*(( int*) _temp190) ==  Cyc_Absyn_Conditional_e){
_LL376: _temp375=(( struct Cyc_Absyn_Conditional_e_struct*) _temp190)->f1; goto
_LL374; _LL374: _temp373=(( struct Cyc_Absyn_Conditional_e_struct*) _temp190)->f2;
goto _LL372; _LL372: _temp371=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp190)->f3; goto _LL257;} else{ goto _LL258;} _LL258: if(*(( int*) _temp190)
==  Cyc_Absyn_Subscript_e){ _LL380: _temp379=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp190)->f1; goto _LL378; _LL378: _temp377=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp190)->f2; goto _LL259;} else{ goto _LL260;} _LL260: if(*(( int*) _temp190)
==  Cyc_Absyn_Tunion_e){ _LL382: _temp381=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp190)->f3; goto _LL261;} else{ goto _LL262;} _LL262: if(*(( int*) _temp190)
==  Cyc_Absyn_Tuple_e){ _LL384: _temp383=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp190)->f1; goto _LL263;} else{ goto _LL264;} _LL264: if(*(( int*) _temp190)
==  Cyc_Absyn_AnonStruct_e){ _LL386: _temp385=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp190)->f2; goto _LL265;} else{ goto _LL266;} _LL266: if(*(( int*) _temp190)
==  Cyc_Absyn_Struct_e){ _LL388: _temp387=(( struct Cyc_Absyn_Struct_e_struct*)
_temp190)->f3; goto _LL267;} else{ goto _LL268;} _LL268: if(*(( int*) _temp190)
==  Cyc_Absyn_Array_e){ _LL390: _temp389=(( struct Cyc_Absyn_Array_e_struct*)
_temp190)->f1; goto _LL269;} else{ goto _LL270;} _LL270: if(*(( int*) _temp190)
==  Cyc_Absyn_Comprehension_e){ _LL396: _temp395=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp190)->f1; goto _LL394; _LL394: _temp393=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp190)->f2; goto _LL392; _LL392: _temp391=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp190)->f3; goto _LL271;} else{ goto _LL272;} _LL272: if(*(( int*) _temp190)
==  Cyc_Absyn_StmtExp_e){ _LL398: _temp397=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp190)->f1; goto _LL273;} else{ goto _LL274;} _LL274: if(*(( int*) _temp190)
==  Cyc_Absyn_Var_e){ _LL400: _temp399=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp190)->f2; if( _temp399 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL275;}
else{ goto _LL276;}} else{ goto _LL276;} _LL276: if(*(( int*) _temp190) ==  Cyc_Absyn_UnknownId_e){
goto _LL277;} else{ goto _LL278;} _LL278: if(*(( int*) _temp190) ==  Cyc_Absyn_UnknownCall_e){
goto _LL279;} else{ goto _LL280;} _LL280: if(*(( int*) _temp190) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL281;} else{ goto _LL282;} _LL282: if(*(( int*) _temp190) ==  Cyc_Absyn_CompoundLit_e){
goto _LL283;} else{ goto _LL284;} _LL284: if(*(( int*) _temp190) ==  Cyc_Absyn_Codegen_e){
goto _LL285;} else{ goto _LL286;} _LL286: if(*(( int*) _temp190) ==  Cyc_Absyn_Fill_e){
goto _LL287;} else{ goto _LL191;} _LL193: _temp290= _temp288; goto _LL195;
_LL195: _temp292= _temp290; goto _LL197; _LL197: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp292); _LL199: goto _LL201; _LL201: return({ struct _tuple0
_temp401; _temp401.f1= inflow; _temp401.f2=( void*) Cyc_CfFlowInfo_Zero;
_temp401;}); _LL203: goto _LL205; _LL205: return({ struct _tuple0 _temp402;
_temp402.f1= inflow; _temp402.f2=( void*) Cyc_CfFlowInfo_NotZeroAll; _temp402;});
_LL207: goto _LL209; _LL209: goto _LL211; _LL211: goto _LL213; _LL213: goto
_LL215; _LL215: goto _LL217; _LL217: goto _LL219; _LL219: goto _LL221; _LL221:
return({ struct _tuple0 _temp403; _temp403.f1= inflow; _temp403.f2= Cyc_CfFlowInfo_unknown_all;
_temp403;}); _LL223: return({ struct _tuple0 _temp404; _temp404.f1= inflow;
_temp404.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp404;}); _LL225:
_temp318= _temp314; goto _LL227; _LL227: _temp322= _temp318; goto _LL229; _LL229:
return({ struct _tuple0 _temp405; _temp405.f1= inflow; _temp405.f2= Cyc_Dict_lookup(
d,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp406=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp406[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp407; _temp407.tag= Cyc_CfFlowInfo_VarRoot;
_temp407.f1= _temp322; _temp407;}); _temp406;})); _temp405;}); _LL231: return
Cyc_NewControlFlow_anal_use_ints( env, inflow, _temp324); _LL233: { void*
_temp411; struct _tuple0 _temp409= Cyc_NewControlFlow_anal_use_ints( env, inflow,({
struct Cyc_Absyn_Exp* _temp408[ 1u]; _temp408[ 0u]= _temp326;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp408, sizeof( struct Cyc_Absyn_Exp*),
1u));})); _LL412: _temp411= _temp409.f1; goto _LL410; _LL410: { void* _temp415;
struct _tuple0 _temp413= Cyc_NewControlFlow_anal_Lexp( env, _temp411, _temp326);
_LL416: _temp415= _temp413.f2; goto _LL414; _LL414:{ struct _tuple0 _temp418=({
struct _tuple0 _temp417; _temp417.f1= _temp415; _temp417.f2= _temp411; _temp417;});
void* _temp424; struct Cyc_Dict_Dict* _temp426; void* _temp428; struct Cyc_CfFlowInfo_Place*
_temp430; _LL420: _LL429: _temp428= _temp418.f1; if(( unsigned int) _temp428 > 
1u?*(( int*) _temp428) ==  Cyc_CfFlowInfo_PlaceL: 0){ _LL431: _temp430=(( struct
Cyc_CfFlowInfo_PlaceL_struct*) _temp428)->f1; goto _LL425;} else{ goto _LL422;}
_LL425: _temp424= _temp418.f2; if(( unsigned int) _temp424 >  1u?*(( int*)
_temp424) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL427: _temp426=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp424)->f1; goto _LL421;} else{ goto _LL422;} _LL422: goto _LL423; _LL421:
_temp411=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp432=( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct));
_temp432[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct _temp433; _temp433.tag=
Cyc_CfFlowInfo_ReachableFL; _temp433.f1= Cyc_CfFlowInfo_assign_place( _temp326->loc,
_temp426, env->all_changed, _temp430, Cyc_CfFlowInfo_unknown_all); _temp433;});
_temp432;}); goto _LL419; _LL423: goto _LL419; _LL419:;} return({ struct _tuple0
_temp434; _temp434.f1= _temp411; _temp434.f2= Cyc_CfFlowInfo_unknown_all;
_temp434;});}} _LL235: { void* _temp438; struct _tuple0 _temp436= Cyc_NewControlFlow_anal_use_ints(
env, inflow,({ struct Cyc_Absyn_Exp* _temp435[ 2u]; _temp435[ 1u]= _temp328;
_temp435[ 0u]= _temp333;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp435, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL439: _temp438=
_temp436.f1; goto _LL437; _LL437: { void* _temp442; struct _tuple0 _temp440= Cyc_NewControlFlow_anal_Lexp(
env, _temp438, e); _LL443: _temp442= _temp440.f2; goto _LL441; _LL441:{ struct
_tuple0 _temp445=({ struct _tuple0 _temp444; _temp444.f1= _temp442; _temp444.f2=
_temp438; _temp444;}); void* _temp451; struct Cyc_Dict_Dict* _temp453; void*
_temp455; struct Cyc_CfFlowInfo_Place* _temp457; _LL447: _LL456: _temp455=
_temp445.f1; if(( unsigned int) _temp455 >  1u?*(( int*) _temp455) ==  Cyc_CfFlowInfo_PlaceL:
0){ _LL458: _temp457=(( struct Cyc_CfFlowInfo_PlaceL_struct*) _temp455)->f1;
goto _LL452;} else{ goto _LL449;} _LL452: _temp451= _temp445.f2; if((
unsigned int) _temp451 >  1u?*(( int*) _temp451) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL454: _temp453=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp451)->f1;
goto _LL448;} else{ goto _LL449;} _LL449: goto _LL450; _LL448: _temp438=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp459=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp459[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp460; _temp460.tag= Cyc_CfFlowInfo_ReachableFL;
_temp460.f1= Cyc_CfFlowInfo_assign_place( _temp333->loc, _temp453, env->all_changed,
_temp457, Cyc_CfFlowInfo_unknown_all); _temp460;}); _temp459;}); goto _LL446;
_LL450: goto _LL446; _LL446:;} return({ struct _tuple0 _temp461; _temp461.f1=
_temp438; _temp461.f2= Cyc_CfFlowInfo_unknown_all; _temp461;});}} _LL237: {
struct Cyc_Set_Set** _temp462= env->all_changed; while( 1) { env->all_changed=({
struct Cyc_Set_Set** _temp463=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp463[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp463;});{ void*
_temp466; void* _temp468; struct _tuple0 _temp464= Cyc_NewControlFlow_anal_Lexp(
env, inflow, _temp339); _LL469: _temp468= _temp464.f1; goto _LL467; _LL467:
_temp466= _temp464.f2; goto _LL465; _LL465: { struct Cyc_Set_Set* _temp470=*((
struct Cyc_Set_Set**) _check_null( env->all_changed)); env->all_changed=({
struct Cyc_Set_Set** _temp471=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp471[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp471;});{ void*
_temp474; void* _temp476; struct _tuple0 _temp472= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp335); _LL477: _temp476= _temp472.f1; goto _LL475; _LL475:
_temp474= _temp472.f2; goto _LL473; _LL473: { struct Cyc_Set_Set* _temp478=*((
struct Cyc_Set_Set**) _check_null( env->all_changed)); void* _temp479= Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _temp470, _temp468, _temp476, _temp470, _temp478); void*
_temp480= Cyc_CfFlowInfo_join_flow( _temp462, inflow, _temp479); if( Cyc_CfFlowInfo_flow_lessthan_approx(
_temp480, inflow)){ if( _temp462 ==  0){ env->all_changed= 0;} else{*(( struct
Cyc_Set_Set**) _check_null( env->all_changed))=(( struct Cyc_Set_Set*(*)( struct
Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(* _temp462,((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
_temp470, _temp478));}{ void* _temp481= _temp479; struct Cyc_Dict_Dict* _temp487;
_LL483: if( _temp481 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL484;} else{
goto _LL485;} _LL485: if(( unsigned int) _temp481 >  1u?*(( int*) _temp481) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL488: _temp487=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp481)->f1; goto _LL486;} else{ goto _LL482;} _LL484: return({ struct _tuple0
_temp489; _temp489.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp489.f2= _temp474;
_temp489;}); _LL486: { void* _temp490= _temp466; struct Cyc_CfFlowInfo_Place*
_temp496; _LL492: if(( unsigned int) _temp490 >  1u?*(( int*) _temp490) ==  Cyc_CfFlowInfo_PlaceL:
0){ _LL497: _temp496=(( struct Cyc_CfFlowInfo_PlaceL_struct*) _temp490)->f1;
goto _LL493;} else{ goto _LL494;} _LL494: if( _temp490 == ( void*) Cyc_CfFlowInfo_UnknownL){
goto _LL495;} else{ goto _LL491;} _LL493: _temp487= Cyc_CfFlowInfo_assign_place(
e->loc, _temp487, env->all_changed, _temp496, _temp474); _temp479=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp498=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp498[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp499; _temp499.tag= Cyc_CfFlowInfo_ReachableFL;
_temp499.f1= _temp487; _temp499;}); _temp498;}); Cyc_NewControlFlow_update_tryflow(
env, _temp479); return({ struct _tuple0 _temp500; _temp500.f1= _temp479;
_temp500.f2= _temp474; _temp500;}); _LL495: return({ struct _tuple0 _temp501;
_temp501.f1= Cyc_NewControlFlow_use_Rval( env, _temp335->loc, _temp479, _temp474);
_temp501.f2= _temp474; _temp501;}); _LL491:;} _LL482:;}} inflow= _temp480;}}}}}}
_LL239: return Cyc_NewControlFlow_anal_Rexp( env,( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp343)).f1, _temp341); _LL241: { void* _temp504; void* _temp506;
struct _tuple0 _temp502= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp345);
_LL507: _temp506= _temp502.f1; goto _LL505; _LL505: _temp504= _temp502.f2; goto
_LL503; _LL503: Cyc_NewControlFlow_use_Rval( env, _temp345->loc, _temp506,
_temp504); return({ struct _tuple0 _temp508; _temp508.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp508.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp508;});} _LL243:
_temp347=({ struct Cyc_List_List* _temp509=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp509->hd=( void*) _temp349; _temp509->tl=
_temp347; _temp509;});{ struct Cyc_List_List* _temp512; void* _temp514; struct
_tuple4 _temp510= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp347);
_LL515: _temp514= _temp510.f1; goto _LL513; _LL513: _temp512= _temp510.f2; goto
_LL511; _LL511: for( 0; _temp512 !=  0;( _temp512= _temp512->tl, _temp347=((
struct Cyc_List_List*) _check_null( _temp347))->tl)){ _temp514= Cyc_NewControlFlow_use_Rval(
env,(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp347))->hd)->loc,
_temp514,( void*) _temp512->hd);} return({ struct _tuple0 _temp516; _temp516.f1=
_temp514; _temp516.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp516;});} _LL245: {
void* root=( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp535=( struct
Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp535[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp536; _temp536.tag=
Cyc_CfFlowInfo_MallocPt; _temp536.f1= e; _temp536;}); _temp535;}); struct Cyc_CfFlowInfo_Place*
place=({ struct Cyc_CfFlowInfo_Place* _temp534=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp534->root=( void*) root;
_temp534->fields= 0; _temp534;}); void* rval=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct*
_temp532=( struct Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_AddressOf_struct)); _temp532[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct
_temp533; _temp533.tag= Cyc_CfFlowInfo_AddressOf; _temp533.f1= place; _temp533;});
_temp532;}); void* place_val= Cyc_CfFlowInfo_typ_to_absrval( _temp351, Cyc_CfFlowInfo_unknown_none);
if( env->all_changed !=  0){*(( struct Cyc_Set_Set**) _check_null( env->all_changed))=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( env->all_changed)), place);}
if( _temp353 !=  0){ void* _temp517=( Cyc_NewControlFlow_anal_Rexp( env, inflow,(
struct Cyc_Absyn_Exp*) _check_null( _temp353))).f1; struct Cyc_Dict_Dict*
_temp523; _LL519: if( _temp517 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL520;}
else{ goto _LL521;} _LL521: if(( unsigned int) _temp517 >  1u?*(( int*) _temp517)
==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL524: _temp523=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp517)->f1; goto _LL522;} else{ goto _LL518;} _LL520: return({ struct _tuple0
_temp525; _temp525.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp525.f2= rval;
_temp525;}); _LL522: return({ struct _tuple0 _temp526; _temp526.f1=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp527=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp527[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp528; _temp528.tag= Cyc_CfFlowInfo_ReachableFL;
_temp528.f1= Cyc_Dict_insert( _temp523, root, place_val); _temp528;}); _temp527;});
_temp526.f2= rval; _temp526;}); _LL518:;} return({ struct _tuple0 _temp529;
_temp529.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp530=(
struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct));
_temp530[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct _temp531; _temp531.tag=
Cyc_CfFlowInfo_ReachableFL; _temp531.f1= Cyc_Dict_insert( d, root, place_val);
_temp531;}); _temp530;}); _temp529.f2= rval; _temp529;});} _LL247: { void* root=(
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp565=( struct Cyc_CfFlowInfo_MallocPt_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp565[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp566; _temp566.tag= Cyc_CfFlowInfo_MallocPt;
_temp566.f1= _temp355; _temp566;}); _temp565;}); struct Cyc_CfFlowInfo_Place*
place=({ struct Cyc_CfFlowInfo_Place* _temp564=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp564->root=( void*) root;
_temp564->fields= 0; _temp564;}); void* rval=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct*
_temp562=( struct Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_AddressOf_struct)); _temp562[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct
_temp563; _temp563.tag= Cyc_CfFlowInfo_AddressOf; _temp563.f1= place; _temp563;});
_temp562;}); if( env->all_changed !=  0){*(( struct Cyc_Set_Set**) _check_null(
env->all_changed))=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( env->all_changed)),
place);}{ void* outflow; void* place_val; if( _temp357 !=  0){ struct Cyc_List_List*
_temp540; void* _temp542; struct _tuple4 _temp538= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp537[ 2u]; _temp537[ 1u]= _temp355;
_temp537[ 0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp357);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp537, sizeof( struct Cyc_Absyn_Exp*),
2u));})); _LL543: _temp542= _temp538.f1; goto _LL541; _LL541: _temp540= _temp538.f2;
goto _LL539; _LL539: outflow= _temp542; place_val=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp540))->tl))->hd;} else{
void* _temp546; void* _temp548; struct _tuple0 _temp544= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp355); _LL549: _temp548= _temp544.f1; goto _LL547; _LL547:
_temp546= _temp544.f2; goto _LL545; _LL545: outflow= _temp548; place_val=
_temp546;}{ void* _temp550= outflow; struct Cyc_Dict_Dict* _temp556; _LL552: if(
_temp550 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL553;} else{ goto _LL554;}
_LL554: if(( unsigned int) _temp550 >  1u?*(( int*) _temp550) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL557: _temp556=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp550)->f1;
goto _LL555;} else{ goto _LL551;} _LL553: return({ struct _tuple0 _temp558;
_temp558.f1= outflow; _temp558.f2= rval; _temp558;}); _LL555: return({ struct
_tuple0 _temp559; _temp559.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp560=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp560[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp561; _temp561.tag= Cyc_CfFlowInfo_ReachableFL; _temp561.f1= Cyc_Dict_insert(
_temp556, root, place_val); _temp561;}); _temp560;}); _temp559.f2= rval;
_temp559;}); _LL551:;}}} _LL249: { void* _temp569; void* _temp571; struct
_tuple0 _temp567= Cyc_NewControlFlow_anal_Lexp( env, inflow, _temp359); _LL572:
_temp571= _temp567.f1; goto _LL570; _LL570: _temp569= _temp567.f2; goto _LL568;
_LL568: { void* _temp573= _temp569; struct Cyc_CfFlowInfo_Place* _temp579;
_LL575: if( _temp573 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL576;} else{
goto _LL577;} _LL577: if(( unsigned int) _temp573 >  1u?*(( int*) _temp573) == 
Cyc_CfFlowInfo_PlaceL: 0){ _LL580: _temp579=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp573)->f1; goto _LL578;} else{ goto _LL574;} _LL576: return({ struct _tuple0
_temp581; _temp581.f1= _temp571; _temp581.f2=( void*) Cyc_CfFlowInfo_NotZeroAll;
_temp581;}); _LL578: return({ struct _tuple0 _temp582; _temp582.f1= _temp571;
_temp582.f2=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct* _temp583=( struct
Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_AddressOf_struct));
_temp583[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct _temp584; _temp584.tag=
Cyc_CfFlowInfo_AddressOf; _temp584.f1= _temp579; _temp584;}); _temp583;});
_temp582;}); _LL574:;}} _LL251: { void* _temp587; void* _temp589; struct _tuple0
_temp585= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp361); _LL590: _temp589=
_temp585.f1; goto _LL588; _LL588: _temp587= _temp585.f2; goto _LL586; _LL586:
return Cyc_NewControlFlow_anal_derefR( env, inflow, _temp589, _temp361, _temp587);}
_LL253: { void* _temp593; void* _temp595; struct _tuple0 _temp591= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp365); _LL596: _temp595= _temp591.f1; goto _LL594; _LL594:
_temp593= _temp591.f2; goto _LL592; _LL592:{ void* _temp597= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp365->topt))->v); _LL599: if((
unsigned int) _temp597 >  4u?*(( int*) _temp597) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL600;} else{ goto _LL601;} _LL601: if(( unsigned int) _temp597 >  4u?*((
int*) _temp597) ==  Cyc_Absyn_UnionType: 0){ goto _LL602;} else{ goto _LL603;}
_LL603: goto _LL604; _LL600: goto _LL602; _LL602: return({ struct _tuple0
_temp605; _temp605.f1= _temp595; _temp605.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp605;}); _LL604: goto _LL598; _LL598:;}{ void* _temp606= _temp593; struct
Cyc_Dict_Dict* _temp612; _LL608: if(( unsigned int) _temp606 >  3u?*(( int*)
_temp606) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL613: _temp612=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp606)->f1; goto _LL609;} else{ goto _LL610;} _LL610: goto _LL611; _LL609:
return({ struct _tuple0 _temp614; _temp614.f1= _temp595; _temp614.f2=(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp612,
_temp363); _temp614;}); _LL611:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp615=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp615[ 0]=({ struct Cyc_Core_Impossible_struct _temp616; _temp616.tag= Cyc_Core_Impossible;
_temp616.f1= _tag_arr("anal_Rexp: StructMember", sizeof( unsigned char), 24u);
_temp616;}); _temp615;})); _LL607:;}} _LL255: { void* _temp619; void* _temp621;
struct _tuple0 _temp617= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp369);
_LL622: _temp621= _temp617.f1; goto _LL620; _LL620: _temp619= _temp617.f2; goto
_LL618; _LL618: { void* _temp625; void* _temp627; struct _tuple0 _temp623= Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp621, _temp369, _temp619); _LL628: _temp627= _temp623.f1; goto
_LL626; _LL626: _temp625= _temp623.f2; goto _LL624; _LL624:{ void* _temp629= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp369->topt))->v); struct Cyc_Absyn_PtrInfo
_temp635; void* _temp637; _LL631: if(( unsigned int) _temp629 >  4u?*(( int*)
_temp629) ==  Cyc_Absyn_PointerType: 0){ _LL636: _temp635=(( struct Cyc_Absyn_PointerType_struct*)
_temp629)->f1; _LL638: _temp637=( void*) _temp635.elt_typ; goto _LL632;} else{
goto _LL633;} _LL633: goto _LL634; _LL632:{ void* _temp639= Cyc_Tcutil_compress(
_temp637); _LL641: if(( unsigned int) _temp639 >  4u?*(( int*) _temp639) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL642;} else{ goto _LL643;} _LL643: if(( unsigned int) _temp639 >  4u?*((
int*) _temp639) ==  Cyc_Absyn_UnionType: 0){ goto _LL644;} else{ goto _LL645;}
_LL645: goto _LL646; _LL642: goto _LL644; _LL644: return({ struct _tuple0
_temp647; _temp647.f1= _temp627; _temp647.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp647;}); _LL646: goto _LL640; _LL640:;} goto _LL630; _LL634:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp648=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp648[ 0]=({ struct
Cyc_Core_Impossible_struct _temp649; _temp649.tag= Cyc_Core_Impossible; _temp649.f1=
_tag_arr("anal_Rexp: StructArrow ptr", sizeof( unsigned char), 27u); _temp649;});
_temp648;})); _LL630:;}{ void* _temp650= _temp625; struct Cyc_Dict_Dict*
_temp656; _LL652: if(( unsigned int) _temp650 >  3u?*(( int*) _temp650) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL657: _temp656=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp650)->f1;
goto _LL653;} else{ goto _LL654;} _LL654: goto _LL655; _LL653: return({ struct
_tuple0 _temp658; _temp658.f1= _temp627; _temp658.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp656, _temp367); _temp658;});
_LL655:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp659=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp659[ 0]=({ struct Cyc_Core_Impossible_struct _temp660; _temp660.tag= Cyc_Core_Impossible;
_temp660.f1= _tag_arr("anal_Rexp: StructArrow", sizeof( unsigned char), 23u);
_temp660;}); _temp659;})); _LL651:;}}} _LL257: { void* _temp663; void* _temp665;
struct _tuple0 _temp661= Cyc_NewControlFlow_anal_test( env, inflow, _temp375);
_LL666: _temp665= _temp661.f1; goto _LL664; _LL664: _temp663= _temp661.f2; goto
_LL662; _LL662: { void* _temp669; void* _temp671; struct _tuple0 _temp667= Cyc_NewControlFlow_anal_Rexp(
env, _temp665, _temp373); _LL672: _temp671= _temp667.f1; goto _LL670; _LL670:
_temp669= _temp667.f2; goto _LL668; _LL668: { void* _temp675; void* _temp677;
struct _tuple0 _temp673= Cyc_NewControlFlow_anal_Rexp( env, _temp663, _temp371);
_LL678: _temp677= _temp673.f1; goto _LL676; _LL676: _temp675= _temp673.f2; goto
_LL674; _LL674: return({ struct _tuple0 _temp679; _temp679.f1= Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_use_Rval( env, _temp373->loc, _temp671,
_temp669), Cyc_NewControlFlow_use_Rval( env, _temp371->loc, _temp677, _temp675));
_temp679.f2= Cyc_CfFlowInfo_unknown_all; _temp679;});}}} _LL259: { struct Cyc_List_List*
_temp683; void* _temp685; struct _tuple4 _temp681= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp680[ 2u]; _temp680[ 1u]= _temp377;
_temp680[ 0u]= _temp379;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp680, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL686: _temp685=
_temp681.f1; goto _LL684; _LL684: _temp683= _temp681.f2; goto _LL682; _LL682:{
void* _temp687= _temp685; struct Cyc_Dict_Dict* _temp693; _LL689: if((
unsigned int) _temp687 >  1u?*(( int*) _temp687) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL694: _temp693=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp687)->f1;
goto _LL690;} else{ goto _LL691;} _LL691: goto _LL692; _LL690: if( Cyc_CfFlowInfo_initlevel(
_temp693,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp683))->tl))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp695[ 0u]={}; Cyc_Tcutil_terr( _temp377->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp695, sizeof( void*), 0u));});} goto
_LL688; _LL692: goto _LL688; _LL688:;}{ void* _temp696= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp379->topt))->v); _LL698: if((
unsigned int) _temp696 >  4u?*(( int*) _temp696) ==  Cyc_Absyn_TupleType: 0){
goto _LL699;} else{ goto _LL700;} _LL700: goto _LL701; _LL699: { void* _temp702=(
void*)(( struct Cyc_List_List*) _check_null( _temp683))->hd; struct Cyc_Dict_Dict*
_temp708; _LL704: if(( unsigned int) _temp702 >  3u?*(( int*) _temp702) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL709: _temp708=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp702)->f1;
goto _LL705;} else{ goto _LL706;} _LL706: goto _LL707; _LL705: return({ struct
_tuple0 _temp710; _temp710.f1= _temp685; _temp710.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp708, Cyc_Absyn_fieldname(( int)
Cyc_Evexp_eval_const_uint_exp( _temp377))); _temp710;}); _LL707:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp711=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp711[ 0]=({ struct
Cyc_Core_Impossible_struct _temp712; _temp712.tag= Cyc_Core_Impossible; _temp712.f1=
_tag_arr("anal_Rexp: Subscript", sizeof( unsigned char), 21u); _temp712;});
_temp711;})); _LL703:;} _LL701: goto _LL697; _LL697:;} return Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp685, _temp379,( void*)(( struct Cyc_List_List*) _check_null(
_temp683))->hd);} _LL261: _temp383= _temp381; goto _LL263; _LL263: { struct Cyc_List_List*
_temp715; void* _temp717; struct _tuple4 _temp713= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp383); _LL718: _temp717= _temp713.f1; goto _LL716; _LL716:
_temp715= _temp713.f2; goto _LL714; _LL714: { struct Cyc_Dict_Dict* aggrdict=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int i= 0; for( 0; _temp715 !=  0;(
_temp715= _temp715->tl, ++ i)){ aggrdict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict, Cyc_Absyn_fieldname(
i),( void*) _temp715->hd);}} return({ struct _tuple0 _temp719; _temp719.f1=
_temp717; _temp719.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp720=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp720[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp721; _temp721.tag= Cyc_CfFlowInfo_Aggregate; _temp721.f1= aggrdict;
_temp721;}); _temp720;}); _temp719;});}} _LL265: _temp387= _temp385; goto _LL267;
_LL267: { struct Cyc_List_List* _temp724; void* _temp726; struct _tuple4
_temp722= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp387)); _LL727:
_temp726= _temp722.f1; goto _LL725; _LL725: _temp724= _temp722.f2; goto _LL723;
_LL723: { struct Cyc_Dict_Dict* aggrdict=(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{
int i= 0; for( 0; _temp724 !=  0;((( _temp724= _temp724->tl, _temp387= _temp387->tl)),
++ i)){ struct Cyc_List_List* ds=(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp387))->hd)).f1; for( 0; ds !=  0; ds= ds->tl){ void* _temp728=(
void*) ds->hd; struct _tagged_arr* _temp734; _LL730: if(*(( int*) _temp728) == 
Cyc_Absyn_ArrayElement){ goto _LL731;} else{ goto _LL732;} _LL732: if(*(( int*)
_temp728) ==  Cyc_Absyn_FieldName){ _LL735: _temp734=(( struct Cyc_Absyn_FieldName_struct*)
_temp728)->f1; goto _LL733;} else{ goto _LL729;} _LL731:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp736=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp736[ 0]=({ struct
Cyc_Core_Impossible_struct _temp737; _temp737.tag= Cyc_Core_Impossible; _temp737.f1=
_tag_arr("anal_Rexp:Struct_e", sizeof( unsigned char), 19u); _temp737;});
_temp736;})); _LL733: aggrdict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict, _temp734,( void*)
_temp724->hd); _LL729:;}}} return({ struct _tuple0 _temp738; _temp738.f1=
_temp726; _temp738.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp739=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp739[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp740; _temp740.tag= Cyc_CfFlowInfo_Aggregate; _temp740.f1= aggrdict;
_temp740;}); _temp739;}); _temp738;});}} _LL269: { struct Cyc_List_List*
_temp741=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple5*)) Cyc_Core_snd, _temp389); struct Cyc_List_List* _temp744; void*
_temp746; struct _tuple4 _temp742= Cyc_NewControlFlow_anal_unordered_Rexps( env,
inflow, _temp741); _LL747: _temp746= _temp742.f1; goto _LL745; _LL745: _temp744=
_temp742.f2; goto _LL743; _LL743: for( 0; _temp744 !=  0;( _temp744= _temp744->tl,
_temp741= _temp741->tl)){ _temp746= Cyc_NewControlFlow_use_Rval( env,(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp741))->hd)->loc,
_temp746,( void*) _temp744->hd);} return({ struct _tuple0 _temp748; _temp748.f1=
_temp746; _temp748.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp748;});} _LL271: {
void* _temp751; void* _temp753; struct _tuple0 _temp749= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp393); _LL754: _temp753= _temp749.f1; goto _LL752; _LL752:
_temp751= _temp749.f2; goto _LL750; _LL750: { void* _temp755= _temp753; struct
Cyc_Dict_Dict* _temp761; _LL757: if( _temp755 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL758;} else{ goto _LL759;} _LL759: if(( unsigned int) _temp755 >  1u?*((
int*) _temp755) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL762: _temp761=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp755)->f1; goto _LL760;} else{ goto
_LL756;} _LL758: return({ struct _tuple0 _temp763; _temp763.f1= _temp753;
_temp763.f2= Cyc_CfFlowInfo_unknown_all; _temp763;}); _LL760: if( Cyc_CfFlowInfo_initlevel(
_temp761, _temp751) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp764[ 0u]={};
Cyc_Tcutil_terr( _temp393->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp764, sizeof( void*), 0u));});}{
void* _temp765= _temp751; _LL767: if( _temp765 == ( void*) Cyc_CfFlowInfo_Zero){
goto _LL768;} else{ goto _LL769;} _LL769: if( _temp765 == ( void*) Cyc_CfFlowInfo_NotZeroThis){
goto _LL770;} else{ goto _LL771;} _LL771: if( _temp765 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL772;} else{ goto _LL773;} _LL773: if(( unsigned int) _temp765 >  3u?*((
int*) _temp765) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL774;} else{ goto
_LL775;} _LL775: goto _LL776; _LL768: return({ struct _tuple0 _temp777; _temp777.f1=
_temp753; _temp777.f2= Cyc_CfFlowInfo_unknown_all; _temp777;}); _LL770: goto
_LL772; _LL772: goto _LL774; _LL774: { struct Cyc_List_List _temp778=({ struct
Cyc_List_List _temp796; _temp796.hd=( void*) _temp395; _temp796.tl= 0; _temp796;});
_temp753= Cyc_NewControlFlow_add_vars( _temp753,( struct Cyc_List_List*)&
_temp778, Cyc_CfFlowInfo_unknown_all);{ void* _temp781; void* _temp783; struct
_tuple0 _temp779= Cyc_NewControlFlow_anal_Rexp( env, _temp753, _temp391); _LL784:
_temp783= _temp779.f1; goto _LL782; _LL782: _temp781= _temp779.f2; goto _LL780;
_LL780:{ void* _temp785= _temp783; struct Cyc_Dict_Dict* _temp791; _LL787: if(
_temp785 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL788;} else{ goto _LL789;}
_LL789: if(( unsigned int) _temp785 >  1u?*(( int*) _temp785) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL792: _temp791=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp785)->f1;
goto _LL790;} else{ goto _LL786;} _LL788: return({ struct _tuple0 _temp793;
_temp793.f1= _temp783; _temp793.f2= Cyc_CfFlowInfo_unknown_all; _temp793;});
_LL790: if( Cyc_CfFlowInfo_initlevel( _temp791, _temp781) != ( void*) Cyc_CfFlowInfo_AllIL){({
void* _temp794[ 0u]={}; Cyc_Tcutil_terr( _temp393->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp794, sizeof( void*), 0u));});
return({ struct _tuple0 _temp795; _temp795.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp795.f2= Cyc_CfFlowInfo_unknown_all; _temp795;});} _LL786:;} _temp753=
_temp783; goto _LL776;}} _LL776: while( 1) { struct Cyc_List_List _temp797=({
struct Cyc_List_List _temp815; _temp815.hd=( void*) _temp395; _temp815.tl= 0;
_temp815;}); _temp753= Cyc_NewControlFlow_add_vars( _temp753,( struct Cyc_List_List*)&
_temp797, Cyc_CfFlowInfo_unknown_all);{ void* _temp800; void* _temp802; struct
_tuple0 _temp798= Cyc_NewControlFlow_anal_Rexp( env, _temp753, _temp391); _LL803:
_temp802= _temp798.f1; goto _LL801; _LL801: _temp800= _temp798.f2; goto _LL799;
_LL799:{ void* _temp804= _temp802; struct Cyc_Dict_Dict* _temp810; _LL806: if(
_temp804 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL807;} else{ goto _LL808;}
_LL808: if(( unsigned int) _temp804 >  1u?*(( int*) _temp804) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL811: _temp810=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp804)->f1;
goto _LL809;} else{ goto _LL805;} _LL807: goto _LL805; _LL809: if( Cyc_CfFlowInfo_initlevel(
_temp810, _temp800) != ( void*) Cyc_CfFlowInfo_AllIL){({ void* _temp812[ 0u]={};
Cyc_Tcutil_terr( _temp393->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp812, sizeof( void*), 0u));});
return({ struct _tuple0 _temp813; _temp813.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp813.f2= Cyc_CfFlowInfo_unknown_all; _temp813;});} _LL805:;}{ void* _temp814=
Cyc_CfFlowInfo_join_flow( env->all_changed, _temp753, _temp802); if( Cyc_CfFlowInfo_flow_lessthan_approx(
_temp814, _temp753)){ break;} _temp753= _temp814;}}} return({ struct _tuple0
_temp816; _temp816.f1= _temp753; _temp816.f2= Cyc_CfFlowInfo_unknown_all;
_temp816;}); _LL766:;} _LL756:;}} _LL273: while( 1) { inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, inflow, _temp397))->flow;{ void* _temp817=( void*) _temp397->r; struct Cyc_Absyn_Stmt*
_temp827; struct Cyc_Absyn_Stmt* _temp829; struct Cyc_Absyn_Stmt* _temp831;
struct Cyc_Absyn_Decl* _temp833; struct Cyc_Absyn_Exp* _temp835; _LL819: if((
unsigned int) _temp817 >  1u?*(( int*) _temp817) ==  Cyc_Absyn_Seq_s: 0){ _LL830:
_temp829=(( struct Cyc_Absyn_Seq_s_struct*) _temp817)->f1; goto _LL828; _LL828:
_temp827=(( struct Cyc_Absyn_Seq_s_struct*) _temp817)->f2; goto _LL820;} else{
goto _LL821;} _LL821: if(( unsigned int) _temp817 >  1u?*(( int*) _temp817) == 
Cyc_Absyn_Decl_s: 0){ _LL834: _temp833=(( struct Cyc_Absyn_Decl_s_struct*)
_temp817)->f1; goto _LL832; _LL832: _temp831=(( struct Cyc_Absyn_Decl_s_struct*)
_temp817)->f2; goto _LL822;} else{ goto _LL823;} _LL823: if(( unsigned int)
_temp817 >  1u?*(( int*) _temp817) ==  Cyc_Absyn_Exp_s: 0){ _LL836: _temp835=((
struct Cyc_Absyn_Exp_s_struct*) _temp817)->f1; goto _LL824;} else{ goto _LL825;}
_LL825: goto _LL826; _LL820: inflow= Cyc_NewControlFlow_anal_stmt( env, inflow,
_temp829); _temp397= _temp827; goto _LL818; _LL822: inflow= Cyc_NewControlFlow_anal_decl(
env, inflow, _temp833); _temp397= _temp831; goto _LL818; _LL824: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp835); _LL826:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp837=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp837[ 0]=({ struct Cyc_Core_Impossible_struct _temp838; _temp838.tag= Cyc_Core_Impossible;
_temp838.f1= _tag_arr("analyze_Rexp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp838;}); _temp837;})); _LL818:;}} _LL275: goto _LL277; _LL277: goto
_LL279; _LL279: goto _LL281; _LL281: goto _LL283; _LL283: goto _LL285; _LL285:
goto _LL287; _LL287:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp839=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp839[ 0]=({ struct Cyc_Core_Impossible_struct _temp840; _temp840.tag= Cyc_Core_Impossible;
_temp840.f1= _tag_arr("anal_Rexp, unexpected exp form", sizeof( unsigned char),
31u); _temp840;}); _temp839;})); _LL191:;}} static struct _tuple0 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* f, struct Cyc_Absyn_Exp*
e, void* r, struct Cyc_List_List* flds){ void* _temp841= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo
_temp847; struct Cyc_Absyn_Conref* _temp849; void* _temp851; _LL843: if((
unsigned int) _temp841 >  4u?*(( int*) _temp841) ==  Cyc_Absyn_PointerType: 0){
_LL848: _temp847=(( struct Cyc_Absyn_PointerType_struct*) _temp841)->f1; _LL852:
_temp851=( void*) _temp847.elt_typ; goto _LL850; _LL850: _temp849= _temp847.bounds;
goto _LL844;} else{ goto _LL845;} _LL845: goto _LL846; _LL844: { void* _temp853=
f; struct Cyc_Dict_Dict* _temp859; _LL855: if( _temp853 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL856;} else{ goto _LL857;} _LL857: if(( unsigned int) _temp853 >  1u?*((
int*) _temp853) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL860: _temp859=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp853)->f1; goto _LL858;} else{ goto
_LL854;} _LL856: return({ struct _tuple0 _temp861; _temp861.f1= f; _temp861.f2=(
void*) Cyc_CfFlowInfo_UnknownL; _temp861;}); _LL858: if( Cyc_Tcutil_is_bound_one(
_temp849)){ void* _temp862= r; struct Cyc_CfFlowInfo_Place* _temp876; struct Cyc_CfFlowInfo_Place
_temp878; struct Cyc_List_List* _temp879; void* _temp881; void* _temp883; _LL864:
if( _temp862 == ( void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL865;} else{ goto
_LL866;} _LL866: if( _temp862 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto
_LL867;} else{ goto _LL868;} _LL868: if(( unsigned int) _temp862 >  3u?*(( int*)
_temp862) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL877: _temp876=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp862)->f1; _temp878=* _temp876; _LL882: _temp881=( void*) _temp878.root;
goto _LL880; _LL880: _temp879= _temp878.fields; goto _LL869;} else{ goto _LL870;}
_LL870: if( _temp862 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL871;} else{ goto
_LL872;} _LL872: if(( unsigned int) _temp862 >  3u?*(( int*) _temp862) ==  Cyc_CfFlowInfo_UnknownR:
0){ _LL884: _temp883=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp862)->f1;
goto _LL873;} else{ goto _LL874;} _LL874: goto _LL875; _LL865: goto _LL867;
_LL867:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero)); goto
_LL863; _LL869:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero));
return({ struct _tuple0 _temp885; _temp885.f1= f; _temp885.f2=( void*)({ struct
Cyc_CfFlowInfo_PlaceL_struct* _temp886=( struct Cyc_CfFlowInfo_PlaceL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct)); _temp886[ 0]=({ struct
Cyc_CfFlowInfo_PlaceL_struct _temp887; _temp887.tag= Cyc_CfFlowInfo_PlaceL;
_temp887.f1=({ struct Cyc_CfFlowInfo_Place* _temp888=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp888->root=( void*)
_temp881; _temp888->fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp879, flds); _temp888;});
_temp887;}); _temp886;}); _temp885;}); _LL871:( void*)( e->annot=( void*)(( void*)
Cyc_CfFlowInfo_IsZero)); return({ struct _tuple0 _temp889; _temp889.f1=( void*)
Cyc_CfFlowInfo_BottomFL; _temp889.f2=( void*) Cyc_CfFlowInfo_UnknownL; _temp889;});
_LL873: f= Cyc_NewControlFlow_notzero( env, inflow, f, e, _temp883); goto _LL875;
_LL875:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_UnknownZ)); _LL863:;}
else{( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_UnknownZ));} if( Cyc_CfFlowInfo_initlevel(
_temp859, r) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp890[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("dereference of possibly uninitialized pointer", sizeof(
unsigned char), 46u), _tag_arr( _temp890, sizeof( void*), 0u));});} return({
struct _tuple0 _temp891; _temp891.f1= f; _temp891.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp891;}); _LL854:;} _LL846:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp892=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp892[ 0]=({ struct Cyc_Core_Impossible_struct _temp893; _temp893.tag= Cyc_Core_Impossible;
_temp893.f1= _tag_arr("left deref of non-pointer-type", sizeof( unsigned char),
31u); _temp893;}); _temp892;})); _LL842:;} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* flds){ struct Cyc_Dict_Dict* d;{ void* _temp894= inflow;
struct Cyc_Dict_Dict* _temp900; _LL896: if( _temp894 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL897;} else{ goto _LL898;} _LL898: if(( unsigned int) _temp894 >  1u?*((
int*) _temp894) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL901: _temp900=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp894)->f1; goto _LL899;} else{ goto
_LL895;} _LL897: return({ struct _tuple0 _temp902; _temp902.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp902.f2=( void*) Cyc_CfFlowInfo_UnknownL; _temp902;}); _LL899: d= _temp900;
_LL895:;}{ void* _temp903=( void*) e->r; struct Cyc_Absyn_Exp* _temp929; struct
Cyc_Absyn_Exp* _temp931; struct Cyc_Absyn_Exp* _temp933; void* _temp935; struct
Cyc_Absyn_Vardecl* _temp937; void* _temp939; struct Cyc_Absyn_Vardecl* _temp941;
void* _temp943; struct Cyc_Absyn_Vardecl* _temp945; void* _temp947; struct Cyc_Absyn_Vardecl*
_temp949; struct _tagged_arr* _temp951; struct Cyc_Absyn_Exp* _temp953; struct
Cyc_Absyn_Exp* _temp955; struct Cyc_Absyn_Exp* _temp957; struct Cyc_Absyn_Exp*
_temp959; struct _tagged_arr* _temp961; struct Cyc_Absyn_Exp* _temp963; _LL905:
if(*(( int*) _temp903) ==  Cyc_Absyn_Cast_e){ _LL930: _temp929=(( struct Cyc_Absyn_Cast_e_struct*)
_temp903)->f2; goto _LL906;} else{ goto _LL907;} _LL907: if(*(( int*) _temp903)
==  Cyc_Absyn_NoInstantiate_e){ _LL932: _temp931=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp903)->f1; goto _LL908;} else{ goto _LL909;} _LL909: if(*(( int*) _temp903)
==  Cyc_Absyn_Instantiate_e){ _LL934: _temp933=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp903)->f1; goto _LL910;} else{ goto _LL911;} _LL911: if(*(( int*) _temp903)
==  Cyc_Absyn_Var_e){ _LL936: _temp935=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp903)->f2; if(( unsigned int) _temp935 >  1u?*(( int*) _temp935) ==  Cyc_Absyn_Param_b:
0){ _LL938: _temp937=(( struct Cyc_Absyn_Param_b_struct*) _temp935)->f1; goto
_LL912;} else{ goto _LL913;}} else{ goto _LL913;} _LL913: if(*(( int*) _temp903)
==  Cyc_Absyn_Var_e){ _LL940: _temp939=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp903)->f2; if(( unsigned int) _temp939 >  1u?*(( int*) _temp939) ==  Cyc_Absyn_Local_b:
0){ _LL942: _temp941=(( struct Cyc_Absyn_Local_b_struct*) _temp939)->f1; goto
_LL914;} else{ goto _LL915;}} else{ goto _LL915;} _LL915: if(*(( int*) _temp903)
==  Cyc_Absyn_Var_e){ _LL944: _temp943=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp903)->f2; if(( unsigned int) _temp943 >  1u?*(( int*) _temp943) ==  Cyc_Absyn_Pat_b:
0){ _LL946: _temp945=(( struct Cyc_Absyn_Pat_b_struct*) _temp943)->f1; goto
_LL916;} else{ goto _LL917;}} else{ goto _LL917;} _LL917: if(*(( int*) _temp903)
==  Cyc_Absyn_Var_e){ _LL948: _temp947=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp903)->f2; if(( unsigned int) _temp947 >  1u?*(( int*) _temp947) ==  Cyc_Absyn_Global_b:
0){ _LL950: _temp949=(( struct Cyc_Absyn_Global_b_struct*) _temp947)->f1; goto
_LL918;} else{ goto _LL919;}} else{ goto _LL919;} _LL919: if(*(( int*) _temp903)
==  Cyc_Absyn_StructArrow_e){ _LL954: _temp953=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp903)->f1; goto _LL952; _LL952: _temp951=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp903)->f2; goto _LL920;} else{ goto _LL921;} _LL921: if(*(( int*) _temp903)
==  Cyc_Absyn_Deref_e){ _LL956: _temp955=(( struct Cyc_Absyn_Deref_e_struct*)
_temp903)->f1; goto _LL922;} else{ goto _LL923;} _LL923: if(*(( int*) _temp903)
==  Cyc_Absyn_Subscript_e){ _LL960: _temp959=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp903)->f1; goto _LL958; _LL958: _temp957=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp903)->f2; goto _LL924;} else{ goto _LL925;} _LL925: if(*(( int*) _temp903)
==  Cyc_Absyn_StructMember_e){ _LL964: _temp963=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp903)->f1; goto _LL962; _LL962: _temp961=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp903)->f2; goto _LL926;} else{ goto _LL927;} _LL927: goto _LL928; _LL906:
_temp931= _temp929; goto _LL908; _LL908: _temp933= _temp931; goto _LL910; _LL910:
return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, _temp933, flds); _LL912:
_temp941= _temp937; goto _LL914; _LL914: _temp945= _temp941; goto _LL916; _LL916:
return({ struct _tuple0 _temp965; _temp965.f1= inflow; _temp965.f2=( void*)({
struct Cyc_CfFlowInfo_PlaceL_struct* _temp966=( struct Cyc_CfFlowInfo_PlaceL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct)); _temp966[ 0]=({ struct
Cyc_CfFlowInfo_PlaceL_struct _temp967; _temp967.tag= Cyc_CfFlowInfo_PlaceL;
_temp967.f1=({ struct Cyc_CfFlowInfo_Place* _temp968=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp968->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp969=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp969[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp970; _temp970.tag= Cyc_CfFlowInfo_VarRoot;
_temp970.f1= _temp945; _temp970;}); _temp969;})); _temp968->fields= flds;
_temp968;}); _temp967;}); _temp966;}); _temp965;}); _LL918: return({ struct
_tuple0 _temp971; _temp971.f1= inflow; _temp971.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp971;}); _LL920:{ void* _temp972= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp953->topt))->v); _LL974: if(( unsigned int) _temp972 >  4u?*((
int*) _temp972) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL975;} else{ goto _LL976;}
_LL976: if(( unsigned int) _temp972 >  4u?*(( int*) _temp972) ==  Cyc_Absyn_UnionType:
0){ goto _LL977;} else{ goto _LL978;} _LL978: goto _LL979; _LL975: goto _LL977;
_LL977: goto _LL973; _LL979: flds=({ struct Cyc_List_List* _temp980=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp980->hd=( void*) _temp951;
_temp980->tl= flds; _temp980;}); _LL973:;} _temp955= _temp953; goto _LL922;
_LL922: { void* _temp983; void* _temp985; struct _tuple0 _temp981= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp955); _LL986: _temp985= _temp981.f1; goto _LL984; _LL984:
_temp983= _temp981.f2; goto _LL982; _LL982: return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp985, _temp955, _temp983, flds);} _LL924:{ void* _temp987= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp959->topt))->v); _LL989: if((
unsigned int) _temp987 >  4u?*(( int*) _temp987) ==  Cyc_Absyn_TupleType: 0){
goto _LL990;} else{ goto _LL991;} _LL991: goto _LL992; _LL990: { struct
_tagged_arr* _temp993= Cyc_Absyn_fieldname(( int) Cyc_Evexp_eval_const_uint_exp(
_temp957)); return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, _temp959,({
struct Cyc_List_List* _temp994=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp994->hd=( void*) _temp993; _temp994->tl= flds;
_temp994;}));} _LL992: goto _LL988; _LL988:;}{ struct Cyc_List_List* _temp998;
void* _temp1000; struct _tuple4 _temp996= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp995[ 2u]; _temp995[ 1u]= _temp957;
_temp995[ 0u]= _temp959;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp995, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL1001: _temp1000=
_temp996.f1; goto _LL999; _LL999: _temp998= _temp996.f2; goto _LL997; _LL997:{
void* _temp1002= _temp1000; struct Cyc_Dict_Dict* _temp1008; _LL1004: if((
unsigned int) _temp1002 >  1u?*(( int*) _temp1002) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL1009: _temp1008=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1002)->f1;
goto _LL1005;} else{ goto _LL1006;} _LL1006: goto _LL1007; _LL1005: if( Cyc_CfFlowInfo_initlevel(
_temp1008,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp998))->tl))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp1010[ 0u]={}; Cyc_Tcutil_terr( _temp957->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1010, sizeof( void*), 0u));});}
goto _LL1003; _LL1007: goto _LL1003; _LL1003:;} return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp1000, _temp959,( void*)(( struct Cyc_List_List*) _check_null(
_temp998))->hd, flds);} _LL926:{ void* _temp1011= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp963->topt))->v); _LL1013: if((
unsigned int) _temp1011 >  4u?*(( int*) _temp1011) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL1014;} else{ goto _LL1015;} _LL1015: if(( unsigned int) _temp1011 > 
4u?*(( int*) _temp1011) ==  Cyc_Absyn_UnionType: 0){ goto _LL1016;} else{ goto
_LL1017;} _LL1017: goto _LL1018; _LL1014: goto _LL1016; _LL1016: return({ struct
_tuple0 _temp1019; _temp1019.f1= inflow; _temp1019.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1019;}); _LL1018: flds=({ struct Cyc_List_List* _temp1020=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1020->hd=( void*) _temp961;
_temp1020->tl= flds; _temp1020;}); _LL1012:;} return Cyc_NewControlFlow_anal_Lexp_rec(
env, inflow, _temp963, flds); _LL928: return({ struct _tuple0 _temp1021;
_temp1021.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp1021.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1021;}); _LL904:;}} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e){
return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, e, 0);} static struct
_tuple0 Cyc_NewControlFlow_anal_test( struct Cyc_NewControlFlow_AnalEnv* env,
void* inflow, struct Cyc_Absyn_Exp* e){{ void* _temp1022=( void*) e->r; struct
Cyc_Absyn_Exp* _temp1034; struct Cyc_Absyn_Exp* _temp1036; struct Cyc_Absyn_Exp*
_temp1038; struct Cyc_List_List* _temp1040; struct Cyc_List_List _temp1042;
struct Cyc_List_List* _temp1043; struct Cyc_Absyn_Exp* _temp1045; void*
_temp1047; struct Cyc_List_List* _temp1049; void* _temp1051; struct Cyc_List_List*
_temp1053; void* _temp1055; _LL1024: if(*(( int*) _temp1022) ==  Cyc_Absyn_Conditional_e){
_LL1039: _temp1038=(( struct Cyc_Absyn_Conditional_e_struct*) _temp1022)->f1;
goto _LL1037; _LL1037: _temp1036=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1022)->f2; goto _LL1035; _LL1035: _temp1034=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1022)->f3; goto _LL1025;} else{ goto _LL1026;} _LL1026: if(*(( int*)
_temp1022) ==  Cyc_Absyn_Primop_e){ _LL1048: _temp1047=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1022)->f1; if( _temp1047 == ( void*) Cyc_Absyn_Not){ goto _LL1041;} else{
goto _LL1028;} _LL1041: _temp1040=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1022)->f2; if( _temp1040 ==  0){ goto _LL1028;} else{ _temp1042=* _temp1040;
_LL1046: _temp1045=( struct Cyc_Absyn_Exp*) _temp1042.hd; goto _LL1044; _LL1044:
_temp1043= _temp1042.tl; if( _temp1043 ==  0){ goto _LL1027;} else{ goto _LL1028;}}}
else{ goto _LL1028;} _LL1028: if(*(( int*) _temp1022) ==  Cyc_Absyn_Primop_e){
_LL1052: _temp1051=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1022)->f1;
if( _temp1051 == ( void*) Cyc_Absyn_Eq){ goto _LL1050;} else{ goto _LL1030;}
_LL1050: _temp1049=(( struct Cyc_Absyn_Primop_e_struct*) _temp1022)->f2; goto
_LL1029;} else{ goto _LL1030;} _LL1030: if(*(( int*) _temp1022) ==  Cyc_Absyn_Primop_e){
_LL1056: _temp1055=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1022)->f1;
if( _temp1055 == ( void*) Cyc_Absyn_Neq){ goto _LL1054;} else{ goto _LL1032;}
_LL1054: _temp1053=(( struct Cyc_Absyn_Primop_e_struct*) _temp1022)->f2; goto
_LL1031;} else{ goto _LL1032;} _LL1032: goto _LL1033; _LL1025: { void* _temp1059;
void* _temp1061; struct _tuple0 _temp1057= Cyc_NewControlFlow_anal_test( env,
inflow, _temp1038); _LL1062: _temp1061= _temp1057.f1; goto _LL1060; _LL1060:
_temp1059= _temp1057.f2; goto _LL1058; _LL1058: { void* _temp1065; void*
_temp1067; struct _tuple0 _temp1063= Cyc_NewControlFlow_anal_test( env,
_temp1061, _temp1036); _LL1068: _temp1067= _temp1063.f1; goto _LL1066; _LL1066:
_temp1065= _temp1063.f2; goto _LL1064; _LL1064: { void* _temp1071; void*
_temp1073; struct _tuple0 _temp1069= Cyc_NewControlFlow_anal_test( env,
_temp1059, _temp1034); _LL1074: _temp1073= _temp1069.f1; goto _LL1072; _LL1072:
_temp1071= _temp1069.f2; goto _LL1070; _LL1070: return({ struct _tuple0
_temp1075; _temp1075.f1= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp1067,
_temp1073); _temp1075.f2= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp1065,
_temp1071); _temp1075;});}}} _LL1027: { void* _temp1078; void* _temp1080; struct
_tuple0 _temp1076= Cyc_NewControlFlow_anal_test( env, inflow, _temp1045);
_LL1081: _temp1080= _temp1076.f1; goto _LL1079; _LL1079: _temp1078= _temp1076.f2;
goto _LL1077; _LL1077: return({ struct _tuple0 _temp1082; _temp1082.f1=
_temp1078; _temp1082.f2= _temp1080; _temp1082;});} _LL1029: { struct Cyc_List_List*
_temp1085; void* _temp1087; struct _tuple4 _temp1083= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp1049); _LL1088: _temp1087= _temp1083.f1; goto _LL1086; _LL1086:
_temp1085= _temp1083.f2; goto _LL1084; _LL1084: { void* _temp1089= _temp1087;
struct Cyc_Dict_Dict* _temp1095; _LL1091: if( _temp1089 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1092;} else{ goto _LL1093;} _LL1093: if(( unsigned int) _temp1089 >  1u?*((
int*) _temp1089) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1096: _temp1095=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1089)->f1; goto _LL1094;} else{
goto _LL1090;} _LL1092: return({ struct _tuple0 _temp1097; _temp1097.f1=
_temp1087; _temp1097.f2= _temp1087; _temp1097;}); _LL1094: { void* _temp1098=(
void*)(( struct Cyc_List_List*) _check_null( _temp1085))->hd; void* _temp1099=(
void*)(( struct Cyc_List_List*) _check_null( _temp1085->tl))->hd; if( Cyc_CfFlowInfo_initlevel(
_temp1095, _temp1098) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1100[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1049))->hd)->loc, _tag_arr("expression may not be initialized", sizeof(
unsigned char), 34u), _tag_arr( _temp1100, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1095, _temp1099) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1101[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1049))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1101, sizeof( void*), 0u));});}{
struct _tuple0 _temp1103=({ struct _tuple0 _temp1102; _temp1102.f1= _temp1098;
_temp1102.f2= _temp1099; _temp1102;}); void* _temp1111; void* _temp1113; void*
_temp1115; void* _temp1117; void* _temp1119; void* _temp1121; _LL1105: _LL1114:
_temp1113= _temp1103.f1; if(( unsigned int) _temp1113 >  3u?*(( int*) _temp1113)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1116: _temp1115=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1113)->f1; goto _LL1112;} else{ goto _LL1107;} _LL1112: _temp1111=
_temp1103.f2; if( _temp1111 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1106;}
else{ goto _LL1107;} _LL1107: _LL1122: _temp1121= _temp1103.f1; if( _temp1121 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1118;} else{ goto _LL1109;} _LL1118:
_temp1117= _temp1103.f2; if(( unsigned int) _temp1117 >  3u?*(( int*) _temp1117)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1120: _temp1119=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1117)->f1; goto _LL1108;} else{ goto _LL1109;} _LL1109: goto _LL1110;
_LL1106: { void* _temp1125; void* _temp1127; struct _tuple0 _temp1123= Cyc_NewControlFlow_splitzero(
env, _temp1087, _temp1087,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1049))->hd, _temp1115); _LL1128: _temp1127= _temp1123.f1; goto
_LL1126; _LL1126: _temp1125= _temp1123.f2; goto _LL1124; _LL1124: return({
struct _tuple0 _temp1129; _temp1129.f1= _temp1125; _temp1129.f2= _temp1127;
_temp1129;});} _LL1108: { void* _temp1132; void* _temp1134; struct _tuple0
_temp1130= Cyc_NewControlFlow_splitzero( env, _temp1087, _temp1087,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1049))->tl))->hd, _temp1119); _LL1135: _temp1134= _temp1130.f1; goto
_LL1133; _LL1133: _temp1132= _temp1130.f2; goto _LL1131; _LL1131: return({
struct _tuple0 _temp1136; _temp1136.f1= _temp1132; _temp1136.f2= _temp1134;
_temp1136;});} _LL1110: return({ struct _tuple0 _temp1137; _temp1137.f1=
_temp1087; _temp1137.f2= _temp1087; _temp1137;}); _LL1104:;}} _LL1090:;}}
_LL1031: { struct Cyc_List_List* _temp1140; void* _temp1142; struct _tuple4
_temp1138= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1053);
_LL1143: _temp1142= _temp1138.f1; goto _LL1141; _LL1141: _temp1140= _temp1138.f2;
goto _LL1139; _LL1139: { void* _temp1144= _temp1142; struct Cyc_Dict_Dict*
_temp1150; _LL1146: if( _temp1144 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1147;} else{ goto _LL1148;} _LL1148: if(( unsigned int) _temp1144 >  1u?*((
int*) _temp1144) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1151: _temp1150=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1144)->f1; goto _LL1149;} else{
goto _LL1145;} _LL1147: return({ struct _tuple0 _temp1152; _temp1152.f1=
_temp1142; _temp1152.f2= _temp1142; _temp1152;}); _LL1149: { void* _temp1153=(
void*)(( struct Cyc_List_List*) _check_null( _temp1140))->hd; void* _temp1154=(
void*)(( struct Cyc_List_List*) _check_null( _temp1140->tl))->hd; if( Cyc_CfFlowInfo_initlevel(
_temp1150, _temp1153) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1155[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1053))->hd)->loc, _tag_arr("expression may not be initialized", sizeof(
unsigned char), 34u), _tag_arr( _temp1155, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1150, _temp1154) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1156[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1053))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1156, sizeof( void*), 0u));});}{
struct _tuple0 _temp1158=({ struct _tuple0 _temp1157; _temp1157.f1= _temp1153;
_temp1157.f2= _temp1154; _temp1157;}); void* _temp1166; void* _temp1168; void*
_temp1170; void* _temp1172; void* _temp1174; void* _temp1176; _LL1160: _LL1169:
_temp1168= _temp1158.f1; if(( unsigned int) _temp1168 >  3u?*(( int*) _temp1168)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1171: _temp1170=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1168)->f1; goto _LL1167;} else{ goto _LL1162;} _LL1167: _temp1166=
_temp1158.f2; if( _temp1166 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1161;}
else{ goto _LL1162;} _LL1162: _LL1177: _temp1176= _temp1158.f1; if( _temp1176 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1173;} else{ goto _LL1164;} _LL1173:
_temp1172= _temp1158.f2; if(( unsigned int) _temp1172 >  3u?*(( int*) _temp1172)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1175: _temp1174=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1172)->f1; goto _LL1163;} else{ goto _LL1164;} _LL1164: goto _LL1165;
_LL1161: return Cyc_NewControlFlow_splitzero( env, _temp1142, _temp1142,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1053))->hd, _temp1170);
_LL1163: return Cyc_NewControlFlow_splitzero( env, _temp1142, _temp1142,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1053))->tl))->hd, _temp1174); _LL1165: return({ struct _tuple0
_temp1178; _temp1178.f1= _temp1142; _temp1178.f2= _temp1142; _temp1178;});
_LL1159:;}} _LL1145:;}} _LL1033: goto _LL1023; _LL1023:;}{ void* _temp1181; void*
_temp1183; struct _tuple0 _temp1179= Cyc_NewControlFlow_anal_Rexp( env, inflow,
e); _LL1184: _temp1183= _temp1179.f1; goto _LL1182; _LL1182: _temp1181=
_temp1179.f2; goto _LL1180; _LL1180: { void* _temp1185= _temp1183; struct Cyc_Dict_Dict*
_temp1191; _LL1187: if( _temp1185 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1188;} else{ goto _LL1189;} _LL1189: if(( unsigned int) _temp1185 >  1u?*((
int*) _temp1185) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1192: _temp1191=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1185)->f1; goto _LL1190;} else{
goto _LL1186;} _LL1188: return({ struct _tuple0 _temp1193; _temp1193.f1=
_temp1183; _temp1193.f2= _temp1183; _temp1193;}); _LL1190: { void* _temp1194=
_temp1181; void* _temp1214; void* _temp1216; void* _temp1218; _LL1196: if(
_temp1194 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1197;} else{ goto _LL1198;}
_LL1198: if( _temp1194 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto _LL1199;}
else{ goto _LL1200;} _LL1200: if( _temp1194 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL1201;} else{ goto _LL1202;} _LL1202: if(( unsigned int) _temp1194 >  3u?*((
int*) _temp1194) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL1203;} else{ goto
_LL1204;} _LL1204: if(( unsigned int) _temp1194 >  3u?*(( int*) _temp1194) == 
Cyc_CfFlowInfo_UnknownR: 0){ _LL1215: _temp1214=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1194)->f1; if( _temp1214 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL1205;}
else{ goto _LL1206;}} else{ goto _LL1206;} _LL1206: if(( unsigned int) _temp1194
>  3u?*(( int*) _temp1194) ==  Cyc_CfFlowInfo_Esc: 0){ _LL1217: _temp1216=( void*)((
struct Cyc_CfFlowInfo_Esc_struct*) _temp1194)->f1; if( _temp1216 == ( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL1207;} else{ goto _LL1208;}} else{ goto _LL1208;} _LL1208: if((
unsigned int) _temp1194 >  3u?*(( int*) _temp1194) ==  Cyc_CfFlowInfo_UnknownR:
0){ _LL1219: _temp1218=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1194)->f1; goto _LL1209;} else{ goto _LL1210;} _LL1210: if(( unsigned int)
_temp1194 >  3u?*(( int*) _temp1194) ==  Cyc_CfFlowInfo_Esc: 0){ goto _LL1211;}
else{ goto _LL1212;} _LL1212: if(( unsigned int) _temp1194 >  3u?*(( int*)
_temp1194) ==  Cyc_CfFlowInfo_Aggregate: 0){ goto _LL1213;} else{ goto _LL1195;}
_LL1197: return({ struct _tuple0 _temp1220; _temp1220.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1220.f2= _temp1183; _temp1220;}); _LL1199: goto _LL1201; _LL1201: goto
_LL1203; _LL1203: return({ struct _tuple0 _temp1221; _temp1221.f1= _temp1183;
_temp1221.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1221;}); _LL1205: goto
_LL1207; _LL1207:({ void* _temp1222[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1222, sizeof( void*), 0u));});
return({ struct _tuple0 _temp1223; _temp1223.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1223.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1223;}); _LL1209: return Cyc_NewControlFlow_splitzero(
env, inflow, _temp1183, e, _temp1218); _LL1211: return({ struct _tuple0
_temp1224; _temp1224.f1= _temp1183; _temp1224.f2= _temp1183; _temp1224;});
_LL1213:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1225=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1225[ 0]=({ struct Cyc_Core_Impossible_struct _temp1226; _temp1226.tag= Cyc_Core_Impossible;
_temp1226.f1= _tag_arr("anal_test", sizeof( unsigned char), 10u); _temp1226;});
_temp1225;})); _LL1195:;} _LL1186:;}}} static void* Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* scs){
for( 0; scs !=  0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp1229;
struct Cyc_Absyn_Stmt* _temp1230; struct Cyc_Absyn_Exp* _temp1232; struct Cyc_Core_Opt*
_temp1234; struct Cyc_Absyn_Switch_clause* _temp1227=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp1229=* _temp1227; _LL1235: _temp1234= _temp1229.pat_vars; goto
_LL1233; _LL1233: _temp1232= _temp1229.where_clause; goto _LL1231; _LL1231:
_temp1230= _temp1229.body; goto _LL1228; _LL1228: { void* clause_inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1234))->v,
Cyc_CfFlowInfo_unknown_all); void* clause_outflow; if( _temp1232 !=  0){ void*
_temp1238; void* _temp1240; struct _tuple0 _temp1236= Cyc_NewControlFlow_anal_test(
env, clause_inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1232)); _LL1241:
_temp1240= _temp1236.f1; goto _LL1239; _LL1239: _temp1238= _temp1236.f2; goto
_LL1237; _LL1237: inflow= _temp1238; clause_outflow= Cyc_NewControlFlow_anal_stmt(
env, _temp1240, _temp1230);} else{ clause_outflow= Cyc_NewControlFlow_anal_stmt(
env, clause_inflow, _temp1230);}{ void* _temp1242= clause_outflow; _LL1244: if(
_temp1242 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1245;} else{ goto
_LL1246;} _LL1246: goto _LL1247; _LL1245: goto _LL1243; _LL1247: if( scs->tl == 
0){ return clause_outflow;} else{ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs->tl))->hd)->pat_vars))->v !=  0){({ void* _temp1248[ 0u]={};
Cyc_Tcutil_terr( _temp1230->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1248, sizeof( void*), 0u));});}
else{({ void* _temp1249[ 0u]={}; Cyc_Tcutil_warn( _temp1230->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1249, sizeof( void*), 0u));});} Cyc_NewControlFlow_update_flow(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs->tl))->hd)->body, clause_outflow);} goto _LL1243; _LL1243:;}}} return( void*)
Cyc_CfFlowInfo_BottomFL;} static void* Cyc_NewControlFlow_anal_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1250= Cyc_NewControlFlow_pre_stmt_check( env, inflow, s); inflow=( void*)
_temp1250->flow;{ void* _temp1251=( void*) s->r; struct Cyc_Absyn_Exp* _temp1293;
struct Cyc_Absyn_Exp* _temp1295; struct Cyc_Absyn_Exp* _temp1297; struct Cyc_Absyn_Stmt*
_temp1299; struct Cyc_Absyn_Stmt* _temp1301; struct Cyc_Absyn_Stmt* _temp1303;
struct Cyc_Absyn_Stmt* _temp1305; struct Cyc_Absyn_Exp* _temp1307; struct Cyc_Absyn_Stmt*
_temp1309; struct _tuple3 _temp1311; struct Cyc_Absyn_Stmt* _temp1313; struct
Cyc_Absyn_Exp* _temp1315; struct _tuple3 _temp1317; struct Cyc_Absyn_Stmt*
_temp1319; struct Cyc_Absyn_Exp* _temp1321; struct Cyc_Absyn_Stmt* _temp1323;
struct Cyc_Absyn_Stmt* _temp1325; struct _tuple3 _temp1327; struct Cyc_Absyn_Stmt*
_temp1329; struct Cyc_Absyn_Exp* _temp1331; struct _tuple3 _temp1333; struct Cyc_Absyn_Stmt*
_temp1335; struct Cyc_Absyn_Exp* _temp1337; struct Cyc_Absyn_Exp* _temp1339;
struct Cyc_Absyn_Stmt* _temp1341; struct Cyc_Absyn_Switch_clause** _temp1343;
struct Cyc_Absyn_Switch_clause* _temp1345; struct Cyc_List_List* _temp1346;
struct Cyc_Absyn_Stmt* _temp1348; struct Cyc_Absyn_Stmt* _temp1350; struct Cyc_Absyn_Stmt*
_temp1352; struct Cyc_List_List* _temp1354; struct Cyc_Absyn_Exp* _temp1356;
struct Cyc_List_List* _temp1358; struct Cyc_Absyn_Stmt* _temp1360; struct Cyc_Absyn_Stmt*
_temp1362; struct Cyc_Absyn_Decl* _temp1364; struct Cyc_Absyn_Stmt* _temp1366;
struct Cyc_Absyn_Stmt* _temp1368; struct Cyc_Absyn_Vardecl* _temp1370; struct
Cyc_Absyn_Tvar* _temp1372; _LL1253: if( _temp1251 == ( void*) Cyc_Absyn_Skip_s){
goto _LL1254;} else{ goto _LL1255;} _LL1255: if(( unsigned int) _temp1251 >  1u?*((
int*) _temp1251) ==  Cyc_Absyn_Return_s: 0){ _LL1294: _temp1293=(( struct Cyc_Absyn_Return_s_struct*)
_temp1251)->f1; if( _temp1293 ==  0){ goto _LL1256;} else{ goto _LL1257;}} else{
goto _LL1257;} _LL1257: if(( unsigned int) _temp1251 >  1u?*(( int*) _temp1251)
==  Cyc_Absyn_Return_s: 0){ _LL1296: _temp1295=(( struct Cyc_Absyn_Return_s_struct*)
_temp1251)->f1; goto _LL1258;} else{ goto _LL1259;} _LL1259: if(( unsigned int)
_temp1251 >  1u?*(( int*) _temp1251) ==  Cyc_Absyn_Exp_s: 0){ _LL1298: _temp1297=((
struct Cyc_Absyn_Exp_s_struct*) _temp1251)->f1; goto _LL1260;} else{ goto
_LL1261;} _LL1261: if(( unsigned int) _temp1251 >  1u?*(( int*) _temp1251) == 
Cyc_Absyn_Seq_s: 0){ _LL1302: _temp1301=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1251)->f1; goto _LL1300; _LL1300: _temp1299=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1251)->f2; goto _LL1262;} else{ goto _LL1263;} _LL1263: if(( unsigned int)
_temp1251 >  1u?*(( int*) _temp1251) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1308:
_temp1307=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1251)->f1; goto _LL1306;
_LL1306: _temp1305=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1251)->f2;
goto _LL1304; _LL1304: _temp1303=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1251)->f3; goto _LL1264;} else{ goto _LL1265;} _LL1265: if(( unsigned int)
_temp1251 >  1u?*(( int*) _temp1251) ==  Cyc_Absyn_While_s: 0){ _LL1312:
_temp1311=(( struct Cyc_Absyn_While_s_struct*) _temp1251)->f1; _LL1316:
_temp1315= _temp1311.f1; goto _LL1314; _LL1314: _temp1313= _temp1311.f2; goto
_LL1310; _LL1310: _temp1309=(( struct Cyc_Absyn_While_s_struct*) _temp1251)->f2;
goto _LL1266;} else{ goto _LL1267;} _LL1267: if(( unsigned int) _temp1251 >  1u?*((
int*) _temp1251) ==  Cyc_Absyn_Do_s: 0){ _LL1324: _temp1323=(( struct Cyc_Absyn_Do_s_struct*)
_temp1251)->f1; goto _LL1318; _LL1318: _temp1317=(( struct Cyc_Absyn_Do_s_struct*)
_temp1251)->f2; _LL1322: _temp1321= _temp1317.f1; goto _LL1320; _LL1320:
_temp1319= _temp1317.f2; goto _LL1268;} else{ goto _LL1269;} _LL1269: if((
unsigned int) _temp1251 >  1u?*(( int*) _temp1251) ==  Cyc_Absyn_For_s: 0){
_LL1340: _temp1339=(( struct Cyc_Absyn_For_s_struct*) _temp1251)->f1; goto
_LL1334; _LL1334: _temp1333=(( struct Cyc_Absyn_For_s_struct*) _temp1251)->f2;
_LL1338: _temp1337= _temp1333.f1; goto _LL1336; _LL1336: _temp1335= _temp1333.f2;
goto _LL1328; _LL1328: _temp1327=(( struct Cyc_Absyn_For_s_struct*) _temp1251)->f3;
_LL1332: _temp1331= _temp1327.f1; goto _LL1330; _LL1330: _temp1329= _temp1327.f2;
goto _LL1326; _LL1326: _temp1325=(( struct Cyc_Absyn_For_s_struct*) _temp1251)->f4;
goto _LL1270;} else{ goto _LL1271;} _LL1271: if(( unsigned int) _temp1251 >  1u?*((
int*) _temp1251) ==  Cyc_Absyn_Break_s: 0){ _LL1342: _temp1341=(( struct Cyc_Absyn_Break_s_struct*)
_temp1251)->f1; if( _temp1341 ==  0){ goto _LL1272;} else{ goto _LL1273;}} else{
goto _LL1273;} _LL1273: if(( unsigned int) _temp1251 >  1u?*(( int*) _temp1251)
==  Cyc_Absyn_Fallthru_s: 0){ _LL1347: _temp1346=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1251)->f1; goto _LL1344; _LL1344: _temp1343=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1251)->f2; if( _temp1343 ==  0){ goto _LL1275;} else{ _temp1345=* _temp1343;
goto _LL1274;}} else{ goto _LL1275;} _LL1275: if(( unsigned int) _temp1251 >  1u?*((
int*) _temp1251) ==  Cyc_Absyn_Break_s: 0){ _LL1349: _temp1348=(( struct Cyc_Absyn_Break_s_struct*)
_temp1251)->f1; goto _LL1276;} else{ goto _LL1277;} _LL1277: if(( unsigned int)
_temp1251 >  1u?*(( int*) _temp1251) ==  Cyc_Absyn_Continue_s: 0){ _LL1351:
_temp1350=(( struct Cyc_Absyn_Continue_s_struct*) _temp1251)->f1; goto _LL1278;}
else{ goto _LL1279;} _LL1279: if(( unsigned int) _temp1251 >  1u?*(( int*)
_temp1251) ==  Cyc_Absyn_Goto_s: 0){ _LL1353: _temp1352=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1251)->f2; goto _LL1280;} else{ goto _LL1281;} _LL1281: if(( unsigned int)
_temp1251 >  1u?*(( int*) _temp1251) ==  Cyc_Absyn_Switch_s: 0){ _LL1357:
_temp1356=(( struct Cyc_Absyn_Switch_s_struct*) _temp1251)->f1; goto _LL1355;
_LL1355: _temp1354=(( struct Cyc_Absyn_Switch_s_struct*) _temp1251)->f2; goto
_LL1282;} else{ goto _LL1283;} _LL1283: if(( unsigned int) _temp1251 >  1u?*((
int*) _temp1251) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1361: _temp1360=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1251)->f1; goto _LL1359; _LL1359: _temp1358=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1251)->f2; goto _LL1284;} else{ goto _LL1285;} _LL1285: if(( unsigned int)
_temp1251 >  1u?*(( int*) _temp1251) ==  Cyc_Absyn_Decl_s: 0){ _LL1365:
_temp1364=(( struct Cyc_Absyn_Decl_s_struct*) _temp1251)->f1; goto _LL1363;
_LL1363: _temp1362=(( struct Cyc_Absyn_Decl_s_struct*) _temp1251)->f2; goto
_LL1286;} else{ goto _LL1287;} _LL1287: if(( unsigned int) _temp1251 >  1u?*((
int*) _temp1251) ==  Cyc_Absyn_Label_s: 0){ _LL1367: _temp1366=(( struct Cyc_Absyn_Label_s_struct*)
_temp1251)->f2; goto _LL1288;} else{ goto _LL1289;} _LL1289: if(( unsigned int)
_temp1251 >  1u?*(( int*) _temp1251) ==  Cyc_Absyn_Region_s: 0){ _LL1373:
_temp1372=(( struct Cyc_Absyn_Region_s_struct*) _temp1251)->f1; goto _LL1371;
_LL1371: _temp1370=(( struct Cyc_Absyn_Region_s_struct*) _temp1251)->f2; goto
_LL1369; _LL1369: _temp1368=(( struct Cyc_Absyn_Region_s_struct*) _temp1251)->f3;
goto _LL1290;} else{ goto _LL1291;} _LL1291: goto _LL1292; _LL1254: return
inflow; _LL1256: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1258: { void*
_temp1376; void* _temp1378; struct _tuple0 _temp1374= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1295)); _LL1379:
_temp1378= _temp1374.f1; goto _LL1377; _LL1377: _temp1376= _temp1374.f2; goto
_LL1375; _LL1375: Cyc_NewControlFlow_use_Rval( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp1295))->loc, _temp1378, _temp1376); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1260: return( Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp1297)).f1;
_LL1262: return Cyc_NewControlFlow_anal_stmt( env, Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1301), _temp1299); _LL1264: { void* _temp1382; void* _temp1384;
struct _tuple0 _temp1380= Cyc_NewControlFlow_anal_test( env, inflow, _temp1307);
_LL1385: _temp1384= _temp1380.f1; goto _LL1383; _LL1383: _temp1382= _temp1380.f2;
goto _LL1381; _LL1381: return Cyc_CfFlowInfo_join_flow( env->all_changed, Cyc_NewControlFlow_anal_stmt(
env, _temp1384, _temp1305), Cyc_NewControlFlow_anal_stmt( env, _temp1382,
_temp1303));} _LL1266: { void* _temp1386=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, inflow, _temp1313))->flow; void* _temp1389; void* _temp1391; struct _tuple0
_temp1387= Cyc_NewControlFlow_anal_test( env, _temp1386, _temp1315); _LL1392:
_temp1391= _temp1387.f1; goto _LL1390; _LL1390: _temp1389= _temp1387.f2; goto
_LL1388; _LL1388: { void* _temp1393= Cyc_NewControlFlow_anal_stmt( env,
_temp1391, _temp1309); Cyc_NewControlFlow_update_flow( env, _temp1313, _temp1393);
return _temp1389;}} _LL1268: { void* _temp1394= Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1323); void* _temp1395=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1394, _temp1319))->flow; void* _temp1398; void* _temp1400; struct
_tuple0 _temp1396= Cyc_NewControlFlow_anal_test( env, _temp1395, _temp1321);
_LL1401: _temp1400= _temp1396.f1; goto _LL1399; _LL1399: _temp1398= _temp1396.f2;
goto _LL1397; _LL1397: Cyc_NewControlFlow_update_flow( env, _temp1323, _temp1400);
return _temp1398;} _LL1270: { void* _temp1402=( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1339)).f1; void* _temp1403=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1402, _temp1335))->flow; void* _temp1406; void* _temp1408; struct
_tuple0 _temp1404= Cyc_NewControlFlow_anal_test( env, _temp1403, _temp1337);
_LL1409: _temp1408= _temp1404.f1; goto _LL1407; _LL1407: _temp1406= _temp1404.f2;
goto _LL1405; _LL1405: { void* _temp1410= Cyc_NewControlFlow_anal_stmt( env,
_temp1408, _temp1325); void* _temp1411=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1410, _temp1329))->flow; void* _temp1412=( Cyc_NewControlFlow_anal_Rexp(
env, _temp1411, _temp1331)).f1; Cyc_NewControlFlow_update_flow( env, _temp1335,
_temp1412); return _temp1406;}} _LL1272: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1274: { struct Cyc_List_List* _temp1415; void* _temp1417; struct _tuple4
_temp1413= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1346);
_LL1418: _temp1417= _temp1413.f1; goto _LL1416; _LL1416: _temp1415= _temp1413.f2;
goto _LL1414; _LL1414: for( 0; _temp1415 !=  0;( _temp1415= _temp1415->tl,
_temp1346= _temp1346->tl)){ _temp1417= Cyc_NewControlFlow_use_Rval( env,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1346))->hd)->loc,
_temp1417,( void*) _temp1415->hd);} _temp1417= Cyc_NewControlFlow_add_vars(
_temp1417,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1345->pat_vars))->v, Cyc_CfFlowInfo_unknown_all); Cyc_NewControlFlow_update_flow(
env,( struct Cyc_Absyn_Stmt*) _temp1345->body, _temp1417); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1276: _temp1350= _temp1348; goto _LL1278; _LL1278: _temp1352= _temp1350; goto
_LL1280; _LL1280: if( env->iteration_num ==  1){ struct Cyc_Absyn_Stmt*
_temp1419= _temp1250->encloser; struct Cyc_Absyn_Stmt* _temp1420=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp1352)))->encloser; while( _temp1420 != 
_temp1419) { struct Cyc_Absyn_Stmt* _temp1421=( Cyc_NewControlFlow_get_stmt_annot(
_temp1419))->encloser; if( _temp1421 ==  _temp1419){({ void* _temp1422[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("goto enters local scope or exception handler",
sizeof( unsigned char), 45u), _tag_arr( _temp1422, sizeof( void*), 0u));});
break;} _temp1419= _temp1421;}} Cyc_NewControlFlow_update_flow( env,( struct Cyc_Absyn_Stmt*)
_check_null( _temp1352), inflow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1282: { void* _temp1425; void* _temp1427; struct _tuple0 _temp1423= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1356); _LL1428: _temp1427= _temp1423.f1; goto _LL1426; _LL1426:
_temp1425= _temp1423.f2; goto _LL1424; _LL1424: _temp1427= Cyc_NewControlFlow_use_Rval(
env, _temp1356->loc, _temp1427, _temp1425); return Cyc_NewControlFlow_anal_scs(
env, _temp1427, _temp1354);} _LL1284: { int old_in_try= env->in_try; void*
old_tryflow=( void*) env->tryflow; env->in_try= 1;( void*)( env->tryflow=( void*)
inflow);{ void* s1_outflow= Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1360);
void* scs_inflow=( void*) env->tryflow; env->in_try= old_in_try;( void*)( env->tryflow=(
void*) old_tryflow); Cyc_NewControlFlow_update_tryflow( env, scs_inflow);{ void*
scs_outflow= Cyc_NewControlFlow_anal_scs( env, scs_inflow, _temp1358);{ void*
_temp1429= scs_outflow; _LL1431: if( _temp1429 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1432;} else{ goto _LL1433;} _LL1433: goto _LL1434; _LL1432: goto _LL1430;
_LL1434:({ void* _temp1435[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("last catch clause may implicitly fallthru",
sizeof( unsigned char), 42u), _tag_arr( _temp1435, sizeof( void*), 0u));});
_LL1430:;} return s1_outflow;}}} _LL1286: return Cyc_NewControlFlow_anal_stmt(
env, Cyc_NewControlFlow_anal_decl( env, inflow, _temp1364), _temp1362); _LL1288:
return Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1366); _LL1290: { struct
Cyc_List_List _temp1436=({ struct Cyc_List_List _temp1437; _temp1437.hd=( void*)
_temp1370; _temp1437.tl= 0; _temp1437;}); inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)& _temp1436, Cyc_CfFlowInfo_unknown_all); return
Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1368);} _LL1292:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp1438=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1438[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1439; _temp1439.tag= Cyc_Core_Impossible;
_temp1439.f1= _tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 56u); _temp1439;}); _temp1438;})); _LL1252:;}} static
void* Cyc_NewControlFlow_anal_decl( struct Cyc_NewControlFlow_AnalEnv* env, void*
inflow, struct Cyc_Absyn_Decl* decl){ void* _temp1440=( void*) decl->r; struct
Cyc_Absyn_Vardecl* _temp1450; struct Cyc_Absyn_Exp* _temp1452; struct Cyc_Core_Opt*
_temp1454; struct Cyc_Core_Opt _temp1456; struct Cyc_List_List* _temp1457;
struct Cyc_List_List* _temp1459; _LL1442: if(*(( int*) _temp1440) ==  Cyc_Absyn_Var_d){
_LL1451: _temp1450=(( struct Cyc_Absyn_Var_d_struct*) _temp1440)->f1; goto
_LL1443;} else{ goto _LL1444;} _LL1444: if(*(( int*) _temp1440) ==  Cyc_Absyn_Let_d){
_LL1455: _temp1454=(( struct Cyc_Absyn_Let_d_struct*) _temp1440)->f2; if(
_temp1454 ==  0){ goto _LL1446;} else{ _temp1456=* _temp1454; _LL1458: _temp1457=(
struct Cyc_List_List*) _temp1456.v; goto _LL1453;} _LL1453: _temp1452=(( struct
Cyc_Absyn_Let_d_struct*) _temp1440)->f4; goto _LL1445;} else{ goto _LL1446;}
_LL1446: if(*(( int*) _temp1440) ==  Cyc_Absyn_Letv_d){ _LL1460: _temp1459=((
struct Cyc_Absyn_Letv_d_struct*) _temp1440)->f1; goto _LL1447;} else{ goto
_LL1448;} _LL1448: goto _LL1449; _LL1443: { struct Cyc_List_List _temp1461=({
struct Cyc_List_List _temp1483; _temp1483.hd=( void*) _temp1450; _temp1483.tl= 0;
_temp1483;}); inflow= Cyc_NewControlFlow_add_vars( inflow,( struct Cyc_List_List*)&
_temp1461, Cyc_CfFlowInfo_unknown_none);{ struct Cyc_Absyn_Exp* _temp1462=
_temp1450->initializer; if( _temp1462 ==  0){ return inflow;}{ void* _temp1465;
void* _temp1467; struct _tuple0 _temp1463= Cyc_NewControlFlow_anal_Rexp( env,
inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1462)); _LL1468: _temp1467=
_temp1463.f1; goto _LL1466; _LL1466: _temp1465= _temp1463.f2; goto _LL1464;
_LL1464: { void* _temp1469= _temp1467; struct Cyc_Dict_Dict* _temp1475; _LL1471:
if( _temp1469 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1472;} else{ goto
_LL1473;} _LL1473: if(( unsigned int) _temp1469 >  1u?*(( int*) _temp1469) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL1476: _temp1475=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1469)->f1; goto _LL1474;} else{ goto _LL1470;} _LL1472: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1474: _temp1475= Cyc_CfFlowInfo_assign_place( decl->loc, _temp1475, env->all_changed,({
struct Cyc_CfFlowInfo_Place* _temp1477=( struct Cyc_CfFlowInfo_Place*) _cycalloc(
sizeof( struct Cyc_CfFlowInfo_Place)); _temp1477->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1478=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1478[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1479; _temp1479.tag= Cyc_CfFlowInfo_VarRoot;
_temp1479.f1= _temp1450; _temp1479;}); _temp1478;})); _temp1477->fields= 0;
_temp1477;}), _temp1465);{ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp1480=({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp1481=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp1481[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp1482; _temp1482.tag= Cyc_CfFlowInfo_ReachableFL;
_temp1482.f1= _temp1475; _temp1482;}); _temp1481;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp1480); return( void*) _temp1480;} _LL1470:;}}}} _LL1445: {
void* _temp1486; void* _temp1488; struct _tuple0 _temp1484= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1452); _LL1489: _temp1488= _temp1484.f1; goto _LL1487; _LL1487:
_temp1486= _temp1484.f2; goto _LL1485; _LL1485: _temp1488= Cyc_NewControlFlow_use_Rval(
env, _temp1452->loc, _temp1488, _temp1486); return Cyc_NewControlFlow_add_vars(
_temp1488, _temp1457, Cyc_CfFlowInfo_unknown_all);} _LL1447: return Cyc_NewControlFlow_add_vars(
inflow, _temp1459, Cyc_CfFlowInfo_unknown_none); _LL1449:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp1490=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1490[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1491; _temp1491.tag= Cyc_Core_Impossible;
_temp1491.f1= _tag_arr("anal_decl: unexpected decl variant", sizeof(
unsigned char), 35u); _temp1491;}); _temp1490;})); _LL1441:;} static void Cyc_NewControlFlow_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp1492=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1516; struct _tuple3
_temp1518; struct Cyc_Absyn_Stmt* _temp1520; struct _tuple3 _temp1522; struct
Cyc_Absyn_Stmt* _temp1524; struct Cyc_Absyn_Stmt* _temp1526; struct Cyc_Absyn_Stmt*
_temp1528; struct Cyc_Absyn_Stmt* _temp1530; struct _tuple3 _temp1532; struct
Cyc_Absyn_Stmt* _temp1534; struct _tuple3 _temp1536; struct Cyc_Absyn_Stmt*
_temp1538; struct Cyc_Absyn_Stmt* _temp1540; struct Cyc_Absyn_Stmt* _temp1542;
struct Cyc_Absyn_Stmt* _temp1544; struct Cyc_Absyn_Stmt* _temp1546; struct Cyc_Absyn_Stmt*
_temp1548; struct Cyc_Absyn_Stmt* _temp1550; struct Cyc_List_List* _temp1552;
struct Cyc_Absyn_Stmt* _temp1554; struct Cyc_List_List* _temp1556; _LL1494: if((
unsigned int) _temp1492 >  1u?*(( int*) _temp1492) ==  Cyc_Absyn_For_s: 0){
_LL1523: _temp1522=(( struct Cyc_Absyn_For_s_struct*) _temp1492)->f2; _LL1525:
_temp1524= _temp1522.f2; goto _LL1519; _LL1519: _temp1518=(( struct Cyc_Absyn_For_s_struct*)
_temp1492)->f3; _LL1521: _temp1520= _temp1518.f2; goto _LL1517; _LL1517:
_temp1516=(( struct Cyc_Absyn_For_s_struct*) _temp1492)->f4; goto _LL1495;}
else{ goto _LL1496;} _LL1496: if(( unsigned int) _temp1492 >  1u?*(( int*)
_temp1492) ==  Cyc_Absyn_Seq_s: 0){ _LL1529: _temp1528=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1492)->f1; goto _LL1527; _LL1527: _temp1526=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1492)->f2; goto _LL1497;} else{ goto _LL1498;} _LL1498: if(( unsigned int)
_temp1492 >  1u?*(( int*) _temp1492) ==  Cyc_Absyn_While_s: 0){ _LL1533:
_temp1532=(( struct Cyc_Absyn_While_s_struct*) _temp1492)->f1; _LL1535:
_temp1534= _temp1532.f2; goto _LL1531; _LL1531: _temp1530=(( struct Cyc_Absyn_While_s_struct*)
_temp1492)->f2; goto _LL1499;} else{ goto _LL1500;} _LL1500: if(( unsigned int)
_temp1492 >  1u?*(( int*) _temp1492) ==  Cyc_Absyn_Do_s: 0){ _LL1541: _temp1540=((
struct Cyc_Absyn_Do_s_struct*) _temp1492)->f1; goto _LL1537; _LL1537: _temp1536=((
struct Cyc_Absyn_Do_s_struct*) _temp1492)->f2; _LL1539: _temp1538= _temp1536.f2;
goto _LL1501;} else{ goto _LL1502;} _LL1502: if(( unsigned int) _temp1492 >  1u?*((
int*) _temp1492) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1545: _temp1544=(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp1492)->f2; goto _LL1543; _LL1543: _temp1542=((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1492)->f3; goto _LL1503;} else{ goto
_LL1504;} _LL1504: if(( unsigned int) _temp1492 >  1u?*(( int*) _temp1492) == 
Cyc_Absyn_Region_s: 0){ _LL1547: _temp1546=(( struct Cyc_Absyn_Region_s_struct*)
_temp1492)->f3; goto _LL1505;} else{ goto _LL1506;} _LL1506: if(( unsigned int)
_temp1492 >  1u?*(( int*) _temp1492) ==  Cyc_Absyn_Label_s: 0){ _LL1549:
_temp1548=(( struct Cyc_Absyn_Label_s_struct*) _temp1492)->f2; goto _LL1507;}
else{ goto _LL1508;} _LL1508: if(( unsigned int) _temp1492 >  1u?*(( int*)
_temp1492) ==  Cyc_Absyn_Decl_s: 0){ _LL1551: _temp1550=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1492)->f2; goto _LL1509;} else{ goto _LL1510;} _LL1510: if(( unsigned int)
_temp1492 >  1u?*(( int*) _temp1492) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1555:
_temp1554=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1492)->f1; goto _LL1553;
_LL1553: _temp1552=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1492)->f2; goto
_LL1511;} else{ goto _LL1512;} _LL1512: if(( unsigned int) _temp1492 >  1u?*((
int*) _temp1492) ==  Cyc_Absyn_Switch_s: 0){ _LL1557: _temp1556=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1492)->f2; goto _LL1513;} else{ goto _LL1514;} _LL1514: goto _LL1515;
_LL1495: Cyc_NewControlFlow_postpass( _temp1520); _temp1528= _temp1516;
_temp1526= _temp1524; goto _LL1497; _LL1497: _temp1534= _temp1528; _temp1530=
_temp1526; goto _LL1499; _LL1499: _temp1540= _temp1530; _temp1538= _temp1534;
goto _LL1501; _LL1501: _temp1544= _temp1540; _temp1542= _temp1538; goto _LL1503;
_LL1503: Cyc_NewControlFlow_postpass( _temp1544); _temp1546= _temp1542; goto
_LL1505; _LL1505: _temp1548= _temp1546; goto _LL1507; _LL1507: _temp1550=
_temp1548; goto _LL1509; _LL1509: Cyc_NewControlFlow_postpass( _temp1550);
return; _LL1511: Cyc_NewControlFlow_postpass( _temp1554); _temp1556= _temp1552;
goto _LL1513; _LL1513: for( 0; _temp1556 !=  0; _temp1556= _temp1556->tl){ Cyc_NewControlFlow_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1556->hd)->body);} return; _LL1515:
return; _LL1493:;}} static void Cyc_NewControlFlow_check_fun( struct Cyc_Absyn_Fndecl*
fd){ void* inflow= Cyc_NewControlFlow_add_vars(( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp1566=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp1566[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp1567; _temp1567.tag= Cyc_CfFlowInfo_ReachableFL; _temp1567.f1= Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp); _temp1567;}); _temp1566;}),( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v, Cyc_CfFlowInfo_unknown_all);
struct Cyc_NewControlFlow_AnalEnv* env=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1565=( struct Cyc_NewControlFlow_AnalEnv*) _cycalloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1565->iterate_again= 1; _temp1565->iteration_num= 0; _temp1565->in_try= 0;
_temp1565->tryflow=( void*) inflow; _temp1565->all_changed= 0; _temp1565;});
void* outflow= inflow; while( env->iterate_again) { ++ env->iteration_num; env->iterate_again=
0; outflow= Cyc_NewControlFlow_anal_stmt( env, inflow, fd->body);}{ void*
_temp1558= outflow; _LL1560: if( _temp1558 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1561;} else{ goto _LL1562;} _LL1562: goto _LL1563; _LL1561: goto _LL1559;
_LL1563: if(( void*) fd->ret_type != ( void*) Cyc_Absyn_VoidType){({ void*
_temp1564[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc, _tag_arr("function may not return a value",
sizeof( unsigned char), 32u), _tag_arr( _temp1564, sizeof( void*), 0u));});}
_LL1559:;} Cyc_NewControlFlow_postpass( fd->body);} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds !=  0; ds= ds->tl){ void* _temp1568=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1580; struct
Cyc_List_List* _temp1582; struct Cyc_List_List* _temp1584; struct Cyc_List_List*
_temp1586; _LL1570: if(*(( int*) _temp1568) ==  Cyc_Absyn_Fn_d){ _LL1581:
_temp1580=(( struct Cyc_Absyn_Fn_d_struct*) _temp1568)->f1; goto _LL1571;} else{
goto _LL1572;} _LL1572: if(*(( int*) _temp1568) ==  Cyc_Absyn_ExternC_d){
_LL1583: _temp1582=(( struct Cyc_Absyn_ExternC_d_struct*) _temp1568)->f1; goto
_LL1573;} else{ goto _LL1574;} _LL1574: if(*(( int*) _temp1568) ==  Cyc_Absyn_Using_d){
_LL1585: _temp1584=(( struct Cyc_Absyn_Using_d_struct*) _temp1568)->f2; goto
_LL1575;} else{ goto _LL1576;} _LL1576: if(*(( int*) _temp1568) ==  Cyc_Absyn_Namespace_d){
_LL1587: _temp1586=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1568)->f2; goto
_LL1577;} else{ goto _LL1578;} _LL1578: goto _LL1579; _LL1571: Cyc_NewControlFlow_check_fun(
_temp1580); goto _LL1569; _LL1573: _temp1584= _temp1582; goto _LL1575; _LL1575:
_temp1586= _temp1584; goto _LL1577; _LL1577: Cyc_NewControlFlow_cf_check(
_temp1586); goto _LL1569; _LL1579: goto _LL1569; _LL1569:;}}
