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
extern int Cyc_Core_ptrcmp( void**, void**); extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[
8u]; extern int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List*
l, void* x); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* cmp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern struct Cyc_List_List* Cyc_Set_elements( struct Cyc_Set_Set*
s); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k, void* v);
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k); extern void Cyc_Dict_iter_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern
struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_union_two_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2); extern int Cyc_Dict_forall_c( int(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d); extern int Cyc_Dict_forall_intersect(
int(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*);
struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line;
int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct
Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt;
struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_SwitchC_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Static= 0;
static const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2;
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
struct _tuple0* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
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
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e= 14; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static const
int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e= 16;
struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
22; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 23; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
24; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 31; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
32; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 33; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 34; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 35; struct Cyc_Absyn_Fill_e_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); struct Cyc_CfFlowInfo_Place; enum  Cyc_CfFlowInfo_Escaped; struct Cyc_CfFlowInfo_InitState;
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ; enum 
Cyc_CfFlowInfo_Escaped{ Cyc_CfFlowInfo_Esc  =  0u, Cyc_CfFlowInfo_Unesc  =  1u};
static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_MustPointTo=
0; struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; struct Cyc_CfFlowInfo_InitState{ enum  Cyc_CfFlowInfo_Escaped esc; void*
level; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; struct Cyc_CfFlowInfo_InitState f1; } ; static const int Cyc_CfFlowInfo_DictPI=
1; struct Cyc_CfFlowInfo_DictPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern struct Cyc_Set_Set* Cyc_CfFlowInfo_mt_place_set(); extern void* Cyc_CfFlowInfo_mkLeafPI(
enum  Cyc_CfFlowInfo_Escaped esc, void* il); extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern int Cyc_CfFlowInfo_place_cmp( struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
void* t); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo);
extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo,
struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed); extern void* Cyc_CfFlowInfo_assign_unknown_dict(
void*, void*); extern void* Cyc_CfFlowInfo_join_flow( struct Cyc_Set_Set**, void*,
void*); extern void* Cyc_CfFlowInfo_after_flow( struct Cyc_Set_Set**, void*,
void*, struct Cyc_Set_Set*, struct Cyc_Set_Set*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); static struct Cyc_Set_Set** Cyc_CfFlowInfo_mt_place_set_opt=
0; struct Cyc_Set_Set* Cyc_CfFlowInfo_mt_place_set(){ if( Cyc_CfFlowInfo_mt_place_set_opt
==  0){ Cyc_CfFlowInfo_mt_place_set_opt=({ struct Cyc_Set_Set** _temp0=( struct
Cyc_Set_Set**) _cycalloc( sizeof( struct Cyc_Set_Set*)); _temp0[ 0]=(( struct
Cyc_Set_Set*(*)( int(* cmp)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*)))
Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp); _temp0;});} return*(( struct Cyc_Set_Set**)
_check_null( Cyc_CfFlowInfo_mt_place_set_opt));} static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_none={ 0u,{ Cyc_CfFlowInfo_Unesc,( void*)(( void*) 0u)}};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_this={ 0u,{
Cyc_CfFlowInfo_Unesc,( void*)(( void*) 1u)}}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_all={ 0u,{ Cyc_CfFlowInfo_Unesc,( void*)(( void*) 2u)}};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_none={ 0u,{
Cyc_CfFlowInfo_Esc,( void*)(( void*) 0u)}}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_this={ 0u,{ Cyc_CfFlowInfo_Esc,( void*)(( void*) 1u)}};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_all={ 0u,{
Cyc_CfFlowInfo_Esc,( void*)(( void*) 2u)}}; struct _tuple3{ enum  Cyc_CfFlowInfo_Escaped
f1; void* f2; } ; void* Cyc_CfFlowInfo_mkLeafPI( enum  Cyc_CfFlowInfo_Escaped
esc, void* il){ struct _tuple3 _temp8=({ struct _tuple3 _temp7; _temp7.f1= esc;
_temp7.f2= il; _temp7;}); void* _temp24; enum  Cyc_CfFlowInfo_Escaped _temp26;
void* _temp28; enum  Cyc_CfFlowInfo_Escaped _temp30; void* _temp32; enum  Cyc_CfFlowInfo_Escaped
_temp34; void* _temp36; enum  Cyc_CfFlowInfo_Escaped _temp38; void* _temp40;
enum  Cyc_CfFlowInfo_Escaped _temp42; void* _temp44; enum  Cyc_CfFlowInfo_Escaped
_temp46; _LL10: _LL27: _temp26= _temp8.f1; if( _temp26 ==  Cyc_CfFlowInfo_Unesc){
goto _LL25;} else{ goto _LL12;} _LL25: _temp24= _temp8.f2; if( _temp24 == ( void*)
Cyc_CfFlowInfo_NoneIL){ goto _LL11;} else{ goto _LL12;} _LL12: _LL31: _temp30=
_temp8.f1; if( _temp30 ==  Cyc_CfFlowInfo_Unesc){ goto _LL29;} else{ goto _LL14;}
_LL29: _temp28= _temp8.f2; if( _temp28 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto
_LL13;} else{ goto _LL14;} _LL14: _LL35: _temp34= _temp8.f1; if( _temp34 ==  Cyc_CfFlowInfo_Unesc){
goto _LL33;} else{ goto _LL16;} _LL33: _temp32= _temp8.f2; if( _temp32 == ( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL15;} else{ goto _LL16;} _LL16: _LL39: _temp38=
_temp8.f1; if( _temp38 ==  Cyc_CfFlowInfo_Esc){ goto _LL37;} else{ goto _LL18;}
_LL37: _temp36= _temp8.f2; if( _temp36 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL17;} else{ goto _LL18;} _LL18: _LL43: _temp42= _temp8.f1; if( _temp42 ==  Cyc_CfFlowInfo_Esc){
goto _LL41;} else{ goto _LL20;} _LL41: _temp40= _temp8.f2; if( _temp40 == ( void*)
Cyc_CfFlowInfo_ThisIL){ goto _LL19;} else{ goto _LL20;} _LL20: _LL47: _temp46=
_temp8.f1; if( _temp46 ==  Cyc_CfFlowInfo_Esc){ goto _LL45;} else{ goto _LL22;}
_LL45: _temp44= _temp8.f2; if( _temp44 == ( void*) Cyc_CfFlowInfo_AllIL){ goto
_LL21;} else{ goto _LL22;} _LL22: goto _LL23; _LL11: return( void*)& Cyc_CfFlowInfo_leaf_unesc_none;
_LL13: return( void*)& Cyc_CfFlowInfo_leaf_unesc_this; _LL15: return( void*)&
Cyc_CfFlowInfo_leaf_unesc_all; _LL17: return( void*)& Cyc_CfFlowInfo_leaf_esc_none;
_LL19: return( void*)& Cyc_CfFlowInfo_leaf_esc_this; _LL21: return( void*)& Cyc_CfFlowInfo_leaf_esc_all;
_LL23: return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct* _temp48=( struct
Cyc_CfFlowInfo_LeafPI_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp48[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp49; _temp49.tag= Cyc_CfFlowInfo_LeafPI;
_temp49.f1=({ struct Cyc_CfFlowInfo_InitState _temp50; _temp50.esc= esc; _temp50.level=(
void*) il; _temp50;}); _temp49;}); _temp48;}); _LL9:;} struct _tuple4{ void* f1;
void* f2; } ; int Cyc_CfFlowInfo_local_root_cmp( void* r1, void* r2){ if( r1 == 
r2){ return 0;}{ struct _tuple4 _temp52=({ struct _tuple4 _temp51; _temp51.f1=
r1; _temp51.f2= r2; _temp51;}); void* _temp62; void* _temp64; void* _temp66;
void* _temp68; void* _temp70; struct Cyc_Absyn_Vardecl* _temp72; void* _temp74;
struct Cyc_Absyn_Vardecl* _temp76; void* _temp78; struct Cyc_Absyn_Exp* _temp80;
void* _temp82; struct Cyc_Absyn_Exp* _temp84; _LL54: _LL65: _temp64= _temp52.f1;
if(*(( int*) _temp64) ==  Cyc_CfFlowInfo_VarRoot){ goto _LL63;} else{ goto _LL56;}
_LL63: _temp62= _temp52.f2; if(*(( int*) _temp62) ==  Cyc_CfFlowInfo_MallocPt){
goto _LL55;} else{ goto _LL56;} _LL56: _LL69: _temp68= _temp52.f1; if(*(( int*)
_temp68) ==  Cyc_CfFlowInfo_MallocPt){ goto _LL67;} else{ goto _LL58;} _LL67:
_temp66= _temp52.f2; if(*(( int*) _temp66) ==  Cyc_CfFlowInfo_VarRoot){ goto
_LL57;} else{ goto _LL58;} _LL58: _LL75: _temp74= _temp52.f1; if(*(( int*)
_temp74) ==  Cyc_CfFlowInfo_VarRoot){ _LL77: _temp76=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp74)->f1; goto _LL71;} else{ goto _LL60;} _LL71: _temp70= _temp52.f2; if(*((
int*) _temp70) ==  Cyc_CfFlowInfo_VarRoot){ _LL73: _temp72=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp70)->f1; goto _LL59;} else{ goto _LL60;} _LL60: _LL83: _temp82= _temp52.f1;
if(*(( int*) _temp82) ==  Cyc_CfFlowInfo_MallocPt){ _LL85: _temp84=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp82)->f1; goto _LL79;} else{ goto _LL53;} _LL79: _temp78= _temp52.f2; if(*((
int*) _temp78) ==  Cyc_CfFlowInfo_MallocPt){ _LL81: _temp80=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp78)->f1; goto _LL61;} else{ goto _LL53;} _LL55: return 1; _LL57: return - 1;
_LL59: return(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp)( _temp76, _temp72); _LL61: return(( int(*)( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp)( _temp84, _temp80); _LL53:;}} int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place* p1, struct Cyc_CfFlowInfo_Place* p2){ if(( int) p1
== ( int) p2){ return 0;}{ int i= Cyc_CfFlowInfo_local_root_cmp(( void*) p1->root,(
void*) p2->root); if( i !=  0){ return i;} return(( int(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_cmp)( Cyc_Std_zstrptrcmp, p1->fields, p2->fields);}} struct
_tagged_arr* Cyc_CfFlowInfo_place2string( struct Cyc_CfFlowInfo_Place* p){
struct Cyc_List_List* sl= 0;{ void* _temp86=( void*) p->root; struct Cyc_Absyn_Vardecl*
_temp92; struct Cyc_Absyn_Exp* _temp94; _LL88: if(*(( int*) _temp86) ==  Cyc_CfFlowInfo_VarRoot){
_LL93: _temp92=(( struct Cyc_CfFlowInfo_VarRoot_struct*) _temp86)->f1; goto
_LL89;} else{ goto _LL90;} _LL90: if(*(( int*) _temp86) ==  Cyc_CfFlowInfo_MallocPt){
_LL95: _temp94=(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp86)->f1; goto
_LL91;} else{ goto _LL87;} _LL89: sl=({ struct Cyc_List_List* _temp96=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp96->hd=( void*)({
struct _tagged_arr* _temp97=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp97[ 0]=({ struct Cyc_Std_String_pa_struct _temp99; _temp99.tag=
Cyc_Std_String_pa; _temp99.f1=( struct _tagged_arr)*(* _temp92->name).f2;{ void*
_temp98[ 1u]={& _temp99}; Cyc_Std_aprintf( _tag_arr("%s", sizeof( unsigned char),
3u), _tag_arr( _temp98, sizeof( void*), 1u));}}); _temp97;}); _temp96->tl= sl;
_temp96;}); goto _LL87; _LL91: sl=({ struct Cyc_List_List* _temp100=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp100->hd=( void*)({ struct
_tagged_arr* _temp101=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp101[ 0]=({ struct Cyc_Std_Int_pa_struct _temp103; _temp103.tag=
Cyc_Std_Int_pa; _temp103.f1=( int)(( unsigned int)(( int) _temp94));{ void*
_temp102[ 1u]={& _temp103}; Cyc_Std_aprintf( _tag_arr("mpt%d", sizeof(
unsigned char), 6u), _tag_arr( _temp102, sizeof( void*), 1u));}}); _temp101;});
_temp100->tl= sl; _temp100;}); _LL87:;}{ struct Cyc_List_List* fields= p->fields;
for( 0; fields !=  0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
sl=({ struct Cyc_List_List* _temp104=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp104->hd=( void*)({ struct _tagged_arr* _temp105=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp105[ 0]=({
struct Cyc_Std_String_pa_struct _temp107; _temp107.tag= Cyc_Std_String_pa;
_temp107.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( fields))->hd);{ void* _temp106[ 1u]={& _temp107}; Cyc_Std_aprintf(
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp106, sizeof( void*),
1u));}}); _temp105;}); _temp104->tl= sl; _temp104;});}}{ struct _tagged_arr*
_temp108=({ struct _tagged_arr* _temp112=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp112[ 0]=({ struct Cyc_Std_String_pa_struct
_temp114; _temp114.tag= Cyc_Std_String_pa; _temp114.f1= _tag_arr("", sizeof(
unsigned char), 1u);{ void* _temp113[ 1u]={& _temp114}; Cyc_Std_aprintf(
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp113, sizeof( void*),
1u));}}); _temp112;}); for( 0; sl !=  0; sl=(( struct Cyc_List_List*)
_check_null( sl))->tl){* _temp108=({ struct Cyc_Std_String_pa_struct _temp111;
_temp111.tag= Cyc_Std_String_pa; _temp111.f1=( struct _tagged_arr)* _temp108;{
struct Cyc_Std_String_pa_struct _temp110; _temp110.tag= Cyc_Std_String_pa;
_temp110.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( sl))->hd);{ void* _temp109[ 2u]={& _temp110,& _temp111}; Cyc_Std_aprintf(
_tag_arr("%s.%s", sizeof( unsigned char), 6u), _tag_arr( _temp109, sizeof( void*),
2u));}}});} return _temp108;}} static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place* place, struct Cyc_Set_Set* set){{ struct Cyc_List_List*
_temp115=(( struct Cyc_List_List*(*)( struct Cyc_Set_Set* s)) Cyc_Set_elements)(
set); for( 0; _temp115 !=  0; _temp115=(( struct Cyc_List_List*) _check_null(
_temp115))->tl){ struct Cyc_CfFlowInfo_Place* _temp116=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*) _check_null( _temp115))->hd; if( Cyc_CfFlowInfo_local_root_cmp((
void*) place->root,( void*) _temp116->root) !=  0){ continue;}{ struct Cyc_List_List*
_temp117= place->fields; struct Cyc_List_List* _temp118= _temp116->fields; for(
0; _temp117 !=  0? _temp118 !=  0: 0;( _temp117=(( struct Cyc_List_List*)
_check_null( _temp117))->tl, _temp118=(( struct Cyc_List_List*) _check_null(
_temp118))->tl)){ if( Cyc_Std_zstrptrcmp(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp117))->hd,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp118))->hd) !=  0){ break;}} if( _temp117 ==  0){ return 1;}}}}
return 0;} struct _tuple5{ struct Cyc_Absyn_Tqual f1; void* f2; } ; void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
void* t){ void* _temp119= Cyc_Tcutil_compress( t); struct Cyc_Absyn_TunionFieldInfo
_temp131; void* _temp133; struct Cyc_Absyn_Tunionfield* _temp135; struct Cyc_List_List*
_temp137; struct Cyc_List_List* _temp139; struct Cyc_Absyn_Structdecl** _temp141;
struct Cyc_Absyn_Structdecl* _temp143; struct Cyc_Absyn_Structdecl _temp144;
struct Cyc_Core_Opt* _temp145; struct Cyc_Core_Opt _temp147; struct Cyc_List_List*
_temp148; _LL121: if(( unsigned int) _temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_TunionFieldType:
0){ _LL132: _temp131=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp119)->f1;
_LL134: _temp133=( void*) _temp131.field_info; if(*(( int*) _temp133) ==  Cyc_Absyn_KnownTunionfield){
_LL136: _temp135=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp133)->f2;
goto _LL122;} else{ goto _LL123;}} else{ goto _LL123;} _LL123: if(( unsigned int)
_temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_TupleType: 0){ _LL138: _temp137=((
struct Cyc_Absyn_TupleType_struct*) _temp119)->f1; goto _LL124;} else{ goto
_LL125;} _LL125: if(( unsigned int) _temp119 >  4u?*(( int*) _temp119) ==  Cyc_Absyn_AnonStructType:
0){ _LL140: _temp139=(( struct Cyc_Absyn_AnonStructType_struct*) _temp119)->f1;
goto _LL126;} else{ goto _LL127;} _LL127: if(( unsigned int) _temp119 >  4u?*((
int*) _temp119) ==  Cyc_Absyn_StructType: 0){ _LL142: _temp141=(( struct Cyc_Absyn_StructType_struct*)
_temp119)->f3; if( _temp141 ==  0){ goto _LL129;} else{ _temp143=* _temp141;
_temp144=* _temp143; _LL146: _temp145= _temp144.fields; if( _temp145 ==  0){
goto _LL129;} else{ _temp147=* _temp145; _LL149: _temp148=( struct Cyc_List_List*)
_temp147.v; goto _LL128;}}} else{ goto _LL129;} _LL129: goto _LL130; _LL122: if(
_temp135->typs ==  0){ return( void*)& Cyc_CfFlowInfo_leaf_unesc_none;} _temp137=
_temp135->typs; goto _LL124; _LL124: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{
int i= 0; for( 0; _temp137 !=  0;( _temp137=(( struct Cyc_List_List*)
_check_null( _temp137))->tl, ++ i)){ d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( d, Cyc_Absyn_fieldname( i),
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp137))->hd)).f2));}} return( void*)({ struct Cyc_CfFlowInfo_DictPI_struct*
_temp150=( struct Cyc_CfFlowInfo_DictPI_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct));
_temp150[ 0]=({ struct Cyc_CfFlowInfo_DictPI_struct _temp151; _temp151.tag= Cyc_CfFlowInfo_DictPI;
_temp151.f1= d; _temp151;}); _temp150;});} _LL126: _temp148= _temp139; goto
_LL128; _LL128: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
for( 0; _temp148 !=  0; _temp148=(( struct Cyc_List_List*) _check_null( _temp148))->tl){
struct Cyc_Absyn_Structfield _temp154; void* _temp155; struct _tagged_arr*
_temp157; struct Cyc_Absyn_Structfield* _temp152=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp148))->hd; _temp154=* _temp152; _LL158:
_temp157= _temp154.name; goto _LL156; _LL156: _temp155=( void*) _temp154.type;
goto _LL153; _LL153: if( _get_arr_size(* _temp157, sizeof( unsigned char)) !=  1){
d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k,
void* v)) Cyc_Dict_insert)( d, _temp157, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp155));}} return( void*)({ struct Cyc_CfFlowInfo_DictPI_struct* _temp159=(
struct Cyc_CfFlowInfo_DictPI_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct));
_temp159[ 0]=({ struct Cyc_CfFlowInfo_DictPI_struct _temp160; _temp160.tag= Cyc_CfFlowInfo_DictPI;
_temp160.f1= d; _temp160;}); _temp159;});} _LL130: return( void*)& Cyc_CfFlowInfo_leaf_unesc_none;
_LL120:;} void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place){ struct Cyc_CfFlowInfo_Place _temp163; struct Cyc_List_List* _temp164;
void* _temp166; struct Cyc_CfFlowInfo_Place* _temp161= place; _temp163=*
_temp161; _LL167: _temp166=( void*) _temp163.root; goto _LL165; _LL165: _temp164=
_temp163.fields; goto _LL162; _LL162: { void* _temp168= Cyc_Dict_lookup( d,
_temp166); for( 0; _temp164 !=  0; _temp164=(( struct Cyc_List_List*)
_check_null( _temp164))->tl){ struct _tuple0 _temp170=({ struct _tuple0 _temp169;
_temp169.f1= _temp168; _temp169.f2=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp164))->hd; _temp169;}); struct _tagged_arr* _temp176; void*
_temp178; struct Cyc_Dict_Dict* _temp180; _LL172: _LL179: _temp178= _temp170.f1;
if(*(( int*) _temp178) ==  Cyc_CfFlowInfo_DictPI){ _LL181: _temp180=(( struct
Cyc_CfFlowInfo_DictPI_struct*) _temp178)->f1; goto _LL177;} else{ goto _LL174;}
_LL177: _temp176= _temp170.f2; goto _LL173; _LL174: goto _LL175; _LL173:
_temp168=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
_temp180, _temp176); goto _LL171; _LL175:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp182=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp182[ 0]=({ struct Cyc_Core_Impossible_struct _temp183; _temp183.tag= Cyc_Core_Impossible;
_temp183.f1= _tag_arr("bad lookup_place", sizeof( unsigned char), 17u); _temp183;});
_temp182;})); _LL171:;} return _temp168;}} struct _tuple6{ void* f1; struct Cyc_List_List*
f2; } ; void* Cyc_CfFlowInfo_insert_place_rec( void* insert_path, struct Cyc_List_List*
fields, void* pinfo){ if( fields ==  0){ return pinfo;}{ struct _tuple6 _temp185=({
struct _tuple6 _temp184; _temp184.f1= insert_path; _temp184.f2= fields; _temp184;});
struct Cyc_List_List* _temp191; struct Cyc_List_List _temp193; struct Cyc_List_List*
_temp194; struct _tagged_arr* _temp196; void* _temp198; struct Cyc_Dict_Dict*
_temp200; _LL187: _LL199: _temp198= _temp185.f1; if(*(( int*) _temp198) ==  Cyc_CfFlowInfo_DictPI){
_LL201: _temp200=(( struct Cyc_CfFlowInfo_DictPI_struct*) _temp198)->f1; goto
_LL192;} else{ goto _LL189;} _LL192: _temp191= _temp185.f2; if( _temp191 ==  0){
goto _LL189;} else{ _temp193=* _temp191; _LL197: _temp196=( struct _tagged_arr*)
_temp193.hd; goto _LL195; _LL195: _temp194= _temp193.tl; goto _LL188;} _LL189:
goto _LL190; _LL188: { void* _temp202=(( void*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp200, _temp196); return( void*)({
struct Cyc_CfFlowInfo_DictPI_struct* _temp203=( struct Cyc_CfFlowInfo_DictPI_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct)); _temp203[ 0]=({ struct
Cyc_CfFlowInfo_DictPI_struct _temp204; _temp204.tag= Cyc_CfFlowInfo_DictPI;
_temp204.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k, void* v)) Cyc_Dict_insert)( _temp200, _temp196, Cyc_CfFlowInfo_insert_place_rec(
_temp202, _temp194, pinfo)); _temp204;}); _temp203;});} _LL190:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp205=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp205[ 0]=({ struct
Cyc_Core_Impossible_struct _temp206; _temp206.tag= Cyc_Core_Impossible; _temp206.f1=
_tag_arr("bad insert_place", sizeof( unsigned char), 17u); _temp206;}); _temp205;}));
_LL186:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict*
d, struct Cyc_CfFlowInfo_Place* place, void* pinfo){ struct Cyc_CfFlowInfo_Place
_temp209; struct Cyc_List_List* _temp210; void* _temp212; struct Cyc_CfFlowInfo_Place*
_temp207= place; _temp209=* _temp207; _LL213: _temp212=( void*) _temp209.root;
goto _LL211; _LL211: _temp210= _temp209.fields; goto _LL208; _LL208: return Cyc_Dict_insert(
d, _temp212, Cyc_CfFlowInfo_insert_place_rec( Cyc_Dict_lookup( d, _temp212),
_temp210, pinfo));} struct Cyc_CfFlowInfo_AllInitEnv{ struct Cyc_Dict_Dict*
top_info; struct Cyc_List_List* seen; } ; int Cyc_CfFlowInfo_isAllInit_pinfo(
struct Cyc_CfFlowInfo_AllInitEnv* env, void* path); int Cyc_CfFlowInfo_isAllInit_is(
struct Cyc_CfFlowInfo_AllInitEnv* env, struct Cyc_CfFlowInfo_InitState state){
void* _temp214=( void*) state.level; struct Cyc_CfFlowInfo_Place* _temp224;
_LL216: if( _temp214 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL217;} else{
goto _LL218;} _LL218: if( _temp214 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto
_LL219;} else{ goto _LL220;} _LL220: if( _temp214 == ( void*) Cyc_CfFlowInfo_AllIL){
goto _LL221;} else{ goto _LL222;} _LL222: if(( unsigned int) _temp214 >  3u?*((
int*) _temp214) ==  Cyc_CfFlowInfo_MustPointTo: 0){ _LL225: _temp224=(( struct
Cyc_CfFlowInfo_MustPointTo_struct*) _temp214)->f1; goto _LL223;} else{ goto
_LL215;} _LL217: goto _LL219; _LL219: return 0; _LL221: return 1; _LL223: if(((
int(*)( int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,
env->seen, _temp224)){ return 1;} env->seen=({ struct Cyc_List_List* _temp226=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp226->hd=(
void*) _temp224; _temp226->tl= env->seen; _temp226;}); return Cyc_CfFlowInfo_isAllInit_pinfo(
env, Cyc_CfFlowInfo_lookup_place( env->top_info, _temp224)); _LL215:;} int Cyc_CfFlowInfo_isAllInit_f(
struct Cyc_CfFlowInfo_AllInitEnv* env, void* ignore, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct Cyc_CfFlowInfo_AllInitEnv*
env, void* path){ void* _temp227= path; struct Cyc_CfFlowInfo_InitState _temp233;
struct Cyc_Dict_Dict* _temp235; _LL229: if(*(( int*) _temp227) ==  Cyc_CfFlowInfo_LeafPI){
_LL234: _temp233=(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp227)->f1; goto
_LL230;} else{ goto _LL231;} _LL231: if(*(( int*) _temp227) ==  Cyc_CfFlowInfo_DictPI){
_LL236: _temp235=(( struct Cyc_CfFlowInfo_DictPI_struct*) _temp227)->f1; goto
_LL232;} else{ goto _LL228;} _LL230: return Cyc_CfFlowInfo_isAllInit_is( env,
_temp233); _LL232: return(( int(*)( int(* f)( struct Cyc_CfFlowInfo_AllInitEnv*,
struct _tagged_arr*, void*), struct Cyc_CfFlowInfo_AllInitEnv* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_forall_c)(( int(*)( struct Cyc_CfFlowInfo_AllInitEnv* env, struct
_tagged_arr* ignore, void* path)) Cyc_CfFlowInfo_isAllInit_f, env, _temp235);
_LL228:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo){ struct Cyc_CfFlowInfo_AllInitEnv _temp237=({ struct Cyc_CfFlowInfo_AllInitEnv
_temp238; _temp238.top_info= pinfo_dict; _temp238.seen= 0; _temp238;}); return
Cyc_CfFlowInfo_isAllInit_pinfo(& _temp237, pinfo);} static void Cyc_CfFlowInfo_add_place(
struct _RegionHandle* r, struct Cyc_List_List** places, struct Cyc_CfFlowInfo_Place*
place){ if( !(( int(*)( int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
places, place)){* places=({ struct Cyc_List_List* _temp239=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp239->hd=( void*) place;
_temp239->tl=* places; _temp239;});}} struct _tuple7{ struct Cyc_Dict_Dict* f1;
struct Cyc_List_List** f2; struct _RegionHandle* f3; } ; void* Cyc_CfFlowInfo_pinfo_getplaces_esc(
struct _tuple7* env, void* pinfo){ struct _tuple7 _temp242; struct _RegionHandle*
_temp243; struct Cyc_List_List** _temp245; struct Cyc_Dict_Dict* _temp247;
struct _tuple7* _temp240= env; _temp242=* _temp240; _LL248: _temp247= _temp242.f1;
goto _LL246; _LL246: _temp245= _temp242.f2; goto _LL244; _LL244: _temp243=
_temp242.f3; goto _LL241; _LL241: { void* _temp249= pinfo; struct Cyc_CfFlowInfo_InitState
_temp257; void* _temp259; struct Cyc_CfFlowInfo_Place* _temp261; struct Cyc_CfFlowInfo_InitState
_temp263; void* _temp265; struct Cyc_Dict_Dict* _temp267; _LL251: if(*(( int*)
_temp249) ==  Cyc_CfFlowInfo_LeafPI){ _LL258: _temp257=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp249)->f1; _LL260: _temp259=( void*) _temp257.level; if(( unsigned int)
_temp259 >  3u?*(( int*) _temp259) ==  Cyc_CfFlowInfo_MustPointTo: 0){ _LL262:
_temp261=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp259)->f1; goto
_LL252;} else{ goto _LL253;}} else{ goto _LL253;} _LL253: if(*(( int*) _temp249)
==  Cyc_CfFlowInfo_LeafPI){ _LL264: _temp263=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp249)->f1; _LL266: _temp265=( void*) _temp263.level; goto _LL254;} else{
goto _LL255;} _LL255: if(*(( int*) _temp249) ==  Cyc_CfFlowInfo_DictPI){ _LL268:
_temp267=(( struct Cyc_CfFlowInfo_DictPI_struct*) _temp249)->f1; goto _LL256;}
else{ goto _LL250;} _LL252: Cyc_CfFlowInfo_add_place( _temp243, _temp245,
_temp261); if( Cyc_CfFlowInfo_isAllInit( _temp247, Cyc_CfFlowInfo_lookup_place(
_temp247, _temp261))){ return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_ThisIL); _LL254: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc,
_temp265); _LL256: return( void*)({ struct Cyc_CfFlowInfo_DictPI_struct*
_temp269=( struct Cyc_CfFlowInfo_DictPI_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct));
_temp269[ 0]=({ struct Cyc_CfFlowInfo_DictPI_struct _temp270; _temp270.tag= Cyc_CfFlowInfo_DictPI;
_temp270.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp267); _temp270;}); _temp269;}); _LL250:;}} struct _tuple8{ struct Cyc_List_List**
f1; struct _RegionHandle* f2; } ; void Cyc_CfFlowInfo_pinfo_getplaces( struct
_tuple8* env, void* a, void* pinfo){ struct _tuple8 _temp273; struct
_RegionHandle* _temp274; struct Cyc_List_List** _temp276; struct _tuple8*
_temp271= env; _temp273=* _temp271; _LL277: _temp276= _temp273.f1; goto _LL275;
_LL275: _temp274= _temp273.f2; goto _LL272; _LL272: { void* _temp278= pinfo;
struct Cyc_CfFlowInfo_InitState _temp284; struct Cyc_Dict_Dict* _temp286; _LL280:
if(*(( int*) _temp278) ==  Cyc_CfFlowInfo_LeafPI){ _LL285: _temp284=(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp278)->f1; goto _LL281;} else{ goto _LL282;}
_LL282: if(*(( int*) _temp278) ==  Cyc_CfFlowInfo_DictPI){ _LL287: _temp286=((
struct Cyc_CfFlowInfo_DictPI_struct*) _temp278)->f1; goto _LL283;} else{ goto
_LL279;} _LL281: { void* _temp288=( void*) _temp284.level; struct Cyc_CfFlowInfo_Place*
_temp294; _LL290: if(( unsigned int) _temp288 >  3u?*(( int*) _temp288) ==  Cyc_CfFlowInfo_MustPointTo:
0){ _LL295: _temp294=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp288)->f1;
goto _LL291;} else{ goto _LL292;} _LL292: goto _LL293; _LL291: Cyc_CfFlowInfo_add_place(
_temp274, _temp276, _temp294); return; _LL293: return; _LL289:;} _LL283:(( void(*)(
void(* f)( struct _tuple8*, struct _tagged_arr*, void*), struct _tuple8* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple8* env,
struct _tagged_arr* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, env,
_temp286); return; _LL279:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct _RegionHandle* rgn, struct Cyc_List_List* places, struct Cyc_Dict_Dict* d,
struct Cyc_Set_Set** all_changed){ while( places !=  0) { struct Cyc_CfFlowInfo_Place*
_temp296=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_List_List*) _check_null(
places))->hd; places=(( struct Cyc_List_List*) _check_null( places))->tl; if(
all_changed !=  0){*(( struct Cyc_Set_Set**) _check_null( all_changed))=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( all_changed)), _temp296);}{
void* place_old_pinfo;{ struct _handler_cons _temp297; _push_handler(& _temp297);{
int _temp299= 0; if( setjmp( _temp297.handler)){ _temp299= 1;} if( ! _temp299){
place_old_pinfo= Cyc_CfFlowInfo_lookup_place( d, _temp296);; _pop_handler();}
else{ void* _temp298=( void*) _exn_thrown; void* _temp301= _temp298; _LL303: if(
_temp301 ==  Cyc_Dict_Absent){ goto _LL304;} else{ goto _LL305;} _LL305: goto
_LL306; _LL304: continue; _LL306:( void) _throw( _temp301); _LL302:;}}}{ struct
_tuple7 _temp307=({ struct _tuple7 _temp309; _temp309.f1= d; _temp309.f2=&
places; _temp309.f3= rgn; _temp309;}); void* _temp308= Cyc_CfFlowInfo_pinfo_getplaces_esc(&
_temp307, place_old_pinfo); if( place_old_pinfo !=  _temp308){ d= Cyc_CfFlowInfo_insert_place(
d, _temp296, _temp308);}}}} return d;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed){ struct
_RegionHandle _temp310= _new_region(); struct _RegionHandle* r=& _temp310;
_push_region( r);{ struct Cyc_List_List* _temp311= 0; struct _tuple8 _temp312=({
struct _tuple8 _temp314; _temp314.f1=& _temp311; _temp314.f2= r; _temp314;});((
void(*)( struct _tuple8* env, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(&
_temp312, 0, pinfo);{ struct Cyc_Dict_Dict* _temp313= Cyc_CfFlowInfo_escape_these(
r, _temp311, d, all_changed); _npop_handler( 0u); return _temp313;}};
_pop_region( r);} void* Cyc_CfFlowInfo_assign_unknown_dict( void* level, void*
pinfo){ void* _temp315= pinfo; struct Cyc_CfFlowInfo_InitState _temp321; enum 
Cyc_CfFlowInfo_Escaped _temp323; struct Cyc_Dict_Dict* _temp325; _LL317: if(*((
int*) _temp315) ==  Cyc_CfFlowInfo_LeafPI){ _LL322: _temp321=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp315)->f1; _LL324: _temp323= _temp321.esc; goto _LL318;} else{ goto _LL319;}
_LL319: if(*(( int*) _temp315) ==  Cyc_CfFlowInfo_DictPI){ _LL326: _temp325=((
struct Cyc_CfFlowInfo_DictPI_struct*) _temp315)->f1; goto _LL320;} else{ goto
_LL316;} _LL318: return Cyc_CfFlowInfo_mkLeafPI( _temp323, level); _LL320:
return( void*)({ struct Cyc_CfFlowInfo_DictPI_struct* _temp327=( struct Cyc_CfFlowInfo_DictPI_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct)); _temp327[ 0]=({ struct
Cyc_CfFlowInfo_DictPI_struct _temp328; _temp328.tag= Cyc_CfFlowInfo_DictPI;
_temp328.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp325); _temp328;}); _temp327;}); _LL316:;} struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2; struct Cyc_List_List**
places; struct _RegionHandle* rgn; } ; enum  Cyc_CfFlowInfo_WhoIsChanged{ Cyc_CfFlowInfo_Neither
 =  0u, Cyc_CfFlowInfo_One  =  1u, Cyc_CfFlowInfo_Two  =  2u}; struct Cyc_CfFlowInfo_AfterEnv{
struct Cyc_CfFlowInfo_JoinEnv joinenv; struct Cyc_Set_Set* chg1; struct Cyc_Set_Set*
chg2; struct Cyc_CfFlowInfo_Place* curr_place; enum  Cyc_CfFlowInfo_WhoIsChanged
changed; struct Cyc_List_List** last_field_cell; } ; static void* Cyc_CfFlowInfo_join_initstate(
struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_CfFlowInfo_InitState state1,
struct Cyc_CfFlowInfo_InitState state2){ void* result_level; enum  Cyc_CfFlowInfo_Escaped
result_esc;{ struct _tuple4 _temp330=({ struct _tuple4 _temp329; _temp329.f1=(
void*) state1.level; _temp329.f2=( void*) state2.level; _temp329;}); void*
_temp348; struct Cyc_CfFlowInfo_Place* _temp350; void* _temp352; struct Cyc_CfFlowInfo_Place*
_temp354; void* _temp356; void* _temp358; struct Cyc_CfFlowInfo_Place* _temp360;
void* _temp362; struct Cyc_CfFlowInfo_Place* _temp364; void* _temp366; void*
_temp368; void* _temp370; void* _temp372; void* _temp374; _LL332: _LL353:
_temp352= _temp330.f1; if(( unsigned int) _temp352 >  3u?*(( int*) _temp352) == 
Cyc_CfFlowInfo_MustPointTo: 0){ _LL355: _temp354=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp352)->f1; goto _LL349;} else{ goto _LL334;} _LL349: _temp348= _temp330.f2;
if(( unsigned int) _temp348 >  3u?*(( int*) _temp348) ==  Cyc_CfFlowInfo_MustPointTo:
0){ _LL351: _temp350=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp348)->f1;
goto _LL333;} else{ goto _LL334;} _LL334: _LL359: _temp358= _temp330.f1; if((
unsigned int) _temp358 >  3u?*(( int*) _temp358) ==  Cyc_CfFlowInfo_MustPointTo:
0){ _LL361: _temp360=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp358)->f1;
goto _LL357;} else{ goto _LL336;} _LL357: _temp356= _temp330.f2; goto _LL335;
_LL336: _LL367: _temp366= _temp330.f1; goto _LL363; _LL363: _temp362= _temp330.f2;
if(( unsigned int) _temp362 >  3u?*(( int*) _temp362) ==  Cyc_CfFlowInfo_MustPointTo:
0){ _LL365: _temp364=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp362)->f1;
goto _LL337;} else{ goto _LL338;} _LL338: _LL369: _temp368= _temp330.f1; if(
_temp368 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL339;} else{ goto _LL340;}
_LL340: _LL371: _temp370= _temp330.f2; if( _temp370 == ( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL341;} else{ goto _LL342;} _LL342: _LL373: _temp372= _temp330.f1; if(
_temp372 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL343;} else{ goto _LL344;}
_LL344: _LL375: _temp374= _temp330.f2; if( _temp374 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL345;} else{ goto _LL346;} _LL346: goto _LL347; _LL333: if( Cyc_CfFlowInfo_place_cmp(
_temp354, _temp350) ==  0){ return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp376=( struct Cyc_CfFlowInfo_LeafPI_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp376[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp377; _temp377.tag= Cyc_CfFlowInfo_LeafPI;
_temp377.f1= state1; _temp377;}); _temp376;});}{ struct Cyc_CfFlowInfo_AllInitEnv
_temp378=({ struct Cyc_CfFlowInfo_AllInitEnv _temp381; _temp381.top_info= env->d1;
_temp381.seen= 0; _temp381;}); struct Cyc_CfFlowInfo_AllInitEnv _temp379=({
struct Cyc_CfFlowInfo_AllInitEnv _temp380; _temp380.top_info= env->d2; _temp380.seen=
0; _temp380;}); if( Cyc_CfFlowInfo_isAllInit_pinfo(& _temp378, Cyc_CfFlowInfo_lookup_place(
env->d1, _temp354))? Cyc_CfFlowInfo_isAllInit_pinfo(& _temp379, Cyc_CfFlowInfo_lookup_place(
env->d2, _temp350)): 0){ Cyc_CfFlowInfo_add_place( env->rgn, env->places,
_temp354); Cyc_CfFlowInfo_add_place( env->rgn, env->places, _temp350); return
Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Unesc,( void*) Cyc_CfFlowInfo_AllIL);}
return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Unesc,( void*) Cyc_CfFlowInfo_ThisIL);}
_LL335: { struct Cyc_CfFlowInfo_AllInitEnv _temp382=({ struct Cyc_CfFlowInfo_AllInitEnv
_temp383; _temp383.top_info= env->d1; _temp383.seen= 0; _temp383;}); if(
_temp356 == ( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp382, Cyc_CfFlowInfo_lookup_place( env->d1, _temp360)): 0){ Cyc_CfFlowInfo_add_place(
env->rgn, env->places, _temp360); return Cyc_CfFlowInfo_mkLeafPI( state2.esc,(
void*) Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI( state2.esc,
_temp356 == ( void*) Cyc_CfFlowInfo_NoneIL?( void*) Cyc_CfFlowInfo_NoneIL:( void*)
Cyc_CfFlowInfo_ThisIL);} _LL337: { struct Cyc_CfFlowInfo_AllInitEnv _temp384=({
struct Cyc_CfFlowInfo_AllInitEnv _temp385; _temp385.top_info= env->d2; _temp385.seen=
0; _temp385;}); if( _temp366 == ( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp384, Cyc_CfFlowInfo_lookup_place( env->d2, _temp364)): 0){ Cyc_CfFlowInfo_add_place(
env->rgn, env->places, _temp364); return Cyc_CfFlowInfo_mkLeafPI( state1.esc,(
void*) Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI( state1.esc,
_temp366 == ( void*) Cyc_CfFlowInfo_NoneIL?( void*) Cyc_CfFlowInfo_NoneIL:( void*)
Cyc_CfFlowInfo_ThisIL);} _LL339: goto _LL341; _LL341: result_level=( void*) Cyc_CfFlowInfo_NoneIL;
goto _LL331; _LL343: goto _LL345; _LL345: result_level=( void*) Cyc_CfFlowInfo_ThisIL;
goto _LL331; _LL347: result_level=( void*) Cyc_CfFlowInfo_AllIL; goto _LL331;
_LL331:;} if( state1.esc ==  Cyc_CfFlowInfo_Unesc? state2.esc ==  Cyc_CfFlowInfo_Unesc:
0){ result_esc= Cyc_CfFlowInfo_Unesc;} else{ result_esc= Cyc_CfFlowInfo_Esc;}
return Cyc_CfFlowInfo_mkLeafPI( result_esc, result_level);} static void* Cyc_CfFlowInfo_join_pathinfo(
struct Cyc_CfFlowInfo_JoinEnv* env, void* ignore, void* pinfo1, void* pinfo2){
if( pinfo1 ==  pinfo2){ return pinfo1;}{ struct _tuple4 _temp387=({ struct
_tuple4 _temp386; _temp386.f1= pinfo1; _temp386.f2= pinfo2; _temp386;}); void*
_temp395; struct Cyc_CfFlowInfo_InitState _temp397; void* _temp399; struct Cyc_CfFlowInfo_InitState
_temp401; void* _temp403; struct Cyc_Dict_Dict* _temp405; void* _temp407; struct
Cyc_Dict_Dict* _temp409; _LL389: _LL400: _temp399= _temp387.f1; if(*(( int*)
_temp399) ==  Cyc_CfFlowInfo_LeafPI){ _LL402: _temp401=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp399)->f1; goto _LL396;} else{ goto _LL391;} _LL396: _temp395= _temp387.f2;
if(*(( int*) _temp395) ==  Cyc_CfFlowInfo_LeafPI){ _LL398: _temp397=(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp395)->f1; goto _LL390;} else{ goto _LL391;}
_LL391: _LL408: _temp407= _temp387.f1; if(*(( int*) _temp407) ==  Cyc_CfFlowInfo_DictPI){
_LL410: _temp409=(( struct Cyc_CfFlowInfo_DictPI_struct*) _temp407)->f1; goto
_LL404;} else{ goto _LL393;} _LL404: _temp403= _temp387.f2; if(*(( int*)
_temp403) ==  Cyc_CfFlowInfo_DictPI){ _LL406: _temp405=(( struct Cyc_CfFlowInfo_DictPI_struct*)
_temp403)->f1; goto _LL392;} else{ goto _LL393;} _LL393: goto _LL394; _LL390:
if( _temp401.esc ==  _temp397.esc?( void*) _temp401.level == ( void*) _temp397.level:
0){ return pinfo1;} return Cyc_CfFlowInfo_join_initstate( env, _temp401,
_temp397); _LL392: if( _temp409 ==  _temp405){ return pinfo1;} return( void*)({
struct Cyc_CfFlowInfo_DictPI_struct* _temp411=( struct Cyc_CfFlowInfo_DictPI_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct)); _temp411[ 0]=({ struct
Cyc_CfFlowInfo_DictPI_struct _temp412; _temp412.tag= Cyc_CfFlowInfo_DictPI;
_temp412.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
struct _tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(( void*(*)(
struct Cyc_CfFlowInfo_JoinEnv* env, struct _tagged_arr* ignore, void* pinfo1,
void* pinfo2)) Cyc_CfFlowInfo_join_pathinfo, env, _temp409, _temp405); _temp412;});
_temp411;}); _LL394:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp413=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp413[ 0]=({ struct Cyc_Core_Impossible_struct _temp414; _temp414.tag= Cyc_Core_Impossible;
_temp414.f1= _tag_arr("join_pathinfo", sizeof( unsigned char), 14u); _temp414;});
_temp413;})); _LL388:;}} static void* Cyc_CfFlowInfo_after_pathinfo_dict( struct
Cyc_CfFlowInfo_AfterEnv* env, struct _tagged_arr* key, void* pinfo1, void*
pinfo2); static void* Cyc_CfFlowInfo_after_pathinfo( struct Cyc_CfFlowInfo_AfterEnv*
env, void* pinfo1, void* pinfo2){ int _temp415= env->changed ==  Cyc_CfFlowInfo_One?
1:(( int(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_member)(
env->chg1, env->curr_place); int _temp416= env->changed ==  Cyc_CfFlowInfo_Two?
1:(( int(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_member)(
env->chg2, env->curr_place); if( _temp415? _temp416: 0){ return(( void*(*)(
struct Cyc_CfFlowInfo_JoinEnv* env, int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_join_pathinfo)(&
env->joinenv, 0, pinfo1, pinfo2);} if( _temp415){ if( ! Cyc_CfFlowInfo_prefix_of_member(
env->curr_place, env->chg2)){ return pinfo1;} env->changed= Cyc_CfFlowInfo_One;{
struct _tuple4 _temp418=({ struct _tuple4 _temp417; _temp417.f1= pinfo1;
_temp417.f2= pinfo2; _temp417;}); void* _temp424; struct Cyc_Dict_Dict* _temp426;
void* _temp428; struct Cyc_Dict_Dict* _temp430; _LL420: _LL429: _temp428=
_temp418.f1; if(*(( int*) _temp428) ==  Cyc_CfFlowInfo_DictPI){ _LL431: _temp430=((
struct Cyc_CfFlowInfo_DictPI_struct*) _temp428)->f1; goto _LL425;} else{ goto
_LL422;} _LL425: _temp424= _temp418.f2; if(*(( int*) _temp424) ==  Cyc_CfFlowInfo_DictPI){
_LL427: _temp426=(( struct Cyc_CfFlowInfo_DictPI_struct*) _temp424)->f1; goto
_LL421;} else{ goto _LL422;} _LL422: goto _LL423; _LL421: return( void*)({
struct Cyc_CfFlowInfo_DictPI_struct* _temp432=( struct Cyc_CfFlowInfo_DictPI_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct)); _temp432[ 0]=({ struct
Cyc_CfFlowInfo_DictPI_struct _temp433; _temp433.tag= Cyc_CfFlowInfo_DictPI;
_temp433.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_AfterEnv* env, struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_after_pathinfo_dict,
env, _temp430, _temp426); _temp433;}); _temp432;}); _LL423:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp434=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp434[ 0]=({ struct
Cyc_Core_Impossible_struct _temp435; _temp435.tag= Cyc_Core_Impossible; _temp435.f1=
_tag_arr("after_pathinfo 1", sizeof( unsigned char), 17u); _temp435;}); _temp434;}));
_LL419:;}} if( _temp416){ if( ! Cyc_CfFlowInfo_prefix_of_member( env->curr_place,
env->chg1)){ return pinfo2;} env->changed= Cyc_CfFlowInfo_Two;{ struct _tuple4
_temp437=({ struct _tuple4 _temp436; _temp436.f1= pinfo1; _temp436.f2= pinfo2;
_temp436;}); void* _temp443; struct Cyc_Dict_Dict* _temp445; void* _temp447;
struct Cyc_Dict_Dict* _temp449; _LL439: _LL448: _temp447= _temp437.f1; if(*((
int*) _temp447) ==  Cyc_CfFlowInfo_DictPI){ _LL450: _temp449=(( struct Cyc_CfFlowInfo_DictPI_struct*)
_temp447)->f1; goto _LL444;} else{ goto _LL441;} _LL444: _temp443= _temp437.f2;
if(*(( int*) _temp443) ==  Cyc_CfFlowInfo_DictPI){ _LL446: _temp445=(( struct
Cyc_CfFlowInfo_DictPI_struct*) _temp443)->f1; goto _LL440;} else{ goto _LL441;}
_LL441: goto _LL442; _LL440: return( void*)({ struct Cyc_CfFlowInfo_DictPI_struct*
_temp451=( struct Cyc_CfFlowInfo_DictPI_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct));
_temp451[ 0]=({ struct Cyc_CfFlowInfo_DictPI_struct _temp452; _temp452.tag= Cyc_CfFlowInfo_DictPI;
_temp452.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_AfterEnv* env, struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_after_pathinfo_dict,
env, _temp449, _temp445); _temp452;}); _temp451;}); _LL442:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp453=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp453[ 0]=({ struct
Cyc_Core_Impossible_struct _temp454; _temp454.tag= Cyc_Core_Impossible; _temp454.f1=
_tag_arr("after_pathinfo 2", sizeof( unsigned char), 17u); _temp454;}); _temp453;}));
_LL438:;}}{ struct _tuple4 _temp456=({ struct _tuple4 _temp455; _temp455.f1=
pinfo1; _temp455.f2= pinfo2; _temp455;}); void* _temp462; struct Cyc_Dict_Dict*
_temp464; void* _temp466; struct Cyc_Dict_Dict* _temp468; _LL458: _LL467:
_temp466= _temp456.f1; if(*(( int*) _temp466) ==  Cyc_CfFlowInfo_DictPI){ _LL469:
_temp468=(( struct Cyc_CfFlowInfo_DictPI_struct*) _temp466)->f1; goto _LL463;}
else{ goto _LL460;} _LL463: _temp462= _temp456.f2; if(*(( int*) _temp462) == 
Cyc_CfFlowInfo_DictPI){ _LL465: _temp464=(( struct Cyc_CfFlowInfo_DictPI_struct*)
_temp462)->f1; goto _LL459;} else{ goto _LL460;} _LL460: goto _LL461; _LL459:
return( void*)({ struct Cyc_CfFlowInfo_DictPI_struct* _temp470=( struct Cyc_CfFlowInfo_DictPI_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_DictPI_struct)); _temp470[ 0]=({ struct
Cyc_CfFlowInfo_DictPI_struct _temp471; _temp471.tag= Cyc_CfFlowInfo_DictPI;
_temp471.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_AfterEnv* env, struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_after_pathinfo_dict,
env, _temp468, _temp464); _temp471;}); _temp470;}); _LL461:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp472=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp472[ 0]=({ struct
Cyc_Core_Impossible_struct _temp473; _temp473.tag= Cyc_Core_Impossible; _temp473.f1=
_tag_arr("after_pathinfo -- nothing changed!", sizeof( unsigned char), 35u);
_temp473;}); _temp472;})); _LL457:;}} static void* Cyc_CfFlowInfo_after_pathinfo_dict(
struct Cyc_CfFlowInfo_AfterEnv* env, struct _tagged_arr* key, void* pinfo1, void*
pinfo2){ if( pinfo1 ==  pinfo2){ return pinfo1;}{ struct Cyc_List_List**
_temp474= env->last_field_cell; enum  Cyc_CfFlowInfo_WhoIsChanged _temp475= env->changed;*
env->last_field_cell=({ struct Cyc_List_List* _temp476=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp476->hd=( void*) key; _temp476->tl=
0; _temp476;}); env->last_field_cell=&(( struct Cyc_List_List*) _check_null(*
env->last_field_cell))->tl;{ void* _temp477= Cyc_CfFlowInfo_after_pathinfo( env,
pinfo1, pinfo2); env->last_field_cell= _temp474;(( struct Cyc_List_List*)
_check_null(* env->last_field_cell))->tl= 0; env->changed= _temp475; return
_temp477;}}} static void* Cyc_CfFlowInfo_after_pathinfo_root( struct Cyc_CfFlowInfo_AfterEnv*
env, void* root, void* pinfo1, void* pinfo2){ if( pinfo1 ==  pinfo2){ return
pinfo1;}* env->curr_place=({ struct Cyc_CfFlowInfo_Place _temp478; _temp478.root=(
void*) root; _temp478.fields= 0; _temp478;}); env->last_field_cell=&( env->curr_place)->fields;
env->changed= Cyc_CfFlowInfo_Neither; return Cyc_CfFlowInfo_after_pathinfo( env,
pinfo1, pinfo2);} void* Cyc_CfFlowInfo_join_flow( struct Cyc_Set_Set**
all_changed, void* f1, void* f2){ if( f1 ==  f2){ return f1;}{ struct _tuple4
_temp480=({ struct _tuple4 _temp479; _temp479.f1= f1; _temp479.f2= f2; _temp479;});
void* _temp488; void* _temp490; void* _temp492; struct Cyc_Dict_Dict* _temp494;
void* _temp496; struct Cyc_Dict_Dict* _temp498; _LL482: _LL489: _temp488=
_temp480.f1; if( _temp488 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL483;}
else{ goto _LL484;} _LL484: _LL491: _temp490= _temp480.f2; if( _temp490 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL485;} else{ goto _LL486;} _LL486:
_LL497: _temp496= _temp480.f1; if(( unsigned int) _temp496 >  1u?*(( int*)
_temp496) ==  Cyc_CfFlowInfo_InitsFL: 0){ _LL499: _temp498=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp496)->f1; goto _LL493;} else{ goto _LL481;} _LL493: _temp492= _temp480.f2;
if(( unsigned int) _temp492 >  1u?*(( int*) _temp492) ==  Cyc_CfFlowInfo_InitsFL:
0){ _LL495: _temp494=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp492)->f1;
goto _LL487;} else{ goto _LL481;} _LL483: return f2; _LL485: return f1; _LL487:
if( _temp498 ==  _temp494){ return f1;} if( Cyc_CfFlowInfo_flow_lessthan_approx(
f1, f2)){ return f2;} if( Cyc_CfFlowInfo_flow_lessthan_approx( f2, f1)){ return
f1;}{ struct _RegionHandle _temp500= _new_region(); struct _RegionHandle* r=&
_temp500; _push_region( r);{ struct Cyc_List_List* _temp501= 0; struct Cyc_CfFlowInfo_JoinEnv
_temp502=({ struct Cyc_CfFlowInfo_JoinEnv _temp508; _temp508.d1= _temp498;
_temp508.d2= _temp494; _temp508.places=& _temp501; _temp508.rgn= r; _temp508;});
struct Cyc_Dict_Dict* _temp503=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct
Cyc_CfFlowInfo_JoinEnv*, void*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo,& _temp502, _temp498, _temp494); struct Cyc_Dict_Dict*
_temp504= Cyc_CfFlowInfo_escape_these( r, _temp501, _temp503, all_changed); void*
_temp507=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp505=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp505[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp506; _temp506.tag= Cyc_CfFlowInfo_InitsFL;
_temp506.f1= _temp504; _temp506;}); _temp505;}); _npop_handler( 0u); return
_temp507;}; _pop_region( r);} _LL481:;}} static struct Cyc_Absyn_Const_e_struct
Cyc_CfFlowInfo_dummy_rawexp={ 0u,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_CfFlowInfo_dummy_exp={ 0,( void*)(( void*)& Cyc_CfFlowInfo_dummy_rawexp), 0};
static struct Cyc_CfFlowInfo_MallocPt_struct Cyc_CfFlowInfo_dummy_root={ 1u,&
Cyc_CfFlowInfo_dummy_exp}; void* Cyc_CfFlowInfo_after_flow( struct Cyc_Set_Set**
all_changed, void* f1, void* f2, struct Cyc_Set_Set* chg1, struct Cyc_Set_Set*
chg2){ if( f1 ==  f2){ return f1;}{ struct _tuple4 _temp512=({ struct _tuple4
_temp511; _temp511.f1= f1; _temp511.f2= f2; _temp511;}); void* _temp520; void*
_temp522; void* _temp524; struct Cyc_Dict_Dict* _temp526; void* _temp528; struct
Cyc_Dict_Dict* _temp530; _LL514: _LL521: _temp520= _temp512.f1; if( _temp520 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL515;} else{ goto _LL516;} _LL516:
_LL523: _temp522= _temp512.f2; if( _temp522 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL517;} else{ goto _LL518;} _LL518: _LL529: _temp528= _temp512.f1; if((
unsigned int) _temp528 >  1u?*(( int*) _temp528) ==  Cyc_CfFlowInfo_InitsFL: 0){
_LL531: _temp530=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp528)->f1; goto
_LL525;} else{ goto _LL513;} _LL525: _temp524= _temp512.f2; if(( unsigned int)
_temp524 >  1u?*(( int*) _temp524) ==  Cyc_CfFlowInfo_InitsFL: 0){ _LL527:
_temp526=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp524)->f1; goto _LL519;}
else{ goto _LL513;} _LL515: goto _LL517; _LL517: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL519: if( _temp530 ==  _temp526){ return f1;}{ struct _RegionHandle _temp532=
_new_region(); struct _RegionHandle* r2=& _temp532; _push_region( r2);{ struct
Cyc_List_List* _temp533= 0; struct Cyc_CfFlowInfo_Place* _temp534=({ struct Cyc_CfFlowInfo_Place*
_temp543=( struct Cyc_CfFlowInfo_Place*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp543->root=( void*)(( void*)& Cyc_CfFlowInfo_dummy_root); _temp543->fields=
0; _temp543;}); struct Cyc_CfFlowInfo_AfterEnv _temp535=({ struct Cyc_CfFlowInfo_AfterEnv
_temp541; _temp541.joinenv=({ struct Cyc_CfFlowInfo_JoinEnv _temp542; _temp542.d1=
_temp530; _temp542.d2= _temp526; _temp542.places=& _temp533; _temp542.rgn= r2;
_temp542;}); _temp541.chg1= chg1; _temp541.chg2= chg2; _temp541.curr_place=
_temp534; _temp541.changed= Cyc_CfFlowInfo_Neither; _temp541.last_field_cell=&
_temp534->fields; _temp541;}); struct Cyc_Dict_Dict* _temp536=(( struct Cyc_Dict_Dict*(*)(
void*(* f)( struct Cyc_CfFlowInfo_AfterEnv*, void*, void*, void*), struct Cyc_CfFlowInfo_AfterEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_union_two_c)(
Cyc_CfFlowInfo_after_pathinfo_root,& _temp535, _temp530, _temp526); struct Cyc_Dict_Dict*
_temp537= Cyc_CfFlowInfo_escape_these( r2, _temp533, _temp536, all_changed);
void* _temp540=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp538=(
struct Cyc_CfFlowInfo_InitsFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp538[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp539; _temp539.tag= Cyc_CfFlowInfo_InitsFL;
_temp539.f1= _temp537; _temp539;}); _temp538;}); _npop_handler( 0u); return
_temp540;}; _pop_region( r2);} _LL513:;}} static int Cyc_CfFlowInfo_initstate_lessthan_approx(
struct Cyc_CfFlowInfo_InitState state1, struct Cyc_CfFlowInfo_InitState state2){
if( state1.esc ==  Cyc_CfFlowInfo_Esc? state2.esc ==  Cyc_CfFlowInfo_Unesc: 0){
return 0;} if(( void*) state1.level == ( void*) state2.level){ return 1;}{
struct _tuple4 _temp545=({ struct _tuple4 _temp544; _temp544.f1=( void*) state1.level;
_temp544.f2=( void*) state2.level; _temp544;}); void* _temp559; struct Cyc_CfFlowInfo_Place*
_temp561; void* _temp563; struct Cyc_CfFlowInfo_Place* _temp565; void* _temp567;
void* _temp569; void* _temp571; void* _temp573; void* _temp575; _LL547: _LL564:
_temp563= _temp545.f1; if(( unsigned int) _temp563 >  3u?*(( int*) _temp563) == 
Cyc_CfFlowInfo_MustPointTo: 0){ _LL566: _temp565=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp563)->f1; goto _LL560;} else{ goto _LL549;} _LL560: _temp559= _temp545.f2;
if(( unsigned int) _temp559 >  3u?*(( int*) _temp559) ==  Cyc_CfFlowInfo_MustPointTo:
0){ _LL562: _temp561=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp559)->f1;
goto _LL548;} else{ goto _LL549;} _LL549: _LL568: _temp567= _temp545.f1; if((
unsigned int) _temp567 >  3u?*(( int*) _temp567) ==  Cyc_CfFlowInfo_MustPointTo:
0){ goto _LL550;} else{ goto _LL551;} _LL551: _LL570: _temp569= _temp545.f2; if((
unsigned int) _temp569 >  3u?*(( int*) _temp569) ==  Cyc_CfFlowInfo_MustPointTo:
0){ goto _LL552;} else{ goto _LL553;} _LL553: _LL572: _temp571= _temp545.f1; if(
_temp571 == ( void*) Cyc_CfFlowInfo_AllIL){ goto _LL554;} else{ goto _LL555;}
_LL555: _LL576: _temp575= _temp545.f1; if( _temp575 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL574;} else{ goto _LL557;} _LL574: _temp573= _temp545.f2; if( _temp573 == (
void*) Cyc_CfFlowInfo_NoneIL){ goto _LL556;} else{ goto _LL557;} _LL557: goto
_LL558; _LL548: return Cyc_CfFlowInfo_place_cmp( _temp565, _temp561) ==  0;
_LL550: goto _LL552; _LL552: return 0; _LL554: goto _LL556; _LL556: return 1;
_LL558: return 0; _LL546:;}} static int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 ==  pinfo2){ return 1;}{
struct _tuple4 _temp578=({ struct _tuple4 _temp577; _temp577.f1= pinfo1;
_temp577.f2= pinfo2; _temp577;}); void* _temp586; struct Cyc_CfFlowInfo_InitState
_temp588; void* _temp590; struct Cyc_CfFlowInfo_InitState _temp592; void*
_temp594; struct Cyc_Dict_Dict* _temp596; void* _temp598; struct Cyc_Dict_Dict*
_temp600; _LL580: _LL591: _temp590= _temp578.f1; if(*(( int*) _temp590) ==  Cyc_CfFlowInfo_LeafPI){
_LL593: _temp592=(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp590)->f1; goto
_LL587;} else{ goto _LL582;} _LL587: _temp586= _temp578.f2; if(*(( int*)
_temp586) ==  Cyc_CfFlowInfo_LeafPI){ _LL589: _temp588=(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp586)->f1; goto _LL581;} else{ goto _LL582;} _LL582: _LL599: _temp598=
_temp578.f1; if(*(( int*) _temp598) ==  Cyc_CfFlowInfo_DictPI){ _LL601: _temp600=((
struct Cyc_CfFlowInfo_DictPI_struct*) _temp598)->f1; goto _LL595;} else{ goto
_LL584;} _LL595: _temp594= _temp578.f2; if(*(( int*) _temp594) ==  Cyc_CfFlowInfo_DictPI){
_LL597: _temp596=(( struct Cyc_CfFlowInfo_DictPI_struct*) _temp594)->f1; goto
_LL583;} else{ goto _LL584;} _LL584: goto _LL585; _LL581: return Cyc_CfFlowInfo_initstate_lessthan_approx(
_temp592, _temp588); _LL583: return _temp600 ==  _temp596? 1:(( int(*)( int(* f)(
struct _tagged_arr*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_arr* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp600, _temp596);
_LL585:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp602=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp602[ 0]=({ struct Cyc_Core_Impossible_struct _temp603; _temp603.tag= Cyc_Core_Impossible;
_temp603.f1= _tag_arr("pinfo_lessthan", sizeof( unsigned char), 15u); _temp603;});
_temp602;})); _LL579:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2){ if( f1 ==  f2){ return 1;}{ struct _tuple4 _temp605=({ struct _tuple4
_temp604; _temp604.f1= f1; _temp604.f2= f2; _temp604;}); void* _temp613; void*
_temp615; void* _temp617; struct Cyc_Dict_Dict* _temp619; void* _temp621; struct
Cyc_Dict_Dict* _temp623; _LL607: _LL614: _temp613= _temp605.f1; if( _temp613 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL608;} else{ goto _LL609;} _LL609:
_LL616: _temp615= _temp605.f2; if( _temp615 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL610;} else{ goto _LL611;} _LL611: _LL622: _temp621= _temp605.f1; if((
unsigned int) _temp621 >  1u?*(( int*) _temp621) ==  Cyc_CfFlowInfo_InitsFL: 0){
_LL624: _temp623=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp621)->f1; goto
_LL618;} else{ goto _LL606;} _LL618: _temp617= _temp605.f2; if(( unsigned int)
_temp617 >  1u?*(( int*) _temp617) ==  Cyc_CfFlowInfo_InitsFL: 0){ _LL620:
_temp619=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp617)->f1; goto _LL612;}
else{ goto _LL606;} _LL608: return 1; _LL610: return 0; _LL612: if( _temp623 == 
_temp619){ return 1;} return Cyc_Dict_forall_intersect( Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp623, _temp619); _LL606:;}}
