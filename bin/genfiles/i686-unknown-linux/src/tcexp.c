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
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_UnknownId_p= 10;
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
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
Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
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
struct Cyc_CfFlowInfo_Place; struct Cyc_CfFlowInfo_InitState; static const int
Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct
Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct
Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ; static const
int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc= 1; static
const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL= 1;
static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_MustPointTo=
0; struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; struct Cyc_CfFlowInfo_InitState{ void* esc; void* level; } ; static
const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag;
struct Cyc_CfFlowInfo_InitState f1; } ; static const int Cyc_CfFlowInfo_DictPI=
1; struct Cyc_CfFlowInfo_DictPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; struct Cyc_Set_Set** all_changed; } ; static
void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap){({ void* _temp0[ 0u]={}; Cyc_Tcutil_terr(
loc,( struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, msg, ap),
_tag_arr( _temp0, sizeof( void*), 0u));}); return Cyc_Absyn_wildtyp(({ struct
Cyc_Core_Opt* _temp1=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1;}));} static struct
Cyc_Core_Opt Cyc_Tcexp_rk={( void*)(( void*) 3u)}; static struct Cyc_Core_Opt
Cyc_Tcexp_ak={( void*)(( void*) 0u)}; static struct Cyc_Core_Opt Cyc_Tcexp_bk={(
void*)(( void*) 2u)}; static struct Cyc_Core_Opt Cyc_Tcexp_mk={( void*)(( void*)
1u)}; static void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, struct _tuple1* q){ struct _handler_cons _temp2;
_push_handler(& _temp2);{ int _temp4= 0; if( setjmp( _temp2.handler)){ _temp4= 1;}
if( ! _temp4){{ void* _temp5= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void*
_temp15; struct Cyc_Absyn_Enumfield* _temp17; struct Cyc_Absyn_Enumdecl* _temp19;
struct Cyc_Absyn_Tunionfield* _temp21; struct Cyc_Absyn_Tuniondecl* _temp23;
_LL7: if(*(( int*) _temp5) ==  Cyc_Tcenv_VarRes){ _LL16: _temp15=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp5)->f1; goto _LL8;} else{ goto _LL9;} _LL9:
if(*(( int*) _temp5) ==  Cyc_Tcenv_EnumRes){ _LL20: _temp19=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp5)->f1; goto _LL18; _LL18: _temp17=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp5)->f2; goto _LL10;} else{ goto _LL11;} _LL11: if(*(( int*) _temp5) ==  Cyc_Tcenv_TunionRes){
_LL24: _temp23=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f1; goto _LL22;
_LL22: _temp21=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f2; goto _LL12;}
else{ goto _LL13;} _LL13: if(*(( int*) _temp5) ==  Cyc_Tcenv_StructRes){ goto
_LL14;} else{ goto _LL6;} _LL8:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp25=( struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp25[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp26; _temp26.tag= Cyc_Absyn_Var_e;
_temp26.f1= q; _temp26.f2=( void*) _temp15; _temp26;}); _temp25;}))); goto _LL6;
_LL10:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp27=(
struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp27[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp28; _temp28.tag= Cyc_Absyn_Enum_e;
_temp28.f1= q; _temp28.f2=( struct Cyc_Absyn_Enumdecl*) _temp19; _temp28.f3=(
struct Cyc_Absyn_Enumfield*) _temp17; _temp28;}); _temp27;}))); goto _LL6; _LL12:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp29=(
struct Cyc_Absyn_Tunion_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp29[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp30; _temp30.tag= Cyc_Absyn_Tunion_e;
_temp30.f1= 0; _temp30.f2= 0; _temp30.f3= 0; _temp30.f4= _temp23; _temp30.f5=
_temp21; _temp30;}); _temp29;}))); goto _LL6; _LL14:({ struct Cyc_Std_String_pa_struct
_temp32; _temp32.tag= Cyc_Std_String_pa; _temp32.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp31[ 1u]={& _temp32}; Cyc_Tcutil_terr( e->loc, _tag_arr("bad occurrence of struct name %s",
sizeof( unsigned char), 33u), _tag_arr( _temp31, sizeof( void*), 1u));}});( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp33=( struct Cyc_Absyn_Var_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp33[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp34; _temp34.tag= Cyc_Absyn_Var_e; _temp34.f1= q; _temp34.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp34;}); _temp33;}))); goto _LL6; _LL6:;};
_pop_handler();} else{ void* _temp3=( void*) _exn_thrown; void* _temp36= _temp3;
_LL38: if( _temp36 ==  Cyc_Dict_Absent){ goto _LL39;} else{ goto _LL40;} _LL40:
goto _LL41; _LL39:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp42=( struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp42[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp43; _temp43.tag= Cyc_Absyn_Var_e;
_temp43.f1= q; _temp43.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp43;});
_temp42;}))); goto _LL37; _LL41:( void) _throw( _temp36); _LL37:;}}} struct
_tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple7* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({ struct
_tuple7* _temp44=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7)); _temp44->f1=
0; _temp44->f2= e; _temp44;});} static void Cyc_Tcexp_resolve_unknown_fn( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1, struct
Cyc_List_List* es){ void* _temp45=( void*) e1->r; struct _tuple1* _temp51; _LL47:
if(*(( int*) _temp45) ==  Cyc_Absyn_UnknownId_e){ _LL52: _temp51=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp45)->f1; goto _LL48;} else{ goto _LL49;} _LL49: goto _LL50; _LL48: { struct
_handler_cons _temp53; _push_handler(& _temp53);{ int _temp55= 0; if( setjmp(
_temp53.handler)){ _temp55= 1;} if( ! _temp55){{ void* _temp56= Cyc_Tcenv_lookup_ordinary(
te, e1->loc, _temp51); void* _temp66; struct Cyc_Absyn_Tunionfield* _temp68;
struct Cyc_Absyn_Tuniondecl* _temp70; struct Cyc_Absyn_Structdecl* _temp72;
_LL58: if(*(( int*) _temp56) ==  Cyc_Tcenv_VarRes){ _LL67: _temp66=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp56)->f1; goto _LL59;} else{ goto _LL60;}
_LL60: if(*(( int*) _temp56) ==  Cyc_Tcenv_TunionRes){ _LL71: _temp70=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp56)->f1; goto _LL69; _LL69: _temp68=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp56)->f2; goto _LL61;} else{ goto _LL62;} _LL62:
if(*(( int*) _temp56) ==  Cyc_Tcenv_StructRes){ _LL73: _temp72=(( struct Cyc_Tcenv_StructRes_struct*)
_temp56)->f1; goto _LL63;} else{ goto _LL64;} _LL64: if(*(( int*) _temp56) == 
Cyc_Tcenv_EnumRes){ goto _LL65;} else{ goto _LL57;} _LL59:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp74=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp74[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp75; _temp75.tag= Cyc_Absyn_FnCall_e; _temp75.f1= e1; _temp75.f2= es;
_temp75.f3= 0; _temp75;}); _temp74;}))); _npop_handler( 0u); return; _LL61: if(
_temp68->typs ==  0){({ struct Cyc_Std_String_pa_struct _temp77; _temp77.tag=
Cyc_Std_String_pa; _temp77.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp68->name);{ void* _temp76[ 1u]={& _temp77}; Cyc_Tcutil_terr( e->loc,
_tag_arr("%s is a constant, not a function", sizeof( unsigned char), 33u),
_tag_arr( _temp76, sizeof( void*), 1u));}});}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_e_struct* _temp78=( struct Cyc_Absyn_Tunion_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp78[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp79; _temp79.tag= Cyc_Absyn_Tunion_e; _temp79.f1= 0; _temp79.f2= 0; _temp79.f3=
es; _temp79.f4= _temp70; _temp79.f5= _temp68; _temp79;}); _temp78;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL63: { struct Cyc_List_List*
_temp80=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp72->name !=  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp81=( struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp81[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp82; _temp82.tag= Cyc_Absyn_Struct_e;
_temp82.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp72->name))->v;
_temp82.f2= 0; _temp82.f3= _temp80; _temp82.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp82;}); _temp81;})));} else{({ void* _temp83[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("missing struct name", sizeof( unsigned char), 20u), _tag_arr(
_temp83, sizeof( void*), 0u));});( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp84=( struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp84[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp85; _temp85.tag= Cyc_Absyn_Struct_e;
_temp85.f1= _temp51; _temp85.f2= 0; _temp85.f3= _temp80; _temp85.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp85;}); _temp84;})));} _npop_handler( 0u); return;} _LL65:({ struct
Cyc_Std_String_pa_struct _temp87; _temp87.tag= Cyc_Std_String_pa; _temp87.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp51);{ void* _temp86[ 1u]={&
_temp87}; Cyc_Tcutil_terr( e->loc, _tag_arr("%s is an enum constructor, not a function",
sizeof( unsigned char), 42u), _tag_arr( _temp86, sizeof( void*), 1u));}});
_npop_handler( 0u); return; _LL57:;}; _pop_handler();} else{ void* _temp54=(
void*) _exn_thrown; void* _temp89= _temp54; _LL91: if( _temp89 ==  Cyc_Dict_Absent){
goto _LL92;} else{ goto _LL93;} _LL93: goto _LL94; _LL92:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp95=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp95[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp96; _temp96.tag= Cyc_Absyn_FnCall_e; _temp96.f1= e1; _temp96.f2= es;
_temp96.f3= 0; _temp96;}); _temp95;}))); return; _LL94:( void) _throw( _temp89);
_LL90:;}}} _LL50:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp97=( struct Cyc_Absyn_FnCall_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp97[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp98; _temp98.tag= Cyc_Absyn_FnCall_e;
_temp98.f1= e1; _temp98.f2= es; _temp98.f3= 0; _temp98;}); _temp97;}))); return;
_LL46:;} static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
Cyc_List_List* des){ if( topt ==  0){( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Array_e_struct* _temp99=( struct Cyc_Absyn_Array_e_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Array_e_struct)); _temp99[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp100; _temp100.tag= Cyc_Absyn_Array_e; _temp100.f1= des; _temp100;});
_temp99;}))); return;}{ void* t=*(( void**) _check_null( topt)); void* _temp101=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tqual _temp111; void* _temp113; struct
Cyc_Absyn_Structdecl** _temp115; struct Cyc_List_List* _temp117; struct _tuple1*
_temp119; _LL103: if(( unsigned int) _temp101 >  4u?*(( int*) _temp101) ==  Cyc_Absyn_ArrayType:
0){ _LL114: _temp113=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp101)->f1;
goto _LL112; _LL112: _temp111=(( struct Cyc_Absyn_ArrayType_struct*) _temp101)->f2;
goto _LL104;} else{ goto _LL105;} _LL105: if(( unsigned int) _temp101 >  4u?*((
int*) _temp101) ==  Cyc_Absyn_StructType: 0){ _LL120: _temp119=(( struct Cyc_Absyn_StructType_struct*)
_temp101)->f1; goto _LL118; _LL118: _temp117=(( struct Cyc_Absyn_StructType_struct*)
_temp101)->f2; goto _LL116; _LL116: _temp115=(( struct Cyc_Absyn_StructType_struct*)
_temp101)->f3; goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int)
_temp101 >  4u?*(( int*) _temp101) ==  Cyc_Absyn_AnonStructType: 0){ goto _LL108;}
else{ goto _LL109;} _LL109: goto _LL110; _LL104:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp121=( struct Cyc_Absyn_Array_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp121[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp122; _temp122.tag= Cyc_Absyn_Array_e; _temp122.f1= des; _temp122;});
_temp121;}))); goto _LL102; _LL106: if( _temp115 ==  0? 1: _temp119 ==  0){({
void* _temp123[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("Compiler bug: struct type not properly set",
sizeof( unsigned char), 43u), _tag_arr( _temp123, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct* _temp124=(
struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp124[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp125; _temp125.tag= Cyc_Absyn_Struct_e;
_temp125.f1=( struct _tuple1*) _check_null( _temp119); _temp125.f2= 0; _temp125.f3=
des; _temp125.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp115)); _temp125;}); _temp124;}))); goto _LL102; _LL108:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp126=( struct
Cyc_Absyn_AnonStruct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp126[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp127; _temp127.tag= Cyc_Absyn_AnonStruct_e;
_temp127.f1=( void*) t; _temp127.f2= des; _temp127;}); _temp126;}))); goto
_LL102; _LL110:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp128=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp128[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp129; _temp129.tag= Cyc_Absyn_Array_e;
_temp129.f1= des; _temp129;}); _temp128;}))); goto _LL102; _LL102:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es !=  0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){({ struct Cyc_Std_String_pa_struct _temp132; _temp132.tag= Cyc_Std_String_pa;
_temp132.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct _temp131; _temp131.tag=
Cyc_Std_String_pa; _temp131.f1=( struct _tagged_arr) msg_part;{ void* _temp130[
2u]={& _temp131,& _temp132}; Cyc_Tcutil_terr( e->loc, _tag_arr("test of %s has type %s instead of integral or * type",
sizeof( unsigned char), 53u), _tag_arr( _temp130, sizeof( void*), 2u));}}});}}
static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp133= c;
void* _temp149; void* _temp151; void* _temp153; int _temp155; void* _temp157;
struct _tagged_arr _temp159; _LL135: if(( unsigned int) _temp133 >  1u?*(( int*)
_temp133) ==  Cyc_Absyn_Char_c: 0){ _LL150: _temp149=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp133)->f1; goto _LL136;} else{ goto _LL137;} _LL137: if(( unsigned int)
_temp133 >  1u?*(( int*) _temp133) ==  Cyc_Absyn_Short_c: 0){ _LL152: _temp151=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp133)->f1; goto _LL138;} else{
goto _LL139;} _LL139: if(( unsigned int) _temp133 >  1u?*(( int*) _temp133) == 
Cyc_Absyn_LongLong_c: 0){ _LL154: _temp153=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp133)->f1; goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int)
_temp133 >  1u?*(( int*) _temp133) ==  Cyc_Absyn_Float_c: 0){ goto _LL142;}
else{ goto _LL143;} _LL143: if(( unsigned int) _temp133 >  1u?*(( int*) _temp133)
==  Cyc_Absyn_Int_c: 0){ _LL158: _temp157=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp133)->f1; goto _LL156; _LL156: _temp155=(( struct Cyc_Absyn_Int_c_struct*)
_temp133)->f2; goto _LL144;} else{ goto _LL145;} _LL145: if(( unsigned int)
_temp133 >  1u?*(( int*) _temp133) ==  Cyc_Absyn_String_c: 0){ _LL160: _temp159=((
struct Cyc_Absyn_String_c_struct*) _temp133)->f1; goto _LL146;} else{ goto
_LL147;} _LL147: if( _temp133 == ( void*) Cyc_Absyn_Null_c){ goto _LL148;} else{
goto _LL134;} _LL136: t= _temp149 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t; goto _LL134; _LL138: t= _temp151 == ( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL134; _LL140: t= _temp153 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto
_LL134; _LL142: t= Cyc_Absyn_float_t; goto _LL134; _LL144: if( topt ==  0){ t=
_temp157 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp161= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); void*
_temp171; void* _temp173; void* _temp175; void* _temp177; void* _temp179; _LL163:
if(( unsigned int) _temp161 >  4u?*(( int*) _temp161) ==  Cyc_Absyn_IntType: 0){
_LL174: _temp173=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp161)->f1;
goto _LL172; _LL172: _temp171=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp161)->f2; if( _temp171 == ( void*) Cyc_Absyn_B1){ goto _LL164;} else{ goto
_LL165;}} else{ goto _LL165;} _LL165: if(( unsigned int) _temp161 >  4u?*(( int*)
_temp161) ==  Cyc_Absyn_IntType: 0){ _LL178: _temp177=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp161)->f1; goto _LL176; _LL176: _temp175=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp161)->f2; if( _temp175 == ( void*) Cyc_Absyn_B2){ goto _LL166;} else{ goto
_LL167;}} else{ goto _LL167;} _LL167: if(( unsigned int) _temp161 >  4u?*(( int*)
_temp161) ==  Cyc_Absyn_IntType: 0){ _LL180: _temp179=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp161)->f1; goto _LL168;} else{ goto _LL169;} _LL169: goto _LL170; _LL164: t=
_temp173 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp181=(
struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp181[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp182; _temp182.tag= Cyc_Absyn_Const_e;
_temp182.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp183=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp183[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp184; _temp184.tag= Cyc_Absyn_Char_c;
_temp184.f1=( void*) _temp173; _temp184.f2=( unsigned char) _temp155; _temp184;});
_temp183;})); _temp182;}); _temp181;}))); goto _LL162; _LL166: t= _temp177 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp185=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp185[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp186; _temp186.tag= Cyc_Absyn_Const_e; _temp186.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp187=( struct Cyc_Absyn_Short_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp187[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp188; _temp188.tag= Cyc_Absyn_Short_c; _temp188.f1=( void*) _temp177;
_temp188.f2=( short) _temp155; _temp188;}); _temp187;})); _temp186;}); _temp185;})));
goto _LL162; _LL168: t= _temp179 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL162; _LL170: t= _temp157 == ( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL162; _LL162:;} goto _LL134; _LL146: {
int len=( int) _get_arr_size( _temp159, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp189=({ struct Cyc_Absyn_Const_e_struct* _temp226=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp226[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp227; _temp227.tag= Cyc_Absyn_Const_e; _temp227.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp228=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp228[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp229; _temp229.tag= Cyc_Absyn_Int_c; _temp229.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp229.f2= len; _temp229;}); _temp228;})); _temp227;}); _temp226;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp189, loc); elen->topt=({
struct Cyc_Core_Opt* _temp190=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp190->v=( void*) Cyc_Absyn_uint_t; _temp190;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp191=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp191[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp192; _temp192.tag= Cyc_Absyn_Upper_b; _temp192.f1= elen; _temp192;});
_temp191;})); if( topt !=  0){ void* _temp193= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp201; struct Cyc_Absyn_Tqual
_temp203; void* _temp205; struct Cyc_Absyn_PtrInfo _temp207; struct Cyc_Absyn_Conref*
_temp209; struct Cyc_Absyn_Tqual _temp211; struct Cyc_Absyn_Conref* _temp213;
void* _temp215; void* _temp217; _LL195: if(( unsigned int) _temp193 >  4u?*((
int*) _temp193) ==  Cyc_Absyn_ArrayType: 0){ _LL206: _temp205=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp193)->f1; goto _LL204; _LL204: _temp203=((
struct Cyc_Absyn_ArrayType_struct*) _temp193)->f2; goto _LL202; _LL202: _temp201=((
struct Cyc_Absyn_ArrayType_struct*) _temp193)->f3; goto _LL196;} else{ goto
_LL197;} _LL197: if(( unsigned int) _temp193 >  4u?*(( int*) _temp193) ==  Cyc_Absyn_PointerType:
0){ _LL208: _temp207=(( struct Cyc_Absyn_PointerType_struct*) _temp193)->f1;
_LL218: _temp217=( void*) _temp207.elt_typ; goto _LL216; _LL216: _temp215=( void*)
_temp207.rgn_typ; goto _LL214; _LL214: _temp213= _temp207.nullable; goto _LL212;
_LL212: _temp211= _temp207.tq; goto _LL210; _LL210: _temp209= _temp207.bounds;
goto _LL198;} else{ goto _LL199;} _LL199: goto _LL200; _LL196: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp219=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp219[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp220; _temp220.tag= Cyc_Absyn_ArrayType; _temp220.f1=(
void*) Cyc_Absyn_uchar_t; _temp220.f2= _temp203; _temp220.f3=( struct Cyc_Absyn_Exp*)
elen; _temp220;}); _temp219;}); _LL198: if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp221=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp221->v=( void*) t; _temp221;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp222=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp222->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp222;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp223=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp223[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp224; _temp224.tag= Cyc_Absyn_Upper_b; _temp224.f1= elen; _temp224;});
_temp223;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp225=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp225->v=( void*) t; _temp225;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL194; _LL200:
goto _LL194; _LL194:;} return t;} _LL148: { struct Cyc_List_List* _temp230= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp231=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp231[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp232; _temp232.tag= Cyc_Absyn_PointerType;
_temp232.f1=({ struct Cyc_Absyn_PtrInfo _temp233; _temp233.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp235=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp235->v=( void*)
_temp230; _temp235;})); _temp233.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp234=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp234->v=( void*) _temp230;
_temp234;})); _temp233.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp233.tq= Cyc_Absyn_empty_tqual(); _temp233.bounds= Cyc_Absyn_empty_conref();
_temp233;}); _temp232;}); _temp231;}); goto _LL134;} _LL134:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp236= b; struct Cyc_Absyn_Vardecl*
_temp250; struct Cyc_Absyn_Fndecl* _temp252; struct Cyc_Absyn_Vardecl* _temp254;
struct Cyc_Absyn_Vardecl* _temp256; struct Cyc_Absyn_Vardecl* _temp258; _LL238:
if( _temp236 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL239;} else{ goto
_LL240;} _LL240: if(( unsigned int) _temp236 >  1u?*(( int*) _temp236) ==  Cyc_Absyn_Global_b:
0){ _LL251: _temp250=(( struct Cyc_Absyn_Global_b_struct*) _temp236)->f1; goto
_LL241;} else{ goto _LL242;} _LL242: if(( unsigned int) _temp236 >  1u?*(( int*)
_temp236) ==  Cyc_Absyn_Funname_b: 0){ _LL253: _temp252=(( struct Cyc_Absyn_Funname_b_struct*)
_temp236)->f1; goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int)
_temp236 >  1u?*(( int*) _temp236) ==  Cyc_Absyn_Pat_b: 0){ _LL255: _temp254=((
struct Cyc_Absyn_Pat_b_struct*) _temp236)->f1; goto _LL245;} else{ goto _LL246;}
_LL246: if(( unsigned int) _temp236 >  1u?*(( int*) _temp236) ==  Cyc_Absyn_Local_b:
0){ _LL257: _temp256=(( struct Cyc_Absyn_Local_b_struct*) _temp236)->f1; goto
_LL247;} else{ goto _LL248;} _LL248: if(( unsigned int) _temp236 >  1u?*(( int*)
_temp236) ==  Cyc_Absyn_Param_b: 0){ _LL259: _temp258=(( struct Cyc_Absyn_Param_b_struct*)
_temp236)->f1; goto _LL249;} else{ goto _LL237;} _LL239: return({ struct Cyc_Std_String_pa_struct
_temp261; _temp261.tag= Cyc_Std_String_pa; _temp261.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp260[ 1u]={& _temp261}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("undeclared identifier: %s",
sizeof( unsigned char), 26u), _tag_arr( _temp260, sizeof( void*), 1u));}});
_LL241: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp250->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp250->type; _LL243:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp252->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp252);
_LL245: _temp256= _temp254; goto _LL247; _LL247: _temp258= _temp256; goto _LL249;
_LL249:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp258->type; _LL237:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_Core_Opt* opt_args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp262=( void*) fmt->r; void* _temp270; struct _tagged_arr
_temp272; struct Cyc_Absyn_Exp* _temp274; struct Cyc_Absyn_Exp _temp276; void*
_temp277; void* _temp279; struct _tagged_arr _temp281; _LL264: if(*(( int*)
_temp262) ==  Cyc_Absyn_Const_e){ _LL271: _temp270=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp262)->f1; if(( unsigned int) _temp270 >  1u?*(( int*) _temp270) ==  Cyc_Absyn_String_c:
0){ _LL273: _temp272=(( struct Cyc_Absyn_String_c_struct*) _temp270)->f1; goto
_LL265;} else{ goto _LL266;}} else{ goto _LL266;} _LL266: if(*(( int*) _temp262)
==  Cyc_Absyn_Cast_e){ _LL275: _temp274=(( struct Cyc_Absyn_Cast_e_struct*)
_temp262)->f2; _temp276=* _temp274; _LL278: _temp277=( void*) _temp276.r; if(*((
int*) _temp277) ==  Cyc_Absyn_Const_e){ _LL280: _temp279=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp277)->f1; if(( unsigned int) _temp279 >  1u?*(( int*) _temp279) ==  Cyc_Absyn_String_c:
0){ _LL282: _temp281=(( struct Cyc_Absyn_String_c_struct*) _temp279)->f1; goto
_LL267;} else{ goto _LL268;}} else{ goto _LL268;}} else{ goto _LL268;} _LL268:
goto _LL269; _LL265: _temp281= _temp272; goto _LL267; _LL267: desc_types=
type_getter( te,( struct _tagged_arr) _temp281, fmt->loc); goto _LL263; _LL269:
return; _LL263:;} if( opt_args !=  0){ struct Cyc_List_List* _temp283=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( opt_args))->v; for( 0;
desc_types !=  0? _temp283 !=  0: 0;( desc_types=(( struct Cyc_List_List*)
_check_null( desc_types))->tl, _temp283=(( struct Cyc_List_List*) _check_null(
_temp283))->tl)){ void* t=( void*)(( struct Cyc_List_List*) _check_null(
desc_types))->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp283))->hd; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct
Cyc_Std_String_pa_struct _temp286; _temp286.tag= Cyc_Std_String_pa; _temp286.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct _temp285; _temp285.tag=
Cyc_Std_String_pa; _temp285.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp284[ 2u]={& _temp285,& _temp286}; Cyc_Tcutil_terr( e->loc, _tag_arr("descriptor has type \n%s\n but argument has type \n%s",
sizeof( unsigned char), 51u), _tag_arr( _temp284, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}
if( desc_types !=  0){({ void* _temp287[ 0u]={}; Cyc_Tcutil_terr( fmt->loc,
_tag_arr("too few arguments", sizeof( unsigned char), 18u), _tag_arr( _temp287,
sizeof( void*), 0u));});} if( _temp283 !=  0){({ void* _temp288[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp283))->hd)->loc,
_tag_arr("too many arguments", sizeof( unsigned char), 19u), _tag_arr( _temp288,
sizeof( void*), 0u));});}}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp289= p; _LL291: if( _temp289 == ( void*) Cyc_Absyn_Plus){ goto _LL292;}
else{ goto _LL293;} _LL293: if( _temp289 == ( void*) Cyc_Absyn_Minus){ goto
_LL294;} else{ goto _LL295;} _LL295: if( _temp289 == ( void*) Cyc_Absyn_Not){
goto _LL296;} else{ goto _LL297;} _LL297: if( _temp289 == ( void*) Cyc_Absyn_Bitnot){
goto _LL298;} else{ goto _LL299;} _LL299: if( _temp289 == ( void*) Cyc_Absyn_Size){
goto _LL300;} else{ goto _LL301;} _LL301: goto _LL302; _LL292: goto _LL294;
_LL294: if( ! Cyc_Tcutil_is_numeric( e)){({ struct Cyc_Std_String_pa_struct
_temp304; _temp304.tag= Cyc_Std_String_pa; _temp304.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp303[ 1u]={& _temp304}; Cyc_Tcutil_terr( loc, _tag_arr("expecting arithmetic type but found %s",
sizeof( unsigned char), 39u), _tag_arr( _temp303, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL296: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){({ struct Cyc_Std_String_pa_struct
_temp306; _temp306.tag= Cyc_Std_String_pa; _temp306.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp305[ 1u]={& _temp306}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral or * type but found %s",
sizeof( unsigned char), 42u), _tag_arr( _temp305, sizeof( void*), 1u));}});}
return Cyc_Absyn_sint_t; _LL298: if( ! Cyc_Tcutil_is_integral( e)){({ struct Cyc_Std_String_pa_struct
_temp308; _temp308.tag= Cyc_Std_String_pa; _temp308.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp307[ 1u]={& _temp308}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp307, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL300:{ void*
_temp309= t; struct Cyc_Absyn_PtrInfo _temp317; _LL311: if(( unsigned int)
_temp309 >  4u?*(( int*) _temp309) ==  Cyc_Absyn_ArrayType: 0){ goto _LL312;}
else{ goto _LL313;} _LL313: if(( unsigned int) _temp309 >  4u?*(( int*) _temp309)
==  Cyc_Absyn_PointerType: 0){ _LL318: _temp317=(( struct Cyc_Absyn_PointerType_struct*)
_temp309)->f1; goto _LL314;} else{ goto _LL315;} _LL315: goto _LL316; _LL312:
goto _LL310; _LL314: goto _LL310; _LL316:({ struct Cyc_Std_String_pa_struct
_temp320; _temp320.tag= Cyc_Std_String_pa; _temp320.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp319[ 1u]={& _temp320}; Cyc_Tcutil_terr( loc, _tag_arr("size requires pointer or array type, not %s",
sizeof( unsigned char), 44u), _tag_arr( _temp319, sizeof( void*), 1u));}}); goto
_LL310; _LL310:;} return Cyc_Absyn_uint_t; _LL302: return({ void* _temp321[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("Non-unary primop", sizeof( unsigned char), 17u),
_tag_arr( _temp321, sizeof( void*), 0u));}); _LL290:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){({ struct Cyc_Std_String_pa_struct
_temp323; _temp323.tag= Cyc_Std_String_pa; _temp323.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp322[ 1u]={&
_temp323}; Cyc_Tcutil_terr( e1->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp322, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp324=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp324->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp324;}));} if( ! checker( e2)){({ struct Cyc_Std_String_pa_struct
_temp326; _temp326.tag= Cyc_Std_String_pa; _temp326.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp325[ 1u]={&
_temp326}; Cyc_Tcutil_terr( e2->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp325, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp327=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp327->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp327;}));}{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); void* _temp328= t1; struct
Cyc_Absyn_PtrInfo _temp334; struct Cyc_Absyn_Conref* _temp336; struct Cyc_Absyn_Tqual
_temp338; struct Cyc_Absyn_Conref* _temp340; void* _temp342; void* _temp344;
_LL330: if(( unsigned int) _temp328 >  4u?*(( int*) _temp328) ==  Cyc_Absyn_PointerType:
0){ _LL335: _temp334=(( struct Cyc_Absyn_PointerType_struct*) _temp328)->f1;
_LL345: _temp344=( void*) _temp334.elt_typ; goto _LL343; _LL343: _temp342=( void*)
_temp334.rgn_typ; goto _LL341; _LL341: _temp340= _temp334.nullable; goto _LL339;
_LL339: _temp338= _temp334.tq; goto _LL337; _LL337: _temp336= _temp334.bounds;
goto _LL331;} else{ goto _LL332;} _LL332: goto _LL333; _LL331: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){({ struct Cyc_Std_String_pa_struct _temp347; _temp347.tag= Cyc_Std_String_pa;
_temp347.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp346[
1u]={& _temp347}; Cyc_Tcutil_terr( e2->loc, _tag_arr("expecting int but found %s",
sizeof( unsigned char), 27u), _tag_arr( _temp346, sizeof( void*), 1u));}});}
_temp336= Cyc_Absyn_compress_conref( _temp336);{ void* _temp348=( void*)
_temp336->v; void* _temp356; void* _temp358; struct Cyc_Absyn_Exp* _temp360;
_LL350: if(( unsigned int) _temp348 >  1u?*(( int*) _temp348) ==  Cyc_Absyn_Eq_constr:
0){ _LL357: _temp356=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp348)->f1;
if( _temp356 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL351;} else{ goto _LL352;}}
else{ goto _LL352;} _LL352: if(( unsigned int) _temp348 >  1u?*(( int*) _temp348)
==  Cyc_Absyn_Eq_constr: 0){ _LL359: _temp358=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp348)->f1; if(( unsigned int) _temp358 >  1u?*(( int*) _temp358) ==  Cyc_Absyn_Upper_b:
0){ _LL361: _temp360=(( struct Cyc_Absyn_Upper_b_struct*) _temp358)->f1; goto
_LL353;} else{ goto _LL354;}} else{ goto _LL354;} _LL354: goto _LL355; _LL351:
return t1; _LL353: { struct Cyc_Absyn_PointerType_struct* _temp362=({ struct Cyc_Absyn_PointerType_struct*
_temp363=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp363[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp364; _temp364.tag= Cyc_Absyn_PointerType;
_temp364.f1=({ struct Cyc_Absyn_PtrInfo _temp365; _temp365.elt_typ=( void*)
_temp344; _temp365.rgn_typ=( void*) _temp342; _temp365.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp365.tq= _temp338; _temp365.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp365;}); _temp364;}); _temp363;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp362); return( void*) _temp362;} _LL355:( void*)( _temp336->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp366=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp366[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp367; _temp367.tag= Cyc_Absyn_Eq_constr; _temp367.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp367;}); _temp366;}))); return t1;
_LL349:;} _LL333: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL329:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){({ struct Cyc_Std_String_pa_struct
_temp370; _temp370.tag= Cyc_Std_String_pa; _temp370.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp369; _temp369.tag= Cyc_Std_String_pa; _temp369.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp368[ 2u]={&
_temp369,& _temp370}; Cyc_Tcutil_terr( e1->loc, _tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof( unsigned char), 59u), _tag_arr( _temp368, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){({
struct Cyc_Std_String_pa_struct _temp373; _temp373.tag= Cyc_Std_String_pa;
_temp373.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp372; _temp372.tag= Cyc_Std_String_pa; _temp372.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp371[ 2u]={& _temp372,& _temp373}; Cyc_Tcutil_terr( e2->loc,
_tag_arr("expecting either %s or int but found %s", sizeof( unsigned char), 40u),
_tag_arr( _temp371, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp374=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp374->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp374;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp376=({ struct _tuple0 _temp375;
_temp375.f1= Cyc_Tcutil_compress( t1); _temp375.f2= Cyc_Tcutil_compress( t2);
_temp375;}); void* _temp382; struct Cyc_Absyn_PtrInfo _temp384; void* _temp386;
void* _temp388; struct Cyc_Absyn_PtrInfo _temp390; void* _temp392; _LL378:
_LL389: _temp388= _temp376.f1; if(( unsigned int) _temp388 >  4u?*(( int*)
_temp388) ==  Cyc_Absyn_PointerType: 0){ _LL391: _temp390=(( struct Cyc_Absyn_PointerType_struct*)
_temp388)->f1; _LL393: _temp392=( void*) _temp390.elt_typ; goto _LL383;} else{
goto _LL380;} _LL383: _temp382= _temp376.f2; if(( unsigned int) _temp382 >  4u?*((
int*) _temp382) ==  Cyc_Absyn_PointerType: 0){ _LL385: _temp384=(( struct Cyc_Absyn_PointerType_struct*)
_temp382)->f1; _LL387: _temp386=( void*) _temp384.elt_typ; goto _LL379;} else{
goto _LL380;} _LL380: goto _LL381; _LL379: if( Cyc_Tcutil_unify( _temp392,
_temp386)){ return Cyc_Absyn_sint_t;} goto _LL377; _LL381: goto _LL377; _LL377:;}({
struct Cyc_Std_String_pa_struct _temp396; _temp396.tag= Cyc_Std_String_pa;
_temp396.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp395; _temp395.tag= Cyc_Std_String_pa; _temp395.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp394[ 2u]={& _temp395,& _temp396}; Cyc_Tcutil_terr( loc,
_tag_arr("comparison not allowed between %s and %s", sizeof( unsigned char), 41u),
_tag_arr( _temp394, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp397=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp397->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp397;}));}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp398= p; _LL400: if( _temp398 == ( void*)
Cyc_Absyn_Plus){ goto _LL401;} else{ goto _LL402;} _LL402: if( _temp398 == (
void*) Cyc_Absyn_Minus){ goto _LL403;} else{ goto _LL404;} _LL404: if( _temp398
== ( void*) Cyc_Absyn_Times){ goto _LL405;} else{ goto _LL406;} _LL406: if(
_temp398 == ( void*) Cyc_Absyn_Div){ goto _LL407;} else{ goto _LL408;} _LL408:
if( _temp398 == ( void*) Cyc_Absyn_Mod){ goto _LL409;} else{ goto _LL410;}
_LL410: if( _temp398 == ( void*) Cyc_Absyn_Bitand){ goto _LL411;} else{ goto
_LL412;} _LL412: if( _temp398 == ( void*) Cyc_Absyn_Bitor){ goto _LL413;} else{
goto _LL414;} _LL414: if( _temp398 == ( void*) Cyc_Absyn_Bitxor){ goto _LL415;}
else{ goto _LL416;} _LL416: if( _temp398 == ( void*) Cyc_Absyn_Bitlshift){ goto
_LL417;} else{ goto _LL418;} _LL418: if( _temp398 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL419;} else{ goto _LL420;} _LL420: if( _temp398 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL421;} else{ goto _LL422;} _LL422: if( _temp398 == ( void*) Cyc_Absyn_Eq){
goto _LL423;} else{ goto _LL424;} _LL424: if( _temp398 == ( void*) Cyc_Absyn_Neq){
goto _LL425;} else{ goto _LL426;} _LL426: if( _temp398 == ( void*) Cyc_Absyn_Gt){
goto _LL427;} else{ goto _LL428;} _LL428: if( _temp398 == ( void*) Cyc_Absyn_Lt){
goto _LL429;} else{ goto _LL430;} _LL430: if( _temp398 == ( void*) Cyc_Absyn_Gte){
goto _LL431;} else{ goto _LL432;} _LL432: if( _temp398 == ( void*) Cyc_Absyn_Lte){
goto _LL433;} else{ goto _LL434;} _LL434: goto _LL435; _LL401: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL403: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL405: goto _LL407;
_LL407: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL409: goto _LL411; _LL411: goto _LL413; _LL413: goto _LL415; _LL415: goto
_LL417; _LL417: goto _LL419; _LL419: goto _LL421; _LL421: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL423: goto _LL425; _LL425: goto _LL427;
_LL427: goto _LL429; _LL429: goto _LL431; _LL431: goto _LL433; _LL433: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL435:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp436=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp436[ 0]=({ struct Cyc_Core_Failure_struct
_temp437; _temp437.tag= Cyc_Core_Failure; _temp437.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp437;}); _temp436;})); _LL399:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p == ( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) ==  1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t; switch((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ case 0: _LL438: return({ void* _temp440[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("primitive operator has 0 arguments", sizeof( unsigned char),
35u), _tag_arr( _temp440, sizeof( void*), 0u));}); case 1: _LL439: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
es))->hd); break; case 2: _LL441: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd); break; default: _LL442: return({ void* _temp444[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u), _tag_arr( _temp444, sizeof( void*), 0u));});}
return t;}} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp445=( void*) e->r; void* _temp469; struct Cyc_Absyn_Vardecl* _temp471;
void* _temp473; struct Cyc_Absyn_Vardecl* _temp475; void* _temp477; struct Cyc_Absyn_Vardecl*
_temp479; void* _temp481; struct Cyc_Absyn_Vardecl* _temp483; struct Cyc_Absyn_Exp*
_temp485; struct Cyc_Absyn_Exp* _temp487; struct _tagged_arr* _temp489; struct
Cyc_Absyn_Exp* _temp491; struct _tagged_arr* _temp493; struct Cyc_Absyn_Exp*
_temp495; struct Cyc_Absyn_Exp* _temp497; struct Cyc_Absyn_Exp* _temp499; struct
Cyc_Absyn_Exp* _temp501; _LL447: if(*(( int*) _temp445) ==  Cyc_Absyn_Var_e){
_LL470: _temp469=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp445)->f2; if((
unsigned int) _temp469 >  1u?*(( int*) _temp469) ==  Cyc_Absyn_Param_b: 0){
_LL472: _temp471=(( struct Cyc_Absyn_Param_b_struct*) _temp469)->f1; goto _LL448;}
else{ goto _LL449;}} else{ goto _LL449;} _LL449: if(*(( int*) _temp445) ==  Cyc_Absyn_Var_e){
_LL474: _temp473=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp445)->f2; if((
unsigned int) _temp473 >  1u?*(( int*) _temp473) ==  Cyc_Absyn_Local_b: 0){
_LL476: _temp475=(( struct Cyc_Absyn_Local_b_struct*) _temp473)->f1; goto _LL450;}
else{ goto _LL451;}} else{ goto _LL451;} _LL451: if(*(( int*) _temp445) ==  Cyc_Absyn_Var_e){
_LL478: _temp477=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp445)->f2; if((
unsigned int) _temp477 >  1u?*(( int*) _temp477) ==  Cyc_Absyn_Pat_b: 0){ _LL480:
_temp479=(( struct Cyc_Absyn_Pat_b_struct*) _temp477)->f1; goto _LL452;} else{
goto _LL453;}} else{ goto _LL453;} _LL453: if(*(( int*) _temp445) ==  Cyc_Absyn_Var_e){
_LL482: _temp481=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp445)->f2; if((
unsigned int) _temp481 >  1u?*(( int*) _temp481) ==  Cyc_Absyn_Global_b: 0){
_LL484: _temp483=(( struct Cyc_Absyn_Global_b_struct*) _temp481)->f1; goto
_LL454;} else{ goto _LL455;}} else{ goto _LL455;} _LL455: if(*(( int*) _temp445)
==  Cyc_Absyn_Subscript_e){ _LL488: _temp487=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp445)->f1; goto _LL486; _LL486: _temp485=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp445)->f2; goto _LL456;} else{ goto _LL457;} _LL457: if(*(( int*) _temp445)
==  Cyc_Absyn_StructMember_e){ _LL492: _temp491=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp445)->f1; goto _LL490; _LL490: _temp489=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp445)->f2; goto _LL458;} else{ goto _LL459;} _LL459: if(*(( int*) _temp445)
==  Cyc_Absyn_StructArrow_e){ _LL496: _temp495=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp445)->f1; goto _LL494; _LL494: _temp493=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp445)->f2; goto _LL460;} else{ goto _LL461;} _LL461: if(*(( int*) _temp445)
==  Cyc_Absyn_Deref_e){ _LL498: _temp497=(( struct Cyc_Absyn_Deref_e_struct*)
_temp445)->f1; goto _LL462;} else{ goto _LL463;} _LL463: if(*(( int*) _temp445)
==  Cyc_Absyn_NoInstantiate_e){ _LL500: _temp499=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp445)->f1; goto _LL464;} else{ goto _LL465;} _LL465: if(*(( int*) _temp445)
==  Cyc_Absyn_Instantiate_e){ _LL502: _temp501=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp445)->f1; goto _LL466;} else{ goto _LL467;} _LL467: goto _LL468; _LL448:
_temp475= _temp471; goto _LL450; _LL450: _temp479= _temp475; goto _LL452; _LL452:
_temp483= _temp479; goto _LL454; _LL454: if( !( _temp483->tq).q_const){ return;}
goto _LL446; _LL456:{ void* _temp503= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp487->topt))->v); struct Cyc_Absyn_PtrInfo _temp513; struct Cyc_Absyn_Tqual
_temp515; struct Cyc_Absyn_Tqual _temp517; struct Cyc_List_List* _temp519;
_LL505: if(( unsigned int) _temp503 >  4u?*(( int*) _temp503) ==  Cyc_Absyn_PointerType:
0){ _LL514: _temp513=(( struct Cyc_Absyn_PointerType_struct*) _temp503)->f1;
_LL516: _temp515= _temp513.tq; goto _LL506;} else{ goto _LL507;} _LL507: if((
unsigned int) _temp503 >  4u?*(( int*) _temp503) ==  Cyc_Absyn_ArrayType: 0){
_LL518: _temp517=(( struct Cyc_Absyn_ArrayType_struct*) _temp503)->f2; goto
_LL508;} else{ goto _LL509;} _LL509: if(( unsigned int) _temp503 >  4u?*(( int*)
_temp503) ==  Cyc_Absyn_TupleType: 0){ _LL520: _temp519=(( struct Cyc_Absyn_TupleType_struct*)
_temp503)->f1; goto _LL510;} else{ goto _LL511;} _LL511: goto _LL512; _LL506:
_temp517= _temp515; goto _LL508; _LL508: if( ! _temp517.q_const){ return;} goto
_LL504; _LL510: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp485);{
struct _handler_cons _temp521; _push_handler(& _temp521);{ int _temp523= 0; if(
setjmp( _temp521.handler)){ _temp523= 1;} if( ! _temp523){{ struct _tuple8
_temp526; struct Cyc_Absyn_Tqual _temp527; struct _tuple8* _temp524=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp519,( int) i);
_temp526=* _temp524; _LL528: _temp527= _temp526.f1; goto _LL525; _LL525: if( !
_temp527.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp522=( void*) _exn_thrown; void* _temp530= _temp522; _LL532: if( _temp530 == 
Cyc_List_Nth){ goto _LL533;} else{ goto _LL534;} _LL534: goto _LL535; _LL533:
return; _LL535:( void) _throw( _temp530); _LL531:;}}} goto _LL504;} _LL512: goto
_LL504; _LL504:;} goto _LL446; _LL458:{ void* _temp536= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp491->topt))->v); struct Cyc_Absyn_Structdecl**
_temp548; struct Cyc_List_List* _temp550; struct Cyc_List_List* _temp552; struct
Cyc_Absyn_Uniondecl** _temp554; _LL538: if(( unsigned int) _temp536 >  4u?*((
int*) _temp536) ==  Cyc_Absyn_StructType: 0){ _LL549: _temp548=(( struct Cyc_Absyn_StructType_struct*)
_temp536)->f3; goto _LL539;} else{ goto _LL540;} _LL540: if(( unsigned int)
_temp536 >  4u?*(( int*) _temp536) ==  Cyc_Absyn_AnonUnionType: 0){ _LL551:
_temp550=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp536)->f1; goto _LL541;}
else{ goto _LL542;} _LL542: if(( unsigned int) _temp536 >  4u?*(( int*) _temp536)
==  Cyc_Absyn_AnonStructType: 0){ _LL553: _temp552=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp536)->f1; goto _LL543;} else{ goto _LL544;} _LL544: if(( unsigned int)
_temp536 >  4u?*(( int*) _temp536) ==  Cyc_Absyn_UnionType: 0){ _LL555: _temp554=((
struct Cyc_Absyn_UnionType_struct*) _temp536)->f3; goto _LL545;} else{ goto
_LL546;} _LL546: goto _LL547; _LL539: { struct Cyc_Absyn_Structfield* sf=
_temp548 ==  0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp548)), _temp489); if( sf ==  0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL537;} _LL541: _temp552= _temp550; goto _LL543; _LL543: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp552, _temp489); if( sf ==  0? 1: !((( struct
Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;} goto _LL537;}
_LL545: { struct Cyc_Absyn_Structfield* sf= _temp554 ==  0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)*(( struct Cyc_Absyn_Uniondecl**) _check_null(
_temp554)), _temp489); if( sf ==  0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL537;} _LL547: goto _LL537;
_LL537:;} goto _LL446; _LL460:{ void* _temp556= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp495->topt))->v); struct Cyc_Absyn_PtrInfo
_temp562; void* _temp564; _LL558: if(( unsigned int) _temp556 >  4u?*(( int*)
_temp556) ==  Cyc_Absyn_PointerType: 0){ _LL563: _temp562=(( struct Cyc_Absyn_PointerType_struct*)
_temp556)->f1; _LL565: _temp564=( void*) _temp562.elt_typ; goto _LL559;} else{
goto _LL560;} _LL560: goto _LL561; _LL559:{ void* _temp566= Cyc_Tcutil_compress(
_temp564); struct Cyc_Absyn_Structdecl** _temp578; struct Cyc_Absyn_Uniondecl**
_temp580; struct Cyc_List_List* _temp582; struct Cyc_List_List* _temp584; _LL568:
if(( unsigned int) _temp566 >  4u?*(( int*) _temp566) ==  Cyc_Absyn_StructType:
0){ _LL579: _temp578=(( struct Cyc_Absyn_StructType_struct*) _temp566)->f3; goto
_LL569;} else{ goto _LL570;} _LL570: if(( unsigned int) _temp566 >  4u?*(( int*)
_temp566) ==  Cyc_Absyn_UnionType: 0){ _LL581: _temp580=(( struct Cyc_Absyn_UnionType_struct*)
_temp566)->f3; goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int)
_temp566 >  4u?*(( int*) _temp566) ==  Cyc_Absyn_AnonStructType: 0){ _LL583:
_temp582=(( struct Cyc_Absyn_AnonStructType_struct*) _temp566)->f1; goto _LL573;}
else{ goto _LL574;} _LL574: if(( unsigned int) _temp566 >  4u?*(( int*) _temp566)
==  Cyc_Absyn_AnonUnionType: 0){ _LL585: _temp584=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp566)->f1; goto _LL575;} else{ goto _LL576;} _LL576: goto _LL577; _LL569: {
struct Cyc_Absyn_Structfield* sf= _temp578 ==  0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp578)), _temp493); if( sf ==  0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL567;} _LL571: { struct Cyc_Absyn_Structfield*
sf= _temp580 ==  0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp580)), _temp493); if( sf ==  0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL567;} _LL573: _temp584= _temp582; goto _LL575; _LL575: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp584, _temp493); if( sf ==  0? 1: !((( struct
Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;} goto _LL567;}
_LL577: goto _LL567; _LL567:;} goto _LL557; _LL561: goto _LL557; _LL557:;} goto
_LL446; _LL462:{ void* _temp586= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp497->topt))->v); struct Cyc_Absyn_PtrInfo _temp594; struct Cyc_Absyn_Tqual
_temp596; struct Cyc_Absyn_Tqual _temp598; _LL588: if(( unsigned int) _temp586 > 
4u?*(( int*) _temp586) ==  Cyc_Absyn_PointerType: 0){ _LL595: _temp594=(( struct
Cyc_Absyn_PointerType_struct*) _temp586)->f1; _LL597: _temp596= _temp594.tq;
goto _LL589;} else{ goto _LL590;} _LL590: if(( unsigned int) _temp586 >  4u?*((
int*) _temp586) ==  Cyc_Absyn_ArrayType: 0){ _LL599: _temp598=(( struct Cyc_Absyn_ArrayType_struct*)
_temp586)->f2; goto _LL591;} else{ goto _LL592;} _LL592: goto _LL593; _LL589:
_temp598= _temp596; goto _LL591; _LL591: if( ! _temp598.q_const){ return;} goto
_LL587; _LL593: goto _LL587; _LL587:;} goto _LL446; _LL464: _temp501= _temp499;
goto _LL466; _LL466: Cyc_Tcexp_check_writable( te, _temp501); return; _LL468:
goto _LL446; _LL446:;}({ struct Cyc_Std_String_pa_struct _temp601; _temp601.tag=
Cyc_Std_String_pa; _temp601.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{
void* _temp600[ 1u]={& _temp601}; Cyc_Tcutil_terr( e->loc, _tag_arr("attempt to write a const location: %s",
sizeof( unsigned char), 38u), _tag_arr( _temp600, sizeof( void*), 1u));}});}
static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return({ void* _temp602[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("increment/decrement of non-lvalue", sizeof( unsigned char),
34u), _tag_arr( _temp602, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable( te,
e);{ void* t=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( !
Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){({ struct
Cyc_Std_String_pa_struct _temp604; _temp604.tag= Cyc_Std_String_pa; _temp604.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp603[ 1u]={& _temp604};
Cyc_Tcutil_terr( e->loc, _tag_arr("expecting arithmetic or ? type but found %s",
sizeof( unsigned char), 44u), _tag_arr( _temp603, sizeof( void*), 1u));}});}
return t;}} static void* Cyc_Tcexp_tcConditional( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest( te, e1, _tag_arr("conditional expression",
sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp( te, topt, e2); Cyc_Tcexp_tcExp(
te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({
struct Cyc_Core_Opt* _temp612=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp612->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp612;}));
struct Cyc_List_List* _temp605=({ struct Cyc_List_List* _temp611=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp611->hd=( void*) e3; _temp611->tl=
0; _temp611;}); struct Cyc_List_List* _temp606=({ struct Cyc_List_List* _temp610=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp610->hd=(
void*) e2; _temp610->tl= _temp605; _temp610;}); if( ! Cyc_Tcutil_coerce_list( te,
t, _temp606)){({ struct Cyc_Std_String_pa_struct _temp609; _temp609.tag= Cyc_Std_String_pa;
_temp609.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e3->topt))->v);{ struct Cyc_Std_String_pa_struct _temp608; _temp608.tag=
Cyc_Std_String_pa; _temp608.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp607[ 2u]={&
_temp608,& _temp609}; Cyc_Tcutil_terr( loc, _tag_arr("conditional clause types do not match: %s != %s",
sizeof( unsigned char), 48u), _tag_arr( _temp607, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1); Cyc_Tcexp_tcExp(
te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v)),
e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void*
t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ void* _temp613=
Cyc_Tcutil_compress( t1); _LL615: if(( unsigned int) _temp613 >  4u?*(( int*)
_temp613) ==  Cyc_Absyn_ArrayType: 0){ goto _LL616;} else{ goto _LL617;} _LL617:
goto _LL618; _LL616:({ void* _temp619[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("cannot assign to an array",
sizeof( unsigned char), 26u), _tag_arr( _temp619, sizeof( void*), 0u));}); goto
_LL614; _LL618: goto _LL614; _LL614:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return({
void* _temp620[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue",
sizeof( unsigned char), 25u), _tag_arr( _temp620, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e1); if( po ==  0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ void*
_temp621=({ struct Cyc_Std_String_pa_struct _temp624; _temp624.tag= Cyc_Std_String_pa;
_temp624.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp623; _temp623.tag= Cyc_Std_String_pa; _temp623.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp622[ 2u]={& _temp623,& _temp624}; Cyc_Tcexp_expr_err( te, loc,
_tag_arr("type mismatch: %s != %s", sizeof( unsigned char), 24u), _tag_arr(
_temp622, sizeof( void*), 2u));}}}); Cyc_Tcutil_unify( t1, t2); Cyc_Tcutil_explain_failure();
return _temp621;}} else{ void* _temp625=( void*)(( struct Cyc_Core_Opt*)
_check_null( po))->v; void* _temp626= Cyc_Tcexp_tcBinPrimop( te, loc, 0,
_temp625, e1, e2); if( !( Cyc_Tcutil_unify( _temp626, t1)? 1:( Cyc_Tcutil_coerceable(
_temp626)? Cyc_Tcutil_coerceable( t1): 0))){ void* _temp627=({ struct Cyc_Std_String_pa_struct
_temp630; _temp630.tag= Cyc_Std_String_pa; _temp630.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ struct Cyc_Std_String_pa_struct _temp629; _temp629.tag= Cyc_Std_String_pa;
_temp629.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp628[
2u]={& _temp629,& _temp630}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof( unsigned char), 82u), _tag_arr( _temp628, sizeof( void*), 2u));}}}); Cyc_Tcutil_unify(
_temp626, t1); Cyc_Tcutil_explain_failure(); return _temp627;} return _temp626;}
return t1;}} static void* Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v;} static struct Cyc_Absyn_Tunionfield*
Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
tu, struct _RegionHandle* r, struct Cyc_List_List* inst, struct Cyc_List_List*
fs){ struct Cyc_List_List* fields; void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;{ void* _temp631= Cyc_Tcutil_compress( t1); void*
_temp641; void* _temp643; _LL633: if( _temp631 == ( void*) Cyc_Absyn_FloatType){
goto _LL634;} else{ goto _LL635;} _LL635: if(( unsigned int) _temp631 >  4u?*((
int*) _temp631) ==  Cyc_Absyn_IntType: 0){ _LL642: _temp641=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp631)->f2; if( _temp641 == ( void*) Cyc_Absyn_B1){
goto _LL636;} else{ goto _LL637;}} else{ goto _LL637;} _LL637: if(( unsigned int)
_temp631 >  4u?*(( int*) _temp631) ==  Cyc_Absyn_IntType: 0){ _LL644: _temp643=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp631)->f2; if( _temp643 == ( void*)
Cyc_Absyn_B2){ goto _LL638;} else{ goto _LL639;}} else{ goto _LL639;} _LL639:
goto _LL640; _LL634: Cyc_Tcutil_unchecked_cast( te, e,( void*) Cyc_Absyn_DoubleType);
t1=( void*) Cyc_Absyn_DoubleType; goto _LL632; _LL636: goto _LL638; _LL638: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); t1= Cyc_Absyn_sint_t; goto _LL632; _LL640: goto _LL632;
_LL632:;} for( fields= fs; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ void* _temp647; struct Cyc_Position_Segment*
_temp649; struct Cyc_List_List* _temp651; struct Cyc_List_List* _temp653; struct
_tuple1* _temp655; struct Cyc_Absyn_Tunionfield _temp645=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd); _LL656: _temp655= _temp645.name;
goto _LL654; _LL654: _temp653= _temp645.tvs; goto _LL652; _LL652: _temp651=
_temp645.typs; goto _LL650; _LL650: _temp649= _temp645.loc; goto _LL648; _LL648:
_temp647=( void*) _temp645.sc; goto _LL646; _LL646: if(( _temp651 ==  0? 1:((
struct Cyc_List_List*) _check_null( _temp651))->tl !=  0)? 1: _temp653 !=  0){
continue;}{ void* t2= Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( _temp651))->hd)).f2); if( Cyc_Tcutil_unify(
t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd);}}} for( fields= fs; fields != 
0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ void* _temp659;
struct Cyc_Position_Segment* _temp661; struct Cyc_List_List* _temp663; struct
Cyc_List_List* _temp665; struct _tuple1* _temp667; struct Cyc_Absyn_Tunionfield
_temp657=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL668: _temp667= _temp657.name; goto _LL666; _LL666: _temp665=
_temp657.tvs; goto _LL664; _LL664: _temp663= _temp657.typs; goto _LL662; _LL662:
_temp661= _temp657.loc; goto _LL660; _LL660: _temp659=( void*) _temp657.sc; goto
_LL658; _LL658: if(( _temp663 ==  0? 1:(( struct Cyc_List_List*) _check_null(
_temp663))->tl !=  0)? 1: _temp665 !=  0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp663))->hd)).f2);
if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}({
struct Cyc_Std_String_pa_struct _temp671; _temp671.tag= Cyc_Std_String_pa;
_temp671.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ struct Cyc_Std_String_pa_struct
_temp670; _temp670.tag= Cyc_Std_String_pa; _temp670.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
tu);{ void* _temp669[ 2u]={& _temp670,& _temp671}; Cyc_Tcutil_terr( e->loc,
_tag_arr("can't find a field in %s to inject a value of type %s", sizeof(
unsigned char), 54u), _tag_arr( _temp669, sizeof( void*), 2u));}}}); return 0;}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* args, struct
Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List* _temp672=
args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp673= t; struct Cyc_Absyn_PtrInfo _temp679; struct Cyc_Absyn_Conref*
_temp681; struct Cyc_Absyn_Tqual _temp683; struct Cyc_Absyn_Conref* _temp685;
void* _temp687; void* _temp689; _LL675: if(( unsigned int) _temp673 >  4u?*((
int*) _temp673) ==  Cyc_Absyn_PointerType: 0){ _LL680: _temp679=(( struct Cyc_Absyn_PointerType_struct*)
_temp673)->f1; _LL690: _temp689=( void*) _temp679.elt_typ; goto _LL688; _LL688:
_temp687=( void*) _temp679.rgn_typ; goto _LL686; _LL686: _temp685= _temp679.nullable;
goto _LL684; _LL684: _temp683= _temp679.tq; goto _LL682; _LL682: _temp681=
_temp679.bounds; goto _LL676;} else{ goto _LL677;} _LL677: goto _LL678; _LL676:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp687); Cyc_Tcutil_check_nonzero_bound(
loc, _temp681);{ void* _temp691= Cyc_Tcutil_compress( _temp689); struct Cyc_Absyn_FnInfo
_temp697; struct Cyc_List_List* _temp699; struct Cyc_List_List* _temp701; struct
Cyc_Absyn_VarargInfo* _temp703; int _temp705; struct Cyc_List_List* _temp707;
void* _temp709; struct Cyc_Core_Opt* _temp711; struct Cyc_List_List* _temp713;
_LL693: if(( unsigned int) _temp691 >  4u?*(( int*) _temp691) ==  Cyc_Absyn_FnType:
0){ _LL698: _temp697=(( struct Cyc_Absyn_FnType_struct*) _temp691)->f1; _LL714:
_temp713= _temp697.tvars; goto _LL712; _LL712: _temp711= _temp697.effect; goto
_LL710; _LL710: _temp709=( void*) _temp697.ret_typ; goto _LL708; _LL708:
_temp707= _temp697.args; goto _LL706; _LL706: _temp705= _temp697.c_varargs; goto
_LL704; _LL704: _temp703= _temp697.cyc_varargs; goto _LL702; _LL702: _temp701=
_temp697.rgn_po; goto _LL700; _LL700: _temp699= _temp697.attributes; goto _LL694;}
else{ goto _LL695;} _LL695: goto _LL696; _LL694: if( topt !=  0){ Cyc_Tcutil_unify(
_temp709,*(( void**) _check_null( topt)));} while( _temp672 !=  0? _temp707 != 
0: 0) { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp672))->hd; void* t2=(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp707))->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( !
Cyc_Tcutil_coerce_arg( te, e1, t2)){({ struct Cyc_Std_String_pa_struct _temp717;
_temp717.tag= Cyc_Std_String_pa; _temp717.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ struct Cyc_Std_String_pa_struct _temp716; _temp716.tag= Cyc_Std_String_pa;
_temp716.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v);{ void* _temp715[ 2u]={& _temp716,& _temp717}; Cyc_Tcutil_terr(
e1->loc, _tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof( unsigned char), 54u), _tag_arr( _temp715, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
_temp672=(( struct Cyc_List_List*) _check_null( _temp672))->tl; _temp707=((
struct Cyc_List_List*) _check_null( _temp707))->tl;} if( _temp707 !=  0){({ void*
_temp718[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function",
sizeof( unsigned char), 31u), _tag_arr( _temp718, sizeof( void*), 0u));});}
else{ if(( _temp672 !=  0? 1: _temp705)? 1: _temp703 !=  0){ if( _temp705){ for(
0; _temp672 !=  0; _temp672=(( struct Cyc_List_List*) _check_null( _temp672))->tl){
Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp672))->hd);}} else{ if( _temp703 ==  0){({ void* _temp719[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("too many arguments for function", sizeof(
unsigned char), 32u), _tag_arr( _temp719, sizeof( void*), 0u));});} else{ int
_temp722; void* _temp724; struct Cyc_Absyn_VarargInfo _temp720=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp703)); _LL725: _temp724=( void*) _temp720.type; goto _LL723;
_LL723: _temp722= _temp720.inject; goto _LL721; _LL721: { struct Cyc_Absyn_VarargCallInfo*
_temp726=({ struct Cyc_Absyn_VarargCallInfo* _temp756=( struct Cyc_Absyn_VarargCallInfo*)
_cycalloc( sizeof( struct Cyc_Absyn_VarargCallInfo)); _temp756->num_varargs= 0;
_temp756->injectors= 0; _temp756->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp703); _temp756;});* vararg_call_info=( struct Cyc_Absyn_VarargCallInfo*)
_temp726; if( ! _temp722){ for( 0; _temp672 !=  0; _temp672=(( struct Cyc_List_List*)
_check_null( _temp672))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp672))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp726))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp724,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp724)){({ struct Cyc_Std_String_pa_struct
_temp729; _temp729.tag= Cyc_Std_String_pa; _temp729.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp728; _temp728.tag= Cyc_Std_String_pa; _temp728.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp724);{ void* _temp727[ 2u]={& _temp728,& _temp729}; Cyc_Tcutil_terr( loc,
_tag_arr("vararg requires type %s but argument has type %s", sizeof(
unsigned char), 49u), _tag_arr( _temp727, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}}
else{ void* _temp730= Cyc_Tcutil_compress( _temp724); struct Cyc_Absyn_TunionInfo
_temp736; void* _temp738; struct Cyc_List_List* _temp740; void* _temp742; struct
Cyc_Absyn_Tuniondecl** _temp744; struct Cyc_Absyn_Tuniondecl* _temp746; _LL732:
if(( unsigned int) _temp730 >  4u?*(( int*) _temp730) ==  Cyc_Absyn_TunionType:
0){ _LL737: _temp736=(( struct Cyc_Absyn_TunionType_struct*) _temp730)->f1;
_LL743: _temp742=( void*) _temp736.tunion_info; if(*(( int*) _temp742) ==  Cyc_Absyn_KnownTunion){
_LL745: _temp744=(( struct Cyc_Absyn_KnownTunion_struct*) _temp742)->f1;
_temp746=* _temp744; goto _LL741;} else{ goto _LL734;} _LL741: _temp740=
_temp736.targs; goto _LL739; _LL739: _temp738=( void*) _temp736.rgn; goto _LL733;}
else{ goto _LL734;} _LL734: goto _LL735; _LL733: { struct Cyc_Absyn_Tuniondecl*
_temp747=* Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp746->name); struct Cyc_List_List*
fields= 0; if( _temp747->fields ==  0){({ struct Cyc_Std_String_pa_struct
_temp749; _temp749.tag= Cyc_Std_String_pa; _temp749.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp724);{ void* _temp748[ 1u]={& _temp749}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject into %s",
sizeof( unsigned char), 21u), _tag_arr( _temp748, sizeof( void*), 1u));}});}
else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp747->fields))->v;} if( ! Cyc_Tcutil_unify( _temp738, Cyc_Tcenv_curr_rgn( te))){({
void* _temp750[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad region for injected varargs",
sizeof( unsigned char), 32u), _tag_arr( _temp750, sizeof( void*), 0u));});}{
struct _RegionHandle _temp751= _new_region(); struct _RegionHandle* rgn=&
_temp751; _push_region( rgn);{ struct Cyc_List_List* _temp752=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp747->tvs, _temp740);
for( 0; _temp672 !=  0; _temp672=(( struct Cyc_List_List*) _check_null( _temp672))->tl){((
struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp726))->num_varargs ++;{
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp672))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{ struct Cyc_Absyn_Tunionfield*
_temp753= Cyc_Tcexp_tcInjection( te, e1, _temp724, rgn, _temp752, fields); if(
_temp753 !=  0){(( struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp726))->injectors=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp726))->injectors,({ struct
Cyc_List_List* _temp754=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp754->hd=( void*)(( struct Cyc_Absyn_Tunionfield*) _check_null( _temp753));
_temp754->tl= 0; _temp754;}));}}}}}; _pop_region( rgn);} goto _LL731;} _LL735:({
void* _temp755[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad inject vararg type",
sizeof( unsigned char), 23u), _tag_arr( _temp755, sizeof( void*), 0u));}); goto
_LL731; _LL731:;}}}}}}{ struct Cyc_List_List* a= _temp699; for( 0; a !=  0; a=((
struct Cyc_List_List*) _check_null( a))->tl){ void* _temp757=( void*)(( struct
Cyc_List_List*) _check_null( a))->hd; int _temp763; int _temp765; void* _temp767;
_LL759: if(( unsigned int) _temp757 >  16u?*(( int*) _temp757) ==  Cyc_Absyn_Format_att:
0){ _LL768: _temp767=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp757)->f1;
goto _LL766; _LL766: _temp765=(( struct Cyc_Absyn_Format_att_struct*) _temp757)->f2;
goto _LL764; _LL764: _temp763=(( struct Cyc_Absyn_Format_att_struct*) _temp757)->f3;
goto _LL760;} else{ goto _LL761;} _LL761: goto _LL762; _LL760:{ struct
_handler_cons _temp769; _push_handler(& _temp769);{ int _temp771= 0; if( setjmp(
_temp769.handler)){ _temp771= 1;} if( ! _temp771){{ struct Cyc_Absyn_Exp*
_temp772=(( struct Cyc_Absyn_Exp*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)(
args, _temp765 -  1); struct Cyc_Core_Opt* fmt_args; if( _temp763 ==  0){
fmt_args= 0;} else{ fmt_args=({ struct Cyc_Core_Opt* _temp773=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp773->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth_tail)( args, _temp763 -  1);
_temp773;});}{ void* _temp774= _temp767; _LL776: if( _temp774 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL777;} else{ goto _LL778;} _LL778: if( _temp774 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL779;} else{ goto _LL775;} _LL777: Cyc_Tcexp_check_format_args( te,
_temp772, fmt_args, Cyc_Formatstr_get_format_typs); goto _LL775; _LL779: Cyc_Tcexp_check_format_args(
te, _temp772, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL775; _LL775:;}};
_pop_handler();} else{ void* _temp770=( void*) _exn_thrown; void* _temp781=
_temp770; _LL783: if( _temp781 ==  Cyc_List_Nth){ goto _LL784;} else{ goto
_LL785;} _LL785: goto _LL786; _LL784:({ void* _temp787[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad format arguments", sizeof( unsigned char), 21u), _tag_arr(
_temp787, sizeof( void*), 0u));}); goto _LL782; _LL786:( void) _throw( _temp781);
_LL782:;}}} goto _LL758; _LL762: goto _LL758; _LL758:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp711))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp701); return _temp709; _LL696: return({ void* _temp788[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function", sizeof(
unsigned char), 29u), _tag_arr( _temp788, sizeof( void*), 0u));}); _LL692:;}
_LL678: return({ void* _temp789[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u), _tag_arr( _temp789, sizeof( void*), 0u));});
_LL674:;}} static void* Cyc_Tcexp_tcThrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ,
e); if( ! Cyc_Tcutil_coerce_arg( te, e, Cyc_Absyn_exn_typ)){({ struct Cyc_Std_String_pa_struct
_temp791; _temp791.tag= Cyc_Std_String_pa; _temp791.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp790[ 1u]={&
_temp791}; Cyc_Tcutil_terr( loc, _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp790, sizeof( void*), 1u));}});}
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp792=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp792->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp792;}));} static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp793= t1;
struct Cyc_Absyn_PtrInfo _temp799; struct Cyc_Absyn_Conref* _temp801; struct Cyc_Absyn_Tqual
_temp803; struct Cyc_Absyn_Conref* _temp805; void* _temp807; void* _temp809;
_LL795: if(( unsigned int) _temp793 >  4u?*(( int*) _temp793) ==  Cyc_Absyn_PointerType:
0){ _LL800: _temp799=(( struct Cyc_Absyn_PointerType_struct*) _temp793)->f1;
_LL810: _temp809=( void*) _temp799.elt_typ; goto _LL808; _LL808: _temp807=( void*)
_temp799.rgn_typ; goto _LL806; _LL806: _temp805= _temp799.nullable; goto _LL804;
_LL804: _temp803= _temp799.tq; goto _LL802; _LL802: _temp801= _temp799.bounds;
goto _LL796;} else{ goto _LL797;} _LL797: goto _LL798; _LL796:{ void* _temp811=
Cyc_Tcutil_compress( _temp809); struct Cyc_Absyn_FnInfo _temp817; struct Cyc_List_List*
_temp819; struct Cyc_List_List* _temp821; struct Cyc_Absyn_VarargInfo* _temp823;
int _temp825; struct Cyc_List_List* _temp827; void* _temp829; struct Cyc_Core_Opt*
_temp831; struct Cyc_List_List* _temp833; _LL813: if(( unsigned int) _temp811 > 
4u?*(( int*) _temp811) ==  Cyc_Absyn_FnType: 0){ _LL818: _temp817=(( struct Cyc_Absyn_FnType_struct*)
_temp811)->f1; _LL834: _temp833= _temp817.tvars; goto _LL832; _LL832: _temp831=
_temp817.effect; goto _LL830; _LL830: _temp829=( void*) _temp817.ret_typ; goto
_LL828; _LL828: _temp827= _temp817.args; goto _LL826; _LL826: _temp825= _temp817.c_varargs;
goto _LL824; _LL824: _temp823= _temp817.cyc_varargs; goto _LL822; _LL822:
_temp821= _temp817.rgn_po; goto _LL820; _LL820: _temp819= _temp817.attributes;
goto _LL814;} else{ goto _LL815;} _LL815: goto _LL816; _LL814: { struct Cyc_List_List*
instantiation= 0; if( Cyc_List_length( ts) != (( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp833)){ return({ void* _temp835[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u),
_tag_arr( _temp835, sizeof( void*), 0u));});} for( 0; ts !=  0;( ts=(( struct
Cyc_List_List*) _check_null( ts))->tl, _temp833=(( struct Cyc_List_List*)
_check_null( _temp833))->tl)){ void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp833))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k, 1,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); instantiation=({ struct Cyc_List_List* _temp836=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp836->hd=( void*)({
struct _tuple5* _temp837=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp837->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp833))->hd; _temp837->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp837;}); _temp836->tl= instantiation; _temp836;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp841=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp841[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp842; _temp842.tag= Cyc_Absyn_FnType; _temp842.f1=({ struct Cyc_Absyn_FnInfo
_temp843; _temp843.tvars= 0; _temp843.effect= _temp831; _temp843.ret_typ=( void*)
_temp829; _temp843.args= _temp827; _temp843.c_varargs= _temp825; _temp843.cyc_varargs=
_temp823; _temp843.rgn_po= _temp821; _temp843.attributes= _temp819; _temp843;});
_temp842;}); _temp841;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp838=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp838[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp839; _temp839.tag= Cyc_Absyn_PointerType;
_temp839.f1=({ struct Cyc_Absyn_PtrInfo _temp840; _temp840.elt_typ=( void*)
new_fn_typ; _temp840.rgn_typ=( void*) _temp807; _temp840.nullable= _temp805;
_temp840.tq= _temp803; _temp840.bounds= _temp801; _temp840;}); _temp839;});
_temp838;}); return new_typ;}} _LL816: goto _LL812; _LL812:;} goto _LL794;
_LL798: goto _LL794; _LL794:;} return({ struct Cyc_Std_String_pa_struct _temp845;
_temp845.tag= Cyc_Std_String_pa; _temp845.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp844[ 1u]={& _temp845}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting polymorphic type but found %s",
sizeof( unsigned char), 40u), _tag_arr( _temp844, sizeof( void*), 1u));}});}}
static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t); Cyc_Tcexp_tcExp(
te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0){ Cyc_Tcutil_unify( t2, t);{ void* _temp846=({ struct Cyc_Std_String_pa_struct
_temp849; _temp849.tag= Cyc_Std_String_pa; _temp849.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp848; _temp848.tag= Cyc_Std_String_pa;
_temp848.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp847[
2u]={& _temp848,& _temp849}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u), _tag_arr( _temp847, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();
return _temp846;}} return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
void** _temp850= 0; struct Cyc_Absyn_Tqual _temp851= Cyc_Absyn_empty_tqual();
if( topt !=  0){ void* _temp852= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp858; struct Cyc_Absyn_Tqual _temp860;
void* _temp862; _LL854: if(( unsigned int) _temp852 >  4u?*(( int*) _temp852) == 
Cyc_Absyn_PointerType: 0){ _LL859: _temp858=(( struct Cyc_Absyn_PointerType_struct*)
_temp852)->f1; _LL863: _temp862=( void*) _temp858.elt_typ; goto _LL861; _LL861:
_temp860= _temp858.tq; goto _LL855;} else{ goto _LL856;} _LL856: goto _LL857;
_LL855: _temp850=({ void** _temp864=( void**) _cycalloc( sizeof( void*));
_temp864[ 0]= _temp862; _temp864;}); _temp851= _temp860; goto _LL853; _LL857:
goto _LL853; _LL853:;} Cyc_Tcexp_tcExpNoInst( te, _temp850, e);{ void* _temp865=(
void*) e->r; _LL867: if(*(( int*) _temp865) ==  Cyc_Absyn_Struct_e){ goto _LL868;}
else{ goto _LL869;} _LL869: if(*(( int*) _temp865) ==  Cyc_Absyn_Tuple_e){ goto
_LL870;} else{ goto _LL871;} _LL871: goto _LL872; _LL868: goto _LL870; _LL870:({
void* _temp873[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate",
sizeof( unsigned char), 19u), _tag_arr( _temp873, sizeof( void*), 0u));});{ void*(*
_temp874)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if(
topt !=  0){ void* _temp875= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp881; struct Cyc_Absyn_Conref* _temp883; _LL877:
if(( unsigned int) _temp875 >  4u?*(( int*) _temp875) ==  Cyc_Absyn_PointerType:
0){ _LL882: _temp881=(( struct Cyc_Absyn_PointerType_struct*) _temp875)->f1;
_LL884: _temp883= _temp881.nullable; goto _LL878;} else{ goto _LL879;} _LL879:
goto _LL880; _LL878: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp883)){ _temp874= Cyc_Absyn_star_typ;} goto _LL876; _LL880: goto _LL876;
_LL876:;} return _temp874(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp851);} _LL872: goto _LL866; _LL866:;}{ void*
_temp887; int _temp889; struct _tuple6 _temp885= Cyc_Tcutil_addressof_props( te,
e); _LL890: _temp889= _temp885.f1; goto _LL888; _LL888: _temp887= _temp885.f2;
goto _LL886; _LL886: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp889){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp887, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
1, t); if( ! Cyc_Evexp_okay_szofarg( t)){({ struct Cyc_Std_String_pa_struct
_temp892; _temp892.tag= Cyc_Std_String_pa; _temp892.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp891[ 1u]={& _temp892}; Cyc_Tcutil_terr( loc, _tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof( unsigned char), 55u), _tag_arr( _temp891, sizeof( void*), 1u));}});}
return Cyc_Absyn_uint_t;} int Cyc_Tcexp_structfield_has_name( struct _tagged_arr*
n, struct Cyc_Absyn_Structfield* sf){ return Cyc_Std_strcmp(* n,* sf->name) == 
0;} static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, void* n){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, 1, t);{ void* _temp893= n; struct _tagged_arr*
_temp899; unsigned int _temp901; _LL895: if(*(( int*) _temp893) ==  Cyc_Absyn_StructField){
_LL900: _temp899=(( struct Cyc_Absyn_StructField_struct*) _temp893)->f1; goto
_LL896;} else{ goto _LL897;} _LL897: if(*(( int*) _temp893) ==  Cyc_Absyn_TupleIndex){
_LL902: _temp901=(( struct Cyc_Absyn_TupleIndex_struct*) _temp893)->f1; goto
_LL898;} else{ goto _LL894;} _LL896: { int bad_type= 1;{ void* _temp903= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp915; struct _tuple1* _temp917; struct Cyc_List_List*
_temp919; struct Cyc_Absyn_Uniondecl** _temp921; struct _tuple1* _temp923;
struct Cyc_List_List* _temp925; _LL905: if(( unsigned int) _temp903 >  4u?*((
int*) _temp903) ==  Cyc_Absyn_StructType: 0){ _LL918: _temp917=(( struct Cyc_Absyn_StructType_struct*)
_temp903)->f1; goto _LL916; _LL916: _temp915=(( struct Cyc_Absyn_StructType_struct*)
_temp903)->f3; goto _LL906;} else{ goto _LL907;} _LL907: if(( unsigned int)
_temp903 >  4u?*(( int*) _temp903) ==  Cyc_Absyn_AnonStructType: 0){ _LL920:
_temp919=(( struct Cyc_Absyn_AnonStructType_struct*) _temp903)->f1; goto _LL908;}
else{ goto _LL909;} _LL909: if(( unsigned int) _temp903 >  4u?*(( int*) _temp903)
==  Cyc_Absyn_UnionType: 0){ _LL924: _temp923=(( struct Cyc_Absyn_UnionType_struct*)
_temp903)->f1; goto _LL922; _LL922: _temp921=(( struct Cyc_Absyn_UnionType_struct*)
_temp903)->f3; goto _LL910;} else{ goto _LL911;} _LL911: if(( unsigned int)
_temp903 >  4u?*(( int*) _temp903) ==  Cyc_Absyn_AnonUnionType: 0){ _LL926:
_temp925=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp903)->f1; goto _LL912;}
else{ goto _LL913;} _LL913: goto _LL914; _LL906: if( _temp915 ==  0){ return({
void* _temp927[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType",
sizeof( unsigned char), 33u), _tag_arr( _temp927, sizeof( void*), 0u));});} if((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp915)))->fields ==  0){ goto
_LL904;} if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Structfield*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name,
_temp899,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp915)))->fields))->v)){({ struct
Cyc_Std_String_pa_struct _temp929; _temp929.tag= Cyc_Std_String_pa; _temp929.f1=(
struct _tagged_arr)* _temp899;{ void* _temp928[ 1u]={& _temp929}; Cyc_Tcutil_terr(
loc, _tag_arr("no field of struct has name %s", sizeof( unsigned char), 31u),
_tag_arr( _temp928, sizeof( void*), 1u));}});} bad_type= 0; goto _LL904; _LL908:
if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Structfield*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name,
_temp899, _temp919)){({ struct Cyc_Std_String_pa_struct _temp931; _temp931.tag=
Cyc_Std_String_pa; _temp931.f1=( struct _tagged_arr)* _temp899;{ void* _temp930[
1u]={& _temp931}; Cyc_Tcutil_terr( loc, _tag_arr("no field of struct has name %s",
sizeof( unsigned char), 31u), _tag_arr( _temp930, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL904; _LL910: if( _temp921 ==  0){ return({ void* _temp932[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp932, sizeof( void*), 0u));});} if((*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp921)))->fields ==  0){ goto
_LL904;} if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Structfield*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name,
_temp899,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp921)))->fields))->v)){({ struct
Cyc_Std_String_pa_struct _temp934; _temp934.tag= Cyc_Std_String_pa; _temp934.f1=(
struct _tagged_arr)* _temp899;{ void* _temp933[ 1u]={& _temp934}; Cyc_Tcutil_terr(
loc, _tag_arr("no field of union has name %s", sizeof( unsigned char), 30u),
_tag_arr( _temp933, sizeof( void*), 1u));}});} bad_type= 0; goto _LL904; _LL912:
if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Structfield*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name,
_temp899, _temp925)){({ struct Cyc_Std_String_pa_struct _temp936; _temp936.tag=
Cyc_Std_String_pa; _temp936.f1=( struct _tagged_arr)* _temp899;{ void* _temp935[
1u]={& _temp936}; Cyc_Tcutil_terr( loc, _tag_arr("no field of anonymous union has name %s",
sizeof( unsigned char), 40u), _tag_arr( _temp935, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL904; _LL914: goto _LL904; _LL904:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp938; _temp938.tag= Cyc_Std_String_pa; _temp938.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp937[ 1u]={& _temp938};
Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union type", sizeof(
unsigned char), 36u), _tag_arr( _temp937, sizeof( void*), 1u));}});} goto _LL894;}
_LL898: { int bad_type= 1;{ void* _temp939= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp955; struct Cyc_List_List* _temp957; struct Cyc_Absyn_Uniondecl** _temp959;
struct Cyc_List_List* _temp961; struct Cyc_List_List* _temp963; struct Cyc_Absyn_TunionFieldInfo
_temp965; void* _temp967; struct Cyc_Absyn_Tunionfield* _temp969; struct Cyc_Absyn_Tuniondecl*
_temp971; _LL941: if(( unsigned int) _temp939 >  4u?*(( int*) _temp939) ==  Cyc_Absyn_StructType:
0){ _LL956: _temp955=(( struct Cyc_Absyn_StructType_struct*) _temp939)->f3; goto
_LL942;} else{ goto _LL943;} _LL943: if(( unsigned int) _temp939 >  4u?*(( int*)
_temp939) ==  Cyc_Absyn_AnonStructType: 0){ _LL958: _temp957=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp939)->f1; goto _LL944;} else{ goto _LL945;} _LL945: if(( unsigned int)
_temp939 >  4u?*(( int*) _temp939) ==  Cyc_Absyn_UnionType: 0){ _LL960: _temp959=((
struct Cyc_Absyn_UnionType_struct*) _temp939)->f3; goto _LL946;} else{ goto
_LL947;} _LL947: if(( unsigned int) _temp939 >  4u?*(( int*) _temp939) ==  Cyc_Absyn_AnonUnionType:
0){ _LL962: _temp961=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp939)->f1;
goto _LL948;} else{ goto _LL949;} _LL949: if(( unsigned int) _temp939 >  4u?*((
int*) _temp939) ==  Cyc_Absyn_TupleType: 0){ _LL964: _temp963=(( struct Cyc_Absyn_TupleType_struct*)
_temp939)->f1; goto _LL950;} else{ goto _LL951;} _LL951: if(( unsigned int)
_temp939 >  4u?*(( int*) _temp939) ==  Cyc_Absyn_TunionFieldType: 0){ _LL966:
_temp965=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp939)->f1; _LL968:
_temp967=( void*) _temp965.field_info; if(*(( int*) _temp967) ==  Cyc_Absyn_KnownTunionfield){
_LL972: _temp971=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp967)->f1;
goto _LL970; _LL970: _temp969=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp967)->f2; goto _LL952;} else{ goto _LL953;}} else{ goto _LL953;} _LL953:
goto _LL954; _LL942: if( _temp955 ==  0){ return({ void* _temp973[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("offsetof on unchecked StructType", sizeof( unsigned char), 33u),
_tag_arr( _temp973, sizeof( void*), 0u));});} if((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp955)))->fields ==  0){ goto _LL940;} if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp955)))->fields))->v)
<=  _temp901){({ struct Cyc_Std_Int_pa_struct _temp976; _temp976.tag= Cyc_Std_Int_pa;
_temp976.f1=( int) _temp901;{ struct Cyc_Std_Int_pa_struct _temp975; _temp975.tag=
Cyc_Std_Int_pa; _temp975.f1=( int)(( unsigned int)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp955)))->fields))->v));{
void* _temp974[ 2u]={& _temp975,& _temp976}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp974, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL940; _LL944: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp957) <=  _temp901){({ struct Cyc_Std_Int_pa_struct _temp979; _temp979.tag=
Cyc_Std_Int_pa; _temp979.f1=( int) _temp901;{ struct Cyc_Std_Int_pa_struct
_temp978; _temp978.tag= Cyc_Std_Int_pa; _temp978.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp957));{ void* _temp977[ 2u]={&
_temp978,& _temp979}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp977, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL940; _LL946: if( _temp959 ==  0){ return({ void* _temp980[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp980, sizeof( void*), 0u));});} if((*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp959)))->fields ==  0){ goto
_LL940;} if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((*(( struct Cyc_Absyn_Uniondecl**) _check_null(
_temp959)))->fields))->v) <=  _temp901){({ struct Cyc_Std_Int_pa_struct _temp983;
_temp983.tag= Cyc_Std_Int_pa; _temp983.f1=( int) _temp901;{ struct Cyc_Std_Int_pa_struct
_temp982; _temp982.tag= Cyc_Std_Int_pa; _temp982.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp959)))->fields))->v));{
void* _temp981[ 2u]={& _temp982,& _temp983}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp981, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL940; _LL948: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp961) <=  _temp901){({ struct Cyc_Std_Int_pa_struct _temp986; _temp986.tag=
Cyc_Std_Int_pa; _temp986.f1=( int) _temp901;{ struct Cyc_Std_Int_pa_struct
_temp985; _temp985.tag= Cyc_Std_Int_pa; _temp985.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp961));{ void* _temp984[ 2u]={&
_temp985,& _temp986}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp984, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL940; _LL950: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp963) <=  _temp901){({ struct Cyc_Std_Int_pa_struct _temp989; _temp989.tag=
Cyc_Std_Int_pa; _temp989.f1=( int) _temp901;{ struct Cyc_Std_Int_pa_struct
_temp988; _temp988.tag= Cyc_Std_Int_pa; _temp988.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp963));{ void* _temp987[ 2u]={&
_temp988,& _temp989}; Cyc_Tcutil_terr( loc, _tag_arr("tuple has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp987, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL940; _LL952: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp969->typs) <  _temp901){({ struct Cyc_Std_Int_pa_struct _temp992; _temp992.tag=
Cyc_Std_Int_pa; _temp992.f1=( int) _temp901;{ struct Cyc_Std_Int_pa_struct
_temp991; _temp991.tag= Cyc_Std_Int_pa; _temp991.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp969->typs));{ void* _temp990[ 2u]={&
_temp991,& _temp992}; Cyc_Tcutil_terr( loc, _tag_arr("tunionfield has too few components: %d < %d",
sizeof( unsigned char), 44u), _tag_arr( _temp990, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL940; _LL954: goto _LL940; _LL940:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp994; _temp994.tag= Cyc_Std_String_pa; _temp994.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp993[ 1u]={& _temp994};
Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union/tuple/tunion type",
sizeof( unsigned char), 49u), _tag_arr( _temp993, sizeof( void*), 1u));}});}
goto _LL894;} _LL894:;} return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp995= t;
struct Cyc_Absyn_PtrInfo _temp1001; struct Cyc_Absyn_Conref* _temp1003; void*
_temp1005; void* _temp1007; _LL997: if(( unsigned int) _temp995 >  4u?*(( int*)
_temp995) ==  Cyc_Absyn_PointerType: 0){ _LL1002: _temp1001=(( struct Cyc_Absyn_PointerType_struct*)
_temp995)->f1; _LL1008: _temp1007=( void*) _temp1001.elt_typ; goto _LL1006;
_LL1006: _temp1005=( void*) _temp1001.rgn_typ; goto _LL1004; _LL1004: _temp1003=
_temp1001.bounds; goto _LL998;} else{ goto _LL999;} _LL999: goto _LL1000; _LL998:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1005); Cyc_Tcutil_check_nonzero_bound(
loc, _temp1003); return _temp1007; _LL1000: return({ struct Cyc_Std_String_pa_struct
_temp1010; _temp1010.tag= Cyc_Std_String_pa; _temp1010.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1009[ 1u]={& _temp1010}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting * or @ type but found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1009, sizeof( void*), 1u));}}); _LL996:;}} static void* Cyc_Tcexp_tcStructMember(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp1011= t; struct Cyc_Absyn_Structdecl**
_temp1027; struct Cyc_Absyn_Structdecl* _temp1029; struct Cyc_List_List*
_temp1030; struct _tuple1* _temp1032; struct Cyc_List_List* _temp1034; struct
Cyc_List_List* _temp1036; struct Cyc_Absyn_Uniondecl** _temp1038; struct Cyc_Absyn_Uniondecl*
_temp1040; struct Cyc_List_List* _temp1041; struct _tuple1* _temp1043; _LL1013:
if(( unsigned int) _temp1011 >  4u?*(( int*) _temp1011) ==  Cyc_Absyn_StructType:
0){ _LL1033: _temp1032=(( struct Cyc_Absyn_StructType_struct*) _temp1011)->f1;
goto _LL1031; _LL1031: _temp1030=(( struct Cyc_Absyn_StructType_struct*)
_temp1011)->f2; goto _LL1028; _LL1028: _temp1027=(( struct Cyc_Absyn_StructType_struct*)
_temp1011)->f3; if( _temp1027 ==  0){ goto _LL1015;} else{ _temp1029=* _temp1027;
goto _LL1014;}} else{ goto _LL1015;} _LL1015: if(( unsigned int) _temp1011 >  4u?*((
int*) _temp1011) ==  Cyc_Absyn_AnonStructType: 0){ _LL1035: _temp1034=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1011)->f1; goto _LL1016;} else{ goto
_LL1017;} _LL1017: if(( unsigned int) _temp1011 >  4u?*(( int*) _temp1011) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1037: _temp1036=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1011)->f1; goto _LL1018;} else{ goto _LL1019;} _LL1019: if(( unsigned int)
_temp1011 >  4u?*(( int*) _temp1011) ==  Cyc_Absyn_UnionType: 0){ _LL1044:
_temp1043=(( struct Cyc_Absyn_UnionType_struct*) _temp1011)->f1; goto _LL1042;
_LL1042: _temp1041=(( struct Cyc_Absyn_UnionType_struct*) _temp1011)->f2; goto
_LL1039; _LL1039: _temp1038=(( struct Cyc_Absyn_UnionType_struct*) _temp1011)->f3;
if( _temp1038 ==  0){ goto _LL1021;} else{ _temp1040=* _temp1038; goto _LL1020;}}
else{ goto _LL1021;} _LL1021: if(( unsigned int) _temp1011 >  4u?*(( int*)
_temp1011) ==  Cyc_Absyn_ArrayType: 0){ goto _LL1045;} else{ goto _LL1023;}
_LL1045: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
==  0){ goto _LL1022;} else{ goto _LL1023;} _LL1023: if(( unsigned int)
_temp1011 >  4u?*(( int*) _temp1011) ==  Cyc_Absyn_PointerType: 0){ goto _LL1046;}
else{ goto _LL1025;} _LL1046: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof(
unsigned char), 5u)) ==  0){ goto _LL1024;} else{ goto _LL1025;} _LL1025: goto
_LL1026; _LL1014: if( _temp1032 ==  0){ return({ void* _temp1047[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved struct type (COMPILER ERROR)", sizeof(
unsigned char), 40u), _tag_arr( _temp1047, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp1048= Cyc_Absyn_lookup_struct_field( _temp1029, f); if( _temp1048 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1051; _temp1051.tag= Cyc_Std_String_pa;
_temp1051.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1050; _temp1050.tag= Cyc_Std_String_pa; _temp1050.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1032));{ void*
_temp1049[ 2u]={& _temp1050,& _temp1051}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp1049, sizeof( void*), 2u));}}});}{
void* t2;{ struct _RegionHandle _temp1052= _new_region(); struct _RegionHandle*
rgn=& _temp1052; _push_region( rgn);{ struct Cyc_List_List* _temp1053=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r1, struct _RegionHandle* r2, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1029->tvs,
_temp1030); t2= Cyc_Tcutil_rsubstitute( rgn, _temp1053,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1048))->type);}; _pop_region( rgn);} return t2;}} _LL1016: {
struct Cyc_Absyn_Structfield* _temp1054= Cyc_Absyn_lookup_field( _temp1034, f);
if( _temp1054 ==  0){ return({ struct Cyc_Std_String_pa_struct _temp1056;
_temp1056.tag= Cyc_Std_String_pa; _temp1056.f1=( struct _tagged_arr)* f;{ void*
_temp1055[ 1u]={& _temp1056}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct has no %s field",
sizeof( unsigned char), 23u), _tag_arr( _temp1055, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1054))->type;}
_LL1018: { struct Cyc_Absyn_Structfield* _temp1057= Cyc_Absyn_lookup_field(
_temp1036, f); if( _temp1057 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1059; _temp1059.tag= Cyc_Std_String_pa; _temp1059.f1=( struct _tagged_arr)*
f;{ void* _temp1058[ 1u]={& _temp1059}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp1058, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1057))->type;}
_LL1020: if( _temp1043 ==  0){ return({ void* _temp1060[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u), _tag_arr( _temp1060, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp1061= Cyc_Absyn_lookup_union_field( _temp1040, f); if( _temp1061 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1064; _temp1064.tag= Cyc_Std_String_pa;
_temp1064.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1063; _temp1063.tag= Cyc_Std_String_pa; _temp1063.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1043));{ void*
_temp1062[ 2u]={& _temp1063,& _temp1064}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp1062, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp1065=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1040->tvs, _temp1041); void*
_temp1066= Cyc_Tcutil_substitute( _temp1065,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1061))->type); return _temp1066;}} _LL1022:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1067=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1067[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1068; _temp1068.tag= Cyc_Absyn_Primop_e;
_temp1068.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1068.f2=({ struct Cyc_List_List*
_temp1069=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1069->hd=( void*) e; _temp1069->tl= 0; _temp1069;}); _temp1068;});
_temp1067;}))); return Cyc_Absyn_uint_t; _LL1024:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1070=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1070[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1071; _temp1071.tag= Cyc_Absyn_Primop_e;
_temp1071.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1071.f2=({ struct Cyc_List_List*
_temp1072=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1072->hd=( void*) e; _temp1072->tl= 0; _temp1072;}); _temp1071;});
_temp1070;}))); return Cyc_Absyn_uint_t; _LL1026: if( Cyc_Std_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1074; _temp1074.tag= Cyc_Std_String_pa; _temp1074.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1073[ 1u]={& _temp1074}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u), _tag_arr( _temp1073, sizeof( void*), 1u));}});} else{
return({ struct Cyc_Std_String_pa_struct _temp1076; _temp1076.tag= Cyc_Std_String_pa;
_temp1076.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1075[
1u]={& _temp1076}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct or union, found %s",
sizeof( unsigned char), 36u), _tag_arr( _temp1075, sizeof( void*), 1u));}});}
_LL1012:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1077= t;
struct Cyc_Absyn_PtrInfo _temp1083; struct Cyc_Absyn_Conref* _temp1085; void*
_temp1087; void* _temp1089; _LL1079: if(( unsigned int) _temp1077 >  4u?*(( int*)
_temp1077) ==  Cyc_Absyn_PointerType: 0){ _LL1084: _temp1083=(( struct Cyc_Absyn_PointerType_struct*)
_temp1077)->f1; _LL1090: _temp1089=( void*) _temp1083.elt_typ; goto _LL1088;
_LL1088: _temp1087=( void*) _temp1083.rgn_typ; goto _LL1086; _LL1086: _temp1085=
_temp1083.bounds; goto _LL1080;} else{ goto _LL1081;} _LL1081: goto _LL1082;
_LL1080: Cyc_Tcutil_check_nonzero_bound( loc, _temp1085);{ void* _temp1091= Cyc_Tcutil_compress(
_temp1089); struct Cyc_Absyn_Structdecl** _temp1103; struct Cyc_Absyn_Structdecl*
_temp1105; struct Cyc_List_List* _temp1106; struct _tuple1* _temp1108; struct
Cyc_Absyn_Uniondecl** _temp1110; struct Cyc_Absyn_Uniondecl* _temp1112; struct
Cyc_List_List* _temp1113; struct _tuple1* _temp1115; struct Cyc_List_List*
_temp1117; struct Cyc_List_List* _temp1119; _LL1093: if(( unsigned int)
_temp1091 >  4u?*(( int*) _temp1091) ==  Cyc_Absyn_StructType: 0){ _LL1109:
_temp1108=(( struct Cyc_Absyn_StructType_struct*) _temp1091)->f1; goto _LL1107;
_LL1107: _temp1106=(( struct Cyc_Absyn_StructType_struct*) _temp1091)->f2; goto
_LL1104; _LL1104: _temp1103=(( struct Cyc_Absyn_StructType_struct*) _temp1091)->f3;
if( _temp1103 ==  0){ goto _LL1095;} else{ _temp1105=* _temp1103; goto _LL1094;}}
else{ goto _LL1095;} _LL1095: if(( unsigned int) _temp1091 >  4u?*(( int*)
_temp1091) ==  Cyc_Absyn_UnionType: 0){ _LL1116: _temp1115=(( struct Cyc_Absyn_UnionType_struct*)
_temp1091)->f1; goto _LL1114; _LL1114: _temp1113=(( struct Cyc_Absyn_UnionType_struct*)
_temp1091)->f2; goto _LL1111; _LL1111: _temp1110=(( struct Cyc_Absyn_UnionType_struct*)
_temp1091)->f3; if( _temp1110 ==  0){ goto _LL1097;} else{ _temp1112=* _temp1110;
goto _LL1096;}} else{ goto _LL1097;} _LL1097: if(( unsigned int) _temp1091 >  4u?*((
int*) _temp1091) ==  Cyc_Absyn_AnonStructType: 0){ _LL1118: _temp1117=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1091)->f1; goto _LL1098;} else{ goto
_LL1099;} _LL1099: if(( unsigned int) _temp1091 >  4u?*(( int*) _temp1091) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1120: _temp1119=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1091)->f1; goto _LL1100;} else{ goto _LL1101;} _LL1101: goto _LL1102;
_LL1094: { struct Cyc_Absyn_Structfield* _temp1121= Cyc_Absyn_lookup_struct_field(
_temp1105, f); if( _temp1121 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1124; _temp1124.tag= Cyc_Std_String_pa; _temp1124.f1=( struct _tagged_arr)*
f;{ struct Cyc_Std_String_pa_struct _temp1123; _temp1123.tag= Cyc_Std_String_pa;
_temp1123.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1108));{ void* _temp1122[ 2u]={& _temp1123,& _temp1124}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct %s has no %s field", sizeof( unsigned char), 26u),
_tag_arr( _temp1122, sizeof( void*), 2u));}}});}{ void* t3;{ struct
_RegionHandle _temp1125= _new_region(); struct _RegionHandle* rgn=& _temp1125;
_push_region( rgn);{ struct Cyc_List_List* _temp1126=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1105->tvs, _temp1106);
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1126,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1121))->type);}; _pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1087); return t3;}} _LL1096: { struct Cyc_Absyn_Structfield*
_temp1127= Cyc_Absyn_lookup_union_field( _temp1112, f); if( _temp1127 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1130; _temp1130.tag= Cyc_Std_String_pa;
_temp1130.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1129; _temp1129.tag= Cyc_Std_String_pa; _temp1129.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1115));{ void*
_temp1128[ 2u]={& _temp1129,& _temp1130}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp1128, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp1131=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1112->tvs, _temp1113); void*
_temp1132= Cyc_Tcutil_substitute( _temp1131,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1127))->type); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1087); return _temp1132;}} _LL1098: { struct Cyc_Absyn_Structfield*
_temp1133= Cyc_Absyn_lookup_field( _temp1117, f); if( _temp1133 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1135; _temp1135.tag= Cyc_Std_String_pa;
_temp1135.f1=( struct _tagged_arr)* f;{ void* _temp1134[ 1u]={& _temp1135}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1134, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1133))->type;} _LL1100: { struct Cyc_Absyn_Structfield*
_temp1136= Cyc_Absyn_lookup_field( _temp1119, f); if( _temp1136 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1138; _temp1138.tag= Cyc_Std_String_pa;
_temp1138.f1=( struct _tagged_arr)* f;{ void* _temp1137[ 1u]={& _temp1138}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union has no %s field", sizeof( unsigned char), 22u),
_tag_arr( _temp1137, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1136))->type;} _LL1102: return({ struct Cyc_Std_String_pa_struct
_temp1140; _temp1140.tag= Cyc_Std_String_pa; _temp1140.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1139[ 1u]={& _temp1140}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct pointer, found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1139, sizeof( void*), 1u));}}); _LL1092:;} _LL1082: return({
struct Cyc_Std_String_pa_struct _temp1142; _temp1142.tag= Cyc_Std_String_pa;
_temp1142.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1141[
1u]={& _temp1142}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp1141, sizeof( void*), 1u));}});
_LL1078:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1143; _push_handler(& _temp1143);{ int _temp1145= 0; if(
setjmp( _temp1143.handler)){ _temp1145= 1;} if( ! _temp1145){{ void* _temp1146=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1146;}; _pop_handler();} else{ void* _temp1144=(
void*) _exn_thrown; void* _temp1148= _temp1144; _LL1150: if( _temp1148 ==  Cyc_List_Nth){
goto _LL1151;} else{ goto _LL1152;} _LL1152: goto _LL1153; _LL1151: return({
struct Cyc_Std_Int_pa_struct _temp1156; _temp1156.tag= Cyc_Std_Int_pa; _temp1156.f1=(
int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts));{
struct Cyc_Std_Int_pa_struct _temp1155; _temp1155.tag= Cyc_Std_Int_pa; _temp1155.f1=(
int) i;{ void* _temp1154[ 2u]={& _temp1155,& _temp1156}; Cyc_Tcexp_expr_err( te,
loc, _tag_arr("index is %d but tuple has only %d fields", sizeof( unsigned char),
41u), _tag_arr( _temp1154, sizeof( void*), 2u));}}}); _LL1153:( void) _throw(
_temp1148); _LL1149:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return({
struct Cyc_Std_String_pa_struct _temp1158; _temp1158.tag= Cyc_Std_String_pa;
_temp1158.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp1157[
1u]={& _temp1158}; Cyc_Tcexp_expr_err( te, e2->loc, _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp1157, sizeof( void*), 1u));}});}{
void* _temp1159= t1; struct Cyc_Absyn_PtrInfo _temp1167; struct Cyc_Absyn_Conref*
_temp1169; struct Cyc_Absyn_Tqual _temp1171; void* _temp1173; void* _temp1175;
struct Cyc_List_List* _temp1177; _LL1161: if(( unsigned int) _temp1159 >  4u?*((
int*) _temp1159) ==  Cyc_Absyn_PointerType: 0){ _LL1168: _temp1167=(( struct Cyc_Absyn_PointerType_struct*)
_temp1159)->f1; _LL1176: _temp1175=( void*) _temp1167.elt_typ; goto _LL1174;
_LL1174: _temp1173=( void*) _temp1167.rgn_typ; goto _LL1172; _LL1172: _temp1171=
_temp1167.tq; goto _LL1170; _LL1170: _temp1169= _temp1167.bounds; goto _LL1162;}
else{ goto _LL1163;} _LL1163: if(( unsigned int) _temp1159 >  4u?*(( int*)
_temp1159) ==  Cyc_Absyn_TupleType: 0){ _LL1178: _temp1177=(( struct Cyc_Absyn_TupleType_struct*)
_temp1159)->f1; goto _LL1164;} else{ goto _LL1165;} _LL1165: goto _LL1166;
_LL1162: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1169);} else{ if( Cyc_Tcutil_is_bound_one( _temp1169)){({ void*
_temp1179[ 0u]={}; Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u), _tag_arr( _temp1179, sizeof( void*), 0u));});} Cyc_Tcutil_check_nonzero_bound(
loc, _temp1169);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1173); return
_temp1175; _LL1164: return Cyc_Tcexp_ithTupleType( te, loc, _temp1177, e2);
_LL1166: return({ struct Cyc_Std_String_pa_struct _temp1181; _temp1181.tag= Cyc_Std_String_pa;
_temp1181.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp1180[
1u]={& _temp1181}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1180, sizeof( void*), 1u));}});
_LL1160:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){ int done= 0;
struct Cyc_List_List* fields= 0; if( topt !=  0){ void* _temp1182= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_List_List* _temp1188; _LL1184: if((
unsigned int) _temp1182 >  4u?*(( int*) _temp1182) ==  Cyc_Absyn_TupleType: 0){
_LL1189: _temp1188=(( struct Cyc_Absyn_TupleType_struct*) _temp1182)->f1; goto
_LL1185;} else{ goto _LL1186;} _LL1186: goto _LL1187; _LL1185: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1188) != (( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ goto _LL1183;} for( 0; es !=  0;( es=(( struct Cyc_List_List*)
_check_null( es))->tl, _temp1188=(( struct Cyc_List_List*) _check_null(
_temp1188))->tl)){ void* _temp1190=(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp1188))->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)&
_temp1190,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
fields=({ struct Cyc_List_List* _temp1191=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1191->hd=( void*)({ struct _tuple8*
_temp1192=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1192->f1=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1188))->hd)).f1;
_temp1192->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1192;});
_temp1191->tl= fields; _temp1191;});} done= 1; goto _LL1183; _LL1187: goto
_LL1183; _LL1183:;} if( ! done){ for( 0; es !=  0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd); fields=({ struct Cyc_List_List*
_temp1193=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1193->hd=( void*)({ struct _tuple8* _temp1194=( struct _tuple8*) _cycalloc(
sizeof( struct _tuple8)); _temp1194->f1= Cyc_Absyn_empty_tqual(); _temp1194->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1194;}); _temp1193->tl=
fields; _temp1193;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1195=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1195[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1196; _temp1196.tag= Cyc_Absyn_TupleType;
_temp1196.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1196;}); _temp1195;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return({ void* _temp1197[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("tcCompoundLit", sizeof( unsigned char), 14u), _tag_arr(
_temp1197, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1220=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1220->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1220;})); struct Cyc_Absyn_Const_e_struct* _temp1198=({ struct Cyc_Absyn_Const_e_struct*
_temp1216=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1216[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1217; _temp1217.tag= Cyc_Absyn_Const_e;
_temp1217.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1218=(
struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1218[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1219; _temp1219.tag= Cyc_Absyn_Int_c;
_temp1219.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1219.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1219;}); _temp1218;}));
_temp1217;}); _temp1216;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) _temp1198, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1199=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1199->v=( void*)
Cyc_Absyn_uint_t; _temp1199;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1214=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1214[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1215; _temp1215.tag= Cyc_Absyn_ArrayType;
_temp1215.f1=( void*) res; _temp1215.f2= Cyc_Absyn_empty_tqual(); _temp1215.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1215;}); _temp1214;});{ struct Cyc_List_List*
es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){({ struct
Cyc_Std_String_pa_struct _temp1201; _temp1201.tag= Cyc_Std_String_pa; _temp1201.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( res);{ void* _temp1200[ 1u]={&
_temp1201}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc, _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp1200, sizeof( void*), 1u));}});}{
int offset= 0; for( 0; des !=  0;( offset ++, des=(( struct Cyc_List_List*)
_check_null( des))->tl)){ struct Cyc_List_List* ds=(*(( struct _tuple7*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1; if( ds !=  0){ void*
_temp1202=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_Absyn_Exp*
_temp1208; _LL1204: if(*(( int*) _temp1202) ==  Cyc_Absyn_FieldName){ goto
_LL1205;} else{ goto _LL1206;} _LL1206: if(*(( int*) _temp1202) ==  Cyc_Absyn_ArrayElement){
_LL1209: _temp1208=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1202)->f1;
goto _LL1207;} else{ goto _LL1203;} _LL1205:({ void* _temp1210[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("only array index designators are supported", sizeof(
unsigned char), 43u), _tag_arr( _temp1210, sizeof( void*), 0u));}); goto _LL1203;
_LL1207: Cyc_Tcexp_tcExpInitializer( te, 0, _temp1208);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1208); if( i !=  offset){({ struct Cyc_Std_Int_pa_struct _temp1213;
_temp1213.tag= Cyc_Std_Int_pa; _temp1213.f1=( int) i;{ struct Cyc_Std_Int_pa_struct
_temp1212; _temp1212.tag= Cyc_Std_Int_pa; _temp1212.f1=( int)(( unsigned int)
offset);{ void* _temp1211[ 2u]={& _temp1212,& _temp1213}; Cyc_Tcutil_terr(
_temp1208->loc, _tag_arr("expecting index designator %d but found %d", sizeof(
unsigned char), 43u), _tag_arr( _temp1211, sizeof( void*), 2u));}}});} goto
_LL1203;} _LL1203:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){({
struct Cyc_Std_String_pa_struct _temp1222; _temp1222.tag= Cyc_Std_String_pa;
_temp1222.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v);{ void* _temp1221[ 1u]={& _temp1222}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("expecting unsigned int, found %s", sizeof( unsigned char),
33u), _tag_arr( _temp1221, sizeof( void*), 1u));}});} if( !( vd->tq).q_const){({
void* _temp1223[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u), _tag_arr( _temp1223, sizeof( void*), 0u));});} if(
te->le !=  0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);}{ struct Cyc_Absyn_PtrInfo pinfo; void** _temp1224= 0; struct Cyc_Absyn_Tqual*
_temp1225= 0; if( topt !=  0){ void* _temp1226= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1234; struct Cyc_Absyn_Exp*
_temp1236; struct Cyc_Absyn_Tqual _temp1238; struct Cyc_Absyn_Tqual* _temp1240;
void* _temp1241; void** _temp1243; _LL1228: if(( unsigned int) _temp1226 >  4u?*((
int*) _temp1226) ==  Cyc_Absyn_PointerType: 0){ _LL1235: _temp1234=(( struct Cyc_Absyn_PointerType_struct*)
_temp1226)->f1; goto _LL1229;} else{ goto _LL1230;} _LL1230: if(( unsigned int)
_temp1226 >  4u?*(( int*) _temp1226) ==  Cyc_Absyn_ArrayType: 0){ _LL1242:
_temp1241=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1226)->f1;
_temp1243=( void**)&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1226)->f1);
goto _LL1239; _LL1239: _temp1238=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1226)->f2; _temp1240=( struct Cyc_Absyn_Tqual*)&(( struct Cyc_Absyn_ArrayType_struct*)
_temp1226)->f2; goto _LL1237; _LL1237: _temp1236=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1226)->f3; goto _LL1231;} else{ goto _LL1232;} _LL1232: goto _LL1233;
_LL1229: pinfo= _temp1234; _temp1224=( void**)(( void**)(( void*)& pinfo.elt_typ));
_temp1225=( struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1227; _LL1231: _temp1224=(
void**) _temp1243; _temp1225=( struct Cyc_Absyn_Tqual*) _temp1240; goto _LL1227;
_LL1233: goto _LL1227; _LL1227:;}{ void* t= Cyc_Tcexp_tcExp( te, _temp1224, body);
if( te->le ==  0){ if( ! Cyc_Tcutil_is_const_exp( te, bound)){({ void* _temp1244[
0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant", sizeof(
unsigned char), 22u), _tag_arr( _temp1244, sizeof( void*), 0u));});} if( ! Cyc_Tcutil_is_const_exp(
te, body)){({ void* _temp1245[ 0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("body is not constant",
sizeof( unsigned char), 21u), _tag_arr( _temp1245, sizeof( void*), 0u));});}}
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1246=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1246[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1247; _temp1247.tag= Cyc_Absyn_ArrayType;
_temp1247.f1=( void*) t; _temp1247.f2= _temp1225 ==  0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1225)); _temp1247.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1247;}); _temp1246;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt !=  0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1248; _push_handler(&
_temp1248);{ int _temp1250= 0; if( setjmp( _temp1248.handler)){ _temp1250= 1;}
if( ! _temp1250){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1249=( void*) _exn_thrown; void* _temp1252= _temp1249; _LL1254:
if( _temp1252 ==  Cyc_Dict_Absent){ goto _LL1255;} else{ goto _LL1256;} _LL1256:
goto _LL1257; _LL1255:({ struct Cyc_Std_String_pa_struct _temp1259; _temp1259.tag=
Cyc_Std_String_pa; _temp1259.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1258[ 1u]={& _temp1259}; Cyc_Tcutil_terr( loc, _tag_arr("unbound struct name %s",
sizeof( unsigned char), 23u), _tag_arr( _temp1258, sizeof( void*), 1u));}});
return topt !=  0?*(( void**) _check_null( topt)):( void*) Cyc_Absyn_VoidType;
_LL1257:( void) _throw( _temp1252); _LL1253:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*)
sd; if( sd->name !=  0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v;}}{ struct _RegionHandle _temp1260= _new_region();
struct _RegionHandle* rgn=& _temp1260; _push_region( rgn);{ struct _tuple4
_temp1261=({ struct _tuple4 _temp1285; _temp1285.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1285.f2= rgn; _temp1285;}); struct Cyc_List_List* _temp1262=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1261, sd->tvs); struct Cyc_List_List*
_temp1263=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1262); struct
Cyc_Absyn_StructType_struct* _temp1264=({ struct Cyc_Absyn_StructType_struct*
_temp1282=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp1282[ 0]=({ struct Cyc_Absyn_StructType_struct _temp1283; _temp1283.tag=
Cyc_Absyn_StructType; _temp1283.f1=( struct _tuple1*)* tn; _temp1283.f2=
_temp1263; _temp1283.f3=({ struct Cyc_Absyn_Structdecl** _temp1284=( struct Cyc_Absyn_Structdecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1284[ 0]= sd; _temp1284;});
_temp1283;}); _temp1282;}); if( topt !=  0){ Cyc_Tcutil_unify(( void*) _temp1264,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1265=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1265->v=( void*)
_temp1263; _temp1265;}); if( sd->fields ==  0){({ void* _temp1266[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("can't build abstract struct", sizeof( unsigned char), 28u),
_tag_arr( _temp1266, sizeof( void*), 0u));});{ void* _temp1267=( void*)
_temp1264; _npop_handler( 0u); return _temp1267;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields !=  0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1270; struct Cyc_Absyn_Exp* _temp1271; struct Cyc_Absyn_Structfield*
_temp1273; struct _tuple9* _temp1268=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1270=* _temp1268; _LL1274: _temp1273= _temp1270.f1;
goto _LL1272; _LL1272: _temp1271= _temp1270.f2; goto _LL1269; _LL1269: { void*
_temp1275= Cyc_Tcutil_rsubstitute( rgn, _temp1262,( void*) _temp1273->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1275, _temp1271); if( ! Cyc_Tcutil_coerce_arg( te, _temp1271,
_temp1275)){({ struct Cyc_Std_String_pa_struct _temp1280; _temp1280.tag= Cyc_Std_String_pa;
_temp1280.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1271->topt))->v);{ struct Cyc_Std_String_pa_struct _temp1279;
_temp1279.tag= Cyc_Std_String_pa; _temp1279.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1275);{ struct Cyc_Std_String_pa_struct _temp1278; _temp1278.tag= Cyc_Std_String_pa;
_temp1278.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(* tn);{ struct Cyc_Std_String_pa_struct
_temp1277; _temp1277.tag= Cyc_Std_String_pa; _temp1277.f1=( struct _tagged_arr)*
_temp1273->name;{ void* _temp1276[ 4u]={& _temp1277,& _temp1278,& _temp1279,&
_temp1280}; Cyc_Tcutil_terr( _temp1271->loc, _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1276, sizeof( void*), 4u));}}}}});
Cyc_Tcutil_explain_failure();}}}{ void* _temp1281=( void*) _temp1264;
_npop_handler( 0u); return _temp1281;}}}; _pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* ts, struct
Cyc_List_List* args){{ struct _RegionHandle _temp1286= _new_region(); struct
_RegionHandle* rgn=& _temp1286; _push_region( rgn);{ void* _temp1287= Cyc_Tcutil_compress(
ts); struct Cyc_List_List* _temp1293; _LL1289: if(( unsigned int) _temp1287 >  4u?*((
int*) _temp1287) ==  Cyc_Absyn_AnonStructType: 0){ _LL1294: _temp1293=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1287)->f1; goto _LL1290;} else{ goto
_LL1291;} _LL1291: goto _LL1292; _LL1290: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1293); for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1297; struct Cyc_Absyn_Exp*
_temp1298; struct Cyc_Absyn_Structfield* _temp1300; struct _tuple9* _temp1295=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1297=*
_temp1295; _LL1301: _temp1300= _temp1297.f1; goto _LL1299; _LL1299: _temp1298=
_temp1297.f2; goto _LL1296; _LL1296: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1300->type)), _temp1298); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1298,( void*) _temp1300->type)){({ struct Cyc_Std_String_pa_struct
_temp1305; _temp1305.tag= Cyc_Std_String_pa; _temp1305.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1298->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1304; _temp1304.tag= Cyc_Std_String_pa;
_temp1304.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*) _temp1300->type);{
struct Cyc_Std_String_pa_struct _temp1303; _temp1303.tag= Cyc_Std_String_pa;
_temp1303.f1=( struct _tagged_arr)* _temp1300->name;{ void* _temp1302[ 3u]={&
_temp1303,& _temp1304,& _temp1305}; Cyc_Tcutil_terr( _temp1298->loc, _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1302, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} goto _LL1288;} _LL1292:({ void* _temp1306[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("tcAnonStruct:  not an AnonStructType", sizeof( unsigned char), 37u),
_tag_arr( _temp1306, sizeof( void*), 0u));}); goto _LL1288; _LL1288:;};
_pop_region( rgn);} return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List*
es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield* tuf){ struct
_RegionHandle _temp1307= _new_region(); struct _RegionHandle* rgn=& _temp1307;
_push_region( rgn);{ struct _tuple4 _temp1308=({ struct _tuple4 _temp1354;
_temp1354.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1354.f2= rgn; _temp1354;});
struct Cyc_List_List* _temp1309=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1308, tud->tvs); struct Cyc_List_List* _temp1310=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1308, tuf->tvs); struct Cyc_List_List* _temp1311=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1309); struct Cyc_List_List* _temp1312=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1310); struct Cyc_List_List*
_temp1313=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1309, _temp1310);*
all_ref=({ struct Cyc_Core_Opt* _temp1314=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1314->v=( void*) _temp1311; _temp1314;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1315=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1315->v=( void*) _temp1312; _temp1315;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1349=( struct
Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1349[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1350; _temp1350.tag=
Cyc_Absyn_TunionFieldType; _temp1350.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1351; _temp1351.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1352=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1352[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1353; _temp1353.tag= Cyc_Absyn_KnownTunionfield; _temp1353.f1= tud;
_temp1353.f2= tuf; _temp1353;}); _temp1352;})); _temp1351.targs= _temp1311;
_temp1351;}); _temp1350;}); _temp1349;}); if( topt !=  0){ void* _temp1316= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_TunionInfo _temp1324; void*
_temp1326; struct Cyc_List_List* _temp1328; _LL1318: if(( unsigned int)
_temp1316 >  4u?*(( int*) _temp1316) ==  Cyc_Absyn_TunionFieldType: 0){ goto
_LL1319;} else{ goto _LL1320;} _LL1320: if(( unsigned int) _temp1316 >  4u?*((
int*) _temp1316) ==  Cyc_Absyn_TunionType: 0){ _LL1325: _temp1324=(( struct Cyc_Absyn_TunionType_struct*)
_temp1316)->f1; _LL1329: _temp1328= _temp1324.targs; goto _LL1327; _LL1327:
_temp1326=( void*) _temp1324.rgn; goto _LL1321;} else{ goto _LL1322;} _LL1322:
goto _LL1323; _LL1319: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res);
goto _LL1317; _LL1321:{ struct Cyc_List_List* a= _temp1311; for( 0; a !=  0?
_temp1328 !=  0: 0;( a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1328=((
struct Cyc_List_List*) _check_null( _temp1328))->tl)){ Cyc_Tcutil_unify(( void*)((
struct Cyc_List_List*) _check_null( a))->hd,( void*)(( struct Cyc_List_List*)
_check_null( _temp1328))->hd);}} if( tuf->typs ==  0? es ==  0: 0){ e->topt=({
struct Cyc_Core_Opt* _temp1330=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1330->v=( void*) res; _temp1330;}); res=( void*)({ struct
Cyc_Absyn_TunionType_struct* _temp1331=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1331[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1332; _temp1332.tag= Cyc_Absyn_TunionType;
_temp1332.f1=({ struct Cyc_Absyn_TunionInfo _temp1333; _temp1333.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp1334=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp1334[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp1335; _temp1335.tag= Cyc_Absyn_KnownTunion;
_temp1335.f1=({ struct Cyc_Absyn_Tuniondecl** _temp1336=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp1336[ 0]= tud; _temp1336;});
_temp1335;}); _temp1334;})); _temp1333.targs= _temp1311; _temp1333.rgn=( void*)
_temp1326; _temp1333;}); _temp1332;}); _temp1331;}); Cyc_Tcutil_unchecked_cast(
te, e, res);{ void* _temp1337= res; _npop_handler( 0u); return _temp1337;}} goto
_LL1317; _LL1323: goto _LL1317; _LL1317:;}{ struct Cyc_List_List* ts= tuf->typs;
for( 0; es !=  0? ts !=  0: 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl,
ts=(( struct Cyc_List_List*) _check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void* t=
Cyc_Tcutil_rsubstitute( rgn, _temp1313,(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( ts))->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct Cyc_Std_String_pa_struct
_temp1341; _temp1341.tag= Cyc_Std_String_pa; _temp1341.f1=( struct _tagged_arr)(
e->topt ==  0? _tag_arr("?", sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ struct Cyc_Std_String_pa_struct
_temp1340; _temp1340.tag= Cyc_Std_String_pa; _temp1340.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp1339;
_temp1339.tag= Cyc_Std_String_pa; _temp1339.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1338[ 3u]={& _temp1339,& _temp1340,& _temp1341}; Cyc_Tcutil_terr(
e->loc, _tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u), _tag_arr( _temp1338, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} if( es !=  0){ void* _temp1344=({ struct Cyc_Std_String_pa_struct
_temp1343; _temp1343.tag= Cyc_Std_String_pa; _temp1343.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1342[ 1u]={& _temp1343}; Cyc_Tcexp_expr_err(
te,(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,
_tag_arr("too many arguments for tunion constructor %s", sizeof( unsigned char),
45u), _tag_arr( _temp1342, sizeof( void*), 1u));}}); _npop_handler( 0u); return
_temp1344;} if( ts !=  0){ void* _temp1347=({ struct Cyc_Std_String_pa_struct
_temp1346; _temp1346.tag= Cyc_Std_String_pa; _temp1346.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1345[ 1u]={& _temp1346}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("too few arguments for tunion constructor %s", sizeof(
unsigned char), 44u), _tag_arr( _temp1345, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1347;}{ void* _temp1348= res; _npop_handler( 0u);
return _temp1348;}}}}; _pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 
0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1365=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1365[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1366; _temp1366.tag= Cyc_Absyn_RgnHandleType; _temp1366.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1367=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1367->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1367;})); _temp1366;}); _temp1365;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1355= Cyc_Tcutil_compress( handle_type); void*
_temp1361; _LL1357: if(( unsigned int) _temp1355 >  4u?*(( int*) _temp1355) == 
Cyc_Absyn_RgnHandleType: 0){ _LL1362: _temp1361=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1355)->f1; goto _LL1358;} else{ goto _LL1359;} _LL1359: goto _LL1360;
_LL1358: rgn= _temp1361; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1356; _LL1360:({ struct Cyc_Std_String_pa_struct _temp1364; _temp1364.tag=
Cyc_Std_String_pa; _temp1364.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1363[ 1u]={& _temp1364}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc, _tag_arr("expecting region_t type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp1363, sizeof( void*), 1u));}});
goto _LL1356; _LL1356:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, 1, t);{ void* _temp1368= t; struct Cyc_Absyn_TunionFieldInfo
_temp1374; void* _temp1376; struct Cyc_Absyn_Tunionfield* _temp1378; _LL1370:
if(( unsigned int) _temp1368 >  4u?*(( int*) _temp1368) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1375: _temp1374=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1368)->f1;
_LL1377: _temp1376=( void*) _temp1374.field_info; if(*(( int*) _temp1376) == 
Cyc_Absyn_KnownTunionfield){ _LL1379: _temp1378=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1376)->f2; goto _LL1371;} else{ goto _LL1372;}} else{ goto _LL1372;}
_LL1372: goto _LL1373; _LL1371: if( _temp1378->tvs !=  0){({ void* _temp1380[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u), _tag_arr( _temp1380, sizeof( void*), 0u));});}
goto _LL1369; _LL1373: goto _LL1369; _LL1369:;}{ void*(* _temp1381)( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt !=  0){ void*
_temp1382= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1388; struct Cyc_Absyn_Conref* _temp1390; _LL1384: if(( unsigned int)
_temp1382 >  4u?*(( int*) _temp1382) ==  Cyc_Absyn_PointerType: 0){ _LL1389:
_temp1388=(( struct Cyc_Absyn_PointerType_struct*) _temp1382)->f1; _LL1391:
_temp1390= _temp1388.nullable; goto _LL1385;} else{ goto _LL1386;} _LL1386: goto
_LL1387; _LL1385: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1390)){ _temp1381= Cyc_Absyn_star_typ;} goto _LL1383; _LL1387: goto _LL1383;
_LL1383:;} return _temp1381( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1392=( void*) s->r; struct Cyc_Absyn_Exp* _temp1402;
struct Cyc_Absyn_Stmt* _temp1404; struct Cyc_Absyn_Stmt* _temp1406; struct Cyc_Absyn_Stmt*
_temp1408; struct Cyc_Absyn_Decl* _temp1410; _LL1394: if(( unsigned int)
_temp1392 >  1u?*(( int*) _temp1392) ==  Cyc_Absyn_Exp_s: 0){ _LL1403: _temp1402=((
struct Cyc_Absyn_Exp_s_struct*) _temp1392)->f1; goto _LL1395;} else{ goto
_LL1396;} _LL1396: if(( unsigned int) _temp1392 >  1u?*(( int*) _temp1392) == 
Cyc_Absyn_Seq_s: 0){ _LL1407: _temp1406=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1392)->f1; goto _LL1405; _LL1405: _temp1404=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1392)->f2; goto _LL1397;} else{ goto _LL1398;} _LL1398: if(( unsigned int)
_temp1392 >  1u?*(( int*) _temp1392) ==  Cyc_Absyn_Decl_s: 0){ _LL1411:
_temp1410=(( struct Cyc_Absyn_Decl_s_struct*) _temp1392)->f1; goto _LL1409;
_LL1409: _temp1408=(( struct Cyc_Absyn_Decl_s_struct*) _temp1392)->f2; goto
_LL1399;} else{ goto _LL1400;} _LL1400: goto _LL1401; _LL1395: return( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1402->topt))->v; _LL1397: s= _temp1404;
continue; _LL1399: s= _temp1408; continue; _LL1401: return({ void* _temp1412[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u), _tag_arr( _temp1412, sizeof( void*), 0u));});
_LL1393:;}} static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return({
void* _temp1413[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u), _tag_arr( _temp1413, sizeof( void*), 0u));});}
static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ return({ void* _temp1414[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u),
_tag_arr( _temp1414, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){
void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle !=  0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1425=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1425[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1426; _temp1426.tag= Cyc_Absyn_RgnHandleType;
_temp1426.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp1427=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1427->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1427;}));
_temp1426;}); _temp1425;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle)); void*
_temp1415= Cyc_Tcutil_compress( handle_type); void* _temp1421; _LL1417: if((
unsigned int) _temp1415 >  4u?*(( int*) _temp1415) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1422: _temp1421=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1415)->f1; goto _LL1418;} else{ goto _LL1419;} _LL1419: goto _LL1420;
_LL1418: rgn= _temp1421; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1416; _LL1420:({ struct Cyc_Std_String_pa_struct _temp1424; _temp1424.tag=
Cyc_Std_String_pa; _temp1424.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1423[ 1u]={& _temp1424}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle))->loc, _tag_arr("expecting region_t type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp1423, sizeof( void*), 1u));}});
goto _LL1416; _LL1416:;}{ void* _temp1428=( void*) e1->r; struct Cyc_Absyn_Exp*
_temp1440; struct Cyc_Absyn_Exp* _temp1442; struct Cyc_Absyn_Vardecl* _temp1444;
struct Cyc_List_List* _temp1446; struct Cyc_Core_Opt* _temp1448; struct Cyc_List_List*
_temp1450; void* _temp1452; struct _tagged_arr _temp1454; _LL1430: if(*(( int*)
_temp1428) ==  Cyc_Absyn_Comprehension_e){ _LL1445: _temp1444=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1428)->f1; goto _LL1443; _LL1443: _temp1442=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1428)->f2; goto _LL1441; _LL1441: _temp1440=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1428)->f3; goto _LL1431;} else{ goto _LL1432;} _LL1432: if(*(( int*)
_temp1428) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1449: _temp1448=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1428)->f1; goto _LL1447; _LL1447: _temp1446=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1428)->f2; goto _LL1433;} else{ goto _LL1434;} _LL1434: if(*(( int*)
_temp1428) ==  Cyc_Absyn_Array_e){ _LL1451: _temp1450=(( struct Cyc_Absyn_Array_e_struct*)
_temp1428)->f1; goto _LL1435;} else{ goto _LL1436;} _LL1436: if(*(( int*)
_temp1428) ==  Cyc_Absyn_Const_e){ _LL1453: _temp1452=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1428)->f1; if(( unsigned int) _temp1452 >  1u?*(( int*) _temp1452) ==  Cyc_Absyn_String_c:
0){ _LL1455: _temp1454=(( struct Cyc_Absyn_String_c_struct*) _temp1452)->f1;
goto _LL1437;} else{ goto _LL1438;}} else{ goto _LL1438;} _LL1438: goto _LL1439;
_LL1431: { void* _temp1456= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1457= Cyc_Tcutil_compress( _temp1456); struct Cyc_Absyn_Tqual _temp1463;
void* _temp1465; _LL1459: if(( unsigned int) _temp1457 >  4u?*(( int*) _temp1457)
==  Cyc_Absyn_ArrayType: 0){ _LL1466: _temp1465=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1457)->f1; goto _LL1464; _LL1464: _temp1463=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1457)->f2; goto _LL1460;} else{ goto _LL1461;} _LL1461: goto _LL1462;
_LL1460: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1442)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1471=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1471[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1472; _temp1472.tag= Cyc_Absyn_Upper_b; _temp1472.f1=
_temp1442; _temp1472;}); _temp1471;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1468=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1468[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1469; _temp1469.tag= Cyc_Absyn_PointerType;
_temp1469.f1=({ struct Cyc_Absyn_PtrInfo _temp1470; _temp1470.elt_typ=( void*)
_temp1465; _temp1470.rgn_typ=( void*) rgn; _temp1470.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1470.tq= _temp1463; _temp1470.bounds= Cyc_Absyn_new_conref(
b); _temp1470;}); _temp1469;}); _temp1468;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1467=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1467->v=( void*) res_typ; _temp1467;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return
res_typ;} _LL1462: return({ void* _temp1473[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u), _tag_arr( _temp1473, sizeof( void*), 0u));});
_LL1458:;} _LL1433:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1474=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1474[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1475; _temp1475.tag= Cyc_Absyn_Array_e;
_temp1475.f1= _temp1446; _temp1475;}); _temp1474;}))); _temp1450= _temp1446;
goto _LL1435; _LL1435: { void** elt_typ_opt= 0; if( topt !=  0){ void* _temp1476=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1482; struct Cyc_Absyn_Tqual _temp1484; void* _temp1486; void** _temp1488;
_LL1478: if(( unsigned int) _temp1476 >  4u?*(( int*) _temp1476) ==  Cyc_Absyn_PointerType:
0){ _LL1483: _temp1482=(( struct Cyc_Absyn_PointerType_struct*) _temp1476)->f1;
_LL1487: _temp1486=( void*) _temp1482.elt_typ; _temp1488=( void**)&((( struct
Cyc_Absyn_PointerType_struct*) _temp1476)->f1).elt_typ; goto _LL1485; _LL1485:
_temp1484= _temp1482.tq; goto _LL1479;} else{ goto _LL1480;} _LL1480: goto
_LL1481; _LL1479: elt_typ_opt=( void**) _temp1488; goto _LL1477; _LL1481: goto
_LL1477; _LL1477:;}{ void* _temp1489= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt,
e1); void* res_typ;{ void* _temp1490= Cyc_Tcutil_compress( _temp1489); struct
Cyc_Absyn_Exp* _temp1496; struct Cyc_Absyn_Tqual _temp1498; void* _temp1500;
_LL1492: if(( unsigned int) _temp1490 >  4u?*(( int*) _temp1490) ==  Cyc_Absyn_ArrayType:
0){ _LL1501: _temp1500=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1490)->f1;
goto _LL1499; _LL1499: _temp1498=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1490)->f2; goto _LL1497; _LL1497: _temp1496=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1490)->f3; goto _LL1493;} else{ goto _LL1494;} _LL1494: goto _LL1495;
_LL1493: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1502=(
struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1502[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1503; _temp1503.tag=
Cyc_Absyn_PointerType; _temp1503.f1=({ struct Cyc_Absyn_PtrInfo _temp1504;
_temp1504.elt_typ=( void*) _temp1500; _temp1504.rgn_typ=( void*) rgn; _temp1504.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1504.tq=
_temp1498; _temp1504.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1505=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1505[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1506; _temp1506.tag= Cyc_Absyn_Upper_b;
_temp1506.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1496); _temp1506;});
_temp1505;})); _temp1504;}); _temp1503;}); _temp1502;}); if( topt !=  0){ if( !
Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1507=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1507->v=( void*) res_typ; _temp1507;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1491; _LL1495: return({ void* _temp1508[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u), _tag_arr( _temp1508, sizeof( void*), 0u));});
_LL1491:;} return res_typ;}} _LL1437: { void* _temp1509= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b); void* _temp1510= Cyc_Tcexp_tcExp(
te,( void**)& _temp1509, e1); return Cyc_Absyn_atb_typ( _temp1510, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1511=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1511[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1512; _temp1512.tag= Cyc_Absyn_Upper_b; _temp1512.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1512;}); _temp1511;}));} _LL1439: { void** topt2=
0; if( topt !=  0){ void* _temp1513= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1521; struct Cyc_Absyn_Conref* _temp1523;
void* _temp1525; void** _temp1527; _LL1515: if(( unsigned int) _temp1513 >  4u?*((
int*) _temp1513) ==  Cyc_Absyn_PointerType: 0){ _LL1522: _temp1521=(( struct Cyc_Absyn_PointerType_struct*)
_temp1513)->f1; _LL1526: _temp1525=( void*) _temp1521.elt_typ; _temp1527=( void**)&(((
struct Cyc_Absyn_PointerType_struct*) _temp1513)->f1).elt_typ; goto _LL1524;
_LL1524: _temp1523= _temp1521.nullable; goto _LL1516;} else{ goto _LL1517;}
_LL1517: if(( unsigned int) _temp1513 >  4u?*(( int*) _temp1513) ==  Cyc_Absyn_TunionType:
0){ goto _LL1518;} else{ goto _LL1519;} _LL1519: goto _LL1520; _LL1516: topt2=(
void**) _temp1527; goto _LL1514; _LL1518: topt2=({ void** _temp1528=( void**)
_cycalloc( sizeof( void*)); _temp1528[ 0]=*(( void**) _check_null( topt));
_temp1528;}); goto _LL1514; _LL1520: goto _LL1514; _LL1514:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1530=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1530[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1531; _temp1531.tag=
Cyc_Absyn_PointerType; _temp1531.f1=({ struct Cyc_Absyn_PtrInfo _temp1532;
_temp1532.elt_typ=( void*) telt; _temp1532.rgn_typ=( void*) rgn; _temp1532.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1532.tq= Cyc_Absyn_empty_tqual();
_temp1532.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1533=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1533[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1534; _temp1534.tag= Cyc_Absyn_Upper_b;
_temp1534.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1534;}); _temp1533;})); _temp1532;});
_temp1531;}); _temp1530;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1529=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1529->v=(
void*) res_typ; _temp1529;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}}
_LL1429:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress( Cyc_Tcexp_tcExpNoPromote(
te, topt, e)); void* _temp1535= t; struct Cyc_Absyn_Exp* _temp1541; struct Cyc_Absyn_Tqual
_temp1543; void* _temp1545; _LL1537: if(( unsigned int) _temp1535 >  4u?*(( int*)
_temp1535) ==  Cyc_Absyn_ArrayType: 0){ _LL1546: _temp1545=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1535)->f1; goto _LL1544; _LL1544: _temp1543=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1535)->f2; goto _LL1542; _LL1542: _temp1541=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1535)->f3; goto _LL1538;} else{ goto _LL1539;} _LL1539: goto _LL1540;
_LL1538: { void* _temp1549; struct _tuple6 _temp1547= Cyc_Tcutil_addressof_props(
te, e); _LL1550: _temp1549= _temp1547.f2; goto _LL1548; _LL1548: { void*
_temp1551= _temp1541 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1552=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1552[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1553; _temp1553.tag= Cyc_Absyn_Upper_b;
_temp1553.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1541); _temp1553;});
_temp1552;}); t= Cyc_Absyn_atb_typ( _temp1545, _temp1549, _temp1543, _temp1551);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1540: return t; _LL1536:;} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1554=( void*) e->r; void* _temp1564; _LL1556: if(*(( int*)
_temp1554) ==  Cyc_Absyn_Array_e){ goto _LL1557;} else{ goto _LL1558;} _LL1558:
if(*(( int*) _temp1554) ==  Cyc_Absyn_Comprehension_e){ goto _LL1559;} else{
goto _LL1560;} _LL1560: if(*(( int*) _temp1554) ==  Cyc_Absyn_Const_e){ _LL1565:
_temp1564=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1554)->f1; if((
unsigned int) _temp1564 >  1u?*(( int*) _temp1564) ==  Cyc_Absyn_String_c: 0){
goto _LL1561;} else{ goto _LL1562;}} else{ goto _LL1562;} _LL1562: goto _LL1563;
_LL1557: return t; _LL1559: return t; _LL1561: return t; _LL1563: t= Cyc_Tcutil_compress(
t);{ void* _temp1566= t; struct Cyc_Absyn_Exp* _temp1572; struct Cyc_Absyn_Tqual
_temp1574; void* _temp1576; _LL1568: if(( unsigned int) _temp1566 >  4u?*(( int*)
_temp1566) ==  Cyc_Absyn_ArrayType: 0){ _LL1577: _temp1576=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1566)->f1; goto _LL1575; _LL1575: _temp1574=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1566)->f2; goto _LL1573; _LL1573: _temp1572=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1566)->f3; goto _LL1569;} else{ goto _LL1570;} _LL1570: goto _LL1571;
_LL1569: { void* _temp1580; struct _tuple6 _temp1578= Cyc_Tcutil_addressof_props(
te, e); _LL1581: _temp1580= _temp1578.f2; goto _LL1579; _LL1579: { void* b=
_temp1572 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1582=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1582[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1583; _temp1583.tag= Cyc_Absyn_Upper_b;
_temp1583.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1572); _temp1583;});
_temp1582;}); t= Cyc_Absyn_atb_typ( _temp1576, _temp1580, _temp1574, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1571: return t; _LL1567:;} _LL1555:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1584=( void*) e->r; struct Cyc_Absyn_Exp* _temp1590; _LL1586: if(*(( int*)
_temp1584) ==  Cyc_Absyn_NoInstantiate_e){ _LL1591: _temp1590=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1584)->f1; goto _LL1587;} else{ goto _LL1588;} _LL1588: goto _LL1589;
_LL1587: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1590);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1590->topt))->v=( void*) Cyc_Absyn_pointer_expand(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1590->topt))->v)); e->topt= _temp1590->topt;
goto _LL1585; _LL1589: Cyc_Tcexp_tcExpNoInst( te, topt, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1592=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct Cyc_Absyn_PtrInfo _temp1598; struct Cyc_Absyn_Conref* _temp1600; struct
Cyc_Absyn_Tqual _temp1602; struct Cyc_Absyn_Conref* _temp1604; void* _temp1606;
void* _temp1608; _LL1594: if(( unsigned int) _temp1592 >  4u?*(( int*) _temp1592)
==  Cyc_Absyn_PointerType: 0){ _LL1599: _temp1598=(( struct Cyc_Absyn_PointerType_struct*)
_temp1592)->f1; _LL1609: _temp1608=( void*) _temp1598.elt_typ; goto _LL1607;
_LL1607: _temp1606=( void*) _temp1598.rgn_typ; goto _LL1605; _LL1605: _temp1604=
_temp1598.nullable; goto _LL1603; _LL1603: _temp1602= _temp1598.tq; goto _LL1601;
_LL1601: _temp1600= _temp1598.bounds; goto _LL1595;} else{ goto _LL1596;}
_LL1596: goto _LL1597; _LL1595:{ void* _temp1610= Cyc_Tcutil_compress( _temp1608);
struct Cyc_Absyn_FnInfo _temp1616; struct Cyc_List_List* _temp1618; struct Cyc_List_List*
_temp1620; struct Cyc_Absyn_VarargInfo* _temp1622; int _temp1624; struct Cyc_List_List*
_temp1626; void* _temp1628; struct Cyc_Core_Opt* _temp1630; struct Cyc_List_List*
_temp1632; _LL1612: if(( unsigned int) _temp1610 >  4u?*(( int*) _temp1610) == 
Cyc_Absyn_FnType: 0){ _LL1617: _temp1616=(( struct Cyc_Absyn_FnType_struct*)
_temp1610)->f1; _LL1633: _temp1632= _temp1616.tvars; goto _LL1631; _LL1631:
_temp1630= _temp1616.effect; goto _LL1629; _LL1629: _temp1628=( void*) _temp1616.ret_typ;
goto _LL1627; _LL1627: _temp1626= _temp1616.args; goto _LL1625; _LL1625:
_temp1624= _temp1616.c_varargs; goto _LL1623; _LL1623: _temp1622= _temp1616.cyc_varargs;
goto _LL1621; _LL1621: _temp1620= _temp1616.rgn_po; goto _LL1619; _LL1619:
_temp1618= _temp1616.attributes; goto _LL1613;} else{ goto _LL1614;} _LL1614:
goto _LL1615; _LL1613: if( _temp1632 !=  0){ struct _RegionHandle _temp1634=
_new_region(); struct _RegionHandle* rgn=& _temp1634; _push_region( rgn);{
struct _tuple4 _temp1635=({ struct _tuple4 _temp1645; _temp1645.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1645.f2= rgn; _temp1645;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1635, _temp1632); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1642=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1642[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1643; _temp1643.tag= Cyc_Absyn_FnType; _temp1643.f1=({ struct Cyc_Absyn_FnInfo
_temp1644; _temp1644.tvars= 0; _temp1644.effect= _temp1630; _temp1644.ret_typ=(
void*) _temp1628; _temp1644.args= _temp1626; _temp1644.c_varargs= _temp1624;
_temp1644.cyc_varargs= _temp1622; _temp1644.rgn_po= _temp1620; _temp1644.attributes=
_temp1618; _temp1644;}); _temp1643;}); _temp1642;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1639=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1639[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1640; _temp1640.tag= Cyc_Absyn_PointerType;
_temp1640.f1=({ struct Cyc_Absyn_PtrInfo _temp1641; _temp1641.elt_typ=( void*)
ftyp; _temp1641.rgn_typ=( void*) _temp1606; _temp1641.nullable= _temp1604;
_temp1641.tq= _temp1602; _temp1641.bounds= _temp1600; _temp1641;}); _temp1640;});
_temp1639;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1636=( struct Cyc_Absyn_Instantiate_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1636[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1637; _temp1637.tag= Cyc_Absyn_Instantiate_e;
_temp1637.f1= inner; _temp1637.f2= ts; _temp1637;}); _temp1636;}))); e->topt=({
struct Cyc_Core_Opt* _temp1638=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1638->v=( void*) new_typ; _temp1638;});}; _pop_region( rgn);}
goto _LL1611; _LL1615: goto _LL1611; _LL1611:;} goto _LL1593; _LL1597: goto
_LL1593; _LL1593:;} goto _LL1585; _LL1585:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1646=( void*) e->r; struct Cyc_Absyn_Exp* _temp1722; struct
_tuple1* _temp1724; struct Cyc_List_List* _temp1726; struct Cyc_Absyn_Exp*
_temp1728; struct Cyc_List_List* _temp1730; struct Cyc_Core_Opt* _temp1732; void*
_temp1734; void* _temp1736; struct _tuple1* _temp1738; struct Cyc_List_List*
_temp1740; void* _temp1742; void* _temp1744; struct Cyc_Absyn_Exp* _temp1746;
struct Cyc_Absyn_Exp* _temp1748; struct Cyc_Core_Opt* _temp1750; struct Cyc_Absyn_Exp*
_temp1752; struct Cyc_Absyn_Exp* _temp1754; struct Cyc_Absyn_Exp* _temp1756;
struct Cyc_Absyn_Exp* _temp1758; struct Cyc_Absyn_Exp* _temp1760; struct Cyc_Absyn_Exp*
_temp1762; struct Cyc_Absyn_VarargCallInfo* _temp1764; struct Cyc_Absyn_VarargCallInfo**
_temp1766; struct Cyc_List_List* _temp1767; struct Cyc_Absyn_Exp* _temp1769;
struct Cyc_Absyn_Exp* _temp1771; struct Cyc_List_List* _temp1773; struct Cyc_Absyn_Exp*
_temp1775; struct Cyc_Absyn_Exp* _temp1777; void* _temp1779; struct Cyc_Absyn_Exp*
_temp1781; struct Cyc_Absyn_Exp* _temp1783; struct Cyc_Absyn_Exp* _temp1785;
struct Cyc_Absyn_Exp* _temp1787; void* _temp1789; void* _temp1791; void*
_temp1793; struct Cyc_Absyn_Exp* _temp1795; struct _tagged_arr* _temp1797;
struct Cyc_Absyn_Exp* _temp1799; struct _tagged_arr* _temp1801; struct Cyc_Absyn_Exp*
_temp1803; struct Cyc_Absyn_Exp* _temp1805; struct Cyc_Absyn_Exp* _temp1807;
struct Cyc_List_List* _temp1809; struct Cyc_List_List* _temp1811; struct _tuple2*
_temp1813; struct Cyc_List_List* _temp1815; struct Cyc_Absyn_Stmt* _temp1817;
struct Cyc_Absyn_Fndecl* _temp1819; struct Cyc_Absyn_Exp* _temp1821; struct Cyc_Absyn_Exp*
_temp1823; struct Cyc_Absyn_Exp* _temp1825; struct Cyc_Absyn_Vardecl* _temp1827;
struct Cyc_Absyn_Structdecl* _temp1829; struct Cyc_Absyn_Structdecl** _temp1831;
struct Cyc_List_List* _temp1832; struct Cyc_Core_Opt* _temp1834; struct Cyc_Core_Opt**
_temp1836; struct _tuple1* _temp1837; struct _tuple1** _temp1839; struct Cyc_List_List*
_temp1840; void* _temp1842; struct Cyc_Absyn_Tunionfield* _temp1844; struct Cyc_Absyn_Tuniondecl*
_temp1846; struct Cyc_List_List* _temp1848; struct Cyc_Core_Opt* _temp1850;
struct Cyc_Core_Opt** _temp1852; struct Cyc_Core_Opt* _temp1853; struct Cyc_Core_Opt**
_temp1855; struct Cyc_Absyn_Enumfield* _temp1856; struct Cyc_Absyn_Enumdecl*
_temp1858; struct _tuple1* _temp1860; struct _tuple1** _temp1862; void*
_temp1863; struct Cyc_Absyn_Exp* _temp1865; _LL1648: if(*(( int*) _temp1646) == 
Cyc_Absyn_NoInstantiate_e){ _LL1723: _temp1722=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1646)->f1; goto _LL1649;} else{ goto _LL1650;} _LL1650: if(*(( int*)
_temp1646) ==  Cyc_Absyn_UnknownId_e){ _LL1725: _temp1724=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1646)->f1; goto _LL1651;} else{ goto _LL1652;} _LL1652: if(*(( int*)
_temp1646) ==  Cyc_Absyn_UnknownCall_e){ _LL1729: _temp1728=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1646)->f1; goto _LL1727; _LL1727: _temp1726=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1646)->f2; goto _LL1653;} else{ goto _LL1654;} _LL1654: if(*(( int*)
_temp1646) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1733: _temp1732=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1646)->f1; goto _LL1731; _LL1731: _temp1730=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1646)->f2; goto _LL1655;} else{ goto _LL1656;} _LL1656: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Const_e){ _LL1735: _temp1734=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1646)->f1; goto _LL1657;} else{ goto _LL1658;} _LL1658: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Var_e){ _LL1739: _temp1738=(( struct Cyc_Absyn_Var_e_struct*)
_temp1646)->f1; goto _LL1737; _LL1737: _temp1736=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1646)->f2; goto _LL1659;} else{ goto _LL1660;} _LL1660: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Primop_e){ _LL1743: _temp1742=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1646)->f1; goto _LL1741; _LL1741: _temp1740=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1646)->f2; goto _LL1661;} else{ goto _LL1662;} _LL1662: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Increment_e){ _LL1747: _temp1746=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1646)->f1; goto _LL1745; _LL1745: _temp1744=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1646)->f2; goto _LL1663;} else{ goto _LL1664;} _LL1664: if(*(( int*)
_temp1646) ==  Cyc_Absyn_AssignOp_e){ _LL1753: _temp1752=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1646)->f1; goto _LL1751; _LL1751: _temp1750=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1646)->f2; goto _LL1749; _LL1749: _temp1748=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1646)->f3; goto _LL1665;} else{ goto _LL1666;} _LL1666: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Conditional_e){ _LL1759: _temp1758=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1646)->f1; goto _LL1757; _LL1757: _temp1756=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1646)->f2; goto _LL1755; _LL1755: _temp1754=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1646)->f3; goto _LL1667;} else{ goto _LL1668;} _LL1668: if(*(( int*)
_temp1646) ==  Cyc_Absyn_SeqExp_e){ _LL1763: _temp1762=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1646)->f1; goto _LL1761; _LL1761: _temp1760=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1646)->f2; goto _LL1669;} else{ goto _LL1670;} _LL1670: if(*(( int*)
_temp1646) ==  Cyc_Absyn_FnCall_e){ _LL1770: _temp1769=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1646)->f1; goto _LL1768; _LL1768: _temp1767=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1646)->f2; goto _LL1765; _LL1765: _temp1764=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1646)->f3; _temp1766=( struct Cyc_Absyn_VarargCallInfo**)&(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1646)->f3; goto _LL1671;} else{ goto _LL1672;} _LL1672: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Throw_e){ _LL1772: _temp1771=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1646)->f1; goto _LL1673;} else{ goto _LL1674;} _LL1674: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Instantiate_e){ _LL1776: _temp1775=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1646)->f1; goto _LL1774; _LL1774: _temp1773=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1646)->f2; goto _LL1675;} else{ goto _LL1676;} _LL1676: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Cast_e){ _LL1780: _temp1779=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1646)->f1; goto _LL1778; _LL1778: _temp1777=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1646)->f2; goto _LL1677;} else{ goto _LL1678;} _LL1678: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Address_e){ _LL1782: _temp1781=(( struct Cyc_Absyn_Address_e_struct*)
_temp1646)->f1; goto _LL1679;} else{ goto _LL1680;} _LL1680: if(*(( int*)
_temp1646) ==  Cyc_Absyn_New_e){ _LL1786: _temp1785=(( struct Cyc_Absyn_New_e_struct*)
_temp1646)->f1; goto _LL1784; _LL1784: _temp1783=(( struct Cyc_Absyn_New_e_struct*)
_temp1646)->f2; goto _LL1681;} else{ goto _LL1682;} _LL1682: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Sizeofexp_e){ _LL1788: _temp1787=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1646)->f1; goto _LL1683;} else{ goto _LL1684;} _LL1684: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1790: _temp1789=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1646)->f1; goto _LL1685;} else{ goto _LL1686;} _LL1686: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Offsetof_e){ _LL1794: _temp1793=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1646)->f1; goto _LL1792; _LL1792: _temp1791=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1646)->f2; goto _LL1687;} else{ goto _LL1688;} _LL1688: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Gentyp_e){ goto _LL1689;} else{ goto _LL1690;} _LL1690:
if(*(( int*) _temp1646) ==  Cyc_Absyn_Deref_e){ _LL1796: _temp1795=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1646)->f1; goto _LL1691;} else{ goto _LL1692;} _LL1692: if(*(( int*)
_temp1646) ==  Cyc_Absyn_StructMember_e){ _LL1800: _temp1799=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1646)->f1; goto _LL1798; _LL1798: _temp1797=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1646)->f2; goto _LL1693;} else{ goto _LL1694;} _LL1694: if(*(( int*)
_temp1646) ==  Cyc_Absyn_StructArrow_e){ _LL1804: _temp1803=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1646)->f1; goto _LL1802; _LL1802: _temp1801=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1646)->f2; goto _LL1695;} else{ goto _LL1696;} _LL1696: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Subscript_e){ _LL1808: _temp1807=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1646)->f1; goto _LL1806; _LL1806: _temp1805=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1646)->f2; goto _LL1697;} else{ goto _LL1698;} _LL1698: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Tuple_e){ _LL1810: _temp1809=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1646)->f1; goto _LL1699;} else{ goto _LL1700;} _LL1700: if(*(( int*)
_temp1646) ==  Cyc_Absyn_CompoundLit_e){ _LL1814: _temp1813=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1646)->f1; goto _LL1812; _LL1812: _temp1811=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1646)->f2; goto _LL1701;} else{ goto _LL1702;} _LL1702: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Array_e){ _LL1816: _temp1815=(( struct Cyc_Absyn_Array_e_struct*)
_temp1646)->f1; goto _LL1703;} else{ goto _LL1704;} _LL1704: if(*(( int*)
_temp1646) ==  Cyc_Absyn_StmtExp_e){ _LL1818: _temp1817=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1646)->f1; goto _LL1705;} else{ goto _LL1706;} _LL1706: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Codegen_e){ _LL1820: _temp1819=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1646)->f1; goto _LL1707;} else{ goto _LL1708;} _LL1708: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Fill_e){ _LL1822: _temp1821=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1646)->f1; goto _LL1709;} else{ goto _LL1710;} _LL1710: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Comprehension_e){ _LL1828: _temp1827=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1646)->f1; goto _LL1826; _LL1826: _temp1825=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1646)->f2; goto _LL1824; _LL1824: _temp1823=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1646)->f3; goto _LL1711;} else{ goto _LL1712;} _LL1712: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Struct_e){ _LL1838: _temp1837=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1646)->f1; _temp1839=( struct _tuple1**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1646)->f1; goto _LL1835; _LL1835: _temp1834=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1646)->f2; _temp1836=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1646)->f2; goto _LL1833; _LL1833: _temp1832=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1646)->f3; goto _LL1830; _LL1830: _temp1829=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1646)->f4; _temp1831=( struct Cyc_Absyn_Structdecl**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1646)->f4; goto _LL1713;} else{ goto _LL1714;} _LL1714: if(*(( int*)
_temp1646) ==  Cyc_Absyn_AnonStruct_e){ _LL1843: _temp1842=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1646)->f1; goto _LL1841; _LL1841: _temp1840=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1646)->f2; goto _LL1715;} else{ goto _LL1716;} _LL1716: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Tunion_e){ _LL1854: _temp1853=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1646)->f1; _temp1855=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1646)->f1; goto _LL1851; _LL1851: _temp1850=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1646)->f2; _temp1852=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1646)->f2; goto _LL1849; _LL1849: _temp1848=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1646)->f3; goto _LL1847; _LL1847: _temp1846=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1646)->f4; goto _LL1845; _LL1845: _temp1844=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1646)->f5; goto _LL1717;} else{ goto _LL1718;} _LL1718: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Enum_e){ _LL1861: _temp1860=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1646)->f1; _temp1862=( struct _tuple1**)&(( struct Cyc_Absyn_Enum_e_struct*)
_temp1646)->f1; goto _LL1859; _LL1859: _temp1858=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1646)->f2; goto _LL1857; _LL1857: _temp1856=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1646)->f3; goto _LL1719;} else{ goto _LL1720;} _LL1720: if(*(( int*)
_temp1646) ==  Cyc_Absyn_Malloc_e){ _LL1866: _temp1865=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1646)->f1; goto _LL1864; _LL1864: _temp1863=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1646)->f2; goto _LL1721;} else{ goto _LL1647;} _LL1649: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1722); return; _LL1651: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1724); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1653: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1728, _temp1726); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1655: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1730); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1657: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1734, e);
goto _LL1647; _LL1659: t= Cyc_Tcexp_tcVar( te, loc, _temp1738, _temp1736); goto
_LL1647; _LL1661: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1742, _temp1740);
goto _LL1647; _LL1663: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1746,
_temp1744); goto _LL1647; _LL1665: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1752, _temp1750, _temp1748); goto _LL1647; _LL1667: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1758, _temp1756, _temp1754); goto _LL1647; _LL1669: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1762, _temp1760); goto _LL1647; _LL1671: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1769, _temp1767, _temp1766); goto _LL1647; _LL1673: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1771); goto _LL1647; _LL1675: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1775, _temp1773); goto _LL1647; _LL1677: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1779, _temp1777); goto _LL1647; _LL1679: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1781); goto _LL1647; _LL1681: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1785, e, _temp1783); goto _LL1647; _LL1683: { void* _temp1867= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1787); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1867); goto
_LL1647;} _LL1685: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1789); goto
_LL1647; _LL1687: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1793, _temp1791);
goto _LL1647; _LL1689:({ void* _temp1868[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("gen() not in top-level initializer",
sizeof( unsigned char), 35u), _tag_arr( _temp1868, sizeof( void*), 0u));});
return; _LL1691: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1795); goto _LL1647;
_LL1693: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1799, _temp1797);
goto _LL1647; _LL1695: t= Cyc_Tcexp_tcStructArrow( te, loc, topt, _temp1803,
_temp1801); goto _LL1647; _LL1697: t= Cyc_Tcexp_tcSubscript( te, loc, topt,
_temp1807, _temp1805); goto _LL1647; _LL1699: t= Cyc_Tcexp_tcTuple( te, loc,
topt, _temp1809); goto _LL1647; _LL1701: t= Cyc_Tcexp_tcCompoundLit( te, loc,
topt, _temp1813, _temp1811); goto _LL1647; _LL1703: { void** elt_topt= 0; if(
topt !=  0){ void* _temp1869= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
void* _temp1875; void** _temp1877; _LL1871: if(( unsigned int) _temp1869 >  4u?*((
int*) _temp1869) ==  Cyc_Absyn_ArrayType: 0){ _LL1876: _temp1875=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1869)->f1; _temp1877=( void**)&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1869)->f1); goto _LL1872;} else{ goto
_LL1873;} _LL1873: goto _LL1874; _LL1872: elt_topt=( void**) _temp1877; goto
_LL1870; _LL1874: goto _LL1870; _LL1870:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1815); goto _LL1647;} _LL1705: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1817); goto _LL1647; _LL1707: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1819); goto _LL1647; _LL1709: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1821);
goto _LL1647; _LL1711: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1827,
_temp1825, _temp1823); goto _LL1647; _LL1713: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1839, _temp1836, _temp1832, _temp1831); goto _LL1647; _LL1715: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1842, _temp1840); goto _LL1647; _LL1717: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1855, _temp1852, _temp1848, _temp1846, _temp1844); goto
_LL1647; _LL1719:* _temp1862=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1856))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1878=(
struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1878[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1879; _temp1879.tag= Cyc_Absyn_EnumType;
_temp1879.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1858))->name;
_temp1879.f2= _temp1858; _temp1879;}); _temp1878;}); goto _LL1647; _LL1721: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1865, _temp1863); goto _LL1647; _LL1647:;}
e->topt=({ struct Cyc_Core_Opt* _temp1880=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1880->v=( void*) t; _temp1880;});}
