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
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; struct _tuple0{ void* f1;
void* f2; } ; extern void* Cyc_Core_snd( struct _tuple0*); extern unsigned char
Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
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
extern struct Cyc_Core_Opt Cyc_Tcutil_rk; extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk; extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern int Cyc_Tcutil_zero_to_null( struct Cyc_Tcenv_Tenv*, void* t, struct Cyc_Absyn_Exp*
e); extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern void Cyc_Tcutil_explain_failure();
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
te); _temp1;}));} static void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tuple1* q){ struct _handler_cons _temp2;
_push_handler(& _temp2);{ int _temp4= 0; if( setjmp( _temp2.handler)){ _temp4= 1;}
if( ! _temp4){{ void* _temp5= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void*
_temp17; struct Cyc_Absyn_Enumfield* _temp19; struct Cyc_Absyn_Enumdecl* _temp21;
struct Cyc_Absyn_Enumfield* _temp23; void* _temp25; struct Cyc_Absyn_Tunionfield*
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
else{ void* _temp171= Cyc_Tcutil_compress(* topt); void* _temp183; void*
_temp185; void* _temp187; void* _temp189; void* _temp191; _LL173: if((
unsigned int) _temp171 >  4u?*(( int*) _temp171) ==  Cyc_Absyn_IntType: 0){
_LL186: _temp185=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1;
goto _LL184; _LL184: _temp183=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f2; if( _temp183 == ( void*) Cyc_Absyn_B1){ goto _LL174;} else{ goto
_LL175;}} else{ goto _LL175;} _LL175: if(( unsigned int) _temp171 >  4u?*(( int*)
_temp171) ==  Cyc_Absyn_IntType: 0){ _LL190: _temp189=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f1; goto _LL188; _LL188: _temp187=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f2; if( _temp187 == ( void*) Cyc_Absyn_B2){ goto _LL176;} else{ goto
_LL177;}} else{ goto _LL177;} _LL177: if(( unsigned int) _temp171 >  4u?*(( int*)
_temp171) ==  Cyc_Absyn_IntType: 0){ _LL192: _temp191=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f1; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp171 >  4u?*(( int*) _temp171) ==  Cyc_Absyn_PointerType: 0){ goto _LL193;}
else{ goto _LL181;} _LL193: if( _temp165 ==  0){ goto _LL180;} else{ goto _LL181;}
_LL181: goto _LL182; _LL174: t= _temp185 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t;( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp194=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp194[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp195; _temp195.tag= Cyc_Absyn_Const_e;
_temp195.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp196=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp196[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp197; _temp197.tag= Cyc_Absyn_Char_c;
_temp197.f1=( void*) _temp185; _temp197.f2=( unsigned char) _temp165; _temp197;});
_temp196;})); _temp195;}); _temp194;}))); goto _LL172; _LL176: t= _temp189 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp198=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp198[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp199; _temp199.tag= Cyc_Absyn_Const_e; _temp199.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp200=( struct Cyc_Absyn_Short_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp200[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp201; _temp201.tag= Cyc_Absyn_Short_c; _temp201.f1=( void*) _temp189;
_temp201.f2=( short) _temp165; _temp201;}); _temp200;})); _temp199;}); _temp198;})));
goto _LL172; _LL178: t= _temp191 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL172; _LL180:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Const_e_struct* _temp202=( struct Cyc_Absyn_Const_e_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Const_e_struct)); _temp202[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp203; _temp203.tag= Cyc_Absyn_Const_e; _temp203.f1=( void*)(( void*) Cyc_Absyn_Null_c);
_temp203;}); _temp202;})));{ struct Cyc_List_List* _temp204= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp205=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp205[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp206; _temp206.tag= Cyc_Absyn_PointerType;
_temp206.f1=({ struct Cyc_Absyn_PtrInfo _temp207; _temp207.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({ struct Cyc_Core_Opt* _temp209=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp209->v=( void*)
_temp204; _temp209;})); _temp207.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp208=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp208->v=( void*) _temp204;
_temp208;})); _temp207.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp207.tq= Cyc_Absyn_empty_tqual(); _temp207.bounds= Cyc_Absyn_empty_conref();
_temp207;}); _temp206;}); _temp205;}); goto _LL172;} _LL182: t= _temp167 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL172;
_LL172:;} goto _LL144; _LL156: { int len=( int) _get_arr_size( _temp169, sizeof(
unsigned char)); struct Cyc_Absyn_Const_e_struct* _temp210=({ struct Cyc_Absyn_Const_e_struct*
_temp247=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp247[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp248; _temp248.tag= Cyc_Absyn_Const_e;
_temp248.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp249=( struct
Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp249[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp250; _temp250.tag= Cyc_Absyn_Int_c;
_temp250.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp250.f2= len; _temp250;});
_temp249;})); _temp248;}); _temp247;}); struct Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp((
void*) _temp210, loc); elen->topt=({ struct Cyc_Core_Opt* _temp211=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp211->v=( void*) Cyc_Absyn_uint_t;
_temp211;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_const_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp212=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp212[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp213; _temp213.tag= Cyc_Absyn_Upper_b;
_temp213.f1= elen; _temp213;}); _temp212;})); if( topt !=  0){ void* _temp214=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_Exp* _temp222; struct Cyc_Absyn_Tqual
_temp224; void* _temp226; struct Cyc_Absyn_PtrInfo _temp228; struct Cyc_Absyn_Conref*
_temp230; struct Cyc_Absyn_Tqual _temp232; struct Cyc_Absyn_Conref* _temp234;
void* _temp236; void* _temp238; _LL216: if(( unsigned int) _temp214 >  4u?*((
int*) _temp214) ==  Cyc_Absyn_ArrayType: 0){ _LL227: _temp226=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp214)->f1; goto _LL225; _LL225: _temp224=((
struct Cyc_Absyn_ArrayType_struct*) _temp214)->f2; goto _LL223; _LL223: _temp222=((
struct Cyc_Absyn_ArrayType_struct*) _temp214)->f3; goto _LL217;} else{ goto
_LL218;} _LL218: if(( unsigned int) _temp214 >  4u?*(( int*) _temp214) ==  Cyc_Absyn_PointerType:
0){ _LL229: _temp228=(( struct Cyc_Absyn_PointerType_struct*) _temp214)->f1;
_LL239: _temp238=( void*) _temp228.elt_typ; goto _LL237; _LL237: _temp236=( void*)
_temp228.rgn_typ; goto _LL235; _LL235: _temp234= _temp228.nullable; goto _LL233;
_LL233: _temp232= _temp228.tq; goto _LL231; _LL231: _temp230= _temp228.bounds;
goto _LL219;} else{ goto _LL220;} _LL220: goto _LL221; _LL217: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp240=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp240[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp241; _temp241.tag= Cyc_Absyn_ArrayType; _temp241.f1=(
void*) Cyc_Absyn_uchar_t; _temp241.f2= _temp224; _temp241.f3=( struct Cyc_Absyn_Exp*)
elen; _temp241;}); _temp240;}); _LL219: if( ! Cyc_Tcutil_unify(* topt, t)? Cyc_Tcutil_silent_castable(
te, loc, t,* topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp242=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp242->v=( void*) t; _temp242;});
Cyc_Tcutil_unchecked_cast( te, e,* topt); t=* topt;} else{ t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt* _temp243=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp243->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp243;})),
Cyc_Absyn_const_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp244=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp244[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp245; _temp245.tag= Cyc_Absyn_Upper_b;
_temp245.f1= elen; _temp245;}); _temp244;})); if( ! Cyc_Tcutil_unify(* topt, t)?
Cyc_Tcutil_silent_castable( te, loc, t,* topt): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp246=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp246->v=( void*) t; _temp246;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); t=*
topt;}} goto _LL215; _LL221: goto _LL215; _LL215:;} return t;} _LL158: { struct
Cyc_List_List* _temp251= Cyc_Tcenv_lookup_type_vars( te); t=( void*)({ struct
Cyc_Absyn_PointerType_struct* _temp252=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp252[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp253; _temp253.tag= Cyc_Absyn_PointerType;
_temp253.f1=({ struct Cyc_Absyn_PtrInfo _temp254; _temp254.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({ struct Cyc_Core_Opt* _temp256=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp256->v=( void*)
_temp251; _temp256;})); _temp254.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp255=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp255->v=( void*) _temp251;
_temp255;})); _temp254.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp254.tq= Cyc_Absyn_empty_tqual(); _temp254.bounds= Cyc_Absyn_empty_conref();
_temp254;}); _temp253;}); _temp252;}); goto _LL144;} _LL144:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp257= b; struct Cyc_Absyn_Vardecl*
_temp271; struct Cyc_Absyn_Fndecl* _temp273; struct Cyc_Absyn_Vardecl* _temp275;
struct Cyc_Absyn_Vardecl* _temp277; struct Cyc_Absyn_Vardecl* _temp279; _LL259:
if( _temp257 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL260;} else{ goto
_LL261;} _LL261: if(( unsigned int) _temp257 >  1u?*(( int*) _temp257) ==  Cyc_Absyn_Global_b:
0){ _LL272: _temp271=(( struct Cyc_Absyn_Global_b_struct*) _temp257)->f1; goto
_LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp257 >  1u?*(( int*)
_temp257) ==  Cyc_Absyn_Funname_b: 0){ _LL274: _temp273=(( struct Cyc_Absyn_Funname_b_struct*)
_temp257)->f1; goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int)
_temp257 >  1u?*(( int*) _temp257) ==  Cyc_Absyn_Pat_b: 0){ _LL276: _temp275=((
struct Cyc_Absyn_Pat_b_struct*) _temp257)->f1; goto _LL266;} else{ goto _LL267;}
_LL267: if(( unsigned int) _temp257 >  1u?*(( int*) _temp257) ==  Cyc_Absyn_Local_b:
0){ _LL278: _temp277=(( struct Cyc_Absyn_Local_b_struct*) _temp257)->f1; goto
_LL268;} else{ goto _LL269;} _LL269: if(( unsigned int) _temp257 >  1u?*(( int*)
_temp257) ==  Cyc_Absyn_Param_b: 0){ _LL280: _temp279=(( struct Cyc_Absyn_Param_b_struct*)
_temp257)->f1; goto _LL270;} else{ goto _LL258;} _LL260: return({ struct Cyc_Std_String_pa_struct
_temp282; _temp282.tag= Cyc_Std_String_pa; _temp282.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp281[ 1u]={& _temp282}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("undeclared identifier: %s",
sizeof( unsigned char), 26u), _tag_arr( _temp281, sizeof( void*), 1u));}});
_LL262: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp271->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp271->type; _LL264:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp273->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp273);
_LL266: _temp277= _temp275; goto _LL268; _LL268: _temp279= _temp277; goto _LL270;
_LL270:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp279->type; _LL258:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_Core_Opt* opt_args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp283=( void*) fmt->r; void* _temp291; struct _tagged_arr
_temp293; struct Cyc_Absyn_Exp* _temp295; struct Cyc_Absyn_Exp _temp297; void*
_temp298; void* _temp300; struct _tagged_arr _temp302; _LL285: if(*(( int*)
_temp283) ==  Cyc_Absyn_Const_e){ _LL292: _temp291=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp283)->f1; if(( unsigned int) _temp291 >  1u?*(( int*) _temp291) ==  Cyc_Absyn_String_c:
0){ _LL294: _temp293=(( struct Cyc_Absyn_String_c_struct*) _temp291)->f1; goto
_LL286;} else{ goto _LL287;}} else{ goto _LL287;} _LL287: if(*(( int*) _temp283)
==  Cyc_Absyn_Cast_e){ _LL296: _temp295=(( struct Cyc_Absyn_Cast_e_struct*)
_temp283)->f2; _temp297=* _temp295; _LL299: _temp298=( void*) _temp297.r; if(*((
int*) _temp298) ==  Cyc_Absyn_Const_e){ _LL301: _temp300=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp298)->f1; if(( unsigned int) _temp300 >  1u?*(( int*) _temp300) ==  Cyc_Absyn_String_c:
0){ _LL303: _temp302=(( struct Cyc_Absyn_String_c_struct*) _temp300)->f1; goto
_LL288;} else{ goto _LL289;}} else{ goto _LL289;}} else{ goto _LL289;} _LL289:
goto _LL290; _LL286: _temp302= _temp293; goto _LL288; _LL288: desc_types=
type_getter( te,( struct _tagged_arr) _temp302, fmt->loc); goto _LL284; _LL290:
return; _LL284:;} if( opt_args !=  0){ struct Cyc_List_List* _temp304=( struct
Cyc_List_List*) opt_args->v; for( 0; desc_types !=  0? _temp304 !=  0: 0;(
desc_types= desc_types->tl, _temp304= _temp304->tl)){ void* t=( void*)
desc_types->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) _temp304->hd;
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct Cyc_Std_String_pa_struct
_temp307; _temp307.tag= Cyc_Std_String_pa; _temp307.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp306; _temp306.tag= Cyc_Std_String_pa; _temp306.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp305[ 2u]={& _temp306,& _temp307}; Cyc_Tcutil_terr( e->loc,
_tag_arr("descriptor has type \n%s\n but argument has type \n%s", sizeof(
unsigned char), 51u), _tag_arr( _temp305, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}
if( desc_types !=  0){({ void* _temp308[ 0u]={}; Cyc_Tcutil_terr( fmt->loc,
_tag_arr("too few arguments", sizeof( unsigned char), 18u), _tag_arr( _temp308,
sizeof( void*), 0u));});} if( _temp304 !=  0){({ void* _temp309[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) _temp304->hd)->loc, _tag_arr("too many arguments",
sizeof( unsigned char), 19u), _tag_arr( _temp309, sizeof( void*), 0u));});}}}
static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp310= p;
_LL312: if( _temp310 == ( void*) Cyc_Absyn_Plus){ goto _LL313;} else{ goto
_LL314;} _LL314: if( _temp310 == ( void*) Cyc_Absyn_Minus){ goto _LL315;} else{
goto _LL316;} _LL316: if( _temp310 == ( void*) Cyc_Absyn_Not){ goto _LL317;}
else{ goto _LL318;} _LL318: if( _temp310 == ( void*) Cyc_Absyn_Bitnot){ goto
_LL319;} else{ goto _LL320;} _LL320: if( _temp310 == ( void*) Cyc_Absyn_Size){
goto _LL321;} else{ goto _LL322;} _LL322: goto _LL323; _LL313: goto _LL315;
_LL315: if( ! Cyc_Tcutil_is_numeric( e)){({ struct Cyc_Std_String_pa_struct
_temp325; _temp325.tag= Cyc_Std_String_pa; _temp325.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp324[ 1u]={& _temp325}; Cyc_Tcutil_terr( loc, _tag_arr("expecting arithmetic type but found %s",
sizeof( unsigned char), 39u), _tag_arr( _temp324, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL317: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){({ struct Cyc_Std_String_pa_struct
_temp327; _temp327.tag= Cyc_Std_String_pa; _temp327.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp326[ 1u]={& _temp327}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral or * type but found %s",
sizeof( unsigned char), 42u), _tag_arr( _temp326, sizeof( void*), 1u));}});}
return Cyc_Absyn_sint_t; _LL319: if( ! Cyc_Tcutil_is_integral( e)){({ struct Cyc_Std_String_pa_struct
_temp329; _temp329.tag= Cyc_Std_String_pa; _temp329.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp328[ 1u]={& _temp329}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp328, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL321:{ void*
_temp330= t; struct Cyc_Absyn_PtrInfo _temp338; _LL332: if(( unsigned int)
_temp330 >  4u?*(( int*) _temp330) ==  Cyc_Absyn_ArrayType: 0){ goto _LL333;}
else{ goto _LL334;} _LL334: if(( unsigned int) _temp330 >  4u?*(( int*) _temp330)
==  Cyc_Absyn_PointerType: 0){ _LL339: _temp338=(( struct Cyc_Absyn_PointerType_struct*)
_temp330)->f1; goto _LL335;} else{ goto _LL336;} _LL336: goto _LL337; _LL333:
goto _LL331; _LL335: goto _LL331; _LL337:({ struct Cyc_Std_String_pa_struct
_temp341; _temp341.tag= Cyc_Std_String_pa; _temp341.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp340[ 1u]={& _temp341}; Cyc_Tcutil_terr( loc, _tag_arr("size requires pointer or array type, not %s",
sizeof( unsigned char), 44u), _tag_arr( _temp340, sizeof( void*), 1u));}}); goto
_LL331; _LL331:;} return Cyc_Absyn_uint_t; _LL323: return({ void* _temp342[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("Non-unary primop", sizeof( unsigned char), 17u),
_tag_arr( _temp342, sizeof( void*), 0u));}); _LL311:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){({ struct Cyc_Std_String_pa_struct
_temp344; _temp344.tag= Cyc_Std_String_pa; _temp344.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp343[ 1u]={&
_temp344}; Cyc_Tcutil_terr( e1->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp343, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp345=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp345->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp345;}));} if( ! checker( e2)){({ struct Cyc_Std_String_pa_struct
_temp347; _temp347.tag= Cyc_Std_String_pa; _temp347.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp346[ 1u]={&
_temp347}; Cyc_Tcutil_terr( e2->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp346, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp348=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp348->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp348;}));}{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); void* _temp349= t1; struct
Cyc_Absyn_PtrInfo _temp355; struct Cyc_Absyn_Conref* _temp357; struct Cyc_Absyn_Tqual
_temp359; struct Cyc_Absyn_Conref* _temp361; void* _temp363; void* _temp365;
_LL351: if(( unsigned int) _temp349 >  4u?*(( int*) _temp349) ==  Cyc_Absyn_PointerType:
0){ _LL356: _temp355=(( struct Cyc_Absyn_PointerType_struct*) _temp349)->f1;
_LL366: _temp365=( void*) _temp355.elt_typ; goto _LL364; _LL364: _temp363=( void*)
_temp355.rgn_typ; goto _LL362; _LL362: _temp361= _temp355.nullable; goto _LL360;
_LL360: _temp359= _temp355.tq; goto _LL358; _LL358: _temp357= _temp355.bounds;
goto _LL352;} else{ goto _LL353;} _LL353: goto _LL354; _LL352: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){({ struct Cyc_Std_String_pa_struct _temp368; _temp368.tag= Cyc_Std_String_pa;
_temp368.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp367[
1u]={& _temp368}; Cyc_Tcutil_terr( e2->loc, _tag_arr("expecting int but found %s",
sizeof( unsigned char), 27u), _tag_arr( _temp367, sizeof( void*), 1u));}});}
_temp357= Cyc_Absyn_compress_conref( _temp357);{ void* _temp369=( void*)
_temp357->v; void* _temp377; void* _temp379; struct Cyc_Absyn_Exp* _temp381;
_LL371: if(( unsigned int) _temp369 >  1u?*(( int*) _temp369) ==  Cyc_Absyn_Eq_constr:
0){ _LL378: _temp377=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp369)->f1;
if( _temp377 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL372;} else{ goto _LL373;}}
else{ goto _LL373;} _LL373: if(( unsigned int) _temp369 >  1u?*(( int*) _temp369)
==  Cyc_Absyn_Eq_constr: 0){ _LL380: _temp379=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp369)->f1; if(( unsigned int) _temp379 >  1u?*(( int*) _temp379) ==  Cyc_Absyn_Upper_b:
0){ _LL382: _temp381=(( struct Cyc_Absyn_Upper_b_struct*) _temp379)->f1; goto
_LL374;} else{ goto _LL375;}} else{ goto _LL375;} _LL375: goto _LL376; _LL372:
return t1; _LL374: { struct Cyc_Absyn_PointerType_struct* _temp383=({ struct Cyc_Absyn_PointerType_struct*
_temp384=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp384[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp385; _temp385.tag= Cyc_Absyn_PointerType;
_temp385.f1=({ struct Cyc_Absyn_PtrInfo _temp386; _temp386.elt_typ=( void*)
_temp365; _temp386.rgn_typ=( void*) _temp363; _temp386.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp386.tq= _temp359; _temp386.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp386;}); _temp385;}); _temp384;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp383); return( void*) _temp383;} _LL376:( void*)( _temp357->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp387=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp387[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp388; _temp388.tag= Cyc_Absyn_Eq_constr; _temp388.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp388;}); _temp387;}))); return t1;
_LL370:;} _LL354: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL350:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){({ struct Cyc_Std_String_pa_struct
_temp391; _temp391.tag= Cyc_Std_String_pa; _temp391.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp390; _temp390.tag= Cyc_Std_String_pa; _temp390.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp389[ 2u]={&
_temp390,& _temp391}; Cyc_Tcutil_terr( e1->loc, _tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof( unsigned char), 59u), _tag_arr( _temp389, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){({
struct Cyc_Std_String_pa_struct _temp394; _temp394.tag= Cyc_Std_String_pa;
_temp394.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp393; _temp393.tag= Cyc_Std_String_pa; _temp393.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp392[ 2u]={& _temp393,& _temp394}; Cyc_Tcutil_terr( e2->loc,
_tag_arr("expecting either %s or int but found %s", sizeof( unsigned char), 40u),
_tag_arr( _temp392, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({
struct Cyc_Core_Opt* _temp395=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp395->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp395;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_zero_to_null( te, t2, e1)?
1: Cyc_Tcutil_zero_to_null( te, t1, e2)){ return Cyc_Absyn_sint_t;} else{{
struct _tuple0 _temp397=({ struct _tuple0 _temp396; _temp396.f1= Cyc_Tcutil_compress(
t1); _temp396.f2= Cyc_Tcutil_compress( t2); _temp396;}); void* _temp403; struct
Cyc_Absyn_PtrInfo _temp405; void* _temp407; void* _temp409; struct Cyc_Absyn_PtrInfo
_temp411; void* _temp413; _LL399: _LL410: _temp409= _temp397.f1; if((
unsigned int) _temp409 >  4u?*(( int*) _temp409) ==  Cyc_Absyn_PointerType: 0){
_LL412: _temp411=(( struct Cyc_Absyn_PointerType_struct*) _temp409)->f1; _LL414:
_temp413=( void*) _temp411.elt_typ; goto _LL404;} else{ goto _LL401;} _LL404:
_temp403= _temp397.f2; if(( unsigned int) _temp403 >  4u?*(( int*) _temp403) == 
Cyc_Absyn_PointerType: 0){ _LL406: _temp405=(( struct Cyc_Absyn_PointerType_struct*)
_temp403)->f1; _LL408: _temp407=( void*) _temp405.elt_typ; goto _LL400;} else{
goto _LL401;} _LL401: goto _LL402; _LL400: if( Cyc_Tcutil_unify( _temp413,
_temp407)){ return Cyc_Absyn_sint_t;} goto _LL398; _LL402: goto _LL398; _LL398:;}
FAIL:({ struct Cyc_Std_String_pa_struct _temp417; _temp417.tag= Cyc_Std_String_pa;
_temp417.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp416; _temp416.tag= Cyc_Std_String_pa; _temp416.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp415[ 2u]={& _temp416,& _temp417}; Cyc_Tcutil_terr( loc,
_tag_arr("comparison not allowed between %s and %s", sizeof( unsigned char), 41u),
_tag_arr( _temp415, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp418=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp418->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp418;}));}}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp419= p; _LL421: if( _temp419 == ( void*)
Cyc_Absyn_Plus){ goto _LL422;} else{ goto _LL423;} _LL423: if( _temp419 == (
void*) Cyc_Absyn_Minus){ goto _LL424;} else{ goto _LL425;} _LL425: if( _temp419
== ( void*) Cyc_Absyn_Times){ goto _LL426;} else{ goto _LL427;} _LL427: if(
_temp419 == ( void*) Cyc_Absyn_Div){ goto _LL428;} else{ goto _LL429;} _LL429:
if( _temp419 == ( void*) Cyc_Absyn_Mod){ goto _LL430;} else{ goto _LL431;}
_LL431: if( _temp419 == ( void*) Cyc_Absyn_Bitand){ goto _LL432;} else{ goto
_LL433;} _LL433: if( _temp419 == ( void*) Cyc_Absyn_Bitor){ goto _LL434;} else{
goto _LL435;} _LL435: if( _temp419 == ( void*) Cyc_Absyn_Bitxor){ goto _LL436;}
else{ goto _LL437;} _LL437: if( _temp419 == ( void*) Cyc_Absyn_Bitlshift){ goto
_LL438;} else{ goto _LL439;} _LL439: if( _temp419 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL440;} else{ goto _LL441;} _LL441: if( _temp419 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL442;} else{ goto _LL443;} _LL443: if( _temp419 == ( void*) Cyc_Absyn_Eq){
goto _LL444;} else{ goto _LL445;} _LL445: if( _temp419 == ( void*) Cyc_Absyn_Neq){
goto _LL446;} else{ goto _LL447;} _LL447: if( _temp419 == ( void*) Cyc_Absyn_Gt){
goto _LL448;} else{ goto _LL449;} _LL449: if( _temp419 == ( void*) Cyc_Absyn_Lt){
goto _LL450;} else{ goto _LL451;} _LL451: if( _temp419 == ( void*) Cyc_Absyn_Gte){
goto _LL452;} else{ goto _LL453;} _LL453: if( _temp419 == ( void*) Cyc_Absyn_Lte){
goto _LL454;} else{ goto _LL455;} _LL455: goto _LL456; _LL422: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL424: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL426: goto _LL428;
_LL428: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL430: goto _LL432; _LL432: goto _LL434; _LL434: goto _LL436; _LL436: goto
_LL438; _LL438: goto _LL440; _LL440: goto _LL442; _LL442: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL444: goto _LL446; _LL446: goto _LL448;
_LL448: goto _LL450; _LL450: goto _LL452; _LL452: goto _LL454; _LL454: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL456:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp457=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp457[ 0]=({ struct Cyc_Core_Failure_struct
_temp458; _temp458.tag= Cyc_Core_Failure; _temp458.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp458;}); _temp457;})); _LL420:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p == ( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) ==  1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t; switch((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ case 0: _LL459: return({ void* _temp461[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("primitive operator has 0 arguments", sizeof( unsigned char),
35u), _tag_arr( _temp461, sizeof( void*), 0u));}); case 1: _LL460: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
es))->hd); break; case 2: _LL462: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd); break; default: _LL463: return({ void* _temp465[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u), _tag_arr( _temp465, sizeof( void*), 0u));});}
return t;}} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp466=( void*) e->r; void* _temp490; struct Cyc_Absyn_Vardecl* _temp492;
void* _temp494; struct Cyc_Absyn_Vardecl* _temp496; void* _temp498; struct Cyc_Absyn_Vardecl*
_temp500; void* _temp502; struct Cyc_Absyn_Vardecl* _temp504; struct Cyc_Absyn_Exp*
_temp506; struct Cyc_Absyn_Exp* _temp508; struct _tagged_arr* _temp510; struct
Cyc_Absyn_Exp* _temp512; struct _tagged_arr* _temp514; struct Cyc_Absyn_Exp*
_temp516; struct Cyc_Absyn_Exp* _temp518; struct Cyc_Absyn_Exp* _temp520; struct
Cyc_Absyn_Exp* _temp522; _LL468: if(*(( int*) _temp466) ==  Cyc_Absyn_Var_e){
_LL491: _temp490=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp466)->f2; if((
unsigned int) _temp490 >  1u?*(( int*) _temp490) ==  Cyc_Absyn_Param_b: 0){
_LL493: _temp492=(( struct Cyc_Absyn_Param_b_struct*) _temp490)->f1; goto _LL469;}
else{ goto _LL470;}} else{ goto _LL470;} _LL470: if(*(( int*) _temp466) ==  Cyc_Absyn_Var_e){
_LL495: _temp494=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp466)->f2; if((
unsigned int) _temp494 >  1u?*(( int*) _temp494) ==  Cyc_Absyn_Local_b: 0){
_LL497: _temp496=(( struct Cyc_Absyn_Local_b_struct*) _temp494)->f1; goto _LL471;}
else{ goto _LL472;}} else{ goto _LL472;} _LL472: if(*(( int*) _temp466) ==  Cyc_Absyn_Var_e){
_LL499: _temp498=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp466)->f2; if((
unsigned int) _temp498 >  1u?*(( int*) _temp498) ==  Cyc_Absyn_Pat_b: 0){ _LL501:
_temp500=(( struct Cyc_Absyn_Pat_b_struct*) _temp498)->f1; goto _LL473;} else{
goto _LL474;}} else{ goto _LL474;} _LL474: if(*(( int*) _temp466) ==  Cyc_Absyn_Var_e){
_LL503: _temp502=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp466)->f2; if((
unsigned int) _temp502 >  1u?*(( int*) _temp502) ==  Cyc_Absyn_Global_b: 0){
_LL505: _temp504=(( struct Cyc_Absyn_Global_b_struct*) _temp502)->f1; goto
_LL475;} else{ goto _LL476;}} else{ goto _LL476;} _LL476: if(*(( int*) _temp466)
==  Cyc_Absyn_Subscript_e){ _LL509: _temp508=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp466)->f1; goto _LL507; _LL507: _temp506=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp466)->f2; goto _LL477;} else{ goto _LL478;} _LL478: if(*(( int*) _temp466)
==  Cyc_Absyn_StructMember_e){ _LL513: _temp512=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp466)->f1; goto _LL511; _LL511: _temp510=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp466)->f2; goto _LL479;} else{ goto _LL480;} _LL480: if(*(( int*) _temp466)
==  Cyc_Absyn_StructArrow_e){ _LL517: _temp516=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp466)->f1; goto _LL515; _LL515: _temp514=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp466)->f2; goto _LL481;} else{ goto _LL482;} _LL482: if(*(( int*) _temp466)
==  Cyc_Absyn_Deref_e){ _LL519: _temp518=(( struct Cyc_Absyn_Deref_e_struct*)
_temp466)->f1; goto _LL483;} else{ goto _LL484;} _LL484: if(*(( int*) _temp466)
==  Cyc_Absyn_NoInstantiate_e){ _LL521: _temp520=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp466)->f1; goto _LL485;} else{ goto _LL486;} _LL486: if(*(( int*) _temp466)
==  Cyc_Absyn_Instantiate_e){ _LL523: _temp522=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp466)->f1; goto _LL487;} else{ goto _LL488;} _LL488: goto _LL489; _LL469:
_temp496= _temp492; goto _LL471; _LL471: _temp500= _temp496; goto _LL473; _LL473:
_temp504= _temp500; goto _LL475; _LL475: if( !( _temp504->tq).q_const){ return;}
goto _LL467; _LL477:{ void* _temp524= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp508->topt))->v); struct Cyc_Absyn_PtrInfo _temp534; struct Cyc_Absyn_Tqual
_temp536; struct Cyc_Absyn_Tqual _temp538; struct Cyc_List_List* _temp540;
_LL526: if(( unsigned int) _temp524 >  4u?*(( int*) _temp524) ==  Cyc_Absyn_PointerType:
0){ _LL535: _temp534=(( struct Cyc_Absyn_PointerType_struct*) _temp524)->f1;
_LL537: _temp536= _temp534.tq; goto _LL527;} else{ goto _LL528;} _LL528: if((
unsigned int) _temp524 >  4u?*(( int*) _temp524) ==  Cyc_Absyn_ArrayType: 0){
_LL539: _temp538=(( struct Cyc_Absyn_ArrayType_struct*) _temp524)->f2; goto
_LL529;} else{ goto _LL530;} _LL530: if(( unsigned int) _temp524 >  4u?*(( int*)
_temp524) ==  Cyc_Absyn_TupleType: 0){ _LL541: _temp540=(( struct Cyc_Absyn_TupleType_struct*)
_temp524)->f1; goto _LL531;} else{ goto _LL532;} _LL532: goto _LL533; _LL527:
_temp538= _temp536; goto _LL529; _LL529: if( ! _temp538.q_const){ return;} goto
_LL525; _LL531: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp506);{
struct _handler_cons _temp542; _push_handler(& _temp542);{ int _temp544= 0; if(
setjmp( _temp542.handler)){ _temp544= 1;} if( ! _temp544){{ struct _tuple8
_temp547; struct Cyc_Absyn_Tqual _temp548; struct _tuple8* _temp545=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp540,( int) i);
_temp547=* _temp545; _LL549: _temp548= _temp547.f1; goto _LL546; _LL546: if( !
_temp548.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp543=( void*) _exn_thrown; void* _temp551= _temp543; _LL553: if( _temp551 == 
Cyc_List_Nth){ goto _LL554;} else{ goto _LL555;} _LL555: goto _LL556; _LL554:
return; _LL556:( void) _throw( _temp551); _LL552:;}}} goto _LL525;} _LL533: goto
_LL525; _LL525:;} goto _LL467; _LL479:{ void* _temp557= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp512->topt))->v); struct Cyc_Absyn_Structdecl**
_temp569; struct Cyc_List_List* _temp571; struct Cyc_List_List* _temp573; struct
Cyc_Absyn_Uniondecl** _temp575; _LL559: if(( unsigned int) _temp557 >  4u?*((
int*) _temp557) ==  Cyc_Absyn_StructType: 0){ _LL570: _temp569=(( struct Cyc_Absyn_StructType_struct*)
_temp557)->f3; goto _LL560;} else{ goto _LL561;} _LL561: if(( unsigned int)
_temp557 >  4u?*(( int*) _temp557) ==  Cyc_Absyn_AnonUnionType: 0){ _LL572:
_temp571=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp557)->f1; goto _LL562;}
else{ goto _LL563;} _LL563: if(( unsigned int) _temp557 >  4u?*(( int*) _temp557)
==  Cyc_Absyn_AnonStructType: 0){ _LL574: _temp573=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp557)->f1; goto _LL564;} else{ goto _LL565;} _LL565: if(( unsigned int)
_temp557 >  4u?*(( int*) _temp557) ==  Cyc_Absyn_UnionType: 0){ _LL576: _temp575=((
struct Cyc_Absyn_UnionType_struct*) _temp557)->f3; goto _LL566;} else{ goto
_LL567;} _LL567: goto _LL568; _LL560: { struct Cyc_Absyn_Structfield* sf=
_temp569 ==  0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*
_temp569, _temp510); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL558;}
_LL562: _temp573= _temp571; goto _LL564; _LL564: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp573, _temp510); if( sf ==  0? 1: !( sf->tq).q_const){
return;} goto _LL558;} _LL566: { struct Cyc_Absyn_Structfield* sf= _temp575 == 
0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)* _temp575,
_temp510); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL558;} _LL568:
goto _LL558; _LL558:;} goto _LL467; _LL481:{ void* _temp577= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp516->topt))->v); struct Cyc_Absyn_PtrInfo
_temp583; void* _temp585; _LL579: if(( unsigned int) _temp577 >  4u?*(( int*)
_temp577) ==  Cyc_Absyn_PointerType: 0){ _LL584: _temp583=(( struct Cyc_Absyn_PointerType_struct*)
_temp577)->f1; _LL586: _temp585=( void*) _temp583.elt_typ; goto _LL580;} else{
goto _LL581;} _LL581: goto _LL582; _LL580:{ void* _temp587= Cyc_Tcutil_compress(
_temp585); struct Cyc_Absyn_Structdecl** _temp599; struct Cyc_Absyn_Uniondecl**
_temp601; struct Cyc_List_List* _temp603; struct Cyc_List_List* _temp605; _LL589:
if(( unsigned int) _temp587 >  4u?*(( int*) _temp587) ==  Cyc_Absyn_StructType:
0){ _LL600: _temp599=(( struct Cyc_Absyn_StructType_struct*) _temp587)->f3; goto
_LL590;} else{ goto _LL591;} _LL591: if(( unsigned int) _temp587 >  4u?*(( int*)
_temp587) ==  Cyc_Absyn_UnionType: 0){ _LL602: _temp601=(( struct Cyc_Absyn_UnionType_struct*)
_temp587)->f3; goto _LL592;} else{ goto _LL593;} _LL593: if(( unsigned int)
_temp587 >  4u?*(( int*) _temp587) ==  Cyc_Absyn_AnonStructType: 0){ _LL604:
_temp603=(( struct Cyc_Absyn_AnonStructType_struct*) _temp587)->f1; goto _LL594;}
else{ goto _LL595;} _LL595: if(( unsigned int) _temp587 >  4u?*(( int*) _temp587)
==  Cyc_Absyn_AnonUnionType: 0){ _LL606: _temp605=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp587)->f1; goto _LL596;} else{ goto _LL597;} _LL597: goto _LL598; _LL590: {
struct Cyc_Absyn_Structfield* sf= _temp599 ==  0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)* _temp599, _temp514); if( sf ==  0? 1: !( sf->tq).q_const){
return;} goto _LL588;} _LL592: { struct Cyc_Absyn_Structfield* sf= _temp601 == 
0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)* _temp601,
_temp514); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL588;} _LL594:
_temp605= _temp603; goto _LL596; _LL596: { struct Cyc_Absyn_Structfield* sf= Cyc_Absyn_lookup_field(
_temp605, _temp514); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL588;}
_LL598: goto _LL588; _LL588:;} goto _LL578; _LL582: goto _LL578; _LL578:;} goto
_LL467; _LL483:{ void* _temp607= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp518->topt))->v); struct Cyc_Absyn_PtrInfo _temp615; struct Cyc_Absyn_Tqual
_temp617; struct Cyc_Absyn_Tqual _temp619; _LL609: if(( unsigned int) _temp607 > 
4u?*(( int*) _temp607) ==  Cyc_Absyn_PointerType: 0){ _LL616: _temp615=(( struct
Cyc_Absyn_PointerType_struct*) _temp607)->f1; _LL618: _temp617= _temp615.tq;
goto _LL610;} else{ goto _LL611;} _LL611: if(( unsigned int) _temp607 >  4u?*((
int*) _temp607) ==  Cyc_Absyn_ArrayType: 0){ _LL620: _temp619=(( struct Cyc_Absyn_ArrayType_struct*)
_temp607)->f2; goto _LL612;} else{ goto _LL613;} _LL613: goto _LL614; _LL610:
_temp619= _temp617; goto _LL612; _LL612: if( ! _temp619.q_const){ return;} goto
_LL608; _LL614: goto _LL608; _LL608:;} goto _LL467; _LL485: _temp522= _temp520;
goto _LL487; _LL487: Cyc_Tcexp_check_writable( te, _temp522); return; _LL489:
goto _LL467; _LL467:;}({ struct Cyc_Std_String_pa_struct _temp622; _temp622.tag=
Cyc_Std_String_pa; _temp622.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{
void* _temp621[ 1u]={& _temp622}; Cyc_Tcutil_terr( e->loc, _tag_arr("attempt to write a const location: %s",
sizeof( unsigned char), 38u), _tag_arr( _temp621, sizeof( void*), 1u));}});}
static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return({ void* _temp623[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("increment/decrement of non-lvalue", sizeof( unsigned char),
34u), _tag_arr( _temp623, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable( te,
e);{ void* t=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( !
Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){({ struct
Cyc_Std_String_pa_struct _temp625; _temp625.tag= Cyc_Std_String_pa; _temp625.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp624[ 1u]={& _temp625};
Cyc_Tcutil_terr( e->loc, _tag_arr("expecting arithmetic or ? type but found %s",
sizeof( unsigned char), 44u), _tag_arr( _temp624, sizeof( void*), 1u));}});}
return t;}} static void* Cyc_Tcexp_tcConditional( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest( te, e1, _tag_arr("conditional expression",
sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp( te, topt, e2); Cyc_Tcexp_tcExp(
te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt* _temp633=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp633->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp633;}));
struct Cyc_List_List* _temp626=({ struct Cyc_List_List* _temp632=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp632->hd=( void*) e3; _temp632->tl=
0; _temp632;}); struct Cyc_List_List* _temp627=({ struct Cyc_List_List* _temp631=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp631->hd=(
void*) e2; _temp631->tl= _temp626; _temp631;}); if( ! Cyc_Tcutil_coerce_list( te,
t, _temp627)){({ struct Cyc_Std_String_pa_struct _temp630; _temp630.tag= Cyc_Std_String_pa;
_temp630.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e3->topt))->v);{ struct Cyc_Std_String_pa_struct _temp629; _temp629.tag=
Cyc_Std_String_pa; _temp629.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp628[ 2u]={&
_temp629,& _temp630}; Cyc_Tcutil_terr( loc, _tag_arr("conditional clause types do not match: %s != %s",
sizeof( unsigned char), 48u), _tag_arr( _temp628, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1); Cyc_Tcexp_tcExp(
te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v)),
e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void*
t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ void* _temp634=
Cyc_Tcutil_compress( t1); _LL636: if(( unsigned int) _temp634 >  4u?*(( int*)
_temp634) ==  Cyc_Absyn_ArrayType: 0){ goto _LL637;} else{ goto _LL638;} _LL638:
goto _LL639; _LL637:({ void* _temp640[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("cannot assign to an array",
sizeof( unsigned char), 26u), _tag_arr( _temp640, sizeof( void*), 0u));}); goto
_LL635; _LL639: goto _LL635; _LL635:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return({
void* _temp641[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue",
sizeof( unsigned char), 25u), _tag_arr( _temp641, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e1); if( po ==  0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ void*
_temp642=({ struct Cyc_Std_String_pa_struct _temp645; _temp645.tag= Cyc_Std_String_pa;
_temp645.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp644; _temp644.tag= Cyc_Std_String_pa; _temp644.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp643[ 2u]={& _temp644,& _temp645}; Cyc_Tcexp_expr_err( te, loc,
_tag_arr("type mismatch: %s != %s", sizeof( unsigned char), 24u), _tag_arr(
_temp643, sizeof( void*), 2u));}}}); Cyc_Tcutil_unify( t1, t2); Cyc_Tcutil_explain_failure();
return _temp642;}} else{ void* _temp646=( void*) po->v; void* _temp647= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, _temp646, e1, e2); if( !( Cyc_Tcutil_unify( _temp647, t1)? 1:( Cyc_Tcutil_coerceable(
_temp647)? Cyc_Tcutil_coerceable( t1): 0))){ void* _temp648=({ struct Cyc_Std_String_pa_struct
_temp651; _temp651.tag= Cyc_Std_String_pa; _temp651.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ struct Cyc_Std_String_pa_struct _temp650; _temp650.tag= Cyc_Std_String_pa;
_temp650.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp649[
2u]={& _temp650,& _temp651}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof( unsigned char), 82u), _tag_arr( _temp649, sizeof( void*), 2u));}}}); Cyc_Tcutil_unify(
_temp647, t1); Cyc_Tcutil_explain_failure(); return _temp648;} return _temp647;}
return t1;}} static void* Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v;} static struct Cyc_Absyn_Tunionfield*
Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
tu, struct _RegionHandle* r, struct Cyc_List_List* inst, struct Cyc_List_List*
fs){ struct Cyc_List_List* fields; void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;{ void* _temp652= Cyc_Tcutil_compress( t1); void*
_temp662; void* _temp664; _LL654: if( _temp652 == ( void*) Cyc_Absyn_FloatType){
goto _LL655;} else{ goto _LL656;} _LL656: if(( unsigned int) _temp652 >  4u?*((
int*) _temp652) ==  Cyc_Absyn_IntType: 0){ _LL663: _temp662=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp652)->f2; if( _temp662 == ( void*) Cyc_Absyn_B1){
goto _LL657;} else{ goto _LL658;}} else{ goto _LL658;} _LL658: if(( unsigned int)
_temp652 >  4u?*(( int*) _temp652) ==  Cyc_Absyn_IntType: 0){ _LL665: _temp664=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp652)->f2; if( _temp664 == ( void*)
Cyc_Absyn_B2){ goto _LL659;} else{ goto _LL660;}} else{ goto _LL660;} _LL660:
goto _LL661; _LL655: Cyc_Tcutil_unchecked_cast( te, e,( void*) Cyc_Absyn_DoubleType);
t1=( void*) Cyc_Absyn_DoubleType; goto _LL653; _LL657: goto _LL659; _LL659: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); t1= Cyc_Absyn_sint_t; goto _LL653; _LL661: goto _LL653;
_LL653:;} for( fields= fs; fields !=  0; fields= fields->tl){ void* _temp668;
struct Cyc_Position_Segment* _temp670; struct Cyc_List_List* _temp672; struct
Cyc_List_List* _temp674; struct _tuple1* _temp676; struct Cyc_Absyn_Tunionfield
_temp666=*(( struct Cyc_Absyn_Tunionfield*) fields->hd); _LL677: _temp676=
_temp666.name; goto _LL675; _LL675: _temp674= _temp666.tvs; goto _LL673; _LL673:
_temp672= _temp666.typs; goto _LL671; _LL671: _temp670= _temp666.loc; goto
_LL669; _LL669: _temp668=( void*) _temp666.sc; goto _LL667; _LL667: if((
_temp672 ==  0? 1: _temp672->tl !=  0)? 1: _temp674 !=  0){ continue;}{ void* t2=
Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*) _temp672->hd)).f2); if(
Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)
fields->hd);}}} for( fields= fs; fields !=  0; fields= fields->tl){ void*
_temp680; struct Cyc_Position_Segment* _temp682; struct Cyc_List_List* _temp684;
struct Cyc_List_List* _temp686; struct _tuple1* _temp688; struct Cyc_Absyn_Tunionfield
_temp678=*(( struct Cyc_Absyn_Tunionfield*) fields->hd); _LL689: _temp688=
_temp678.name; goto _LL687; _LL687: _temp686= _temp678.tvs; goto _LL685; _LL685:
_temp684= _temp678.typs; goto _LL683; _LL683: _temp682= _temp678.loc; goto
_LL681; _LL681: _temp680=( void*) _temp678.sc; goto _LL679; _LL679: if((
_temp684 ==  0? 1: _temp684->tl !=  0)? 1: _temp686 !=  0){ continue;}{ void* t2=
Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*) _temp684->hd)).f2); if(
Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*) fields->hd);}}}({ struct Cyc_Std_String_pa_struct
_temp692; _temp692.tag= Cyc_Std_String_pa; _temp692.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ struct Cyc_Std_String_pa_struct _temp691; _temp691.tag= Cyc_Std_String_pa;
_temp691.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( tu);{ void* _temp690[
2u]={& _temp691,& _temp692}; Cyc_Tcutil_terr( e->loc, _tag_arr("can't find a field in %s to inject a value of type %s",
sizeof( unsigned char), 54u), _tag_arr( _temp690, sizeof( void*), 2u));}}});
return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
args, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List*
_temp693= args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{
void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp694= t; struct Cyc_Absyn_PtrInfo _temp700; struct Cyc_Absyn_Conref*
_temp702; struct Cyc_Absyn_Tqual _temp704; struct Cyc_Absyn_Conref* _temp706;
void* _temp708; void* _temp710; _LL696: if(( unsigned int) _temp694 >  4u?*((
int*) _temp694) ==  Cyc_Absyn_PointerType: 0){ _LL701: _temp700=(( struct Cyc_Absyn_PointerType_struct*)
_temp694)->f1; _LL711: _temp710=( void*) _temp700.elt_typ; goto _LL709; _LL709:
_temp708=( void*) _temp700.rgn_typ; goto _LL707; _LL707: _temp706= _temp700.nullable;
goto _LL705; _LL705: _temp704= _temp700.tq; goto _LL703; _LL703: _temp702=
_temp700.bounds; goto _LL697;} else{ goto _LL698;} _LL698: goto _LL699; _LL697:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp708); Cyc_Tcutil_check_nonzero_bound(
loc, _temp702);{ void* _temp712= Cyc_Tcutil_compress( _temp710); struct Cyc_Absyn_FnInfo
_temp718; struct Cyc_List_List* _temp720; struct Cyc_List_List* _temp722; struct
Cyc_Absyn_VarargInfo* _temp724; int _temp726; struct Cyc_List_List* _temp728;
void* _temp730; struct Cyc_Core_Opt* _temp732; struct Cyc_List_List* _temp734;
_LL714: if(( unsigned int) _temp712 >  4u?*(( int*) _temp712) ==  Cyc_Absyn_FnType:
0){ _LL719: _temp718=(( struct Cyc_Absyn_FnType_struct*) _temp712)->f1; _LL735:
_temp734= _temp718.tvars; goto _LL733; _LL733: _temp732= _temp718.effect; goto
_LL731; _LL731: _temp730=( void*) _temp718.ret_typ; goto _LL729; _LL729:
_temp728= _temp718.args; goto _LL727; _LL727: _temp726= _temp718.c_varargs; goto
_LL725; _LL725: _temp724= _temp718.cyc_varargs; goto _LL723; _LL723: _temp722=
_temp718.rgn_po; goto _LL721; _LL721: _temp720= _temp718.attributes; goto _LL715;}
else{ goto _LL716;} _LL716: goto _LL717; _LL715: if( topt !=  0){ Cyc_Tcutil_unify(
_temp730,* topt);} while( _temp693 !=  0? _temp728 !=  0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*) _temp693->hd; void* t2=(*(( struct _tuple2*)
_temp728->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg(
te, e1, t2)){({ struct Cyc_Std_String_pa_struct _temp738; _temp738.tag= Cyc_Std_String_pa;
_temp738.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp737; _temp737.tag= Cyc_Std_String_pa; _temp737.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp736[ 2u]={&
_temp737,& _temp738}; Cyc_Tcutil_terr( e1->loc, _tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof( unsigned char), 54u), _tag_arr( _temp736, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
_temp693= _temp693->tl; _temp728= _temp728->tl;} if( _temp728 !=  0){({ void*
_temp739[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function",
sizeof( unsigned char), 31u), _tag_arr( _temp739, sizeof( void*), 0u));});}
else{ if(( _temp693 !=  0? 1: _temp726)? 1: _temp724 !=  0){ if( _temp726){ for(
0; _temp693 !=  0; _temp693= _temp693->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_temp693->hd);}} else{ if( _temp724 ==  0){({ void* _temp740[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments for function", sizeof( unsigned char), 32u),
_tag_arr( _temp740, sizeof( void*), 0u));});} else{ int _temp743; void* _temp745;
struct Cyc_Absyn_VarargInfo _temp741=* _temp724; _LL746: _temp745=( void*)
_temp741.type; goto _LL744; _LL744: _temp743= _temp741.inject; goto _LL742;
_LL742: { struct Cyc_Absyn_VarargCallInfo* _temp747=({ struct Cyc_Absyn_VarargCallInfo*
_temp777=( struct Cyc_Absyn_VarargCallInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp777->num_varargs= 0; _temp777->injectors= 0; _temp777->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp724); _temp777;});* vararg_call_info=( struct Cyc_Absyn_VarargCallInfo*)
_temp747; if( ! _temp743){ for( 0; _temp693 !=  0; _temp693= _temp693->tl){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp693->hd; _temp747->num_varargs
++; Cyc_Tcexp_tcExp( te,( void**)& _temp745, e1); if( ! Cyc_Tcutil_coerce_arg(
te, e1, _temp745)){({ struct Cyc_Std_String_pa_struct _temp750; _temp750.tag=
Cyc_Std_String_pa; _temp750.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp749; _temp749.tag= Cyc_Std_String_pa; _temp749.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp745);{ void* _temp748[ 2u]={& _temp749,& _temp750}; Cyc_Tcutil_terr( loc,
_tag_arr("vararg requires type %s but argument has type %s", sizeof(
unsigned char), 49u), _tag_arr( _temp748, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}}
else{ void* _temp751= Cyc_Tcutil_compress( _temp745); struct Cyc_Absyn_TunionInfo
_temp757; void* _temp759; struct Cyc_List_List* _temp761; void* _temp763; struct
Cyc_Absyn_Tuniondecl** _temp765; struct Cyc_Absyn_Tuniondecl* _temp767; _LL753:
if(( unsigned int) _temp751 >  4u?*(( int*) _temp751) ==  Cyc_Absyn_TunionType:
0){ _LL758: _temp757=(( struct Cyc_Absyn_TunionType_struct*) _temp751)->f1;
_LL764: _temp763=( void*) _temp757.tunion_info; if(*(( int*) _temp763) ==  Cyc_Absyn_KnownTunion){
_LL766: _temp765=(( struct Cyc_Absyn_KnownTunion_struct*) _temp763)->f1;
_temp767=* _temp765; goto _LL762;} else{ goto _LL755;} _LL762: _temp761=
_temp757.targs; goto _LL760; _LL760: _temp759=( void*) _temp757.rgn; goto _LL754;}
else{ goto _LL755;} _LL755: goto _LL756; _LL754: { struct Cyc_Absyn_Tuniondecl*
_temp768=* Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp767->name); struct Cyc_List_List*
fields= 0; if( _temp768->fields ==  0){({ struct Cyc_Std_String_pa_struct
_temp770; _temp770.tag= Cyc_Std_String_pa; _temp770.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp745);{ void* _temp769[ 1u]={& _temp770}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject into %s",
sizeof( unsigned char), 21u), _tag_arr( _temp769, sizeof( void*), 1u));}});}
else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp768->fields))->v;} if( ! Cyc_Tcutil_unify( _temp759, Cyc_Tcenv_curr_rgn( te))){({
void* _temp771[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad region for injected varargs",
sizeof( unsigned char), 32u), _tag_arr( _temp771, sizeof( void*), 0u));});}{
struct _RegionHandle _temp772= _new_region(); struct _RegionHandle* rgn=&
_temp772; _push_region( rgn);{ struct Cyc_List_List* _temp773=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp768->tvs, _temp761);
for( 0; _temp693 !=  0; _temp693= _temp693->tl){ _temp747->num_varargs ++;{
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp693->hd; Cyc_Tcexp_tcExp(
te, 0, e1);{ struct Cyc_Absyn_Tunionfield* _temp774= Cyc_Tcexp_tcInjection( te,
e1, _temp745, rgn, _temp773, fields); if( _temp774 !=  0){ _temp747->injectors=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp747->injectors,({ struct Cyc_List_List* _temp775=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp775->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)
_check_null( _temp774)); _temp775->tl= 0; _temp775;}));}}}}}; _pop_region( rgn);}
goto _LL752;} _LL756:({ void* _temp776[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad inject vararg type",
sizeof( unsigned char), 23u), _tag_arr( _temp776, sizeof( void*), 0u));}); goto
_LL752; _LL752:;}}}}}}{ struct Cyc_List_List* a= _temp720; for( 0; a !=  0; a= a->tl){
void* _temp778=( void*) a->hd; int _temp784; int _temp786; void* _temp788;
_LL780: if(( unsigned int) _temp778 >  16u?*(( int*) _temp778) ==  Cyc_Absyn_Format_att:
0){ _LL789: _temp788=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp778)->f1;
goto _LL787; _LL787: _temp786=(( struct Cyc_Absyn_Format_att_struct*) _temp778)->f2;
goto _LL785; _LL785: _temp784=(( struct Cyc_Absyn_Format_att_struct*) _temp778)->f3;
goto _LL781;} else{ goto _LL782;} _LL782: goto _LL783; _LL781:{ struct
_handler_cons _temp790; _push_handler(& _temp790);{ int _temp792= 0; if( setjmp(
_temp790.handler)){ _temp792= 1;} if( ! _temp792){{ struct Cyc_Absyn_Exp*
_temp793=(( struct Cyc_Absyn_Exp*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)(
args, _temp786 -  1); struct Cyc_Core_Opt* fmt_args; if( _temp784 ==  0){
fmt_args= 0;} else{ fmt_args=({ struct Cyc_Core_Opt* _temp794=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp794->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth_tail)( args, _temp784 -  1);
_temp794;});}{ void* _temp795= _temp788; _LL797: if( _temp795 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL798;} else{ goto _LL799;} _LL799: if( _temp795 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL800;} else{ goto _LL796;} _LL798: Cyc_Tcexp_check_format_args( te,
_temp793, fmt_args, Cyc_Formatstr_get_format_typs); goto _LL796; _LL800: Cyc_Tcexp_check_format_args(
te, _temp793, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL796; _LL796:;}};
_pop_handler();} else{ void* _temp791=( void*) _exn_thrown; void* _temp802=
_temp791; _LL804: if( _temp802 ==  Cyc_List_Nth){ goto _LL805;} else{ goto
_LL806;} _LL806: goto _LL807; _LL805:({ void* _temp808[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad format arguments", sizeof( unsigned char), 21u), _tag_arr(
_temp808, sizeof( void*), 0u));}); goto _LL803; _LL807:( void) _throw( _temp802);
_LL803:;}}} goto _LL779; _LL783: goto _LL779; _LL779:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp732))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp722); return _temp730; _LL717: return({ void* _temp809[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function", sizeof(
unsigned char), 29u), _tag_arr( _temp809, sizeof( void*), 0u));}); _LL713:;}
_LL699: return({ void* _temp810[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u), _tag_arr( _temp810, sizeof( void*), 0u));});
_LL695:;}} static void* Cyc_Tcexp_tcThrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ,
e); if( ! Cyc_Tcutil_coerce_arg( te, e, Cyc_Absyn_exn_typ)){({ struct Cyc_Std_String_pa_struct
_temp812; _temp812.tag= Cyc_Std_String_pa; _temp812.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp811[ 1u]={&
_temp812}; Cyc_Tcutil_terr( loc, _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp811, sizeof( void*), 1u));}});}
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp813=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp813->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp813;}));} static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp814= t1;
struct Cyc_Absyn_PtrInfo _temp820; struct Cyc_Absyn_Conref* _temp822; struct Cyc_Absyn_Tqual
_temp824; struct Cyc_Absyn_Conref* _temp826; void* _temp828; void* _temp830;
_LL816: if(( unsigned int) _temp814 >  4u?*(( int*) _temp814) ==  Cyc_Absyn_PointerType:
0){ _LL821: _temp820=(( struct Cyc_Absyn_PointerType_struct*) _temp814)->f1;
_LL831: _temp830=( void*) _temp820.elt_typ; goto _LL829; _LL829: _temp828=( void*)
_temp820.rgn_typ; goto _LL827; _LL827: _temp826= _temp820.nullable; goto _LL825;
_LL825: _temp824= _temp820.tq; goto _LL823; _LL823: _temp822= _temp820.bounds;
goto _LL817;} else{ goto _LL818;} _LL818: goto _LL819; _LL817:{ void* _temp832=
Cyc_Tcutil_compress( _temp830); struct Cyc_Absyn_FnInfo _temp838; struct Cyc_List_List*
_temp840; struct Cyc_List_List* _temp842; struct Cyc_Absyn_VarargInfo* _temp844;
int _temp846; struct Cyc_List_List* _temp848; void* _temp850; struct Cyc_Core_Opt*
_temp852; struct Cyc_List_List* _temp854; _LL834: if(( unsigned int) _temp832 > 
4u?*(( int*) _temp832) ==  Cyc_Absyn_FnType: 0){ _LL839: _temp838=(( struct Cyc_Absyn_FnType_struct*)
_temp832)->f1; _LL855: _temp854= _temp838.tvars; goto _LL853; _LL853: _temp852=
_temp838.effect; goto _LL851; _LL851: _temp850=( void*) _temp838.ret_typ; goto
_LL849; _LL849: _temp848= _temp838.args; goto _LL847; _LL847: _temp846= _temp838.c_varargs;
goto _LL845; _LL845: _temp844= _temp838.cyc_varargs; goto _LL843; _LL843:
_temp842= _temp838.rgn_po; goto _LL841; _LL841: _temp840= _temp838.attributes;
goto _LL835;} else{ goto _LL836;} _LL836: goto _LL837; _LL835: { struct Cyc_List_List*
instantiation= 0; if( Cyc_List_length( ts) != (( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp854)){ return({ void* _temp856[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u),
_tag_arr( _temp856, sizeof( void*), 0u));});} for( 0; ts !=  0;( ts= ts->tl,
_temp854= _temp854->tl)){ void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp854))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k, 1,( void*) ts->hd); instantiation=({
struct Cyc_List_List* _temp857=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp857->hd=( void*)({ struct _tuple5* _temp858=(
struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp858->f1=( struct Cyc_Absyn_Tvar*)
_temp854->hd; _temp858->f2=( void*) ts->hd; _temp858;}); _temp857->tl=
instantiation; _temp857;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp862=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp862[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp863; _temp863.tag= Cyc_Absyn_FnType; _temp863.f1=({ struct Cyc_Absyn_FnInfo
_temp864; _temp864.tvars= 0; _temp864.effect= _temp852; _temp864.ret_typ=( void*)
_temp850; _temp864.args= _temp848; _temp864.c_varargs= _temp846; _temp864.cyc_varargs=
_temp844; _temp864.rgn_po= _temp842; _temp864.attributes= _temp840; _temp864;});
_temp863;}); _temp862;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp859=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp859[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp860; _temp860.tag= Cyc_Absyn_PointerType;
_temp860.f1=({ struct Cyc_Absyn_PtrInfo _temp861; _temp861.elt_typ=( void*)
new_fn_typ; _temp861.rgn_typ=( void*) _temp828; _temp861.nullable= _temp826;
_temp861.tq= _temp824; _temp861.bounds= _temp822; _temp861;}); _temp860;});
_temp859;}); return new_typ;}} _LL837: goto _LL833; _LL833:;} goto _LL815;
_LL819: goto _LL815; _LL815:;} return({ struct Cyc_Std_String_pa_struct _temp866;
_temp866.tag= Cyc_Std_String_pa; _temp866.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp865[ 1u]={& _temp866}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting polymorphic type but found %s",
sizeof( unsigned char), 40u), _tag_arr( _temp865, sizeof( void*), 1u));}});}}
static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t); Cyc_Tcexp_tcExp(
te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
if(( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0)? ! Cyc_Tcutil_zero_to_null( te, t, e): 0){ Cyc_Tcutil_unify( t2,
t);{ void* _temp867=({ struct Cyc_Std_String_pa_struct _temp870; _temp870.tag=
Cyc_Std_String_pa; _temp870.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp869; _temp869.tag= Cyc_Std_String_pa;
_temp869.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp868[
2u]={& _temp869,& _temp870}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u), _tag_arr( _temp868, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();
return _temp867;}} return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e0, void** topt,
struct Cyc_Absyn_Exp* e){ void** _temp871= 0; struct Cyc_Absyn_Tqual _temp872=
Cyc_Absyn_empty_tqual(); if( topt !=  0){ void* _temp873= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_PtrInfo _temp879; struct Cyc_Absyn_Tqual _temp881; void*
_temp883; _LL875: if(( unsigned int) _temp873 >  4u?*(( int*) _temp873) ==  Cyc_Absyn_PointerType:
0){ _LL880: _temp879=(( struct Cyc_Absyn_PointerType_struct*) _temp873)->f1;
_LL884: _temp883=( void*) _temp879.elt_typ; goto _LL882; _LL882: _temp881=
_temp879.tq; goto _LL876;} else{ goto _LL877;} _LL877: goto _LL878; _LL876:
_temp871=({ void** _temp885=( void**) _cycalloc( sizeof( void*)); _temp885[ 0]=
_temp883; _temp885;}); _temp872= _temp881; goto _LL874; _LL878: goto _LL874;
_LL874:;} Cyc_Tcexp_tcExpNoInst( te, _temp871, e);{ void* _temp886=( void*) e->r;
struct Cyc_Absyn_Exp* _temp896; struct Cyc_Absyn_Exp* _temp898; _LL888: if(*((
int*) _temp886) ==  Cyc_Absyn_Struct_e){ goto _LL889;} else{ goto _LL890;}
_LL890: if(*(( int*) _temp886) ==  Cyc_Absyn_Tuple_e){ goto _LL891;} else{ goto
_LL892;} _LL892: if(*(( int*) _temp886) ==  Cyc_Absyn_Subscript_e){ _LL899:
_temp898=(( struct Cyc_Absyn_Subscript_e_struct*) _temp886)->f1; goto _LL897;
_LL897: _temp896=(( struct Cyc_Absyn_Subscript_e_struct*) _temp886)->f2; goto
_LL893;} else{ goto _LL894;} _LL894: goto _LL895; _LL889: goto _LL891; _LL891:({
void* _temp900[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate",
sizeof( unsigned char), 19u), _tag_arr( _temp900, sizeof( void*), 0u));});{ void*(*
_temp901)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if(
topt !=  0){ void* _temp902= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp908; struct Cyc_Absyn_Conref* _temp910; _LL904: if(( unsigned int) _temp902
>  4u?*(( int*) _temp902) ==  Cyc_Absyn_PointerType: 0){ _LL909: _temp908=((
struct Cyc_Absyn_PointerType_struct*) _temp902)->f1; _LL911: _temp910= _temp908.nullable;
goto _LL905;} else{ goto _LL906;} _LL906: goto _LL907; _LL905: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp910)){ _temp901= Cyc_Absyn_star_typ;}
goto _LL903; _LL907: goto _LL903; _LL903:;} return _temp901(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v,( void*) Cyc_Absyn_HeapRgn, _temp872);}
_LL893:{ void* _temp912= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp898->topt))->v); _LL914: if(( unsigned int) _temp912 >  4u?*((
int*) _temp912) ==  Cyc_Absyn_TupleType: 0){ goto _LL915;} else{ goto _LL916;}
_LL916: goto _LL917; _LL915: goto _LL913; _LL917:( void*)( e0->r=( void*)(( void*)({
struct Cyc_Absyn_Primop_e_struct* _temp918=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp918[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp919; _temp919.tag= Cyc_Absyn_Primop_e; _temp919.f1=(
void*)(( void*) Cyc_Absyn_Plus); _temp919.f2=({ struct Cyc_List_List* _temp920=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp920->hd=(
void*) _temp898; _temp920->tl=({ struct Cyc_List_List* _temp921=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp921->hd=( void*) _temp896;
_temp921->tl= 0; _temp921;}); _temp920;}); _temp919;}); _temp918;}))); return
Cyc_Tcexp_tcPlus( te, _temp898, _temp896); _LL913:;} goto _LL887; _LL895: goto
_LL887; _LL887:;}{ void* _temp924; int _temp926; struct _tuple6 _temp922= Cyc_Tcutil_addressof_props(
te, e); _LL927: _temp926= _temp922.f1; goto _LL925; _LL925: _temp924= _temp922.f2;
goto _LL923; _LL923: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp926){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp924, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
1, t); if( ! Cyc_Evexp_okay_szofarg( t)){({ struct Cyc_Std_String_pa_struct
_temp929; _temp929.tag= Cyc_Std_String_pa; _temp929.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp928[ 1u]={& _temp929}; Cyc_Tcutil_terr( loc, _tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof( unsigned char), 55u), _tag_arr( _temp928, sizeof( void*), 1u));}});}
return Cyc_Absyn_uint_t;} int Cyc_Tcexp_structfield_has_name( struct _tagged_arr*
n, struct Cyc_Absyn_Structfield* sf){ return Cyc_Std_strcmp(* n,* sf->name) == 
0;} static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, void* n){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, 1, t);{ void* _temp930= n; struct _tagged_arr*
_temp936; unsigned int _temp938; _LL932: if(*(( int*) _temp930) ==  Cyc_Absyn_StructField){
_LL937: _temp936=(( struct Cyc_Absyn_StructField_struct*) _temp930)->f1; goto
_LL933;} else{ goto _LL934;} _LL934: if(*(( int*) _temp930) ==  Cyc_Absyn_TupleIndex){
_LL939: _temp938=(( struct Cyc_Absyn_TupleIndex_struct*) _temp930)->f1; goto
_LL935;} else{ goto _LL931;} _LL933: { int bad_type= 1;{ void* _temp940= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp952; struct _tuple1* _temp954; struct Cyc_List_List*
_temp956; struct Cyc_Absyn_Uniondecl** _temp958; struct _tuple1* _temp960;
struct Cyc_List_List* _temp962; _LL942: if(( unsigned int) _temp940 >  4u?*((
int*) _temp940) ==  Cyc_Absyn_StructType: 0){ _LL955: _temp954=(( struct Cyc_Absyn_StructType_struct*)
_temp940)->f1; goto _LL953; _LL953: _temp952=(( struct Cyc_Absyn_StructType_struct*)
_temp940)->f3; goto _LL943;} else{ goto _LL944;} _LL944: if(( unsigned int)
_temp940 >  4u?*(( int*) _temp940) ==  Cyc_Absyn_AnonStructType: 0){ _LL957:
_temp956=(( struct Cyc_Absyn_AnonStructType_struct*) _temp940)->f1; goto _LL945;}
else{ goto _LL946;} _LL946: if(( unsigned int) _temp940 >  4u?*(( int*) _temp940)
==  Cyc_Absyn_UnionType: 0){ _LL961: _temp960=(( struct Cyc_Absyn_UnionType_struct*)
_temp940)->f1; goto _LL959; _LL959: _temp958=(( struct Cyc_Absyn_UnionType_struct*)
_temp940)->f3; goto _LL947;} else{ goto _LL948;} _LL948: if(( unsigned int)
_temp940 >  4u?*(( int*) _temp940) ==  Cyc_Absyn_AnonUnionType: 0){ _LL963:
_temp962=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp940)->f1; goto _LL949;}
else{ goto _LL950;} _LL950: goto _LL951; _LL943: if( _temp952 ==  0){ return({
void* _temp964[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType",
sizeof( unsigned char), 33u), _tag_arr( _temp964, sizeof( void*), 0u));});} if((*
_temp952)->fields ==  0){ goto _LL941;} if( !(( int(*)( int(* pred)( struct
_tagged_arr*, struct Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name, _temp936,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp952)->fields))->v)){({
struct Cyc_Std_String_pa_struct _temp966; _temp966.tag= Cyc_Std_String_pa;
_temp966.f1=( struct _tagged_arr)* _temp936;{ void* _temp965[ 1u]={& _temp966};
Cyc_Tcutil_terr( loc, _tag_arr("no field of struct has name %s", sizeof(
unsigned char), 31u), _tag_arr( _temp965, sizeof( void*), 1u));}});} bad_type= 0;
goto _LL941; _LL945: if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct
Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcexp_structfield_has_name, _temp936, _temp956)){({ struct Cyc_Std_String_pa_struct
_temp968; _temp968.tag= Cyc_Std_String_pa; _temp968.f1=( struct _tagged_arr)*
_temp936;{ void* _temp967[ 1u]={& _temp968}; Cyc_Tcutil_terr( loc, _tag_arr("no field of struct has name %s",
sizeof( unsigned char), 31u), _tag_arr( _temp967, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL941; _LL947: if( _temp958 ==  0){ return({ void* _temp969[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp969, sizeof( void*), 0u));});} if((*
_temp958)->fields ==  0){ goto _LL941;} if( !(( int(*)( int(* pred)( struct
_tagged_arr*, struct Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name, _temp936,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp958)->fields))->v)){({
struct Cyc_Std_String_pa_struct _temp971; _temp971.tag= Cyc_Std_String_pa;
_temp971.f1=( struct _tagged_arr)* _temp936;{ void* _temp970[ 1u]={& _temp971};
Cyc_Tcutil_terr( loc, _tag_arr("no field of union has name %s", sizeof(
unsigned char), 30u), _tag_arr( _temp970, sizeof( void*), 1u));}});} bad_type= 0;
goto _LL941; _LL949: if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct
Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcexp_structfield_has_name, _temp936, _temp962)){({ struct Cyc_Std_String_pa_struct
_temp973; _temp973.tag= Cyc_Std_String_pa; _temp973.f1=( struct _tagged_arr)*
_temp936;{ void* _temp972[ 1u]={& _temp973}; Cyc_Tcutil_terr( loc, _tag_arr("no field of anonymous union has name %s",
sizeof( unsigned char), 40u), _tag_arr( _temp972, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL941; _LL951: goto _LL941; _LL941:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp975; _temp975.tag= Cyc_Std_String_pa; _temp975.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp974[ 1u]={& _temp975};
Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union type", sizeof(
unsigned char), 36u), _tag_arr( _temp974, sizeof( void*), 1u));}});} goto _LL931;}
_LL935: { int bad_type= 1;{ void* _temp976= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp992; struct Cyc_List_List* _temp994; struct Cyc_Absyn_Uniondecl** _temp996;
struct Cyc_List_List* _temp998; struct Cyc_List_List* _temp1000; struct Cyc_Absyn_TunionFieldInfo
_temp1002; void* _temp1004; struct Cyc_Absyn_Tunionfield* _temp1006; struct Cyc_Absyn_Tuniondecl*
_temp1008; _LL978: if(( unsigned int) _temp976 >  4u?*(( int*) _temp976) ==  Cyc_Absyn_StructType:
0){ _LL993: _temp992=(( struct Cyc_Absyn_StructType_struct*) _temp976)->f3; goto
_LL979;} else{ goto _LL980;} _LL980: if(( unsigned int) _temp976 >  4u?*(( int*)
_temp976) ==  Cyc_Absyn_AnonStructType: 0){ _LL995: _temp994=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp976)->f1; goto _LL981;} else{ goto _LL982;} _LL982: if(( unsigned int)
_temp976 >  4u?*(( int*) _temp976) ==  Cyc_Absyn_UnionType: 0){ _LL997: _temp996=((
struct Cyc_Absyn_UnionType_struct*) _temp976)->f3; goto _LL983;} else{ goto
_LL984;} _LL984: if(( unsigned int) _temp976 >  4u?*(( int*) _temp976) ==  Cyc_Absyn_AnonUnionType:
0){ _LL999: _temp998=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp976)->f1;
goto _LL985;} else{ goto _LL986;} _LL986: if(( unsigned int) _temp976 >  4u?*((
int*) _temp976) ==  Cyc_Absyn_TupleType: 0){ _LL1001: _temp1000=(( struct Cyc_Absyn_TupleType_struct*)
_temp976)->f1; goto _LL987;} else{ goto _LL988;} _LL988: if(( unsigned int)
_temp976 >  4u?*(( int*) _temp976) ==  Cyc_Absyn_TunionFieldType: 0){ _LL1003:
_temp1002=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp976)->f1; _LL1005:
_temp1004=( void*) _temp1002.field_info; if(*(( int*) _temp1004) ==  Cyc_Absyn_KnownTunionfield){
_LL1009: _temp1008=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1004)->f1;
goto _LL1007; _LL1007: _temp1006=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1004)->f2; goto _LL989;} else{ goto _LL990;}} else{ goto _LL990;} _LL990:
goto _LL991; _LL979: if( _temp992 ==  0){ return({ void* _temp1010[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("offsetof on unchecked StructType", sizeof( unsigned char), 33u),
_tag_arr( _temp1010, sizeof( void*), 0u));});} if((* _temp992)->fields ==  0){
goto _LL977;} if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp992)->fields))->v)
<=  _temp938){({ struct Cyc_Std_Int_pa_struct _temp1013; _temp1013.tag= Cyc_Std_Int_pa;
_temp1013.f1=( int) _temp938;{ struct Cyc_Std_Int_pa_struct _temp1012; _temp1012.tag=
Cyc_Std_Int_pa; _temp1012.f1=( int)(( unsigned int)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((* _temp992)->fields))->v));{ void* _temp1011[ 2u]={& _temp1012,&
_temp1013}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp1011, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL977; _LL981: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp994) <=  _temp938){({ struct Cyc_Std_Int_pa_struct _temp1016; _temp1016.tag=
Cyc_Std_Int_pa; _temp1016.f1=( int) _temp938;{ struct Cyc_Std_Int_pa_struct
_temp1015; _temp1015.tag= Cyc_Std_Int_pa; _temp1015.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp994));{ void* _temp1014[
2u]={& _temp1015,& _temp1016}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp1014, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL977; _LL983: if( _temp996 ==  0){ return({ void* _temp1017[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp1017, sizeof( void*), 0u));});} if((*
_temp996)->fields ==  0){ goto _LL977;} if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*
_temp996)->fields))->v) <=  _temp938){({ struct Cyc_Std_Int_pa_struct _temp1020;
_temp1020.tag= Cyc_Std_Int_pa; _temp1020.f1=( int) _temp938;{ struct Cyc_Std_Int_pa_struct
_temp1019; _temp1019.tag= Cyc_Std_Int_pa; _temp1019.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((* _temp996)->fields))->v));{ void* _temp1018[
2u]={& _temp1019,& _temp1020}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp1018, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL977; _LL985: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp998) <=  _temp938){({ struct Cyc_Std_Int_pa_struct _temp1023; _temp1023.tag=
Cyc_Std_Int_pa; _temp1023.f1=( int) _temp938;{ struct Cyc_Std_Int_pa_struct
_temp1022; _temp1022.tag= Cyc_Std_Int_pa; _temp1022.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp998));{ void* _temp1021[
2u]={& _temp1022,& _temp1023}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp1021, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL977; _LL987: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1000) <=  _temp938){({ struct Cyc_Std_Int_pa_struct _temp1026; _temp1026.tag=
Cyc_Std_Int_pa; _temp1026.f1=( int) _temp938;{ struct Cyc_Std_Int_pa_struct
_temp1025; _temp1025.tag= Cyc_Std_Int_pa; _temp1025.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1000));{ void*
_temp1024[ 2u]={& _temp1025,& _temp1026}; Cyc_Tcutil_terr( loc, _tag_arr("tuple has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp1024, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL977; _LL989: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1006->typs) <  _temp938){({ struct Cyc_Std_Int_pa_struct _temp1029;
_temp1029.tag= Cyc_Std_Int_pa; _temp1029.f1=( int) _temp938;{ struct Cyc_Std_Int_pa_struct
_temp1028; _temp1028.tag= Cyc_Std_Int_pa; _temp1028.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1006->typs));{ void*
_temp1027[ 2u]={& _temp1028,& _temp1029}; Cyc_Tcutil_terr( loc, _tag_arr("tunionfield has too few components: %d < %d",
sizeof( unsigned char), 44u), _tag_arr( _temp1027, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL977; _LL991: goto _LL977; _LL977:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp1031; _temp1031.tag= Cyc_Std_String_pa; _temp1031.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1030[ 1u]={&
_temp1031}; Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union/tuple/tunion type",
sizeof( unsigned char), 49u), _tag_arr( _temp1030, sizeof( void*), 1u));}});}
goto _LL931;} _LL931:;} return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1032= t;
struct Cyc_Absyn_PtrInfo _temp1038; struct Cyc_Absyn_Conref* _temp1040; void*
_temp1042; void* _temp1044; _LL1034: if(( unsigned int) _temp1032 >  4u?*(( int*)
_temp1032) ==  Cyc_Absyn_PointerType: 0){ _LL1039: _temp1038=(( struct Cyc_Absyn_PointerType_struct*)
_temp1032)->f1; _LL1045: _temp1044=( void*) _temp1038.elt_typ; goto _LL1043;
_LL1043: _temp1042=( void*) _temp1038.rgn_typ; goto _LL1041; _LL1041: _temp1040=
_temp1038.bounds; goto _LL1035;} else{ goto _LL1036;} _LL1036: goto _LL1037;
_LL1035: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1042); Cyc_Tcutil_check_nonzero_bound(
loc, _temp1040); return _temp1044; _LL1037: return({ struct Cyc_Std_String_pa_struct
_temp1047; _temp1047.tag= Cyc_Std_String_pa; _temp1047.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1046[ 1u]={& _temp1047}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting * or @ type but found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1046, sizeof( void*), 1u));}}); _LL1033:;}} static void*
Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1048= t;
struct Cyc_Absyn_Structdecl** _temp1064; struct Cyc_Absyn_Structdecl* _temp1066;
struct Cyc_List_List* _temp1067; struct _tuple1* _temp1069; struct Cyc_List_List*
_temp1071; struct Cyc_List_List* _temp1073; struct Cyc_Absyn_Uniondecl**
_temp1075; struct Cyc_Absyn_Uniondecl* _temp1077; struct Cyc_List_List*
_temp1078; struct _tuple1* _temp1080; _LL1050: if(( unsigned int) _temp1048 >  4u?*((
int*) _temp1048) ==  Cyc_Absyn_StructType: 0){ _LL1070: _temp1069=(( struct Cyc_Absyn_StructType_struct*)
_temp1048)->f1; goto _LL1068; _LL1068: _temp1067=(( struct Cyc_Absyn_StructType_struct*)
_temp1048)->f2; goto _LL1065; _LL1065: _temp1064=(( struct Cyc_Absyn_StructType_struct*)
_temp1048)->f3; if( _temp1064 ==  0){ goto _LL1052;} else{ _temp1066=* _temp1064;
goto _LL1051;}} else{ goto _LL1052;} _LL1052: if(( unsigned int) _temp1048 >  4u?*((
int*) _temp1048) ==  Cyc_Absyn_AnonStructType: 0){ _LL1072: _temp1071=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1048)->f1; goto _LL1053;} else{ goto
_LL1054;} _LL1054: if(( unsigned int) _temp1048 >  4u?*(( int*) _temp1048) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1074: _temp1073=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1048)->f1; goto _LL1055;} else{ goto _LL1056;} _LL1056: if(( unsigned int)
_temp1048 >  4u?*(( int*) _temp1048) ==  Cyc_Absyn_UnionType: 0){ _LL1081:
_temp1080=(( struct Cyc_Absyn_UnionType_struct*) _temp1048)->f1; goto _LL1079;
_LL1079: _temp1078=(( struct Cyc_Absyn_UnionType_struct*) _temp1048)->f2; goto
_LL1076; _LL1076: _temp1075=(( struct Cyc_Absyn_UnionType_struct*) _temp1048)->f3;
if( _temp1075 ==  0){ goto _LL1058;} else{ _temp1077=* _temp1075; goto _LL1057;}}
else{ goto _LL1058;} _LL1058: if(( unsigned int) _temp1048 >  4u?*(( int*)
_temp1048) ==  Cyc_Absyn_ArrayType: 0){ goto _LL1082;} else{ goto _LL1060;}
_LL1082: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
==  0){ goto _LL1059;} else{ goto _LL1060;} _LL1060: if(( unsigned int)
_temp1048 >  4u?*(( int*) _temp1048) ==  Cyc_Absyn_PointerType: 0){ goto _LL1083;}
else{ goto _LL1062;} _LL1083: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof(
unsigned char), 5u)) ==  0){ goto _LL1061;} else{ goto _LL1062;} _LL1062: goto
_LL1063; _LL1051: if( _temp1069 ==  0){ return({ void* _temp1084[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved struct type (COMPILER ERROR)", sizeof(
unsigned char), 40u), _tag_arr( _temp1084, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp1085= Cyc_Absyn_lookup_struct_field( _temp1066, f); if( _temp1085 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1088; _temp1088.tag= Cyc_Std_String_pa;
_temp1088.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1087; _temp1087.tag= Cyc_Std_String_pa; _temp1087.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1069));{ void*
_temp1086[ 2u]={& _temp1087,& _temp1088}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp1086, sizeof( void*), 2u));}}});}{
void* t2;{ struct _RegionHandle _temp1089= _new_region(); struct _RegionHandle*
rgn=& _temp1089; _push_region( rgn);{ struct Cyc_List_List* _temp1090=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r1, struct _RegionHandle* r2, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1066->tvs,
_temp1067); t2= Cyc_Tcutil_rsubstitute( rgn, _temp1090,( void*) _temp1085->type);};
_pop_region( rgn);} return t2;}} _LL1053: { struct Cyc_Absyn_Structfield*
_temp1091= Cyc_Absyn_lookup_field( _temp1071, f); if( _temp1091 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1093; _temp1093.tag= Cyc_Std_String_pa;
_temp1093.f1=( struct _tagged_arr)* f;{ void* _temp1092[ 1u]={& _temp1093}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1092, sizeof( void*), 1u));}});} return( void*) _temp1091->type;}
_LL1055: { struct Cyc_Absyn_Structfield* _temp1094= Cyc_Absyn_lookup_field(
_temp1073, f); if( _temp1094 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1096; _temp1096.tag= Cyc_Std_String_pa; _temp1096.f1=( struct _tagged_arr)*
f;{ void* _temp1095[ 1u]={& _temp1096}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp1095, sizeof( void*), 1u));}});}
return( void*) _temp1094->type;} _LL1057: if( _temp1080 ==  0){ return({ void*
_temp1097[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("unresolved union type (COMPILER ERROR)",
sizeof( unsigned char), 39u), _tag_arr( _temp1097, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structfield* _temp1098= Cyc_Absyn_lookup_union_field( _temp1077,
f); if( _temp1098 ==  0){ return({ struct Cyc_Std_String_pa_struct _temp1101;
_temp1101.tag= Cyc_Std_String_pa; _temp1101.f1=( struct _tagged_arr)* f;{ struct
Cyc_Std_String_pa_struct _temp1100; _temp1100.tag= Cyc_Std_String_pa; _temp1100.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp1080));{ void* _temp1099[ 2u]={& _temp1100,& _temp1101}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union %s has no %s field", sizeof( unsigned char), 25u),
_tag_arr( _temp1099, sizeof( void*), 2u));}}});}{ struct Cyc_List_List*
_temp1102=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1077->tvs, _temp1078); void* _temp1103= Cyc_Tcutil_substitute(
_temp1102,( void*) _temp1098->type); return _temp1103;}} _LL1059:( void*)(
outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1104=(
struct Cyc_Absyn_Primop_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp1104[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp1105; _temp1105.tag= Cyc_Absyn_Primop_e;
_temp1105.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1105.f2=({ struct Cyc_List_List*
_temp1106=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1106->hd=( void*) e; _temp1106->tl= 0; _temp1106;}); _temp1105;});
_temp1104;}))); return Cyc_Absyn_uint_t; _LL1061:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1107=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1107[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1108; _temp1108.tag= Cyc_Absyn_Primop_e;
_temp1108.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1108.f2=({ struct Cyc_List_List*
_temp1109=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1109->hd=( void*) e; _temp1109->tl= 0; _temp1109;}); _temp1108;});
_temp1107;}))); return Cyc_Absyn_uint_t; _LL1063: if( Cyc_Std_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1111; _temp1111.tag= Cyc_Std_String_pa; _temp1111.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1110[ 1u]={& _temp1111}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u), _tag_arr( _temp1110, sizeof( void*), 1u));}});} else{
return({ struct Cyc_Std_String_pa_struct _temp1113; _temp1113.tag= Cyc_Std_String_pa;
_temp1113.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1112[
1u]={& _temp1113}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct or union, found %s",
sizeof( unsigned char), 36u), _tag_arr( _temp1112, sizeof( void*), 1u));}});}
_LL1049:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1114= t;
struct Cyc_Absyn_PtrInfo _temp1120; struct Cyc_Absyn_Conref* _temp1122; void*
_temp1124; void* _temp1126; _LL1116: if(( unsigned int) _temp1114 >  4u?*(( int*)
_temp1114) ==  Cyc_Absyn_PointerType: 0){ _LL1121: _temp1120=(( struct Cyc_Absyn_PointerType_struct*)
_temp1114)->f1; _LL1127: _temp1126=( void*) _temp1120.elt_typ; goto _LL1125;
_LL1125: _temp1124=( void*) _temp1120.rgn_typ; goto _LL1123; _LL1123: _temp1122=
_temp1120.bounds; goto _LL1117;} else{ goto _LL1118;} _LL1118: goto _LL1119;
_LL1117: Cyc_Tcutil_check_nonzero_bound( loc, _temp1122);{ void* _temp1128= Cyc_Tcutil_compress(
_temp1126); struct Cyc_Absyn_Structdecl** _temp1140; struct Cyc_Absyn_Structdecl*
_temp1142; struct Cyc_List_List* _temp1143; struct _tuple1* _temp1145; struct
Cyc_Absyn_Uniondecl** _temp1147; struct Cyc_Absyn_Uniondecl* _temp1149; struct
Cyc_List_List* _temp1150; struct _tuple1* _temp1152; struct Cyc_List_List*
_temp1154; struct Cyc_List_List* _temp1156; _LL1130: if(( unsigned int)
_temp1128 >  4u?*(( int*) _temp1128) ==  Cyc_Absyn_StructType: 0){ _LL1146:
_temp1145=(( struct Cyc_Absyn_StructType_struct*) _temp1128)->f1; goto _LL1144;
_LL1144: _temp1143=(( struct Cyc_Absyn_StructType_struct*) _temp1128)->f2; goto
_LL1141; _LL1141: _temp1140=(( struct Cyc_Absyn_StructType_struct*) _temp1128)->f3;
if( _temp1140 ==  0){ goto _LL1132;} else{ _temp1142=* _temp1140; goto _LL1131;}}
else{ goto _LL1132;} _LL1132: if(( unsigned int) _temp1128 >  4u?*(( int*)
_temp1128) ==  Cyc_Absyn_UnionType: 0){ _LL1153: _temp1152=(( struct Cyc_Absyn_UnionType_struct*)
_temp1128)->f1; goto _LL1151; _LL1151: _temp1150=(( struct Cyc_Absyn_UnionType_struct*)
_temp1128)->f2; goto _LL1148; _LL1148: _temp1147=(( struct Cyc_Absyn_UnionType_struct*)
_temp1128)->f3; if( _temp1147 ==  0){ goto _LL1134;} else{ _temp1149=* _temp1147;
goto _LL1133;}} else{ goto _LL1134;} _LL1134: if(( unsigned int) _temp1128 >  4u?*((
int*) _temp1128) ==  Cyc_Absyn_AnonStructType: 0){ _LL1155: _temp1154=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1128)->f1; goto _LL1135;} else{ goto
_LL1136;} _LL1136: if(( unsigned int) _temp1128 >  4u?*(( int*) _temp1128) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1157: _temp1156=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1128)->f1; goto _LL1137;} else{ goto _LL1138;} _LL1138: goto _LL1139;
_LL1131: { struct Cyc_Absyn_Structfield* _temp1158= Cyc_Absyn_lookup_struct_field(
_temp1142, f); if( _temp1158 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1161; _temp1161.tag= Cyc_Std_String_pa; _temp1161.f1=( struct _tagged_arr)*
f;{ struct Cyc_Std_String_pa_struct _temp1160; _temp1160.tag= Cyc_Std_String_pa;
_temp1160.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1145));{ void* _temp1159[ 2u]={& _temp1160,& _temp1161}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct %s has no %s field", sizeof( unsigned char), 26u),
_tag_arr( _temp1159, sizeof( void*), 2u));}}});}{ void* t3;{ struct
_RegionHandle _temp1162= _new_region(); struct _RegionHandle* rgn=& _temp1162;
_push_region( rgn);{ struct Cyc_List_List* _temp1163=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1142->tvs, _temp1143);
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1163,( void*) _temp1158->type);};
_pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1124); return
t3;}} _LL1133: { struct Cyc_Absyn_Structfield* _temp1164= Cyc_Absyn_lookup_union_field(
_temp1149, f); if( _temp1164 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1167; _temp1167.tag= Cyc_Std_String_pa; _temp1167.f1=( struct _tagged_arr)*
f;{ struct Cyc_Std_String_pa_struct _temp1166; _temp1166.tag= Cyc_Std_String_pa;
_temp1166.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1152));{ void* _temp1165[ 2u]={& _temp1166,& _temp1167}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union %s has no %s field", sizeof( unsigned char), 25u),
_tag_arr( _temp1165, sizeof( void*), 2u));}}});}{ struct Cyc_List_List*
_temp1168=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1149->tvs, _temp1150); void* _temp1169= Cyc_Tcutil_substitute(
_temp1168,( void*) _temp1164->type); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1124); return _temp1169;}} _LL1135: { struct Cyc_Absyn_Structfield*
_temp1170= Cyc_Absyn_lookup_field( _temp1154, f); if( _temp1170 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1172; _temp1172.tag= Cyc_Std_String_pa;
_temp1172.f1=( struct _tagged_arr)* f;{ void* _temp1171[ 1u]={& _temp1172}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1171, sizeof( void*), 1u));}});} return( void*) _temp1170->type;}
_LL1137: { struct Cyc_Absyn_Structfield* _temp1173= Cyc_Absyn_lookup_field(
_temp1156, f); if( _temp1173 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1175; _temp1175.tag= Cyc_Std_String_pa; _temp1175.f1=( struct _tagged_arr)*
f;{ void* _temp1174[ 1u]={& _temp1175}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp1174, sizeof( void*), 1u));}});}
return( void*) _temp1173->type;} _LL1139: return({ struct Cyc_Std_String_pa_struct
_temp1177; _temp1177.tag= Cyc_Std_String_pa; _temp1177.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1176[ 1u]={& _temp1177}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct pointer, found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1176, sizeof( void*), 1u));}}); _LL1129:;} _LL1119: return({
struct Cyc_Std_String_pa_struct _temp1179; _temp1179.tag= Cyc_Std_String_pa;
_temp1179.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1178[
1u]={& _temp1179}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp1178, sizeof( void*), 1u));}});
_LL1115:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1180; _push_handler(& _temp1180);{ int _temp1182= 0; if(
setjmp( _temp1180.handler)){ _temp1182= 1;} if( ! _temp1182){{ void* _temp1183=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1183;}; _pop_handler();} else{ void* _temp1181=(
void*) _exn_thrown; void* _temp1185= _temp1181; _LL1187: if( _temp1185 ==  Cyc_List_Nth){
goto _LL1188;} else{ goto _LL1189;} _LL1189: goto _LL1190; _LL1188: return({
struct Cyc_Std_Int_pa_struct _temp1193; _temp1193.tag= Cyc_Std_Int_pa; _temp1193.f1=(
int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts));{
struct Cyc_Std_Int_pa_struct _temp1192; _temp1192.tag= Cyc_Std_Int_pa; _temp1192.f1=(
int) i;{ void* _temp1191[ 2u]={& _temp1192,& _temp1193}; Cyc_Tcexp_expr_err( te,
loc, _tag_arr("index is %d but tuple has only %d fields", sizeof( unsigned char),
41u), _tag_arr( _temp1191, sizeof( void*), 2u));}}}); _LL1190:( void) _throw(
_temp1185); _LL1186:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return({
struct Cyc_Std_String_pa_struct _temp1195; _temp1195.tag= Cyc_Std_String_pa;
_temp1195.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp1194[
1u]={& _temp1195}; Cyc_Tcexp_expr_err( te, e2->loc, _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp1194, sizeof( void*), 1u));}});}{
void* _temp1196= t1; struct Cyc_Absyn_PtrInfo _temp1204; struct Cyc_Absyn_Conref*
_temp1206; struct Cyc_Absyn_Tqual _temp1208; void* _temp1210; void* _temp1212;
struct Cyc_List_List* _temp1214; _LL1198: if(( unsigned int) _temp1196 >  4u?*((
int*) _temp1196) ==  Cyc_Absyn_PointerType: 0){ _LL1205: _temp1204=(( struct Cyc_Absyn_PointerType_struct*)
_temp1196)->f1; _LL1213: _temp1212=( void*) _temp1204.elt_typ; goto _LL1211;
_LL1211: _temp1210=( void*) _temp1204.rgn_typ; goto _LL1209; _LL1209: _temp1208=
_temp1204.tq; goto _LL1207; _LL1207: _temp1206= _temp1204.bounds; goto _LL1199;}
else{ goto _LL1200;} _LL1200: if(( unsigned int) _temp1196 >  4u?*(( int*)
_temp1196) ==  Cyc_Absyn_TupleType: 0){ _LL1215: _temp1214=(( struct Cyc_Absyn_TupleType_struct*)
_temp1196)->f1; goto _LL1201;} else{ goto _LL1202;} _LL1202: goto _LL1203;
_LL1199: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1206);} else{ if( Cyc_Tcutil_is_bound_one( _temp1206)){({ void*
_temp1216[ 0u]={}; Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u), _tag_arr( _temp1216, sizeof( void*), 0u));});} Cyc_Tcutil_check_nonzero_bound(
loc, _temp1206);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1210); return
_temp1212; _LL1201: return Cyc_Tcexp_ithTupleType( te, loc, _temp1214, e2);
_LL1203: return({ struct Cyc_Std_String_pa_struct _temp1218; _temp1218.tag= Cyc_Std_String_pa;
_temp1218.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp1217[
1u]={& _temp1218}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1217, sizeof( void*), 1u));}});
_LL1197:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){ int done= 0;
struct Cyc_List_List* fields= 0; if( topt !=  0){ void* _temp1219= Cyc_Tcutil_compress(*
topt); struct Cyc_List_List* _temp1225; _LL1221: if(( unsigned int) _temp1219 > 
4u?*(( int*) _temp1219) ==  Cyc_Absyn_TupleType: 0){ _LL1226: _temp1225=((
struct Cyc_Absyn_TupleType_struct*) _temp1219)->f1; goto _LL1222;} else{ goto
_LL1223;} _LL1223: goto _LL1224; _LL1222: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp1225) != (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ goto _LL1220;} for( 0; es !=  0;( es= es->tl, _temp1225= _temp1225->tl)){
void* _temp1227=(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
_temp1225))->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1227,(
struct Cyc_Absyn_Exp*) es->hd); fields=({ struct Cyc_List_List* _temp1228=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1228->hd=(
void*)({ struct _tuple8* _temp1229=( struct _tuple8*) _cycalloc( sizeof( struct
_tuple8)); _temp1229->f1=(*(( struct _tuple8*) _temp1225->hd)).f1; _temp1229->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) es->hd)->topt))->v;
_temp1229;}); _temp1228->tl= fields; _temp1228;});} done= 1; goto _LL1220;
_LL1224: goto _LL1220; _LL1220:;} if( ! done){ for( 0; es !=  0; es= es->tl){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*) es->hd); fields=({
struct Cyc_List_List* _temp1230=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1230->hd=( void*)({ struct _tuple8* _temp1231=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1231->f1= Cyc_Absyn_empty_tqual();
_temp1231->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)
es->hd)->topt))->v; _temp1231;}); _temp1230->tl= fields; _temp1230;});}} return(
void*)({ struct Cyc_Absyn_TupleType_struct* _temp1232=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1232[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1233; _temp1233.tag= Cyc_Absyn_TupleType;
_temp1233.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1233;}); _temp1232;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return({ void* _temp1234[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("tcCompoundLit", sizeof( unsigned char), 14u), _tag_arr(
_temp1234, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({ struct Cyc_Core_Opt* _temp1257=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1257->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1257;})); struct Cyc_Absyn_Const_e_struct* _temp1235=({ struct Cyc_Absyn_Const_e_struct*
_temp1253=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1253[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1254; _temp1254.tag= Cyc_Absyn_Const_e;
_temp1254.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1255=(
struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1255[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1256; _temp1256.tag= Cyc_Absyn_Int_c;
_temp1256.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1256.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1256;}); _temp1255;}));
_temp1254;}); _temp1253;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) _temp1235, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1236=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1236->v=( void*)
Cyc_Absyn_uint_t; _temp1236;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1251=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1251[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1252; _temp1252.tag= Cyc_Absyn_ArrayType;
_temp1252.f1=( void*) res; _temp1252.f2= Cyc_Absyn_empty_tqual(); _temp1252.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1252;}); _temp1251;});{ struct Cyc_List_List*
es2= es; for( 0; es2 !=  0; es2= es2->tl){ Cyc_Tcexp_tcExpInitializer( te,
elt_topt,( struct Cyc_Absyn_Exp*) es2->hd);}} if( ! Cyc_Tcutil_coerce_list( te,
res, es)){({ struct Cyc_Std_String_pa_struct _temp1238; _temp1238.tag= Cyc_Std_String_pa;
_temp1238.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( res);{ void*
_temp1237[ 1u]={& _temp1238}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->loc, _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp1237, sizeof( void*), 1u));}});}{
int offset= 0; for( 0; des !=  0;( offset ++, des= des->tl)){ struct Cyc_List_List*
ds=(*(( struct _tuple7*) des->hd)).f1; if( ds !=  0){ void* _temp1239=( void*)
ds->hd; struct Cyc_Absyn_Exp* _temp1245; _LL1241: if(*(( int*) _temp1239) == 
Cyc_Absyn_FieldName){ goto _LL1242;} else{ goto _LL1243;} _LL1243: if(*(( int*)
_temp1239) ==  Cyc_Absyn_ArrayElement){ _LL1246: _temp1245=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1239)->f1; goto _LL1244;} else{ goto _LL1240;} _LL1242:({ void* _temp1247[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("only array index designators are supported",
sizeof( unsigned char), 43u), _tag_arr( _temp1247, sizeof( void*), 0u));}); goto
_LL1240; _LL1244: Cyc_Tcexp_tcExpInitializer( te, 0, _temp1245);{ unsigned int i=
Cyc_Evexp_eval_const_uint_exp( _temp1245); if( i !=  offset){({ struct Cyc_Std_Int_pa_struct
_temp1250; _temp1250.tag= Cyc_Std_Int_pa; _temp1250.f1=( int) i;{ struct Cyc_Std_Int_pa_struct
_temp1249; _temp1249.tag= Cyc_Std_Int_pa; _temp1249.f1=( int)(( unsigned int)
offset);{ void* _temp1248[ 2u]={& _temp1249,& _temp1250}; Cyc_Tcutil_terr(
_temp1245->loc, _tag_arr("expecting index designator %d but found %d", sizeof(
unsigned char), 43u), _tag_arr( _temp1248, sizeof( void*), 2u));}}});} goto
_LL1240;} _LL1240:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){({
struct Cyc_Std_String_pa_struct _temp1259; _temp1259.tag= Cyc_Std_String_pa;
_temp1259.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v);{ void* _temp1258[ 1u]={& _temp1259}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("expecting unsigned int, found %s", sizeof( unsigned char),
33u), _tag_arr( _temp1258, sizeof( void*), 1u));}});} if( !( vd->tq).q_const){({
void* _temp1260[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u), _tag_arr( _temp1260, sizeof( void*), 0u));});} if(
te->le !=  0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);}{ struct Cyc_Absyn_PtrInfo pinfo; void** _temp1261= 0; struct Cyc_Absyn_Tqual*
_temp1262= 0; if( topt !=  0){ void* _temp1263= Cyc_Tcutil_compress(* topt);
struct Cyc_Absyn_PtrInfo _temp1271; struct Cyc_Absyn_Exp* _temp1273; struct Cyc_Absyn_Tqual
_temp1275; struct Cyc_Absyn_Tqual* _temp1277; void* _temp1278; void** _temp1280;
_LL1265: if(( unsigned int) _temp1263 >  4u?*(( int*) _temp1263) ==  Cyc_Absyn_PointerType:
0){ _LL1272: _temp1271=(( struct Cyc_Absyn_PointerType_struct*) _temp1263)->f1;
goto _LL1266;} else{ goto _LL1267;} _LL1267: if(( unsigned int) _temp1263 >  4u?*((
int*) _temp1263) ==  Cyc_Absyn_ArrayType: 0){ _LL1279: _temp1278=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1263)->f1; _temp1280=( void**)&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1263)->f1); goto _LL1276; _LL1276:
_temp1275=(( struct Cyc_Absyn_ArrayType_struct*) _temp1263)->f2; _temp1277=(
struct Cyc_Absyn_Tqual*)&(( struct Cyc_Absyn_ArrayType_struct*) _temp1263)->f2;
goto _LL1274; _LL1274: _temp1273=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1263)->f3; goto _LL1268;} else{ goto _LL1269;} _LL1269: goto _LL1270;
_LL1266: pinfo= _temp1271; _temp1261=( void**)(( void**)(( void*)& pinfo.elt_typ));
_temp1262=( struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1264; _LL1268: _temp1261=(
void**) _temp1280; _temp1262=( struct Cyc_Absyn_Tqual*) _temp1277; goto _LL1264;
_LL1270: goto _LL1264; _LL1264:;}{ void* t= Cyc_Tcexp_tcExp( te, _temp1261, body);
if( te->le ==  0){ if( ! Cyc_Tcutil_is_const_exp( te, bound)){({ void* _temp1281[
0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant", sizeof(
unsigned char), 22u), _tag_arr( _temp1281, sizeof( void*), 0u));});} if( ! Cyc_Tcutil_is_const_exp(
te, body)){({ void* _temp1282[ 0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("body is not constant",
sizeof( unsigned char), 21u), _tag_arr( _temp1282, sizeof( void*), 0u));});}}
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1283=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1283[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1284; _temp1284.tag= Cyc_Absyn_ArrayType;
_temp1284.f1=( void*) t; _temp1284.f2= _temp1262 ==  0? Cyc_Absyn_empty_tqual():*
_temp1262; _temp1284.f3=( struct Cyc_Absyn_Exp*) bound; _temp1284;}); _temp1283;});}}}
struct _tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct _tuple1** tn, struct Cyc_Core_Opt** otyps, struct Cyc_List_List*
args, struct Cyc_Absyn_Structdecl** sd_opt){ struct Cyc_Absyn_Structdecl* sd;
if(* sd_opt !=  0){ sd=( struct Cyc_Absyn_Structdecl*) _check_null(* sd_opt);}
else{{ struct _handler_cons _temp1285; _push_handler(& _temp1285);{ int
_temp1287= 0; if( setjmp( _temp1285.handler)){ _temp1287= 1;} if( ! _temp1287){
sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();} else{ void*
_temp1286=( void*) _exn_thrown; void* _temp1289= _temp1286; _LL1291: if(
_temp1289 ==  Cyc_Dict_Absent){ goto _LL1292;} else{ goto _LL1293;} _LL1293:
goto _LL1294; _LL1292:({ struct Cyc_Std_String_pa_struct _temp1296; _temp1296.tag=
Cyc_Std_String_pa; _temp1296.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1295[ 1u]={& _temp1296}; Cyc_Tcutil_terr( loc, _tag_arr("unbound struct name %s",
sizeof( unsigned char), 23u), _tag_arr( _temp1295, sizeof( void*), 1u));}});
return topt !=  0?* topt:( void*) Cyc_Absyn_VoidType; _LL1294:( void) _throw(
_temp1289); _LL1290:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!=  0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1297= _new_region(); struct _RegionHandle* rgn=&
_temp1297; _push_region( rgn);{ struct _tuple4 _temp1298=({ struct _tuple4
_temp1322; _temp1322.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1322.f2= rgn;
_temp1322;}); struct Cyc_List_List* _temp1299=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1298, sd->tvs); struct Cyc_List_List* _temp1300=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1299); struct Cyc_Absyn_StructType_struct*
_temp1301=({ struct Cyc_Absyn_StructType_struct* _temp1319=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1319[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1320; _temp1320.tag= Cyc_Absyn_StructType;
_temp1320.f1=( struct _tuple1*)* tn; _temp1320.f2= _temp1300; _temp1320.f3=({
struct Cyc_Absyn_Structdecl** _temp1321=( struct Cyc_Absyn_Structdecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1321[ 0]= sd; _temp1321;});
_temp1320;}); _temp1319;}); if( topt !=  0){ Cyc_Tcutil_unify(( void*) _temp1301,*
topt);}* otyps=({ struct Cyc_Core_Opt* _temp1302=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1302->v=( void*) _temp1300;
_temp1302;}); if( sd->fields ==  0){({ void* _temp1303[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("can't build abstract struct", sizeof( unsigned char), 28u),
_tag_arr( _temp1303, sizeof( void*), 0u));});{ void* _temp1304=( void*)
_temp1301; _npop_handler( 0u); return _temp1304;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields !=  0; fields= fields->tl){ struct _tuple9 _temp1307; struct Cyc_Absyn_Exp*
_temp1308; struct Cyc_Absyn_Structfield* _temp1310; struct _tuple9* _temp1305=(
struct _tuple9*) fields->hd; _temp1307=* _temp1305; _LL1311: _temp1310=
_temp1307.f1; goto _LL1309; _LL1309: _temp1308= _temp1307.f2; goto _LL1306;
_LL1306: { void* _temp1312= Cyc_Tcutil_rsubstitute( rgn, _temp1299,( void*)
_temp1310->type); Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1312, _temp1308);
if( ! Cyc_Tcutil_coerce_arg( te, _temp1308, _temp1312)){({ struct Cyc_Std_String_pa_struct
_temp1317; _temp1317.tag= Cyc_Std_String_pa; _temp1317.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1308->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1316; _temp1316.tag= Cyc_Std_String_pa;
_temp1316.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp1312);{ struct
Cyc_Std_String_pa_struct _temp1315; _temp1315.tag= Cyc_Std_String_pa; _temp1315.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string(* tn);{ struct Cyc_Std_String_pa_struct
_temp1314; _temp1314.tag= Cyc_Std_String_pa; _temp1314.f1=( struct _tagged_arr)*
_temp1310->name;{ void* _temp1313[ 4u]={& _temp1314,& _temp1315,& _temp1316,&
_temp1317}; Cyc_Tcutil_terr( _temp1308->loc, _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1313, sizeof( void*), 4u));}}}}});
Cyc_Tcutil_explain_failure();}}}{ void* _temp1318=( void*) _temp1301;
_npop_handler( 0u); return _temp1318;}}}; _pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* ts, struct
Cyc_List_List* args){{ struct _RegionHandle _temp1323= _new_region(); struct
_RegionHandle* rgn=& _temp1323; _push_region( rgn);{ void* _temp1324= Cyc_Tcutil_compress(
ts); struct Cyc_List_List* _temp1330; _LL1326: if(( unsigned int) _temp1324 >  4u?*((
int*) _temp1324) ==  Cyc_Absyn_AnonStructType: 0){ _LL1331: _temp1330=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1324)->f1; goto _LL1327;} else{ goto
_LL1328;} _LL1328: goto _LL1329; _LL1327: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1330); for( 0; fields !=  0; fields= fields->tl){ struct
_tuple9 _temp1334; struct Cyc_Absyn_Exp* _temp1335; struct Cyc_Absyn_Structfield*
_temp1337; struct _tuple9* _temp1332=( struct _tuple9*) fields->hd; _temp1334=*
_temp1332; _LL1338: _temp1337= _temp1334.f1; goto _LL1336; _LL1336: _temp1335=
_temp1334.f2; goto _LL1333; _LL1333: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1337->type)), _temp1335); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1335,( void*) _temp1337->type)){({ struct Cyc_Std_String_pa_struct
_temp1342; _temp1342.tag= Cyc_Std_String_pa; _temp1342.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1335->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1341; _temp1341.tag= Cyc_Std_String_pa;
_temp1341.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*) _temp1337->type);{
struct Cyc_Std_String_pa_struct _temp1340; _temp1340.tag= Cyc_Std_String_pa;
_temp1340.f1=( struct _tagged_arr)* _temp1337->name;{ void* _temp1339[ 3u]={&
_temp1340,& _temp1341,& _temp1342}; Cyc_Tcutil_terr( _temp1335->loc, _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1339, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} goto _LL1325;} _LL1329:({ void* _temp1343[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("tcAnonStruct:  not an AnonStructType", sizeof( unsigned char), 37u),
_tag_arr( _temp1343, sizeof( void*), 0u));}); goto _LL1325; _LL1325:;};
_pop_region( rgn);} return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List*
es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield* tuf){ struct
_RegionHandle _temp1344= _new_region(); struct _RegionHandle* rgn=& _temp1344;
_push_region( rgn);{ struct _tuple4 _temp1345=({ struct _tuple4 _temp1391;
_temp1391.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1391.f2= rgn; _temp1391;});
struct Cyc_List_List* _temp1346=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1345, tud->tvs); struct Cyc_List_List* _temp1347=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1345, tuf->tvs); struct Cyc_List_List* _temp1348=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1346); struct Cyc_List_List* _temp1349=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1347); struct Cyc_List_List*
_temp1350=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1346, _temp1347);*
all_ref=({ struct Cyc_Core_Opt* _temp1351=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1351->v=( void*) _temp1348; _temp1351;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1352=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1352->v=( void*) _temp1349; _temp1352;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1386=( struct
Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1386[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1387; _temp1387.tag=
Cyc_Absyn_TunionFieldType; _temp1387.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1388; _temp1388.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1389=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1389[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1390; _temp1390.tag= Cyc_Absyn_KnownTunionfield; _temp1390.f1= tud;
_temp1390.f2= tuf; _temp1390;}); _temp1389;})); _temp1388.targs= _temp1348;
_temp1388;}); _temp1387;}); _temp1386;}); if( topt !=  0){ void* _temp1353= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_TunionInfo _temp1361; void* _temp1363; struct Cyc_List_List*
_temp1365; _LL1355: if(( unsigned int) _temp1353 >  4u?*(( int*) _temp1353) == 
Cyc_Absyn_TunionFieldType: 0){ goto _LL1356;} else{ goto _LL1357;} _LL1357: if((
unsigned int) _temp1353 >  4u?*(( int*) _temp1353) ==  Cyc_Absyn_TunionType: 0){
_LL1362: _temp1361=(( struct Cyc_Absyn_TunionType_struct*) _temp1353)->f1;
_LL1366: _temp1365= _temp1361.targs; goto _LL1364; _LL1364: _temp1363=( void*)
_temp1361.rgn; goto _LL1358;} else{ goto _LL1359;} _LL1359: goto _LL1360;
_LL1356: Cyc_Tcutil_unify(* topt, res); goto _LL1354; _LL1358:{ struct Cyc_List_List*
a= _temp1348; for( 0; a !=  0? _temp1365 !=  0: 0;( a= a->tl, _temp1365=
_temp1365->tl)){ Cyc_Tcutil_unify(( void*) a->hd,( void*) _temp1365->hd);}} if(
tuf->typs ==  0? es ==  0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1367=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1367->v=(
void*) res; _temp1367;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1368=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1368[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1369; _temp1369.tag=
Cyc_Absyn_TunionType; _temp1369.f1=({ struct Cyc_Absyn_TunionInfo _temp1370;
_temp1370.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1371=( struct Cyc_Absyn_KnownTunion_struct*) _cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1371[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1372; _temp1372.tag=
Cyc_Absyn_KnownTunion; _temp1372.f1=({ struct Cyc_Absyn_Tuniondecl** _temp1373=(
struct Cyc_Absyn_Tuniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp1373[ 0]= tud; _temp1373;}); _temp1372;}); _temp1371;})); _temp1370.targs=
_temp1348; _temp1370.rgn=( void*) _temp1363; _temp1370;}); _temp1369;});
_temp1368;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1374= res;
_npop_handler( 0u); return _temp1374;}} goto _LL1354; _LL1360: goto _LL1354;
_LL1354:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es !=  0? ts !=  0: 0;(
es= es->tl, ts= ts->tl)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) es->hd;
void* t= Cyc_Tcutil_rsubstitute( rgn, _temp1350,(*(( struct _tuple8*) ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){({ struct Cyc_Std_String_pa_struct _temp1378; _temp1378.tag= Cyc_Std_String_pa;
_temp1378.f1=( struct _tagged_arr)( e->topt ==  0? _tag_arr("?", sizeof(
unsigned char), 2u): Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v));{ struct Cyc_Std_String_pa_struct _temp1377;
_temp1377.tag= Cyc_Std_String_pa; _temp1377.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp1376; _temp1376.tag= Cyc_Std_String_pa;
_temp1376.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tuf->name);{ void*
_temp1375[ 3u]={& _temp1376,& _temp1377,& _temp1378}; Cyc_Tcutil_terr( e->loc,
_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u), _tag_arr( _temp1375, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} if( es !=  0){ void* _temp1381=({ struct Cyc_Std_String_pa_struct
_temp1380; _temp1380.tag= Cyc_Std_String_pa; _temp1380.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1379[ 1u]={& _temp1380}; Cyc_Tcexp_expr_err(
te,(( struct Cyc_Absyn_Exp*) es->hd)->loc, _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u), _tag_arr( _temp1379, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1381;} if( ts !=  0){ void* _temp1384=({ struct
Cyc_Std_String_pa_struct _temp1383; _temp1383.tag= Cyc_Std_String_pa; _temp1383.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1382[ 1u]={&
_temp1383}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments for tunion constructor %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1382, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1384;}{ void* _temp1385= res; _npop_handler( 0u);
return _temp1385;}}}}; _pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 
0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1402=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1402[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1403; _temp1403.tag= Cyc_Absyn_RgnHandleType; _temp1403.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp1404=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1404->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1404;})); _temp1403;}); _temp1402;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1392= Cyc_Tcutil_compress( handle_type); void*
_temp1398; _LL1394: if(( unsigned int) _temp1392 >  4u?*(( int*) _temp1392) == 
Cyc_Absyn_RgnHandleType: 0){ _LL1399: _temp1398=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1392)->f1; goto _LL1395;} else{ goto _LL1396;} _LL1396: goto _LL1397;
_LL1395: rgn= _temp1398; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1393; _LL1397:({ struct Cyc_Std_String_pa_struct _temp1401; _temp1401.tag=
Cyc_Std_String_pa; _temp1401.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1400[ 1u]={& _temp1401}; Cyc_Tcutil_terr( ropt->loc,
_tag_arr("expecting region_t type but found %s", sizeof( unsigned char), 37u),
_tag_arr( _temp1400, sizeof( void*), 1u));}}); goto _LL1393; _LL1393:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t);{
void* _temp1405= t; struct Cyc_Absyn_TunionFieldInfo _temp1411; void* _temp1413;
struct Cyc_Absyn_Tunionfield* _temp1415; _LL1407: if(( unsigned int) _temp1405 > 
4u?*(( int*) _temp1405) ==  Cyc_Absyn_TunionFieldType: 0){ _LL1412: _temp1411=((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1405)->f1; _LL1414: _temp1413=(
void*) _temp1411.field_info; if(*(( int*) _temp1413) ==  Cyc_Absyn_KnownTunionfield){
_LL1416: _temp1415=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1413)->f2;
goto _LL1408;} else{ goto _LL1409;}} else{ goto _LL1409;} _LL1409: goto _LL1410;
_LL1408: if( _temp1415->tvs !=  0){({ void* _temp1417[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("malloc with existential types not yet implemented", sizeof(
unsigned char), 50u), _tag_arr( _temp1417, sizeof( void*), 0u));});} goto
_LL1406; _LL1410: goto _LL1406; _LL1406:;}{ void*(* _temp1418)( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt !=  0){ void*
_temp1419= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp1425;
struct Cyc_Absyn_Conref* _temp1427; _LL1421: if(( unsigned int) _temp1419 >  4u?*((
int*) _temp1419) ==  Cyc_Absyn_PointerType: 0){ _LL1426: _temp1425=(( struct Cyc_Absyn_PointerType_struct*)
_temp1419)->f1; _LL1428: _temp1427= _temp1425.nullable; goto _LL1422;} else{
goto _LL1423;} _LL1423: goto _LL1424; _LL1422: if((( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1427)){ _temp1418= Cyc_Absyn_star_typ;} goto
_LL1420; _LL1424: goto _LL1420; _LL1420:;} return _temp1418( t, rgn, Cyc_Absyn_empty_tqual());}}
static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1429=( void*) s->r; struct Cyc_Absyn_Exp* _temp1439;
struct Cyc_Absyn_Stmt* _temp1441; struct Cyc_Absyn_Stmt* _temp1443; struct Cyc_Absyn_Stmt*
_temp1445; struct Cyc_Absyn_Decl* _temp1447; _LL1431: if(( unsigned int)
_temp1429 >  1u?*(( int*) _temp1429) ==  Cyc_Absyn_Exp_s: 0){ _LL1440: _temp1439=((
struct Cyc_Absyn_Exp_s_struct*) _temp1429)->f1; goto _LL1432;} else{ goto
_LL1433;} _LL1433: if(( unsigned int) _temp1429 >  1u?*(( int*) _temp1429) == 
Cyc_Absyn_Seq_s: 0){ _LL1444: _temp1443=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1429)->f1; goto _LL1442; _LL1442: _temp1441=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1429)->f2; goto _LL1434;} else{ goto _LL1435;} _LL1435: if(( unsigned int)
_temp1429 >  1u?*(( int*) _temp1429) ==  Cyc_Absyn_Decl_s: 0){ _LL1448:
_temp1447=(( struct Cyc_Absyn_Decl_s_struct*) _temp1429)->f1; goto _LL1446;
_LL1446: _temp1445=(( struct Cyc_Absyn_Decl_s_struct*) _temp1429)->f2; goto
_LL1436;} else{ goto _LL1437;} _LL1437: goto _LL1438; _LL1432: return( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1439->topt))->v; _LL1434: s= _temp1441;
continue; _LL1436: s= _temp1445; continue; _LL1438: return({ void* _temp1449[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u), _tag_arr( _temp1449, sizeof( void*), 0u));});
_LL1430:;}} static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return({
void* _temp1450[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u), _tag_arr( _temp1450, sizeof( void*), 0u));});}
static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ return({ void* _temp1451[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u),
_tag_arr( _temp1451, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){
void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle !=  0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1462=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1462[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1463; _temp1463.tag= Cyc_Absyn_RgnHandleType;
_temp1463.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt* _temp1464=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1464->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1464;}));
_temp1463;}); _temp1462;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle)); void*
_temp1452= Cyc_Tcutil_compress( handle_type); void* _temp1458; _LL1454: if((
unsigned int) _temp1452 >  4u?*(( int*) _temp1452) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1459: _temp1458=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1452)->f1; goto _LL1455;} else{ goto _LL1456;} _LL1456: goto _LL1457;
_LL1455: rgn= _temp1458; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1453; _LL1457:({ struct Cyc_Std_String_pa_struct _temp1461; _temp1461.tag=
Cyc_Std_String_pa; _temp1461.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1460[ 1u]={& _temp1461}; Cyc_Tcutil_terr( rgn_handle->loc,
_tag_arr("expecting region_t type but found %s", sizeof( unsigned char), 37u),
_tag_arr( _temp1460, sizeof( void*), 1u));}}); goto _LL1453; _LL1453:;}{ void*
_temp1465=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1477; struct Cyc_Absyn_Exp*
_temp1479; struct Cyc_Absyn_Vardecl* _temp1481; struct Cyc_List_List* _temp1483;
struct Cyc_Core_Opt* _temp1485; struct Cyc_List_List* _temp1487; void* _temp1489;
struct _tagged_arr _temp1491; _LL1467: if(*(( int*) _temp1465) ==  Cyc_Absyn_Comprehension_e){
_LL1482: _temp1481=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1465)->f1;
goto _LL1480; _LL1480: _temp1479=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1465)->f2; goto _LL1478; _LL1478: _temp1477=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1465)->f3; goto _LL1468;} else{ goto _LL1469;} _LL1469: if(*(( int*)
_temp1465) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1486: _temp1485=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1465)->f1; goto _LL1484; _LL1484: _temp1483=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1465)->f2; goto _LL1470;} else{ goto _LL1471;} _LL1471: if(*(( int*)
_temp1465) ==  Cyc_Absyn_Array_e){ _LL1488: _temp1487=(( struct Cyc_Absyn_Array_e_struct*)
_temp1465)->f1; goto _LL1472;} else{ goto _LL1473;} _LL1473: if(*(( int*)
_temp1465) ==  Cyc_Absyn_Const_e){ _LL1490: _temp1489=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1465)->f1; if(( unsigned int) _temp1489 >  1u?*(( int*) _temp1489) ==  Cyc_Absyn_String_c:
0){ _LL1492: _temp1491=(( struct Cyc_Absyn_String_c_struct*) _temp1489)->f1;
goto _LL1474;} else{ goto _LL1475;}} else{ goto _LL1475;} _LL1475: goto _LL1476;
_LL1468: { void* _temp1493= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1494= Cyc_Tcutil_compress( _temp1493); struct Cyc_Absyn_Tqual _temp1500;
void* _temp1502; _LL1496: if(( unsigned int) _temp1494 >  4u?*(( int*) _temp1494)
==  Cyc_Absyn_ArrayType: 0){ _LL1503: _temp1502=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1494)->f1; goto _LL1501; _LL1501: _temp1500=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1494)->f2; goto _LL1497;} else{ goto _LL1498;} _LL1498: goto _LL1499;
_LL1497: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1479)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1508=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1508[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1509; _temp1509.tag= Cyc_Absyn_Upper_b; _temp1509.f1=
_temp1479; _temp1509;}); _temp1508;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1505=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1505[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1506; _temp1506.tag= Cyc_Absyn_PointerType;
_temp1506.f1=({ struct Cyc_Absyn_PtrInfo _temp1507; _temp1507.elt_typ=( void*)
_temp1502; _temp1507.rgn_typ=( void*) rgn; _temp1507.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1507.tq= _temp1500; _temp1507.bounds= Cyc_Absyn_new_conref(
b); _temp1507;}); _temp1506;}); _temp1505;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*
topt, res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1504=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1504->v=( void*) res_typ; _temp1504;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_typ=* topt;}} return res_typ;} _LL1499: return({ void*
_temp1510[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u), _tag_arr( _temp1510, sizeof( void*), 0u));});
_LL1495:;} _LL1470:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1511=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1511[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1512; _temp1512.tag= Cyc_Absyn_Array_e;
_temp1512.f1= _temp1483; _temp1512;}); _temp1511;}))); _temp1487= _temp1483;
goto _LL1472; _LL1472: { void** elt_typ_opt= 0; if( topt !=  0){ void* _temp1513=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp1519; struct Cyc_Absyn_Tqual
_temp1521; void* _temp1523; void** _temp1525; _LL1515: if(( unsigned int)
_temp1513 >  4u?*(( int*) _temp1513) ==  Cyc_Absyn_PointerType: 0){ _LL1520:
_temp1519=(( struct Cyc_Absyn_PointerType_struct*) _temp1513)->f1; _LL1524:
_temp1523=( void*) _temp1519.elt_typ; _temp1525=( void**)&((( struct Cyc_Absyn_PointerType_struct*)
_temp1513)->f1).elt_typ; goto _LL1522; _LL1522: _temp1521= _temp1519.tq; goto
_LL1516;} else{ goto _LL1517;} _LL1517: goto _LL1518; _LL1516: elt_typ_opt=(
void**) _temp1525; goto _LL1514; _LL1518: goto _LL1514; _LL1514:;}{ void*
_temp1526= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1527= Cyc_Tcutil_compress( _temp1526); struct Cyc_Absyn_Exp* _temp1533;
struct Cyc_Absyn_Tqual _temp1535; void* _temp1537; _LL1529: if(( unsigned int)
_temp1527 >  4u?*(( int*) _temp1527) ==  Cyc_Absyn_ArrayType: 0){ _LL1538:
_temp1537=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1527)->f1; goto
_LL1536; _LL1536: _temp1535=(( struct Cyc_Absyn_ArrayType_struct*) _temp1527)->f2;
goto _LL1534; _LL1534: _temp1533=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1527)->f3; goto _LL1530;} else{ goto _LL1531;} _LL1531: goto _LL1532;
_LL1530: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1539=(
struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1539[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1540; _temp1540.tag=
Cyc_Absyn_PointerType; _temp1540.f1=({ struct Cyc_Absyn_PtrInfo _temp1541;
_temp1541.elt_typ=( void*) _temp1537; _temp1541.rgn_typ=( void*) rgn; _temp1541.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1541.tq=
_temp1535; _temp1541.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1542=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1542[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1543; _temp1543.tag= Cyc_Absyn_Upper_b;
_temp1543.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1533); _temp1543;});
_temp1542;})); _temp1541;}); _temp1540;}); _temp1539;}); if( topt !=  0){ if( !
Cyc_Tcutil_unify(* topt, res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*
topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1544=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1544->v=( void*) res_typ;
_temp1544;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); res_typ=* topt;}} goto
_LL1528; _LL1532: return({ void* _temp1545[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u), _tag_arr( _temp1545, sizeof( void*), 0u));});
_LL1528:;} return res_typ;}} _LL1474: { void* _temp1546= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b); void* _temp1547= Cyc_Tcexp_tcExp(
te,( void**)& _temp1546, e1); return Cyc_Absyn_atb_typ( _temp1547, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1548=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1548[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1549; _temp1549.tag= Cyc_Absyn_Upper_b; _temp1549.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1549;}); _temp1548;}));} _LL1476: { void** topt2=
0; if( topt !=  0){ void* _temp1550= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp1558; struct Cyc_Absyn_Conref* _temp1560; void* _temp1562; void** _temp1564;
_LL1552: if(( unsigned int) _temp1550 >  4u?*(( int*) _temp1550) ==  Cyc_Absyn_PointerType:
0){ _LL1559: _temp1558=(( struct Cyc_Absyn_PointerType_struct*) _temp1550)->f1;
_LL1563: _temp1562=( void*) _temp1558.elt_typ; _temp1564=( void**)&((( struct
Cyc_Absyn_PointerType_struct*) _temp1550)->f1).elt_typ; goto _LL1561; _LL1561:
_temp1560= _temp1558.nullable; goto _LL1553;} else{ goto _LL1554;} _LL1554: if((
unsigned int) _temp1550 >  4u?*(( int*) _temp1550) ==  Cyc_Absyn_TunionType: 0){
goto _LL1555;} else{ goto _LL1556;} _LL1556: goto _LL1557; _LL1553: topt2=( void**)
_temp1564; goto _LL1551; _LL1555: topt2=({ void** _temp1565=( void**) _cycalloc(
sizeof( void*)); _temp1565[ 0]=* topt; _temp1565;}); goto _LL1551; _LL1557: goto
_LL1551; _LL1551:;}{ void* telt= Cyc_Tcexp_tcExp( te, topt2, e1); void* res_typ=(
void*)({ struct Cyc_Absyn_PointerType_struct* _temp1567=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1567[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1568; _temp1568.tag= Cyc_Absyn_PointerType;
_temp1568.f1=({ struct Cyc_Absyn_PtrInfo _temp1569; _temp1569.elt_typ=( void*)
telt; _temp1569.rgn_typ=( void*) rgn; _temp1569.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1569.tq= Cyc_Absyn_empty_tqual(); _temp1569.bounds=
Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1570=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1570[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1571; _temp1571.tag= Cyc_Absyn_Upper_b;
_temp1571.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1571;}); _temp1570;})); _temp1569;});
_temp1568;}); _temp1567;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(* topt,
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1566=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1566->v=( void*) res_typ; _temp1566;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_typ=* topt;}} return res_typ;}} _LL1466:;}} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote( te, topt, e)); void* _temp1572= t; struct Cyc_Absyn_Exp*
_temp1578; struct Cyc_Absyn_Tqual _temp1580; void* _temp1582; _LL1574: if((
unsigned int) _temp1572 >  4u?*(( int*) _temp1572) ==  Cyc_Absyn_ArrayType: 0){
_LL1583: _temp1582=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1572)->f1;
goto _LL1581; _LL1581: _temp1580=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1572)->f2; goto _LL1579; _LL1579: _temp1578=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1572)->f3; goto _LL1575;} else{ goto _LL1576;} _LL1576: goto _LL1577;
_LL1575: { void* _temp1586; struct _tuple6 _temp1584= Cyc_Tcutil_addressof_props(
te, e); _LL1587: _temp1586= _temp1584.f2; goto _LL1585; _LL1585: { void*
_temp1588= _temp1578 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1589=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1589[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1590; _temp1590.tag= Cyc_Absyn_Upper_b;
_temp1590.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1578); _temp1590;});
_temp1589;}); t= Cyc_Absyn_atb_typ( _temp1582, _temp1586, _temp1580, _temp1588);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1577: return t; _LL1573:;} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1591=( void*) e->r; void* _temp1601; _LL1593: if(*(( int*)
_temp1591) ==  Cyc_Absyn_Array_e){ goto _LL1594;} else{ goto _LL1595;} _LL1595:
if(*(( int*) _temp1591) ==  Cyc_Absyn_Comprehension_e){ goto _LL1596;} else{
goto _LL1597;} _LL1597: if(*(( int*) _temp1591) ==  Cyc_Absyn_Const_e){ _LL1602:
_temp1601=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1591)->f1; if((
unsigned int) _temp1601 >  1u?*(( int*) _temp1601) ==  Cyc_Absyn_String_c: 0){
goto _LL1598;} else{ goto _LL1599;}} else{ goto _LL1599;} _LL1599: goto _LL1600;
_LL1594: return t; _LL1596: return t; _LL1598: return t; _LL1600: t= Cyc_Tcutil_compress(
t);{ void* _temp1603= t; struct Cyc_Absyn_Exp* _temp1609; struct Cyc_Absyn_Tqual
_temp1611; void* _temp1613; _LL1605: if(( unsigned int) _temp1603 >  4u?*(( int*)
_temp1603) ==  Cyc_Absyn_ArrayType: 0){ _LL1614: _temp1613=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1603)->f1; goto _LL1612; _LL1612: _temp1611=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1603)->f2; goto _LL1610; _LL1610: _temp1609=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1603)->f3; goto _LL1606;} else{ goto _LL1607;} _LL1607: goto _LL1608;
_LL1606: { void* _temp1617; struct _tuple6 _temp1615= Cyc_Tcutil_addressof_props(
te, e); _LL1618: _temp1617= _temp1615.f2; goto _LL1616; _LL1616: { void* b=
_temp1609 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1619=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1619[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1620; _temp1620.tag= Cyc_Absyn_Upper_b;
_temp1620.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1609); _temp1620;});
_temp1619;}); t= Cyc_Absyn_atb_typ( _temp1613, _temp1617, _temp1611, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1608: return t; _LL1604:;} _LL1592:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1621=( void*) e->r; struct Cyc_Absyn_Exp* _temp1627; _LL1623: if(*(( int*)
_temp1621) ==  Cyc_Absyn_NoInstantiate_e){ _LL1628: _temp1627=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1621)->f1; goto _LL1624;} else{ goto _LL1625;} _LL1625: goto _LL1626;
_LL1624: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1627);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1627->topt))->v=( void*) Cyc_Absyn_pointer_expand(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1627->topt))->v)); e->topt= _temp1627->topt;
goto _LL1622; _LL1626: Cyc_Tcexp_tcExpNoInst( te, topt, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1629=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct Cyc_Absyn_PtrInfo _temp1635; struct Cyc_Absyn_Conref* _temp1637; struct
Cyc_Absyn_Tqual _temp1639; struct Cyc_Absyn_Conref* _temp1641; void* _temp1643;
void* _temp1645; _LL1631: if(( unsigned int) _temp1629 >  4u?*(( int*) _temp1629)
==  Cyc_Absyn_PointerType: 0){ _LL1636: _temp1635=(( struct Cyc_Absyn_PointerType_struct*)
_temp1629)->f1; _LL1646: _temp1645=( void*) _temp1635.elt_typ; goto _LL1644;
_LL1644: _temp1643=( void*) _temp1635.rgn_typ; goto _LL1642; _LL1642: _temp1641=
_temp1635.nullable; goto _LL1640; _LL1640: _temp1639= _temp1635.tq; goto _LL1638;
_LL1638: _temp1637= _temp1635.bounds; goto _LL1632;} else{ goto _LL1633;}
_LL1633: goto _LL1634; _LL1632:{ void* _temp1647= Cyc_Tcutil_compress( _temp1645);
struct Cyc_Absyn_FnInfo _temp1653; struct Cyc_List_List* _temp1655; struct Cyc_List_List*
_temp1657; struct Cyc_Absyn_VarargInfo* _temp1659; int _temp1661; struct Cyc_List_List*
_temp1663; void* _temp1665; struct Cyc_Core_Opt* _temp1667; struct Cyc_List_List*
_temp1669; _LL1649: if(( unsigned int) _temp1647 >  4u?*(( int*) _temp1647) == 
Cyc_Absyn_FnType: 0){ _LL1654: _temp1653=(( struct Cyc_Absyn_FnType_struct*)
_temp1647)->f1; _LL1670: _temp1669= _temp1653.tvars; goto _LL1668; _LL1668:
_temp1667= _temp1653.effect; goto _LL1666; _LL1666: _temp1665=( void*) _temp1653.ret_typ;
goto _LL1664; _LL1664: _temp1663= _temp1653.args; goto _LL1662; _LL1662:
_temp1661= _temp1653.c_varargs; goto _LL1660; _LL1660: _temp1659= _temp1653.cyc_varargs;
goto _LL1658; _LL1658: _temp1657= _temp1653.rgn_po; goto _LL1656; _LL1656:
_temp1655= _temp1653.attributes; goto _LL1650;} else{ goto _LL1651;} _LL1651:
goto _LL1652; _LL1650: if( _temp1669 !=  0){ struct _RegionHandle _temp1671=
_new_region(); struct _RegionHandle* rgn=& _temp1671; _push_region( rgn);{
struct _tuple4 _temp1672=({ struct _tuple4 _temp1682; _temp1682.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1682.f2= rgn; _temp1682;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1672, _temp1669); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1679=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1679[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1680; _temp1680.tag= Cyc_Absyn_FnType; _temp1680.f1=({ struct Cyc_Absyn_FnInfo
_temp1681; _temp1681.tvars= 0; _temp1681.effect= _temp1667; _temp1681.ret_typ=(
void*) _temp1665; _temp1681.args= _temp1663; _temp1681.c_varargs= _temp1661;
_temp1681.cyc_varargs= _temp1659; _temp1681.rgn_po= _temp1657; _temp1681.attributes=
_temp1655; _temp1681;}); _temp1680;}); _temp1679;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1676=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1676[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1677; _temp1677.tag= Cyc_Absyn_PointerType;
_temp1677.f1=({ struct Cyc_Absyn_PtrInfo _temp1678; _temp1678.elt_typ=( void*)
ftyp; _temp1678.rgn_typ=( void*) _temp1643; _temp1678.nullable= _temp1641;
_temp1678.tq= _temp1639; _temp1678.bounds= _temp1637; _temp1678;}); _temp1677;});
_temp1676;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1673=( struct Cyc_Absyn_Instantiate_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1673[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1674; _temp1674.tag= Cyc_Absyn_Instantiate_e;
_temp1674.f1= inner; _temp1674.f2= ts; _temp1674;}); _temp1673;}))); e->topt=({
struct Cyc_Core_Opt* _temp1675=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1675->v=( void*) new_typ; _temp1675;});}; _pop_region( rgn);}
goto _LL1648; _LL1652: goto _LL1648; _LL1648:;} goto _LL1630; _LL1634: goto
_LL1630; _LL1630:;} goto _LL1622; _LL1622:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1683=( void*) e->r; struct Cyc_Absyn_Exp* _temp1761; struct
_tuple1* _temp1763; struct Cyc_List_List* _temp1765; struct Cyc_Absyn_Exp*
_temp1767; struct Cyc_List_List* _temp1769; struct Cyc_Core_Opt* _temp1771; void*
_temp1773; void* _temp1775; struct _tuple1* _temp1777; struct Cyc_List_List*
_temp1779; void* _temp1781; void* _temp1783; struct Cyc_Absyn_Exp* _temp1785;
struct Cyc_Absyn_Exp* _temp1787; struct Cyc_Core_Opt* _temp1789; struct Cyc_Absyn_Exp*
_temp1791; struct Cyc_Absyn_Exp* _temp1793; struct Cyc_Absyn_Exp* _temp1795;
struct Cyc_Absyn_Exp* _temp1797; struct Cyc_Absyn_Exp* _temp1799; struct Cyc_Absyn_Exp*
_temp1801; struct Cyc_Absyn_VarargCallInfo* _temp1803; struct Cyc_Absyn_VarargCallInfo**
_temp1805; struct Cyc_List_List* _temp1806; struct Cyc_Absyn_Exp* _temp1808;
struct Cyc_Absyn_Exp* _temp1810; struct Cyc_List_List* _temp1812; struct Cyc_Absyn_Exp*
_temp1814; struct Cyc_Absyn_Exp* _temp1816; void* _temp1818; struct Cyc_Absyn_Exp*
_temp1820; struct Cyc_Absyn_Exp* _temp1822; struct Cyc_Absyn_Exp* _temp1824;
struct Cyc_Absyn_Exp* _temp1826; void* _temp1828; void* _temp1830; void*
_temp1832; struct Cyc_Absyn_Exp* _temp1834; struct _tagged_arr* _temp1836;
struct Cyc_Absyn_Exp* _temp1838; struct _tagged_arr* _temp1840; struct Cyc_Absyn_Exp*
_temp1842; struct Cyc_Absyn_Exp* _temp1844; struct Cyc_Absyn_Exp* _temp1846;
struct Cyc_List_List* _temp1848; struct Cyc_List_List* _temp1850; struct _tuple2*
_temp1852; struct Cyc_List_List* _temp1854; struct Cyc_Absyn_Stmt* _temp1856;
struct Cyc_Absyn_Fndecl* _temp1858; struct Cyc_Absyn_Exp* _temp1860; struct Cyc_Absyn_Exp*
_temp1862; struct Cyc_Absyn_Exp* _temp1864; struct Cyc_Absyn_Vardecl* _temp1866;
struct Cyc_Absyn_Structdecl* _temp1868; struct Cyc_Absyn_Structdecl** _temp1870;
struct Cyc_List_List* _temp1871; struct Cyc_Core_Opt* _temp1873; struct Cyc_Core_Opt**
_temp1875; struct _tuple1* _temp1876; struct _tuple1** _temp1878; struct Cyc_List_List*
_temp1879; void* _temp1881; struct Cyc_Absyn_Tunionfield* _temp1883; struct Cyc_Absyn_Tuniondecl*
_temp1885; struct Cyc_List_List* _temp1887; struct Cyc_Core_Opt* _temp1889;
struct Cyc_Core_Opt** _temp1891; struct Cyc_Core_Opt* _temp1892; struct Cyc_Core_Opt**
_temp1894; struct Cyc_Absyn_Enumfield* _temp1895; struct Cyc_Absyn_Enumdecl*
_temp1897; struct _tuple1* _temp1899; struct _tuple1** _temp1901; struct Cyc_Absyn_Enumfield*
_temp1902; void* _temp1904; struct _tuple1* _temp1906; struct _tuple1**
_temp1908; void* _temp1909; struct Cyc_Absyn_Exp* _temp1911; _LL1685: if(*(( int*)
_temp1683) ==  Cyc_Absyn_NoInstantiate_e){ _LL1762: _temp1761=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1683)->f1; goto _LL1686;} else{ goto _LL1687;} _LL1687: if(*(( int*)
_temp1683) ==  Cyc_Absyn_UnknownId_e){ _LL1764: _temp1763=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1683)->f1; goto _LL1688;} else{ goto _LL1689;} _LL1689: if(*(( int*)
_temp1683) ==  Cyc_Absyn_UnknownCall_e){ _LL1768: _temp1767=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1683)->f1; goto _LL1766; _LL1766: _temp1765=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1683)->f2; goto _LL1690;} else{ goto _LL1691;} _LL1691: if(*(( int*)
_temp1683) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1772: _temp1771=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1683)->f1; goto _LL1770; _LL1770: _temp1769=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1683)->f2; goto _LL1692;} else{ goto _LL1693;} _LL1693: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Const_e){ _LL1774: _temp1773=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1683)->f1; goto _LL1694;} else{ goto _LL1695;} _LL1695: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Var_e){ _LL1778: _temp1777=(( struct Cyc_Absyn_Var_e_struct*)
_temp1683)->f1; goto _LL1776; _LL1776: _temp1775=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1683)->f2; goto _LL1696;} else{ goto _LL1697;} _LL1697: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Primop_e){ _LL1782: _temp1781=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1683)->f1; goto _LL1780; _LL1780: _temp1779=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1683)->f2; goto _LL1698;} else{ goto _LL1699;} _LL1699: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Increment_e){ _LL1786: _temp1785=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1683)->f1; goto _LL1784; _LL1784: _temp1783=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1683)->f2; goto _LL1700;} else{ goto _LL1701;} _LL1701: if(*(( int*)
_temp1683) ==  Cyc_Absyn_AssignOp_e){ _LL1792: _temp1791=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1683)->f1; goto _LL1790; _LL1790: _temp1789=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1683)->f2; goto _LL1788; _LL1788: _temp1787=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1683)->f3; goto _LL1702;} else{ goto _LL1703;} _LL1703: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Conditional_e){ _LL1798: _temp1797=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1683)->f1; goto _LL1796; _LL1796: _temp1795=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1683)->f2; goto _LL1794; _LL1794: _temp1793=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1683)->f3; goto _LL1704;} else{ goto _LL1705;} _LL1705: if(*(( int*)
_temp1683) ==  Cyc_Absyn_SeqExp_e){ _LL1802: _temp1801=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1683)->f1; goto _LL1800; _LL1800: _temp1799=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1683)->f2; goto _LL1706;} else{ goto _LL1707;} _LL1707: if(*(( int*)
_temp1683) ==  Cyc_Absyn_FnCall_e){ _LL1809: _temp1808=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1683)->f1; goto _LL1807; _LL1807: _temp1806=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1683)->f2; goto _LL1804; _LL1804: _temp1803=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1683)->f3; _temp1805=( struct Cyc_Absyn_VarargCallInfo**)&(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1683)->f3; goto _LL1708;} else{ goto _LL1709;} _LL1709: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Throw_e){ _LL1811: _temp1810=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1683)->f1; goto _LL1710;} else{ goto _LL1711;} _LL1711: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Instantiate_e){ _LL1815: _temp1814=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1683)->f1; goto _LL1813; _LL1813: _temp1812=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1683)->f2; goto _LL1712;} else{ goto _LL1713;} _LL1713: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Cast_e){ _LL1819: _temp1818=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1683)->f1; goto _LL1817; _LL1817: _temp1816=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1683)->f2; goto _LL1714;} else{ goto _LL1715;} _LL1715: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Address_e){ _LL1821: _temp1820=(( struct Cyc_Absyn_Address_e_struct*)
_temp1683)->f1; goto _LL1716;} else{ goto _LL1717;} _LL1717: if(*(( int*)
_temp1683) ==  Cyc_Absyn_New_e){ _LL1825: _temp1824=(( struct Cyc_Absyn_New_e_struct*)
_temp1683)->f1; goto _LL1823; _LL1823: _temp1822=(( struct Cyc_Absyn_New_e_struct*)
_temp1683)->f2; goto _LL1718;} else{ goto _LL1719;} _LL1719: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Sizeofexp_e){ _LL1827: _temp1826=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1683)->f1; goto _LL1720;} else{ goto _LL1721;} _LL1721: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1829: _temp1828=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1683)->f1; goto _LL1722;} else{ goto _LL1723;} _LL1723: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Offsetof_e){ _LL1833: _temp1832=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1683)->f1; goto _LL1831; _LL1831: _temp1830=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1683)->f2; goto _LL1724;} else{ goto _LL1725;} _LL1725: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Gentyp_e){ goto _LL1726;} else{ goto _LL1727;} _LL1727:
if(*(( int*) _temp1683) ==  Cyc_Absyn_Deref_e){ _LL1835: _temp1834=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1683)->f1; goto _LL1728;} else{ goto _LL1729;} _LL1729: if(*(( int*)
_temp1683) ==  Cyc_Absyn_StructMember_e){ _LL1839: _temp1838=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1683)->f1; goto _LL1837; _LL1837: _temp1836=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1683)->f2; goto _LL1730;} else{ goto _LL1731;} _LL1731: if(*(( int*)
_temp1683) ==  Cyc_Absyn_StructArrow_e){ _LL1843: _temp1842=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1683)->f1; goto _LL1841; _LL1841: _temp1840=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1683)->f2; goto _LL1732;} else{ goto _LL1733;} _LL1733: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Subscript_e){ _LL1847: _temp1846=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1683)->f1; goto _LL1845; _LL1845: _temp1844=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1683)->f2; goto _LL1734;} else{ goto _LL1735;} _LL1735: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Tuple_e){ _LL1849: _temp1848=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1683)->f1; goto _LL1736;} else{ goto _LL1737;} _LL1737: if(*(( int*)
_temp1683) ==  Cyc_Absyn_CompoundLit_e){ _LL1853: _temp1852=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1683)->f1; goto _LL1851; _LL1851: _temp1850=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1683)->f2; goto _LL1738;} else{ goto _LL1739;} _LL1739: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Array_e){ _LL1855: _temp1854=(( struct Cyc_Absyn_Array_e_struct*)
_temp1683)->f1; goto _LL1740;} else{ goto _LL1741;} _LL1741: if(*(( int*)
_temp1683) ==  Cyc_Absyn_StmtExp_e){ _LL1857: _temp1856=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1683)->f1; goto _LL1742;} else{ goto _LL1743;} _LL1743: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Codegen_e){ _LL1859: _temp1858=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1683)->f1; goto _LL1744;} else{ goto _LL1745;} _LL1745: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Fill_e){ _LL1861: _temp1860=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1683)->f1; goto _LL1746;} else{ goto _LL1747;} _LL1747: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Comprehension_e){ _LL1867: _temp1866=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1683)->f1; goto _LL1865; _LL1865: _temp1864=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1683)->f2; goto _LL1863; _LL1863: _temp1862=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1683)->f3; goto _LL1748;} else{ goto _LL1749;} _LL1749: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Struct_e){ _LL1877: _temp1876=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1683)->f1; _temp1878=( struct _tuple1**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1683)->f1; goto _LL1874; _LL1874: _temp1873=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1683)->f2; _temp1875=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1683)->f2; goto _LL1872; _LL1872: _temp1871=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1683)->f3; goto _LL1869; _LL1869: _temp1868=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1683)->f4; _temp1870=( struct Cyc_Absyn_Structdecl**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1683)->f4; goto _LL1750;} else{ goto _LL1751;} _LL1751: if(*(( int*)
_temp1683) ==  Cyc_Absyn_AnonStruct_e){ _LL1882: _temp1881=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1683)->f1; goto _LL1880; _LL1880: _temp1879=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1683)->f2; goto _LL1752;} else{ goto _LL1753;} _LL1753: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Tunion_e){ _LL1893: _temp1892=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1683)->f1; _temp1894=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1683)->f1; goto _LL1890; _LL1890: _temp1889=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1683)->f2; _temp1891=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1683)->f2; goto _LL1888; _LL1888: _temp1887=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1683)->f3; goto _LL1886; _LL1886: _temp1885=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1683)->f4; goto _LL1884; _LL1884: _temp1883=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1683)->f5; goto _LL1754;} else{ goto _LL1755;} _LL1755: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Enum_e){ _LL1900: _temp1899=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1683)->f1; _temp1901=( struct _tuple1**)&(( struct Cyc_Absyn_Enum_e_struct*)
_temp1683)->f1; goto _LL1898; _LL1898: _temp1897=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1683)->f2; goto _LL1896; _LL1896: _temp1895=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1683)->f3; goto _LL1756;} else{ goto _LL1757;} _LL1757: if(*(( int*)
_temp1683) ==  Cyc_Absyn_AnonEnum_e){ _LL1907: _temp1906=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1683)->f1; _temp1908=( struct _tuple1**)&(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1683)->f1; goto _LL1905; _LL1905: _temp1904=( void*)(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1683)->f2; goto _LL1903; _LL1903: _temp1902=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1683)->f3; goto _LL1758;} else{ goto _LL1759;} _LL1759: if(*(( int*)
_temp1683) ==  Cyc_Absyn_Malloc_e){ _LL1912: _temp1911=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1683)->f1; goto _LL1910; _LL1910: _temp1909=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1683)->f2; goto _LL1760;} else{ goto _LL1684;} _LL1686: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1761); return; _LL1688: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1763); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1690: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1767, _temp1765); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1692: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1769); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1694: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1773, e);
goto _LL1684; _LL1696: t= Cyc_Tcexp_tcVar( te, loc, _temp1777, _temp1775); goto
_LL1684; _LL1698: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1781, _temp1779);
goto _LL1684; _LL1700: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1785,
_temp1783); goto _LL1684; _LL1702: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1791, _temp1789, _temp1787); goto _LL1684; _LL1704: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1797, _temp1795, _temp1793); goto _LL1684; _LL1706: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1801, _temp1799); goto _LL1684; _LL1708: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1808, _temp1806, _temp1805); goto _LL1684; _LL1710: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1810); goto _LL1684; _LL1712: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1814, _temp1812); goto _LL1684; _LL1714: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1818, _temp1816); goto _LL1684; _LL1716: t= Cyc_Tcexp_tcAddress(
te, loc, e, topt, _temp1820); goto _LL1684; _LL1718: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1824, e, _temp1822); goto _LL1684; _LL1720: { void* _temp1913= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1826); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1913); goto
_LL1684;} _LL1722: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1828); goto
_LL1684; _LL1724: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1832, _temp1830);
goto _LL1684; _LL1726:({ void* _temp1914[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("gen() not in top-level initializer",
sizeof( unsigned char), 35u), _tag_arr( _temp1914, sizeof( void*), 0u));});
return; _LL1728: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1834); goto _LL1684;
_LL1730: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1838, _temp1836);
goto _LL1684; _LL1732: t= Cyc_Tcexp_tcStructArrow( te, loc, topt, _temp1842,
_temp1840); goto _LL1684; _LL1734: t= Cyc_Tcexp_tcSubscript( te, loc, topt,
_temp1846, _temp1844); goto _LL1684; _LL1736: t= Cyc_Tcexp_tcTuple( te, loc,
topt, _temp1848); goto _LL1684; _LL1738: t= Cyc_Tcexp_tcCompoundLit( te, loc,
topt, _temp1852, _temp1850); goto _LL1684; _LL1740: { void** elt_topt= 0; if(
topt !=  0){ void* _temp1915= Cyc_Tcutil_compress(* topt); void* _temp1921; void**
_temp1923; _LL1917: if(( unsigned int) _temp1915 >  4u?*(( int*) _temp1915) == 
Cyc_Absyn_ArrayType: 0){ _LL1922: _temp1921=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1915)->f1; _temp1923=( void**)&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1915)->f1); goto _LL1918;} else{ goto _LL1919;} _LL1919: goto _LL1920;
_LL1918: elt_topt=( void**) _temp1923; goto _LL1916; _LL1920: goto _LL1916;
_LL1916:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp1854); goto _LL1684;}
_LL1742: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp1856); goto _LL1684;
_LL1744: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp1858); goto _LL1684;
_LL1746: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1860); goto _LL1684; _LL1748:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1866, _temp1864, _temp1862);
goto _LL1684; _LL1750: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp1878,
_temp1875, _temp1871, _temp1870); goto _LL1684; _LL1752: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1881, _temp1879); goto _LL1684; _LL1754: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1894, _temp1891, _temp1887, _temp1885, _temp1883); goto
_LL1684; _LL1756:* _temp1901=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1895))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1924=(
struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1924[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1925; _temp1925.tag= Cyc_Absyn_EnumType;
_temp1925.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1897))->name;
_temp1925.f2= _temp1897; _temp1925;}); _temp1924;}); goto _LL1684; _LL1758:*
_temp1908=(( struct Cyc_Absyn_Enumfield*) _check_null( _temp1902))->name; t=
_temp1904; goto _LL1684; _LL1760: t= Cyc_Tcexp_tcMalloc( te, loc, topt,
_temp1911, _temp1909); goto _LL1684; _LL1684:;} e->topt=({ struct Cyc_Core_Opt*
_temp1926=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1926->v=( void*) t; _temp1926;});}
