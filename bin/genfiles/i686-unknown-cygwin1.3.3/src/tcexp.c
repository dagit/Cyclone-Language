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
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
extern void* Cyc_List_nth( struct Cyc_List_List* x, int n); extern struct Cyc_List_List*
Cyc_List_nth_tail( struct Cyc_List_List* x, int i); extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; static
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulonglong_t; extern void*
Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct
_tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern
int Cyc_Std_zstrcmp( struct _tagged_arr, struct _tagged_arr); struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern struct _tagged_arr Cyc_Std_vrprintf( struct _RegionHandle* r1, struct
_tagged_arr fmt, struct _tagged_arr ap); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
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
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* po); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern void Cyc_Tcutil_explain_failure();
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_rsubstitute( struct
_RegionHandle*, struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*); struct _tuple4{ struct Cyc_List_List* f1; struct
_RegionHandle* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple4*, struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*); extern
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, int allow_evars, void*); extern void
Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*, struct Cyc_Absyn_Conref*);
extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*, unsigned int i,
struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref*
b); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*);
struct _tuple6{ int f1; void* f2; } ; extern struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern int Cyc_Evexp_okay_szofarg( void* t); extern
void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int
new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*); extern
struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*,
struct _tagged_arr, struct Cyc_Position_Segment*); extern void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part);
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ;
static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; extern unsigned char Cyc_CfFlowInfo_IsZero[
11u]; extern unsigned char Cyc_CfFlowInfo_NotZero[ 12u]; extern unsigned char
Cyc_CfFlowInfo_UnknownZ[ 13u]; static const int Cyc_CfFlowInfo_PlaceL= 0; struct
Cyc_CfFlowInfo_PlaceL_struct{ int tag; struct Cyc_CfFlowInfo_Place* f1; } ;
static const int Cyc_CfFlowInfo_UnknownL= 0; static const int Cyc_CfFlowInfo_Zero=
0; static const int Cyc_CfFlowInfo_NotZeroAll= 1; static const int Cyc_CfFlowInfo_NotZeroThis=
2; static const int Cyc_CfFlowInfo_UnknownR= 0; struct Cyc_CfFlowInfo_UnknownR_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_Esc= 1; struct Cyc_CfFlowInfo_Esc_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_AddressOf= 2; struct Cyc_CfFlowInfo_AddressOf_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_Aggregate=
3; struct Cyc_CfFlowInfo_Aggregate_struct{ int tag; struct Cyc_Dict_Dict* f1; }
; static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_ReachableFL=
0; struct Cyc_CfFlowInfo_ReachableFL_struct{ int tag; struct Cyc_Dict_Dict* f1;
} ; extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); static void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr
ap){({ void* _temp0[ 0u]={}; Cyc_Tcutil_terr( loc,( struct _tagged_arr) Cyc_Std_vrprintf(
Cyc_Core_heap_region, msg, ap), _tag_arr( _temp0, sizeof( void*), 0u));});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp1=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1;}));} static struct Cyc_Core_Opt Cyc_Tcexp_rk={( void*)(( void*) 3u)};
static struct Cyc_Core_Opt Cyc_Tcexp_ak={( void*)(( void*) 0u)}; static struct
Cyc_Core_Opt Cyc_Tcexp_bk={( void*)(( void*) 2u)}; static struct Cyc_Core_Opt
Cyc_Tcexp_mk={( void*)(( void*) 1u)}; static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tuple1* q){ struct
_handler_cons _temp2; _push_handler(& _temp2);{ int _temp4= 0; if( setjmp(
_temp2.handler)){ _temp4= 1;} if( ! _temp4){{ void* _temp5= Cyc_Tcenv_lookup_ordinary(
te, e->loc, q); void* _temp17; struct Cyc_Absyn_Enumfield* _temp19; struct Cyc_Absyn_Enumdecl*
_temp21; struct Cyc_Absyn_Enumfield* _temp23; void* _temp25; struct Cyc_Absyn_Tunionfield*
_temp27; struct Cyc_Absyn_Tuniondecl* _temp29; _LL7: if(*(( int*) _temp5) == 
Cyc_Tcenv_VarRes){ _LL18: _temp17=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp5)->f1; goto _LL8;} else{ goto _LL9;} _LL9: if(*(( int*) _temp5) ==  Cyc_Tcenv_EnumRes){
_LL22: _temp21=(( struct Cyc_Tcenv_EnumRes_struct*) _temp5)->f1; goto _LL20;
_LL20: _temp19=(( struct Cyc_Tcenv_EnumRes_struct*) _temp5)->f2; goto _LL10;}
else{ goto _LL11;} _LL11: if(*(( int*) _temp5) ==  Cyc_Tcenv_AnonEnumRes){ _LL26:
_temp25=( void*)(( struct Cyc_Tcenv_AnonEnumRes_struct*) _temp5)->f1; goto _LL24;
_LL24: _temp23=(( struct Cyc_Tcenv_AnonEnumRes_struct*) _temp5)->f2; goto _LL12;}
else{ goto _LL13;} _LL13: if(*(( int*) _temp5) ==  Cyc_Tcenv_TunionRes){ _LL30:
_temp29=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f1; goto _LL28; _LL28:
_temp27=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f2; goto _LL14;} else{
goto _LL15;} _LL15: if(*(( int*) _temp5) ==  Cyc_Tcenv_StructRes){ goto _LL16;}
else{ goto _LL6;} _LL8:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp31=( struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp31[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp32; _temp32.tag= Cyc_Absyn_Var_e;
_temp32.f1= q; _temp32.f2=( void*) _temp17; _temp32;}); _temp31;}))); goto _LL6;
_LL10:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp33=(
struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp33[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp34; _temp34.tag= Cyc_Absyn_Enum_e;
_temp34.f1= q; _temp34.f2=( struct Cyc_Absyn_Enumdecl*) _temp21; _temp34.f3=(
struct Cyc_Absyn_Enumfield*) _temp19; _temp34;}); _temp33;}))); goto _LL6; _LL12:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonEnum_e_struct* _temp35=(
struct Cyc_Absyn_AnonEnum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnum_e_struct));
_temp35[ 0]=({ struct Cyc_Absyn_AnonEnum_e_struct _temp36; _temp36.tag= Cyc_Absyn_AnonEnum_e;
_temp36.f1= q; _temp36.f2=( void*) _temp25; _temp36.f3=( struct Cyc_Absyn_Enumfield*)
_temp23; _temp36;}); _temp35;}))); goto _LL6; _LL14:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp37=( struct Cyc_Absyn_Tunion_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp37[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp38; _temp38.tag= Cyc_Absyn_Tunion_e; _temp38.f1= 0; _temp38.f2= 0; _temp38.f3=
0; _temp38.f4= _temp29; _temp38.f5= _temp27; _temp38;}); _temp37;}))); goto _LL6;
_LL16:({ struct Cyc_Std_String_pa_struct _temp40; _temp40.tag= Cyc_Std_String_pa;
_temp40.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{ void* _temp39[ 1u]={&
_temp40}; Cyc_Tcutil_terr( e->loc, _tag_arr("bad occurrence of struct name %s",
sizeof( unsigned char), 33u), _tag_arr( _temp39, sizeof( void*), 1u));}});( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp41=( struct Cyc_Absyn_Var_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp41[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp42; _temp42.tag= Cyc_Absyn_Var_e; _temp42.f1= q; _temp42.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp42;}); _temp41;}))); goto _LL6; _LL6:;};
_pop_handler();} else{ void* _temp3=( void*) _exn_thrown; void* _temp44= _temp3;
_LL46: if( _temp44 ==  Cyc_Dict_Absent){ goto _LL47;} else{ goto _LL48;} _LL48:
goto _LL49; _LL47:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp50=( struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp50[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp51; _temp51.tag= Cyc_Absyn_Var_e;
_temp51.f1= q; _temp51.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp51;});
_temp50;}))); goto _LL45; _LL49:( void) _throw( _temp44); _LL45:;}}} struct
_tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple7* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({ struct
_tuple7* _temp52=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7)); _temp52->f1=
0; _temp52->f2= e; _temp52;});} static void Cyc_Tcexp_resolve_unknown_fn( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1, struct
Cyc_List_List* es){ void* _temp53=( void*) e1->r; struct _tuple1* _temp59; _LL55:
if(*(( int*) _temp53) ==  Cyc_Absyn_UnknownId_e){ _LL60: _temp59=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp53)->f1; goto _LL56;} else{ goto _LL57;} _LL57: goto _LL58; _LL56: { struct
_handler_cons _temp61; _push_handler(& _temp61);{ int _temp63= 0; if( setjmp(
_temp61.handler)){ _temp63= 1;} if( ! _temp63){{ void* _temp64= Cyc_Tcenv_lookup_ordinary(
te, e1->loc, _temp59); void* _temp76; struct Cyc_Absyn_Tunionfield* _temp78;
struct Cyc_Absyn_Tuniondecl* _temp80; struct Cyc_Absyn_Structdecl* _temp82;
_LL66: if(*(( int*) _temp64) ==  Cyc_Tcenv_VarRes){ _LL77: _temp76=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp64)->f1; goto _LL67;} else{ goto _LL68;}
_LL68: if(*(( int*) _temp64) ==  Cyc_Tcenv_TunionRes){ _LL81: _temp80=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp64)->f1; goto _LL79; _LL79: _temp78=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp64)->f2; goto _LL69;} else{ goto _LL70;} _LL70:
if(*(( int*) _temp64) ==  Cyc_Tcenv_StructRes){ _LL83: _temp82=(( struct Cyc_Tcenv_StructRes_struct*)
_temp64)->f1; goto _LL71;} else{ goto _LL72;} _LL72: if(*(( int*) _temp64) == 
Cyc_Tcenv_AnonEnumRes){ goto _LL73;} else{ goto _LL74;} _LL74: if(*(( int*)
_temp64) ==  Cyc_Tcenv_EnumRes){ goto _LL75;} else{ goto _LL65;} _LL67:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp84=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp84[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp85; _temp85.tag= Cyc_Absyn_FnCall_e; _temp85.f1= e1; _temp85.f2= es;
_temp85.f3= 0; _temp85;}); _temp84;}))); _npop_handler( 0u); return; _LL69: if(
_temp78->typs ==  0){({ struct Cyc_Std_String_pa_struct _temp87; _temp87.tag=
Cyc_Std_String_pa; _temp87.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp78->name);{ void* _temp86[ 1u]={& _temp87}; Cyc_Tcutil_terr( e->loc,
_tag_arr("%s is a constant, not a function", sizeof( unsigned char), 33u),
_tag_arr( _temp86, sizeof( void*), 1u));}});}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_e_struct* _temp88=( struct Cyc_Absyn_Tunion_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp88[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp89; _temp89.tag= Cyc_Absyn_Tunion_e; _temp89.f1= 0; _temp89.f2= 0; _temp89.f3=
es; _temp89.f4= _temp80; _temp89.f5= _temp78; _temp89;}); _temp88;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL71: { struct Cyc_List_List*
_temp90=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp82->name !=  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp91=( struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp91[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp92; _temp92.tag= Cyc_Absyn_Struct_e;
_temp92.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp82->name))->v;
_temp92.f2= 0; _temp92.f3= _temp90; _temp92.f4=( struct Cyc_Absyn_Structdecl*)
_temp82; _temp92;}); _temp91;})));} else{({ void* _temp93[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("missing struct name", sizeof( unsigned char), 20u), _tag_arr(
_temp93, sizeof( void*), 0u));});( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp94=( struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp94[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp95; _temp95.tag= Cyc_Absyn_Struct_e;
_temp95.f1= _temp59; _temp95.f2= 0; _temp95.f3= _temp90; _temp95.f4=( struct Cyc_Absyn_Structdecl*)
_temp82; _temp95;}); _temp94;})));} _npop_handler( 0u); return;} _LL73: goto
_LL75; _LL75:({ struct Cyc_Std_String_pa_struct _temp97; _temp97.tag= Cyc_Std_String_pa;
_temp97.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp59);{ void*
_temp96[ 1u]={& _temp97}; Cyc_Tcutil_terr( e->loc, _tag_arr("%s is an enum constructor, not a function",
sizeof( unsigned char), 42u), _tag_arr( _temp96, sizeof( void*), 1u));}});
_npop_handler( 0u); return; _LL65:;}; _pop_handler();} else{ void* _temp62=(
void*) _exn_thrown; void* _temp99= _temp62; _LL101: if( _temp99 ==  Cyc_Dict_Absent){
goto _LL102;} else{ goto _LL103;} _LL103: goto _LL104; _LL102:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp105=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp105[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp106; _temp106.tag= Cyc_Absyn_FnCall_e; _temp106.f1=
e1; _temp106.f2= es; _temp106.f3= 0; _temp106;}); _temp105;}))); return; _LL104:(
void) _throw( _temp99); _LL100:;}}} _LL58:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp107=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp107[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp108; _temp108.tag= Cyc_Absyn_FnCall_e; _temp108.f1=
e1; _temp108.f2= es; _temp108.f3= 0; _temp108;}); _temp107;}))); return; _LL54:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt ==  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp109=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp109[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp110; _temp110.tag= Cyc_Absyn_Array_e;
_temp110.f1= des; _temp110;}); _temp109;}))); return;}{ void* t=* topt; void*
_temp111= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tqual _temp121; void*
_temp123; struct Cyc_Absyn_Structdecl** _temp125; struct Cyc_List_List* _temp127;
struct _tuple1* _temp129; _LL113: if(( unsigned int) _temp111 >  4u?*(( int*)
_temp111) ==  Cyc_Absyn_ArrayType: 0){ _LL124: _temp123=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp111)->f1; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_ArrayType_struct*)
_temp111)->f2; goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp111 >  4u?*(( int*) _temp111) ==  Cyc_Absyn_StructType: 0){ _LL130:
_temp129=(( struct Cyc_Absyn_StructType_struct*) _temp111)->f1; goto _LL128;
_LL128: _temp127=(( struct Cyc_Absyn_StructType_struct*) _temp111)->f2; goto
_LL126; _LL126: _temp125=(( struct Cyc_Absyn_StructType_struct*) _temp111)->f3;
goto _LL116;} else{ goto _LL117;} _LL117: if(( unsigned int) _temp111 >  4u?*((
int*) _temp111) ==  Cyc_Absyn_AnonStructType: 0){ goto _LL118;} else{ goto
_LL119;} _LL119: goto _LL120; _LL114:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Array_e_struct* _temp131=( struct Cyc_Absyn_Array_e_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Array_e_struct)); _temp131[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp132; _temp132.tag= Cyc_Absyn_Array_e; _temp132.f1= des; _temp132;});
_temp131;}))); goto _LL112; _LL116: if( _temp125 ==  0? 1: _temp129 ==  0){({
void* _temp133[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("Compiler bug: struct type not properly set",
sizeof( unsigned char), 43u), _tag_arr( _temp133, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct* _temp134=(
struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp134[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp135; _temp135.tag= Cyc_Absyn_Struct_e;
_temp135.f1=( struct _tuple1*) _check_null( _temp129); _temp135.f2= 0; _temp135.f3=
des; _temp135.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp125)); _temp135;}); _temp134;}))); goto _LL112; _LL118:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp136=( struct
Cyc_Absyn_AnonStruct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp136[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp137; _temp137.tag= Cyc_Absyn_AnonStruct_e;
_temp137.f1=( void*) t; _temp137.f2= des; _temp137;}); _temp136;}))); goto
_LL112; _LL120:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp138=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp138[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp139; _temp139.tag= Cyc_Absyn_Array_e;
_temp139.f1= des; _temp139;}); _temp138;}))); goto _LL112; _LL112:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es !=  0; es= es->tl){ Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*) es->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){({ struct Cyc_Std_String_pa_struct _temp142; _temp142.tag= Cyc_Std_String_pa;
_temp142.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct _temp141; _temp141.tag=
Cyc_Std_String_pa; _temp141.f1=( struct _tagged_arr) msg_part;{ void* _temp140[
2u]={& _temp141,& _temp142}; Cyc_Tcutil_terr( e->loc, _tag_arr("test of %s has type %s instead of integral or * type",
sizeof( unsigned char), 53u), _tag_arr( _temp140, sizeof( void*), 2u));}}});}}
static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp143= c;
void* _temp159; void* _temp161; void* _temp163; int _temp165; void* _temp167;
struct _tagged_arr _temp169; _LL145: if(( unsigned int) _temp143 >  1u?*(( int*)
_temp143) ==  Cyc_Absyn_Char_c: 0){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp143)->f1; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp143 >  1u?*(( int*) _temp143) ==  Cyc_Absyn_Short_c: 0){ _LL162: _temp161=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp143)->f1; goto _LL148;} else{
goto _LL149;} _LL149: if(( unsigned int) _temp143 >  1u?*(( int*) _temp143) == 
Cyc_Absyn_LongLong_c: 0){ _LL164: _temp163=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp143)->f1; goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int)
_temp143 >  1u?*(( int*) _temp143) ==  Cyc_Absyn_Float_c: 0){ goto _LL152;}
else{ goto _LL153;} _LL153: if(( unsigned int) _temp143 >  1u?*(( int*) _temp143)
==  Cyc_Absyn_Int_c: 0){ _LL168: _temp167=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp143)->f1; goto _LL166; _LL166: _temp165=(( struct Cyc_Absyn_Int_c_struct*)
_temp143)->f2; goto _LL154;} else{ goto _LL155;} _LL155: if(( unsigned int)
_temp143 >  1u?*(( int*) _temp143) ==  Cyc_Absyn_String_c: 0){ _LL170: _temp169=((
struct Cyc_Absyn_String_c_struct*) _temp143)->f1; goto _LL156;} else{ goto
_LL157;} _LL157: if( _temp143 == ( void*) Cyc_Absyn_Null_c){ goto _LL158;} else{
goto _LL144;} _LL146: t= _temp159 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t; goto _LL144; _LL148: t= _temp161 == ( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL144; _LL150: t= _temp163 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto
_LL144; _LL152: t= Cyc_Absyn_float_t; goto _LL144; _LL154: if( topt ==  0){ t=
_temp167 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp171= Cyc_Tcutil_compress(* topt); void* _temp181; void*
_temp183; void* _temp185; void* _temp187; void* _temp189; _LL173: if((
unsigned int) _temp171 >  4u?*(( int*) _temp171) ==  Cyc_Absyn_IntType: 0){
_LL184: _temp183=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1;
goto _LL182; _LL182: _temp181=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f2; if( _temp181 == ( void*) Cyc_Absyn_B1){ goto _LL174;} else{ goto
_LL175;}} else{ goto _LL175;} _LL175: if(( unsigned int) _temp171 >  4u?*(( int*)
_temp171) ==  Cyc_Absyn_IntType: 0){ _LL188: _temp187=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f1; goto _LL186; _LL186: _temp185=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f2; if( _temp185 == ( void*) Cyc_Absyn_B2){ goto _LL176;} else{ goto
_LL177;}} else{ goto _LL177;} _LL177: if(( unsigned int) _temp171 >  4u?*(( int*)
_temp171) ==  Cyc_Absyn_IntType: 0){ _LL190: _temp189=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f1; goto _LL178;} else{ goto _LL179;} _LL179: goto _LL180; _LL174: t=
_temp183 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp191=(
struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp191[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp192; _temp192.tag= Cyc_Absyn_Const_e;
_temp192.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp193=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp193[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp194; _temp194.tag= Cyc_Absyn_Char_c;
_temp194.f1=( void*) _temp183; _temp194.f2=( unsigned char) _temp165; _temp194;});
_temp193;})); _temp192;}); _temp191;}))); goto _LL172; _LL176: t= _temp187 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp195=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp195[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp196; _temp196.tag= Cyc_Absyn_Const_e; _temp196.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp197=( struct Cyc_Absyn_Short_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp197[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp198; _temp198.tag= Cyc_Absyn_Short_c; _temp198.f1=( void*) _temp187;
_temp198.f2=( short) _temp165; _temp198;}); _temp197;})); _temp196;}); _temp195;})));
goto _LL172; _LL178: t= _temp189 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL172; _LL180: t= _temp167 == ( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL172; _LL172:;} goto _LL144; _LL156: {
int len=( int) _get_arr_size( _temp169, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp199=({ struct Cyc_Absyn_Const_e_struct* _temp236=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp236[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp237; _temp237.tag= Cyc_Absyn_Const_e; _temp237.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp238=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp238[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp239; _temp239.tag= Cyc_Absyn_Int_c; _temp239.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp239.f2= len; _temp239;}); _temp238;})); _temp237;}); _temp236;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp199, loc); elen->topt=({
struct Cyc_Core_Opt* _temp200=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp200->v=( void*) Cyc_Absyn_uint_t; _temp200;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp201=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp201[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp202; _temp202.tag= Cyc_Absyn_Upper_b; _temp202.f1= elen; _temp202;});
_temp201;})); if( topt !=  0){ void* _temp203= Cyc_Tcutil_compress(* topt);
struct Cyc_Absyn_Exp* _temp211; struct Cyc_Absyn_Tqual _temp213; void* _temp215;
struct Cyc_Absyn_PtrInfo _temp217; struct Cyc_Absyn_Conref* _temp219; struct Cyc_Absyn_Tqual
_temp221; struct Cyc_Absyn_Conref* _temp223; void* _temp225; void* _temp227;
_LL205: if(( unsigned int) _temp203 >  4u?*(( int*) _temp203) ==  Cyc_Absyn_ArrayType:
0){ _LL216: _temp215=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f1;
goto _LL214; _LL214: _temp213=(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f2;
goto _LL212; _LL212: _temp211=(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f3;
goto _LL206;} else{ goto _LL207;} _LL207: if(( unsigned int) _temp203 >  4u?*((
int*) _temp203) ==  Cyc_Absyn_PointerType: 0){ _LL218: _temp217=(( struct Cyc_Absyn_PointerType_struct*)
_temp203)->f1; _LL228: _temp227=( void*) _temp217.elt_typ; goto _LL226; _LL226:
_temp225=( void*) _temp217.rgn_typ; goto _LL224; _LL224: _temp223= _temp217.nullable;
goto _LL222; _LL222: _temp221= _temp217.tq; goto _LL220; _LL220: _temp219=
_temp217.bounds; goto _LL208;} else{ goto _LL209;} _LL209: goto _LL210; _LL206:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp229=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp229[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp230; _temp230.tag= Cyc_Absyn_ArrayType; _temp230.f1=(
void*) Cyc_Absyn_uchar_t; _temp230.f2= _temp213; _temp230.f3=( struct Cyc_Absyn_Exp*)
elen; _temp230;}); _temp229;}); _LL208: if( ! Cyc_Tcutil_unify(* topt, t)? Cyc_Tcutil_silent_castable(
te, loc, t,* topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp231=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp231->v=( void*) t; _temp231;});
Cyc_Tcutil_unchecked_cast( te, e,* topt); t=* topt;} else{ t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp232=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp232->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp232;})),
Cyc_Absyn_const_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp233=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp233[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp234; _temp234.tag= Cyc_Absyn_Upper_b;
_temp234.f1= elen; _temp234;}); _temp233;})); if( ! Cyc_Tcutil_unify(* topt, t)?
Cyc_Tcutil_silent_castable( te, loc, t,* topt): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp235=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp235->v=( void*) t; _temp235;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); t=*
topt;}} goto _LL204; _LL210: goto _LL204; _LL204:;} return t;} _LL158: { struct
Cyc_List_List* _temp240= Cyc_Tcenv_lookup_type_vars( te); t=( void*)({ struct
Cyc_Absyn_PointerType_struct* _temp241=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp241[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp242; _temp242.tag= Cyc_Absyn_PointerType;
_temp242.f1=({ struct Cyc_Absyn_PtrInfo _temp243; _temp243.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp245=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp245->v=( void*)
_temp240; _temp245;})); _temp243.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp244=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp244->v=( void*) _temp240;
_temp244;})); _temp243.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp243.tq= Cyc_Absyn_empty_tqual(); _temp243.bounds= Cyc_Absyn_empty_conref();
_temp243;}); _temp242;}); _temp241;}); goto _LL144;} _LL144:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp246= b; struct Cyc_Absyn_Vardecl*
_temp260; struct Cyc_Absyn_Fndecl* _temp262; struct Cyc_Absyn_Vardecl* _temp264;
struct Cyc_Absyn_Vardecl* _temp266; struct Cyc_Absyn_Vardecl* _temp268; _LL248:
if( _temp246 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL249;} else{ goto
_LL250;} _LL250: if(( unsigned int) _temp246 >  1u?*(( int*) _temp246) ==  Cyc_Absyn_Global_b:
0){ _LL261: _temp260=(( struct Cyc_Absyn_Global_b_struct*) _temp246)->f1; goto
_LL251;} else{ goto _LL252;} _LL252: if(( unsigned int) _temp246 >  1u?*(( int*)
_temp246) ==  Cyc_Absyn_Funname_b: 0){ _LL263: _temp262=(( struct Cyc_Absyn_Funname_b_struct*)
_temp246)->f1; goto _LL253;} else{ goto _LL254;} _LL254: if(( unsigned int)
_temp246 >  1u?*(( int*) _temp246) ==  Cyc_Absyn_Pat_b: 0){ _LL265: _temp264=((
struct Cyc_Absyn_Pat_b_struct*) _temp246)->f1; goto _LL255;} else{ goto _LL256;}
_LL256: if(( unsigned int) _temp246 >  1u?*(( int*) _temp246) ==  Cyc_Absyn_Local_b:
0){ _LL267: _temp266=(( struct Cyc_Absyn_Local_b_struct*) _temp246)->f1; goto
_LL257;} else{ goto _LL258;} _LL258: if(( unsigned int) _temp246 >  1u?*(( int*)
_temp246) ==  Cyc_Absyn_Param_b: 0){ _LL269: _temp268=(( struct Cyc_Absyn_Param_b_struct*)
_temp246)->f1; goto _LL259;} else{ goto _LL247;} _LL249: return({ struct Cyc_Std_String_pa_struct
_temp271; _temp271.tag= Cyc_Std_String_pa; _temp271.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp270[ 1u]={& _temp271}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("undeclared identifier: %s",
sizeof( unsigned char), 26u), _tag_arr( _temp270, sizeof( void*), 1u));}});
_LL251: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp260->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp260->type; _LL253:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp262->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp262);
_LL255: _temp266= _temp264; goto _LL257; _LL257: _temp268= _temp266; goto _LL259;
_LL259:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp268->type; _LL247:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_Core_Opt* opt_args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp272=( void*) fmt->r; void* _temp280; struct _tagged_arr
_temp282; struct Cyc_Absyn_Exp* _temp284; struct Cyc_Absyn_Exp _temp286; void*
_temp287; void* _temp289; struct _tagged_arr _temp291; _LL274: if(*(( int*)
_temp272) ==  Cyc_Absyn_Const_e){ _LL281: _temp280=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp272)->f1; if(( unsigned int) _temp280 >  1u?*(( int*) _temp280) ==  Cyc_Absyn_String_c:
0){ _LL283: _temp282=(( struct Cyc_Absyn_String_c_struct*) _temp280)->f1; goto
_LL275;} else{ goto _LL276;}} else{ goto _LL276;} _LL276: if(*(( int*) _temp272)
==  Cyc_Absyn_Cast_e){ _LL285: _temp284=(( struct Cyc_Absyn_Cast_e_struct*)
_temp272)->f2; _temp286=* _temp284; _LL288: _temp287=( void*) _temp286.r; if(*((
int*) _temp287) ==  Cyc_Absyn_Const_e){ _LL290: _temp289=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp287)->f1; if(( unsigned int) _temp289 >  1u?*(( int*) _temp289) ==  Cyc_Absyn_String_c:
0){ _LL292: _temp291=(( struct Cyc_Absyn_String_c_struct*) _temp289)->f1; goto
_LL277;} else{ goto _LL278;}} else{ goto _LL278;}} else{ goto _LL278;} _LL278:
goto _LL279; _LL275: _temp291= _temp282; goto _LL277; _LL277: desc_types=
type_getter( te,( struct _tagged_arr) _temp291, fmt->loc); goto _LL273; _LL279:
return; _LL273:;} if( opt_args !=  0){ struct Cyc_List_List* _temp293=( struct
Cyc_List_List*) opt_args->v; for( 0; desc_types !=  0? _temp293 !=  0: 0;(
desc_types= desc_types->tl, _temp293= _temp293->tl)){ void* t=( void*)
desc_types->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) _temp293->hd;
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct Cyc_Std_String_pa_struct
_temp296; _temp296.tag= Cyc_Std_String_pa; _temp296.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp295; _temp295.tag= Cyc_Std_String_pa; _temp295.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp294[ 2u]={& _temp295,& _temp296}; Cyc_Tcutil_terr( e->loc,
_tag_arr("descriptor has type \n%s\n but argument has type \n%s", sizeof(
unsigned char), 51u), _tag_arr( _temp294, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}
if( desc_types !=  0){({ void* _temp297[ 0u]={}; Cyc_Tcutil_terr( fmt->loc,
_tag_arr("too few arguments", sizeof( unsigned char), 18u), _tag_arr( _temp297,
sizeof( void*), 0u));});} if( _temp293 !=  0){({ void* _temp298[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) _temp293->hd)->loc, _tag_arr("too many arguments",
sizeof( unsigned char), 19u), _tag_arr( _temp298, sizeof( void*), 0u));});}}}
static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp299= p;
_LL301: if( _temp299 == ( void*) Cyc_Absyn_Plus){ goto _LL302;} else{ goto
_LL303;} _LL303: if( _temp299 == ( void*) Cyc_Absyn_Minus){ goto _LL304;} else{
goto _LL305;} _LL305: if( _temp299 == ( void*) Cyc_Absyn_Not){ goto _LL306;}
else{ goto _LL307;} _LL307: if( _temp299 == ( void*) Cyc_Absyn_Bitnot){ goto
_LL308;} else{ goto _LL309;} _LL309: if( _temp299 == ( void*) Cyc_Absyn_Size){
goto _LL310;} else{ goto _LL311;} _LL311: goto _LL312; _LL302: goto _LL304;
_LL304: if( ! Cyc_Tcutil_is_numeric( e)){({ struct Cyc_Std_String_pa_struct
_temp314; _temp314.tag= Cyc_Std_String_pa; _temp314.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp313[ 1u]={& _temp314}; Cyc_Tcutil_terr( loc, _tag_arr("expecting arithmetic type but found %s",
sizeof( unsigned char), 39u), _tag_arr( _temp313, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL306: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){({ struct Cyc_Std_String_pa_struct
_temp316; _temp316.tag= Cyc_Std_String_pa; _temp316.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp315[ 1u]={& _temp316}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral or * type but found %s",
sizeof( unsigned char), 42u), _tag_arr( _temp315, sizeof( void*), 1u));}});}
return Cyc_Absyn_sint_t; _LL308: if( ! Cyc_Tcutil_is_integral( e)){({ struct Cyc_Std_String_pa_struct
_temp318; _temp318.tag= Cyc_Std_String_pa; _temp318.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp317[ 1u]={& _temp318}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp317, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL310:{ void*
_temp319= t; struct Cyc_Absyn_PtrInfo _temp327; _LL321: if(( unsigned int)
_temp319 >  4u?*(( int*) _temp319) ==  Cyc_Absyn_ArrayType: 0){ goto _LL322;}
else{ goto _LL323;} _LL323: if(( unsigned int) _temp319 >  4u?*(( int*) _temp319)
==  Cyc_Absyn_PointerType: 0){ _LL328: _temp327=(( struct Cyc_Absyn_PointerType_struct*)
_temp319)->f1; goto _LL324;} else{ goto _LL325;} _LL325: goto _LL326; _LL322:
goto _LL320; _LL324: goto _LL320; _LL326:({ struct Cyc_Std_String_pa_struct
_temp330; _temp330.tag= Cyc_Std_String_pa; _temp330.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp329[ 1u]={& _temp330}; Cyc_Tcutil_terr( loc, _tag_arr("size requires pointer or array type, not %s",
sizeof( unsigned char), 44u), _tag_arr( _temp329, sizeof( void*), 1u));}}); goto
_LL320; _LL320:;} return Cyc_Absyn_uint_t; _LL312: return({ void* _temp331[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("Non-unary primop", sizeof( unsigned char), 17u),
_tag_arr( _temp331, sizeof( void*), 0u));}); _LL300:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){({ struct Cyc_Std_String_pa_struct
_temp333; _temp333.tag= Cyc_Std_String_pa; _temp333.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp332[ 1u]={&
_temp333}; Cyc_Tcutil_terr( e1->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp332, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp334=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp334->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp334;}));} if( ! checker( e2)){({ struct Cyc_Std_String_pa_struct
_temp336; _temp336.tag= Cyc_Std_String_pa; _temp336.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp335[ 1u]={&
_temp336}; Cyc_Tcutil_terr( e2->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp335, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp337=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp337->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp337;}));}{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); void* _temp338= t1; struct
Cyc_Absyn_PtrInfo _temp344; struct Cyc_Absyn_Conref* _temp346; struct Cyc_Absyn_Tqual
_temp348; struct Cyc_Absyn_Conref* _temp350; void* _temp352; void* _temp354;
_LL340: if(( unsigned int) _temp338 >  4u?*(( int*) _temp338) ==  Cyc_Absyn_PointerType:
0){ _LL345: _temp344=(( struct Cyc_Absyn_PointerType_struct*) _temp338)->f1;
_LL355: _temp354=( void*) _temp344.elt_typ; goto _LL353; _LL353: _temp352=( void*)
_temp344.rgn_typ; goto _LL351; _LL351: _temp350= _temp344.nullable; goto _LL349;
_LL349: _temp348= _temp344.tq; goto _LL347; _LL347: _temp346= _temp344.bounds;
goto _LL341;} else{ goto _LL342;} _LL342: goto _LL343; _LL341: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){({ struct Cyc_Std_String_pa_struct _temp357; _temp357.tag= Cyc_Std_String_pa;
_temp357.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp356[
1u]={& _temp357}; Cyc_Tcutil_terr( e2->loc, _tag_arr("expecting int but found %s",
sizeof( unsigned char), 27u), _tag_arr( _temp356, sizeof( void*), 1u));}});}
_temp346= Cyc_Absyn_compress_conref( _temp346);{ void* _temp358=( void*)
_temp346->v; void* _temp366; void* _temp368; struct Cyc_Absyn_Exp* _temp370;
_LL360: if(( unsigned int) _temp358 >  1u?*(( int*) _temp358) ==  Cyc_Absyn_Eq_constr:
0){ _LL367: _temp366=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp358)->f1;
if( _temp366 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL361;} else{ goto _LL362;}}
else{ goto _LL362;} _LL362: if(( unsigned int) _temp358 >  1u?*(( int*) _temp358)
==  Cyc_Absyn_Eq_constr: 0){ _LL369: _temp368=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp358)->f1; if(( unsigned int) _temp368 >  1u?*(( int*) _temp368) ==  Cyc_Absyn_Upper_b:
0){ _LL371: _temp370=(( struct Cyc_Absyn_Upper_b_struct*) _temp368)->f1; goto
_LL363;} else{ goto _LL364;}} else{ goto _LL364;} _LL364: goto _LL365; _LL361:
return t1; _LL363: { struct Cyc_Absyn_PointerType_struct* _temp372=({ struct Cyc_Absyn_PointerType_struct*
_temp373=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp373[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp374; _temp374.tag= Cyc_Absyn_PointerType;
_temp374.f1=({ struct Cyc_Absyn_PtrInfo _temp375; _temp375.elt_typ=( void*)
_temp354; _temp375.rgn_typ=( void*) _temp352; _temp375.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp375.tq= _temp348; _temp375.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp375;}); _temp374;}); _temp373;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp372); return( void*) _temp372;} _LL365:( void*)( _temp346->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp376=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp376[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp377; _temp377.tag= Cyc_Absyn_Eq_constr; _temp377.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp377;}); _temp376;}))); return t1;
_LL359:;} _LL343: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL339:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){({ struct Cyc_Std_String_pa_struct
_temp380; _temp380.tag= Cyc_Std_String_pa; _temp380.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp379; _temp379.tag= Cyc_Std_String_pa; _temp379.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp378[ 2u]={&
_temp379,& _temp380}; Cyc_Tcutil_terr( e1->loc, _tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof( unsigned char), 59u), _tag_arr( _temp378, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){({
struct Cyc_Std_String_pa_struct _temp383; _temp383.tag= Cyc_Std_String_pa;
_temp383.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp382; _temp382.tag= Cyc_Std_String_pa; _temp382.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp381[ 2u]={& _temp382,& _temp383}; Cyc_Tcutil_terr( e2->loc,
_tag_arr("expecting either %s or int but found %s", sizeof( unsigned char), 40u),
_tag_arr( _temp381, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp384=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp384->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp384;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp386=({ struct _tuple0 _temp385;
_temp385.f1= Cyc_Tcutil_compress( t1); _temp385.f2= Cyc_Tcutil_compress( t2);
_temp385;}); void* _temp392; struct Cyc_Absyn_PtrInfo _temp394; void* _temp396;
void* _temp398; struct Cyc_Absyn_PtrInfo _temp400; void* _temp402; _LL388:
_LL399: _temp398= _temp386.f1; if(( unsigned int) _temp398 >  4u?*(( int*)
_temp398) ==  Cyc_Absyn_PointerType: 0){ _LL401: _temp400=(( struct Cyc_Absyn_PointerType_struct*)
_temp398)->f1; _LL403: _temp402=( void*) _temp400.elt_typ; goto _LL393;} else{
goto _LL390;} _LL393: _temp392= _temp386.f2; if(( unsigned int) _temp392 >  4u?*((
int*) _temp392) ==  Cyc_Absyn_PointerType: 0){ _LL395: _temp394=(( struct Cyc_Absyn_PointerType_struct*)
_temp392)->f1; _LL397: _temp396=( void*) _temp394.elt_typ; goto _LL389;} else{
goto _LL390;} _LL390: goto _LL391; _LL389: if( Cyc_Tcutil_unify( _temp402,
_temp396)){ return Cyc_Absyn_sint_t;} goto _LL387; _LL391: goto _LL387; _LL387:;}({
struct Cyc_Std_String_pa_struct _temp406; _temp406.tag= Cyc_Std_String_pa;
_temp406.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp405; _temp405.tag= Cyc_Std_String_pa; _temp405.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp404[ 2u]={& _temp405,& _temp406}; Cyc_Tcutil_terr( loc,
_tag_arr("comparison not allowed between %s and %s", sizeof( unsigned char), 41u),
_tag_arr( _temp404, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp407=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp407->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp407;}));}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp408= p; _LL410: if( _temp408 == ( void*)
Cyc_Absyn_Plus){ goto _LL411;} else{ goto _LL412;} _LL412: if( _temp408 == (
void*) Cyc_Absyn_Minus){ goto _LL413;} else{ goto _LL414;} _LL414: if( _temp408
== ( void*) Cyc_Absyn_Times){ goto _LL415;} else{ goto _LL416;} _LL416: if(
_temp408 == ( void*) Cyc_Absyn_Div){ goto _LL417;} else{ goto _LL418;} _LL418:
if( _temp408 == ( void*) Cyc_Absyn_Mod){ goto _LL419;} else{ goto _LL420;}
_LL420: if( _temp408 == ( void*) Cyc_Absyn_Bitand){ goto _LL421;} else{ goto
_LL422;} _LL422: if( _temp408 == ( void*) Cyc_Absyn_Bitor){ goto _LL423;} else{
goto _LL424;} _LL424: if( _temp408 == ( void*) Cyc_Absyn_Bitxor){ goto _LL425;}
else{ goto _LL426;} _LL426: if( _temp408 == ( void*) Cyc_Absyn_Bitlshift){ goto
_LL427;} else{ goto _LL428;} _LL428: if( _temp408 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL429;} else{ goto _LL430;} _LL430: if( _temp408 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL431;} else{ goto _LL432;} _LL432: if( _temp408 == ( void*) Cyc_Absyn_Eq){
goto _LL433;} else{ goto _LL434;} _LL434: if( _temp408 == ( void*) Cyc_Absyn_Neq){
goto _LL435;} else{ goto _LL436;} _LL436: if( _temp408 == ( void*) Cyc_Absyn_Gt){
goto _LL437;} else{ goto _LL438;} _LL438: if( _temp408 == ( void*) Cyc_Absyn_Lt){
goto _LL439;} else{ goto _LL440;} _LL440: if( _temp408 == ( void*) Cyc_Absyn_Gte){
goto _LL441;} else{ goto _LL442;} _LL442: if( _temp408 == ( void*) Cyc_Absyn_Lte){
goto _LL443;} else{ goto _LL444;} _LL444: goto _LL445; _LL411: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL413: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL415: goto _LL417;
_LL417: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL419: goto _LL421; _LL421: goto _LL423; _LL423: goto _LL425; _LL425: goto
_LL427; _LL427: goto _LL429; _LL429: goto _LL431; _LL431: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL433: goto _LL435; _LL435: goto _LL437;
_LL437: goto _LL439; _LL439: goto _LL441; _LL441: goto _LL443; _LL443: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL445:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp446=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp446[ 0]=({ struct Cyc_Core_Failure_struct
_temp447; _temp447.tag= Cyc_Core_Failure; _temp447.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp447;}); _temp446;})); _LL409:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p == ( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) ==  1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t; switch((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ case 0: _LL448: return({ void* _temp450[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("primitive operator has 0 arguments", sizeof( unsigned char),
35u), _tag_arr( _temp450, sizeof( void*), 0u));}); case 1: _LL449: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
es))->hd); break; case 2: _LL451: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd); break; default: _LL452: return({ void* _temp454[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u), _tag_arr( _temp454, sizeof( void*), 0u));});}
return t;}} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp455=( void*) e->r; void* _temp479; struct Cyc_Absyn_Vardecl* _temp481;
void* _temp483; struct Cyc_Absyn_Vardecl* _temp485; void* _temp487; struct Cyc_Absyn_Vardecl*
_temp489; void* _temp491; struct Cyc_Absyn_Vardecl* _temp493; struct Cyc_Absyn_Exp*
_temp495; struct Cyc_Absyn_Exp* _temp497; struct _tagged_arr* _temp499; struct
Cyc_Absyn_Exp* _temp501; struct _tagged_arr* _temp503; struct Cyc_Absyn_Exp*
_temp505; struct Cyc_Absyn_Exp* _temp507; struct Cyc_Absyn_Exp* _temp509; struct
Cyc_Absyn_Exp* _temp511; _LL457: if(*(( int*) _temp455) ==  Cyc_Absyn_Var_e){
_LL480: _temp479=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp455)->f2; if((
unsigned int) _temp479 >  1u?*(( int*) _temp479) ==  Cyc_Absyn_Param_b: 0){
_LL482: _temp481=(( struct Cyc_Absyn_Param_b_struct*) _temp479)->f1; goto _LL458;}
else{ goto _LL459;}} else{ goto _LL459;} _LL459: if(*(( int*) _temp455) ==  Cyc_Absyn_Var_e){
_LL484: _temp483=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp455)->f2; if((
unsigned int) _temp483 >  1u?*(( int*) _temp483) ==  Cyc_Absyn_Local_b: 0){
_LL486: _temp485=(( struct Cyc_Absyn_Local_b_struct*) _temp483)->f1; goto _LL460;}
else{ goto _LL461;}} else{ goto _LL461;} _LL461: if(*(( int*) _temp455) ==  Cyc_Absyn_Var_e){
_LL488: _temp487=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp455)->f2; if((
unsigned int) _temp487 >  1u?*(( int*) _temp487) ==  Cyc_Absyn_Pat_b: 0){ _LL490:
_temp489=(( struct Cyc_Absyn_Pat_b_struct*) _temp487)->f1; goto _LL462;} else{
goto _LL463;}} else{ goto _LL463;} _LL463: if(*(( int*) _temp455) ==  Cyc_Absyn_Var_e){
_LL492: _temp491=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp455)->f2; if((
unsigned int) _temp491 >  1u?*(( int*) _temp491) ==  Cyc_Absyn_Global_b: 0){
_LL494: _temp493=(( struct Cyc_Absyn_Global_b_struct*) _temp491)->f1; goto
_LL464;} else{ goto _LL465;}} else{ goto _LL465;} _LL465: if(*(( int*) _temp455)
==  Cyc_Absyn_Subscript_e){ _LL498: _temp497=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp455)->f1; goto _LL496; _LL496: _temp495=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp455)->f2; goto _LL466;} else{ goto _LL467;} _LL467: if(*(( int*) _temp455)
==  Cyc_Absyn_StructMember_e){ _LL502: _temp501=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp455)->f1; goto _LL500; _LL500: _temp499=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp455)->f2; goto _LL468;} else{ goto _LL469;} _LL469: if(*(( int*) _temp455)
==  Cyc_Absyn_StructArrow_e){ _LL506: _temp505=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp455)->f1; goto _LL504; _LL504: _temp503=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp455)->f2; goto _LL470;} else{ goto _LL471;} _LL471: if(*(( int*) _temp455)
==  Cyc_Absyn_Deref_e){ _LL508: _temp507=(( struct Cyc_Absyn_Deref_e_struct*)
_temp455)->f1; goto _LL472;} else{ goto _LL473;} _LL473: if(*(( int*) _temp455)
==  Cyc_Absyn_NoInstantiate_e){ _LL510: _temp509=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp455)->f1; goto _LL474;} else{ goto _LL475;} _LL475: if(*(( int*) _temp455)
==  Cyc_Absyn_Instantiate_e){ _LL512: _temp511=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp455)->f1; goto _LL476;} else{ goto _LL477;} _LL477: goto _LL478; _LL458:
_temp485= _temp481; goto _LL460; _LL460: _temp489= _temp485; goto _LL462; _LL462:
_temp493= _temp489; goto _LL464; _LL464: if( !( _temp493->tq).q_const){ return;}
goto _LL456; _LL466:{ void* _temp513= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp497->topt))->v); struct Cyc_Absyn_PtrInfo _temp523; struct Cyc_Absyn_Tqual
_temp525; struct Cyc_Absyn_Tqual _temp527; struct Cyc_List_List* _temp529;
_LL515: if(( unsigned int) _temp513 >  4u?*(( int*) _temp513) ==  Cyc_Absyn_PointerType:
0){ _LL524: _temp523=(( struct Cyc_Absyn_PointerType_struct*) _temp513)->f1;
_LL526: _temp525= _temp523.tq; goto _LL516;} else{ goto _LL517;} _LL517: if((
unsigned int) _temp513 >  4u?*(( int*) _temp513) ==  Cyc_Absyn_ArrayType: 0){
_LL528: _temp527=(( struct Cyc_Absyn_ArrayType_struct*) _temp513)->f2; goto
_LL518;} else{ goto _LL519;} _LL519: if(( unsigned int) _temp513 >  4u?*(( int*)
_temp513) ==  Cyc_Absyn_TupleType: 0){ _LL530: _temp529=(( struct Cyc_Absyn_TupleType_struct*)
_temp513)->f1; goto _LL520;} else{ goto _LL521;} _LL521: goto _LL522; _LL516:
_temp527= _temp525; goto _LL518; _LL518: if( ! _temp527.q_const){ return;} goto
_LL514; _LL520: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp495);{
struct _handler_cons _temp531; _push_handler(& _temp531);{ int _temp533= 0; if(
setjmp( _temp531.handler)){ _temp533= 1;} if( ! _temp533){{ struct _tuple8
_temp536; struct Cyc_Absyn_Tqual _temp537; struct _tuple8* _temp534=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp529,( int) i);
_temp536=* _temp534; _LL538: _temp537= _temp536.f1; goto _LL535; _LL535: if( !
_temp537.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp532=( void*) _exn_thrown; void* _temp540= _temp532; _LL542: if( _temp540 == 
Cyc_List_Nth){ goto _LL543;} else{ goto _LL544;} _LL544: goto _LL545; _LL543:
return; _LL545:( void) _throw( _temp540); _LL541:;}}} goto _LL514;} _LL522: goto
_LL514; _LL514:;} goto _LL456; _LL468:{ void* _temp546= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp501->topt))->v); struct Cyc_Absyn_Structdecl**
_temp558; struct Cyc_List_List* _temp560; struct Cyc_List_List* _temp562; struct
Cyc_Absyn_Uniondecl** _temp564; _LL548: if(( unsigned int) _temp546 >  4u?*((
int*) _temp546) ==  Cyc_Absyn_StructType: 0){ _LL559: _temp558=(( struct Cyc_Absyn_StructType_struct*)
_temp546)->f3; goto _LL549;} else{ goto _LL550;} _LL550: if(( unsigned int)
_temp546 >  4u?*(( int*) _temp546) ==  Cyc_Absyn_AnonUnionType: 0){ _LL561:
_temp560=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp546)->f1; goto _LL551;}
else{ goto _LL552;} _LL552: if(( unsigned int) _temp546 >  4u?*(( int*) _temp546)
==  Cyc_Absyn_AnonStructType: 0){ _LL563: _temp562=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp546)->f1; goto _LL553;} else{ goto _LL554;} _LL554: if(( unsigned int)
_temp546 >  4u?*(( int*) _temp546) ==  Cyc_Absyn_UnionType: 0){ _LL565: _temp564=((
struct Cyc_Absyn_UnionType_struct*) _temp546)->f3; goto _LL555;} else{ goto
_LL556;} _LL556: goto _LL557; _LL549: { struct Cyc_Absyn_Structfield* sf=
_temp558 ==  0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*
_temp558, _temp499); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL547;}
_LL551: _temp562= _temp560; goto _LL553; _LL553: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp562, _temp499); if( sf ==  0? 1: !( sf->tq).q_const){
return;} goto _LL547;} _LL555: { struct Cyc_Absyn_Structfield* sf= _temp564 == 
0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)* _temp564,
_temp499); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL547;} _LL557:
goto _LL547; _LL547:;} goto _LL456; _LL470:{ void* _temp566= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp505->topt))->v); struct Cyc_Absyn_PtrInfo
_temp572; void* _temp574; _LL568: if(( unsigned int) _temp566 >  4u?*(( int*)
_temp566) ==  Cyc_Absyn_PointerType: 0){ _LL573: _temp572=(( struct Cyc_Absyn_PointerType_struct*)
_temp566)->f1; _LL575: _temp574=( void*) _temp572.elt_typ; goto _LL569;} else{
goto _LL570;} _LL570: goto _LL571; _LL569:{ void* _temp576= Cyc_Tcutil_compress(
_temp574); struct Cyc_Absyn_Structdecl** _temp588; struct Cyc_Absyn_Uniondecl**
_temp590; struct Cyc_List_List* _temp592; struct Cyc_List_List* _temp594; _LL578:
if(( unsigned int) _temp576 >  4u?*(( int*) _temp576) ==  Cyc_Absyn_StructType:
0){ _LL589: _temp588=(( struct Cyc_Absyn_StructType_struct*) _temp576)->f3; goto
_LL579;} else{ goto _LL580;} _LL580: if(( unsigned int) _temp576 >  4u?*(( int*)
_temp576) ==  Cyc_Absyn_UnionType: 0){ _LL591: _temp590=(( struct Cyc_Absyn_UnionType_struct*)
_temp576)->f3; goto _LL581;} else{ goto _LL582;} _LL582: if(( unsigned int)
_temp576 >  4u?*(( int*) _temp576) ==  Cyc_Absyn_AnonStructType: 0){ _LL593:
_temp592=(( struct Cyc_Absyn_AnonStructType_struct*) _temp576)->f1; goto _LL583;}
else{ goto _LL584;} _LL584: if(( unsigned int) _temp576 >  4u?*(( int*) _temp576)
==  Cyc_Absyn_AnonUnionType: 0){ _LL595: _temp594=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp576)->f1; goto _LL585;} else{ goto _LL586;} _LL586: goto _LL587; _LL579: {
struct Cyc_Absyn_Structfield* sf= _temp588 ==  0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)* _temp588, _temp503); if( sf ==  0? 1: !( sf->tq).q_const){
return;} goto _LL577;} _LL581: { struct Cyc_Absyn_Structfield* sf= _temp590 == 
0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)* _temp590,
_temp503); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL577;} _LL583:
_temp594= _temp592; goto _LL585; _LL585: { struct Cyc_Absyn_Structfield* sf= Cyc_Absyn_lookup_field(
_temp594, _temp503); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL577;}
_LL587: goto _LL577; _LL577:;} goto _LL567; _LL571: goto _LL567; _LL567:;} goto
_LL456; _LL472:{ void* _temp596= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp507->topt))->v); struct Cyc_Absyn_PtrInfo _temp604; struct Cyc_Absyn_Tqual
_temp606; struct Cyc_Absyn_Tqual _temp608; _LL598: if(( unsigned int) _temp596 > 
4u?*(( int*) _temp596) ==  Cyc_Absyn_PointerType: 0){ _LL605: _temp604=(( struct
Cyc_Absyn_PointerType_struct*) _temp596)->f1; _LL607: _temp606= _temp604.tq;
goto _LL599;} else{ goto _LL600;} _LL600: if(( unsigned int) _temp596 >  4u?*((
int*) _temp596) ==  Cyc_Absyn_ArrayType: 0){ _LL609: _temp608=(( struct Cyc_Absyn_ArrayType_struct*)
_temp596)->f2; goto _LL601;} else{ goto _LL602;} _LL602: goto _LL603; _LL599:
_temp608= _temp606; goto _LL601; _LL601: if( ! _temp608.q_const){ return;} goto
_LL597; _LL603: goto _LL597; _LL597:;} goto _LL456; _LL474: _temp511= _temp509;
goto _LL476; _LL476: Cyc_Tcexp_check_writable( te, _temp511); return; _LL478:
goto _LL456; _LL456:;}({ struct Cyc_Std_String_pa_struct _temp611; _temp611.tag=
Cyc_Std_String_pa; _temp611.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{
void* _temp610[ 1u]={& _temp611}; Cyc_Tcutil_terr( e->loc, _tag_arr("attempt to write a const location: %s",
sizeof( unsigned char), 38u), _tag_arr( _temp610, sizeof( void*), 1u));}});}
static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return({ void* _temp612[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("increment/decrement of non-lvalue", sizeof( unsigned char),
34u), _tag_arr( _temp612, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable( te,
e);{ void* t=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( !
Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){({ struct
Cyc_Std_String_pa_struct _temp614; _temp614.tag= Cyc_Std_String_pa; _temp614.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp613[ 1u]={& _temp614};
Cyc_Tcutil_terr( e->loc, _tag_arr("expecting arithmetic or ? type but found %s",
sizeof( unsigned char), 44u), _tag_arr( _temp613, sizeof( void*), 1u));}});}
return t;}} static void* Cyc_Tcexp_tcConditional( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest( te, e1, _tag_arr("conditional expression",
sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp( te, topt, e2); Cyc_Tcexp_tcExp(
te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({
struct Cyc_Core_Opt* _temp622=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp622->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp622;}));
struct Cyc_List_List* _temp615=({ struct Cyc_List_List* _temp621=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp621->hd=( void*) e3; _temp621->tl=
0; _temp621;}); struct Cyc_List_List* _temp616=({ struct Cyc_List_List* _temp620=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp620->hd=(
void*) e2; _temp620->tl= _temp615; _temp620;}); if( ! Cyc_Tcutil_coerce_list( te,
t, _temp616)){({ struct Cyc_Std_String_pa_struct _temp619; _temp619.tag= Cyc_Std_String_pa;
_temp619.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e3->topt))->v);{ struct Cyc_Std_String_pa_struct _temp618; _temp618.tag=
Cyc_Std_String_pa; _temp618.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp617[ 2u]={&
_temp618,& _temp619}; Cyc_Tcutil_terr( loc, _tag_arr("conditional clause types do not match: %s != %s",
sizeof( unsigned char), 48u), _tag_arr( _temp617, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1); Cyc_Tcexp_tcExp(
te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v)),
e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void*
t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ void* _temp623=
Cyc_Tcutil_compress( t1); _LL625: if(( unsigned int) _temp623 >  4u?*(( int*)
_temp623) ==  Cyc_Absyn_ArrayType: 0){ goto _LL626;} else{ goto _LL627;} _LL627:
goto _LL628; _LL626:({ void* _temp629[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("cannot assign to an array",
sizeof( unsigned char), 26u), _tag_arr( _temp629, sizeof( void*), 0u));}); goto
_LL624; _LL628: goto _LL624; _LL624:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return({
void* _temp630[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue",
sizeof( unsigned char), 25u), _tag_arr( _temp630, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e1); if( po ==  0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ void*
_temp631=({ struct Cyc_Std_String_pa_struct _temp634; _temp634.tag= Cyc_Std_String_pa;
_temp634.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp633; _temp633.tag= Cyc_Std_String_pa; _temp633.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp632[ 2u]={& _temp633,& _temp634}; Cyc_Tcexp_expr_err( te, loc,
_tag_arr("type mismatch: %s != %s", sizeof( unsigned char), 24u), _tag_arr(
_temp632, sizeof( void*), 2u));}}}); Cyc_Tcutil_unify( t1, t2); Cyc_Tcutil_explain_failure();
return _temp631;}} else{ void* _temp635=( void*) po->v; void* _temp636= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, _temp635, e1, e2); if( !( Cyc_Tcutil_unify( _temp636, t1)? 1:( Cyc_Tcutil_coerceable(
_temp636)? Cyc_Tcutil_coerceable( t1): 0))){ void* _temp637=({ struct Cyc_Std_String_pa_struct
_temp640; _temp640.tag= Cyc_Std_String_pa; _temp640.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ struct Cyc_Std_String_pa_struct _temp639; _temp639.tag= Cyc_Std_String_pa;
_temp639.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp638[
2u]={& _temp639,& _temp640}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof( unsigned char), 82u), _tag_arr( _temp638, sizeof( void*), 2u));}}}); Cyc_Tcutil_unify(
_temp636, t1); Cyc_Tcutil_explain_failure(); return _temp637;} return _temp636;}
return t1;}} static void* Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v;} static struct Cyc_Absyn_Tunionfield*
Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
tu, struct _RegionHandle* r, struct Cyc_List_List* inst, struct Cyc_List_List*
fs){ struct Cyc_List_List* fields; void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;{ void* _temp641= Cyc_Tcutil_compress( t1); void*
_temp651; void* _temp653; _LL643: if( _temp641 == ( void*) Cyc_Absyn_FloatType){
goto _LL644;} else{ goto _LL645;} _LL645: if(( unsigned int) _temp641 >  4u?*((
int*) _temp641) ==  Cyc_Absyn_IntType: 0){ _LL652: _temp651=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp641)->f2; if( _temp651 == ( void*) Cyc_Absyn_B1){
goto _LL646;} else{ goto _LL647;}} else{ goto _LL647;} _LL647: if(( unsigned int)
_temp641 >  4u?*(( int*) _temp641) ==  Cyc_Absyn_IntType: 0){ _LL654: _temp653=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp641)->f2; if( _temp653 == ( void*)
Cyc_Absyn_B2){ goto _LL648;} else{ goto _LL649;}} else{ goto _LL649;} _LL649:
goto _LL650; _LL644: Cyc_Tcutil_unchecked_cast( te, e,( void*) Cyc_Absyn_DoubleType);
t1=( void*) Cyc_Absyn_DoubleType; goto _LL642; _LL646: goto _LL648; _LL648: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); t1= Cyc_Absyn_sint_t; goto _LL642; _LL650: goto _LL642;
_LL642:;} for( fields= fs; fields !=  0; fields= fields->tl){ void* _temp657;
struct Cyc_Position_Segment* _temp659; struct Cyc_List_List* _temp661; struct
Cyc_List_List* _temp663; struct _tuple1* _temp665; struct Cyc_Absyn_Tunionfield
_temp655=*(( struct Cyc_Absyn_Tunionfield*) fields->hd); _LL666: _temp665=
_temp655.name; goto _LL664; _LL664: _temp663= _temp655.tvs; goto _LL662; _LL662:
_temp661= _temp655.typs; goto _LL660; _LL660: _temp659= _temp655.loc; goto
_LL658; _LL658: _temp657=( void*) _temp655.sc; goto _LL656; _LL656: if((
_temp661 ==  0? 1: _temp661->tl !=  0)? 1: _temp663 !=  0){ continue;}{ void* t2=
Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*) _temp661->hd)).f2); if(
Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)
fields->hd);}}} for( fields= fs; fields !=  0; fields= fields->tl){ void*
_temp669; struct Cyc_Position_Segment* _temp671; struct Cyc_List_List* _temp673;
struct Cyc_List_List* _temp675; struct _tuple1* _temp677; struct Cyc_Absyn_Tunionfield
_temp667=*(( struct Cyc_Absyn_Tunionfield*) fields->hd); _LL678: _temp677=
_temp667.name; goto _LL676; _LL676: _temp675= _temp667.tvs; goto _LL674; _LL674:
_temp673= _temp667.typs; goto _LL672; _LL672: _temp671= _temp667.loc; goto
_LL670; _LL670: _temp669=( void*) _temp667.sc; goto _LL668; _LL668: if((
_temp673 ==  0? 1: _temp673->tl !=  0)? 1: _temp675 !=  0){ continue;}{ void* t2=
Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*) _temp673->hd)).f2); if(
Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*) fields->hd);}}}({ struct Cyc_Std_String_pa_struct
_temp681; _temp681.tag= Cyc_Std_String_pa; _temp681.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ struct Cyc_Std_String_pa_struct _temp680; _temp680.tag= Cyc_Std_String_pa;
_temp680.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( tu);{ void* _temp679[
2u]={& _temp680,& _temp681}; Cyc_Tcutil_terr( e->loc, _tag_arr("can't find a field in %s to inject a value of type %s",
sizeof( unsigned char), 54u), _tag_arr( _temp679, sizeof( void*), 2u));}}});
return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
args, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List*
_temp682= args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{
void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp683= t; struct Cyc_Absyn_PtrInfo _temp689; struct Cyc_Absyn_Conref*
_temp691; struct Cyc_Absyn_Tqual _temp693; struct Cyc_Absyn_Conref* _temp695;
void* _temp697; void* _temp699; _LL685: if(( unsigned int) _temp683 >  4u?*((
int*) _temp683) ==  Cyc_Absyn_PointerType: 0){ _LL690: _temp689=(( struct Cyc_Absyn_PointerType_struct*)
_temp683)->f1; _LL700: _temp699=( void*) _temp689.elt_typ; goto _LL698; _LL698:
_temp697=( void*) _temp689.rgn_typ; goto _LL696; _LL696: _temp695= _temp689.nullable;
goto _LL694; _LL694: _temp693= _temp689.tq; goto _LL692; _LL692: _temp691=
_temp689.bounds; goto _LL686;} else{ goto _LL687;} _LL687: goto _LL688; _LL686:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp697); Cyc_Tcutil_check_nonzero_bound(
loc, _temp691);{ void* _temp701= Cyc_Tcutil_compress( _temp699); struct Cyc_Absyn_FnInfo
_temp707; struct Cyc_List_List* _temp709; struct Cyc_List_List* _temp711; struct
Cyc_Absyn_VarargInfo* _temp713; int _temp715; struct Cyc_List_List* _temp717;
void* _temp719; struct Cyc_Core_Opt* _temp721; struct Cyc_List_List* _temp723;
_LL703: if(( unsigned int) _temp701 >  4u?*(( int*) _temp701) ==  Cyc_Absyn_FnType:
0){ _LL708: _temp707=(( struct Cyc_Absyn_FnType_struct*) _temp701)->f1; _LL724:
_temp723= _temp707.tvars; goto _LL722; _LL722: _temp721= _temp707.effect; goto
_LL720; _LL720: _temp719=( void*) _temp707.ret_typ; goto _LL718; _LL718:
_temp717= _temp707.args; goto _LL716; _LL716: _temp715= _temp707.c_varargs; goto
_LL714; _LL714: _temp713= _temp707.cyc_varargs; goto _LL712; _LL712: _temp711=
_temp707.rgn_po; goto _LL710; _LL710: _temp709= _temp707.attributes; goto _LL704;}
else{ goto _LL705;} _LL705: goto _LL706; _LL704: if( topt !=  0){ Cyc_Tcutil_unify(
_temp719,* topt);} while( _temp682 !=  0? _temp717 !=  0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*) _temp682->hd; void* t2=(*(( struct _tuple2*)
_temp717->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg(
te, e1, t2)){({ struct Cyc_Std_String_pa_struct _temp727; _temp727.tag= Cyc_Std_String_pa;
_temp727.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp726; _temp726.tag= Cyc_Std_String_pa; _temp726.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp725[ 2u]={&
_temp726,& _temp727}; Cyc_Tcutil_terr( e1->loc, _tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof( unsigned char), 54u), _tag_arr( _temp725, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
_temp682= _temp682->tl; _temp717= _temp717->tl;} if( _temp717 !=  0){({ void*
_temp728[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function",
sizeof( unsigned char), 31u), _tag_arr( _temp728, sizeof( void*), 0u));});}
else{ if(( _temp682 !=  0? 1: _temp715)? 1: _temp713 !=  0){ if( _temp715){ for(
0; _temp682 !=  0; _temp682= _temp682->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_temp682->hd);}} else{ if( _temp713 ==  0){({ void* _temp729[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments for function", sizeof( unsigned char), 32u),
_tag_arr( _temp729, sizeof( void*), 0u));});} else{ int _temp732; void* _temp734;
struct Cyc_Absyn_VarargInfo _temp730=* _temp713; _LL735: _temp734=( void*)
_temp730.type; goto _LL733; _LL733: _temp732= _temp730.inject; goto _LL731;
_LL731: { struct Cyc_Absyn_VarargCallInfo* _temp736=({ struct Cyc_Absyn_VarargCallInfo*
_temp766=( struct Cyc_Absyn_VarargCallInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp766->num_varargs= 0; _temp766->injectors= 0; _temp766->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp713); _temp766;});* vararg_call_info=( struct Cyc_Absyn_VarargCallInfo*)
_temp736; if( ! _temp732){ for( 0; _temp682 !=  0; _temp682= _temp682->tl){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp682->hd; _temp736->num_varargs
++; Cyc_Tcexp_tcExp( te,( void**)& _temp734, e1); if( ! Cyc_Tcutil_coerce_arg(
te, e1, _temp734)){({ struct Cyc_Std_String_pa_struct _temp739; _temp739.tag=
Cyc_Std_String_pa; _temp739.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp738; _temp738.tag= Cyc_Std_String_pa; _temp738.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp734);{ void* _temp737[ 2u]={& _temp738,& _temp739}; Cyc_Tcutil_terr( loc,
_tag_arr("vararg requires type %s but argument has type %s", sizeof(
unsigned char), 49u), _tag_arr( _temp737, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}}
else{ void* _temp740= Cyc_Tcutil_compress( _temp734); struct Cyc_Absyn_TunionInfo
_temp746; void* _temp748; struct Cyc_List_List* _temp750; void* _temp752; struct
Cyc_Absyn_Tuniondecl** _temp754; struct Cyc_Absyn_Tuniondecl* _temp756; _LL742:
if(( unsigned int) _temp740 >  4u?*(( int*) _temp740) ==  Cyc_Absyn_TunionType:
0){ _LL747: _temp746=(( struct Cyc_Absyn_TunionType_struct*) _temp740)->f1;
_LL753: _temp752=( void*) _temp746.tunion_info; if(*(( int*) _temp752) ==  Cyc_Absyn_KnownTunion){
_LL755: _temp754=(( struct Cyc_Absyn_KnownTunion_struct*) _temp752)->f1;
_temp756=* _temp754; goto _LL751;} else{ goto _LL744;} _LL751: _temp750=
_temp746.targs; goto _LL749; _LL749: _temp748=( void*) _temp746.rgn; goto _LL743;}
else{ goto _LL744;} _LL744: goto _LL745; _LL743: { struct Cyc_Absyn_Tuniondecl*
_temp757=* Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp756->name); struct Cyc_List_List*
fields= 0; if( _temp757->fields ==  0){({ struct Cyc_Std_String_pa_struct
_temp759; _temp759.tag= Cyc_Std_String_pa; _temp759.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp734);{ void* _temp758[ 1u]={& _temp759}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject into %s",
sizeof( unsigned char), 21u), _tag_arr( _temp758, sizeof( void*), 1u));}});}
else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp757->fields))->v;} if( ! Cyc_Tcutil_unify( _temp748, Cyc_Tcenv_curr_rgn( te))){({
void* _temp760[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad region for injected varargs",
sizeof( unsigned char), 32u), _tag_arr( _temp760, sizeof( void*), 0u));});}{
struct _RegionHandle _temp761= _new_region(); struct _RegionHandle* rgn=&
_temp761; _push_region( rgn);{ struct Cyc_List_List* _temp762=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp757->tvs, _temp750);
for( 0; _temp682 !=  0; _temp682= _temp682->tl){ _temp736->num_varargs ++;{
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp682->hd; Cyc_Tcexp_tcExp(
te, 0, e1);{ struct Cyc_Absyn_Tunionfield* _temp763= Cyc_Tcexp_tcInjection( te,
e1, _temp734, rgn, _temp762, fields); if( _temp763 !=  0){ _temp736->injectors=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp736->injectors,({ struct Cyc_List_List* _temp764=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp764->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)
_check_null( _temp763)); _temp764->tl= 0; _temp764;}));}}}}}; _pop_region( rgn);}
goto _LL741;} _LL745:({ void* _temp765[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad inject vararg type",
sizeof( unsigned char), 23u), _tag_arr( _temp765, sizeof( void*), 0u));}); goto
_LL741; _LL741:;}}}}}}{ struct Cyc_List_List* a= _temp709; for( 0; a !=  0; a= a->tl){
void* _temp767=( void*) a->hd; int _temp773; int _temp775; void* _temp777;
_LL769: if(( unsigned int) _temp767 >  16u?*(( int*) _temp767) ==  Cyc_Absyn_Format_att:
0){ _LL778: _temp777=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp767)->f1;
goto _LL776; _LL776: _temp775=(( struct Cyc_Absyn_Format_att_struct*) _temp767)->f2;
goto _LL774; _LL774: _temp773=(( struct Cyc_Absyn_Format_att_struct*) _temp767)->f3;
goto _LL770;} else{ goto _LL771;} _LL771: goto _LL772; _LL770:{ struct
_handler_cons _temp779; _push_handler(& _temp779);{ int _temp781= 0; if( setjmp(
_temp779.handler)){ _temp781= 1;} if( ! _temp781){{ struct Cyc_Absyn_Exp*
_temp782=(( struct Cyc_Absyn_Exp*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)(
args, _temp775 -  1); struct Cyc_Core_Opt* fmt_args; if( _temp773 ==  0){
fmt_args= 0;} else{ fmt_args=({ struct Cyc_Core_Opt* _temp783=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp783->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth_tail)( args, _temp773 -  1);
_temp783;});}{ void* _temp784= _temp777; _LL786: if( _temp784 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL787;} else{ goto _LL788;} _LL788: if( _temp784 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL789;} else{ goto _LL785;} _LL787: Cyc_Tcexp_check_format_args( te,
_temp782, fmt_args, Cyc_Formatstr_get_format_typs); goto _LL785; _LL789: Cyc_Tcexp_check_format_args(
te, _temp782, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL785; _LL785:;}};
_pop_handler();} else{ void* _temp780=( void*) _exn_thrown; void* _temp791=
_temp780; _LL793: if( _temp791 ==  Cyc_List_Nth){ goto _LL794;} else{ goto
_LL795;} _LL795: goto _LL796; _LL794:({ void* _temp797[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad format arguments", sizeof( unsigned char), 21u), _tag_arr(
_temp797, sizeof( void*), 0u));}); goto _LL792; _LL796:( void) _throw( _temp791);
_LL792:;}}} goto _LL768; _LL772: goto _LL768; _LL768:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp721))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp711); return _temp719; _LL706: return({ void* _temp798[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function", sizeof(
unsigned char), 29u), _tag_arr( _temp798, sizeof( void*), 0u));}); _LL702:;}
_LL688: return({ void* _temp799[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u), _tag_arr( _temp799, sizeof( void*), 0u));});
_LL684:;}} static void* Cyc_Tcexp_tcThrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ,
e); if( ! Cyc_Tcutil_coerce_arg( te, e, Cyc_Absyn_exn_typ)){({ struct Cyc_Std_String_pa_struct
_temp801; _temp801.tag= Cyc_Std_String_pa; _temp801.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp800[ 1u]={&
_temp801}; Cyc_Tcutil_terr( loc, _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp800, sizeof( void*), 1u));}});}
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp802=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp802->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp802;}));} static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp803= t1;
struct Cyc_Absyn_PtrInfo _temp809; struct Cyc_Absyn_Conref* _temp811; struct Cyc_Absyn_Tqual
_temp813; struct Cyc_Absyn_Conref* _temp815; void* _temp817; void* _temp819;
_LL805: if(( unsigned int) _temp803 >  4u?*(( int*) _temp803) ==  Cyc_Absyn_PointerType:
0){ _LL810: _temp809=(( struct Cyc_Absyn_PointerType_struct*) _temp803)->f1;
_LL820: _temp819=( void*) _temp809.elt_typ; goto _LL818; _LL818: _temp817=( void*)
_temp809.rgn_typ; goto _LL816; _LL816: _temp815= _temp809.nullable; goto _LL814;
_LL814: _temp813= _temp809.tq; goto _LL812; _LL812: _temp811= _temp809.bounds;
goto _LL806;} else{ goto _LL807;} _LL807: goto _LL808; _LL806:{ void* _temp821=
Cyc_Tcutil_compress( _temp819); struct Cyc_Absyn_FnInfo _temp827; struct Cyc_List_List*
_temp829; struct Cyc_List_List* _temp831; struct Cyc_Absyn_VarargInfo* _temp833;
int _temp835; struct Cyc_List_List* _temp837; void* _temp839; struct Cyc_Core_Opt*
_temp841; struct Cyc_List_List* _temp843; _LL823: if(( unsigned int) _temp821 > 
4u?*(( int*) _temp821) ==  Cyc_Absyn_FnType: 0){ _LL828: _temp827=(( struct Cyc_Absyn_FnType_struct*)
_temp821)->f1; _LL844: _temp843= _temp827.tvars; goto _LL842; _LL842: _temp841=
_temp827.effect; goto _LL840; _LL840: _temp839=( void*) _temp827.ret_typ; goto
_LL838; _LL838: _temp837= _temp827.args; goto _LL836; _LL836: _temp835= _temp827.c_varargs;
goto _LL834; _LL834: _temp833= _temp827.cyc_varargs; goto _LL832; _LL832:
_temp831= _temp827.rgn_po; goto _LL830; _LL830: _temp829= _temp827.attributes;
goto _LL824;} else{ goto _LL825;} _LL825: goto _LL826; _LL824: { struct Cyc_List_List*
instantiation= 0; if( Cyc_List_length( ts) != (( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp843)){ return({ void* _temp845[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u),
_tag_arr( _temp845, sizeof( void*), 0u));});} for( 0; ts !=  0;( ts= ts->tl,
_temp843= _temp843->tl)){ void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp843))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k, 1,( void*) ts->hd); instantiation=({
struct Cyc_List_List* _temp846=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp846->hd=( void*)({ struct _tuple5* _temp847=(
struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp847->f1=( struct Cyc_Absyn_Tvar*)
_temp843->hd; _temp847->f2=( void*) ts->hd; _temp847;}); _temp846->tl=
instantiation; _temp846;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp851=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp851[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp852; _temp852.tag= Cyc_Absyn_FnType; _temp852.f1=({ struct Cyc_Absyn_FnInfo
_temp853; _temp853.tvars= 0; _temp853.effect= _temp841; _temp853.ret_typ=( void*)
_temp839; _temp853.args= _temp837; _temp853.c_varargs= _temp835; _temp853.cyc_varargs=
_temp833; _temp853.rgn_po= _temp831; _temp853.attributes= _temp829; _temp853;});
_temp852;}); _temp851;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp848=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp848[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp849; _temp849.tag= Cyc_Absyn_PointerType;
_temp849.f1=({ struct Cyc_Absyn_PtrInfo _temp850; _temp850.elt_typ=( void*)
new_fn_typ; _temp850.rgn_typ=( void*) _temp817; _temp850.nullable= _temp815;
_temp850.tq= _temp813; _temp850.bounds= _temp811; _temp850;}); _temp849;});
_temp848;}); return new_typ;}} _LL826: goto _LL822; _LL822:;} goto _LL804;
_LL808: goto _LL804; _LL804:;} return({ struct Cyc_Std_String_pa_struct _temp855;
_temp855.tag= Cyc_Std_String_pa; _temp855.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp854[ 1u]={& _temp855}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting polymorphic type but found %s",
sizeof( unsigned char), 40u), _tag_arr( _temp854, sizeof( void*), 1u));}});}}
static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t); Cyc_Tcexp_tcExp(
te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0){ Cyc_Tcutil_unify( t2, t);{ void* _temp856=({ struct Cyc_Std_String_pa_struct
_temp859; _temp859.tag= Cyc_Std_String_pa; _temp859.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp858; _temp858.tag= Cyc_Std_String_pa;
_temp858.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp857[
2u]={& _temp858,& _temp859}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u), _tag_arr( _temp857, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();
return _temp856;}} return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
void** _temp860= 0; struct Cyc_Absyn_Tqual _temp861= Cyc_Absyn_empty_tqual();
if( topt !=  0){ void* _temp862= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp868; struct Cyc_Absyn_Tqual _temp870; void* _temp872; _LL864: if((
unsigned int) _temp862 >  4u?*(( int*) _temp862) ==  Cyc_Absyn_PointerType: 0){
_LL869: _temp868=(( struct Cyc_Absyn_PointerType_struct*) _temp862)->f1; _LL873:
_temp872=( void*) _temp868.elt_typ; goto _LL871; _LL871: _temp870= _temp868.tq;
goto _LL865;} else{ goto _LL866;} _LL866: goto _LL867; _LL865: _temp860=({ void**
_temp874=( void**) _cycalloc( sizeof( void*)); _temp874[ 0]= _temp872; _temp874;});
_temp861= _temp870; goto _LL863; _LL867: goto _LL863; _LL863:;} Cyc_Tcexp_tcExpNoInst(
te, _temp860, e);{ void* _temp875=( void*) e->r; _LL877: if(*(( int*) _temp875)
==  Cyc_Absyn_Struct_e){ goto _LL878;} else{ goto _LL879;} _LL879: if(*(( int*)
_temp875) ==  Cyc_Absyn_Tuple_e){ goto _LL880;} else{ goto _LL881;} _LL881: goto
_LL882; _LL878: goto _LL880; _LL880:({ void* _temp883[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("& used to allocate", sizeof( unsigned char), 19u), _tag_arr(
_temp883, sizeof( void*), 0u));});{ void*(* _temp884)( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt !=  0){ void* _temp885=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp891; struct Cyc_Absyn_Conref*
_temp893; _LL887: if(( unsigned int) _temp885 >  4u?*(( int*) _temp885) ==  Cyc_Absyn_PointerType:
0){ _LL892: _temp891=(( struct Cyc_Absyn_PointerType_struct*) _temp885)->f1;
_LL894: _temp893= _temp891.nullable; goto _LL888;} else{ goto _LL889;} _LL889:
goto _LL890; _LL888: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp893)){ _temp884= Cyc_Absyn_star_typ;} goto _LL886; _LL890: goto _LL886;
_LL886:;} return _temp884(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp861);} _LL882: goto _LL876; _LL876:;}{ void*
_temp897; int _temp899; struct _tuple6 _temp895= Cyc_Tcutil_addressof_props( te,
e); _LL900: _temp899= _temp895.f1; goto _LL898; _LL898: _temp897= _temp895.f2;
goto _LL896; _LL896: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp899){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp897, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
1, t); if( ! Cyc_Evexp_okay_szofarg( t)){({ struct Cyc_Std_String_pa_struct
_temp902; _temp902.tag= Cyc_Std_String_pa; _temp902.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp901[ 1u]={& _temp902}; Cyc_Tcutil_terr( loc, _tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof( unsigned char), 55u), _tag_arr( _temp901, sizeof( void*), 1u));}});}
return Cyc_Absyn_uint_t;} int Cyc_Tcexp_structfield_has_name( struct _tagged_arr*
n, struct Cyc_Absyn_Structfield* sf){ return Cyc_Std_strcmp(* n,* sf->name) == 
0;} static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, void* n){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, 1, t);{ void* _temp903= n; struct _tagged_arr*
_temp909; unsigned int _temp911; _LL905: if(*(( int*) _temp903) ==  Cyc_Absyn_StructField){
_LL910: _temp909=(( struct Cyc_Absyn_StructField_struct*) _temp903)->f1; goto
_LL906;} else{ goto _LL907;} _LL907: if(*(( int*) _temp903) ==  Cyc_Absyn_TupleIndex){
_LL912: _temp911=(( struct Cyc_Absyn_TupleIndex_struct*) _temp903)->f1; goto
_LL908;} else{ goto _LL904;} _LL906: { int bad_type= 1;{ void* _temp913= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp925; struct _tuple1* _temp927; struct Cyc_List_List*
_temp929; struct Cyc_Absyn_Uniondecl** _temp931; struct _tuple1* _temp933;
struct Cyc_List_List* _temp935; _LL915: if(( unsigned int) _temp913 >  4u?*((
int*) _temp913) ==  Cyc_Absyn_StructType: 0){ _LL928: _temp927=(( struct Cyc_Absyn_StructType_struct*)
_temp913)->f1; goto _LL926; _LL926: _temp925=(( struct Cyc_Absyn_StructType_struct*)
_temp913)->f3; goto _LL916;} else{ goto _LL917;} _LL917: if(( unsigned int)
_temp913 >  4u?*(( int*) _temp913) ==  Cyc_Absyn_AnonStructType: 0){ _LL930:
_temp929=(( struct Cyc_Absyn_AnonStructType_struct*) _temp913)->f1; goto _LL918;}
else{ goto _LL919;} _LL919: if(( unsigned int) _temp913 >  4u?*(( int*) _temp913)
==  Cyc_Absyn_UnionType: 0){ _LL934: _temp933=(( struct Cyc_Absyn_UnionType_struct*)
_temp913)->f1; goto _LL932; _LL932: _temp931=(( struct Cyc_Absyn_UnionType_struct*)
_temp913)->f3; goto _LL920;} else{ goto _LL921;} _LL921: if(( unsigned int)
_temp913 >  4u?*(( int*) _temp913) ==  Cyc_Absyn_AnonUnionType: 0){ _LL936:
_temp935=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp913)->f1; goto _LL922;}
else{ goto _LL923;} _LL923: goto _LL924; _LL916: if( _temp925 ==  0){ return({
void* _temp937[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType",
sizeof( unsigned char), 33u), _tag_arr( _temp937, sizeof( void*), 0u));});} if((*
_temp925)->fields ==  0){ goto _LL914;} if( !(( int(*)( int(* pred)( struct
_tagged_arr*, struct Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name, _temp909,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp925)->fields))->v)){({
struct Cyc_Std_String_pa_struct _temp939; _temp939.tag= Cyc_Std_String_pa;
_temp939.f1=( struct _tagged_arr)* _temp909;{ void* _temp938[ 1u]={& _temp939};
Cyc_Tcutil_terr( loc, _tag_arr("no field of struct has name %s", sizeof(
unsigned char), 31u), _tag_arr( _temp938, sizeof( void*), 1u));}});} bad_type= 0;
goto _LL914; _LL918: if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct
Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcexp_structfield_has_name, _temp909, _temp929)){({ struct Cyc_Std_String_pa_struct
_temp941; _temp941.tag= Cyc_Std_String_pa; _temp941.f1=( struct _tagged_arr)*
_temp909;{ void* _temp940[ 1u]={& _temp941}; Cyc_Tcutil_terr( loc, _tag_arr("no field of struct has name %s",
sizeof( unsigned char), 31u), _tag_arr( _temp940, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL914; _LL920: if( _temp931 ==  0){ return({ void* _temp942[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp942, sizeof( void*), 0u));});} if((*
_temp931)->fields ==  0){ goto _LL914;} if( !(( int(*)( int(* pred)( struct
_tagged_arr*, struct Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name, _temp909,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp931)->fields))->v)){({
struct Cyc_Std_String_pa_struct _temp944; _temp944.tag= Cyc_Std_String_pa;
_temp944.f1=( struct _tagged_arr)* _temp909;{ void* _temp943[ 1u]={& _temp944};
Cyc_Tcutil_terr( loc, _tag_arr("no field of union has name %s", sizeof(
unsigned char), 30u), _tag_arr( _temp943, sizeof( void*), 1u));}});} bad_type= 0;
goto _LL914; _LL922: if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct
Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcexp_structfield_has_name, _temp909, _temp935)){({ struct Cyc_Std_String_pa_struct
_temp946; _temp946.tag= Cyc_Std_String_pa; _temp946.f1=( struct _tagged_arr)*
_temp909;{ void* _temp945[ 1u]={& _temp946}; Cyc_Tcutil_terr( loc, _tag_arr("no field of anonymous union has name %s",
sizeof( unsigned char), 40u), _tag_arr( _temp945, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL914; _LL924: goto _LL914; _LL914:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp948; _temp948.tag= Cyc_Std_String_pa; _temp948.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp947[ 1u]={& _temp948};
Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union type", sizeof(
unsigned char), 36u), _tag_arr( _temp947, sizeof( void*), 1u));}});} goto _LL904;}
_LL908: { int bad_type= 1;{ void* _temp949= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp965; struct Cyc_List_List* _temp967; struct Cyc_Absyn_Uniondecl** _temp969;
struct Cyc_List_List* _temp971; struct Cyc_List_List* _temp973; struct Cyc_Absyn_TunionFieldInfo
_temp975; void* _temp977; struct Cyc_Absyn_Tunionfield* _temp979; struct Cyc_Absyn_Tuniondecl*
_temp981; _LL951: if(( unsigned int) _temp949 >  4u?*(( int*) _temp949) ==  Cyc_Absyn_StructType:
0){ _LL966: _temp965=(( struct Cyc_Absyn_StructType_struct*) _temp949)->f3; goto
_LL952;} else{ goto _LL953;} _LL953: if(( unsigned int) _temp949 >  4u?*(( int*)
_temp949) ==  Cyc_Absyn_AnonStructType: 0){ _LL968: _temp967=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp949)->f1; goto _LL954;} else{ goto _LL955;} _LL955: if(( unsigned int)
_temp949 >  4u?*(( int*) _temp949) ==  Cyc_Absyn_UnionType: 0){ _LL970: _temp969=((
struct Cyc_Absyn_UnionType_struct*) _temp949)->f3; goto _LL956;} else{ goto
_LL957;} _LL957: if(( unsigned int) _temp949 >  4u?*(( int*) _temp949) ==  Cyc_Absyn_AnonUnionType:
0){ _LL972: _temp971=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp949)->f1;
goto _LL958;} else{ goto _LL959;} _LL959: if(( unsigned int) _temp949 >  4u?*((
int*) _temp949) ==  Cyc_Absyn_TupleType: 0){ _LL974: _temp973=(( struct Cyc_Absyn_TupleType_struct*)
_temp949)->f1; goto _LL960;} else{ goto _LL961;} _LL961: if(( unsigned int)
_temp949 >  4u?*(( int*) _temp949) ==  Cyc_Absyn_TunionFieldType: 0){ _LL976:
_temp975=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp949)->f1; _LL978:
_temp977=( void*) _temp975.field_info; if(*(( int*) _temp977) ==  Cyc_Absyn_KnownTunionfield){
_LL982: _temp981=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp977)->f1;
goto _LL980; _LL980: _temp979=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp977)->f2; goto _LL962;} else{ goto _LL963;}} else{ goto _LL963;} _LL963:
goto _LL964; _LL952: if( _temp965 ==  0){ return({ void* _temp983[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("offsetof on unchecked StructType", sizeof( unsigned char), 33u),
_tag_arr( _temp983, sizeof( void*), 0u));});} if((* _temp965)->fields ==  0){
goto _LL950;} if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp965)->fields))->v)
<=  _temp911){({ struct Cyc_Std_Int_pa_struct _temp986; _temp986.tag= Cyc_Std_Int_pa;
_temp986.f1=( int) _temp911;{ struct Cyc_Std_Int_pa_struct _temp985; _temp985.tag=
Cyc_Std_Int_pa; _temp985.f1=( int)(( unsigned int)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((* _temp965)->fields))->v));{ void* _temp984[ 2u]={& _temp985,&
_temp986}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp984, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL950; _LL954: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp967) <=  _temp911){({ struct Cyc_Std_Int_pa_struct _temp989; _temp989.tag=
Cyc_Std_Int_pa; _temp989.f1=( int) _temp911;{ struct Cyc_Std_Int_pa_struct
_temp988; _temp988.tag= Cyc_Std_Int_pa; _temp988.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp967));{ void* _temp987[ 2u]={&
_temp988,& _temp989}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp987, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL950; _LL956: if( _temp969 ==  0){ return({ void* _temp990[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp990, sizeof( void*), 0u));});} if((*
_temp969)->fields ==  0){ goto _LL950;} if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*
_temp969)->fields))->v) <=  _temp911){({ struct Cyc_Std_Int_pa_struct _temp993;
_temp993.tag= Cyc_Std_Int_pa; _temp993.f1=( int) _temp911;{ struct Cyc_Std_Int_pa_struct
_temp992; _temp992.tag= Cyc_Std_Int_pa; _temp992.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((* _temp969)->fields))->v));{ void* _temp991[ 2u]={& _temp992,&
_temp993}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp991, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL950; _LL958: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp971) <=  _temp911){({ struct Cyc_Std_Int_pa_struct _temp996; _temp996.tag=
Cyc_Std_Int_pa; _temp996.f1=( int) _temp911;{ struct Cyc_Std_Int_pa_struct
_temp995; _temp995.tag= Cyc_Std_Int_pa; _temp995.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp971));{ void* _temp994[ 2u]={&
_temp995,& _temp996}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp994, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL950; _LL960: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp973) <=  _temp911){({ struct Cyc_Std_Int_pa_struct _temp999; _temp999.tag=
Cyc_Std_Int_pa; _temp999.f1=( int) _temp911;{ struct Cyc_Std_Int_pa_struct
_temp998; _temp998.tag= Cyc_Std_Int_pa; _temp998.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp973));{ void* _temp997[ 2u]={&
_temp998,& _temp999}; Cyc_Tcutil_terr( loc, _tag_arr("tuple has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp997, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL950; _LL962: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp979->typs) <  _temp911){({ struct Cyc_Std_Int_pa_struct _temp1002;
_temp1002.tag= Cyc_Std_Int_pa; _temp1002.f1=( int) _temp911;{ struct Cyc_Std_Int_pa_struct
_temp1001; _temp1001.tag= Cyc_Std_Int_pa; _temp1001.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp979->typs));{ void*
_temp1000[ 2u]={& _temp1001,& _temp1002}; Cyc_Tcutil_terr( loc, _tag_arr("tunionfield has too few components: %d < %d",
sizeof( unsigned char), 44u), _tag_arr( _temp1000, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL950; _LL964: goto _LL950; _LL950:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp1004; _temp1004.tag= Cyc_Std_String_pa; _temp1004.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1003[ 1u]={&
_temp1004}; Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union/tuple/tunion type",
sizeof( unsigned char), 49u), _tag_arr( _temp1003, sizeof( void*), 1u));}});}
goto _LL904;} _LL904:;} return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1005= t;
struct Cyc_Absyn_PtrInfo _temp1011; struct Cyc_Absyn_Conref* _temp1013; void*
_temp1015; void* _temp1017; _LL1007: if(( unsigned int) _temp1005 >  4u?*(( int*)
_temp1005) ==  Cyc_Absyn_PointerType: 0){ _LL1012: _temp1011=(( struct Cyc_Absyn_PointerType_struct*)
_temp1005)->f1; _LL1018: _temp1017=( void*) _temp1011.elt_typ; goto _LL1016;
_LL1016: _temp1015=( void*) _temp1011.rgn_typ; goto _LL1014; _LL1014: _temp1013=
_temp1011.bounds; goto _LL1008;} else{ goto _LL1009;} _LL1009: goto _LL1010;
_LL1008: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1015); Cyc_Tcutil_check_nonzero_bound(
loc, _temp1013); return _temp1017; _LL1010: return({ struct Cyc_Std_String_pa_struct
_temp1020; _temp1020.tag= Cyc_Std_String_pa; _temp1020.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1019[ 1u]={& _temp1020}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting * or @ type but found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1019, sizeof( void*), 1u));}}); _LL1006:;}} static void*
Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1021= t;
struct Cyc_Absyn_Structdecl** _temp1037; struct Cyc_Absyn_Structdecl* _temp1039;
struct Cyc_List_List* _temp1040; struct _tuple1* _temp1042; struct Cyc_List_List*
_temp1044; struct Cyc_List_List* _temp1046; struct Cyc_Absyn_Uniondecl**
_temp1048; struct Cyc_Absyn_Uniondecl* _temp1050; struct Cyc_List_List*
_temp1051; struct _tuple1* _temp1053; _LL1023: if(( unsigned int) _temp1021 >  4u?*((
int*) _temp1021) ==  Cyc_Absyn_StructType: 0){ _LL1043: _temp1042=(( struct Cyc_Absyn_StructType_struct*)
_temp1021)->f1; goto _LL1041; _LL1041: _temp1040=(( struct Cyc_Absyn_StructType_struct*)
_temp1021)->f2; goto _LL1038; _LL1038: _temp1037=(( struct Cyc_Absyn_StructType_struct*)
_temp1021)->f3; if( _temp1037 ==  0){ goto _LL1025;} else{ _temp1039=* _temp1037;
goto _LL1024;}} else{ goto _LL1025;} _LL1025: if(( unsigned int) _temp1021 >  4u?*((
int*) _temp1021) ==  Cyc_Absyn_AnonStructType: 0){ _LL1045: _temp1044=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1021)->f1; goto _LL1026;} else{ goto
_LL1027;} _LL1027: if(( unsigned int) _temp1021 >  4u?*(( int*) _temp1021) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1047: _temp1046=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1021)->f1; goto _LL1028;} else{ goto _LL1029;} _LL1029: if(( unsigned int)
_temp1021 >  4u?*(( int*) _temp1021) ==  Cyc_Absyn_UnionType: 0){ _LL1054:
_temp1053=(( struct Cyc_Absyn_UnionType_struct*) _temp1021)->f1; goto _LL1052;
_LL1052: _temp1051=(( struct Cyc_Absyn_UnionType_struct*) _temp1021)->f2; goto
_LL1049; _LL1049: _temp1048=(( struct Cyc_Absyn_UnionType_struct*) _temp1021)->f3;
if( _temp1048 ==  0){ goto _LL1031;} else{ _temp1050=* _temp1048; goto _LL1030;}}
else{ goto _LL1031;} _LL1031: if(( unsigned int) _temp1021 >  4u?*(( int*)
_temp1021) ==  Cyc_Absyn_ArrayType: 0){ goto _LL1055;} else{ goto _LL1033;}
_LL1055: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
==  0){ goto _LL1032;} else{ goto _LL1033;} _LL1033: if(( unsigned int)
_temp1021 >  4u?*(( int*) _temp1021) ==  Cyc_Absyn_PointerType: 0){ goto _LL1056;}
else{ goto _LL1035;} _LL1056: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof(
unsigned char), 5u)) ==  0){ goto _LL1034;} else{ goto _LL1035;} _LL1035: goto
_LL1036; _LL1024: if( _temp1042 ==  0){ return({ void* _temp1057[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved struct type (COMPILER ERROR)", sizeof(
unsigned char), 40u), _tag_arr( _temp1057, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp1058= Cyc_Absyn_lookup_struct_field( _temp1039, f); if( _temp1058 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1061; _temp1061.tag= Cyc_Std_String_pa;
_temp1061.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1060; _temp1060.tag= Cyc_Std_String_pa; _temp1060.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1042));{ void*
_temp1059[ 2u]={& _temp1060,& _temp1061}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp1059, sizeof( void*), 2u));}}});}{
void* t2;{ struct _RegionHandle _temp1062= _new_region(); struct _RegionHandle*
rgn=& _temp1062; _push_region( rgn);{ struct Cyc_List_List* _temp1063=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r1, struct _RegionHandle* r2, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1039->tvs,
_temp1040); t2= Cyc_Tcutil_rsubstitute( rgn, _temp1063,( void*) _temp1058->type);};
_pop_region( rgn);} return t2;}} _LL1026: { struct Cyc_Absyn_Structfield*
_temp1064= Cyc_Absyn_lookup_field( _temp1044, f); if( _temp1064 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1066; _temp1066.tag= Cyc_Std_String_pa;
_temp1066.f1=( struct _tagged_arr)* f;{ void* _temp1065[ 1u]={& _temp1066}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1065, sizeof( void*), 1u));}});} return( void*) _temp1064->type;}
_LL1028: { struct Cyc_Absyn_Structfield* _temp1067= Cyc_Absyn_lookup_field(
_temp1046, f); if( _temp1067 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1069; _temp1069.tag= Cyc_Std_String_pa; _temp1069.f1=( struct _tagged_arr)*
f;{ void* _temp1068[ 1u]={& _temp1069}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp1068, sizeof( void*), 1u));}});}
return( void*) _temp1067->type;} _LL1030: if( _temp1053 ==  0){ return({ void*
_temp1070[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("unresolved union type (COMPILER ERROR)",
sizeof( unsigned char), 39u), _tag_arr( _temp1070, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structfield* _temp1071= Cyc_Absyn_lookup_union_field( _temp1050,
f); if( _temp1071 ==  0){ return({ struct Cyc_Std_String_pa_struct _temp1074;
_temp1074.tag= Cyc_Std_String_pa; _temp1074.f1=( struct _tagged_arr)* f;{ struct
Cyc_Std_String_pa_struct _temp1073; _temp1073.tag= Cyc_Std_String_pa; _temp1073.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp1053));{ void* _temp1072[ 2u]={& _temp1073,& _temp1074}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union %s has no %s field", sizeof( unsigned char), 25u),
_tag_arr( _temp1072, sizeof( void*), 2u));}}});}{ struct Cyc_List_List*
_temp1075=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1050->tvs, _temp1051); void* _temp1076= Cyc_Tcutil_substitute(
_temp1075,( void*) _temp1071->type); return _temp1076;}} _LL1032:( void*)(
outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1077=(
struct Cyc_Absyn_Primop_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp1077[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp1078; _temp1078.tag= Cyc_Absyn_Primop_e;
_temp1078.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1078.f2=({ struct Cyc_List_List*
_temp1079=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1079->hd=( void*) e; _temp1079->tl= 0; _temp1079;}); _temp1078;});
_temp1077;}))); return Cyc_Absyn_uint_t; _LL1034:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1080=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1080[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1081; _temp1081.tag= Cyc_Absyn_Primop_e;
_temp1081.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1081.f2=({ struct Cyc_List_List*
_temp1082=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1082->hd=( void*) e; _temp1082->tl= 0; _temp1082;}); _temp1081;});
_temp1080;}))); return Cyc_Absyn_uint_t; _LL1036: if( Cyc_Std_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1084; _temp1084.tag= Cyc_Std_String_pa; _temp1084.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1083[ 1u]={& _temp1084}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u), _tag_arr( _temp1083, sizeof( void*), 1u));}});} else{
return({ struct Cyc_Std_String_pa_struct _temp1086; _temp1086.tag= Cyc_Std_String_pa;
_temp1086.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1085[
1u]={& _temp1086}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct or union, found %s",
sizeof( unsigned char), 36u), _tag_arr( _temp1085, sizeof( void*), 1u));}});}
_LL1022:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1087= t;
struct Cyc_Absyn_PtrInfo _temp1093; struct Cyc_Absyn_Conref* _temp1095; void*
_temp1097; void* _temp1099; _LL1089: if(( unsigned int) _temp1087 >  4u?*(( int*)
_temp1087) ==  Cyc_Absyn_PointerType: 0){ _LL1094: _temp1093=(( struct Cyc_Absyn_PointerType_struct*)
_temp1087)->f1; _LL1100: _temp1099=( void*) _temp1093.elt_typ; goto _LL1098;
_LL1098: _temp1097=( void*) _temp1093.rgn_typ; goto _LL1096; _LL1096: _temp1095=
_temp1093.bounds; goto _LL1090;} else{ goto _LL1091;} _LL1091: goto _LL1092;
_LL1090: Cyc_Tcutil_check_nonzero_bound( loc, _temp1095);{ void* _temp1101= Cyc_Tcutil_compress(
_temp1099); struct Cyc_Absyn_Structdecl** _temp1113; struct Cyc_Absyn_Structdecl*
_temp1115; struct Cyc_List_List* _temp1116; struct _tuple1* _temp1118; struct
Cyc_Absyn_Uniondecl** _temp1120; struct Cyc_Absyn_Uniondecl* _temp1122; struct
Cyc_List_List* _temp1123; struct _tuple1* _temp1125; struct Cyc_List_List*
_temp1127; struct Cyc_List_List* _temp1129; _LL1103: if(( unsigned int)
_temp1101 >  4u?*(( int*) _temp1101) ==  Cyc_Absyn_StructType: 0){ _LL1119:
_temp1118=(( struct Cyc_Absyn_StructType_struct*) _temp1101)->f1; goto _LL1117;
_LL1117: _temp1116=(( struct Cyc_Absyn_StructType_struct*) _temp1101)->f2; goto
_LL1114; _LL1114: _temp1113=(( struct Cyc_Absyn_StructType_struct*) _temp1101)->f3;
if( _temp1113 ==  0){ goto _LL1105;} else{ _temp1115=* _temp1113; goto _LL1104;}}
else{ goto _LL1105;} _LL1105: if(( unsigned int) _temp1101 >  4u?*(( int*)
_temp1101) ==  Cyc_Absyn_UnionType: 0){ _LL1126: _temp1125=(( struct Cyc_Absyn_UnionType_struct*)
_temp1101)->f1; goto _LL1124; _LL1124: _temp1123=(( struct Cyc_Absyn_UnionType_struct*)
_temp1101)->f2; goto _LL1121; _LL1121: _temp1120=(( struct Cyc_Absyn_UnionType_struct*)
_temp1101)->f3; if( _temp1120 ==  0){ goto _LL1107;} else{ _temp1122=* _temp1120;
goto _LL1106;}} else{ goto _LL1107;} _LL1107: if(( unsigned int) _temp1101 >  4u?*((
int*) _temp1101) ==  Cyc_Absyn_AnonStructType: 0){ _LL1128: _temp1127=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1101)->f1; goto _LL1108;} else{ goto
_LL1109;} _LL1109: if(( unsigned int) _temp1101 >  4u?*(( int*) _temp1101) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1130: _temp1129=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1101)->f1; goto _LL1110;} else{ goto _LL1111;} _LL1111: goto _LL1112;
_LL1104: { struct Cyc_Absyn_Structfield* _temp1131= Cyc_Absyn_lookup_struct_field(
_temp1115, f); if( _temp1131 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1134; _temp1134.tag= Cyc_Std_String_pa; _temp1134.f1=( struct _tagged_arr)*
f;{ struct Cyc_Std_String_pa_struct _temp1133; _temp1133.tag= Cyc_Std_String_pa;
_temp1133.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1118));{ void* _temp1132[ 2u]={& _temp1133,& _temp1134}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct %s has no %s field", sizeof( unsigned char), 26u),
_tag_arr( _temp1132, sizeof( void*), 2u));}}});}{ void* t3;{ struct
_RegionHandle _temp1135= _new_region(); struct _RegionHandle* rgn=& _temp1135;
_push_region( rgn);{ struct Cyc_List_List* _temp1136=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1115->tvs, _temp1116);
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1136,( void*) _temp1131->type);};
_pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1097); return
t3;}} _LL1106: { struct Cyc_Absyn_Structfield* _temp1137= Cyc_Absyn_lookup_union_field(
_temp1122, f); if( _temp1137 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1140; _temp1140.tag= Cyc_Std_String_pa; _temp1140.f1=( struct _tagged_arr)*
f;{ struct Cyc_Std_String_pa_struct _temp1139; _temp1139.tag= Cyc_Std_String_pa;
_temp1139.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1125));{ void* _temp1138[ 2u]={& _temp1139,& _temp1140}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union %s has no %s field", sizeof( unsigned char), 25u),
_tag_arr( _temp1138, sizeof( void*), 2u));}}});}{ struct Cyc_List_List*
_temp1141=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1122->tvs, _temp1123); void* _temp1142= Cyc_Tcutil_substitute(
_temp1141,( void*) _temp1137->type); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1097); return _temp1142;}} _LL1108: { struct Cyc_Absyn_Structfield*
_temp1143= Cyc_Absyn_lookup_field( _temp1127, f); if( _temp1143 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1145; _temp1145.tag= Cyc_Std_String_pa;
_temp1145.f1=( struct _tagged_arr)* f;{ void* _temp1144[ 1u]={& _temp1145}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1144, sizeof( void*), 1u));}});} return( void*) _temp1143->type;}
_LL1110: { struct Cyc_Absyn_Structfield* _temp1146= Cyc_Absyn_lookup_field(
_temp1129, f); if( _temp1146 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1148; _temp1148.tag= Cyc_Std_String_pa; _temp1148.f1=( struct _tagged_arr)*
f;{ void* _temp1147[ 1u]={& _temp1148}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp1147, sizeof( void*), 1u));}});}
return( void*) _temp1146->type;} _LL1112: return({ struct Cyc_Std_String_pa_struct
_temp1150; _temp1150.tag= Cyc_Std_String_pa; _temp1150.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1149[ 1u]={& _temp1150}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct pointer, found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1149, sizeof( void*), 1u));}}); _LL1102:;} _LL1092: return({
struct Cyc_Std_String_pa_struct _temp1152; _temp1152.tag= Cyc_Std_String_pa;
_temp1152.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1151[
1u]={& _temp1152}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp1151, sizeof( void*), 1u));}});
_LL1088:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1153; _push_handler(& _temp1153);{ int _temp1155= 0; if(
setjmp( _temp1153.handler)){ _temp1155= 1;} if( ! _temp1155){{ void* _temp1156=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1156;}; _pop_handler();} else{ void* _temp1154=(
void*) _exn_thrown; void* _temp1158= _temp1154; _LL1160: if( _temp1158 ==  Cyc_List_Nth){
goto _LL1161;} else{ goto _LL1162;} _LL1162: goto _LL1163; _LL1161: return({
struct Cyc_Std_Int_pa_struct _temp1166; _temp1166.tag= Cyc_Std_Int_pa; _temp1166.f1=(
int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts));{
struct Cyc_Std_Int_pa_struct _temp1165; _temp1165.tag= Cyc_Std_Int_pa; _temp1165.f1=(
int) i;{ void* _temp1164[ 2u]={& _temp1165,& _temp1166}; Cyc_Tcexp_expr_err( te,
loc, _tag_arr("index is %d but tuple has only %d fields", sizeof( unsigned char),
41u), _tag_arr( _temp1164, sizeof( void*), 2u));}}}); _LL1163:( void) _throw(
_temp1158); _LL1159:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return({
struct Cyc_Std_String_pa_struct _temp1168; _temp1168.tag= Cyc_Std_String_pa;
_temp1168.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp1167[
1u]={& _temp1168}; Cyc_Tcexp_expr_err( te, e2->loc, _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp1167, sizeof( void*), 1u));}});}{
void* _temp1169= t1; struct Cyc_Absyn_PtrInfo _temp1177; struct Cyc_Absyn_Conref*
_temp1179; struct Cyc_Absyn_Tqual _temp1181; void* _temp1183; void* _temp1185;
struct Cyc_List_List* _temp1187; _LL1171: if(( unsigned int) _temp1169 >  4u?*((
int*) _temp1169) ==  Cyc_Absyn_PointerType: 0){ _LL1178: _temp1177=(( struct Cyc_Absyn_PointerType_struct*)
_temp1169)->f1; _LL1186: _temp1185=( void*) _temp1177.elt_typ; goto _LL1184;
_LL1184: _temp1183=( void*) _temp1177.rgn_typ; goto _LL1182; _LL1182: _temp1181=
_temp1177.tq; goto _LL1180; _LL1180: _temp1179= _temp1177.bounds; goto _LL1172;}
else{ goto _LL1173;} _LL1173: if(( unsigned int) _temp1169 >  4u?*(( int*)
_temp1169) ==  Cyc_Absyn_TupleType: 0){ _LL1188: _temp1187=(( struct Cyc_Absyn_TupleType_struct*)
_temp1169)->f1; goto _LL1174;} else{ goto _LL1175;} _LL1175: goto _LL1176;
_LL1172: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1179);} else{ if( Cyc_Tcutil_is_bound_one( _temp1179)){({ void*
_temp1189[ 0u]={}; Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u), _tag_arr( _temp1189, sizeof( void*), 0u));});} Cyc_Tcutil_check_nonzero_bound(
loc, _temp1179);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1183); return
_temp1185; _LL1174: return Cyc_Tcexp_ithTupleType( te, loc, _temp1187, e2);
_LL1176: return({ struct Cyc_Std_String_pa_struct _temp1191; _temp1191.tag= Cyc_Std_String_pa;
_temp1191.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp1190[
1u]={& _temp1191}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1190, sizeof( void*), 1u));}});
_LL1170:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){ int done= 0;
struct Cyc_List_List* fields= 0; if( topt !=  0){ void* _temp1192= Cyc_Tcutil_compress(*
topt); struct Cyc_List_List* _temp1198; _LL1194: if(( unsigned int) _temp1192 > 
4u?*(( int*) _temp1192) ==  Cyc_Absyn_TupleType: 0){ _LL1199: _temp1198=((
struct Cyc_Absyn_TupleType_struct*) _temp1192)->f1; goto _LL1195;} else{ goto
_LL1196;} _LL1196: goto _LL1197; _LL1195: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp1198) != (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ goto _LL1193;} for( 0; es !=  0;( es= es->tl, _temp1198= _temp1198->tl)){
void* _temp1200=(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
_temp1198))->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1200,(
struct Cyc_Absyn_Exp*) es->hd); fields=({ struct Cyc_List_List* _temp1201=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1201->hd=(
void*)({ struct _tuple8* _temp1202=( struct _tuple8*) _cycalloc( sizeof( struct
_tuple8)); _temp1202->f1=(*(( struct _tuple8*) _temp1198->hd)).f1; _temp1202->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) es->hd)->topt))->v;
_temp1202;}); _temp1201->tl= fields; _temp1201;});} done= 1; goto _LL1193;
_LL1197: goto _LL1193; _LL1193:;} if( ! done){ for( 0; es !=  0; es= es->tl){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*) es->hd); fields=({
struct Cyc_List_List* _temp1203=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1203->hd=( void*)({ struct _tuple8* _temp1204=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1204->f1= Cyc_Absyn_empty_tqual();
_temp1204->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)
es->hd)->topt))->v; _temp1204;}); _temp1203->tl= fields; _temp1203;});}} return(
void*)({ struct Cyc_Absyn_TupleType_struct* _temp1205=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1205[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1206; _temp1206.tag= Cyc_Absyn_TupleType;
_temp1206.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1206;}); _temp1205;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return({ void* _temp1207[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("tcCompoundLit", sizeof( unsigned char), 14u), _tag_arr(
_temp1207, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1230=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1230->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1230;})); struct Cyc_Absyn_Const_e_struct* _temp1208=({ struct Cyc_Absyn_Const_e_struct*
_temp1226=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1226[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1227; _temp1227.tag= Cyc_Absyn_Const_e;
_temp1227.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1228=(
struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1228[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1229; _temp1229.tag= Cyc_Absyn_Int_c;
_temp1229.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1229.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1229;}); _temp1228;}));
_temp1227;}); _temp1226;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) _temp1208, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1209=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1209->v=( void*)
Cyc_Absyn_uint_t; _temp1209;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1224=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1224[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1225; _temp1225.tag= Cyc_Absyn_ArrayType;
_temp1225.f1=( void*) res; _temp1225.f2= Cyc_Absyn_empty_tqual(); _temp1225.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1225;}); _temp1224;});{ struct Cyc_List_List*
es2= es; for( 0; es2 !=  0; es2= es2->tl){ Cyc_Tcexp_tcExpInitializer( te,
elt_topt,( struct Cyc_Absyn_Exp*) es2->hd);}} if( ! Cyc_Tcutil_coerce_list( te,
res, es)){({ struct Cyc_Std_String_pa_struct _temp1211; _temp1211.tag= Cyc_Std_String_pa;
_temp1211.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( res);{ void*
_temp1210[ 1u]={& _temp1211}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->loc, _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp1210, sizeof( void*), 1u));}});}{
int offset= 0; for( 0; des !=  0;( offset ++, des= des->tl)){ struct Cyc_List_List*
ds=(*(( struct _tuple7*) des->hd)).f1; if( ds !=  0){ void* _temp1212=( void*)
ds->hd; struct Cyc_Absyn_Exp* _temp1218; _LL1214: if(*(( int*) _temp1212) == 
Cyc_Absyn_FieldName){ goto _LL1215;} else{ goto _LL1216;} _LL1216: if(*(( int*)
_temp1212) ==  Cyc_Absyn_ArrayElement){ _LL1219: _temp1218=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1212)->f1; goto _LL1217;} else{ goto _LL1213;} _LL1215:({ void* _temp1220[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("only array index designators are supported",
sizeof( unsigned char), 43u), _tag_arr( _temp1220, sizeof( void*), 0u));}); goto
_LL1213; _LL1217: Cyc_Tcexp_tcExpInitializer( te, 0, _temp1218);{ unsigned int i=
Cyc_Evexp_eval_const_uint_exp( _temp1218); if( i !=  offset){({ struct Cyc_Std_Int_pa_struct
_temp1223; _temp1223.tag= Cyc_Std_Int_pa; _temp1223.f1=( int) i;{ struct Cyc_Std_Int_pa_struct
_temp1222; _temp1222.tag= Cyc_Std_Int_pa; _temp1222.f1=( int)(( unsigned int)
offset);{ void* _temp1221[ 2u]={& _temp1222,& _temp1223}; Cyc_Tcutil_terr(
_temp1218->loc, _tag_arr("expecting index designator %d but found %d", sizeof(
unsigned char), 43u), _tag_arr( _temp1221, sizeof( void*), 2u));}}});} goto
_LL1213;} _LL1213:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){({
struct Cyc_Std_String_pa_struct _temp1232; _temp1232.tag= Cyc_Std_String_pa;
_temp1232.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v);{ void* _temp1231[ 1u]={& _temp1232}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("expecting unsigned int, found %s", sizeof( unsigned char),
33u), _tag_arr( _temp1231, sizeof( void*), 1u));}});} if( !( vd->tq).q_const){({
void* _temp1233[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u), _tag_arr( _temp1233, sizeof( void*), 0u));});} if(
te->le !=  0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);}{ struct Cyc_Absyn_PtrInfo pinfo; void** _temp1234= 0; struct Cyc_Absyn_Tqual*
_temp1235= 0; if( topt !=  0){ void* _temp1236= Cyc_Tcutil_compress(* topt);
struct Cyc_Absyn_PtrInfo _temp1244; struct Cyc_Absyn_Exp* _temp1246; struct Cyc_Absyn_Tqual
_temp1248; struct Cyc_Absyn_Tqual* _temp1250; void* _temp1251; void** _temp1253;
_LL1238: if(( unsigned int) _temp1236 >  4u?*(( int*) _temp1236) ==  Cyc_Absyn_PointerType:
0){ _LL1245: _temp1244=(( struct Cyc_Absyn_PointerType_struct*) _temp1236)->f1;
goto _LL1239;} else{ goto _LL1240;} _LL1240: if(( unsigned int) _temp1236 >  4u?*((
int*) _temp1236) ==  Cyc_Absyn_ArrayType: 0){ _LL1252: _temp1251=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1236)->f1; _temp1253=( void**)&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1236)->f1); goto _LL1249; _LL1249:
_temp1248=(( struct Cyc_Absyn_ArrayType_struct*) _temp1236)->f2; _temp1250=(
struct Cyc_Absyn_Tqual*)&(( struct Cyc_Absyn_ArrayType_struct*) _temp1236)->f2;
goto _LL1247; _LL1247: _temp1246=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1236)->f3; goto _LL1241;} else{ goto _LL1242;} _LL1242: goto _LL1243;
_LL1239: pinfo= _temp1244; _temp1234=( void**)(( void**)(( void*)& pinfo.elt_typ));
_temp1235=( struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1237; _LL1241: _temp1234=(
void**) _temp1253; _temp1235=( struct Cyc_Absyn_Tqual*) _temp1250; goto _LL1237;
_LL1243: goto _LL1237; _LL1237:;}{ void* t= Cyc_Tcexp_tcExp( te, _temp1234, body);
if( te->le ==  0){ if( ! Cyc_Tcutil_is_const_exp( te, bound)){({ void* _temp1254[
0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant", sizeof(
unsigned char), 22u), _tag_arr( _temp1254, sizeof( void*), 0u));});} if( ! Cyc_Tcutil_is_const_exp(
te, body)){({ void* _temp1255[ 0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("body is not constant",
sizeof( unsigned char), 21u), _tag_arr( _temp1255, sizeof( void*), 0u));});}}
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1256=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1256[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1257; _temp1257.tag= Cyc_Absyn_ArrayType;
_temp1257.f1=( void*) t; _temp1257.f2= _temp1235 ==  0? Cyc_Absyn_empty_tqual():*
_temp1235; _temp1257.f3=( struct Cyc_Absyn_Exp*) bound; _temp1257;}); _temp1256;});}}}
struct _tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct _tuple1** tn, struct Cyc_Core_Opt** otyps, struct Cyc_List_List*
args, struct Cyc_Absyn_Structdecl** sd_opt){ struct Cyc_Absyn_Structdecl* sd;
if(* sd_opt !=  0){ sd=( struct Cyc_Absyn_Structdecl*) _check_null(* sd_opt);}
else{{ struct _handler_cons _temp1258; _push_handler(& _temp1258);{ int
_temp1260= 0; if( setjmp( _temp1258.handler)){ _temp1260= 1;} if( ! _temp1260){
sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();} else{ void*
_temp1259=( void*) _exn_thrown; void* _temp1262= _temp1259; _LL1264: if(
_temp1262 ==  Cyc_Dict_Absent){ goto _LL1265;} else{ goto _LL1266;} _LL1266:
goto _LL1267; _LL1265:({ struct Cyc_Std_String_pa_struct _temp1269; _temp1269.tag=
Cyc_Std_String_pa; _temp1269.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1268[ 1u]={& _temp1269}; Cyc_Tcutil_terr( loc, _tag_arr("unbound struct name %s",
sizeof( unsigned char), 23u), _tag_arr( _temp1268, sizeof( void*), 1u));}});
return topt !=  0?* topt:( void*) Cyc_Absyn_VoidType; _LL1267:( void) _throw(
_temp1262); _LL1263:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!=  0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1270= _new_region(); struct _RegionHandle* rgn=&
_temp1270; _push_region( rgn);{ struct _tuple4 _temp1271=({ struct _tuple4
_temp1295; _temp1295.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1295.f2= rgn;
_temp1295;}); struct Cyc_List_List* _temp1272=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1271, sd->tvs); struct Cyc_List_List* _temp1273=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1272); struct Cyc_Absyn_StructType_struct*
_temp1274=({ struct Cyc_Absyn_StructType_struct* _temp1292=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1292[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1293; _temp1293.tag= Cyc_Absyn_StructType;
_temp1293.f1=( struct _tuple1*)* tn; _temp1293.f2= _temp1273; _temp1293.f3=({
struct Cyc_Absyn_Structdecl** _temp1294=( struct Cyc_Absyn_Structdecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1294[ 0]= sd; _temp1294;});
_temp1293;}); _temp1292;}); if( topt !=  0){ Cyc_Tcutil_unify(( void*) _temp1274,*
topt);}* otyps=({ struct Cyc_Core_Opt* _temp1275=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1275->v=( void*) _temp1273;
_temp1275;}); if( sd->fields ==  0){({ void* _temp1276[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("can't build abstract struct", sizeof( unsigned char), 28u),
_tag_arr( _temp1276, sizeof( void*), 0u));});{ void* _temp1277=( void*)
_temp1274; _npop_handler( 0u); return _temp1277;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields !=  0; fields= fields->tl){ struct _tuple9 _temp1280; struct Cyc_Absyn_Exp*
_temp1281; struct Cyc_Absyn_Structfield* _temp1283; struct _tuple9* _temp1278=(
struct _tuple9*) fields->hd; _temp1280=* _temp1278; _LL1284: _temp1283=
_temp1280.f1; goto _LL1282; _LL1282: _temp1281= _temp1280.f2; goto _LL1279;
_LL1279: { void* _temp1285= Cyc_Tcutil_rsubstitute( rgn, _temp1272,( void*)
_temp1283->type); Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1285, _temp1281);
if( ! Cyc_Tcutil_coerce_arg( te, _temp1281, _temp1285)){({ struct Cyc_Std_String_pa_struct
_temp1290; _temp1290.tag= Cyc_Std_String_pa; _temp1290.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1281->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1289; _temp1289.tag= Cyc_Std_String_pa;
_temp1289.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp1285);{ struct
Cyc_Std_String_pa_struct _temp1288; _temp1288.tag= Cyc_Std_String_pa; _temp1288.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string(* tn);{ struct Cyc_Std_String_pa_struct
_temp1287; _temp1287.tag= Cyc_Std_String_pa; _temp1287.f1=( struct _tagged_arr)*
_temp1283->name;{ void* _temp1286[ 4u]={& _temp1287,& _temp1288,& _temp1289,&
_temp1290}; Cyc_Tcutil_terr( _temp1281->loc, _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1286, sizeof( void*), 4u));}}}}});
Cyc_Tcutil_explain_failure();}}}{ void* _temp1291=( void*) _temp1274;
_npop_handler( 0u); return _temp1291;}}}; _pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* ts, struct
Cyc_List_List* args){{ struct _RegionHandle _temp1296= _new_region(); struct
_RegionHandle* rgn=& _temp1296; _push_region( rgn);{ void* _temp1297= Cyc_Tcutil_compress(
ts); struct Cyc_List_List* _temp1303; _LL1299: if(( unsigned int) _temp1297 >  4u?*((
int*) _temp1297) ==  Cyc_Absyn_AnonStructType: 0){ _LL1304: _temp1303=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1297)->f1; goto _LL1300;} else{ goto
_LL1301;} _LL1301: goto _LL1302; _LL1300: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1303); for( 0; fields !=  0; fields= fields->tl){ struct
_tuple9 _temp1307; struct Cyc_Absyn_Exp* _temp1308; struct Cyc_Absyn_Structfield*
_temp1310; struct _tuple9* _temp1305=( struct _tuple9*) fields->hd; _temp1307=*
_temp1305; _LL1311: _temp1310= _temp1307.f1; goto _LL1309; _LL1309: _temp1308=
_temp1307.f2; goto _LL1306; _LL1306: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1310->type)), _temp1308); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1308,( void*) _temp1310->type)){({ struct Cyc_Std_String_pa_struct
_temp1315; _temp1315.tag= Cyc_Std_String_pa; _temp1315.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1308->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1314; _temp1314.tag= Cyc_Std_String_pa;
_temp1314.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*) _temp1310->type);{
struct Cyc_Std_String_pa_struct _temp1313; _temp1313.tag= Cyc_Std_String_pa;
_temp1313.f1=( struct _tagged_arr)* _temp1310->name;{ void* _temp1312[ 3u]={&
_temp1313,& _temp1314,& _temp1315}; Cyc_Tcutil_terr( _temp1308->loc, _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1312, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} goto _LL1298;} _LL1302:({ void* _temp1316[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("tcAnonStruct:  not an AnonStructType", sizeof( unsigned char), 37u),
_tag_arr( _temp1316, sizeof( void*), 0u));}); goto _LL1298; _LL1298:;};
_pop_region( rgn);} return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List*
es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield* tuf){ struct
_RegionHandle _temp1317= _new_region(); struct _RegionHandle* rgn=& _temp1317;
_push_region( rgn);{ struct _tuple4 _temp1318=({ struct _tuple4 _temp1364;
_temp1364.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1364.f2= rgn; _temp1364;});
struct Cyc_List_List* _temp1319=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1318, tud->tvs); struct Cyc_List_List* _temp1320=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1318, tuf->tvs); struct Cyc_List_List* _temp1321=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1319); struct Cyc_List_List* _temp1322=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1320); struct Cyc_List_List*
_temp1323=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1319, _temp1320);*
all_ref=({ struct Cyc_Core_Opt* _temp1324=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1324->v=( void*) _temp1321; _temp1324;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1325=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1325->v=( void*) _temp1322; _temp1325;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1359=( struct
Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1359[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1360; _temp1360.tag=
Cyc_Absyn_TunionFieldType; _temp1360.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1361; _temp1361.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1362=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1362[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1363; _temp1363.tag= Cyc_Absyn_KnownTunionfield; _temp1363.f1= tud;
_temp1363.f2= tuf; _temp1363;}); _temp1362;})); _temp1361.targs= _temp1321;
_temp1361;}); _temp1360;}); _temp1359;}); if( topt !=  0){ void* _temp1326= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_TunionInfo _temp1334; void* _temp1336; struct Cyc_List_List*
_temp1338; _LL1328: if(( unsigned int) _temp1326 >  4u?*(( int*) _temp1326) == 
Cyc_Absyn_TunionFieldType: 0){ goto _LL1329;} else{ goto _LL1330;} _LL1330: if((
unsigned int) _temp1326 >  4u?*(( int*) _temp1326) ==  Cyc_Absyn_TunionType: 0){
_LL1335: _temp1334=(( struct Cyc_Absyn_TunionType_struct*) _temp1326)->f1;
_LL1339: _temp1338= _temp1334.targs; goto _LL1337; _LL1337: _temp1336=( void*)
_temp1334.rgn; goto _LL1331;} else{ goto _LL1332;} _LL1332: goto _LL1333;
_LL1329: Cyc_Tcutil_unify(* topt, res); goto _LL1327; _LL1331:{ struct Cyc_List_List*
a= _temp1321; for( 0; a !=  0? _temp1338 !=  0: 0;( a= a->tl, _temp1338=
_temp1338->tl)){ Cyc_Tcutil_unify(( void*) a->hd,( void*) _temp1338->hd);}} if(
tuf->typs ==  0? es ==  0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1340=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1340->v=(
void*) res; _temp1340;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1341=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1341[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1342; _temp1342.tag=
Cyc_Absyn_TunionType; _temp1342.f1=({ struct Cyc_Absyn_TunionInfo _temp1343;
_temp1343.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1344=( struct Cyc_Absyn_KnownTunion_struct*) _cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1344[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1345; _temp1345.tag=
Cyc_Absyn_KnownTunion; _temp1345.f1=({ struct Cyc_Absyn_Tuniondecl** _temp1346=(
struct Cyc_Absyn_Tuniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp1346[ 0]= tud; _temp1346;}); _temp1345;}); _temp1344;})); _temp1343.targs=
_temp1321; _temp1343.rgn=( void*) _temp1336; _temp1343;}); _temp1342;});
_temp1341;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1347= res;
_npop_handler( 0u); return _temp1347;}} goto _LL1327; _LL1333: goto _LL1327;
_LL1327:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es !=  0? ts !=  0: 0;(
es= es->tl, ts= ts->tl)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) es->hd;
void* t= Cyc_Tcutil_rsubstitute( rgn, _temp1323,(*(( struct _tuple8*) ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){({ struct Cyc_Std_String_pa_struct _temp1351; _temp1351.tag= Cyc_Std_String_pa;
_temp1351.f1=( struct _tagged_arr)( e->topt ==  0? _tag_arr("?", sizeof(
unsigned char), 2u): Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v));{ struct Cyc_Std_String_pa_struct _temp1350;
_temp1350.tag= Cyc_Std_String_pa; _temp1350.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp1349; _temp1349.tag= Cyc_Std_String_pa;
_temp1349.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tuf->name);{ void*
_temp1348[ 3u]={& _temp1349,& _temp1350,& _temp1351}; Cyc_Tcutil_terr( e->loc,
_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u), _tag_arr( _temp1348, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} if( es !=  0){ void* _temp1354=({ struct Cyc_Std_String_pa_struct
_temp1353; _temp1353.tag= Cyc_Std_String_pa; _temp1353.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1352[ 1u]={& _temp1353}; Cyc_Tcexp_expr_err(
te,(( struct Cyc_Absyn_Exp*) es->hd)->loc, _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u), _tag_arr( _temp1352, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1354;} if( ts !=  0){ void* _temp1357=({ struct
Cyc_Std_String_pa_struct _temp1356; _temp1356.tag= Cyc_Std_String_pa; _temp1356.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1355[ 1u]={&
_temp1356}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments for tunion constructor %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1355, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1357;}{ void* _temp1358= res; _npop_handler( 0u);
return _temp1358;}}}}; _pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 
0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1375=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1375[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1376; _temp1376.tag= Cyc_Absyn_RgnHandleType; _temp1376.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1377=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1377->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1377;})); _temp1376;}); _temp1375;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1365= Cyc_Tcutil_compress( handle_type); void*
_temp1371; _LL1367: if(( unsigned int) _temp1365 >  4u?*(( int*) _temp1365) == 
Cyc_Absyn_RgnHandleType: 0){ _LL1372: _temp1371=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1365)->f1; goto _LL1368;} else{ goto _LL1369;} _LL1369: goto _LL1370;
_LL1368: rgn= _temp1371; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1366; _LL1370:({ struct Cyc_Std_String_pa_struct _temp1374; _temp1374.tag=
Cyc_Std_String_pa; _temp1374.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1373[ 1u]={& _temp1374}; Cyc_Tcutil_terr( ropt->loc,
_tag_arr("expecting region_t type but found %s", sizeof( unsigned char), 37u),
_tag_arr( _temp1373, sizeof( void*), 1u));}}); goto _LL1366; _LL1366:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t);{
void* _temp1378= t; struct Cyc_Absyn_TunionFieldInfo _temp1384; void* _temp1386;
struct Cyc_Absyn_Tunionfield* _temp1388; _LL1380: if(( unsigned int) _temp1378 > 
4u?*(( int*) _temp1378) ==  Cyc_Absyn_TunionFieldType: 0){ _LL1385: _temp1384=((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1378)->f1; _LL1387: _temp1386=(
void*) _temp1384.field_info; if(*(( int*) _temp1386) ==  Cyc_Absyn_KnownTunionfield){
_LL1389: _temp1388=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1386)->f2;
goto _LL1381;} else{ goto _LL1382;}} else{ goto _LL1382;} _LL1382: goto _LL1383;
_LL1381: if( _temp1388->tvs !=  0){({ void* _temp1390[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("malloc with existential types not yet implemented", sizeof(
unsigned char), 50u), _tag_arr( _temp1390, sizeof( void*), 0u));});} goto
_LL1379; _LL1383: goto _LL1379; _LL1379:;}{ void*(* _temp1391)( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt !=  0){ void*
_temp1392= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp1398;
struct Cyc_Absyn_Conref* _temp1400; _LL1394: if(( unsigned int) _temp1392 >  4u?*((
int*) _temp1392) ==  Cyc_Absyn_PointerType: 0){ _LL1399: _temp1398=(( struct Cyc_Absyn_PointerType_struct*)
_temp1392)->f1; _LL1401: _temp1400= _temp1398.nullable; goto _LL1395;} else{
goto _LL1396;} _LL1396: goto _LL1397; _LL1395: if((( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1400)){ _temp1391= Cyc_Absyn_star_typ;} goto
_LL1393; _LL1397: goto _LL1393; _LL1393:;} return _temp1391( t, rgn, Cyc_Absyn_empty_tqual());}}
static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1402=( void*) s->r; struct Cyc_Absyn_Exp* _temp1412;
struct Cyc_Absyn_Stmt* _temp1414; struct Cyc_Absyn_Stmt* _temp1416; struct Cyc_Absyn_Stmt*
_temp1418; struct Cyc_Absyn_Decl* _temp1420; _LL1404: if(( unsigned int)
_temp1402 >  1u?*(( int*) _temp1402) ==  Cyc_Absyn_Exp_s: 0){ _LL1413: _temp1412=((
struct Cyc_Absyn_Exp_s_struct*) _temp1402)->f1; goto _LL1405;} else{ goto
_LL1406;} _LL1406: if(( unsigned int) _temp1402 >  1u?*(( int*) _temp1402) == 
Cyc_Absyn_Seq_s: 0){ _LL1417: _temp1416=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1402)->f1; goto _LL1415; _LL1415: _temp1414=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1402)->f2; goto _LL1407;} else{ goto _LL1408;} _LL1408: if(( unsigned int)
_temp1402 >  1u?*(( int*) _temp1402) ==  Cyc_Absyn_Decl_s: 0){ _LL1421:
_temp1420=(( struct Cyc_Absyn_Decl_s_struct*) _temp1402)->f1; goto _LL1419;
_LL1419: _temp1418=(( struct Cyc_Absyn_Decl_s_struct*) _temp1402)->f2; goto
_LL1409;} else{ goto _LL1410;} _LL1410: goto _LL1411; _LL1405: return( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1412->topt))->v; _LL1407: s= _temp1414;
continue; _LL1409: s= _temp1418; continue; _LL1411: return({ void* _temp1422[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u), _tag_arr( _temp1422, sizeof( void*), 0u));});
_LL1403:;}} static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return({
void* _temp1423[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u), _tag_arr( _temp1423, sizeof( void*), 0u));});}
static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ return({ void* _temp1424[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u),
_tag_arr( _temp1424, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){
void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle !=  0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1435=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1435[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1436; _temp1436.tag= Cyc_Absyn_RgnHandleType;
_temp1436.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp1437=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1437->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1437;}));
_temp1436;}); _temp1435;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle)); void*
_temp1425= Cyc_Tcutil_compress( handle_type); void* _temp1431; _LL1427: if((
unsigned int) _temp1425 >  4u?*(( int*) _temp1425) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1432: _temp1431=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1425)->f1; goto _LL1428;} else{ goto _LL1429;} _LL1429: goto _LL1430;
_LL1428: rgn= _temp1431; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1426; _LL1430:({ struct Cyc_Std_String_pa_struct _temp1434; _temp1434.tag=
Cyc_Std_String_pa; _temp1434.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1433[ 1u]={& _temp1434}; Cyc_Tcutil_terr( rgn_handle->loc,
_tag_arr("expecting region_t type but found %s", sizeof( unsigned char), 37u),
_tag_arr( _temp1433, sizeof( void*), 1u));}}); goto _LL1426; _LL1426:;}{ void*
_temp1438=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1450; struct Cyc_Absyn_Exp*
_temp1452; struct Cyc_Absyn_Vardecl* _temp1454; struct Cyc_List_List* _temp1456;
struct Cyc_Core_Opt* _temp1458; struct Cyc_List_List* _temp1460; void* _temp1462;
struct _tagged_arr _temp1464; _LL1440: if(*(( int*) _temp1438) ==  Cyc_Absyn_Comprehension_e){
_LL1455: _temp1454=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1438)->f1;
goto _LL1453; _LL1453: _temp1452=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1438)->f2; goto _LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1438)->f3; goto _LL1441;} else{ goto _LL1442;} _LL1442: if(*(( int*)
_temp1438) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1459: _temp1458=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1438)->f1; goto _LL1457; _LL1457: _temp1456=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1438)->f2; goto _LL1443;} else{ goto _LL1444;} _LL1444: if(*(( int*)
_temp1438) ==  Cyc_Absyn_Array_e){ _LL1461: _temp1460=(( struct Cyc_Absyn_Array_e_struct*)
_temp1438)->f1; goto _LL1445;} else{ goto _LL1446;} _LL1446: if(*(( int*)
_temp1438) ==  Cyc_Absyn_Const_e){ _LL1463: _temp1462=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1438)->f1; if(( unsigned int) _temp1462 >  1u?*(( int*) _temp1462) ==  Cyc_Absyn_String_c:
0){ _LL1465: _temp1464=(( struct Cyc_Absyn_String_c_struct*) _temp1462)->f1;
goto _LL1447;} else{ goto _LL1448;}} else{ goto _LL1448;} _LL1448: goto _LL1449;
_LL1441: { void* _temp1466= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1467= Cyc_Tcutil_compress( _temp1466); struct Cyc_Absyn_Tqual _temp1473;
void* _temp1475; _LL1469: if(( unsigned int) _temp1467 >  4u?*(( int*) _temp1467)
==  Cyc_Absyn_ArrayType: 0){ _LL1476: _temp1475=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1467)->f1; goto _LL1474; _LL1474: _temp1473=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1467)->f2; goto _LL1470;} else{ goto _LL1471;} _LL1471: goto _LL1472;
_LL1470: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1452)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1481=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1481[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1482; _temp1482.tag= Cyc_Absyn_Upper_b; _temp1482.f1=
_temp1452; _temp1482;}); _temp1481;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1478=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1478[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1479; _temp1479.tag= Cyc_Absyn_PointerType;
_temp1479.f1=({ struct Cyc_Absyn_PtrInfo _temp1480; _temp1480.elt_typ=( void*)
_temp1475; _temp1480.rgn_typ=( void*) rgn; _temp1480.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1480.tq= _temp1473; _temp1480.bounds= Cyc_Absyn_new_conref(
b); _temp1480;}); _temp1479;}); _temp1478;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*
topt, res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1477=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1477->v=( void*) res_typ; _temp1477;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_typ=* topt;}} return res_typ;} _LL1472: return({ void*
_temp1483[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u), _tag_arr( _temp1483, sizeof( void*), 0u));});
_LL1468:;} _LL1443:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1484=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1484[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1485; _temp1485.tag= Cyc_Absyn_Array_e;
_temp1485.f1= _temp1456; _temp1485;}); _temp1484;}))); _temp1460= _temp1456;
goto _LL1445; _LL1445: { void** elt_typ_opt= 0; if( topt !=  0){ void* _temp1486=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp1492; struct Cyc_Absyn_Tqual
_temp1494; void* _temp1496; void** _temp1498; _LL1488: if(( unsigned int)
_temp1486 >  4u?*(( int*) _temp1486) ==  Cyc_Absyn_PointerType: 0){ _LL1493:
_temp1492=(( struct Cyc_Absyn_PointerType_struct*) _temp1486)->f1; _LL1497:
_temp1496=( void*) _temp1492.elt_typ; _temp1498=( void**)&((( struct Cyc_Absyn_PointerType_struct*)
_temp1486)->f1).elt_typ; goto _LL1495; _LL1495: _temp1494= _temp1492.tq; goto
_LL1489;} else{ goto _LL1490;} _LL1490: goto _LL1491; _LL1489: elt_typ_opt=(
void**) _temp1498; goto _LL1487; _LL1491: goto _LL1487; _LL1487:;}{ void*
_temp1499= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1500= Cyc_Tcutil_compress( _temp1499); struct Cyc_Absyn_Exp* _temp1506;
struct Cyc_Absyn_Tqual _temp1508; void* _temp1510; _LL1502: if(( unsigned int)
_temp1500 >  4u?*(( int*) _temp1500) ==  Cyc_Absyn_ArrayType: 0){ _LL1511:
_temp1510=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1500)->f1; goto
_LL1509; _LL1509: _temp1508=(( struct Cyc_Absyn_ArrayType_struct*) _temp1500)->f2;
goto _LL1507; _LL1507: _temp1506=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1500)->f3; goto _LL1503;} else{ goto _LL1504;} _LL1504: goto _LL1505;
_LL1503: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1512=(
struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1512[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1513; _temp1513.tag=
Cyc_Absyn_PointerType; _temp1513.f1=({ struct Cyc_Absyn_PtrInfo _temp1514;
_temp1514.elt_typ=( void*) _temp1510; _temp1514.rgn_typ=( void*) rgn; _temp1514.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1514.tq=
_temp1508; _temp1514.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1515=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1515[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1516; _temp1516.tag= Cyc_Absyn_Upper_b;
_temp1516.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1506); _temp1516;});
_temp1515;})); _temp1514;}); _temp1513;}); _temp1512;}); if( topt !=  0){ if( !
Cyc_Tcutil_unify(* topt, res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*
topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1517=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1517->v=( void*) res_typ;
_temp1517;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); res_typ=* topt;}} goto
_LL1501; _LL1505: return({ void* _temp1518[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u), _tag_arr( _temp1518, sizeof( void*), 0u));});
_LL1501:;} return res_typ;}} _LL1447: { void* _temp1519= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b); void* _temp1520= Cyc_Tcexp_tcExp(
te,( void**)& _temp1519, e1); return Cyc_Absyn_atb_typ( _temp1520, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1521=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1521[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1522; _temp1522.tag= Cyc_Absyn_Upper_b; _temp1522.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1522;}); _temp1521;}));} _LL1449: { void** topt2=
0; if( topt !=  0){ void* _temp1523= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp1531; struct Cyc_Absyn_Conref* _temp1533; void* _temp1535; void** _temp1537;
_LL1525: if(( unsigned int) _temp1523 >  4u?*(( int*) _temp1523) ==  Cyc_Absyn_PointerType:
0){ _LL1532: _temp1531=(( struct Cyc_Absyn_PointerType_struct*) _temp1523)->f1;
_LL1536: _temp1535=( void*) _temp1531.elt_typ; _temp1537=( void**)&((( struct
Cyc_Absyn_PointerType_struct*) _temp1523)->f1).elt_typ; goto _LL1534; _LL1534:
_temp1533= _temp1531.nullable; goto _LL1526;} else{ goto _LL1527;} _LL1527: if((
unsigned int) _temp1523 >  4u?*(( int*) _temp1523) ==  Cyc_Absyn_TunionType: 0){
goto _LL1528;} else{ goto _LL1529;} _LL1529: goto _LL1530; _LL1526: topt2=( void**)
_temp1537; goto _LL1524; _LL1528: topt2=({ void** _temp1538=( void**) _cycalloc(
sizeof( void*)); _temp1538[ 0]=* topt; _temp1538;}); goto _LL1524; _LL1530: goto
_LL1524; _LL1524:;}{ void* telt= Cyc_Tcexp_tcExp( te, topt2, e1); void* res_typ=(
void*)({ struct Cyc_Absyn_PointerType_struct* _temp1540=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1540[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1541; _temp1541.tag= Cyc_Absyn_PointerType;
_temp1541.f1=({ struct Cyc_Absyn_PtrInfo _temp1542; _temp1542.elt_typ=( void*)
telt; _temp1542.rgn_typ=( void*) rgn; _temp1542.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1542.tq= Cyc_Absyn_empty_tqual(); _temp1542.bounds=
Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1543=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1543[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1544; _temp1544.tag= Cyc_Absyn_Upper_b;
_temp1544.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1544;}); _temp1543;})); _temp1542;});
_temp1541;}); _temp1540;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(* topt,
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1539=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1539->v=( void*) res_typ; _temp1539;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_typ=* topt;}} return res_typ;}} _LL1439:;}} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote( te, topt, e)); void* _temp1545= t; struct Cyc_Absyn_Exp*
_temp1551; struct Cyc_Absyn_Tqual _temp1553; void* _temp1555; _LL1547: if((
unsigned int) _temp1545 >  4u?*(( int*) _temp1545) ==  Cyc_Absyn_ArrayType: 0){
_LL1556: _temp1555=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1545)->f1;
goto _LL1554; _LL1554: _temp1553=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1545)->f2; goto _LL1552; _LL1552: _temp1551=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1545)->f3; goto _LL1548;} else{ goto _LL1549;} _LL1549: goto _LL1550;
_LL1548: { void* _temp1559; struct _tuple6 _temp1557= Cyc_Tcutil_addressof_props(
te, e); _LL1560: _temp1559= _temp1557.f2; goto _LL1558; _LL1558: { void*
_temp1561= _temp1551 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1562=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1562[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1563; _temp1563.tag= Cyc_Absyn_Upper_b;
_temp1563.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1551); _temp1563;});
_temp1562;}); t= Cyc_Absyn_atb_typ( _temp1555, _temp1559, _temp1553, _temp1561);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1550: return t; _LL1546:;} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1564=( void*) e->r; void* _temp1574; _LL1566: if(*(( int*)
_temp1564) ==  Cyc_Absyn_Array_e){ goto _LL1567;} else{ goto _LL1568;} _LL1568:
if(*(( int*) _temp1564) ==  Cyc_Absyn_Comprehension_e){ goto _LL1569;} else{
goto _LL1570;} _LL1570: if(*(( int*) _temp1564) ==  Cyc_Absyn_Const_e){ _LL1575:
_temp1574=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1564)->f1; if((
unsigned int) _temp1574 >  1u?*(( int*) _temp1574) ==  Cyc_Absyn_String_c: 0){
goto _LL1571;} else{ goto _LL1572;}} else{ goto _LL1572;} _LL1572: goto _LL1573;
_LL1567: return t; _LL1569: return t; _LL1571: return t; _LL1573: t= Cyc_Tcutil_compress(
t);{ void* _temp1576= t; struct Cyc_Absyn_Exp* _temp1582; struct Cyc_Absyn_Tqual
_temp1584; void* _temp1586; _LL1578: if(( unsigned int) _temp1576 >  4u?*(( int*)
_temp1576) ==  Cyc_Absyn_ArrayType: 0){ _LL1587: _temp1586=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1576)->f1; goto _LL1585; _LL1585: _temp1584=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1576)->f2; goto _LL1583; _LL1583: _temp1582=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1576)->f3; goto _LL1579;} else{ goto _LL1580;} _LL1580: goto _LL1581;
_LL1579: { void* _temp1590; struct _tuple6 _temp1588= Cyc_Tcutil_addressof_props(
te, e); _LL1591: _temp1590= _temp1588.f2; goto _LL1589; _LL1589: { void* b=
_temp1582 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1592=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1592[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1593; _temp1593.tag= Cyc_Absyn_Upper_b;
_temp1593.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1582); _temp1593;});
_temp1592;}); t= Cyc_Absyn_atb_typ( _temp1586, _temp1590, _temp1584, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1581: return t; _LL1577:;} _LL1565:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1594=( void*) e->r; struct Cyc_Absyn_Exp* _temp1600; _LL1596: if(*(( int*)
_temp1594) ==  Cyc_Absyn_NoInstantiate_e){ _LL1601: _temp1600=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1594)->f1; goto _LL1597;} else{ goto _LL1598;} _LL1598: goto _LL1599;
_LL1597: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1600);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1600->topt))->v=( void*) Cyc_Absyn_pointer_expand(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1600->topt))->v)); e->topt= _temp1600->topt;
goto _LL1595; _LL1599: Cyc_Tcexp_tcExpNoInst( te, topt, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1602=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct Cyc_Absyn_PtrInfo _temp1608; struct Cyc_Absyn_Conref* _temp1610; struct
Cyc_Absyn_Tqual _temp1612; struct Cyc_Absyn_Conref* _temp1614; void* _temp1616;
void* _temp1618; _LL1604: if(( unsigned int) _temp1602 >  4u?*(( int*) _temp1602)
==  Cyc_Absyn_PointerType: 0){ _LL1609: _temp1608=(( struct Cyc_Absyn_PointerType_struct*)
_temp1602)->f1; _LL1619: _temp1618=( void*) _temp1608.elt_typ; goto _LL1617;
_LL1617: _temp1616=( void*) _temp1608.rgn_typ; goto _LL1615; _LL1615: _temp1614=
_temp1608.nullable; goto _LL1613; _LL1613: _temp1612= _temp1608.tq; goto _LL1611;
_LL1611: _temp1610= _temp1608.bounds; goto _LL1605;} else{ goto _LL1606;}
_LL1606: goto _LL1607; _LL1605:{ void* _temp1620= Cyc_Tcutil_compress( _temp1618);
struct Cyc_Absyn_FnInfo _temp1626; struct Cyc_List_List* _temp1628; struct Cyc_List_List*
_temp1630; struct Cyc_Absyn_VarargInfo* _temp1632; int _temp1634; struct Cyc_List_List*
_temp1636; void* _temp1638; struct Cyc_Core_Opt* _temp1640; struct Cyc_List_List*
_temp1642; _LL1622: if(( unsigned int) _temp1620 >  4u?*(( int*) _temp1620) == 
Cyc_Absyn_FnType: 0){ _LL1627: _temp1626=(( struct Cyc_Absyn_FnType_struct*)
_temp1620)->f1; _LL1643: _temp1642= _temp1626.tvars; goto _LL1641; _LL1641:
_temp1640= _temp1626.effect; goto _LL1639; _LL1639: _temp1638=( void*) _temp1626.ret_typ;
goto _LL1637; _LL1637: _temp1636= _temp1626.args; goto _LL1635; _LL1635:
_temp1634= _temp1626.c_varargs; goto _LL1633; _LL1633: _temp1632= _temp1626.cyc_varargs;
goto _LL1631; _LL1631: _temp1630= _temp1626.rgn_po; goto _LL1629; _LL1629:
_temp1628= _temp1626.attributes; goto _LL1623;} else{ goto _LL1624;} _LL1624:
goto _LL1625; _LL1623: if( _temp1642 !=  0){ struct _RegionHandle _temp1644=
_new_region(); struct _RegionHandle* rgn=& _temp1644; _push_region( rgn);{
struct _tuple4 _temp1645=({ struct _tuple4 _temp1655; _temp1655.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1655.f2= rgn; _temp1655;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1645, _temp1642); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1652=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1652[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1653; _temp1653.tag= Cyc_Absyn_FnType; _temp1653.f1=({ struct Cyc_Absyn_FnInfo
_temp1654; _temp1654.tvars= 0; _temp1654.effect= _temp1640; _temp1654.ret_typ=(
void*) _temp1638; _temp1654.args= _temp1636; _temp1654.c_varargs= _temp1634;
_temp1654.cyc_varargs= _temp1632; _temp1654.rgn_po= _temp1630; _temp1654.attributes=
_temp1628; _temp1654;}); _temp1653;}); _temp1652;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1649=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1649[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1650; _temp1650.tag= Cyc_Absyn_PointerType;
_temp1650.f1=({ struct Cyc_Absyn_PtrInfo _temp1651; _temp1651.elt_typ=( void*)
ftyp; _temp1651.rgn_typ=( void*) _temp1616; _temp1651.nullable= _temp1614;
_temp1651.tq= _temp1612; _temp1651.bounds= _temp1610; _temp1651;}); _temp1650;});
_temp1649;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1646=( struct Cyc_Absyn_Instantiate_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1646[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1647; _temp1647.tag= Cyc_Absyn_Instantiate_e;
_temp1647.f1= inner; _temp1647.f2= ts; _temp1647;}); _temp1646;}))); e->topt=({
struct Cyc_Core_Opt* _temp1648=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1648->v=( void*) new_typ; _temp1648;});}; _pop_region( rgn);}
goto _LL1621; _LL1625: goto _LL1621; _LL1621:;} goto _LL1603; _LL1607: goto
_LL1603; _LL1603:;} goto _LL1595; _LL1595:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1656=( void*) e->r; struct Cyc_Absyn_Exp* _temp1734; struct
_tuple1* _temp1736; struct Cyc_List_List* _temp1738; struct Cyc_Absyn_Exp*
_temp1740; struct Cyc_List_List* _temp1742; struct Cyc_Core_Opt* _temp1744; void*
_temp1746; void* _temp1748; struct _tuple1* _temp1750; struct Cyc_List_List*
_temp1752; void* _temp1754; void* _temp1756; struct Cyc_Absyn_Exp* _temp1758;
struct Cyc_Absyn_Exp* _temp1760; struct Cyc_Core_Opt* _temp1762; struct Cyc_Absyn_Exp*
_temp1764; struct Cyc_Absyn_Exp* _temp1766; struct Cyc_Absyn_Exp* _temp1768;
struct Cyc_Absyn_Exp* _temp1770; struct Cyc_Absyn_Exp* _temp1772; struct Cyc_Absyn_Exp*
_temp1774; struct Cyc_Absyn_VarargCallInfo* _temp1776; struct Cyc_Absyn_VarargCallInfo**
_temp1778; struct Cyc_List_List* _temp1779; struct Cyc_Absyn_Exp* _temp1781;
struct Cyc_Absyn_Exp* _temp1783; struct Cyc_List_List* _temp1785; struct Cyc_Absyn_Exp*
_temp1787; struct Cyc_Absyn_Exp* _temp1789; void* _temp1791; struct Cyc_Absyn_Exp*
_temp1793; struct Cyc_Absyn_Exp* _temp1795; struct Cyc_Absyn_Exp* _temp1797;
struct Cyc_Absyn_Exp* _temp1799; void* _temp1801; void* _temp1803; void*
_temp1805; struct Cyc_Absyn_Exp* _temp1807; struct _tagged_arr* _temp1809;
struct Cyc_Absyn_Exp* _temp1811; struct _tagged_arr* _temp1813; struct Cyc_Absyn_Exp*
_temp1815; struct Cyc_Absyn_Exp* _temp1817; struct Cyc_Absyn_Exp* _temp1819;
struct Cyc_List_List* _temp1821; struct Cyc_List_List* _temp1823; struct _tuple2*
_temp1825; struct Cyc_List_List* _temp1827; struct Cyc_Absyn_Stmt* _temp1829;
struct Cyc_Absyn_Fndecl* _temp1831; struct Cyc_Absyn_Exp* _temp1833; struct Cyc_Absyn_Exp*
_temp1835; struct Cyc_Absyn_Exp* _temp1837; struct Cyc_Absyn_Vardecl* _temp1839;
struct Cyc_Absyn_Structdecl* _temp1841; struct Cyc_Absyn_Structdecl** _temp1843;
struct Cyc_List_List* _temp1844; struct Cyc_Core_Opt* _temp1846; struct Cyc_Core_Opt**
_temp1848; struct _tuple1* _temp1849; struct _tuple1** _temp1851; struct Cyc_List_List*
_temp1852; void* _temp1854; struct Cyc_Absyn_Tunionfield* _temp1856; struct Cyc_Absyn_Tuniondecl*
_temp1858; struct Cyc_List_List* _temp1860; struct Cyc_Core_Opt* _temp1862;
struct Cyc_Core_Opt** _temp1864; struct Cyc_Core_Opt* _temp1865; struct Cyc_Core_Opt**
_temp1867; struct Cyc_Absyn_Enumfield* _temp1868; struct Cyc_Absyn_Enumdecl*
_temp1870; struct _tuple1* _temp1872; struct _tuple1** _temp1874; struct Cyc_Absyn_Enumfield*
_temp1875; void* _temp1877; struct _tuple1* _temp1879; struct _tuple1**
_temp1881; void* _temp1882; struct Cyc_Absyn_Exp* _temp1884; _LL1658: if(*(( int*)
_temp1656) ==  Cyc_Absyn_NoInstantiate_e){ _LL1735: _temp1734=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1656)->f1; goto _LL1659;} else{ goto _LL1660;} _LL1660: if(*(( int*)
_temp1656) ==  Cyc_Absyn_UnknownId_e){ _LL1737: _temp1736=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1656)->f1; goto _LL1661;} else{ goto _LL1662;} _LL1662: if(*(( int*)
_temp1656) ==  Cyc_Absyn_UnknownCall_e){ _LL1741: _temp1740=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1656)->f1; goto _LL1739; _LL1739: _temp1738=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1656)->f2; goto _LL1663;} else{ goto _LL1664;} _LL1664: if(*(( int*)
_temp1656) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1745: _temp1744=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1656)->f1; goto _LL1743; _LL1743: _temp1742=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1656)->f2; goto _LL1665;} else{ goto _LL1666;} _LL1666: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Const_e){ _LL1747: _temp1746=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1656)->f1; goto _LL1667;} else{ goto _LL1668;} _LL1668: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Var_e){ _LL1751: _temp1750=(( struct Cyc_Absyn_Var_e_struct*)
_temp1656)->f1; goto _LL1749; _LL1749: _temp1748=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1656)->f2; goto _LL1669;} else{ goto _LL1670;} _LL1670: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Primop_e){ _LL1755: _temp1754=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1656)->f1; goto _LL1753; _LL1753: _temp1752=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1656)->f2; goto _LL1671;} else{ goto _LL1672;} _LL1672: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Increment_e){ _LL1759: _temp1758=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1656)->f1; goto _LL1757; _LL1757: _temp1756=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1656)->f2; goto _LL1673;} else{ goto _LL1674;} _LL1674: if(*(( int*)
_temp1656) ==  Cyc_Absyn_AssignOp_e){ _LL1765: _temp1764=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1656)->f1; goto _LL1763; _LL1763: _temp1762=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1656)->f2; goto _LL1761; _LL1761: _temp1760=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1656)->f3; goto _LL1675;} else{ goto _LL1676;} _LL1676: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Conditional_e){ _LL1771: _temp1770=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1656)->f1; goto _LL1769; _LL1769: _temp1768=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1656)->f2; goto _LL1767; _LL1767: _temp1766=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1656)->f3; goto _LL1677;} else{ goto _LL1678;} _LL1678: if(*(( int*)
_temp1656) ==  Cyc_Absyn_SeqExp_e){ _LL1775: _temp1774=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1656)->f1; goto _LL1773; _LL1773: _temp1772=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1656)->f2; goto _LL1679;} else{ goto _LL1680;} _LL1680: if(*(( int*)
_temp1656) ==  Cyc_Absyn_FnCall_e){ _LL1782: _temp1781=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1656)->f1; goto _LL1780; _LL1780: _temp1779=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1656)->f2; goto _LL1777; _LL1777: _temp1776=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1656)->f3; _temp1778=( struct Cyc_Absyn_VarargCallInfo**)&(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1656)->f3; goto _LL1681;} else{ goto _LL1682;} _LL1682: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Throw_e){ _LL1784: _temp1783=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1656)->f1; goto _LL1683;} else{ goto _LL1684;} _LL1684: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Instantiate_e){ _LL1788: _temp1787=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1656)->f1; goto _LL1786; _LL1786: _temp1785=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1656)->f2; goto _LL1685;} else{ goto _LL1686;} _LL1686: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Cast_e){ _LL1792: _temp1791=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1656)->f1; goto _LL1790; _LL1790: _temp1789=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1656)->f2; goto _LL1687;} else{ goto _LL1688;} _LL1688: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Address_e){ _LL1794: _temp1793=(( struct Cyc_Absyn_Address_e_struct*)
_temp1656)->f1; goto _LL1689;} else{ goto _LL1690;} _LL1690: if(*(( int*)
_temp1656) ==  Cyc_Absyn_New_e){ _LL1798: _temp1797=(( struct Cyc_Absyn_New_e_struct*)
_temp1656)->f1; goto _LL1796; _LL1796: _temp1795=(( struct Cyc_Absyn_New_e_struct*)
_temp1656)->f2; goto _LL1691;} else{ goto _LL1692;} _LL1692: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Sizeofexp_e){ _LL1800: _temp1799=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1656)->f1; goto _LL1693;} else{ goto _LL1694;} _LL1694: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1802: _temp1801=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1656)->f1; goto _LL1695;} else{ goto _LL1696;} _LL1696: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Offsetof_e){ _LL1806: _temp1805=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1656)->f1; goto _LL1804; _LL1804: _temp1803=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1656)->f2; goto _LL1697;} else{ goto _LL1698;} _LL1698: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Gentyp_e){ goto _LL1699;} else{ goto _LL1700;} _LL1700:
if(*(( int*) _temp1656) ==  Cyc_Absyn_Deref_e){ _LL1808: _temp1807=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1656)->f1; goto _LL1701;} else{ goto _LL1702;} _LL1702: if(*(( int*)
_temp1656) ==  Cyc_Absyn_StructMember_e){ _LL1812: _temp1811=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1656)->f1; goto _LL1810; _LL1810: _temp1809=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1656)->f2; goto _LL1703;} else{ goto _LL1704;} _LL1704: if(*(( int*)
_temp1656) ==  Cyc_Absyn_StructArrow_e){ _LL1816: _temp1815=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1656)->f1; goto _LL1814; _LL1814: _temp1813=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1656)->f2; goto _LL1705;} else{ goto _LL1706;} _LL1706: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Subscript_e){ _LL1820: _temp1819=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1656)->f1; goto _LL1818; _LL1818: _temp1817=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1656)->f2; goto _LL1707;} else{ goto _LL1708;} _LL1708: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Tuple_e){ _LL1822: _temp1821=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1656)->f1; goto _LL1709;} else{ goto _LL1710;} _LL1710: if(*(( int*)
_temp1656) ==  Cyc_Absyn_CompoundLit_e){ _LL1826: _temp1825=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1656)->f1; goto _LL1824; _LL1824: _temp1823=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1656)->f2; goto _LL1711;} else{ goto _LL1712;} _LL1712: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Array_e){ _LL1828: _temp1827=(( struct Cyc_Absyn_Array_e_struct*)
_temp1656)->f1; goto _LL1713;} else{ goto _LL1714;} _LL1714: if(*(( int*)
_temp1656) ==  Cyc_Absyn_StmtExp_e){ _LL1830: _temp1829=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1656)->f1; goto _LL1715;} else{ goto _LL1716;} _LL1716: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Codegen_e){ _LL1832: _temp1831=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1656)->f1; goto _LL1717;} else{ goto _LL1718;} _LL1718: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Fill_e){ _LL1834: _temp1833=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1656)->f1; goto _LL1719;} else{ goto _LL1720;} _LL1720: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Comprehension_e){ _LL1840: _temp1839=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1656)->f1; goto _LL1838; _LL1838: _temp1837=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1656)->f2; goto _LL1836; _LL1836: _temp1835=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1656)->f3; goto _LL1721;} else{ goto _LL1722;} _LL1722: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Struct_e){ _LL1850: _temp1849=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1656)->f1; _temp1851=( struct _tuple1**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1656)->f1; goto _LL1847; _LL1847: _temp1846=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1656)->f2; _temp1848=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1656)->f2; goto _LL1845; _LL1845: _temp1844=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1656)->f3; goto _LL1842; _LL1842: _temp1841=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1656)->f4; _temp1843=( struct Cyc_Absyn_Structdecl**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1656)->f4; goto _LL1723;} else{ goto _LL1724;} _LL1724: if(*(( int*)
_temp1656) ==  Cyc_Absyn_AnonStruct_e){ _LL1855: _temp1854=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1656)->f1; goto _LL1853; _LL1853: _temp1852=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1656)->f2; goto _LL1725;} else{ goto _LL1726;} _LL1726: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Tunion_e){ _LL1866: _temp1865=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1656)->f1; _temp1867=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1656)->f1; goto _LL1863; _LL1863: _temp1862=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1656)->f2; _temp1864=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1656)->f2; goto _LL1861; _LL1861: _temp1860=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1656)->f3; goto _LL1859; _LL1859: _temp1858=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1656)->f4; goto _LL1857; _LL1857: _temp1856=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1656)->f5; goto _LL1727;} else{ goto _LL1728;} _LL1728: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Enum_e){ _LL1873: _temp1872=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1656)->f1; _temp1874=( struct _tuple1**)&(( struct Cyc_Absyn_Enum_e_struct*)
_temp1656)->f1; goto _LL1871; _LL1871: _temp1870=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1656)->f2; goto _LL1869; _LL1869: _temp1868=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1656)->f3; goto _LL1729;} else{ goto _LL1730;} _LL1730: if(*(( int*)
_temp1656) ==  Cyc_Absyn_AnonEnum_e){ _LL1880: _temp1879=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1656)->f1; _temp1881=( struct _tuple1**)&(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1656)->f1; goto _LL1878; _LL1878: _temp1877=( void*)(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1656)->f2; goto _LL1876; _LL1876: _temp1875=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1656)->f3; goto _LL1731;} else{ goto _LL1732;} _LL1732: if(*(( int*)
_temp1656) ==  Cyc_Absyn_Malloc_e){ _LL1885: _temp1884=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1656)->f1; goto _LL1883; _LL1883: _temp1882=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1656)->f2; goto _LL1733;} else{ goto _LL1657;} _LL1659: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1734); return; _LL1661: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1736); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1663: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1740, _temp1738); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1665: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1742); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1667: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1746, e);
goto _LL1657; _LL1669: t= Cyc_Tcexp_tcVar( te, loc, _temp1750, _temp1748); goto
_LL1657; _LL1671: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1754, _temp1752);
goto _LL1657; _LL1673: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1758,
_temp1756); goto _LL1657; _LL1675: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1764, _temp1762, _temp1760); goto _LL1657; _LL1677: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1770, _temp1768, _temp1766); goto _LL1657; _LL1679: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1774, _temp1772); goto _LL1657; _LL1681: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1781, _temp1779, _temp1778); goto _LL1657; _LL1683: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1783); goto _LL1657; _LL1685: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1787, _temp1785); goto _LL1657; _LL1687: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1791, _temp1789); goto _LL1657; _LL1689: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1793); goto _LL1657; _LL1691: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1797, e, _temp1795); goto _LL1657; _LL1693: { void* _temp1886= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1799); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1886); goto
_LL1657;} _LL1695: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1801); goto
_LL1657; _LL1697: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1805, _temp1803);
goto _LL1657; _LL1699:({ void* _temp1887[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("gen() not in top-level initializer",
sizeof( unsigned char), 35u), _tag_arr( _temp1887, sizeof( void*), 0u));});
return; _LL1701: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1807); goto _LL1657;
_LL1703: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1811, _temp1809);
goto _LL1657; _LL1705: t= Cyc_Tcexp_tcStructArrow( te, loc, topt, _temp1815,
_temp1813); goto _LL1657; _LL1707: t= Cyc_Tcexp_tcSubscript( te, loc, topt,
_temp1819, _temp1817); goto _LL1657; _LL1709: t= Cyc_Tcexp_tcTuple( te, loc,
topt, _temp1821); goto _LL1657; _LL1711: t= Cyc_Tcexp_tcCompoundLit( te, loc,
topt, _temp1825, _temp1823); goto _LL1657; _LL1713: { void** elt_topt= 0; if(
topt !=  0){ void* _temp1888= Cyc_Tcutil_compress(* topt); void* _temp1894; void**
_temp1896; _LL1890: if(( unsigned int) _temp1888 >  4u?*(( int*) _temp1888) == 
Cyc_Absyn_ArrayType: 0){ _LL1895: _temp1894=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1888)->f1; _temp1896=( void**)&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1888)->f1); goto _LL1891;} else{ goto _LL1892;} _LL1892: goto _LL1893;
_LL1891: elt_topt=( void**) _temp1896; goto _LL1889; _LL1893: goto _LL1889;
_LL1889:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp1827); goto _LL1657;}
_LL1715: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp1829); goto _LL1657;
_LL1717: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp1831); goto _LL1657;
_LL1719: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1833); goto _LL1657; _LL1721:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1839, _temp1837, _temp1835);
goto _LL1657; _LL1723: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp1851,
_temp1848, _temp1844, _temp1843); goto _LL1657; _LL1725: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1854, _temp1852); goto _LL1657; _LL1727: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1867, _temp1864, _temp1860, _temp1858, _temp1856); goto
_LL1657; _LL1729:* _temp1874=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1868))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1897=(
struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1897[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1898; _temp1898.tag= Cyc_Absyn_EnumType;
_temp1898.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1870))->name;
_temp1898.f2= _temp1870; _temp1898;}); _temp1897;}); goto _LL1657; _LL1731:*
_temp1881=(( struct Cyc_Absyn_Enumfield*) _check_null( _temp1875))->name; t=
_temp1877; goto _LL1657; _LL1733: t= Cyc_Tcexp_tcMalloc( te, loc, topt,
_temp1884, _temp1882); goto _LL1657; _LL1657:;} e->topt=({ struct Cyc_Core_Opt*
_temp1899=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1899->v=( void*) t; _temp1899;});}
