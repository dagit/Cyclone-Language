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
void* hd; struct Cyc_List_List* tl; } ; extern void* Cyc_List_hd( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
extern int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List* l,
void* x); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* cmp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern struct Cyc_List_List* Cyc_Set_elements( struct Cyc_Set_Set*
s); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k, void* v);
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k); extern void*
Cyc_Dict_fold_c( void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d, void* accum); extern void Cyc_Dict_iter_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct
Cyc_Dict_Dict* Cyc_Dict_union_two_c( void*(* f)( void*, void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct
Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern int Cyc_Dict_forall_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern int
Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0;
struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref;
static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1;
static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0;
static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const
int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
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
f3; } ; static const int Cyc_Absyn_EnumType= 11; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_AnonStructType= 12; struct Cyc_Absyn_AnonStructType_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType= 13;
struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_AnonEnumType= 14; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
15; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 16; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; static const
int Cyc_Absyn_Skip_s= 0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1;
struct Cyc_Absyn_Seq_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_IfThenElse_s=
3; struct Cyc_Absyn_IfThenElse_s_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3; } ; static const int Cyc_Absyn_While_s=
4; struct _tuple2{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Break_s= 5; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Continue_s= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Goto_s= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_For_s=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4; } ; static const int
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
tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s=
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
} ; extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt, struct
_tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
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
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_bits_only(
void* t); static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ;
static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; unsigned char Cyc_CfFlowInfo_IsZero[
11u]="\000\000\000\000IsZero"; unsigned char Cyc_CfFlowInfo_NotZero[ 12u]="\000\000\000\000NotZero";
unsigned char Cyc_CfFlowInfo_UnknownZ[ 13u]="\000\000\000\000UnknownZ"; static
const int Cyc_CfFlowInfo_PlaceL= 0; struct Cyc_CfFlowInfo_PlaceL_struct{ int tag;
struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_UnknownL= 0;
static const int Cyc_CfFlowInfo_Zero= 0; static const int Cyc_CfFlowInfo_NotZeroAll=
1; static const int Cyc_CfFlowInfo_NotZeroThis= 2; static const int Cyc_CfFlowInfo_UnknownR=
0; struct Cyc_CfFlowInfo_UnknownR_struct{ int tag; void* f1; } ; static const
int Cyc_CfFlowInfo_Esc= 1; struct Cyc_CfFlowInfo_Esc_struct{ int tag; void* f1;
} ; static const int Cyc_CfFlowInfo_AddressOf= 2; struct Cyc_CfFlowInfo_AddressOf_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_Aggregate=
3; struct Cyc_CfFlowInfo_Aggregate_struct{ int tag; struct Cyc_Dict_Dict* f1; }
; static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_ReachableFL=
0; struct Cyc_CfFlowInfo_ReachableFL_struct{ int tag; struct Cyc_Dict_Dict* f1;
} ; extern struct Cyc_Set_Set* Cyc_CfFlowInfo_mt_place_set(); extern void* Cyc_CfFlowInfo_unknown_none;
extern void* Cyc_CfFlowInfo_unknown_this; extern void* Cyc_CfFlowInfo_unknown_all;
extern int Cyc_CfFlowInfo_root_cmp( void*, void*); extern int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*); extern void* Cyc_CfFlowInfo_typ_to_absrval(
void* t, void* leafval); extern void* Cyc_CfFlowInfo_initlevel( struct Cyc_Dict_Dict*
d, void* r); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place); extern int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed, void* r); extern
struct Cyc_Dict_Dict* Cyc_CfFlowInfo_assign_place( struct Cyc_Position_Segment*
loc, struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed, struct Cyc_CfFlowInfo_Place*
place, void* r); extern void* Cyc_CfFlowInfo_join_flow( struct Cyc_Set_Set**,
void*, void*); extern void* Cyc_CfFlowInfo_after_flow( struct Cyc_Set_Set**,
void*, void*, struct Cyc_Set_Set*, struct Cyc_Set_Set*); static struct Cyc_CfFlowInfo_UnknownR_struct
Cyc_CfFlowInfo_unknown_none_v={ 0u,( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_UnknownR_struct
Cyc_CfFlowInfo_unknown_this_v={ 0u,( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_UnknownR_struct
Cyc_CfFlowInfo_unknown_all_v={ 0u,( void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_Esc_struct
Cyc_CfFlowInfo_esc_none_v={ 1u,( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_Esc_struct
Cyc_CfFlowInfo_esc_this_v={ 1u,( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_Esc_struct
Cyc_CfFlowInfo_esc_all_v={ 1u,( void*)(( void*) 2u)}; void* Cyc_CfFlowInfo_unknown_none=(
void*)& Cyc_CfFlowInfo_unknown_none_v; void* Cyc_CfFlowInfo_unknown_this=( void*)&
Cyc_CfFlowInfo_unknown_this_v; void* Cyc_CfFlowInfo_unknown_all=( void*)& Cyc_CfFlowInfo_unknown_all_v;
void* Cyc_CfFlowInfo_esc_none=( void*)& Cyc_CfFlowInfo_esc_none_v; void* Cyc_CfFlowInfo_esc_this=(
void*)& Cyc_CfFlowInfo_esc_this_v; void* Cyc_CfFlowInfo_esc_all=( void*)& Cyc_CfFlowInfo_esc_all_v;
static struct Cyc_Set_Set** Cyc_CfFlowInfo_mt_place_set_opt= 0; struct Cyc_Set_Set*
Cyc_CfFlowInfo_mt_place_set(){ if( Cyc_CfFlowInfo_mt_place_set_opt ==  0){ Cyc_CfFlowInfo_mt_place_set_opt=({
struct Cyc_Set_Set** _temp6=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp6[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp);
_temp6;});} return*(( struct Cyc_Set_Set**) _check_null( Cyc_CfFlowInfo_mt_place_set_opt));}
int Cyc_CfFlowInfo_root_cmp( void* r1, void* r2){ if(( int) r1 == ( int) r2){
return 0;}{ int p1; int p2;{ void* _temp7= r1; struct Cyc_Absyn_Vardecl* _temp13;
struct Cyc_Absyn_Exp* _temp15; _LL9: if(*(( int*) _temp7) ==  Cyc_CfFlowInfo_VarRoot){
_LL14: _temp13=(( struct Cyc_CfFlowInfo_VarRoot_struct*) _temp7)->f1; goto _LL10;}
else{ goto _LL11;} _LL11: if(*(( int*) _temp7) ==  Cyc_CfFlowInfo_MallocPt){
_LL16: _temp15=(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp7)->f1; goto
_LL12;} else{ goto _LL8;} _LL10: p1=( int) _temp13; goto _LL8; _LL12: p1=( int)
_temp15; _LL8:;}{ void* _temp17= r2; struct Cyc_Absyn_Vardecl* _temp23; struct
Cyc_Absyn_Exp* _temp25; _LL19: if(*(( int*) _temp17) ==  Cyc_CfFlowInfo_VarRoot){
_LL24: _temp23=(( struct Cyc_CfFlowInfo_VarRoot_struct*) _temp17)->f1; goto
_LL20;} else{ goto _LL21;} _LL21: if(*(( int*) _temp17) ==  Cyc_CfFlowInfo_MallocPt){
_LL26: _temp25=(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp17)->f1; goto
_LL22;} else{ goto _LL18;} _LL20: p2=( int) _temp23; goto _LL18; _LL22: p2=( int)
_temp25; _LL18:;} return p1 -  p2;}} int Cyc_CfFlowInfo_place_cmp( struct Cyc_CfFlowInfo_Place*
p1, struct Cyc_CfFlowInfo_Place* p2){ if(( int) p1 == ( int) p2){ return 0;}{
int i= Cyc_CfFlowInfo_root_cmp(( void*) p1->root,( void*) p2->root); if( i !=  0){
return i;} return(( int(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_zstrptrcmp,
p1->fields, p2->fields);}} static struct _tagged_arr* Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place* p){ struct Cyc_List_List* sl= 0;{ void* _temp27=(
void*) p->root; struct Cyc_Absyn_Vardecl* _temp33; struct Cyc_Absyn_Exp* _temp35;
_LL29: if(*(( int*) _temp27) ==  Cyc_CfFlowInfo_VarRoot){ _LL34: _temp33=((
struct Cyc_CfFlowInfo_VarRoot_struct*) _temp27)->f1; goto _LL30;} else{ goto
_LL31;} _LL31: if(*(( int*) _temp27) ==  Cyc_CfFlowInfo_MallocPt){ _LL36:
_temp35=(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp27)->f1; goto _LL32;}
else{ goto _LL28;} _LL30: sl=({ struct Cyc_List_List* _temp37=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp37->hd=( void*)({ struct
_tagged_arr* _temp38=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp38[ 0]=({ struct Cyc_Std_String_pa_struct _temp40; _temp40.tag=
Cyc_Std_String_pa; _temp40.f1=( struct _tagged_arr)*(* _temp33->name).f2;{ void*
_temp39[ 1u]={& _temp40}; Cyc_Std_aprintf( _tag_arr("%s", sizeof( unsigned char),
3u), _tag_arr( _temp39, sizeof( void*), 1u));}}); _temp38;}); _temp37->tl= sl;
_temp37;}); goto _LL28; _LL32: sl=({ struct Cyc_List_List* _temp41=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp41->hd=( void*)({ struct
_tagged_arr* _temp42=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp42[ 0]=({ struct Cyc_Std_Int_pa_struct _temp44; _temp44.tag=
Cyc_Std_Int_pa; _temp44.f1=( int)(( unsigned int)(( int) _temp35));{ void*
_temp43[ 1u]={& _temp44}; Cyc_Std_aprintf( _tag_arr("mpt%d", sizeof(
unsigned char), 6u), _tag_arr( _temp43, sizeof( void*), 1u));}}); _temp42;});
_temp41->tl= sl; _temp41;}); _LL28:;}{ struct Cyc_List_List* fields= p->fields;
for( 0; fields !=  0; fields= fields->tl){ sl=({ struct Cyc_List_List* _temp45=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp45->hd=(
void*)({ struct _tagged_arr* _temp46=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp46[ 0]=({ struct Cyc_Std_String_pa_struct _temp48;
_temp48.tag= Cyc_Std_String_pa; _temp48.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) fields->hd);{ void* _temp47[ 1u]={& _temp48}; Cyc_Std_aprintf(
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp47, sizeof( void*), 1u));}});
_temp46;}); _temp45->tl= sl; _temp45;});}}{ struct _tagged_arr* _temp49=({
struct _tagged_arr* _temp53=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp53[ 0]=({ struct Cyc_Std_String_pa_struct _temp55; _temp55.tag=
Cyc_Std_String_pa; _temp55.f1= _tag_arr("", sizeof( unsigned char), 1u);{ void*
_temp54[ 1u]={& _temp55}; Cyc_Std_aprintf( _tag_arr("%s", sizeof( unsigned char),
3u), _tag_arr( _temp54, sizeof( void*), 1u));}}); _temp53;}); for( 0; sl !=  0;
sl= sl->tl){* _temp49=({ struct Cyc_Std_String_pa_struct _temp52; _temp52.tag=
Cyc_Std_String_pa; _temp52.f1=( struct _tagged_arr)* _temp49;{ struct Cyc_Std_String_pa_struct
_temp51; _temp51.tag= Cyc_Std_String_pa; _temp51.f1=( struct _tagged_arr)*((
struct _tagged_arr*) sl->hd);{ void* _temp50[ 2u]={& _temp51,& _temp52}; Cyc_Std_aprintf(
_tag_arr("%s.%s", sizeof( unsigned char), 6u), _tag_arr( _temp50, sizeof( void*),
2u));}}});} return _temp49;}} struct _tuple3{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; void* Cyc_CfFlowInfo_typ_to_absrval( void* t, void* leafval){ void*
_temp56= Cyc_Tcutil_compress( t); struct Cyc_Absyn_TunionFieldInfo _temp68; void*
_temp70; struct Cyc_Absyn_Tunionfield* _temp72; struct Cyc_List_List* _temp74;
struct Cyc_List_List* _temp76; struct Cyc_Absyn_Structdecl** _temp78; struct Cyc_Absyn_Structdecl*
_temp80; struct Cyc_Absyn_Structdecl _temp81; struct Cyc_Core_Opt* _temp82;
struct Cyc_Core_Opt _temp84; struct Cyc_List_List* _temp85; _LL58: if((
unsigned int) _temp56 >  4u?*(( int*) _temp56) ==  Cyc_Absyn_TunionFieldType: 0){
_LL69: _temp68=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp56)->f1; _LL71:
_temp70=( void*) _temp68.field_info; if(*(( int*) _temp70) ==  Cyc_Absyn_KnownTunionfield){
_LL73: _temp72=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp70)->f2; goto
_LL59;} else{ goto _LL60;}} else{ goto _LL60;} _LL60: if(( unsigned int) _temp56
>  4u?*(( int*) _temp56) ==  Cyc_Absyn_TupleType: 0){ _LL75: _temp74=(( struct
Cyc_Absyn_TupleType_struct*) _temp56)->f1; goto _LL61;} else{ goto _LL62;} _LL62:
if(( unsigned int) _temp56 >  4u?*(( int*) _temp56) ==  Cyc_Absyn_AnonStructType:
0){ _LL77: _temp76=(( struct Cyc_Absyn_AnonStructType_struct*) _temp56)->f1;
goto _LL63;} else{ goto _LL64;} _LL64: if(( unsigned int) _temp56 >  4u?*(( int*)
_temp56) ==  Cyc_Absyn_StructType: 0){ _LL79: _temp78=(( struct Cyc_Absyn_StructType_struct*)
_temp56)->f3; if( _temp78 ==  0){ goto _LL66;} else{ _temp80=* _temp78; _temp81=*
_temp80; _LL83: _temp82= _temp81.fields; if( _temp82 ==  0){ goto _LL66;} else{
_temp84=* _temp82; _LL86: _temp85=( struct Cyc_List_List*) _temp84.v; goto _LL65;}}}
else{ goto _LL66;} _LL66: goto _LL67; _LL59: if( _temp72->typs ==  0){ return
leafval;} _temp74= _temp72->typs; goto _LL61; _LL61: { struct Cyc_Dict_Dict* d=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int i= 0; for( 0; _temp74 !=  0;( _temp74=
_temp74->tl, ++ i)){ d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( d, Cyc_Absyn_fieldname( i),
Cyc_CfFlowInfo_typ_to_absrval((*(( struct _tuple3*) _temp74->hd)).f2, leafval));}}
return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct* _temp87=( struct Cyc_CfFlowInfo_Aggregate_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Aggregate_struct)); _temp87[ 0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _temp88; _temp88.tag= Cyc_CfFlowInfo_Aggregate;
_temp88.f1= d; _temp88;}); _temp87;});} _LL63: _temp85= _temp76; goto _LL65;
_LL65: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); for(
0; _temp85 !=  0; _temp85= _temp85->tl){ struct Cyc_Absyn_Structfield _temp91;
void* _temp92; struct _tagged_arr* _temp94; struct Cyc_Absyn_Structfield*
_temp89=( struct Cyc_Absyn_Structfield*) _temp85->hd; _temp91=* _temp89; _LL95:
_temp94= _temp91.name; goto _LL93; _LL93: _temp92=( void*) _temp91.type; goto
_LL90; _LL90: if( _get_arr_size(* _temp94, sizeof( unsigned char)) !=  1){ d=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void*
v)) Cyc_Dict_insert)( d, _temp94, Cyc_CfFlowInfo_typ_to_absrval( _temp92,
leafval));}} return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct* _temp96=(
struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Aggregate_struct));
_temp96[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct _temp97; _temp97.tag= Cyc_CfFlowInfo_Aggregate;
_temp97.f1= d; _temp97;}); _temp96;});} _LL67: return Cyc_Tcutil_bits_only( t)?
Cyc_CfFlowInfo_unknown_all: leafval; _LL57:;} static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place* place, struct Cyc_Set_Set* set){{ struct Cyc_List_List*
_temp98=(( struct Cyc_List_List*(*)( struct Cyc_Set_Set* s)) Cyc_Set_elements)(
set); for( 0; _temp98 !=  0; _temp98= _temp98->tl){ struct Cyc_CfFlowInfo_Place*
_temp99=( struct Cyc_CfFlowInfo_Place*) _temp98->hd; if( Cyc_CfFlowInfo_root_cmp((
void*) place->root,( void*) _temp99->root) !=  0){ continue;}{ struct Cyc_List_List*
_temp100= place->fields; struct Cyc_List_List* _temp101= _temp99->fields; for( 0;
_temp100 !=  0? _temp101 !=  0: 0;( _temp100= _temp100->tl, _temp101= _temp101->tl)){
if( Cyc_Std_zstrptrcmp(( struct _tagged_arr*) _temp100->hd,( struct _tagged_arr*)
_temp101->hd) !=  0){ break;}} if( _temp100 ==  0){ return 1;}}}} return 0;}
struct Cyc_CfFlowInfo_EscPile{ struct _RegionHandle* rgn; struct Cyc_List_List*
places; } ; static void Cyc_CfFlowInfo_add_place( struct Cyc_CfFlowInfo_EscPile*
pile, struct Cyc_CfFlowInfo_Place* place){ if( !(( int(*)( int(* compare)(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*), struct Cyc_List_List*
l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,
pile->places, place)){ pile->places=({ struct Cyc_List_List* _temp102=( struct
Cyc_List_List*) _region_malloc( pile->rgn, sizeof( struct Cyc_List_List));
_temp102->hd=( void*) place; _temp102->tl= pile->places; _temp102;});}} static
void Cyc_CfFlowInfo_add_places( struct Cyc_CfFlowInfo_EscPile* pile, void* a,
void* r){ void* _temp103= r; struct Cyc_CfFlowInfo_Place* _temp111; struct Cyc_Dict_Dict*
_temp113; _LL105: if(( unsigned int) _temp103 >  3u?*(( int*) _temp103) ==  Cyc_CfFlowInfo_AddressOf:
0){ _LL112: _temp111=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp103)->f1;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp103 >  3u?*((
int*) _temp103) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL114: _temp113=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp103)->f1; goto _LL108;} else{ goto _LL109;} _LL109: goto _LL110; _LL106:
Cyc_CfFlowInfo_add_place( pile, _temp111); return; _LL108:(( void(*)( void(* f)(
struct Cyc_CfFlowInfo_EscPile*, struct _tagged_arr*, void*), struct Cyc_CfFlowInfo_EscPile*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_CfFlowInfo_EscPile*
pile, struct _tagged_arr* a, void* r)) Cyc_CfFlowInfo_add_places, pile, _temp113);
return; _LL110: return; _LL104:;} static void* Cyc_CfFlowInfo_insert_place_inner(
void* new_val, void* old_val){ void* _temp115= old_val; struct Cyc_Dict_Dict*
_temp121; _LL117: if(( unsigned int) _temp115 >  3u?*(( int*) _temp115) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL122: _temp121=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp115)->f1;
goto _LL118;} else{ goto _LL119;} _LL119: goto _LL120; _LL118: return( void*)({
struct Cyc_CfFlowInfo_Aggregate_struct* _temp123=( struct Cyc_CfFlowInfo_Aggregate_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Aggregate_struct)); _temp123[ 0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _temp124; _temp124.tag= Cyc_CfFlowInfo_Aggregate;
_temp124.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_insert_place_inner,
new_val, _temp121); _temp124;}); _temp123;}); _LL120: return new_val; _LL116:;}
struct _tuple4{ struct Cyc_List_List* f1; void* f2; } ; static void* Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List* fs, void* old_val, void* new_val){ if( fs ==  0){ return
Cyc_CfFlowInfo_insert_place_inner( new_val, old_val);}{ struct _tuple4 _temp126=({
struct _tuple4 _temp125; _temp125.f1= fs; _temp125.f2= old_val; _temp125;});
void* _temp132; struct Cyc_Dict_Dict* _temp134; struct Cyc_List_List* _temp136;
struct Cyc_List_List _temp138; struct Cyc_List_List* _temp139; struct
_tagged_arr* _temp141; _LL128: _LL137: _temp136= _temp126.f1; if( _temp136 ==  0){
goto _LL130;} else{ _temp138=* _temp136; _LL142: _temp141=( struct _tagged_arr*)
_temp138.hd; goto _LL140; _LL140: _temp139= _temp138.tl; goto _LL133;} _LL133:
_temp132= _temp126.f2; if(( unsigned int) _temp132 >  3u?*(( int*) _temp132) == 
Cyc_CfFlowInfo_Aggregate: 0){ _LL135: _temp134=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp132)->f1; goto _LL129;} else{ goto _LL130;} _LL130: goto _LL131; _LL129: {
void* _temp143= Cyc_CfFlowInfo_insert_place_outer( _temp139,(( void*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp134, _temp141),
new_val); return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct* _temp144=(
struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Aggregate_struct));
_temp144[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct _temp145; _temp145.tag=
Cyc_CfFlowInfo_Aggregate; _temp145.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( _temp134, _temp141,
_temp143); _temp145;}); _temp144;});} _LL131:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp146=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp146[ 0]=({ struct Cyc_Core_Impossible_struct _temp147; _temp147.tag= Cyc_Core_Impossible;
_temp147.f1= _tag_arr("bad insert place", sizeof( unsigned char), 17u); _temp147;});
_temp146;})); _LL127:;}} static struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_EscPile* pile, struct Cyc_Set_Set** all_changed, struct
Cyc_Dict_Dict* d){ while( pile->places !=  0) { struct Cyc_CfFlowInfo_Place*
_temp148=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_List_List*) _check_null(
pile->places))->hd; pile->places=(( struct Cyc_List_List*) _check_null( pile->places))->tl;
if( all_changed !=  0){* all_changed=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* all_changed, _temp148);}{
void* oldval; void* newval;{ struct _handler_cons _temp149; _push_handler(&
_temp149);{ int _temp151= 0; if( setjmp( _temp149.handler)){ _temp151= 1;} if( !
_temp151){ oldval= Cyc_CfFlowInfo_lookup_place( d, _temp148);; _pop_handler();}
else{ void* _temp150=( void*) _exn_thrown; void* _temp153= _temp150; _LL155: if(
_temp153 ==  Cyc_Dict_Absent){ goto _LL156;} else{ goto _LL157;} _LL157: goto
_LL158; _LL156: continue; _LL158:( void) _throw( _temp153); _LL154:;}}}{ void*
_temp159= Cyc_CfFlowInfo_initlevel( d, oldval); _LL161: if( _temp159 == ( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL162;} else{ goto _LL163;} _LL163: if( _temp159 == (
void*) Cyc_CfFlowInfo_ThisIL){ goto _LL164;} else{ goto _LL165;} _LL165: if(
_temp159 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL166;} else{ goto _LL160;}
_LL162: newval= Cyc_CfFlowInfo_esc_all; goto _LL160; _LL164: newval= Cyc_CfFlowInfo_esc_this;
goto _LL160; _LL166: newval= Cyc_CfFlowInfo_esc_none; goto _LL160; _LL160:;}((
void(*)( struct Cyc_CfFlowInfo_EscPile* pile, int a, void* r)) Cyc_CfFlowInfo_add_places)(
pile, 0, oldval); d= Cyc_Dict_insert( d,( void*) _temp148->root, Cyc_CfFlowInfo_insert_place_outer(
_temp148->fields, Cyc_Dict_lookup( d,( void*) _temp148->root), newval));}}
return d;} struct Cyc_CfFlowInfo_InitlevelEnv{ struct Cyc_Dict_Dict* d; struct
Cyc_List_List* seen; } ; static void* Cyc_CfFlowInfo_initlevel_approx( void* r){
void* _temp167= r; void* _temp181; void* _temp183; _LL169: if(( unsigned int)
_temp167 >  3u?*(( int*) _temp167) ==  Cyc_CfFlowInfo_UnknownR: 0){ _LL182:
_temp181=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp167)->f1; goto
_LL170;} else{ goto _LL171;} _LL171: if(( unsigned int) _temp167 >  3u?*(( int*)
_temp167) ==  Cyc_CfFlowInfo_Esc: 0){ _LL184: _temp183=( void*)(( struct Cyc_CfFlowInfo_Esc_struct*)
_temp167)->f1; goto _LL172;} else{ goto _LL173;} _LL173: if( _temp167 == ( void*)
Cyc_CfFlowInfo_Zero){ goto _LL174;} else{ goto _LL175;} _LL175: if( _temp167 == (
void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL176;} else{ goto _LL177;} _LL177: if(
_temp167 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto _LL178;} else{ goto
_LL179;} _LL179: goto _LL180; _LL170: return _temp181; _LL172: return _temp183;
_LL174: goto _LL176; _LL176: return( void*) Cyc_CfFlowInfo_AllIL; _LL178: return(
void*) Cyc_CfFlowInfo_ThisIL; _LL180:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp185=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp185[ 0]=({ struct Cyc_Core_Impossible_struct _temp186; _temp186.tag= Cyc_Core_Impossible;
_temp186.f1= _tag_arr("initlevel_approx", sizeof( unsigned char), 17u); _temp186;});
_temp185;})); _LL168:;} static void* Cyc_CfFlowInfo_initlevel_rec( struct Cyc_CfFlowInfo_InitlevelEnv*
env, void* a, void* r, void* acc){ void* this_ans; if( acc == ( void*) Cyc_CfFlowInfo_NoneIL){
return( void*) Cyc_CfFlowInfo_NoneIL;}{ void* _temp187= r; struct Cyc_Dict_Dict*
_temp195; struct Cyc_CfFlowInfo_Place* _temp197; _LL189: if(( unsigned int)
_temp187 >  3u?*(( int*) _temp187) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL196:
_temp195=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp187)->f1; goto _LL190;}
else{ goto _LL191;} _LL191: if(( unsigned int) _temp187 >  3u?*(( int*) _temp187)
==  Cyc_CfFlowInfo_AddressOf: 0){ _LL198: _temp197=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp187)->f1; goto _LL192;} else{ goto _LL193;} _LL193: goto _LL194; _LL190:
this_ans=(( void*(*)( void*(* f)( struct Cyc_CfFlowInfo_InitlevelEnv*, struct
_tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_InitlevelEnv* env, struct Cyc_Dict_Dict*
d, void* accum)) Cyc_Dict_fold_c)(( void*(*)( struct Cyc_CfFlowInfo_InitlevelEnv*
env, struct _tagged_arr* a, void* r, void* acc)) Cyc_CfFlowInfo_initlevel_rec,
env, _temp195,( void*) Cyc_CfFlowInfo_AllIL); goto _LL188; _LL192: if((( int(*)(
int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,
env->seen, _temp197)){ this_ans=( void*) Cyc_CfFlowInfo_AllIL;} else{ env->seen=({
struct Cyc_List_List* _temp199=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp199->hd=( void*) _temp197; _temp199->tl= env->seen;
_temp199;}); this_ans=(( void*(*)( struct Cyc_CfFlowInfo_InitlevelEnv* env, int
a, void* r, void* acc)) Cyc_CfFlowInfo_initlevel_rec)( env, 0, Cyc_CfFlowInfo_lookup_place(
env->d, _temp197),( void*) Cyc_CfFlowInfo_AllIL); env->seen=(( struct Cyc_List_List*)
_check_null( env->seen))->tl; if( this_ans == ( void*) Cyc_CfFlowInfo_NoneIL){
this_ans=( void*) Cyc_CfFlowInfo_ThisIL;}} goto _LL188; _LL194: this_ans= Cyc_CfFlowInfo_initlevel_approx(
r); _LL188:;} if( this_ans == ( void*) Cyc_CfFlowInfo_NoneIL){ return( void*)
Cyc_CfFlowInfo_NoneIL;} if( this_ans == ( void*) Cyc_CfFlowInfo_ThisIL? 1: acc
== ( void*) Cyc_CfFlowInfo_ThisIL){ return( void*) Cyc_CfFlowInfo_ThisIL;}
return( void*) Cyc_CfFlowInfo_AllIL;} void* Cyc_CfFlowInfo_initlevel( struct Cyc_Dict_Dict*
d, void* r){ struct Cyc_CfFlowInfo_InitlevelEnv _temp200=({ struct Cyc_CfFlowInfo_InitlevelEnv
_temp201; _temp201.d= d; _temp201.seen= 0; _temp201;}); return(( void*(*)(
struct Cyc_CfFlowInfo_InitlevelEnv* env, int a, void* r, void* acc)) Cyc_CfFlowInfo_initlevel_rec)(&
_temp200, 0, r,( void*) Cyc_CfFlowInfo_AllIL);} void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place){ struct Cyc_CfFlowInfo_Place
_temp204; struct Cyc_List_List* _temp205; void* _temp207; struct Cyc_CfFlowInfo_Place*
_temp202= place; _temp204=* _temp202; _LL208: _temp207=( void*) _temp204.root;
goto _LL206; _LL206: _temp205= _temp204.fields; goto _LL203; _LL203: { void*
_temp209= Cyc_Dict_lookup( d, _temp207); for( 0; _temp205 !=  0; _temp205=
_temp205->tl){ struct _tuple0 _temp211=({ struct _tuple0 _temp210; _temp210.f1=
_temp209; _temp210.f2=( struct _tagged_arr*) _temp205->hd; _temp210;}); struct
_tagged_arr* _temp217; void* _temp219; struct Cyc_Dict_Dict* _temp221; _LL213:
_LL220: _temp219= _temp211.f1; if(( unsigned int) _temp219 >  3u?*(( int*)
_temp219) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL222: _temp221=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp219)->f1; goto _LL218;} else{ goto _LL215;} _LL218: _temp217= _temp211.f2;
goto _LL214; _LL215: goto _LL216; _LL214: _temp209=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp221, _temp217); goto _LL212;
_LL216:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp223=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp223[ 0]=({ struct Cyc_Core_Impossible_struct _temp224; _temp224.tag= Cyc_Core_Impossible;
_temp224.f1= _tag_arr("bad lookup_place", sizeof( unsigned char), 17u); _temp224;});
_temp223;})); _LL212:;} return _temp209;}} static int Cyc_CfFlowInfo_is_rval_unescaped(
void* a, void* b, void* rval){ void* _temp225= rval; struct Cyc_Dict_Dict*
_temp233; _LL227: if(( unsigned int) _temp225 >  3u?*(( int*) _temp225) ==  Cyc_CfFlowInfo_Esc:
0){ goto _LL228;} else{ goto _LL229;} _LL229: if(( unsigned int) _temp225 >  3u?*((
int*) _temp225) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL234: _temp233=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp225)->f1; goto _LL230;} else{ goto _LL231;} _LL231: goto _LL232; _LL228:
return 0; _LL230: return(( int(*)( int(* f)( int, struct _tagged_arr*, void*),
int env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)( int a, struct
_tagged_arr* b, void* rval)) Cyc_CfFlowInfo_is_rval_unescaped, 0, _temp233);
_LL232: return 1; _LL226:;} int Cyc_CfFlowInfo_is_unescaped( struct Cyc_Dict_Dict*
d, struct Cyc_CfFlowInfo_Place* place){ return(( int(*)( int a, int b, void*
rval)) Cyc_CfFlowInfo_is_rval_unescaped)( 0, 0, Cyc_CfFlowInfo_lookup_place( d,
place));} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_deref( struct Cyc_Dict_Dict*
d, struct Cyc_Set_Set** all_changed, void* r){ struct _RegionHandle _temp235=
_new_region(); struct _RegionHandle* rgn=& _temp235; _push_region( rgn);{ struct
Cyc_CfFlowInfo_EscPile* pile=({ struct Cyc_CfFlowInfo_EscPile* _temp237=( struct
Cyc_CfFlowInfo_EscPile*) _region_malloc( rgn, sizeof( struct Cyc_CfFlowInfo_EscPile));
_temp237->rgn= rgn; _temp237->places= 0; _temp237;});(( void(*)( struct Cyc_CfFlowInfo_EscPile*
pile, int a, void* r)) Cyc_CfFlowInfo_add_places)( pile, 0, r);{ struct Cyc_Dict_Dict*
_temp236= Cyc_CfFlowInfo_escape_these( pile, all_changed, d); _npop_handler( 0u);
return _temp236;}}; _pop_region( rgn);} struct Cyc_CfFlowInfo_AssignEnv{ struct
Cyc_CfFlowInfo_EscPile* pile; struct Cyc_Dict_Dict* d; struct Cyc_Position_Segment*
loc; } ; struct _tuple5{ void* f1; void* f2; } ; static void* Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv* env, void* a, void* oldval, void* newval){
struct _tuple5 _temp239=({ struct _tuple5 _temp238; _temp238.f1= oldval;
_temp238.f2= newval; _temp238;}); void* _temp251; struct Cyc_CfFlowInfo_Place*
_temp253; void* _temp255; void* _temp257; void* _temp259; struct Cyc_Dict_Dict*
_temp261; void* _temp263; struct Cyc_Dict_Dict* _temp265; void* _temp267; void*
_temp269; _LL241: _LL256: _temp255= _temp239.f1; if(( unsigned int) _temp255 > 
3u?*(( int*) _temp255) ==  Cyc_CfFlowInfo_Esc: 0){ goto _LL252;} else{ goto
_LL243;} _LL252: _temp251= _temp239.f2; if(( unsigned int) _temp251 >  3u?*((
int*) _temp251) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL254: _temp253=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp251)->f1; goto _LL242;} else{ goto _LL243;} _LL243: _LL258: _temp257=
_temp239.f1; if(( unsigned int) _temp257 >  3u?*(( int*) _temp257) ==  Cyc_CfFlowInfo_Esc:
0){ goto _LL244;} else{ goto _LL245;} _LL245: _LL264: _temp263= _temp239.f1; if((
unsigned int) _temp263 >  3u?*(( int*) _temp263) ==  Cyc_CfFlowInfo_Aggregate: 0){
_LL266: _temp265=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp263)->f1; goto
_LL260;} else{ goto _LL247;} _LL260: _temp259= _temp239.f2; if(( unsigned int)
_temp259 >  3u?*(( int*) _temp259) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL262:
_temp261=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp259)->f1; goto _LL246;}
else{ goto _LL247;} _LL247: _LL268: _temp267= _temp239.f2; if(( unsigned int)
_temp267 >  3u?*(( int*) _temp267) ==  Cyc_CfFlowInfo_Esc: 0){ _LL270: _temp269=(
void*)(( struct Cyc_CfFlowInfo_Esc_struct*) _temp267)->f1; goto _LL248;} else{
goto _LL249;} _LL249: goto _LL250; _LL242: Cyc_CfFlowInfo_add_place( env->pile,
_temp253); goto _LL244; _LL244: if( Cyc_CfFlowInfo_initlevel( env->d, newval) != (
void*) Cyc_CfFlowInfo_AllIL){({ void* _temp271[ 0u]={}; Cyc_Tcutil_terr( env->loc,
_tag_arr("assignment puts possibly-uninitialized data in an escaped location",
sizeof( unsigned char), 67u), _tag_arr( _temp271, sizeof( void*), 0u));});}
return Cyc_CfFlowInfo_esc_all; _LL246: { struct Cyc_Dict_Dict* _temp272=((
struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_AssignEnv*, struct
_tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_AssignEnv* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_union_two_c)(( void*(*)( struct Cyc_CfFlowInfo_AssignEnv*
env, struct _tagged_arr* a, void* oldval, void* newval)) Cyc_CfFlowInfo_assign_place_inner,
env, _temp265, _temp261); if( _temp272 ==  _temp265){ return oldval;} if(
_temp272 ==  _temp261){ return newval;} return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp273=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp273[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp274; _temp274.tag= Cyc_CfFlowInfo_Aggregate; _temp274.f1= _temp272;
_temp274;}); _temp273;});} _LL248: { void* _temp275= _temp269; _LL277: if(
_temp275 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL278;} else{ goto _LL279;}
_LL279: if( _temp275 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL280;} else{
goto _LL281;} _LL281: if( _temp275 == ( void*) Cyc_CfFlowInfo_AllIL){ goto
_LL282;} else{ goto _LL276;} _LL278: return Cyc_CfFlowInfo_unknown_none; _LL280:
return Cyc_CfFlowInfo_unknown_this; _LL282: return Cyc_CfFlowInfo_unknown_all;
_LL276:;} _LL250: return newval; _LL240:;} static void* Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv* env, struct Cyc_List_List* fs, void* oldval,
void* newval){ if( fs ==  0){ return(( void*(*)( struct Cyc_CfFlowInfo_AssignEnv*
env, int a, void* oldval, void* newval)) Cyc_CfFlowInfo_assign_place_inner)( env,
0, oldval, newval);}{ struct _tuple4 _temp284=({ struct _tuple4 _temp283;
_temp283.f1= fs; _temp283.f2= oldval; _temp283;}); void* _temp290; struct Cyc_Dict_Dict*
_temp292; struct Cyc_List_List* _temp294; struct Cyc_List_List _temp296; struct
Cyc_List_List* _temp297; struct _tagged_arr* _temp299; _LL286: _LL295: _temp294=
_temp284.f1; if( _temp294 ==  0){ goto _LL288;} else{ _temp296=* _temp294;
_LL300: _temp299=( struct _tagged_arr*) _temp296.hd; goto _LL298; _LL298:
_temp297= _temp296.tl; goto _LL291;} _LL291: _temp290= _temp284.f2; if((
unsigned int) _temp290 >  3u?*(( int*) _temp290) ==  Cyc_CfFlowInfo_Aggregate: 0){
_LL293: _temp292=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp290)->f1; goto
_LL287;} else{ goto _LL288;} _LL288: goto _LL289; _LL287: { void* _temp301= Cyc_CfFlowInfo_assign_place_outer(
env, _temp297,(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
_temp292, _temp299), newval); return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp302=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp302[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp303; _temp303.tag= Cyc_CfFlowInfo_Aggregate; _temp303.f1=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)(
_temp292, _temp299, _temp301); _temp303;}); _temp302;});} _LL289:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp304=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp304[ 0]=({ struct
Cyc_Core_Impossible_struct _temp305; _temp305.tag= Cyc_Core_Impossible; _temp305.f1=
_tag_arr("bad insert place", sizeof( unsigned char), 17u); _temp305;}); _temp304;}));
_LL285:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_assign_place( struct Cyc_Position_Segment*
loc, struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed, struct Cyc_CfFlowInfo_Place*
place, void* r){ if( all_changed !=  0){* all_changed=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*
all_changed, place);}{ struct _RegionHandle _temp306= _new_region(); struct
_RegionHandle* rgn=& _temp306; _push_region( rgn);{ struct Cyc_CfFlowInfo_Place
_temp309; struct Cyc_List_List* _temp310; void* _temp312; struct Cyc_CfFlowInfo_Place*
_temp307= place; _temp309=* _temp307; _LL313: _temp312=( void*) _temp309.root;
goto _LL311; _LL311: _temp310= _temp309.fields; goto _LL308; _LL308: { struct
Cyc_CfFlowInfo_AssignEnv env=({ struct Cyc_CfFlowInfo_AssignEnv _temp315;
_temp315.pile=({ struct Cyc_CfFlowInfo_EscPile* _temp316=( struct Cyc_CfFlowInfo_EscPile*)
_region_malloc( rgn, sizeof( struct Cyc_CfFlowInfo_EscPile)); _temp316->rgn= rgn;
_temp316->places= 0; _temp316;}); _temp315.d= d; _temp315.loc= loc; _temp315;});
void* newval= Cyc_CfFlowInfo_assign_place_outer(& env, _temp310, Cyc_Dict_lookup(
d, _temp312), r); struct Cyc_Dict_Dict* _temp314= Cyc_CfFlowInfo_escape_these(
env.pile, all_changed, Cyc_Dict_insert( d, _temp312, newval)); _npop_handler( 0u);
return _temp314;}}; _pop_region( rgn);}} struct Cyc_CfFlowInfo_JoinEnv{ struct
Cyc_CfFlowInfo_EscPile* pile; struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2;
} ; enum  Cyc_CfFlowInfo_WhoIsChanged{ Cyc_CfFlowInfo_Neither  =  0u, Cyc_CfFlowInfo_One
 =  1u, Cyc_CfFlowInfo_Two  =  2u}; struct Cyc_CfFlowInfo_AfterEnv{ struct Cyc_CfFlowInfo_JoinEnv
joinenv; struct Cyc_Set_Set* chg1; struct Cyc_Set_Set* chg2; struct Cyc_CfFlowInfo_Place*
curr_place; struct Cyc_List_List** last_field_cell; enum  Cyc_CfFlowInfo_WhoIsChanged
changed; } ; static void* Cyc_CfFlowInfo_join_absRval( struct Cyc_CfFlowInfo_JoinEnv*
env, void* a, void* r1, void* r2){ if( r1 ==  r2){ return r1;}{ struct _tuple5
_temp318=({ struct _tuple5 _temp317; _temp317.f1= r1; _temp317.f2= r2; _temp317;});
void* _temp334; struct Cyc_CfFlowInfo_Place* _temp336; void* _temp338; struct
Cyc_CfFlowInfo_Place* _temp340; void* _temp342; struct Cyc_CfFlowInfo_Place*
_temp344; void* _temp346; struct Cyc_CfFlowInfo_Place* _temp348; void* _temp350;
void* _temp352; void* _temp354; void* _temp356; void* _temp358; struct Cyc_Dict_Dict*
_temp360; void* _temp362; struct Cyc_Dict_Dict* _temp364; _LL320: _LL339:
_temp338= _temp318.f1; if(( unsigned int) _temp338 >  3u?*(( int*) _temp338) == 
Cyc_CfFlowInfo_AddressOf: 0){ _LL341: _temp340=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp338)->f1; goto _LL335;} else{ goto _LL322;} _LL335: _temp334= _temp318.f2;
if(( unsigned int) _temp334 >  3u?*(( int*) _temp334) ==  Cyc_CfFlowInfo_AddressOf:
0){ _LL337: _temp336=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp334)->f1;
goto _LL321;} else{ goto _LL322;} _LL322: _LL343: _temp342= _temp318.f1; if((
unsigned int) _temp342 >  3u?*(( int*) _temp342) ==  Cyc_CfFlowInfo_AddressOf: 0){
_LL345: _temp344=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp342)->f1; goto
_LL323;} else{ goto _LL324;} _LL324: _LL347: _temp346= _temp318.f2; if((
unsigned int) _temp346 >  3u?*(( int*) _temp346) ==  Cyc_CfFlowInfo_AddressOf: 0){
_LL349: _temp348=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp346)->f1; goto
_LL325;} else{ goto _LL326;} _LL326: _LL353: _temp352= _temp318.f1; if( _temp352
== ( void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL351;} else{ goto _LL328;} _LL351:
_temp350= _temp318.f2; if( _temp350 == ( void*) Cyc_CfFlowInfo_NotZeroThis){
goto _LL327;} else{ goto _LL328;} _LL328: _LL357: _temp356= _temp318.f1; if(
_temp356 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto _LL355;} else{ goto
_LL330;} _LL355: _temp354= _temp318.f2; if( _temp354 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL329;} else{ goto _LL330;} _LL330: _LL363: _temp362= _temp318.f1; if((
unsigned int) _temp362 >  3u?*(( int*) _temp362) ==  Cyc_CfFlowInfo_Aggregate: 0){
_LL365: _temp364=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp362)->f1; goto
_LL359;} else{ goto _LL332;} _LL359: _temp358= _temp318.f2; if(( unsigned int)
_temp358 >  3u?*(( int*) _temp358) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL361:
_temp360=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp358)->f1; goto _LL331;}
else{ goto _LL332;} _LL332: goto _LL333; _LL321: if( Cyc_CfFlowInfo_place_cmp(
_temp340, _temp336) ==  0){ return r1;} Cyc_CfFlowInfo_add_place( env->pile,
_temp340); Cyc_CfFlowInfo_add_place( env->pile, _temp336); goto _LL319; _LL323:
Cyc_CfFlowInfo_add_place( env->pile, _temp344); goto _LL319; _LL325: Cyc_CfFlowInfo_add_place(
env->pile, _temp348); goto _LL319; _LL327: goto _LL329; _LL329: return( void*)
Cyc_CfFlowInfo_NotZeroThis; _LL331: { struct Cyc_Dict_Dict* _temp366=(( struct
Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*, struct
_tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_union_two_c)(( void*(*)( struct Cyc_CfFlowInfo_JoinEnv*
env, struct _tagged_arr* a, void* r1, void* r2)) Cyc_CfFlowInfo_join_absRval,
env, _temp364, _temp360); if( _temp366 ==  _temp364){ return r1;} if( _temp366
==  _temp360){ return r2;} return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp367=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp367[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp368; _temp368.tag= Cyc_CfFlowInfo_Aggregate; _temp368.f1= _temp366;
_temp368;}); _temp367;});} _LL333: goto _LL319; _LL319:;}{ void* il1= Cyc_CfFlowInfo_initlevel(
env->d1, r1); void* il2= Cyc_CfFlowInfo_initlevel( env->d2, r2); struct _tuple5
_temp370=({ struct _tuple5 _temp369; _temp369.f1= r1; _temp369.f2= r2; _temp369;});
void* _temp378; void* _temp380; _LL372: _LL379: _temp378= _temp370.f1; if((
unsigned int) _temp378 >  3u?*(( int*) _temp378) ==  Cyc_CfFlowInfo_Esc: 0){
goto _LL373;} else{ goto _LL374;} _LL374: _LL381: _temp380= _temp370.f2; if((
unsigned int) _temp380 >  3u?*(( int*) _temp380) ==  Cyc_CfFlowInfo_Esc: 0){
goto _LL375;} else{ goto _LL376;} _LL376: goto _LL377; _LL373: goto _LL375;
_LL375: { struct _tuple5 _temp383=({ struct _tuple5 _temp382; _temp382.f1= il1;
_temp382.f2= il2; _temp382;}); void* _temp395; void* _temp397; void* _temp399;
void* _temp401; _LL385: _LL396: _temp395= _temp383.f2; if( _temp395 == ( void*)
Cyc_CfFlowInfo_NoneIL){ goto _LL386;} else{ goto _LL387;} _LL387: _LL398:
_temp397= _temp383.f1; if( _temp397 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL388;} else{ goto _LL389;} _LL389: _LL400: _temp399= _temp383.f2; if( _temp399
== ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL390;} else{ goto _LL391;} _LL391:
_LL402: _temp401= _temp383.f1; if( _temp401 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL392;} else{ goto _LL393;} _LL393: goto _LL394; _LL386: goto _LL388;
_LL388: return Cyc_CfFlowInfo_esc_none; _LL390: goto _LL392; _LL392: return Cyc_CfFlowInfo_esc_this;
_LL394: return Cyc_CfFlowInfo_esc_all; _LL384:;} _LL377: { struct _tuple5
_temp404=({ struct _tuple5 _temp403; _temp403.f1= il1; _temp403.f2= il2;
_temp403;}); void* _temp416; void* _temp418; void* _temp420; void* _temp422;
_LL406: _LL417: _temp416= _temp404.f2; if( _temp416 == ( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL407;} else{ goto _LL408;} _LL408: _LL419: _temp418= _temp404.f1; if(
_temp418 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL409;} else{ goto _LL410;}
_LL410: _LL421: _temp420= _temp404.f2; if( _temp420 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL411;} else{ goto _LL412;} _LL412: _LL423: _temp422= _temp404.f1; if(
_temp422 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL413;} else{ goto _LL414;}
_LL414: goto _LL415; _LL407: goto _LL409; _LL409: return Cyc_CfFlowInfo_unknown_none;
_LL411: goto _LL413; _LL413: return Cyc_CfFlowInfo_unknown_this; _LL415: return
Cyc_CfFlowInfo_unknown_all; _LL405:;} _LL371:;}} void* Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set** all_changed, void* f1, void* f2){ if( f1 ==  f2){ return f1;}{
struct _tuple5 _temp425=({ struct _tuple5 _temp424; _temp424.f1= f1; _temp424.f2=
f2; _temp424;}); void* _temp433; void* _temp435; void* _temp437; struct Cyc_Dict_Dict*
_temp439; void* _temp441; struct Cyc_Dict_Dict* _temp443; _LL427: _LL434:
_temp433= _temp425.f1; if( _temp433 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL428;} else{ goto _LL429;} _LL429: _LL436: _temp435= _temp425.f2; if( _temp435
== ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL430;} else{ goto _LL431;} _LL431:
_LL442: _temp441= _temp425.f1; if(( unsigned int) _temp441 >  1u?*(( int*)
_temp441) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL444: _temp443=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp441)->f1; goto _LL438;} else{ goto _LL426;} _LL438: _temp437= _temp425.f2;
if(( unsigned int) _temp437 >  1u?*(( int*) _temp437) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL440: _temp439=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp437)->f1;
goto _LL432;} else{ goto _LL426;} _LL428: return f2; _LL430: return f1; _LL432:
if( _temp443 ==  _temp439){ return f1;} if( Cyc_CfFlowInfo_flow_lessthan_approx(
f1, f2)){ return f2;} if( Cyc_CfFlowInfo_flow_lessthan_approx( f2, f1)){ return
f1;}{ struct _RegionHandle _temp445= _new_region(); struct _RegionHandle* rgn=&
_temp445; _push_region( rgn);{ struct Cyc_CfFlowInfo_JoinEnv _temp446=({ struct
Cyc_CfFlowInfo_JoinEnv _temp451; _temp451.pile=({ struct Cyc_CfFlowInfo_EscPile*
_temp452=( struct Cyc_CfFlowInfo_EscPile*) _region_malloc( rgn, sizeof( struct
Cyc_CfFlowInfo_EscPile)); _temp452->rgn= rgn; _temp452->places= 0; _temp452;});
_temp451.d1= _temp443; _temp451.d2= _temp439; _temp451;}); struct Cyc_Dict_Dict*
_temp447=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_absRval,&
_temp446, _temp443, _temp439); void* _temp450=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp448=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp448[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp449; _temp449.tag= Cyc_CfFlowInfo_ReachableFL; _temp449.f1= Cyc_CfFlowInfo_escape_these(
_temp446.pile, all_changed, _temp447); _temp449;}); _temp448;}); _npop_handler(
0u); return _temp450;}; _pop_region( rgn);} _LL426:;}} static void* Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*, struct _tagged_arr*, void*, void*); static void*
Cyc_CfFlowInfo_after_absRval( struct Cyc_CfFlowInfo_AfterEnv* env, void* r1,
void* r2){ int changed1= env->changed ==  Cyc_CfFlowInfo_One? 1:(( int(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_member)( env->chg1,
env->curr_place); int changed2= env->changed ==  Cyc_CfFlowInfo_Two? 1:(( int(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_member)( env->chg2,
env->curr_place); if( changed1? changed2: 0){ return(( void*(*)( struct Cyc_CfFlowInfo_JoinEnv*
env, int a, void* r1, void* r2)) Cyc_CfFlowInfo_join_absRval)(& env->joinenv, 0,
r1, r2);} if( changed1){ if( ! Cyc_CfFlowInfo_prefix_of_member( env->curr_place,
env->chg2)){ return r1;} env->changed= Cyc_CfFlowInfo_One;} if( changed2){ if( !
Cyc_CfFlowInfo_prefix_of_member( env->curr_place, env->chg1)){ return r2;} env->changed=
Cyc_CfFlowInfo_Two;}{ struct _tuple5 _temp454=({ struct _tuple5 _temp453;
_temp453.f1= r1; _temp453.f2= r2; _temp453;}); void* _temp460; struct Cyc_Dict_Dict*
_temp462; void* _temp464; struct Cyc_Dict_Dict* _temp466; _LL456: _LL465:
_temp464= _temp454.f1; if(( unsigned int) _temp464 >  3u?*(( int*) _temp464) == 
Cyc_CfFlowInfo_Aggregate: 0){ _LL467: _temp466=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp464)->f1; goto _LL461;} else{ goto _LL458;} _LL461: _temp460= _temp454.f2;
if(( unsigned int) _temp460 >  3u?*(( int*) _temp460) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL463: _temp462=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp460)->f1;
goto _LL457;} else{ goto _LL458;} _LL458: goto _LL459; _LL457: { struct Cyc_Dict_Dict*
_temp468=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_AfterEnv* env, struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_union_two_c)( Cyc_CfFlowInfo_after_absRval_d,
env, _temp466, _temp462); if( _temp468 ==  _temp466){ return r1;} if( _temp468
==  _temp462){ return r2;} return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp469=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp469[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp470; _temp470.tag= Cyc_CfFlowInfo_Aggregate; _temp470.f1= _temp468;
_temp470;}); _temp469;});} _LL459:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp471=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp471[ 0]=({ struct Cyc_Core_Impossible_struct _temp472; _temp472.tag= Cyc_Core_Impossible;
_temp472.f1= _tag_arr("after_pathinfo -- non-aggregates!", sizeof( unsigned char),
34u); _temp472;}); _temp471;})); _LL455:;}} static void* Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv* env, struct _tagged_arr* key, void* r1, void* r2){
if( r1 ==  r2){ return r1;}{ struct Cyc_List_List** _temp473= env->last_field_cell;
enum  Cyc_CfFlowInfo_WhoIsChanged _temp474= env->changed;* env->last_field_cell=({
struct Cyc_List_List* _temp475=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp475->hd=( void*) key; _temp475->tl= 0; _temp475;});
env->last_field_cell=&(( struct Cyc_List_List*) _check_null(* env->last_field_cell))->tl;{
void* _temp476= Cyc_CfFlowInfo_after_absRval( env, r1, r2); env->last_field_cell=
_temp473;(( struct Cyc_List_List*) _check_null(* env->last_field_cell))->tl= 0;
env->changed= _temp474; return _temp476;}}} static void* Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv* env, void* root, void* r1, void* r2){ if( r1 == 
r2){ return r1;}* env->curr_place=({ struct Cyc_CfFlowInfo_Place _temp477;
_temp477.root=( void*) root; _temp477.fields= 0; _temp477;}); env->last_field_cell=&(
env->curr_place)->fields; env->changed= Cyc_CfFlowInfo_Neither; return Cyc_CfFlowInfo_after_absRval(
env, r1, r2);} void* Cyc_CfFlowInfo_after_flow( struct Cyc_Set_Set** all_changed,
void* f1, void* f2, struct Cyc_Set_Set* chg1, struct Cyc_Set_Set* chg2){ static
struct Cyc_Absyn_Const_e_struct dummy_rawexp={ 0u,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp dummy_exp={ 0,( void*)(( void*)& dummy_rawexp), 0,( void*)((
void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={ 1u,& dummy_exp}; if( f1 ==  f2){ return f1;}{ struct _tuple5
_temp479=({ struct _tuple5 _temp478; _temp478.f1= f1; _temp478.f2= f2; _temp478;});
void* _temp487; void* _temp489; void* _temp491; struct Cyc_Dict_Dict* _temp493;
void* _temp495; struct Cyc_Dict_Dict* _temp497; _LL481: _LL488: _temp487=
_temp479.f1; if( _temp487 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL482;}
else{ goto _LL483;} _LL483: _LL490: _temp489= _temp479.f2; if( _temp489 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL484;} else{ goto _LL485;} _LL485:
_LL496: _temp495= _temp479.f1; if(( unsigned int) _temp495 >  1u?*(( int*)
_temp495) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL498: _temp497=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp495)->f1; goto _LL492;} else{ goto _LL480;} _LL492: _temp491= _temp479.f2;
if(( unsigned int) _temp491 >  1u?*(( int*) _temp491) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL494: _temp493=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp491)->f1;
goto _LL486;} else{ goto _LL480;} _LL482: goto _LL484; _LL484: return( void*)
Cyc_CfFlowInfo_BottomFL; _LL486: if( _temp497 ==  _temp493){ return f1;}{ struct
_RegionHandle _temp499= _new_region(); struct _RegionHandle* rgn=& _temp499;
_push_region( rgn);{ struct Cyc_CfFlowInfo_Place* _temp500=({ struct Cyc_CfFlowInfo_Place*
_temp509=( struct Cyc_CfFlowInfo_Place*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp509->root=( void*)(( void*)& dummy_root); _temp509->fields= 0; _temp509;});
struct Cyc_CfFlowInfo_AfterEnv _temp501=({ struct Cyc_CfFlowInfo_AfterEnv
_temp506; _temp506.joinenv=({ struct Cyc_CfFlowInfo_JoinEnv _temp507; _temp507.pile=({
struct Cyc_CfFlowInfo_EscPile* _temp508=( struct Cyc_CfFlowInfo_EscPile*)
_region_malloc( rgn, sizeof( struct Cyc_CfFlowInfo_EscPile)); _temp508->rgn= rgn;
_temp508->places= 0; _temp508;}); _temp507.d1= _temp497; _temp507.d2= _temp493;
_temp507;}); _temp506.chg1= chg1; _temp506.chg2= chg2; _temp506.curr_place=
_temp500; _temp506.last_field_cell=& _temp500->fields; _temp506.changed= Cyc_CfFlowInfo_Neither;
_temp506;}); struct Cyc_Dict_Dict* _temp502=(( struct Cyc_Dict_Dict*(*)( void*(*
f)( struct Cyc_CfFlowInfo_AfterEnv*, void*, void*, void*), struct Cyc_CfFlowInfo_AfterEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_union_two_c)(
Cyc_CfFlowInfo_after_root,& _temp501, _temp497, _temp493); void* _temp505=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp503=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp503[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp504; _temp504.tag= Cyc_CfFlowInfo_ReachableFL;
_temp504.f1= Cyc_CfFlowInfo_escape_these(( _temp501.joinenv).pile, all_changed,
_temp502); _temp504;}); _temp503;}); _npop_handler( 0u); return _temp505;};
_pop_region( rgn);} _LL480:;}} static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void* ignore, void* r1, void* r2){ if( r1 ==  r2){ return 1;}{ struct _tuple5
_temp513=({ struct _tuple5 _temp512; _temp512.f1= r1; _temp512.f2= r2; _temp512;});
void* _temp535; struct Cyc_CfFlowInfo_Place* _temp537; void* _temp539; struct
Cyc_CfFlowInfo_Place* _temp541; void* _temp543; void* _temp545; void* _temp547;
struct Cyc_Dict_Dict* _temp549; void* _temp551; struct Cyc_Dict_Dict* _temp553;
void* _temp555; void* _temp557; void* _temp559; void* _temp561; void* _temp563;
void* _temp565; _LL515: _LL540: _temp539= _temp513.f1; if(( unsigned int)
_temp539 >  3u?*(( int*) _temp539) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL542:
_temp541=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp539)->f1; goto _LL536;}
else{ goto _LL517;} _LL536: _temp535= _temp513.f2; if(( unsigned int) _temp535 > 
3u?*(( int*) _temp535) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL538: _temp537=((
struct Cyc_CfFlowInfo_AddressOf_struct*) _temp535)->f1; goto _LL516;} else{ goto
_LL517;} _LL517: _LL544: _temp543= _temp513.f1; if(( unsigned int) _temp543 >  3u?*((
int*) _temp543) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL518;} else{ goto
_LL519;} _LL519: _LL546: _temp545= _temp513.f2; if(( unsigned int) _temp545 >  3u?*((
int*) _temp545) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL520;} else{ goto
_LL521;} _LL521: _LL552: _temp551= _temp513.f1; if(( unsigned int) _temp551 >  3u?*((
int*) _temp551) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL554: _temp553=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp551)->f1; goto _LL548;} else{ goto _LL523;} _LL548: _temp547= _temp513.f2;
if(( unsigned int) _temp547 >  3u?*(( int*) _temp547) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL550: _temp549=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp547)->f1;
goto _LL522;} else{ goto _LL523;} _LL523: _LL556: _temp555= _temp513.f2; if(
_temp555 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto _LL524;} else{ goto
_LL525;} _LL525: _LL558: _temp557= _temp513.f2; if( _temp557 == ( void*) Cyc_CfFlowInfo_Zero){
goto _LL526;} else{ goto _LL527;} _LL527: _LL560: _temp559= _temp513.f2; if(
_temp559 == ( void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL528;} else{ goto _LL529;}
_LL529: _LL564: _temp563= _temp513.f1; if(( unsigned int) _temp563 >  3u?*(( int*)
_temp563) ==  Cyc_CfFlowInfo_Esc: 0){ goto _LL562;} else{ goto _LL531;} _LL562:
_temp561= _temp513.f2; if(( unsigned int) _temp561 >  3u?*(( int*) _temp561) == 
Cyc_CfFlowInfo_Esc: 0){ goto _LL530;} else{ goto _LL531;} _LL531: _LL566:
_temp565= _temp513.f1; if(( unsigned int) _temp565 >  3u?*(( int*) _temp565) == 
Cyc_CfFlowInfo_Esc: 0){ goto _LL532;} else{ goto _LL533;} _LL533: goto _LL534;
_LL516: return Cyc_CfFlowInfo_place_cmp( _temp541, _temp537) ==  0; _LL518: goto
_LL520; _LL520: return 0; _LL522: return _temp553 ==  _temp549? 1:(( int(*)( int(*
f)( struct _tagged_arr*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_arr* ignore, void* r1,
void* r2)) Cyc_CfFlowInfo_absRval_lessthan_approx, _temp553, _temp549); _LL524:
return r1 == ( void*) Cyc_CfFlowInfo_NotZeroAll; _LL526: goto _LL528; _LL528:
return 0; _LL530: goto _LL514; _LL532: return 0; _LL534: goto _LL514; _LL514:;}{
struct _tuple5 _temp568=({ struct _tuple5 _temp567; _temp567.f1= Cyc_CfFlowInfo_initlevel_approx(
r1); _temp567.f2= Cyc_CfFlowInfo_initlevel_approx( r2); _temp567;}); void*
_temp580; void* _temp582; void* _temp584; void* _temp586; void* _temp588; void*
_temp590; _LL570: _LL583: _temp582= _temp568.f1; if( _temp582 == ( void*) Cyc_CfFlowInfo_AllIL){
goto _LL581;} else{ goto _LL572;} _LL581: _temp580= _temp568.f2; if( _temp580 == (
void*) Cyc_CfFlowInfo_AllIL){ goto _LL571;} else{ goto _LL572;} _LL572: _LL585:
_temp584= _temp568.f2; if( _temp584 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL573;} else{ goto _LL574;} _LL574: _LL587: _temp586= _temp568.f1; if( _temp586
== ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL575;} else{ goto _LL576;} _LL576:
_LL589: _temp588= _temp568.f2; if( _temp588 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL577;} else{ goto _LL578;} _LL578: _LL591: _temp590= _temp568.f1; if(
_temp590 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL579;} else{ goto _LL569;}
_LL571: return 1; _LL573: return 1; _LL575: return 0; _LL577: return 1; _LL579:
return 0; _LL569:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2){
if( f1 ==  f2){ return 1;}{ struct _tuple5 _temp593=({ struct _tuple5 _temp592;
_temp592.f1= f1; _temp592.f2= f2; _temp592;}); void* _temp601; void* _temp603;
void* _temp605; struct Cyc_Dict_Dict* _temp607; void* _temp609; struct Cyc_Dict_Dict*
_temp611; _LL595: _LL602: _temp601= _temp593.f1; if( _temp601 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL596;} else{ goto _LL597;} _LL597: _LL604: _temp603= _temp593.f2; if(
_temp603 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL598;} else{ goto _LL599;}
_LL599: _LL610: _temp609= _temp593.f1; if(( unsigned int) _temp609 >  1u?*(( int*)
_temp609) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL612: _temp611=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp609)->f1; goto _LL606;} else{ goto _LL594;} _LL606: _temp605= _temp593.f2;
if(( unsigned int) _temp605 >  1u?*(( int*) _temp605) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL608: _temp607=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp605)->f1;
goto _LL600;} else{ goto _LL594;} _LL596: return 1; _LL598: return 0; _LL600:
if( _temp611 ==  _temp607){ return 1;} return Cyc_Dict_forall_intersect( Cyc_CfFlowInfo_absRval_lessthan_approx,
_temp611, _temp607); _LL594:;}}
