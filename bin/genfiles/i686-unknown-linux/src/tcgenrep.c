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
void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list(
struct _tagged_arr); extern int Cyc_List_length( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void*
env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern void* Cyc_List_fold_left_c( void*(* f)( void*, void*, void*), void*, void*
accum, struct Cyc_List_List* x); extern void* Cyc_List_fold_right_c( void*(* f)(
void*, void*, void*), void*, struct Cyc_List_List* x, void* accum); extern
struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x); extern
unsigned char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_tabulate( int n, void*(* f)( int)); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k, void* v);
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k); extern struct
Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* k); extern
void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict* d); extern
int Cyc_Std_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); struct Cyc_Hashtable_Table;
extern struct Cyc_Hashtable_Table* Cyc_Hashtable_create( int sz, int(* cmp)(
void*, void*), int(* hash)( void*)); extern void Cyc_Hashtable_insert( struct
Cyc_Hashtable_Table* t, void* key, void* val); extern void* Cyc_Hashtable_lookup(
struct Cyc_Hashtable_Table* t, void* key); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static
const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static
const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; }
; extern unsigned char Cyc_Position_Nocontext[ 14u]; static const int Cyc_Typerep_Var=
0; struct Cyc_Typerep_Var_struct{ int tag; struct _tagged_arr* f1; } ; static
const int Cyc_Typerep_Int= 1; struct Cyc_Typerep_Int_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Typerep_Float= 0; static const int Cyc_Typerep_Double=
1; static const int Cyc_Typerep_ThinPtr= 2; struct Cyc_Typerep_ThinPtr_struct{
int tag; unsigned int f1; void* f2; } ; static const int Cyc_Typerep_FatPtr= 3;
struct Cyc_Typerep_FatPtr_struct{ int tag; void* f1; } ; static const int Cyc_Typerep_Tuple=
4; struct _tuple0{ unsigned int f1; void* f2; } ; struct Cyc_Typerep_Tuple_struct{
int tag; unsigned int f1; struct _tagged_arr f2; } ; static const int Cyc_Typerep_TUnion=
5; struct Cyc_Typerep_TUnion_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Typerep_Union= 6; struct Cyc_Typerep_Union_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Typerep_Forall= 7; struct Cyc_Typerep_Forall_struct{
int tag; struct _tagged_arr f1; void** f2; } ; static const int Cyc_Typerep_App=
8; struct Cyc_Typerep_App_struct{ int tag; void* f1; struct _tagged_arr f2; } ;
struct _tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo;
struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp;
struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_SwitchC_clause; struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl;
struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield;
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract=
1; static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
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
struct _tuple1* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
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
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Gentyp_e=
19; struct Cyc_Absyn_Gentyp_e_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_Deref_e= 20; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_StructMember_e= 21; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 22; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
23; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 24; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
25; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 26; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
27; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
28; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 29; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 30;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_at_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple1*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp( void* t, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x,
void* t, struct Cyc_Absyn_Exp* init); struct Cyc_Std___sFILE; extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_printf( struct _tagged_arr fmt, struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt, struct
_tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;
extern struct _tagged_arr Cyc_Absynpp_typ2string( void*); extern struct
_tagged_arr Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
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
ae; struct Cyc_Core_Opt* le; } ; extern struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_typ_kind(
void* t); extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); struct Cyc_Tcgenrep_RepInfo{ struct Cyc_List_List*
decls; struct Cyc_Absyn_Exp* exp; struct Cyc_List_List* dependencies; struct Cyc_Core_Opt*
fwd_decl; int emitted; } ; static int Cyc_Tcgenrep_typecmp( void* t1, void* t2){
Cyc_Absynpp_set_params(& Cyc_Absynpp_c_params_r);{ struct _tagged_arr t1str= Cyc_Absynpp_typ2string(
t1); struct _tagged_arr t2str= Cyc_Absynpp_typ2string( t2); return Cyc_Std_strcmp(
t1str, t2str);}} struct Cyc_Dict_Dict* Cyc_Tcgenrep_empty_typerep_dict(){ return((
struct Cyc_Dict_Dict*(*)( int(* cmp)( void*, void*))) Cyc_Dict_empty)( Cyc_Tcgenrep_typecmp);}
void Cyc_Tcgenrep_print_dict_entry( void* type, struct Cyc_Tcgenrep_RepInfo*
info){({ struct Cyc_Std_Int_pa_struct _temp2; _temp2.tag= Cyc_Std_Int_pa; _temp2.f1=(
int)(( unsigned int) info->emitted);{ struct Cyc_Std_String_pa_struct _temp1;
_temp1.tag= Cyc_Std_String_pa; _temp1.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
type);{ void* _temp0[ 2u]={& _temp1,& _temp2}; Cyc_Std_printf( _tag_arr("(%s,%d)\n",
sizeof( unsigned char), 9u), _tag_arr( _temp0, sizeof( void*), 2u));}}});} void
Cyc_Tcgenrep_print_typerep_dict( struct Cyc_Dict_Dict* dict){(( void(*)( void(*
f)( void*, struct Cyc_Tcgenrep_RepInfo*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)(
Cyc_Tcgenrep_print_dict_entry, dict);} static int Cyc_Tcgenrep_rephash( struct
Cyc_Tcgenrep_RepInfo* ri){ return( int) ri;} static int Cyc_Tcgenrep_repcmp(
struct Cyc_Tcgenrep_RepInfo* r1, struct Cyc_Tcgenrep_RepInfo* r2){ unsigned int
r1p; unsigned int r2p; r1p=( unsigned int) r1; r2p=( unsigned int) r2; if( r1 < 
r2){ return - 1;} else{ if( r1 ==  r2){ return 0;} else{ return 1;}}} static
struct Cyc_Absyn_Tunionfield* Cyc_Tcgenrep_getField( struct Cyc_Absyn_Tuniondecl*
td, struct _tuple1* fieldname){ if( td->fields ==  0){({ void* _temp3[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("Could not find field in tuniondecl", sizeof( unsigned char), 35u),
_tag_arr( _temp3, sizeof( void*), 0u));});} else{ struct Cyc_List_List* l=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( td->fields))->v;
for( 0; l !=  0; l=(( struct Cyc_List_List*) _check_null( l))->tl){ if( ! Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( l))->hd)->name,
fieldname)){ return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( l))->hd;}}}({ void* _temp4[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("Could not find field in tuniondecl",
sizeof( unsigned char), 35u), _tag_arr( _temp4, sizeof( void*), 0u));});( int)
_throw(( void*) Cyc_Tcutil_TypeErr);} static unsigned char _temp5[ 9u]="Tcgenrep";
static struct _tagged_arr Cyc_Tcgenrep_tcgenrep_nm={ _temp5, _temp5, _temp5 +  9u};
static struct Cyc_List_List Cyc_Tcgenrep_l={( void*)& Cyc_Tcgenrep_tcgenrep_nm,
0}; static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_tcgenrep_nmspace={ 1u,(
struct Cyc_List_List*)& Cyc_Tcgenrep_l}; static void* Cyc_Tcgenrep_tcgenrep_nmspace2=(
void*)& Cyc_Tcgenrep_tcgenrep_nmspace; static unsigned char _temp7[ 8u]="Typerep";
static struct _tagged_arr Cyc_Tcgenrep_typerep_nm={ _temp7, _temp7, _temp7 +  8u};
static struct Cyc_List_List Cyc_Tcgenrep_l2={( void*)& Cyc_Tcgenrep_typerep_nm,
0}; static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_typerep_nmspace={ 1u,(
struct Cyc_List_List*)& Cyc_Tcgenrep_l2}; static struct Cyc_List_List* Cyc_Tcgenrep_dfsvisit(
struct Cyc_Tcgenrep_RepInfo* ri, struct Cyc_Hashtable_Table* visited){ struct
_handler_cons _temp9; _push_handler(& _temp9);{ int _temp11= 0; if( setjmp(
_temp9.handler)){ _temp11= 1;} if( ! _temp11){(( int(*)( struct Cyc_Hashtable_Table*
t, struct Cyc_Tcgenrep_RepInfo* key)) Cyc_Hashtable_lookup)( visited, ri);{
struct Cyc_List_List* _temp12= 0; _npop_handler( 0u); return _temp12;};
_pop_handler();} else{ void* _temp10=( void*) _exn_thrown; void* _temp14=
_temp10; _LL16: if( _temp14 ==  Cyc_Core_Not_found){ goto _LL17;} else{ goto
_LL18;} _LL18: goto _LL19; _LL17:(( void(*)( struct Cyc_Hashtable_Table* t,
struct Cyc_Tcgenrep_RepInfo* key, int val)) Cyc_Hashtable_insert)( visited, ri,
1);{ struct Cyc_List_List* ds= 0;{ struct Cyc_List_List* l= ri->dependencies;
for( 0; l !=  0; l=(( struct Cyc_List_List*) _check_null( l))->tl){ ds=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
ds, Cyc_Tcgenrep_dfsvisit(( struct Cyc_Tcgenrep_RepInfo*)(( struct Cyc_List_List*)
_check_null( l))->hd, visited));}} if( ri->emitted ==  0){ if( ri->fwd_decl != 
0){ ds=({ struct Cyc_List_List* _temp20=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp20->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp21[ 1u]; _temp21[ 0u]=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( ri->fwd_decl))->v;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp21, sizeof( struct Cyc_Absyn_Decl*), 1u));});
_temp20->tl= ds; _temp20;});}{ struct Cyc_List_List* _temp22=({ struct Cyc_List_List*
_temp23=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp23->hd=( void*) ri->decls; _temp23->tl= 0; _temp23;}); ri->emitted= 1;
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( ds, _temp22);}} else{ return ds;}} _LL19:( void)
_throw( _temp14); _LL15:;}}} static struct Cyc_List_List* Cyc_Tcgenrep_dfs(
struct Cyc_Tcgenrep_RepInfo* ri){ struct Cyc_Hashtable_Table* tab=(( struct Cyc_Hashtable_Table*(*)(
int sz, int(* cmp)( struct Cyc_Tcgenrep_RepInfo*, struct Cyc_Tcgenrep_RepInfo*),
int(* hash)( struct Cyc_Tcgenrep_RepInfo*))) Cyc_Hashtable_create)( 53, Cyc_Tcgenrep_repcmp,
Cyc_Tcgenrep_rephash); struct Cyc_List_List* ds= Cyc_Tcgenrep_dfsvisit( ri, tab);
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(
ds);} static unsigned char _temp24[ 11u]="Typestruct"; static struct _tagged_arr
Cyc_Tcgenrep_typestruct_str={ _temp24, _temp24, _temp24 +  11u}; static
unsigned char _temp25[ 4u]="Var"; static struct _tagged_arr Cyc_Tcgenrep_var_str={
_temp25, _temp25, _temp25 +  4u}; static unsigned char _temp26[ 4u]="Int";
static struct _tagged_arr Cyc_Tcgenrep_int_str={ _temp26, _temp26, _temp26 +  4u};
static unsigned char _temp27[ 6u]="Float"; static struct _tagged_arr Cyc_Tcgenrep_float_str={
_temp27, _temp27, _temp27 +  6u}; static unsigned char _temp28[ 7u]="Double";
static struct _tagged_arr Cyc_Tcgenrep_double_str={ _temp28, _temp28, _temp28 + 
7u}; static unsigned char _temp29[ 8u]="ThinPtr"; static struct _tagged_arr Cyc_Tcgenrep_thinptr_str={
_temp29, _temp29, _temp29 +  8u}; static unsigned char _temp30[ 7u]="FatPtr";
static struct _tagged_arr Cyc_Tcgenrep_fatptr_str={ _temp30, _temp30, _temp30 + 
7u}; static unsigned char _temp31[ 6u]="Tuple"; static struct _tagged_arr Cyc_Tcgenrep_tuple_str={
_temp31, _temp31, _temp31 +  6u}; static unsigned char _temp32[ 7u]="TUnion";
static struct _tagged_arr Cyc_Tcgenrep_tunion_str={ _temp32, _temp32, _temp32 + 
7u}; static unsigned char _temp33[ 6u]="Union"; static struct _tagged_arr Cyc_Tcgenrep_union_str={
_temp33, _temp33, _temp33 +  6u}; static struct _tuple1 Cyc_Tcgenrep_typestruct_name={(
void*)& Cyc_Tcgenrep_typerep_nmspace,& Cyc_Tcgenrep_typestruct_str}; static
struct Cyc_Absyn_Tuniondecl** Cyc_Tcgenrep_typestruct_decl= 0; static int Cyc_Tcgenrep_gen_id_counter=
0; static struct _tagged_arr* Cyc_Tcgenrep_new_gen_id( struct _tagged_arr name){
return({ struct _tagged_arr* _temp34=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp34[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp37; _temp37.tag= Cyc_Std_Int_pa; _temp37.f1=( int)(( unsigned int) Cyc_Tcgenrep_gen_id_counter
++);{ struct Cyc_Std_String_pa_struct _temp36; _temp36.tag= Cyc_Std_String_pa;
_temp36.f1=( struct _tagged_arr) name;{ void* _temp35[ 2u]={& _temp36,& _temp37};
Cyc_Std_aprintf( _tag_arr("_gen%s_%d", sizeof( unsigned char), 10u), _tag_arr(
_temp35, sizeof( void*), 2u));}}}); _temp34;});} static void Cyc_Tcgenrep_print_params(
struct Cyc_List_List* l){({ void* _temp38[ 0u]={}; Cyc_Std_printf( _tag_arr("<",
sizeof( unsigned char), 2u), _tag_arr( _temp38, sizeof( void*), 0u));});{ struct
Cyc_List_List* p= l; for( 0; p !=  0; p=(( struct Cyc_List_List*) _check_null( p))->tl){({
struct Cyc_Std_Int_pa_struct _temp41; _temp41.tag= Cyc_Std_Int_pa; _temp41.f1=(
int)(( unsigned int)(( int)((( struct Cyc_List_List*) _check_null( p))->tl !=  0?',':' ')));{
struct Cyc_Std_String_pa_struct _temp40; _temp40.tag= Cyc_Std_String_pa; _temp40.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*)
_check_null( p))->hd);{ void* _temp39[ 2u]={& _temp40,& _temp41}; Cyc_Std_printf(
_tag_arr("%s%c", sizeof( unsigned char), 5u), _tag_arr( _temp39, sizeof( void*),
2u));}}});}}({ void* _temp42[ 0u]={}; Cyc_Std_printf( _tag_arr(">\n", sizeof(
unsigned char), 3u), _tag_arr( _temp42, sizeof( void*), 0u));});} static void
Cyc_Tcgenrep_print_tvars( struct Cyc_List_List* l){({ void* _temp43[ 0u]={}; Cyc_Std_printf(
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr( _temp43, sizeof( void*), 0u));});{
struct Cyc_List_List* p= l; for( 0; p !=  0; p=(( struct Cyc_List_List*)
_check_null( p))->tl){({ struct Cyc_Std_Int_pa_struct _temp46; _temp46.tag= Cyc_Std_Int_pa;
_temp46.f1=( int)(( unsigned int)(( int)((( struct Cyc_List_List*) _check_null(
p))->tl !=  0?',':' ')));{ struct Cyc_Std_String_pa_struct _temp45; _temp45.tag=
Cyc_Std_String_pa; _temp45.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( p))->hd)->name;{ void* _temp44[ 2u]={&
_temp45,& _temp46}; Cyc_Std_printf( _tag_arr("%s%c", sizeof( unsigned char), 5u),
_tag_arr( _temp44, sizeof( void*), 2u));}}});}}({ void* _temp47[ 0u]={}; Cyc_Std_printf(
_tag_arr(">\n", sizeof( unsigned char), 3u), _tag_arr( _temp47, sizeof( void*),
0u));});} static struct Cyc_Absyn_Rel_n_struct Cyc_Tcgenrep_top_nm_0={ 0u, 0};
static void* Cyc_Tcgenrep_top_nm=( void*)& Cyc_Tcgenrep_top_nm_0; static struct
_tuple1* Cyc_Tcgenrep_toplevel_name( struct _tagged_arr* name){ return({ struct
_tuple1* _temp49=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp49->f1=
Cyc_Tcgenrep_top_nm; _temp49->f2=( struct _tagged_arr*) name; _temp49;});}
static struct _tuple1* Cyc_Tcgenrep_relative_name( struct Cyc_List_List* pathl,
struct _tagged_arr* name){ struct Cyc_Absyn_Rel_n_struct* path=({ struct Cyc_Absyn_Rel_n_struct*
_temp51=( struct Cyc_Absyn_Rel_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp51[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp52; _temp52.tag= Cyc_Absyn_Rel_n;
_temp52.f1= pathl; _temp52;}); _temp51;}); return({ struct _tuple1* _temp50=(
struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp50->f1=( void*) path;
_temp50->f2=( struct _tagged_arr*) name; _temp50;});} static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_gen_decl( struct _tagged_arr* name, void* type, struct Cyc_Absyn_Exp*
init, struct Cyc_Position_Segment* seg){ struct _tuple1* qvar= Cyc_Tcgenrep_toplevel_name(
name); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( qvar, type, init);(
void*)( vd->sc=( void*)(( void*) Cyc_Absyn_Static));{ struct Cyc_Absyn_Var_d_struct*
r1=({ struct Cyc_Absyn_Var_d_struct* _temp53=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp53[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp54; _temp54.tag= Cyc_Absyn_Var_d; _temp54.f1= vd; _temp54;}); _temp53;});
void* r2=( void*) r1; return Cyc_Absyn_new_decl( r2, seg);}} static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_gen_vardecl_sc( struct _tagged_arr* name, void* type, struct Cyc_Absyn_Exp*
init, void* sc, struct Cyc_Position_Segment* seg){ struct _tuple1* qvar= Cyc_Tcgenrep_toplevel_name(
name); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( qvar, type, init);(
void*)( vd->sc=( void*) sc);{ struct Cyc_Absyn_Var_d_struct* r1=({ struct Cyc_Absyn_Var_d_struct*
_temp55=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp55[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp56; _temp56.tag= Cyc_Absyn_Var_d;
_temp56.f1= vd; _temp56;}); _temp55;}); void* r2=( void*) r1; return Cyc_Absyn_new_decl(
r2, seg);}} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_cnst_string( struct
_tagged_arr s, struct Cyc_Position_Segment* seg){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_String_c_struct* _temp57=( struct Cyc_Absyn_String_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp57[ 0]=({ struct Cyc_Absyn_String_c_struct
_temp58; _temp58.tag= Cyc_Absyn_String_c; _temp58.f1= s; _temp58;}); _temp57;}),
seg);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_cnst_int( int i, struct Cyc_Position_Segment*
seg){ return Cyc_Absyn_const_exp(( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp59=( struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp59[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp60; _temp60.tag= Cyc_Absyn_Int_c;
_temp60.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp60.f2= i; _temp60;});
_temp59;}), seg);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_cnst_int_cls(
struct Cyc_Position_Segment* seg, int i){ return Cyc_Absyn_const_exp(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp61=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp61[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp62; _temp62.tag= Cyc_Absyn_Int_c; _temp62.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp62.f2= i; _temp62;}); _temp61;}), seg);} static int Cyc_Tcgenrep_size_of2int(
void* sz){ void* _temp63= sz; _LL65: if( _temp63 == ( void*) Cyc_Absyn_B1){ goto
_LL66;} else{ goto _LL67;} _LL67: if( _temp63 == ( void*) Cyc_Absyn_B2){ goto
_LL68;} else{ goto _LL69;} _LL69: if( _temp63 == ( void*) Cyc_Absyn_B4){ goto
_LL70;} else{ goto _LL71;} _LL71: if( _temp63 == ( void*) Cyc_Absyn_B8){ goto
_LL72;} else{ goto _LL64;} _LL66: return 8; _LL68: return 16; _LL70: return 32;
_LL72: return 64; _LL64:;} static void* Cyc_Tcgenrep_array_typ( void* ty, struct
Cyc_Absyn_Tqual tq, struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp73=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp73[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp74; _temp74.tag= Cyc_Absyn_ArrayType;
_temp74.f1=( void*) ty; _temp74.f2= tq; _temp74.f3= e; _temp74;}); _temp73;});}
static void* Cyc_Tcgenrep_tunion_typ( struct _tagged_arr* name){ return( void*)({
struct Cyc_Absyn_TunionType_struct* _temp75=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp75[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp76; _temp76.tag= Cyc_Absyn_TunionType; _temp76.f1=({
struct Cyc_Absyn_TunionInfo _temp77; _temp77.tunion_info=( void*)(( void*)({
struct Cyc_Absyn_UnknownTunion_struct* _temp78=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp78[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp79; _temp79.tag= Cyc_Absyn_UnknownTunion;
_temp79.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp80; _temp80.name= Cyc_Tcgenrep_toplevel_name(
name); _temp80.is_xtunion= 0; _temp80;}); _temp79;}); _temp78;})); _temp77.targs=
0; _temp77.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp77;}); _temp76;});
_temp75;});} static void* Cyc_Tcgenrep_tunionfield_typ( struct _tagged_arr* name,
struct _tagged_arr* fieldname){ return( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp81=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp81[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp82; _temp82.tag= Cyc_Absyn_TunionFieldType; _temp82.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp83; _temp83.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp84=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunionfield_struct)); _temp84[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp85; _temp85.tag= Cyc_Absyn_UnknownTunionfield; _temp85.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp86; _temp86.tunion_name= Cyc_Tcgenrep_toplevel_name( name); _temp86.field_name=
Cyc_Tcgenrep_toplevel_name( fieldname); _temp86.is_xtunion= 0; _temp86;});
_temp85;}); _temp84;})); _temp83.targs= 0; _temp83;}); _temp82;}); _temp81;});}
static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_call_exp( struct _tuple1* name, struct
Cyc_List_List* args, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_unknowncall_exp(
Cyc_Absyn_unknownid_exp( name, loc), args, loc);} static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_tunion_constructor_decl( struct _tagged_arr* tunionname, struct
_tagged_arr* fieldname, struct _tagged_arr* varname, struct Cyc_List_List* args,
struct Cyc_Position_Segment* loc){ void* _temp87= Cyc_Tcgenrep_tunionfield_typ(
tunionname, fieldname); struct Cyc_Absyn_Exp* _temp88= Cyc_Tcgenrep_call_exp(
Cyc_Tcgenrep_toplevel_name( fieldname), args, loc); struct Cyc_Absyn_Decl*
_temp89= Cyc_Tcgenrep_gen_decl( varname, _temp87,( struct Cyc_Absyn_Exp*)
_temp88, loc); return _temp89;} struct _tuple4{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; static void* Cyc_Tcgenrep_get_second( struct _tuple4* pair){ return(*
pair).f2;} struct _tuple5{ struct _tagged_arr* f1; struct Cyc_Absyn_Exp* f2; } ;
static struct _tagged_arr* Cyc_Tcgenrep_get_first( struct _tuple5* pair){ return(*
pair).f1;} static unsigned char _temp90[ 5u]="list"; static struct _tagged_arr
Cyc_Tcgenrep_list_str={ _temp90, _temp90, _temp90 +  5u}; static unsigned char
_temp91[ 5u]="List"; static struct _tagged_arr Cyc_Tcgenrep_List_str={ _temp91,
_temp91, _temp91 +  5u}; struct _tuple6{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_tuple2_exp_cls( struct Cyc_Position_Segment*
loc, struct _tuple6* es){ struct _tuple6 _temp94; struct Cyc_Absyn_Exp* _temp95;
struct Cyc_Absyn_Exp* _temp97; struct _tuple6* _temp92= es; _temp94=* _temp92;
_LL98: _temp97= _temp94.f1; goto _LL96; _LL96: _temp95= _temp94.f2; goto _LL93;
_LL93: return Cyc_Absyn_tuple_exp(({ struct Cyc_Absyn_Exp* _temp99[ 2u]; _temp99[
1u]= _temp95; _temp99[ 0u]= _temp97;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp99, sizeof( struct Cyc_Absyn_Exp*),
2u));}), loc);} static unsigned char _temp100[ 5u]="NULL"; static struct
_tagged_arr Cyc_Tcgenrep_null_str={ _temp100, _temp100, _temp100 +  5u}; static
struct Cyc_Absyn_Exp* Cyc_Tcgenrep_list_exp( struct Cyc_List_List* l, struct Cyc_Position_Segment*
loc){ if( l ==  0){ return Cyc_Absyn_null_exp( loc);} return Cyc_Tcgenrep_call_exp(
Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_list_str), l, loc);} static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_make_sizeoftyp_exp( struct Cyc_Position_Segment* loc, void* type){
return Cyc_Absyn_sizeoftyp_exp( type, loc);} struct _tuple7{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; static struct _tuple7* Cyc_Tcgenrep_tri_sum_exps(
struct Cyc_Position_Segment* loc, struct _tuple7* carry, struct Cyc_Absyn_Exp* e){
struct _tuple7 _temp103; struct Cyc_Absyn_Exp* _temp104; struct Cyc_List_List*
_temp106; struct _tuple7* _temp101= carry; _temp103=* _temp101; _LL107: _temp106=
_temp103.f1; goto _LL105; _LL105: _temp104= _temp103.f2; goto _LL102; _LL102: {
struct Cyc_List_List* _temp108=({ struct Cyc_List_List* _temp110=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp110->hd=( void*) _temp104;
_temp110->tl= _temp106; _temp110;}); return({ struct _tuple7* _temp109=( struct
_tuple7*) _cycalloc( sizeof( struct _tuple7)); _temp109->f1= _temp108; _temp109->f2=
Cyc_Absyn_add_exp( e, _temp104, loc); _temp109;});}} static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_get_and_cast_ri_exp( struct Cyc_Position_Segment* loc, struct Cyc_Tcgenrep_RepInfo*
info){ return Cyc_Absyn_cast_exp( Cyc_Tcgenrep_tunion_typ(& Cyc_Tcgenrep_typestruct_str),(
struct Cyc_Absyn_Exp*) _check_null( info->exp), loc);} static struct _tuple5*
Cyc_Tcgenrep_gen_id_for_exp( struct Cyc_Absyn_Exp* e){ return({ struct _tuple5*
_temp111=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp111->f1=
Cyc_Tcgenrep_new_gen_id( _tag_arr("tuple", sizeof( unsigned char), 6u));
_temp111->f2= e; _temp111;});} struct _tuple8{ void* f1; struct Cyc_Position_Segment*
f2; } ; static struct Cyc_Absyn_Decl* Cyc_Tcgenrep_gen_decl_cls( struct _tuple8*
env, struct _tuple5* name_exp){ struct _tuple8 _temp114; struct Cyc_Position_Segment*
_temp115; void* _temp117; struct _tuple8* _temp112= env; _temp114=* _temp112;
_LL118: _temp117= _temp114.f1; goto _LL116; _LL116: _temp115= _temp114.f2; goto
_LL113; _LL113: { struct _tuple5 _temp121; struct Cyc_Absyn_Exp* _temp122;
struct _tagged_arr* _temp124; struct _tuple5* _temp119= name_exp; _temp121=*
_temp119; _LL125: _temp124= _temp121.f1; goto _LL123; _LL123: _temp122= _temp121.f2;
goto _LL120; _LL120: return Cyc_Tcgenrep_gen_decl( _temp124, _temp117,( struct
Cyc_Absyn_Exp*) _temp122, _temp115);}} static struct _tuple7* Cyc_Tcgenrep_null_designator_exp(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple7* _temp126=( struct _tuple7*)
_cycalloc( sizeof( struct _tuple7)); _temp126->f1= 0; _temp126->f2= e; _temp126;});}
static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_arr_init_exp( struct Cyc_List_List* l,
struct Cyc_Position_Segment* loc){ struct Cyc_List_List* _temp127=(( struct Cyc_List_List*(*)(
struct _tuple7*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcgenrep_null_designator_exp, l); return Cyc_Absyn_unresolvedmem_exp( 0,
_temp127, loc);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_address_exp_cls(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_address_exp(
e, loc);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_unknownid_exp_cls( struct
Cyc_Position_Segment* loc, struct _tuple1* e){ return Cyc_Absyn_unknownid_exp( e,
loc);} static void* Cyc_Tcgenrep_select_structfield_type( struct Cyc_Absyn_Structfield*
sf){ if( Cyc_Std_strcmp(* sf->name, _tag_arr("", sizeof( unsigned char), 1u)) == 
0){({ void* _temp128[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): anonymous (padding) structfield not handled yet",
sizeof( unsigned char), 55u), _tag_arr( _temp128, sizeof( void*), 0u));});} if(
sf->width !=  0){({ void* _temp129[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): bitfields in structs not handled yet",
sizeof( unsigned char), 44u), _tag_arr( _temp129, sizeof( void*), 0u));});}
return( void*) sf->type;} static struct Cyc_Dict_Dict* Cyc_Tcgenrep_update_info(
struct Cyc_Dict_Dict* dict, void* type, struct Cyc_List_List* decls, struct Cyc_Absyn_Exp*
exp, struct Cyc_List_List* dependencies, struct Cyc_Core_Opt* fwd_decl){ struct
Cyc_Core_Opt* _temp130=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, void*
k)) Cyc_Dict_lookup_opt)( dict, type); if( _temp130 !=  0){({ struct Cyc_Std_String_pa_struct
_temp132; _temp132.tag= Cyc_Std_String_pa; _temp132.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
type);{ void* _temp131[ 1u]={& _temp132}; Cyc_Std_printf( _tag_arr("Updated ri for %s\n",
sizeof( unsigned char), 19u), _tag_arr( _temp131, sizeof( void*), 1u));}});((
struct Cyc_Tcgenrep_RepInfo*)(( struct Cyc_Core_Opt*) _check_null( _temp130))->v)->decls=
decls;(( struct Cyc_Tcgenrep_RepInfo*)(( struct Cyc_Core_Opt*) _check_null(
_temp130))->v)->exp= exp;(( struct Cyc_Tcgenrep_RepInfo*)(( struct Cyc_Core_Opt*)
_check_null( _temp130))->v)->dependencies= dependencies; return dict;} else{({
struct Cyc_Std_String_pa_struct _temp134; _temp134.tag= Cyc_Std_String_pa;
_temp134.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( type);{ void* _temp133[
1u]={& _temp134}; Cyc_Std_printf( _tag_arr("Added ri for %s\n", sizeof(
unsigned char), 17u), _tag_arr( _temp133, sizeof( void*), 1u));}}); return((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* k, struct Cyc_Tcgenrep_RepInfo*
v)) Cyc_Dict_insert)( dict, type,({ struct Cyc_Tcgenrep_RepInfo* _temp135=(
struct Cyc_Tcgenrep_RepInfo*) _cycalloc( sizeof( struct Cyc_Tcgenrep_RepInfo));
_temp135->decls= decls; _temp135->exp= exp; _temp135->dependencies= dependencies;
_temp135->fwd_decl= fwd_decl; _temp135->emitted= 0; _temp135;}));}} static
struct Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={ 0, 0, 0}; static struct _tuple4*
Cyc_Tcgenrep_tqual_type( struct Cyc_Absyn_Tqual* tq, void* type){ return({
struct _tuple4* _temp136=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp136->f1=*(( struct Cyc_Absyn_Tqual*) _check_null( tq)); _temp136->f2= type;
_temp136;});} static void* Cyc_Tcgenrep_tuple_typ( struct Cyc_List_List* types){
struct Cyc_List_List* _temp137=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Tqual*, void*), struct Cyc_Absyn_Tqual* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_tqual_type,( struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none,
types); struct Cyc_Absyn_TupleType_struct* tuple_type_base=({ struct Cyc_Absyn_TupleType_struct*
_temp138=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp138[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp139; _temp139.tag= Cyc_Absyn_TupleType;
_temp139.f1= _temp137; _temp139;}); _temp138;}); void* tuple_type=( void*)
tuple_type_base; return tuple_type;} static struct _tuple7* Cyc_Tcgenrep_array_decls(
void* type, struct Cyc_List_List* exps, struct Cyc_Position_Segment* loc){
struct Cyc_List_List* _temp140=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_gen_id_for_exp,
exps); struct Cyc_List_List* _temp141=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(*
f)( struct _tuple8*, struct _tuple5*), struct _tuple8* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_gen_decl_cls,({ struct _tuple8* _temp154=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp154->f1= type;
_temp154->f2= loc; _temp154;}), _temp140); struct Cyc_List_List* _temp142=((
struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcgenrep_get_first, _temp140); struct Cyc_List_List*
_temp143=(( struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tagged_arr*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_toplevel_name, _temp142);
struct Cyc_List_List* _temp144=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Position_Segment*, struct _tuple1*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_unknownid_exp_cls,
loc, _temp143); struct Cyc_List_List* _temp145=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct Cyc_Absyn_Exp*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_address_exp_cls,
loc, _temp144); struct Cyc_Absyn_Exp* _temp146= Cyc_Tcgenrep_arr_init_exp(
_temp145, loc); struct _tagged_arr* _temp147= Cyc_Tcgenrep_new_gen_id( _tag_arr("arr",
sizeof( unsigned char), 4u)); void* _temp148= Cyc_Absyn_at_typ( type,( void*)
Cyc_Absyn_HeapRgn, Cyc_Tcgenrep_tq_none); void* _temp149= Cyc_Tcgenrep_array_typ(
_temp148, Cyc_Tcgenrep_tq_none, 0); struct Cyc_Absyn_Decl* _temp150= Cyc_Tcgenrep_gen_decl(
_temp147, _temp149,( struct Cyc_Absyn_Exp*) _temp146, loc); struct Cyc_Absyn_Exp*
_temp151= Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( _temp147), loc);
return({ struct _tuple7* _temp152=( struct _tuple7*) _cycalloc( sizeof( struct
_tuple7)); _temp152->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp141,({ struct Cyc_Absyn_Decl*
_temp153[ 1u]; _temp153[ 0u]= _temp150;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp153, sizeof( struct Cyc_Absyn_Decl*),
1u));})); _temp152->f2= _temp151; _temp152;});} static void* Cyc_Tcgenrep_check_tunionfield_and_get_type(
struct Cyc_Absyn_Tunionfield* tuf){ if( tuf->tvs !=  0){({ void* _temp155[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen(): tunionfield type varaibles not handled", sizeof( unsigned char),
46u), _tag_arr( _temp155, sizeof( void*), 0u));});} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp156=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp156[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp157; _temp157.tag= Cyc_Absyn_TupleType;
_temp157.f1= tuf->typs; _temp157;}); _temp156;});} static void* Cyc_Tcgenrep_id(
void* x){ return x;} struct _tuple9{ struct Cyc_Dict_Dict* f1; struct Cyc_Tcgenrep_RepInfo*
f2; } ; static struct _tuple9* Cyc_Tcgenrep_lookupRep( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment* loc, void* type);
struct _tuple10{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List* f2; } ; struct
_tuple11{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Position_Segment* f2; } ; static
struct _tuple10* Cyc_Tcgenrep_lookupRepsCls( struct _tuple11* env, void* type,
struct _tuple10* carry){ struct _tuple9 _temp160; struct Cyc_Tcgenrep_RepInfo*
_temp161; struct Cyc_Dict_Dict* _temp163; struct _tuple9* _temp158= Cyc_Tcgenrep_lookupRep((*
env).f1,(* carry).f1,(* env).f2, type); _temp160=* _temp158; _LL164: _temp163=
_temp160.f1; goto _LL162; _LL162: _temp161= _temp160.f2; goto _LL159; _LL159:
return({ struct _tuple10* _temp165=( struct _tuple10*) _cycalloc( sizeof( struct
_tuple10)); _temp165->f1= _temp163; _temp165->f2=({ struct Cyc_List_List*
_temp166=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp166->hd=( void*) _temp161; _temp166->tl=(* carry).f2; _temp166;}); _temp165;});}
static struct Cyc_Dict_Dict* Cyc_Tcgenrep_buildRepTuple( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment* loc, struct
_tagged_arr* varname, void* type, struct Cyc_List_List* types){ struct Cyc_Absyn_Exp*
_temp167= Cyc_Absyn_sizeoftyp_exp( type, loc); struct _tuple11* _temp168=({
struct _tuple11* _temp199=( struct _tuple11*) _cycalloc( sizeof( struct _tuple11));
_temp199->f1= te; _temp199->f2= loc; _temp199;}); struct _tuple10* _temp169=({
struct _tuple10* _temp198=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10));
_temp198->f1= dict; _temp198->f2= 0; _temp198;}); struct _tuple10 _temp172;
struct Cyc_List_List* _temp173; struct Cyc_Dict_Dict* _temp175; struct _tuple10*
_temp170=(( struct _tuple10*(*)( struct _tuple10*(* f)( struct _tuple11*, void*,
struct _tuple10*), struct _tuple11*, struct Cyc_List_List* x, struct _tuple10*
accum)) Cyc_List_fold_right_c)( Cyc_Tcgenrep_lookupRepsCls, _temp168, types,
_temp169); _temp172=* _temp170; _LL176: _temp175= _temp172.f1; goto _LL174;
_LL174: _temp173= _temp172.f2; goto _LL171; _LL171: dict= _temp175;{ struct Cyc_List_List*
_temp177=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcgenrep_make_sizeoftyp_exp, loc, types); struct _tuple7 _temp181; struct
Cyc_List_List* _temp182; struct _tuple7* _temp179=(( struct _tuple7*(*)( struct
_tuple7*(* f)( struct Cyc_Position_Segment*, struct _tuple7*, struct Cyc_Absyn_Exp*),
struct Cyc_Position_Segment*, struct _tuple7* accum, struct Cyc_List_List* x))
Cyc_List_fold_left_c)( Cyc_Tcgenrep_tri_sum_exps, loc,({ struct _tuple7*
_temp178=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7)); _temp178->f1= 0;
_temp178->f2= Cyc_Tcgenrep_cnst_int( 0, loc); _temp178;}), _temp177); _temp181=*
_temp179; _LL183: _temp182= _temp181.f1; goto _LL180; _LL180: { struct Cyc_List_List*
_temp184=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp182); struct Cyc_List_List* _temp185=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_get_and_cast_ri_exp,
loc, _temp173); struct Cyc_List_List* _temp186=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct _tuple6*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_tuple2_exp_cls,
loc,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp184, _temp185)); void* tuple_type= Cyc_Tcgenrep_tuple_typ(({
void* _temp197[ 2u]; _temp197[ 1u]= Cyc_Tcgenrep_tunion_typ(& Cyc_Tcgenrep_typestruct_str);
_temp197[ 0u]= Cyc_Absyn_uint_t; Cyc_List_list( _tag_arr( _temp197, sizeof( void*),
2u));})); struct _tuple7 _temp189; struct Cyc_Absyn_Exp* _temp190; struct Cyc_List_List*
_temp192; struct _tuple7* _temp187= Cyc_Tcgenrep_array_decls( tuple_type,
_temp186, loc); _temp189=* _temp187; _LL193: _temp192= _temp189.f1; goto _LL191;
_LL191: _temp190= _temp189.f2; goto _LL188; _LL188: { struct Cyc_Absyn_Decl*
_temp194= Cyc_Tcgenrep_tunion_constructor_decl(& Cyc_Tcgenrep_typestruct_str,&
Cyc_Tcgenrep_tuple_str, varname,({ struct Cyc_Absyn_Exp* _temp196[ 2u]; _temp196[
1u]= _temp190; _temp196[ 0u]= _temp167;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp196, sizeof( struct Cyc_Absyn_Exp*),
2u));}), loc); return Cyc_Tcgenrep_update_info( dict, type,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp192,({ struct Cyc_Absyn_Decl* _temp195[ 1u]; _temp195[ 0u]= _temp194;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp195, sizeof( struct Cyc_Absyn_Decl*), 1u));})),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( varname), loc), loc),
_temp173, 0);}}}} static struct Cyc_Dict_Dict* Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment*
loc, struct _tagged_arr* varname, void* type, struct Cyc_List_List* types){
struct _tuple11* _temp200=({ struct _tuple11* _temp221=( struct _tuple11*)
_cycalloc( sizeof( struct _tuple11)); _temp221->f1= te; _temp221->f2= loc;
_temp221;}); struct _tuple10* _temp201=({ struct _tuple10* _temp220=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp220->f1= dict; _temp220->f2=
0; _temp220;}); struct _tuple10 _temp204; struct Cyc_List_List* _temp205; struct
Cyc_Dict_Dict* _temp207; struct _tuple10* _temp202=(( struct _tuple10*(*)(
struct _tuple10*(* f)( struct _tuple11*, void*, struct _tuple10*), struct
_tuple11*, struct Cyc_List_List* x, struct _tuple10* accum)) Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls, _temp200, types, _temp201); _temp204=* _temp202;
_LL208: _temp207= _temp204.f1; goto _LL206; _LL206: _temp205= _temp204.f2; goto
_LL203; _LL203: dict= _temp207;{ struct Cyc_List_List* _temp209=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_get_and_cast_ri_exp,
loc, _temp205); struct _tuple7 _temp212; struct Cyc_Absyn_Exp* _temp213; struct
Cyc_List_List* _temp215; struct _tuple7* _temp210= Cyc_Tcgenrep_array_decls( Cyc_Tcgenrep_tunion_typ(&
Cyc_Tcgenrep_typestruct_str), _temp209, loc); _temp212=* _temp210; _LL216:
_temp215= _temp212.f1; goto _LL214; _LL214: _temp213= _temp212.f2; goto _LL211;
_LL211: { struct Cyc_Absyn_Decl* _temp217= Cyc_Tcgenrep_tunion_constructor_decl(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_union_str, varname,({ struct Cyc_Absyn_Exp*
_temp219[ 1u]; _temp219[ 0u]= _temp213;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp219, sizeof( struct Cyc_Absyn_Exp*),
1u));}), loc); return Cyc_Tcgenrep_update_info( dict, type,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp215,({ struct Cyc_Absyn_Decl* _temp218[ 1u]; _temp218[ 0u]= _temp217;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp218, sizeof( struct Cyc_Absyn_Decl*), 1u));})),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( varname), loc), loc),
_temp205, 0);}}} static struct Cyc_Dict_Dict* Cyc_Tcgenrep_buildRepTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment*
loc, struct _tagged_arr* varname, void* type, struct Cyc_List_List* types){
struct _tuple11* _temp222=({ struct _tuple11* _temp248=( struct _tuple11*)
_cycalloc( sizeof( struct _tuple11)); _temp248->f1= te; _temp248->f2= loc;
_temp248;}); struct _tuple10* _temp223=({ struct _tuple10* _temp247=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp247->f1= dict; _temp247->f2=
0; _temp247;}); struct _tuple10 _temp226; struct Cyc_List_List* _temp227; struct
Cyc_Dict_Dict* _temp229; struct _tuple10* _temp224=(( struct _tuple10*(*)(
struct _tuple10*(* f)( struct _tuple11*, void*, struct _tuple10*), struct
_tuple11*, struct Cyc_List_List* x, struct _tuple10* accum)) Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls, _temp222, types, _temp223); _temp226=* _temp224;
_LL230: _temp229= _temp226.f1; goto _LL228; _LL228: _temp227= _temp226.f2; goto
_LL225; _LL225: dict= _temp229;{ struct Cyc_List_List* _temp231=(( struct Cyc_List_List*(*)(
int n, int(* f)( int))) Cyc_List_tabulate)((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp227),( int(*)( int x)) Cyc_Tcgenrep_id); struct Cyc_List_List*
_temp232=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
int), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_int_cls, loc, _temp231); struct Cyc_List_List* _temp233=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_get_and_cast_ri_exp, loc, _temp227); struct
Cyc_List_List* _temp234=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp232, _temp233); struct Cyc_List_List*
_temp235=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
struct _tuple6*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcgenrep_tuple2_exp_cls, loc, _temp234); void* tuple_type=
Cyc_Tcgenrep_tuple_typ(({ void* _temp246[ 2u]; _temp246[ 1u]= Cyc_Tcgenrep_tunion_typ(&
Cyc_Tcgenrep_typestruct_str); _temp246[ 0u]= Cyc_Absyn_uint_t; Cyc_List_list(
_tag_arr( _temp246, sizeof( void*), 2u));})); struct _tuple7 _temp238; struct
Cyc_Absyn_Exp* _temp239; struct Cyc_List_List* _temp241; struct _tuple7*
_temp236= Cyc_Tcgenrep_array_decls( tuple_type, _temp235, loc); _temp238=*
_temp236; _LL242: _temp241= _temp238.f1; goto _LL240; _LL240: _temp239= _temp238.f2;
goto _LL237; _LL237: { struct Cyc_Absyn_Decl* _temp243= Cyc_Tcgenrep_tunion_constructor_decl(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_tunion_str, varname,({ struct Cyc_Absyn_Exp*
_temp245[ 1u]; _temp245[ 0u]= _temp239;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp245, sizeof( struct Cyc_Absyn_Exp*),
1u));}), loc); return Cyc_Tcgenrep_update_info( dict, type,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp241,({ struct Cyc_Absyn_Decl* _temp244[ 1u]; _temp244[ 0u]= _temp243;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp244, sizeof( struct Cyc_Absyn_Decl*), 1u));})),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( varname), loc), loc),
_temp227, 0); return dict;}}} struct _tuple12{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; } ; static struct _tuple9* Cyc_Tcgenrep_lookupRep( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment* loc, void* type){
if( Cyc_Tcgenrep_typestruct_decl ==  0){ Cyc_Tcenv_lookup_tuniondecl( te, loc,&
Cyc_Tcgenrep_typestruct_name);}{ struct Cyc_Core_Opt* info=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, void* k)) Cyc_Dict_lookup_opt)( dict, type); if( info
!=  0){ struct _tuple9* _temp249=({ struct _tuple9* _temp250=( struct _tuple9*)
_cycalloc( sizeof( struct _tuple9)); _temp250->f1= dict; _temp250->f2=( struct
Cyc_Tcgenrep_RepInfo*)(( struct Cyc_Core_Opt*) _check_null( info))->v; _temp250;});
return _temp249;} else{{ void* _temp251= type; void* _temp299; struct Cyc_Absyn_PtrInfo
_temp301; struct Cyc_Absyn_Exp* _temp303; struct Cyc_Absyn_Tqual _temp305; void*
_temp307; struct Cyc_List_List* _temp309; struct Cyc_Core_Opt* _temp311; struct
Cyc_List_List* _temp313; struct _tuple1* _temp315; struct Cyc_Absyn_Structdecl**
_temp317; struct Cyc_List_List* _temp319; struct _tuple1* _temp321; struct Cyc_List_List*
_temp323; struct Cyc_Absyn_Tvar* _temp325; struct Cyc_Absyn_FnInfo _temp327;
struct Cyc_Absyn_TunionInfo _temp329; struct Cyc_Absyn_TunionFieldInfo _temp331;
struct Cyc_Absyn_Uniondecl** _temp333; struct Cyc_List_List* _temp335; struct
_tuple1* _temp337; struct Cyc_List_List* _temp339; _LL253: if(( unsigned int)
_temp251 >  4u?*(( int*) _temp251) ==  Cyc_Absyn_IntType: 0){ _LL300: _temp299=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp251)->f2; goto _LL254;} else{
goto _LL255;} _LL255: if( _temp251 == ( void*) Cyc_Absyn_FloatType){ goto _LL256;}
else{ goto _LL257;} _LL257: if( _temp251 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL258;} else{ goto _LL259;} _LL259: if(( unsigned int) _temp251 >  4u?*(( int*)
_temp251) ==  Cyc_Absyn_PointerType: 0){ _LL302: _temp301=(( struct Cyc_Absyn_PointerType_struct*)
_temp251)->f1; goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int)
_temp251 >  4u?*(( int*) _temp251) ==  Cyc_Absyn_ArrayType: 0){ _LL308: _temp307=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp251)->f1; goto _LL306; _LL306:
_temp305=(( struct Cyc_Absyn_ArrayType_struct*) _temp251)->f2; goto _LL304;
_LL304: _temp303=(( struct Cyc_Absyn_ArrayType_struct*) _temp251)->f3; goto
_LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp251 >  4u?*(( int*)
_temp251) ==  Cyc_Absyn_TupleType: 0){ _LL310: _temp309=(( struct Cyc_Absyn_TupleType_struct*)
_temp251)->f1; goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int)
_temp251 >  4u?*(( int*) _temp251) ==  Cyc_Absyn_TypedefType: 0){ _LL316:
_temp315=(( struct Cyc_Absyn_TypedefType_struct*) _temp251)->f1; goto _LL314;
_LL314: _temp313=(( struct Cyc_Absyn_TypedefType_struct*) _temp251)->f2; goto
_LL312; _LL312: _temp311=(( struct Cyc_Absyn_TypedefType_struct*) _temp251)->f3;
goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int) _temp251 >  4u?*((
int*) _temp251) ==  Cyc_Absyn_StructType: 0){ _LL322: _temp321=(( struct Cyc_Absyn_StructType_struct*)
_temp251)->f1; goto _LL320; _LL320: _temp319=(( struct Cyc_Absyn_StructType_struct*)
_temp251)->f2; goto _LL318; _LL318: _temp317=(( struct Cyc_Absyn_StructType_struct*)
_temp251)->f3; goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int)
_temp251 >  4u?*(( int*) _temp251) ==  Cyc_Absyn_AnonStructType: 0){ _LL324:
_temp323=(( struct Cyc_Absyn_AnonStructType_struct*) _temp251)->f1; goto _LL270;}
else{ goto _LL271;} _LL271: if( _temp251 == ( void*) Cyc_Absyn_VoidType){ goto
_LL272;} else{ goto _LL273;} _LL273: if(( unsigned int) _temp251 >  4u?*(( int*)
_temp251) ==  Cyc_Absyn_Evar: 0){ goto _LL274;} else{ goto _LL275;} _LL275: if((
unsigned int) _temp251 >  4u?*(( int*) _temp251) ==  Cyc_Absyn_VarType: 0){
_LL326: _temp325=(( struct Cyc_Absyn_VarType_struct*) _temp251)->f1; goto _LL276;}
else{ goto _LL277;} _LL277: if(( unsigned int) _temp251 >  4u?*(( int*) _temp251)
==  Cyc_Absyn_FnType: 0){ _LL328: _temp327=(( struct Cyc_Absyn_FnType_struct*)
_temp251)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int)
_temp251 >  4u?*(( int*) _temp251) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL280;}
else{ goto _LL281;} _LL281: if( _temp251 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL282;} else{ goto _LL283;} _LL283: if(( unsigned int) _temp251 >  4u?*(( int*)
_temp251) ==  Cyc_Absyn_AccessEff: 0){ goto _LL284;} else{ goto _LL285;} _LL285:
if(( unsigned int) _temp251 >  4u?*(( int*) _temp251) ==  Cyc_Absyn_JoinEff: 0){
goto _LL286;} else{ goto _LL287;} _LL287: if(( unsigned int) _temp251 >  4u?*((
int*) _temp251) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL288;} else{ goto _LL289;}
_LL289: if(( unsigned int) _temp251 >  4u?*(( int*) _temp251) ==  Cyc_Absyn_TunionType:
0){ _LL330: _temp329=(( struct Cyc_Absyn_TunionType_struct*) _temp251)->f1; goto
_LL290;} else{ goto _LL291;} _LL291: if(( unsigned int) _temp251 >  4u?*(( int*)
_temp251) ==  Cyc_Absyn_TunionFieldType: 0){ _LL332: _temp331=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp251)->f1; goto _LL292;} else{ goto _LL293;} _LL293: if(( unsigned int)
_temp251 >  4u?*(( int*) _temp251) ==  Cyc_Absyn_UnionType: 0){ _LL338: _temp337=((
struct Cyc_Absyn_UnionType_struct*) _temp251)->f1; goto _LL336; _LL336: _temp335=((
struct Cyc_Absyn_UnionType_struct*) _temp251)->f2; goto _LL334; _LL334: _temp333=((
struct Cyc_Absyn_UnionType_struct*) _temp251)->f3; goto _LL294;} else{ goto
_LL295;} _LL295: if(( unsigned int) _temp251 >  4u?*(( int*) _temp251) ==  Cyc_Absyn_AnonUnionType:
0){ _LL340: _temp339=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp251)->f1;
goto _LL296;} else{ goto _LL297;} _LL297: if(( unsigned int) _temp251 >  4u?*((
int*) _temp251) ==  Cyc_Absyn_EnumType: 0){ goto _LL298;} else{ goto _LL252;}
_LL254: { struct _tagged_arr* _temp341= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep",
sizeof( unsigned char), 4u)); struct Cyc_Absyn_Decl* _temp342= Cyc_Tcgenrep_tunion_constructor_decl(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_int_str, _temp341,({ struct Cyc_Absyn_Exp*
_temp344[ 1u]; _temp344[ 0u]= Cyc_Tcgenrep_cnst_int( Cyc_Tcgenrep_size_of2int(
_temp299), loc);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp344, sizeof( struct Cyc_Absyn_Exp*), 1u));}), loc); dict= Cyc_Tcgenrep_update_info(
dict, type,({ struct Cyc_Absyn_Decl* _temp343[ 1u]; _temp343[ 0u]= _temp342;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp343, sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( _temp341), loc), loc), 0, 0);
goto _LL252;} _LL256: dict= Cyc_Tcgenrep_update_info( dict, type, 0,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_float_str),
loc), 0, 0); goto _LL252; _LL258: dict= Cyc_Tcgenrep_update_info( dict, type, 0,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_double_str),
loc), 0, 0); goto _LL252; _LL260: { struct _tuple9 _temp347; struct Cyc_Tcgenrep_RepInfo*
_temp348; struct Cyc_Dict_Dict* _temp350; struct _tuple9* _temp345= Cyc_Tcgenrep_lookupRep(
te, dict, loc,( void*) _temp301.elt_typ); _temp347=* _temp345; _LL351: _temp350=
_temp347.f1; goto _LL349; _LL349: _temp348= _temp347.f2; goto _LL346; _LL346:
dict= _temp350;{ void* _temp352= Cyc_Absyn_conref_val( _temp301.bounds); struct
Cyc_Absyn_Exp* _temp358; _LL354: if(( unsigned int) _temp352 >  1u?*(( int*)
_temp352) ==  Cyc_Absyn_Upper_b: 0){ _LL359: _temp358=(( struct Cyc_Absyn_Upper_b_struct*)
_temp352)->f1; goto _LL355;} else{ goto _LL356;} _LL356: if( _temp352 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL357;} else{ goto _LL353;} _LL355: { unsigned int
bound= Cyc_Evexp_eval_const_uint_exp( _temp358); struct _tagged_arr* _temp360=
Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof( unsigned char), 4u)); struct
Cyc_Absyn_Decl* _temp361= Cyc_Tcgenrep_tunion_constructor_decl(& Cyc_Tcgenrep_typestruct_str,&
Cyc_Tcgenrep_thinptr_str, _temp360,({ struct Cyc_Absyn_Exp* _temp364[ 2u];
_temp364[ 1u]=( struct Cyc_Absyn_Exp*) _check_null( _temp348->exp); _temp364[ 0u]=
Cyc_Tcgenrep_cnst_int(( int) bound, loc);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp364, sizeof( struct Cyc_Absyn_Exp*),
2u));}), loc); dict= Cyc_Tcgenrep_update_info( dict, type,({ struct Cyc_Absyn_Decl*
_temp362[ 1u]; _temp362[ 0u]= _temp361;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp362, sizeof( struct Cyc_Absyn_Decl*),
1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name( _temp360), loc), loc),({ struct Cyc_Tcgenrep_RepInfo*
_temp363[ 1u]; _temp363[ 0u]= _temp348;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp363, sizeof( struct Cyc_Tcgenrep_RepInfo*),
1u));}), 0); goto _LL353;} _LL357: { struct _tagged_arr* _temp365= Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u)); struct Cyc_Absyn_Decl* _temp366=
Cyc_Tcgenrep_tunion_constructor_decl(& Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_fatptr_str,
_temp365,({ struct Cyc_Absyn_Exp* _temp369[ 1u]; _temp369[ 0u]=( struct Cyc_Absyn_Exp*)
_check_null( _temp348->exp);(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp369, sizeof( struct Cyc_Absyn_Exp*), 1u));}), loc);
dict= Cyc_Tcgenrep_update_info( dict, type,({ struct Cyc_Absyn_Decl* _temp367[ 1u];
_temp367[ 0u]= _temp366;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp367, sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(
_temp365), loc), loc),({ struct Cyc_Tcgenrep_RepInfo* _temp368[ 1u]; _temp368[ 0u]=
_temp348;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp368, sizeof( struct Cyc_Tcgenrep_RepInfo*), 1u));}), 0); goto
_LL353;} _LL353:;} goto _LL252;} _LL262: if( _temp303 ==  0){({ void* _temp370[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("tcgenrep:At this point, array bounds must be constants", sizeof(
unsigned char), 55u), _tag_arr( _temp370, sizeof( void*), 0u));});}{ struct
_tuple9 _temp373; struct Cyc_Tcgenrep_RepInfo* _temp374; struct Cyc_Dict_Dict*
_temp376; struct _tuple9* _temp371= Cyc_Tcgenrep_lookupRep( te, dict, loc,
_temp307); _temp373=* _temp371; _LL377: _temp376= _temp373.f1; goto _LL375;
_LL375: _temp374= _temp373.f2; goto _LL372; _LL372: dict= _temp376;{
unsigned int bound= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp303)); struct _tagged_arr* _temp378= Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u)); struct Cyc_Absyn_Decl* _temp379=
Cyc_Tcgenrep_tunion_constructor_decl(& Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_thinptr_str,
_temp378,({ struct Cyc_Absyn_Exp* _temp382[ 2u]; _temp382[ 1u]=( struct Cyc_Absyn_Exp*)
_check_null( _temp374->exp); _temp382[ 0u]= Cyc_Tcgenrep_cnst_int(( int) bound,
loc);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp382, sizeof( struct Cyc_Absyn_Exp*), 2u));}), loc); dict= Cyc_Tcgenrep_update_info(
dict, type,({ struct Cyc_Absyn_Decl* _temp380[ 1u]; _temp380[ 0u]= _temp379;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp380, sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( _temp378), loc), loc),({
struct Cyc_Tcgenrep_RepInfo* _temp381[ 1u]; _temp381[ 0u]= _temp374;(( struct
Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp381,
sizeof( struct Cyc_Tcgenrep_RepInfo*), 1u));}), 0); goto _LL252;}} _LL264: {
struct Cyc_List_List* _temp383=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_get_second,
_temp309); struct _tagged_arr* _temp384= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep",
sizeof( unsigned char), 4u)); dict= Cyc_Tcgenrep_buildRepTuple( te, dict, loc,
_temp384, type, _temp383); goto _LL252;} _LL266: if( _temp313 !=  0){({ void*
_temp385[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen(): typedefs with parameters not yet supported", sizeof(
unsigned char), 50u), _tag_arr( _temp385, sizeof( void*), 0u));}); Cyc_Tcgenrep_print_params(
_temp313);} if( _temp311 ==  0){({ void* _temp386[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): typedef definition must be present by now",
sizeof( unsigned char), 49u), _tag_arr( _temp386, sizeof( void*), 0u));});}{
struct _tuple9 _temp389; struct Cyc_Tcgenrep_RepInfo* _temp390; struct Cyc_Dict_Dict*
_temp392; struct _tuple9* _temp387= Cyc_Tcgenrep_lookupRep( te, dict, loc,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp311))->v); _temp389=* _temp387; _LL393:
_temp392= _temp389.f1; goto _LL391; _LL391: _temp390= _temp389.f2; goto _LL388;
_LL388: dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* k,
struct Cyc_Tcgenrep_RepInfo* v)) Cyc_Dict_insert)( _temp392, type, _temp390);
goto _LL252;} _LL268: if( _temp319 !=  0){({ void* _temp394[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): structs with parameters not yet supported",
sizeof( unsigned char), 49u), _tag_arr( _temp394, sizeof( void*), 0u));}); Cyc_Tcgenrep_print_params(
_temp319);} if( _temp317 ==  0){({ void* _temp395[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): struct declaration must be present by now",
sizeof( unsigned char), 49u), _tag_arr( _temp395, sizeof( void*), 0u));});}{
struct _tagged_arr* _temp396= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); struct Cyc_Core_Opt* _temp397=({ struct Cyc_Core_Opt*
_temp432=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp432->v=( void*) Cyc_Tcgenrep_gen_vardecl_sc( _temp396, Cyc_Tcgenrep_tunionfield_typ(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_tuple_str), 0,( void*) Cyc_Absyn_Extern,
loc); _temp432;}); dict= Cyc_Tcgenrep_update_info( dict, type, 0,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(
_temp396), loc), loc), 0, _temp397);{ struct Cyc_Absyn_Structdecl* _temp398=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp317)); if( _temp398->tvs !=  0){({
void* _temp399[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("gen(): structs with tyvariables not yet supported",
sizeof( unsigned char), 50u), _tag_arr( _temp399, sizeof( void*), 0u));}); Cyc_Tcgenrep_print_tvars(
_temp398->tvs);}{ struct Cyc_List_List* _temp400= _temp398->tvs; if( Cyc_List_length(
_temp319) != (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp400)){({
void* _temp401[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("gen(): number of params differs from number of tyvars",
sizeof( unsigned char), 54u), _tag_arr( _temp401, sizeof( void*), 0u));});}{
struct Cyc_List_List* _temp405; struct Cyc_List_List* _temp407; struct _tuple12
_temp403=({ struct _tuple12 _temp402; _temp402.f1= _temp400; _temp402.f2=
_temp319; _temp402;}); _LL408: _temp407= _temp403.f1; goto _LL406; _LL406:
_temp405= _temp403.f2; goto _LL404; _LL404: { struct Cyc_List_List* r= 0; for( 0;
_temp407 !=  0;( _temp407=(( struct Cyc_List_List*) _check_null( _temp407))->tl,
_temp405=(( struct Cyc_List_List*) _check_null( _temp405))->tl)){ void* _temp409=
Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( _temp405))->hd);
_LL411: if( _temp409 == ( void*) Cyc_Absyn_AnyKind){ goto _LL412;} else{ goto
_LL413;} _LL413: if( _temp409 == ( void*) Cyc_Absyn_MemKind){ goto _LL414;}
else{ goto _LL415;} _LL415: if( _temp409 == ( void*) Cyc_Absyn_BoxKind){ goto
_LL416;} else{ goto _LL417;} _LL417: goto _LL418; _LL412: goto _LL414; _LL414:
goto _LL416; _LL416: { struct _tuple9 _temp421; struct Cyc_Tcgenrep_RepInfo*
_temp422; struct Cyc_Dict_Dict* _temp424; struct _tuple9* _temp419= Cyc_Tcgenrep_lookupRep(
te, dict, loc,( void*)(( struct Cyc_List_List*) _check_null( _temp405))->hd);
_temp421=* _temp419; _LL425: _temp424= _temp421.f1; goto _LL423; _LL423:
_temp422= _temp421.f2; goto _LL420; _LL420: dict= _temp424;{ struct Cyc_Absyn_VarType_struct*
_temp426=({ struct Cyc_Absyn_VarType_struct* _temp428=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp428[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp429; _temp429.tag= Cyc_Absyn_VarType; _temp429.f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp400))->hd; _temp429;}); _temp428;}); r=({
struct Cyc_List_List* _temp427=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp427->hd=( void*)(( void*) _temp426); _temp427->tl=
r; _temp427;}); dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void*
k, struct Cyc_Tcgenrep_RepInfo* v)) Cyc_Dict_insert)( dict,( void*) _temp426,
_temp422); goto _LL410;}} _LL418:; _LL410:;} if( _temp398->fields ==  0){({ void*
_temp430[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen(): struct fields must be present", sizeof( unsigned char), 37u),
_tag_arr( _temp430, sizeof( void*), 0u));});}{ struct Cyc_List_List* _temp431=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp398->fields))->v); dict= Cyc_Tcgenrep_buildRepTuple(
te, dict, loc, _temp396, type, _temp431);{ struct Cyc_List_List* rp= r; for( 0;
rp !=  0; rp=(( struct Cyc_List_List*) _check_null( rp))->tl){;}} goto _LL252;}}}}}}
_LL270: { struct Cyc_List_List* _temp433=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type,
_temp323); struct _tagged_arr* _temp434= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep",
sizeof( unsigned char), 4u)); dict= Cyc_Tcgenrep_buildRepTuple( te, dict, loc,
_temp434, type, _temp433); goto _LL252;} _LL272:({ void* _temp435[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("found void in gen() expression", sizeof( unsigned char), 31u),
_tag_arr( _temp435, sizeof( void*), 0u));}); goto _LL252; _LL274:({ void*
_temp436[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found evar in gen() expression",
sizeof( unsigned char), 31u), _tag_arr( _temp436, sizeof( void*), 0u));}); goto
_LL252; _LL276:({ void* _temp437[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found tyvar in gen() expression",
sizeof( unsigned char), 32u), _tag_arr( _temp437, sizeof( void*), 0u));}); goto
_LL252;({ void* _temp438[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found variable type in gen() expression",
sizeof( unsigned char), 40u), _tag_arr( _temp438, sizeof( void*), 0u));}); goto
_LL252; _LL278:({ void* _temp439[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found function type in gen() expression",
sizeof( unsigned char), 40u), _tag_arr( _temp439, sizeof( void*), 0u));}); goto
_LL252; _LL280: goto _LL282; _LL282: goto _LL284; _LL284: goto _LL286; _LL286:
goto _LL288; _LL288:({ void* _temp440[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("gen(): unhandled region, handle or effect type",
sizeof( unsigned char), 47u), _tag_arr( _temp440, sizeof( void*), 0u));}); goto
_LL252; _LL290: if( _temp329.targs !=  0){({ void* _temp441[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunion type parameters not handled yet",
sizeof( unsigned char), 46u), _tag_arr( _temp441, sizeof( void*), 0u));});}{
void* _temp442=( void*) _temp329.tunion_info; struct Cyc_Absyn_Tuniondecl*
_temp448; _LL444: if(*(( int*) _temp442) ==  Cyc_Absyn_KnownTunion){ _LL449:
_temp448=(( struct Cyc_Absyn_KnownTunion_struct*) _temp442)->f1; goto _LL445;}
else{ goto _LL446;} _LL446: if(*(( int*) _temp442) ==  Cyc_Absyn_UnknownTunion){
goto _LL447;} else{ goto _LL443;} _LL445: if( _temp448->tvs !=  0){({ void*
_temp450[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen(): tunion type variables not handled yet", sizeof( unsigned char),
45u), _tag_arr( _temp450, sizeof( void*), 0u));});} if( _temp448->fields ==  0){({
void* _temp451[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("gen(): tunion fields not present", sizeof(
unsigned char), 33u), _tag_arr( _temp451, sizeof( void*), 0u));});}{ struct
_tagged_arr* _temp452= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); struct Cyc_Core_Opt* _temp453=({ struct Cyc_Core_Opt*
_temp456=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp456->v=( void*) Cyc_Tcgenrep_gen_vardecl_sc( _temp452, Cyc_Tcgenrep_tunionfield_typ(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_tunion_str), 0,( void*) Cyc_Absyn_Extern,
loc); _temp456;}); dict= Cyc_Tcgenrep_update_info( dict, type, 0,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(
_temp452), loc), loc), 0, _temp453);{ struct Cyc_List_List* _temp454=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp448->fields))->v;
struct Cyc_List_List* _temp455=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Tunionfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_check_tunionfield_and_get_type,
_temp454); dict= Cyc_Tcgenrep_buildRepTunion( te, dict, loc, _temp452, type,
_temp455); goto _LL443;}} _LL447:({ void* _temp457[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunion must be resolved by now",
sizeof( unsigned char), 38u), _tag_arr( _temp457, sizeof( void*), 0u));}); goto
_LL443; _LL443:;} goto _LL252; _LL292: if( _temp331.targs !=  0){({ void*
_temp458[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen(): tunionfield type parameters not handled yet", sizeof(
unsigned char), 51u), _tag_arr( _temp458, sizeof( void*), 0u));});}{ void*
_temp459=( void*) _temp331.field_info; struct Cyc_Absyn_Tunionfield* _temp465;
struct Cyc_Absyn_Tuniondecl* _temp467; _LL461: if(*(( int*) _temp459) ==  Cyc_Absyn_KnownTunionfield){
_LL468: _temp467=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp459)->f1;
goto _LL466; _LL466: _temp465=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp459)->f2; goto _LL462;} else{ goto _LL463;} _LL463: goto _LL464; _LL462:
if( _temp465->tvs !=  0){({ void* _temp469[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunionfield type parameters not handled yet",
sizeof( unsigned char), 51u), _tag_arr( _temp469, sizeof( void*), 0u));});}{
struct Cyc_List_List* _temp470=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_get_second,
_temp465->typs); struct Cyc_List_List* _temp471=({ struct Cyc_List_List*
_temp473=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp473->hd=( void*) Cyc_Absyn_uint_t; _temp473->tl= _temp470; _temp473;});
struct _tagged_arr* _temp472= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); dict= Cyc_Tcgenrep_buildRepTuple( te, dict, loc, _temp472,
type, _temp471); goto _LL460;} _LL464:({ void* _temp474[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunionfield must be known at this point",
sizeof( unsigned char), 47u), _tag_arr( _temp474, sizeof( void*), 0u));}); goto
_LL460; _LL460:;} goto _LL252; _LL294: if( _temp335 !=  0){({ void* _temp475[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen(): unions with parameters not yet supported", sizeof(
unsigned char), 48u), _tag_arr( _temp475, sizeof( void*), 0u));});} if( _temp333
==  0){({ void* _temp476[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): union declaration must be present by now",
sizeof( unsigned char), 48u), _tag_arr( _temp476, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Uniondecl* _temp477=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp333)); if( _temp477->tvs !=  0){({ void* _temp478[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen: structs with parameters not yet supported", sizeof( unsigned char),
47u), _tag_arr( _temp478, sizeof( void*), 0u));});} if( _temp477->fields ==  0){({
void* _temp479[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("gen(): struct fields must be present", sizeof(
unsigned char), 37u), _tag_arr( _temp479, sizeof( void*), 0u));});}{ struct Cyc_List_List*
_temp480=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp477->fields))->v);
struct _tagged_arr* _temp481= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); dict= Cyc_Tcgenrep_buildRepUnion( te, dict, loc, _temp481,
type, _temp480); goto _LL252;}} _LL296: { struct Cyc_List_List* _temp482=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type, _temp339); struct
_tagged_arr* _temp483= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); dict= Cyc_Tcgenrep_buildRepUnion( te, dict, loc, _temp483,
type, _temp482); goto _LL252;} _LL298: { struct _tagged_arr* _temp484= Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u)); struct Cyc_Absyn_Decl* _temp485=
Cyc_Tcgenrep_tunion_constructor_decl(& Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_int_str,
_temp484,({ struct Cyc_Absyn_Exp* _temp487[ 1u]; _temp487[ 0u]= Cyc_Tcgenrep_cnst_int(
32, loc);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp487, sizeof( struct Cyc_Absyn_Exp*), 1u));}), loc); dict= Cyc_Tcgenrep_update_info(
dict, type,({ struct Cyc_Absyn_Decl* _temp486[ 1u]; _temp486[ 0u]= _temp485;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp486, sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( _temp484), loc), loc), 0, 0);
goto _LL252;} _LL252:;} return({ struct _tuple9* _temp488=( struct _tuple9*)
_cycalloc( sizeof( struct _tuple9)); _temp488->f1= dict; _temp488->f2=(( struct
Cyc_Tcgenrep_RepInfo*(*)( struct Cyc_Dict_Dict* d, void* k)) Cyc_Dict_lookup)(
dict, type); _temp488;});}}} static int Cyc_Tcgenrep_not_emitted_filter( struct
Cyc_Tcgenrep_RepInfo* ri){ return ri->emitted ==  0;} static void Cyc_Tcgenrep_mark_emitted(
struct Cyc_Tcgenrep_RepInfo* ri){ ri->emitted= 1;} struct _tuple13{ struct Cyc_Dict_Dict*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple13 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, void* type, struct Cyc_Dict_Dict* dict){ Cyc_Tcutil_check_valid_toplevel_type(
loc, te, type);{ struct _tuple9 _temp491; struct Cyc_Tcgenrep_RepInfo* _temp492;
struct Cyc_Dict_Dict* _temp494; struct _tuple9* _temp489= Cyc_Tcgenrep_lookupRep(
te, dict, loc, type); _temp491=* _temp489; _LL495: _temp494= _temp491.f1; goto
_LL493; _LL493: _temp492= _temp491.f2; goto _LL490; _LL490: { struct Cyc_List_List*
_temp496= Cyc_Tcgenrep_dfs( _temp492);({ struct Cyc_Std_String_pa_struct
_temp498; _temp498.tag= Cyc_Std_String_pa; _temp498.f1=( struct _tagged_arr) Cyc_Absynpp_decllist2string(
_temp496);{ void* _temp497[ 1u]={& _temp498}; Cyc_Std_printf( _tag_arr("Emitting...\n%s\n",
sizeof( unsigned char), 16u), _tag_arr( _temp497, sizeof( void*), 1u));}});
return({ struct _tuple13 _temp499; _temp499.f1= _temp494; _temp499.f2= _temp496;
_temp499.f3=( struct Cyc_Absyn_Exp*) _check_null( _temp492->exp); _temp499;});}}}
