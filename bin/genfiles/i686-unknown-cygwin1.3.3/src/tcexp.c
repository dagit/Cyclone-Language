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
const int Cyc_Absyn_DoubleType= 6; struct Cyc_Absyn_DoubleType_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_ArrayType= 7; struct Cyc_Absyn_ArrayType_struct{
int tag; void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ;
static const int Cyc_Absyn_FnType= 8; struct Cyc_Absyn_FnType_struct{ int tag;
struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 9; struct
Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static const
int Cyc_Absyn_StructType= 10; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 11; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_EnumType= 12; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_AnonStructType= 13; struct Cyc_Absyn_AnonStructType_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType= 14;
struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_AnonEnumType= 15; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
16; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 17; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 2; static const int Cyc_Absyn_AccessEff= 18;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
19; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 20; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
struct _tuple1* _temp129; _LL113: if(( unsigned int) _temp111 >  3u?*(( int*)
_temp111) ==  Cyc_Absyn_ArrayType: 0){ _LL124: _temp123=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp111)->f1; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_ArrayType_struct*)
_temp111)->f2; goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp111 >  3u?*(( int*) _temp111) ==  Cyc_Absyn_StructType: 0){ _LL130:
_temp129=(( struct Cyc_Absyn_StructType_struct*) _temp111)->f1; goto _LL128;
_LL128: _temp127=(( struct Cyc_Absyn_StructType_struct*) _temp111)->f2; goto
_LL126; _LL126: _temp125=(( struct Cyc_Absyn_StructType_struct*) _temp111)->f3;
goto _LL116;} else{ goto _LL117;} _LL117: if(( unsigned int) _temp111 >  3u?*((
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
unsigned int) _temp171 >  3u?*(( int*) _temp171) ==  Cyc_Absyn_IntType: 0){
_LL186: _temp185=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1;
goto _LL184; _LL184: _temp183=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f2; if( _temp183 == ( void*) Cyc_Absyn_B1){ goto _LL174;} else{ goto
_LL175;}} else{ goto _LL175;} _LL175: if(( unsigned int) _temp171 >  3u?*(( int*)
_temp171) ==  Cyc_Absyn_IntType: 0){ _LL190: _temp189=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f1; goto _LL188; _LL188: _temp187=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f2; if( _temp187 == ( void*) Cyc_Absyn_B2){ goto _LL176;} else{ goto
_LL177;}} else{ goto _LL177;} _LL177: if(( unsigned int) _temp171 >  3u?*(( int*)
_temp171) ==  Cyc_Absyn_IntType: 0){ _LL192: _temp191=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f1; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp171 >  3u?*(( int*) _temp171) ==  Cyc_Absyn_PointerType: 0){ goto _LL193;}
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
void* _temp236; void* _temp238; _LL216: if(( unsigned int) _temp214 >  3u?*((
int*) _temp214) ==  Cyc_Absyn_ArrayType: 0){ _LL227: _temp226=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp214)->f1; goto _LL225; _LL225: _temp224=((
struct Cyc_Absyn_ArrayType_struct*) _temp214)->f2; goto _LL223; _LL223: _temp222=((
struct Cyc_Absyn_ArrayType_struct*) _temp214)->f3; goto _LL217;} else{ goto
_LL218;} _LL218: if(( unsigned int) _temp214 >  3u?*(( int*) _temp214) ==  Cyc_Absyn_PointerType:
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
_temp330 >  3u?*(( int*) _temp330) ==  Cyc_Absyn_ArrayType: 0){ goto _LL333;}
else{ goto _LL334;} _LL334: if(( unsigned int) _temp330 >  3u?*(( int*) _temp330)
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
_LL351: if(( unsigned int) _temp349 >  3u?*(( int*) _temp349) ==  Cyc_Absyn_PointerType:
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
unsigned int) _temp409 >  3u?*(( int*) _temp409) ==  Cyc_Absyn_PointerType: 0){
_LL412: _temp411=(( struct Cyc_Absyn_PointerType_struct*) _temp409)->f1; _LL414:
_temp413=( void*) _temp411.elt_typ; goto _LL404;} else{ goto _LL401;} _LL404:
_temp403= _temp397.f2; if(( unsigned int) _temp403 >  3u?*(( int*) _temp403) == 
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
_LL526: if(( unsigned int) _temp524 >  3u?*(( int*) _temp524) ==  Cyc_Absyn_PointerType:
0){ _LL535: _temp534=(( struct Cyc_Absyn_PointerType_struct*) _temp524)->f1;
_LL537: _temp536= _temp534.tq; goto _LL527;} else{ goto _LL528;} _LL528: if((
unsigned int) _temp524 >  3u?*(( int*) _temp524) ==  Cyc_Absyn_ArrayType: 0){
_LL539: _temp538=(( struct Cyc_Absyn_ArrayType_struct*) _temp524)->f2; goto
_LL529;} else{ goto _LL530;} _LL530: if(( unsigned int) _temp524 >  3u?*(( int*)
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
Cyc_Absyn_Uniondecl** _temp575; _LL559: if(( unsigned int) _temp557 >  3u?*((
int*) _temp557) ==  Cyc_Absyn_StructType: 0){ _LL570: _temp569=(( struct Cyc_Absyn_StructType_struct*)
_temp557)->f3; goto _LL560;} else{ goto _LL561;} _LL561: if(( unsigned int)
_temp557 >  3u?*(( int*) _temp557) ==  Cyc_Absyn_AnonUnionType: 0){ _LL572:
_temp571=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp557)->f1; goto _LL562;}
else{ goto _LL563;} _LL563: if(( unsigned int) _temp557 >  3u?*(( int*) _temp557)
==  Cyc_Absyn_AnonStructType: 0){ _LL574: _temp573=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp557)->f1; goto _LL564;} else{ goto _LL565;} _LL565: if(( unsigned int)
_temp557 >  3u?*(( int*) _temp557) ==  Cyc_Absyn_UnionType: 0){ _LL576: _temp575=((
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
_temp583; void* _temp585; _LL579: if(( unsigned int) _temp577 >  3u?*(( int*)
_temp577) ==  Cyc_Absyn_PointerType: 0){ _LL584: _temp583=(( struct Cyc_Absyn_PointerType_struct*)
_temp577)->f1; _LL586: _temp585=( void*) _temp583.elt_typ; goto _LL580;} else{
goto _LL581;} _LL581: goto _LL582; _LL580:{ void* _temp587= Cyc_Tcutil_compress(
_temp585); struct Cyc_Absyn_Structdecl** _temp599; struct Cyc_Absyn_Uniondecl**
_temp601; struct Cyc_List_List* _temp603; struct Cyc_List_List* _temp605; _LL589:
if(( unsigned int) _temp587 >  3u?*(( int*) _temp587) ==  Cyc_Absyn_StructType:
0){ _LL600: _temp599=(( struct Cyc_Absyn_StructType_struct*) _temp587)->f3; goto
_LL590;} else{ goto _LL591;} _LL591: if(( unsigned int) _temp587 >  3u?*(( int*)
_temp587) ==  Cyc_Absyn_UnionType: 0){ _LL602: _temp601=(( struct Cyc_Absyn_UnionType_struct*)
_temp587)->f3; goto _LL592;} else{ goto _LL593;} _LL593: if(( unsigned int)
_temp587 >  3u?*(( int*) _temp587) ==  Cyc_Absyn_AnonStructType: 0){ _LL604:
_temp603=(( struct Cyc_Absyn_AnonStructType_struct*) _temp587)->f1; goto _LL594;}
else{ goto _LL595;} _LL595: if(( unsigned int) _temp587 >  3u?*(( int*) _temp587)
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
3u?*(( int*) _temp607) ==  Cyc_Absyn_PointerType: 0){ _LL616: _temp615=(( struct
Cyc_Absyn_PointerType_struct*) _temp607)->f1; _LL618: _temp617= _temp615.tq;
goto _LL610;} else{ goto _LL611;} _LL611: if(( unsigned int) _temp607 >  3u?*((
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
Cyc_Tcutil_compress( t1); _LL636: if(( unsigned int) _temp634 >  3u?*(( int*)
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
struct Cyc_Core_Opt*) _check_null( e2->topt))->v;} static struct Cyc_Absyn_DoubleType_struct
Cyc_Tcexp_dbl={ 6u, 0}; static void* Cyc_Tcexp_dbl_t=( void*)& Cyc_Tcexp_dbl;
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;{ void* _temp653= Cyc_Tcutil_compress(
t1); void* _temp663; void* _temp665; _LL655: if( _temp653 == ( void*) Cyc_Absyn_FloatType){
goto _LL656;} else{ goto _LL657;} _LL657: if(( unsigned int) _temp653 >  3u?*((
int*) _temp653) ==  Cyc_Absyn_IntType: 0){ _LL664: _temp663=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp653)->f2; if( _temp663 == ( void*) Cyc_Absyn_B1){
goto _LL658;} else{ goto _LL659;}} else{ goto _LL659;} _LL659: if(( unsigned int)
_temp653 >  3u?*(( int*) _temp653) ==  Cyc_Absyn_IntType: 0){ _LL666: _temp665=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp653)->f2; if( _temp665 == ( void*)
Cyc_Absyn_B2){ goto _LL660;} else{ goto _LL661;}} else{ goto _LL661;} _LL661:
goto _LL662; _LL656: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Tcexp_dbl_t); t1= Cyc_Tcexp_dbl_t;
goto _LL654; _LL658: goto _LL660; _LL660: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_sint_t);
t1= Cyc_Absyn_sint_t; goto _LL654; _LL662: goto _LL654; _LL654:;} for( fields=
fs; fields !=  0; fields= fields->tl){ void* _temp669; struct Cyc_Position_Segment*
_temp671; struct Cyc_List_List* _temp673; struct Cyc_List_List* _temp675; struct
_tuple1* _temp677; struct Cyc_Absyn_Tunionfield _temp667=*(( struct Cyc_Absyn_Tunionfield*)
fields->hd); _LL678: _temp677= _temp667.name; goto _LL676; _LL676: _temp675=
_temp667.tvs; goto _LL674; _LL674: _temp673= _temp667.typs; goto _LL672; _LL672:
_temp671= _temp667.loc; goto _LL670; _LL670: _temp669=( void*) _temp667.sc; goto
_LL668; _LL668: if(( _temp673 ==  0? 1: _temp673->tl !=  0)? 1: _temp675 !=  0){
continue;}{ void* t2= Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*)
_temp673->hd)).f2); if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*) fields->hd);}}} for( fields= fs; fields !=  0;
fields= fields->tl){ void* _temp681; struct Cyc_Position_Segment* _temp683;
struct Cyc_List_List* _temp685; struct Cyc_List_List* _temp687; struct _tuple1*
_temp689; struct Cyc_Absyn_Tunionfield _temp679=*(( struct Cyc_Absyn_Tunionfield*)
fields->hd); _LL690: _temp689= _temp679.name; goto _LL688; _LL688: _temp687=
_temp679.tvs; goto _LL686; _LL686: _temp685= _temp679.typs; goto _LL684; _LL684:
_temp683= _temp679.loc; goto _LL682; _LL682: _temp681=( void*) _temp679.sc; goto
_LL680; _LL680: if(( _temp685 ==  0? 1: _temp685->tl !=  0)? 1: _temp687 !=  0){
continue;}{ void* t2= Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*)
_temp685->hd)).f2); if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*) fields->hd);}}}({ struct Cyc_Std_String_pa_struct
_temp693; _temp693.tag= Cyc_Std_String_pa; _temp693.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ struct Cyc_Std_String_pa_struct _temp692; _temp692.tag= Cyc_Std_String_pa;
_temp692.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( tu);{ void* _temp691[
2u]={& _temp692,& _temp693}; Cyc_Tcutil_terr( e->loc, _tag_arr("can't find a field in %s to inject a value of type %s",
sizeof( unsigned char), 54u), _tag_arr( _temp691, sizeof( void*), 2u));}}});
return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
args, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List*
_temp694= args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{
void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp695= t; struct Cyc_Absyn_PtrInfo _temp701; struct Cyc_Absyn_Conref*
_temp703; struct Cyc_Absyn_Tqual _temp705; struct Cyc_Absyn_Conref* _temp707;
void* _temp709; void* _temp711; _LL697: if(( unsigned int) _temp695 >  3u?*((
int*) _temp695) ==  Cyc_Absyn_PointerType: 0){ _LL702: _temp701=(( struct Cyc_Absyn_PointerType_struct*)
_temp695)->f1; _LL712: _temp711=( void*) _temp701.elt_typ; goto _LL710; _LL710:
_temp709=( void*) _temp701.rgn_typ; goto _LL708; _LL708: _temp707= _temp701.nullable;
goto _LL706; _LL706: _temp705= _temp701.tq; goto _LL704; _LL704: _temp703=
_temp701.bounds; goto _LL698;} else{ goto _LL699;} _LL699: goto _LL700; _LL698:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp709); Cyc_Tcutil_check_nonzero_bound(
loc, _temp703);{ void* _temp713= Cyc_Tcutil_compress( _temp711); struct Cyc_Absyn_FnInfo
_temp719; struct Cyc_List_List* _temp721; struct Cyc_List_List* _temp723; struct
Cyc_Absyn_VarargInfo* _temp725; int _temp727; struct Cyc_List_List* _temp729;
void* _temp731; struct Cyc_Core_Opt* _temp733; struct Cyc_List_List* _temp735;
_LL715: if(( unsigned int) _temp713 >  3u?*(( int*) _temp713) ==  Cyc_Absyn_FnType:
0){ _LL720: _temp719=(( struct Cyc_Absyn_FnType_struct*) _temp713)->f1; _LL736:
_temp735= _temp719.tvars; goto _LL734; _LL734: _temp733= _temp719.effect; goto
_LL732; _LL732: _temp731=( void*) _temp719.ret_typ; goto _LL730; _LL730:
_temp729= _temp719.args; goto _LL728; _LL728: _temp727= _temp719.c_varargs; goto
_LL726; _LL726: _temp725= _temp719.cyc_varargs; goto _LL724; _LL724: _temp723=
_temp719.rgn_po; goto _LL722; _LL722: _temp721= _temp719.attributes; goto _LL716;}
else{ goto _LL717;} _LL717: goto _LL718; _LL716: if( topt !=  0){ Cyc_Tcutil_unify(
_temp731,* topt);} while( _temp694 !=  0? _temp729 !=  0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*) _temp694->hd; void* t2=(*(( struct _tuple2*)
_temp729->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg(
te, e1, t2)){({ struct Cyc_Std_String_pa_struct _temp739; _temp739.tag= Cyc_Std_String_pa;
_temp739.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp738; _temp738.tag= Cyc_Std_String_pa; _temp738.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp737[ 2u]={&
_temp738,& _temp739}; Cyc_Tcutil_terr( e1->loc, _tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof( unsigned char), 54u), _tag_arr( _temp737, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
_temp694= _temp694->tl; _temp729= _temp729->tl;} if( _temp729 !=  0){({ void*
_temp740[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function",
sizeof( unsigned char), 31u), _tag_arr( _temp740, sizeof( void*), 0u));});}
else{ if(( _temp694 !=  0? 1: _temp727)? 1: _temp725 !=  0){ if( _temp727){ for(
0; _temp694 !=  0; _temp694= _temp694->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_temp694->hd);}} else{ if( _temp725 ==  0){({ void* _temp741[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments for function", sizeof( unsigned char), 32u),
_tag_arr( _temp741, sizeof( void*), 0u));});} else{ int _temp744; void* _temp746;
struct Cyc_Absyn_VarargInfo _temp742=* _temp725; _LL747: _temp746=( void*)
_temp742.type; goto _LL745; _LL745: _temp744= _temp742.inject; goto _LL743;
_LL743: { struct Cyc_Absyn_VarargCallInfo* _temp748=({ struct Cyc_Absyn_VarargCallInfo*
_temp778=( struct Cyc_Absyn_VarargCallInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp778->num_varargs= 0; _temp778->injectors= 0; _temp778->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp725); _temp778;});* vararg_call_info=( struct Cyc_Absyn_VarargCallInfo*)
_temp748; if( ! _temp744){ for( 0; _temp694 !=  0; _temp694= _temp694->tl){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp694->hd; _temp748->num_varargs
++; Cyc_Tcexp_tcExp( te,( void**)& _temp746, e1); if( ! Cyc_Tcutil_coerce_arg(
te, e1, _temp746)){({ struct Cyc_Std_String_pa_struct _temp751; _temp751.tag=
Cyc_Std_String_pa; _temp751.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp750; _temp750.tag= Cyc_Std_String_pa; _temp750.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp746);{ void* _temp749[ 2u]={& _temp750,& _temp751}; Cyc_Tcutil_terr( loc,
_tag_arr("vararg requires type %s but argument has type %s", sizeof(
unsigned char), 49u), _tag_arr( _temp749, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}}
else{ void* _temp752= Cyc_Tcutil_compress( _temp746); struct Cyc_Absyn_TunionInfo
_temp758; void* _temp760; struct Cyc_List_List* _temp762; void* _temp764; struct
Cyc_Absyn_Tuniondecl** _temp766; struct Cyc_Absyn_Tuniondecl* _temp768; _LL754:
if(( unsigned int) _temp752 >  3u?*(( int*) _temp752) ==  Cyc_Absyn_TunionType:
0){ _LL759: _temp758=(( struct Cyc_Absyn_TunionType_struct*) _temp752)->f1;
_LL765: _temp764=( void*) _temp758.tunion_info; if(*(( int*) _temp764) ==  Cyc_Absyn_KnownTunion){
_LL767: _temp766=(( struct Cyc_Absyn_KnownTunion_struct*) _temp764)->f1;
_temp768=* _temp766; goto _LL763;} else{ goto _LL756;} _LL763: _temp762=
_temp758.targs; goto _LL761; _LL761: _temp760=( void*) _temp758.rgn; goto _LL755;}
else{ goto _LL756;} _LL756: goto _LL757; _LL755: { struct Cyc_Absyn_Tuniondecl*
_temp769=* Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp768->name); struct Cyc_List_List*
fields= 0; if( _temp769->fields ==  0){({ struct Cyc_Std_String_pa_struct
_temp771; _temp771.tag= Cyc_Std_String_pa; _temp771.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp746);{ void* _temp770[ 1u]={& _temp771}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject into %s",
sizeof( unsigned char), 21u), _tag_arr( _temp770, sizeof( void*), 1u));}});}
else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp769->fields))->v;} if( ! Cyc_Tcutil_unify( _temp760, Cyc_Tcenv_curr_rgn( te))){({
void* _temp772[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad region for injected varargs",
sizeof( unsigned char), 32u), _tag_arr( _temp772, sizeof( void*), 0u));});}{
struct _RegionHandle _temp773= _new_region(); struct _RegionHandle* rgn=&
_temp773; _push_region( rgn);{ struct Cyc_List_List* _temp774=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp769->tvs, _temp762);
for( 0; _temp694 !=  0; _temp694= _temp694->tl){ _temp748->num_varargs ++;{
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp694->hd; Cyc_Tcexp_tcExp(
te, 0, e1);{ struct Cyc_Absyn_Tunionfield* _temp775= Cyc_Tcexp_tcInjection( te,
e1, _temp746, rgn, _temp774, fields); if( _temp775 !=  0){ _temp748->injectors=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp748->injectors,({ struct Cyc_List_List* _temp776=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp776->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)
_check_null( _temp775)); _temp776->tl= 0; _temp776;}));}}}}}; _pop_region( rgn);}
goto _LL753;} _LL757:({ void* _temp777[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad inject vararg type",
sizeof( unsigned char), 23u), _tag_arr( _temp777, sizeof( void*), 0u));}); goto
_LL753; _LL753:;}}}}}}{ struct Cyc_List_List* a= _temp721; for( 0; a !=  0; a= a->tl){
void* _temp779=( void*) a->hd; int _temp785; int _temp787; void* _temp789;
_LL781: if(( unsigned int) _temp779 >  16u?*(( int*) _temp779) ==  Cyc_Absyn_Format_att:
0){ _LL790: _temp789=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp779)->f1;
goto _LL788; _LL788: _temp787=(( struct Cyc_Absyn_Format_att_struct*) _temp779)->f2;
goto _LL786; _LL786: _temp785=(( struct Cyc_Absyn_Format_att_struct*) _temp779)->f3;
goto _LL782;} else{ goto _LL783;} _LL783: goto _LL784; _LL782:{ struct
_handler_cons _temp791; _push_handler(& _temp791);{ int _temp793= 0; if( setjmp(
_temp791.handler)){ _temp793= 1;} if( ! _temp793){{ struct Cyc_Absyn_Exp*
_temp794=(( struct Cyc_Absyn_Exp*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)(
args, _temp787 -  1); struct Cyc_Core_Opt* fmt_args; if( _temp785 ==  0){
fmt_args= 0;} else{ fmt_args=({ struct Cyc_Core_Opt* _temp795=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp795->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth_tail)( args, _temp785 -  1);
_temp795;});}{ void* _temp796= _temp789; _LL798: if( _temp796 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL799;} else{ goto _LL800;} _LL800: if( _temp796 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL801;} else{ goto _LL797;} _LL799: Cyc_Tcexp_check_format_args( te,
_temp794, fmt_args, Cyc_Formatstr_get_format_typs); goto _LL797; _LL801: Cyc_Tcexp_check_format_args(
te, _temp794, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL797; _LL797:;}};
_pop_handler();} else{ void* _temp792=( void*) _exn_thrown; void* _temp803=
_temp792; _LL805: if( _temp803 ==  Cyc_List_Nth){ goto _LL806;} else{ goto
_LL807;} _LL807: goto _LL808; _LL806:({ void* _temp809[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad format arguments", sizeof( unsigned char), 21u), _tag_arr(
_temp809, sizeof( void*), 0u));}); goto _LL804; _LL808:( void) _throw( _temp803);
_LL804:;}}} goto _LL780; _LL784: goto _LL780; _LL780:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp733))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp723); return _temp731; _LL718: return({ void* _temp810[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function", sizeof(
unsigned char), 29u), _tag_arr( _temp810, sizeof( void*), 0u));}); _LL714:;}
_LL700: return({ void* _temp811[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u), _tag_arr( _temp811, sizeof( void*), 0u));});
_LL696:;}} static void* Cyc_Tcexp_tcThrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ,
e); if( ! Cyc_Tcutil_coerce_arg( te, e, Cyc_Absyn_exn_typ)){({ struct Cyc_Std_String_pa_struct
_temp813; _temp813.tag= Cyc_Std_String_pa; _temp813.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp812[ 1u]={&
_temp813}; Cyc_Tcutil_terr( loc, _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp812, sizeof( void*), 1u));}});}
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp814=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp814->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp814;}));} static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp815= t1;
struct Cyc_Absyn_PtrInfo _temp821; struct Cyc_Absyn_Conref* _temp823; struct Cyc_Absyn_Tqual
_temp825; struct Cyc_Absyn_Conref* _temp827; void* _temp829; void* _temp831;
_LL817: if(( unsigned int) _temp815 >  3u?*(( int*) _temp815) ==  Cyc_Absyn_PointerType:
0){ _LL822: _temp821=(( struct Cyc_Absyn_PointerType_struct*) _temp815)->f1;
_LL832: _temp831=( void*) _temp821.elt_typ; goto _LL830; _LL830: _temp829=( void*)
_temp821.rgn_typ; goto _LL828; _LL828: _temp827= _temp821.nullable; goto _LL826;
_LL826: _temp825= _temp821.tq; goto _LL824; _LL824: _temp823= _temp821.bounds;
goto _LL818;} else{ goto _LL819;} _LL819: goto _LL820; _LL818:{ void* _temp833=
Cyc_Tcutil_compress( _temp831); struct Cyc_Absyn_FnInfo _temp839; struct Cyc_List_List*
_temp841; struct Cyc_List_List* _temp843; struct Cyc_Absyn_VarargInfo* _temp845;
int _temp847; struct Cyc_List_List* _temp849; void* _temp851; struct Cyc_Core_Opt*
_temp853; struct Cyc_List_List* _temp855; _LL835: if(( unsigned int) _temp833 > 
3u?*(( int*) _temp833) ==  Cyc_Absyn_FnType: 0){ _LL840: _temp839=(( struct Cyc_Absyn_FnType_struct*)
_temp833)->f1; _LL856: _temp855= _temp839.tvars; goto _LL854; _LL854: _temp853=
_temp839.effect; goto _LL852; _LL852: _temp851=( void*) _temp839.ret_typ; goto
_LL850; _LL850: _temp849= _temp839.args; goto _LL848; _LL848: _temp847= _temp839.c_varargs;
goto _LL846; _LL846: _temp845= _temp839.cyc_varargs; goto _LL844; _LL844:
_temp843= _temp839.rgn_po; goto _LL842; _LL842: _temp841= _temp839.attributes;
goto _LL836;} else{ goto _LL837;} _LL837: goto _LL838; _LL836: { struct Cyc_List_List*
instantiation= 0; if( Cyc_List_length( ts) != (( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp855)){ return({ void* _temp857[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u),
_tag_arr( _temp857, sizeof( void*), 0u));});} for( 0; ts !=  0;( ts= ts->tl,
_temp855= _temp855->tl)){ void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp855))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k, 1,( void*) ts->hd); instantiation=({
struct Cyc_List_List* _temp858=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp858->hd=( void*)({ struct _tuple5* _temp859=(
struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp859->f1=( struct Cyc_Absyn_Tvar*)
_temp855->hd; _temp859->f2=( void*) ts->hd; _temp859;}); _temp858->tl=
instantiation; _temp858;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp863=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp863[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp864; _temp864.tag= Cyc_Absyn_FnType; _temp864.f1=({ struct Cyc_Absyn_FnInfo
_temp865; _temp865.tvars= 0; _temp865.effect= _temp853; _temp865.ret_typ=( void*)
_temp851; _temp865.args= _temp849; _temp865.c_varargs= _temp847; _temp865.cyc_varargs=
_temp845; _temp865.rgn_po= _temp843; _temp865.attributes= _temp841; _temp865;});
_temp864;}); _temp863;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp860=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp860[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp861; _temp861.tag= Cyc_Absyn_PointerType;
_temp861.f1=({ struct Cyc_Absyn_PtrInfo _temp862; _temp862.elt_typ=( void*)
new_fn_typ; _temp862.rgn_typ=( void*) _temp829; _temp862.nullable= _temp827;
_temp862.tq= _temp825; _temp862.bounds= _temp823; _temp862;}); _temp861;});
_temp860;}); return new_typ;}} _LL838: goto _LL834; _LL834:;} goto _LL816;
_LL820: goto _LL816; _LL816:;} return({ struct Cyc_Std_String_pa_struct _temp867;
_temp867.tag= Cyc_Std_String_pa; _temp867.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp866[ 1u]={& _temp867}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting polymorphic type but found %s",
sizeof( unsigned char), 40u), _tag_arr( _temp866, sizeof( void*), 1u));}});}}
static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t); Cyc_Tcexp_tcExp(
te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
if(( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0)? ! Cyc_Tcutil_zero_to_null( te, t, e): 0){ Cyc_Tcutil_unify( t2,
t);{ void* _temp868=({ struct Cyc_Std_String_pa_struct _temp871; _temp871.tag=
Cyc_Std_String_pa; _temp871.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp870; _temp870.tag= Cyc_Std_String_pa;
_temp870.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp869[
2u]={& _temp870,& _temp871}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u), _tag_arr( _temp869, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();
return _temp868;}} return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e0, void** topt,
struct Cyc_Absyn_Exp* e){ void** _temp872= 0; struct Cyc_Absyn_Tqual _temp873=
Cyc_Absyn_empty_tqual(); if( topt !=  0){ void* _temp874= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_PtrInfo _temp880; struct Cyc_Absyn_Tqual _temp882; void*
_temp884; _LL876: if(( unsigned int) _temp874 >  3u?*(( int*) _temp874) ==  Cyc_Absyn_PointerType:
0){ _LL881: _temp880=(( struct Cyc_Absyn_PointerType_struct*) _temp874)->f1;
_LL885: _temp884=( void*) _temp880.elt_typ; goto _LL883; _LL883: _temp882=
_temp880.tq; goto _LL877;} else{ goto _LL878;} _LL878: goto _LL879; _LL877:
_temp872=({ void** _temp886=( void**) _cycalloc( sizeof( void*)); _temp886[ 0]=
_temp884; _temp886;}); _temp873= _temp882; goto _LL875; _LL879: goto _LL875;
_LL875:;} Cyc_Tcexp_tcExpNoInst( te, _temp872, e);{ void* _temp887=( void*) e->r;
struct Cyc_Absyn_Exp* _temp897; struct Cyc_Absyn_Exp* _temp899; _LL889: if(*((
int*) _temp887) ==  Cyc_Absyn_Struct_e){ goto _LL890;} else{ goto _LL891;}
_LL891: if(*(( int*) _temp887) ==  Cyc_Absyn_Tuple_e){ goto _LL892;} else{ goto
_LL893;} _LL893: if(*(( int*) _temp887) ==  Cyc_Absyn_Subscript_e){ _LL900:
_temp899=(( struct Cyc_Absyn_Subscript_e_struct*) _temp887)->f1; goto _LL898;
_LL898: _temp897=(( struct Cyc_Absyn_Subscript_e_struct*) _temp887)->f2; goto
_LL894;} else{ goto _LL895;} _LL895: goto _LL896; _LL890: goto _LL892; _LL892:({
void* _temp901[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate",
sizeof( unsigned char), 19u), _tag_arr( _temp901, sizeof( void*), 0u));});{ void*(*
_temp902)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if(
topt !=  0){ void* _temp903= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp909; struct Cyc_Absyn_Conref* _temp911; _LL905: if(( unsigned int) _temp903
>  3u?*(( int*) _temp903) ==  Cyc_Absyn_PointerType: 0){ _LL910: _temp909=((
struct Cyc_Absyn_PointerType_struct*) _temp903)->f1; _LL912: _temp911= _temp909.nullable;
goto _LL906;} else{ goto _LL907;} _LL907: goto _LL908; _LL906: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp911)){ _temp902= Cyc_Absyn_star_typ;}
goto _LL904; _LL908: goto _LL904; _LL904:;} return _temp902(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v,( void*) Cyc_Absyn_HeapRgn, _temp873);}
_LL894:{ void* _temp913= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp899->topt))->v); _LL915: if(( unsigned int) _temp913 >  3u?*((
int*) _temp913) ==  Cyc_Absyn_TupleType: 0){ goto _LL916;} else{ goto _LL917;}
_LL917: goto _LL918; _LL916: goto _LL914; _LL918:( void*)( e0->r=( void*)(( void*)({
struct Cyc_Absyn_Primop_e_struct* _temp919=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp919[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp920; _temp920.tag= Cyc_Absyn_Primop_e; _temp920.f1=(
void*)(( void*) Cyc_Absyn_Plus); _temp920.f2=({ struct Cyc_List_List* _temp921=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp921->hd=(
void*) _temp899; _temp921->tl=({ struct Cyc_List_List* _temp922=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp922->hd=( void*) _temp897;
_temp922->tl= 0; _temp922;}); _temp921;}); _temp920;}); _temp919;}))); return
Cyc_Tcexp_tcPlus( te, _temp899, _temp897); _LL914:;} goto _LL888; _LL896: goto
_LL888; _LL888:;}{ void* _temp925; int _temp927; struct _tuple6 _temp923= Cyc_Tcutil_addressof_props(
te, e); _LL928: _temp927= _temp923.f1; goto _LL926; _LL926: _temp925= _temp923.f2;
goto _LL924; _LL924: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp927){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp925, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
1, t); if( ! Cyc_Evexp_okay_szofarg( t)){({ struct Cyc_Std_String_pa_struct
_temp930; _temp930.tag= Cyc_Std_String_pa; _temp930.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp929[ 1u]={& _temp930}; Cyc_Tcutil_terr( loc, _tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof( unsigned char), 55u), _tag_arr( _temp929, sizeof( void*), 1u));}});}
return Cyc_Absyn_uint_t;} int Cyc_Tcexp_structfield_has_name( struct _tagged_arr*
n, struct Cyc_Absyn_Structfield* sf){ return Cyc_Std_strcmp(* n,* sf->name) == 
0;} static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, void* n){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, 1, t);{ void* _temp931= n; struct _tagged_arr*
_temp937; unsigned int _temp939; _LL933: if(*(( int*) _temp931) ==  Cyc_Absyn_StructField){
_LL938: _temp937=(( struct Cyc_Absyn_StructField_struct*) _temp931)->f1; goto
_LL934;} else{ goto _LL935;} _LL935: if(*(( int*) _temp931) ==  Cyc_Absyn_TupleIndex){
_LL940: _temp939=(( struct Cyc_Absyn_TupleIndex_struct*) _temp931)->f1; goto
_LL936;} else{ goto _LL932;} _LL934: { int bad_type= 1;{ void* _temp941= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp953; struct _tuple1* _temp955; struct Cyc_List_List*
_temp957; struct Cyc_Absyn_Uniondecl** _temp959; struct _tuple1* _temp961;
struct Cyc_List_List* _temp963; _LL943: if(( unsigned int) _temp941 >  3u?*((
int*) _temp941) ==  Cyc_Absyn_StructType: 0){ _LL956: _temp955=(( struct Cyc_Absyn_StructType_struct*)
_temp941)->f1; goto _LL954; _LL954: _temp953=(( struct Cyc_Absyn_StructType_struct*)
_temp941)->f3; goto _LL944;} else{ goto _LL945;} _LL945: if(( unsigned int)
_temp941 >  3u?*(( int*) _temp941) ==  Cyc_Absyn_AnonStructType: 0){ _LL958:
_temp957=(( struct Cyc_Absyn_AnonStructType_struct*) _temp941)->f1; goto _LL946;}
else{ goto _LL947;} _LL947: if(( unsigned int) _temp941 >  3u?*(( int*) _temp941)
==  Cyc_Absyn_UnionType: 0){ _LL962: _temp961=(( struct Cyc_Absyn_UnionType_struct*)
_temp941)->f1; goto _LL960; _LL960: _temp959=(( struct Cyc_Absyn_UnionType_struct*)
_temp941)->f3; goto _LL948;} else{ goto _LL949;} _LL949: if(( unsigned int)
_temp941 >  3u?*(( int*) _temp941) ==  Cyc_Absyn_AnonUnionType: 0){ _LL964:
_temp963=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp941)->f1; goto _LL950;}
else{ goto _LL951;} _LL951: goto _LL952; _LL944: if( _temp953 ==  0){ return({
void* _temp965[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType",
sizeof( unsigned char), 33u), _tag_arr( _temp965, sizeof( void*), 0u));});} if((*
_temp953)->fields ==  0){ goto _LL942;} if( !(( int(*)( int(* pred)( struct
_tagged_arr*, struct Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name, _temp937,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp953)->fields))->v)){({
struct Cyc_Std_String_pa_struct _temp967; _temp967.tag= Cyc_Std_String_pa;
_temp967.f1=( struct _tagged_arr)* _temp937;{ void* _temp966[ 1u]={& _temp967};
Cyc_Tcutil_terr( loc, _tag_arr("no field of struct has name %s", sizeof(
unsigned char), 31u), _tag_arr( _temp966, sizeof( void*), 1u));}});} bad_type= 0;
goto _LL942; _LL946: if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct
Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcexp_structfield_has_name, _temp937, _temp957)){({ struct Cyc_Std_String_pa_struct
_temp969; _temp969.tag= Cyc_Std_String_pa; _temp969.f1=( struct _tagged_arr)*
_temp937;{ void* _temp968[ 1u]={& _temp969}; Cyc_Tcutil_terr( loc, _tag_arr("no field of struct has name %s",
sizeof( unsigned char), 31u), _tag_arr( _temp968, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL942; _LL948: if( _temp959 ==  0){ return({ void* _temp970[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp970, sizeof( void*), 0u));});} if((*
_temp959)->fields ==  0){ goto _LL942;} if( !(( int(*)( int(* pred)( struct
_tagged_arr*, struct Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name, _temp937,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp959)->fields))->v)){({
struct Cyc_Std_String_pa_struct _temp972; _temp972.tag= Cyc_Std_String_pa;
_temp972.f1=( struct _tagged_arr)* _temp937;{ void* _temp971[ 1u]={& _temp972};
Cyc_Tcutil_terr( loc, _tag_arr("no field of union has name %s", sizeof(
unsigned char), 30u), _tag_arr( _temp971, sizeof( void*), 1u));}});} bad_type= 0;
goto _LL942; _LL950: if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct
Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcexp_structfield_has_name, _temp937, _temp963)){({ struct Cyc_Std_String_pa_struct
_temp974; _temp974.tag= Cyc_Std_String_pa; _temp974.f1=( struct _tagged_arr)*
_temp937;{ void* _temp973[ 1u]={& _temp974}; Cyc_Tcutil_terr( loc, _tag_arr("no field of anonymous union has name %s",
sizeof( unsigned char), 40u), _tag_arr( _temp973, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL942; _LL952: goto _LL942; _LL942:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp976; _temp976.tag= Cyc_Std_String_pa; _temp976.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp975[ 1u]={& _temp976};
Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union type", sizeof(
unsigned char), 36u), _tag_arr( _temp975, sizeof( void*), 1u));}});} goto _LL932;}
_LL936: { int bad_type= 1;{ void* _temp977= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp993; struct Cyc_List_List* _temp995; struct Cyc_Absyn_Uniondecl** _temp997;
struct Cyc_List_List* _temp999; struct Cyc_List_List* _temp1001; struct Cyc_Absyn_TunionFieldInfo
_temp1003; void* _temp1005; struct Cyc_Absyn_Tunionfield* _temp1007; struct Cyc_Absyn_Tuniondecl*
_temp1009; _LL979: if(( unsigned int) _temp977 >  3u?*(( int*) _temp977) ==  Cyc_Absyn_StructType:
0){ _LL994: _temp993=(( struct Cyc_Absyn_StructType_struct*) _temp977)->f3; goto
_LL980;} else{ goto _LL981;} _LL981: if(( unsigned int) _temp977 >  3u?*(( int*)
_temp977) ==  Cyc_Absyn_AnonStructType: 0){ _LL996: _temp995=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp977)->f1; goto _LL982;} else{ goto _LL983;} _LL983: if(( unsigned int)
_temp977 >  3u?*(( int*) _temp977) ==  Cyc_Absyn_UnionType: 0){ _LL998: _temp997=((
struct Cyc_Absyn_UnionType_struct*) _temp977)->f3; goto _LL984;} else{ goto
_LL985;} _LL985: if(( unsigned int) _temp977 >  3u?*(( int*) _temp977) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1000: _temp999=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp977)->f1;
goto _LL986;} else{ goto _LL987;} _LL987: if(( unsigned int) _temp977 >  3u?*((
int*) _temp977) ==  Cyc_Absyn_TupleType: 0){ _LL1002: _temp1001=(( struct Cyc_Absyn_TupleType_struct*)
_temp977)->f1; goto _LL988;} else{ goto _LL989;} _LL989: if(( unsigned int)
_temp977 >  3u?*(( int*) _temp977) ==  Cyc_Absyn_TunionFieldType: 0){ _LL1004:
_temp1003=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp977)->f1; _LL1006:
_temp1005=( void*) _temp1003.field_info; if(*(( int*) _temp1005) ==  Cyc_Absyn_KnownTunionfield){
_LL1010: _temp1009=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1005)->f1;
goto _LL1008; _LL1008: _temp1007=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1005)->f2; goto _LL990;} else{ goto _LL991;}} else{ goto _LL991;} _LL991:
goto _LL992; _LL980: if( _temp993 ==  0){ return({ void* _temp1011[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("offsetof on unchecked StructType", sizeof( unsigned char), 33u),
_tag_arr( _temp1011, sizeof( void*), 0u));});} if((* _temp993)->fields ==  0){
goto _LL978;} if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp993)->fields))->v)
<=  _temp939){({ struct Cyc_Std_Int_pa_struct _temp1014; _temp1014.tag= Cyc_Std_Int_pa;
_temp1014.f1=( int) _temp939;{ struct Cyc_Std_Int_pa_struct _temp1013; _temp1013.tag=
Cyc_Std_Int_pa; _temp1013.f1=( int)(( unsigned int)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((* _temp993)->fields))->v));{ void* _temp1012[ 2u]={& _temp1013,&
_temp1014}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp1012, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL978; _LL982: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp995) <=  _temp939){({ struct Cyc_Std_Int_pa_struct _temp1017; _temp1017.tag=
Cyc_Std_Int_pa; _temp1017.f1=( int) _temp939;{ struct Cyc_Std_Int_pa_struct
_temp1016; _temp1016.tag= Cyc_Std_Int_pa; _temp1016.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp995));{ void* _temp1015[
2u]={& _temp1016,& _temp1017}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp1015, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL978; _LL984: if( _temp997 ==  0){ return({ void* _temp1018[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp1018, sizeof( void*), 0u));});} if((*
_temp997)->fields ==  0){ goto _LL978;} if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*
_temp997)->fields))->v) <=  _temp939){({ struct Cyc_Std_Int_pa_struct _temp1021;
_temp1021.tag= Cyc_Std_Int_pa; _temp1021.f1=( int) _temp939;{ struct Cyc_Std_Int_pa_struct
_temp1020; _temp1020.tag= Cyc_Std_Int_pa; _temp1020.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((* _temp997)->fields))->v));{ void* _temp1019[
2u]={& _temp1020,& _temp1021}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp1019, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL978; _LL986: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp999) <=  _temp939){({ struct Cyc_Std_Int_pa_struct _temp1024; _temp1024.tag=
Cyc_Std_Int_pa; _temp1024.f1=( int) _temp939;{ struct Cyc_Std_Int_pa_struct
_temp1023; _temp1023.tag= Cyc_Std_Int_pa; _temp1023.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp999));{ void* _temp1022[
2u]={& _temp1023,& _temp1024}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp1022, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL978; _LL988: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1001) <=  _temp939){({ struct Cyc_Std_Int_pa_struct _temp1027; _temp1027.tag=
Cyc_Std_Int_pa; _temp1027.f1=( int) _temp939;{ struct Cyc_Std_Int_pa_struct
_temp1026; _temp1026.tag= Cyc_Std_Int_pa; _temp1026.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1001));{ void*
_temp1025[ 2u]={& _temp1026,& _temp1027}; Cyc_Tcutil_terr( loc, _tag_arr("tuple has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp1025, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL978; _LL990: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1007->typs) <  _temp939){({ struct Cyc_Std_Int_pa_struct _temp1030;
_temp1030.tag= Cyc_Std_Int_pa; _temp1030.f1=( int) _temp939;{ struct Cyc_Std_Int_pa_struct
_temp1029; _temp1029.tag= Cyc_Std_Int_pa; _temp1029.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1007->typs));{ void*
_temp1028[ 2u]={& _temp1029,& _temp1030}; Cyc_Tcutil_terr( loc, _tag_arr("tunionfield has too few components: %d < %d",
sizeof( unsigned char), 44u), _tag_arr( _temp1028, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL978; _LL992: goto _LL978; _LL978:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp1032; _temp1032.tag= Cyc_Std_String_pa; _temp1032.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1031[ 1u]={&
_temp1032}; Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union/tuple/tunion type",
sizeof( unsigned char), 49u), _tag_arr( _temp1031, sizeof( void*), 1u));}});}
goto _LL932;} _LL932:;} return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1033= t;
struct Cyc_Absyn_PtrInfo _temp1039; struct Cyc_Absyn_Conref* _temp1041; void*
_temp1043; void* _temp1045; _LL1035: if(( unsigned int) _temp1033 >  3u?*(( int*)
_temp1033) ==  Cyc_Absyn_PointerType: 0){ _LL1040: _temp1039=(( struct Cyc_Absyn_PointerType_struct*)
_temp1033)->f1; _LL1046: _temp1045=( void*) _temp1039.elt_typ; goto _LL1044;
_LL1044: _temp1043=( void*) _temp1039.rgn_typ; goto _LL1042; _LL1042: _temp1041=
_temp1039.bounds; goto _LL1036;} else{ goto _LL1037;} _LL1037: goto _LL1038;
_LL1036: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1043); Cyc_Tcutil_check_nonzero_bound(
loc, _temp1041); return _temp1045; _LL1038: return({ struct Cyc_Std_String_pa_struct
_temp1048; _temp1048.tag= Cyc_Std_String_pa; _temp1048.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1047[ 1u]={& _temp1048}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting * or @ type but found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1047, sizeof( void*), 1u));}}); _LL1034:;}} static void*
Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1049= t;
struct Cyc_Absyn_Structdecl** _temp1065; struct Cyc_Absyn_Structdecl* _temp1067;
struct Cyc_List_List* _temp1068; struct _tuple1* _temp1070; struct Cyc_List_List*
_temp1072; struct Cyc_List_List* _temp1074; struct Cyc_Absyn_Uniondecl**
_temp1076; struct Cyc_Absyn_Uniondecl* _temp1078; struct Cyc_List_List*
_temp1079; struct _tuple1* _temp1081; _LL1051: if(( unsigned int) _temp1049 >  3u?*((
int*) _temp1049) ==  Cyc_Absyn_StructType: 0){ _LL1071: _temp1070=(( struct Cyc_Absyn_StructType_struct*)
_temp1049)->f1; goto _LL1069; _LL1069: _temp1068=(( struct Cyc_Absyn_StructType_struct*)
_temp1049)->f2; goto _LL1066; _LL1066: _temp1065=(( struct Cyc_Absyn_StructType_struct*)
_temp1049)->f3; if( _temp1065 ==  0){ goto _LL1053;} else{ _temp1067=* _temp1065;
goto _LL1052;}} else{ goto _LL1053;} _LL1053: if(( unsigned int) _temp1049 >  3u?*((
int*) _temp1049) ==  Cyc_Absyn_AnonStructType: 0){ _LL1073: _temp1072=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1049)->f1; goto _LL1054;} else{ goto
_LL1055;} _LL1055: if(( unsigned int) _temp1049 >  3u?*(( int*) _temp1049) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1075: _temp1074=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1049)->f1; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(( unsigned int)
_temp1049 >  3u?*(( int*) _temp1049) ==  Cyc_Absyn_UnionType: 0){ _LL1082:
_temp1081=(( struct Cyc_Absyn_UnionType_struct*) _temp1049)->f1; goto _LL1080;
_LL1080: _temp1079=(( struct Cyc_Absyn_UnionType_struct*) _temp1049)->f2; goto
_LL1077; _LL1077: _temp1076=(( struct Cyc_Absyn_UnionType_struct*) _temp1049)->f3;
if( _temp1076 ==  0){ goto _LL1059;} else{ _temp1078=* _temp1076; goto _LL1058;}}
else{ goto _LL1059;} _LL1059: if(( unsigned int) _temp1049 >  3u?*(( int*)
_temp1049) ==  Cyc_Absyn_ArrayType: 0){ goto _LL1083;} else{ goto _LL1061;}
_LL1083: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
==  0){ goto _LL1060;} else{ goto _LL1061;} _LL1061: if(( unsigned int)
_temp1049 >  3u?*(( int*) _temp1049) ==  Cyc_Absyn_PointerType: 0){ goto _LL1084;}
else{ goto _LL1063;} _LL1084: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof(
unsigned char), 5u)) ==  0){ goto _LL1062;} else{ goto _LL1063;} _LL1063: goto
_LL1064; _LL1052: if( _temp1070 ==  0){ return({ void* _temp1085[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved struct type (COMPILER ERROR)", sizeof(
unsigned char), 40u), _tag_arr( _temp1085, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp1086= Cyc_Absyn_lookup_struct_field( _temp1067, f); if( _temp1086 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1089; _temp1089.tag= Cyc_Std_String_pa;
_temp1089.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1088; _temp1088.tag= Cyc_Std_String_pa; _temp1088.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1070));{ void*
_temp1087[ 2u]={& _temp1088,& _temp1089}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp1087, sizeof( void*), 2u));}}});}{
void* t2;{ struct _RegionHandle _temp1090= _new_region(); struct _RegionHandle*
rgn=& _temp1090; _push_region( rgn);{ struct Cyc_List_List* _temp1091=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r1, struct _RegionHandle* r2, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1067->tvs,
_temp1068); t2= Cyc_Tcutil_rsubstitute( rgn, _temp1091,( void*) _temp1086->type);};
_pop_region( rgn);} return t2;}} _LL1054: { struct Cyc_Absyn_Structfield*
_temp1092= Cyc_Absyn_lookup_field( _temp1072, f); if( _temp1092 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1094; _temp1094.tag= Cyc_Std_String_pa;
_temp1094.f1=( struct _tagged_arr)* f;{ void* _temp1093[ 1u]={& _temp1094}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1093, sizeof( void*), 1u));}});} return( void*) _temp1092->type;}
_LL1056: { struct Cyc_Absyn_Structfield* _temp1095= Cyc_Absyn_lookup_field(
_temp1074, f); if( _temp1095 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1097; _temp1097.tag= Cyc_Std_String_pa; _temp1097.f1=( struct _tagged_arr)*
f;{ void* _temp1096[ 1u]={& _temp1097}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp1096, sizeof( void*), 1u));}});}
return( void*) _temp1095->type;} _LL1058: if( _temp1081 ==  0){ return({ void*
_temp1098[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("unresolved union type (COMPILER ERROR)",
sizeof( unsigned char), 39u), _tag_arr( _temp1098, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structfield* _temp1099= Cyc_Absyn_lookup_union_field( _temp1078,
f); if( _temp1099 ==  0){ return({ struct Cyc_Std_String_pa_struct _temp1102;
_temp1102.tag= Cyc_Std_String_pa; _temp1102.f1=( struct _tagged_arr)* f;{ struct
Cyc_Std_String_pa_struct _temp1101; _temp1101.tag= Cyc_Std_String_pa; _temp1101.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp1081));{ void* _temp1100[ 2u]={& _temp1101,& _temp1102}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union %s has no %s field", sizeof( unsigned char), 25u),
_tag_arr( _temp1100, sizeof( void*), 2u));}}});}{ struct Cyc_List_List*
_temp1103=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1078->tvs, _temp1079); void* _temp1104= Cyc_Tcutil_substitute(
_temp1103,( void*) _temp1099->type); return _temp1104;}} _LL1060:( void*)(
outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1105=(
struct Cyc_Absyn_Primop_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp1105[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp1106; _temp1106.tag= Cyc_Absyn_Primop_e;
_temp1106.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1106.f2=({ struct Cyc_List_List*
_temp1107=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1107->hd=( void*) e; _temp1107->tl= 0; _temp1107;}); _temp1106;});
_temp1105;}))); return Cyc_Absyn_uint_t; _LL1062:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1108=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1108[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1109; _temp1109.tag= Cyc_Absyn_Primop_e;
_temp1109.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1109.f2=({ struct Cyc_List_List*
_temp1110=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1110->hd=( void*) e; _temp1110->tl= 0; _temp1110;}); _temp1109;});
_temp1108;}))); return Cyc_Absyn_uint_t; _LL1064: if( Cyc_Std_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1112; _temp1112.tag= Cyc_Std_String_pa; _temp1112.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1111[ 1u]={& _temp1112}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u), _tag_arr( _temp1111, sizeof( void*), 1u));}});} else{
return({ struct Cyc_Std_String_pa_struct _temp1114; _temp1114.tag= Cyc_Std_String_pa;
_temp1114.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1113[
1u]={& _temp1114}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct or union, found %s",
sizeof( unsigned char), 36u), _tag_arr( _temp1113, sizeof( void*), 1u));}});}
_LL1050:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1115= t;
struct Cyc_Absyn_PtrInfo _temp1121; struct Cyc_Absyn_Conref* _temp1123; void*
_temp1125; void* _temp1127; _LL1117: if(( unsigned int) _temp1115 >  3u?*(( int*)
_temp1115) ==  Cyc_Absyn_PointerType: 0){ _LL1122: _temp1121=(( struct Cyc_Absyn_PointerType_struct*)
_temp1115)->f1; _LL1128: _temp1127=( void*) _temp1121.elt_typ; goto _LL1126;
_LL1126: _temp1125=( void*) _temp1121.rgn_typ; goto _LL1124; _LL1124: _temp1123=
_temp1121.bounds; goto _LL1118;} else{ goto _LL1119;} _LL1119: goto _LL1120;
_LL1118: Cyc_Tcutil_check_nonzero_bound( loc, _temp1123);{ void* _temp1129= Cyc_Tcutil_compress(
_temp1127); struct Cyc_Absyn_Structdecl** _temp1141; struct Cyc_Absyn_Structdecl*
_temp1143; struct Cyc_List_List* _temp1144; struct _tuple1* _temp1146; struct
Cyc_Absyn_Uniondecl** _temp1148; struct Cyc_Absyn_Uniondecl* _temp1150; struct
Cyc_List_List* _temp1151; struct _tuple1* _temp1153; struct Cyc_List_List*
_temp1155; struct Cyc_List_List* _temp1157; _LL1131: if(( unsigned int)
_temp1129 >  3u?*(( int*) _temp1129) ==  Cyc_Absyn_StructType: 0){ _LL1147:
_temp1146=(( struct Cyc_Absyn_StructType_struct*) _temp1129)->f1; goto _LL1145;
_LL1145: _temp1144=(( struct Cyc_Absyn_StructType_struct*) _temp1129)->f2; goto
_LL1142; _LL1142: _temp1141=(( struct Cyc_Absyn_StructType_struct*) _temp1129)->f3;
if( _temp1141 ==  0){ goto _LL1133;} else{ _temp1143=* _temp1141; goto _LL1132;}}
else{ goto _LL1133;} _LL1133: if(( unsigned int) _temp1129 >  3u?*(( int*)
_temp1129) ==  Cyc_Absyn_UnionType: 0){ _LL1154: _temp1153=(( struct Cyc_Absyn_UnionType_struct*)
_temp1129)->f1; goto _LL1152; _LL1152: _temp1151=(( struct Cyc_Absyn_UnionType_struct*)
_temp1129)->f2; goto _LL1149; _LL1149: _temp1148=(( struct Cyc_Absyn_UnionType_struct*)
_temp1129)->f3; if( _temp1148 ==  0){ goto _LL1135;} else{ _temp1150=* _temp1148;
goto _LL1134;}} else{ goto _LL1135;} _LL1135: if(( unsigned int) _temp1129 >  3u?*((
int*) _temp1129) ==  Cyc_Absyn_AnonStructType: 0){ _LL1156: _temp1155=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1129)->f1; goto _LL1136;} else{ goto
_LL1137;} _LL1137: if(( unsigned int) _temp1129 >  3u?*(( int*) _temp1129) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1158: _temp1157=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1129)->f1; goto _LL1138;} else{ goto _LL1139;} _LL1139: goto _LL1140;
_LL1132: { struct Cyc_Absyn_Structfield* _temp1159= Cyc_Absyn_lookup_struct_field(
_temp1143, f); if( _temp1159 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1162; _temp1162.tag= Cyc_Std_String_pa; _temp1162.f1=( struct _tagged_arr)*
f;{ struct Cyc_Std_String_pa_struct _temp1161; _temp1161.tag= Cyc_Std_String_pa;
_temp1161.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1146));{ void* _temp1160[ 2u]={& _temp1161,& _temp1162}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct %s has no %s field", sizeof( unsigned char), 26u),
_tag_arr( _temp1160, sizeof( void*), 2u));}}});}{ void* t3;{ struct
_RegionHandle _temp1163= _new_region(); struct _RegionHandle* rgn=& _temp1163;
_push_region( rgn);{ struct Cyc_List_List* _temp1164=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1143->tvs, _temp1144);
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1164,( void*) _temp1159->type);};
_pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1125); return
t3;}} _LL1134: { struct Cyc_Absyn_Structfield* _temp1165= Cyc_Absyn_lookup_union_field(
_temp1150, f); if( _temp1165 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1168; _temp1168.tag= Cyc_Std_String_pa; _temp1168.f1=( struct _tagged_arr)*
f;{ struct Cyc_Std_String_pa_struct _temp1167; _temp1167.tag= Cyc_Std_String_pa;
_temp1167.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1153));{ void* _temp1166[ 2u]={& _temp1167,& _temp1168}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union %s has no %s field", sizeof( unsigned char), 25u),
_tag_arr( _temp1166, sizeof( void*), 2u));}}});}{ struct Cyc_List_List*
_temp1169=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1150->tvs, _temp1151); void* _temp1170= Cyc_Tcutil_substitute(
_temp1169,( void*) _temp1165->type); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1125); return _temp1170;}} _LL1136: { struct Cyc_Absyn_Structfield*
_temp1171= Cyc_Absyn_lookup_field( _temp1155, f); if( _temp1171 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1173; _temp1173.tag= Cyc_Std_String_pa;
_temp1173.f1=( struct _tagged_arr)* f;{ void* _temp1172[ 1u]={& _temp1173}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1172, sizeof( void*), 1u));}});} return( void*) _temp1171->type;}
_LL1138: { struct Cyc_Absyn_Structfield* _temp1174= Cyc_Absyn_lookup_field(
_temp1157, f); if( _temp1174 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1176; _temp1176.tag= Cyc_Std_String_pa; _temp1176.f1=( struct _tagged_arr)*
f;{ void* _temp1175[ 1u]={& _temp1176}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp1175, sizeof( void*), 1u));}});}
return( void*) _temp1174->type;} _LL1140: return({ struct Cyc_Std_String_pa_struct
_temp1178; _temp1178.tag= Cyc_Std_String_pa; _temp1178.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1177[ 1u]={& _temp1178}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct pointer, found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1177, sizeof( void*), 1u));}}); _LL1130:;} _LL1120: return({
struct Cyc_Std_String_pa_struct _temp1180; _temp1180.tag= Cyc_Std_String_pa;
_temp1180.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1179[
1u]={& _temp1180}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp1179, sizeof( void*), 1u));}});
_LL1116:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1181; _push_handler(& _temp1181);{ int _temp1183= 0; if(
setjmp( _temp1181.handler)){ _temp1183= 1;} if( ! _temp1183){{ void* _temp1184=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1184;}; _pop_handler();} else{ void* _temp1182=(
void*) _exn_thrown; void* _temp1186= _temp1182; _LL1188: if( _temp1186 ==  Cyc_List_Nth){
goto _LL1189;} else{ goto _LL1190;} _LL1190: goto _LL1191; _LL1189: return({
struct Cyc_Std_Int_pa_struct _temp1194; _temp1194.tag= Cyc_Std_Int_pa; _temp1194.f1=(
int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts));{
struct Cyc_Std_Int_pa_struct _temp1193; _temp1193.tag= Cyc_Std_Int_pa; _temp1193.f1=(
int) i;{ void* _temp1192[ 2u]={& _temp1193,& _temp1194}; Cyc_Tcexp_expr_err( te,
loc, _tag_arr("index is %d but tuple has only %d fields", sizeof( unsigned char),
41u), _tag_arr( _temp1192, sizeof( void*), 2u));}}}); _LL1191:( void) _throw(
_temp1186); _LL1187:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return({
struct Cyc_Std_String_pa_struct _temp1196; _temp1196.tag= Cyc_Std_String_pa;
_temp1196.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp1195[
1u]={& _temp1196}; Cyc_Tcexp_expr_err( te, e2->loc, _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp1195, sizeof( void*), 1u));}});}{
void* _temp1197= t1; struct Cyc_Absyn_PtrInfo _temp1205; struct Cyc_Absyn_Conref*
_temp1207; struct Cyc_Absyn_Tqual _temp1209; void* _temp1211; void* _temp1213;
struct Cyc_List_List* _temp1215; _LL1199: if(( unsigned int) _temp1197 >  3u?*((
int*) _temp1197) ==  Cyc_Absyn_PointerType: 0){ _LL1206: _temp1205=(( struct Cyc_Absyn_PointerType_struct*)
_temp1197)->f1; _LL1214: _temp1213=( void*) _temp1205.elt_typ; goto _LL1212;
_LL1212: _temp1211=( void*) _temp1205.rgn_typ; goto _LL1210; _LL1210: _temp1209=
_temp1205.tq; goto _LL1208; _LL1208: _temp1207= _temp1205.bounds; goto _LL1200;}
else{ goto _LL1201;} _LL1201: if(( unsigned int) _temp1197 >  3u?*(( int*)
_temp1197) ==  Cyc_Absyn_TupleType: 0){ _LL1216: _temp1215=(( struct Cyc_Absyn_TupleType_struct*)
_temp1197)->f1; goto _LL1202;} else{ goto _LL1203;} _LL1203: goto _LL1204;
_LL1200: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1207);} else{ if( Cyc_Tcutil_is_bound_one( _temp1207)){({ void*
_temp1217[ 0u]={}; Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u), _tag_arr( _temp1217, sizeof( void*), 0u));});} Cyc_Tcutil_check_nonzero_bound(
loc, _temp1207);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1211); return
_temp1213; _LL1202: return Cyc_Tcexp_ithTupleType( te, loc, _temp1215, e2);
_LL1204: return({ struct Cyc_Std_String_pa_struct _temp1219; _temp1219.tag= Cyc_Std_String_pa;
_temp1219.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp1218[
1u]={& _temp1219}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1218, sizeof( void*), 1u));}});
_LL1198:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){ int done= 0;
struct Cyc_List_List* fields= 0; if( topt !=  0){ void* _temp1220= Cyc_Tcutil_compress(*
topt); struct Cyc_List_List* _temp1226; _LL1222: if(( unsigned int) _temp1220 > 
3u?*(( int*) _temp1220) ==  Cyc_Absyn_TupleType: 0){ _LL1227: _temp1226=((
struct Cyc_Absyn_TupleType_struct*) _temp1220)->f1; goto _LL1223;} else{ goto
_LL1224;} _LL1224: goto _LL1225; _LL1223: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp1226) != (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ goto _LL1221;} for( 0; es !=  0;( es= es->tl, _temp1226= _temp1226->tl)){
void* _temp1228=(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
_temp1226))->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1228,(
struct Cyc_Absyn_Exp*) es->hd); fields=({ struct Cyc_List_List* _temp1229=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1229->hd=(
void*)({ struct _tuple8* _temp1230=( struct _tuple8*) _cycalloc( sizeof( struct
_tuple8)); _temp1230->f1=(*(( struct _tuple8*) _temp1226->hd)).f1; _temp1230->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) es->hd)->topt))->v;
_temp1230;}); _temp1229->tl= fields; _temp1229;});} done= 1; goto _LL1221;
_LL1225: goto _LL1221; _LL1221:;} if( ! done){ for( 0; es !=  0; es= es->tl){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*) es->hd); fields=({
struct Cyc_List_List* _temp1231=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1231->hd=( void*)({ struct _tuple8* _temp1232=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1232->f1= Cyc_Absyn_empty_tqual();
_temp1232->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)
es->hd)->topt))->v; _temp1232;}); _temp1231->tl= fields; _temp1231;});}} return(
void*)({ struct Cyc_Absyn_TupleType_struct* _temp1233=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1233[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1234; _temp1234.tag= Cyc_Absyn_TupleType;
_temp1234.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1234;}); _temp1233;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return({ void* _temp1235[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("tcCompoundLit", sizeof( unsigned char), 14u), _tag_arr(
_temp1235, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({ struct Cyc_Core_Opt* _temp1258=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1258->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1258;})); struct Cyc_Absyn_Const_e_struct* _temp1236=({ struct Cyc_Absyn_Const_e_struct*
_temp1254=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1254[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1255; _temp1255.tag= Cyc_Absyn_Const_e;
_temp1255.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1256=(
struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1256[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1257; _temp1257.tag= Cyc_Absyn_Int_c;
_temp1257.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1257.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1257;}); _temp1256;}));
_temp1255;}); _temp1254;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) _temp1236, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1237=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1237->v=( void*)
Cyc_Absyn_uint_t; _temp1237;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1252=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1252[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1253; _temp1253.tag= Cyc_Absyn_ArrayType;
_temp1253.f1=( void*) res; _temp1253.f2= Cyc_Absyn_empty_tqual(); _temp1253.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1253;}); _temp1252;});{ struct Cyc_List_List*
es2= es; for( 0; es2 !=  0; es2= es2->tl){ Cyc_Tcexp_tcExpInitializer( te,
elt_topt,( struct Cyc_Absyn_Exp*) es2->hd);}} if( ! Cyc_Tcutil_coerce_list( te,
res, es)){({ struct Cyc_Std_String_pa_struct _temp1239; _temp1239.tag= Cyc_Std_String_pa;
_temp1239.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( res);{ void*
_temp1238[ 1u]={& _temp1239}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->loc, _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp1238, sizeof( void*), 1u));}});}{
int offset= 0; for( 0; des !=  0;( offset ++, des= des->tl)){ struct Cyc_List_List*
ds=(*(( struct _tuple7*) des->hd)).f1; if( ds !=  0){ void* _temp1240=( void*)
ds->hd; struct Cyc_Absyn_Exp* _temp1246; _LL1242: if(*(( int*) _temp1240) == 
Cyc_Absyn_FieldName){ goto _LL1243;} else{ goto _LL1244;} _LL1244: if(*(( int*)
_temp1240) ==  Cyc_Absyn_ArrayElement){ _LL1247: _temp1246=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1240)->f1; goto _LL1245;} else{ goto _LL1241;} _LL1243:({ void* _temp1248[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("only array index designators are supported",
sizeof( unsigned char), 43u), _tag_arr( _temp1248, sizeof( void*), 0u));}); goto
_LL1241; _LL1245: Cyc_Tcexp_tcExpInitializer( te, 0, _temp1246);{ unsigned int i=
Cyc_Evexp_eval_const_uint_exp( _temp1246); if( i !=  offset){({ struct Cyc_Std_Int_pa_struct
_temp1251; _temp1251.tag= Cyc_Std_Int_pa; _temp1251.f1=( int) i;{ struct Cyc_Std_Int_pa_struct
_temp1250; _temp1250.tag= Cyc_Std_Int_pa; _temp1250.f1=( int)(( unsigned int)
offset);{ void* _temp1249[ 2u]={& _temp1250,& _temp1251}; Cyc_Tcutil_terr(
_temp1246->loc, _tag_arr("expecting index designator %d but found %d", sizeof(
unsigned char), 43u), _tag_arr( _temp1249, sizeof( void*), 2u));}}});} goto
_LL1241;} _LL1241:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){({
struct Cyc_Std_String_pa_struct _temp1260; _temp1260.tag= Cyc_Std_String_pa;
_temp1260.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v);{ void* _temp1259[ 1u]={& _temp1260}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("expecting unsigned int, found %s", sizeof( unsigned char),
33u), _tag_arr( _temp1259, sizeof( void*), 1u));}});} if( !( vd->tq).q_const){({
void* _temp1261[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u), _tag_arr( _temp1261, sizeof( void*), 0u));});} if(
te->le !=  0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);}{ struct Cyc_Absyn_PtrInfo pinfo; void** _temp1262= 0; struct Cyc_Absyn_Tqual*
_temp1263= 0; if( topt !=  0){ void* _temp1264= Cyc_Tcutil_compress(* topt);
struct Cyc_Absyn_PtrInfo _temp1272; struct Cyc_Absyn_Exp* _temp1274; struct Cyc_Absyn_Tqual
_temp1276; struct Cyc_Absyn_Tqual* _temp1278; void* _temp1279; void** _temp1281;
_LL1266: if(( unsigned int) _temp1264 >  3u?*(( int*) _temp1264) ==  Cyc_Absyn_PointerType:
0){ _LL1273: _temp1272=(( struct Cyc_Absyn_PointerType_struct*) _temp1264)->f1;
goto _LL1267;} else{ goto _LL1268;} _LL1268: if(( unsigned int) _temp1264 >  3u?*((
int*) _temp1264) ==  Cyc_Absyn_ArrayType: 0){ _LL1280: _temp1279=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1264)->f1; _temp1281=( void**)&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1264)->f1); goto _LL1277; _LL1277:
_temp1276=(( struct Cyc_Absyn_ArrayType_struct*) _temp1264)->f2; _temp1278=(
struct Cyc_Absyn_Tqual*)&(( struct Cyc_Absyn_ArrayType_struct*) _temp1264)->f2;
goto _LL1275; _LL1275: _temp1274=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1264)->f3; goto _LL1269;} else{ goto _LL1270;} _LL1270: goto _LL1271;
_LL1267: pinfo= _temp1272; _temp1262=( void**)(( void**)(( void*)& pinfo.elt_typ));
_temp1263=( struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1265; _LL1269: _temp1262=(
void**) _temp1281; _temp1263=( struct Cyc_Absyn_Tqual*) _temp1278; goto _LL1265;
_LL1271: goto _LL1265; _LL1265:;}{ void* t= Cyc_Tcexp_tcExp( te, _temp1262, body);
if( te->le ==  0){ if( ! Cyc_Tcutil_is_const_exp( te, bound)){({ void* _temp1282[
0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant", sizeof(
unsigned char), 22u), _tag_arr( _temp1282, sizeof( void*), 0u));});} if( ! Cyc_Tcutil_is_const_exp(
te, body)){({ void* _temp1283[ 0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("body is not constant",
sizeof( unsigned char), 21u), _tag_arr( _temp1283, sizeof( void*), 0u));});}}
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1284=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1284[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1285; _temp1285.tag= Cyc_Absyn_ArrayType;
_temp1285.f1=( void*) t; _temp1285.f2= _temp1263 ==  0? Cyc_Absyn_empty_tqual():*
_temp1263; _temp1285.f3=( struct Cyc_Absyn_Exp*) bound; _temp1285;}); _temp1284;});}}}
struct _tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct _tuple1** tn, struct Cyc_Core_Opt** otyps, struct Cyc_List_List*
args, struct Cyc_Absyn_Structdecl** sd_opt){ struct Cyc_Absyn_Structdecl* sd;
if(* sd_opt !=  0){ sd=( struct Cyc_Absyn_Structdecl*) _check_null(* sd_opt);}
else{{ struct _handler_cons _temp1286; _push_handler(& _temp1286);{ int
_temp1288= 0; if( setjmp( _temp1286.handler)){ _temp1288= 1;} if( ! _temp1288){
sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();} else{ void*
_temp1287=( void*) _exn_thrown; void* _temp1290= _temp1287; _LL1292: if(
_temp1290 ==  Cyc_Dict_Absent){ goto _LL1293;} else{ goto _LL1294;} _LL1294:
goto _LL1295; _LL1293:({ struct Cyc_Std_String_pa_struct _temp1297; _temp1297.tag=
Cyc_Std_String_pa; _temp1297.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1296[ 1u]={& _temp1297}; Cyc_Tcutil_terr( loc, _tag_arr("unbound struct name %s",
sizeof( unsigned char), 23u), _tag_arr( _temp1296, sizeof( void*), 1u));}});
return topt !=  0?* topt:( void*) Cyc_Absyn_VoidType; _LL1295:( void) _throw(
_temp1290); _LL1291:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!=  0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1298= _new_region(); struct _RegionHandle* rgn=&
_temp1298; _push_region( rgn);{ struct _tuple4 _temp1299=({ struct _tuple4
_temp1323; _temp1323.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1323.f2= rgn;
_temp1323;}); struct Cyc_List_List* _temp1300=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1299, sd->tvs); struct Cyc_List_List* _temp1301=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1300); struct Cyc_Absyn_StructType_struct*
_temp1302=({ struct Cyc_Absyn_StructType_struct* _temp1320=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1320[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1321; _temp1321.tag= Cyc_Absyn_StructType;
_temp1321.f1=( struct _tuple1*)* tn; _temp1321.f2= _temp1301; _temp1321.f3=({
struct Cyc_Absyn_Structdecl** _temp1322=( struct Cyc_Absyn_Structdecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1322[ 0]= sd; _temp1322;});
_temp1321;}); _temp1320;}); if( topt !=  0){ Cyc_Tcutil_unify(( void*) _temp1302,*
topt);}* otyps=({ struct Cyc_Core_Opt* _temp1303=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1303->v=( void*) _temp1301;
_temp1303;}); if( sd->fields ==  0){({ void* _temp1304[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("can't build abstract struct", sizeof( unsigned char), 28u),
_tag_arr( _temp1304, sizeof( void*), 0u));});{ void* _temp1305=( void*)
_temp1302; _npop_handler( 0u); return _temp1305;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields !=  0; fields= fields->tl){ struct _tuple9 _temp1308; struct Cyc_Absyn_Exp*
_temp1309; struct Cyc_Absyn_Structfield* _temp1311; struct _tuple9* _temp1306=(
struct _tuple9*) fields->hd; _temp1308=* _temp1306; _LL1312: _temp1311=
_temp1308.f1; goto _LL1310; _LL1310: _temp1309= _temp1308.f2; goto _LL1307;
_LL1307: { void* _temp1313= Cyc_Tcutil_rsubstitute( rgn, _temp1300,( void*)
_temp1311->type); Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1313, _temp1309);
if( ! Cyc_Tcutil_coerce_arg( te, _temp1309, _temp1313)){({ struct Cyc_Std_String_pa_struct
_temp1318; _temp1318.tag= Cyc_Std_String_pa; _temp1318.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1309->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1317; _temp1317.tag= Cyc_Std_String_pa;
_temp1317.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp1313);{ struct
Cyc_Std_String_pa_struct _temp1316; _temp1316.tag= Cyc_Std_String_pa; _temp1316.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string(* tn);{ struct Cyc_Std_String_pa_struct
_temp1315; _temp1315.tag= Cyc_Std_String_pa; _temp1315.f1=( struct _tagged_arr)*
_temp1311->name;{ void* _temp1314[ 4u]={& _temp1315,& _temp1316,& _temp1317,&
_temp1318}; Cyc_Tcutil_terr( _temp1309->loc, _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1314, sizeof( void*), 4u));}}}}});
Cyc_Tcutil_explain_failure();}}}{ void* _temp1319=( void*) _temp1302;
_npop_handler( 0u); return _temp1319;}}}; _pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* ts, struct
Cyc_List_List* args){{ struct _RegionHandle _temp1324= _new_region(); struct
_RegionHandle* rgn=& _temp1324; _push_region( rgn);{ void* _temp1325= Cyc_Tcutil_compress(
ts); struct Cyc_List_List* _temp1331; _LL1327: if(( unsigned int) _temp1325 >  3u?*((
int*) _temp1325) ==  Cyc_Absyn_AnonStructType: 0){ _LL1332: _temp1331=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1325)->f1; goto _LL1328;} else{ goto
_LL1329;} _LL1329: goto _LL1330; _LL1328: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1331); for( 0; fields !=  0; fields= fields->tl){ struct
_tuple9 _temp1335; struct Cyc_Absyn_Exp* _temp1336; struct Cyc_Absyn_Structfield*
_temp1338; struct _tuple9* _temp1333=( struct _tuple9*) fields->hd; _temp1335=*
_temp1333; _LL1339: _temp1338= _temp1335.f1; goto _LL1337; _LL1337: _temp1336=
_temp1335.f2; goto _LL1334; _LL1334: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1338->type)), _temp1336); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1336,( void*) _temp1338->type)){({ struct Cyc_Std_String_pa_struct
_temp1343; _temp1343.tag= Cyc_Std_String_pa; _temp1343.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1336->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1342; _temp1342.tag= Cyc_Std_String_pa;
_temp1342.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*) _temp1338->type);{
struct Cyc_Std_String_pa_struct _temp1341; _temp1341.tag= Cyc_Std_String_pa;
_temp1341.f1=( struct _tagged_arr)* _temp1338->name;{ void* _temp1340[ 3u]={&
_temp1341,& _temp1342,& _temp1343}; Cyc_Tcutil_terr( _temp1336->loc, _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1340, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} goto _LL1326;} _LL1330:({ void* _temp1344[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("tcAnonStruct:  not an AnonStructType", sizeof( unsigned char), 37u),
_tag_arr( _temp1344, sizeof( void*), 0u));}); goto _LL1326; _LL1326:;};
_pop_region( rgn);} return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List*
es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield* tuf){ struct
_RegionHandle _temp1345= _new_region(); struct _RegionHandle* rgn=& _temp1345;
_push_region( rgn);{ struct _tuple4 _temp1346=({ struct _tuple4 _temp1392;
_temp1392.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1392.f2= rgn; _temp1392;});
struct Cyc_List_List* _temp1347=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1346, tud->tvs); struct Cyc_List_List* _temp1348=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1346, tuf->tvs); struct Cyc_List_List* _temp1349=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1347); struct Cyc_List_List* _temp1350=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1348); struct Cyc_List_List*
_temp1351=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1347, _temp1348);*
all_ref=({ struct Cyc_Core_Opt* _temp1352=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1352->v=( void*) _temp1349; _temp1352;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1353=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1353->v=( void*) _temp1350; _temp1353;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1387=( struct
Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1387[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1388; _temp1388.tag=
Cyc_Absyn_TunionFieldType; _temp1388.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1389; _temp1389.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1390=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1390[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1391; _temp1391.tag= Cyc_Absyn_KnownTunionfield; _temp1391.f1= tud;
_temp1391.f2= tuf; _temp1391;}); _temp1390;})); _temp1389.targs= _temp1349;
_temp1389;}); _temp1388;}); _temp1387;}); if( topt !=  0){ void* _temp1354= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_TunionInfo _temp1362; void* _temp1364; struct Cyc_List_List*
_temp1366; _LL1356: if(( unsigned int) _temp1354 >  3u?*(( int*) _temp1354) == 
Cyc_Absyn_TunionFieldType: 0){ goto _LL1357;} else{ goto _LL1358;} _LL1358: if((
unsigned int) _temp1354 >  3u?*(( int*) _temp1354) ==  Cyc_Absyn_TunionType: 0){
_LL1363: _temp1362=(( struct Cyc_Absyn_TunionType_struct*) _temp1354)->f1;
_LL1367: _temp1366= _temp1362.targs; goto _LL1365; _LL1365: _temp1364=( void*)
_temp1362.rgn; goto _LL1359;} else{ goto _LL1360;} _LL1360: goto _LL1361;
_LL1357: Cyc_Tcutil_unify(* topt, res); goto _LL1355; _LL1359:{ struct Cyc_List_List*
a= _temp1349; for( 0; a !=  0? _temp1366 !=  0: 0;( a= a->tl, _temp1366=
_temp1366->tl)){ Cyc_Tcutil_unify(( void*) a->hd,( void*) _temp1366->hd);}} if(
tuf->typs ==  0? es ==  0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1368=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1368->v=(
void*) res; _temp1368;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1369=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1369[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1370; _temp1370.tag=
Cyc_Absyn_TunionType; _temp1370.f1=({ struct Cyc_Absyn_TunionInfo _temp1371;
_temp1371.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1372=( struct Cyc_Absyn_KnownTunion_struct*) _cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1372[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1373; _temp1373.tag=
Cyc_Absyn_KnownTunion; _temp1373.f1=({ struct Cyc_Absyn_Tuniondecl** _temp1374=(
struct Cyc_Absyn_Tuniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp1374[ 0]= tud; _temp1374;}); _temp1373;}); _temp1372;})); _temp1371.targs=
_temp1349; _temp1371.rgn=( void*) _temp1364; _temp1371;}); _temp1370;});
_temp1369;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1375= res;
_npop_handler( 0u); return _temp1375;}} goto _LL1355; _LL1361: goto _LL1355;
_LL1355:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es !=  0? ts !=  0: 0;(
es= es->tl, ts= ts->tl)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) es->hd;
void* t= Cyc_Tcutil_rsubstitute( rgn, _temp1351,(*(( struct _tuple8*) ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){({ struct Cyc_Std_String_pa_struct _temp1379; _temp1379.tag= Cyc_Std_String_pa;
_temp1379.f1=( struct _tagged_arr)( e->topt ==  0? _tag_arr("?", sizeof(
unsigned char), 2u): Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v));{ struct Cyc_Std_String_pa_struct _temp1378;
_temp1378.tag= Cyc_Std_String_pa; _temp1378.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp1377; _temp1377.tag= Cyc_Std_String_pa;
_temp1377.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tuf->name);{ void*
_temp1376[ 3u]={& _temp1377,& _temp1378,& _temp1379}; Cyc_Tcutil_terr( e->loc,
_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u), _tag_arr( _temp1376, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} if( es !=  0){ void* _temp1382=({ struct Cyc_Std_String_pa_struct
_temp1381; _temp1381.tag= Cyc_Std_String_pa; _temp1381.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1380[ 1u]={& _temp1381}; Cyc_Tcexp_expr_err(
te,(( struct Cyc_Absyn_Exp*) es->hd)->loc, _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u), _tag_arr( _temp1380, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1382;} if( ts !=  0){ void* _temp1385=({ struct
Cyc_Std_String_pa_struct _temp1384; _temp1384.tag= Cyc_Std_String_pa; _temp1384.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1383[ 1u]={&
_temp1384}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments for tunion constructor %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1383, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1385;}{ void* _temp1386= res; _npop_handler( 0u);
return _temp1386;}}}}; _pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 
0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1403=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1403[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1404; _temp1404.tag= Cyc_Absyn_RgnHandleType; _temp1404.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp1405=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1405->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1405;})); _temp1404;}); _temp1403;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1393= Cyc_Tcutil_compress( handle_type); void*
_temp1399; _LL1395: if(( unsigned int) _temp1393 >  3u?*(( int*) _temp1393) == 
Cyc_Absyn_RgnHandleType: 0){ _LL1400: _temp1399=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1393)->f1; goto _LL1396;} else{ goto _LL1397;} _LL1397: goto _LL1398;
_LL1396: rgn= _temp1399; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1394; _LL1398:({ struct Cyc_Std_String_pa_struct _temp1402; _temp1402.tag=
Cyc_Std_String_pa; _temp1402.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1401[ 1u]={& _temp1402}; Cyc_Tcutil_terr( ropt->loc,
_tag_arr("expecting region_t type but found %s", sizeof( unsigned char), 37u),
_tag_arr( _temp1401, sizeof( void*), 1u));}}); goto _LL1394; _LL1394:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t);{
void* _temp1406= t; struct Cyc_Absyn_TunionFieldInfo _temp1412; void* _temp1414;
struct Cyc_Absyn_Tunionfield* _temp1416; _LL1408: if(( unsigned int) _temp1406 > 
3u?*(( int*) _temp1406) ==  Cyc_Absyn_TunionFieldType: 0){ _LL1413: _temp1412=((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1406)->f1; _LL1415: _temp1414=(
void*) _temp1412.field_info; if(*(( int*) _temp1414) ==  Cyc_Absyn_KnownTunionfield){
_LL1417: _temp1416=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1414)->f2;
goto _LL1409;} else{ goto _LL1410;}} else{ goto _LL1410;} _LL1410: goto _LL1411;
_LL1409: if( _temp1416->tvs !=  0){({ void* _temp1418[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("malloc with existential types not yet implemented", sizeof(
unsigned char), 50u), _tag_arr( _temp1418, sizeof( void*), 0u));});} goto
_LL1407; _LL1411: goto _LL1407; _LL1407:;}{ void*(* _temp1419)( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt !=  0){ void*
_temp1420= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp1426;
struct Cyc_Absyn_Conref* _temp1428; _LL1422: if(( unsigned int) _temp1420 >  3u?*((
int*) _temp1420) ==  Cyc_Absyn_PointerType: 0){ _LL1427: _temp1426=(( struct Cyc_Absyn_PointerType_struct*)
_temp1420)->f1; _LL1429: _temp1428= _temp1426.nullable; goto _LL1423;} else{
goto _LL1424;} _LL1424: goto _LL1425; _LL1423: if((( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1428)){ _temp1419= Cyc_Absyn_star_typ;} goto
_LL1421; _LL1425: goto _LL1421; _LL1421:;} return _temp1419( t, rgn, Cyc_Absyn_empty_tqual());}}
static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1430=( void*) s->r; struct Cyc_Absyn_Exp* _temp1440;
struct Cyc_Absyn_Stmt* _temp1442; struct Cyc_Absyn_Stmt* _temp1444; struct Cyc_Absyn_Stmt*
_temp1446; struct Cyc_Absyn_Decl* _temp1448; _LL1432: if(( unsigned int)
_temp1430 >  1u?*(( int*) _temp1430) ==  Cyc_Absyn_Exp_s: 0){ _LL1441: _temp1440=((
struct Cyc_Absyn_Exp_s_struct*) _temp1430)->f1; goto _LL1433;} else{ goto
_LL1434;} _LL1434: if(( unsigned int) _temp1430 >  1u?*(( int*) _temp1430) == 
Cyc_Absyn_Seq_s: 0){ _LL1445: _temp1444=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1430)->f1; goto _LL1443; _LL1443: _temp1442=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1430)->f2; goto _LL1435;} else{ goto _LL1436;} _LL1436: if(( unsigned int)
_temp1430 >  1u?*(( int*) _temp1430) ==  Cyc_Absyn_Decl_s: 0){ _LL1449:
_temp1448=(( struct Cyc_Absyn_Decl_s_struct*) _temp1430)->f1; goto _LL1447;
_LL1447: _temp1446=(( struct Cyc_Absyn_Decl_s_struct*) _temp1430)->f2; goto
_LL1437;} else{ goto _LL1438;} _LL1438: goto _LL1439; _LL1433: return( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1440->topt))->v; _LL1435: s= _temp1442;
continue; _LL1437: s= _temp1446; continue; _LL1439: return({ void* _temp1450[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u), _tag_arr( _temp1450, sizeof( void*), 0u));});
_LL1431:;}} static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return({
void* _temp1451[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u), _tag_arr( _temp1451, sizeof( void*), 0u));});}
static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ return({ void* _temp1452[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u),
_tag_arr( _temp1452, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){
void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle !=  0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1463=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1463[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1464; _temp1464.tag= Cyc_Absyn_RgnHandleType;
_temp1464.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt* _temp1465=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1465->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1465;}));
_temp1464;}); _temp1463;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle)); void*
_temp1453= Cyc_Tcutil_compress( handle_type); void* _temp1459; _LL1455: if((
unsigned int) _temp1453 >  3u?*(( int*) _temp1453) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1460: _temp1459=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1453)->f1; goto _LL1456;} else{ goto _LL1457;} _LL1457: goto _LL1458;
_LL1456: rgn= _temp1459; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1454; _LL1458:({ struct Cyc_Std_String_pa_struct _temp1462; _temp1462.tag=
Cyc_Std_String_pa; _temp1462.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1461[ 1u]={& _temp1462}; Cyc_Tcutil_terr( rgn_handle->loc,
_tag_arr("expecting region_t type but found %s", sizeof( unsigned char), 37u),
_tag_arr( _temp1461, sizeof( void*), 1u));}}); goto _LL1454; _LL1454:;}{ void*
_temp1466=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1478; struct Cyc_Absyn_Exp*
_temp1480; struct Cyc_Absyn_Vardecl* _temp1482; struct Cyc_List_List* _temp1484;
struct Cyc_Core_Opt* _temp1486; struct Cyc_List_List* _temp1488; void* _temp1490;
struct _tagged_arr _temp1492; _LL1468: if(*(( int*) _temp1466) ==  Cyc_Absyn_Comprehension_e){
_LL1483: _temp1482=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1466)->f1;
goto _LL1481; _LL1481: _temp1480=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1466)->f2; goto _LL1479; _LL1479: _temp1478=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1466)->f3; goto _LL1469;} else{ goto _LL1470;} _LL1470: if(*(( int*)
_temp1466) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1487: _temp1486=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1466)->f1; goto _LL1485; _LL1485: _temp1484=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1466)->f2; goto _LL1471;} else{ goto _LL1472;} _LL1472: if(*(( int*)
_temp1466) ==  Cyc_Absyn_Array_e){ _LL1489: _temp1488=(( struct Cyc_Absyn_Array_e_struct*)
_temp1466)->f1; goto _LL1473;} else{ goto _LL1474;} _LL1474: if(*(( int*)
_temp1466) ==  Cyc_Absyn_Const_e){ _LL1491: _temp1490=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1466)->f1; if(( unsigned int) _temp1490 >  1u?*(( int*) _temp1490) ==  Cyc_Absyn_String_c:
0){ _LL1493: _temp1492=(( struct Cyc_Absyn_String_c_struct*) _temp1490)->f1;
goto _LL1475;} else{ goto _LL1476;}} else{ goto _LL1476;} _LL1476: goto _LL1477;
_LL1469: { void* _temp1494= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1495= Cyc_Tcutil_compress( _temp1494); struct Cyc_Absyn_Tqual _temp1501;
void* _temp1503; _LL1497: if(( unsigned int) _temp1495 >  3u?*(( int*) _temp1495)
==  Cyc_Absyn_ArrayType: 0){ _LL1504: _temp1503=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1495)->f1; goto _LL1502; _LL1502: _temp1501=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1495)->f2; goto _LL1498;} else{ goto _LL1499;} _LL1499: goto _LL1500;
_LL1498: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1480)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1509=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1509[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1510; _temp1510.tag= Cyc_Absyn_Upper_b; _temp1510.f1=
_temp1480; _temp1510;}); _temp1509;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1506=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1506[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1507; _temp1507.tag= Cyc_Absyn_PointerType;
_temp1507.f1=({ struct Cyc_Absyn_PtrInfo _temp1508; _temp1508.elt_typ=( void*)
_temp1503; _temp1508.rgn_typ=( void*) rgn; _temp1508.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1508.tq= _temp1501; _temp1508.bounds= Cyc_Absyn_new_conref(
b); _temp1508;}); _temp1507;}); _temp1506;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*
topt, res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1505=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1505->v=( void*) res_typ; _temp1505;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_typ=* topt;}} return res_typ;} _LL1500: return({ void*
_temp1511[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u), _tag_arr( _temp1511, sizeof( void*), 0u));});
_LL1496:;} _LL1471:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1512=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1512[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1513; _temp1513.tag= Cyc_Absyn_Array_e;
_temp1513.f1= _temp1484; _temp1513;}); _temp1512;}))); _temp1488= _temp1484;
goto _LL1473; _LL1473: { void** elt_typ_opt= 0; if( topt !=  0){ void* _temp1514=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp1520; struct Cyc_Absyn_Tqual
_temp1522; void* _temp1524; void** _temp1526; _LL1516: if(( unsigned int)
_temp1514 >  3u?*(( int*) _temp1514) ==  Cyc_Absyn_PointerType: 0){ _LL1521:
_temp1520=(( struct Cyc_Absyn_PointerType_struct*) _temp1514)->f1; _LL1525:
_temp1524=( void*) _temp1520.elt_typ; _temp1526=( void**)&((( struct Cyc_Absyn_PointerType_struct*)
_temp1514)->f1).elt_typ; goto _LL1523; _LL1523: _temp1522= _temp1520.tq; goto
_LL1517;} else{ goto _LL1518;} _LL1518: goto _LL1519; _LL1517: elt_typ_opt=(
void**) _temp1526; goto _LL1515; _LL1519: goto _LL1515; _LL1515:;}{ void*
_temp1527= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1528= Cyc_Tcutil_compress( _temp1527); struct Cyc_Absyn_Exp* _temp1534;
struct Cyc_Absyn_Tqual _temp1536; void* _temp1538; _LL1530: if(( unsigned int)
_temp1528 >  3u?*(( int*) _temp1528) ==  Cyc_Absyn_ArrayType: 0){ _LL1539:
_temp1538=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1528)->f1; goto
_LL1537; _LL1537: _temp1536=(( struct Cyc_Absyn_ArrayType_struct*) _temp1528)->f2;
goto _LL1535; _LL1535: _temp1534=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1528)->f3; goto _LL1531;} else{ goto _LL1532;} _LL1532: goto _LL1533;
_LL1531: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1540=(
struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1540[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1541; _temp1541.tag=
Cyc_Absyn_PointerType; _temp1541.f1=({ struct Cyc_Absyn_PtrInfo _temp1542;
_temp1542.elt_typ=( void*) _temp1538; _temp1542.rgn_typ=( void*) rgn; _temp1542.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1542.tq=
_temp1536; _temp1542.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1543=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1543[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1544; _temp1544.tag= Cyc_Absyn_Upper_b;
_temp1544.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1534); _temp1544;});
_temp1543;})); _temp1542;}); _temp1541;}); _temp1540;}); if( topt !=  0){ if( !
Cyc_Tcutil_unify(* topt, res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*
topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1545=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1545->v=( void*) res_typ;
_temp1545;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); res_typ=* topt;}} goto
_LL1529; _LL1533: return({ void* _temp1546[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u), _tag_arr( _temp1546, sizeof( void*), 0u));});
_LL1529:;} return res_typ;}} _LL1475: { void* _temp1547= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b); void* _temp1548= Cyc_Tcexp_tcExp(
te,( void**)& _temp1547, e1); return Cyc_Absyn_atb_typ( _temp1548, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1549=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1549[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1550; _temp1550.tag= Cyc_Absyn_Upper_b; _temp1550.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1550;}); _temp1549;}));} _LL1477: { void** topt2=
0; if( topt !=  0){ void* _temp1551= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp1559; struct Cyc_Absyn_Conref* _temp1561; void* _temp1563; void** _temp1565;
_LL1553: if(( unsigned int) _temp1551 >  3u?*(( int*) _temp1551) ==  Cyc_Absyn_PointerType:
0){ _LL1560: _temp1559=(( struct Cyc_Absyn_PointerType_struct*) _temp1551)->f1;
_LL1564: _temp1563=( void*) _temp1559.elt_typ; _temp1565=( void**)&((( struct
Cyc_Absyn_PointerType_struct*) _temp1551)->f1).elt_typ; goto _LL1562; _LL1562:
_temp1561= _temp1559.nullable; goto _LL1554;} else{ goto _LL1555;} _LL1555: if((
unsigned int) _temp1551 >  3u?*(( int*) _temp1551) ==  Cyc_Absyn_TunionType: 0){
goto _LL1556;} else{ goto _LL1557;} _LL1557: goto _LL1558; _LL1554: topt2=( void**)
_temp1565; goto _LL1552; _LL1556: topt2=({ void** _temp1566=( void**) _cycalloc(
sizeof( void*)); _temp1566[ 0]=* topt; _temp1566;}); goto _LL1552; _LL1558: goto
_LL1552; _LL1552:;}{ void* telt= Cyc_Tcexp_tcExp( te, topt2, e1); void* res_typ=(
void*)({ struct Cyc_Absyn_PointerType_struct* _temp1568=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1568[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1569; _temp1569.tag= Cyc_Absyn_PointerType;
_temp1569.f1=({ struct Cyc_Absyn_PtrInfo _temp1570; _temp1570.elt_typ=( void*)
telt; _temp1570.rgn_typ=( void*) rgn; _temp1570.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1570.tq= Cyc_Absyn_empty_tqual(); _temp1570.bounds=
Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1571=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1571[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1572; _temp1572.tag= Cyc_Absyn_Upper_b;
_temp1572.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1572;}); _temp1571;})); _temp1570;});
_temp1569;}); _temp1568;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(* topt,
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1567=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1567->v=( void*) res_typ; _temp1567;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_typ=* topt;}} return res_typ;}} _LL1467:;}} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote( te, topt, e)); void* _temp1573= t; struct Cyc_Absyn_Exp*
_temp1579; struct Cyc_Absyn_Tqual _temp1581; void* _temp1583; _LL1575: if((
unsigned int) _temp1573 >  3u?*(( int*) _temp1573) ==  Cyc_Absyn_ArrayType: 0){
_LL1584: _temp1583=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1573)->f1;
goto _LL1582; _LL1582: _temp1581=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1573)->f2; goto _LL1580; _LL1580: _temp1579=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1573)->f3; goto _LL1576;} else{ goto _LL1577;} _LL1577: goto _LL1578;
_LL1576: { void* _temp1587; struct _tuple6 _temp1585= Cyc_Tcutil_addressof_props(
te, e); _LL1588: _temp1587= _temp1585.f2; goto _LL1586; _LL1586: { void*
_temp1589= _temp1579 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1590=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1590[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1591; _temp1591.tag= Cyc_Absyn_Upper_b;
_temp1591.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1579); _temp1591;});
_temp1590;}); t= Cyc_Absyn_atb_typ( _temp1583, _temp1587, _temp1581, _temp1589);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1578: return t; _LL1574:;} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1592=( void*) e->r; void* _temp1602; _LL1594: if(*(( int*)
_temp1592) ==  Cyc_Absyn_Array_e){ goto _LL1595;} else{ goto _LL1596;} _LL1596:
if(*(( int*) _temp1592) ==  Cyc_Absyn_Comprehension_e){ goto _LL1597;} else{
goto _LL1598;} _LL1598: if(*(( int*) _temp1592) ==  Cyc_Absyn_Const_e){ _LL1603:
_temp1602=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1592)->f1; if((
unsigned int) _temp1602 >  1u?*(( int*) _temp1602) ==  Cyc_Absyn_String_c: 0){
goto _LL1599;} else{ goto _LL1600;}} else{ goto _LL1600;} _LL1600: goto _LL1601;
_LL1595: return t; _LL1597: return t; _LL1599: return t; _LL1601: t= Cyc_Tcutil_compress(
t);{ void* _temp1604= t; struct Cyc_Absyn_Exp* _temp1610; struct Cyc_Absyn_Tqual
_temp1612; void* _temp1614; _LL1606: if(( unsigned int) _temp1604 >  3u?*(( int*)
_temp1604) ==  Cyc_Absyn_ArrayType: 0){ _LL1615: _temp1614=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1604)->f1; goto _LL1613; _LL1613: _temp1612=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1604)->f2; goto _LL1611; _LL1611: _temp1610=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1604)->f3; goto _LL1607;} else{ goto _LL1608;} _LL1608: goto _LL1609;
_LL1607: { void* _temp1618; struct _tuple6 _temp1616= Cyc_Tcutil_addressof_props(
te, e); _LL1619: _temp1618= _temp1616.f2; goto _LL1617; _LL1617: { void* b=
_temp1610 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1620=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1620[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1621; _temp1621.tag= Cyc_Absyn_Upper_b;
_temp1621.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1610); _temp1621;});
_temp1620;}); t= Cyc_Absyn_atb_typ( _temp1614, _temp1618, _temp1612, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1609: return t; _LL1605:;} _LL1593:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1622=( void*) e->r; struct Cyc_Absyn_Exp* _temp1628; _LL1624: if(*(( int*)
_temp1622) ==  Cyc_Absyn_NoInstantiate_e){ _LL1629: _temp1628=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1622)->f1; goto _LL1625;} else{ goto _LL1626;} _LL1626: goto _LL1627;
_LL1625: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1628);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1628->topt))->v=( void*) Cyc_Absyn_pointer_expand(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1628->topt))->v)); e->topt= _temp1628->topt;
goto _LL1623; _LL1627: Cyc_Tcexp_tcExpNoInst( te, topt, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1630=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct Cyc_Absyn_PtrInfo _temp1636; struct Cyc_Absyn_Conref* _temp1638; struct
Cyc_Absyn_Tqual _temp1640; struct Cyc_Absyn_Conref* _temp1642; void* _temp1644;
void* _temp1646; _LL1632: if(( unsigned int) _temp1630 >  3u?*(( int*) _temp1630)
==  Cyc_Absyn_PointerType: 0){ _LL1637: _temp1636=(( struct Cyc_Absyn_PointerType_struct*)
_temp1630)->f1; _LL1647: _temp1646=( void*) _temp1636.elt_typ; goto _LL1645;
_LL1645: _temp1644=( void*) _temp1636.rgn_typ; goto _LL1643; _LL1643: _temp1642=
_temp1636.nullable; goto _LL1641; _LL1641: _temp1640= _temp1636.tq; goto _LL1639;
_LL1639: _temp1638= _temp1636.bounds; goto _LL1633;} else{ goto _LL1634;}
_LL1634: goto _LL1635; _LL1633:{ void* _temp1648= Cyc_Tcutil_compress( _temp1646);
struct Cyc_Absyn_FnInfo _temp1654; struct Cyc_List_List* _temp1656; struct Cyc_List_List*
_temp1658; struct Cyc_Absyn_VarargInfo* _temp1660; int _temp1662; struct Cyc_List_List*
_temp1664; void* _temp1666; struct Cyc_Core_Opt* _temp1668; struct Cyc_List_List*
_temp1670; _LL1650: if(( unsigned int) _temp1648 >  3u?*(( int*) _temp1648) == 
Cyc_Absyn_FnType: 0){ _LL1655: _temp1654=(( struct Cyc_Absyn_FnType_struct*)
_temp1648)->f1; _LL1671: _temp1670= _temp1654.tvars; goto _LL1669; _LL1669:
_temp1668= _temp1654.effect; goto _LL1667; _LL1667: _temp1666=( void*) _temp1654.ret_typ;
goto _LL1665; _LL1665: _temp1664= _temp1654.args; goto _LL1663; _LL1663:
_temp1662= _temp1654.c_varargs; goto _LL1661; _LL1661: _temp1660= _temp1654.cyc_varargs;
goto _LL1659; _LL1659: _temp1658= _temp1654.rgn_po; goto _LL1657; _LL1657:
_temp1656= _temp1654.attributes; goto _LL1651;} else{ goto _LL1652;} _LL1652:
goto _LL1653; _LL1651: if( _temp1670 !=  0){ struct _RegionHandle _temp1672=
_new_region(); struct _RegionHandle* rgn=& _temp1672; _push_region( rgn);{
struct _tuple4 _temp1673=({ struct _tuple4 _temp1683; _temp1683.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1683.f2= rgn; _temp1683;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1673, _temp1670); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1680=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1680[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1681; _temp1681.tag= Cyc_Absyn_FnType; _temp1681.f1=({ struct Cyc_Absyn_FnInfo
_temp1682; _temp1682.tvars= 0; _temp1682.effect= _temp1668; _temp1682.ret_typ=(
void*) _temp1666; _temp1682.args= _temp1664; _temp1682.c_varargs= _temp1662;
_temp1682.cyc_varargs= _temp1660; _temp1682.rgn_po= _temp1658; _temp1682.attributes=
_temp1656; _temp1682;}); _temp1681;}); _temp1680;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1677=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1677[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1678; _temp1678.tag= Cyc_Absyn_PointerType;
_temp1678.f1=({ struct Cyc_Absyn_PtrInfo _temp1679; _temp1679.elt_typ=( void*)
ftyp; _temp1679.rgn_typ=( void*) _temp1644; _temp1679.nullable= _temp1642;
_temp1679.tq= _temp1640; _temp1679.bounds= _temp1638; _temp1679;}); _temp1678;});
_temp1677;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1674=( struct Cyc_Absyn_Instantiate_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1674[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1675; _temp1675.tag= Cyc_Absyn_Instantiate_e;
_temp1675.f1= inner; _temp1675.f2= ts; _temp1675;}); _temp1674;}))); e->topt=({
struct Cyc_Core_Opt* _temp1676=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1676->v=( void*) new_typ; _temp1676;});}; _pop_region( rgn);}
goto _LL1649; _LL1653: goto _LL1649; _LL1649:;} goto _LL1631; _LL1635: goto
_LL1631; _LL1631:;} goto _LL1623; _LL1623:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1684=( void*) e->r; struct Cyc_Absyn_Exp* _temp1762; struct
_tuple1* _temp1764; struct Cyc_List_List* _temp1766; struct Cyc_Absyn_Exp*
_temp1768; struct Cyc_List_List* _temp1770; struct Cyc_Core_Opt* _temp1772; void*
_temp1774; void* _temp1776; struct _tuple1* _temp1778; struct Cyc_List_List*
_temp1780; void* _temp1782; void* _temp1784; struct Cyc_Absyn_Exp* _temp1786;
struct Cyc_Absyn_Exp* _temp1788; struct Cyc_Core_Opt* _temp1790; struct Cyc_Absyn_Exp*
_temp1792; struct Cyc_Absyn_Exp* _temp1794; struct Cyc_Absyn_Exp* _temp1796;
struct Cyc_Absyn_Exp* _temp1798; struct Cyc_Absyn_Exp* _temp1800; struct Cyc_Absyn_Exp*
_temp1802; struct Cyc_Absyn_VarargCallInfo* _temp1804; struct Cyc_Absyn_VarargCallInfo**
_temp1806; struct Cyc_List_List* _temp1807; struct Cyc_Absyn_Exp* _temp1809;
struct Cyc_Absyn_Exp* _temp1811; struct Cyc_List_List* _temp1813; struct Cyc_Absyn_Exp*
_temp1815; struct Cyc_Absyn_Exp* _temp1817; void* _temp1819; struct Cyc_Absyn_Exp*
_temp1821; struct Cyc_Absyn_Exp* _temp1823; struct Cyc_Absyn_Exp* _temp1825;
struct Cyc_Absyn_Exp* _temp1827; void* _temp1829; void* _temp1831; void*
_temp1833; struct Cyc_Absyn_Exp* _temp1835; struct _tagged_arr* _temp1837;
struct Cyc_Absyn_Exp* _temp1839; struct _tagged_arr* _temp1841; struct Cyc_Absyn_Exp*
_temp1843; struct Cyc_Absyn_Exp* _temp1845; struct Cyc_Absyn_Exp* _temp1847;
struct Cyc_List_List* _temp1849; struct Cyc_List_List* _temp1851; struct _tuple2*
_temp1853; struct Cyc_List_List* _temp1855; struct Cyc_Absyn_Stmt* _temp1857;
struct Cyc_Absyn_Fndecl* _temp1859; struct Cyc_Absyn_Exp* _temp1861; struct Cyc_Absyn_Exp*
_temp1863; struct Cyc_Absyn_Exp* _temp1865; struct Cyc_Absyn_Vardecl* _temp1867;
struct Cyc_Absyn_Structdecl* _temp1869; struct Cyc_Absyn_Structdecl** _temp1871;
struct Cyc_List_List* _temp1872; struct Cyc_Core_Opt* _temp1874; struct Cyc_Core_Opt**
_temp1876; struct _tuple1* _temp1877; struct _tuple1** _temp1879; struct Cyc_List_List*
_temp1880; void* _temp1882; struct Cyc_Absyn_Tunionfield* _temp1884; struct Cyc_Absyn_Tuniondecl*
_temp1886; struct Cyc_List_List* _temp1888; struct Cyc_Core_Opt* _temp1890;
struct Cyc_Core_Opt** _temp1892; struct Cyc_Core_Opt* _temp1893; struct Cyc_Core_Opt**
_temp1895; struct Cyc_Absyn_Enumfield* _temp1896; struct Cyc_Absyn_Enumdecl*
_temp1898; struct _tuple1* _temp1900; struct _tuple1** _temp1902; struct Cyc_Absyn_Enumfield*
_temp1903; void* _temp1905; struct _tuple1* _temp1907; struct _tuple1**
_temp1909; void* _temp1910; struct Cyc_Absyn_Exp* _temp1912; _LL1686: if(*(( int*)
_temp1684) ==  Cyc_Absyn_NoInstantiate_e){ _LL1763: _temp1762=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1684)->f1; goto _LL1687;} else{ goto _LL1688;} _LL1688: if(*(( int*)
_temp1684) ==  Cyc_Absyn_UnknownId_e){ _LL1765: _temp1764=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1684)->f1; goto _LL1689;} else{ goto _LL1690;} _LL1690: if(*(( int*)
_temp1684) ==  Cyc_Absyn_UnknownCall_e){ _LL1769: _temp1768=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1684)->f1; goto _LL1767; _LL1767: _temp1766=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1684)->f2; goto _LL1691;} else{ goto _LL1692;} _LL1692: if(*(( int*)
_temp1684) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1773: _temp1772=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1684)->f1; goto _LL1771; _LL1771: _temp1770=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1684)->f2; goto _LL1693;} else{ goto _LL1694;} _LL1694: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Const_e){ _LL1775: _temp1774=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1684)->f1; goto _LL1695;} else{ goto _LL1696;} _LL1696: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Var_e){ _LL1779: _temp1778=(( struct Cyc_Absyn_Var_e_struct*)
_temp1684)->f1; goto _LL1777; _LL1777: _temp1776=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1684)->f2; goto _LL1697;} else{ goto _LL1698;} _LL1698: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Primop_e){ _LL1783: _temp1782=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1684)->f1; goto _LL1781; _LL1781: _temp1780=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1684)->f2; goto _LL1699;} else{ goto _LL1700;} _LL1700: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Increment_e){ _LL1787: _temp1786=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1684)->f1; goto _LL1785; _LL1785: _temp1784=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1684)->f2; goto _LL1701;} else{ goto _LL1702;} _LL1702: if(*(( int*)
_temp1684) ==  Cyc_Absyn_AssignOp_e){ _LL1793: _temp1792=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1684)->f1; goto _LL1791; _LL1791: _temp1790=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1684)->f2; goto _LL1789; _LL1789: _temp1788=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1684)->f3; goto _LL1703;} else{ goto _LL1704;} _LL1704: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Conditional_e){ _LL1799: _temp1798=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1684)->f1; goto _LL1797; _LL1797: _temp1796=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1684)->f2; goto _LL1795; _LL1795: _temp1794=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1684)->f3; goto _LL1705;} else{ goto _LL1706;} _LL1706: if(*(( int*)
_temp1684) ==  Cyc_Absyn_SeqExp_e){ _LL1803: _temp1802=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1684)->f1; goto _LL1801; _LL1801: _temp1800=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1684)->f2; goto _LL1707;} else{ goto _LL1708;} _LL1708: if(*(( int*)
_temp1684) ==  Cyc_Absyn_FnCall_e){ _LL1810: _temp1809=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1684)->f1; goto _LL1808; _LL1808: _temp1807=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1684)->f2; goto _LL1805; _LL1805: _temp1804=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1684)->f3; _temp1806=( struct Cyc_Absyn_VarargCallInfo**)&(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1684)->f3; goto _LL1709;} else{ goto _LL1710;} _LL1710: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Throw_e){ _LL1812: _temp1811=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1684)->f1; goto _LL1711;} else{ goto _LL1712;} _LL1712: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Instantiate_e){ _LL1816: _temp1815=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1684)->f1; goto _LL1814; _LL1814: _temp1813=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1684)->f2; goto _LL1713;} else{ goto _LL1714;} _LL1714: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Cast_e){ _LL1820: _temp1819=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1684)->f1; goto _LL1818; _LL1818: _temp1817=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1684)->f2; goto _LL1715;} else{ goto _LL1716;} _LL1716: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Address_e){ _LL1822: _temp1821=(( struct Cyc_Absyn_Address_e_struct*)
_temp1684)->f1; goto _LL1717;} else{ goto _LL1718;} _LL1718: if(*(( int*)
_temp1684) ==  Cyc_Absyn_New_e){ _LL1826: _temp1825=(( struct Cyc_Absyn_New_e_struct*)
_temp1684)->f1; goto _LL1824; _LL1824: _temp1823=(( struct Cyc_Absyn_New_e_struct*)
_temp1684)->f2; goto _LL1719;} else{ goto _LL1720;} _LL1720: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Sizeofexp_e){ _LL1828: _temp1827=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1684)->f1; goto _LL1721;} else{ goto _LL1722;} _LL1722: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1830: _temp1829=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1684)->f1; goto _LL1723;} else{ goto _LL1724;} _LL1724: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Offsetof_e){ _LL1834: _temp1833=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1684)->f1; goto _LL1832; _LL1832: _temp1831=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1684)->f2; goto _LL1725;} else{ goto _LL1726;} _LL1726: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Gentyp_e){ goto _LL1727;} else{ goto _LL1728;} _LL1728:
if(*(( int*) _temp1684) ==  Cyc_Absyn_Deref_e){ _LL1836: _temp1835=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1684)->f1; goto _LL1729;} else{ goto _LL1730;} _LL1730: if(*(( int*)
_temp1684) ==  Cyc_Absyn_StructMember_e){ _LL1840: _temp1839=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1684)->f1; goto _LL1838; _LL1838: _temp1837=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1684)->f2; goto _LL1731;} else{ goto _LL1732;} _LL1732: if(*(( int*)
_temp1684) ==  Cyc_Absyn_StructArrow_e){ _LL1844: _temp1843=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1684)->f1; goto _LL1842; _LL1842: _temp1841=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1684)->f2; goto _LL1733;} else{ goto _LL1734;} _LL1734: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Subscript_e){ _LL1848: _temp1847=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1684)->f1; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1684)->f2; goto _LL1735;} else{ goto _LL1736;} _LL1736: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Tuple_e){ _LL1850: _temp1849=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1684)->f1; goto _LL1737;} else{ goto _LL1738;} _LL1738: if(*(( int*)
_temp1684) ==  Cyc_Absyn_CompoundLit_e){ _LL1854: _temp1853=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1684)->f1; goto _LL1852; _LL1852: _temp1851=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1684)->f2; goto _LL1739;} else{ goto _LL1740;} _LL1740: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Array_e){ _LL1856: _temp1855=(( struct Cyc_Absyn_Array_e_struct*)
_temp1684)->f1; goto _LL1741;} else{ goto _LL1742;} _LL1742: if(*(( int*)
_temp1684) ==  Cyc_Absyn_StmtExp_e){ _LL1858: _temp1857=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1684)->f1; goto _LL1743;} else{ goto _LL1744;} _LL1744: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Codegen_e){ _LL1860: _temp1859=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1684)->f1; goto _LL1745;} else{ goto _LL1746;} _LL1746: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Fill_e){ _LL1862: _temp1861=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1684)->f1; goto _LL1747;} else{ goto _LL1748;} _LL1748: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Comprehension_e){ _LL1868: _temp1867=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1684)->f1; goto _LL1866; _LL1866: _temp1865=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1684)->f2; goto _LL1864; _LL1864: _temp1863=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1684)->f3; goto _LL1749;} else{ goto _LL1750;} _LL1750: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Struct_e){ _LL1878: _temp1877=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1684)->f1; _temp1879=( struct _tuple1**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1684)->f1; goto _LL1875; _LL1875: _temp1874=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1684)->f2; _temp1876=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1684)->f2; goto _LL1873; _LL1873: _temp1872=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1684)->f3; goto _LL1870; _LL1870: _temp1869=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1684)->f4; _temp1871=( struct Cyc_Absyn_Structdecl**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1684)->f4; goto _LL1751;} else{ goto _LL1752;} _LL1752: if(*(( int*)
_temp1684) ==  Cyc_Absyn_AnonStruct_e){ _LL1883: _temp1882=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1684)->f1; goto _LL1881; _LL1881: _temp1880=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1684)->f2; goto _LL1753;} else{ goto _LL1754;} _LL1754: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Tunion_e){ _LL1894: _temp1893=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1684)->f1; _temp1895=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1684)->f1; goto _LL1891; _LL1891: _temp1890=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1684)->f2; _temp1892=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1684)->f2; goto _LL1889; _LL1889: _temp1888=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1684)->f3; goto _LL1887; _LL1887: _temp1886=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1684)->f4; goto _LL1885; _LL1885: _temp1884=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1684)->f5; goto _LL1755;} else{ goto _LL1756;} _LL1756: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Enum_e){ _LL1901: _temp1900=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1684)->f1; _temp1902=( struct _tuple1**)&(( struct Cyc_Absyn_Enum_e_struct*)
_temp1684)->f1; goto _LL1899; _LL1899: _temp1898=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1684)->f2; goto _LL1897; _LL1897: _temp1896=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1684)->f3; goto _LL1757;} else{ goto _LL1758;} _LL1758: if(*(( int*)
_temp1684) ==  Cyc_Absyn_AnonEnum_e){ _LL1908: _temp1907=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1684)->f1; _temp1909=( struct _tuple1**)&(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1684)->f1; goto _LL1906; _LL1906: _temp1905=( void*)(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1684)->f2; goto _LL1904; _LL1904: _temp1903=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1684)->f3; goto _LL1759;} else{ goto _LL1760;} _LL1760: if(*(( int*)
_temp1684) ==  Cyc_Absyn_Malloc_e){ _LL1913: _temp1912=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1684)->f1; goto _LL1911; _LL1911: _temp1910=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1684)->f2; goto _LL1761;} else{ goto _LL1685;} _LL1687: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1762); return; _LL1689: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1764); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1691: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1768, _temp1766); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1693: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1770); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1695: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1774, e);
goto _LL1685; _LL1697: t= Cyc_Tcexp_tcVar( te, loc, _temp1778, _temp1776); goto
_LL1685; _LL1699: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1782, _temp1780);
goto _LL1685; _LL1701: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1786,
_temp1784); goto _LL1685; _LL1703: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1792, _temp1790, _temp1788); goto _LL1685; _LL1705: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1798, _temp1796, _temp1794); goto _LL1685; _LL1707: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1802, _temp1800); goto _LL1685; _LL1709: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1809, _temp1807, _temp1806); goto _LL1685; _LL1711: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1811); goto _LL1685; _LL1713: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1815, _temp1813); goto _LL1685; _LL1715: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1819, _temp1817); goto _LL1685; _LL1717: t= Cyc_Tcexp_tcAddress(
te, loc, e, topt, _temp1821); goto _LL1685; _LL1719: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1825, e, _temp1823); goto _LL1685; _LL1721: { void* _temp1914= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1827); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1914); goto
_LL1685;} _LL1723: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1829); goto
_LL1685; _LL1725: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1833, _temp1831);
goto _LL1685; _LL1727:({ void* _temp1915[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("gen() not in top-level initializer",
sizeof( unsigned char), 35u), _tag_arr( _temp1915, sizeof( void*), 0u));});
return; _LL1729: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1835); goto _LL1685;
_LL1731: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1839, _temp1837);
goto _LL1685; _LL1733: t= Cyc_Tcexp_tcStructArrow( te, loc, topt, _temp1843,
_temp1841); goto _LL1685; _LL1735: t= Cyc_Tcexp_tcSubscript( te, loc, topt,
_temp1847, _temp1845); goto _LL1685; _LL1737: t= Cyc_Tcexp_tcTuple( te, loc,
topt, _temp1849); goto _LL1685; _LL1739: t= Cyc_Tcexp_tcCompoundLit( te, loc,
topt, _temp1853, _temp1851); goto _LL1685; _LL1741: { void** elt_topt= 0; if(
topt !=  0){ void* _temp1916= Cyc_Tcutil_compress(* topt); void* _temp1922; void**
_temp1924; _LL1918: if(( unsigned int) _temp1916 >  3u?*(( int*) _temp1916) == 
Cyc_Absyn_ArrayType: 0){ _LL1923: _temp1922=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1916)->f1; _temp1924=( void**)&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1916)->f1); goto _LL1919;} else{ goto _LL1920;} _LL1920: goto _LL1921;
_LL1919: elt_topt=( void**) _temp1924; goto _LL1917; _LL1921: goto _LL1917;
_LL1917:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp1855); goto _LL1685;}
_LL1743: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp1857); goto _LL1685;
_LL1745: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp1859); goto _LL1685;
_LL1747: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1861); goto _LL1685; _LL1749:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1867, _temp1865, _temp1863);
goto _LL1685; _LL1751: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp1879,
_temp1876, _temp1872, _temp1871); goto _LL1685; _LL1753: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1882, _temp1880); goto _LL1685; _LL1755: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1895, _temp1892, _temp1888, _temp1886, _temp1884); goto
_LL1685; _LL1757:* _temp1902=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1896))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1925=(
struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1925[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1926; _temp1926.tag= Cyc_Absyn_EnumType;
_temp1926.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1898))->name;
_temp1926.f2= _temp1898; _temp1926;}); _temp1925;}); goto _LL1685; _LL1759:*
_temp1909=(( struct Cyc_Absyn_Enumfield*) _check_null( _temp1903))->name; t=
_temp1905; goto _LL1685; _LL1761: t= Cyc_Tcexp_tcMalloc( te, loc, topt,
_temp1912, _temp1910); goto _LL1685; _LL1685:;} e->topt=({ struct Cyc_Core_Opt*
_temp1927=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1927->v=( void*) t; _temp1927;});}
