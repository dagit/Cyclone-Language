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
void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); extern struct Cyc_List_List*
Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern
int Cyc_Std_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr* s2); extern
int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); struct Cyc_Lineno_Pos{
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
tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff= 16;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 18; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
f3; } ; static const int Cyc_Absyn_Malloc_e= 32; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
33; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 34; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 35; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 36; struct Cyc_Absyn_Fill_e_struct{
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
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_UnknownId_p= 10;
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct
Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_SwitchC_clause{
struct Cyc_Absyn_Exp* cnst_exp; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment*
loc; } ; static const int Cyc_Absyn_Unresolved_b= 0; static const int Cyc_Absyn_Global_b=
0; struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Funname_b= 1; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Param_b= 2;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Local_b= 3; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Pat_b= 4; struct
Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct Cyc_Absyn_Vardecl{
void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct _tagged_arr Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_ShortPtr_sa= 0; struct
Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
extern struct _tagged_arr Cyc_Absynpp_typ2string( void*); extern struct
_tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); struct Cyc_Set_Set;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* cmp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict*
d, void* k); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* k, void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* k); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict*
d, void* k); extern void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_filter_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0;
static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2;
static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct
Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_Frame=
1; struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv();
extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tagged_arr*,
struct Cyc_List_List*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern unsigned char
Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct _tagged_arr fmt,
struct _tagged_arr ap); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type(
void* t); extern void Cyc_Tcutil_explain_failure(); extern void* Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*); extern void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width,
struct _tagged_arr* fn); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, int allow_evars, void*); extern void
Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*, struct Cyc_List_List*);
extern void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*); extern int
Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_supports_default( void*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
var_default_init, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); extern void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*, struct Cyc_Position_Segment*,
struct Cyc_Absyn_Structdecl*); extern void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*, struct Cyc_Position_Segment*, struct Cyc_Absyn_Uniondecl*);
extern void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*, struct Cyc_Absyn_Tuniondecl*); extern void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*, struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*); extern unsigned char Cyc_Tcdecl_Incompatible[ 17u];
struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl* base; struct
Cyc_Absyn_Tunionfield* field; } ; extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern void* Cyc_Tcdecl_merge_binding( void* d0,
void* d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern
struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List* f,
int* res, struct _tagged_arr* v, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg); struct Cyc_Tcgenrep_RepInfo; extern struct Cyc_Dict_Dict* Cyc_Tcgenrep_empty_typerep_dict();
struct _tuple3{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Exp*
f3; } ; extern struct _tuple3 Cyc_Tcgenrep_tcGenrep( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, void* type, struct
Cyc_Dict_Dict* dict); static unsigned char _temp0[ 1u]=""; static struct
_tagged_arr Cyc_Tc_tc_msg_c={ _temp0, _temp0, _temp0 +  1u}; static struct
_tagged_arr* Cyc_Tc_tc_msg=( struct _tagged_arr*)& Cyc_Tc_tc_msg_c; static
struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp1= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp7;
struct Cyc_List_List* _temp9; struct Cyc_List_List** _temp11; _LL3: if((
unsigned int) _temp1 >  4u?*(( int*) _temp1) ==  Cyc_Absyn_FnType: 0){ _LL8:
_temp7=(( struct Cyc_Absyn_FnType_struct*) _temp1)->f1; _LL10: _temp9= _temp7.attributes;
_temp11=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*) _temp1)->f1).attributes;
goto _LL4;} else{ goto _LL5;} _LL5: goto _LL6; _LL4: { struct Cyc_List_List*
_temp12= 0; for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){*
_temp11=({ struct Cyc_List_List* _temp13=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp13->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp13->tl=* _temp11; _temp13;});} else{ _temp12=({
struct Cyc_List_List* _temp14=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp14->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp14->tl= _temp12; _temp14;});}} return _temp12;}
_LL6: return({ void* _temp15[ 0u]={};(( struct Cyc_List_List*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("transfer_fn_type_atts",
sizeof( unsigned char), 22u), _tag_arr( _temp15, sizeof( void*), 0u));}); _LL2:;}
struct _tuple4{ void* f1; int f2; } ; static void Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl*
vd, int check_var_init){ struct _tagged_arr* v=(* vd->name).f2; void* t=( void*)
vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;{
void* _temp16=(* vd->name).f1; struct Cyc_List_List* _temp24; struct Cyc_List_List*
_temp26; _LL18: if(( unsigned int) _temp16 >  1u?*(( int*) _temp16) ==  Cyc_Absyn_Rel_n:
0){ _LL25: _temp24=(( struct Cyc_Absyn_Rel_n_struct*) _temp16)->f1; if( _temp24
==  0){ goto _LL19;} else{ goto _LL20;}} else{ goto _LL20;} _LL20: if((
unsigned int) _temp16 >  1u?*(( int*) _temp16) ==  Cyc_Absyn_Abs_n: 0){ _LL27:
_temp26=(( struct Cyc_Absyn_Abs_n_struct*) _temp16)->f1; if( _temp26 ==  0){
goto _LL21;} else{ goto _LL22;}} else{ goto _LL22;} _LL22: goto _LL23; _LL19:
goto _LL17; _LL21: goto _LL17; _LL23:({ struct Cyc_Std_String_pa_struct _temp29;
_temp29.tag= Cyc_Std_String_pa; _temp29.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp28[ 1u]={& _temp29}; Cyc_Tcutil_terr( loc, _tag_arr("qualified variable declarations are not implemented (%s)",
sizeof( unsigned char), 57u), _tag_arr( _temp28, sizeof( void*), 1u));}});
return; _LL17:;}(* vd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp30=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp30[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp31; _temp31.tag= Cyc_Absyn_Abs_n;
_temp31.f1= te->ns; _temp31;}); _temp30;});{ void* _temp32= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Exp* _temp39; struct Cyc_Absyn_Tqual _temp41; void* _temp43;
_LL34: if(( unsigned int) _temp32 >  4u?*(( int*) _temp32) ==  Cyc_Absyn_ArrayType:
0){ _LL44: _temp43=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp32)->f1;
goto _LL42; _LL42: _temp41=(( struct Cyc_Absyn_ArrayType_struct*) _temp32)->f2;
goto _LL40; _LL40: _temp39=(( struct Cyc_Absyn_ArrayType_struct*) _temp32)->f3;
if( _temp39 ==  0){ goto _LL38;} else{ goto _LL36;}} else{ goto _LL36;} _LL38:
if( vd->initializer !=  0){ goto _LL35;} else{ goto _LL36;} _LL36: goto _LL37;
_LL35:{ void* _temp45=( void*)(( struct Cyc_Absyn_Exp*) _check_null( vd->initializer))->r;
void* _temp57; struct _tagged_arr _temp59; struct Cyc_Absyn_Exp* _temp61; struct
Cyc_List_List* _temp63; struct Cyc_List_List* _temp65; _LL47: if(*(( int*)
_temp45) ==  Cyc_Absyn_Const_e){ _LL58: _temp57=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp45)->f1; if(( unsigned int) _temp57 >  1u?*(( int*) _temp57) ==  Cyc_Absyn_String_c:
0){ _LL60: _temp59=(( struct Cyc_Absyn_String_c_struct*) _temp57)->f1; goto
_LL48;} else{ goto _LL49;}} else{ goto _LL49;} _LL49: if(*(( int*) _temp45) == 
Cyc_Absyn_Comprehension_e){ _LL62: _temp61=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp45)->f2; goto _LL50;} else{ goto _LL51;} _LL51: if(*(( int*) _temp45) == 
Cyc_Absyn_UnresolvedMem_e){ _LL64: _temp63=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp45)->f2; goto _LL52;} else{ goto _LL53;} _LL53: if(*(( int*) _temp45) == 
Cyc_Absyn_Array_e){ _LL66: _temp65=(( struct Cyc_Absyn_Array_e_struct*) _temp45)->f1;
goto _LL54;} else{ goto _LL55;} _LL55: goto _LL56; _LL48: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp67=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp67[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp68; _temp68.tag= Cyc_Absyn_ArrayType; _temp68.f1=(
void*) _temp43; _temp68.f2= _temp41; _temp68.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
_get_arr_size( _temp59, sizeof( unsigned char)), 0); _temp68;}); _temp67;})));
goto _LL46; _LL50: t=( void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp69=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp69[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp70; _temp70.tag= Cyc_Absyn_ArrayType;
_temp70.f1=( void*) _temp43; _temp70.f2= _temp41; _temp70.f3=( struct Cyc_Absyn_Exp*)
_temp61; _temp70;}); _temp69;}))); goto _LL46; _LL52: _temp65= _temp63; goto
_LL54; _LL54: t=( void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp71=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp71[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp72; _temp72.tag= Cyc_Absyn_ArrayType;
_temp72.f1=( void*) _temp43; _temp72.f2= _temp41; _temp72.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp65), 0); _temp72;}); _temp71;}))); goto _LL46; _LL56: goto _LL46; _LL46:;}
goto _LL33; _LL37: goto _LL33; _LL33:;} Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);} if( sc == ( void*) Cyc_Absyn_Extern? 1: sc == ( void*) Cyc_Absyn_ExternC){
if( vd->initializer !=  0){({ void* _temp73[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("extern declaration should not have initializer", sizeof( unsigned char),
47u), _tag_arr( _temp73, sizeof( void*), 0u));});}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp74=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL76:
if(( unsigned int) _temp74 >  16u?*(( int*) _temp74) ==  Cyc_Absyn_Aligned_att:
0){ goto _LL77;} else{ goto _LL78;} _LL78: if(( unsigned int) _temp74 >  16u?*((
int*) _temp74) ==  Cyc_Absyn_Section_att: 0){ goto _LL79;} else{ goto _LL80;}
_LL80: if( _temp74 == ( void*) Cyc_Absyn_Nocommon_att){ goto _LL81;} else{ goto
_LL82;} _LL82: if( _temp74 == ( void*) Cyc_Absyn_Shared_att){ goto _LL83;} else{
goto _LL84;} _LL84: if( _temp74 == ( void*) Cyc_Absyn_Unused_att){ goto _LL85;}
else{ goto _LL86;} _LL86: if( _temp74 == ( void*) Cyc_Absyn_Weak_att){ goto
_LL87;} else{ goto _LL88;} _LL88: if( _temp74 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL89;} else{ goto _LL90;} _LL90: if( _temp74 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL91;} else{ goto _LL92;} _LL92: goto _LL93; _LL77: continue; _LL79:
continue; _LL81: continue; _LL83: continue; _LL85: continue; _LL87: continue;
_LL89: continue; _LL91: continue; _LL93:({ struct Cyc_Std_String_pa_struct
_temp96; _temp96.tag= Cyc_Std_String_pa; _temp96.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ struct Cyc_Std_String_pa_struct _temp95; _temp95.tag= Cyc_Std_String_pa;
_temp95.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp94[ 2u]={& _temp95,&
_temp96}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s for variable %s",
sizeof( unsigned char), 33u), _tag_arr( _temp94, sizeof( void*), 2u));}}}); goto
_LL75; _LL75:;} if( vd->initializer ==  0){ if( check_var_init? ! Cyc_Tcutil_supports_default(
t): 0){({ struct Cyc_Std_String_pa_struct _temp99; _temp99.tag= Cyc_Std_String_pa;
_temp99.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct
_temp98; _temp98.tag= Cyc_Std_String_pa; _temp98.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp97[ 2u]={& _temp98,& _temp99}; Cyc_Tcutil_terr( loc,
_tag_arr("initializer required for variable %s of type %s", sizeof(
unsigned char), 48u), _tag_arr( _temp97, sizeof( void*), 2u));}}});}} else{
struct Cyc_Absyn_Exp* _temp100=( struct Cyc_Absyn_Exp*) _check_null( vd->initializer);
void* _temp101= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp100); if( !
Cyc_Tcutil_coerce_assign( te, _temp100, t)){({ struct Cyc_Std_String_pa_struct
_temp105; _temp105.tag= Cyc_Std_String_pa; _temp105.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp101);{ struct Cyc_Std_String_pa_struct _temp104; _temp104.tag= Cyc_Std_String_pa;
_temp104.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct
_temp103; _temp103.tag= Cyc_Std_String_pa; _temp103.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp102[ 3u]={& _temp103,& _temp104,& _temp105}; Cyc_Tcutil_terr(
loc, _tag_arr("%s is declared with type \n%s\n but initialized with type \n%s",
sizeof( unsigned char), 60u), _tag_arr( _temp102, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();} if( ! Cyc_Tcutil_is_const_exp( te, _temp100)){({
void* _temp106[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("initializer is not a constant expression",
sizeof( unsigned char), 41u), _tag_arr( _temp106, sizeof( void*), 0u));});}}}
else{ for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp107=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL109:
if(( unsigned int) _temp107 >  16u?*(( int*) _temp107) ==  Cyc_Absyn_Regparm_att:
0){ goto _LL110;} else{ goto _LL111;} _LL111: if( _temp107 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL112;} else{ goto _LL113;} _LL113: if( _temp107 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL114;} else{ goto _LL115;} _LL115: if( _temp107 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL116;} else{ goto _LL117;} _LL117: if( _temp107 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp107 >  16u?*((
int*) _temp107) ==  Cyc_Absyn_Format_att: 0){ goto _LL120;} else{ goto _LL121;}
_LL121: if( _temp107 == ( void*) Cyc_Absyn_Const_att){ goto _LL122;} else{ goto
_LL123;} _LL123: if(( unsigned int) _temp107 >  16u?*(( int*) _temp107) ==  Cyc_Absyn_Aligned_att:
0){ goto _LL124;} else{ goto _LL125;} _LL125: if( _temp107 == ( void*) Cyc_Absyn_Packed_att){
goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL110: goto _LL112;
_LL112: goto _LL114; _LL114: goto _LL116; _LL116: goto _LL118; _LL118: goto
_LL120; _LL120: goto _LL122; _LL122:({ void* _temp129[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("tcVardecl: fn type attributes in function var decl",
sizeof( unsigned char), 51u), _tag_arr( _temp129, sizeof( void*), 0u));}); goto
_LL108; _LL124: goto _LL126; _LL126:({ struct Cyc_Std_String_pa_struct _temp131;
_temp131.tag= Cyc_Std_String_pa; _temp131.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp130[ 1u]={&
_temp131}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s in function declaration",
sizeof( unsigned char), 41u), _tag_arr( _temp130, sizeof( void*), 1u));}}); goto
_LL108; _LL128: continue; _LL108:;}}}{ struct _handler_cons _temp132;
_push_handler(& _temp132);{ int _temp134= 0; if( setjmp( _temp132.handler)){
_temp134= 1;} if( ! _temp134){{ struct _tuple4* _temp135=(( struct _tuple4*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->ordinaries,
v); void* _temp136=(* _temp135).f1; void* _temp146; _LL138: if(*(( int*)
_temp136) ==  Cyc_Tcenv_VarRes){ _LL147: _temp146=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp136)->f1; goto _LL139;} else{ goto _LL140;} _LL140: if(*(( int*) _temp136)
==  Cyc_Tcenv_StructRes){ goto _LL141;} else{ goto _LL142;} _LL142: if(*(( int*)
_temp136) ==  Cyc_Tcenv_TunionRes){ goto _LL143;} else{ goto _LL144;} _LL144:
if(*(( int*) _temp136) ==  Cyc_Tcenv_EnumRes){ goto _LL145;} else{ goto _LL137;}
_LL139: { struct Cyc_Absyn_Global_b_struct* _temp148=({ struct Cyc_Absyn_Global_b_struct*
_temp153=( struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp153[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp154; _temp154.tag= Cyc_Absyn_Global_b;
_temp154.f1= vd; _temp154;}); _temp153;}); void* _temp149= Cyc_Tcdecl_merge_binding(
_temp146,( void*) _temp148, loc, Cyc_Tc_tc_msg); if( _temp149 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL137;} if( _temp149 ==  _temp146?(* _temp135).f2: 0){ goto _LL137;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
_tuple4* v)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple4* _temp150=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp150->f1=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp151=( struct Cyc_Tcenv_VarRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp151[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp152; _temp152.tag= Cyc_Tcenv_VarRes; _temp152.f1=( void*) _temp149;
_temp152;}); _temp151;}); _temp150->f2= 1; _temp150;})); goto _LL137;} _LL141:({
void* _temp155[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous struct declaration",
sizeof( unsigned char), 57u), _tag_arr( _temp155, sizeof( void*), 0u));}); goto
_LL137; _LL143:({ void* _temp156[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u), _tag_arr( _temp156, sizeof( void*), 0u));}); goto
_LL137; _LL145:({ void* _temp157[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous enum tag",
sizeof( unsigned char), 47u), _tag_arr( _temp157, sizeof( void*), 0u));}); goto
_LL137; _LL137:;}; _pop_handler();} else{ void* _temp133=( void*) _exn_thrown;
void* _temp159= _temp133; _LL161: if( _temp159 ==  Cyc_Dict_Absent){ goto _LL162;}
else{ goto _LL163;} _LL163: goto _LL164; _LL162: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple4* _temp165=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp165->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp166=( struct Cyc_Tcenv_VarRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp166[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp167; _temp167.tag= Cyc_Tcenv_VarRes;
_temp167.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp168=(
struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp168[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp169; _temp169.tag= Cyc_Absyn_Global_b;
_temp169.f1= vd; _temp169;}); _temp168;})); _temp167;}); _temp166;}); _temp165->f2=
0; _temp165;})); goto _LL160; _LL164:( void) _throw( _temp159); _LL160:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp170=(* fd->name).f1;
struct Cyc_List_List* _temp178; struct Cyc_List_List* _temp180; _LL172: if((
unsigned int) _temp170 >  1u?*(( int*) _temp170) ==  Cyc_Absyn_Rel_n: 0){ _LL179:
_temp178=(( struct Cyc_Absyn_Rel_n_struct*) _temp170)->f1; if( _temp178 ==  0){
goto _LL173;} else{ goto _LL174;}} else{ goto _LL174;} _LL174: if(( unsigned int)
_temp170 >  1u?*(( int*) _temp170) ==  Cyc_Absyn_Abs_n: 0){ _LL181: _temp180=((
struct Cyc_Absyn_Abs_n_struct*) _temp170)->f1; goto _LL175;} else{ goto _LL176;}
_LL176: goto _LL177; _LL173: goto _LL171; _LL175:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp182=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp182[ 0]=({ struct
Cyc_Core_Impossible_struct _temp183; _temp183.tag= Cyc_Core_Impossible; _temp183.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp183;});
_temp182;})); _LL177:({ struct Cyc_Std_String_pa_struct _temp185; _temp185.tag=
Cyc_Std_String_pa; _temp185.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( fd->name);{
void* _temp184[ 1u]={& _temp185}; Cyc_Tcutil_terr( loc, _tag_arr("qualified function declarations are not implemented (%s)",
sizeof( unsigned char), 57u), _tag_arr( _temp184, sizeof( void*), 1u));}});
return; _LL171:;}(* fd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp186=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp186[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp187; _temp187.tag= Cyc_Absyn_Abs_n;
_temp187.f1= te->ns; _temp187;}); _temp186;}); Cyc_Tcutil_check_fndecl_valid_type(
loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ( fd); fd->attributes= Cyc_Tc_transfer_fn_type_atts(
t, fd->attributes);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 
0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp188=(
void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL190: if( _temp188 == (
void*) Cyc_Absyn_Packed_att){ goto _LL191;} else{ goto _LL192;} _LL192: if((
unsigned int) _temp188 >  16u?*(( int*) _temp188) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL193;} else{ goto _LL194;} _LL194: goto _LL195; _LL191: goto _LL193;
_LL193:({ struct Cyc_Std_String_pa_struct _temp197; _temp197.tag= Cyc_Std_String_pa;
_temp197.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp196[ 1u]={& _temp197}; Cyc_Tcutil_terr(
loc, _tag_arr("bad attribute %s for function", sizeof( unsigned char), 30u),
_tag_arr( _temp196, sizeof( void*), 1u));}}); goto _LL189; _LL195: goto _LL189;
_LL189:;}}{ struct _handler_cons _temp198; _push_handler(& _temp198);{ int
_temp200= 0; if( setjmp( _temp198.handler)){ _temp200= 1;} if( ! _temp200){{
struct _tuple4* _temp201=(( struct _tuple4*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( ge->ordinaries, v); void* _temp202=(*
_temp201).f1; void* _temp212; _LL204: if(*(( int*) _temp202) ==  Cyc_Tcenv_VarRes){
_LL213: _temp212=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp202)->f1; goto
_LL205;} else{ goto _LL206;} _LL206: if(*(( int*) _temp202) ==  Cyc_Tcenv_StructRes){
goto _LL207;} else{ goto _LL208;} _LL208: if(*(( int*) _temp202) ==  Cyc_Tcenv_TunionRes){
goto _LL209;} else{ goto _LL210;} _LL210: if(*(( int*) _temp202) ==  Cyc_Tcenv_EnumRes){
goto _LL211;} else{ goto _LL203;} _LL205: { struct Cyc_Absyn_Funname_b_struct*
_temp214=({ struct Cyc_Absyn_Funname_b_struct* _temp219=( struct Cyc_Absyn_Funname_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp219[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp220; _temp220.tag= Cyc_Absyn_Funname_b; _temp220.f1=
fd; _temp220;}); _temp219;}); void* _temp215= Cyc_Tcdecl_merge_binding( _temp212,(
void*) _temp214, loc, Cyc_Tc_tc_msg); if( _temp215 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL203;} if( _temp215 ==  _temp212?(* _temp201).f2: 0){ goto _LL203;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
_tuple4* v)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple4* _temp216=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp216->f1=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp217=( struct Cyc_Tcenv_VarRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp217[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp218; _temp218.tag= Cyc_Tcenv_VarRes; _temp218.f1=( void*) _temp215;
_temp218;}); _temp217;}); _temp216->f2= 1; _temp216;})); goto _LL203;} _LL207:({
void* _temp221[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous struct declaration",
sizeof( unsigned char), 57u), _tag_arr( _temp221, sizeof( void*), 0u));}); goto
_LL203; _LL209:({ void* _temp222[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u), _tag_arr( _temp222, sizeof( void*), 0u));}); goto
_LL203; _LL211:({ void* _temp223[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous enum tag",
sizeof( unsigned char), 47u), _tag_arr( _temp223, sizeof( void*), 0u));}); goto
_LL203; _LL203:;}; _pop_handler();} else{ void* _temp199=( void*) _exn_thrown;
void* _temp225= _temp199; _LL227: if( _temp225 ==  Cyc_Dict_Absent){ goto _LL228;}
else{ goto _LL229;} _LL229: goto _LL230; _LL228: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple4* _temp231=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp231->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp232=( struct Cyc_Tcenv_VarRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp232[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp233; _temp233.tag= Cyc_Tcenv_VarRes;
_temp233.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp234=(
struct Cyc_Absyn_Funname_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp234[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp235; _temp235.tag= Cyc_Absyn_Funname_b;
_temp235.f1= fd; _temp235;}); _temp234;})); _temp233;}); _temp232;}); _temp231->f2=
0; _temp231;})); goto _LL226; _LL230:( void) _throw( _temp225); _LL226:;}}} te->le=({
struct Cyc_Core_Opt* _temp236=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp236->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp237=( struct Cyc_Tcenv_Outermost_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp237[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp238; _temp238.tag= Cyc_Tcenv_Outermost;
_temp238.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp238;}); _temp237;}));
_temp236;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){({ void* _temp239[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u), _tag_arr( _temp239, sizeof( void*), 0u));});} te->le=
0;}} static void Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_arr* v=(* td->name).f2;{ void* _temp240=(* td->name).f1; struct Cyc_List_List*
_temp248; struct Cyc_List_List* _temp250; _LL242: if(( unsigned int) _temp240 > 
1u?*(( int*) _temp240) ==  Cyc_Absyn_Rel_n: 0){ _LL249: _temp248=(( struct Cyc_Absyn_Rel_n_struct*)
_temp240)->f1; if( _temp248 ==  0){ goto _LL243;} else{ goto _LL244;}} else{
goto _LL244;} _LL244: if(( unsigned int) _temp240 >  1u?*(( int*) _temp240) == 
Cyc_Absyn_Abs_n: 0){ _LL251: _temp250=(( struct Cyc_Absyn_Abs_n_struct*)
_temp240)->f1; if( _temp250 ==  0){ goto _LL245;} else{ goto _LL246;}} else{
goto _LL246;} _LL246: goto _LL247; _LL243: goto _LL241; _LL245: goto _LL241;
_LL247:({ struct Cyc_Std_String_pa_struct _temp253; _temp253.tag= Cyc_Std_String_pa;
_temp253.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( td->name);{ void*
_temp252[ 1u]={& _temp253}; Cyc_Tcutil_terr( loc, _tag_arr("qualified typedef declarations are not implemented (%s)",
sizeof( unsigned char), 56u), _tag_arr( _temp252, sizeof( void*), 1u));}});
return; _LL241:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k))
Cyc_Dict_member)( ge->typedefs, v)){({ struct Cyc_Std_String_pa_struct _temp255;
_temp255.tag= Cyc_Std_String_pa; _temp255.f1=( struct _tagged_arr)* v;{ void*
_temp254[ 1u]={& _temp255}; Cyc_Tcutil_terr( loc, _tag_arr("redeclaration of typedef %s",
sizeof( unsigned char), 28u), _tag_arr( _temp254, sizeof( void*), 1u));}});
return;}(* td->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp256=(
struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp256[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp257; _temp257.tag= Cyc_Absyn_Abs_n;
_temp257.f1= te->ns; _temp257;}); _temp256;}); Cyc_Tcutil_check_unique_tvars(
loc, td->tvs); Cyc_Tcutil_add_tvar_identities( td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind, 0,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Typedefdecl* v)) Cyc_Dict_insert)( ge->typedefs, v, td);} static void
Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct _tagged_arr obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp258= _new_region();
struct _RegionHandle* uprev_rgn=& _temp258; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp259= fields; for( 0;
_temp259 !=  0; _temp259=(( struct Cyc_List_List*) _check_null( _temp259))->tl){
struct Cyc_Absyn_Structfield _temp262; struct Cyc_List_List* _temp263; struct
Cyc_Absyn_Exp* _temp265; void* _temp267; struct Cyc_Absyn_Tqual _temp269; struct
_tagged_arr* _temp271; struct Cyc_Absyn_Structfield* _temp260=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp259))->hd; _temp262=* _temp260; _LL272:
_temp271= _temp262.name; goto _LL270; _LL270: _temp269= _temp262.tq; goto _LL268;
_LL268: _temp267=( void*) _temp262.type; goto _LL266; _LL266: _temp265= _temp262.width;
goto _LL264; _LL264: _temp263= _temp262.attributes; goto _LL261; _LL261: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,
_temp271)){({ struct Cyc_Std_String_pa_struct _temp275; _temp275.tag= Cyc_Std_String_pa;
_temp275.f1=( struct _tagged_arr) obj;{ struct Cyc_Std_String_pa_struct _temp274;
_temp274.tag= Cyc_Std_String_pa; _temp274.f1=( struct _tagged_arr)* _temp271;{
void* _temp273[ 2u]={& _temp274,& _temp275}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in %s",
sizeof( unsigned char), 25u), _tag_arr( _temp273, sizeof( void*), 2u));}}});}
if( Cyc_Std_strcmp(* _temp271, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp276=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp276->hd=( void*)
_temp271; _temp276->tl= prev_fields; _temp276;});} Cyc_Tcutil_check_type( loc,
te, tvs,( void*) Cyc_Absyn_MemKind, 0, _temp267); Cyc_Tcutil_check_bitfield( loc,
te, _temp267, _temp265, _temp271);}}; _pop_region( uprev_rgn);} static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct _tagged_arr obj,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp277=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; if( sd->name ==  0){({ struct Cyc_Std_String_pa_struct
_temp279; _temp279.tag= Cyc_Std_String_pa; _temp279.f1=( struct _tagged_arr) obj;{
void* _temp278[ 1u]={& _temp279}; Cyc_Tcutil_terr( loc, _tag_arr("anonymous %ss are not allowed at top level",
sizeof( unsigned char), 43u), _tag_arr( _temp278, sizeof( void*), 1u));}});
return;}{ struct Cyc_List_List* atts= sd->attributes; for( 0; atts !=  0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp280=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL282: if( _temp280 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL283;} else{ goto _LL284;} _LL284: if((
unsigned int) _temp280 >  16u?*(( int*) _temp280) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL285;} else{ goto _LL286;} _LL286: goto _LL287; _LL283: continue; _LL285:
continue; _LL287:({ struct Cyc_Std_String_pa_struct _temp291; _temp291.tag= Cyc_Std_String_pa;
_temp291.f1=( struct _tagged_arr)* _temp277;{ struct Cyc_Std_String_pa_struct
_temp290; _temp290.tag= Cyc_Std_String_pa; _temp290.f1=( struct _tagged_arr) obj;{
struct Cyc_Std_String_pa_struct _temp289; _temp289.tag= Cyc_Std_String_pa;
_temp289.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp288[ 3u]={& _temp289,&
_temp290,& _temp291}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s in %s %s definition",
sizeof( unsigned char), 37u), _tag_arr( _temp288, sizeof( void*), 3u));}}}});
goto _LL281; _LL281:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs !=  0;
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref*
_temp292= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); void* _temp293=( void*) _temp292->v; void*
_temp301; _LL295: if( _temp293 == ( void*) Cyc_Absyn_No_constr){ goto _LL296;}
else{ goto _LL297;} _LL297: if(( unsigned int) _temp293 >  1u?*(( int*) _temp293)
==  Cyc_Absyn_Eq_constr: 0){ _LL302: _temp301=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp293)->f1; if( _temp301 == ( void*) Cyc_Absyn_MemKind){ goto _LL298;} else{
goto _LL299;}} else{ goto _LL299;} _LL299: goto _LL300; _LL296:( void*)(
_temp292->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp303=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp303[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp304; _temp304.tag= Cyc_Absyn_Eq_constr;
_temp304.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp304;}); _temp303;})));
continue; _LL298:({ struct Cyc_Std_String_pa_struct _temp308; _temp308.tag= Cyc_Std_String_pa;
_temp308.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->name;{ struct Cyc_Std_String_pa_struct _temp307;
_temp307.tag= Cyc_Std_String_pa; _temp307.f1=( struct _tagged_arr)* _temp277;{
struct Cyc_Std_String_pa_struct _temp306; _temp306.tag= Cyc_Std_String_pa;
_temp306.f1=( struct _tagged_arr) obj;{ void* _temp305[ 3u]={& _temp306,&
_temp307,& _temp308}; Cyc_Tcutil_terr( loc, _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u), _tag_arr( _temp305, sizeof( void*), 3u));}}}});
continue; _LL300: continue; _LL294:;}}{ void* _temp309=(*(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1; struct Cyc_List_List*
_temp317; struct Cyc_List_List* _temp319; _LL311: if(( unsigned int) _temp309 > 
1u?*(( int*) _temp309) ==  Cyc_Absyn_Rel_n: 0){ _LL318: _temp317=(( struct Cyc_Absyn_Rel_n_struct*)
_temp309)->f1; if( _temp317 ==  0){ goto _LL312;} else{ goto _LL313;}} else{
goto _LL313;} _LL313: if(( unsigned int) _temp309 >  1u?*(( int*) _temp309) == 
Cyc_Absyn_Abs_n: 0){ _LL320: _temp319=(( struct Cyc_Absyn_Abs_n_struct*)
_temp309)->f1; if( _temp319 ==  0){ goto _LL314;} else{ goto _LL315;}} else{
goto _LL315;} _LL315: goto _LL316; _LL312: goto _LL310; _LL314: goto _LL310;
_LL316:({ struct Cyc_Std_String_pa_struct _temp322; _temp322.tag= Cyc_Std_String_pa;
_temp322.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( sd->name))->v);{ void* _temp321[ 1u]={&
_temp322}; Cyc_Tcutil_terr( loc, _tag_arr("qualified struct declarations are not implemented (%s)",
sizeof( unsigned char), 55u), _tag_arr( _temp321, sizeof( void*), 1u));}});
return; _LL310:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp323=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp323[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp324; _temp324.tag= Cyc_Absyn_Abs_n; _temp324.f1= te->ns; _temp324;});
_temp323;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs); Cyc_Tcutil_add_tvar_identities(
sd->tvs);} struct _tuple5{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp325=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; const unsigned char* _temp326="struct"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp326, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List*
_temp327= sd->tvs;{ struct _tuple5 _temp329=({ struct _tuple5 _temp328; _temp328.f1=
sd->fields; _temp328.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp325);
_temp328;}); struct Cyc_Core_Opt* _temp339; struct Cyc_Core_Opt* _temp341;
struct Cyc_Core_Opt* _temp343; struct Cyc_Core_Opt* _temp345; struct Cyc_Core_Opt
_temp347; struct Cyc_List_List* _temp348; struct Cyc_Core_Opt* _temp350; struct
Cyc_Core_Opt _temp352; struct Cyc_Absyn_Structdecl** _temp353; struct Cyc_Core_Opt*
_temp355; struct Cyc_Core_Opt _temp357; struct Cyc_List_List* _temp358; struct
Cyc_Core_Opt* _temp360; struct Cyc_Core_Opt _temp362; struct Cyc_Absyn_Structdecl**
_temp363; struct Cyc_Core_Opt* _temp365; _LL331: _LL342: _temp341= _temp329.f1;
if( _temp341 ==  0){ goto _LL340;} else{ goto _LL333;} _LL340: _temp339=
_temp329.f2; if( _temp339 ==  0){ goto _LL332;} else{ goto _LL333;} _LL333:
_LL346: _temp345= _temp329.f1; if( _temp345 ==  0){ goto _LL335;} else{ _temp347=*
_temp345; _LL349: _temp348=( struct Cyc_List_List*) _temp347.v; goto _LL344;}
_LL344: _temp343= _temp329.f2; if( _temp343 ==  0){ goto _LL334;} else{ goto
_LL335;} _LL335: _LL356: _temp355= _temp329.f1; if( _temp355 ==  0){ goto _LL337;}
else{ _temp357=* _temp355; _LL359: _temp358=( struct Cyc_List_List*) _temp357.v;
goto _LL351;} _LL351: _temp350= _temp329.f2; if( _temp350 ==  0){ goto _LL337;}
else{ _temp352=* _temp350; _LL354: _temp353=( struct Cyc_Absyn_Structdecl**)
_temp352.v; goto _LL336;} _LL337: _LL366: _temp365= _temp329.f1; if( _temp365 == 
0){ goto _LL361;} else{ goto _LL330;} _LL361: _temp360= _temp329.f2; if(
_temp360 ==  0){ goto _LL330;} else{ _temp362=* _temp360; _LL364: _temp363=(
struct Cyc_Absyn_Structdecl**) _temp362.v; goto _LL338;} _LL332: ge->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Structdecl** v)) Cyc_Dict_insert)( ge->structdecls, _temp325,({ struct
Cyc_Absyn_Structdecl** _temp367=( struct Cyc_Absyn_Structdecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp367[ 0]= sd; _temp367;})); goto
_LL330; _LL334: { struct Cyc_Absyn_Structdecl** _temp368=({ struct Cyc_Absyn_Structdecl**
_temp369=( struct Cyc_Absyn_Structdecl**) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp369[ 0]=({ struct Cyc_Absyn_Structdecl* _temp370=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp370->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp370->name= sd->name; _temp370->tvs= _temp327; _temp370->fields=
0; _temp370->attributes= 0; _temp370;}); _temp369;}); ge->structdecls=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl**
v)) Cyc_Dict_insert)( ge->structdecls, _temp325, _temp368); Cyc_Tc_tcStructFields(
te, ge, loc, _tag_arr( _temp326, sizeof( unsigned char), 7u), _temp348, _temp327);*
_temp368= sd; goto _LL330;} _LL336: { struct Cyc_Absyn_Structdecl* _temp371=*
_temp353;* _temp353=({ struct Cyc_Absyn_Structdecl* _temp372=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp372->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp372->name= sd->name; _temp372->tvs= _temp327; _temp372->fields=
0; _temp372->attributes= 0; _temp372;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp326, sizeof( unsigned char), 7u), _temp358, _temp327);* _temp353=
_temp371; _temp363= _temp353; goto _LL338;} _LL338: { struct Cyc_Absyn_Structdecl*
_temp373= Cyc_Tcdecl_merge_structdecl(* _temp363, sd, loc, Cyc_Tc_tc_msg); if(
_temp373 ==  0){ return;} else{* _temp363=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp373); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp373);
goto _LL330;}} _LL330:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)( ge->ordinaries,
_temp325,({ struct _tuple4* _temp374=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp374->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp375=( struct Cyc_Tcenv_StructRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp375[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp376; _temp376.tag= Cyc_Tcenv_StructRes;
_temp376.f1= sd; _temp376;}); _temp375;}); _temp374->f2= 1; _temp374;}));}} void
Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct
Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct _tagged_arr*
_temp377=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)).f2;
const unsigned char* _temp378="union"; Cyc_Tc_tcStructOrUniondecl( te, ge,
_tag_arr( _temp378, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp379= ud->tvs; struct _tuple5 _temp381=({ struct
_tuple5 _temp380; _temp380.f1= ud->fields; _temp380.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp377); _temp380;}); struct Cyc_Core_Opt* _temp391; struct Cyc_Core_Opt*
_temp393; struct Cyc_Core_Opt* _temp395; struct Cyc_Core_Opt* _temp397; struct
Cyc_Core_Opt _temp399; struct Cyc_List_List* _temp400; struct Cyc_Core_Opt*
_temp402; struct Cyc_Core_Opt _temp404; struct Cyc_Absyn_Uniondecl** _temp405;
struct Cyc_Core_Opt* _temp407; struct Cyc_Core_Opt _temp409; struct Cyc_List_List*
_temp410; struct Cyc_Core_Opt* _temp412; struct Cyc_Core_Opt _temp414; struct
Cyc_Absyn_Uniondecl** _temp415; struct Cyc_Core_Opt* _temp417; _LL383: _LL394:
_temp393= _temp381.f1; if( _temp393 ==  0){ goto _LL392;} else{ goto _LL385;}
_LL392: _temp391= _temp381.f2; if( _temp391 ==  0){ goto _LL384;} else{ goto
_LL385;} _LL385: _LL398: _temp397= _temp381.f1; if( _temp397 ==  0){ goto _LL387;}
else{ _temp399=* _temp397; _LL401: _temp400=( struct Cyc_List_List*) _temp399.v;
goto _LL396;} _LL396: _temp395= _temp381.f2; if( _temp395 ==  0){ goto _LL386;}
else{ goto _LL387;} _LL387: _LL408: _temp407= _temp381.f1; if( _temp407 ==  0){
goto _LL389;} else{ _temp409=* _temp407; _LL411: _temp410=( struct Cyc_List_List*)
_temp409.v; goto _LL403;} _LL403: _temp402= _temp381.f2; if( _temp402 ==  0){
goto _LL389;} else{ _temp404=* _temp402; _LL406: _temp405=( struct Cyc_Absyn_Uniondecl**)
_temp404.v; goto _LL388;} _LL389: _LL418: _temp417= _temp381.f1; if( _temp417 == 
0){ goto _LL413;} else{ goto _LL382;} _LL413: _temp412= _temp381.f2; if(
_temp412 ==  0){ goto _LL382;} else{ _temp414=* _temp412; _LL416: _temp415=(
struct Cyc_Absyn_Uniondecl**) _temp414.v; goto _LL390;} _LL384: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Uniondecl** v)) Cyc_Dict_insert)( ge->uniondecls, _temp377,({ struct
Cyc_Absyn_Uniondecl** _temp419=( struct Cyc_Absyn_Uniondecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp419[ 0]= ud; _temp419;})); goto
_LL382; _LL386: { struct Cyc_Absyn_Uniondecl** _temp420=({ struct Cyc_Absyn_Uniondecl**
_temp425=( struct Cyc_Absyn_Uniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp425[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp426=( struct Cyc_Absyn_Uniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp426->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp426->name= ud->name; _temp426->tvs= _temp379; _temp426->fields=
0; _temp426->attributes= ud->attributes; _temp426;}); _temp425;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Uniondecl** v)) Cyc_Dict_insert)( ge->uniondecls, _temp377, _temp420);
Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp378, sizeof( unsigned char),
6u), _temp400, _temp379);{ struct Cyc_List_List* f= _temp400; for( 0; f !=  0; f=((
struct Cyc_List_List*) _check_null( f))->tl){ if( ! Cyc_Tcutil_bits_only(( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type)){({
struct Cyc_Std_String_pa_struct _temp424; _temp424.tag= Cyc_Std_String_pa;
_temp424.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( f))->hd)->type);{ struct Cyc_Std_String_pa_struct
_temp423; _temp423.tag= Cyc_Std_String_pa; _temp423.f1=( struct _tagged_arr)*
_temp377;{ struct Cyc_Std_String_pa_struct _temp422; _temp422.tag= Cyc_Std_String_pa;
_temp422.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( f))->hd)->name;{ void* _temp421[ 3u]={& _temp422,& _temp423,&
_temp424}; Cyc_Tcutil_terr( loc, _tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof( unsigned char), 58u), _tag_arr( _temp421, sizeof( void*), 3u));}}}});}}}*
_temp420= ud; goto _LL382;} _LL388: { struct Cyc_Absyn_Uniondecl* _temp427=*
_temp405;* _temp405=({ struct Cyc_Absyn_Uniondecl* _temp428=( struct Cyc_Absyn_Uniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp428->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp428->name= ud->name; _temp428->tvs= _temp379; _temp428->fields=
0; _temp428->attributes= ud->attributes; _temp428;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp378, sizeof( unsigned char), 6u), _temp410, _temp379);*
_temp405= _temp427; _temp415= _temp405; goto _LL390;} _LL390: { struct Cyc_Absyn_Uniondecl*
_temp429= Cyc_Tcdecl_merge_uniondecl(* _temp415, ud, loc, Cyc_Tc_tc_msg); if(
_temp429 ==  0){ return;} else{* _temp415=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp429); goto _LL382;}} _LL382:;}} struct _tuple6{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp430= fields; for( 0; _temp430 !=  0; _temp430=(( struct Cyc_List_List*)
_check_null( _temp430))->tl){ struct Cyc_Absyn_Tunionfield* _temp431=( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp430))->hd;{
struct Cyc_List_List* tvs= _temp431->tvs; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* _temp432=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; struct Cyc_Absyn_Conref* _temp433=
Cyc_Absyn_compress_conref( _temp432->kind); void* _temp434=( void*) _temp433->v;
void* _temp442; _LL436: if( _temp434 == ( void*) Cyc_Absyn_No_constr){ goto
_LL437;} else{ goto _LL438;} _LL438: if(( unsigned int) _temp434 >  1u?*(( int*)
_temp434) ==  Cyc_Absyn_Eq_constr: 0){ _LL443: _temp442=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp434)->f1; if( _temp442 == ( void*) Cyc_Absyn_MemKind){ goto _LL439;} else{
goto _LL440;}} else{ goto _LL440;} _LL440: goto _LL441; _LL437:( void*)(
_temp433->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp444=(
struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp444[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp445; _temp445.tag= Cyc_Absyn_Eq_constr;
_temp445.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp445;}); _temp444;})));
goto _LL435; _LL439:({ struct Cyc_Std_String_pa_struct _temp447; _temp447.tag=
Cyc_Std_String_pa; _temp447.f1=( struct _tagged_arr)*(* _temp431->name).f2;{
void* _temp446[ 1u]={& _temp447}; Cyc_Tcutil_terr( _temp431->loc, _tag_arr("field %s abstracts type variable of kind M",
sizeof( unsigned char), 43u), _tag_arr( _temp446, sizeof( void*), 1u));}}); goto
_LL435; _LL441: goto _LL435; _LL435:;}}{ struct Cyc_List_List* alltvs=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs, _temp431->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs); Cyc_Tcutil_add_tvar_identities(
_temp431->tvs);{ struct Cyc_List_List* typs= _temp431->typs; for( 0; typs !=  0;
typs=(( struct Cyc_List_List*) _check_null( typs))->tl){ Cyc_Tcutil_check_type(
_temp431->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, 0,(*(( struct _tuple6*)((
struct Cyc_List_List*) _check_null( typs))->hd)).f2);}}{ void* _temp448=(*
_temp431->name).f1; struct Cyc_List_List* _temp458; _LL450: if(( unsigned int)
_temp448 >  1u?*(( int*) _temp448) ==  Cyc_Absyn_Rel_n: 0){ _LL459: _temp458=((
struct Cyc_Absyn_Rel_n_struct*) _temp448)->f1; if( _temp458 ==  0){ goto _LL451;}
else{ goto _LL452;}} else{ goto _LL452;} _LL452: if(( unsigned int) _temp448 > 
1u?*(( int*) _temp448) ==  Cyc_Absyn_Rel_n: 0){ goto _LL453;} else{ goto _LL454;}
_LL454: if(( unsigned int) _temp448 >  1u?*(( int*) _temp448) ==  Cyc_Absyn_Abs_n:
0){ goto _LL455;} else{ goto _LL456;} _LL456: if( _temp448 == ( void*) Cyc_Absyn_Loc_n){
goto _LL457;} else{ goto _LL449;} _LL451: if( is_xtunion){(* _temp431->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp460=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp460[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp461; _temp461.tag= Cyc_Absyn_Abs_n; _temp461.f1= te->ns; _temp461;});
_temp460;});} else{(* _temp431->name).f1=(* name).f1;} goto _LL449; _LL453:({
struct Cyc_Std_String_pa_struct _temp463; _temp463.tag= Cyc_Std_String_pa;
_temp463.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp431->name);{
void* _temp462[ 1u]={& _temp463}; Cyc_Tcutil_terr( _temp431->loc, _tag_arr("qualified tunionfield declarations are not allowed (%s)",
sizeof( unsigned char), 56u), _tag_arr( _temp462, sizeof( void*), 1u));}}); goto
_LL449; _LL455: goto _LL449; _LL457:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp464=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp464[ 0]=({ struct Cyc_Core_Impossible_struct _temp465; _temp465.tag= Cyc_Core_Impossible;
_temp465.f1= _tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u);
_temp465;}); _temp464;})); goto _LL449; _LL449:;}}}}{ struct Cyc_List_List*
fields2; if( is_xtunion){ int _temp466= 1; struct Cyc_List_List* _temp467= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp466,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp466){ fields2=
_temp467;} else{ fields2= 0;}} else{ struct _RegionHandle _temp468= _new_region();
struct _RegionHandle* uprev_rgn=& _temp468; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs != 
0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Tunionfield*
_temp469=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fs))->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp,
prev_fields,(* _temp469->name).f2)){({ struct Cyc_Std_String_pa_struct _temp472;
_temp472.tag= Cyc_Std_String_pa; _temp472.f1=( struct _tagged_arr) obj;{ struct
Cyc_Std_String_pa_struct _temp471; _temp471.tag= Cyc_Std_String_pa; _temp471.f1=(
struct _tagged_arr)*(* _temp469->name).f2;{ void* _temp470[ 2u]={& _temp471,&
_temp472}; Cyc_Tcutil_terr( _temp469->loc, _tag_arr("duplicate field name %s in %s",
sizeof( unsigned char), 30u), _tag_arr( _temp470, sizeof( void*), 2u));}}});}
else{ prev_fields=({ struct Cyc_List_List* _temp473=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp473->hd=( void*)(*
_temp469->name).f2; _temp473->tl= prev_fields; _temp473;});} if(( void*)
_temp469->sc != ( void*) Cyc_Absyn_Public){({ struct Cyc_Std_String_pa_struct
_temp475; _temp475.tag= Cyc_Std_String_pa; _temp475.f1=( struct _tagged_arr)*(*
_temp469->name).f2;{ void* _temp474[ 1u]={& _temp475}; Cyc_Tcutil_warn( loc,
_tag_arr("ignoring scope of field %s", sizeof( unsigned char), 27u), _tag_arr(
_temp474, sizeof( void*), 1u));}});( void*)( _temp469->sc=( void*)(( void*) Cyc_Absyn_Public));}}}
fields2= fields;}; _pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp476=
fields; for( 0; _temp476 !=  0; _temp476=(( struct Cyc_List_List*) _check_null(
_temp476))->tl){ struct Cyc_Absyn_Tunionfield* _temp477=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp476))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp477->name).f2,({ struct _tuple4* _temp478=( struct
_tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp478->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp479=( struct Cyc_Tcenv_TunionRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp479[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp480; _temp480.tag= Cyc_Tcenv_TunionRes; _temp480.f1=
tudres; _temp480.f2= _temp477; _temp480;}); _temp479;}); _temp478->f2= 1;
_temp478;}));}} return fields2;}} void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->kind); void* _temp481=( void*)
c->v; void* _temp489; _LL483: if( _temp481 == ( void*) Cyc_Absyn_No_constr){
goto _LL484;} else{ goto _LL485;} _LL485: if(( unsigned int) _temp481 >  1u?*((
int*) _temp481) ==  Cyc_Absyn_Eq_constr: 0){ _LL490: _temp489=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp481)->f1; if( _temp489 == ( void*) Cyc_Absyn_MemKind){
goto _LL486;} else{ goto _LL487;}} else{ goto _LL487;} _LL487: goto _LL488;
_LL484:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp491=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp491[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp492; _temp492.tag= Cyc_Absyn_Eq_constr;
_temp492.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp492;}); _temp491;})));
goto _LL482; _LL486:({ struct Cyc_Std_String_pa_struct _temp496; _temp496.tag=
Cyc_Std_String_pa; _temp496.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->name;{ struct Cyc_Std_String_pa_struct
_temp495; _temp495.tag= Cyc_Std_String_pa; _temp495.f1=( struct _tagged_arr)* v;{
struct Cyc_Std_String_pa_struct _temp494; _temp494.tag= Cyc_Std_String_pa;
_temp494.f1=( struct _tagged_arr) obj;{ void* _temp493[ 3u]={& _temp494,&
_temp495,& _temp496}; Cyc_Tcutil_terr( loc, _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u), _tag_arr( _temp493, sizeof( void*), 3u));}}}});
goto _LL482; _LL488: goto _LL482; _LL482:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_Core_Opt* tud_opt; if(
tud->is_xtunion){{ struct _handler_cons _temp497; _push_handler(& _temp497);{
int _temp499= 0; if( setjmp( _temp497.handler)){ _temp499= 1;} if( ! _temp499){
tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc, tud->name);; _pop_handler();}
else{ void* _temp498=( void*) _exn_thrown; void* _temp501= _temp498; _LL503: if(
_temp501 ==  Cyc_Dict_Absent){ goto _LL504;} else{ goto _LL505;} _LL505: goto
_LL506; _LL504:({ struct Cyc_Std_String_pa_struct _temp508; _temp508.tag= Cyc_Std_String_pa;
_temp508.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tud->name);{ void*
_temp507[ 1u]={& _temp508}; Cyc_Tcutil_terr( loc, _tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof( unsigned char), 60u), _tag_arr( _temp507, sizeof( void*), 1u));}});
return; _LL506:( void) _throw( _temp501); _LL502:;}}} if( tud_opt !=  0){ tud->name=(*((
struct Cyc_Absyn_Tuniondecl**)(( struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;}
else{(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp509=(
struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp509[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp510; _temp510.tag= Cyc_Absyn_Abs_n;
_temp510.f1= te->ns; _temp510;}); _temp509;});}} else{{ void* _temp511=(* tud->name).f1;
struct Cyc_List_List* _temp519; _LL513: if(( unsigned int) _temp511 >  1u?*((
int*) _temp511) ==  Cyc_Absyn_Rel_n: 0){ _LL520: _temp519=(( struct Cyc_Absyn_Rel_n_struct*)
_temp511)->f1; if( _temp519 ==  0){ goto _LL514;} else{ goto _LL515;}} else{
goto _LL515;} _LL515: if(( unsigned int) _temp511 >  1u?*(( int*) _temp511) == 
Cyc_Absyn_Abs_n: 0){ goto _LL516;} else{ goto _LL517;} _LL517: goto _LL518;
_LL514:(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp521=(
struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp521[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp522; _temp522.tag= Cyc_Absyn_Abs_n;
_temp522.f1= te->ns; _temp522;}); _temp521;}); goto _LL512; _LL516: goto _LL518;
_LL518:({ struct Cyc_Std_String_pa_struct _temp524; _temp524.tag= Cyc_Std_String_pa;
_temp524.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tud->name);{ void*
_temp523[ 1u]={& _temp524}; Cyc_Tcutil_terr( loc, _tag_arr("qualified tunion declarations are not implemented (%s)",
sizeof( unsigned char), 55u), _tag_arr( _temp523, sizeof( void*), 1u));}});
return; _LL512:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple5 _temp526=({ struct _tuple5 _temp525; _temp525.f1= tud->fields; _temp525.f2=
tud_opt; _temp525;}); struct Cyc_Core_Opt* _temp536; struct Cyc_Core_Opt*
_temp538; struct Cyc_Core_Opt* _temp540; struct Cyc_Core_Opt* _temp542; struct
Cyc_Core_Opt _temp544; struct Cyc_List_List* _temp545; struct Cyc_List_List**
_temp547; struct Cyc_Core_Opt* _temp548; struct Cyc_Core_Opt _temp550; struct
Cyc_Absyn_Tuniondecl** _temp551; struct Cyc_Core_Opt* _temp553; struct Cyc_Core_Opt
_temp555; struct Cyc_List_List* _temp556; struct Cyc_List_List** _temp558;
struct Cyc_Core_Opt* _temp559; struct Cyc_Core_Opt _temp561; struct Cyc_Absyn_Tuniondecl**
_temp562; struct Cyc_Core_Opt* _temp564; _LL528: _LL539: _temp538= _temp526.f1;
if( _temp538 ==  0){ goto _LL537;} else{ goto _LL530;} _LL537: _temp536=
_temp526.f2; if( _temp536 ==  0){ goto _LL529;} else{ goto _LL530;} _LL530:
_LL543: _temp542= _temp526.f1; if( _temp542 ==  0){ goto _LL532;} else{ _temp544=*
_temp542; _LL546: _temp545=( struct Cyc_List_List*) _temp544.v; _temp547=(
struct Cyc_List_List**)&(* _temp526.f1).v; goto _LL541;} _LL541: _temp540=
_temp526.f2; if( _temp540 ==  0){ goto _LL531;} else{ goto _LL532;} _LL532:
_LL554: _temp553= _temp526.f1; if( _temp553 ==  0){ goto _LL534;} else{ _temp555=*
_temp553; _LL557: _temp556=( struct Cyc_List_List*) _temp555.v; _temp558=(
struct Cyc_List_List**)&(* _temp526.f1).v; goto _LL549;} _LL549: _temp548=
_temp526.f2; if( _temp548 ==  0){ goto _LL534;} else{ _temp550=* _temp548;
_LL552: _temp551=( struct Cyc_Absyn_Tuniondecl**) _temp550.v; goto _LL533;}
_LL534: _LL565: _temp564= _temp526.f1; if( _temp564 ==  0){ goto _LL560;} else{
goto _LL527;} _LL560: _temp559= _temp526.f2; if( _temp559 ==  0){ goto _LL527;}
else{ _temp561=* _temp559; _LL563: _temp562=( struct Cyc_Absyn_Tuniondecl**)
_temp561.v; goto _LL535;} _LL529: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Tuniondecl** v))
Cyc_Dict_insert)( ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp566=(
struct Cyc_Absyn_Tuniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp566[ 0]= tud; _temp566;})); goto _LL527; _LL531: { struct Cyc_Absyn_Tuniondecl**
_temp567=({ struct Cyc_Absyn_Tuniondecl** _temp568=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp568[ 0]=({ struct Cyc_Absyn_Tuniondecl*
_temp569=( struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp569->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp569->name= tud->name;
_temp569->tvs= tvs; _temp569->fields= 0; _temp569->is_xtunion= tud->is_xtunion;
_temp569;}); _temp568;}); ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Tuniondecl** v)) Cyc_Dict_insert)(
ge->tuniondecls, v, _temp567);* _temp547= Cyc_Tc_tcTunionFields( te, ge, loc,
obj, tud->is_xtunion, tud->name,* _temp547, tvs, tud);* _temp567= tud; goto
_LL527;} _LL533: { struct Cyc_Absyn_Tuniondecl* _temp570=* _temp551;* _temp551=({
struct Cyc_Absyn_Tuniondecl* _temp571=( struct Cyc_Absyn_Tuniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp571->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp571->name= tud->name; _temp571->tvs= tvs; _temp571->fields= 0; _temp571->is_xtunion=
tud->is_xtunion; _temp571;});* _temp558= Cyc_Tc_tcTunionFields( te, ge, loc, obj,
tud->is_xtunion, tud->name,* _temp558, tvs, tud);* _temp551= _temp570; _temp562=
_temp551; goto _LL535;} _LL535: { struct Cyc_Absyn_Tuniondecl* _temp572= Cyc_Tcdecl_merge_tuniondecl(*
_temp562, tud, loc, Cyc_Tc_tc_msg); if( _temp572 ==  0){ return;} else{*
_temp562=( struct Cyc_Absyn_Tuniondecl*) _check_null( _temp572); goto _LL527;}}
_LL527:;}}}} void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl* ed){ struct
_tagged_arr* v=(* ed->name).f2;{ void* _temp573=(* ed->name).f1; struct Cyc_List_List*
_temp581; struct Cyc_List_List* _temp583; _LL575: if(( unsigned int) _temp573 > 
1u?*(( int*) _temp573) ==  Cyc_Absyn_Rel_n: 0){ _LL582: _temp581=(( struct Cyc_Absyn_Rel_n_struct*)
_temp573)->f1; if( _temp581 ==  0){ goto _LL576;} else{ goto _LL577;}} else{
goto _LL577;} _LL577: if(( unsigned int) _temp573 >  1u?*(( int*) _temp573) == 
Cyc_Absyn_Abs_n: 0){ _LL584: _temp583=(( struct Cyc_Absyn_Abs_n_struct*)
_temp573)->f1; if( _temp583 ==  0){ goto _LL578;} else{ goto _LL579;}} else{
goto _LL579;} _LL579: goto _LL580; _LL576: goto _LL574; _LL578: goto _LL574;
_LL580:({ struct Cyc_Std_String_pa_struct _temp586; _temp586.tag= Cyc_Std_String_pa;
_temp586.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( ed->name);{ void*
_temp585[ 1u]={& _temp586}; Cyc_Tcutil_terr( loc, _tag_arr("qualified enum declarations are not implemented (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp585, sizeof( void*), 1u));}});
return; _LL574:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp587=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp587[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp588; _temp588.tag= Cyc_Absyn_Abs_n;
_temp588.f1= te->ns; _temp588;}); _temp587;}); if( ed->fields !=  0){ struct
_RegionHandle _temp589= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp589; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs !=  0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp590=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp,
prev_fields,(* _temp590->name).f2)){({ struct Cyc_Std_String_pa_struct _temp592;
_temp592.tag= Cyc_Std_String_pa; _temp592.f1=( struct _tagged_arr)*(* _temp590->name).f2;{
void* _temp591[ 1u]={& _temp592}; Cyc_Tcutil_terr( _temp590->loc, _tag_arr("duplicate field name %s",
sizeof( unsigned char), 24u), _tag_arr( _temp591, sizeof( void*), 1u));}});}
else{ prev_fields=({ struct Cyc_List_List* _temp593=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp593->hd=( void*)(*
_temp590->name).f2; _temp593->tl= prev_fields; _temp593;});} if( _temp590->tag
==  0){ _temp590->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count,
_temp590->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp590->tag))){({ struct Cyc_Std_String_pa_struct _temp596;
_temp596.tag= Cyc_Std_String_pa; _temp596.f1=( struct _tagged_arr)*(* _temp590->name).f2;{
struct Cyc_Std_String_pa_struct _temp595; _temp595.tag= Cyc_Std_String_pa;
_temp595.f1=( struct _tagged_arr)* v;{ void* _temp594[ 2u]={& _temp595,&
_temp596}; Cyc_Tcutil_terr( loc, _tag_arr("enum %s, field %s: expression is not constant",
sizeof( unsigned char), 46u), _tag_arr( _temp594, sizeof( void*), 2u));}}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp590->tag)); tag_count= t1 +  1;(* _temp590->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp597=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp597[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp598; _temp598.tag= Cyc_Absyn_Abs_n; _temp598.f1= te->ns; _temp598;});
_temp597;});}}}; _pop_region( uprev_rgn);}{ struct _handler_cons _temp599;
_push_handler(& _temp599);{ int _temp601= 0; if( setjmp( _temp599.handler)){
_temp601= 1;} if( ! _temp601){{ struct Cyc_Absyn_Enumdecl** _temp602=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
ge->enumdecls, v); struct Cyc_Absyn_Enumdecl* _temp603= Cyc_Tcdecl_merge_enumdecl(*
_temp602, ed, loc, Cyc_Tc_tc_msg); if( _temp603 ==  0){ _npop_handler( 0u);
return;}* _temp602=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp603);};
_pop_handler();} else{ void* _temp600=( void*) _exn_thrown; void* _temp605=
_temp600; _LL607: if( _temp605 ==  Cyc_Dict_Absent){ goto _LL608;} else{ goto
_LL609;} _LL609: goto _LL610; _LL608: { struct Cyc_Absyn_Enumdecl** _temp611=({
struct Cyc_Absyn_Enumdecl** _temp612=( struct Cyc_Absyn_Enumdecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp612[ 0]= ed; _temp612;}); ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Enumdecl** v)) Cyc_Dict_insert)( ge->enumdecls, v, _temp611); goto
_LL606;} _LL610:( void) _throw( _temp605); _LL606:;}}} if( ed->fields !=  0){
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Enumfield* _temp613=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp613->name).f2,({ struct _tuple4* _temp614=( struct
_tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp614->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp615=( struct Cyc_Tcenv_EnumRes_struct*) _cycalloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp615[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp616; _temp616.tag= Cyc_Tcenv_EnumRes; _temp616.f1= ed; _temp616.f2=
_temp613; _temp616;}); _temp615;}); _temp614->f2= 1; _temp614;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp617=
sc; _LL619: if( _temp617 == ( void*) Cyc_Absyn_Static){ goto _LL620;} else{ goto
_LL621;} _LL621: if( _temp617 == ( void*) Cyc_Absyn_Abstract){ goto _LL622;}
else{ goto _LL623;} _LL623: if( _temp617 == ( void*) Cyc_Absyn_Public){ goto
_LL624;} else{ goto _LL625;} _LL625: if( _temp617 == ( void*) Cyc_Absyn_Extern){
goto _LL626;} else{ goto _LL627;} _LL627: if( _temp617 == ( void*) Cyc_Absyn_ExternC){
goto _LL628;} else{ goto _LL618;} _LL620:({ void* _temp629[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("static declaration nested within extern \"C\"", sizeof(
unsigned char), 44u), _tag_arr( _temp629, sizeof( void*), 0u));}); return 0;
_LL622:({ void* _temp630[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u), _tag_arr( _temp630, sizeof( void*), 0u));});
return 0; _LL624: return 1; _LL626: return 1; _LL628:({ void* _temp631[ 0u]={};
Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration", sizeof(
unsigned char), 30u), _tag_arr( _temp631, sizeof( void*), 0u));}); return 1;
_LL618:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC, int check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)(
te->ae, te->ns); struct Cyc_List_List* last= 0; struct Cyc_Dict_Dict* dict= Cyc_Tcgenrep_empty_typerep_dict();
struct Cyc_List_List* _temp632= ds0; for( 0; _temp632 !=  0;( last= _temp632,
_temp632=(( struct Cyc_List_List*) _check_null( _temp632))->tl)){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp632))->hd;
struct Cyc_Position_Segment* loc= d->loc; void* _temp633=( void*) d->r; struct
Cyc_Absyn_Vardecl* _temp659; struct Cyc_Absyn_Fndecl* _temp661; struct Cyc_Absyn_Typedefdecl*
_temp663; struct Cyc_Absyn_Structdecl* _temp665; struct Cyc_Absyn_Uniondecl*
_temp667; struct Cyc_Absyn_Tuniondecl* _temp669; struct Cyc_Absyn_Enumdecl*
_temp671; struct Cyc_List_List* _temp673; struct _tagged_arr* _temp675; struct
Cyc_List_List* _temp677; struct _tuple0* _temp679; struct _tuple0 _temp681;
struct _tagged_arr* _temp682; void* _temp684; struct Cyc_List_List* _temp686;
_LL635: if(*(( int*) _temp633) ==  Cyc_Absyn_Let_d){ goto _LL636;} else{ goto
_LL637;} _LL637: if(*(( int*) _temp633) ==  Cyc_Absyn_Letv_d){ goto _LL638;}
else{ goto _LL639;} _LL639: if(*(( int*) _temp633) ==  Cyc_Absyn_Var_d){ _LL660:
_temp659=(( struct Cyc_Absyn_Var_d_struct*) _temp633)->f1; goto _LL640;} else{
goto _LL641;} _LL641: if(*(( int*) _temp633) ==  Cyc_Absyn_Fn_d){ _LL662:
_temp661=(( struct Cyc_Absyn_Fn_d_struct*) _temp633)->f1; goto _LL642;} else{
goto _LL643;} _LL643: if(*(( int*) _temp633) ==  Cyc_Absyn_Typedef_d){ _LL664:
_temp663=(( struct Cyc_Absyn_Typedef_d_struct*) _temp633)->f1; goto _LL644;}
else{ goto _LL645;} _LL645: if(*(( int*) _temp633) ==  Cyc_Absyn_Struct_d){
_LL666: _temp665=(( struct Cyc_Absyn_Struct_d_struct*) _temp633)->f1; goto
_LL646;} else{ goto _LL647;} _LL647: if(*(( int*) _temp633) ==  Cyc_Absyn_Union_d){
_LL668: _temp667=(( struct Cyc_Absyn_Union_d_struct*) _temp633)->f1; goto _LL648;}
else{ goto _LL649;} _LL649: if(*(( int*) _temp633) ==  Cyc_Absyn_Tunion_d){
_LL670: _temp669=(( struct Cyc_Absyn_Tunion_d_struct*) _temp633)->f1; goto
_LL650;} else{ goto _LL651;} _LL651: if(*(( int*) _temp633) ==  Cyc_Absyn_Enum_d){
_LL672: _temp671=(( struct Cyc_Absyn_Enum_d_struct*) _temp633)->f1; goto _LL652;}
else{ goto _LL653;} _LL653: if(*(( int*) _temp633) ==  Cyc_Absyn_Namespace_d){
_LL676: _temp675=(( struct Cyc_Absyn_Namespace_d_struct*) _temp633)->f1; goto
_LL674; _LL674: _temp673=(( struct Cyc_Absyn_Namespace_d_struct*) _temp633)->f2;
goto _LL654;} else{ goto _LL655;} _LL655: if(*(( int*) _temp633) ==  Cyc_Absyn_Using_d){
_LL680: _temp679=(( struct Cyc_Absyn_Using_d_struct*) _temp633)->f1; _temp681=*
_temp679; _LL685: _temp684= _temp681.f1; goto _LL683; _LL683: _temp682= _temp681.f2;
goto _LL678; _LL678: _temp677=(( struct Cyc_Absyn_Using_d_struct*) _temp633)->f2;
goto _LL656;} else{ goto _LL657;} _LL657: if(*(( int*) _temp633) ==  Cyc_Absyn_ExternC_d){
_LL687: _temp686=(( struct Cyc_Absyn_ExternC_d_struct*) _temp633)->f1; goto
_LL658;} else{ goto _LL634;} _LL636:({ void* _temp688[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("top level let-declarations are not implemented", sizeof(
unsigned char), 47u), _tag_arr( _temp688, sizeof( void*), 0u));}); goto _LL634;
_LL638:({ void* _temp689[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u), _tag_arr( _temp689, sizeof( void*), 0u));}); goto
_LL634; _LL640: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp659->sc):
0){( void*)( _temp659->sc=( void*)(( void*) Cyc_Absyn_ExternC));} if( _temp659->initializer
!=  0){ void* _temp690=( void*)(( struct Cyc_Absyn_Exp*) _check_null( _temp659->initializer))->r;
void* _temp696; _LL692: if(*(( int*) _temp690) ==  Cyc_Absyn_Gentyp_e){ _LL697:
_temp696=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp690)->f2; goto
_LL693;} else{ goto _LL694;} _LL694: goto _LL695; _LL693: { struct Cyc_Absyn_Exp*
_temp700; struct Cyc_List_List* _temp702; struct Cyc_Dict_Dict* _temp704; struct
_tuple3 _temp698= Cyc_Tcgenrep_tcGenrep( te, ge, loc, _temp696, dict); _LL705:
_temp704= _temp698.f1; goto _LL703; _LL703: _temp702= _temp698.f2; goto _LL701;
_LL701: _temp700= _temp698.f3; goto _LL699; _LL699: dict= _temp704; Cyc_Tc_tc_decls(
te, _temp702, in_externC, check_var_init); _temp659->initializer=( struct Cyc_Absyn_Exp*)
_temp700; Cyc_Tc_tcVardecl( te, ge, loc, _temp659, check_var_init); if( _temp702
!=  0){ if( last !=  0){(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp702, _temp632);(( struct
Cyc_List_List*) _check_null( last))->tl= _temp702;} else{ struct Cyc_List_List
tmp=({ struct Cyc_List_List _temp706; _temp706.hd=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp632))->hd); _temp706.tl=(( struct Cyc_List_List*)
_check_null( _temp632))->tl; _temp706;});( struct Cyc_Absyn_Decl*)((( struct Cyc_List_List*)
_check_null( _temp632))->hd=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp702))->hd));(( struct Cyc_List_List*) _check_null( _temp632))->tl=((
struct Cyc_List_List*) _check_null( _temp702))->tl;( struct Cyc_Absyn_Decl*)(((
struct Cyc_List_List*) _check_null( _temp702))->hd=( void*)(( struct Cyc_Absyn_Decl*)
tmp.hd));(( struct Cyc_List_List*) _check_null( _temp702))->tl= tmp.tl;(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp632, _temp702);}} continue;} _LL695: goto _LL691; _LL691:;} Cyc_Tc_tcVardecl(
te, ge, loc, _temp659, check_var_init); goto _LL634; _LL642: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp661->sc): 0){( void*)( _temp661->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp661); goto _LL634; _LL644: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp663); goto _LL634; _LL646: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp665->sc): 0){( void*)( _temp665->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp665); goto _LL634; _LL648: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp667->sc): 0){( void*)( _temp667->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp667);
goto _LL634; _LL650: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp669->sc): 0){( void*)( _temp669->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp669); goto _LL634; _LL652: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp671->sc): 0){( void*)( _temp671->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp671);
goto _LL634; _LL654: { struct Cyc_List_List* _temp707= te->ns; struct Cyc_List_List*
_temp708=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp707,({ struct Cyc_List_List* _temp709=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp709->hd=( void*) _temp675;
_temp709->tl= 0; _temp709;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp675)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp675); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k, struct Cyc_Tcenv_Genv* v)) Cyc_Dict_insert)( te->ae,
_temp708, Cyc_Tcenv_empty_genv());} te->ns= _temp708; Cyc_Tc_tc_decls( te,
_temp673, in_externC, check_var_init); te->ns= _temp707; goto _LL634;} _LL656: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp710= _temp684;
struct Cyc_List_List* _temp722; struct Cyc_List_List* _temp724; struct Cyc_List_List*
_temp726; struct Cyc_List_List _temp728; struct Cyc_List_List* _temp729; struct
_tagged_arr* _temp731; struct Cyc_List_List* _temp733; struct Cyc_List_List
_temp735; struct Cyc_List_List* _temp736; struct _tagged_arr* _temp738; _LL712:
if( _temp710 == ( void*) Cyc_Absyn_Loc_n){ goto _LL713;} else{ goto _LL714;}
_LL714: if(( unsigned int) _temp710 >  1u?*(( int*) _temp710) ==  Cyc_Absyn_Rel_n:
0){ _LL723: _temp722=(( struct Cyc_Absyn_Rel_n_struct*) _temp710)->f1; if(
_temp722 ==  0){ goto _LL715;} else{ goto _LL716;}} else{ goto _LL716;} _LL716:
if(( unsigned int) _temp710 >  1u?*(( int*) _temp710) ==  Cyc_Absyn_Abs_n: 0){
_LL725: _temp724=(( struct Cyc_Absyn_Abs_n_struct*) _temp710)->f1; if( _temp724
==  0){ goto _LL717;} else{ goto _LL718;}} else{ goto _LL718;} _LL718: if((
unsigned int) _temp710 >  1u?*(( int*) _temp710) ==  Cyc_Absyn_Rel_n: 0){ _LL727:
_temp726=(( struct Cyc_Absyn_Rel_n_struct*) _temp710)->f1; if( _temp726 ==  0){
goto _LL720;} else{ _temp728=* _temp726; _LL732: _temp731=( struct _tagged_arr*)
_temp728.hd; goto _LL730; _LL730: _temp729= _temp728.tl; goto _LL719;}} else{
goto _LL720;} _LL720: if(( unsigned int) _temp710 >  1u?*(( int*) _temp710) == 
Cyc_Absyn_Abs_n: 0){ _LL734: _temp733=(( struct Cyc_Absyn_Abs_n_struct*)
_temp710)->f1; if( _temp733 ==  0){ goto _LL711;} else{ _temp735=* _temp733;
_LL739: _temp738=( struct _tagged_arr*) _temp735.hd; goto _LL737; _LL737:
_temp736= _temp735.tl; goto _LL721;}} else{ goto _LL711;} _LL713: goto _LL715;
_LL715: goto _LL717; _LL717: first= _temp682; rest= 0; goto _LL711; _LL719:
_temp738= _temp731; _temp736= _temp729; goto _LL721; _LL721: first= _temp738;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp736,({ struct Cyc_List_List* _temp740=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp740->hd=( void*) _temp682;
_temp740->tl= 0; _temp740;})); goto _LL711; _LL711:;}{ struct Cyc_List_List*
_temp741= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp742=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp742->hd=( void*) _temp741; _temp742->tl= ge->availables;
_temp742;}); Cyc_Tc_tc_decls( te, _temp677, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL634;}} _LL658:
Cyc_Tc_tc_decls( te, _temp686, 1, check_var_init); goto _LL634; _LL634:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* x)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple7{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp743=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp753; struct Cyc_List_List*
_temp755; struct Cyc_List_List** _temp757; struct Cyc_List_List* _temp758;
struct Cyc_List_List** _temp760; _LL745: if(*(( int*) _temp743) ==  Cyc_Absyn_Var_d){
_LL754: _temp753=(( struct Cyc_Absyn_Var_d_struct*) _temp743)->f1; goto _LL746;}
else{ goto _LL747;} _LL747: if(*(( int*) _temp743) ==  Cyc_Absyn_Using_d){
_LL756: _temp755=(( struct Cyc_Absyn_Using_d_struct*) _temp743)->f2; _temp757=(
struct Cyc_List_List**)&(( struct Cyc_Absyn_Using_d_struct*) _temp743)->f2; goto
_LL748;} else{ goto _LL749;} _LL749: if(*(( int*) _temp743) ==  Cyc_Absyn_Namespace_d){
_LL759: _temp758=(( struct Cyc_Absyn_Namespace_d_struct*) _temp743)->f2;
_temp760=( struct Cyc_List_List**)&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp743)->f2; goto _LL750;} else{ goto _LL751;} _LL751: goto _LL752; _LL746:
if(( void*) _temp753->sc != ( void*) Cyc_Absyn_Extern){ return 1;}{ struct
_tuple0 _temp763; struct _tagged_arr* _temp764; void* _temp766; struct _tuple0*
_temp761= _temp753->name; _temp763=* _temp761; _LL767: _temp766= _temp763.f1;
goto _LL765; _LL765: _temp764= _temp763.f2; goto _LL762; _LL762: { struct Cyc_List_List*
ns;{ void* _temp768= _temp766; struct Cyc_List_List* _temp776; struct Cyc_List_List*
_temp778; _LL770: if( _temp768 == ( void*) Cyc_Absyn_Loc_n){ goto _LL771;} else{
goto _LL772;} _LL772: if(( unsigned int) _temp768 >  1u?*(( int*) _temp768) == 
Cyc_Absyn_Rel_n: 0){ _LL777: _temp776=(( struct Cyc_Absyn_Rel_n_struct*)
_temp768)->f1; goto _LL773;} else{ goto _LL774;} _LL774: if(( unsigned int)
_temp768 >  1u?*(( int*) _temp768) ==  Cyc_Absyn_Abs_n: 0){ _LL779: _temp778=((
struct Cyc_Absyn_Abs_n_struct*) _temp768)->f1; goto _LL775;} else{ goto _LL769;}
_LL771: ns= 0; goto _LL769; _LL773: ns= _temp776; goto _LL769; _LL775: ns=
_temp778; goto _LL769; _LL769:;}{ struct _tuple7* _temp780=(( struct _tuple7*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp781=(* _temp780).f1; int _temp782=(*(( struct
_tuple4*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
_temp781->ordinaries, _temp764)).f2; if( ! _temp782){(* _temp780).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp780).f2, _temp764);} return _temp782;}}} _LL748: _temp760= _temp757; goto
_LL750; _LL750:* _temp760= Cyc_Tc_treeshake_f( env,* _temp760); return 1; _LL752:
return 1; _LL744:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, void* y){ return !(( int(*)( struct Cyc_Set_Set* s,
struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple7* nsenv){ struct Cyc_Tcenv_Genv*
_temp783=(* nsenv).f1; struct Cyc_Set_Set* _temp784=(* nsenv).f2; _temp783->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple4*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)((
int(*)( struct Cyc_Set_Set* set, struct _tagged_arr* x, struct _tuple4* y)) Cyc_Tc_treeshake_remove_f,
_temp784, _temp783->ordinaries);} static struct _tuple7* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple7* _temp785=( struct _tuple7*)
_cycalloc( sizeof( struct _tuple7)); _temp785->f1= ge; _temp785->f2=(( struct
Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)(
Cyc_Std_strptrcmp); _temp785;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct
Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* _temp786=((
struct Cyc_Dict_Dict*(*)( struct _tuple7*(* f)( struct Cyc_Tcenv_Genv*), struct
Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae); struct
Cyc_List_List* _temp787= Cyc_Tc_treeshake_f( _temp786, ds);(( void(*)( void(* f)(
struct Cyc_List_List*, struct _tuple7*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)(
Cyc_Tc_treeshake_remove, _temp786); return _temp787;}
